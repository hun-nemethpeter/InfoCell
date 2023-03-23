#include "Cells.h"
#include "SVGPrinter.h"
#include "SVGStructPrinter.h"
#include "StructPrinter.h"
#include "ValuePrinter.h"
#include <fstream>

#include <gtest/gtest.h>

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

static void oldTest();

class CellTest : public ::testing::Test
{
protected:
    CellTest() :
        kb(*m_kb),
        cells(kb.cells)
    {
    }

    static std::unique_ptr<brain::Brain> m_kb;
    brain::Brain& kb;
    brain::Cells& cells = kb.cells;
    PrintAs printAs;
};
std::unique_ptr<brain::Brain> CellTest::m_kb(std::make_unique<brain::Brain>());

TEST_F(CellTest, HybridPicture)
{
    input::Picture inputPicture("input");
    inputPicture.loadFromJsonArray("[[0, 7, 0], [7, 7, 7], [0, 7, 0]]");
    hybrid::Picture picture(kb, inputPicture);

    printAs.svg(picture);
    printAs.svg(picture[kb.visualization.pixels]);
    printAs.value(picture[kb.visualization.pixels]);

    EXPECT_EQ(&picture[kb.cells.type], &kb.type.Picture);
    EXPECT_EQ(&picture[kb.dimensions.width], &kb.pools.numbers.get(3));
    EXPECT_EQ(&picture[kb.dimensions.height], &kb.pools.numbers.get(3));
    EXPECT_EQ(&picture[kb.visualization.pixels][kb.cells.type], &kb.type.ListOf(kb.type.Pixel));
}

TEST_F(CellTest, BasicObjectTest)
{
    Type testType(kb, "Test");
    testType.addSlots(
        cells.slot(kb.coding.result, kb.type.Digit),
        cells.slot(kb.coding.value, kb.type.Number)); // TODO implement type checking

    Object object(kb, testType, "testObject");

    EXPECT_EQ(object.label(), "testObject");
    EXPECT_EQ(&object.type(), &testType);

    EXPECT_EQ(&object[kb.coding.value], &kb.cells.emptyObject);
    EXPECT_NO_THROW(object.set(kb.coding.value, kb.pools.numbers.get(42)));
    printAs.value(object[kb.coding.value]);
    EXPECT_ANY_THROW(object.set(kb.coding.argument, kb.pools.numbers.get(42)));
}

TEST_F(CellTest, BasicControlOpTest)
{
    Ref testValue1(kb, kb.type.Char);
    Ref testValue2(kb, kb.type.Color);
    Same sameOpEq(kb, testValue1, testValue1);
    Same sameOpNe(kb, testValue1, testValue2);
    sameOpEq();
    sameOpNe();

    printAs.value(sameOpEq[kb.coding.value], "testValue1 == testValue1");
    printAs.value(sameOpNe[kb.coding.value], "testValue1 != testValue1");

    EXPECT_EQ(&sameOpEq[kb.coding.value], &kb.boolean.true_);
    EXPECT_EQ(&sameOpNe[kb.coding.value], &kb.boolean.false_);
}

TEST_F(CellTest, BasicControlAddTest)
{
    Ref start(kb, kb.pools.numbers.get(42));
    Ref value10(kb, kb.pools.numbers.get(10));
    Add add10(kb, start, value10);
    add10();
    std::cout << "42 + 10 = ";
    printAs.value(add10[kb.coding.value]);

    EXPECT_EQ(&add10[kb.coding.value], &kb.pools.numbers.get(52));
}

int main(int argc, char** argv)
{
    oldTest();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

void oldTest()
{
    brain::Brain kb;
    auto& cells = kb.cells;

    PrintAs printAs;

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
    printAs.value(templateListType, "compiled templateListType");
    printAs.cell(templateListType);
    printAs.value(templateListType[kb.cells.subTypes][kb.cells.index][kb.coding.objectType]);

    Object testList(kb, kb.type.ListOf(kb.type.Number));
    testList.set(kb.dimensions.size, kb.pools.numbers.get(0));
    testList.set(kb.coding.objectType, kb.type.Number);
    printAs.cell(testList.type(), "1");
    printAs.cell(testList.type()[kb.cells.memberOf], "2");
    printAs.cell(testList.type()[kb.cells.memberOf][kb.cells.index], "3");
    printAs.value(testList[kb.cells.type][kb.cells.subTypes][kb.cells.index][kb.coding.objectType], "testList::objectType");

    Object listAddInput(kb, kb.listAdd.inputType());
    printAs.value(listAddInput, "listAddInput");
    listAddInput.set(kb.coding.value, number_255);
    printAs.cell(listAddInput, "listAddInput set");
    CellI& listAddCompiled = kb.listAdd.compile();
    printAs.cell(listAddCompiled, "listAddCompiled");
    printAs.value(listAddCompiled[kb.coding.input][kb.cells.index], "listAddCompiled.input.index");
    listAddCompiled[kb.coding.input][kb.cells.index][kb.coding.self].set(kb.coding.value, testList);
    listAddCompiled[kb.coding.input][kb.cells.index][kb.coding.value].set(kb.coding.value, kb.pools.numbers.get(1));
    printAs.value(testList, "testList");
    printAs.cell(testList, "testList");
    listAddCompiled.eval();
    printAs.value(testList, "testList after add1");
    printAs.cell(testList, "testList after add1");
    printAs.cell(testList[kb.sequence.first], "testList[first] after add1");
    listAddCompiled[kb.coding.input][kb.cells.index][kb.coding.value].set(kb.coding.value, kb.pools.numbers.get(2));
    listAddCompiled.eval();
    printAs.value(testList, "testList after add2");
    printAs.cell(testList, "testList after add2");
    printAs.cell(testList[kb.sequence.first], "testList[first] after add2");
    printAs.cell(testList[kb.sequence.last], "testList[last] after add2");

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