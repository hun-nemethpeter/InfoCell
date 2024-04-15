#include "Brain.h"

#include <boost/algorithm/string/regex.hpp>
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
    currentFn(kb, kb.type.Cell, "currentFn"),
    currentStruct(kb, kb.type.Cell, "currentStruct"),
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
    incomplete(kb, kb.type.Cell, "incomplete"),
    index(kb, kb.type.Cell, "index"),
    indexType(kb, kb.type.Cell, "indexType"),
    input(kb, kb.type.Cell, "input"),
    inputPixels(kb, kb.type.Cell, "inputPixels"),
    insert(kb, kb.type.Cell, "insert"),
    instanceOf(kb, kb.type.Cell, "instanceOf"),
    instances(kb, kb.type.Cell, "instances"),
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
    resolvedScope(kb, kb.type.Cell, "resolvedScope"),
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
    structTs(kb, kb.type.Cell, "structTs"),
    structType(kb, kb.type.Cell, "structType"),
    subTypes(kb, kb.type.Cell, "subTypes"),
    template_(kb, kb.type.Cell, "template"),
    templateId(kb, kb.type.Cell, "templateParams"),
    templateParams(kb, kb.type.Cell, "templateParams"),
    then(kb, kb.type.Cell, "then"),
    type(kb, kb.type.Cell, "type"),
    unknownInstances(kb, kb.type.Cell, "unknownInstances"),
    unknownStructs(kb, kb.type.Cell, "unknownStructs"),
    value(kb, kb.type.Cell, "value"),
    variables(kb, kb.type.Cell, "variables"),
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
    auto& ids   = kb.ids;
    auto& type = kb.type;
    auto& ast  = kb.type.ast;

    CellI* map = nullptr;

    map = &kb.slots(type.slot(ids.ast, ast.Base),
                    type.slot(ids.lhs, Base),
                    type.slot(ids.rhs, Base),
                    type.slot(ids.value, type.Number));
    Add.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.ast, ast.Base),
                    type.slot(ids.lhs, Base),
                    type.slot(ids.rhs, Base),
                    type.slot(ids.value, type.Boolean));
    And.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.ast, ast.Base),
                    type.slot(ids.status, type.Cell),
                    type.slot(ids.ops, type.Cell),
                    type.slot(ids.value, type.Cell));
    Block.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.ast, ast.Base),
                    type.slot(ids.value, type.Cell));
    ConstVar.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.ast, ast.Base),
                    type.slot(ids.input, Base));
    Delete.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.ast, ast.Base),
                    type.slot(ids.lhs, Base),
                    type.slot(ids.rhs, Base),
                    type.slot(ids.value, type.Number));
    Divide.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.ast, ast.Base),
                    type.slot(ids.status, type.Cell),
                    type.slot(ids.condition, Base),
                    type.slot(ids.statement, Base));
    Do.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.ast, ast.Base),
                    type.slot(ids.lhs, Base),
                    type.slot(ids.rhs, Base),
                    type.slot(ids.value, type.Boolean));
    Equal.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.ast, ast.Base),
                    type.slot(ids.cell, Base),
                    type.slot(ids.role, Base),
                    type.slot(ids.value, Base));
    Erase.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.ast, ast.Base),
                    type.slot(ids.value, Var));
    EvalVar.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.ast, ast.Base),
                    type.slot(ids.stack, type.Stack),
                    type.slot(ids.op, type.ListOf(Base)),
                    type.slot(ids.static_, type.Boolean));
    Function.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.ast, ast.Base),
                    type.slot(ids.cell, Base),
                    type.slot(ids.role, Base),
                    type.slot(ids.value, type.Cell));
    Get.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.ast, ast.Base),
                    type.slot(ids.lhs, Base),
                    type.slot(ids.rhs, Base),
                    type.slot(ids.value, type.Boolean));
    GreaterThan.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.ast, ast.Base),
                    type.slot(ids.lhs, Base),
                    type.slot(ids.rhs, Base),
                    type.slot(ids.value, type.Boolean));
    GreaterThanOrEqual.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.ast, ast.Base),
                    type.slot(ids.cell, Base),
                    type.slot(ids.role, Base),
                    type.slot(ids.value, type.Boolean));
    Has.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.ast, ast.Base),
                    type.slot(ids.status, type.Cell),
                    type.slot(ids.condition, Base),
                    type.slot(ids.then, Base),
                    type.slot(ids.else_, Base));
    If.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.ast, ast.Base),
                    type.slot(ids.lhs, Base),
                    type.slot(ids.rhs, Base),
                    type.slot(ids.value, type.Boolean));
    LessThan.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.ast, ast.Base),
                    type.slot(ids.lhs, Base),
                    type.slot(ids.rhs, Base),
                    type.slot(ids.value, type.Boolean));
    LessThanOrEqual.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.ast, ast.Base),
                    type.slot(ids.cell, Base),
                    type.slot(ids.role, Base),
                    type.slot(ids.value, type.Boolean));
    Missing.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.ast, ast.Base),
                    type.slot(ids.lhs, Base),
                    type.slot(ids.rhs, Base),
                    type.slot(ids.value, type.Number));
    Multiply.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.ast, ast.Base),
                    type.slot(ids.value, type.Cell),
                    type.slot(ids.objectType, Base));
    New.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.ast, ast.Base),
                    type.slot(ids.input, Base),
                    type.slot(ids.value, type.Boolean));
    Not.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.ast, ast.Base),
                    type.slot(ids.lhs, Base),
                    type.slot(ids.rhs, Base),
                    type.slot(ids.value, type.Boolean));
    NotEqual.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.ast, ast.Base),
                    type.slot(ids.lhs, Base),
                    type.slot(ids.rhs, Base),
                    type.slot(ids.value, type.Boolean));
    NotSame.set(kb.ids.slots, *map);

    map = &kb.slots(type.slot(ids.ast, ast.Base),
                    type.slot(ids.lhs, Base),
                    type.slot(ids.rhs, Base),
                    type.slot(ids.value, type.Boolean));
    Or.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.ast, ast.Base),
                    type.slot(ids.result, ast.Base));
    Return.set(kb.ids.slots, *map);

    map = &kb.slots(type.slot(ids.ast, ast.Base),
                    type.slot(ids.lhs, Base),
                    type.slot(ids.rhs, Base),
                    type.slot(ids.value, type.Boolean));
    Same.set(kb.ids.slots, *map);

    map = &kb.slots(type.slot(ids.ast, ast.Base),
                    type.slot(ids.cell, Base),
                    type.slot(ids.role, Base),
                    type.slot(ids.value, Base));
    Set.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.ast, ast.Base),
                    type.slot(ids.lhs, Base),
                    type.slot(ids.rhs, Base),
                    type.slot(ids.value, type.Number));
    Subtract.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.ast, ast.Base),
                    type.slot(ids.objectType, type.Type_),
                    type.slot(ids.value, type.Cell));
    Var.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.ast, ast.Base),
                    type.slot(ids.status, type.Cell),
                    type.slot(ids.condition, Base),
                    type.slot(ids.statement, Base));
    While.set(ids.slots, *map);
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
    auto& ids   = kb.ids;
    auto& type = kb.type;
    CellI* map = nullptr;

    map = &kb.slots(type.slot(ids.lhs, Base),
                    type.slot(ids.rhs, Base));
    Add.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.lhs, Base),
                    type.slot(ids.rhs, Base));
    And.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.asts, type.Cell));
    Block.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.cell, Base),
                    type.slot(ids.method, Base),
                    type.slot(ids.parameters, type.ListOf(Slot)));
    Call.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.value, type.Cell));
    Cell.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.cell, Base));
    Delete.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.lhs, Base),
                    type.slot(ids.rhs, Base));
    Divide.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.condition, Base),
                    type.slot(ids.statement, Base));
    Do.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.lhs, Base),
                    type.slot(ids.rhs, Base));
    Equal.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.cell, Base),
                    type.slot(ids.role, Base));
    Erase.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.name, type.Cell),
                    type.slot(ids.structType, type.Cell),
                    type.slot(ids.parameters, type.ListOf(Slot)),
                    type.slot(ids.returnType, type.Type_),
                    type.slot(ids.code, Base),
                    type.slot(ids.scope, Base),
                    type.slot(ids.static_, type.Boolean));
    Function.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.name, type.Cell),
                    type.slot(ids.parameters, type.ListOf(Slot)),
                    type.slot(ids.returnType, type.Type_),
                    type.slot(ids.code, Base),
                    type.slot(ids.scope, Base),
                    type.slot(ids.static_, type.Boolean));
    FunctionT.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.cell, Base),
                    type.slot(ids.role, Base));
    Get.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.lhs, Base),
                    type.slot(ids.rhs, Base));
    GreaterThan.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.lhs, Base),
                    type.slot(ids.rhs, Base));
    GreaterThanOrEqual.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.cell, Base),
                    type.slot(ids.role, Base));
    Has.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.condition, Base),
                    type.slot(ids.then, Base),
                    type.slot(ids.else_, Base));
    If.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.lhs, Base),
                    type.slot(ids.rhs, Base));
    LessThan.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.lhs, Base),
                    type.slot(ids.rhs, Base));
    LessThanOrEqual.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.role, Base));
    Member.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.cell, Base),
                    type.slot(ids.role, Base));
    Missing.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.lhs, Base),
                    type.slot(ids.rhs, Base));
    Multiply.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.objectType, Base),
                    type.slot(ids.constructor, Base),
                    type.slot(ids.parameters, type.ListOf(type.ast.Slot)));
    New.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.input, Base));
    Not.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.lhs, Base),
                    type.slot(ids.rhs, Base));
    NotEqual.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.lhs, Base),
                    type.slot(ids.rhs, Base));
    NotSame.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.lhs, Base),
                    type.slot(ids.rhs, Base));
    Or.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.role, type.Cell));
    Parameter.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.value, type.Cell));
    Return.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.lhs, Base),
                    type.slot(ids.rhs, Base));
    Same.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.id, type.Cell),
                    type.slot(ids.scopes, type.TrieMap),
                    type.slot(ids.resolvedScope, type.ast.Scope),
                    type.slot(ids.parent, Scope),
                    type.slot(ids.functions, type.TrieMap),
                    type.slot(ids.structs, type.TrieMap),
                    type.slot(ids.structTs, type.TrieMap),
                    type.slot(ids.variables, type.ListOf(type.ast.Slot)));
    Scope.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.cell, Base),
                    type.slot(ids.role, Base),
                    type.slot(ids.value, Base));
    Set.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.slotRole, Base),
                    type.slot(ids.slotType, Base));
    Slot.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.cell, Base),
                    type.slot(ids.method, Base),
                    type.slot(ids.parameters, type.ListOf(Slot)));
    StaticCall.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.id, type.Cell),
                    type.slot(ids.incomplete, type.Boolean),
                    type.slot(ids.instanceOf, Base),
                    type.slot(ids.scope, Base),
                    type.slot(ids.methods, type.MapOf(type.Cell, type.ast.Function)),
                    type.slot(ids.members, type.ListOf(type.ast.Slot)),
                    type.slot(ids.subTypes, type.ListOf(type.ast.Slot)),
                    type.slot(ids.memberOf, type.ListOf(type.Type_)));
    Struct.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.value, type.Struct));
    StructRef.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.id, type.Cell),
                    type.slot(ids.scope, Base),
                    type.slot(ids.methods, type.MapOf(type.Cell, type.ast.Function)),
                    type.slot(ids.members, type.ListOf(type.ast.Slot)),
                    type.slot(ids.subTypes, type.ListOf(type.ast.Slot)),
                    type.slot(ids.memberOf, type.ListOf(type.Type_)),
                    type.slot(ids.templateParams, type.MapOf(type.Cell, type.Type_)));
    StructT.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.id, type.Cell));
    SubType.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.lhs, Base),
                    type.slot(ids.rhs, Base));
    Subtract.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.id, Base),
                    type.slot(ids.scopes, type.List),
                    type.slot(ids.parameters, type.ListOf(Slot)));
    TemplatedType.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.role, type.Cell));
    TemplateParam.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.role, Base));
    Var.set(ids.slots, *map);

    map = &kb.slots(type.slot(ids.condition, Base),
                    type.slot(ids.statement, Base));
    While.set(ids.slots, *map);
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
    auto& ids   = kb.ids;
    auto& type = kb.type;

    mapPtr = &kb.slots(type.slot(ids.slotType, type.Type_),
                       type.slot(ids.slotRole, type.Cell));
    Slot.set(kb.ids.slots, *mapPtr);

    mapPtr = &kb.slots(type.slot(ids.slots, MapCellToSlot),
                       type.slot(ids.incomplete, type.Boolean),
                       type.slot(ids.sharedObject, Slot),
                       type.slot(ids.subTypes, MapCellToType),
                       type.slot(ids.memberOf, MapTypeToType),
                       type.slot(ids.asts, MapCellToAstFunction),
                       type.slot(ids.methods, MapCellToOpFunction));
    Type_.set(ids.slots, *mapPtr);

    mapPtr = &kb.slots(type.slot(ids.members, List));
    Enum.set(ids.slots, *mapPtr);

    mapPtr = &kb.slots(type.slot(ids.previous, type.ListItem),
                       type.slot(ids.next, type.ListItem),
                       type.slot(ids.value, type.Cell));
    type.ListItem.set(ids.slots, *mapPtr);

    mapPtr = &kb.slots(type.slot(ids.keyType, type.Cell),
                       type.slot(ids.objectType, type.Cell),
                       type.slot(ids.list, type.ListOf(type.Cell)),
                       type.slot(ids.listType, type.List),
                       type.slot(ids.rootNode, type.TrieMapNode),
                       type.slot(ids.size, type.Number));
    type.TrieMap.set(ids.slots, *mapPtr);

    mapPtr = &kb.slots(type.slot(ids.children, type.Index),
                       type.slot(ids.data, type.ListItem),
                       type.slot(ids.parent, type.TrieMapNode));
    type.TrieMapNode.set(ids.slots, *mapPtr);

    mapPtr = &kb.slots(type.slot(ids.data, type.ScopeData),
                       type.slot(ids.code, type.op.Base),
                       type.slot(ids.stack, type.kb.type.ListItem));
    type.Program.set(ids.slots, *mapPtr);

    mapPtr = &kb.slots(type.slot(ids.id, type.Cell),
                       type.slot("idScope", type.ast.Scope),
                       type.slot(ids.scope, type.ast.Scope),
                       type.slot(ids.resolvedScope, type.ast.Scope),
                       type.slot(ids.currentFn, type.ast.Function),
                       type.slot(ids.currentStruct, type.ast.Struct),
                       type.slot(ids.templateId, type.List),
                       type.slot(ids.templateParams, type.List),
                       type.slot(ids.value, type.ast.Struct));
    type.StructReference.set(ids.slots, *mapPtr);

    mapPtr = &kb.slots(type.slot(ids.id, type.Cell),
                       type.slot(ids.currentFn, type.ast.Function),
                       type.slot(ids.currentStruct, type.ast.Struct),
                       type.slot(ids.scope, type.ast.Scope),
                       type.slot(ids.resolvedScope, type.ast.Scope),
                       type.slot("globalScope", type.ast.Scope),
                       type.slot("globalResolvedScope", type.ast.Scope),
                       type.slot(ids.functions, type.TrieMap),
                       type.slot(ids.structs, type.TrieMap),
                       type.slot(ids.unknownStructs, type.TrieMap),
                       type.slot(ids.instances, type.TrieMap),
                       type.slot(ids.unknownInstances, type.TrieMap),
                       type.slot(ids.variables, type.ListOf(type.ast.Slot)));
    type.CompileState.set(ids.slots, *mapPtr);

    mapPtr = &kb.slots(type.slot(ids.id, type.Cell),
                       type.slot(ids.scopes, type.Map),
                       type.slot(ids.parent, ScopeData),
                       type.slot(ids.functions, type.TrieMap),
                       type.slot(ids.structs, type.TrieMap),
                       type.slot(ids.unknownStructs, type.TrieMap),
                       type.slot(ids.variables, type.ListOf(type.ast.Slot)));
    type.ScopeData.set(ids.slots, *mapPtr);

    mapPtr = &kb.slots(type.slot(ids.method, type.op.Function),
                       type.slot(ids.input, type.Index),
                       type.slot(ids.output, type.op.Var),
                       type.slot(ids.localVars, type.Index));
    type.StackFrame.set(ids.slots, *mapPtr);

    kb.m_initPhase = Brain::InitPhase::SlotTypeInitialzed;
}

cells::CellI& Types::slot(cells::CellI& role, cells::CellI& type)
{
    CellI& ret = *new Object(kb, kb.type.Slot);
    ret.set(kb.ids.slotRole, role);
    ret.set(kb.ids.slotType, type);

    return ret;
}

cells::CellI& Types::slot(const std::string& role, cells::CellI& type)
{
    CellI& ret = *new Object(kb, kb.type.Slot);
    ret.set(kb.ids.slotRole, kb.id(role));
    ret.set(kb.ids.slotType, type);

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
        listType.set(kb.ids.subTypes, kb.map(kb.type.Cell, kb.type.Type_, kb.ids.objectType, type, kb.ids.itemType, itemType));
        listType.set(kb.ids.memberOf, kb.map(kb.type.Type_, kb.type.Type_, kb.type.List, kb.type.List));
        itemType.set(kb.ids.memberOf, kb.map(kb.type.Type_, kb.type.Type_, kb.type.ListItem, kb.type.ListItem));
        itemType.set(kb.ids.subTypes, kb.map(kb.type.Cell, kb.type.Type_, kb.ids.objectType, type, kb.ids.itemType, itemType));

        CellI* map = nullptr;

        map = &kb.slots(kb.type.slot(kb.ids.first, itemType),
                        kb.type.slot(kb.ids.last, itemType),
                        kb.type.slot(kb.ids.size, kb.type.Number),
                        kb.type.slot(kb.ids.itemType, itemType),
                        kb.type.slot(kb.ids.objectType, type));
        listType.set(kb.ids.slots, *map);

        map = &kb.slots(kb.type.slot(kb.ids.previous, itemType),
                        kb.type.slot(kb.ids.next, itemType),
                        kb.type.slot(kb.ids.value, type));
        itemType.set(kb.ids.slots, *map);

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

    if (kb.type.Map.has(kb.ids.methods)) {
        mapType.set(kb.ids.methods, kb.type.Map[kb.ids.methods]); // TODO
    }
    mapType.set(kb.ids.memberOf, kb.map(kb.type.Type_, kb.type.Type_, kb.type.Map, kb.type.Map));
    mapType.set(kb.ids.subTypes, kb.map(kb.type.Cell, kb.type.Type_, kb.ids.keyType, keyType, kb.ids.objectType, valueType));

    map = &kb.slots(kb.type.slot(kb.ids.list, ListOf(valueType)),
                    kb.type.slot(kb.ids.index, kb.type.Index),
                    kb.type.slot(kb.ids.indexType, kb.type.Type_),
                    kb.type.slot(kb.ids.keyType, keyType),
                    kb.type.slot(kb.ids.objectType, valueType),
                    kb.type.slot(kb.ids.size, kb.type.Number));
    mapType.set(kb.ids.slots, *map);

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

    setType.set(kb.ids.methods, kb.type.Set[kb.ids.methods]);
    setType.set(kb.ids.memberOf, kb.map(kb.type.Type_, kb.type.Type_, kb.type.Set, kb.type.Set));
    setType.set(kb.ids.subTypes, kb.map(kb.type.Cell, kb.type.Type_, kb.ids.objectType, valueType));

    map = &kb.slots(kb.type.slot(kb.ids.list, ListOf(valueType)),
                    kb.type.slot(kb.ids.index, kb.type.Index),
                    kb.type.slot(kb.ids.indexType, kb.type.Type_),
                    kb.type.slot(kb.ids.objectType, valueType),
                    kb.type.slot(kb.ids.size, kb.type.Number));
    setType.set(kb.ids.slots, *map);

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
        auto& cellValue = typeAst[kb.ids.value];
        if (!cellValue.isA(kb.type.Type_) && !cellValue.isA(kb.type.Enum)) {
            throw "Type AST referencing a non-type!";
        }
        return typeAst[kb.ids.value];
    }
    if (&typeAst.type() == &kb.type.ast.StructRef) {
        CellI& astStructId = typeAst[kb.ids.value];
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

CellI& Ast::Base::resolveId(CellI& structId, CellI& containerId, CellI& unknownContainerId, CellI& resolveState, std::function<void(CellI& structReference)> unknownCb)
{
    auto& container = static_cast<TrieMap&>(resolveState[containerId]);

    if (container.hasKey(structId)) {
        return container.getValue(structId);
    } else {
        auto& unresolvedContainer = static_cast<TrieMap&>(resolveState[unknownContainerId]);
        if (unresolvedContainer.hasKey(structId)) {
            return unresolvedContainer.getValue(structId)[kb.ids.value];
        } else {
            auto& unresolvedStruct = *new Object(kb, kb.type.Type_, std::format("{}", structId.label()));
            unresolvedStruct.set(kb.ids.incomplete, kb.boolean.true_);

            auto& structReference = *new Object(kb, kb.type.StructReference);
            structReference.set(kb.ids.value, unresolvedStruct);
            structReference.set(kb.ids.id, structId);
            structReference.set(kb.ids.scope, resolveState[kb.ids.scope]);
            structReference.set(kb.ids.resolvedScope, resolveState[kb.ids.resolvedScope]);

            if (resolveState.has(kb.ids.currentFn)) {
                structReference.set(kb.ids.currentFn, resolveState[kb.ids.currentFn]);
            }
            if (resolveState.has(kb.ids.currentStruct)) {
                structReference.set(kb.ids.currentStruct, resolveState[kb.ids.currentStruct]);
            }
            unknownCb(structReference);

            unresolvedContainer.add(structId, structReference);
            return unresolvedStruct;
        }
    }
}

CellI& Ast::Base::resolveTemplateInstanceId(CellI& structId, CellI& idScope, CellI& resolveState, CellI& ast, CellI& templateParams)
{
    auto& templateId = ast[kb.ids.id];
    return resolveId(structId, kb.ids.instances, kb.ids.unknownInstances, resolveState, [this, &resolveState, &templateId, &templateParams, &ast, &idScope](CellI& structReference) {
        structReference.set(kb.ids.templateId, templateId);
        structReference.set(kb.ids.templateParams, templateParams);
        if (ast.has(kb.ids.scopes)) {
            structReference.set(kb.id("idScope"), idScope);
        }
    });
}

CellI& Ast::Base::resolveStructId(CellI& structId, CellI& resolveState)
{
    static std::function<void(CellI&)> emptyCb = [](CellI&) {};

    return resolveId(structId, kb.ids.structs, kb.ids.unknownStructs, resolveState, emptyCb);
}

CellI& Ast::Base::resolveTemplatedType(CellI& ast, CellI& resolveState)
{
    auto& templateId     = ast[kb.ids.id];
    auto& templateParams = ast[kb.ids.parameters];

    Scope* scopePtr = &static_cast<Scope&>(resolveState[kb.ids.scope]);
    if (ast.has(kb.ids.scopes)) {
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
        if (resolveState.has(kb.ids.currentStruct)) {
            std::cerr << std::format("Current struct: {}", resolveState[kb.ids.currentStruct].label()) << std::endl;
        }
        if (resolveState.has(kb.ids.currentFn)) {
            std::cerr << std::format("Current function: {}", resolveState[kb.ids.currentFn].label()) << std::endl;
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
        CellI& slotRole = slot[kb.ids.slotRole];
        CellI& slotType = slot[kb.ids.slotType];
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
    set(kb.ids.role, role);
}

Ast::Get& Ast::Parameter::operator/(Base& role)
{
    return Get::New(kb, *this, role);
}

Ast::Slot::Slot(brain::Brain& kb, CellI& role, CellI& type) :
    BaseT<Slot>(kb, kb.type.ast.Slot, "ast.slot")
{
    set(kb.ids.slotRole, role);
    set(kb.ids.slotType, type);
}

Ast::Call::Call(brain::Brain& kb, CellI& cell, CellI& method) :
    BaseT<Call>(kb, kb.type.ast.Call, "ast.call")
{
    set(kb.ids.cell, cell);
    set(kb.ids.method, method);
}

void Ast::Call::addParam(Slot& slot)
{
    if (missing(kb.ids.parameters)) {
        set(kb.ids.parameters, kb.list(slot));
    } else {
        List& paramList = static_cast<List&>(get(kb.ids.parameters));
        paramList.add(slot);
    }
}

Ast::StaticCall::StaticCall(brain::Brain& kb, CellI& cell, CellI& method) :
    BaseT<StaticCall>(kb, kb.type.ast.StaticCall, "ast.staticCall")
{
    set(kb.ids.cell, cell);
    set(kb.ids.method, method);
}

void Ast::StaticCall::addParam(Slot& slot)
{
    if (missing(kb.ids.parameters)) {
        set(kb.ids.parameters, kb.list(slot));
    } else {
        List& paramList = static_cast<List&>(get(kb.ids.parameters));
        paramList.add(slot);
    }
}

Ast::Cell::Cell(brain::Brain& kb, CellI& value) :
    BaseT<Cell>(kb, kb.type.ast.Cell)
{
    set(kb.ids.value, value);
}

Ast::Get& Ast::Cell::operator/(Base& role)
{
    return Get::New(kb, *this, role);
}

Ast::StructRef::StructRef(brain::Brain& kb, CellI& value) :
    BaseT<StructRef>(kb, kb.type.ast.StructRef)
{
    set(kb.ids.value, value);
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
    set(kb.ids.value, value);
}

Ast::Block::Block(brain::Brain& kb, List& list) :
    BaseT<Block>(kb, kb.type.ast.Block, "ast.block")
{
    set(kb.ids.asts, list);
}

Ast::Scope::Scope(brain::Brain& kb, const std::string& name) :
    BaseT<Scope>(kb, kb.type.ast.Scope, name)
{
    set(kb.ids.id, kb.id(name));
}

bool Ast::Scope::hasScope(CellI& id)
{
    if (missing(kb.ids.scopes)) {
        return false;
    }

    return scopes().hasKey(id);
}

Ast::Scope& Ast::Scope::getScope(CellI& id)
{
    if (missing(kb.ids.scopes)) {
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

    if (missing(kb.ids.scopes)) {
        set(kb.ids.scopes, *new TrieMap(kb, kb.type.Cell, kb.type.ast.Scope, "TrieMap<Cell, Type::Ast::Scope>(...)"));
    }
    if (scopes().hasKey(id)) {
        throw "Already registered!";
    }

    scope.set(kb.ids.parent, *this);
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

    if (missing(kb.ids.functions)) {
        set(kb.ids.functions, *new TrieMap(kb, kb.type.Cell, kb.type.ast.Function, "TrieMap<Cell, Type::Ast::Function>(...)"));
    }
    if (functions().hasKey(name)) {
        throw "Already registered!";
    }
    function.set(kb.ids.scope, *this);
    functions().add(name, function);
}

Ast::FunctionT& Ast::Scope::addFunctionT(const std::string& name)
{
    return addFunctionT(kb.id(name), name);
}

Ast::FunctionT& Ast::Scope::addFunctionT(CellI& id, const std::string& label)
{
    if (missing(kb.ids.functionTs)) {
        set(kb.ids.functionTs, *new Map(kb, kb.type.Cell, kb.type.ast.FunctionT, "Map<Cell, Type::Ast::FunctionT>(...)"));
    }
    if (functionTs().hasKey(id)) {
        throw "Already registered!";
    }
    auto& functionT = *new Ast::FunctionT(kb, id, label);
    functionTs().add(id, functionT);
    functionT.set(kb.ids.scope, *this);

    return functionT;
}

bool Ast::Scope::hasVariable(CellI& id)
{
    if (missing(kb.ids.variables)) {
        if (missing(kb.ids.parent)) {
            return false;
        }
    } else if (variables().hasKey(id)) {
        return true;
    }

    if (missing(kb.ids.parent)) {
        return false;
    } else {
        return static_cast<Scope&>(get(kb.ids.parent)).hasVariable(id);
    }
}

Ast::Var& Ast::Scope::getVariable(CellI& id)
{
    if (missing(kb.ids.variables)) {
        if (missing(kb.ids.parent)) {
            throw "No such variable!";
        }
    } else if (variables().hasKey(id)) {
        return static_cast<Ast::Var&>(variables().getValue(id));
    }

    if (missing(kb.ids.parent)) {
        throw "No such variable!";
    } else {
        return static_cast<Scope&>(get(kb.ids.parent)).getVariable(id);
    }
}

Ast::Var& Ast::Scope::addVariable(CellI& id)
{
    if (missing(kb.ids.variables)) {
        set(kb.ids.variables, *new Map(kb, kb.type.Cell, kb.type.ast.Var, "Map<Cell, Type::Ast::Var>(...)"));
    }
    if (variables().hasKey(id)) {
        throw "Already registered!";
    }
    auto& var = *new Ast::Var(kb, id);
    variables().add(id, var);
    var.set(kb.ids.scope, *this);

    return var;
}

bool Ast::Scope::hasStruct(CellI& id)
{
    if (missing(kb.ids.structs)) {
        if (missing(kb.ids.parent)) {
            return false;
        }
    } else if (structs().hasKey(id)) {
        return true;
    }

    if (missing(kb.ids.parent)) {
        return false;
    } else {
        return static_cast<Scope&>(get(kb.ids.parent)).hasStruct(id);
    }
}

Ast::Struct& Ast::Scope::getStruct(CellI& id)
{
    if (missing(kb.ids.structs)) {
        if (missing(kb.ids.parent)) {
            throw "No such struct";
        }
    } else if (structs().hasKey(id)) {
        return static_cast<Ast::Struct&>(structs().getValue(id));
    }

    if (missing(kb.ids.parent)) {
        throw "No such struct";
    } else {
        return static_cast<Scope&>(get(kb.ids.parent)).getStruct(id);
    }
}

void Ast::Scope::addStruct(Struct& struct_)
{
    CellI& id = struct_[kb.ids.id];

    if (missing(kb.ids.structs)) {
        set(kb.ids.structs, *new TrieMap(kb, kb.type.Cell, kb.type.ast.Struct, "TrieMap<Cell, Type::Ast::Struct>(...)"));
    }
    if (structs().hasKey(id)) {
        throw "Already registered!";
    }
    structs().add(id, struct_);
    struct_.set(kb.ids.scope, *this);
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
    if (missing(kb.ids.structTs)) {
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
    if (missing(kb.ids.structTs)) {
        set(kb.ids.structTs, *new TrieMap(kb, kb.type.Cell, kb.type.ast.StructT, "TrieMap<Cell, Type::Ast::StructT>(...)"));
    }
    if (structTs().hasKey(id)) {
        throw "Already registered!";
    }
    auto& structT = *new Ast::StructT(kb, id, name);
    structTs().add(id, structT);
    structT.set(kb.ids.scope, *this);

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

    while (currentScope->has(kb.ids.parent)) {
        currentScope = &static_cast<Scope&>(currentScope->get(kb.ids.parent));
    }

    return currentScope;
}

CellI& Ast::Scope::compile()
{
    auto& program     = *new Object(kb, kb.type.Program, "Program");
    auto& programData = *new Object(kb, kb.type.ScopeData, "ProgramData");
    program.set(kb.ids.data, programData);

    auto& resolvedScope = *new Ast::Scope(kb, label());
    set(kb.ids.resolvedScope, resolvedScope);

    auto& compileState      = *new Object(kb, kb.type.CompileState, "CompileState");
    auto& compiledFunctions = *new TrieMap(kb, kb.type.Cell, kb.type.op.Function, "Functions");
    auto& compiledStructs   = *new TrieMap(kb, kb.type.Cell, kb.type.Type_, "Types");

    auto& functions         = *new List(kb, kb.type.op.Function, "Functions");
    auto& structs           = *new TrieMap(kb, kb.type.Cell, kb.type.Type_, "structs");
    auto& unknownStructs    = *new TrieMap(kb, kb.type.Cell, kb.type.Type_, "unknownStructs");
    auto& instances         = *new TrieMap(kb, kb.type.Cell, kb.type.Type_, "instances");
    auto& unknownInstances  = *new TrieMap(kb, kb.type.Cell, kb.type.Type_, "unknownInstances");
    auto& compiledVariables = *new TrieMap(kb, kb.type.Cell, kb.type.op.Var, "Variables");

    programData.set(kb.ids.functions, compiledFunctions);
    programData.set(kb.ids.structs, compiledStructs);
    programData.set(kb.ids.variables, compiledVariables);

    compileState.set(kb.ids.functions, functions);
    compileState.set(kb.ids.structs, structs);
    compileState.set(kb.ids.unknownStructs, unknownStructs);
    compileState.set(kb.ids.instances, instances);
    compileState.set(kb.ids.unknownInstances, unknownInstances);
    compileState.set(kb.ids.variables, compiledVariables);
    compileState.set(kb.ids.resolvedScope, resolvedScope);
    compileState.set(kb.id("globalScope"), *this);
    compileState.set(kb.id("globalResolvedScope"), resolvedScope);

    resolveTypes(compileState);

    Visitor::visitList(unknownStructs[kb.ids.list], [this](CellI& unknownStruct, int i, bool& stop) {
        std::cout << "unknown struct: " << unknownStruct[kb.ids.value].label() << std::endl;
    });
    int instantiedNum = 0;
    Visitor::visitList(unknownInstances[kb.ids.list], [this, &compileState, &instantiedNum](CellI& unknownInstance, int i, bool& stop) {
        auto& unknownInstanceId = unknownInstance[kb.ids.id];
        std::cout << "unknown instance: " << unknownInstanceId.label() << std::endl;
        if (unknownInstance.has(kb.ids.currentStruct)) {
            std::cout << "     from struct: " << unknownInstance[kb.ids.currentStruct].label() << std::endl;
        }
        if (unknownInstance.has(kb.ids.currentFn)) {
            std::cout << "   from function: " << unknownInstance[kb.ids.currentFn].label() << std::endl;
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
        std::cout << ss.str() << "\n" << std::endl;
        if (unknownInstanceId.label() == "ListItem<objectType=Slot>") {
            std::cout << "";
        }

        auto& scope = static_cast<Scope&>(unknownInstance[kb.ids.scope]);
        auto& resolvedScope    = static_cast<Scope&>(unknownInstance[kb.ids.resolvedScope]);
        auto& idScope          = unknownInstance.has(kb.id("idScope")) ? static_cast<Scope&>(unknownInstance[kb.id("idScope")]) : scope;
        auto& resolvedIdScope  = static_cast<Scope&>(idScope[kb.ids.resolvedScope]);
        compileState.set(kb.ids.scope, idScope);
        auto& structT          = idScope.getStructT(templateId);
        auto& instantiedStruct = structT.instantiateWith(resolvedIdScope, static_cast<List&>(templateParams), compileState);
        auto& resolvedStruct   = instantiedStruct.resolveTypes(compileState);
        resolvedIdScope.addStruct(resolvedStruct);
        instantiedNum = i + 1;
    });
    if (unknownStructs.size() > 0 || unknownInstances.size() != instantiedNum) {
        throw "Referencing to an unknown type!";
    }
    compileState.set(kb.ids.scope, *this);
    compileState.set(kb.ids.resolvedScope, resolvedScope);

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

    if (scope.has(kb.ids.functions)) {
        Visitor::visitList(resolvedScope.functions()[kb.ids.list], [this, &state, &compiledFunctions](CellI& function, int i, bool& stop) {
            Ast::Function& astFunction = static_cast<Ast::Function&>(function);
            auto& compiledFunction     = astFunction.compile(state);
            compiledFunctions.add(astFunction[kb.ids.name], compiledFunction);
        });
    }
    if (scope.has(kb.ids.structs)) {
        Visitor::visitList(resolvedScope.structs()[kb.ids.list], [this, &state, &compiledStructs](CellI& struct_, int i, bool& stop) {
            Ast::Struct& astStruct = static_cast<Ast::Struct&>(struct_);
            auto& compiledStruct = astStruct.compile(state);
            compiledStructs.add(astStruct.id(), compiledStruct);
        });
    }
    if (scope.has(kb.ids.variables)) {
        Visitor::visitList(resolvedScope.variables()[kb.ids.list], [this, &compiledVariables](CellI& var, int i, bool& stop) {
            Ast::Var& astVar = static_cast<Ast::Var&>(var);
            auto& varName    = astVar[kb.ids.role];
            auto& compiledVariable = *new Object(kb, kb.type.op.Var);
            compiledVariables.add(varName, compiledVariable);
        });
    }
    if (scope.has(kb.ids.scopes)) {
        Visitor::visitList(scopes()[kb.ids.list], [this, &programData, &state, &resolvedScope](CellI& scope, int i, bool& stop) {
            Ast::Scope& astScope = static_cast<Ast::Scope&>(scope);
            state.set(kb.ids.scope, astScope);
            state.set(kb.ids.resolvedScope, resolvedScope.getScope(astScope[kb.ids.id]));
            astScope.compileTheResolvedAsts(programData, state);
            state.set(kb.ids.scope, *this);
            state.set(kb.ids.resolvedScope, resolvedScope);
        });
    }
}

void Ast::Scope::resolveTypes(CellI& state)
{
    state.set(kb.ids.scope, *this);
    Ast::Scope& resolvedScope = static_cast<Ast::Scope&>(state[kb.ids.resolvedScope]);

    if (has(kb.ids.functions)) {
        state.erase(kb.ids.currentStruct);
        Visitor::visitList(functions()[kb.ids.list], [this, &state, &resolvedScope](CellI& origAstFunctionCell, int i, bool& stop) {
            Ast::Function& origAstFunction = static_cast<Ast::Function&>(origAstFunctionCell);
            Ast::Function& resolvedAstFunction = origAstFunction.resolveTypes(state);
            resolvedScope.addFunction(resolvedAstFunction);
            std::cout << resolvedAstFunction.label() << std::endl;
        });
    }
    if (has(kb.ids.structs)) {
        Visitor::visitList(structs()[kb.ids.list], [this, &state, &resolvedScope](CellI& origAstStructCell, int i, bool& stop) {
            Ast::Struct& origAstStruct = static_cast<Ast::Struct&>(origAstStructCell);
            Ast::Struct& resolvedAstStruct = origAstStruct.resolveTypes(state);
            resolvedScope.addStruct(resolvedAstStruct);
        });
    }
    if (has(kb.ids.variables)) {
        Visitor::visitList(variables()[kb.ids.list], [this, &state, &resolvedScope](CellI& origAstVarCell, int i, bool& stop) {
            Ast::Var& origAstVar = static_cast<Ast::Var&>(origAstVarCell);
            auto& varName        = origAstVarCell[kb.ids.role];
            // TODO
            // addVariable();
        });
    }
    if (has(kb.ids.scopes)) {
        Visitor::visitList(scopes()[kb.ids.list], [this, &state, &resolvedScope](CellI& origAstScopeCell, int i, bool& stop) {
            Ast::Scope& origAstScope = static_cast<Ast::Scope&>(origAstScopeCell);
            auto& newResolvedScope   = *new Ast::Scope(kb, origAstScope.label());
            origAstScope.set(kb.ids.resolvedScope, newResolvedScope);
            resolvedScope.addScope(newResolvedScope);
            state.set(kb.ids.resolvedScope, newResolvedScope);
            origAstScope.resolveTypes(state);
            state.set(kb.ids.scope, *this);
            state.set(kb.ids.resolvedScope, resolvedScope);
        });
    }
}

TrieMap& Ast::Scope::variables()
{
    if (missing(kb.ids.variables)) {
        throw "No variables!";
    } else {
        return static_cast<TrieMap&>(get(kb.ids.variables));
    }
}

TrieMap& Ast::Scope::scopes()
{
    if (missing(kb.ids.scopes)) {
        throw "No scopes!";
    } else {
        return static_cast<TrieMap&>(get(kb.ids.scopes));
    }
}

TrieMap& Ast::Scope::functions()
{
    if (missing(kb.ids.functions)) {
        throw "No functions!";
    } else {
        return static_cast<TrieMap&>(get(kb.ids.functions));
    }
}

TrieMap& Ast::Scope::functionTs()
{
    if (missing(kb.ids.functionTs)) {
        throw "No functions!";
    } else {
        return static_cast<TrieMap&>(get(kb.ids.functionTs));
    }
}

TrieMap& Ast::Scope::structs()
{
    if (missing(kb.ids.structs)) {
        throw "No structs!";
    } else {
        return static_cast<TrieMap&>(get(kb.ids.structs));
    }
}

TrieMap& Ast::Scope::structTs()
{
    if (missing(kb.ids.structTs)) {
        throw "No structs!";
    } else {
        return static_cast<TrieMap&>(get(kb.ids.structTs));
    }
}

Ast::StructBase::StructBase(brain::Brain& kb, CellI& astType, CellI& id, const std::string& label) :
    Base(kb, astType, label)
{
    set(kb.ids.id, id);
}

Ast::StructBase::StructBase(brain::Brain& kb, CellI& astType, const std::string& name) :
    Base(kb, astType, name)
{
    set(kb.ids.id, kb.id(name));
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

    if (missing(kb.ids.methods)) {
        set(kb.ids.methods, *new Map(kb, kb.type.Cell, kb.type.ast.Function, "Map<Cell, Type::Ast::Function>(...)"));
    }
    if (methods().hasKey(name)) {
        throw "Already registered!";
    }
    method.set(kb.ids.structType, *this);
    methods().add(name, method);
}

void Ast::StructBase::members(Slot& param)
{
    if (missing(kb.ids.members)) {
        set(kb.ids.members, *new List(kb, kb.type.ast.Slot));
    }
    members().add(param);
}

void Ast::StructBase::subTypes(Slot& param)
{
    if (missing(kb.ids.subTypes)) {
        set(kb.ids.subTypes, *new List(kb, kb.type.ast.Slot));
    }
    subTypes().add(param);
}

void Ast::StructBase::memberOf(CellI& type)
{
    if (missing(kb.ids.memberOf)) {
        set(kb.ids.memberOf, *new List(kb, kb.type.Type_));
    }
    memberOf().add(type);
}

Map& Ast::StructBase::methods()
{
    if (missing(kb.ids.methods)) {
        throw "No methods!";
    } else {
        return static_cast<Map&>(get(kb.ids.methods));
    }
}

List& Ast::StructBase::members()
{
    if (missing(kb.ids.members)) {
        throw "No members!";
    } else {
        return static_cast<List&>(get(kb.ids.members));
    }
}

List& Ast::StructBase::subTypes()
{
    if (missing(kb.ids.subTypes)) {
        throw "No subTypes!";
    } else {
        return static_cast<List&>(get(kb.ids.subTypes));
    }
}

List& Ast::StructBase::memberOf()
{
    if (missing(kb.ids.memberOf)) {
        throw "No memberOf!";
    } else {
        return static_cast<List&>(get(kb.ids.memberOf));
    }
}

CellI& Ast::StructBase::id()
{
    return get(kb.ids.id);
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
    auto& structId       = get(kb.ids.id);
    auto& structs        = static_cast<TrieMap&>(state[kb.ids.structs]);
    auto& unknownStructs = static_cast<TrieMap&>(state[kb.ids.unknownStructs]);
    Ast::Struct& ret     = *new Ast::Struct(kb, structId, std::format("{}", label()));

    if (has(kb.ids.instanceOf)) {
        ret.set(kb.ids.instanceOf, get(kb.ids.instanceOf));
    }

    if (unknownStructs.hasKey(structId)) {
        unknownStructs.remove(structId);
    }
    auto& resolvedStruct = *new Object(kb, kb.type.Type_, std::format("{}", structId.label()));
    structs.add(structId, resolvedStruct);

    state.set(kb.ids.currentStruct, ret);

    std::stringstream ss;
    ss << std::format("struct {}", label());

    // resolve memberOf list
    if (has(kb.ids.memberOf)) {
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
    std::cout << ss.str() << " {" << std::endl;

    // resolve sub types
    if (has(kb.ids.subTypes)) {
        Visitor::visitList(subTypes(), [this, &ret, &state](CellI& subTypeCell, int i, bool& stop) {
            CellI& subTypeId           = subTypeCell[kb.ids.slotRole];
            CellI& subTypeType         = subTypeCell[kb.ids.slotType];
            CellI& resolvedSubTypeType = resolveType(subTypeType, state);
            ret.subTypes(kb.ast.slot(subTypeId, resolvedSubTypeType));
            std::cout << std::format("    alias {} = {};", subTypeId.label(), resolvedSubTypeType.label()) << std::endl;
        });
        if (has(kb.ids.methods) || has(kb.ids.members)) {
            std::cout << std::endl;
        }
    }

    // resolve methods
    if (has(kb.ids.methods)) {
        Visitor::visitList(methods()[kb.ids.list], [this, &ret, &state](CellI& origAstFunctionCell, int i, bool& stop) {
            auto& origAstFunction = static_cast<Ast::Function&>(origAstFunctionCell);
            auto& resolvedAstFunction = origAstFunction.resolveTypes(state);
            ret.addMethod(resolvedAstFunction);
            std::cout << std::format("    {};\n", resolvedAstFunction.shortName());
        });
        if (has(kb.ids.members)) {
            std::cout << std::endl;
        }
    }
    // resolve members
    if (has(kb.ids.members)) {
        Visitor::visitList(members(), [this, &ret, &state](CellI& memberCell, int i, bool& stop) {
            CellI& memberId   = memberCell[kb.ids.slotRole];
            CellI& memberType = memberCell[kb.ids.slotType];
            CellI& resolvedMemberType = resolveType(memberType, state);
            ret.members(kb.ast.slot(memberId, resolvedMemberType));
            std::cout << std::format("    {}: {};", memberId.label(), resolvedMemberType.label()) << std::endl;
        });
    }

    std::cout << "}" << std::endl;

    return ret;
}

CellI& Ast::Struct::compile(CellI& state)
{
    auto& structId        = get(kb.ids.id);
    CellI& compiledStruct = getResolvedTypeById(structId, has(kb.ids.instanceOf), state);
    // std::cout << std::format("DDDD compile {} resolved at {:p}\n", structId.label(), (void*)&compiledStruct) << std::endl;

    // compile methods
    if (has(kb.ids.methods)) {
        Map& compiledMethods = *new Map(kb, kb.type.Cell, kb.type.ast.Function);
        Visitor::visitList(methods()[kb.ids.list], [this, &compiledMethods, &state](CellI& astFunction, int i, bool& stop) {
            auto& compiledFunction = static_cast<Ast::Function&>(astFunction).compile(state);
            compiledMethods.add(astFunction[kb.ids.name], compiledFunction);
        });
        compiledStruct.set(kb.ids.methods, compiledMethods);
    }

    // compile members
    if (has(kb.ids.members)) {
        Map& compiledMembers = *new Map(kb, kb.type.Cell, kb.type.Slot, "members Map<Cell, Slot>(...)");
        Visitor::visitList(members(), [this, &compiledMembers, &compiledStruct, &state](CellI& slot, int i, bool& stop) {
            CellI& slotRole          = slot[kb.ids.slotRole];
            CellI& slotType          = slot[kb.ids.slotType];
            compiledMembers.add(slotRole, kb.type.slot(slotRole, slotType));
        });
        compiledStruct.set(kb.ids.slots, compiledMembers);
    }

    // compile sub types
    if (has(kb.ids.subTypes)) {
        Map& compiledSubTypes = *new Map(kb, kb.type.Cell, kb.type.Type_, "subTypes Map<Cell, Type>(...)");
        Visitor::visitList(subTypes(), [this, &compiledSubTypes](CellI& slot, int i, bool& stop) {
            CellI& slotRole          = slot[kb.ids.slotRole];
            CellI& slotType          = slot[kb.ids.slotType];
            compiledSubTypes.add(slotRole, slotType);
        });
        compiledStruct.set(kb.ids.subTypes, compiledSubTypes);
    }

    // compile memberOf list
    if (has(kb.ids.memberOf)) {
        Map& compiledMemberOfs = *new Map(kb, kb.type.Type_, kb.type.Type_, "memberOf Map<Type, Type>(...)");
        Visitor::visitList(memberOf(), [this, &compiledMemberOfs](CellI& membershipType, int i, bool& stop) {
            compiledMemberOfs.add(membershipType, membershipType);
        });
        compiledStruct.set(kb.ids.memberOf, compiledMemberOfs);
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
    if (missing(kb.ids.templateParams)) {
        set(kb.ids.templateParams, *new Map(kb, kb.type.Cell, kb.type.Type_));
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
}

Ast::Struct& Ast::StructT::instantiateWith(Scope& scope, List& inputParams, CellI& state)
{
    // process input parameters
    if (!inputParams.empty() && missing(kb.ids.templateParams)) {
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
    idCell.label(std::format("id of {}<{}>", id().label(), ss.str()));
    Ast::Struct& ret = *new Ast::Struct(kb, idCell, std::format("{}<{}>", id().label(), ss.str()));
    ret.set(kb.ids.instanceOf, *this);

    // instantiate methods
    if (has(kb.ids.methods)) {
        Visitor::visitList(methods()[kb.ids.list], [this, &inputParameters, &ret, &state](CellI& astFunction, int i, bool& stop) {
            Ast::Function& instantiedFunction = *new Function(kb, astFunction[kb.ids.name], astFunction[kb.ids.name].label());
            ret.addMethod(instantiedFunction);

            // parameters
            if (astFunction.has(kb.ids.parameters)) {
                List& instantiatedParameters = *new List(kb, kb.type.Slot);
                Visitor::visitList(astFunction[kb.ids.parameters], [this, &inputParameters, &instantiatedParameters, &ret, &state](CellI& slot, int i, bool& stop) {
                    CellI& slotRole          = slot[kb.ids.slotRole];
                    CellI& slotType          = slot[kb.ids.slotType];
                    CellI& instantiatedParam = instantiateTemplateParamType(slotType, ret, inputParameters, state);
                    instantiatedParameters.add(kb.ast.slot(slotRole, instantiatedParam));
                });
                instantiedFunction.set(kb.ids.parameters, instantiatedParameters);
            }
            // return type
            if (astFunction.has(kb.ids.returnType)) {
                CellI& returnType             = astFunction[kb.ids.returnType];
                CellI& instantiatedReturnType = instantiateTemplateParamType(returnType, ret, inputParameters, state);
                instantiedFunction.set(kb.ids.returnType, instantiatedReturnType);
            }
            // code
            if (astFunction.has(kb.ids.code)) {
                instantiedFunction.set(kb.ids.code, instantiateAst(astFunction[kb.ids.code], ret, inputParameters, state));
            }
        });
    }

    // instantiate members
    if (has(kb.ids.members)) {
        List& instantiatedMembers = *new List(kb, kb.type.Slot);
        Visitor::visitList(members(), [this, &inputParameters, &instantiatedMembers, &ret, &state](CellI& slot, int i, bool& stop) {
            CellI& slotRole = slot[kb.ids.slotRole];
            CellI& slotType = slot[kb.ids.slotType];
            CellI& instantiatedParam = instantiateTemplateParamType(slotType, ret, inputParameters, state);
            instantiatedMembers.add(kb.ast.slot(slotRole, instantiatedParam));
        });
        ret.set(kb.ids.members, instantiatedMembers);
    }

    // instantiate sub types
    if (has(kb.ids.subTypes)) {
        List& instantiatedSubTypes = *new List(kb, kb.type.Type_);
        Visitor::visitList(subTypes(), [this, &inputParameters, &instantiatedSubTypes, &ret, &state](CellI& slot, int i, bool& stop) {
            CellI& slotRole = slot[kb.ids.slotRole];
            CellI& slotType = slot[kb.ids.slotType];
            CellI& instantiatedParam = instantiateTemplateParamType(slotType, ret, inputParameters, state);
            instantiatedSubTypes.add(kb.ast.slot(slotRole, instantiatedParam));
        });
        ret.set(kb.ids.subTypes, instantiatedSubTypes);
    }

    // instantiate memberOf list
    if (has(kb.ids.memberOf)) {
        List& instantiatedMemberOfs = *new List(kb, kb.type.Type_);
        Visitor::visitList(memberOf(), [this, &inputParameters, &instantiatedMemberOfs, &ret, &state](CellI& membershipType, int i, bool& stop) {
            CellI& instantiatedParam = instantiateTemplateParamType(membershipType, ret, inputParameters, state);
            instantiatedMemberOfs.add(instantiatedParam);
        });
        ret.set(kb.ids.memberOf, instantiatedMemberOfs);
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
    if (&param.type() == &kb.type.ast.Cell || &param.type() == &kb.type.ast.StructRef) {
        return param;
    }

    throw "Unknown template parameter!";
}

Ast::Base& Ast::StructT::instantiateAst(CellI& ast, CellI& selfType, Map& inputParameters, CellI& state)
{
    auto& ids = kb.ids;

    auto instantiate = [this, &selfType, &inputParameters, &state](CellI& ast) -> Ast::Base& { return instantiateAst(ast, selfType, inputParameters, state); };

    if (&ast.type() == &kb.type.ast.New) {
        auto* objectTypePtr = &ast[ids.objectType];
        if (&(*objectTypePtr).type() == &kb.type.ast.TemplatedType) {
            CellI& resolvedObjectType = instantiateTemplateParamType(*objectTypePtr, selfType, inputParameters, state);
            objectTypePtr             = &resolvedObjectType;
        }
        auto& objectType = *static_cast<Base*>(objectTypePtr);
        if (ast.has(ids.constructor)) {
            auto& constructor = ast[ids.constructor];
            Ast::Base& ret    = kb.ast.new_(objectType, static_cast<Base&>(constructor));
            if (ast.has(ids.parameters)) {
                auto& newParameters = *new cells::List(kb, kb.type.ast.Slot);
                Visitor::visitList(ast[ids.parameters], [this, &newParameters, &instantiate](CellI& slot, int, bool&) {
                    newParameters.add(kb.ast.slot(slot[kb.ids.slotRole], instantiate(slot[kb.ids.slotType])));
                });
                ret.set(ids.parameters, newParameters);
            }
            return ret;
        } else {
            return kb.ast.new_(objectType);
        }
    } else if (&ast.type() == &kb.type.ast.Call) {
        Ast::Base& ret = kb.ast.call(instantiate(ast[ids.cell]), ast[ids.method]);
        if (ast.has(ids.parameters)) {
            // TODO process parameters
            ret.set(ids.parameters, ast[ids.parameters]);
        }
        return ret;
    } else if (&ast.type() == &kb.type.ast.StaticCall) {
        Ast::Base& ret = kb.ast.scall(instantiate(ast[ids.cell]), ast[ids.method]);
        if (ast.has(ids.parameters)) {
            // TODO process parameters
            ret.set(ids.parameters, ast[ids.parameters]);
        }
        return ret;
    } 

    // do nothing just traverse and copy the AST nodes
    if (&ast.type() == &kb.type.ast.Block) {
        auto& instantiedAsts = *new cells::List(kb, kb.type.ast.Base);
        Visitor::visitList(ast[ids.asts], [this, &instantiedAsts, &instantiate](CellI& ast, int, bool&) {
            instantiedAsts.add(instantiate(ast));
        });
        return *new Block(kb, instantiedAsts);
    } else if (&ast.type() == &kb.type.ast.Cell) {
        return kb.ast.cell(ast[ids.value]);
    } else if (&ast.type() == &kb.type.ast.StructRef) {
        return kb.ast.structRef(ast[ids.value]);
    } else if (&ast.type() == &kb.type.ast.SelfFn) {
        return kb.ast.selfFn();
    } else if (&ast.type() == &kb.type.ast.Self) {
        return kb.ast.self();
    } else if (&ast.type() == &kb.type.ast.Parameter) {
        return kb.ast.parameter(ast[ids.role]);
    } else if (&ast.type() == &kb.type.ast.Var) {
        return kb.ast.var(ast[ids.role]);
    } else if (&ast.type() == &kb.type.ast.Delete) {
        return kb.ast.delete_(instantiate(ast[ids.cell]));
    } else if (&ast.type() == &kb.type.ast.Set) {
        return kb.ast.set(instantiate(ast[ids.cell]), instantiate(ast[ids.role]), instantiate(ast[ids.value]));
    } else if (&ast.type() == &kb.type.ast.Erase) {
        return kb.ast.erase(instantiate(ast[ids.cell]), instantiate(ast[ids.role]));
    } else if (&ast.type() == &kb.type.ast.If) {
        if (ast.has(ids.else_)) {
            return kb.ast.if_(instantiate(ast[ids.condition]), instantiate(ast[ids.then]), instantiate(ast[ids.else_]));
        } else {
            return kb.ast.if_(instantiate(ast[ids.condition]), instantiate(ast[ids.then]));
        }
    } else if (&ast.type() == &kb.type.ast.Do) {
        return kb.ast.do_(instantiate(ast[ids.statement]), instantiate(ast[ids.condition]));
    } else if (&ast.type() == &kb.type.ast.While) {
        return kb.ast.while_(instantiate(ast[ids.condition]), instantiate(ast[ids.statement]));
    } else if (&ast.type() == &kb.type.ast.And) {
        return kb.ast.and_(instantiate(ast[ids.lhs]), instantiate(ast[ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.Or) {
        return kb.ast.or_(instantiate(ast[ids.lhs]), instantiate(ast[ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.Not) {
        return kb.ast.not_(instantiate(ast[ids.input]));
    } else if (&ast.type() == &kb.type.ast.Add) {
        return kb.ast.add(instantiate(ast[ids.lhs]), instantiate(ast[ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.Subtract) {
        return kb.ast.subtract(instantiate(ast[ids.lhs]), instantiate(ast[ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.Multiply) {
        return kb.ast.multiply(instantiate(ast[ids.lhs]), instantiate(ast[ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.Divide) {
        return kb.ast.divide(instantiate(ast[ids.lhs]), instantiate(ast[ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.LessThan) {
        return kb.ast.lessThan(instantiate(ast[ids.lhs]), instantiate(ast[ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.LessThanOrEqual) {
        return kb.ast.lessThanOrEqual(instantiate(ast[ids.lhs]), instantiate(ast[ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.GreaterThan) {
        return kb.ast.greaterThan(instantiate(ast[ids.lhs]), instantiate(ast[ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.GreaterThanOrEqual) {
        return kb.ast.greaterThanOrEqual(instantiate(ast[ids.lhs]), instantiate(ast[ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.Same) {
        return kb.ast.same(instantiate(ast[ids.lhs]), instantiate(ast[ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.NotSame) {
        return kb.ast.notSame(instantiate(ast[ids.lhs]), instantiate(ast[ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.Equal) {
        return kb.ast.equal(instantiate(ast[ids.lhs]), instantiate(ast[ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.NotEqual) {
        return kb.ast.notEqual(instantiate(ast[ids.lhs]), instantiate(ast[ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.Has) {
        return kb.ast.has(instantiate(ast[ids.cell]), instantiate(ast[ids.role]));
    } else if (&ast.type() == &kb.type.ast.Missing) {
        return kb.ast.missing(instantiate(ast[ids.cell]), instantiate(ast[ids.role]));
    } else if (&ast.type() == &kb.type.ast.Get) {
        return kb.ast.get(instantiate(ast[ids.cell]), instantiate(ast[ids.role]));
    } else if (&ast.type() == &kb.type.ast.Member) {
        return kb.ast.member(ast[ids.role]);
    } else if (&ast.type() == &kb.type.ast.Return) {
        if (ast.has(ids.value)) {
            return kb.ast.return_(static_cast<Base&>(instantiate(ast[ids.value])));
        }
        return kb.ast.return_();
    }

    throw "Unknown AST to instantiate!";
}

Map& Ast::StructT::templateParams()
{
    if (missing(kb.ids.templateParams)) {
        throw "No templateParams!";
    } else {
        return static_cast<Map&>(get(kb.ids.templateParams));
    }
}

Ast::Function::Function(brain::Brain& kb, CellI& name, const std::string& label) :
    BaseT<Function>(kb, kb.type.ast.Function, label)
{
    set(kb.ids.name, name);
    this->label(label);
}

Ast::Function::Function(brain::Brain& kb, const std::string& name) :
    BaseT<Function>(kb, kb.type.ast.Function, name)
{
    set(kb.ids.id, kb.pools.strings.get(name)[kb.ids.value]);
    label(name);
}

void Ast::Function::parameters(Slot& param)
{
    if (missing(kb.ids.parameters)) {
        set(kb.ids.parameters, *new List(kb, kb.type.Slot));
    }
    parameters().add(param);
}

void Ast::Function::returnType(CellI& type)
{
    set(kb.ids.returnType, type);
}

void Ast::Function::addBlock(Block& block)
{
    set(kb.ids.code, block);
}

Ast::Function& Ast::Function::resolveTypes(CellI& state)
{
    Ast::Function& ret = *new Ast::Function(kb, get(kb.ids.name), std::format("{}", label()));
    state.set(kb.ids.currentFn, ret);
    std::stringstream ss;

    if (has(kb.ids.static_)) {
        ret.set(kb.ids.static_, get(kb.ids.static_));
        ss << "static ";
    }
    ss << "fn";
    if (has(kb.ids.structType)) {
        auto& structType = get(kb.ids.structType);
        ret.set(kb.ids.structType, structType);
        ss << std::format(" {}::{}(", structType.label(), get(kb.ids.name).label());
    } else {
        ss << std::format(" {}(", get(kb.ids.name).label());
    }

    if (has(kb.ids.parameters)) {
        Visitor::visitList(parameters(), [this, &state, &ret, &ss](CellI& param, int i, bool& stop) {
            CellI& paramId           = param[kb.ids.slotRole];
            CellI& paramType         = param[kb.ids.slotType];
            CellI& resolvedParamType = resolveType(paramType, state);
            if (i > 0) {
                ss << ", ";
            }
            ss << std::format("{}: {}", paramId.label(), resolvedParamType.label());
            ret.parameters(kb.ast.slot(paramId, resolvedParamType));
        });
    }
    ss << ")";
    if (has(kb.ids.returnType)) {
        ss << " -> ";
        CellI& retType = returnType();
        CellI& resolvedRetType = resolveType(retType, state);
        ss << resolvedRetType.label();
        ret.returnType(resolvedRetType);
    }
    ret.label(ss.str());

    CellI& resolvedCodeAst = resolveTypesInCode(state, code());
    ret.set(kb.ids.code, resolvedCodeAst);

    return ret;
}

Ast::Base& Ast::Function::resolveTypesInCode(CellI& resolveState, CellI& ast)
{
    auto& ids = kb.ids;

    auto resolveNode = [this, &resolveState](CellI& ast) -> Ast::Base& { return resolveTypesInCode(resolveState, ast); };

    if (&ast.type() == &kb.type.ast.New) {
        CellI& objectType             = ast[ids.objectType];
        Ast::Base* resolvedObjectTypePtr = nullptr;
        if (&objectType.type() == &kb.type.ast.Member) {
            resolvedObjectTypePtr = &static_cast<Ast::Base&>(objectType);
        } else {
            resolvedObjectTypePtr = &kb.ast.cell(resolveType(objectType, resolveState));
        }
        Ast::Base& resolvedObjectType = *resolvedObjectTypePtr;

        if (ast.has(ids.constructor)) {
            auto& constructor = ast[ids.constructor];
            Ast::Base& ret = kb.ast.new_(resolvedObjectType, static_cast<Base&>(ast[ids.constructor]));

            if (ast.has(ids.parameters)) {
                auto& newParameters = *new cells::List(kb, kb.type.ast.Slot);
                Visitor::visitList(ast[ids.parameters], [this, &newParameters, &resolveNode, &resolveState](CellI& slot, int, bool&) {
                    newParameters.add(kb.ast.slot(slot[kb.ids.slotRole], resolveNode(slot[kb.ids.slotType])));
                });
                ret.set(ids.parameters, newParameters);
            }
            return ret;
        } else {
            return kb.ast.new_(resolvedObjectType);
        }
    }

    if (&ast.type() == &kb.type.ast.Call || &ast.type() == &kb.type.ast.StaticCall) {

        Ast::Base* retPtr = nullptr;
        if (&ast.type() == &kb.type.ast.Call) {
            retPtr = &kb.ast.call(resolveNode(ast[ids.cell]), ast[ids.method]);
        } else {
            retPtr = &kb.ast.scall(kb.ast.cell(resolveType(ast[ids.cell], resolveState)), ast[ids.method]);
        }
        Ast::Base& ret = *retPtr;
        if (ast.has(ids.parameters)) {
            auto& newParameters = *new cells::List(kb, kb.type.ast.Slot);
            Visitor::visitList(ast[ids.parameters], [this, &newParameters, &resolveNode, &resolveState](CellI& slot, int, bool&) {
                newParameters.add(kb.ast.slot(slot[kb.ids.slotRole], resolveNode(slot[kb.ids.slotType])));
            });
            ret.set(ids.parameters, newParameters);
        }
        return ret;
    }

    // do nothing just traverse and copy the AST nodes
    if (&ast.type() == &kb.type.ast.Block) {
        auto& instantiedAsts = *new cells::List(kb, kb.type.ast.Base);
        Visitor::visitList(ast[ids.asts], [this, &instantiedAsts, &resolveNode](CellI& ast, int, bool&) {
            instantiedAsts.add(resolveNode(ast));
        });
        return *new Block(kb, instantiedAsts);
    } else if (&ast.type() == &kb.type.ast.Cell) {
        return kb.ast.cell(ast[ids.value]);
    } else if (&ast.type() == &kb.type.ast.StructRef) {
        return kb.ast.structRef(ast[ids.value]);
    } else if (&ast.type() == &kb.type.ast.SelfFn) {
        return kb.ast.selfFn();
    } else if (&ast.type() == &kb.type.ast.Self) {
        return kb.ast.self();
    } else if (&ast.type() == &kb.type.ast.Parameter) {
        return kb.ast.parameter(ast[ids.role]);
    } else if (&ast.type() == &kb.type.ast.Var) {
        return kb.ast.var(ast[ids.role]);
    } else if (&ast.type() == &kb.type.ast.Delete) {
        return kb.ast.delete_(resolveNode(ast[ids.cell]));
    } else if (&ast.type() == &kb.type.ast.Set) {
        return kb.ast.set(resolveNode(ast[ids.cell]), resolveNode(ast[ids.role]), resolveNode(ast[ids.value]));
    } else if (&ast.type() == &kb.type.ast.Erase) {
        return kb.ast.erase(resolveNode(ast[ids.cell]), resolveNode(ast[ids.role]));
    } else if (&ast.type() == &kb.type.ast.If) {
        if (ast.has(ids.else_)) {
            return kb.ast.if_(resolveNode(ast[ids.condition]), resolveNode(ast[ids.then]), resolveNode(ast[ids.else_]));
        } else {
            return kb.ast.if_(resolveNode(ast[ids.condition]), resolveNode(ast[ids.then]));
        }
    } else if (&ast.type() == &kb.type.ast.Do) {
        return kb.ast.do_(resolveNode(ast[ids.statement]), resolveNode(ast[ids.condition]));
    } else if (&ast.type() == &kb.type.ast.While) {
        return kb.ast.while_(resolveNode(ast[ids.condition]), resolveNode(ast[ids.statement]));
    } else if (&ast.type() == &kb.type.ast.And) {
        return kb.ast.and_(resolveNode(ast[ids.lhs]), resolveNode(ast[ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.Or) {
        return kb.ast.or_(resolveNode(ast[ids.lhs]), resolveNode(ast[ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.Not) {
        return kb.ast.not_(resolveNode(ast[ids.input]));
    } else if (&ast.type() == &kb.type.ast.Add) {
        return kb.ast.add(resolveNode(ast[ids.lhs]), resolveNode(ast[ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.Subtract) {
        return kb.ast.subtract(resolveNode(ast[ids.lhs]), resolveNode(ast[ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.Multiply) {
        return kb.ast.multiply(resolveNode(ast[ids.lhs]), resolveNode(ast[ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.Divide) {
        return kb.ast.divide(resolveNode(ast[ids.lhs]), resolveNode(ast[ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.LessThan) {
        return kb.ast.lessThan(resolveNode(ast[ids.lhs]), resolveNode(ast[ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.LessThanOrEqual) {
        return kb.ast.lessThanOrEqual(resolveNode(ast[ids.lhs]), resolveNode(ast[ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.GreaterThan) {
        return kb.ast.greaterThan(resolveNode(ast[ids.lhs]), resolveNode(ast[ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.GreaterThanOrEqual) {
        return kb.ast.greaterThanOrEqual(resolveNode(ast[ids.lhs]), resolveNode(ast[ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.Same) {
        return kb.ast.same(resolveNode(ast[ids.lhs]), resolveNode(ast[ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.NotSame) {
        return kb.ast.notSame(resolveNode(ast[ids.lhs]), resolveNode(ast[ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.Equal) {
        return kb.ast.equal(resolveNode(ast[ids.lhs]), resolveNode(ast[ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.NotEqual) {
        return kb.ast.notEqual(resolveNode(ast[ids.lhs]), resolveNode(ast[ids.rhs]));
    } else if (&ast.type() == &kb.type.ast.Has) {
        return kb.ast.has(resolveNode(ast[ids.cell]), resolveNode(ast[ids.role]));
    } else if (&ast.type() == &kb.type.ast.Missing) {
        return kb.ast.missing(resolveNode(ast[ids.cell]), resolveNode(ast[ids.role]));
    } else if (&ast.type() == &kb.type.ast.Get) {
        return kb.ast.get(resolveNode(ast[ids.cell]), resolveNode(ast[ids.role]));
    } else if (&ast.type() == &kb.type.ast.Member) {
        return kb.ast.member(ast[ids.role]);
    } else if (&ast.type() == &kb.type.ast.Return) {
        if (ast.has(ids.value)) {
            return kb.ast.return_(static_cast<Base&>(resolveNode(ast[ids.value])));
        }
        return kb.ast.return_();
    }

    throw "Unknown AST to instantiate!";
}

CellI& Ast::Function::compile(CellI& state)
{
    cells::Object& functionType = *new cells::Object(kb, kb.type.Type_);
    functionType.set(kb.ids.memberOf, kb.map(kb.type.Type_, kb.type.Type_, kb.type.op.Function, kb.type.op.Function));
    cells::Map& subTypesMap = kb.map(kb.type.Cell, kb.type.Type_,
                                     kb.ids.name, get(kb.ids.name));
    functionType.set(kb.ids.subTypes, subTypesMap);
    functionType.set(kb.ids.slots, kb.type.op.Function[kb.ids.slots]);

    cells::Object& function = *new cells::Object(kb, functionType);
    compileParams(function, subTypesMap, state);
    functionType.label(std::format("Type for {}", function.label()));
    function.set(kb.ids.ast, *this);
    function.set(kb.ids.op, compileAst(code(), function, state));
    if (has(kb.ids.static_)) {
        function.set(kb.ids.static_, get(kb.ids.static_));
    }

    return function;
}

std::string Ast::Function::shortName()
{
    std::stringstream iss;
    std::stringstream oss;
    if (has(kb.ids.parameters) || has(kb.ids.structType)) {
        if (has(kb.ids.parameters)) {
            Visitor::visitList(parameters(), [this, &iss](CellI& slot, int i, bool& stop) {
                if (i > 0) {
                    iss << ", ";
                }
                iss << "p_" << slot[kb.ids.slotRole].label() << ": " << slot[kb.ids.slotType].label();
            });
        }
    }
    if (has(kb.ids.returnType)) {
        oss << returnType().label();
    }
    if (has(kb.ids.returnType)) {
        return std::format("fn {}({}) -> {}", get(kb.ids.name).label(), iss.str(), oss.str());
    } else {
        return std::format("fn {}({})", get(kb.ids.name).label(), iss.str());
    }
}

void Ast::Function::compileParams(cells::Object& function, cells::Map& subTypesMap, CellI& state)
{
    std::stringstream iss;
    std::stringstream oss;
    std::string structTypeStr;
    if (has(kb.ids.parameters) || has(kb.ids.structType)) {
        cells::Object& parametersType = *new cells::Object(kb, kb.type.Type_);
        Map& slots                    = *new Map(kb, kb.type.Cell, kb.type.Slot);
        if (has(kb.ids.structType)) {
            CellI& type = get(kb.ids.structType);
            Object& var = *new Object(kb, kb.type.op.Var, "self");
            var.set(kb.ids.objectType, type);
            slots.add(kb.ids.self, kb.type.slot(kb.ids.self, type));
            structTypeStr = std::format("{}::", type.label());
        }
        if (has(kb.ids.parameters)) {
            Visitor::visitList(parameters(), [this, &slots, &iss](CellI& slot, int i, bool& stop) {
                if (i > 0) {
                    iss << ", ";
                }
                iss << "p_" << slot[kb.ids.slotRole].label() << ": " << slot[kb.ids.slotType].label();
                slots.add(slot[kb.ids.slotRole], kb.type.slot(slot[kb.ids.slotRole], slot[kb.ids.slotType]));
            });
        }
        parametersType.set(kb.ids.slots, slots);
        subTypesMap.add(kb.ids.parameters, parametersType);
    }
    if (has(kb.ids.returnType)) {
        oss << returnType().label();
        subTypesMap.add(kb.ids.returnType, returnType());
    }
    if (has(kb.ids.returnType)) {
        function.label(std::format("fn {}{}({}) -> {}", structTypeStr, get(kb.ids.name).label(), iss.str(), oss.str()));
    } else {
        function.label(std::format("fn {}{}({})", structTypeStr, get(kb.ids.name).label(), iss.str()));
    }
}

CellI& Ast::Function::compileAst(CellI& ast, cells::Object& function, CellI& state)
{
    auto compile = [this, &function, &state](CellI& ast) -> CellI& { return compileAst(ast, function, state); };
    const auto _ = [this](CellI& cell) -> Ast::Cell& { return kb.ast.cell(cell); };
    auto& ids = kb.ids;

    if (&ast.type() == &kb.type.ast.Block) {
        CellI& list        = ast[ids.asts];
        auto& compiledAsts = *new cells::List(kb, kb.type.op.Base);
        Visitor::visitList(list, [this, &compile, &compiledAsts, &ast, &function](CellI& ast, int, bool&) {
            compiledAsts.add(compile(ast));
        });
        Object& opBlock = *new Object(kb, kb.type.op.Block);
        opBlock.set(ids.ast, ast);
        opBlock.set(ids.ops, compiledAsts);
        return opBlock;
    } else if (&ast.type() == &kb.type.ast.Cell) {
        Object& constVar = *new Object(kb, kb.type.op.ConstVar);
        constVar.set(ids.ast, ast);
        constVar.set(ids.value, ast[ids.value]);
        return constVar;
    } else if (&ast.type() == &kb.type.ast.StructRef) {
        Object& constVar = *new Object(kb, kb.type.op.ConstVar);
        constVar.set(ids.ast, ast);
        constVar.set(ids.value, resolveType(ast, state));
        return constVar;
    } else if (&ast.type() == &kb.type.ast.SelfFn) {
        Object& constVar = *new Object(kb, kb.type.op.ConstVar);
        constVar.set(ids.ast, ast);
        constVar.set(ids.value, function);
        return constVar;
    } else if (&ast.type() == &kb.type.ast.Self) {
        CellI& retOp = compile(kb.ast.get(_(function), _(ids.stack)) / _(ids.value) / _(ids.input) / _(ids.self));
        retOp.set(ids.ast, ast);
        retOp.label("self");
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Parameter) {
        CellI& retOp = compile(kb.ast.get(_(function), _(ids.stack)) / _(ids.value) / _(ids.input) / _(ast[ids.role]));
        retOp.set(ids.ast, ast);
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Var) {
        if (function.type()[ids.subTypes][ids.index].missing(ids.localVars)) {
            cells::Object& functionLocalVarsType = *new cells::Object(kb, kb.type.Type_, std::format("LocalVarsType of {}", function.label()));
            functionLocalVarsType.set(ids.memberOf, kb.map(kb.type.Type_, kb.type.Type_, kb.type.Index, kb.type.Index));
            static_cast<Map&>(function.type()[ids.subTypes]).add(ids.localVars, functionLocalVarsType);
        }
        CellI& localVarsType = function.type()[ids.subTypes][ids.index][ids.localVars][ids.value];
        if (localVarsType.missing(ids.slots)) {
            localVarsType.set(ids.slots, *new Map(kb, kb.type.Cell, kb.type.Slot));
        }
        auto& slotsMap = static_cast<Map&>(localVarsType[ids.slots]);
        if (!slotsMap.hasKey(ast[ids.role])) {
            slotsMap.add(ast[ids.role], kb.type.slot(ast[ids.role], kb.type.op.Var));
        }
        CellI& retOp = compile(kb.ast.get(_(function), _(ids.stack)) / _(ids.value) / _(ids.localVars) / _(ast[ids.role]));
        retOp.set(ids.ast, ast);
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Delete) {
        Object& retOp = *new Object(kb, kb.type.op.Delete);
        retOp.set(ids.ast, ast);
        retOp.set(ids.input, compile(ast[ids.cell]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Set) {
        Object& retOp = *new Object(kb, kb.type.op.Set);
        retOp.set(ids.ast, ast);
        retOp.set(ids.cell, compile(ast[ids.cell]));
        retOp.set(ids.role, compile(ast[ids.role]));
        retOp.set(ids.value, compile(ast[ids.value]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Erase) {
        Object& retOp = *new Object(kb, kb.type.op.Erase);
        retOp.set(ids.ast, ast);
        retOp.set(ids.cell, compile(ast[ids.cell]));
        retOp.set(ids.role, compile(ast[ids.role]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.If) {
        Object& retOp = *new Object(kb, kb.type.op.If);
        retOp.set(ids.ast, ast);
        retOp.set(ids.condition, compile(ast[ids.condition]));
        retOp.set(ids.then, compile(ast[ids.then]));
        if (ast.has(ids.else_)) {
            retOp.set(ids.else_, compile(ast[ids.else_]));
            return retOp;
        } else {
            return retOp;
        }
    } else if (&ast.type() == &kb.type.ast.Do) {
        Object& retOp = *new Object(kb, kb.type.op.Do);
        retOp.set(ids.ast, ast);
        retOp.set(ids.condition, compile(ast[ids.condition]));
        retOp.set(ids.statement, compile(ast[ids.statement]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.While) {
        Object& retOp = *new Object(kb, kb.type.op.While);
        retOp.set(ids.ast, ast);
        retOp.set(ids.condition, compile(ast[ids.condition]));
        retOp.set(ids.statement, compile(ast[ids.statement]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.New) {
        auto& compiledAsts = *new cells::List(kb, kb.type.op.Base);

        Object& block = *new Object(kb, kb.type.op.Block);
        block.set(ids.ast, ast);
        block.set(ids.ops, compiledAsts);
        Object& opSet = *new Object(kb, kb.type.op.Set, "block.value = new objectType()");
        opSet.set(ids.ast, ast);
        opSet.set(ids.cell, compile(kb.ast.cell(block)));
        opSet.set(ids.role, compile(kb.ast.cell(ids.value)));
        Object& opNew = *new Object(kb, kb.type.op.New);
        opNew.set(ids.ast, ast);
        opNew.set(ids.objectType, compile(ast[ids.objectType]));
        opSet.set(ids.value, opNew);

        compiledAsts.add(opSet);
        if (ast.has(ids.constructor)) {
            Object& callAst = *new Object(kb, kb.type.ast.Call);
            callAst.set(ids.cell, kb.ast.get(kb.ast.cell(block), kb.ast.cell(ids.value)));
            callAst.set(ids.method, ast[ids.constructor]);
            if (ast.has(ids.parameters)) {
                callAst.set(ids.parameters, ast[ids.parameters]);
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
        Ast::Base& astCell     = static_cast<Ast::Base&>(ast[ids.cell]);
        Ast::Base& astMethod   = static_cast<Ast::Base&>(ast[ids.method]);
        Ast::Get* getMethodPtr = nullptr;
        if (&ast.type() == &kb.type.ast.Call) {
            getMethodPtr = &(kb.ast.get(astCell, _(ids.type)) / _(ids.methods));
        } else {
            getMethodPtr = &(kb.ast.get(astCell, _(ids.methods)));
        }
        Ast::Get& getMethod = (*getMethodPtr) / _(ids.index) / astMethod / _(ids.value);
        auto& compiledAsts  = *new cells::List(kb, kb.type.op.Base);
        Object& block       = *new Object(kb, kb.type.op.Block, blockName);
        block.set(ids.ast, ast);
        block.set(ids.ops, compiledAsts);
        Object& varMethod = *new Object(kb, kb.type.op.Var, "Call { var_method; }");
        varMethod.set(ids.objectType, kb.type.op.Function);
        CellI& storeMethod     = compile(kb.ast.set(_(varMethod), _(ids.value), getMethod));
        CellI& setCurrentStack = compile(kb.ast.set(_(varMethod) / _(ids.value), _(ids.stack), kb.ast.get(_(function), _(ids.stack))));

        Object& varNewStackItem = *new Object(kb, kb.type.op.Var, "Call { var_newStackItem; }");
        varNewStackItem.set(ids.objectType, kb.type.ListItem);
        Object& varNewStackFrame = *new Object(kb, kb.type.op.Var, "Call { var_newStackFrame; }");
        varNewStackFrame.set(ids.objectType, kb.type.StackFrame);

        Object& varInputIndex = *new Object(kb, kb.type.Index, "Call { var_inputIndex; }");
        Object& varLocalVars = *new Object(kb, kb.type.op.Var, "Call { var_localVars; }");
        varLocalVars.set(ids.objectType, kb.type.Index);
        Object& varLocalVarsList = *new Object(kb, kb.type.op.Var, "Call { var_localVarsList; }");
        varLocalVarsList.set(ids.objectType, kb.type.List);
        Object& varLocalVarsListItem = *new Object(kb, kb.type.op.Var, "Call { var_localVarsListItem; }");
        varLocalVarsListItem.set(ids.objectType, kb.type.ListItem);
        Object& varLocalVarsListItemHasNext = *new Object(kb, kb.type.op.Var, "Call { var_localVarsListItemHasNext; }");
        varLocalVarsListItemHasNext.set(ids.objectType, kb.type.Boolean);

        CellI& storeStackItem     = compile(kb.ast.set(_(varNewStackItem), _(ids.value), kb.ast.new_(_(kb.type.ListItem))));
        CellI& storeStackFrame    = compile(kb.ast.set(_(varNewStackFrame), _(ids.value), kb.ast.new_(_(kb.type.StackFrame))));
        CellI& setListItem        = compile(kb.ast.set(_(varNewStackItem) / _(ids.value), _(ids.value), _(varNewStackFrame) / _(ids.value)));
        CellI& setListItemPrev    = compile(kb.ast.set(_(varNewStackItem) / _(ids.value), _(ids.previous), _(varMethod) / _(ids.value) / _(ids.stack)));
        CellI& setListItemNext    = compile(kb.ast.set(_(varMethod) / _(ids.value) / _(ids.stack), _(ids.next), _(varNewStackItem) / _(ids.value)));
        CellI& storeInputIndex    = compile(kb.ast.set(_(varInputIndex), _(ids.value), kb.ast.new_(_(kb.type.Index))));
        CellI& setMethod          = compile(kb.ast.set(_(varNewStackFrame) / _(ids.value), _(ids.method), _(varMethod) / _(ids.value)));

        CellI& createNewLocalVars = compile(
            kb.ast.if_(kb.ast.has(_(varMethod) / _(ids.value) / _(ids.type) / _(ids.subTypes) / _(ids.index), _(ids.localVars)),
                       kb.ast.block(
                           kb.ast.set(_(varLocalVars), _(ids.value), kb.ast.new_(_(varMethod) / _(ids.value) / _(ids.type) / _(ids.subTypes) / _(ids.index) / _(ids.localVars) / _(ids.value))),
                           kb.ast.set(_(varNewStackFrame) / _(ids.value), _(ids.localVars), _(varLocalVars) / _(ids.value)),
                           kb.ast.set(_(varLocalVarsListItem), _(ids.value), _(varMethod) / _(ids.value) / _(ids.type) / _(ids.subTypes) / _(ids.index) / _(ids.localVars) / _(ids.value) / _(ids.slots) / _(ids.list) / _(ids.first)),
                           kb.ast.do_(kb.ast.block(
                                          kb.ast.set(_(varLocalVarsListItemHasNext), _(ids.value), _(kb.boolean.true_)),
                                          kb.ast.set(_(varLocalVars) / _(ids.value), _(varLocalVarsListItem) / _(ids.value) / _(ids.value) / _(ids.slotRole), kb.ast.new_(_(kb.type.op.Var))),
                                          kb.ast.if_(kb.ast.has(_(varLocalVarsListItem) / _(ids.value), _(ids.next)),
                                                     kb.ast.set(_(varLocalVarsListItem), _(ids.value), _(varLocalVarsListItem) / _(ids.value) / _(ids.next)),
                                                     kb.ast.set(_(varLocalVarsListItemHasNext), _(ids.value), _(kb.boolean.false_)))),
                                      kb.ast.same(_(varLocalVarsListItemHasNext) / _(ids.value), _(kb.boolean.true_))))));

        CellI& setInput      = compile(kb.ast.set(_(varNewStackFrame) / _(ids.value), _(ids.input), _(varInputIndex) / _(ids.value)));
        CellI& setSelf       = compile(kb.ast.set(_(varInputIndex) / _(ids.value), _(ids.self), astCell));
        CellI& setStackToNew = compile(kb.ast.set(_(varMethod) / _(ids.value), _(ids.stack), _(varNewStackItem) / _(ids.value)));
        CellI& setRetValue   = compile(kb.ast.if_(kb.ast.has(_(varMethod) / _(ids.value) / _(ids.type) / _(ids.subTypes) / _(ids.index), _(ids.returnType)),
                                                  kb.ast.block(kb.ast.set(_(varNewStackFrame) / _(ids.value), _(ids.output), kb.ast.new_(_(kb.type.op.Var))),
                                                               kb.ast.set(_(varNewStackFrame) / _(ids.value) / _(ids.output), _(ids.objectType), _(varMethod) / _(ids.value) / _(ids.type) / _(ids.subTypes) / _(ids.index) / _(ids.returnType) / _(ids.value)))));
        CellI& getResult     = compile(kb.ast.if_(kb.ast.has(_(varMethod) / _(ids.value) / _(ids.type) / _(ids.subTypes) / _(ids.index), _(ids.returnType)),
                                                  kb.ast.set(_(block), _(ids.value), _(varMethod) / _(ids.value) / _(ids.stack) / _(ids.value) / _(ids.output) / _(ids.value))));
        CellI& setStackToOld = compile(kb.ast.set(_(varMethod) / _(ids.value) / _(ids.stack) / _(kb.ids.previous) / _(ids.value) / _(ids.method), _(ids.stack), _(varMethod) / _(ids.value) / _(ids.stack) / _(ids.previous)));

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


        if (ast.has(ids.parameters)) {
            Visitor::visitList(ast[ids.parameters], [this, &compiledAsts, &compile, &varInputIndex, &_, &ids](CellI& param, int, bool&) {
                Ast::Base& paramRole  = static_cast<Ast::Base&>(param[ids.slotRole]);
                Ast::Base& paramValue = static_cast<Ast::Base&>(param[ids.slotType]);
                CellI& setParam       = compile(kb.ast.set(_(varInputIndex) / _(ids.value), paramRole, paramValue));
                setParam.label("Call { setParam; }");
                compiledAsts.add(setParam);
            });
        }
        CellI& evalMethod = *new Object(kb, kb.type.op.EvalVar, std::format("{}::Call {{ evalVar; }}", function.label()));
        evalMethod.set(ids.value, varMethod);
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
        retOp.set(ids.ast, ast);
        retOp.set(ids.lhs, compile(ast[ids.lhs]));
        retOp.set(ids.rhs, compile(ast[ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Or) {
        Object& retOp = *new Object(kb, kb.type.op.Or);
        retOp.set(ids.ast, ast);
        retOp.set(ids.lhs, compile(ast[ids.lhs]));
        retOp.set(ids.rhs, compile(ast[ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Not) {
        Object& retOp = *new Object(kb, kb.type.op.Not);
        retOp.set(ids.ast, ast);
        retOp.set(ids.input, compile(ast[ids.input]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Add) {
        Object& retOp = *new Object(kb, kb.type.op.Add);
        retOp.set(ids.ast, ast);
        retOp.set(ids.lhs, compile(ast[ids.lhs]));
        retOp.set(ids.rhs, compile(ast[ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Subtract) {
        Object& retOp = *new Object(kb, kb.type.op.Subtract);
        retOp.set(ids.ast, ast);
        retOp.set(ids.lhs, compile(ast[ids.lhs]));
        retOp.set(ids.rhs, compile(ast[ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Multiply) {
        Object& retOp = *new Object(kb, kb.type.op.Multiply);
        retOp.set(ids.ast, ast);
        retOp.set(ids.lhs, compile(ast[ids.lhs]));
        retOp.set(ids.rhs, compile(ast[ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Divide) {
        Object& retOp = *new Object(kb, kb.type.op.Divide);
        retOp.set(ids.ast, ast);
        retOp.set(ids.lhs, compile(ast[ids.lhs]));
        retOp.set(ids.rhs, compile(ast[ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.LessThan) {
        Object& retOp = *new Object(kb, kb.type.op.LessThan);
        retOp.set(ids.ast, ast);
        retOp.set(ids.lhs, compile(ast[ids.lhs]));
        retOp.set(ids.rhs, compile(ast[ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.LessThanOrEqual) {
        Object& retOp = *new Object(kb, kb.type.op.LessThanOrEqual);
        retOp.set(ids.ast, ast);
        retOp.set(ids.lhs, compile(ast[ids.lhs]));
        retOp.set(ids.rhs, compile(ast[ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.GreaterThan) {
        Object& retOp = *new Object(kb, kb.type.op.GreaterThan);
        retOp.set(ids.ast, ast);
        retOp.set(ids.lhs, compile(ast[ids.lhs]));
        retOp.set(ids.rhs, compile(ast[ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.GreaterThanOrEqual) {
        Object& retOp = *new Object(kb, kb.type.op.GreaterThanOrEqual);
        retOp.set(ids.ast, ast);
        retOp.set(ids.lhs, compile(ast[ids.lhs]));
        retOp.set(ids.rhs, compile(ast[ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Same) {
        Object& retOp = *new Object(kb, kb.type.op.Same);
        retOp.set(ids.ast, ast);
        retOp.set(ids.lhs, compile(ast[ids.lhs]));
        retOp.set(ids.rhs, compile(ast[ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.NotSame) {
        Object& retOp = *new Object(kb, kb.type.op.NotSame);
        retOp.set(ids.ast, ast);
        retOp.set(ids.lhs, compile(ast[ids.lhs]));
        retOp.set(ids.rhs, compile(ast[ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Equal) {
        Object& retOp = *new Object(kb, kb.type.op.Equal);
        retOp.set(ids.ast, ast);
        retOp.set(ids.lhs, compile(ast[ids.lhs]));
        retOp.set(ids.rhs, compile(ast[ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.NotEqual) {
        Object& retOp = *new Object(kb, kb.type.op.NotEqual);
        retOp.set(ids.ast, ast);
        retOp.set(ids.lhs, compile(ast[ids.lhs]));
        retOp.set(ids.rhs, compile(ast[ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Has) {
        Object& retOp = *new Object(kb, kb.type.op.Has);
        retOp.set(ids.ast, ast);
        retOp.set(ids.cell, compile(ast[ids.cell]));
        retOp.set(ids.role, compile(ast[ids.role]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Missing) {
        Object& retOp = *new Object(kb, kb.type.op.Missing);
        retOp.set(ids.ast, ast);
        retOp.set(ids.cell, compile(ast[ids.cell]));
        retOp.set(ids.role, compile(ast[ids.role]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Get) {
        Object& retOp = *new Object(kb, kb.type.op.Get);
        retOp.set(ids.ast, ast);
        retOp.set(ids.cell, compile(ast[ids.cell]));
        retOp.set(ids.role, compile(ast[ids.role]));
        return retOp;
    } else if (&ast.type() == &kb.type.ast.Member) {
        CellI& member = compile(kb.ast.get(kb.ast.self(), kb.ast.cell(ast[ids.role])));
        member.set(ids.ast, ast);
        return member;
    } else if (&ast.type() == &kb.type.ast.Return) {
        Object& retOp = *new Object(kb, kb.type.op.Return, "op.return");
        retOp.set(ids.ast, ast);
        if (ast.has(ids.value)) {
            retOp.set(ids.result, compile(kb.ast.set(kb.ast.get(_(function), _(ids.stack)) / _(ids.value) / _(ids.output), _(ids.value), static_cast<Ast::Base&>(ast[ids.value]))));
        }
        return retOp;
    }

    throw "Unknown function AST!";
}

List& Ast::Function::parameters()
{
    if (missing(kb.ids.parameters)) {
        throw "No parameters!";
    } else {
        return static_cast<List&>(get(kb.ids.parameters));
    }
}

CellI& Ast::Function::returnType()
{
    if (missing(kb.ids.returnType)) {
        throw "No returnType!";
    } else {
        return get(kb.ids.returnType);
    }
}

Ast::Base& Ast::Function::code()
{
    if (missing(kb.ids.code)) {
        throw "No code!";
    } else {
        return static_cast<Ast::Base&>(get(kb.ids.code));
    }
}

Ast::FunctionT::FunctionT(brain::Brain& kb, CellI& name, const std::string& label) :
    BaseT<FunctionT>(kb, kb.type.ast.FunctionT, label)
{
    set(kb.ids.name, name);
    this->label(label);
}

Ast::FunctionT::FunctionT(brain::Brain& kb, const std::string& name) :
    BaseT<FunctionT>(kb, kb.type.ast.FunctionT, name)
{
    set(kb.ids.id, kb.pools.strings.get(name)[kb.ids.value]);
    label(name);
}

void Ast::FunctionT::parameters(Slot& param)
{
    if (missing(kb.ids.parameters)) {
        set(kb.ids.parameters, *new List(kb, kb.type.Slot));
    }
    parameters().add(param);
}

void Ast::FunctionT::returnType(CellI& type)
{
    set(kb.ids.returnType, type);
}

void Ast::FunctionT::addBlock(Block& block)
{
    set(kb.ids.code, block);
}

List& Ast::FunctionT::parameters()
{
    if (missing(kb.ids.parameters)) {
        throw "No parameters!";
    } else {
        return static_cast<List&>(get(kb.ids.parameters));
    }
}

CellI& Ast::FunctionT::returnType()
{
    if (missing(kb.ids.returnType)) {
        throw "No returnType!";
    } else {
        return get(kb.ids.returnType);
    }
}

Ast::Base& Ast::FunctionT::code()
{
    if (missing(kb.ids.code)) {
        throw "No code!";
    } else {
        return static_cast<Ast::Base&>(get(kb.ids.code));
    }
}

Ast::Delete::Delete(brain::Brain& kb, Base& cell) :
    BaseT<Delete>(kb, kb.type.ast.Delete, "ast.delete")
{
    set(kb.ids.cell, cell);
}

Ast::Set::Set(brain::Brain& kb, Base& cell, Base& role, Base& value) :
    BaseT<Set>(kb, kb.type.ast.Set, "ast.set")
{
    set(kb.ids.cell, cell);
    set(kb.ids.role, role);
    set(kb.ids.value, value);
}

Ast::Erase::Erase(brain::Brain& kb, Base& cell, Base& role) :
    BaseT<Erase>(kb, kb.type.ast.Erase, "ast.erase")
{
    set(kb.ids.cell, cell);
    set(kb.ids.role, role);
}

Ast::If::If(brain::Brain& kb, Base& condition, Base& thenBranch) :
    BaseT<If>(kb, kb.type.ast.If, "ast.if")
{
    set(kb.ids.condition, condition);
    set(kb.ids.then, thenBranch);
}

Ast::If::If(brain::Brain& kb, Base& condition, Base& thenBranch, Base& elseBranch) :
    BaseT<If>(kb, kb.type.ast.If, "ast.ifElse")
{
    set(kb.ids.condition, condition);
    set(kb.ids.then, thenBranch);
    set(kb.ids.else_, elseBranch);
}

Ast::Do::Do(brain::Brain& kb, Base& statement, Base& condition) :
    BaseT<Do>(kb, kb.type.ast.Do, "ast.do")
{
    set(kb.ids.condition, condition);
    set(kb.ids.statement, statement);
}

Ast::While::While(brain::Brain& kb, Base& condition, Base& statement) :
    BaseT<While>(kb, kb.type.ast.While, "ast.while")
{
    set(kb.ids.condition, condition);
    set(kb.ids.statement, statement);
}

Ast::Var::Var(brain::Brain& kb, CellI& role) :
    BaseT<Var>(kb, kb.type.ast.Var, std::format("var {}", role.label()))
{
    set(kb.ids.role, role);
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
    set(kb.ids.role, role);
}

Ast::Set& Ast::Member::operator=(Base& value)
{
    Ast::Set& ret = Set::New(kb, Self::New(kb), Cell::New(kb, get(kb.ids.role)), value);
    return ret;
}

Ast::Get& Ast::Member::operator/(Base& role)
{
    return Get::New(kb, *this, role);
}

Ast::Has& Ast::Member::exist()
{
    return Has::New(kb, Self::New(kb), Cell::New(kb, get(kb.ids.role)));
}

Ast::Missing& Ast::Member::missing()
{
    return Missing::New(kb, Self::New(kb), Cell::New(kb, get(kb.ids.role)));
}

Ast::Call& Ast::Member::call(const std::string& method) {
    return kb.ast.call(*this, method);
}

Ast::SubType::SubType(brain::Brain& kb, CellI& id) :
    BaseT<SubType>(kb, kb.type.ast.SubType, "ast.subType")
{
    set(kb.ids.id, id);
}

Ast::TemplatedType::TemplatedType(brain::Brain& kb, CellI& id, CellI& typeList) :
    BaseT<TemplatedType>(kb, kb.type.ast.TemplatedType, "ast.templatedType")
{
    set(kb.ids.id, id);
    set(kb.ids.parameters, typeList);
}

void Ast::TemplatedType::addParam(CellI& role, CellI& type)
{
    List& paramList = static_cast<List&>(get(kb.ids.parameters));
    paramList.add(kb.ast.slot(role, type));
}

void Ast::TemplatedType::addParam(CellI& role, const std::string& type)
{
    addParam(role, kb.ast.structRef(kb.pools.strings.get(type)[kb.ids.value]));
}

Ast::TemplateParam::TemplateParam(brain::Brain& kb, CellI& role) :
    BaseT<TemplateParam>(kb, kb.type.ast.TemplateParam, "ast.templateParam")
{
    set(kb.ids.role, role);
}

Ast::New::New(brain::Brain& kb, Base& objectType) :
    BaseT<New>(kb, kb.type.ast.New, "ast.new")
{
    set(kb.ids.objectType, objectType);
}

Ast::New::New(brain::Brain& kb, Base& objectType, Base& constructor) :
    BaseT<New>(kb, kb.type.ast.New, "ast.new()")
{
    set(kb.ids.objectType, objectType);
    set(kb.ids.constructor, constructor);
}

void Ast::New::addParam(Slot& slot)
{
    if (missing(kb.ids.parameters)) {
        set(kb.ids.parameters, kb.list(slot));
    } else {
        List& paramList = static_cast<List&>(get(kb.ids.parameters));
        paramList.add(slot);
    }
}

Ast::Same::Same(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Same>(kb, kb.type.ast.Same, "ast.same")
{
    set(kb.ids.lhs, lhs);
    set(kb.ids.rhs, rhs);
}

Ast::NotSame::NotSame(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<NotSame>(kb, kb.type.ast.NotSame, "ast.notSame")
{
    set(kb.ids.lhs, lhs);
    set(kb.ids.rhs, rhs);
}

Ast::Equal::Equal(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Equal>(kb, kb.type.ast.Equal, "ast.equal")
{
    set(kb.ids.lhs, lhs);
    set(kb.ids.rhs, rhs);
}

Ast::NotEqual::NotEqual(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<NotEqual>(kb, kb.type.ast.NotEqual, "ast.notEqual")
{
    set(kb.ids.lhs, lhs);
    set(kb.ids.rhs, rhs);
}

Ast::Has::Has(brain::Brain& kb, Base& cell, Base& role) :
    BaseT<Has>(kb, kb.type.ast.Has, "ast.has")
{
    set(kb.ids.cell, cell);
    set(kb.ids.role, role);
}

Ast::Missing::Missing(brain::Brain& kb, Base& cell, Base& role) :
    BaseT<Missing>(kb, kb.type.ast.Missing, "ast.missing")
{
    set(kb.ids.cell, cell);
    set(kb.ids.role, role);
}

Ast::Get::Get(brain::Brain& kb, Base& cell, Base& role) :
    BaseT<Get>(kb, kb.type.ast.Get, "ast.get")
{
    set(kb.ids.cell, cell);
    set(kb.ids.role, role);
}

Ast::Get& Ast::Get::operator/(Base& role)
{
    return Get::New(kb, *this, role);
}

Ast::And::And(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<And>(kb, kb.type.ast.And, "ast.and")
{
    set(kb.ids.lhs, lhs);
    set(kb.ids.rhs, rhs);
}

Ast::Or::Or(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Or>(kb, kb.type.ast.Or, "ast.or")
{
    set(kb.ids.lhs, lhs);
    set(kb.ids.rhs, rhs);
}

Ast::Not::Not(brain::Brain& kb, Base& input) :
    BaseT<Not>(kb, kb.type.ast.Not, "ast.not")
{
    set(kb.ids.input, input);
}

Ast::Add::Add(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Add>(kb, kb.type.ast.Add, "ast.add")
{
    set(kb.ids.lhs, lhs);
    set(kb.ids.rhs, rhs);
}

Ast::Subtract::Subtract(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Subtract>(kb, kb.type.ast.Subtract, "ast.subtract")
{
    set(kb.ids.lhs, lhs);
    set(kb.ids.rhs, rhs);
}

Ast::Multiply::Multiply(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Multiply>(kb, kb.type.ast.Multiply, "ast.multiply")
{
    set(kb.ids.lhs, lhs);
    set(kb.ids.rhs, rhs);
}

Ast::Divide::Divide(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Divide>(kb, kb.type.ast.Divide, "ast.divide")
{
    set(kb.ids.lhs, lhs);
    set(kb.ids.rhs, rhs);
}

Ast::LessThan::LessThan(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<LessThan>(kb, kb.type.ast.LessThan, "ast.lessThan")
{
    set(kb.ids.lhs, lhs);
    set(kb.ids.rhs, rhs);
}

Ast::LessThanOrEqual::LessThanOrEqual(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<LessThanOrEqual>(kb, kb.type.ast.LessThanOrEqual, "ast.lessThanOrEqual")
{
    set(kb.ids.lhs, lhs);
    set(kb.ids.rhs, rhs);
}

Ast::GreaterThan::GreaterThan(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<GreaterThan>(kb, kb.type.ast.GreaterThan, "ast.greaterThan")
{
    set(kb.ids.lhs, lhs);
    set(kb.ids.rhs, rhs);
}

Ast::GreaterThanOrEqual::GreaterThanOrEqual(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<GreaterThanOrEqual>(kb, kb.type.ast.GreaterThanOrEqual, "ast.greaterThanOrEqual")
{
    set(kb.ids.lhs, lhs);
    set(kb.ids.rhs, rhs);
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
        ret.set(kb.ids.scopes, namespaceList);
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
    kb.type.Directions.set(kb.ids.members, kb.list(up, down, left, right));
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
        return kb.ids.emptyObject;
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


// ============================================================================
Strings::Strings(brain::Brain& kb) :
    m_kb(kb)
{
}

String& Strings::get(const std::string& str)
{
    auto numberIt = m_strings.find(str);
    if (numberIt != m_strings.end()) {
        return numberIt->second;
    } else {
        auto it           = m_strings.emplace(std::piecewise_construct,
                                              std::forward_as_tuple(str),
                                              std::forward_as_tuple(m_kb, str));
        String& newString = it.first->second;
        newString.label(str);
        return newString;
    }
}

List& Strings::getCharList(const std::string& str)
{
    auto& string = get(str);
    return static_cast<List&>(string[string.kb.ids.value]);
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
    auto& ids   = kb.ids;
    CellI* map = nullptr;

    map = &kb.map(kb.type.Cell, kb.type.Slot,
                  kb.ids.input, type.slot(kb.ids.input, kb.type.Picture),
                  kb.ids.output, type.slot(kb.ids.output, kb.type.Picture));
    Demonstration.set(ids.slots, *map);

    map = &kb.map(kb.type.Cell, kb.type.Slot,
                  examples, type.slot(examples, kb.type.ListOf(Demonstration)),
                  kb.ids.input, type.slot(kb.ids.input, kb.type.Picture),
                  kb.ids.output, type.slot(kb.ids.output, kb.type.Picture));
    Task.set(ids.slots, *map);
}

Test::Test(brain::Brain& kb) :
    factorial(kb, kb.type.Cell, "factorial")
{
}

Ast::Cell& Brain::_(CellI& cell)
{
    return ast.cell(cell);
}

Ast::Parameter& Brain::p_(CellI& role)
{
    return ast.parameter(role);
}

Ast::Member& Brain::m_(CellI& role)
{
    return ast.member(role);
}

Ast::Var& Brain::var_(const std::string& name)
{
    return ast.var(id(name));
}

Ast::Slot& Brain::param(CellI& role, CellI& value)
{
    return ast.slot(role, value);
}

Ast::Slot& Brain::member(CellI& role, CellI& type)
{
    return ast.slot(role, type);
}

Ast::TemplateParam& Brain::tp_(CellI& role)
{
    return ast.templateParam(role);
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
        param(ids.objectType, _(type.Type_)));
    listItemStructT.memberOf(
        _(type.ListItem));
    listItemStructT.subTypes(
        param(ids.objectType, tp_(ids.objectType)));
    listItemStructT.members(
        member(ids.previous, tt_("ListItem", ids.objectType, tp_(ids.objectType))),
        member(ids.next, tt_("ListItem", ids.objectType, tp_(ids.objectType))),
        member(ids.value, tp_(ids.objectType)));

    auto& listItemConstructor = listItemStructT.addMethod("constructor");
    listItemConstructor.parameters(
        param(ids.value, tp_(ids.objectType)));
    listItemConstructor.code(
        m_(ids.value) = p_(ids.value));
#pragma endregion
#pragma region List
    auto& listStructT = stdScope.addStructT("List");
    listStructT.templateParams(
        param(ids.objectType, _(type.Type_)));
    listStructT.memberOf(
        _(type.Container),
        _(type.List));
    listStructT.subTypes(
        param(ids.itemType, tt_("ListItem", ids.objectType, tp_(ids.objectType))),
        param(ids.objectType, tp_(ids.objectType)));
    listStructT.members(
        member(ids.first, tt_("ListItem", ids.objectType, tp_(ids.objectType))),
        member(ids.last, tt_("ListItem", ids.objectType, tp_(ids.objectType))),
        member(ids.size, _(type.Number)));

    Ast::Function& listCtor = listStructT.addMethod("constructor");
    listCtor.code(
        m_(ids.size) = _(_0_));

    Ast::Function& listAdd = listStructT.addMethod("add");
    listAdd.parameters(
        param(ids.value, tp_(ids.objectType)));
    listAdd.returnType(tt_("ListItem", ids.objectType, tp_(ids.objectType)));
    listAdd.code(
        var_("item") = ast.new_(tt_("ListItem", ids.objectType, tp_(ids.objectType)), "constructor", param(_(ids.value), p_(ids.value))),
        ast.if_(ast.not_(m_(ids.first).exist()),
                m_(ids.first) = *var_("item"),                               // then
                ast.block(ast.set(m_(ids.last), _(ids.next), *var_("item")), // else
                          ast.set(*var_("item"), _(ids.previous), m_(ids.last)))),
        m_(ids.last) = *var_("item"),
        m_(ids.size) = ast.add(m_(ids.size), _(_1_)),
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
        param(ids.item, _(type.Cell)));
    listRemove.code(
        ast.if_(ast.has(p_(ids.item), _(ids.previous)),
                ast.if_(ast.has(p_(ids.item), _(ids.next)),
                        ast.set(p_(ids.item) / _(ids.previous), _(ids.next), p_(ids.item) / _(ids.next)),
                        ast.erase(p_(ids.item) / _(ids.previous), _(ids.next))),
                ast.if_(ast.has(p_(ids.item), _(ids.next)),
                        m_(ids.first) = p_(ids.item) / _(ids.next),
                        ast.erase(ast.self(), _(ids.first)))),
        ast.if_(ast.has(p_(ids.item), _(ids.next)),
                ast.if_(ast.has(p_(ids.item), _(ids.previous)),
                        ast.set(p_(ids.item) / _(ids.next), _(ids.previous), p_(ids.item) / _(ids.previous)),
                        ast.erase(p_(ids.item) / _(ids.next), _(ids.previous))),
                ast.if_(ast.has(p_(ids.item), _(ids.previous)),
                        m_(ids.last) = p_(ids.item) / _(ids.previous),
                        ast.erase(ast.self(), _(ids.last)))),
        m_(ids.size) = ast.subtract(m_(ids.size), _(_1_)));

    Ast::Function& listSize = listStructT.addMethod("size");
    listSize.returnType(_(type.Number));
    listSize.code(
        ast.return_(m_(ids.size)));

    Ast::Function& listEmpty = listStructT.addMethod("empty");
    listEmpty.returnType(_(type.Boolean));
    listEmpty.code(
        ast.if_(ast.equal(m_(ids.size), _(_0_)), ast.return_(_(boolean.true_)), ast.return_(_(boolean.false_))));
#pragma endregion
#pragma region Type
    auto& typeStruct = stdScope.addStruct("Type");
    typeStruct.members(
        member(ids.slots, tt_("Map", ids.keyType, _(type.Cell), ids.objectType, _(type.Slot))),
        member(ids.incomplete, _(type.Boolean)),
        member(ids.sharedObject, _(type.Slot)),
        member(ids.subTypes, tt_("Map", ids.keyType, _(type.Cell), ids.objectType, "Type")),
        member(ids.memberOf, tt_("Map", ids.keyType, "Type", ids.objectType, "Type")),
        member(ids.asts, tt_("Map", ids.keyType, _(type.Cell), ids.objectType, _(type.ast.Function))),
        member(ids.methods, tt_("Map", ids.keyType, _(type.Cell), ids.objectType, _(type.op.Function))));

    Ast::Function& typeCtor = typeStruct.addMethod("constructor");
    typeCtor.code(
        m_(ids.slots) = ast.new_(tt_("Map", ids.keyType, _(type.Cell), ids.objectType, _(type.Slot)), "constructor"));

    Ast::Function& typeCtorWithRecursiveType = typeStruct.addMethod("constructorWithRecursiveType");
    typeCtorWithRecursiveType.code(
        m_(ids.slots) = ast.new_(tt_("Map", ids.keyType, _(type.Cell), ids.objectType, _(type.Slot)), "constructorWithIndexType", param(_(ids.indexType), ast.self())));

    Ast::Function& typeAddSubType = typeStruct.addMethod("addSubType");
    typeAddSubType.parameters(
        param(ids.slotRole, _(type.Cell)),
        param(ids.slotType, _(type.Type_)));
    typeAddSubType.code(
        ast.if_(m_(ids.subTypes).missing(), m_(ids.subTypes) = ast.new_(tt_("Map", ids.keyType, _(type.Cell), ids.objectType, "Type"), "constructor")),
        m_(ids.subTypes).call("add", param(_(ids.key), p_(ids.slotRole)), param(_(ids.value), p_(ids.slotType))));

    Ast::Function& typeAddMembership = typeStruct.addMethod("addMembership");
    typeAddMembership.parameters(
        param(ids.cell, _(type.Type_)));
    typeAddMembership.code(
        ast.if_(m_(ids.memberOf).missing(), m_(ids.memberOf) = ast.new_(tt_("Map", ids.keyType, "Type", ids.objectType, "Type"), "constructor")),
        m_(ids.memberOf).call("add", param(_(ids.key), p_(ids.cell)), param(_(ids.value), p_(ids.cell))));

    Ast::Function& typeAddSlot = typeStruct.addMethod("addSlot");
    typeAddSlot.parameters(
        param(ids.slotRole, _(type.Cell)),
        param(ids.slotType, _(type.Slot)));
    typeAddSlot.code(
        ast.if_(m_(ids.slots).missing(), m_(ids.slots) = ast.new_(tt_("Map", ids.keyType, _(type.Cell), ids.objectType, _(type.Slot)), "constructor")),
        var_("slot") = ast.new_(_(type.Slot)),
        ast.set(*var_("slot"), _(ids.slotRole), p_(ids.slotRole)),
        ast.set(*var_("slot"), _(ids.slotType), p_(ids.slotType)),
        m_(ids.slots).call("add", param(_(ids.key), p_(ids.slotRole)), param(_(ids.value), *var_("slot"))));

    Ast::Function& typeAddSlots = typeStruct.addMethod("addSlots");
    typeAddSlots.parameters(
        param(ids.list, tt_("List", ids.objectType, _(type.Slot))));
    typeAddSlots.code(
        ast.if_(ast.equal(p_(ids.list) / _(ids.size), _(_0_)), ast.return_()),
        var_("item") = p_(ids.list) / _(ids.first),
        ast.if_(m_(ids.slots).missing(), m_(ids.slots) = ast.new_(tt_("Map", ids.keyType, _(type.Cell), ids.objectType, _(type.Slot)), "constructor")),
        ast.do_(ast.block(
                    var_("next") = _(boolean.true_),
                    m_(ids.slots).call("add", param(_(ids.key), *var_("item") / _(ids.value) / _(ids.slotRole)), param(_(ids.value), *var_("item") / _(ids.value))),
                    ast.if_(ast.has(*var_("item"), _(ids.next)),
                            var_("item") = *var_("item") / _(ids.next),
                            var_("next") = _(boolean.false_))),
                ast.same(*var_("next"), _(boolean.true_))));

    Ast::Function& typeHasSlot = typeStruct.addMethod("hasSlot");
    typeHasSlot.parameters(
        param(ids.slotRole, _(type.Cell)));
    typeHasSlot.returnType(_(type.Boolean));
    typeHasSlot.code(
        ast.if_(m_(ids.slots).missing(), ast.return_(_(boolean.false_))),
        ast.return_(m_(ids.slots).call("hasKey", param(_(ids.key), p_(ids.slotRole)))));

    Ast::Function& typeRemoveSlot = typeStruct.addMethod("removeSlot");
    typeRemoveSlot.parameters(
        param(ids.slotRole, _(type.Cell)));
    typeRemoveSlot.code(
        ast.if_(m_(ids.slots).missing(), ast.return_()),
        m_(ids.slots).call("remove", param(_(ids.key), p_(ids.slotRole))));
#pragma endregion
#pragma region Index
    auto& indexStruct = stdScope.addStruct("Index");
    indexStruct.memberOf(_(type.Type_));

    Ast::Function& indexCtor = indexStruct.addMethod("constructor");
    indexCtor.code(
        ast.set(ast.self(), _(ids.type), ast.new_("Type", "constructorWithRecursiveType")),
        ast.set(m_(ids.type), _(ids.methods), ast.get(struct_("Index"), _(ids.methods))),
        ast.set(m_(ids.type), _(ids.memberOf), _(map(type.Type_, type.Type_, type.Index, type.Index))));

    Ast::Function& indexCtorWithSelfType = indexStruct.addMethod("constructorWithSelfType");
    indexCtorWithSelfType.parameters(
        param(ids.indexType, _(type.Type_)));
    indexCtorWithSelfType.code(
        ast.if_(ast.missing(p_(ids.indexType), _(ids.sharedObject)),
                ast.block(ast.set(p_(ids.indexType), _(ids.sharedObject), ast.new_(_(type.Slot))),
                          ast.set(p_(ids.indexType) / _(ids.sharedObject), _(ids.slotRole), ast.self()),
                          ast.set(p_(ids.indexType) / _(ids.sharedObject), _(ids.slotType), struct_("Index")))),
        ast.set(ast.self(), _(ids.type), p_(ids.indexType)));

    /*
    void Index::insert(CellI& key, CellI& value)
    {
        if (&key == &kb.ids.type) {
            throw "The type key can not be changed!";
        }
        m_slots[&key] = &value;
        if (m_recursiveType) {
            return;
        }
        Object& slot = *new Object(kb, kb.type.Slot);
        slot.set(kb.ids.slotRole, key);
        slot.set(kb.ids.slotType, kb.type.Slot);
        m_type->addSlot(key, slot);
    }
    */
    Ast::Function& indexInsert = indexStruct.addMethod("insert");
    indexInsert.parameters(
        param(ids.key, _(type.Cell)),
        param(ids.value, _(type.Cell)));
    indexInsert.code(
        ast.if_(ast.same(p_(ids.key), _(ids.type)), ast.return_()),
        ast.set(ast.self(), p_(ids.key), p_(ids.value)),
        ast.if_(ast.and_(ast.has(m_(ids.type), _(ids.sharedObject)), ast.same(m_(ids.type) / _(ids.sharedObject) / _(ids.slotRole), ast.self())),
                ast.return_()),
        m_(ids.type).call("addSlot", param(_(ids.slotRole), p_(ids.key)), param(_(ids.slotType), _(type.Slot))));

    Ast::Function& indexEmpty = indexStruct.addMethod("empty");
    indexEmpty.returnType(_(type.Boolean));
    indexEmpty.code(
        ast.return_(ast.call(m_(ids.type) / _(ids.slots), "empty")));

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
        param(ids.key, _(type.Cell)));
    indexRemove.code(
        ast.if_(ast.not_(m_(ids.type).call("hasSlot", param(_(ids.slotRole), p_(ids.key)))),
                ast.return_()),
        ast.erase(ast.self(), p_(ids.key)),
        m_(ids.type).call("removeSlot", param(_(ids.slotRole), p_(ids.key))));

    Ast::Function& indexSize = indexStruct.addMethod("size");
    indexSize.returnType(_(type.Number));
    indexSize.code(
        ast.return_(ast.call(m_(ids.type) / _(ids.slots), "size")));
#pragma endregion
#pragma region Map
    auto& mapStructT = stdScope.addStructT("Map");
    mapStructT.templateParams(
        param(ids.keyType, _(type.Type_)),
        param(ids.objectType, _(type.Type_)));
    mapStructT.subTypes(
        param(ids.keyType, tp_(ids.keyType)),
        param(ids.objectType, tp_(ids.objectType)),
        param(ids.listType, tt_("List", ids.objectType, tp_(ids.objectType))));
    mapStructT.memberOf(_(type.Container), _(type.Map));
    mapStructT.members(
        member(ids.list, tt_("List", ids.objectType, tp_(ids.objectType))),
        member(ids.index, struct_("Index")),
        member(ids.size, _(type.Number)));

    Ast::Function& mapCtor = mapStructT.addMethod("constructor");
    mapCtor.code(
        m_(ids.size)  = _(_0_),
        m_(ids.list)  = ast.new_(tt_("List", ids.objectType, tp_(ids.objectType)), "constructor"),
        m_(ids.index) = ast.new_("Index", "constructor"));

    Ast::Function& mapCtorWithIndexType = mapStructT.addMethod("constructorWithIndexType");
    mapCtorWithIndexType.parameters(
        param(ids.indexType, _(type.Type_)));
    mapCtorWithIndexType.code(
        m_(ids.size)  = _(_0_),
        m_(ids.list)  = ast.new_(tt_("List", ids.objectType, tp_(ids.objectType)), "constructor"),
        m_(ids.index) = ast.new_("Index", "constructorWithSelfType", param(_(ids.indexType), p_(ids.indexType))));

    /*
    bool Map::hasKey(CellI& key)
    {
        return m_index.has(key);
    }
    */
    Ast::Function& mapHasKey = mapStructT.addMethod("hasKey");
    mapHasKey.parameters(
        param(ids.key, tp_(ids.keyType)));
    mapHasKey.returnType(_(type.Boolean));
    mapHasKey.code(
        ast.if_(ast.has(m_(ids.index), p_(ids.key)), ast.return_(_(boolean.true_)), ast.return_(_(boolean.false_))));

    /*
    CellI& Map::getValue(CellI& key)
    {
        if (m_index.has(key)) {
            return m_index[key][kb.ids.value];
        }
        throw "No such role!";
    }
    */
    Ast::Function& mapGetValue = mapStructT.addMethod("getValue");
    mapGetValue.parameters(
        param(ids.key, tp_(ids.keyType)));
    mapGetValue.returnType(tp_(ids.objectType));
    mapGetValue.code(
        ast.if_(ast.has(m_(ids.index), p_(ids.key)), ast.return_(m_(ids.index) / p_(ids.key) / _(ids.value)), ast.return_(_(ids.emptyObject))));

    /*
    void Map::add(CellI& key, CellI& value)
    {
        if (&key == &kb.ids.type) {
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
        param(ids.key, tp_(ids.keyType)),
        param(ids.value, tp_(ids.objectType)));
    mapAdd.code(
        ast.if_(ast.same(p_(ids.key), _(ids.type)), ast.return_()),
        ast.if_(ast.has(m_(ids.index), p_(ids.key)), ast.return_()),
        m_(ids.size)   = ast.add(m_(ids.size), _(_1_)),
        var_("item") = m_(ids.list).call("add", param(_(ids.value), p_(ids.value))),
        m_(ids.index).call("insert", param(_(ids.key), p_(ids.key)), param(_(ids.value), *var_("item"))));

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
        param(ids.key, tp_(ids.keyType)));
    mapRemove.code(
        ast.if_(ast.missing(m_(ids.index), p_(ids.key)),
                ast.return_()),
        m_(ids.list).call("remove", param(_(ids.item), m_(ids.index) / p_(ids.key))),
        m_(ids.index).call("remove", param(_(ids.key), p_(ids.key))),
        m_(ids.size) = ast.subtract(m_(ids.size), _(_1_)));

    Ast::Function& mapSize = mapStructT.addMethod("size");
    mapSize.returnType(_(type.Number));
    mapSize.code(
        ast.return_(m_(ids.size)));

    Ast::Function& mapEmpty = mapStructT.addMethod("empty");
    mapEmpty.returnType(_(type.Boolean));
    mapEmpty.code(
        ast.if_(ast.equal(m_(ids.size), _(_0_)), ast.return_(_(boolean.true_)), ast.return_(_(boolean.false_))));
#pragma endregion
#pragma region Set
    auto& setStructT = stdScope.addStructT("Set");
    setStructT.templateParams(
        param(ids.keyType, _(type.Type_)),
        param(ids.objectType, _(type.Type_)));
    setStructT.subTypes(
        param(ids.objectType, tp_(ids.objectType)),
        param(ids.listType, tt_("List", ids.objectType, tp_(ids.objectType))));
    setStructT.memberOf(_(type.Container));
    setStructT.members(
        member(ids.list, tt_("List", ids.objectType, tp_(ids.objectType))),
        member(ids.index, struct_("Index")),
        member(ids.size, _(type.Number)));

    Ast::Function& setCtor = setStructT.addMethod("constructor");
    setCtor.code(
        m_(ids.size)  = _(_0_),
        m_(ids.list)  = ast.new_(tt_("List", ids.objectType, tp_(ids.objectType)), "constructor"),
        m_(ids.index) = ast.new_(struct_("Index"), "constructor"));

    Ast::Function& setAdd = setStructT.addMethod("add");
    setAdd.parameters(
        param(ids.value, tp_(ids.objectType)));
    setAdd.returnType(tt_("ListItem", ids.objectType, tp_(ids.objectType)));
    setAdd.code(
        ast.if_(ast.has(m_(ids.index), p_(ids.value)),
                ast.return_(m_(ids.index) / p_(ids.value))),
        var_("listItem") = m_(ids.list).call("add", ast.slot(_(ids.value), p_(ids.value))),
        m_(ids.size)     = ast.add(m_(ids.size), _(_1_)),
        ast.call(m_(ids.indexType) / _(ids.slots), "add", ast.slot(_(ids.key), p_(ids.value)), ast.slot(_(ids.value), *var_("listItem"))),
        ast.set(m_(ids.index), p_(ids.value), *var_("listItem")),
        ast.return_(*var_("listItem")));

    Ast::Function& setContains = setStructT.addMethod("contains");
    setContains.parameters(
        param(ids.value, tp_(ids.objectType)));
    setContains.returnType(_(type.Boolean));
    setContains.code(
        ast.if_(ast.has(m_(ids.index), p_(ids.value)),
                ast.return_(_(boolean.true_)),
                ast.return_(_(boolean.false_))));

    Ast::Function& setErase = setStructT.addMethod("erase");
    setErase.parameters(
        param(ids.value, tp_(ids.objectType)));
    setErase.code(
        ast.if_(ast.missing(m_(ids.index), p_(ids.value)),
                ast.return_()),
        ast.erase(m_(ids.index), p_(ids.value)),
        m_(ids.size) = ast.subtract(m_(ids.size), _(_1_)));

    Ast::Function& setSize = setStructT.addMethod("size");
    setSize.returnType(_(type.Number));
    setSize.code(
        ast.return_(m_(ids.size)));

    Ast::Function& setEmpty = setStructT.addMethod("empty");
    setEmpty.returnType(_(type.Boolean));
    setEmpty.code(
        ast.if_(ast.equal(m_(ids.size), _(_0_)), ast.return_(_(boolean.true_)), ast.return_(_(boolean.false_))));
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
        var_("result") = ast.new_(tt_("std::List", ids.objectType, _(type.Number))),
        var_("result") = ast.new_(tt_("std::List", ids.objectType, _(type.Cell))),
        var_("result") = ast.new_(tt_("std::List", ids.objectType, _(type.Pixel))),
        var_("result") = ast.new_(tt_("std::Set", ids.objectType, _(type.Number))),
        var_("result") = ast.new_(tt_("std::Map", ids.keyType, _(type.Number), ids.objectType, _(type.Color))));

    CellI* mapPtr = nullptr;
    mapPtr        = &slots(type.slot(ids.value, type.ListOf(type.Digit)),
                           type.slot(numbers.sign, type.Number)); // TODO sign has no class currently
    type.Number.set(ids.slots, *mapPtr);

    Ast::Function& numberFactorial = testStruct.addMethod("factorial");
    numberFactorial.parameters(
        param(ids.input, _(type.Number)));
    numberFactorial.returnType(_(type.Number));
    numberFactorial.code(
        ast.if_(ast.greaterThanOrEqual(p_(ids.input), _(_1_)),
                ast.return_(ast.multiply(p_(ids.input), ast.call(ast.self(), "factorial", ast.slot(_(ids.input), ast.subtract(p_(ids.input), _(_1_)))))),
                ast.return_(_(_1_))));

    // TODO
//    type.String.method(ids.addSlots, { ids.list, list(type.slot(ids.value, type.ListOf(type.Char))) });
}

void Brain::createArcSolver()
{
    auto& arcScope = globalScope.addScope("arc");

    auto& colorStruct = arcScope.addStruct("Color");
    colorStruct.members(
        member(colors.red, _(type.Number)),
        member(colors.green, _(type.Number)),
        member(colors.blue, _(type.Number)));

    auto& pixelStruct = arcScope.addStruct("Pixel");
    pixelStruct.members(
        member(directions.up, struct_("Pixel")),
        member(directions.down, struct_("Pixel")),
        member(directions.left, struct_("Pixel")),
        member(directions.right, struct_("Pixel")),
        member(coordinates.x, _(type.Number)),
        member(coordinates.y, _(type.Number)));

    auto& pictureStruct = arcScope.addStruct("Picture");
    pictureStruct.members(
        member(ids.width, _(type.Number)),
        member(ids.height, _(type.Number)),
        member(ids.pixels, tt_("std::List", ids.objectType, "Pixel")));

    auto& shapeStruct = arcScope.addStruct("Shape");
    shapeStruct.members(
        member(ids.color, struct_("Color")),
        member(ids.width, _(type.Number)),
        member(ids.height, _(type.Number)),
        member(ids.pixels, tt_("std::List", ids.objectType, "Pixel")));

    auto& shaperStruct = arcScope.addStruct("Shaper");
    shaperStruct.members(
        member(ids.width, _(type.Number)),
        member(ids.height, _(type.Number)),
        member(ids.picture, _(type.Picture)),
        member(ids.shapes, tt_("std::List", ids.objectType, "Shape")),
        member(ids.inputPixels, tt_("std::List", ids.objectType, "Pixel")));

    /*
    * class Shape
{
public:
    Shape(int id, input::Color color, int width, int height) :
        m_id(id), m_color(color), m_width(width), m_height(height) { }
    */
    Ast::Function& shapeCtor = shapeStruct.addMethod("constructor");
    shapeCtor.parameters(
        param(ids.id, _(type.Number)),
        param(ids.color, struct_("Color")),
        param(ids.width, _(type.Number)),
        param(ids.height, _(type.Number)));
    shapeCtor.code(
        m_(ids.id)     = p_(ids.id),
        m_(ids.color)  = p_(ids.color),
        m_(ids.width)  = p_(ids.width),
        m_(ids.height) = p_(ids.height));

    /*
    void Shape::addPixel(cells::hybrid::Pixel& pixel)
    {
        m_pixels.push_back({ pixel.m_x.value(), pixel.m_y.value() });
        m_hybridPixels.insert(&pixel);
    }
    */
    Ast::Function& shapeAddPixel = shapeStruct.addMethod("addPixel");
    shapeAddPixel.parameters(
        param(ids.pixel, struct_("Pixel")));
    shapeAddPixel.code(
        m_(ids.pixels).call("add", ast.slot(_(ids.value), p_(ids.pixel))));

    /*
    bool Shape::hasPixel(cells::hybrid::Pixel& pixel) const
    {
        return m_hybridPixels.find(&pixel) != m_hybridPixels.end();
    }
    */
    Ast::Function& shapeHasPixel = shapeStruct.addMethod("hasPixel");
    shapeHasPixel.parameters(
        param(ids.pixel, struct_("Pixel")));
    shapeHasPixel.code(
        m_(ids.pixels) / _(ids.index) / p_(ids.pixel)); // TODO we need a Set class

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
        param(ids.width, _(type.Number)),
        param(ids.height, _(type.Number)),
        param(ids.picture, struct_("Picture")));
    shaperCtor.code(
        m_(ids.width)   = p_(ids.width),
        m_(ids.height)  = p_(ids.height),
        m_(ids.picture) = p_(ids.picture),
        ast.call(ast.self(), _(ids.processInputPixels)));
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
        var_("pixels") = m_(ids.picture) / _(ids.pixels),
        var_("pixel")  = *var_("pixels") / _(ids.first),
        ast.while_(ast.same(*var_("pixel"), *var_("pixels") / _(ids.last)),
                   ast.block(
                       m_(ids.inputPixels).call("add", ast.slot(_(ids.value), *var_("pixel"))),
                       var_("pixel") = *var_("pixels") / _(ids.first))));

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
        ast.while_(ast.not_(m_(ids.inputPixels).call("empty")),
                   ast.block(
                       var_("firstPixel") = m_(ids.inputPixels).call("first", ast.slot(_(ids.pixel), p_(ids.checkPixel))), // TODO
                       m_(ids.shapes).call("add", ast.slot(_(ids.value), ast.new_("Shape", "constructor", ast.slot(_(ids.id), *var_("shapeId")), ast.slot(_(ids.color), *var_("firstPixel") / _(ids.color)), ast.slot(_(ids.width), m_(ids.width)), ast.slot(_(ids.height), m_(ids.height))))),
                       var_("shape")       = m_(ids.shapes).call("last"),
                       var_("checkPixels") = ast.new_(tt_("std::List", ids.objectType, "Pixel"), "constructor"),
                       ast.call(*var_("checkPixels"), "add", ast.slot(_(ids.value), *var_("firstPixel"))),
                       ast.while_(ast.not_(ast.call(*var_("checkPixels"), _(ids.empty))),
                                  ast.block(
                                      var_("checkPixel") = ast.call(*var_("checkPixels"), "first"),
                                      ast.call(ast.self(), "processPixel", ast.slot(_(ids.shape), *var_("shape")), ast.slot(_(ids.checkPixels), *var_("checkPixels")), ast.slot(_(ids.checkPixel), *var_("checkPixel"))),
                                      ast.call(*var_("checkPixels"), "erase", ast.slot(_(ids.value), *var_("firstPixel"))))),
                       ast.call(*var_("shape"), _(ids.sortPixels)))));

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
        param(ids.shape, struct_("Shape")),
        param(ids.checkPixels, tt_("std::List", ids.objectType, "Pixel")),
        param(ids.checkPixel, struct_("Pixel")));
    shaperProcessPixel.code(
        ast.call(p_(ids.shape), "addPixel", ast.slot(_(ids.pixel), p_(ids.checkPixel))),
        m_(ids.inputPixels).call("erase", ast.slot(_(ids.pixel), p_(ids.checkPixel))),
        ast.if_(var_("pixel") = ast.call(ast.self(), "processAdjacentPixel", ast.slot(_(ids.direction), _(directions.up)), ast.slot(_(ids.shape), p_(ids.shape)), ast.slot(_(ids.checkPixels), p_(ids.checkPixels)), ast.slot(_(ids.checkPixel), p_(ids.checkPixel))),
                ast.block(
                    ast.call(ast.self(), "processAdjacentPixel", ast.slot(_(ids.direction), _(directions.left)), ast.slot(_(ids.shape), p_(ids.shape)), ast.slot(_(ids.checkPixels), p_(ids.checkPixels)), ast.slot(_(ids.checkPixel), *var_("pixel"))),
                    ast.call(ast.self(), "processAdjacentPixel", ast.slot(_(ids.direction), _(directions.right)), ast.slot(_(ids.shape), p_(ids.shape)), ast.slot(_(ids.checkPixels), p_(ids.checkPixels)), ast.slot(_(ids.checkPixel), *var_("pixel"))))),
        ast.if_(var_("pixel") = ast.call(ast.self(), "processAdjacentPixel", ast.slot(_(ids.direction), _(directions.down)), ast.slot(_(ids.shape), p_(ids.shape)), ast.slot(_(ids.checkPixels), p_(ids.checkPixels)), ast.slot(_(ids.checkPixel), p_(ids.checkPixel))),
                ast.block(
                    ast.call(ast.self(), "processAdjacentPixel", ast.slot(_(ids.direction), _(directions.left)), ast.slot(_(ids.shape), p_(ids.shape)), ast.slot(_(ids.checkPixels), p_(ids.checkPixels)), ast.slot(_(ids.checkPixel), *var_("pixel"))),
                    ast.call(ast.self(), "processAdjacentPixel", ast.slot(_(ids.direction), _(directions.right)), ast.slot(_(ids.shape), p_(ids.shape)), ast.slot(_(ids.checkPixels), p_(ids.checkPixels)), ast.slot(_(ids.checkPixel), *var_("pixel"))))),
        ast.call(ast.self(), "processAdjacentPixel", ast.slot(_(ids.direction), _(directions.left)), ast.slot(_(ids.shape), p_(ids.shape)), ast.slot(_(ids.checkPixels), p_(ids.checkPixels)), ast.slot(_(ids.checkPixel), p_(ids.checkPixel))),
        ast.call(ast.self(), "processAdjacentPixel", ast.slot(_(ids.direction), _(directions.right)), ast.slot(_(ids.shape), p_(ids.shape)), ast.slot(_(ids.checkPixels), p_(ids.checkPixels)), ast.slot(_(ids.checkPixel), p_(ids.checkPixel))));

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
        param(ids.direction, _(type.Directions)),
        param(ids.shape, struct_("Shape")),
        param(ids.checkPixels, tt_("std::List", ids.objectType, "Pixel")),
        param(ids.checkPixel, struct_("Pixel")));
    shaperProcessAdjacentPixel.returnType(struct_("Pixel")); // maybe hybrid pixel?!
    shaperProcessAdjacentPixel.code(
        ast.if_(ast.call(*var_("checkPixel"), "has", ast.slot(_(ids.direction), p_(ids.direction))),
                ast.block(
                    var_("pixel") = _(ids.checkPixel) / _(ids.direction),
                    ast.if_(ast.and_(ast.same(*var_("pixel") / _(ids.color), *var_("shape") / _(ids.color)), ast.not_(ast.call(*var_("shape"), "hasPixel", ast.slot(_(ids.pixel), *var_("pixel"))))),
                            ast.call(*var_("checkPixels"), "insert", ast.slot(_(ids.pixel), *var_("pixel")))),
                    ast.return_(_(ids.pixel))),
                ast.return_(_(ids.emptyObject))));
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
    compiledGlobalScopePtr    = &compiledGlobalScope[ids.data];
    m_initPhase               = InitPhase::FullyConstructed;

    // Test should be removed from here
    auto& compiledStructs        = static_cast<TrieMap&>(compiledGlobalScope[ids.data][ids.structs]);
    auto& compiledListItemStruct = static_cast<TrieMap&>(compiledGlobalScope[ids.data][ids.structs]).getValue(templateId("ListItem", ids.objectType, type.Cell));
    auto& compiledListStruct     = static_cast<TrieMap&>(compiledGlobalScope[ids.data][ids.structs]).getValue(templateId("List", ids.objectType, type.Cell));
    auto& compiledTypeStruct     = static_cast<TrieMap&>(compiledGlobalScope[ids.data][ids.structs]).getValue(id("Type"));
    auto& compiledIndexStruct    = static_cast<TrieMap&>(compiledGlobalScope[ids.data][ids.structs]).getValue(id("Index"));
    type.ListItem.set(ids.methods, compiledListItemStruct[ids.methods]);
    type.List.set(ids.methods, compiledListStruct[ids.methods]);
    type.Type_.set(ids.methods, compiledTypeStruct[ids.methods]);

    Object testType(*this, compiledTypeStruct, id("constructor"), "testType");
    Object testRecursiveType(*this, compiledTypeStruct, id("constructorWithRecursiveType"), "testRecursiveType");

    Object testIndex(*this, compiledIndexStruct, id("constructor"), "testIndex");
    testIndex.method(id("insert"), { ids.key, _1_ }, { ids.value, _2_ });
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
