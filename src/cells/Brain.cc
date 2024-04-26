#include "Brain.h"

#include <boost/algorithm/string/regex.hpp>
#include <format>
#include <sstream>

namespace synth {
namespace cells {
namespace brain {

ID::ID(brain::Brain& kb) :
    kb(kb),
    argument(kb, kb.type.Char, "argument"),
    ast(kb, kb.type.Char, "ast"),
    asts(kb, kb.type.Char, "asts"),
    cell(kb, kb.type.Char, "cell"),
    children(kb, kb.type.Char, "children"),
    code(kb, kb.type.Char, "code"),
    color(kb, kb.type.Char, "color"),
    compiled(kb, kb.type.Char, "compiled"),
    condition(kb, kb.type.Char, "condition"),
    constructor(kb, kb.type.Char, "constructor"),
    continue_(kb, kb.type.Char, "continue_"),
    currentFn(kb, kb.type.Char, "currentFn"),
    currentStruct(kb, kb.type.Char, "currentStruct"),
    data(kb, kb.type.Char, "data"),
    destructor(kb, kb.type.Char, "destructor"),
    else_(kb, kb.type.Char, "else_"),
    emptyObject(kb, kb.type.Char, "emptyObject"),
    first(kb, kb.type.Char, "first"),
    functions(kb, kb.type.Char, "functions"),
    globalScope(kb, kb.type.Char, "globalScope"),
    height(kb, kb.type.Char, "height"),
    id(kb, kb.type.Char, "id"),
    index(kb, kb.type.Char, "index"),
    input(kb, kb.type.Char, "input"),
    instances(kb, kb.type.Char, "instances"),
    item(kb, kb.type.Char, "item"),
    itemType(kb, kb.type.Char, "itemType"),
    key(kb, kb.type.Char, "key"),
    keyType(kb, kb.type.Char, "keyType"),
    last(kb, kb.type.Char, "last"),
    lhs(kb, kb.type.Char, "lhs"),
    list(kb, kb.type.Char, "list"),
    listType(kb, kb.type.Char, "listType"),
    localVars(kb, kb.type.Char, "localVars"),
    memberOf(kb, kb.type.Char, "memberOf"),
    members(kb, kb.type.Char, "members"),
    method(kb, kb.type.Char, "method"),
    methods(kb, kb.type.Char, "methods"),
    name(kb, kb.type.Char, "name"),
    next(kb, kb.type.Char, "next"),
    objectType(kb, kb.type.Char, "objectType"),
    op(kb, kb.type.Char, "op"),
    ops(kb, kb.type.Char, "ops"),
    output(kb, kb.type.Char, "output"),
    parameters(kb, kb.type.Char, "parameters"),
    parent(kb, kb.type.Char, "parent"),
    pixels(kb, kb.type.Char, "pixels"),
    previous(kb, kb.type.Char, "previous"),
    resolvedScope(kb, kb.type.Char, "resolvedScope"),
    result(kb, kb.type.Char, "result"),
    returnType(kb, kb.type.Char, "returnType"),
    rhs(kb, kb.type.Char, "rhs"),
    role(kb, kb.type.Char, "role"),
    rootNode(kb, kb.type.Char, "rootNode"),
    scope(kb, kb.type.Char, "scope"),
    scopes(kb, kb.type.Char, "scopes"),
    self(kb, kb.type.Char, "self"),
    size(kb, kb.type.Char, "size"),
    slotRole(kb, kb.type.Char, "slotRole"),
    slots(kb, kb.type.Char, "slots"),
    slotType(kb, kb.type.Char, "slotType"),
    stack(kb, kb.type.Char, "stack"),
    statement(kb, kb.type.Char, "statement"),
    static_(kb, kb.type.Char, "static_"),
    status(kb, kb.type.Char, "status"),
    stop(kb, kb.type.Char, "stop"),
    structs(kb, kb.type.Char, "structs"),
    structType(kb, kb.type.Char, "structType"),
    subTypes(kb, kb.type.Char, "subTypes"),
    templateId(kb, kb.type.Char, "templateId"),
    templateParams(kb, kb.type.Char, "templateParams"),
    then(kb, kb.type.Char, "then"),
    type(kb, kb.type.Char, "type"),
    unknownInstances(kb, kb.type.Char, "unknownInstances"),
    unknownStructs(kb, kb.type.Char, "unknownStructs"),
    value(kb, kb.type.Char, "value"),
    valueType(kb, kb.type.Char, "valueType"),
    variables(kb, kb.type.Char, "variables"),
    width(kb, kb.type.Char, "width")
{
}

namespace type {

Op::Op(brain::Brain& kb) :
    Add(kb, kb.type.Type_, "op::Add"),
    And(kb, kb.type.Type_, "op::And"),
    Base(kb, kb.type.Type_, "op::Base"),
    Block(kb, kb.type.Type_, "op::Block"),
    ConstVar(kb, kb.type.Type_, "op::ConstVar"),
    Delete(kb, kb.type.Type_, "op::Delete"),
    Divide(kb, kb.type.Type_, "op::Divide"),
    Do(kb, kb.type.Type_, "op::Do"),
    Equal(kb, kb.type.Type_, "op::Equal"),
    Erase(kb, kb.type.Type_, "op::Erase"),
    EvalVar(kb, kb.type.Type_, "op::EvalVar"),
    Function(kb, kb.type.Type_, "op::Function"),
    Get(kb, kb.type.Type_, "op::Get"),
    GreaterThan(kb, kb.type.Type_, "op::GreaterThan"),
    GreaterThanOrEqual(kb, kb.type.Type_, "op::GreaterThanOrEqual"),
    Has(kb, kb.type.Type_, "op::Has"),
    If(kb, kb.type.Type_, "op::If"),
    LessThan(kb, kb.type.Type_, "op::LessThan"),
    LessThanOrEqual(kb, kb.type.Type_, "op::LessThanOrEqual"),
    Missing(kb, kb.type.Type_, "op::Missing"),
    Multiply(kb, kb.type.Type_, "op::Multiply"),
    New(kb, kb.type.Type_, "op::New"),
    Not(kb, kb.type.Type_, "op::Not"),
    NotEqual(kb, kb.type.Type_, "op::NotEqual"),
    NotSame(kb, kb.type.Type_, "op::NotSame"),
    Or(kb, kb.type.Type_, "op::Or"),
    Return(kb, kb.type.Type_, "op::Return"),
    Same(kb, kb.type.Type_, "op::Same"),
    Set(kb, kb.type.Type_, "op::Set"),
    Subtract(kb, kb.type.Type_, "op::Subtract"),
    Var(kb, kb.type.Type_, "op::Var"),
    While(kb, kb.type.Type_, "op::While")
{
    auto& type = kb.type;
    auto& ast  = kb.type.ast;

    CellI* map = nullptr;

    map = &kb.slots(type.slot("ast", ast.Base),
                    type.slot("lhs", Base),
                    type.slot("rhs", Base),
                    type.slot("value", type.Number));
    Add.set("slots", *map);

    map = &kb.slots(type.slot("ast", ast.Base),
                    type.slot("lhs", Base),
                    type.slot("rhs", Base),
                    type.slot("value", type.Boolean));
    And.set("slots", *map);

    map = &kb.slots(type.slot("ast", ast.Base),
                    type.slot("status", type.Cell),
                    type.slot("ops", type.Cell),
                    type.slot("value", type.Cell));
    Block.set("slots", *map);

    map = &kb.slots(type.slot("ast", ast.Base),
                    type.slot("value", type.Cell));
    ConstVar.set("slots", *map);

    map = &kb.slots(type.slot("ast", ast.Base),
                    type.slot("input", Base));
    Delete.set("slots", *map);

    map = &kb.slots(type.slot("ast", ast.Base),
                    type.slot("lhs", Base),
                    type.slot("rhs", Base),
                    type.slot("value", type.Number));
    Divide.set("slots", *map);

    map = &kb.slots(type.slot("ast", ast.Base),
                    type.slot("status", type.Cell),
                    type.slot("condition", Base),
                    type.slot("statement", Base));
    Do.set("slots", *map);

    map = &kb.slots(type.slot("ast", ast.Base),
                    type.slot("lhs", Base),
                    type.slot("rhs", Base),
                    type.slot("value", type.Boolean));
    Equal.set("slots", *map);

    map = &kb.slots(type.slot("ast", ast.Base),
                    type.slot("cell", Base),
                    type.slot("role", Base),
                    type.slot("value", Base));
    Erase.set("slots", *map);

    map = &kb.slots(type.slot("ast", ast.Base),
                    type.slot("value", Var));
    EvalVar.set("slots", *map);

    map = &kb.slots(type.slot("ast", ast.Base),
                    type.slot("stack", type.Stack),
                    type.slot("op", type.ListOf(Base)),
                    type.slot("static_", type.Boolean));
    Function.set("slots", *map);

    map = &kb.slots(type.slot("ast", ast.Base),
                    type.slot("cell", Base),
                    type.slot("role", Base),
                    type.slot("value", type.Cell));
    Get.set("slots", *map);

    map = &kb.slots(type.slot("ast", ast.Base),
                    type.slot("lhs", Base),
                    type.slot("rhs", Base),
                    type.slot("value", type.Boolean));
    GreaterThan.set("slots", *map);

    map = &kb.slots(type.slot("ast", ast.Base),
                    type.slot("lhs", Base),
                    type.slot("rhs", Base),
                    type.slot("value", type.Boolean));
    GreaterThanOrEqual.set("slots", *map);

    map = &kb.slots(type.slot("ast", ast.Base),
                    type.slot("cell", Base),
                    type.slot("role", Base),
                    type.slot("value", type.Boolean));
    Has.set("slots", *map);

    map = &kb.slots(type.slot("ast", ast.Base),
                    type.slot("status", type.Cell),
                    type.slot("condition", Base),
                    type.slot("then", Base),
                    type.slot("else_", Base));
    If.set("slots", *map);

    map = &kb.slots(type.slot("ast", ast.Base),
                    type.slot("lhs", Base),
                    type.slot("rhs", Base),
                    type.slot("value", type.Boolean));
    LessThan.set("slots", *map);

    map = &kb.slots(type.slot("ast", ast.Base),
                    type.slot("lhs", Base),
                    type.slot("rhs", Base),
                    type.slot("value", type.Boolean));
    LessThanOrEqual.set("slots", *map);

    map = &kb.slots(type.slot("ast", ast.Base),
                    type.slot("cell", Base),
                    type.slot("role", Base),
                    type.slot("value", type.Boolean));
    Missing.set("slots", *map);

    map = &kb.slots(type.slot("ast", ast.Base),
                    type.slot("lhs", Base),
                    type.slot("rhs", Base),
                    type.slot("value", type.Number));
    Multiply.set("slots", *map);

    map = &kb.slots(type.slot("ast", ast.Base),
                    type.slot("value", type.Cell),
                    type.slot("objectType", Base));
    New.set("slots", *map);

    map = &kb.slots(type.slot("ast", ast.Base),
                    type.slot("input", Base),
                    type.slot("value", type.Boolean));
    Not.set("slots", *map);

    map = &kb.slots(type.slot("ast", ast.Base),
                    type.slot("lhs", Base),
                    type.slot("rhs", Base),
                    type.slot("value", type.Boolean));
    NotEqual.set("slots", *map);

    map = &kb.slots(type.slot("ast", ast.Base),
                    type.slot("lhs", Base),
                    type.slot("rhs", Base),
                    type.slot("value", type.Boolean));
    NotSame.set("slots", *map);

    map = &kb.slots(type.slot("ast", ast.Base),
                    type.slot("lhs", Base),
                    type.slot("rhs", Base),
                    type.slot("value", type.Boolean));
    Or.set("slots", *map);

    map = &kb.slots(type.slot("ast", ast.Base),
                    type.slot("result", ast.Base));
    Return.set("slots", *map);

    map = &kb.slots(type.slot("ast", ast.Base),
                    type.slot("lhs", Base),
                    type.slot("rhs", Base),
                    type.slot("value", type.Boolean));
    Same.set("slots", *map);

    map = &kb.slots(type.slot("ast", ast.Base),
                    type.slot("cell", Base),
                    type.slot("role", Base),
                    type.slot("value", Base));
    Set.set("slots", *map);

    map = &kb.slots(type.slot("ast", ast.Base),
                    type.slot("lhs", Base),
                    type.slot("rhs", Base),
                    type.slot("value", type.Number));
    Subtract.set("slots", *map);

    map = &kb.slots(type.slot("ast", ast.Base),
                    type.slot("valueType", type.Type_),
                    type.slot("value", type.Cell));
    Var.set("slots", *map);

    map = &kb.slots(type.slot("ast", ast.Base),
                    type.slot("status", type.Cell),
                    type.slot("condition", Base),
                    type.slot("statement", Base));
    While.set("slots", *map);
}

Ast::Ast(brain::Brain& kb) :
    kb(kb),
    Add(kb, kb.type.Type_, "ast::Add"),
    And(kb, kb.type.Type_, "ast::And"),
    Base(kb, kb.type.Type_, "ast::Base"),
    Block(kb, kb.type.Type_, "ast::Block"),
    Call(kb, kb.type.Type_, "ast::Call"),
    Cell(kb, kb.type.Type_, "ast::Cell"),
    Delete(kb, kb.type.Type_, "ast::Delete"),
    Divide(kb, kb.type.Type_, "ast::Divide"),
    Do(kb, kb.type.Type_, "ast::Do"),
    Equal(kb, kb.type.Type_, "ast::Equal"),
    Erase(kb, kb.type.Type_, "ast::Erase"),
    Function(kb, kb.type.Type_, "ast::Function"),
    FunctionT(kb, kb.type.Type_, "ast::FunctionT"),
    Get(kb, kb.type.Type_, "ast::Get"),
    GreaterThan(kb, kb.type.Type_, "ast::GreaterThan"),
    GreaterThanOrEqual(kb, kb.type.Type_, "ast::GreaterThanOrEqual"),
    Has(kb, kb.type.Type_, "ast::Has"),
    If(kb, kb.type.Type_, "ast::If"),
    LessThan(kb, kb.type.Type_, "ast::LessThan"),
    LessThanOrEqual(kb, kb.type.Type_, "ast::LessThanOrEqual"),
    Member(kb, kb.type.Type_, "ast::Member"),
    Missing(kb, kb.type.Type_, "ast::Missing"),
    Multiply(kb, kb.type.Type_, "ast::Multiply"),
    New(kb, kb.type.Type_, "ast::New"),
    Not(kb, kb.type.Type_, "ast::Not"),
    NotEqual(kb, kb.type.Type_, "ast::NotEqual"),
    NotSame(kb, kb.type.Type_, "ast::NotSame"),
    Or(kb, kb.type.Type_, "ast::Or"),
    Parameter(kb, kb.type.Type_, "ast::Parameter"),
    ResolvedType(kb, kb.type.Type_, "ast::ResolvedType"),
    Return(kb, kb.type.Type_, "ast::Return"),
    Same(kb, kb.type.Type_, "ast::Same"),
    Scope(kb, kb.type.Type_, "ast::Scope"),
    Self(kb, kb.type.Type_, "ast::Self"),
    SelfFn(kb, kb.type.Type_, "ast::SelfFn"),
    Set(kb, kb.type.Type_, "ast::Set"),
    Slot(kb, kb.type.Type_, "ast::Slot"),
    StaticCall(kb, kb.type.Type_, "ast::StaticCall"),
    Struct(kb, kb.type.Type_, "ast::Struct"),
    StructName(kb, kb.type.Type_, "ast::StructName"),
    StructT(kb, kb.type.Type_, "ast::StructT"),
    SubTypeName(kb, kb.type.Type_, "ast::SubTypeName"),
    Subtract(kb, kb.type.Type_, "ast::Subtract"),
    TemplatedType(kb, kb.type.Type_, "ast::TemplatedType"),
    TemplateParam(kb, kb.type.Type_, "ast::TemplateParam"),
    Var(kb, kb.type.Type_, "ast::Var"),
    While(kb, kb.type.Type_, "ast::While")
{
    auto& type = kb.type;
    CellI* map = nullptr;

    map = &kb.slots(type.slot("lhs", Base),
                    type.slot("rhs", Base));
    Add.set("slots", *map);

    map = &kb.slots(type.slot("lhs", Base),
                    type.slot("rhs", Base));
    And.set("slots", *map);

    map = &kb.slots(type.slot("asts", type.Cell));
    Block.set("slots", *map);

    map = &kb.slots(type.slot("cell", Base),
                    type.slot("method", Base),
                    type.slot("parameters", type.ListOf(Slot)));
    Call.set("slots", *map);

    map = &kb.slots(type.slot("value", type.Cell));
    Cell.set("slots", *map);

    map = &kb.slots(type.slot("cell", Base));
    Delete.set("slots", *map);

    map = &kb.slots(type.slot("lhs", Base),
                    type.slot("rhs", Base));
    Divide.set("slots", *map);

    map = &kb.slots(type.slot("condition", Base),
                    type.slot("statement", Base));
    Do.set("slots", *map);

    map = &kb.slots(type.slot("lhs", Base),
                    type.slot("rhs", Base));
    Equal.set("slots", *map);

    map = &kb.slots(type.slot("cell", Base),
                    type.slot("role", Base));
    Erase.set("slots", *map);

    map = &kb.slots(type.slot("name", type.Cell),
                    type.slot("fullId", type.Cell),
                    type.slot("structType", type.Cell),
                    type.slot("parameters", type.ListOf(Slot)),
                    type.slot("returnType", type.Type_),
                    type.slot("code", Base),
                    type.slot("scope", Base),
                    type.slot("static_", type.Boolean));
    Function.set("slots", *map);

    map = &kb.slots(type.slot("name", type.Cell),
                    type.slot("parameters", type.ListOf(Slot)),
                    type.slot("returnType", type.Type_),
                    type.slot("code", Base),
                    type.slot("scope", Base),
                    type.slot("static_", type.Boolean));
    FunctionT.set("slots", *map);

    map = &kb.slots(type.slot("cell", Base),
                    type.slot("role", Base));
    Get.set("slots", *map);

    map = &kb.slots(type.slot("lhs", Base),
                    type.slot("rhs", Base));
    GreaterThan.set("slots", *map);

    map = &kb.slots(type.slot("lhs", Base),
                    type.slot("rhs", Base));
    GreaterThanOrEqual.set("slots", *map);

    map = &kb.slots(type.slot("cell", Base),
                    type.slot("role", Base));
    Has.set("slots", *map);

    map = &kb.slots(type.slot("condition", Base),
                    type.slot("then", Base),
                    type.slot("else_", Base));
    If.set("slots", *map);

    map = &kb.slots(type.slot("lhs", Base),
                    type.slot("rhs", Base));
    LessThan.set("slots", *map);

    map = &kb.slots(type.slot("lhs", Base),
                    type.slot("rhs", Base));
    LessThanOrEqual.set("slots", *map);

    map = &kb.slots(type.slot("role", Base));
    Member.set("slots", *map);

    map = &kb.slots(type.slot("cell", Base),
                    type.slot("role", Base));
    Missing.set("slots", *map);

    map = &kb.slots(type.slot("lhs", Base),
                    type.slot("rhs", Base));
    Multiply.set("slots", *map);

    map = &kb.slots(type.slot("objectType", Base),
                    type.slot("constructor", Base),
                    type.slot("parameters", type.ListOf(type.ast.Slot)));
    New.set("slots", *map);

    map = &kb.slots(type.slot("input", Base));
    Not.set("slots", *map);

    map = &kb.slots(type.slot("lhs", Base),
                    type.slot("rhs", Base));
    NotEqual.set("slots", *map);

    map = &kb.slots(type.slot("lhs", Base),
                    type.slot("rhs", Base));
    NotSame.set("slots", *map);

    map = &kb.slots(type.slot("lhs", Base),
                    type.slot("rhs", Base));
    Or.set("slots", *map);

    map = &kb.slots(type.slot("role", type.Cell));
    Parameter.set("slots", *map);

    map = &kb.slots(type.slot("ast", Struct),
                    type.slot("compiled", type.Type_));
    ResolvedType.set("slots", *map);

    map = &kb.slots(type.slot("value", type.Cell));
    Return.set("slots", *map);

    map = &kb.slots(type.slot("lhs", Base),
                    type.slot("rhs", Base));
    Same.set("slots", *map);

    map = &kb.slots(type.slot("id", type.Cell),
                    type.slot("fullId", type.Cell),
                    type.slot("scopes", type.TrieMap),
                    type.slot("resolvedScope", type.ast.Scope),
                    type.slot("parent", Scope),
                    type.slot("functions", type.TrieMap),
                    type.slot("structs", type.TrieMap),
                    type.slot("structTs", type.TrieMap),
                    type.slot("variables", type.ListOf(type.ast.Slot)));
    Scope.set("slots", *map);

    map = &kb.slots(type.slot("cell", Base),
                    type.slot("role", Base),
                    type.slot("value", Base));
    Set.set("slots", *map);

    map = &kb.slots(type.slot("slotRole", Base),
                    type.slot("slotType", Base));
    Slot.set("slots", *map);

    map = &kb.slots(type.slot("cell", Base),
                    type.slot("method", Base),
                    type.slot("parameters", type.ListOf(Slot)));
    StaticCall.set("slots", *map);

    map = &kb.slots(type.slot("id", type.Cell),
                    type.slot("fullId", type.Cell),
                    type.slot("incomplete", type.Boolean),
                    type.slot("instanceOf", Base),
                    type.slot("templateParams", type.List),
                    type.slot("scope", Scope),
                    type.slot("methods", type.MapOf(type.Cell, type.ast.Function)),
                    type.slot("members", type.MapOf(type.Cell, type.ast.Slot)),
                    type.slot("subTypes", type.ListOf(type.ast.Slot)),
                    type.slot("memberOf", type.ListOf(type.Type_)));
    Struct.set("slots", *map);

    map = &kb.slots(type.slot("name", type.Cell),
                    type.slot("scopes", type.List));
    StructName.set("slots", *map);

    map = &kb.slots(type.slot("id", type.Cell),
                    type.slot("scope", Base),
                    type.slot("methods", type.MapOf(type.Cell, type.ast.Function)),
                    type.slot("members", type.ListOf(type.ast.Slot)),
                    type.slot("subTypes", type.ListOf(type.ast.Slot)),
                    type.slot("memberOf", type.ListOf(type.Type_)),
                    type.slot("templateParams", type.MapOf(type.Cell, type.Type_)));
    StructT.set("slots", *map);

    map = &kb.slots(type.slot("name", type.Cell));
    SubTypeName.set("slots", *map);

    map = &kb.slots(type.slot("lhs", Base),
                    type.slot("rhs", Base));
    Subtract.set("slots", *map);

    map = &kb.slots(type.slot("id", Base),
                    type.slot("scopes", type.List),
                    type.slot("parameters", type.ListOf(Slot)));
    TemplatedType.set("slots", *map);

    map = &kb.slots(type.slot("role", type.Cell));
    TemplateParam.set("slots", *map);

    map = &kb.slots(type.slot("role", Base),
                    type.slot("scope", Scope));
    Var.set("slots", *map);

    map = &kb.slots(type.slot("condition", Base),
                    type.slot("statement", Base));
    While.set("slots", *map);
}

Arc::Arc(brain::Brain& kb) :
    kb(kb),
    Demonstration(kb, kb.type.Type_, "Demonstration"),
    Task(kb, kb.type.Type_, "Task"),
    Shape(kb, kb.type.Type_, "Shape"),
    Shaper(kb, kb.type.Type_, "Shaper")
{
}

} // namespace type

Types::Types(brain::Brain& kb) :
    kb(kb),
    Type_(kb, kb.type.Type_, "Type"),
    Template(kb, kb.type.Type_, "Template"),
    Struct(kb, kb.type.Type_, "Struct"),
    Enum(kb, kb.type.Type_, "Enum"),
    Cell(kb, kb.type.Type_, "Cell"),
    Slot(kb, kb.type.Type_, "Slot"),
    Container(kb, kb.type.Type_, "Conatainer"),
    Iterator(kb, kb.type.Type_, "Iterator"),
    List(kb, kb.type.Type_, "List"),
    ListOfSlot(kb, kb.type.Type_, "List<Slot>"),
    ListItem(kb, kb.type.Type_, "ListItem"),
    KVPair(kb, kb.type.Type_, "KVPair"),
    Map(kb, kb.type.Type_, "Map"),
    MapCellToSlot(kb, kb.type.Type_, "Map<Cell, Slot>"),
    MapCellToType(kb, kb.type.Type_, "Map<Cell, Type>"),
    MapCellToAstFunction(kb, kb.type.Type_, "Map<Cell, ast::Function>"),
    MapCellToOpFunction(kb, kb.type.Type_, "Map<Cell, op::Function>"),
    MapCellToOpVar(kb, kb.type.Type_, "Map<Cell, op::Var>"),
    MapCellToOpBase(kb, kb.type.Type_, "Map<Cell, op::Base>"),
    MapTypeToType(kb, kb.type.Type_, "Map<Type, Type>"),
    Index(kb, kb.type.Type_, "Index"),
    TrieMap(kb, kb.type.Type_, "TrieMap"),
    TrieMapNode(kb, kb.type.Type_, "TrieMapNode"),
    Set(kb, kb.type.Type_, "Set"),
    Boolean(kb, kb.type.Type_, "Boolean"),
    Char(kb, kb.type.Type_, "Char"),
    Digit(kb, kb.type.Type_, "Digit"),
    Number(kb, kb.type.Type_, "Number"),
    String(kb, kb.type.Type_, "String"),
    Color(kb, kb.type.Type_, "Color"),
    Pixel(kb, kb.type.Type_, "Pixel"),
    Picture(kb, kb.type.Type_, "Picture"),
    Stack(kb, kb.type.Type_, "Stack"),
    StackFrame(kb, kb.type.Type_, "StackFrame"),
    Program(kb, kb.type.Type_, "Program"),
    StructReference(kb, kb.type.Type_, "StructReference"),
    CompileState(kb, kb.type.Type_, "CompileState"),
    ScopeData(kb, kb.type.Type_, "ScopeData"),
    Directions(kb, kb.type.Enum, "Directions"),
    Shape(kb, kb.type.Struct, "Shape"),
    op(kb),
    ast(kb),
    arc(kb)
{
    CellI* mapPtr = nullptr;
    auto& type = kb.type;

    mapPtr = &kb.slots(type.slot("slotType", type.Type_),
                       type.slot("slotRole", type.Cell));
    Slot.set("slots", *mapPtr);

    mapPtr = &kb.slots(type.slot("name", type.ListOf(type.Char)),
                       type.slot("slots", MapCellToSlot),
                       type.slot("incomplete", type.Boolean),
                       type.slot("sharedObject", Slot),
                       type.slot("subTypes", MapCellToType),
                       type.slot("memberOf", MapTypeToType),
                       type.slot("asts", MapCellToAstFunction),
                       type.slot("methods", MapCellToOpFunction));
    Type_.set("slots", *mapPtr);

    mapPtr = &kb.slots(type.slot("members", List));
    Enum.set("slots", *mapPtr);

    mapPtr = &kb.slots(type.slot("previous", type.ListItem),
                       type.slot("next", type.ListItem),
                       type.slot("value", type.Cell));
    type.ListItem.set("slots", *mapPtr);

    mapPtr = &kb.slots(type.slot("key", type.Cell),
                       type.slot("value", type.Cell));
    type.KVPair.set("slots", *mapPtr);

    mapPtr = &kb.slots(type.slot("list", type.ListOf(type.Cell)),
                       type.slot("rootNode", type.TrieMapNode),
                       type.slot("size", type.Number));
    type.TrieMap.set("slots", *mapPtr);

    mapPtr = &kb.slots(type.slot("children", type.Index),
                       type.slot("data", type.ListItem),
                       type.slot("parent", type.TrieMapNode));
    type.TrieMapNode.set("slots", *mapPtr);

    mapPtr = &kb.slots(type.slot("data", type.ScopeData),
                       type.slot("code", type.op.Base),
                       type.slot("stack", type.kb.type.ListItem));
    type.Program.set("slots", *mapPtr);

    mapPtr = &kb.slots(type.slot("id", type.Cell),
                       type.slot("idScope", type.ast.Scope),
                       type.slot("scope", type.ast.Scope),
                       type.slot("resolvedScope", type.ast.Scope),
                       type.slot("currentFn", type.ast.Function),
                       type.slot("currentStruct", type.ast.Struct),
                       type.slot("templateId", type.List),
                       type.slot("templateParams", type.List),
                       type.slot("value", type.ast.Struct));
    type.StructReference.set("slots", *mapPtr);

    mapPtr = &kb.slots(type.slot("id", type.Cell),
                       type.slot("currentFn", type.ast.Function),
                       type.slot("currentStruct", type.ast.Struct),
                       type.slot("scope", type.ast.Scope),
                       type.slot("resolvedScope", type.ast.Scope),
                       type.slot("globalScope", type.ast.Scope),
                       type.slot("globalResolvedScope", type.ast.Scope),
                       type.slot("functions", type.TrieMap),
                       type.slot("structs", type.TrieMap),
                       type.slot("unknownStructs", type.TrieMap),
                       type.slot("instances", type.TrieMap),
                       type.slot("unknownInstances", type.TrieMap),
                       type.slot("instanceAsts", type.TrieMap),
                       type.slot("unknownInstanceAsts", type.TrieMap),
                       type.slot("variables", type.ListOf(type.ast.Slot)));
    type.CompileState.set("slots", *mapPtr);

    mapPtr = &kb.slots(type.slot("id", type.Cell),
                       type.slot("scopes", type.Map),
                       type.slot("parent", ScopeData),
                       type.slot("functions", type.TrieMap),
                       type.slot("structs", type.TrieMap),
                       type.slot("unknownStructs", type.TrieMap),
                       type.slot("variables", type.ListOf(type.ast.Slot)));
    type.ScopeData.set("slots", *mapPtr);

    mapPtr = &kb.slots(type.slot("method", type.op.Function),
                       type.slot("input", type.Index),
                       type.slot("output", type.op.Var),
                       type.slot("localVars", type.Index));
    type.StackFrame.set("slots", *mapPtr);

    mapPtr = &kb.slots(type.slot(kb.colors.red, type.Color),
                       type.slot(kb.colors.green, type.Color),
                       type.slot(kb.colors.blue, type.Color));
    type.Color.set("slots", *mapPtr);

    kb.m_initPhase = Brain::InitPhase::SlotTypeInitialzed;
}

cells::CellI& Types::slot(cells::CellI& role, cells::CellI& type)
{
    CellI& ret = *new Object(kb, kb.type.Slot);
    ret.set("slotRole", role);
    ret.set("slotType", type);

    return ret;
}

cells::CellI& Types::slot(const std::string& role, cells::CellI& type)
{
    CellI& ret = *new Object(kb, kb.type.Slot);
    ret.set("slotRole", kb.id(role));
    ret.set("slotType", type);

    return ret;
}

cells::CellI& Types::kvPair(cells::CellI& key, cells::CellI& value)
{
    CellI& ret = *new Object(kb, kb.type.KVPair);
    ret.set("key", key);
    ret.set("value", value);

    return ret;
}

Object& Types::ListOf(CellI& type)
{
    auto typeIt = m_listTypes.find(&type);
    if (typeIt != m_listTypes.end()) {
        return typeIt->second;
    } else {
        auto it          = m_listTypes.emplace(std::piecewise_construct,
                                               std::forward_as_tuple(&type),
                                               std::forward_as_tuple(kb, kb.type.Type_, std::format("List<{}>", type.label())));
        Object& listType = it.first->second;
        Object& itemType = *new Object(kb, kb.type.Type_, std::format("ListItem<{}>", type.label()));
        listType.set("subTypes", kb.map(kb.type.Cell, kb.type.Type_, kb.ids.valueType, type, kb.ids.itemType, itemType));
        listType.set("memberOf", kb.map(kb.type.Type_, kb.type.Type_, kb.type.List, kb.type.List));
        itemType.set("memberOf", kb.map(kb.type.Type_, kb.type.Type_, kb.type.ListItem, kb.type.ListItem));
        itemType.set("subTypes", kb.map(kb.type.Cell, kb.type.Type_, kb.ids.valueType, type, kb.ids.itemType, itemType));

        CellI* map = nullptr;

        map = &kb.slots(kb.type.slot("first", itemType),
                        kb.type.slot("last", itemType),
                        kb.type.slot("size", kb.type.Number));
        listType.set("slots", *map);

        map = &kb.slots(kb.type.slot("previous", itemType),
                        kb.type.slot("next", itemType),
                        kb.type.slot("value", type));
        itemType.set("slots", *map);

        return it.first->second;
    }
}

Object& Types::MapOf(CellI& keyType, CellI& valueType)
{
    auto keyTypeIt = m_mapTypes.find(&keyType);
    if (keyTypeIt != m_mapTypes.end()) {
        auto& valueTypeMap = keyTypeIt->second;
        auto valueTypeIt  = valueTypeMap.find(&valueType);
        if (valueTypeIt != valueTypeMap.end()) {
            return valueTypeIt->second;
        }
    } else {
        keyTypeIt = m_mapTypes.emplace(&keyType, std::map<CellI*, Object>()).first;
    }
    auto& valueTypeMap = keyTypeIt->second;
    auto it            = valueTypeMap.emplace(std::piecewise_construct,
                                              std::forward_as_tuple(&keyType),
                                              std::forward_as_tuple(kb, kb.type.Type_, std::format("Map<{}, {}>", keyType.label(), valueType.label())));

    Object& mapType = it.first->second;
    CellI* map      = nullptr;

    if (kb.type.Map.has("methods")) {
        mapType.set("methods", kb.type.Map[kb.ids.methods]); // TODO
    }
    mapType.set("memberOf", kb.map(kb.type.Type_, kb.type.Type_, kb.type.Map, kb.type.Map));
    mapType.set("subTypes", kb.map(kb.type.Cell, kb.type.Type_, kb.ids.keyType, keyType, kb.ids.valueType, valueType));

    map = &kb.slots(kb.type.slot("list", ListOf(valueType)),
                    kb.type.slot("index", kb.type.Index),
                    kb.type.slot("size", kb.type.Number));
    mapType.set("slots", *map);

    return mapType;
}

Object& Types::SetOf(CellI& valueType)
{
    auto valueTypeIt = m_setTypes.find(&valueType);
    if (valueTypeIt != m_setTypes.end()) {
        return valueTypeIt->second;
    } else {
        valueTypeIt = m_setTypes.emplace(std::piecewise_construct,
                                         std::forward_as_tuple(&valueType),
                                         std::forward_as_tuple(kb, kb.type.Type_, std::format("Set<{}>", valueType.label()))).first;
    }
    Object& setType = valueTypeIt->second;
    CellI* map      = nullptr;

    setType.set("methods", kb.type.Set[kb.ids.methods]);
    setType.set("memberOf", kb.map(kb.type.Type_, kb.type.Type_, kb.type.Set, kb.type.Set));
    setType.set("subTypes", kb.map(kb.type.Cell, kb.type.Type_, kb.ids.valueType, valueType));

    map = &kb.slots(kb.type.slot("list", ListOf(valueType)),
                    kb.type.slot("index", kb.type.Index),
                    kb.type.slot("size", kb.type.Number));
    setType.set("slots", *map);

    return setType;
}

// ============================================================================
Ast::Base::Base(brain::Brain& kb, CellI& classCell, const std::string& label) :
    Object(kb, classCell, label)
{
}

Ast::Base& Ast::Base::resolveType(CellI& typeAst, CellI& resolveState)
{
    if (&typeAst.type() == &kb.type.ast.Cell) {
        auto& cell      = static_cast<Ast::Cell&>(typeAst);
        auto& cellValue = typeAst[kb.ids.value];
        if (!cellValue.isA(kb.type.Type_) && !cellValue.isA(kb.type.Enum)) {
            throw "Type AST referencing a non-type!";
        }
        return cell;
    }
    if (&typeAst.type() == &kb.type.ast.StructName) {
        auto& resolveAstStruct      = resolveStructNameAsAst(typeAst, resolveState);
        auto& resolveCompiledStruct = resolveStructName(resolveAstStruct.getFullId(), resolveState);
        auto& reslvedTypeNode       = resolvedType(resolveAstStruct, resolveCompiledStruct);

        return reslvedTypeNode;
    }
    if (&typeAst.type() == &kb.type.ast.SubTypeName) {
        auto& resolveAstStruct = resolveSubTypeNameAsAst(typeAst, resolveState);

        return resolveAstStruct;
    }
    if (&typeAst.type() == &kb.type.ast.TemplatedType) {
        return resolveTemplatedType(typeAst, resolveState);
    }

    throw "Unknown ast type node!";
}

CellI& Ast::Base::getCompiledTypeFromResolvedType(CellI& resolvedTypeAst)
{
    if (resolvedTypeAst.isA(kb.type.ast.Cell)) {
        return  resolvedTypeAst[kb.ids.value];
    } else if (resolvedTypeAst.isA(kb.type.ast.ResolvedType)) {
        return resolvedTypeAst[kb.ids.compiled];
    } else {
        throw "Unexpected AST type!";
    }
}

CellI& Ast::Base::getResolvedTypeById(CellI& id, bool isInstance, CellI& resolveState)
{
    if (isInstance) {
        auto& instances = static_cast<TrieMap&>(resolveState[kb.ids.instances]);
        if (instances.hasKey(id)) {
            return instances.getValue(id);
        } else {
            auto& unknownInstances = static_cast<TrieMap&>(resolveState[kb.ids.unknownInstances]);
            return unknownInstances.getValue(id)[kb.ids.value];
        }
    } else {
        auto& structs = static_cast<TrieMap&>(resolveState[kb.ids.structs]);
        return structs.getValue(id);
    }
}

CellI& Ast::Base::resolveId(CellI& structId, CellI& containerId, CellI& unknownContainerId, CellI& resolveState, std::function<CellI&(CellI& structReference)> unknownCb)
{
    auto& container = static_cast<TrieMap&>(resolveState[containerId]);

    if (container.hasKey(structId)) {
        return container.getValue(structId);
    } else {
        auto& unresolvedContainer = static_cast<TrieMap&>(resolveState[unknownContainerId]);
        if (unresolvedContainer.hasKey(structId)) {
            return unresolvedContainer.getValue(structId)[kb.ids.value];
        } else {

            auto& structReference = *new Object(kb, kb.type.StructReference);
            auto& unresolvedStruct = unknownCb(structReference);
            structReference.set("value", unresolvedStruct);
            structReference.set("id", structId);
            structReference.set("scope", resolveState[kb.ids.scope]);
            structReference.set("resolvedScope", resolveState[kb.ids.resolvedScope]);

            if (resolveState.has("currentFn")) {
                structReference.set("currentFn", resolveState[kb.ids.currentFn]);
            }
            if (resolveState.has("currentStruct")) {
                structReference.set("currentStruct", resolveState[kb.ids.currentStruct]);
            }
            unresolvedContainer.add(structId, structReference);

            return unresolvedStruct;
        }
    }
}

CellI& Ast::Base::resolveTemplateInstanceId(CellI& structId, CellI& idScope, CellI& resolveState, CellI& ast, CellI& templateParams)
{
    auto& templateId = ast[kb.ids.id];
    return resolveId(structId, kb.id("instances"), kb.id("unknownInstances"), resolveState, [this, &resolveState, &templateId, &structId ,&templateParams, &ast, &idScope](CellI& structReference) -> CellI& {
        structReference.set("templateId", templateId);
        structReference.set("templateParams", templateParams);
        if (ast.has("scopes")) {
            structReference.set(kb.id("idScope"), idScope);
        }
        auto& unresolvedStruct = *new Object(kb, kb.type.Type_, std::format("{}", structId.label()));
        unresolvedStruct.set("incomplete", kb.boolean.true_);

        return unresolvedStruct;
    });
}


Ast::Struct& Ast::Base::resolveTemplateInstanceIdAsAst(CellI& structId, CellI& idScope, CellI& resolveState, CellI& ast, CellI& templateParams)
{
    auto& templateId = ast[kb.ids.id];
    auto& ret = resolveId(structId, kb.id("instanceAsts"), kb.id("unknownInstanceAsts"), resolveState, [this, &resolveState, &templateId, &structId, &templateParams, &ast, &idScope](CellI& structReference) -> CellI& {
        structReference.set("templateId", templateId);
        structReference.set("templateParams", templateParams);
        if (ast.has("scopes")) {
            structReference.set(kb.id("idScope"), idScope);
        }
        auto& unresolvedStruct = *new Ast::Struct(kb, structId, std::format("{}", structId.label()));

        return unresolvedStruct;
    });
    if (ret.missing("scope")) {
        ret.set("scope", idScope);
    }

    return static_cast<Ast::Struct&>(ret);
}

CellI& Ast::Base::resolveStructName(CellI& structId, CellI& resolveState)
{
    return resolveId(structId, kb.ids.structs, kb.ids.unknownStructs, resolveState, [this, &structId](CellI& structReference) -> CellI& {
        auto& unresolvedStruct = *new Object(kb, kb.type.Type_, std::format("{}", structId.label()));
        unresolvedStruct.set("incomplete", kb.boolean.true_);

        return unresolvedStruct;
    });
}

Ast::Struct& Ast::Base::resolveStructNameAsAst(CellI& structName, CellI& resolveState)
{
    Scope& currentScope = static_cast<Scope&>(resolveState[kb.ids.scope]);
    return currentScope.resolveStructName(structName);

}

Ast::Base& Ast::Base::resolveSubTypeNameAsAst(CellI& subTypeName, CellI& resolveState)
{
    Struct& currentStruct = static_cast<Struct&>(resolveState[kb.ids.currentStruct]);
    auto& subTypeAst      = currentStruct.getSubType(subTypeName[kb.ids.name]);
    return subTypeAst;
}

Ast::Base& Ast::Base::resolveTemplatedType(CellI& ast, CellI& resolveState)
{
    auto& templateId     = ast[kb.ids.id];
    auto& templateParams = ast[kb.ids.parameters];

    Scope* scopePtr = &static_cast<Scope&>(resolveState[kb.ids.scope]);
    if (ast.has("scopes")) {
        auto& scopeList = ast[kb.ids.scopes];
        scopePtr        = scopePtr->resolveFullTemplateId(scopeList, templateId);
        if (!scopePtr) {
            throw "Unknow namespace!";
        }
    }
    auto& scope = *scopePtr;
    if (!scope.hasStructT(templateId)) {
        std::cerr << std::format("Unknown template {}", templateId.label()) << std::endl;
        std::cerr << std::format("Current scope: {}", scope.label()) << std::endl;
        if (resolveState.has("currentStruct")) {
            std::cerr << std::format("Current struct: {}", resolveState[kb.ids.currentStruct].label()) << std::endl;
        }
        if (resolveState.has("currentFn")) {
            std::cerr << std::format("Current function: {}", resolveState[kb.ids.currentFn].label()) << std::endl;
        }
        throw std::format("Unknown template {}", templateId.label());
    }

    List& resolvedTemplateParams   = *new List(kb, kb.type.Cell, "resolvedTemplateParams");
    List& idCell                   = generateTemplateId(templateId, templateParams, resolveState, resolvedTemplateParams);
    auto& resolvedAstInstance      = resolveTemplateInstanceIdAsAst(idCell, scope, resolveState, ast, resolvedTemplateParams);
    resolvedAstInstance.set("instanceOf", scope.getStructT(templateId));
    resolvedAstInstance.set("templateParams", resolvedTemplateParams);
    auto& resolvedCompiledInstance = resolveTemplateInstanceId(resolvedAstInstance.getFullId(), scope, resolveState, ast, resolvedTemplateParams);

    // std::cout << std::format("DDDD {} resolved at {:p}\n", idCell.label(), (void*)&resolvedInstance) << std::endl;

    return resolvedType(resolvedAstInstance, resolvedCompiledInstance);
}

List& Ast::Base::generateTemplateId(CellI& id, CellI& parameters, CellI& resolveState, List& resolvedParams)
{
    List& idCell = *new List(kb, kb.type.Cell);
    std::stringstream ss;
    Visitor::visitList(id, [this, &idCell, &ss](CellI& character, int, bool&) {
        idCell.add(character);
        ss << character.label();
    });

    ss << "<";
    Visitor::visitList(parameters, [this, &idCell, &ss, &resolveState, &resolvedParams](CellI& slot, int i, bool&) {
        if (i != 0) {
            ss << ", ";
        }
        CellI& slotRole         = slot[kb.ids.slotRole];
        CellI& slotType         = slot[kb.ids.slotType];
        CellI& resolvedSlotType = resolveType(slotType, resolveState);
        CellI& compiledSlotType = getCompiledTypeFromResolvedType(resolvedSlotType);

        idCell.add(slotRole);
        idCell.add(compiledSlotType);
        resolvedParams.add(kb.ast.slot(slotRole, compiledSlotType));
        std::string paramNamespace;
        if (&resolvedSlotType.type() == &kb.type.ast.ResolvedType) {
            auto& paramTypeAst = static_cast<Struct&>(resolvedSlotType[kb.ids.ast]);
            Scope& scope       = static_cast<Scope&>(paramTypeAst[kb.ids.scope]);
            CellI& typeScopeId = scope.getFullId();
            paramNamespace     = typeScopeId.label();
            if (!paramNamespace.empty()) {
                paramNamespace += "::";
            }
        }
        ss << std::format("{}={}{}", slotRole.label(), paramNamespace, compiledSlotType.label());
    });
    ss << ">";
    idCell.label(ss.str());

    return idCell;
}

Ast::ResolvedType& Ast::Base::resolvedType(CellI& astType, CellI& compiledType)
{
    return ResolvedType::New(kb, astType, compiledType);
}

Ast::Parameter::Parameter(brain::Brain& kb, CellI& role) :
    BaseT<Parameter>(kb, kb.type.ast.Parameter)
{
    set("role", role);
}

Ast::ResolvedType::ResolvedType(brain::Brain& kb, CellI& astType, CellI& compiledType) :
    BaseT<ResolvedType>(kb, kb.type.ast.ResolvedType)
{
    set(kb.ids.ast, astType);
    set(kb.ids.compiled, compiledType);
}

Ast::Get& Ast::Parameter::operator/(Base& role)
{
    return Get::New(kb, *this, role);
}

Ast::Get& Ast::Parameter::operator/(const std::string& role)
{
    return Get::New(kb, *this, kb._(role));
}

Ast::Slot::Slot(brain::Brain& kb, CellI& role, CellI& type) :
    BaseT<Slot>(kb, kb.type.ast.Slot, "ast.slot")
{
    set("slotRole", role);
    set("slotType", type);
}

Ast::Call::Call(brain::Brain& kb, CellI& cell, CellI& method) :
    BaseT<Call>(kb, kb.type.ast.Call, "ast.call")
{
    set("cell", cell);
    set("method", method);
}

void Ast::Call::addParam(Slot& slot)
{
    if (missing("parameters")) {
        set("parameters", kb.list(slot));
    } else {
        List& paramList = static_cast<List&>(get("parameters"));
        paramList.add(slot);
    }
}

Ast::StaticCall::StaticCall(brain::Brain& kb, CellI& cell, CellI& method) :
    BaseT<StaticCall>(kb, kb.type.ast.StaticCall, "ast.staticCall")
{
    set("cell", cell);
    set("method", method);
}

void Ast::StaticCall::addParam(Slot& slot)
{
    if (missing("parameters")) {
        set("parameters", kb.list(slot));
    } else {
        List& paramList = static_cast<List&>(get("parameters"));
        paramList.add(slot);
    }
}

Ast::Cell::Cell(brain::Brain& kb, CellI& value) :
    BaseT<Cell>(kb, kb.type.ast.Cell)
{
    set("value", value);
}

Ast::Get& Ast::Cell::operator/(Base& role)
{
    return Get::New(kb, *this, role);
}

Ast::Get& Ast::Cell::operator/(const std::string& role)
{
    return Get::New(kb, *this, kb._(role));
}
Ast::StructName::StructName(brain::Brain& kb, CellI& name) :
    BaseT<StructName>(kb, kb.type.ast.StructName)
{
    set("name", name);
}

Ast::Self::Self(brain::Brain& kb) :
    BaseT<Self>(kb, kb.type.ast.Self, "ast.self")
{
}

Ast::Call& Ast::Self::call(const std::string& method)
{
    return kb.ast.call(*this, method);
}

Ast::SelfFn::SelfFn(brain::Brain& kb) :
    BaseT<SelfFn>(kb, kb.type.ast.SelfFn, "ast.selfFn")
{
}

Ast::Return::Return(brain::Brain& kb) :
    BaseT<Return>(kb, kb.type.ast.Return, "ast.return")
{
}

Ast::Return::Return(brain::Brain& kb, CellI& value) :
    BaseT<Return>(kb, kb.type.ast.Return, "ast.return")
{
    set("value", value);
}

Ast::Block::Block(brain::Brain& kb, List& list) :
    BaseT<Block>(kb, kb.type.ast.Block, "ast.block")
{
    set("asts", list);
}

Ast::Scope::Scope(brain::Brain& kb, const std::string& name) :
    BaseT<Scope>(kb, kb.type.ast.Scope, name)
{
    set("id", kb.id(name));
}

bool Ast::Scope::hasScope(CellI& id)
{
    if (missing("scopes")) {
        return false;
    }

    return scopes().hasKey(id);
}

Ast::Scope& Ast::Scope::getScope(const std::string& name)
{
    return getScope(kb.id(name));
}

Ast::Scope& Ast::Scope::getScope(CellI& id)
{
    if (missing("scopes")) {
        throw "No such scope";
    }

    if (scopes().hasKey(id)) {
        return static_cast<Ast::Scope&>(scopes().getValue(id));
    }

    throw "No such scope";
}

Ast::Scope& Ast::Scope::addScope(const std::string& name)
{
    auto& scope = *new Ast::Scope(kb, name);
    addScope(scope);

    return scope;
}

void Ast::Scope::addScope(Scope& scope)
{
    CellI& id = scope[kb.ids.id];

    if (missing("scopes")) {
        set("scopes", *new TrieMap(kb, kb.type.Cell, kb.type.ast.Scope, "TrieMap<Cell, Type::Ast::Scope>(...)"));
    }
    if (scopes().hasKey(id)) {
        throw "Already registered!";
    }

    scope.set("parent", *this);
    scopes().add(id, scope);
}

Ast::Function& Ast::Scope::addFunction(const std::string& name)
{
    return addFunction(kb.id(name), name);
}

Ast::Function& Ast::Scope::addFunction(CellI& id, const std::string& label)
{
    auto& function = *new Ast::Function(kb, id, label);
    addFunction(function);

    return function;
}

void Ast::Scope::addFunction(Function& function)
{
    auto& name = function[kb.ids.name];

    if (missing("functions")) {
        set("functions", *new TrieMap(kb, kb.type.Cell, kb.type.ast.Function, "TrieMap<Cell, Type::Ast::Function>(...)"));
    }
    if (functions().hasKey(name)) {
        throw "Already registered!";
    }
    function.set("scope", *this);
    functions().add(name, function);
}

Ast::FunctionT& Ast::Scope::addFunctionT(const std::string& name)
{
    return addFunctionT(kb.id(name), name);
}

Ast::FunctionT& Ast::Scope::addFunctionT(CellI& id, const std::string& label)
{
    if (missing("functionTs")) {
        set("functionTs", *new Map(kb, kb.type.Cell, kb.type.ast.FunctionT, "Map<Cell, Type::Ast::FunctionT>(...)"));
    }
    if (functionTs().hasKey(id)) {
        throw "Already registered!";
    }
    auto& functionT = *new Ast::FunctionT(kb, id, label);
    functionTs().add(id, functionT);
    functionT.set("scope", *this);

    return functionT;
}

bool Ast::Scope::hasVariable(CellI& id)
{
    if (missing("variables")) {
        if (missing("parent")) {
            return false;
        }
    } else if (variables().hasKey(id)) {
        return true;
    }

    if (missing("parent")) {
        return false;
    } else {
        return static_cast<Scope&>(get("parent")).hasVariable(id);
    }
}

Ast::Var& Ast::Scope::getVariable(CellI& id)
{
    if (missing("variables")) {
        if (missing("parent")) {
            throw "No such variable!";
        }
    } else if (variables().hasKey(id)) {
        return static_cast<Ast::Var&>(variables().getValue(id));
    }

    if (missing("parent")) {
        throw "No such variable!";
    } else {
        return static_cast<Scope&>(get("parent")).getVariable(id);
    }
}

Ast::Var& Ast::Scope::addVariable(CellI& id)
{
    if (missing("variables")) {
        set("variables", *new Map(kb, kb.type.Cell, kb.type.ast.Var, "Map<Cell, Type::Ast::Var>(...)"));
    }
    if (variables().hasKey(id)) {
        throw "Already registered!";
    }
    auto& var = *new Ast::Var(kb, id);
    variables().add(id, var);
    var.set("scope", *this);

    return var;
}

Ast::Struct& Ast::Scope::resolveStructName(CellI& structName)
{
    auto& name = structName[kb.ids.name];
    if (structName.has(kb.ids.scopes)) {
        auto& scopes = structName[kb.ids.scopes];
        return resolveFullStructId(scopes, name);
    } else {
        return resolveStructNameImpl(name);
    }
}

Ast::Struct& Ast::Scope::resolveStructNameImpl(CellI& name)
{
    if (missing("structs")) {
        if (missing("parent")) {
            throw "No struct in scope!";
        }
    } else if (structs().hasKey(name)) {
        return static_cast<Ast::Struct&>(structs().getValue(name));
    }

    if (missing("parent")) {
        throw "No struct in scope!";
    } else {
        return static_cast<Scope&>(get("parent")).resolveStructNameImpl(name);
    }
}


bool Ast::Scope::hasStruct(CellI& id)
{
    if (missing("structs")) {
        return false;
    }
    return (structs().hasKey(id));
}

Ast::Struct& Ast::Scope::getStruct(const std::string& name)
{
    return getStruct(kb.id(name));
}

Ast::Struct& Ast::Scope::getStruct(CellI& id)
{
    if (missing("structs")) {
        throw "No such struct";
    }
    return static_cast<Ast::Struct&>(structs().getValue(id));
}

void Ast::Scope::addStruct(Struct& struct_)
{
    CellI& id = struct_[kb.ids.id];

    if (missing("structs")) {
        set("structs", *new TrieMap(kb, kb.type.Cell, kb.type.ast.Struct, "TrieMap<Cell, Type::Ast::Struct>(...)"));
    }
    if (structs().hasKey(id)) {
        throw "Already registered!";
    }
    structs().add(id, struct_);
    struct_.set("scope", *this);
}

Ast::Struct& Ast::Scope::addStruct(const std::string& name)
{
    CellI& id = kb.id(name);

    auto& struct_ = *new Ast::Struct(kb, id, name);
    addStruct(struct_);

    return struct_;
}

bool Ast::Scope::hasStructT(CellI& id)
{
    if (missing("structTs")) {
        return false;
    }
    return structTs().hasKey(id);
}

Ast::StructT& Ast::Scope::getStructT(CellI& id)
{
    return static_cast<Ast::StructT&>(structTs().getValue(id));
}

Ast::StructT& Ast::Scope::addStructT(const std::string& name)
{
    CellI& id = kb.id(name);
    if (missing("structTs")) {
        set("structTs", *new TrieMap(kb, kb.type.Cell, kb.type.ast.StructT, "TrieMap<Cell, Type::Ast::StructT>(...)"));
    }
    if (structTs().hasKey(id)) {
        throw "Already registered!";
    }
    auto& structT = *new Ast::StructT(kb, id, name);
    structTs().add(id, structT);
    structT.set("scope", *this);

    return structT;
}


Ast::Struct& Ast::Scope::resolveFullStructId(CellI& scopeList, CellI& id)
{
    auto* currentScope = this;

    // resolve in local scope
    Visitor::visitList(scopeList, [this, &currentScope](CellI& scopeId, int, bool& stop) {
        if (currentScope->hasScope(scopeId)) {
            currentScope = &currentScope->getScope(scopeId);
        } else {
            currentScope = nullptr;
            stop         = true;
        }
    });
    if (currentScope && currentScope->hasStruct(id)) {
        return currentScope->getStruct(id);
    }

    // resolve in root scope
    currentScope = &getRootScope();
    Visitor::visitList(scopeList, [this, &currentScope](CellI& scopeId, int, bool& stop) {
        if (currentScope->hasScope(scopeId)) {
            currentScope = &currentScope->getScope(scopeId);
        } else {
            currentScope = nullptr;
            stop         = true;
        }
    });
    if (currentScope && currentScope->hasStruct(id)) {
        return currentScope->getStruct(id);
    }

    throw "Unknown Struct!";
}

Ast::Scope* Ast::Scope::resolveFullTemplateId(CellI& scopeList, CellI& id)
{
    auto* currentScope = this;

    // resolve in local scope
    Visitor::visitList(scopeList, [this, &currentScope](CellI& scopeId, int, bool& stop) {
        if (currentScope->hasScope(scopeId)) {
            currentScope = &currentScope->getScope(scopeId);
        } else {
            currentScope = nullptr;
            stop         = true;
        }
    });
    if (currentScope && currentScope->hasStructT(id)) {
        return currentScope;
    }

    // resolve in root scope
    currentScope = &getRootScope();
    Visitor::visitList(scopeList, [this, &currentScope](CellI& scopeId, int, bool& stop) {
        if (currentScope->hasScope(scopeId)) {
            currentScope = &currentScope->getScope(scopeId);
        } else {
            currentScope = nullptr;
            stop         = true;
        }
    });
    if (currentScope && currentScope->hasStructT(id)) {
        return currentScope;
    }

    return nullptr;
}

Ast::Scope& Ast::Scope::getRootScope()
{
    auto* currentScope = this;

    while (currentScope->has("parent")) {
        currentScope = &static_cast<Scope&>(currentScope->get("parent"));
    }

    return *currentScope;
}

CellI& Ast::Scope::getFullId()
{
    if (has("fullId")) {
        return get("fullId");
    }
    Scope& rootScope   = getRootScope();
    auto* currentScope = this;
    List& fullId = *new List(kb, kb.type.Char);
    while (currentScope && currentScope != &rootScope) {
        auto& currentScopeId = (*currentScope)[kb.ids.id];
        if (!fullId.empty()) {
            fullId.addFront(kb.pools.chars.get(':'));
            fullId.addFront(kb.pools.chars.get(':'));
        }
        Visitor::visitListInReverse(currentScopeId, [this, &fullId](CellI& character, int i, bool& stop) {
            fullId.addFront(character);
        });
        currentScope = currentScope->has("parent") ? &static_cast<Scope&>(currentScope->get("parent")) : nullptr;
    };
    std::stringstream ss;
    Visitor::visitList(fullId, [this, &fullId, &ss](CellI& character, int i, bool& stop) {
        ss << character.label();
    });
    fullId.label(ss.str());

    return fullId;
}

static bool debugCompiledStructs = true;

CellI& Ast::Scope::compile()
{
    auto& program     = *new Object(kb, kb.type.Program, "Program");
    auto& programData = *new Object(kb, kb.type.ScopeData, "ProgramData");
    program.set("data", programData);

    auto& resolvedScope = *new Ast::Scope(kb, label());
    set("resolvedScope", resolvedScope);

    auto& compileState      = *new Object(kb, kb.type.CompileState, "CompileState");
    auto& compiledFunctions = *new TrieMap(kb, kb.type.Cell, kb.type.op.Function, "Functions");
    auto& compiledStructs   = *new TrieMap(kb, kb.type.Cell, kb.type.Type_, "Types");

    auto& functions           = *new List(kb, kb.type.op.Function, "Functions");
    auto& structs             = *new TrieMap(kb, kb.type.Cell, kb.type.Type_, "structs");
    auto& unknownStructs      = *new TrieMap(kb, kb.type.Cell, kb.type.Type_, "unknownStructs");
    auto& instances           = *new TrieMap(kb, kb.type.Cell, kb.type.Type_, "instances");
    auto& unknownInstances    = *new TrieMap(kb, kb.type.Cell, kb.type.Type_, "unknownInstances");
    auto& instanceAsts        = *new TrieMap(kb, kb.type.Cell, kb.type.Type_, "instanceAsts");
    auto& unknownInstanceAsts = *new TrieMap(kb, kb.type.Cell, kb.type.Type_, "unknownInstanceAsts");
    auto& compiledVariables   = *new TrieMap(kb, kb.type.Cell, kb.type.op.Var, "Variables");

    programData.set("functions", compiledFunctions);
    programData.set("structs", compiledStructs);
    programData.set("variables", compiledVariables);

    compileState.set("functions", functions);
    compileState.set("structs", structs);
    compileState.set("unknownStructs", unknownStructs);
    compileState.set("instances", instances);
    compileState.set("unknownInstances", unknownInstances);
    compileState.set("instanceAsts", instanceAsts);
    compileState.set("unknownInstanceAsts", unknownInstanceAsts);
    compileState.set("variables", compiledVariables);
    compileState.set("resolvedScope", resolvedScope);
    compileState.set(kb.id("globalScope"), *this);
    compileState.set(kb.id("globalResolvedScope"), resolvedScope);

    resolveTypes(compileState);

    Visitor::visitList(unknownStructs[kb.ids.list], [this](CellI& unknownStruct, int i, bool& stop) {
        if (debugCompiledStructs) {
            std::cout << "unknown struct: " << unknownStruct[kb.ids.value][kb.ids.value].label() << std::endl;
        }
    });
    int instantiedNum = 0;
    Visitor::visitList(unknownInstances[kb.ids.list], [this, &compileState, &instantiedNum](CellI& unknownInstanceSlot, int i, bool& stop) {
        CellI& unknownInstance  = unknownInstanceSlot[kb.ids.value];
        auto& unknownInstanceId = unknownInstance[kb.ids.id];
        if (debugCompiledStructs) {
            std::cout << "unknown instance: " << unknownInstanceId.label() << std::endl;
            if (unknownInstance.has("currentStruct")) {
                std::cout << "     from struct: " << unknownInstance[kb.ids.currentStruct].label() << std::endl;
            }
            if (unknownInstance.has("currentFn")) {
                std::cout << "   from function: " << unknownInstance[kb.ids.currentFn].label() << std::endl;
            }
        }
        std::stringstream ss;

        CellI& templateId     = unknownInstance[kb.ids.templateId];
        CellI& templateParams = unknownInstance[kb.ids.templateParams];

        ss << std::format("  instantiate id: {} <", templateId.label());
        Visitor::visitList(templateParams, [this, &ss, &compileState](CellI& param, int i, bool& stop) {
            CellI& paramId   = param[kb.ids.slotRole];
            CellI& paramType = param[kb.ids.slotType];
            if (i > 0) {
                ss << ", ";
            }
            ss << std::format("{}: {}", paramId.label(), paramType.label());
        });
        ss << ">";
        if (debugCompiledStructs) {
            std::cout << ss.str() << "\n"
                      << std::endl;
        }

        auto& scope           = static_cast<Scope&>(unknownInstance[kb.ids.scope]);
        auto& idScope         = unknownInstance.has(kb.id("idScope")) ? static_cast<Scope&>(unknownInstance[kb.id("idScope")]) : scope;
        auto& resolvedIdScope = static_cast<Scope&>(idScope[kb.ids.resolvedScope]);
        compileState.set("scope", idScope);
        auto& structT          = idScope.getStructT(templateId);
        auto& instantiedStruct = structT.instantiateWith(static_cast<List&>(templateParams), compileState);
        auto& resolvedStruct = instantiedStruct.resolveTypes(compileState);
        resolvedIdScope.addStruct(resolvedStruct);
        instantiedNum = i + 1;
    });
    if (unknownStructs.size() > 0 || unknownInstances.size() != instantiedNum) {
        throw "Referencing to an unknown type!";
    }
    compileState.set("scope", *this);
    compileState.set("resolvedScope", resolvedScope);

    compileTheResolvedAsts(programData, compileState);

    return program;
}

void Ast::Scope::compileTheResolvedAsts(CellI& programData, CellI& state)
{
    auto& scope             = static_cast<Scope&>(state[kb.ids.scope]);
    auto& resolvedScope     = static_cast<Scope&>(state[kb.ids.resolvedScope]);
    auto& compiledFunctions = static_cast<TrieMap&>(programData[kb.ids.functions]);
    auto& compiledStructs   = static_cast<TrieMap&>(programData[kb.ids.structs]);
    auto& compiledVariables = static_cast<TrieMap&>(programData[kb.ids.variables]);

    if (scope.has("functions")) {
        Visitor::visitList(resolvedScope.functions()[kb.ids.list], [this, &state, &compiledFunctions](CellI& function, int i, bool& stop) {
            Ast::Function& astFunction = static_cast<Ast::Function&>(function[kb.ids.value]);
            auto& compiledFunction     = astFunction.compile(state);
            compiledFunctions.add(astFunction.getFullId(), compiledFunction);
        });
    }
    if (scope.has("structs")) {
        Visitor::visitList(resolvedScope.structs()[kb.ids.list], [this, &state, &compiledStructs](CellI& struct_, int i, bool& stop) {
            Ast::Struct& astStruct = static_cast<Ast::Struct&>(struct_[kb.ids.value]);
            auto& compiledStruct = astStruct.compile(state);
            compiledStructs.add(astStruct.getFullId(), compiledStruct);
        });
    }
    if (scope.has("variables")) {
        Visitor::visitList(resolvedScope.variables()[kb.ids.list], [this, &compiledVariables](CellI& var, int i, bool& stop) {
            Ast::Var& astVar       = static_cast<Ast::Var&>(var[kb.ids.value]);
            auto& varName    = astVar[kb.ids.role];
            auto& compiledVariable = *new Object(kb, kb.type.op.Var, std::format("var {}", astVar.label()));
            compiledVariables.add(varName, compiledVariable);
        });
    }
    if (scope.has("scopes")) {
        Visitor::visitList(scopes()[kb.ids.list], [this, &programData, &state, &resolvedScope](CellI& scope, int i, bool& stop) {
            Ast::Scope& astScope = static_cast<Ast::Scope&>(scope[kb.ids.value]);
            state.set("scope", astScope);
            state.set("resolvedScope", resolvedScope.getScope(astScope[kb.ids.id]));
            astScope.compileTheResolvedAsts(programData, state);
            state.set("scope", *this);
            state.set("resolvedScope", resolvedScope);
        });
    }
}

void Ast::Scope::resolveTypes(CellI& state)
{
    state.set("scope", *this);
    Ast::Scope& resolvedScope = static_cast<Ast::Scope&>(state[kb.ids.resolvedScope]);

    if (has("functions")) {
        state.erase("currentStruct");
        Visitor::visitList(functions()[kb.ids.list], [this, &state, &resolvedScope](CellI& origAstFunctionCell, int i, bool& stop) {
            Ast::Function& origAstFunction     = static_cast<Ast::Function&>(origAstFunctionCell[kb.ids.value]);
            Ast::Function& resolvedAstFunction = origAstFunction.resolveTypes(state);
            resolvedScope.addFunction(resolvedAstFunction);
            std::cout << resolvedAstFunction.label() << std::endl;
        });
    }
    if (has("structs")) {
        Visitor::visitList(structs()[kb.ids.list], [this, &state, &resolvedScope](CellI& origAstStructCell, int i, bool& stop) {
            Ast::Struct& origAstStruct     = static_cast<Ast::Struct&>(origAstStructCell[kb.ids.value]);
            Ast::Struct& resolvedAstStruct = origAstStruct.resolveTypes(state);
            resolvedScope.addStruct(resolvedAstStruct);
        });
    }
    if (has("variables")) {
        Visitor::visitList(variables()[kb.ids.list], [this, &state, &resolvedScope](CellI& origAstVarCell, int i, bool& stop) {
            Ast::Var& origAstVar = static_cast<Ast::Var&>(origAstVarCell[kb.ids.value]);
            resolvedScope.addVariable(origAstVar);
        });
    }
    if (has("scopes")) {
        Visitor::visitList(scopes()[kb.ids.list], [this, &state, &resolvedScope](CellI& origAstScopeCell, int i, bool& stop) {
            Ast::Scope& origAstScope = static_cast<Ast::Scope&>(origAstScopeCell[kb.ids.value]);
            auto& newResolvedScope   = *new Ast::Scope(kb, origAstScope.label());
            origAstScope.set("resolvedScope", newResolvedScope);
            resolvedScope.addScope(newResolvedScope);
            state.set("resolvedScope", newResolvedScope);
            origAstScope.resolveTypes(state);
            state.set("scope", *this);
            state.set("resolvedScope", resolvedScope);
        });
    }
}

TrieMap& Ast::Scope::variables()
{
    if (missing("variables")) {
        throw "No variables!";
    } else {
        return static_cast<TrieMap&>(get("variables"));
    }
}

TrieMap& Ast::Scope::scopes()
{
    if (missing("scopes")) {
        throw "No scopes!";
    } else {
        return static_cast<TrieMap&>(get("scopes"));
    }
}

TrieMap& Ast::Scope::functions()
{
    if (missing("functions")) {
        throw "No functions!";
    } else {
        return static_cast<TrieMap&>(get("functions"));
    }
}

TrieMap& Ast::Scope::functionTs()
{
    if (missing("functionTs")) {
        throw "No functions!";
    } else {
        return static_cast<TrieMap&>(get("functionTs"));
    }
}

TrieMap& Ast::Scope::structs()
{
    if (missing("structs")) {
        throw "No structs!";
    } else {
        return static_cast<TrieMap&>(get("structs"));
    }
}

TrieMap& Ast::Scope::structTs()
{
    if (missing("structTs")) {
        throw "No structs!";
    } else {
        return static_cast<TrieMap&>(get("structTs"));
    }
}

Ast::StructBase::StructBase(brain::Brain& kb, CellI& astType, CellI& id, const std::string& label) :
    Base(kb, astType, label)
{
    set("id", id);
}

Ast::StructBase::StructBase(brain::Brain& kb, CellI& astType, const std::string& name) :
    Base(kb, astType, name)
{
    set("id", kb.id(name));
}

Ast::Function& Ast::StructBase::addMethod(const std::string& name)
{
    auto& id = kb.id(name);
    Ast::Function& method = *new Ast::Function(kb, id, name);
    addMethod(method);

    return method;
}

void Ast::StructBase::addMethod(Function& method)
{
    auto& name = method[kb.ids.name];

    if (missing("methods")) {
        set("methods", *new Map(kb, kb.type.Cell, kb.type.ast.Function, "Map<Cell, Type::Ast::Function>(...)"));
    }
    if (methods().hasKey(name)) {
        throw "Already registered!";
    }
    method.set("structType", *this);
    methods().add(name, method);
}

Ast::StructBase& Ast::StructBase::members(Slot& slot)
{
    if (missing("members")) {
        set("members", *new Map(kb, kb.type.Cell, kb.type.ast.Slot));
    }
    members().add(slot[kb.ids.slotRole], slot);

    return *this;
}

Ast::StructBase& Ast::StructBase::subTypes(Slot& slot)
{
    if (missing("subTypes")) {
        set("subTypes", *new Map(kb, kb.type.Cell, kb.type.ast.Base));
    }
    CellI& slotRole = slot[kb.ids.slotRole];
    CellI& slotType = slot[kb.ids.slotType];

    subTypes().add(slotRole, slot);

    return *this;
}

Ast::StructBase& Ast::StructBase::memberOf(CellI& type)
{
    if (missing("memberOf")) {
        set("memberOf", *new List(kb, kb.type.Type_));
    }
    memberOf().add(type);

    return *this;
}

Map& Ast::StructBase::methods()
{
    if (missing("methods")) {
        throw "No methods!";
    } else {
        return static_cast<Map&>(get("methods"));
    }
}

Map& Ast::StructBase::members()
{
    if (missing("members")) {
        throw "No members!";
    } else {
        return static_cast<Map&>(get("members"));
    }
}

Map& Ast::StructBase::subTypes()
{
    if (missing("subTypes")) {
        throw "No subTypes!";
    } else {
        return static_cast<Map&>(get("subTypes"));
    }
}

List& Ast::StructBase::memberOf()
{
    if (missing("memberOf")) {
        throw "No memberOf!";
    } else {
        return static_cast<List&>(get("memberOf"));
    }
}

CellI& Ast::StructBase::id()
{
    return get("id");
}

Ast::Base& Ast::StructBase::getSubType(CellI& name)
{
    return static_cast<Ast::Base&>(subTypes().getValue(name)[kb.ids.slotType]);
}

Ast::Struct::Struct(brain::Brain& kb, CellI& id, const std::string& label) :
    StructBase(kb, kb.type.ast.Struct, id, label)
{
}

Ast::Struct::Struct(brain::Brain& kb, const std::string& name) :
    StructBase(kb, kb.type.ast.Struct, name)
{
    getFullId();
}

CellI& Ast::Struct::getFullId()
{
    if (has("fullId")) {
        return get("fullId");
    }
    Scope& scope = static_cast<Scope&>(get("scope"));
    CellI& scopeFullId = scope.getFullId();

    List& fullId = *new List(kb, kb.type.Char);
    Visitor::visitList(scopeFullId, [this, &fullId](CellI& character, int i, bool& stop) {
        fullId.add(character);
    });
    if (!fullId.empty()) {
        fullId.add(kb.pools.chars.get(':'));
        fullId.add(kb.pools.chars.get(':'));
    }
    Visitor::visitList(get(kb.ids.id), [this, &fullId](CellI& character, int i, bool& stop) {
        fullId.add(character);
    });
    std::stringstream ss;
    int templateParamPrintModeFromCharIndex = fullId.size();
    if (has("instanceOf")) {
        int paramsLength = static_cast<List&>(get("templateParams")).size();
        templateParamPrintModeFromCharIndex = fullId.size() - paramsLength * 2;
    }
    Visitor::visitList(fullId, [this, &fullId, &ss, &templateParamPrintModeFromCharIndex](CellI& character, int i, bool& stop) {
        if (i == templateParamPrintModeFromCharIndex) {
            stop = true;
            return;
        }
        ss << character.label();
    });
    if (has("instanceOf")) {
        ss << "<";
        Visitor::visitList(get("templateParams"), [this, &ss](CellI& slot, int i, bool& stop) {
            CellI& slotRole = slot[kb.ids.slotRole];
            CellI& slotType = slot[kb.ids.slotType];
            if (i != 0) {
                ss << ", ";
            }
            ss << slotRole.label() << "=" << slotType.label();
        });
        ss << ">";
    }
    fullId.label(ss.str());
    set("fullId", fullId);

    return fullId;
}

Ast::Struct& Ast::Struct::resolveTypes(CellI& state)
{
    auto& structId       = get("id");
    auto& structs        = static_cast<TrieMap&>(state[kb.ids.structs]);
    auto& unknownStructs = static_cast<TrieMap&>(state[kb.ids.unknownStructs]);
    Ast::Struct& ret     = *new Ast::Struct(kb, structId, std::format("{}", label()));

    if (has("instanceOf")) {
        ret.set("instanceOf", get("instanceOf"));
        ret.set("templateParams", get("templateParams"));
        ret.set("scope", get("scope"));
    }

    if (unknownStructs.hasKey(structId)) {
        unknownStructs.remove(structId);
    }
    auto& resolvedStruct = *new Object(kb, kb.type.Type_, std::format("{}", structId.label()));
    structs.add(getFullId(), resolvedStruct);

    state.set("currentStruct", ret);

    std::stringstream ss;
    std::stringstream subTypesSs;
    if (debugCompiledStructs) {
        ss << std::format("struct {}", label());
    }

    // resolve sub types
    if (has("subTypes")) {
        Visitor::visitList(subTypes()[kb.ids.list], [this, &ret, &state, &subTypesSs](CellI& subTypeCell, int i, bool& stop) {
            CellI& subTypeId           = subTypeCell[kb.ids.slotRole];
            CellI& subTypeType         = subTypeCell[kb.ids.slotType];
            CellI& resolvedSubTypeType = resolveType(subTypeType, state);
            ret.subTypes(kb.ast.slot(subTypeId, resolvedSubTypeType));
            if (debugCompiledStructs) {
                subTypesSs << std::format("    alias {} = {};", subTypeId.label(), getCompiledTypeFromResolvedType(resolvedSubTypeType).label()) << std::endl;
            }
        });
        if (debugCompiledStructs) {
            if (has("methods") || has("members")) {
                subTypesSs << std::endl;
            }
        }
    }

    // resolve memberOf list
    if (has("memberOf")) {
        ss << " : ";
        Visitor::visitList(memberOf(), [this, &ret, &state, &ss](CellI& membershipType, int i, bool& stop) {
            CellI& resolvedMembershipType = resolveType(membershipType, state);
            if (i > 0) {
                ss << ", ";
            }
            ss << getCompiledTypeFromResolvedType(resolvedMembershipType).label();
            ret.memberOf(resolvedMembershipType);
        });
    }
    if (debugCompiledStructs) {
        std::cout << ss.str() << " {" << std::endl;
        std::cout << subTypesSs.str();
    }

    // resolve methods
    if (has("methods")) {
        Visitor::visitList(methods()[kb.ids.list], [this, &ret, &state](CellI& origAstFunctionCell, int i, bool& stop) {
            auto& origAstFunction     = static_cast<Ast::Function&>(origAstFunctionCell);
            auto& resolvedAstFunction = origAstFunction.resolveTypes(state);
            ret.addMethod(resolvedAstFunction);
            if (debugCompiledStructs) {
                std::cout << std::format("    {};\n", resolvedAstFunction.shortName());
            }
        });
        if (debugCompiledStructs) {
            if (has("members")) {
                std::cout << std::endl;
            }
        }
    }
    // resolve members
    if (has("members")) {
        CellI& membersList = members()[kb.ids.list];
        Visitor::visitList(membersList, [this, &ret, &state](CellI& memberCell, int i, bool& stop) {
            CellI& memberId   = memberCell[kb.ids.slotRole];
            CellI& memberType = memberCell[kb.ids.slotType];
            CellI& resolvedMemberType = resolveType(memberType, state);
            ret.members(kb.ast.slot(memberId, resolvedMemberType));
            if (debugCompiledStructs) {
                std::cout << std::format("    {}: {};", memberId.label(), getCompiledTypeFromResolvedType(resolvedMemberType).label()) << std::endl;
            }
        });
    }

    if (debugCompiledStructs) {
        std::cout << "}" << std::endl;
    }

    return ret;
}

CellI& Ast::Struct::compile(CellI& state)
{
    CellI& compiledStruct = getResolvedTypeById(getFullId(), has("instanceOf"), state);
    compiledStruct.erase("incomplete");
    // std::cout << std::format("DDDD compile {} resolved at {:p}\n", structId.label(), (void*)&compiledStruct) << std::endl;

    // compile sub types
    if (has("subTypes")) {
        Map& compiledSubTypes = *new Map(kb, kb.type.Cell, kb.type.Type_, "subTypes Map<Cell, Type>(...)");
        Visitor::visitList(subTypes()[kb.ids.list], [this, &compiledSubTypes](CellI& slot, int i, bool& stop) {
            CellI& slotRole        = slot[kb.ids.slotRole];
            CellI& slotType        = slot[kb.ids.slotType];
            auto& compiledSlotType = getCompiledTypeFromResolvedType(slotType);
            compiledSubTypes.add(slotRole, compiledSlotType);
        });
        compiledStruct.set("subTypes", compiledSubTypes);
    }

    // compile methods
    if (has("methods")) {
        Map& compiledMethods = *new Map(kb, kb.type.Cell, kb.type.ast.Function);
        Visitor::visitList(methods()[kb.ids.list], [this, &compiledMethods, &state](CellI& astFunction, int i, bool& stop) {
            auto& compiledFunction = static_cast<Ast::Function&>(astFunction).compile(state);
            compiledMethods.add(astFunction[kb.ids.name], compiledFunction);
        });
        compiledStruct.set("methods", compiledMethods);
    }

    // compile members
    if (has("members")) {
        Map& compiledMembers = *new Map(kb, kb.type.Cell, kb.type.Slot, "members Map<Cell, Slot>(...)");
        Visitor::visitList(members()[kb.ids.list], [this, &compiledMembers, &compiledStruct, &state](CellI& slot, int i, bool& stop) {
            CellI& slotRole          = slot[kb.ids.slotRole];
            CellI& slotType          = slot[kb.ids.slotType];
            auto& compiledSlotType   = getCompiledTypeFromResolvedType(slotType);
            compiledMembers.add(slotRole, kb.type.slot(slotRole, compiledSlotType));
        });
        compiledStruct.set("slots", compiledMembers);
    }

    // compile memberOf list
    if (has("memberOf")) {
        Map& compiledMemberOfs = *new Map(kb, kb.type.Type_, kb.type.Type_, "memberOf Map<Type, Type>(...)");
        Visitor::visitList(memberOf(), [this, &compiledMemberOfs](CellI& membershipType, int i, bool& stop) {
            auto& compiledMembershipType = getCompiledTypeFromResolvedType(membershipType);
            compiledMemberOfs.add(compiledMembershipType, compiledMembershipType);
        });
        compiledStruct.set("memberOf", compiledMemberOfs);
    }

    return compiledStruct;
}

Ast::StructT::StructT(brain::Brain& kb, CellI& id, const std::string& label) :
    StructBase(kb, kb.type.ast.StructT, id, label)
{
}

Ast::StructT::StructT(brain::Brain& kb, const std::string& name) :
    StructBase(kb, kb.type.ast.Struct, name)
{
}

Ast::StructT& Ast::StructT::templateParams(Slot& slot)
{
    if (missing("templateParams")) {
        set("templateParams", *new Map(kb, kb.type.Cell, kb.type.Type_));
    }
    CellI& slotRole = slot[kb.ids.slotRole];
    CellI& slotType = slot[kb.ids.slotType];
    if (!(&slotType.type() == &kb.type.ast.Cell || &slotType.type() == &kb.type.ast.TemplatedType)) {
        throw "Invalid template param type!";
    }
    CellI* paramType = nullptr;
    if (&slotType.type() == &kb.type.ast.Cell) {
        paramType = &slotType[kb.ids.value];
    } else {
        throw "TODO";
    }
    templateParams().add(slotRole, *paramType);

    return *this;
}

Ast::Struct& Ast::StructT::instantiateWith(List& inputParams, CellI& state)
{
    // process input parameters
    if (!inputParams.empty() && missing("templateParams")) {
        throw "No template parameter was given!";
    }
    std::stringstream ss;
    Map inputParameters(kb, kb.type.Cell, kb.type.Cell);
    List& idCell = *new List(kb, kb.type.Cell);
    Visitor::visitList(id(), [this, &idCell](CellI& character, int i, bool& stop) {
        idCell.add(character);
    });

    Visitor::visitList(inputParams, [this, &inputParameters, &ss, &idCell](CellI& slot, int i, bool& stop) {
        CellI& slotRole = slot[kb.ids.slotRole];
        CellI& slotType = slot[kb.ids.slotType];
        inputParameters.add(slotRole, slotType);
        if (i != 0) {
            ss << ", ";
        }
        ss << slotRole.label() << "=" << slotType.label();
        if (!templateParams().hasKey(slotRole)) {
            throw "Instantiating with unknown template parameter!";
        }
        idCell.add(slotRole);
        idCell.add(slotType);
    });
    idCell.label(std::format("{}<{}>", id().label(), ss.str()));
    Ast::Struct* retPtr = nullptr;
    auto& unknownInstanceAsts = static_cast<TrieMap&>(state["unknownInstanceAsts"]);
    if (unknownInstanceAsts.hasKey(idCell)) {
        retPtr = &static_cast<Ast::Struct&>(unknownInstanceAsts.getValue(idCell)[kb.ids.value]);
     } else {
        auto& instanceAsts = static_cast<TrieMap&>(state["instanceAsts"]);
        retPtr             = new Ast::Struct(kb, idCell, std::format("{}<{}>", id().label(), ss.str()));
        instanceAsts.add(idCell, *retPtr);
    }
    Ast::Struct& ret = *retPtr;
    ret.set("instanceOf", *this);
    ret.set("templateParams", inputParams);
    ret.set("scope", static_cast<Scope&>(get("scope")));

    // instantiate sub types
    if (has("subTypes")) {
        Map& instantiatedSubTypes = *new Map(kb, kb.type.Cell, kb.type.ast.Base);
        Visitor::visitList(subTypes()[kb.ids.list], [this, &inputParameters, &instantiatedSubTypes, &ret, &state](CellI& slot, int i, bool& stop) {
            CellI& slotRole          = slot[kb.ids.slotRole];
            CellI& slotType          = slot[kb.ids.slotType];
            CellI& instantiatedParam = instantiateTemplateParamType(slotType, ret, inputParameters, state);
            instantiatedSubTypes.add(slotRole, kb.ast.slot(slotRole, instantiatedParam));
        });
        ret.set("subTypes", instantiatedSubTypes);
    }

    // instantiate methods
    if (has("methods")) {
        Visitor::visitList(methods()[kb.ids.list], [this, &inputParameters, &ret, &state](CellI& astFunction, int i, bool& stop) {
            Ast::Function& instantiedFunction = *new Function(kb, astFunction[kb.ids.name], astFunction[kb.ids.name].label());
            ret.addMethod(instantiedFunction);

            // parameters
            if (astFunction.has("parameters")) {
                List& instantiatedParameters = *new List(kb, kb.type.Slot);
                Visitor::visitList(astFunction[kb.ids.parameters], [this, &inputParameters, &instantiatedParameters, &ret, &state](CellI& slot, int i, bool& stop) {
                    CellI& slotRole          = slot[kb.ids.slotRole];
                    CellI& slotType          = slot[kb.ids.slotType];
                    CellI& instantiatedParam = instantiateTemplateParamType(slotType, ret, inputParameters, state);
                    instantiatedParameters.add(kb.ast.slot(slotRole, instantiatedParam));
                });
                instantiedFunction.set("parameters", instantiatedParameters);
            }
            // return type
            if (astFunction.has("returnType")) {
                CellI& returnType             = astFunction[kb.ids.returnType];
                CellI& instantiatedReturnType = instantiateTemplateParamType(returnType, ret, inputParameters, state);
                instantiedFunction.set("returnType", instantiatedReturnType);
            }
            // code
            if (astFunction.has("code")) {
                instantiedFunction.set("code", instantiateAst(astFunction[kb.ids.code], ret, inputParameters, state));
            }
        });
    }

    // instantiate members
    if (has("members")) {
        Map& instantiatedMembers = *new Map(kb, kb.type.Cell, kb.type.Slot);
        Visitor::visitList(members()[kb.ids.list], [this, &inputParameters, &instantiatedMembers, &ret, &state](CellI& slot, int i, bool& stop) {
            CellI& slotRole = slot[kb.ids.slotRole];
            CellI& slotType = slot[kb.ids.slotType];
            CellI& instantiatedParam = instantiateTemplateParamType(slotType, ret, inputParameters, state);
            instantiatedMembers.add(slotRole, kb.ast.slot(slotRole, instantiatedParam));
        });
        ret.set("members", instantiatedMembers);
    }

    // instantiate memberOf list
    if (has("memberOf")) {
        List& instantiatedMemberOfs = *new List(kb, kb.type.Type_);
        Visitor::visitList(memberOf(), [this, &inputParameters, &instantiatedMemberOfs, &ret, &state](CellI& membershipType, int i, bool& stop) {
            CellI& instantiatedParam = instantiateTemplateParamType(membershipType, ret, inputParameters, state);
            instantiatedMemberOfs.add(instantiatedParam);
        });
        ret.set("memberOf", instantiatedMemberOfs);
    }

    return ret;
}

CellI& Ast::StructT::instantiateTemplateParamType(CellI& param, CellI& selfType, Map& inputParameters, CellI& state)
{
    if (&param.type() == &kb.type.ast.TemplateParam) {
        CellI& paramValue = param[kb.ids.role];
        if (!inputParameters.hasKey(paramValue)) {
            throw "Instantiating with unknown template parameter!";
        }
        return kb.ast.cell(inputParameters.getValue(paramValue));
    }
    if (&param.type() == &kb.type.ast.TemplatedType) {
        List& resolvedParameterList = *new List(kb, kb.type.Slot);
        auto& ret                   = *new TemplatedType(kb, param[kb.ids.id], resolvedParameterList);
        auto& parametersList        = param[kb.ids.parameters];

        Visitor::visitList(parametersList, [this, &resolvedParameterList, &selfType, &inputParameters, &state](CellI& slot, int, bool&) {
            CellI& slotRole         = slot[kb.ids.slotRole];
            CellI& slotType         = slot[kb.ids.slotType];
            CellI& resolvedSlotType = instantiateTemplateParamType(slotType, selfType, inputParameters, state);
            resolvedParameterList.add(kb.ast.slot(slotRole, resolvedSlotType));
        });

        return ret;
    }
    if (&param.type() == &kb.type.ast.Cell || &param.type() == &kb.type.ast.StructName || &param.type() == &kb.type.ast.SubTypeName) {
        return param;
    }

    throw "Unknown template parameter!";
}

Ast::Base& Ast::StructT::instantiateAst(CellI& ast, CellI& selfType, Map& inputParameters, CellI& state)
{
    auto instantiate = [this, &selfType, &inputParameters, &state](CellI& ast) -> Ast::Base& { return instantiateAst(ast, selfType, inputParameters, state); };

    if (&ast.type() == &kb.type.ast.New) {
        auto* objectTypePtr = &ast[kb.ids.objectType];
        if (&(*objectTypePtr).type() == &kb.type.ast.TemplatedType) {
            CellI& resolvedObjectType = instantiateTemplateParamType(*objectTypePtr, selfType, inputParameters, state);
            objectTypePtr             = &resolvedObjectType;
        }
        auto& objectType = *static_cast<Base*>(objectTypePtr);
        if (ast.has("constructor")) {
            auto& constructor = ast[kb.ids.constructor];
            Ast::Base& ret    = kb.ast.new_(objectType, static_cast<Base&>(constructor));
            if (ast.has("parameters")) {
                auto& newParameters = *new cells::List(kb, kb.type.ast.Slot);
                Visitor::visitList(ast[kb.ids.parameters], [this, &newParameters, &instantiate](CellI& slot, int, bool&) {
                    newParameters.add(kb.ast.slot(slot[kb.ids.slotRole], instantiate(slot[kb.ids.slotType])));
                });
                ret.set("parameters", newParameters);
            }
            return ret;
        } else {
            return kb.ast.new_(objectType);
        }
    } else if (&ast.type() == &kb.type.ast.Call) {
        Ast::Base& ret = kb.ast.call(instantiate(ast[kb.ids.cell]), ast[kb.ids.method]);
        if (ast.has("parameters")) {
            // TODO process parameters
            ret.set("parameters", ast[kb.ids.parameters]);
        }
        return ret;
    } else if (&ast.type() == &kb.type.ast.StaticCall) {
        Ast::Base& ret = kb.ast.scall(instantiate(ast[kb.ids.cell]), ast[kb.ids.method]);
        if (ast.has("parameters")) {
            // TODO process parameters
            ret.set("parameters", ast[kb.ids.parameters]);
        }
        return ret;
    } 

    // do nothing just traverse and copy the AST nodes
    if (&ast.type() == &kb.type.ast.Block) {
        auto& instantiedAsts = *new cells::List(kb, kb.type.ast.Base);
        Visitor::visitList(ast[kb.ids.asts], [this, &instantiedAsts, &instantiate](CellI& ast, int, bool&) {
            instantiedAsts.add(instantiate(ast));
        });
        return *new Block(kb, instantiedAsts);
    } else if (&ast.type() == &kb.type.ast.Cell) {
        return kb.ast.cell(ast[kb.ids.value]);
    } else if (&ast.type() == &kb.type.ast.StructName) {
        auto& ret = kb.ast.structName(ast[kb.ids.value]);
        if (ast.has(kb.ids.scopes)) {
            ret.set(kb.ids.scopes, ast[kb.ids.scopes]);
        }
        return ret;
    } else if (&ast.type() == &kb.type.ast.SelfFn) {
        return kb.ast.selfFn();
    } else if (&ast.type() == &kb.type.ast.Self) {
        return kb.ast.self();
    } else if (&ast.type() == &kb.type.ast.Parameter) {
        return kb.ast.parameter(ast[kb.ids.role]);
    } else if (&ast.type() == &kb.type.ast.Var) {
        return kb.ast.var(ast[kb.ids.role]);
    } else if (&ast.type() == &kb.type.ast.Delete) {
        return kb.ast.delete_(instantiate(ast[kb.ids.cell]));
    } else if (&ast.type() == &kb.type.ast.Set) {
        return kb.ast.set(instantiate(ast[kb.ids.cell]), instantiate(ast[kb.ids.role]), instantiate(ast[kb.ids.value]));
    } else if (&ast.type() == &kb.type.ast.Erase) {
        return kb.ast.erase(instantiate(ast[kb.ids.cell]), instantiate(ast[kb.ids.role]));
    } else if (&ast.type() == &kb.type.ast.If) {
        if (ast.has("else_")) {
            return kb.ast.if_(instantiate(ast[kb.ids.condition]), instantiate(ast[kb.ids.then]), instantiate(ast[kb.ids.else_]));
        } else {
            return kb.ast.if_(instantiate(ast[kb.ids.condition]), instantiate(ast[kb.ids.then]));
        }
    } else if (&ast.type() == &kb.type.ast.Do) {
        return kb.ast.do_(instantiate(ast[kb.ids.statement]), instantiate(ast[kb.ids.condition]));
    } else if (&ast.type() == &kb.type.ast.While) {
        return kb.ast.while_(instantiate(ast[kb.ids.condition]), instantiate(ast[kb.ids.statement]));
    } else if (&ast.type() == &kb.type.ast.And) {
        return kb.ast.and_(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.Or) {
        return kb.ast.or_(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.Not) {
        return kb.ast.not_(instantiate(ast[kb.ids.input]));
    } else if (&ast.type() == &kb.type.ast.Add) {
        return kb.ast.add(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.Subtract) {
        return kb.ast.subtract(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.Multiply) {
        return kb.ast.multiply(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.Divide) {
        return kb.ast.divide(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.LessThan) {
        return kb.ast.lessThan(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.LessThanOrEqual) {
        return kb.ast.lessThanOrEqual(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.GreaterThan) {
        return kb.ast.greaterThan(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.GreaterThanOrEqual) {
        return kb.ast.greaterThanOrEqual(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.Same) {
        return kb.ast.same(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.NotSame) {
        return kb.ast.notSame(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.Equal) {
        return kb.ast.equal(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.NotEqual) {
        return kb.ast.notEqual(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.Has) {
        return kb.ast.has(instantiate(ast[kb.ids.cell]), instantiate(ast[kb.ids.role]));
    } else if (&ast.type() == &kb.type.ast.Missing) {
        return kb.ast.missing(instantiate(ast[kb.ids.cell]), instantiate(ast[kb.ids.role]));
    } else if (&ast.type() == &kb.type.ast.Get) {
        return kb.ast.get(instantiate(ast[kb.ids.cell]), instantiate(ast[kb.ids.role]));
    } else if (&ast.type() == &kb.type.ast.Member) {
        return kb.ast.member(ast[kb.ids.role]);
    } else if (&ast.type() == &kb.type.ast.Return) {
        if (ast.has("value")) {
            return kb.ast.return_(static_cast<Base&>(instantiate(ast[kb.ids.value])));
        }
        return kb.ast.return_();
    }

    throw "Unknown AST to instantiate!";
}

Map& Ast::StructT::templateParams()
{
    if (missing("templateParams")) {
        throw "No templateParams!";
    } else {
        return static_cast<Map&>(get("templateParams"));
    }
}

Ast::Function::Function(brain::Brain& kb, CellI& name, const std::string& label) :
    BaseT<Function>(kb, kb.type.ast.Function, label)
{
    set("name", name);
    this->label(label);
}

Ast::Function::Function(brain::Brain& kb, const std::string& name) :
    BaseT<Function>(kb, kb.type.ast.Function, name)
{
    set("id", kb.pools.strings.get(name)[kb.ids.value]);
    label(name);
}

Ast::Function& Ast::Function::parameters(Slot& param)
{
    if (missing("parameters")) {
        set("parameters", *new List(kb, kb.type.Slot));
    }
    parameters().add(param);

    return *this;
}

Ast::Function& Ast::Function::returnType(CellI& type)
{
    set("returnType", type);

    return *this;
}

void Ast::Function::addBlock(Block& block)
{
    set("code", block);
}

Ast::Function& Ast::Function::resolveTypes(CellI& state)
{
    Ast::Function& ret = *new Ast::Function(kb, get("name"), std::format("{}", label()));
    state.set("currentFn", ret);
    std::stringstream ss;

    if (has("static_")) {
        ret.set("static_", get("static_"));
        ss << "static ";
    }
    ss << "fn";
    if (has("structType")) {
        auto& structType = get("structType");
        ret.set("structType", structType);
        ss << std::format(" {}::{}(", structType.label(), get("name").label());
    } else {
        ss << std::format(" {}(", get("name").label());
    }

    if (has("parameters")) {
        Visitor::visitList(parameters(), [this, &state, &ret, &ss](CellI& param, int i, bool& stop) {
            CellI& paramId           = param[kb.ids.slotRole];
            CellI& paramType         = param[kb.ids.slotType];
            CellI& resolvedParamType = resolveType(paramType, state);
            CellI& compiledParamType = getCompiledTypeFromResolvedType(resolvedParamType);
            if (i > 0) {
                ss << ", ";
            }
            ss << std::format("{}: {}", paramId.label(), compiledParamType.label());
            ret.parameters(kb.ast.slot(paramId, resolvedParamType));
        });
    }
    ss << ")";
    if (has("returnType")) {
        ss << " -> ";
        CellI& retType = returnType();
        CellI& resolvedRetType = resolveType(retType, state);
        CellI& compiledRetType = getCompiledTypeFromResolvedType(resolvedRetType);
        ss << compiledRetType.label();
        ret.returnType(resolvedRetType);
    }
    ret.label(ss.str());

    CellI& resolvedCodeAst = resolveTypesInCode(state, code());
    ret.set("code", resolvedCodeAst);

    return ret;
}

Ast::Base& Ast::Function::resolveTypesInCode(CellI& resolveState, CellI& ast)
{
    auto resolveNode = [this, &resolveState](CellI& ast) -> Ast::Base& { return resolveTypesInCode(resolveState, ast); };

    if (&ast.type() == &kb.type.ast.New) {
        CellI& objectType                = ast[kb.ids.objectType];
        Ast::Base* resolvedObjectTypePtr = nullptr;
        if (&objectType.type() == &kb.type.ast.Member) {
            resolvedObjectTypePtr = &static_cast<Ast::Base&>(objectType);
        } else {
            resolvedObjectTypePtr = &resolveType(objectType, resolveState);
        }
        Ast::Base& resolvedObjectType = *resolvedObjectTypePtr;

        if (ast.has("constructor")) {
            auto& constructor = ast[kb.ids.constructor];
            Ast::Base& ret    = kb.ast.new_(resolvedObjectType, static_cast<Base&>(ast[kb.ids.constructor]));

            if (ast.has("parameters")) {
                auto& newParameters = *new cells::List(kb, kb.type.ast.Slot);
                Visitor::visitList(ast[kb.ids.parameters], [this, &newParameters, &resolveNode, &resolveState](CellI& slot, int, bool&) {
                    newParameters.add(kb.ast.slot(slot[kb.ids.slotRole], resolveNode(slot[kb.ids.slotType])));
                });
                ret.set("parameters", newParameters);
            }
            return ret;
        } else {
            return kb.ast.new_(resolvedObjectType);
        }
    }

    if (&ast.type() == &kb.type.ast.Call || &ast.type() == &kb.type.ast.StaticCall) {

        Ast::Base* retPtr = nullptr;
        if (&ast.type() == &kb.type.ast.Call) {
            retPtr = &kb.ast.call(resolveNode(ast[kb.ids.cell]), ast[kb.ids.method]);
        } else {
            retPtr = &kb.ast.scall(resolveType(ast[kb.ids.cell], resolveState), ast[kb.ids.method]);
        }
        Ast::Base& ret = *retPtr;
        if (ast.has("parameters")) {
            auto& newParameters = *new cells::List(kb, kb.type.ast.Slot);
            Visitor::visitList(ast[kb.ids.parameters], [this, &newParameters, &resolveNode, &resolveState](CellI& slot, int, bool&) {
                newParameters.add(kb.ast.slot(slot[kb.ids.slotRole], resolveNode(slot[kb.ids.slotType])));
            });
            ret.set("parameters", newParameters);
        }
        return ret;
    } else if (&ast.type() == &kb.type.ast.StructName) {
        return resolveType(ast, resolveState);
    }

    // do nothing just traverse and copy the AST nodes
    if (&ast.type() == &kb.type.ast.Block) {
        auto& instantiedAsts = *new cells::List(kb, kb.type.ast.Base);
        Visitor::visitList(ast[kb.ids.asts], [this, &instantiedAsts, &resolveNode](CellI& ast, int, bool&) {
            instantiedAsts.add(resolveNode(ast));
        });
        return *new Block(kb, instantiedAsts);
    } else if (&ast.type() == &kb.type.ast.Cell) {
        return kb.ast.cell(ast[kb.ids.value]);
    } else if (&ast.type() == &kb.type.ast.SelfFn) {
        return kb.ast.selfFn();
    } else if (&ast.type() == &kb.type.ast.Self) {
        return kb.ast.self();
    } else if (&ast.type() == &kb.type.ast.Parameter) {
        return kb.ast.parameter(ast[kb.ids.role]);
    } else if (&ast.type() == &kb.type.ast.Var) {
        return kb.ast.var(ast[kb.ids.role]);
    } else if (&ast.type() == &kb.type.ast.Delete) {
        return kb.ast.delete_(resolveNode(ast[kb.ids.cell]));
    } else if (&ast.type() == &kb.type.ast.Set) {
        return kb.ast.set(resolveNode(ast[kb.ids.cell]), resolveNode(ast[kb.ids.role]), resolveNode(ast[kb.ids.value]));
    } else if (&ast.type() == &kb.type.ast.Erase) {
        return kb.ast.erase(resolveNode(ast[kb.ids.cell]), resolveNode(ast[kb.ids.role]));
    } else if (&ast.type() == &kb.type.ast.If) {
        if (ast.has("else_")) {
            return kb.ast.if_(resolveNode(ast[kb.ids.condition]), resolveNode(ast[kb.ids.then]), resolveNode(ast[kb.ids.else_]));
        } else {
            return kb.ast.if_(resolveNode(ast[kb.ids.condition]), resolveNode(ast[kb.ids.then]));
        }
    } else if (&ast.type() == &kb.type.ast.Do) {
        return kb.ast.do_(resolveNode(ast[kb.ids.statement]), resolveNode(ast[kb.ids.condition]));
    } else if (&ast.type() == &kb.type.ast.While) {
        return kb.ast.while_(resolveNode(ast[kb.ids.condition]), resolveNode(ast[kb.ids.statement]));
    } else if (&ast.type() == &kb.type.ast.And) {
        return kb.ast.and_(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.Or) {
        return kb.ast.or_(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.Not) {
        return kb.ast.not_(resolveNode(ast[kb.ids.input]));
    } else if (&ast.type() == &kb.type.ast.Add) {
        return kb.ast.add(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.Subtract) {
        return kb.ast.subtract(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.Multiply) {
        return kb.ast.multiply(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.Divide) {
        return kb.ast.divide(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.LessThan) {
        return kb.ast.lessThan(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.LessThanOrEqual) {
        return kb.ast.lessThanOrEqual(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.GreaterThan) {
        return kb.ast.greaterThan(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.GreaterThanOrEqual) {
        return kb.ast.greaterThanOrEqual(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.Same) {
        return kb.ast.same(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.NotSame) {
        return kb.ast.notSame(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.Equal) {
        return kb.ast.equal(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.NotEqual) {
        return kb.ast.notEqual(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.Has) {
        return kb.ast.has(resolveNode(ast[kb.ids.cell]), resolveNode(ast[kb.ids.role]));
    } else if (&ast.type() == &kb.type.ast.Missing) {
        return kb.ast.missing(resolveNode(ast[kb.ids.cell]), resolveNode(ast[kb.ids.role]));
    } else if (&ast.type() == &kb.type.ast.Get) {
        return kb.ast.get(resolveNode(ast[kb.ids.cell]), resolveNode(ast[kb.ids.role]));
    } else if (&ast.type() == &kb.type.ast.Member) {
        return kb.ast.member(ast[kb.ids.role]);
    } else if (&ast.type() == &kb.type.ast.Return) {
        if (ast.has("value")) {
            return kb.ast.return_(static_cast<Base&>(resolveNode(ast[kb.ids.value])));
        }
        return kb.ast.return_();
    }

    throw "Unknown AST to instantiate!";
}

CellI& Ast::Function::compile(CellI& state)
{
    cells::Object& functionType = *new cells::Object(kb, kb.type.Type_);
    functionType.set("memberOf", kb.map(kb.type.Type_, kb.type.Type_, kb.type.op.Function, kb.type.op.Function));
    cells::Map& subTypesMap = kb.map(kb.type.Cell, kb.type.Type_,
                                     kb.ids.name, get("name"));
    functionType.set("subTypes", subTypesMap);
    functionType.set("slots", kb.type.op.Function[kb.ids.slots]);

    cells::Object& function = *new cells::Object(kb, functionType);
    compileParams(function, subTypesMap, state);
    functionType.label(std::format("Type for {}", function.label()));
    function.set("ast", *this);
    function.set("op", compileAst(code(), function, state));
    if (has("static_")) {
        function.set("static_", get("static_"));
    }

    return function;
}

std::string Ast::Function::shortName()
{
    std::stringstream iss;
    std::stringstream oss;
    if (has("parameters") || has("structType")) {
        if (has("parameters")) {
            Visitor::visitList(parameters(), [this, &iss](CellI& slot, int i, bool& stop) {
                if (i > 0) {
                    iss << ", ";
                }
                iss << "p_" << slot[kb.ids.slotRole].label() << ": " << getCompiledTypeFromResolvedType(slot[kb.ids.slotType]).label();
            });
        }
    }
    if (has("returnType")) {
        oss << getCompiledTypeFromResolvedType(returnType()).label();
    }
    if (has("returnType")) {
        return std::format("fn {}({}) -> {}", get("name").label(), iss.str(), oss.str());
    } else {
        return std::format("fn {}({})", get("name").label(), iss.str());
    }
}

CellI& Ast::Function::getFullId()
{
    if (has("fullId")) {
        return get("fullId");
    }

    List& fullId = *new List(kb, kb.type.Cell);
    if (has("structType")) {
        Struct& structType = static_cast<Struct&>(get("structType"));
        Visitor::visitList(structType.getFullId(), [this, &fullId](CellI& character, int i, bool& stop) {
            fullId.add(character);
        });
    } else {
        Scope& scope       = static_cast<Scope&>(get("scope"));
        CellI& scopeFullId = scope.getFullId();

        Visitor::visitList(scopeFullId, [this, &fullId](CellI& character, int i, bool& stop) {
            fullId.add(character);
        });
    }
    if (!fullId.empty()) {
        fullId.add(kb.pools.chars.get(':'));
        fullId.add(kb.pools.chars.get(':'));
    }
    Visitor::visitList(get(kb.ids.name), [this, &fullId](CellI& character, int i, bool& stop) {
        fullId.add(character);
    });
    std::stringstream ss;
    Visitor::visitList(fullId, [this, &fullId, &ss](CellI& character, int i, bool& stop) {
        ss << character.label();
    });
    fullId.label(ss.str());

    return fullId;
}

void Ast::Function::compileParams(cells::Object& function, cells::Map& subTypesMap, CellI& state)
{
    std::stringstream iss;
    std::stringstream oss;
    std::string structTypeStr;
    if (has("parameters") || has("structType")) {
        cells::Object& parametersType = *new cells::Object(kb, kb.type.Type_);
        Map& slots                    = *new Map(kb, kb.type.Cell, kb.type.Slot);
        if (has("structType")) {
            CellI& type = get("structType");
            Object& var = *new Object(kb, kb.type.op.Var, "self");
            var.set("valueType", type);
            slots.add(kb.id("self"), kb.type.slot("self", type));
            structTypeStr = std::format("{}::", type.label());
        }
        if (has("parameters")) {
            Visitor::visitList(parameters(), [this, &slots, &iss](CellI& slot, int i, bool& stop) {
                if (i > 0) {
                    iss << ", ";
                }
                auto& slotRole         = slot[kb.ids.slotRole];
                auto& slotType         = slot[kb.ids.slotType];
                auto& compiledSlotType = getCompiledTypeFromResolvedType(slotType);
                iss << "p_" << slotRole.label() << ": " << compiledSlotType.label();
                slots.add(slotRole, kb.type.slot(slotRole, compiledSlotType));
            });
        }
        parametersType.set("slots", slots);
        subTypesMap.add(kb.id("parameters"), parametersType);
    }
    if (has("returnType")) {
        auto& asrReturnType      = returnType();
        auto& compiledReturnType = getCompiledTypeFromResolvedType(asrReturnType);
        oss << compiledReturnType.label();
        subTypesMap.add(kb.id("returnType"), compiledReturnType);
    }
    if (has("returnType")) {
        function.label(std::format("fn {}{}({}) -> {}", structTypeStr, get("name").label(), iss.str(), oss.str()));
    } else {
        function.label(std::format("fn {}{}({})", structTypeStr, get("name").label(), iss.str()));
    }
}

CellI& Ast::Function::compileAst(CellI& ast, cells::Object& function, CellI& state)
{
    auto compile = [this, &function, &state](CellI& ast) -> CellI& { return compileAst(ast, function, state); };
    const auto _ = [this](auto& cell) -> Ast::Cell& { return kb._(cell); };

    if (&ast.type() == &kb.type.ast.Block) {
        CellI& list        = ast[kb.ids.asts];
        auto& compiledAsts = *new cells::List(kb, kb.type.op.Base);
        Visitor::visitList(list, [this, &compile, &compiledAsts, &ast, &function](CellI& ast, int, bool&) {
            compiledAsts.add(compile(ast));
        });
        Object& opBlock = *new Object(kb, kb.type.op.Block);
        opBlock.set("ast", ast);
        opBlock.set("ops", compiledAsts);
        return opBlock;
    } else if (&ast.type() == &kb.type.ast.Cell) {
        Object& constVar = *new Object(kb, kb.type.op.ConstVar);
        constVar.set("ast", ast);
        constVar.set("value", ast[kb.ids.value]);
        return constVar;
    } else if (&ast.type() == &kb.type.ast.ResolvedType) {
        Object& constVar = *new Object(kb, kb.type.op.ConstVar);
        constVar.set("ast", ast);
        constVar.set("value", ast[kb.ids.compiled]);
        return constVar;
    } else if (&ast.type() == &kb.type.ast.SelfFn) {
        Object& constVar = *new Object(kb, kb.type.op.ConstVar);
        constVar.set("ast", ast);
        constVar.set("value", function);
        return constVar;
    } else if (&ast.type() == &kb.type.ast.Self) {
        CellI& retOp = compile(kb.ast.get(_(function), _("stack")) / "value" / "input" / "self");
        retOp.set("ast", ast);
        retOp.label("self");
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Parameter) {
        CellI& retOp = compile(kb.ast.get(_(function), _("stack")) / "value" / "input" / _(ast[kb.ids.role]));
        retOp.set("ast", ast);
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Member) {
        CellI& member = compile(kb.ast.get(kb.ast.self(), kb.ast.cell(ast[kb.ids.role])));
        member.set("ast", ast);
        return member;
    } else if (&ast.type() == &kb.type.ast.Return) {
        Object& retOp = *new Object(kb, kb.type.op.Return, "op.return");
        retOp.set("ast", ast);
        if (ast.has("value")) {
            retOp.set("result", compile(kb.ast.set(kb.ast.get(_(function), _("stack")) / "value" / "output", _("value"), static_cast<Ast::Base&>(ast[kb.ids.value]))));
        }
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Var) {
        if (function.type()[kb.ids.subTypes][kb.ids.index].missing("localVars")) {
            cells::Object& functionLocalVarsType = *new cells::Object(kb, kb.type.Type_, std::format("LocalVarsType of {}", function.label()));
            functionLocalVarsType.set("memberOf", kb.map(kb.type.Type_, kb.type.Type_, kb.type.Index, kb.type.Index));
            static_cast<Map&>(function.type()[kb.ids.subTypes]).add(kb.id("localVars"), functionLocalVarsType);
        }
        CellI& localVarsType = function.type()[kb.ids.subTypes][kb.ids.index][kb.ids.localVars][kb.ids.value];
        if (localVarsType.missing("slots")) {
            localVarsType.set("slots", *new Map(kb, kb.type.Cell, kb.type.Slot));
        }
        auto& slotsMap = static_cast<Map&>(localVarsType[kb.ids.slots]);
        if (!slotsMap.hasKey(ast[kb.ids.role])) {
            slotsMap.add(ast[kb.ids.role], kb.type.slot(ast[kb.ids.role], kb.type.op.Var));
        }
        CellI& retOp = compile(kb.ast.get(_(function), _("stack")) / "value" / "localVars" / _(ast[kb.ids.role]));
        retOp.set("ast", ast);
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Delete) {
        Object& retOp = *new Object(kb, kb.type.op.Delete);
        retOp.set("ast", ast);
        retOp.set("input", compile(ast[kb.ids.cell]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Set) {
        Object& retOp = *new Object(kb, kb.type.op.Set);
        retOp.set("ast", ast);
        retOp.set("cell", compile(ast[kb.ids.cell]));
        retOp.set("role", compile(ast[kb.ids.role]));
        retOp.set("value", compile(ast[kb.ids.value]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Erase) {
        Object& retOp = *new Object(kb, kb.type.op.Erase);
        retOp.set("ast", ast);
        retOp.set("cell", compile(ast[kb.ids.cell]));
        retOp.set("role", compile(ast[kb.ids.role]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.If) {
        Object& retOp = *new Object(kb, kb.type.op.If);
        retOp.set("ast", ast);
        retOp.set("condition", compile(ast[kb.ids.condition]));
        retOp.set("then", compile(ast[kb.ids.then]));
        if (ast.has("else_")) {
            retOp.set("else_", compile(ast[kb.ids.else_]));
            return retOp;
        } else {
            return retOp;
        }
    } else if (&ast.type() == &kb.type.ast.Do) {
        Object& retOp = *new Object(kb, kb.type.op.Do);
        retOp.set("ast", ast);
        retOp.set("condition", compile(ast[kb.ids.condition]));
        retOp.set("statement", compile(ast[kb.ids.statement]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.While) {
        Object& retOp = *new Object(kb, kb.type.op.While);
        retOp.set("ast", ast);
        retOp.set("condition", compile(ast[kb.ids.condition]));
        retOp.set("statement", compile(ast[kb.ids.statement]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.New) {
        auto& compiledAsts = *new cells::List(kb, kb.type.op.Base);
        auto& astObjectType = ast[kb.ids.objectType];

        Object& block = *new Object(kb, kb.type.op.Block);
        block.set("ast", ast);
        block.set("ops", compiledAsts);

        Object& opSet = *new Object(kb, kb.type.op.Set, "New { block.value = new objectType(); }");
        opSet.set("ast", ast);
        opSet.set("cell", compile(kb.ast.cell(block)));
        opSet.set("role", compile(kb.ast.cell(kb.id("value"))));

        Object& opNew = *new Object(kb, kb.type.op.New);
        opNew.set("ast", ast);
        opNew.set("objectType", compile(astObjectType));
        opSet.set("value", opNew);

        compiledAsts.add(opSet);
        if (ast.has("constructor")) {
            Object& callAst = *new Object(kb, kb.type.ast.Call);
            callAst.set("cell", kb.ast.get(kb.ast.cell(block), kb.ast.cell(kb.id("value"))));
            callAst.set("method", ast[kb.ids.constructor]);
            if (ast.has("parameters")) {
                callAst.set("parameters", ast[kb.ids.parameters]);
            }
            CellI& callConstructor = compile(callAst);
            callConstructor.label("New { call constructor; }");
            compiledAsts.add(callConstructor);
        }
        return block;
    } else if (&ast.type() == &kb.type.ast.Call || &ast.type() == &kb.type.ast.StaticCall) {
/*
block {
    var_method.value  = m_list.type.methods.index.add;
    var_method.value.stack = fn Map::add(self: Map, in_key: Cell, in_value: Cell).stack;
    var_newStackItem.value = new ListItem;
    var_newStackFrame.value = new StackFrame;
    var_newStackItem.value.value = var_newStackFrame.value;
    var_newStackItem.value.previous = var_method.value.stack;
    var_inputIndex.value = new Index;
    if var_method.value.type.subTypes.index has localVars then {
        var_localVars.value = new var_method.value.type.subTypes.index.localVars;
        var_newStackFrame.value.localVars = var_localVars.value;
        var_localVarsListItem.value = var_method.value.type.subTypes.index.localVars.slots.list.first;
        do {
            var_localVarsListItemHasNext.value = true;
            var_localVars.value.var_localVarsListItem.value.value.slotRole = new op::Var;
            if var_localVarsListItem.value has next then
                var_localVarsListItem.value = var_localVarsListItem.value.next;
             else
                var_localVarsListItemHasNext.value = false;
        } (var_localVarsListItemHasNext.value is true);
    };
    var_newStackFrame.value.input = var_inputIndex.value;
    if var_method.value.type.subTypes.index.output has slots then
        if var_method.value.type.subTypes.index.output.slots.index has value then {
        var_newStackFrame.value.output = new var_method.value.type.subTypes.index.output;
        var_newStackFrame.value.output.value = new op::Var;
    };
    var_inputIndex.value.self = m_list;
    var_inputIndex.value.value = in_value;
    var_method.value.stack = var_newStackItem.value;
    eval(var_method);
    if var_method.value.type.subTypes.index.output has slots then
        if var_method.value.type.subTypes.index.output.slots.index has value then
            block.value = var_method.value.stack.value.output.value.value;
    var_method.value.stack = var_method.value.stack.previous;
 }
*/
        const char* blockName  = &ast.type() == &kb.type.ast.Call ? "Call { ... }" : "SCall { ... }";
        Ast::Base& astCell     = static_cast<Ast::Base&>(ast[kb.ids.cell]);
        Ast::Base& astMethod   = static_cast<Ast::Base&>(ast[kb.ids.method]);
        auto& astMethodId      = astMethod[kb.ids.value];
#if 1
        bool checked = false;
        // New<T>.constructor(...)
        if (&astCell.type() == &kb.type.ast.Get && &astCell[kb.ids.cell].type() == &kb.type.ast.Cell && &astCell[kb.ids.cell][kb.ids.value].type() == &kb.type.op.Block) {
            auto& opBlock = astCell[kb.ids.cell][kb.ids.value];
            auto& opBlockAst = opBlock[kb.ids.ast];
            if (&opBlockAst.type() == &kb.type.ast.New) {
                auto& astObjectTypeStruct = opBlockAst[kb.ids.objectType];
                checkMethodCall(astObjectTypeStruct, astMethodId, state);
                checked = true;
            }
        // var_xxx.call(...)
        } else if (&astCell.type() == &kb.type.ast.Get && &astCell[kb.ids.cell].type() == &kb.type.ast.Var) {
            auto& astVar = astCell[kb.ids.cell];
            std::cout << "In " << astVar.label() << " ";
        // m_xxx.method(...)
        } else if (&astCell.type() == &kb.type.ast.Member) {
            auto& astMemberId = astCell[kb.ids.role];
            auto& astMembersType = function[kb.ids.ast][kb.ids.structType];
            if (&astMemberId == &kb.ids.type) {
                // std::cout << "DDDD " << astMembersType.label();
                auto& stdScope = static_cast<Scope&>(state[kb.ids.globalScope]).getScope("std");
                auto& type     = stdScope.getStruct("Type");
                checkMethodCall(type, astMethodId, state);
                checked        = true;
            } else {
                auto& astMembers = static_cast<Map&>(function[kb.ids.ast][kb.ids.structType][kb.ids.members]);
                if (astMembers.hasKey(astMemberId)) {
                    Slot& slot = static_cast<Slot&>(astMembers.getValue(astMemberId));
                    auto& slotType = slot[kb.ids.slotType];
                    checkMethodCall(slotType, astMethodId, state);
                    checked = true;
                } else {
                    throw "Unknown member name!";
                }
            }
        } else if (&astCell.type() == &kb.type.ast.Self) {
            auto& astSelfType = function[kb.ids.ast][kb.ids.structType];
            checkMethodCall(astSelfType, astMethodId, state);
            checked = true;
        } else if (&astCell.type() == &kb.type.ast.Parameter) {
            auto& parameterRole         = astCell[kb.ids.role];
            auto& astFunctionParameters = function[kb.ids.ast][kb.ids.parameters];
            Slot* astFunctionParameterSlot = nullptr;
            Visitor::visitList(astFunctionParameters, [this, &parameterRole, &astFunctionParameterSlot](CellI& slot, int i, bool& stop) {
                CellI& slotRole = slot[kb.ids.slotRole];
                CellI& slotType = slot[kb.ids.slotType];
                if (&slotRole == &parameterRole) {
                    astFunctionParameterSlot = &static_cast<Slot&>(slot);
                    stop = true;
                    return;
                }
            });
            if (astFunctionParameterSlot) {
                Slot& slot             = *astFunctionParameterSlot;
                auto& astParameterType = slot[kb.ids.slotType];
                checkMethodCall(astParameterType, astMethodId, state);
                checked = true;
            } else {
                throw "Unknown parameter name!";
            }
        }
        if (!checked) {
            std::cout << "Unchecked method call " << astMethodId.label() << " in " << function.label() << std::endl;
        }
#endif
        Ast::Get* getMethodPtr = nullptr;
        if (&ast.type() == &kb.type.ast.Call) {
            getMethodPtr = &(kb.ast.get(astCell, _("type")) / "methods");
        } else {
            getMethodPtr = &(kb.ast.get(astCell, _("methods")));
        }
        Ast::Get& getMethod = (*getMethodPtr) / "index" / astMethod / "value";
        auto& compiledAsts  = *new cells::List(kb, kb.type.op.Base);
        Object& block       = *new Object(kb, kb.type.op.Block, blockName);
        block.set("ast", ast);
        block.set("ops", compiledAsts);
        Object& varMethod = *new Object(kb, kb.type.op.Var, "Call { var_method; }");
        varMethod.set("valueType", kb.type.op.Function);
        CellI& storeMethod     = compile(kb.ast.set(_(varMethod), _("value"), getMethod));
        CellI& setCurrentStack = compile(kb.ast.set(_(varMethod) / "value", _("stack"), kb.ast.get(_(function), _("stack"))));

        Object& varNewStackItem = *new Object(kb, kb.type.op.Var, "Call { var_newStackItem; }");
        varNewStackItem.set("valueType", kb.type.ListItem);
        Object& varNewStackFrame = *new Object(kb, kb.type.op.Var, "Call { var_newStackFrame; }");
        varNewStackFrame.set("valueType", kb.type.StackFrame);

        Object& varInputIndex = *new Object(kb, kb.type.Index, "Call { var_inputIndex; }");
        Object& varLocalVars = *new Object(kb, kb.type.op.Var, "Call { var_localVars; }");
        varLocalVars.set("valueType", kb.type.Index);
        Object& varLocalVarsList = *new Object(kb, kb.type.op.Var, "Call { var_localVarsList; }");
        varLocalVarsList.set("valueType", kb.type.List);
        Object& varLocalVarsListItem = *new Object(kb, kb.type.op.Var, "Call { var_localVarsListItem; }");
        varLocalVarsListItem.set("valueType", kb.type.ListItem);
        Object& varLocalVarsListItemHasNext = *new Object(kb, kb.type.op.Var, "Call { var_localVarsListItemHasNext; }");
        varLocalVarsListItemHasNext.set("valueType", kb.type.Boolean);

        CellI& storeStackItem     = compile(kb.ast.set(_(varNewStackItem), _("value"), kb.ast.new_(_(kb.type.ListItem))));
        CellI& storeStackFrame    = compile(kb.ast.set(_(varNewStackFrame), _("value"), kb.ast.new_(_(kb.type.StackFrame))));
        CellI& setListItem        = compile(kb.ast.set(_(varNewStackItem) / "value", _("value"), _(varNewStackFrame) / "value"));
        CellI& setListItemPrev    = compile(kb.ast.set(_(varNewStackItem) / "value", _("previous"), _(varMethod) / "value" / "stack"));
        CellI& setListItemNext    = compile(kb.ast.set(_(varMethod) / "value" / "stack", _("next"), _(varNewStackItem) / "value"));
        CellI& storeInputIndex    = compile(kb.ast.set(_(varInputIndex), _("value"), kb.ast.new_(_(kb.type.Index))));
        CellI& setMethod          = compile(kb.ast.set(_(varNewStackFrame) / "value", _("method"), _(varMethod) / "value"));

        CellI& createNewLocalVars = compile(
            kb.ast.if_(kb.ast.has(_(varMethod) / "value" / "type" / "subTypes" / "index", _("localVars")),
                       kb.ast.block(
                           kb.ast.set(_(varLocalVars), _("value"), kb.ast.new_(_(varMethod) / "value" / "type" / "subTypes" / "index" / "localVars" / "value")),
                           kb.ast.set(_(varNewStackFrame) / "value", _("localVars"), _(varLocalVars) / "value"),
                           kb.ast.set(_(varLocalVarsListItem), _("value"), _(varMethod) / "value" / "type" / "subTypes" / "index" / "localVars" / "value" / "slots" / "list" / "first"),
                           kb.ast.do_(kb.ast.block(
                                          kb.ast.set(_(varLocalVarsListItemHasNext), _("value"), _(kb.boolean.true_)),
                                          kb.ast.set(_(varLocalVars) / "value", _(varLocalVarsListItem) / "value" / "value" / "slotRole", kb.ast.new_(_(kb.type.op.Var))),
                                          kb.ast.if_(kb.ast.has(_(varLocalVarsListItem) / _("value"), _("next")),
                                                     kb.ast.set(_(varLocalVarsListItem), _("value"), _(varLocalVarsListItem) / "value" / "next"),
                                                     kb.ast.set(_(varLocalVarsListItemHasNext), _("value"), _(kb.boolean.false_)))),
                                      kb.ast.same(_(varLocalVarsListItemHasNext) / "value", _(kb.boolean.true_))))));

        CellI& setInput      = compile(kb.ast.set(_(varNewStackFrame) / "value", _("input"), _(varInputIndex) / "value"));
        CellI& setSelf       = compile(kb.ast.set(_(varInputIndex) / "value", _("self"), astCell));
        CellI& setStackToNew = compile(kb.ast.set(_(varMethod) / "value", _("stack"), _(varNewStackItem) / "value"));
        CellI& setRetValue   = compile(kb.ast.if_(kb.ast.has(_(varMethod) / "value" / "type" / "subTypes" / "index", _("returnType")),
                                                  kb.ast.block(kb.ast.set(_(varNewStackFrame) / "value", _("output"), kb.ast.new_(_(kb.type.op.Var))),
                                                               kb.ast.set(_(varNewStackFrame) / "value" / "output", _("valueType"), _(varMethod) / "value" / "type" / "subTypes" / "index" / "returnType" / "value"))));
        CellI& getResult     = compile(kb.ast.if_(kb.ast.has(_(varMethod) / _("value") / _("type") / _("subTypes") / _("index"), _("returnType")),
                                                  kb.ast.set(_(block), _("value"), _(varMethod) / "value" / "stack" / "value" / "output" / "value")));
        CellI& setStackToOld = compile(kb.ast.set(_(varMethod) / "value" / "stack" / "previous" / "value" / "method", _("stack"), _(varMethod) / "value" / "stack" / "previous"));

        compiledAsts.add(storeMethod);
        compiledAsts.add(setCurrentStack);
        compiledAsts.add(storeStackItem);
        compiledAsts.add(storeStackFrame);
        compiledAsts.add(setListItem);
        compiledAsts.add(setListItemPrev);
        compiledAsts.add(setListItemNext);
        compiledAsts.add(storeInputIndex);
        compiledAsts.add(createNewLocalVars);
        compiledAsts.add(setMethod);
        compiledAsts.add(setInput);
        compiledAsts.add(setRetValue);
        compiledAsts.add(setSelf);


        if (ast.has("parameters")) {
            Visitor::visitList(ast[kb.ids.parameters], [this, &compiledAsts, &compile, &varInputIndex, &_](CellI& param, int, bool&) {
                Ast::Base& paramRole  = static_cast<Ast::Base&>(param[kb.ids.slotRole]);
                Ast::Base& paramValue = static_cast<Ast::Base&>(param[kb.ids.slotType]);
                CellI& setParam       = compile(kb.ast.set(_(varInputIndex) / "value", _(paramRole), paramValue));
                setParam.label("Call { setParam; }");
                compiledAsts.add(setParam);
            });
        }
        CellI& evalMethod = *new Object(kb, kb.type.op.EvalVar, std::format("{}::Call {{ evalVar; }}", function.label()));
        evalMethod.set("value", varMethod);
        compiledAsts.add(setStackToNew);
        compiledAsts.add(evalMethod);
        compiledAsts.add(getResult);
        compiledAsts.add(setStackToOld);

        getMethod.label("Call { getMethod; }");
        storeMethod.label("Call { storeMethod; }");
        setCurrentStack.label("Call  { setCurrentStack; }");
        storeStackItem.label("Call { storeStackFrame; }");
        storeStackFrame.label("Call { storeStackFrame; }");
        setListItem.label("Call { setListItem; }");
        setListItemPrev.label("Call { setListItemPrev; }");
        setListItemPrev.label("Call { setListItemNext; }");
        storeInputIndex.label("Call { storeInputIndex; }");
        createNewLocalVars.label("Call { createNewLocalVars; }");
        setMethod.label("Call { setMethod; }");
        setInput.label("Call { setInput; }");
        setRetValue.label("Call { setRetValue; }");
        setSelf.label("Call { setSelf; }");
        setStackToNew.label("Call { setStackToNew; }");
        getResult.label("Call { getResult; }");
        setStackToOld.label("Call { setStackToOld; }");

        return block;
    } else if (&ast.type() == &kb.type.ast.And) {
        Object& retOp = *new Object(kb, kb.type.op.And);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast[kb.ids.lhs]));
        retOp.set("rhs", compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Or) {
        Object& retOp = *new Object(kb, kb.type.op.Or);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast[kb.ids.lhs]));
        retOp.set("rhs", compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Not) {
        Object& retOp = *new Object(kb, kb.type.op.Not);
        retOp.set("ast", ast);
        retOp.set("input", compile(ast[kb.ids.input]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Add) {
        Object& retOp = *new Object(kb, kb.type.op.Add);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast[kb.ids.lhs]));
        retOp.set("rhs", compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Subtract) {
        Object& retOp = *new Object(kb, kb.type.op.Subtract);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast[kb.ids.lhs]));
        retOp.set("rhs", compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Multiply) {
        Object& retOp = *new Object(kb, kb.type.op.Multiply);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast[kb.ids.lhs]));
        retOp.set("rhs", compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Divide) {
        Object& retOp = *new Object(kb, kb.type.op.Divide);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast[kb.ids.lhs]));
        retOp.set("rhs", compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.LessThan) {
        Object& retOp = *new Object(kb, kb.type.op.LessThan);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast[kb.ids.lhs]));
        retOp.set("rhs", compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.LessThanOrEqual) {
        Object& retOp = *new Object(kb, kb.type.op.LessThanOrEqual);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast[kb.ids.lhs]));
        retOp.set("rhs", compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.GreaterThan) {
        Object& retOp = *new Object(kb, kb.type.op.GreaterThan);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast[kb.ids.lhs]));
        retOp.set("rhs", compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.GreaterThanOrEqual) {
        Object& retOp = *new Object(kb, kb.type.op.GreaterThanOrEqual);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast[kb.ids.lhs]));
        retOp.set("rhs", compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Same) {
        Object& retOp = *new Object(kb, kb.type.op.Same);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast[kb.ids.lhs]));
        retOp.set("rhs", compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.NotSame) {
        Object& retOp = *new Object(kb, kb.type.op.NotSame);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast[kb.ids.lhs]));
        retOp.set("rhs", compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Equal) {
        Object& retOp = *new Object(kb, kb.type.op.Equal);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast[kb.ids.lhs]));
        retOp.set("rhs", compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.NotEqual) {
        Object& retOp = *new Object(kb, kb.type.op.NotEqual);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast[kb.ids.lhs]));
        retOp.set("rhs", compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Has) {
        Object& retOp = *new Object(kb, kb.type.op.Has);
        retOp.set("ast", ast);
        retOp.set("cell", compile(ast[kb.ids.cell]));
        retOp.set("role", compile(ast[kb.ids.role]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Missing) {
        Object& retOp = *new Object(kb, kb.type.op.Missing);
        retOp.set("ast", ast);
        retOp.set("cell", compile(ast[kb.ids.cell]));
        retOp.set("role", compile(ast[kb.ids.role]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Get) {
        Object& retOp = *new Object(kb, kb.type.op.Get);
        retOp.set("ast", ast);
        retOp.set("cell", compile(ast[kb.ids.cell]));
        retOp.set("role", compile(ast[kb.ids.role]));
        return retOp;
    }

    throw "Unknown function AST!";
}

void Ast::Function::checkMethodCall(CellI& astType, CellI& astMethodId, CellI& state)
{
    CellI* typePtr = nullptr;
    Map* methodsPtr = nullptr;
    if (astType.isA(kb.type.ast.Cell)) {
        auto& cellValue = astType[kb.ids.value];
        typePtr         = &cellValue;
        methodsPtr      = &static_cast<Map&>(cellValue[kb.ids.methods]);
    } else if (astType.isA(kb.type.ast.Struct)) {
        typePtr    = &astType;
        methodsPtr = &static_cast<Map&>(astType[kb.ids.methods]);
    } else if (astType.isA(kb.type.ast.ResolvedType)) {
        typePtr    = &astType[kb.ids.compiled];
        methodsPtr = &static_cast<Map&>(astType[kb.ids.ast][kb.ids.methods]);
    } else {
        throw "Unexpected AST type";
    }
    CellI& type = *typePtr;
    if (&type.type() != &kb.type.ast.Struct && &type.type() != &kb.type.Type_ && type.label() != "Type") {
        throw "Resolved type must be a type!";
    }
    auto& methods = *methodsPtr;
    if (!methods.hasKey(astMethodId)) {
        std::cerr << std::format("Method '{}' doesn't exist in type {}", astMethodId.label(), type.label()) << std::endl;
        throw "Method doesn't exist in type!";
    }
}

List& Ast::Function::parameters()
{
    if (missing("parameters")) {
        throw "No parameters!";
    } else {
        return static_cast<List&>(get("parameters"));
    }
}

CellI& Ast::Function::returnType()
{
    if (missing("returnType")) {
        throw "No returnType!";
    } else {
        return get("returnType");
    }
}

Ast::Base& Ast::Function::code()
{
    if (missing("code")) {
        throw "No code!";
    } else {
        return static_cast<Ast::Base&>(get("code"));
    }
}

Ast::FunctionT::FunctionT(brain::Brain& kb, CellI& name, const std::string& label) :
    BaseT<FunctionT>(kb, kb.type.ast.FunctionT, label)
{
    set("name", name);
    this->label(label);
}

Ast::FunctionT::FunctionT(brain::Brain& kb, const std::string& name) :
    BaseT<FunctionT>(kb, kb.type.ast.FunctionT, name)
{
    set("id", kb.pools.strings.get(name)[kb.ids.value]);
    label(name);
}

void Ast::FunctionT::parameters(Slot& param)
{
    if (missing("parameters")) {
        set("parameters", *new List(kb, kb.type.Slot));
    }
    parameters().add(param);
}

void Ast::FunctionT::returnType(CellI& type)
{
    set("returnType", type);
}

void Ast::FunctionT::addBlock(Block& block)
{
    set("code", block);
}

List& Ast::FunctionT::parameters()
{
    if (missing("parameters")) {
        throw "No parameters!";
    } else {
        return static_cast<List&>(get("parameters"));
    }
}

CellI& Ast::FunctionT::returnType()
{
    if (missing("returnType")) {
        throw "No returnType!";
    } else {
        return get("returnType");
    }
}

Ast::Base& Ast::FunctionT::code()
{
    if (missing("code")) {
        throw "No code!";
    } else {
        return static_cast<Ast::Base&>(get("code"));
    }
}

Ast::Delete::Delete(brain::Brain& kb, Base& cell) :
    BaseT<Delete>(kb, kb.type.ast.Delete, "ast.delete")
{
    set("cell", cell);
}

Ast::Set::Set(brain::Brain& kb, Base& cell, Base& role, Base& value) :
    BaseT<Set>(kb, kb.type.ast.Set, "ast.set")
{
    set("cell", cell);
    set("role", role);
    set("value", value);
}

Ast::Erase::Erase(brain::Brain& kb, Base& cell, Base& role) :
    BaseT<Erase>(kb, kb.type.ast.Erase, "ast.erase")
{
    set("cell", cell);
    set("role", role);
}

Ast::If::If(brain::Brain& kb, Base& condition, Base& thenBranch) :
    BaseT<If>(kb, kb.type.ast.If, "ast.if")
{
    set("condition", condition);
    set("then", thenBranch);
}

Ast::If::If(brain::Brain& kb, Base& condition, Base& thenBranch, Base& elseBranch) :
    BaseT<If>(kb, kb.type.ast.If, "ast.ifElse")
{
    set("condition", condition);
    set("then", thenBranch);
    set("else_", elseBranch);
}

Ast::Do::Do(brain::Brain& kb, Base& statement, Base& condition) :
    BaseT<Do>(kb, kb.type.ast.Do, "ast.do")
{
    set("condition", condition);
    set("statement", statement);
}

Ast::While::While(brain::Brain& kb, Base& condition, Base& statement) :
    BaseT<While>(kb, kb.type.ast.While, "ast.while")
{
    set("condition", condition);
    set("statement", statement);
}

Ast::Var::Var(brain::Brain& kb, CellI& role) :
    BaseT<Var>(kb, kb.type.ast.Var, role.label())
{
    set("role", role);
}

Ast::Set& Ast::Var::operator=(Base& value)
{
    return Set::New(kb, *this, Cell::New(kb, kb.ids.value), value);
}

Ast::Get& Ast::Var::operator*()
{
    return Get::New(kb, *this, Cell::New(kb, kb.ids.value));
}

Ast::Member::Member(brain::Brain& kb, CellI& role) :
    BaseT<Member>(kb, kb.type.ast.Member, "ast.member")
{
    set("role", role);
}

Ast::Set& Ast::Member::operator=(Base& value)
{
    Ast::Set& ret = Set::New(kb, Self::New(kb), Cell::New(kb, get("role")), value);
    return ret;
}

Ast::Get& Ast::Member::operator/(Base& role)
{
    return Get::New(kb, *this, role);
}

Ast::Get& Ast::Member::operator/(const std::string& role)
{
    return Get::New(kb, *this, kb._(role));
}

Ast::Has& Ast::Member::exist()
{
    return Has::New(kb, Self::New(kb), Cell::New(kb, get("role")));
}

Ast::Missing& Ast::Member::missing()
{
    return Missing::New(kb, Self::New(kb), Cell::New(kb, get("role")));
}

Ast::Call& Ast::Member::call(const std::string& method) {
    return kb.ast.call(*this, method);
}

Ast::SubType::SubType(brain::Brain& kb, CellI& name) :
    BaseT<SubType>(kb, kb.type.ast.SubTypeName, "ast.subTypeName")
{
    set("name", name);
}

Ast::TemplatedType::TemplatedType(brain::Brain& kb, CellI& id, CellI& typeList) :
    BaseT<TemplatedType>(kb, kb.type.ast.TemplatedType, "ast.templatedType")
{
    set("id", id);
    set("parameters", typeList);
}

void Ast::TemplatedType::addParam(const std::string& role, CellI& type)
{
    List& paramList = static_cast<List&>(get("parameters"));
    paramList.add(kb.ast.slot(role, type));
}

void Ast::TemplatedType::addParam(const std::string& role, const std::string& type)
{
    addParam(role, kb.ast.structName(type));
}

Ast::TemplateParam::TemplateParam(brain::Brain& kb, CellI& role) :
    BaseT<TemplateParam>(kb, kb.type.ast.TemplateParam, "ast.templateParam")
{
    set("role", role);
}

Ast::New::New(brain::Brain& kb, Base& objectType) :
    BaseT<New>(kb, kb.type.ast.New, "ast.new")
{
    set("objectType", objectType);
}

Ast::New::New(brain::Brain& kb, Base& objectType, Base& constructor) :
    BaseT<New>(kb, kb.type.ast.New, "ast.new()")
{
    set("objectType", objectType);
    set("constructor", constructor);
}

void Ast::New::addParam(Slot& slot)
{
    if (missing("parameters")) {
        set("parameters", kb.list(slot));
    } else {
        List& paramList = static_cast<List&>(get("parameters"));
        paramList.add(slot);
    }
}

Ast::Same::Same(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Same>(kb, kb.type.ast.Same, "ast.same")
{
    set("lhs", lhs);
    set("rhs", rhs);
}

Ast::NotSame::NotSame(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<NotSame>(kb, kb.type.ast.NotSame, "ast.notSame")
{
    set("lhs", lhs);
    set("rhs", rhs);
}

Ast::Equal::Equal(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Equal>(kb, kb.type.ast.Equal, "ast.equal")
{
    set("lhs", lhs);
    set("rhs", rhs);
}

Ast::NotEqual::NotEqual(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<NotEqual>(kb, kb.type.ast.NotEqual, "ast.notEqual")
{
    set("lhs", lhs);
    set("rhs", rhs);
}

Ast::Has::Has(brain::Brain& kb, Base& cell, Base& role) :
    BaseT<Has>(kb, kb.type.ast.Has, "ast.has")
{
    set("cell", cell);
    set("role", role);
}

Ast::Missing::Missing(brain::Brain& kb, Base& cell, Base& role) :
    BaseT<Missing>(kb, kb.type.ast.Missing, "ast.missing")
{
    set("cell", cell);
    set("role", role);
}

Ast::Get::Get(brain::Brain& kb, Base& cell, Base& role) :
    BaseT<Get>(kb, kb.type.ast.Get, "ast.get")
{
    set("cell", cell);
    set("role", role);
}

Ast::Get& Ast::Get::operator/(Base& role)
{
    return Get::New(kb, *this, role);
}
Ast::Get& Ast::Get::operator/(const std::string& role)
{
    return Get::New(kb, *this, kb._(role));
}

Ast::And::And(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<And>(kb, kb.type.ast.And, "ast.and")
{
    set("lhs", lhs);
    set("rhs", rhs);
}

Ast::Or::Or(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Or>(kb, kb.type.ast.Or, "ast.or")
{
    set("lhs", lhs);
    set("rhs", rhs);
}

Ast::Not::Not(brain::Brain& kb, Base& input) :
    BaseT<Not>(kb, kb.type.ast.Not, "ast.not")
{
    set("input", input);
}

Ast::Add::Add(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Add>(kb, kb.type.ast.Add, "ast.add")
{
    set("lhs", lhs);
    set("rhs", rhs);
}

Ast::Subtract::Subtract(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Subtract>(kb, kb.type.ast.Subtract, "ast.subtract")
{
    set("lhs", lhs);
    set("rhs", rhs);
}

Ast::Multiply::Multiply(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Multiply>(kb, kb.type.ast.Multiply, "ast.multiply")
{
    set("lhs", lhs);
    set("rhs", rhs);
}

Ast::Divide::Divide(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Divide>(kb, kb.type.ast.Divide, "ast.divide")
{
    set("lhs", lhs);
    set("rhs", rhs);
}

Ast::LessThan::LessThan(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<LessThan>(kb, kb.type.ast.LessThan, "ast.lessThan")
{
    set("lhs", lhs);
    set("rhs", rhs);
}

Ast::LessThanOrEqual::LessThanOrEqual(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<LessThanOrEqual>(kb, kb.type.ast.LessThanOrEqual, "ast.lessThanOrEqual")
{
    set("lhs", lhs);
    set("rhs", rhs);
}

Ast::GreaterThan::GreaterThan(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<GreaterThan>(kb, kb.type.ast.GreaterThan, "ast.greaterThan")
{
    set("lhs", lhs);
    set("rhs", rhs);
}

Ast::GreaterThanOrEqual::GreaterThanOrEqual(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<GreaterThanOrEqual>(kb, kb.type.ast.GreaterThanOrEqual, "ast.greaterThanOrEqual")
{
    set("lhs", lhs);
    set("rhs", rhs);
}

Ast::Ast(brain::Brain& kb) :
    kb(kb)
{
}

Ast::Cell& Ast::cell(CellI& cell)
{
    return Cell::New(kb, cell);
}

Ast::StructName& Ast::structName(CellI& id)
{
    return StructName::New(kb, id);
}

Ast::StructName& Ast::structName(const std::string& idStr)
{
    CellI& ret = processNamespacedName(idStr, [this](const std::string& outName) -> CellI& {
        return StructName::New(kb, kb.id(outName));
    });

    return static_cast<Ast::StructName&>(ret);
}

Ast::Self& Ast::self()
{
    return Self::New(kb);
}

Ast::SelfFn& Ast::selfFn()
{
    return SelfFn::New(kb);
}

Ast::Return& Ast::return_()
{
    return Return::New(kb);
}

Ast::Return& Ast::return_(Base& value)
{
    return Return::New(kb, value);
}

Ast::Parameter& Ast::parameter(CellI& role)
{
    auto& ast = kb.ast;
    return Parameter::New(kb, role);
}

Ast::Slot& Ast::slot(const std::string& role, CellI& type)
{
    return Slot::New(kb, kb.id(role), type);
}

Ast::Slot& Ast::slot(CellI& role, CellI& type)
{
    return Slot::New(kb, role, type);
}

Ast::Call& Ast::call(CellI& object, const std::string& method)
{
    return Call::New(kb, object, kb.ast.cell(kb.id(method)));
}

Ast::Call& Ast::call(CellI& cell, CellI& method)
{
    return Call::New(kb, cell, method);
}

Ast::StaticCall& Ast::scall(CellI& cell, CellI& method)
{
    return StaticCall::New(kb, cell, method);
}

Ast::StaticCall& Ast::scall(CellI& type, const std::string& method)
{
    return scall(type, kb.ast.cell(kb.id(method)));
}

Ast::Delete& Ast::delete_(Base& ast)
{
    return Delete::New(kb, ast);
}

Ast::Set& Ast::set(Base& cell, Base& role, Base& value)
{
    return Set::New(kb, cell, role, value);
}

Ast::Set& Ast::set(Base& cell, const std::string& role, Base& value)
{
    return Set::New(kb, cell, kb._(role), value);
}

Ast::Erase& Ast::erase(Base& cell, Base& role)
{
    return Erase::New(kb, cell, role);
}

Ast::If& Ast::if_(Base& condition, Base& thenBranch)
{
    return If::New(kb, condition, thenBranch);
}

Ast::If& Ast::if_(Base& condition, Base& thenBranch, Base& elseBranch)
{
    return If::New(kb, condition, thenBranch, elseBranch);
}

Ast::Do& Ast::do_(Base& statement, Base& condition)
{
    return Do::New(kb, statement, condition);
}

Ast::While& Ast::while_(Base& condition, Base& statement)
{
    return While::New(kb, condition, statement);
}

Ast::Var& Ast::var(CellI& role)
{
    return Var::New(kb, role);
}

Ast::Member& Ast::member(CellI& role)
{
    return Member::New(kb, role);
}

Ast::SubType& Ast::subType(CellI& role)
{
    return SubType::New(kb, role);
}

Ast::TemplatedType& Ast::templatedType(const std::string& idStr, CellI& type)
{
    CellI& ret = processNamespacedName(idStr, [this, &type](const std::string& outName)->CellI& {
        return TemplatedType::New(kb, kb.id(outName), kb.list(type));
    });

    return static_cast<Ast::TemplatedType&>(ret);
}

Ast::TemplateParam& Ast::templateParam(CellI& role)
{
    return TemplateParam::New(kb, role);
}

Ast::New& Ast::new_(Base& objectType)
{
    return New::NewT<Ast::New>::New(kb, objectType);
}

Ast::New& Ast::new_(Base& objectType, Base& constructor)
{
    return New::NewT<Ast::New>::New(kb, objectType, constructor);
}

Ast::Same& Ast::same(Base& lhs, Base& rhs)
{
    return Same::New(kb, lhs, rhs);
}

Ast::NotSame& Ast::notSame(Base& lhs, Base& rhs)
{
    return NotSame::New(kb, lhs, rhs);
}

Ast::Equal& Ast::equal(Base& lhs, Base& rhs)
{
    return Equal::New(kb, lhs, rhs);
}

Ast::NotEqual& Ast::notEqual(Base& lhs, Base& rhs)
{
    return NotEqual::New(kb, lhs, rhs);
}

Ast::Has& Ast::has(Base& cell, Base& role)
{
    return Has::New(kb, cell, role);
}

Ast::Has& Ast::has(Base& cell, const std::string& role)
{
    return Has::New(kb, cell, kb._(role));
}

Ast::Missing& Ast::missing(Base& cell, Base& role)
{
    return Missing::New(kb, cell, role);
}

Ast::Get& Ast::get(Base& cell, Base& role)
{
    return Get::New(kb, cell, role);
}

Ast::And& Ast::and_(Base& lhs, Base& rhs)
{
    return And::New(kb, lhs, rhs);
}

Ast::Or& Ast::or_(Base& lhs, Base& rhs)
{
    return Or::New(kb, lhs, rhs);
}

Ast::Not& Ast::not_(Base& input)
{
    return Not::New(kb, input);
}

Ast::Add& Ast::add(Base& lhs, Base& rhs)
{
    return Add::New(kb, lhs, rhs);
}

Ast::Subtract& Ast::subtract(Base& lhs, Base& rhs)
{
    return Subtract::New(kb, lhs, rhs);
}

Ast::Multiply& Ast::multiply(Base& lhs, Base& rhs)
{
    return Multiply::New(kb, lhs, rhs);
}

Ast::Divide& Ast::divide(Base& lhs, Base& rhs)
{
    return Divide::New(kb, lhs, rhs);
}

Ast::LessThan& Ast::lessThan(Base& lhs, Base& rhs)
{
    return LessThan::New(kb, lhs, rhs);
}

Ast::LessThanOrEqual& Ast::lessThanOrEqual(Base& lhs, Base& rhs)
{
    return LessThanOrEqual::New(kb, lhs, rhs);
}

Ast::GreaterThan& Ast::greaterThan(Base& lhs, Base& rhs)
{
    return GreaterThan::New(kb, lhs, rhs);
}

Ast::GreaterThanOrEqual& Ast::greaterThanOrEqual(Base& lhs, Base& rhs)
{
    return GreaterThanOrEqual::New(kb, lhs, rhs);
}

CellI& Ast::processNamespacedName(const std::string& inputName, std::function<CellI&(const std::string& outName)> createCb)
{
    std::vector<std::string> sliced;
    boost::algorithm::split_regex(sliced, inputName, boost::regex("::"));

    if (sliced.empty()) {
        throw "Invalid template ID!";
    }
    const auto& outName = sliced.back();
    auto& obj           = createCb(outName);
    if (sliced.size() > 1) {
        auto& namespaceList = *new List(kb, kb.type.Cell, "namespaces");
        obj.set("scopes", namespaceList);
        for (int i = 0; i < sliced.size() - 1; ++i) {
            const auto& currentId = sliced[i];
            namespaceList.add(kb.id(currentId));
        }
    }

    return obj;
}

Directions::Directions(brain::Brain& kb) :
    up(kb, kb.type.Directions, "up"),
    down(kb, kb.type.Directions, "down"),
    left(kb, kb.type.Directions, "left"),
    right(kb, kb.type.Directions, "right")
{
    kb.type.Directions.set("members", kb.list(up, down, left, right));
}

Coordinates::Coordinates(brain::Brain& kb) :
    x(kb, kb.type.Cell, "x"),
    y(kb, kb.type.Cell, "y")
{
}

Colors::Colors(brain::Brain& kb) :
    red(kb, kb.type.Cell, "red"),
    green(kb, kb.type.Cell, "green"),
    blue(kb, kb.type.Cell, "blue")
{
}

Boolean::Boolean(brain::Brain& kb) :
    true_(kb, kb.type.Boolean, "true"),
    false_(kb, kb.type.Boolean, "false")
{
}

Numbers::Numbers(brain::Brain& kb) :
    sign(kb, kb.type.Cell, kb.type.Cell, "sign"),
    positive(kb, kb.type.Cell, "positive"),
    negative(kb, kb.type.Cell, "negative")

{
    sign.add(positive, positive);
    sign.add(negative, negative);
}

namespace pools {

// ============================================================================
Chars::Chars(brain::Brain& kb) :
    kb(kb)
{
    // These are enough for me currently
    registerUnicodeBlock(0x020, 0x07e); // Basic Latin - without the DEL (0x7f) control character
    registerUnicodeBlock(0x080, 0x0ff); // Latin-1 Supplement
    registerUnicodeBlock(0x100, 0x17f); // Latin Extended-A
}

Object& Chars::get(char32_t utf32Char)
{
    auto unicodeCellIt = m_characters.find(utf32Char);
    if (unicodeCellIt != m_characters.end()) {
        return unicodeCellIt->second;
    } else {
        throw "Unknown char!";
    }
}

void Chars::registerUnicodeBlock(char32_t from, char32_t to)
{
    for (char32_t unicodeValue = from; unicodeValue <= to; ++unicodeValue) {
        std::string characterName;
        if (unicodeValue == 32) {
            characterName = "(space)";
        } else if (unicodeValue > 32 && unicodeValue < 127) {
            characterName = " ";
            characterName[0] = unicodeValue;
        } else {
            characterName = std::format("Unicode_{:#04x}", (int)unicodeValue);
        }
        auto it = m_characters.emplace(std::piecewise_construct,
                             std::forward_as_tuple(unicodeValue),
                             std::forward_as_tuple(kb, kb.type.Char, characterName));
        auto& value = it.first->second;
    }
}

// ============================================================================
Digits::Digits(brain::Brain& kb)
{
    m_digits.reserve(10);
    for (int i = 0; i < 10; ++i) {
        std::string digitName = "Digit_" + std::to_string(i);
        m_digits.emplace_back(kb, kb.type.Digit, digitName);
    }
}

Object& Digits::operator[](int digit)
{
    return m_digits[digit];
}

// ============================================================================
Numbers::Numbers(brain::Brain& kb) :
    m_kb(kb)
{
}

Number& Numbers::get(int number)
{
    auto numberIt = m_numbers.find(number);
    if (numberIt != m_numbers.end()) {
        return numberIt->second;
    } else {
        auto it           = m_numbers.emplace(std::piecewise_construct,
                                              std::forward_as_tuple(number),
                                              std::forward_as_tuple(m_kb, number));
        Number& newNumber = it.first->second;
        newNumber.label(std::to_string(number));
        return newNumber;
    }
}


struct StringInit
{
    StringInit(const std::string& str, List& list) :
        str(str), list(list) {}
    std::string str;
    List& list;
};

// ============================================================================
Strings::Strings(brain::Brain& kb) :
    kb(kb)
{
    StringInit reservedStrings[] = {
        { "argument", kb.ids.argument },
        { "ast", kb.ids.ast },
        { "asts", kb.ids.asts },
        { "cell", kb.ids.cell },
        { "children", kb.ids.children },
        { "code", kb.ids.code },
        { "color", kb.ids.color },
        { "compiled", kb.ids.compiled },
        { "condition", kb.ids.condition },
        { "constructor", kb.ids.constructor },
        { "continue_", kb.ids.continue_ },
        { "currentFn", kb.ids.currentFn },
        { "currentStruct", kb.ids.currentStruct },
        { "data", kb.ids.data },
        { "destructor", kb.ids.destructor },
        { "else_", kb.ids.else_ },
        { "emptyObject", kb.ids.emptyObject },
        { "first", kb.ids.first },
        { "functions", kb.ids.functions },
        { "globalScope", kb.ids.globalScope },
        { "height", kb.ids.height },
        { "id", kb.ids.id },
        { "index", kb.ids.index },
        { "input", kb.ids.input },
        { "instances", kb.ids.instances },
        { "item", kb.ids.item },
        { "itemType", kb.ids.itemType },
        { "key", kb.ids.key },
        { "keyType", kb.ids.keyType },
        { "last", kb.ids.last },
        { "lhs", kb.ids.lhs },
        { "list", kb.ids.list },
        { "listType", kb.ids.listType },
        { "localVars", kb.ids.localVars },
        { "memberOf", kb.ids.memberOf },
        { "members", kb.ids.members },
        { "method", kb.ids.method },
        { "methods", kb.ids.methods },
        { "name", kb.ids.name },
        { "next", kb.ids.next },
        { "objectType", kb.ids.objectType },
        { "op", kb.ids.op },
        { "ops", kb.ids.ops },
        { "output", kb.ids.output },
        { "parameters", kb.ids.parameters },
        { "parent", kb.ids.parent },
        { "pixels", kb.ids.pixels },
        { "previous", kb.ids.previous },
        { "resolvedScope", kb.ids.resolvedScope },
        { "result", kb.ids.result },
        { "returnType", kb.ids.returnType },
        { "rhs", kb.ids.rhs },
        { "role", kb.ids.role },
        { "rootNode", kb.ids.rootNode },
        { "scope", kb.ids.scope },
        { "scopes", kb.ids.scopes },
        { "self", kb.ids.self },
        { "size", kb.ids.size },
        { "slotRole", kb.ids.slotRole },
        { "slots", kb.ids.slots },
        { "slotType", kb.ids.slotType },
        { "stack", kb.ids.stack },
        { "statement", kb.ids.statement },
        { "static_", kb.ids.static_ },
        { "status", kb.ids.status },
        { "stop", kb.ids.stop },
        { "structs", kb.ids.structs },
        { "structType", kb.ids.structType },
        { "subTypes", kb.ids.subTypes },
        { "templateId", kb.ids.templateId },
        { "templateParams", kb.ids.templateParams },
        { "then", kb.ids.then },
        { "type", kb.ids.type },
        { "unknownInstances", kb.ids.unknownInstances },
        { "unknownStructs", kb.ids.unknownStructs },
        { "value", kb.ids.value },
        { "valueType", kb.ids.valueType },
        { "variables", kb.ids.variables },
        { "width", kb.ids.width }
    };
    for (auto& reservedString : reservedStrings) {
        auto& str = reservedString.str;
        m_strings.emplace(std::piecewise_construct,
                          std::forward_as_tuple(reservedString.str),
                          std::forward_as_tuple(kb, reservedString.list, reservedString.str));

    }
}

String& Strings::get(const std::string& str)
{
    auto numberIt = m_strings.find(str);
    if (numberIt != m_strings.end()) {
        return numberIt->second;
    } else {
        auto it           = m_strings.emplace(std::piecewise_construct,
                                              std::forward_as_tuple(str),
                                              std::forward_as_tuple(kb, str));
        String& newString = it.first->second;
        newString.label(str);
        return newString;
    }
}

List& Strings::getCharList(const std::string& str)
{
    auto& string = get(str);
    auto& ret = static_cast<List&>(string[string.kb.ids.value]);

    return ret;
}

} // namespace pools

Pools::Pools(brain::Brain& kb) :
    chars(kb),
    digits(kb),
    numbers(kb),
    strings(kb)
{
}

Arc::Arc(brain::Brain& kb) :
    Demonstration(kb, kb.type.Type_, "Demonstration"),
    Task(kb, kb.type.Type_, "Task"),
    examples(kb, kb.type.Cell, "examples")
{
    auto& type = kb.type;
    CellI* map = nullptr;

    map = &kb.map(kb.type.Cell, kb.type.Slot,
                  kb.ids.input, type.slot("input", kb.type.Picture),
                  kb.ids.output, type.slot("output", kb.type.Picture));
    Demonstration.set("slots", *map);

    map = &kb.map(kb.type.Cell, kb.type.Slot,
                  examples, type.slot(examples, kb.type.ListOf(Demonstration)),
                  kb.ids.input, type.slot("input", kb.type.Picture),
                  kb.ids.output, type.slot("output", kb.type.Picture));
    Task.set("slots", *map);
}

Test::Test(brain::Brain& kb) :
    factorial(kb, kb.type.Cell, "factorial")
{
}

Ast::Cell& Brain::_(CellI& cell)
{
    return ast.cell(cell);
}

Ast::Cell& Brain::_(const std::string& name)
{
    return ast.cell(id(name));
}

Ast::Parameter& Brain::p_(const std::string& name)
{
    return ast.parameter(id(name));
}

Ast::Member& Brain::m_(const std::string& name)
{
    return ast.member(id(name));
}

Ast::Var& Brain::var_(const std::string& name)
{
    return ast.var(id(name));
}

Ast::Slot& Brain::param(const std::string& name, CellI& value)
{
    return ast.slot(id(name), value);
}

Ast::Slot& Brain::member(const std::string& name, CellI& type)
{
    return ast.slot(id(name), type);
}

Ast::TemplateParam& Brain::tp_(const std::string& name)
{
    return ast.templateParam(id(name));
}

Ast::StructName& Brain::struct_(const std::string& name)
{
    return ast.structName(name);
}

void Brain::createStd()
{
    auto& stdScope = globalScope.addScope("std");

    /*
    Compiler steps:
    Resolve template related references in normal functions or structs:
      - where templated types is used, for example tt_("List", ids.valueType, _(type.Slot))
        it must be resolved to a StructName, with id L,i,s,t,ids.valueType,type.Slot
      - create a shadow ast tree with subtituted nodes
      - create a list of candidates for template instantiation with method names
        instantiate structT without methods
        instantiate structT with listed method
      - templates are instantied to a dedicated place
    */

#pragma region ListItem
    auto& listItemStructT
        = stdScope.addStructT("ListItem")
              .templateParams(
                  param("valueType", _(type.Type_)))
              .memberOf(
                  _(type.ListItem))
              .subTypes(
                  param("valueType", tp_("valueType")))
              .members(
                  member("previous", tt_("ListItem", "valueType", tp_("valueType"))),
                  member("next", tt_("ListItem", "valueType", tp_("valueType"))),
                  member("value", tp_("valueType")));

    listItemStructT.addMethod("constructor")
        .parameters(
            param("value", tp_("valueType")))
        .code(
            m_("value") = p_("value"));
#pragma endregion
#pragma region List
    auto& listStructT
        = stdScope.addStructT("List")
              .templateParams(
                  param("valueType", _(type.Type_)))
              .memberOf(
                  _(type.Container),
                  _(type.List))
              .subTypes(
                  param("itemType", tt_("ListItem", "valueType", tp_("valueType"))),
                  param("valueType", tp_("valueType")))
              .members(
                  member("first", st_("itemType")),
                  member("last", st_("itemType")),
                  member("size", _(type.Number)));

    listStructT.addMethod("constructor")
        .code(
            m_("size") = _(_0_));

    listStructT.addMethod("add")
        .parameters(
            param("value", tp_("valueType")))
        .returnType(st_("itemType"))
        .code(
            var_("item") = ast.new_(st_("itemType"), "constructor", param("value", p_("value"))),
            ast.if_(ast.not_(m_("first").exist()),
                    m_("first") = *var_("item"),                          // then
                    ast.block(ast.set(m_("last"), "next", *var_("item")), // else
                              ast.set(*var_("item"), "previous", m_("last")))),
            m_("last") = *var_("item"),
            m_("size") = ast.add(m_("size"), _(_1_)),
            ast.return_(*var_("item")));

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
            param("item", _(type.Cell)))
        .code(
            ast.if_(ast.has(p_("item"), "previous"),
                    ast.if_(ast.has(p_("item"), "next"),
                            ast.set(p_("item") / "previous", "next", p_("item") / "next"),
                            ast.erase(p_("item") / "previous", _("next"))),
                    ast.if_(ast.has(p_("item"), "next"),
                            m_("first") = p_("item") / "next",
                            ast.erase(ast.self(), _("first")))),
            ast.if_(ast.has(p_("item"), "next"),
                    ast.if_(ast.has(p_("item"), "previous"),
                            ast.set(p_("item") / "next", "previous", p_("item") / "previous"),
                            ast.erase(p_("item") / "next", _("previous"))),
                    ast.if_(ast.has(p_("item"), "previous"),
                            m_("last") = p_("item") / "previous",
                            ast.erase(ast.self(), _("last")))),
            m_("size") = ast.subtract(m_("size"), _(_1_)));

    listStructT.addMethod("size")
        .returnType(_(type.Number))
        .code(
            ast.return_(m_("size")));

    listStructT.addMethod("empty")
        .returnType(_(type.Boolean))
        .code(
            ast.return_(ast.equal(m_("size"), _(_0_))));

    listStructT.addMethod("first")
        .returnType(tp_("valueType"))
        .code(
            ast.return_(m_("first") / "value"));

    listStructT.addMethod("last")
        .returnType(tp_("valueType"))
        .code(
            ast.return_(m_("last") / "value"));

    listStructT.addMethod("begin")
        .returnType(st_("itemType"))
        .code(
            ast.return_(m_("first")));

    listStructT.addMethod("end")
        .returnType(st_("itemType"))
        .code(
            ast.return_(m_("last")));
#pragma endregion
#pragma region Type
    auto& typeStruct
        = stdScope.addStruct("Type")
              .members(
                  member("slots", tt_("Map", "keyType", _(type.Cell), "valueType", _(type.Slot))),
                  member("incomplete", _(type.Boolean)),
                  member("sharedObject", _(type.Slot)),
                  member("subTypes", tt_("Map", "keyType", _(type.Cell), "valueType", "Type")),
                  member("memberOf", tt_("Map", "keyType", "Type", "valueType", "Type")),
                  member("asts", tt_("Map", "keyType", _(type.Cell), "valueType", _(type.ast.Function))),
                  member("methods", tt_("Map", "keyType", _(type.Cell), "valueType", _(type.op.Function))));

    typeStruct.addMethod("constructor")
        .code(
            m_("slots") = ast.new_(tt_("Map", "keyType", _(type.Cell), "valueType", _(type.Slot)), "constructor"));

    typeStruct.addMethod("constructorWithRecursiveType")
        .code(
            m_("slots") = ast.new_(tt_("Map", "keyType", _(type.Cell), "valueType", _(type.Slot)), "constructorWithIndexType", param("indexType", ast.self())));

    typeStruct.addMethod("addSubType")
        .parameters(
            param("slotRole", _(type.Cell)),
            param("slotType", _(type.Type_)))
        .code(
            ast.if_(m_("subTypes").missing(),
                    m_("subTypes") = ast.new_(tt_("Map", "keyType", _(type.Cell), "valueType", "Type"), "constructor")),
            m_("subTypes").call("add", param("key", p_("slotRole")), param("value", p_("slotType"))));

    typeStruct.addMethod("addMembership")
        .parameters(
            param("cell", _(type.Type_)))
        .code(
            ast.if_(m_("memberOf").missing(),
                    m_("memberOf") = ast.new_(tt_("Map", "keyType", "Type", "valueType", "Type"), "constructor")),
            m_("memberOf").call("add", param("key", p_("cell")), param("value", p_("cell"))));

    typeStruct.addMethod("addSlot")
        .parameters(
            param("slotRole", _(type.Cell)),
            param("slotType", _(type.Slot)))
        .code(
            ast.if_(m_("slots").missing(),
                    m_("slots") = ast.new_(tt_("Map", "keyType", _(type.Cell), "valueType", _(type.Slot)), "constructor")),
            var_("slot") = ast.new_(_(type.Slot)),
            ast.set(*var_("slot"), "slotRole", p_("slotRole")),
            ast.set(*var_("slot"), "slotType", p_("slotType")),
            m_("slots").call("add", param("key", p_("slotRole")), param("value", *var_("slot"))));

    typeStruct.addMethod("addSlots")
        .parameters(
            param("list", tt_("List", "valueType", _(type.Slot))))
        .code(
            ast.if_(ast.equal(p_("list") / "size", _(_0_)),
                    ast.return_()),
            var_("item") = p_("list") / "first",
            ast.if_(m_("slots").missing(),
                    m_("slots") = ast.new_(tt_("Map", "keyType", _(type.Cell), "valueType", _(type.Slot)), "constructor")),
            ast.do_(ast.block(
                        var_("next") = _(boolean.true_),
                        m_("slots").call("add", param("key", *var_("item") / "value" / "slotRole"), param("value", *var_("item") / "value")),
                        ast.if_(ast.has(*var_("item"), "next"),
                                var_("item") = *var_("item") / "next",
                                var_("next") = _(boolean.false_))),
                    ast.same(*var_("next"), _(boolean.true_))));

    typeStruct.addMethod("hasSlot")
        .parameters(
            param("slotRole", _(type.Cell)))
        .returnType(_(type.Boolean))
        .code(
            ast.if_(m_("slots").missing(),
                    ast.return_(_(boolean.false_))),
            ast.return_(m_("slots").call("hasKey", param("key", p_("slotRole")))));

    typeStruct.addMethod("removeSlot")
        .parameters(
            param("slotRole", _(type.Cell)))
        .code(
            ast.if_(m_("slots").missing(), ast.return_()),
            m_("slots").call("remove", param("key", p_("slotRole"))));
#pragma endregion
#pragma region Index
    auto& indexStruct
        = stdScope.addStruct("Index")
              .memberOf(_(type.Type_));

    indexStruct.addMethod("constructor")
        .code(
            ast.set(ast.self(), "type", ast.new_("Type", "constructorWithRecursiveType")),
            ast.set(m_("type"), "methods", ast.get(struct_("Index"), _("methods"))),
            ast.set(m_("type"), "memberOf", _(map(type.Type_, type.Type_, type.Index, type.Index))));

    indexStruct.addMethod("constructorWithSelfType")
        .parameters(
            param("indexType", _(type.Type_)))
        .code(
            ast.if_(ast.missing(p_("indexType"), _("sharedObject")),
                    ast.block(ast.set(p_("indexType"), "sharedObject", ast.new_(_(type.Slot))),
                              ast.set(p_("indexType") / "sharedObject", "slotRole", ast.self()),
                              ast.set(p_("indexType") / "sharedObject", "slotType", struct_("Index")))),
            ast.set(ast.self(), "type", p_("indexType")));

    /*
    void Index::insert(CellI& key, CellI& value)
    {
        if (&key == &"type") {
            throw "The type key can not be changed!";
        }
        m_slots[&key] = &value;
        if (m_recursiveType) {
            return;
        }
        Object& slot = *new Object(kb, kb.type.Slot);
        slot.set("slotRole", key);
        slot.set("slotType", kb.type.Slot);
        m_type->addSlot(key, slot);
    }
    */
    indexStruct.addMethod("insert")
        .parameters(
            param("key", _(type.Cell)),
            param("value", _(type.Cell)))
        .code(
            ast.if_(ast.same(p_("key"), _("type")),
                    ast.return_()),
            ast.set(ast.self(), p_("key"), p_("value")),
            ast.if_(ast.and_(ast.has(m_("type"), "sharedObject"), ast.same(m_("type") / "sharedObject" / "slotRole", ast.self())),
                    ast.return_()),
            m_("type").call("addSlot", param("slotRole", p_("key")), param("slotType", _(type.Slot))));

    indexStruct.addMethod("empty")
        .returnType(_(type.Boolean))
        .code(
            ast.return_(ast.call(m_("type") / "slots", "empty")));

    /*
    void Index::erase(CellI& role)
    {
        if (!m_type->hasSlot(role)) {
            return;
        }
        m_slots.erase(&role);
        m_type->removeSlot(role);
    }
    */
    indexStruct.addMethod("remove")
        .parameters(
            param("key", _(type.Cell)))
        .code(
            ast.if_(ast.not_(m_("type").call("hasSlot", param("slotRole", p_("key")))),
                    ast.return_()),
            ast.erase(ast.self(), p_("key")),
            m_("type").call("removeSlot", param("slotRole", p_("key"))));

    indexStruct.addMethod("size")
        .returnType(_(type.Number))
        .code(
            ast.return_(ast.call(m_("type") / "slots", "size")));
#pragma endregion
#pragma region Map
    auto& mapStructT
        = stdScope.addStructT("Map")
              .templateParams(
                  param("keyType", _(type.Type_)),
                  param("valueType", _(type.Type_)))
              .subTypes(
                  param("keyType", tp_("keyType")),
                  param("valueType", tp_("valueType")),
                  param("listType", tt_("List", "valueType", tp_("valueType"))))
              .memberOf(_(type.Container), _(type.Map))
              .members(
                  member("list", st_("listType")),
                  member("index", struct_("Index")),
                  member("size", _(type.Number)));

    mapStructT.addMethod("constructor")
        .code(
            m_("size")  = _(_0_),
            m_("list")  = ast.new_(st_("listType"), "constructor"),
            m_("index") = ast.new_("Index", "constructor"));

    mapStructT.addMethod("constructorWithIndexType")
        .parameters(
            param("indexType", _(type.Type_)))
        .code(
            m_("size")  = _(_0_),
            m_("list")  = ast.new_(st_("listType"), "constructor"),
            m_("index") = ast.new_("Index", "constructorWithSelfType", param("indexType", p_("indexType"))));

    /*
    bool Map::hasKey(CellI& key)
    {
        return m_index.has(key);
    }
    */
    mapStructT.addMethod("hasKey")
        .parameters(
            param("key", tp_("keyType")))
        .returnType(_(type.Boolean))
        .code(
            ast.return_(ast.has(m_("index"), p_("key"))));

    /*
    CellI& Map::getValue(CellI& key)
    {
        if (m_index.has(key)) {
            return m_index[key][kb.ids.value];
        }
        throw "No such role!";
    }
    */
    mapStructT.addMethod("getValue")
        .parameters(
            param("key", tp_("keyType")))
        .returnType(tp_("valueType"))
        .code(
            ast.if_(ast.has(m_("index"), p_("key")),
                    ast.return_(m_("index") / p_("key") / "value"),
                    ast.return_(_("emptyObject"))));

    /*
    void Map::add(CellI& key, CellI& value)
    {
        if (&key == &"type") {
            throw "ids.type can not be stored in a map!";
        }
        if (m_index.has(key)) {
            throw "A value already registered with this role";
        }
        List::Item& item = *m_list.add(value);
        m_index.insert(key, item);
        ++m_size;
    }
    */
    mapStructT.addMethod("add")
        .parameters(
            param("key", tp_("keyType")),
            param("value", tp_("valueType")))
        .code(
            ast.if_(ast.same(p_("key"), _("type")),
                    ast.return_()),
            ast.if_(ast.has(m_("index"), p_("key")),
                    ast.return_()),
            m_("size")   = ast.add(m_("size"), _(_1_)),
            var_("item") = m_("list").call("add", param("value", p_("value"))),
            m_("index").call("insert", param("key", p_("key")), param("value", *var_("item"))));

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
            param("key", tp_("keyType")))
        .code(
            ast.if_(ast.missing(m_("index"), p_("key")),
                    ast.return_()),
            m_("list").call("remove", param("item", m_("index") / p_("key"))),
            m_("index").call("remove", param("key", p_("key"))),
            m_("size") = ast.subtract(m_("size"), _(_1_)));

    mapStructT.addMethod("size")
        .returnType(_(type.Number))
        .code(
            ast.return_(m_("size")));

    mapStructT.addMethod("empty")
        .returnType(_(type.Boolean))
        .code(
            ast.return_(ast.equal(m_("size"), _(_0_))));

    mapStructT.addMethod("first")
        .returnType(tp_("valueType"))
        .code(
            ast.return_(m_("list") / "first" / "value"));

    mapStructT.addMethod("last")
        .returnType(tp_("valueType"))
        .code(
            ast.return_(m_("list") / "last" / "value"));

    mapStructT.addMethod("begin")
        .returnType(tt_("ListItem", "valueType", tp_("valueType")))
        .code(
            ast.return_(m_("list") / "first"));

    mapStructT.addMethod("end")
        .returnType(tt_("ListItem", "valueType", tp_("valueType")))
        .code(
            ast.return_(m_("list") / "last"));
#pragma endregion
#pragma region TrieMap
    auto& kvPairT
        = stdScope.addStructT("kvPair")
              .templateParams(
                  param("keyType", _(type.Type_)),
                  param("valueType", _(type.Type_)))
              .subTypes(
                  param("keyType", tp_("keyType")),
                  param("valueType", tp_("valueType")))
              .members(
                  member("key", tp_("keyType")),
                  member("value", tp_("valueType")));

    auto& trieMapStructT
        = stdScope.addStructT("TrieMap")
              .templateParams(
                  param("keyType", _(type.Type_)),
                  param("valueType", _(type.Type_)))
              .subTypes(
                  param("keyType", tp_("keyType")),
                  param("valueType", tp_("valueType")),
                  param("listType", tt_("List", "valueType", tt_("kvPair", "keyType", tp_("keyType"), "valueType", tp_("valueType")))))
              .memberOf(_(type.Container), _(type.TrieMap))
              .members(
                  member("list", st_("listType")),
                  member("rootNode", _(type.TrieMapNode)),
                  member("size", _(type.Number)));

    trieMapStructT.addMethod("constructor")
        .code(
            m_("list")  = ast.new_(st_("listType"), "constructor"),
            m_("rootNode") = ast.new_(_(type.TrieMapNode)),
            m_("size")  = _(_0_));

    /*
    bool TrieMap::hasKey(CellI& key)
    {
        CellI* currentNode = &m_rootNode;

        if (isA(key, kb.type.List)) {
            throw "Key is not a list!";
        }

        Visitor::visitList(key, [this, &currentNode](CellI& keyItem, int i, bool& stop) {
            CellI* children = nullptr;
            if (currentNode->missing(kb.ids.children)) {
                stop        = true;
                currentNode = nullptr;
                return;
            }
            Index& childrenIndex = static_cast<Index&>(currentNode->get(kb.ids.children));
            if (childrenIndex.has(keyItem)) {
                children = &childrenIndex.get(keyItem);
            } else {
                stop        = true;
                currentNode = nullptr;
                return;
            }
            currentNode = children;
        });

        if (!currentNode || currentNode->missing(kb.ids.data)) {
            return false;
        }

        return true;
    }
    */
#if 0
    trieMapStructT.addMethod("hasKey")
        .parameters(
            param("key", tp_("keyType")))
        .returnType(_(type.Boolean))
        .code(
            ast.return_(ast.has(m_("index"), p_("key")))); // TODO

    /*
    CellI& Map::getValue(CellI& key)
    {
        if (m_index.has(key)) {
            return m_index[key][kb.ids.value];
        }
        throw "No such role!";
    }
    */
    trieMapStructT.addMethod("getValue")
        .parameters(
            param("key", tp_("keyType")))
        .returnType(tp_("valueType"))
        .code(
            ast.if_(ast.has(m_("index"), p_("key")),
                    ast.return_(m_("index") / p_("key") / "value"),
                    ast.return_(_("emptyObject"))));

    /*
    void Map::add(CellI& key, CellI& value)
    {
        if (&key == &"type") {
            throw "ids.type can not be stored in a map!";
        }
        if (m_index.has(key)) {
            throw "A value already registered with this role";
        }
        List::Item& item = *m_list.add(value);
        m_index.insert(key, item);
        ++m_size;
    }
    */
    trieMapStructT.addMethod("add")
        .parameters(
            param("key", tp_("keyType")),
            param("value", tp_("valueType")))
        .code(
            ast.if_(ast.same(p_("key"), _("type")),
                    ast.return_()),
            ast.if_(ast.has(m_("index"), p_("key")),
                    ast.return_()),
            m_("size")   = ast.add(m_("size"), _(_1_)),
            var_("item") = m_("list").call("add", param("value", p_("value"))),
            m_("index").call("insert", param("key", p_("key")), param("value", *var_("item"))));

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
    trieMapStructT.addMethod("remove")
        .parameters(
            param("key", tp_("keyType")))
        .code(
            ast.if_(ast.missing(m_("index"), p_("key")),
                    ast.return_()),
            m_("list").call("remove", param("item", m_("index") / p_("key"))),
            m_("index").call("remove", param("key", p_("key"))),
            m_("size") = ast.subtract(m_("size"), _(_1_)));

    trieMapStructT.addMethod("size")
        .returnType(_(type.Number))
        .code(
            ast.return_(m_("size")));

    trieMapStructT.addMethod("empty")
        .returnType(_(type.Boolean))
        .code(
            ast.return_(ast.equal(m_("size"), _(_0_))));

    trieMapStructT.addMethod("first")
        .returnType(tp_("valueType"))
        .code(
            ast.return_(m_("list") / "first" / "value"));

    trieMapStructT.addMethod("last")
        .returnType(tp_("valueType"))
        .code(
            ast.return_(m_("list") / "last" / "value"));

    trieMapStructT.addMethod("begin")
        .returnType(tt_("ListItem", "valueType", tp_("valueType")))
        .code(
            ast.return_(m_("list") / "first"));

    trieMapStructT.addMethod("end")
        .returnType(tt_("ListItem", "valueType", tp_("valueType")))
        .code(
            ast.return_(m_("list") / "last"));
#pragma endregion
#endif
#pragma region Set
    auto& setStructT
        = stdScope.addStructT("Set")
              .templateParams(
                  param("keyType", _(type.Type_)),
                  param("valueType", _(type.Type_)))
              .subTypes(
                  param("valueType", tp_("valueType")),
                  param("listType", tt_("List", "valueType", tp_("valueType"))))
              .memberOf(_(type.Container))
              .members(
                  member("index", struct_("Index")),
                  member("size", _(type.Number)));

    setStructT.addMethod("constructor")
        .code(
            m_("size")  = _(_0_),
            m_("index") = ast.new_(struct_("Index"), "constructor"));

    setStructT.addMethod("add")
        .parameters(
            param("value", tp_("valueType")))
        .code(
            ast.if_(ast.has(m_("index"), p_("value")),
                    ast.return_()),
            ast.call(m_("index"), "insert", param("key", p_("value")), param("value", p_("value"))),
            m_("size") = ast.add(m_("size"), _(_1_)));

    setStructT.addMethod("contains")
        .parameters(
            param("value", tp_("valueType")))
        .returnType(_(type.Boolean))
        .code(
            ast.return_(ast.has(m_("index"), p_("value"))));

    setStructT.addMethod("remove")
        .parameters(
            param("value", tp_("valueType")))
        .code(
            ast.if_(ast.missing(m_("index"), p_("value")),
                    ast.return_()),
            ast.call(m_("index"), "remove", param("key", p_("value"))),
            m_("size") = ast.subtract(m_("size"), _(_1_)));

    setStructT.addMethod("first")
        .returnType(tp_("valueType"))
        .code(
            ast.return_(m_("index") / "type" / "slots" / "list" / "first" / "value" / "slotRole"));

    setStructT.addMethod("last")
        .returnType(tp_("valueType"))
        .code(
            ast.return_(m_("index") / "type" / "slots" / "list" / "last" / "value" / "slotRole"));

    setStructT.addMethod("begin")
        .returnType(tt_("ListItem", "valueType", tp_("valueType")))
        .code(
            ast.return_(m_("index") / "type" / "slots" / "list" / "last"));

    setStructT.addMethod("end")
        .returnType(tt_("ListItem", "valueType", tp_("valueType")))
        .code(
            ast.return_(m_("list") / "last"));

    setStructT.addMethod("size")
        .returnType(_(type.Number))
        .code(
            ast.return_(m_("size")));

    setStructT.addMethod("empty")
        .returnType(_(type.Boolean))
        .code(
            ast.if_(ast.equal(m_("size"), _(_0_)), ast.return_(_(boolean.true_)), ast.return_(_(boolean.false_))));
#pragma endregion
}

void Brain::createArcSolver()
{
    auto& arcScope = globalScope.addScope("arc");

    auto& colorStruct
        = arcScope.addStruct("Color")
              .members(
                  member("red", _(type.Number)),
                  member("green", _(type.Number)),
                  member("blue", _(type.Number)));

    // struct Pixel
    auto& pixelStruct
        = arcScope.addStruct("Pixel")
              .members(
                  member("x", _(type.Number)),
                  member("y", _(type.Number)));

    pixelStruct.addMethod("constructor")
        .parameters(
            param("x", _(type.Number)),
            param("y", _(type.Number)))
        .code(
            m_("x") = p_("x"),
            m_("y") = p_("y"));

    // struct Shape
    auto& shapeStruct
        = arcScope.addStruct("Shape")
              .members(
                  member("id", _(type.Number)),
                  member("color", struct_("Color")),
                  member("width", _(type.Number)),
                  member("height", _(type.Number)),
                  member("hybridPixels", tt_("std::Set", "valueType", _(type.Pixel))),
                  member("pixels", tt_("std::List", "valueType", struct_("Pixel"))));


    /*
    Shape(int id, input::Color color, int width, int height) :
        m_id(id), m_color(color), m_width(width), m_height(height) { }
    */
    shapeStruct.addMethod("constructor")
        .parameters(
            param("id", _(type.Number)),
            param("color", struct_("Color")),
            param("width", _(type.Number)),
            param("height", _(type.Number)))
        .code(
            m_("id")           = p_("id"),
            m_("color")        = p_("color"),
            m_("width")        = p_("width"),
            m_("height")       = p_("height"),
            m_("hybridPixels") = ast.new_(tt_("std::Set", "valueType", _(type.Pixel)), "constructor"),
            m_("pixels")       = ast.new_(tt_("std::List", "valueType", struct_("Pixel")), "constructor"));

    /*
    void Shape::addPixel(cells::hybrid::Pixel& pixel)
    {
        m_pixels.push_back({ pixel.m_x.value(), pixel.m_y.value() });
        m_hybridPixels.insert(&pixel);
    }
    */
    shapeStruct.addMethod("addPixel")
        .parameters(
            param("pixel", struct_("Pixel")))
        .code(
            m_("pixels").call("add", param("value", p_("pixel"))),
            m_("hybridPixels").call("add", param("value", p_("pixel"))));

    /*
    bool Shape::hasPixel(cells::hybrid::Pixel& pixel) const
    {
        return m_hybridPixels.find(&pixel) != m_hybridPixels.end();
    }
    */
    shapeStruct.addMethod("hasPixel")
        .parameters(
            param("pixel", _(type.Pixel)))
        .returnType(_(type.Boolean))
        .code(
            ast.return_(m_("hybridPixels").call("contains", param("value", p_("pixel")))));

    /*
    void Shape::sortPixels()
    {
        int width = m_width;
        std::sort(m_pixels.begin(), m_pixels.end(), [width](const Pixel& p1, const Pixel& p2) {
            return p1.y * width + p1.x < p2.y * width + p2.x;
        });
    }
    */
    shapeStruct.addMethod("sortPixels")
        .code(ast.return_()); // TODO sorting


    // struct Shaper
    auto& shaperStruct
        = arcScope.addStruct("Shaper")
              .members(
                  member("width", _(type.Number)),
                  member("height", _(type.Number)),
                  member("picture", _(type.Picture)),
                  member("shapes", tt_("std::List", "valueType", "Shape")),
                  member("inputPixels", tt_("std::Set", "valueType", _(type.Pixel))));

    /*
    Shaper::Shaper(const cells::hybrid::Picture& picture) :
        m_width(picture.width()),
        m_height(picture.height()),
        m_picture(picture),
        kb(picture.kb)
    {
        processInputPixels();
    }
    */
    shaperStruct.addMethod("constructor")
        .parameters(
            param("picture", _(type.Picture)))
        .code(
            m_("picture")     = p_("picture"),
            m_("width")       = p_("picture") / "width",
            m_("height")      = p_("picture") / "height",
            m_("shapes")      = ast.new_(tt_("std::List", "valueType", "Shape"), "constructor"),
            m_("inputPixels") = ast.new_(tt_("std::Set", "valueType", _(type.Pixel)), "constructor"),
            ast.self().call("processInputPixels"));
    /*
    void Shaper::processInputPixels()
    {
        std::vector<cells::hybrid::Pixel>& pixels = const_cast<cells::hybrid::Picture&>(m_picture).pixels();
        for (cells::hybrid::Pixel& pixel : pixels) {
            m_inputPixels.insert(&pixel);
        }
    }
    */
    shaperStruct.addMethod("processInputPixels")
        .code(
            var_("pixels") = m_("picture") / "pixels",
            var_("pixel")  = _(ids.emptyObject),
            ast.if_(ast.has(*var_("pixels"), "first"),
                    var_("pixel") = *var_("pixels") / "first"),
            ast.while_(ast.notSame(*var_("pixel"), _(ids.emptyObject)),
                       ast.block(
                           m_("inputPixels").call("add", param("value", *var_("pixel") / "value")),
                           ast.if_(ast.has(*var_("pixel"), "next"),
                                   var_("pixel") = *var_("pixel") / "next",
                                   var_("pixel") = _(ids.emptyObject)))));

    /*
    void Shaper::process()
    {
        int shapeId = 1;
        while (!m_inputPixels.empty()) {
            cells::hybrid::Pixel& firstPixel = **m_inputPixels.begin();
            m_shapes.push_back(std::make_shared<Shape>(shapeId++, firstPixel.color(), m_width, m_height));
            Shape& shape = *m_shapes.back();
            std::set<cells::hybrid::Pixel*> checkPixels;
            checkPixels.insert(&firstPixel);
            while (!checkPixels.empty()) {
                auto checkPixelIt                = checkPixels.begin();
                cells::hybrid::Pixel& checkPixel = **checkPixelIt;
                processPixel(shape, checkPixels, checkPixel);
                checkPixels.erase(checkPixelIt);
            }
            shape.sortPixels();
        }
        std::sort(m_shapes.begin(), m_shapes.end(),
            [](const std::shared_ptr<Shape>& lhs, const std::shared_ptr<Shape>& rhs)
            { return *lhs < *rhs; }
        );
    }
    */
    shaperStruct.addMethod("process")
        .code(
            var_("shapeId") = _(_1_),
            ast.while_(ast.not_(m_("inputPixels").call("empty")),
                       ast.block(
                           var_("firstPixel") = m_("inputPixels").call("first"),
                           m_("shapes").call("add", param("value", ast.new_("Shape", "constructor", param("id", *var_("shapeId")), param("color", *var_("firstPixel") / "color"), param("width", m_("width")), param("height", m_("height"))))),
                           var_("shapeId")     = ast.add(*var_("shapeId"), _(_1_)),
                           var_("shape")       = m_("shapes").call("last"),
                           var_("checkPixels") = ast.new_(tt_("std::Set", "valueType", _(type.Pixel)), "constructor"),
                           ast.call(*var_("checkPixels"), "add", param("value", *var_("firstPixel"))),
                           ast.while_(ast.not_(ast.call(*var_("checkPixels"), _("empty"))),
                                      ast.block(
                                          var_("checkPixel") = ast.call(*var_("checkPixels"), "first"),
                                          ast.self().call("processPixel", param("shape", *var_("shape")), param("checkPixels", *var_("checkPixels")), param("checkPixel", *var_("checkPixel"))),
                                          ast.call(*var_("checkPixels"), "remove", param("value", *var_("checkPixel"))))),
                           ast.call(*var_("shape"), _("sortPixels")))));

    /*
    void Shaper::processPixel(Shape& shape, std::set<cells::hybrid::Pixel*>& checkPixels, cells::hybrid::Pixel& checkPixel)
    {
        shape.addPixel(checkPixel);
        m_inputPixels.erase(&checkPixel);

        if (cells::hybrid::Pixel* pixel = processAdjacentPixel(kb.directions.up, shape, checkPixels, checkPixel)) {
            processAdjacentPixel(kb.directions.left, shape, checkPixels, *pixel);
            processAdjacentPixel(kb.directions.right, shape, checkPixels, *pixel);
        }
        if (cells::hybrid::Pixel* pixel = processAdjacentPixel(kb.directions.down, shape, checkPixels, checkPixel)) {
            processAdjacentPixel(kb.directions.left, shape, checkPixels, *pixel);
            processAdjacentPixel(kb.directions.right, shape, checkPixels, *pixel);
        }
        processAdjacentPixel(kb.directions.left, shape, checkPixels, checkPixel);
        processAdjacentPixel(kb.directions.right, shape, checkPixels, checkPixel);
    }
    */
    shaperStruct.addMethod("processPixel")
        .parameters(
            param("shape", struct_("Shape")),
            param("checkPixels", tt_("std::Set", "valueType", "Pixel")),
            param("checkPixel", struct_("Pixel")))
        .code(
            ast.call(p_("shape"), "addPixel", param("pixel", p_("checkPixel"))),
            m_("inputPixels").call("remove", param("value", p_("checkPixel"))),
            var_("pixel") = ast.self().call("processAdjacentPixel", param("direction", _(directions.up)), param("shape", p_("shape")), param("checkPixels", p_("checkPixels")), param("checkPixel", p_("checkPixel"))),
            ast.if_(ast.notSame(*var_("pixel"), _(ids.emptyObject)),
                    ast.block(
                        ast.self().call("processAdjacentPixel", param("direction", _(directions.left)), param("shape", p_("shape")), param("checkPixels", p_("checkPixels")), param("checkPixel", *var_("pixel"))),
                        ast.self().call("processAdjacentPixel", param("direction", _(directions.right)), param("shape", p_("shape")), param("checkPixels", p_("checkPixels")), param("checkPixel", *var_("pixel"))))),
            var_("pixel") = ast.self().call("processAdjacentPixel", param("direction", _(directions.down)), param("shape", p_("shape")), param("checkPixels", p_("checkPixels")), param("checkPixel", p_("checkPixel"))),
            ast.if_(ast.notSame(*var_("pixel"), _(ids.emptyObject)),
                    ast.block(
                        ast.self().call("processAdjacentPixel", param("direction", _(directions.left)), param("shape", p_("shape")), param("checkPixels", p_("checkPixels")), param("checkPixel", *var_("pixel"))),
                        ast.self().call("processAdjacentPixel", param("direction", _(directions.right)), param("shape", p_("shape")), param("checkPixels", p_("checkPixels")), param("checkPixel", *var_("pixel"))))),
            ast.self().call("processAdjacentPixel", param("direction", _(directions.left)), param("shape", p_("shape")), param("checkPixels", p_("checkPixels")), param("checkPixel", p_("checkPixel"))),
            ast.self().call("processAdjacentPixel", param("direction", _(directions.right)), param("shape", p_("shape")), param("checkPixels", p_("checkPixels")), param("checkPixel", p_("checkPixel"))));

    /*
    cells::hybrid::Pixel* Shaper::processAdjacentPixel(cells::CellI& direction, Shape& shape, std::set<cells::hybrid::Pixel*>& checkPixels, cells::hybrid::Pixel& checkPixel)
    {
        if (checkPixel.has(direction)) {
            cells::hybrid::Pixel& pixel = static_cast<cells::hybrid::Pixel&>(checkPixel[direction]);
            if (pixel.color() == shape.color() && !shape.hasPixel(pixel)) {
                checkPixels.insert(&pixel);
            }
            return &pixel;
        }

        return nullptr;
    }
    */
    shaperStruct.addMethod("processAdjacentPixel")
        .parameters(
            param("direction", _(type.Directions)),
            param("shape", struct_("Shape")),
            param("checkPixels", tt_("std::Set", "valueType", _(type.Pixel))),
            param("checkPixel", _(type.Pixel)))
        .returnType(_(type.Pixel))
        .code(
            ast.if_(ast.has(p_("checkPixel"), p_("direction")),
                    ast.block(
                        var_("pixel") = p_("checkPixel") / p_("direction"),
                        ast.if_(ast.and_(ast.equal(*var_("pixel") / "color", p_("shape") / "color"), ast.not_(ast.call(p_("shape"), "hasPixel", param("pixel", *var_("pixel"))))),
                                ast.call(p_("checkPixels"), "add", param("value", *var_("pixel")))),
                        ast.return_(*var_("pixel"))),
                    ast.return_(_(ids.emptyObject))));
}

void Brain::createTests()
{
    auto& testScope = globalScope.addScope("test");

    auto& testFunction = testScope.addFunction("testFunction");
    testFunction.code(
        var_("result") = ast.new_(struct_("std::Index")));

    auto& testVariable = testScope.addVariable(id("testVariable"));

    auto& testStruct = testScope.addStruct("Test");
    CellI& fullId    = testStruct.getFullId();

    testStruct.addMethod("testCreateNewListOfNumbers")
        .code(
            var_("result") = ast.new_(struct_("std::Index")),
            var_("result") = ast.new_(tt_("std::List", "valueType", _(type.Number))),
            var_("result") = ast.new_(tt_("std::List", "valueType", _(type.Cell))),
            var_("result") = ast.new_(tt_("std::List", "valueType", _(type.Pixel))),
            var_("result") = ast.new_(tt_("std::Set", "valueType", _(type.Number))),
            var_("result") = ast.new_(tt_("std::Map", "keyType", _(type.Number), "valueType", _(type.Color))),
            var_("result") = ast.new_(tt_("std::TrieMap", "keyType", _(type.Number), "valueType", _(type.Color))));

    CellI* mapPtr = nullptr;
    mapPtr        = &slots(type.slot("value", type.ListOf(type.Digit)),
                           type.slot("sign", type.Number)); // TODO sign has no class currently
    type.Number.set("slots", *mapPtr);

    testStruct.addMethod("factorial")
        .parameters(
            param("input", _(type.Number)))
        .returnType(_(type.Number))
        .code(
            ast.if_(ast.greaterThanOrEqual(p_("input"), _(_1_)),
                    ast.return_(ast.multiply(p_("input"), ast.self().call("factorial", param("input", ast.subtract(p_("input"), _(_1_)))))),
                    ast.return_(_(_1_))));

    // TODO
    //    type.String.method(ids.addSlots, { ids.list, list(type.slot(ids.value, type.ListOf(type.Char))) });
}

Brain::Brain() :
    m_initPhase(InitPhase::Init),
    pools(*this),
    ids(*this),
    type(*this),
    ast(*this),
    directions(*this),
    coordinates(*this),
    colors(*this),
    boolean(*this),
    numbers(*this),
    arc(*this),
    test(*this),
    _0_(pools.numbers.get(0)),
    _1_(pools.numbers.get(1)),
    _2_(pools.numbers.get(2)),
    _3_(pools.numbers.get(3)),
    _4_(pools.numbers.get(4)),
    _5_(pools.numbers.get(5)),
    _6_(pools.numbers.get(6)),
    _7_(pools.numbers.get(7)),
    _8_(pools.numbers.get(8)),
    _9_(pools.numbers.get(9)),
    globalScope(Ast::Scope(*this, "global"))
{
    createStd();
    createArcSolver();
    createTests();

    auto& compiledGlobalScope = globalScope.compile();
    compiledGlobalScopePtr    = &compiledGlobalScope[ids.data];
    m_initPhase               = InitPhase::FullyConstructed;

    // Test should be removed from here
    auto& compiledStructs        = static_cast<TrieMap&>(compiledGlobalScope[ids.data][ids.structs]);
    Visitor::visitList(compiledStructs[ids.list], [this](CellI& kv, int, bool&) {
        std::cout << kv[ids.key].label() << std::endl;
    });
    auto& compiledFunctions = static_cast<TrieMap&>(compiledGlobalScope[ids.data][ids.functions]);
    Visitor::visitList(compiledFunctions[ids.list], [this](CellI& kv, int, bool&) {
        std::cout << kv[ids.key].label() << " : " << kv[ids.value].label() << std::endl;
    });
    auto& compiledVariables = static_cast<TrieMap&>(compiledGlobalScope[ids.data][ids.variables]);
    Visitor::visitList(compiledVariables[ids.list], [this](CellI& kv, int, bool&) {
        std::cout << kv[ids.key].label() << " : " << kv[ids.value].label() << std::endl;
    });
    auto& compiledListItemStruct = static_cast<TrieMap&>(compiledGlobalScope[ids.data][ids.structs]).getValue(templateId("std::ListItem", id("valueType"), type.Cell));
    auto& compiledListStruct     = static_cast<TrieMap&>(compiledGlobalScope[ids.data][ids.structs]).getValue(templateId("std::List", id("valueType"), type.Cell));
    auto& compiledTypeStruct     = static_cast<TrieMap&>(compiledGlobalScope[ids.data][ids.structs]).getValue(id("std::Type"));
    auto& compiledIndexStruct    = static_cast<TrieMap&>(compiledGlobalScope[ids.data][ids.structs]).getValue(id("std::Index"));
    type.ListItem.set("methods", compiledListItemStruct[ids.methods]);
    type.List.set("methods", compiledListStruct[ids.methods]);
    type.Type_.set("methods", compiledTypeStruct[ids.methods]);

    Object testType(*this, compiledTypeStruct, id("constructor"), "testType");
    Object testRecursiveType(*this, compiledTypeStruct, id("constructorWithRecursiveType"), "testRecursiveType");

    Object testIndex(*this, compiledIndexStruct, id("constructor"), "testIndex");
    testIndex.method(id("insert"), { "key", _1_ }, { "value", _2_ });
}

Brain::~Brain()
{
    m_initPhase = InitPhase::DestructBegin;
}

CellI& Brain::id(const std::string& str)
{
    return pools.strings.getCharList(str);
}

CellI& Brain::toKbBool(bool value)
{
    return value ? boolean.true_ : boolean.false_;
}

Brain::InitPhase Brain::initPhase()
{
    return m_initPhase;
}

} // namespace brain
} // namespace cells
} // namespace synth
