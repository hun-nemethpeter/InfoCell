#include "Cells.h"
#include "Brain.h"
#include "StructPrinter.h"
#include "SVGPrinter.h"
#include "ValuePrinter.h"
#include <fstream>

using namespace synth;
using namespace synth::cells;

class PrintAs
{
public:
    PrintAs() :
        m_svgPrinter(800, 600)
    {
    }

    ~PrintAs()
    {
        m_svgPrinter.writeFile(std::format("F:\\Devel\\ARC\\synth\\{:02d}.svg", 1));
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

    svg::Printer m_svgPrinter;
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
                  { { "name", kb.type.String, kb.coding.name },
                    { "value", kb.type.Number, kb.coding.value } });
    Object var1(kb, "var1", Variable);

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


    Object colorRed(kb, kb.type.Any);
    Object colorGreen(kb, kb.type.Any);
    Object colorBlue(kb, kb.type.Any);

    Type colorClass(kb, "Color",
                    { { "red", kb.type.Number, colorRed },
                      { "green", kb.type.Number, colorGreen },
                      { "blue", kb.type.Number, colorBlue } });

    Object redColor(kb, "redColor", colorClass);
    redColor.set(colorRed, kb.pools.numbers.get(255));
    redColor.set(colorGreen, kb.pools.numbers.get(0));
    redColor.set(colorBlue, kb.pools.numbers.get(0));

    Number& number_0   = kb.pools.numbers.get(0);
    Number& number_255 = kb.pools.numbers.get(255);

    printAs.value(colorClass);
    printAs.value(colorClass.getSlot("red"));
    printAs.value(redColor);
    printAs.value(number_255);
    printAs.value(number_255[kb.coding.value][kb.sequence.first][kb.coding.value]);

    printAs.cell(redColor);
    printAs.cell(colorClass.getSlot("red"));
    printAs.cell(colorClass);
    printAs.cell(number_255);
    printAs.cell(number_255[kb.numbers.sign]);
    printAs.cell(number_255[kb.coding.value]);
    printAs.cell(number_255[kb.coding.value][kb.sequence.first]);
    printAs.cell(number_255[kb.coding.value][kb.sequence.first][kb.coding.value]);
    printAs.cell(number_255[kb.coding.value][kb.sequence.last]);
    printAs.cell(number_255[kb.coding.value][kb.dimensions.size]);

    printAs.svg(redColor);
    printAs.svg(colorClass.getSlot("red"));
    printAs.svg(colorClass);
    printAs.svg(number_255);
    printAs.svg(number_255[kb.numbers.sign]);
    printAs.svg(number_255[kb.coding.value]);
    printAs.svg(number_255[kb.coding.value][kb.sequence.first]);
    printAs.svg(number_255[kb.coding.value][kb.sequence.first][kb.coding.value]);
    printAs.svg(number_255[kb.coding.value][kb.sequence.last]);
    printAs.svg(number_255[kb.coding.value][kb.dimensions.size]);
}