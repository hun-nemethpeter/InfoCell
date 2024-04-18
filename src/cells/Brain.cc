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
    color(kb, kb.type.Char, "color"),
    condition(kb, kb.type.Char, "condition"),
    constructor(kb, kb.type.Char, "constructor"),
    continue_(kb, kb.type.Char, "continue_"),
    data(kb, kb.type.Char, "data"),
    destructor(kb, kb.type.Char, "destructor"),
    else_(kb, kb.type.Char, "else_"),
    emptyObject(kb, kb.type.Char, "emptyObject"),
    first(kb, kb.type.Char, "first"),
    height(kb, kb.type.Char, "height"),
    index(kb, kb.type.Char, "index"),
    input(kb, kb.type.Char, "input"),
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
    result(kb, kb.type.Char, "result"),
    returnType(kb, kb.type.Char, "returnType"),
    rhs(kb, kb.type.Char, "rhs"),
    role(kb, kb.type.Char, "role"),
    rootNode(kb, kb.type.Char, "rootNode"),
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
    subTypes(kb, kb.type.Char, "subTypes"),
    then(kb, kb.type.Char, "then"),
    type(kb, kb.type.Char, "type"),
    value(kb, kb.type.Char, "value"),
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
                    type.slot("objectType", type.Type_),
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
    Return(kb, kb.type.Type_, "ast::Return"),
    Same(kb, kb.type.Type_, "ast::Same"),
    Scope(kb, kb.type.Type_, "ast::Scope"),
    Self(kb, kb.type.Type_, "ast::Self"),
    SelfFn(kb, kb.type.Type_, "ast::SelfFn"),
    Set(kb, kb.type.Type_, "ast::Set"),
    Slot(kb, kb.type.Type_, "ast::Slot"),
    StaticCall(kb, kb.type.Type_, "ast::StaticCall"),
    Struct(kb, kb.type.Type_, "ast::Struct"),
    StructRef(kb, kb.type.Type_, "ast::StructRef"),
    StructT(kb, kb.type.Type_, "ast::StructT"),
    SubType(kb, kb.type.Type_, "ast::SubType"),
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

    map = &kb.slots(type.slot("value", type.Cell));
    Return.set("slots", *map);

    map = &kb.slots(type.slot("lhs", Base),
                    type.slot("rhs", Base));
    Same.set("slots", *map);

    map = &kb.slots(type.slot("id", type.Cell),
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
                    type.slot("incomplete", type.Boolean),
                    type.slot("instanceOf", Base),
                    type.slot("scope", Base),
                    type.slot("methods", type.MapOf(type.Cell, type.ast.Function)),
                    type.slot("members", type.ListOf(type.ast.Slot)),
                    type.slot("subTypes", type.ListOf(type.ast.Slot)),
                    type.slot("memberOf", type.ListOf(type.Type_)));
    Struct.set("slots", *map);

    map = &kb.slots(type.slot("value", type.Struct));
    StructRef.set("slots", *map);

    map = &kb.slots(type.slot("id", type.Cell),
                    type.slot("scope", Base),
                    type.slot("methods", type.MapOf(type.Cell, type.ast.Function)),
                    type.slot("members", type.ListOf(type.ast.Slot)),
                    type.slot("subTypes", type.ListOf(type.ast.Slot)),
                    type.slot("memberOf", type.ListOf(type.Type_)),
                    type.slot("templateParams", type.MapOf(type.Cell, type.Type_)));
    StructT.set("slots", *map);

    map = &kb.slots(type.slot("id", type.Cell));
    SubType.set("slots", *map);

    map = &kb.slots(type.slot("lhs", Base),
                    type.slot("rhs", Base));
    Subtract.set("slots", *map);

    map = &kb.slots(type.slot("id", Base),
                    type.slot("scopes", type.List),
                    type.slot("parameters", type.ListOf(Slot)));
    TemplatedType.set("slots", *map);

    map = &kb.slots(type.slot("role", type.Cell));
    TemplateParam.set("slots", *map);

    map = &kb.slots(type.slot("role", Base));
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

    mapPtr = &kb.slots(type.slot("slots", MapCellToSlot),
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
        listType.set("subTypes", kb.map(kb.type.Cell, kb.type.Type_, kb.ids.objectType, type, kb.ids.itemType, itemType));
        listType.set("memberOf", kb.map(kb.type.Type_, kb.type.Type_, kb.type.List, kb.type.List));
        itemType.set("memberOf", kb.map(kb.type.Type_, kb.type.Type_, kb.type.ListItem, kb.type.ListItem));
        itemType.set("subTypes", kb.map(kb.type.Cell, kb.type.Type_, kb.ids.objectType, type, kb.ids.itemType, itemType));

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
        mapType.set("methods", kb.type.Map["methods"]); // TODO
    }
    mapType.set("memberOf", kb.map(kb.type.Type_, kb.type.Type_, kb.type.Map, kb.type.Map));
    mapType.set("subTypes", kb.map(kb.type.Cell, kb.type.Type_, kb.ids.keyType, keyType, kb.ids.objectType, valueType));

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

    setType.set("methods", kb.type.Set["methods"]);
    setType.set("memberOf", kb.map(kb.type.Type_, kb.type.Type_, kb.type.Set, kb.type.Set));
    setType.set("subTypes", kb.map(kb.type.Cell, kb.type.Type_, kb.ids.objectType, valueType));

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

CellI& Ast::Base::resolveType(CellI& typeAst, CellI& resolveState)
{
    if (&typeAst.type() == &kb.type.ast.Cell) {
        auto& cellValue = typeAst["value"];
        if (!cellValue.isA(kb.type.Type_) && !cellValue.isA(kb.type.Enum)) {
            throw "Type AST referencing a non-type!";
        }
        return typeAst["value"];
    }
    if (&typeAst.type() == &kb.type.ast.StructRef) {
        CellI& astStructId = typeAst["value"];
        auto& resolveStruct = resolveStructId(astStructId, resolveState);
        // std::cout << std::format("DDDD {} resolved at {:p}\n", astStructId.label(), (void*)&resolveStruct) << std::endl;
        return resolveStruct;
    }
    if (&typeAst.type() == &kb.type.ast.TemplatedType) {
        return resolveTemplatedType(typeAst, resolveState);
    }

    throw "Unknown ast type node!";
}

CellI& Ast::Base::getResolvedTypeById(CellI& id, bool isInstance, CellI& resolveState)
{
    if (isInstance) {
        auto& instances = static_cast<TrieMap&>(resolveState["instances"]);
        if (instances.hasKey(id)) {
            return instances.getValue(id);
        } else {
            auto& unknownInstances = static_cast<TrieMap&>(resolveState["unknownInstances"]);
            return unknownInstances.getValue(id)["value"];
        }
    } else {
        auto& structs = static_cast<TrieMap&>(resolveState["structs"]);
        return structs.getValue(id);
    }
}

CellI& Ast::Base::resolveId(CellI& structId, CellI& containerId, CellI& unknownContainerId, CellI& resolveState, std::function<void(CellI& structReference)> unknownCb)
{
    auto& container = static_cast<TrieMap&>(resolveState[containerId]);

    if (container.hasKey(structId)) {
        return container.getValue(structId);
    } else {
        auto& unresolvedContainer = static_cast<TrieMap&>(resolveState[unknownContainerId]);
        if (unresolvedContainer.hasKey(structId)) {
            return unresolvedContainer.getValue(structId)["value"];
        } else {
            auto& unresolvedStruct = *new Object(kb, kb.type.Type_, std::format("{}", structId.label()));
            unresolvedStruct.set("incomplete", kb.boolean.true_);

            auto& structReference = *new Object(kb, kb.type.StructReference);
            structReference.set("value", unresolvedStruct);
            structReference.set("id", structId);
            structReference.set("scope", resolveState["scope"]);
            structReference.set("resolvedScope", resolveState["resolvedScope"]);

            if (resolveState.has("currentFn")) {
                structReference.set("currentFn", resolveState["currentFn"]);
            }
            if (resolveState.has("currentStruct")) {
                structReference.set("currentStruct", resolveState["currentStruct"]);
            }
            unknownCb(structReference);

            unresolvedContainer.add(structId, structReference);
            return unresolvedStruct;
        }
    }
}

CellI& Ast::Base::resolveTemplateInstanceId(CellI& structId, CellI& idScope, CellI& resolveState, CellI& ast, CellI& templateParams)
{
    auto& templateId = ast["id"];
    return resolveId(structId, kb.id("instances"), kb.id("unknownInstances"), resolveState, [this, &resolveState, &templateId, &templateParams, &ast, &idScope](CellI& structReference) {
        structReference.set("templateId", templateId);
        structReference.set("templateParams", templateParams);
        if (ast.has("scopes")) {
            structReference.set(kb.id("idScope"), idScope);
        }
    });
}

CellI& Ast::Base::resolveStructId(CellI& structId, CellI& resolveState)
{
    static std::function<void(CellI&)> emptyCb = [](CellI&) {};

    return resolveId(structId, kb.id("structs"), kb.id("unknownStructs"), resolveState, emptyCb);
}

CellI& Ast::Base::resolveTemplatedType(CellI& ast, CellI& resolveState)
{
    auto& templateId     = ast["id"];
    auto& templateParams = ast["parameters"];

    Scope* scopePtr = &static_cast<Scope&>(resolveState["scope"]);
    if (ast.has("scopes")) {
        auto& scopeList = ast["scopes"];
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
            std::cerr << std::format("Current struct: {}", resolveState["currentStruct"].label()) << std::endl;
        }
        if (resolveState.has("currentFn")) {
            std::cerr << std::format("Current function: {}", resolveState["currentFn"].label()) << std::endl;
        }
        throw std::format("Unknown template {}", templateId.label());
    }

    List& resolvedTemplateParams = *new List(kb, kb.type.Cell, "resolvedTemplateParams");
    List& idCell                 = generateTemplateId(templateId, templateParams, resolveState, resolvedTemplateParams);
    auto& resolvedInstance       = resolveTemplateInstanceId(idCell, scope, resolveState, ast, resolvedTemplateParams);

    // std::cout << std::format("DDDD {} resolved at {:p}\n", idCell.label(), (void*)&resolvedInstance) << std::endl;

    return resolvedInstance;
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
        CellI& slotRole = slot["slotRole"];
        CellI& slotType = slot["slotType"];
        CellI& resolvedSlotType = resolveType(slotType, resolveState);

        idCell.add(slotRole);
        idCell.add(resolvedSlotType);
        resolvedParams.add(kb.ast.slot(slotRole, resolvedSlotType));

        ss << std::format("{}={}", slotRole.label(), resolvedSlotType.label());
    });
    ss << ">";
    idCell.label(ss.str());

    return idCell;
}

Ast::Parameter::Parameter(brain::Brain& kb, CellI& role) :
    BaseT<Parameter>(kb, kb.type.ast.Parameter)
{
    set("role", role);
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
Ast::StructRef::StructRef(brain::Brain& kb, CellI& value) :
    BaseT<StructRef>(kb, kb.type.ast.StructRef)
{
    set("value", value);
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
    CellI& id = scope["id"];

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
    auto& name = function["name"];

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

bool Ast::Scope::hasStruct(CellI& id)
{
    if (missing("structs")) {
        if (missing("parent")) {
            return false;
        }
    } else if (structs().hasKey(id)) {
        return true;
    }

    if (missing("parent")) {
        return false;
    } else {
        return static_cast<Scope&>(get("parent")).hasStruct(id);
    }
}

Ast::Struct& Ast::Scope::getStruct(CellI& id)
{
    if (missing("structs")) {
        if (missing("parent")) {
            throw "No such struct";
        }
    } else if (structs().hasKey(id)) {
        return static_cast<Ast::Struct&>(structs().getValue(id));
    }

    if (missing("parent")) {
        throw "No such struct";
    } else {
        return static_cast<Scope&>(get("parent")).getStruct(id);
    }
}

void Ast::Scope::addStruct(Struct& struct_)
{
    CellI& id = struct_["id"];

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
    currentScope = getRootScope();
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

Ast::Scope* Ast::Scope::getRootScope()
{
    auto* currentScope = this;

    while (currentScope->has("parent")) {
        currentScope = &static_cast<Scope&>(currentScope->get("parent"));
    }

    return currentScope;
}

static bool debugCompiledStructs = false;

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

    auto& functions         = *new List(kb, kb.type.op.Function, "Functions");
    auto& structs           = *new TrieMap(kb, kb.type.Cell, kb.type.Type_, "structs");
    auto& unknownStructs    = *new TrieMap(kb, kb.type.Cell, kb.type.Type_, "unknownStructs");
    auto& instances         = *new TrieMap(kb, kb.type.Cell, kb.type.Type_, "instances");
    auto& unknownInstances  = *new TrieMap(kb, kb.type.Cell, kb.type.Type_, "unknownInstances");
    auto& compiledVariables = *new TrieMap(kb, kb.type.Cell, kb.type.op.Var, "Variables");

    programData.set("functions", compiledFunctions);
    programData.set("structs", compiledStructs);
    programData.set("variables", compiledVariables);

    compileState.set("functions", functions);
    compileState.set("structs", structs);
    compileState.set("unknownStructs", unknownStructs);
    compileState.set("instances", instances);
    compileState.set("unknownInstances", unknownInstances);
    compileState.set("variables", compiledVariables);
    compileState.set("resolvedScope", resolvedScope);
    compileState.set(kb.id("globalScope"), *this);
    compileState.set(kb.id("globalResolvedScope"), resolvedScope);

    resolveTypes(compileState);

    Visitor::visitList(unknownStructs["list"], [this](CellI& unknownStruct, int i, bool& stop) {
        if (debugCompiledStructs) {
            std::cout << "unknown struct: " << unknownStruct["value"].label() << std::endl;
        }
    });
    int instantiedNum = 0;
    Visitor::visitList(unknownInstances["list"], [this, &compileState, &instantiedNum](CellI& unknownInstance, int i, bool& stop) {
        auto& unknownInstanceId = unknownInstance["id"];
        if (debugCompiledStructs) {
            std::cout << "unknown instance: " << unknownInstanceId.label() << std::endl;
            if (unknownInstance.has("currentStruct")) {
                std::cout << "     from struct: " << unknownInstance["currentStruct"].label() << std::endl;
            }
            if (unknownInstance.has("currentFn")) {
                std::cout << "   from function: " << unknownInstance["currentFn"].label() << std::endl;
            }
        }
        std::stringstream ss;

        CellI& templateId     = unknownInstance["templateId"];
        CellI& templateParams = unknownInstance["templateParams"];

        ss << std::format("  instantiate id: {} <", templateId.label());
        Visitor::visitList(templateParams, [this, &ss, &compileState](CellI& param, int i, bool& stop) {
            CellI& paramId   = param["slotRole"];
            CellI& paramType = param["slotType"];
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

        auto& scope = static_cast<Scope&>(unknownInstance["scope"]);
        auto& resolvedScope    = static_cast<Scope&>(unknownInstance["resolvedScope"]);
        auto& idScope          = unknownInstance.has(kb.id("idScope")) ? static_cast<Scope&>(unknownInstance[kb.id("idScope")]) : scope;
        auto& resolvedIdScope  = static_cast<Scope&>(idScope["resolvedScope"]);
        compileState.set("scope", idScope);
        auto& structT          = idScope.getStructT(templateId);
        auto& instantiedStruct = structT.instantiateWith(resolvedIdScope, static_cast<List&>(templateParams), compileState);
        auto& resolvedStruct   = instantiedStruct.resolveTypes(compileState);
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
    auto& scope             = static_cast<Scope&>(state["scope"]);
    auto& resolvedScope     = static_cast<Scope&>(state["resolvedScope"]);
    auto& compiledFunctions = static_cast<TrieMap&>(programData["functions"]);
    auto& compiledStructs   = static_cast<TrieMap&>(programData["structs"]);
    auto& compiledVariables = static_cast<TrieMap&>(programData["variables"]);

    if (scope.has("functions")) {
        Visitor::visitList(resolvedScope.functions()["list"], [this, &state, &compiledFunctions](CellI& function, int i, bool& stop) {
            Ast::Function& astFunction = static_cast<Ast::Function&>(function);
            auto& compiledFunction     = astFunction.compile(state);
            compiledFunctions.add(astFunction["name"], compiledFunction);
        });
    }
    if (scope.has("structs")) {
        Visitor::visitList(resolvedScope.structs()["list"], [this, &state, &compiledStructs](CellI& struct_, int i, bool& stop) {
            Ast::Struct& astStruct = static_cast<Ast::Struct&>(struct_);
            auto& compiledStruct = astStruct.compile(state);
            compiledStructs.add(astStruct.id(), compiledStruct);
        });
    }
    if (scope.has("variables")) {
        Visitor::visitList(resolvedScope.variables()["list"], [this, &compiledVariables](CellI& var, int i, bool& stop) {
            Ast::Var& astVar = static_cast<Ast::Var&>(var);
            auto& varName    = astVar["role"];
            auto& compiledVariable = *new Object(kb, kb.type.op.Var);
            compiledVariables.add(varName, compiledVariable);
        });
    }
    if (scope.has("scopes")) {
        Visitor::visitList(scopes()["list"], [this, &programData, &state, &resolvedScope](CellI& scope, int i, bool& stop) {
            Ast::Scope& astScope = static_cast<Ast::Scope&>(scope);
            state.set("scope", astScope);
            state.set("resolvedScope", resolvedScope.getScope(astScope["id"]));
            astScope.compileTheResolvedAsts(programData, state);
            state.set("scope", *this);
            state.set("resolvedScope", resolvedScope);
        });
    }
}

void Ast::Scope::resolveTypes(CellI& state)
{
    state.set("scope", *this);
    Ast::Scope& resolvedScope = static_cast<Ast::Scope&>(state["resolvedScope"]);

    if (has("functions")) {
        state.erase("currentStruct");
        Visitor::visitList(functions()["list"], [this, &state, &resolvedScope](CellI& origAstFunctionCell, int i, bool& stop) {
            Ast::Function& origAstFunction = static_cast<Ast::Function&>(origAstFunctionCell);
            Ast::Function& resolvedAstFunction = origAstFunction.resolveTypes(state);
            resolvedScope.addFunction(resolvedAstFunction);
            std::cout << resolvedAstFunction.label() << std::endl;
        });
    }
    if (has("structs")) {
        Visitor::visitList(structs()["list"], [this, &state, &resolvedScope](CellI& origAstStructCell, int i, bool& stop) {
            Ast::Struct& origAstStruct = static_cast<Ast::Struct&>(origAstStructCell);
            Ast::Struct& resolvedAstStruct = origAstStruct.resolveTypes(state);
            resolvedScope.addStruct(resolvedAstStruct);
        });
    }
    if (has("variables")) {
        Visitor::visitList(variables()["list"], [this, &state, &resolvedScope](CellI& origAstVarCell, int i, bool& stop) {
            Ast::Var& origAstVar = static_cast<Ast::Var&>(origAstVarCell);
            auto& varName        = origAstVarCell["role"];
            // TODO
            // addVariable();
        });
    }
    if (has("scopes")) {
        Visitor::visitList(scopes()["list"], [this, &state, &resolvedScope](CellI& origAstScopeCell, int i, bool& stop) {
            Ast::Scope& origAstScope = static_cast<Ast::Scope&>(origAstScopeCell);
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
    auto& name = method["name"];

    if (missing("methods")) {
        set("methods", *new Map(kb, kb.type.Cell, kb.type.ast.Function, "Map<Cell, Type::Ast::Function>(...)"));
    }
    if (methods().hasKey(name)) {
        throw "Already registered!";
    }
    method.set("structType", *this);
    methods().add(name, method);
}

void Ast::StructBase::members(Slot& param)
{
    if (missing("members")) {
        set("members", *new List(kb, kb.type.ast.Slot));
    }
    members().add(param);
}

void Ast::StructBase::subTypes(Slot& param)
{
    if (missing("subTypes")) {
        set("subTypes", *new List(kb, kb.type.ast.Slot));
    }
    subTypes().add(param);
}

void Ast::StructBase::memberOf(CellI& type)
{
    if (missing("memberOf")) {
        set("memberOf", *new List(kb, kb.type.Type_));
    }
    memberOf().add(type);
}

Map& Ast::StructBase::methods()
{
    if (missing("methods")) {
        throw "No methods!";
    } else {
        return static_cast<Map&>(get("methods"));
    }
}

List& Ast::StructBase::members()
{
    if (missing("members")) {
        throw "No members!";
    } else {
        return static_cast<List&>(get("members"));
    }
}

List& Ast::StructBase::subTypes()
{
    if (missing("subTypes")) {
        throw "No subTypes!";
    } else {
        return static_cast<List&>(get("subTypes"));
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

Ast::Struct::Struct(brain::Brain& kb, CellI& id, const std::string& label) :
    StructBase(kb, kb.type.ast.Struct, id, label)
{
}

Ast::Struct::Struct(brain::Brain& kb, const std::string& name) :
    StructBase(kb, kb.type.ast.Struct, name)
{
}

Ast::Struct& Ast::Struct::resolveTypes(CellI& state)
{
    auto& structId       = get("id");
    auto& structs        = static_cast<TrieMap&>(state["structs"]);
    auto& unknownStructs = static_cast<TrieMap&>(state["unknownStructs"]);
    Ast::Struct& ret     = *new Ast::Struct(kb, structId, std::format("{}", label()));

    if (has("instanceOf")) {
        ret.set("instanceOf", get("instanceOf"));
    }

    if (unknownStructs.hasKey(structId)) {
        unknownStructs.remove(structId);
    }
    auto& resolvedStruct = *new Object(kb, kb.type.Type_, std::format("{}", structId.label()));
    structs.add(structId, resolvedStruct);

    state.set("currentStruct", ret);

    std::stringstream ss;
    if (debugCompiledStructs) {
        ss << std::format("struct {}", label());
    }

    // resolve memberOf list
    if (has("memberOf")) {
        ss << " : ";
        Visitor::visitList(memberOf(), [this, &ret, &state, &ss](CellI& membershipType, int i, bool& stop) {
            CellI& resolvedMembershipType = resolveType(membershipType, state);
            if (i > 0) {
                ss << ", ";
            }
            ss << resolvedMembershipType.label();
            ret.memberOf(resolvedMembershipType);
        });
    }
    if (debugCompiledStructs) {
        std::cout << ss.str() << " {" << std::endl;
    }

    // resolve sub types
    if (has("subTypes")) {
        Visitor::visitList(subTypes(), [this, &ret, &state](CellI& subTypeCell, int i, bool& stop) {
            CellI& subTypeId           = subTypeCell["slotRole"];
            CellI& subTypeType         = subTypeCell["slotType"];
            CellI& resolvedSubTypeType = resolveType(subTypeType, state);
            ret.subTypes(kb.ast.slot(subTypeId, resolvedSubTypeType));
            if (debugCompiledStructs) {
                std::cout << std::format("    alias {} = {};", subTypeId.label(), resolvedSubTypeType.label()) << std::endl;
            }
        });
        if (debugCompiledStructs) {
            if (has("methods") || has("members")) {
                std::cout << std::endl;
            }
        }
    }

    // resolve methods
    if (has("methods")) {
        Visitor::visitList(methods()["list"], [this, &ret, &state](CellI& origAstFunctionCell, int i, bool& stop) {
            auto& origAstFunction = static_cast<Ast::Function&>(origAstFunctionCell);
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
        Visitor::visitList(members(), [this, &ret, &state](CellI& memberCell, int i, bool& stop) {
            CellI& memberId   = memberCell["slotRole"];
            CellI& memberType = memberCell["slotType"];
            CellI& resolvedMemberType = resolveType(memberType, state);
            ret.members(kb.ast.slot(memberId, resolvedMemberType));
            if (debugCompiledStructs) {
                std::cout << std::format("    {}: {};", memberId.label(), resolvedMemberType.label()) << std::endl;
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
    auto& structId        = get("id");
    CellI& compiledStruct = getResolvedTypeById(structId, has("instanceOf"), state);
    compiledStruct.erase("incomplete");
    // std::cout << std::format("DDDD compile {} resolved at {:p}\n", structId.label(), (void*)&compiledStruct) << std::endl;

    // compile methods
    if (has("methods")) {
        Map& compiledMethods = *new Map(kb, kb.type.Cell, kb.type.ast.Function);
        Visitor::visitList(methods()["list"], [this, &compiledMethods, &state](CellI& astFunction, int i, bool& stop) {
            auto& compiledFunction = static_cast<Ast::Function&>(astFunction).compile(state);
            compiledMethods.add(astFunction["name"], compiledFunction);
        });
        compiledStruct.set("methods", compiledMethods);
    }

    // compile members
    if (has("members")) {
        Map& compiledMembers = *new Map(kb, kb.type.Cell, kb.type.Slot, "members Map<Cell, Slot>(...)");
        Visitor::visitList(members(), [this, &compiledMembers, &compiledStruct, &state](CellI& slot, int i, bool& stop) {
            CellI& slotRole          = slot["slotRole"];
            CellI& slotType          = slot["slotType"];
            compiledMembers.add(slotRole, kb.type.slot(slotRole, slotType));
        });
        compiledStruct.set("slots", compiledMembers);
    }

    // compile sub types
    if (has("subTypes")) {
        Map& compiledSubTypes = *new Map(kb, kb.type.Cell, kb.type.Type_, "subTypes Map<Cell, Type>(...)");
        Visitor::visitList(subTypes(), [this, &compiledSubTypes](CellI& slot, int i, bool& stop) {
            CellI& slotRole          = slot["slotRole"];
            CellI& slotType          = slot["slotType"];
            compiledSubTypes.add(slotRole, slotType);
        });
        compiledStruct.set("subTypes", compiledSubTypes);
    }

    // compile memberOf list
    if (has("memberOf")) {
        Map& compiledMemberOfs = *new Map(kb, kb.type.Type_, kb.type.Type_, "memberOf Map<Type, Type>(...)");
        Visitor::visitList(memberOf(), [this, &compiledMemberOfs](CellI& membershipType, int i, bool& stop) {
            compiledMemberOfs.add(membershipType, membershipType);
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

void Ast::StructT::templateParams(Slot& slot)
{
    if (missing("templateParams")) {
        set("templateParams", *new Map(kb, kb.type.Cell, kb.type.Type_));
    }
    CellI& slotRole = slot["slotRole"];
    CellI& slotType = slot["slotType"];
    if (!(&slotType.type() == &kb.type.ast.Cell || &slotType.type() == &kb.type.ast.TemplatedType)) {
        throw "Invalid template param type!";
    }
    CellI* paramType = nullptr;
    if (&slotType.type() == &kb.type.ast.Cell) {
        paramType = &slotType["value"];
    } else {
        throw "TODO";
    }
    templateParams().add(slotRole, *paramType);
}

Ast::Struct& Ast::StructT::instantiateWith(Scope& scope, List& inputParams, CellI& state)
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
        CellI& slotRole = slot["slotRole"];
        CellI& slotType = slot["slotType"];
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
    idCell.label(std::format("id of {}<{}>", id().label(), ss.str()));
    Ast::Struct& ret = *new Ast::Struct(kb, idCell, std::format("{}<{}>", id().label(), ss.str()));
    ret.set("instanceOf", *this);

    // instantiate methods
    if (has("methods")) {
        Visitor::visitList(methods()["list"], [this, &inputParameters, &ret, &state](CellI& astFunction, int i, bool& stop) {
            Ast::Function& instantiedFunction = *new Function(kb, astFunction["name"], astFunction["name"].label());
            ret.addMethod(instantiedFunction);

            // parameters
            if (astFunction.has("parameters")) {
                List& instantiatedParameters = *new List(kb, kb.type.Slot);
                Visitor::visitList(astFunction["parameters"], [this, &inputParameters, &instantiatedParameters, &ret, &state](CellI& slot, int i, bool& stop) {
                    CellI& slotRole          = slot["slotRole"];
                    CellI& slotType          = slot["slotType"];
                    CellI& instantiatedParam = instantiateTemplateParamType(slotType, ret, inputParameters, state);
                    instantiatedParameters.add(kb.ast.slot(slotRole, instantiatedParam));
                });
                instantiedFunction.set("parameters", instantiatedParameters);
            }
            // return type
            if (astFunction.has("returnType")) {
                CellI& returnType             = astFunction["returnType"];
                CellI& instantiatedReturnType = instantiateTemplateParamType(returnType, ret, inputParameters, state);
                instantiedFunction.set("returnType", instantiatedReturnType);
            }
            // code
            if (astFunction.has("code")) {
                instantiedFunction.set("code", instantiateAst(astFunction["code"], ret, inputParameters, state));
            }
        });
    }

    // instantiate members
    if (has("members")) {
        List& instantiatedMembers = *new List(kb, kb.type.Slot);
        Visitor::visitList(members(), [this, &inputParameters, &instantiatedMembers, &ret, &state](CellI& slot, int i, bool& stop) {
            CellI& slotRole = slot["slotRole"];
            CellI& slotType = slot["slotType"];
            CellI& instantiatedParam = instantiateTemplateParamType(slotType, ret, inputParameters, state);
            instantiatedMembers.add(kb.ast.slot(slotRole, instantiatedParam));
        });
        ret.set("members", instantiatedMembers);
    }

    // instantiate sub types
    if (has("subTypes")) {
        List& instantiatedSubTypes = *new List(kb, kb.type.Type_);
        Visitor::visitList(subTypes(), [this, &inputParameters, &instantiatedSubTypes, &ret, &state](CellI& slot, int i, bool& stop) {
            CellI& slotRole = slot["slotRole"];
            CellI& slotType = slot["slotType"];
            CellI& instantiatedParam = instantiateTemplateParamType(slotType, ret, inputParameters, state);
            instantiatedSubTypes.add(kb.ast.slot(slotRole, instantiatedParam));
        });
        ret.set("subTypes", instantiatedSubTypes);
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
        CellI& paramValue = param["role"];
        if (!inputParameters.hasKey(paramValue)) {
            throw "Instantiating with unknown template parameter!";
        }
        return kb.ast.cell(inputParameters.getValue(paramValue));
    }
    if (&param.type() == &kb.type.ast.TemplatedType) {
        List& resolvedParameterList = *new List(kb, kb.type.Slot);
        auto& ret                   = *new TemplatedType(kb, param["id"], resolvedParameterList);
        auto& parametersList        = param["parameters"];

        Visitor::visitList(parametersList, [this, &resolvedParameterList, &selfType, &inputParameters, &state](CellI& slot, int, bool&) {
            CellI& slotRole         = slot["slotRole"];
            CellI& slotType         = slot["slotType"];
            CellI& resolvedSlotType = instantiateTemplateParamType(slotType, selfType, inputParameters, state);
            resolvedParameterList.add(kb.ast.slot(slotRole, resolvedSlotType));
        });

        return ret;
    }
    if (&param.type() == &kb.type.ast.Cell || &param.type() == &kb.type.ast.StructRef) {
        return param;
    }

    throw "Unknown template parameter!";
}

Ast::Base& Ast::StructT::instantiateAst(CellI& ast, CellI& selfType, Map& inputParameters, CellI& state)
{
    auto instantiate = [this, &selfType, &inputParameters, &state](CellI& ast) -> Ast::Base& { return instantiateAst(ast, selfType, inputParameters, state); };

    if (&ast.type() == &kb.type.ast.New) {
        auto* objectTypePtr = &ast["objectType"];
        if (&(*objectTypePtr).type() == &kb.type.ast.TemplatedType) {
            CellI& resolvedObjectType = instantiateTemplateParamType(*objectTypePtr, selfType, inputParameters, state);
            objectTypePtr             = &resolvedObjectType;
        }
        auto& objectType = *static_cast<Base*>(objectTypePtr);
        if (ast.has("constructor")) {
            auto& constructor = ast["constructor"];
            Ast::Base& ret    = kb.ast.new_(objectType, static_cast<Base&>(constructor));
            if (ast.has("parameters")) {
                auto& newParameters = *new cells::List(kb, kb.type.ast.Slot);
                Visitor::visitList(ast["parameters"], [this, &newParameters, &instantiate](CellI& slot, int, bool&) {
                    newParameters.add(kb.ast.slot(slot["slotRole"], instantiate(slot["slotType"])));
                });
                ret.set("parameters", newParameters);
            }
            return ret;
        } else {
            return kb.ast.new_(objectType);
        }
    } else if (&ast.type() == &kb.type.ast.Call) {
        Ast::Base& ret = kb.ast.call(instantiate(ast["cell"]), ast["method"]);
        if (ast.has("parameters")) {
            // TODO process parameters
            ret.set("parameters", ast["parameters"]);
        }
        return ret;
    } else if (&ast.type() == &kb.type.ast.StaticCall) {
        Ast::Base& ret = kb.ast.scall(instantiate(ast["cell"]), ast["method"]);
        if (ast.has("parameters")) {
            // TODO process parameters
            ret.set("parameters", ast["parameters"]);
        }
        return ret;
    } 

    // do nothing just traverse and copy the AST nodes
    if (&ast.type() == &kb.type.ast.Block) {
        auto& instantiedAsts = *new cells::List(kb, kb.type.ast.Base);
        Visitor::visitList(ast["asts"], [this, &instantiedAsts, &instantiate](CellI& ast, int, bool&) {
            instantiedAsts.add(instantiate(ast));
        });
        return *new Block(kb, instantiedAsts);
    } else if (&ast.type() == &kb.type.ast.Cell) {
        return kb.ast.cell(ast["value"]);
    } else if (&ast.type() == &kb.type.ast.StructRef) {
        return kb.ast.structRef(ast["value"]);
    } else if (&ast.type() == &kb.type.ast.SelfFn) {
        return kb.ast.selfFn();
    } else if (&ast.type() == &kb.type.ast.Self) {
        return kb.ast.self();
    } else if (&ast.type() == &kb.type.ast.Parameter) {
        return kb.ast.parameter(ast["role"]);
    } else if (&ast.type() == &kb.type.ast.Var) {
        return kb.ast.var(ast["role"]);
    } else if (&ast.type() == &kb.type.ast.Delete) {
        return kb.ast.delete_(instantiate(ast["cell"]));
    } else if (&ast.type() == &kb.type.ast.Set) {
        return kb.ast.set(instantiate(ast["cell"]), instantiate(ast["role"]), instantiate(ast["value"]));
    } else if (&ast.type() == &kb.type.ast.Erase) {
        return kb.ast.erase(instantiate(ast["cell"]), instantiate(ast["role"]));
    } else if (&ast.type() == &kb.type.ast.If) {
        if (ast.has("else_")) {
            return kb.ast.if_(instantiate(ast["condition"]), instantiate(ast["then"]), instantiate(ast["else_"]));
        } else {
            return kb.ast.if_(instantiate(ast["condition"]), instantiate(ast["then"]));
        }
    } else if (&ast.type() == &kb.type.ast.Do) {
        return kb.ast.do_(instantiate(ast["statement"]), instantiate(ast["condition"]));
    } else if (&ast.type() == &kb.type.ast.While) {
        return kb.ast.while_(instantiate(ast["condition"]), instantiate(ast["statement"]));
    } else if (&ast.type() == &kb.type.ast.And) {
        return kb.ast.and_(instantiate(ast["lhs"]), instantiate(ast["rhs"]));
    } else if (&ast.type() == &kb.type.ast.Or) {
        return kb.ast.or_(instantiate(ast["lhs"]), instantiate(ast["rhs"]));
    } else if (&ast.type() == &kb.type.ast.Not) {
        return kb.ast.not_(instantiate(ast["input"]));
    } else if (&ast.type() == &kb.type.ast.Add) {
        return kb.ast.add(instantiate(ast["lhs"]), instantiate(ast["rhs"]));
    } else if (&ast.type() == &kb.type.ast.Subtract) {
        return kb.ast.subtract(instantiate(ast["lhs"]), instantiate(ast["rhs"]));
    } else if (&ast.type() == &kb.type.ast.Multiply) {
        return kb.ast.multiply(instantiate(ast["lhs"]), instantiate(ast["rhs"]));
    } else if (&ast.type() == &kb.type.ast.Divide) {
        return kb.ast.divide(instantiate(ast["lhs"]), instantiate(ast["rhs"]));
    } else if (&ast.type() == &kb.type.ast.LessThan) {
        return kb.ast.lessThan(instantiate(ast["lhs"]), instantiate(ast["rhs"]));
    } else if (&ast.type() == &kb.type.ast.LessThanOrEqual) {
        return kb.ast.lessThanOrEqual(instantiate(ast["lhs"]), instantiate(ast["rhs"]));
    } else if (&ast.type() == &kb.type.ast.GreaterThan) {
        return kb.ast.greaterThan(instantiate(ast["lhs"]), instantiate(ast["rhs"]));
    } else if (&ast.type() == &kb.type.ast.GreaterThanOrEqual) {
        return kb.ast.greaterThanOrEqual(instantiate(ast["lhs"]), instantiate(ast["rhs"]));
    } else if (&ast.type() == &kb.type.ast.Same) {
        return kb.ast.same(instantiate(ast["lhs"]), instantiate(ast["rhs"]));
    } else if (&ast.type() == &kb.type.ast.NotSame) {
        return kb.ast.notSame(instantiate(ast["lhs"]), instantiate(ast["rhs"]));
    } else if (&ast.type() == &kb.type.ast.Equal) {
        return kb.ast.equal(instantiate(ast["lhs"]), instantiate(ast["rhs"]));
    } else if (&ast.type() == &kb.type.ast.NotEqual) {
        return kb.ast.notEqual(instantiate(ast["lhs"]), instantiate(ast["rhs"]));
    } else if (&ast.type() == &kb.type.ast.Has) {
        return kb.ast.has(instantiate(ast["cell"]), instantiate(ast["role"]));
    } else if (&ast.type() == &kb.type.ast.Missing) {
        return kb.ast.missing(instantiate(ast["cell"]), instantiate(ast["role"]));
    } else if (&ast.type() == &kb.type.ast.Get) {
        return kb.ast.get(instantiate(ast["cell"]), instantiate(ast["role"]));
    } else if (&ast.type() == &kb.type.ast.Member) {
        return kb.ast.member(ast["role"]);
    } else if (&ast.type() == &kb.type.ast.Return) {
        if (ast.has("value")) {
            return kb.ast.return_(static_cast<Base&>(instantiate(ast["value"])));
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
    set("id", kb.pools.strings.get(name)["value"]);
    label(name);
}

void Ast::Function::parameters(Slot& param)
{
    if (missing("parameters")) {
        set("parameters", *new List(kb, kb.type.Slot));
    }
    parameters().add(param);
}

void Ast::Function::returnType(CellI& type)
{
    set("returnType", type);
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
            CellI& paramId           = param["slotRole"];
            CellI& paramType         = param["slotType"];
            CellI& resolvedParamType = resolveType(paramType, state);
            if (i > 0) {
                ss << ", ";
            }
            ss << std::format("{}: {}", paramId.label(), resolvedParamType.label());
            ret.parameters(kb.ast.slot(paramId, resolvedParamType));
        });
    }
    ss << ")";
    if (has("returnType")) {
        ss << " -> ";
        CellI& retType = returnType();
        CellI& resolvedRetType = resolveType(retType, state);
        ss << resolvedRetType.label();
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
        CellI& objectType             = ast["objectType"];
        Ast::Base* resolvedObjectTypePtr = nullptr;
        if (&objectType.type() == &kb.type.ast.Member) {
            resolvedObjectTypePtr = &static_cast<Ast::Base&>(objectType);
        } else {
            resolvedObjectTypePtr = &kb.ast.cell(resolveType(objectType, resolveState));
        }
        Ast::Base& resolvedObjectType = *resolvedObjectTypePtr;

        if (ast.has("constructor")) {
            auto& constructor = ast["constructor"];
            Ast::Base& ret = kb.ast.new_(resolvedObjectType, static_cast<Base&>(ast["constructor"]));

            if (ast.has("parameters")) {
                auto& newParameters = *new cells::List(kb, kb.type.ast.Slot);
                Visitor::visitList(ast["parameters"], [this, &newParameters, &resolveNode, &resolveState](CellI& slot, int, bool&) {
                    newParameters.add(kb.ast.slot(slot["slotRole"], resolveNode(slot["slotType"])));
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
            retPtr = &kb.ast.call(resolveNode(ast["cell"]), ast["method"]);
        } else {
            retPtr = &kb.ast.scall(kb.ast.cell(resolveType(ast["cell"], resolveState)), ast["method"]);
        }
        Ast::Base& ret = *retPtr;
        if (ast.has("parameters")) {
            auto& newParameters = *new cells::List(kb, kb.type.ast.Slot);
            Visitor::visitList(ast["parameters"], [this, &newParameters, &resolveNode, &resolveState](CellI& slot, int, bool&) {
                newParameters.add(kb.ast.slot(slot["slotRole"], resolveNode(slot["slotType"])));
            });
            ret.set("parameters", newParameters);
        }
        return ret;
    }

    // do nothing just traverse and copy the AST nodes
    if (&ast.type() == &kb.type.ast.Block) {
        auto& instantiedAsts = *new cells::List(kb, kb.type.ast.Base);
        Visitor::visitList(ast["asts"], [this, &instantiedAsts, &resolveNode](CellI& ast, int, bool&) {
            instantiedAsts.add(resolveNode(ast));
        });
        return *new Block(kb, instantiedAsts);
    } else if (&ast.type() == &kb.type.ast.Cell) {
        return kb.ast.cell(ast["value"]);
    } else if (&ast.type() == &kb.type.ast.StructRef) {
        return kb.ast.structRef(ast["value"]);
    } else if (&ast.type() == &kb.type.ast.SelfFn) {
        return kb.ast.selfFn();
    } else if (&ast.type() == &kb.type.ast.Self) {
        return kb.ast.self();
    } else if (&ast.type() == &kb.type.ast.Parameter) {
        return kb.ast.parameter(ast["role"]);
    } else if (&ast.type() == &kb.type.ast.Var) {
        return kb.ast.var(ast["role"]);
    } else if (&ast.type() == &kb.type.ast.Delete) {
        return kb.ast.delete_(resolveNode(ast["cell"]));
    } else if (&ast.type() == &kb.type.ast.Set) {
        return kb.ast.set(resolveNode(ast["cell"]), resolveNode(ast["role"]), resolveNode(ast["value"]));
    } else if (&ast.type() == &kb.type.ast.Erase) {
        return kb.ast.erase(resolveNode(ast["cell"]), resolveNode(ast["role"]));
    } else if (&ast.type() == &kb.type.ast.If) {
        if (ast.has("else_")) {
            return kb.ast.if_(resolveNode(ast["condition"]), resolveNode(ast["then"]), resolveNode(ast["else_"]));
        } else {
            return kb.ast.if_(resolveNode(ast["condition"]), resolveNode(ast["then"]));
        }
    } else if (&ast.type() == &kb.type.ast.Do) {
        return kb.ast.do_(resolveNode(ast["statement"]), resolveNode(ast["condition"]));
    } else if (&ast.type() == &kb.type.ast.While) {
        return kb.ast.while_(resolveNode(ast["condition"]), resolveNode(ast["statement"]));
    } else if (&ast.type() == &kb.type.ast.And) {
        return kb.ast.and_(resolveNode(ast["lhs"]), resolveNode(ast["rhs"]));
    } else if (&ast.type() == &kb.type.ast.Or) {
        return kb.ast.or_(resolveNode(ast["lhs"]), resolveNode(ast["rhs"]));
    } else if (&ast.type() == &kb.type.ast.Not) {
        return kb.ast.not_(resolveNode(ast["input"]));
    } else if (&ast.type() == &kb.type.ast.Add) {
        return kb.ast.add(resolveNode(ast["lhs"]), resolveNode(ast["rhs"]));
    } else if (&ast.type() == &kb.type.ast.Subtract) {
        return kb.ast.subtract(resolveNode(ast["lhs"]), resolveNode(ast["rhs"]));
    } else if (&ast.type() == &kb.type.ast.Multiply) {
        return kb.ast.multiply(resolveNode(ast["lhs"]), resolveNode(ast["rhs"]));
    } else if (&ast.type() == &kb.type.ast.Divide) {
        return kb.ast.divide(resolveNode(ast["lhs"]), resolveNode(ast["rhs"]));
    } else if (&ast.type() == &kb.type.ast.LessThan) {
        return kb.ast.lessThan(resolveNode(ast["lhs"]), resolveNode(ast["rhs"]));
    } else if (&ast.type() == &kb.type.ast.LessThanOrEqual) {
        return kb.ast.lessThanOrEqual(resolveNode(ast["lhs"]), resolveNode(ast["rhs"]));
    } else if (&ast.type() == &kb.type.ast.GreaterThan) {
        return kb.ast.greaterThan(resolveNode(ast["lhs"]), resolveNode(ast["rhs"]));
    } else if (&ast.type() == &kb.type.ast.GreaterThanOrEqual) {
        return kb.ast.greaterThanOrEqual(resolveNode(ast["lhs"]), resolveNode(ast["rhs"]));
    } else if (&ast.type() == &kb.type.ast.Same) {
        return kb.ast.same(resolveNode(ast["lhs"]), resolveNode(ast["rhs"]));
    } else if (&ast.type() == &kb.type.ast.NotSame) {
        return kb.ast.notSame(resolveNode(ast["lhs"]), resolveNode(ast["rhs"]));
    } else if (&ast.type() == &kb.type.ast.Equal) {
        return kb.ast.equal(resolveNode(ast["lhs"]), resolveNode(ast["rhs"]));
    } else if (&ast.type() == &kb.type.ast.NotEqual) {
        return kb.ast.notEqual(resolveNode(ast["lhs"]), resolveNode(ast["rhs"]));
    } else if (&ast.type() == &kb.type.ast.Has) {
        return kb.ast.has(resolveNode(ast["cell"]), resolveNode(ast["role"]));
    } else if (&ast.type() == &kb.type.ast.Missing) {
        return kb.ast.missing(resolveNode(ast["cell"]), resolveNode(ast["role"]));
    } else if (&ast.type() == &kb.type.ast.Get) {
        return kb.ast.get(resolveNode(ast["cell"]), resolveNode(ast["role"]));
    } else if (&ast.type() == &kb.type.ast.Member) {
        return kb.ast.member(ast["role"]);
    } else if (&ast.type() == &kb.type.ast.Return) {
        if (ast.has("value")) {
            return kb.ast.return_(static_cast<Base&>(resolveNode(ast["value"])));
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
    functionType.set("slots", kb.type.op.Function["slots"]);

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
                iss << "p_" << slot["slotRole"].label() << ": " << slot["slotType"].label();
            });
        }
    }
    if (has("returnType")) {
        oss << returnType().label();
    }
    if (has("returnType")) {
        return std::format("fn {}({}) -> {}", get("name").label(), iss.str(), oss.str());
    } else {
        return std::format("fn {}({})", get("name").label(), iss.str());
    }
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
            var.set("objectType", type);
            slots.add(kb.id("self"), kb.type.slot("self", type));
            structTypeStr = std::format("{}::", type.label());
        }
        if (has("parameters")) {
            Visitor::visitList(parameters(), [this, &slots, &iss](CellI& slot, int i, bool& stop) {
                if (i > 0) {
                    iss << ", ";
                }
                iss << "p_" << slot["slotRole"].label() << ": " << slot["slotType"].label();
                slots.add(slot["slotRole"], kb.type.slot(slot["slotRole"], slot["slotType"]));
            });
        }
        parametersType.set("slots", slots);
        subTypesMap.add(kb.id("parameters"), parametersType);
    }
    if (has("returnType")) {
        oss << returnType().label();
        subTypesMap.add(kb.id("returnType"), returnType());
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
        CellI& list        = ast["asts"];
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
        constVar.set("value", ast["value"]);
        return constVar;
    } else if (&ast.type() == &kb.type.ast.StructRef) {
        Object& constVar = *new Object(kb, kb.type.op.ConstVar);
        constVar.set("ast", ast);
        constVar.set("value", resolveType(ast, state));
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
        CellI& retOp = compile(kb.ast.get(_(function), _("stack")) / "value" / "input" / _(ast["role"]));
        retOp.set("ast", ast);
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Var) {
        if (function.type()["subTypes"]["index"].missing("localVars")) {
            cells::Object& functionLocalVarsType = *new cells::Object(kb, kb.type.Type_, std::format("LocalVarsType of {}", function.label()));
            functionLocalVarsType.set("memberOf", kb.map(kb.type.Type_, kb.type.Type_, kb.type.Index, kb.type.Index));
            static_cast<Map&>(function.type()["subTypes"]).add(kb.id("localVars"), functionLocalVarsType);
        }
        CellI& localVarsType = function.type()["subTypes"]["index"]["localVars"]["value"];
        if (localVarsType.missing("slots")) {
            localVarsType.set("slots", *new Map(kb, kb.type.Cell, kb.type.Slot));
        }
        auto& slotsMap = static_cast<Map&>(localVarsType["slots"]);
        if (!slotsMap.hasKey(ast["role"])) {
            slotsMap.add(ast["role"], kb.type.slot(ast["role"], kb.type.op.Var));
        }
        CellI& retOp = compile(kb.ast.get(_(function), _("stack")) / "value" / "localVars" / _(ast["role"]));
        retOp.set("ast", ast);
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Delete) {
        Object& retOp = *new Object(kb, kb.type.op.Delete);
        retOp.set("ast", ast);
        retOp.set("input", compile(ast["cell"]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Set) {
        Object& retOp = *new Object(kb, kb.type.op.Set);
        retOp.set("ast", ast);
        retOp.set("cell", compile(ast["cell"]));
        retOp.set("role", compile(ast["role"]));
        retOp.set("value", compile(ast["value"]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Erase) {
        Object& retOp = *new Object(kb, kb.type.op.Erase);
        retOp.set("ast", ast);
        retOp.set("cell", compile(ast["cell"]));
        retOp.set("role", compile(ast["role"]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.If) {
        Object& retOp = *new Object(kb, kb.type.op.If);
        retOp.set("ast", ast);
        retOp.set("condition", compile(ast["condition"]));
        retOp.set("then", compile(ast["then"]));
        if (ast.has("else_")) {
            retOp.set("else_", compile(ast["else_"]));
            return retOp;
        } else {
            return retOp;
        }
    } else if (&ast.type() == &kb.type.ast.Do) {
        Object& retOp = *new Object(kb, kb.type.op.Do);
        retOp.set("ast", ast);
        retOp.set("condition", compile(ast["condition"]));
        retOp.set("statement", compile(ast["statement"]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.While) {
        Object& retOp = *new Object(kb, kb.type.op.While);
        retOp.set("ast", ast);
        retOp.set("condition", compile(ast["condition"]));
        retOp.set("statement", compile(ast["statement"]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.New) {
        auto& compiledAsts = *new cells::List(kb, kb.type.op.Base);

        Object& block = *new Object(kb, kb.type.op.Block);
        block.set("ast", ast);
        block.set("ops", compiledAsts);
        Object& opSet = *new Object(kb, kb.type.op.Set, "New { block.value = new objectType(); }");
        opSet.set("ast", ast);
        opSet.set("cell", compile(kb.ast.cell(block)));
        opSet.set("role", compile(kb.ast.cell(kb.id("value"))));
        Object& opNew = *new Object(kb, kb.type.op.New);
        opNew.set("ast", ast);
        opNew.set("objectType", compile(ast["objectType"]));
        opSet.set("value", opNew);

        compiledAsts.add(opSet);
        if (ast.has("constructor")) {
            Object& callAst = *new Object(kb, kb.type.ast.Call);
            callAst.set("cell", kb.ast.get(kb.ast.cell(block), kb.ast.cell(kb.id("value"))));
            callAst.set("method", ast["constructor"]);
            if (ast.has("parameters")) {
                callAst.set("parameters", ast["parameters"]);
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
        Ast::Base& astCell     = static_cast<Ast::Base&>(ast["cell"]);
        Ast::Base& astMethod   = static_cast<Ast::Base&>(ast["method"]);
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
        varMethod.set("objectType", kb.type.op.Function);
        CellI& storeMethod     = compile(kb.ast.set(_(varMethod), _("value"), getMethod));
        CellI& setCurrentStack = compile(kb.ast.set(_(varMethod) / "value", _("stack"), kb.ast.get(_(function), _("stack"))));

        Object& varNewStackItem = *new Object(kb, kb.type.op.Var, "Call { var_newStackItem; }");
        varNewStackItem.set("objectType", kb.type.ListItem);
        Object& varNewStackFrame = *new Object(kb, kb.type.op.Var, "Call { var_newStackFrame; }");
        varNewStackFrame.set("objectType", kb.type.StackFrame);

        Object& varInputIndex = *new Object(kb, kb.type.Index, "Call { var_inputIndex; }");
        Object& varLocalVars = *new Object(kb, kb.type.op.Var, "Call { var_localVars; }");
        varLocalVars.set("objectType", kb.type.Index);
        Object& varLocalVarsList = *new Object(kb, kb.type.op.Var, "Call { var_localVarsList; }");
        varLocalVarsList.set("objectType", kb.type.List);
        Object& varLocalVarsListItem = *new Object(kb, kb.type.op.Var, "Call { var_localVarsListItem; }");
        varLocalVarsListItem.set("objectType", kb.type.ListItem);
        Object& varLocalVarsListItemHasNext = *new Object(kb, kb.type.op.Var, "Call { var_localVarsListItemHasNext; }");
        varLocalVarsListItemHasNext.set("objectType", kb.type.Boolean);

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
                                                               kb.ast.set(_(varNewStackFrame) / "value" / "output", _("objectType"), _(varMethod) / "value" / "type" / "subTypes" / "index" / "returnType" / "value"))));
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
            Visitor::visitList(ast["parameters"], [this, &compiledAsts, &compile, &varInputIndex, &_](CellI& param, int, bool&) {
                Ast::Base& paramRole  = static_cast<Ast::Base&>(param["slotRole"]);
                Ast::Base& paramValue = static_cast<Ast::Base&>(param["slotType"]);
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
        retOp.set("lhs", compile(ast["lhs"]));
        retOp.set("rhs", compile(ast["rhs"]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Or) {
        Object& retOp = *new Object(kb, kb.type.op.Or);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast["lhs"]));
        retOp.set("rhs", compile(ast["rhs"]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Not) {
        Object& retOp = *new Object(kb, kb.type.op.Not);
        retOp.set("ast", ast);
        retOp.set("input", compile(ast["input"]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Add) {
        Object& retOp = *new Object(kb, kb.type.op.Add);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast["lhs"]));
        retOp.set("rhs", compile(ast["rhs"]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Subtract) {
        Object& retOp = *new Object(kb, kb.type.op.Subtract);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast["lhs"]));
        retOp.set("rhs", compile(ast["rhs"]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Multiply) {
        Object& retOp = *new Object(kb, kb.type.op.Multiply);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast["lhs"]));
        retOp.set("rhs", compile(ast["rhs"]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Divide) {
        Object& retOp = *new Object(kb, kb.type.op.Divide);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast["lhs"]));
        retOp.set("rhs", compile(ast["rhs"]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.LessThan) {
        Object& retOp = *new Object(kb, kb.type.op.LessThan);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast["lhs"]));
        retOp.set("rhs", compile(ast["rhs"]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.LessThanOrEqual) {
        Object& retOp = *new Object(kb, kb.type.op.LessThanOrEqual);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast["lhs"]));
        retOp.set("rhs", compile(ast["rhs"]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.GreaterThan) {
        Object& retOp = *new Object(kb, kb.type.op.GreaterThan);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast["lhs"]));
        retOp.set("rhs", compile(ast["rhs"]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.GreaterThanOrEqual) {
        Object& retOp = *new Object(kb, kb.type.op.GreaterThanOrEqual);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast["lhs"]));
        retOp.set("rhs", compile(ast["rhs"]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Same) {
        Object& retOp = *new Object(kb, kb.type.op.Same);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast["lhs"]));
        retOp.set("rhs", compile(ast["rhs"]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.NotSame) {
        Object& retOp = *new Object(kb, kb.type.op.NotSame);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast["lhs"]));
        retOp.set("rhs", compile(ast["rhs"]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Equal) {
        Object& retOp = *new Object(kb, kb.type.op.Equal);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast["lhs"]));
        retOp.set("rhs", compile(ast["rhs"]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.NotEqual) {
        Object& retOp = *new Object(kb, kb.type.op.NotEqual);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast["lhs"]));
        retOp.set("rhs", compile(ast["rhs"]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Has) {
        Object& retOp = *new Object(kb, kb.type.op.Has);
        retOp.set("ast", ast);
        retOp.set("cell", compile(ast["cell"]));
        retOp.set("role", compile(ast["role"]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Missing) {
        Object& retOp = *new Object(kb, kb.type.op.Missing);
        retOp.set("ast", ast);
        retOp.set("cell", compile(ast["cell"]));
        retOp.set("role", compile(ast["role"]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Get) {
        Object& retOp = *new Object(kb, kb.type.op.Get);
        retOp.set("ast", ast);
        retOp.set("cell", compile(ast["cell"]));
        retOp.set("role", compile(ast["role"]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Member) {
        CellI& member = compile(kb.ast.get(kb.ast.self(), kb.ast.cell(ast["role"])));
        member.set("ast", ast);
        return member;
    } else if (&ast.type() == &kb.type.ast.Return) {
        Object& retOp = *new Object(kb, kb.type.op.Return, "op.return");
        retOp.set("ast", ast);
        if (ast.has("value")) {
            retOp.set("result", compile(kb.ast.set(kb.ast.get(_(function), _("stack")) / "value" / "output", _("value"), static_cast<Ast::Base&>(ast["value"]))));
        }
        return retOp;
    }

    throw "Unknown function AST!";
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
    set("id", kb.pools.strings.get(name)["value"]);
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
    BaseT<Var>(kb, kb.type.ast.Var, std::format("var {}", role.label()))
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

Ast::SubType::SubType(brain::Brain& kb, CellI& id) :
    BaseT<SubType>(kb, kb.type.ast.SubType, "ast.subType")
{
    set("id", id);
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
    addParam(role, kb.ast.structRef(kb.id(type)));
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

Ast::StructRef& Ast::structRef(CellI& cell)
{
    return StructRef::New(kb, cell);
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
    std::vector<std::string> sliced;
    boost::algorithm::split_regex(sliced, idStr, boost::regex("::"));

    if (sliced.empty()) {
        throw "Invalid template ID!";
    }
    const auto& templateId = sliced.back();

    auto& ret = TemplatedType::New(kb, kb.id(templateId), kb.list(type));
    if (sliced.size() > 1) {
        auto& namespaceList = *new List(kb, kb.type.Cell, "namespaces");
        ret.set("scopes", namespaceList);
        for (int i = 0; i < sliced.size() - 1; ++i) {
            const auto& currentId = sliced[i];
            namespaceList.add(kb.id(currentId));
        }
    }

    return ret;
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
        { "color", kb.ids.color },
        { "condition", kb.ids.condition },
        { "constructor", kb.ids.constructor },
        { "continue_", kb.ids.continue_ },
        { "data", kb.ids.data },
        { "destructor", kb.ids.destructor },
        { "else_", kb.ids.else_ },
        { "emptyObject", kb.ids.emptyObject },
        { "first", kb.ids.first },
        { "height", kb.ids.height },
        { "index", kb.ids.index },
        { "input", kb.ids.input },
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
        { "result", kb.ids.result },
        { "returnType", kb.ids.returnType },
        { "rhs", kb.ids.rhs },
        { "role", kb.ids.role },
        { "rootNode", kb.ids.rootNode },
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
        { "subTypes", kb.ids.subTypes },
        { "then", kb.ids.then },
        { "type", kb.ids.type },
        { "value", kb.ids.value },
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

Ast::StructRef& Brain::struct_(const std::string& name)
{
    return ast.structRef(id(name));
}

void Brain::createStd()
{
    auto& stdScope = globalScope.addScope("std");

    /*
    Compiler steps:
    Resolve template related references in normal functions or structs:
      - where templated types is used, for example tt_("List", ids.objectType, _(type.Slot))
        it must be resolved to a StructRef, with id L,i,s,t,ids.objectType,type.Slot
      - create a shadow ast tree with subtituted nodes
      - create a list of candidates for template instantiation with method names
        instantiate structT without methods
        instantiate structT with listed method
      - templates are instantied to a dedicated place
    */

#pragma region ListItem
    auto& listItemStructT = stdScope.addStructT("ListItem");
    listItemStructT.templateParams(
        param("objectType", _(type.Type_)));
    listItemStructT.memberOf(
        _(type.ListItem));
    listItemStructT.subTypes(
        param("objectType", tp_("objectType")));
    listItemStructT.members(
        member("previous", tt_("ListItem", "objectType", tp_("objectType"))),
        member("next", tt_("ListItem", "objectType", tp_("objectType"))),
        member("value", tp_("objectType")));

    auto& listItemConstructor = listItemStructT.addMethod("constructor");
    listItemConstructor.parameters(
        param("value", tp_("objectType")));
    listItemConstructor.code(
        m_("value") = p_("value"));
#pragma endregion
#pragma region List
    auto& listStructT = stdScope.addStructT("List");
    listStructT.templateParams(
        param("objectType", _(type.Type_)));
    listStructT.memberOf(
        _(type.Container),
        _(type.List));
    listStructT.subTypes(
        param("itemType", tt_("ListItem", "objectType", tp_("objectType"))),
        param("objectType", tp_("objectType")));
    listStructT.members(
        member("first", tt_("ListItem", "objectType", tp_("objectType"))),
        member("last", tt_("ListItem", "objectType", tp_("objectType"))),
        member("size", _(type.Number)));

    Ast::Function& listCtor = listStructT.addMethod("constructor");
    listCtor.code(
        m_("size") = _(_0_));

    Ast::Function& listAdd = listStructT.addMethod("add");
    listAdd.parameters(
        param("value", tp_("objectType")));
    listAdd.returnType(tt_("ListItem", "objectType", tp_("objectType")));
    listAdd.code(
        var_("item") = ast.new_(tt_("ListItem", "objectType", tp_("objectType")), "constructor", param("value", p_("value"))),
        ast.if_(ast.not_(m_("first").exist()),
                m_("first") = *var_("item"),                               // then
                ast.block(ast.set(m_("last"), _("next"), *var_("item")), // else
                          ast.set(*var_("item"), _("previous"), m_("last")))),
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
    Ast::Function& listRemove = listStructT.addMethod("remove");
    listRemove.parameters(
        param("item", _(type.Cell)));
    listRemove.code(
        ast.if_(ast.has(p_("item"), _("previous")),
                ast.if_(ast.has(p_("item"), _("next")),
                        ast.set(p_("item") / "previous", _("next"), p_("item") / "next"),
                        ast.erase(p_("item") / "previous", _("next"))),
                ast.if_(ast.has(p_("item"), _("next")),
                        m_("first") = p_("item") / "next",
                        ast.erase(ast.self(), _("first")))),
        ast.if_(ast.has(p_("item"), _("next")),
                ast.if_(ast.has(p_("item"), _("previous")),
                        ast.set(p_("item") / "next", _("previous"), p_("item") / "previous"),
                        ast.erase(p_("item") / "next", _("previous"))),
                ast.if_(ast.has(p_("item"), _("previous")),
                        m_("last") = p_("item") / "previous",
                        ast.erase(ast.self(), _("last")))),
        m_("size") = ast.subtract(m_("size"), _(_1_)));

    Ast::Function& listSize = listStructT.addMethod("size");
    listSize.returnType(_(type.Number));
    listSize.code(
        ast.return_(m_("size")));

    Ast::Function& listEmpty = listStructT.addMethod("empty");
    listEmpty.returnType(_(type.Boolean));
    listEmpty.code(
        ast.if_(ast.equal(m_("size"), _(_0_)), ast.return_(_(boolean.true_)), ast.return_(_(boolean.false_))));
#pragma endregion
#pragma region Type
    auto& typeStruct = stdScope.addStruct("Type");
    typeStruct.members(
        member("slots", tt_("Map", "keyType", _(type.Cell), "objectType", _(type.Slot))),
        member("incomplete", _(type.Boolean)),
        member("sharedObject", _(type.Slot)),
        member("subTypes", tt_("Map", "keyType", _(type.Cell), "objectType", "Type")),
        member("memberOf", tt_("Map", "keyType", "Type", "objectType", "Type")),
        member("asts", tt_("Map", "keyType", _(type.Cell), "objectType", _(type.ast.Function))),
        member("methods", tt_("Map", "keyType", _(type.Cell), "objectType", _(type.op.Function))));

    Ast::Function& typeCtor = typeStruct.addMethod("constructor");
    typeCtor.code(
        m_("slots") = ast.new_(tt_("Map", "keyType", _(type.Cell), "objectType", _(type.Slot)), "constructor"));

    Ast::Function& typeCtorWithRecursiveType = typeStruct.addMethod("constructorWithRecursiveType");
    typeCtorWithRecursiveType.code(
        m_("slots") = ast.new_(tt_("Map", "keyType", _(type.Cell), "objectType", _(type.Slot)), "constructorWithIndexType", param("indexType", ast.self())));

    Ast::Function& typeAddSubType = typeStruct.addMethod("addSubType");
    typeAddSubType.parameters(
        param("slotRole", _(type.Cell)),
        param("slotType", _(type.Type_)));
    typeAddSubType.code(
        ast.if_(m_("subTypes").missing(), m_("subTypes") = ast.new_(tt_("Map", "keyType", _(type.Cell), "objectType", "Type"), "constructor")),
        m_("subTypes").call("add", param("key", p_("slotRole")), param("value", p_("slotType"))));

    Ast::Function& typeAddMembership = typeStruct.addMethod("addMembership");
    typeAddMembership.parameters(
        param("cell", _(type.Type_)));
    typeAddMembership.code(
        ast.if_(m_("memberOf").missing(), m_("memberOf") = ast.new_(tt_("Map", "keyType", "Type", "objectType", "Type"), "constructor")),
        m_("memberOf").call("add", param("key", p_("cell")), param("value", p_("cell"))));

    Ast::Function& typeAddSlot = typeStruct.addMethod("addSlot");
    typeAddSlot.parameters(
        param("slotRole", _(type.Cell)),
        param("slotType", _(type.Slot)));
    typeAddSlot.code(
        ast.if_(m_("slots").missing(), m_("slots") = ast.new_(tt_("Map", "keyType", _(type.Cell), "objectType", _(type.Slot)), "constructor")),
        var_("slot") = ast.new_(_(type.Slot)),
        ast.set(*var_("slot"), _("slotRole"), p_("slotRole")),
        ast.set(*var_("slot"), _("slotType"), p_("slotType")),
        m_("slots").call("add", param("key", p_("slotRole")), param("value", *var_("slot"))));

    Ast::Function& typeAddSlots = typeStruct.addMethod("addSlots");
    typeAddSlots.parameters(
        param("list", tt_("List", "objectType", _(type.Slot))));
    typeAddSlots.code(
        ast.if_(ast.equal(p_("list") / "size", _(_0_)), ast.return_()),
        var_("item") = p_("list") / "first",
        ast.if_(m_("slots").missing(), m_("slots") = ast.new_(tt_("Map", "keyType", _(type.Cell), "objectType", _(type.Slot)), "constructor")),
        ast.do_(ast.block(
                    var_("next") = _(boolean.true_),
                    m_("slots").call("add", param("key", *var_("item") / "value" / "slotRole"), param("value", *var_("item") / "value")),
                    ast.if_(ast.has(*var_("item"), _("next")),
                            var_("item") = *var_("item") / "next",
                            var_("next") = _(boolean.false_))),
                ast.same(*var_("next"), _(boolean.true_))));

    Ast::Function& typeHasSlot = typeStruct.addMethod("hasSlot");
    typeHasSlot.parameters(
        param("slotRole", _(type.Cell)));
    typeHasSlot.returnType(_(type.Boolean));
    typeHasSlot.code(
        ast.if_(m_("slots").missing(), ast.return_(_(boolean.false_))),
        ast.return_(m_("slots").call("hasKey", param("key", p_("slotRole")))));

    Ast::Function& typeRemoveSlot = typeStruct.addMethod("removeSlot");
    typeRemoveSlot.parameters(
        param("slotRole", _(type.Cell)));
    typeRemoveSlot.code(
        ast.if_(m_("slots").missing(), ast.return_()),
        m_("slots").call("remove", param("key", p_("slotRole"))));
#pragma endregion
#pragma region Index
    auto& indexStruct = stdScope.addStruct("Index");
    indexStruct.memberOf(_(type.Type_));

    Ast::Function& indexCtor = indexStruct.addMethod("constructor");
    indexCtor.code(
        ast.set(ast.self(), _("type"), ast.new_("Type", "constructorWithRecursiveType")),
        ast.set(m_("type"), _("methods"), ast.get(struct_("Index"), _("methods"))),
        ast.set(m_("type"), _("memberOf"), _(map(type.Type_, type.Type_, type.Index, type.Index))));

    Ast::Function& indexCtorWithSelfType = indexStruct.addMethod("constructorWithSelfType");
    indexCtorWithSelfType.parameters(
        param("indexType", _(type.Type_)));
    indexCtorWithSelfType.code(
        ast.if_(ast.missing(p_("indexType"), _("sharedObject")),
                ast.block(ast.set(p_("indexType"), _("sharedObject"), ast.new_(_(type.Slot))),
                          ast.set(p_("indexType") / "sharedObject", _("slotRole"), ast.self()),
                          ast.set(p_("indexType") / "sharedObject", _("slotType"), struct_("Index")))),
        ast.set(ast.self(), _("type"), p_("indexType")));

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
    Ast::Function& indexInsert = indexStruct.addMethod("insert");
    indexInsert.parameters(
        param("key", _(type.Cell)),
        param("value", _(type.Cell)));
    indexInsert.code(
        ast.if_(ast.same(p_("key"), _("type")), ast.return_()),
        ast.set(ast.self(), p_("key"), p_("value")),
        ast.if_(ast.and_(ast.has(m_("type"), _("sharedObject")), ast.same(m_("type") / "sharedObject" / "slotRole", ast.self())),
                ast.return_()),
        m_("type").call("addSlot", param("slotRole", p_("key")), param("slotType", _(type.Slot))));

    Ast::Function& indexEmpty = indexStruct.addMethod("empty");
    indexEmpty.returnType(_(type.Boolean));
    indexEmpty.code(
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
    Ast::Function& indexRemove = indexStruct.addMethod("remove");
    indexRemove.parameters(
        param("key", _(type.Cell)));
    indexRemove.code(
        ast.if_(ast.not_(m_("type").call("hasSlot", param("slotRole", p_("key")))),
                ast.return_()),
        ast.erase(ast.self(), p_("key")),
        m_("type").call("removeSlot", param("slotRole", p_("key"))));

    Ast::Function& indexSize = indexStruct.addMethod("size");
    indexSize.returnType(_(type.Number));
    indexSize.code(
        ast.return_(ast.call(m_("type") / "slots", "size")));
#pragma endregion
#pragma region Map
    auto& mapStructT = stdScope.addStructT("Map");
    mapStructT.templateParams(
        param("keyType", _(type.Type_)),
        param("objectType", _(type.Type_)));
    mapStructT.subTypes(
        param("keyType", tp_("keyType")),
        param("objectType", tp_("objectType")),
        param("listType", tt_("List", "objectType", tp_("objectType"))));
    mapStructT.memberOf(_(type.Container), _(type.Map));
    mapStructT.members(
        member("list", tt_("List", "objectType", tp_("objectType"))),
        member("index", struct_("Index")),
        member("size", _(type.Number)));

    Ast::Function& mapCtor = mapStructT.addMethod("constructor");
    mapCtor.code(
        m_("size")  = _(_0_),
        m_("list")  = ast.new_(tt_("List", "objectType", tp_("objectType")), "constructor"),
        m_("index") = ast.new_("Index", "constructor"));

    Ast::Function& mapCtorWithIndexType = mapStructT.addMethod("constructorWithIndexType");
    mapCtorWithIndexType.parameters(
        param("indexType", _(type.Type_)));
    mapCtorWithIndexType.code(
        m_("size")  = _(_0_),
        m_("list")  = ast.new_(tt_("List", "objectType", tp_("objectType")), "constructor"),
        m_("index") = ast.new_("Index", "constructorWithSelfType", param("indexType", p_("indexType"))));

    /*
    bool Map::hasKey(CellI& key)
    {
        return m_index.has(key);
    }
    */
    Ast::Function& mapHasKey = mapStructT.addMethod("hasKey");
    mapHasKey.parameters(
        param("key", tp_("keyType")));
    mapHasKey.returnType(_(type.Boolean));
    mapHasKey.code(
        ast.if_(ast.has(m_("index"), p_("key")), ast.return_(_(boolean.true_)), ast.return_(_(boolean.false_))));

    /*
    CellI& Map::getValue(CellI& key)
    {
        if (m_index.has(key)) {
            return m_index[key]["value"];
        }
        throw "No such role!";
    }
    */
    Ast::Function& mapGetValue = mapStructT.addMethod("getValue");
    mapGetValue.parameters(
        param("key", tp_("keyType")));
    mapGetValue.returnType(tp_("objectType"));
    mapGetValue.code(
        ast.if_(ast.has(m_("index"), p_("key")), ast.return_(m_("index") / p_("key") / "value"), ast.return_(_("emptyObject"))));

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
    Ast::Function& mapAdd = mapStructT.addMethod("add");
    mapAdd.parameters(
        param("key", tp_("keyType")),
        param("value", tp_("objectType")));
    mapAdd.code(
        ast.if_(ast.same(p_("key"), _("type")), ast.return_()),
        ast.if_(ast.has(m_("index"), p_("key")), ast.return_()),
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
    Ast::Function& mapRemove = mapStructT.addMethod("remove");
    mapRemove.parameters(
        param("key", tp_("keyType")));
    mapRemove.code(
        ast.if_(ast.missing(m_("index"), p_("key")),
                ast.return_()),
        m_("list").call("remove", param("item", m_("index") / p_("key"))),
        m_("index").call("remove", param("key", p_("key"))),
        m_("size") = ast.subtract(m_("size"), _(_1_)));

    Ast::Function& mapSize = mapStructT.addMethod("size");
    mapSize.returnType(_(type.Number));
    mapSize.code(
        ast.return_(m_("size")));

    Ast::Function& mapEmpty = mapStructT.addMethod("empty");
    mapEmpty.returnType(_(type.Boolean));
    mapEmpty.code(
        ast.if_(ast.equal(m_("size"), _(_0_)), ast.return_(_(boolean.true_)), ast.return_(_(boolean.false_))));
#pragma endregion
#pragma region Set
    auto& setStructT = stdScope.addStructT("Set");
    setStructT.templateParams(
        param("keyType", _(type.Type_)),
        param("objectType", _(type.Type_)));
    setStructT.subTypes(
        param("objectType", tp_("objectType")),
        param("listType", tt_("List", "objectType", tp_("objectType"))));
    setStructT.memberOf(_(type.Container));
    setStructT.members(
        member("list", tt_("List", "objectType", tp_("objectType"))),
        member("index", struct_("Index")),
        member("size", _(type.Number)));

    Ast::Function& setCtor = setStructT.addMethod("constructor");
    setCtor.code(
        m_("size")  = _(_0_),
        m_("list")  = ast.new_(tt_("List", "objectType", tp_("objectType")), "constructor"),
        m_("index") = ast.new_(struct_("Index"), "constructor"));

    Ast::Function& setAdd = setStructT.addMethod("add");
    setAdd.parameters(
        param("value", tp_("objectType")));
    setAdd.returnType(tt_("ListItem", "objectType", tp_("objectType")));
    setAdd.code(
        ast.if_(ast.has(m_("index"), p_("value")),
                ast.return_(m_("index") / p_("value"))),
        var_("listItem") = m_("list").call("add", param("value", p_("value"))),
        m_("size")     = ast.add(m_("size"), _(_1_)),
        ast.call(m_("indexType") / "slots", "add", param("key", p_("value")), param("value", *var_("listItem"))),
        ast.set(m_("index"), p_("value"), *var_("listItem")),
        ast.return_(*var_("listItem")));

    Ast::Function& setContains = setStructT.addMethod("contains");
    setContains.parameters(
        param("value", tp_("objectType")));
    setContains.returnType(_(type.Boolean));
    setContains.code(
        ast.if_(ast.has(m_("index"), p_("value")),
                ast.return_(_(boolean.true_)),
                ast.return_(_(boolean.false_))));

    Ast::Function& setErase = setStructT.addMethod("erase");
    setErase.parameters(
        param("value", tp_("objectType")));
    setErase.code(
        ast.if_(ast.missing(m_("index"), p_("value")),
                ast.return_()),
        ast.erase(m_("index"), p_("value")),
        m_("size") = ast.subtract(m_("size"), _(_1_)));

    Ast::Function& setSize = setStructT.addMethod("size");
    setSize.returnType(_(type.Number));
    setSize.code(
        ast.return_(m_("size")));

    Ast::Function& setEmpty = setStructT.addMethod("empty");
    setEmpty.returnType(_(type.Boolean));
    setEmpty.code(
        ast.if_(ast.equal(m_("size"), _(_0_)), ast.return_(_(boolean.true_)), ast.return_(_(boolean.false_))));
#pragma endregion
}

void Brain::createTests()
{
    auto& testScope = globalScope.addScope("test");

#if 0
    Object methodData(*this, type.Cell);
    methodData.set(ids.ast, listAdd);
    methodData.set(ids.op, listAdd.compile());
    methodData.set(ids.static_, boolean.false_);
    methodData.set(ids.const_, boolean.false_);
#endif

    auto& testStruct                 = testScope.addStruct("Test");
    auto& testCreateNewListOfNumbers = testStruct.addMethod("testCreateNewListOfNumbers");
    testCreateNewListOfNumbers.code(
        var_("result") = ast.new_(tt_("std::List", "objectType", _(type.Number))),
        var_("result") = ast.new_(tt_("std::List", "objectType", _(type.Cell))),
        var_("result") = ast.new_(tt_("std::List", "objectType", _(type.Pixel))),
        var_("result") = ast.new_(tt_("std::Set", "objectType", _(type.Number))),
        var_("result") = ast.new_(tt_("std::Map", "keyType", _(type.Number), "objectType", _(type.Color))));

    CellI* mapPtr = nullptr;
    mapPtr        = &slots(type.slot("value", type.ListOf(type.Digit)),
                           type.slot("sign", type.Number)); // TODO sign has no class currently
    type.Number.set("slots", *mapPtr);

    Ast::Function& numberFactorial = testStruct.addMethod("factorial");
    numberFactorial.parameters(
        param("input", _(type.Number)));
    numberFactorial.returnType(_(type.Number));
    numberFactorial.code(
        ast.if_(ast.greaterThanOrEqual(p_("input"), _(_1_)),
                ast.return_(ast.multiply(p_("input"), ast.self().call("factorial", param("input", ast.subtract(p_("input"), _(_1_)))))),
                ast.return_(_(_1_))));

    // TODO
//    type.String.method(ids.addSlots, { ids.list, list(type.slot(ids.value, type.ListOf(type.Char))) });
}

void Brain::createArcSolver()
{
    auto& arcScope = globalScope.addScope("arc");

    auto& colorStruct = arcScope.addStruct("Color");
    colorStruct.members(
        member("red", _(type.Number)),
        member("green", _(type.Number)),
        member("blue", _(type.Number)));

    auto& pixelStruct = arcScope.addStruct("Pixel");
    pixelStruct.members(
        member("x", _(type.Number)),
        member("y", _(type.Number)));

    Ast::Function& pixelCtor = pixelStruct.addMethod("constructor");
    pixelCtor.parameters(
        param("x", _(type.Number)),
        param("y", _(type.Number)));
    pixelCtor.code(
        m_("x") = p_("x"),
        m_("y") = p_("y"));

    auto& shapeStruct = arcScope.addStruct("Shape");
    shapeStruct.members(
        member("color", struct_("Color")),
        member("width", _(type.Number)),
        member("height", _(type.Number)),
        member("hybridPixels", tt_("std::Set", "objectType", _(type.Pixel))),
        member("pixels", tt_("std::List", "objectType", struct_("Pixel"))));

    auto& shaperStruct = arcScope.addStruct("Shaper");
    shaperStruct.members(
        member("width", _(type.Number)),
        member("height", _(type.Number)),
        member("picture", _(type.Picture)),
        member("shapes", tt_("std::List", "objectType", "Shape")),
        member("inputPixels", tt_("std::Set", "objectType", _(type.Pixel))));

    /*
    * class Shape
{
public:
    Shape(int id, input::Color color, int width, int height) :
        m_id(id), m_color(color), m_width(width), m_height(height) { }
    */
    Ast::Function& shapeCtor = shapeStruct.addMethod("constructor");
    shapeCtor.parameters(
        param("id", _(type.Number)),
        param("color", struct_("Color")),
        param("width", _(type.Number)),
        param("height", _(type.Number)));
    shapeCtor.code(
        m_("id")     = p_("id"),
        m_("color")  = p_("color"),
        m_("width")  = p_("width"),
        m_("height") = p_("height"));

    /*
    void Shape::addPixel(cells::hybrid::Pixel& pixel)
    {
        m_pixels.push_back({ pixel.m_x.value(), pixel.m_y.value() });
        m_hybridPixels.insert(&pixel);
    }
    */
    Ast::Function& shapeAddPixel = shapeStruct.addMethod("addPixel");
    shapeAddPixel.parameters(
        param("pixel", struct_("Pixel")));
    shapeAddPixel.code(
        m_("pixels").call("add", param("value", p_("pixel"))),
        m_("hybridPixels").call("add", param("value", p_("pixel"))));

    /*
    bool Shape::hasPixel(cells::hybrid::Pixel& pixel) const
    {
        return m_hybridPixels.find(&pixel) != m_hybridPixels.end();
    }
    */
    Ast::Function& shapeHasPixel = shapeStruct.addMethod("hasPixel");
    shapeHasPixel.parameters(
        param("pixel", _(type.Pixel)));
    shapeHasPixel.returnType(_(type.Boolean)); // maybe hybrid pixel?!
    shapeHasPixel.code(
        ast.return_(m_("hybridPixels").call("contains", param("value", p_("pixel")))));

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
    Ast::Function& shaperCtor = shaperStruct.addMethod("constructor");
    shaperCtor.parameters(
        param("width", _(type.Number)),
        param("height", _(type.Number)),
        param("picture", _(type.Picture)));
    shaperCtor.code(
        m_("width")   = p_("width"),
        m_("height")  = p_("height"),
        m_("picture") = p_("picture"),
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
    Ast::Function& shaperProcessInputPixels = shaperStruct.addMethod("processInputPixels");
    shaperProcessInputPixels.code(
        var_("pixels") = m_("picture") / "pixels",
        var_("pixel")  = *var_("pixels") / "first",
        ast.while_(ast.not_(ast.same(*var_("pixel"), *var_("pixels") / "last")),
                   ast.block(
                       m_("inputPixels").call("add", param("value", *var_("pixel"))),
                       var_("pixel") = *var_("pixels") / "next")),
        ast.if_(ast.same(*var_("pixel"), *var_("pixels") / "last"), m_("inputPixels").call("add", param("value", *var_("pixel")))));

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
    Ast::Function& shaperProcess = shaperStruct.addMethod("process");
    shaperProcess.code(
        var_("shapeId") = _(_1_),
        ast.while_(ast.not_(m_("inputPixels").call("empty")),
                   ast.block(
                       var_("firstPixel") = m_("inputPixels").call("first", param("pixel", p_("checkPixel"))), // TODO
                       m_("shapes").call("add", param("value", ast.new_("Shape", "constructor", param("id", *var_("shapeId")), param("color", *var_("firstPixel") / "color"), param("width", m_("width")), param("height", m_("height"))))),
                       var_("shape")       = m_("shapes").call("last"),
                       var_("checkPixels") = ast.new_(tt_("std::List", "objectType", "Pixel"), "constructor"),
                       ast.call(*var_("checkPixels"), "add", param("value", *var_("firstPixel"))),
                       ast.while_(ast.not_(ast.call(*var_("checkPixels"), _("empty"))),
                                  ast.block(
                                      var_("checkPixel") = ast.call(*var_("checkPixels"), "first"),
                                      ast.self().call("processPixel", param("shape", *var_("shape")), param("checkPixels", *var_("checkPixels")), param("checkPixel", *var_("checkPixel"))),
                                      ast.call(*var_("checkPixels"), "erase", param("value", *var_("firstPixel"))))),
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
    Ast::Function& shaperProcessPixel = shaperStruct.addMethod("processPixel");
    shaperProcessPixel.parameters(
        param("shape", struct_("Shape")),
        param("checkPixels", tt_("std::Set", "objectType", "Pixel")),
        param("checkPixel", struct_("Pixel")));
    shaperProcessPixel.code(
        ast.call(p_("shape"), "addPixel", param("pixel", p_("checkPixel"))),
        m_("inputPixels").call("erase", param("pixel", p_("checkPixel"))),
        ast.if_(var_("pixel") = ast.self().call("processAdjacentPixel", param("direction", _(directions.up)), param("shape", p_("shape")), param("checkPixels", p_("checkPixels")), param("checkPixel", p_("checkPixel"))),
                ast.block(
                    ast.self().call("processAdjacentPixel", param("direction", _(directions.left)), param("shape", p_("shape")), param("checkPixels", p_("checkPixels")), param("checkPixel", *var_("pixel"))),
                    ast.self().call("processAdjacentPixel", param("direction", _(directions.right)), param("shape", p_("shape")), param("checkPixels", p_("checkPixels")), param("checkPixel", *var_("pixel"))))),
        ast.if_(var_("pixel") = ast.self().call("processAdjacentPixel", param("direction", _(directions.down)), param("shape", p_("shape")), param("checkPixels", p_("checkPixels")), param("checkPixel", p_("checkPixel"))),
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
    Ast::Function& shaperProcessAdjacentPixel = shaperStruct.addMethod("processAdjacentPixel");
    shaperProcessAdjacentPixel.parameters(
        param("direction", _(type.Directions)),
        param("shape", struct_("Shape")),
        param("checkPixels", tt_("std::List", "objectType", "Pixel")),
        param("checkPixel", struct_("Pixel")));
    shaperProcessAdjacentPixel.returnType(struct_("Pixel")); // maybe hybrid pixel?!
    shaperProcessAdjacentPixel.code(
        ast.if_(ast.call(*var_("checkPixel"), "has", param("direction", p_("direction"))),
                ast.block(
                    var_("pixel") = _("checkPixel") / _("direction"),
                    ast.if_(ast.and_(ast.same(*var_("pixel") / "color", *var_("shape") / "color"), ast.not_(ast.call(*var_("shape"), "hasPixel", param("pixel", *var_("pixel"))))),
                            ast.call(*var_("checkPixels"), "insert", param("pixel", *var_("pixel")))),
                    ast.return_(_("pixel"))),
                ast.return_(_("emptyObject"))));
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
    createTests();
    createArcSolver();

    auto& compiledGlobalScope = globalScope.compile();
    compiledGlobalScopePtr    = &compiledGlobalScope["data"];
    m_initPhase               = InitPhase::FullyConstructed;

    // Test should be removed from here
    auto& compiledStructs        = static_cast<TrieMap&>(compiledGlobalScope["data"]["structs"]);
    auto& compiledListItemStruct = static_cast<TrieMap&>(compiledGlobalScope["data"]["structs"]).getValue(templateId("ListItem", id("objectType"), type.Cell));
    auto& compiledListStruct     = static_cast<TrieMap&>(compiledGlobalScope["data"]["structs"]).getValue(templateId("List", id("objectType"), type.Cell));
    auto& compiledTypeStruct     = static_cast<TrieMap&>(compiledGlobalScope["data"]["structs"]).getValue(id("Type"));
    auto& compiledIndexStruct    = static_cast<TrieMap&>(compiledGlobalScope["data"]["structs"]).getValue(id("Index"));
    type.ListItem.set("methods", compiledListItemStruct["methods"]);
    type.List.set("methods", compiledListStruct["methods"]);
    type.Type_.set("methods", compiledTypeStruct["methods"]);

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
