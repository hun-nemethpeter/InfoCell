#include "Picture.h"

#include <nlohmann/json.hpp>

namespace synth {
namespace input {

const std::array<Color, 10> arcColors = {
    Color(0x00, 0x00, 0x00), /* black */
    Color(0x00, 0x74, 0xD9), /* blue */
    Color(0xFF, 0x41, 0x36), /* red */
    Color(0x2E, 0xCC, 0x40), /* green */
    Color(0xFF, 0xDC, 0x00), /* yellow */
    Color(0xAA, 0xAA, 0xAA), /* grey */
    Color(0xF0, 0x12, 0xBE), /* fuschia */
    Color(0xFF, 0x85, 0x1B), /* orange */
    Color(0x7F, 0xDB, 0xFF), /* teal */
    Color(0x87, 0x0C, 0x25)  /* brown */
};

Picture::Picture(const std::string& name) :
    m_name(name)
{
}
Picture::Picture(const std::string& name, const std::string& jsonStr) :
    m_name(name)
{
    loadFromJsonArray(jsonStr);
}

Picture& Picture::loadFromJsonArray(const std::string& jsonStr)
{
    nlohmann::json arcMatrix = nlohmann::json::parse(jsonStr);
    size_t matrixHeight = arcMatrix.size();
    size_t matrixWidth  = arcMatrix[0].size();

    m_width  = (int)matrixWidth;
    m_height = (int)matrixHeight;

    int y = 0;
    for (auto inputRowIt = arcMatrix.begin(); inputRowIt != arcMatrix.end(); ++inputRowIt) {
        for (const int val : *inputRowIt) {
            m_pixels.push_back(arcColors[val]);
        }
    }

    return *this;
}

Picture& Picture::loadFromVector(int width, int height, const std::vector<int>& pixelColors)
{
    m_width  = width;
    m_height = height;
    m_pixels.clear();
    for (int pixelColor : pixelColors) {
        m_pixels.push_back(arcColors[pixelColor]);
    }

    return *this;
}

const std::string& Picture::name() const
{
    return m_name;
}

int Picture::width() const
{
    return m_width;
}

int Picture::height() const
{
    return m_height;
}

std::vector<Color>& Picture::pixels()
{
    return m_pixels;
}

const std::vector<Color>& Picture::pixels() const
{
    return m_pixels;
}

} // namespace input
} // namespace synth
