#pragma once

#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <vector>

#include "app/Picture.h"

namespace synth {
namespace cells {
namespace brain {
class Brain;

namespace templates {
class ParameterDecl;
class Slot;
class CellDescription;
class Cell;
class Parameter;
class TemplateOf;
class SelfType;
} // namespace templates
} // namespace brain

// ============================================================================
class Visitor;
class CellI
{
public:
    CellI(brain::Brain& kb);
    CellI(brain::Brain& kb, const std::string& label);
    CellI(const CellI& rhs);
    virtual ~CellI();

    virtual bool has(CellI& role)               = 0;
    virtual void set(CellI& role, CellI& value) = 0;
    virtual void operator()()                   = 0;
    virtual CellI& operator[](CellI& role)      = 0;
    virtual void accept(Visitor& visitor)       = 0;

    CellI& get(CellI& role);
    CellI& type();

    std::string label() const;
    void label(const std::string& label);

    bool operator==(CellI& rhs);
    bool operator!=(CellI& rhs);

    brain::Brain& kb;    // knowledge base
    std::string m_label; // for comments

    static int s_constructed;
    static int s_destructed;
};

namespace util {

template <typename T>
T& ref(T& obj) { return obj; }

template <typename T>
T& ref(T* obj) { return *obj; }

} // namespace util

// ============================================================================
class RefList : public CellI
{
public:
    struct Value;
    class Item : public CellI
    {
    public:
        Item(brain::Brain& kb, Value& value);

        bool has(CellI& role) override;
        void set(CellI& role, CellI& value) override;
        void operator()() override;
        CellI& operator[](CellI& role) override;
        void accept(Visitor& visitor) override;

        Value& m_value;
    };

    RefList(brain::Brain& kb, CellI& valueType);

    template <typename T>
    RefList(brain::Brain& kb, std::vector<T>& values) :
        RefList(kb, util::ref(values.front()).type())
    {
        for (auto& valueT : values) {
            add(util::ref(valueT));
        }
    }

    template <typename Key, typename Value>
    RefList(brain::Brain& kb, std::map<Key, Value>& values) :
        RefList(kb, util::ref((*values.begin())).second.type())
    {
        for (auto& valuePairs : values) {
            add(util::ref(valuePairs.second));
        }
    }

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

    void add(CellI& value);
    bool empty() const;

    struct Value
    {
        Value(RefList& list, CellI& value);

        Value* prev();
        Value* next();

        RefList& m_list;
        CellI& m_value;
        std::list<Value>::iterator m_iterator;
        Item m_listItem;
    };

protected:
    CellI& m_valueType;
    std::list<Value> m_items;
};

// ============================================================================
class RefMap : public CellI
{
#if 0
SomeValue
    type: SomeType
    roleSize: Number
    roleName: String

SlotMap
    list:   List
    index:  Index

    Index
        type:     Index::Type
        roleSize: Slot1
        roleName: Slot2

        Index::Type
            slotList:  Index::Type::SlotList
            slotIndex: Index::Type::SlotIndex

            Index::Type::SlotList
                type:  ListOf(Slot)
                first: Index::Type::SlotList::Item1
                last:  Index::Type::SlotList::Item2
                size:  2

                Index::Type::SlotList::Item1               Index::Type::SlotList::Item2
                    type:  ListItemOf(Slot)                    type:  ListItemOf(Slot)
                    next:  Index::Type::SlotList::Item2        prev:  Index::Type::SlotList::Item1
                    value: Index::Type::Slot1                  value: Index::Type::Slot2

                Index::Type::Slot1                         Index::Type::Slot2
                    type:     Slot                             type:     Slot
                    slotRole: roleSize                         slotRole: roleName
                    slotType: Slot                             slotType: Slot

            Index::Type::SlotIndex
                type:     Index::Type // !!
                roleSize: Index::Type::Slot1
                roleName: Index::Type::Slot2

#endif
public:
    struct Value;
    typedef std::map<CellI*, Value> IndexedValues;
    typedef std::list<Value*> OrderedValues;

    class Index : public CellI
    {
    public:
        class Type : public CellI
        {
        public:
            class Slots : public CellI
            {
            public:
                class SlotList : public CellI
                {
                public:
                    class Item : public CellI
                    {
                    public:
                        Item(brain::Brain& kb, Value& value);

                        bool has(CellI& role) override;
                        void set(CellI& role, CellI& value) override;
                        void operator()() override;
                        CellI& operator[](CellI& role) override;
                        void accept(Visitor& visitor) override;

                        Value& m_value;
                    };

                    SlotList(brain::Brain& kb, OrderedValues& orderedValues);

                    bool has(CellI& role) override;
                    void set(CellI& role, CellI& value) override;
                    void operator()() override;
                    CellI& operator[](CellI& role) override;
                    void accept(Visitor& visitor) override;

                    OrderedValues& m_orderedValues;
                };

                class SlotIndex : public CellI
                {
                public:
                    SlotIndex(brain::Brain& kb, IndexedValues& indexedValues, Type& type);

                    bool has(CellI& role) override;
                    void set(CellI& role, CellI& value) override;
                    void operator()() override;
                    CellI& operator[](CellI& role) override;
                    void accept(Visitor& visitor) override;

                    IndexedValues& m_indexedValues;
                    Type& m_type;
                };

                Slots(brain::Brain& kb, IndexedValues& indexedValues, OrderedValues& orderedValues, CellI& valueType, Type& type);

                bool has(CellI& role) override;
                void set(CellI& role, CellI& value) override;
                void operator()() override;
                CellI& operator[](CellI& role) override;
                void accept(Visitor& visitor) override;

                SlotList m_slotList;
                SlotIndex m_slotIndex;
            };

            class Slot : public CellI
            {
            public:
                Slot(brain::Brain& kb, CellI& slotRole);

                bool has(CellI& role) override;
                void set(CellI& role, CellI& value) override;
                void operator()() override;
                CellI& operator[](CellI& role) override;
                void accept(Visitor& visitor) override;

                CellI& m_slotRole;
            };

            Type(brain::Brain& kb, IndexedValues& indexedValues, OrderedValues& orderedValues, CellI& valueType);

            bool has(CellI& role) override;
            void set(CellI& role, CellI& value) override;
            void operator()() override;
            CellI& operator[](CellI& role) override;
            void accept(Visitor& visitor) override;

            Slots m_slots;
        };

        Index(brain::Brain& kb, IndexedValues& indexedValues, OrderedValues& orderedValues, CellI& valueType);

        bool has(CellI& role) override;
        void set(CellI& role, CellI& value) override;
        void operator()() override;
        CellI& operator[](CellI& role) override;
        void accept(Visitor& visitor) override;

        Type m_type;
        IndexedValues& m_indexedValues;
        OrderedValues& m_orderedValues;
    };

    struct Value
    {
        Value(RefMap& group, CellI& value, CellI& index, size_t listItemIndex);

        Value* prev();
        Value* next();

        RefMap& m_group;
        CellI& m_value;
        std::list<Value*>::iterator m_iterator;
        Index::Type::Slots::SlotList::Item m_indexTypeSlotsListItem;
        Index::Type::Slot m_indexTypeSlot;
    };

    RefMap(brain::Brain& kb, CellI& valueType, const std::string& label = "");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

    void add(CellI& key, CellI& value);
    bool empty() const;

protected:
    CellI& m_valueType;

    IndexedValues m_indexedValues;
    OrderedValues m_orderedValues;

public:
    RefList m_list;
    Index m_index;
};

// ============================================================================
class Slot : public CellI
{
public:
    Slot(brain::Brain& kb, CellI& role, CellI& type);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

    Slot& slotTypeParameters(CellI& paramCell);
    Slot& valueDefinition(CellI& defCell);
    Slot& defaultValue(CellI& valueCell);
    Slot& required();

protected:
    CellI& m_slotRole;
    CellI& m_slotType;
    CellI* m_valueDefinition = nullptr;
    bool m_required          = false;
};

// ============================================================================
class Type : public CellI
{
public:
    explicit Type(brain::Brain& kb, const std::string& label = "Type");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;
    void addSlot(CellI& role, CellI& type);
    void addSlots(Slot& slot);
    template <typename... Args>
    void addSlots(Slot& slot, Args&&... args)
    {
        addSlots(slot);
        addSlots(std::forward<Args>(args)...);
    }
    void addSubType(CellI& role, Type& type);
    void addMembership(CellI& type);

protected:
    RefMap m_slots;
    RefMap m_subTypes;
    RefMap m_memberOf;
};

// ============================================================================
class Object : public CellI
{
public:
    Object(brain::Brain& kb, CellI& classCell, const std::string& label = "");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

protected:
    CellI& m_type;
    std::map<CellI*, CellI*> m_slots;
};

// ============================================================================
#if 0
A generic list can hold any type of values.
But how can we express the thing "any type"? We can create an extra type, for example: kb.type.Any.

A List of Slots or List<Slot> should holds only Slot types. But how can we express such a specialization?
This List<Slot> still a list, and should be used anywhere where a generic list is used.

The C++ template style template can not express such a relation as List<> is not a type but a template.

So we need a List type and a specialized List with a specialization.

A type can have a subType. List object will create ListItem type which is a kind of sequence.
A type should have a parameter like thing where we can express what kind of object is allowed.
   Maybe we can introduce three kind of parameters for this
    1. typeIs: the type is Slot, List<typeIs: Slot>
    2. groupOf: the type is part of a group, List<groupOf: Numbers>
    3. expression: Slot | Digit | Number < 12, List<expression: { Slot | Digit | Number < 12 }>
   Maybe we can use a value definition parameter

A type should have some kind of definition where we can express what is the relation between theese parameters, members, and what is the purpose of theese.
#endif

// ============================================================================
class Template : public CellI
{
public:
    explicit Template(brain::Brain& kb, const std::string& label = "Template");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

    void addParams(brain::templates::ParameterDecl& param);
    template <typename... Args>
    void addParams(brain::templates::ParameterDecl& param, Args&&... args)
    {
        addParams(param);
        addParams(std::forward<Args>(args)...);
    }

    void addSlots(brain::templates::Slot& slot);
    template <typename... Args>
    void addSlots(brain::templates::Slot& slot, Args&&... args)
    {
        addSlots(slot);
        addSlots(std::forward<Args>(args)...);
    }

    void addSubTypes(brain::templates::Slot& slot);
    template <typename... Args>
    void addSubTypes(brain::templates::Slot& slot, Args&&... args)
    {
        addSubTypes(slot);
        addSubTypes(std::forward<Args>(args)...);
    }

    CellI& getParamType();
    Type& compile(CellI& param);

protected:
    CellI& compileCell(CellI& descriptor, CellI& param, CellI& selfType);

    RefMap m_parameters;
    std::unique_ptr<Type> m_parametersType;

    RefList m_slots;
    RefList m_subTypes;
    RefList m_memberOf;
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
    void accept(Visitor& visitor) override;

    int value() const;
    void value(int newValue);

protected:
    void calculateDigits();

    int m_value;
    std::vector<Object*> m_digits;
    std::unique_ptr<RefList> m_digitsList;
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
    void accept(Visitor& visitor) override;

    const std::string& value() const;

protected:
    void calculateCharacters();

    std::string m_value;
    std::vector<Object*> m_characters;
    std::unique_ptr<RefList> m_charactersList;
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
    void accept(Visitor& visitor) override;

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
    void accept(Visitor& visitor) override;

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
    void accept(Visitor& visitor) override;

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
    int m_width;
    int m_height;
    Number& m_widthCell;
    Number& m_heightCell;
    std::vector<Pixel> m_pixels;
    std::unique_ptr<RefList> m_pixelsList;
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
    void accept(Visitor& visitor) override;

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
    void accept(Visitor& visitor) override;

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
    void accept(Visitor& visitor) override;

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
    void accept(Visitor& visitor) override;

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
    void accept(Visitor& visitor) override;

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
    void accept(Visitor& visitor) override;

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
    void accept(Visitor& visitor) override;

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
    void accept(Visitor& visitor) override;

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
    void accept(Visitor& visitor) override;

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
    void accept(Visitor& visitor) override;

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
    void accept(Visitor& visitor) override;

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
    void accept(Visitor& visitor) override;

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
    void accept(Visitor& visitor) override;

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
    void accept(Visitor& visitor) override;

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
    void accept(Visitor& visitor) override;

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
    void accept(Visitor& visitor) override;

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
    Base(brain::Brain& kb, Base* first, const std::string& label);

    void addNext(Base& cell);
    void setCurrent();

    Base* m_next = nullptr;
    Base* m_first;
};

// ============================================================================
class Void : public Base
{
public:
    Void(brain::Brain& kb, const std::string& label = "Void");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

protected:
    Base* m_current;
};

// ============================================================================
class Input : public Base
{
public:
    Input(brain::Brain& kb, CellI& value, const std::string& label = "Input");
    Input(brain::Brain& kb, CellI* value = nullptr, const std::string& label = "Input");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

protected:
    CellI* m_value;
    Base* m_current;
};

// ============================================================================
class New : public Base
{
public:
    New(brain::Brain& kb, Type& objectType, const std::string& label = "New");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

protected:
    Type& m_objectType;
    CellI* m_value = nullptr;
    Base* m_current;
};

// ============================================================================
class Fork : public Base
{
public:
    Fork(brain::Brain& kb, Base& input, const std::string& label = "Fork");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

    void addBranch(Base& cell);

protected:
    Base& m_input;
    CellI* m_value = nullptr;
    Base* m_branch = nullptr;
};

// ============================================================================
class Delete : public Base
{
public:
    Delete(brain::Brain& kb, Base& input, const std::string& label = "Delete");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

protected:
    CellI* m_input = nullptr;
};

// ============================================================================
class Node : public Base
{
public:
    template <typename... T>
    Node(brain::Brain& kb, Base& input, CellI& op, T&... args) :
        Base(kb, input.m_first, "Node"), m_input(&input)
    {
        initOp(op, args...);
        input.addNext(*this);
    }

    template <typename... T>
    Node(brain::Brain& kb, Base& input, const std::string& label, CellI& op, T&... args) :
        Base(kb, input.m_first, label), m_input(&input)
    {
        initOp(op, args...);
        input.addNext(*this);
    }

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

protected:
    template <typename T1>
    void initOp(CellI& op, T1& arg1);

    template <typename T1, typename T2>
    void initOp(CellI& op, T1& arg1, T2& arg2);

    template <typename T1, typename T2, typename T3>
    void initOp(CellI& op, T1& arg1, T2& arg2, T3& arg3);

    Base* m_input = nullptr;
    std::unique_ptr<CellI> m_op;
    CellI* m_value = nullptr;
};

// ============================================================================
class IfThen : public Base
{
public:
    IfThen(brain::Brain& kb, Base& input, const std::string& label = "IfThen");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

    void addCondition(Base& cell);
    void addThenBranch(Base& cell);
    void addElseBranch(Base& cell);

protected:
    CellI& m_input;
    Base* m_condition  = nullptr;
    Base* m_thenBranch = nullptr;
    Base* m_elseBranch = nullptr;
};

// ============================================================================
class DoWhile : public Base
{
public:
    DoWhile(brain::Brain& kb, Base& input, const std::string& label = "DoWhile");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

    void addCondition(Base& cell);
    void addStatement(Base& cell);

protected:
    CellI& m_input;
    Base* m_condition = nullptr;
    Base* m_statement = nullptr;
};

// ============================================================================
class While : public Base
{
public:
    While(brain::Brain& kb, Base& input, const std::string& label = "While");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

    void addCondition(Base& cell);
    void addStatement(Base& cell);

protected:
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
    virtual void visit(Slot&)   = 0;
    virtual void visit(Type&)   = 0;
    virtual void visit(Object&) = 0;
    virtual void visit(Number&) = 0;
    virtual void visit(String&) = 0;

    virtual void visit(RefList&)       = 0;
    virtual void visit(RefList::Item&) = 0;

    virtual void visit(RefMap::Index::Type::Slots::SlotList::Item&) = 0;
    virtual void visit(RefMap::Index::Type::Slots::SlotList&)       = 0;
    virtual void visit(RefMap::Index::Type::Slots::SlotIndex&)      = 0;
    virtual void visit(RefMap::Index::Type::Slots&)                 = 0;
    virtual void visit(RefMap::Index::Type::Slot&)                  = 0;
    virtual void visit(RefMap::Index::Type&)                        = 0;
    virtual void visit(RefMap::Index&)                              = 0;
    virtual void visit(RefMap&)                                     = 0;

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

    static void visitList(CellI& list, std::function<void(CellI& value, int i)> fn);
};

bool tryVisitWith(CellI& cell, Visitor& visitor);

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
