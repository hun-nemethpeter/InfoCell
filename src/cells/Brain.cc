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
    addPixel(kb, kb.type.Cell, "addPixel"),
    addSlot(kb, kb.type.Cell, "addSlot"),
    addSlots(kb, kb.type.Cell, "addSlots"),
    addSubType(kb, kb.type.Cell, "addSubType"),
    argument(kb, kb.type.Cell, "argument"),
    ast(kb, kb.type.Cell, "ast"),
    asts(kb, kb.type.Cell, "asts"),
    branch(kb, kb.type.Cell, "branch"),
    cell(kb, kb.type.Cell, "cell"),
    checkPixel(kb, kb.type.Cell, "checkPixel"),
    checkPixels(kb, kb.type.Cell, "checkPixels"),
    children(kb, kb.type.Cell, "children"),
    code(kb, kb.type.Cell, "code"),
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
    data(kb, kb.type.Cell, "data"),
    destructor(kb, kb.type.Cell, "destructor"),
    direction(kb, kb.type.Cell, "direction"),
    else_(kb, kb.type.Cell, "else_"),
    empty(kb, kb.type.Cell, "empty"),
    emptyObject(kb, kb.type.Cell, "emptyObject"),
    erase(kb, kb.type.Cell, "erase"),
    eval(kb, kb.type.Cell, "eval"),
    first(kb, kb.type.Cell, "first"),
    firstPixel(kb, kb.type.Cell, "firstPixel"),
    functions(kb, kb.type.Cell, "functions"),
    functionTs(kb, kb.type.Cell, "functionTs"),
    getValue(kb, kb.type.Cell, "getValue"),
    global(kb, kb.type.Cell, "global"),
    has(kb, kb.type.Cell, "has"),
    hasKey(kb, kb.type.Cell, "hasKey"),
    hasPixel(kb, kb.type.Cell, "hasPixel"),
    hasSlot(kb, kb.type.Cell, "hasSlot"),
    height(kb, kb.type.Cell, "height"),
    id(kb, kb.type.Cell, "id"),
    incompleteStructTypes(kb, kb.type.Cell, "incompleteStructTypes"),
    index(kb, kb.type.Cell, "index"),
    indexType(kb, kb.type.Cell, "indexType"),
    input(kb, kb.type.Cell, "input"),
    inputPixels(kb, kb.type.Cell, "inputPixels"),
    insert(kb, kb.type.Cell, "insert"),
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
    parent(kb, kb.type.Cell, "parent"),
    picture(kb, kb.type.Cell, "picture"),
    pixel(kb, kb.type.Cell, "pixel"),
    pixels(kb, kb.type.Cell, "pixels"),
    previous(kb, kb.type.Cell, "previous"),
    process(kb, kb.type.Cell, "process"),
    processAdjacentPixel(kb, kb.type.Cell, "processAdjacentPixel"),
    processInputPixels(kb, kb.type.Cell, "processInputPixels"),
    processPixel(kb, kb.type.Cell, "processPixel"),
    recursiveType(kb, kb.type.Cell, "recursiveType"),
    remove(kb, kb.type.Cell, "remove"),
    removeSlot(kb, kb.type.Cell, "removeSlot"),
    result(kb, kb.type.Cell, "result"),
    returnType(kb, kb.type.Cell, "returnType"),
    returnValue(kb, kb.type.Cell, "returnValue"),
    rhs(kb, kb.type.Cell, "rhs"),
    role(kb, kb.type.Cell, "role"),
    rootNode(kb, kb.type.Cell, "rootNode"),
    scope(kb, kb.type.Cell, "scope"),
    scopes(kb, kb.type.Cell, "scopes"),
    self(kb, kb.type.Cell, "self"),
    shape(kb, kb.type.Cell, "shape"),
    shapeId(kb, kb.type.Cell, "shapeId"),
    shapes(kb, kb.type.Cell, "shapes"),
    sharedObject(kb, kb.type.Cell, "sharedObject"),
    size(kb, kb.type.Cell, "size"),
    slot(kb, kb.type.Cell, "slot"),
    slotRole(kb, kb.type.Cell, "slotRole"),
    slots(kb, kb.type.Cell, "slots"),
    slotType(kb, kb.type.Cell, "slotType"),
    sortPixels(kb, kb.type.Cell, "sortPixels"),
    stack(kb, kb.type.Cell, "stack"),
    statement(kb, kb.type.Cell, "statement"),
    static_(kb, kb.type.Cell, "static"),
    status(kb, kb.type.Cell, "status"),
    stop(kb, kb.type.Cell, "stop"),
    structs(kb, kb.type.Cell, "structs"),
    structTInstances(kb, kb.type.Cell, "structTInstances"),
    structTs(kb, kb.type.Cell, "structTs"),
    subTypes(kb, kb.type.Cell, "subTypes"),
    template_(kb, kb.type.Cell, "template"),
    templateParams(kb, kb.type.Cell, "templateParams"),
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
    DependentType(kb, kb.type.Type_, "ast::DependentType"),
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
    StructT(kb, kb.type.Type_, "ast::StructT"),
    Subtract(kb, kb.type.Type_, "ast::Subtract"),
    TemplateParam(kb, kb.type.Type_, "ast::TemplateParam"),
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

    map = &kb.slots(type.slot(id.id, Base),
                    type.slot(id.parameters, type.ListOf(Slot)));
    DependentType.set(id.slots, *map);

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
                    type.slot(id.parameters, type.ListOf(Slot)),
                    type.slot(id.returnType, type.Type_),
                    type.slot(id.code, Base),
                    type.slot(id.scope, Base),
                    type.slot(id.static_, type.Boolean));
    Function.set(id.slots, *map);

    map = &kb.slots(type.slot(id.objectType, type.Cell),
                    type.slot(id.name, type.Cell),
                    type.slot(id.parameters, type.ListOf(Slot)),
                    type.slot(id.returnType, type.Type_),
                    type.slot(id.code, Base),
                    type.slot(id.scope, Base),
                    type.slot(id.static_, type.Boolean));
    FunctionT.set(id.slots, *map);

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
    Parameter.set(id.slots, *map);

    map = &kb.slots(type.slot(id.value, type.Cell));
    Return.set(id.slots, *map);

    map = &kb.slots(type.slot(id.lhs, Base),
                    type.slot(id.rhs, Base));
    Same.set(id.slots, *map);

    map = &kb.slots(type.slot(id.incompleteStructTypes, type.TrieMap),
                    type.slot(id.structTInstances, type.TrieMap),
                    type.slot(id.id, type.Cell),
                    type.slot(id.scopes, type.Map),
                    type.slot(id.functions, type.Map),
                    type.slot(id.structs, type.Map),
                    type.slot(id.structTs, type.Map));
    Scope.set(id.slots, *map);

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

    map = &kb.slots(type.slot(id.id, type.Cell),
                    type.slot(id.scope, Base),
                    type.slot(id.methods, type.MapOf(type.Cell, type.ast.Function)),
                    type.slot(id.members, type.ListOf(type.ast.Slot)),
                    type.slot(id.subTypes, type.ListOf(type.ast.Slot)),
                    type.slot(id.memberOf, type.ListOf(type.Type_)));
    Struct.set(id.slots, *map);

    map = &kb.slots(type.slot(id.id, type.Cell),
                    type.slot(id.scope, Base),
                    type.slot(id.methods, type.MapOf(type.Cell, type.ast.Function)),
                    type.slot(id.members, type.ListOf(type.ast.Slot)),
                    type.slot(id.subTypes, type.ListOf(type.ast.Slot)),
                    type.slot(id.memberOf, type.ListOf(type.Type_)),
                    type.slot(id.templateParams, type.MapOf(type.Cell, type.Type_)));
    StructT.set(id.slots, *map);

    map = &kb.slots(type.slot(id.lhs, Base),
                    type.slot(id.rhs, Base));
    Subtract.set(id.slots, *map);

    map = &kb.slots(type.slot(id.role, type.Cell));
    TemplateParam.set(id.slots, *map);

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
    Directions(kb, kb.type.Enum, "Directions"),
    Shape(kb, kb.type.Struct, "Shape"),
    op(kb),
    ast(kb),
    arc(kb)
{
    CellI* mapPtr = nullptr;
    auto& id   = kb.id;
    auto& type = kb.type;

    mapPtr = &kb.slots(type.slot(id.slotType, type.Type_),
                       type.slot(id.slotRole, type.Cell));
    Slot.set(kb.id.slots, *mapPtr);

    mapPtr = &kb.slots(type.slot(id.slots, MapCellToSlot),
                       type.slot(id.sharedObject, Slot),
                       type.slot(id.subTypes, MapCellToType),
                       type.slot(id.memberOf, MapTypeToType),
                       type.slot(id.asts, MapCellToAstFunction),
                       type.slot(id.methods, MapCellToOpFunction));
    Type_.set(id.slots, *mapPtr);

    mapPtr = &kb.slots(type.slot(id.members, List));
    Enum.set(id.slots, *mapPtr);

    mapPtr = &kb.slots(type.slot(id.keyType, type.Cell),
                       type.slot(id.objectType, type.Cell),
                       type.slot(id.list, type.ListOf(type.Cell)),
                       type.slot(id.listType, type.List),
                       type.slot(id.rootNode, type.TrieMapNode),
                       type.slot(id.size, type.Number));
    type.TrieMap.set(id.slots, *mapPtr);

    mapPtr = &kb.slots(type.slot(id.children, type.Index),
                       type.slot(id.data, type.ListItem),
                       type.slot(id.parent, type.TrieMapNode));
    type.TrieMapNode.set(id.slots, *mapPtr);

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

    if (kb.type.Map.has(kb.id.methods)) {
        mapType.set(kb.id.methods, kb.type.Map[kb.id.methods]); // TODO
    }
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

Ast::Parameter::Parameter(brain::Brain& kb, CellI& role) :
    BaseT<Parameter>(kb, kb.type.ast.Parameter)
{
    set(kb.id.role, role);
}

Ast::Get& Ast::Parameter::operator/(Base& role)
{
    return Get::New(kb, *this, role);
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

Ast::Scope::Scope(brain::Brain& kb, CellI& id, const std::string& label) :
    BaseT<Scope>(kb, kb.type.ast.Scope, label)
{
    set(kb.id.id, id);
}

Ast::Scope& Ast::Scope::addScope(CellI& id, const std::string& label)
{
    if (missing(kb.id.scopes)) {
        set(kb.id.scopes, *new Map(kb, kb.type.Cell, kb.type.ast.Scope, "Map<Cell, Type::Ast::Scope>(...)"));
    }
    if (scopes().hasKey(id)) {
        throw "Already registered!";
    }
    auto& scope = *new Ast::Scope(kb, id, label);
    scopes().add(id, scope);

    return scope;
}

Ast::Function& Ast::Scope::addFunction(CellI& id, const std::string& label)
{
    if (missing(kb.id.functions)) {
        set(kb.id.functions, *new Map(kb, kb.type.Cell, kb.type.ast.Function, "Map<Cell, Type::Ast::Function>(...)"));
    }
    if (functions().hasKey(id)) {
        throw "Already registered!";
    }
    auto& function = *new Ast::Function(kb, id, label);
    functions().add(id, function);
    function.set(kb.id.scope, *this);

    return function;
}

Ast::FunctionT& Ast::Scope::addFunctionT(CellI& id, const std::string& label)
{
    if (missing(kb.id.functionTs)) {
        set(kb.id.functionTs, *new Map(kb, kb.type.Cell, kb.type.ast.FunctionT, "Map<Cell, Type::Ast::FunctionT>(...)"));
    }
    if (functionTs().hasKey(id)) {
        throw "Already registered!";
    }
    auto& functionT = *new Ast::FunctionT(kb, id, label);
    functionTs().add(id, functionT);
    functionT.set(kb.id.scope, *this);

    return functionT;
}

Ast::Struct& Ast::Scope::addStruct(CellI& id, const std::string& label)
{
    if (missing(kb.id.structs)) {
        set(kb.id.structs, *new Map(kb, kb.type.Cell, kb.type.ast.Struct, "Map<Cell, Type::Ast::Struct>(...)"));
    }
    if (structs().hasKey(id)) {
        throw "Already registered!";
    }
    auto& struct_ = *new Ast::Struct(kb, id, label);
    structs().add(id, struct_);
    struct_.set(kb.id.scope, *this);

    return struct_;
}

Ast::StructT& Ast::Scope::addStructT(CellI& id, const std::string& label)
{
    if (missing(kb.id.structTs)) {
        set(kb.id.structTs, *new Map(kb, kb.type.Cell, kb.type.ast.StructT, "Map<Cell, Type::Ast::StructT>(...)"));
    }
    if (structTs().hasKey(id)) {
        throw "Already registered!";
    }
    auto& structT = *new Ast::StructT(kb, id, label);
    structTs().add(id, structT);
    structT.set(kb.id.scope, *this);

    return structT;
}

void Ast::Scope::addStructTInstance(Struct& astStruct)
{
    List& id = static_cast<List&>(astStruct.id());
    if (missing(kb.id.structTInstances)) {
        set(kb.id.structTInstances, *new TrieMap(kb, kb.type.Cell, kb.type.ast.Struct, "TrieMap<Cell, Type::Ast::StructT>(...)"));
    }
    if (structTs().hasKey(id)) {
        throw "Already registered!";
    }
    structTInstances().add(id, astStruct);
    astStruct.set(kb.id.scope, *this);
}

void Ast::Scope::implicitInstantiation()
{
    if (has(kb.id.functions)) {
        Visitor::visitList(functions()[kb.id.list], [this](CellI& function, int i, bool& stop) {
            Ast::Function& astFunction = static_cast<Ast::Function&>(function);
            astFunction.implicitInstantiation();
        });
    }
    if (has(kb.id.structs)) {
        Visitor::visitList(structs()[kb.id.list], [this](CellI& struct_, int i, bool& stop) {
            Ast::Struct& astStruct = static_cast<Ast::Struct&>(struct_);
            astStruct.implicitInstantiation();
        });
    }
    if (has(kb.id.scopes)) {
        Visitor::visitList(scopes()[kb.id.list], [this](CellI& scope, int i, bool& stop) {
            Ast::Scope& astScope = static_cast<Ast::Scope&>(scope);
            astScope.implicitInstantiation();
        });
    }
}

CellI& Ast::Scope::compile()
{
    implicitInstantiation();
    // TODO Maybe we should give back a compiled object?
    return kb.id.cell;
}

Ast::Struct& Ast::Scope::instantiateIncompleteStructT(CellI& id, List& parameters)
{
    if (missing(kb.id.structTs)) {
        throw "No such struct template!";
    }

    Ast::StructT& structT = static_cast<Ast::StructT&>(structTs().getValue(id));
    List& idCell          = *new List(kb, kb.type.Cell);
    idCell.add(id);

    Visitor::visitList(parameters, [this, &idCell](CellI& slot, int, bool&) {
        idCell.add(slot[kb.id.slotRole]);
        idCell.add(slot[kb.id.slotType]);
    });

    if (has(kb.id.structs)) {
        auto& existingStructs = structs();
        if (existingStructs.hasKey(idCell)) {
            return static_cast<Ast::Struct&>(existingStructs.getValue(idCell));
        }
    }

    return addIncompleteStruct(idCell);
}

void Ast::Scope::addIncompleteStruct(Struct& astStruct)
{
    if (missing(kb.id.incompleteStructTypes)) {
        set(kb.id.incompleteStructTypes, *new TrieMap(kb, kb.type.Cell, kb.type.ast.Struct, "TrieMap<List(id, params...), Type::Ast::Struct>(...)"));
    }
    List& id = static_cast<List&>(astStruct.get(kb.id.id));
    if (incompleteStructTypes().has(id)) {
        return;
    }
    incompleteStructTypes().add(id, astStruct);
}

Ast::Struct& Ast::Scope::addIncompleteStruct(List& id)
{
    if (missing(kb.id.incompleteStructTypes)) {
        set(kb.id.incompleteStructTypes, *new TrieMap(kb, kb.type.Cell, kb.type.ast.Struct, "TrieMap<List(id, params...), Type::Ast::Struct>(...)"));
    }
    if (incompleteStructTypes().hasKey(id)) {
        return static_cast<Ast::Struct&>(incompleteStructTypes().getValue(id));
    }
    Struct& ret = *new Struct(kb, id);
    incompleteStructTypes().add(id, ret);

    return ret;
}

Map& Ast::Scope::scopes()
{
    if (missing(kb.id.scopes)) {
        throw "No scopes!";
    } else {
        return static_cast<Map&>(get(kb.id.scopes));
    }
}

Map& Ast::Scope::functions()
{
    if (missing(kb.id.functions)) {
        throw "No functions!";
    } else {
        return static_cast<Map&>(get(kb.id.functions));
    }
}

Map& Ast::Scope::functionTs()
{
    if (missing(kb.id.functionTs)) {
        throw "No functions!";
    } else {
        return static_cast<Map&>(get(kb.id.functionTs));
    }
}

Map& Ast::Scope::structs()
{
    if (missing(kb.id.structs)) {
        throw "No structs!";
    } else {
        return static_cast<Map&>(get(kb.id.structs));
    }
}

Map& Ast::Scope::structTs()
{
    if (missing(kb.id.structTs)) {
        throw "No structs!";
    } else {
        return static_cast<Map&>(get(kb.id.structTs));
    }
}

TrieMap& Ast::Scope::structTInstances()
{
    if (missing(kb.id.structTInstances)) {
        throw "No incomplete struct types!";
    } else {
        return static_cast<TrieMap&>(get(kb.id.structTInstances));
    }
}

TrieMap& Ast::Scope::incompleteStructTypes()
{
    if (missing(kb.id.incompleteStructTypes)) {
        throw "No incomplete struct types!";
    } else {
        return static_cast<TrieMap&>(get(kb.id.incompleteStructTypes));
    }
}

Ast::StructBase::StructBase(brain::Brain& kb, CellI& astType, CellI& id, const std::string& label) :
    Base(kb, astType, label)
{
    set(kb.id.id, id);
}

Ast::Function& Ast::StructBase::addMethod(CellI& id, const std::string& label)
{
    Ast::Function& function = *new Ast::Function(kb, this->id(), id, label);
    if (missing(kb.id.methods)) {
        set(kb.id.methods, *new Map(kb, kb.type.Cell, kb.type.ast.Function, "Map<Cell, Type::Ast::Function>(...)"));
    }
    if (methods().hasKey(id)) {
        throw "Already registered!";
    }
    methods().add(id, function);

    return function;
}

void Ast::StructBase::members(Slot& param)
{
    if (missing(kb.id.members)) {
        set(kb.id.members, *new List(kb, kb.type.ast.Slot));
    }
    members().add(param);
}

void Ast::StructBase::subTypes(Slot& param)
{
    if (missing(kb.id.subTypes)) {
        set(kb.id.subTypes, *new List(kb, kb.type.ast.Slot));
    }
    subTypes().add(param);
}

void Ast::StructBase::memberOf(CellI& type)
{
    if (missing(kb.id.memberOf)) {
        set(kb.id.memberOf, *new List(kb, kb.type.Type_));
    }
    memberOf().add(type);
}

Map& Ast::StructBase::methods()
{
    if (missing(kb.id.methods)) {
        throw "No methods!";
    } else {
        return static_cast<Map&>(get(kb.id.methods));
    }
}

List& Ast::StructBase::members()
{
    if (missing(kb.id.members)) {
        throw "No members!";
    } else {
        return static_cast<List&>(get(kb.id.members));
    }
}

List& Ast::StructBase::subTypes()
{
    if (missing(kb.id.subTypes)) {
        throw "No subTypes!";
    } else {
        return static_cast<List&>(get(kb.id.subTypes));
    }
}

List& Ast::StructBase::memberOf()
{
    if (missing(kb.id.memberOf)) {
        throw "No memberOf!";
    } else {
        return static_cast<List&>(get(kb.id.memberOf));
    }
}

CellI& Ast::StructBase::id()
{
    return get(kb.id.id);
}

Ast::Struct::Struct(brain::Brain& kb, CellI& id, const std::string& label) :
    StructBase(kb, kb.type.ast.Struct, id, label)
{
}

void Ast::Struct::implicitInstantiation()
{
    if (has(kb.id.methods)) {
        Visitor::visitList(methods()[kb.id.list], [this](CellI& function, int i, bool& stop) {
            Ast::Function& astFunction = static_cast<Ast::Function&>(function);
            astFunction.implicitInstantiation();
        });
    }
}

Ast::StructT::StructT(brain::Brain& kb, CellI& id, const std::string& label) :
    StructBase(kb, kb.type.ast.StructT, id, label)
{
}

void Ast::StructT::templateParams(Slot& slot)
{
    if (missing(kb.id.templateParams)) {
        set(kb.id.templateParams, *new Map(kb, kb.type.Cell, kb.type.Type_));
    }
    templateParams().add(slot[kb.id.slotRole], slot);
}

Ast::Struct& Ast::StructT::declareType(List& parameters)
{
    Scope& scope = static_cast<Scope&>(get(kb.id.scope));
    return scope.instantiateIncompleteStructT(id(), parameters);
}

Ast::Struct& Ast::StructT::instantiate(Scope& scope, List& parameters)
{
    return instantiateWith(scope, parameters);
}

Ast::Struct& Ast::StructT::instantiateWith(Scope& scope, List& inputParams)
{
    // process input parameters
    if (!inputParams.empty() && missing(kb.id.templateParams)) {
        throw "No template parameter was given!";
    }
    std::stringstream ss;
    Map inputParameters(kb, kb.type.Cell, kb.type.Cell);
    List& idCell = *new List(kb, kb.type.Cell);
    idCell.add(id());

    Visitor::visitList(inputParams, [this, &inputParameters, &ss, &idCell](CellI& slot, int i, bool& stop) {
        CellI& slotRole = slot[kb.id.slotRole];
        CellI& slotType = slot[kb.id.slotType];
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
    Ast::Struct* ret = nullptr;
    if (scope.has(kb.id.incompleteStructTypes) && scope.incompleteStructTypes().hasKey(idCell)) {
        ret = &static_cast<Ast::Struct&>(scope.incompleteStructTypes().getValue(idCell));
        ret->label(std::format("{}<{}>", id().label(), ss.str()));
    } else {
        ret = new Ast::Struct(kb, idCell, std::format("{}<{}>", id().label(), ss.str()));
        scope.addIncompleteStruct(*ret);
    }

    // instantiate methods
    if (has(kb.id.methods)) {
        Map& instantiatedMethods = *new Map(kb, kb.type.Cell, kb.type.ast.Function);
        Visitor::visitList(methods()[kb.id.list], [this, &inputParameters, ret, &instantiatedMethods](CellI& astFunction, int i, bool& stop) {
            Ast::Function& instantiedFunction = *new Function(kb, kb.type.Cell, astFunction[kb.id.name], "");
            instantiatedMethods.add(astFunction[kb.id.name], instantiedFunction);

            // parameters
            if (astFunction.has(kb.id.parameters)) {
                List& instantiatedParameters = *new List(kb, kb.type.Slot);
                Visitor::visitList(astFunction[kb.id.parameters], [this, &inputParameters, &instantiatedParameters, ret](CellI& slot, int i, bool& stop) {
                    CellI& slotRole          = slot[kb.id.slotRole];
                    CellI& slotType          = slot[kb.id.slotType];
                    CellI& instantiatedParam = instantiateTemplateParam(slotType, *ret, inputParameters);
                    instantiatedParameters.add(kb.ast.slot(slotRole, instantiatedParam));
                });
                instantiedFunction.set(kb.id.parameters, instantiatedParameters);
            }
            // return type
            if (astFunction.has(kb.id.returnType)) {
                CellI& returnType             = astFunction[kb.id.returnType];
                CellI& instantiatedReturnType = instantiateTemplateParam(returnType, *ret, inputParameters);
                instantiedFunction.set(kb.id.returnType, instantiatedReturnType);
            }
            // code
            if (astFunction.has(kb.id.ast)) {
                instantiedFunction.set(kb.id.ast, instantiateAst(astFunction[kb.id.ast], *ret, inputParameters));
            }
        });
        ret->set(kb.id.methods, instantiatedMethods);
    }

    // instantiate members
    if (has(kb.id.members)) {
        List& instantiatedMembers = *new List(kb, kb.type.Slot);
        Visitor::visitList(members(), [this, &inputParameters, &instantiatedMembers, ret](CellI& slot, int i, bool& stop) {
            CellI& slotRole = slot[kb.id.slotRole];
            CellI& slotType = slot[kb.id.slotType];
            CellI& instantiatedParam = instantiateTemplateParam(slotType, *ret, inputParameters);
            instantiatedMembers.add(kb.ast.slot(slotRole, instantiatedParam));
        });
        ret->set(kb.id.members, instantiatedMembers);
    }

    // instantiate sub types
    if (has(kb.id.subTypes)) {
        List& instantiatedSubTypes = *new List(kb, kb.type.Type_);
        Visitor::visitList(subTypes(), [this, &inputParameters, &instantiatedSubTypes, ret](CellI& slot, int i, bool& stop) {
            CellI& slotRole = slot[kb.id.slotRole];
            CellI& slotType = slot[kb.id.slotType];
            CellI& instantiatedParam = instantiateTemplateParam(slotType, *ret, inputParameters);
            instantiatedSubTypes.add(kb.ast.slot(slotRole, instantiatedParam));
        });
        ret->set(kb.id.subTypes, instantiatedSubTypes);
    }

    // instantiate memberOf list
    if (has(kb.id.memberOf)) {
        List& instantiatedMemberOfs = *new List(kb, kb.type.Type_);
        Visitor::visitList(memberOf(), [this, &inputParameters, &instantiatedMemberOfs, ret](CellI& membershipType, int i, bool& stop) {
            CellI& instantiatedParam = instantiateTemplateParam(membershipType, *ret, inputParameters);
            instantiatedMemberOfs.add(instantiatedParam);
        });
        ret->set(kb.id.memberOf, instantiatedMemberOfs);
    }
    scope.addStructTInstance(*ret);

    return *ret;
}

CellI& Ast::StructT::instantiateTemplateParam(CellI& param, CellI& selfType, Map& inputParameters)
{
    if (&param.type() == &kb.type.ast.TemplateParam) {
        CellI& paramValue = param[kb.id.role];
        if (&paramValue == &kb.id.self) {
            return selfType;
        }
        if (!inputParameters.hasKey(paramValue)) {
            throw "Instantiating with unknown template parameter!";
        }
        return inputParameters.getValue(paramValue);
    }
    if (&param.type() == &kb.type.ast.DependentType) {
        Scope& scope = static_cast<Scope&>(get(kb.id.scope));
        if (scope.missing(kb.id.structTs)) {
            throw "No template in scope!";
        }
        StructT& structT = static_cast<StructT&>(scope.structTs().getValue(param[kb.id.id]));
        auto& parametersList  = param[kb.id.parameters];
        List resolvedParameterList(kb, kb.type.Cell);
        Visitor::visitList(parametersList, [this, &resolvedParameterList, &selfType, &inputParameters](CellI& slot, int, bool&) {
            CellI& slotRole = slot[kb.id.slotRole];
            CellI& slotType = slot[kb.id.slotType];
            CellI& resolvedSlotType = instantiateTemplateParam(slotType, selfType, inputParameters);
            resolvedParameterList.add(kb.ast.slot(slotRole, resolvedSlotType));
        });
        auto& ret = structT.declareType(resolvedParameterList);
        return ret;
    }
    if (&param.type() == &kb.type.ast.Cell) {
        return param[kb.id.value];
    }

    throw "Unknown template parameter!";
}

Ast::Base& Ast::StructT::instantiateAst(CellI& ast, CellI& selfType, Map& inputParameters)
{
    auto& id = kb.id;

    auto instantiate = [this, &selfType, &inputParameters](CellI& ast) -> Ast::Base& { return instantiateAst(ast, selfType, inputParameters); };

    if (&ast.type() == &kb.type.ast.TemplateParam) {
        CellI& paramValue = ast[kb.id.role];
        if (&paramValue == &kb.id.self) {
            return kb.ast.cell(selfType);
        }
        if (!inputParameters.hasKey(paramValue)) {
            throw "Instantiating with unknown template parameter!";
        }
        return kb.ast.cell(inputParameters.getValue(paramValue));
    } else if (&ast.type() == &kb.type.ast.DependentType) {
        return kb.ast.cell(ast); // TODO
    } else if (&ast.type() == &kb.type.ast.Block) {
        auto& instantiedAsts = *new cells::List(kb, kb.type.ast.Base);
        Visitor::visitList(ast[id.asts], [this, &instantiedAsts, &instantiate](CellI& ast, int, bool&) {
            instantiedAsts.add(instantiate(ast));
        });
        return kb.ast.block(instantiedAsts);
    } else if (&ast.type() == &kb.type.ast.Cell) {
        return kb.ast.cell(ast[id.value]);
    } else if (&ast.type() == &kb.type.ast.SelfFn) {
        return kb.ast.selfFn();
    } else if (&ast.type() == &kb.type.ast.Self) {
        return kb.ast.self();
    } else if (&ast.type() == &kb.type.ast.Parameter) {
        return kb.ast.parameter(ast[id.role]);
    } else if (&ast.type() == &kb.type.ast.Var) {
        return kb.ast.var(ast[id.role]);
    } else if (&ast.type() == &kb.type.ast.Delete) {
        return kb.ast.delete_(instantiate(ast[id.cell]));
    } else if (&ast.type() == &kb.type.ast.Set) {
        return kb.ast.set(instantiate(ast[id.cell]), instantiate(ast[id.role]), instantiate(ast[id.value]));
    } else if (&ast.type() == &kb.type.ast.Erase) {
        return kb.ast.erase(instantiate(ast[id.cell]), instantiate(ast[id.role]));
    } else if (&ast.type() == &kb.type.ast.If) {
        if (ast.has(id.else_)) {
            return kb.ast.if_(instantiate(ast[id.condition]), instantiate(ast[id.then]), instantiate(ast[id.else_]));
        } else {
            return kb.ast.if_(instantiate(ast[id.condition]), instantiate(ast[id.then]));
        }
    } else if (&ast.type() == &kb.type.ast.Do) {
        return kb.ast.do_(instantiate(ast[id.condition]), instantiate(ast[id.statement]));
    } else if (&ast.type() == &kb.type.ast.While) {
        return kb.ast.while_(instantiate(ast[id.condition]), instantiate(ast[id.statement]));
    } else if (&ast.type() == &kb.type.ast.New) {
        if (ast.has(id.constructor)) {
            auto* objectType = &ast[id.objectType];
            if (objectType == &kb.type.ast.DependentType) {
                // implicit instantiation here
                Scope& scope = static_cast<Scope&>(get(kb.id.scope));
                if (scope.structTs().hasKey(objectType->get(kb.id.id))) {
                    std::cout << "";
                }
            }
            Ast::Base& ret = kb.ast.new_(static_cast<Base&>(instantiate(ast[id.objectType]), instantiate(ast[id.constructor])));
            if (ast.has(id.parameters)) {
                // TODO process parameters
                ret.set(id.parameters, ast[id.parameters]);
            }
            return ret;
        } else {
            return kb.ast.new_(static_cast<Base&>(instantiate(ast[id.objectType])));
        }
    } else if (&ast.type() == &kb.type.ast.Call) {
        Ast::Base& ret = kb.ast.call(instantiate(ast[id.cell]), instantiate(ast[id.method]));
        if (ast.has(id.parameters)) {
            // TODO process parameters
            ret.set(id.parameters, ast[id.parameters]);
        }
        return ret;
    } else if (&ast.type() == &kb.type.ast.StaticCall) {
        Ast::Base& ret = kb.ast.scall(instantiate(ast[id.cell]), instantiate(ast[id.method]));
        if (ast.has(id.parameters)) {
            // TODO process parameters
            ret.set(id.parameters, ast[id.parameters]);
        }
        return ret;
    } else if (&ast.type() == &kb.type.ast.And) {
        return kb.ast.and_(instantiate(ast[id.lhs]), instantiate(ast[id.rhs]));
    } else if (&ast.type() == &kb.type.ast.Or) {
        return kb.ast.or_(instantiate(ast[id.lhs]), instantiate(ast[id.rhs]));
    } else if (&ast.type() == &kb.type.ast.Not) {
        return kb.ast.not_(instantiate(ast[id.input]));
    } else if (&ast.type() == &kb.type.ast.Add) {
        return kb.ast.add(instantiate(ast[id.lhs]), instantiate(ast[id.rhs]));
    } else if (&ast.type() == &kb.type.ast.Subtract) {
        return kb.ast.subtract(instantiate(ast[id.lhs]), instantiate(ast[id.rhs]));
    } else if (&ast.type() == &kb.type.ast.Multiply) {
        return kb.ast.multiply(instantiate(ast[id.lhs]), instantiate(ast[id.rhs]));
    } else if (&ast.type() == &kb.type.ast.Divide) {
        return kb.ast.divide(instantiate(ast[id.lhs]), instantiate(ast[id.rhs]));
    } else if (&ast.type() == &kb.type.ast.LessThan) {
        return kb.ast.lessThan(instantiate(ast[id.lhs]), instantiate(ast[id.rhs]));
    } else if (&ast.type() == &kb.type.ast.LessThanOrEqual) {
        return kb.ast.lessThanOrEqual(instantiate(ast[id.lhs]), instantiate(ast[id.rhs]));
    } else if (&ast.type() == &kb.type.ast.GreaterThan) {
        return kb.ast.greaterThan(instantiate(ast[id.lhs]), instantiate(ast[id.rhs]));
    } else if (&ast.type() == &kb.type.ast.GreaterThanOrEqual) {
        return kb.ast.greaterThanOrEqual(instantiate(ast[id.lhs]), instantiate(ast[id.rhs]));
    } else if (&ast.type() == &kb.type.ast.Same) {
        return kb.ast.same(instantiate(ast[id.lhs]), instantiate(ast[id.rhs]));
    } else if (&ast.type() == &kb.type.ast.NotSame) {
        return kb.ast.notSame(instantiate(ast[id.lhs]), instantiate(ast[id.rhs]));
    } else if (&ast.type() == &kb.type.ast.Equal) {
        return kb.ast.equal(instantiate(ast[id.lhs]), instantiate(ast[id.rhs]));
    } else if (&ast.type() == &kb.type.ast.NotEqual) {
        return kb.ast.notEqual(instantiate(ast[id.lhs]), instantiate(ast[id.rhs]));
    } else if (&ast.type() == &kb.type.ast.Has) {
        return kb.ast.has(instantiate(ast[id.cell]), instantiate(ast[id.role]));
    } else if (&ast.type() == &kb.type.ast.Missing) {
        return kb.ast.missing(instantiate(ast[id.cell]), instantiate(ast[id.role]));
    } else if (&ast.type() == &kb.type.ast.Get) {
        return kb.ast.get(instantiate(ast[id.cell]), instantiate(ast[id.role]));
    } else if (&ast.type() == &kb.type.ast.Member) {
        return kb.ast.member(ast[id.role]);
    } else if (&ast.type() == &kb.type.ast.Return) {
        if (ast.has(id.value)) {
            return kb.ast.return_(static_cast<Base&>(instantiate(ast[id.value])));
        }
        return kb.ast.return_();
    }

    throw "Unknown AST to instantiate!";
}

Map& Ast::StructT::templateParams()
{
    if (missing(kb.id.templateParams)) {
        throw "No templateParams!";
    } else {
        return static_cast<Map&>(get(kb.id.templateParams));
    }
}

Ast::Function::Function(brain::Brain& kb, CellI& name, const std::string& label) :
    BaseT<Function>(kb, kb.type.ast.Function, label)
{
    set(kb.id.name, name);
    this->label(label);
}

Ast::Function::Function(brain::Brain& kb, CellI& objType, CellI& name, const std::string& label) :
    BaseT<Function>(kb, kb.type.ast.Function, label)
{
    set(kb.id.objectType, objType);
    set(kb.id.name, name);
    this->label(label);
}

void Ast::Function::parameters(Slot& param)
{
    if (missing(kb.id.parameters)) {
        set(kb.id.parameters, *new List(kb, kb.type.Slot));
    }
    parameters().add(param);
}

void Ast::Function::returnType(CellI& type)
{
    set(kb.id.returnType, type);
}

void Ast::Function::addBlock(Block& block)
{
    set(kb.id.code, block);
}

void Ast::Function::implicitInstantiation()
{
    implicitInstantiationInAst(code());
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
    cells::Object& functionType = *new cells::Object(kb, kb.type.Type_);
    functionType.set(kb.id.memberOf, kb.map(kb.type.Type_, kb.type.Type_, kb.type.op.Function, kb.type.op.Function));
    cells::Map& subTypesMap = kb.map(kb.type.Cell, kb.type.Type_,
                                     kb.id.objectType, get(kb.id.objectType),
                                     kb.id.name, get(kb.id.name));
    functionType.set(kb.id.subTypes, subTypesMap);
    functionType.set(kb.id.slots, kb.type.op.Function[kb.id.slots]);

    cells::Object& function = *new cells::Object(kb, functionType);
    compileParams(function, subTypesMap, type);
    functionType.label(std::format("Type for {}", function.label()));
    function.set(kb.id.ast, code());
    function.set(kb.id.op, compileAst(code(), function, type));
    if (has(kb.id.static_)) {
        function.set(kb.id.static_, get(kb.id.static_));
    }

    return function;
}

void Ast::Function::compileParams(cells::Object& function, cells::Map& subTypesMap, CellI* type)
{
    std::stringstream iss;
    std::stringstream oss;
    if (has(kb.id.parameters) || type) {
        cells::Object& parametersType = *new cells::Object(kb, kb.type.Type_);
        Map& slots                    = *new Map(kb, kb.type.Cell, kb.type.Slot);
        if (type) {
            Object& var = *new Object(kb, kb.type.op.Var, "self");
            var.set(kb.id.objectType, *type);
            slots.add(kb.id.self, kb.type.slot(kb.id.self, *type));
            iss << kb.id.self.label() << ": " << (*type).label();
        }
        if (has(kb.id.parameters)) {
            Visitor::visitList(parameters(), [this, &slots, &iss](CellI& slot, int i, bool& stop) {
                if (!slots.empty()) {
                    iss << ", ";
                }
                iss << "p_" << slot[kb.id.slotRole].label() << ": " << slot[kb.id.slotType].label();
                slots.add(slot[kb.id.slotRole], kb.type.slot(slot[kb.id.slotRole], slot[kb.id.slotType]));
            });
        }
        parametersType.set(kb.id.slots, slots);
        subTypesMap.add(kb.id.parameters, parametersType);
    }
    if (has(kb.id.returnType)) {
        oss << returnType().label();
        subTypesMap.add(kb.id.returnType, returnType());
    }
    if (has(kb.id.returnType)) {
        function.label(std::format("fn {}({}) -> {}", label(), iss.str(), oss.str()));
    } else {
        function.label(std::format("fn {}({})", label(), iss.str()));
    }
}

void Ast::Function::implicitInstantiationInAst(CellI& ast)
{
    auto& id = kb.id;
    if (&ast.type() == &kb.type.ast.Block) {
        Visitor::visitList(ast[id.asts], [this](CellI& ast, int, bool&) {
            implicitInstantiationInAst(ast);
        });
    } else if (&ast.type() == &kb.type.ast.Delete) {
        implicitInstantiationInAst(ast[id.cell]);
    } else if (&ast.type() == &kb.type.ast.Set) {
        implicitInstantiationInAst(ast[id.cell]);
        implicitInstantiationInAst(ast[id.role]);
        implicitInstantiationInAst(ast[id.value]);
    } else if (&ast.type() == &kb.type.ast.Erase) {
        implicitInstantiationInAst(ast[id.cell]);
        implicitInstantiationInAst(ast[id.role]);
    } else if (&ast.type() == &kb.type.ast.If) {
        implicitInstantiationInAst(ast[id.condition]);
        implicitInstantiationInAst(ast[id.then]);
        if (ast.has(id.else_)) {
            implicitInstantiationInAst(ast[id.else_]);
        }
    } else if (&ast.type() == &kb.type.ast.Do) {
        implicitInstantiationInAst(ast[id.condition]);
        implicitInstantiationInAst(ast[id.statement]);
    } else if (&ast.type() == &kb.type.ast.While) {
        implicitInstantiationInAst(ast[id.condition]);
        implicitInstantiationInAst(ast[id.statement]);
    } else if (&ast.type() == &kb.type.ast.Set) {
        implicitInstantiationInAst(ast[id.cell]);
        implicitInstantiationInAst(ast[id.role]);
        implicitInstantiationInAst(ast[id.value]);
    } else if (&ast.type() == &kb.type.ast.New) {
        if (&ast[id.objectType].type() == &kb.type.ast.DependentType) {
            // implicit instantiation here
            auto& dependentType        = static_cast<Ast::DependentType&>(ast[id.objectType]);
            auto& templateId           = dependentType[id.id];
            auto& templateParams       = static_cast<List&>(dependentType[id.parameters]);
            Scope& scope               = static_cast<Scope&>(get(kb.id.scope));
            auto& structT              = static_cast<Ast::StructT&>(scope.structTs().getValue(templateId));
            auto& instantiatedTemplate = structT.instantiate(scope, templateParams);
            ast.set(id.objectType, instantiatedTemplate);
        }
        if (ast.has(id.parameters)) {
            // TODO process parameters
        }
    } else if (&ast.type() == &kb.type.ast.And) {
        implicitInstantiationInAst(ast[id.lhs]);
        implicitInstantiationInAst(ast[id.rhs]);
    } else if (&ast.type() == &kb.type.ast.Or) {
        implicitInstantiationInAst(ast[id.lhs]);
        implicitInstantiationInAst(ast[id.rhs]);
    } else if (&ast.type() == &kb.type.ast.Not) {
        implicitInstantiationInAst(ast[id.input]);
    } else if (&ast.type() == &kb.type.ast.Add) {
        implicitInstantiationInAst(ast[id.lhs]);
        implicitInstantiationInAst(ast[id.rhs]);
    } else if (&ast.type() == &kb.type.ast.Subtract) {
        implicitInstantiationInAst(ast[id.lhs]);
        implicitInstantiationInAst(ast[id.rhs]);
    } else if (&ast.type() == &kb.type.ast.Multiply) {
        implicitInstantiationInAst(ast[id.lhs]);
        implicitInstantiationInAst(ast[id.rhs]);
    } else if (&ast.type() == &kb.type.ast.Divide) {
        implicitInstantiationInAst(ast[id.lhs]);
        implicitInstantiationInAst(ast[id.rhs]);
    } else if (&ast.type() == &kb.type.ast.LessThan) {
        implicitInstantiationInAst(ast[id.lhs]);
        implicitInstantiationInAst(ast[id.rhs]);
    } else if (&ast.type() == &kb.type.ast.LessThanOrEqual) {
        implicitInstantiationInAst(ast[id.lhs]);
        implicitInstantiationInAst(ast[id.rhs]);
    } else if (&ast.type() == &kb.type.ast.GreaterThan) {
        implicitInstantiationInAst(ast[id.lhs]);
        implicitInstantiationInAst(ast[id.rhs]);
    } else if (&ast.type() == &kb.type.ast.GreaterThanOrEqual) {
        implicitInstantiationInAst(ast[id.lhs]);
        implicitInstantiationInAst(ast[id.rhs]);
    } else if (&ast.type() == &kb.type.ast.Same) {
        implicitInstantiationInAst(ast[id.lhs]);
        implicitInstantiationInAst(ast[id.rhs]);
    } else if (&ast.type() == &kb.type.ast.NotSame) {
        implicitInstantiationInAst(ast[id.lhs]);
        implicitInstantiationInAst(ast[id.rhs]);
    } else if (&ast.type() == &kb.type.ast.Equal) {
        implicitInstantiationInAst(ast[id.lhs]);
        implicitInstantiationInAst(ast[id.rhs]);
    } else if (&ast.type() == &kb.type.ast.NotEqual) {
        implicitInstantiationInAst(ast[id.lhs]);
        implicitInstantiationInAst(ast[id.rhs]);
    } else if (&ast.type() == &kb.type.ast.Has) {
        implicitInstantiationInAst(ast[id.cell]);
        implicitInstantiationInAst(ast[id.role]);
    } else if (&ast.type() == &kb.type.ast.Missing) {
        implicitInstantiationInAst(ast[id.cell]);
        implicitInstantiationInAst(ast[id.role]);
    } else if (&ast.type() == &kb.type.ast.Get) {
        implicitInstantiationInAst(ast[id.cell]);
        implicitInstantiationInAst(ast[id.role]);
    } else if (&ast.type() == &kb.type.ast.Return) {
        if (ast.has(id.value)) {
            implicitInstantiationInAst(ast[id.value]);
        }
    }
}

CellI& Ast::Function::compileAst(CellI& ast, cells::Object& function, CellI* type)
{
    auto compile = [this, &function, type](CellI& ast) -> CellI& { return compileAst(ast, function, type); };
    const auto _ = [this](CellI& cell) -> Ast::Cell& { return kb.ast.cell(cell); };
    auto& id = kb.id;

    if (&ast.type() == &kb.type.ast.Block) {
        CellI& list        = ast[id.asts];
        auto& compiledAsts = *new cells::List(kb, kb.type.op.Base);
        Visitor::visitList(list, [this, &compiledAsts, &ast, &function, type](CellI& ast, int, bool&) {
            compiledAsts.add(compileAst(ast, function, type));
        });
        Object& opBlock = *new Object(kb, kb.type.op.Block);
        opBlock.set(id.ast, ast);
        opBlock.set(id.ops, compiledAsts);
        return opBlock;
    } else if (&ast.type() == &kb.type.ast.Cell) {
        Object& constVar = *new Object(kb, kb.type.op.ConstVar);
        constVar.set(id.ast, ast);
        constVar.set(id.value, ast[id.value]);
        return constVar;
    } else if (&ast.type() == &kb.type.ast.SelfFn) {
        Object& constVar = *new Object(kb, kb.type.op.ConstVar);
        constVar.set(id.ast, ast);
        constVar.set(id.value, function);
        return constVar;
    } else if (&ast.type() == &kb.type.ast.Self) {
        CellI& retOp = compile(kb.ast.get(_(function), _(id.stack)) / _(id.value) / _(id.input) / _(id.self));
        retOp.set(id.ast, ast);
        retOp.label("self");
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Parameter) {
        CellI& retOp = compile(kb.ast.get(_(function), _(id.stack)) / _(id.value) / _(id.input) / _(ast[id.role]));
        retOp.set(id.ast, ast);
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Var) {
        if (function.type()[id.subTypes][id.index].missing(id.localVars)) {
            cells::Object& functionLocalVarsType = *new cells::Object(kb, kb.type.Type_, std::format("LocalVarsType of {}", function.label()));
            functionLocalVarsType.set(id.memberOf, kb.map(kb.type.Type_, kb.type.Type_, kb.type.Index, kb.type.Index));
            static_cast<Map&>(function.type()[id.subTypes]).add(id.localVars, functionLocalVarsType);
        }
        CellI& localVarsType = function.type()[id.subTypes][id.index][id.localVars][id.value];
        if (localVarsType.missing(id.slots)) {
            localVarsType.set(id.slots, *new Map(kb, kb.type.Cell, kb.type.Slot));
        }
        auto& slotsMap = static_cast<Map&>(localVarsType[id.slots]);
        if (!slotsMap.hasKey(ast[id.role])) {
            slotsMap.add(ast[id.role], kb.type.slot(ast[id.role], kb.type.op.Var));
        }
        CellI& retOp = compile(kb.ast.get(_(function), _(id.stack)) / _(id.value) / _(id.localVars) / _(ast[id.role]));
        retOp.set(id.ast, ast);
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Delete) {
        Object& retOp = *new Object(kb, kb.type.op.Delete);
        retOp.set(id.ast, ast);
        retOp.set(id.input, compile(ast[id.cell]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Set) {
        Object& retOp = *new Object(kb, kb.type.op.Set);
        retOp.set(id.ast, ast);
        retOp.set(id.cell, compile(ast[id.cell]));
        retOp.set(id.role, compile(ast[id.role]));
        retOp.set(id.value, compile(ast[id.value]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Erase) {
        Object& retOp = *new Object(kb, kb.type.op.Erase);
        retOp.set(id.ast, ast);
        retOp.set(id.cell, compile(ast[id.cell]));
        retOp.set(id.role, compile(ast[id.role]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.If) {
        Object& retOp = *new Object(kb, kb.type.op.If);
        retOp.set(id.ast, ast);
        retOp.set(id.condition, compile(ast[id.condition]));
        retOp.set(id.then, compile(ast[id.then]));
        if (ast.has(id.else_)) {
            retOp.set(id.else_, compile(ast[id.else_]));
            return retOp;
        } else {
            return retOp;
        }
    } else if (&ast.type() == &kb.type.ast.Do) {
        Object& retOp = *new Object(kb, kb.type.op.Do);
        retOp.set(id.ast, ast);
        retOp.set(id.condition, compile(ast[id.condition]));
        retOp.set(id.statement, compile(ast[id.statement]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.While) {
        Object& retOp = *new Object(kb, kb.type.op.While);
        retOp.set(id.ast, ast);
        retOp.set(id.condition, compile(ast[id.condition]));
        retOp.set(id.statement, compile(ast[id.statement]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.New) {
        auto& compiledAsts = *new cells::List(kb, kb.type.op.Base);

        Object& block = *new Object(kb, kb.type.op.Block);
        block.set(id.ast, ast);
        block.set(id.ops, compiledAsts);
        Object& opSet = *new Object(kb, kb.type.op.Set, "block.value = new objectType()");
        opSet.set(id.ast, ast);
        opSet.set(id.cell, compile(kb.ast.cell(block)));
        opSet.set(id.role, compile(kb.ast.cell(id.value)));
        Object& opNew = *new Object(kb, kb.type.op.New);
        opNew.set(id.ast, ast);
        opNew.set(id.objectType, compile(ast[id.objectType]));
        opSet.set(id.value, opNew);

        compiledAsts.add(opSet);
        if (ast.has(id.constructor)) {
            Object& callAst = *new Object(kb, kb.type.ast.Call);
            callAst.set(id.cell, kb.ast.get(kb.ast.cell(block), kb.ast.cell(id.value)));
            callAst.set(id.method, ast[id.constructor]);
            if (ast.has(id.parameters)) {
                callAst.set(id.parameters, ast[id.parameters]);
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
        Ast::Base& astCell     = static_cast<Ast::Base&>(ast[id.cell]);
        Ast::Base& astMethod   = static_cast<Ast::Base&>(ast[id.method]);
        Ast::Get* getMethodPtr = nullptr;
        if (&ast.type() == &kb.type.ast.Call) {
            getMethodPtr = &(kb.ast.get(astCell, _(id.type)) / _(id.methods));
        } else {
            getMethodPtr = &(kb.ast.get(astCell, _(id.methods)));
        }
        Ast::Get& getMethod = (*getMethodPtr) / _(id.index) / astMethod / _(id.value);
        auto& compiledAsts  = *new cells::List(kb, kb.type.op.Base);
        Object& block       = *new Object(kb, kb.type.op.Block, blockName);
        block.set(id.ast, ast);
        block.set(id.ops, compiledAsts);
        Object& varMethod = *new Object(kb, kb.type.op.Var, "Call { var_method; }");
        varMethod.set(id.objectType, kb.type.op.Function);
        CellI& storeMethod     = compile(kb.ast.set(_(varMethod), _(id.value), getMethod));
        CellI& setCurrentStack = compile(kb.ast.set(_(varMethod) / _(id.value), _(id.stack), kb.ast.get(_(function), _(id.stack))));

        Object& varNewStackItem = *new Object(kb, kb.type.op.Var, "Call { var_newStackItem; }");
        varNewStackItem.set(id.objectType, kb.type.ListItem);
        Object& varNewStackFrame = *new Object(kb, kb.type.op.Var, "Call { var_newStackFrame; }");
        varNewStackFrame.set(id.objectType, kb.type.StackFrame);

        Object& varInputIndex = *new Object(kb, kb.type.Index, "Call { var_inputIndex; }");
        Object& varLocalVars = *new Object(kb, kb.type.op.Var, "Call { var_localVars; }");
        varLocalVars.set(id.objectType, kb.type.Index);
        Object& varLocalVarsList = *new Object(kb, kb.type.op.Var, "Call { var_localVarsList; }");
        varLocalVarsList.set(id.objectType, kb.type.List);
        Object& varLocalVarsListItem = *new Object(kb, kb.type.op.Var, "Call { var_localVarsListItem; }");
        varLocalVarsListItem.set(id.objectType, kb.type.ListItem);
        Object& varLocalVarsListItemHasNext = *new Object(kb, kb.type.op.Var, "Call { var_localVarsListItemHasNext; }");
        varLocalVarsListItemHasNext.set(id.objectType, kb.type.Boolean);

        CellI& storeStackItem     = compile(kb.ast.set(_(varNewStackItem), _(id.value), kb.ast.new_(_(kb.type.ListItem))));
        CellI& storeStackFrame    = compile(kb.ast.set(_(varNewStackFrame), _(id.value), kb.ast.new_(_(kb.type.StackFrame))));
        CellI& setListItem        = compile(kb.ast.set(_(varNewStackItem) / _(id.value), _(id.value), _(varNewStackFrame) / _(id.value)));
        CellI& setListItemPrev    = compile(kb.ast.set(_(varNewStackItem) / _(id.value), _(id.previous), _(varMethod) / _(id.value) / _(id.stack)));
        CellI& setListItemNext    = compile(kb.ast.set(_(varMethod) / _(id.value) / _(id.stack), _(id.next), _(varNewStackItem) / _(id.value)));
        CellI& storeInputIndex    = compile(kb.ast.set(_(varInputIndex), _(id.value), kb.ast.new_(_(kb.type.Index))));
        CellI& setMethod          = compile(kb.ast.set(_(varNewStackFrame) / _(id.value), _(id.method), _(varMethod) / _(id.value)));

        CellI& createNewLocalVars = compile(
            kb.ast.if_(kb.ast.has(_(varMethod) / _(id.value) / _(id.type) / _(id.subTypes) / _(id.index), _(id.localVars)),
                       kb.ast.block(
                           kb.ast.set(_(varLocalVars), _(id.value), kb.ast.new_(_(varMethod) / _(id.value) / _(id.type) / _(id.subTypes) / _(id.index) / _(id.localVars) / _(id.value))),
                           kb.ast.set(_(varNewStackFrame) / _(id.value), _(id.localVars), _(varLocalVars) / _(id.value)),
                           kb.ast.set(_(varLocalVarsListItem), _(id.value), _(varMethod) / _(id.value) / _(id.type) / _(id.subTypes) / _(id.index) / _(id.localVars) / _(id.value) / _(id.slots) / _(id.list) / _(id.first)),
                           kb.ast.do_(kb.ast.block(
                                          kb.ast.set(_(varLocalVarsListItemHasNext), _(id.value), _(kb.boolean.true_)),
                                          kb.ast.set(_(varLocalVars) / _(id.value), _(varLocalVarsListItem) / _(id.value) / _(id.value) / _(id.slotRole), kb.ast.new_(_(kb.type.op.Var))),
                                          kb.ast.if_(kb.ast.has(_(varLocalVarsListItem) / _(id.value), _(id.next)),
                                                     kb.ast.set(_(varLocalVarsListItem), _(id.value), _(varLocalVarsListItem) / _(id.value) / _(id.next)),
                                                     kb.ast.set(_(varLocalVarsListItemHasNext), _(id.value), _(kb.boolean.false_)))),
                                      kb.ast.same(_(varLocalVarsListItemHasNext) / _(id.value), _(kb.boolean.true_))))));

        CellI& setInput      = compile(kb.ast.set(_(varNewStackFrame) / _(id.value), _(id.input), _(varInputIndex) / _(id.value)));
        CellI& setSelf       = compile(kb.ast.set(_(varInputIndex) / _(id.value), _(id.self), astCell));
        CellI& setStackToNew = compile(kb.ast.set(_(varMethod) / _(id.value), _(id.stack), _(varNewStackItem) / _(id.value)));
        CellI& setRetValue   = compile(kb.ast.if_(kb.ast.has(_(varMethod) / _(id.value) / _(id.type) / _(id.subTypes) / _(id.index), _(id.returnType)),
                                                  kb.ast.block(kb.ast.set(_(varNewStackFrame) / _(id.value), _(id.output), kb.ast.new_(_(kb.type.op.Var))),
                                                               kb.ast.set(_(varNewStackFrame) / _(id.value) / _(id.output), _(id.objectType), _(varMethod) / _(id.value) / _(id.type) / _(id.subTypes) / _(id.index) / _(id.returnType) / _(id.value)))));
        CellI& getResult     = compile(kb.ast.if_(kb.ast.has(_(varMethod) / _(id.value) / _(id.type) / _(id.subTypes) / _(id.index), _(id.returnType)),
                                                  kb.ast.set(_(block), _(id.value), _(varMethod) / _(id.value) / _(id.stack) / _(id.value) / _(id.output) / _(id.value))));
        CellI& setStackToOld = compile(kb.ast.set(_(varMethod) / _(id.value) / _(id.stack) / _(kb.id.previous) / _(id.value) / _(id.method), _(id.stack), _(varMethod) / _(id.value) / _(id.stack) / _(id.previous)));

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


        if (ast.has(id.parameters)) {
            Visitor::visitList(ast[id.parameters], [this, &compiledAsts, &compile, &varInputIndex, &_, &id](CellI& param, int, bool&) {
                Ast::Base& paramRole  = static_cast<Ast::Base&>(param[id.slotRole]);
                Ast::Base& paramValue = static_cast<Ast::Base&>(param[id.slotType]);
                CellI& setParam       = compile(kb.ast.set(_(varInputIndex) / _(id.value), paramRole, paramValue));
                setParam.label("Call { setParam; }");
                compiledAsts.add(setParam);
            });
        }
        CellI& evalMethod = *new Object(kb, kb.type.op.EvalVar, std::format("{}::Call {{ evalVar; }}", function.label()));
        evalMethod.set(id.value, varMethod);
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
        retOp.set(id.ast, ast);
        retOp.set(id.lhs, compile(ast[id.lhs]));
        retOp.set(id.rhs, compile(ast[id.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Or) {
        Object& retOp = *new Object(kb, kb.type.op.Or);
        retOp.set(id.ast, ast);
        retOp.set(id.lhs, compile(ast[id.lhs]));
        retOp.set(id.rhs, compile(ast[id.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Not) {
        Object& retOp = *new Object(kb, kb.type.op.Not);
        retOp.set(id.ast, ast);
        retOp.set(id.input, compile(ast[id.input]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Add) {
        Object& retOp = *new Object(kb, kb.type.op.Add);
        retOp.set(id.ast, ast);
        retOp.set(id.lhs, compile(ast[id.lhs]));
        retOp.set(id.rhs, compile(ast[id.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Subtract) {
        Object& retOp = *new Object(kb, kb.type.op.Subtract);
        retOp.set(id.ast, ast);
        retOp.set(id.lhs, compile(ast[id.lhs]));
        retOp.set(id.rhs, compile(ast[id.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Multiply) {
        Object& retOp = *new Object(kb, kb.type.op.Multiply);
        retOp.set(id.ast, ast);
        retOp.set(id.lhs, compile(ast[id.lhs]));
        retOp.set(id.rhs, compile(ast[id.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Divide) {
        Object& retOp = *new Object(kb, kb.type.op.Divide);
        retOp.set(id.ast, ast);
        retOp.set(id.lhs, compile(ast[id.lhs]));
        retOp.set(id.rhs, compile(ast[id.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.LessThan) {
        Object& retOp = *new Object(kb, kb.type.op.LessThan);
        retOp.set(id.ast, ast);
        retOp.set(id.lhs, compile(ast[id.lhs]));
        retOp.set(id.rhs, compile(ast[id.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.LessThanOrEqual) {
        Object& retOp = *new Object(kb, kb.type.op.LessThanOrEqual);
        retOp.set(id.ast, ast);
        retOp.set(id.lhs, compile(ast[id.lhs]));
        retOp.set(id.rhs, compile(ast[id.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.GreaterThan) {
        Object& retOp = *new Object(kb, kb.type.op.GreaterThan);
        retOp.set(id.ast, ast);
        retOp.set(id.lhs, compile(ast[id.lhs]));
        retOp.set(id.rhs, compile(ast[id.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.GreaterThanOrEqual) {
        Object& retOp = *new Object(kb, kb.type.op.GreaterThanOrEqual);
        retOp.set(id.ast, ast);
        retOp.set(id.lhs, compile(ast[id.lhs]));
        retOp.set(id.rhs, compile(ast[id.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Same) {
        Object& retOp = *new Object(kb, kb.type.op.Same);
        retOp.set(id.ast, ast);
        retOp.set(id.lhs, compile(ast[id.lhs]));
        retOp.set(id.rhs, compile(ast[id.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.NotSame) {
        Object& retOp = *new Object(kb, kb.type.op.NotSame);
        retOp.set(id.ast, ast);
        retOp.set(id.lhs, compile(ast[id.lhs]));
        retOp.set(id.rhs, compile(ast[id.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Equal) {
        Object& retOp = *new Object(kb, kb.type.op.Equal);
        retOp.set(id.ast, ast);
        retOp.set(id.lhs, compile(ast[id.lhs]));
        retOp.set(id.rhs, compile(ast[id.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.NotEqual) {
        Object& retOp = *new Object(kb, kb.type.op.NotEqual);
        retOp.set(id.ast, ast);
        retOp.set(id.lhs, compile(ast[id.lhs]));
        retOp.set(id.rhs, compile(ast[id.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Has) {
        Object& retOp = *new Object(kb, kb.type.op.Has);
        retOp.set(id.ast, ast);
        retOp.set(id.cell, compile(ast[id.cell]));
        retOp.set(id.role, compile(ast[id.role]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Missing) {
        Object& retOp = *new Object(kb, kb.type.op.Missing);
        retOp.set(id.ast, ast);
        retOp.set(id.cell, compile(ast[id.cell]));
        retOp.set(id.role, compile(ast[id.role]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Get) {
        Object& retOp = *new Object(kb, kb.type.op.Get);
        retOp.set(id.ast, ast);
        retOp.set(id.cell, compile(ast[id.cell]));
        retOp.set(id.role, compile(ast[id.role]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Member) {
        CellI& member = compile(kb.ast.get(kb.ast.self(), kb.ast.cell(ast[id.role])));
        member.set(id.ast, ast);
        return member;
    } else if (&ast.type() == &kb.type.ast.Return) {
        Object& retOp = *new Object(kb, kb.type.op.Return, "op.return");
        retOp.set(id.ast, ast);
        if (ast.has(id.value)) {
            retOp.set(id.result, compile(kb.ast.set(kb.ast.get(_(function), _(id.stack)) / _(id.value) / _(id.output), _(id.value), static_cast<Ast::Base&>(ast[id.value]))));
        }
        return retOp;
    }

    throw "Unknown function AST!";
}

List& Ast::Function::parameters()
{
    if (missing(kb.id.parameters)) {
        throw "No parameters!";
    } else {
        return static_cast<List&>(get(kb.id.parameters));
    }
}

CellI& Ast::Function::returnType()
{
    if (missing(kb.id.returnType)) {
        throw "No returnType!";
    } else {
        return get(kb.id.returnType);
    }
}

Ast::Base& Ast::Function::code()
{
    if (missing(kb.id.code)) {
        throw "No code!";
    } else {
        return static_cast<Ast::Base&>(get(kb.id.code));
    }
}

Ast::FunctionT::FunctionT(brain::Brain& kb, CellI& name, const std::string& label) :
    BaseT<FunctionT>(kb, kb.type.ast.FunctionT, label)
{
    set(kb.id.name, name);
    this->label(label);
}

void Ast::FunctionT::parameters(Slot& param)
{
    if (missing(kb.id.parameters)) {
        set(kb.id.parameters, *new List(kb, kb.type.Slot));
    }
    parameters().add(param);
}

void Ast::FunctionT::returnType(CellI& type)
{
    set(kb.id.returnType, type);
}

void Ast::FunctionT::addBlock(Block& block)
{
    set(kb.id.code, block);
}

List& Ast::FunctionT::parameters()
{
    if (missing(kb.id.parameters)) {
        throw "No parameters!";
    } else {
        return static_cast<List&>(get(kb.id.parameters));
    }
}

CellI& Ast::FunctionT::returnType()
{
    if (missing(kb.id.returnType)) {
        throw "No returnType!";
    } else {
        return get(kb.id.returnType);
    }
}

Ast::Base& Ast::FunctionT::code()
{
    if (missing(kb.id.code)) {
        throw "No code!";
    } else {
        return static_cast<Ast::Base&>(get(kb.id.code));
    }
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

Ast::DependentType::DependentType(brain::Brain& kb, CellI& id, CellI& typeList) :
    BaseT<DependentType>(kb, kb.type.ast.DependentType, "ast.dependentType")
{
    set(kb.id.id, id);
    set(kb.id.parameters, typeList);
}

Ast::TemplateParam::TemplateParam(brain::Brain& kb, CellI& role) :
    BaseT<TemplateParam>(kb, kb.type.ast.TemplateParam, "ast.templateParam")
{
    set(kb.id.role, role);
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

Ast::Parameter& Ast::parameter(CellI& role)
{
    auto& ast = kb.ast;
    return Parameter::New(kb, role);
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
Ast::DependentType& Ast::dependentType(CellI& role, CellI& type)
{
    return DependentType::New(kb, role, kb.list(type));
}
Ast::DependentType& Ast::dependentType(CellI& role, CellI& type1, CellI& type2)
{
    return DependentType::New(kb, role, kb.list(type1, type2));
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
    const auto _      = [this](CellI& cell) -> Ast::Cell& { return ast.cell(cell); };
    const auto p_     = [this](CellI& role) -> Ast::Parameter& { return ast.parameter(role); };
    const auto m_     = [this](CellI& role) -> Ast::Member& { return ast.member(role); };
    const auto var_   = [this](CellI& role) -> Ast::Var& { return ast.var(role); };
    const auto param  = [this](CellI& role, CellI& value) -> Ast::Slot& { return ast.slot(role, value); };
    const auto member = [this](CellI& role, CellI& value) -> Ast::Slot& { return ast.slot(role, value); };

    CellI* mapPtr = nullptr;

#if 1
#pragma region Struct test
    const auto t_  = [this](CellI& role) -> Ast::TemplateParam& { return ast.templateParam(role); };
    const auto dt_ = [this](CellI& role, auto&... param) -> Ast::DependentType& { return ast.dependentType(role, param...); };

    auto& globalScope = *new Ast::Scope(*this, id.global, "global");
    auto& listItemStructT = globalScope.addStructT(id.listItem, "ListItem<objectType:Type>");
    listItemStructT.templateParams(
        param(id.objectType, type.Type_));
    listItemStructT.members(
        member(id.previous, t_(id.self)),
        member(id.next, t_(id.self)),
        member(id.value, t_(id.objectType)));

    auto& listItemConstructor = listItemStructT.addMethod(id.constructor, "ListItem::constructor");
    listItemConstructor.parameters(
        param(id.value, t_(id.objectType)));
    listItemConstructor.code(
        m_(id.value) = p_(id.value));

    auto& listItemNumberStruct = globalScope.instantiateStructT(id.listItem, param(id.objectType, type.Number));

    auto& listStructT = globalScope.addStructT(id.list, "List<objectType:Type>");
    listStructT.templateParams(
        param(id.objectType, type.Type_));
    listStructT.memberOf(
        _(type.Container));
    listStructT.subTypes(
        param(id.itemType, dt_(id.listItem, param(id.objectType, t_(id.objectType)))),
        param(id.objectType, t_(id.objectType)));
    listStructT.members(
        member(id.itemType, dt_(id.listItem, param(id.objectType, t_(id.objectType)))),
        member(id.objectType, t_(id.objectType)),
        member(id.first, dt_(id.listItem, param(id.objectType, t_(id.objectType)))),
        member(id.last, dt_(id.listItem, param(id.objectType, t_(id.objectType)))),
        member(id.size, _(type.Number)));

    auto& listNumberStruct = globalScope.instantiateStructT(id.list, param(id.objectType, type.Number)); // explicit instantiation

    auto& testCreateNewListOfNumbers = globalScope.addFunction(id.list, "testCreateNewListOfNumbers");
    testCreateNewListOfNumbers.code(
        var_(id.result) = ast.new_(dt_(id.list, param(id.objectType, type.Number)))); // implicit instantiation

    globalScope.instantiateStructT(id.list, param(id.objectType, type.Number));

    auto& compiledGlobalScope = globalScope.compile();

#pragma endregion
#endif

#pragma region ListItem
    mapPtr = &slots(type.slot(id.previous, type.ListItem),
                    type.slot(id.next, type.ListItem),
                    type.slot(id.value, type.Cell));
    type.ListItem.set(id.slots, *mapPtr);
    type.ListItem.set(id.memberOf, map(type.Type_, type.Type_, type.Iterator, type.Iterator));

    Ast::Function& listItemTemplate = *new Ast::Function(*this, type.ListItem, id.template_, "static ListItem::template");
    listItemTemplate.set(id.static_, boolean.true_);
    listItemTemplate.parameters(
        param(id.objectType, type.Type_));
    listItemTemplate.returnType(type.ListItem);
    listItemTemplate.code(
        var_(id.result) = ast.new_(_(type.Type_)),

        ast.call(*var_(id.result), _(id.addMembership), param(_(id.cell), _(type.ListItem))),

        ast.call(*var_(id.result), _(id.addSlot), param(_(id.slotRole), _(id.previous)), param(_(id.slotType), *var_(id.result))),
        ast.call(*var_(id.result), _(id.addSlot), param(_(id.slotRole), _(id.next)), param(_(id.slotType), *var_(id.result))),
        ast.call(*var_(id.result), _(id.addSlot), param(_(id.slotRole), _(id.value)), param(_(id.slotType), p_(id.objectType))),

        ast.set(*var_(id.result), _(id.methods), m_(id.methods)),

        ast.return_(*var_(id.result)));

    Ast::Function& listItemCtor = *new Ast::Function(*this, type.ListItem, id.constructor, "ListItem::constructor");
    listItemCtor.parameters(
        param(id.value, type.Cell));
    listItemCtor.code(
        m_(id.value) = p_(id.value));

    registerMethods(type.ListItem,
                    id.template_, listItemTemplate,
                    id.constructor, listItemCtor);

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

    type.ListOf(type.Cell)[id.subTypes][id.index][id.itemType][id.value].set(id.methods, type.ListItem[id.methods]);
    type.ListOf(type.Slot)[id.subTypes][id.index][id.itemType][id.value].set(id.methods, type.ListItem[id.methods]);
    type.ListOf(type.Type_)[id.subTypes][id.index][id.itemType][id.value].set(id.methods, type.ListItem[id.methods]);

    // Pre created:
    // Slot
    Ast::Function& listTemplate = *new Ast::Function(*this, type.List, id.template_, "static List::template");
    listTemplate.set(id.static_, boolean.true_);
    listTemplate.parameters(
        param(id.objectType, type.Type_));
    listTemplate.returnType(type.List);
    listTemplate.code(
        var_(type.ListItem) = ast.scall(_(type.ListItem), _(id.template_), param(_(id.objectType), p_(id.objectType))),
        var_(id.result)     = ast.new_(_(type.Type_)),

        ast.call(*var_(id.result), _(id.addSubType), param(_(id.slotRole), _(id.itemType)), param(_(id.slotType), *var_(type.ListItem))),
        ast.call(*var_(id.result), _(id.addSubType), param(_(id.slotRole), _(id.objectType)), param(_(id.slotType), p_(id.objectType))),
        ast.call(*var_(id.result), _(id.addMembership), param(_(id.cell), _(type.List))),

        ast.call(*var_(id.result), _(id.addSlot), param(_(id.slotRole), _(id.first)), param(_(id.slotType), *var_(type.ListItem))),
        ast.call(*var_(id.result), _(id.addSlot), param(_(id.slotRole), _(id.last)), param(_(id.slotType), *var_(type.ListItem))),
        ast.call(*var_(id.result), _(id.addSlot), param(_(id.slotRole), _(id.size)), param(_(id.slotType), _(type.Number))),
        ast.call(*var_(id.result), _(id.addSlot), param(_(id.slotRole), _(id.itemType)), param(_(id.slotType), *var_(type.ListItem))),
        ast.call(*var_(id.result), _(id.addSlot), param(_(id.slotRole), _(id.objectType)), param(_(id.slotType), p_(id.objectType))),

        ast.set(*var_(id.result), _(id.methods), m_(id.methods)),

        ast.return_(*var_(id.result)));

    Ast::Function& listCtor = *new Ast::Function(*this, type.List, id.constructor, "List::constructor");
    listCtor.code(
        m_(id.size)       = _(_0_),
        m_(id.objectType) = m_(id.type) / _(id.subTypes) / _(id.index) / _(id.objectType) / _(id.value),
        m_(id.itemType)   = m_(id.type) / _(id.subTypes) / _(id.index) / _(id.itemType) / _(id.value));

    Ast::Function& listAdd = *new Ast::Function(*this, type.List, id.add, "List::add");
    listAdd.parameters(
        param(id.value, type.Cell));
    listAdd.returnType(type.ListItem);
    listAdd.code(
        var_(id.item) = ast.new_(m_(id.itemType), _(id.constructor), param(_(id.value), p_(id.value))),
        ast.if_(ast.not_(m_(id.first).exist()),
                m_(id.first) = *var_(id.item),                              // then
                ast.block(ast.set(m_(id.last), _(id.next), *var_(id.item)), // else
                          ast.set(*var_(id.item), _(id.previous), m_(id.last)))),
        m_(id.last) = *var_(id.item),
        m_(id.size) = ast.add(m_(id.size), _(_1_)),
        ast.return_(*var_(id.item)));
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
    listRemove.parameters(
        param(id.item, type.Cell));
    listRemove.code(
        ast.if_(ast.has(p_(id.item), _(id.previous)),
                ast.if_(ast.has(p_(id.item), _(id.next)),
                        ast.set(p_(id.item) / _(id.previous), _(id.next), p_(id.item) / _(id.next)),
                        ast.erase(p_(id.item) / _(id.previous), _(id.next))),
                ast.if_(ast.has(p_(id.item), _(id.next)),
                        m_(id.first) = p_(id.item) / _(id.next),
                        ast.erase(ast.self(), _(id.first)))),
        ast.if_(ast.has(p_(id.item), _(id.next)),
                ast.if_(ast.has(p_(id.item), _(id.previous)),
                        ast.set(p_(id.item) / _(id.next), _(id.previous), p_(id.item) / _(id.previous)),
                        ast.erase(p_(id.item) / _(id.next), _(id.previous))),
                ast.if_(ast.has(p_(id.item), _(id.previous)),
                        m_(id.last) = p_(id.item) / _(id.previous),
                        ast.erase(ast.self(), _(id.last)))),
        m_(id.size) = ast.subtract(m_(id.size), _(_1_)));

    Ast::Function& listSize = *new Ast::Function(*this, type.List, id.size, "List::size");
    listSize.returnType(type.Number);
    listSize.code(
        ast.return_(m_(id.size)));

    Ast::Function& listEmpty = *new Ast::Function(*this, type.List, id.empty, "List::empty");
    listEmpty.returnType(type.Boolean);
    listEmpty.code(
        ast.if_(ast.equal(m_(id.size), _(_0_)), ast.return_(_(boolean.true_)), ast.return_(_(boolean.false_))));

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
    typeCtor.code(
        m_(id.slots) = ast.new_(_(type.MapCellToSlot), _(id.constructor)));

    Ast::Function& typeCtorWithRecursiveType = *new Ast::Function(*this, type.Type_, id.constructorWithRecursiveType, "Type::constructorWithRecursiveType");
    typeCtorWithRecursiveType.code(
        m_(id.slots) = ast.new_(_(type.MapCellToSlot), _(id.constructorWithIndexType), param(_(id.indexType), ast.self())));

    Ast::Function& typeAddSubType = *new Ast::Function(*this, type.Type_, id.addSubType, "Type::addSubType");
    typeAddSubType.parameters(
        param(id.slotRole, type.Cell),
        param(id.slotType, type.Type_));
    typeAddSubType.code(
        ast.if_(m_(id.subTypes).missing(), m_(id.subTypes) = ast.new_(_(type.MapCellToType), _(id.constructor))),
        ast.call(m_(id.subTypes), _(id.add), param(_(id.key), p_(id.slotRole)), param(_(id.value), p_(id.slotType))));

    Ast::Function& typeAddMembership = *new Ast::Function(*this, type.Type_, id.addMembership, "Type::addMembership");
    typeAddMembership.parameters(
        param(id.cell, type.Type_));
    typeAddMembership.code(
        ast.if_(m_(id.memberOf).missing(), m_(id.memberOf) = ast.new_(_(type.MapTypeToType), _(id.constructor))),
        ast.call(m_(id.memberOf), _(id.add), param(_(id.key), p_(id.cell)), param(_(id.value), p_(id.cell))));

    Ast::Function& typeAddSlot = *new Ast::Function(*this, type.Type_, id.addSlot, "Type::addSlot");
    typeAddSlot.parameters(
        param(id.slotRole, type.Cell),
        param(id.slotType, type.Slot));
    typeAddSlot.code(
        ast.if_(m_(id.slots).missing(), m_(id.slots) = ast.new_(_(type.MapCellToSlot), _(id.constructor))),
        var_(id.slot) = ast.new_(_(type.Slot)),
        ast.set(*var_(id.slot), _(id.slotRole), p_(id.slotRole)),
        ast.set(*var_(id.slot), _(id.slotType), p_(id.slotType)),
        ast.call(m_(id.slots), _(id.add), param(_(id.key), p_(id.slotRole)), param(_(id.value), *var_(id.slot))));

    Ast::Function& typeAddSlots = *new Ast::Function(*this, type.Type_, id.addSlots, "Type::addSlots");
    typeAddSlots.parameters(
        param(id.list, type.ListOf(type.Slot)));
    typeAddSlots.code(
        ast.if_(ast.equal(p_(id.list) / _(id.size), _(_0_)), ast.return_()),
        var_(id.item) = p_(id.list) / _(id.first),
        ast.if_(m_(id.slots).missing(), m_(id.slots) = ast.new_(_(type.MapCellToSlot), _(id.constructor))),
        ast.do_(ast.block(
                    var_(id.next) = _(boolean.true_),
                    ast.call(m_(id.slots), _(id.add), param(_(id.key), *var_(id.item) / _(id.value) / _(id.slotRole)), param(_(id.value), *var_(id.item) / _(id.value))),
                    ast.if_(ast.has(*var_(id.item), _(id.next)),
                            var_(id.item) = *var_(id.item) / _(id.next),
                            var_(id.next) = _(boolean.false_))),
                ast.same(*var_(id.next), _(boolean.true_))));

    Ast::Function& typeHasSlot = *new Ast::Function(*this, type.Type_, id.hasSlot, "Type::hasSlot");
    typeHasSlot.parameters(
        param(id.slotRole, type.Cell));
    typeHasSlot.returnType(type.Boolean);
    typeHasSlot.code(
        ast.if_(m_(id.slots).missing(), ast.return_(_(boolean.false_))),
        ast.return_(ast.call(m_(id.slots), _(id.hasKey), param(_(id.key), p_(id.slotRole)))));

    Ast::Function& typeRemoveSlot = *new Ast::Function(*this, type.Type_, id.removeSlot, "Type::removeSlot");
    typeRemoveSlot.parameters(
        param(id.slotRole, type.Cell));
    typeRemoveSlot.code(
        ast.if_(m_(id.slots).missing(), ast.return_()),
        ast.call(m_(id.slots), _(id.remove), param(_(id.key), p_(id.slotRole))));

    registerMethods(type.Type_,
                    id.constructor, typeCtor,
                    id.constructorWithRecursiveType, typeCtorWithRecursiveType,
                    id.addSubType, typeAddSubType,
                    id.addMembership, typeAddMembership,
                    id.addSlot, typeAddSlot,
                    id.addSlots, typeAddSlots,
                    id.hasSlot, typeHasSlot,
                    id.removeSlot, typeRemoveSlot);
#pragma endregion
#pragma region Index
    type.Index.set(id.memberOf, map(type.Type_, type.Type_));

    Ast::Function& indexCtor = *new Ast::Function(*this, type.Index, id.constructor, "Index::constructor");
    indexCtor.code(
        ast.set(ast.self(), _(id.type), ast.new_(_(type.Type_), _(id.constructorWithRecursiveType))),
        ast.set(m_(id.type), _(id.methods), _(type.Index) / _(id.methods)),
        ast.set(m_(id.type), _(id.memberOf), _(map(type.Type_, type.Type_, type.Index, type.Index))));

    Ast::Function& indexCtorWithSelfType = *new Ast::Function(*this, type.Index, id.constructorWithSelfType, "Index::constructorWithSelfType");
    indexCtorWithSelfType.parameters(
        param(id.indexType, type.Type_));
    indexCtorWithSelfType.code(
        ast.if_(ast.missing(p_(id.indexType), _(id.sharedObject)),
                ast.block(ast.set(p_(id.indexType), _(id.sharedObject), ast.new_(_(type.Slot))),
                          ast.set(p_(id.indexType) / _(id.sharedObject), _(id.slotRole), ast.self()),
                          ast.set(p_(id.indexType) / _(id.sharedObject), _(id.slotType), _(type.Index)))),
        ast.set(ast.self(), _(id.type), p_(id.indexType)));

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
    indexInsert.parameters(
        param(id.key, type.Cell),
        param(id.value, type.Cell));
    indexInsert.code(
        ast.if_(ast.same(p_(id.key), _(id.type)), ast.return_()),
        ast.set(ast.self(), p_(id.key), p_(id.value)),
        ast.if_(ast.and_(ast.has(m_(id.type), _(id.sharedObject)), ast.same(m_(id.type) / _(id.sharedObject) / _(id.slotRole), ast.self())),
            ast.return_()),
        ast.call(m_(id.type), _(id.addSlot), param(_(id.slotRole), p_(id.key)), param(_(id.slotType), _(type.Slot))));

    Ast::Function& indexEmpty = *new Ast::Function(*this, type.Index, id.size, "Index::empty");
    indexEmpty.returnType(type.Boolean);
    indexEmpty.code(
        ast.return_(ast.call(m_(id.type) / _(id.slots), _(id.empty))));

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
    indexRemove.parameters(
        param(id.key, type.Cell));
    indexRemove.code(
        ast.if_(ast.not_(ast.call(m_(id.type), _(id.hasSlot), param(_(id.slotRole), p_(id.key)))),
                ast.return_()),
        ast.erase(ast.self(), p_(id.key)),
        ast.call(m_(id.type), _(id.removeSlot), param(_(id.slotRole), p_(id.key))));

    Ast::Function& indexSize = *new Ast::Function(*this, type.Index, id.size, "Index::size");
    indexSize.returnType(type.Number);
    indexSize.code(
        ast.return_(ast.call(m_(id.type) / _(id.slots), _(id.size))));

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
                    type.slot(id.listType, type.List),
                    type.slot(id.index, type.Index),
                    type.slot(id.size, type.Number));
    type.Map.set(id.slots, *mapPtr);
    mapPtr = &map(type.Cell, type.Type_,
                  id.keyType, type.Cell,
                  id.objectType, type.Cell,
                  id.listType, type.ListOf(type.Cell));
    type.Map.set(id.subTypes, *mapPtr);
    type.Map.set(id.memberOf, map(type.Type_, type.Type_, type.Container, type.Container));

    Ast::Function& mapCtor = *new Ast::Function(*this, type.Map, id.constructor, "Map::constructor");
    mapCtor.code(
        m_(id.size)       = _(_0_),
        m_(id.keyType)    = m_(id.type) / _(id.subTypes) / _(id.index) / _(id.keyType) / _(id.value),
        m_(id.objectType) = m_(id.type) / _(id.subTypes) / _(id.index) / _(id.objectType) / _(id.value),
        m_(id.listType)   = m_(id.type) / _(id.subTypes) / _(id.index) / _(id.listType) / _(id.value),
        m_(id.list)       = ast.new_(m_(id.listType), _(id.constructor)),
        m_(id.index)      = ast.new_(_(type.Index), _(id.constructor)));

    Ast::Function& mapCtorWithIndexType = *new Ast::Function(*this, type.Map, id.constructorWithIndexType, "Map::constructorWithIndexType");
    mapCtorWithIndexType.parameters(
        param(id.indexType, type.Type_));
    mapCtorWithIndexType.code(
        m_(id.size)       = _(_0_),
        m_(id.keyType)    = m_(id.type) / _(id.subTypes) / _(id.index) / _(id.keyType) / _(id.value),
        m_(id.objectType) = m_(id.type) / _(id.subTypes) / _(id.index) / _(id.objectType) / _(id.value),
        m_(id.listType)   = m_(id.type) / _(id.subTypes) / _(id.index) / _(id.listType) / _(id.value),
        m_(id.list)       = ast.new_(m_(id.listType), _(id.constructor)),
        m_(id.index)      = ast.new_(_(type.Index), _(id.constructorWithSelfType), param(_(id.indexType), p_(id.indexType))));

    Ast::Function& mapTemplate = *new Ast::Function(*this, type.Map, id.template_, "static Map::template");
    mapTemplate.set(id.static_, boolean.true_);
    mapTemplate.parameters(
        param(id.keyType, type.Type_),
        param(id.objectType, type.Type_));
    mapTemplate.returnType(type.Map);
    mapTemplate.code(
        var_(id.result)   = ast.new_(_(type.Type_)),
        var_(id.listType) = ast.scall(_(type.List), _(id.template_), ast.slot(_(id.objectType), p_(id.objectType))),

        ast.call(*var_(id.result), _(id.addSubType), ast.slot(_(id.slotRole), _(id.keyType)), ast.slot(_(id.slotType), p_(id.keyType))),
        ast.call(*var_(id.result), _(id.addSubType), ast.slot(_(id.slotRole), _(id.objectType)), ast.slot(_(id.slotType), p_(id.objectType))),
        ast.call(*var_(id.result), _(id.addSubType), ast.slot(_(id.slotRole), _(id.listType)), ast.slot(_(id.slotType), *var_(id.listType))),

        ast.call(*var_(id.result), _(id.addMembership), ast.slot(_(id.cell), _(type.Map))),

        ast.call(*var_(id.result), _(id.addSlot), ast.slot(_(id.slotRole), _(id.keyType)), ast.slot(_(id.slotType), p_(id.keyType))),
        ast.call(*var_(id.result), _(id.addSlot), ast.slot(_(id.slotRole), _(id.objectType)), ast.slot(_(id.slotType), p_(id.objectType))),
        ast.call(*var_(id.result), _(id.addSlot), ast.slot(_(id.slotRole), _(id.list)), ast.slot(_(id.slotType), *var_(id.listType))),
        ast.call(*var_(id.result), _(id.addSlot), ast.slot(_(id.slotRole), _(id.listType)), ast.slot(_(id.slotType), *var_(id.listType))),
        ast.call(*var_(id.result), _(id.addSlot), ast.slot(_(id.slotRole), _(id.index)), ast.slot(_(id.slotType), _(type.Index))),
        ast.call(*var_(id.result), _(id.addSlot), ast.slot(_(id.slotRole), _(id.indexType)), ast.slot(_(id.slotType), _(type.Type_))),
        ast.call(*var_(id.result), _(id.addSlot), ast.slot(_(id.slotRole), _(id.size)), ast.slot(_(id.slotType), _(type.Number))),

        ast.set(*var_(id.result), _(id.methods), m_(id.methods)),

        ast.return_(*var_(id.result)));
    /*
    bool Map::hasKey(CellI& key)
    {
        return m_index.has(key);
    }
    */
    Ast::Function& mapHasKey = *new Ast::Function(*this, type.Map, id.hasKey, "Map::hasKey");
    mapHasKey.parameters(
        param(id.key, type.Cell));
    mapHasKey.returnType(type.Boolean);
    mapHasKey.code(
        ast.if_(ast.has(m_(id.index), p_(id.key)), ast.return_(_(boolean.true_)), ast.return_(_(boolean.false_))));

    /*
    CellI& Map::getValue(CellI& key)
    {
        if (m_index.has(key)) {
            return m_index[key][kb.id.value];
        }
        throw "No such role!";
    }
    */
    Ast::Function& mapGetValue = *new Ast::Function(*this, type.Map, id.hasKey, "Map::getValue");
    mapGetValue.parameters(
        param(id.key, type.Cell));
    mapGetValue.code(
        ast.if_(ast.has(m_(id.index), p_(id.key)), ast.return_(m_(id.index) / p_(id.key) / _(id.value)), ast.return_(_(id.emptyObject))));

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
    Ast::Function& mapAdd = *new Ast::Function(*this, type.Map, id.add, "Map::add");
    mapAdd.parameters(
        param(id.key, type.Cell),
        param(id.value, type.Cell));
    mapAdd.code(
        ast.if_(ast.same(p_(id.key), _(id.type)), ast.return_()),
        ast.if_(ast.has(m_(id.index), p_(id.key)), ast.return_()),
        m_(id.size)   = ast.add(m_(id.size), _(_1_)),
        var_(id.item) = m_(id.list).call(_(id.add), param(_(id.value), p_(id.value))),
        ast.call(m_(id.index), _(id.insert), param(_(id.key), p_(id.key)), param(_(id.value), *var_(id.item))));

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
    Ast::Function& mapRemove = *new Ast::Function(*this, type.Map, id.remove, "Map::remove");
    mapRemove.parameters(
        param(id.key, type.Cell));
    mapRemove.code(
        ast.if_(ast.missing(m_(id.index), p_(id.key)),
                ast.return_()),
        ast.call(m_(id.list), _(id.remove), param(_(id.item), m_(id.index) / p_(id.key))),
        ast.call(m_(id.index), _(id.remove), param(_(id.key), p_(id.key))),
        m_(id.size) = ast.subtract(m_(id.size), _(_1_)));

    Ast::Function& mapSize = *new Ast::Function(*this, type.Map, id.size, "Map::size");
    mapSize.returnType(type.Number);
    mapSize.code(
        ast.return_(m_(id.size)));

    Ast::Function& mapEmpty = *new Ast::Function(*this, type.Map, id.empty, "Map::empty");
    mapEmpty.returnType(type.Boolean);
    mapEmpty.code(
        ast.if_(ast.equal(m_(id.size), _(_0_)), ast.return_(_(boolean.true_)), ast.return_(_(boolean.false_))));

    registerMethods(type.Map,
                    id.constructor, mapCtor,
                    id.constructorWithIndexType, mapCtorWithIndexType,
                    id.template_, mapTemplate,
                    id.hasKey, mapHasKey,
                    id.getValue, mapGetValue,
                    id.add, mapAdd,
                    id.remove, mapRemove,
                    id.size, mapSize,
                    id.empty, mapEmpty);

    // Map<Cell, Slot>
    type.MapCellToSlot.set(id.subTypes, map(type.Cell, type.Type_, id.keyType, type.Cell, id.objectType, type.Slot, id.listType, type.ListOf(type.Slot)));
    type.MapCellToSlot.set(id.memberOf, map(type.Type_, type.Type_, type.Map, type.Map));
    mapPtr = &slots(type.slot(id.list, type.ListOf(type.Slot)),
                    type.slot(id.listType, type.ListOf(type.Slot)),
                    type.slot(id.index, type.Index),
                    type.slot(id.keyType, type.Cell),
                    type.slot(id.objectType, type.Slot),
                    type.slot(id.size, type.Number));
    type.MapCellToSlot.set(id.slots, *mapPtr);
    registerMethods(type.MapCellToSlot,
                    id.constructor, mapCtor,
                    id.constructorWithIndexType, mapCtorWithIndexType,
                    id.template_, mapTemplate,
                    id.hasKey, mapHasKey,
                    id.getValue, mapGetValue,
                    id.add, mapAdd,
                    id.remove, mapRemove,
                    id.size, mapSize,
                    id.empty, mapEmpty);

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
    registerMethods(type.MapCellToType,
                    id.constructor, mapCtor,
                    id.constructorWithIndexType, mapCtorWithIndexType,
                    id.template_, mapTemplate,
                    id.hasKey, mapHasKey,
                    id.getValue, mapGetValue,
                    id.add, mapAdd,
                    id.remove, mapRemove,
                    id.size, mapSize,
                    id.empty, mapEmpty);

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
    registerMethods(type.MapTypeToType,
                    id.constructor, mapCtor,
                    id.constructorWithIndexType, mapCtorWithIndexType,
                    id.template_, mapTemplate,
                    id.hasKey, mapHasKey,
                    id.getValue, mapGetValue,
                    id.add, mapAdd,
                    id.remove, mapRemove,
                    id.size, mapSize,
                    id.empty, mapEmpty);
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
    setCtor.code(
        m_(id.size)       = _(_0_),
        m_(id.objectType) = m_(id.type) / _(id.subTypes) / _(id.index) / _(id.objectType) / _(id.value),
        m_(id.listType)   = m_(id.type) / _(id.subTypes) / _(id.index) / _(id.listType) / _(id.value),
        m_(id.list)       = ast.new_(m_(id.listType), _(id.constructor)),
        m_(id.index)      = ast.new_(_(type.Index), _(id.constructor)));

    Ast::Function& setTemplate = *new Ast::Function(*this, type.Set, id.template_, "static Set::template");
    setTemplate.set(id.static_, boolean.true_);
    setTemplate.parameters(
        param(id.objectType, type.Type_));
    setTemplate.returnType(type.Set);
    setTemplate.code(
        var_(id.result)   = ast.new_(_(type.Type_)),
        var_(id.listType) = ast.scall(_(type.List), _(id.template_), param(_(id.objectType), p_(id.objectType))),

        ast.call(*var_(id.result), _(id.addSubType), param(_(id.slotRole), _(id.objectType)), param(_(id.slotType), p_(id.objectType))),
        ast.call(*var_(id.result), _(id.addSubType), param(_(id.slotRole), _(id.listType)), param(_(id.slotType), *var_(id.listType))),

        ast.call(*var_(id.result), _(id.addMembership), param(_(id.cell), _(type.Map))),

        ast.call(*var_(id.result), _(id.addSlot), param(_(id.slotRole), _(id.objectType)), param(_(id.slotType), p_(id.objectType))),
        ast.call(*var_(id.result), _(id.addSlot), param(_(id.slotRole), _(id.list)), param(_(id.slotType), *var_(id.listType))),
        ast.call(*var_(id.result), _(id.addSlot), param(_(id.slotRole), _(id.listType)), param(_(id.slotType), *var_(id.listType))),
        ast.call(*var_(id.result), _(id.addSlot), param(_(id.slotRole), _(id.index)), param(_(id.slotType), _(type.Index))),
        ast.call(*var_(id.result), _(id.addSlot), param(_(id.slotRole), _(id.indexType)), param(_(id.slotType), _(type.Type_))),
        ast.call(*var_(id.result), _(id.addSlot), param(_(id.slotRole), _(id.size)), param(_(id.slotType), _(type.Number))),

        ast.set(*var_(id.result), _(id.methods), m_(id.methods)),

        ast.return_(*var_(id.result)));

    Ast::Function& setAdd = *new Ast::Function(*this, type.Set, id.add, "Set::add");
    setAdd.parameters(
        param(id.value, type.Cell));
    setAdd.returnType(type.ListItem);
    setAdd.code(
        ast.if_(ast.has(m_(id.index), p_(id.value)),
                ast.return_(m_(id.index) / p_(id.value))),
        var_(id.listItem)   = m_(id.list).call(_(id.add), ast.slot(_(id.value), p_(id.value))),
        m_(id.size) = ast.add(m_(id.size), _(_1_)),
        ast.call(m_(id.indexType) / _(id.slots), _(id.add), ast.slot(_(id.key), p_(id.value)), ast.slot(_(id.value), *var_(id.listItem))),
        ast.set(m_(id.index), p_(id.value), *var_(id.listItem)),
        ast.return_(*var_(id.listItem)));

    Ast::Function& setContains = *new Ast::Function(*this, type.Set, id.contains, "Set::contains");
    setContains.parameters(
        param(id.value, type.Cell));
    setContains.returnType(type.Boolean);
    setContains.code(
        ast.if_(ast.has(m_(id.index), p_(id.value)),
                ast.return_(_(boolean.true_)),
                ast.return_(_(boolean.false_))));

    Ast::Function& setErase = *new Ast::Function(*this, type.Set, id.add, "Set::erase");
    setErase.parameters(
        param(id.value, type.Cell));
    setErase.code(
        ast.if_(ast.missing(m_(id.index), p_(id.value)),
                ast.return_()),
        ast.erase(m_(id.index), p_(id.value)),
        m_(id.size) = ast.subtract(m_(id.size), _(_1_)));


    Ast::Function& setSize = *new Ast::Function(*this, type.Set, id.size, "Set::size");
    setSize.returnType(type.Number);
    setSize.code(
        ast.return_(m_(id.size)));

    Ast::Function& setEmpty = *new Ast::Function(*this, type.Set, id.empty, "Set::empty");
    setEmpty.returnType(type.Boolean);
    setEmpty.code(
        ast.if_(ast.equal(m_(id.size), _(_0_)), ast.return_(_(boolean.true_)), ast.return_(_(boolean.false_))));

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

    mapPtr = &slots(type.slot(id.method, type.op.Function),
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
    numberFactorial.parameters(
        param(id.input, type.Number));
    numberFactorial.returnType(type.Number);
    numberFactorial.code(
        ast.if_(ast.greaterThanOrEqual(p_(id.input), _(_1_)),
                ast.return_(ast.multiply(p_(id.input), ast.call(ast.self(), _(test.factorial), ast.slot(_(id.input), ast.subtract(p_(id.input), _(_1_)))))),
                ast.return_(_(_1_))));

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
    shapeCtor.parameters(
        param(id.id, type.Number),
        param(id.color, type.Color),
        param(id.width, type.Number),
        param(id.height, type.Number));
    shapeCtor.code(
        m_(id.id)  = p_(id.id),
        m_(id.color) = p_(id.color),
        m_(id.width)  = p_(id.width),
        m_(id.height) = p_(id.height));

    /*
    void Shape::addPixel(cells::hybrid::Pixel& pixel)
    {
        m_pixels.push_back({ pixel.m_x.value(), pixel.m_y.value() });
        m_hybridPixels.insert(&pixel);
    }
    */
    Ast::Function& shapeAddPixel = *new Ast::Function(*this, type.arc.Shaper, id.addPixel, "Shape::addPixel");
    shapeAddPixel.parameters(
        param(id.pixel, type.Pixel));
    shapeAddPixel.code(
        ast.call(m_(id.pixels), _(id.add), ast.slot(_(id.value), p_(id.pixel))));

    /*
    bool Shape::hasPixel(cells::hybrid::Pixel& pixel) const
    {
        return m_hybridPixels.find(&pixel) != m_hybridPixels.end();
    }
    */
    Ast::Function& shapeHasPixel = *new Ast::Function(*this, type.arc.Shaper, id.hasPixel, "Shape::hasPixel");
    shapeHasPixel.parameters(
        param(id.pixel, type.Pixel));
    shapeHasPixel.code(
        m_(id.pixels) / _(id.index) / p_(id.pixel)); // TODO we need a Set class

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
    shaperCtor.parameters(
        param(id.width, type.Number),
        param(id.height, type.Number),
        param(id.picture, type.Picture));
    shaperCtor.code(
        m_(id.width)   = p_(id.width),
        m_(id.height)  = p_(id.height),
        m_(id.picture) = p_(id.picture),
        ast.call(ast.self(), _(id.processInputPixels)));
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
    shaperProcessInputPixels.code(
        var_(id.pixels) = m_(id.picture) / _(id.pixels),
        var_(id.pixel)  = *var_(id.pixels) / _(id.first),
        ast.while_(ast.same(*var_(id.pixel), *var_(id.pixels) / _(id.last)),
                   ast.block(
                       ast.call(m_(id.inputPixels), _(id.add), ast.slot(_(id.value), *var_(id.pixel))),
                       var_(id.pixel) = *var_(id.pixels) / _(id.first))));

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
    shaperProcess.code(
        var_(id.shapeId) = _(_1_),
        ast.while_(ast.not_(ast.call(m_(id.inputPixels), _(id.empty))),
                   ast.block(
                       var_(id.firstPixel) = ast.call(m_(id.inputPixels), _(id.first), ast.slot(_(id.pixel), p_(id.checkPixel))),
                       ast.call(m_(id.shapes), _(id.add), ast.slot(_(id.value), ast.new_(_(type.arc.Shape), _(id.constructor), ast.slot(_(id.id), *var_(id.id)), ast.slot(_(id.color), *var_(id.firstPixel) / _(id.color)), ast.slot(_(id.width), m_(id.width)), ast.slot(_(id.height), m_(id.height))))),
                       var_(id.shape)       = ast.call(m_(id.shapes), _(id.last)),
                       var_(id.checkPixels) = ast.new_(_(type.List)),
                       ast.call(*var_(id.checkPixels), _(id.add), ast.slot(_(id.value), *var_(id.firstPixel))),
                       ast.while_(ast.not_(ast.call(*var_(id.checkPixels), _(id.empty))),
                                  ast.block(
                                      var_(id.checkPixel) = ast.call(*var_(id.checkPixels), _(id.first)),
                                      ast.call(ast.self(), _(id.processPixel), ast.slot(_(id.shape), *var_(id.shape)), ast.slot(_(id.checkPixels), *var_(id.checkPixels)), ast.slot(_(id.checkPixel), *var_(id.checkPixel))),
                                      ast.call(*var_(id.checkPixels), _(id.erase), ast.slot(_(id.value), *var_(id.firstPixel))))),
                       ast.call(*var_(id.shape), _(id.sortPixels)))));

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
    shaperProcessPixel.parameters(
        param(id.shape, type.Shape),
        param(id.checkPixels, type.ListOf(type.Pixel)),
        param(id.checkPixel, type.Pixel));
    shaperProcessPixel.returnType(type.Number);
    shaperProcessPixel.code(
        ast.call(p_(id.shape), _(id.addPixel), ast.slot(_(id.pixel), p_(id.checkPixel))),
        ast.call(m_(id.inputPixels), _(id.erase), ast.slot(_(id.pixel), p_(id.checkPixel))),
        ast.if_(var_(id.pixel) = ast.call(ast.self(), _(id.processAdjacentPixel), ast.slot(_(id.direction), _(directions.up)), ast.slot(_(id.shape), p_(id.shape)), ast.slot(_(id.checkPixels), p_(id.checkPixels)), ast.slot(_(id.checkPixel), p_(id.checkPixel))),
                ast.block(
                    ast.call(ast.self(), _(id.processAdjacentPixel), ast.slot(_(id.direction), _(directions.left)), ast.slot(_(id.shape), p_(id.shape)), ast.slot(_(id.checkPixels), p_(id.checkPixels)), ast.slot(_(id.checkPixel), *var_(id.pixel))),
                    ast.call(ast.self(), _(id.processAdjacentPixel), ast.slot(_(id.direction), _(directions.right)), ast.slot(_(id.shape), p_(id.shape)), ast.slot(_(id.checkPixels), p_(id.checkPixels)), ast.slot(_(id.checkPixel), *var_(id.pixel))))),
        ast.if_(var_(id.pixel) = ast.call(ast.self(), _(id.processAdjacentPixel), ast.slot(_(id.direction), _(directions.down)), ast.slot(_(id.shape), p_(id.shape)), ast.slot(_(id.checkPixels), p_(id.checkPixels)), ast.slot(_(id.checkPixel), p_(id.checkPixel))),
                ast.block(
                    ast.call(ast.self(), _(id.processAdjacentPixel), ast.slot(_(id.direction), _(directions.left)), ast.slot(_(id.shape), p_(id.shape)), ast.slot(_(id.checkPixels), p_(id.checkPixels)), ast.slot(_(id.checkPixel), *var_(id.pixel))),
                    ast.call(ast.self(), _(id.processAdjacentPixel), ast.slot(_(id.direction), _(directions.right)), ast.slot(_(id.shape), p_(id.shape)), ast.slot(_(id.checkPixels), p_(id.checkPixels)), ast.slot(_(id.checkPixel), *var_(id.pixel))))),
        ast.call(ast.self(), _(id.processAdjacentPixel), ast.slot(_(id.direction), _(directions.left)), ast.slot(_(id.shape), p_(id.shape)), ast.slot(_(id.checkPixels), p_(id.checkPixels)), ast.slot(_(id.checkPixel), p_(id.checkPixel))),
        ast.call(ast.self(), _(id.processAdjacentPixel), ast.slot(_(id.direction), _(directions.right)), ast.slot(_(id.shape), p_(id.shape)), ast.slot(_(id.checkPixels), p_(id.checkPixels)), ast.slot(_(id.checkPixel), p_(id.checkPixel))));

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
    shaperProcessAdjacentPixel.parameters(
        param(id.direction, type.Directions),
        param(id.shape, type.Shape),
        param(id.checkPixels, type.ListOf(type.Pixel)),
        param(id.checkPixel, type.Pixel));
    shaperProcessAdjacentPixel.returnType(type.Number);
    shaperProcessAdjacentPixel.code(
        ast.if_(ast.call(*var_(id.checkPixel), _(id.has), ast.slot(_(id.direction), p_(id.direction))),
                ast.block(
                    var_(id.pixel) = _(id.checkPixel) / _(id.direction),
                    ast.if_(ast.and_(ast.same(*var_(id.pixel) / _(id.color), *var_(id.shape) / _(id.color)), ast.not_(ast.call(*var_(id.shape), _(id.hasPixel), ast.slot(_(id.pixel), *var_(id.pixel))))),
                        ast.call(*var_(id.checkPixels), _(id.insert), ast.slot(_(id.pixel), *var_(id.pixel)))),
                    ast.return_(_(id.pixel))),
        ast.return_(_(id.emptyObject))));

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
