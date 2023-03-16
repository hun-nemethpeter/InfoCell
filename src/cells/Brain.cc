#include "Cells.h"

#include "Brain.h"

#include <format>

namespace synth {
namespace cells {
namespace brain {

static void test()
{
    Brain kb;
}
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

Pipelines::Pipelines(brain::Brain& kb) :
    Base(kb, "Base"),
    Void(kb, "Void"),
    Input(kb, "Input"),
    New(kb, "New"),
    Fork(kb, "Fork"),
    Delete(kb, "Delete"),
    Node(kb, "Node"),
    IfThen(kb, "IfThen"),
    DoWhile(kb, "DoWhile"),
    While(kb, "While")
{
}

namespace op {

Logic::Logic(brain::Brain& kb) :
    And(kb, "And"),
    Or(kb, "Or"),
    Not(kb, "Not")
{
}

Math::Math(brain::Brain& kb) :
    Add(kb, "Add"),
    Subtract(kb, "Subtract"),
    Multiply(kb, "Multiply"),
    Divide(kb, "Divide"),
    LessThan(kb, "LessThan"),
    GreaterThan(kb, "GreaterThan")
{
}

} // namespace op

Operations::Operations(brain::Brain& kb) :
    Base(kb, "Base"),
    Same(kb, "Same"),
    NotSame(kb, "NotSame"),
    Equal(kb, "Equal"),
    NotEqual(kb, "NotEqual"),
    Has(kb, "Has"),
    Get(kb, "Get"),
    Set(kb, "Set"),
    logic(kb), math(kb)
{
}

Ast::Pipelines::Pipelines(brain::Brain& kb) :
    Input(kb, "Input"),
    New(kb, "New"),
    Fork(kb, "Fork"),
    Delete(kb, "Delete"),
    Node(kb, "Node"),
    If(kb, "If"),
    Do(kb, "Do"),
    While(kb, "While")
{
}

Ast::Operations::Logic::Logic(brain::Brain& kb) :
    And(kb, "And"),
    Or(kb, "Or"),
    Not(kb, "Not")
{
}

Ast::Operations::Math::Math(brain::Brain& kb) :
    Add(kb, "Add"),
    Subtract(kb, "Subtract"),
    Multiply(kb, "Multiply"),
    Divide(kb, "Divide"),
    LessThan(kb, "LessThan"),
    GreaterThan(kb, "GreaterThan")
{
}

Ast::Operations::Operations(brain::Brain& kb) :
    Base(kb, "Base"),
    Same(kb, "Same"),
    NotSame(kb, "NotSame"),
    Equal(kb, "Equal"),
    NotEqual(kb, "NotEqual"),
    Has(kb, "Has"),
    Get(kb, "Get"),
    Set(kb, "Set"),
    logic(kb),
    math(kb)
{
}

Ast::Ast(brain::Brain& kb) :
        kb(kb),
        Parameter(kb, "Parameter"),
        Cell(kb, "Cell"),
        HasMember(kb, "HasMember"),
        GetMember(kb, "GetMember"),
        SetMember(kb, "SetMember"),
        SetVar(kb, "SetVar"),
        GetVar(kb, "GetVar"),
        Self(kb, "Self"),
        Block(kb, "Block"),
        op(kb),
        pipeline(kb)
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
    op(kb),
    pipeline(kb),
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
    branch(kb, anyType, "branch"),
    cell(kb, anyType, "cell"),
    condition(kb, anyType, "condition"),
    container(kb, anyType, "container"),
    else_(kb, anyType, "else_"),
    input(kb, anyType, "input"),
    item(kb, anyType, "item"),
    label(kb, anyType, "label"),
    objectType(kb, anyType, "objectType"),
    op(kb, anyType, "op"),
    output(kb, anyType, "output"),
    parameter(kb, anyType, "parameter"),
    parameters(kb, anyType, "parameters"),
    result(kb, anyType, "result"),
    role(kb, anyType, "role"),
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

namespace ast {
Base::Base(brain::Brain& kb, CellI& classCell, const std::string& label) :
    Object(kb, classCell, label)
{
}

namespace pipeline {

Input::Input(brain::Brain& kb, CellI& cell) :
    BaseT<Input>(kb, kb.type.ast.pipeline.Input)
{
    set(kb.coding.input, cell);
}

New::New(brain::Brain& kb, Base& ast) :
    BaseT<New>(kb, kb.type.ast.pipeline.New)
{
}

Fork::Fork(brain::Brain& kb) :
    BaseT<Fork>(kb, kb.type.ast.pipeline.Fork)
{
}

Delete::Delete(brain::Brain& kb, Base& ast) :
    BaseT<Delete>(kb, kb.type.ast.pipeline.Delete)
{
}

Node::Node(brain::Brain& kb) :
    BaseT<Node>(kb, kb.type.ast.pipeline.Node)
{
}

If::If(brain::Brain& kb, Base& condition, Base& thenBranch) :
    BaseT<If>(kb, kb.type.ast.pipeline.If)
{
}

If::If(brain::Brain& kb, Base& condition, Base& thenBranch, Base& elseBranch) :
    BaseT<If>(kb, kb.type.ast.pipeline.If)
{
}

Do::Do(brain::Brain& kb, Base& condition, Base& statement) :
    BaseT<Do>(kb, kb.type.ast.pipeline.Do)
{
}

While::While(brain::Brain& kb, Base& condition, Base& statement) :
    BaseT<While>(kb, kb.type.ast.pipeline.While)
{
}

} // namespace pipeline

Pipeline::Pipeline(brain::Brain& kb) :
    kb(kb)
{
}

pipeline::Input& Pipeline::input(CellI& cell)
{
    return pipeline::Input::New(kb, cell);
}

pipeline::New& Pipeline::new_(Base& ast)
{
    return pipeline::New::NewT<pipeline::New>::New(kb, ast);
}

pipeline::Fork& Pipeline::fork()
{
    return pipeline::Fork::New(kb);
}

pipeline::Delete& Pipeline::delete_(Base& ast)
{
    return pipeline::Delete::New(kb, ast);
}

pipeline::Node& Pipeline::node()
{
    return pipeline::Node::New(kb);
}

pipeline::If& Pipeline::if_(Base& condition, Base& thenBranch)
{
    return pipeline::If::New(kb, condition, thenBranch);
}

pipeline::If& Pipeline::if_(Base& condition, Base& thenBranch, Base& elseBranch)
{
    return pipeline::If::New(kb, condition, thenBranch, elseBranch);
}

pipeline::Do& Pipeline::do_(Base& condition, Base& statement)
{
    return pipeline::Do::New(kb, condition, statement);
}

pipeline::While& Pipeline::while_(Base& condition, Base& statement)
{
    return pipeline::While::New(kb, condition, statement);
}

namespace logic {

And::And(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<And>(kb, kb.type.ast.op.logic.And)
{
}

Or::Or(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Or>(kb, kb.type.ast.op.logic.Or)
{
}

Not::Not(brain::Brain& kb, Base& input) :
    BaseT<Not>(kb, kb.type.ast.op.logic.Not)
{
}

} // namespace logic

Logic::Logic(brain::Brain& kb) :
    kb(kb)
{
}

logic::And& Logic::and_(Base& lhs, Base& rhs)
{
    return logic::And::New(kb, lhs, rhs);
}

logic::Or& Logic::or_(Base& lhs, Base& rhs)
{
    return logic::Or::New(kb, lhs, rhs);
}

logic::Not& Logic::not_(Base& input)
{
    return logic::Not::New(kb, input);
}
namespace math {
Add::Add(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Add>(kb, kb.type.ast.op.math.Add)
{
}

Subtract::Subtract(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Subtract>(kb, kb.type.ast.op.math.Subtract)
{
}

Multiply::Multiply(brain::Brain& kb, Base& lhs, Base& rhs)
    : BaseT<Multiply>(kb, kb.type.ast.op.math.Multiply)
{
}

Divide::Divide(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Divide>(kb, kb.type.ast.op.math.Divide)
{
}

LessThan::LessThan(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<LessThan>(kb, kb.type.ast.op.math.LessThan)
{
}

GreaterThan::GreaterThan(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<GreaterThan>(kb, kb.type.ast.op.math.GreaterThan)
{
}

} // namespace math

Math::Math(brain::Brain& kb) :
    kb(kb)
{
}

math::Add& Math::add(Base& lhs, Base& rhs)
{
    return math::Add::New(kb, lhs, rhs);
}

math::Subtract& Math::subtract(Base& lhs, Base& rhs)
{
    return math::Subtract::New(kb, lhs, rhs);
}

math::Multiply& Math::multiply(Base& lhs, Base& rhs)
{
    return math::Multiply::New(kb, lhs, rhs);
}

math::Divide& Math::divide(Base& lhs, Base& rhs)
{
    return math::Divide::New(kb, lhs, rhs);
}

math::LessThan& Math::lessThan(Base& lhs, Base& rhs)
{
    return math::LessThan::New(kb, lhs, rhs);
}

math::GreaterThan& Math::greaterThan(Base& lhs, Base& rhs)
{
    return math::GreaterThan::New(kb, lhs, rhs);
}

Same::Same(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Same>(kb, kb.type.ast.op.Same)
{
}

NotSame::NotSame(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<NotSame>(kb, kb.type.ast.op.NotSame)
{
}

Equal::Equal(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Equal>(kb, kb.type.ast.op.Equal)
{
}

NotEqual::NotEqual(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<NotEqual>(kb, kb.type.ast.op.NotEqual)
{
}

Has::Has(brain::Brain& kb, Base& cell, Base& role) :
    BaseT<Has>(kb, kb.type.ast.op.Has)
{
}

Get::Get(brain::Brain& kb, Base& cell, Base& role) :
    BaseT<Get>(kb, kb.type.ast.op.Get)
{
}

Set::Set(brain::Brain& kb, Base& cell, Base& role, Base& value) :
    BaseT<Set>(kb, kb.type.ast.op.Set)
{
}

Op::Op(brain::Brain& kb) :
    kb(kb),
    logic(kb),
    math(kb)
{
}

Same& Op::same(Base& lhs, Base& rhs)
{
    return Same::New(kb, lhs, rhs);
}

NotSame& Op::notSame(Base& lhs, Base& rhs)
{
    return NotSame::New(kb, lhs, rhs);
}

Equal& Op::equal(Base& lhs, Base& rhs)
{
    return Equal::New(kb, lhs, rhs);
}

NotEqual& Op::notEqual(Base& lhs, Base& rhs)
{
    return NotEqual::New(kb, lhs, rhs);
}

Has& Op::has(Base& cell, Base& role)
{
    return Has::New(kb, cell, role);
}

Get& Op::get(Base& cell, Base& role)
{
    return Get::New(kb, cell, role);
}

Set& Op::set(Base& cell, Base& role, Base& value)
{
    return Set::New(kb, cell, role, value);
}

Parameter::Parameter(brain::Brain& kb, CellI& cell) :
    BaseT<Parameter>(kb, kb.type.ast.Parameter)
{
}

Cell::Cell(brain::Brain& kb, CellI& cell) :
    BaseT<Cell>(kb, kb.type.ast.Cell)
{
}

HasMember::HasMember(brain::Brain& kb, Base& role) :
    BaseT<HasMember>(kb, kb.type.ast.HasMember)
{
}

GetMember::GetMember(brain::Brain& kb, Base& role) :
    BaseT<GetMember>(kb, kb.type.ast.GetMember)
{
}

SetMember::SetMember(brain::Brain& kb, Base& role, Base& value) :
    BaseT<SetMember>(kb, kb.type.ast.SetMember)
{
}

SetVar::SetVar(brain::Brain& kb, CellI& cell, Base& ast) :
    BaseT<SetVar>(kb, kb.type.ast.SetVar)
{
}

GetVar::GetVar(brain::Brain& kb, CellI& cell) :
    BaseT<GetVar>(kb, kb.type.ast.GetVar)
{
}

Self::Self(brain::Brain& kb) :
    BaseT<Self>(kb, kb.type.ast.Self)
{
}

Block::Block(brain::Brain& kb) :
    BaseT<Block>(kb, kb.type.ast.Block)
{
}

void Block::add(Base& ast)
{
    // TODO
}

List& Block::toList()
{
    return kb.list(kb.type.Any); // TODO
}

} // namespace ast

Ast::Ast(brain::Brain& kb) :
    kb(kb),
    pipeline(kb),
    op(kb)
{
}

ast::Parameter& Ast::parameter(CellI& cell)
{
    return ast::Parameter::New(kb, cell);
}

ast::Cell& Ast::cell(CellI& cell)
{
    return ast::Cell::New(kb, cell);
}

ast::HasMember& Ast::hasMember(ast::Base& role)
{
    return ast::HasMember::New(kb, role);
}

ast::GetMember& Ast::getMember(ast::Base& role)
{
    return ast::GetMember::New(kb, role);
}

ast::SetMember& Ast::setMember(ast::Base& role, ast::Base& value)
{
    return ast::SetMember::New(kb, role, value);
}

ast::SetVar& Ast::setVar(CellI& cell, ast::Base& ast)
{
    return ast::SetVar::New(kb, cell, ast);
}

ast::GetVar& Ast::getVar(CellI& cell)
{
    return ast::GetVar::New(kb, cell);
}

ast::Self& Ast::self()
{
    return ast::Self::New(kb);
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
    arc(*this)
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
        type.Iterator
    );

    type.List.addSlots(
        cells.slot(sequence.first, listSubType),
        cells.slot(sequence.last, listSubType),
        cells.slot(dimensions.size, type.Number));
    type.List.addSubType(
        coding.objectType, listSubType);
    type.List.addMembership(
        type.Container
    );

    listItem.addMembership(
        templates.cell(listSubType));

    templates.list.addMembership(
        templates.cell(type.List));

    type.Template.addSlots(
        cells.slot(coding.parameters, type.MapOf(type.template_.ParameterDecl)),
        cells.slot(cells.slots, type.ListOf(type.template_.Slot)),
        cells.slot(cells.subTypes, type.ListOf(type.template_.Slot)),
        cells.slot(cells.memberOf, type.ListOf(type.template_.Descriptor)));

    Function listAdd(*this, "List::Add");
    listAdd.addAsts(ast.block(
        ast.setVar(pools.numbers.get(1), ast.pipeline.new_(ast.self())),
        ast.op.set(ast.getVar(pools.numbers.get(1)), ast.cell(coding.value), ast.parameter(coding.value)),
                         ast.pipeline.if_(ast.op.logic.not_(ast.hasMember(ast.cell(sequence.first))),
                         ast.setMember(ast.cell(sequence.first), ast.getVar(pools.numbers.get(1))),
                         ast.block(ast.setMember(ast.cell(sequence.next), ast.getVar(pools.numbers.get(1))),
                                   ast.op.set(ast.getVar(pools.numbers.get(1)), ast.cell(sequence.previous), ast.getMember(ast.cell(sequence.last))))),
        ast.setMember(ast.cell(sequence.last), ast.getVar(pools.numbers.get(1)))
    ).toList());
#if 0
    Function listAdd(*this, "List::Add");
    listAdd.addInputs(list(function.parameterDecl(coding.self, type.List),
                           function.parameterDecl(coding.value, type.Any)));
    listAdd.addAsts(list(
        ListItem* newListItem = new ListItem();                                    // auto newListItem = ast.new_(type.self);
        newListItem->value    = param[coding.value];                               // ast.op.set(newListItem, coding.value, ast.parameter(coding.value));
        if (!param[coding.self].has(sequence.first)) {                             // ast.pipeline.if_(ast.op.logic.not(ast.op.has(ast.self, sequence.first)),
            param[coding.self].set(sequence.first, newListItem);                   //                  ast.op.set(ast.self, sequence.first, newListItem)
        } else {
            param[coding.self][sequence.last].set(sequence.next, newListItem);     //                  list(ast.op.set(ast.self, sequence.next, newListItem),
            newListItem.set(sequence.previous, param[coding.self][sequence.last]); //                       ast.op.set(newListItem, sequence.previous, )));
        }
        param[coding.self].set(sequence.last, newListItem);                        // ast.op.set(ast.self, sequence.last, newListItem);
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

    type.op.Same.addSlots(
        cells.slot(equation.lhs, type.pipeline.Base),
        cells.slot(equation.rhs, type.pipeline.Base),
        cells.slot(coding.output, type.pipeline.Base));

    type.op.NotSame.addSlots(
        cells.slot(equation.lhs, type.pipeline.Base),
        cells.slot(equation.rhs, type.pipeline.Base),
        cells.slot(coding.output, type.pipeline.Base));

    type.op.Equal.addSlots(
        cells.slot(equation.lhs, type.pipeline.Base),
        cells.slot(equation.rhs, type.pipeline.Base),
        cells.slot(coding.output, type.pipeline.Base));

    type.op.NotEqual.addSlots(
        cells.slot(equation.lhs, type.pipeline.Base),
        cells.slot(equation.rhs, type.pipeline.Base),
        cells.slot(coding.output, type.pipeline.Base));

    type.op.Has.addSlots(
        cells.slot(coding.cell, type.pipeline.Base),
        cells.slot(coding.role, type.pipeline.Base),
        cells.slot(coding.output, type.op.Base));

    type.op.Get.addSlots(
        cells.slot(coding.cell, type.pipeline.Base),
        cells.slot(coding.role, type.pipeline.Base),
        cells.slot(coding.output, type.op.Base));

    type.op.Set.addSlots(
        cells.slot(coding.cell, type.pipeline.Base),
        cells.slot(coding.role, type.pipeline.Base),
        cells.slot(coding.output, type.op.Base),
        cells.slot(coding.value, type.pipeline.Base));

    type.op.logic.And.addSlots(
        cells.slot(equation.lhs, type.pipeline.Base),
        cells.slot(equation.rhs, type.pipeline.Base),
        cells.slot(coding.output, type.pipeline.Base));

    type.op.logic.Or.addSlots(
        cells.slot(equation.lhs, type.pipeline.Base),
        cells.slot(equation.rhs, type.pipeline.Base),
        cells.slot(coding.output, type.pipeline.Base));

    type.op.logic.Not.addSlots(
        cells.slot(coding.input, type.pipeline.Base),
        cells.slot(coding.output, type.pipeline.Base));

    type.op.math.Add.addSlots(
        cells.slot(equation.lhs, type.pipeline.Base),
        cells.slot(equation.rhs, type.pipeline.Base),
        cells.slot(coding.output, type.pipeline.Base));

    type.op.math.Subtract.addSlots(
        cells.slot(equation.lhs, type.pipeline.Base),
        cells.slot(equation.rhs, type.pipeline.Base),
        cells.slot(coding.output, type.pipeline.Base));

    type.op.math.Multiply.addSlots(
        cells.slot(equation.lhs, type.pipeline.Base),
        cells.slot(equation.rhs, type.pipeline.Base),
        cells.slot(coding.output, type.pipeline.Base));

    type.op.math.Divide.addSlots(
        cells.slot(equation.lhs, type.pipeline.Base),
        cells.slot(equation.rhs, type.pipeline.Base),
        cells.slot(coding.output, type.pipeline.Base));

    type.op.math.LessThan.addSlots(
        cells.slot(equation.lhs, type.pipeline.Base),
        cells.slot(equation.rhs, type.pipeline.Base),
        cells.slot(coding.output, type.pipeline.Base));

    type.op.math.GreaterThan.addSlots(
        cells.slot(equation.lhs, type.pipeline.Base),
        cells.slot(equation.rhs, type.pipeline.Base),
        cells.slot(coding.output, type.pipeline.Base));

    type.pipeline.Void.addSlots(
        cells.slot(sequence.first, type.pipeline.Base),
        cells.slot(sequence.next, type.pipeline.Base),
        cells.slot(sequence.current, type.pipeline.Base));

    type.pipeline.Input.addSlots(
        cells.slot(sequence.first, type.pipeline.Base),
        cells.slot(sequence.next, type.pipeline.Base),
        cells.slot(sequence.current, type.pipeline.Base),
        cells.slot(coding.value, type.Any));

    type.pipeline.New.addSlots(
        cells.slot(sequence.first, type.pipeline.Base),
        cells.slot(sequence.next, type.pipeline.Base),
        cells.slot(sequence.current, type.pipeline.Base),
        cells.slot(coding.value, type.pipeline.Base),
        cells.slot(coding.objectType, type.op.Base));

    type.pipeline.Fork.addSlots(
        cells.slot(sequence.first, type.pipeline.Base),
        cells.slot(sequence.next, type.pipeline.Base),
        cells.slot(coding.input, type.pipeline.Base),
        cells.slot(coding.value, type.pipeline.Base),
        cells.slot(coding.branch, type.op.Base));

    type.pipeline.Delete.addSlots(
        cells.slot(sequence.first, type.pipeline.Base),
        cells.slot(coding.input, type.pipeline.Base));

    type.pipeline.Node.addSlots(
        cells.slot(sequence.first, type.pipeline.Base),
        cells.slot(sequence.next, type.pipeline.Base),
        cells.slot(coding.input, type.pipeline.Base),
        cells.slot(coding.op, type.op.Base),
        cells.slot(coding.value, type.pipeline.Base));

    type.pipeline.IfThen.addSlots(
        cells.slot(sequence.first, type.pipeline.Base),
        cells.slot(sequence.next, type.pipeline.Base),
        cells.slot(coding.input, type.pipeline.Base),
        cells.slot(coding.condition, type.pipeline.Base),
        cells.slot(coding.then, type.op.Base),
        cells.slot(coding.else_, type.op.Base));

    type.pipeline.DoWhile.addSlots(
        cells.slot(sequence.first, type.pipeline.Base),
        cells.slot(sequence.next, type.pipeline.Base),
        cells.slot(coding.input, type.pipeline.Base),
        cells.slot(coding.condition, type.pipeline.Base),
        cells.slot(coding.statement, type.op.Base));

    type.pipeline.While.addSlots(
        cells.slot(sequence.first, type.pipeline.Base),
        cells.slot(sequence.next, type.pipeline.Base),
        cells.slot(coding.input, type.pipeline.Base),
        cells.slot(coding.condition, type.pipeline.Base),
        cells.slot(coding.statement, type.op.Base));

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
