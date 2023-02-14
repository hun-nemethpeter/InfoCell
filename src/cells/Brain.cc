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
    TemplateSlot(kb, "TemplateSlot"),
    DescriptorCell(kb, "DescriptorCell"),
    DescriptorParameter(kb, "DescriptorParameter"),
    DescriptorTemplate(kb, "DescriptorTemplate"),
    DescriptorSelf(kb, "DescriptorSelf")
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

} // namespace type

TypeInit::TypeInit(Types& types)
{
    // This is the first type, breaking the infinite init loop here with pre initializing a List<Slot> and ListItem<Slot> here
    // Every type has a list of Slots, so it needs a List<Slot>
    Brain& kb  = types.kb;

    {
        Type& type     = types.kb.type.Slot;
        auto listIt    = types.m_listTypes.emplace(std::piecewise_construct,
                                                   std::forward_as_tuple(&type),
                                                   std::forward_as_tuple(kb, "List<Slot>", Type::InitMode::InitDuringBootstrap));
        Type& listType = listIt.first->second;
        Type& itemType = listType.addSubType(kb.coding.objectType, "ListItem<Slot>", Type::InitMode::InitDuringBootstrap);
        listType.addSlots({ { kb.sequence.first, itemType },
                            { kb.sequence.last, itemType },
                            { kb.dimensions.size, kb.type.Number } });
        itemType.addSlots({ { kb.sequence.previous, itemType },
                            { kb.sequence.next, itemType },
                            { kb.coding.value, type } });

        itemType.manualInit();
        listType.manualInit();
    }
    {
        Type& type = types.kb.type.Type_;
        auto listIt    = types.m_listTypes.emplace(std::piecewise_construct,
                                                   std::forward_as_tuple(&type),
                                                   std::forward_as_tuple(kb, "List<Type>", Type::InitMode::InitDuringBootstrap));
        Type& listType = listIt.first->second;
        Type& itemType = listType.addSubType(kb.coding.objectType, "ListItem<Type>", Type::InitMode::InitDuringBootstrap);
        listType.addSlots({ { kb.sequence.first, itemType },
                            { kb.sequence.last, itemType },
                            { kb.dimensions.size, kb.type.Number } });
        itemType.addSlots({ { kb.sequence.previous, itemType },
                            { kb.sequence.next, itemType },
                            { kb.coding.value, type } });
        itemType.manualInit();
        listType.manualInit();
    }
}

Types::Types(brain::Brain& kb) :
    m_init(*this),
    Type_(kb, "Type"),
    Slot(kb, "Slot"),
    List(kb, "List"),
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
    kb(kb)
{
    m_listTypes.at(&kb.type.Slot).manualInitMembers();
    m_listTypes.at(&kb.type.Type_).manualInitMembers();
}

Type& Types::ListOf(CellI& type)
{
    auto numberIt = m_listTypes.find(&type);
    if (numberIt != m_listTypes.end()) {
        return numberIt->second;
    } else {
        auto it        = m_listTypes.emplace(std::piecewise_construct,
                                             std::forward_as_tuple(&type),
                                             std::forward_as_tuple(kb, std::format("List<{}>", type.label()), Type::InitMode::InitSubTypes));
        Type& listType = it.first->second;
        Type& itemType = listType.addSubType(kb.coding.objectType, std::format("ListItem<{}>", type.label()), Type::InitMode::InitSubTypes);
        listType.addSlots({ { kb.sequence.first, itemType },
                            { kb.sequence.last, itemType },
                            { kb.dimensions.size, kb.type.Number } });
        itemType.addSlots({ { kb.sequence.previous, itemType },
                            { kb.sequence.next, itemType },
                            { kb.coding.value, type } });

        return it.first->second;
    }
    using CellT       = Template::CellDescription::Cell;
    using ParamT      = Template::CellDescription::Parameter;
    using TemplateOfT = Template::CellDescription::TemplateOf;
    using SelfTypeT   = Template::CellDescription::SelfType;

    cells::Template item(kb, "ListItem<T>", { { kb.coding.objectType, kb.type.Any } });
    item.addSlots({ { CellT(kb.sequence.previous), SelfTypeT() },
                    { CellT(kb.sequence.next), SelfTypeT() },
                    { CellT(kb.coding.value), ParamT(kb.coding.objectType) } });
    cells::Template list(kb, "List<T>", { { kb.coding.objectType, kb.type.Any } });
    list.addSlots({ { CellT(kb.sequence.first), TemplateOfT(item, ParamT(kb.coding.objectType)) },
                    { CellT(kb.sequence.last), SelfTypeT() },
                    { CellT(kb.dimensions.size), CellT(kb.type.Number) } });
}

Type& Types::GroupOf(CellI& type)
{
    return kb.type.Any; // TODO
}

Cells::Cells(brain::Brain& kb, Type& voidType, Type& anyType) :
    type(kb, anyType, "type"),
    slots(kb, anyType, "slots"),
    slotType(kb, anyType, "slotType"),
    slotRole(kb, anyType, "slotRole"),
    subTypes(kb, anyType, "subTypes"),
    list(kb, anyType, "list"),
    index(kb, anyType, "index"),
    emptyObject(kb, voidType, "emptyObject")
{
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

Sequence::Sequence(brain::Brain& kb, Type& anyType) :
    first(kb, anyType, "first"),
    last(kb, anyType, "last"),
    previous(kb, anyType, "previous"),
    next(kb, anyType, "next"),
    current(kb, anyType, "current")
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
    sign.add(positive);
    sign.add(negative);
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
        auto it = m_numbers.emplace(std::piecewise_construct,
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
    Demonstration.addSlots(
        { { kb.coding.input, kb.type.Picture },
          { kb.coding.output, kb.type.Picture } });

    Task.addSlots(
        { { examples, kb.type.ListOf(Demonstration) },
          { kb.coding.input, kb.type.Picture },
          { kb.coding.output, kb.type.Picture } });
}

Brain::Brain() :
    type(*this),
    cells(*this, type.Void, type.Any),
    coding(*this, type.Any),
    pools(*this, type.Char, cells.emptyObject, type.Digit),
    sequence(*this, type.Any),
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
        { { cells.slots, type.GroupOf(type.Slot) },
          { cells.subTypes, type.GroupOf(type.Type_) } });

    type.Slot.addSlots(
        { { cells.slotType, type.Type_ },
          { cells.slotRole, type.Any } });

    type.template_.TemplateSlot.addSlots(
        { { cells.slotType, type.Any },
          { cells.slotRole, type.Any } });

    type.template_.DescriptorCell.addSlots(
        { { coding.value, type.Any } });

    type.template_.DescriptorParameter.addSlots(
        { { coding.value, type.Any } });

    type.template_.DescriptorTemplate.addSlots(
        { { coding.template_, type.Template },
          { coding.parameter, type.Any } });

    {
        using CellT       = Template::CellDescription::Cell;
        using ParamT      = Template::CellDescription::Parameter;
        using TemplateOfT = Template::CellDescription::TemplateOf;
        using SelfTypeT   = Template::CellDescription::SelfType;

        cells::Template item(*this, "ListItem<T>", { { coding.objectType, type.Any } });
        item.addSlots({ { CellT(sequence.previous), SelfTypeT() },
                        { CellT(sequence.next), SelfTypeT() },
                        { CellT(coding.value), ParamT(coding.objectType) } });
        cells::Template list(*this, "List<T>", { { coding.objectType, type.Any } });
        list.addSlots({ { CellT(sequence.first), TemplateOfT(item, ParamT(coding.objectType)) },
                        { CellT(sequence.last), SelfTypeT() },
                        { CellT(dimensions.size), CellT(type.Number) } });
    }
    Type& listSubType = type.List.addSubType(coding.objectType);
    listSubType.addSlots({ { sequence.previous, listSubType },
                           { sequence.next, listSubType },
                           { coding.value, type.Any } });
    type.List.addSlots(
        { { sequence.first, listSubType },
          { sequence.last, listSubType },
          { dimensions.size, type.Number } });

    type.Number.addSlots(
        { { coding.value, type.ListOf(type.Digit) },
          { numbers.sign, type.Number } }); // TODO

    type.String.addSlots({ { coding.value, type.ListOf(type.Char) } });

    type.Color.addSlots(
        { { colors.red, type.Number },
          { colors.green, type.Number },
          { colors.blue, type.Number } });

    type.Pixel.addSlots(
        { { directions.up, type.Pixel },
          { directions.down, type.Pixel },
          { directions.left, type.Pixel },
          { directions.right, type.Pixel },
          { coordinates.x, type.Number },
          { coordinates.y, type.Number } });

    type.Picture.addSlots(
        { { dimensions.width, type.Number },
          { dimensions.height, type.Number },
          { visualization.pixels, type.ListOf(type.Pixel) } });

    type.op.Same.addSlots({ { equation.lhs, type.pipeline.Base },
                       { equation.rhs, type.pipeline.Base },
                       { coding.output, type.pipeline.Base } });

    type.op.NotSame.addSlots({ { equation.lhs, type.pipeline.Base },
                          { equation.rhs, type.pipeline.Base },
                          { coding.output, type.pipeline.Base } });

    type.op.Equal.addSlots({ { equation.lhs, type.pipeline.Base },
                        { equation.rhs, type.pipeline.Base },
                        { coding.output, type.pipeline.Base } });

    type.op.NotEqual.addSlots({ { equation.lhs, type.pipeline.Base },
                           { equation.rhs, type.pipeline.Base },
                           { coding.output, type.pipeline.Base } });

    type.op.Has.addSlots({ { coding.cell, type.pipeline.Base },
                      { coding.role, type.pipeline.Base },
                      { coding.output, type.op.Base } });

    type.op.Get.addSlots({ { coding.cell, type.pipeline.Base },
                      { coding.role, type.pipeline.Base },
                      { coding.output, type.op.Base } });

    type.op.Set.addSlots({ { coding.cell, type.pipeline.Base },
                      { coding.role, type.pipeline.Base },
                      { coding.output, type.op.Base },
                      { coding.value, type.pipeline.Base } });

    type.op.logic.And.addSlots({ { equation.lhs, type.pipeline.Base },
                            { equation.rhs, type.pipeline.Base },
                            { coding.output, type.pipeline.Base } });

    type.op.logic.Or.addSlots({ { equation.lhs, type.pipeline.Base },
                           { equation.rhs, type.pipeline.Base },
                           { coding.output, type.pipeline.Base } });

    type.op.logic.Not.addSlots({ { coding.input, type.pipeline.Base },
                            { coding.output, type.pipeline.Base } });

    type.op.math.Add.addSlots({ { equation.lhs, type.pipeline.Base },
                           { equation.rhs, type.pipeline.Base },
                           { coding.output, type.pipeline.Base } });

    type.op.math.Subtract.addSlots({ { equation.lhs, type.pipeline.Base },
                                { equation.rhs, type.pipeline.Base },
                                { coding.output, type.pipeline.Base } });

    type.op.math.Multiply.addSlots({ { equation.lhs, type.pipeline.Base },
                                { equation.rhs, type.pipeline.Base },
                                { coding.output, type.pipeline.Base } });

    type.op.math.Divide.addSlots({ { equation.lhs, type.pipeline.Base },
                              { equation.rhs, type.pipeline.Base },
                              { coding.output, type.pipeline.Base } });

    type.op.math.LessThan.addSlots({ { equation.lhs, type.pipeline.Base },
                                { equation.rhs, type.pipeline.Base },
                                { coding.output, type.pipeline.Base } });

    type.op.math.GreaterThan.addSlots({ { equation.lhs, type.pipeline.Base },
                                   { equation.rhs, type.pipeline.Base },
                                   { coding.output, type.pipeline.Base } });

    type.pipeline.Void.addSlots({ { sequence.first, type.pipeline.Base },
                             { sequence.next, type.pipeline.Base },
                             { sequence.current, type.pipeline.Base } });

    type.pipeline.Input.addSlots({ { sequence.first, type.pipeline.Base },
                              { sequence.next, type.pipeline.Base },
                              { sequence.current, type.pipeline.Base },
                              { coding.value, type.Any } });

    type.pipeline.New.addSlots({ { sequence.first, type.pipeline.Base },
                            { sequence.next, type.pipeline.Base },
                            { sequence.current, type.pipeline.Base },
                            { coding.value, type.pipeline.Base },
                            { coding.objectType, type.op.Base } });

    type.pipeline.Fork.addSlots({ { sequence.first, type.pipeline.Base },
                             { sequence.next, type.pipeline.Base },
                             { coding.input, type.pipeline.Base },
                             { coding.value, type.pipeline.Base },
                             { coding.branch, type.op.Base } });

    type.pipeline.Delete.addSlots({ { sequence.first, type.pipeline.Base },
                               { coding.input, type.pipeline.Base } });

    type.pipeline.Node.addSlots({ { sequence.first, type.pipeline.Base },
                             { sequence.next, type.pipeline.Base },
                             { coding.input, type.pipeline.Base },
                             { coding.op, type.op.Base },
                             { coding.value, type.pipeline.Base } });

    type.pipeline.IfThen.addSlots({ { sequence.first, type.pipeline.Base },
                               { sequence.next, type.pipeline.Base },
                               { coding.input, type.pipeline.Base },
                               { coding.condition, type.pipeline.Base },
                               { coding.then, type.op.Base },
                               { coding.else_, type.op.Base } });

    type.pipeline.DoWhile.addSlots({ { sequence.first, type.pipeline.Base },
                                { sequence.next, type.pipeline.Base },
                                { coding.input, type.pipeline.Base },
                                { coding.condition, type.pipeline.Base },
                                { coding.statement, type.op.Base } });

    type.pipeline.While.addSlots({ { sequence.first, type.pipeline.Base },
                              { sequence.next, type.pipeline.Base },
                              { coding.input, type.pipeline.Base },
                              { coding.condition, type.pipeline.Base },
                              { coding.statement, type.op.Base } });
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
