#pragma once

#include <gtest/gtest.h>

#include "cells/World.h"
#include "cells/Compiler.h"
#include "cells/NodeBase.h"
#include "TestLib.h"

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
        Object& compileAsPrompt(const std::string& scopeName, const std::string& fnName, Ast::Base& ast);

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
        Map& ret = *new Map(w, w.std.Cell, w.std.ast.Slot);
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