#include "Colors.h"
#include "Solver.h"
#include <array>
#include <set>


namespace synth {

const std::array<input::Color, 10> cellColors = {
    input::Color(0x00, 0x00, 0x00), /* black */
    input::Color(0x00, 0x74, 0xD9), /* blue */
    input::Color(0xFF, 0x41, 0x36), /* red */
    input::Color(0x2E, 0xCC, 0x40), /* green */
    input::Color(0xFF, 0xDC, 0x00), /* yellow */
    input::Color(0xAA, 0xAA, 0xAA), /* grey */
    input::Color(0xF0, 0x12, 0xBE), /* fuschia */
    input::Color(0xFF, 0x85, 0x1B), /* orange */
    input::Color(0x7F, 0xDB, 0xFF), /* teal */
    input::Color(0x87, 0x0C, 0x25)  /* brown */
};

const input::Color& color(arc::Colors c)
{
    return cellColors[(int)c];
}

const std::map<const input::Color, int> cellColorsToColorId = {
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

Pixel Pixel::operator+(const Vector& rhs) const
{
    return Pixel(x + rhs.x, y + rhs.y);
}

Pixel& Pixel::operator+=(const Vector& rhs)
{
    x += rhs.x;
    y += rhs.y;

    return *this;
}

Pixel& Pixel::operator-=(const Vector& rhs)
{
    x -= rhs.x;
    y -= rhs.y;

    return *this;
}

std::ostream& operator<<(std::ostream& os, const Pixel& pixel)
{
    os << "P[" << pixel.x << "," << pixel.y << "]";
    return os;
}


Vector& Vector::operator+=(const Vector& rhs)
{
    x += rhs.x;
    y += rhs.y;

    return *this;
}

Vector& Vector::operator-=(const Vector& rhs)
{
    x -= rhs.x;
    y -= rhs.y;

    return *this;
}

Vector Vector::rotate(RotationDir rotationDir) const
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

std::ostream& operator<<(std::ostream& os, const Vector& vector)
{
    os << "V[" << vector.x << "," << vector.y << "]";
    return os;
}

void BoundingBox::fromPixels(const std::vector<Pixel>& pixels)
{
    leftX = pixels.front().x;
    topY  = pixels.front().y;
    for (const Pixel& pixel : pixels) {
        update(pixel);
    }

    firstPixelVector(pixels.front());
    lastPixelVector(pixels.back());
}

BoundingBox::BoundingBox(const VectorShape& vektorShape)
{
    fromVectors(vektorShape.firstPixel(), vektorShape.vectors());
}

void BoundingBox::fromVectors(const Pixel& firstPixel, const std::vector<Vector>& vectors)
{
    Pixel actualPixel = firstPixel;

    leftX = firstPixel.x;
    topY  = firstPixel.y;
    update(actualPixel);

    for (const Vector& vector : vectors) {
        actualPixel += vector;
        update(actualPixel);
    }
    firstPixelVector(firstPixel);
    lastPixelVector(actualPixel);
}

void BoundingBox::update(const Pixel& pixel)
{
    if (pixel.x < leftX)
        leftX = pixel.x;
    if (pixel.x > rightX)
        rightX = pixel.x;
    if (pixel.y < topY)
        topY = pixel.y;
    if (pixel.y > bottomY)
        bottomY = pixel.y;
}

void BoundingBox::firstPixelVector(const Pixel& pixel)
{
    m_toFirstPixel.x = pixel.x - leftX;
    m_toFirstPixel.y = pixel.y - topY;
}

void BoundingBox::lastPixelVector(const Pixel& pixel)
{
    m_toLastPixel.x = pixel.x - leftX;
    m_toLastPixel.y = pixel.y - topY;
}

void VectorShape::fromPixels(const std::vector<Pixel>& pixels)
{
    m_vectors.clear();
    firstPixel(pixels.front());
    const Pixel* prevPixel = &pixels.front();
    bool isFirstPixel      = true;

    for (const auto& pixel : pixels) {
        if (isFirstPixel) {
            isFirstPixel = false;
            continue;
        }
        const Pixel* currPixel = &pixel;
        m_vectors.push_back({ currPixel->x - prevPixel->x, currPixel->y - prevPixel->y });
        prevPixel = currPixel;
    }
}

VectorShape VectorShape::rotate(RotationDir rotationDir) const
{
    std::vector<Vector> rotatedVectors;

    for (const Vector& vector : m_vectors) {
        Vector newVector = vector.rotate(rotationDir);
        rotatedVectors.push_back(newVector);
        //            loggerPtr->log(DEBUG) << " rotate vector [" << vector.x << ", " << vector.y << "] => [" << newVector.x << ", " << newVector.y << "]";
    }

    return VectorShape(std::move(rotatedVectors), m_color, m_firstPixel);
}

VectorShape VectorShape::stretch(int horizontal, int vertical) const
{
    if ((horizontal < 1 || vertical < 1) || (horizontal == 1 && vertical == 1)) {
        // Do nothing
        return VectorShape(*this);
    }
    std::vector<Vector> stretchVectors;
    stretchPixel(stretchVectors, horizontal, vertical);

    for (const Vector& vector : m_vectors) {
        stretchVectors.push_back({ -(horizontal - 1) + (vector.x * horizontal), -(vertical - 1) - vector.y * vertical });
        // loggerPtr->log(DEBUG) << " stretchVector [" << vector.x << ", " << vector.y << "] -> [" << stretchVectors.back().x << ", " << stretchVectors.back().y << "]";
        stretchPixel(stretchVectors, horizontal, vertical);
    }

    return VectorShape(std::move(stretchVectors), color(), firstPixel());
}

void VectorShape::stretchPixel(std::vector<Vector>& stretchVectors, int horizontal, int vertical)
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

Pixel VectorShape::calculatePixel(DistanceType distanceType, const Vector distance) const
{
    if (distanceType == DistanceType::FromFirstPixel) {
        return m_firstPixel + distance;
    } else {
        BoundingBox boundingBox(*this);
        switch (distanceType) {
        case DistanceType::FromTopLeft:
            return boundingBox.topLeftPixel() + distance;
        case DistanceType::FromTopRight:
            return boundingBox.topRightPixel() + distance;
        case DistanceType::FromBottomLeft:
            return boundingBox.bottomLeftPixel() + distance;
        case DistanceType::FromBottomRight:
            return boundingBox.bottomRightPixel() + distance;
        case DistanceType::FromLastPixel:
            return m_firstPixel + boundingBox.toLastPixel() + distance;
        case DistanceType::FromFirstPixel:
            break;
        }
    }

    return m_firstPixel;
}

VectorShape VectorShape::mirror(DistanceType distanceType, const Vector distance, RotationDir axisDir) const
{
    Pixel mirrorPixel = calculatePixel(distanceType, distance);
    std::vector<Vector> mirrorShape;

    switch (axisDir) {
    case RotationDir::Degree_0:
    case RotationDir::Degree_180:
        // y coordinate will be the same
        {
            const Vector vectorToMirrorPoint(mirrorPixel.x - m_firstPixel.x, 0);
            Pixel firstPixel = m_firstPixel + vectorToMirrorPoint + vectorToMirrorPoint;
            for (const Vector& vector : m_vectors) {
                mirrorShape.push_back(vector.mirrorY());
            }

            return VectorShape(std::move(mirrorShape), color(), firstPixel);
        }
        break;

    case RotationDir::Degree_45:
    case RotationDir::Degree_225: {
        // the mirror pixel for the first pixel
        mirrorPixel = Pixel(mirrorPixel.x + (mirrorPixel.y - m_firstPixel.y), m_firstPixel.y);

        // vector to the mirror point
        const Vector vectorToMirrorPoint(mirrorPixel.x - m_firstPixel.x, 0);
        const RotationDir rotationDir = RotationDir::Degree_90;

        Pixel firstPixel = m_firstPixel + vectorToMirrorPoint + vectorToMirrorPoint.rotate(rotationDir);

        for (const Vector& vector : m_vectors) {
            mirrorShape.push_back(vector.mirrorY().rotate(rotationDir));
        }

        return VectorShape(std::move(mirrorShape), color(), firstPixel);
    } break;

    case RotationDir::Degree_135:
    case RotationDir::Degree_315: {
        // the mirror pixel for the first pixel
        mirrorPixel = Pixel(mirrorPixel.x - (mirrorPixel.y - m_firstPixel.y), m_firstPixel.y);

        // vector to the mirror point
        const Vector vectorToMirrorPoint(mirrorPixel.x - m_firstPixel.x, 0);
        const RotationDir rotationDir = RotationDir::Degree_270;

        Pixel firstPixel = m_firstPixel + vectorToMirrorPoint + vectorToMirrorPoint.rotate(rotationDir);

        for (const Vector& vector : m_vectors) {
            mirrorShape.push_back(vector.mirrorY().rotate(rotationDir));
        }

        return VectorShape(std::move(mirrorShape), color(), firstPixel);
    } break;

    case RotationDir::Degree_90:
    case RotationDir::Degree_270:
        // x coordinate will be the same
        {
            const Vector vectorToMirrorPoint(0, mirrorPixel.y - m_firstPixel.y);
            Pixel firstPixel = m_firstPixel + vectorToMirrorPoint + vectorToMirrorPoint;
            for (const Vector& vector : m_vectors) {
                mirrorShape.push_back(vector.mirrorX());
            }

            return VectorShape(std::move(mirrorShape), color(), firstPixel);
        }
        break;
    }

    return *this;
}

VectorShapeRelation VectorShape::compare(const VectorShape& other)
{
    VectorShapeRelation ret;

    if (vectors() == other.vectors()) {
        ret.exactMatch = true;
        return ret;
    }
    // TODO

    return ret;
}

void Patch::addPixel(cells::hybrid::Pixel& pixel)
{
    m_pixels.push_back({ pixel.m_x.value(), pixel.m_y.value() });
    m_hybridPixels.insert(&pixel);
}

bool Patch::hasPixel(cells::hybrid::Pixel& pixel) const
{
    return m_hybridPixels.find(&pixel) != m_hybridPixels.end();
}

void Patch::sortPixels()
{
    int width = m_width;
    std::sort(m_pixels.begin(), m_pixels.end(), [width](const Pixel& p1, const Pixel& p2) {
        return p1.y * width + p1.x < p2.y * width + p2.x;
    });
}

std::string Patch::toString() const
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

VectorShape Patch::toVectorShape() const
{
    VectorShape ret(color());
    ret.fromPixels(pixels());

    return ret;
}

DrawingBoard::DrawingBoard(int width, int height) :
    m_width(width), m_height(height), m_defaultBgColor(cellColors[(int)arc::Colors::black]), m_colors(width * height, m_defaultBgColor)
{
}

void DrawingBoard::clear()
{
    for (input::Color& color : m_colors) {
        color = m_defaultBgColor;
    }
}

void DrawingBoard::setColor(int x, int y, input::Color color)
{
    if (!isInRange(x, y))
        return;
    m_colors[currentIndex(x, y)] = color;
}

void DrawingBoard::renderLine(int x, int y, int len, const input::Color& color, RotationDir direction)
{
    switch (direction) {

    // 🡩🡩🡩🡩🡩🡩🡩🡩🡩🡩🡩🡩🡩🡩
    // 🡫🡫🡫🡫🡫🡫🡫🡫🡫🡫🡫🡫🡫🡫
    case RotationDir::Degree_0:
    case RotationDir::Degree_180:
        for (int i = 0; i < len; ++i) {
            setColor(x, y + i, color);
        }
        break;

    // 🡭🡭🡭🡭🡭🡭🡭🡭🡭🡭🡭🡭🡭🡭🡭
    // 🡯🡯🡯🡯🡯🡯🡯🡯🡯🡯🡯🡯🡯🡯🡯
    case RotationDir::Degree_45:
    case RotationDir::Degree_225: {
        int newX = x;
        int newY = y;
        for (int i = 0; i < len; ++i) {
            setColor(newX++, newY++, color);
        }

    } break;

    // 🡪🡪🡪🡪🡪🡪🡪🡪🡪🡪🡪🡪🡪🡪🡪
    // 🡨🡨🡨🡨🡨🡨🡨🡨🡨🡨🡨🡨🡨🡨🡨
    case RotationDir::Degree_90:
    case RotationDir::Degree_270:
        for (int i = 0; i < len; ++i) {
            setColor(x + i, y, color);
        }
        break;

    // 🡮🡮🡮🡮🡮🡮🡮🡮🡮🡮🡮🡮🡮🡮🡮
    // 🡬🡬🡬🡬🡬🡬🡬🡬🡬🡬🡬🡬🡬🡬🡬
    case RotationDir::Degree_135:
    case RotationDir::Degree_315: {
        int newX = x;
        int newY = y;
        for (int i = 0; i < len; ++i) {
            setColor(newX++, newY--, color);
        }
    } break;
    }
}

void DrawingBoard::renderLine(int x, int y, const input::Color& color, RotationDir direction)
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
            newY += 1;
        }
        newX = x - 1;
        newY = y - 1;
        for (int i = x - 1; i >= 0; --i) {
            if (!isInRange(newX, newY)) {
                continue;
            }
            m_colors[currentIndex(newX, newY)] = color;
            newX -= 1;
            newY -= 1;
        }
    } break;
    }
}

void DrawingBoard::renderVectorShape(int x, int y, const VectorShape& vectorShape)
{
    setColor(x, y, cellColors[2]);
    // loggerPtr->log(DEBUG) << " setColor [" << x << ", " << y << "]";
    for (const Vector& vector : vectorShape.vectors()) {
        x += vector.x;
        y += vector.y;
        setColor(x, y, vectorShape.color());
        // loggerPtr->log(DEBUG) << " setColor [" << x << ", " << y << "]";
    }
}

void DrawingBoard::renderBoundingBox(const BoundingBox& boundingBox)
{
    const input::Color pixelColor = color(arc::Colors::grey);
    renderLine(boundingBox.leftX, boundingBox.topY, boundingBox.height(), pixelColor, RotationDir::Degree_0);
    renderLine(boundingBox.rightX, boundingBox.topY, boundingBox.height(), pixelColor, RotationDir::Degree_0);

    renderLine(boundingBox.leftX, boundingBox.topY, boundingBox.width(), pixelColor, RotationDir::Degree_90);
    renderLine(boundingBox.leftX, boundingBox.bottomY, boundingBox.width(), pixelColor, RotationDir::Degree_90);
}

std::string DrawingBoard::toString() const
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

PatchBoard::PatchBoard(const cells::hybrid::Picture& picture) :
    m_width(picture.width()),
    m_height(picture.height()),
    m_picture(picture),
    kb(picture.kb)
{
    processInputPixels();
}

void PatchBoard::processInputPixels()
{
    std::vector<cells::hybrid::Pixel>& pixels = const_cast<cells::hybrid::Picture&>(m_picture).pixels();
    for (cells::hybrid::Pixel& pixel : pixels) {
        m_inputPixels.insert(&pixel);
    }
}

// The object extractor algorithm
// - has an input set of pixels (x:0, y:0, color)
// - has a rule for grouping same pixels:
//     + when a pixel-group (currently this is the class Patch) is started, the color of the pixel-group will be the color of the first pixel
//     + try growing the pixel in every possible (8) direction by moving pixels from the input set to the group's set
//     + when no more possibility for growing start a new pixel-group
// - so this algo only dealing with one pixel-group a time
void PatchBoard::process()
{
    int patchId = 1;
    while (!m_inputPixels.empty()) {
        cells::hybrid::Pixel& firstPixel = **m_inputPixels.begin();
        m_patches.push_back(std::make_shared<Patch>(patchId++, firstPixel.color(), m_width, m_height));
        Patch& patch = *m_patches.back();
        std::set<cells::hybrid::Pixel*> checkPixels;
        checkPixels.insert(&firstPixel);
        while (!checkPixels.empty()) {
            auto checkPixelIt                = checkPixels.begin();
            cells::hybrid::Pixel& checkPixel = **checkPixelIt;
            processPixel(patch, checkPixels, checkPixel);
            checkPixels.erase(checkPixelIt);
        }
        patch.sortPixels();
    }
    std::sort(m_patches.begin(), m_patches.end(),
        [](const std::shared_ptr<Patch>& lhs, const std::shared_ptr<Patch>& rhs)
        { return *lhs < *rhs; }
    );
}

void PatchBoard::processPixel(Patch& patch, std::set<cells::hybrid::Pixel*>& checkPixels, cells::hybrid::Pixel& checkPixel)
{
    patch.addPixel(checkPixel);
    m_inputPixels.erase(&checkPixel);

    if (cells::hybrid::Pixel* pixel = processAdjacentPixel(kb.directions.up, patch, checkPixels, checkPixel)) {
        processAdjacentPixel(kb.directions.left, patch, checkPixels, *pixel);
        processAdjacentPixel(kb.directions.right, patch, checkPixels, *pixel);
    }
    if (cells::hybrid::Pixel* pixel = processAdjacentPixel(kb.directions.down, patch, checkPixels, checkPixel)) {
        processAdjacentPixel(kb.directions.left, patch, checkPixels, *pixel);
        processAdjacentPixel(kb.directions.right, patch, checkPixels, *pixel);
    }
    processAdjacentPixel(kb.directions.left, patch, checkPixels, checkPixel);
    processAdjacentPixel(kb.directions.right, patch, checkPixels, checkPixel);
}

cells::hybrid::Pixel* PatchBoard::processAdjacentPixel(cells::CellI& direction, Patch& patch, std::set<cells::hybrid::Pixel*>& checkPixels, cells::hybrid::Pixel& checkPixel)
{
    if (checkPixel.has(direction)) {
        cells::hybrid::Pixel& pixel = static_cast<cells::hybrid::Pixel&>(checkPixel[direction]);
        if (pixel.color() == patch.color() && !patch.hasPixel(pixel)) {
            checkPixels.insert(&pixel);
        }
        return &pixel;
    }

    return nullptr;
}

Grid::Grid(std::vector<std::shared_ptr<Patch>> patches)
{
    for (const auto& patch : patches) {
        shapes.push_back(patch->toVectorShape());
    }
}

Solver::Solver(Logger& logger, const ArcTask& arcTask) :
    logger(logger), m_arcTask(arcTask)
{
    solve();
}

void Solver::solve()
{
    loggerPtr = &logger;
    logger.clearLogFile();
    logger.log(INFO) << "There are " << m_arcTask.m_demonstrations.size() << " demo tasks";
    unsigned int i = 1;

    std::vector<Rules> rules;
    for (const auto& arcDemo : m_arcTask.m_demonstrations) {
        const cells::hybrid::Picture& m_input = arcDemo.m_input;
        const cells::hybrid::Picture& m_output = arcDemo.m_output;
        //        logger.log(INFO) << " (" << i << ") mapping[" << m_input.m_width << ", " << m_input.m_height << "] to[" << m_output.m_width << ", " << m_output.m_height << "] ";
        const Grid& input = parse(m_input);
        const Grid& output = parse(m_output);
        rules.push_back(gridDiff(input, output));
    }
    const Grid& testInput = parse(m_arcTask.m_testInput);
    const Code& code = processRules(rules);
    DrawingBoard result = applyCode(testInput, code);

    const cells::hybrid::Picture& m_input = m_arcTask.m_testInput;
    logger.log(INFO) << "Mapping input[" << m_input.width() << ", " << m_input.height() << "] to ... ?";
}

Grid Solver::parse(const cells::hybrid::Picture& picture)
{
    PatchBoard patchBoard(picture);
    patchBoard.process();
    for (std::shared_ptr<Patch> patch : patchBoard.patches()) {
        logger.log(DEBUG) << "Patch:";
        logger.logBoard(DEBUG) << patch->toString() << "\n";
        VectorShape vectorShape = patch->toVectorShape();
    }
    logger.log(DEBUG) << "Number of patches found: " << patchBoard.patches().size();

    return Grid(patchBoard.patches());
}

Rules Solver::gridDiff(const Grid& input, const Grid& output)
{
    Rules ret;
    return ret;
}

Code Solver::processRules(const std::vector<Rules>& rules)
{
    Code ret;
    return ret;
}

DrawingBoard Solver::applyCode(const Grid& input, const Code& code)
{
    DrawingBoard ret(1, 1);
    return ret;
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

 A gondolkodási keret az, hogy van a forrás kép és a cél kép.
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

Nagyjából a következő feldolgozási sorhoz állt össze a fejemben:

Van egy bemenő és kimenő pixelhalmazunk, ahol a pixelek mátrix szerűen kapcsolódnak egymáshoz. A mátrix szerű kapcsolódás azt jelenti, hogy minden pixelnek van le/fel/jobbra/balra tulajdonsága.
A feladatunk az, hogy megtaláljuk azt a transzformációs sort, ami eljuttat a bementből a kimenetbe.

Az első transzformációs lépés, hogy a bemenő pixeleket szín alapján csoportosítsuk. Egy ilyen színcsoport neve folt / alakzat (patch) lesz.
Ez azért jó, mert egy ilyen alakzat már lehet transtformálni, pl. forgatni, mozgatni, nagyítani, kicsinyíteni, átszinezni.
Ez már le is van kódolva,

    const cells::Picture& picture = input;
    PatchBoard patchBoard(picture);
    patchBoard.process();
    for (auto patch : patchBoard.patches()) ...

Felmerül a kérdés, hogy ezt az algoritmust biztos nekünk kell leprogramozni? Mi lenne, ha ezt az algoritmust egy általánosabb algoritmus készítené, hisz valamelyik fázisában a programnak, valahol kell ilyet csinálnia.

Szóval a feldolgozási sor a következő: Picture (ami pixelek halmaza) amiből készítünk PatchBoard-ot (ami Patch-ek halmaza)

Valahogy az algoritmusnak rá kellene jönnie, hogy azonos színű összekapcsolódott pixelhalmazokat hozunk létre.

   ..aaa.a.    ..     .   aaa           a
   ..a...a.    ..     .   a             a
   ..bbbbb. => ..     . +     + bbbbb +
   ........    ........
   ........    ........



*/

} // namespace synth