#pragma once
#include <string>
#include <tuple>

namespace infocell {
namespace arc {

// ============================================================================
enum class ArcColor
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
    Color(ArcColor arcColor, int red, int green, int blue) :
        m_arcColor(arcColor), m_red(red), m_green(green), m_blue(blue) { }

    bool operator==(const Color& rhs) const
    {
        return rhs.m_red == m_red && rhs.m_green == m_green && rhs.m_blue == m_blue;
    }

    bool operator<(const Color& rhs) const
    {
        return std::tie(m_red, m_green, m_blue) < std::tie(rhs.m_red, rhs.m_green, rhs.m_blue);
    }

    ArcColor arcColor() const
    {
        return m_arcColor;
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

    ArcColor m_arcColor;
    uint8_t m_red   = 0;
    uint8_t m_green = 0;
    uint8_t m_blue  = 0;
    uint8_t m_alpha = 0;
};

Color colors(int arcColor);
Color colors(ArcColor arcColor);
std::string getArcColorName(ArcColor arcColor);

} // namespace arc
} // namespace infocell