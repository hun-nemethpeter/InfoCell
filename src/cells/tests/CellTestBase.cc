#include "cells/printers/StructPrinter.h"
#include "cells/printers/ValuePrinter.h"

#include "CellTestBase.h"

#include "cells/Compiler.h"

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
    CellValuePrinter valuePrinter(cell.w);

    if (!label.empty()) {
        std::cout << label << ": ";
    }

    std::cout << valuePrinter.print(cell) << std::endl;
}

void PrintAs::cell(CellI& cell, const std::string& label)
{
    CellStructPrinter structPrinter(cell.w);

    if (!label.empty()) {
        std::cout << label << ": ";
    }

    std::cout << structPrinter.print(cell) << std::endl;
}

CellTestStaticData::CellTestStaticData(World& w) :
    m_testSyms(w),
    m_rootScope(w, "testRootScope"),
    m_compiler(std::make_unique<Compiler>(w)),
    m_testLib(w, m_rootScope, *m_compiler, m_testSyms)
{
    m_testLib.include(w.stdLib());
    m_compiler->compile(m_testLib);
    m_testLib.mergeTo(w.stdLib());
}

// ============================================================================
CellTest::CellTest(std::function<void()> loggerLevelInit) :
    NodeBase(m_world.get() ? *m_world : (m_world = std::make_unique<World>(loggerLevelInit), *m_world)),
    test(m_staticData.get() ? m_staticData->m_testSyms : (m_staticData = std::make_unique<CellTestStaticData>(w), m_staticData->m_testSyms)),
    testLib(m_staticData->m_testLib),
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

void CellTest::printMethodInType(CellI& type, const std::string& methodStr)
{
    CellI& method = static_cast<Map&>(type[id.methods]).getValue(w.name(methodStr));
    printAs.value(method);
}

std::unique_ptr<World> CellTest::m_world;
std::unique_ptr<CellTestStaticData> CellTest::m_staticData;

CellTest::LibraryTester::LibraryTester(World& w, TestLib& testLib) :
    Library(w, rootScope),
    compiler(w),
    rootScope(w, "root"),
    testLib(testLib)
{
}

Object& CellTest::LibraryTester::compile(const std::string& scopeName, const std::string& fnName, Ast::Base& ast)
{
    Ast::Scope* testScopePtr = nullptr;
    if (rootScope.hasItem<Ast::Scope>(w.name(scopeName))) {
        testScopePtr = &rootScope.getItem<Ast::Scope>(scopeName);
    } else {
        testScopePtr = &rootScope.add<Ast::Scope>(scopeName);
    }
    Ast::Scope& testScope = *testScopePtr;
    auto& testFunction    = testScope.add<Ast::Function>(fnName);
    testFunction.instructions(ast);
    include(testLib);

    return compiler.compileAsPrompt(testFunction);
}

Object& CellTest::LibraryTester::compileAsPrompt(const std::string& scopeName, const std::string& fnName, Ast::Base& ast)
{
    Ast::Scope* testScopePtr = nullptr;
    if (rootScope.hasItem<Ast::Scope>(w.name(scopeName))) {
        testScopePtr = &rootScope.getItem<Ast::Scope>(scopeName);
    } else {
        testScopePtr = &rootScope.add<Ast::Scope>(scopeName);
    }
    Ast::Scope& testScope = *testScopePtr;
    auto& testFunction    = testScope.add<Ast::Function>(fnName);
    testFunction.description(ast);
    include(testLib);

    return compiler.compileAsPrompt(testFunction);
}

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