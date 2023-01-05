#include "Cells.h"

#include <format>

#pragma warning(disable : 4996)
#include <utf8.h>

namespace synth {
namespace cells {

// ============================================================================

SlotRef::SlotRef(const std::string& name, Type& type, CellI& role) :
    m_name(name), m_type(type), m_role(role)
{
}

std::unique_ptr<Type> Slot::s_type;
Slot* Slot::s_slotSlotType = nullptr;
Slot* Slot::s_slotSlotName = nullptr;
Slot* Slot::s_slotSlotRole = nullptr;

Slot::Slot(const std::string& name, Type& type, CellI& role) :
    m_name(name), m_slotType(type), m_slotRole(role)
{
}

bool Slot::has(CellI& role)
{
    if (&role == &data::type || &role == &data::slotType || &role == &data::slotName || &role == &data::slotRole) {
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
    if (&role == &data::type) {
        return *s_type;
    }
    if (&role == &data::slotType) {
        return m_slotType;
    }
    if (&role == &data::slotName) {
        if (!m_slotNameString) {
            m_slotNameString.reset(new String(m_name));
        }
        return *m_slotNameString;
    }
    if (&role == &data::slotRole) {
        return *s_slotSlotRole;
    }

    return Object::emptyObject();
}

Type& Slot::type()
{
    return *s_type;
}

std::string Slot::printAs(Printer& printer)
{
    return printer.print(*this);
}

std::string Slot::name() const
{
    return m_name;
}

void Slot::staticInit()
{
    s_type.reset(new Type("Slot"));
}

void Slot::staticInitMembers()
{
    s_slotSlotType = &s_type->createSlot("slotType", Type::t(), data::slotType);
    s_slotSlotName = &s_type->createSlot("slotName", String::t(), data::slotName);
    s_slotSlotRole = &s_type->createSlot("slotRole", Type::anyType(), data::slotRole);
}

Type& Slot::t()
{
    return *s_type;
}

Slot& Slot::slotSlotName()
{
    return *s_slotSlotName;
}

Slot& Slot::slotSlotType()
{
    return *s_slotSlotType;
}

Slot& Slot::slotSlotRole()
{
    return *s_slotSlotRole;
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
std::unique_ptr<Type> Type::s_type;
Slot* Type::s_slotType  = nullptr;
Slot* Type::s_slotSlots = nullptr;
std::unique_ptr<Type> Type::s_anyType;

Type::Type(const std::string& name) :
    m_name(name)
{
    registerTypeSlot();
}

Type::Type(const std::string& name, std::initializer_list<SlotRef> slots) :
    m_name(name)
{
    registerTypeSlot();
    for (const SlotRef& slotRef : slots) {
        createSlot(slotRef.m_name, slotRef.m_type, slotRef.m_role);
    }
}

bool Type::has(CellI& role)
{
    if (&role == &data::type || &role == &data::slots) {
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
    if (&role == &data::type) {
        return *s_type;
    }
    if (&role == &data::slots) {
        if (!m_slotsList)
            m_slotsList.reset(new List(m_slotRefs));

        return *m_slotsList;
    }

    return Object::emptyObject();
}

Type& Type::type()
{
    return *s_type;
}

std::string Type::printAs(Printer& printer)
{
    return printer.print(*this);
}

std::string Type::name() const
{
    return m_name;
}

void Type::staticInit()
{
    s_type.reset(new Type("Type"));
    s_slotType = &s_type->createSlot("type", *s_type, data::type);
    s_type->referenceSlot("type", *s_slotType);
    s_anyType.reset(new Type("AnyType"));
}

void Type::staticInitMembers()
{
    static Type listItemForSlot("ListItem<Slot>");
    listItemForSlot.createSlot("prev", listItemForSlot, data::previous);
    listItemForSlot.createSlot("next", listItemForSlot, data::next);
    listItemForSlot.createSlot("value", Slot::t(), data::value);

    static Type listOfSlots("List<Slot>");
    listOfSlots.createSlot("first", listItemForSlot, data::first);
    listOfSlots.createSlot("last", listItemForSlot, data::last);
    listOfSlots.createSlot("size", Number::t(), data::size);

    s_slotSlots = &s_type->createSlot("slots", listOfSlots, data::slots);
}

void Type::addSlots(std::initializer_list<SlotRef> slots)
{
    for (const SlotRef& slotRef : slots) {
        createSlot(slotRef.m_name, slotRef.m_type, slotRef.m_role);
    }
}

Slot& Type::createSlot(const std::string& name, Type& type, CellI& role)
{
    auto slotIt = m_slotRefs.find(name);
    if (slotIt != m_slotRefs.end()) {
        if (&slotIt->second->slotType() != &type) {
            throw "Member name already registered with an other class";
        }
        return *slotIt->second;
    } else {
        auto it = m_slots.emplace(std::piecewise_construct,
                                  std::forward_as_tuple(name),
                                  std::forward_as_tuple(name, type, role));

        Slot& slot       = it.first->second;
        m_slotRefs[name] = &slot;
        m_roles[&role]   = &slot;

        return slot;
    }
}

void Type::referenceSlot(const std::string& name, Slot& slot)
{
    m_slotRefs[name]          = &slot;
    m_roles[&slot.slotRole()] = &slot;
}

bool Type::has(const std::string& name) const
{
    return m_slotRefs.find(name) != m_slotRefs.end();
}

bool Type::hasSlot(CellI& role)
{
    return m_roles.find(&role) != m_roles.end();
}

bool Type::hasSlot(const std::string& name)
{
    return m_slotRefs.find(name) != m_slotRefs.end();
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
    auto findIt = m_slotRefs.find(name);
    if (findIt == m_slotRefs.end())
        throw "emptyMember";

    return *findIt->second;
}

std::map<std::string, Slot*>& Type::slots()
{
    return m_slotRefs;
}

Type& Type::t()
{
    return *s_type;
}

Slot& Type::slotType()
{
    return *s_slotType;
}

Slot& Type::slotSlots()
{
    return *s_slotSlots;
}

Type& Type::anyType()
{
    return *s_anyType;
}

void Type::registerTypeSlot()
{
    if (!s_slotType) {
        return;
    }
    referenceSlot("type", *s_slotType);
}

// ============================================================================
std::unique_ptr<Object> Object::s_emptyObject;

Object::Object(Type& type) :
    Object("", type)
{
}

Object::Object(const std::string& name, Type& type) :
    m_name(name), m_type(type)
{
    m_roles[&data::type] = &type;
}

bool Object::has(CellI& role)
{
    if (&role == &data::type)
        return true;

    return m_roles.find(&role) != m_roles.end();
}

void Object::set(CellI& role, CellI& value)
{
    if (&role == &data::type) {
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
        return emptyObject();

    return *findIt->second;
}

Type& Object::type()
{
    return m_type;
}

std::string Object::printAs(Printer& printer)
{
    return printer.print(*this);
}

std::string Object::name() const
{
    return m_name;
}

std::map<CellI*, CellI*>& Object::roles()
{
    return m_roles;
}

void Object::staticInit()
{
    static Type emptyClass("Empty");
    s_emptyObject.reset(new Object(emptyClass));
}

Object& Object::emptyObject()
{
    return *s_emptyObject;
}

// ============================================================================
ListItem::ListItem(Type& t) :
    m_type(t)
{
    m_slotPrev  = &t.getSlot("prev");
    m_slotNext  = &t.getSlot("next");
    m_slotValue = &t.getSlot("value");
}

bool ListItem::has(CellI& role)
{
    if (&role == &data::type || &role == &data::previous || &role == &data::next || &role == &data::value) {
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
    if (&role == &data::type) {
        return m_type;
    }
    if (&role == &data::previous) {
        if (m_prev)
            return *m_prev;
        else
            return Object::emptyObject();
    }
    if (&role == &data::next) {
        if (m_next)
            return *m_next;
        else
            return Object::emptyObject();
    }
    if (&role == &data::value) {
        if (m_value)
            return *m_value;
        else
            return *m_value;
    }

    return Object::emptyObject();
}

Type& ListItem::type()
{
    return m_type;
}

std::string ListItem::printAs(Printer& printer)
{
    return printer.print(*this);
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

Slot& ListItem::slotPrev()
{
    return *m_slotPrev;
}
Slot& ListItem::slotNext()
{
    return *m_slotNext;
}

Slot& ListItem::slotValue()
{
    return *m_slotValue;
}

// ============================================================================
template <typename T>
List::List(const std::vector<T>& values) :
    m_listType("List<T>"),
    m_itemType("ListItem<T>")
{
    m_slotFirst = &m_listType.createSlot("first", m_itemType, data::first);
    m_slotLast  = &m_listType.createSlot("last", m_itemType, data::last);
    m_slotSize  = &m_listType.createSlot("size", Number::t(), data::size);

    Type& valueType = values.front()->type();
    m_itemType.createSlot("prev", m_itemType, data::previous);
    m_itemType.createSlot("next", m_itemType, data::next);
    m_itemType.createSlot("value", valueType, data::value);

    m_items.reserve(values.size());
    ListItem* prevListItem = nullptr;
    for (CellI* value : values) {
        auto& listItem = m_items.emplace_back(m_itemType);
        if (prevListItem == nullptr) {
            listItem.prev(nullptr);
        } else {
            listItem.prev(prevListItem);
            prevListItem->next(&listItem);
        }
        listItem.next(nullptr);
        listItem.value(value);
        prevListItem = &listItem;
    }
}

template <typename T>
List::List(std::map<std::string, T>& values) :
    m_listType("List<T>"),
    m_itemType("ListItem<T>")
{
    m_slotFirst = &m_listType.createSlot("first", m_itemType, data::first);
    m_slotLast  = &m_listType.createSlot("last", m_itemType, data::last);
    m_slotSize  = &m_listType.createSlot("size", Number::t(), data::size);

    Type& valueType = (*values.begin()).second->type();
    m_itemType.createSlot("prev", m_itemType, data::previous);
    m_itemType.createSlot("next", m_itemType, data::next);
    m_itemType.createSlot("value", valueType, data::value);

    m_items.reserve(values.size());
    ListItem* prevListItem = nullptr;
    for (auto& valuePairs : values) {
        CellI* value       = valuePairs.second;
        auto& listItemCell = m_items.emplace_back(m_itemType);
        if (prevListItem == nullptr) {
            listItemCell.prev(nullptr);
        } else {
            listItemCell.prev(prevListItem);
            prevListItem->next(&listItemCell);
        }
        listItemCell.next(nullptr);
        listItemCell.value(value);
        prevListItem = &listItemCell;
    }
}

bool List::has(CellI& role)
{
    if (&role == &data::type || &role == &data::first || &role == &data::last || &role == &data::size) {
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
    if (&role == &data::type) {
        return m_listType;
    }
    if (&role == &data::first) {
        if (m_items.empty()) {
            return Object::emptyObject();
        }
        return m_items.front();
    }
    if (&role == &data::last) {
        if (m_items.empty()) {
            return Object::emptyObject();
        }
        return m_items.back();
    }
    if (&role == &data::size) {
        Number* number = new Number((int)m_items.size());
        return *number;
    }

    return Object::emptyObject();
}

Type& List::type()
{
    return m_listType;
}

std::string List::printAs(Printer& printer)
{
    return printer.print(*this);
}

std::string List::name() const
{
    std::stringstream ss;
    ss << "List<>" << std::endl;

    return ss.str();
}

std::vector<ListItem>& List::items()
{
    return m_items;
}

Slot& List::slotFirst()
{
    return *m_slotFirst;
}

Slot& List::slotLast()
{
    return *m_slotLast;
}

Slot& List::slotSize()
{
    return *m_slotSize;
}

// ============================================================================
std::unique_ptr<Type> Digits::s_digitClassCell;
std::vector<Object> Digits::s_digits;

void Digits::staticInit()
{
    s_digitClassCell.reset(new Type("Digit"));
    for (int i = 0; i < 10; ++i) {
        std::string digitName = "Digit_" + std::to_string(i);
        s_digits.push_back({ digitName, *s_digitClassCell });
    }
}

// ============================================================================
std::unique_ptr<Type> Number::s_type;
Slot* Number::s_slotValue = nullptr;
Slot* Number::s_slotSign  = nullptr;

Number::Number(int value) :
    m_value(value)
{
}

bool Number::has(CellI& role)
{
    if (&role == &data::type || &role == &data::value || &role == &data::sign) {
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
    if (&role == &data::type) {
        return *s_type;
    }

    if (&role == &data::sign) {
        return Object::emptyObject(); // TODO
    }

    if (&role == &data::value) {
        if (m_digits.empty()) {
            calculateDigits();
            m_digitsList.reset(new List(m_digits));
        }

        return *m_digitsList;
    }

    return Object::emptyObject();
}

Type& Number::type()
{
    return *s_type;
}

std::string Number::printAs(Printer& printer)
{
    return printer.print(*this);
}

std::string Number::name() const
{
    return std::to_string(m_value);
}

int Number::value() const
{
    return m_value;
}

void Number::staticInit()
{
    s_type.reset(new Type("Number"));
    //    s_slotValue = &s_type->createSlot("value", List::t()); // TODO
    s_slotValue = &s_type->createSlot("value", Type::anyType(), data::value);
    s_slotSign  = &s_type->createSlot("sign", Number::t(), data::sign); // TODO
}

Type& Number::t()
{
    return *s_type;
}

Slot& Number::slotSign()
{
    return *s_slotSign;
}

Slot& Number::slotValue()
{
    return *s_slotValue;
}

void Number::calculateDigits()
{
    if (m_value == 0) {
        m_digits.push_back(&Digits::s_digits[0]);
        return;
    }
    int value = m_value;
    while (value) {
        m_digits.push_back(&Digits::s_digits[value % 10]);
        value /= 10;
    }
    std::reverse(m_digits.begin(), m_digits.end());
}

// ============================================================================
std::map<int, Number> Numbers::s_numbers;

Number& Numbers::get(int number)
{
    auto numberIt = s_numbers.find(number);
    if (numberIt != s_numbers.end()) {
        return numberIt->second;
    } else {
        auto it = s_numbers.emplace(number, number);
        return it.first->second;
    }
}

// ============================================================================
std::unique_ptr<Type> Chars::s_type;
std::map<char32_t, Object> Chars::s_characters;

void Chars::staticInit()
{
    s_type.reset(new Type("UnicodeCharacter"));
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
        return Object::emptyObject();
    }
}

Type& Chars::type()
{
    return *s_type;
}

void Chars::registerUnicodeBlock(char32_t from, char32_t to)
{
    for (char32_t unicodeValue = from; unicodeValue <= to; ++unicodeValue) {
        std::string characterName = std::format("Unicode_{:#04x}", (int)unicodeValue);
        s_characters.emplace(std::piecewise_construct,
                             std::forward_as_tuple(unicodeValue),
                             std::forward_as_tuple(characterName, *s_type));
    }
}

// ============================================================================
std::unique_ptr<Type> String::s_type;
Slot* String::s_slotCharacters = nullptr;

String::String(const std::string& str) :
    m_value(str)
{
}

bool String::has(CellI& role)
{
    if (&role == &data::type || &role == &data::value) {
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
    if (&role == &data::type) {
        return *s_type;
    } else if (&role == &data::value) {
        if (m_characters.empty()) {
            calculateCharacters();
            m_charactersList.reset(new List(m_characters));
        }

        return *m_charactersList;
    } else {
        return Object::emptyObject();
    }
}

Type& String::type()
{
    return *s_type;
}

std::string String::printAs(Printer& printer)
{
    return printer.print(*this);
}

std::string String::name() const
{
    return m_value;
}

const std::string& String::value() const
{
    return m_value;
}

void String::staticInit()
{
    s_type.reset(new Type("String"));
    //    s_slotCharacters = &s_type->createSlot("value", List::t());
    s_slotCharacters = &s_type->createSlot("value", Type::anyType(), data::value); // TODO
}

Type& String::t()
{
    return *s_type;
}

Slot& String::slotCharacters()
{
    return *s_slotCharacters;
}

void String::calculateCharacters()
{
    utf8::iterator<const char*> valueIt(m_value.data(), m_value.data(), m_value.data() + m_value.size());
    utf8::iterator<const char*> valueEndIt(m_value.data() + m_value.size(), m_value.data(), m_value.data() + m_value.size());

    for (auto& it = valueIt; it != valueEndIt; ++valueIt) {
        char32_t unicodeValue = *it;
        m_characters.push_back(&Chars::get(unicodeValue));
    }
}

namespace hybrid {
// ============================================================================
Color::Color(const input::Color& inputColor) :
    m_inputColor(inputColor)
{
}

bool Color::has(CellI& role)
{
    if (&role == &data::type) {
        return true;
    }
    if (&role == &data::colors::red || &role == &data::colors::green || &role == &data::colors::blue) {
        return true;
    }

    return false;
}

void Color::set(CellI& role, CellI& value)
{
    throw "Setting a generated pixelRef cell is not possible";
}

void Color::operator()()
{
    // Do nothing
}

CellI& Color::operator[](CellI& role)
{
    if (&role == &data::type) {
        return t();
    }
    if (&role == &data::colors::red) {
        return Numbers::get(m_inputColor.m_red);
    }
    if (&role == &data::colors::green) {
        return Numbers::get(m_inputColor.m_green);
    }
    if (&role == &data::colors::blue) {
        return Numbers::get(m_inputColor.m_blue);
    }

    return Object::emptyObject();
}

Type& Color::type()
{
    return t();
}

std::string Color::printAs(Printer& printer)
{
    return printer.print(*this);
}

std::string Color::name() const
{
    std::stringstream ss;
    ss << "Color(" << m_inputColor.m_red << ", " << m_inputColor.m_green << ", " << m_inputColor.m_blue << ")";
    return ss.str();
}

Type& Color::t()
{
    return type::Color;
}

const input::Color& Color::color() const
{
    return m_inputColor;
}

// ============================================================================
Pixel::Pixel(int x, int y, const input::Color& inputColor) :
    m_x(Numbers::get(x)),
    m_y(Numbers::get(y)),
    m_color(inputColor),
    m_inputColor(inputColor)
{
}

bool Pixel::has(CellI& role)
{
    if (&role == &data::type) {
        return true;
    }
    if (&role == &data::directions::up && m_up) {
        return true;
    }
    if (&role == &data::directions::down && m_down) {
        return true;
    }
    if (&role == &data::directions::left && m_left) {
        return true;
    }
    if (&role == &data::directions::right && m_right) {
        return true;
    }
    if (&role == &data::coordinates::x) {
        return true;
    }
    if (&role == &data::coordinates::y) {
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
    if (&role == &data::type) {
        return t();
    }
    if (&role == &data::directions::up && m_up) {
        return *m_up;
    }
    if (&role == &data::directions::down && m_down) {
        return *m_down;
    }
    if (&role == &data::directions::left && m_left) {
        return *m_left;
    }
    if (&role == &data::directions::right && m_right) {
        return *m_right;
    }
    if (&role == &data::color) {
        return m_color;
    }
    if (&role == &data::coordinates::x) {
        return m_x;
    }
    if (&role == &data::coordinates::y) {
        return m_y;
    }

    return Object::emptyObject();
}

Type& Pixel::type()
{
    return t();
}

std::string Pixel::printAs(Printer& printer)
{
    return printer.print(*this);
}

std::string Pixel::name() const
{
    std::stringstream ss;
    ss << "Pixel[" << m_x.value() << ", " << m_y.value() << "](" << m_inputColor.m_red << ", " << m_inputColor.m_green << ", " << m_inputColor.m_blue << ")";
    return ss.str();
}

Type& Pixel::t()
{
    return type::Pixel;
}

const input::Color& Pixel::color() const
{
    return m_inputColor;
}

// ============================================================================
Sensor::Sensor(input::Picture& picture) :
    m_name(picture.name()), m_width(picture.width()), m_height(picture.height()), m_widthCell(Numbers::get(m_width)), m_heightCell(Numbers::get(m_height))
{
    const int senzorSize = m_height * m_width;

    m_pixels.clear();
    m_pixels.reserve(senzorSize);

    int x = 0;
    int y = 0;

    for (const input::Color& pixel : picture.pixels()) {
        m_pixels.emplace_back(x++, y, pixel);
        if (x == m_width) {
            x = 0;
            y += 1;
        }
    }

    for (y = 0; y < m_height; ++y) {
        for (x = 0; x < m_width; ++x) {
            Pixel& pixel = m_pixels[currentIndex(x, y)];
            pixel.m_up      = upPixel(x, y);
            pixel.m_down    = downPixel(x, y);
            pixel.m_left    = leftPixel(x, y);
            pixel.m_right   = rightPixel(x, y);
        }
    }
}

bool Sensor::has(CellI& role)
{
    if (&role == &data::type || &role == &data::width || &role == &data::height || &role == &data::first || &role == &data::last) {
        return true;
    }

    return false;
}

void Sensor::set(CellI& role, CellI& value)
{
    throw "Setting a generated Sensor cell is not possible";
}

void Sensor::operator()()
{
}

CellI& Sensor::operator[](CellI& role)
{
    if (&role == &data::type) {
        return t();
    }
    if (&role == &data::width) {
        return m_widthCell;
    }
    if (&role == &data::height) {
        return m_heightCell;
    }
    if (&role == &data::first) {
        return m_pixels.front();
    }
    if (&role == &data::last) {
        return m_pixels.back();
    }

    return Object::emptyObject();
}

Type& Sensor::type()
{
    return t();
}

std::string Sensor::printAs(Printer& printer)
{
    return printer.print(*this);
}

std::string Sensor::name() const
{
    return m_name;
}

Type& Sensor::t()
{
    return type::Sensor;
}

Pixel& Sensor::getPixel(int x, int y)
{
    return m_pixels[currentIndex(x, y)];
}

const Pixel& Sensor::getPixel(int x, int y) const
{
    return m_pixels.at(currentIndex(x, y));
}

int Sensor::currentIndex(int x, int y) const
{
    return y * m_width + x;
}

bool Sensor::isInRange(int x, int y) const
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

Pixel* Sensor::upPixel(int x, int y)
{
    if (!isInRange(x, y) || y == 0) {
        return nullptr;
    }
    int upIndex = (y - 1) * m_width + x;
    return &m_pixels[upIndex];
}

Pixel* Sensor::downPixel(int x, int y)
{
    if (!isInRange(x, y) || y == m_height - 1) {
        return nullptr;
    } else {
        int downIndex = (y + 1) * m_width + x;
        return &m_pixels[downIndex];
    }
}

Pixel* Sensor::leftPixel(int x, int y)
{
    if (!isInRange(x, y) || x == 0) {
        return nullptr;
    } else {
        return &m_pixels[y * m_width + x - 1];
    }
}

Pixel* Sensor::rightPixel(int x, int y)
{
    if (!isInRange(x, y) || x == m_width - 1) {
        return nullptr;
    } else {
        return &m_pixels[y * m_width + x + 1];
    }
}

int Sensor::width() const
{
    return m_width;
}

int Sensor::height() const
{
    return m_height;
}

} // namespace hybrid

namespace type {
Type Color("Color");
Type Pixel("Pixel");
Type Sensor("Sensor");
} // namespace type

static void staticInitClasses()
{
    type::Color.addSlots(
        { { "red", Number::t(), data::colors::red },
          { "green", Number::t(), data::colors::green },
          { "blue", Number::t(), data::colors::blue } });

    type::Pixel.addSlots(
        { { "up", hybrid::Pixel::t(), data::directions::up },
          { "down", hybrid::Pixel::t(), data::directions::down },
          { "left", hybrid::Pixel::t(), data::directions::left },
          { "right", hybrid::Pixel::t(), data::directions::right },
          { "x", Number::t(), data::coordinates::x },
          { "y", Number::t(), data::coordinates::y } });

    type::Sensor.addSlots(
        { { "width", Number::t(), data::width },
          { "height", Number::t(), data::height },
          { "firstPixel", hybrid::Pixel::t(), data::first },
          { "lastPixel", hybrid::Pixel::t(), data::last } });
}

namespace data {
Object slotType(Type::anyType());
Object slotName(Type::anyType());
Object slotRole(Type::anyType());

Object first(Type::anyType());
Object last(Type::anyType());
Object previous(Type::anyType());
Object next(Type::anyType());

namespace directions {
Object up(Type::anyType());
Object down(Type::anyType());
Object left(Type::anyType());
Object right(Type::anyType());
} // namespace directions

namespace coordinates {
Object x(Type::anyType());
Object y(Type::anyType());
} // namespace coordinates

extern Object color(Type::anyType());
namespace colors {
Object red(Type::anyType());
Object green(Type::anyType());
Object blue(Type::anyType());
} // namespace colors

Object width(Type::anyType());
Object height(Type::anyType());

Object size(Type::anyType());
Object value(Type::anyType());
Object type(Type::anyType());
Object slots(Type::anyType());
Object sign(Type::anyType());
} // namespace cells

// ============================================================================
void StaticInitializations()
{
    Type::staticInit();
    Slot::staticInit();
    Object::staticInit();
    Digits::staticInit();
    Number::staticInit();
    Chars::staticInit();
    String::staticInit();
    Type::staticInitMembers();
    Slot::staticInitMembers();

    staticInitClasses();
}

// ============================================================================
std::string CellValuePrinter::print(Slot& slotCell)
{
    std::stringstream ss;
    ss << slotCell.name() << ": " << slotCell.slotType().name();

    return ss.str();
}

std::string CellValuePrinter::print(Type& type)
{
    std::stringstream ss;
    ss << "class " << type.name() << " { ";
    bool isFirst = true;
    for (auto& slotI : type.slots()) {
        if (isFirst) {
            isFirst = false;
        } else {
            ss << ", ";
        }
        ss << slotI.first << ": " << slotI.second->slotType().name();
    }
    ss << " }";

    return ss.str();
}

std::string CellValuePrinter::print(Object& dataCell)
{
    std::stringstream ss;
    if (!dataCell.name().empty()) {
        ss << dataCell.name() << ": ";
    }
    ss << dataCell.type().name() << " { ";
    bool isFirst = true;
    for (auto& slotI : dataCell.type().slots()) {
        if (isFirst) {
            isFirst = false;
        } else {
            ss << ", ";
        }
        ss << "." << slotI.second->printAs(*this);
    }
    ss << " }";

    return ss.str();
}

std::string CellValuePrinter::print(ListItem& listItemCell)
{
    std::stringstream ss;
    ss << "[ ";
    if (!listItemCell.value().name().empty()) {
        ss << listItemCell.value().name() << " ";
    }
    ss << "]";

    return ss.str();
}

std::string CellValuePrinter::print(List& List)
{
    std::stringstream ss;
    ss << "[";
    bool isFirst = true;
    for (auto& item : List.items()) {
        if (isFirst) {
            isFirst = false;
        } else {
            ss << ",";
        }
        ss << " " << item.value().name();
    }
    ss << " ]";

    return ss.str();
}

std::string CellValuePrinter::print(Number& cell)
{
    std::stringstream ss;
    ss << "(Number) " << cell.value();

    return ss.str();
}

std::string CellValuePrinter::print(String& cell)
{
    std::stringstream ss;
    ss << "(String) " << cell.value();

    return ss.str();
}

std::string CellValuePrinter::print(hybrid::Color& cell)
{
    std::stringstream ss;
    ss << "(Color) [" << cell.color().m_red << ", " << cell.color().m_green << "" << cell.color().m_blue << "]";

    return ss.str();
}

std::string CellValuePrinter::print(hybrid::Pixel& cell)
{
    std::stringstream ss;
    ss << "(Pixel) [" << cell.color().m_red << ", " << cell.color().m_green << "" << cell.color().m_blue << "]";

    return ss.str();
}

std::string CellValuePrinter::print(hybrid::Sensor& cell)
{
    std::stringstream ss;
    ss << "(Sensor) " << cell.name();

    return ss.str();
}

// ============================================================================
std::string CellStructPrinter::print(Slot& cell)
{
    std::stringstream ss;
    if (!cell.name().empty()) {
        ss << cell.name() << ": ";
    }
    ss << printImpl(cell);

    return ss.str();
}

std::string CellStructPrinter::print(Type& cell)
{
    std::stringstream ss;
    if (!cell.name().empty()) {
        ss << cell.name() << ": ";
    }
    ss << printImpl(cell);

    return ss.str();
}

std::string CellStructPrinter::print(Object& cell)
{
    std::stringstream ss;
    if (!cell.name().empty()) {
        ss << cell.name() << ": ";
    }
    ss << printImpl(cell);

    return ss.str();
}

std::string CellStructPrinter::print(ListItem& cell)
{
    return printImpl(cell);
}

std::string CellStructPrinter::print(List& cell)
{
    return printImpl(cell);
}

std::string CellStructPrinter::print(Number& cell)
{
    return printImpl(cell);
}

std::string CellStructPrinter::print(String& cell)
{
    return printImpl(cell);
}

std::string CellStructPrinter::print(hybrid::Color& cell)
{
    return printImpl(cell);
}

std::string CellStructPrinter::print(hybrid::Pixel& cell)
{
    return printImpl(cell);
}

std::string CellStructPrinter::print(hybrid::Sensor& cell)
{
    return printImpl(cell);
}

std::string CellStructPrinter::printImpl(CellI& cell)
{
    std::stringstream ss;
    Type& type = cell.type();
    ss << "(" << type.name() << ") ID" << &cell << std::endl;
    for (auto& slotI : type.slots()) {
        const std::string& slotSlotName = slotI.first;
        Slot& slot                      = *slotI.second;

        if (!cell.has(slot.slotRole())) {
            continue;
        }
        CellValuePrinter valuePrinter;
        Type& slotType       = static_cast<Type&>(slot[Slot::slotSlotType()]);
        CellI& connectedCell = cell[slot.slotRole()];
        ss << "    +--(" << slotSlotName << ")--> (" << slotType.name() << ") ID" << &connectedCell << " // " << connectedCell.printAs(valuePrinter) << std::endl;
    }
    return ss.str();
}

} // namespace cells
} // namespace synth