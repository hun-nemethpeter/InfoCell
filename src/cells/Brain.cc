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
    input(kb, kb.type.Cell, "input"),
    item(kb, kb.type.Cell, "item"),
    key(kb, kb.type.Cell, "key"),
    keyType(kb, kb.type.Cell, "keyType"),
    label(kb, kb.type.Cell, "label"),
    lhs(kb, kb.type.Cell, "lhs"),
    list(kb, kb.type.Cell, "list"),
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

namespace type {
Op::Op(brain::Brain& kb) :
    Base(kb, "op::Base"),
    Block(kb, "op::Block"),
    EvalVar(kb, "op::EvalVar"),
    Function(kb, "op::Function"),
    Delete(kb, "op::Delete"),
    Set(kb, "op::Set"),
    If(kb, "op::If"),
    Do(kb, "op::Do"),
    While(kb, "op::While"),
    Expression(kb, "op::Expression"),
    ConstVar(kb, "op::ConstVar"),
    Var(kb, "op::Var"),
    New(kb, "op::New"),
    Same(kb, "op::Same"),
    NotSame(kb, "op::NotSame"),
    Equal(kb, "op::Equal"),
    NotEqual(kb, "op::NotEqual"),
    Has(kb, "op::Has"),
    Get(kb, "op::Get"),
    And(kb, "op::And"),
    Or(kb, "op::Or"),
    Not(kb, "op::Not"),
    Add(kb, "op::Add"),
    Subtract(kb, "op::Subtract"),
    Multiply(kb, "op::Multiply"),
    Divide(kb, "op::Divide"),
    LessThan(kb, "op::LessThan"),
    GreaterThan(kb, "op::GreaterThan")
{
    auto& coding = kb.coding;
    auto& type   = kb.type;

    Same.addSlots(
        coding.slot(coding.lhs, Base),
        coding.slot(coding.rhs, Base),
        coding.slot(coding.value, type.Boolean));

    NotSame.addSlots(
        coding.slot(coding.lhs, Base),
        coding.slot(coding.rhs, Base),
        coding.slot(coding.value, type.Boolean));

    Equal.addSlots(
        coding.slot(coding.lhs, Base),
        coding.slot(coding.rhs, Base),
        coding.slot(coding.value, type.Boolean));

    NotEqual.addSlots(
        coding.slot(coding.lhs, Base),
        coding.slot(coding.rhs, Base),
        coding.slot(coding.value, type.Boolean));

    Has.addSlots(
        coding.slot(coding.cell, Base),
        coding.slot(coding.role, Base),
        coding.slot(coding.value, type.Boolean));

    Get.addSlots(
        coding.slot(coding.cell, Base),
        coding.slot(coding.role, Base),
        coding.slot(coding.value, type.Cell));

    Set.addSlots(
        coding.slot(coding.cell, Base),
        coding.slot(coding.role, Base),
        coding.slot(coding.value, Base));

    And.addSlots(
        coding.slot(coding.lhs, Base),
        coding.slot(coding.rhs, Base),
        coding.slot(coding.value, type.Boolean));

    Or.addSlots(
        coding.slot(coding.lhs, Base),
        coding.slot(coding.rhs, Base),
        coding.slot(coding.value, type.Boolean));

    Not.addSlots(
        coding.slot(coding.input, Base),
        coding.slot(coding.value, type.Boolean));

    Add.addSlots(
        coding.slot(coding.lhs, Base),
        coding.slot(coding.rhs, Base),
        coding.slot(coding.value, type.Number));

    Subtract.addSlots(
        coding.slot(coding.lhs, Base),
        coding.slot(coding.rhs, Base),
        coding.slot(coding.value, type.Number));

    Multiply.addSlots(
        coding.slot(coding.lhs, Base),
        coding.slot(coding.rhs, Base),
        coding.slot(coding.value, type.Number));

    Divide.addSlots(
        coding.slot(coding.lhs, Base),
        coding.slot(coding.rhs, Base),
        coding.slot(coding.value, type.Number));

    LessThan.addSlots(
        coding.slot(coding.lhs, Base),
        coding.slot(coding.rhs, Base),
        coding.slot(coding.value, type.Boolean));

    GreaterThan.addSlots(
        coding.slot(coding.lhs, Base),
        coding.slot(coding.rhs, Base),
        coding.slot(coding.value, type.Boolean));

    ConstVar.addSlots(
        coding.slot(coding.value, type.Cell));

    Var.addSlots(
        coding.slot(coding.objectType, type.Type_),
        coding.slot(coding.value, type.Cell));

    New.addSlots(
        coding.slot(coding.value, type.Cell),
        coding.slot(coding.objectType, Base));

    Delete.addSlots(
        coding.slot(coding.input, Base));

    If.addSlots(
        coding.slot(coding.condition, Base),
        coding.slot(coding.then, Base),
        coding.slot(coding.else_, Base));

    Do.addSlots(
        coding.slot(coding.condition, Base),
        coding.slot(coding.statement, Base));

    While.addSlots(
        coding.slot(coding.condition, Base),
        coding.slot(coding.statement, Base));

    Block.addSlots(
        coding.slot(coding.ops, type.Cell),
        coding.slot(coding.value, type.Cell));

    EvalVar.addSlots(
        coding.slot(coding.value, Var));

    Function.addSlots(
        coding.slot(coding.input, type.MapOf(Base)),
        coding.slot(coding.ast, type.ast.Base),
        coding.slot(coding.op, type.ListOf(Base)),
        coding.slot(coding.output, type.MapOf(Base)));
}

Ast::Ast(brain::Brain& kb) :
    kb(kb),
    Base(kb, "ast::Base"),
    Parameter(kb, "ast::Parameter"),
    Slot(kb, "ast::Slot"),
    Call(kb, "ast::Call"),
    StaticCall(kb, "ast::StaticCall"),
    Cell(kb, "ast::Cell"),
    Self(kb, "ast::Self"),
    SelfFn(kb, "ast::SelfFn"),
    Block(kb, "ast::Block"),
    Function(kb, "ast::Function"),
    Delete(kb, "ast::Delete"),
    Set(kb, "ast::Set"),
    If(kb, "ast::If"),
    Do(kb, "ast::Do"),
    While(kb, "ast::While"),
    Expression(kb, "ast::Expression"),
    ConstVar(kb, "ast::ConstVar"),
    Var(kb, "ast::Var"),
    Member(kb, "ast::Member"),
    New(kb, "ast::New"),
    Same(kb, "ast::Same"),
    NotSame(kb, "ast::NotSame"),
    Equal(kb, "ast::Equal"),
    NotEqual(kb, "ast::NotEqual"),
    Has(kb, "ast::Has"),
    Get(kb, "ast::Get"),
    And(kb, "ast::And"),
    Or(kb, "ast::Or"),
    Not(kb, "ast::Not"),
    Add(kb, "ast::Add"),
    Subtract(kb, "ast::Subtract"),
    Multiply(kb, "ast::Multiply"),
    Divide(kb, "ast::Divide"),
    LessThan(kb, "ast::LessThan"),
    GreaterThan(kb, "ast::GreaterThan")
{
    auto& coding = kb.coding;
    auto& type   = kb.type;

    ConstVar.addSlots(
        coding.slot(coding.value, Base));

    Var.addSlots(
        coding.slot(coding.role, Base));

    Member.addSlots(
        coding.slot(coding.role, Base));

    New.addSlots(
        coding.slot(coding.objectType, Base),
        coding.slot(coding.constructor, Base),
        coding.slot(kb.coding.parameters, kb.type.ListOf(kb.type.ast.Slot)));

    Delete.addSlots(
        coding.slot(coding.cell, Base));

    If.addSlots(
        coding.slot(coding.condition, Base),
        coding.slot(coding.then, Base),
        coding.slot(coding.else_, Base));

    Do.addSlots(
        coding.slot(coding.condition, Base),
        coding.slot(coding.statement, Base));

    While.addSlots(
        coding.slot(coding.condition, Base),
        coding.slot(coding.statement, Base));

    And.addSlots(
        coding.slot(coding.lhs, Base),
        coding.slot(coding.rhs, Base));

    Or.addSlots(
        coding.slot(coding.lhs, Base),
        coding.slot(coding.rhs, Base));

    Not.addSlots(
        coding.slot(coding.value, Base));

    Add.addSlots(
        coding.slot(coding.lhs, Base),
        coding.slot(coding.rhs, Base));

    Subtract.addSlots(
        coding.slot(coding.lhs, Base),
        coding.slot(coding.rhs, Base));

    Multiply.addSlots(
        coding.slot(coding.lhs, Base),
        coding.slot(coding.rhs, Base));

    Divide.addSlots(
        coding.slot(coding.lhs, Base),
        coding.slot(coding.rhs, Base));

    LessThan.addSlots(
        coding.slot(coding.lhs, Base),
        coding.slot(coding.rhs, Base));

    GreaterThan.addSlots(
        coding.slot(coding.lhs, Base),
        coding.slot(coding.rhs, Base));

    Same.addSlots(
        coding.slot(coding.lhs, Base),
        coding.slot(coding.rhs, Base));

    NotSame.addSlots(
        coding.slot(coding.lhs, Base),
        coding.slot(coding.rhs, Base));

    Equal.addSlots(
        coding.slot(coding.lhs, Base),
        coding.slot(coding.rhs, Base));

    NotEqual.addSlots(
        coding.slot(coding.lhs, Base),
        coding.slot(coding.rhs, Base));

    Has.addSlots(
        coding.slot(coding.cell, Base),
        coding.slot(coding.role, Base));

    Get.addSlots(
        coding.slot(coding.cell, Base),
        coding.slot(coding.role, Base));

    Set.addSlots(
        coding.slot(coding.cell, Base),
        coding.slot(coding.role, Base),
        coding.slot(coding.value, Base));

    Parameter.addSlots(
        coding.slot(coding.role, type.Cell));

    Slot.addSlots(
        coding.slot(coding.slotRole, Base),
        coding.slot(coding.slotType, Base));

    Cell.addSlots(
        coding.slot(coding.value, type.Cell));

    Call.addSlots(
        coding.slot(kb.coding.cell, Base),
        coding.slot(kb.coding.method, Base),
        coding.slot(kb.coding.parameters, type.ListOf(Slot)));

    StaticCall.addSlots(
        coding.slot(kb.coding.cell, Base),
        coding.slot(kb.coding.method, Base),
        coding.slot(kb.coding.parameters, type.ListOf(Slot)));

    Block.addSlots(
        coding.slot(coding.asts, type.Cell));

    Function.addSlots(
        coding.slot(coding.input, type.ListOf(Slot)),
        coding.slot(coding.ast, type.ListOf(Base)),
        coding.slot(coding.output, type.ListOf(Slot)));
}

} // namespace type

Types::Types(brain::Brain& kb) :
    kb(kb),
    Type_(kb, "Type"),
    Cell(kb, "Cell"),
    Slot(kb, "Slot"),
    Container(kb, "Conatainer"),
    Iterator(kb, "Iterator"),
    List(kb, "List"),
    ListItem(kb, "ListItem"),
    Map(kb, "Map"),
    Index(kb, "Index"),
    Boolean(kb, "Boolean"),
    Char(kb, "Char"),
    Digit(kb, "Digit"),
    Number(kb, "Number"),
    String(kb, "String"),
    Color(kb, "Color"),
    Pixel(kb, "Pixel"),
    Picture(kb, "Picture"),
    op(kb),
    ast(kb)
{
    Slot.addSlots(
        kb.coding.slot(kb.coding.slotType, kb.type.Type_),
        kb.coding.slot(kb.coding.slotRole, kb.type.Cell));
    kb.m_initPhase = Brain::InitPhase::SlotTypeInitialzed;
}

Type& Types::ListOf(CellI& type)
{
    auto typeIt = m_listTypes.find(&type);
    if (typeIt != m_listTypes.end()) {
        return typeIt->second;
    } else {
        auto it        = m_listTypes.emplace(std::piecewise_construct,
                                             std::forward_as_tuple(&type),
                                             std::forward_as_tuple(kb, std::format("List<{}>", type.label())));
        Type& listType = it.first->second;
        Type& itemType = *new Type(kb, std::format("ListItem<{}>", type.label()));
        listType.addSubType(kb.coding.objectType, itemType);
        listType.addMembership(kb.type.List);
        itemType.addMembership(kb.type.ListItem);
        auto& coding = kb.coding;

        listType.addSlots(coding.slot(kb.sequence.first, itemType),
                          coding.slot(kb.sequence.last, itemType),
                          coding.slot(kb.dimensions.size, kb.type.Number),
                          coding.slot(kb.coding.objectType, type));
        itemType.addSlots(coding.slot(kb.sequence.previous, itemType),
                          coding.slot(kb.sequence.next, itemType),
                          coding.slot(kb.coding.value, type));

        return it.first->second;
    }
}

Type& Types::MapOf(CellI& type)
{
    auto typeIt = m_mapTypes.find(&type);
    if (typeIt != m_mapTypes.end()) {
        return typeIt->second;
    } else {
        auto it       = m_mapTypes.emplace(std::piecewise_construct,
                                           std::forward_as_tuple(&type),
                                           std::forward_as_tuple(kb, std::format("Map<{}>", type.label())));
        Type& mapType = it.first->second;
        auto& coding  = kb.coding;
        mapType.addMembership(kb.type.Map);
        mapType.addSlots(coding.slot(kb.coding.list, ListOf(type)),
                         coding.slot(kb.coding.index, kb.type.Index),
                         coding.slot(kb.coding.keyType, kb.type.Cell),
                         coding.slot(kb.coding.objectType, type),
                         coding.slot(kb.dimensions.size, kb.type.Number));

        return it.first->second;
    }
}

// ============================================================================
Ast::Base::Base(brain::Brain& kb, CellI& classCell, const std::string& label) :
    Object(kb, classCell, label)
{
}

Ast::Parameter::Parameter(brain::Brain& kb, CellI& role) :
    BaseT<Parameter>(kb, kb.type.ast.Parameter)
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
    cells::op::Function& function = *new cells::op::Function(kb);
    compileParams(function, type);
    function.set(kb.coding.ast, asts());
    function.set(kb.coding.op, compileAst(asts(), function, type));

    return function;
}

void Ast::Function::compileParams(cells::op::Function& function, CellI* type)
{
    std::stringstream iss;
    std::stringstream oss;
    if (m_inputs || type) {
        Map& params = *new Map(kb, kb.type.op.Var);
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
                iss << "p_" << slot[kb.coding.slotRole].label() << ": " << slot[kb.coding.slotType].label();
                Object& var = *new Object(kb, kb.type.op.Var, std::format("p_{}", slot[kb.coding.slotRole].label()));
                var.set(kb.coding.objectType, slot[kb.coding.slotType]);
                params.add(slot[kb.coding.slotRole], var);
            });
        }
        function.addInputs(params);
    }
    if (m_outputs) {
        Map& params = *new Map(kb, kb.type.op.Var);
        Visitor::visitList(outputs(), [this, &params, &oss](CellI& slot, int i) {
            if (!params.empty()) {
                oss << ", ";
            }
            oss << "o_" << slot[kb.coding.slotRole].label() << ": " << slot[kb.coding.slotType].label();
            Object& var = *new Object(kb, kb.type.op.Var, std::format("o_{}", slot[kb.coding.slotRole].label()));
            var.set(kb.coding.objectType, slot[kb.coding.slotType]);
            params.add(slot[kb.coding.slotRole], var);
        });
        function.addOutputs(params);
    }
    if (m_outputs) {
        function.label(std::format("fn {}({}) -> ({})", label(), iss.str(), oss.str()));
    } else {
        function.label(std::format("fn {}({})", label(), iss.str()));
    }
}

CellI& Ast::Function::compileAst(CellI& ast, cells::op::Function& function, CellI* type)
{
    auto compile = [this, &function, type](CellI& ast) -> CellI& { return compileAst(ast, function, type); };

    if (&ast.type() == &kb.type.ast.Block) {
        CellI& list        = ast[kb.coding.asts];
        auto& compiledAsts = *new cells::List(kb, kb.type.op.Base);
        Visitor::visitList(list, [this, &compiledAsts, &ast, &function, type](CellI& ast, int) {
            compiledAsts.add(compileAst(ast, function, type));
        });
        Object& opBlock = *new Object(kb, kb.type.op.Block);
        opBlock.set(kb.coding.ops, compiledAsts);
        return opBlock;
    } else if (&ast.type() == &kb.type.ast.Cell) {
        return *new op::ConstVar(kb, ast[kb.coding.value]);
    } else if (&ast.type() == &kb.type.ast.SelfFn) {
        return *new op::ConstVar(kb, function);
    } else if (&ast.type() == &kb.type.ast.Self) {
        return function[kb.coding.input][kb.coding.index][kb.coding.self];
    } else if (&ast.type() == &kb.type.ast.Parameter) {
        return function[kb.coding.input][kb.coding.index][ast[kb.coding.role]];
    } else if (&ast.type() == &kb.type.ast.Delete) {
        Object& opDelete = *new Object(kb, kb.type.op.Delete);
        opDelete.set(kb.coding.input, compile(ast[kb.coding.cell]));
        return opDelete;
    } else if (&ast.type() == &kb.type.ast.Set) {
        Object& opSet = *new Object(kb, kb.type.op.Set);
        opSet.set(kb.coding.cell, compile(ast[kb.coding.cell]));
        opSet.set(kb.coding.role, compile(ast[kb.coding.role]));
        opSet.set(kb.coding.value, compile(ast[kb.coding.value]));
        return opSet;
    } else if (&ast.type() == &kb.type.ast.If) {
        if (ast.has(kb.coding.else_)) {
            return *new op::If(kb, compile(ast[kb.coding.condition]), compile(ast[kb.coding.then]), compile(ast[kb.coding.else_]));
        } else {
            return *new op::If(kb, compile(ast[kb.coding.condition]), compile(ast[kb.coding.then]));
        }
    } else if (&ast.type() == &kb.type.ast.Do) {
        return *new op::Do(kb, compile(ast[kb.coding.condition]), compile(ast[kb.coding.statement]));
    } else if (&ast.type() == &kb.type.ast.While) {
        return *new op::While(kb, compile(ast[kb.coding.condition]), compile(ast[kb.coding.statement]));
    } else if (&ast.type() == &kb.type.ast.ConstVar) {
        return *new op::ConstVar(kb, compile(ast[kb.coding.value]));
    } else if (&ast.type() == &kb.type.ast.Var) {
        return *new op::ConstVar(kb, function.getOrCreateVar(ast[kb.coding.role], kb.type.Cell));
    } else if (&ast.type() == &kb.type.ast.New) {
        auto& compiledAsts = *new cells::List(kb, kb.type.op.Base);

        Object& block = *new Object(kb, kb.type.op.Block);
        block.set(kb.coding.ops, compiledAsts);
        Object& opSet = *new Object(kb, kb.type.op.Set, "block.value = new objectType()");
        opSet.set(kb.coding.cell, compile(kb.ast.cell(block)));
        opSet.set(kb.coding.role, compile(kb.ast.cell(kb.coding.value)));
        opSet.set(kb.coding.value, *new op::New(kb, compile(ast[kb.coding.objectType])));

        compiledAsts.add(opSet);
        if (ast.has(kb.coding.constructor)) {
            Object callAst(kb, kb.type.ast.Call);
            callAst.set(kb.coding.cell, kb.ast.get(kb.ast.cell(block), kb.ast.cell(kb.coding.value)));
            callAst.set(kb.coding.method, ast[kb.coding.constructor]);
            callAst.set(kb.coding.parameters, ast[kb.coding.parameters]);
            compiledAsts.add(compile(callAst));
        }
        return block;
    } else if (&ast.type() == &kb.type.ast.Call) {
        auto& compiledAsts  = *new cells::List(kb, kb.type.op.Base);
        Object& block      = *new Object(kb, kb.type.op.Block, "Call { ... }");
        block.set(kb.coding.ops, compiledAsts);
        Ast::Get& getMethod = kb.ast.get(kb.ast.get(kb.ast.get(kb.ast.get(static_cast<Ast::Base&>(ast[kb.coding.cell]), kb.ast.cell(kb.coding.type)), kb.ast.cell(kb.coding.methods)), kb.ast.cell(kb.coding.index)), static_cast<Ast::Base&>(ast[kb.coding.method]));
        Object& varMethod   = *new Object(kb, kb.type.op.Var, "Call { var method; }");
        varMethod.set(kb.coding.objectType, kb.type.op.Function);
        CellI& storeMethod  = compile(kb.ast.set(kb.ast.cell(varMethod), kb.ast.cell(kb.coding.value), getMethod));
        CellI& setSelf      = compile(kb.ast.set(kb.ast.get(kb.ast.get(kb.ast.get(kb.ast.get(kb.ast.cell(varMethod), kb.ast.cell(kb.coding.value)), kb.ast.cell(kb.coding.input)), kb.ast.cell(kb.coding.index)), kb.ast.cell(kb.coding.self)), kb.ast.cell(kb.coding.value), static_cast<Ast::Base&>(ast[kb.coding.cell])));
        compiledAsts.add(storeMethod);
        compiledAsts.add(setSelf);
        storeMethod.label("Call { storeMethod; }");
        setSelf.label("Call { setSelf; }");
        Visitor::visitList(ast[kb.coding.parameters], [this, &ast, &function, type, &compiledAsts, &varMethod](CellI& ast, int) {
            CellI& setParam = compileAst(kb.ast.set(kb.ast.get(kb.ast.get(kb.ast.get(kb.ast.get(kb.ast.cell(varMethod), kb.ast.cell(kb.coding.value)), kb.ast.cell(kb.coding.input)), kb.ast.cell(kb.coding.index)), static_cast<Ast::Base&>(ast[kb.coding.slotRole])), kb.ast.cell(kb.coding.value), static_cast<Ast::Base&>(ast[kb.coding.slotType])), function, type);
            setParam.label("Call { setParam; }");
            compiledAsts.add(setParam);
        });
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
        Object& block      = *new Object(kb, kb.type.op.Block, "Call { ... }");
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
        Visitor::visitList(ast[kb.coding.parameters], [this, &ast, &function, type, &compiledAsts, &varMethod](CellI& ast, int) {
            CellI& setParam = compileAst(kb.ast.set(kb.ast.get(kb.ast.get(kb.ast.get(kb.ast.get(kb.ast.cell(varMethod), kb.ast.cell(kb.coding.value)), kb.ast.cell(kb.coding.input)), kb.ast.cell(kb.coding.index)), static_cast<Ast::Base&>(ast[kb.coding.slotRole])), kb.ast.cell(kb.coding.value), static_cast<Ast::Base&>(ast[kb.coding.slotType])), function, type);
            setParam.label("Call { setParam; }");
            compiledAsts.add(setParam);
        });
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
        return *new op::And(kb, compile(ast[kb.coding.lhs]), compile(ast[kb.coding.rhs]));
    } else if (&ast.type() == &kb.type.ast.Or) {
        return *new op::Or(kb, compile(ast[kb.coding.lhs]), compile(ast[kb.coding.rhs]));
    } else if (&ast.type() == &kb.type.ast.Not) {
        return *new op::Not(kb, compile(ast[kb.coding.value]));
    } else if (&ast.type() == &kb.type.ast.Add) {
        return *new op::Add(kb, compile(ast[kb.coding.lhs]), compile(ast[kb.coding.rhs]));
    } else if (&ast.type() == &kb.type.ast.Subtract) {
        return *new op::Subtract(kb, compile(ast[kb.coding.lhs]), compile(ast[kb.coding.rhs]));
    } else if (&ast.type() == &kb.type.ast.Multiply) {
        return *new op::Multiply(kb, compile(ast[kb.coding.lhs]), compile(ast[kb.coding.rhs]));
    } else if (&ast.type() == &kb.type.ast.Divide) {
        return *new op::Divide(kb, compile(ast[kb.coding.lhs]), compile(ast[kb.coding.rhs]));
    } else if (&ast.type() == &kb.type.ast.LessThan) {
        return *new op::LessThan(kb, compile(ast[kb.coding.lhs]), compile(ast[kb.coding.rhs]));
    } else if (&ast.type() == &kb.type.ast.GreaterThan) {
        return *new op::GreaterThan(kb, compile(ast[kb.coding.lhs]), compile(ast[kb.coding.rhs]));
    } else if (&ast.type() == &kb.type.ast.Same) {
        return *new op::Same(kb, compile(ast[kb.coding.lhs]), compile(ast[kb.coding.rhs]));
    } else if (&ast.type() == &kb.type.ast.NotSame) {
        return *new op::NotSame(kb, compile(ast[kb.coding.lhs]), compile(ast[kb.coding.rhs]));
    } else if (&ast.type() == &kb.type.ast.Equal) {
        return *new op::Equal(kb, compile(ast[kb.coding.lhs]), compile(ast[kb.coding.rhs]));
    } else if (&ast.type() == &kb.type.ast.NotEqual) {
        return *new op::NotEqual(kb, compile(ast[kb.coding.lhs]), compile(ast[kb.coding.rhs]));
    } else if (&ast.type() == &kb.type.ast.Has) {
        return *new op::Has(kb, compile(ast[kb.coding.cell]), compile(ast[kb.coding.role]));
    } else if (&ast.type() == &kb.type.ast.Get) {
        return *new op::Get(kb, compile(ast[kb.coding.cell]), compile(ast[kb.coding.role]));
    } else if (&ast.type() == &kb.type.ast.Member) {
        return compile(kb.ast.get(kb.ast.self(), static_cast<Ast::Base&>(ast[kb.coding.role])));
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

Ast::Do::Do(brain::Brain& kb, Base& condition, Base& statement) :
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

Ast::ConstVar::ConstVar(brain::Brain& kb, CellI& cell) :
    BaseT<ConstVar>(kb, kb.type.ast.ConstVar)
{
    set(kb.coding.value, cell);
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
    Ast::Set& ret = Set::New(kb, Self::New(kb), static_cast<Ast::Base&>(get(kb.coding.role)), value);
    delete this;
    return ret;
}

Ast::Get& Ast::Member::operator/(Base& role)
{
    if (has(kb.coding.value)) {
        throw "Member already set!";
    }
    return Get::New(kb, *this, role);
}

Ast::Has& Ast::Member::exist()
{
    return Has::New(kb, Self::New(kb), static_cast<Ast::Base&>(get(kb.coding.role)));
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

Ast::Not::Not(brain::Brain& kb, Base& value) :
    BaseT<Not>(kb, kb.type.ast.Not)
{
    set(kb.coding.value, value);
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

Ast::Set& Ast::return_(Base& value)
{
    auto& ast = kb.ast;
    return ast.set(ast.get(ast.get(ast.get(ast.selfFn(), ast.cell(kb.coding.output)), ast.cell(kb.coding.index)), ast.cell(kb.coding.value)), ast.cell(kb.coding.value), value);
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

Ast::If& Ast::if_(Base& condition, Base& thenBranch)
{
    return If::New(kb, condition, thenBranch);
}

Ast::If& Ast::if_(Base& condition, Base& thenBranch, Base& elseBranch)
{
    return If::New(kb, condition, thenBranch, elseBranch);
}

Ast::Do& Ast::do_(Base& condition, Base& statement)
{
    return Do::New(kb, condition, statement);
}

Ast::While& Ast::while_(Base& condition, Base& statement)
{
    return While::New(kb, condition, statement);
}

Ast::ConstVar& Ast::ref(CellI& cell)
{
    return ConstVar::New(kb, cell);
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
    sign(kb, kb.type.Cell, "sign"),
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
    Demonstration(kb, "Demonstration"),
    Task(kb, "Task"),
    examples(kb, kb.type.Cell)
{
    auto& coding = kb.coding;

    Demonstration.addSlots(
        coding.slot(kb.coding.input, kb.type.Picture),
        coding.slot(kb.coding.output, kb.type.Picture));

    Task.addSlots(
        coding.slot(examples, kb.type.ListOf(Demonstration)),
        coding.slot(kb.coding.input, kb.type.Picture),
        coding.slot(kb.coding.output, kb.type.Picture));
}

Brain::Brain() :
    m_initPhase(InitPhase::Init),
    sequence(*this),
    dimensions(*this),
    coding(*this),
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
    const auto _   = [this](CellI& cell) -> Ast::Cell& { return ast.cell(cell); };
    const auto p_  = [this](CellI& role) -> Ast::Parameter& { return ast.parameter(role); };
    const auto m   = [this](Ast::Base& role) -> Ast::Member& { return ast.member(role); };
    const auto m_  = [this](CellI& role) -> Ast::Member& { return ast.member(ast.cell(role)); };
    const auto var = [this](CellI& role) -> Ast::Var& { return ast.var(role); };

    type.Type_.addSlots(
        coding.slot(coding.slots, type.MapOf(type.Slot)),
        coding.slot(coding.subTypes, type.MapOf(type.Type_)),
        coding.slot(coding.memberOf, type.MapOf(type.Type_)),
        coding.slot(coding.asts, type.MapOf(type.ast.Function)),
        coding.slot(coding.methods, type.MapOf(type.op.Function)));

    type.ListItem.addSlots(
        coding.slot(sequence.previous, type.ListItem),
        coding.slot(sequence.next, type.ListItem),
        coding.slot(coding.value, type.Cell));
    type.ListItem.addMembership(
        type.Iterator);

    type.List.addSlots(
        coding.slot(sequence.first, type.ListItem),
        coding.slot(sequence.last, type.ListItem),
        coding.slot(dimensions.size, type.Number),
        coding.slot(coding.objectType, type.Cell),
        coding.slot(coding.item, type.Type_));
    type.List.addSubType(
        coding.objectType, type.ListItem);
    type.List.addMembership(
        type.Container);

    type.Map.addSlots(
        coding.slot(coding.keyType, type.Cell),
        coding.slot(coding.objectType, type.Cell),
        coding.slot(coding.list, type.ListOf(type.Cell)),
        coding.slot(coding.index, type.Index),
        coding.slot(dimensions.size, type.Number));

    Ast::Function& mapCtor = *new Ast::Function(*this, "Map::Ctor");
    mapCtor.addInputs(list(
        ast.slot(coding.keyType, type.Cell),
        ast.slot(coding.objectType, type.Cell)));
    mapCtor.addAsts(ast.block(
        m_(dimensions.size)   = _(_0_),
        m_(coding.keyType)    = p_(coding.keyType),
        m_(coding.objectType) = p_(coding.objectType)));
    type.Map.addMethod(coding.constructor, mapCtor);

    Ast::Function& mapAdd = *new Ast::Function(*this, "Map::Add");
    mapAdd.addInputs(list(
        ast.slot(coding.key, type.Cell),
        ast.slot(coding.value, type.Cell)));
    mapAdd.addAsts(ast.block(
        ast.if_(ast.equal(m_(dimensions.size), _(_0_)),
                ast.block(m_(coding.list)  = ast.new_(_(type.List), _(coding.constructor), ast.slot(_(coding.objectType), m_(coding.objectType))),
                          m_(coding.index) = ast.new_(_(type.Index)))),
        m_(coding.list).call(_(sequence.add), ast.slot(_(coding.value), p_(coding.value))),
        ast.set(m_(coding.index), p_(coding.key), p_(coding.value)),
        m_(dimensions.size) = ast.add(m_(dimensions.size), _(_1_))));
    type.Map.addMethod(sequence.add, mapAdd);

    Ast::Function& mapSize = *new Ast::Function(*this, "Map::Size");
    mapSize.addOutputs(list(
        ast.slot(coding.value, type.Number)));
    mapSize.addAsts(ast.block(
        ast.return_(m_(dimensions.size))));
    type.Map.addMethod(dimensions.size, mapSize);

    Ast::Function& mapEmpty = *new Ast::Function(*this, "Map::Empty");
    mapEmpty.addOutputs(list(
        ast.slot(coding.value, type.Boolean)));
    mapEmpty.addAsts(ast.block(
        ast.if_(ast.equal(m_(dimensions.size), _(_0_)), ast.return_(_(boolean.true_)), ast.return_(_(boolean.false_)))));
    type.Map.addMethod(sequence.empty, mapEmpty);

    Ast::Function& typeCtor = *new Ast::Function(*this, "Type::Ctor");
    typeCtor.addInputs(list(
        ast.slot(coding.slots, type.MapOf(type.Slot)),
        ast.slot(coding.subTypes, type.MapOf(type.Type_)),
        ast.slot(coding.memberOf, type.MapOf(type.Type_)),
        ast.slot(coding.methods, type.MapOf(type.ast.Function))));
    typeCtor.addAsts(ast.block(
        m_(coding.slots)    = p_(coding.slots),
        m_(coding.subTypes) = p_(coding.subTypes),
        m_(coding.memberOf) = p_(coding.memberOf),
        m_(coding.methods)  = p_(coding.methods)));
    type.Type_.addMethod(coding.constructor, typeCtor);

    Type& mapOfSlot = *new cells::Type(*this, "Map<Cell, Slot>");
    mapOfSlot.addMembership(type.Map);
    mapOfSlot.addSlots(coding.slot(coding.list, type.ListOf(type.Slot)),
                       coding.slot(coding.index, type.Index),
                       coding.slot(coding.keyType, type.Cell),
                       coding.slot(coding.objectType, type.Slot),
                       coding.slot(dimensions.size, type.Number));
    mapOfSlot.addMethod(coding.constructor, mapCtor);
    mapOfSlot.addMethod(dimensions.size, mapSize);
    mapOfSlot.addMethod(sequence.add, mapAdd);
    mapOfSlot.addMethod(sequence.empty, mapEmpty);

    Type& mapOfType = *new cells::Type(*this, "Map<Cell, Type>");
    mapOfType.addMembership(type.Map);
    mapOfType.addSlots(coding.slot(coding.list, type.ListOf(type.Type_)),
                       coding.slot(coding.index, type.Index),
                       coding.slot(coding.keyType, type.Cell),
                       coding.slot(coding.objectType, type.Type_),
                       coding.slot(dimensions.size, type.Number));
    mapOfType.addMethod(coding.constructor, mapCtor);
    mapOfType.addMethod(dimensions.size, mapSize);
    mapOfType.addMethod(sequence.add, mapAdd);
    mapOfType.addMethod(sequence.empty, mapEmpty);

    Ast::Function& listItemTemplate = *new Ast::Function(*this, "ListItem<T>::Ctor");
    listItemTemplate.addInputs(list(
        ast.slot(coding.objectType, type.Type_)));
    listItemTemplate.addOutputs(list(
        ast.slot(coding.value, type.ListItem)));
    listItemTemplate.addAsts(ast.block(
        var(coding.result) = ast.new_(_(type.Type_)),
        ast.set(*var(coding.result), _(coding.slots), ast.new_(_(mapOfType), _(coding.constructor), ast.slot(_(coding.keyType), _(type.Cell)), ast.slot(_(coding.objectType), _(type.Slot)))),
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
        ast.set(*var(coding.item), _(coding.slotType), p_(coding.objectType)),
        ast.call(*var(coding.result) / _(coding.slots), _(sequence.add), ast.slot(_(coding.key), _(coding.value)), ast.slot(_(coding.value), *var(coding.item))),

        ast.set(*var(coding.result), _(coding.memberOf), ast.new_(_(mapOfSlot), _(coding.constructor), ast.slot(_(coding.keyType), _(type.Type_)), ast.slot(_(coding.objectType), _(type.Type_)))),
        ast.call(*var(coding.result) / _(coding.memberOf), _(sequence.add), ast.slot(_(coding.key), _(type.ListItem)), ast.slot(_(coding.value), _(type.ListItem))),

        ast.set(*var(coding.result), _(coding.methods), m_(coding.methods)),

        ast.return_(*var(coding.result))));
    type.ListItem.addMethod(coding.template_, listItemTemplate);

    Ast::Function& listItemCtor = *new Ast::Function(*this, "ListItem::Ctor");
    listItemCtor.addInputs(list(
        ast.slot(coding.value, type.Cell)));
    listItemCtor.addAsts(ast.block(
        m_(coding.value) = p_(coding.value)));
    type.ListItem.addMethod(coding.constructor, listItemCtor);

    Ast::Function& listTemplate = *new Ast::Function(*this, "List<T>::Ctor");
    listTemplate.addInputs(list(
        ast.slot(coding.objectType, type.Type_)));
    listTemplate.addOutputs(list(
        ast.slot(coding.value, type.List)));
    listTemplate.addAsts(ast.block(
        var(type.ListItem) = ast.scall(_(type.ListItem), _(coding.template_), ast.slot(_(coding.objectType), p_(coding.objectType))),
        var(coding.result) = ast.new_(_(type.Type_)),

        ast.set(*var(coding.result), _(coding.subTypes), ast.new_(_(mapOfType), _(coding.constructor), ast.slot(_(coding.keyType), _(type.Type_)), ast.slot(_(coding.objectType), _(type.Type_)))),
        ast.call(*var(coding.result) / _(coding.subTypes), _(sequence.add), ast.slot(_(coding.key), _(coding.objectType)), ast.slot(_(coding.value), *var(type.ListItem))),

        ast.set(*var(coding.result), _(coding.memberOf), ast.new_(_(mapOfSlot), _(coding.constructor), ast.slot(_(coding.keyType), _(type.Type_)), ast.slot(_(coding.objectType), _(type.Type_)))),
        ast.call(*var(coding.result) / _(coding.memberOf), _(sequence.add), ast.slot(_(coding.key), _(type.List)), ast.slot(_(coding.value), _(type.List))),

        ast.set(*var(coding.result), _(coding.methods), m_(coding.methods)),

        ast.set(*var(coding.result), _(coding.slots), ast.new_(_(mapOfSlot), _(coding.constructor), ast.slot(_(coding.keyType), _(type.Cell)), ast.slot(_(coding.objectType), _(type.Slot)))),
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
        ast.set(*var(coding.item), _(coding.slotType), p_(coding.objectType)),
        ast.call(*var(coding.result) / _(coding.slots), _(sequence.add), ast.slot(_(coding.key), _(coding.objectType)), ast.slot(_(coding.value), *var(coding.item))),

        var(coding.item) = ast.new_(_(type.Slot)),
        ast.set(*var(coding.item), _(coding.slotRole), _(coding.item)),
        ast.set(*var(coding.item), _(coding.slotType), *var(type.ListItem)),
        ast.call(*var(coding.result) / _(coding.slots), _(sequence.add), ast.slot(_(coding.key), _(coding.item)), ast.slot(_(coding.value), *var(coding.item))),

        ast.return_(*var(coding.result))));
    type.List.addMethod(coding.template_, listTemplate);

    Ast::Function& listCtor = *new Ast::Function(*this, "List::Ctor");
    listCtor.addInputs(list(
        ast.slot(coding.objectType, type.Type_)));
    listCtor.addAsts(ast.block(
        m_(dimensions.size)   = _(_0_),
        m_(coding.objectType) = p_(coding.objectType),
        m_(coding.item)       = m_(coding.type) / _(coding.subTypes) / _(coding.index) / _(coding.objectType)
    ));
    type.List.addMethod(coding.constructor, listCtor);

    Ast::Function& listAdd = *new Ast::Function(*this, "List::Add");
    listAdd.addInputs(list(
        ast.slot(coding.value, type.Cell)));
    listAdd.addAsts(ast.block(
        var(coding.item) = ast.new_(m_(coding.item), _(coding.constructor), ast.slot(_(coding.value), p_(coding.value))),
        ast.if_(ast.not_(m_(sequence.first).exist()),
                m_(sequence.first) = *var(coding.item),                                    // then
                ast.block(ast.set(m_(sequence.last), _(sequence.next), *var(coding.item)), // else
                          ast.set(*var(coding.item), _(sequence.previous), m_(sequence.last)))),
        m_(sequence.last)   = *var(coding.item),
        m_(dimensions.size) = ast.add(m_(dimensions.size), _(_1_))));
    type.List.addMethod(sequence.add, listAdd);

    Ast::Function& listSize = *new Ast::Function(*this, "List::Size");
    listSize.addOutputs(list(
        ast.slot(coding.value, type.Number)));
    listSize.addAsts(ast.block(
        ast.return_(m_(dimensions.size))));
    type.List.addMethod(dimensions.size, listSize);

    Ast::Function& listEmpty = *new Ast::Function(*this, "List::Empty");
    listEmpty.addOutputs(list(
        ast.slot(coding.value, type.Boolean)));
    listEmpty.addAsts(ast.block(
        ast.if_(ast.equal(m_(dimensions.size), _(_0_)), ast.return_(_(boolean.true_)), ast.return_(_(boolean.false_)))));
    type.List.addMethod(sequence.empty, listEmpty);
#if 0
    listAdd.addAsts(list(
        ListItem* newListItem = new ListItem();
        newListItem->value    = param[coding.value];
        if (!param[coding.self].has(sequence.first)) {
            param[coding.self].set(sequence.first, newListItem);
        } else {
            param[coding.self][sequence.last].set(sequence.next, newListItem);
            newListItem.set(sequence.previous, param[coding.self][sequence.last]);
        }
        param[coding.self].set(sequence.last, newListItem);
    ));
#endif

#if 0
    Object methodData(*this, type.Cell);
    methodData.set(coding.ast, listAdd);
    methodData.set(coding.op, listAdd.compile());
    methodData.set(coding.static_, boolean.false_);
    methodData.set(coding.const_, boolean.false_);
#endif

    type.Number.addSlots(
        coding.slot(coding.value, type.ListOf(type.Digit)),
        coding.slot(numbers.sign, type.Number)); // TODO

    type.String.addSlots(
        coding.slot(coding.value, type.ListOf(type.Char)));

    type.Color.addSlots(
        coding.slot(colors.red, type.Number),
        coding.slot(colors.green, type.Number),
        coding.slot(colors.blue, type.Number));

    type.Pixel.addSlots(
        coding.slot(directions.up, type.Pixel),
        coding.slot(directions.down, type.Pixel),
        coding.slot(directions.left, type.Pixel),
        coding.slot(directions.right, type.Pixel),
        coding.slot(coordinates.x, type.Number),
        coding.slot(coordinates.y, type.Number));

    type.Picture.addSlots(
        coding.slot(dimensions.width, type.Number),
        coding.slot(dimensions.height, type.Number),
        coding.slot(visualization.pixels, type.ListOf(type.Pixel)));

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
