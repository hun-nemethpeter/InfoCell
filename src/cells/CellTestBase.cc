#include "CellTestBase.h"

using namespace infocell::cells;

namespace infocell {
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

#if 0
class AstTest : public brain::AstHelper
{
public:
    AstTest(brain::Brain& kb);
};

AstTest::AstTest(brain::Brain& kb) :
    AstHelper(kb)
{
    auto& testScope = globalScope.add<Scope>("test");

    auto& testFunction = testScope.add<Function>("testFunction");
    testFunction.code(
        var_("result") = new_(struct_("std::Index")));

    auto& testVariable = testScope.add<Var>("testVariable");
    auto& testStruct   = testScope.add<Struct>("TestStruct");

    testStruct.addMethod("testCreateNewListOfNumbers")
        .code(
            var_("result") = new_(struct_("std::Index")),
            var_("result") = new_(tt_("std::List", "valueType", _(std.Number))),
            var_("result") = new_(tt_("std::List", "valueType", _(std.Cell))),
            var_("result") = new_(tt_("std::List", "valueType", _(std.Pixel))),
            var_("result") = new_(tt_("std::Set", "valueType", _(std.Number))),
            var_("result") = new_(tt_("std::Map", "keyType", _(std.Number), "valueType", _(std.Color))),
            var_("result") = new_(tt_("std::TrieMap", "keyType", _(std.Number), "valueType", _(std.Color))));

    testStruct.addMethod("factorial")
        .parameters(
            param("input", _(std.Number)))
        .returnType(_(std.Number))
        .code(
            if_(greaterThanOrEqual(p_("input"), _(_1_)))
                .then_(return_(multiply(p_("input"), self()("factorial", param("input", subtract(p_("input"), _(_1_)))))))
                .else_(return_(_(_1_))));

    testScope.add<Enum>("TestEnum")
        .values(
            ev_("value1"), // init with Void
            ev_("value2"));

    testScope.add<Enum>("TestEnumWithValues")
        .values(
            ev_("value1", _(_1_)), // init with a value
            ev_("value2", _(_2_)));

    testScope.add<Enum>("TestEnumTyped")
        .values(
            tev_("value1", struct_("TestStruct")), // init with value
            tev_("value2", "TestStruct"));

    testScope.add<Enum>("TestEnumTypedWithValues")
        .values(
            tev_("value1", "TestStruct", _(_1_)), // init with value
            tev_("value2", "TestStruct", _(_2_)));

    // TODO
    //    type.String.method(ids.addSlots, { ids.list, list(type.slot(ids.value, type.ListOf(type.Char))) });
    // try/catch: almost the same as break/continue/return it can go through function calls. We need an op::Catch node
    // output: we need some kind of output, maybe a console thing first. Maybe just a new hybrid cell is needed
    // SlotType should hold an std::Type which can be a std::Struct, std::Enum or similar
    // Iterators, range-based-for
    // Variable scopes
    //
    TrieMap earlyStructs(kb, std.Cell, std.Cell, "earlyStructs");
    testScope.compile(earlyStructs);
}

CellTest::CellTest(std::function<void()> loggerLevelInit) :
    kb(m_kb.get() ? *m_kb : (m_kb = std::make_unique<brain::Brain>(loggerLevelInit), AstTest(*m_kb), *m_kb)),
    printAs(::testing::UnitTest::GetInstance()->current_test_info()->name())
{
}
#else
CellTest::CellTest(std::function<void()> loggerLevelInit) :
    kb(m_kb.get() ? *m_kb : (m_kb = std::make_unique<brain::Brain>(loggerLevelInit), *m_kb)),
    printAs(::testing::UnitTest::GetInstance()->current_test_info()->name())
{
}
#endif

void CellTest::freeKb()
{
    m_kb.reset();
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
} // namespace infocell