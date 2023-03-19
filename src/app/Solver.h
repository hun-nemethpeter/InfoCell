#pragma once

#include "Picture.h"
#include "cells/Brain.h"
#include "util/ArcTask.h"
#include "util/Logging.h"

#include <set>

namespace synth {

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

extern const std::array<input::Color, 10> cellColors;
extern const std::map<const input::Color, int> cellColorsToColorId;
const input::Color& color(arc::Colors c);

class Vector;

// ============================================================================
class Pixel
{
public:
    Pixel() :
        x(0), y(0)
    {
    }

    Pixel(int x, int y) :
        x(x), y(y)
    {
    }

    Pixel operator+(const Vector& rhs) const;
    Pixel& operator+=(const Vector& rhs);
    Pixel& operator-=(const Vector& rhs);

    bool operator<(const Pixel& rhs) const
    {
        return std::tie(x, y) < std::tie(rhs.x, rhs.y);
    }

    int x;
    int y;
};

std::ostream& operator<<(std::ostream& os, const Pixel& pixel);

// ============================================================================
class Vector
{
public:
    bool operator==(const Vector& rhs) const
    {
        return std::tie(x, y) == std::tie(rhs.x, rhs.y);
    }

    bool operator<(const Vector& rhs) const
    {
        return std::tie(x, y) < std::tie(rhs.x, rhs.y);
    }

    Vector operator+(const Vector& rhs) const
    {
        return Vector({ x + rhs.x, y + rhs.y });
    }

    Vector operator-() const
    {
        return { -x, -y };
    }

    Vector operator-(const Vector& rhs) const
    {
        return Vector({ x - rhs.x, y - rhs.y });
    }

    Vector& operator+=(const Vector& rhs);
    Vector& operator-=(const Vector& rhs);

    Vector rotate(RotationDir rotationDir) const;

    Vector mirrorY() const
    {
        return { -x, y };
    }

    Vector mirrorX() const
    {
        return { x, -y };
    }

    int x = 0;
    int y = 0;
};

std::ostream& operator<<(std::ostream& os, const Vector& vector);

// ============================================================================
class Box
{
public:
    int width() const
    {
        return rightX - leftX + 1;
    }

    int height() const
    {
        return bottomY - topY + 1;
    }

    Pixel topLeftPixel() const
    {
        return Pixel(leftX, topY);
    }

    Pixel topRightPixel() const
    {
        return Pixel(rightX, topY);
    }

    Pixel bottomLeftPixel() const
    {
        return Pixel(leftX, bottomY);
    }

    Pixel bottomRightPixel() const
    {
        return Pixel(rightX, bottomY);
    }

    int leftX   = 0;
    int topY    = 0;
    int rightX  = 0;
    int bottomY = 0;
};

class VectorShape;
// ============================================================================
class BoundingBox : public Box
{
public:
    BoundingBox(const std::vector<Pixel>& pixels)
    {
        fromPixels(pixels);
    }

    BoundingBox(const VectorShape& vektorShape);
    BoundingBox(const Pixel& firstPixel, const std::vector<Vector>& vectors)
    {
        fromVectors(firstPixel, vectors);
    }

    const Vector& toFirstPixel() const
    {
        return m_toFirstPixel;
    }

    const Vector& toLastPixel() const
    {
        return m_toLastPixel;
    }

private:
    void fromPixels(const std::vector<Pixel>& pixels);
    void fromVectors(const Pixel& firstPixel, const std::vector<Vector>& vectors);
    void update(const Pixel& pixel);
    void firstPixelVector(const Pixel& pixel);
    void lastPixelVector(const Pixel& pixel);

    Vector m_toFirstPixel;
    Vector m_toLastPixel;
};

enum class DistanceType
{
    FromTopLeft,
    FromTopRight,
    FromBottomLeft,
    FromBottomRight,
    FromFirstPixel,
    FromLastPixel
};

// ============================================================================
class VectorShapeRelation
{
public:
    bool isUnrelated() const
    {
        return !exactMatch && !isRotated && !isMirrored && !isStreched;
    }

    bool exactMatch = false;
    bool isRotated  = false;

    // lhs.rotate(rotationDir) == rhs
    RotationDir rotationDir;
    bool isMirrored = false;
    RotationDir mirrorAxisDir;
    bool isStreched = false;
    int stretchHorizontal;
    int strechVertical;
};

// ============================================================================
class VectorShape
{
public:
    VectorShape(const input::Color& color) :
        m_color(color)
    {
    }

    VectorShape(const std::vector<Vector>& vectors, const input::Color& color, const Pixel& firstPixel) :
        m_vectors(vectors), m_color(color), m_firstPixel(firstPixel)
    {
    }

    VectorShape(std::vector<Vector>&& vectors, const input::Color& color, const Pixel& firstPixel) :
        m_vectors(std::move(vectors)), m_color(color), m_firstPixel(firstPixel)
    {
    }

    void fromPixels(const std::vector<Pixel>& pixels);
    VectorShape rotate(RotationDir rotationDir) const;
    VectorShape stretch(int horizontal, int vertical) const;
    VectorShape mirror(DistanceType distanceType, const Vector distance, RotationDir axisDir) const;
    VectorShapeRelation compare(const VectorShape& other);

    const input::Color& color() const
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

    Pixel calculatePixel(DistanceType distanceType, const Vector distance) const;

private:
    static void stretchPixel(std::vector<Vector>& stretchVectors, int horizontal, int vertical);

    input::Color m_color;
    std::vector<Vector> m_vectors;
    Pixel m_firstPixel;
};

// ============================================================================
class Patch;
class PatchBoardI
{
public:
    virtual void subscribePatchForPixel(std::shared_ptr<Patch> patch, int x, int y)       = 0;
    virtual void mergePatch(std::shared_ptr<Patch> winner, std::shared_ptr<Patch> looser) = 0;

    virtual int width() const  = 0;
    virtual int height() const = 0;
};

// ============================================================================
class Patch : public std::enable_shared_from_this<Patch>
{
public:
    Patch(input::Color color, PatchBoardI* patchBoardI) :
        m_color(color), m_patchBoardI(patchBoardI), m_width(patchBoardI->width()), m_height(patchBoardI->height()) { }

    void registerSubscribedPixel(int x, int y)
    {
        m_subscribedPixels.insert({ x, y });
    }

    void addPixelCoordinate(int x, int y);
    void merge(std::shared_ptr<Patch> other);
    void sortPixels();

    int id() const
    {
        return m_id;
    }

    void id(int id)
    {
        m_id = id;
    }

    const input::Color& color() const
    {
        return m_color;
    }

    size_t size() const
    {
        return m_pixels.size();
    }

    const Pixel& firstPixel() const
    {
        return m_pixels.front();
    }

    const Pixel& lastPixel() const
    {
        return m_pixels.back();
    }

    const std::vector<Pixel>& pixels() const
    {
        return m_pixels;
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

    std::string toString() const;
    VectorShape toVectorShape() const;

    const std::set<Pixel>& subscribedPixels() const
    {
        return m_subscribedPixels;
    }

private:
    int m_id = 0;
    input::Color m_color;
    PatchBoardI* m_patchBoardI = nullptr;
    const int m_width;
    const int m_height;
    std::vector<Pixel> m_pixels;
    std::set<Pixel> m_subscribedPixels;
};

// ============================================================================
struct PatchSlot
{
    PatchSlot(PatchBoardI* patchBoardI) :
        m_patchBoardI(patchBoardI) { }

    void registerCandidate(std::shared_ptr<Patch> patch);
    void unRegisterCandidate(std::shared_ptr<Patch> patch);
    std::shared_ptr<Patch> getCandidate(const input::Color& color);

private:
    PatchBoardI* m_patchBoardI = nullptr;
    std::map<input::Color, std::set<std::shared_ptr<Patch>>> m_candidates;
};

// ============================================================================
class PatchBoard : public PatchBoardI
{
public:
    PatchBoard(const cells::hybrid::Picture& picture) :
        m_width(picture.width()), m_height(picture.height()), m_picture(picture), m_patchSlots(m_height * m_width, PatchSlot(this))
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

    void process();
    void processPixel(int x, int y, const input::Color& color);

    const std::set<std::shared_ptr<Patch>>& patches() const
    {
        return m_patches;
    }

    void subscribePatchForPixel(std::shared_ptr<Patch> patch, int x, int y) override;
    void mergePatch(std::shared_ptr<Patch> winner, std::shared_ptr<Patch> looser) override;

protected:
    void subscribePatchForPixelImpl(std::shared_ptr<Patch> patch, int x, int y);

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
    const cells::hybrid::Picture& m_picture;
    std::vector<PatchSlot> m_patchSlots;
    std::set<std::shared_ptr<Patch>> m_patches;
};

// ============================================================================
class DrawingBoard
{
public:
    DrawingBoard(int width, int height);

    void clear();
    void setColor(int x, int y, input::Color color);
    void renderLine(int x, int y, const input::Color& color, RotationDir direction);
    void renderLine(int x, int y, int len, const input::Color& color, RotationDir direction);
    void renderVectorShape(const VectorShape& vectorShape)
    {
        renderVectorShape(vectorShape.firstPixel().x, vectorShape.firstPixel().y, vectorShape);
    }

    void renderVectorShape(int x, int y, const VectorShape& vectorShape);
    void renderBoundingBox(const BoundingBox& boundingBox);

    std::string toString() const;

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
    const input::Color& m_defaultBgColor;
    std::vector<input::Color> m_colors;
};

// ============================================================================
class Grid
{
public:
    Grid(std::set<std::shared_ptr<Patch>> patches);

private:
    std::vector<VectorShape> shapes;
};

// ============================================================================
class Rules
{
public:
    std::string a; // TODO placeholder
};

class Code
{
public:
    std::string a; // TODO placeholder
};

// ============================================================================
class Solver
{
public:
    Solver(Logger& logger, const ArcTask& arcTask);

    void solve();

private:
    Grid parse(const cells::hybrid::Picture& picture);
    Rules gridDiff(const Grid& input, const Grid& output);
    Code processRules(const std::vector<Rules>& rules);
    DrawingBoard applyCode(const Grid& input, const Code& code);

    Logger& logger;
    const ArcTask& m_arcTask;
};

} // namespace synth