#include "Cells.h"

#include <format>
#include <sstream>

#pragma warning(disable : 4996)
#include <utf8.h>

namespace synth {
namespace cells {

bool CellI::operator==(CellI& rhs)
{
    if (&type() != &rhs.type()) {
        return false;
    }
    for (auto& slot : type().slots()) {
        CellI& role      = slot.second->slotRole();
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

void Slot::accept(Visitor& visitor)
{
    visitor.visit(*this);
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

void Type::accept(Visitor& visitor)
{
    visitor.visit(*this);
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
T* ptr(T& obj) { return &obj; }

template <typename T>
T* ptr(T* obj) { return obj; }

template <typename T>
List::List(std::vector<T>& values) :
    m_listType("List<T>"),
    m_itemType("ListItem<T>")
{
    m_slotFirst = &m_listType.createSlot("first", m_itemType, data::first);
    m_slotLast  = &m_listType.createSlot("last", m_itemType, data::last);
    m_slotSize  = &m_listType.createSlot("size", Number::t(), data::size);

    Type& valueType = ptr(values.front())->type();
    m_itemType.createSlot("prev", m_itemType, data::previous);
    m_itemType.createSlot("next", m_itemType, data::next);
    m_itemType.createSlot("value", valueType, data::value);

    m_items.reserve(values.size());
    ListItem* prevListItem = nullptr;
    for (auto& valueT : values) {
        CellI* value   = ptr(valueT);
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
    Type& valueType = (*values.begin()).second->type();

    m_slotFirst = &m_listType.createSlot("first", m_itemType, data::first);
    m_slotLast  = &m_listType.createSlot("last", m_itemType, data::last);
    m_slotSize  = &m_listType.createSlot("size", Number::t(), data::size);
    m_listType.createSlot("valueType", valueType, data::size);

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

void List::createListType(Type& valueType, Type& listType, Type& listItemType)
{
    listType.createSlot("first", listItemType, data::first);
    listType.createSlot("last", listItemType, data::last);
    listType.createSlot("size", Number::t(), data::size);
    listType.createSlot("valueType", valueType, data::coding::objectType);

    listItemType.createSlot("prev", listItemType, data::previous);
    listItemType.createSlot("next", listItemType, data::next);
    listItemType.createSlot("value", valueType, data::value);
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
std::vector<Object> Digits::s_digits;

void Digits::staticInit()
{
    for (int i = 0; i < 10; ++i) {
        std::string digitName = "Digit_" + std::to_string(i);
        s_digits.emplace_back(digitName, type::Digit);
    }
}

// ============================================================================
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
        return t();
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
    return t();
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

void Number::staticInit()
{
    static Type digitListType("List<Digit>");
    static Type digitListItemType("ListItem<Digit>");
    List::createListType(type::Digit, digitListType, digitListItemType);
    s_slotValue = &t().createSlot("value", digitListType, data::value);
    s_slotSign  = &t().createSlot("sign", Number::t(), data::sign); // TODO
}

Type& Number::t()
{
    return type::Number;
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
std::map<char32_t, Object> Chars::s_characters;

void Chars::staticInit()
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
        return Object::emptyObject();
    }
}

Type& Chars::type()
{
    return type::Char;
}

void Chars::registerUnicodeBlock(char32_t from, char32_t to)
{
    for (char32_t unicodeValue = from; unicodeValue <= to; ++unicodeValue) {
        std::string characterName = std::format("Unicode_{:#04x}", (int)unicodeValue);
        s_characters.emplace(std::piecewise_construct,
                             std::forward_as_tuple(unicodeValue),
                             std::forward_as_tuple(characterName, type::Char));
    }
}

// ============================================================================
Slot* String::s_slotValue = nullptr;

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
        return t();
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
    return t();
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

void String::staticInit()
{
    static Type charListType("List<Char>");
    static Type charListItemType("ListItem<Char>");
    List::createListType(type::Char, charListType, charListItemType);
    s_slotValue = &t().createSlot("value", charListType, data::value);
}

Type& String::t()
{
    return type::String;
}

Slot& String::slotValue()
{
    return *s_slotValue;
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

Type& Pixel::t()
{
    return type::Pixel;
}

const input::Color& Pixel::color() const
{
    return m_inputColor;
}

// ============================================================================
Picture::Picture(input::Picture& picture) :
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
            Pixel& pixel  = m_pixels[currentIndex(x, y)];
            pixel.m_up    = upPixel(x, y);
            pixel.m_down  = downPixel(x, y);
            pixel.m_left  = leftPixel(x, y);
            pixel.m_right = rightPixel(x, y);
        }
    }
    m_pixelsList.reset(new List(m_pixels));
}

bool Picture::has(CellI& role)
{
    if (&role == &data::type || &role == &data::width || &role == &data::height || &role == &data::pixels) {
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
    if (&role == &data::type) {
        return t();
    }
    if (&role == &data::width) {
        return m_widthCell;
    }
    if (&role == &data::height) {
        return m_heightCell;
    }
    if (&role == &data::pixels) {
        return *m_pixelsList;
    }

    return Object::emptyObject();
}

Type& Picture::type()
{
    return t();
}

Type& Picture::pixelsType()
{
    static Type pixelListType("List<Pixel>");
    static Type pixelListItemType("ListItem<Pixel>");
    List::createListType(type::Pixel, pixelListType, pixelListItemType);
    return pixelListType;
}

void Picture::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string Picture::name() const
{
    return m_name;
}

Type& Picture::t()
{
    return type::Picture;
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
Same::Same(pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs) :
    m_output(output), m_lhs(lhs), m_rhs(rhs)
{
}

bool Same::has(CellI& role)
{
    if (&role == &data::type) {
        return true;
    }
    if (&role == &data::equation::lhs) {
        return true;
    }
    if (&role == &data::equation::rhs) {
        return true;
    }
    if (&role == &data::coding::output) {
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
    if (&m_lhs[data::coding::value] == &m_rhs[data::coding::value]) {
        m_output.set(data::coding::value, cells::data::boolean::true_);
    } else {
        m_output.set(data::coding::value, cells::data::boolean::false_);
    }
}

CellI& Same::operator[](CellI& role)
{
    if (&role == &data::type) {
        return t();
    }
    if (&role == &data::equation::lhs) {
        return m_lhs;
    }
    if (&role == &data::equation::rhs) {
        return m_rhs;
    }
    if (&role == &data::coding::output) {
        return m_output;
    }

    return Object::emptyObject();
}

Type& Same::type()
{
    return t();
}

void Same::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string Same::name() const
{
    return "Same";
}

Type& Same::t()
{
    return type::op::Same;
}

// ============================================================================
NotSame::NotSame(pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs) :
    m_output(output), m_lhs(lhs), m_rhs(rhs)
{
}

bool NotSame::has(CellI& role)
{
    if (&role == &data::type) {
        return true;
    }
    if (&role == &data::equation::lhs) {
        return true;
    }
    if (&role == &data::equation::rhs) {
        return true;
    }
    if (&role == &data::coding::output) {
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
    if (&m_lhs[data::coding::value] != &m_rhs[data::coding::value]) {
        m_output.set(data::coding::value, cells::data::boolean::true_);
    } else {
        m_output.set(data::coding::value, cells::data::boolean::false_);
    }
}

CellI& NotSame::operator[](CellI& role)
{
    if (&role == &data::type) {
        return t();
    }
    if (&role == &data::equation::lhs) {
        return m_lhs;
    }
    if (&role == &data::equation::rhs) {
        return m_rhs;
    }
    if (&role == &data::coding::output) {
        return m_output;
    }

    return Object::emptyObject();
}

Type& NotSame::type()
{
    return t();
}

void NotSame::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string NotSame::name() const
{
    return "NotSame";
}

Type& NotSame::t()
{
    return type::op::NotSame;
}

// ============================================================================
Equal::Equal(pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs) :
    m_output(output), m_lhs(lhs), m_rhs(rhs)
{
}

bool Equal::has(CellI& role)
{
    if (&role == &data::type) {
        return true;
    }
    if (&role == &data::equation::lhs) {
        return true;
    }
    if (&role == &data::equation::rhs) {
        return true;
    }
    if (&role == &data::coding::output) {
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
    if (m_lhs[data::coding::value] == m_rhs[data::coding::value]) {
        m_output.set(data::coding::value, cells::data::boolean::true_);
    } else {
        m_output.set(data::coding::value, cells::data::boolean::false_);
    }
}

CellI& Equal::operator[](CellI& role)
{
    if (&role == &data::type) {
        return t();
    }
    if (&role == &data::equation::lhs) {
        return m_lhs;
    }
    if (&role == &data::equation::rhs) {
        return m_rhs;
    }
    if (&role == &data::coding::output) {
        return m_output;
    }

    return Object::emptyObject();
}

Type& Equal::type()
{
    return t();
}

void Equal::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string Equal::name() const
{
    return "Equal";
}

Type& Equal::t()
{
    return type::op::Equal;
}

// ============================================================================
NotEqual::NotEqual(pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs) :
    m_output(output), m_lhs(lhs), m_rhs(rhs)
{
}

bool NotEqual::has(CellI& role)
{
    if (&role == &data::type) {
        return true;
    }
    if (&role == &data::equation::lhs) {
        return true;
    }
    if (&role == &data::equation::rhs) {
        return true;
    }
    if (&role == &data::coding::output) {
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
    if (m_lhs[data::coding::value] != m_rhs[data::coding::value]) {
        m_output.set(data::coding::value, cells::data::boolean::true_);
    } else {
        m_output.set(data::coding::value, cells::data::boolean::false_);
    }
}

CellI& NotEqual::operator[](CellI& role)
{
    if (&role == &data::type) {
        return t();
    }
    if (&role == &data::equation::lhs) {
        return m_lhs;
    }
    if (&role == &data::equation::rhs) {
        return m_rhs;
    }
    if (&role == &data::coding::output) {
        return m_output;
    }

    return Object::emptyObject();
}

Type& NotEqual::type()
{
    return t();
}

void NotEqual::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string NotEqual::name() const
{
    return "NotEqual";
}

Type& NotEqual::t()
{
    return type::op::NotEqual;
}

// ============================================================================
Has::Has(pipeline::Base& output, pipeline::Base& cell, pipeline::Base& role) :
    m_output(output), m_cell(cell), m_role(role)
{
}

bool Has::has(CellI& role)
{
    if (&role == &data::type) {
        return true;
    }
    if (&role == &data::coding::cell) {
        return true;
    }
    if (&role == &data::coding::role) {
        return true;
    }
    if (&role == &data::coding::output) {
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
    m_output.set(data::coding::value, m_cell.has(m_role) ? cells::data::boolean::true_ : cells::data::boolean::false_);
}

CellI& Has::operator[](CellI& role)
{
    if (&role == &data::type) {
        return t();
    }
    if (&role == &data::coding::cell) {
        return m_cell;
    }
    if (&role == &data::coding::role) {
        return m_role;
    }
    if (&role == &data::coding::output) {
        return m_output;
    }

    return Object::emptyObject();
}

Type& Has::type()
{
    return t();
}

void Has::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string Has::name() const
{
    return "Has";
}

Type& Has::t()
{
    return type::op::Has;
}

// ============================================================================
Get::Get(pipeline::Base& output, pipeline::Base& cell, pipeline::Base& role) :
    m_output(output), m_cell(cell), m_role(role)
{
}

bool Get::has(CellI& role)
{
    if (&role == &data::type) {
        return true;
    }
    if (&role == &data::coding::cell) {
        return true;
    }
    if (&role == &data::coding::role) {
        return true;
    }
    if (&role == &data::coding::output) {
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
    m_output.set(data::coding::value, m_cell[m_role]);
}

CellI& Get::operator[](CellI& role)
{
    if (&role == &data::type) {
        return t();
    }
    if (&role == &data::coding::cell) {
        return m_cell;
    }
    if (&role == &data::coding::role) {
        return m_role;
    }
    if (&role == &data::coding::output) {
        return m_output;
    }

    return Object::emptyObject();
}

Type& Get::type()
{
    return t();
}

void Get::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string Get::name() const
{
    return "Get";
}

Type& Get::t()
{
    return type::op::Get;
}

// ============================================================================
Set::Set(pipeline::Base& output, pipeline::Base& cell, pipeline::Base& role, pipeline::Base& value) :
    m_output(output), m_cell(cell), m_role(role), m_value(value)
{
}

bool Set::has(CellI& role)
{
    if (&role == &data::type) {
        return true;
    }
    if (&role == &data::coding::cell) {
        return true;
    }
    if (&role == &data::coding::role) {
        return true;
    }
    if (&role == &data::coding::value) {
        return true;
    }
    if (&role == &data::coding::output) {
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
    m_cell.set(m_role, m_value);
}

CellI& Set::operator[](CellI& role)
{
    if (&role == &data::type) {
        return t();
    }
    if (&role == &data::coding::cell) {
        return m_cell;
    }
    if (&role == &data::coding::role) {
        return m_role;
    }
    if (&role == &data::coding::value) {
        return m_value;
    }
    if (&role == &data::coding::output) {
        return m_output;
    }

    return Object::emptyObject();
}

Type& Set::type()
{
    return t();
}

void Set::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string Set::name() const
{
    return "Set";
}

Type& Set::t()
{
    return type::op::Set;
}
namespace logic {

// ============================================================================
And::And(pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs) :
    m_output(output), m_lhs(lhs), m_rhs(rhs)
{
}

bool And::has(CellI& role)
{
    if (&role == &data::type) {
        return true;
    }
    if (&role == &data::equation::lhs) {
        return true;
    }
    if (&role == &data::equation::rhs) {
        return true;
    }
    if (&role == &data::coding::output) {
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
    bool lhs = m_lhs[data::coding::value] == cells::data::boolean::true_;
    bool rhs = m_rhs[data::coding::value] == cells::data::boolean::true_;
    if (lhs && rhs) {
        m_output.set(data::coding::value, cells::data::boolean::true_);
    } else {
        m_output.set(data::coding::value, cells::data::boolean::false_);
    }
}

CellI& And::operator[](CellI& role)
{
    if (&role == &data::type) {
        return t();
    }
    if (&role == &data::equation::lhs) {
        return m_lhs;
    }
    if (&role == &data::equation::rhs) {
        return m_rhs;
    }
    if (&role == &data::coding::output) {
        return m_output;
    }

    return Object::emptyObject();
}

Type& And::type()
{
    return t();
}

void And::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string And::name() const
{
    return "And";
}

Type& And::t()
{
    return type::op::logic::And;
}

// ============================================================================
Or::Or(pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs) :
    m_output(output), m_lhs(lhs), m_rhs(rhs)
{
}

bool Or::has(CellI& role)
{
    if (&role == &data::type) {
        return true;
    }
    if (&role == &data::equation::lhs) {
        return true;
    }
    if (&role == &data::equation::rhs) {
        return true;
    }
    if (&role == &data::coding::output) {
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
    bool lhs = m_lhs[data::coding::value] == cells::data::boolean::true_;
    bool rhs = m_rhs[data::coding::value] == cells::data::boolean::true_;
    if (lhs || rhs) {
        m_output.set(data::coding::value, cells::data::boolean::true_);
    } else {
        m_output.set(data::coding::value, cells::data::boolean::false_);
    }
}

CellI& Or::operator[](CellI& role)
{
    if (&role == &data::type) {
        return t();
    }
    if (&role == &data::equation::lhs) {
        return m_lhs;
    }
    if (&role == &data::equation::rhs) {
        return m_rhs;
    }
    if (&role == &data::coding::output) {
        return m_output;
    }

    return Object::emptyObject();
}

Type& Or::type()
{
    return t();
}

void Or::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string Or::name() const
{
    return "Or";
}

Type& Or::t()
{
    return type::op::logic::Or;
}

// ============================================================================
Not::Not(pipeline::Base& output, pipeline::Base& input) :
    m_output(output), m_input(input)
{
}

bool Not::has(CellI& role)
{
    if (&role == &data::type) {
        return true;
    }
    if (&role == &data::coding::input) {
        return true;
    }
    if (&role == &data::coding::output) {
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
    bool input = m_input[data::coding::value] == cells::data::boolean::true_;
    if (!input) {
        m_output.set(data::coding::value, cells::data::boolean::true_);
    } else {
        m_output.set(data::coding::value, cells::data::boolean::false_);
    }
}

CellI& Not::operator[](CellI& role)
{
    if (&role == &data::type) {
        return t();
    }
    if (&role == &data::coding::input) {
        return m_input;
    }
    if (&role == &data::coding::output) {
        return m_output;
    }

    return Object::emptyObject();
}

Type& Not::type()
{
    return t();
}

void Not::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string Not::name() const
{
    return "Not";
}

Type& Not::t()
{
    return type::op::logic::Not;
}

} // namespace logic
namespace math {

// ============================================================================
Add::Add(pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs) :
    m_output(output), m_lhs(lhs), m_rhs(rhs)
{
}

bool Add::has(CellI& role)
{
    if (&role == &data::type) {
        return true;
    }
    if (&role == &data::equation::lhs) {
        return true;
    }
    if (&role == &data::equation::rhs) {
        return true;
    }
    if (&role == &data::coding::output) {
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
    int lhs = static_cast<Number&>(m_lhs[data::coding::value]).value();
    int rhs = static_cast<Number&>(m_rhs[data::coding::value]).value();
    m_output.set(data::coding::value, Numbers::get(lhs + rhs));
}

CellI& Add::operator[](CellI& role)
{
    if (&role == &data::type) {
        return t();
    }
    if (&role == &data::equation::lhs) {
        return m_lhs;
    }
    if (&role == &data::equation::rhs) {
        return m_rhs;
    }
    if (&role == &data::coding::output) {
        return m_output;
    }

    return Object::emptyObject();
}

Type& Add::type()
{
    return t();
}

void Add::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string Add::name() const
{
    return "Add";
}

Type& Add::t()
{
    return type::op::math::Add;
}

// ============================================================================
Subtract::Subtract(pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs) :
    m_output(output), m_lhs(lhs), m_rhs(rhs)
{
}

bool Subtract::has(CellI& role)
{
    if (&role == &data::type) {
        return true;
    }
    if (&role == &data::equation::lhs) {
        return true;
    }
    if (&role == &data::equation::rhs) {
        return true;
    }
    if (&role == &data::coding::output) {
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
    int lhs = static_cast<Number&>(m_lhs[data::coding::value]).value();
    int rhs = static_cast<Number&>(m_rhs[data::coding::value]).value();
    m_output.set(data::coding::value, Numbers::get(lhs - rhs));
}

CellI& Subtract::operator[](CellI& role)
{
    if (&role == &data::type) {
        return t();
    }
    if (&role == &data::equation::lhs) {
        return m_lhs;
    }
    if (&role == &data::equation::rhs) {
        return m_rhs;
    }
    if (&role == &data::coding::output) {
        return m_output;
    }

    return Object::emptyObject();
}

Type& Subtract::type()
{
    return t();
}

void Subtract::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string Subtract::name() const
{
    return "Subtract";
}

Type& Subtract::t()
{
    return type::op::math::Subtract;
}

// ============================================================================
Multiply::Multiply(pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs) :
    m_output(output), m_lhs(lhs), m_rhs(rhs)
{
}

bool Multiply::has(CellI& role)
{
    if (&role == &data::type) {
        return true;
    }
    if (&role == &data::equation::lhs) {
        return true;
    }
    if (&role == &data::equation::rhs) {
        return true;
    }
    if (&role == &data::coding::output) {
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
    int lhs = static_cast<Number&>(m_lhs[data::coding::value]).value();
    int rhs = static_cast<Number&>(m_rhs[data::coding::value]).value();
    m_output.set(data::coding::value, Numbers::get(lhs * rhs));
}

CellI& Multiply::operator[](CellI& role)
{
    if (&role == &data::type) {
        return t();
    }
    if (&role == &data::equation::lhs) {
        return m_lhs;
    }
    if (&role == &data::equation::rhs) {
        return m_rhs;
    }
    if (&role == &data::coding::output) {
        return m_output;
    }

    return Object::emptyObject();
}

Type& Multiply::type()
{
    return t();
}

void Multiply::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string Multiply::name() const
{
    return "Multiply";
}

Type& Multiply::t()
{
    return type::op::math::Multiply;
}

// ============================================================================
Divide::Divide(pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs) :
    m_output(output), m_lhs(lhs), m_rhs(rhs)
{
}

bool Divide::has(CellI& role)
{
    if (&role == &data::type) {
        return true;
    }
    if (&role == &data::equation::lhs) {
        return true;
    }
    if (&role == &data::equation::rhs) {
        return true;
    }
    if (&role == &data::coding::output) {
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
    int lhs = static_cast<Number&>(m_lhs[data::coding::value]).value();
    int rhs = static_cast<Number&>(m_rhs[data::coding::value]).value();
    m_output.set(data::coding::value, Numbers::get(lhs / rhs));
}

CellI& Divide::operator[](CellI& role)
{
    if (&role == &data::type) {
        return t();
    }
    if (&role == &data::equation::lhs) {
        return m_lhs;
    }
    if (&role == &data::equation::rhs) {
        return m_rhs;
    }
    if (&role == &data::coding::output) {
        return m_output;
    }

    return Object::emptyObject();
}

Type& Divide::type()
{
    return t();
}

void Divide::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string Divide::name() const
{
    return "Divide";
}

Type& Divide::t()
{
    return type::op::math::Divide;
}

// ============================================================================
LessThan::LessThan(pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs) :
    m_output(output), m_lhs(lhs), m_rhs(rhs)
{
}

bool LessThan::has(CellI& role)
{
    if (&role == &data::type) {
        return true;
    }
    if (&role == &data::equation::lhs) {
        return true;
    }
    if (&role == &data::equation::rhs) {
        return true;
    }
    if (&role == &data::coding::output) {
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
    int lhs = static_cast<Number&>(m_lhs[data::coding::value]).value();
    int rhs = static_cast<Number&>(m_rhs[data::coding::value]).value();
    m_output.set(data::coding::value, lhs < rhs ? data::boolean::true_ : data::boolean::false_);
}

CellI& LessThan::operator[](CellI& role)
{
    if (&role == &data::type) {
        return t();
    }
    if (&role == &data::equation::lhs) {
        return m_lhs;
    }
    if (&role == &data::equation::rhs) {
        return m_rhs;
    }
    if (&role == &data::coding::output) {
        return m_output;
    }

    return Object::emptyObject();
}

Type& LessThan::type()
{
    return t();
}

void LessThan::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string LessThan::name() const
{
    return "LessThan";
}

Type& LessThan::t()
{
    return type::op::math::LessThan;
}

// ============================================================================
GreaterThan::GreaterThan(pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs) :
    m_output(output), m_lhs(lhs), m_rhs(rhs)
{
}

bool GreaterThan::has(CellI& role)
{
    if (&role == &data::type) {
        return true;
    }
    if (&role == &data::equation::lhs) {
        return true;
    }
    if (&role == &data::equation::rhs) {
        return true;
    }
    if (&role == &data::coding::output) {
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
    int lhs = static_cast<Number&>(m_lhs[data::coding::value]).value();
    int rhs = static_cast<Number&>(m_rhs[data::coding::value]).value();
    m_output.set(data::coding::value, lhs > rhs ? data::boolean::true_ : data::boolean::false_);
}

CellI& GreaterThan::operator[](CellI& role)
{
    if (&role == &data::type) {
        return t();
    }
    if (&role == &data::equation::lhs) {
        return m_lhs;
    }
    if (&role == &data::equation::rhs) {
        return m_rhs;
    }
    if (&role == &data::coding::output) {
        return m_output;
    }

    return Object::emptyObject();
}

Type& GreaterThan::type()
{
    return t();
}

void GreaterThan::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string GreaterThan::name() const
{
    return "GreaterThan";
}

Type& GreaterThan::t()
{
    return type::op::math::GreaterThan;
}

} // namespace math
} // namespace op
namespace pipeline {
// ============================================================================
void Base::addNext(Base& cell)
{
    m_next = &cell;
}

// ============================================================================
Start::Start(CellI& input, const std::string& name) :
    m_name(name), m_value(input)
{
}

bool Start::has(CellI& role)
{
    if (&role == &data::type) {
        return true;
    }
    if (&role == &data::next) {
        return m_next;
    }
    if (&role == &data::coding::value) {
        return true;
    }

    return false;
}

void Start::set(CellI& role, CellI& value)
{
}

void Start::operator()()
{
    if (m_next) {
        (*m_next)();
    }
}

CellI& Start::operator[](CellI& role)
{
    if (&role == &data::type) {
        return t();
    }
    if (&role == &data::next && m_next) {
        return *m_next;
    }
    if (&role == &data::coding::value) {
        return m_value;
    }

    return Object::emptyObject();
}

Type& Start::type()
{
    return t();
}

void Start::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string Start::name() const
{
    return m_name;
}

Type& Start::t()
{
    return type::pipeline::Start;
}

// ============================================================================
Fork::Fork(Base& input, const std::string& name) :
    m_input(input), m_name(name)
{
    input.addNext(*this);
}

bool Fork::has(CellI& role)
{
    if (&role == &data::type) {
        return true;
    }
    if (&role == &data::next) {
        return m_next;
    }
    if (&role == &data::coding::value && m_value) {
        return true;
    }
    if (&role == &data::coding::branch && m_branch) {
        return true;
    }

    return false;
}

void Fork::set(CellI& role, CellI& value)
{
}

void Fork::operator()()
{
    m_value = &m_input[data::coding::value];
    if (m_branch) {
        (*m_branch)();
    }
    if (m_next) {
        (*m_next)();
    }
}

CellI& Fork::operator[](CellI& role)
{
    if (&role == &data::type) {
        return t();
    }
    if (&role == &data::next && m_next) {
        return *m_next;
    }
    if (&role == &data::coding::value && m_value) {
        return *m_value;
    }
    if (&role == &data::coding::branch && m_branch) {
        return *m_branch;
    }

    return Object::emptyObject();
}

Type& Fork::type()
{
    return t();
}

void Fork::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string Fork::name() const
{
    return m_name;
}

Type& Fork::t()
{
    return type::pipeline::Fork;
}

void Fork::addBranch(Base& cell)
{
    m_branch = &cell;
}

// ============================================================================
Empty::Empty(const std::string& name) :
    m_name(name)
{
}

bool Empty::has(CellI& role)
{
    if (&role == &data::type) {
        return true;
    }
    if (&role == &data::next) {
        return m_next;
    }

    return false;
}

void Empty::set(CellI& role, CellI& value)
{
}

void Empty::operator()()
{
    if (m_next) {
        (*m_next)();
    }
}

CellI& Empty::operator[](CellI& role)
{
    if (&role == &data::type) {
        return t();
    }
    if (&role == &data::next && m_next) {
        return *m_next;
    }

    return Object::emptyObject();
}

Type& Empty::type()
{
    return t();
}

void Empty::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string Empty::name() const
{
    return m_name;
}

Type& Empty::t()
{
    return type::pipeline::Empty;
}

// ============================================================================
New::New(Type& objectType, const std::string& name) :
    m_name(name), m_objectType(objectType)
{
}

bool New::has(CellI& role)
{
    if (&role == &data::type) {
        return true;
    }
    if (&role == &data::next) {
        return m_next;
    }
    if (&role == &data::coding::objectType) {
        return true;
    }

    return false;
}

void New::set(CellI& role, CellI& value)
{
    if (&role == &data::coding::objectType && &value.type() == &Type::t()) {
        m_value = &value;
    }
}

void New::operator()()
{
    if (&m_objectType == &Type::t()) {
        m_value = new Type();
    }
    if (&m_objectType == &Number::t()) {
        m_value = new Number();
    }
    if (&m_objectType == &String::t()) {
        m_value = new String();
    }
    if (m_next) {
        (*m_next)();
    }
}

CellI& New::operator[](CellI& role)
{
    if (&role == &data::type) {
        return t();
    }
    if (&role == &data::next && m_next) {
        return *m_next;
    }
    if (&role == &data::coding::objectType) {
        return m_objectType;
    }

    return Object::emptyObject();
}

Type& New::type()
{
    return t();
}

void New::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string New::name() const
{
    return m_name;
}

Type& New::t()
{
    return type::pipeline::New;
}

// ============================================================================
Delete::Delete(Base& input, const std::string& name) :
    m_name(name), m_input(&input)
{
}

bool Delete::has(CellI& role)
{
    if (&role == &data::type) {
        return true;
    }
    if (&role == &data::coding::input) {
        return true;
    }

    return false;
}

void Delete::set(CellI& role, CellI& value)
{
    if (&role == &data::coding::input) {
        m_input = &value;
    }
}

void Delete::operator()()
{
    CellI* cell = &(*m_input)[data::coding::value];
    delete cell;
}

CellI& Delete::operator[](CellI& role)
{
    if (&role == &data::type) {
        return t();
    }
    if (&role == &data::coding::input && m_input) {
        return *m_input;
    }

    return Object::emptyObject();
}

Type& Delete::type()
{
    return t();
}

void Delete::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string Delete::name() const
{
    return m_name;
}

Type& Delete::t()
{
    return type::pipeline::Delete;
}

// ============================================================================
bool Node::has(CellI& role)
{
    if (&role == &data::type) {
        return true;
    }
    if (&role == &data::coding::input) {
        return true;
    }
    if (&role == &data::coding::op) {
        return true;
    }
    if (&role == &data::next) {
        return m_next;
    }
    if (&role == &data::coding::value) {
        return m_value;
    }

    return false;
}

void Node::set(CellI& role, CellI& value)
{
    if (&role == &data::coding::value) {
        m_value = &value;
    }
}

void Node::operator()()
{
    m_value = &(*m_input)[data::coding::value];
    (*m_op)();
    if (m_next) {
        (*m_next)();
    }
}

CellI& Node::operator[](CellI& role)
{
    if (&role == &data::type) {
        return t();
    }
    if (&role == &data::coding::input && m_input) {
        return *m_input;
    }
    if (&role == &data::coding::op) {
        return *m_op;
    }
    if (&role == &data::next && m_next) {
        return *m_next;
    }
    if (&role == &data::coding::value && m_value) {
        return *m_value;
    }

    return Object::emptyObject();
}

Type& Node::type()
{
    return t();
}

void Node::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string Node::name() const
{
    return m_name;
}

Type& Node::t()
{
    return type::pipeline::Node;
}

// ============================================================================
IfThen::IfThen(Base& inputNode, const std::string& name) :
    m_name(name), m_input(inputNode)
{
}

bool IfThen::has(CellI& role)
{
    if (&role == &data::type) {
        return true;
    }
    if (&role == &data::next) {
        return m_next;
    }
    if (&role == &data::coding::value) {
        return m_value;
    }
    if (&role == &data::coding::then) {
        return m_thenBranch;
    }
    if (&role == &data::coding::else_) {
        return m_elseBranch;
    }

    return false;
}

void IfThen::set(CellI& role, CellI& value)
{
    if (&role == &data::coding::value) {
        m_value = &value;
    }
}

void IfThen::operator()()
{
    m_value = &m_input[data::coding::value];
    if (m_value == &data::boolean::true_) {
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
    if (&role == &data::type) {
        return t();
    }
    if (&role == &data::coding::then && m_thenBranch) {
        return *m_thenBranch;
    }
    if (&role == &data::coding::else_ && m_elseBranch) {
        return *m_elseBranch;
    }
    if (&role == &data::coding::value && m_value) {
        return *m_value;
    }
    if (&role == &data::next && m_next) {
        return *m_next;
    }

    return Object::emptyObject();
}

Type& IfThen::type()
{
    return t();
}

void IfThen::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string IfThen::name() const
{
    return m_name;
}

Type& IfThen::t()
{
    return type::pipeline::IfThen;
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
DoWhile::DoWhile(Base& inputNode, const std::string& name) :
    m_name(name), m_input(inputNode)
{
}

bool DoWhile::has(CellI& role)
{
    if (&role == &data::type) {
        return true;
    }
    if (&role == &data::next) {
        return m_next;
    }
    if (&role == &data::coding::condition) {
        return m_condition;
    }
    if (&role == &data::coding::statement) {
        return m_statement;
    }

    return false;
}

void DoWhile::set(CellI& role, CellI& value)
{
    if (&role == &data::coding::value) {
        m_value = &value;
    }
}

void DoWhile::operator()()
{
    do {
        (*m_statement)();
        (*m_condition)();
    } while (m_value == &data::boolean::true_);

    if (m_next) {
        (*m_next)();
    }
}

CellI& DoWhile::operator[](CellI& role)
{
    if (&role == &data::type) {
        return t();
    }
    if (&role == &data::coding::condition && m_condition) {
        return *m_condition;
    }
    if (&role == &data::coding::statement && m_statement) {
        return *m_statement;
    }
    if (&role == &data::coding::value && m_value) {
        return *m_value;
    }
    if (&role == &data::next && m_next) {
        return *m_next;
    }

    return Object::emptyObject();
}

Type& DoWhile::type()
{
    return t();
}

void DoWhile::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string DoWhile::name() const
{
    return m_name;
}

Type& DoWhile::t()
{
    return type::pipeline::DoWhile;
}

// ============================================================================
While::While(Base& inputNode, const std::string& name) :
    m_name(name), m_input(inputNode)
{
}

bool While::has(CellI& role)
{
    if (&role == &data::type) {
        return true;
    }
    if (&role == &data::next) {
        return m_next;
    }
    if (&role == &data::coding::condition) {
        return m_condition;
    }
    if (&role == &data::coding::statement) {
        return m_statement;
    }

    return false;
}

void While::set(CellI& role, CellI& value)
{
    if (&role == &data::coding::value) {
        m_value = &value;
    }
}

void While::operator()()
{
    (*m_condition)();
    while (m_value == &data::boolean::true_) {
        (*m_statement)();
        (*m_condition)();
    };

    if (m_next) {
        (*m_next)();
    }
}

CellI& While::operator[](CellI& role)
{
    if (&role == &data::type) {
        return t();
    }
    if (&role == &data::coding::condition && m_condition) {
        return *m_condition;
    }
    if (&role == &data::coding::statement && m_statement) {
        return *m_statement;
    }
    if (&role == &data::coding::value && m_value) {
        return *m_value;
    }
    if (&role == &data::next && m_next) {
        return *m_next;
    }

    return Object::emptyObject();
}

Type& While::type()
{
    return t();
}

void While::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

std::string While::name() const
{
    return m_name;
}

Type& While::t()
{
    return type::pipeline::While;
}

} // namespace pipeline
} // namespace control

// ============================================================================
namespace type {
Type Boolean("Boolean");
Type Digit("Digit");
Type Number("Number");
Type Char("Char");
Type String("String");
Type Color("Color");
Type Pixel("Pixel");
Type Picture("Picture");

namespace op {
Type Base("Base");
Type Same("Same");
Type NotSame("NotSame");
Type Equal("Equal");
Type NotEqual("NotEqual");

Type New("New");
Type Delete("Delete");

Type Has("Has");
Type Get("Get");
Type Set("Set");

namespace logic {
Type And("And");
Type Or("Or");
Type Not("Not");
} // namespace logic

namespace math {
Type Add("Add");
Type Subtract("Subtract");
Type Multiply("Multiply");
Type Divide("Divide");
Type LessThan("LessThan");
Type GreaterThan("GreaterThan");
} // namespace math
} // namespace op

namespace pipeline {
Type Base("Base");
Type Start("Start");
Type Fork("Fork");
Type Empty("Empty");
Type New("New");
Type Delete("Delete");
Type Node("Node");
Type IfThen("IfThen");
Type DoWhile("DoWhile");
Type While("While");
} // namespace pipeline

static void staticInit()
{
    Color.addSlots(
        { { "red", Number::t(), data::colors::red },
          { "green", Number::t(), data::colors::green },
          { "blue", Number::t(), data::colors::blue } });

    Pixel.addSlots(
        { { "up", hybrid::Pixel::t(), data::directions::up },
          { "down", hybrid::Pixel::t(), data::directions::down },
          { "left", hybrid::Pixel::t(), data::directions::left },
          { "right", hybrid::Pixel::t(), data::directions::right },
          { "x", Number::t(), data::coordinates::x },
          { "y", Number::t(), data::coordinates::y } });

    Picture.addSlots(
        { { "width", Number::t(), data::width },
          { "height", Number::t(), data::height },
          { "pixels", hybrid::Picture::pixelsType(), data::pixels } });

    op::Same.addSlots({ { "lhs", type::pipeline::Base, data::equation::lhs },
                        { "rhs", type::pipeline::Base, data::equation::rhs },
                        { "output", type::pipeline::Base, data::coding::output } });

    op::NotSame.addSlots({ { "lhs", type::pipeline::Base, data::equation::lhs },
                           { "rhs", type::pipeline::Base, data::equation::rhs },
                           { "output", type::pipeline::Base, data::coding::output } });

    op::Equal.addSlots({ { "lhs", type::pipeline::Base, data::equation::lhs },
                         { "rhs", type::pipeline::Base, data::equation::rhs },
                         { "output", type::pipeline::Base, data::coding::output } });

    op::NotEqual.addSlots({ { "lhs", type::pipeline::Base, data::equation::lhs },
                            { "rhs", type::pipeline::Base, data::equation::rhs },
                            { "output", type::pipeline::Base, data::coding::output } });

    pipeline::Node.addSlots({ { "input", type::pipeline::Base, data::coding::input },
                              { "next", type::pipeline::Base, data::next },
                              { "op", type::op::Base, data::coding::op },
                              { "value", type::pipeline::Base, data::coding::value } });
}

} // namespace type
namespace data {
Object slotType(Type::anyType());
Object slotName(Type::anyType());
Object slotRole(Type::anyType());

Object first(Type::anyType());
Object last(Type::anyType());
Object previous(Type::anyType());
Object next(Type::anyType());

Object pixels(Type::anyType());

namespace coding {
Object argument(Type::anyType());
Object branch(Type::anyType());
Object cell(Type::anyType());
Object condition(Type::anyType());
Object else_(Type::anyType());
Object input(Type::anyType());
Object name(Type::anyType());
Object objectType(Type::anyType());
Object op(Type::anyType());
Object output(Type::anyType());
Object result(Type::anyType());
Object role(Type::anyType());
Object statement(Type::anyType());
Object then(Type::anyType());
Object value(Type::anyType());
} // namespace coding

namespace equation {
Object lhs(Type::anyType());
Object rhs(Type::anyType());
} // namespace equation

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

namespace boolean {
Object true_("true", type::Boolean);
Object false_("false", type::Boolean);
} // namespace boolean

Object width(Type::anyType());
Object height(Type::anyType());

Object size(Type::anyType());
Object value(Type::anyType());
Object type(Type::anyType());
Object slots(Type::anyType());
Object sign(Type::anyType());
} // namespace data

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

    type::staticInit();
}

} // namespace cells
} // namespace synth