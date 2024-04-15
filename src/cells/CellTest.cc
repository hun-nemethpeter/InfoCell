namespace A {
struct AStruct
{
    int a;
};
} // namespace A

namespace B {
namespace A {
struct AStruct
{
    int a;
};
} // namespace A
A::AStruct test;
} // namespace B

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
        m_svgPrinter.writeFile(std::format("E:\\Devel\\ARC\\synth\\svgv-{}.svg", m_postfix));
        m_svgStructPrinter.writeFile(std::format("E:\\Devel\\ARC\\synth\\svgs-{}.svg", m_postfix));
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

    CellI& getStruct(CellI& id)
    {
        return static_cast<TrieMap&>((*kb.compiledGlobalScopePtr)[ids.structs]).getValue(id);
    };

    static std::unique_ptr<brain::Brain> m_kb;
    brain::Brain& kb;
    brain::ID& ids = kb.ids;
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

// TODO
// inline methods
// maybe create a Set<type>?
// type checking
// create an own type for every function to able to save return values to the fn object
// remove .label() from CellI

TEST_F(CellTest, PrintMethod)
{
    const auto printOp = [this](CellI& type, const std::string& method) { printAs.value(type[ids.methods][ids.index][kb.id(method)][ids.value]); };

#if 1
    auto& MapStruct      = getStruct(kb.templateId("Map", ids.keyType, kb.type.Cell, ids.objectType, kb.type.Slot));
    auto& TestStruct     = getStruct(kb.id("Test"));
    auto& TypeStruct     = getStruct(kb.id("Type"));
    auto& ListStruct     = getStruct(kb.templateId("List", ids.objectType, kb.type.Number));
    auto& ListItemStruct = getStruct(kb.templateId("ListItem", ids.objectType, kb.type.Number));

    printOp(MapStruct, "constructor");
    printOp(MapStruct, "constructorWithIndexType");
    printOp(MapStruct, "add");
    printOp(MapStruct, "empty");
    printOp(MapStruct, "getValue");
    printOp(MapStruct, "hasKey");
    printOp(MapStruct, "remove");
    printOp(MapStruct, "size");

    printOp(TypeStruct, "constructor");
    printOp(TypeStruct, "constructorWithRecursiveType");
    printOp(TypeStruct, "addMembership");
    printOp(TypeStruct, "addSlot");
    printOp(TypeStruct, "addSlots");
    printOp(TypeStruct, "addSubType");
    printOp(TypeStruct, "hasSlot");
    printOp(TypeStruct, "removeSlot");

    printOp(ListItemStruct, "constructor");

    printOp(ListStruct, "constructor");
    printOp(ListStruct, "add");
    printOp(ListStruct, "remove");
    printOp(ListStruct, "size");
    printOp(ListStruct, "empty");

    printOp(TestStruct, "factorial");
#endif
}

TEST_F(CellTest, RecursiveCall)
{
    auto& TestStruct = getStruct(kb.id("Test"));
    Object testNumber(kb, TestStruct, "TestStruct");

    EXPECT_EQ(&testNumber.method(kb.id("factorial"), { ids.input, _0_ }), &_1_);
    EXPECT_EQ(&testNumber.method(kb.id("factorial"), { ids.input, _1_ }), &_1_);
    EXPECT_EQ(&testNumber.method(kb.id("factorial"), { ids.input, _2_ }), &_2_);
    EXPECT_EQ(&testNumber.method(kb.id("factorial"), { ids.input, _3_ }), &_6_);
    EXPECT_EQ(&testNumber.method(kb.id("factorial"), { ids.input, _4_ }), &kb.pools.numbers.get(24));
    EXPECT_EQ(&testNumber.method(kb.id("factorial"), { ids.input, _5_ }), &kb.pools.numbers.get(120));
}

TEST_F(CellTest, List)
{
    auto& ListStruct = getStruct(kb.templateId("List", ids.objectType, kb.type.Number));
    Object list(kb, ListStruct, kb.id("constructor"));

    printAs.value(list);
    printAs.cell(list);
    EXPECT_EQ(&list[ids.size], &_0_);
    EXPECT_EQ(&list.method(kb.id("size")), &_0_);
    EXPECT_EQ(&list.method(kb.id("empty")), &true_);
    EXPECT_EQ(&list.type()[ids.subTypes][kb.ids.index][ids.objectType][kb.ids.value], &kb.type.Number);
    EXPECT_FALSE(list.has(ids.first));
    EXPECT_FALSE(list.has(ids.last));

    list.method(kb.id("add"), { ids.value, _1_ });
    EXPECT_EQ(&list[ids.size], &_1_);
    EXPECT_EQ(&list.method(kb.id("size")), &_1_);
    EXPECT_EQ(&list.method(kb.id("empty")), &false_);
    EXPECT_TRUE(list.has(ids.first));
    EXPECT_TRUE(list.has(ids.last));

    CellI& firstItem = list[ids.first];
    EXPECT_EQ(&firstItem, &list[ids.last]);
    EXPECT_FALSE(firstItem.has(ids.previous));
    EXPECT_FALSE(firstItem.has(ids.next));
    EXPECT_EQ(&firstItem[ids.value], &_1_);
    printAs.value(list);

    list.method(kb.id("add"), { ids.value, kb.pools.numbers.get(2) });
    EXPECT_EQ(&list[ids.size], &_2_);
    EXPECT_EQ(&list.method(kb.id("size")), &_2_);
    EXPECT_EQ(&list.method(kb.id("empty")), &false_);

    CellI& secondItem = list[ids.last];
    EXPECT_EQ(&firstItem, &list[ids.first]);
    EXPECT_NE(&firstItem, &list[ids.last]);

    EXPECT_EQ(firstItem.has(ids.previous), false);
    EXPECT_EQ(&firstItem[ids.next], &secondItem);
    EXPECT_EQ(&firstItem[ids.value], &_1_);

    EXPECT_EQ(&secondItem[ids.previous], &firstItem);
    EXPECT_EQ(secondItem.has(ids.next), false);
    EXPECT_EQ(&secondItem[ids.value], &_2_);
    printAs.value(list);

    list.method(kb.id("add"), { ids.value, _3_ });
    EXPECT_EQ(&list[ids.size], &_3_);
    EXPECT_EQ(&list.method(kb.id("size")), &_3_);
    EXPECT_EQ(&list.method(kb.id("empty")), &false_);

    CellI& thirdItem = list[ids.last];
    EXPECT_EQ(&firstItem, &list[ids.first]);
    EXPECT_NE(&firstItem, &list[ids.last]);

    EXPECT_EQ(firstItem.has(ids.previous), false);
    EXPECT_EQ(&firstItem[ids.next], &secondItem);
    EXPECT_EQ(&firstItem[ids.value], &_1_);

    EXPECT_EQ(&secondItem[ids.previous], &firstItem);
    EXPECT_EQ(&secondItem[ids.next], &thirdItem);
    EXPECT_EQ(&secondItem[ids.value], &_2_);

    EXPECT_EQ(&thirdItem[ids.previous], &secondItem);
    EXPECT_EQ(thirdItem.has(ids.next), false);
    EXPECT_EQ(&thirdItem[ids.value], &_3_);
    printAs.value(list);

    list.method(kb.id("remove"), { ids.item, thirdItem });
    EXPECT_EQ(&list[ids.size], &_2_);
    EXPECT_EQ(&list.method(kb.id("size")), &_2_);
    EXPECT_EQ(&list.method(kb.id("empty")), &false_);
    {
        CellI& secondItem = list[ids.last];
        EXPECT_EQ(&firstItem, &list[ids.first]);
        EXPECT_NE(&firstItem, &list[ids.last]);

        EXPECT_EQ(firstItem.has(ids.previous), false);
        EXPECT_EQ(&firstItem[ids.next], &secondItem);
        EXPECT_EQ(&firstItem[ids.value], &_1_);

        EXPECT_EQ(&secondItem[ids.previous], &firstItem);
        EXPECT_EQ(secondItem.has(ids.next), false);
        EXPECT_EQ(&secondItem[ids.value], &_2_);
    }
    printAs.value(list);

    list.method(kb.id("remove"), { ids.item, secondItem });
    EXPECT_EQ(&list[ids.size], &_1_);
    EXPECT_EQ(&list.method(kb.id("size")), &_1_);
    EXPECT_EQ(&list.method(kb.id("empty")), &false_);
    {
        CellI& firstItem = list[ids.first];
        EXPECT_EQ(&firstItem, &list[ids.last]);
        EXPECT_EQ(firstItem.has(ids.previous), false);
        EXPECT_EQ(firstItem.has(ids.next), false);
        EXPECT_EQ(&firstItem[ids.value], &_1_);
    }
    printAs.value(list);

    list.method(kb.id("remove"), { ids.item, firstItem });
    EXPECT_EQ(&list[ids.size], &_0_);
    EXPECT_EQ(&list.method(kb.id("size")), &_0_);
    EXPECT_EQ(&list.method(kb.id("empty")), &true_);
    EXPECT_EQ(&list.type()[ids.subTypes][kb.ids.index][ids.objectType][kb.ids.value], &kb.type.Number);
    EXPECT_FALSE(list.has(ids.first));
    EXPECT_FALSE(list.has(ids.last));
    printAs.value(list);

    CellI& size = list.method(kb.id("size"));
    printAs.value(size);
}

TEST_F(CellTest, Map)
{
    auto& MapStruct = getStruct(kb.templateId("Map", ids.keyType, kb.type.Cell, ids.objectType, kb.type.Slot));
    Object map(kb, MapStruct, kb.id("constructor"));

    printAs.value(map);
    printAs.cell(map);
    EXPECT_EQ(&map[ids.size], &_0_);
    EXPECT_EQ(&map.method(kb.id("size")), &_0_);
    EXPECT_EQ(&map.method(kb.id("empty")), &true_);
    EXPECT_EQ(&map.type()[ids.subTypes][kb.ids.index][ids.keyType][kb.ids.value], &kb.type.Cell);
    EXPECT_EQ(&map.type()[ids.subTypes][kb.ids.index][ids.objectType][kb.ids.value], &kb.type.Slot);

    map.method(kb.id("add"), { ids.key, _1_ }, { ids.value, kb.colors.red });
    printAs.value(map);
    printAs.cell(map);
    EXPECT_EQ(&map[ids.size], &_1_);
    EXPECT_EQ(&map.method(kb.id("size")), &_1_);
    EXPECT_EQ(&map[ids.list][ids.size], &_1_);
    EXPECT_EQ(&map[ids.list][ids.first][ids.value], &kb.colors.red);
    EXPECT_EQ(&map[ids.list][ids.first], &map[ids.list][ids.last]);
    EXPECT_TRUE(map[ids.index].has(_1_));
    EXPECT_EQ(&map[ids.index][_1_][ids.value], &kb.colors.red);
    EXPECT_EQ(&map.method(kb.id("empty")), &false_);
    EXPECT_TRUE(map[ids.index][ids.type][ids.memberOf][ids.index].has(kb.type.Index));

    map.method(kb.id("add"), { ids.key, _2_ }, { ids.value, kb.colors.green });
    map.method(kb.id("add"), { ids.key, _3_ }, { ids.value, kb.colors.blue });
    EXPECT_EQ(&map[ids.index][_1_][ids.value], &kb.colors.red);
    EXPECT_EQ(&map[ids.index][_2_][ids.value], &kb.colors.green);
    EXPECT_EQ(&map[ids.index][_3_][ids.value], &kb.colors.blue);
    printAs.value(map);
    printAs.cell(map);
}

TEST_F(CellTest, MapTypes)
{
    auto& MapStruct = getStruct(kb.templateId("Map", ids.keyType, kb.type.Cell, ids.objectType, kb.type.Slot));
    Object map(kb, MapStruct, kb.id("constructor"));

    printAs.value(map.type());
    printAs.value(map[ids.list].type());
    printAs.value(map[ids.index], "map[ids.index]");
    printAs.value(map[ids.index].type(), "map[ids.index].type()");
    map.method(kb.id("add"), { ids.key, _1_ }, { ids.value, kb.colors.red });
    printAs.value(map[ids.index], "map[ids.index]");
    printAs.value(map[ids.index].type(), "map[ids.index].type()");
    printAs.cell(map[ids.index].type()[ids.slots], "map[ids.index].type()[ids.slots]");
    printAs.cell(map[ids.index].type()[ids.slots][ids.index], "map[ids.index].type()[ids.slots][ids.index]");
    printAs.value(map[ids.index].type()[ids.slots][ids.index], "map[ids.index].type()[ids.slots][ids.index]");
}

TEST_F(CellTest, BuiltInMap)
{
    Map map(kb, kb.type.Number, kb.type.Color);

    printAs.value(map.type());
    printAs.value(map[ids.list].type());
    printAs.cell(map[ids.index], "map[ids.index]");
    printAs.value(map[ids.index], "map[ids.index]");
    printAs.value(map[ids.index].type(), "map[ids.index].type()");
    map.add(_1_, kb.colors.red);
    printAs.value(map, "map");
    printAs.cell(map[ids.index], "map[ids.index]");
    printAs.value(map[ids.index], "map[ids.index]");
    printAs.value(map[ids.index].type(), "map[ids.index].type()");
    printAs.cell(map[ids.index].type()[ids.slots], "map[ids.index].type()[ids.slots]");
    EXPECT_EQ(&map[ids.index][_1_], &map[ids.list][ids.first]);
    EXPECT_EQ(&map[ids.index].type()[ids.slots][ids.size], &_1_);
    EXPECT_EQ(&map[ids.index].type()[ids.slots][ids.keyType], &kb.type.Cell);
    EXPECT_EQ(&map[ids.index].type()[ids.slots][ids.objectType], &kb.type.Slot);
    EXPECT_EQ(&map[ids.index].type()[ids.slots][ids.listType], &kb.type.ListOf(kb.type.Slot));
    EXPECT_EQ(&map[ids.index].type()[ids.slots][ids.list][ids.size], &_1_);
    EXPECT_EQ(&map[ids.index].type()[ids.slots][ids.index][_1_][ids.value][ids.slotRole], &_1_);
    EXPECT_EQ(&map[ids.index].type()[ids.slots][ids.index][_1_][ids.value][ids.slotType], &kb.type.Slot);

    EXPECT_TRUE(map[ids.index].type().has(ids.slots));
    EXPECT_TRUE(map[ids.index].type()[ids.slots].has(ids.index));
    EXPECT_TRUE(map[ids.index].type()[ids.slots][ids.index].has(ids.type));
    EXPECT_EQ(&map[ids.index].type()[ids.slots][ids.index].type(), &map[ids.index].type()[ids.slots][ids.index].type()[ids.slots][ids.index].type());

    EXPECT_TRUE(map[ids.index].type()[ids.slots][ids.index].type().has(ids.slots));
    printAs.cell(map[ids.index].type()[ids.slots], "map[ids.index].type()[ids.slots][]");
    printAs.cell(map[ids.index].type()[ids.slots][ids.index], "map[ids.index].type()[ids.slots][ids.index]");
    printAs.value(map[ids.index].type()[ids.slots][ids.index].type(), "map[ids.index].type()[ids.slots][ids.index].type()");
    printAs.value(map[ids.index].type()[ids.slots][ids.index].type()[ids.slots], "map[ids.index].type()[ids.slots][ids.index].type()[ids.slots]");
    EXPECT_EQ(&map[ids.index].type()[ids.slots][ids.index].type()[ids.slots][ids.size], &_1_);
    EXPECT_EQ(&map[ids.index].type()[ids.slots][ids.index].type()[ids.slots][ids.index][_1_][ids.value][ids.slotRole], &_1_);
    CellI& debug = map[ids.index].type()[ids.slots][ids.index].type()[ids.slots][ids.index][_1_][ids.value][ids.slotType];
    EXPECT_EQ(&map[ids.index].type()[ids.slots][ids.index].type()[ids.slots][ids.index][_1_][ids.value][ids.slotType], &kb.type.Slot);
    printAs.value(map[ids.index].type()[ids.slots][ids.index].type()[ids.slots][ids.index], "map[ids.index].type()[ids.slots][ids.index].type()[ids.slots][ids.index]");
    printAs.value(map[ids.index].type()[ids.slots][ids.index].type()[ids.slots][ids.index].type(), "map[ids.index].type()[ids.slots][ids.index].type()[ids.slots][ids.index].type()");

    map.remove(_1_);
    printAs.value(map, "map");
}

TEST_F(CellTest, MapTemplateTypes)
{
    auto& MapNumberToColor = getStruct(kb.templateId("Map", ids.keyType, kb.type.Number, ids.objectType, kb.type.Color));
    Object map(kb, MapNumberToColor, kb.id("constructor"));

    printAs.value(map.type());
    printAs.value(map[ids.list].type());
    printAs.cell(map[ids.index], "map[ids.index]");
    printAs.value(map[ids.index], "map[ids.index]");
    printAs.value(map[ids.index].type(), "map[ids.index].type()");
    map.method(kb.id("add"), { ids.key, _1_ }, { ids.value, kb.colors.red });
    printAs.value(map, "map");
    printAs.cell(map[ids.index], "map[ids.index]");
    printAs.value(map[ids.index], "map[ids.index]");
    printAs.value(map[ids.index].type(), "map[ids.index].type()");
    printAs.cell(map[ids.index].type()[ids.slots], "map[ids.index].type()[ids.slots]");
    EXPECT_EQ(&map[ids.index][_1_], &map[ids.list][ids.first]);
    EXPECT_EQ(&map[ids.index].type()[ids.slots][ids.size], &_1_);
    EXPECT_EQ(&map[ids.index].type()[ids.slots][ids.list][ids.size], &_1_);
    EXPECT_EQ(&map[ids.index].type()[ids.slots][ids.index][_1_][ids.value][ids.slotRole], &_1_);
    CellI& debug2 = map[ids.index].type()[ids.slots][ids.index][_1_][ids.value][ids.slotType];
    EXPECT_EQ(&map[ids.index].type()[ids.slots][ids.index][_1_][ids.value][ids.slotType], &kb.type.Slot);

    EXPECT_TRUE(map[ids.index].type().has(ids.slots));
    EXPECT_TRUE(map[ids.index].type()[ids.slots].has(ids.index));
    EXPECT_TRUE(map[ids.index].type()[ids.slots][ids.index].has(ids.type));
    EXPECT_EQ(&map[ids.index].type()[ids.slots][ids.index].type(), &map[ids.index].type()[ids.slots][ids.index].type()[ids.slots][ids.index].type());

    EXPECT_TRUE(map[ids.index].type()[ids.slots][ids.index].type().has(ids.slots));
    printAs.cell(map[ids.index].type()[ids.slots], "map[ids.index].type()[ids.slots][]");
    printAs.cell(map[ids.index].type()[ids.slots][ids.index], "map[ids.index].type()[ids.slots][ids.index]");
    printAs.value(map[ids.index].type()[ids.slots][ids.index].type(), "map[ids.index].type()[ids.slots][ids.index].type()");
    printAs.value(map[ids.index].type()[ids.slots][ids.index].type()[ids.slots], "map[ids.index].type()[ids.slots][ids.index].type()[ids.slots]");
    EXPECT_EQ(&map[ids.index].type()[ids.slots][ids.index].type()[ids.slots][ids.size], &_1_);
    EXPECT_EQ(&map[ids.index].type()[ids.slots][ids.index].type()[ids.slots][ids.index][_1_][ids.value][ids.slotRole], &_1_);
    CellI& debug = map[ids.index].type()[ids.slots][ids.index].type()[ids.slots][ids.index][_1_][ids.value][ids.slotType];
    EXPECT_EQ(&map[ids.index].type()[ids.slots][ids.index].type()[ids.slots][ids.index][_1_][ids.value][ids.slotType], &kb.type.Slot);
    printAs.value(map[ids.index].type()[ids.slots][ids.index].type()[ids.slots][ids.index], "map[ids.index].type()[ids.slots][ids.index].type()[ids.slots][ids.index]");
    printAs.value(map[ids.index].type()[ids.slots][ids.index].type()[ids.slots][ids.index].type(), "map[ids.index].type()[ids.slots][ids.index].type()[ids.slots][ids.index].type()");
}

TEST_F(CellTest, MapNumberToColor)
{
    auto& MapNumberToColor = getStruct(kb.templateId("Map", ids.keyType, kb.type.Number, ids.objectType, kb.type.Color));
    Object map(kb, MapNumberToColor, kb.id("constructor"));

    EXPECT_EQ(&map.type()[ids.subTypes][kb.ids.index][ids.keyType][kb.ids.value], &kb.type.Number);
    EXPECT_EQ(&map.type()[ids.subTypes][kb.ids.index][ids.objectType][kb.ids.value], &kb.type.Color);

    printAs.value(map);
    printAs.cell(map);
    EXPECT_EQ(&map[ids.size], &_0_);
    EXPECT_EQ(&map.method(kb.id("size")), &_0_);
    EXPECT_EQ(&map.method(kb.id("empty")), &true_);

    map.method(kb.id("add"), { ids.key, _1_}, { ids.value, kb.colors.red });
    printAs.value(map);
    printAs.cell(map);
    EXPECT_EQ(&map[ids.size], &_1_);
    EXPECT_EQ(&map.method(kb.id("size")), &_1_);
    EXPECT_EQ(&map[ids.list][ids.size], &_1_);
    EXPECT_EQ(&map[ids.list][ids.first][ids.value], &kb.colors.red);
    EXPECT_EQ(&map[ids.list][ids.first], &map[ids.list][ids.last]);
    EXPECT_TRUE(map[ids.index].has(_1_));
    EXPECT_EQ(&map[ids.index][_1_][ids.value], &kb.colors.red);
    EXPECT_EQ(&map.method(kb.id("empty")), &false_);
    EXPECT_TRUE(map[ids.index][ids.type][ids.memberOf][ids.index].has(kb.type.Index));

    map.method(kb.id("add"), { ids.key, _2_ }, { ids.value, kb.colors.green });
    map.method(kb.id("add"), { ids.key, _3_ }, { ids.value, kb.colors.blue });
    EXPECT_EQ(&map[ids.index][_1_][ids.value], &kb.colors.red);
    EXPECT_EQ(&map[ids.index][_2_][ids.value], &kb.colors.green);
    EXPECT_EQ(&map[ids.index][_3_][ids.value], &kb.colors.blue);
    printAs.value(map);
    printAs.cell(map);
}

TEST_F(CellTest, ListItem)
{
    auto& ListItemStruct = getStruct(kb.templateId("ListItem", ids.objectType, kb.type.Color));
    Object listItem(kb, ListItemStruct, kb.id("constructor"), { ids.value, kb.colors.green });

    EXPECT_EQ(&listItem[ids.value], &kb.colors.green);

    EXPECT_EQ(&listItem.type()[ids.memberOf][ids.size], &_1_);
    EXPECT_TRUE(listItem.type()[ids.memberOf][ids.index].has(kb.type.ListItem));

    EXPECT_EQ(&listItem.type()[ids.slots][ids.size], &_3_);
    EXPECT_TRUE(listItem.type()[ids.slots][ids.index].has(ids.previous));
    EXPECT_TRUE(listItem.type()[ids.slots][ids.index].has(ids.next));
    EXPECT_TRUE(listItem.type()[ids.slots][ids.index].has(ids.value));

    EXPECT_TRUE(listItem.type()[ids.methods][ids.index].has(kb.id("constructor")));
}

TEST_F(CellTest, ListItemTemplate)
{
    auto& ListItemNumber  = getStruct(kb.templateId("ListItem", ids.objectType, kb.type.Number));
    Object listItemNumber(kb, ListItemNumber, kb.id("constructor"), { ids.value, _1_ });

    EXPECT_EQ(&listItemNumber[ids.value], &_1_);

    printAs.value(kb.type.ListItem, "type.ListItem");
    printAs.value(ListItemNumber, "listItemNumber");
    printAs.value(kb.type.ListItem[ids.slots][ids.list], "type.ListItem[slots]");
    printAs.value(ListItemNumber[ids.slots][ids.list], "listItemNumber[slots]");

    EXPECT_EQ(&ListItemNumber[ids.memberOf][ids.size], &_1_);
    EXPECT_TRUE(ListItemNumber[ids.memberOf][ids.index].has(kb.type.ListItem));

    EXPECT_EQ(&ListItemNumber[ids.slots][ids.size], &_3_);
    EXPECT_TRUE(ListItemNumber[ids.slots][ids.index].has(ids.previous));
    EXPECT_TRUE(ListItemNumber[ids.slots][ids.index].has(ids.next));
    EXPECT_TRUE(ListItemNumber[ids.slots][ids.index].has(ids.value));

    EXPECT_TRUE(ListItemNumber.has(ids.methods));
}

TEST_F(CellTest, ListTemplate)
{
    auto& ListOfNumbers = getStruct(kb.templateId("List", ids.objectType, kb.type.Number));

    EXPECT_EQ(&ListOfNumbers[ids.subTypes][ids.size], &_2_);
    EXPECT_TRUE(ListOfNumbers[ids.subTypes][ids.index].has(ids.objectType));
    EXPECT_EQ(&ListOfNumbers[ids.subTypes][ids.index][ids.objectType][ids.value], &kb.type.Number);
    EXPECT_TRUE(ListOfNumbers[ids.subTypes][ids.index].has(ids.itemType));
    CellI& ListItemType = ListOfNumbers[ids.subTypes][ids.index][ids.itemType][ids.value];
    EXPECT_EQ(&ListItemType[ids.slots][ids.index][ids.value][ids.value][ids.slotType], &kb.type.Number);
    EXPECT_NE(&ListItemType, &kb.type.ListItem);
    EXPECT_EQ(&ListItemType[ids.memberOf][ids.size], &_1_);
    EXPECT_TRUE(ListItemType[ids.memberOf][ids.index].has(kb.type.ListItem));

    EXPECT_EQ(&ListItemType[ids.slots][ids.size], &_3_);
    EXPECT_TRUE(ListItemType[ids.slots][ids.index].has(ids.previous));
    EXPECT_TRUE(ListItemType[ids.slots][ids.index].has(ids.next));
    EXPECT_TRUE(ListItemType[ids.slots][ids.index].has(ids.value));

    EXPECT_EQ(&ListOfNumbers[ids.slots][ids.size], &_3_);
    EXPECT_TRUE(ListOfNumbers[ids.slots][ids.index].has(ids.first));
    EXPECT_EQ(&ListOfNumbers[ids.slots][ids.index][ids.first][ids.value][ids.slotType], &ListItemType);

    EXPECT_TRUE(ListOfNumbers[ids.slots][ids.index].has(ids.last));
    EXPECT_EQ(&ListOfNumbers[ids.slots][ids.index][ids.last][ids.value][ids.slotType], &ListItemType);

    EXPECT_TRUE(ListOfNumbers[ids.slots][ids.index].has(ids.size));
    EXPECT_EQ(&ListOfNumbers[ids.slots][ids.index][ids.size][ids.value][ids.slotType], &kb.type.Number);

    EXPECT_TRUE(ListOfNumbers[ids.memberOf][ids.index].has(kb.type.List));
    EXPECT_EQ(&ListOfNumbers[ids.memberOf][ids.size], &_2_);

    printAs.value(kb.type.List, "type.List");
    printAs.value(ListOfNumbers, "ListOfNumbers");
}

TEST_F(CellTest, FunctionTypes)
{
    CellI& function = kb.type.Type_[ids.methods][ids.index][kb.id("addSlot")][ids.value];
    printAs.cell(function.type(), "function");
    printAs.cell(function.type()[ids.subTypes][ids.index][ids.localVars][ids.value], "function[ids.subTypes][ids.localVars][ids.value]");
    printAs.value(function.type()[ids.subTypes][ids.index][ids.localVars][ids.value], "function[ids.subTypes][ids.localVars][ids.value]");
}

TEST_F(CellTest, HybridPicture)
{
    input::Picture inputPicture("input");
    inputPicture.loadFromJsonArray("[[0, 7, 0], [7, 7, 7], [0, 7, 0]]");
    hybrid::Picture picture(kb, inputPicture);

    printAs.svg(picture);
    printAs.svg(picture[ids.pixels]);
    printAs.value(picture[ids.pixels]);

    EXPECT_EQ(&picture[ids.type], &kb.type.Picture);
    EXPECT_EQ(&picture[ids.width], &kb.pools.numbers.get(3));
    EXPECT_EQ(&picture[ids.height], &kb.pools.numbers.get(3));
    EXPECT_EQ(&picture[ids.pixels][ids.type], &kb.type.ListOf(kb.type.Pixel));

    auto& ListOfPixels  = getStruct(kb.templateId("List", ids.objectType, kb.type.Pixel));
    Object listOfPixels(kb, ListOfPixels, kb.id("constructor"), "listOfPixels");
    listOfPixels.method(kb.id("add"), { ids.value, picture[ids.pixels][ids.first][ids.value] });
    listOfPixels.method(kb.id("add"), { ids.value, picture[ids.pixels][ids.first][ids.next][ids.value] });
    printAs.value(listOfPixels);
}

TEST_F(CellTest, BasicObjectTest)
{
    auto& Type_ = getStruct(kb.id("Type"));
    auto& List = getStruct(kb.templateId("List", ids.objectType, kb.type.Cell));
    Object testType(kb, Type_, "Test");
    Object emptyList(kb, List, kb.id("constructor"));

    testType.method(kb.id("addSlots"), { ids.list, emptyList });

    testType.method(kb.id("addSlots"), { ids.list, kb.list(
        kb.type.slot(ids.result, kb.type.Digit),
        kb.type.slot(ids.value, kb.type.Number))}); // TODO implement type checking

    Object object(kb, testType, "testObject");

    EXPECT_EQ(object.label(), "testObject");
    EXPECT_EQ(&object.type(), &testType);

    EXPECT_ANY_THROW(&object[ids.value]);
    EXPECT_NO_THROW(object.set(ids.value, kb.pools.numbers.get(42)));
    printAs.value(object[ids.value]);
    EXPECT_ANY_THROW(object.set(ids.argument, kb.pools.numbers.get(42)));
}

TEST_F(CellTest, BasicControlOpTest)
{
    Object testValue1(kb, kb.type.op.ConstVar);
    testValue1.set(ids.value, kb.type.Char);

    Object testValue2(kb, kb.type.op.ConstVar);
    testValue2.set(ids.value, kb.type.Color);

    Object sameOpEq(kb, kb.type.op.Same, "sameOpEq");
    sameOpEq.set(ids.lhs, testValue1);
    sameOpEq.set(ids.rhs, testValue1);

    Object sameOpNe(kb, kb.type.op.Same, "sameOpNe");
    sameOpNe.set(ids.lhs, testValue1);
    sameOpNe.set(ids.rhs, testValue2);

    sameOpEq();
    sameOpNe();

    printAs.value(sameOpEq[ids.value], "testValue1 == testValue1");
    printAs.value(sameOpNe[ids.value], "testValue1 != testValue1");

    EXPECT_EQ(&sameOpEq[ids.value], &true_);
    EXPECT_EQ(&sameOpNe[ids.value], &false_);
}

TEST_F(CellTest, BasicControlAddTest)
{
    Object start(kb, kb.type.op.ConstVar);
    start.set(ids.value, kb.pools.numbers.get(42));

    Object value10(kb, kb.type.op.ConstVar);
    value10.set(ids.value, kb.pools.numbers.get(10));

    Object add10(kb, kb.type.op.Add, "add10");
    add10.set(ids.lhs, start);
    add10.set(ids.rhs, value10);
    add10();
    std::cout << "42 + 10 = ";
    printAs.value(add10[ids.value]);

    EXPECT_EQ(&add10[ids.value], &kb.pools.numbers.get(52));
}

TEST_F(CellTest, CreatingCustomType)
{
    Object colorRed(kb, kb.type.Cell, "red");
    Object colorGreen(kb, kb.type.Cell, "green");
    Object colorBlue(kb, kb.type.Cell, "blue");

    Object colorClass(kb, kb.type.Type_, "Color");
#if 1 // TODO
    auto& MapCellToSlot = getStruct(kb.templateId("Map", ids.keyType, kb.type.Cell, ids.objectType, kb.type.Slot));
    Object* slotMapPtr  = new Object(kb, MapCellToSlot, kb.id("constructor"));
    colorClass.set(ids.slots, *slotMapPtr);
#endif

    colorClass.method(kb.id("addSlots"), { ids.list, kb.list(kb.type.slot(colorRed, kb.type.Number), kb.type.slot(colorGreen, kb.type.Number), kb.type.slot(colorBlue, kb.type.Number)) });

    Object redColor(kb, colorClass, "redColor");
    redColor.set(colorRed, kb.pools.numbers.get(255));
    redColor.set(colorGreen, kb.pools.numbers.get(0));
    redColor.set(colorBlue, kb.pools.numbers.get(0));

    printAs.value(colorClass, "colorClass:");
    printAs.value(colorClass[ids.slots][ids.list], "colorClass::slots");
    printAs.value(colorClass[ids.slots][ids.index][colorRed]);
    EXPECT_TRUE(colorClass[ids.slots][ids.index].has(colorRed));
    EXPECT_TRUE(colorClass[ids.slots][ids.index].has(colorGreen));
    EXPECT_TRUE(colorClass[ids.slots][ids.index].has(colorBlue));
    printAs.value(colorClass[ids.slots][ids.index][colorRed][ids.type], "colorClass slot of colorRed");
    EXPECT_EQ(&colorClass[ids.slots][ids.index][colorRed][ids.value][ids.type], &kb.type.Slot);
    EXPECT_EQ(&colorClass[ids.slots][ids.index][colorRed][ids.value][ids.type], &kb.type.Slot);
    EXPECT_EQ(&colorClass[ids.slots][ids.index][colorGreen][ids.value][ids.type], &kb.type.Slot);
    EXPECT_EQ(&colorClass[ids.slots][ids.index][colorBlue][ids.value][ids.type], &kb.type.Slot);
    printAs.value(redColor);

    printAs.cell(redColor);
    printAs.cell(colorClass[ids.slots][ids.index][colorRed][ids.value], "colorClass[ids.slots][ids.index][colorRed][ids.value]");
    printAs.cell(colorClass[ids.slots][ids.index], "colorClass[ids.slots][ids.index]");

    printAs.svgStruct(redColor, "redColor");
    printAs.svgStruct(colorRed, "colorRed");
    printAs.svgStruct(colorClass, "Color");
    printAs.svgStruct(colorClass[ids.slots][ids.index], "SlotIndex of Color");
    printAs.svgStruct(colorClass[ids.slots][ids.list], "SlotList of Color");
    printAs.svgStruct(colorClass[ids.slots][ids.list][ids.first], "SlotListItem1 of Color");
    printAs.svgStruct(colorClass[ids.slots][ids.list][ids.first][ids.value], "Slot1 of Color");
    printAs.svgStruct(colorClass[ids.slots][ids.list][ids.first][ids.next], "SlotListItem2 of Color");
    printAs.svgStruct(colorClass[ids.slots][ids.list][ids.first][ids.next][ids.value], "Slot2 of Color");
    printAs.value(colorClass[ids.slots][ids.index], "colorClass[ids.slots][ids.index]");
    printAs.value(colorClass[ids.slots][ids.index][ids.type], "colorClass[ids.slots][ids.index][ids.type]");
    printAs.value(colorClass[ids.slots][ids.index][ids.type][ids.slots], "colorClass[ids.slots][ids.index][ids.type][ids.slots]");
    printAs.value(colorClass[ids.slots][ids.index][ids.type][ids.slots][ids.index], "colorClass[ids.slots][ids.index][ids.type][ids.slots][ids.index]");
    printAs.cell(colorClass);

    printAs.svg(redColor);
    printAs.svg(colorClass[ids.slots][ids.index][colorRed][ids.value]);
    printAs.svg(colorClass);
}

TEST_F(CellTest, CreatingNumber)
{
    Number& number_0   = kb.pools.numbers.get(0);
    Number& number_255 = kb.pools.numbers.get(255);

    printAs.value(number_255);
    printAs.value(number_255[ids.value][ids.first][ids.value]);

    printAs.svgStruct(ids.emptyObject, "emptyObject");

    printAs.cell(number_255);
    printAs.cell(number_255[kb.numbers.sign]);
    printAs.cell(number_255[ids.value]);
    printAs.cell(number_255[ids.value][ids.first]);
    printAs.cell(number_255[ids.value][ids.first][ids.value]);
    printAs.cell(number_255[ids.value][ids.last]);
    printAs.cell(number_255[ids.value][ids.size]);

    printAs.svg(number_255);
    printAs.svg(number_255[kb.numbers.sign]);
    printAs.svg(number_255[ids.value]);
    printAs.svg(number_255[ids.value][ids.first]);
    printAs.svg(number_255[ids.value][ids.first][ids.value]);
    printAs.svg(number_255[ids.value][ids.last]);
    printAs.svg(number_255[ids.value][ids.size]);
}

TEST_F(CellTest, NextgenList)
{
    List list(kb, kb.type.Number);
    EXPECT_FALSE(list.has(ids.first));
    EXPECT_FALSE(list.has(ids.last));
    EXPECT_EQ(&list[ids.size], &_0_);
    EXPECT_EQ(&list[ids.objectType], &kb.type.Number);
    EXPECT_TRUE(list.empty());
    printAs.value(list);

    auto& item1 = *list.add(_1_);
    EXPECT_TRUE(list.has(ids.first));
    EXPECT_TRUE(list.has(ids.last));
    EXPECT_EQ(&list[ids.size], &_1_);
    EXPECT_FALSE(list.empty());

    EXPECT_EQ(&item1, &list[ids.first]);
    EXPECT_EQ(&item1, &list[ids.last]);
    EXPECT_FALSE(item1.has(ids.previous));
    EXPECT_FALSE(item1.has(ids.next));
    EXPECT_EQ(&item1[ids.value], &_1_);
    printAs.value(list);

    auto& item2 = *list.add(_2_);
    EXPECT_TRUE(list.has(ids.first));
    EXPECT_TRUE(list.has(ids.last));
    EXPECT_EQ(&list[ids.size], &_2_);
    EXPECT_FALSE(list.empty());

    EXPECT_EQ(&item1, &list[ids.first]);
    EXPECT_EQ(&item2, &list[ids.last]);
    EXPECT_FALSE(item1.has(ids.previous));
    EXPECT_TRUE(item1.has(ids.next));
    EXPECT_TRUE(item2.has(ids.previous));
    EXPECT_FALSE(item2.has(ids.next));
    EXPECT_EQ(&item1[ids.value], &_1_);
    EXPECT_EQ(&item2[ids.value], &_2_);
    printAs.value(list);

    auto& item3 = *list.add(_3_);
    EXPECT_TRUE(list.has(ids.first));
    EXPECT_TRUE(list.has(ids.last));
    EXPECT_EQ(&list[ids.size], &_3_);
    EXPECT_FALSE(list.empty());
    printAs.value(list);

    list.removeItem(&item2);
    EXPECT_TRUE(list.has(ids.first));
    EXPECT_TRUE(list.has(ids.last));
    EXPECT_EQ(&list[ids.size], &_2_);
    EXPECT_FALSE(list.empty());
    printAs.value(list);

    list.removeItem(&item1);
    EXPECT_TRUE(list.has(ids.first));
    EXPECT_TRUE(list.has(ids.last));
    EXPECT_EQ(&list[ids.size], &_1_);
    EXPECT_FALSE(list.empty());
    printAs.value(list);

    list.removeItem(&item3);
    EXPECT_FALSE(list.has(ids.first));
    EXPECT_FALSE(list.has(ids.last));
    EXPECT_EQ(&list[ids.size], &_0_);
    EXPECT_TRUE(list.empty());
    printAs.value(list);
}


TEST_F(CellTest, NextgenType)
{
    Map map(kb, kb.type.Number, kb.type.Color);
    map.add(_1_, kb.colors.blue);
    printAs.value(map, "Map<Number, Color>");

    printAs.value(map[ids.index][ids.type][ids.slots][ids.list], "map[ids.index][ids.type][ids.slots][ids.list]");
    printAs.value(map[ids.index][ids.type][ids.slots][ids.index], "map[ids.index][ids.type][ids.slots][ids.list]");
    printAs.value(map[ids.index][ids.type][ids.slots][ids.index][ids.type][ids.slots][ids.list], "map[ids.index][ids.type][ids.slots][ids.index][ids.type][ids.slots][ids.list]");
    printAs.value(map[ids.index][ids.type][ids.slots][ids.index][ids.type][ids.slots][ids.index], "map[ids.index][ids.type][ids.slots][ids.index][ids.type][ids.slots][ids.index]");
    EXPECT_EQ(&map[ids.index][ids.type], &map[ids.index][ids.type][ids.slots][ids.index][ids.type]);

    Index index(kb);
    index.set(kb.type.Number, kb.type.Color);
    printAs.value(index, "Index");

    Type type(kb, "type");
    type.addSlot(_1_, kb.type.slot(_1_, kb.type.Number));
    printAs.value(type);

    Set set(kb, kb.type.Number);
    EXPECT_EQ(&set[ids.size], &_0_);
    set.add(_1_);
    EXPECT_EQ(&set[ids.size], &_1_);
    printAs.value(set[ids.index], "set.index");
    set.add(_2_);
    EXPECT_EQ(&set[ids.size], &_2_);
    printAs.value(set[ids.index], "set.index");
    set.remove(_1_);
    EXPECT_EQ(&set[ids.size], &_1_);
    printAs.value(set[ids.index], "set.index");
    set.remove(_2_);
    EXPECT_EQ(&set[ids.size], &_0_);
    printAs.value(set[ids.index], "set.index");
}

TEST_F(CellTest, NextgenBrainType)
{
    auto& MapNumberToColor = getStruct(kb.templateId("Map", ids.keyType, kb.type.Number, ids.objectType, kb.type.Color));
    Object map(kb, MapNumberToColor, kb.id("constructor"));

    EXPECT_EQ(&map[ids.size], &_0_);
    EXPECT_EQ(&map.method(kb.id("size")), &_0_);
    EXPECT_EQ(&map.method(kb.id("empty")), &true_);
    printAs.value(map.type());
    printAs.value(map[ids.list].type());
    printAs.value(map[ids.index], "map[ids.index]");
    printAs.value(map[ids.index].type(), "map[ids.index].type()");

    map.method(kb.id("add"), { ids.key, _1_ }, { ids.value, kb.colors.red });
    printAs.value(map[ids.index], "map[ids.index]");
    printAs.value(map[ids.index].type(), "map[ids.index].type()");
    printAs.cell(map[ids.index].type()[ids.slots], "map[ids.index].type()[ids.slots]");
    printAs.cell(map[ids.index].type()[ids.slots][ids.index], "map[ids.index].type()[ids.slots][ids.index]");
    printAs.value(map[ids.index].type()[ids.slots][ids.index], "map[ids.index].type()[ids.slots][ids.index]");
    EXPECT_EQ(&map[ids.size], &_1_);
    EXPECT_EQ(&map.method(kb.id("size")), &_1_);
    EXPECT_EQ(&map[ids.list][ids.size], &_1_);
    EXPECT_EQ(&map[ids.list][ids.first][ids.value], &kb.colors.red);
    EXPECT_EQ(&map[ids.list][ids.first], &map[ids.list][ids.last]);
    EXPECT_TRUE(map[ids.index].has(_1_));
    EXPECT_EQ(&map[ids.index][_1_][ids.value], &kb.colors.red);
    EXPECT_EQ(&map.method(kb.id("empty")), &false_);
    EXPECT_TRUE(map[ids.index][ids.type][ids.memberOf][ids.index].has(kb.type.Index));

    map.method(kb.id("add"), { ids.key, _2_ }, { ids.value, kb.colors.green });
    map.method(kb.id("add"), { ids.key, _3_ }, { ids.value, kb.colors.blue });
    EXPECT_EQ(&map[ids.index][_1_][ids.value], &kb.colors.red);
    EXPECT_EQ(&map[ids.index][_2_][ids.value], &kb.colors.green);
    EXPECT_EQ(&map[ids.index][_3_][ids.value], &kb.colors.blue);
    printAs.value(map);
    printAs.cell(map);

    map.method(kb.id("remove"), { ids.key, _3_ });
    map.method(kb.id("remove"), { ids.key, _2_ });

    EXPECT_EQ(&map[ids.size], &_1_);
    EXPECT_EQ(&map.method(kb.id("size")), &_1_);
    EXPECT_EQ(&map[ids.list][ids.size], &_1_);
    EXPECT_EQ(&map[ids.list][ids.first][ids.value], &kb.colors.red);
    EXPECT_EQ(&map[ids.list][ids.first], &map[ids.list][ids.last]);
    EXPECT_TRUE(map[ids.index].has(_1_));
    EXPECT_EQ(&map[ids.index][_1_][ids.value], &kb.colors.red);
    EXPECT_EQ(&map.method(kb.id("empty")), &false_);
    EXPECT_TRUE(map[ids.index][ids.type][ids.memberOf][ids.index].has(kb.type.Index));
}

////////////
// Proof of concept trie class
class Trie
{
    struct Node
    {
        Node(char data);
        ~Node();

        char m_data = 0;
        std::map<char, Node*> m_children;
        int m_isLeaf = false;
    };

public:
    Trie();
    bool empty();
    void insert(const std::string& word);
    int search(const std::string& word);
    void remove(const std::string& word);
    void print();

private:
    void printCb(Node* node);
    void removeCb(Node& node, const std::string& key, int depth);

    std::unique_ptr<Node> m_root;
};

Trie::Node::Node(char data) :
    m_data(data) { }

Trie::Node::~Node()
{
    for (auto& pair : m_children) {
        delete pair.second;
    }
}

Trie::Trie()
{
    m_root = std::make_unique<Node>('\0');
}

bool Trie::empty()
{
    return m_root->m_children.empty();
}

void Trie::insert(const std::string& word)
{
    Node* currentNode = m_root.get();

    for (char ch : word) {
        Node*& children = currentNode->m_children[ch];
        if (children == nullptr) {
            children = new Node(ch);
        }
        currentNode = children;
    }
    // At the end of the word, mark this node as the leaf node
    currentNode->m_isLeaf = 1;
}

int Trie::search(const std::string& word)
{
    // Searches for word in the Trie
    Node* currentNode = m_root.get();

    for (char ch : word) {
        auto chFindIt = currentNode->m_children.find(ch);
        if (chFindIt == currentNode->m_children.end())
            return 0;
        currentNode = chFindIt->second;
    }
    if (currentNode->m_isLeaf == 1)
        return 1;

    return 0;
}
void Trie::remove(const std::string& word)
{
    removeCb(*m_root, word, 0);
}

void Trie::removeCb(Node& node, const std::string& key, int depth)
{
    if (depth == key.length()) {
        node.m_isLeaf = false;
        return;
    }

    char ch       = key[depth];
    auto chFindIt = node.m_children.find(ch);
    if (chFindIt == node.m_children.end()) {
        return;
    }
    Node& child = *chFindIt->second;

    removeCb(child, key, depth + 1);

    if (!child.m_isLeaf) {
        if (child.m_children.empty()) {
            delete &child;
            node.m_children.erase(chFindIt);
        }
    }
}

void Trie::print()
{
    if (empty())
        return;
    printCb(m_root.get());
}

void Trie::printCb(Node* node)
{
    printf("%c -> ", node->m_data);

    for (auto& it : node->m_children) {
        printCb(it.second);
    }
}

void print_search_new(Trie& trie, const std::string& word)
{
    std::cout << std::format("Searching for {}:", word);
    if (trie.search(word) == 0)
        std::cout << "Not Found\n";
    else
        std::cout << "Found!\n";
}
////////////

TEST_F(CellTest, TrieTest)
{
    Trie trie;
    trie.insert("hello");
    trie.insert("hi");
    trie.insert("teabag");
    trie.insert("teacan");
    print_search_new(trie, "tea");
    print_search_new(trie, "teabag");
    print_search_new(trie, "teacan");
    print_search_new(trie, "hi");
    print_search_new(trie, "hey");
    print_search_new(trie, "hello");
    trie.print();
    printf("\n");
    trie.remove("hello");
    printf("After deleting 'hello'...\n");
    trie.print();
    printf("\n");
    trie.remove("teacan");
    printf("After deleting 'teacan'...\n");
    trie.print();
    printf("\n");
}

TEST_F(CellTest, TrieMapTest)
{
    TrieMap trieMap(kb, kb.type.Number, kb.type.Number, "testTrieMap");
    EXPECT_EQ(&trieMap[ids.size], &_0_);
    auto& key1   = kb.list(kb._0_, kb._1_, kb._2_, kb._3_, kb._4_);
    auto& value1 = kb.directions.down;
    trieMap.add(key1, value1);
    EXPECT_EQ(&trieMap[ids.size], &_1_);
    EXPECT_TRUE(trieMap.hasKey(key1));
    EXPECT_EQ(&trieMap.getValue(key1), &value1);
    trieMap.remove(key1);
    EXPECT_EQ(&trieMap[ids.size], &_0_);
    EXPECT_FALSE(trieMap.hasKey(key1));
}

TEST_F(CellTest, StringTest)
{
    String& testStr1 = kb.pools.strings.get("test");
    String& testStr2 = kb.pools.strings.get("test");
    EXPECT_EQ(&testStr1, &testStr2);
    EXPECT_EQ(&testStr1.type(), &kb.type.String);
    EXPECT_EQ(&testStr1[kb.ids.value].type(), &kb.type.ListOf(kb.type.Char));
    EXPECT_EQ(&testStr1[kb.ids.value], &testStr1[kb.ids.value]);
    EXPECT_EQ(&testStr1[kb.ids.value], &testStr2[kb.ids.value]);
    printAs.value(testStr1[kb.ids.value]);
    printAs.cell(testStr1[kb.ids.value]);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    std::cout << "Constructed: " << CellI::s_constructed << ", destructed: " << CellI::s_destructed << ", live: " << CellI::s_constructed - CellI::s_destructed << std::endl;

    return ret;
}