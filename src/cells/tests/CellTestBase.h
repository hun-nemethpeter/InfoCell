#pragma once

#include <gtest/gtest.h>

#include "cells/Cells.h"
#include "cells/Brain.h"
#include "cells/NodeBase.h"

namespace infocell {
namespace cells {
namespace test {

class PrintAs
{
public:
    PrintAs(const std::string& postfix);
    ~PrintAs();

    void value(CellI& cell, const std::string& label = "");
    void cell(CellI& cell, const std::string& label = "");

    std::string m_postfix;
};

class CellTest :
    public ::testing::Test,
    public NodeBase
{
public:
    static void freeKb();
    static brain::Brain& getKb();

protected:
    CellTest(std::function<void()> loggerLevelInit = []() {});

    void printMethodInType(CellI& type, const std::string& method);

    static std::unique_ptr<brain::Brain> m_kb;
    PrintAs printAs;
};

class TestBase : public ::testing::Test
{
public:
    static void freeKb();
    static brain::Brain& getKb();
    static void createKb(std::function<void()> loggerLevelInit = []() { });

protected:
    TestBase();

    void printMethodInType(CellI& type, const std::string& method);

    static std::unique_ptr<brain::Brain> m_kb;
    PrintAs printAs;
};


} // namespace test
} // namespace cells
} // namespace infocell