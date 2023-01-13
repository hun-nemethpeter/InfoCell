#include <memory>   // for make_shared
#include <utility>  // for move

#include "elements.h"  // for Element, Decorator, bgcolor, color
#include "node_decorator.h"  // for NodeDecorator
#include "box.h"          // for Box
#include "color.h"        // for Color

namespace fsvgui {

Color::Color(uint8_t red, uint8_t green, uint8_t blue) :
    red_(red), green_(green), blue_(blue), alpha_(0)
{
}

bool Color::operator==(const Color& rhs) const
{
    return red_ == rhs.red_ && green_ == rhs.green_ && blue_ == rhs.blue_ && alpha_ == rhs.alpha_;
}

bool Color::operator!=(const Color& rhs) const
{
    return !operator==(rhs);
}

class BgColor : public NodeDecorator
{
public:
    BgColor(Element child, Color color) :
        NodeDecorator(std::move(child)), color_(color) { }

    void Render(Screen& screen) override
    {
        NodeDecorator::Render(screen);
    }

    Color color_;
};

class FgColor : public NodeDecorator
{
public:
    FgColor(Element child, Color color) :
        NodeDecorator(std::move(child)), color_(color) { }

    void Render(Screen& screen) override
    {
        NodeDecorator::Render(screen);
    }

    Color color_;
};

/// @brief Set the foreground color of an element.
/// @param color The color of the output element.
/// @param child The input element.
/// @return The output element colored.
/// @ingroup dom
///
/// ### Example
///
/// ```cpp
/// Element document = color(Color::Green, text("Success")),
/// ```
Element color(Color color, Element child)
{
    return std::make_shared<FgColor>(std::move(child), color);
}

/// @brief Set the background color of an element.
/// @param color The color of the output element.
/// @param child The input element.
/// @return The output element colored.
/// @ingroup dom
///
/// ### Example
///
/// ```cpp
/// Element document = bgcolor(Color::Green, text("Success")),
/// ```
Element bgcolor(Color color, Element child)
{
    return std::make_shared<BgColor>(std::move(child), color);
}

/// @brief Decorate using a foreground color.
/// @param c The foreground color to be applied.
/// @return The Decorator applying the color.
/// @ingroup dom
///
/// ### Example
///
/// ```cpp
/// Element document = text("red") | color(Color::Red);
/// ```
Decorator color(Color c)
{
    return [c](Element child) { return color(c, std::move(child)); };
}

/// @brief Decorate using a background color.
/// @param color The background color to be applied.
/// @return The Decorator applying the color.
/// @ingroup dom
///
/// ### Example
///
/// ```cpp
/// Element document = text("red") | bgcolor(Color::Red);
/// ```
Decorator bgcolor(Color color)
{
    return [color](Element child) { return bgcolor(color, std::move(child)); };
}

} // namespace fsvgui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
