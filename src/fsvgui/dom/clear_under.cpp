#include <fmt/core.h>
#include <memory>  // for make_shared
#include <utility> // for move

#include "box.h"            // for Box
#include "elements.h"       // for Element, clear_under
#include "node.h"           // for Node
#include "node_decorator.h" // for NodeDecorator
#include "screen.h"         // for Pixel, Screen

namespace fsvgui {

using fsvgui::Screen;

class ClearUnder : public NodeDecorator
{
public:
    using NodeDecorator::NodeDecorator;

    void Render(Screen& screen) override
    {
        const int x = box_.x_min;
        const int y = box_.y_min;
        screen.addSvg(x, y, fmt::format("<rect x=\"{}\" y=\"{}\" width=\"{}\" height=\"{}\" style=\"fill:rgb(255, 255, 255)\" />\n", x, y, box_.width(), box_.height()));
        Node::Render(screen);
    }
};

/// @brief Before drawing |child|, clear the pixels below. This is useful in
//         combinaison with dbox.
/// @see ftxui::dbox
/// @ingroup dom
Element clear_under(Element element)
{
    return std::make_shared<ClearUnder>(std::move(element));
}

} // namespace fsvgui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
