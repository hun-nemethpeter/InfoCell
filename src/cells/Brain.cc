#include "Brain.h"

#include <format>
#include <sstream>

namespace synth {
namespace cells {
namespace brain {
namespace type {
Control::Control(brain::Brain& kb) :
    Base(kb, "control::Base"),
    Block(kb, "control::Block"),
    Function(kb, "control::Function"),
    Delete(kb, "control::Delete"),
    Set(kb, "control::Set"),
    If(kb, "control::If"),
    Do(kb, "control::Do"),
    While(kb, "control::While"),
    Expression(kb, "control::Expression"),
    Ref(kb, "control::Ref"),
    Var(kb, "control::Var"),
    New(kb, "control::New"),
    Same(kb, "control::Same"),
    NotSame(kb, "control::NotSame"),
    Equal(kb, "control::Equal"),
    NotEqual(kb, "control::NotEqual"),
    Has(kb, "control::Has"),
    Get(kb, "control::Get"),
    And(kb, "control::And"),
    Or(kb, "control::Or"),
    Not(kb, "control::Not"),
    Add(kb, "control::Add"),
    Subtract(kb, "control::Subtract"),
    Multiply(kb, "control::Multiply"),
    Divide(kb, "control::Divide"),
    LessThan(kb, "control::LessThan"),
    GreaterThan(kb, "control::GreaterThan")
{
}

Ast::Ast(brain::Brain& kb) :
    kb(kb),
    Base(kb, "ast::Base"),
    Parameter(kb, "ast::Parameter"),
    ParameterDecl(kb, "ast::ParameterDecl"),
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
}

} // namespace type

Types::Types(brain::Brain& kb) :
    kb(kb),
    Type_(kb, "Type"),
    Slot(kb, "Slot"),
    Container(kb, "Conatainer"),
    Iterator(kb, "Iterator"),
    List(kb, "List"),
    ListItem(kb, "ListItem"),
    Map(kb, "Map"),
    Index(kb, "Index"),
    Void(kb, "Void"),
    Any(kb, "Any"),
    Boolean(kb, "Boolean"),
    Char(kb, "Char"),
    Digit(kb, "Digit"),
    Number(kb, "Number"),
    String(kb, "String"),
    Color(kb, "Color"),
    Pixel(kb, "Pixel"),
    Picture(kb, "Picture"),
    Template(kb, "Template"),
    control(kb),
    ast(kb)
{
    Slot.addSlots(
        kb.cells.slot(kb.cells.slotType, kb.type.Type_),
        kb.cells.slot(kb.cells.slotRole, kb.type.Any));
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

Cells::Cells(brain::Brain& kb, Type& voidType, Type& anyType) :
    kb(kb),
    type(kb, anyType, "type"),
    constructor(kb, anyType, "constructor"),
    destructor(kb, anyType, "destructor"),
    slots(kb, anyType, "slots"),
    slotType(kb, anyType, "slotType"),
    slotRole(kb, anyType, "slotRole"),
    subTypes(kb, anyType, "subTypes"),
    index(kb, anyType, "index"),
    list(kb, anyType, "list"),
    memberOf(kb, anyType, "memberOf"),
    methods(kb, anyType, "methods"),
    emptyObject(kb, voidType, "emptyObject")
{
}

cells::CellI& Cells::slot(cells::CellI& role, cells::CellI& type)
{
    CellI& ret = *new Object(kb, kb.type.Slot);
    ret.set(kb.cells.slotRole, role);
    ret.set(kb.cells.slotType, type);
    return ret;
}

Coding::Coding(brain::Brain& kb, Type& anyType) :
    argument(kb, anyType, "argument"),
    ast(kb, anyType, "ast"),
    branch(kb, anyType, "branch"),
    cell(kb, anyType, "cell"),
    condition(kb, anyType, "condition"),
    container(kb, anyType, "container"),
    else_(kb, anyType, "else_"),
    input(kb, anyType, "input"),
    item(kb, anyType, "item"),
    label(kb, anyType, "label"),
    lhs(kb, anyType, "lhs"),
    objectType(kb, anyType, "objectType"),
    op(kb, anyType, "op"),
    output(kb, anyType, "output"),
    parameter(kb, anyType, "parameter"),
    parameters(kb, anyType, "parameters"),
    result(kb, anyType, "result"),
    rhs(kb, anyType, "rhs"),
    role(kb, anyType, "role"),
    self(kb, anyType, "self"),
    statement(kb, anyType, "statement"),
    template_(kb, anyType, "template"),
    then(kb, anyType, "then"),
    value(kb, anyType, "value")
{
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

Ast::ParameterDecl::ParameterDecl(brain::Brain& kb, CellI& role, CellI& type) :
    BaseT<ParameterDecl>(kb, kb.type.ast.ParameterDecl)
{
    set(kb.cells.slotRole, role);
    set(kb.cells.slotType, type);
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
    cells::control::Function& function = *new cells::control::Function(kb);
    compileParams(function, type);

    std::stringstream ss;
    Visitor::visitList(function[kb.coding.input][kb.cells.list], [this, &ss, &function](CellI& slot, int i) {
        if (i != 0) {
            ss << ", ";
        }
        ss << slot[kb.cells.slotRole].label() << ": " << slot[kb.cells.slotType].label();
    });
    function.label(std::format("{}({})", label(), ss.str()));
    function.set(kb.coding.op, compileAst(asts(), function, type));

    return function;
}

void Ast::Function::compileParams(cells::control::Function& function, CellI* type)
{
    if (m_inputs || type) {
        Map& params = *new Map(kb, kb.type.control.Var);
        if (type) {
            params.add(kb.coding.self, *new cells::control::Var(kb, *type));
        }
        if (m_inputs) {
            Visitor::visitList(inputs(), [this, &params](CellI& slot, int i) {
                params.add(slot[kb.cells.slotRole], *new cells::control::Var(kb, slot[kb.cells.slotType]));
            });
        }
        function.addInputs(params);
    }
    if (m_outputs) {
        Map& params = *new Map(kb, kb.type.control.Var);
        Visitor::visitList(outputs(), [this, &params](CellI& slot, int i) {
            params.add(slot[kb.cells.slotRole], *new cells::control::Var(kb, slot[kb.cells.slotType]));
        });
        function.addOutputs(params);
    }
}

CellI& Ast::Function::compileAst(CellI& ast, cells::control::Function& function, CellI* type)
{
    auto compile = [this, &function, type](CellI& ast) -> CellI& { return compileAst(ast, function, type); };

    if (&ast.type() == &kb.type.ast.Block) {
        List& list         = static_cast<List&>(ast[kb.coding.value]);
        auto& compiledAsts = *new cells::List(kb, kb.type.control.Base);
        Visitor::visitList(list, [this, &compiledAsts, &ast, &function, type](CellI& ast, int) {
            compiledAsts.add(compileAst(ast, function, type));
        });
        return *new control::Block(kb, compiledAsts);
    } else if (&ast.type() == &kb.type.ast.Cell) {
        return *new control::Ref(kb, ast[kb.coding.value]);
    } else if (&ast.type() == &kb.type.ast.SelfFn) {
        return *new control::Ref(kb, function);
    } else if (&ast.type() == &kb.type.ast.Self) {
        return function[kb.coding.input][kb.cells.index][kb.coding.self];
    } else if (&ast.type() == &kb.type.ast.Parameter) {
        return function[kb.coding.input][kb.cells.index][ast[kb.coding.role]];
    } else if (&ast.type() == &kb.type.ast.Delete) {
        return *new control::Delete(kb, compile(ast[kb.coding.cell]));
    } else if (&ast.type() == &kb.type.ast.Set) {
        return *new control::Set(kb, compile(ast[kb.coding.cell]), compile(ast[kb.coding.role]), compile(ast[kb.coding.value]));
    } else if (&ast.type() == &kb.type.ast.If) {
        if (ast.has(kb.coding.else_)) {
            return *new control::If(kb, compile(ast[kb.coding.condition]), compile(ast[kb.coding.then]), compile(ast[kb.coding.else_]));
        } else {
            return *new control::If(kb, compile(ast[kb.coding.condition]), compile(ast[kb.coding.then]));
        }
    } else if (&ast.type() == &kb.type.ast.Do) {
        return *new control::Do(kb, compile(ast[kb.coding.condition]), compile(ast[kb.coding.statement]));
    } else if (&ast.type() == &kb.type.ast.While) {
        return *new control::While(kb, compile(ast[kb.coding.condition]), compile(ast[kb.coding.statement]));
    } else if (&ast.type() == &kb.type.ast.Ref) {
        return *new control::Ref(kb, compile(ast[kb.coding.value]));
    } else if (&ast.type() == &kb.type.ast.Var) {
        return *new control::Ref(kb, function.getOrCreateVar(ast[kb.coding.role], kb.type.Any));
    } else if (&ast.type() == &kb.type.ast.New) {
        return *new control::New(kb, compile(ast[kb.coding.objectType]));
    } else if (&ast.type() == &kb.type.ast.And) {
        return *new control::And(kb, compile(ast[kb.coding.lhs]), compile(ast[kb.coding.rhs]));
    } else if (&ast.type() == &kb.type.ast.Or) {
        return *new control::Or(kb, compile(ast[kb.coding.lhs]), compile(ast[kb.coding.rhs]));
    } else if (&ast.type() == &kb.type.ast.Not) {
        return *new control::Not(kb, compile(ast[kb.coding.value]));
    } else if (&ast.type() == &kb.type.ast.Add) {
        return *new control::Add(kb, compile(ast[kb.coding.lhs]), compile(ast[kb.coding.rhs]));
    } else if (&ast.type() == &kb.type.ast.Subtract) {
        return *new control::Subtract(kb, compile(ast[kb.coding.lhs]), compile(ast[kb.coding.rhs]));
    } else if (&ast.type() == &kb.type.ast.Multiply) {
        return *new control::Multiply(kb, compile(ast[kb.coding.lhs]), compile(ast[kb.coding.rhs]));
    } else if (&ast.type() == &kb.type.ast.Divide) {
        return *new control::Divide(kb, compile(ast[kb.coding.lhs]), compile(ast[kb.coding.rhs]));
    } else if (&ast.type() == &kb.type.ast.LessThan) {
        return *new control::LessThan(kb, compile(ast[kb.coding.lhs]), compile(ast[kb.coding.rhs]));
    } else if (&ast.type() == &kb.type.ast.GreaterThan) {
        return *new control::GreaterThan(kb, compile(ast[kb.coding.lhs]), compile(ast[kb.coding.rhs]));
    } else if (&ast.type() == &kb.type.ast.Same) {
        return *new control::Same(kb, compile(ast[kb.coding.lhs]), compile(ast[kb.coding.rhs]));
    } else if (&ast.type() == &kb.type.ast.NotSame) {
        return *new control::NotSame(kb, compile(ast[kb.coding.lhs]), compile(ast[kb.coding.rhs]));
    } else if (&ast.type() == &kb.type.ast.Equal) {
        return *new control::Equal(kb, compile(ast[kb.coding.lhs]), compile(ast[kb.coding.rhs]));
    } else if (&ast.type() == &kb.type.ast.NotEqual) {
        return *new control::NotEqual(kb, compile(ast[kb.coding.lhs]), compile(ast[kb.coding.rhs]));
    } else if (&ast.type() == &kb.type.ast.Has) {
        return *new control::Has(kb, compile(ast[kb.coding.cell]), compile(ast[kb.coding.role]));
    } else if (&ast.type() == &kb.type.ast.Get) {
        return *new control::Get(kb, compile(ast[kb.coding.cell]), compile(ast[kb.coding.role]));
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

Ast::ParameterDecl& Ast::parameterDecl(CellI& role, CellI& type)
{
    return ParameterDecl::New(kb, role, type);
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

Sequence::Sequence(brain::Brain& kb) :
    first(kb, kb.type.Any, "first"),
    last(kb, kb.type.Any, "last"),
    previous(kb, kb.type.Any, "previous"),
    next(kb, kb.type.Any, "next"),
    current(kb, kb.type.Any, "current"),
    add(kb, kb.type.Any, "add")
{
}

Equation::Equation(brain::Brain& kb, Type& anyType) :
    lhs(kb, anyType, "lhs"),
    rhs(kb, anyType, "rhs")
{
}

Directions::Directions(brain::Brain& kb, Type& anyType) :
    up(kb, anyType, "up"),
    down(kb, anyType, "down"),
    left(kb, anyType, "left"),
    right(kb, anyType, "right")
{
}

Coordinates::Coordinates(brain::Brain& kb, Type& anyType) :
    x(kb, anyType, "x"),
    y(kb, anyType, "y")
{
}

Colors::Colors(brain::Brain& kb, Type& anyType) :
    red(kb, anyType, "red"),
    green(kb, anyType, "green"),
    blue(kb, anyType, "blue")
{
}

Boolean::Boolean(brain::Brain& kb) :
    true_(kb, kb.type.Boolean, "true"),
    false_(kb, kb.type.Boolean, "false")
{
}

Dimensions::Dimensions(brain::Brain& kb, Type& anyType) :
    width(kb, anyType, "width"),
    height(kb, anyType, "height"),
    size(kb, anyType, "size")
{
}

Visualization::Visualization(brain::Brain& kb, Type& anyType) :
    color(kb, anyType, "color"),
    pixels(kb, anyType, "pixels")
{
}

Numbers::Numbers(brain::Brain& kb, Type& anyType) :
    sign(kb, anyType, "sign"),
    positive(kb, anyType, "positive"),
    negative(kb, anyType, "negative")

{
    sign.add(positive, positive);
    sign.add(negative, negative);
}

namespace pools {

// ============================================================================
Chars::Chars(brain::Brain& kb, Type& charType, Object& emptyObject) :
    m_charType(charType), m_emptyObject(emptyObject), m_kb(kb)
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
        return m_emptyObject;
    }
}

Type& Chars::type()
{
    return m_charType;
}

void Chars::registerUnicodeBlock(char32_t from, char32_t to)
{
    for (char32_t unicodeValue = from; unicodeValue <= to; ++unicodeValue) {
        std::string characterName = std::format("Unicode_{:#04x}", (int)unicodeValue);
        s_characters.emplace(std::piecewise_construct,
                             std::forward_as_tuple(unicodeValue),
                             std::forward_as_tuple(m_kb, m_charType, characterName));
    }
}

// ============================================================================
Digits::Digits(brain::Brain& kb, Type& digit)
{
    m_digits.reserve(10);
    for (int i = 0; i < 10; ++i) {
        std::string digitName = "Digit_" + std::to_string(i);
        m_digits.emplace_back(kb, digit, digitName);
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

Pools::Pools(brain::Brain& kb, Type& charType, Object& emptyObject, Type& digit) :
    chars(kb, charType, emptyObject),
    digits(kb, digit),
    numbers(kb)
{
}

Arc::Arc(brain::Brain& kb) :
    Demonstration(kb, "Demonstration"),
    Task(kb, "Task"),
    examples(kb, kb.type.Any)
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
    cells(*this, type.Void, type.Any),
    type(*this),
    coding(*this, type.Any),
    pools(*this, type.Char, cells.emptyObject, type.Digit),
    ast(*this),
    sequence(*this),
    equation(*this, type.Any),
    directions(*this, type.Any),
    coordinates(*this, type.Any),
    colors(*this, type.Any),
    boolean(*this),
    dimensions(*this, type.Any),
    visualization(*this, type.Any),
    numbers(*this, type.Any),
    arc(*this)
{
    type.Type_.addSlots(
        cells.slot(cells.slots, type.MapOf(type.Slot)),
        cells.slot(cells.subTypes, type.MapOf(type.Type_)),
        cells.slot(cells.memberOf, type.MapOf(type.Type_)),
        cells.slot(cells.methods, type.MapOf(type.control.Function)));

    type.ListItem.addSlots(
        cells.slot(sequence.previous, type.ListItem),
        cells.slot(sequence.next, type.ListItem),
        cells.slot(coding.value, type.Any));
    type.ListItem.addMembership(
        type.Iterator);

    type.List.addSlots(
        cells.slot(sequence.first, type.ListItem),
        cells.slot(sequence.last, type.ListItem),
        cells.slot(dimensions.size, type.Number),
        cells.slot(coding.objectType, type.Any));
    type.List.addSubType(
        coding.objectType, type.ListItem);
    type.List.addMembership(
        type.Container);

    type.ast.Ref.addSlots(
        cells.slot(coding.value, type.ast.Base));

    type.ast.Var.addSlots(
        cells.slot(coding.role, type.ast.Base));

    type.ast.New.addSlots(
        cells.slot(coding.objectType, type.ast.Base));

    type.ast.Delete.addSlots(
        cells.slot(coding.cell, type.ast.Base));

    type.ast.If.addSlots(
        cells.slot(coding.condition, type.ast.Base),
        cells.slot(coding.then, type.ast.Base),
        cells.slot(coding.else_, type.ast.Base));

    type.ast.Do.addSlots(
        cells.slot(coding.condition, type.ast.Base),
        cells.slot(coding.statement, type.ast.Base));

    type.ast.While.addSlots(
        cells.slot(coding.condition, type.ast.Base),
        cells.slot(coding.statement, type.ast.Base));

    type.ast.And.addSlots(
        cells.slot(coding.lhs, type.ast.Base),
        cells.slot(coding.rhs, type.ast.Base));

    type.ast.Or.addSlots(
        cells.slot(coding.lhs, type.ast.Base),
        cells.slot(coding.rhs, type.ast.Base));

    type.ast.Not.addSlots(
        cells.slot(coding.value, type.ast.Base));

    type.ast.Add.addSlots(
        cells.slot(coding.lhs, type.ast.Base),
        cells.slot(coding.rhs, type.ast.Base));

    type.ast.Subtract.addSlots(
        cells.slot(coding.lhs, type.ast.Base),
        cells.slot(coding.rhs, type.ast.Base));

    type.ast.Multiply.addSlots(
        cells.slot(coding.lhs, type.ast.Base),
        cells.slot(coding.rhs, type.ast.Base));

    type.ast.Divide.addSlots(
        cells.slot(coding.lhs, type.ast.Base),
        cells.slot(coding.rhs, type.ast.Base));

    type.ast.LessThan.addSlots(
        cells.slot(coding.lhs, type.ast.Base),
        cells.slot(coding.rhs, type.ast.Base));

    type.ast.GreaterThan.addSlots(
        cells.slot(coding.lhs, type.ast.Base),
        cells.slot(coding.rhs, type.ast.Base));

    type.ast.Same.addSlots(
        cells.slot(coding.lhs, type.ast.Base),
        cells.slot(coding.rhs, type.ast.Base));

    type.ast.NotSame.addSlots(
        cells.slot(coding.lhs, type.ast.Base),
        cells.slot(coding.rhs, type.ast.Base));

    type.ast.Equal.addSlots(
        cells.slot(coding.lhs, type.ast.Base),
        cells.slot(coding.rhs, type.ast.Base));

    type.ast.NotEqual.addSlots(
        cells.slot(coding.lhs, type.ast.Base),
        cells.slot(coding.rhs, type.ast.Base));

    type.ast.Has.addSlots(
        cells.slot(coding.cell, type.ast.Base),
        cells.slot(coding.role, type.ast.Base));

    type.ast.Get.addSlots(
        cells.slot(coding.cell, type.ast.Base),
        cells.slot(coding.role, type.ast.Base));

    type.ast.Set.addSlots(
        cells.slot(coding.cell, type.ast.Base),
        cells.slot(coding.role, type.ast.Base),
        cells.slot(coding.value, type.ast.Base));

    type.ast.Parameter.addSlots(
        cells.slot(coding.role, type.Any));

    type.ast.ParameterDecl.addSlots(
        cells.slot(cells.slotRole, type.ast.Base),
        cells.slot(cells.slotType, type.ast.Base));

    type.ast.Cell.addSlots(
        cells.slot(coding.value, type.Any));

    type.ast.Block.addSlots(
        cells.slot(coding.value, type.Any));

    type.ast.Function.addSlots(
        cells.slot(coding.input, type.ListOf(type.ast.ParameterDecl)),
        cells.slot(coding.ast, type.ListOf(type.ast.Base)),
        cells.slot(coding.output, type.ListOf(type.ast.ParameterDecl)));

    Ast::Function listCtor(*this, "List::Ctor");
    Ast::Function listAdd(*this, "List::Add");
    Ast::Function listSize(*this, "List::Size");

    listCtor.addAsts(ast.block(
        ast.setMember(ast.cell(dimensions.size), ast.cell(pools.numbers.get(0))),
        ast.setMember(ast.cell(coding.objectType), ast.cell(type.Any))));

    listAdd.addInputs(list(
        ast.parameterDecl(coding.value, type.Any)));

    listAdd.addAsts(ast.block(
        ast.setVar(pools.numbers.get(1), ast.new_(ast.get(ast.get(ast.get(ast.get(ast.self(), ast.cell(cells.type)), ast.cell(cells.subTypes)), ast.cell(cells.index)), ast.cell(coding.objectType)))),
        ast.set(ast.getVar(pools.numbers.get(1)), ast.cell(coding.value), ast.parameter(coding.value)),
        ast.if_(ast.not_(ast.hasMember(ast.cell(sequence.first))),
                ast.setMember(ast.cell(sequence.first), ast.getVar(pools.numbers.get(1))),                                                  // then
                ast.block(ast.set(ast.get(ast.self(), ast.cell(sequence.last)), ast.cell(sequence.next), ast.getVar(pools.numbers.get(1))), // else
                          ast.set(ast.getVar(pools.numbers.get(1)), ast.cell(sequence.previous), ast.getMember(ast.cell(sequence.last))))),
        ast.setMember(ast.cell(sequence.last), ast.getVar(pools.numbers.get(1))),
        ast.setMember(ast.cell(dimensions.size), ast.add(ast.getMember(ast.cell(dimensions.size)), ast.cell(pools.numbers.get(1))))));

    listSize.addOutputs(list(
        ast.parameterDecl(coding.value, type.Any)));
    listSize.addAsts(ast.block(
        ast.return_(ast.getMember(ast.cell(dimensions.size)))));
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
    Object methodData(*this, type.Any);
    methodData.set(coding.ast, listAdd);
    methodData.set(coding.op, listAdd.compile());
    methodData.set(coding.static_, boolean.false_);
    methodData.set(coding.const_, boolean.false_);
#endif
    type.List.addMethod(cells.constructor, listCtor.compile(type.List));
    type.List.addMethod(sequence.add, listAdd.compile(type.List));
    type.List.addMethod(dimensions.size, listSize.compile(type.List));

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

    type.control.Same.addSlots(
        cells.slot(equation.lhs, type.control.Base),
        cells.slot(equation.rhs, type.control.Base),
        cells.slot(coding.value, type.Boolean));

    type.control.NotSame.addSlots(
        cells.slot(equation.lhs, type.control.Base),
        cells.slot(equation.rhs, type.control.Base),
        cells.slot(coding.value, type.Boolean));

    type.control.Equal.addSlots(
        cells.slot(equation.lhs, type.control.Base),
        cells.slot(equation.rhs, type.control.Base),
        cells.slot(coding.value, type.Boolean));

    type.control.NotEqual.addSlots(
        cells.slot(equation.lhs, type.control.Base),
        cells.slot(equation.rhs, type.control.Base),
        cells.slot(coding.value, type.Boolean));

    type.control.Has.addSlots(
        cells.slot(coding.cell, type.control.Base),
        cells.slot(coding.role, type.control.Base),
        cells.slot(coding.value, type.Boolean));

    type.control.Get.addSlots(
        cells.slot(coding.cell, type.control.Base),
        cells.slot(coding.role, type.control.Base),
        cells.slot(coding.value, type.Any));

    type.control.Set.addSlots(
        cells.slot(coding.cell, type.control.Base),
        cells.slot(coding.role, type.control.Base),
        cells.slot(coding.value, type.control.Base));

    type.control.And.addSlots(
        cells.slot(equation.lhs, type.control.Base),
        cells.slot(equation.rhs, type.control.Base),
        cells.slot(coding.value, type.Boolean));

    type.control.Or.addSlots(
        cells.slot(equation.lhs, type.control.Base),
        cells.slot(equation.rhs, type.control.Base),
        cells.slot(coding.value, type.Boolean));

    type.control.Not.addSlots(
        cells.slot(coding.input, type.control.Base),
        cells.slot(coding.value, type.Boolean));

    type.control.Add.addSlots(
        cells.slot(equation.lhs, type.control.Base),
        cells.slot(equation.rhs, type.control.Base),
        cells.slot(coding.value, type.Number));

    type.control.Subtract.addSlots(
        cells.slot(equation.lhs, type.control.Base),
        cells.slot(equation.rhs, type.control.Base),
        cells.slot(coding.value, type.Number));

    type.control.Multiply.addSlots(
        cells.slot(equation.lhs, type.control.Base),
        cells.slot(equation.rhs, type.control.Base),
        cells.slot(coding.value, type.Number));

    type.control.Divide.addSlots(
        cells.slot(equation.lhs, type.control.Base),
        cells.slot(equation.rhs, type.control.Base),
        cells.slot(coding.value, type.Number));

    type.control.LessThan.addSlots(
        cells.slot(equation.lhs, type.control.Base),
        cells.slot(equation.rhs, type.control.Base),
        cells.slot(coding.value, type.Boolean));

    type.control.GreaterThan.addSlots(
        cells.slot(equation.lhs, type.control.Base),
        cells.slot(equation.rhs, type.control.Base),
        cells.slot(coding.value, type.Boolean));

    type.control.Ref.addSlots(
        cells.slot(coding.value, type.Any));

    type.control.Var.addSlots(
        cells.slot(coding.value, type.Any));

    type.control.New.addSlots(
        cells.slot(coding.value, type.Any),
        cells.slot(coding.objectType, type.control.Base));

    type.control.Delete.addSlots(
        cells.slot(coding.input, type.control.Base));

    type.control.If.addSlots(
        cells.slot(coding.condition, type.control.Base),
        cells.slot(coding.then, type.control.Base),
        cells.slot(coding.else_, type.control.Base));

    type.control.Do.addSlots(
        cells.slot(coding.condition, type.control.Base),
        cells.slot(coding.statement, type.control.Base));

    type.control.While.addSlots(
        cells.slot(coding.condition, type.control.Base),
        cells.slot(coding.statement, type.control.Base));

    type.control.Function.addSlots(
        cells.slot(coding.input, type.MapOf(type.control.Base)),
        cells.slot(coding.op, type.ListOf(type.control.Base)),
        cells.slot(coding.output, type.MapOf(type.control.Base)));

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
