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

using namespace op;

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
    CellI& _0_ = kb.pools.numbers.get(0);
    CellI& _1_ = kb.pools.numbers.get(1);
    CellI& _2_ = kb.pools.numbers.get(2);
    CellI& _3_ = kb.pools.numbers.get(3);
};
std::unique_ptr<brain::Brain> CellTest::m_kb(std::make_unique<brain::Brain>());

TEST_F(CellTest, CallMethod)
{
    Object list(kb, kb.type.List);

    list.constructor();
    printAs.value(list);
    EXPECT_EQ(&list[kb.dimensions.size], &_0_);
    EXPECT_EQ(&list[kb.coding.objectType], &kb.type.Cell);

    list.method(kb.sequence.add, { kb.coding.value, _1_ });
    EXPECT_EQ(&list[kb.dimensions.size], &_1_);

    CellI& firstItem = list[kb.sequence.first];
    EXPECT_EQ(&firstItem, &list[kb.sequence.last]);
    EXPECT_EQ(firstItem.has(kb.sequence.previous), false);
    EXPECT_EQ(firstItem.has(kb.sequence.next), false);
    EXPECT_EQ(&firstItem[kb.coding.value], &_1_);
    printAs.value(list);

    list.method(kb.sequence.add, { kb.coding.value, kb.pools.numbers.get(2) });
    EXPECT_EQ(&list[kb.dimensions.size], &_2_);

    CellI& secondItem = list[kb.sequence.last];
    EXPECT_EQ(&firstItem, &list[kb.sequence.first]);
    EXPECT_NE(&firstItem, &list[kb.sequence.last]);

    EXPECT_EQ(firstItem.has(kb.sequence.previous), false);
    EXPECT_EQ(&firstItem[kb.sequence.next], &secondItem);
    EXPECT_EQ(&firstItem[kb.coding.value], &_1_);

    EXPECT_EQ(&secondItem[kb.sequence.previous], &firstItem);
    EXPECT_EQ(secondItem.has(kb.sequence.next), false);
    EXPECT_EQ(&secondItem[kb.coding.value], &_2_);

    printAs.value(list);
    list.method(kb.sequence.add, { kb.coding.value, _3_ });
    EXPECT_EQ(&list[kb.dimensions.size], &_3_);

    CellI& thirdItem = list[kb.sequence.last];
    EXPECT_EQ(&firstItem, &list[kb.sequence.first]);
    EXPECT_NE(&firstItem, &list[kb.sequence.last]);

    EXPECT_EQ(firstItem.has(kb.sequence.previous), false);
    EXPECT_EQ(&firstItem[kb.sequence.next], &secondItem);
    EXPECT_EQ(&firstItem[kb.coding.value], &_1_);

    EXPECT_EQ(&secondItem[kb.sequence.previous], &firstItem);
    EXPECT_EQ(&secondItem[kb.sequence.next], &thirdItem);
    EXPECT_EQ(&secondItem[kb.coding.value], &_2_);

    EXPECT_EQ(&thirdItem[kb.sequence.previous], &secondItem);
    EXPECT_EQ(thirdItem.has(kb.sequence.next), false);
    EXPECT_EQ(&thirdItem[kb.coding.value], &_3_);

    printAs.value(list);
    CellI& size = list.method(kb.dimensions.size);
    printAs.value(size);
}

TEST_F(CellTest, CreatetTypeWithConstructor)
{
    Object newType(kb, kb.type.Type_);
    newType.constructor({ cells.slots, kb.map(kb.sequence.previous, kb.cells.slot(kb.sequence.previous, kb.type.ListItem)) });
//    newType.constructor({ cells.slots, kb.list(kb.cells.slot(kb.sequence.previous, kb.type.ListItem)) });

    printAs.cell(newType);
}

TEST_F(CellTest, CreatingListAdd)
{
    Object testList(kb, kb.type.ListOf(kb.type.Number));
    testList.set(kb.dimensions.size, _0_);
    testList.set(kb.coding.objectType, kb.type.Number);

    printAs.cell(testList.type(), "1");
    printAs.cell(testList.type()[kb.cells.memberOf], "2");
    printAs.cell(testList.type()[kb.cells.memberOf][kb.cells.index], "3");
    printAs.value(testList[kb.cells.type][kb.cells.subTypes][kb.cells.index][kb.coding.objectType], "testList::objectType");

    CellI& listAddCompiled = kb.type.List[kb.cells.methods][kb.cells.index][kb.sequence.add];
    printAs.cell(listAddCompiled, "listAddCompiled");
    printAs.value(listAddCompiled[kb.coding.input][kb.cells.index], "listAddCompiled.input.index");
    listAddCompiled[kb.coding.input][kb.cells.index][kb.coding.self].set(kb.coding.value, testList);
    listAddCompiled[kb.coding.input][kb.cells.index][kb.coding.value].set(kb.coding.value, _1_);
    printAs.value(testList, "testList");
    printAs.cell(testList, "testList");
    listAddCompiled.eval();
    printAs.value(testList, "testList after add1");
    printAs.cell(testList, "testList after add1");
    printAs.cell(testList[kb.sequence.first], "testList[first] after add1");
    listAddCompiled[kb.coding.input][kb.cells.index][kb.coding.value].set(kb.coding.value, _2_);
    listAddCompiled.eval();
    printAs.value(testList, "testList after add2");
    printAs.cell(testList, "testList after add2");
    printAs.cell(testList[kb.sequence.first], "testList[first] after add2");
    printAs.cell(testList[kb.sequence.last], "testList[last] after add2");
}

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

TEST_F(CellTest, CreatingCustomType)
{
    Object colorRed(kb, kb.type.Cell, "red");
    Object colorGreen(kb, kb.type.Cell, "green");
    Object colorBlue(kb, kb.type.Cell, "blue");

    Type colorClass(kb, "Color");
    colorClass.addSlots(
        cells.slot(colorRed, kb.type.Number),
        cells.slot(colorGreen, kb.type.Number),
        cells.slot(colorBlue, kb.type.Number));

    Object redColor(kb, colorClass, "redColor");
    redColor.set(colorRed, kb.pools.numbers.get(255));
    redColor.set(colorGreen, kb.pools.numbers.get(0));
    redColor.set(colorBlue, kb.pools.numbers.get(0));

    printAs.value(colorClass, "colorClass:");
    printAs.value(colorClass[kb.cells.slots][kb.cells.list], "colorClass::slots");
    printAs.value(colorClass[kb.cells.slots][kb.cells.index][colorRed]);
    EXPECT_TRUE(colorClass[kb.cells.slots][kb.cells.index].has(colorRed));
    EXPECT_TRUE(colorClass[kb.cells.slots][kb.cells.index].has(colorGreen));
    EXPECT_TRUE(colorClass[kb.cells.slots][kb.cells.index].has(colorBlue));
    printAs.value(colorClass[kb.cells.slots][kb.cells.index][colorRed][kb.cells.type], "colorClass slot of colorRed");
    EXPECT_EQ(&colorClass[kb.cells.slots][kb.cells.index][colorRed][kb.cells.type], &kb.type.Slot);
    EXPECT_EQ(&colorClass[kb.cells.slots][kb.cells.index][colorRed][kb.cells.type], &kb.type.Slot);
    EXPECT_EQ(&colorClass[kb.cells.slots][kb.cells.index][colorGreen][kb.cells.type], &kb.type.Slot);
    EXPECT_EQ(&colorClass[kb.cells.slots][kb.cells.index][colorBlue][kb.cells.type], &kb.type.Slot);
    printAs.value(redColor);

    printAs.cell(redColor);
    printAs.cell(colorClass[kb.cells.slots][kb.cells.index][colorRed]);
    printAs.cell(colorClass[kb.cells.slots][kb.cells.index]);

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

    printAs.svg(redColor);
    printAs.svg(colorClass[kb.cells.slots][kb.cells.index][colorRed]);
    printAs.svg(colorClass);
}

TEST_F(CellTest, CreatingNumber)
{
    Number& number_0   = kb.pools.numbers.get(0);
    Number& number_255 = kb.pools.numbers.get(255);

    printAs.value(number_255);
    printAs.value(number_255[kb.coding.value][kb.sequence.first][kb.coding.value]);

    printAs.svgStruct(kb.cells.emptyObject, "emptyObject");

    printAs.cell(number_255);
    printAs.cell(number_255[kb.numbers.sign]);
    printAs.cell(number_255[kb.coding.value]);
    printAs.cell(number_255[kb.coding.value][kb.sequence.first]);
    printAs.cell(number_255[kb.coding.value][kb.sequence.first][kb.coding.value]);
    printAs.cell(number_255[kb.coding.value][kb.sequence.last]);
    printAs.cell(number_255[kb.coding.value][kb.dimensions.size]);

    printAs.svg(number_255);
    printAs.svg(number_255[kb.numbers.sign]);
    printAs.svg(number_255[kb.coding.value]);
    printAs.svg(number_255[kb.coding.value][kb.sequence.first]);
    printAs.svg(number_255[kb.coding.value][kb.sequence.first][kb.coding.value]);
    printAs.svg(number_255[kb.coding.value][kb.sequence.last]);
    printAs.svg(number_255[kb.coding.value][kb.dimensions.size]);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    std::cout << "Constructed: " << CellI::s_constructed << ", destructed: " << CellI::s_destructed << ", live: " << CellI::s_constructed - CellI::s_destructed << std::endl;

    return ret;
}