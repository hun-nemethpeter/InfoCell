#pragma once
#include <map>
#include <string>

#include "color.h"
#include "node.h"

namespace fsvgui {

class Screen;

class TextBoxSize
{
public:
    int m_width;
    int m_height;
    int m_bearing;
};

class Text : public Node
{
public:
    explicit Text(std::string text);

    void ComputeRequirement() override;
    void Render(Screen& screen) override;

    std::shared_ptr<Text> fontSize(int size);
    std::shared_ptr<Text> fontName(const std::string& m_fontName);
    std::shared_ptr<Text> fontColor(Color color);

    static std::map<std::string, const std::string> m_fontPaths;

private:
    std::string m_text;
    int m_fontSize;
    std::string m_fontName;
    Color m_fontColor;
    TextBoxSize m_stringBoxSize;
};
} // namespace fsvgui