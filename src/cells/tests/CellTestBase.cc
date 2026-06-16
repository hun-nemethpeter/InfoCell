#include "cells/printers/StructPrinter.h"
#include "cells/printers/ValuePrinter.h"

#include "CellTestBase.h"

using namespace infocell::cells;

namespace infocell {
namespace cells {
namespace test {

// ============================================================================
PrintAs::PrintAs(const std::string& postfix) :
    m_postfix(postfix)
{
}

PrintAs::~PrintAs()
{
}

void PrintAs::value(CellI& cell, const std::string& label)
{
    CellValuePrinter valuePrinter;
    cell.accept(valuePrinter);

    if (!label.empty()) {
        std::cout << label << ": ";
    }

    std::cout << valuePrinter.print() << std::endl;
}

void PrintAs::cell(CellI& cell, const std::string& label)
{
    CellStructPrinter structPrinter;
    cell.accept(structPrinter);

    if (!label.empty()) {
        std::cout << label << ": ";
    }

    std::cout << structPrinter.print() << std::endl;
}

// ============================================================================
CellTest::CellTest(std::function<void()> loggerLevelInit) :
    NodeBase(m_world.get() ? *m_world : (m_world = std::make_unique<World>(loggerLevelInit), *m_world)),
    printAs(::testing::UnitTest::GetInstance()->current_test_info()->name())
{
}

void CellTest::freeWorld()
{
    m_world.reset();
}

World& CellTest::getWorld()
{
    return *m_world;
}

void CellTest::printMethodInType(CellI& type, const std::string& method)
{
    printAs.value(type[id.methods][id.index][w.name(method)][id.value]);
}

std::unique_ptr<World> CellTest::m_world;

// ============================================================================
TestBase::TestBase() :
    printAs(::testing::UnitTest::GetInstance()->current_test_info()->name())
{
}

void TestBase::freeWorld()
{
    m_world.reset();
}

World& TestBase::getWorld()
{
    if (!m_world) {
        createWorld();
    }
    return *m_world;
}

void TestBase::createWorld(std::function<void()> loggerLevelInit)
{
    m_world = std::make_unique<World>(loggerLevelInit);
}

void TestBase::printMethodInType(CellI& type, const std::string& method)
{
    World& w = getWorld();
    printAs.value(type[w.id.methods][w.id.index][w.name(method)][w.id.value]);
}

std::unique_ptr<World> TestBase::m_world;

} // namespace test
} // namespace cells
} // namespace infocell