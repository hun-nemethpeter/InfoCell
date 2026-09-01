#pragma once

#include <gtest/gtest.h>

#include "cells/World.h"
#include "cells/Compiler.h"
#include "cells/NodeBase.h"
#include "TestLib.h"

namespace testing::internal {

template <>
class UniversalPrinter<infocell::cells::CellI*>
{
public:
    static void Print(const infocell::cells::CellI* val, ::std::ostream* os)
    {
        if (!val) {
            return;
        }
        *os << val->label();
    }
};

#define IMPLEMENT_GTEST_PRINTER_FOR(x)                      \
    template <>                                             \
    class UniversalPrinter<x*>                              \
    {                                                       \
    public:                                                 \
        static void Print(const x* val, ::std::ostream* os) \
        {                                                   \
            *os << val->label();                            \
        }                                                   \
    };

IMPLEMENT_GTEST_PRINTER_FOR(infocell::cells::Std::Op::EActivate);
IMPLEMENT_GTEST_PRINTER_FOR(infocell::cells::Std::Op::EAdd);
IMPLEMENT_GTEST_PRINTER_FOR(infocell::cells::Std::Op::EAnd);
IMPLEMENT_GTEST_PRINTER_FOR(infocell::cells::Std::Op::EBlock);
IMPLEMENT_GTEST_PRINTER_FOR(infocell::cells::Std::Op::ECall);
IMPLEMENT_GTEST_PRINTER_FOR(infocell::cells::Std::Op::EDelete);
IMPLEMENT_GTEST_PRINTER_FOR(infocell::cells::Std::Op::EDivide);
IMPLEMENT_GTEST_PRINTER_FOR(infocell::cells::Std::Op::EDo);
IMPLEMENT_GTEST_PRINTER_FOR(infocell::cells::Std::Op::EEqual);
IMPLEMENT_GTEST_PRINTER_FOR(infocell::cells::Std::Op::EErase);
IMPLEMENT_GTEST_PRINTER_FOR(infocell::cells::Std::Op::EFunction);
IMPLEMENT_GTEST_PRINTER_FOR(infocell::cells::Std::Op::EGet);
IMPLEMENT_GTEST_PRINTER_FOR(infocell::cells::Std::Op::EGreaterThan);
IMPLEMENT_GTEST_PRINTER_FOR(infocell::cells::Std::Op::EGreaterThanOrEqual);
IMPLEMENT_GTEST_PRINTER_FOR(infocell::cells::Std::Op::EHas);
IMPLEMENT_GTEST_PRINTER_FOR(infocell::cells::Std::Op::EIf);
IMPLEMENT_GTEST_PRINTER_FOR(infocell::cells::Std::Op::ELessThan);
IMPLEMENT_GTEST_PRINTER_FOR(infocell::cells::Std::Op::ELessThanOrEqual);
IMPLEMENT_GTEST_PRINTER_FOR(infocell::cells::Std::Op::EMissing);
IMPLEMENT_GTEST_PRINTER_FOR(infocell::cells::Std::Op::EMultiply);
IMPLEMENT_GTEST_PRINTER_FOR(infocell::cells::Std::Op::ENew);
IMPLEMENT_GTEST_PRINTER_FOR(infocell::cells::Std::Op::ENot);
IMPLEMENT_GTEST_PRINTER_FOR(infocell::cells::Std::Op::ENotEqual);
IMPLEMENT_GTEST_PRINTER_FOR(infocell::cells::Std::Op::ENotSame);
IMPLEMENT_GTEST_PRINTER_FOR(infocell::cells::Std::Op::EOr);
IMPLEMENT_GTEST_PRINTER_FOR(infocell::cells::Std::Op::EReturn);
IMPLEMENT_GTEST_PRINTER_FOR(infocell::cells::Std::Op::ESame);
IMPLEMENT_GTEST_PRINTER_FOR(infocell::cells::Std::Op::ESet);
IMPLEMENT_GTEST_PRINTER_FOR(infocell::cells::Std::Op::EState);
IMPLEMENT_GTEST_PRINTER_FOR(infocell::cells::Std::Op::ESubtract);
IMPLEMENT_GTEST_PRINTER_FOR(infocell::cells::Std::Op::EWhile);
IMPLEMENT_GTEST_PRINTER_FOR(infocell::cells::Object);

IMPLEMENT_GTEST_PRINTER_FOR(infocell::cells::Std::EBoolean);
IMPLEMENT_GTEST_PRINTER_FOR(infocell::cells::Std::SCell);
IMPLEMENT_GTEST_PRINTER_FOR(infocell::cells::Std::EDirection);
IMPLEMENT_GTEST_PRINTER_FOR(infocell::cells::Std::SNumber);
IMPLEMENT_GTEST_PRINTER_FOR(infocell::cells::Std::ENumberSign);

} // namespace testing::internal

namespace infocell {
namespace cells {
class Compiler;
namespace test {

// ============================================================================
class PrintAs
{
public:
    PrintAs(const std::string& postfix);
    ~PrintAs();

    void value(CellI& cell, const std::string& label = "");
    void cell(CellI& cell, const std::string& label = "");

    std::string m_postfix;
};

class CellTestStaticData
{
public:
    CellTestStaticData(World& w);

    TestSyms m_testSyms;
    Ast::Scope m_rootScope;
    std::unique_ptr<Compiler> m_compiler;
    TestLib m_testLib;
};

// ============================================================================
class CellTest :
    public ::testing::Test,
    public NodeBase
{
public:
    static void freeWorld();
    static World& getWorld();

    class LibraryTester : public Library
    {
    public:
        LibraryTester(World& w, TestLib& testLib);

        Object& compile(const std::string& scopeName, const std::string& fnName, Ast::Base& ast);
        Object& compileAsPrompt(Ast::Base& ast);

        Compiler compiler;
        Ast::Scope rootScope;
        TestLib& testLib;
    };

protected:
    CellTest(std::function<void()> loggerLevelInit = []() {});

    void printMethodInType(CellI& type, const std::string& method);

    template <typename... Args>
    Map& parameters(CellI& key, CellI& value, Args&&... args)
    {
        World& w = key.w;
        Map& ret = *new Map(w, w.std.Cell, w.std.ast.Parameter);
        ret.add(key, w.ast.parameterInit(key, value));
        if constexpr (sizeof...(Args) > 0) {
            addParameter(ret, std::forward<Args>(args)...);
         }

        return ret;
    }

    template <typename... Args>
    Map& addParameter(Map& map, CellI& key, CellI& value, Args&&... args)
    {
        map.add(key, value.w.ast.parameterInit(key, value));
        if constexpr (sizeof...(Args) > 0) {
            addParameter(map, std::forward<Args>(args)...);
        }
    }

    static std::unique_ptr<World> m_world;
    static std::unique_ptr<CellTestStaticData> m_staticData;
    TestSyms& test;
    TestLib& testLib;
    PrintAs printAs;
};

// ============================================================================
class TestBase : public ::testing::Test
{
public:
    static void freeWorld();
    static World& getWorld();
    static void createWorld(std::function<void()> loggerLevelInit = []() { });

protected:
    TestBase();

    void printMethodInType(CellI& type, const std::string& method);

    static std::unique_ptr<World> m_world;
    PrintAs printAs;
};


} // namespace test
} // namespace cells
} // namespace infocell