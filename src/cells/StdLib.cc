#include "Compiler.h"
#include "StdLib.h"
#include "World.h"

namespace infocell {
namespace cells {

// ============================================================================
Std::EBoolean::EBoolean(World& w, CellI& type, const std::string& label) :
    Object(w, type, label),
    true_(w, w.std.Boolean, "true"),
    false_(w, w.std.Boolean, "false")
{
}

Std::EDirection::EDirection(World& w, CellI& type, const std::string& label) :
    Object(w, type, label),
    up(w, w.std.Direction, "up"),
    down(w, w.std.Direction, "down"),
    left(w, w.std.Direction, "left"),
    right(w, w.std.Direction, "right")
{
}

Std::ENumberSign::ENumberSign(World& w, CellI& type, const std::string& label) :
    Object(w, type, label),
    positive(w, w.std.NumberSign, "positive"),
    negative(w, w.std.NumberSign, "negative")
{
}

// ============================================================================
Std::Op::Op(World& w) :
    w(w),
    Activate(w, w.std.Struct, "op::Activate"),
    Add(w, w.std.Struct, "op::Add"),
    And(w, w.std.Struct, "op::And"),
    Base(w, w.std.Struct, "op::Base"),
    Block(w, w.std.Struct, "op::Block"),
    Call(w, w.std.Struct, "op::Call"),
    ConstVar(w, w.std.Struct, "op::ConstVar"),
    Delete(w, w.std.Struct, "op::Delete"),
    Divide(w, w.std.Struct, "op::Divide"),
    Do(w, w.std.Struct, "op::Do"),
    Equal(w, w.std.Struct, "op::Equal"),
    Erase(w, w.std.Struct, "op::Erase"),
    Function(w, w.std.Struct, "op::Function"),
    Get(w, w.std.Struct, "op::Get"),
    GreaterThan(w, w.std.Struct, "op::GreaterThan"),
    GreaterThanOrEqual(w, w.std.Struct, "op::GreaterThanOrEqual"),
    Has(w, w.std.Struct, "op::Has"),
    If(w, w.std.Struct, "op::If"),
    LessThan(w, w.std.Struct, "op::LessThan"),
    LessThanOrEqual(w, w.std.Struct, "op::LessThanOrEqual"),
    Missing(w, w.std.Struct, "op::Missing"),
    Multiply(w, w.std.Struct, "op::Multiply"),
    New(w, w.std.Struct, "op::New"),
    Not(w, w.std.Struct, "op::Not"),
    NotEqual(w, w.std.Struct, "op::NotEqual"),
    NotSame(w, w.std.Struct, "op::NotSame"),
    Or(w, w.std.Struct, "op::Or"),
    Return(w, w.std.Struct, "op::Return"),
    Same(w, w.std.Struct, "op::Same"),
    Set(w, w.std.Struct, "op::Set"),
    Subtract(w, w.std.Struct, "op::Subtract"),
    Var(w, w.std.Struct, "op::Var"),
    While(w, w.std.Struct, "op::While")
{
}

// ============================================================================
Std::Ast::Ast(World& w) :
    w(w),
    Add(w, w.std.Struct, "ast::Add"),
    And(w, w.std.Struct, "ast::And"),
    AssociatedType(w, w.std.Struct, "ast::AssociatedType"),
    Base(w, w.std.Struct, "ast::Base"),
    Block(w, w.std.Struct, "ast::Block"),
    Break(w, w.std.Struct, "ast::Break"),
    Call(w, w.std.Struct, "ast::Call"),
    Cell(w, w.std.Struct, "ast::Cell"),
    Continue(w, w.std.Struct, "ast::Continue"),
    Delete(w, w.std.Struct, "ast::Delete"),
    Divide(w, w.std.Struct, "ast::Divide"),
    Do(w, w.std.Struct, "ast::Do"),
    Enum(w, w.std.Struct, "ast::Enum"),
    EnumValue(w, w.std.Struct, "ast::EnumValue"),
    Equal(w, w.std.Struct, "ast::Equal"),
    Erase(w, w.std.Struct, "ast::Erase"),
    For(w, w.std.Struct, "ast::For"),
    Function(w, w.std.Struct, "ast::Function"),
    FunctionT(w, w.std.Struct, "ast::FunctionT"),
    Get(w, w.std.Struct, "ast::Get"),
    GreaterThan(w, w.std.Struct, "ast::GreaterThan"),
    GreaterThanOrEqual(w, w.std.Struct, "ast::GreaterThanOrEqual"),
    Has(w, w.std.Struct, "ast::Has"),
    If(w, w.std.Struct, "ast::If"),
    LessThan(w, w.std.Struct, "ast::LessThan"),
    LessThanOrEqual(w, w.std.Struct, "ast::LessThanOrEqual"),
    Match(w, w.std.Struct, "ast::Match"),
    Member(w, w.std.Struct, "ast::Member"),
    Missing(w, w.std.Struct, "ast::Missing"),
    Multiply(w, w.std.Struct, "ast::Multiply"),
    New(w, w.std.Struct, "ast::New"),
    Not(w, w.std.Struct, "ast::Not"),
    NotEqual(w, w.std.Struct, "ast::NotEqual"),
    NotSame(w, w.std.Struct, "ast::NotSame"),
    Or(w, w.std.Struct, "ast::Or"),
    Parameter(w, w.std.Struct, "ast::Parameter"),
    ResolvedType(w, w.std.Struct, "ast::ResolvedType"),
    Return(w, w.std.Struct, "ast::Return"),
    Same(w, w.std.Struct, "ast::Same"),
    Scope(w, w.std.Struct, "ast::Scope"),
    Self(w, w.std.Struct, "ast::Self"),
    SelfFn(w, w.std.Struct, "ast::SelfFn"),
    Set(w, w.std.Struct, "ast::Set"),
    Slot(w, w.std.Struct, "ast::Slot"),
    StaticCall(w, w.std.Struct, "ast::StaticCall"),
    Struct(w, w.std.Struct, "ast::Struct"),
    StructName(w, w.std.Struct, "ast::StructName"),
    StructT(w, w.std.Struct, "ast::StructT"),
    TypeAlias(w, w.std.Struct, "ast::TypeAlias"),
    Subtract(w, w.std.Struct, "ast::Subtract"),
    TemplatedType(w, w.std.Struct, "ast::TemplatedType"),
    TemplateParam(w, w.std.Struct, "ast::TemplateParam"),
    Throw(w, w.std.Struct, "ast::Throw"),
    Trait(w, w.std.Struct, "ast::Trait"),
    TraitImpl(w, w.std.Struct, "ast::TraitImpl"),
    Try(w, w.std.Struct, "ast::Try"),
    TypedEnumValue(w, w.std.Struct, "ast::TypedEnumValue"),
    Var(w, w.std.Struct, "ast::Var"),
    While(w, w.std.Struct, "ast::While")
{
}

// ============================================================================
Std::Std(World& w) :
    w(w),
    Boolean(w, w.std.Enum, "Boolean"),
    Cell(w, w.std.Struct, "Cell"),
    Char(w, w.std.Struct, "Char"),
    Container(w, w.std.Struct, "Conatainer"),
    Digit(w, w.std.Struct, "Digit"),
    Direction(w, w.std.Enum, "Direction"),
    Enum(w, w.std.Struct, "Enum"),
    Index(w, w.std.Struct, "Index"),
    KVPair(w, w.std.Struct, "KVPair"),
    Library(w, w.std.Struct, "Library"),
    List(w, w.std.Struct, "List"),
    ListItem(w, w.std.Struct, "ListItem"),
    Map(w, w.std.Struct, "Map"),
    Number(w, w.std.Struct, "Number"),
    NumberSign(w, w.std.Enum, "NumberSign"),
    OpState(w, w.std.Struct, "OpState"),
    Slot(w, w.std.Struct, "Slot"),
    Stack(w, w.std.Struct, "Stack"),
    StackFrame(w, w.std.Struct, "StackFrame"),
    String(w, w.std.Struct, "String"),
    Struct(w, w.std.Struct, "Struct"),
    StructReference(w, w.std.Struct, "StructReference"),
    TrieMap(w, w.std.Struct, "TrieMap"),
    TrieMapNode(w, w.std.Struct, "TrieMapNode"),
    op(w),
    ast(w)
{
}

cells::CellI& Std::slot(cells::CellI& key, cells::CellI& type)
{
    CellI& ret = *new Object(w, w.std.Slot);
    ret.set(w.id.key, key);
    ret.set(w.id.type, type);

    return ret;
}

cells::CellI& Std::slot(const std::string& key, cells::CellI& type)
{
    CellI& ret = *new Object(w, w.std.Slot);
    ret.set(w.id.key, w.name(key));
    ret.set(w.id.type, type);

    return ret;
}

cells::CellI& Std::kvPair(cells::CellI& key, cells::CellI& value)
{
    CellI& ret = *new Object(w, w.std.KVPair);
    ret.set(w.id.key, key);
    ret.set(w.id.value, value);

    return ret;
}

// ============================================================================
class StdLibAst : public AstHelper
{
public:
    class Traits : public AstHelper
    {
        Traits(World& w, Ast::Scope& std);
        friend class StdLibAst;

    public:
        Ast::Trait& Iterable;
        Ast::Trait& Iterator;
    };

    StdLibAst(World& w, Ast::Scope& scope);

private:
    void createOp();
    void createAst();
    void createEnums();

    void createIndex();
    void createKVPair();
    void createList();
    void createListItem();
    void createMap();
    void createSet();
    void createStruct();
    void createTrieMap();

    Ast::Scope& stdScope;
    Traits traits;
};

// ============================================================================
StdLibAst::Traits::Traits(World& w, Ast::Scope& stdScope) :
    AstHelper(w),
    Iterable(stdScope.add<Trait>("Iterable")),
    Iterator(stdScope.add<Trait>("Iterator"))
{
    /*
    trait Iterable {
        type Iterator: std::Iterator;

        fn iterator() -> Self::Iterator;
    }
    */
    Iterable.
        associatedTypes(
                parameter("Iterator", _("Iterator")))
        .addMethod("iterator")
        .returnType(at_("Iterator"));

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
    Iterator.
        associatedTypes(
                parameter("ValueType", _(std.Struct)));

    Iterator.addMethod("isContainerEmpty").returnType(_(std.Boolean));
    Iterator.addMethod("goToFirstNode");
    Iterator.addMethod("getCurrentNodeValue").returnType(at_("ValueType"));
    Iterator.addMethod("hasNextNode").returnType(_(std.Boolean));
    Iterator.addMethod("goToNextNode");
}

// ============================================================================
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
            var_("x")("isA")("__type__", m_("valueType")),
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

// ============================================================================
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

    astScope.add<Struct>("AssociatedType")
        .members(
            member("key", "std::Cell"));

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
            member("compiledStruct", "std::op::Base"),
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
            member("structType", "std::Struct"),
            member("scope", "Base"),
            member("methods", MapOf(std.Cell, std.ast.Function)),
            member("associatedTypes", MapOf(std.Cell, std.ast.Base)),
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

// ============================================================================
void StdLibAst::createEnums()
{
    stdScope.add<Enum>("Boolean")
        .values(
            ev_("false"),
            ev_("true"));

    stdScope.add<Enum>("Direction")
        .values(
            ev_("up"),
            ev_("down"),
            ev_("left"),
            ev_("right"));

    stdScope.add<Enum>("NumberSign")
        .values(
            ev_("positive"),
            ev_("negative"));
}

// ============================================================================
void StdLibAst::createIndex()
{
#pragma region Index
    auto& indexStruct
        = stdScope.add<Struct>("Index")
              .memberOf(_(std.Struct));

    indexStruct.addMethod("constructor")
        .instructions(
            set(self(), "__type__", new_("Struct", "constructorWithRecursiveType")),
            set(m_("__type__"), "methods", get(__type__("Index"), _("methods"))),
            set(m_("__type__"), "memberOf", _(map(std.Struct, std.Struct, std.Index, std.Index))));

    indexStruct.addMethod("constructorWithSelfType")
        .parameters(
            parameter("indexType", _(std.Struct)))
        .instructions(
            if_(missing(p_("indexType"), _("sharedObject")))
                .then_(block(set(p_("indexType"), "sharedObject", new_(_(std.Slot))),
                             set(p_("indexType") / "sharedObject", "key", self()),
                             set(p_("indexType") / "sharedObject", "type", __type__("Index")))),
            set(p_("indexType"), "methods", m_("__type__") / "methods"),
            set(self(), "__type__", p_("indexType")));

    /*
    void Index::insert(CellI& key, CellI& value)
    {
        if (&key == &"__type__") {
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
            if_(same(p_("key"), _("__type__")))
                .then_(return_()),
            set(self(), p_("key"), p_("value")),
            if_(and_(has(m_("__type__"), "sharedObject"), same(m_("__type__") / "sharedObject" / "key", self())))
                .then_(return_()),
            m_("__type__")("addSlot")("key", p_("key"))("type", _(std.Slot)));

    indexStruct.addMethod("empty")
        .returnType(_(std.Boolean))
        .instructions(
            return_((m_("__type__") / "slots")("empty")));

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
            if_(not_(m_("__type__")("hasSlot")("key", p_("key"))))
                .then_(return_()),
            erase(self(), p_("key")),
            m_("__type__")("removeSlot")("key", p_("key")));

    indexStruct.addMethod("size")
        .returnType(_(std.Number))
        .instructions(
            return_((m_("__type__") / "slots")("size")));
#pragma endregion
}

// ============================================================================
void StdLibAst::createKVPair()
{
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
              .memberOf(__type__("KVPair"))
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
}

// ============================================================================
void StdLibAst::createList()
{
#pragma region List
    stdScope.add<Struct>("List")
        .typeAliases(
            typeAlias("itemType", __type__("ListItem")),
            typeAlias("valueType", __type__("Cell")))
        .members(
            member("first", "ListItem"),
            member("last", "ListItem"),
            member("size", _(std.Number)));

    auto& listStructT
        = stdScope.add<StructT>("List")
              .templateParams(
                  parameter("valueType", _(std.Struct)))
              .memberOf(
                  __type__("Container"),
                  __type__("List"))
              .typeAliases(
                  typeAlias("itemType", tt_("ListItem", "valueType", tp_("valueType"))),
                  typeAlias("valueType", tp_("valueType")))
              .members(
                  member("first", ta_("itemType")),
                  member("last", ta_("itemType")),
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
    impl<T> trait Iterable for List<T> {
        type Iterator = ListIterator<T>;

        fn iterator() -> Self::Iterator {
            return new Self::Iterator(list: self())
        }
    }
    */
    auto& implIterableTraitForListT
        = listStructT.addTraitImpl("Iterable")
              .associatedTypes(
                  parameter("Iterator", tt_("ListIterator", "valueType", tp_("valueType"))));

    implIterableTraitForListT.addMethod("iterator")
        .returnType(at_("Iterator"))
        .instructions(
            return_(new_(at_("Iterator"), "constructor")("list", self())));

    // impl Iterator for ListItem<T>
    auto& implIteratorTraitForListT
        = listIteratorStructT.addTraitImpl("Iterator")
              .associatedTypes(
                  parameter("ValueType", tp_("valueType")));

    implIteratorTraitForListT.addMethod("isContainerEmpty")
        .returnType(_(std.Boolean))
        .instructions(
            return_(equal(m_("list") / _(id.size), _(_0_))));

    implIteratorTraitForListT.addMethod("goToFirstNode")
        .instructions(
            set(self(), "node", m_("list") / _(id.first)));

    implIteratorTraitForListT.addMethod("getCurrentNodeValue")
        .returnType(at_("ValueType"))
        .instructions(
            return_(m_("node") / _(id.value)));

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
        .returnType(ta_("itemType"))
        .instructions(
            var_("item") = new_(ta_("itemType"), "constructor")("value", p_("value")),
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
        .returnType(ta_("itemType"))
        .instructions(
            return_(m_("first")));

    listStructT.addMethod("end")
        .returnType(ta_("itemType"))
        .instructions(
            return_(m_("last")));
#pragma endregion
}

// ============================================================================
void StdLibAst::createListItem()
{
#pragma region ListItem
    stdScope.add<Struct>("ListItem")
        .typeAliases(
            typeAlias("ValueType", __type__("Cell")))
        .members(
            member("previous", "ListItem"),
            member("next", "ListItem"),
            member("value", ta_("ValueType")));

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
}

// ============================================================================
void StdLibAst::createMap()
{
#pragma region Map
    stdScope.add<Struct>("Map")
        .typeAliases(
            typeAlias("keyType", __type__("Cell")),
            typeAlias("valueType", __type__("Cell")),
            typeAlias("listType", tt_("List", "valueType", __type__("Cell"))))
        .memberOf(__type__("Container"))
        .members(
            member("list", ta_("listType")),
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
              .memberOf(__type__("Map"))
              .members(
                  member("list", ta_("listType")),
                  member("index", "Index"),
                  member("size", _(std.Number)));

    mapStructT.addMethod("constructor")
        .instructions(
            m_("size")  = _(_0_),
            m_("list")  = new_(ta_("listType"), "constructor"),
            m_("index") = new_("Index", "constructor"));

    mapStructT.addMethod("constructorWithIndexType")
        .parameters(
            parameter("indexType", _(std.Struct)))
        .instructions(
            m_("size")  = _(_0_),
            m_("list")  = new_(ta_("listType"), "constructor"),
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
            return m_index[key][w.id.value];
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
        if (&key == &"__type__") {
            throw "id.type can not be stored in a map!";
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
            if_(same(p_("key"), _("__type__")))
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
}

// ============================================================================
void StdLibAst::createSet()
{
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
                  member("index", __type__("Index")),
                  member("size", _(std.Number)));

    setStructT.addMethod("constructor")
        .instructions(
            m_("size")  = _(_0_),
            m_("index") = new_(__type__("Index"), "constructor"));

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
            return_(m_("index") / "__type__" / "slots" / "list" / "first" / "value" / "key"));

    setStructT.addMethod("last")
        .returnType(tp_("valueType"))
        .instructions(
            return_(m_("index") / "__type__" / "slots" / "list" / "last" / "value" / "key"));

    setStructT.addMethod("begin")
        .returnType(tt_("ListItem", "valueType", tp_("valueType")))
        .instructions(
            return_(m_("index") / "__type__" / "slots" / "list" / "last"));

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

// ============================================================================
void StdLibAst::createStruct()
{
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
}

// ============================================================================
void StdLibAst::createTrieMap()
{
#pragma region TrieMap
    // TODO This can be a template but nevermind ...

    stdScope.add<Struct>("TrieMap")
        .typeAliases(
            typeAlias("keyType", __type__("Cell")),
            typeAlias("valueType", __type__("Cell")),
            typeAlias("pairType", tt_("KVPair", "keyType", __type__("Cell"), "valueType", __type__("Cell"))),
            typeAlias("listType", tt_("List", "valueType", ta_("pairType"))))
        .memberOf(__type__("Container"))
        .members(
            member("list", ta_("listType")),
            member("rootNode", __type__("TrieMapNode")),
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
                  typeAlias("listType", tt_("List", "valueType", ta_("pairType"))))
              .memberOf(_(std.Container), _(std.TrieMap))
              .members(
                  member("list", ta_("listType")),
                  member("rootNode", _(std.TrieMapNode)),
                  member("size", _(std.Number)));

    trieMapStructT.addMethod("constructor")
        .instructions(
            m_("list")     = new_(ta_("listType"), "constructor"),
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
            if (currentNode->missing(w.id.children)) {
                stop        = true;
                currentNode = nullptr;
                return;
            }
            Index& childrenIndex = static_cast<Index&>(currentNode->get(w.id.children));
            if (childrenIndex.has(keyItem)) {
                children = &childrenIndex.get(keyItem);
            } else {
                stop        = true;
                currentNode = nullptr;
                return;
            }
            currentNode = children;
        });

        if (!currentNode || currentNode->missing(w.id.data)) {
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
            var_("keyItem")     = _(id.emptyObject),
            if_(has(p_("key"), "first"))
                .then_(var_("keyItem") = p_("key") / "first"),
            while_(notSame(*var_("keyItem"), _(id.emptyObject)))
                .do_(block(
                    var_("keyItemObj") = *var_("keyItem") / "value",
                    var_("child")      = _(id.emptyObject),
                    if_(missing(*var_("currentNode"), "children"))
                        .then_(return_(false_())),
                    var_("childrenIndex") = *var_("currentNode") / "children",
                    if_(has(*var_("childrenIndex"), *var_("keyItemObj")))
                        .then_(var_("child") = *var_("childrenIndex") / *var_("keyItemObj"))
                        .else_(return_(false_())),
                    var_("currentNode") = *var_("child"),
                    if_(has(*var_("keyItem"), "next"))
                        .then_(var_("keyItem") = *var_("keyItem") / "next")
                        .else_(var_("keyItem") = _(id.emptyObject)))),
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
            if (currentNode->missing(w.id.children)) {
                stop        = true;
                currentNode = nullptr;
                return;
            }
            Index& childrenIndex = static_cast<Index&>(currentNode->get(w.id.children));
            if (childrenIndex.has(keyItem)) {
                children = &childrenIndex.get(keyItem);
            } else {
                stop        = true;
                currentNode = nullptr;
                return;
            }
            currentNode = children;
        });

        if (!currentNode || currentNode->missing(w.id.data)) {
            throw "No such key!";
        }

        return (*currentNode)[w.id.data][w.id.value][w.id.value];
    }
    */
    trieMapStructT.addMethod("getValue")
        .parameters(
            parameter("key", tp_("keyType")))
        .returnType(tp_("valueType"))
        .instructions(
            var_("currentNode") = m_("rootNode"),
            var_("keyItem")     = _(id.emptyObject),
            if_(has(p_("key"), "first"))
                .then_(var_("keyItem") = p_("key") / "first"),
            while_(notSame(*var_("keyItem"), _(id.emptyObject)))
                .do_(block(
                    var_("keyItemObj") = *var_("keyItem") / "value",
                    var_("child")      = _(id.emptyObject),
                    if_(missing(*var_("currentNode"), "children"))
                        .then_(return_(_(id.emptyObject))),
                    var_("childrenIndex") = *var_("currentNode") / "children",
                    if_(has(*var_("childrenIndex"), *var_("keyItemObj")))
                        .then_(var_("child") = *var_("childrenIndex") / *var_("keyItemObj"))
                        .else_(return_(_(id.emptyObject))),
                    var_("currentNode") = *var_("child"),
                    if_(has(*var_("keyItem"), "next"))
                        .then_(var_("keyItem") = *var_("keyItem") / "next")
                        .else_(var_("keyItem") = _(id.emptyObject)))),
            if_(missing(*var_("currentNode"), "data"))
                .then_(return_(_(id.emptyObject))),
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
            if (currentNode->missing(w.id.children)) {
                currentNode->set(w.id.children, *new Index(w));
            }
            Index& childrenIndex = static_cast<Index&>(currentNode->get(w.id.children));
            if (childrenIndex.has(keyItem)) {
                child = &childrenIndex.get(keyItem);
            } else {
                child = new Object(w, w.type.TrieMapNode);
                child->set(w.id.parent, *currentNode);
                childrenIndex.insert(keyItem, *child);
            }
            currentNode = child;
        });

        List::Item& item = *m_list.add(w.type.kvPair(key, value));
        currentNode->set(w.id.data, item);
        ++m_size;
    }
    */
    trieMapStructT.addMethod("add")
        .parameters(
            parameter("key", tp_("keyType")),
            parameter("value", tp_("valueType")))
        .instructions(
            var_("currentNode") = m_("rootNode"),
            var_("keyItem")     = _(id.emptyObject),
            if_(has(p_("key"), "first"))
                .then_(var_("keyItem") = p_("key") / "first"),
            while_(notSame(*var_("keyItem"), _(id.emptyObject)))
                .do_(block(
                    var_("keyItemObj") = *var_("keyItem") / "value",
                    var_("child")      = _(id.emptyObject),
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
                        .else_(var_("keyItem") = _(id.emptyObject)))),
            var_("item") = m_("list")("add")("value", new_(ta_("pairType"), "constructor")("key", p_("key"))("value", p_("value"))),
            set(*var_("currentNode"), "data", *var_("item")),
            m_("size") = add(m_("size"), _(_1_)));

    /*
    void TrieMap::remove(CellI& key)
    {
        if (isA(key, w.type.List)) {
            throw "Key is not a list!";
        }

        if (&key[w.id.size] == &w._0_) {
            return;
        }

        CellI* currentNode    = &m_rootNode;

        Visitor::visitList(key, [this, &currentNode](CellI& keyItem, int i, bool& stop) {
            CellI* children = nullptr;
            if (currentNode->missing(w.id.children)) {
                stop        = true;
                currentNode = nullptr;
                return;
            }
            Index& childrenIndex = static_cast<Index&>(currentNode->get(w.id.children));
            if (childrenIndex.has(keyItem)) {
                children = &childrenIndex.get(keyItem);
            } else {
                stop        = true;
                currentNode = nullptr;
                return;
            }
            currentNode = children;
        });

        if (!currentNode || currentNode->missing(w.id.data)) {
            return;
        }
        List::Item* valueItem = &static_cast<List::Item&>((*currentNode)[w.id.data]);
        currentNode->erase(w.id.data);

        CellI* keyItemPtr = &key[w.id.last];
        while (currentNode->has(w.id.parent)) {
            CellI& keyItem = *keyItemPtr;
            CellI& parent = currentNode->get(w.id.parent);
            CellI& child = *currentNode;
            if (child.missing(w.id.data)) {
                if (child.missing(w.id.children) || ( child.has(w.id.children) && static_cast<Index&>(child[w.id.children]).empty())) {
                    delete currentNode;
                    parent[w.id.children].erase(keyItem[w.id.value]);
                }
            }
            currentNode = &parent;
            if (keyItem.has(w.id.previous)) {
                keyItemPtr = &keyItem[w.id.previous];
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
            var_("keyItem")     = _(id.emptyObject),
            if_(has(p_("key"), "first"))
                .then_(var_("keyItem") = p_("key") / "first"),
            while_(notSame(*var_("keyItem"), _(id.emptyObject)))
                .do_(block(
                    var_("keyItemObj") = *var_("keyItem") / "value",
                    var_("child")      = _(id.emptyObject),
                    if_(missing(*var_("currentNode"), "children"))
                        .then_(return_()),
                    var_("childrenIndex") = *var_("currentNode") / "children",
                    if_(has(*var_("childrenIndex"), *var_("keyItemObj")))
                        .then_(var_("child") = *var_("childrenIndex") / *var_("keyItemObj"))
                        .else_(return_()),
                    var_("currentNode") = *var_("child"),
                    if_(has(*var_("keyItem"), "next"))
                        .then_(var_("keyItem") = *var_("keyItem") / "next")
                        .else_(var_("keyItem") = _(id.emptyObject)))),
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
}

// ============================================================================
StdLibAst::StdLibAst(World& w, Ast::Scope& scope) :
    AstHelper(w),
    stdScope(scope),
    traits(w, scope)
{
    createOp();
    createAst();
    createEnums();

    stdScope.add<Struct>("Cell");

    stdScope.add<Struct>("Char");

    stdScope.add<Struct>("Container");

    stdScope.add<Struct>("Digit");

    stdScope.add<Struct>("Enum")
        .members(
            member("name", tt_("List", "valueType", "Char")),
            member("fullyQualifiedName", "std::Cell"),
            member("slots", tt_("Map", "keyType", "Cell", "valueType", "Slot")),
            member("enum", "Boolean"),
            member("incomplete", "Boolean"),
            member("typeAliases", tt_("Map", "keyType", "Cell", "valueType", "Struct")),
            member("ast", "std::ast::Base"),
            member("methods", tt_("Map", "keyType", "Cell", "valueType", "op::Function")),
            member("values", tt_("Map", "keyType", "Cell", "valueType", "Struct")));

    createIndex();

    createKVPair();

    stdScope.add<Struct>("Library")
        .members(
            member("scope", "ast::Scope"),
            member("resolvedScope", "ast::Scope"),
            member("functions", tt_("TrieMap", "keyType", "Cell", "valueType", "op::Function")),
            member("structs", tt_("TrieMap", "keyType", "Cell", "valueType", "Struct")),
            member("variables", tt_("TrieMap", "keyType", "Cell", "valueType", "op::Var")));

    createList();
    createListItem();
    createMap();

    stdScope.add<Struct>("Number")
        .members(
            member("value", ListOf(std.Digit)),
            member("sign", "NumberSign"));

    stdScope.add<Struct>("OpState")
        .members(
            member("op", "op::Base"),
            member("state", "Cell"),
            member("value", "Cell"));

    createSet();

    stdScope.add<Struct>("Slot")
        .members(
            member("key", "Cell"),
            member("type", "Struct"));

    stdScope.add<Struct>("Stack");

    stdScope.add<Struct>("StackFrame")
        .members(
            member("method", "op::Function"),
            member("ops", "List"),
            member("input", "Index"),
            member("localVars", "Index"));

    stdScope.add<Struct>("String");

    createStruct();

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

    createTrieMap();

    stdScope.add<Struct>("TrieMapNode")
        .members(
            member("children", "Index"),
            member("data", "ListItem"),
            member("parent", "TrieMapNode"));

    stdScope.add<Struct>("Void");
}

StdLib::StdLib(World& w, Ast::Scope & parentScope, Compiler& compiler) :
    Library(w, parentScope)
{
    Std& std = w.std;
    StdLibAst stdLibAst(w, parentScope.add<Ast::Scope>("std"));

    compiler.reigisterStructBeforeCompilation(w.tt_("std::List", "valueType", w._(std.Char))); // TODO instantiate on demand in getStruct

    compiler.registerBuiltInStruct("std::op::Activate", std.op.Activate);
    compiler.registerBuiltInStruct("std::op::Add", std.op.Add);
    compiler.registerBuiltInStruct("std::op::And", std.op.And);
    compiler.registerBuiltInStruct("std::op::Base", std.op.Base);
    compiler.registerBuiltInStruct("std::op::Block", std.op.Block);
    compiler.registerBuiltInStruct("std::op::Call", std.op.Call);
    compiler.registerBuiltInStruct("std::op::ConstVar", std.op.ConstVar);
    compiler.registerBuiltInStruct("std::op::Delete", std.op.Delete);
    compiler.registerBuiltInStruct("std::op::Divide", std.op.Divide);
    compiler.registerBuiltInStruct("std::op::Do", std.op.Do);
    compiler.registerBuiltInStruct("std::op::Equal", std.op.Equal);
    compiler.registerBuiltInStruct("std::op::Erase", std.op.Erase);
    compiler.registerBuiltInStruct("std::op::Function", std.op.Function);
    compiler.registerBuiltInStruct("std::op::Get", std.op.Get);
    compiler.registerBuiltInStruct("std::op::GreaterThan", std.op.GreaterThan);
    compiler.registerBuiltInStruct("std::op::GreaterThanOrEqual", std.op.GreaterThanOrEqual);
    compiler.registerBuiltInStruct("std::op::Has", std.op.Has);
    compiler.registerBuiltInStruct("std::op::If", std.op.If);
    compiler.registerBuiltInStruct("std::op::LessThan", std.op.LessThan);
    compiler.registerBuiltInStruct("std::op::LessThanOrEqual", std.op.LessThanOrEqual);
    compiler.registerBuiltInStruct("std::op::Missing", std.op.Missing);
    compiler.registerBuiltInStruct("std::op::Multiply", std.op.Multiply);
    compiler.registerBuiltInStruct("std::op::New", std.op.New);
    compiler.registerBuiltInStruct("std::op::Not", std.op.Not);
    compiler.registerBuiltInStruct("std::op::NotEqual", std.op.NotEqual);
    compiler.registerBuiltInStruct("std::op::NotSame", std.op.NotSame);
    compiler.registerBuiltInStruct("std::op::Or", std.op.Or);
    compiler.registerBuiltInStruct("std::op::Return", std.op.Return);
    compiler.registerBuiltInStruct("std::op::Same", std.op.Same);
    compiler.registerBuiltInStruct("std::op::Set", std.op.Set);
    compiler.registerBuiltInStruct("std::op::Subtract", std.op.Subtract);
    compiler.registerBuiltInStruct("std::op::Var", std.op.Var);
    compiler.registerBuiltInStruct("std::op::While", std.op.While);

    compiler.registerBuiltInStruct("std::ast::Add", std.ast.Add);
    compiler.registerBuiltInStruct("std::ast::And", std.ast.And);
    compiler.registerBuiltInStruct("std::ast::AssociatedType", std.ast.AssociatedType);
    compiler.registerBuiltInStruct("std::ast::Base", std.ast.Base);
    compiler.registerBuiltInStruct("std::ast::Block", std.ast.Block);
    compiler.registerBuiltInStruct("std::ast::Break", std.ast.Break);
    compiler.registerBuiltInStruct("std::ast::Call", std.ast.Call);
    compiler.registerBuiltInStruct("std::ast::Cell", std.ast.Cell);
    compiler.registerBuiltInStruct("std::ast::Continue", std.ast.Continue);
    compiler.registerBuiltInStruct("std::ast::Delete", std.ast.Delete);
    compiler.registerBuiltInStruct("std::ast::Divide", std.ast.Divide);
    compiler.registerBuiltInStruct("std::ast::Do", std.ast.Do);
    compiler.registerBuiltInStruct("std::ast::Enum", std.ast.Enum);
    compiler.registerBuiltInStruct("std::ast::EnumValue", std.ast.EnumValue);
    compiler.registerBuiltInStruct("std::ast::Equal", std.ast.Equal);
    compiler.registerBuiltInStruct("std::ast::Erase", std.ast.Erase);
    compiler.registerBuiltInStruct("std::ast::For", std.ast.For);
    compiler.registerBuiltInStruct("std::ast::Function", std.ast.Function);
    compiler.registerBuiltInStruct("std::ast::FunctionT", std.ast.FunctionT);
    compiler.registerBuiltInStruct("std::ast::Get", std.ast.Get);
    compiler.registerBuiltInStruct("std::ast::GreaterThan", std.ast.GreaterThan);
    compiler.registerBuiltInStruct("std::ast::GreaterThanOrEqual", std.ast.GreaterThanOrEqual);
    compiler.registerBuiltInStruct("std::ast::Has", std.ast.Has);
    compiler.registerBuiltInStruct("std::ast::If", std.ast.If);
    compiler.registerBuiltInStruct("std::ast::LessThan", std.ast.LessThan);
    compiler.registerBuiltInStruct("std::ast::LessThanOrEqual", std.ast.LessThanOrEqual);
    compiler.registerBuiltInStruct("std::ast::Match", std.ast.Match);
    compiler.registerBuiltInStruct("std::ast::Member", std.ast.Member);
    compiler.registerBuiltInStruct("std::ast::Missing", std.ast.Missing);
    compiler.registerBuiltInStruct("std::ast::Multiply", std.ast.Multiply);
    compiler.registerBuiltInStruct("std::ast::New", std.ast.New);
    compiler.registerBuiltInStruct("std::ast::Not", std.ast.Not);
    compiler.registerBuiltInStruct("std::ast::NotEqual", std.ast.NotEqual);
    compiler.registerBuiltInStruct("std::ast::NotSame", std.ast.NotSame);
    compiler.registerBuiltInStruct("std::ast::Or", std.ast.Or);
    compiler.registerBuiltInStruct("std::ast::Parameter", std.ast.Parameter);
    compiler.registerBuiltInStruct("std::ast::ResolvedType", std.ast.ResolvedType);
    compiler.registerBuiltInStruct("std::ast::Return", std.ast.Return);
    compiler.registerBuiltInStruct("std::ast::Same", std.ast.Same);
    compiler.registerBuiltInStruct("std::ast::Scope", std.ast.Scope);
    compiler.registerBuiltInStruct("std::ast::Self", std.ast.Self);
    compiler.registerBuiltInStruct("std::ast::SelfFn", std.ast.SelfFn);
    compiler.registerBuiltInStruct("std::ast::Set", std.ast.Set);
    compiler.registerBuiltInStruct("std::ast::Slot", std.ast.Slot);
    compiler.registerBuiltInStruct("std::ast::StaticCall", std.ast.StaticCall);
    compiler.registerBuiltInStruct("std::ast::Struct", std.ast.Struct);
    compiler.registerBuiltInStruct("std::ast::StructName", std.ast.StructName);
    compiler.registerBuiltInStruct("std::ast::StructT", std.ast.StructT);
    compiler.registerBuiltInStruct("std::ast::Subtract", std.ast.Subtract);
    compiler.registerBuiltInStruct("std::ast::TypeAlias", std.ast.TypeAlias);
    compiler.registerBuiltInStruct("std::ast::TemplatedType", std.ast.TemplatedType);
    compiler.registerBuiltInStruct("std::ast::TemplateParam", std.ast.TemplateParam);
    compiler.registerBuiltInStruct("std::ast::Trait", std.ast.Trait);
    compiler.registerBuiltInStruct("std::ast::TraitImpl", std.ast.TraitImpl);
    compiler.registerBuiltInStruct("std::ast::TypedEnumValue", std.ast.TypedEnumValue);
    compiler.registerBuiltInStruct("std::ast::Var", std.ast.Var);
    compiler.registerBuiltInStruct("std::ast::While", std.ast.While);

    // enums
    compiler.registerBuiltInStruct("std::Boolean", std.Boolean);
    compiler.registerBuiltInEnumValue("std::Boolean::true", std.Boolean.true_);
    compiler.registerBuiltInEnumValue("std::Boolean::false", std.Boolean.false_);

    compiler.registerBuiltInStruct("std::Direction", std.Direction);
    compiler.registerBuiltInEnumValue("std::Direction::up", std.Direction.up);
    compiler.registerBuiltInEnumValue("std::Direction::down", std.Direction.down);
    compiler.registerBuiltInEnumValue("std::Direction::left", std.Direction.left);
    compiler.registerBuiltInEnumValue("std::Direction::right", std.Direction.right);

    compiler.registerBuiltInStruct("std::NumberSign", std.NumberSign);
    compiler.registerBuiltInEnumValue("std::NumberSign::positive", std.NumberSign.positive);
    compiler.registerBuiltInEnumValue("std::NumberSign::negative", std.NumberSign.negative);

    // structs
    compiler.registerBuiltInStruct("std::Cell", std.Cell);
    compiler.registerBuiltInStruct("std::Char", std.Char);
    compiler.registerBuiltInStruct("std::Container", std.Container);
    compiler.registerBuiltInStruct("std::Digit", std.Digit);
    compiler.registerBuiltInStruct("std::Enum", std.Enum);
    compiler.registerBuiltInStruct("std::Index", std.Index);
    compiler.registerBuiltInStruct("std::KVPair", std.KVPair);
    compiler.registerBuiltInStruct("std::Library", std.Library);
    compiler.registerBuiltInStruct("std::List", std.List);
    compiler.registerBuiltInStruct("std::ListItem", std.ListItem);
    compiler.registerBuiltInStruct("std::Map", std.Map);
    compiler.registerBuiltInStruct("std::Number", std.Number);
    compiler.registerBuiltInStruct("std::OpState", std.OpState);
    compiler.registerBuiltInStruct("std::Slot", std.Slot);
    compiler.registerBuiltInStruct("std::Stack", std.Stack);
    compiler.registerBuiltInStruct("std::StackFrame", std.StackFrame);
    compiler.registerBuiltInStruct("std::String", std.String);
    compiler.registerBuiltInStruct("std::Struct", std.Struct);
    compiler.registerBuiltInStruct("std::StructReference", std.StructReference);
    compiler.registerBuiltInStruct("std::TrieMap", std.TrieMap);
    compiler.registerBuiltInStruct("std::TrieMapNode", std.TrieMapNode);
}

} // namespace cells
} // namespace infocell
