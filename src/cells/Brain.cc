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

Types::Types(brain::Brain& kb) :
    Type_(kb, "Type"),
    Slot(kb, "Slot"),
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
    op(kb),
    pipeline(kb),
    m_kb(kb)
{
}

Type& Types::ListOf(Type& type)
{
    auto numberIt = m_listTypes.find(&type);
    if (numberIt != m_listTypes.end()) {
        return numberIt->second.listType();
    } else {
        auto it = m_listTypes.emplace(std::piecewise_construct,
                                    std::forward_as_tuple(&type),
                                    std::forward_as_tuple(m_kb, type));
        return it.first->second.listType();
    }
}

Cells::Cells(brain::Brain& kb, Type& voidType, Type& anyType) :
    type(kb, anyType),
    slots(kb, anyType),
    slotType(kb, anyType),
    slotName(kb, anyType),
    slotRole(kb, anyType),
    emptyObject(kb, voidType)
{
}

Coding::Coding(brain::Brain& kb, Type& anyType) :
    argument(kb, anyType),
    branch(kb, anyType),
    cell(kb, anyType),
    condition(kb, anyType),
    else_(kb, anyType),
    input(kb, anyType),
    name(kb, anyType),
    objectType(kb, anyType),
    op(kb, anyType),
    output(kb, anyType),
    result(kb, anyType),
    role(kb, anyType),
    statement(kb, anyType),
    then(kb, anyType),
    value(kb, anyType)
{
}

Sequence::Sequence(brain::Brain& kb, Type& anyType) :
    first(kb, anyType),
    last(kb, anyType),
    previous(kb, anyType),
    next(kb, anyType),
    current(kb, anyType)
{
}

Equation::Equation(brain::Brain& kb, Type& anyType) :
    lhs(kb, anyType),
    rhs(kb, anyType)
{
}

Directions::Directions(brain::Brain& kb, Type& anyType) :
    up(kb, anyType),
    down(kb, anyType),
    left(kb, anyType),
    right(kb, anyType)
{
}

Coordinates::Coordinates(brain::Brain& kb, Type& anyType) :
    x(kb, anyType),
    y(kb, anyType)
{
}

Colors::Colors(brain::Brain& kb, Type& anyType) :
    red(kb, anyType),
    green(kb, anyType),
    blue(kb, anyType)
{
}

Boolean::Boolean(brain::Brain& kb) :
    true_(kb, "true_", kb.type.Boolean),
    false_(kb, "false_", kb.type.Boolean)
{
}

Dimensions::Dimensions(brain::Brain& kb, Type& anyType) :
    width(kb, anyType),
    height(kb, anyType),
    size(kb, anyType)
{
}

Visualization::Visualization(brain::Brain& kb, Type& anyType) :
    color(kb, anyType),
    pixels(kb, anyType)
{
}

Numbers::Numbers(brain::Brain& kb, Type& anyType) :
    sign(kb, anyType),
    positive(kb, anyType),
    negative(kb, anyType)
{
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
                             std::forward_as_tuple(m_kb, characterName, m_charType));
    }
}

// ============================================================================
Digits::Digits(brain::Brain& kb, Type& digit)
{
    for (int i = 0; i < 10; ++i) {
        std::string digitName = "Digit_" + std::to_string(i);
        m_digits.emplace_back(kb, digitName, digit);
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
        return it.first->second;
    }
}

} // namespace pools

Pools::Pools(brain::Brain& kb, Type& charType, Object& emptyObject, Type& digit) :
    chars(kb, charType, emptyObject),
    digits(kb, digit),
    numbers(kb)
{
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
    numbers(*this, type.Any)
{
    type.Type_.addSlots(
        { { "type", type.Type_, cells.type },
          { "slots", type.ListOf(type.Slot), cells.slots } });

    type.Slot.addSlots(
        { { "slotType", type.Type_, cells.slotType },
          { "slotName", type.String, cells.slotName },
          { "slotRole", type.Any, cells.slotRole } });

    type.Number.addSlots(
        { { "value", type.ListOf(type.Digit), coding.value },
          { "sign", type.Number, numbers.sign } }); // TODO

    type.String.addSlots({ { "value", type.ListOf(type.Char), coding.value } });

    type.Color.addSlots(
        { { "red", type.Number, colors.red },
          { "green", type.Number, colors.green },
          { "blue", type.Number, colors.blue } });

    type.Pixel.addSlots(
        { { "up", type.Pixel, directions.up },
          { "down", type.Pixel, directions.down },
          { "left", type.Pixel, directions.left },
          { "right", type.Pixel, directions.right },
          { "x", type.Number, coordinates.x },
          { "y", type.Number, coordinates.y } });

    type.Picture.addSlots(
        { { "width", type.Number, dimensions.width },
          { "height", type.Number, dimensions.height },
          { "pixels", type.ListOf(type.Pixel), visualization.pixels } });

    type.op.Same.addSlots({ { "lhs", type.pipeline.Base, equation.lhs },
                            { "rhs", type.pipeline.Base, equation.rhs },
                            { "output", type.pipeline.Base, coding.output } });

    type.op.NotSame.addSlots({ { "lhs", type.pipeline.Base, equation.lhs },
                               { "rhs", type.pipeline.Base, equation.rhs },
                               { "output", type.pipeline.Base, coding.output } });

    type.op.Equal.addSlots({ { "lhs", type.pipeline.Base, equation.lhs },
                             { "rhs", type.pipeline.Base, equation.rhs },
                             { "output", type.pipeline.Base, coding.output } });

    type.op.NotEqual.addSlots({ { "lhs", type.pipeline.Base, equation.lhs },
                                { "rhs", type.pipeline.Base, equation.rhs },
                                { "output", type.pipeline.Base, coding.output } });

    type.op.Has.addSlots({ { "cell", type.pipeline.Base, coding.cell },
                           { "role", type.pipeline.Base, coding.role },
                           { "ooutput", type.op.Base, coding.output } });

    type.op.Get.addSlots({ { "cell", type.pipeline.Base, coding.cell },
                           { "role", type.pipeline.Base, coding.role },
                           { "ooutput", type.op.Base, coding.output } });

    type.op.Set.addSlots({ { "cell", type.pipeline.Base, coding.cell },
                           { "role", type.pipeline.Base, coding.role },
                           { "ooutput", type.op.Base, coding.output },
                           { "value", type.pipeline.Base, coding.value } });

    type.op.logic.And.addSlots({ { "lhs", type.pipeline.Base, equation.lhs },
                                 { "rhs", type.pipeline.Base, equation.rhs },
                                 { "output", type.pipeline.Base, coding.output } });

    type.op.logic.Or.addSlots({ { "lhs", type.pipeline.Base, equation.lhs },
                                { "rhs", type.pipeline.Base, equation.rhs },
                                { "output", type.pipeline.Base, coding.output } });

    type.op.logic.Not.addSlots({ { "input", type.pipeline.Base, coding.input },
                                 { "output", type.pipeline.Base, coding.output } });

    type.op.math.Add.addSlots({ { "lhs", type.pipeline.Base, equation.lhs },
                                { "rhs", type.pipeline.Base, equation.rhs },
                                { "output", type.pipeline.Base, coding.output } });

    type.op.math.Subtract.addSlots({ { "lhs", type.pipeline.Base, equation.lhs },
                                     { "rhs", type.pipeline.Base, equation.rhs },
                                     { "output", type.pipeline.Base, coding.output } });

    type.op.math.Multiply.addSlots({ { "lhs", type.pipeline.Base, equation.lhs },
                                     { "rhs", type.pipeline.Base, equation.rhs },
                                     { "output", type.pipeline.Base, coding.output } });

    type.op.math.Divide.addSlots({ { "lhs", type.pipeline.Base, equation.lhs },
                                   { "rhs", type.pipeline.Base, equation.rhs },
                                   { "output", type.pipeline.Base, coding.output } });

    type.op.math.LessThan.addSlots({ { "lhs", type.pipeline.Base, equation.lhs },
                                     { "rhs", type.pipeline.Base, equation.rhs },
                                     { "output", type.pipeline.Base, coding.output } });

    type.op.math.GreaterThan.addSlots({ { "lhs", type.pipeline.Base, equation.lhs },
                                        { "rhs", type.pipeline.Base, equation.rhs },
                                        { "output", type.pipeline.Base, coding.output } });

    type.pipeline.Void.addSlots({ { "first", type.pipeline.Base, sequence.first },
                                  { "next", type.pipeline.Base, sequence.next },
                                  { "current", type.pipeline.Base, sequence.current } });

    type.pipeline.Input.addSlots({ { "first", type.pipeline.Base, sequence.first },
                                   { "next", type.pipeline.Base, sequence.next },
                                   { "current", type.pipeline.Base, sequence.current },
                                   { "value", type.Any, coding.value } });

    type.pipeline.New.addSlots({ { "first", type.pipeline.Base, sequence.first },
                                 { "next", type.pipeline.Base, sequence.next },
                                 { "current", type.pipeline.Base, sequence.current },
                                 { "value", type.pipeline.Base, coding.value },
                                 { "objectType", type.op.Base, coding.objectType } });

    type.pipeline.Fork.addSlots({ { "first", type.pipeline.Base, sequence.first },
                                  { "next", type.pipeline.Base, sequence.next },
                                  { "input", type.pipeline.Base, coding.input },
                                  { "value", type.pipeline.Base, coding.value },
                                  { "branch", type.op.Base, coding.branch } });

    type.pipeline.Delete.addSlots({ { "first", type.pipeline.Base, sequence.first },
                                    { "input", type.pipeline.Base, coding.input } });

    type.pipeline.Node.addSlots({ { "first", type.pipeline.Base, sequence.first },
                                  { "next", type.pipeline.Base, sequence.next },
                                  { "input", type.pipeline.Base, coding.input },
                                  { "op", type.op.Base, coding.op },
                                  { "value", type.pipeline.Base, coding.value } });

    type.pipeline.IfThen.addSlots({ { "first", type.pipeline.Base, sequence.first },
                                    { "next", type.pipeline.Base, sequence.next },
                                    { "input", type.pipeline.Base, coding.input },
                                    { "condition", type.pipeline.Base, coding.condition },
                                    { "then", type.op.Base, coding.then },
                                    { "else", type.op.Base, coding.else_ } });

    type.pipeline.DoWhile.addSlots({ { "first", type.pipeline.Base, sequence.first },
                                     { "next", type.pipeline.Base, sequence.next },
                                     { "input", type.pipeline.Base, coding.input },
                                     { "condition", type.pipeline.Base, coding.condition },
                                     { "statement", type.op.Base, coding.statement } });

    type.pipeline.While.addSlots({ { "first", type.pipeline.Base, sequence.first },
                                   { "next", type.pipeline.Base, sequence.next },
                                   { "input", type.pipeline.Base, coding.input },
                                   { "condition", type.pipeline.Base, coding.condition },
                                   { "statement", type.op.Base, coding.statement } });
}

CellI& Brain::toKbBool(bool value)
{
    return value ? boolean.true_ : boolean.false_;
}

} // namespace brain
} // namespace cells
} // namespace synth
