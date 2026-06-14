#include "TestLib.h"

namespace infocell {
namespace cells {


class TestLibAst : public AstHelper
{
public:
    TestLibAst(World& w, Ast::Scope& scope);
};

TestLibAst::TestLibAst(World& w, Ast::Scope& scope) :
    AstHelper(w)
{
    auto& testFunction = scope.add<Function>("testFunction");
    testFunction.instructions(
        var_("result") = new_(struct_("std::Index")));

    auto& testVariable = scope.add<Var>("testVariable");
    auto& testStruct   = scope.add<Struct>("TestStruct");

    testStruct.addMethod("testCreateNewListOfNumbers")
        .instructions(
            var_("result") = new_(struct_("std::Index")),
            var_("result") = new_(tt_("std::List", "valueType", _(std.Number))),
            var_("result") = new_(tt_("std::List", "valueType", _(std.Cell))),
            var_("result") = new_(tt_("std::List", "valueType", _(std.Pixel))),
            var_("result") = new_(tt_("std::Set", "valueType", _(std.Number))),
            var_("result") = new_(tt_("std::Map", "keyType", _(std.Number), "valueType", _(std.Color))),
            var_("result") = new_(tt_("std::TrieMap", "keyType", _(std.Number), "valueType", _(std.Color))));

    testStruct.addMethod("factorial")
        .parameters(
            parameter("input", _(std.Number)))
        .returnType(_(std.Number))
        .instructions(
            if_(greaterThanOrEqual(p_("input"), _(_1_)))
                .then_(return_(multiply(p_("input"), self()("factorial")("input", subtract(p_("input"), _(_1_))))))
                .else_(return_(_(_1_))));

    scope.add<Enum>("TestEnum")
        .values(
            ev_("value1"), // init with Void
            ev_("value2"));

    scope.add<Enum>("TestEnumWithValues")
        .values(
            ev_("value1", _(_1_)), // init with a value
            ev_("value2", _(_2_)));

    scope.add<Enum>("TestEnumTyped")
        .values(
            tev_("value1", struct_("TestStruct")), // init with value
            tev_("value2", "TestStruct"));

    scope.add<Enum>("TestEnumTypedWithValues")
        .values(
            tev_("value1", "TestStruct", _(_1_)), // init with value
            tev_("value2", "TestStruct", _(_2_)));

    // TODO
    //    type.String.method(ids.addSlots, { ids.list, list(type.slot(ids.value, type.ListOf(type.Char))) });
    // try/catch: almost the same as break/continue/return it can go through function calls. We need an op::Catch node
    // output: we need some kind of output, maybe a console thing first. Maybe just a new hybrid cell is needed
    // Type should hold an std::Type which can be a std::Struct, std::Enum or similar
    // Iterators, range-based-for
    // Variable scopes
    //
}

TestLib::TestLib(World& w, Ast::Scope& parentScope) :
    Library(w, parentScope)
{
    TestLibAst testLibAst(w, parentScope.add<Ast::Scope>("test"));
}

} // namespace cells
} // namespace infocell
