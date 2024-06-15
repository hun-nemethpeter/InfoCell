#include <fmt/core.h>
#include <memory>    // for make_shared
#include <string>    // for string, wstring

#include "rect.h"      // for string_width, Utf8ToGlyphs, to_string
#include "screen.h"

namespace fsvgui {


Rect::Rect(int width, int height) :
    m_width(width), m_height(height)
{
}

void Rect::ComputeRequirement()
{
    requirement_.min_x = m_width;
    requirement_.min_y = m_height;
}

void Rect::Render(Screen& screen)
{
    const int x = box_.x_min;
    const int y = box_.y_min;
    screen.addSvg(x, y, fmt::format("<rect x=\"{}\" y=\"{}\" width=\"{}\" height=\"{}\" style=\"fill:rgb({}, {}, {})\" />", x, y, m_width, m_height, m_color.red_, m_color.green_, m_color.blue_));
}

TNode<Rect> Rect::color(Color color)
{
    m_color = color;
    return std::static_pointer_cast<Rect>(shared_from_this());
}

TNode<Rect> rect(int width, int height)
{
    return std::make_shared<Rect>(width, height);
}

} // namespace fsvgui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
