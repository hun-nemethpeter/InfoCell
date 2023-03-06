#include "Cells.h"

#include <format>
#include <sstream>
#include <utility>

#pragma warning(disable : 4996)
#include <utf8.h>

namespace synth {
namespace cells {

int CellI::s_constructed = 0;
int CellI::s_destructed  = 0;

CellI::CellI(brain::Brain& kb) :
    kb(kb)
{
    s_constructed += 1;
}

CellI::CellI(brain::Brain& kb, const std::string& label) :
    kb(kb), m_label(label)
{
    s_constructed += 1;
}

CellI::CellI(const CellI& rhs) :
    kb(rhs.kb), m_label(rhs.m_label)
{
    throw "Unexpected cell copy";
}

CellI::~CellI()
{
    s_destructed += 1;
}

CellI& CellI::get(CellI& role)
{
    return (*this)[role];
}

CellI& CellI::type()
{
    return (*this)[kb.cells.type];
}

std::string CellI::label() const
{
    return m_label;
}

void CellI::label(const std::string& label)
{
    m_label = label;
}

bool CellI::operator==(CellI& rhs)
{
    if (&type() != &rhs.type()) {
        return false;
    }
    CellI& slotList    = type()[kb.cells.slots][kb.cells.list];
    CellI* slotItemPtr = slotList.has(kb.sequence.first) ? &slotList[kb.sequence.first] : nullptr;
    while (slotItemPtr) {
        CellI& slotItem = *slotItemPtr;
        CellI& slot     = slotItem[kb.coding.value];
        CellI& role     = slot[kb.cells.slotRole];

        bool hasLeftSlot = has(role);
        if (hasLeftSlot != rhs.has(role)) {
            return false;
        }
        if (hasLeftSlot && (&(*this)[role] != &rhs[role])) {
            return false;
        }

        slotItemPtr = slotItem.has(kb.sequence.next) ? &slotItem[kb.sequence.next] : nullptr;
    }

    return true;
}

bool CellI::operator!=(CellI& rhs)
{
    return !((*this) == rhs);
}

// ============================================================================
Slot::Slot(brain::Brain& kb, CellI& role, CellI& type) :
    CellI(kb),
    m_slotRole(role),
    m_slotType(type)
{
}

bool Slot::has(CellI& role)
{
    if (&role == &kb.cells.type || &role == &kb.cells.slotType || &role == &kb.cells.slotRole) {
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
        return kb.type.Slot;
    }
    if (&role == &kb.cells.slotType) {
        return m_slotType;
    }
    if (&role == &kb.cells.slotRole) {
        return m_slotRole;
    }

    return kb.cells.emptyObject;
}

void Slot::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

// ============================================================================
Type::Type(brain::Brain& kb, const std::string& label) :
    CellI(kb, label),
    m_slots(kb, kb.type.Slot),
    m_subTypes(kb, kb.type.Type_),
    m_memberOf(kb, kb.type.Type_)
{
}

bool Type::has(CellI& role)
{
    if (&role == &kb.cells.type) {
        return true;
    }
    if (&role == &kb.cells.slots && !m_slots.empty()) {
        return true;
    }
    if (&role == &kb.cells.subTypes && !m_subTypes.empty()) {
        return true;
    }
    if (&role == &kb.cells.memberOf && !m_memberOf.empty()) {
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
        return kb.type.Type_;
    }
    if (&role == &kb.cells.slots) {
        return m_slots;
    }
    if (&role == &kb.cells.subTypes) {
        return m_subTypes;
    }
    if (&role == &kb.cells.memberOf) {
        return m_memberOf;
    }

    return kb.cells.emptyObject;
}

void Type::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

void Type::addSlot(CellI& role, CellI& type)
{
    Slot& slot = *new Slot(kb, role, type);
    if (kb.isInitialized()) {
        slot.label(std::format("Slot of {}.{}", label(), role.label()));
    }
    m_slots.add(role, slot);
}

void Type::addSlots(Slot& slot)
{
    CellI& role = slot[kb.cells.slotRole];
    if (kb.isInitialized()) {
        slot.label(std::format("Slot of {}.{}", label(), role.label()));
    }
    m_slots.add(role, slot);
}

void Type::addSubType(CellI& role, Type& type)
{
    m_subTypes.add(role, type);
}

void Type::addMembership(CellI& type)
{
    m_memberOf.add(type, type);
}

// ============================================================================
Object::Object(brain::Brain& kb, CellI& type, const std::string& label) :
    CellI(kb, label), m_type(type)
{
    m_slots[&kb.cells.type] = &type;
}

bool Object::has(CellI& role)
{
    if (&role == &kb.cells.type)
        return true;

    return m_slots.find(&role) != m_slots.end();
}

void Object::set(CellI& role, CellI& value)
{
    if (&role == &kb.cells.type) {
        throw "Type change not allowed.";
    }

    if (type()[kb.cells.slots][kb.cells.index].has(role)) {
        m_slots[&role] = &value;
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
    auto findIt = m_slots.find(&role);
    if (findIt == m_slots.end())
        return kb.cells.emptyObject;

    return *findIt->second;
}

void Object::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

// ============================================================================
RefList::Item::Item(brain::Brain& kb, Value& value) :
    CellI(kb), m_value(value)
{
}

bool RefList::Item::has(CellI& role)
{
    if (&role == &kb.cells.type || &role == &kb.coding.value) {
        return true;
    }
    if (&role == &kb.sequence.previous && m_value.prev()) {
        return true;
    }
    if (&role == &kb.sequence.next && m_value.next()) {
        return true;
    }

    return false;
}

void RefList::Item::set(CellI& role, CellI& value)
{
    // Do nothing
}

void RefList::Item::operator()()
{
    // Do nothing
}

CellI& RefList::Item::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return kb.type.ListOf(m_value.m_list.m_valueType)[kb.cells.subTypes][kb.cells.index][kb.coding.objectType];
    }
    if (&role == &kb.sequence.previous) {
        if (m_value.prev())
            return m_value.prev()->m_listItem;
        else
            return kb.cells.emptyObject;
    }
    if (&role == &kb.sequence.next) {
        if (m_value.next())
            return m_value.next()->m_listItem;
        else
            return kb.cells.emptyObject;
    }
    if (&role == &kb.coding.value) {
        return m_value.m_value;
    }

    return kb.cells.emptyObject;
}

void RefList::Item::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

// ============================================================================
RefList::Value::Value(RefList& list, CellI& value) :
    m_list(list),
    m_value(value),
    m_listItem(list.kb, *this)
{
}

RefList::Value* RefList::Value::prev()
{
    return m_iterator != m_list.m_items.begin() ? &*std::prev(m_iterator) : nullptr;
}

RefList::Value* RefList::Value::next()
{
    return m_iterator != std::prev(m_list.m_items.end()) ? &*std::next(m_iterator) : nullptr;
}

// ============================================================================
RefList::RefList(brain::Brain& kb, CellI& valueType) :
    CellI(kb),
    m_valueType(valueType)
{
}

bool RefList::has(CellI& role)
{
    if (&role == &kb.cells.type || &role == &kb.dimensions.size) {
        return true;
    }
    if ((&role == &kb.sequence.first || &role == &kb.sequence.last) && !m_items.empty()) {
        return true;
    }

    return false;
}

void RefList::set(CellI& role, CellI& value)
{
    throw "Not supported";
}

void RefList::operator()()
{
    // Do nothing, this is a data cell
}

CellI& RefList::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return kb.type.ListOf(m_valueType);
    }
    if (&role == &kb.sequence.first) {
        return m_items.front().m_listItem;
    }
    if (&role == &kb.sequence.last) {
        return m_items.back().m_listItem;
    }
    if (&role == &kb.dimensions.size) {
        int size = (int)m_items.size();

        return kb.pools.numbers.get(size);
    }

    return kb.cells.emptyObject;
}

void RefList::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

void RefList::add(CellI& value)
{
    m_items.emplace_back(*this, value);
    m_items.back().m_iterator = std::prev(m_items.end());
}

bool RefList::empty() const
{
    return m_items.empty();
}

// ============================================================================
RefMap::Index::Type::Slots::SlotList::Item::Item(brain::Brain& kb, Value& value) :
    CellI(kb),
    m_value(value)
{
}

bool RefMap::Index::Type::Slots::SlotList::Item::has(CellI& role)
{
    if (&role == &kb.cells.type || &role == &kb.coding.value) {
        return true;
    }
    if (&role == &kb.sequence.previous && m_value.prev()) {
        return true;
    }
    if (&role == &kb.sequence.next && m_value.next()) {
        return true;
    }

    return false;
}

void RefMap::Index::Type::Slots::SlotList::Item::set(CellI& role, CellI& value)
{
    // Do nothing
}

void RefMap::Index::Type::Slots::SlotList::Item::operator()()
{
    // Do nothing, this is a data cell
}

CellI& RefMap::Index::Type::Slots::SlotList::Item::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return kb.type.ListOf(kb.type.Slot)[kb.cells.subTypes][kb.cells.index][kb.coding.objectType];
    }
    if (&role == &kb.sequence.previous) {
        if (m_value.prev())
            return m_value.prev()->m_indexTypeSlotsListItem;
        else
            return kb.cells.emptyObject;
    }
    if (&role == &kb.sequence.next) {
        if (m_value.next())
            return m_value.next()->m_indexTypeSlotsListItem;
        else
            return kb.cells.emptyObject;
    }
    if (&role == &kb.coding.value) {
        return m_value.m_indexTypeSlot;
    }

    return kb.cells.emptyObject;
}

void RefMap::Index::Type::Slots::SlotList::Item::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

// ============================================================================
RefMap::Index::Type::Slots::SlotList::SlotList(brain::Brain& kb, OrderedValues& orderedValues) :
    CellI(kb),
    m_orderedValues(orderedValues)
{
}

bool RefMap::Index::Type::Slots::SlotList::has(CellI& role)
{
    if (&role == &kb.cells.type || &role == &kb.sequence.first || &role == &kb.sequence.last || &role == &kb.dimensions.size) {
        return true;
    }

    return false;
}

void RefMap::Index::Type::Slots::SlotList::set(CellI& role, CellI& value)
{
    // Do nothing
}

void RefMap::Index::Type::Slots::SlotList::operator()()
{
    // Do nothing, this is a data cell
}

CellI& RefMap::Index::Type::Slots::SlotList::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return kb.type.ListOf(kb.type.Slot);
    }
    if (&role == &kb.sequence.first) {
        if (m_orderedValues.empty()) {
            return kb.cells.emptyObject;
        }
        return (*m_orderedValues.begin())->m_indexTypeSlotsListItem;
    }
    if (&role == &kb.sequence.last) {
        if (m_orderedValues.empty()) {
            return kb.cells.emptyObject;
        }
        return (*m_orderedValues.rbegin())->m_indexTypeSlotsListItem;
    }
    if (&role == &kb.dimensions.size) {
        return kb.pools.numbers.get((int)m_orderedValues.size());
    }

    return kb.cells.emptyObject;
}

void RefMap::Index::Type::Slots::SlotList::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

// ============================================================================
RefMap::Index::Type::Slots::SlotIndex::SlotIndex(brain::Brain& kb, IndexedValues& indexedValues, Type& type) :
    CellI(kb),
    m_indexedValues(indexedValues),
    m_type(type)
{
}

bool RefMap::Index::Type::Slots::SlotIndex::has(CellI& role)
{
    if (&role == &kb.cells.type) {
        return true;
    }
    auto slotIt = m_indexedValues.find(&role);
    if (slotIt != m_indexedValues.end()) {
        return true;
    }

    return false;
}

void RefMap::Index::Type::Slots::SlotIndex::set(CellI& role, CellI& value)
{
    // Do nothing
}

void RefMap::Index::Type::Slots::SlotIndex::operator()()
{
    // Do nothing, this is a data cell
}

CellI& RefMap::Index::Type::Slots::SlotIndex::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return m_type;
    }
    auto slotIt = m_indexedValues.find(&role);
    if (slotIt != m_indexedValues.end()) {
        return slotIt->second.m_indexTypeSlot;
    }

    return kb.cells.emptyObject;
}

void RefMap::Index::Type::Slots::SlotIndex::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

// ============================================================================
RefMap::Index::Type::Slot::Slot(brain::Brain& kb, CellI& slotRole) :
    CellI(kb),
    m_slotRole(slotRole)
{
}

bool RefMap::Index::Type::Slot::has(CellI& role)
{
    if (&role == &kb.cells.type || &role == &kb.cells.slotType || &role == &kb.cells.slotRole) {
        return true;
    }
    return false;
}

void RefMap::Index::Type::Slot::set(CellI& role, CellI& value)
{
    // Do nothing
}

void RefMap::Index::Type::Slot::operator()()
{
    // Do nothing, this is a data cell
}

CellI& RefMap::Index::Type::Slot::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return kb.type.Slot;
    }
    if (&role == &kb.cells.slotType) {
        return kb.type.Slot;
    }
    if (&role == &kb.cells.slotRole) {
        return m_slotRole;
    }

    return kb.cells.emptyObject;
}

void RefMap::Index::Type::Slot::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

// ============================================================================
RefMap::Index::Type::Slots::Slots(brain::Brain& kb, IndexedValues& indexedValues, OrderedValues& orderedValues, CellI& valueType, Type& type) :
    CellI(kb),
    m_slotList(kb, orderedValues),
    m_slotIndex(kb, indexedValues, type)
{
    if (&valueType == &kb.type.Slot) {
        label("Index<Slot>::RefMap");
    } else {
        label(std::format("Index<{}>::RefMap", valueType.label()));
    }
}

bool RefMap::Index::Type::Slots::has(CellI& role)
{
    if (&role == &kb.cells.type || &role == &kb.cells.index || &role == &kb.cells.list) {
        return true;
    }

    return false;
}

void RefMap::Index::Type::Slots::set(CellI& role, CellI& value)
{
    // Do nothing
}

void RefMap::Index::Type::Slots::operator()()
{
    // Do nothing, this is a data cell
}

CellI& RefMap::Index::Type::Slots::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return kb.type.MapOf(kb.type.Slot);
    }
    if (&role == &kb.cells.index) {
        return m_slotIndex;
    }
    if (&role == &kb.cells.list) {
        return m_slotList;
    }

    return kb.cells.emptyObject;
}

void RefMap::Index::Type::Slots::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

// ============================================================================
RefMap::Index::Type::Type(brain::Brain& kb, IndexedValues& indexedValues, OrderedValues& orderedValues, CellI& valueType) :
    CellI(kb),
    m_slots(kb, indexedValues, orderedValues, valueType, *this)
{
    if (&valueType == &kb.type.Slot) {
        label("Index<Slot>");
    } else {
        label(std::format("Index<{}>", valueType.label()));
    }
}

bool RefMap::Index::Type::has(CellI& role)
{
    if (&role == &kb.cells.type || &role == &kb.cells.slots) {
        return true;
    }

    return false;
}

void RefMap::Index::Type::set(CellI& role, CellI& value)
{
    // Do nothing
}

void RefMap::Index::Type::operator()()
{
    // Do nothing, this is a data cell
}

CellI& RefMap::Index::Type::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return kb.type.Type_;
    }
    if (&role == &kb.cells.slots) {
        return m_slots;
    }

    return kb.cells.emptyObject;
}

void RefMap::Index::Type::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

// ============================================================================
RefMap::Index::Index(brain::Brain& kb, IndexedValues& indexedValues, OrderedValues& orderedValues, CellI& valueType) :
    CellI(kb),
    m_type(kb, indexedValues, orderedValues, valueType),
    m_indexedValues(indexedValues),
    m_orderedValues(orderedValues)
{
}

bool RefMap::Index::has(CellI& role)
{
    if (&role == &kb.cells.type) {
        return true;
    }
    auto slotIt = m_indexedValues.find(&role);
    if (slotIt != m_indexedValues.end()) {
        return true;
    }

    return false;
}

void RefMap::Index::set(CellI& role, CellI& value)
{
    // Do nothing
}

void RefMap::Index::operator()()
{
    // Do nothing, this is a data cell
}

CellI& RefMap::Index::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return m_type;
    }
    auto slotIt = m_indexedValues.find(&role);
    if (slotIt != m_indexedValues.end()) {
        return slotIt->second.m_value;
    }

    return kb.cells.emptyObject;
}

void RefMap::Index::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

// ============================================================================
RefMap::Value::Value(RefMap& group, CellI& value, CellI& index, size_t listItemIndex) :
    m_group(group),
    m_value(value),
    m_indexTypeSlotsListItem(group.kb, *this),
    m_indexTypeSlot(group.kb, index)
{
}

RefMap::Value* RefMap::Value::prev()
{
    return m_iterator != m_group.m_orderedValues.begin() ? *std::prev(m_iterator) : nullptr;
}

RefMap::Value* RefMap::Value::next()
{
    return m_iterator != std::prev(m_group.m_orderedValues.end()) ? *std::next(m_iterator) : nullptr;
}

// ============================================================================
RefMap::RefMap(brain::Brain& kb, CellI& valueType, const std::string& label) :
    CellI(kb, label),
    m_valueType(valueType),
    m_list(kb, valueType),
    m_index(kb, m_indexedValues, m_orderedValues, valueType)
{
}

bool RefMap::has(CellI& role)
{
    if (&role == &kb.cells.type || &role == &kb.dimensions.size) {
        return true;
    }
    if (&role == &kb.cells.index && !m_orderedValues.empty()) {
        return true;
    }
    if (&role == &kb.cells.list && !m_orderedValues.empty()) {
        return true;
    }

    return false;
}

void RefMap::set(CellI& role, CellI& value)
{
    throw "Not supported";
}

void RefMap::operator()()
{
    // Do nothing, this is a data cell
}

CellI& RefMap::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return kb.type.MapOf(m_valueType);
    }
    if (&role == &kb.cells.index) {
        return m_index;
    }
    if (&role == &kb.cells.list) {
        return m_list;
    }
    if (&role == &kb.dimensions.size) {
        int size = (int)m_orderedValues.size();

        return kb.pools.numbers.get(size);
    }

    return kb.cells.emptyObject;
}

void RefMap::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

#if 0
void Map::add(CellI& value)
{
    add(value, value);
}
#endif

void RefMap::add(CellI& key, CellI& value)
{
    if (m_indexedValues.find(&key) != m_indexedValues.end()) {
        throw "A value already registered with this role";
    }
    auto it = m_indexedValues.emplace(std::piecewise_construct,
                                      std::forward_as_tuple(&key),
                                      std::forward_as_tuple(*this, value, key, m_orderedValues.size()));
    m_orderedValues.push_back(&it.first->second);
    m_orderedValues.back()->m_iterator = std::prev(m_orderedValues.end());
    m_list.add(value);
}

bool RefMap::empty() const
{
    return m_indexedValues.empty();
}

// ============================================================================
Template::Template(brain::Brain& kb, const std::string& label) :
    CellI(kb, label),
    m_parameters(kb, kb.type.template_.ParameterDecl),
    m_slots(kb, kb.type.template_.Slot),
    m_subTypes(kb, kb.type.template_.Slot),
    m_memberOf(kb, kb.type.template_.Descriptor)
{
}

bool Template::has(CellI& role)
{
    if (&role == &kb.cells.type) {
        return true;
    }
    if (&role == &kb.coding.parameters && !m_parameters.empty()) {
        return true;
    }
    if (&role == &kb.cells.slots && !m_slots.empty()) {
        return true;
    }
    if (&role == &kb.cells.subTypes && !m_subTypes.empty()) {
        return true;
    }

    return false;
}

void Template::set(CellI& role, CellI& value)
{
    // Do nothing
}

void Template::operator()()
{
    // Do nothing, this is a data cell
}

CellI& Template::operator[](CellI& role)
{
    if (&role == &kb.cells.type) {
        return kb.type.Template;
    }
    if (&role == &kb.coding.parameters) {
        return m_parameters;
    }
    if (&role == &kb.cells.slots) {
        return m_slots;
    }
    if (&role == &kb.cells.subTypes) {
        return m_subTypes;
    }

    return kb.cells.emptyObject;
}

void Template::accept(Visitor& visitor)
{
    // TODO
}

void Template::addParams(brain::templates::ParameterDecl& param)
{
    m_parameters.add(param[kb.cells.slotRole], param);
}

void Template::addSlots(brain::templates::Slot& slot)
{
    m_slots.add(slot);
}

void Template::addSubTypes(brain::templates::Slot& slot)
{
    m_subTypes.add(slot);
}

CellI& Template::getParamType()
{
    if (!m_parametersType) {
        m_parametersType = std::make_unique<Type>(kb, std::format("{}<T>::parameters", label()));
        Type& type       = *m_parametersType;
        Visitor::visitList(m_parameters[kb.cells.list], [this, &type](CellI& slot, int i) {
            type.addSlot(slot[kb.cells.slotRole], slot[kb.cells.slotType]);
        });
    }

    return *m_parametersType;
}

Type& Template::compile(CellI& param)
{
    Type& type = *new Type(kb);
    std::stringstream ss;
    Visitor::visitList(m_parameters[kb.cells.list], [this, &ss, &param](CellI& slot, int i) {
        CellI& role = slot[kb.cells.slotRole];
        if (!param.has(role)) {
            return;
        }
        if (i != 0) {
            ss << ", ";
        }
        ss << param[role].label();
    });
    type.label(std::format("{}<{}>", label(), ss.str()));
    Visitor::visitList(m_slots, [this, &type, &param](CellI& slot, int i) {
        type.addSlot(compileCell(slot[kb.cells.slotRole], param, type), compileCell(slot[kb.cells.slotType], param, type));
    });
    Visitor::visitList(m_subTypes, [this, &type, &param](CellI& slot, int i) {
        type.addSubType(compileCell(slot[kb.cells.slotRole], param, type), static_cast<Template&>(compileCell(slot[kb.cells.slotType], param, type)).compile(param));
    });

    return type;
}

CellI& Template::compileCell(CellI& descriptor, CellI& param, CellI& selfType)
{
    if (&descriptor.type() == &kb.type.template_.Cell) {
        return descriptor[kb.coding.value];
    } else if (&descriptor.type() == &kb.type.template_.Parameter) {
        return param[descriptor[kb.coding.value]];
    } else if (&descriptor.type() == &kb.type.template_.TemplateOf) {
        Template& templateObj = static_cast<Template&>(descriptor[kb.coding.template_]);
        Object paramObject(kb, templateObj.getParamType());
        paramObject.set(compileCell(descriptor[kb.coding.parameter], param, selfType), compileCell(descriptor[kb.coding.value], param, selfType));
        return templateObj.compile(paramObject);
    } else if (&descriptor.type() == &kb.type.template_.SelfType) {
        return selfType;
    }

    throw "Unknown template descriptor!";
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
            m_digitsList.reset(new RefList(kb, m_digits));
        }

        return *m_digitsList;
    }

    return kb.cells.emptyObject;
}

void Number::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

int Number::value() const
{
    return m_value;
}

void Number::value(int newValue)
{
    m_value = newValue;
    m_digitsList.reset();
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
        return kb.type.String;
    } else if (&role == &kb.coding.value) {
        if (m_characters.empty()) {
            calculateCharacters();
            m_charactersList.reset(new RefList(kb, m_characters));
        }

        return *m_charactersList;
    } else {
        return kb.cells.emptyObject;
    }
}

void String::accept(Visitor& visitor)
{
    visitor.visit(*this);
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
        return kb.type.Color;
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

void Color::accept(Visitor& visitor)
{
    visitor.visit(*this);
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
        return kb.type.Pixel;
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

void Pixel::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

const input::Color& Pixel::color() const
{
    return m_inputColor;
}

// ============================================================================
Picture::Picture(brain::Brain& kb, input::Picture& picture) :
    CellI(kb, picture.label()),
    m_width(picture.width()),
    m_height(picture.height()),
    m_widthCell(kb.pools.numbers.get(m_width)),
    m_heightCell(kb.pools.numbers.get(m_height))
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
    m_pixelsList.reset(new RefList(kb, m_pixels));
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
        return kb.type.Picture;
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

void Picture::accept(Visitor& visitor)
{
    visitor.visit(*this);
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
    CellI(kb, "Same"),
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
        return kb.type.op.Same;
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

void Same::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

// ============================================================================
NotSame::NotSame(brain::Brain& kb, pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs) :
    CellI(kb, "NotSame"),
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
        return kb.type.op.NotSame;
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

void NotSame::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

// ============================================================================
Equal::Equal(brain::Brain& kb, pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs) :
    CellI(kb, "Equal"),
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
        return kb.type.op.Equal;
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

void Equal::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

// ============================================================================
NotEqual::NotEqual(brain::Brain& kb, pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs) :
    CellI(kb, "NotEqual"),
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
        return kb.type.op.NotEqual;
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

void NotEqual::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

// ============================================================================
Has::Has(brain::Brain& kb, pipeline::Base& output, pipeline::Base& cell, pipeline::Base& role) :
    CellI(kb, "Has"),
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
        return kb.type.op.Has;
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

void Has::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

// ============================================================================
Get::Get(brain::Brain& kb, pipeline::Base& output, pipeline::Base& cell, pipeline::Base& role) :
    CellI(kb, "Get"),
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
        return kb.type.op.Get;
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

void Get::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

// ============================================================================
Set::Set(brain::Brain& kb, pipeline::Base& output, pipeline::Base& cell, pipeline::Base& role, pipeline::Base& value) :
    CellI(kb, "Set"),
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
        return kb.type.op.Set;
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

void Set::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

namespace logic {

// ============================================================================
And::And(brain::Brain& kb, pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs) :
    CellI(kb, "And"),
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
        return kb.type.op.logic.And;
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

void And::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

// ============================================================================
Or::Or(brain::Brain& kb, pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs) :
    CellI(kb, "Or"),
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
        return kb.type.op.logic.Or;
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

void Or::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

// ============================================================================
Not::Not(brain::Brain& kb, pipeline::Base& output, pipeline::Base& input) :
    CellI(kb, "Not"),
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
        return kb.type.op.logic.Not;
    }
    if (&role == &kb.coding.input) {
        return m_input;
    }
    if (&role == &kb.coding.output) {
        return m_output;
    }

    return kb.cells.emptyObject;
}

void Not::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

} // namespace logic
namespace math {

// ============================================================================
Add::Add(brain::Brain& kb, pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs) :
    CellI(kb, "Add"),
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
        return kb.type.op.math.Add;
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

void Add::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

// ============================================================================
Subtract::Subtract(brain::Brain& kb, pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs) :
    CellI(kb, "Subtract"),
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
        return kb.type.op.math.Subtract;
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

void Subtract::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

// ============================================================================
Multiply::Multiply(brain::Brain& kb, pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs) :
    CellI(kb, "Multiply"),
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
        return kb.type.op.math.Multiply;
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

void Multiply::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

// ============================================================================
Divide::Divide(brain::Brain& kb, pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs) :
    CellI(kb, "Divide"),
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
        return kb.type.op.math.Divide;
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

void Divide::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

// ============================================================================
LessThan::LessThan(brain::Brain& kb, pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs) :
    CellI(kb, "LessThan"),
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
        return kb.type.op.math.LessThan;
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

void LessThan::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

// ============================================================================
GreaterThan::GreaterThan(brain::Brain& kb, pipeline::Base& output, pipeline::Base& lhs, pipeline::Base& rhs) :
    CellI(kb, "GreaterThan"),
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
        return kb.type.op.math.GreaterThan;
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

void GreaterThan::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

} // namespace math
} // namespace op
namespace pipeline {
// ============================================================================
Base::Base(brain::Brain& kb, Base* first, const std::string& label) :
    CellI(kb, label),
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
Void::Void(brain::Brain& kb, const std::string& label) :
    Base(kb, this, label), m_current(this)
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
        return kb.type.pipeline.Void;
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

void Void::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

// ============================================================================
Input::Input(brain::Brain& kb, CellI& value, const std::string& label) :
    Base(kb, this, label), m_value(&value), m_current(this)
{
}

Input::Input(brain::Brain& kb, CellI* value, const std::string& label) :
    Base(kb, this, label), m_value(value), m_current(this)
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
        return kb.type.pipeline.Input;
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

void Input::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

// ============================================================================
New::New(brain::Brain& kb, Type& objectType, const std::string& label) :
    Base(kb, this, label), m_objectType(objectType), m_current(this)
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

void New::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

// ============================================================================
Fork::Fork(brain::Brain& kb, Base& input, const std::string& label) :
    Base(kb, input.m_first, label), m_input(input)
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
        return kb.type.pipeline.Fork;
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

void Fork::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

void Fork::addBranch(Base& cell)
{
    m_branch = &cell;
}

// ============================================================================
Delete::Delete(brain::Brain& kb, Base& input, const std::string& label) :
    Base(kb, input.m_first, label), m_input(&input)
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
        return kb.type.pipeline.Delete;
    }
    if (&role == &kb.sequence.first) {
        return *m_first;
    }
    if (&role == &kb.coding.input && m_input) {
        return *m_input;
    }

    return kb.cells.emptyObject;
}

void Delete::accept(Visitor& visitor)
{
    visitor.visit(*this);
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
        return kb.type.pipeline.Node;
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

void Node::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

// ============================================================================
IfThen::IfThen(brain::Brain& kb, Base& input, const std::string& label) :
    Base(kb, input.m_first, label), m_input(input)
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
        return kb.type.pipeline.IfThen;
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

void IfThen::accept(Visitor& visitor)
{
    visitor.visit(*this);
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
DoWhile::DoWhile(brain::Brain& kb, Base& input, const std::string& label) :
    Base(kb, input.m_first, label), m_input(input)
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
        return kb.type.pipeline.DoWhile;
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

void DoWhile::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

// ============================================================================
While::While(brain::Brain& kb, Base& input, const std::string& label) :
    Base(kb, input.m_first, label), m_input(input)
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
        return kb.type.pipeline.While;
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

void While::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

} // namespace pipeline
} // namespace control

void Visitor::visitList(CellI& list, std::function<void(CellI& value, int i)> visitFn)
{
    brain::Brain& kb = list.kb;
    int i            = 0;

    CellI* currentListItemPtr = list.has(kb.sequence.first) ? &list[kb.sequence.first] : nullptr;
    while (currentListItemPtr) {
        CellI& currentListItem = *currentListItemPtr;
        CellI& value           = currentListItem[kb.coding.value];

        visitFn(value, i++);

        currentListItemPtr = currentListItem.has(kb.sequence.next) ? &currentListItem[kb.sequence.next] : nullptr;
    }
}

bool tryVisitWith(CellI& cell, Visitor& visitor)
{
    brain::Brain& kb = cell.kb;

    if (&cell.type() == &kb.type.Slot) {
        visitor.visit(static_cast<Slot&>(cell));
        return true;
    }
    if (&cell.type() == &kb.type.Type_) {
        visitor.visit(static_cast<Type&>(cell));
        return true;
    }
    if (&cell.type() == &kb.type.Number) {
        visitor.visit(static_cast<Number&>(cell));
        return true;
    }
    if (&cell.type() == &kb.type.String) {
        visitor.visit(static_cast<String&>(cell));
        return true;
    }
    if (&cell.type() == &kb.type.Color) {
        visitor.visit(static_cast<hybrid::Color&>(cell));
        return true;
    }
    if (&cell.type() == &kb.type.Pixel) {
        visitor.visit(static_cast<hybrid::Pixel&>(cell));
        return true;
    }
    if (&cell.type() == &kb.type.Picture) {
        visitor.visit(static_cast<hybrid::Picture&>(cell));
        return true;
    }

    return false;
}

} // namespace cells
} // namespace synth
