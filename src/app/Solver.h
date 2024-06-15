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
    Vector() :
        x(0), y(0)
    {
    }

    Vector(int x, int y) :
        x(x), y(y)
    {
    }

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
class Shape
{
public:
    Shape(int id, input::Color color, int width, int height) :
        m_id(id), m_color(color), m_width(width), m_height(height) { }

    void addPixel(cells::hybrid::Pixel& pixel);
    bool hasPixel(cells::hybrid::Pixel& pixel) const;
    void sortPixels();

    int id() const
    {
        return m_id;
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

    bool operator<(const Shape& rhs) const
    {
        return firstPixelIndex() < rhs.firstPixelIndex();
    }

    std::string toString() const;
    VectorShape toVectorShape() const;

private:
    int m_id = 0;
    input::Color m_color;
    const int m_width;
    const int m_height;
    std::set<cells::hybrid::Pixel*> m_hybridPixels;
    std::vector<Pixel> m_pixels;
};

// ============================================================================
class Shaper
{
public:
    Shaper(const cells::hybrid::Picture& picture);

    void process();

    const std::vector<std::shared_ptr<Shape>>& shapes() const
    {
        return m_shapes;
    }

protected:
    void processInputPixels();
    void processPixel(Shape& shape, std::set<cells::hybrid::Pixel*>& checkPixels, cells::hybrid::Pixel& checkPixel);
    cells::hybrid::Pixel* processAdjacentPixel(cells::CellI& direction, Shape& shape, std::set<cells::hybrid::Pixel*>& checkPixels, cells::hybrid::Pixel& checkPixel);

    cells::brain::Brain& kb;
    const int m_width;
    const int m_height;
    const cells::hybrid::Picture& m_picture;
    std::set<cells::hybrid::Pixel*> m_inputPixels;
    std::vector<std::shared_ptr<Shape>> m_shapes;
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
    Grid(std::vector<std::shared_ptr<Shape>> shapes);

private:
    std::vector<VectorShape> vectorShapes;
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