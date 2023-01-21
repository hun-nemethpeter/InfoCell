#pragma once

#include <iostream>
#include <list>
#include <map>
#include <vector>

#include "app/Picture.h"

namespace synth {
namespace cells {
namespace brain {
struct Brain;
}

// ============================================================================
class Type;
class Visitor;
class CellI
{
public:
    CellI(brain::Brain& kb);

    virtual bool has(CellI& role)               = 0;
    virtual void set(CellI& role, CellI& value) = 0;
    virtual void operator()()                   = 0;
    virtual CellI& operator[](CellI& role)      = 0;
    virtual Type& type()                        = 0;
    virtual void accept(Visitor& visitor)       = 0;
    virtual std::string name() const            = 0;
    bool operator==(CellI& rhs);
    bool operator!=(CellI& rhs);
    brain::Brain& kb;
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
    Slot(brain::Brain& kb, const std::string& name, Type& type, CellI& role);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    Type& slotType();
    CellI& slotRole();

protected:
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
    explicit Type(brain::Brain& kb, const std::string& name = "Type");
    Type(brain::Brain& kb, const std::string& name, std::initializer_list<SlotRef> slots);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    void addSlots(std::initializer_list<SlotRef> slots);
    Slot& createSlot(const std::string& name, Type& type, CellI& role);

    bool has(const std::string& name) const;

    bool hasSlot(CellI& role);
    bool hasSlot(const std::string& name);
    Slot& getSlot(CellI& role);
    Slot& getSlot(const std::string& name);

    std::map<std::string, Slot>& slots();

protected:
    std::map<std::string, Slot> m_slots;
    std::map<CellI*, Slot*> m_roles;
    std::string m_name;
    std::unique_ptr<List> m_slotsList;
};

// ============================================================================
class Object : public CellI
{
public:
    Object(brain::Brain& kb, Type& classCell);
    Object(brain::Brain& kb, const std::string& name, Type& classCell);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    std::map<CellI*, CellI*>& roles();

protected:
    std::string m_name;
    Type& m_type;
    std::map<CellI*, CellI*> m_roles;
};

// ============================================================================
class ListItem : public CellI
{
public:
    ListItem(brain::Brain& kb, Type& type);

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

protected:
    Type& m_type;
    CellI* m_prev  = nullptr;
    CellI* m_next  = nullptr;
    CellI* m_value = nullptr;
};

// ============================================================================
class List : public CellI
{
public:
    List(brain::Brain& kb, Type& valueType);

    template <typename T>
    List(brain::Brain& kb, std::vector<T>& values);

    template <typename T>
    List(brain::Brain& kb, std::map<std::string, T>& values);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    std::list<ListItem>& items();
    void add(CellI& value);
    Type& valueType();
    Type& listType();
    Type& itemType();

protected:
    Type& m_valueType;
    Type m_listType;
    Type m_itemType;
    std::list<ListItem> m_items;
};

// ============================================================================
class Number : public CellI
{
public:
    explicit Number(brain::Brain& kb, int value = 0);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    int value() const;

protected:
    void calculateDigits();

    int m_value;
    std::vector<Object*> m_digits;
    std::unique_ptr<List> m_digitsList;
};

// ============================================================================
class String : public CellI
{
public:
    explicit String(brain::Brain& kb, const std::string& str = "");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    const std::string& value() const;

protected:
    void calculateCharacters();

    std::string m_value;
    std::vector<Object*> m_characters;
    std::unique_ptr<List> m_charactersList;
};

namespace hybrid {

// ============================================================================
class Color : public CellI
{
public:
    Color(brain::Brain& kb, const input::Color& inputColor);
    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

    const input::Color& color() const;

private:
    const input::Color& m_inputColor;
};

// ============================================================================
class Pixel : public CellI
{
public:
    Pixel(brain::Brain& kb, int x, int y, const input::Color& inputColor);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

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
    Picture(brain::Brain& kb, input::Picture& screen);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

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
    Same(brain::Brain& kb, pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

protected:
    pipeline::Base& m_output;
    pipeline::Base& m_lhs;
    pipeline::Base& m_rhs;
};

// ============================================================================
class NotSame : public CellI
{
public:
    NotSame(brain::Brain& kb, pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

protected:
    pipeline::Base& m_output;
    pipeline::Base& m_lhs;
    pipeline::Base& m_rhs;
};

// ============================================================================
class Equal : public CellI
{
public:
    Equal(brain::Brain& kb, pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

protected:
    pipeline::Base& m_output;
    pipeline::Base& m_lhs;
    pipeline::Base& m_rhs;
};

// ============================================================================
class NotEqual : public CellI
{
public:
    NotEqual(brain::Brain& kb, pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

protected:
    pipeline::Base& m_output;
    pipeline::Base& m_lhs;
    pipeline::Base& m_rhs;
};

// ============================================================================
class Has : public CellI
{
public:
    Has(brain::Brain& kb, pipeline::Base& output, pipeline::Base& cell, pipeline::Base& role);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

protected:
    pipeline::Base& m_output;
    pipeline::Base& m_cell;
    pipeline::Base& m_role;
};

// ============================================================================
class Get : public CellI
{
public:
    Get(brain::Brain& kb, pipeline::Base& output, pipeline::Base& cell, pipeline::Base& role);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

protected:
    pipeline::Base& m_output;
    pipeline::Base& m_cell;
    pipeline::Base& m_role;
};

// ============================================================================
class Set : public CellI
{
public:
    Set(brain::Brain& kb, pipeline::Base& output, pipeline::Base& cell, pipeline::Base& role, pipeline::Base& value);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

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
    And(brain::Brain& kb, pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

protected:
    pipeline::Base& m_output;
    pipeline::Base& m_lhs;
    pipeline::Base& m_rhs;
};

// ============================================================================
class Or : public CellI
{
public:
    Or(brain::Brain& kb, pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

protected:
    pipeline::Base& m_output;
    pipeline::Base& m_lhs;
    pipeline::Base& m_rhs;
};

// ============================================================================
class Not : public CellI
{
public:
    Not(brain::Brain& kb, pipeline::Base& output, pipeline::Base& input);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

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
    Add(brain::Brain& kb, pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

protected:
    pipeline::Base& m_output;
    pipeline::Base& m_lhs;
    pipeline::Base& m_rhs;
};

// ============================================================================
class Subtract : public CellI
{
public:
    Subtract(brain::Brain& kb, pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

protected:
    pipeline::Base& m_output;
    pipeline::Base& m_lhs;
    pipeline::Base& m_rhs;
};

// ============================================================================
class Multiply : public CellI
{
public:
    Multiply(brain::Brain& kb, pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

protected:
    pipeline::Base& m_output;
    pipeline::Base& m_lhs;
    pipeline::Base& m_rhs;
};

// ============================================================================
class Divide : public CellI
{
public:
    Divide(brain::Brain& kb, pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

protected:
    pipeline::Base& m_output;
    pipeline::Base& m_lhs;
    pipeline::Base& m_rhs;
};

// ============================================================================
class LessThan : public CellI
{
public:
    LessThan(brain::Brain& kb, pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

protected:
    pipeline::Base& m_output;
    pipeline::Base& m_lhs;
    pipeline::Base& m_rhs;
};

// ============================================================================
class GreaterThan : public CellI
{
public:
    GreaterThan(brain::Brain& kb, pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

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
    Base(brain::Brain& kb, Base* first);
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
    Void(brain::Brain& kb, const std::string& name = "Void");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

protected:
    std::string m_name;
    Base* m_current;
};

// ============================================================================
class Input : public Base
{
public:
    Input(brain::Brain& kb, CellI& value, const std::string& name = "Input");
    Input(brain::Brain& kb, CellI* value = nullptr, const std::string& name = "Input");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

 protected:
    std::string m_name;
    CellI* m_value;
    Base* m_current;
};

// ============================================================================
class New : public Base
{
public:
    New(brain::Brain& kb, Type& objectType, const std::string& name = "New");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

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
    Fork(brain::Brain& kb, Base& input, const std::string& name = "Fork");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

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
    Delete(brain::Brain& kb, Base& input, const std::string& name = "Delete");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

protected:
    std::string m_name;
    CellI* m_input = nullptr;
};

// ============================================================================
class Node : public Base
{
public:
    template <typename... T>
    Node(brain::Brain& kb, Base& input, CellI& op, T&... args) :
        Base(kb, input.m_first), m_input(&input)
    {
        initOp(op, args...);
        input.addNext(*this);
    }

    template <typename... T>
    Node(brain::Brain& kb, Base& input, const std::string& name, CellI& op, T&... args) :
        Base(kb, input.m_first), m_name(name), m_input(&input)
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

protected:
    template <typename T1>
    void initOp(CellI& op, T1& arg1);

    template <typename T1, typename T2>
    void initOp(CellI& op, T1& arg1, T2& arg2);

    template <typename T1, typename T2, typename T3>
    void initOp(CellI& op, T1& arg1, T2& arg2, T3& arg3);

    std::string m_name;
    Base* m_input = nullptr;
    std::unique_ptr<CellI> m_op;
    CellI* m_value = nullptr;
};

// ============================================================================
class IfThen : public Base
{
public:
    IfThen(brain::Brain& kb, Base& input, const std::string& name = "IfThen");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

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
    DoWhile(brain::Brain& kb, Base& input, const std::string& name = "DoWhile");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

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
    While(brain::Brain& kb, Base& input, const std::string& name = "While");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    void accept(Visitor& visitor) override;
    std::string name() const override;

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

    virtual void visit(hybrid::Color&)   = 0;
    virtual void visit(hybrid::Pixel&)   = 0;
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

} // namespace cells
} // namespace synth

#include "Brain.h"

namespace synth {
namespace cells {
namespace control {
namespace pipeline {

template <typename T1>
void Node::initOp(CellI& op, T1& arg1)
{
    if (&op == &kb.type.op.logic.Not) {
        m_op = std::make_unique<op::logic::Not>(kb, *this, arg1);
    }
}

template <typename T1, typename T2>
void Node::initOp(CellI& op, T1& arg1, T2& arg2)
{
    if (&op == &kb.type.op.Same) {
        m_op = std::make_unique<op::Same>(kb, *this, arg1, arg2);
    }
    if (&op == &kb.type.op.NotSame) {
        m_op = std::make_unique<op::NotSame>(kb, *this, arg1, arg2);
    }
    if (&op == &kb.type.op.Equal) {
        m_op = std::make_unique<op::Equal>(kb, *this, arg1, arg2);
    }
    if (&op == &kb.type.op.NotEqual) {
        m_op = std::make_unique<op::NotEqual>(kb, *this, arg1, arg2);
    }
    if (&op == &kb.type.op.Has) {
        m_op = std::make_unique<op::Has>(kb, *this, arg1, arg2);
    }
    if (&op == &kb.type.op.Get) {
        m_op = std::make_unique<op::Get>(kb, *this, arg1, arg2);
    }
    if (&op == &kb.type.op.logic.And) {
        m_op = std::make_unique<op::logic::And>(kb, *this, arg1, arg2);
    }
    if (&op == &kb.type.op.logic.Or) {
        m_op = std::make_unique<op::logic::Or>(kb, *this, arg1, arg2);
    }
    if (&op == &kb.type.op.math.Add) {
        m_op = std::make_unique<op::math::Add>(kb, *this, arg1, arg2);
    }
    if (&op == &kb.type.op.math.Subtract) {
        m_op = std::make_unique<op::math::Subtract>(kb, *this, arg1, arg2);
    }
    if (&op == &kb.type.op.math.Multiply) {
        m_op = std::make_unique<op::math::Multiply>(kb, *this, arg1, arg2);
    }
    if (&op == &kb.type.op.math.Divide) {
        m_op = std::make_unique<op::math::Divide>(kb, *this, arg1, arg2);
    }
    if (&op == &kb.type.op.math.LessThan) {
        m_op = std::make_unique<op::math::LessThan>(kb, *this, arg1, arg2);
    }
    if (&op == &kb.type.op.math.GreaterThan) {
        m_op = std::make_unique<op::math::GreaterThan>(kb, *this, arg1, arg2);
    }
}

template <typename T1, typename T2, typename T3>
void Node::initOp(CellI& op, T1& arg1, T2& arg2, T3& arg3)
{
    if (&op == &kb.type.op.Set) {
        m_op = std::make_unique<op::Set>(kb, *this, arg1, arg2, arg3);
    }
}

} // namespace pipeline
} // namespace control
} // namespace cells
} // namespace synth
