#include "Brain.h"

#include <format>
#include <sstream>

namespace synth {
namespace cells {
namespace brain {

ID::ID(brain::Brain& kb) :
    kb(kb),
    add(kb, kb.type.Cell, "add"),
    addMembership(kb, kb.type.Cell, "addMembership"),
    addMethod(kb, kb.type.Cell, "addMethod"),
    addPixel(kb, kb.type.Struct, "addPixel"),
    addSlot(kb, kb.type.Cell, "addSlot"),
    addSlots(kb, kb.type.Cell, "addSlots"),
    addSubType(kb, kb.type.Cell, "addSubType"),
    argument(kb, kb.type.Cell, "argument"),
    ast(kb, kb.type.Cell, "ast"),
    asts(kb, kb.type.Cell, "asts"),
    branch(kb, kb.type.Cell, "branch"),
    cell(kb, kb.type.Cell, "cell"),
    checkPixel(kb, kb.type.Struct, "checkPixel"),
    checkPixels(kb, kb.type.Struct, "checkPixels"),
    color(kb, kb.type.Cell, "color"),
    condition(kb, kb.type.Cell, "condition"),
    constructor(kb, kb.type.Cell, "constructor"),
    constructorWithIndexType(kb, kb.type.Cell, "constructorWithIndexType"),
    constructorWithRecursiveType(kb, kb.type.Cell, "constructorWithRecursiveType"),
    constructorWithSelfType(kb, kb.type.Cell, "constructorWithSelfType"),
    container(kb, kb.type.Cell, "container"),
    contains(kb, kb.type.Cell, "contains"),
    continue_(kb, kb.type.Cell, "continue"),
    current(kb, kb.type.Cell, "current"),
    destructor(kb, kb.type.Cell, "destructor"),
    direction(kb, kb.type.Struct, "direction"),
    else_(kb, kb.type.Cell, "else_"),
    empty(kb, kb.type.Cell, "empty"),
    emptyObject(kb, kb.type.Cell, "emptyObject"),
    erase(kb, kb.type.Cell, "erase"),
    eval(kb, kb.type.Cell, "eval"),
    first(kb, kb.type.Cell, "first"),
    firstPixel(kb, kb.type.Struct, "firstPixel"),
    getValue(kb, kb.type.Cell, "getValue"),
    has(kb, kb.type.Struct, "has"),
    hasKey(kb, kb.type.Cell, "hasKey"),
    hasPixel(kb, kb.type.Struct, "hasPixel"),
    hasSlot(kb, kb.type.Cell, "hasSlot"),
    height(kb, kb.type.Cell, "height"),
    id(kb, kb.type.Struct, "id"),
    index(kb, kb.type.Cell, "index"),
    indexType(kb, kb.type.Cell, "indexType"),
    input(kb, kb.type.Cell, "input"),
    inputPixels(kb, kb.type.Struct, "inputPixels"),
    insert(kb, kb.type.Struct, "insert"),
    item(kb, kb.type.Cell, "item"),
    itemType(kb, kb.type.Cell, "itemType"),
    key(kb, kb.type.Cell, "key"),
    keyType(kb, kb.type.Cell, "keyType"),
    label(kb, kb.type.Cell, "label"),
    last(kb, kb.type.Cell, "last"),
    lhs(kb, kb.type.Cell, "lhs"),
    list(kb, kb.type.Cell, "list"),
    listItem(kb, kb.type.Cell, "listItem"),
    listType(kb, kb.type.Cell, "listType"),
    localVars(kb, kb.type.Cell, "localVars"),
    memberOf(kb, kb.type.Cell, "memberOf"),
    members(kb, kb.type.Cell, "members"),
    method(kb, kb.type.Cell, "method"),
    methods(kb, kb.type.Cell, "methods"),
    name(kb, kb.type.Cell, "name"),
    next(kb, kb.type.Cell, "next"),
    objectType(kb, kb.type.Cell, "objectType"),
    op(kb, kb.type.Cell, "op"),
    ops(kb, kb.type.Cell, "ops"),
    output(kb, kb.type.Cell, "output"),
    parameter(kb, kb.type.Cell, "parameter"),
    parameters(kb, kb.type.Cell, "parameters"),
    picture(kb, kb.type.Struct, "picture"),
    pixel(kb, kb.type.Struct, "pixel"),
    pixels(kb, kb.type.Struct, "pixels"),
    previous(kb, kb.type.Cell, "previous"),
    process(kb, kb.type.Struct, "process"),
    processAdjacentPixel(kb, kb.type.Struct, "processAdjacentPixel"),
    processInputPixels(kb, kb.type.Struct, "processInputPixels"),
    processPixel(kb, kb.type.Struct, "processPixel"),
    recursiveType(kb, kb.type.Struct, "recursiveType"),
    remove(kb, kb.type.Cell, "remove"),
    removeSlot(kb, kb.type.Cell, "removeSlot"),
    result(kb, kb.type.Cell, "result"),
    rhs(kb, kb.type.Cell, "rhs"),
    role(kb, kb.type.Cell, "role"),
    self(kb, kb.type.Cell, "self"),
    shape(kb, kb.type.Struct, "shape"),
    shapeId(kb, kb.type.Struct, "shapeId"),
    shapes(kb, kb.type.Struct, "shapes"),
    sharedObject(kb, kb.type.Cell, "sharedObject"),
    size(kb, kb.type.Cell, "size"),
    slot(kb, kb.type.Cell, "slot"),
    slotRole(kb, kb.type.Cell, "slotRole"),
    slots(kb, kb.type.Cell, "slots"),
    slotType(kb, kb.type.Cell, "slotType"),
    sortPixels(kb, kb.type.Struct, "sortPixels"),
    stack(kb, kb.type.Cell, "stack"),
    statement(kb, kb.type.Cell, "statement"),
    static_(kb, kb.type.Cell, "static"),
    status(kb, kb.type.Cell, "status"),
    stop(kb, kb.type.Cell, "stop"),
    subTypes(kb, kb.type.Cell, "subTypes"),
    template_(kb, kb.type.Cell, "template"),
    then(kb, kb.type.Cell, "then"),
    type(kb, kb.type.Cell, "type"),
    value(kb, kb.type.Cell, "value"),
    width(kb, kb.type.Cell, "width")
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
    auto& id   = kb.id;
    auto& type = kb.type;
    auto& ast  = kb.type.ast;

    CellI* map = nullptr;

    map = &kb.slots(type.slot(id.ast, ast.Base),
                    type.slot(id.lhs, Base),
                    type.slot(id.rhs, Base),
                    type.slot(id.value, type.Number));
    Add.set(id.slots, *map);

    map = &kb.slots(type.slot(id.ast, ast.Base),
                    type.slot(id.lhs, Base),
                    type.slot(id.rhs, Base),
                    type.slot(id.value, type.Boolean));
    And.set(id.slots, *map);

    map = &kb.slots(type.slot(id.ast, ast.Base),
                    type.slot(id.status, type.Cell),
                    type.slot(id.ops, type.Cell),
                    type.slot(id.value, type.Cell));
    Block.set(id.slots, *map);

    map = &kb.slots(type.slot(id.ast, ast.Base),
                    type.slot(id.value, type.Cell));
    ConstVar.set(id.slots, *map);

    map = &kb.slots(type.slot(id.ast, ast.Base),
                    type.slot(id.input, Base));
    Delete.set(id.slots, *map);

    map = &kb.slots(type.slot(id.ast, ast.Base),
                    type.slot(id.lhs, Base),
                    type.slot(id.rhs, Base),
                    type.slot(id.value, type.Number));
    Divide.set(id.slots, *map);

    map = &kb.slots(type.slot(id.ast, ast.Base),
                    type.slot(id.status, type.Cell),
                    type.slot(id.condition, Base),
                    type.slot(id.statement, Base));
    Do.set(id.slots, *map);

    map = &kb.slots(type.slot(id.ast, ast.Base),
                    type.slot(id.lhs, Base),
                    type.slot(id.rhs, Base),
                    type.slot(id.value, type.Boolean));
    Equal.set(id.slots, *map);

    map = &kb.slots(type.slot(id.ast, ast.Base),
                    type.slot(id.cell, Base),
                    type.slot(id.role, Base),
                    type.slot(id.value, Base));
    Erase.set(id.slots, *map);

    map = &kb.slots(type.slot(id.ast, ast.Base),
                    type.slot(id.value, Var));
    EvalVar.set(id.slots, *map);

    map = &kb.slots(type.slot(id.ast, ast.Base),
                    type.slot(id.stack, type.Stack),
                    type.slot(id.op, type.ListOf(Base)),
                    type.slot(id.static_, type.Boolean));
    Function.set(id.slots, *map);

    map = &kb.slots(type.slot(id.ast, ast.Base),
                    type.slot(id.cell, Base),
                    type.slot(id.role, Base),
                    type.slot(id.value, type.Cell));
    Get.set(id.slots, *map);

    map = &kb.slots(type.slot(id.ast, ast.Base),
                    type.slot(id.lhs, Base),
                    type.slot(id.rhs, Base),
                    type.slot(id.value, type.Boolean));
    GreaterThan.set(id.slots, *map);

    map = &kb.slots(type.slot(id.ast, ast.Base),
                    type.slot(id.lhs, Base),
                    type.slot(id.rhs, Base),
                    type.slot(id.value, type.Boolean));
    GreaterThanOrEqual.set(id.slots, *map);

    map = &kb.slots(type.slot(id.ast, ast.Base),
                    type.slot(id.cell, Base),
                    type.slot(id.role, Base),
                    type.slot(id.value, type.Boolean));
    Has.set(id.slots, *map);

    map = &kb.slots(type.slot(id.ast, ast.Base),
                    type.slot(id.status, type.Cell),
                    type.slot(id.condition, Base),
                    type.slot(id.then, Base),
                    type.slot(id.else_, Base));
    If.set(id.slots, *map);

    map = &kb.slots(type.slot(id.ast, ast.Base),
                    type.slot(id.lhs, Base),
                    type.slot(id.rhs, Base),
                    type.slot(id.value, type.Boolean));
    LessThan.set(id.slots, *map);

    map = &kb.slots(type.slot(id.ast, ast.Base),
                    type.slot(id.lhs, Base),
                    type.slot(id.rhs, Base),
                    type.slot(id.value, type.Boolean));
    LessThanOrEqual.set(id.slots, *map);

    map = &kb.slots(type.slot(id.ast, ast.Base),
                    type.slot(id.cell, Base),
                    type.slot(id.role, Base),
                    type.slot(id.value, type.Boolean));
    Missing.set(id.slots, *map);

    map = &kb.slots(type.slot(id.ast, ast.Base),
                    type.slot(id.lhs, Base),
                    type.slot(id.rhs, Base),
                    type.slot(id.value, type.Number));
    Multiply.set(id.slots, *map);

    map = &kb.slots(type.slot(id.ast, ast.Base),
                    type.slot(id.value, type.Cell),
                    type.slot(id.objectType, Base));
    New.set(id.slots, *map);

    map = &kb.slots(type.slot(id.ast, ast.Base),
                    type.slot(id.input, Base),
                    type.slot(id.value, type.Boolean));
    Not.set(id.slots, *map);

    map = &kb.slots(type.slot(id.ast, ast.Base),
                    type.slot(id.lhs, Base),
                    type.slot(id.rhs, Base),
                    type.slot(id.value, type.Boolean));
    NotEqual.set(id.slots, *map);

    map = &kb.slots(type.slot(id.ast, ast.Base),
                    type.slot(id.lhs, Base),
                    type.slot(id.rhs, Base),
                    type.slot(id.value, type.Boolean));
    NotSame.set(kb.id.slots, *map);

    map = &kb.slots(type.slot(id.ast, ast.Base),
                    type.slot(id.lhs, Base),
                    type.slot(id.rhs, Base),
                    type.slot(id.value, type.Boolean));
    Or.set(id.slots, *map);

    map = &kb.slots(type.slot(id.ast, ast.Base),
                    type.slot(id.result, ast.Base));
    Return.set(kb.id.slots, *map);

    map = &kb.slots(type.slot(id.ast, ast.Base),
                    type.slot(id.lhs, Base),
                    type.slot(id.rhs, Base),
                    type.slot(id.value, type.Boolean));
    Same.set(kb.id.slots, *map);

    map = &kb.slots(type.slot(id.ast, ast.Base),
                    type.slot(id.cell, Base),
                    type.slot(id.role, Base),
                    type.slot(id.value, Base));
    Set.set(id.slots, *map);

    map = &kb.slots(type.slot(id.ast, ast.Base),
                    type.slot(id.lhs, Base),
                    type.slot(id.rhs, Base),
                    type.slot(id.value, type.Number));
    Subtract.set(id.slots, *map);

    map = &kb.slots(type.slot(id.ast, ast.Base),
                    type.slot(id.objectType, type.Type_),
                    type.slot(id.value, type.Cell));
    Var.set(id.slots, *map);

    map = &kb.slots(type.slot(id.ast, ast.Base),
                    type.slot(id.status, type.Cell),
                    type.slot(id.condition, Base),
                    type.slot(id.statement, Base));
    While.set(id.slots, *map);
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
    Get(kb, kb.type.Type_, "ast::Get"),
    GreaterThan(kb, kb.type.Type_, "ast::GreaterThan"),
    GreaterThanOrEqual(kb, kb.type.Type_, "ast::GreaterThanOrEqual"),
    Has(kb, kb.type.Type_, "ast::Has"),
    If(kb, kb.type.Type_, "ast::If"),
    Input(kb, kb.type.Type_, "ast::Input"),
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
    Output(kb, kb.type.Type_, "ast::Output"),
    Return(kb, kb.type.Type_, "ast::Return"),
    Same(kb, kb.type.Type_, "ast::Same"),
    Self(kb, kb.type.Type_, "ast::Self"),
    SelfFn(kb, kb.type.Type_, "ast::SelfFn"),
    Set(kb, kb.type.Type_, "ast::Set"),
    Slot(kb, kb.type.Type_, "ast::Slot"),
    StaticCall(kb, kb.type.Type_, "ast::StaticCall"),
    Subtract(kb, kb.type.Type_, "ast::Subtract"),
    Var(kb, kb.type.Type_, "ast::Var"),
    While(kb, kb.type.Type_, "ast::While")
{
    auto& id   = kb.id;
    auto& type = kb.type;
    CellI* map = nullptr;

    map = &kb.slots(type.slot(id.lhs, Base),
                    type.slot(id.rhs, Base));
    Add.set(id.slots, *map);

    map = &kb.slots(type.slot(id.lhs, Base),
                    type.slot(id.rhs, Base));
    And.set(id.slots, *map);

    map = &kb.slots(type.slot(id.asts, type.Cell));
    Block.set(id.slots, *map);

    map = &kb.slots(type.slot(id.cell, Base),
                    type.slot(id.method, Base),
                    type.slot(id.parameters, type.ListOf(Slot)));
    Call.set(id.slots, *map);

    map = &kb.slots(type.slot(id.value, type.Cell));
    Cell.set(id.slots, *map);

    map = &kb.slots(type.slot(id.cell, Base));
    Delete.set(id.slots, *map);

    map = &kb.slots(type.slot(id.lhs, Base),
                    type.slot(id.rhs, Base));
    Divide.set(id.slots, *map);

    map = &kb.slots(type.slot(id.condition, Base),
                    type.slot(id.statement, Base));
    Do.set(id.slots, *map);

    map = &kb.slots(type.slot(id.lhs, Base),
                    type.slot(id.rhs, Base));
    Equal.set(id.slots, *map);

    map = &kb.slots(type.slot(id.cell, Base),
                    type.slot(id.role, Base));
    Erase.set(id.slots, *map);

    map = &kb.slots(type.slot(id.objectType, type.Cell),
                    type.slot(id.name, type.Cell),
                    type.slot(id.input, type.ListOf(Slot)),
                    type.slot(id.ast, type.ListOf(Base)),
                    type.slot(id.output, type.ListOf(Slot)),
                    type.slot(id.static_, type.Boolean));
    Function.set(id.slots, *map);

    map = &kb.slots(type.slot(id.cell, Base),
                    type.slot(id.role, Base));
    Get.set(id.slots, *map);

    map = &kb.slots(type.slot(id.lhs, Base),
                    type.slot(id.rhs, Base));
    GreaterThan.set(id.slots, *map);

    map = &kb.slots(type.slot(id.lhs, Base),
                    type.slot(id.rhs, Base));
    GreaterThanOrEqual.set(id.slots, *map);

    map = &kb.slots(type.slot(id.cell, Base),
                    type.slot(id.role, Base));
    Has.set(id.slots, *map);

    map = &kb.slots(type.slot(id.condition, Base),
                    type.slot(id.then, Base),
                    type.slot(id.else_, Base));
    If.set(id.slots, *map);

    map = &kb.slots(type.slot(id.role, type.Cell));
    Input.set(id.slots, *map);

    map = &kb.slots(type.slot(id.lhs, Base),
                    type.slot(id.rhs, Base));
    LessThan.set(id.slots, *map);

    map = &kb.slots(type.slot(id.lhs, Base),
                    type.slot(id.rhs, Base));
    LessThanOrEqual.set(id.slots, *map);

    map = &kb.slots(type.slot(id.role, Base));
    Member.set(id.slots, *map);

    map = &kb.slots(type.slot(id.cell, Base),
                    type.slot(id.role, Base));
    Missing.set(id.slots, *map);

    map = &kb.slots(type.slot(id.lhs, Base),
                    type.slot(id.rhs, Base));
    Multiply.set(id.slots, *map);

    map = &kb.slots(type.slot(id.objectType, Base),
                    type.slot(id.constructor, Base),
                    type.slot(id.parameters, type.ListOf(type.ast.Slot)));
    New.set(id.slots, *map);

    map = &kb.slots(type.slot(id.input, Base));
    Not.set(id.slots, *map);

    map = &kb.slots(type.slot(id.lhs, Base),
                    type.slot(id.rhs, Base));
    NotEqual.set(id.slots, *map);

    map = &kb.slots(type.slot(id.lhs, Base),
                    type.slot(id.rhs, Base));
    NotSame.set(id.slots, *map);

    map = &kb.slots(type.slot(id.lhs, Base),
                    type.slot(id.rhs, Base));
    Or.set(id.slots, *map);

    map = &kb.slots(type.slot(id.role, type.Cell));
    Output.set(id.slots, *map);

    map = &kb.slots(type.slot(id.value, type.Cell));
    Return.set(id.slots, *map);

    map = &kb.slots(type.slot(id.lhs, Base),
                    type.slot(id.rhs, Base));
    Same.set(id.slots, *map);

    map = &kb.slots(type.slot(id.cell, Base),
                    type.slot(id.role, Base),
                    type.slot(id.value, Base));
    Set.set(id.slots, *map);

    map = &kb.slots(type.slot(id.slotRole, Base),
                    type.slot(id.slotType, Base));
    Slot.set(id.slots, *map);

    map = &kb.slots(type.slot(id.cell, Base),
                    type.slot(id.method, Base),
                    type.slot(id.parameters, type.ListOf(Slot)));
    StaticCall.set(id.slots, *map);

    map = &kb.slots(type.slot(id.lhs, Base),
                    type.slot(id.rhs, Base));
    Subtract.set(id.slots, *map);

    map = &kb.slots(type.slot(id.role, Base));
    Var.set(id.slots, *map);

    map = &kb.slots(type.slot(id.condition, Base),
                    type.slot(id.statement, Base));
    While.set(id.slots, *map);
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
    Type2_(kb, kb.type.Type_, "Type"),
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
    Map2(kb, kb.type.Type2_, "Map2"),
    MapCellToSlot(kb, kb.type.Type_, "Map<Cell, Slot>"),
    Map2CellToSlot(kb, kb.type.Type_, "Map2<Cell, Slot>"),
    MapCellToType(kb, kb.type.Type_, "Map<Cell, Type>"),
    MapCellToAstFunction(kb, kb.type.Type_, "Map<Cell, ast::Function>"),
    MapCellToOpFunction(kb, kb.type.Type_, "Map<Cell, op::Function>"),
    MapCellToOpVar(kb, kb.type.Type_, "Map<Cell, op::Var>"),
    MapCellToOpBase(kb, kb.type.Type_, "Map<Cell, op::Base>"),
    MapTypeToType(kb, kb.type.Type_, "Map<Type, Type>"),
    Index(kb, kb.type.Type2_, "Index"),
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
    Directions(kb, kb.type.Enum, "Directions"),
    Shape(kb, kb.type.Struct, "Shape"),
    op(kb),
    ast(kb),
    arc(kb)
{
    CellI* map = nullptr;
    auto& id   = kb.id;
    auto& type = kb.type;

    map = &kb.slots(type.slot(id.slotType, type.Type_),
                    type.slot(id.slotRole, type.Cell));
    Slot.set(kb.id.slots, *map);

    map = &kb.slots(type.slot(id.slots, MapCellToSlot),
                    type.slot(id.subTypes, MapCellToType),
                    type.slot(id.memberOf, MapTypeToType),
                    type.slot(id.asts, MapCellToAstFunction),
                    type.slot(id.methods, MapCellToOpFunction));
    Type_.set(id.slots, *map);

    map = &kb.slots(type.slot(id.slots, MapCellToSlot),
                    type.slot(id.sharedObject, Slot),
                    type.slot(id.subTypes, MapCellToType),
                    type.slot(id.memberOf, MapTypeToType),
                    type.slot(id.asts, MapCellToAstFunction),
                    type.slot(id.methods, MapCellToOpFunction));
    Type2_.set(id.slots, *map);

    map = &kb.slots(type.slot(id.members, List));
    Enum.set(id.slots, *map);

    kb.m_initPhase = Brain::InitPhase::SlotTypeInitialzed;
}

cells::CellI& Types::slot(cells::CellI& role, cells::CellI& type)
{
    CellI& ret = *new Object(kb, kb.type.Slot);
    ret.set(kb.id.slotRole, role);
    ret.set(kb.id.slotType, type);

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
        listType.set(kb.id.subTypes, kb.map(kb.type.Cell, kb.type.Type_, kb.id.objectType, type, kb.id.itemType, itemType));
        listType.set(kb.id.memberOf, kb.map(kb.type.Type_, kb.type.Type_, kb.type.List, kb.type.List));
        itemType.set(kb.id.memberOf, kb.map(kb.type.Type_, kb.type.Type_, kb.type.ListItem, kb.type.ListItem));

        CellI* map = nullptr;

        map = &kb.slots(kb.type.slot(kb.id.first, itemType),
                        kb.type.slot(kb.id.last, itemType),
                        kb.type.slot(kb.id.size, kb.type.Number),
                        kb.type.slot(kb.id.itemType, itemType),
                        kb.type.slot(kb.id.objectType, type));
        listType.set(kb.id.slots, *map);

        map = &kb.slots(kb.type.slot(kb.id.previous, itemType),
                        kb.type.slot(kb.id.next, itemType),
                        kb.type.slot(kb.id.value, type));
        itemType.set(kb.id.slots, *map);

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

    mapType.set(kb.id.methods, kb.type.Map[kb.id.methods]);
    mapType.set(kb.id.memberOf, kb.map(kb.type.Type_, kb.type.Type_, kb.type.Map, kb.type.Map));
    mapType.set(kb.id.subTypes, kb.map(kb.type.Cell, kb.type.Type_, kb.id.keyType, keyType, kb.id.objectType, valueType));

    map = &kb.slots(kb.type.slot(kb.id.list, ListOf(valueType)),
                    kb.type.slot(kb.id.index, kb.type.Index),
                    kb.type.slot(kb.id.indexType, kb.type.Type_),
                    kb.type.slot(kb.id.keyType, keyType),
                    kb.type.slot(kb.id.objectType, valueType),
                    kb.type.slot(kb.id.size, kb.type.Number));
    mapType.set(kb.id.slots, *map);

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

    setType.set(kb.id.methods, kb.type.Set[kb.id.methods]);
    setType.set(kb.id.memberOf, kb.map(kb.type.Type_, kb.type.Type_, kb.type.Set, kb.type.Set));
    setType.set(kb.id.subTypes, kb.map(kb.type.Cell, kb.type.Type_, kb.id.objectType, valueType));

    map = &kb.slots(kb.type.slot(kb.id.list, ListOf(valueType)),
                    kb.type.slot(kb.id.index, kb.type.Index),
                    kb.type.slot(kb.id.indexType, kb.type.Type_),
                    kb.type.slot(kb.id.objectType, valueType),
                    kb.type.slot(kb.id.size, kb.type.Number));
    setType.set(kb.id.slots, *map);

    return setType;
}

// ============================================================================
Ast::Base::Base(brain::Brain& kb, CellI& classCell, const std::string& label) :
    Object(kb, classCell, label)
{
}

Ast::Input::Input(brain::Brain& kb, CellI& role) :
    BaseT<Input>(kb, kb.type.ast.Input)
{
    set(kb.id.role, role);
}

Ast::Get& Ast::Input::operator/(Base& role)
{
    return Get::New(kb, *this, role);
}

Ast::Output::Output(brain::Brain& kb, CellI& role) :
    BaseT<Output>(kb, kb.type.ast.Output, "ast.output")
{
    set(kb.id.role, role);
}

Ast::Slot::Slot(brain::Brain& kb, CellI& role, CellI& type) :
    BaseT<Slot>(kb, kb.type.ast.Slot, "ast.slot")
{
    set(kb.id.slotRole, role);
    set(kb.id.slotType, type);
}

Ast::Call::Call(brain::Brain& kb, CellI& cell, CellI& method) :
    BaseT<Call>(kb, kb.type.ast.Call, "ast.call")
{
    set(kb.id.cell, cell);
    set(kb.id.method, method);
}

Ast::Call::Call(brain::Brain& kb, CellI& cell, CellI& method, Slot& slot1) :
    BaseT<Call>(kb, kb.type.ast.Call, "ast.call")
{
    set(kb.id.cell, cell);
    set(kb.id.method, method);
    set(kb.id.parameters, kb.list(slot1));
}

Ast::Call::Call(brain::Brain& kb, CellI& cell, CellI& method, Slot& slot1, Slot& slot2) :
    BaseT<Call>(kb, kb.type.ast.Call, "ast.call")
{
    set(kb.id.cell, cell);
    set(kb.id.method, method);
    set(kb.id.parameters, kb.list(slot1, slot2));
}

Ast::Call::Call(brain::Brain& kb, CellI& cell, CellI& method, Slot& slot1, Slot& slot2, Slot& slot3) :
    BaseT<Call>(kb, kb.type.ast.Call, "ast.call")
{
    set(kb.id.cell, cell);
    set(kb.id.method, method);
    set(kb.id.parameters, kb.list(slot1, slot2, slot3));
}

Ast::Call::Call(brain::Brain& kb, CellI& cell, CellI& method, Slot& slot1, Slot& slot2, Slot& slot3, Slot& slot4) :
    BaseT<Call>(kb, kb.type.ast.Call, "ast.call")
{
    set(kb.id.cell, cell);
    set(kb.id.method, method);
    set(kb.id.parameters, kb.list(slot1, slot2, slot3, slot4));
}

Ast::StaticCall::StaticCall(brain::Brain& kb, CellI& cell, CellI& method) :
    BaseT<StaticCall>(kb, kb.type.ast.StaticCall, "ast.staticCall")
{
    set(kb.id.cell, cell);
    set(kb.id.method, method);
}

Ast::StaticCall::StaticCall(brain::Brain& kb, CellI& cell, CellI& method, Slot& slot1) :
    BaseT<StaticCall>(kb, kb.type.ast.StaticCall, "ast.staticCall")
{
    set(kb.id.cell, cell);
    set(kb.id.method, method);
    set(kb.id.parameters, kb.list(slot1));
}

Ast::StaticCall::StaticCall(brain::Brain& kb, CellI& cell, CellI& method, Slot& slot1, Slot& slot2) :
    BaseT<StaticCall>(kb, kb.type.ast.StaticCall, "ast.staticCall")
{
    set(kb.id.cell, cell);
    set(kb.id.method, method);
    set(kb.id.parameters, kb.list(slot1, slot2));
}

Ast::StaticCall::StaticCall(brain::Brain& kb, CellI& cell, CellI& method, Slot& slot1, Slot& slot2, Slot& slot3) :
    BaseT<StaticCall>(kb, kb.type.ast.StaticCall, "ast.staticCall")
{
    set(kb.id.cell, cell);
    set(kb.id.method, method);
    set(kb.id.parameters, kb.list(slot1, slot2, slot3));
}

Ast::StaticCall::StaticCall(brain::Brain& kb, CellI& cell, CellI& method, Slot& slot1, Slot& slot2, Slot& slot3, Slot& slot4) :
    BaseT<StaticCall>(kb, kb.type.ast.StaticCall, "ast.staticCall")
{
    set(kb.id.cell, cell);
    set(kb.id.method, method);
    set(kb.id.parameters, kb.list(slot1, slot2, slot3, slot4));
}

Ast::Cell::Cell(brain::Brain& kb, CellI& value) :
    BaseT<Cell>(kb, kb.type.ast.Cell)
{
    set(kb.id.value, value);
}

Ast::Get& Ast::Cell::operator/(Base& role)
{
    return Get::New(kb, *this, role);
}

Ast::Self::Self(brain::Brain& kb) :
    BaseT<Self>(kb, kb.type.ast.Self, "ast.self")
{
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
    set(kb.id.value, value);
}

Ast::Block::Block(brain::Brain& kb, List& list) :
    BaseT<Block>(kb, kb.type.ast.Block, "ast.block")
{
    set(kb.id.asts, list);
}

Ast::Function::Function(brain::Brain& kb, CellI& objType, CellI& name, const std::string& label) :
    BaseT<Function>(kb, kb.type.ast.Function, "ast.function")
{
    set(kb.id.objectType, objType);
    set(kb.id.name, name);
    this->label(label);
}

void Ast::Function::addInputs(List& input)
{
    set(kb.id.input, input);
    m_inputs = &input;
}

void Ast::Function::addOutputs(List& output)
{
    set(kb.id.output, output);
    m_outputs = &output;
}

void Ast::Function::addBlock(Block& ast)
{
    set(kb.id.ast, ast);
    m_asts = &ast;
}

CellI& Ast::Function::compile()
{
    return compileImpl(nullptr);
}

CellI& Ast::Function::compile(CellI& type)
{
    return compileImpl(&type);
}

CellI& Ast::Function::compileImpl(CellI* type)
{
    cells::Object& functionType       = *new cells::Object(kb, kb.type.Type_);
    cells::Object& functionInputType  = *new cells::Object(kb, kb.type.Type_);
    cells::Object& functionOutputType = *new cells::Object(kb, kb.type.Type_);
    functionType.set(kb.id.memberOf, kb.map(kb.type.Type_, kb.type.Type_, kb.type.op.Function, kb.type.op.Function));
    cells::Map& subTypesMap = kb.map(kb.type.Cell, kb.type.Type_, kb.id.objectType, get(kb.id.objectType), kb.id.name, get(kb.id.name), kb.id.input, functionInputType, kb.id.output, functionOutputType);
    functionType.set(kb.id.subTypes, subTypesMap);
    functionType.set(kb.id.slots, kb.type.op.Function[kb.id.slots]);

    cells::Object& function = *new cells::Object(kb, functionType);
    compileParams(function, functionInputType, functionOutputType, type);
    functionType.label(std::format("Type for {}", function.label()));
    function.set(kb.id.ast, asts());
    function.set(kb.id.op, compileAst(asts(), function, type));
    if (has(kb.id.static_)) {
        function.set(kb.id.static_, get(kb.id.static_));
    }

    return function;
}

void Ast::Function::compileParams(cells::Object& function, cells::Object& inputType, cells::Object& outputType, CellI* type)
{
    std::stringstream iss;
    std::stringstream oss;
    if (m_inputs || type) {
        Map& slots = *new Map(kb, kb.type.Cell, kb.type.Slot);
        if (type) {
            Object& var = *new Object(kb, kb.type.op.Var, "self");
            var.set(kb.id.objectType, *type);
            slots.add(kb.id.self, kb.type.slot(kb.id.self, *type));
            iss << kb.id.self.label() << ": " << (*type).label();
        }
        if (m_inputs) {
            Visitor::visitList(inputs(), [this, &slots, &iss](CellI& slot, int i, bool& stop) {
                if (!slots.empty()) {
                    iss << ", ";
                }
                iss << "in_" << slot[kb.id.slotRole].label() << ": " << slot[kb.id.slotType].label();
                slots.add(slot[kb.id.slotRole], kb.type.slot(slot[kb.id.slotRole], slot[kb.id.slotType]));
            });
        }
        inputType.set(kb.id.slots, slots);
    }
    if (m_outputs) {
        Map& slots  = *new Map(kb, kb.type.Cell, kb.type.Slot);
        Visitor::visitList(outputs(), [this, &slots, &oss](CellI& slot, int i, bool& stop) {
            if (!slots.empty()) {
                oss << ", ";
            }
            oss << "out_" << slot[kb.id.slotRole].label() << ": " << slot[kb.id.slotType].label();
            slots.add(slot[kb.id.slotRole], kb.type.slot(slot[kb.id.slotRole], slot[kb.id.slotType]));
        });
        outputType.set(kb.id.slots, slots);
    }
    if (m_outputs) {
        function.label(std::format("fn {}({}) -> ({})", label(), iss.str(), oss.str()));
    } else {
        function.label(std::format("fn {}({})", label(), iss.str()));
    }
}

CellI& Ast::Function::compileAst(CellI& ast, cells::Object& function, CellI* type)
{
    auto compile = [this, &function, type](CellI& ast) -> CellI& { return compileAst(ast, function, type); };
    const auto _ = [this](CellI& cell) -> Ast::Cell& { return kb.ast.cell(cell); };
    auto& id = kb.id;

    if (&ast.type() == &kb.type.ast.Block) {
        CellI& list        = ast[kb.id.asts];
        auto& compiledAsts = *new cells::List(kb, kb.type.op.Base);
        Visitor::visitList(list, [this, &compiledAsts, &ast, &function, type](CellI& ast, int, bool&) {
            compiledAsts.add(compileAst(ast, function, type));
        });
        Object& opBlock = *new Object(kb, kb.type.op.Block);
        opBlock.set(kb.id.ast, ast);
        opBlock.set(kb.id.ops, compiledAsts);
        return opBlock;
    } else if (&ast.type() == &kb.type.ast.Cell) {
        Object& constVar = *new Object(kb, kb.type.op.ConstVar);
        constVar.set(kb.id.ast, ast);
        constVar.set(kb.id.value, ast[kb.id.value]);
        return constVar;
    } else if (&ast.type() == &kb.type.ast.SelfFn) {
        Object& constVar = *new Object(kb, kb.type.op.ConstVar);
        constVar.set(kb.id.ast, ast);
        constVar.set(kb.id.value, function);
        return constVar;
    } else if (&ast.type() == &kb.type.ast.Self) {
        CellI& retOp = compile(kb.ast.get(_(function), _(id.stack)) / _(id.value) / _(id.input) / _(id.self));
        retOp.set(id.ast, ast);
        retOp.label("self");
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Input) {
        CellI& retOp = compile(kb.ast.get(_(function), _(id.stack)) / _(id.value) / _(id.input) / _(ast[id.role]));
        retOp.set(id.ast, ast);
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Output) {
        CellI& retOp = compile(kb.ast.get(_(function), _(id.stack)) / _(id.value) / _(id.output) / _(ast[id.role]));
        retOp.set(id.ast, ast);
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Var) {
        if (function.type()[kb.id.subTypes][kb.id.index].missing(kb.id.localVars)) {
            cells::Object& functionLocalVarsType = *new cells::Object(kb, kb.type.Type_);
            functionLocalVarsType.set(kb.id.memberOf, kb.map(kb.type.Type_, kb.type.Type_, kb.type.Index, kb.type.Index));
            static_cast<Map&>(function.type()[kb.id.subTypes]).add(kb.id.localVars, functionLocalVarsType);
        }
        CellI& localVarsType = function.type()[kb.id.subTypes][kb.id.index][kb.id.localVars];
        if (localVarsType.missing(kb.id.slots)) {
            localVarsType.set(kb.id.slots, *new Map(kb, kb.type.Cell, kb.type.Slot));
        }
        auto& slotsMap = static_cast<Map&>(localVarsType[kb.id.slots]);
        if (!slotsMap.hasKey(ast[kb.id.role])) {
            slotsMap.add(ast[kb.id.role], kb.type.slot(ast[kb.id.role], kb.type.op.Var));
        }
        CellI& retOp = compile(kb.ast.get(_(function), _(id.stack)) / _(id.value) / _(id.localVars) / _(ast[id.role]));
        retOp.set(id.ast, ast);
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Delete) {
        Object& retOp = *new Object(kb, kb.type.op.Delete);
        retOp.set(kb.id.ast, ast);
        retOp.set(kb.id.input, compile(ast[kb.id.cell]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Set) {
        Object& retOp = *new Object(kb, kb.type.op.Set);
        retOp.set(kb.id.ast, ast);
        retOp.set(kb.id.cell, compile(ast[kb.id.cell]));
        retOp.set(kb.id.role, compile(ast[kb.id.role]));
        retOp.set(kb.id.value, compile(ast[kb.id.value]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Erase) {
        Object& retOp = *new Object(kb, kb.type.op.Erase);
        retOp.set(kb.id.ast, ast);
        retOp.set(kb.id.cell, compile(ast[kb.id.cell]));
        retOp.set(kb.id.role, compile(ast[kb.id.role]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.If) {
        Object& retOp = *new Object(kb, kb.type.op.If);
        retOp.set(kb.id.ast, ast);
        retOp.set(kb.id.condition, compile(ast[kb.id.condition]));
        retOp.set(kb.id.then, compile(ast[kb.id.then]));
        if (ast.has(kb.id.else_)) {
            retOp.set(kb.id.else_, compile(ast[kb.id.else_]));
            return retOp;
        } else {
            return retOp;
        }
    } else if (&ast.type() == &kb.type.ast.Do) {
        Object& retOp = *new Object(kb, kb.type.op.Do);
        retOp.set(kb.id.ast, ast);
        retOp.set(kb.id.condition, compile(ast[kb.id.condition]));
        retOp.set(kb.id.statement, compile(ast[kb.id.statement]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.While) {
        Object& retOp = *new Object(kb, kb.type.op.While);
        retOp.set(kb.id.ast, ast);
        retOp.set(kb.id.condition, compile(ast[kb.id.condition]));
        retOp.set(kb.id.statement, compile(ast[kb.id.statement]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.New) {
        auto& compiledAsts = *new cells::List(kb, kb.type.op.Base);

        Object& block = *new Object(kb, kb.type.op.Block);
        block.set(kb.id.ast, ast);
        block.set(kb.id.ops, compiledAsts);
        Object& opSet = *new Object(kb, kb.type.op.Set, "block.value = new objectType()");
        opSet.set(kb.id.ast, ast);
        opSet.set(kb.id.cell, compile(kb.ast.cell(block)));
        opSet.set(kb.id.role, compile(kb.ast.cell(kb.id.value)));
        Object& opNew = *new Object(kb, kb.type.op.New);
        opNew.set(kb.id.ast, ast);
        opNew.set(kb.id.objectType, compile(ast[kb.id.objectType]));
        opSet.set(kb.id.value, opNew);

        compiledAsts.add(opSet);
        if (ast.has(kb.id.constructor)) {
            Object& callAst = *new Object(kb, kb.type.ast.Call);
            callAst.set(kb.id.cell, kb.ast.get(kb.ast.cell(block), kb.ast.cell(kb.id.value)));
            callAst.set(kb.id.method, ast[kb.id.constructor]);
            if (ast.has(kb.id.parameters)) {
                callAst.set(kb.id.parameters, ast[kb.id.parameters]);
            }
            compiledAsts.add(compile(callAst));
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
        Ast::Base& astCell     = static_cast<Ast::Base&>(ast[kb.id.cell]);
        Ast::Base& astMethod   = static_cast<Ast::Base&>(ast[kb.id.method]);
        Ast::Get* getMethodPtr = nullptr;
        if (&ast.type() == &kb.type.ast.Call) {
            getMethodPtr = &(kb.ast.get(astCell, _(kb.id.type)) / _(kb.id.methods));
        } else {
            getMethodPtr = &(kb.ast.get(astCell, _(kb.id.methods)));
        }
        Ast::Get& getMethod = (*getMethodPtr) / _(kb.id.index) / astMethod;
        auto& compiledAsts  = *new cells::List(kb, kb.type.op.Base);
        Object& block       = *new Object(kb, kb.type.op.Block, blockName);
        block.set(kb.id.ast, ast);
        block.set(kb.id.ops, compiledAsts);
        Object& varMethod = *new Object(kb, kb.type.op.Var, "Call { var_method; }");
        varMethod.set(kb.id.objectType, kb.type.op.Function);
        CellI& storeMethod     = compile(kb.ast.set(_(varMethod), _(kb.id.value), getMethod));
        CellI& setCurrentStack = compile(kb.ast.set(_(varMethod) / _(kb.id.value), _(kb.id.stack), kb.ast.get(_(function), _(id.stack))));

        Object& varNewStackItem = *new Object(kb, kb.type.op.Var, "Call { var_newStackItem; }");
        varNewStackItem.set(kb.id.objectType, kb.type.ListItem);
        Object& varNewStackFrame = *new Object(kb, kb.type.op.Var, "Call { var_newStackFrame; }");
        varNewStackFrame.set(kb.id.objectType, kb.type.StackFrame);

        Object& varInputIndex = *new Object(kb, kb.type.Index, "Call { var_inputIndex; }");
        Object& varLocalVars = *new Object(kb, kb.type.op.Var, "Call { var_localVars; }");
        varLocalVars.set(kb.id.objectType, kb.type.Index);
        Object& varLocalVarsList = *new Object(kb, kb.type.op.Var, "Call { var_localVarsList; }");
        varLocalVarsList.set(kb.id.objectType, kb.type.List);
        Object& varLocalVarsListItem = *new Object(kb, kb.type.op.Var, "Call { var_localVarsListItem; }");
        varLocalVarsListItem.set(kb.id.objectType, kb.type.ListItem);
        Object& varLocalVarsListItemHasNext = *new Object(kb, kb.type.op.Var, "Call { var_localVarsListItemHasNext; }");
        varLocalVarsListItemHasNext.set(kb.id.objectType, kb.type.Boolean);

        CellI& storeStackItem     = compile(kb.ast.set(_(varNewStackItem), _(id.value), kb.ast.new_(_(kb.type.ListItem))));
        CellI& storeStackFrame    = compile(kb.ast.set(_(varNewStackFrame), _(id.value), kb.ast.new_(_(kb.type.StackFrame))));
        CellI& setListItem        = compile(kb.ast.set(_(varNewStackItem) / _(id.value), _(id.value), _(varNewStackFrame) / _(id.value)));
        CellI& setListItemPrev    = compile(kb.ast.set(_(varNewStackItem) / _(id.value), _(kb.id.previous), _(varMethod) / _(id.value) / _(id.stack)));
        CellI& storeInputIndex    = compile(kb.ast.set(_(varInputIndex), _(id.value), kb.ast.new_(_(kb.type.Index))));
        CellI& setMethod          = compile(kb.ast.set(_(varNewStackFrame) / _(id.value), _(id.method), _(varMethod) / _(id.value)));

        CellI& createNewLocalVars = compile(
            kb.ast.if_(kb.ast.has(_(varMethod) / _(id.value) / _(id.type) / _(id.subTypes) / _(id.index), _(id.localVars)),
                       kb.ast.block(
                           kb.ast.set(_(varLocalVars), _(id.value), kb.ast.new_(_(varMethod) / _(id.value) / _(id.type) / _(id.subTypes) / _(id.index) / _(id.localVars))),
                           kb.ast.set(_(varNewStackFrame) / _(id.value), _(id.localVars), _(varLocalVars) / _(id.value)),
                           kb.ast.set(_(varLocalVarsListItem), _(id.value), _(varMethod) / _(id.value) / _(id.type) / _(id.subTypes) / _(id.index) / _(id.localVars) / _(id.slots) / _(id.list) / _(kb.id.first)),
                           kb.ast.do_(kb.ast.block(
                                          kb.ast.set(_(varLocalVarsListItemHasNext), _(id.value), _(kb.boolean.true_)),
                                          kb.ast.set(_(varLocalVars) / _(id.value), _(varLocalVarsListItem) / _(kb.id.value) / _(kb.id.value) / _(id.slotRole), kb.ast.new_(_(kb.type.op.Var))),
                                          kb.ast.if_(kb.ast.has(_(varLocalVarsListItem) / _(id.value), _(kb.id.next)),
                                                     kb.ast.set(_(varLocalVarsListItem), _(id.value), _(varLocalVarsListItem) / _(id.value) / _(kb.id.next)),
                                                     kb.ast.set(_(varLocalVarsListItemHasNext), _(id.value), _(kb.boolean.false_)))),
                                      kb.ast.same(_(varLocalVarsListItemHasNext) / _(id.value), _(kb.boolean.true_))))));

        CellI& setInput      = compile(kb.ast.set(_(varNewStackFrame) / _(id.value), _(id.input), _(varInputIndex) / _(id.value)));
        CellI& setSelf       = compile(kb.ast.set(_(varInputIndex) / _(id.value), _(id.self), astCell));
        CellI& setStackToNew = compile(kb.ast.set(_(varMethod) / _(id.value), _(id.stack), _(varNewStackItem) / _(id.value)));
        CellI& setOutput     = compile(kb.ast.if_(kb.ast.has(_(varMethod) / _(id.value) / _(id.type) / _(id.subTypes) / _(id.index) / _(id.output), _(id.slots)),
                                                  kb.ast.if_(kb.ast.has(_(varMethod) / _(id.value) / _(id.type) / _(id.subTypes) / _(id.index) / _(id.output) / _(id.slots) / _(id.index), _(id.value)),
                                                             kb.ast.block(kb.ast.set(_(varNewStackFrame) / _(id.value), _(id.output), kb.ast.new_(_(varMethod) / _(id.value) / _(id.type) / _(id.subTypes) / _(id.index) / _(id.output))),
                                                                          kb.ast.set(_(varNewStackFrame) / _(id.value) / _(id.output), _(id.value), kb.ast.new_(_(kb.type.op.Var)))))));
        CellI& getResult     = compile(kb.ast.if_(kb.ast.has(_(varMethod) / _(id.value) / _(id.type) / _(id.subTypes) / _(id.index) / _(id.output), _(id.slots)),
                                                  kb.ast.if_(kb.ast.has(_(varMethod) / _(id.value) / _(id.type) / _(id.subTypes) / _(id.index) / _(id.output) / _(id.slots) / _(id.index), _(id.value)),
                                                             kb.ast.set(_(block), _(kb.id.value), _(varMethod) / _(id.value) / _(id.stack) / _(id.value) / _(id.output) / _(kb.id.value) / _(kb.id.value)))));
        CellI& setStackToOld = compile(kb.ast.set(_(varMethod) / _(id.value) / _(id.stack) / _(kb.id.previous) / _(id.value) / _(id.method), _(id.stack), _(varMethod) / _(id.value) / _(id.stack) / _(kb.id.previous)));

        compiledAsts.add(storeMethod);
        compiledAsts.add(setCurrentStack);
        compiledAsts.add(storeStackItem);
        compiledAsts.add(storeStackFrame);
        compiledAsts.add(setListItem);
        compiledAsts.add(setListItemPrev);
        compiledAsts.add(storeInputIndex);
        compiledAsts.add(createNewLocalVars);
        compiledAsts.add(setMethod);
        compiledAsts.add(setInput);
        compiledAsts.add(setOutput);
        compiledAsts.add(setSelf);


        if (ast.has(kb.id.parameters)) {
            Visitor::visitList(ast[kb.id.parameters], [this, &compiledAsts, &compile, &varInputIndex, &_, &id](CellI& param, int, bool&) {
                Ast::Base& paramRole  = static_cast<Ast::Base&>(param[kb.id.slotRole]);
                Ast::Base& paramValue = static_cast<Ast::Base&>(param[kb.id.slotType]);
                CellI& setParam       = compile(kb.ast.set(_(varInputIndex) / _(id.value), paramRole, paramValue));
                setParam.label("Call { setParam; }");
                compiledAsts.add(setParam);
            });
        }
        CellI& evalMethod = *new Object(kb, kb.type.op.EvalVar, std::format("{}::Call {{ evalVar; }}", function.label()));
        evalMethod.set(kb.id.value, varMethod);
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
        storeInputIndex.label("Call { storeInputIndex; }");
        createNewLocalVars.label("Call { createNewLocalVars; }");
        setMethod.label("Call { setMethod; }");
        setInput.label("Call { setInput; }");
        setOutput.label("Call { setOutput; }");
        setSelf.label("Call { setSelf; }");
        setStackToNew.label("Call { setStackToNew; }");
        getResult.label("Call { getResult; }");
        setStackToOld.label("Call { setStackToOld; }");

        return block;
    } else if (&ast.type() == &kb.type.ast.And) {
        Object& retOp = *new Object(kb, kb.type.op.And);
        retOp.set(kb.id.ast, ast);
        retOp.set(kb.id.lhs, compile(ast[kb.id.lhs]));
        retOp.set(kb.id.rhs, compile(ast[kb.id.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Or) {
        Object& retOp = *new Object(kb, kb.type.op.Or);
        retOp.set(kb.id.ast, ast);
        retOp.set(kb.id.lhs, compile(ast[kb.id.lhs]));
        retOp.set(kb.id.rhs, compile(ast[kb.id.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Not) {
        Object& retOp = *new Object(kb, kb.type.op.Not);
        retOp.set(kb.id.ast, ast);
        retOp.set(kb.id.input, compile(ast[kb.id.input]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Add) {
        Object& retOp = *new Object(kb, kb.type.op.Add);
        retOp.set(kb.id.ast, ast);
        retOp.set(kb.id.lhs, compile(ast[kb.id.lhs]));
        retOp.set(kb.id.rhs, compile(ast[kb.id.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Subtract) {
        Object& retOp = *new Object(kb, kb.type.op.Subtract);
        retOp.set(kb.id.ast, ast);
        retOp.set(kb.id.lhs, compile(ast[kb.id.lhs]));
        retOp.set(kb.id.rhs, compile(ast[kb.id.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Multiply) {
        Object& retOp = *new Object(kb, kb.type.op.Multiply);
        retOp.set(kb.id.ast, ast);
        retOp.set(kb.id.lhs, compile(ast[kb.id.lhs]));
        retOp.set(kb.id.rhs, compile(ast[kb.id.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Divide) {
        Object& retOp = *new Object(kb, kb.type.op.Divide);
        retOp.set(kb.id.ast, ast);
        retOp.set(kb.id.lhs, compile(ast[kb.id.lhs]));
        retOp.set(kb.id.rhs, compile(ast[kb.id.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.LessThan) {
        Object& retOp = *new Object(kb, kb.type.op.LessThan);
        retOp.set(kb.id.ast, ast);
        retOp.set(kb.id.lhs, compile(ast[kb.id.lhs]));
        retOp.set(kb.id.rhs, compile(ast[kb.id.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.LessThanOrEqual) {
        Object& retOp = *new Object(kb, kb.type.op.LessThanOrEqual);
        retOp.set(kb.id.ast, ast);
        retOp.set(kb.id.lhs, compile(ast[kb.id.lhs]));
        retOp.set(kb.id.rhs, compile(ast[kb.id.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.GreaterThan) {
        Object& retOp = *new Object(kb, kb.type.op.GreaterThan);
        retOp.set(kb.id.ast, ast);
        retOp.set(kb.id.lhs, compile(ast[kb.id.lhs]));
        retOp.set(kb.id.rhs, compile(ast[kb.id.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.GreaterThanOrEqual) {
        Object& retOp = *new Object(kb, kb.type.op.GreaterThanOrEqual);
        retOp.set(kb.id.ast, ast);
        retOp.set(kb.id.lhs, compile(ast[kb.id.lhs]));
        retOp.set(kb.id.rhs, compile(ast[kb.id.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Same) {
        Object& retOp = *new Object(kb, kb.type.op.Same);
        retOp.set(kb.id.ast, ast);
        retOp.set(kb.id.lhs, compile(ast[kb.id.lhs]));
        retOp.set(kb.id.rhs, compile(ast[kb.id.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.NotSame) {
        Object& retOp = *new Object(kb, kb.type.op.NotSame);
        retOp.set(kb.id.ast, ast);
        retOp.set(kb.id.lhs, compile(ast[kb.id.lhs]));
        retOp.set(kb.id.rhs, compile(ast[kb.id.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Equal) {
        Object& retOp = *new Object(kb, kb.type.op.Equal);
        retOp.set(kb.id.ast, ast);
        retOp.set(kb.id.lhs, compile(ast[kb.id.lhs]));
        retOp.set(kb.id.rhs, compile(ast[kb.id.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.NotEqual) {
        Object& retOp = *new Object(kb, kb.type.op.NotEqual);
        retOp.set(kb.id.ast, ast);
        retOp.set(kb.id.lhs, compile(ast[kb.id.lhs]));
        retOp.set(kb.id.rhs, compile(ast[kb.id.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Has) {
        Object& retOp = *new Object(kb, kb.type.op.Has);
        retOp.set(kb.id.ast, ast);
        retOp.set(kb.id.cell, compile(ast[kb.id.cell]));
        retOp.set(kb.id.role, compile(ast[kb.id.role]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Missing) {
        Object& retOp = *new Object(kb, kb.type.op.Missing);
        retOp.set(kb.id.ast, ast);
        retOp.set(kb.id.cell, compile(ast[kb.id.cell]));
        retOp.set(kb.id.role, compile(ast[kb.id.role]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Get) {
        Object& retOp = *new Object(kb, kb.type.op.Get);
        retOp.set(kb.id.ast, ast);
        retOp.set(kb.id.cell, compile(ast[kb.id.cell]));
        retOp.set(kb.id.role, compile(ast[kb.id.role]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Member) {
        CellI& member = compile(kb.ast.get(kb.ast.self(), kb.ast.cell(ast[kb.id.role])));
        member.set(kb.id.ast, ast);
        return member;
    } else if (&ast.type() == &kb.type.ast.Return) {
        Object& retOp = *new Object(kb, kb.type.op.Return, "op.return");
        retOp.set(kb.id.ast, ast);
        if (ast.has(kb.id.value)) {
            retOp.set(kb.id.result, compile(kb.ast.set(kb.ast.get(_(function), _(id.stack)) / _(id.value) / _(id.output) / _(id.value), _(id.value), static_cast<Ast::Base&>(ast[id.value]))));
        }
        return retOp;
    }

    throw "Unknown function AST!";
}

//////

List& Ast::Function::inputs()
{
    return *m_inputs;
}

List& Ast::Function::outputs()
{
    return *m_outputs;
}

Ast::Block& Ast::Function::asts()
{
    return *m_asts;
}

Ast::Delete::Delete(brain::Brain& kb, Base& cell) :
    BaseT<Delete>(kb, kb.type.ast.Delete, "ast.delete")
{
    set(kb.id.cell, cell);
}

Ast::Set::Set(brain::Brain& kb, Base& cell, Base& role, Base& value) :
    BaseT<Set>(kb, kb.type.ast.Set, "ast.set")
{
    set(kb.id.cell, cell);
    set(kb.id.role, role);
    set(kb.id.value, value);
}

Ast::Erase::Erase(brain::Brain& kb, Base& cell, Base& role) :
    BaseT<Erase>(kb, kb.type.ast.Erase, "ast.erase")
{
    set(kb.id.cell, cell);
    set(kb.id.role, role);
}

Ast::If::If(brain::Brain& kb, Base& condition, Base& thenBranch) :
    BaseT<If>(kb, kb.type.ast.If, "ast.if")
{
    set(kb.id.condition, condition);
    set(kb.id.then, thenBranch);
}

Ast::If::If(brain::Brain& kb, Base& condition, Base& thenBranch, Base& elseBranch) :
    BaseT<If>(kb, kb.type.ast.If, "ast.ifElse")
{
    set(kb.id.condition, condition);
    set(kb.id.then, thenBranch);
    set(kb.id.else_, elseBranch);
}

Ast::Do::Do(brain::Brain& kb, Base& statement, Base& condition) :
    BaseT<Do>(kb, kb.type.ast.Do, "ast.do")
{
    set(kb.id.condition, condition);
    set(kb.id.statement, statement);
}

Ast::While::While(brain::Brain& kb, Base& condition, Base& statement) :
    BaseT<While>(kb, kb.type.ast.While, "ast.while")
{
    set(kb.id.condition, condition);
    set(kb.id.statement, statement);
}

Ast::Var::Var(brain::Brain& kb, CellI& role) :
    BaseT<Var>(kb, kb.type.ast.Var, "ast.var")
{
    set(kb.id.role, role);
}

Ast::Set& Ast::Var::operator=(Base& value)
{
    return Set::New(kb, *this, Cell::New(kb, kb.id.value), value);
}

Ast::Get& Ast::Var::operator*()
{
    return Get::New(kb, *this, Cell::New(kb, kb.id.value));
}

Ast::Member::Member(brain::Brain& kb, CellI& role) :
    BaseT<Member>(kb, kb.type.ast.Member, "ast.member")
{
    set(kb.id.role, role);
}

Ast::Set& Ast::Member::operator=(Base& value)
{
    Ast::Set& ret = Set::New(kb, Self::New(kb), Cell::New(kb, get(kb.id.role)), value);
    return ret;
}

Ast::Get& Ast::Member::operator/(Base& role)
{
    return Get::New(kb, *this, role);
}

Ast::Has& Ast::Member::exist()
{
    return Has::New(kb, Self::New(kb), Cell::New(kb, get(kb.id.role)));
}

Ast::Missing& Ast::Member::missing()
{
    return Missing::New(kb, Self::New(kb), Cell::New(kb, get(kb.id.role)));
}

Ast::Call& Ast::Member::call(CellI& method)
{
    return Call::New(kb, *this, method);
}

Ast::Call& Ast::Member::call(CellI& method, Slot& slot1)
{
    return Call::New(kb, *this, method, slot1);
}

Ast::Call& Ast::Member::call(CellI& method, Slot& slot1, Slot& slot2)
{
    return Call::New(kb, *this, method, slot1, slot2);
}

Ast::Call& Ast::Member::call(CellI& method, Slot& slot1, Slot& slot2, Slot& slot3)
{
    return Call::New(kb, *this, method, slot1, slot2, slot3);
}

Ast::Call& Ast::Member::call(CellI& method, Slot& slot1, Slot& slot2, Slot& slot3, Slot& slot4)
{
    return Call::New(kb, *this, method, slot1, slot2, slot3, slot4);
}

Ast::New::New(brain::Brain& kb, Base& objectType) :
    BaseT<New>(kb, kb.type.ast.New, "ast.new")
{
    set(kb.id.objectType, objectType);
}

Ast::New::New(brain::Brain& kb, Base& objectType, Base& constructor) :
    BaseT<New>(kb, kb.type.ast.New, "ast.new()")
{
    set(kb.id.objectType, objectType);
    set(kb.id.constructor, constructor);
}

Ast::New::New(brain::Brain& kb, Base& objectType, Base& constructor, Slot& slot1) :
    BaseT<New>(kb, kb.type.ast.New, "ast.new(p1)")
{
    set(kb.id.objectType, objectType);
    set(kb.id.constructor, constructor);
    set(kb.id.parameters, kb.list(slot1));
}

Ast::New::New(brain::Brain& kb, Base& objectType, Base& constructor, Slot& slot1, Slot& slot2) :
    BaseT<New>(kb, kb.type.ast.New, "ast.new(p1, p2)")
{
    set(kb.id.objectType, objectType);
    set(kb.id.constructor, constructor);
    set(kb.id.parameters, kb.list(slot1, slot2));
}

Ast::New::New(brain::Brain& kb, Base& objectType, Base& constructor, Slot& slot1, Slot& slot2, Slot& slot3) :
    BaseT<New>(kb, kb.type.ast.New, "ast.new(p1, p2, p3)")
{
    set(kb.id.objectType, objectType);
    set(kb.id.constructor, constructor);
    set(kb.id.parameters, kb.list(slot1, slot2, slot3));
}

Ast::New::New(brain::Brain& kb, Base& objectType, Base& constructor, Slot& slot1, Slot& slot2, Slot& slot3, Slot& slot4) :
    BaseT<New>(kb, kb.type.ast.New, "ast.new(p1, p2, p3, p4)")
{
    set(kb.id.objectType, objectType);
    set(kb.id.constructor, constructor);
    set(kb.id.parameters, kb.list(slot1, slot2, slot3, slot4));
}

Ast::Same::Same(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Same>(kb, kb.type.ast.Same, "ast.same")
{
    set(kb.id.lhs, lhs);
    set(kb.id.rhs, rhs);
}

Ast::NotSame::NotSame(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<NotSame>(kb, kb.type.ast.NotSame, "ast.notSame")
{
    set(kb.id.lhs, lhs);
    set(kb.id.rhs, rhs);
}

Ast::Equal::Equal(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Equal>(kb, kb.type.ast.Equal, "ast.equal")
{
    set(kb.id.lhs, lhs);
    set(kb.id.rhs, rhs);
}

Ast::NotEqual::NotEqual(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<NotEqual>(kb, kb.type.ast.NotEqual, "ast.notEqual")
{
    set(kb.id.lhs, lhs);
    set(kb.id.rhs, rhs);
}

Ast::Has::Has(brain::Brain& kb, Base& cell, Base& role) :
    BaseT<Has>(kb, kb.type.ast.Has, "ast.has")
{
    set(kb.id.cell, cell);
    set(kb.id.role, role);
}

Ast::Missing::Missing(brain::Brain& kb, Base& cell, Base& role) :
    BaseT<Missing>(kb, kb.type.ast.Missing, "ast.missing")
{
    set(kb.id.cell, cell);
    set(kb.id.role, role);
}

Ast::Get::Get(brain::Brain& kb, Base& cell, Base& role) :
    BaseT<Get>(kb, kb.type.ast.Get, "ast.get")
{
    set(kb.id.cell, cell);
    set(kb.id.role, role);
}

Ast::Get& Ast::Get::operator/(Base& role)
{
    return Get::New(kb, *this, role);
}

Ast::And::And(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<And>(kb, kb.type.ast.And, "ast.and")
{
    set(kb.id.lhs, lhs);
    set(kb.id.rhs, rhs);
}

Ast::Or::Or(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Or>(kb, kb.type.ast.Or, "ast.or")
{
    set(kb.id.lhs, lhs);
    set(kb.id.rhs, rhs);
}

Ast::Not::Not(brain::Brain& kb, Base& input) :
    BaseT<Not>(kb, kb.type.ast.Not, "ast.not")
{
    set(kb.id.input, input);
}

Ast::Add::Add(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Add>(kb, kb.type.ast.Add, "ast.add")
{
    set(kb.id.lhs, lhs);
    set(kb.id.rhs, rhs);
}

Ast::Subtract::Subtract(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Subtract>(kb, kb.type.ast.Subtract, "ast.subtract")
{
    set(kb.id.lhs, lhs);
    set(kb.id.rhs, rhs);
}

Ast::Multiply::Multiply(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Multiply>(kb, kb.type.ast.Multiply, "ast.multiply")
{
    set(kb.id.lhs, lhs);
    set(kb.id.rhs, rhs);
}

Ast::Divide::Divide(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Divide>(kb, kb.type.ast.Divide, "ast.divide")
{
    set(kb.id.lhs, lhs);
    set(kb.id.rhs, rhs);
}

Ast::LessThan::LessThan(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<LessThan>(kb, kb.type.ast.LessThan, "ast.lessThan")
{
    set(kb.id.lhs, lhs);
    set(kb.id.rhs, rhs);
}

Ast::LessThanOrEqual::LessThanOrEqual(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<LessThanOrEqual>(kb, kb.type.ast.LessThanOrEqual, "ast.lessThanOrEqual")
{
    set(kb.id.lhs, lhs);
    set(kb.id.rhs, rhs);
}

Ast::GreaterThan::GreaterThan(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<GreaterThan>(kb, kb.type.ast.GreaterThan, "ast.greaterThan")
{
    set(kb.id.lhs, lhs);
    set(kb.id.rhs, rhs);
}

Ast::GreaterThanOrEqual::GreaterThanOrEqual(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<GreaterThanOrEqual>(kb, kb.type.ast.GreaterThanOrEqual, "ast.greaterThanOrEqual")
{
    set(kb.id.lhs, lhs);
    set(kb.id.rhs, rhs);
}

Ast::Ast(brain::Brain& kb) :
    kb(kb)
{
}

Ast::Cell& Ast::cell(CellI& cell)
{
    return Cell::New(kb, cell);
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

Ast::Input& Ast::input(CellI& role)
{
    auto& ast = kb.ast;
    return Input::New(kb, role);
}

Ast::Output& Ast::output(CellI& role)
{
    auto& ast = kb.ast;
    return Output::New(kb, role);
}

Ast::Slot& Ast::slot(CellI& role, CellI& type)
{
    return Slot::New(kb, role, type);
}

Ast::Call& Ast::call(CellI& cell, CellI& method)
{
    return Call::New(kb, cell, method);
}

Ast::Call& Ast::call(CellI& cell, CellI& method, Slot& slot1)
{
    return Call::New(kb, cell, method, slot1);
}

Ast::Call& Ast::call(CellI& cell, CellI& method, Slot& slot1, Slot& slot2)
{
    return Call::New(kb, cell, method, slot1, slot2);
}

Ast::Call& Ast::call(CellI& cell, CellI& method, Slot& slot1, Slot& slot2, Slot& slot3)
{
    return Call::New(kb, cell, method, slot1, slot2, slot3);
}

Ast::Call& Ast::call(CellI& cell, CellI& method, Slot& slot1, Slot& slot2, Slot& slot3, Slot& slot4)
{
    return Call::New(kb, cell, method, slot1, slot2, slot3, slot4);
}

Ast::StaticCall& Ast::scall(CellI& cell, CellI& method)
{
    return StaticCall::New(kb, cell, method);
}

Ast::StaticCall& Ast::scall(CellI& cell, CellI& method, Slot& slot1)
{
    return StaticCall::New(kb, cell, method, slot1);
}

Ast::StaticCall& Ast::scall(CellI& cell, CellI& method, Slot& slot1, Slot& slot2)
{
    return StaticCall::New(kb, cell, method, slot1, slot2);
}

Ast::StaticCall& Ast::scall(CellI& cell, CellI& method, Slot& slot1, Slot& slot2, Slot& slot3)
{
    return StaticCall::New(kb, cell, method, slot1, slot2, slot3);
}

Ast::StaticCall& Ast::scall(CellI& cell, CellI& method, Slot& slot1, Slot& slot2, Slot& slot3, Slot& slot4)
{
    return StaticCall::New(kb, cell, method, slot1, slot2, slot3, slot4);
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

Ast::New& Ast::new_(Base& objectType)
{
    return New::NewT<Ast::New>::New(kb, objectType);
}

Ast::New& Ast::new_(Base& objectType, Base& constructor)
{
    return New::NewT<Ast::New>::New(kb, objectType, constructor);
}

Ast::New& Ast::new_(Base& objectType, Base& constructor, Slot& slot1)
{
    return New::NewT<Ast::New>::New(kb, objectType, constructor, slot1);
}

Ast::New& Ast::new_(Base& objectType, Base& constructor, Slot& slot1, Slot& slot2)
{
    return New::NewT<Ast::New>::New(kb, objectType, constructor, slot1, slot2);
}

Ast::New& Ast::new_(Base& objectType, Base& constructor, Slot& slot1, Slot& slot2, Slot& slot3)
{
    return New::NewT<Ast::New>::New(kb, objectType, constructor, slot1, slot2, slot3);
}

Ast::New& Ast::new_(Base& objectType, Base& constructor, Slot& slot1, Slot& slot2, Slot& slot3, Slot& slot4)
{
    return New::NewT<Ast::New>::New(kb, objectType, constructor, slot1, slot2, slot3, slot4);
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
    kb.type.Directions.set(kb.id.members, kb.list(up, down, left, right));
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
    auto unicodeCellIt = s_characters.find(utf32Char);
    if (unicodeCellIt != s_characters.end()) {
        return unicodeCellIt->second;
    } else {
        return kb.id.emptyObject;
    }
}

void Chars::registerUnicodeBlock(char32_t from, char32_t to)
{
    for (char32_t unicodeValue = from; unicodeValue <= to; ++unicodeValue) {
        std::string characterName = std::format("Unicode_{:#04x}", (int)unicodeValue);
        s_characters.emplace(std::piecewise_construct,
                             std::forward_as_tuple(unicodeValue),
                             std::forward_as_tuple(kb, kb.type.Char, characterName));
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

} // namespace pools

Pools::Pools(brain::Brain& kb) :
    chars(kb),
    digits(kb),
    numbers(kb)
{
}

Arc::Arc(brain::Brain& kb) :
    Demonstration(kb, kb.type.Type_, "Demonstration"),
    Task(kb, kb.type.Type_, "Task"),
    examples(kb, kb.type.Cell, "examples")
{
    auto& type = kb.type;
    auto& id   = kb.id;
    CellI* map = nullptr;

    map = &kb.map(kb.type.Cell, kb.type.Slot,
                  kb.id.input, type.slot(kb.id.input, kb.type.Picture),
                  kb.id.output, type.slot(kb.id.output, kb.type.Picture));
    Demonstration.set(id.slots, *map);

    map = &kb.map(kb.type.Cell, kb.type.Slot,
                  examples, type.slot(examples, kb.type.ListOf(Demonstration)),
                  kb.id.input, type.slot(kb.id.input, kb.type.Picture),
                  kb.id.output, type.slot(kb.id.output, kb.type.Picture));
    Task.set(id.slots, *map);
}

Test::Test(brain::Brain& kb) :
    factorial(kb, kb.type.Cell, "factorial")
{
}

Brain::Brain() :
    m_initPhase(InitPhase::Init),
    id(*this),
    type(*this),
    pools(*this),
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
    _9_(pools.numbers.get(9))

{
    const auto _     = [this](CellI& cell) -> Ast::Cell& { return ast.cell(cell); };
    const auto in_   = [this](CellI& role) -> Ast::Input& { return ast.input(role); };
    const auto out_  = [this](CellI& role) -> Ast::Output& { return ast.output(role); };
    const auto m_    = [this](CellI& role) -> Ast::Member& { return ast.member(role); };
    const auto var_  = [this](CellI& role) -> Ast::Var& { return ast.var(role); };
    const auto param = [this](CellI& role, CellI& value) -> Ast::Slot& { return ast.slot(role, value); };

    CellI* mapPtr = nullptr;

#pragma region ListItem
    mapPtr = &slots(type.slot(id.previous, type.ListItem),
                    type.slot(id.next, type.ListItem),
                    type.slot(id.value, type.Cell));
    type.ListItem.set(id.slots, *mapPtr);
    type.ListItem.set(id.memberOf, map(type.Type_, type.Type_, type.Iterator, type.Iterator));

    Ast::Function& listItemTemplate = *new Ast::Function(*this, type.ListItem, id.template_, "static ListItem::template");
    listItemTemplate.set(id.static_, boolean.true_);
    listItemTemplate.addInputs(list(
        param(id.objectType, type.Type_)));
    listItemTemplate.addOutputs(list(
        param(id.value, type.ListItem)));
    listItemTemplate.addBlock(ast.block(
        var_(id.result) = ast.new_(_(type.Type_)),

        ast.call(*var_(id.result), _(id.addMembership), param(_(id.cell), _(type.ListItem))),

        ast.call(*var_(id.result), _(id.addSlot), param(_(id.slotRole), _(id.previous)), param(_(id.slotType), *var_(id.result))),
        ast.call(*var_(id.result), _(id.addSlot), param(_(id.slotRole), _(id.next)), param(_(id.slotType), *var_(id.result))),
        ast.call(*var_(id.result), _(id.addSlot), param(_(id.slotRole), _(id.value)), param(_(id.slotType), in_(id.objectType))),

        ast.set(*var_(id.result), _(id.methods), m_(id.methods)),

        ast.return_(*var_(id.result))));

    Ast::Function& listItemCtor = *new Ast::Function(*this, type.ListItem, id.constructor, "ListItem::constructor");
    listItemCtor.addInputs(list(
        param(id.value, type.Cell)));
    listItemCtor.addBlock(ast.block(
        m_(id.value) = in_(id.value)));

    registerMethods(type.ListItem,
                    id.template_, listItemTemplate,
                    id.constructor, listItemCtor);

    mapPtr = &type.ListItem[id.methods];
    type.ListOf(type.Cell)[id.subTypes][id.index][id.itemType].set(id.methods, *mapPtr);
    type.ListOf(type.Slot)[id.subTypes][id.index][id.itemType].set(id.methods, *mapPtr);
    type.ListOf(type.Type_)[id.subTypes][id.index][id.itemType].set(id.methods, *mapPtr);
#pragma endregion
#pragma region List
    mapPtr = &slots(type.slot(id.first, type.ListItem),
                    type.slot(id.last, type.ListItem),
                    type.slot(id.size, type.Number),
                    type.slot(id.itemType, type.ListItem),
                    type.slot(id.objectType, type.Cell));
    type.List.set(id.slots, *mapPtr);
    type.List.set(id.subTypes, map(type.Cell, type.Type_, id.itemType, type.ListItem, id.objectType, type.Cell));
    type.List.set(id.memberOf, map(type.Type_, type.Type_, type.Container, type.Container));

    // Pre created:
    // Slot
    Ast::Function& listTemplate = *new Ast::Function(*this, type.List, id.template_, "static List::template");
    listTemplate.set(id.static_, boolean.true_);
    listTemplate.addInputs(list(
        param(id.objectType, type.Type_)));
    listTemplate.addOutputs(list(
        param(id.value, type.List)));
    listTemplate.addBlock(ast.block(
        var_(type.ListItem) = ast.scall(_(type.ListItem), _(id.template_), param(_(id.objectType), in_(id.objectType))),
        var_(id.result)     = ast.new_(_(type.Type_)),

        ast.call(*var_(id.result), _(id.addSubType), param(_(id.slotRole), _(id.itemType)), param(_(id.slotType), *var_(type.ListItem))),
        ast.call(*var_(id.result), _(id.addSubType), param(_(id.slotRole), _(id.objectType)), param(_(id.slotType), in_(id.objectType))),
        ast.call(*var_(id.result), _(id.addMembership), param(_(id.cell), _(type.List))),

        ast.call(*var_(id.result), _(id.addSlot), param(_(id.slotRole), _(id.first)), param(_(id.slotType), *var_(type.ListItem))),
        ast.call(*var_(id.result), _(id.addSlot), param(_(id.slotRole), _(id.last)), param(_(id.slotType), *var_(type.ListItem))),
        ast.call(*var_(id.result), _(id.addSlot), param(_(id.slotRole), _(id.size)), param(_(id.slotType), _(type.Number))),
        ast.call(*var_(id.result), _(id.addSlot), param(_(id.slotRole), _(id.itemType)), param(_(id.slotType), *var_(type.ListItem))),
        ast.call(*var_(id.result), _(id.addSlot), param(_(id.slotRole), _(id.objectType)), param(_(id.slotType), in_(id.objectType))),

        ast.set(*var_(id.result), _(id.methods), m_(id.methods)),

        ast.return_(*var_(id.result))));

    Ast::Function& listCtor = *new Ast::Function(*this, type.List, id.constructor, "List::constructor");
    listCtor.addBlock(ast.block(
        m_(id.size)       = _(_0_),
        m_(id.objectType) = m_(id.type) / _(id.subTypes) / _(id.index) / _(id.objectType),
        m_(id.itemType)   = m_(id.type) / _(id.subTypes) / _(id.index) / _(id.itemType)));

    Ast::Function& listAdd = *new Ast::Function(*this, type.List, id.add, "List::add");
    listAdd.addInputs(list(
        param(id.value, type.Cell)));
    listAdd.addOutputs(list(
        param(id.value, type.ListItem)));
    listAdd.addBlock(ast.block(
        var_(id.item) = ast.new_(m_(id.itemType), _(id.constructor), param(_(id.value), in_(id.value))),
        ast.if_(ast.not_(m_(id.first).exist()),
                m_(id.first) = *var_(id.item),                              // then
                ast.block(ast.set(m_(id.last), _(id.next), *var_(id.item)), // else
                          ast.set(*var_(id.item), _(id.previous), m_(id.last)))),
        m_(id.last) = *var_(id.item),
        m_(id.size) = ast.add(m_(id.size), _(_1_)),
        ast.return_(*var_(id.item))));
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
    Ast::Function& listRemove = *new Ast::Function(*this, type.List, id.remove, "List::remove");
    listRemove.addInputs(list(
        param(id.item, type.Cell)));
    listRemove.addBlock(ast.block(
        ast.if_(ast.has(in_(id.item), _(id.previous)),
                ast.if_(ast.has(in_(id.item), _(id.next)),
                        ast.set(in_(id.item) / _(id.previous), _(id.next), in_(id.item) / _(id.next)),
                        ast.erase(in_(id.item) / _(id.previous), _(id.next))),
                ast.if_(ast.has(in_(id.item), _(id.next)),
                        m_(id.first) = in_(id.item) / _(id.next),
                        ast.erase(ast.self(), _(id.first)))),
        ast.if_(ast.has(in_(id.item), _(id.next)),
                ast.if_(ast.has(in_(id.item), _(id.previous)),
                        ast.set(in_(id.item) / _(id.next), _(id.previous), in_(id.item) / _(id.previous)),
                        ast.erase(in_(id.item) / _(id.next), _(id.previous))),
                ast.if_(ast.has(in_(id.item), _(id.previous)),
                        m_(id.last) = in_(id.item) / _(id.previous),
                        ast.erase(ast.self(), _(id.last)))),
        m_(id.size) = ast.subtract(m_(id.size), _(_1_))));

    Ast::Function& listSize = *new Ast::Function(*this, type.List, id.size, "List::size");
    listSize.addOutputs(list(
        param(id.value, type.Number)));
    listSize.addBlock(ast.block(
        ast.return_(m_(id.size))));

    Ast::Function& listEmpty = *new Ast::Function(*this, type.List, id.empty, "List::empty");
    listEmpty.addOutputs(list(
        param(id.value, type.Boolean)));
    listEmpty.addBlock(ast.block(
        ast.if_(ast.equal(m_(id.size), _(_0_)), ast.return_(_(boolean.true_)), ast.return_(_(boolean.false_)))));

    registerMethods(type.List,
                    id.template_, listTemplate,
                    id.constructor, listCtor,
                    id.add, listAdd,
                    id.remove, listRemove,
                    id.size, listSize,
                    id.empty, listEmpty);

    mapPtr = &type.List[id.methods];
    type.ListOf(type.Cell).set(id.methods, *mapPtr);
    type.ListOf(type.Slot).set(id.methods, *mapPtr);
    type.ListOf(type.Type_).set(id.methods, *mapPtr);
#pragma endregion
#pragma region Type
    Ast::Function& typeCtor = *new Ast::Function(*this, type.Type_, id.constructor, "Type::constructor");
    typeCtor.addInputs(list(
        ast.slot(id.slots, type.MapCellToSlot),
        ast.slot(id.subTypes, type.MapCellToType),
        ast.slot(id.memberOf, type.MapTypeToType),
        ast.slot(id.methods, type.MapCellToOpFunction)));
    typeCtor.addBlock(ast.block(
        m_(id.slots)    = in_(id.slots),
        m_(id.subTypes) = in_(id.subTypes),
        m_(id.memberOf) = in_(id.memberOf),
        m_(id.methods)  = in_(id.methods))); // TODO we have to compile AST to OP here

    Ast::Function& typeAddSlot = *new Ast::Function(*this, type.Type_, id.addSlot, "Type::addSlot");
    typeAddSlot.addInputs(list(
        param(id.slotRole, type.Cell),
        param(id.slotType, type.Type_)));
    typeAddSlot.addBlock(ast.block(
        ast.if_(m_(id.slots).missing(), m_(id.slots) = ast.new_(_(type.MapCellToSlot), _(id.constructor))),
        var_(id.item) = ast.new_(_(type.Slot)),
        ast.set(*var_(id.item), _(id.slotRole), in_(id.slotRole)),
        ast.set(*var_(id.item), _(id.slotType), in_(id.slotType)),
        ast.call(m_(id.slots), _(id.add), ast.slot(_(id.key), in_(id.slotRole)), ast.slot(_(id.value), *var_(id.item)))));

    Ast::Function& typeAddSubType = *new Ast::Function(*this, type.Type_, id.addSubType, "Type::addSubType");
    typeAddSubType.addInputs(list(
        ast.slot(id.slotRole, type.Cell),
        ast.slot(id.slotType, type.Type_)));
    typeAddSubType.addBlock(ast.block(
        ast.if_(m_(id.subTypes).missing(), m_(id.subTypes) = ast.new_(_(type.MapCellToType), _(id.constructor))),
        ast.call(m_(id.subTypes), _(id.add), ast.slot(_(id.key), in_(id.slotRole)), ast.slot(_(id.value), in_(id.slotType)))));

    Ast::Function& typeAddMembership = *new Ast::Function(*this, type.Type_, id.addMembership, "Type::addMembership");
    typeAddMembership.addInputs(list(
        ast.slot(id.cell, type.Type_)));
    typeAddMembership.addBlock(ast.block(
        ast.if_(m_(id.memberOf).missing(), m_(id.memberOf) = ast.new_(_(type.MapTypeToType), _(id.constructor))),
        ast.call(m_(id.memberOf), _(id.add), ast.slot(_(id.key), in_(id.cell)), ast.slot(_(id.value), in_(id.cell)))));

    Ast::Function& typeAddSlots = *new Ast::Function(*this, type.Type_, id.addSlots, "Type::addSlots");
    typeAddSlots.addInputs(list(
        ast.slot(id.list, type.ListOf(type.Slot))));
    typeAddSlots.addBlock(ast.block(
        ast.if_(ast.equal(in_(id.list) / _(id.size), _(_0_)), ast.return_()),
        var_(id.item) = in_(id.list) / _(id.first),
        ast.if_(m_(id.slots).missing(), m_(id.slots) = ast.new_(_(type.MapCellToSlot), _(id.constructor))),
        ast.do_(ast.block(
                    var_(id.next) = _(boolean.true_),
                    ast.call(m_(id.slots), _(id.add), ast.slot(_(id.key), *var_(id.item) / _(id.value) / _(id.slotRole)), ast.slot(_(id.value), *var_(id.item) / _(id.value))),
                    ast.if_(ast.has(*var_(id.item), _(id.next)),
                            var_(id.item) = *var_(id.item) / _(id.next),
                            var_(id.next) = _(boolean.false_))),
                ast.same(*var_(id.next), _(boolean.true_)))));

    registerMethods(type.Type_,
                    id.constructor, typeCtor,
                    id.addSlot, typeAddSlot,
                    id.addSubType, typeAddSubType,
                    id.addMembership, typeAddMembership,
                    id.addSlots, typeAddSlots);
#pragma endregion
#pragma region Type2
    Ast::Function& type2Ctor = *new Ast::Function(*this, type.Type2_, id.constructor, "Type::constructor");
    type2Ctor.addBlock(ast.block(
        m_(id.slots) = ast.new_(_(type.Map2CellToSlot), _(id.constructor))));

    Ast::Function& type2CtorWithRecursiveType = *new Ast::Function(*this, type.Type2_, id.constructorWithRecursiveType, "Type::constructorWithRecursiveType");
    type2CtorWithRecursiveType.addBlock(ast.block(
        m_(id.slots) = ast.new_(_(type.Map2), _(id.constructorWithIndexType), param(_(id.indexType), ast.self()))));

    Ast::Function& type2AddSubType = *new Ast::Function(*this, type.Type2_, id.addSubType, "Type::addSubType");
    type2AddSubType.addInputs(list(
        param(id.slotRole, type.Cell),
        param(id.slotType, type.Type_)));
    type2AddSubType.addBlock(ast.block(
        ast.if_(m_(id.subTypes).missing(), m_(id.subTypes) = ast.new_(_(type.MapCellToType), _(id.constructor))),
        ast.call(m_(id.subTypes), _(id.add), param(_(id.key), in_(id.slotRole)), param(_(id.value), in_(id.slotType)))));

    Ast::Function& type2AddMembership = *new Ast::Function(*this, type.Type2_, id.addMembership, "Type::addMembership");
    type2AddMembership.addInputs(list(
        param(id.cell, type.Type_)));
    type2AddMembership.addBlock(ast.block(
        ast.if_(m_(id.memberOf).missing(), m_(id.memberOf) = ast.new_(_(type.MapTypeToType), _(id.constructor))),
        ast.call(m_(id.memberOf), _(id.add), param(_(id.key), in_(id.cell)), param(_(id.value), in_(id.cell)))));

    Ast::Function& type2AddSlot = *new Ast::Function(*this, type.Type2_, id.addSlot, "Type::addSlot");
    type2AddSlot.addInputs(list(
        param(id.slotRole, type.Cell),
        param(id.slotType, type.Slot)));
    type2AddSlot.addBlock(ast.block(
        ast.if_(m_(id.slots).missing(), m_(id.slots) = ast.new_(_(type.Map2CellToSlot), _(id.constructor))),
        ast.call(m_(id.slots), _(id.add), param(_(id.key), in_(id.slotRole)), param(_(id.value), in_(id.slotType)))));

    Ast::Function& type2AddSlots = *new Ast::Function(*this, type.Type2_, id.addSlots, "Type::addSlots");
    type2AddSlots.addInputs(list(
        ast.slot(id.list, type.ListOf(type.Slot))));
    type2AddSlots.addBlock(ast.block(
        ast.if_(ast.equal(in_(id.list) / _(id.size), _(_0_)), ast.return_()),
        var_(id.item) = in_(id.list) / _(id.first),
        ast.if_(m_(id.slots).missing(), m_(id.slots) = ast.new_(_(type.Map2CellToSlot), _(id.constructor))),
        ast.do_(ast.block(
                    var_(id.next) = _(boolean.true_),
                    ast.call(m_(id.slots), _(id.add), param(_(id.key), *var_(id.item) / _(id.value) / _(id.slotRole)), param(_(id.value), *var_(id.item) / _(id.value))),
                    ast.if_(ast.has(*var_(id.item), _(id.next)),
                            var_(id.item) = *var_(id.item) / _(id.next),
                            var_(id.next) = _(boolean.false_))),
                ast.same(*var_(id.next), _(boolean.true_)))));

    Ast::Function& type2HasSlot = *new Ast::Function(*this, type.Type2_, id.hasSlot, "Type::hasSlot");
    type2HasSlot.addInputs(list(
        param(id.slotRole, type.Cell)));
    type2HasSlot.addOutputs(list(
        param(id.value, type.Boolean)));
    type2HasSlot.addBlock(ast.block(
        ast.if_(m_(id.slots).missing(), ast.return_(_(boolean.false_))),
        ast.return_(ast.call(m_(id.slots), _(id.hasKey), param(_(id.key), in_(id.slotRole))))));

    Ast::Function& type2RemoveSlot = *new Ast::Function(*this, type.Type2_, id.removeSlot, "Type::removeSlot");
    type2RemoveSlot.addInputs(list(
        param(id.slotRole, type.Cell)));
    type2RemoveSlot.addBlock(ast.block(
        ast.if_(m_(id.slots).missing(), ast.return_()),
        ast.call(m_(id.slots), _(id.remove), param(_(id.key), in_(id.slotRole)))));

    registerMethods(type.Type2_,
                    id.constructor, type2Ctor,
                    id.constructorWithRecursiveType, type2CtorWithRecursiveType,
                    id.addSubType, type2AddSubType,
                    id.addMembership, type2AddMembership,
                    id.addSlot, type2AddSlot,
                    id.addSlots, type2AddSlots,
                    id.hasSlot, type2HasSlot,
                    id.removeSlot, type2RemoveSlot);
#pragma endregion
#pragma region Index
    type.Index.set(id.memberOf, map(type.Type2_, type.Type2_));

    Ast::Function& indexCtor = *new Ast::Function(*this, type.Index, id.constructor, "Index::constructor");
    indexCtor.addBlock(ast.block(
        ast.set(ast.self(), m_(id.type), ast.new_(_(type.Type2_), _(id.constructorWithRecursiveType))),
        ast.set(m_(id.type), _(id.memberOf), _(map(type.Type2_, type.Type2_, type.Index, type.Index)))));

    Ast::Function& indexCtorWithSelfType = *new Ast::Function(*this, type.Index, id.constructorWithSelfType, "Index::constructorWithSelfType");
    indexCtorWithSelfType.addInputs(list(
        param(id.indexType, type.Type_)));
    indexCtorWithSelfType.addBlock(ast.block(
        ast.if_(ast.missing(in_(id.indexType), _(id.sharedObject)),
                ast.block(ast.set(in_(id.indexType), _(id.sharedObject), ast.new_(_(type.Slot))),
                          ast.set(in_(id.indexType) / _(id.sharedObject), _(id.slotRole), ast.self()),
                          ast.set(in_(id.indexType) / _(id.sharedObject), _(id.slotType), _(type.Index)))),
        ast.set(ast.self(), m_(id.type), in_(id.indexType))));

    /*
    void Index::insert(CellI& key, CellI& value)
    {
        if (&key == &kb.id.type) {
            throw "The type key can not be changed!";
        }
        m_slots[&key] = &value;
        if (m_recursiveType) {
            return;
        }
        Object& slot = *new Object(kb, kb.type.Slot);
        slot.set(kb.id.slotRole, key);
        slot.set(kb.id.slotType, kb.type.Slot);
        m_type->addSlot(key, slot);
    }
    */
    Ast::Function& indexInsert = *new Ast::Function(*this, type.Index, id.insert, "Index::insert");
    indexInsert.addInputs(list(
        param(id.key, type.Cell),
        param(id.value, type.Cell)));
    indexInsert.addBlock(ast.block(
        ast.if_(ast.equal(in_(id.key), _(id.type)), ast.return_()),
        ast.set(ast.self(), in_(id.key), in_(id.value)),
        ast.if_(ast.and_(ast.has(m_(id.type), _(id.sharedObject)), ast.equal(m_(id.type) / _(id.sharedObject) / _(id.slotRole), ast.self())),
            ast.return_()),
        var_(id.slot) = ast.new_(_(type.Slot)),
        ast.set(*var_(id.slot), _(id.slotRole), in_(id.key)),
        ast.set(*var_(id.slot), _(id.slotType), _(type.Slot)),
        ast.call(m_(id.type), _(id.addSlot), param(_(id.slotRole), in_(id.key)), param(_(id.slotType), *var_(id.slot)))));

    Ast::Function& indexEmpty = *new Ast::Function(*this, type.Index, id.size, "Index::empty");
    indexEmpty.addOutputs(list(
        param(id.value, type.Number)));
    indexEmpty.addBlock(ast.block(
        ast.return_(ast.call(m_(id.type) / _(id.slots), _(id.empty)))));

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
    Ast::Function& indexRemove = *new Ast::Function(*this, type.Index, id.remove, "Index::remove");
    indexRemove.addInputs(list(
        param(id.key, type.Cell)));
    indexRemove.addBlock(ast.block(
        ast.if_(ast.not_(ast.call(m_(id.type), _(id.hasSlot), param(_(id.slotRole), in_(id.key)))),
                ast.return_()),
        ast.erase(ast.self(), in_(id.key)),
        ast.call(m_(id.type), _(id.removeSlot), param(_(id.slotRole), in_(id.key)))));

    Ast::Function& indexSize = *new Ast::Function(*this, type.Index, id.size, "Index::size");
    indexSize.addOutputs(list(
        param(id.value, type.Number)));
    indexSize.addBlock(ast.block(
        ast.return_(ast.call(m_(id.type) / _(id.slots), _(id.size)))));

    registerMethods(type.Index,
                    id.constructor, indexCtor,
                    id.constructorWithSelfType, indexCtorWithSelfType,
                    id.empty, indexEmpty,
                    id.insert, indexInsert,
                    id.remove, indexRemove,
                    id.size, indexSize);
#pragma endregion
#pragma region Map
    mapPtr = &slots(type.slot(id.keyType, type.Cell),
                    type.slot(id.objectType, type.Cell),
                    type.slot(id.list, type.ListOf(type.Cell)),
                    type.slot(id.listType, type.Type_),
                    type.slot(id.index, type.Index),
                    type.slot(id.indexType, type.Type_),
                    type.slot(id.size, type.Number));
    type.Map.set(id.slots, *mapPtr);
    mapPtr = &map(type.Cell, type.Type_,
                  id.keyType, type.Cell,
                  id.objectType, type.Cell,
                  id.listType, type.ListOf(type.Cell));
    type.Map.set(id.subTypes, *mapPtr);
    type.Map.set(id.memberOf, map(type.Type_, type.Type_, type.Container, type.Container));

    Ast::Function& mapCtor = *new Ast::Function(*this, type.Map, id.constructor, "Map::constructor");
    mapCtor.addBlock(ast.block(
        m_(id.size)       = _(_0_),
        m_(id.keyType)    = m_(id.type) / _(id.subTypes) / _(id.index) / _(id.keyType),
        m_(id.objectType) = m_(id.type) / _(id.subTypes) / _(id.index) / _(id.objectType),
        m_(id.listType)   = m_(id.type) / _(id.subTypes) / _(id.index) / _(id.listType),
        m_(id.list)       = ast.new_(m_(id.listType), _(id.constructor)),
        m_(id.indexType)  = ast.new_(_(type.Type_)),
        ast.set(m_(id.indexType), _(id.slots), ast.new_(_(type.MapCellToSlot))),
        ast.set(m_(id.indexType) / _(id.slots), _(id.size), _(_0_)),

        ast.set(m_(id.indexType) / _(id.slots), _(id.indexType), ast.new_(_(type.Type_))),
        ast.set(m_(id.indexType) / _(id.slots) / _(id.indexType), _(id.slots), ast.new_(_(type.MapCellToSlot))),
        ast.set(m_(id.indexType) / _(id.slots) / _(id.indexType) / _(id.slots), _(id.size), _(_0_)),
        ast.set(m_(id.indexType) / _(id.slots) / _(id.indexType) / _(id.slots), _(id.keyType), _(type.Cell)),
        ast.set(m_(id.indexType) / _(id.slots) / _(id.indexType) / _(id.slots), _(id.objectType), _(type.Slot)),
        ast.set(m_(id.indexType) / _(id.slots) / _(id.indexType) / _(id.slots), _(id.listType), _(type.ListOf(type.Slot))),
        ast.set(m_(id.indexType) / _(id.slots) / _(id.indexType) / _(id.slots), _(id.list), ast.new_(_(type.ListOf(type.Slot)), _(id.constructor))),
        ast.set(m_(id.indexType) / _(id.slots) / _(id.indexType) / _(id.slots), _(id.index), ast.new_(m_(id.indexType) / _(id.slots) / _(id.indexType))),
        ast.set(m_(id.indexType) / _(id.slots) / _(id.indexType), _(id.memberOf), _(map(type.Type_, type.Type_, type.Index, type.Index))),

        ast.set(m_(id.indexType) / _(id.slots), _(id.keyType), _(type.Cell)),
        ast.set(m_(id.indexType) / _(id.slots), _(id.objectType), _(type.Slot)),
        ast.set(m_(id.indexType) / _(id.slots), _(id.listType), _(type.ListOf(type.Slot))),
        ast.set(m_(id.indexType) / _(id.slots), _(id.list), ast.new_(_(type.ListOf(type.Slot)), _(id.constructor))),
        ast.set(m_(id.indexType) / _(id.slots), _(id.index), ast.new_(m_(id.indexType) / _(id.slots) / _(id.indexType))),
        ast.set(m_(id.indexType), _(id.memberOf), _(map(type.Type_, type.Type_, type.Index, type.Index))),
        m_(id.index) = ast.new_(m_(id.indexType))));

    // MapCellToSlot;
    // MapCellToType;
    // MapCellToAstFunction;
    // MapCellToOpFunction;
    // MapCellToOpVar;
    // MapCellToOpBase;
    // MapTypeToType;
    Ast::Function& mapTemplate = *new Ast::Function(*this, type.Map, id.template_, "static Map::template");
    mapTemplate.set(id.static_, boolean.true_);
    mapTemplate.addInputs(list(
        ast.slot(id.keyType, type.Type_),
        ast.slot(id.objectType, type.Type_)));
    mapTemplate.addOutputs(list(
        ast.slot(id.value, type.Map)));
    mapTemplate.addBlock(ast.block(
        var_(id.result)   = ast.new_(_(type.Type_)),
        var_(id.listType) = ast.scall(_(type.List), _(id.template_), ast.slot(_(id.objectType), in_(id.objectType))),

        ast.call(*var_(id.result), _(id.addSubType), ast.slot(_(id.slotRole), _(id.keyType)), ast.slot(_(id.slotType), in_(id.keyType))),
        ast.call(*var_(id.result), _(id.addSubType), ast.slot(_(id.slotRole), _(id.objectType)), ast.slot(_(id.slotType), in_(id.objectType))),
        ast.call(*var_(id.result), _(id.addSubType), ast.slot(_(id.slotRole), _(id.listType)), ast.slot(_(id.slotType), *var_(id.listType))),

        ast.call(*var_(id.result), _(id.addMembership), ast.slot(_(id.cell), _(type.Map))),

        ast.call(*var_(id.result), _(id.addSlot), ast.slot(_(id.slotRole), _(id.keyType)), ast.slot(_(id.slotType), in_(id.keyType))),
        ast.call(*var_(id.result), _(id.addSlot), ast.slot(_(id.slotRole), _(id.objectType)), ast.slot(_(id.slotType), in_(id.objectType))),
        ast.call(*var_(id.result), _(id.addSlot), ast.slot(_(id.slotRole), _(id.list)), ast.slot(_(id.slotType), *var_(id.listType))),
        ast.call(*var_(id.result), _(id.addSlot), ast.slot(_(id.slotRole), _(id.listType)), ast.slot(_(id.slotType), *var_(id.listType))),
        ast.call(*var_(id.result), _(id.addSlot), ast.slot(_(id.slotRole), _(id.index)), ast.slot(_(id.slotType), _(type.Index))),
        ast.call(*var_(id.result), _(id.addSlot), ast.slot(_(id.slotRole), _(id.indexType)), ast.slot(_(id.slotType), _(type.Type_))),
        ast.call(*var_(id.result), _(id.addSlot), ast.slot(_(id.slotRole), _(id.size)), ast.slot(_(id.slotType), _(type.Number))),

        ast.set(*var_(id.result), _(id.methods), m_(id.methods)),

        ast.return_(*var_(id.result))));

    Ast::Function& mapAdd = *new Ast::Function(*this, type.Map, id.add, "Map::add");
    mapAdd.addInputs(list(
        ast.slot(id.key, type.Cell),
        ast.slot(id.value, type.Cell)));
    mapAdd.addBlock(ast.block(
        m_(id.list).call(_(id.add), ast.slot(_(id.value), in_(id.value))),
        m_(id.size) = ast.add(m_(id.size), _(_1_)),

        var_(type.Slot) = ast.new_(_(type.Slot)),
        ast.set(*var_(type.Slot), _(id.slotRole), in_(id.key)),
        ast.set(*var_(type.Slot), _(id.slotType), _(type.Slot)),
        ast.call(m_(id.indexType) / _(id.slots) / _(id.indexType) / _(id.slots) / _(id.list), _(id.add), ast.slot(_(id.value), *var_(type.Slot))),
        ast.set(m_(id.indexType) / _(id.slots) / _(id.indexType) / _(id.slots) / _(id.index), in_(id.key), *var_(type.Slot)),
        ast.set(m_(id.indexType) / _(id.slots) / _(id.indexType) / _(id.slots), _(id.size), m_(id.size)),

        var_(type.Slot) = ast.new_(_(type.Slot)),
        ast.set(*var_(type.Slot), _(id.slotRole), in_(id.key)),
        ast.set(*var_(type.Slot), _(id.slotType), m_(id.objectType)),
        ast.call(m_(id.indexType) / _(id.slots) / _(id.list), _(id.add), ast.slot(_(id.value), *var_(type.Slot))),
        ast.set(m_(id.indexType) / _(id.slots) / _(id.index), in_(id.key), *var_(type.Slot)),
        ast.set(m_(id.indexType) / _(id.slots), _(id.size), m_(id.size)),

        ast.set(m_(id.index), in_(id.key), in_(id.value))));

    Ast::Function& mapSize = *new Ast::Function(*this, type.Map, id.size, "Map::size");
    mapSize.addOutputs(list(
        ast.slot(id.value, type.Number)));
    mapSize.addBlock(ast.block(
        ast.return_(m_(id.size))));

    Ast::Function& mapEmpty = *new Ast::Function(*this, type.Map, id.empty, "Map::empty");
    mapEmpty.addOutputs(list(
        ast.slot(id.value, type.Boolean)));
    mapEmpty.addBlock(ast.block(
        ast.if_(ast.equal(m_(id.size), _(_0_)), ast.return_(_(boolean.true_)), ast.return_(_(boolean.false_)))));

    registerMethods(type.Map,
                    id.constructor, mapCtor,
                    id.template_, mapTemplate,
                    id.add, mapAdd,
                    id.size, mapSize,
                    id.empty, mapEmpty);

    // Map<Cell, Slot>
    type.MapCellToSlot.set(id.subTypes, map(type.Cell, type.Type_, id.keyType, type.Cell, id.objectType, type.Slot, id.listType, type.ListOf(type.Slot)));
    type.MapCellToSlot.set(id.memberOf, map(type.Type_, type.Type_, type.Map, type.Map));
    mapPtr = &slots(type.slot(id.list, type.ListOf(type.Slot)),
                    type.slot(id.listType, type.ListOf(type.Slot)),
                    type.slot(id.index, type.Index),
                    type.slot(id.indexType, type.Type_),
                    type.slot(id.keyType, type.Cell),
                    type.slot(id.objectType, type.Slot),
                    type.slot(id.size, type.Number));
    type.MapCellToSlot.set(id.slots, *mapPtr);
    type.MapCellToSlot.set(id.methods, map(type.Cell, type.op.Function, id.constructor, mapCtor.compile(type.Map), id.size, mapSize.compile(type.Map), id.add, mapAdd.compile(type.Map), id.empty, mapEmpty.compile(type.Map)));

    // Map<Cell, Type>
    type.MapCellToType.set(id.subTypes, map(type.Cell, type.Type_, id.keyType, type.Cell, id.objectType, type.Type_, id.listType, type.ListOf(type.Type_)));
    type.MapCellToType.set(id.memberOf, map(type.Type_, type.Type_, type.Map, type.Map));
    mapPtr = &map(type.Cell, type.Type_,
                  id.list, type.slot(id.list, type.ListOf(type.Type_)),
                  id.listType, type.slot(id.listType, type.ListOf(type.Type_)),
                  id.index, type.slot(id.index, type.Index),
                  id.indexType, type.slot(id.indexType, type.Type_),
                  id.keyType, type.slot(id.keyType, type.Cell),
                  id.objectType, type.slot(id.objectType, type.Type_),
                  id.size, type.slot(id.size, type.Number));
    type.MapCellToType.set(id.slots, *mapPtr);
    type.MapCellToType.set(id.methods, map(type.Cell, type.op.Function, id.constructor, mapCtor.compile(type.Map), id.size, mapSize.compile(type.Map), id.add, mapAdd.compile(type.Map), id.empty, mapEmpty.compile(type.Map)));

    // Map<Type, Type>
    type.MapTypeToType.set(id.subTypes, map(type.Cell, type.Type_, id.keyType, type.Type_, id.objectType, type.Type_, id.listType, type.ListOf(type.Type_)));
    type.MapTypeToType.set(id.memberOf, map(type.Type_, type.Type_, type.Map, type.Map));
    mapPtr = &map(type.Type_, type.Type_,
                  id.list, type.slot(id.list, type.ListOf(type.Type_)),
                  id.listType, type.slot(id.listType, type.ListOf(type.Type_)),
                  id.index, type.slot(id.index, type.Index),
                  id.indexType, type.slot(id.indexType, type.Type_),
                  id.keyType, type.slot(id.keyType, type.Type_),
                  id.objectType, type.slot(id.objectType, type.Type_),
                  id.size, type.slot(id.size, type.Number));
    type.MapTypeToType.set(id.slots, *mapPtr);
    type.MapTypeToType.set(id.methods, map(type.Cell, type.op.Function, id.constructor, mapCtor.compile(type.Map), id.size, mapSize.compile(type.Map), id.add, mapAdd.compile(type.Map), id.empty, mapEmpty.compile(type.Map)));
#pragma endregion
#pragma region Map2
    mapPtr = &slots(type.slot(id.keyType, type.Cell),
                    type.slot(id.objectType, type.Cell),
                    type.slot(id.list, type.ListOf(type.Cell)),
                    type.slot(id.index, type.Index),
                    type.slot(id.size, type.Number));
    type.Map2.set(id.slots, *mapPtr);
    mapPtr = &map(type.Cell, type.Type_,
                  id.keyType, type.Cell,
                  id.objectType, type.Cell,
                  id.listType, type.ListOf(type.Cell));
    type.Map2.set(id.subTypes, *mapPtr);
    type.Map2.set(id.memberOf, map(type.Type_, type.Type_, type.Container, type.Container));

    Ast::Function& map2Ctor = *new Ast::Function(*this, type.Map2, id.constructor, "Map::constructor");
    map2Ctor.addBlock(ast.block(
        m_(id.size)       = _(_0_),
        m_(id.keyType)    = m_(id.type) / _(id.subTypes) / _(id.index) / _(id.keyType),
        m_(id.objectType) = m_(id.type) / _(id.subTypes) / _(id.index) / _(id.objectType),
        m_(id.list)       = ast.new_(m_(id.type) / _(id.subTypes) / _(id.index) / _(id.listType), _(id.constructor)),
        m_(id.index)      = ast.new_(_(type.Index), _(id.constructor))));

    Ast::Function& map2CtorWithIndexType = *new Ast::Function(*this, type.Map2, id.constructorWithIndexType, "Map::constructorWithIndexType");
    map2CtorWithIndexType.addInputs(list(
        param(id.indexType, type.Type_)));
    map2CtorWithIndexType.addBlock(ast.block(
        m_(id.size)       = _(_0_),
        m_(id.keyType)    = m_(id.type) / _(id.subTypes) / _(id.index) / _(id.keyType),
        m_(id.objectType) = m_(id.type) / _(id.subTypes) / _(id.index) / _(id.objectType),
        m_(id.list)       = ast.new_(m_(id.type) / _(id.subTypes) / _(id.index) / _(id.listType), _(id.constructor)),
        m_(id.index)      = ast.new_(_(type.Index), _(id.constructorWithSelfType), param(_(id.indexType), in_(id.indexType)))));

    Ast::Function& map2Template = *new Ast::Function(*this, type.Map2, id.template_, "static Map::template");
    map2Template.set(id.static_, boolean.true_);
    map2Template.addInputs(list(
        ast.slot(id.keyType, type.Type_),
        ast.slot(id.objectType, type.Type_)));
    map2Template.addOutputs(list(
        ast.slot(id.value, type.Map)));
    map2Template.addBlock(ast.block(
        var_(id.result)   = ast.new_(_(type.Type_)),
        var_(id.listType) = ast.scall(_(type.List), _(id.template_), ast.slot(_(id.objectType), in_(id.objectType))),

        ast.call(*var_(id.result), _(id.addSubType), ast.slot(_(id.slotRole), _(id.keyType)), ast.slot(_(id.slotType), in_(id.keyType))),
        ast.call(*var_(id.result), _(id.addSubType), ast.slot(_(id.slotRole), _(id.objectType)), ast.slot(_(id.slotType), in_(id.objectType))),
        ast.call(*var_(id.result), _(id.addSubType), ast.slot(_(id.slotRole), _(id.listType)), ast.slot(_(id.slotType), *var_(id.listType))),

        ast.call(*var_(id.result), _(id.addMembership), ast.slot(_(id.cell), _(type.Map))),

        ast.call(*var_(id.result), _(id.addSlot), ast.slot(_(id.slotRole), _(id.keyType)), ast.slot(_(id.slotType), in_(id.keyType))),
        ast.call(*var_(id.result), _(id.addSlot), ast.slot(_(id.slotRole), _(id.objectType)), ast.slot(_(id.slotType), in_(id.objectType))),
        ast.call(*var_(id.result), _(id.addSlot), ast.slot(_(id.slotRole), _(id.list)), ast.slot(_(id.slotType), *var_(id.listType))),
        ast.call(*var_(id.result), _(id.addSlot), ast.slot(_(id.slotRole), _(id.listType)), ast.slot(_(id.slotType), *var_(id.listType))),
        ast.call(*var_(id.result), _(id.addSlot), ast.slot(_(id.slotRole), _(id.index)), ast.slot(_(id.slotType), _(type.Index))),
        ast.call(*var_(id.result), _(id.addSlot), ast.slot(_(id.slotRole), _(id.indexType)), ast.slot(_(id.slotType), _(type.Type_))),
        ast.call(*var_(id.result), _(id.addSlot), ast.slot(_(id.slotRole), _(id.size)), ast.slot(_(id.slotType), _(type.Number))),

        ast.set(*var_(id.result), _(id.methods), m_(id.methods)),

        ast.return_(*var_(id.result))));
    /*
    bool Map::hasKey(CellI& key)
    {
        return m_index.has(key);
    }
    */
    Ast::Function& map2HasKey = *new Ast::Function(*this, type.Map2, id.hasKey, "Map::hasKey");
    map2HasKey.addInputs(list(
        param(id.key, type.Cell)));
    map2HasKey.addOutputs(list(
        ast.slot(id.value, type.Boolean)));
    map2HasKey.addBlock(ast.block(
        ast.if_(ast.has(m_(id.index), in_(id.key)), ast.return_(_(boolean.true_)), ast.return_(_(boolean.false_)))));

    /*
    CellI& Map::getValue(CellI& key)
    {
        if (m_index.has(key)) {
            return m_index[key][kb.id.value];
        }
        throw "No such role!";
    }
    */
    Ast::Function& map2GetValue = *new Ast::Function(*this, type.Map2, id.hasKey, "Map::getValue");
    map2GetValue.addInputs(list(
        param(id.key, type.Cell)));
    map2GetValue.addBlock(ast.block(
        ast.if_(ast.has(m_(id.index), in_(id.key)), ast.return_(m_(id.index) / in_(id.key) / _(id.value)), ast.return_(_(id.emptyObject)))));


    /*
    void Map::add(CellI& key, CellI& value)
    {
        if (&key == &kb.id.type) {
            throw "id.type can not be stored in a map!";
        }
        if (m_index.has(key)) {
            throw "A value already registered with this role";
        }
        List::Item& item = *m_list.add(value);
        m_index.insert(key, item);
        ++m_size;
    }
    */
    Ast::Function& map2Add = *new Ast::Function(*this, type.Map2, id.add, "Map::add");
    map2Add.addInputs(list(
        param(id.key, type.Cell),
        param(id.value, type.Cell)));
    map2Add.addBlock(ast.block(
        ast.if_(ast.equal(in_(id.key), _(id.type)), ast.return_()),
        ast.if_(ast.has(m_(id.index), in_(id.key)), ast.return_()),
        var_(id.item) = m_(id.list).call(_(id.add), param(_(id.value), in_(id.value))),
        ast.call(m_(id.index), _(id.insert), param(_(id.key), in_(id.key)), param(_(id.value), *var_(id.item))),
        m_(id.size) = ast.add(m_(id.size), _(_1_))));

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
    Ast::Function& map2Remove = *new Ast::Function(*this, type.Map2, id.remove, "Map::remove");
    map2Remove.addInputs(list(
        param(id.key, type.Cell)));
    map2Remove.addBlock(ast.block(
        ast.if_(ast.missing(m_(id.index), in_(id.key)),
                ast.return_()),
        ast.call(m_(id.list), _(id.remove), param(_(id.item), m_(id.index) / in_(id.key))),
        ast.call(m_(id.index), _(id.remove), param(_(id.key), in_(id.key))),
        m_(id.size) = ast.subtract(m_(id.size), _(_1_))));

    Ast::Function& map2Size = *new Ast::Function(*this, type.Map2, id.size, "Map::size");
    map2Size.addOutputs(list(
        param(id.value, type.Number)));
    map2Size.addBlock(ast.block(
        ast.return_(m_(id.size))));

    Ast::Function& map2Empty = *new Ast::Function(*this, type.Map2, id.empty, "Map::empty");
    map2Empty.addOutputs(list(
        param(id.value, type.Boolean)));
    map2Empty.addBlock(ast.block(
        ast.if_(ast.equal(m_(id.size), _(_0_)), ast.return_(_(boolean.true_)), ast.return_(_(boolean.false_)))));

    registerMethods(type.Map2,
                    id.constructor, map2Ctor,
                    id.constructorWithIndexType, map2CtorWithIndexType,
                    id.template_, map2Template,
                    id.hasKey, map2HasKey,
                    id.getValue, map2GetValue,
                    id.add, map2Add,
                    id.remove, map2Remove,
                    id.size, map2Size,
                    id.empty, map2Empty);

    // Map<Cell, Slot>
    type.Map2CellToSlot.set(id.subTypes, map(type.Cell, type.Type2_, id.keyType, type.Cell, id.objectType, type.Slot, id.listType, type.ListOf(type.Slot)));
    type.Map2CellToSlot.set(id.memberOf, map(type.Type2_, type.Type2_, type.Map2, type.Map2));
    mapPtr = &slots(type.slot(id.list, type.ListOf(type.Slot)),
                    type.slot(id.index, type.Index),
                    type.slot(id.keyType, type.Cell),
                    type.slot(id.objectType, type.Slot),
                    type.slot(id.size, type.Number));
    type.Map2CellToSlot.set(id.slots, *mapPtr);
    registerMethods(type.Map2CellToSlot,
                    id.constructor, map2Ctor,
                    id.constructorWithIndexType, map2CtorWithIndexType,
                    id.template_, map2Template,
                    id.hasKey, map2HasKey,
                    id.getValue, map2GetValue,
                    id.add, map2Add,
                    id.remove, map2Remove,
                    id.size, map2Size,
                    id.empty, map2Empty);


 #pragma endregion
#pragma region Set
    mapPtr = &slots(type.slot(id.objectType, type.Cell),
                    type.slot(id.list, type.ListOf(type.Cell)),
                    type.slot(id.listType, type.Type_),
                    type.slot(id.index, type.Index),
                    type.slot(id.indexType, type.Type_),
                    type.slot(id.size, type.Number));
    type.Set.set(id.slots, *mapPtr);
    mapPtr = &map(type.Cell, type.Type_,
                  id.objectType, type.Cell,
                  id.listType, type.ListOf(type.Cell));
    type.Set.set(id.subTypes, *mapPtr);
    type.Set.set(id.memberOf, map(type.Type_, type.Type_, type.Container, type.Container));
    Ast::Function& setCtor = *new Ast::Function(*this, type.Set, id.constructor, "Set::constructor");
    setCtor.addBlock(ast.block(
        m_(id.size) = _(_0_),
        m_(id.objectType)   = m_(id.type) / _(id.subTypes) / _(id.index) / _(id.objectType),
        m_(id.listType)     = m_(id.type) / _(id.subTypes) / _(id.index) / _(id.listType),
        m_(id.list)         = ast.new_(m_(id.listType), _(id.constructor)),
        m_(id.indexType)    = ast.new_(_(type.Type_)),
        ast.set(m_(id.indexType), _(id.slots), ast.new_(_(type.MapCellToSlot), _(id.constructor))),
        ast.set(m_(id.indexType), _(id.memberOf), _(map(type.Type_, type.Type_, type.Index, type.Index))),
        m_(id.index) = ast.new_(m_(id.indexType))));

    // MapCellToSlot;
    // MapCellToType;
    // MapCellToAstFunction;
    // MapCellToOpFunction;
    // MapCellToOpVar;
    // MapCellToOpBase;
    // MapTypeToType;
    Ast::Function& setTemplate = *new Ast::Function(*this, type.Set, id.template_, "static Set::template");
    setTemplate.set(id.static_, boolean.true_);
    setTemplate.addInputs(list(
        ast.slot(id.objectType, type.Type_)));
    setTemplate.addOutputs(list(
        ast.slot(id.value, type.Set)));
    setTemplate.addBlock(ast.block(
        var_(id.result)   = ast.new_(_(type.Type_)),
        var_(id.listType) = ast.scall(_(type.List), _(id.template_), ast.slot(_(id.objectType), in_(id.objectType))),

        ast.call(*var_(id.result), _(id.addSubType), ast.slot(_(id.slotRole), _(id.objectType)), ast.slot(_(id.slotType), in_(id.objectType))),
        ast.call(*var_(id.result), _(id.addSubType), ast.slot(_(id.slotRole), _(id.listType)), ast.slot(_(id.slotType), *var_(id.listType))),

        ast.call(*var_(id.result), _(id.addMembership), ast.slot(_(id.cell), _(type.Map))),

        ast.call(*var_(id.result), _(id.addSlot), ast.slot(_(id.slotRole), _(id.objectType)), ast.slot(_(id.slotType), in_(id.objectType))),
        ast.call(*var_(id.result), _(id.addSlot), ast.slot(_(id.slotRole), _(id.list)), ast.slot(_(id.slotType), *var_(id.listType))),
        ast.call(*var_(id.result), _(id.addSlot), ast.slot(_(id.slotRole), _(id.listType)), ast.slot(_(id.slotType), *var_(id.listType))),
        ast.call(*var_(id.result), _(id.addSlot), ast.slot(_(id.slotRole), _(id.index)), ast.slot(_(id.slotType), _(type.Index))),
        ast.call(*var_(id.result), _(id.addSlot), ast.slot(_(id.slotRole), _(id.indexType)), ast.slot(_(id.slotType), _(type.Type_))),
        ast.call(*var_(id.result), _(id.addSlot), ast.slot(_(id.slotRole), _(id.size)), ast.slot(_(id.slotType), _(type.Number))),

        ast.set(*var_(id.result), _(id.methods), m_(id.methods)),

        ast.return_(*var_(id.result))));

    Ast::Function& setAdd = *new Ast::Function(*this, type.Set, id.add, "Set::add");
    setAdd.addInputs(list(
        ast.slot(id.value, type.Cell)));
    setAdd.addOutputs(list(
        ast.slot(id.value, type.ListItem)));
    setAdd.addBlock(ast.block(
        ast.if_(ast.has(m_(id.index), in_(id.value)),
                ast.return_(m_(id.index) / in_(id.value))),
        var_(id.listItem)   = m_(id.list).call(_(id.add), ast.slot(_(id.value), in_(id.value))),
        m_(id.size) = ast.add(m_(id.size), _(_1_)),
        ast.call(m_(id.indexType) / _(id.slots), _(id.add), ast.slot(_(id.key), in_(id.value)), ast.slot(_(id.value), *var_(id.listItem))),
        ast.set(m_(id.index), in_(id.value), *var_(id.listItem)),
        ast.return_(*var_(id.listItem))));

    Ast::Function& setContains = *new Ast::Function(*this, type.Set, id.contains, "Set::contains");
    setContains.addInputs(list(
        ast.slot(id.value, type.Cell)));
    setContains.addOutputs(list(
        ast.slot(id.value, type.Boolean)));
    setContains.addBlock(ast.block(
        ast.if_(ast.has(m_(id.index), in_(id.value)),
                ast.return_(_(boolean.true_)),
                ast.return_(_(boolean.false_)))));

    Ast::Function& setErase = *new Ast::Function(*this, type.Set, id.add, "Set::erase");
    setErase.addInputs(list(
        ast.slot(id.value, type.Cell)));
    setErase.addOutputs(list(
        ast.slot(id.value, type.ListItem)));
    setErase.addBlock(ast.block(
        ast.if_(ast.missing(m_(id.index), in_(id.value)),
                ast.return_(_(id.emptyObject))),
        ast.erase(m_(id.index), in_(id.value)),
        m_(id.size) = ast.subtract(m_(id.size), _(_1_))));


    Ast::Function& setSize = *new Ast::Function(*this, type.Set, id.size, "Set::size");
    setSize.addOutputs(list(
        ast.slot(id.value, type.Number)));
    setSize.addBlock(ast.block(
        ast.return_(m_(id.size))));

    Ast::Function& setEmpty = *new Ast::Function(*this, type.Set, id.empty, "Set::empty");
    setEmpty.addOutputs(list(
        ast.slot(id.value, type.Boolean)));
    setEmpty.addBlock(ast.block(
        ast.if_(ast.equal(m_(id.size), _(_0_)), ast.return_(_(boolean.true_)), ast.return_(_(boolean.false_)))));

    registerMethods(type.Set,
                    id.constructor, setCtor,
                    id.template_, setTemplate,
                    id.add, setAdd,
                    id.contains, setContains,
                    id.size, setSize,
                    id.empty, setEmpty);
#pragma endregion

#if 0
    Object methodData(*this, type.Cell);
    methodData.set(id.ast, listAdd);
    methodData.set(id.op, listAdd.compile());
    methodData.set(id.static_, boolean.false_);
    methodData.set(id.const_, boolean.false_);
#endif
    mapPtr = &slots(type.slot(id.value, type.ListOf(type.Digit)),
                    type.slot(numbers.sign, type.Number)); // TODO sign has no class currently
    type.Number.set(id.slots, *mapPtr);

    mapPtr = &slots(type.slot(id.stack, type.ListOf(type.StackFrame)),
                    type.slot(id.method, type.op.Function),
                    type.slot(id.input, type.ListOf(type.op.Var)),
                    type.slot(id.output, type.ListOf(type.op.Var)),
                    type.slot(id.localVars, type.Index));
    type.StackFrame.set(id.slots, *mapPtr);

    type.String.method(id.addSlots, { id.list, list(type.slot(id.value, type.ListOf(type.Char))) });

    type.Color.method(id.addSlots, { id.list, list(
        type.slot(colors.red, type.Number),
        type.slot(colors.green, type.Number),
        type.slot(colors.blue, type.Number)) });

    type.Pixel.method(id.addSlots, { id.list, list(
        type.slot(directions.up, type.Pixel),
        type.slot(directions.down, type.Pixel),
        type.slot(directions.left, type.Pixel),
        type.slot(directions.right, type.Pixel),
        type.slot(coordinates.x, type.Number),
        type.slot(coordinates.y, type.Number)) });

    type.Picture.method(id.addSlots, { id.list, list(
        type.slot(id.width, type.Number),
        type.slot(id.height, type.Number),
        type.slot(id.pixels, type.ListOf(type.Pixel))) });


    Ast::Function& numberFactorial = *new Ast::Function(*this, type.Number, test.factorial, "Number::factorial");
    numberFactorial.addInputs(list(
        ast.slot(id.input, type.Number)));
    numberFactorial.addOutputs(list(
        ast.slot(id.value, type.Number)));
    numberFactorial.addBlock(ast.block(
        ast.if_(ast.greaterThanOrEqual(in_(id.input), _(_1_)),
                ast.return_(ast.multiply(in_(id.input), ast.call(ast.self(), _(test.factorial), ast.slot(_(id.input), ast.subtract(in_(id.input), _(_1_)))))),
                ast.return_(_(_1_)))));

    mapPtr = &map(type.Cell, type.ast.Function,
                  test.factorial, numberFactorial);
    type.Number.set(id.asts, *mapPtr);
    mapPtr = &map(type.Cell, type.op.Function,
                  test.factorial, numberFactorial.compile(type.Number));
    type.Number.set(id.methods, *mapPtr);

    m_initPhase = InitPhase::FullyConstructed;

    mapPtr = &slots(type.slot(id.color, type.Color),
                    type.slot(id.width, type.Number),
                    type.slot(id.height, type.Number),
                    type.slot(id.pixels, type.List));
    type.arc.Shape.set(id.slots, *mapPtr);

    mapPtr = &slots(type.slot(id.width, type.Number),
                    type.slot(id.height, type.Number),
                    type.slot(id.picture, type.Picture),
                    type.slot(id.shapes, type.List),
                    type.slot(id.inputPixels, type.List));
    type.arc.Shaper.set(id.slots, *mapPtr);

    /*
    * class Shape
{
public:
    Shape(int id, input::Color color, int width, int height) :
        m_id(id), m_color(color), m_width(width), m_height(height) { }
    */
    Ast::Function& shapeCtor = *new Ast::Function(*this, type.arc.Shaper, id.constructor, "Shape::Shape");
    shapeCtor.addInputs(list(
        ast.slot(id.id, type.Number),
        ast.slot(id.color, type.Color),
        ast.slot(id.width, type.Number),
        ast.slot(id.height, type.Number)));
    shapeCtor.addBlock(ast.block(
        m_(id.id)  = in_(id.id),
        m_(id.color) = in_(id.color),
        m_(id.width)  = in_(id.width),
        m_(id.height) = in_(id.height)));

    /*
    void Shape::addPixel(cells::hybrid::Pixel& pixel)
    {
        m_pixels.push_back({ pixel.m_x.value(), pixel.m_y.value() });
        m_hybridPixels.insert(&pixel);
    }
    */
    Ast::Function& shapeAddPixel = *new Ast::Function(*this, type.arc.Shaper, id.addPixel, "Shape::addPixel");
    shapeAddPixel.addInputs(list(
        ast.slot(id.pixel, type.Pixel)));
    shapeAddPixel.addBlock(ast.block(
        ast.call(m_(id.pixels), _(id.add), ast.slot(_(id.value), in_(id.pixel)))));

    /*
    bool Shape::hasPixel(cells::hybrid::Pixel& pixel) const
    {
        return m_hybridPixels.find(&pixel) != m_hybridPixels.end();
    }
    */
    Ast::Function& shapeHasPixel = *new Ast::Function(*this, type.arc.Shaper, id.hasPixel, "Shape::hasPixel");
    shapeHasPixel.addInputs(list(
        ast.slot(id.pixel, type.Pixel)));
    shapeHasPixel.addBlock(ast.block(
        m_(id.pixels) / _(id.index) / in_(id.pixel))); // TODO we need a Set class

    registerMethods(type.arc.Shape,
                    id.constructor, shapeCtor,
                    id.addPixel, shapeAddPixel,
                    id.hasPixel, shapeHasPixel);

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
    Ast::Function& shaperCtor = *new Ast::Function(*this, type.arc.Shaper, id.constructor, "Shaper::Shaper");
    shaperCtor.addInputs(list(
        ast.slot(id.width, type.Number),
        ast.slot(id.height, type.Number),
        ast.slot(id.picture, type.Picture)));
    shaperCtor.addBlock(ast.block(
        m_(id.width)  = in_(id.width),
        m_(id.height) = in_(id.height),
        m_(id.picture)        = in_(id.picture),
        ast.call(ast.self(), _(id.processInputPixels))));
    /*
    void Shaper::processInputPixels()
    {
        std::vector<cells::hybrid::Pixel>& pixels = const_cast<cells::hybrid::Picture&>(m_picture).pixels();
        for (cells::hybrid::Pixel& pixel : pixels) {
            m_inputPixels.insert(&pixel);
        }
    }
    */
    Ast::Function& shaperProcessInputPixels = *new Ast::Function(*this, type.arc.Shaper, id.processInputPixels, "Shaper::processInputPixels");
    shaperProcessInputPixels.addBlock(ast.block(
        var_(id.pixels) = m_(id.picture) / _(id.pixels),
        var_(id.pixel) = *var_(id.pixels)/ _(id.first),
        ast.while_(ast.equal(*var_(id.pixel), *var_(id.pixels) / _(id.last)),
                   ast.block(
                          ast.call(m_(id.inputPixels), _(id.add), ast.slot(_(id.value), *var_(id.pixel))),
                          var_(id.pixel) = *var_(id.pixels)/ _(id.first)))));

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
    Ast::Function& shaperProcess = *new Ast::Function(*this, type.arc.Shaper, id.process, "Shaper::process");
    shaperProcess.addBlock(ast.block(
        var_(id.shapeId) = _(_1_),
        ast.while_(ast.not_(ast.call(m_(id.inputPixels), _(id.empty))),
                   ast.block(
                       var_(id.firstPixel) = ast.call(m_(id.inputPixels), _(id.first), ast.slot(_(id.pixel), in_(id.checkPixel))),
                       ast.call(m_(id.shapes), _(id.add), ast.slot(_(id.value), ast.new_(_(type.arc.Shape), _(id.constructor), ast.slot(_(id.id), *var_(id.id)), ast.slot(_(id.color), *var_(id.firstPixel) / _(id.color)), ast.slot(_(id.width), m_(id.width)), ast.slot(_(id.height), m_(id.height))))),
                       var_(id.shape)       = ast.call(m_(id.shapes), _(id.last)),
                       var_(id.checkPixels) = ast.new_(_(type.List)),
                       ast.call(*var_(id.checkPixels), _(id.add), ast.slot(_(id.value), *var_(id.firstPixel))),
                       ast.while_(ast.not_(ast.call(*var_(id.checkPixels), _(id.empty))),
                                  ast.block(
                                      var_(id.checkPixel) = ast.call(*var_(id.checkPixels), _(id.first)),
                                      ast.call(ast.self(), _(id.processPixel), ast.slot(_(id.shape), *var_(id.shape)), ast.slot(_(id.checkPixels), *var_(id.checkPixels)), ast.slot(_(id.checkPixel), *var_(id.checkPixel))),
                                      ast.call(*var_(id.checkPixels), _(id.erase), ast.slot(_(id.value), *var_(id.firstPixel)))
                                  )),
                       ast.call(*var_(id.shape), _(id.sortPixels))
                       ))
    ));

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
    Ast::Function& shaperProcessPixel = *new Ast::Function(*this, type.arc.Shaper, id.processPixel, "Shaper::processPixel");
    shaperProcessPixel.addInputs(list(
        ast.slot(id.shape, type.Shape),
        ast.slot(id.checkPixels, type.ListOf(type.Pixel)),
        ast.slot(id.checkPixel, type.Pixel)));
    shaperProcessPixel.addOutputs(list(
        ast.slot(id.value, type.Number)));
    shaperProcessPixel.addBlock(ast.block(
        ast.call(in_(id.shape), _(id.addPixel), ast.slot(_(id.pixel), in_(id.checkPixel))),
        ast.call(m_(id.inputPixels), _(id.erase), ast.slot(_(id.pixel), in_(id.checkPixel))),
        ast.if_(var_(id.pixel) = ast.call(ast.self(), _(id.processAdjacentPixel), ast.slot(_(id.direction), _(directions.up)), ast.slot(_(id.shape), in_(id.shape)), ast.slot(_(id.checkPixels), in_(id.checkPixels)), ast.slot(_(id.checkPixel), in_(id.checkPixel))),
                ast.block(
                    ast.call(ast.self(), _(id.processAdjacentPixel), ast.slot(_(id.direction), _(directions.left)), ast.slot(_(id.shape), in_(id.shape)), ast.slot(_(id.checkPixels), in_(id.checkPixels)), ast.slot(_(id.checkPixel), *var_(id.pixel))),
                    ast.call(ast.self(), _(id.processAdjacentPixel), ast.slot(_(id.direction), _(directions.right)), ast.slot(_(id.shape), in_(id.shape)), ast.slot(_(id.checkPixels), in_(id.checkPixels)), ast.slot(_(id.checkPixel), *var_(id.pixel))))),
        ast.if_(var_(id.pixel) = ast.call(ast.self(), _(id.processAdjacentPixel), ast.slot(_(id.direction), _(directions.down)), ast.slot(_(id.shape), in_(id.shape)), ast.slot(_(id.checkPixels), in_(id.checkPixels)), ast.slot(_(id.checkPixel), in_(id.checkPixel))),
                ast.block(
                    ast.call(ast.self(), _(id.processAdjacentPixel), ast.slot(_(id.direction), _(directions.left)), ast.slot(_(id.shape), in_(id.shape)), ast.slot(_(id.checkPixels), in_(id.checkPixels)), ast.slot(_(id.checkPixel), *var_(id.pixel))),
                    ast.call(ast.self(), _(id.processAdjacentPixel), ast.slot(_(id.direction), _(directions.right)), ast.slot(_(id.shape), in_(id.shape)), ast.slot(_(id.checkPixels), in_(id.checkPixels)), ast.slot(_(id.checkPixel), *var_(id.pixel))))),
        ast.call(ast.self(), _(id.processAdjacentPixel), ast.slot(_(id.direction), _(directions.left)), ast.slot(_(id.shape), in_(id.shape)), ast.slot(_(id.checkPixels), in_(id.checkPixels)), ast.slot(_(id.checkPixel), in_(id.checkPixel))),
        ast.call(ast.self(), _(id.processAdjacentPixel), ast.slot(_(id.direction), _(directions.right)), ast.slot(_(id.shape), in_(id.shape)), ast.slot(_(id.checkPixels), in_(id.checkPixels)), ast.slot(_(id.checkPixel), in_(id.checkPixel)))));

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
    Ast::Function& shaperProcessAdjacentPixel = *new Ast::Function(*this, type.arc.Shaper, id.processAdjacentPixel, "Shaper::processAdjacentPixel");
    shaperProcessAdjacentPixel.addInputs(list(
        ast.slot(id.direction, type.Directions),
        ast.slot(id.shape, type.Shape),
        ast.slot(id.checkPixels, type.ListOf(type.Pixel)),
        ast.slot(id.checkPixel, type.Pixel)));
    shaperProcessAdjacentPixel.addOutputs(list(
        ast.slot(id.value, type.Number)));
    shaperProcessAdjacentPixel.addBlock(ast.block(
        ast.if_(ast.call(*var_(id.checkPixel), _(id.has), ast.slot(_(id.direction), in_(id.direction))),
                ast.block(
                    var_(id.pixel) = _(id.checkPixel) / _(id.direction),
                    ast.if_(ast.and_(ast.equal(*var_(id.pixel) / _(id.color), *var_(id.shape) / _(id.color)), ast.not_(ast.call(*var_(id.shape), _(id.hasPixel), ast.slot(_(id.pixel), *var_(id.pixel))))),
                        ast.call(*var_(id.checkPixels), _(id.insert), ast.slot(_(id.pixel), *var_(id.pixel)))),
                    ast.return_(_(id.pixel))),
        ast.return_(_(id.emptyObject)))));

    registerMethods(type.arc.Shaper,
                    id.constructor, shaperCtor,
                    id.processInputPixels, shaperProcessInputPixels,
                    id.process, shaperProcess,
                    id.processPixel, shaperProcessPixel,
                    id.processAdjacentPixel, shaperProcessAdjacentPixel);

    m_initPhase = InitPhase::FullyConstructed;
}
Brain::~Brain()
{
    m_initPhase = InitPhase::DestructBegin;
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
