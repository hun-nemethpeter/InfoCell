#include "Brain.h"

#include <format>
#include <sstream>

namespace synth {
namespace cells {
namespace brain {
namespace type {
Template::Template(brain::Brain& kb) :
    ParameterDecl(kb, "ParameterDecl"),
    Slot(kb, "Slot"),
    Descriptor(kb, "Descriptor"),
    Cell(kb, "Cell"),
    Parameter(kb, "Parameter"),
    TemplateOf(kb, "Template"),
    Self(kb, "Self")
{
}

Control::Control(brain::Brain& kb) :
    Base(kb, "Base"),
    Block(kb, "Block"),
    Function(kb, "Function"),
    Delete(kb, "Delete"),
    Set(kb, "Set"),
    If(kb, "If"),
    Do(kb, "Do"),
    While(kb, "While"),
    Expression(kb, "Expression"),
    Input(kb, "Input"),
    New(kb, "New"),
    Same(kb, "Same"),
    NotSame(kb, "NotSame"),
    Equal(kb, "Equal"),
    NotEqual(kb, "NotEqual"),
    Has(kb, "Has"),
    Get(kb, "Get"),
    And(kb, "And"),
    Or(kb, "Or"),
    Not(kb, "Not"),
    Add(kb, "Add"),
    Subtract(kb, "Subtract"),
    Multiply(kb, "Multiply"),
    Divide(kb, "Divide"),
    LessThan(kb, "LessThan"),
    GreaterThan(kb, "GreaterThan")
{
}

Ast::Ast(brain::Brain& kb) :
    kb(kb),
    Base(kb, "Base"),
    Parameter(kb, "Parameter"),
    ParameterDecl(kb, "ParameterDecl"),
    Cell(kb, "Cell"),
    HasMember(kb, "HasMember"),
    GetMember(kb, "GetMember"),
    SetMember(kb, "SetMember"),
    SetVar(kb, "SetVar"),
    GetVar(kb, "GetVar"),
    Self(kb, "Self"),
    Block(kb, "Block"),
    Function(kb, "Function"),
    Delete(kb, "Delete"),
    Set(kb, "Set"),
    If(kb, "If"),
    Do(kb, "Do"),
    While(kb, "While"),
    Expression(kb, "Expression"),
    Input(kb, "Input"),
    New(kb, "New"),
    Same(kb, "Same"),
    NotSame(kb, "NotSame"),
    Equal(kb, "Equal"),
    NotEqual(kb, "NotEqual"),
    Has(kb, "Has"),
    Get(kb, "Get"),
    And(kb, "And"),
    Or(kb, "Or"),
    Not(kb, "Not"),
    Add(kb, "Add"),
    Subtract(kb, "Subtract"),
    Multiply(kb, "Multiply"),
    Divide(kb, "Divide"),
    LessThan(kb, "LessThan"),
    GreaterThan(kb, "GreaterThan")
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
    template_(kb),
    control(kb),
    ast(kb)
{
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
        listType.addMembership(kb.type.Container);
        itemType.addMembership(kb.type.Iterator);
        auto& cells = kb.cells;

        listType.addSlots(cells.slot(kb.sequence.first, itemType),
                          cells.slot(kb.sequence.last, itemType),
                          cells.slot(kb.dimensions.size, kb.type.Number));
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
        mapType.addSlots(cells.slot(kb.cells.list, ListOf(type)),
                         cells.slot(kb.cells.index, kb.type.Index),
                         cells.slot(kb.dimensions.size, kb.type.Number));

        return it.first->second;
    }
}

Cells::Cells(brain::Brain& kb, Type& voidType, Type& anyType) :
    kb(kb),
    type(kb, anyType, "type"),
    slots(kb, anyType, "slots"),
    slotType(kb, anyType, "slotType"),
    slotRole(kb, anyType, "slotRole"),
    subTypes(kb, anyType, "subTypes"),
    index(kb, anyType, "index"),
    list(kb, anyType, "list"),
    memberOf(kb, anyType, "memberOf"),
    emptyObject(kb, voidType, "emptyObject")
{
}

cells::Slot& Cells::slot(cells::CellI& role, cells::CellI& type)
{
    return *new cells::Slot(kb, role, type);
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

namespace templates {
CellDescription::CellDescription(brain::Brain& kb, CellI& classCell, const std::string& label) :
    Object(kb, classCell, label)
{
}

ParameterDecl::ParameterDecl(brain::Brain& kb, CellI& role, CellI& type) :
    Object(kb, kb.type.template_.ParameterDecl)
{
    set(kb.cells.slotRole, role);
    set(kb.cells.slotType, type);
}

Slot::Slot(brain::Brain& kb, templates::CellDescription& role, templates::CellDescription& type) :
    Object(kb, kb.type.template_.Slot)
{
    set(kb.cells.slotRole, role);
    set(kb.cells.slotType, type);
}

Cell::Cell(brain::Brain& kb, CellI& cell) :
    CellDescriptionT<Cell>(kb, kb.type.template_.Cell)
{
    set(kb.coding.value, cell);
}

Parameter::Parameter(brain::Brain& kb, CellI& paramRole) :
    CellDescriptionT<Parameter>(kb, kb.type.template_.Parameter)
{
    set(kb.coding.value, paramRole);
}

TemplateOf::TemplateOf(brain::Brain& kb, Template& templateOf, CellDescription& paramDescription, CellDescription& valueDescription) :
    CellDescriptionT<TemplateOf>(kb, kb.type.template_.TemplateOf)
{
    set(kb.coding.template_, templateOf);
    set(kb.coding.parameter, paramDescription);
    set(kb.coding.value, valueDescription);
}

Self::Self(brain::Brain& kb) :
    CellDescriptionT<Self>(kb, kb.type.template_.Self)
{
}

} // namespace templates

Templates::Templates(brain::Brain& kb) :
    kb(kb),
    list(kb, kb.type.Type_, "List")
{
}

templates::ParameterDecl& Templates::parameterDecl(CellI& role, CellI& type)
{
    return templates::ParameterDecl::New(kb, role, type);
}

templates::Slot& Templates::slot(templates::CellDescription& role, templates::CellDescription& type)
{
    return templates::Slot::New(kb, role, type);
}

templates::Cell& Templates::cell(CellI& cell)
{
    return templates::Cell::New(kb, cell);
}

templates::Parameter& Templates::parameter(CellI& paramRole)
{
    return templates::Parameter::New(kb, paramRole);
}

templates::TemplateOf& Templates::templateOf(Template& templateOf, templates::CellDescription& paramDescription, templates::CellDescription& valueDescription)
{
    return templates::TemplateOf::New(kb, templateOf, paramDescription, valueDescription);
}

templates::Self& Templates::self()
{
    return templates::Self::New(kb);
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

Ast::HasMember::HasMember(brain::Brain& kb, Base& role) :
    BaseT<HasMember>(kb, kb.type.ast.HasMember)
{
    set(kb.coding.role, role);
}

Ast::GetMember::GetMember(brain::Brain& kb, Base& role) :
    BaseT<GetMember>(kb, kb.type.ast.GetMember)
{
    set(kb.coding.role, role);
}

Ast::SetMember::SetMember(brain::Brain& kb, Base& role, Base& value) :
    BaseT<SetMember>(kb, kb.type.ast.SetMember)
{
    set(kb.coding.role, role);
    set(kb.coding.value, value);
}

Ast::SetVar::SetVar(brain::Brain& kb, CellI& role, Base& value) :
    BaseT<SetVar>(kb, kb.type.ast.SetVar)
{
    set(kb.coding.role, role);
    set(kb.coding.value, value);
}

Ast::GetVar::GetVar(brain::Brain& kb, CellI& role) :
    BaseT<GetVar>(kb, kb.type.ast.GetVar)
{
    set(kb.coding.role, role);
}

Ast::Self::Self(brain::Brain& kb) :
    BaseT<Self>(kb, kb.type.ast.Self)
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
    m_inputs = &output;
}

void Ast::Function::addAsts(Block& ast)
{
    set(kb.coding.ast, ast);
    m_asts = &ast;
}

CellI& Ast::Function::inputType()
{
    if (!m_inputType) {
        m_inputType = std::make_unique<Type>(kb, std::format("{}::parameters", label()));
        Type& type  = *m_inputType;
        Visitor::visitList(inputs(), [this, &type](CellI& slot, int i) {
            type.addSlot(slot[kb.cells.slotRole], slot[kb.cells.slotType]);
        });
    }

    return *m_inputType;
}

CellI& Ast::Function::compile(CellI& parameters)
{
    cells::control::Function& ret = *new cells::control::Function(kb);
    CellI& block                  = compileAst(asts(), inputs(), ret);

    std::stringstream ss;
    Visitor::visitList(inputs(), [this, &ss, &parameters](CellI& slot, int i) {
        CellI& role = slot[kb.cells.slotRole];
        if (!parameters.has(role)) {
            return;
        }
        if (i != 0) {
            ss << ", ";
        }
        ss << slot[kb.cells.slotRole].label() << ": " << slot[kb.cells.slotType].label();
    });
    ret.label(std::format("{}({})", label(), ss.str()));

    return ret;
}

CellI& Ast::Function::compileAst(CellI& ast, CellI& parameters, CellI& self)
{
    if (&ast.type() == &kb.type.ast.Block) {
        List& list         = static_cast<List&>(ast[kb.coding.value]);
        auto& compiledAsts = *new cells::List(kb, kb.type.control.Base);
        Visitor::visitList(list, [this, &compiledAsts, &ast, &parameters, &self](CellI& ast, int) {
            compiledAsts.add(compileAst(ast, parameters, self));
        });
        return *new control::Block(kb, compiledAsts);
    } else if (&ast.type() == &kb.type.ast.Delete) {
        return *new control::Delete(kb, compileAst(ast[kb.coding.cell], parameters, self));
    } else if (&ast.type() == &kb.type.ast.Set) {
        return *new control::Set(kb, compileAst(ast[kb.coding.cell], parameters, self), compileAst(ast[kb.coding.role], parameters, self), compileAst(ast[kb.coding.value], parameters, self));
    } else if (&ast.type() == &kb.type.ast.If) {
        if (ast.has(kb.coding.else_)) {
            return *new control::If(kb, compileAst(ast[kb.coding.condition], parameters, self), compileAst(ast[kb.coding.then], parameters, self), compileAst(ast[kb.coding.else_], parameters, self));
        } else {
            return *new control::If(kb, compileAst(ast[kb.coding.condition], parameters, self), compileAst(ast[kb.coding.then], parameters, self));
        }
    } else if (&ast.type() == &kb.type.ast.Do) {
        return *new control::Do(kb, compileAst(ast[kb.coding.condition], parameters, self), compileAst(ast[kb.coding.statement], parameters, self));
    } else if (&ast.type() == &kb.type.ast.While) {
        return *new control::While(kb, compileAst(ast[kb.coding.condition], parameters, self), compileAst(ast[kb.coding.statement], parameters, self));
    } else if (&ast.type() == &kb.type.ast.Input) {
        return *new control::Input(kb, &compileAst(ast[kb.coding.value], parameters, self));
    } else if (&ast.type() == &kb.type.ast.New) {
        return *new control::New(kb, compileAst(ast[kb.coding.objectType], parameters, self));
    } else if (&ast.type() == &kb.type.ast.And) {
        return *new control::And(kb, compileAst(ast[kb.coding.lhs], parameters, self), compileAst(ast[kb.coding.rhs], parameters, self));
    } else if (&ast.type() == &kb.type.ast.Or) {
        return *new control::Or(kb, compileAst(ast[kb.coding.lhs], parameters, self), compileAst(ast[kb.coding.rhs], parameters, self));
    } else if (&ast.type() == &kb.type.ast.Not) {
        return *new control::Not(kb, compileAst(ast[kb.coding.value], parameters, self));
    } else if (&ast.type() == &kb.type.ast.Add) {
        return *new control::Add(kb, compileAst(ast[kb.coding.lhs], parameters, self), compileAst(ast[kb.coding.rhs], parameters, self));
    } else if (&ast.type() == &kb.type.ast.Subtract) {
        return *new control::Subtract(kb, compileAst(ast[kb.coding.lhs], parameters, self), compileAst(ast[kb.coding.rhs], parameters, self));
    } else if (&ast.type() == &kb.type.ast.Multiply) {
        return *new control::Multiply(kb, compileAst(ast[kb.coding.lhs], parameters, self), compileAst(ast[kb.coding.rhs], parameters, self));
    } else if (&ast.type() == &kb.type.ast.Divide) {
        return *new control::Divide(kb, compileAst(ast[kb.coding.lhs], parameters, self), compileAst(ast[kb.coding.rhs], parameters, self));
    } else if (&ast.type() == &kb.type.ast.LessThan) {
        return *new control::LessThan(kb, compileAst(ast[kb.coding.lhs], parameters, self), compileAst(ast[kb.coding.rhs], parameters, self));
    } else if (&ast.type() == &kb.type.ast.GreaterThan) {
        return *new control::GreaterThan(kb, compileAst(ast[kb.coding.lhs], parameters, self), compileAst(ast[kb.coding.rhs], parameters, self));
    } else if (&ast.type() == &kb.type.ast.Same) {
        return *new control::Same(kb, compileAst(ast[kb.coding.lhs], parameters, self), compileAst(ast[kb.coding.rhs], parameters, self));
    } else if (&ast.type() == &kb.type.ast.NotSame) {
        return *new control::NotSame(kb, compileAst(ast[kb.coding.lhs], parameters, self), compileAst(ast[kb.coding.rhs], parameters, self));
    } else if (&ast.type() == &kb.type.ast.Equal) {
        return *new control::Equal(kb, compileAst(ast[kb.coding.lhs], parameters, self), compileAst(ast[kb.coding.rhs], parameters, self));
    } else if (&ast.type() == &kb.type.ast.NotEqual) {
        return *new control::NotEqual(kb, compileAst(ast[kb.coding.lhs], parameters, self), compileAst(ast[kb.coding.rhs], parameters, self));
    } else if (&ast.type() == &kb.type.ast.Has) {
        return *new control::Has(kb, compileAst(ast[kb.coding.cell], parameters, self), compileAst(ast[kb.coding.role], parameters, self));
    } else if (&ast.type() == &kb.type.ast.Get) {
        return *new control::Get(kb, compileAst(ast[kb.coding.cell], parameters, self), compileAst(ast[kb.coding.role], parameters, self));
    } else if (&ast.type() == &kb.type.ast.Parameter) {
        return parameters[ast[kb.coding.role]];
    } else if (&ast.type() == &kb.type.ast.Cell) {
        return ast[kb.coding.value];
    } else if (&ast.type() == &kb.type.ast.HasMember) {
        return ast; // TODO
    } else if (&ast.type() == &kb.type.ast.GetMember) {
        return ast; // TODO
    } else if (&ast.type() == &kb.type.ast.SetMember) {
        return ast; // TODO
    } else if (&ast.type() == &kb.type.ast.SetVar) {
        return ast; // TODO
    } else if (&ast.type() == &kb.type.ast.GetVar) {
        return ast; // TODO
    } else if (&ast.type() == &kb.type.ast.Function) {
        return ast; // TODO
    } else if (&ast.type() == &kb.type.ast.Self) {
        return parameters[kb.coding.self];
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

Ast::Input::Input(brain::Brain& kb, CellI& cell) :
    BaseT<Input>(kb, kb.type.ast.Input)
{
    set(kb.coding.input, cell);
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

Ast::Parameter& Ast::parameter(CellI& cell)
{
    return Parameter::New(kb, cell);
}

Ast::ParameterDecl& Ast::parameterDecl(CellI& role, CellI& type)
{
    return ParameterDecl::New(kb, role, type);
}

Ast::Cell& Ast::cell(CellI& cell)
{
    return Cell::New(kb, cell);
}

Ast::HasMember& Ast::hasMember(Base& role)
{
    return HasMember::New(kb, role);
}

Ast::GetMember& Ast::getMember(Base& role)
{
    return GetMember::New(kb, role);
}

Ast::SetMember& Ast::setMember(Base& role, Base& value)
{
    return Ast::SetMember::New(kb, role, value);
}

Ast::SetVar& Ast::setVar(CellI& cell, Base& ast)
{
    return SetVar::New(kb, cell, ast);
}

Ast::GetVar& Ast::getVar(CellI& cell)
{
    return GetVar::New(kb, cell);
}

Ast::Self& Ast::self()
{
    return Self::New(kb);
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

Ast::Input& Ast::input(CellI& cell)
{
    return Input::New(kb, cell);
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
    current(kb, kb.type.Any, "current")
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
    type(*this),
    cells(*this, type.Void, type.Any),
    coding(*this, type.Any),
    pools(*this, type.Char, cells.emptyObject, type.Digit),
    templates(*this),
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
    arc(*this),
    listAdd(*this, "List::Add")
{
    type.Type_.addSlots(
        cells.slot(cells.slots, type.MapOf(type.Slot)),
        cells.slot(cells.subTypes, type.MapOf(type.Type_)),
        cells.slot(cells.memberOf, type.MapOf(type.Type_)));

    type.Slot.addSlots(
        cells.slot(cells.slotType, type.Type_),
        cells.slot(cells.slotRole, type.Any));

    type.template_.ParameterDecl.addSlots(
        cells.slot(cells.slotType, type.Type_),
        cells.slot(cells.slotRole, type.Any));

    type.template_.Slot.addSlots(
        cells.slot(cells.slotType, type.template_.Descriptor),
        cells.slot(cells.slotRole, type.template_.Descriptor));

    type.template_.Cell.addMembership(
        type.template_.Descriptor);
    type.template_.Cell.addSlots(
        cells.slot(coding.value, type.Any));

    type.template_.Parameter.addMembership(
        type.template_.Descriptor);
    type.template_.Parameter.addSlots(
        cells.slot(coding.value, type.Any));

    type.template_.TemplateOf.addMembership(
        type.template_.Descriptor);
    type.template_.TemplateOf.addSlots(
        cells.slot(coding.template_, type.Template),
        cells.slot(coding.parameter, type.template_.Descriptor),
        cells.slot(coding.value, type.template_.Descriptor));

    type.template_.Self.addMembership(
        type.template_.Descriptor);

    Template& listItem = *new cells::Template(*this, type.Type_, "ListItem");
    listItem.addParams(
        templates.parameterDecl(coding.objectType, type.Type_));
    listItem.addSlots(
        templates.slot(templates.cell(sequence.previous), templates.self()),
        templates.slot(templates.cell(sequence.next), templates.self()),
        templates.slot(templates.cell(coding.value), templates.parameter(coding.objectType)));

    templates.list.addParams(
        templates.parameterDecl(coding.objectType, type.Type_));
    templates.list.addSlots(
        templates.slot(templates.cell(sequence.first), templates.templateOf(listItem, templates.cell(coding.objectType), templates.cell(coding.objectType))),
        templates.slot(templates.cell(sequence.last), templates.templateOf(listItem, templates.cell(coding.objectType), templates.cell(coding.objectType))),
        templates.slot(templates.cell(coding.objectType), templates.parameter(coding.objectType)),
        templates.slot(templates.cell(dimensions.size), templates.cell(type.Number)));
    templates.list.addSubTypes(
        templates.slot(templates.cell(coding.objectType), templates.cell(listItem)));

    // We should indicate that template.list is a container and it has a first, last, size, objectType member.
    // And also, that template.list -> item is an iterator prev, next, value

    Type& listSubType = *new Type(*this, "ListItem");
    listSubType.addSlots(
        cells.slot(sequence.previous, listSubType),
        cells.slot(sequence.next, listSubType),
        cells.slot(coding.value, type.Any));
    listSubType.addMembership(
        type.Iterator);

    type.List.addSlots(
        cells.slot(sequence.first, listSubType),
        cells.slot(sequence.last, listSubType),
        cells.slot(dimensions.size, type.Number));
    type.List.addSubType(
        coding.objectType, listSubType);
    type.List.addMembership(
        type.Container);

    listItem.addMembership(
        templates.cell(listSubType));

    templates.list.addMembership(
        templates.cell(type.List));

    type.Template.addSlots(
        cells.slot(coding.parameters, type.MapOf(type.template_.ParameterDecl)),
        cells.slot(cells.slots, type.ListOf(type.template_.Slot)),
        cells.slot(cells.subTypes, type.ListOf(type.template_.Slot)),
        cells.slot(cells.memberOf, type.ListOf(type.template_.Descriptor)));

    type.ast.Input.addSlots(
        cells.slot(coding.value, type.ast.Base));

    type.ast.New.addSlots(
        cells.slot(coding.objectType, type.ast.Base));

    // type.ast.Fork.addSlots();

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

    type.ast.HasMember.addSlots(
        cells.slot(coding.role, type.ast.Base));

    type.ast.GetMember.addSlots(
        cells.slot(coding.role, type.ast.Base));

    type.ast.SetMember.addSlots(
        cells.slot(coding.role, type.ast.Base),
        cells.slot(coding.value, type.ast.Base));

    type.ast.SetVar.addSlots(
        cells.slot(coding.role, type.Any),
        cells.slot(coding.value, type.ast.Base));

    type.ast.GetVar.addSlots(
        cells.slot(coding.role, type.Any));

    type.ast.Block.addSlots(
        cells.slot(coding.value, type.Any));

    type.ast.Function.addSlots(
        cells.slot(coding.input, type.ListOf(type.ast.ParameterDecl)),
        cells.slot(coding.ast, type.ListOf(type.ast.Base)),
        cells.slot(coding.output, type.ListOf(type.ast.ParameterDecl)));

    listAdd.addInputs(list(
        ast.parameterDecl(coding.self, type.List),
        ast.parameterDecl(coding.value, type.Any)));

    listAdd.addAsts(ast.block(
        ast.setVar(pools.numbers.get(1), ast.new_(ast.self())),
        ast.set(ast.getVar(pools.numbers.get(1)), ast.cell(coding.value), ast.parameter(coding.value)),
        ast.if_(ast.not_(ast.hasMember(ast.cell(sequence.first))),
                         ast.setMember(ast.cell(sequence.first), ast.getVar(pools.numbers.get(1))),          // then
                         ast.block(ast.setMember(ast.cell(sequence.next), ast.getVar(pools.numbers.get(1))), // else
                                   ast.set(ast.getVar(pools.numbers.get(1)), ast.cell(sequence.previous), ast.getMember(ast.cell(sequence.last))))),
        ast.setMember(ast.cell(sequence.last), ast.getVar(pools.numbers.get(1)))));
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

    CellTemplate testTemplate(*this);
    testTemplate.type(type.Type_);
    testTemplate.addParams(map(coding.objectType, type.Type_));
    testTemplate.addSlots(list(templates.slot(templates.cell(sequence.first), templates.templateOf(listItem, templates.cell(coding.objectType), templates.cell(coding.objectType))),
                               templates.slot(templates.cell(sequence.last), templates.templateOf(listItem, templates.cell(coding.objectType), templates.cell(coding.objectType))),
                               templates.slot(templates.cell(coding.objectType), templates.parameter(coding.objectType)),
                               templates.slot(templates.cell(dimensions.size), templates.cell(type.Number))));

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
        cells.slot(coding.output, type.control.Base));

    type.control.NotSame.addSlots(
        cells.slot(equation.lhs, type.control.Base),
        cells.slot(equation.rhs, type.control.Base),
        cells.slot(coding.output, type.control.Base));

    type.control.Equal.addSlots(
        cells.slot(equation.lhs, type.control.Base),
        cells.slot(equation.rhs, type.control.Base),
        cells.slot(coding.output, type.control.Base));

    type.control.NotEqual.addSlots(
        cells.slot(equation.lhs, type.control.Base),
        cells.slot(equation.rhs, type.control.Base),
        cells.slot(coding.output, type.control.Base));

    type.control.Has.addSlots(
        cells.slot(coding.cell, type.control.Base),
        cells.slot(coding.role, type.control.Base),
        cells.slot(coding.output, type.control.Base));

    type.control.Get.addSlots(
        cells.slot(coding.cell, type.control.Base),
        cells.slot(coding.role, type.control.Base),
        cells.slot(coding.output, type.control.Base));

    type.control.Set.addSlots(
        cells.slot(coding.cell, type.control.Base),
        cells.slot(coding.role, type.control.Base),
        cells.slot(coding.output, type.control.Base),
        cells.slot(coding.value, type.control.Base));

    type.control.And.addSlots(
        cells.slot(equation.lhs, type.control.Base),
        cells.slot(equation.rhs, type.control.Base),
        cells.slot(coding.output, type.control.Base));

    type.control.Or.addSlots(
        cells.slot(equation.lhs, type.control.Base),
        cells.slot(equation.rhs, type.control.Base),
        cells.slot(coding.output, type.control.Base));

    type.control.Not.addSlots(
        cells.slot(coding.input, type.control.Base),
        cells.slot(coding.output, type.control.Base));

    type.control.Add.addSlots(
        cells.slot(equation.lhs, type.control.Base),
        cells.slot(equation.rhs, type.control.Base),
        cells.slot(coding.output, type.control.Base));

    type.control.Subtract.addSlots(
        cells.slot(equation.lhs, type.control.Base),
        cells.slot(equation.rhs, type.control.Base),
        cells.slot(coding.output, type.control.Base));

    type.control.Multiply.addSlots(
        cells.slot(equation.lhs, type.control.Base),
        cells.slot(equation.rhs, type.control.Base),
        cells.slot(coding.output, type.control.Base));

    type.control.Divide.addSlots(
        cells.slot(equation.lhs, type.control.Base),
        cells.slot(equation.rhs, type.control.Base),
        cells.slot(coding.output, type.control.Base));

    type.control.LessThan.addSlots(
        cells.slot(equation.lhs, type.control.Base),
        cells.slot(equation.rhs, type.control.Base),
        cells.slot(coding.output, type.control.Base));

    type.control.GreaterThan.addSlots(
        cells.slot(equation.lhs, type.control.Base),
        cells.slot(equation.rhs, type.control.Base),
        cells.slot(coding.output, type.control.Base));

    type.control.Input.addSlots(
        cells.slot(sequence.first, type.control.Base),
        cells.slot(sequence.next, type.control.Base),
        cells.slot(sequence.current, type.control.Base),
        cells.slot(coding.value, type.Any));

    type.control.New.addSlots(
        cells.slot(sequence.first, type.control.Base),
        cells.slot(sequence.next, type.control.Base),
        cells.slot(sequence.current, type.control.Base),
        cells.slot(coding.value, type.control.Base),
        cells.slot(coding.objectType, type.control.Base));

    type.control.Delete.addSlots(
        cells.slot(sequence.first, type.control.Base),
        cells.slot(coding.input, type.control.Base));

    type.control.If.addSlots(
        cells.slot(sequence.first, type.control.Base),
        cells.slot(sequence.next, type.control.Base),
        cells.slot(coding.input, type.control.Base),
        cells.slot(coding.condition, type.control.Base),
        cells.slot(coding.then, type.control.Base),
        cells.slot(coding.else_, type.control.Base));

    type.control.Do.addSlots(
        cells.slot(sequence.first, type.control.Base),
        cells.slot(sequence.next, type.control.Base),
        cells.slot(coding.input, type.control.Base),
        cells.slot(coding.condition, type.control.Base),
        cells.slot(coding.statement, type.control.Base));

    type.control.While.addSlots(
        cells.slot(sequence.first, type.control.Base),
        cells.slot(sequence.next, type.control.Base),
        cells.slot(coding.input, type.control.Base),
        cells.slot(coding.condition, type.control.Base),
        cells.slot(coding.statement, type.control.Base));

    type.control.Function.addSlots(
        cells.slot(coding.input, type.MapOf(type.control.Base)),
        cells.slot(coding.ast, type.ListOf(type.control.Base)),
        cells.slot(coding.output, type.MapOf(type.control.Base)));

    m_initialized = true;
}

CellI& Brain::toKbBool(bool value)
{
    return value ? boolean.true_ : boolean.false_;
}

bool Brain::isInitialized()
{
    return m_initialized;
}

} // namespace brain
} // namespace cells
} // namespace synth
