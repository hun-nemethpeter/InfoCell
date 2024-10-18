#include "CellTestBase.h"

using namespace synth::cells;

namespace synth {
namespace cells {
namespace test {

PrintAs::PrintAs(const std::string& postfix) :
    m_postfix(postfix),
    m_svgPrinter(800, 600),
    m_svgStructPrinter(800, 600)
{
}

PrintAs::~PrintAs()
{
    m_svgPrinter.writeFile(fmt::format("svgv-{}.svg", m_postfix));
    m_svgStructPrinter.writeFile(fmt::format("svgs-{}.svg", m_postfix));
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

void PrintAs::svg(CellI& cell, const std::string& caseName)
{
    cell.accept(m_svgPrinter);
    m_svgPrinter.showcaseLastResult(caseName);
}

void PrintAs::svgStruct(CellI& cell, const std::string& caseName)
{
    cell.accept(m_svgStructPrinter);
    m_svgStructPrinter.showcaseLastResult(caseName);
}

CellTest::CellTest(std::function<void()> loggerLevelInit) :
    kb(m_kb.get() ? *m_kb : (m_kb = std::make_unique<brain::Brain>(loggerLevelInit), *m_kb)),
    printAs(::testing::UnitTest::GetInstance()->current_test_info()->name())
{
}

void CellTest::printMethodInType(CellI& type, const std::string& method)
{
    printAs.value(type[ids.methods][ids.index][kb.name(method)][ids.value]);
}

CellI& CellTest::getVariable(const std::string& name)
{
    return kb.getVariable(name);
}

CellI& CellTest::getStruct(const std::string& name)
{
    return kb.getStruct(name);
}

CellI& CellTest::getStruct(CellI& id)
{
    return kb.getStruct(id);
}

CellI& CellTest::toCellNumber(int number)
{
    return kb.pools.numbers.get(number);
}

std::unique_ptr<brain::Brain> CellTest::m_kb;

} // namespace test
} // namespace cells
} // namespace synth