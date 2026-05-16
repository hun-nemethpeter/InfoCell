#include "Grid.h"

#include <nlohmann/json.hpp>

namespace infocell {
namespace arc {
namespace input {

Grid::Grid(const std::string& name) :
    m_name(name)
{
}
Grid::Grid(const std::string& name, const std::string& jsonStr) :
    m_name(name)
{
    loadFromJsonArray(jsonStr);
}

Grid& Grid::loadFromJsonArray(const std::string& jsonStr)
{
    nlohmann::json arcMatrix = nlohmann::json::parse(jsonStr);
    size_t matrixHeight      = arcMatrix.size();
    size_t matrixWidth       = arcMatrix[0].size();

    m_width  = (int)matrixWidth;
    m_height = (int)matrixHeight;

    int y = 0;
    for (auto inputRowIt = arcMatrix.begin(); inputRowIt != arcMatrix.end(); ++inputRowIt) {
        for (const int val : *inputRowIt) {
            m_pixels.push_back(colors(val));
        }
    }

    return *this;
}

Grid& Grid::loadFromVector(int width, int height, const std::vector<int>& pixelColors)
{
    m_width  = width;
    m_height = height;
    m_pixels.clear();
    for (int pixelColor : pixelColors) {
        m_pixels.push_back(colors(pixelColor));
    }

    return *this;
}

const std::string& Grid::label() const
{
    return m_name;
}

int Grid::width() const
{
    return m_width;
}

int Grid::height() const
{
    return m_height;
}

std::vector<Color>& Grid::pixels()
{
    return m_pixels;
}

const std::vector<Color>& Grid::pixels() const
{
    return m_pixels;
}

} // namespace input
} // namespace arc
} // namespace infocell