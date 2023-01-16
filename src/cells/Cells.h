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
    explicit Type(const std::string& name);
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
    Number(int value);

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

    static std::unique_ptr<Type> s_type;
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
    String(const std::string& str);

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
    static Slot& slotCharacters();

protected:
    void calculateCharacters();

    static std::unique_ptr<Type> s_type;
    static Slot* s_slotCharacters;

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
class Sensor : public CellI
{
public:
    Sensor(input::Picture& screen);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

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
class BaseNode;
}
namespace op {

// ============================================================================
class Same : public CellI
{
public:
    Same(pipeline::BaseNode& m_output, pipeline::BaseNode& lhs, pipeline::BaseNode& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

protected:
    pipeline::BaseNode& m_output;
    pipeline::BaseNode& m_lhs;
    pipeline::BaseNode& m_rhs;
};

// ============================================================================
class NotSame : public CellI
{
public:
    NotSame(pipeline::BaseNode& m_output, pipeline::BaseNode& lhs, pipeline::BaseNode& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

protected:
    pipeline::BaseNode& m_output;
    pipeline::BaseNode& m_lhs;
    pipeline::BaseNode& m_rhs;
};

// ============================================================================
class Equal : public CellI
{
public:
    Equal(pipeline::BaseNode& m_output, pipeline::BaseNode& lhs, pipeline::BaseNode& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

protected:
    pipeline::BaseNode& m_output;
    pipeline::BaseNode& m_lhs;
    pipeline::BaseNode& m_rhs;
};

// ============================================================================
class NotEqual : public CellI
{
public:
    NotEqual(pipeline::BaseNode& m_output, pipeline::BaseNode& lhs, pipeline::BaseNode& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

protected:
    pipeline::BaseNode& m_output;
    pipeline::BaseNode& m_lhs;
    pipeline::BaseNode& m_rhs;
};

// ============================================================================
class New : public CellI
{
public:
    New(pipeline::BaseNode& m_output, pipeline::BaseNode& type);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

protected:
    pipeline::BaseNode& m_output;
    pipeline::BaseNode& m_type;
};

// ============================================================================
class Delete : public CellI
{
public:
    Delete(pipeline::BaseNode& m_output, pipeline::BaseNode& m_input);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

protected:
    pipeline::BaseNode& m_output;
    pipeline::BaseNode& m_input;
};

// ============================================================================
class Has : public CellI
{
public:
    Has(pipeline::BaseNode& m_output, pipeline::BaseNode& m_cell, pipeline::BaseNode& m_role);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

protected:
    pipeline::BaseNode& m_output;
    pipeline::BaseNode& m_cell;
    pipeline::BaseNode& m_role;
};

// ============================================================================
class Get : public CellI
{
public:
    Get(pipeline::BaseNode& m_output, pipeline::BaseNode& m_cell, pipeline::BaseNode& m_role);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

protected:
    pipeline::BaseNode& m_output;
    pipeline::BaseNode& m_cell;
    pipeline::BaseNode& m_role;
};

// ============================================================================
class Set : public CellI
{
public:
    Set(pipeline::BaseNode& m_output, pipeline::BaseNode& m_cell, pipeline::BaseNode& m_role, pipeline::BaseNode& m_value);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

protected:
    pipeline::BaseNode& m_output;
    pipeline::BaseNode& m_cell;
    pipeline::BaseNode& m_role;
    pipeline::BaseNode& m_value;
};

namespace logic {
// ============================================================================
class And : public CellI
{
public:
    And(pipeline::BaseNode& m_output, pipeline::BaseNode& lhs, pipeline::BaseNode& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

protected:
    pipeline::BaseNode& m_output;
    pipeline::BaseNode& m_lhs;
    pipeline::BaseNode& m_rhs;
};

// ============================================================================
class Or : public CellI
{
public:
    Or(pipeline::BaseNode& m_output, pipeline::BaseNode& lhs, pipeline::BaseNode& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

protected:
    pipeline::BaseNode& m_output;
    pipeline::BaseNode& m_lhs;
    pipeline::BaseNode& m_rhs;
};

// ============================================================================
class Not : public CellI
{
public:
    Not(pipeline::BaseNode& m_output, pipeline::BaseNode& m_input);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

protected:
    pipeline::BaseNode& m_output;
    pipeline::BaseNode& m_input;
};

} // namespace logic

namespace math {

// ============================================================================
class Add : public CellI
{
public:
    Add(pipeline::BaseNode& m_output, pipeline::BaseNode& lhs, pipeline::BaseNode& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

protected:
    pipeline::BaseNode& m_output;
    pipeline::BaseNode& m_lhs;
    pipeline::BaseNode& m_rhs;
};

// ============================================================================
class Subtract : public CellI
{
public:
    Subtract(pipeline::BaseNode& m_output, pipeline::BaseNode& lhs, pipeline::BaseNode& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

protected:
    pipeline::BaseNode& m_output;
    pipeline::BaseNode& m_lhs;
    pipeline::BaseNode& m_rhs;
};

// ============================================================================
class Multiply : public CellI
{
public:
    Multiply(pipeline::BaseNode& m_output, pipeline::BaseNode& lhs, pipeline::BaseNode& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

protected:
    pipeline::BaseNode& m_output;
    pipeline::BaseNode& m_lhs;
    pipeline::BaseNode& m_rhs;
};

// ============================================================================
class Divide : public CellI
{
public:
    Divide(pipeline::BaseNode& m_output, pipeline::BaseNode& lhs, pipeline::BaseNode& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

protected:
    pipeline::BaseNode& m_output;
    pipeline::BaseNode& m_lhs;
    pipeline::BaseNode& m_rhs;
};

// ============================================================================
class LessThan : public CellI
{
public:
    LessThan(pipeline::BaseNode& m_output, pipeline::BaseNode& lhs, pipeline::BaseNode& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

protected:
    pipeline::BaseNode& m_output;
    pipeline::BaseNode& m_lhs;
    pipeline::BaseNode& m_rhs;
};

// ============================================================================
class GreaterThan : public CellI
{
public:
    GreaterThan(pipeline::BaseNode& m_output, pipeline::BaseNode& lhs, pipeline::BaseNode& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

protected:
    pipeline::BaseNode& m_output;
    pipeline::BaseNode& m_lhs;
    pipeline::BaseNode& m_rhs;
};

} // namespace math
} // namespace op

namespace pipeline {

// ============================================================================
class BaseNode : public CellI
{
public:
    bool has(CellI& role) override = 0;
    void set(CellI& role, CellI& value) override = 0;
    void operator()() override = 0;
    CellI& operator[](CellI& role) override = 0;
    Type& type() override = 0;
    void accept(Visitor& visitor) override = 0;
    std::string name() const override = 0;

    void addNext(BaseNode& cell);

protected:
    BaseNode* m_next = nullptr;
};

// ============================================================================
class StartNode : public BaseNode
{
public:
    StartNode(CellI& input, const std::string& name = "RefNode");

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
    CellI& m_value;
};

// ============================================================================
class RefNode : public BaseNode
{
public:
    RefNode(BaseNode& input, const std::string& name = "RefNode");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

protected:
    BaseNode& m_input;
    CellI* m_value = nullptr;
    std::string m_name;
};

// ============================================================================
class EmptyNode : public BaseNode
{
public:
    EmptyNode(const std::string& name = "EmptyNode");

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
};

// ============================================================================
class Node : public BaseNode
{
public:
    template <typename... T>
    Node(BaseNode& inputNode, CellI& op, T&... args) :
        m_input(inputNode)
    {
        initOp(op, args...);
        m_input.addNext(*this);
    }

    template <typename... T>
    Node(BaseNode& inputNode, const std::string& name, CellI& op, T&... args) :
        m_name(name), m_input(inputNode)
    {
        initOp(op, args...);
        m_input.addNext(*this);
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
#if 0 // TODO
        if (&op == &control::op::New::t()) {
            m_op = std::make_unique<op::New>(*this, arg1);
        }
        if (&op == &control::op::Delete::t()) {
            m_op = std::make_unique<op::Delete>(*this, arg1);
        }
        if (&op == &control::op::logic::Not::t()) {
            m_op = std::make_unique<op::logic::Not>(*this, arg1);
        }
#endif
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
#if 0 // TODO
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
#endif
    }

    template <typename T1, typename T2, typename T3>
    void initOp(CellI& op, T1& arg1, T2& arg2, T3& arg3)
    {
#if 0 // TODO

        if (&op == &control::op::Has::t()) {
            m_op = std::make_unique<op::Has>(*this, arg1, arg2, arg3);
        }
        if (&op == &control::op::Get::t()) {
            m_op = std::make_unique<op::Get>(*this, arg1, arg2, arg3);
        }
        if (&op == &control::op::Set::t()) {
            m_op = std::make_unique<op::Set>(*this, arg1, arg2, arg3);
        }
#endif
    }

    std::string m_name;
    BaseNode& m_input;
    std::unique_ptr<CellI> m_op;
    CellI* m_value = nullptr;
};

// ============================================================================
class IfThen : public BaseNode
{
public:
    IfThen(BaseNode& inputNode, const std::string& name = "IfThen");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

    void addCondition(BaseNode& cell);
    void addThenBranch(BaseNode& cell);
    void addElseBranch(BaseNode& cell);

protected:
    std::string m_name;
    CellI& m_input;
    BaseNode* m_condition  = nullptr;
    BaseNode* m_thenBranch = nullptr;
    BaseNode* m_elseBranch = nullptr;
    CellI* m_value         = nullptr;
};

// ============================================================================
class DoWhile : public BaseNode
{
public:
    DoWhile(BaseNode& inputNode, const std::string& name = "DoWhile");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

    void addCondition(BaseNode& cell);
    void addStatement(BaseNode& cell);

protected:
    std::string m_name;
    CellI& m_input;
    BaseNode* m_condition = nullptr;
    BaseNode* m_statement = nullptr;
    CellI* m_value        = nullptr;
};

// ============================================================================
class While : public BaseNode
{
public:
    While(BaseNode& inputNode, const std::string& name = "While");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    static Type& t();

    void addCondition(BaseNode& cell);
    void addStatement(BaseNode& cell);

protected:
    std::string m_name;
    CellI& m_input;
    BaseNode* m_condition = nullptr;
    BaseNode* m_statement = nullptr;
    CellI* m_value        = nullptr;
};

} // namespace pipeline
} // namespace control

// ============================================================================
class Visitor
{
public:
    virtual void visit(Slot& cell)           = 0;
    virtual void visit(Type& cell)           = 0;
    virtual void visit(Object& cell)         = 0;
    virtual void visit(ListItem& cell)       = 0;
    virtual void visit(List& cell)           = 0;
    virtual void visit(Number& cell)         = 0;
    virtual void visit(String& cell)         = 0;
    virtual void visit(hybrid::Color& cell)  = 0;
    virtual void visit(hybrid::Pixel& cell)  = 0;
    virtual void visit(hybrid::Sensor& cell) = 0;
};

namespace type
{
extern Type Boolean;
extern Type Digit;
extern Type Number;
extern Type Char;
extern Type String;
extern Type Color;
extern Type Pixel;
extern Type Sensor;

namespace op {
extern Type Same;
extern Type NotSame;
extern Type Equal;
extern Type NotEqual;

extern Type New;
extern Type Delete;

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

namespace pipeline {
extern Type StartNode;
extern Type RefNode;
extern Type EmptyNode;
extern Type Node;
extern Type IfThen;
extern Type DoWhile;
extern Type While;
} // namespace pipeline

} // namespace control
} // namespace type


namespace data {
extern Object slotType;
extern Object slotName;
extern Object slotRole;

extern Object first;
extern Object last;
extern Object previous;
extern Object next;

extern Object listOfPixels;

namespace coding {
extern Object name;
extern Object value;
extern Object cell;
extern Object input;
extern Object output;
extern Object result;
extern Object argument;
extern Object op;
extern Object condition;
extern Object statement;
extern Object then;
extern Object else_;
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
} // namespace cells

void StaticInitializations();

} // namespace cells
} // namespace synth