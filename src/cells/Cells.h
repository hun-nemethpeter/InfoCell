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
    CellI& call(CellI& method);
    CellI& call(CellI& method, CellI& param1Role, CellI& param1Value);

    CellI& get(CellI& role);
    CellI& type();
    void eval();

    std::string label() const;
    void label(const std::string& label);

    bool operator==(CellI& rhs);
    bool operator!=(CellI& rhs);

    brain::Brain& kb;    // knowledge base
    std::string m_label; // for comments

    static int s_constructed;
    static int s_destructed;
};

struct Param
{
    Param(CellI& role, CellI& value) :
        role(role), value(value) { }

    CellI& role;
    CellI& value;
};

// ============================================================================
class Object : public CellI
{
public:
    Object(brain::Brain& kb, CellI& type, const std::string& label = "");
    Object(brain::Brain& kb, CellI& type, CellI& constructor, const std::string& label = "");
    Object(brain::Brain& kb, CellI& type, CellI& constructor, Param param1, const std::string& label = "");
    Object(brain::Brain& kb, CellI& type, CellI& constructor, Param param1, Param param2, const std::string& label = "");
    Object(brain::Brain& kb, CellI& type, CellI& constructor, Param param1, Param param2, Param param3, const std::string& label = "");
    Object(brain::Brain& kb, CellI& type, CellI& constructor, Param param1, Param param2, Param param3, Param param4, const std::string& label = "");
    ~Object();

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

    CellI& method(CellI& role);
    CellI& method(CellI& role, Param param1);
    CellI& method(CellI& role, Param param1, Param param2);
    CellI& method(CellI& role, Param param1, Param param2, Param param3);
    CellI& method(CellI& role, Param param1, Param param2, Param param3, Param param4);

    CellI& smethod(CellI& role);
    CellI& smethod(CellI& role, Param param1);
    CellI& smethod(CellI& role, Param param1, Param param2);
    CellI& smethod(CellI& role, Param param1, Param param2, Param param3);
    CellI& smethod(CellI& role, Param param1, Param param2, Param param3, Param param4);

protected:
    void destructor();

    bool hasMethod(CellI& role);
    CellI& getMethod(CellI& role);
    CellI& getStaticMethod(CellI& role);
    CellI& getFnValue(CellI& fn);

    void setSelf(CellI& fn);
    void setFnParam(CellI& fn, Param param);

    template <typename... Args>
    void setFnParam(CellI& fn, Param param, Args&&... args)
    {
        setFnParam(fn, param);
        setFnParam(fn, std::forward<Args>(args)...);
    }

    CellI& m_type;
    std::map<CellI*, CellI*> m_slots;
};

namespace util {

template <typename T>
T& ref(T& obj) { return obj; }

template <typename T>
T& ref(T* obj) { return *obj; }

} // namespace util

// ============================================================================
class List : public CellI
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

    List(brain::Brain& kb, CellI& valueType);

    template <typename T>
    List(brain::Brain& kb, std::vector<T>& values) :
        List(kb, util::ref(values.front()).type())
    {
        for (auto& valueT : values) {
            add(util::ref(valueT));
        }
    }

    template <typename Key, typename Value>
    List(brain::Brain& kb, std::map<Key, Value>& values) :
        List(kb, util::ref((*values.begin())).second.type())
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

    template <typename... Args>
    void add(CellI& value, Args&&... args)
    {
        add(value);
        add(std::forward<Args>(args)...);
    }

    bool empty() const;

    CellI& toNative();

    struct Value
    {
        Value(List& list, CellI& value);

        Value* prev();
        Value* next();

        List& m_list;
        CellI& m_value;
        std::list<Value>::iterator m_iterator;
        Item m_listItem;
    };

protected:
    CellI& m_valueType;
    std::list<Value> m_items;
};

// ============================================================================
class Map : public CellI
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
        Value(Map& group, CellI& value, CellI& index, size_t listItemIndex);

        Value* prev();
        Value* next();

        Map& m_group;
        CellI& m_value;
        std::list<Value*>::iterator m_iterator;
        Index::Type::Slots::SlotList::Item m_indexTypeSlotsListItem;
        Index::Type::Slot m_indexTypeSlot;
    };

    Map(brain::Brain& kb, CellI& valueType, const std::string& label = "");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

    bool hasKey(CellI& key);
    CellI& getValue(CellI& key);

    void add(CellI& key, CellI& value);
    template <typename... Args>
    void add(CellI& key, CellI& value, Args&&... args)
    {
        add(key, value);
        add(std::forward<Args>(args)...);
    }
    bool empty() const;

protected:
    CellI& m_valueType;

    IndexedValues m_indexedValues;
    OrderedValues m_orderedValues;

public:
    List m_list;
    Index m_index;
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

    void addSlots(CellI& slot);
    template <typename... Args>
    void addSlots(CellI& slot, Args&&... args)
    {
        addSlots(slot);
        addSlots(std::forward<Args>(args)...);
    }
    void addSubType(CellI& role, Type& type);
    void addMembership(CellI& type);
    void addMethod(CellI& role, CellI& method);

protected:
    Map m_slots;
    Map m_subTypes;
    Map m_memberOf;
    Map m_asts;
    Map m_methods;
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
    void accept(Visitor& visitor) override;

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
    std::unique_ptr<List> m_pixelsList;
};

} // namespace hybrid

namespace op {
// ============================================================================
class Base : public CellI
{
public:
    Base(brain::Brain& kb, const std::string& label);
};

// ============================================================================
class Block : public Base
{
public:
    Block(brain::Brain& kb, List& ops, const std::string& label = "Block");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

protected:
    List& m_ops;
    CellI* m_value = nullptr;
};

// ============================================================================
class EvalVar : public Base
{
public:
    EvalVar(brain::Brain& kb, CellI& var, const std::string& label = "EvalVar");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

protected:
    CellI* m_var;
};

// ============================================================================
class Function : public Base
{
public:
    explicit Function(brain::Brain& kb, const std::string& label = "Function");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

    void addInputs(Map& input);
    void addOutputs(Map& output);
    CellI& getOrCreateVar(CellI& role, CellI& type);

protected:
    Map* m_inputs  = nullptr;
    Map* m_outputs = nullptr;
    CellI* m_op     = nullptr;
    Map m_localVars;
};

// ============================================================================
class Delete : public Base
{
public:
    Delete(brain::Brain& kb, CellI& cell, const std::string& label = "Delete");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

protected:
    CellI* m_cell = nullptr;
};

// ============================================================================
class Set : public Base
{
public:
    Set(brain::Brain& kb, CellI& cell, CellI& role, CellI& value);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

protected:
    CellI& m_cell;
    CellI& m_role;
    CellI& m_value;
};

// ============================================================================
class If : public Base
{
public:
    If(brain::Brain& kb, CellI& condition, CellI& thenBranch, const std::string& label = "If");
    If(brain::Brain& kb, CellI& condition, CellI& thenBranch, CellI& elseBranch, const std::string& label = "If");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

    void addCondition(Base& cell);
    void addThenBranch(Base& cell);
    void addElseBranch(Base& cell);

protected:
    CellI* m_condition = nullptr;
    CellI* m_thenBranch = nullptr;
    CellI* m_elseBranch = nullptr;
};

// ============================================================================
class Do : public Base
{
public:
    Do(brain::Brain& kb, CellI& condition, CellI& statement, const std::string& label = "Do");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

    void addCondition(Base& cell);
    void addStatement(Base& cell);

protected:
    CellI* m_condition = nullptr;
    CellI* m_statement = nullptr;
};

// ============================================================================
class While : public Base
{
public:
    While(brain::Brain& kb, CellI& condition, CellI& statement, const std::string& label = "While");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

    void addCondition(Base& cell);
    void addStatement(Base& cell);

protected:
    CellI* m_condition = nullptr;
    CellI* m_statement = nullptr;
};

// ============================================================================
class Expression : public Base
{
public:
    Expression(brain::Brain& kb, const std::string& label);

    CellI* m_value = nullptr;
};

// ============================================================================
class ConstVar : public Expression
{
public:
    ConstVar(brain::Brain& kb, CellI& value, const std::string& label = "ConstVar");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;
};

// ============================================================================
class New : public Expression
{
public:
    New(brain::Brain& kb, CellI& objectType, const std::string& label = "New");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

protected:
    CellI& m_objectType;
};

// ============================================================================
class Same : public Expression
{
public:
    Same(brain::Brain& kb, CellI& lhs, CellI& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

protected:
    CellI& m_lhs;
    CellI& m_rhs;
};

// ============================================================================
class NotSame : public Expression
{
public:
    NotSame(brain::Brain& kb, CellI& lhs, CellI& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

protected:
    CellI& m_lhs;
    CellI& m_rhs;
};

// ============================================================================
class Equal : public Expression
{
public:
    Equal(brain::Brain& kb, CellI& lhs, CellI& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

protected:
    CellI& m_lhs;
    CellI& m_rhs;
};

// ============================================================================
class NotEqual : public Expression
{
public:
    NotEqual(brain::Brain& kb, CellI& lhs, CellI& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

protected:
    CellI& m_lhs;
    CellI& m_rhs;
};

// ============================================================================
class Has : public Expression
{
public:
    Has(brain::Brain& kb, CellI& cell, CellI& role);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

protected:
    CellI& m_cell;
    CellI& m_role;
};

// ============================================================================
class Get : public Expression
{
public:
    Get(brain::Brain& kb, CellI& cell, CellI& role);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

protected:
    CellI& m_cell;
    CellI& m_role;
};

// ============================================================================
class And : public Expression
{
public:
    And(brain::Brain& kb, CellI& lhs, CellI& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

protected:
    CellI& m_lhs;
    CellI& m_rhs;
};

// ============================================================================
class Or : public Expression
{
public:
    Or(brain::Brain& kb, CellI& lhs, CellI& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

protected:
    CellI& m_lhs;
    CellI& m_rhs;
};

// ============================================================================
class Not : public Expression
{
public:
    Not(brain::Brain& kb, CellI& input);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

protected:
    CellI& m_input;
};

// ============================================================================
class Add : public Expression
{
public:
    Add(brain::Brain& kb, CellI& lhs, CellI& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

protected:
    CellI& m_lhs;
    CellI& m_rhs;
};

// ============================================================================
class Subtract : public Expression
{
public:
    Subtract(brain::Brain& kb, CellI& lhs, CellI& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

protected:
    CellI& m_lhs;
    CellI& m_rhs;
};

// ============================================================================
class Multiply : public Expression
{
public:
    Multiply(brain::Brain& kb, CellI& lhs, CellI& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

protected:
    CellI& m_lhs;
    CellI& m_rhs;
};

// ============================================================================
class Divide : public Expression
{
public:
    Divide(brain::Brain& kb, CellI& lhs, CellI& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

protected:
    CellI& m_lhs;
    CellI& m_rhs;
};

// ============================================================================
class LessThan : public Expression
{
public:
    LessThan(brain::Brain& kb, CellI& lhs, CellI& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

protected:
    CellI& m_lhs;
    CellI& m_rhs;
};

// ============================================================================
class GreaterThan : public Expression
{
public:
    GreaterThan(brain::Brain& kb, CellI& lhs, CellI& rhs);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

protected:
    CellI& m_lhs;
    CellI& m_rhs;
};

} // namespace control

// ============================================================================
class Visitor
{
public:
    virtual void visit(Type&)   = 0;
    virtual void visit(Object&) = 0;
    virtual void visit(Number&) = 0;
    virtual void visit(String&) = 0;

    virtual void visit(List&)       = 0;
    virtual void visit(List::Item&) = 0;

    virtual void visit(Map::Index::Type::Slots::SlotList::Item&) = 0;
    virtual void visit(Map::Index::Type::Slots::SlotList&)       = 0;
    virtual void visit(Map::Index::Type::Slots::SlotIndex&)      = 0;
    virtual void visit(Map::Index::Type::Slots&)                 = 0;
    virtual void visit(Map::Index::Type::Slot&)                  = 0;
    virtual void visit(Map::Index::Type&)                        = 0;
    virtual void visit(Map::Index&)                              = 0;
    virtual void visit(Map&)                                     = 0;

    virtual void visit(hybrid::Color&)   = 0;
    virtual void visit(hybrid::Pixel&)   = 0;
    virtual void visit(hybrid::Picture&) = 0;

    virtual void visit(op::Block&) { }
    virtual void visit(op::EvalVar&) { }
    virtual void visit(op::Function&) = 0;
    virtual void visit(op::Delete&) { }
    virtual void visit(op::Set&) { }
    virtual void visit(op::If&) { }
    virtual void visit(op::Do&) { }
    virtual void visit(op::While&) { }
    virtual void visit(op::ConstVar&) { }
    virtual void visit(op::New&) { }
    virtual void visit(op::Same&) { }
    virtual void visit(op::NotSame&) { }
    virtual void visit(op::Equal&) { }
    virtual void visit(op::NotEqual&) { }
    virtual void visit(op::Has&) { }
    virtual void visit(op::Get&) { }
    virtual void visit(op::And&) { }
    virtual void visit(op::Or&) { }
    virtual void visit(op::Not&) { }
    virtual void visit(op::Add&) { }
    virtual void visit(op::Subtract&) { }
    virtual void visit(op::Multiply&) { }
    virtual void visit(op::Divide&) { }
    virtual void visit(op::LessThan&) { }
    virtual void visit(op::GreaterThan&) { }

    static void visitList(CellI& list, std::function<void(CellI& value, int i)> fn);
};

bool tryVisitWith(CellI& cell, Visitor& visitor);

} // namespace cells
} // namespace synth