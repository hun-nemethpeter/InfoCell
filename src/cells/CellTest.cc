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
    brain::ID& id = kb.id;
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
    const auto printOp = [this](CellI& type, CellI& method) { printAs.value(type[kb.id.methods][kb.id.index][method]); };

    printOp(kb.type.Map, kb.id.constructor);
    printOp(kb.type.Map, kb.id.template_);
    printOp(kb.type.Map, kb.dimensions.size);
    printOp(kb.type.Map, kb.sequence.add);
    printOp(kb.type.Map, kb.sequence.empty);

#if 1
    printOp(kb.type.Type_, kb.id.constructor);
    printOp(kb.type.Type_, kb.methods.addSlot);
    printOp(kb.type.Type_, kb.methods.addSubType);
    printOp(kb.type.Type_, kb.methods.addMembership);
    printOp(kb.type.Type_, kb.methods.addSlots);

    printOp(kb.type.ListItem, kb.id.template_);
    printOp(kb.type.ListItem, kb.id.constructor);

    printOp(kb.type.List, kb.id.template_);
    printOp(kb.type.List, kb.id.constructor);
    printOp(kb.type.List, kb.sequence.add);
    printOp(kb.type.List, kb.dimensions.size);
    printOp(kb.type.List, kb.sequence.empty);

    printOp(kb.type.Number, kb.test.factorial);
    Object testNumber(kb, kb.type.Number, "testNumber");
    EXPECT_EQ(&testNumber.method(kb.test.factorial, { kb.id.input, _0_ }), &_1_);
    EXPECT_EQ(&testNumber.method(kb.test.factorial, { kb.id.input, _1_ }), &_1_);
    EXPECT_EQ(&testNumber.method(kb.test.factorial, { kb.id.input, _2_ }), &_2_);
    EXPECT_EQ(&testNumber.method(kb.test.factorial, { kb.id.input, _3_ }), &_6_);
    EXPECT_EQ(&testNumber.method(kb.test.factorial, { kb.id.input, _4_ }), &kb.pools.numbers.get(24));
    EXPECT_EQ(&testNumber.method(kb.test.factorial, { kb.id.input, _5_ }), &kb.pools.numbers.get(120));
#endif

    // TODO
    // inline methods
    // maybe create a Set<type>?
    // type checking
    // create an own type for every function to able to save return values to the fn object
    // remove .label() from CellI
}

TEST_F(CellTest, List)
{
    Object list(kb, kb.type.List, kb.id.constructor);

    printAs.value(list);
    printAs.cell(list);
    EXPECT_EQ(&list[kb.dimensions.size], &_0_);
    EXPECT_EQ(&list.method(kb.dimensions.size), &_0_);
    EXPECT_EQ(&list.method(kb.sequence.empty), &true_);
    EXPECT_EQ(&list[kb.id.objectType], &kb.type.Cell);

    list.method(kb.sequence.add, { kb.id.value, _1_ });
    EXPECT_EQ(&list[kb.dimensions.size], &_1_);
    EXPECT_EQ(&list.method(kb.dimensions.size), &_1_);
    EXPECT_EQ(&list.method(kb.sequence.empty), &false_);

    CellI& firstItem = list[kb.sequence.first];
    EXPECT_EQ(&firstItem, &list[kb.sequence.last]);
    EXPECT_EQ(firstItem.has(kb.sequence.previous), false);
    EXPECT_EQ(firstItem.has(kb.sequence.next), false);
    EXPECT_EQ(&firstItem[kb.id.value], &_1_);
    printAs.value(list);

    list.method(kb.sequence.add, { kb.id.value, kb.pools.numbers.get(2) });
    EXPECT_EQ(&list[kb.dimensions.size], &_2_);
    EXPECT_EQ(&list.method(kb.dimensions.size), &_2_);
    EXPECT_EQ(&list.method(kb.sequence.empty), &false_);

    CellI& secondItem = list[kb.sequence.last];
    EXPECT_EQ(&firstItem, &list[kb.sequence.first]);
    EXPECT_NE(&firstItem, &list[kb.sequence.last]);

    EXPECT_EQ(firstItem.has(kb.sequence.previous), false);
    EXPECT_EQ(&firstItem[kb.sequence.next], &secondItem);
    EXPECT_EQ(&firstItem[kb.id.value], &_1_);

    EXPECT_EQ(&secondItem[kb.sequence.previous], &firstItem);
    EXPECT_EQ(secondItem.has(kb.sequence.next), false);
    EXPECT_EQ(&secondItem[kb.id.value], &_2_);

    printAs.value(list);
    list.method(kb.sequence.add, { kb.id.value, _3_ });
    EXPECT_EQ(&list[kb.dimensions.size], &_3_);
    EXPECT_EQ(&list.method(kb.dimensions.size), &_3_);
    EXPECT_EQ(&list.method(kb.sequence.empty), &false_);

    CellI& thirdItem = list[kb.sequence.last];
    EXPECT_EQ(&firstItem, &list[kb.sequence.first]);
    EXPECT_NE(&firstItem, &list[kb.sequence.last]);

    EXPECT_EQ(firstItem.has(kb.sequence.previous), false);
    EXPECT_EQ(&firstItem[kb.sequence.next], &secondItem);
    EXPECT_EQ(&firstItem[kb.id.value], &_1_);

    EXPECT_EQ(&secondItem[kb.sequence.previous], &firstItem);
    EXPECT_EQ(&secondItem[kb.sequence.next], &thirdItem);
    EXPECT_EQ(&secondItem[kb.id.value], &_2_);

    EXPECT_EQ(&thirdItem[kb.sequence.previous], &secondItem);
    EXPECT_EQ(thirdItem.has(kb.sequence.next), false);
    EXPECT_EQ(&thirdItem[kb.id.value], &_3_);

    printAs.value(list);
    CellI& size = list.method(kb.dimensions.size);
    printAs.value(size);
}

TEST_F(CellTest, ListOfNumber)
{
    CellI& ListOfNumber = kb.type.List.smethod(kb.id.template_, { kb.id.objectType, kb.type.Number });
    Object list(kb, ListOfNumber, kb.id.constructor);

    printAs.value(list);
    printAs.cell(list);
    EXPECT_EQ(&list[kb.dimensions.size], &_0_);
    EXPECT_EQ(&list.method(kb.dimensions.size), &_0_);
    EXPECT_EQ(&list.method(kb.sequence.empty), &true_);
    EXPECT_EQ(&list[kb.id.objectType], &kb.type.Number);

    list.method(kb.sequence.add, { kb.id.value, _1_ });
    EXPECT_EQ(&list[kb.dimensions.size], &_1_);
    EXPECT_EQ(&list.method(kb.dimensions.size), &_1_);
    EXPECT_EQ(&list.method(kb.sequence.empty), &false_);

    CellI& firstItem = list[kb.sequence.first];
    EXPECT_EQ(&firstItem, &list[kb.sequence.last]);
    EXPECT_EQ(firstItem.has(kb.sequence.previous), false);
    EXPECT_EQ(firstItem.has(kb.sequence.next), false);
    EXPECT_EQ(&firstItem[kb.id.value], &_1_);
    printAs.value(list);

    list.method(kb.sequence.add, { kb.id.value, kb.pools.numbers.get(2) });
    EXPECT_EQ(&list[kb.dimensions.size], &_2_);
    EXPECT_EQ(&list.method(kb.dimensions.size), &_2_);
    EXPECT_EQ(&list.method(kb.sequence.empty), &false_);

    CellI& secondItem = list[kb.sequence.last];
    EXPECT_EQ(&firstItem, &list[kb.sequence.first]);
    EXPECT_NE(&firstItem, &list[kb.sequence.last]);

    EXPECT_EQ(firstItem.has(kb.sequence.previous), false);
    EXPECT_EQ(&firstItem[kb.sequence.next], &secondItem);
    EXPECT_EQ(&firstItem[kb.id.value], &_1_);

    EXPECT_EQ(&secondItem[kb.sequence.previous], &firstItem);
    EXPECT_EQ(secondItem.has(kb.sequence.next), false);
    EXPECT_EQ(&secondItem[kb.id.value], &_2_);

    printAs.value(list);
    list.method(kb.sequence.add, { kb.id.value, _3_ });
    EXPECT_EQ(&list[kb.dimensions.size], &_3_);
    EXPECT_EQ(&list.method(kb.dimensions.size), &_3_);
    EXPECT_EQ(&list.method(kb.sequence.empty), &false_);

    CellI& thirdItem = list[kb.sequence.last];
    EXPECT_EQ(&firstItem, &list[kb.sequence.first]);
    EXPECT_NE(&firstItem, &list[kb.sequence.last]);

    EXPECT_EQ(firstItem.has(kb.sequence.previous), false);
    EXPECT_EQ(&firstItem[kb.sequence.next], &secondItem);
    EXPECT_EQ(&firstItem[kb.id.value], &_1_);

    EXPECT_EQ(&secondItem[kb.sequence.previous], &firstItem);
    EXPECT_EQ(&secondItem[kb.sequence.next], &thirdItem);
    EXPECT_EQ(&secondItem[kb.id.value], &_2_);

    EXPECT_EQ(&thirdItem[kb.sequence.previous], &secondItem);
    EXPECT_EQ(thirdItem.has(kb.sequence.next), false);
    EXPECT_EQ(&thirdItem[kb.id.value], &_3_);

    printAs.value(list);
    CellI& size = list.method(kb.dimensions.size);
    printAs.value(size);
}

TEST_F(CellTest, Map)
{
    Object map(kb, kb.type.Map, kb.id.constructor);

    printAs.value(map);
    printAs.cell(map);
    EXPECT_EQ(&map[kb.dimensions.size], &_0_);
    EXPECT_EQ(&map.method(kb.dimensions.size), &_0_);
    EXPECT_EQ(&map.method(kb.sequence.empty), &true_);
    EXPECT_EQ(&map[kb.id.keyType], &kb.type.Cell);
    EXPECT_EQ(&map[kb.id.objectType], &kb.type.Cell);

    map.method(kb.sequence.add, { id.key, _1_ }, { id.value, kb.colors.red });
    printAs.value(map);
    printAs.cell(map);
    EXPECT_EQ(&map[kb.dimensions.size], &_1_);
    EXPECT_EQ(&map.method(kb.dimensions.size), &_1_);
    EXPECT_EQ(&map[id.list][kb.dimensions.size], &_1_);
    EXPECT_EQ(&map[id.list][kb.id.objectType], &kb.type.Cell);
    EXPECT_EQ(&map[id.list][kb.sequence.first][kb.id.value], &kb.colors.red);
    EXPECT_EQ(&map[id.list][kb.sequence.first], &map[id.list][kb.sequence.last]);
    EXPECT_TRUE(map[id.index].has(_1_));
    EXPECT_EQ(&map[id.index][_1_], &kb.colors.red);
    EXPECT_EQ(&map.method(kb.sequence.empty), &false_);
    EXPECT_TRUE(map[id.index][kb.id.type][kb.id.memberOf][kb.id.index].has(kb.type.Index));

    map.method(kb.sequence.add, { id.key, _2_ }, { id.value, kb.colors.green });
    map.method(kb.sequence.add, { id.key, _3_ }, { id.value, kb.colors.blue });
    EXPECT_EQ(&map[id.index][_1_], &kb.colors.red);
    EXPECT_EQ(&map[id.index][_2_], &kb.colors.green);
    EXPECT_EQ(&map[id.index][_3_], &kb.colors.blue);
    printAs.value(map);
    printAs.cell(map);
}

TEST_F(CellTest, MapTypes)
{
    Object map(kb, kb.type.Map, kb.id.constructor);
    printAs.value(map.type());
    printAs.value(map[kb.id.list].type());
    printAs.value(map[kb.id.index], "map[kb.id.index]");
    printAs.value(map[kb.id.index].type(), "map[kb.id.index].type()");
    map.method(kb.sequence.add, { id.key, _1_ }, { id.value, kb.colors.red });
    printAs.value(map[kb.id.index], "map[kb.id.index]");
    printAs.value(map[kb.id.index].type(), "map[kb.id.index].type()");
    printAs.cell(map[kb.id.index].type()[kb.id.slots], "map[kb.id.index].type()[kb.id.slots]");
    printAs.cell(map[kb.id.index].type()[kb.id.slots][kb.id.index], "map[kb.id.index].type()[kb.id.slots][kb.id.index]");
    printAs.value(map[kb.id.index].type()[kb.id.slots][kb.id.index], "map[kb.id.index].type()[kb.id.slots][kb.id.index]");
}

TEST_F(CellTest, BuiltInMapTypes)
{
    Map& map = *new cells::Map(kb, kb.type.Number, kb.type.Color);
    printAs.value(map.type());
    printAs.value(map[kb.id.list].type());
    printAs.cell(map[kb.id.index], "map[kb.id.index]");
    printAs.value(map[kb.id.index], "map[kb.id.index]");
    printAs.value(map[kb.id.index].type(), "map[kb.id.index].type()");
    map.add(_1_, kb.colors.red);
    printAs.cell(map[kb.id.index], "map[kb.id.index]");
    printAs.value(map[kb.id.index], "map[kb.id.index]");
    printAs.value(map[kb.id.index].type(), "map[kb.id.index].type()");
    printAs.cell(map[kb.id.index].type()[kb.id.slots], "map[kb.id.index].type()[kb.id.slots]");
    EXPECT_EQ(&map[kb.id.index][_1_], &kb.colors.red);
    EXPECT_EQ(&map[kb.id.index].type()[kb.id.slots][kb.dimensions.size], &_1_);
    EXPECT_EQ(&map[kb.id.index].type()[kb.id.slots][kb.id.keyType], &kb.type.Cell);
    EXPECT_EQ(&map[kb.id.index].type()[kb.id.slots][kb.id.objectType], &kb.type.Slot);
    EXPECT_EQ(&map[kb.id.index].type()[kb.id.slots][kb.id.listType], &kb.type.ListOf(kb.type.Slot));
    EXPECT_EQ(&map[kb.id.index].type()[kb.id.slots][kb.id.list][kb.dimensions.size], &_1_);
    EXPECT_EQ(&map[kb.id.index].type()[kb.id.slots][kb.id.index][_1_][kb.id.slotRole], &_1_);
    EXPECT_EQ(&map[kb.id.index].type()[kb.id.slots][kb.id.index][_1_][kb.id.slotType], &kb.type.Color);

    EXPECT_TRUE(map[kb.id.index].type().has(kb.id.slots));
    EXPECT_TRUE(map[kb.id.index].type()[kb.id.slots].has(kb.id.index));
    EXPECT_TRUE(map[kb.id.index].type()[kb.id.slots][kb.id.index].has(kb.id.type));
    EXPECT_EQ(&map[kb.id.index].type()[kb.id.slots][kb.id.index].type(), &map[kb.id.index].type()[kb.id.slots][kb.id.index].type()[kb.id.slots][kb.id.index].type());

    EXPECT_TRUE(map[kb.id.index].type()[kb.id.slots][kb.id.index].type().has(kb.id.slots));
    printAs.cell(map[kb.id.index].type()[kb.id.slots], "map[kb.id.index].type()[kb.id.slots][]");
    printAs.cell(map[kb.id.index].type()[kb.id.slots][kb.id.index], "map[kb.id.index].type()[kb.id.slots][kb.id.index]");
    printAs.value(map[kb.id.index].type()[kb.id.slots][kb.id.index].type(), "map[kb.id.index].type()[kb.id.slots][kb.id.index].type()");
    printAs.value(map[kb.id.index].type()[kb.id.slots][kb.id.index].type()[kb.id.slots], "map[kb.id.index].type()[kb.id.slots][kb.id.index].type()[kb.id.slots]");
    EXPECT_EQ(&map[kb.id.index].type()[kb.id.slots][kb.id.index].type()[kb.id.slots][kb.dimensions.size], &_1_);
    EXPECT_EQ(&map[kb.id.index].type()[kb.id.slots][kb.id.index].type()[kb.id.slots][kb.id.index][_1_][kb.id.slotRole], &_1_);
    EXPECT_EQ(&map[kb.id.index].type()[kb.id.slots][kb.id.index].type()[kb.id.slots][kb.id.index][_1_][kb.id.slotType], &kb.type.Color);
    printAs.value(map[kb.id.index].type()[kb.id.slots][kb.id.index].type()[kb.id.slots][kb.id.index], "map[kb.id.index].type()[kb.id.slots][kb.id.index].type()[kb.id.slots][kb.id.index]");
    printAs.value(map[kb.id.index].type()[kb.id.slots][kb.id.index].type()[kb.id.slots][kb.id.index].type(), "map[kb.id.index].type()[kb.id.slots][kb.id.index].type()[kb.id.slots][kb.id.index].type()");
}


TEST_F(CellTest, MapTemplateTypes)
{
    CellI& MapNumberToColor = kb.type.Map.smethod(kb.id.template_, { kb.id.keyType, kb.type.Number }, { kb.id.objectType, kb.type.Color });
    Object map(kb, MapNumberToColor, kb.id.constructor);
    printAs.value(map.type());
    printAs.value(map[kb.id.list].type());
    printAs.cell(map[kb.id.index], "map[kb.id.index]");
    printAs.value(map[kb.id.index], "map[kb.id.index]");
    printAs.value(map[kb.id.index].type(), "map[kb.id.index].type()");
    map.method(kb.sequence.add, { id.key, _1_ }, { id.value, kb.colors.red });
    printAs.cell(map[kb.id.index], "map[kb.id.index]");
    printAs.value(map[kb.id.index], "map[kb.id.index]");
    printAs.value(map[kb.id.index].type(), "map[kb.id.index].type()");
    printAs.cell(map[kb.id.index].type()[kb.id.slots], "map[kb.id.index].type()[kb.id.slots]");
    EXPECT_EQ(&map[kb.id.index].type()[kb.id.slots][kb.dimensions.size], &_1_);
    EXPECT_EQ(&map[kb.id.index].type()[kb.id.slots][kb.id.keyType], &kb.type.Cell);
    EXPECT_EQ(&map[kb.id.index].type()[kb.id.slots][kb.id.objectType], &kb.type.Slot);
    EXPECT_EQ(&map[kb.id.index].type()[kb.id.slots][kb.id.listType], &kb.type.ListOf(kb.type.Slot));
    EXPECT_EQ(&map[kb.id.index].type()[kb.id.slots][kb.id.list][kb.dimensions.size], &_1_);
    EXPECT_EQ(&map[kb.id.index].type()[kb.id.slots][kb.id.index][_1_][kb.id.slotRole], &_1_);
    EXPECT_EQ(&map[kb.id.index].type()[kb.id.slots][kb.id.index][_1_][kb.id.slotType], &kb.type.Color);

    EXPECT_TRUE(map[kb.id.index].type().has(kb.id.slots));
    EXPECT_TRUE(map[kb.id.index].type()[kb.id.slots].has(kb.id.index));
    EXPECT_TRUE(map[kb.id.index].type()[kb.id.slots][kb.id.index].has(kb.id.type));
    EXPECT_EQ(&map[kb.id.index].type()[kb.id.slots][kb.id.index].type(), &map[kb.id.index].type()[kb.id.slots][kb.id.index].type()[kb.id.slots][kb.id.index].type());

    EXPECT_TRUE(map[kb.id.index].type()[kb.id.slots][kb.id.index].type().has(kb.id.slots));
    printAs.cell(map[kb.id.index].type()[kb.id.slots], "map[kb.id.index].type()[kb.id.slots][]");
    printAs.cell(map[kb.id.index].type()[kb.id.slots][kb.id.index], "map[kb.id.index].type()[kb.id.slots][kb.id.index]");
    printAs.value(map[kb.id.index].type()[kb.id.slots][kb.id.index].type(), "map[kb.id.index].type()[kb.id.slots][kb.id.index].type()");
    printAs.value(map[kb.id.index].type()[kb.id.slots][kb.id.index].type()[kb.id.slots], "map[kb.id.index].type()[kb.id.slots][kb.id.index].type()[kb.id.slots]");
    EXPECT_EQ(&map[kb.id.index].type()[kb.id.slots][kb.id.index].type()[kb.id.slots][kb.dimensions.size], &_1_);
    EXPECT_EQ(&map[kb.id.index].type()[kb.id.slots][kb.id.index].type()[kb.id.slots][kb.id.index][_1_][kb.id.slotRole], &_1_);
    EXPECT_EQ(&map[kb.id.index].type()[kb.id.slots][kb.id.index].type()[kb.id.slots][kb.id.index][_1_][kb.id.slotType], &kb.type.Slot);
    printAs.value(map[kb.id.index].type()[kb.id.slots][kb.id.index].type()[kb.id.slots][kb.id.index], "map[kb.id.index].type()[kb.id.slots][kb.id.index].type()[kb.id.slots][kb.id.index]");
    printAs.value(map[kb.id.index].type()[kb.id.slots][kb.id.index].type()[kb.id.slots][kb.id.index].type(), "map[kb.id.index].type()[kb.id.slots][kb.id.index].type()[kb.id.slots][kb.id.index].type()");
}

TEST_F(CellTest, MapNumberToColor)
{
    CellI& MapNumberToColor = kb.type.Map.smethod(kb.id.template_, { kb.id.keyType, kb.type.Number }, { kb.id.objectType, kb.type.Color });
    Object map(kb, MapNumberToColor, kb.id.constructor);

    printAs.value(map);
    printAs.cell(map);
    EXPECT_EQ(&map[kb.dimensions.size], &_0_);
    EXPECT_EQ(&map.method(kb.dimensions.size), &_0_);
    EXPECT_EQ(&map.method(kb.sequence.empty), &true_);
    EXPECT_EQ(&map[kb.id.keyType], &kb.type.Number);
    EXPECT_EQ(&map[kb.id.objectType], &kb.type.Color);

    map.method(kb.sequence.add, { id.key, _1_}, { id.value, kb.colors.red });
    printAs.value(map);
    printAs.cell(map);
    EXPECT_EQ(&map[kb.dimensions.size], &_1_);
    EXPECT_EQ(&map.method(kb.dimensions.size), &_1_);
    EXPECT_EQ(&map[id.list][kb.dimensions.size], &_1_);
    EXPECT_EQ(&map[id.list][kb.id.objectType], &kb.type.Color);
    EXPECT_EQ(&map[id.list][kb.sequence.first][kb.id.value], &kb.colors.red);
    EXPECT_EQ(&map[id.list][kb.sequence.first], &map[id.list][kb.sequence.last]);
    EXPECT_TRUE(map[id.index].has(_1_));
    EXPECT_EQ(&map[id.index][_1_], &kb.colors.red);
    EXPECT_EQ(&map.method(kb.sequence.empty), &false_);
    EXPECT_TRUE(map[id.index][kb.id.type][kb.id.memberOf][kb.id.index].has(kb.type.Index));

    map.method(kb.sequence.add, { id.key, _2_ }, { id.value, kb.colors.green });
    map.method(kb.sequence.add, { id.key, _3_ }, { id.value, kb.colors.blue });
    EXPECT_EQ(&map[id.index][_1_], &kb.colors.red);
    EXPECT_EQ(&map[id.index][_2_], &kb.colors.green);
    EXPECT_EQ(&map[id.index][_3_], &kb.colors.blue);
    printAs.value(map);
    printAs.cell(map);
}

TEST_F(CellTest, ListItem)
{
    Object listItem(kb, kb.type.ListItem, kb.id.constructor, { id.value, kb.colors.green });

    EXPECT_EQ(&listItem[kb.id.value], &kb.colors.green);

    EXPECT_EQ(&listItem.type()[kb.id.memberOf][kb.dimensions.size], &_1_);
    EXPECT_TRUE(listItem.type()[kb.id.memberOf][kb.id.index].has(kb.type.Iterator));

    EXPECT_EQ(&listItem.type()[kb.id.slots][kb.dimensions.size], &_3_);
    EXPECT_TRUE(listItem.type()[kb.id.slots][kb.id.index].has(kb.sequence.previous));
    EXPECT_TRUE(listItem.type()[kb.id.slots][kb.id.index].has(kb.sequence.next));
    EXPECT_TRUE(listItem.type()[kb.id.slots][kb.id.index].has(kb.id.value));

    EXPECT_TRUE(listItem.type()[kb.id.methods][kb.id.index].has(kb.id.template_));
    EXPECT_TRUE(listItem.type()[kb.id.methods][kb.id.index].has(kb.id.constructor));
}

TEST_F(CellTest, ListItemTemplate)
{
    CellI& ListItemNumber = kb.type.ListItem.smethod(kb.id.template_, { kb.id.objectType, kb.type.Number });
    Object listItemNumber(kb, ListItemNumber, kb.id.constructor, { id.value, _1_ });

    EXPECT_EQ(&listItemNumber[kb.id.value], &_1_);

    printAs.value(kb.type.ListItem, "type.ListItem");
    printAs.value(ListItemNumber, "listItemNumber");
    printAs.value(kb.type.ListItem[kb.id.slots][kb.id.list], "type.ListItem[slots]");
    printAs.value(ListItemNumber[kb.id.slots][kb.id.list], "listItemNumber[slots]");

    EXPECT_EQ(&ListItemNumber[kb.id.memberOf][kb.dimensions.size], &_1_);
    EXPECT_TRUE(ListItemNumber[kb.id.memberOf][kb.id.index].has(kb.type.ListItem));

    EXPECT_EQ(&ListItemNumber[kb.id.slots][kb.dimensions.size], &_3_);
    EXPECT_TRUE(ListItemNumber[kb.id.slots][kb.id.index].has(kb.sequence.previous));
    EXPECT_TRUE(ListItemNumber[kb.id.slots][kb.id.index].has(kb.sequence.next));
    EXPECT_TRUE(ListItemNumber[kb.id.slots][kb.id.index].has(kb.id.value));

    EXPECT_EQ(&ListItemNumber[kb.id.methods], &kb.type.ListItem[kb.id.methods]);
}

TEST_F(CellTest, ListTemplate)
{
    CellI& ListOfNumbers = kb.type.List.smethod(kb.id.template_, { kb.id.objectType, kb.type.Number });

    EXPECT_EQ(&ListOfNumbers[id.subTypes][kb.dimensions.size], &_2_);
    EXPECT_TRUE(ListOfNumbers[id.subTypes][id.index].has(id.objectType));
    EXPECT_EQ(&ListOfNumbers[id.subTypes][id.index][id.objectType], &kb.type.Number);
    EXPECT_TRUE(ListOfNumbers[id.subTypes][id.index].has(id.itemType));
    CellI& ListItemType = ListOfNumbers[id.subTypes][id.index][id.itemType];
    EXPECT_EQ(&ListItemType[kb.id.slots][kb.id.index][kb.id.value][kb.id.slotType], &kb.type.Number);
    EXPECT_NE(&ListItemType, &kb.type.ListItem);
    EXPECT_EQ(&ListItemType[kb.id.memberOf][kb.dimensions.size], &_1_);
    EXPECT_TRUE(ListItemType[kb.id.memberOf][kb.id.index].has(kb.type.ListItem));

    EXPECT_EQ(&ListItemType[kb.id.slots][kb.dimensions.size], &_3_);
    EXPECT_TRUE(ListItemType[kb.id.slots][kb.id.index].has(kb.sequence.previous));
    EXPECT_TRUE(ListItemType[kb.id.slots][kb.id.index].has(kb.sequence.next));
    EXPECT_TRUE(ListItemType[kb.id.slots][kb.id.index].has(kb.id.value));

    EXPECT_EQ(&ListItemType[kb.id.methods], &kb.type.ListItem[kb.id.methods]);

    EXPECT_EQ(&ListOfNumbers[kb.id.methods], &kb.type.List[kb.id.methods]);

    EXPECT_EQ(&ListOfNumbers[kb.id.slots][kb.dimensions.size], &_5_);
    EXPECT_TRUE(ListOfNumbers[kb.id.slots][kb.id.index].has(kb.sequence.first));
    EXPECT_EQ(&ListOfNumbers[kb.id.slots][kb.id.index][kb.sequence.first][kb.id.slotType], &ListItemType);

    EXPECT_TRUE(ListOfNumbers[kb.id.slots][kb.id.index].has(kb.sequence.last));
    EXPECT_EQ(&ListOfNumbers[kb.id.slots][kb.id.index][kb.sequence.last][kb.id.slotType], &ListItemType);

    EXPECT_TRUE(ListOfNumbers[kb.id.slots][kb.id.index].has(kb.dimensions.size));
    EXPECT_EQ(&ListOfNumbers[kb.id.slots][kb.id.index][kb.dimensions.size][kb.id.slotType], &kb.type.Number);

    EXPECT_TRUE(ListOfNumbers[kb.id.slots][kb.id.index].has(kb.id.objectType));
    EXPECT_EQ(&ListOfNumbers[kb.id.slots][kb.id.index][kb.id.objectType][kb.id.slotType], &kb.type.Number);

    EXPECT_TRUE(ListOfNumbers[kb.id.slots][kb.id.index].has(kb.id.itemType));
    EXPECT_EQ(&ListOfNumbers[kb.id.slots][kb.id.index][kb.id.itemType][kb.id.slotType], &ListItemType);

    EXPECT_TRUE(ListOfNumbers[kb.id.memberOf][kb.id.index].has(kb.type.List));
    EXPECT_EQ(&ListOfNumbers[kb.id.memberOf][kb.dimensions.size], &_1_);

    printAs.value(kb.type.List, "type.List");
    printAs.value(ListOfNumbers, "ListOfNumbers");

    Object listOfNumbers(kb, ListOfNumbers, kb.id.constructor);
    CellI& listItemNumber = listOfNumbers[kb.id.itemType];
    EXPECT_EQ(&ListItemType, &listItemNumber);
    EXPECT_EQ(&ListOfNumbers[id.subTypes][id.index][id.itemType], &listItemNumber);
}

TEST_F(CellTest, FunctionTypes)
{
    CellI& function = kb.type.Type_[kb.id.methods][kb.id.index][kb.methods.addSlot];
    printAs.cell(function.type(), "function");
    printAs.cell(function.type()[kb.id.subTypes][kb.id.index][kb.id.localVars], "function[kb.id.subTypes][kb.id.localVars]");
    printAs.value(function.type()[kb.id.subTypes][kb.id.index][kb.id.localVars], "function[kb.id.subTypes][kb.id.localVars]");
}

TEST_F(CellTest, CreatedTypeWithConstructor)
{
    Object emptySubTypesMap(kb, kb.type.MapCellToType, kb.id.constructor);
    Object emptyMethodsMap(kb, kb.type.MapCellToType, kb.id.constructor);
    Object slotsMap(kb, kb.type.MapCellToSlot, kb.id.constructor);
    slotsMap.method(kb.sequence.add, { kb.id.key, kb.dimensions.size }, { kb.id.value, kb.type.slot(kb.dimensions.size, kb.type.Number) });

    Object newType(kb, kb.type.Type_, kb.id.constructor,
                   { id.slots, slotsMap },
                   { id.subTypes, emptySubTypesMap },
                   { id.memberOf, kb.map(kb.type.Cell, kb.type.Cell) },
                   { id.methods, emptyMethodsMap });
    printAs.value(newType[id.slots]);
    printAs.value(newType[id.slots][id.list], "newType[id.slots][id.list]");
    printAs.value(newType[id.memberOf][id.list], "newType[id.memberOf][id.list]");
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

    EXPECT_EQ(&picture[kb.id.type], &kb.type.Picture);
    EXPECT_EQ(&picture[kb.dimensions.width], &kb.pools.numbers.get(3));
    EXPECT_EQ(&picture[kb.dimensions.height], &kb.pools.numbers.get(3));
    EXPECT_EQ(&picture[kb.visualization.pixels][kb.id.type], &kb.type.ListOf(kb.type.Pixel));

    CellI& ListOfPixels = kb.type.List.smethod(kb.id.template_, { kb.id.objectType, kb.type.Pixel });
    Object listOfPixels(kb, ListOfPixels, kb.id.constructor, "listOfPixels");
    listOfPixels.method(kb.sequence.add, { kb.id.value, picture[kb.visualization.pixels][kb.sequence.first][kb.id.value] });
    listOfPixels.method(kb.sequence.add, { kb.id.value, picture[kb.visualization.pixels][kb.sequence.first][kb.sequence.next][kb.id.value] });
    printAs.value(listOfPixels);
}

TEST_F(CellTest, BasicObjectTest)
{
    Object testType(kb, kb.type.Type_, "Test");
    Object emptyList(kb, kb.type.List, kb.id.constructor);

    testType.method(kb.methods.addSlots, { kb.id.list, emptyList });

    testType.method(kb.methods.addSlots, { kb.id.list, kb.list(
        kb.type.slot(kb.id.result, kb.type.Digit),
        kb.type.slot(kb.id.value, kb.type.Number))}); // TODO implement type checking

    Object object(kb, testType, "testObject");

    EXPECT_EQ(object.label(), "testObject");
    EXPECT_EQ(&object.type(), &testType);

    EXPECT_ANY_THROW(&object[kb.id.value]);
    EXPECT_NO_THROW(object.set(kb.id.value, kb.pools.numbers.get(42)));
    printAs.value(object[kb.id.value]);
    EXPECT_ANY_THROW(object.set(kb.id.argument, kb.pools.numbers.get(42)));
}

TEST_F(CellTest, BasicControlOpTest)
{
    Object testValue1(kb, kb.type.op.ConstVar);
    testValue1.set(kb.id.value, kb.type.Char);

    Object testValue2(kb, kb.type.op.ConstVar);
    testValue2.set(kb.id.value, kb.type.Color);

    Object sameOpEq(kb, kb.type.op.Same, "sameOpEq");
    sameOpEq.set(kb.id.lhs, testValue1);
    sameOpEq.set(kb.id.rhs, testValue1);

    Object sameOpNe(kb, kb.type.op.Same, "sameOpNe");
    sameOpNe.set(kb.id.lhs, testValue1);
    sameOpNe.set(kb.id.rhs, testValue2);

    sameOpEq();
    sameOpNe();

    printAs.value(sameOpEq[kb.id.value], "testValue1 == testValue1");
    printAs.value(sameOpNe[kb.id.value], "testValue1 != testValue1");

    EXPECT_EQ(&sameOpEq[kb.id.value], &true_);
    EXPECT_EQ(&sameOpNe[kb.id.value], &false_);
}

TEST_F(CellTest, BasicControlAddTest)
{
    Object start(kb, kb.type.op.ConstVar);
    start.set(kb.id.value, kb.pools.numbers.get(42));

    Object value10(kb, kb.type.op.ConstVar);
    value10.set(kb.id.value, kb.pools.numbers.get(10));

    Object add10(kb, kb.type.op.Add, "add10");
    add10.set(kb.id.lhs, start);
    add10.set(kb.id.rhs, value10);
    add10();
    std::cout << "42 + 10 = ";
    printAs.value(add10[kb.id.value]);

    EXPECT_EQ(&add10[kb.id.value], &kb.pools.numbers.get(52));
}

TEST_F(CellTest, CreatingCustomType)
{
    Object colorRed(kb, kb.type.Cell, "red");
    Object colorGreen(kb, kb.type.Cell, "green");
    Object colorBlue(kb, kb.type.Cell, "blue");

    Object colorClass(kb, kb.type.Type_, "Color");
#if 1 // TODO
    Object* slotMapPtr = new Object(kb, kb.type.MapCellToSlot, kb.id.constructor);
    colorClass.set(id.slots, *slotMapPtr);
#endif

    colorClass.method(kb.methods.addSlots, { kb.id.list, kb.list(kb.type.slot(colorRed, kb.type.Number), kb.type.slot(colorGreen, kb.type.Number), kb.type.slot(colorBlue, kb.type.Number)) });

    Object redColor(kb, colorClass, "redColor");
    redColor.set(colorRed, kb.pools.numbers.get(255));
    redColor.set(colorGreen, kb.pools.numbers.get(0));
    redColor.set(colorBlue, kb.pools.numbers.get(0));

    printAs.value(colorClass, "colorClass:");
    printAs.value(colorClass[kb.id.slots][kb.id.list], "colorClass::slots");
    printAs.value(colorClass[kb.id.slots][kb.id.index][colorRed]);
    EXPECT_TRUE(colorClass[kb.id.slots][kb.id.index].has(colorRed));
    EXPECT_TRUE(colorClass[kb.id.slots][kb.id.index].has(colorGreen));
    EXPECT_TRUE(colorClass[kb.id.slots][kb.id.index].has(colorBlue));
    printAs.value(colorClass[kb.id.slots][kb.id.index][colorRed][kb.id.type], "colorClass slot of colorRed");
    EXPECT_EQ(&colorClass[kb.id.slots][kb.id.index][colorRed][kb.id.type], &kb.type.Slot);
    EXPECT_EQ(&colorClass[kb.id.slots][kb.id.index][colorRed][kb.id.type], &kb.type.Slot);
    EXPECT_EQ(&colorClass[kb.id.slots][kb.id.index][colorGreen][kb.id.type], &kb.type.Slot);
    EXPECT_EQ(&colorClass[kb.id.slots][kb.id.index][colorBlue][kb.id.type], &kb.type.Slot);
    printAs.value(redColor);

    printAs.cell(redColor);
    printAs.cell(colorClass[kb.id.slots][kb.id.index][colorRed], "colorClass[kb.id.slots][kb.id.index][colorRed]");
    printAs.cell(colorClass[kb.id.slots][kb.id.index], "colorClass[kb.id.slots][kb.id.index]");

    printAs.svgStruct(redColor, "redColor");
    printAs.svgStruct(colorRed, "colorRed");
    printAs.svgStruct(colorClass, "Color");
    printAs.svgStruct(colorClass[kb.id.slots][kb.id.index], "SlotIndex of Color");
    printAs.svgStruct(colorClass[kb.id.slots][kb.id.list], "SlotList of Color");
    printAs.svgStruct(colorClass[kb.id.slots][kb.id.list][kb.sequence.first], "SlotListItem1 of Color");
    printAs.svgStruct(colorClass[kb.id.slots][kb.id.list][kb.sequence.first][kb.id.value], "Slot1 of Color");
    printAs.svgStruct(colorClass[kb.id.slots][kb.id.list][kb.sequence.first][kb.sequence.next], "SlotListItem2 of Color");
    printAs.svgStruct(colorClass[kb.id.slots][kb.id.list][kb.sequence.first][kb.sequence.next][kb.id.value], "Slot2 of Color");
    printAs.value(colorClass[kb.id.slots][kb.id.index], "colorClass[kb.id.slots][kb.id.index]");
    printAs.value(colorClass[kb.id.slots][kb.id.index][kb.id.type], "colorClass[kb.id.slots][kb.id.index][kb.id.type]");
    printAs.value(colorClass[kb.id.slots][kb.id.index][kb.id.type][kb.id.slots], "colorClass[kb.id.slots][kb.id.index][kb.id.type][kb.id.slots]");
    printAs.value(colorClass[kb.id.slots][kb.id.index][kb.id.type][kb.id.slots][kb.id.index], "colorClass[kb.id.slots][kb.id.index][kb.id.type][kb.id.slots][kb.id.index]");
    printAs.cell(colorClass);

    printAs.svg(redColor);
    printAs.svg(colorClass[kb.id.slots][kb.id.index][colorRed]);
    printAs.svg(colorClass);
}

TEST_F(CellTest, CreatingNumber)
{
    Number& number_0   = kb.pools.numbers.get(0);
    Number& number_255 = kb.pools.numbers.get(255);

    printAs.value(number_255);
    printAs.value(number_255[kb.id.value][kb.sequence.first][kb.id.value]);

    printAs.svgStruct(kb.id.emptyObject, "emptyObject");

    printAs.cell(number_255);
    printAs.cell(number_255[kb.numbers.sign]);
    printAs.cell(number_255[kb.id.value]);
    printAs.cell(number_255[kb.id.value][kb.sequence.first]);
    printAs.cell(number_255[kb.id.value][kb.sequence.first][kb.id.value]);
    printAs.cell(number_255[kb.id.value][kb.sequence.last]);
    printAs.cell(number_255[kb.id.value][kb.dimensions.size]);

    printAs.svg(number_255);
    printAs.svg(number_255[kb.numbers.sign]);
    printAs.svg(number_255[kb.id.value]);
    printAs.svg(number_255[kb.id.value][kb.sequence.first]);
    printAs.svg(number_255[kb.id.value][kb.sequence.first][kb.id.value]);
    printAs.svg(number_255[kb.id.value][kb.sequence.last]);
    printAs.svg(number_255[kb.id.value][kb.dimensions.size]);
}

TEST_F(CellTest, NextgenList)
{
    nextgen::List list(kb, kb.type.Number);
    EXPECT_FALSE(list.has(kb.sequence.first));
    EXPECT_FALSE(list.has(kb.sequence.last));
    EXPECT_EQ(&list[kb.dimensions.size], &_0_);
    EXPECT_EQ(&list[kb.id.objectType], &kb.type.Number);
    EXPECT_TRUE(list.empty());
    printAs.value(list);

    auto& item1 = *list.add(_1_);
    EXPECT_TRUE(list.has(kb.sequence.first));
    EXPECT_TRUE(list.has(kb.sequence.last));
    EXPECT_EQ(&list[kb.dimensions.size], &_1_);
    EXPECT_FALSE(list.empty());

    EXPECT_EQ(&item1, &list[kb.sequence.first]);
    EXPECT_EQ(&item1, &list[kb.sequence.last]);
    EXPECT_FALSE(item1.has(kb.sequence.previous));
    EXPECT_FALSE(item1.has(kb.sequence.next));
    EXPECT_EQ(&item1[kb.id.value], &_1_);
    printAs.value(list);

    auto& item2 = *list.add(_2_);
    EXPECT_TRUE(list.has(kb.sequence.first));
    EXPECT_TRUE(list.has(kb.sequence.last));
    EXPECT_EQ(&list[kb.dimensions.size], &_2_);
    EXPECT_FALSE(list.empty());

    EXPECT_EQ(&item1, &list[kb.sequence.first]);
    EXPECT_EQ(&item2, &list[kb.sequence.last]);
    EXPECT_FALSE(item1.has(kb.sequence.previous));
    EXPECT_TRUE(item1.has(kb.sequence.next));
    EXPECT_TRUE(item2.has(kb.sequence.previous));
    EXPECT_FALSE(item2.has(kb.sequence.next));
    EXPECT_EQ(&item1[kb.id.value], &_1_);
    EXPECT_EQ(&item2[kb.id.value], &_2_);
    printAs.value(list);

    auto& item3 = *list.add(_3_);
    EXPECT_TRUE(list.has(kb.sequence.first));
    EXPECT_TRUE(list.has(kb.sequence.last));
    EXPECT_EQ(&list[kb.dimensions.size], &_3_);
    EXPECT_FALSE(list.empty());
    printAs.value(list);

    list.removeItem(&item2);
    EXPECT_TRUE(list.has(kb.sequence.first));
    EXPECT_TRUE(list.has(kb.sequence.last));
    EXPECT_EQ(&list[kb.dimensions.size], &_2_);
    EXPECT_FALSE(list.empty());
    printAs.value(list);

    list.removeItem(&item1);
    EXPECT_TRUE(list.has(kb.sequence.first));
    EXPECT_TRUE(list.has(kb.sequence.last));
    EXPECT_EQ(&list[kb.dimensions.size], &_1_);
    EXPECT_FALSE(list.empty());
    printAs.value(list);

    list.removeItem(&item3);
    EXPECT_FALSE(list.has(kb.sequence.first));
    EXPECT_FALSE(list.has(kb.sequence.last));
    EXPECT_EQ(&list[kb.dimensions.size], &_0_);
    EXPECT_TRUE(list.empty());
    printAs.value(list);
}

TEST_F(CellTest, NextgenMap)
{
    nextgen::Map map(kb, kb.type.Number, kb.type.Color);
    printAs.value(map.type());
    printAs.value(map[kb.id.list].type());
    printAs.cell(map[kb.id.index], "map[kb.id.index]");
    printAs.value(map[kb.id.index], "map[kb.id.index]");
    printAs.value(map[kb.id.index].type(), "map[kb.id.index].type()");
    map.add(_1_, kb.colors.red);
    printAs.cell(map[kb.id.index], "map[kb.id.index]");
    printAs.value(map[kb.id.index], "map[kb.id.index]");
    printAs.value(map[kb.id.index].type(), "map[kb.id.index].type()");
    printAs.cell(map[kb.id.index].type()[kb.id.slots], "map[kb.id.index].type()[kb.id.slots]");
    EXPECT_EQ(&map[kb.id.index][_1_], &map[kb.id.list][kb.sequence.first]);
    EXPECT_EQ(&map[kb.id.index].type()[kb.id.slots][kb.dimensions.size], &_1_);
    EXPECT_EQ(&map[kb.id.index].type()[kb.id.slots][kb.id.keyType], &kb.type.Cell);
    EXPECT_EQ(&map[kb.id.index].type()[kb.id.slots][kb.id.objectType], &kb.type.Slot);
    EXPECT_EQ(&map[kb.id.index].type()[kb.id.slots][kb.id.listType], &kb.type.ListOf(kb.type.Slot));
    EXPECT_EQ(&map[kb.id.index].type()[kb.id.slots][kb.id.list][kb.dimensions.size], &_1_);
    EXPECT_EQ(&map[kb.id.index].type()[kb.id.slots][kb.id.index][_1_][kb.id.value][kb.id.slotRole], &_1_);
    EXPECT_EQ(&map[kb.id.index].type()[kb.id.slots][kb.id.index][_1_][kb.id.value][kb.id.slotType], &kb.type.Slot);

    EXPECT_TRUE(map[kb.id.index].type().has(kb.id.slots));
    EXPECT_TRUE(map[kb.id.index].type()[kb.id.slots].has(kb.id.index));
    EXPECT_TRUE(map[kb.id.index].type()[kb.id.slots][kb.id.index].has(kb.id.type));
    EXPECT_EQ(&map[kb.id.index].type()[kb.id.slots][kb.id.index].type(), &map[kb.id.index].type()[kb.id.slots][kb.id.index].type()[kb.id.slots][kb.id.index].type());

    EXPECT_TRUE(map[kb.id.index].type()[kb.id.slots][kb.id.index].type().has(kb.id.slots));
    printAs.cell(map[kb.id.index].type()[kb.id.slots], "map[kb.id.index].type()[kb.id.slots][]");
//    printAs.cell(map[kb.id.index].type()[kb.id.slots][kb.id.index], "map[kb.id.index].type()[kb.id.slots][kb.id.index]");
    printAs.value(map[kb.id.index].type()[kb.id.slots][kb.id.index].type(), "map[kb.id.index].type()[kb.id.slots][kb.id.index].type()");
//    printAs.value(map[kb.id.index].type()[kb.id.slots][kb.id.index].type()[kb.id.slots], "map[kb.id.index].type()[kb.id.slots][kb.id.index].type()[kb.id.slots]");
    EXPECT_EQ(&map[kb.id.index].type()[kb.id.slots][kb.id.index].type()[kb.id.slots][kb.dimensions.size], &_1_);
    EXPECT_EQ(&map[kb.id.index].type()[kb.id.slots][kb.id.index].type()[kb.id.slots][kb.id.index][_1_][kb.id.value][kb.id.slotRole], &_1_);
    CellI& debug = map[kb.id.index].type()[kb.id.slots][kb.id.index].type()[kb.id.slots][kb.id.index][_1_][kb.id.value][kb.id.slotType];
    EXPECT_EQ(&map[kb.id.index].type()[kb.id.slots][kb.id.index].type()[kb.id.slots][kb.id.index][_1_][kb.id.value][kb.id.slotType], &kb.type.Slot);
    printAs.value(map[kb.id.index].type()[kb.id.slots][kb.id.index].type()[kb.id.slots][kb.id.index], "map[kb.id.index].type()[kb.id.slots][kb.id.index].type()[kb.id.slots][kb.id.index]");
    printAs.value(map[kb.id.index].type()[kb.id.slots][kb.id.index].type()[kb.id.slots][kb.id.index].type(), "map[kb.id.index].type()[kb.id.slots][kb.id.index].type()[kb.id.slots][kb.id.index].type()");
}

TEST_F(CellTest, NextgenType)
{
    nextgen::Map map(kb, kb.type.Number, kb.type.Color);
    map.add(_1_, kb.colors.blue);
    CellI& list = map[kb.id.index][kb.id.type][kb.id.slots][kb.id.list];
    printAs.value(list, "Map<Number, Color>::index::type::slots::list");
    printAs.value(map, "Map<Number, Color>");

    nextgen::Index index(kb);
    index.set(kb.type.Number, kb.type.Color);
    printAs.value(index, "Index");

    nextgen::Type type(kb, "type");
    type.addSlot(_1_, kb.type.slot(_1_, kb.type.Number));
    printAs.value(type);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    std::cout << "Constructed: " << CellI::s_constructed << ", destructed: " << CellI::s_destructed << ", live: " << CellI::s_constructed - CellI::s_destructed << std::endl;

    return ret;
}