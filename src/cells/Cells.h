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
}

// ============================================================================
class Visitor;
class CellI
{
public:
    CellI(brain::Brain& kb);
    CellI(brain::Brain& kb, const std::string& label);
    CellI(const CellI& rhs);
    ~CellI();

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

    brain::Brain& kb; // knowledge base
    std::string m_label;

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
class Slot : public CellI
{
public:
    Slot(brain::Brain& kb, CellI& role, CellI& type);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

protected:
    CellI& m_slotRole;
    CellI& m_slotType;
};

// ============================================================================
class SlotRef // Only exists to bypass the non-movable std::initalizer_list limitations
{
public:
    SlotRef(CellI& role, CellI& type);

    CellI& m_role;
    CellI& m_type;
};

// ============================================================================
class IndexedList;
class Type : public CellI
{
public:
    enum class InitMode
    {
        InitDuringBootstrap,
        InitSubTypes,
        Normal
    };

    explicit Type(brain::Brain& kb, const std::string& label = "Type");
    Type(brain::Brain& kb, std::initializer_list<SlotRef> slots);
    Type(brain::Brain& kb, const std::string& label, std::initializer_list<SlotRef> slots);
    Type(brain::Brain& kb, const std::string& label, InitMode initMode);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

    void addSlots(std::initializer_list<SlotRef> slots);
    Type& addSubType(CellI& role, const std::string& label, InitMode initMode = InitMode::Normal);
    void manualInit();
    void manualInitSubTypes();

protected:
    void createSlot(CellI& role, CellI& type);

    std::map<CellI*, Slot> m_slotsMap;
    std::vector<CellI*> m_slotsOrder;
    std::unique_ptr<IndexedList> m_slots;

    std::map<CellI*, Slot> m_parametersMap;
    std::vector<CellI*> m_parametersOrder;
    std::unique_ptr<IndexedList> m_parameters;

    std::map<CellI*, Type> m_subTypesMap;
    std::vector<CellI*> m_subTypesOrder;
    std::unique_ptr<IndexedList> m_subTypes;
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
class ListItem : public CellI
{
public:
    ListItem(brain::Brain& kb, CellI& type);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

    CellI& prev();
    void prev(ListItem* p);

    CellI& next();
    void next(ListItem* n);

    CellI& value();
    void value(CellI* v);

protected:
    CellI& m_type;
    CellI* m_prev  = nullptr;
    CellI* m_next  = nullptr;
    CellI* m_value = nullptr;
};

// ============================================================================
class Number;
class List : public CellI
{
public:
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

protected:
    CellI& m_valueType;
    Type& m_listType;
    CellI& m_itemType;
    std::list<ListItem> m_items;
};

// ============================================================================
class IndexedList
{
#if 0
SomeValue
    type: SomeType
    roleSize: Number
    roleName: String

IndexedList
    valueList:   ValueList
    valueIndex:  ValueIndex

    ValueList
        type:  ListOf(Slot)
        first: ValueList::Item1
        last:  ValueList::Item2
        size:  2

        ValueList::Item1                ValueList::Item2
            type:  ListItemOf(Slot)         type:  ListItemOf(Slot)
            next:  ValueList::Item2         prev:  ValueList::Item1
            value: Slot1                    value: Slot2

        Slot1                           Slot2
            type:     Slot                  type:     Slot
            slotRole: roleSize              slotRole: roleName
            slotType: Number                slotType: String

    ValueIndex
        type:     ValueIndex::Type
        roleSize: Slot1
        roleName: Slot2

        ValueIndex::Type
            slotList:   ValueIndex::Type::SlotList
            slotIndex:  ValueIndex::Type::SlotIndex

            ValueIndex::Type::SlotList
                type:  ListOf(Slot)
                first: ValueIndex::Type::SlotList::Item1
                last:  ValueIndex::Type::SlotList::Item2
                size:  2

                ValueIndex::Type::SlotList::Item1               ValueIndex::Type::SlotList::Item2
                    type:  ListItemOf(Slot)                         type:  ListItemOf(Slot)
                    next:  ValueIndex::Type::SlotList::Item2        prev:  ValueIndex::Type::SlotList::Item1
                    value: ValueIndex::Type::Slot1                  value: ValueIndex::Type::Slot2

                ValueIndex::Type::Slot1                         ValueIndex::Type::Slot2
                    type:     Slot                                  type:     Slot
                    slotRole: roleSize                              slotRole: roleName
                    slotType: Slot                                  slotType: Slot

            ValueIndex::Type::SlotIndex
                type:     ValueIndex::Type // !!
                roleSize: ValueIndex::Type::Slot1
                roleName: ValueIndex::Type::Slot2

#endif
public:
    struct ValueItem;
    typedef std::map<CellI*, ValueItem> IndexedValueItems;
    typedef std::vector<ValueItem*> OrderedValueItems;

    class ValueList : public CellI
    {
    public:
        class Item : public CellI
        {
        public:
            Item(brain::Brain& kb, ValueItem& valueItem);

            bool has(CellI& role) override;
            void set(CellI& role, CellI& value) override;
            void operator()() override;
            CellI& operator[](CellI& role) override;
            void accept(Visitor& visitor) override;

            ValueItem& m_valueItem;
        };

        ValueList(brain::Brain& kb, OrderedValueItems& listItems, CellI& valueType);

        bool has(CellI& role) override;
        void set(CellI& role, CellI& value) override;
        void operator()() override;
        CellI& operator[](CellI& role) override;
        void accept(Visitor& visitor) override;

        OrderedValueItems& m_listItems;
        CellI& m_listType;
    };

    class ValueIndex : public CellI
    {
    public:
        class Type : public CellI
        {
        public:
            class SlotList : public CellI
            {
            public:
                class Item : public CellI
                {
                public:
                    Item(brain::Brain& kb, ValueItem& valueItem);

                    bool has(CellI& role) override;
                    void set(CellI& role, CellI& value) override;
                    void operator()() override;
                    CellI& operator[](CellI& role) override;
                    void accept(Visitor& visitor) override;

                    ValueItem& m_valueItem;
                };

                SlotList(brain::Brain& kb, OrderedValueItems& orderedValueItems);

                bool has(CellI& role) override;
                void set(CellI& role, CellI& value) override;
                void operator()() override;
                CellI& operator[](CellI& role) override;
                void accept(Visitor& visitor) override;

                OrderedValueItems& m_listItems;
            };

            class SlotIndex : public CellI
            {
            public:
                SlotIndex(brain::Brain& kb, IndexedValueItems& indexedValueItems, Type& type);

                bool has(CellI& role) override;
                void set(CellI& role, CellI& value) override;
                void operator()() override;
                CellI& operator[](CellI& role) override;
                void accept(Visitor& visitor) override;

                IndexedValueItems& m_indexedValueItems;
                Type& m_type;
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

            Type(brain::Brain& kb, IndexedValueItems& indexedValueItems, OrderedValueItems& orderedValueItems, CellI& valueType);

            bool has(CellI& role) override;
            void set(CellI& role, CellI& value) override;
            void operator()() override;
            CellI& operator[](CellI& role) override;
            void accept(Visitor& visitor) override;

            SlotList m_slotList;
            SlotIndex m_slotIndex;
        };

        ValueIndex(brain::Brain& kb, IndexedValueItems& indexedValueItems, OrderedValueItems& orderedValueItems, CellI& valueType);

        bool has(CellI& role) override;
        void set(CellI& role, CellI& value) override;
        void operator()() override;
        CellI& operator[](CellI& role) override;
        void accept(Visitor& visitor) override;

        Type m_type;
        IndexedValueItems& m_indexedValueItems;
        OrderedValueItems& m_orderedValueItems;
    };

    struct ValueItem
    {
        ValueItem(brain::Brain& kb, CellI& index, CellI& value, size_t listItemIndex, IndexedList& indexedList);

        ValueItem* prev();
        ValueItem* next();

        CellI& m_value;
        size_t m_listItemIndex;
        IndexedList& m_indexedList;
        ValueList::Item m_valueListItem;
        ValueIndex::Type::SlotList::Item m_valueIndexTypeSlotListItem;
        ValueIndex::Type::Slot m_valueIndexTypeSlot;
    };

    // Store valueType and indexed with valueType.role
    // for example valueType: Slot, role: slotRole, then storing a list of those Slot
    // and creating an index-cell where (Slot obj).role -> Slot
    IndexedList(brain::Brain& kb, CellI& valueType);

    void add(CellI& key, CellI& value);

protected:
    brain::Brain& m_kb;
    CellI& m_valueType;

    IndexedValueItems m_indexedValueItems;
    OrderedValueItems m_orderedValueItems;

public:
    ValueList m_valueList;
    ValueIndex m_valueIndex;

    friend ValueItem;
};

// ============================================================================
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
class Template;
class TemplateParam;
class TemplateParamCell
{
public:
    CellI* m_cell = nullptr;
};

class TemplateParamParam
{
public:
    CellI* paramRole = nullptr;
};

class TemplateParamTemplateOf
{
public:
    Template* m_tamplateOf = nullptr;
    CellI* m_templateParam = nullptr;
};

class TemplateParamSelfType
{
public:
};

// ============================================================================
class TemplateCellExpression
{
public:
    TemplateCellExpression(CellI& exprType, TemplateParamCell& expr);
    TemplateCellExpression(CellI& exprType, TemplateParamParam expr);
    TemplateCellExpression(CellI& exprType, TemplateParamTemplateOf expr);
    TemplateCellExpression(CellI& exprType, TemplateParamSelfType& expr);

    CellI& m_exprType; // kb.cells.template.cell | kb.cells.template.param | kb.cells.template.templateOf | kb.cells.template.selfType
    union Param
    {
        TemplateParamCell m_cell;
        TemplateParamParam m_param;
        TemplateParamTemplateOf m_templateOf;
        TemplateParamSelfType m_selfType;
    };
    Param m_data;
};

// ============================================================================
class TemplateSlot : public Object
{
public:
    TemplateSlot(brain::Brain& kb, CellI& role, CellI& type);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;
};

// ============================================================================
class TemplateSlotRef // Only exists to bypass the non-movable std::initalizer_list limitations
{
public:
    TemplateSlotRef(TemplateCellExpression& role, TemplateCellExpression& type);

    TemplateCellExpression m_role;
    TemplateCellExpression m_type;
};

// ============================================================================
class Template : public CellI
{
public:
    explicit Template(brain::Brain& kb, const std::string& label = "Template");
    Template(brain::Brain& kb, std::initializer_list<SlotRef> params, std::initializer_list<TemplateSlotRef> slots);
    Template(brain::Brain& kb, const std::string& label, std::initializer_list<SlotRef> params, std::initializer_list<TemplateSlotRef> slots);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

    void addParam(TemplateParam& param);
    void addSlot(std::initializer_list<TemplateSlotRef> slots);
    CellI* instantiate(CellI& param);

protected:
    std::map<CellI*, TemplateParam> m_params;
    std::vector<CellI*> m_paramsOrder;
    std::map<CellI*, TemplateSlot> m_slots;
    std::vector<CellI*> m_slotsOrder;

    IndexedList m_paramsList;
    IndexedList m_slotsList;
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
    virtual void visit(Slot&)     = 0;
    virtual void visit(Type&)     = 0;
    virtual void visit(Object&)   = 0;
    virtual void visit(ListItem&) = 0;
    virtual void visit(List&)     = 0;
    virtual void visit(Number&)   = 0;
    virtual void visit(String&)   = 0;

    virtual void visit(IndexedList::ValueList::Item&) = 0;
    virtual void visit(IndexedList::ValueList&)       = 0;

    virtual void visit(IndexedList::ValueIndex::Type::SlotList::Item&) = 0;
    virtual void visit(IndexedList::ValueIndex::Type::SlotList&)       = 0;
    virtual void visit(IndexedList::ValueIndex::Type::SlotIndex&)      = 0;
    virtual void visit(IndexedList::ValueIndex::Type::Slot&)           = 0;
    virtual void visit(IndexedList::ValueIndex::Type&)                 = 0;
    virtual void visit(IndexedList::ValueIndex&)                       = 0;

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

    void visitList(CellI& list, std::function<void(CellI& value, int i)> fn);
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
