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

TypeInit::TypeInit(Types& types)
{
    // This is the first type, breaking the infinite init loop here with pre initializing a List<Slot> and ListItem<Slot> here
    // Every type has a list of Slots, so it needs a List<Slot>
    Brain& kb       = types.kb;
    Type& type      = types.kb.type.Slot;
    auto listItemIt = types.m_listItemTypes.emplace(std::piecewise_construct,
                                                    std::forward_as_tuple(&type),
                                                    std::forward_as_tuple(kb, "ListItem<Slot>", true));

    Type& itemType = listItemIt.first->second;
    itemType.add({ { kb.sequence.previous, itemType },
                   { kb.sequence.next, itemType },
                   { kb.coding.value, type } });

    auto listIt    = types.m_listTypes.emplace(std::piecewise_construct,
                                               std::forward_as_tuple(&type),
                                               std::forward_as_tuple(kb, "List<Slot>", true));
    Type& listType = listIt.first->second;
    listType.add({ { kb.sequence.first, itemType },
                   { kb.sequence.last, itemType },
                   { kb.dimensions.size, kb.type.Number } });
    itemType.manualInit();
    listType.manualInit();
}

Types::Types(brain::Brain& kb) :
    m_init(*this),
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
    kb(kb)
{
}

Type& Types::ListOf(CellI& type)
{
    auto numberIt = m_listTypes.find(&type);
    if (numberIt != m_listTypes.end()) {
        return numberIt->second;
    } else {
        std::string typeName = std::format("List<{}>", type.label());
        auto it              = m_listTypes.emplace(std::piecewise_construct,
                                             std::forward_as_tuple(&type),
                                             std::forward_as_tuple(kb, typeName));
        Type& listType = it.first->second;
        Type& itemType = ListItemOf(type);
        listType.add({ { kb.sequence.first, itemType },
                       { kb.sequence.last, itemType },
                       { kb.dimensions.size, kb.type.Number } });

        return it.first->second;
    }
}

Type& Types::ListItemOf(CellI& type)
{
#if 0
    Template ListItemTemplate(kb, "template<T> ListItem",
        { { kb.coding.objectType, kb.type.Type_ } },
        { { kb.coding.value, kb.coding.params, kb.coding.objectType } });

    ListItemTemplate.add({ { kb.sequence.previous, ListItemTemplate },
                           { kb.sequence.next, ListItemTemplate } });

    ListItemTemplate[kb.type.Type_][kb.type.template_];

    ListItemTemplateParam1[kb.coding.role] = kb.coding.objectType;
    ListItemTemplateParam1[kb.coding.type] = kb.type.Type_;
    ListItemTemplate[kb.coding.params]  = { ListItemTemplateParam1 };

    ListItemTemplateSlot1[kb.coding.role]                 = kb.coding.value;
    ListItemTemplateSlot1[kb.coding.template_.slotType]  = kb.coding.template_.slot.param; // one of { type, param, templateOf }
    ListItemTemplateSlot1[kb.coding.template_.param] = kb.coding.objectType;

    ListItemTemplateSlot2[kb.coding.template_.slotRole]   = kb.sequence.previous;
    ListItemTemplateSlot2[kb.coding.template_.slotType]   = kb.coding.template_.slot.templateOf; // one of { type, param, templateOf, selfType }
    ListItemTemplateSlot2[kb.coding.template_.templateOf] = ListItemTemplate;
    ListItemTemplateSlot2[kb.coding.template_.param]      = kb.coding.objectType;

    ListItemTemplateSlot3[kb.coding.template_.slotRole]   = kb.sequence.next;
    ListItemTemplateSlot3[kb.coding.template_.slotType]   = kb.coding.template_.slot.selfType; // one of { type, param, templateOf, selfType }

    ListItemTemplate[kb.coding.slots] = { ListItemTemplateSlot1, ListItemTemplateSlot2, ListItemTemplateSlot3 };

    TemplateInstance listItemT(ListItemTemplate, kb.coding.objectType, type);

 // Type ListItemTemplate(kb, "template<T> ListItem", { { kb.coding.parameters, kb.type.Type_ } });
#endif
    auto listItemIt = m_listItemTypes.find(&type);
    if (listItemIt != m_listItemTypes.end()) {
        return listItemIt->second;
    } else {
        std::string typeName = std::format("ListItem<{}>", type.label());
        auto it = m_listItemTypes.emplace(std::piecewise_construct,
                                          std::forward_as_tuple(&type),
                                          std::forward_as_tuple(kb, typeName));

        Type& itemType = it.first->second;
        itemType.add({ { kb.sequence.previous, itemType },
                       { kb.sequence.next, itemType },
                       { kb.coding.value, type } });
        return itemType;
    }
}

Cells::Cells(brain::Brain& kb, Type& voidType, Type& anyType) :
    type(kb, anyType, "type"),
    slotList(kb, anyType, "slotList"),
    slotMap(kb, anyType, "slotMap"),
    slotType(kb, anyType, "slotType"),
    slotRole(kb, anyType, "slotRole"),
    emptyObject(kb, voidType)
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
    result(kb, anyType, "result"),
    role(kb, anyType, "role"),
    statement(kb, anyType, "statement"),
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

Arc::Arc(brain::Brain& kb) :
    Demonstration(kb, "Demonstration"),
    Task(kb, "Task"),
    examples(kb, kb.type.Any)
{
    Demonstration.add(
        { { kb.coding.input, kb.type.Picture },
          { kb.coding.output, kb.type.Picture } });

    Task.add(
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
    type.Type_.add(
        { { cells.slotList, type.ListOf(type.Slot) } });

    type.Slot.add(
        { { cells.slotType, type.Type_ },
          { cells.slotRole, type.Any } });

    type.Number.add(
        { { coding.value, type.ListOf(type.Digit) },
          { numbers.sign, type.Number } }); // TODO

    type.String.add({ { coding.value, type.ListOf(type.Char) } });

    type.Color.add(
        { { colors.red, type.Number },
          { colors.green, type.Number },
          { colors.blue, type.Number } });

    type.Pixel.add(
        { { directions.up, type.Pixel },
          { directions.down, type.Pixel },
          { directions.left, type.Pixel },
          { directions.right, type.Pixel },
          { coordinates.x, type.Number },
          { coordinates.y, type.Number } });

    type.Picture.add(
        { { dimensions.width, type.Number },
          { dimensions.height, type.Number },
          { visualization.pixels, type.ListOf(type.Pixel) } });

    type.op.Same.add({ { equation.lhs, type.pipeline.Base },
                       { equation.rhs, type.pipeline.Base },
                       { coding.output, type.pipeline.Base } });

    type.op.NotSame.add({ { equation.lhs, type.pipeline.Base },
                          { equation.rhs, type.pipeline.Base },
                          { coding.output, type.pipeline.Base } });

    type.op.Equal.add({ { equation.lhs, type.pipeline.Base },
                        { equation.rhs, type.pipeline.Base },
                        { coding.output, type.pipeline.Base } });

    type.op.NotEqual.add({ { equation.lhs, type.pipeline.Base },
                           { equation.rhs, type.pipeline.Base },
                           { coding.output, type.pipeline.Base } });

    type.op.Has.add({ { coding.cell, type.pipeline.Base },
                      { coding.role, type.pipeline.Base },
                      { coding.output, type.op.Base } });

    type.op.Get.add({ { coding.cell, type.pipeline.Base },
                      { coding.role, type.pipeline.Base },
                      { coding.output, type.op.Base } });

    type.op.Set.add({ { coding.cell, type.pipeline.Base },
                      { coding.role, type.pipeline.Base },
                      { coding.output, type.op.Base },
                      { coding.value, type.pipeline.Base } });

    type.op.logic.And.add({ { equation.lhs, type.pipeline.Base },
                            { equation.rhs, type.pipeline.Base },
                            { coding.output, type.pipeline.Base } });

    type.op.logic.Or.add({ { equation.lhs, type.pipeline.Base },
                           { equation.rhs, type.pipeline.Base },
                           { coding.output, type.pipeline.Base } });

    type.op.logic.Not.add({ { coding.input, type.pipeline.Base },
                            { coding.output, type.pipeline.Base } });

    type.op.math.Add.add({ { equation.lhs, type.pipeline.Base },
                           { equation.rhs, type.pipeline.Base },
                           { coding.output, type.pipeline.Base } });

    type.op.math.Subtract.add({ { equation.lhs, type.pipeline.Base },
                                { equation.rhs, type.pipeline.Base },
                                { coding.output, type.pipeline.Base } });

    type.op.math.Multiply.add({ { equation.lhs, type.pipeline.Base },
                                { equation.rhs, type.pipeline.Base },
                                { coding.output, type.pipeline.Base } });

    type.op.math.Divide.add({ { equation.lhs, type.pipeline.Base },
                              { equation.rhs, type.pipeline.Base },
                              { coding.output, type.pipeline.Base } });

    type.op.math.LessThan.add({ { equation.lhs, type.pipeline.Base },
                                { equation.rhs, type.pipeline.Base },
                                { coding.output, type.pipeline.Base } });

    type.op.math.GreaterThan.add({ { equation.lhs, type.pipeline.Base },
                                   { equation.rhs, type.pipeline.Base },
                                   { coding.output, type.pipeline.Base } });

    type.pipeline.Void.add({ { sequence.first, type.pipeline.Base },
                             { sequence.next, type.pipeline.Base },
                             { sequence.current, type.pipeline.Base } });

    type.pipeline.Input.add({ { sequence.first, type.pipeline.Base },
                              { sequence.next, type.pipeline.Base },
                              { sequence.current, type.pipeline.Base },
                              { coding.value, type.Any } });

    type.pipeline.New.add({ { sequence.first, type.pipeline.Base },
                            { sequence.next, type.pipeline.Base },
                            { sequence.current, type.pipeline.Base },
                            { coding.value, type.pipeline.Base },
                            { coding.objectType, type.op.Base } });

    type.pipeline.Fork.add({ { sequence.first, type.pipeline.Base },
                             { sequence.next, type.pipeline.Base },
                             { coding.input, type.pipeline.Base },
                             { coding.value, type.pipeline.Base },
                             { coding.branch, type.op.Base } });

    type.pipeline.Delete.add({ { sequence.first, type.pipeline.Base },
                               { coding.input, type.pipeline.Base } });

    type.pipeline.Node.add({ { sequence.first, type.pipeline.Base },
                             { sequence.next, type.pipeline.Base },
                             { coding.input, type.pipeline.Base },
                             { coding.op, type.op.Base },
                             { coding.value, type.pipeline.Base } });

    type.pipeline.IfThen.add({ { sequence.first, type.pipeline.Base },
                               { sequence.next, type.pipeline.Base },
                               { coding.input, type.pipeline.Base },
                               { coding.condition, type.pipeline.Base },
                               { coding.then, type.op.Base },
                               { coding.else_, type.op.Base } });

    type.pipeline.DoWhile.add({ { sequence.first, type.pipeline.Base },
                                { sequence.next, type.pipeline.Base },
                                { coding.input, type.pipeline.Base },
                                { coding.condition, type.pipeline.Base },
                                { coding.statement, type.op.Base } });

    type.pipeline.While.add({ { sequence.first, type.pipeline.Base },
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
