#include "Cells.h"
#include "SVGPrinter.h"
#include "SVGStructPrinter.h"
#include "StructPrinter.h"
#include "ValuePrinter.h"
#include <fstream>

using namespace synth;
using namespace synth::cells;

class PrintAs
{
public:
    PrintAs() :
        m_svgPrinter(800, 600),
        m_svgStructPrinter(800, 600)
    {
    }

    ~PrintAs()
    {
        m_svgPrinter.writeFile(std::format("F:\\Devel\\ARC\\synth\\{:02d}.svg", 1));
        m_svgStructPrinter.writeFile(std::format("F:\\Devel\\ARC\\synth\\struct-{:02d}.svg", 1));
    }

    void value(CellI& cell)
    {
        CellValuePrinter valuePrinter;
        cell.accept(valuePrinter);

        std::cout << valuePrinter.print() << std::endl;
    }

    void cell(CellI& cell)
    {
        CellStructPrinter structPrinter;
        cell.accept(structPrinter);

        std::cout << structPrinter.print() << std::endl;
    }

    void svg(CellI& cell, const std::string& caseName = "Case")
    {
        cell.accept(m_svgPrinter);
        m_svgPrinter.showcaseLastResult(caseName);
    }

    void svgStruct(CellI& cell, const std::string& caseName = "Case")
    {
        cell.accept(m_svgStructPrinter);
        m_svgStructPrinter.showcaseLastResult(caseName);
    }

    svg::Printer m_svgPrinter;
    svg::StructPrinter m_svgStructPrinter;
};

using namespace control::pipeline;
using namespace control::op;

int main(int argc, char* argv[])
{
    brain::Brain kb;

    PrintAs printAs;

    input::Picture inputPicture("input");
    inputPicture.loadFromJsonArray("[[0, 7, 0], [7, 7, 7], [0, 7, 0]]");
    hybrid::Picture picture(kb, inputPicture);

    printAs.svg(picture);
    printAs.svg(picture[kb.visualization.pixels]);

    Type Variable(kb, "Color",
                  { { kb.coding.label, kb.type.String },
                    { kb.coding.value, kb.type.Number } });
    Object var1(kb, Variable, "var1");

    Input mainStartNode(kb, picture);
    Node node1(kb, mainStartNode, kb.type.op.Same, mainStartNode, mainStartNode);
    mainStartNode();
    std::cout << "SameOp: ";
    printAs.value(node1[kb.coding.value]);

    Input start(kb, kb.pools.numbers.get(42));
    Fork fork1(kb, start);
    Input value10(kb, kb.pools.numbers.get(10));
    fork1.addBranch(value10);
    Node add10(kb, fork1, kb.type.op.math.Add, fork1, value10);
    start();
    std::cout << "42 + 10 = ";
    printAs.value(add10[kb.coding.value]);

    printAs.value(var1[kb.coding.value]);
    printAs.value(picture[kb.visualization.pixels]);

    Object colorRed(kb, kb.type.Any, "red");
    Object colorGreen(kb, kb.type.Any, "green");
    Object colorBlue(kb, kb.type.Any, "blue");

    Type colorClass(kb, "Color",
                    { { colorRed, kb.type.Number },
                      { colorGreen, kb.type.Number },
                      { colorBlue, kb.type.Number } });

    Object redColor(kb, colorClass, "redColor");
    redColor.set(colorRed, kb.pools.numbers.get(255));
    redColor.set(colorGreen, kb.pools.numbers.get(0));
    redColor.set(colorBlue, kb.pools.numbers.get(0));

    Number& number_0   = kb.pools.numbers.get(0);
    Number& number_255 = kb.pools.numbers.get(255);

    printAs.value(colorClass);
    printAs.value(colorClass[kb.cells.slotMap][colorRed]);
    printAs.value(redColor);
    printAs.value(number_255);
    printAs.value(number_255[kb.coding.value][kb.sequence.first][kb.coding.value]);

    printAs.cell(redColor);
    printAs.cell(colorClass[kb.cells.slotMap][colorRed]);
    printAs.cell(colorClass[kb.cells.slotMap]);
    printAs.svgStruct(colorClass, "Color");
    printAs.svgStruct(colorClass[kb.cells.slotMap], "SlotMap of Color");
    printAs.svgStruct(colorClass[kb.cells.slotList], "SlotList of Color");
    printAs.svgStruct(colorClass[kb.cells.slotList][kb.sequence.first], "SlotListItem1 of Color");
    printAs.value(colorClass[kb.cells.slotMap]);
    printAs.value(colorClass[kb.cells.slotMap][kb.cells.type]);
    printAs.value(colorClass[kb.cells.slotMap][kb.cells.type][kb.cells.slotMap]);
    printAs.cell(colorClass);
    printAs.cell(number_255);
    printAs.cell(number_255[kb.numbers.sign]);
    printAs.cell(number_255[kb.coding.value]);
    printAs.cell(number_255[kb.coding.value][kb.sequence.first]);
    printAs.cell(number_255[kb.coding.value][kb.sequence.first][kb.coding.value]);
    printAs.cell(number_255[kb.coding.value][kb.sequence.last]);
    printAs.cell(number_255[kb.coding.value][kb.dimensions.size]);

    printAs.svg(redColor);
    printAs.svg(colorClass[kb.cells.slotMap][colorRed]);
    printAs.svg(colorClass);
    printAs.svg(number_255);
    printAs.svg(number_255[kb.numbers.sign]);
    printAs.svg(number_255[kb.coding.value]);
    printAs.svg(number_255[kb.coding.value][kb.sequence.first]);
    printAs.svg(number_255[kb.coding.value][kb.sequence.first][kb.coding.value]);
    printAs.svg(number_255[kb.coding.value][kb.sequence.last]);
    printAs.svg(number_255[kb.coding.value][kb.dimensions.size]);
}