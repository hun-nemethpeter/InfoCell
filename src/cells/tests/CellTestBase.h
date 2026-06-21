#pragma once

#include <gtest/gtest.h>

#include "cells/World.h"
#include "cells/NodeBase.h"
#include "cells/TestLib.h"

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

protected:
    CellTest(std::function<void()> loggerLevelInit = []() {});

    void printMethodInType(CellI& type, const std::string& method);

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