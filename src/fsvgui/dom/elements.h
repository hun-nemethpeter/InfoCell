#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "color.h"
#include "flexbox_config.h"
#include "node.h"
#include "screen.h"
#include "text.h"

namespace fsvgui {
class Node;
using Element   = std::shared_ptr<Node>;
using Elements  = std::vector<Element>;
using Decorator = std::function<Element(Element)>;

enum BorderStyle
{
    LIGHT,
    HEAVY,
    DOUBLE,
    ROUNDED,
    EMPTY
};

// Pipe elements into decorator togethers.
// For instance the next lines are equivalents:
// -> text("ftxui") | bold | underlined
// -> underlined(bold(text("FTXUI")))
Element operator|(Element, Decorator);
Element& operator|=(Element&, Decorator);
Elements operator|(Elements, Decorator);
Decorator operator|(Decorator, Decorator);

// --- Widget ---
std::shared_ptr<Text> text(std::string text);

Element border(Element);
Element borderLight(Element);
Element borderHeavy(Element);
Element borderDouble(Element);
Element borderRounded(Element);
Element borderEmpty(Element);
Decorator borderStyled(BorderStyle);
Decorator borderWidth(int width);

Element paragraph(const std::string& text);
Element paragraphAlignLeft(const std::string& text);
Element paragraphAlignRight(const std::string& text);
Element paragraphAlignCenter(const std::string& text);
Element paragraphAlignJustify(const std::string& text);

Element emptyElement();

// -- Decorator ---
Element bold(Element);
Element dim(Element);
Element inverted(Element);
Element underlined(Element);
Decorator color(Color);
Decorator bgcolor(Color);
Element color(Color, Element);
Element bgcolor(Color, Element);

// --- Layout is
// Horizontal, Vertical or stacked set of elements.
Element hbox(Elements);
Element vbox(Elements);
Element dbox(Elements);
Element flexbox(Elements, FlexboxConfig config = FlexboxConfig());
Element gridbox(std::vector<Elements> lines);

Element hflow(Elements); // Helper: default flexbox with row direction.
Element vflow(Elements); // Helper: default flexbox with column direction.

// -- Flexibility ---
// Define how to share the remaining space when not all of it is used inside a
// container.
Element flex(Element);        // Expand/Minimize if possible/needed.
Element flex_grow(Element);   // Expand element if possible.
Element flex_shrink(Element); // Minimize element if needed.

Element xflex(Element);        // Expand/Minimize if possible/needed on X axis.
Element xflex_grow(Element);   // Expand element if possible on X axis.
Element xflex_shrink(Element); // Minimize element if needed on X axis.

Element yflex(Element);        // Expand/Minimize if possible/needed on Y axis.
Element yflex_grow(Element);   // Expand element if possible on Y axis.
Element yflex_shrink(Element); // Minimize element if needed on Y axis.

Element notflex(Element); // Reset the flex attribute.
Element filler();         // A blank expandable element.

// -- Size override;
enum Direction
{
    WIDTH,
    HEIGHT
};
enum Constraint
{
    LESS_THAN,
    EQUAL,
    GREATER_THAN
};
Decorator size(Direction, Constraint, int value);

// --- Misc ---
Decorator reflect(Box& box);
// Before drawing the |element| clear the pixel below. This is useful in
// combinaison with dbox.
Element clear_under(Element element);

// --- Util --------------------------------------------------------------------
Element hcenter(Element);
Element vcenter(Element);
Element center(Element);
Element align_right(Element);
Element nothing(Element element);

namespace Dimension {
Dimensions Fit(Element&);
} // namespace Dimension

} // namespace synth

  // Make container able to take any number of children as input.
#include "take_any_args.h"

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
