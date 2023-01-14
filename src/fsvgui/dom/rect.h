#pragma once
#include <map>
#include <string>

#include "color.h"
#include "node.h"

namespace fsvgui {

class Screen;

class Rect : public Node
{
public:
    explicit Rect(int width, int height);

    void ComputeRequirement() override;
    void Render(Screen& screen) override;

    TNode<Rect> color(Color color);

    static std::map<std::string, const std::string> m_fontPaths;

private:
    int m_width;
    int m_height;
    Color m_color;
};
} // namespace fsvgui