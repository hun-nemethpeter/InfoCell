#include "CellTestBase.h"

#include <fstream>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "Config.h"
#include "arc/Grid.h"
#include "arc/Task.h"
#include "arc/hybridcells/Grid.h"
#include "cells/Cells.h"
#include "cells/Compiler.h"
#include "cells/TestLib.h"
#include "tui/App.h"

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include "util/Log.h"

namespace fs = std::filesystem;
using nlohmann::json;
using namespace infocell;
using namespace infocell::cells;

using infocell::cells::test::CellTest;
namespace nativearc = infocell::arc::native;
namespace hybridarc = infocell::cells::arc;

// TODO
// inline methods
// type checking
// remove .label() from CellI

TEST_F(CellTest, CompilerSmokeTest)
{
    Ast::Scope rootScope(w, "root");

    TestLib testLib(w, rootScope);
    testLib.include(w.stdLib());

    Compiler compiler(w);
    compiler.compile(testLib);

    testLib.mergeTo(w.stdLib());

    TRACE(compiledSymbols, "All compiled symbols:");

    TRACE(compiledSymbols, "  structs:");
    Visitor::visitList(testLib.structs()[id.list], [this](CellI& kv, int, bool&) {
        TRACE(compiledSymbols, "    {}", kv[id.key].label());
    });

    TRACE(compiledSymbols, "  functions:");
    Visitor::visitList(testLib.functions()[id.list], [this](CellI& kv, int, bool&) {
        TRACE(compiledSymbols, "    {} : {}", kv[id.key].label(), kv[id.value].label());
    });

    TRACE(compiledSymbols, "  variables:");
    Visitor::visitList(testLib.variables()[id.list], [this](CellI& kv, int, bool&) {
        TRACE(compiledSymbols, "    {} : {}", kv[id.key].label(), kv[id.value].label());
    });

    Object testStruct(w, w.getStruct("std::Struct"), w.name("constructor"), "testStruct");
    Object testRecursiveStruct(w, w.getStruct("std::Struct"), w.name("constructorWithRecursiveType"), "testRecursiveStruct");

    Object testIndex(w, w.getStruct("std::Index"), w.name("constructor"), "testIndex");
    testIndex.method(w.name("insert"), { "key", _1_ }, { "value", _2_ });
}

TEST_F(CellTest, StringSplit)
{
    const std::string inputStr = "std::Cell";
    std::vector<std::string> sliced;
    splitNamespacedString(sliced, inputStr);
    EXPECT_EQ(sliced.size(), 2);
    EXPECT_EQ(sliced[0], "std");
    EXPECT_EQ(sliced[1], "Cell");
}

TEST_F(CellTest, StringSplitWithExtraPostfix)
{
    const std::string inputStr = "std::Cell::";
    std::vector<std::string> sliced;
    splitNamespacedString(sliced, inputStr);
    EXPECT_EQ(sliced.size(), 3);
    EXPECT_EQ(sliced[0], "std");
    EXPECT_EQ(sliced[1], "Cell");
    EXPECT_EQ(sliced[2], "");
}

TEST_F(CellTest, StringSplitWithExtraPrefix)
{
    const std::string inputStr = "::std::Cell";
    std::vector<std::string> sliced;
    splitNamespacedString(sliced, inputStr);
    EXPECT_EQ(sliced.size(), 3);
    EXPECT_EQ(sliced[0], "");
    EXPECT_EQ(sliced[1], "std");
    EXPECT_EQ(sliced[2], "Cell");
}

TEST_F(CellTest, StringSplitWithExtraChar)
{
    const std::string inputStr = ":::std::Cell";
    std::vector<std::string> sliced;
    splitNamespacedString(sliced, inputStr);
    EXPECT_EQ(sliced.size(), 3);
    EXPECT_EQ(sliced[0], "");
    EXPECT_EQ(sliced[1], ":std");
    EXPECT_EQ(sliced[2], "Cell");
}

TEST_F(CellTest, ToolFinderTestForSet)
{
    ToolFinder& toolFinder = *w.globalScope.m_toolFinder;
    // test the pixel.set(green, 5)
    Object& pixel           = *new Object(w, std.Color, "pixel");
    CellI& requestForSetGet = *new Object(w, std.ast.Get);
    requestForSetGet.set(id.cell, w.ast.cell(pixel));
    requestForSetGet.set(id.key, w.ast.cell(id.green));

    CellI& requestForSet = *new Object(w, std.ast.Equal, "pixel.get(green) == 5");
    requestForSet.set(id.lhs, requestForSetGet);
    requestForSet.set(id.rhs, w.ast.cell(w._5_));

    CellI& requestForSetAstList = toolFinder.serializeEffectAst(requestForSet);
    {
        std::stringstream ss;
        Visitor::visitList(requestForSetAstList, [&ss](CellI& value, int, bool& stop) {
            ss << value.label() << " ";
        });
        EXPECT_EQ(ss.str(), "__type__ ast::Equal lhs op push __type__ ast::Get cell pixel key green op pop rhs 5 ");
    }

    CellI& resultToolAst = *toolFinder.findToolByEffectAst(requestForSet);

    EXPECT_EQ(&resultToolAst.__type__(), &std.ast.Set);
    EXPECT_EQ(&resultToolAst[id.cell].__type__(), &std.ast.Cell);
    EXPECT_EQ(&resultToolAst[id.cell][id.value], &pixel);
    EXPECT_EQ(&resultToolAst[id.key].__type__(), &std.ast.Cell);
    EXPECT_EQ(&resultToolAst[id.key][id.value], &id.green);
    EXPECT_EQ(&resultToolAst[id.value].__type__(), &std.ast.Cell);
    EXPECT_EQ(&resultToolAst[id.value][id.value], &w._5_);
}

TEST_F(CellTest, ToolFinderTestForGet)
{
    ToolFinder& toolFinder = *w.globalScope.m_toolFinder;
    Object& pixel                 = *new Object(w, std.Color, "pixel");

    // test the return get(x, y)
    CellI& requestForGetGet = *new Object(w, std.ast.Get);
    requestForGetGet.set(id.cell, w.ast.cell(pixel));
    requestForGetGet.set(id.key, w.ast.cell(id.green));

    CellI& requestForGet = *new Object(w, std.ast.Return, "return pixel.get(green)");
    requestForGet.set(id.value, requestForGetGet);

    CellI& requestForGetAstList = toolFinder.serializeEffectAst(requestForGet);
    {
        std::stringstream ss;
        Visitor::visitList(requestForGetAstList, [&ss](CellI& value, int, bool& stop) {
            ss << value.label() << " ";
        });
        EXPECT_EQ(ss.str(), "__type__ ast::Return value op push __type__ ast::Get cell pixel key green op pop ");
    }

    // the trick here is that we finding tool for "return get(x, y)" but creating tool with just "get(x, y)" without return...
    // so we can test tool creation in a composed tool as "return get(x, y)" doesn't make sense as a standalone request

    // passing requestForGet here which is "return get(x, y)"
#if 0 // TODO
    CellI& resultToolAst    = *toolFinder.findToolByAst(requestForGet);

    EXPECT_EQ(&resultToolAst.struct_(), &std.ast.Get);
    EXPECT_EQ(&resultToolAst[w.ids.cell].struct_(), &std.ast.Cell);
    EXPECT_EQ(&resultToolAst[w.ids.cell][w.ids.value], &pixel);
    EXPECT_EQ(&resultToolAst[w.ids.key].struct_(), &std.ast.Cell);
    EXPECT_EQ(&resultToolAst[w.ids.key][w.ids.value], &w.ids.green);
#endif
}

TEST_F(CellTest, ToolFinderTestForGetInGet)
{
    ToolFinder& toolFinder = *w.globalScope.m_toolFinder;

    // currentTheme is a test structure to be able to test a nested get. So instead of pixel.get(green) we can replace the "green" node with "currentTheme / std.Color" so we can write
    // currentTheme.get(std.Color).get(green) == 5
    Index currentTheme(w, "currentTheme");
    currentTheme.set(std.Color, id.green);

    // test the return currentTheme.get(std.Color).get(green) == 5
    CellI& requestForSetWithGetGetGet = *new Object(w, std.ast.Get, "currentTheme.get(std.Color)");
    requestForSetWithGetGetGet.set(id.cell, w.ast.cell(currentTheme));
    requestForSetWithGetGetGet.set(id.key, w.ast.cell(std.Color));

    CellI& requestForSetWithGetGet = *new Object(w, std.ast.Get, "currentTheme.get(std.Color).get(green)");
    requestForSetWithGetGet.set(id.cell, requestForSetWithGetGetGet);
    requestForSetWithGetGet.set(id.key, w.ast.cell(id.green));

    CellI& requestForSetWithGet = *new Object(w, std.ast.Equal, "currentTheme.get(std.Color).get(green) == 5");
    requestForSetWithGet.set(id.lhs, requestForSetWithGetGet);
    requestForSetWithGet.set(id.rhs, w.ast.cell(w._5_));

    CellI& requestForSetWithGetAstList = toolFinder.serializeEffectAst(requestForSetWithGet);
    {
        std::stringstream ss;
        Visitor::visitList(requestForSetWithGetAstList, [&ss](CellI& value, int, bool& stop) {
            ss << value.label() << " ";
        });
        EXPECT_EQ(ss.str(), "__type__ ast::Equal lhs op push __type__ ast::Get cell op push __type__ ast::Get cell currentTheme key Color op pop key green op pop rhs 5 ");
    }

    CellI& resultToolAst = *toolFinder.findToolByEffectAst(requestForSetWithGet);

    EXPECT_EQ(&resultToolAst.__type__(), &std.ast.Set);
    EXPECT_EQ(&resultToolAst[id.cell].__type__(), &std.ast.Get);
    EXPECT_EQ(&resultToolAst[id.cell][id.cell].__type__(), &std.ast.Cell);
    EXPECT_EQ(&resultToolAst[id.cell][id.cell][id.value], &currentTheme);
    EXPECT_EQ(&resultToolAst[id.cell][id.key].__type__(), &std.ast.Cell);
    EXPECT_EQ(&resultToolAst[id.cell][id.key][id.value], &std.Color);
    EXPECT_EQ(&resultToolAst[id.key].__type__(), &std.ast.Cell);
    EXPECT_EQ(&resultToolAst[id.key][id.value], &id.green);
    EXPECT_EQ(&resultToolAst[id.value].__type__(), &std.ast.Cell);
    EXPECT_EQ(&resultToolAst[id.value][id.value], &w._5_);
    std::cout << "";
}

TEST_F(CellTest, ToolFinderTestForGetInGetWithAstHelper)
{
    ToolFinder& toolFinder = *w.globalScope.m_toolFinder;

    class RequestHelper : public AstHelper
    {
    public:
        Base* value = nullptr;
        RequestHelper(World& w) :
            AstHelper(w)
        {
            // currentTheme.get(std.Color).get(green) == 5
            Var& currentTheme = var_("currentTheme");
            Base& ast = equal(_(currentTheme) / _(std.Color) / _(id.green), _(_5_));
            value     = &ast;
        }
    } requestHelper(w);
    CellI& request = *requestHelper.value;
    CellI& serializedRequest = toolFinder.serializeEffectAst(request);
    {
        std::stringstream ss;
        Visitor::visitList(serializedRequest, [&ss](CellI& value, int, bool& stop) {
            ss << value.label() << " ";
        });
        EXPECT_EQ(ss.str(), "__type__ ast::Equal lhs op push __type__ ast::Get cell op push __type__ ast::Get cell currentTheme key Color op pop key green op pop rhs 5 ");
    }
}

TEST_F(CellTest, ToolFinderTestForMathAdd)
{
    ToolFinder& toolFinder = *w.globalScope.m_toolFinder;

    class RequestHelper : public AstHelper
    {
    public:
        Base* varX = nullptr;
        Base* request = nullptr;
        RequestHelper(World& w) :
            AstHelper(w)
        {
            Var& x = var_("x");

#if 1 // TODO
            // test x + 2 = 4 => equal(add(get(x, value), 2)), 4)
            Base& ast = equal(add(_(x) / _(id.value), _(_2_)), _(_4_));
#else
            // test 2 + x = 4 => equal(add(2, get(x, value))), 4)
            Base& ast = equal(add(_(_2_), _(x) / _(w.ids.value)), _(_4_));
#endif
            varX    = &x;
            request = &ast;
        }
    } requestHelper(w);

    CellI& request = *requestHelper.request;
    CellI& varX = *requestHelper.varX;

    CellI& serializedRequest = toolFinder.serializeEffectAst(request);
    {
        std::stringstream ss;
        Visitor::visitList(serializedRequest, [&ss](CellI& value, int, bool& stop) {
            ss << value.label() << " ";
        });
        EXPECT_EQ(ss.str(), "__type__ ast::Equal lhs op push __type__ ast::Add lhs op push __type__ ast::Get cell x key value op pop rhs 2 op pop rhs 4 ");
    }

    CellI& resultToolAst = *toolFinder.findToolByEffectAst(request);

    // set(_(x), _(id.value), subtract(_(4), _(2)))
    EXPECT_EQ(&resultToolAst.__type__(), &std.ast.Set);
    EXPECT_EQ(&resultToolAst[id.cell].__type__(), &std.ast.Cell);
    EXPECT_EQ(&resultToolAst[id.cell][id.value], &varX);
    EXPECT_EQ(&resultToolAst[id.key].__type__(), &std.ast.Cell);
    EXPECT_EQ(&resultToolAst[id.key][id.value], &id.value);
    EXPECT_EQ(&resultToolAst[id.value].__type__(), &std.ast.Subtract);
    EXPECT_EQ(&resultToolAst[id.value][id.lhs].__type__(), &std.ast.Cell);
    EXPECT_EQ(&resultToolAst[id.value][id.lhs][id.value], &_4_);
    EXPECT_EQ(&resultToolAst[id.value][id.rhs].__type__(), &std.ast.Cell);
    EXPECT_EQ(&resultToolAst[id.value][id.rhs][id.value], &_2_);
}

TEST_F(CellTest, Numbers)
{
    CellI& digit_0 = w.pools.digits[0];
    CellI& digit_1 = w.pools.digits[1];
    CellI& digit_2 = w.pools.digits[2];
    CellI& digit_3 = w.pools.digits[3];
    CellI& digit_4 = w.pools.digits[4];
    CellI& digit_5 = w.pools.digits[5];
    CellI& digit_6 = w.pools.digits[6];
    CellI& digit_7 = w.pools.digits[7];
    CellI& digit_8 = w.pools.digits[8];
    CellI& digit_9 = w.pools.digits[9];

    CellI& number_123 = toCellNumber(1234567890);
    EXPECT_EQ(&number_123[w.numbers.sign], &w.numbers.positive);
    List& number_123_digits = static_cast<List&>(number_123[id.value]);
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
    EXPECT_EQ(&number_minus_123[w.numbers.sign], &w.numbers.negative);
    List& number_minus_123_digits = static_cast<List&>(number_123[id.value]);
    // TODO
}

TEST_F(CellTest, PrintStdCodes)
{
#if 1
    auto& ListItemStruct = getStruct(w.templateId("std::ListItem", id.valueType, std.Number));
    auto& ListStruct     = getStruct(w.templateId("std::List", id.valueType, std.Number));
    auto& MapStruct      = getStruct(w.templateId("std::Map", id.keyType, std.Cell, id.valueType, std.Slot));
    auto& TrieMapStruct  = getStruct(w.templateId("std::TrieMap", id.keyType, std.Number, id.valueType, std.Color));
    auto& SetStruct      = getStruct(w.templateId("std::Set", id.valueType, std.Number));
    auto& IndexStruct    = getStruct("std::Index");
    auto& Struct         = getStruct("std::Struct");

    EXPECT_EQ(&Struct, &std.Struct);

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
    printMethodInType(Struct, "addTypeAlias");
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
    auto& TestStruct = getStruct("test::TestStruct");
    printMethodInType(TestStruct, "factorial");
    printMethodInType(TestStruct, "testCreateNewListOfNumbers");
}

TEST_F(CellTest, PrintArcCodes)
{
    auto& VectorStruct = getStruct("arc::Vector");
    auto& ShapeStruct  = getStruct("arc::Shape");
    auto& FrameStruct = getStruct("arc::Frame");

    printMethodInType(VectorStruct, "rotate");

    printAs.value(ShapeStruct);
    printMethodInType(ShapeStruct, "constructor");

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
    Object testNumber(w, TestStruct, "testNumber");
    CellI& TestStructMethodsIndex = testNumber.__type__()[id.methods][id.index];
    CellI& factorialMethod        = TestStructMethodsIndex[w.name("factorial")][id.value];

    EXPECT_EQ(&testNumber.method(w.name("factorial"), { id.input, _0_ }), &_1_);
    EXPECT_EQ(&testNumber.method(w.name("factorial"), { id.input, _1_ }), &_1_);
    EXPECT_EQ(&testNumber.method(w.name("factorial"), { id.input, _2_ }), &_2_);
    EXPECT_EQ(&testNumber.method(w.name("factorial"), { id.input, _3_ }), &_6_);
    EXPECT_EQ(&testNumber.method(w.name("factorial"), { id.input, _4_ }), &w.pools.numbers.get(24));
    EXPECT_EQ(&testNumber.method(w.name("factorial"), { id.input, _5_ }), &w.pools.numbers.get(120));

    int liveCells                 = CellI::s_constructed - CellI::s_destructed;
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(&testNumber.method(w.name("factorial"), { id.input, _5_ }), &w.pools.numbers.get(120));
        testNumber.clearStack(factorialMethod);
    }
    liveCells = CellI::s_constructed - CellI::s_destructed - liveCells;
    EXPECT_EQ(liveCells, 0);
    std::cout << "extra cells: " << liveCells << std::endl;
    std::cout << "";
}

TEST_F(CellTest, List)
{
    auto& ListStruct = getStruct(w.templateId("std::List", id.valueType, std.Number));
    Object list(w, ListStruct, w.name("constructor"));

    printAs.value(list);
    printAs.cell(list);
    EXPECT_EQ(&list[id.size], &_0_);
    EXPECT_EQ(&list.method(w.name("size")), &_0_);
    auto& aaaa = list.method(w.name("empty"));
    EXPECT_EQ(&list.method(w.name("empty")), &true_);
    EXPECT_EQ(&list.__type__()[id.typeAliases][id.index][id.valueType][id.value], &std.Number);
    EXPECT_FALSE(list.has(id.first));
    EXPECT_FALSE(list.has(id.last));

    list.method(w.name("add"), { id.value, _1_ });
    EXPECT_EQ(&list[id.size], &_1_);
    EXPECT_EQ(&list.method(w.name("size")), &_1_);
    EXPECT_EQ(&list.method(w.name("empty")), &false_);
    EXPECT_TRUE(list.has(id.first));
    EXPECT_TRUE(list.has(id.last));

    CellI& firstItem = list[id.first];
    EXPECT_EQ(&firstItem, &list[id.last]);
    EXPECT_FALSE(firstItem.has(id.previous));
    EXPECT_FALSE(firstItem.has(id.next));
    EXPECT_EQ(&firstItem[id.value], &_1_);
    printAs.value(list);

    list.method(w.name("add"), { id.value, w.pools.numbers.get(2) });
    EXPECT_EQ(&list[id.size], &_2_);
    EXPECT_EQ(&list.method(w.name("size")), &_2_);
    EXPECT_EQ(&list.method(w.name("empty")), &false_);

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

    list.method(w.name("add"), { id.value, _3_ });
    EXPECT_EQ(&list[id.size], &_3_);
    EXPECT_EQ(&list.method(w.name("size")), &_3_);
    EXPECT_EQ(&list.method(w.name("empty")), &false_);

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

    list.method(w.name("remove"), { id.item, thirdItem });
    EXPECT_EQ(&list[id.size], &_2_);
    EXPECT_EQ(&list.method(w.name("size")), &_2_);
    EXPECT_EQ(&list.method(w.name("empty")), &false_);
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

    list.method(w.name("remove"), { id.item, secondItem });
    EXPECT_EQ(&list[id.size], &_1_);
    EXPECT_EQ(&list.method(w.name("size")), &_1_);
    EXPECT_EQ(&list.method(w.name("empty")), &false_);
    {
        CellI& firstItem = list[id.first];
        EXPECT_EQ(&firstItem, &list[id.last]);
        EXPECT_EQ(firstItem.has(id.previous), false);
        EXPECT_EQ(firstItem.has(id.next), false);
        EXPECT_EQ(&firstItem[id.value], &_1_);
    }
    printAs.value(list);

    list.method(w.name("remove"), { id.item, firstItem });
    EXPECT_EQ(&list[id.size], &_0_);
    EXPECT_EQ(&list.method(w.name("size")), &_0_);
    EXPECT_EQ(&list.method(w.name("empty")), &true_);
    EXPECT_EQ(&list.__type__()[id.typeAliases][id.index][id.valueType][id.value], &std.Number);
    EXPECT_FALSE(list.has(id.first));
    EXPECT_FALSE(list.has(id.last));
    printAs.value(list);

    CellI& size = list.method(w.name("size"));
    printAs.value(size);
}

TEST_F(CellTest, Map)
{
    auto& MapStruct = getStruct(w.templateId("std::Map", id.keyType, std.Cell, id.valueType, std.Slot));
    Object map(w, MapStruct, w.name("constructor"));

    printAs.value(map);
    printAs.cell(map);
    EXPECT_EQ(&map[id.size], &_0_);
    EXPECT_EQ(&map.method(w.name("size")), &_0_);
    EXPECT_EQ(&map.method(w.name("empty")), &true_);
    EXPECT_EQ(&map.__type__()[id.typeAliases][id.index][id.keyType][id.value], &std.Cell);
    EXPECT_EQ(&map.__type__()[id.typeAliases][id.index][id.valueType][id.value], &std.Slot);

    map.method(w.name("add"), { id.key, _1_ }, { id.value, id.red });
    printAs.value(map);
    printAs.cell(map);
    EXPECT_EQ(&map[id.size], &_1_);
    EXPECT_EQ(&map.method(w.name("size")), &_1_);
    EXPECT_EQ(&map[id.list][id.size], &_1_);
    EXPECT_EQ(&map[id.list][id.first][id.value], &id.red);
    EXPECT_EQ(&map[id.list][id.first], &map[id.list][id.last]);
    EXPECT_TRUE(map[id.index].has(_1_));
    EXPECT_EQ(&map[id.index][_1_][id.value], &id.red);
    EXPECT_EQ(&map.method(w.name("empty")), &false_);
    EXPECT_TRUE(map[id.index][id.__type__][id.memberOf][id.index].has(std.Index));

    map.method(w.name("add"), { id.key, _2_ }, { id.value, id.green });
    map.method(w.name("add"), { id.key, _3_ }, { id.value, id.blue });
    EXPECT_EQ(&map[id.index][_1_][id.value], &id.red);
    EXPECT_EQ(&map[id.index][_2_][id.value], &id.green);
    EXPECT_EQ(&map[id.index][_3_][id.value], &id.blue);
    printAs.value(map);
    printAs.cell(map);
}

TEST_F(CellTest, MapTypes)
{
    auto& MapStruct = getStruct(w.templateId("std::Map", id.keyType, std.Cell, id.valueType, std.Slot));
    Object map(w, MapStruct, w.name("constructor"));

    printAs.value(map.__type__());
    printAs.value(map[id.list].__type__());
    printAs.value(map[id.index], "map[ids.index]");
    printAs.value(map[id.index].__type__(), "map[ids.index].struct_()");
    map.method(w.name("add"), { id.key, _1_ }, { id.value, id.red });
    printAs.value(map[id.index], "map[ids.index]");
    printAs.value(map[id.index].__type__(), "map[ids.index].struct_()");
    printAs.cell(map[id.index].__type__()[id.slots], "map[ids.index].struct_()[ids.slots]");
    printAs.cell(map[id.index].__type__()[id.slots][id.index], "map[ids.index].struct_()[ids.slots][ids.index]");
    printAs.value(map[id.index].__type__()[id.slots][id.index], "map[ids.index].struct_()[ids.slots][ids.index]");
}

TEST_F(CellTest, BuiltInType)
{
    std::string testTypeName = "Test1234";
    Struct type(w, testTypeName);

    EXPECT_EQ(&type[id.__type__], &std.Struct);
    EXPECT_EQ(&type[id.name], &w.name(testTypeName));
    EXPECT_EQ(&type[id.name], &w.name(testTypeName)); // query twice to trigger cache
}

TEST_F(CellTest, BuiltInMap)
{
    Map map(w, std.Number, std.Color);

    printAs.value(map.__type__());
    printAs.value(map[id.list].__type__());
    printAs.cell(map[id.index], "map[ids.index]");
    printAs.value(map[id.index], "map[ids.index]");
    printAs.value(map[id.index].__type__(), "map[ids.index].struct_()");
    map.add(_1_, id.red);
    printAs.value(map, "map");
    printAs.cell(map[id.index], "map[ids.index]");
    printAs.value(map[id.index], "map[ids.index]");
    printAs.value(map[id.index].__type__(), "map[ids.index].struct_()");
    printAs.cell(map[id.index].__type__()[id.slots], "map[ids.index].struct_()[ids.slots]");
    EXPECT_EQ(&map[id.index][_1_], &map[id.list][id.first]);
    EXPECT_EQ(&map[id.index].__type__()[id.slots][id.size], &_1_);
    EXPECT_EQ(&map[id.index].__type__()[id.slots][id.list][id.size], &_1_);
    EXPECT_EQ(&map[id.index].__type__()[id.slots][id.index][_1_][id.value][id.key], &_1_);
    EXPECT_EQ(&map[id.index].__type__()[id.slots][id.index][_1_][id.value][id.type], &std.Slot);

    EXPECT_TRUE(map[id.index].__type__().has(id.slots));
    EXPECT_TRUE(map[id.index].__type__()[id.slots].has(id.index));
    EXPECT_TRUE(map[id.index].__type__()[id.slots][id.index].has(id.__type__));
    EXPECT_EQ(&map[id.index].__type__()[id.slots][id.index].__type__(), &map[id.index].__type__()[id.slots][id.index].__type__()[id.slots][id.index].__type__());

    EXPECT_TRUE(map[id.index].__type__()[id.slots][id.index].__type__().has(id.slots));
    printAs.cell(map[id.index].__type__()[id.slots], "map[ids.index].struct_()[ids.slots][]");
    printAs.cell(map[id.index].__type__()[id.slots][id.index], "map[ids.index].struct_()[ids.slots][ids.index]");
    printAs.value(map[id.index].__type__()[id.slots][id.index].__type__(), "map[ids.index].struct_()[ids.slots][ids.index].struct_()");
    printAs.value(map[id.index].__type__()[id.slots][id.index].__type__()[id.slots], "map[ids.index].struct_()[ids.slots][ids.index].struct_()[ids.slots]");
    EXPECT_EQ(&map[id.index].__type__()[id.slots][id.index].__type__()[id.slots][id.size], &_1_);
    EXPECT_EQ(&map[id.index].__type__()[id.slots][id.index].__type__()[id.slots][id.index][_1_][id.value][id.key], &_1_);
    CellI& debug = map[id.index].__type__()[id.slots][id.index].__type__()[id.slots][id.index][_1_][id.value][id.type];
    EXPECT_EQ(&map[id.index].__type__()[id.slots][id.index].__type__()[id.slots][id.index][_1_][id.value][id.type], &std.Slot);
    printAs.value(map[id.index].__type__()[id.slots][id.index].__type__()[id.slots][id.index], "map[ids.index].struct_()[ids.slots][ids.index].struct_()[ids.slots][ids.index]");
    printAs.value(map[id.index].__type__()[id.slots][id.index].__type__()[id.slots][id.index].__type__(), "map[ids.index].struct_()[ids.slots][ids.index].struct_()[ids.slots][ids.index].struct_()");

    map.remove(_1_);
    printAs.value(map, "map");
}

TEST_F(CellTest, MapTemplateTypes)
{
    auto& MapNumberToColor = getStruct(w.templateId("std::Map", id.keyType, std.Number, id.valueType, std.Color));
    Object map(w, MapNumberToColor, w.name("constructor"));

    printAs.value(map.__type__());
    printAs.value(map[id.list].__type__());
    printAs.cell(map[id.index], "map[ids.index]");
    printAs.value(map[id.index], "map[ids.index]");
    printAs.value(map[id.index].__type__(), "map[ids.index].struct_()");
    map.method(w.name("add"), { id.key, _1_ }, { id.value, id.red });
    printAs.value(map, "map");
    printAs.cell(map[id.index], "map[ids.index]");
    printAs.value(map[id.index], "map[ids.index]");
    printAs.value(map[id.index].__type__(), "map[ids.index].struct_()");
    printAs.cell(map[id.index].__type__()[id.slots], "map[ids.index].struct_()[ids.slots]");
    EXPECT_EQ(&map[id.index][_1_], &map[id.list][id.first]);
    EXPECT_EQ(&map[id.index].__type__()[id.slots][id.size], &_1_);
    EXPECT_EQ(&map[id.index].__type__()[id.slots][id.list][id.size], &_1_);
    EXPECT_EQ(&map[id.index].__type__()[id.slots][id.index][_1_][id.value][id.key], &_1_);
    CellI& debug2 = map[id.index].__type__()[id.slots][id.index][_1_][id.value][id.type];
    EXPECT_EQ(&map[id.index].__type__()[id.slots][id.index][_1_][id.value][id.type], &std.Slot);

    EXPECT_TRUE(map[id.index].__type__().has(id.slots));
    EXPECT_TRUE(map[id.index].__type__()[id.slots].has(id.index));
    EXPECT_TRUE(map[id.index].__type__()[id.slots][id.index].has(id.__type__));
    EXPECT_EQ(&map[id.index].__type__()[id.slots][id.index].__type__(), &map[id.index].__type__()[id.slots][id.index].__type__()[id.slots][id.index].__type__());

    EXPECT_TRUE(map[id.index].__type__()[id.slots][id.index].__type__().has(id.slots));
    printAs.cell(map[id.index].__type__()[id.slots], "map[ids.index].struct_()[ids.slots][]");
    printAs.cell(map[id.index].__type__()[id.slots][id.index], "map[ids.index].struct_()[ids.slots][ids.index]");
    printAs.value(map[id.index].__type__()[id.slots][id.index].__type__(), "map[ids.index].struct_()[ids.slots][ids.index].struct_()");
    printAs.value(map[id.index].__type__()[id.slots][id.index].__type__()[id.slots], "map[ids.index].struct_()[ids.slots][ids.index].struct_()[ids.slots]");
    EXPECT_EQ(&map[id.index].__type__()[id.slots][id.index].__type__()[id.slots][id.size], &_1_);
    EXPECT_EQ(&map[id.index].__type__()[id.slots][id.index].__type__()[id.slots][id.index][_1_][id.value][id.key], &_1_);
    CellI& debug = map[id.index].__type__()[id.slots][id.index].__type__()[id.slots][id.index][_1_][id.value][id.type];
    EXPECT_EQ(&map[id.index].__type__()[id.slots][id.index].__type__()[id.slots][id.index][_1_][id.value][id.type], &std.Slot);
    printAs.value(map[id.index].__type__()[id.slots][id.index].__type__()[id.slots][id.index], "map[ids.index].struct_()[ids.slots][ids.index].struct_()[ids.slots][ids.index]");
    printAs.value(map[id.index].__type__()[id.slots][id.index].__type__()[id.slots][id.index].__type__(), "map[ids.index].struct_()[ids.slots][ids.index].struct_()[ids.slots][ids.index].struct_()");
}

TEST_F(CellTest, MapNumberToColor)
{
    auto& MapNumberToColor = getStruct(w.templateId("std::Map", id.keyType, std.Number, id.valueType, std.Color));
    Object map(w, MapNumberToColor, w.name("constructor"));

    EXPECT_EQ(&map.__type__()[id.typeAliases][id.index][id.keyType][id.value], &std.Number);
    EXPECT_EQ(&map.__type__()[id.typeAliases][id.index][id.valueType][id.value], &std.Color);

    printAs.value(map);
    printAs.cell(map);
    EXPECT_EQ(&map[id.size], &_0_);
    EXPECT_EQ(&map.method(w.name("size")), &_0_);
    EXPECT_EQ(&map.method(w.name("empty")), &true_);

    map.method(w.name("add"), { id.key, _1_}, { id.value, id.red });
    printAs.value(map);
    printAs.cell(map);
    EXPECT_EQ(&map[id.size], &_1_);
    EXPECT_EQ(&map.method(w.name("size")), &_1_);
    EXPECT_EQ(&map[id.list][id.size], &_1_);
    EXPECT_EQ(&map[id.list][id.first][id.value], &id.red);
    EXPECT_EQ(&map[id.list][id.first], &map[id.list][id.last]);
    EXPECT_TRUE(map[id.index].has(_1_));
    EXPECT_EQ(&map[id.index][_1_][id.value], &id.red);
    EXPECT_EQ(&map.method(w.name("empty")), &false_);
    EXPECT_TRUE(map[id.index][id.__type__][id.memberOf][id.index].has(std.Index));

    map.method(w.name("add"), { id.key, _2_ }, { id.value, id.green });
    map.method(w.name("add"), { id.key, _3_ }, { id.value, id.blue });
    EXPECT_EQ(&map[id.index][_1_][id.value], &id.red);
    EXPECT_EQ(&map[id.index][_2_][id.value], &id.green);
    EXPECT_EQ(&map[id.index][_3_][id.value], &id.blue);
    printAs.value(map);
    printAs.cell(map);
}

TEST_F(CellTest, ListItem)
{
    auto& ListItemStruct = getStruct(w.templateId("std::ListItem", id.valueType, std.Color));
    Object listItem(w, ListItemStruct, w.name("constructor"), { id.value, id.green });

    EXPECT_EQ(&listItem[id.value], &id.green);

    EXPECT_EQ(&listItem.__type__()[id.memberOf][id.size], &_1_);
    EXPECT_TRUE(listItem.__type__()[id.memberOf][id.index].has(std.ListItem));

    EXPECT_EQ(&listItem.__type__()[id.slots][id.size], &_3_);
    EXPECT_TRUE(listItem.__type__()[id.slots][id.index].has(id.previous));
    EXPECT_TRUE(listItem.__type__()[id.slots][id.index].has(id.next));
    EXPECT_TRUE(listItem.__type__()[id.slots][id.index].has(id.value));

    EXPECT_TRUE(listItem.__type__()[id.methods][id.index].has(w.name("constructor")));
}

TEST_F(CellTest, ListItemTemplate)
{
    auto& ListItemNumber  = getStruct(w.templateId("std::ListItem", id.valueType, std.Number));
    Object listItemNumber(w, ListItemNumber, w.name("constructor"), { id.value, _1_ });

    EXPECT_EQ(&listItemNumber[id.value], &_1_);

    printAs.value(std.ListItem, "type.ListItem");
    printAs.value(ListItemNumber, "listItemNumber");
    printAs.value(std.ListItem[id.slots][id.list], "type.ListItem[slots]");
    printAs.value(ListItemNumber[id.slots][id.list], "listItemNumber[slots]");

    EXPECT_EQ(&ListItemNumber[id.memberOf][id.size], &_1_);
    EXPECT_TRUE(ListItemNumber[id.memberOf][id.index].has(std.ListItem));

    EXPECT_EQ(&ListItemNumber[id.slots][id.size], &_3_);
    EXPECT_TRUE(ListItemNumber[id.slots][id.index].has(id.previous));
    EXPECT_TRUE(ListItemNumber[id.slots][id.index].has(id.next));
    EXPECT_TRUE(ListItemNumber[id.slots][id.index].has(id.value));

    EXPECT_TRUE(ListItemNumber.has(id.methods));
}

TEST_F(CellTest, ListTemplate)
{
    auto& ListOfNumbers = getStruct(w.templateId("std::List", id.valueType, std.Number));

    EXPECT_EQ(&ListOfNumbers[id.typeAliases][id.size], &_2_);
    EXPECT_TRUE(ListOfNumbers[id.typeAliases][id.index].has(id.valueType));
    EXPECT_EQ(&ListOfNumbers[id.typeAliases][id.index][id.valueType][id.value], &std.Number);
    EXPECT_TRUE(ListOfNumbers[id.typeAliases][id.index].has(id.itemType));
    CellI& ListItemType = ListOfNumbers[id.typeAliases][id.index][id.itemType][id.value];
    EXPECT_EQ(&ListItemType[id.slots][id.index][id.value][id.value][id.type], &std.Number);
    EXPECT_NE(&ListItemType, &std.ListItem);
    EXPECT_EQ(&ListItemType[id.memberOf][id.size], &_1_);
    EXPECT_TRUE(ListItemType[id.memberOf][id.index].has(std.ListItem));

    EXPECT_EQ(&ListItemType[id.slots][id.size], &_3_);
    EXPECT_TRUE(ListItemType[id.slots][id.index].has(id.previous));
    EXPECT_TRUE(ListItemType[id.slots][id.index].has(id.next));
    EXPECT_TRUE(ListItemType[id.slots][id.index].has(id.value));

    EXPECT_EQ(&ListOfNumbers[id.slots][id.size], &_3_);
    EXPECT_TRUE(ListOfNumbers[id.slots][id.index].has(id.first));
    EXPECT_EQ(&ListOfNumbers[id.slots][id.index][id.first][id.value][id.type], &ListItemType);

    EXPECT_TRUE(ListOfNumbers[id.slots][id.index].has(id.last));
    EXPECT_EQ(&ListOfNumbers[id.slots][id.index][id.last][id.value][id.type], &ListItemType);

    EXPECT_TRUE(ListOfNumbers[id.slots][id.index].has(id.size));
    EXPECT_EQ(&ListOfNumbers[id.slots][id.index][id.size][id.value][id.type], &std.Number);

    EXPECT_TRUE(ListOfNumbers[id.memberOf][id.index].has(std.List));
    EXPECT_EQ(&ListOfNumbers[id.memberOf][id.size], &_2_);

    printAs.value(std.List, "type.List");
    printAs.value(ListOfNumbers, "ListOfNumbers");
}

TEST_F(CellTest, FunctionTypes)
{
    CellI& function = std.Struct[id.methods][id.index][w.name("addSlot")][id.value];
    printAs.cell(function.__type__(), "function");
    printAs.cell(function[id.localVars], "function[ids.localVars]");
    printAs.value(function[id.localVars], "function[ids.localVars]");
}

TEST_F(CellTest, HybridGrid)
{
    nativearc::Grid inputGrid("input");
    inputGrid.loadFromJsonArray("[[0, 7, 0], [7, 7, 7], [0, 7, 0]]");
    hybridarc::Grid grid(w, inputGrid);

    printAs.value(grid[id.pixels]);

    EXPECT_EQ(&grid[id.__type__], &std.Grid);
    EXPECT_EQ(&grid[id.width], &w.pools.numbers.get(3));
    EXPECT_EQ(&grid[id.height], &w.pools.numbers.get(3));
    EXPECT_EQ(&grid[id.pixels][id.__type__], &w.ListOf(std.Pixel));

    auto& ListOfPixels  = getStruct(w.templateId("std::List", id.valueType, std.Pixel));
    Object listOfPixels(w, ListOfPixels, w.name("constructor"), "listOfPixels");
    listOfPixels.method(w.name("add"), { id.value, grid[id.pixels][id.first][id.value] });
    listOfPixels.method(w.name("add"), { id.value, grid[id.pixels][id.first][id.next][id.value] });
    printAs.value(listOfPixels);
}

TEST_F(CellTest, BasicObjectTest)
{
    auto& Type_ = getStruct("std::Struct");
    auto& List = getStruct(w.templateId("std::List", id.valueType, std.Cell));
    Object testType(w, Type_, "Test");
    Object emptyList(w, List, w.name("constructor"));

    testType.method(w.name("addSlots"), { id.list, emptyList });

    testType.method(w.name("addSlots"), { id.list, w.list(
        std.slot(id.result, std.Digit),
        std.slot(id.value, std.Number))}); // TODO implement type checking

    Object object(w, testType, "testObject");

    EXPECT_EQ(object.label(), "testObject");
    EXPECT_EQ(&object.__type__(), &testType);

    EXPECT_ANY_THROW(&object[id.value]);
    EXPECT_NO_THROW(object.set(id.value, w.pools.numbers.get(42)));
    printAs.value(object[id.value]);
    EXPECT_ANY_THROW(object.set(id.argument, w.pools.numbers.get(42)));
}

TEST_F(CellTest, BasicControlOpTest)
{
    Object testValue1(w, std.op.ConstVar);
    testValue1.set(id.value, std.Char);

    Object testValue2(w, std.op.ConstVar);
    testValue2.set(id.value, std.Color);

    Object sameOpEq(w, std.op.Same, "sameOpEq");
    sameOpEq.set(id.lhs, testValue1);
    sameOpEq.set(id.rhs, testValue1);

    Object sameOpNe(w, std.op.Same, "sameOpNe");
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
    Object start(w, std.op.ConstVar);
    start.set(id.value, w.pools.numbers.get(42));

    Object value10(w, std.op.ConstVar);
    value10.set(id.value, w.pools.numbers.get(10));

    Object add10(w, std.op.Add, "add10");
    add10.set(id.lhs, start);
    add10.set(id.rhs, value10);
    add10();
    std::cout << "42 + 10 = ";
    printAs.value(add10[id.value]);

    EXPECT_EQ(&add10[id.value], &w.pools.numbers.get(52));
}

TEST_F(CellTest, CreatingCustomType)
{
    Object colorRed(w, std.Cell, "red");
    Object colorGreen(w, std.Cell, "green");
    Object colorBlue(w, std.Cell, "blue");

    Object colorClass(w, std.Struct, "Color");
#if 1 // TODO
    auto& MapCellToSlot = getStruct(w.templateId("std::Map", id.keyType, std.Cell, id.valueType, std.Slot));
    Object* slotMapPtr  = new Object(w, MapCellToSlot, w.name("constructor"));
    colorClass.set(id.slots, *slotMapPtr);
#endif

    colorClass.method(w.name("addSlots"), { id.list, w.list(std.slot(colorRed, std.Number), std.slot(colorGreen, std.Number), std.slot(colorBlue, std.Number)) });

    Object redColor(w, colorClass, "redColor");
    redColor.set(colorRed, w.pools.numbers.get(255));
    redColor.set(colorGreen, w.pools.numbers.get(0));
    redColor.set(colorBlue, w.pools.numbers.get(0));

    printAs.value(colorClass, "colorClass:");
    printAs.value(colorClass[id.slots][id.list], "colorClass::slots");
    printAs.value(colorClass[id.slots][id.index][colorRed]);
    EXPECT_TRUE(colorClass[id.slots][id.index].has(colorRed));
    EXPECT_TRUE(colorClass[id.slots][id.index].has(colorGreen));
    EXPECT_TRUE(colorClass[id.slots][id.index].has(colorBlue));
    printAs.value(colorClass[id.slots][id.index][colorRed][id.__type__], "colorClass slot of colorRed");
    EXPECT_EQ(&colorClass[id.slots][id.index][colorRed][id.value][id.__type__], &std.Slot);
    EXPECT_EQ(&colorClass[id.slots][id.index][colorRed][id.value][id.__type__], &std.Slot);
    EXPECT_EQ(&colorClass[id.slots][id.index][colorGreen][id.value][id.__type__], &std.Slot);
    EXPECT_EQ(&colorClass[id.slots][id.index][colorBlue][id.value][id.__type__], &std.Slot);
    printAs.value(redColor);

    printAs.cell(redColor);
    printAs.cell(colorClass[id.slots][id.index][colorRed][id.value], "colorClass[ids.slots][ids.index][colorRed][ids.value]");
    printAs.cell(colorClass[id.slots][id.index], "colorClass[ids.slots][ids.index]");

    printAs.value(colorClass[id.slots][id.index], "colorClass[ids.slots][ids.index]");
    printAs.value(colorClass[id.slots][id.index][id.__type__], "colorClass[ids.slots][ids.index][ids.type]");
    printAs.value(colorClass[id.slots][id.index][id.__type__][id.slots], "colorClass[ids.slots][ids.index][ids.type][ids.slots]");
    printAs.value(colorClass[id.slots][id.index][id.__type__][id.slots][id.index], "colorClass[ids.slots][ids.index][ids.type][ids.slots][ids.index]");
    printAs.cell(colorClass);
}

TEST_F(CellTest, CreatingNumber)
{
    Number& number_0   = w.pools.numbers.get(0);
    Number& number_255 = w.pools.numbers.get(255);

    printAs.value(number_255);
    printAs.value(number_255[id.value][id.first][id.value]);

    printAs.cell(number_255);
    printAs.cell(number_255[w.numbers.sign]);
    printAs.cell(number_255[id.value]);
    printAs.cell(number_255[id.value][id.first]);
    printAs.cell(number_255[id.value][id.first][id.value]);
    printAs.cell(number_255[id.value][id.last]);
    printAs.cell(number_255[id.value][id.size]);
}

TEST_F(CellTest, NextgenList)
{
    List list(w, std.Number);
    EXPECT_FALSE(list.has(id.first));
    EXPECT_FALSE(list.has(id.last));
    EXPECT_EQ(&list[id.size], &_0_);
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

    list.remove(&item2);
    EXPECT_TRUE(list.has(id.first));
    EXPECT_TRUE(list.has(id.last));
    EXPECT_EQ(&list[id.size], &_2_);
    EXPECT_FALSE(list.empty());
    printAs.value(list);

    list.remove(&item1);
    EXPECT_TRUE(list.has(id.first));
    EXPECT_TRUE(list.has(id.last));
    EXPECT_EQ(&list[id.size], &_1_);
    EXPECT_FALSE(list.empty());
    printAs.value(list);

    list.remove(&item3);
    EXPECT_FALSE(list.has(id.first));
    EXPECT_FALSE(list.has(id.last));
    EXPECT_EQ(&list[id.size], &_0_);
    EXPECT_TRUE(list.empty());
    printAs.value(list);
}


TEST_F(CellTest, NextgenType)
{
    Map map(w, std.Number, std.Color);
    map.add(_1_, id.blue);
    printAs.value(map, "Map<Number, Color>");

    printAs.value(map[id.index][id.__type__][id.slots][id.list], "map[ids.index][ids.type][ids.slots][ids.list]");
    printAs.value(map[id.index][id.__type__][id.slots][id.index], "map[ids.index][ids.type][ids.slots][ids.list]");
    printAs.value(map[id.index][id.__type__][id.slots][id.index][id.__type__][id.slots][id.list], "map[ids.index][ids.type][ids.slots][ids.index][ids.type][ids.slots][ids.list]");
    printAs.value(map[id.index][id.__type__][id.slots][id.index][id.__type__][id.slots][id.index], "map[ids.index][ids.type][ids.slots][ids.index][ids.type][ids.slots][ids.index]");
    EXPECT_EQ(&map[id.index][id.__type__], &map[id.index][id.__type__][id.slots][id.index][id.__type__]);

    Index index(w);
    index.set(std.Number, std.Color);
    printAs.value(index, "Index");

    Struct __type__(w, "__type__");
    __type__.addSlot(_1_, std.slot(_1_, std.Number));
    printAs.value(__type__);

    Set set(w, std.Number);
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
    auto& MapNumberToColor = getStruct(w.templateId("std::Map", id.keyType, std.Number, id.valueType, std.Color));
    Object map(w, MapNumberToColor, w.name("constructor"));

    EXPECT_EQ(&map[id.size], &_0_);
    EXPECT_EQ(&map.method(w.name("size")), &_0_);
    EXPECT_EQ(&map.method(w.name("empty")), &true_);
    printAs.value(map.__type__());
    printAs.value(map[id.list].__type__());
    printAs.value(map[id.index], "map[ids.index]");
    printAs.value(map[id.index].__type__(), "map[ids.index].struct_()");

    map.method(w.name("add"), { id.key, _1_ }, { id.value, id.red });
    printAs.value(map[id.index], "map[ids.index]");
    printAs.value(map[id.index].__type__(), "map[ids.index].struct_()");
    printAs.cell(map[id.index].__type__()[id.slots], "map[ids.index].struct_()[ids.slots]");
    printAs.cell(map[id.index].__type__()[id.slots][id.index], "map[ids.index].struct_()[ids.slots][ids.index]");
    printAs.value(map[id.index].__type__()[id.slots][id.index], "map[ids.index].struct_()[ids.slots][ids.index]");
    EXPECT_EQ(&map[id.size], &_1_);
    EXPECT_EQ(&map.method(w.name("size")), &_1_);
    EXPECT_EQ(&map[id.list][id.size], &_1_);
    EXPECT_EQ(&map[id.list][id.first][id.value], &id.red);
    EXPECT_EQ(&map[id.list][id.first], &map[id.list][id.last]);
    EXPECT_TRUE(map[id.index].has(_1_));
    EXPECT_EQ(&map[id.index][_1_][id.value], &id.red);
    EXPECT_EQ(&map.method(w.name("empty")), &false_);
    EXPECT_TRUE(map[id.index][id.__type__][id.memberOf][id.index].has(std.Index));

    map.method(w.name("add"), { id.key, _2_ }, { id.value, id.green });
    map.method(w.name("add"), { id.key, _3_ }, { id.value, id.blue });
    EXPECT_EQ(&map[id.index][_1_][id.value], &id.red);
    EXPECT_EQ(&map[id.index][_2_][id.value], &id.green);
    EXPECT_EQ(&map[id.index][_3_][id.value], &id.blue);
    printAs.value(map);
    printAs.cell(map);

    map.method(w.name("remove"), { id.key, _3_ });
    map.method(w.name("remove"), { id.key, _2_ });

    EXPECT_EQ(&map[id.size], &_1_);
    EXPECT_EQ(&map.method(w.name("size")), &_1_);
    EXPECT_EQ(&map[id.list][id.size], &_1_);
    EXPECT_EQ(&map[id.list][id.first][id.value], &id.red);
    EXPECT_EQ(&map[id.list][id.first], &map[id.list][id.last]);
    EXPECT_TRUE(map[id.index].has(_1_));
    EXPECT_EQ(&map[id.index][_1_][id.value], &id.red);
    EXPECT_EQ(&map.method(w.name("empty")), &false_);
    EXPECT_TRUE(map[id.index][id.__type__][id.memberOf][id.index].has(std.Index));
}


TEST_F(CellTest, TrieMap)
{
    auto& MapNumberToColor = getStruct(w.templateId("std::TrieMap", id.keyType, std.Number, id.valueType, std.Color));
    Object trieMap(w, MapNumberToColor, w.name("constructor"));

    EXPECT_EQ(&trieMap[id.size], &_0_);
    EXPECT_EQ(&trieMap.method(w.name("size")), &_0_);
    EXPECT_EQ(&trieMap.method(w.name("empty")), &true_);
    printAs.value(trieMap.__type__());
    printAs.value(trieMap[id.list].__type__());

    EXPECT_EQ(&trieMap.method(w.name("hasKey"), { id.key, w.list(_1_, _1_, _1_, _1_) }), &false_);
    trieMap.method(w.name("add"), { id.key, w.list(_1_, _1_, _1_, _1_) }, { id.value, id.red });
    EXPECT_EQ(&trieMap[id.size], &_1_);
    EXPECT_EQ(&trieMap.method(w.name("size")), &_1_);
    EXPECT_EQ(&trieMap[id.list][id.size], &_1_);
    EXPECT_EQ(&trieMap.method(w.name("empty")), &false_);
    CellI& test = trieMap.method(w.name("hasKey"), { id.key, w.list(_1_, _1_, _1_, _1_) });
    EXPECT_EQ(&trieMap.method(w.name("hasKey"), { id.key, w.list(_1_, _1_, _1_, _1_) }), &true_);
    EXPECT_EQ(&trieMap.method(w.name("getValue"), { id.key, w.list(_1_, _1_, _1_, _1_) }), &id.red);

    EXPECT_EQ(&trieMap.method(w.name("hasKey"), { id.key, w.list(_2_, _2_, _2_, _2_) }), &false_);
    trieMap.method(w.name("add"), { id.key, w.list(_2_, _2_, _2_, _2_) }, { id.value, id.green });
    EXPECT_EQ(&trieMap.method(w.name("hasKey"), { id.key, w.list(_2_, _2_, _2_, _2_) }), &true_);

    trieMap.method(w.name("add"), { id.key, w.list(_3_, _3_, _3_, _3_) }, { id.value, id.blue });
    EXPECT_EQ(&trieMap.method(w.name("hasKey"), { id.key, w.list(_3_, _3_, _3_, _3_) }), &true_);
    printAs.value(trieMap);
    printAs.cell(trieMap);

    trieMap.method(w.name("remove"), { id.key, w.list(_3_, _3_, _3_, _3_) });
    EXPECT_EQ(&trieMap.method(w.name("hasKey"), { id.key, w.list(_1_, _1_, _1_, _1_) }), &true_);
    EXPECT_EQ(&trieMap.method(w.name("hasKey"), { id.key, w.list(_2_, _2_, _2_, _2_) }), &true_);
    EXPECT_EQ(&trieMap.method(w.name("hasKey"), { id.key, w.list(_3_, _3_, _3_, _3_) }), &false_);
    trieMap.method(w.name("remove"), { id.key, w.list(_2_, _2_, _2_, _2_) });
    EXPECT_EQ(&trieMap.method(w.name("hasKey"), { id.key, w.list(_1_, _1_, _1_, _1_) }), &true_);
    EXPECT_EQ(&trieMap.method(w.name("hasKey"), { id.key, w.list(_2_, _2_, _2_, _2_) }), &false_);
    EXPECT_EQ(&trieMap.method(w.name("hasKey"), { id.key, w.list(_3_, _3_, _3_, _3_) }), &false_);

    EXPECT_EQ(&trieMap[id.size], &_1_);
    EXPECT_EQ(&trieMap.method(w.name("size")), &_1_);
    EXPECT_EQ(&trieMap[id.list][id.size], &_1_);
    EXPECT_EQ(&trieMap.method(w.name("empty")), &false_);

    trieMap.method(w.name("remove"), { id.key, w.list(_1_, _1_, _1_, _1_) });

    EXPECT_EQ(&trieMap[id.size], &_0_);
    EXPECT_EQ(&trieMap.method(w.name("size")), &_0_);
    EXPECT_EQ(&trieMap[id.list][id.size], &_0_);
    EXPECT_EQ(&trieMap.method(w.name("empty")), &true_);
}

TEST_F(CellTest, Set)
{
    auto& SetOfNumbers = getStruct(w.templateId("std::Set", id.valueType, std.Number));
    Object set(w, SetOfNumbers, w.name("constructor"));

    EXPECT_EQ(&set[id.size], &_0_);
    EXPECT_EQ(&set.method(w.name("size")), &_0_);
    EXPECT_EQ(&set.method(w.name("empty")), &true_);
    printAs.value(set.__type__());
    printAs.value(set[id.index], "map[ids.index]");
    printAs.value(set[id.index].__type__(), "map[ids.index].struct_()");

    set.method(w.name("add"), { id.value, _1_ });
    printAs.value(set[id.index], "map[ids.index]");
    printAs.value(set[id.index].__type__(), "map[ids.index].struct_()");
    printAs.cell(set[id.index].__type__()[id.slots], "map[ids.index].struct_()[ids.slots]");
    printAs.cell(set[id.index].__type__()[id.slots][id.index], "map[ids.index].struct_()[ids.slots][ids.index]");
    printAs.value(set[id.index].__type__()[id.slots][id.index], "map[ids.index].struct_()[ids.slots][ids.index]");
    EXPECT_EQ(&set[id.size], &_1_);
    EXPECT_EQ(&set.method(w.name("size")), &_1_);
    EXPECT_TRUE(set[id.index].has(_1_));
    EXPECT_EQ(&set[id.index][_1_], &_1_);
    EXPECT_EQ(&set.method(w.name("empty")), &false_);
    EXPECT_EQ(&set.method(w.name("contains"), { "value", _1_ }), &true_);
    EXPECT_TRUE(set[id.index][id.__type__][id.memberOf][id.index].has(std.Index));

    set.method(w.name("add"), { id.value, _2_ });
    set.method(w.name("add"), { id.value, _3_ });
    EXPECT_EQ(&set[id.index][_1_], &_1_);
    EXPECT_EQ(&set[id.index][_2_], &_2_);
    EXPECT_EQ(&set[id.index][_3_], &_3_);
    printAs.value(set);
    printAs.cell(set);

    set.method(w.name("remove"), { id.value, _3_ });
    set.method(w.name("remove"), { id.value, _2_ });

    EXPECT_EQ(&set[id.size], &_1_);
    EXPECT_EQ(&set.method(w.name("size")), &_1_);
    EXPECT_TRUE(set[id.index].has(_1_));
    EXPECT_EQ(&set[id.index][_1_], &_1_);
    EXPECT_EQ(&set.method(w.name("empty")), &false_);
    EXPECT_TRUE(set[id.index][id.__type__][id.memberOf][id.index].has(std.Index));
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
    TrieMap trieMap(w, std.Number, std.Number, "testTrieMap");
    EXPECT_EQ(&trieMap[id.size], &_0_);
    auto& key1   = w.list(_0_, _1_, _2_, _3_, _4_);
    auto& value1 = w.directions.down;
    trieMap.add(key1, value1);
    EXPECT_EQ(&trieMap[id.size], &_1_);
    EXPECT_TRUE(trieMap.hasKey(key1));
    EXPECT_EQ(&trieMap.getValue(key1), &value1);
    trieMap.remove(key1);
    EXPECT_EQ(&trieMap[id.size], &_0_);
    EXPECT_FALSE(trieMap.hasKey(key1));
}

TEST_F(CellTest, StringTest)
{
    String& testStr1 = w.pools.strings.get("test");
    String& testStr2 = w.pools.strings.get("test");
    EXPECT_EQ(&testStr1, &testStr2);
    EXPECT_EQ(&testStr1.__type__(), &std.String);
    EXPECT_EQ(&testStr1[id.value].__type__(), &w.ListOf(std.Char));
    EXPECT_EQ(&testStr1[id.value], &testStr1[id.value]);
    EXPECT_EQ(&testStr1[id.value], &testStr2[id.value]);
    printAs.value(testStr1[id.value]);
    printAs.cell(testStr1[id.value]);
}

static ftxui::Color ftxAlphaColor(255, 255, 255);
static ftxui::Element colorTile(const ftxui::Color& p_color)
{
    if (&p_color == &ftxAlphaColor)
        return ftxui::text(L"╳╳") | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 2) | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 1) | ftxui::color(ftxui::Color::GrayDark) | ftxui::bgcolor(ftxui::Color::GrayLight);
    return ftxui::text("") | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 2) | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 1) | bgcolor(p_color);
}

static ftxui::Element colorTile(infocell::arc::ColorId colorId)
{
    if (colorId == infocell::arc::ColorId::alpha)
        return ftxui::text(L"╳╳") | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 2) | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 1) | ftxui::color(ftxui::Color::GrayDark) | ftxui::bgcolor(ftxui::Color::GrayLight);
    return colorTile(tui::App::arcColors[(int)colorId]);
}

static ftxui::Element renderJsonBoard(const nlohmann::json& inputRow)
{
    ftxui::Elements boardLines;
    for (auto inputRowIt = inputRow.begin(); inputRowIt != inputRow.end(); ++inputRowIt) {
        ftxui::Elements arcSetInputLine;
        for (const int colorValue : *inputRowIt) {
            arcSetInputLine.push_back(colorTile((infocell::arc::ColorId)colorValue));
        }
        boardLines.push_back(hbox(arcSetInputLine));
    }

    return vbox(boardLines);
}

static ftxui::Element renderJsonBoard(hybridarc::Grid& grid)
{
    ftxui::Elements boardLines;
    for (int y = 0; y < grid.height(); ++y) {
        ftxui::Elements arcSetInputLine;
        for (int x = 0; x < grid.width(); ++x) {
            auto& pixel = grid.getPixel(x, y);
            const ftxui::Color& ftxColor = tui::App::arcColors[pixel.color()];
            arcSetInputLine.push_back(colorTile(ftxColor));
        }
        boardLines.push_back(hbox(arcSetInputLine));
    }

    return vbox(boardLines);
}

static void printGrid(hybridarc::Grid& grid)
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
    CellI& shapeColorTag    = shape["color"]["tag"];
    CellI& shapeColorValue  = shape["color"][shapeColorTag];
    int shapeColorNum       = static_cast<Number&>(shapeColorValue).value();

    const ftxui::Color& shapeColor = tui::App::arcColors[shapeColorNum];
    ftxui::Elements boardLines;
    for (int y = 0; y < height; ++y) {
        ftxui::Elements arcSetInputLine;
        for (int x = 0; x < width; ++x) {
            const ftxui::Color* currentColor = &ftxAlphaColor;
            if (currentPixelItem && shapePixelX == x && shapePixelY == y) {
                currentColor = &shapeColor;
                if (currentPixelItem->has("next")) {
                    currentPixelItem = &currentPixelItem->get("next");
                    shapePixelX      = static_cast<Number&>((*currentPixelItem)["value"]["x"]).value();
                    shapePixelY      = static_cast<Number&>((*currentPixelItem)["value"]["y"]).value();
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

    CellI& shapeColorTag           = shape["color"]["tag"];
    CellI& shapeColorValue         = shape["color"][shapeColorTag];
    int shapeColorNum              = static_cast<Number&>(shapeColorValue).value();
    const ftxui::Color& shapeColor = tui::App::arcColors[shapeColorNum];
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

static void debugShapePixels(CellI& frame)
{
    World& w = frame.w;
    ID& id   = w.id;

    static CellI& ShapePixelStruct = w.getStruct("arc::ShapePixel");

    Object& shapePixels    = static_cast<Object&>(frame["shapePixels"]);
    CellI* previousUpPixel = nullptr;
    CellI* upPixel         = nullptr;
    CellI* leftPixel       = nullptr;
    CellI* firstShapePixel = nullptr;
    const int height       = static_cast<Number&>(frame["height"]).value();
    const int width        = static_cast<Number&>(frame["width"]).value();

    for (int y = 0; y < height; ++y) {
        Object& colX = static_cast<Object&>(shapePixels.method(w.name("getValue"), { id.key, w.pools.numbers.get(y) }));
        for (int x = 0; x < width; ++x) {
            CellI& shapePixel = colX.method(w.name("getValue"), { id.key, w.pools.numbers.get(x) });
            if (!firstShapePixel) {
                firstShapePixel = &shapePixel;
            }
            if (leftPixel) {
                (*leftPixel).set("right", shapePixel);
                shapePixel.set("left", *leftPixel);
            }
            if (upPixel) {
                (*upPixel).set("down", shapePixel);
                shapePixel.set("up", *upPixel);
            }

            CellI& currentShape = shapePixel["shape"];
            CellI& pixel        = shapePixel["pixel"];

            if (currentShape.missing("pixels")) {
                currentShape.set("pixels", *new List(w, ShapePixelStruct));
            }
            List& shapePixelList = static_cast<List&>(currentShape["pixels"]);
            shapePixelList.add(shapePixel["pixel"]);

            // stepping
            if (pixel.missing("left")) {
                // first column
                previousUpPixel = &shapePixel;
            }
            if (pixel.has("right")) {
                leftPixel = &shapePixel;
                if (upPixel) {
                    upPixel = &(*upPixel)["right"];
                }
            } else {
                // last column
                leftPixel = nullptr;
                if (previousUpPixel) {
                    upPixel = previousUpPixel;
                }
            }
        }
    }
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
    nativearc::Grid inputGrid1("inputGrid1", "[[0, 7, 7], [7, 7, 7], [0, 7, 7]]");
    hybridarc::Grid grid1(w, inputGrid1);
    printGrid(grid1);
    Object frame1(w, FrameStruct, w.name("constructor"), { "grid", grid1 });
    frame1.method("process");
    debugShapePixels(frame1);
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
    EXPECT_EQ(&vectorShape1_2_2v["x"], &w.pools.numbers.get(-2));
    EXPECT_EQ(&vectorShape1_2_2v["y"], &_1_);
    printVectorShape(vectorShape1_2);
    CellI& rotated_45 = vectorShape1_2.method(w.name("rotate"), { "rotationDir", rotationDir_45 });
    printVectorShape(rotated_45);
    CellI& rotated_90 = vectorShape1_2.method(w.name("rotate"), { "rotationDir", rotationDir_90 });
    printVectorShape(rotated_90);
    CellI& rotated_180 = vectorShape1_2.method(w.name("rotate"), { "rotationDir", rotationDir_180 });
    printVectorShape(rotated_180);
    CellI& rotated_270 = vectorShape1_2.method(w.name("rotate"), { "rotationDir", rotationDir_270 });
    printVectorShape(rotated_270);

    // 7 0 0
    // 0 7 0
    // 0 0 7
    nativearc::Grid inputGrid2("inputGrid2", "[[7, 0, 0], [0, 7, 0], [0, 0, 7]]");
    hybridarc::Grid grid2(w, inputGrid2);
    printGrid(grid2);
    Object frame2(w, FrameStruct, w.name("constructor"), { "grid", grid2 });
    frame2.method("process");
    debugShapePixels(frame2);
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
    nativearc::Grid inputGrid3("inputGrid3", "[" \
                                                     "[7, 0, 7]," \
                                                     "[7, 0, 7]," \
                                                     "[7, 7, 7]]");
    hybridarc::Grid grid3(w, inputGrid3);
    printGrid(grid3);
    Object frame3(w, FrameStruct, w.name("constructor"), { "grid", grid3 });
    frame3.method("process");
    debugShapePixels(frame3);
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

    infocell::arc::Task arcTaskLoader(w, "007bbfb7", jsonTask);
    CellI& arcTask = arcTaskLoader.m_cellTask;

    Object frame(w, frameStruct, w.name("constructor"), { "grid", arcTask["challenge"]});
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

        infocell::arc::Task arcTaskLoader(w, riddleId, jsonRiddle);
        CellI* arcTaskPtr = &arcTaskLoader.m_cellTask;
        for (const auto& cellPath : cellPaths) {
            arcTaskPtr = &(*arcTaskPtr)[cellPath];
        }
        CellI& arcTask = *arcTaskPtr;

        Object frame(w, frameStruct, w.name("constructor"), { "grid", arcTask });
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
    CellI& colorTag                 = hybridColor["tag"];
    CellI& colorValue               = hybridColor[colorTag];
    infocell::arc::ColorId colorNum = static_cast<infocell::arc::ColorId>(static_cast<Number&>(colorValue).value());

    return infocell::arc::getArcColorName(colorNum);
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

        infocell::arc::Task arcTaskLoader(w, riddleId, jsonRiddle);
        {
            CellI* arcTaskPtr = &arcTaskLoader.m_cellTask;
            for (const auto& cellPath : cellInputPath) {
                arcTaskPtr = &(*arcTaskPtr)[cellPath];
            }
            inputGridPtr = arcTaskPtr;
            CellI& arcTask  = *arcTaskPtr;

            Object frame(w, frameStruct, w.name("constructor"), { "grid", arcTask });
            frame.method("process");
            debugShapePixels(frame);
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

            Object frame(w, frameStruct, w.name("constructor"), { "grid", arcTask });
            frame.method("process");
            debugShapePixels(frame);
            outputShapesPtr = &frame["shapes"];
            printAs.value(frame["shapes"]["size"], "frame[shapes][size]");
            printShapeList(frame["shapes"]);
        }
        CellI& inputGrid     = *inputGridPtr;
        CellI& inputShapes   = *inputShapesPtr;
        CellI& outputGrid    = *outputGridPtr;
        CellI& outputShapes  = *outputShapesPtr;
        int inputGridWidth   = static_cast<Number&>(inputGrid[id.width]).value();
        int outputGridWidth  = static_cast<Number&>(outputGrid[id.width]).value();
        int inputGridHeight  = static_cast<Number&>(inputGrid[id.height]).value();
        int outputGridHeight = static_cast<Number&>(outputGrid[id.height]).value();
        int inputShapesNum   = static_cast<Number&>(inputShapes[id.size]).value();
        int outputShapesNum  = static_cast<Number&>(outputShapes[id.size]).value();
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
    infocell::arc::TaskSet taskSet(w, INFOCELL_ARCPRIZE_PATH INFOCELL_ARC_PRIZE_TRAINING_CHALLENGES_FILENAME);
    for (auto& task : taskSet.m_tasks) {
        std::cout <<
            "id: " << task.first <<
            ", examples num:" << static_cast<List&>(task.second.m_cellExamplesList).size() <<
            ", tests num:" << static_cast<List&>(task.second.m_cellTestsList).size() << std::endl;
        std::cout <<"   examples:" << std::endl;
        Visitor::visitList(task.second.m_cellExamplesList, [](CellI& example, int i, bool&) {
            std::cout <<
                "    size " << static_cast<hybridarc::Grid&>(example["input"]).width() << "x" << static_cast<hybridarc::Grid&>(example["input"]).height() <<
                " -> " << static_cast<hybridarc::Grid&>(example["output"]).width() << "x" << static_cast<hybridarc::Grid&>(example["output"]).height() << std::endl;
        });
        std::cout << "   tests:" << std::endl;
        Visitor::visitList(task.second.m_cellTestsList, [](CellI& example, int i, bool&) {
            std::cout << "    size " << static_cast<hybridarc::Grid&>(example["input"]).width() << "x" << static_cast<hybridarc::Grid&>(example["input"]).height() << std::endl;
        });
    }
}

TEST_F(CellTest, LoadThoseArcTaskWhereInputSizeEqOutputSize)
{
    static const std::string arcFilePath = INFOCELL_ARCPRIZE_PATH INFOCELL_ARC_PRIZE_TRAINING_CHALLENGES_FILENAME;
    infocell::arc::TaskSet taskSet(w, INFOCELL_ARCPRIZE_PATH INFOCELL_ARC_PRIZE_TRAINING_CHALLENGES_FILENAME);
    for (auto& task : taskSet.m_tasks) {
        bool allSameSize = false;
        Visitor::visitList(task.second.m_cellExamplesList, [&allSameSize](CellI& example, int i, bool& stop) {
            int inputWidth  = static_cast<hybridarc::Grid&>(example["input"]).width();
            int inputHeight = static_cast<hybridarc::Grid&>(example["input"]).height();
            int outputWidth = static_cast<hybridarc::Grid&>(example["output"]).width();
            int outputHeight = static_cast<hybridarc::Grid&>(example["output"]).height();
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

        infocell::arc::Task arcTaskLoader(w, riddleId, jsonRiddle);
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
            List pixelContent(w, std.Pixel);
            pixelContent.add(arcPixel[w.coordinates.x]);
            pixelContent.add(arcPixel[w.coordinates.y]);

            CellI& outputColor = static_cast<TrieMap&>(outputGrid["pixelsMap"]).getValue(pixelContent);
            if (&outputColor != &arcPixel["color"])
            {
                std::cout << "DDDD removedPixel: " << fmt::format("[{}, {}, {}]\n", arcPixel["x"].label(), arcPixel["y"].label(), arcPixel["color"].label());
            }
        });
        Visitor::visitList(outputGrid["pixels"], [this, &inputGrid](CellI& arcPixel, int, bool&) {
            //                std::cout << fmt::format("[{}, {}]", arcPixel["x"].label(), arcPixel["y"].label());
            // pixelHashList = arcPixel.hashList();
            List pixelContent(w, std.Pixel);
            pixelContent.add(arcPixel[w.coordinates.x]);
            pixelContent.add(arcPixel[w.coordinates.y]);

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

            infocell::arc::Task arcTaskLoader(w, it.key(), jsonRiddle);
            {
                CellI* arcTaskPtr = &arcTaskLoader.m_cellTask;
                for (const auto& cellPath : cellInputPath) {
                    arcTaskPtr = &(*arcTaskPtr)[cellPath];
                }
                inputGridPtr = arcTaskPtr;

                Object frame(w, frameStruct, w.name("constructor"), { "grid", *inputGridPtr });
                frame.method("process");
                debugShapePixels(frame);
                // printAs.value(frame["shapes"]["size"], "frame[shapes][size]");
                // printShapeList(frame["shapes"]);
            }
            {
                CellI* arcTaskPtr = &arcTaskLoader.m_cellTask;
                for (const auto& cellPath : cellOutputPath) {
                    arcTaskPtr = &(*arcTaskPtr)[cellPath];
                }
                outputGridPtr = arcTaskPtr;

                Object frame(w, frameStruct, w.name("constructor"), { "grid", *outputGridPtr });
                frame.method("process");
                debugShapePixels(frame);
                // printAs.value(frame["shapes"]["size"], "frame[shapes][size]");
                // printShapeList(frame["shapes"]);
            }
            CellI& inputGrid  = *inputGridPtr;
            CellI& outputGrid = *outputGridPtr;
            if (&inputGrid[id.width] == &outputGrid[id.width] && &inputGrid[id.height] == &outputGrid[id.height]) {
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
    std::cout << "Tick count: " << CellTest::getWorld().ap.m_time.value() << std::endl;
    CellTest::freeWorld();

    return ret;
}