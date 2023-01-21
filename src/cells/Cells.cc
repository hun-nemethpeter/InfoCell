#include "Cells.h"

#include <sstream>

#pragma warning(disable : 4996)
#include <utf8.h>

namespace synth {
namespace cells {

CellI::CellI(brain::Brain& kb) :
    kb(kb)
{
}

bool CellI::operator==(CellI& rhs)
{
    if (&type() != &rhs.type()) {
        return false;
    }
    for (auto& slot : type().slots()) {
        CellI& role      = slot.second.slotRole();
        bool hasLeftSlot = has(role);
        if (hasLeftSlot != rhs.has(role)) {
            return false;
        }
        if (!hasLeftSlot) {
            continue;
        }
        if (&(*this)[role] != &rhs[role]) {
            return false;
        }
    }

    return true;
}

bool CellI::operator!=(CellI& rhs)
{
    return !((*this) == rhs);
}

// ============================================================================
SlotRef::SlotRef(const std::string& name, Type& type, CellI& role) :
    m_name(name), m_type(type), m_role(role)
{
}

Slot::Slot(brain::Brain& kb, const std::string& name, Type& type, CellI& role) :
    CellI(kb),
    m_name(name), m_slotType(type), m_slotRole(role)
{
}

bool Slot::has(CellI& role)
{
    if (&role == &kb.cells.type || &role == &kb.cells.slotType || &role == &kb.cells.slotName || &role == &kb.cells.slotRole) {
        return true;
    }
    return false;
}

void Slot::set(CellI& role, CellI& value)
{
    throw "Not supported";
}
void Slot::operator()()
{
    // Do nothing, this is a data cell
}

CellI& Slot::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return type();
    }
    if (&role == &kb.cells.slotType) {
        return m_slotType;
    }
    if (&role == &kb.cells.slotName) {
        if (!m_slotNameString) {
            m_slotNameString.reset(new String(kb, m_name));
        }
        return *m_slotNameString;
    }
    if (&role == &kb.cells.slotRole) {
        return m_slotRole;
    }

    return kb.cells.emptyObject;
}

Type& Slot::type()
{
    return kb.type.Slot;
}

void Slot::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string Slot::name() const
{
    return m_name;
}

Type& Slot::slotType()
{
    return m_slotType;
}

CellI& Slot::slotRole()
{
    return m_slotRole;
}

// ============================================================================
Type::Type(brain::Brain& kb, const std::string& name) :
    CellI(kb),
    m_name(name)
{
    createSlot("type", kb.type.Type_, kb.cells.type);
}

Type::Type(brain::Brain& kb, const std::string& name, std::initializer_list<SlotRef> slots) :
    CellI(kb),
    m_name(name)
{
    createSlot("type", kb.type.Type_, kb.cells.type);
    for (const SlotRef& slotRef : slots) {
        createSlot(slotRef.m_name, slotRef.m_type, slotRef.m_role);
    }
}

bool Type::has(CellI& role)
{
    if (&role == &kb.cells.type || &role == &kb.cells.slots) {
        return true;
    }

    return false;
}

void Type::set(CellI& role, CellI& value)
{
    throw "Not supported";
}

void Type::operator()()
{
    // Do nothing, this is a data cell
}

CellI& Type::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return type();
    }
    if (&role == &kb.cells.slots) {
        if (!m_slotsList)
            m_slotsList.reset(new List(kb, m_slots));

        return *m_slotsList;
    }

    return kb.cells.emptyObject;
}

Type& Type::type()
{
    return kb.type.Type_;
}

void Type::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string Type::name() const
{
    return m_name;
}

void Type::addSlots(std::initializer_list<SlotRef> slots)
{
    for (const SlotRef& slotRef : slots) {
        createSlot(slotRef.m_name, slotRef.m_type, slotRef.m_role);
    }
}

Slot& Type::createSlot(const std::string& name, Type& type, CellI& role)
{
    auto slotIt = m_slots.find(name);
    if (slotIt != m_slots.end()) {
        if (&slotIt->second.slotType() != &type) {
            throw "Member name already registered with an other class";
        }
        return slotIt->second;
    } else {
        auto it = m_slots.emplace(std::piecewise_construct,
                                  std::forward_as_tuple(name),
                                  std::forward_as_tuple(kb, name, type, role));

        Slot& slot     = it.first->second;
        m_roles[&role] = &slot;

        return slot;
    }
}

bool Type::has(const std::string& name) const
{
    return m_slots.find(name) != m_slots.end();
}

bool Type::hasSlot(CellI& role)
{
    return m_roles.find(&role) != m_roles.end();
}

bool Type::hasSlot(const std::string& name)
{
    return m_slots.find(name) != m_slots.end();
}

Slot& Type::getSlot(CellI& role)
{
    auto findIt = m_roles.find(&role);
    if (findIt == m_roles.end())
        throw "emptyMember";

    return *findIt->second;
}

Slot& Type::getSlot(const std::string& name)
{
    auto findIt = m_slots.find(name);
    if (findIt == m_slots.end())
        throw "emptyMember";

    return findIt->second;
}

std::map<std::string, Slot>& Type::slots()
{
    return m_slots;
}

// ============================================================================
Object::Object(brain::Brain& kb, Type& type) :
    Object(kb, "", type)
{
}

Object::Object(brain::Brain& kb, const std::string& name, Type& type) :
    CellI(kb),
    m_name(name), m_type(type)
{
    m_roles[&kb.cells.type] = &type;
}

bool Object::has(CellI& role)
{
    if (&role == &kb.cells.type)
        return true;

    return m_roles.find(&role) != m_roles.end();
}

void Object::set(CellI& role, CellI& value)
{
    if (&role == &kb.cells.type) {
        throw "Type change not allowed.";
    }

    if (type().hasSlot(role)) {
        m_roles[&role] = &value;
    } else {
        throw "The type doesn't contains this role.";
    }
}

void Object::operator()()
{
    // Do nothing, this is a data cell
}

CellI& Object::operator[](CellI& role)
{
    auto findIt = m_roles.find(&role);
    if (findIt == m_roles.end())
        return kb.cells.emptyObject;

    return *findIt->second;
}

Type& Object::type()
{
    return m_type;
}

void Object::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string Object::name() const
{
    return m_name;
}

std::map<CellI*, CellI*>& Object::roles()
{
    return m_roles;
}

// ============================================================================
ListItem::ListItem(brain::Brain& kb, Type& t) :
    CellI(kb),
    m_type(t)
{
}

bool ListItem::has(CellI& role)
{
    if (&role == &kb.cells.type || &role == &kb.sequence.previous || &role == &kb.sequence.next || &role == &kb.coding.value) {
        return true;
    }

    return false;
}

void ListItem::set(CellI& role, CellI& value)
{
    throw "Not supported";
}

void ListItem::operator()()
{
    // Do nothing, this is a data cell
}

CellI& ListItem::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return m_type;
    }
    if (&role == &kb.sequence.previous) {
        if (m_prev)
            return *m_prev;
        else
            return kb.cells.emptyObject;
    }
    if (&role == &kb.sequence.next) {
        if (m_next)
            return *m_next;
        else
            return kb.cells.emptyObject;
    }
    if (&role == &kb.coding.value) {
        if (m_value)
            return *m_value;
        else
            return *m_value;
    }

    return kb.cells.emptyObject;
}

Type& ListItem::type()
{
    return m_type;
}

void ListItem::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string ListItem::name() const
{
    std::stringstream ss;
    if (m_value) {
        ss << "\"" << m_value->name() << "\"";
    }
    return ss.str();
}

CellI& ListItem::prev()
{
    return *m_prev;
}

void ListItem::prev(ListItem* p)
{
    m_prev = p;
}

CellI& ListItem::next()
{
    return *m_next;
}

void ListItem::next(ListItem* n)
{
    m_next = n;
}

CellI& ListItem::value()
{
    return *m_value;
}

void ListItem::value(CellI* v)
{
    m_value = v;
}

// ============================================================================
template <typename T>
T* ptr(T& obj) { return &obj; }

template <typename T>
T* ptr(T* obj) { return obj; }

List::List(brain::Brain& kb, Type& valueType) :
    CellI(kb),
    m_valueType(valueType),
    m_listType(kb, "List<T>"),
    m_itemType(kb, "ListItem<T>")
{
    m_itemType.addSlots({ { "prev", m_itemType, kb.sequence.previous },
                          { "next", m_itemType, kb.sequence.next },
                          { "value", valueType, kb.coding.value } });
    m_listType.addSlots({
        { "first", m_itemType, kb.sequence.first },
        { "last", m_itemType, kb.sequence.last },
        { "size", kb.type.Number, kb.dimensions.size } });
}

template <typename T>
List::List(brain::Brain& kb, std::vector<T>& values) :
    List(kb, ptr(values.front())->type())
{
    for (auto& valueT : values) {
        add(*ptr(valueT));
    }
}

template <typename T>
List::List(brain::Brain& kb, std::map<std::string, T>& values) :
    List(kb, (*values.begin()).second.type())
{
    for (auto& valuePairs : values) {
        add(valuePairs.second);
    }
}

bool List::has(CellI& role)
{
    if (&role == &kb.cells.type || &role == &kb.sequence.first || &role == &kb.sequence.last || &role == &kb.dimensions.size) {
        return true;
    }
    return false;
}

void List::set(CellI& role, CellI& value)
{
    throw "Not supported";
}

void List::operator()()
{
    // Do nothing, this is a data cell
}

CellI& List::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return m_listType;
    }
    if (&role == &kb.sequence.first) {
        if (m_items.empty()) {
            return kb.cells.emptyObject;
        }
        return m_items.front();
    }
    if (&role == &kb.sequence.last) {
        if (m_items.empty()) {
            return kb.cells.emptyObject;
        }
        return m_items.back();
    }
    if (&role == &kb.dimensions.size) {
        Number* number = new Number(kb, (int)m_items.size());
        return *number;
    }

    return kb.cells.emptyObject;
}

Type& List::type()
{
    return m_listType;
}

void List::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string List::name() const
{
    std::stringstream ss;
    ss << "List<>" << std::endl;

    return ss.str();
}

std::list<ListItem>& List::items()
{
    return m_items;
}

void List::add(CellI& value)
{
//    if (m_valueType != value.type()) {
//        return;
//    }

    ListItem* prevListItem = m_items.empty() ? nullptr : &m_items.back();
    auto& listItemCell     = m_items.emplace_back(kb, m_itemType);
    if (prevListItem == nullptr) {
        listItemCell.prev(nullptr);
    } else {
        listItemCell.prev(prevListItem);
        prevListItem->next(&listItemCell);
    }
    listItemCell.next(nullptr);
    listItemCell.value(&value);
}

Type& List::valueType()
{
    return m_valueType;
}

Type& List::listType()
{
    return m_listType;
}

Type& List::itemType()
{
    return m_itemType;
}

// ============================================================================
Number::Number(brain::Brain& kb, int value) :
    CellI(kb),
    m_value(value)
{
}

bool Number::has(CellI& role)
{
    if (&role == &kb.cells.type || &role == &kb.coding.value || &role == &kb.numbers.sign) {
        return true;
    }
    return false;
}

void Number::set(CellI& role, CellI& value)
{
    throw "Not supported";
}

void Number::operator()()
{
    // Do nothing, this is a data cell
}

CellI& Number::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return kb.type.Number;
    }

    if (&role == &kb.numbers.sign) {
        return kb.cells.emptyObject; // TODO
    }

    if (&role == &kb.coding.value) {
        if (m_digits.empty()) {
            calculateDigits();
            m_digitsList.reset(new List(kb, m_digits));
        }

        return *m_digitsList;
    }

    return kb.cells.emptyObject;
}

Type& Number::type()
{
    return kb.type.Number;
}

void Number::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string Number::name() const
{
    return std::to_string(m_value);
}

int Number::value() const
{
    return m_value;
}

void Number::calculateDigits()
{
    if (m_value == 0) {
        m_digits.push_back(&kb.pools.digits[0]);
        return;
    }
    int value = m_value;
    while (value) {
        m_digits.push_back(&kb.pools.digits[value % 10]);
        value /= 10;
    }
    std::reverse(m_digits.begin(), m_digits.end());
}

// ============================================================================
String::String(brain::Brain& kb, const std::string& str) :
    CellI(kb),
    m_value(str)
{
}

bool String::has(CellI& role)
{
    if (&role == &kb.cells.type || &role == &kb.coding.value) {
        return true;
    }
    return false;
}

void String::set(CellI& role, CellI& value)
{
    throw "Not supported";
}

void String::operator()()
{
    // Do nothing, this is a data cell
}

CellI& String::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return type();
    } else if (&role == &kb.coding.value) {
        if (m_characters.empty()) {
            calculateCharacters();
            m_charactersList.reset(new List(kb, m_characters));
        }

        return *m_charactersList;
    } else {
        return kb.cells.emptyObject;
    }
}

Type& String::type()
{
    return kb.type.String;
}

void String::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string String::name() const
{
    return m_value;
}

const std::string& String::value() const
{
    return m_value;
}

void String::calculateCharacters()
{
    utf8::iterator<const char*> valueIt(m_value.data(), m_value.data(), m_value.data() + m_value.size());
    utf8::iterator<const char*> valueEndIt(m_value.data() + m_value.size(), m_value.data(), m_value.data() + m_value.size());

    for (auto& it = valueIt; it != valueEndIt; ++valueIt) {
        char32_t unicodeValue = *it;
        m_characters.push_back(&kb.pools.chars.get(unicodeValue));
    }
}

namespace hybrid {
// ============================================================================
Color::Color(brain::Brain& kb, const input::Color& inputColor) :
    CellI(kb),
    m_inputColor(inputColor)
{
}

bool Color::has(CellI& role)
{
    if (&role == &kb.cells.type) {
        return true;
    }
    if (&role == &kb.colors.red || &role == &kb.colors.green || &role == &kb.colors.blue) {
        return true;
    }

    return false;
}

void Color::set(CellI& role, CellI& value)
{
}

void Color::operator()()
{
    // Do nothing
}

CellI& Color::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return type();
    }
    if (&role == &kb.colors.red) {
        return kb.pools.numbers.get(m_inputColor.m_red);
    }
    if (&role == &kb.colors.green) {
        return kb.pools.numbers.get(m_inputColor.m_green);
    }
    if (&role == &kb.colors.blue) {
        return kb.pools.numbers.get(m_inputColor.m_blue);
    }

    return kb.cells.emptyObject;
}

Type& Color::type()
{
    return kb.type.Color;
}

void Color::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string Color::name() const
{
    std::stringstream ss;
    ss << "Color(" << m_inputColor.m_red << ", " << m_inputColor.m_green << ", " << m_inputColor.m_blue << ")";
    return ss.str();
}

const input::Color& Color::color() const
{
    return m_inputColor;
}

// ============================================================================
Pixel::Pixel(brain::Brain& kb, int x, int y, const input::Color& inputColor) :
    CellI(kb),
    m_x(kb.pools.numbers.get(x)),
    m_y(kb.pools.numbers.get(y)),
    m_color(kb, inputColor),
    m_inputColor(inputColor)
{
}

bool Pixel::has(CellI& role)
{
    if (&role == &kb.cells.type) {
        return true;
    }
    if (&role == &kb.directions.up && m_up) {
        return true;
    }
    if (&role == &kb.directions.down && m_down) {
        return true;
    }
    if (&role == &kb.directions.left && m_left) {
        return true;
    }
    if (&role == &kb.directions.right && m_right) {
        return true;
    }
    if (&role == &kb.visualization.color) {
        return true;
    }
    if (&role == &kb.coordinates.x) {
        return true;
    }
    if (&role == &kb.coordinates.y) {
        return true;
    }

    return false;
}

void Pixel::set(CellI& role, CellI& value)
{
    throw "Setting a generated pixelRef cell is not possible";
}

void Pixel::operator()()
{
    // Do nothing
}

CellI& Pixel::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return type();
    }
    if (&role == &kb.directions.up && m_up) {
        return *m_up;
    }
    if (&role == &kb.directions.down && m_down) {
        return *m_down;
    }
    if (&role == &kb.directions.left && m_left) {
        return *m_left;
    }
    if (&role == &kb.directions.right && m_right) {
        return *m_right;
    }
    if (&role == &kb.visualization.color) {
        return m_color;
    }
    if (&role == &kb.coordinates.x) {
        return m_x;
    }
    if (&role == &kb.coordinates.y) {
        return m_y;
    }

    return kb.cells.emptyObject;
}

Type& Pixel::type()
{
    return kb.type.Pixel;
}

void Pixel::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string Pixel::name() const
{
    std::stringstream ss;
    ss << "Pixel[" << m_x.value() << ", " << m_y.value() << "](" << (int)m_inputColor.m_red << ", " << (int)m_inputColor.m_green << ", " << (int)m_inputColor.m_blue << ")";
    return ss.str();
}

const input::Color& Pixel::color() const
{
    return m_inputColor;
}

// ============================================================================
Picture::Picture(brain::Brain& kb, input::Picture& picture) :
    CellI(kb),
    m_name(picture.name()), m_width(picture.width()), m_height(picture.height()), m_widthCell(kb.pools.numbers.get(m_width)), m_heightCell(kb.pools.numbers.get(m_height))
{
    const int senzorSize = m_height * m_width;

    m_pixels.clear();
    m_pixels.reserve(senzorSize);

    int x = 0;
    int y = 0;

    for (const input::Color& pixel : picture.pixels()) {
        m_pixels.emplace_back(kb, x++, y, pixel);
        if (x == m_width) {
            x = 0;
            y += 1;
        }
    }

    for (y = 0; y < m_height; ++y) {
        for (x = 0; x < m_width; ++x) {
            Pixel& pixel  = m_pixels[currentIndex(x, y)];
            pixel.m_up    = upPixel(x, y);
            pixel.m_down  = downPixel(x, y);
            pixel.m_left  = leftPixel(x, y);
            pixel.m_right = rightPixel(x, y);
        }
    }
    m_pixelsList.reset(new List(kb, m_pixels));
}

bool Picture::has(CellI& role)
{
    if (&role == &kb.cells.type || &role == &kb.dimensions.width || &role == &kb.dimensions.height || &role == &kb.visualization.pixels) {
        return true;
    }

    return false;
}

void Picture::set(CellI& role, CellI& value)
{
    throw "Setting a generated Picture cell is not possible";
}

void Picture::operator()()
{
}

CellI& Picture::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return type();
    }
    if (&role == &kb.dimensions.width) {
        return m_widthCell;
    }
    if (&role == &kb.dimensions.height) {
        return m_heightCell;
    }
    if (&role == &kb.visualization.pixels) {
        return *m_pixelsList;
    }

    return kb.cells.emptyObject;
}

Type& Picture::type()
{
    return kb.type.Picture;
}

void Picture::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string Picture::name() const
{
    return m_name;
}

Pixel& Picture::getPixel(int x, int y)
{
    return m_pixels[currentIndex(x, y)];
}

const Pixel& Picture::getPixel(int x, int y) const
{
    return m_pixels.at(currentIndex(x, y));
}

int Picture::currentIndex(int x, int y) const
{
    return y * m_width + x;
}

bool Picture::isInRange(int x, int y) const
{
    if (y < 0 || x < 0 || x > m_width - 1 || y > m_height - 1) {
        return false;
    }

    return true;
}

/*
       5 x 5
m_width  = 5
m_height = 5

     0  1  2  3  4
 0  00 01 02 03 04
 1  05 06 07 08 09
 2  10 11 12 13 14
 3  15 16 17 18 19
 4  20 21 22 23 24

 upPixel(4, 4) = 19
 3 * 5 + 4

 */

Pixel* Picture::upPixel(int x, int y)
{
    if (!isInRange(x, y) || y == 0) {
        return nullptr;
    }
    int upIndex = (y - 1) * m_width + x;
    return &m_pixels[upIndex];
}

Pixel* Picture::downPixel(int x, int y)
{
    if (!isInRange(x, y) || y == m_height - 1) {
        return nullptr;
    } else {
        int downIndex = (y + 1) * m_width + x;
        return &m_pixels[downIndex];
    }
}

Pixel* Picture::leftPixel(int x, int y)
{
    if (!isInRange(x, y) || x == 0) {
        return nullptr;
    } else {
        return &m_pixels[y * m_width + x - 1];
    }
}

Pixel* Picture::rightPixel(int x, int y)
{
    if (!isInRange(x, y) || x == m_width - 1) {
        return nullptr;
    } else {
        return &m_pixels[y * m_width + x + 1];
    }
}

std::vector<Pixel>& Picture::pixels()
{
    return m_pixels;
}

int Picture::width() const
{
    return m_width;
}

int Picture::height() const
{
    return m_height;
}

} // namespace hybrid
namespace control {
namespace op {

// ============================================================================
Same::Same(brain::Brain& kb, pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs) :
    CellI(kb),
    m_output(output), m_lhs(lhs), m_rhs(rhs)
{
}

bool Same::has(CellI& role)
{
    if (&role == &kb.cells.type) {
        return true;
    }
    if (&role == &kb.equation.lhs) {
        return true;
    }
    if (&role == &kb.equation.rhs) {
        return true;
    }
    if (&role == &kb.coding.output) {
        return true;
    }

    return false;
}

void Same::set(CellI& role, CellI& value)
{
    // Editing a control node is not possible
}

void Same::operator()()
{
    CellI* lhs = &m_lhs[kb.coding.value];
    CellI* rhs = &m_rhs[kb.coding.value];
    m_output.set(kb.coding.value, kb.toKbBool(lhs == rhs));
}

CellI& Same::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return type();
    }
    if (&role == &kb.equation.lhs) {
        return m_lhs;
    }
    if (&role == &kb.equation.rhs) {
        return m_rhs;
    }
    if (&role == &kb.coding.output) {
        return m_output;
    }

    return kb.cells.emptyObject;
}

Type& Same::type()
{
    return kb.type.op.Same;
}

void Same::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string Same::name() const
{
    return "Same";
}

// ============================================================================
NotSame::NotSame(brain::Brain& kb, pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs) :
    CellI(kb),
    m_output(output), m_lhs(lhs), m_rhs(rhs)
{
}

bool NotSame::has(CellI& role)
{
    if (&role == &kb.cells.type) {
        return true;
    }
    if (&role == &kb.equation.lhs) {
        return true;
    }
    if (&role == &kb.equation.rhs) {
        return true;
    }
    if (&role == &kb.coding.output) {
        return true;
    }

    return false;
}

void NotSame::set(CellI& role, CellI& value)
{
    // Editing a control node is not possible
}

void NotSame::operator()()
{
    CellI* lhs = &m_lhs[kb.coding.value];
    CellI* rhs = &m_rhs[kb.coding.value];
    m_output.set(kb.coding.value, kb.toKbBool(lhs != rhs));
}

CellI& NotSame::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return type();
    }
    if (&role == &kb.equation.lhs) {
        return m_lhs;
    }
    if (&role == &kb.equation.rhs) {
        return m_rhs;
    }
    if (&role == &kb.coding.output) {
        return m_output;
    }

    return kb.cells.emptyObject;
}

Type& NotSame::type()
{
    return kb.type.op.NotSame;
}

void NotSame::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string NotSame::name() const
{
    return "NotSame";
}

// ============================================================================
Equal::Equal(brain::Brain& kb, pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs) :
    CellI(kb),
    m_output(output), m_lhs(lhs), m_rhs(rhs)
{
}

bool Equal::has(CellI& role)
{
    if (&role == &kb.cells.type) {
        return true;
    }
    if (&role == &kb.equation.lhs) {
        return true;
    }
    if (&role == &kb.equation.rhs) {
        return true;
    }
    if (&role == &kb.coding.output) {
        return true;
    }

    return false;
}

void Equal::set(CellI& role, CellI& value)
{
    // Editing a control node is not possible
}

void Equal::operator()()
{
    CellI& lhs = m_lhs[kb.coding.value];
    CellI& rhs = m_rhs[kb.coding.value];
    m_output.set(kb.coding.value, kb.toKbBool(lhs == rhs));
}

CellI& Equal::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return type();
    }
    if (&role == &kb.equation.lhs) {
        return m_lhs;
    }
    if (&role == &kb.equation.rhs) {
        return m_rhs;
    }
    if (&role == &kb.coding.output) {
        return m_output;
    }

    return kb.cells.emptyObject;
}

Type& Equal::type()
{
    return kb.type.op.Equal;
}

void Equal::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string Equal::name() const
{
    return "Equal";
}

// ============================================================================
NotEqual::NotEqual(brain::Brain& kb, pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs) :
    CellI(kb),
    m_output(output), m_lhs(lhs), m_rhs(rhs)
{
}

bool NotEqual::has(CellI& role)
{
    if (&role == &kb.cells.type) {
        return true;
    }
    if (&role == &kb.equation.lhs) {
        return true;
    }
    if (&role == &kb.equation.rhs) {
        return true;
    }
    if (&role == &kb.coding.output) {
        return true;
    }

    return false;
}

void NotEqual::set(CellI& role, CellI& value)
{
    // Editing a control node is not possible
}

void NotEqual::operator()()
{
    CellI& lhs = m_lhs[kb.coding.value];
    CellI& rhs = m_rhs[kb.coding.value];
    m_output.set(kb.coding.value, kb.toKbBool(lhs != rhs));
}

CellI& NotEqual::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return type();
    }
    if (&role == &kb.equation.lhs) {
        return m_lhs;
    }
    if (&role == &kb.equation.rhs) {
        return m_rhs;
    }
    if (&role == &kb.coding.output) {
        return m_output;
    }

    return kb.cells.emptyObject;
}

Type& NotEqual::type()
{
    return kb.type.op.NotEqual;
}

void NotEqual::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string NotEqual::name() const
{
    return "NotEqual";
}

// ============================================================================
Has::Has(brain::Brain& kb, pipeline::Base& output, pipeline::Base& cell, pipeline::Base& role) :
    CellI(kb),
    m_output(output), m_cell(cell), m_role(role)
{
}

bool Has::has(CellI& role)
{
    if (&role == &kb.cells.type) {
        return true;
    }
    if (&role == &kb.coding.cell) {
        return true;
    }
    if (&role == &kb.coding.role) {
        return true;
    }
    if (&role == &kb.coding.output) {
        return true;
    }

    return false;
}

void Has::set(CellI& role, CellI& value)
{
    // Editing a control node is not possible
}

void Has::operator()()
{
    CellI& cell = m_cell[kb.coding.value];
    CellI& role = m_role[kb.coding.value];
    m_output.set(kb.coding.value, kb.toKbBool(cell.has(role)));
}

CellI& Has::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return type();
    }
    if (&role == &kb.coding.cell) {
        return m_cell;
    }
    if (&role == &kb.coding.role) {
        return m_role;
    }
    if (&role == &kb.coding.output) {
        return m_output;
    }

    return kb.cells.emptyObject;
}

Type& Has::type()
{
    return kb.type.op.Has;
}

void Has::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string Has::name() const
{
    return "Has";
}

// ============================================================================
Get::Get(brain::Brain& kb, pipeline::Base& output, pipeline::Base& cell, pipeline::Base& role) :
    CellI(kb),
    m_output(output), m_cell(cell), m_role(role)
{
}

bool Get::has(CellI& role)
{
    if (&role == &kb.cells.type) {
        return true;
    }
    if (&role == &kb.coding.cell) {
        return true;
    }
    if (&role == &kb.coding.role) {
        return true;
    }
    if (&role == &kb.coding.output) {
        return true;
    }

    return false;
}

void Get::set(CellI& role, CellI& value)
{
    // Editing a control node is not possible
}

void Get::operator()()
{
    CellI& cell = m_cell[kb.coding.value];
    CellI& role = m_role[kb.coding.value];
    m_output.set(kb.coding.value, cell[role]);
}

CellI& Get::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return type();
    }
    if (&role == &kb.coding.cell) {
        return m_cell;
    }
    if (&role == &kb.coding.role) {
        return m_role;
    }
    if (&role == &kb.coding.output) {
        return m_output;
    }

    return kb.cells.emptyObject;
}

Type& Get::type()
{
    return kb.type.op.Get;
}

void Get::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string Get::name() const
{
    return "Get";
}

// ============================================================================
Set::Set(brain::Brain& kb, pipeline::Base& output, pipeline::Base& cell, pipeline::Base& role, pipeline::Base& value) :
    CellI(kb),
    m_output(output), m_cell(cell), m_role(role), m_value(value)
{
}

bool Set::has(CellI& role)
{
    if (&role == &kb.cells.type) {
        return true;
    }
    if (&role == &kb.coding.cell) {
        return true;
    }
    if (&role == &kb.coding.role) {
        return true;
    }
    if (&role == &kb.coding.value) {
        return true;
    }
    if (&role == &kb.coding.output) {
        return true;
    }

    return false;
}

void Set::set(CellI& role, CellI& value)
{
    // Editing a control node is not possible
}

void Set::operator()()
{
    CellI& cell  = m_cell[kb.coding.value];
    CellI& role  = m_role[kb.coding.value];
    CellI& value = m_value[kb.coding.value];

    cell.set(role, value);
}

CellI& Set::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return type();
    }
    if (&role == &kb.coding.cell) {
        return m_cell;
    }
    if (&role == &kb.coding.role) {
        return m_role;
    }
    if (&role == &kb.coding.value) {
        return m_value;
    }
    if (&role == &kb.coding.output) {
        return m_output;
    }

    return kb.cells.emptyObject;
}

Type& Set::type()
{
    return kb.type.op.Set;
}

void Set::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string Set::name() const
{
    return "Set";
}

namespace logic {

// ============================================================================
And::And(brain::Brain& kb, pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs) :
    CellI(kb),
    m_output(output), m_lhs(lhs), m_rhs(rhs)
{
}

bool And::has(CellI& role)
{
    if (&role == &kb.cells.type) {
        return true;
    }
    if (&role == &kb.equation.lhs) {
        return true;
    }
    if (&role == &kb.equation.rhs) {
        return true;
    }
    if (&role == &kb.coding.output) {
        return true;
    }

    return false;
}

void And::set(CellI& role, CellI& value)
{
    // Editing a control node is not possible
}

void And::operator()()
{
    bool lhs = m_lhs[kb.coding.value] == kb.boolean.true_;
    bool rhs = m_rhs[kb.coding.value] == kb.boolean.true_;
    m_output.set(kb.coding.value, kb.toKbBool(lhs && rhs));
}

CellI& And::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return type();
    }
    if (&role == &kb.equation.lhs) {
        return m_lhs;
    }
    if (&role == &kb.equation.rhs) {
        return m_rhs;
    }
    if (&role == &kb.coding.output) {
        return m_output;
    }

    return kb.cells.emptyObject;
}

Type& And::type()
{
    return kb.type.op.logic.And;
}

void And::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string And::name() const
{
    return "And";
}

// ============================================================================
Or::Or(brain::Brain& kb, pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs) :
    CellI(kb),
    m_output(output), m_lhs(lhs), m_rhs(rhs)
{
}

bool Or::has(CellI& role)
{
    if (&role == &kb.cells.type) {
        return true;
    }
    if (&role == &kb.equation.lhs) {
        return true;
    }
    if (&role == &kb.equation.rhs) {
        return true;
    }
    if (&role == &kb.coding.output) {
        return true;
    }

    return false;
}

void Or::set(CellI& role, CellI& value)
{
    // Editing a control node is not possible
}

void Or::operator()()
{
    bool lhs = m_lhs[kb.coding.value] == kb.boolean.true_;
    bool rhs = m_rhs[kb.coding.value] == kb.boolean.true_;
    m_output.set(kb.coding.value, kb.toKbBool(lhs || rhs));
}

CellI& Or::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return type();
    }
    if (&role == &kb.equation.lhs) {
        return m_lhs;
    }
    if (&role == &kb.equation.rhs) {
        return m_rhs;
    }
    if (&role == &kb.coding.output) {
        return m_output;
    }

    return kb.cells.emptyObject;
}

Type& Or::type()
{
    return kb.type.op.logic.Or;
}

void Or::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string Or::name() const
{
    return "Or";
}

// ============================================================================
Not::Not(brain::Brain& kb, pipeline::Base& output, pipeline::Base& input) :
    CellI(kb),
    m_output(output), m_input(input)
{
}

bool Not::has(CellI& role)
{
    if (&role == &kb.cells.type) {
        return true;
    }
    if (&role == &kb.coding.input) {
        return true;
    }
    if (&role == &kb.coding.output) {
        return true;
    }

    return false;
}

void Not::set(CellI& role, CellI& value)
{
    // Editing a control node is not possible
}

void Not::operator()()
{
    bool input = m_input[kb.coding.value] == kb.boolean.true_;
    m_output.set(kb.coding.value, kb.toKbBool(!input));
}

CellI& Not::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return type();
    }
    if (&role == &kb.coding.input) {
        return m_input;
    }
    if (&role == &kb.coding.output) {
        return m_output;
    }

    return kb.cells.emptyObject;
}

Type& Not::type()
{
    return kb.type.op.logic.Not;
}

void Not::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string Not::name() const
{
    return "Not";
}

} // namespace logic
namespace math {

// ============================================================================
Add::Add(brain::Brain& kb, pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs) :
    CellI(kb),
    m_output(output), m_lhs(lhs), m_rhs(rhs)
{
}

bool Add::has(CellI& role)
{
    if (&role == &kb.cells.type) {
        return true;
    }
    if (&role == &kb.equation.lhs) {
        return true;
    }
    if (&role == &kb.equation.rhs) {
        return true;
    }
    if (&role == &kb.coding.output) {
        return true;
    }

    return false;
}

void Add::set(CellI& role, CellI& value)
{
    // Editing a control node is not possible
}

void Add::operator()()
{
    int lhs = static_cast<Number&>(m_lhs[kb.coding.value]).value();
    int rhs = static_cast<Number&>(m_rhs[kb.coding.value]).value();
    m_output.set(kb.coding.value, kb.pools.numbers.get(lhs + rhs));
}

CellI& Add::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return type();
    }
    if (&role == &kb.equation.lhs) {
        return m_lhs;
    }
    if (&role == &kb.equation.rhs) {
        return m_rhs;
    }
    if (&role == &kb.coding.output) {
        return m_output;
    }

    return kb.cells.emptyObject;
}

Type& Add::type()
{
    return kb.type.op.math.Add;
}

void Add::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string Add::name() const
{
    return "Add";
}

// ============================================================================
Subtract::Subtract(brain::Brain& kb, pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs) :
    CellI(kb),
    m_output(output), m_lhs(lhs), m_rhs(rhs)
{
}

bool Subtract::has(CellI& role)
{
    if (&role == &kb.cells.type) {
        return true;
    }
    if (&role == &kb.equation.lhs) {
        return true;
    }
    if (&role == &kb.equation.rhs) {
        return true;
    }
    if (&role == &kb.coding.output) {
        return true;
    }

    return false;
}

void Subtract::set(CellI& role, CellI& value)
{
    // Editing a control node is not possible
}

void Subtract::operator()()
{
    int lhs = static_cast<Number&>(m_lhs[kb.coding.value]).value();
    int rhs = static_cast<Number&>(m_rhs[kb.coding.value]).value();
    m_output.set(kb.coding.value, kb.pools.numbers.get(lhs - rhs));
}

CellI& Subtract::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return type();
    }
    if (&role == &kb.equation.lhs) {
        return m_lhs;
    }
    if (&role == &kb.equation.rhs) {
        return m_rhs;
    }
    if (&role == &kb.coding.output) {
        return m_output;
    }

    return kb.cells.emptyObject;
}

Type& Subtract::type()
{
    return kb.type.op.math.Subtract;
}

void Subtract::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string Subtract::name() const
{
    return "Subtract";
}

// ============================================================================
Multiply::Multiply(brain::Brain& kb, pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs) :
    CellI(kb),
    m_output(output), m_lhs(lhs), m_rhs(rhs)
{
}

bool Multiply::has(CellI& role)
{
    if (&role == &kb.cells.type) {
        return true;
    }
    if (&role == &kb.equation.lhs) {
        return true;
    }
    if (&role == &kb.equation.rhs) {
        return true;
    }
    if (&role == &kb.coding.output) {
        return true;
    }

    return false;
}

void Multiply::set(CellI& role, CellI& value)
{
    // Editing a control node is not possible
}

void Multiply::operator()()
{
    int lhs = static_cast<Number&>(m_lhs[kb.coding.value]).value();
    int rhs = static_cast<Number&>(m_rhs[kb.coding.value]).value();
    m_output.set(kb.coding.value, kb.pools.numbers.get(lhs * rhs));
}

CellI& Multiply::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return type();
    }
    if (&role == &kb.equation.lhs) {
        return m_lhs;
    }
    if (&role == &kb.equation.rhs) {
        return m_rhs;
    }
    if (&role == &kb.coding.output) {
        return m_output;
    }

    return kb.cells.emptyObject;
}

Type& Multiply::type()
{
    return kb.type.op.math.Multiply;
}

void Multiply::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string Multiply::name() const
{
    return "Multiply";
}

// ============================================================================
Divide::Divide(brain::Brain& kb, pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs) :
    CellI(kb),
    m_output(output), m_lhs(lhs), m_rhs(rhs)
{
}

bool Divide::has(CellI& role)
{
    if (&role == &kb.cells.type) {
        return true;
    }
    if (&role == &kb.equation.lhs) {
        return true;
    }
    if (&role == &kb.equation.rhs) {
        return true;
    }
    if (&role == &kb.coding.output) {
        return true;
    }

    return false;
}

void Divide::set(CellI& role, CellI& value)
{
    // Editing a control node is not possible
}

void Divide::operator()()
{
    int lhs = static_cast<Number&>(m_lhs[kb.coding.value]).value();
    int rhs = static_cast<Number&>(m_rhs[kb.coding.value]).value();
    m_output.set(kb.coding.value, kb.pools.numbers.get(lhs / rhs));
}

CellI& Divide::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return type();
    }
    if (&role == &kb.equation.lhs) {
        return m_lhs;
    }
    if (&role == &kb.equation.rhs) {
        return m_rhs;
    }
    if (&role == &kb.coding.output) {
        return m_output;
    }

    return kb.cells.emptyObject;
}

Type& Divide::type()
{
    return kb.type.op.math.Divide;
}

void Divide::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string Divide::name() const
{
    return "Divide";
}

// ============================================================================
LessThan::LessThan(brain::Brain& kb, pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs) :
    CellI(kb),
    m_output(output), m_lhs(lhs), m_rhs(rhs)
{
}

bool LessThan::has(CellI& role)
{
    if (&role == &kb.cells.type) {
        return true;
    }
    if (&role == &kb.equation.lhs) {
        return true;
    }
    if (&role == &kb.equation.rhs) {
        return true;
    }
    if (&role == &kb.coding.output) {
        return true;
    }

    return false;
}

void LessThan::set(CellI& role, CellI& value)
{
    // Editing a control node is not possible
}

void LessThan::operator()()
{
    int lhs = static_cast<Number&>(m_lhs[kb.coding.value]).value();
    int rhs = static_cast<Number&>(m_rhs[kb.coding.value]).value();
    m_output.set(kb.coding.value, lhs < rhs ? kb.boolean.true_ : kb.boolean.false_);
}

CellI& LessThan::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return type();
    }
    if (&role == &kb.equation.lhs) {
        return m_lhs;
    }
    if (&role == &kb.equation.rhs) {
        return m_rhs;
    }
    if (&role == &kb.coding.output) {
        return m_output;
    }

    return kb.cells.emptyObject;
}

Type& LessThan::type()
{
    return kb.type.op.math.LessThan;
}

void LessThan::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string LessThan::name() const
{
    return "LessThan";
}

// ============================================================================
GreaterThan::GreaterThan(brain::Brain& kb, pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs) :
    CellI(kb),
    m_output(output), m_lhs(lhs), m_rhs(rhs)
{
}

bool GreaterThan::has(CellI& role)
{
    if (&role == &kb.cells.type) {
        return true;
    }
    if (&role == &kb.equation.lhs) {
        return true;
    }
    if (&role == &kb.equation.rhs) {
        return true;
    }
    if (&role == &kb.coding.output) {
        return true;
    }

    return false;
}

void GreaterThan::set(CellI& role, CellI& value)
{
    // Editing a control node is not possible
}

void GreaterThan::operator()()
{
    int lhs = static_cast<Number&>(m_lhs[kb.coding.value]).value();
    int rhs = static_cast<Number&>(m_rhs[kb.coding.value]).value();
    m_output.set(kb.coding.value, lhs > rhs ? kb.boolean.true_ : kb.boolean.false_);
}

CellI& GreaterThan::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return type();
    }
    if (&role == &kb.equation.lhs) {
        return m_lhs;
    }
    if (&role == &kb.equation.rhs) {
        return m_rhs;
    }
    if (&role == &kb.coding.output) {
        return m_output;
    }

    return kb.cells.emptyObject;
}

Type& GreaterThan::type()
{
    return kb.type.op.math.GreaterThan;
}

void GreaterThan::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string GreaterThan::name() const
{
    return "GreaterThan";
}

} // namespace math
} // namespace op
namespace pipeline {
// ============================================================================
Base::Base(brain::Brain& kb, Base* first) :
    CellI(kb),
    m_first(first)
{
}

void Base::addNext(Base& cell)
{
    m_next = &cell;
}

void Base::setCurrent()
{
    m_first->set(kb.sequence.current, *this);
}

// ============================================================================
Void::Void(brain::Brain& kb, const std::string& name) :
    Base(kb, this), m_name(name), m_current(this)
{
}

bool Void::has(CellI& role)
{
    if (&role == &kb.cells.type) {
        return true;
    }
    if (&role == &kb.sequence.first) {
        return true;
    }
    if (&role == &kb.sequence.next) {
        return m_next;
    }
    if (&role == &kb.sequence.current) {
        return true;
    }

    return false;
}

void Void::set(CellI& role, CellI& value)
{
}

void Void::operator()()
{
    setCurrent();
    if (m_next) {
        (*m_next)();
    }
}

CellI& Void::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return type();
    }
    if (&role == &kb.sequence.first) {
        return *m_first;
    }
    if (&role == &kb.sequence.next && m_next) {
        return *m_next;
    }
    if (&role == &kb.sequence.current) {
        return *m_current;
    }

    return kb.cells.emptyObject;
}

Type& Void::type()
{
    return kb.type.pipeline.Void;
}

void Void::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string Void::name() const
{
    return m_name;
}

// ============================================================================
Input::Input(brain::Brain& kb, CellI& value, const std::string& name) :
    Base(kb, this), m_name(name), m_value(&value), m_current(this)
{
}

Input::Input(brain::Brain& kb, CellI* value, const std::string& name) :
    Base(kb, this), m_name(name), m_value(value), m_current(this)
{
}

bool Input::has(CellI& role)
{
    if (&role == &kb.cells.type) {
        return true;
    }
    if (&role == &kb.sequence.first) {
        return true;
    }
    if (&role == &kb.sequence.next) {
        return m_next;
    }
    if (&role == &kb.sequence.current) {
        return true;
    }
    if (&role == &kb.coding.value) {
        return true;
    }

    return false;
}

void Input::set(CellI& role, CellI& value)
{
}

void Input::operator()()
{
    if (m_next) {
        (*m_next)();
    }
}

CellI& Input::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return type();
    }
    if (&role == &kb.sequence.first) {
        return *m_first;
    }
    if (&role == &kb.sequence.next && m_next) {
        return *m_next;
    }
    if (&role == &kb.sequence.current) {
        return *m_current;
    }
    if (&role == &kb.coding.value && m_value) {
        return *m_value;
    }

    return kb.cells.emptyObject;
}

Type& Input::type()
{
    return kb.type.pipeline.Input;
}

void Input::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string Input::name() const
{
    return m_name;
}

// ============================================================================
New::New(brain::Brain& kb, Type& objectType, const std::string& name) :
    Base(kb, this), m_name(name), m_objectType(objectType), m_current(this)
{
}

bool New::has(CellI& role)
{
    if (&role == &kb.cells.type) {
        return true;
    }
    if (&role == &kb.sequence.first) {
        return true;
    }
    if (&role == &kb.sequence.next) {
        return m_next;
    }
    if (&role == &kb.sequence.current) {
        return true;
    }
    if (&role == &kb.coding.value && m_value) {
        return m_value;
    }
    if (&role == &kb.coding.objectType) {
        return true;
    }

    return false;
}

void New::set(CellI& role, CellI& value)
{
    if (&role == &kb.coding.objectType && &value.type() == &kb.type.Type_) {
        m_value = &value;
    }
}

void New::operator()()
{
    if (&m_objectType == &kb.type.Type_) {
        m_value = new Type(kb);
    }
    if (&m_objectType == &kb.type.Number) {
        m_value = new Number(kb);
    }
    if (&m_objectType == &kb.type.String) {
        m_value = new String(kb);
    }
    if (m_next) {
        (*m_next)();
    }
}

CellI& New::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return kb.type.pipeline.New;
    }
    if (&role == &kb.sequence.first) {
        return *m_first;
    }
    if (&role == &kb.sequence.next && m_next) {
        return *m_next;
    }
    if (&role == &kb.sequence.current) {
        return *m_current;
    }
    if (&role == &kb.coding.value && m_value) {
        return *m_value;
    }
    if (&role == &kb.coding.objectType) {
        return m_objectType;
    }

    return kb.cells.emptyObject;
}

Type& New::type()
{
    return kb.type.pipeline.New;
}

void New::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string New::name() const
{
    return m_name;
}

// ============================================================================
Fork::Fork(brain::Brain& kb, Base& input, const std::string& name) :
    Base(kb, input.m_first), m_input(input), m_name(name)
{
    input.addNext(*this);
}

bool Fork::has(CellI& role)
{
    if (&role == &kb.cells.type) {
        return true;
    }
    if (&role == &kb.sequence.first) {
        return true;
    }
    if (&role == &kb.sequence.next) {
        return m_next;
    }
    if (&role == &kb.coding.input) {
        return true;
    }
    if (&role == &kb.coding.value && m_value) {
        return true;
    }
    if (&role == &kb.coding.branch && m_branch) {
        return true;
    }

    return false;
}

void Fork::set(CellI& role, CellI& value)
{
}

void Fork::operator()()
{
    setCurrent();
    m_value = &m_input[kb.coding.value];
    if (m_branch) {
        (*m_branch)();
    }
    if (m_next) {
        (*m_next)();
    }
}

CellI& Fork::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return type();
    }
    if (&role == &kb.sequence.first) {
        return *m_first;
    }
    if (&role == &kb.sequence.next && m_next) {
        return *m_next;
    }
    if (&role == &kb.coding.input) {
        return m_input;
    }
    if (&role == &kb.coding.value && m_value) {
        return *m_value;
    }
    if (&role == &kb.coding.branch && m_branch) {
        return *m_branch;
    }

    return kb.cells.emptyObject;
}

Type& Fork::type()
{
    return kb.type.pipeline.Fork;
}

void Fork::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string Fork::name() const
{
    return m_name;
}

void Fork::addBranch(Base& cell)
{
    m_branch = &cell;
}

// ============================================================================
Delete::Delete(brain::Brain& kb, Base& input, const std::string& name) :
    Base(kb, input.m_first), m_name(name), m_input(&input)
{
}

bool Delete::has(CellI& role)
{
    if (&role == &kb.cells.type) {
        return true;
    }
    if (&role == &kb.sequence.first) {
        return true;
    }
    if (&role == &kb.coding.input) {
        return true;
    }

    return false;
}

void Delete::set(CellI& role, CellI& value)
{
    if (&role == &kb.coding.input) {
        m_input = &value;
    }
}

void Delete::operator()()
{
    setCurrent();
    CellI* cell = &(*m_input)[kb.coding.value];
    delete cell;
}

CellI& Delete::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return type();
    }
    if (&role == &kb.sequence.first) {
        return *m_first;
    }
    if (&role == &kb.coding.input && m_input) {
        return *m_input;
    }

    return kb.cells.emptyObject;
}

Type& Delete::type()
{
    return kb.type.pipeline.Delete;
}

void Delete::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string Delete::name() const
{
    return m_name;
}

// ============================================================================
bool Node::has(CellI& role)
{
    if (&role == &kb.cells.type) {
        return true;
    }
    if (&role == &kb.sequence.first) {
        return true;
    }
    if (&role == &kb.sequence.next) {
        return m_next;
    }
    if (&role == &kb.coding.input) {
        return true;
    }
    if (&role == &kb.coding.op) {
        return true;
    }
    if (&role == &kb.coding.value) {
        return m_value;
    }

    return false;
}

void Node::set(CellI& role, CellI& value)
{
    if (&role == &kb.coding.value) {
        m_value = &value;
    }
}

void Node::operator()()
{
    setCurrent();
    m_value = &(*m_input)[kb.coding.value];
    (*m_op)();
    if (m_next) {
        (*m_next)();
    }
}

CellI& Node::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return type();
    }
    if (&role == &kb.sequence.first) {
        return *m_first;
    }
    if (&role == &kb.sequence.next && m_next) {
        return *m_next;
    }
    if (&role == &kb.coding.input && m_input) {
        return *m_input;
    }
    if (&role == &kb.coding.op) {
        return *m_op;
    }
    if (&role == &kb.coding.value && m_value) {
        return *m_value;
    }

    return kb.cells.emptyObject;
}

Type& Node::type()
{
    return kb.type.pipeline.Node;
}

void Node::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string Node::name() const
{
    return m_name;
}

// ============================================================================
IfThen::IfThen(brain::Brain& kb, Base& input, const std::string& name) :
    Base(kb, input.m_first), m_name(name), m_input(input)
{
}

bool IfThen::has(CellI& role)
{
    if (&role == &kb.cells.type) {
        return true;
    }
    if (&role == &kb.sequence.first) {
        return true;
    }
    if (&role == &kb.sequence.next) {
        return m_next;
    }
    if (&role == &kb.coding.input) {
        return true;
    }
    if (&role == &kb.coding.condition) {
        return m_condition;
    }
    if (&role == &kb.coding.then) {
        return m_thenBranch;
    }
    if (&role == &kb.coding.else_) {
        return m_elseBranch;
    }

    return false;
}

void IfThen::set(CellI& role, CellI& value)
{
}

void IfThen::operator()()
{
    setCurrent();
    (*m_condition)();
    bool condition = &(*m_condition)[kb.coding.value] == &kb.boolean.true_;
    if (condition) {
        (*m_thenBranch)();
    } else {
        if (m_elseBranch) {
            (*m_elseBranch)();
        }
    }
    if (m_next) {
        (*m_next)();
    }
}

CellI& IfThen::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return type();
    }
    if (&role == &kb.sequence.first) {
        return *m_first;
    }
    if (&role == &kb.sequence.next && m_next) {
        return *m_next;
    }
    if (&role == &kb.coding.input) {
        return m_input;
    }
    if (&role == &kb.coding.condition && m_condition) {
        return *m_condition;
    }
    if (&role == &kb.coding.then && m_thenBranch) {
        return *m_thenBranch;
    }
    if (&role == &kb.coding.else_ && m_elseBranch) {
        return *m_elseBranch;
    }

    return kb.cells.emptyObject;
}

Type& IfThen::type()
{
    return kb.type.pipeline.IfThen;
}

void IfThen::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string IfThen::name() const
{
    return m_name;
}

void IfThen::addCondition(Base& cell)
{
    m_condition = &cell;
}

void IfThen::addThenBranch(Base& cell)
{
    m_thenBranch = &cell;
}

void IfThen::addElseBranch(Base& cell)
{
    m_elseBranch = &cell;
}

// ============================================================================
DoWhile::DoWhile(brain::Brain& kb, Base& input, const std::string& name) :
    Base(kb, input.m_first), m_name(name), m_input(input)
{
}

bool DoWhile::has(CellI& role)
{
    if (&role == &kb.cells.type) {
        return true;
    }
    if (&role == &kb.sequence.first) {
        return true;
    }
    if (&role == &kb.sequence.next) {
        return m_next;
    }
    if (&role == &kb.coding.input) {
        return true;
    }
    if (&role == &kb.coding.condition) {
        return m_condition;
    }
    if (&role == &kb.coding.statement) {
        return m_statement;
    }

    return false;
}

void DoWhile::set(CellI& role, CellI& value)
{
}

void DoWhile::operator()()
{
    setCurrent();
    bool condition = false;
    do {
        (*m_statement)();
        (*m_condition)();
        condition = &(*m_condition)[kb.coding.value] == &kb.boolean.true_;
    } while (condition);

    if (m_next) {
        (*m_next)();
    }
}

CellI& DoWhile::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return type();
    }
    if (&role == &kb.sequence.first) {
        return *m_first;
    }
    if (&role == &kb.sequence.next && m_next) {
        return *m_next;
    }
    if (&role == &kb.coding.input) {
        return m_input;
    }
    if (&role == &kb.coding.condition && m_condition) {
        return *m_condition;
    }
    if (&role == &kb.coding.statement && m_statement) {
        return *m_statement;
    }

    return kb.cells.emptyObject;
}

Type& DoWhile::type()
{
    return kb.type.pipeline.DoWhile;
}

void DoWhile::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string DoWhile::name() const
{
    return m_name;
}

// ============================================================================
While::While(brain::Brain& kb, Base& input, const std::string& name) :
    Base(kb, input.m_first), m_name(name), m_input(input)
{
}

bool While::has(CellI& role)
{
    if (&role == &kb.cells.type) {
        return true;
    }
    if (&role == &kb.sequence.first) {
        return true;
    }
    if (&role == &kb.sequence.next) {
        return m_next;
    }
    if (&role == &kb.coding.condition) {
        return m_condition;
    }
    if (&role == &kb.coding.statement) {
        return m_statement;
    }

    return false;
}

void While::set(CellI& role, CellI& value)
{
}

void While::operator()()
{
    setCurrent();
    bool condition = false;
    (*m_condition)();
    condition = &(*m_condition)[kb.coding.value] == &kb.boolean.true_;
    while (condition) {
        (*m_statement)();
        (*m_condition)();
        condition = &(*m_condition)[kb.coding.value] == &kb.boolean.true_;
    };

    if (m_next) {
        (*m_next)();
    }
}

CellI& While::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return type();
    }
    if (&role == &kb.sequence.first) {
        return *m_first;
    }
    if (&role == &kb.sequence.next && m_next) {
        return *m_next;
    }
    if (&role == &kb.coding.input) {
        return m_input;
    }
    if (&role == &kb.coding.condition && m_condition) {
        return *m_condition;
    }
    if (&role == &kb.coding.statement && m_statement) {
        return *m_statement;
    }

    return kb.cells.emptyObject;
}

Type& While::type()
{
    return kb.type.pipeline.While;
}

void While::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string While::name() const
{
    return m_name;
}

} // namespace pipeline
} // namespace control
} // namespace cells
} // namespace synth
