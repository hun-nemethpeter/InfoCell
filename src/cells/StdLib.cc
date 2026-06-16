#include "StdLib.h"

namespace infocell {
namespace cells {

class StdLibAst : public AstHelper
{
public:
    StdLibAst(World& w, Ast::Scope& scope);

private:
    void createOp();
    void createAst();

    Ast::Scope& stdScope;
};

void StdLibAst::createOp()
{
    auto& opScope = stdScope.add<Scope>("op");
    opScope.add<Struct>("Base");
    opScope.add<Struct>("Activate")
        .members(
            member("ast", "ast::Base"),
            member("cell", "Base"),
            member("next", "Base"),
            member("parent", "Base"),
            member("previous", "std::Cell"),
            member("state", "std::Cell"));

    opScope.add<Struct>("Add")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "std::Number"));

    opScope.add<Struct>("And")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "std::Boolean"));

    opScope.add<Struct>("Block")
        .members(
            member("ast", "ast::Base"),
            member("previous", "std::Cell"),
            member("state", "std::Cell"),
            member("status", "std::Cell"),
            member("ops", "std::Cell"),
            member("value", "std::Cell"));

    opScope.add<Struct>("Call")
        .members(
            member("ast", "ast::Base"),
            member("cell", "ast::Base"),
            member("method", "ast::Base"),
            member("currentParam", "std::Cell"),
            member("parameters", tt_("std::List", "valueType", "std::Slot")),
            member("stack", "ast::Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "std::Cell"));

    opScope.add<Struct>("ConstVar")
        .members(
            member("ast", "ast::Base"),
            member("value", "std::Cell"));

    opScope.add<Struct>("Delete")
        .members(
            member("ast", "ast::Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("input", "Base"));

    opScope.add<Struct>("Divide")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "std::Number"));

    opScope.add<Struct>("Do")
        .members(
            member("ast", "ast::Base"),
            member("status", "std::Cell"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("condition", "Base"),
            member("statement", "Base"));

    opScope.add<Struct>("Equal")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "std::Boolean"));

    opScope.add<Struct>("Erase")
        .members(
            member("ast", "ast::Base"),
            member("cell", "Base"),
            member("key", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "Base"));

    opScope.add<Struct>("Function")
        .members(
            member("ast", "ast::Base"),
            member("name", "std::Cell"),
            member("parameters", tt_("std::Map", "keyType", "std::Cell", "valueType", "std::Slot")),
            member("localVars", "std::Index"),
            member("returnType", "std::Cell"),
            member("objectType", "std::Cell"),
            member("lastOp", tt_("std::List", "valueType", "Base")),
            member("op", tt_("std::List", "valueType", "Base")),
            member("previous", "std::Cell"),
            member("stack", "Stack"),
            member("state", "std::Cell"),
            member("static_", "std::Boolean"),
            member("value", "std::Cell"));

    opScope.add<Struct>("Get")
        .members(
            member("ast", "ast::Base"),
            member("cell", "Base"),
            member("key", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "std::Cell"));

    opScope.add<Struct>("GreaterThan")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "std::Boolean"));

    opScope.add<Struct>("GreaterThanOrEqual")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "std::Boolean"));

    opScope.add<Struct>("Has")
        .members(
            member("ast", "ast::Base"),
            member("cell", "Base"),
            member("key", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "std::Boolean"));

    opScope.add<Struct>("If")
        .members(
            member("ast", "ast::Base"),
            member("status", "std::Cell"),
            member("condition", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("then", "Base"),
            member("else_", "Base"));

    opScope.add<Struct>("LessThan")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "std::Boolean"));

    opScope.add<Struct>("LessThanOrEqual")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "std::Boolean"));

    opScope.add<Struct>("Missing")
        .members(
            member("ast", "ast::Base"),
            member("cell", "Base"),
            member("key", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "std::Boolean"));

    opScope.add<Struct>("Multiply")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "std::Number"));

    opScope.add<Struct>("New")
        .members(
            member("ast", "ast::Base"),
            member("value", "std::Cell"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("objectType", "Base"));

    opScope.add<Struct>("Not")
        .members(
            member("ast", "ast::Base"),
            member("input", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "std::Boolean"));

    opScope.add<Struct>("NotEqual")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "std::Boolean"));

    opScope.add<Struct>("NotSame")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "std::Boolean"));

    opScope.add<Struct>("Or")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "std::Boolean"));

    opScope.add<Struct>("Return")
        .members(
            member("ast", "ast::Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("result", "ast::Base"));

    opScope.add<Struct>("Same")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "std::Boolean"));

    opScope.add<Struct>("Set")
        .members(
            member("ast", "ast::Base"),
            member("cell", "Base"),
            member("key", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "Base"));

    opScope.add<Struct>("Subtract")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "std::Number"));

    opScope.add<Struct>("Var")
#if 0 // TODO
        .description(
            var_("x")("isA")("struct", m_("valueType")),
            self()("hasA")("member", member("ast", "ast::Base")),
            self()("stores")("place", m_("value"))("value", var_("x")))
#endif
        .members(
            member("ast", "ast::Base"),
            member("valueType", "std::Struct"),
            member("value", "std::Cell"));

    opScope.add<Struct>("While")
        .members(
            member("ast", "ast::Base"),
            member("status", "std::Cell"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("condition", "Base"),
            member("statement", "Base"));
}

void StdLibAst::createAst()
{
    auto& astScope = stdScope.add<Scope>("ast");
    astScope.add<Struct>("Base");
    astScope.add<Struct>("Add")
        .memberOf(
            _(std.ast.Base))
        .primitiveTool()
        .returnType(_(std.Number))
        .description(
            subtract(return_(), m_("rhs")) == m_("lhs"),
            subtract(return_(), m_("lhs")) == m_("rhs"),
            return_(add(m_("lhs"), m_("rhs"))),
            return_(add(m_("rhs"), m_("lhs"))))
        .members(
            member("lhs", _(std.Number)),
            member("rhs", _(std.Number)));

    astScope.add<Struct>("And")
        .memberOf(
            _(std.ast.Base))
        .primitiveTool()
        .returnType(_(std.Boolean))
        .description(
            return_(and_(m_("lhs"), m_("rhs"))))
        .members(
            member("lhs", _(std.Boolean)),
            member("rhs", _(std.Boolean)));

    astScope.add<Struct>("Block")
        .members(
            member("asts", "std::Cell"));

    astScope.add<Struct>("Break");

    astScope.add<Struct>("Call")
        .members(
            member("cell", "Base"),
            member("method", "Base"),
            member("parameters", ListOf(std.Slot)));

    astScope.add<Struct>("Cell")
        .members(
            member("value", "std::Cell"));

    astScope.add<Struct>("Continue");

    astScope.add<Struct>("Delete")
        .members(
            member("cell", "Base"));

    astScope.add<Struct>("Divide")
        .memberOf(
            _(std.ast.Base))
        .primitiveTool()
        .returnType(_(std.Number))
        .description(
            // TODO check rhs != 0
            multiply(return_(), m_("rhs")) == m_("lhs"),
            return_(divide(m_("lhs"), m_("rhs"))))
        .members(
            member("lhs", _(std.Number)),
            member("rhs", _(std.Number)));

    astScope.add<Struct>("Do")
        .members(
            member("condition", _(std.Boolean)),
            member("statement", "Base"));

    astScope.add<Struct>("Enum")
        .members(
            member("name", "std::Cell"),
            member("fullyQualifiedName", "std::Cell"),
            member("scope", "Scope"),
            member("values", "TrieMap"));

    astScope.add<Struct>("EnumValue")
        .members(
            member("name", "std::Cell"),
            member("fullyQualifiedName", "std::Cell"),
            member("enum", "Enum"),
            member("value", "std::Cell"));

    astScope.add<Struct>("Equal")
        .memberOf(
            _(std.ast.Base))
        .primitiveTool()
        .returnType(_(std.Boolean))
        .description(
            return_(m_("lhs") == m_("rhs")))
        .members(
            member("lhs", "Base"),
            member("rhs", "Base"));

    astScope.add<Struct>("Erase")
        .description(
            has(m_("cell"), m_("key")) == false_())
        .members(
            member("cell", "Base"),
            member("key", "Base"));

    astScope.add<Struct>("For")
        .members(
            member("variable", "std::Cell"),
            member("container", "Base"),
            member("statement", "Base"));

    astScope.add<Struct>("Function")
        .members(
            member("name", "std::Cell"),
            member("fullyQualifiedName", "std::Cell"),
            member("structType", "std::Cell"),
            member("parameters", ListOf(std.Slot)),
            member("returnType", "std::Struct"),
            member("instructions", "Base"),
            member("scope", "Base"),
            member("static_", "std::Boolean"));

    astScope.add<Struct>("FunctionT")
        .members(
            member("name", "std::Cell"),
            member("parameters", ListOf(std.Slot)),
            member("returnType", "std::Struct"),
            member("instructions", "Base"),
            member("scope", "Base"),
            member("static_", "std::Boolean"));

    astScope.add<Struct>("Get")
        .memberOf(
            _(std.ast.Base))
        .primitiveTool()
        .returnType(_(std.ast.Base))
        .description(
            return_(m_("cell") / m_("key")))
        .members(
            member("cell", "Base"),
            member("key", "Base"));

    astScope.add<Struct>("GreaterThan")
        .memberOf(
            _(std.ast.Base))
        .primitiveTool()
        .returnType(_(std.Boolean))
        .description(
            lessThan(subtract(m_("rhs"), m_("lhs")), _(_0_)),
            return_(greaterThan(m_("lhs"), m_("rhs"))))
        .members(
            member("lhs", _(std.Number)),
            member("rhs", _(std.Number)));

    astScope.add<Struct>("GreaterThanOrEqual")
        .memberOf(
            _(std.ast.Base))
        .primitiveTool()
        .returnType(_(std.Boolean))
        .description(
            lessThanOrEqual(subtract(m_("rhs"), m_("lhs")), _(_0_)),
            return_(greaterThanOrEqual(m_("lhs"), m_("rhs"))))
        .members(
            member("lhs", _(std.Number)),
            member("rhs", _(std.Number)));

    astScope.add<Struct>("Match")
        .memberOf(
            _(std.ast.Base))
        .members(
            member("cases", _(std.List)),
            member("enum", _(std.ast.Base)));

    astScope.add<Struct>("Has")
        .memberOf(
            _(std.ast.Base))
        .primitiveTool()
        .returnType(_(std.Boolean))
        .description(
            return_(has(m_("cell"), m_("key"))))
        .members(
            member("cell", "Base"),
            member("key", "Base"));

    astScope.add<Struct>("If")
#if 0 // for the CellTrie we have to reference _every_ member but the else_ here is optional so this is a TODO
        .description(
//            equal(_("ActivationPointer") / _("currentCell"), _("condition")), // do we need this?!
            if_(m_("condition")).then_(m_("then")) // is this enough?!
            )
#endif
        .members(
            member("condition", "Base"),
            member("then", "Base"),
            member("else_", "Base"));

    astScope.add<Struct>("LessThan")
        .memberOf(
            _(std.ast.Base))
        .primitiveTool()
        .returnType(_(std.Boolean))
        .description(
            greaterThan(subtract(m_("rhs"), m_("lhs")), _(_0_)),
            return_(lessThan(m_("lhs"), m_("rhs"))))
        .members(
            member("lhs", _(std.Number)),
            member("rhs", _(std.Number)));

    astScope.add<Struct>("LessThanOrEqual")
        .memberOf(
            _(std.ast.Base))
        .primitiveTool()
        .returnType(_(std.Boolean))
        .description(
            greaterThanOrEqual(subtract(m_("rhs"), m_("lhs")), _(_0_)),
            return_(lessThanOrEqual(m_("lhs"), m_("rhs"))))
        .members(
            member("lhs", _(std.Number)),
            member("rhs", _(std.Number)));

    astScope.add<Struct>("Member")
        .members(
            member("key", "Base"));

    astScope.add<Struct>("Missing")
        .memberOf(
            _(std.ast.Base))
        .primitiveTool()
        .returnType(_(std.Boolean))
        .description(
            return_(missing(m_("cell"), m_("key"))))
        .members(
            member("cell", "Base"),
            member("key", "Base"));

    astScope.add<Struct>("Multiply")
        .memberOf(
            _(std.ast.Base))
        .primitiveTool()
        .returnType(_(std.Number))
        .description(
#if 0 // we need a precondition secton for this if block
            if_(notSame(m_("lhs"), _(_0_))).then_(
#endif
            divide(return_(), m_("lhs")) == m_("rhs")
#if 0 // we need a precondition secton for this if block
            )
#else
            ,
#endif
            return_(multiply(m_("lhs"), m_("rhs"))))
        .members(
            member("lhs", _(std.Number)),
            member("rhs", _(std.Number)));

    astScope.add<Struct>("New")
        .members(
            member("objectType", "Base"),
            member("constructor", "Base"),
            member("parameters", ListOf(std.ast.Slot)));

    astScope.add<Struct>("Not")
        .memberOf(
            _(std.ast.Base))
        .primitiveTool()
        .returnType(_(std.Boolean))
        .description(
            return_(not_(m_("input"))))
        .members(
            member("input", _(std.Boolean)));

    astScope.add<Struct>("NotEqual")
        .memberOf(
            _(std.ast.Base))
        .primitiveTool()
        .returnType(_(std.Boolean))
        .description(
            return_(notEqual(m_("lhs"), m_("rhs"))))
        .members(
            member("lhs", "Base"),
            member("rhs", "Base"));

    astScope.add<Struct>("NotSame")
        .memberOf(
            _(std.ast.Base))
        .primitiveTool()
        .returnType(_(std.Boolean))
        .description(
            return_(notSame(m_("lhs"), m_("rhs"))))
        .members(
            member("lhs", "Base"),
            member("rhs", "Base"));

    astScope.add<Struct>("Or")
        .memberOf(
            _(std.ast.Base))
        .primitiveTool()
        .returnType(_(std.Boolean))
        .description(
            return_(or_(m_("lhs"), m_("rhs"))))
        .members(
            member("lhs", _(std.Boolean)),
            member("rhs", _(std.Boolean)));

    astScope.add<Struct>("Parameter")
        .members(
            member("key", "std::Cell"));

    astScope.add<Struct>("ResolvedType")
        .members(
            member("ast", "std::Struct"),
            member("compiled", "std::Struct"));

    astScope.add<Struct>("Return")
        .memberOf(
            _(std.ast.Base))
        .members(
            member("value", "std::Cell"));

    astScope.add<Struct>("Same")
        .memberOf(
            _(std.ast.Base))
        .primitiveTool()
        .returnType(_(std.Boolean))
        .description(
            return_(same(m_("lhs"), m_("rhs"))))
        .members(
            member("lhs", "Base"),
            member("rhs", "Base"));

    astScope.add<Struct>("Scope")
        .members(
            member("link", "Scope"),
            member("name", "std::Cell"),
            member("fullyQualifiedName", "std::Cell"),
            member("scopes", "std::TrieMap"),
            member("resolvedScope", "Scope"),
            member("scope", "Scope"),
            member("functions", "std::TrieMap"),
            member("structs", "std::TrieMap"),
            member("structTs", "std::TrieMap"),
            member("enums", "std::TrieMap"),
            member("variables", ListOf(std.ast.Slot)));

    astScope.add<Struct>("Self");

    astScope.add<Struct>("SelfFn");

    astScope.add<Struct>("Set")
        .memberOf(
            _(std.ast.Base))
        .primitiveTool()
        .description(
            m_("cell") / m_("key") == m_("value"))
        .members(
            member("cell", "Base"),
            member("key", "Base"),
            member("value", "Base"));

    astScope.add<Struct>("Slot")
        .members(
            member("key", "Base"),
            member("type", "Base"));

    astScope.add<Struct>("StaticCall")
        .members(
            member("cell", "Base"),
            member("method", "Base"),
            member("parameters", ListOf(std.Slot)));

    astScope.add<Struct>("Struct")
        .members(
            member("name", "std::Cell"),
            member("compiledStruct", "std::op::Base"),
            member("fullyQualifiedName", "std::Cell"),
            member("incomplete", "std::Boolean"),
            member("instanceOf", "Base"),
            member("templateParams", "std::List"),
            member("scope", "Scope"),
            member("primitiveTool", _(std.Boolean)),
            member("returnType", "Base"),
            member("methods", MapOf(std.Cell, std.ast.Function)),
            member("members", MapOf(std.Cell, std.ast.Slot)),
            member("typeAliases", ListOf(std.ast.Slot)),
            member("memberOf", ListOf(std.Struct)));

    astScope.add<Struct>("StructName")
        .members(
            member("name", "std::Cell"),
            member("scopes", "std::List"));

    astScope.add<Struct>("StructT")
        .members(
            member("name", "std::Cell"),
            member("scope", "Base"),
            member("methods", MapOf(std.Cell, std.ast.Function)),
            member("members", ListOf(std.ast.Slot)),
            member("typeAliases", ListOf(std.ast.Slot)),
            member("memberOf", ListOf(std.Struct)),
            member("templateParams", MapOf(std.Cell, std.Struct)));

    astScope.add<Struct>("TypeAlias")
        .members(
            member("name", "std::Cell"));

    astScope.add<Struct>("Subtract")
        .memberOf(
            _(std.ast.Base))
        .primitiveTool()
        .returnType(_(std.Number))
        .description(
            add(return_(), m_("rhs")) == m_("lhs"),
            add(m_("rhs"), return_()) == m_("lhs"),
//            equal(m_("lhs"), add(return_(), m_("rhs"))),
            return_(subtract(m_("lhs"), m_("rhs"))))
        .members(
            member("lhs", _(std.Number)),
            member("rhs", _(std.Number)));

    astScope.add<Struct>("TemplatedType")
        .members(
            member("id", "Base"),
            member("scopes", "std::List"),
            member("parameters", ListOf(std.Slot)));

    astScope.add<Struct>("TemplateParam")
        .members(
            member("key", "std::Cell"));

    astScope.add<Struct>("Throw")
        .members(
            member("value", "Base"));

    astScope.add<Struct>("Trait")
        .members(
            member("name", "std::Cell"),
            member("scope", "Base"),
            member("methods", MapOf(std.Cell, std.ast.Function)),
            member("associatedTypes", ListOf(std.ast.Slot)),
            member("typeAliases", ListOf(std.ast.Slot)),
            member("templateParams", MapOf(std.Cell, std.Struct)));

    astScope.add<Struct>("TraitImpl")
        .members(
            member("name", "std::Cell"),
            member("scope", "Base"),
            member("methods", MapOf(std.Cell, std.ast.Function)),
            member("associatedTypes", ListOf(std.ast.Slot)),
            member("typeAliases", ListOf(std.ast.Slot)),
            member("templateParams", MapOf(std.Cell, std.Struct)));

    astScope.add<Struct>("Try")
        .members(
            member("tryBranch", "Base"),
            member("catchBranch", "Base"));

    astScope.add<Struct>("TypedEnumValue")
        .members(
            member("name", "std::Cell"),
            member("fullyQualifiedName", "std::Cell"),
            member("enum", "Enum"),
            member("enumType", "Struct"),
            member("value", "std::Cell"));

    astScope.add<Struct>("Var")
        .members(
            member("name", "std::Cell"),
            member("fullyQualifiedName", "std::Cell"),
            member("scope", "Scope"));

    astScope.add<Struct>("While")
        .members(
            member("condition", _(std.Boolean)),
            member("statement", "Base"));
}


StdLibAst::StdLibAst(World& w, Ast::Scope& scope) :
    AstHelper(w),
    stdScope(scope)
{
    createOp();
    createAst();

        /*
     * enum type
     *   tag: roleId
     *   roleId: value
     */
    stdScope.add<Enum>("Boolean")
        .values(
            ev_("true"),
            ev_("false"));

    stdScope.add<Struct>("Cell");
    stdScope.add<Struct>("Void");
    stdScope.add<Struct>("Slot")
        .members(
            member("key", "Cell"),
            member("type", "Struct"));

    stdScope.add<Struct>("Enum")
        .members(
            member("values", tt_("Map", "keyType", "Cell", "valueType", "Struct")));

    stdScope.add<Struct>("OpState")
        .members(
            member("op", "op::Base"),
            member("state", "Cell"),
            member("value", "Cell"));

    stdScope.add<Struct>("Container");
    stdScope.add<Struct>("Boolean");
    stdScope.add<Struct>("Char");
    stdScope.add<Struct>("Digit");

    stdScope.add<Enum>("NumberSign")
        .values(
            ev_("positive"),
            ev_("negative"));

    stdScope.add<Struct>("Number")
        .members(
            member("value", ListOf(std.Digit)),
            member("sign", "NumberSign"));

    stdScope.add<Struct>("String");

    stdScope.add<Struct>("Color")
        .members(
            member("red", "Number"),
            member("green", "Number"),
            member("blue", "Number"));

    stdScope.add<Struct>("Pixel");
    stdScope.add<Struct>("Grid");
    stdScope.add<Struct>("Stack");

    stdScope.add<Struct>("StackFrame")
        .members(
            member("method", "op::Function"),
            member("ops", "List"),
            member("input", "Index"),
            member("localVars", "Index"));

    stdScope.add<Struct>("Library")
        .members(
            member("scope", "ast::Scope"),
            member("resolvedScope", "ast::Scope"),
            member("functions", tt_("TrieMap", "keyType", "Cell", "valueType", "op::Function")),
            member("structs", tt_("TrieMap", "keyType", "Cell", "valueType", "Struct")),
            member("variables", tt_("TrieMap", "keyType", "Cell", "valueType", "op::Var")));

    stdScope.add<Struct>("StructReference")
        .members(
            member("id", tt_("List", "valueType", "Char")),
            member("idScope", "ast::Scope"),
            member("scope", "ast::Scope"),
            member("resolvedScope", "ast::Scope"),
            member("currentFn", "ast::Function"),
            member("currentStruct", "ast::Struct"),
            member("templateId", tt_("List", "valueType", "Cell")),
            member("templateParams", tt_("List", "valueType", "ast::Base")),
            member("value", "Struct"));

    stdScope.add<Struct>("Directions");

#pragma region ListItem
    stdScope.add<Struct>("ListItem")
        .typeAliases(
            typeAlias("ValueType", struct_("Cell")))
        .members(
            member("previous", "ListItem"),
            member("next", "ListItem"),
            member("value", st_("ValueType")));

    auto& listItemStructT
        = stdScope.add<StructT>("ListItem")
              .templateParams(
                  parameter("valueType", _(std.Struct)))
              .memberOf(
                  _(std.ListItem))
              .typeAliases(
                  typeAlias("valueType", tp_("valueType")))
              .members(
                  member("previous", tt_("ListItem", "valueType", tp_("valueType"))),
                  member("next", tt_("ListItem", "valueType", tp_("valueType"))),
                  member("value", tp_("valueType")));

    listItemStructT.addMethod("constructor")
        .parameters(
            parameter("value", tp_("valueType")))
        .instructions(
            m_("value") = p_("value"));
#pragma endregion
#pragma region List
    stdScope.add<Struct>("List")
        .typeAliases(
            typeAlias("itemType", struct_("ListItem")),
            typeAlias("valueType", struct_("Cell")))
        .members(
            member("first", "ListItem"),
            member("last", "ListItem"),
            member("size", _(std.Number)));

    auto& listStructT
        = stdScope.add<StructT>("List")
              .templateParams(
                  parameter("valueType", _(std.Struct)))
              .memberOf(
                  struct_("Container"),
                  struct_("List"))
              .typeAliases(
                  typeAlias("itemType", tt_("ListItem", "valueType", tp_("valueType"))),
                  typeAlias("valueType", tp_("valueType")))
              .members(
                  member("first", st_("itemType")),
                  member("last", st_("itemType")),
                  member("size", _(std.Number)));

    auto& listIteratorStructT
        = stdScope.add<StructT>("ListIterator")
              .templateParams(
                  parameter("valueType", _(std.Struct)))
              .members(
                  member("list", tt_("List", "valueType", tp_("valueType"))),
                  member("node", tp_("valueType")));

    listIteratorStructT.addMethod("constructor")
        .parameters(
            parameter("list", tt_("List", "valueType", tp_("valueType"))))
        .instructions(
            m_("list") = p_("list"));
    /*
    trait Iterable {
        type Iterator: std::Iterator;

        fn iterator() -> Self::Iterator;
    }
    */
    auto& iterableTrait
        = stdScope.add<Trait>("Iterable")
              .associatedTypes(
                  parameter("Iterator", _("Iterator")))
              .addMethod("iterator").returnType(at_("Iterator"));

    /*
    impl<T> trait Iterable for List<T> {
        type Iterator = ListIterator<T>;

        fn iterator() -> Self::Iterator {
            return new Self::Iterator(list: self())
        }
    }
    */
    auto& implIterableTraitForListT
        = stdScope.add<TraitImpl>("Iterable")
              .templateParams(
                  parameter("valueType", _(std.Struct)))
              .implementedFor(tt_("List", "valueType", tp_("valueType")))
              .associatedTypes(
                  parameter("Iterator", tt_("ListIterator", tp_("valueType"))));

    implIterableTraitForListT.addMethod("iterator")
        .returnType(at_("Iterator"))
        .instructions(
            return_(new_(at_("Iterator"), "constructor")("list", self())));

    /*
    trait Iterator
    {
        type ValueType;

        bool isContainerEmpty();
        void goToFirstNode();
        ValueType getCurrentNodeValue();
        bool hasNextNode();
        void goToNextNode();
    }
    */
    auto& iteratorTrait
        = stdScope.add<Trait>("Iterator")
              .associatedTypes(parameter("ValueType", _(std.Struct)));

    iteratorTrait.addMethod("isContainerEmpty").returnType(_(std.Boolean));
    iteratorTrait.addMethod("goToFirstNode");
    iteratorTrait.addMethod("getCurrentNodeValue").returnType(_(std.Boolean));
    iteratorTrait.addMethod("hasNextNode").returnType(_(std.Boolean));
    iteratorTrait.addMethod("goToNextNode");

    auto& implIteratorTraitForListT
        = stdScope.add<TraitImpl>("Iterator")
              .templateParams(
                  parameter("ValueType", _(std.Struct)))
              .implementedFor(tt_("ListIterator", "valueType", tp_("ValueType")))
              .associatedTypes(
                  parameter("NodeType", tp_("ValueType")))
              .members(
                  member("node", tp_("ValueType")));

    implIteratorTraitForListT.addMethod("isContainerEmpty")
        .returnType(_(std.Boolean))
        .instructions(
            return_(equal(m_("size"), _(_0_))));

    implIteratorTraitForListT.addMethod("goToFirstNode")
        .instructions(
            set(m_("node"), _(ids.value), m_("first")));

    implIteratorTraitForListT.addMethod("getCurrentNodeValue")
        .returnType(st_("NodeType"))
        .instructions(
            return_(m_("node")));

    implIteratorTraitForListT.addMethod("hasNextNode")
        .returnType(_(std.Boolean))
        .instructions(
            return_(has(m_("node"), "next")));

    implIteratorTraitForListT.addMethod("goToNextNode")
        .instructions(
            m_("node") = m_("node") / "next");

    listStructT.addMethod("constructor")
        .instructions(
            m_("size") = _(_0_));

    listStructT.addMethod("add")
        .parameters(
            parameter("value", tp_("valueType")))
        .returnType(st_("itemType"))
        .instructions(
            var_("item") = new_(st_("itemType"), "constructor")("value", p_("value")),
            if_(not_(m_("first").exist()))
                .then_(m_("first") = *var_("item"))
                .else_(block(
                    set(m_("last"), "next", *var_("item")),
                    set(*var_("item"), "previous", m_("last")))),
            m_("last") = *var_("item"),
            m_("size") = add(m_("size"), _(_1_)),
            return_(*var_("item")));

    /*
    void List::removeItem(Item* item)
    {
        if (item->m_previous) {
            item->m_previous->m_next = item->m_next;
        } else {
            m_firstItem = item->m_next;
        }
        if (item->m_next) {
            item->m_next->m_previous = item->m_previous;
        } else {
            m_lastItem = item->m_previous;
        }
        --m_size;
    }
    */
    listStructT.addMethod("remove")
        .parameters(
            parameter("item", _(std.Cell)))
        .instructions(
            if_(has(p_("item"), "previous"))
                .then_(
                    if_(has(p_("item"), "next"))
                        .then_(set(p_("item") / "previous", "next", p_("item") / "next"))
                        .else_(erase(p_("item") / "previous", "next")))
                .else_(
                    if_(has(p_("item"), "next"))
                        .then_(m_("first") = p_("item") / "next")
                        .else_(erase(self(), "first"))),
            if_(has(p_("item"), "next"))
                .then_(
                    if_(has(p_("item"), "previous"))
                        .then_(set(p_("item") / "next", "previous", p_("item") / "previous"))
                        .else_(erase(p_("item") / "next", "previous")))
                .else_(
                    if_(has(p_("item"), "previous"))
                        .then_(m_("last") = p_("item") / "previous")
                        .else_(erase(self(), "last"))),
            m_("size") = subtract(m_("size"), _(_1_)));

    listStructT.addMethod("size")
        .returnType(_(std.Number))
        .instructions(
            return_(m_("size")));

    listStructT.addMethod("empty")
        .returnType(_(std.Boolean))
        .instructions(
            return_(equal(m_("size"), _(_0_))));

    listStructT.addMethod("first")
        .returnType(tp_("valueType"))
        .instructions(
            return_(m_("first") / "value"));

    listStructT.addMethod("last")
        .returnType(tp_("valueType"))
        .instructions(
            return_(m_("last") / "value"));

    listStructT.addMethod("begin")
        .returnType(st_("itemType"))
        .instructions(
            return_(m_("first")));

    listStructT.addMethod("end")
        .returnType(st_("itemType"))
        .instructions(
            return_(m_("last")));
#pragma endregion
#pragma region Struct
    auto& structStruct
        = stdScope.add<Struct>("Struct")
              .members(
                  member("name", tt_("List", "valueType", "Char")),
                  member("fullyQualifiedName", "std::Cell"),
                  member("slots", tt_("Map", "keyType", "Cell", "valueType", "Slot")),
                  member("enum", "Boolean"),
                  member("incomplete", "Boolean"),
                  member("sharedObject", "Slot"),
                  member("typeAliases", tt_("Map", "keyType", "Cell", "valueType", "Struct")),
                  member("memberOf", tt_("Map", "keyType", "Struct", "valueType", "Struct")),
                  member("ast", "std::ast::Struct"),
//                  member("asts", tt_("Map", "keyType", "Cell", "valueType", "ast::Function")),
                  member("methods", tt_("Map", "keyType", "Cell", "valueType", "op::Function")));

    structStruct.addMethod("constructor")
        .instructions(
            m_("slots") = new_(tt_("Map", "keyType", "Cell", "valueType", "Slot"), "constructor"));

    structStruct.addMethod("constructorWithRecursiveType")
        .instructions(
            m_("slots") = new_(tt_("Map", "keyType", "Cell", "valueType", "Slot"), "constructorWithIndexType")("indexType", self()));

    structStruct.addMethod("addTypeAlias")
        .parameters(
            parameter("alias", _(std.Cell)),
            parameter("type", _(std.Struct)))
        .instructions(
            if_(m_("typeAliases").missing())
                .then_(m_("typeAliases") = new_(tt_("Map", "keyType", _(std.Cell), "valueType", "Struct"), "constructor")),
            m_("typeAliases")("add")("key", p_("alias"))("value", p_("type")));

    structStruct.addMethod("addMembership")
        .parameters(
            parameter("cell", _(std.Struct)))
        .instructions(
            if_(m_("memberOf").missing())
                .then_(m_("memberOf") = new_(tt_("Map", "keyType", "Struct", "valueType", "Struct"), "constructor")),
            m_("memberOf")("add")("key", p_("cell"))("value", p_("cell")));

    structStruct.addMethod("addSlot")
        .parameters(
            parameter("key", _(std.Cell)),
            parameter("type", _(std.Slot)))
        .instructions(
            if_(m_("slots").missing())
                .then_(m_("slots") = new_(tt_("Map", "keyType", _(std.Cell), "valueType", _(std.Slot)), "constructor")),
            var_("slot") = new_(_(std.Slot)),
            set(*var_("slot"), "key", p_("key")),
            set(*var_("slot"), "type", p_("type")),
            m_("slots")("add")("key", p_("key"))("value", *var_("slot")));

    structStruct.addMethod("addSlots")
        .parameters(
            parameter("list", tt_("List", "valueType", _(std.Slot))))
        .instructions(
            if_(equal(p_("list") / "size", _(_0_)))
                .then_(return_()),
            var_("item") = p_("list") / "first",
            if_(m_("slots").missing())
                .then_(m_("slots") = new_(tt_("Map", "keyType", _(std.Cell), "valueType", _(std.Slot)), "constructor")),
            do_(block(
                        var_("next") = true_(),
                        m_("slots")("add")("key", *var_("item") / "value" / "key")("value", *var_("item") / "value"),
                        if_(has(*var_("item"), "next"))
                            .then_(var_("item") = *var_("item") / "next")
                            .else_(var_("next") = false_())))
                .while_(same(*var_("next"), true_())));

    structStruct.addMethod("hasSlot")
        .parameters(
            parameter("key", _(std.Cell)))
        .returnType(_(std.Boolean))
        .instructions(
            if_(m_("slots").missing())
                .then_(return_(false_())),
            return_(m_("slots")("hasKey")("key", p_("key"))));

    structStruct.addMethod("removeSlot")
        .parameters(
            parameter("key", _(std.Cell)))
        .instructions(
            if_(m_("slots").missing())
                .then_(return_()),
            m_("slots")("remove")("key", p_("key")));
#pragma endregion
#pragma region Index
    auto& indexStruct
        = stdScope.add<Struct>("Index")
              .memberOf(_(std.Struct));

    indexStruct.addMethod("constructor")
        .instructions(
            set(self(), "struct", new_("Struct", "constructorWithRecursiveType")),
            set(m_("struct"), "methods", get(struct_("Index"), _("methods"))),
            set(m_("struct"), "memberOf", _(map(std.Struct, std.Struct, std.Index, std.Index))));

    indexStruct.addMethod("constructorWithSelfType")
        .parameters(
            parameter("indexType", _(std.Struct)))
        .instructions(
            if_(missing(p_("indexType"), _("sharedObject")))
                .then_(block(set(p_("indexType"), "sharedObject", new_(_(std.Slot))),
                                 set(p_("indexType") / "sharedObject", "key", self()),
                                 set(p_("indexType") / "sharedObject", "type", struct_("Index")))),
            set(p_("indexType"), "methods", m_("struct") / "methods"),
            set(self(), "struct", p_("indexType")));

    /*
    void Index::insert(CellI& key, CellI& value)
    {
        if (&key == &"struct") {
            throw "The type key can not be changed!";
        }
        m_slots[&key] = &value;
        if (m_recursiveType) {
            return;
        }
        Object& slot = *new Object(w, w.type.Slot);
        slot.set("key", key);
        slot.set("type", w.type.Slot);
        m_type->addSlot(key, slot);
    }
    */
    indexStruct.addMethod("insert")
        .parameters(
            parameter("key", _(std.Cell)),
            parameter("value", _(std.Cell)))
        .instructions(
            if_(same(p_("key"), _("struct")))
                .then_(return_()),
            set(self(), p_("key"), p_("value")),
            if_(and_(has(m_("struct"), "sharedObject"), same(m_("struct") / "sharedObject" / "key", self())))
                .then_(return_()),
            m_("struct")("addSlot")("key", p_("key"))("type", _(std.Slot)));

    indexStruct.addMethod("empty")
        .returnType(_(std.Boolean))
        .instructions(
            return_((m_("struct") / "slots")("empty")));

    /*
    void Index::erase(CellI& key)
    {
        if (!m_type->hasSlot(key)) {
            return;
        }
        m_slots.erase(&key);
        m_type->removeSlot(key);
    }
    */
    indexStruct.addMethod("remove")
        .parameters(
            parameter("key", _(std.Cell)))
        .instructions(
            if_(not_(m_("struct")("hasSlot")("key", p_("key"))))
                .then_(return_()),
            erase(self(), p_("key")),
            m_("struct")("removeSlot")("key", p_("key")));

    indexStruct.addMethod("size")
        .returnType(_(std.Number))
        .instructions(
            return_((m_("struct") / "slots")("size")));
#pragma endregion
#pragma region Map
    stdScope.add<Struct>("Map")
        .typeAliases(
            typeAlias("keyType", struct_("Cell")),
            typeAlias("valueType", struct_("Cell")),
            typeAlias("listType", tt_("List", "valueType", struct_("Cell"))))
        .memberOf(struct_("Container"))
        .members(
            member("list", st_("listType")),
            member("index", "Index"),
            member("size", _(std.Number)));

    auto& mapStructT
        = stdScope.add<StructT>("Map")
              .templateParams(
                  parameter("keyType", _(std.Struct)),
                  parameter("valueType", _(std.Struct)))
              .typeAliases(
                  typeAlias("keyType", tp_("keyType")),
                  typeAlias("valueType", tp_("valueType")),
                  typeAlias("listType", tt_("List", "valueType", tp_("valueType"))))
              .memberOf(struct_("Map"))
              .members(
                  member("list", st_("listType")),
                  member("index", "Index"),
                  member("size", _(std.Number)));

    mapStructT.addMethod("constructor")
        .instructions(
            m_("size")  = _(_0_),
            m_("list")  = new_(st_("listType"), "constructor"),
            m_("index") = new_("Index", "constructor"));

    mapStructT.addMethod("constructorWithIndexType")
        .parameters(
            parameter("indexType", _(std.Struct)))
        .instructions(
            m_("size")  = _(_0_),
            m_("list")  = new_(st_("listType"), "constructor"),
            m_("index") = new_("Index", "constructorWithSelfType")("indexType", p_("indexType")));

    /*
    bool Map::hasKey(CellI& key)
    {
        return m_index.has(key);
    }
    */
    mapStructT.addMethod("hasKey")
        .parameters(
            parameter("key", tp_("keyType")))
        .returnType(_(std.Boolean))
        .instructions(
            return_(has(m_("index"), p_("key"))));

    /*
    CellI& Map::getValue(CellI& key)
    {
        if (m_index.has(key)) {
            return m_index[key][w.ids.value];
        }
        throw "No such key!";
    }
    */
    mapStructT.addMethod("getValue")
        .parameters(
            parameter("key", tp_("keyType")))
        .returnType(tp_("valueType"))
        .instructions(
            if_(has(m_("index"), p_("key")))
                .then_(return_(m_("index") / p_("key") / "value"))
                .else_(return_(_("emptyObject"))));

    /*
    void Map::add(CellI& key, CellI& value)
    {
        if (&key == &"struct") {
            throw "ids.type can not be stored in a map!";
        }
        if (m_index.has(key)) {
            throw "A value already registered with this key";
        }
        List::Item& item = *m_list.add(value);
        m_index.insert(key, item);
        ++m_size;
    }
    */
    mapStructT.addMethod("add")
        .parameters(
            parameter("key", tp_("keyType")),
            parameter("value", tp_("valueType")))
        .instructions(
            if_(same(p_("key"), _("struct")))
                .then_(return_()),
            if_(has(m_("index"), p_("key")))
                .then_(return_()),
            m_("size")   = add(m_("size"), _(_1_)),
            var_("item") = m_("list")("add")("value", p_("value")),
            m_("index")("insert")("key", p_("key"))("value", *var_("item")));

    /*
    void Map::remove(CellI& key)
    {
        if (!m_index.has(key)) {
            return;
        }
        List::Item* item = &static_cast<List::Item&>(m_index[key]);
        m_list.removeItem(item);
        m_index.erase(key);
        --m_size;
    }
    */
    mapStructT.addMethod("remove")
        .parameters(
            parameter("key", tp_("keyType")))
        .instructions(
            if_(missing(m_("index"), p_("key")))
                .then_(return_()),
            m_("list")("remove")("item", m_("index") / p_("key")),
            m_("index")("remove")("key", p_("key")),
            m_("size") = subtract(m_("size"), _(_1_)));

    mapStructT.addMethod("size")
        .returnType(_(std.Number))
        .instructions(
            return_(m_("size")));

    mapStructT.addMethod("empty")
        .returnType(_(std.Boolean))
        .instructions(
            return_(equal(m_("size"), _(_0_))));

    mapStructT.addMethod("first")
        .returnType(tp_("valueType"))
        .instructions(
            return_(m_("list") / "first" / "value"));

    mapStructT.addMethod("last")
        .returnType(tp_("valueType"))
        .instructions(
            return_(m_("list") / "last" / "value"));

    mapStructT.addMethod("begin")
        .returnType(tt_("ListItem", "valueType", tp_("valueType")))
        .instructions(
            return_(m_("list") / "first"));

    mapStructT.addMethod("end")
        .returnType(tt_("ListItem", "valueType", tp_("valueType")))
        .instructions(
            return_(m_("list") / "last"));
#pragma endregion
#pragma region TrieMap
    stdScope.add<Struct>("KVPair")
        .members(
            member("key", "Cell"),
            member("value", "Cell"));

    auto& kvPairT
        = stdScope.add<StructT>("KVPair")
              .templateParams(
                  parameter("keyType", _(std.Struct)),
                  parameter("valueType", _(std.Struct)))
              .typeAliases(
                  typeAlias("keyType", tp_("keyType")),
                  typeAlias("valueType", tp_("valueType")))
              .memberOf(struct_("KVPair"))
              .members(
                  member("key", tp_("keyType")),
                  member("value", tp_("valueType")));

    kvPairT.addMethod("constructor")
        .parameters(
            parameter("key", tp_("keyType")),
            parameter("value", tp_("valueType")))
        .instructions(
            m_("key")   = p_("key"),
            m_("value") = p_("value"));

    // TODO This can be a template but nevermind ...
    stdScope.add<Struct>("TrieMapNode")
        .members(
            member("children", "Index"),
            member("data", "ListItem"),
            member("parent", "TrieMapNode"));

    stdScope.add<Struct>("TrieMap")
        .typeAliases(
            typeAlias("keyType", struct_("Cell")),
            typeAlias("valueType", struct_("Cell")),
            typeAlias("pairType", tt_("KVPair", "keyType", struct_("Cell"), "valueType", struct_("Cell"))),
            typeAlias("listType", tt_("List", "valueType", st_("pairType"))))
        .memberOf(struct_("Container"))
        .members(
            member("list", st_("listType")),
            member("rootNode", struct_("TrieMapNode")),
            member("size", _(std.Number)));

    auto& trieMapStructT
        = stdScope.add<StructT>("TrieMap")
              .templateParams(
                  parameter("keyType", _(std.Struct)),
                  parameter("valueType", _(std.Struct)))
              .typeAliases(
                  typeAlias("keyType", tp_("keyType")),
                  typeAlias("valueType", tp_("valueType")),
                  typeAlias("pairType", tt_("KVPair", "keyType", tp_("keyType"), "valueType", tp_("valueType"))),
                  typeAlias("listType", tt_("List", "valueType", st_("pairType"))))
              .memberOf(_(std.Container), _(std.TrieMap))
              .members(
                  member("list", st_("listType")),
                  member("rootNode", _(std.TrieMapNode)),
                  member("size", _(std.Number)));

    trieMapStructT.addMethod("constructor")
        .instructions(
            m_("list")     = new_(st_("listType"), "constructor"),
            m_("rootNode") = new_(_(std.TrieMapNode)),
            m_("size")     = _(_0_));

    /*
    bool TrieMap::hasKey(CellI& key)
    {
        CellI* currentNode = &m_rootNode;

        if (isA(key, w.type.List)) {
            throw "Key is not a list!";
        }

        Visitor::visitList(key, [this, &currentNode](CellI& keyItem, int i, bool& stop) {
            CellI* children = nullptr;
            if (currentNode->missing(w.ids.children)) {
                stop        = true;
                currentNode = nullptr;
                return;
            }
            Index& childrenIndex = static_cast<Index&>(currentNode->get(w.ids.children));
            if (childrenIndex.has(keyItem)) {
                children = &childrenIndex.get(keyItem);
            } else {
                stop        = true;
                currentNode = nullptr;
                return;
            }
            currentNode = children;
        });

        if (!currentNode || currentNode->missing(w.ids.data)) {
            return false;
        }

        return true;
    }
    */
    trieMapStructT.addMethod("hasKey")
        .parameters(
            parameter("key", tp_("keyType")))
        .returnType(_(std.Boolean))
        .instructions(
            var_("currentNode") = m_("rootNode"),
            var_("keyItem")     = _(ids.emptyObject),
            if_(has(p_("key"), "first"))
                .then_(var_("keyItem") = p_("key") / "first"),
            while_(notSame(*var_("keyItem"), _(ids.emptyObject)))
                .do_(block(
                    var_("keyItemObj") = *var_("keyItem") / "value",
                    var_("child")      = _(ids.emptyObject),
                    if_(missing(*var_("currentNode"), "children"))
                        .then_(return_(false_())),
                    var_("childrenIndex") = *var_("currentNode") / "children",
                    if_(has(*var_("childrenIndex"), *var_("keyItemObj")))
                        .then_(var_("child") = *var_("childrenIndex") / *var_("keyItemObj"))
                        .else_(return_(false_())),
                    var_("currentNode") = *var_("child"),
                    if_(has(*var_("keyItem"), "next"))
                        .then_(var_("keyItem") = *var_("keyItem") / "next")
                        .else_(var_("keyItem") = _(ids.emptyObject)))),
            if_(missing(*var_("currentNode"), "data"))
                .then_(return_(false_())),
            return_(true_()));

    /*
    CellI& TrieMap::getValue(CellI& key)
    {
        if (isA(key, w.type.List)) {
            throw "Key is not a list!";
        }

        CellI* currentNode = &m_rootNode;

        Visitor::visitList(key, [this, &currentNode](CellI& keyItem, int i, bool& stop) {
            CellI* children = nullptr;
            if (currentNode->missing(w.ids.children)) {
                stop        = true;
                currentNode = nullptr;
                return;
            }
            Index& childrenIndex = static_cast<Index&>(currentNode->get(w.ids.children));
            if (childrenIndex.has(keyItem)) {
                children = &childrenIndex.get(keyItem);
            } else {
                stop        = true;
                currentNode = nullptr;
                return;
            }
            currentNode = children;
        });

        if (!currentNode || currentNode->missing(w.ids.data)) {
            throw "No such key!";
        }

        return (*currentNode)[w.ids.data][w.ids.value][w.ids.value];
    }
    */
    trieMapStructT.addMethod("getValue")
        .parameters(
            parameter("key", tp_("keyType")))
        .returnType(tp_("valueType"))
        .instructions(
            var_("currentNode") = m_("rootNode"),
            var_("keyItem")     = _(ids.emptyObject),
            if_(has(p_("key"), "first"))
                .then_(var_("keyItem") = p_("key") / "first"),
            while_(notSame(*var_("keyItem"), _(ids.emptyObject)))
                .do_(block(
                    var_("keyItemObj") = *var_("keyItem") / "value",
                    var_("child")      = _(ids.emptyObject),
                    if_(missing(*var_("currentNode"), "children"))
                        .then_(return_(_(ids.emptyObject))),
                    var_("childrenIndex") = *var_("currentNode") / "children",
                    if_(has(*var_("childrenIndex"), *var_("keyItemObj")))
                        .then_(var_("child") = *var_("childrenIndex") / *var_("keyItemObj"))
                        .else_(return_(_(ids.emptyObject))),
                    var_("currentNode") = *var_("child"),
                    if_(has(*var_("keyItem"), "next"))
                        .then_(var_("keyItem") = *var_("keyItem") / "next")
                        .else_(var_("keyItem") = _(ids.emptyObject)))),
            if_(missing(*var_("currentNode"), "data"))
                .then_(return_(_(ids.emptyObject))),
            return_(*var_("currentNode") / "data" / "value" / "value"));

    /*
    void TrieMap::add(CellI& key, CellI& value)
    {
        if (isA(key, w.type.List)) {
            throw "Key is not a list!";
        }

        CellI* currentNode = &m_rootNode;

        Visitor::visitList(key, [this, &currentNode](CellI& keyItem, int i, bool& stop) {
            CellI* child = nullptr;
            if (currentNode->missing(w.ids.children)) {
                currentNode->set(w.ids.children, *new Index(w));
            }
            Index& childrenIndex = static_cast<Index&>(currentNode->get(w.ids.children));
            if (childrenIndex.has(keyItem)) {
                child = &childrenIndex.get(keyItem);
            } else {
                child = new Object(w, w.type.TrieMapNode);
                child->set(w.ids.parent, *currentNode);
                childrenIndex.insert(keyItem, *child);
            }
            currentNode = child;
        });

        List::Item& item = *m_list.add(w.type.kvPair(key, value));
        currentNode->set(w.ids.data, item);
        ++m_size;
    }
    */
    trieMapStructT.addMethod("add")
        .parameters(
            parameter("key", tp_("keyType")),
            parameter("value", tp_("valueType")))
        .instructions(
            var_("currentNode") = m_("rootNode"),
            var_("keyItem")     = _(ids.emptyObject),
            if_(has(p_("key"), "first"))
                .then_(var_("keyItem") = p_("key") / "first"),
            while_(notSame(*var_("keyItem"), _(ids.emptyObject)))
                .do_(block(
                    var_("keyItemObj") = *var_("keyItem") / "value",
                    var_("child")      = _(ids.emptyObject),
                    if_(missing(*var_("currentNode"), "children"))
                        .then_(set(*var_("currentNode"), "children", new_("Index", "constructor"))),
                    var_("childrenIndex") = *var_("currentNode") / "children",
                    if_(has(*var_("childrenIndex"), *var_("keyItemObj")))
                        .then_(var_("child") = *var_("childrenIndex") / *var_("keyItemObj"))
                        .else_(block(
                            var_("child") = new_(_(std.TrieMapNode)),
                            set(*var_("child"), "parent", *var_("currentNode")),
                            var_("childrenIndex")("insert")("key", *var_("keyItemObj"))("value", *var_("child")))),
                    var_("currentNode") = *var_("child"),
                    if_(has(*var_("keyItem"), "next"))
                        .then_(var_("keyItem") = *var_("keyItem") / "next")
                        .else_(var_("keyItem") = _(ids.emptyObject)))),
            var_("item") = m_("list")("add")("value", new_(st_("pairType"), "constructor")("key", p_("key"))("value", p_("value"))),
            set(*var_("currentNode"), "data", *var_("item")),
            m_("size") = add(m_("size"), _(_1_)));

    /*
    void TrieMap::remove(CellI& key)
    {
        if (isA(key, w.type.List)) {
            throw "Key is not a list!";
        }

        if (&key[w.ids.size] == &w._0_) {
            return;
        }

        CellI* currentNode    = &m_rootNode;

        Visitor::visitList(key, [this, &currentNode](CellI& keyItem, int i, bool& stop) {
            CellI* children = nullptr;
            if (currentNode->missing(w.ids.children)) {
                stop        = true;
                currentNode = nullptr;
                return;
            }
            Index& childrenIndex = static_cast<Index&>(currentNode->get(w.ids.children));
            if (childrenIndex.has(keyItem)) {
                children = &childrenIndex.get(keyItem);
            } else {
                stop        = true;
                currentNode = nullptr;
                return;
            }
            currentNode = children;
        });

        if (!currentNode || currentNode->missing(w.ids.data)) {
            return;
        }
        List::Item* valueItem = &static_cast<List::Item&>((*currentNode)[w.ids.data]);
        currentNode->erase(w.ids.data);

        CellI* keyItemPtr = &key[w.ids.last];
        while (currentNode->has(w.ids.parent)) {
            CellI& keyItem = *keyItemPtr;
            CellI& parent = currentNode->get(w.ids.parent);
            CellI& child = *currentNode;
            if (child.missing(w.ids.data)) {
                if (child.missing(w.ids.children) || ( child.has(w.ids.children) && static_cast<Index&>(child[w.ids.children]).empty())) {
                    delete currentNode;
                    parent[w.ids.children].erase(keyItem[w.ids.value]);
                }
            }
            currentNode = &parent;
            if (keyItem.has(w.ids.previous)) {
                keyItemPtr = &keyItem[w.ids.previous];
            } else {
                break;
            }
        }
        if (!valueItem) {
            return;
        }
        m_list.removeItem(valueItem);
        --m_size;
    }
    */
    trieMapStructT.addMethod("remove")
        .parameters(
            parameter("key", tp_("keyType")))
        .instructions(
            var_("currentNode") = m_("rootNode"),
            var_("keyItem")     = _(ids.emptyObject),
            if_(has(p_("key"), "first"))
                .then_(var_("keyItem") = p_("key") / "first"),
            while_(notSame(*var_("keyItem"), _(ids.emptyObject)))
                .do_(block(
                    var_("keyItemObj") = *var_("keyItem") / "value",
                    var_("child")      = _(ids.emptyObject),
                    if_(missing(*var_("currentNode"), "children"))
                        .then_(return_()),
                    var_("childrenIndex") = *var_("currentNode") / "children",
                    if_(has(*var_("childrenIndex"), *var_("keyItemObj")))
                        .then_(var_("child") = *var_("childrenIndex") / *var_("keyItemObj"))
                        .else_(return_()),
                    var_("currentNode") = *var_("child"),
                    if_(has(*var_("keyItem"), "next"))
                        .then_(var_("keyItem") = *var_("keyItem") / "next")
                        .else_(var_("keyItem") = _(ids.emptyObject)))),
            if_(missing(*var_("currentNode"), "data"))
                .then_(return_()),
            var_("valueItem") = *var_("currentNode") / "data",
            erase(*var_("currentNode"), "data"),
            var_("keyItem") = p_("key") / "last",
            while_(has(*var_("currentNode"), "parent"))
                .do_(block(
                    var_("parent") = *var_("currentNode") / "parent",
                    var_("child")  = *var_("currentNode"),
                    if_(missing(*var_("child"), "data"))
                        .then_(
                            if_(or_(missing(*var_("child"), "children"), and_(has(*var_("child"), "children"), ((*var_("child") / "children")("empty")))))
                                .then_(block(
                                    delete_(*var_("currentNode")),
                                    erase(*var_("parent") / "children", *var_("keyItem") / "value")))),
                    var_("currentNode") = *var_("parent"),
                    if_(has(*var_("keyItem"), "previous"))
                        .then_(var_("keyItem") = *var_("keyItem") / "previous")
                        .else_(break_()))),
            m_("list")("remove")("item", *var_("valueItem")),
            m_("size") = subtract(m_("size"), _(_1_)));

    trieMapStructT.addMethod("size")
        .returnType(_(std.Number))
        .instructions(
            return_(m_("size")));

    trieMapStructT.addMethod("empty")
        .returnType(_(std.Boolean))
        .instructions(
            return_(equal(m_("size"), _(_0_))));

    trieMapStructT.addMethod("first")
        .returnType(tp_("valueType"))
        .instructions(
            return_(m_("list") / "first" / "value"));

    trieMapStructT.addMethod("last")
        .returnType(tp_("valueType"))
        .instructions(
            return_(m_("list") / "last" / "value"));

    trieMapStructT.addMethod("begin")
        .returnType(tt_("ListItem", "valueType", tp_("valueType")))
        .instructions(
            return_(m_("list") / "first"));

    trieMapStructT.addMethod("end")
        .returnType(tt_("ListItem", "valueType", tp_("valueType")))
        .instructions(
            return_(m_("list") / "last"));
#pragma endregion
#pragma region Set
    auto& setStructT
        = stdScope.add<StructT>("Set")
              .templateParams(
                  parameter("keyType", _(std.Struct)),
                  parameter("valueType", _(std.Struct)))
              .typeAliases(
                  typeAlias("valueType", tp_("valueType")),
                  typeAlias("listType", tt_("List", "valueType", tp_("valueType"))))
              .memberOf(_(std.Container))
              .members(
                  member("index", struct_("Index")),
                  member("size", _(std.Number)));

    setStructT.addMethod("constructor")
        .instructions(
            m_("size")  = _(_0_),
            m_("index") = new_(struct_("Index"), "constructor"));

    setStructT.addMethod("add")
        .parameters(
            parameter("value", tp_("valueType")))
        .instructions(
            if_(has(m_("index"), p_("value")))
                .then_(return_()),
            m_("index")("insert")("key", p_("value"))("value", p_("value")),
            m_("size") = add(m_("size"), _(_1_)));

    setStructT.addMethod("contains")
        .parameters(
            parameter("value", tp_("valueType")))
        .returnType(_(std.Boolean))
        .instructions(
            return_(has(m_("index"), p_("value"))));

    setStructT.addMethod("remove")
        .parameters(
            parameter("value", tp_("valueType")))
        .instructions(
            if_(missing(m_("index"), p_("value")))
                .then_(return_()),
            m_("index")("remove")("key", p_("value")),
            m_("size") = subtract(m_("size"), _(_1_)));

    setStructT.addMethod("first")
        .returnType(tp_("valueType"))
        .instructions(
            return_(m_("index") / "struct" / "slots" / "list" / "first" / "value" / "key"));

    setStructT.addMethod("last")
        .returnType(tp_("valueType"))
        .instructions(
            return_(m_("index") / "struct" / "slots" / "list" / "last" / "value" / "key"));

    setStructT.addMethod("begin")
        .returnType(tt_("ListItem", "valueType", tp_("valueType")))
        .instructions(
            return_(m_("index") / "struct" / "slots" / "list" / "last"));

    setStructT.addMethod("end")
        .returnType(tt_("ListItem", "valueType", tp_("valueType")))
        .instructions(
            return_(m_("list") / "last"));

    setStructT.addMethod("size")
        .returnType(_(std.Number))
        .instructions(
            return_(m_("size")));

    setStructT.addMethod("empty")
        .returnType(_(std.Boolean))
        .instructions(return_(equal(m_("size"), _(_0_))));
#pragma endregion
}

StdLib::StdLib(World& w, Ast::Scope& parentScope) :
    Library(w, parentScope)
{
    StdLibAst stdLibAst(w, parentScope.add<Ast::Scope>("std"));
}

} // namespace cells
} // namespace infocell
