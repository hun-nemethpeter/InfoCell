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

    void value(CellI& cell, const std::string& label = "")
    {
        CellValuePrinter valuePrinter;
        cell.accept(valuePrinter);

        if (!label.empty()) {
            std::cout << label << ": ";
        }

        std::cout << valuePrinter.print() << std::endl;
    }

    void cell(CellI& cell, const std::string& label = "")
    {
        CellStructPrinter structPrinter;
        cell.accept(structPrinter);

        if (!label.empty()) {
            std::cout << label << ": ";
        }

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

using namespace control;

int main(int argc, char* argv[])
{
    brain::Brain kb;
    auto& cells = kb.cells;

    PrintAs printAs;

    input::Picture inputPicture("input");
    inputPicture.loadFromJsonArray("[[0, 7, 0], [7, 7, 7], [0, 7, 0]]");
    hybrid::Picture picture(kb, inputPicture);

    printAs.svg(picture);
    printAs.svg(picture[kb.visualization.pixels]);

    Type Variable(kb, "Color");
    Variable.addSlots(
        cells.slot(kb.coding.label, kb.type.String),
        cells.slot(kb.coding.value, kb.type.Number));

    Object var1(kb, Variable, "var1");

    Ref mainStartNode(kb, picture);
    Same node1(kb, mainStartNode, mainStartNode);
    node1();
    std::cout << "SameOp: ";
    printAs.value(node1[kb.coding.value]);

    Ref start(kb, kb.pools.numbers.get(42));
    Ref value10(kb, kb.pools.numbers.get(10));
    Add add10(kb, start, value10);
    add10();
    std::cout << "42 + 10 = ";
    printAs.value(add10[kb.coding.value]);

    printAs.value(var1[kb.coding.value]);
    printAs.value(picture[kb.visualization.pixels]);

    Object colorRed(kb, kb.type.Any, "red");
    Object colorGreen(kb, kb.type.Any, "green");
    Object colorBlue(kb, kb.type.Any, "blue");

    Type colorClass(kb, "Color");
    colorClass.addSlots(
        cells.slot(colorRed, kb.type.Number),
        cells.slot(colorGreen, kb.type.Number),
        cells.slot(colorBlue, kb.type.Number));

    Object redColor(kb, colorClass, "redColor");
    redColor.set(colorRed, kb.pools.numbers.get(255));
    redColor.set(colorGreen, kb.pools.numbers.get(0));
    redColor.set(colorBlue, kb.pools.numbers.get(0));

    Number& number_0   = kb.pools.numbers.get(0);
    Number& number_255 = kb.pools.numbers.get(255);

    CellI& templateListParamType = kb.templates.list.getParamType();
    Object templateListParam(kb, templateListParamType);
    templateListParam.set(kb.coding.objectType, kb.type.Digit);
    CellI& templateListType = kb.templates.list.compile(templateListParam);
    printAs.value(templateListType);
    printAs.cell(templateListType);
    printAs.value(templateListType[kb.cells.subTypes][kb.cells.index][kb.coding.objectType]);

    Object listAddInput(kb, kb.listAdd.inputType());
    printAs.value(listAddInput, "listAddInput");
    listAddInput.set(kb.coding.value, number_255);
    printAs.cell(listAddInput, "listAddInput set");
    CellI& listAddCompiled = kb.listAdd.compile();
    printAs.cell(listAddCompiled, "listAddCompiled");

    printAs.value(colorClass);
    printAs.value(colorClass[kb.cells.slots][kb.cells.index][colorRed]);
    printAs.value(redColor);
    printAs.value(number_255);
    printAs.value(number_255[kb.coding.value][kb.sequence.first][kb.coding.value]);

    printAs.cell(redColor);
    printAs.cell(colorClass[kb.cells.slots][kb.cells.index][colorRed]);
    printAs.cell(colorClass[kb.cells.slots][kb.cells.index]);
    printAs.svgStruct(kb.cells.emptyObject, "emptyObject");
    printAs.svgStruct(redColor, "redColor");
    printAs.svgStruct(colorRed, "colorRed");
    printAs.svgStruct(colorClass, "Color");
    printAs.svgStruct(colorClass[kb.cells.slots][kb.cells.index], "SlotIndex of Color");
    printAs.svgStruct(colorClass[kb.cells.slots][kb.cells.list], "SlotList of Color");
    printAs.svgStruct(colorClass[kb.cells.slots][kb.cells.list][kb.sequence.first], "SlotListItem1 of Color");
    printAs.svgStruct(colorClass[kb.cells.slots][kb.cells.list][kb.sequence.first][kb.coding.value], "Slot1 of Color");
    printAs.svgStruct(colorClass[kb.cells.slots][kb.cells.list][kb.sequence.first][kb.sequence.next], "SlotListItem2 of Color");
    printAs.svgStruct(colorClass[kb.cells.slots][kb.cells.list][kb.sequence.first][kb.sequence.next][kb.coding.value], "Slot2 of Color");
    printAs.value(colorClass[kb.cells.slots][kb.cells.index]);
    printAs.value(colorClass[kb.cells.slots][kb.cells.index][kb.cells.type]);
    printAs.value(colorClass[kb.cells.slots][kb.cells.index][kb.cells.type][kb.cells.slots][kb.cells.index]);
    printAs.cell(colorClass);
    printAs.cell(number_255);
    printAs.cell(number_255[kb.numbers.sign]);
    printAs.cell(number_255[kb.coding.value]);
    printAs.cell(number_255[kb.coding.value][kb.sequence.first]);
    printAs.cell(number_255[kb.coding.value][kb.sequence.first][kb.coding.value]);
    printAs.cell(number_255[kb.coding.value][kb.sequence.last]);
    printAs.cell(number_255[kb.coding.value][kb.dimensions.size]);

    printAs.svg(redColor);
    printAs.svg(colorClass[kb.cells.slots][kb.cells.index][colorRed]);
    printAs.svg(colorClass);
    printAs.svg(number_255);
    printAs.svg(number_255[kb.numbers.sign]);
    printAs.svg(number_255[kb.coding.value]);
    printAs.svg(number_255[kb.coding.value][kb.sequence.first]);
    printAs.svg(number_255[kb.coding.value][kb.sequence.first][kb.coding.value]);
    printAs.svg(number_255[kb.coding.value][kb.sequence.last]);
    printAs.svg(number_255[kb.coding.value][kb.dimensions.size]);

    std::cout << "Constructed: " << CellI::s_constructed << ", destructed: " << CellI::s_destructed << ", live: " << CellI::s_constructed - CellI::s_destructed << std::endl;
}