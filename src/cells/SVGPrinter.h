#include <filesystem>
#include <stack>

#include "Cells.h"
#include "fsvgui/dom/elements.h"

namespace synth {
namespace cells {
namespace svg {

class Point
{
public:
    int x;
    int y;
};

class Printer : public cells::Visitor
{
public:
    Printer(int width, int height);

    void visit(Slot& cell) override;
    void visit(SlotMapType& cell) override;
    void visit(SlotMap& cell) override;
    void visit(Type& cell) override;
    void visit(Object& cell) override;
    void visit(ListItem& cell) override;
    void visit(List& cell) override;
    void visit(Number& cell) override;
    void visit(String& cell) override;
    void visit(hybrid::Color& cell) override;
    void visit(hybrid::Pixel& cell) override;
    void visit(hybrid::Picture& cell) override;

    void showcaseLastResult(const std::string& caseName);
    std::string print();
    void writeFile(const std::filesystem::path& path);

protected:
    fsvgui::Screen processResult();

    int m_width;
    int m_height;
    int m_fontSize;
    std::string m_fontName;
    std::stack<fsvgui::Element> m_stack;
    fsvgui::Elements m_showcaseItems;
};

} // namespace svg
} // namespace cells
} // namespace synth