#include <algorithm> // for min
#include <memory>    // for make_shared
#include <string>    // for string, wstring
#include <utility>   // for move
#include <vector>    // for vector

#include "text.h"
#include "box.h"         // for Box
#include "color.h"
#include "elements.h"    // for Element, text, vtext
#include "node.h"        // for Node
#include "requirement.h" // for Requirement
#include "screen.h"      // for Pixel, Screen
#include "string.h"      // for string_width, Utf8ToGlyphs, to_string

#include <ft2build.h>
#include FT_FREETYPE_H

namespace fsvgui {


TextBoxSize stringBB(const std::string& str, int fontSize, const std::string& fontName, const std::string& fontPath)
{
    FT_Library library;
    FT_Face face;
    FT_Error error = FT_Init_FreeType(&library); /* initialize library */

    /* error handling omitted */
    error = FT_New_Face(library, fontPath.c_str(), 0, &face); /* create face object */
    bool hasKerning = FT_HAS_KERNING(face);

    /* use 50pt at 100dpi */
    error = FT_Set_Char_Size(face, fontSize * 64, fontSize * 64, 0, 0); /* set character size */

    FT_Bool use_kerning    = FT_HAS_KERNING(face);
    FT_UInt previous_glyph = 0;

    int pen_x        = 0;
    int maxUpperSize = 0;
    int maxBelowSize = 0;

    for (auto ch : str) {
        FT_UInt glyph_index = FT_Get_Char_Index(face, ch);

        if (use_kerning && previous_glyph && glyph_index) {
            FT_Vector delta;
            FT_Error ft_error = FT_Get_Kerning(face, previous_glyph, glyph_index, FT_KERNING_DEFAULT, &delta);
            if (delta.x != 0 || ft_error != 0) {
                bool haKerning = true;
            }
            pen_x += delta.x >> 6;
        }

        error = FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER);
        if (error)
            continue; /* ignore errors */

        // Dividing by 64 and increment pen position.
        pen_x += face->glyph->advance.x >> 6;

        int belowSize = (face->glyph->metrics.height - face->glyph->metrics.horiBearingY) >> 6;
        if (belowSize > maxBelowSize)
            maxBelowSize = belowSize;

        int upperSize = face->glyph->metrics.horiBearingY >> 6;
        if (upperSize > maxUpperSize)
            maxUpperSize = upperSize;

        // Record current glyph index
        previous_glyph = glyph_index;
    }

    int maxY = maxUpperSize + maxBelowSize;

    return { pen_x, maxY, maxBelowSize - 1 };
}

#if 0
void test()
{
    //    stringBB("ab", 16, "Arial", "C:\\Windows\\Fonts\\arial.ttf");
    stringBB("Hg", 16, "Times New Roman", "C:\\Windows\\Fonts\\times.ttf");
    stringBB("g", 16, "Times New Roman", "C:\\Windows\\Fonts\\times.ttf");
    stringBB("Lorem ipsum dolor sit amet", 18, "Arial", "C:\\Windows\\Fonts\\arial.ttf");
    stringBB("World!", 20, "Times New Roman", "C:\\Windows\\Fonts\\times.ttf");
    stringBB("fgsdfgsd", 20, "Times New Roman", "C:\\Windows\\Fonts\\times.ttf");
    stringBB("lllllllll", 20, "Times New Roman", "C:\\Windows\\Fonts\\times.ttf");
    stringBB("Hello Worsdfgsdgfsdfld!", 22, "Arial", "C:\\Windows\\Fonts\\arial.ttf");
    stringBB("Hellgsdfgdso World!", 24, "Times New Roman", "C:\\Windows\\Fonts\\times.ttf");
    stringBB("QWERTYUIOPASDFGHJKLZXCVBNM", 26, "Times New Roman", "C:\\Windows\\Fonts\\times.ttf");
    stringBB("qwertyuiopasdfghjklzxcvbnm", 28, "Times New Roman", "C:\\Windows\\Fonts\\times.ttf");
}
#endif

Text::Text(std::string text) :
    m_text(std::move(text))
{
    m_fontSize = 25;
    m_fontName = "Times New Roman";
}

void Text::ComputeRequirement()
{
    m_stringBoxSize    = stringBB(m_text, m_fontSize, m_fontName, m_fontPaths[m_fontName]);
    requirement_.min_x = m_stringBoxSize.m_width;
    requirement_.min_y = m_stringBoxSize.m_height;
}

void Text::Render(Screen& screen)
{
    const int x = box_.x_min;
    const int y = box_.y_max - m_stringBoxSize.m_bearing;
    screen.addSvg(x, y, std::format("<text x=\"{}\" y=\"{}\" font-size=\"{}\" fill=\"rgb({}, {}, {})\">{}</text>", x, y, m_fontSize, m_fontColor.red_, m_fontColor.green_, m_fontColor.blue_, m_text));
}

std::shared_ptr<Text> Text::fontSize(int size)
{
    m_fontSize = size;
    return std::static_pointer_cast<Text>(shared_from_this());
}

std::shared_ptr<Text> Text::fontName(const std::string& name)
{
    m_fontName = name;
    return std::static_pointer_cast<Text>(shared_from_this());
}

std::shared_ptr<Text> Text::fontColor(Color color)
{
    m_fontColor = color;
    return std::static_pointer_cast<Text>(shared_from_this());
}


std::map<std::string, const std::string> Text::m_fontPaths = {
    { "Arial", "C:\\Windows\\Fonts\\arial.ttf" },
    { "Times New Roman", "C:\\Windows\\Fonts\\times.ttf" }
};

class VText : public Node
{
public:
    explicit VText(std::string text) :
        m_text(std::move(text))
    {
        m_fontSize = 16;
        m_fontName = "Times New Roman";
    }

    void ComputeRequirement() override
    {
        TextBoxSize boxSize    = stringBB(m_text, m_fontSize, m_fontName, Text::m_fontPaths[m_fontName]);
        requirement_.min_x = boxSize.m_height;
        requirement_.min_y = boxSize.m_width;
    }

    void Render(Screen& screen) override
    {
        // TODO
    }

private:
    std::string m_text;
    int m_fontSize;
    std::string m_fontName;
};

/// @brief Display a piece of UTF8 encoded unicode text.
/// @ingroup dom
/// @see ftxui::to_wstring
///
/// ### Example
///
/// ```cpp
/// Element document = text("Hello world!");
/// ```
///
/// ### Output
///
/// ```bash
/// Hello world!
/// ```
TNode<Text> text(std::string text)
{
    return std::make_shared<Text>(std::move(text));
}

/// @brief Display a piece of unicode text vertically.
/// @ingroup dom
/// @see ftxui::to_wstring
///
/// ### Example
///
/// ```cpp
/// Element document = vtext("Hello world!");
/// ```
///
/// ### Output
///
/// ```bash
/// H
/// e
/// l
/// l
/// o
///
/// w
/// o
/// r
/// l
/// d
/// !
/// ```
Element vtext(std::string text)
{
    return std::make_shared<VText>(std::move(text));
}

} // namespace fsvgui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
