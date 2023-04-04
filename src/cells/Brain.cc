#include "Brain.h"

#include <format>
#include <sstream>

namespace synth {
namespace cells {
namespace brain {

Cells::Cells(brain::Brain& kb) :
    kb(kb),
    type(kb, kb.type.Cell, "type"),
    eval(kb, kb.type.Cell, "eval"),
    constructor(kb, kb.type.Cell, "constructor"),
    destructor(kb, kb.type.Cell, "destructor"),
    slots(kb, kb.type.Cell, "slots"),
    slotType(kb, kb.type.Cell, "slotType"),
    slotRole(kb, kb.type.Cell, "slotRole"),
    subTypes(kb, kb.type.Cell, "subTypes"),
    index(kb, kb.type.Cell, "index"),
    list(kb, kb.type.Cell, "list"),
    memberOf(kb, kb.type.Cell, "memberOf"),
    methods(kb, kb.type.Cell, "methods"),
    emptyObject(kb, kb.type.Cell, "emptyObject")
{
}

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
    argument(kb, kb.type.Cell, "argument"),
    ast(kb, kb.type.Cell, "ast"),
    branch(kb, kb.type.Cell, "branch"),
    cell(kb, kb.type.Cell, "cell"),
    condition(kb, kb.type.Cell, "condition"),
    container(kb, kb.type.Cell, "container"),
    else_(kb, kb.type.Cell, "else_"),
    input(kb, kb.type.Cell, "input"),
    item(kb, kb.type.Cell, "item"),
    key(kb, kb.type.Cell, "key"),
    keyType(kb, kb.type.Cell, "keyType"),
    label(kb, kb.type.Cell, "label"),
    lhs(kb, kb.type.Cell, "lhs"),
    objectType(kb, kb.type.Cell, "objectType"),
    op(kb, kb.type.Cell, "op"),
    output(kb, kb.type.Cell, "output"),
    parameter(kb, kb.type.Cell, "parameter"),
    parameters(kb, kb.type.Cell, "parameters"),
    result(kb, kb.type.Cell, "result"),
    rhs(kb, kb.type.Cell, "rhs"),
    role(kb, kb.type.Cell, "role"),
    self(kb, kb.type.Cell, "self"),
    statement(kb, kb.type.Cell, "statement"),
    template_(kb, kb.type.Cell, "template"),
    then(kb, kb.type.Cell, "then"),
    value(kb, kb.type.Cell, "value")
{
}

namespace type {
Op::Op(brain::Brain& kb) :
    Base(kb, "op::Base"),
    Block(kb, "op::Block"),
    Function(kb, "op::Function"),
    Delete(kb, "op::Delete"),
    Set(kb, "op::Set"),
    If(kb, "op::If"),
    Do(kb, "op::Do"),
    While(kb, "op::While"),
    Expression(kb, "op::Expression"),
    Ref(kb, "op::Ref"),
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
    auto& cells  = kb.cells;
    auto& coding = kb.coding;
    auto& type   = kb.type;

    Same.addSlots(
        cells.slot(coding.lhs, Base),
        cells.slot(coding.rhs, Base),
        cells.slot(coding.value, type.Boolean));

    NotSame.addSlots(
        cells.slot(coding.lhs, Base),
        cells.slot(coding.rhs, Base),
        cells.slot(coding.value, type.Boolean));

    Equal.addSlots(
        cells.slot(coding.lhs, Base),
        cells.slot(coding.rhs, Base),
        cells.slot(coding.value, type.Boolean));

    NotEqual.addSlots(
        cells.slot(coding.lhs, Base),
        cells.slot(coding.rhs, Base),
        cells.slot(coding.value, type.Boolean));

    Has.addSlots(
        cells.slot(coding.cell, Base),
        cells.slot(coding.role, Base),
        cells.slot(coding.value, type.Boolean));

    Get.addSlots(
        cells.slot(coding.cell, Base),
        cells.slot(coding.role, Base),
        cells.slot(coding.value, type.Cell));

    Set.addSlots(
        cells.slot(coding.cell, Base),
        cells.slot(coding.role, Base),
        cells.slot(coding.value, Base));

    And.addSlots(
        cells.slot(coding.lhs, Base),
        cells.slot(coding.rhs, Base),
        cells.slot(coding.value, type.Boolean));

    Or.addSlots(
        cells.slot(coding.lhs, Base),
        cells.slot(coding.rhs, Base),
        cells.slot(coding.value, type.Boolean));

    Not.addSlots(
        cells.slot(coding.input, Base),
        cells.slot(coding.value, type.Boolean));

    Add.addSlots(
        cells.slot(coding.lhs, Base),
        cells.slot(coding.rhs, Base),
        cells.slot(coding.value, type.Number));

    Subtract.addSlots(
        cells.slot(coding.lhs, Base),
        cells.slot(coding.rhs, Base),
        cells.slot(coding.value, type.Number));

    Multiply.addSlots(
        cells.slot(coding.lhs, Base),
        cells.slot(coding.rhs, Base),
        cells.slot(coding.value, type.Number));

    Divide.addSlots(
        cells.slot(coding.lhs, Base),
        cells.slot(coding.rhs, Base),
        cells.slot(coding.value, type.Number));

    LessThan.addSlots(
        cells.slot(coding.lhs, Base),
        cells.slot(coding.rhs, Base),
        cells.slot(coding.value, type.Boolean));

    GreaterThan.addSlots(
        cells.slot(coding.lhs, Base),
        cells.slot(coding.rhs, Base),
        cells.slot(coding.value, type.Boolean));

    Ref.addSlots(
        cells.slot(coding.value, type.Cell));

    Var.addSlots(
        cells.slot(coding.value, type.Cell));

    New.addSlots(
        cells.slot(coding.value, type.Cell),
        cells.slot(coding.objectType, Base));

    Delete.addSlots(
        cells.slot(coding.input, Base));

    If.addSlots(
        cells.slot(coding.condition, Base),
        cells.slot(coding.then, Base),
        cells.slot(coding.else_, Base));

    Do.addSlots(
        cells.slot(coding.condition, Base),
        cells.slot(coding.statement, Base));

    While.addSlots(
        cells.slot(coding.condition, Base),
        cells.slot(coding.statement, Base));

    Function.addSlots(
        cells.slot(coding.input, type.MapOf(Base)),
        cells.slot(coding.ast, type.ast.Base),
        cells.slot(coding.op, type.ListOf(Base)),
        cells.slot(coding.output, type.MapOf(Base)));
}

Ast::Ast(brain::Brain& kb) :
    kb(kb),
    Base(kb, "ast::Base"),
    Parameter(kb, "ast::Parameter"),
    Slot(kb, "ast::Slot"),
    Call(kb, "ast::Call"),
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
    Ref(kb, "ast::Ref"),
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
    auto& cells = kb.cells;
    auto& coding = kb.coding;
    auto& type   = kb.type;

    Ref.addSlots(
        cells.slot(coding.value, Base));

    Var.addSlots(
        cells.slot(coding.role, Base));

    New.addSlots(
        cells.slot(coding.objectType, Base),
        cells.slot(kb.coding.parameters, kb.type.ListOf(kb.type.ast.Slot)));

    Delete.addSlots(
        cells.slot(coding.cell, Base));

    If.addSlots(
        cells.slot(coding.condition, Base),
        cells.slot(coding.then, Base),
        cells.slot(coding.else_, Base));

    Do.addSlots(
        cells.slot(coding.condition, Base),
        cells.slot(coding.statement, Base));

    While.addSlots(
        cells.slot(coding.condition, Base),
        cells.slot(coding.statement, Base));

    And.addSlots(
        cells.slot(coding.lhs, Base),
        cells.slot(coding.rhs, Base));

    Or.addSlots(
        cells.slot(coding.lhs, Base),
        cells.slot(coding.rhs, Base));

    Not.addSlots(
        cells.slot(coding.value, Base));

    Add.addSlots(
        cells.slot(coding.lhs, Base),
        cells.slot(coding.rhs, Base));

    Subtract.addSlots(
        cells.slot(coding.lhs, Base),
        cells.slot(coding.rhs, Base));

    Multiply.addSlots(
        cells.slot(coding.lhs, Base),
        cells.slot(coding.rhs, Base));

    Divide.addSlots(
        cells.slot(coding.lhs, Base),
        cells.slot(coding.rhs, Base));

    LessThan.addSlots(
        cells.slot(coding.lhs, Base),
        cells.slot(coding.rhs, Base));

    GreaterThan.addSlots(
        cells.slot(coding.lhs, Base),
        cells.slot(coding.rhs, Base));

    Same.addSlots(
        cells.slot(coding.lhs, Base),
        cells.slot(coding.rhs, Base));

    NotSame.addSlots(
        cells.slot(coding.lhs, Base),
        cells.slot(coding.rhs, Base));

    Equal.addSlots(
        cells.slot(coding.lhs, Base),
        cells.slot(coding.rhs, Base));

    NotEqual.addSlots(
        cells.slot(coding.lhs, Base),
        cells.slot(coding.rhs, Base));

    Has.addSlots(
        cells.slot(coding.cell, Base),
        cells.slot(coding.role, Base));

    Get.addSlots(
        cells.slot(coding.cell, Base),
        cells.slot(coding.role, Base));

    Set.addSlots(
        cells.slot(coding.cell, Base),
        cells.slot(coding.role, Base),
        cells.slot(coding.value, Base));

    Parameter.addSlots(
        cells.slot(coding.role, type.Cell));

    Slot.addSlots(
        cells.slot(cells.slotRole, Base),
        cells.slot(cells.slotType, Base));

    Cell.addSlots(
        cells.slot(coding.value, type.Cell));

    Call.addSlots(
        cells.slot(kb.coding.cell, Base),
        cells.slot(kb.cells.methods, Base),
        cells.slot(kb.coding.parameters, type.ListOf(Slot)));

    Block.addSlots(
        cells.slot(coding.value, type.Cell));

    Function.addSlots(
        cells.slot(coding.input, type.ListOf(Slot)),
        cells.slot(coding.ast, type.ListOf(Base)),
        cells.slot(coding.output, type.ListOf(Slot)));
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
        kb.cells.slot(kb.cells.slotType, kb.type.Type_),
        kb.cells.slot(kb.cells.slotRole, kb.type.Cell));
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
        auto& cells = kb.cells;

        listType.addSlots(cells.slot(kb.sequence.first, itemType),
                          cells.slot(kb.sequence.last, itemType),
                          cells.slot(kb.dimensions.size, kb.type.Number),
                          cells.slot(kb.coding.objectType, type));
        itemType.addSlots(cells.slot(kb.sequence.previous, itemType),
                          cells.slot(kb.sequence.next, itemType),
                          cells.slot(kb.coding.value, type));

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
        auto& cells   = kb.cells;
        mapType.addMembership(kb.type.Map);
        mapType.addSlots(cells.slot(kb.cells.list, ListOf(type)),
                         cells.slot(kb.cells.index, kb.type.Index),
                         cells.slot(kb.dimensions.size, kb.type.Number));

        return it.first->second;
    }
}

cells::CellI& Cells::slot(cells::CellI& role, cells::CellI& type)
{
    CellI& ret = *new Object(kb, kb.type.Slot);
    ret.set(kb.cells.slotRole, role);
    ret.set(kb.cells.slotType, type);
    return ret;
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
    set(kb.cells.slotRole, role);
    set(kb.cells.slotType, type);
}

Ast::Call::Call(brain::Brain& kb, CellI& cell, CellI& method) :
    BaseT<Call>(kb, kb.type.ast.Call)
{
    set(kb.coding.cell, cell);
    set(kb.cells.methods, method);
}

Ast::Call::Call(brain::Brain& kb, CellI& cell, CellI& method, Slot& slot1) :
    BaseT<Call>(kb, kb.type.ast.Call)
{
    set(kb.coding.cell, cell);
    set(kb.cells.methods, method);
    set(kb.coding.parameters, kb.list(slot1));
}

Ast::Call::Call(brain::Brain& kb, CellI& cell, CellI& method, Slot& slot1, Slot& slot2) :
    BaseT<Call>(kb, kb.type.ast.Call)
{
    set(kb.coding.cell, cell);
    set(kb.cells.methods, method);
    set(kb.coding.parameters, kb.list(slot1, slot2));
}

Ast::Call::Call(brain::Brain& kb, CellI& cell, CellI& method, Slot& slot1, Slot& slot2, Slot& slot3) :
    BaseT<Call>(kb, kb.type.ast.Call)
{
    set(kb.coding.cell, cell);
    set(kb.cells.methods, method);
    set(kb.coding.parameters, kb.list(slot1, slot2, slot3));
}

Ast::Call::Call(brain::Brain& kb, CellI& cell, CellI& method, Slot& slot1, Slot& slot2, Slot& slot3, Slot& slot4) :
    BaseT<Call>(kb, kb.type.ast.Call)
{
    set(kb.coding.cell, cell);
    set(kb.cells.methods, method);
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
    set(kb.coding.value, list);
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
            params.add(kb.coding.self, *new cells::op::Var(kb, *type));
            iss << kb.coding.self.label() << ": " << (*type).label();
        }
        if (m_inputs) {
            Visitor::visitList(inputs(), [this, &params, &iss](CellI& slot, int i) {
                if (!params.empty()) {
                    iss << ", ";
                }
                iss << slot[kb.cells.slotRole].label() << ": " << slot[kb.cells.slotType].label();
                params.add(slot[kb.cells.slotRole], *new cells::op::Var(kb, slot[kb.cells.slotType]));
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
            oss << slot[kb.cells.slotRole].label() << ": " << slot[kb.cells.slotType].label();
            params.add(slot[kb.cells.slotRole], *new cells::op::Var(kb, slot[kb.cells.slotType]));
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
        List& list         = static_cast<List&>(ast[kb.coding.value]);
        auto& compiledAsts = *new cells::List(kb, kb.type.op.Base);
        Visitor::visitList(list, [this, &compiledAsts, &ast, &function, type](CellI& ast, int) {
            compiledAsts.add(compileAst(ast, function, type));
        });
        return *new op::Block(kb, compiledAsts);
    } else if (&ast.type() == &kb.type.ast.Cell) {
        return *new op::Ref(kb, ast[kb.coding.value]);
    } else if (&ast.type() == &kb.type.ast.SelfFn) {
        return *new op::Ref(kb, function);
    } else if (&ast.type() == &kb.type.ast.Self) {
        return function[kb.coding.input][kb.cells.index][kb.coding.self];
    } else if (&ast.type() == &kb.type.ast.Parameter) {
        return function[kb.coding.input][kb.cells.index][ast[kb.coding.role]];
    } else if (&ast.type() == &kb.type.ast.Delete) {
        return *new op::Delete(kb, compile(ast[kb.coding.cell]));
    } else if (&ast.type() == &kb.type.ast.Set) {
        return *new op::Set(kb, compile(ast[kb.coding.cell]), compile(ast[kb.coding.role]), compile(ast[kb.coding.value]));
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
    } else if (&ast.type() == &kb.type.ast.Ref) {
        return *new op::Ref(kb, compile(ast[kb.coding.value]));
    } else if (&ast.type() == &kb.type.ast.Var) {
        return *new op::Ref(kb, function.getOrCreateVar(ast[kb.coding.role], kb.type.Cell));
    } else if (&ast.type() == &kb.type.ast.New) {
        return *new op::New(kb, compile(ast[kb.coding.objectType]));
    } else if (&ast.type() == &kb.type.ast.Call) {
        return ast; // TODO
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

Ast::Ref::Ref(brain::Brain& kb, CellI& cell) :
    BaseT<Ref>(kb, kb.type.ast.Ref)
{
    set(kb.coding.value, cell);
}

Ast::Var::Var(brain::Brain& kb, CellI& role) :
    BaseT<Var>(kb, kb.type.ast.Var)
{
    set(kb.coding.role, role);
}

Ast::New::New(brain::Brain& kb, Base& objectType) :
    BaseT<New>(kb, kb.type.ast.New)
{
    set(kb.coding.objectType, objectType);
}

Ast::New::New(brain::Brain& kb, Base& objectType, Slot& slot1) :
    BaseT<New>(kb, kb.type.ast.New)
{
    set(kb.coding.objectType, objectType);
    set(kb.coding.parameters, kb.list(slot1));
}

Ast::New::New(brain::Brain& kb, Base& objectType, Slot& slot1, Slot& slot2) :
    BaseT<New>(kb, kb.type.ast.New)
{
    set(kb.coding.objectType, objectType);
    set(kb.coding.parameters, kb.list(slot1, slot2));
}

Ast::New::New(brain::Brain& kb, Base& objectType, Slot& slot1, Slot& slot2, Slot& slot3) :
    BaseT<New>(kb, kb.type.ast.New)
{
    set(kb.coding.objectType, objectType);
    set(kb.coding.parameters, kb.list(slot1, slot2, slot3));
}

Ast::New::New(brain::Brain& kb, Base& objectType, Slot& slot1, Slot& slot2, Slot& slot3, Slot& slot4) :
    BaseT<New>(kb, kb.type.ast.New)
{
    set(kb.coding.objectType, objectType);
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
    return ast.set(ast.get(ast.get(ast.get(ast.selfFn(), ast.cell(kb.coding.output)), ast.cell(kb.cells.index)), ast.cell(kb.coding.value)), ast.cell(kb.coding.value), value);
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

Ast::Has& Ast::hasMember(Base& role)
{
    return kb.ast.has(kb.ast.self(), role);
}

Ast::Get& Ast::getMember(Base& role)
{
    return kb.ast.get(kb.ast.self(), role);
}

Ast::Set& Ast::setMember(Base& role, Base& value)
{
    return kb.ast.set(kb.ast.self(), role, value);
}

Ast::Set& Ast::setVar(CellI& role, Base& ast)
{
    return kb.ast.set(kb.ast.var(role), kb.ast.cell(kb.coding.value), ast);
}

Ast::Get& Ast::getVar(CellI& role)
{
    return kb.ast.get(kb.ast.var(role), kb.ast.cell(kb.coding.value));
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

Ast::Ref& Ast::ref(CellI& cell)
{
    return Ref::New(kb, cell);
}

Ast::Var& Ast::var(CellI& role)
{
    return Var::New(kb, role);
}

Ast::New& Ast::new_(Base& ast)
{
    return New::NewT<Ast::New>::New(kb, ast);
}

Ast::New& Ast::new_(Base& ast, Slot& slot1)
{
    return New::NewT<Ast::New>::New(kb, ast, slot1);
}

Ast::New& Ast::new_(Base& ast, Slot& slot1, Slot& slot2)
{
    return New::NewT<Ast::New>::New(kb, ast, slot1, slot2);
}

Ast::New& Ast::new_(Base& ast, Slot& slot1, Slot& slot2, Slot& slot3)
{
    return New::NewT<Ast::New>::New(kb, ast, slot1, slot2, slot3);
}

Ast::New& Ast::new_(Base& ast, Slot& slot1, Slot& slot2, Slot& slot3, Slot& slot4)
{
    return New::NewT<Ast::New>::New(kb, ast, slot1, slot2, slot3, slot4);
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
        return kb.cells.emptyObject;
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
    auto& cells = kb.cells;

    Demonstration.addSlots(
        cells.slot(kb.coding.input, kb.type.Picture),
        cells.slot(kb.coding.output, kb.type.Picture));

    Task.addSlots(
        cells.slot(examples, kb.type.ListOf(Demonstration)),
        cells.slot(kb.coding.input, kb.type.Picture),
        cells.slot(kb.coding.output, kb.type.Picture));
}

Brain::Brain() :
    m_initPhase(InitPhase::Init),
    cells(*this),
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
    type.Type_.addSlots(
        cells.slot(cells.slots, type.MapOf(type.Slot)),
        cells.slot(cells.subTypes, type.MapOf(type.Type_)),
        cells.slot(cells.memberOf, type.MapOf(type.Type_)),
        cells.slot(cells.methods, type.MapOf(type.ast.Function)));

    type.ListItem.addSlots(
        cells.slot(sequence.previous, type.ListItem),
        cells.slot(sequence.next, type.ListItem),
        cells.slot(coding.value, type.Cell));
    type.ListItem.addMembership(
        type.Iterator);

    type.List.addSlots(
        cells.slot(sequence.first, type.ListItem),
        cells.slot(sequence.last, type.ListItem),
        cells.slot(dimensions.size, type.Number),
        cells.slot(coding.objectType, type.Cell));
    type.List.addSubType(
        coding.objectType, type.ListItem);
    type.List.addMembership(
        type.Container);

    type.Map.addSlots(
        cells.slot(coding.keyType, type.Cell),
        cells.slot(coding.objectType, type.Cell),
        cells.slot(cells.list, type.ListOf(type.Cell)),
        cells.slot(cells.index, type.Index),
        cells.slot(dimensions.size, type.Number));

    Ast::Function& mapCtor = *new Ast::Function(*this, "Map::Ctor");
    mapCtor.addInputs(list(
        ast.slot(coding.keyType, type.Cell),
        ast.slot(coding.objectType, type.Cell)));
    mapCtor.addAsts(ast.block(
        ast.setMember(ast.cell(dimensions.size), ast.cell(_0_)),
        ast.setMember(ast.cell(coding.keyType), ast.parameter(coding.keyType)),
        ast.setMember(ast.cell(coding.objectType), ast.parameter(coding.objectType))));
    type.Map.addMethod(cells.constructor, mapCtor);

    Ast::Function& mapAdd  = *new Ast::Function(*this, "Map::Add");
    mapAdd.addInputs(list(
        ast.slot(coding.key, type.Cell),
        ast.slot(coding.value, type.Cell)));
    mapAdd.addAsts(ast.block(
        ast.if_(ast.equal(ast.getMember(ast.cell(dimensions.size)), ast.cell(_0_)),
                ast.block(ast.setMember(ast.cell(cells.list), ast.new_(ast.cell(type.List))),
                          ast.setMember(ast.cell(cells.index), ast.new_(ast.cell(type.Index))),
                          ast.call(ast.getMember(ast.cell(cells.list)), ast.cell(cells.constructor), ast.slot(ast.cell(coding.objectType), ast.getMember(ast.cell(coding.objectType)))))),
        ast.call(ast.getMember(ast.cell(cells.list)), ast.cell(sequence.add), ast.slot(ast.cell(coding.value), ast.parameter(coding.value))),
        ast.set(ast.getMember(ast.cell(cells.index)), ast.parameter(coding.value), ast.get(ast.getMember(ast.cell(cells.list)), ast.cell(sequence.last))),
        ast.setMember(ast.cell(dimensions.size), ast.add(ast.getMember(ast.cell(dimensions.size)), ast.cell(_1_)))));
    type.Map.addMethod(sequence.add, mapAdd);

    Ast::Function& mapSize = *new Ast::Function(*this, "Map::Size");
    mapSize.addOutputs(list(
        ast.slot(coding.value, type.Number)));
    mapSize.addAsts(ast.block(
        ast.return_(ast.getMember(ast.cell(dimensions.size)))));
    type.Map.addMethod(dimensions.size, mapSize);

    Ast::Function& mapEmpty = *new Ast::Function(*this, "Map::Empty");
    mapEmpty.addOutputs(list(
        ast.slot(coding.value, type.Boolean)));
    mapEmpty.addAsts(ast.block(
        ast.if_(ast.equal(ast.getMember(ast.cell(dimensions.size)), ast.cell(_0_)), ast.return_(ast.cell(boolean.true_)), ast.return_(ast.cell(boolean.false_)))));
    type.Map.addMethod(sequence.empty, mapEmpty);

    Ast::Function& typeCtor = *new Ast::Function(*this, "Type::Ctor");
    typeCtor.addInputs(list(
        ast.slot(cells.slots, type.ListOf(type.Slot)),
        ast.slot(cells.subTypes, type.MapOf(type.Type_)),
        ast.slot(cells.memberOf, type.MapOf(type.Type_)),
        ast.slot(cells.methods, type.MapOf(type.ast.Function))));
    typeCtor.addAsts(ast.block(
        ast.setMember(ast.cell(cells.slots), ast.parameter(cells.slots)),
        ast.setMember(ast.cell(cells.subTypes), ast.parameter(cells.subTypes)),
        ast.setMember(ast.cell(cells.memberOf), ast.parameter(cells.memberOf)),
        ast.setMember(ast.cell(cells.methods), ast.parameter(cells.methods))));
    type.Type_.addMethod(cells.constructor, typeCtor);


    Ast::Function& listCtor = *new Ast::Function(*this, "List::Ctor");
    listCtor.addAsts(ast.block(
        ast.setMember(ast.cell(dimensions.size), ast.cell(_0_)),
        ast.setMember(ast.cell(coding.objectType), ast.cell(type.Cell))));
    type.List.addMethod(cells.constructor, listCtor);

    Ast::Function& listAdd = *new Ast::Function(*this, "List::Add");
    listAdd.addInputs(list(
        ast.slot(coding.value, type.Cell)));
    listAdd.addAsts(ast.block(
        ast.setVar(_1_, ast.new_(ast.get(ast.get(ast.get(ast.get(ast.self(), ast.cell(cells.type)), ast.cell(cells.subTypes)), ast.cell(cells.index)), ast.cell(coding.objectType)))),
        ast.set(ast.getVar(_1_), ast.cell(coding.value), ast.parameter(coding.value)),
        ast.if_(ast.not_(ast.hasMember(ast.cell(sequence.first))),
                ast.setMember(ast.cell(sequence.first), ast.getVar(_1_)),                                                  // then
                ast.block(ast.set(ast.get(ast.self(), ast.cell(sequence.last)), ast.cell(sequence.next), ast.getVar(_1_)), // else
                          ast.set(ast.getVar(_1_), ast.cell(sequence.previous), ast.getMember(ast.cell(sequence.last))))),
        ast.setMember(ast.cell(sequence.last), ast.getVar(pools.numbers.get(1))),
        ast.setMember(ast.cell(dimensions.size), ast.add(ast.getMember(ast.cell(dimensions.size)), ast.cell(_1_)))));
    type.List.addMethod(sequence.add, listAdd);

    Ast::Function& listSize = *new Ast::Function(*this, "List::Size");
    listSize.addOutputs(list(
        ast.slot(coding.value, type.Number)));
    listSize.addAsts(ast.block(
        ast.return_(ast.getMember(ast.cell(dimensions.size)))));
    type.List.addMethod(dimensions.size, listSize);

    Ast::Function& listEmpty = *new Ast::Function(*this, "List::Empty");
    listEmpty.addOutputs(list(
        ast.slot(coding.value, type.Boolean)));
    listEmpty.addAsts(ast.block(
        ast.if_(ast.equal(ast.getMember(ast.cell(dimensions.size)), ast.cell(_0_)), ast.return_(ast.cell(boolean.true_)), ast.return_(ast.cell(boolean.false_)))));
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
        cells.slot(coding.value, type.ListOf(type.Digit)),
        cells.slot(numbers.sign, type.Number)); // TODO

    type.String.addSlots(
        cells.slot(coding.value, type.ListOf(type.Char)));

    type.Color.addSlots(
        cells.slot(colors.red, type.Number),
        cells.slot(colors.green, type.Number),
        cells.slot(colors.blue, type.Number));

    type.Pixel.addSlots(
        cells.slot(directions.up, type.Pixel),
        cells.slot(directions.down, type.Pixel),
        cells.slot(directions.left, type.Pixel),
        cells.slot(directions.right, type.Pixel),
        cells.slot(coordinates.x, type.Number),
        cells.slot(coordinates.y, type.Number));

    type.Picture.addSlots(
        cells.slot(dimensions.width, type.Number),
        cells.slot(dimensions.height, type.Number),
        cells.slot(visualization.pixels, type.ListOf(type.Pixel)));

    m_initPhase = InitPhase::FullyConstructed;
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
