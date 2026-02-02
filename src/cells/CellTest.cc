#include "CellTestBase.h"

#include <fmt/core.h>
#include <fstream>
#include <gtest/gtest.h>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <nlohmann/json.hpp>

#include "Config.h"
#include "app/App.h"
#include "Cells.h"
#include "SVGPrinter.h"
#include "SVGStructPrinter.h"
#include "StructPrinter.h"
#include "ValuePrinter.h"
#include "util/ArcTask.h"

namespace fs = std::filesystem;
using nlohmann::json;
using namespace infocell;
using namespace infocell::cells;

using infocell::cells::test::CellTest;

// TODO
// inline methods
// type checking
// remove .label() from CellI

TEST_F(CellTest, CellTrieTestForSet)
{
    brain::CellTrie& cellTrie = *kb.globalScope.m_cellTrie;
    // test the pixel.set(green, 5)
    Object& pixel           = *new Object(kb, kb.std.Color, "pixel");
    CellI& requestForSetGet = *new Object(kb, kb.std.ast.Get);
    requestForSetGet.set(kb.ids.cell, kb.ast.cell(pixel));
    requestForSetGet.set(kb.ids.role, kb.ast.cell(kb.ids.green));

    CellI& requestForSet = *new Object(kb, kb.std.ast.Equal, "pixel.get(green) == 5");
    requestForSet.set(kb.ids.lhs, requestForSetGet);
    requestForSet.set(kb.ids.rhs, kb.ast.cell(kb._5_));

    CellI& requestForSetAstList = cellTrie.serializeAst(requestForSet);
    {
        std::stringstream ss;
        Visitor::visitList(requestForSetAstList, [&ss](CellI& value, int, bool& stop) {
            ss << value.label() << " ";
        });
        EXPECT_EQ(ss.str(), "struct ast::Equal lhs op push struct ast::Get cell pixel role green op pop rhs 5 ");
    }

    CellI* toolBuilder      = cellTrie.findToolByAst(requestForSet);
    CellI& resultToolAstVar = *new Object(kb, kb.std.ast.Var, "resultToolAstVar");
    cellTrie.createTool(resultToolAstVar, requestForSet, *toolBuilder);
    CellI& resultToolAst = resultToolAstVar[kb.ids.name];

    EXPECT_EQ(&resultToolAst.struct_(), &kb.std.ast.Set);
    EXPECT_EQ(&resultToolAst[kb.ids.cell].struct_(), &kb.std.ast.Cell);
    EXPECT_EQ(&resultToolAst[kb.ids.cell][kb.ids.value], &pixel);
    EXPECT_EQ(&resultToolAst[kb.ids.role].struct_(), &kb.std.ast.Cell);
    EXPECT_EQ(&resultToolAst[kb.ids.role][kb.ids.value], &kb.ids.green);
    EXPECT_EQ(&resultToolAst[kb.ids.value].struct_(), &kb.std.ast.Cell);
    EXPECT_EQ(&resultToolAst[kb.ids.value][kb.ids.value], &kb._5_);
}

TEST_F(CellTest, CellTrieTestForGet)
{
    brain::CellTrie& cellTrie = *kb.globalScope.m_cellTrie;
    Object& pixel             = *new Object(kb, kb.std.Color, "pixel");

    // test the return get(x, y)
    CellI& requestForGetGet = *new Object(kb, kb.std.ast.Get);
    requestForGetGet.set(kb.ids.cell, kb.ast.cell(pixel));
    requestForGetGet.set(kb.ids.role, kb.ast.cell(kb.ids.green));

    CellI& requestForGet = *new Object(kb, kb.std.ast.Return, "return pixel.get(green)");
    requestForGet.set(kb.ids.value, requestForGetGet);

    CellI& requestForGetAstList = cellTrie.serializeAst(requestForGet);
    {
        std::stringstream ss;
        Visitor::visitList(requestForGetAstList, [&ss](CellI& value, int, bool& stop) {
            ss << value.label() << " ";
        });
        EXPECT_EQ(ss.str(), "struct ast::Return value op push struct ast::Get cell pixel role green op pop ");
    }

    CellI* toolBuilder      = cellTrie.findToolByAst(requestForGet);
    CellI& resultToolAstVar = *new Object(kb, kb.std.ast.Var, "resultToolAstVar");
    cellTrie.createTool(resultToolAstVar, requestForGet, *toolBuilder);
    CellI& resultToolAst = resultToolAstVar[kb.ids.name];

    EXPECT_EQ(&resultToolAst.struct_(), &kb.std.ast.Get);
    EXPECT_EQ(&resultToolAst[kb.ids.cell].struct_(), &kb.std.ast.Cell);
    EXPECT_EQ(&resultToolAst[kb.ids.cell][kb.ids.value], &pixel);
    EXPECT_EQ(&resultToolAst[kb.ids.role].struct_(), &kb.std.ast.Cell);
    EXPECT_EQ(&resultToolAst[kb.ids.role][kb.ids.value], &kb.ids.green);
}

TEST_F(CellTest, CellTrieTestForGetInGet)
{
    brain::CellTrie& cellTrie = *kb.globalScope.m_cellTrie;

    // currentTheme is a test structure to be able to test a nested get. So instead of pixel.get(green) we can replace the "green" node with "currentTheme / std.Color" so we can write
    // pixel.get(currentTheme.get(std.Color)) == 5
    Index currentTheme(kb, "currentTheme");
    currentTheme.set(kb.std.Color, kb.ids.green);

    // test the return get((get(...), y)
    CellI& requestForSetWithGetGetGet = *new Object(kb, kb.std.ast.Get, "currentTheme.get(std.Color)");
    requestForSetWithGetGetGet.set(kb.ids.cell, kb.ast.cell(currentTheme));
    requestForSetWithGetGetGet.set(kb.ids.role, kb.ast.cell(kb.std.Color));

    CellI& requestForSetWithGetGet = *new Object(kb, kb.std.ast.Get, "currentTheme.get(std.Color).get(green)");
    requestForSetWithGetGet.set(kb.ids.cell, requestForSetWithGetGetGet);
    requestForSetWithGetGet.set(kb.ids.role, kb.ast.cell(kb.ids.green));

    CellI& requestForSetWithGet = *new Object(kb, kb.std.ast.Equal, "currentTheme.get(std.Color).get(green) == 5");
    requestForSetWithGet.set(kb.ids.lhs, requestForSetWithGetGet);
    requestForSetWithGet.set(kb.ids.rhs, kb.ast.cell(kb._5_));

    CellI& requestForSetWithGetAstList = cellTrie.serializeAst(requestForSetWithGet);
    {
        std::stringstream ss;
        Visitor::visitList(requestForSetWithGetAstList, [&ss](CellI& value, int, bool& stop) {
            ss << value.label() << " ";
        });
        EXPECT_EQ(ss.str(), "struct ast::Equal lhs op push struct ast::Get cell op push struct ast::Get cell currentTheme role Color op pop role green op pop rhs 5 ");
    }

    CellI* toolBuilder      = cellTrie.findToolByAst(requestForSetWithGet);
    CellI& resultToolAstVar = *new Object(kb, kb.std.ast.Var, "resultToolAstVar");
    cellTrie.createTool(resultToolAstVar, requestForSetWithGet, *toolBuilder);
    CellI& resultToolAst = resultToolAstVar[kb.ids.name];

    EXPECT_EQ(&resultToolAst.struct_(), &kb.std.ast.Set);
    EXPECT_EQ(&resultToolAst[kb.ids.cell].struct_(), &kb.std.ast.Get);
    EXPECT_EQ(&resultToolAst[kb.ids.cell][kb.ids.cell].struct_(), &kb.std.ast.Cell);
    EXPECT_EQ(&resultToolAst[kb.ids.cell][kb.ids.cell][kb.ids.value], &currentTheme);
    EXPECT_EQ(&resultToolAst[kb.ids.cell][kb.ids.role].struct_(), &kb.std.ast.Cell);
    EXPECT_EQ(&resultToolAst[kb.ids.cell][kb.ids.role][kb.ids.value], &kb.std.Color);
    EXPECT_EQ(&resultToolAst[kb.ids.role].struct_(), &kb.std.ast.Cell);
    EXPECT_EQ(&resultToolAst[kb.ids.role][kb.ids.value], &kb.ids.green);
    EXPECT_EQ(&resultToolAst[kb.ids.value].struct_(), &kb.std.ast.Cell);
    EXPECT_EQ(&resultToolAst[kb.ids.value][kb.ids.value], &kb._5_);
    std::cout << "";
}

TEST_F(CellTest, Numbers)
{
    CellI& digit_0 = kb.pools.digits[0];
    CellI& digit_1 = kb.pools.digits[1];
    CellI& digit_2 = kb.pools.digits[2];
    CellI& digit_3 = kb.pools.digits[3];
    CellI& digit_4 = kb.pools.digits[4];
    CellI& digit_5 = kb.pools.digits[5];
    CellI& digit_6 = kb.pools.digits[6];
    CellI& digit_7 = kb.pools.digits[7];
    CellI& digit_8 = kb.pools.digits[8];
    CellI& digit_9 = kb.pools.digits[9];

    CellI& number_123 = toCellNumber(1234567890);
    EXPECT_EQ(&number_123[kb.numbers.sign], &kb.numbers.positive);
    List& number_123_digits = static_cast<List&>(number_123[kb.ids.value]);
    EXPECT_EQ(number_123_digits.size(), 10);
    EXPECT_EQ(&number_123_digits["first"]["value"], &digit_1);
    EXPECT_EQ(&number_123_digits["first"]["next"]["value"], &digit_2);
    EXPECT_EQ(&number_123_digits["first"]["next"]["next"]["value"], &digit_3);
    EXPECT_EQ(&number_123_digits["first"]["next"]["next"]["next"]["value"], &digit_4);
    EXPECT_EQ(&number_123_digits["first"]["next"]["next"]["next"]["next"]["value"], &digit_5);
    EXPECT_EQ(&number_123_digits["first"]["next"]["next"]["next"]["next"]["next"]["value"], &digit_6);
    EXPECT_EQ(&number_123_digits["first"]["next"]["next"]["next"]["next"]["next"]["next"]["value"], &digit_7);
    EXPECT_EQ(&number_123_digits["first"]["next"]["next"]["next"]["next"]["next"]["next"]["next"]["value"], &digit_8);
    EXPECT_EQ(&number_123_digits["first"]["next"]["next"]["next"]["next"]["next"]["next"]["next"]["next"]["value"], &digit_9);
    EXPECT_EQ(&number_123_digits["first"]["next"]["next"]["next"]["next"]["next"]["next"]["next"]["next"]["next"]["value"], &digit_0);

    CellI& number_minus_123 = toCellNumber(-123);
    EXPECT_EQ(&number_minus_123[kb.numbers.sign], &kb.numbers.negative);
    List& number_minus_123_digits = static_cast<List&>(number_123[kb.ids.value]);
    // TODO
}

TEST_F(CellTest, PrintStdCodes)
{
#if 1
    auto& ListItemStruct = getStruct(kb.templateId("std::ListItem", ids.valueType, kb.std.Number));
    auto& ListStruct     = getStruct(kb.templateId("std::List", ids.valueType, kb.std.Number));
    auto& MapStruct      = getStruct(kb.templateId("std::Map", ids.keyType, kb.std.Cell, ids.valueType, kb.std.Slot));
    auto& TrieMapStruct  = getStruct(kb.templateId("std::TrieMap", ids.keyType, kb.std.Number, ids.valueType, kb.std.Color));
    auto& SetStruct      = getStruct(kb.templateId("std::Set", ids.valueType, kb.std.Number));
    auto& IndexStruct    = getStruct("std::Index");
    auto& Struct         = getStruct("std::Struct");

    EXPECT_EQ(&Struct, &kb.std.Struct);

    printAs.value(ListItemStruct);
    printMethodInType(ListItemStruct, "constructor");

    printAs.value(ListStruct);
    printMethodInType(ListStruct, "constructor");
    printMethodInType(ListStruct, "add");
    printMethodInType(ListStruct, "remove");
    printMethodInType(ListStruct, "size");
    printMethodInType(ListStruct, "empty");

    printAs.value(MapStruct);
    printMethodInType(MapStruct, "constructor");
    printMethodInType(MapStruct, "constructorWithIndexType");
    printMethodInType(MapStruct, "add");
    printMethodInType(MapStruct, "empty");
    printMethodInType(MapStruct, "getValue");
    printMethodInType(MapStruct, "hasKey");
    printMethodInType(MapStruct, "remove");
    printMethodInType(MapStruct, "size");

    printAs.value(TrieMapStruct);
    printMethodInType(TrieMapStruct, "constructor");
    printMethodInType(TrieMapStruct, "add");
    printMethodInType(TrieMapStruct, "empty");
    printMethodInType(TrieMapStruct, "getValue");
    printMethodInType(TrieMapStruct, "hasKey");
    printMethodInType(TrieMapStruct, "remove");
    printMethodInType(TrieMapStruct, "size");

    printAs.value(SetStruct);
    printMethodInType(SetStruct, "constructor");
    printMethodInType(SetStruct, "add");
    printMethodInType(SetStruct, "contains");
    printMethodInType(SetStruct, "remove");
    printMethodInType(SetStruct, "size");
    printMethodInType(SetStruct, "first");
    printMethodInType(SetStruct, "empty");

    printAs.value(Struct);
    printMethodInType(Struct, "constructor");
    printMethodInType(Struct, "constructorWithRecursiveType");
    printMethodInType(Struct, "addMembership");
    printMethodInType(Struct, "addSlot");
    printMethodInType(Struct, "addSlots");
    printMethodInType(Struct, "addSubType");
    printMethodInType(Struct, "hasSlot");
    printMethodInType(Struct, "removeSlot");

    printAs.value(IndexStruct);
    printMethodInType(IndexStruct, "constructor");
    printMethodInType(IndexStruct, "constructorWithSelfType");
    printMethodInType(IndexStruct, "insert");
    printMethodInType(IndexStruct, "empty");
    printMethodInType(IndexStruct, "remove");
    printMethodInType(IndexStruct, "size");

#endif
}

TEST_F(CellTest, PrintTestCodes)
{
#if 1
    auto& TestStruct = getStruct("test::TestStruct");
    printMethodInType(TestStruct, "factorial");
    printMethodInType(TestStruct, "testCreateNewListOfNumbers");
#endif
}

TEST_F(CellTest, PrintArcCodes)
{
    auto& VectorStruct = getStruct("arc::Vector");
    auto& ShapeStruct  = getStruct("arc::Shape");
    auto& FrameStruct = getStruct("arc::Frame");

    printMethodInType(VectorStruct, "rotate");

    printAs.value(ShapeStruct);
    printMethodInType(ShapeStruct, "constructor");
    printMethodInType(ShapeStruct, "addPixel");
    printMethodInType(ShapeStruct, "hasPixel");

    printAs.value(FrameStruct);
    printMethodInType(FrameStruct, "constructor");
    printMethodInType(FrameStruct, "processInputPixels");
    printMethodInType(FrameStruct, "process");
    printMethodInType(FrameStruct, "processPixel");
    printMethodInType(FrameStruct, "processAdjacentPixel");
}

TEST_F(CellTest, RecursiveCall)
{
    auto& TestStruct = getStruct("test::TestStruct");
    Object testNumber(kb, TestStruct, "testNumber");
    CellI& TestStructMethodsIndex = testNumber.struct_()[kb.ids.methods][kb.ids.index];
    CellI& factorialMethod        = TestStructMethodsIndex[kb.name("factorial")][kb.ids.value];

    EXPECT_EQ(&testNumber.method(kb.name("factorial"), { ids.input, _0_ }), &_1_);
    EXPECT_EQ(&testNumber.method(kb.name("factorial"), { ids.input, _1_ }), &_1_);
    EXPECT_EQ(&testNumber.method(kb.name("factorial"), { ids.input, _2_ }), &_2_);
    EXPECT_EQ(&testNumber.method(kb.name("factorial"), { ids.input, _3_ }), &_6_);
    EXPECT_EQ(&testNumber.method(kb.name("factorial"), { ids.input, _4_ }), &kb.pools.numbers.get(24));
    EXPECT_EQ(&testNumber.method(kb.name("factorial"), { ids.input, _5_ }), &kb.pools.numbers.get(120));

    int liveCells                 = CellI::s_constructed - CellI::s_destructed;
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(&testNumber.method(kb.name("factorial"), { ids.input, _5_ }), &kb.pools.numbers.get(120));
        testNumber.clearStack(factorialMethod);
    }
    liveCells = CellI::s_constructed - CellI::s_destructed - liveCells;
    EXPECT_EQ(liveCells, 0);
    std::cout << "extra cells: " << liveCells << std::endl;
    std::cout << "";
}

TEST_F(CellTest, List)
{
    auto& ListStruct = getStruct(kb.templateId("std::List", ids.valueType, kb.std.Number));
    Object list(kb, ListStruct, kb.name("constructor"));

    printAs.value(list);
    printAs.cell(list);
    EXPECT_EQ(&list[ids.size], &_0_);
    EXPECT_EQ(&list.method(kb.name("size")), &_0_);
    auto& aaaa = list.method(kb.name("empty"));
    EXPECT_EQ(&list.method(kb.name("empty")), &true_);
    EXPECT_EQ(&list.struct_()[ids.subTypes][kb.ids.index][ids.valueType][kb.ids.value], &kb.std.Number);
    EXPECT_FALSE(list.has(ids.first));
    EXPECT_FALSE(list.has(ids.last));

    list.method(kb.name("add"), { ids.value, _1_ });
    EXPECT_EQ(&list[ids.size], &_1_);
    EXPECT_EQ(&list.method(kb.name("size")), &_1_);
    EXPECT_EQ(&list.method(kb.name("empty")), &false_);
    EXPECT_TRUE(list.has(ids.first));
    EXPECT_TRUE(list.has(ids.last));

    CellI& firstItem = list[ids.first];
    EXPECT_EQ(&firstItem, &list[ids.last]);
    EXPECT_FALSE(firstItem.has(ids.previous));
    EXPECT_FALSE(firstItem.has(ids.next));
    EXPECT_EQ(&firstItem[ids.value], &_1_);
    printAs.value(list);

    list.method(kb.name("add"), { ids.value, kb.pools.numbers.get(2) });
    EXPECT_EQ(&list[ids.size], &_2_);
    EXPECT_EQ(&list.method(kb.name("size")), &_2_);
    EXPECT_EQ(&list.method(kb.name("empty")), &false_);

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

    list.method(kb.name("add"), { ids.value, _3_ });
    EXPECT_EQ(&list[ids.size], &_3_);
    EXPECT_EQ(&list.method(kb.name("size")), &_3_);
    EXPECT_EQ(&list.method(kb.name("empty")), &false_);

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

    list.method(kb.name("remove"), { ids.item, thirdItem });
    EXPECT_EQ(&list[ids.size], &_2_);
    EXPECT_EQ(&list.method(kb.name("size")), &_2_);
    EXPECT_EQ(&list.method(kb.name("empty")), &false_);
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

    list.method(kb.name("remove"), { ids.item, secondItem });
    EXPECT_EQ(&list[ids.size], &_1_);
    EXPECT_EQ(&list.method(kb.name("size")), &_1_);
    EXPECT_EQ(&list.method(kb.name("empty")), &false_);
    {
        CellI& firstItem = list[ids.first];
        EXPECT_EQ(&firstItem, &list[ids.last]);
        EXPECT_EQ(firstItem.has(ids.previous), false);
        EXPECT_EQ(firstItem.has(ids.next), false);
        EXPECT_EQ(&firstItem[ids.value], &_1_);
    }
    printAs.value(list);

    list.method(kb.name("remove"), { ids.item, firstItem });
    EXPECT_EQ(&list[ids.size], &_0_);
    EXPECT_EQ(&list.method(kb.name("size")), &_0_);
    EXPECT_EQ(&list.method(kb.name("empty")), &true_);
    EXPECT_EQ(&list.struct_()[ids.subTypes][kb.ids.index][ids.valueType][kb.ids.value], &kb.std.Number);
    EXPECT_FALSE(list.has(ids.first));
    EXPECT_FALSE(list.has(ids.last));
    printAs.value(list);

    CellI& size = list.method(kb.name("size"));
    printAs.value(size);
}

TEST_F(CellTest, Map)
{
    auto& MapStruct = getStruct(kb.templateId("std::Map", ids.keyType, kb.std.Cell, ids.valueType, kb.std.Slot));
    Object map(kb, MapStruct, kb.name("constructor"));

    printAs.value(map);
    printAs.cell(map);
    EXPECT_EQ(&map[ids.size], &_0_);
    EXPECT_EQ(&map.method(kb.name("size")), &_0_);
    EXPECT_EQ(&map.method(kb.name("empty")), &true_);
    EXPECT_EQ(&map.struct_()[ids.subTypes][kb.ids.index][ids.keyType][kb.ids.value], &kb.std.Cell);
    EXPECT_EQ(&map.struct_()[ids.subTypes][kb.ids.index][ids.valueType][kb.ids.value], &kb.std.Slot);

    map.method(kb.name("add"), { ids.key, _1_ }, { ids.value, kb.ids.red });
    printAs.value(map);
    printAs.cell(map);
    EXPECT_EQ(&map[ids.size], &_1_);
    EXPECT_EQ(&map.method(kb.name("size")), &_1_);
    EXPECT_EQ(&map[ids.list][ids.size], &_1_);
    EXPECT_EQ(&map[ids.list][ids.first][ids.value], &kb.ids.red);
    EXPECT_EQ(&map[ids.list][ids.first], &map[ids.list][ids.last]);
    EXPECT_TRUE(map[ids.index].has(_1_));
    EXPECT_EQ(&map[ids.index][_1_][ids.value], &kb.ids.red);
    EXPECT_EQ(&map.method(kb.name("empty")), &false_);
    EXPECT_TRUE(map[ids.index][ids.struct_][ids.memberOf][ids.index].has(kb.std.Index));

    map.method(kb.name("add"), { ids.key, _2_ }, { ids.value, kb.ids.green });
    map.method(kb.name("add"), { ids.key, _3_ }, { ids.value, kb.ids.blue });
    EXPECT_EQ(&map[ids.index][_1_][ids.value], &kb.ids.red);
    EXPECT_EQ(&map[ids.index][_2_][ids.value], &kb.ids.green);
    EXPECT_EQ(&map[ids.index][_3_][ids.value], &kb.ids.blue);
    printAs.value(map);
    printAs.cell(map);
}

TEST_F(CellTest, MapTypes)
{
    auto& MapStruct = getStruct(kb.templateId("std::Map", ids.keyType, kb.std.Cell, ids.valueType, kb.std.Slot));
    Object map(kb, MapStruct, kb.name("constructor"));

    printAs.value(map.struct_());
    printAs.value(map[ids.list].struct_());
    printAs.value(map[ids.index], "map[ids.index]");
    printAs.value(map[ids.index].struct_(), "map[ids.index].struct_()");
    map.method(kb.name("add"), { ids.key, _1_ }, { ids.value, kb.ids.red });
    printAs.value(map[ids.index], "map[ids.index]");
    printAs.value(map[ids.index].struct_(), "map[ids.index].struct_()");
    printAs.cell(map[ids.index].struct_()[ids.slots], "map[ids.index].struct_()[ids.slots]");
    printAs.cell(map[ids.index].struct_()[ids.slots][ids.index], "map[ids.index].struct_()[ids.slots][ids.index]");
    printAs.value(map[ids.index].struct_()[ids.slots][ids.index], "map[ids.index].struct_()[ids.slots][ids.index]");
}

TEST_F(CellTest, BuiltInType)
{
    std::string testTypeName = "Test1234";
    Struct type(kb, testTypeName);

    EXPECT_EQ(&type[ids.struct_], &kb.std.Struct);
    EXPECT_EQ(&type[ids.name], &kb.name(testTypeName));
    EXPECT_EQ(&type[ids.name], &kb.name(testTypeName)); // query twice to trigger cache
}

TEST_F(CellTest, BuiltInMap)
{
    Map map(kb, kb.std.Number, kb.std.Color);

    printAs.value(map.struct_());
    printAs.value(map[ids.list].struct_());
    printAs.cell(map[ids.index], "map[ids.index]");
    printAs.value(map[ids.index], "map[ids.index]");
    printAs.value(map[ids.index].struct_(), "map[ids.index].struct_()");
    map.add(_1_, kb.ids.red);
    printAs.value(map, "map");
    printAs.cell(map[ids.index], "map[ids.index]");
    printAs.value(map[ids.index], "map[ids.index]");
    printAs.value(map[ids.index].struct_(), "map[ids.index].struct_()");
    printAs.cell(map[ids.index].struct_()[ids.slots], "map[ids.index].struct_()[ids.slots]");
    EXPECT_EQ(&map[ids.index][_1_], &map[ids.list][ids.first]);
    EXPECT_EQ(&map[ids.index].struct_()[ids.slots][ids.size], &_1_);
    EXPECT_EQ(&map[ids.index].struct_()[ids.slots][ids.list][ids.size], &_1_);
    EXPECT_EQ(&map[ids.index].struct_()[ids.slots][ids.index][_1_][ids.value][ids.slotRole], &_1_);
    EXPECT_EQ(&map[ids.index].struct_()[ids.slots][ids.index][_1_][ids.value][ids.slotType], &kb.std.Slot);

    EXPECT_TRUE(map[ids.index].struct_().has(ids.slots));
    EXPECT_TRUE(map[ids.index].struct_()[ids.slots].has(ids.index));
    EXPECT_TRUE(map[ids.index].struct_()[ids.slots][ids.index].has(ids.struct_));
    EXPECT_EQ(&map[ids.index].struct_()[ids.slots][ids.index].struct_(), &map[ids.index].struct_()[ids.slots][ids.index].struct_()[ids.slots][ids.index].struct_());

    EXPECT_TRUE(map[ids.index].struct_()[ids.slots][ids.index].struct_().has(ids.slots));
    printAs.cell(map[ids.index].struct_()[ids.slots], "map[ids.index].struct_()[ids.slots][]");
    printAs.cell(map[ids.index].struct_()[ids.slots][ids.index], "map[ids.index].struct_()[ids.slots][ids.index]");
    printAs.value(map[ids.index].struct_()[ids.slots][ids.index].struct_(), "map[ids.index].struct_()[ids.slots][ids.index].struct_()");
    printAs.value(map[ids.index].struct_()[ids.slots][ids.index].struct_()[ids.slots], "map[ids.index].struct_()[ids.slots][ids.index].struct_()[ids.slots]");
    EXPECT_EQ(&map[ids.index].struct_()[ids.slots][ids.index].struct_()[ids.slots][ids.size], &_1_);
    EXPECT_EQ(&map[ids.index].struct_()[ids.slots][ids.index].struct_()[ids.slots][ids.index][_1_][ids.value][ids.slotRole], &_1_);
    CellI& debug = map[ids.index].struct_()[ids.slots][ids.index].struct_()[ids.slots][ids.index][_1_][ids.value][ids.slotType];
    EXPECT_EQ(&map[ids.index].struct_()[ids.slots][ids.index].struct_()[ids.slots][ids.index][_1_][ids.value][ids.slotType], &kb.std.Slot);
    printAs.value(map[ids.index].struct_()[ids.slots][ids.index].struct_()[ids.slots][ids.index], "map[ids.index].struct_()[ids.slots][ids.index].struct_()[ids.slots][ids.index]");
    printAs.value(map[ids.index].struct_()[ids.slots][ids.index].struct_()[ids.slots][ids.index].struct_(), "map[ids.index].struct_()[ids.slots][ids.index].struct_()[ids.slots][ids.index].struct_()");

    map.remove(_1_);
    printAs.value(map, "map");
}

TEST_F(CellTest, MapTemplateTypes)
{
    auto& MapNumberToColor = getStruct(kb.templateId("std::Map", ids.keyType, kb.std.Number, ids.valueType, kb.std.Color));
    Object map(kb, MapNumberToColor, kb.name("constructor"));

    printAs.value(map.struct_());
    printAs.value(map[ids.list].struct_());
    printAs.cell(map[ids.index], "map[ids.index]");
    printAs.value(map[ids.index], "map[ids.index]");
    printAs.value(map[ids.index].struct_(), "map[ids.index].struct_()");
    map.method(kb.name("add"), { ids.key, _1_ }, { ids.value, kb.ids.red });
    printAs.value(map, "map");
    printAs.cell(map[ids.index], "map[ids.index]");
    printAs.value(map[ids.index], "map[ids.index]");
    printAs.value(map[ids.index].struct_(), "map[ids.index].struct_()");
    printAs.cell(map[ids.index].struct_()[ids.slots], "map[ids.index].struct_()[ids.slots]");
    EXPECT_EQ(&map[ids.index][_1_], &map[ids.list][ids.first]);
    EXPECT_EQ(&map[ids.index].struct_()[ids.slots][ids.size], &_1_);
    EXPECT_EQ(&map[ids.index].struct_()[ids.slots][ids.list][ids.size], &_1_);
    EXPECT_EQ(&map[ids.index].struct_()[ids.slots][ids.index][_1_][ids.value][ids.slotRole], &_1_);
    CellI& debug2 = map[ids.index].struct_()[ids.slots][ids.index][_1_][ids.value][ids.slotType];
    EXPECT_EQ(&map[ids.index].struct_()[ids.slots][ids.index][_1_][ids.value][ids.slotType], &kb.std.Slot);

    EXPECT_TRUE(map[ids.index].struct_().has(ids.slots));
    EXPECT_TRUE(map[ids.index].struct_()[ids.slots].has(ids.index));
    EXPECT_TRUE(map[ids.index].struct_()[ids.slots][ids.index].has(ids.struct_));
    EXPECT_EQ(&map[ids.index].struct_()[ids.slots][ids.index].struct_(), &map[ids.index].struct_()[ids.slots][ids.index].struct_()[ids.slots][ids.index].struct_());

    EXPECT_TRUE(map[ids.index].struct_()[ids.slots][ids.index].struct_().has(ids.slots));
    printAs.cell(map[ids.index].struct_()[ids.slots], "map[ids.index].struct_()[ids.slots][]");
    printAs.cell(map[ids.index].struct_()[ids.slots][ids.index], "map[ids.index].struct_()[ids.slots][ids.index]");
    printAs.value(map[ids.index].struct_()[ids.slots][ids.index].struct_(), "map[ids.index].struct_()[ids.slots][ids.index].struct_()");
    printAs.value(map[ids.index].struct_()[ids.slots][ids.index].struct_()[ids.slots], "map[ids.index].struct_()[ids.slots][ids.index].struct_()[ids.slots]");
    EXPECT_EQ(&map[ids.index].struct_()[ids.slots][ids.index].struct_()[ids.slots][ids.size], &_1_);
    EXPECT_EQ(&map[ids.index].struct_()[ids.slots][ids.index].struct_()[ids.slots][ids.index][_1_][ids.value][ids.slotRole], &_1_);
    CellI& debug = map[ids.index].struct_()[ids.slots][ids.index].struct_()[ids.slots][ids.index][_1_][ids.value][ids.slotType];
    EXPECT_EQ(&map[ids.index].struct_()[ids.slots][ids.index].struct_()[ids.slots][ids.index][_1_][ids.value][ids.slotType], &kb.std.Slot);
    printAs.value(map[ids.index].struct_()[ids.slots][ids.index].struct_()[ids.slots][ids.index], "map[ids.index].struct_()[ids.slots][ids.index].struct_()[ids.slots][ids.index]");
    printAs.value(map[ids.index].struct_()[ids.slots][ids.index].struct_()[ids.slots][ids.index].struct_(), "map[ids.index].struct_()[ids.slots][ids.index].struct_()[ids.slots][ids.index].struct_()");
}

TEST_F(CellTest, MapNumberToColor)
{
    auto& MapNumberToColor = getStruct(kb.templateId("std::Map", ids.keyType, kb.std.Number, ids.valueType, kb.std.Color));
    Object map(kb, MapNumberToColor, kb.name("constructor"));

    EXPECT_EQ(&map.struct_()[ids.subTypes][kb.ids.index][ids.keyType][kb.ids.value], &kb.std.Number);
    EXPECT_EQ(&map.struct_()[ids.subTypes][kb.ids.index][ids.valueType][kb.ids.value], &kb.std.Color);

    printAs.value(map);
    printAs.cell(map);
    EXPECT_EQ(&map[ids.size], &_0_);
    EXPECT_EQ(&map.method(kb.name("size")), &_0_);
    EXPECT_EQ(&map.method(kb.name("empty")), &true_);

    map.method(kb.name("add"), { ids.key, _1_}, { ids.value, kb.ids.red });
    printAs.value(map);
    printAs.cell(map);
    EXPECT_EQ(&map[ids.size], &_1_);
    EXPECT_EQ(&map.method(kb.name("size")), &_1_);
    EXPECT_EQ(&map[ids.list][ids.size], &_1_);
    EXPECT_EQ(&map[ids.list][ids.first][ids.value], &kb.ids.red);
    EXPECT_EQ(&map[ids.list][ids.first], &map[ids.list][ids.last]);
    EXPECT_TRUE(map[ids.index].has(_1_));
    EXPECT_EQ(&map[ids.index][_1_][ids.value], &kb.ids.red);
    EXPECT_EQ(&map.method(kb.name("empty")), &false_);
    EXPECT_TRUE(map[ids.index][ids.struct_][ids.memberOf][ids.index].has(kb.std.Index));

    map.method(kb.name("add"), { ids.key, _2_ }, { ids.value, kb.ids.green });
    map.method(kb.name("add"), { ids.key, _3_ }, { ids.value, kb.ids.blue });
    EXPECT_EQ(&map[ids.index][_1_][ids.value], &kb.ids.red);
    EXPECT_EQ(&map[ids.index][_2_][ids.value], &kb.ids.green);
    EXPECT_EQ(&map[ids.index][_3_][ids.value], &kb.ids.blue);
    printAs.value(map);
    printAs.cell(map);
}

TEST_F(CellTest, ListItem)
{
    auto& ListItemStruct = getStruct(kb.templateId("std::ListItem", ids.valueType, kb.std.Color));
    Object listItem(kb, ListItemStruct, kb.name("constructor"), { ids.value, kb.ids.green });

    EXPECT_EQ(&listItem[ids.value], &kb.ids.green);

    EXPECT_EQ(&listItem.struct_()[ids.memberOf][ids.size], &_1_);
    EXPECT_TRUE(listItem.struct_()[ids.memberOf][ids.index].has(kb.std.ListItem));

    EXPECT_EQ(&listItem.struct_()[ids.slots][ids.size], &_3_);
    EXPECT_TRUE(listItem.struct_()[ids.slots][ids.index].has(ids.previous));
    EXPECT_TRUE(listItem.struct_()[ids.slots][ids.index].has(ids.next));
    EXPECT_TRUE(listItem.struct_()[ids.slots][ids.index].has(ids.value));

    EXPECT_TRUE(listItem.struct_()[ids.methods][ids.index].has(kb.name("constructor")));
}

TEST_F(CellTest, ListItemTemplate)
{
    auto& ListItemNumber  = getStruct(kb.templateId("std::ListItem", ids.valueType, kb.std.Number));
    Object listItemNumber(kb, ListItemNumber, kb.name("constructor"), { ids.value, _1_ });

    EXPECT_EQ(&listItemNumber[ids.value], &_1_);

    printAs.value(kb.std.ListItem, "type.ListItem");
    printAs.value(ListItemNumber, "listItemNumber");
    printAs.value(kb.std.ListItem[ids.slots][ids.list], "type.ListItem[slots]");
    printAs.value(ListItemNumber[ids.slots][ids.list], "listItemNumber[slots]");

    EXPECT_EQ(&ListItemNumber[ids.memberOf][ids.size], &_1_);
    EXPECT_TRUE(ListItemNumber[ids.memberOf][ids.index].has(kb.std.ListItem));

    EXPECT_EQ(&ListItemNumber[ids.slots][ids.size], &_3_);
    EXPECT_TRUE(ListItemNumber[ids.slots][ids.index].has(ids.previous));
    EXPECT_TRUE(ListItemNumber[ids.slots][ids.index].has(ids.next));
    EXPECT_TRUE(ListItemNumber[ids.slots][ids.index].has(ids.value));

    EXPECT_TRUE(ListItemNumber.has(ids.methods));
}

TEST_F(CellTest, ListTemplate)
{
    auto& ListOfNumbers = getStruct(kb.templateId("std::List", ids.valueType, kb.std.Number));

    EXPECT_EQ(&ListOfNumbers[ids.subTypes][ids.size], &_2_);
    EXPECT_TRUE(ListOfNumbers[ids.subTypes][ids.index].has(ids.valueType));
    EXPECT_EQ(&ListOfNumbers[ids.subTypes][ids.index][ids.valueType][ids.value], &kb.std.Number);
    EXPECT_TRUE(ListOfNumbers[ids.subTypes][ids.index].has(ids.itemType));
    CellI& ListItemType = ListOfNumbers[ids.subTypes][ids.index][ids.itemType][ids.value];
    EXPECT_EQ(&ListItemType[ids.slots][ids.index][ids.value][ids.value][ids.slotType], &kb.std.Number);
    EXPECT_NE(&ListItemType, &kb.std.ListItem);
    EXPECT_EQ(&ListItemType[ids.memberOf][ids.size], &_1_);
    EXPECT_TRUE(ListItemType[ids.memberOf][ids.index].has(kb.std.ListItem));

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
    EXPECT_EQ(&ListOfNumbers[ids.slots][ids.index][ids.size][ids.value][ids.slotType], &kb.std.Number);

    EXPECT_TRUE(ListOfNumbers[ids.memberOf][ids.index].has(kb.std.List));
    EXPECT_EQ(&ListOfNumbers[ids.memberOf][ids.size], &_2_);

    printAs.value(kb.std.List, "type.List");
    printAs.value(ListOfNumbers, "ListOfNumbers");
}

TEST_F(CellTest, FunctionTypes)
{
    CellI& function = kb.std.Struct[ids.methods][ids.index][kb.name("addSlot")][ids.value];
    printAs.cell(function.struct_(), "function");
    printAs.cell(function.struct_()[ids.subTypes][ids.index][ids.localVars][ids.value], "function[ids.subTypes][ids.localVars][ids.value]");
    printAs.value(function.struct_()[ids.subTypes][ids.index][ids.localVars][ids.value], "function[ids.subTypes][ids.localVars][ids.value]");
}

TEST_F(CellTest, HybridPicture)
{
    input::Grid inputPicture("input");
    inputPicture.loadFromJsonArray("[[0, 7, 0], [7, 7, 7], [0, 7, 0]]");
    hybrid::Picture picture(kb, inputPicture);

    printAs.svg(picture);
    printAs.svg(picture[ids.pixels]);
    printAs.value(picture[ids.pixels]);

    EXPECT_EQ(&picture[ids.struct_], &kb.std.Grid);
    EXPECT_EQ(&picture[ids.width], &kb.pools.numbers.get(3));
    EXPECT_EQ(&picture[ids.height], &kb.pools.numbers.get(3));
    EXPECT_EQ(&picture[ids.pixels][ids.struct_], &kb.ListOf(kb.std.Pixel));

    auto& ListOfPixels  = getStruct(kb.templateId("std::List", ids.valueType, kb.std.Pixel));
    Object listOfPixels(kb, ListOfPixels, kb.name("constructor"), "listOfPixels");
    listOfPixels.method(kb.name("add"), { ids.value, picture[ids.pixels][ids.first][ids.value] });
    listOfPixels.method(kb.name("add"), { ids.value, picture[ids.pixels][ids.first][ids.next][ids.value] });
    printAs.value(listOfPixels);
}

TEST_F(CellTest, BasicObjectTest)
{
    auto& Type_ = getStruct("std::Struct");
    auto& List = getStruct(kb.templateId("std::List", ids.valueType, kb.std.Cell));
    Object testType(kb, Type_, "Test");
    Object emptyList(kb, List, kb.name("constructor"));

    testType.method(kb.name("addSlots"), { ids.list, emptyList });

    testType.method(kb.name("addSlots"), { ids.list, kb.list(
        kb.std.slot(ids.result, kb.std.Digit),
        kb.std.slot(ids.value, kb.std.Number))}); // TODO implement type checking

    Object object(kb, testType, "testObject");

    EXPECT_EQ(object.label(), "testObject");
    EXPECT_EQ(&object.struct_(), &testType);

    EXPECT_ANY_THROW(&object[ids.value]);
    EXPECT_NO_THROW(object.set(ids.value, kb.pools.numbers.get(42)));
    printAs.value(object[ids.value]);
    EXPECT_ANY_THROW(object.set(ids.argument, kb.pools.numbers.get(42)));
}

TEST_F(CellTest, BasicControlOpTest)
{
    Object testValue1(kb, kb.std.op.ConstVar);
    testValue1.set(ids.value, kb.std.Char);

    Object testValue2(kb, kb.std.op.ConstVar);
    testValue2.set(ids.value, kb.std.Color);

    Object sameOpEq(kb, kb.std.op.Same, "sameOpEq");
    sameOpEq.set(ids.lhs, testValue1);
    sameOpEq.set(ids.rhs, testValue1);

    Object sameOpNe(kb, kb.std.op.Same, "sameOpNe");
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
    Object start(kb, kb.std.op.ConstVar);
    start.set(ids.value, kb.pools.numbers.get(42));

    Object value10(kb, kb.std.op.ConstVar);
    value10.set(ids.value, kb.pools.numbers.get(10));

    Object add10(kb, kb.std.op.Add, "add10");
    add10.set(ids.lhs, start);
    add10.set(ids.rhs, value10);
    add10();
    std::cout << "42 + 10 = ";
    printAs.value(add10[ids.value]);

    EXPECT_EQ(&add10[ids.value], &kb.pools.numbers.get(52));
}

TEST_F(CellTest, CreatingCustomType)
{
    Object colorRed(kb, kb.std.Cell, "red");
    Object colorGreen(kb, kb.std.Cell, "green");
    Object colorBlue(kb, kb.std.Cell, "blue");

    Object colorClass(kb, kb.std.Struct, "Color");
#if 1 // TODO
    auto& MapCellToSlot = getStruct(kb.templateId("std::Map", ids.keyType, kb.std.Cell, ids.valueType, kb.std.Slot));
    Object* slotMapPtr  = new Object(kb, MapCellToSlot, kb.name("constructor"));
    colorClass.set(ids.slots, *slotMapPtr);
#endif

    colorClass.method(kb.name("addSlots"), { ids.list, kb.list(kb.std.slot(colorRed, kb.std.Number), kb.std.slot(colorGreen, kb.std.Number), kb.std.slot(colorBlue, kb.std.Number)) });

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
    printAs.value(colorClass[ids.slots][ids.index][colorRed][ids.struct_], "colorClass slot of colorRed");
    EXPECT_EQ(&colorClass[ids.slots][ids.index][colorRed][ids.value][ids.struct_], &kb.std.Slot);
    EXPECT_EQ(&colorClass[ids.slots][ids.index][colorRed][ids.value][ids.struct_], &kb.std.Slot);
    EXPECT_EQ(&colorClass[ids.slots][ids.index][colorGreen][ids.value][ids.struct_], &kb.std.Slot);
    EXPECT_EQ(&colorClass[ids.slots][ids.index][colorBlue][ids.value][ids.struct_], &kb.std.Slot);
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
    printAs.value(colorClass[ids.slots][ids.index][ids.struct_], "colorClass[ids.slots][ids.index][ids.type]");
    printAs.value(colorClass[ids.slots][ids.index][ids.struct_][ids.slots], "colorClass[ids.slots][ids.index][ids.type][ids.slots]");
    printAs.value(colorClass[ids.slots][ids.index][ids.struct_][ids.slots][ids.index], "colorClass[ids.slots][ids.index][ids.type][ids.slots][ids.index]");
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
    List list(kb, kb.std.Number);
    EXPECT_FALSE(list.has(ids.first));
    EXPECT_FALSE(list.has(ids.last));
    EXPECT_EQ(&list[ids.size], &_0_);
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

    list.remove(&item2);
    EXPECT_TRUE(list.has(ids.first));
    EXPECT_TRUE(list.has(ids.last));
    EXPECT_EQ(&list[ids.size], &_2_);
    EXPECT_FALSE(list.empty());
    printAs.value(list);

    list.remove(&item1);
    EXPECT_TRUE(list.has(ids.first));
    EXPECT_TRUE(list.has(ids.last));
    EXPECT_EQ(&list[ids.size], &_1_);
    EXPECT_FALSE(list.empty());
    printAs.value(list);

    list.remove(&item3);
    EXPECT_FALSE(list.has(ids.first));
    EXPECT_FALSE(list.has(ids.last));
    EXPECT_EQ(&list[ids.size], &_0_);
    EXPECT_TRUE(list.empty());
    printAs.value(list);
}


TEST_F(CellTest, NextgenType)
{
    Map map(kb, kb.std.Number, kb.std.Color);
    map.add(_1_, kb.ids.blue);
    printAs.value(map, "Map<Number, Color>");

    printAs.value(map[ids.index][ids.struct_][ids.slots][ids.list], "map[ids.index][ids.type][ids.slots][ids.list]");
    printAs.value(map[ids.index][ids.struct_][ids.slots][ids.index], "map[ids.index][ids.type][ids.slots][ids.list]");
    printAs.value(map[ids.index][ids.struct_][ids.slots][ids.index][ids.struct_][ids.slots][ids.list], "map[ids.index][ids.type][ids.slots][ids.index][ids.type][ids.slots][ids.list]");
    printAs.value(map[ids.index][ids.struct_][ids.slots][ids.index][ids.struct_][ids.slots][ids.index], "map[ids.index][ids.type][ids.slots][ids.index][ids.type][ids.slots][ids.index]");
    EXPECT_EQ(&map[ids.index][ids.struct_], &map[ids.index][ids.struct_][ids.slots][ids.index][ids.struct_]);

    Index index(kb);
    index.set(kb.std.Number, kb.std.Color);
    printAs.value(index, "Index");

    Struct struct_(kb, "struct");
    struct_.addSlot(_1_, kb.std.slot(_1_, kb.std.Number));
    printAs.value(struct_);

    Set set(kb, kb.std.Number);
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
    auto& MapNumberToColor = getStruct(kb.templateId("std::Map", ids.keyType, kb.std.Number, ids.valueType, kb.std.Color));
    Object map(kb, MapNumberToColor, kb.name("constructor"));

    EXPECT_EQ(&map[ids.size], &_0_);
    EXPECT_EQ(&map.method(kb.name("size")), &_0_);
    EXPECT_EQ(&map.method(kb.name("empty")), &true_);
    printAs.value(map.struct_());
    printAs.value(map[ids.list].struct_());
    printAs.value(map[ids.index], "map[ids.index]");
    printAs.value(map[ids.index].struct_(), "map[ids.index].struct_()");

    map.method(kb.name("add"), { ids.key, _1_ }, { ids.value, kb.ids.red });
    printAs.value(map[ids.index], "map[ids.index]");
    printAs.value(map[ids.index].struct_(), "map[ids.index].struct_()");
    printAs.cell(map[ids.index].struct_()[ids.slots], "map[ids.index].struct_()[ids.slots]");
    printAs.cell(map[ids.index].struct_()[ids.slots][ids.index], "map[ids.index].struct_()[ids.slots][ids.index]");
    printAs.value(map[ids.index].struct_()[ids.slots][ids.index], "map[ids.index].struct_()[ids.slots][ids.index]");
    EXPECT_EQ(&map[ids.size], &_1_);
    EXPECT_EQ(&map.method(kb.name("size")), &_1_);
    EXPECT_EQ(&map[ids.list][ids.size], &_1_);
    EXPECT_EQ(&map[ids.list][ids.first][ids.value], &kb.ids.red);
    EXPECT_EQ(&map[ids.list][ids.first], &map[ids.list][ids.last]);
    EXPECT_TRUE(map[ids.index].has(_1_));
    EXPECT_EQ(&map[ids.index][_1_][ids.value], &kb.ids.red);
    EXPECT_EQ(&map.method(kb.name("empty")), &false_);
    EXPECT_TRUE(map[ids.index][ids.struct_][ids.memberOf][ids.index].has(kb.std.Index));

    map.method(kb.name("add"), { ids.key, _2_ }, { ids.value, kb.ids.green });
    map.method(kb.name("add"), { ids.key, _3_ }, { ids.value, kb.ids.blue });
    EXPECT_EQ(&map[ids.index][_1_][ids.value], &kb.ids.red);
    EXPECT_EQ(&map[ids.index][_2_][ids.value], &kb.ids.green);
    EXPECT_EQ(&map[ids.index][_3_][ids.value], &kb.ids.blue);
    printAs.value(map);
    printAs.cell(map);

    map.method(kb.name("remove"), { ids.key, _3_ });
    map.method(kb.name("remove"), { ids.key, _2_ });

    EXPECT_EQ(&map[ids.size], &_1_);
    EXPECT_EQ(&map.method(kb.name("size")), &_1_);
    EXPECT_EQ(&map[ids.list][ids.size], &_1_);
    EXPECT_EQ(&map[ids.list][ids.first][ids.value], &kb.ids.red);
    EXPECT_EQ(&map[ids.list][ids.first], &map[ids.list][ids.last]);
    EXPECT_TRUE(map[ids.index].has(_1_));
    EXPECT_EQ(&map[ids.index][_1_][ids.value], &kb.ids.red);
    EXPECT_EQ(&map.method(kb.name("empty")), &false_);
    EXPECT_TRUE(map[ids.index][ids.struct_][ids.memberOf][ids.index].has(kb.std.Index));
}


TEST_F(CellTest, TrieMap)
{
    auto& MapNumberToColor = getStruct(kb.templateId("std::TrieMap", ids.keyType, kb.std.Number, ids.valueType, kb.std.Color));
    Object trieMap(kb, MapNumberToColor, kb.name("constructor"));

    EXPECT_EQ(&trieMap[ids.size], &_0_);
    EXPECT_EQ(&trieMap.method(kb.name("size")), &_0_);
    EXPECT_EQ(&trieMap.method(kb.name("empty")), &true_);
    printAs.value(trieMap.struct_());
    printAs.value(trieMap[ids.list].struct_());

    EXPECT_EQ(&trieMap.method(kb.name("hasKey"), { ids.key, kb.list(_1_, _1_, _1_, _1_) }), &false_);
    trieMap.method(kb.name("add"), { ids.key, kb.list(_1_, _1_, _1_, _1_) }, { ids.value, kb.ids.red });
    EXPECT_EQ(&trieMap[ids.size], &_1_);
    EXPECT_EQ(&trieMap.method(kb.name("size")), &_1_);
    EXPECT_EQ(&trieMap[ids.list][ids.size], &_1_);
    EXPECT_EQ(&trieMap.method(kb.name("empty")), &false_);
    CellI& test = trieMap.method(kb.name("hasKey"), { ids.key, kb.list(_1_, _1_, _1_, _1_) });
    EXPECT_EQ(&trieMap.method(kb.name("hasKey"), { ids.key, kb.list(_1_, _1_, _1_, _1_) }), &true_);
    EXPECT_EQ(&trieMap.method(kb.name("getValue"), { ids.key, kb.list(_1_, _1_, _1_, _1_) }), &kb.ids.red);

    EXPECT_EQ(&trieMap.method(kb.name("hasKey"), { ids.key, kb.list(_2_, _2_, _2_, _2_) }), &false_);
    trieMap.method(kb.name("add"), { ids.key, kb.list(_2_, _2_, _2_, _2_) }, { ids.value, kb.ids.green });
    EXPECT_EQ(&trieMap.method(kb.name("hasKey"), { ids.key, kb.list(_2_, _2_, _2_, _2_) }), &true_);

    trieMap.method(kb.name("add"), { ids.key, kb.list(_3_, _3_, _3_, _3_) }, { ids.value, kb.ids.blue });
    EXPECT_EQ(&trieMap.method(kb.name("hasKey"), { ids.key, kb.list(_3_, _3_, _3_, _3_) }), &true_);
    printAs.value(trieMap);
    printAs.cell(trieMap);

    trieMap.method(kb.name("remove"), { ids.key, kb.list(_3_, _3_, _3_, _3_) });
    EXPECT_EQ(&trieMap.method(kb.name("hasKey"), { ids.key, kb.list(_1_, _1_, _1_, _1_) }), &true_);
    EXPECT_EQ(&trieMap.method(kb.name("hasKey"), { ids.key, kb.list(_2_, _2_, _2_, _2_) }), &true_);
    EXPECT_EQ(&trieMap.method(kb.name("hasKey"), { ids.key, kb.list(_3_, _3_, _3_, _3_) }), &false_);
    trieMap.method(kb.name("remove"), { ids.key, kb.list(_2_, _2_, _2_, _2_) });
    EXPECT_EQ(&trieMap.method(kb.name("hasKey"), { ids.key, kb.list(_1_, _1_, _1_, _1_) }), &true_);
    EXPECT_EQ(&trieMap.method(kb.name("hasKey"), { ids.key, kb.list(_2_, _2_, _2_, _2_) }), &false_);
    EXPECT_EQ(&trieMap.method(kb.name("hasKey"), { ids.key, kb.list(_3_, _3_, _3_, _3_) }), &false_);

    EXPECT_EQ(&trieMap[ids.size], &_1_);
    EXPECT_EQ(&trieMap.method(kb.name("size")), &_1_);
    EXPECT_EQ(&trieMap[ids.list][ids.size], &_1_);
    EXPECT_EQ(&trieMap.method(kb.name("empty")), &false_);

    trieMap.method(kb.name("remove"), { ids.key, kb.list(_1_, _1_, _1_, _1_) });

    EXPECT_EQ(&trieMap[ids.size], &_0_);
    EXPECT_EQ(&trieMap.method(kb.name("size")), &_0_);
    EXPECT_EQ(&trieMap[ids.list][ids.size], &_0_);
    EXPECT_EQ(&trieMap.method(kb.name("empty")), &true_);
}

TEST_F(CellTest, Set)
{
    auto& SetOfNumbers = getStruct(kb.templateId("std::Set", ids.valueType, kb.std.Number));
    Object set(kb, SetOfNumbers, kb.name("constructor"));

    EXPECT_EQ(&set[ids.size], &_0_);
    EXPECT_EQ(&set.method(kb.name("size")), &_0_);
    EXPECT_EQ(&set.method(kb.name("empty")), &true_);
    printAs.value(set.struct_());
    printAs.value(set[ids.index], "map[ids.index]");
    printAs.value(set[ids.index].struct_(), "map[ids.index].struct_()");

    set.method(kb.name("add"), { ids.value, _1_ });
    printAs.value(set[ids.index], "map[ids.index]");
    printAs.value(set[ids.index].struct_(), "map[ids.index].struct_()");
    printAs.cell(set[ids.index].struct_()[ids.slots], "map[ids.index].struct_()[ids.slots]");
    printAs.cell(set[ids.index].struct_()[ids.slots][ids.index], "map[ids.index].struct_()[ids.slots][ids.index]");
    printAs.value(set[ids.index].struct_()[ids.slots][ids.index], "map[ids.index].struct_()[ids.slots][ids.index]");
    EXPECT_EQ(&set[ids.size], &_1_);
    EXPECT_EQ(&set.method(kb.name("size")), &_1_);
    EXPECT_TRUE(set[ids.index].has(_1_));
    EXPECT_EQ(&set[ids.index][_1_], &_1_);
    EXPECT_EQ(&set.method(kb.name("empty")), &false_);
    EXPECT_EQ(&set.method(kb.name("contains"), { "value", _1_ }), &true_);
    EXPECT_TRUE(set[ids.index][ids.struct_][ids.memberOf][ids.index].has(kb.std.Index));

    set.method(kb.name("add"), { ids.value, _2_ });
    set.method(kb.name("add"), { ids.value, _3_ });
    EXPECT_EQ(&set[ids.index][_1_], &_1_);
    EXPECT_EQ(&set[ids.index][_2_], &_2_);
    EXPECT_EQ(&set[ids.index][_3_], &_3_);
    printAs.value(set);
    printAs.cell(set);

    set.method(kb.name("remove"), { ids.value, _3_ });
    set.method(kb.name("remove"), { ids.value, _2_ });

    EXPECT_EQ(&set[ids.size], &_1_);
    EXPECT_EQ(&set.method(kb.name("size")), &_1_);
    EXPECT_TRUE(set[ids.index].has(_1_));
    EXPECT_EQ(&set[ids.index][_1_], &_1_);
    EXPECT_EQ(&set.method(kb.name("empty")), &false_);
    EXPECT_TRUE(set[ids.index][ids.struct_][ids.memberOf][ids.index].has(kb.std.Index));
}

////////////
// Proof of concept trie class
class Trie
{
    struct Node
    {
        Node() = default;
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

Trie::Node::~Node()
{
    for (auto& pair : m_children) {
        delete pair.second;
    }
}

Trie::Trie()
{
    m_root = std::make_unique<Node>();
}

bool Trie::empty()
{
    return m_root->m_children.empty();
}

void Trie::insert(const std::string& word)
{
    Node* currentNode = m_root.get();
    char lastChar     = 0;

    for (char ch : word) {
        Node*& children = currentNode->m_children[ch];
        if (children == nullptr) {
            children = new Node();
        }
        currentNode = children;
        lastChar    = ch;
    }
    // At the end of the word, mark this node as the leaf node
    currentNode->m_isLeaf = 1;
    currentNode->m_data   = lastChar;
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
    // FIXME: this is a terrible and buggy print implementation! But actually it is not used in real code base this is just a proof of concept something don't build on this algo
    if (empty())
        return;
    printCb(m_root.get());
}

void Trie::printCb(Node* node)
{
    if (node->m_data) {
        printf("!");
    }

    for (auto& it : node->m_children) {
        printf("%c -> ", it.first);
        printCb(it.second);
    }
}

void print_search_new(Trie& trie, const std::string& word)
{
    std::cout << fmt::format("Searching for {}:", word);
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
    TrieMap trieMap(kb, kb.std.Number, kb.std.Number, "testTrieMap");
    EXPECT_EQ(&trieMap[ids.size], &_0_);
    auto& key1   = kb.list(_0_, _1_, _2_, _3_, _4_);
    auto& value1 = kb.directions.down;
    trieMap.add(key1, value1);
    EXPECT_EQ(&trieMap[ids.size], &_1_);
    EXPECT_TRUE(trieMap.hasKey(key1));
    EXPECT_EQ(&trieMap.getValue(key1), &value1);
    trieMap.remove(key1);
    EXPECT_EQ(&trieMap[ids.size], &_0_);
    EXPECT_FALSE(trieMap.hasKey(key1));
}

///
////////////
// Proof of concept trie class for searching tool usage
class CellTrie
{
    struct FindToolStackNode
    {
        CellI& astCell;
        CellI& slotItem;
    };

    struct Node
    {
        ~Node();

        CellI* m_data = 0;
        std::map<CellI*, Node*> m_children;
        bool m_isLeaf = false;
    };

public:
    CellTrie(brain::Brain& kb);
    bool empty();
    CellI& serializeAst(CellI& ast);
    void add(CellI& ast, CellI& tool);
    CellI* findToolByAst(CellI& ast);
    CellI* findToolByList(CellI& list);
    void print();
    void createTool(CellI& var, CellI& ast, CellI& toolDesc);

private:
    void addValue(Node*& node, CellI& value);
    bool checkValue(CellI*& astCellPtr, CellI*& slotItemPtr, bool& first, Node*& node, std::stack<FindToolStackNode>& stack, CellI& role, CellI& value);
    void handleStep(CellI*& astCellPtr, CellI*& slotItemPtr, Node*& node, std::stack<FindToolStackNode>& stack);
    void printCb(Node* node);

    brain::Brain& kb;
    std::unique_ptr<Node> m_root;
};

CellTrie::Node::~Node()
{
    for (auto& pair : m_children) {
        delete pair.second;
    }
}

CellTrie::CellTrie(brain::Brain& kb) :
    kb(kb)
{
    m_root = std::make_unique<Node>();
}

bool CellTrie::empty()
{
    return m_root->m_children.empty();
}

CellI& CellTrie::serializeAst(CellI& ast)
{
    CellI& slotList    = ast.struct_()[kb.ids.slots][kb.ids.list];
    CellI* slotItemPtr = slotList.has(kb.ids.first) ? &slotList[kb.ids.first] : nullptr;
    List& ret          = *new List(kb, kb.std.Cell);
    ret.label(ast.label());
    struct Context
    {
        CellI& ast;
        CellI& slotItem;
    };
    std::stack<Context> stack;
    bool first        = true;
    CellI* currentPtr = &ast;
    while (slotItemPtr) {
        CellI& slotItem = *slotItemPtr;
        CellI& slot     = slotItem[kb.ids.value];
        CellI& role     = slot[kb.ids.slotRole];
        CellI& current  = *currentPtr;

        if (first) {
            first = false;
            ret.add(kb.ids.struct_);
            ret.add(current.struct_());
        }

        if (current.has(role)) {
            CellI& value = current[role];
            ret.add(role);
            if (&role == &kb.ids.struct_) {
                ret.add(value);
                if (&role == &kb.ids.op) {
                    ret.add(value);
                }
            } else if (&value.struct_() == &kb.std.ast.Cell) {
                ret.add(value[kb.ids.value]);
                if (&value[kb.ids.value] == &kb.ids.op) {
                    ret.add(value[kb.ids.value]);
                }
            } else if (&value.struct_() == &kb.std.ast.Var) {
                ret.add(kb.ids.op);
                ret.add(kb.ids.variable);
            } else if ((&role != &kb.ids.struct_) && (&value.struct_() == &kb.std.ast.Get || &value.struct_() == &kb.std.ast.Return)) { // TODO should be some kind of std.ast.Base check here
                ret.add(kb.ids.op);
                ret.add(kb.ids.push);
                stack.push({ current, *slotItemPtr });
                first       = true;
                currentPtr  = &value;
                slotItemPtr = &value.struct_()[kb.ids.slots][kb.ids.list][kb.ids.first];
                continue;
            }
        }

        slotItemPtr = slotItem.has(kb.ids.next) ? &slotItem[kb.ids.next] : nullptr;
        if (!slotItemPtr && !stack.empty()) {
            slotItemPtr = &stack.top().slotItem;
            currentPtr  = &stack.top().ast;
            stack.pop();
            ret.add(kb.ids.op);
            ret.add(kb.ids.pop);
            slotItemPtr = (*slotItemPtr).has(kb.ids.next) ? &(*slotItemPtr)[kb.ids.next] : nullptr;
        }
    }

    return ret;
}
void CellTrie::addValue(Node*& node, CellI& value)
{
    Node*& roleNode = node->m_children[&value];
    if (roleNode == nullptr) {
        roleNode = new Node();
    }
    node = roleNode;
}

void CellTrie::add(CellI& ast, CellI& tool)
{
    struct Context
    {
        CellI& ast;
        CellI& slotItem;
    };
    std::stack<Context> stack;

    Node* currentNode  = m_root.get();
    CellI& slotList    = ast.struct_()[kb.ids.slots][kb.ids.list];
    CellI* slotItemPtr = slotList.has(kb.ids.first) ? &slotList[kb.ids.first] : nullptr;
    bool first         = true;
    CellI* currentPtr  = &ast;

    while (slotItemPtr) {
        CellI& slotItem = *slotItemPtr;
        CellI& slot     = slotItem[kb.ids.value];
        CellI& role     = slot[kb.ids.slotRole];
        CellI& current  = *currentPtr;

        if (first) {
            first = false;
            addValue(currentNode, kb.ids.struct_);
            addValue(currentNode, current.struct_());
        }

        if (current.has(role)) {
            CellI& value = current[role];
            addValue(currentNode, role);
            if (&value.struct_() == &kb.std.ast.Cell) {
                addValue(currentNode, value[kb.ids.value]);
                if (&value[kb.ids.value] == &kb.ids.op) {
                    addValue(currentNode, value[kb.ids.value]);
                }
            } else if (&value.struct_() == &kb.std.ast.Var) {
                addValue(currentNode, kb.ids.op);
                addValue(currentNode, kb.ids.variable);
            } else if ((&role != &kb.ids.struct_) && (&value.struct_() == &kb.std.ast.Get || &value.struct_() == &kb.std.ast.Return)) { // TODO should be some kind of std.ast.Base check here
                addValue(currentNode, kb.ids.op);
                addValue(currentNode, kb.ids.push);
                stack.push({ current, *slotItemPtr });
                first       = true;
                currentPtr  = &value;
                slotItemPtr = &value.struct_()[kb.ids.slots][kb.ids.list][kb.ids.first];
                continue;
            }
        }

        slotItemPtr = slotItem.has(kb.ids.next) ? &slotItem[kb.ids.next] : nullptr;
        if (!slotItemPtr && !stack.empty()) {
            currentPtr  = &stack.top().ast;
            slotItemPtr = &stack.top().slotItem;
            stack.pop();
            addValue(currentNode, kb.ids.op);
            addValue(currentNode, kb.ids.pop);
            slotItemPtr = (*slotItemPtr).has(kb.ids.next) ? &(*slotItemPtr)[kb.ids.next] : nullptr;
        }
    }

    // At the end of the word, mark this node as the leaf node
    currentNode->m_isLeaf = 1;
    currentNode->m_data   = &tool;
}

void CellTrie::handleStep(CellI*& astCellPtr, CellI*& slotItemPtr, Node*& node, std::stack<FindToolStackNode>& stack)
{
    slotItemPtr = (*slotItemPtr).has(kb.ids.next) ? &(*slotItemPtr)[kb.ids.next] : nullptr;
    while (!slotItemPtr) {
        if (stack.empty()) {
            return;
        }

        auto opFindIt = node->m_children.find(&kb.ids.op);
        if (opFindIt == node->m_children.end()) {
            return;
        }
        Node* opNode   = opFindIt->second;
        auto popFindIt = opNode->m_children.find(&kb.ids.pop);
        if (popFindIt == opNode->m_children.end()) {
            return;
        }
        node = popFindIt->second;

        slotItemPtr = &stack.top().slotItem;
        astCellPtr  = &stack.top().astCell;
        stack.pop();
        slotItemPtr = (*slotItemPtr).has(kb.ids.next) ? &(*slotItemPtr)[kb.ids.next] : nullptr;
    }
}

bool CellTrie::checkValue(CellI*& astCellPtr, CellI*& slotItemPtr, bool& first, Node*& node, std::stack<FindToolStackNode>& stack, CellI& role, CellI& value)
{
    auto roleFindIt = node->m_children.find(&role);
    if (roleFindIt == node->m_children.end()) {
        node = nullptr;
        return false;
    } else {
        node = roleFindIt->second;
    }

    auto findIt = node->m_children.find(&value);
    if (findIt == node->m_children.end()) {
        auto opFindIt = node->m_children.find(&kb.ids.op);
        if (opFindIt == node->m_children.end()) {
            node = nullptr;
            return false;
        } else {
            // ok, so value not found but we have an op here
            Node* opNode = opFindIt->second;

            for (auto& [key, nextNode] : opNode->m_children) {
                if (key == &kb.ids.variable) {
                    node = nextNode;
                    handleStep(astCellPtr, slotItemPtr, node, stack);
                    return true;
                }
                if (key == &kb.ids.push) {
                    stack.push({ .astCell = *astCellPtr, .slotItem = *slotItemPtr });
                    astCellPtr  = &(*astCellPtr)[role];
                    slotItemPtr = &value.struct_()[kb.ids.slots][kb.ids.list][kb.ids.first];
                    node        = nextNode;
                    first       = true;
                    return true;
                }
                if (key == &kb.ids.pop) {
                    if (stack.empty()) {
                        return false;
                    }
                    slotItemPtr = &stack.top().slotItem;
                    astCellPtr  = &stack.top().astCell;
                    stack.pop();
                    slotItemPtr = (*slotItemPtr).has(kb.ids.next) ? &(*slotItemPtr)[kb.ids.next] : nullptr;
                    node        = nextNode;
                    if (!slotItemPtr) {
                        handleStep(astCellPtr, slotItemPtr, node, stack);
                    }
                    return true;
                }
            }
        }
    } else {
        node = findIt->second;
    }
    // the first slot is the kb.ids.struct_ but it is not in the slot list
    if (first) {
        first = false;
        return true;
    }
    handleStep(astCellPtr, slotItemPtr, node, stack);

    return true;
}

CellI* CellTrie::findToolByAst(CellI& ast)
{
    std::stack<FindToolStackNode> stack;

    Node* currentNode  = m_root.get();
    CellI& slotList    = ast.struct_()[kb.ids.slots][kb.ids.list];
    CellI* slotItemPtr = slotList.has(kb.ids.first) ? &slotList[kb.ids.first] : nullptr;
    bool first         = true;
    CellI* astCellPtr  = &ast;

    while (slotItemPtr) {
        CellI& role = (*slotItemPtr)[kb.ids.value][kb.ids.slotRole];

        if (first && !checkValue(astCellPtr, slotItemPtr, first, currentNode, stack, kb.ids.struct_, (*astCellPtr).struct_())) {
            return nullptr;
        }

        if ((*astCellPtr).has(role) && !checkValue(astCellPtr, slotItemPtr, first, currentNode, stack, role, (*astCellPtr)[role])) {
            return nullptr;
        }
    }

    if (currentNode && currentNode->m_isLeaf == 1)
        return currentNode->m_data;

    return nullptr;
}

CellI* CellTrie::findToolByList(CellI& list)
{
    brain::Brain& kb  = this->kb;
    Node* currentNode = m_root.get();

    for (CellI* currentListItemPtr = &list[kb.ids.first];;) {
        CellI& value = (*currentListItemPtr)[kb.ids.value];

        if (&value == &kb.ids.op) {
            CellI* nextListItemPtr = &(*currentListItemPtr)[kb.ids.next];
            CellI& nextValue       = (*nextListItemPtr)[kb.ids.value];
            if (&nextValue == &kb.ids.variable) {
            } else if (&nextValue == &kb.ids.push) {
            } else if (&nextValue == &kb.ids.pop) {
            }
        }
        auto chFindIt = currentNode->m_children.find(&value);
        if (chFindIt == currentNode->m_children.end()) {
            auto opFindIt = currentNode->m_children.find(&kb.ids.op);
            if (opFindIt == currentNode->m_children.end()) {
                currentNode = nullptr;
                break;
            } else {
                // ok, so value not found but we have an op here
                Node* opNode   = opFindIt->second;
                auto varFindIt = opNode->m_children.find(&kb.ids.variable);
                if (varFindIt == opNode->m_children.end()) {
                    currentNode = nullptr;
                    break;
                }
                currentNode = varFindIt->second;
            }
        } else {
            currentNode = chFindIt->second;
        }

        if ((*currentListItemPtr).missing(kb.ids.next)) {
            break;
        }
        currentListItemPtr = &(*currentListItemPtr)[kb.ids.next];
    }

    if (currentNode && currentNode->m_isLeaf == 1)
        return currentNode->m_data;

    return nullptr;
}

void CellTrie::createTool(CellI& var, CellI& inputAst, CellI& inputToolDesc)
{
    auto& ListOfCellStruct = kb.getStruct(kb.templateId("std::List", kb.ids.valueType, kb.std.Cell));

    brain::Brain& kb = this->kb;
    List& toCreate   = *new List(kb, kb.std.Cell);
    Index& toCreateItemRoot = *new Index(kb);
    toCreateItemRoot.set(kb.ids.ast, inputAst);
    toCreateItemRoot.set(kb.ids.description, inputToolDesc);
    toCreateItemRoot.set(kb.ids.cell, var);
    toCreateItemRoot.set(kb.ids.role, kb.ids.name);

    toCreate.add(toCreateItemRoot);
    CellI* toCreateItemPtr = &toCreate[kb.ids.first];
    while (toCreateItemPtr) {
        CellI& toCreateItem = (*toCreateItemPtr)[kb.ids.value];
        CellI& ast          = toCreateItem[kb.ids.ast];
        CellI& toolDesc     = toCreateItem[kb.ids.description];
        CellI* ret          = &toCreateItem[kb.ids.cell];
        CellI& retKey       = toCreateItem[kb.ids.role];

        CellI* slotItemPtr = &toolDesc[kb.ids.first];
        bool first         = true;
        List& subTools     = *new List(kb, kb.std.Cell);
        while (slotItemPtr) {
            CellI& key = (*slotItemPtr)[kb.ids.value];

            if (first) {
                if (&key.struct_() != &kb.std.ast.Cell && (&key[kb.ids.value] != &kb.ids.struct_)) {
                    throw "Tool description without type!";
                }
                first               = false;
                CellI& nextSlotItem = (*slotItemPtr)[kb.ids.next];
                CellI& valueCell    = nextSlotItem[kb.ids.value];
                if (&valueCell.struct_() != &kb.std.ast.Cell) {
                    throw "Tool description type is not constant value!";
                }
                CellI& type = valueCell[kb.ids.value];
                CellI* newObj = new Object(kb, type, toolDesc.label());
                ret->set(retKey, *newObj);
                ret = newObj;

                slotItemPtr = &nextSlotItem;
            } else if (&key.struct_() == &kb.std.ast.Cell) {
                CellI& role         = key[kb.ids.value];
                CellI& nextSlotItem = (*slotItemPtr)[kb.ids.next];
                CellI& valueCell    = nextSlotItem[kb.ids.value];
                CellI* valuePtr     = nullptr;
                if (&valueCell.struct_() == &kb.std.ast.Cell) {
                    valuePtr = &ast[role];
                    ret->set(role, *valuePtr);
                } else if (&valueCell.struct_() == &ListOfCellStruct) {
                    valuePtr = &ast;
                    Visitor::visitList(valueCell, [&valuePtr, &kb](CellI& pathItem, int, bool& stop) {
                        CellI& currentValue = *valuePtr;
                        valuePtr            = &currentValue[pathItem];
                    });
                    ret->set(role, *valuePtr);
                } else {
                    throw "Tool description value is not a constant value or List!";
                }
                if (&(*valuePtr).struct_() != &kb.std.ast.Cell) {
                    static CellI& retVal = *new Object(kb, kb.std.ast.Return);
                    retVal.set(kb.ids.value, *valuePtr);
                    subTools.add(kb.ast.slot(*ret, role));
                }
                slotItemPtr = &nextSlotItem;
            } else {
                throw "Tool description role is not constant value!";
            }

            slotItemPtr = (*slotItemPtr).has(kb.ids.next) ? &(*slotItemPtr)[kb.ids.next] : nullptr;
        }
        CellI* subpToolItemPtr = &subTools[kb.ids.first];
        static CellI& retVal   = *new Object(kb, kb.std.ast.Return);
        while (subpToolItemPtr) {
            CellI& slot       = (*subpToolItemPtr)[kb.ids.value];
            CellI& key        = slot[kb.ids.slotRole];
            CellI& value      = slot[kb.ids.slotType];
            CellI& subToolAst = key[value];

            retVal.set(kb.ids.value, subToolAst);

            CellI* subToolDesc = findToolByAst(retVal);

            if (!subToolDesc) {
                throw "Sub tool not found!";
            }
            Index& toCreateItemSub = *new Index(kb);
            toCreateItemSub.set(kb.ids.ast, subToolAst);
            toCreateItemSub.set(kb.ids.description, *subToolDesc);
            toCreateItemSub.set(kb.ids.cell, (*ret));
            toCreateItemSub.set(kb.ids.role, value);
            toCreate.add(toCreateItemSub);

            CellI* toDelete = subpToolItemPtr;
            subpToolItemPtr = (*subpToolItemPtr).has(kb.ids.next) ? &(*subpToolItemPtr)[kb.ids.next] : nullptr;
            subTools.remove((List::Item*)toDelete);
        }
        CellI* toDelete = toCreateItemPtr;
        toCreateItemPtr = (*toCreateItemPtr).has(kb.ids.next) ? &(*toCreateItemPtr)[kb.ids.next] : nullptr;
        toCreate.remove((List::Item*)toDelete);
    }
}

void CellTrie::print()
{
    if (empty())
        return;
    printCb(m_root.get());
}

void CellTrie::printCb(Node* node)
{
    printf("%s -> ", node->m_data->label().c_str());

    for (auto& it : node->m_children) {
        printCb(it.second);
    }
}

void testFindToolByAst(CellTrie& trie, CellI& ast)
{
    std::cout << fmt::format("Searching tool by AST for '{}' ... ", ast.label());
    if (CellI* tool = trie.findToolByAst(ast))
        std::cout << fmt::format("found '{}'!", tool->label());
    else
        std::cout << "Not Found!";
    std::cout << std::endl;
}
void testFindToolByList(CellTrie& trie, CellI& list)
{
    std::cout << fmt::format("Searching tool by list for '{}' ... ", list.label());
    if (CellI* tool = trie.findToolByList(list))
        std::cout << fmt::format("found '{}'!", tool->label());
    else
        std::cout << "Not Found!";
    std::cout << std::endl;
}
////////////

TEST_F(CellTest, CellTrieTest)
{
    CellTrie cellTrie(kb);

    // the op.set(x, y, z) description is op.eq(op.get(x, y), z)
    // here we creating the ast for op.eq(op.get(x, y), z)
    CellI& astVarX = *new Object(kb, kb.std.ast.Var);
    astVarX.set(kb.ids.name, kb.pools.chars.get('X'));

    CellI& astVarY = *new Object(kb, kb.std.ast.Var);
    astVarY.set(kb.ids.name, kb.pools.chars.get('Y'));

    CellI& astVarZ = *new Object(kb, kb.std.ast.Var);
    astVarZ.set(kb.ids.name, kb.pools.chars.get('Z'));

    CellI& setEffectGet = *new Object(kb, kb.std.ast.Get);
    setEffectGet.set(kb.ids.cell, astVarX);
    setEffectGet.set(kb.ids.role, astVarY);

    CellI& setEffect = *new Object(kb, kb.std.ast.Equal);
    setEffect.set(kb.ids.lhs, setEffectGet);
    setEffect.set(kb.ids.rhs, astVarZ);

    CellI& getEffectGet = *new Object(kb, kb.std.ast.Get);
    getEffectGet.set(kb.ids.cell, astVarX);
    getEffectGet.set(kb.ids.role, astVarY);

    CellI& getEffect = *new Object(kb, kb.std.ast.Return);
    getEffect.set(kb.ids.value, getEffectGet);

    // the set(x, y, z) should be recovered from the above structure so we need to serialize the set(...) in a way where the variables can be extracted from the get(...) expression
    List& setBuilder = *new List(kb, kb.std.Cell, "set(x, y, z)");
    setBuilder.add(kb.ast.cell(kb.ids.struct_));
    setBuilder.add(kb.ast.cell(kb.std.ast.Set));

    setBuilder.add(kb.ast.cell(kb.ids.cell));
    setBuilder.add(kb.list(kb.ids.lhs, kb.ids.cell));

    setBuilder.add(kb.ast.cell(kb.ids.role));
    setBuilder.add(kb.list(kb.ids.lhs, kb.ids.role));

    setBuilder.add(kb.ast.cell(kb.ids.value));
    setBuilder.add(kb.list(kb.ids.rhs));

    List& getBuilder = *new List(kb, kb.std.Cell, "return get(x, y)");
    getBuilder.add(kb.ast.cell(kb.ids.struct_));
    getBuilder.add(kb.ast.cell(kb.std.ast.Get));

    getBuilder.add(kb.ast.cell(kb.ids.cell));
    getBuilder.add(kb.ast.cell(kb.ids.cell));

    getBuilder.add(kb.ast.cell(kb.ids.role));
    getBuilder.add(kb.ast.cell(kb.ids.role));

    cellTrie.add(setEffect, setBuilder);
    cellTrie.add(getEffect, getBuilder);

    // here we generating the serialized version of op.eq(op.get(x, y), z) to be able to test
    CellI& setEffectAsList = cellTrie.serializeAst(setEffect);
    {
        std::stringstream ss;
        Visitor::visitList(setEffectAsList, [&ss](CellI& value, int, bool& stop) {
            ss << value.label() << " ";
        });
        EXPECT_EQ(ss.str(), "struct ast::Equal lhs op push struct ast::Get cell op variable role op variable op pop rhs op variable ");
    }

    // here we generating the serialized version of op.eq(op.get(x, y), z) to be able to test
    CellI& getEffectAsList = cellTrie.serializeAst(getEffect);
    {
        std::stringstream ss;
        Visitor::visitList(getEffectAsList, [&ss](CellI& value, int, bool& stop) {
            ss << value.label() << " ";
        });
        EXPECT_EQ(ss.str(), "struct ast::Return value op push struct ast::Get cell op variable role op variable op pop ");
    }

    Object& pixel = *new Object(kb, kb.std.Color, "pixel");
    // currentTheme is a test structure to be able to test a nested get. So instead of pixel.get(green) we can replace the "green" node with "currentTheme / std.Color" so we can write
    // pixel.get(currentTheme.get(std.Color)) == 5
    Index currentTheme(kb, "currentTheme");
    currentTheme.set(kb.std.Color, kb.ids.green);

    // Now we can create a request so I request to satisfy the following pixel.get(green) == 5
    // The expected tool to use is pixel.set(green, 5)
    CellI& requestForSetGet = *new Object(kb, kb.std.ast.Get);
    requestForSetGet.set(kb.ids.cell, kb.ast.cell(pixel));
    requestForSetGet.set(kb.ids.role, kb.ast.cell(kb.ids.green));

    CellI& requestForSet = *new Object(kb, kb.std.ast.Equal, "pixel.get(green) == 5");
    requestForSet.set(kb.ids.lhs, requestForSetGet);
    requestForSet.set(kb.ids.rhs, kb.ast.cell(kb._5_));

    CellI& requestForSetAstList = cellTrie.serializeAst(requestForSet);
    {
        std::stringstream ss;
        Visitor::visitList(requestForSetAstList, [&ss](CellI& value, int, bool& stop) {
            ss << value.label() << " ";
        });
        EXPECT_EQ(ss.str(), "struct ast::Equal lhs op push struct ast::Get cell pixel role green op pop rhs 5 ");
    }

    testFindToolByAst(cellTrie, requestForSet);
    testFindToolByList(cellTrie, requestForSetAstList);
    CellI* resultToolDesc = cellTrie.findToolByList(requestForSetAstList);
    EXPECT_EQ(resultToolDesc, &setBuilder);
    CellI& resultToolAstVar = *new Object(kb, kb.std.ast.Var, "resultToolAstVar");
    cellTrie.createTool(resultToolAstVar, requestForSet, *resultToolDesc);
    CellI& resultToolAst = resultToolAstVar[kb.ids.name];
    EXPECT_EQ(&resultToolAst.struct_(), &kb.std.ast.Set);
    EXPECT_EQ(&resultToolAst[kb.ids.cell].struct_(), &kb.std.ast.Cell);
    EXPECT_EQ(&resultToolAst[kb.ids.cell][kb.ids.value], &pixel);
    EXPECT_EQ(&resultToolAst[kb.ids.role].struct_(), &kb.std.ast.Cell);
    EXPECT_EQ(&resultToolAst[kb.ids.role][kb.ids.value], &kb.ids.green);
    EXPECT_EQ(&resultToolAst[kb.ids.value].struct_(), &kb.std.ast.Cell);
    EXPECT_EQ(&resultToolAst[kb.ids.value][kb.ids.value], &kb._5_);
    // TODO compile the result tp OPs

    // test the return get(x, y)
    CellI& requestForGetGet = *new Object(kb, kb.std.ast.Get);
    requestForGetGet.set(kb.ids.cell, kb.ast.cell(pixel));
    requestForGetGet.set(kb.ids.role, kb.ast.cell(kb.ids.green));

    CellI& requestForGet = *new Object(kb, kb.std.ast.Return, "return pixel.get(green)");
    requestForGet.set(kb.ids.value, requestForGetGet);

    CellI& requestForGetAstList = cellTrie.serializeAst(requestForGet);
    {
        std::stringstream ss;
        Visitor::visitList(requestForGetAstList, [&ss](CellI& value, int, bool& stop) {
            ss << value.label() << " ";
        });
        EXPECT_EQ(ss.str(), "struct ast::Return value op push struct ast::Get cell pixel role green op pop ");
    }

    testFindToolByAst(cellTrie, requestForGet);
    testFindToolByList(cellTrie, requestForGetAstList);

    // test the return get((get(...), y)
    CellI& requestForSetWithGetGetGet = *new Object(kb, kb.std.ast.Get, "get(currentTheme, color)");
    requestForSetWithGetGetGet.set(kb.ids.cell, kb.ast.cell(currentTheme));
    requestForSetWithGetGetGet.set(kb.ids.role, kb.ast.cell(kb.std.Color));

    CellI& requestForSetWithGetGet = *new Object(kb, kb.std.ast.Get, "get(get(currentTheme, color), green)");
    requestForSetWithGetGet.set(kb.ids.cell, requestForSetWithGetGetGet);
    requestForSetWithGetGet.set(kb.ids.role, kb.ast.cell(kb.ids.green));

    CellI& requestForSetWithGet = *new Object(kb, kb.std.ast.Equal, "pixel.get(currentTheme.get(std.Color)) == 5");
    requestForSetWithGet.set(kb.ids.lhs, requestForSetWithGetGet);
    requestForSetWithGet.set(kb.ids.rhs, kb.ast.cell(kb._5_));

    CellI& requestForSetWithGetAstList = cellTrie.serializeAst(requestForSetWithGet);
    {
        std::stringstream ss;
        Visitor::visitList(requestForSetWithGetAstList, [&ss](CellI& value, int, bool& stop) {
            ss << value.label() << " ";
        });
        EXPECT_EQ(ss.str(), "struct ast::Equal lhs op push struct ast::Get cell op push struct ast::Get cell currentTheme role Color op pop role green op pop rhs 5 ");
    }
    CellI* test1 = cellTrie.findToolByAst(requestForSetWithGet);
    CellI* test2 = cellTrie.findToolByList(requestForSetWithGetAstList);
    testFindToolByAst(cellTrie, requestForSetWithGet);
    testFindToolByList(cellTrie, requestForSetWithGetAstList);
    CellI& resultToolForTest1AstVar = *new Object(kb, kb.std.ast.Var, "resultToolForTest1AstVar");
    cellTrie.createTool(resultToolForTest1AstVar, requestForSetWithGet, *test1);
    CellI& resultToolForTest1Ast = resultToolForTest1AstVar[kb.ids.name];
    EXPECT_EQ(&resultToolForTest1Ast.struct_(), &kb.std.ast.Set);
    EXPECT_EQ(&resultToolForTest1Ast[kb.ids.cell].struct_(), &kb.std.ast.Get);
    EXPECT_EQ(&resultToolForTest1Ast[kb.ids.cell][kb.ids.cell].struct_(), &kb.std.ast.Cell);
    EXPECT_EQ(&resultToolForTest1Ast[kb.ids.cell][kb.ids.cell][kb.ids.value], &currentTheme);
    EXPECT_EQ(&resultToolForTest1Ast[kb.ids.cell][kb.ids.role].struct_(), &kb.std.ast.Cell);
    EXPECT_EQ(&resultToolForTest1Ast[kb.ids.cell][kb.ids.role][kb.ids.value], &kb.std.Color);
    EXPECT_EQ(&resultToolForTest1Ast[kb.ids.role].struct_(), &kb.std.ast.Cell);
    EXPECT_EQ(&resultToolForTest1Ast[kb.ids.role][kb.ids.value], &kb.ids.green);
    EXPECT_EQ(&resultToolForTest1Ast[kb.ids.value].struct_(), &kb.std.ast.Cell);
    EXPECT_EQ(&resultToolForTest1Ast[kb.ids.value][kb.ids.value], &kb._5_);
    std::cout << "";
}

TEST_F(CellTest, StringTest)
{
    String& testStr1 = kb.pools.strings.get("test");
    String& testStr2 = kb.pools.strings.get("test");
    EXPECT_EQ(&testStr1, &testStr2);
    EXPECT_EQ(&testStr1.struct_(), &kb.std.String);
    EXPECT_EQ(&testStr1[kb.ids.value].struct_(), &kb.ListOf(kb.std.Char));
    EXPECT_EQ(&testStr1[kb.ids.value], &testStr1[kb.ids.value]);
    EXPECT_EQ(&testStr1[kb.ids.value], &testStr2[kb.ids.value]);
    printAs.value(testStr1[kb.ids.value]);
    printAs.cell(testStr1[kb.ids.value]);
}

static ftxui::Color ftxAlphaColor(255, 255, 255);
static ftxui::Element colorTile(const ftxui::Color& p_color)
{
    if (&p_color == &ftxAlphaColor)
        return ftxui::text(L"╳╳") | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 2) | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 1) | ftxui::color(ftxui::Color::GrayDark) | ftxui::bgcolor(ftxui::Color::GrayLight);
    return ftxui::text("") | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 2) | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 1) | bgcolor(p_color);
}

static ftxui::Element colorTile(arc::Colors arcColor)
{
    if (arcColor == arc::Colors::alpha)
        return ftxui::text(L"╳╳") | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 2) | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 1) | ftxui::color(ftxui::Color::GrayDark) | ftxui::bgcolor(ftxui::Color::GrayLight);
    return colorTile(App::arcColors[(int)arcColor]);
}

static ftxui::Element renderJsonBoard(const nlohmann::json& inputRow)
{
    ftxui::Elements boardLines;
    for (auto inputRowIt = inputRow.begin(); inputRowIt != inputRow.end(); ++inputRowIt) {
        ftxui::Elements arcSetInputLine;
        for (const int val : *inputRowIt) {
            arcSetInputLine.push_back(colorTile((arc::Colors)val));
        }
        boardLines.push_back(hbox(arcSetInputLine));
    }

    return vbox(boardLines);
}

static ftxui::Element renderJsonBoard(cells::hybrid::arc::Grid& grid)
{
    ftxui::Elements boardLines;
    for (int y = 0; y < grid.height(); ++y) {
        ftxui::Elements arcSetInputLine;
        for (int x = 0; x < grid.width(); ++x) {
            auto& pixel = grid.getPixel(x, y);
            const ftxui::Color& ftxColor = infocell::App::arcColors[pixel.color()];
            arcSetInputLine.push_back(colorTile(ftxColor));
        }
        boardLines.push_back(hbox(arcSetInputLine));
    }

    return vbox(boardLines);
}

static void printGrid(cells::hybrid::arc::Grid& grid)
{
    auto document = renderJsonBoard(grid);
    auto screen   = ftxui::Screen::Create(
        ftxui::Dimension::Full(),       // Width
        ftxui::Dimension::Fit(document) // Height
    );
    ftxui::Render(screen, document);
    screen.Print();
}

static ftxui::Element renderShape(CellI& shape)
{
    int width               = static_cast<Number&>(shape["width"]).value();
    int height              = static_cast<Number&>(shape["height"]).value();
    CellI& pixelList        = shape["pixels"];
    CellI* currentPixelItem = &pixelList["first"];
    int shapePixelX         = static_cast<Number&>((*currentPixelItem)["value"]["x"]).value();
    int shapePixelY         = static_cast<Number&>((*currentPixelItem)["value"]["y"]).value();
    int shapeColorNum       = static_cast<Number&>(shape["color"]).value();
    const ftxui::Color& shapeColor = infocell::App::arcColors[shapeColorNum];
    ftxui::Elements boardLines;
    for (int y = 0; y < height; ++y) {
        ftxui::Elements arcSetInputLine;
        for (int x = 0; x < width; ++x) {
            const ftxui::Color* currentColor = &ftxAlphaColor;
            if (currentPixelItem && shapePixelX == x && shapePixelY == y) {
                currentColor = &shapeColor;
                if (currentPixelItem->has("next")) {
                    currentPixelItem = &currentPixelItem->get("next");
                    shapePixelX  = static_cast<Number&>((*currentPixelItem)["value"]["x"]).value();
                    shapePixelY  = static_cast<Number&>((*currentPixelItem)["value"]["y"]).value();
                } else {
                    currentPixelItem = nullptr;
                }
            }
            arcSetInputLine.push_back(colorTile(*currentColor));
        }
        boardLines.push_back(hbox(arcSetInputLine));
    }

    return vbox(boardLines);
}

static void printVectorShape(CellI& shape)
{
    std::map<int, std::map<int, ftxui::Color>> board;
    int shapeColorNum   = static_cast<Number&>(shape["color"]).value();
    const ftxui::Color& shapeColor = infocell::App::arcColors[shapeColorNum];
    int x       = static_cast<Number&>(shape["firstPixel"]["x"]).value();
    int y       = static_cast<Number&>(shape["firstPixel"]["y"]).value();
    board[x][y] = shapeColor;
    int maxX    = x;
    int maxY    = y;
    int minX    = x;
    int minY    = y;

    CellI& vectorList = shape["vectors"];
    Visitor::visitList(vectorList, [&board, &x, &y, &maxX, &maxY, &minX, &minY, shapeColor](CellI& vector, int i, bool&) {
        x += static_cast<Number&>(vector["x"]).value();
        y += static_cast<Number&>(vector["y"]).value();
        if (x > maxX) {
            maxX = x;
        }
        if (y > maxY) {
            maxY = y;
        }
        if (x < minX) {
            minX = x;
        }
        if (y < minY) {
            minY = y;
        }
        board[x][y] = shapeColor;
    });
    ftxui::Elements boardLines;
    int width  = maxX - minX + 1;
    int height = maxY - minY + 1;
    std::map<int, std::map<int, ftxui::Color>> board2;
    for (const auto& x : board) {
        for (const auto& y : x.second) {
            board2[x.first - minX][y.first - minY] = y.second;
        }
    }
    board = board2;
    for (int y = 0; y < height; ++y) {
        ftxui::Elements arcSetInputLine;
        for (int x = 0; x < width; ++x) {
            const ftxui::Color* currentColor = &ftxAlphaColor;
            if (board.contains(x) && board[x].contains(y)) {
                currentColor = &shapeColor;
            }
            arcSetInputLine.push_back(colorTile(*currentColor));
        }
        boardLines.push_back(hbox(arcSetInputLine));
    }

    auto renderedBoard = vbox(boardLines);
    auto document      = renderedBoard | ftxui::border;
    auto screen        = ftxui::Screen::Create(
        ftxui::Dimension::Fit(document), // Width
        ftxui::Dimension::Fit(document)  // Height
    );
    ftxui::Render(screen, document);
    screen.Print();
    std::cout << "\n";
}

static void printShapeList(CellI& shapeList)
{
    ftxui::Elements shapesInLine;
    const int listSize = static_cast<Number&>(shapeList["size"]).value();
    const int lastListIndex = listSize - 1;
    Visitor::visitList(shapeList, [&shapesInLine, lastListIndex](CellI& shape, int i, bool&) {
        auto renderedShape = renderShape(shape);
        if (i != lastListIndex) {
            shapesInLine.push_back(ftxui::hbox(renderedShape, ftxui::separator()));
        } else {
            shapesInLine.push_back(ftxui::hbox(renderedShape));
        }
    });

    auto document = ftxui::hbox(shapesInLine) | ftxui::border;
    auto screen   = ftxui::Screen::Create(
        ftxui::Dimension::Fit(document), // Width
        ftxui::Dimension::Fit(document) // Height
    );
    ftxui::Render(screen, document);
    screen.Print();
    std::cout << "\n";
}

static void printTask(const nlohmann::json& jsonTask)
{
    auto document = renderJsonBoard(jsonTask);
    auto screen   = ftxui::Screen::Create(
        ftxui::Dimension::Full(),       // Width
        ftxui::Dimension::Fit(document) // Height
    );
    ftxui::Render(screen, document);
    screen.Print();
}

TEST_F(CellTest, FrameTest)
{
    auto& FrameStruct     = getStruct("arc::Frame");
    auto& rotationDir_45  = getVariable("arc::RotationDir::Degree_45");
    auto& rotationDir_90  = getVariable("arc::RotationDir::Degree_90");
    auto& rotationDir_135 = getVariable("arc::RotationDir::Degree_135");
    auto& rotationDir_180 = getVariable("arc::RotationDir::Degree_180");
    auto& rotationDir_225 = getVariable("arc::RotationDir::Degree_225");
    auto& rotationDir_270 = getVariable("arc::RotationDir::Degree_270");
    auto& rotationDir_315 = getVariable("arc::RotationDir::Degree_315");

    const auto& printPixels = [this](CellI& pixelList) -> std::string {
        std::stringstream ss;
        Visitor::visitList(pixelList, [this, &ss](CellI& arcPixel, int, bool&) {
            ss << fmt::format("[{}, {}]", arcPixel["x"].label(), arcPixel["y"].label());
        });

        return ss.str();
    };

    // 0 7 7
    // 7 7 7
    // 0 7 7
    input::Grid inputGrid1("inputGrid1", "[[0, 7, 7], [7, 7, 7], [0, 7, 7]]");
    cells::hybrid::arc::Grid grid1(kb, inputGrid1);
    printGrid(grid1);
    Object frame1(kb, FrameStruct, kb.name("constructor"), { "grid", grid1 });
    frame1.method("process");
    printAs.value(frame1["shapes"]["size"], "frame[shapes][size]");
    EXPECT_EQ(&frame1["shapes"]["size"], &_3_);
    printShapeList(frame1["shapes"]);
    Object& shape1_2     = static_cast<Object&>(frame1["shapes"]["first"]["next"]["value"]);
    auto& shape1_2pixels = shape1_2["pixels"];
    //                                      |x  y |x  y |x  y
    EXPECT_EQ(printPixels(shape1_2pixels),       "[1, 0][2, 0]" \
                                           "[0, 1][1, 1][2, 1]" \
                                                 "[1, 2][2, 2]");
    auto& vectorShape1_2 = static_cast<Object&>(shape1_2.method("toVectorShape"));
    CellI& vectorShape1_2_1v = vectorShape1_2["vectors"]["first"]["value"];
    EXPECT_EQ(&vectorShape1_2_1v["x"], &_1_);
    EXPECT_EQ(&vectorShape1_2_1v["y"], &_0_);
    CellI& vectorShape1_2_2v = vectorShape1_2["vectors"]["first"]["next"]["value"];
    EXPECT_EQ(&vectorShape1_2_2v["x"], &kb.pools.numbers.get(-2));
    EXPECT_EQ(&vectorShape1_2_2v["y"], &_1_);
    printVectorShape(vectorShape1_2);
    CellI& rotated_45 = vectorShape1_2.method(kb.name("rotate"), { "rotationDir", rotationDir_45 });
    printVectorShape(rotated_45);
    CellI& rotated_90 = vectorShape1_2.method(kb.name("rotate"), { "rotationDir", rotationDir_90 });
    printVectorShape(rotated_90);
    CellI& rotated_180 = vectorShape1_2.method(kb.name("rotate"), { "rotationDir", rotationDir_180 });
    printVectorShape(rotated_180);
    CellI& rotated_270 = vectorShape1_2.method(kb.name("rotate"), { "rotationDir", rotationDir_270 });
    printVectorShape(rotated_270);

    // 7 0 0
    // 0 7 0
    // 0 0 7
    input::Grid inputGrid2("inputGrid2", "[[7, 0, 0], [0, 7, 0], [0, 0, 7]]");
    cells::hybrid::arc::Grid grid2(kb, inputGrid2);
    printGrid(grid2);
    Object frame2(kb, FrameStruct, kb.name("constructor"), { "grid", grid2 });
    frame2.method("process");
    printAs.value(frame2["shapes"]["size"], "frame[shapes][size]");
    EXPECT_EQ(&frame2["shapes"]["size"], &_5_);
    printShapeList(frame2["shapes"]);
    auto& shape2_1pixels = frame2["shapes"]["first"]["value"]["pixels"];
    //                                      |x  y |x  y |x  y
    EXPECT_EQ(printPixels(shape2_1pixels), "[0, 0]");

    auto& shape2_2pixels = frame2["shapes"]["first"]["next"]["value"]["pixels"];
    //                                      |x  y |x  y |x  y
    EXPECT_EQ(printPixels(shape2_2pixels),       "[1, 0][2, 0]"
                                                 "[2, 1]");

    auto& shape2_3pixels = frame2["shapes"]["first"]["next"]["next"]["value"]["pixels"];
    //                                      |x  y |x  y |x  y
    EXPECT_EQ(printPixels(shape2_3pixels), "[0, 1]"
                                           "[0, 2][1, 2]");

    // 7 0 7
    // 7 0 7
    // 7 7 7
    input::Grid inputGrid3("inputGrid3", "[" \
                                                     "[7, 0, 7]," \
                                                     "[7, 0, 7]," \
                                                     "[7, 7, 7]]");
    cells::hybrid::arc::Grid grid3(kb, inputGrid3);
    printGrid(grid3);
    Object frame3(kb, FrameStruct, kb.name("constructor"), { "grid", grid3 });
    frame3.method("process");
    printAs.value(frame3["shapes"]["size"], "frame[shapes][size]");
    EXPECT_EQ(&frame3["shapes"]["size"], &_2_);
    printShapeList(frame3["shapes"]);
    auto& shape3_1pixels = frame3["shapes"]["first"]["value"]["pixels"];
    //                                      |x  y |x  y |x  y
    EXPECT_EQ(printPixels(shape3_1pixels), "[0, 0]"    "[2, 0]" \
                                           "[0, 1]"    "[2, 1]" \
                                           "[0, 2][1, 2][2, 2]");
}

TEST_F(CellTest, DISABLED_ArcTaskTest)
{
    auto& frameStruct                    = getStruct("arc::Frame");
    static const std::string arcFilePath = INFOCELL_ORIGARC_FILEPATH "007bbfb7.json";
    auto jsonTask                        = json::parse(std::ifstream(arcFilePath));
    auto document                        = renderJsonBoard(jsonTask["/test/0/input"_json_pointer]);
    auto screen                          = ftxui::Screen::Create(
        ftxui::Dimension::Full(),       // Width
        ftxui::Dimension::Fit(document) // Height
    );
    ftxui::Render(screen, document);
    screen.Print();

    ArcPrizeTask arcTaskLoader(kb, "007bbfb7", jsonTask);
    CellI& arcTask = arcTaskLoader.m_cellTask;

    Object frame(kb, frameStruct, kb.name("constructor"), { "grid", arcTask["challenge"]});
    frame.method("process");
    printAs.value(frame["shapes"]["size"], "frame[shapes][size]");
    printShapeList(frame["shapes"]);
}

TEST_F(CellTest, DISABLED_ArcTaskFromArcPrize)
{
    const auto examineTask = [this](const nlohmann::json& allTask, const std::string& riddleId, const std::string& taskPath, const std::vector<std::string>& cellPaths) {
        auto& frameStruct             = getStruct("arc::Frame");
        std::string jsonRiddlePathStr = "/";
        jsonRiddlePathStr += riddleId;
        json::json_pointer jsonRiddlePath(jsonRiddlePathStr);
        auto jsonRiddle = allTask[jsonRiddlePath];
        json::json_pointer jsonTaskPath(taskPath);
        auto jsonTask = jsonRiddle[jsonTaskPath];

        printTask(jsonRiddle[jsonTaskPath]);

        ArcPrizeTask arcTaskLoader(kb, riddleId, jsonRiddle);
        CellI* arcTaskPtr = &arcTaskLoader.m_cellTask;
        for (const auto& cellPath : cellPaths) {
            arcTaskPtr = &(*arcTaskPtr)[cellPath];
        }
        CellI& arcTask = *arcTaskPtr;

        Object frame(kb, frameStruct, kb.name("constructor"), { "grid", arcTask });
        frame.method("process");
        printAs.value(frame["shapes"]["size"], "frame[shapes][size]");
        printShapeList(frame["shapes"]);
    };

    static const std::string arcFilePath = INFOCELL_ARCPRIZE_PATH INFOCELL_ARC_PRIZE_TRAINING_CHALLENGES_FILENAME;
    auto allTask                         = json::parse(std::ifstream(arcFilePath));

    examineTask(allTask, "0ca9ddb6", "/test/0/input", { "challenge" });
    examineTask(allTask, "0ca9ddb6", "/train/0/input", { "examples", "first", "value", "input" });
    examineTask(allTask, "0ca9ddb6", "/train/0/output", { "examples", "first", "value", "output" });
}

std::string getArcColorName(CellI& hybridColor)
{
    static std::map<infocell::arc::Colors, std::string> arcColorNames = {
        { infocell::arc::Colors::black, "black" },
        { infocell::arc::Colors::blue, "blue" },
        { infocell::arc::Colors::red, "red" },
        { infocell::arc::Colors::green, "green" },
        { infocell::arc::Colors::yellow, "yellow" },
        { infocell::arc::Colors::grey, "grey" },
        { infocell::arc::Colors::fuschia, "fuschia" },
        { infocell::arc::Colors::orange, "orange" },
        { infocell::arc::Colors::teal, "teal" },
        { infocell::arc::Colors::brown, "brown" }
    };

    infocell::arc::Colors colorNum = static_cast<infocell::arc::Colors>(static_cast<Number&>(hybridColor).value());

    return arcColorNames.find(colorNum)->second;
}

TEST_F(CellTest, DISABLE_ArcTaskFromArcPrizeExamineTrainPair)
{
    const auto examineTrainPair = [this](const nlohmann::json& allTask, const std::string& riddleId, int trainNum) {
        auto& frameStruct             = getStruct("arc::Frame");
        std::string jsonRiddlePathStr = "/";
        jsonRiddlePathStr += riddleId;
        json::json_pointer jsonRiddlePath(jsonRiddlePathStr);
        auto jsonRiddle = allTask[jsonRiddlePath];
        std::string exampleInputPath = fmt::format("/train/{}/input", trainNum);
        std::string exampleOutputPath = fmt::format("/train/{}/output", trainNum);
        std::vector<std::string> cellInputPath = { "examples", "first" };
        std::vector<std::string> cellOutputPath = { "examples", "first" };
        for (int i = 0; i < trainNum; ++i) {
            cellInputPath.push_back("next");
            cellOutputPath.push_back("next");
        }
        cellInputPath.push_back("value");
        cellOutputPath.push_back("value");
        cellInputPath.push_back("input");
        cellOutputPath.push_back("output");

        json::json_pointer jsonInputTaskPath(exampleInputPath);
        json::json_pointer jsonOutputTaskPath(exampleOutputPath);
        auto jsonInputTask = jsonRiddle[jsonInputTaskPath];
        auto jsonOutputTask = jsonRiddle[jsonOutputTaskPath];

        printTask(jsonRiddle[jsonInputTaskPath]);
        printTask(jsonRiddle[jsonOutputTaskPath]);

        CellI* inputGridPtr  = nullptr;
        CellI* inputShapesPtr   = nullptr;
        CellI* outputGridPtr = nullptr;
        CellI* outputShapesPtr  = nullptr;

        ArcPrizeTask arcTaskLoader(kb, riddleId, jsonRiddle);
        {
            CellI* arcTaskPtr = &arcTaskLoader.m_cellTask;
            for (const auto& cellPath : cellInputPath) {
                arcTaskPtr = &(*arcTaskPtr)[cellPath];
            }
            inputGridPtr = arcTaskPtr;
            CellI& arcTask  = *arcTaskPtr;

            Object frame(kb, frameStruct, kb.name("constructor"), { "grid", arcTask });
            frame.method("process");
            inputShapesPtr = &frame["shapes"];
            printAs.value(frame["shapes"]["size"], "frame[shapes][size]");
            printShapeList(frame["shapes"]);
        }
        {
            CellI* arcTaskPtr = &arcTaskLoader.m_cellTask;
            for (const auto& cellPath : cellOutputPath) {
                arcTaskPtr = &(*arcTaskPtr)[cellPath];
            }
            outputGridPtr = arcTaskPtr;
            CellI& arcTask   = *arcTaskPtr;

            Object frame(kb, frameStruct, kb.name("constructor"), { "grid", arcTask });
            frame.method("process");
            outputShapesPtr = &frame["shapes"];
            printAs.value(frame["shapes"]["size"], "frame[shapes][size]");
            printShapeList(frame["shapes"]);
        }
        CellI& inputGrid     = *inputGridPtr;
        CellI& inputShapes   = *inputShapesPtr;
        CellI& outputGrid    = *outputGridPtr;
        CellI& outputShapes  = *outputShapesPtr;
        int inputGridWidth   = static_cast<Number&>(inputGrid[kb.ids.width]).value();
        int outputGridWidth  = static_cast<Number&>(outputGrid[kb.ids.width]).value();
        int inputGridHeight  = static_cast<Number&>(inputGrid[kb.ids.height]).value();
        int outputGridHeight = static_cast<Number&>(outputGrid[kb.ids.height]).value();
        int inputShapesNum   = static_cast<Number&>(inputShapes[kb.ids.size]).value();
        int outputShapesNum  = static_cast<Number&>(outputShapes[kb.ids.size]).value();
        if (inputGridWidth == outputGridWidth && inputGridHeight == outputGridHeight && outputShapesNum > inputShapesNum) {
            Visitor::visitList(inputShapes, [](CellI& shape, int i, bool&) {
                std::cout << "DDDD input shape color: " << getArcColorName(shape["color"]) << ", size: " << static_cast<Number&>(shape["pixels"]["size"]).value() << std::endl;
            });
            Visitor::visitList(outputShapes, [](CellI& shape, int i, bool&) {
                std::cout << "DDDD output shape color: " << getArcColorName(shape["color"]) << ", size: " << static_cast<Number&>(shape["pixels"]["size"]).value() << std::endl;
            });
        }
    };

    static const std::string arcFilePath = INFOCELL_ARCPRIZE_PATH INFOCELL_ARC_PRIZE_TRAINING_CHALLENGES_FILENAME;
    auto allTask                         = json::parse(std::ifstream(arcFilePath));

    examineTrainPair(allTask, "0ca9ddb6", 0);
}

TEST_F(CellTest, LoadAllArcTask)
{
    static const std::string arcFilePath = INFOCELL_ARCPRIZE_PATH INFOCELL_ARC_PRIZE_TRAINING_CHALLENGES_FILENAME;
    TaskSet taskSet(kb, INFOCELL_ARCPRIZE_PATH INFOCELL_ARC_PRIZE_TRAINING_CHALLENGES_FILENAME);
    for (auto& task : taskSet.m_tasks) {
        std::cout <<
            "id: " << task.first <<
            ", examples num:" << static_cast<List&>(task.second.m_cellExamplesList).size() <<
            ", tests num:" << static_cast<List&>(task.second.m_cellTestsList).size() << std::endl;
        std::cout <<"   examples:" << std::endl;
        Visitor::visitList(task.second.m_cellExamplesList, [](CellI& example, int i, bool&) {
            std::cout <<
                "    size " << static_cast<hybrid::arc::Grid&>(example["input"]).width() << "x" << static_cast<hybrid::arc::Grid&>(example["input"]).height() <<
                " -> " << static_cast<hybrid::arc::Grid&>(example["output"]).width() << "x" << static_cast<hybrid::arc::Grid&>(example["output"]).height() << std::endl;
        });
        std::cout << "   tests:" << std::endl;
        Visitor::visitList(task.second.m_cellTestsList, [](CellI& example, int i, bool&) {
            std::cout << "    size " << static_cast<hybrid::arc::Grid&>(example["input"]).width() << "x" << static_cast<hybrid::arc::Grid&>(example["input"]).height() << std::endl;
        });
    }
}

TEST_F(CellTest, LoadThoseArcTaskWhereInputSizeEqOutputSize)
{
    static const std::string arcFilePath = INFOCELL_ARCPRIZE_PATH INFOCELL_ARC_PRIZE_TRAINING_CHALLENGES_FILENAME;
    TaskSet taskSet(kb, INFOCELL_ARCPRIZE_PATH INFOCELL_ARC_PRIZE_TRAINING_CHALLENGES_FILENAME);
    for (auto& task : taskSet.m_tasks) {
        bool allSameSize = false;
        Visitor::visitList(task.second.m_cellExamplesList, [&allSameSize](CellI& example, int i, bool& stop) {
            int inputWidth  = static_cast<hybrid::arc::Grid&>(example["input"]).width();
            int inputHeight = static_cast<hybrid::arc::Grid&>(example["input"]).height();
            int outputWidth = static_cast<hybrid::arc::Grid&>(example["output"]).width();
            int outputHeight = static_cast<hybrid::arc::Grid&>(example["output"]).height();
            if (inputWidth == outputWidth && inputHeight == outputHeight) {
                allSameSize = true;
            } else {
                allSameSize = false;
                stop        = true;
            }
        });
        if (!allSameSize) {
            continue;
        }
        std::cout << "id: " << task.first << ", examples num:" << static_cast<List&>(task.second.m_cellExamplesList).size() << ", tests num:" << static_cast<List&>(task.second.m_cellTestsList).size() << std::endl;
        std::cout << "   examples:" << std::endl;
    }
}

TEST_F(CellTest, ArcTaskFromArcPrizeExamineTrainPairSketchCpp)
{
    const auto examineTrainPair = [this](const nlohmann::json& allTask, const std::string& riddleId, int trainNum) {
        auto& frameStruct             = getStruct("arc::Frame");
        std::string jsonRiddlePathStr = "/";
        jsonRiddlePathStr += riddleId;
        json::json_pointer jsonRiddlePath(jsonRiddlePathStr);
        auto jsonRiddle                         = allTask[jsonRiddlePath];
        std::string exampleInputPath            = fmt::format("/train/{}/input", trainNum);
        std::string exampleOutputPath           = fmt::format("/train/{}/output", trainNum);
        std::vector<std::string> cellInputPath  = { "examples", "first" };
        std::vector<std::string> cellOutputPath = { "examples", "first" };
        for (int i = 0; i < trainNum; ++i) {
            cellInputPath.push_back("next");
            cellOutputPath.push_back("next");
        }
        cellInputPath.push_back("value");
        cellOutputPath.push_back("value");
        cellInputPath.push_back("input");
        cellOutputPath.push_back("output");

        json::json_pointer jsonInputTaskPath(exampleInputPath);
        json::json_pointer jsonOutputTaskPath(exampleOutputPath);
        auto jsonInputTask  = jsonRiddle[jsonInputTaskPath];
        auto jsonOutputTask = jsonRiddle[jsonOutputTaskPath];

        printTask(jsonRiddle[jsonInputTaskPath]);
        printTask(jsonRiddle[jsonOutputTaskPath]);

        CellI* inputGridPtr    = nullptr;
        CellI* inputShapesPtr  = nullptr;
        CellI* outputGridPtr   = nullptr;
        CellI* outputShapesPtr = nullptr;

        ArcPrizeTask arcTaskLoader(kb, riddleId, jsonRiddle);
        CellI* arcTaskPtr = nullptr;
        {
            arcTaskPtr = &arcTaskLoader.m_cellTask;
            for (const auto& cellPath : cellInputPath) {
                arcTaskPtr = &(*arcTaskPtr)[cellPath];
            }
            inputGridPtr        = arcTaskPtr;
            std::cout << "";
        }
        CellI& inputGrid = *arcTaskPtr;
        {
            arcTaskPtr = &arcTaskLoader.m_cellTask;
            for (const auto& cellPath : cellOutputPath) {
                arcTaskPtr = &(*arcTaskPtr)[cellPath];
            }
            outputGridPtr  = arcTaskPtr;
            CellI& arcTask = *arcTaskPtr;
        }
        CellI& outputGrid = *arcTaskPtr;
        Visitor::visitList(inputGrid["pixels"], [this, &outputGrid](CellI& arcPixel, int, bool&) {
            //                std::cout << fmt::format("[{}, {}]", arcPixel["x"].label(), arcPixel["y"].label());
            // pixelHashList = arcPixel.hashList();
            List pixelContent(kb, kb.std.Pixel);
            pixelContent.add(arcPixel[kb.coordinates.x]);
            pixelContent.add(arcPixel[kb.coordinates.y]);

            CellI& outputColor = static_cast<TrieMap&>(outputGrid["pixelsMap"]).getValue(pixelContent);
            if (&outputColor != &arcPixel["color"])
            {
                std::cout << "DDDD removedPixel: " << fmt::format("[{}, {}, {}]\n", arcPixel["x"].label(), arcPixel["y"].label(), arcPixel["color"].label());
            }
        });
        Visitor::visitList(outputGrid["pixels"], [this, &inputGrid](CellI& arcPixel, int, bool&) {
            //                std::cout << fmt::format("[{}, {}]", arcPixel["x"].label(), arcPixel["y"].label());
            // pixelHashList = arcPixel.hashList();
            List pixelContent(kb, kb.std.Pixel);
            pixelContent.add(arcPixel[kb.coordinates.x]);
            pixelContent.add(arcPixel[kb.coordinates.y]);

            CellI& inputColor = static_cast<TrieMap&>(inputGrid["pixelsMap"]).getValue(pixelContent);
            if (&inputColor != &arcPixel["color"]) {
                std::cout << "DDDD addedPixel: " << fmt::format("[{}, {}, {}]\n", arcPixel["x"].label(), arcPixel["y"].label(), arcPixel["color"].label());
            }
        });
#if 0
                vector<vector<int>> findDifference(vector<int>& nums1, vector<int>& nums2)
                {
                    vector<vector<int>> ret(2);
                    auto& ret0 = ret[0];
                    auto& ret1 = ret[1];

                    std::set<int> nums1Set;
                    for (auto num : nums1) {
                        nums1Set.insert(num);
                    }

                    std::set<int> nums2Set;
                    for (auto num : nums2) {
                        nums2Set.insert(num);
                    }

                    for (auto num : nums1Set) {
                        if (!nums2Set.contains(num)) {
                            ret0.push_back(num);
                        }
                    }
                    for (auto num : nums2Set) {
                        if (!nums1Set.contains(num)) {
                            ret1.push_back(num);
                        }
                    }
                    return ret;
                }
                template<class T>
                struct SetDifference
                {
                    Set<T> removedPixels;
                    Set<T> addedPixels;
                };
                findDifference(hybrid::Grid& inputGrid, hybrid::Grid& outputGrid) -> SetDifference<Pixel>
                {
                    TrieMap<List, Pixel> inputPixelMap;
                    TrieMap<List, Pixel> outputPixelMap;
                    inputGridIt = inputGrid.GetEnumerator();
                    Visitor::visitList(inputGrid["pixels"], [this](CellI& arcPixel, int, bool&) {
                        pixelHashList = arcPixel.hashList();
                        inputPixelMap.insert(pixelHashList, pixel);
                    });
                    Visitor::visitList(inputPixelMap["pixels"], [this](CellI& arcPixel, int, bool&) {
                        pixelHashList = arcPixel.hashList();
                        outputPixelMap.insert(pixelHashList, pixel);
                    });
                    Visitor::visitList(inputPixelMap["list"], [this](CellI& kvPair, int, bool&) {
                        if (not outputPixelMap.hasKey(kvPair.key)) {
                            removedPixels.insert(kvPair);
                        }
                    });
                    Visitor::visitList(outputPixelMap["list"], [this](CellI& kvPair, int, bool&) {
                        if (not inputPixelMap.hasKey(kvPair.key)) {
                            addedPixels.insert(kvPair);
                        }
                    });
                }

#endif
    };

    static const std::string arcFilePath = INFOCELL_ARCPRIZE_PATH INFOCELL_ARC_PRIZE_TRAINING_CHALLENGES_FILENAME;
    auto allTask                         = json::parse(std::ifstream(arcFilePath));

    examineTrainPair(allTask, "0ca9ddb6", 0);
}

TEST_F(CellTest, DISABLED_ArcTaskFromArcPrizeExamineAllTrainPair)
{
    const auto examineTrainPair = [this](const nlohmann::json& allTask, int trainNum) {
        auto& frameStruct = getStruct("arc::Frame");
        int i             = 0;
        for (json::const_iterator it = allTask.begin(); it != allTask.end(); ++it) {
            std::string jsonRiddlePathStr = "/";
            jsonRiddlePathStr += it.key();
            json::json_pointer jsonRiddlePath(jsonRiddlePathStr);
            auto jsonRiddle                         = allTask[jsonRiddlePath];
            std::string exampleInputPath            = fmt::format("/train/{}/input", trainNum);
            std::string exampleOutputPath           = fmt::format("/train/{}/output", trainNum);
            std::vector<std::string> cellInputPath  = { "examples", "first" };
            std::vector<std::string> cellOutputPath = { "examples", "first" };
            for (int i = 0; i < trainNum; ++i) {
                cellInputPath.push_back("next");
                cellOutputPath.push_back("next");
            }
            cellInputPath.push_back("value");
            cellOutputPath.push_back("value");
            cellInputPath.push_back("input");
            cellOutputPath.push_back("output");

            json::json_pointer jsonInputTaskPath(exampleInputPath);
            json::json_pointer jsonOutputTaskPath(exampleOutputPath);
            auto jsonInputTask  = jsonRiddle[jsonInputTaskPath];
            auto jsonOutputTask = jsonRiddle[jsonOutputTaskPath];

//            printTask(jsonRiddle[jsonInputTaskPath]);
//            printTask(jsonRiddle[jsonOutputTaskPath]);

            CellI* inputGridPtr  = nullptr;
            CellI* outputGridPtr = nullptr;

            ArcPrizeTask arcTaskLoader(kb, it.key(), jsonRiddle);
            {
                CellI* arcTaskPtr = &arcTaskLoader.m_cellTask;
                for (const auto& cellPath : cellInputPath) {
                    arcTaskPtr = &(*arcTaskPtr)[cellPath];
                }
                inputGridPtr = arcTaskPtr;

                Object frame(kb, frameStruct, kb.name("constructor"), { "grid", *inputGridPtr });
                frame.method("process");
//                printAs.value(frame["shapes"]["size"], "frame[shapes][size]");
//                printShapeList(frame["shapes"]);
            }
            {
                CellI* arcTaskPtr = &arcTaskLoader.m_cellTask;
                for (const auto& cellPath : cellOutputPath) {
                    arcTaskPtr = &(*arcTaskPtr)[cellPath];
                }
                outputGridPtr = arcTaskPtr;

                Object frame(kb, frameStruct, kb.name("constructor"), { "grid", *outputGridPtr });
                frame.method("process");
//                printAs.value(frame["shapes"]["size"], "frame[shapes][size]");
//                printShapeList(frame["shapes"]);
            }
            CellI& inputGrid  = *inputGridPtr;
            CellI& outputGrid = *outputGridPtr;
            if (&inputGrid[kb.ids.width] == &outputGrid[kb.ids.width] && &inputGrid[kb.ids.height] == &outputGrid[kb.ids.height]) {
                std::cout << i << " " << it.key() << std::endl;
            }
            ++i;
        }
    };

    static const std::string arcFilePath = INFOCELL_ARCPRIZE_PATH INFOCELL_ARC_PRIZE_TRAINING_CHALLENGES_FILENAME;
    auto allTask                         = json::parse(std::ifstream(arcFilePath));

    examineTrainPair(allTask, 0);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    std::cout << "Constructed: " << CellI::s_constructed << ", destructed: " << CellI::s_destructed << ", live: " << CellI::s_constructed - CellI::s_destructed << std::endl;
    std::cout << "Tick count: " << CellTest::getKb().ap.m_time.value() << std::endl;
    CellTest::freeKb();

    return ret;
}