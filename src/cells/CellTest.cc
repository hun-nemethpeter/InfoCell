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
    const auto printOp = [this](CellI& type, CellI& method) { printAs.value(type[id.methods][id.index][method]); };

    printOp(kb.type.Map, id.constructor);
    printOp(kb.type.Map, id.template_);
    printOp(kb.type.Map, id.size);
    printOp(kb.type.Map, id.add);
    printOp(kb.type.Map, id.empty);

#if 1
    printOp(kb.type.Type_, id.constructor);
    printOp(kb.type.Type_, id.addSlot);
    printOp(kb.type.Type_, id.addSubType);
    printOp(kb.type.Type_, id.addMembership);
    printOp(kb.type.Type_, id.addSlots);

    printOp(kb.type.ListItem, id.template_);
    printOp(kb.type.ListItem, id.constructor);

    printOp(kb.type.List, id.template_);
    printOp(kb.type.List, id.constructor);
    printOp(kb.type.List, id.add);
    printOp(kb.type.List, id.size);
    printOp(kb.type.List, id.empty);

    printOp(kb.type.Number, kb.test.factorial);
    Object testNumber(kb, kb.type.Number, "testNumber");
    EXPECT_EQ(&testNumber.method(kb.test.factorial, { id.input, _0_ }), &_1_);
    EXPECT_EQ(&testNumber.method(kb.test.factorial, { id.input, _1_ }), &_1_);
    EXPECT_EQ(&testNumber.method(kb.test.factorial, { id.input, _2_ }), &_2_);
    EXPECT_EQ(&testNumber.method(kb.test.factorial, { id.input, _3_ }), &_6_);
    EXPECT_EQ(&testNumber.method(kb.test.factorial, { id.input, _4_ }), &kb.pools.numbers.get(24));
    EXPECT_EQ(&testNumber.method(kb.test.factorial, { id.input, _5_ }), &kb.pools.numbers.get(120));
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
    Object list(kb, kb.type.List, id.constructor);

    printAs.value(list);
    printAs.cell(list);
    EXPECT_EQ(&list[id.size], &_0_);
    EXPECT_EQ(&list.method(id.size), &_0_);
    EXPECT_EQ(&list.method(id.empty), &true_);
    EXPECT_EQ(&list[id.objectType], &kb.type.Cell);
    EXPECT_FALSE(list.has(id.first));
    EXPECT_FALSE(list.has(id.last));

    list.method(id.add, { id.value, _1_ });
    EXPECT_EQ(&list[id.size], &_1_);
    EXPECT_EQ(&list.method(id.size), &_1_);
    EXPECT_EQ(&list.method(id.empty), &false_);
    EXPECT_TRUE(list.has(id.first));
    EXPECT_TRUE(list.has(id.last));

    CellI& firstItem = list[id.first];
    EXPECT_EQ(&firstItem, &list[id.last]);
    EXPECT_FALSE(firstItem.has(id.previous));
    EXPECT_FALSE(firstItem.has(id.next));
    EXPECT_EQ(&firstItem[id.value], &_1_);
    printAs.value(list);

    list.method(id.add, { id.value, kb.pools.numbers.get(2) });
    EXPECT_EQ(&list[id.size], &_2_);
    EXPECT_EQ(&list.method(id.size), &_2_);
    EXPECT_EQ(&list.method(id.empty), &false_);

    CellI& secondItem = list[id.last];
    EXPECT_EQ(&firstItem, &list[id.first]);
    EXPECT_NE(&firstItem, &list[id.last]);

    EXPECT_EQ(firstItem.has(id.previous), false);
    EXPECT_EQ(&firstItem[id.next], &secondItem);
    EXPECT_EQ(&firstItem[id.value], &_1_);

    EXPECT_EQ(&secondItem[id.previous], &firstItem);
    EXPECT_EQ(secondItem.has(id.next), false);
    EXPECT_EQ(&secondItem[id.value], &_2_);
    printAs.value(list);

    list.method(id.add, { id.value, _3_ });
    EXPECT_EQ(&list[id.size], &_3_);
    EXPECT_EQ(&list.method(id.size), &_3_);
    EXPECT_EQ(&list.method(id.empty), &false_);

    CellI& thirdItem = list[id.last];
    EXPECT_EQ(&firstItem, &list[id.first]);
    EXPECT_NE(&firstItem, &list[id.last]);

    EXPECT_EQ(firstItem.has(id.previous), false);
    EXPECT_EQ(&firstItem[id.next], &secondItem);
    EXPECT_EQ(&firstItem[id.value], &_1_);

    EXPECT_EQ(&secondItem[id.previous], &firstItem);
    EXPECT_EQ(&secondItem[id.next], &thirdItem);
    EXPECT_EQ(&secondItem[id.value], &_2_);

    EXPECT_EQ(&thirdItem[id.previous], &secondItem);
    EXPECT_EQ(thirdItem.has(id.next), false);
    EXPECT_EQ(&thirdItem[id.value], &_3_);
    printAs.value(list);

    list.method(id.remove, { id.item, thirdItem });
    EXPECT_EQ(&list[id.size], &_2_);
    EXPECT_EQ(&list.method(id.size), &_2_);
    EXPECT_EQ(&list.method(id.empty), &false_);
    {
        CellI& secondItem = list[id.last];
        EXPECT_EQ(&firstItem, &list[id.first]);
        EXPECT_NE(&firstItem, &list[id.last]);

        EXPECT_EQ(firstItem.has(id.previous), false);
        EXPECT_EQ(&firstItem[id.next], &secondItem);
        EXPECT_EQ(&firstItem[id.value], &_1_);

        EXPECT_EQ(&secondItem[id.previous], &firstItem);
        EXPECT_EQ(secondItem.has(id.next), false);
        EXPECT_EQ(&secondItem[id.value], &_2_);
    }
    printAs.value(list);

    list.method(id.remove, { id.item, secondItem });
    EXPECT_EQ(&list[id.size], &_1_);
    EXPECT_EQ(&list.method(id.size), &_1_);
    EXPECT_EQ(&list.method(id.empty), &false_);
    {
        CellI& firstItem = list[id.first];
        EXPECT_EQ(&firstItem, &list[id.last]);
        EXPECT_EQ(firstItem.has(id.previous), false);
        EXPECT_EQ(firstItem.has(id.next), false);
        EXPECT_EQ(&firstItem[id.value], &_1_);
    }
    printAs.value(list);

    list.method(id.remove, { id.item, firstItem });
    EXPECT_EQ(&list[id.size], &_0_);
    EXPECT_EQ(&list.method(id.size), &_0_);
    EXPECT_EQ(&list.method(id.empty), &true_);
    EXPECT_EQ(&list[id.objectType], &kb.type.Cell);
    EXPECT_FALSE(list.has(id.first));
    EXPECT_FALSE(list.has(id.last));
    printAs.value(list);

    CellI& size = list.method(id.size);
    printAs.value(size);
}

TEST_F(CellTest, ListOfNumber)
{
    CellI& ListOfNumber = kb.type.List.smethod(id.template_, { id.objectType, kb.type.Number });
    Object list(kb, ListOfNumber, id.constructor);

    printAs.value(list);
    printAs.cell(list);
    EXPECT_EQ(&list[id.size], &_0_);
    EXPECT_EQ(&list.method(id.size), &_0_);
    EXPECT_EQ(&list.method(id.empty), &true_);
    EXPECT_EQ(&list[id.objectType], &kb.type.Number);

    list.method(id.add, { id.value, _1_ });
    EXPECT_EQ(&list[id.size], &_1_);
    EXPECT_EQ(&list.method(id.size), &_1_);
    EXPECT_EQ(&list.method(id.empty), &false_);

    CellI& firstItem = list[id.first];
    EXPECT_EQ(&firstItem, &list[id.last]);
    EXPECT_EQ(firstItem.has(id.previous), false);
    EXPECT_EQ(firstItem.has(id.next), false);
    EXPECT_EQ(&firstItem[id.value], &_1_);
    printAs.value(list);

    list.method(id.add, { id.value, kb.pools.numbers.get(2) });
    EXPECT_EQ(&list[id.size], &_2_);
    EXPECT_EQ(&list.method(id.size), &_2_);
    EXPECT_EQ(&list.method(id.empty), &false_);

    CellI& secondItem = list[id.last];
    EXPECT_EQ(&firstItem, &list[id.first]);
    EXPECT_NE(&firstItem, &list[id.last]);

    EXPECT_EQ(firstItem.has(id.previous), false);
    EXPECT_EQ(&firstItem[id.next], &secondItem);
    EXPECT_EQ(&firstItem[id.value], &_1_);

    EXPECT_EQ(&secondItem[id.previous], &firstItem);
    EXPECT_EQ(secondItem.has(id.next), false);
    EXPECT_EQ(&secondItem[id.value], &_2_);

    printAs.value(list);
    list.method(id.add, { id.value, _3_ });
    EXPECT_EQ(&list[id.size], &_3_);
    EXPECT_EQ(&list.method(id.size), &_3_);
    EXPECT_EQ(&list.method(id.empty), &false_);

    CellI& thirdItem = list[id.last];
    EXPECT_EQ(&firstItem, &list[id.first]);
    EXPECT_NE(&firstItem, &list[id.last]);

    EXPECT_EQ(firstItem.has(id.previous), false);
    EXPECT_EQ(&firstItem[id.next], &secondItem);
    EXPECT_EQ(&firstItem[id.value], &_1_);

    EXPECT_EQ(&secondItem[id.previous], &firstItem);
    EXPECT_EQ(&secondItem[id.next], &thirdItem);
    EXPECT_EQ(&secondItem[id.value], &_2_);

    EXPECT_EQ(&thirdItem[id.previous], &secondItem);
    EXPECT_EQ(thirdItem.has(id.next), false);
    EXPECT_EQ(&thirdItem[id.value], &_3_);

    printAs.value(list);
    CellI& size = list.method(id.size);
    printAs.value(size);
}

TEST_F(CellTest, Map)
{
    Object map(kb, kb.type.Map, id.constructor);

    printAs.value(map);
    printAs.cell(map);
    EXPECT_EQ(&map[id.size], &_0_);
    EXPECT_EQ(&map.method(id.size), &_0_);
    EXPECT_EQ(&map.method(id.empty), &true_);
    EXPECT_EQ(&map[id.keyType], &kb.type.Cell);
    EXPECT_EQ(&map[id.objectType], &kb.type.Cell);

    map.method(id.add, { id.key, _1_ }, { id.value, kb.colors.red });
    printAs.value(map);
    printAs.cell(map);
    EXPECT_EQ(&map[id.size], &_1_);
    EXPECT_EQ(&map.method(id.size), &_1_);
    EXPECT_EQ(&map[id.list][id.size], &_1_);
    EXPECT_EQ(&map[id.list][id.objectType], &kb.type.Cell);
    EXPECT_EQ(&map[id.list][id.first][id.value], &kb.colors.red);
    EXPECT_EQ(&map[id.list][id.first], &map[id.list][id.last]);
    EXPECT_TRUE(map[id.index].has(_1_));
    EXPECT_EQ(&map[id.index][_1_], &kb.colors.red);
    EXPECT_EQ(&map.method(id.empty), &false_);
    EXPECT_TRUE(map[id.index][id.type][id.memberOf][id.index].has(kb.type.Index));

    map.method(id.add, { id.key, _2_ }, { id.value, kb.colors.green });
    map.method(id.add, { id.key, _3_ }, { id.value, kb.colors.blue });
    EXPECT_EQ(&map[id.index][_1_], &kb.colors.red);
    EXPECT_EQ(&map[id.index][_2_], &kb.colors.green);
    EXPECT_EQ(&map[id.index][_3_], &kb.colors.blue);
    printAs.value(map);
    printAs.cell(map);
}

TEST_F(CellTest, MapTypes)
{
    Object map(kb, kb.type.Map, id.constructor);
    printAs.value(map.type());
    printAs.value(map[id.list].type());
    printAs.value(map[id.index], "map[id.index]");
    printAs.value(map[id.index].type(), "map[id.index].type()");
    map.method(id.add, { id.key, _1_ }, { id.value, kb.colors.red });
    printAs.value(map[id.index], "map[id.index]");
    printAs.value(map[id.index].type(), "map[id.index].type()");
    printAs.cell(map[id.index].type()[id.slots], "map[id.index].type()[id.slots]");
    printAs.cell(map[id.index].type()[id.slots][id.index], "map[id.index].type()[id.slots][id.index]");
    printAs.value(map[id.index].type()[id.slots][id.index], "map[id.index].type()[id.slots][id.index]");
}

TEST_F(CellTest, BuiltInMapTypes)
{
    Map& map = *new cells::Map(kb, kb.type.Number, kb.type.Color);
    printAs.value(map.type());
    printAs.value(map[id.list].type());
    printAs.cell(map[id.index], "map[id.index]");
    printAs.value(map[id.index], "map[id.index]");
    printAs.value(map[id.index].type(), "map[id.index].type()");
    map.add(_1_, kb.colors.red);
    printAs.cell(map[id.index], "map[id.index]");
    printAs.value(map[id.index], "map[id.index]");
    printAs.value(map[id.index].type(), "map[id.index].type()");
    printAs.cell(map[id.index].type()[id.slots], "map[id.index].type()[id.slots]");
    EXPECT_EQ(&map[id.index][_1_], &kb.colors.red);
    EXPECT_EQ(&map[id.index].type()[id.slots][id.size], &_1_);
    EXPECT_EQ(&map[id.index].type()[id.slots][id.keyType], &kb.type.Cell);
    EXPECT_EQ(&map[id.index].type()[id.slots][id.objectType], &kb.type.Slot);
    EXPECT_EQ(&map[id.index].type()[id.slots][id.listType], &kb.type.ListOf(kb.type.Slot));
    EXPECT_EQ(&map[id.index].type()[id.slots][id.list][id.size], &_1_);
    EXPECT_EQ(&map[id.index].type()[id.slots][id.index][_1_][id.slotRole], &_1_);
    EXPECT_EQ(&map[id.index].type()[id.slots][id.index][_1_][id.slotType], &kb.type.Color);

    EXPECT_TRUE(map[id.index].type().has(id.slots));
    EXPECT_TRUE(map[id.index].type()[id.slots].has(id.index));
    EXPECT_TRUE(map[id.index].type()[id.slots][id.index].has(id.type));
    EXPECT_EQ(&map[id.index].type()[id.slots][id.index].type(), &map[id.index].type()[id.slots][id.index].type()[id.slots][id.index].type());

    EXPECT_TRUE(map[id.index].type()[id.slots][id.index].type().has(id.slots));
    printAs.cell(map[id.index].type()[id.slots], "map[id.index].type()[id.slots][]");
    printAs.cell(map[id.index].type()[id.slots][id.index], "map[id.index].type()[id.slots][id.index]");
    printAs.value(map[id.index].type()[id.slots][id.index].type(), "map[id.index].type()[id.slots][id.index].type()");
    printAs.value(map[id.index].type()[id.slots][id.index].type()[id.slots], "map[id.index].type()[id.slots][id.index].type()[id.slots]");
    EXPECT_EQ(&map[id.index].type()[id.slots][id.index].type()[id.slots][id.size], &_1_);
    EXPECT_EQ(&map[id.index].type()[id.slots][id.index].type()[id.slots][id.index][_1_][id.slotRole], &_1_);
    EXPECT_EQ(&map[id.index].type()[id.slots][id.index].type()[id.slots][id.index][_1_][id.slotType], &kb.type.Color);
    printAs.value(map[id.index].type()[id.slots][id.index].type()[id.slots][id.index], "map[id.index].type()[id.slots][id.index].type()[id.slots][id.index]");
    printAs.value(map[id.index].type()[id.slots][id.index].type()[id.slots][id.index].type(), "map[id.index].type()[id.slots][id.index].type()[id.slots][id.index].type()");
}


TEST_F(CellTest, MapTemplateTypes)
{
    CellI& MapNumberToColor = kb.type.Map.smethod(id.template_, { id.keyType, kb.type.Number }, { id.objectType, kb.type.Color });
    Object map(kb, MapNumberToColor, id.constructor);
    printAs.value(map.type());
    printAs.value(map[id.list].type());
    printAs.cell(map[id.index], "map[id.index]");
    printAs.value(map[id.index], "map[id.index]");
    printAs.value(map[id.index].type(), "map[id.index].type()");
    map.method(id.add, { id.key, _1_ }, { id.value, kb.colors.red });
    printAs.cell(map[id.index], "map[id.index]");
    printAs.value(map[id.index], "map[id.index]");
    printAs.value(map[id.index].type(), "map[id.index].type()");
    printAs.cell(map[id.index].type()[id.slots], "map[id.index].type()[id.slots]");
    EXPECT_EQ(&map[id.index].type()[id.slots][id.size], &_1_);
    EXPECT_EQ(&map[id.index].type()[id.slots][id.keyType], &kb.type.Cell);
    EXPECT_EQ(&map[id.index].type()[id.slots][id.objectType], &kb.type.Slot);
    EXPECT_EQ(&map[id.index].type()[id.slots][id.listType], &kb.type.ListOf(kb.type.Slot));
    EXPECT_EQ(&map[id.index].type()[id.slots][id.list][id.size], &_1_);
    EXPECT_EQ(&map[id.index].type()[id.slots][id.index][_1_][id.slotRole], &_1_);
    EXPECT_EQ(&map[id.index].type()[id.slots][id.index][_1_][id.slotType], &kb.type.Color);

    EXPECT_TRUE(map[id.index].type().has(id.slots));
    EXPECT_TRUE(map[id.index].type()[id.slots].has(id.index));
    EXPECT_TRUE(map[id.index].type()[id.slots][id.index].has(id.type));
    EXPECT_EQ(&map[id.index].type()[id.slots][id.index].type(), &map[id.index].type()[id.slots][id.index].type()[id.slots][id.index].type());

    EXPECT_TRUE(map[id.index].type()[id.slots][id.index].type().has(id.slots));
    printAs.cell(map[id.index].type()[id.slots], "map[id.index].type()[id.slots][]");
    printAs.cell(map[id.index].type()[id.slots][id.index], "map[id.index].type()[id.slots][id.index]");
    printAs.value(map[id.index].type()[id.slots][id.index].type(), "map[id.index].type()[id.slots][id.index].type()");
    printAs.value(map[id.index].type()[id.slots][id.index].type()[id.slots], "map[id.index].type()[id.slots][id.index].type()[id.slots]");
    EXPECT_EQ(&map[id.index].type()[id.slots][id.index].type()[id.slots][id.size], &_1_);
    EXPECT_EQ(&map[id.index].type()[id.slots][id.index].type()[id.slots][id.index][_1_][id.slotRole], &_1_);
    EXPECT_EQ(&map[id.index].type()[id.slots][id.index].type()[id.slots][id.index][_1_][id.slotType], &kb.type.Slot);
    printAs.value(map[id.index].type()[id.slots][id.index].type()[id.slots][id.index], "map[id.index].type()[id.slots][id.index].type()[id.slots][id.index]");
    printAs.value(map[id.index].type()[id.slots][id.index].type()[id.slots][id.index].type(), "map[id.index].type()[id.slots][id.index].type()[id.slots][id.index].type()");
}

TEST_F(CellTest, MapNumberToColor)
{
    CellI& MapNumberToColor = kb.type.Map.smethod(id.template_, { id.keyType, kb.type.Number }, { id.objectType, kb.type.Color });
    Object map(kb, MapNumberToColor, id.constructor);

    printAs.value(map);
    printAs.cell(map);
    EXPECT_EQ(&map[id.size], &_0_);
    EXPECT_EQ(&map.method(id.size), &_0_);
    EXPECT_EQ(&map.method(id.empty), &true_);
    EXPECT_EQ(&map[id.keyType], &kb.type.Number);
    EXPECT_EQ(&map[id.objectType], &kb.type.Color);

    map.method(id.add, { id.key, _1_}, { id.value, kb.colors.red });
    printAs.value(map);
    printAs.cell(map);
    EXPECT_EQ(&map[id.size], &_1_);
    EXPECT_EQ(&map.method(id.size), &_1_);
    EXPECT_EQ(&map[id.list][id.size], &_1_);
    EXPECT_EQ(&map[id.list][id.objectType], &kb.type.Color);
    EXPECT_EQ(&map[id.list][id.first][id.value], &kb.colors.red);
    EXPECT_EQ(&map[id.list][id.first], &map[id.list][id.last]);
    EXPECT_TRUE(map[id.index].has(_1_));
    EXPECT_EQ(&map[id.index][_1_], &kb.colors.red);
    EXPECT_EQ(&map.method(id.empty), &false_);
    EXPECT_TRUE(map[id.index][id.type][id.memberOf][id.index].has(kb.type.Index));

    map.method(id.add, { id.key, _2_ }, { id.value, kb.colors.green });
    map.method(id.add, { id.key, _3_ }, { id.value, kb.colors.blue });
    EXPECT_EQ(&map[id.index][_1_], &kb.colors.red);
    EXPECT_EQ(&map[id.index][_2_], &kb.colors.green);
    EXPECT_EQ(&map[id.index][_3_], &kb.colors.blue);
    printAs.value(map);
    printAs.cell(map);
}

TEST_F(CellTest, ListItem)
{
    Object listItem(kb, kb.type.ListItem, id.constructor, { id.value, kb.colors.green });

    EXPECT_EQ(&listItem[id.value], &kb.colors.green);

    EXPECT_EQ(&listItem.type()[id.memberOf][id.size], &_1_);
    EXPECT_TRUE(listItem.type()[id.memberOf][id.index].has(kb.type.Iterator));

    EXPECT_EQ(&listItem.type()[id.slots][id.size], &_3_);
    EXPECT_TRUE(listItem.type()[id.slots][id.index].has(id.previous));
    EXPECT_TRUE(listItem.type()[id.slots][id.index].has(id.next));
    EXPECT_TRUE(listItem.type()[id.slots][id.index].has(id.value));

    EXPECT_TRUE(listItem.type()[id.methods][id.index].has(id.template_));
    EXPECT_TRUE(listItem.type()[id.methods][id.index].has(id.constructor));
}

TEST_F(CellTest, ListItemTemplate)
{
    CellI& ListItemNumber = kb.type.ListItem.smethod(id.template_, { id.objectType, kb.type.Number });
    Object listItemNumber(kb, ListItemNumber, id.constructor, { id.value, _1_ });

    EXPECT_EQ(&listItemNumber[id.value], &_1_);

    printAs.value(kb.type.ListItem, "type.ListItem");
    printAs.value(ListItemNumber, "listItemNumber");
    printAs.value(kb.type.ListItem[id.slots][id.list], "type.ListItem[slots]");
    printAs.value(ListItemNumber[id.slots][id.list], "listItemNumber[slots]");

    EXPECT_EQ(&ListItemNumber[id.memberOf][id.size], &_1_);
    EXPECT_TRUE(ListItemNumber[id.memberOf][id.index].has(kb.type.ListItem));

    EXPECT_EQ(&ListItemNumber[id.slots][id.size], &_3_);
    EXPECT_TRUE(ListItemNumber[id.slots][id.index].has(id.previous));
    EXPECT_TRUE(ListItemNumber[id.slots][id.index].has(id.next));
    EXPECT_TRUE(ListItemNumber[id.slots][id.index].has(id.value));

    EXPECT_EQ(&ListItemNumber[id.methods], &kb.type.ListItem[id.methods]);
}

TEST_F(CellTest, ListTemplate)
{
    CellI& ListOfNumbers = kb.type.List.smethod(id.template_, { id.objectType, kb.type.Number });

    EXPECT_EQ(&ListOfNumbers[id.subTypes][id.size], &_2_);
    EXPECT_TRUE(ListOfNumbers[id.subTypes][id.index].has(id.objectType));
    EXPECT_EQ(&ListOfNumbers[id.subTypes][id.index][id.objectType], &kb.type.Number);
    EXPECT_TRUE(ListOfNumbers[id.subTypes][id.index].has(id.itemType));
    CellI& ListItemType = ListOfNumbers[id.subTypes][id.index][id.itemType];
    EXPECT_EQ(&ListItemType[id.slots][id.index][id.value][id.slotType], &kb.type.Number);
    EXPECT_NE(&ListItemType, &kb.type.ListItem);
    EXPECT_EQ(&ListItemType[id.memberOf][id.size], &_1_);
    EXPECT_TRUE(ListItemType[id.memberOf][id.index].has(kb.type.ListItem));

    EXPECT_EQ(&ListItemType[id.slots][id.size], &_3_);
    EXPECT_TRUE(ListItemType[id.slots][id.index].has(id.previous));
    EXPECT_TRUE(ListItemType[id.slots][id.index].has(id.next));
    EXPECT_TRUE(ListItemType[id.slots][id.index].has(id.value));

    EXPECT_EQ(&ListItemType[id.methods], &kb.type.ListItem[id.methods]);

    EXPECT_EQ(&ListOfNumbers[id.methods], &kb.type.List[id.methods]);

    EXPECT_EQ(&ListOfNumbers[id.slots][id.size], &_5_);
    EXPECT_TRUE(ListOfNumbers[id.slots][id.index].has(id.first));
    EXPECT_EQ(&ListOfNumbers[id.slots][id.index][id.first][id.slotType], &ListItemType);

    EXPECT_TRUE(ListOfNumbers[id.slots][id.index].has(id.last));
    EXPECT_EQ(&ListOfNumbers[id.slots][id.index][id.last][id.slotType], &ListItemType);

    EXPECT_TRUE(ListOfNumbers[id.slots][id.index].has(id.size));
    EXPECT_EQ(&ListOfNumbers[id.slots][id.index][id.size][id.slotType], &kb.type.Number);

    EXPECT_TRUE(ListOfNumbers[id.slots][id.index].has(id.objectType));
    EXPECT_EQ(&ListOfNumbers[id.slots][id.index][id.objectType][id.slotType], &kb.type.Number);

    EXPECT_TRUE(ListOfNumbers[id.slots][id.index].has(id.itemType));
    EXPECT_EQ(&ListOfNumbers[id.slots][id.index][id.itemType][id.slotType], &ListItemType);

    EXPECT_TRUE(ListOfNumbers[id.memberOf][id.index].has(kb.type.List));
    EXPECT_EQ(&ListOfNumbers[id.memberOf][id.size], &_1_);

    printAs.value(kb.type.List, "type.List");
    printAs.value(ListOfNumbers, "ListOfNumbers");

    Object listOfNumbers(kb, ListOfNumbers, id.constructor);
    CellI& listItemNumber = listOfNumbers[id.itemType];
    EXPECT_EQ(&ListItemType, &listItemNumber);
    EXPECT_EQ(&ListOfNumbers[id.subTypes][id.index][id.itemType], &listItemNumber);
}

TEST_F(CellTest, FunctionTypes)
{
    CellI& function = kb.type.Type_[id.methods][id.index][id.addSlot];
    printAs.cell(function.type(), "function");
    printAs.cell(function.type()[id.subTypes][id.index][id.localVars], "function[id.subTypes][id.localVars]");
    printAs.value(function.type()[id.subTypes][id.index][id.localVars], "function[id.subTypes][id.localVars]");
}

TEST_F(CellTest, CreatedTypeWithConstructor)
{
    Object emptySubTypesMap(kb, kb.type.MapCellToType, id.constructor);
    Object emptyMethodsMap(kb, kb.type.MapCellToType, id.constructor);
    Object slotsMap(kb, kb.type.MapCellToSlot, id.constructor);
    slotsMap.method(id.add, { id.key, id.size }, { id.value, kb.type.slot(id.size, kb.type.Number) });

    Object newType(kb, kb.type.Type_, id.constructor,
                   { id.slots, slotsMap },
                   { id.subTypes, emptySubTypesMap },
                   { id.memberOf, kb.map(kb.type.Cell, kb.type.Cell) },
                   { id.methods, emptyMethodsMap });
    printAs.value(newType[id.slots]);
    printAs.value(newType[id.slots][id.list], "newType[id.slots][id.list]");
    printAs.value(newType[id.memberOf][id.list], "newType[id.memberOf][id.list]");
    Object newObject(kb, newType);
    printAs.cell(newObject, "Empty newObject");
    EXPECT_ANY_THROW(newObject.set(id.height, _1_));
    EXPECT_NO_THROW(newObject.set(id.size, _1_));
    printAs.cell(newObject, "newObject.size = 1");
}

TEST_F(CellTest, HybridPicture)
{
    input::Picture inputPicture("input");
    inputPicture.loadFromJsonArray("[[0, 7, 0], [7, 7, 7], [0, 7, 0]]");
    hybrid::Picture picture(kb, inputPicture);

    printAs.svg(picture);
    printAs.svg(picture[id.pixels]);
    printAs.value(picture[id.pixels]);

    EXPECT_EQ(&picture[id.type], &kb.type.Picture);
    EXPECT_EQ(&picture[id.width], &kb.pools.numbers.get(3));
    EXPECT_EQ(&picture[id.height], &kb.pools.numbers.get(3));
    EXPECT_EQ(&picture[id.pixels][id.type], &kb.type.ListOf(kb.type.Pixel));

    CellI& ListOfPixels = kb.type.List.smethod(id.template_, { id.objectType, kb.type.Pixel });
    Object listOfPixels(kb, ListOfPixels, id.constructor, "listOfPixels");
    listOfPixels.method(id.add, { id.value, picture[id.pixels][id.first][id.value] });
    listOfPixels.method(id.add, { id.value, picture[id.pixels][id.first][id.next][id.value] });
    printAs.value(listOfPixels);
}

TEST_F(CellTest, BasicObjectTest)
{
    Object testType(kb, kb.type.Type_, "Test");
    Object emptyList(kb, kb.type.List, id.constructor);

    testType.method(id.addSlots, { id.list, emptyList });

    testType.method(id.addSlots, { id.list, kb.list(
        kb.type.slot(id.result, kb.type.Digit),
        kb.type.slot(id.value, kb.type.Number))}); // TODO implement type checking

    Object object(kb, testType, "testObject");

    EXPECT_EQ(object.label(), "testObject");
    EXPECT_EQ(&object.type(), &testType);

    EXPECT_ANY_THROW(&object[id.value]);
    EXPECT_NO_THROW(object.set(id.value, kb.pools.numbers.get(42)));
    printAs.value(object[id.value]);
    EXPECT_ANY_THROW(object.set(id.argument, kb.pools.numbers.get(42)));
}

TEST_F(CellTest, BasicControlOpTest)
{
    Object testValue1(kb, kb.type.op.ConstVar);
    testValue1.set(id.value, kb.type.Char);

    Object testValue2(kb, kb.type.op.ConstVar);
    testValue2.set(id.value, kb.type.Color);

    Object sameOpEq(kb, kb.type.op.Same, "sameOpEq");
    sameOpEq.set(id.lhs, testValue1);
    sameOpEq.set(id.rhs, testValue1);

    Object sameOpNe(kb, kb.type.op.Same, "sameOpNe");
    sameOpNe.set(id.lhs, testValue1);
    sameOpNe.set(id.rhs, testValue2);

    sameOpEq();
    sameOpNe();

    printAs.value(sameOpEq[id.value], "testValue1 == testValue1");
    printAs.value(sameOpNe[id.value], "testValue1 != testValue1");

    EXPECT_EQ(&sameOpEq[id.value], &true_);
    EXPECT_EQ(&sameOpNe[id.value], &false_);
}

TEST_F(CellTest, BasicControlAddTest)
{
    Object start(kb, kb.type.op.ConstVar);
    start.set(id.value, kb.pools.numbers.get(42));

    Object value10(kb, kb.type.op.ConstVar);
    value10.set(id.value, kb.pools.numbers.get(10));

    Object add10(kb, kb.type.op.Add, "add10");
    add10.set(id.lhs, start);
    add10.set(id.rhs, value10);
    add10();
    std::cout << "42 + 10 = ";
    printAs.value(add10[id.value]);

    EXPECT_EQ(&add10[id.value], &kb.pools.numbers.get(52));
}

TEST_F(CellTest, CreatingCustomType)
{
    Object colorRed(kb, kb.type.Cell, "red");
    Object colorGreen(kb, kb.type.Cell, "green");
    Object colorBlue(kb, kb.type.Cell, "blue");

    Object colorClass(kb, kb.type.Type_, "Color");
#if 1 // TODO
    Object* slotMapPtr = new Object(kb, kb.type.MapCellToSlot, id.constructor);
    colorClass.set(id.slots, *slotMapPtr);
#endif

    colorClass.method(id.addSlots, { id.list, kb.list(kb.type.slot(colorRed, kb.type.Number), kb.type.slot(colorGreen, kb.type.Number), kb.type.slot(colorBlue, kb.type.Number)) });

    Object redColor(kb, colorClass, "redColor");
    redColor.set(colorRed, kb.pools.numbers.get(255));
    redColor.set(colorGreen, kb.pools.numbers.get(0));
    redColor.set(colorBlue, kb.pools.numbers.get(0));

    printAs.value(colorClass, "colorClass:");
    printAs.value(colorClass[id.slots][id.list], "colorClass::slots");
    printAs.value(colorClass[id.slots][id.index][colorRed]);
    EXPECT_TRUE(colorClass[id.slots][id.index].has(colorRed));
    EXPECT_TRUE(colorClass[id.slots][id.index].has(colorGreen));
    EXPECT_TRUE(colorClass[id.slots][id.index].has(colorBlue));
    printAs.value(colorClass[id.slots][id.index][colorRed][id.type], "colorClass slot of colorRed");
    EXPECT_EQ(&colorClass[id.slots][id.index][colorRed][id.type], &kb.type.Slot);
    EXPECT_EQ(&colorClass[id.slots][id.index][colorRed][id.type], &kb.type.Slot);
    EXPECT_EQ(&colorClass[id.slots][id.index][colorGreen][id.type], &kb.type.Slot);
    EXPECT_EQ(&colorClass[id.slots][id.index][colorBlue][id.type], &kb.type.Slot);
    printAs.value(redColor);

    printAs.cell(redColor);
    printAs.cell(colorClass[id.slots][id.index][colorRed], "colorClass[id.slots][id.index][colorRed]");
    printAs.cell(colorClass[id.slots][id.index], "colorClass[id.slots][id.index]");

    printAs.svgStruct(redColor, "redColor");
    printAs.svgStruct(colorRed, "colorRed");
    printAs.svgStruct(colorClass, "Color");
    printAs.svgStruct(colorClass[id.slots][id.index], "SlotIndex of Color");
    printAs.svgStruct(colorClass[id.slots][id.list], "SlotList of Color");
    printAs.svgStruct(colorClass[id.slots][id.list][id.first], "SlotListItem1 of Color");
    printAs.svgStruct(colorClass[id.slots][id.list][id.first][id.value], "Slot1 of Color");
    printAs.svgStruct(colorClass[id.slots][id.list][id.first][id.next], "SlotListItem2 of Color");
    printAs.svgStruct(colorClass[id.slots][id.list][id.first][id.next][id.value], "Slot2 of Color");
    printAs.value(colorClass[id.slots][id.index], "colorClass[id.slots][id.index]");
    printAs.value(colorClass[id.slots][id.index][id.type], "colorClass[id.slots][id.index][id.type]");
    printAs.value(colorClass[id.slots][id.index][id.type][id.slots], "colorClass[id.slots][id.index][id.type][id.slots]");
    printAs.value(colorClass[id.slots][id.index][id.type][id.slots][id.index], "colorClass[id.slots][id.index][id.type][id.slots][id.index]");
    printAs.cell(colorClass);

    printAs.svg(redColor);
    printAs.svg(colorClass[id.slots][id.index][colorRed]);
    printAs.svg(colorClass);
}

TEST_F(CellTest, CreatingNumber)
{
    Number& number_0   = kb.pools.numbers.get(0);
    Number& number_255 = kb.pools.numbers.get(255);

    printAs.value(number_255);
    printAs.value(number_255[id.value][id.first][id.value]);

    printAs.svgStruct(id.emptyObject, "emptyObject");

    printAs.cell(number_255);
    printAs.cell(number_255[kb.numbers.sign]);
    printAs.cell(number_255[id.value]);
    printAs.cell(number_255[id.value][id.first]);
    printAs.cell(number_255[id.value][id.first][id.value]);
    printAs.cell(number_255[id.value][id.last]);
    printAs.cell(number_255[id.value][id.size]);

    printAs.svg(number_255);
    printAs.svg(number_255[kb.numbers.sign]);
    printAs.svg(number_255[id.value]);
    printAs.svg(number_255[id.value][id.first]);
    printAs.svg(number_255[id.value][id.first][id.value]);
    printAs.svg(number_255[id.value][id.last]);
    printAs.svg(number_255[id.value][id.size]);
}

TEST_F(CellTest, NextgenList)
{
    nextgen::List list(kb, kb.type.Number);
    EXPECT_FALSE(list.has(id.first));
    EXPECT_FALSE(list.has(id.last));
    EXPECT_EQ(&list[id.size], &_0_);
    EXPECT_EQ(&list[id.objectType], &kb.type.Number);
    EXPECT_TRUE(list.empty());
    printAs.value(list);

    auto& item1 = *list.add(_1_);
    EXPECT_TRUE(list.has(id.first));
    EXPECT_TRUE(list.has(id.last));
    EXPECT_EQ(&list[id.size], &_1_);
    EXPECT_FALSE(list.empty());

    EXPECT_EQ(&item1, &list[id.first]);
    EXPECT_EQ(&item1, &list[id.last]);
    EXPECT_FALSE(item1.has(id.previous));
    EXPECT_FALSE(item1.has(id.next));
    EXPECT_EQ(&item1[id.value], &_1_);
    printAs.value(list);

    auto& item2 = *list.add(_2_);
    EXPECT_TRUE(list.has(id.first));
    EXPECT_TRUE(list.has(id.last));
    EXPECT_EQ(&list[id.size], &_2_);
    EXPECT_FALSE(list.empty());

    EXPECT_EQ(&item1, &list[id.first]);
    EXPECT_EQ(&item2, &list[id.last]);
    EXPECT_FALSE(item1.has(id.previous));
    EXPECT_TRUE(item1.has(id.next));
    EXPECT_TRUE(item2.has(id.previous));
    EXPECT_FALSE(item2.has(id.next));
    EXPECT_EQ(&item1[id.value], &_1_);
    EXPECT_EQ(&item2[id.value], &_2_);
    printAs.value(list);

    auto& item3 = *list.add(_3_);
    EXPECT_TRUE(list.has(id.first));
    EXPECT_TRUE(list.has(id.last));
    EXPECT_EQ(&list[id.size], &_3_);
    EXPECT_FALSE(list.empty());
    printAs.value(list);

    list.removeItem(&item2);
    EXPECT_TRUE(list.has(id.first));
    EXPECT_TRUE(list.has(id.last));
    EXPECT_EQ(&list[id.size], &_2_);
    EXPECT_FALSE(list.empty());
    printAs.value(list);

    list.removeItem(&item1);
    EXPECT_TRUE(list.has(id.first));
    EXPECT_TRUE(list.has(id.last));
    EXPECT_EQ(&list[id.size], &_1_);
    EXPECT_FALSE(list.empty());
    printAs.value(list);

    list.removeItem(&item3);
    EXPECT_FALSE(list.has(id.first));
    EXPECT_FALSE(list.has(id.last));
    EXPECT_EQ(&list[id.size], &_0_);
    EXPECT_TRUE(list.empty());
    printAs.value(list);
}

TEST_F(CellTest, NextgenMap)
{
    nextgen::Map map(kb, kb.type.Number, kb.type.Color);
    printAs.value(map.type());
    printAs.value(map[id.list].type());
    printAs.cell(map[id.index], "map[id.index]");
    printAs.value(map[id.index], "map[id.index]");
    printAs.value(map[id.index].type(), "map[id.index].type()");
    map.add(_1_, kb.colors.red);
    printAs.value(map, "map");
    printAs.cell(map[id.index], "map[id.index]");
    printAs.value(map[id.index], "map[id.index]");
    printAs.value(map[id.index].type(), "map[id.index].type()");
    printAs.cell(map[id.index].type()[id.slots], "map[id.index].type()[id.slots]");
    EXPECT_EQ(&map[id.index][_1_], &map[id.list][id.first]);
    EXPECT_EQ(&map[id.index].type()[id.slots][id.size], &_1_);
    EXPECT_EQ(&map[id.index].type()[id.slots][id.keyType], &kb.type.Cell);
    EXPECT_EQ(&map[id.index].type()[id.slots][id.objectType], &kb.type.Slot);
    EXPECT_EQ(&map[id.index].type()[id.slots][id.listType], &kb.type.ListOf(kb.type.Slot));
    EXPECT_EQ(&map[id.index].type()[id.slots][id.list][id.size], &_1_);
    EXPECT_EQ(&map[id.index].type()[id.slots][id.index][_1_][id.value][id.slotRole], &_1_);
    EXPECT_EQ(&map[id.index].type()[id.slots][id.index][_1_][id.value][id.slotType], &kb.type.Slot);

    EXPECT_TRUE(map[id.index].type().has(id.slots));
    EXPECT_TRUE(map[id.index].type()[id.slots].has(id.index));
    EXPECT_TRUE(map[id.index].type()[id.slots][id.index].has(id.type));
    EXPECT_EQ(&map[id.index].type()[id.slots][id.index].type(), &map[id.index].type()[id.slots][id.index].type()[id.slots][id.index].type());

    EXPECT_TRUE(map[id.index].type()[id.slots][id.index].type().has(id.slots));
    printAs.cell(map[id.index].type()[id.slots], "map[id.index].type()[id.slots][]");
    printAs.cell(map[id.index].type()[id.slots][id.index], "map[id.index].type()[id.slots][id.index]");
    printAs.value(map[id.index].type()[id.slots][id.index].type(), "map[id.index].type()[id.slots][id.index].type()");
    printAs.value(map[id.index].type()[id.slots][id.index].type()[id.slots], "map[id.index].type()[id.slots][id.index].type()[id.slots]");
    EXPECT_EQ(&map[id.index].type()[id.slots][id.index].type()[id.slots][id.size], &_1_);
    EXPECT_EQ(&map[id.index].type()[id.slots][id.index].type()[id.slots][id.index][_1_][id.value][id.slotRole], &_1_);
    CellI& debug = map[id.index].type()[id.slots][id.index].type()[id.slots][id.index][_1_][id.value][id.slotType];
    EXPECT_EQ(&map[id.index].type()[id.slots][id.index].type()[id.slots][id.index][_1_][id.value][id.slotType], &kb.type.Slot);
    printAs.value(map[id.index].type()[id.slots][id.index].type()[id.slots][id.index], "map[id.index].type()[id.slots][id.index].type()[id.slots][id.index]");
    printAs.value(map[id.index].type()[id.slots][id.index].type()[id.slots][id.index].type(), "map[id.index].type()[id.slots][id.index].type()[id.slots][id.index].type()");

    map.remove(_1_);
    printAs.value(map, "map");
}

TEST_F(CellTest, NextgenType)
{
    nextgen::Map map(kb, kb.type.Number, kb.type.Color);
    map.add(_1_, kb.colors.blue);
    printAs.value(map, "Map<Number, Color>");

    printAs.value(map[id.index][id.type][id.slots][id.list], "map[id.index][id.type][id.slots][id.list]");
    printAs.value(map[id.index][id.type][id.slots][id.index], "map[id.index][id.type][id.slots][id.list]");
    printAs.value(map[id.index][id.type][id.slots][id.index][id.type][id.slots][id.list], "map[id.index][id.type][id.slots][id.index][id.type][id.slots][id.list]");
    printAs.value(map[id.index][id.type][id.slots][id.index][id.type][id.slots][id.index], "map[id.index][id.type][id.slots][id.index][id.type][id.slots][id.index]");
    EXPECT_EQ(&map[id.index][id.type], &map[id.index][id.type][id.slots][id.index][id.type]);

    nextgen::Index index(kb);
    index.set(kb.type.Number, kb.type.Color);
    printAs.value(index, "Index");

    nextgen::Type type(kb, "type");
    type.addSlot(_1_, kb.type.slot(_1_, kb.type.Number));
    printAs.value(type);

    nextgen::Set set(kb, kb.type.Number);
    EXPECT_EQ(&set[id.size], &_0_);
    set.add(_1_);
    EXPECT_EQ(&set[id.size], &_1_);
    printAs.value(set[id.index], "set.index");
    set.add(_2_);
    EXPECT_EQ(&set[id.size], &_2_);
    printAs.value(set[id.index], "set.index");
    set.remove(_1_);
    EXPECT_EQ(&set[id.size], &_1_);
    printAs.value(set[id.index], "set.index");
    set.remove(_2_);
    EXPECT_EQ(&set[id.size], &_0_);
    printAs.value(set[id.index], "set.index");
}

TEST_F(CellTest, NextgenBrainType)
{
    Object map(kb, kb.type.Map2, id.constructor);
    printAs.value(map.type());
    printAs.value(map[id.list].type());
    printAs.value(map[id.index], "map[id.index]");
    printAs.value(map[id.index].type(), "map[id.index].type()");
    map.method(id.add, { id.key, _1_ }, { id.value, kb.colors.red });
    printAs.value(map[id.index], "map[id.index]");
    printAs.value(map[id.index].type(), "map[id.index].type()");
    printAs.cell(map[id.index].type()[id.slots], "map[id.index].type()[id.slots]");
    printAs.cell(map[id.index].type()[id.slots][id.index], "map[id.index].type()[id.slots][id.index]");
    printAs.value(map[id.index].type()[id.slots][id.index], "map[id.index].type()[id.slots][id.index]");
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    std::cout << "Constructed: " << CellI::s_constructed << ", destructed: " << CellI::s_destructed << ", live: " << CellI::s_constructed - CellI::s_destructed << std::endl;

    return ret;
}