#pragma once
#include <string>
#include <tuple>

namespace infocell {
namespace arc {

// ============================================================================
enum class ColorId
{
    black,
    blue,
    red,
    green,
    yellow,
    grey,
    fuschia,
    orange,
    teal,
    brown,
    alpha
};

// ============================================================================
class Color
{
public:
    Color(ColorId id, int red, int green, int blue) :
        m_id(id), m_red(red), m_green(green), m_blue(blue) { }

    bool operator==(const Color& rhs) const
    {
        return rhs.m_id == m_id;
    }

    bool operator<(const Color& rhs) const
    {
        return rhs.m_id < m_id;
    }

    ColorId id() const
    {
        return m_id;
    }

    uint8_t red() const
    {
        return m_red;
    }

    uint8_t green() const
    {
        return m_green;
    }

    uint8_t blue() const
    {
        return m_blue;
    }

    uint8_t alpha() const
    {
        return m_alpha;
    }

    ColorId m_id;
    uint8_t m_red   = 0;
    uint8_t m_green = 0;
    uint8_t m_blue  = 0;
    uint8_t m_alpha = 0;
};

Color colors(int arcColor);
Color colors(ColorId arcColor);
std::string getArcColorName(ColorId arcColor);

} // namespace arc
} // namespace infocell