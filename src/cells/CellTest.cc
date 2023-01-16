#include "Cells.h"
#include "StructPrinter.h"
#include "SVGPrinter.h"
#include "ValuePrinter.h"
#include <fstream>

using namespace synth;
using namespace synth::cells;
using namespace synth::cells::data;

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
    StaticInitializations();
    PrintAs printAs;

    input::Picture inputPicture("input");
    inputPicture.loadFromJsonArray("[[0, 7, 0], [7, 7, 7], [0, 7, 0]]");
    hybrid::Sensor sensor(inputPicture);

    printAs.svg(sensor);
    printAs.svg(sensor[data::listOfPixels]);


    Type Variable("Color",
                  { { "name", String::t(), data::coding::name },
                    { "value", Number::t(), data::coding::value } });
    Object var1("var1", Variable);

    Start mainStartNode(sensor);
    Node node1(mainStartNode, Same::t(), mainStartNode, mainStartNode);
    mainStartNode();
    std::cout << "SameOp: ";
    printAs.value(node1[data::coding::value]);

    Start start(Numbers::get(42));
    Fork fork1(start);
    Start value10(Numbers::get(10));
    fork1.addBranch(value10);
    Node add10(fork1, math::Add::t(), fork1, value10);
    start();
    std::cout << "42 + 10 = ";
    printAs.value(add10[data::coding::value]);

    printAs.value(var1[data::coding::value]);
    printAs.value(sensor[data::listOfPixels]);


    Object colorRed(Type::anyType());
    Object colorGreen(Type::anyType());
    Object colorBlue(Type::anyType());

    Type colorClass("Color",
                    { { "red", Number::t(), colorRed },
                      { "green", Number::t(), colorGreen },
                      { "blue", Number::t(), colorBlue } });

    Object redColor("redColor", colorClass);
    redColor.set(colorRed, Numbers::get(255));
    redColor.set(colorGreen, Numbers::get(0));
    redColor.set(colorBlue, Numbers::get(0));

    Number& number_0   = Numbers::get(0);
    Number& number_255 = Numbers::get(255);

    printAs.value(colorClass);
    printAs.value(colorClass.getSlot("red"));
    printAs.value(redColor);
    printAs.value(number_255);
    printAs.value(number_255[value][first][value]);

    printAs.cell(redColor);
    printAs.cell(colorClass.getSlot("red"));
    printAs.cell(colorClass);
    printAs.cell(number_255);
    printAs.cell(number_255[sign]);
    printAs.cell(number_255[value]);
    printAs.cell(number_255[value][first]);
    printAs.cell(number_255[value][first][value]);
    printAs.cell(number_255[value][last]);
    printAs.cell(number_255[value][size]);

    printAs.svg(redColor);
    printAs.svg(colorClass.getSlot("red"));
    printAs.svg(colorClass);
    printAs.svg(number_255);
    printAs.svg(number_255[sign]);
    printAs.svg(number_255[value]);
    printAs.svg(number_255[value][first]);
    printAs.svg(number_255[value][first][value]);
    printAs.svg(number_255[value][last]);
    printAs.svg(number_255[value][size]);
}