#pragma once
#include <iostream>
#include <map>
#include <vector>

#include "app/Picture.h"

namespace synth {
namespace cells {

// ============================================================================
class Type;
class Visitor;
class CellI
{
public:
    virtual bool has(CellI& role)               = 0;
    virtual void set(CellI& role, CellI& value) = 0;
    virtual void operator()()                   = 0;
    virtual CellI& operator[](CellI& role)      = 0;
    virtual Type& type()                        = 0;
    virtual void accept(Visitor& visitor)       = 0;
    virtual std::string name() const            = 0;
    bool operator==(CellI& rhs);
    bool operator!=(CellI& rhs);
};

// ============================================================================
class String;
class SlotRef // Only exists to bypass the non-movable std::initalizer_list limitations
{
public:
    SlotRef(const std::string& name, Type& type, CellI& role);

    const std::string& m_name;
    Type& m_type;
    CellI& m_role;
};

class Slot : public CellI
{
public:
    Slot(const std::string& name, Type& type, CellI& role);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static void staticInit();
    static void staticInitMembers();
    static Type& t();
    static Slot& slotSlotType();
    static Slot& slotSlotName();
    static Slot& slotSlotRole();

    Type& slotType();
    CellI& slotRole();

protected:
    static std::unique_ptr<Type> s_type;
    static Slot* s_slotSlotType;
    static Slot* s_slotSlotName;
    static Slot* s_slotSlotRole;
    std::string m_name;
    std::unique_ptr<String> m_slotNameString;
    Type& m_slotType;
    CellI& m_slotRole;
};

// ============================================================================
class List;
class Type : public CellI
{
public:
    explicit Type(const std::string& name = "Type");
    Type(const std::string& name, std::initializer_list<SlotRef> slots);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static void staticInit();
    static void staticInitMembers();
    void addSlots(std::initializer_list<SlotRef> slots);
    Slot& createSlot(const std::string& name, Type& type, CellI& role);
    void referenceSlot(const std::string& name, Slot& slotCell);

    bool has(const std::string& name) const;

    bool hasSlot(CellI& role);
    bool hasSlot(const std::string& name);
    Slot& getSlot(CellI& role);
    Slot& getSlot(const std::string& name);

    std::map<std::string, Slot*>& slots();
    static Type& t();
    static Slot& slotType();
    static Slot& slotSlots();
    static Type& anyType();

protected:
    void registerTypeSlot();

    static std::unique_ptr<Type> s_type;
    static Slot* s_slotType;
    static Slot* s_slotSlots;
    static std::unique_ptr<Type> s_anyType;
    std::map<std::string, Slot> m_slots;
    std::map<std::string, Slot*> m_slotRefs;
    std::map<CellI*, Slot*> m_roles;
    std::string m_name;
    std::unique_ptr<List> m_slotsList;
};

// ============================================================================
class Object : public CellI
{
public:
    Object(Type& classCell);
    Object(const std::string& name, Type& classCell);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    std::map<CellI*, CellI*>& roles();

    static void staticInit();
    static Object& emptyObject();

protected:
    static std::unique_ptr<Object> s_emptyObject;
    std::string m_name;
    Type& m_type;
    std::map<CellI*, CellI*> m_roles;
};

// ============================================================================
class ListItem : public CellI
{
public:
    ListItem(Type& type);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    CellI& prev();
    void prev(ListItem* p);

    CellI& next();
    void next(ListItem* n);

    CellI& value();
    void value(CellI* v);

    Slot& slotPrev();
    Slot& slotNext();
    Slot& slotValue();

protected:
    Slot* m_slotPrev;
    Slot* m_slotNext;
    Slot* m_slotValue;

    Type& m_type;
    CellI* m_prev  = nullptr;
    CellI* m_next  = nullptr;
    CellI* m_value = nullptr;
};

// ============================================================================
class List : public CellI
{
public:
    template <typename T>
    List(std::vector<T>& values);

    template <typename T>
    List(std::map<std::string, T>& values);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    std::vector<ListItem>& items();

    Slot& slotFirst();
    Slot& slotLast();
    Slot& slotSize();

    static void createListType(Type& valueType, Type& listType, Type& listItemType);

protected:
    Slot* m_slotFirst;
    Slot* m_slotLast;
    Slot* m_slotSize;
    Type m_listType;
    Type m_itemType;
    std::vector<ListItem> m_items;
};

// ============================================================================
class Digits
{
public:
    static void staticInit();
    static std::vector<Object> s_digits;
};

// ============================================================================
class Number : public CellI
{
public:
    explicit Number(int value = 0);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    int value() const;

    static void staticInit();
    static Type& t();
    static Slot& slotSign();
    static Slot& slotValue();

protected:
    void calculateDigits();

    static Slot* s_slotSign;
    static Slot* s_slotValue;

    int m_value;
    std::vector<Object*> m_digits;
    std::unique_ptr<List> m_digitsList;
};

// ============================================================================
class Numbers
{
public:
    static Number& get(int number);

protected:
    static std::map<int, Number> s_numbers;
};

// ============================================================================
class Chars
{
public:
    static void staticInit();
    static Object& get(char32_t utf32Char);
    static Type& type();

protected:
    static void registerUnicodeBlock(char32_t from, char32_t to);
    static std::map<char32_t, Object> s_characters;
};

// ============================================================================
class String : public CellI
{
public:
    explicit String(const std::string& str = "");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    const std::string& value() const;

    static void staticInit();
    static Type& t();
    static Slot& slotValue();

protected:
    void calculateCharacters();

    static Slot* s_slotValue;

    std::string m_value;
    std::vector<Object*> m_characters;
    std::unique_ptr<List> m_charactersList;
};

namespace hybrid {

// ============================================================================
class Color : public CellI
{
public:
    Color(const input::Color& inputColor);
    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

    const input::Color& color() const;

private:
    const input::Color& m_inputColor;
};

// ============================================================================
class Pixel : public CellI
{
public:
    Pixel(int x, int y, const input::Color& inputColor);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

    const input::Color& color() const;

    Pixel* m_up    = nullptr;
    Pixel* m_down  = nullptr;
    Pixel* m_left  = nullptr;
    Pixel* m_right = nullptr;
    Number& m_x;
    Number& m_y;

private:
    Color m_color;
    const input::Color& m_inputColor;
};

// ============================================================================
class Picture : public CellI
{
public:
    Picture(input::Picture& screen);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();
    static Type& pixelsType();

    Pixel& getPixel(int x, int y);
    const Pixel& getPixel(int x, int y) const;

    int currentIndex(int x, int y) const;
    bool isInRange(int x, int y) const;
    Pixel* upPixel(int x, int y);
    Pixel* downPixel(int x, int y);
    Pixel* leftPixel(int x, int y);
    Pixel* rightPixel(int x, int y);
    std::vector<Pixel>& pixels();

    int width() const;
    int height() const;

protected:
    std::string m_name;
    int m_width;
    int m_height;
    Number& m_widthCell;
    Number& m_heightCell;
    std::vector<Pixel> m_pixels;
    std::unique_ptr<List> m_pixelsList;
};

} // namespace hybrid

namespace control {
namespace pipeline {
class Base;
}
namespace op {

// ============================================================================
class Same : public CellI
{
public:
    Same(pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

protected:
    pipeline::Base& m_output;
    pipeline::Base& m_lhs;
    pipeline::Base& m_rhs;
};

// ============================================================================
class NotSame : public CellI
{
public:
    NotSame(pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

protected:
    pipeline::Base& m_output;
    pipeline::Base& m_lhs;
    pipeline::Base& m_rhs;
};

// ============================================================================
class Equal : public CellI
{
public:
    Equal(pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

protected:
    pipeline::Base& m_output;
    pipeline::Base& m_lhs;
    pipeline::Base& m_rhs;
};

// ============================================================================
class NotEqual : public CellI
{
public:
    NotEqual(pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

protected:
    pipeline::Base& m_output;
    pipeline::Base& m_lhs;
    pipeline::Base& m_rhs;
};

// ============================================================================
class Has : public CellI
{
public:
    Has(pipeline::Base& output, pipeline::Base& cell, pipeline::Base& role);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

protected:
    pipeline::Base& m_output;
    pipeline::Base& m_cell;
    pipeline::Base& m_role;
};

// ============================================================================
class Get : public CellI
{
public:
    Get(pipeline::Base& output, pipeline::Base& cell, pipeline::Base& role);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

protected:
    pipeline::Base& m_output;
    pipeline::Base& m_cell;
    pipeline::Base& m_role;
};

// ============================================================================
class Set : public CellI
{
public:
    Set(pipeline::Base& output, pipeline::Base& cell, pipeline::Base& role, pipeline::Base& value);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

protected:
    pipeline::Base& m_output;
    pipeline::Base& m_cell;
    pipeline::Base& m_role;
    pipeline::Base& m_value;
};

namespace logic {
// ============================================================================
class And : public CellI
{
public:
    And(pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

protected:
    pipeline::Base& m_output;
    pipeline::Base& m_lhs;
    pipeline::Base& m_rhs;
};

// ============================================================================
class Or : public CellI
{
public:
    Or(pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

protected:
    pipeline::Base& m_output;
    pipeline::Base& m_lhs;
    pipeline::Base& m_rhs;
};

// ============================================================================
class Not : public CellI
{
public:
    Not(pipeline::Base& output, pipeline::Base& input);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

protected:
    pipeline::Base& m_output;
    pipeline::Base& m_input;
};

} // namespace logic

namespace math {

// ============================================================================
class Add : public CellI
{
public:
    Add(pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

protected:
    pipeline::Base& m_output;
    pipeline::Base& m_lhs;
    pipeline::Base& m_rhs;
};

// ============================================================================
class Subtract : public CellI
{
public:
    Subtract(pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

protected:
    pipeline::Base& m_output;
    pipeline::Base& m_lhs;
    pipeline::Base& m_rhs;
};

// ============================================================================
class Multiply : public CellI
{
public:
    Multiply(pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

protected:
    pipeline::Base& m_output;
    pipeline::Base& m_lhs;
    pipeline::Base& m_rhs;
};

// ============================================================================
class Divide : public CellI
{
public:
    Divide(pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

protected:
    pipeline::Base& m_output;
    pipeline::Base& m_lhs;
    pipeline::Base& m_rhs;
};

// ============================================================================
class LessThan : public CellI
{
public:
    LessThan(pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

protected:
    pipeline::Base& m_output;
    pipeline::Base& m_lhs;
    pipeline::Base& m_rhs;
};

// ============================================================================
class GreaterThan : public CellI
{
public:
    GreaterThan(pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

protected:
    pipeline::Base& m_output;
    pipeline::Base& m_lhs;
    pipeline::Base& m_rhs;
};

} // namespace math
} // namespace op

namespace pipeline {

// ============================================================================
class Base : public CellI
{
public:
    Base(Base* first);
    bool has(CellI& role) override               = 0;
    void set(CellI& role, CellI& value) override = 0;
    void operator()() override                   = 0;
    CellI& operator[](CellI& role) override      = 0;
    Type& type() override                        = 0;
    void accept(Visitor& visitor) override       = 0;
    std::string name() const override            = 0;

    void addNext(Base& cell);
    void setCurrent();

    Base* m_next = nullptr;
    Base* m_first;
};


// ============================================================================
class Void : public Base
{
public:
    Void(const std::string& name = "Void");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

protected:
    std::string m_name;
    Base* m_current;
};

// ============================================================================
class Input : public Base
{
public:
    Input(CellI& value, const std::string& name = "Input");
    Input(CellI* value = nullptr, const std::string& name = "Input");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

protected:
    std::string m_name;
    CellI* m_value;
    Base* m_current;
};

// ============================================================================
class New : public Base
{
public:
    New(Type& objectType, const std::string& name = "New");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

protected:
    std::string m_name;
    Type& m_objectType;
    CellI* m_value = nullptr;
    Base* m_current;
};

// ============================================================================
class Fork : public Base
{
public:
    Fork(Base& input, const std::string& name = "Fork");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();
    void addBranch(Base& cell);

protected:
    Base& m_input;
    CellI* m_value = nullptr;
    Base* m_branch = nullptr;
    std::string m_name;
};

// ============================================================================
class Delete : public Base
{
public:
    Delete(Base& input, const std::string& name = "Delete");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

protected:
    std::string m_name;
    CellI* m_input = nullptr;
};

// ============================================================================
class Node : public Base
{
public:
    template <typename... T>
    Node(Base& input, CellI& op, T&... args) :
        Base(input.m_first), m_input(&input)
    {
        initOp(op, args...);
        input.addNext(*this);
    }

    template <typename... T>
    Node(Base& input, const std::string& name, CellI& op, T&... args) :
        Base(input.m_first), m_name(name), m_input(&input)
    {
        initOp(op, args...);
        input.addNext(*this);
    }

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

protected:
    template <typename T1>
    void initOp(CellI& op, T1& arg1)
    {
        if (&op == &control::op::logic::Not::t()) {
            m_op = std::make_unique<op::logic::Not>(*this, arg1);
        }
    }

    template <typename T1, typename T2>
    void initOp(CellI& op, T1& arg1, T2& arg2)
    {
        if (&op == &control::op::Same::t()) {
            m_op = std::make_unique<op::Same>(*this, arg1, arg2);
        }
        if (&op == &control::op::NotSame::t()) {
            m_op = std::make_unique<op::NotSame>(*this, arg1, arg2);
        }
        if (&op == &control::op::Equal::t()) {
            m_op = std::make_unique<op::Equal>(*this, arg1, arg2);
        }
        if (&op == &control::op::NotEqual::t()) {
            m_op = std::make_unique<op::NotEqual>(*this, arg1, arg2);
        }
        if (&op == &control::op::Has::t()) {
            m_op = std::make_unique<op::Has>(*this, arg1, arg2);
        }
        if (&op == &control::op::Get::t()) {
            m_op = std::make_unique<op::Get>(*this, arg1, arg2);
        }
        if (&op == &control::op::logic::And::t()) {
            m_op = std::make_unique<op::logic::And>(*this, arg1, arg2);
        }
        if (&op == &control::op::logic::Or::t()) {
            m_op = std::make_unique<op::logic::Or>(*this, arg1, arg2);
        }
        if (&op == &control::op::math::Add::t()) {
            m_op = std::make_unique<op::math::Add>(*this, arg1, arg2);
        }
        if (&op == &control::op::math::Subtract::t()) {
            m_op = std::make_unique<op::math::Subtract>(*this, arg1, arg2);
        }
        if (&op == &control::op::math::Multiply::t()) {
            m_op = std::make_unique<op::math::Multiply>(*this, arg1, arg2);
        }
        if (&op == &control::op::math::Divide::t()) {
            m_op = std::make_unique<op::math::Divide>(*this, arg1, arg2);
        }
        if (&op == &control::op::math::LessThan::t()) {
            m_op = std::make_unique<op::math::LessThan>(*this, arg1, arg2);
        }
        if (&op == &control::op::math::GreaterThan::t()) {
            m_op = std::make_unique<op::math::GreaterThan>(*this, arg1, arg2);
        }
    }

    template <typename T1, typename T2, typename T3>
    void initOp(CellI& op, T1& arg1, T2& arg2, T3& arg3)
    {
        if (&op == &control::op::Set::t()) {
            m_op = std::make_unique<op::Set>(*this, arg1, arg2, arg3);
        }
    }

    std::string m_name;
    Base* m_input = nullptr;
    std::unique_ptr<CellI> m_op;
    CellI* m_value = nullptr;
};

// ============================================================================
class IfThen : public Base
{
public:
    IfThen(Base& input, const std::string& name = "IfThen");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

    void addCondition(Base& cell);
    void addThenBranch(Base& cell);
    void addElseBranch(Base& cell);

protected:
    std::string m_name;
    CellI& m_input;
    Base* m_condition  = nullptr;
    Base* m_thenBranch = nullptr;
    Base* m_elseBranch = nullptr;
};

// ============================================================================
class DoWhile : public Base
{
public:
    DoWhile(Base& input, const std::string& name = "DoWhile");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

    void addCondition(Base& cell);
    void addStatement(Base& cell);

protected:
    std::string m_name;
    CellI& m_input;
    Base* m_condition = nullptr;
    Base* m_statement = nullptr;
};

// ============================================================================
class While : public Base
{
public:
    While(Base& input, const std::string& name = "While");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

    void addCondition(Base& cell);
    void addStatement(Base& cell);

protected:
    std::string m_name;
    CellI& m_input;
    Base* m_condition = nullptr;
    Base* m_statement = nullptr;
};

} // namespace pipeline
} // namespace control

// ============================================================================
class Visitor
{
public:
    virtual void visit(Slot&)     = 0;
    virtual void visit(Type&)     = 0;
    virtual void visit(Object&)   = 0;
    virtual void visit(ListItem&) = 0;
    virtual void visit(List&)     = 0;
    virtual void visit(Number&)   = 0;
    virtual void visit(String&)   = 0;

    virtual void visit(hybrid::Color&)  = 0;
    virtual void visit(hybrid::Pixel&)  = 0;
    virtual void visit(hybrid::Picture&) = 0;

    virtual void visit(control::op::Same&) { }
    virtual void visit(control::op::NotSame&) { }
    virtual void visit(control::op::Equal&) { }
    virtual void visit(control::op::NotEqual&) { }
    virtual void visit(control::op::Has&) { }
    virtual void visit(control::op::Get&) { }
    virtual void visit(control::op::Set&) { }
    virtual void visit(control::op::logic::And&) { }
    virtual void visit(control::op::logic::Or&) { }
    virtual void visit(control::op::logic::Not&) { }
    virtual void visit(control::op::math::Add&) { }
    virtual void visit(control::op::math::Subtract&) { }
    virtual void visit(control::op::math::Multiply&) { }
    virtual void visit(control::op::math::Divide&) { }
    virtual void visit(control::op::math::LessThan&) { }
    virtual void visit(control::op::math::GreaterThan&) { }
    virtual void visit(control::pipeline::Void&) { }
    virtual void visit(control::pipeline::Input&) { }
    virtual void visit(control::pipeline::New&) { }
    virtual void visit(control::pipeline::Fork&) { }
    virtual void visit(control::pipeline::Delete&) { }
    virtual void visit(control::pipeline::Node&) { }
    virtual void visit(control::pipeline::IfThen&) { }
    virtual void visit(control::pipeline::DoWhile&) { }
    virtual void visit(control::pipeline::While&) { }
};

namespace type {
extern Type Void;
extern Type Boolean;
extern Type Digit;
extern Type Number;
extern Type Char;
extern Type String;
extern Type Color;
extern Type Pixel;
extern Type Picture;

namespace op {
extern Type Base;

extern Type Same;
extern Type NotSame;
extern Type Equal;
extern Type NotEqual;

extern Type Has;
extern Type Get;
extern Type Set;

namespace logic {
extern Type And;
extern Type Or;
extern Type Not;
} // namespace logic

namespace math {
extern Type Add;
extern Type Subtract;
extern Type Multiply;
extern Type Divide;
extern Type LessThan;
extern Type GreaterThan;
} // namespace math
} // namespace op

namespace pipeline {
extern Type Base;
extern Type Void;
extern Type Input;
extern Type New;
extern Type Fork;
extern Type Delete;
extern Type Node;
extern Type IfThen;
extern Type DoWhile;
extern Type While;
} // namespace pipeline

} // namespace type

namespace data {
extern Object slotType;
extern Object slotName;
extern Object slotRole;

extern Object first;
extern Object last;
extern Object previous;
extern Object next;
extern Object current;

extern Object pixels;

namespace coding {
extern Object argument;
extern Object branch;
extern Object cell;
extern Object condition;
extern Object else_;
extern Object input;
extern Object name;
extern Object objectType;
extern Object op;
extern Object output;
extern Object result;
extern Object role;
extern Object statement;
extern Object then;
extern Object value;
} // namespace coding

namespace equation {
extern Object lhs;
extern Object rhs;
} // namespace equation

namespace directions {
extern Object up;
extern Object down;
extern Object left;
extern Object right;
} // namespace directions

namespace coordinates {
extern Object x;
extern Object y;
} // namespace coordinates

extern Object color;
namespace colors {
extern Object red;
extern Object green;
extern Object blue;
} // namespace colors

namespace boolean {
extern Object true_;
extern Object false_;
} // namespace boolean

extern Object width;
extern Object height;

extern Object size;
extern Object value;
extern Object type;
extern Object slots;
extern Object sign;
} // namespace data

void StaticInitializations();

} // namespace cells
} // namespace synth