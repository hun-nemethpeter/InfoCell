#include "Colors.h"
#include "Solver.h"
#include <array>
#include <set>


namespace synth {

Logger* loggerPtr = nullptr;

const std::array<cells::Color, 10> cellColors = {
    cells::Color(0x00, 0x00, 0x00), /* black */
    cells::Color(0x00, 0x74, 0xD9), /* blue */
    cells::Color(0xFF, 0x41, 0x36), /* red */
    cells::Color(0x2E, 0xCC, 0x40), /* green */
    cells::Color(0xFF, 0xDC, 0x00), /* yellow */
    cells::Color(0xAA, 0xAA, 0xAA), /* grey */
    cells::Color(0xF0, 0x12, 0xBE), /* fuschia */
    cells::Color(0xFF, 0x85, 0x1B), /* orange */
    cells::Color(0x7F, 0xDB, 0xFF), /* teal */
    cells::Color(0x87, 0x0C, 0x25)  /* brown */
};

const cells::Color& color(arc::Colors c)
{
    return cellColors[(int)c];
}

const std::map<const cells::Color, int> cellColorsToColorId = {
    { cellColors[0], 0 },
    { cellColors[1], 1 },
    { cellColors[2], 2 },
    { cellColors[3], 3 },
    { cellColors[4], 4 },
    { cellColors[5], 5 },
    { cellColors[6], 6 },
    { cellColors[7], 7 },
    { cellColors[8], 8 },
    { cellColors[9], 9 }
};

Solver::Solver(Logger& logger, const ArcTask& arcTask) :
    logger(logger), m_arcTask(arcTask)
{
    solve();
}

enum class RotationDir
{
    Degree_0,   // 🡩
    Degree_45,  // 🡭
    Degree_90,  // 🡪
    Degree_135, // 🡮
    Degree_180, // 🡫
    Degree_225, // 🡯
    Degree_270, // 🡨
    Degree_315  // 🡬
};

class Pixel
{
public:
    Pixel(int x, int y) :
        x(x), y(y)
    { }

    bool operator<(const Pixel& rhs) const
    {
        return std::tie(x, y) < std::tie(rhs.x, rhs.y);
    }

    int x;
    int y;
};

class Vector
{
public:
    bool operator<(const Vector& rhs) const
    {
        return std::tie(x, y) < std::tie(rhs.x, rhs.y);
    }

    Vector operator+(const Vector& rhs) const
    {
        return Vector({ x + rhs.x, y + rhs.y });
    }

    Vector operator-(const Vector& rhs) const
    {
        return Vector({ x - rhs.x, y - rhs.y });
    }

    Vector& operator+=(const Vector& rhs)
    {
        this->x += rhs.x;
        this->y += rhs.y;

        return *this;
    }

    Vector& operator-=(const Vector& rhs)
    {
        this->x -= rhs.x;
        this->y -= rhs.y;

        return *this;
    }

    Vector rotate(RotationDir rotationDir) const
    {
        Vector ret(x, y);
        switch (rotationDir) {

        // 🡩🡩🡩🡩🡩🡩🡩🡩🡩🡩🡩🡩🡩🡩🡩
        case RotationDir::Degree_0:
            // Do nothing
            break;

        // 🡭🡭🡭🡭🡭🡭🡭🡭🡭🡭🡭🡭🡭🡭🡭
        case RotationDir::Degree_45:
            ret.x = x - y;
            ret.y = y + x;
            break;

        // 🡪🡪🡪🡪🡪🡪🡪🡪🡪🡪🡪🡪🡪🡪🡪
        case RotationDir::Degree_90:
            ret.x = -y;
            ret.y = x;
            break;

        // 🡮🡮🡮🡮🡮🡮🡮🡮🡮🡮🡮🡮🡮🡮🡮
        case RotationDir::Degree_135:
            ret.x = -x - y;
            ret.y = x - y;
            break;

        // 🡫🡫🡫🡫🡫🡫🡫🡫🡫🡫🡫🡫🡫🡫🡫
        case RotationDir::Degree_180:
            ret.x = -x;
            ret.y = -y;
            break;

        // 🡯🡯🡯🡯🡯🡯🡯🡯🡯🡯🡯🡯🡯🡯🡯
        case RotationDir::Degree_225:
            ret.x = -x + y;
            ret.y = -x - y;
            break;

        // 🡨🡨🡨🡨🡨🡨🡨🡨🡨🡨🡨🡨🡨🡨🡨
        case RotationDir::Degree_270:
            ret.x = y;
            ret.y = -x;
            break;

        // 🡬🡬🡬🡬🡬🡬🡬🡬🡬🡬🡬🡬🡬🡬🡬
        case RotationDir::Degree_315:
            ret.x = x + y;
            ret.y = -x + y;
            break;
        }

        return ret;
    }

    int x;
    int y;
};

class VectorShape
{
public:
    VectorShape(const std::vector<Vector>& vectors, const cells::Color& color, const Pixel& firstPixel) :
        m_vectors(vectors), m_color(color), m_firstPixel(firstPixel)
    {
    }
    VectorShape(std::vector<Vector>&& vectors, const cells::Color& color, const Pixel& firstPixel) :
        m_vectors(std::move(vectors)), m_color(color), m_firstPixel(firstPixel)
    {
    }

    VectorShape rotate(RotationDir rotationDir) const
    {
        std::vector<Vector> rotatedVectors;

        for (const Vector& vector : m_vectors) {
            Vector newVector = vector.rotate(rotationDir);
            rotatedVectors.push_back(newVector);
//            loggerPtr->log(DEBUG) << " rotate vector [" << vector.x << ", " << vector.y << "] => [" << newVector.x << ", " << newVector.y << "]";
        }

        return VectorShape(std::move(rotatedVectors), m_color, m_firstPixel);
    }

    VectorShape stretch(int horizontal, int vertical) const
    {
        if ((horizontal < 1 || vertical < 1) || (horizontal == 1 && vertical == 1)) {
            // Do nothing
            return VectorShape(*this);
        }
        std::vector<Vector> stretchVectors;
        stretchPixel(stretchVectors, horizontal, vertical);

        for (const Vector& vector : m_vectors) {
            stretchVectors.push_back({ -(horizontal - 1) + (vector.x * horizontal), -(vertical - 1) - vector.y * vertical });
            //loggerPtr->log(DEBUG) << " stretchVector [" << vector.x << ", " << vector.y << "] -> [" << stretchVectors.back().x << ", " << stretchVectors.back().y << "]";
            stretchPixel(stretchVectors, horizontal, vertical);
        }

        return VectorShape(std::move(stretchVectors), color(), firstPixel());
    }

private:
    static void stretchPixel(std::vector<Vector>& stretchVectors, int horizontal, int vertical)
    {
        bool onlyHorizontal = (vertical == 1);
        for (int i = 0; i < vertical || onlyHorizontal; ++i) {
            onlyHorizontal = false;
            for (int j = 1; j < horizontal; ++j) {
                stretchVectors.push_back({ 1, 0 });
            }
            if (i == vertical - 1)
                break;
            if (horizontal > 1) {
                stretchVectors.push_back({ -(horizontal - 1), 1 });
            } else {
                stretchVectors.push_back({ 0, 1 });
            }
        }
    }

public:
    VectorShape mirror(const Vector firsPixelDistance, RotationDir axisDir) const
    {
        std::vector<Vector> mirrorShape;

        switch (axisDir) {
        case RotationDir::Degree_0:
        case RotationDir::Degree_180:
            // y coordinate will be the same
            {
                int distance = firsPixelDistance.x;
                Pixel firstPixel(m_firstPixel.x + distance * 2, m_firstPixel.y);
                for (const Vector& vector : m_vectors) {
                    mirrorShape.push_back({ -vector.x, vector.y });
                }

                return VectorShape(std::move(mirrorShape), color(), firstPixel);
            }
            break;

        case RotationDir::Degree_45:
        case RotationDir::Degree_135:
        case RotationDir::Degree_225:
        case RotationDir::Degree_315: {
            Vector distanceVector = firsPixelDistance;
            if (axisDir == RotationDir::Degree_45 || axisDir == RotationDir::Degree_225) {
                distanceVector -= firsPixelDistance.rotate(RotationDir::Degree_90);
            } else {
                distanceVector += firsPixelDistance.rotate(RotationDir::Degree_90);
            }
            Pixel firstPixel(m_firstPixel.x + distanceVector.x, m_firstPixel.y + distanceVector.y);
            for (const Vector& vector : m_vectors) {
                mirrorShape.push_back({ -vector.x, -vector.y });
            }

            return VectorShape(std::move(mirrorShape), color(), firstPixel);
        } break;

        case RotationDir::Degree_90:
        case RotationDir::Degree_270:
            // x coordinate will be the same
            {
                int distance = firsPixelDistance.y;
                Pixel firstPixel(m_firstPixel.x, m_firstPixel.y + distance * 2);
                for (const Vector& vector : m_vectors) {
                    mirrorShape.push_back({ vector.x, -vector.y });
                }

                return VectorShape(std::move(mirrorShape), color(), firstPixel);
            }
            break;
        }

        return *this;
    }

    const cells::Color& color() const
    {
        return m_color;
    }

    const std::vector<Vector>& vectors() const
    {
        return m_vectors;
    }

    const Pixel& firstPixel() const
    {
        return m_firstPixel;
    }

    void firstPixel(const Pixel& pixel)
    {
        m_firstPixel = pixel;
    }

private:
    cells::Color m_color;
    std::vector<Vector> m_vectors;
    Pixel m_firstPixel;
};

class Patch;
class PatchBoardI
{
public:
    virtual void subscribePatchForPixel(std::shared_ptr<Patch> patch, int x, int y)       = 0;
    virtual void mergePatch(std::shared_ptr<Patch> winner, std::shared_ptr<Patch> looser) = 0;

    virtual int width() const  = 0;
    virtual int height() const = 0;
};

class Patch : public std::enable_shared_from_this<Patch>
{
public:
    Patch(cells::Color color, PatchBoardI* patchBoardI) :
        m_color(color), m_patchBoardI(patchBoardI), m_width(patchBoardI->width()), m_height(patchBoardI->height()) { }

    const cells::Color& color() const
    {
        return m_color;
    }

    size_t size() const
    {
        return m_pixels.size();
    }

    void addPixelCoordinate(int x, int y)
    {
        refreshBoundaries(x, y);
        m_pixels.push_back({ x, y });
        m_patchBoardI->subscribePatchForPixel(shared_from_this(), x, y);
    }

    void registerSubscribedPixel(int x, int y)
    {
        m_subscribedPixels.insert({ x, y });
    }

    void merge(std::shared_ptr<Patch> other)
    {
        for (const auto& pixel : other->m_pixels) {
            m_pixels.push_back(pixel);
        }
        sortPixels();
        m_patchBoardI->mergePatch(shared_from_this(), other);
    }

    void sortPixels()
    {
        int patchBoardWidth = m_patchBoardI->width();
        std::sort(m_pixels.begin(), m_pixels.end(), [patchBoardWidth](const Pixel& p1, const Pixel& p2) {
            return p1.y * patchBoardWidth + p1.x < p2.y * patchBoardWidth + p2.x;
        });
    }

    VectorShape vectorize() const
    {
        std::vector<Vector> vectors;

        const Pixel* prevPixel = &m_pixels.front();
        bool isFirstPixel        = true;

        for (const auto& pixel : m_pixels) {
            if (isFirstPixel) {
                isFirstPixel = false;
                continue;
            }
            const Pixel* currPixel = &pixel;
            vectors.push_back({ currPixel->x - prevPixel->x, currPixel->y - prevPixel->y });
            prevPixel = currPixel;
        }
        VectorShape vectorShape(std::move(vectors), color(), firstPixel());

        return vectorShape;
    }

    void refreshBoundaries(int x, int y)
    {
        if (x < m_leftX)
            m_leftX = x;
        if (x > m_rightX)
            m_rightX = x;
        if (y < m_topY)
            m_topY = y;
        if (y > m_bottomY)
            m_bottomY = y;
    }

    void assignId(int id)
    {
        m_id = id;
    }

    const Pixel& firstPixel() const
    {
        return m_pixels.front();
    }

    const Pixel& lastPixel() const
    {
        return m_pixels.back();
    }

    int pixelIndex(const Pixel& pixel) const
    {
        return pixel.y * m_width + pixel.x;
    }

    int firstPixelIndex() const
    {
        const Pixel& pixel = firstPixel();
        return pixel.y * m_width + pixel.x;
    }

    bool operator<(const Patch& rhs) const
    {
        return firstPixelIndex() < rhs.firstPixelIndex();
    }

    int patchWidth() const
    {
        return m_rightX - m_leftX;
    }

    int patchHeight() const
    {
        return m_bottomY - m_topY;
    }

    std::string toString() const
    {
        char boardColor = '0' + cellColorsToColorId.at(color());
        std::string board(m_width * m_height, '.');
        for (const Pixel& pixel : m_pixels) {
            board[pixel.y * m_width + pixel.x] = boardColor;
        }
        std::string ret;
        ret += std::to_string(m_width);
        ret += ' ';
        ret += std::to_string(m_height);
        ret += ' ';
        ret += board;

        return ret;
    }

    const std::set<Pixel>& subscribedPixels() const
    {
        return m_subscribedPixels;
    }

    int m_id = 0;

private:
    int m_leftX   = 0;
    int m_rightX  = 0;
    int m_topY    = 0;
    int m_bottomY = 0;

    cells::Color m_color;
    PatchBoardI* m_patchBoardI = nullptr;
    const int m_width;
    const int m_height;
    ;
    std::vector<Pixel> m_pixels;
    std::set<Pixel> m_subscribedPixels;
};

struct PatchSlot
{
    PatchSlot(PatchBoardI* patchBoardI) :
        m_patchBoardI(patchBoardI) { }

    void registerCandidate(std::shared_ptr<Patch> patch)
    {
        std::set<std::shared_ptr<Patch>>& patches = m_candidates[patch->color()];
        patches.insert(patch);
    }

    void unRegisterCandidate(std::shared_ptr<Patch> patch)
    {
        std::set<std::shared_ptr<Patch>>& patches = m_candidates[patch->color()];
        patches.erase(patch);
    }

    std::shared_ptr<Patch> getCandidate(const cells::Color& color)
    {
        auto findIt = m_candidates.find(color);

        // no patch
        if (findIt == m_candidates.end()) {
            return std::shared_ptr<Patch>();
        }

        std::set<std::shared_ptr<Patch>>& patches = m_candidates[color];

        // one patch
        auto firstPatchIt = patches.begin();
        if (patches.size() == 1)
            return *firstPatchIt;

        std::shared_ptr<Patch> returnPatch = *firstPatchIt;

        // multiple patch
        for (auto i = ++firstPatchIt; i != patches.end();) {
            std::set<std::shared_ptr<Patch>>::iterator current = i++;

            std::shared_ptr<Patch> candidatePatch = *current;
            if (*candidatePatch < *returnPatch) {
                auto tmp       = returnPatch;
                returnPatch    = candidatePatch;
                candidatePatch = tmp;
            }
            // loggerPtr->log(DEBUG) << " - patch (" << candidatePatch.get() << ") merged to " << returnPatch.get();
            // loggerPtr->log(DEBUG) << " - returnPatch (" << returnPatch.get() << ")";
            // loggerPtr->logBoard(DEBUG) << returnPatch->toString() << "\n";
            // loggerPtr->log(DEBUG) << " - candidatePatch (" << candidatePatch.get() << ")";
            // loggerPtr->logBoard(DEBUG) << candidatePatch->toString() << "\n";
            returnPatch->merge(candidatePatch);

            // loggerPtr->log(DEBUG) << " - returnPatch (" << returnPatch.get() << ")";
            // loggerPtr->logBoard(DEBUG) << returnPatch->toString() << "\n";
        }

        return returnPatch;
    }

    PatchBoardI* m_patchBoardI = nullptr;
    std::function<void(std::shared_ptr<Patch>)> m_deletePatchCb;
    std::map<cells::Color, std::set<std::shared_ptr<Patch>>> m_candidates;
};

std::ostream& operator<<(std::ostream& os, const cells::Color& color)
{
    os << "[" << color.red.value << "," << color.green.value << "," << color.blue.value << "]";
    return os;
}

class DrawingBoard
{
public:
    DrawingBoard(int width, int height) :
        m_width(width), m_height(height), m_defaultBgColor(cellColors[(int)arc::Colors::black]), m_colors(width * height, m_defaultBgColor)
    {
    }

    void clear()
    {
        for (cells::Color& color : m_colors) {
            color = m_defaultBgColor;
        }
    }

    void setColor(int x, int y, cells::Color color)
    {
        if (!isInRange(x, y))
            return;
        m_colors[currentIndex(x, y)] = color;
    }

    void renderLine(int x, int y, const cells::Color& color, RotationDir direction)
    {
        if (!isInRange(x, y))
            return;

        switch (direction) {

        // 🡩🡩🡩🡩🡩🡩🡩🡩🡩🡩🡩🡩🡩🡩
        // 🡫🡫🡫🡫🡫🡫🡫🡫🡫🡫🡫🡫🡫🡫
        case RotationDir::Degree_0:
        case RotationDir::Degree_180:
            for (int i = 0; i < m_height; ++i) {
                m_colors[currentIndex(x, i)] = color;
            }
            break;

        // 🡭🡭🡭🡭🡭🡭🡭🡭🡭🡭🡭🡭🡭🡭🡭
        // 🡯🡯🡯🡯🡯🡯🡯🡯🡯🡯🡯🡯🡯🡯🡯
        case RotationDir::Degree_45:
        case RotationDir::Degree_225: {
            int newX = 0;
            int newY = 0;
            int iterCount = 0;

            // line equation is y = x + diffXY
            // line equation is x = y - diffXY
            int diffXY = x - y;

            x = -diffXY; // solved X coordinate when y = 0
            y = diffXY;  // solved y coordinate when x = 0

            // one of the coordinate will be negative, we check here which coordinate is positive
            if (x > y) {
                // x must be a positive integer so y is a negative integer
                newX = x - y;
                newY = 0;

                // solving y for last x coordinate of the board
                int lastY = lastXIndex() + diffXY;
                int iterCountY = lastY - newY;

                // solving x for lastY
                int lastX = lastY - diffXY;
                int iterCountX = lastX - newX;

                iterCount = iterCountX < iterCountY ? iterCountX : iterCountY;
            } else if (y > x) {
                // y must be a positive integer so x is a negative integer
                newX = 0;
                newY = y - x;

                // solving x for last y coordinate of the board
                int lastX = lastYIndex() - diffXY;
                int iterCountX = lastX - newX;

                // solving y for lastX
                int lastY = lastX + diffXY;
                int iterCountY = lastY - newY;

                iterCount = iterCountX < iterCountY ? iterCountX : iterCountY;
            } else if (x == y) {
                iterCount = lastXIndex();
            }

            for (int i = 0; i <= iterCount; ++i) {
                m_colors[currentIndex(newX, newY)] = color;
                newX += 1;
                newY += 1;
            }
        } break;

        // 🡪🡪🡪🡪🡪🡪🡪🡪🡪🡪🡪🡪🡪🡪🡪
        // 🡨🡨🡨🡨🡨🡨🡨🡨🡨🡨🡨🡨🡨🡨🡨
        case RotationDir::Degree_90:
        case RotationDir::Degree_270:
            for (int i = 0; i < m_width; ++i) {
                m_colors[currentIndex(i, y)] = color;
            }
            break;

        // 🡮🡮🡮🡮🡮🡮🡮🡮🡮🡮🡮🡮🡮🡮🡮
        // 🡬🡬🡬🡬🡬🡬🡬🡬🡬🡬🡬🡬🡬🡬🡬
        case RotationDir::Degree_135:
        case RotationDir::Degree_315: {
            int newX = x;
            int newY = y;
            for (int i = x; i < m_width; ++i) {
                if (!isInRange(newX, newY)) {
                    continue;
                }
                m_colors[currentIndex(newX, newY)] = color;
                newX += 1;
                newY -= 1;
            }
            newX = x - 1;
            newY = y + 1;
            for (int i = x - 1; i >= 0; --i) {
                if (!isInRange(newX, newY)) {
                    continue;
                }
                m_colors[currentIndex(newX, newY)] = color;
                newX -= 1;
                newY += 1;
            }
        } break;
        }
    }

    void renderVectorShape(const VectorShape& vectorShape)
    {
        renderVectorShape(vectorShape.firstPixel().x, vectorShape.firstPixel().y, vectorShape);
    }

    void renderVectorShape(int x, int y, const VectorShape& vectorShape)
    {
        setColor(x, y, cellColors[2]);
        //loggerPtr->log(DEBUG) << " setColor [" << x << ", " << y << "]";
        for (const Vector& vector : vectorShape.vectors()) {
            x += vector.x;
            y += vector.y;
            setColor(x, y, vectorShape.color());
            //loggerPtr->log(DEBUG) << " setColor [" << x << ", " << y << "]";
        }
    }

    std::string toString() const
    {
        std::string board(m_width * m_height, '.');
        for (int y = 0; y < m_height; ++y) {
            for (int x = 0; x < m_width; ++x) {
                char boardColor        = '0' + cellColorsToColorId.at(m_colors[currentIndex(x, y)]);
                board[y * m_width + x] = boardColor;
            }
        }
        std::string ret;
        ret += std::to_string(m_width);
        ret += ' ';
        ret += std::to_string(m_height);
        ret += ' ';
        ret += board;

        return ret;
    }

    int lastXIndex() const
    {
        return m_width - 1;
    }

    int lastYIndex() const
    {
        return m_height - 1;
    }

private:
    int currentIndex(int x, int y) const
    {
        return y * m_width + x;
    }

    bool isInRange(int x, int y) const
    {
        if (y < 0 || x < 0 || x >= m_width || y >= m_height) {
            return false;
        }

        return true;
    }

    const int m_width;
    const int m_height;
    const cells::Color& m_defaultBgColor;
    std::vector<cells::Color> m_colors;
};

// Maybe a better algorith just
// - has an input set of pixels (x:0, y:0, color)
// - has a rule for grouping same pixels:
//     + when a pixel-group (currently this is the class Patch) is started, the color of the pixel-group will be the color of the first pixel
//     + try growing the pixel in every possible (8) direction by moving pixels from the input set to the group's set
//     + when no more possibility for growing start a new pixel-group
// - so this algo only dealing with one pixel-group a time
//
// Current algo:
// - creating a patch candidate list for every pixel
// - on every pixel try to grow the patch, by adding the patch pointer to the candidate list
// - if a pixel has no candidate patch then create one
// - if a pixel has a candidate then grow that grouo
// - if a pixel has multiple candidates then merge the patches and the winner is who started earlier (has lovest pixel index)
// - this is too complite but works
//
class PatchBoard : public PatchBoardI
{
public:
    PatchBoard(const cells::Sensor& sensor) :
        m_width(sensor.width()), m_height(sensor.height()), m_sensor(sensor), m_patchSlots(m_height * m_width, PatchSlot(this))
    {
    }

    int width() const override
    {
        return m_width;
    }

    int height() const override
    {
        return m_height;
    }

    void process()
    {
        for (int y = 0; y < height(); ++y) {
            for (int x = 0; x < width(); ++x) {
                const cells::Pixel& pixel = m_sensor.getPixel(x, y);
                //                logger.log(DEBUG) << "Processing pixel[" << x << ", " << y << "]" << pixel.color;
                processPixel(x, y, pixel.color);
            }
        }
        int id = 1;
        std::vector<std::shared_ptr<Patch>> sortedPatches;
        for (std::shared_ptr<Patch> patch : m_patches) {
            sortedPatches.push_back(patch);
        }
        std::sort(sortedPatches.begin(), sortedPatches.end(), [](const std::shared_ptr<Patch>& lhs, const std::shared_ptr<Patch>& rhs) { return *lhs < *rhs; });
        for (std::shared_ptr<Patch> patch : sortedPatches) {
            patch->assignId(id++);
            patch->vectorize();
            loggerPtr->log(DEBUG) << "Patch " << patch->m_id << "\n";
            loggerPtr->logBoard(DEBUG) << patch->toString() << "\n";
        }
    }

    void processPixel(int x, int y, const cells::Color& color)
    {
        PatchSlot& patchSlot             = getPatchSlot(x, y);
        std::shared_ptr<Patch> candidate = patchSlot.getCandidate(color);

        if (candidate) {
            // loggerPtr->log(DEBUG) << " - pixel[" << x << ", " << y << "] " << color << " - patch found " << "(" << candidate.get() << ")";
        } else {
            candidate = std::make_shared<Patch>(color, this);
            // loggerPtr->log(DEBUG) << " - pixel[" << x << ", " << y << "] " << color << " - patch created " << "(" << candidate.get() << ")";
            m_patches.insert(candidate);
        }
        candidate->addPixelCoordinate(x, y);
    }

    const std::set<std::shared_ptr<Patch>>& patches() const
    {
        return m_patches;
    }

    void subscribePatchForPixel(std::shared_ptr<Patch> patch, int x, int y) override
    {
        subscribePatchForPixelImpl(patch, x + 1, y);
        subscribePatchForPixelImpl(patch, x + 1, y + 1);
        subscribePatchForPixelImpl(patch, x, y + 1);
        subscribePatchForPixelImpl(patch, x - 1, y + 1);
    }

    void mergePatch(std::shared_ptr<Patch> winner, std::shared_ptr<Patch> looser) override
    {
        const std::set<Pixel> subscribedPixels = looser->subscribedPixels();
        for (const Pixel& pixel : subscribedPixels) {
            PatchSlot& patchSlot = getPatchSlot(pixel.x, pixel.y);
            patchSlot.unRegisterCandidate(looser);
            patchSlot.registerCandidate(winner);
        }
        m_patches.erase(looser);
        // loggerPtr->log(DEBUG) << " - patch deleted (" << looser.get() << ")";
    }

protected:
    void subscribePatchForPixelImpl(std::shared_ptr<Patch> patch, int x, int y)
    {
        if (!isInRange(x, y))
            return;
        PatchSlot& patchSlot = getPatchSlot(x, y);
        patchSlot.registerCandidate(patch);
        patch->registerSubscribedPixel(x, y);
    }

    PatchSlot& getPatchSlot(int x, int y)
    {
        return m_patchSlots[currentIndex(x, y)];
    }

    int currentIndex(int x, int y) const
    {
        return y * m_width + x;
    }

    bool isInRange(int x, int y) const
    {
        if (y < 0 || x < 0 || x > m_width - 1 || y > m_height - 1) {
            return false;
        }

        return true;
    }

    const int m_width;
    const int m_height;
    const cells::Sensor& m_sensor;
    std::vector<PatchSlot> m_patchSlots;
    std::set<std::shared_ptr<Patch>> m_patches;
};

void Solver::solve()
{
    loggerPtr = &logger;
    logger.clearLogFile();
    logger.log(INFO) << "There are " << m_arcTask.m_demonstrations.size() << " demo tasks";
    unsigned int i = 1;
    for (const auto& arcDemo : m_arcTask.m_demonstrations) {
        const cells::Sensor& m_input  = arcDemo.m_input;
        const cells::Sensor& m_output = arcDemo.m_output;
        //        logger.log(INFO) << " (" << i << ") mapping[" << m_input.m_width << ", " << m_input.m_height << "] to[" << m_output.m_width << ", " << m_output.m_height << "] ";
        solveOne(m_input);
        break;
        solveOne(m_output);
        i++;
    }
    //    solveOne(m_arcTask.m_testInput);

    const cells::Sensor& m_input = m_arcTask.m_testInput;
    logger.log(INFO) << "Mapping input[" << m_input.m_width << ", " << m_input.m_height << "] to ... ?";
}

void Solver::solveOne(const cells::Sensor& sensor)
{
    PatchBoard patchBoard(sensor);
    patchBoard.process();
    std::shared_ptr<Patch> patch;
    for (std::shared_ptr<Patch> i : patchBoard.patches()) {
        if (i->size() == 7)
            patch = i;
    }
    logger.log(DEBUG) << "Patch:";
    loggerPtr->logBoard(DEBUG) << patch->toString() << "\n";
    DrawingBoard drawingBoard(25, 25);
    VectorShape vectorShape = patch->vectorize();
    vectorShape.firstPixel({ 3, 2 });

    for (int i = 0; i < 8; ++i) {
        drawingBoard.clear();
        drawingBoard.renderVectorShape(2, 1, vectorShape.stretch(1, 1));
        drawingBoard.renderVectorShape(2, 10, vectorShape.stretch(1, 2));
        drawingBoard.renderVectorShape(10, 3, vectorShape.stretch(2, 1));
        drawingBoard.renderVectorShape(10, 9, vectorShape.stretch(2, 2));
        drawingBoard.renderVectorShape(10, 18, vectorShape.stretch(3, 3));
#if 0
        VectorShape rotatedShape = vectorShape.rotate((RotationDir)i);
        drawingBoard.renderVectorShape(rotatedShape);
        drawingBoard.renderVectorShape(rotatedShape.mirror({ 3, 0 }, RotationDir::Degree_0));
        drawingBoard.renderVectorShape(rotatedShape.mirror({ 0, 3 }, RotationDir::Degree_90));
        drawingBoard.renderLine(6, 6, color(arc::Colors::grey), RotationDir::Degree_0);
        drawingBoard.renderLine(6, 6, color(arc::Colors::grey), RotationDir::Degree_45);
        drawingBoard.renderLine(6, 6, color(arc::Colors::grey), RotationDir::Degree_90);
        drawingBoard.renderLine(6, 6, color(arc::Colors::grey), RotationDir::Degree_135);
#endif
        logger.log(DEBUG) << "DrawingBoard:";
        loggerPtr->logBoard(DEBUG) << drawingBoard.toString() << "\n";
        break;
    }

    logger.log(DEBUG) << "Number of patches found: " << patchBoard.patches().size();
}

/*

A program célja, hogy két pixelmátrixból megtalálja a transzformációs algoritmust.
A feladatok során mindig található egy transzformációs algoritmus, ami egyik mátrixból előállítja a másikat.

Első lépés, hogy a pixel mátrixban azonosítsuk az azonos színű alakzatokat. Az alakzat akár 1 pixeles is lehet.
Az egyik kitüntetett alakzat a "háttér" alakzat, ez jellemzően fekete. Sok példában, de nem mindben az alakzatok egy háttér előtt vannak.

Egy 2D alakzatot lehet forgatni, kicsinyíteni, nagyítani, mozgatni, átszínezni valamilyen szabály szerint.

Pixels => Shapes

Egyik fő stratégia, hogy megkeressük az egyik alakzatot a másikban.

Másik, hogy fő kategóriákat próbálunk végigpróbálni.

- az alakzat geometriai transzformációja
  az egyik a másiknak kicsinyítése - nagyítása, forgatása, x része (fele, negyede, ...), elmozgatása (más helyen van)
  Mert minden pixel egyben egy alakzat is. Az alakzaton lehet geometriai transzformációt végrehajtani.
  Minden pixel tetszőleges másik alakzatot alkothat, amin szintén lehet geometriai transzformációt végrehajtani.

- az alakzat darabszáma változik, ismétlődések, minták
  + az eredeti alakzat szimplán többször megtalálható a célban
  + az eredeti alakzat valamilyen formában megtalálható a célban, de hiányzik valami...

- az alakzat egy vagy több tulajdonságának vizuális leképzése jelenik meg
  pl. ahány kék négyzet van, annyi kék pixel lesz (1fad071e)

- reprezentáció
  pl. a piros reprezentálja a legkisebb alakzatot
  a pixelek rácsot alkotnak, amiben pixelek vannak
  van forrás-cél az alakzaton belül valamilyen elválasztó mentén

- kitüntett tulajdonság keresése
  pl. jelöld pirossal azokat a helyeket, ahol a legtöbb pixel van (29623171)

https://github.com/arc-community/wiki/wiki/Key-findings-by-various-people-and-FAQ#andreaskopf

Koncepciók, amik fontosak lehetnek:
 - alakzat / háttér felismerés
 - élek
 - sarok ( első körben 90 fokos egyenes vonalak esetén )
 - lekerekített / szögletes sarok
 - teli / üres
 - egyszerű alakzatok, pl. négyzet, téglalap, háromszög, kör
 - kívül / belül
 - ismétlődés
 - vonalak (egyenes, szaggatott, ...)
 - rendezés (pl. nagyság szerint)
 - szimmetriák
 - méretek
 - darabszám
 - távolság
 - minta folytatása
 - mozgások (visszapattan, vonz, taszít)
 - falak, labirintusok
 - logika
 - algoritmikus formák: spirál, hullámvonal
 - halmazműveletek
 - kivétel
 - zaj ( nem odaillő pixelek )

Két dimenziós világ:
 - a kép, mint x * y felbontású pixel mátrix
 - a mátrixon belül alakzatok lehetnek. Az alakzat lehet a háttér is.
 - a sík végtelen kiterjedésű, de a szenzor véges felbontású, tehát a szenzoron kívül is lehetnek pixelek, amiket a szenzor nem lát
 - az alakzatnak van körvonala
   - a körvonalnak van éle
   - az él lehet egyenes vagy görbe
   - egyenes élek találkozásakor sarokról beszélünk

Gondolkozási keret
==================

Forrás -> Cél
-------------
  - A feladat az, hogy rá kell jönni, mik azok a transzformációs lépések, amik a forrás alakzatból elvisznek a cél alakzatba.
  - A forrás és a cél egy pixel mátrix
  - A pixelnek lehet szine
  Cells: Szín, Pixel, Mátrix, Forrás, Cél

Az alakzat
----------
 - Az alakzat a forrás és cél alakzaton belüli pixel halmaz

 Nem tudjuk hány alakzat van, legalább egy van, de lehet több is.
 Az alakzatnak vannak tulajdonságai: pixelek száma, magasság - szélesség pixelben, pixelek színe
 Az alakzatokra bármilyen transzormáció alkalmazható.
 Az alakzat lehet háttér és objektum szerepben.
 A transzformáció bármilyen egy vagy többlépéses művelet lehet.
 A transzformációt műveletek írják le.

 A gondolkodási keret az, hogy van a forrás káp és a cél kép.
 A képen vannak alakzatok, amik megváltoznak valamilyen módon a cél képen.
 Az alakzatok valamilyen háttér előtt vannak.
 Az alakzatok kitakarják a hátteret.

Kategorizálás példák segítségével -> Kell egy önkategorizáló algoritmus
Felismerés, reprezentáció -> a pixelek reprezentálhatnak ismert dolgokat

Ismert dolgok: vonal (függőleges, vízszintes, ferde, görbe)
               A vonalak alkothatnak ábrákat.


Stratégia példák alapján:
https://qugank.github.io/pqa.github.io/

a. Körvonal által kijelölt terület beszínezése
   xxx.    xxx.
   x..x => xxxx
   .xx.    .xx.

b. Végpontok összekötese vonallá
   ....x...    ....x...
   x.......    x.......
   ........ => x.......
   x....x..    xxxxxx..
   ........    ........

c. Közelség értelmezése
   ....a...    ........
   b.......    ........
   ........ => ........
   c.dddd..    ..cccc..
   .....d..    .....c..
   A legközelebbi pixel színét veszi fel a több pixelből álló alakzat

d. Alakzat helyreállítása
   ........    ........
   xxx..x..    xxxxxx..
   ..xxx... => xxxxxx..
   x.xx.x..    xxxxxx..
   ........    ........
   Kiegészítés téglalappá

e. Alakzat és minta hasonlóság
   ab...a..    ab......
   .....b..    ........
   ........ => ........
   ..cccc..    ..aabb..
   ..cccc..    ..aabb..

f. Tükrözés 1 tengelyen
   ..aaa.a.    ..aaa.a.
   ..a...a.    ..a...a.
   ..bbbbb. => ..a...a.
   ........    ..aaa.a.
   ........    ........

g. 4 tengelyen tükrözött minta hiányzó részeinek megtalálása
   abcddcba    abcddcba
   bbadddab    bbadddab
   cbbbbbbc => cbbbbbbc
   cbb..bbc    cbbbbbbc
   bb....ab    bbadddab
   abcddcba    abcddcba
*/

} // namespace synth