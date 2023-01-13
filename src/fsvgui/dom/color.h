#pragma once

#include <cstdint> // for uint8_t
#include <string>  // for wstring

namespace fsvgui {

/// @brief A class representing terminal colors.
/// @ingroup screen
class Color
{
public:
    Color() = default;
    Color(uint8_t red, uint8_t green, uint8_t blue);
    static Color RGB(uint8_t red, uint8_t green, uint8_t blue);
    static Color HSV(uint8_t hue, uint8_t saturation, uint8_t value);
    static Color Interpolate(float t, const Color& a, const Color& b);

    // --- Operators ------
    bool operator==(const Color& rhs) const;
    bool operator!=(const Color& rhs) const;

    std::string Print(bool is_background_color) const;

    uint8_t red_   = 0;
    uint8_t green_ = 0;
    uint8_t blue_  = 0;
    uint8_t alpha_ = 0;
};

inline namespace literals {

/// @brief Creates a color from a combined hex RGB representation,
/// e.g. 0x808000_rgb
Color operator""_rgb(unsigned long long int combined);

} // namespace literals

} // namespace fsvgui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
