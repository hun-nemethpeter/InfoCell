#include "Brain.h"

#include <format>
#include <sstream>

namespace synth {
namespace cells {
namespace brain {

Sequence::Sequence(brain::Brain& kb) :
    first(kb, kb.type.Cell, "first"),
    last(kb, kb.type.Cell, "last"),
    previous(kb, kb.type.Cell, "previous"),
    next(kb, kb.type.Cell, "next"),
    current(kb, kb.type.Cell, "current"),
    add(kb, kb.type.Cell, "add"),
    empty(kb, kb.type.Cell, "empty")
{
}

Dimensions::Dimensions(brain::Brain& kb) :
    width(kb, kb.type.Cell, "width"),
    height(kb, kb.type.Cell, "height"),
    size(kb, kb.type.Cell, "size")
{
}

Coding::Coding(brain::Brain& kb) :
    kb(kb),
    argument(kb, kb.type.Cell, "argument"),
    ast(kb, kb.type.Cell, "ast"),
    asts(kb, kb.type.Cell, "ast"),
    branch(kb, kb.type.Cell, "branch"),
    cell(kb, kb.type.Cell, "cell"),
    condition(kb, kb.type.Cell, "condition"),
    constructor(kb, kb.type.Cell, "constructor"),
    container(kb, kb.type.Cell, "container"),
    destructor(kb, kb.type.Cell, "destructor"),
    else_(kb, kb.type.Cell, "else_"),
    emptyObject(kb, kb.type.Cell, "emptyObject"),
    eval(kb, kb.type.Cell, "eval"),
    index(kb, kb.type.Cell, "index"),
    indexType(kb, kb.type.Cell, "indexType"),
    input(kb, kb.type.Cell, "input"),
    item(kb, kb.type.Cell, "item"),
    key(kb, kb.type.Cell, "key"),
    keyType(kb, kb.type.Cell, "keyType"),
    label(kb, kb.type.Cell, "label"),
    lhs(kb, kb.type.Cell, "lhs"),
    list(kb, kb.type.Cell, "list"),
    localVars(kb, kb.type.Cell, "localVars"),
    memberOf(kb, kb.type.Cell, "memberOf"),
    method(kb, kb.type.Cell, "method"),
    methods(kb, kb.type.Cell, "methods"),
    objectType(kb, kb.type.Cell, "objectType"),
    op(kb, kb.type.Cell, "op"),
    ops(kb, kb.type.Cell, "ops"),
    output(kb, kb.type.Cell, "output"),
    parameter(kb, kb.type.Cell, "parameter"),
    parameters(kb, kb.type.Cell, "parameters"),
    result(kb, kb.type.Cell, "result"),
    rhs(kb, kb.type.Cell, "rhs"),
    role(kb, kb.type.Cell, "role"),
    self(kb, kb.type.Cell, "self"),
    slotRole(kb, kb.type.Cell, "slotRole"),
    slots(kb, kb.type.Cell, "slots"),
    slotType(kb, kb.type.Cell, "slotType"),
    statement(kb, kb.type.Cell, "statement"),
    subTypes(kb, kb.type.Cell, "subTypes"),
    template_(kb, kb.type.Cell, "template"),
    then(kb, kb.type.Cell, "then"),
    type(kb, kb.type.Cell, "type"),
    value(kb, kb.type.Cell, "value")
{
}

cells::CellI& Coding::slot(cells::CellI& role, cells::CellI& type)
{
    CellI& ret = *new Object(kb, kb.type.Slot);
    ret.set(kb.coding.slotRole, role);
    ret.set(kb.coding.slotType, type);
    return ret;
}

Methods::Methods(brain::Brain& kb) :
    kb(kb),
    addMembership(kb, kb.type.Cell, "addMembership"),
    addMethod(kb, kb.type.Cell, "addMethod"),
    addSlot(kb, kb.type.Cell, "addSlot"),
    addSlots(kb, kb.type.Cell, "addSlots"),
    addSubType(kb, kb.type.Cell, "addSubType")
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
    EvalVar(kb, kb.type.Type_, "op::EvalVar"),
    Function(kb, kb.type.Type_, "op::Function"),
    Get(kb, kb.type.Type_, "op::Get"),
    GreaterThan(kb, kb.type.Type_, "op::GreaterThan"),
    Has(kb, kb.type.Type_, "op::Has"),
    If(kb, kb.type.Type_, "op::If"),
    LessThan(kb, kb.type.Type_, "op::LessThan"),
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
    auto& coding = kb.coding;
    auto& type   = kb.type;
    auto& ast    = kb.type.ast;

    CellI* map = nullptr;

    map = &kb.map(kb.type.Cell, kb.type.Slot,
                  coding.ast, coding.slot(coding.ast, ast.Base),
                  coding.lhs, coding.slot(coding.lhs, Base),
                  coding.rhs, coding.slot(coding.rhs, Base),
                  coding.value, coding.slot(coding.value, type.Number));
    Add.set(coding.slots, *map);

    map = &kb.map(kb.type.Cell, kb.type.Slot,
                  coding.ast, coding.slot(coding.ast, ast.Base),
                  coding.lhs, coding.slot(coding.lhs, Base),
                  coding.rhs, coding.slot(coding.rhs, Base),
                  coding.value, coding.slot(coding.value, type.Boolean));
    And.set(coding.slots, *map);

    map = &kb.map(kb.type.Cell, kb.type.Slot,
                  coding.ast, coding.slot(coding.ast, ast.Base),
                  coding.ops, coding.slot(coding.ops, type.Cell),
                  coding.value, coding.slot(coding.value, type.Cell));
    Block.set(coding.slots, *map);

    map = &kb.map(kb.type.Cell, kb.type.Slot,
                  coding.ast, coding.slot(coding.ast, ast.Base),
                  coding.value, coding.slot(coding.value, type.Cell));
    ConstVar.set(coding.slots, *map);

    map = &kb.map(kb.type.Cell, kb.type.Slot,
                  coding.ast, coding.slot(coding.ast, ast.Base),
                  coding.input, coding.slot(coding.input, Base));
    Delete.set(coding.slots, *map);

    map = &kb.map(kb.type.Cell, kb.type.Slot,
                  coding.ast, coding.slot(coding.ast, ast.Base),
                  coding.lhs, coding.slot(coding.lhs, Base),
                  coding.rhs, coding.slot(coding.rhs, Base),
                  coding.value, coding.slot(coding.value, type.Number));
    Divide.set(coding.slots, *map);

    map = &kb.map(kb.type.Cell, kb.type.Slot,
                  coding.ast, coding.slot(coding.ast, ast.Base),
                  coding.condition, coding.slot(coding.condition, Base),
                  coding.statement, coding.slot(coding.statement, Base));
    Do.set(coding.slots, *map);

    map = &kb.map(kb.type.Cell, kb.type.Slot,
                  coding.ast, coding.slot(coding.ast, ast.Base),
                  coding.lhs, coding.slot(coding.lhs, Base),
                  coding.rhs, coding.slot(coding.rhs, Base),
                  coding.value, coding.slot(coding.value, type.Boolean));
    Equal.set(coding.slots, *map);

    map = &kb.map(kb.type.Cell, kb.type.Slot,
                  coding.ast, coding.slot(coding.ast, ast.Base),
                  coding.value, coding.slot(coding.value, Var));
    EvalVar.set(coding.slots, *map);

    map = &kb.map(kb.type.Cell, kb.type.Slot,
                  coding.ast, coding.slot(coding.ast, ast.Base),
                  coding.localVars, coding.slot(coding.localVars, type.MapCellToOpVar),
                  coding.input, coding.slot(coding.input, type.MapCellToOpBase),
                  coding.op, coding.slot(coding.op, type.ListOf(Base)),
                  coding.output, coding.slot(coding.output, type.MapCellToOpBase));
    Function.set(coding.slots, *map);

    map = &kb.map(kb.type.Cell, kb.type.Slot,
                  coding.ast, coding.slot(coding.ast, ast.Base),
                  coding.cell, coding.slot(coding.cell, Base),
                  coding.role, coding.slot(coding.role, Base),
                  coding.value, coding.slot(coding.value, type.Cell));
    Get.set(coding.slots, *map);

    map = &kb.map(kb.type.Cell, kb.type.Slot,
                  coding.ast, coding.slot(coding.ast, ast.Base),
                  coding.lhs, coding.slot(coding.lhs, Base),
                  coding.rhs, coding.slot(coding.rhs, Base),
                  coding.value, coding.slot(coding.value, type.Boolean));
    GreaterThan.set(coding.slots, *map);

    map = &kb.map(kb.type.Cell, kb.type.Slot,
                  coding.ast, coding.slot(coding.ast, ast.Base),
                  coding.cell, coding.slot(coding.cell, Base),
                  coding.role, coding.slot(coding.role, Base),
                  coding.value, coding.slot(coding.value, type.Boolean));
    Has.set(coding.slots, *map);

    map = &kb.map(kb.type.Cell, kb.type.Slot,
                  coding.ast, coding.slot(coding.ast, ast.Base),
                  coding.condition, coding.slot(coding.condition, Base),
                  coding.then, coding.slot(coding.then, Base),
                  coding.else_, coding.slot(coding.else_, Base));
    If.set(coding.slots, *map);

    map = &kb.map(kb.type.Cell, kb.type.Slot,
                  coding.ast, coding.slot(coding.ast, ast.Base),
                  coding.lhs, coding.slot(coding.lhs, Base),
                  coding.rhs, coding.slot(coding.rhs, Base),
                  coding.value, coding.slot(coding.value, type.Boolean));
    LessThan.set(coding.slots, *map);

    map = &kb.map(kb.type.Cell, kb.type.Slot,
                  coding.ast, coding.slot(coding.ast, ast.Base),
                  coding.cell, coding.slot(coding.cell, Base),
                  coding.role, coding.slot(coding.role, Base),
                  coding.value, coding.slot(coding.value, type.Boolean));
    Missing.set(coding.slots, *map);

    map = &kb.map(kb.type.Cell, kb.type.Slot,
                  coding.ast, coding.slot(coding.ast, ast.Base),
                  coding.lhs, coding.slot(coding.lhs, Base),
                  coding.rhs, coding.slot(coding.rhs, Base),
                  coding.value, coding.slot(coding.value, type.Number));
    Multiply.set(coding.slots, *map);

    map = &kb.map(kb.type.Cell, kb.type.Slot,
                  coding.ast, coding.slot(coding.ast, ast.Base),
                  coding.value, coding.slot(coding.value, type.Cell),
                  coding.objectType, coding.slot(coding.objectType, Base));
    New.set(coding.slots, *map);

    map = &kb.map(kb.type.Cell, kb.type.Slot,
                  coding.ast, coding.slot(coding.ast, ast.Base),
                  coding.input, coding.slot(coding.input, Base),
                  coding.value, coding.slot(coding.value, type.Boolean));
    Not.set(coding.slots, *map);

    map = &kb.map(kb.type.Cell, kb.type.Slot,
                  coding.ast, coding.slot(coding.ast, ast.Base),
                  coding.lhs, coding.slot(coding.lhs, Base),
                  coding.rhs, coding.slot(coding.rhs, Base),
                  coding.value, coding.slot(coding.value, type.Boolean));
    NotEqual.set(coding.slots, *map);

    map = &kb.map(kb.type.Cell, kb.type.Slot,
                  coding.ast, coding.slot(coding.ast, ast.Base),
                  coding.lhs, coding.slot(coding.lhs, Base),
                  coding.rhs, coding.slot(coding.rhs, Base),
                  coding.value, coding.slot(coding.value, type.Boolean));
    NotSame.set(kb.coding.slots, *map);

    map = &kb.map(kb.type.Cell, kb.type.Slot,
                  coding.ast, coding.slot(coding.ast, ast.Base),
                  coding.lhs, coding.slot(coding.lhs, Base),
                  coding.rhs, coding.slot(coding.rhs, Base),
                  coding.value, coding.slot(coding.value, type.Boolean));
    Or.set(coding.slots, *map);

    map = &kb.map(kb.type.Cell, kb.type.Slot,
                  coding.ast, coding.slot(coding.ast, ast.Base));
    Return.set(kb.coding.slots, *map);

    map = &kb.map(kb.type.Cell, kb.type.Slot,
                  coding.ast, coding.slot(coding.ast, ast.Base),
                  coding.lhs, coding.slot(coding.lhs, Base),
                  coding.rhs, coding.slot(coding.rhs, Base),
                  coding.value, coding.slot(coding.value, type.Boolean));
    Same.set(kb.coding.slots, *map);

    map = &kb.map(kb.type.Cell, kb.type.Slot,
                  coding.ast, coding.slot(coding.ast, ast.Base),
                  coding.cell, coding.slot(coding.cell, Base),
                  coding.role, coding.slot(coding.role, Base),
                  coding.value, coding.slot(coding.value, Base));
    Set.set(coding.slots, *map);

    map = &kb.map(kb.type.Cell, kb.type.Slot,
                  coding.ast, coding.slot(coding.ast, ast.Base),
                  coding.lhs, coding.slot(coding.lhs, Base),
                  coding.rhs, coding.slot(coding.rhs, Base),
                  coding.value, coding.slot(coding.value, type.Number));
    Subtract.set(coding.slots, *map);

    map = &kb.map(kb.type.Cell, kb.type.Slot,
                  coding.ast, coding.slot(coding.ast, ast.Base),
                  coding.objectType, coding.slot(coding.objectType, type.Type_),
                  coding.value, coding.slot(coding.value, type.Cell));
    Var.set(coding.slots, *map);

    map = &kb.map(kb.type.Cell, kb.type.Slot,
                  coding.ast, coding.slot(coding.ast, ast.Base),
                  coding.condition, coding.slot(coding.condition, Base),
                  coding.statement, coding.slot(coding.statement, Base));
    While.set(coding.slots, *map);
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
    Function(kb, kb.type.Type_, "ast::Function"),
    Get(kb, kb.type.Type_, "ast::Get"),
    GreaterThan(kb, kb.type.Type_, "ast::GreaterThan"),
    Has(kb, kb.type.Type_, "ast::Has"),
    If(kb, kb.type.Type_, "ast::If"),
    Input(kb, kb.type.Type_, "ast::Input"),
    LessThan(kb, kb.type.Type_, "ast::LessThan"),
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
    auto& coding = kb.coding;
    auto& type   = kb.type;
    CellI* map   = nullptr;

    map = &kb.map(type.Cell, type.Slot,
                  coding.lhs, coding.slot(coding.lhs, Base),
                  coding.rhs, coding.slot(coding.rhs, Base));
    Add.set(coding.slots, *map);

    map = &kb.map(type.Cell, type.Slot,
                  coding.lhs, coding.slot(coding.lhs, Base),
                  coding.rhs, coding.slot(coding.rhs, Base));
    And.set(coding.slots, *map);

    map = &kb.map(type.Cell, type.Slot,
                  coding.asts, coding.slot(coding.asts, type.Cell));
    Block.set(coding.slots, *map);

    map = &kb.map(type.Cell, type.Slot,
                  coding.cell, coding.slot(coding.cell, Base),
                  coding.method, coding.slot(coding.method, Base),
                  coding.parameters, coding.slot(coding.parameters, type.ListOf(Slot)));
    Call.set(coding.slots, *map);

    map = &kb.map(type.Cell, type.Slot,
                  coding.value, coding.slot(coding.value, type.Cell));
    Cell.set(coding.slots, *map);

    map = &kb.map(type.Cell, type.Slot,
                  coding.cell, coding.slot(coding.cell, Base));
    Delete.set(coding.slots, *map);

    map = &kb.map(type.Cell, type.Slot,
                  coding.lhs, coding.slot(coding.lhs, Base),
                  coding.rhs, coding.slot(coding.rhs, Base));
    Divide.set(coding.slots, *map);

    map = &kb.map(type.Cell, type.Slot,
                  coding.condition, coding.slot(coding.condition, Base),
                  coding.statement, coding.slot(coding.statement, Base));
    Do.set(coding.slots, *map);

    map = &kb.map(type.Cell, type.Slot,
                  coding.lhs, coding.slot(coding.lhs, Base),
                  coding.rhs, coding.slot(coding.rhs, Base));
    Equal.set(coding.slots, *map);

    map = &kb.map(type.Cell, type.Slot,
                  coding.input, coding.slot(coding.input, type.ListOf(Slot)),
                  coding.ast, coding.slot(coding.ast, type.ListOf(Base)),
                  coding.output, coding.slot(coding.output, type.ListOf(Slot)));
    Function.set(coding.slots, *map);

    map = &kb.map(type.Cell, type.Slot,
                  coding.cell, coding.slot(coding.cell, Base),
                  coding.role, coding.slot(coding.role, Base));
    Get.set(coding.slots, *map);

    map = &kb.map(type.Cell, type.Slot,
                  coding.lhs, coding.slot(coding.lhs, Base),
                  coding.rhs, coding.slot(coding.rhs, Base));
    GreaterThan.set(coding.slots, *map);

    map = &kb.map(type.Cell, type.Slot,
                  coding.cell, coding.slot(coding.cell, Base),
                  coding.role, coding.slot(coding.role, Base));
    Has.set(coding.slots, *map);

    map = &kb.map(type.Cell, type.Slot,
                  coding.condition, coding.slot(coding.condition, Base),
                  coding.then, coding.slot(coding.then, Base),
                  coding.else_, coding.slot(coding.else_, Base));
    If.set(coding.slots, *map);

    map = &kb.map(type.Cell, type.Slot,
                  coding.role, coding.slot(coding.role, type.Cell));
    Input.set(coding.slots, *map);

    LessThan.set(coding.slots, *map);

    map = &kb.map(type.Cell, type.Slot,
                  coding.lhs, coding.slot(coding.lhs, Base),
                  coding.rhs, coding.slot(coding.rhs, Base));

    map = &kb.map(type.Cell, type.Slot,
                  coding.role, coding.slot(coding.role, Base));
    Member.set(coding.slots, *map);

    map = &kb.map(type.Cell, type.Slot,
                  coding.cell, coding.slot(coding.cell, Base),
                  coding.role, coding.slot(coding.role, Base));
    Missing.set(coding.slots, *map);

    map = &kb.map(type.Cell, type.Slot,
                  coding.lhs, coding.slot(coding.lhs, Base),
                  coding.rhs, coding.slot(coding.rhs, Base));
    Multiply.set(coding.slots, *map);

    map = &kb.map(type.Cell, type.Slot,
                  coding.objectType, coding.slot(coding.objectType, Base),
                  coding.constructor, coding.slot(coding.constructor, Base),
                  coding.parameters, coding.slot(coding.parameters, type.ListOf(type.ast.Slot)));
    New.set(coding.slots, *map);

    map = &kb.map(type.Cell, type.Slot,
                  coding.input, coding.slot(coding.input, Base));
    Not.set(coding.slots, *map);

    map = &kb.map(type.Cell, type.Slot,
                  coding.lhs, coding.slot(coding.lhs, Base),
                  coding.rhs, coding.slot(coding.rhs, Base));
    NotEqual.set(coding.slots, *map);

    map = &kb.map(type.Cell, type.Slot,
                  coding.lhs, coding.slot(coding.lhs, Base),
                  coding.rhs, coding.slot(coding.rhs, Base));
    NotSame.set(coding.slots, *map);

    map = &kb.map(type.Cell, type.Slot,
                  coding.lhs, coding.slot(coding.lhs, Base),
                  coding.rhs, coding.slot(coding.rhs, Base));
    Or.set(coding.slots, *map);

    map = &kb.map(type.Cell, type.Slot,
                  coding.role, coding.slot(coding.role, type.Cell));
    Output.set(coding.slots, *map);

    map = &kb.map(type.Cell, type.Slot,
                  coding.lhs, coding.slot(coding.lhs, Base),
                  coding.rhs, coding.slot(coding.rhs, Base));
    Same.set(coding.slots, *map);

    map = &kb.map(type.Cell, type.Slot,
                  coding.cell, coding.slot(coding.cell, Base),
                  coding.role, coding.slot(coding.role, Base),
                  coding.value, coding.slot(coding.value, Base));
    Set.set(coding.slots, *map);

    map = &kb.map(type.Cell, type.Slot,
                  coding.slotRole, coding.slot(coding.slotRole, Base),
                  coding.slotType, coding.slot(coding.slotType, Base));
    Slot.set(coding.slots, *map);

    map = &kb.map(type.Cell, type.Slot,
                  coding.cell, coding.slot(coding.cell, Base),
                  coding.method, coding.slot(coding.method, Base),
                  coding.parameters, coding.slot(coding.parameters, type.ListOf(Slot)));
    StaticCall.set(coding.slots, *map);

    map = &kb.map(type.Cell, type.Slot,
                  coding.lhs, coding.slot(coding.lhs, Base),
                  coding.rhs, coding.slot(coding.rhs, Base));
    Subtract.set(coding.slots, *map);

    map = &kb.map(type.Cell, type.Slot,
                  coding.role, coding.slot(coding.role, Base));
    Var.set(coding.slots, *map);

    map = &kb.map(type.Cell, type.Slot,
                  coding.condition, coding.slot(coding.condition, Base),
                  coding.statement, coding.slot(coding.statement, Base));
    While.set(coding.slots, *map);
}

} // namespace type

Types::Types(brain::Brain& kb) :
    kb(kb),
    Type_(kb, kb.type.Type_, "Type"),
    Cell(kb, kb.type.Type_, "Cell"),
    Slot(kb, kb.type.Type_, "Slot"),
    Container(kb, kb.type.Type_, "Conatainer"),
    Iterator(kb, kb.type.Type_, "Iterator"),
    List(kb, kb.type.Type_, "List"),
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
    Boolean(kb, kb.type.Type_, "Boolean"),
    Char(kb, kb.type.Type_, "Char"),
    Digit(kb, kb.type.Type_, "Digit"),
    Number(kb, kb.type.Type_, "Number"),
    String(kb, kb.type.Type_, "String"),
    Color(kb, kb.type.Type_, "Color"),
    Pixel(kb, kb.type.Type_, "Pixel"),
    Picture(kb, kb.type.Type_, "Picture"),
    op(kb),
    ast(kb)
{
    CellI* map   = nullptr;
    auto& coding = kb.coding;

    map = &kb.map(Cell, Slot,
                  kb.coding.slotType, kb.coding.slot(kb.coding.slotType, kb.type.Type_),
                  kb.coding.slotRole, kb.coding.slot(kb.coding.slotRole, kb.type.Cell));
    Slot.set(kb.coding.slots, *map);

    map = &kb.map(Cell, Slot,
                  coding.slots, coding.slot(coding.slots, MapCellToSlot),
                  coding.subTypes, coding.slot(coding.subTypes, MapCellToType),
                  coding.memberOf, coding.slot(coding.memberOf, MapTypeToType),
                  coding.asts, coding.slot(coding.asts, MapCellToAstFunction),
                  coding.methods, coding.slot(coding.methods, MapCellToOpFunction));
    Type_.set(coding.slots, *map);

    kb.m_initPhase = Brain::InitPhase::SlotTypeInitialzed;
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
        listType.set(kb.coding.subTypes, kb.map(kb.type.Cell, kb.type.Type_, kb.coding.objectType, itemType));
        listType.set(kb.coding.memberOf, kb.map(kb.type.Type_, kb.type.Type_, kb.type.List, kb.type.List));
        itemType.set(kb.coding.memberOf, kb.map(kb.type.Type_, kb.type.Type_, kb.type.ListItem, kb.type.ListItem));
        auto& coding = kb.coding;

        CellI* map = nullptr;

        map = &kb.map(kb.type.Cell, kb.type.Slot,
                      kb.sequence.first, coding.slot(kb.sequence.first, itemType),
                      kb.sequence.last, coding.slot(kb.sequence.last, itemType),
                      kb.dimensions.size, coding.slot(kb.dimensions.size, kb.type.Number),
                      kb.coding.objectType, coding.slot(kb.coding.objectType, type));
        listType.set(kb.coding.slots, *map);

        map = &kb.map(kb.type.Cell, kb.type.Slot,
                      kb.sequence.previous, coding.slot(kb.sequence.previous, itemType),
                      kb.sequence.next, coding.slot(kb.sequence.next, itemType),
                      kb.coding.value, coding.slot(kb.coding.value, type));
        itemType.set(kb.coding.slots, *map);

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
    auto& coding    = kb.coding;
    CellI* map      = nullptr;

    mapType.set(kb.coding.methods, kb.type.Map[kb.coding.methods]);
    mapType.set(kb.coding.memberOf, kb.map(kb.type.Type_, kb.type.Type_, kb.type.Map, kb.type.Map));
    mapType.set(kb.coding.subTypes, kb.map(kb.type.Cell, kb.type.Type_, kb.coding.keyType, keyType, kb.coding.objectType, valueType));

    map = &kb.map(kb.type.Cell, kb.type.Slot,
                  kb.coding.list, coding.slot(kb.coding.list, ListOf(valueType)),
                  kb.coding.index, coding.slot(kb.coding.index, kb.type.Index),
                  kb.coding.indexType, coding.slot(kb.coding.indexType, kb.type.Type_),
                  kb.coding.keyType, coding.slot(kb.coding.keyType, keyType),
                  kb.coding.objectType, coding.slot(kb.coding.objectType, valueType),
                  kb.dimensions.size, coding.slot(kb.dimensions.size, kb.type.Number));
    mapType.set(kb.coding.slots, *map);

    return it.first->second;
}

// ============================================================================
Ast::Base::Base(brain::Brain& kb, CellI& classCell, const std::string& label) :
    Object(kb, classCell, label)
{
}

Ast::Input::Input(brain::Brain& kb, CellI& role) :
    BaseT<Input>(kb, kb.type.ast.Input)
{
    set(kb.coding.role, role);
}

Ast::Get& Ast::Input::operator/(Base& role)
{
    return Get::New(kb, *this, role);
}

Ast::Output::Output(brain::Brain& kb, CellI& role) :
    BaseT<Output>(kb, kb.type.ast.Output)
{
    set(kb.coding.role, role);
}

Ast::Slot::Slot(brain::Brain& kb, CellI& role, CellI& type) :
    BaseT<Slot>(kb, kb.type.ast.Slot)
{
    set(kb.coding.slotRole, role);
    set(kb.coding.slotType, type);
}

Ast::Call::Call(brain::Brain& kb, CellI& cell, CellI& method) :
    BaseT<Call>(kb, kb.type.ast.Call)
{
    set(kb.coding.cell, cell);
    set(kb.coding.method, method);
}

Ast::Call::Call(brain::Brain& kb, CellI& cell, CellI& method, Slot& slot1) :
    BaseT<Call>(kb, kb.type.ast.Call)
{
    set(kb.coding.cell, cell);
    set(kb.coding.method, method);
    set(kb.coding.parameters, kb.list(slot1));
}

Ast::Call::Call(brain::Brain& kb, CellI& cell, CellI& method, Slot& slot1, Slot& slot2) :
    BaseT<Call>(kb, kb.type.ast.Call)
{
    set(kb.coding.cell, cell);
    set(kb.coding.method, method);
    set(kb.coding.parameters, kb.list(slot1, slot2));
}

Ast::Call::Call(brain::Brain& kb, CellI& cell, CellI& method, Slot& slot1, Slot& slot2, Slot& slot3) :
    BaseT<Call>(kb, kb.type.ast.Call)
{
    set(kb.coding.cell, cell);
    set(kb.coding.method, method);
    set(kb.coding.parameters, kb.list(slot1, slot2, slot3));
}

Ast::Call::Call(brain::Brain& kb, CellI& cell, CellI& method, Slot& slot1, Slot& slot2, Slot& slot3, Slot& slot4) :
    BaseT<Call>(kb, kb.type.ast.Call)
{
    set(kb.coding.cell, cell);
    set(kb.coding.method, method);
    set(kb.coding.parameters, kb.list(slot1, slot2, slot3, slot4));
}

Ast::StaticCall::StaticCall(brain::Brain& kb, CellI& cell, CellI& method) :
    BaseT<StaticCall>(kb, kb.type.ast.StaticCall)
{
    set(kb.coding.cell, cell);
    set(kb.coding.method, method);
}

Ast::StaticCall::StaticCall(brain::Brain& kb, CellI& cell, CellI& method, Slot& slot1) :
    BaseT<StaticCall>(kb, kb.type.ast.StaticCall)
{
    set(kb.coding.cell, cell);
    set(kb.coding.method, method);
    set(kb.coding.parameters, kb.list(slot1));
}

Ast::StaticCall::StaticCall(brain::Brain& kb, CellI& cell, CellI& method, Slot& slot1, Slot& slot2) :
    BaseT<StaticCall>(kb, kb.type.ast.StaticCall)
{
    set(kb.coding.cell, cell);
    set(kb.coding.method, method);
    set(kb.coding.parameters, kb.list(slot1, slot2));
}

Ast::StaticCall::StaticCall(brain::Brain& kb, CellI& cell, CellI& method, Slot& slot1, Slot& slot2, Slot& slot3) :
    BaseT<StaticCall>(kb, kb.type.ast.StaticCall)
{
    set(kb.coding.cell, cell);
    set(kb.coding.method, method);
    set(kb.coding.parameters, kb.list(slot1, slot2, slot3));
}

Ast::StaticCall::StaticCall(brain::Brain& kb, CellI& cell, CellI& method, Slot& slot1, Slot& slot2, Slot& slot3, Slot& slot4) :
    BaseT<StaticCall>(kb, kb.type.ast.StaticCall)
{
    set(kb.coding.cell, cell);
    set(kb.coding.method, method);
    set(kb.coding.parameters, kb.list(slot1, slot2, slot3, slot4));
}

Ast::Cell::Cell(brain::Brain& kb, CellI& value) :
    BaseT<Cell>(kb, kb.type.ast.Cell)
{
    set(kb.coding.value, value);
}

Ast::Self::Self(brain::Brain& kb) :
    BaseT<Self>(kb, kb.type.ast.Self)
{
}

Ast::SelfFn::SelfFn(brain::Brain& kb) :
    BaseT<SelfFn>(kb, kb.type.ast.SelfFn)
{
}

Ast::Return::Return(brain::Brain& kb) :
    BaseT<Return>(kb, kb.type.ast.Return)
{
}

Ast::Block::Block(brain::Brain& kb, List& list) :
    BaseT<Block>(kb, kb.type.ast.Block)
{
    set(kb.coding.asts, list);
}

Ast::Function::Function(brain::Brain& kb, const std::string& label) :
    BaseT<Function>(kb, kb.type.ast.Function)
{
    this->label(label);
}

void Ast::Function::addInputs(List& input)
{
    set(kb.coding.input, input);
    m_inputs = &input;
}

void Ast::Function::addOutputs(List& output)
{
    set(kb.coding.output, output);
    m_outputs = &output;
}

void Ast::Function::addAsts(Block& ast)
{
    set(kb.coding.ast, ast);
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
    cells::Object& function = *new cells::Object(kb, kb.type.op.Function);
    function.set(kb.coding.localVars, *new Map(kb, kb.type.Cell, kb.type.op.Var));
    compileParams(function, type);
    function.set(kb.coding.ast, asts());
    function.set(kb.coding.op, compileAst(asts(), function, type));

    return function;
}

void Ast::Function::compileParams(cells::Object& function, CellI* type)
{
    std::stringstream iss;
    std::stringstream oss;
    if (m_inputs || type) {
        Map& params = *new Map(kb, kb.type.Cell, kb.type.op.Var);
        if (type) {
            Object& var = *new Object(kb, kb.type.op.Var, "self");
            var.set(kb.coding.objectType, *type);
            params.add(kb.coding.self, var);
            iss << kb.coding.self.label() << ": " << (*type).label();
        }
        if (m_inputs) {
            Visitor::visitList(inputs(), [this, &params, &iss](CellI& slot, int i) {
                if (!params.empty()) {
                    iss << ", ";
                }
                iss << "in_" << slot[kb.coding.slotRole].label() << ": " << slot[kb.coding.slotType].label();
                Object& var = *new Object(kb, kb.type.op.Var, std::format("in_{}", slot[kb.coding.slotRole].label()));
                var.set(kb.coding.objectType, slot[kb.coding.slotType]);
                params.add(slot[kb.coding.slotRole], var);
            });
        }
        function.set(kb.coding.input, params);

    }
    if (m_outputs) {
        Map& params = *new Map(kb, kb.type.Cell, kb.type.op.Var);
        Visitor::visitList(outputs(), [this, &params, &oss](CellI& slot, int i) {
            if (!params.empty()) {
                oss << ", ";
            }
            oss << "out_" << slot[kb.coding.slotRole].label() << ": " << slot[kb.coding.slotType].label();
            Object& var = *new Object(kb, kb.type.op.Var, std::format("out_{}", slot[kb.coding.slotRole].label()));
            var.set(kb.coding.objectType, slot[kb.coding.slotType]);
            params.add(slot[kb.coding.slotRole], var);
        });
        function.set(kb.coding.output, params);
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

    if (&ast.type() == &kb.type.ast.Block) {
        CellI& list        = ast[kb.coding.asts];
        auto& compiledAsts = *new cells::List(kb, kb.type.op.Base);
        Visitor::visitList(list, [this, &compiledAsts, &ast, &function, type](CellI& ast, int) {
            compiledAsts.add(compileAst(ast, function, type));
        });
        Object& opBlock = *new Object(kb, kb.type.op.Block);
        opBlock.set(kb.coding.ast, ast);
        opBlock.set(kb.coding.ops, compiledAsts);
        return opBlock;
    } else if (&ast.type() == &kb.type.ast.Cell) {
        Object& constVar = *new Object(kb, kb.type.op.ConstVar);
        constVar.set(kb.coding.ast, ast);
        constVar.set(kb.coding.value, ast[kb.coding.value]);
        return constVar;
    } else if (&ast.type() == &kb.type.ast.SelfFn) {
        Object& constVar = *new Object(kb, kb.type.op.ConstVar);
        constVar.set(kb.coding.ast, ast);
        constVar.set(kb.coding.value, function);
        return constVar;
    } else if (&ast.type() == &kb.type.ast.Self) {
        return function[kb.coding.input][kb.coding.index][kb.coding.self];
    } else if (&ast.type() == &kb.type.ast.Input) {
        return function[kb.coding.input][kb.coding.index][ast[kb.coding.role]];
    } else if (&ast.type() == &kb.type.ast.Output) {
        Object& constVar = *new Object(kb, kb.type.op.ConstVar);
        constVar.set(kb.coding.ast, ast);
        constVar.set(kb.coding.value, function[kb.coding.output][kb.coding.index][ast[kb.coding.role]]);
        return constVar;
    } else if (&ast.type() == &kb.type.ast.Delete) {
        Object& opDelete = *new Object(kb, kb.type.op.Delete);
        opDelete.set(kb.coding.ast, ast);
        opDelete.set(kb.coding.input, compile(ast[kb.coding.cell]));
        return opDelete;
    } else if (&ast.type() == &kb.type.ast.Set) {
        Object& opSet = *new Object(kb, kb.type.op.Set);
        opSet.set(kb.coding.ast, ast);
        opSet.set(kb.coding.cell, compile(ast[kb.coding.cell]));
        opSet.set(kb.coding.role, compile(ast[kb.coding.role]));
        opSet.set(kb.coding.value, compile(ast[kb.coding.value]));
        return opSet;
    } else if (&ast.type() == &kb.type.ast.If) {
        Object& opIf = *new Object(kb, kb.type.op.If);
        opIf.set(kb.coding.ast, ast);
        opIf.set(kb.coding.condition, compile(ast[kb.coding.condition]));
        opIf.set(kb.coding.then, compile(ast[kb.coding.then]));
        if (ast.has(kb.coding.else_)) {
            opIf.set(kb.coding.else_, compile(ast[kb.coding.else_]));
            return opIf;
        } else {
            return opIf;
        }
    } else if (&ast.type() == &kb.type.ast.Do) {
        Object& opDo = *new Object(kb, kb.type.op.Do);
        opDo.set(kb.coding.ast, ast);
        opDo.set(kb.coding.condition, compile(ast[kb.coding.condition]));
        opDo.set(kb.coding.statement, compile(ast[kb.coding.statement]));
        return opDo;
    } else if (&ast.type() == &kb.type.ast.While) {
        Object& opWhile = *new Object(kb, kb.type.op.While);
        opWhile.set(kb.coding.ast, ast);
        opWhile.set(kb.coding.condition, compile(ast[kb.coding.condition]));
        opWhile.set(kb.coding.statement, compile(ast[kb.coding.statement]));
        return opWhile;
    } else if (&ast.type() == &kb.type.ast.Var) {
        Object& constVar = *new Object(kb, kb.type.op.ConstVar);
        constVar.set(kb.coding.ast, ast);
        if (!function[kb.coding.localVars][kb.coding.index].has(ast[kb.coding.role])) {
            Object& var = *new Object(kb, kb.type.op.Var);
            var.set(kb.coding.objectType, kb.type.Cell);
            static_cast<Map&>(function[kb.coding.localVars]).add(ast[kb.coding.role], var);
        }
        constVar.set(kb.coding.value, function[kb.coding.localVars][kb.coding.index][ast[kb.coding.role]]);
        return constVar;
    } else if (&ast.type() == &kb.type.ast.New) {
        auto& compiledAsts = *new cells::List(kb, kb.type.op.Base);

        Object& block = *new Object(kb, kb.type.op.Block);
        block.set(kb.coding.ast, ast);
        block.set(kb.coding.ops, compiledAsts);
        Object& opSet = *new Object(kb, kb.type.op.Set, "block.value = new objectType()");
        opSet.set(kb.coding.ast, ast);
        opSet.set(kb.coding.cell, compile(kb.ast.cell(block)));
        opSet.set(kb.coding.role, compile(kb.ast.cell(kb.coding.value)));
        Object& opNew = *new Object(kb, kb.type.op.New);
        opNew.set(kb.coding.ast, ast);
        opNew.set(kb.coding.objectType, compile(ast[kb.coding.objectType]));
        opSet.set(kb.coding.value, opNew);

        compiledAsts.add(opSet);
        if (ast.has(kb.coding.constructor)) {
            Object& callAst = *new Object(kb, kb.type.ast.Call);
            callAst.set(kb.coding.cell, kb.ast.get(kb.ast.cell(block), kb.ast.cell(kb.coding.value)));
            callAst.set(kb.coding.method, ast[kb.coding.constructor]);
            if (ast.has(kb.coding.parameters)) {
                callAst.set(kb.coding.parameters, ast[kb.coding.parameters]);
            }
            compiledAsts.add(compile(callAst));
        }
        return block;
    } else if (&ast.type() == &kb.type.ast.Call) {
        auto& compiledAsts  = *new cells::List(kb, kb.type.op.Base);
        Object& block      = *new Object(kb, kb.type.op.Block, "Call { ... }");
        block.set(kb.coding.ast, ast);
        block.set(kb.coding.ops, compiledAsts);
        Ast::Get& getMethod = kb.ast.get(kb.ast.get(kb.ast.get(kb.ast.get(static_cast<Ast::Base&>(ast[kb.coding.cell]), kb.ast.cell(kb.coding.type)), kb.ast.cell(kb.coding.methods)), kb.ast.cell(kb.coding.index)), static_cast<Ast::Base&>(ast[kb.coding.method]));
        Object& varMethod   = *new Object(kb, kb.type.op.Var, "Call { var method; }");
        varMethod.set(kb.coding.objectType, kb.type.op.Function);
        CellI& storeMethod  = compile(kb.ast.set(kb.ast.cell(varMethod), kb.ast.cell(kb.coding.value), getMethod));
        CellI& setSelf      = compile(kb.ast.set(kb.ast.get(kb.ast.get(kb.ast.get(kb.ast.get(kb.ast.cell(varMethod), kb.ast.cell(kb.coding.value)), kb.ast.cell(kb.coding.input)), kb.ast.cell(kb.coding.index)), kb.ast.cell(kb.coding.self)), kb.ast.cell(kb.coding.value), static_cast<Ast::Base&>(ast[kb.coding.cell])));
        compiledAsts.add(storeMethod);
        compiledAsts.add(setSelf);
        getMethod.label("Call { getMethod; }");
        storeMethod.label("Call { storeMethod; }");
        setSelf.label("Call { setSelf; }");
        if (ast.has(kb.coding.parameters)) {
            Visitor::visitList(ast[kb.coding.parameters], [this, &ast, &function, type, &compiledAsts, &varMethod](CellI& ast, int) {
                CellI& setParam = compileAst(kb.ast.set(kb.ast.get(kb.ast.get(kb.ast.get(kb.ast.get(kb.ast.cell(varMethod), kb.ast.cell(kb.coding.value)), kb.ast.cell(kb.coding.input)), kb.ast.cell(kb.coding.index)), static_cast<Ast::Base&>(ast[kb.coding.slotRole])), kb.ast.cell(kb.coding.value), static_cast<Ast::Base&>(ast[kb.coding.slotType])), function, type);
                setParam.label("Call { setParam; }");
                compiledAsts.add(setParam);
            });
        }
        CellI& evalMethod = *new Object(kb, kb.type.op.EvalVar, std::format("{}::Call {{ evalVar; }}", function.label()));
        evalMethod.set(kb.coding.value, varMethod);
        compiledAsts.add(evalMethod);
        CellI& getResult = compile(kb.ast.if_(kb.ast.has(kb.ast.get(kb.ast.cell(varMethod), kb.ast.cell(kb.coding.value)), kb.ast.cell(kb.coding.output)),
                                              kb.ast.if_(kb.ast.has(kb.ast.get(kb.ast.get(kb.ast.cell(varMethod), kb.ast.cell(kb.coding.value)), kb.ast.cell(kb.coding.output)) / kb.ast.cell(kb.coding.index), kb.ast.cell(kb.coding.value)),
                                                         kb.ast.set(kb.ast.cell(block), kb.ast.cell(kb.coding.value), kb.ast.get(kb.ast.get(kb.ast.cell(varMethod), kb.ast.cell(kb.coding.value)), kb.ast.cell(kb.coding.output)) / kb.ast.cell(kb.coding.index) / kb.ast.cell(kb.coding.value) / kb.ast.cell(kb.coding.value)))));
        getResult.label("Call { getResult; }");
        compiledAsts.add(getResult);

        return block;
    } else if (&ast.type() == &kb.type.ast.StaticCall) {
        auto& compiledAsts  = *new cells::List(kb, kb.type.op.Base);
        Object& block      = *new Object(kb, kb.type.op.Block, "SCall { ... }");
        block.set(kb.coding.ast, ast);
        block.set(kb.coding.ops, compiledAsts);
        Ast::Get& getMethod = kb.ast.get(kb.ast.get(kb.ast.get(static_cast<Ast::Base&>(ast[kb.coding.cell]), kb.ast.cell(kb.coding.methods)), kb.ast.cell(kb.coding.index)), static_cast<Ast::Base&>(ast[kb.coding.method]));
        Object& varMethod   = *new Object(kb, kb.type.op.Var, "Call { var method; }");
        varMethod.set(kb.coding.objectType, kb.type.op.Function);
        CellI& storeMethod = compile(kb.ast.set(kb.ast.cell(varMethod), kb.ast.cell(kb.coding.value), getMethod));
        CellI& setSelf      = compile(kb.ast.set(kb.ast.get(kb.ast.get(kb.ast.get(kb.ast.get(kb.ast.cell(varMethod), kb.ast.cell(kb.coding.value)), kb.ast.cell(kb.coding.input)), kb.ast.cell(kb.coding.index)), kb.ast.cell(kb.coding.self)), kb.ast.cell(kb.coding.value), static_cast<Ast::Base&>(ast[kb.coding.cell])));
        compiledAsts.add(storeMethod);
        compiledAsts.add(setSelf);
        storeMethod.label("Call { storeMethod; }");
        setSelf.label("Call { setSelf; }");
        if (ast.has(kb.coding.parameters)) {
            Visitor::visitList(ast[kb.coding.parameters], [this, &ast, &function, type, &compiledAsts, &varMethod](CellI& ast, int) {
                CellI& setParam = compileAst(kb.ast.set(kb.ast.get(kb.ast.get(kb.ast.get(kb.ast.get(kb.ast.cell(varMethod), kb.ast.cell(kb.coding.value)), kb.ast.cell(kb.coding.input)), kb.ast.cell(kb.coding.index)), static_cast<Ast::Base&>(ast[kb.coding.slotRole])), kb.ast.cell(kb.coding.value), static_cast<Ast::Base&>(ast[kb.coding.slotType])), function, type);
                setParam.label("Call { setParam; }");
                compiledAsts.add(setParam);
            });
        }
        CellI& evalMethod = *new Object(kb, kb.type.op.EvalVar, std::format("{}::Call {{ evalVar; }}", function.label()));
        evalMethod.set(kb.coding.value, varMethod);
        compiledAsts.add(evalMethod);
        CellI& getResult = compile(kb.ast.if_(kb.ast.has(kb.ast.get(kb.ast.cell(varMethod), kb.ast.cell(kb.coding.value)), kb.ast.cell(kb.coding.output)),
                                              kb.ast.if_(kb.ast.has(kb.ast.get(kb.ast.get(kb.ast.cell(varMethod), kb.ast.cell(kb.coding.value)), kb.ast.cell(kb.coding.output)) / kb.ast.cell(kb.coding.index), kb.ast.cell(kb.coding.value)),
                                                         kb.ast.set(kb.ast.cell(block), kb.ast.cell(kb.coding.value), kb.ast.get(kb.ast.get(kb.ast.cell(varMethod), kb.ast.cell(kb.coding.value)), kb.ast.cell(kb.coding.output)) / kb.ast.cell(kb.coding.index) / kb.ast.cell(kb.coding.value) / kb.ast.cell(kb.coding.value)))));
        getResult.label("Call { getResult; }");
        compiledAsts.add(getResult);

        return block;
    } else if (&ast.type() == &kb.type.ast.And) {
        Object& opAnd = *new Object(kb, kb.type.op.And);
        opAnd.set(kb.coding.ast, ast);
        opAnd.set(kb.coding.lhs, compile(ast[kb.coding.lhs]));
        opAnd.set(kb.coding.rhs, compile(ast[kb.coding.rhs]));
        return opAnd;
    } else if (&ast.type() == &kb.type.ast.Or) {
        Object& opOr = *new Object(kb, kb.type.op.Or);
        opOr.set(kb.coding.ast, ast);
        opOr.set(kb.coding.lhs, compile(ast[kb.coding.lhs]));
        opOr.set(kb.coding.rhs, compile(ast[kb.coding.rhs]));
        return opOr;
    } else if (&ast.type() == &kb.type.ast.Not) {
        Object& opNot = *new Object(kb, kb.type.op.Not);
        opNot.set(kb.coding.ast, ast);
        opNot.set(kb.coding.input, compile(ast[kb.coding.input]));
        return opNot;
    } else if (&ast.type() == &kb.type.ast.Add) {
        Object& opAdd = *new Object(kb, kb.type.op.Add);
        opAdd.set(kb.coding.ast, ast);
        opAdd.set(kb.coding.lhs, compile(ast[kb.coding.lhs]));
        opAdd.set(kb.coding.rhs, compile(ast[kb.coding.rhs]));
        return opAdd;
    } else if (&ast.type() == &kb.type.ast.Subtract) {
        Object& opSubtract = *new Object(kb, kb.type.op.Subtract);
        opSubtract.set(kb.coding.ast, ast);
        opSubtract.set(kb.coding.lhs, compile(ast[kb.coding.lhs]));
        opSubtract.set(kb.coding.rhs, compile(ast[kb.coding.rhs]));
        return opSubtract;
    } else if (&ast.type() == &kb.type.ast.Multiply) {
        Object& opMultiply = *new Object(kb, kb.type.op.Multiply);
        opMultiply.set(kb.coding.ast, ast);
        opMultiply.set(kb.coding.lhs, compile(ast[kb.coding.lhs]));
        opMultiply.set(kb.coding.rhs, compile(ast[kb.coding.rhs]));
        return opMultiply;
    } else if (&ast.type() == &kb.type.ast.Divide) {
        Object& opDivide = *new Object(kb, kb.type.op.Divide);
        opDivide.set(kb.coding.ast, ast);
        opDivide.set(kb.coding.lhs, compile(ast[kb.coding.lhs]));
        opDivide.set(kb.coding.rhs, compile(ast[kb.coding.rhs]));
        return opDivide;
    } else if (&ast.type() == &kb.type.ast.LessThan) {
        Object& opLessThan = *new Object(kb, kb.type.op.LessThan);
        opLessThan.set(kb.coding.ast, ast);
        opLessThan.set(kb.coding.lhs, compile(ast[kb.coding.lhs]));
        opLessThan.set(kb.coding.rhs, compile(ast[kb.coding.rhs]));
        return opLessThan;
    } else if (&ast.type() == &kb.type.ast.GreaterThan) {
        Object& opGreaterThan = *new Object(kb, kb.type.op.GreaterThan);
        opGreaterThan.set(kb.coding.ast, ast);
        opGreaterThan.set(kb.coding.lhs, compile(ast[kb.coding.lhs]));
        opGreaterThan.set(kb.coding.rhs, compile(ast[kb.coding.rhs]));
        return opGreaterThan;
    } else if (&ast.type() == &kb.type.ast.Same) {
        Object& opSame = *new Object(kb, kb.type.op.Same);
        opSame.set(kb.coding.ast, ast);
        opSame.set(kb.coding.lhs, compile(ast[kb.coding.lhs]));
        opSame.set(kb.coding.rhs, compile(ast[kb.coding.rhs]));
        return opSame;
    } else if (&ast.type() == &kb.type.ast.NotSame) {
        Object& opNotSame = *new Object(kb, kb.type.op.NotSame);
        opNotSame.set(kb.coding.ast, ast);
        opNotSame.set(kb.coding.lhs, compile(ast[kb.coding.lhs]));
        opNotSame.set(kb.coding.rhs, compile(ast[kb.coding.rhs]));
        return opNotSame;
    } else if (&ast.type() == &kb.type.ast.Equal) {
        Object& opEqual = *new Object(kb, kb.type.op.Equal);
        opEqual.set(kb.coding.ast, ast);
        opEqual.set(kb.coding.lhs, compile(ast[kb.coding.lhs]));
        opEqual.set(kb.coding.rhs, compile(ast[kb.coding.rhs]));
        return opEqual;
    } else if (&ast.type() == &kb.type.ast.NotEqual) {
        Object& opNotEqual = *new Object(kb, kb.type.op.NotEqual);
        opNotEqual.set(kb.coding.ast, ast);
        opNotEqual.set(kb.coding.lhs, compile(ast[kb.coding.lhs]));
        opNotEqual.set(kb.coding.rhs, compile(ast[kb.coding.rhs]));
        return opNotEqual;
    } else if (&ast.type() == &kb.type.ast.Has) {
        Object& opHas = *new Object(kb, kb.type.op.Has);
        opHas.set(kb.coding.ast, ast);
        opHas.set(kb.coding.cell, compile(ast[kb.coding.cell]));
        opHas.set(kb.coding.role, compile(ast[kb.coding.role]));
        return opHas;
    } else if (&ast.type() == &kb.type.ast.Missing) {
        Object& opMissing = *new Object(kb, kb.type.op.Missing);
        opMissing.set(kb.coding.ast, ast);
        opMissing.set(kb.coding.cell, compile(ast[kb.coding.cell]));
        opMissing.set(kb.coding.role, compile(ast[kb.coding.role]));
        return opMissing;
    } else if (&ast.type() == &kb.type.ast.Get) {
        Object& opGet = *new Object(kb, kb.type.op.Get);
        opGet.set(kb.coding.ast, ast);
        opGet.set(kb.coding.cell, compile(ast[kb.coding.cell]));
        opGet.set(kb.coding.role, compile(ast[kb.coding.role]));
        return opGet;
    } else if (&ast.type() == &kb.type.ast.Member) {
        CellI& member = compile(kb.ast.get(kb.ast.self(), kb.ast.cell(ast[kb.coding.role])));
        member.set(kb.coding.ast, ast);
        return member;
    } else if (&ast.type() == &kb.type.ast.Return) {
        Object& retOp = *new Object(kb, kb.type.op.Return);
        retOp.set(kb.coding.ast, ast);
        return retOp;
    }

    throw "Unknown function AST!";
}

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
    BaseT<Delete>(kb, kb.type.ast.Delete)
{
    set(kb.coding.cell, cell);
}

Ast::Set::Set(brain::Brain& kb, Base& cell, Base& role, Base& value) :
    BaseT<Set>(kb, kb.type.ast.Set)
{
    set(kb.coding.cell, cell);
    set(kb.coding.role, role);
    set(kb.coding.value, value);
}

Ast::If::If(brain::Brain& kb, Base& condition, Base& thenBranch) :
    BaseT<If>(kb, kb.type.ast.If)
{
    set(kb.coding.condition, condition);
    set(kb.coding.then, thenBranch);
}

Ast::If::If(brain::Brain& kb, Base& condition, Base& thenBranch, Base& elseBranch) :
    BaseT<If>(kb, kb.type.ast.If)
{
    set(kb.coding.condition, condition);
    set(kb.coding.then, thenBranch);
    set(kb.coding.else_, elseBranch);
}

Ast::Do::Do(brain::Brain& kb, Base& statement, Base& condition) :
    BaseT<Do>(kb, kb.type.ast.Do)
{
    set(kb.coding.condition, condition);
    set(kb.coding.statement, statement);
}

Ast::While::While(brain::Brain& kb, Base& condition, Base& statement) :
    BaseT<While>(kb, kb.type.ast.While)
{
    set(kb.coding.condition, condition);
    set(kb.coding.statement, statement);
}

Ast::Var::Var(brain::Brain& kb, CellI& role) :
    BaseT<Var>(kb, kb.type.ast.Var)
{
    set(kb.coding.role, role);
}

Ast::Set& Ast::Var::operator=(Base& value)
{
    return Set::New(kb, *this, Cell::New(kb, kb.coding.value), value);
}

Ast::Get& Ast::Var::operator*()
{
    return Get::New(kb, *this, Cell::New(kb, kb.coding.value));
}

Ast::Member::Member(brain::Brain& kb, CellI& role) :
    BaseT<Member>(kb, kb.type.ast.Member)
{
    set(kb.coding.role, role);
}

Ast::Set& Ast::Member::operator=(Base& value)
{
    Ast::Set& ret = Set::New(kb, Self::New(kb), Cell::New(kb, get(kb.coding.role)), value);
    return ret;
}

Ast::Get& Ast::Member::operator/(Base& role)
{
    return Get::New(kb, *this, role);
}

Ast::Has& Ast::Member::exist()
{
    return Has::New(kb, Self::New(kb), Cell::New(kb, get(kb.coding.role)));
}

Ast::Missing& Ast::Member::missing()
{
    return Missing::New(kb, Self::New(kb), Cell::New(kb, get(kb.coding.role)));
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
    BaseT<New>(kb, kb.type.ast.New)
{
    set(kb.coding.objectType, objectType);
}

Ast::New::New(brain::Brain& kb, Base& objectType, Base& constructor) :
    BaseT<New>(kb, kb.type.ast.New)
{
    set(kb.coding.objectType, objectType);
    set(kb.coding.constructor, constructor);
}

Ast::New::New(brain::Brain& kb, Base& objectType, Base& constructor, Slot& slot1) :
    BaseT<New>(kb, kb.type.ast.New)
{
    set(kb.coding.objectType, objectType);
    set(kb.coding.constructor, constructor);
    set(kb.coding.parameters, kb.list(slot1));
}

Ast::New::New(brain::Brain& kb, Base& objectType, Base& constructor, Slot& slot1, Slot& slot2) :
    BaseT<New>(kb, kb.type.ast.New)
{
    set(kb.coding.objectType, objectType);
    set(kb.coding.constructor, constructor);
    set(kb.coding.parameters, kb.list(slot1, slot2));
}

Ast::New::New(brain::Brain& kb, Base& objectType, Base& constructor, Slot& slot1, Slot& slot2, Slot& slot3) :
    BaseT<New>(kb, kb.type.ast.New)
{
    set(kb.coding.objectType, objectType);
    set(kb.coding.constructor, constructor);
    set(kb.coding.parameters, kb.list(slot1, slot2, slot3));
}

Ast::New::New(brain::Brain& kb, Base& objectType, Base& constructor, Slot& slot1, Slot& slot2, Slot& slot3, Slot& slot4) :
    BaseT<New>(kb, kb.type.ast.New)
{
    set(kb.coding.objectType, objectType);
    set(kb.coding.constructor, constructor);
    set(kb.coding.parameters, kb.list(slot1, slot2, slot3, slot4));
}

Ast::Same::Same(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Same>(kb, kb.type.ast.Same)
{
    set(kb.coding.lhs, lhs);
    set(kb.coding.rhs, rhs);
}

Ast::NotSame::NotSame(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<NotSame>(kb, kb.type.ast.NotSame)
{
    set(kb.coding.lhs, lhs);
    set(kb.coding.rhs, rhs);
}

Ast::Equal::Equal(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Equal>(kb, kb.type.ast.Equal)
{
    set(kb.coding.lhs, lhs);
    set(kb.coding.rhs, rhs);
}

Ast::NotEqual::NotEqual(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<NotEqual>(kb, kb.type.ast.NotEqual)
{
    set(kb.coding.lhs, lhs);
    set(kb.coding.rhs, rhs);
}

Ast::Has::Has(brain::Brain& kb, Base& cell, Base& role) :
    BaseT<Has>(kb, kb.type.ast.Has)
{
    set(kb.coding.cell, cell);
    set(kb.coding.role, role);
}

Ast::Missing::Missing(brain::Brain& kb, Base& cell, Base& role) :
    BaseT<Missing>(kb, kb.type.ast.Missing)
{
    set(kb.coding.cell, cell);
    set(kb.coding.role, role);
}

Ast::Get::Get(brain::Brain& kb, Base& cell, Base& role) :
    BaseT<Get>(kb, kb.type.ast.Get)
{
    set(kb.coding.cell, cell);
    set(kb.coding.role, role);
}

Ast::Get& Ast::Get::operator/(Base& role)
{
    return Get::New(kb, *this, role);
}

Ast::And::And(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<And>(kb, kb.type.ast.And)
{
    set(kb.coding.lhs, lhs);
    set(kb.coding.rhs, rhs);
}

Ast::Or::Or(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Or>(kb, kb.type.ast.Or)
{
    set(kb.coding.lhs, lhs);
    set(kb.coding.rhs, rhs);
}

Ast::Not::Not(brain::Brain& kb, Base& input) :
    BaseT<Not>(kb, kb.type.ast.Not)
{
    set(kb.coding.input, input);
}

Ast::Add::Add(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Add>(kb, kb.type.ast.Add)
{
    set(kb.coding.lhs, lhs);
    set(kb.coding.rhs, rhs);
}

Ast::Subtract::Subtract(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Subtract>(kb, kb.type.ast.Subtract)
{
    set(kb.coding.lhs, lhs);
    set(kb.coding.rhs, rhs);
}

Ast::Multiply::Multiply(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Multiply>(kb, kb.type.ast.Multiply)
{
    set(kb.coding.lhs, lhs);
    set(kb.coding.rhs, rhs);
}

Ast::Divide::Divide(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Divide>(kb, kb.type.ast.Divide)
{
    set(kb.coding.lhs, lhs);
    set(kb.coding.rhs, rhs);
}

Ast::LessThan::LessThan(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<LessThan>(kb, kb.type.ast.LessThan)
{
    set(kb.coding.lhs, lhs);
    set(kb.coding.rhs, rhs);
}

Ast::GreaterThan::GreaterThan(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<GreaterThan>(kb, kb.type.ast.GreaterThan)
{
    set(kb.coding.lhs, lhs);
    set(kb.coding.rhs, rhs);
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

Ast::Set& Ast::return_(Base& value)
{
    auto& ast = kb.ast;
    return ast.set(ast.output(kb.coding.value), ast.cell(kb.coding.value), value);
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

Ast::GreaterThan& Ast::greaterThan(Base& lhs, Base& rhs)
{
    return GreaterThan::New(kb, lhs, rhs);
}

Directions::Directions(brain::Brain& kb) :
    up(kb, kb.type.Cell, "up"),
    down(kb, kb.type.Cell, "down"),
    left(kb, kb.type.Cell, "left"),
    right(kb, kb.type.Cell, "right")
{
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

Visualization::Visualization(brain::Brain& kb) :
    color(kb, kb.type.Cell, "color"),
    pixels(kb, kb.type.Cell, "pixels")
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
        return kb.coding.emptyObject;
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
    examples(kb, kb.type.Cell)
{
    auto& coding = kb.coding;
    CellI* map   = nullptr;

    map = &kb.map(kb.type.Cell, kb.type.Slot,
                  kb.coding.input, coding.slot(kb.coding.input, kb.type.Picture),
                  kb.coding.output, coding.slot(kb.coding.output, kb.type.Picture));
    Demonstration.set(coding.slots, *map);

    map = &kb.map(kb.type.Cell, kb.type.Slot,
                  examples, coding.slot(examples, kb.type.ListOf(Demonstration)),
                  kb.coding.input, coding.slot(kb.coding.input, kb.type.Picture),
                  kb.coding.output, coding.slot(kb.coding.output, kb.type.Picture));
    Task.set(coding.slots, *map);
}

Brain::Brain() :
    m_initPhase(InitPhase::Init),
    sequence(*this),
    dimensions(*this),
    coding(*this),
    methods(*this),
    type(*this),
    pools(*this),
    ast(*this),
    directions(*this),
    coordinates(*this),
    colors(*this),
    boolean(*this),
    visualization(*this),
    numbers(*this),
    arc(*this),
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
    const auto _    = [this](CellI& cell) -> Ast::Cell& { return ast.cell(cell); };
    const auto in_  = [this](CellI& role) -> Ast::Input& { return ast.input(role); };
    const auto out_ = [this](CellI& role) -> Ast::Output& { return ast.output(role); };
    const auto m_   = [this](CellI& role) -> Ast::Member& { return ast.member(role); };
    const auto var  = [this](CellI& role) -> Ast::Var& { return ast.var(role); };

    CellI* mapPtr = nullptr;

    mapPtr = &map(type.Cell, type.Slot,
                  sequence.previous, coding.slot(sequence.previous, type.ListItem),
                  sequence.next, coding.slot(sequence.next, type.ListItem), coding.value, coding.slot(coding.value, type.Cell));
    type.ListItem.set(coding.slots, *mapPtr);
    type.ListItem.set(coding.memberOf, map(type.Type_, type.Type_, type.Iterator, type.Iterator));

    mapPtr = &map(type.Cell, type.Slot,
                  sequence.first, coding.slot(sequence.first, type.ListItem),
                  sequence.last, coding.slot(sequence.last, type.ListItem),
                  dimensions.size, coding.slot(dimensions.size, type.Number),
                  coding.objectType, coding.slot(coding.objectType, type.Cell),
                  coding.item, coding.slot(coding.item, type.Type_));
    type.List.set(coding.slots, *mapPtr);
    type.List.set(coding.subTypes, map(type.Cell, type.Type_, coding.objectType, type.ListItem));
    type.List.set(coding.memberOf, map(type.Type_, type.Type_, type.Container, type.Container));

    mapPtr = &map(type.Cell, type.Slot,
                  coding.keyType, coding.slot(coding.keyType, type.Cell),
                  coding.objectType, coding.slot(coding.objectType, type.Cell),
                  coding.list, coding.slot(coding.list, type.ListOf(type.Cell)),
                  coding.index, coding.slot(coding.index, type.Index),
                  coding.indexType, coding.slot(coding.indexType, type.Type_),
                  dimensions.size, coding.slot(dimensions.size, type.Number));
    type.Map.set(coding.slots, *mapPtr);

    Ast::Function& mapCtor = *new Ast::Function(*this, "Map::constructor");
    mapCtor.addAsts(ast.block(
        m_(dimensions.size)   = _(_0_),
        m_(coding.indexType)  = ast.new_(_(type.Type_)),
        m_(coding.keyType)    = m_(coding.type) / _(coding.subTypes) / _(coding.index) / _(coding.keyType),
        m_(coding.objectType) = m_(coding.type) / _(coding.subTypes) / _(coding.index) / _(coding.objectType)));

    Ast::Function& mapAdd = *new Ast::Function(*this, "Map::add");
    mapAdd.addInputs(list(
        ast.slot(coding.key, type.Cell),
        ast.slot(coding.value, type.Cell)));
    mapAdd.addAsts(ast.block(
        ast.if_(ast.equal(m_(dimensions.size), _(_0_)),
                ast.block(m_(coding.list) = ast.new_(_(type.List), _(coding.constructor), ast.slot(_(coding.objectType), m_(coding.objectType))),
                          ast.set(m_(coding.indexType), _(coding.slots), ast.new_(_(type.MapCellToSlot), _(coding.constructor))),
                          ast.set(m_(coding.indexType) / _(coding.slots), _(coding.list), ast.new_(_(type.List), _(coding.constructor), ast.slot(_(coding.objectType), _(type.Slot)))),
                          ast.set(m_(coding.indexType) / _(coding.slots), _(coding.index), ast.new_(_(type.Index))),
                          m_(coding.index) = ast.new_(m_(coding.indexType)))),
        m_(coding.list).call(_(sequence.add), ast.slot(_(coding.value), in_(coding.value))),
        var(type.Slot) = ast.new_(_(type.Slot)),
        ast.set(*var(type.Slot), _(coding.slotRole), in_(coding.key)),
        ast.set(*var(type.Slot), _(coding.slotType), m_(coding.objectType)),
        m_(dimensions.size) = ast.add(m_(dimensions.size), _(_1_)),
        ast.set(m_(coding.indexType) / _(coding.slots), _(dimensions.size), m_(dimensions.size)),
        ast.call(m_(coding.indexType) / _(coding.slots) / _(coding.list), _(sequence.add), ast.slot(_(coding.value), *var(type.Slot))),
        ast.set(m_(coding.indexType) / _(coding.slots) / _(coding.index), in_(coding.key), *var(type.Slot)),
        ast.set(m_(coding.index), in_(coding.key), in_(coding.value))));

    Ast::Function& mapSize = *new Ast::Function(*this, "Map::size");
    mapSize.addOutputs(list(
        ast.slot(coding.value, type.Number)));
    mapSize.addAsts(ast.block(
        ast.return_(m_(dimensions.size))));

    Ast::Function& mapEmpty = *new Ast::Function(*this, "Map::empty");
    mapEmpty.addOutputs(list(
        ast.slot(coding.value, type.Boolean)));
    mapEmpty.addAsts(ast.block(
        ast.if_(ast.equal(m_(dimensions.size), _(_0_)), ast.return_(_(boolean.true_)), ast.return_(_(boolean.false_)))));

    type.Map.set(coding.asts, map(type.Cell, type.ast.Function, coding.constructor, mapCtor, sequence.add, mapAdd, dimensions.size, mapSize, sequence.empty, mapEmpty));
    type.Map.set(coding.methods, map(type.Cell, type.op.Function, coding.constructor, mapCtor.compile(type.Map), sequence.add, mapAdd.compile(type.Map), dimensions.size, mapSize.compile(type.Map), sequence.empty, mapEmpty.compile(type.Map)));

    // Map<Cell, Slot>
    type.MapCellToSlot.set(coding.subTypes, map(type.Cell, type.Type_, coding.keyType, type.Cell, coding.objectType, type.Slot));
    type.MapCellToSlot.set(coding.memberOf, map(type.Type_, type.Type_, type.Map, type.Map));
    mapPtr = &map(type.Cell, type.Slot,
                  coding.list, coding.slot(coding.list, type.ListOf(type.Slot)),
                  coding.index, coding.slot(coding.index, type.Index),
                  coding.indexType, coding.slot(coding.indexType, type.Type_),
                  coding.keyType, coding.slot(coding.keyType, type.Cell),
                  coding.objectType, coding.slot(coding.objectType, type.Slot),
                  dimensions.size, coding.slot(dimensions.size, type.Number));
    type.MapCellToSlot.set(coding.slots, *mapPtr);
    type.MapCellToSlot.set(coding.methods, map(type.Cell, type.op.Function, coding.constructor, mapCtor.compile(type.Map), dimensions.size, mapSize.compile(type.Map), sequence.add, mapAdd.compile(type.Map), sequence.empty, mapEmpty.compile(type.Map)));

    // Map<Cell, Type>
    type.MapCellToType.set(coding.subTypes, map(type.Cell, type.Type_, coding.keyType, type.Cell, coding.objectType, type.Type_));
    type.MapCellToType.set(coding.memberOf, map(type.Type_, type.Type_, type.Map, type.Map));
    mapPtr = &map(type.Cell, type.Type_,
                  coding.list, coding.slot(coding.list, type.ListOf(type.Type_)),
                  coding.index, coding.slot(coding.index, type.Index),
                  coding.indexType, coding.slot(coding.indexType, type.Type_),
                  coding.keyType, coding.slot(coding.keyType, type.Cell),
                  coding.objectType, coding.slot(coding.objectType, type.Type_),
                  dimensions.size, coding.slot(dimensions.size, type.Number));
    type.MapCellToType.set(coding.slots, *mapPtr);
    type.MapCellToType.set(coding.methods, map(type.Cell, type.op.Function, coding.constructor, mapCtor.compile(type.Map), dimensions.size, mapSize.compile(type.Map), sequence.add, mapAdd.compile(type.Map), sequence.empty, mapEmpty.compile(type.Map)));

    // Map<Type, Type>
    type.MapTypeToType.set(coding.subTypes, map(type.Cell, type.Type_, coding.keyType, type.Type_, coding.objectType, type.Type_));
    type.MapTypeToType.set(coding.memberOf, map(type.Type_, type.Type_, type.Map, type.Map));
    mapPtr = &map(type.Type_, type.Type_,
                  coding.list, coding.slot(coding.list, type.ListOf(type.Type_)),
                  coding.index, coding.slot(coding.index, type.Index),
                  coding.indexType, coding.slot(coding.indexType, type.Type_),
                  coding.keyType, coding.slot(coding.keyType, type.Type_),
                  coding.objectType, coding.slot(coding.objectType, type.Type_),
                  dimensions.size, coding.slot(dimensions.size, type.Number));
    type.MapTypeToType.set(coding.slots, *mapPtr);
    type.MapTypeToType.set(coding.methods, map(type.Cell, type.op.Function, coding.constructor, mapCtor.compile(type.Map), dimensions.size, mapSize.compile(type.Map), sequence.add, mapAdd.compile(type.Map), sequence.empty, mapEmpty.compile(type.Map)));

    Ast::Function& listItemTemplate = *new Ast::Function(*this, "static ListItem::template");
    listItemTemplate.addInputs(list(
        ast.slot(coding.objectType, type.Type_)));
    listItemTemplate.addOutputs(list(
        ast.slot(coding.value, type.ListItem)));
    listItemTemplate.addAsts(ast.block(
        var(coding.result) = ast.new_(_(type.Type_)),
        ast.set(*var(coding.result), _(coding.slots), ast.new_(_(type.MapCellToSlot), _(coding.constructor))),
        var(coding.item) = ast.new_(_(type.Slot)),
        ast.set(*var(coding.item), _(coding.slotRole), _(sequence.previous)),
        ast.set(*var(coding.item), _(coding.slotType), ast.self()),
        ast.call(*var(coding.result) / _(coding.slots), _(sequence.add), ast.slot(_(coding.key), _(sequence.previous)), ast.slot(_(coding.value), *var(coding.item))),
        var(coding.item) = ast.new_(_(type.Slot)),
        ast.set(*var(coding.item), _(coding.slotRole), _(sequence.next)),
        ast.set(*var(coding.item), _(coding.slotType), ast.self()),
        ast.call(*var(coding.result) / _(coding.slots), _(sequence.add), ast.slot(_(coding.key), _(sequence.next)), ast.slot(_(coding.value), *var(coding.item))),
        var(coding.item) = ast.new_(_(type.Slot)),
        ast.set(*var(coding.item), _(coding.slotRole), _(coding.value)),
        ast.set(*var(coding.item), _(coding.slotType), in_(coding.objectType)),
        ast.call(*var(coding.result) / _(coding.slots), _(sequence.add), ast.slot(_(coding.key), _(coding.value)), ast.slot(_(coding.value), *var(coding.item))),

        ast.set(*var(coding.result), _(coding.memberOf), ast.new_(_(type.MapTypeToType), _(coding.constructor))),
        ast.call(*var(coding.result) / _(coding.memberOf), _(sequence.add), ast.slot(_(coding.key), _(type.ListItem)), ast.slot(_(coding.value), _(type.ListItem))),

        ast.set(*var(coding.result), _(coding.methods), m_(coding.methods)),

        ast.return_(*var(coding.result))));

    Ast::Function& listItemCtor = *new Ast::Function(*this, "ListItem::constructor");
    listItemCtor.addInputs(list(
        ast.slot(coding.value, type.Cell)));
    listItemCtor.addAsts(ast.block(
        m_(coding.value) = in_(coding.value)));
    type.ListItem.set(coding.asts, map(type.Cell, type.ast.Function, coding.template_, listItemTemplate, coding.constructor, listItemCtor));
    type.ListItem.set(coding.methods, map(type.Cell, type.op.Function, coding.template_, listItemTemplate.compile(type.ListItem), coding.constructor, listItemCtor.compile(type.ListItem)));

    Ast::Function& listTemplate = *new Ast::Function(*this, "static List::template");
    listTemplate.addInputs(list(
        ast.slot(coding.objectType, type.Type_)));
    listTemplate.addOutputs(list(
        ast.slot(coding.value, type.List)));
    listTemplate.addAsts(ast.block(
        var(type.ListItem) = ast.scall(_(type.ListItem), _(coding.template_), ast.slot(_(coding.objectType), in_(coding.objectType))),
        var(coding.result) = ast.new_(_(type.Type_)),

        ast.set(*var(coding.result), _(coding.subTypes), ast.new_(_(type.MapCellToType), _(coding.constructor))),
        ast.call(*var(coding.result) / _(coding.subTypes), _(sequence.add), ast.slot(_(coding.key), _(coding.objectType)), ast.slot(_(coding.value), *var(type.ListItem))),

        ast.set(*var(coding.result), _(coding.memberOf), ast.new_(_(type.MapTypeToType), _(coding.constructor))),
        ast.call(*var(coding.result) / _(coding.memberOf), _(sequence.add), ast.slot(_(coding.key), _(type.List)), ast.slot(_(coding.value), _(type.List))),

        ast.set(*var(coding.result), _(coding.methods), m_(coding.methods)),

        ast.set(*var(coding.result), _(coding.slots), ast.new_(_(type.MapCellToSlot), _(coding.constructor))),
        var(coding.item) = ast.new_(_(type.Slot)),
        ast.set(*var(coding.item), _(coding.slotRole), _(sequence.first)),
        ast.set(*var(coding.item), _(coding.slotType), *var(type.ListItem)),
        ast.call(*var(coding.result) / _(coding.slots), _(sequence.add), ast.slot(_(coding.key), _(sequence.first)), ast.slot(_(coding.value), *var(coding.item))),

        var(coding.item) = ast.new_(_(type.Slot)),
        ast.set(*var(coding.item), _(coding.slotRole), _(sequence.last)),
        ast.set(*var(coding.item), _(coding.slotType), *var(type.ListItem)),
        ast.call(*var(coding.result) / _(coding.slots), _(sequence.add), ast.slot(_(coding.key), _(sequence.last)), ast.slot(_(coding.value), *var(coding.item))),

        var(coding.item) = ast.new_(_(type.Slot)),
        ast.set(*var(coding.item), _(coding.slotRole), _(dimensions.size)),
        ast.set(*var(coding.item), _(coding.slotType), _(type.Number)),
        ast.call(*var(coding.result) / _(coding.slots), _(sequence.add), ast.slot(_(coding.key), _(dimensions.size)), ast.slot(_(coding.value), *var(coding.item))),

        var(coding.item) = ast.new_(_(type.Slot)),
        ast.set(*var(coding.item), _(coding.slotRole), _(coding.objectType)),
        ast.set(*var(coding.item), _(coding.slotType), in_(coding.objectType)),
        ast.call(*var(coding.result) / _(coding.slots), _(sequence.add), ast.slot(_(coding.key), _(coding.objectType)), ast.slot(_(coding.value), *var(coding.item))),

        var(coding.item) = ast.new_(_(type.Slot)),
        ast.set(*var(coding.item), _(coding.slotRole), _(coding.item)),
        ast.set(*var(coding.item), _(coding.slotType), *var(type.ListItem)),
        ast.call(*var(coding.result) / _(coding.slots), _(sequence.add), ast.slot(_(coding.key), _(coding.item)), ast.slot(_(coding.value), *var(coding.item))),

        ast.return_(*var(coding.result))));

    Ast::Function& listCtor = *new Ast::Function(*this, "List::constructor");
    listCtor.addInputs(list(
        ast.slot(coding.objectType, type.Type_)));
    listCtor.addAsts(ast.block(
        m_(dimensions.size)   = _(_0_),
        m_(coding.objectType) = in_(coding.objectType),
        m_(coding.item)       = m_(coding.type) / _(coding.subTypes) / _(coding.index) / _(coding.objectType)
    ));

    Ast::Function& listAdd = *new Ast::Function(*this, "List::add");
    listAdd.addInputs(list(
        ast.slot(coding.value, type.Cell)));
    listAdd.addAsts(ast.block(
        var(coding.item) = ast.new_(m_(coding.item), _(coding.constructor), ast.slot(_(coding.value), in_(coding.value))),
        ast.if_(ast.not_(m_(sequence.first).exist()),
                m_(sequence.first) = *var(coding.item),                                    // then
                ast.block(ast.set(m_(sequence.last), _(sequence.next), *var(coding.item)), // else
                          ast.set(*var(coding.item), _(sequence.previous), m_(sequence.last)))),
        m_(sequence.last)   = *var(coding.item),
        m_(dimensions.size) = ast.add(m_(dimensions.size), _(_1_))));

    Ast::Function& listSize = *new Ast::Function(*this, "List::size");
    listSize.addOutputs(list(
        ast.slot(coding.value, type.Number)));
    listSize.addAsts(ast.block(
        ast.return_(m_(dimensions.size))));

    Ast::Function& listEmpty = *new Ast::Function(*this, "List::empty");
    listEmpty.addOutputs(list(
        ast.slot(coding.value, type.Boolean)));
    listEmpty.addAsts(ast.block(
        ast.if_(ast.equal(m_(dimensions.size), _(_0_)), ast.return_(_(boolean.true_)), ast.return_(_(boolean.false_)))));

    type.List.set(coding.asts, map(type.Cell, type.ast.Function, coding.template_, listTemplate, coding.constructor, listCtor, sequence.add, listAdd, dimensions.size, listSize, sequence.empty, listEmpty));
    type.List.set(coding.methods, map(type.Cell, type.op.Function, coding.template_, listTemplate.compile(type.List), coding.constructor, listCtor.compile(type.List), sequence.add, listAdd.compile(type.List), dimensions.size, listSize.compile(type.List), sequence.empty, listEmpty.compile(type.List)));

    Ast::Function& typeCtor = *new Ast::Function(*this, "Type::constructor");
    typeCtor.addInputs(list(
        ast.slot(coding.slots, type.MapCellToSlot),
        ast.slot(coding.subTypes, type.MapCellToType),
        ast.slot(coding.memberOf, type.MapTypeToType),
        ast.slot(coding.methods, type.MapCellToOpFunction)));
    typeCtor.addAsts(ast.block(
        m_(coding.slots)    = in_(coding.slots),
        m_(coding.subTypes) = in_(coding.subTypes),
        m_(coding.memberOf) = in_(coding.memberOf),
        m_(coding.methods)  = in_(coding.methods))); // TODO we have to compile AST to OP here

    Ast::Function& typeAddSlot = *new Ast::Function(*this, "Type::addSlot");
    typeAddSlot.addInputs(list(
        ast.slot(coding.value, type.Slot)));
    typeAddSlot.addAsts(ast.block(
        ast.if_(m_(coding.slots).missing(), m_(coding.slots) = ast.new_(_(type.MapCellToSlot), _(coding.constructor))),
        ast.call(m_(coding.slots), _(sequence.add), ast.slot(_(coding.key), in_(coding.value) / _(coding.slotRole)), ast.slot(_(coding.value), in_(coding.value)))));

    Ast::Function& typeAddSlots = *new Ast::Function(*this, "Type::addSlots");
    typeAddSlots.addInputs(list(
        ast.slot(coding.list, type.ListOf(type.Slot))));
    typeAddSlots.addAsts(ast.block(
        ast.if_(ast.equal(in_(coding.list) / _(dimensions.size), _(_0_)), ast.return_()),
        var(coding.item) = in_(coding.list) / _(sequence.first),
        ast.if_(m_(coding.slots).missing(), m_(coding.slots) = ast.new_(_(type.MapCellToSlot), _(coding.constructor))),
        ast.do_(ast.block(
                    ast.var(sequence.next) = _(boolean.true_),
                    ast.call(m_(coding.slots), _(sequence.add), ast.slot(_(coding.key), *var(coding.item) / _(coding.value) / _(coding.slotRole)), ast.slot(_(coding.value), *var(coding.item) / _(coding.value))),
                    ast.if_(ast.has(*var(coding.item), _(sequence.next)),
                            var(coding.item)   = *var(coding.item) / _(sequence.next),
                            var(sequence.next) = _(boolean.false_))),
                ast.same(*var(sequence.next), _(boolean.true_)))));
    type.Type_.set(coding.asts, map(type.Cell, type.ast.Function, coding.constructor, typeCtor, methods.addSlot, typeAddSlot, methods.addSlots, typeAddSlots));
    type.Type_.set(coding.methods, map(type.Cell, type.op.Function, coding.constructor, typeCtor.compile(type.Type_), methods.addSlot, typeAddSlot.compile(type.Type_), methods.addSlots, typeAddSlots.compile(type.Type_)));

#if 0
    Object methodData(*this, type.Cell);
    methodData.set(coding.ast, listAdd);
    methodData.set(coding.op, listAdd.compile());
    methodData.set(coding.static_, boolean.false_);
    methodData.set(coding.const_, boolean.false_);
#endif
    mapPtr = &map(type.Cell, type.Slot,
                  coding.value, coding.slot(coding.value, type.ListOf(type.Digit)),
                  numbers.sign, coding.slot(numbers.sign, type.Number)); // TODO sign has no class currently
    type.Number.set(coding.slots, *mapPtr);

    type.String.method(methods.addSlots, { coding.list, list(coding.slot(coding.value, type.ListOf(type.Char))) });

    type.Color.method(methods.addSlots, { coding.list, list(
        coding.slot(colors.red, type.Number),
        coding.slot(colors.green, type.Number),
        coding.slot(colors.blue, type.Number)) });

    type.Pixel.method(methods.addSlots, { coding.list, list(
        coding.slot(directions.up, type.Pixel),
        coding.slot(directions.down, type.Pixel),
        coding.slot(directions.left, type.Pixel),
        coding.slot(directions.right, type.Pixel),
        coding.slot(coordinates.x, type.Number),
        coding.slot(coordinates.y, type.Number)) });

    type.Picture.method(methods.addSlots, { coding.list, list(
        coding.slot(dimensions.width, type.Number),
        coding.slot(dimensions.height, type.Number),
        coding.slot(visualization.pixels, type.ListOf(type.Pixel))) });

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
