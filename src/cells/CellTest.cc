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
    PrintAs(const std::string& postfix) :
        m_postfix(postfix),
        m_svgPrinter(800, 600),
        m_svgStructPrinter(800, 600)
    {
    }

    ~PrintAs()
    {
        m_svgPrinter.writeFile(std::format("F:\\Devel\\ARC\\synth\\svgv-{}.svg", m_postfix));
        m_svgStructPrinter.writeFile(std::format("F:\\Devel\\ARC\\synth\\svgs-{}.svg", m_postfix));
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

    std::string m_postfix;
    svg::Printer m_svgPrinter;
    svg::StructPrinter m_svgStructPrinter;
};

class CellTest : public ::testing::Test
{
protected:
    CellTest() :
        kb(*m_kb),
        printAs(::testing::UnitTest::GetInstance()->current_test_info()->name())
    {
    }

    static std::unique_ptr<brain::Brain> m_kb;
    brain::Brain& kb;
    brain::Coding& coding = kb.coding;
    PrintAs printAs;
    CellI& _0_    = kb._0_;
    CellI& _1_    = kb._1_;
    CellI& _2_    = kb._2_;
    CellI& _3_    = kb._3_;
    CellI& _4_    = kb._4_;
    CellI& _5_    = kb._5_;
    CellI& _6_    = kb._6_;
    CellI& _7_    = kb._7_;
    CellI& _8_    = kb._8_;
    CellI& _9_    = kb._9_;
    CellI& true_  = kb.boolean.true_;
    CellI& false_ = kb.boolean.false_;
};
std::unique_ptr<brain::Brain> CellTest::m_kb(std::make_unique<brain::Brain>());

TEST_F(CellTest, PrintMethod)
{
    const auto printOp = [this](CellI& type, CellI& method) { printAs.value(type[kb.coding.methods][kb.coding.index][method]); };

#if 1
    printOp(kb.type.Map, kb.coding.constructor);
    printOp(kb.type.Map, kb.coding.template_);
    printOp(kb.type.Map, kb.dimensions.size);
    printOp(kb.type.Map, kb.sequence.add);
    printOp(kb.type.Map, kb.sequence.empty);

    printOp(kb.type.Type_, kb.coding.constructor);
    printOp(kb.type.Type_, kb.methods.addSlot);
    printOp(kb.type.Type_, kb.methods.addSubType);
    printOp(kb.type.Type_, kb.methods.addMembership);
    printOp(kb.type.Type_, kb.methods.addSlots);

    printOp(kb.type.ListItem, kb.coding.template_);
    printOp(kb.type.ListItem, kb.coding.constructor);

    printOp(kb.type.List, kb.coding.template_);
    printOp(kb.type.List, kb.coding.constructor);
    printOp(kb.type.List, kb.sequence.add);
    printOp(kb.type.List, kb.dimensions.size);
    printOp(kb.type.List, kb.sequence.empty);

    printOp(kb.type.Number, kb.test.factorial);
    Object testNumber(kb, kb.type.Number, "testNumber");
    EXPECT_EQ(&testNumber.method(kb.test.factorial, { kb.coding.input, _0_ }), &_1_);
    EXPECT_EQ(&testNumber.method(kb.test.factorial, { kb.coding.input, _1_ }), &_1_);
    EXPECT_EQ(&testNumber.method(kb.test.factorial, { kb.coding.input, _2_ }), &_2_);
    EXPECT_EQ(&testNumber.method(kb.test.factorial, { kb.coding.input, _3_ }), &_6_);
    EXPECT_EQ(&testNumber.method(kb.test.factorial, { kb.coding.input, _4_ }), &kb.pools.numbers.get(24));
    EXPECT_EQ(&testNumber.method(kb.test.factorial, { kb.coding.input, _5_ }), &kb.pools.numbers.get(120));
#endif

    // TODO
    // List constructor shouldn't expect an objectType
    // print function for AST nodes
    // inline methods
    // maybe create a Set<type>?
    // type checking
    // create an own type for every function to able to save return values to the fn object
    // remove .label() from CellI                                          ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
}

TEST_F(CellTest, List)
{
    Object list(kb, kb.type.List, kb.coding.constructor);

    printAs.value(list);
    printAs.cell(list);
    EXPECT_EQ(&list[kb.dimensions.size], &_0_);
    EXPECT_EQ(&list.method(kb.dimensions.size), &_0_);
    EXPECT_EQ(&list.method(kb.sequence.empty), &true_);
    EXPECT_EQ(&list[kb.coding.objectType], &kb.type.Cell);

    list.method(kb.sequence.add, { kb.coding.value, _1_ });
    EXPECT_EQ(&list[kb.dimensions.size], &_1_);
    EXPECT_EQ(&list.method(kb.dimensions.size), &_1_);
    EXPECT_EQ(&list.method(kb.sequence.empty), &false_);

    CellI& firstItem = list[kb.sequence.first];
    EXPECT_EQ(&firstItem, &list[kb.sequence.last]);
    EXPECT_EQ(firstItem.has(kb.sequence.previous), false);
    EXPECT_EQ(firstItem.has(kb.sequence.next), false);
    EXPECT_EQ(&firstItem[kb.coding.value], &_1_);
    printAs.value(list);

    list.method(kb.sequence.add, { kb.coding.value, kb.pools.numbers.get(2) });
    EXPECT_EQ(&list[kb.dimensions.size], &_2_);
    EXPECT_EQ(&list.method(kb.dimensions.size), &_2_);
    EXPECT_EQ(&list.method(kb.sequence.empty), &false_);

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
    EXPECT_EQ(&list.method(kb.dimensions.size), &_3_);
    EXPECT_EQ(&list.method(kb.sequence.empty), &false_);

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

TEST_F(CellTest, ListOfNumber)
{
    CellI& ListOfNumber = kb.type.List.smethod(kb.coding.template_, { kb.coding.objectType, kb.type.Number });
    Object list(kb, ListOfNumber, kb.coding.constructor);

    printAs.value(list);
    printAs.cell(list);
    EXPECT_EQ(&list[kb.dimensions.size], &_0_);
    EXPECT_EQ(&list.method(kb.dimensions.size), &_0_);
    EXPECT_EQ(&list.method(kb.sequence.empty), &true_);
    EXPECT_EQ(&list[kb.coding.objectType], &kb.type.Number);

    list.method(kb.sequence.add, { kb.coding.value, _1_ });
    EXPECT_EQ(&list[kb.dimensions.size], &_1_);
    EXPECT_EQ(&list.method(kb.dimensions.size), &_1_);
    EXPECT_EQ(&list.method(kb.sequence.empty), &false_);

    CellI& firstItem = list[kb.sequence.first];
    EXPECT_EQ(&firstItem, &list[kb.sequence.last]);
    EXPECT_EQ(firstItem.has(kb.sequence.previous), false);
    EXPECT_EQ(firstItem.has(kb.sequence.next), false);
    EXPECT_EQ(&firstItem[kb.coding.value], &_1_);
    printAs.value(list);

    list.method(kb.sequence.add, { kb.coding.value, kb.pools.numbers.get(2) });
    EXPECT_EQ(&list[kb.dimensions.size], &_2_);
    EXPECT_EQ(&list.method(kb.dimensions.size), &_2_);
    EXPECT_EQ(&list.method(kb.sequence.empty), &false_);

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
    EXPECT_EQ(&list.method(kb.dimensions.size), &_3_);
    EXPECT_EQ(&list.method(kb.sequence.empty), &false_);

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

TEST_F(CellTest, Map)
{
    Object map(kb, kb.type.Map, kb.coding.constructor);

    printAs.value(map);
    printAs.cell(map);
    EXPECT_EQ(&map[kb.dimensions.size], &_0_);
    EXPECT_EQ(&map.method(kb.dimensions.size), &_0_);
    EXPECT_EQ(&map.method(kb.sequence.empty), &true_);
    EXPECT_EQ(&map[kb.coding.keyType], &kb.type.Cell);
    EXPECT_EQ(&map[kb.coding.objectType], &kb.type.Cell);

    map.method(kb.sequence.add, { coding.key, _1_ }, { coding.value, kb.colors.red });
    printAs.value(map);
    printAs.cell(map);
    EXPECT_EQ(&map[kb.dimensions.size], &_1_);
    EXPECT_EQ(&map.method(kb.dimensions.size), &_1_);
    EXPECT_EQ(&map[coding.list][kb.dimensions.size], &_1_);
    EXPECT_EQ(&map[coding.list][kb.sequence.first][kb.coding.value], &kb.colors.red);
    EXPECT_EQ(&map[coding.list][kb.sequence.first], &map[coding.list][kb.sequence.last]);
    EXPECT_TRUE(map[coding.index].has(_1_));
    EXPECT_EQ(&map[coding.index][_1_], &kb.colors.red);
    EXPECT_EQ(&map.method(kb.sequence.empty), &false_);
    EXPECT_TRUE(map[coding.index][kb.coding.type][kb.coding.memberOf][kb.coding.index].has(kb.type.Index));

    map.method(kb.sequence.add, { coding.key, _2_ }, { coding.value, kb.colors.green });
    map.method(kb.sequence.add, { coding.key, _3_ }, { coding.value, kb.colors.blue });
    EXPECT_EQ(&map[coding.index][_1_], &kb.colors.red);
    EXPECT_EQ(&map[coding.index][_2_], &kb.colors.green);
    EXPECT_EQ(&map[coding.index][_3_], &kb.colors.blue);
    printAs.value(map);
    printAs.cell(map);
}

TEST_F(CellTest, MapNumberToColor)
{
    CellI& MapNumberToColor = kb.type.Map.smethod(kb.coding.template_, { kb.coding.keyType, kb.type.Number }, { kb.coding.objectType, kb.type.Color });
    Object map(kb, MapNumberToColor, kb.coding.constructor);

    printAs.value(map);
    printAs.cell(map);
    EXPECT_EQ(&map[kb.dimensions.size], &_0_);
    EXPECT_EQ(&map.method(kb.dimensions.size), &_0_);
    EXPECT_EQ(&map.method(kb.sequence.empty), &true_);
    EXPECT_EQ(&map[kb.coding.keyType], &kb.type.Number);
    EXPECT_EQ(&map[kb.coding.objectType], &kb.type.Color);

    map.method(kb.sequence.add, { coding.key, _1_}, { coding.value, kb.colors.red });
    printAs.value(map);
    printAs.cell(map);
    EXPECT_EQ(&map[kb.dimensions.size], &_1_);
    EXPECT_EQ(&map.method(kb.dimensions.size), &_1_);
    EXPECT_EQ(&map[coding.list][kb.dimensions.size], &_1_);
    EXPECT_EQ(&map[coding.list][kb.sequence.first][kb.coding.value], &kb.colors.red);
    EXPECT_EQ(&map[coding.list][kb.sequence.first], &map[coding.list][kb.sequence.last]);
    EXPECT_TRUE(map[coding.index].has(_1_));
    EXPECT_EQ(&map[coding.index][_1_], &kb.colors.red);
    EXPECT_EQ(&map.method(kb.sequence.empty), &false_);
    EXPECT_TRUE(map[coding.index][kb.coding.type][kb.coding.memberOf][kb.coding.index].has(kb.type.Index));

    map.method(kb.sequence.add, { coding.key, _2_ }, { coding.value, kb.colors.green });
    map.method(kb.sequence.add, { coding.key, _3_ }, { coding.value, kb.colors.blue });
    EXPECT_EQ(&map[coding.index][_1_], &kb.colors.red);
    EXPECT_EQ(&map[coding.index][_2_], &kb.colors.green);
    EXPECT_EQ(&map[coding.index][_3_], &kb.colors.blue);
    printAs.value(map);
    printAs.cell(map);
}

TEST_F(CellTest, ListItem)
{
    Object listItem(kb, kb.type.ListItem, kb.coding.constructor, { coding.value, kb.colors.green });

    EXPECT_EQ(&listItem[kb.coding.value], &kb.colors.green);

    EXPECT_EQ(&listItem.type()[kb.coding.memberOf][kb.dimensions.size], &_1_);
    EXPECT_TRUE(listItem.type()[kb.coding.memberOf][kb.coding.index].has(kb.type.Iterator));

    EXPECT_EQ(&listItem.type()[kb.coding.slots][kb.dimensions.size], &_3_);
    EXPECT_TRUE(listItem.type()[kb.coding.slots][kb.coding.index].has(kb.sequence.previous));
    EXPECT_TRUE(listItem.type()[kb.coding.slots][kb.coding.index].has(kb.sequence.next));
    EXPECT_TRUE(listItem.type()[kb.coding.slots][kb.coding.index].has(kb.coding.value));

    EXPECT_TRUE(listItem.type()[kb.coding.methods][kb.coding.index].has(kb.coding.template_));
    EXPECT_TRUE(listItem.type()[kb.coding.methods][kb.coding.index].has(kb.coding.constructor));
}

TEST_F(CellTest, ListItemTemplate)
{
    CellI& ListItemNumber = kb.type.ListItem.smethod(kb.coding.template_, { kb.coding.objectType, kb.type.Number });
    Object listItemNumber(kb, ListItemNumber, kb.coding.constructor, { coding.value, _1_ });

    EXPECT_EQ(&listItemNumber[kb.coding.value], &_1_);

    printAs.value(kb.type.ListItem, "type.ListItem");
    printAs.value(ListItemNumber, "listItemNumber");
    printAs.value(kb.type.ListItem[kb.coding.slots][kb.coding.list], "type.ListItem[slots]");
    printAs.value(ListItemNumber[kb.coding.slots][kb.coding.list], "listItemNumber[slots]");

    EXPECT_EQ(&ListItemNumber[kb.coding.memberOf][kb.dimensions.size], &_1_);
    EXPECT_TRUE(ListItemNumber[kb.coding.memberOf][kb.coding.index].has(kb.type.ListItem));

    EXPECT_EQ(&ListItemNumber[kb.coding.slots][kb.dimensions.size], &_3_);
    EXPECT_TRUE(ListItemNumber[kb.coding.slots][kb.coding.index].has(kb.sequence.previous));
    EXPECT_TRUE(ListItemNumber[kb.coding.slots][kb.coding.index].has(kb.sequence.next));
    EXPECT_TRUE(ListItemNumber[kb.coding.slots][kb.coding.index].has(kb.coding.value));

    EXPECT_EQ(&ListItemNumber[kb.coding.methods], &kb.type.ListItem[kb.coding.methods]);
}

TEST_F(CellTest, ListTemplate)
{
    CellI& ListOfNumbers = kb.type.List.smethod(kb.coding.template_, { kb.coding.objectType, kb.type.Number });

    EXPECT_EQ(&ListOfNumbers[coding.subTypes][kb.dimensions.size], &_2_);
    EXPECT_TRUE(ListOfNumbers[coding.subTypes][coding.index].has(coding.objectType));
    EXPECT_EQ(&ListOfNumbers[coding.subTypes][coding.index][coding.objectType], &kb.type.Number);
    EXPECT_TRUE(ListOfNumbers[coding.subTypes][coding.index].has(coding.itemType));
    CellI& ListItemType = ListOfNumbers[coding.subTypes][coding.index][coding.itemType];
    EXPECT_EQ(&ListItemType[kb.coding.slots][kb.coding.index][kb.coding.value][kb.coding.slotType], &kb.type.Number);
    EXPECT_NE(&ListItemType, &kb.type.ListItem);
    EXPECT_EQ(&ListItemType[kb.coding.memberOf][kb.dimensions.size], &_1_);
    EXPECT_TRUE(ListItemType[kb.coding.memberOf][kb.coding.index].has(kb.type.ListItem));

    EXPECT_EQ(&ListItemType[kb.coding.slots][kb.dimensions.size], &_3_);
    EXPECT_TRUE(ListItemType[kb.coding.slots][kb.coding.index].has(kb.sequence.previous));
    EXPECT_TRUE(ListItemType[kb.coding.slots][kb.coding.index].has(kb.sequence.next));
    EXPECT_TRUE(ListItemType[kb.coding.slots][kb.coding.index].has(kb.coding.value));

    EXPECT_EQ(&ListItemType[kb.coding.methods], &kb.type.ListItem[kb.coding.methods]);

    EXPECT_EQ(&ListOfNumbers[kb.coding.methods], &kb.type.List[kb.coding.methods]);

    EXPECT_EQ(&ListOfNumbers[kb.coding.slots][kb.dimensions.size], &_5_);
    EXPECT_TRUE(ListOfNumbers[kb.coding.slots][kb.coding.index].has(kb.sequence.first));
    EXPECT_EQ(&ListOfNumbers[kb.coding.slots][kb.coding.index][kb.sequence.first][kb.coding.slotType], &ListItemType);

    EXPECT_TRUE(ListOfNumbers[kb.coding.slots][kb.coding.index].has(kb.sequence.last));
    EXPECT_EQ(&ListOfNumbers[kb.coding.slots][kb.coding.index][kb.sequence.last][kb.coding.slotType], &ListItemType);

    EXPECT_TRUE(ListOfNumbers[kb.coding.slots][kb.coding.index].has(kb.dimensions.size));
    EXPECT_EQ(&ListOfNumbers[kb.coding.slots][kb.coding.index][kb.dimensions.size][kb.coding.slotType], &kb.type.Number);

    EXPECT_TRUE(ListOfNumbers[kb.coding.slots][kb.coding.index].has(kb.coding.objectType));
    EXPECT_EQ(&ListOfNumbers[kb.coding.slots][kb.coding.index][kb.coding.objectType][kb.coding.slotType], &kb.type.Number);

    EXPECT_TRUE(ListOfNumbers[kb.coding.slots][kb.coding.index].has(kb.coding.itemType));
    EXPECT_EQ(&ListOfNumbers[kb.coding.slots][kb.coding.index][kb.coding.itemType][kb.coding.slotType], &ListItemType);

    EXPECT_TRUE(ListOfNumbers[kb.coding.memberOf][kb.coding.index].has(kb.type.List));
    EXPECT_EQ(&ListOfNumbers[kb.coding.memberOf][kb.dimensions.size], &_1_);

    printAs.value(kb.type.List, "type.List");
    printAs.value(ListOfNumbers, "ListOfNumbers");

    Object listOfNumbers(kb, ListOfNumbers, kb.coding.constructor);
    CellI& listItemNumber = listOfNumbers[kb.coding.itemType];
    EXPECT_EQ(&ListItemType, &listItemNumber);
    EXPECT_EQ(&ListOfNumbers[coding.subTypes][coding.index][coding.itemType], &listItemNumber);
}

TEST_F(CellTest, CreatedTypeWithConstructor)
{
    Object emptySubTypesMap(kb, kb.type.MapCellToType, kb.coding.constructor);
    Object emptyMethodsMap(kb, kb.type.MapCellToType, kb.coding.constructor);
    Object slotsMap(kb, kb.type.MapCellToSlot, kb.coding.constructor);
    slotsMap.method(kb.sequence.add, { kb.coding.key, kb.dimensions.size }, { kb.coding.value, kb.coding.slot(kb.dimensions.size, kb.type.Number) });

    Object newType(kb, kb.type.Type_, kb.coding.constructor,
                   { coding.slots, slotsMap },
                   { coding.subTypes, emptySubTypesMap },
                   { coding.memberOf, kb.map(kb.type.Cell, kb.type.Cell) },
                   { coding.methods, emptyMethodsMap });
    printAs.value(newType[coding.slots]);
    printAs.value(newType[coding.slots][coding.list], "newType[coding.slots][coding.list]");
    printAs.value(newType[coding.memberOf][coding.list], "newType[coding.memberOf][coding.list]");
    Object newObject(kb, newType);
    printAs.cell(newObject, "Empty newObject");
    EXPECT_ANY_THROW(newObject.set(kb.dimensions.height, _1_));
    EXPECT_NO_THROW(newObject.set(kb.dimensions.size, _1_));
    printAs.cell(newObject, "newObject.size = 1");
}

TEST_F(CellTest, HybridPicture)
{
    input::Picture inputPicture("input");
    inputPicture.loadFromJsonArray("[[0, 7, 0], [7, 7, 7], [0, 7, 0]]");
    hybrid::Picture picture(kb, inputPicture);

    printAs.svg(picture);
    printAs.svg(picture[kb.visualization.pixels]);
    printAs.value(picture[kb.visualization.pixels]);

    EXPECT_EQ(&picture[kb.coding.type], &kb.type.Picture);
    EXPECT_EQ(&picture[kb.dimensions.width], &kb.pools.numbers.get(3));
    EXPECT_EQ(&picture[kb.dimensions.height], &kb.pools.numbers.get(3));
    EXPECT_EQ(&picture[kb.visualization.pixels][kb.coding.type], &kb.type.ListOf(kb.type.Pixel));

    CellI& ListOfPixels = kb.type.List.smethod(kb.coding.template_, { kb.coding.objectType, kb.type.Pixel });
    Object listOfPixels(kb, ListOfPixels, kb.coding.constructor, "listOfPixels");
    listOfPixels.method(kb.sequence.add, { kb.coding.value, picture[kb.visualization.pixels][kb.sequence.first][kb.coding.value] });
    listOfPixels.method(kb.sequence.add, { kb.coding.value, picture[kb.visualization.pixels][kb.sequence.first][kb.sequence.next][kb.coding.value] });
    printAs.value(listOfPixels);
}

TEST_F(CellTest, BasicObjectTest)
{
    Object testType(kb, kb.type.Type_, "Test");
    Object emptyList(kb, kb.type.List, kb.coding.constructor);

    testType.method(kb.methods.addSlots, { kb.coding.list, emptyList });

    testType.method(kb.methods.addSlots, { kb.coding.list, kb.list(
        coding.slot(kb.coding.result, kb.type.Digit),
        coding.slot(kb.coding.value, kb.type.Number))}); // TODO implement type checking

    Object object(kb, testType, "testObject");

    EXPECT_EQ(object.label(), "testObject");
    EXPECT_EQ(&object.type(), &testType);

    EXPECT_ANY_THROW(&object[kb.coding.value]);
    EXPECT_NO_THROW(object.set(kb.coding.value, kb.pools.numbers.get(42)));
    printAs.value(object[kb.coding.value]);
    EXPECT_ANY_THROW(object.set(kb.coding.argument, kb.pools.numbers.get(42)));
}

TEST_F(CellTest, BasicControlOpTest)
{
    Object testValue1(kb, kb.type.op.ConstVar);
    testValue1.set(kb.coding.value, kb.type.Char);

    Object testValue2(kb, kb.type.op.ConstVar);
    testValue2.set(kb.coding.value, kb.type.Color);

    Object sameOpEq(kb, kb.type.op.Same, "sameOpEq");
    sameOpEq.set(kb.coding.lhs, testValue1);
    sameOpEq.set(kb.coding.rhs, testValue1);

    Object sameOpNe(kb, kb.type.op.Same, "sameOpNe");
    sameOpNe.set(kb.coding.lhs, testValue1);
    sameOpNe.set(kb.coding.rhs, testValue2);

    sameOpEq();
    sameOpNe();

    printAs.value(sameOpEq[kb.coding.value], "testValue1 == testValue1");
    printAs.value(sameOpNe[kb.coding.value], "testValue1 != testValue1");

    EXPECT_EQ(&sameOpEq[kb.coding.value], &true_);
    EXPECT_EQ(&sameOpNe[kb.coding.value], &false_);
}

TEST_F(CellTest, BasicControlAddTest)
{
    Object start(kb, kb.type.op.ConstVar);
    start.set(kb.coding.value, kb.pools.numbers.get(42));

    Object value10(kb, kb.type.op.ConstVar);
    value10.set(kb.coding.value, kb.pools.numbers.get(10));

    Object add10(kb, kb.type.op.Add, "add10");
    add10.set(kb.coding.lhs, start);
    add10.set(kb.coding.rhs, value10);
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

    Object colorClass(kb, kb.type.Type_, "Color");
#if 1 // TODO
    Object* slotMapPtr = new Object(kb, kb.type.MapCellToSlot, kb.coding.constructor);
    colorClass.set(coding.slots, *slotMapPtr);
#endif

    colorClass.method(kb.methods.addSlots, { kb.coding.list, kb.list(coding.slot(colorRed, kb.type.Number), coding.slot(colorGreen, kb.type.Number), coding.slot(colorBlue, kb.type.Number)) });

    Object redColor(kb, colorClass, "redColor");
    redColor.set(colorRed, kb.pools.numbers.get(255));
    redColor.set(colorGreen, kb.pools.numbers.get(0));
    redColor.set(colorBlue, kb.pools.numbers.get(0));

    printAs.value(colorClass, "colorClass:");
    printAs.value(colorClass[kb.coding.slots][kb.coding.list], "colorClass::slots");
    printAs.value(colorClass[kb.coding.slots][kb.coding.index][colorRed]);
    EXPECT_TRUE(colorClass[kb.coding.slots][kb.coding.index].has(colorRed));
    EXPECT_TRUE(colorClass[kb.coding.slots][kb.coding.index].has(colorGreen));
    EXPECT_TRUE(colorClass[kb.coding.slots][kb.coding.index].has(colorBlue));
    printAs.value(colorClass[kb.coding.slots][kb.coding.index][colorRed][kb.coding.type], "colorClass slot of colorRed");
    EXPECT_EQ(&colorClass[kb.coding.slots][kb.coding.index][colorRed][kb.coding.type], &kb.type.Slot);
    EXPECT_EQ(&colorClass[kb.coding.slots][kb.coding.index][colorRed][kb.coding.type], &kb.type.Slot);
    EXPECT_EQ(&colorClass[kb.coding.slots][kb.coding.index][colorGreen][kb.coding.type], &kb.type.Slot);
    EXPECT_EQ(&colorClass[kb.coding.slots][kb.coding.index][colorBlue][kb.coding.type], &kb.type.Slot);
    printAs.value(redColor);

    printAs.cell(redColor);
    printAs.cell(colorClass[kb.coding.slots][kb.coding.index][colorRed], "colorClass[kb.coding.slots][kb.coding.index][colorRed]");
    printAs.cell(colorClass[kb.coding.slots][kb.coding.index], "colorClass[kb.coding.slots][kb.coding.index]");

    printAs.svgStruct(redColor, "redColor");
    printAs.svgStruct(colorRed, "colorRed");
    printAs.svgStruct(colorClass, "Color");
    printAs.svgStruct(colorClass[kb.coding.slots][kb.coding.index], "SlotIndex of Color");
    printAs.svgStruct(colorClass[kb.coding.slots][kb.coding.list], "SlotList of Color");
    printAs.svgStruct(colorClass[kb.coding.slots][kb.coding.list][kb.sequence.first], "SlotListItem1 of Color");
    printAs.svgStruct(colorClass[kb.coding.slots][kb.coding.list][kb.sequence.first][kb.coding.value], "Slot1 of Color");
    printAs.svgStruct(colorClass[kb.coding.slots][kb.coding.list][kb.sequence.first][kb.sequence.next], "SlotListItem2 of Color");
    printAs.svgStruct(colorClass[kb.coding.slots][kb.coding.list][kb.sequence.first][kb.sequence.next][kb.coding.value], "Slot2 of Color");
    printAs.value(colorClass[kb.coding.slots][kb.coding.index], "colorClass[kb.coding.slots][kb.coding.index]");
    printAs.value(colorClass[kb.coding.slots][kb.coding.index][kb.coding.type], "colorClass[kb.coding.slots][kb.coding.index][kb.coding.type]");
    // printAs.value(colorClass[kb.coding.slots][kb.coding.index][kb.coding.type][kb.coding.slots], "colorClass[kb.coding.slots][kb.coding.index][kb.coding.type][kb.coding.slots]"); TODO
    printAs.value(colorClass[kb.coding.slots][kb.coding.index][kb.coding.type][kb.coding.slots][kb.coding.index], "colorClass[kb.coding.slots][kb.coding.index][kb.coding.type][kb.coding.slots][kb.coding.index]");
    printAs.cell(colorClass);

    printAs.svg(redColor);
    printAs.svg(colorClass[kb.coding.slots][kb.coding.index][colorRed]);
    printAs.svg(colorClass);
}

TEST_F(CellTest, CreatingNumber)
{
    Number& number_0   = kb.pools.numbers.get(0);
    Number& number_255 = kb.pools.numbers.get(255);

    printAs.value(number_255);
    printAs.value(number_255[kb.coding.value][kb.sequence.first][kb.coding.value]);

    printAs.svgStruct(kb.coding.emptyObject, "emptyObject");

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