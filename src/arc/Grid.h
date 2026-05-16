#pragma once

#include <string>
#include <vector>

#include "Color.h"

namespace infocell {
namespace arc {
namespace input {

// ============================================================================
class Grid
{
public:
    Grid(const std::string& name);
    Grid(const std::string& name, const std::string& jsonStr);

    Grid& loadFromJsonArray(const std::string& jsonStr);
    Grid& loadFromVector(int width, int height, const std::vector<int>& pixels);
    const std::string& label() const;
    int width() const;
    int height() const;
    std::vector<Color>& pixels();
    const std::vector<Color>& pixels() const;

private:
    std::string m_name;
    int m_width  = 0;
    int m_height = 0;
    std::vector<Color> m_pixels;
};

} // namespace input
} // namespace arc
} // namespace infocell