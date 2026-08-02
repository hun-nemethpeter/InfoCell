#include "Cells.h"
#include "OpActivator.h"
#include "World.h"

#include <utility>

#pragma warning(disable : 4996)
#include <utf8.h>
#include "printers/ValuePrinter.h"

namespace infocell {
namespace cells {
using InitPhase = World::InitPhase;
#pragma region CellI
// ============================================================================
int CellI::s_constructed = 0;
int CellI::s_destructed  = 0;

CellI::CellI(World& w) :
    w(w)
{
    s_constructed += 1;
}

CellI::CellI(World& w, const std::string& label) :
    w(w), m_label(label)
{
    s_constructed += 1;
}

CellI::CellI(const CellI& rhs) :
    w(rhs.w), m_label(rhs.m_label)
{
    throw "Unexpected cell copy";
}

CellI::~CellI()
{
    s_destructed += 1;
}

bool CellI::has(const std::string& key)
{
    return has(w.name(key));
}

void CellI::set(const std::string& key, CellI& value)
{
    set(w.name(key), value);
}

void CellI::erase(const std::string& key)
{
    erase(w.name(key));
}

CellI& CellI::operator[](const std::string& key)
{
    return (*this)[w.name(key)];
}

bool CellI::missing(const std::string& key)
{
    return !has(w.name(key));
}

CellI& CellI::get(const std::string& key)
{
    return (*this)[w.name(key)];
}

bool CellI::missing(CellI& key)
{
    return !has(key);
}

CellI& CellI::get(CellI& key)
{
    return (*this)[key];
}

CellI& CellI::__type__()
{
    return (*this)[w.id.__type__];
}

CellI& CellI::slotList()
{
    return __type__()[w.id.slots][w.id.list];
}

void CellI::eval()
{
    return (*this)();
}

std::string CellI::label() const
{
    return m_label;
}

void CellI::label(const std::string& label)
{
    m_label = label;
}

std::string CellI::printAsValue()
{
    CellValuePrinter valuePrinter(w);

    return valuePrinter.print(*this);
}

bool CellI::isA(CellI& ptype)
{
    return &__type__() == &ptype || (__type__().has(w.id.memberOf) && __type__()[w.id.memberOf][w.id.index].has(ptype));
}

bool CellI::operator==(CellI& rhs)
{
    if (this == &rhs) {
        return true;
    }
    if (&__type__() != &rhs.__type__()) {
        return false;
    }

    for (CellI& slot : slotList()) {
        CellI& key = slot[w.id.key];

        bool hasLeftSlot = has(key);
        if (hasLeftSlot != rhs.has(key)) {
            return false;
        }
        if (hasLeftSlot && (&(*this)[key] != &rhs[key])) {
            return false;
        }
    }

    return true;
}

bool CellI::operator==(const CellI& rhs) const
{
    return const_cast<CellI&>(*this).operator==(const_cast<CellI&>(rhs));
}

bool CellI::operator!=(CellI& rhs)
{
    return !((*this) == rhs);
}

CellI::Iterator::Iterator(CellI* list, CellI* node) :
    m_list(list), m_node(node)
{
}

CellI& CellI::Iterator::operator*() const
{
    World& w = m_node->w;
    return (*m_node)[w.id.value];
}

CellI::Iterator& CellI::Iterator::operator++()
{
    m_node = m_node->getNextOrNullptr();

    return *this;
}

bool CellI::Iterator::operator==(const Iterator& other) const
{
    return m_node == other.m_node;
}

CellI::Iterator CellI::begin()
{
    if (has(w.id.first)) {
        return Iterator(this, &get(w.id.first));
    } else {
        return Iterator(this, nullptr);
    }
}

CellI::Iterator CellI::end()
{
    return Iterator(this, nullptr);
}

CellI* CellI::getNextOrNullptr()
{
    return has(w.id.next) ? &get(w.id.next) : nullptr;
}

#pragma endregion
#pragma region Object
Param::Param(const std::string& key, CellI& value) :
    key(value.w.name(key)), value(value) { }

// ============================================================================
Object::Object(World& w, CellI& type, const std::string& label) :
    CellI(w, label),
    m_type(type)
{
    m_slots[&w.id.__type__] = &type;
}

Object::Object(World& w, CellI& type, CellI& constructor, const std::string& label) :
    CellI(w, label),
    m_type(type)
{
    m_slots[&w.id.__type__] = &type;
    getMethod(constructor)();
}

Object::Object(World& w, CellI& type, CellI& constructor, Param param1, const std::string& label) :
    CellI(w, label),
    m_type(type)
{
    m_slots[&w.id.__type__] = &type;

    CellI& method = getMethod(constructor);
    setFnParam(method, param1);
    method();
}

Object::Object(World& w, CellI& type, CellI& constructor, Param param1, Param param2, const std::string& label) :
    CellI(w, label),
    m_type(type)
{
    m_slots[&w.id.__type__] = &type;

    CellI& method = getMethod(constructor);
    setFnParam(method, param1);
    setFnParam(method, param2);
    method();
}

Object::Object(World& w, CellI& type, CellI& constructor, Param param1, Param param2, Param param3, const std::string& label) :
    CellI(w, label),
    m_type(type)
{
    m_slots[&w.id.__type__] = &type;

    CellI& method = getMethod(constructor);
    setFnParam(method, param1);
    setFnParam(method, param2);
    setFnParam(method, param3);
    method();
}

bool Object::s_debugFunctionCalls = false;
bool Object::s_worldConstructedAndAlive = false;

Object::Object(World& w, CellI& type, CellI& constructor, Param param1, Param param2, Param param3, Param param4, const std::string& label) :
    CellI(w, label),
    m_type(type)
{
    m_slots[&w.id.__type__] = &type;

    CellI& method = getMethod(constructor);
    setFnParam(method, param1);
    setFnParam(method, param2);
    setFnParam(method, param3);
    setFnParam(method, param4);
    method();
}

Object::~Object()
{
    if (!s_worldConstructedAndAlive) {
        return;
    }
    if (!hasMethod(w.id.destructor)) {
        return;
    }
    destructor();
}

bool Object::has(CellI& key)
{
    if (&key == &w.id.__type__)
        return true;

    return m_slots.find(&key) != m_slots.end();
}

void Object::set(CellI& key, CellI& value)
{
    if ((&key == &w.id.__type__) && !((&__type__() == &w.std.Index))) {
        throw "Type change not allowed.";
    }
    if (w.initPhase() == InitPhase::Init || w.initPhase() == InitPhase::Compiling) {
        m_slots[&key] = &value;
        return;
    }
    if (isA(w.std.Index) || __type__()[w.id.slots][w.id.index].has(key)) {
        m_slots[&key] = &value;
    } else {
        throw "The type doesn't contains this key.";
    }
}

void Object::erase(CellI& key)
{
    if (&key == &w.id.__type__) {
        throw "Type change not allowed.";
    }

    auto slotIt = m_slots.find(&key);
    if (slotIt == m_slots.end()) {
        return;
    }
    m_slots.erase(slotIt);
}

void Object::createSelfStack()
{
    createStack(*this);
    initLocalVars(*this);
}

void Object::runAsCall()
{
    Object& method = static_cast<Object&>(get(w.id.method));
    method.createSelfStack();
    set(w.id.stack, method);

    (*this)();
}

void Object::operator()()
{
    OpActivator opActivator(*this);
    opActivator.activate();
}

CellI& Object::operator[](CellI& key)
{
    auto findIt = m_slots.find(&key);
    if (findIt == m_slots.end())
        throw "No such key!";

    return *findIt->second;
}

void Object::destructor()
{
    getMethod(w.id.destructor)();
}

CellI& Object::method(const std::string& key)
{
    return method(w.name(key));
}

CellI& Object::method(CellI& key)
{
    CellI& method = getMethod(key);
    method();

    return getFnValue(method);
}

CellI& Object::method(CellI& key, Param param1)
{
    CellI& method = getMethod(key);
    setFnParam(method, param1);
    method();

    return getFnValue(method);
}

CellI& Object::method(CellI& key, Param param1, Param param2)
{
    CellI& method = getMethod(key);
    setFnParam(method, param1);
    setFnParam(method, param2);
    method();

    return getFnValue(method);
}

CellI& Object::method(CellI& key, Param param1, Param param2, Param param3)
{
    CellI& method = getMethod(key);
    setFnParam(method, param1);
    setFnParam(method, param2);
    setFnParam(method, param3);
    method();

    return getFnValue(method);
}

CellI& Object::method(CellI& key, Param param1, Param param2, Param param3, Param param4)
{
    CellI& method = getMethod(key);
    setFnParam(method, param1);
    setFnParam(method, param2);
    setFnParam(method, param3);
    setFnParam(method, param4);
    method();

    return getFnValue(method);
}

CellI& Object::smethod(CellI& key)
{
    CellI& method = getStaticMethod(key);
    method();

    return getFnValue(method);
}

CellI& Object::smethod(CellI& key, Param param1)
{
    CellI& method = getStaticMethod(key);
    setFnParam(method, param1);
    method();

    return getFnValue(method);
}

CellI& Object::smethod(CellI& key, Param param1, Param param2)
{
    CellI& method = getStaticMethod(key);
    setFnParam(method, param1);
    setFnParam(method, param2);
    method();

    return getFnValue(method);
}

CellI& Object::smethod(CellI& key, Param param1, Param param2, Param param3)
{
    CellI& method = getStaticMethod(key);
    setFnParam(method, param1);
    setFnParam(method, param2);
    setFnParam(method, param3);
    method();

    return getFnValue(method);
}

CellI& Object::smethod(CellI& key, Param param1, Param param2, Param param3, Param param4)
{
    CellI& method = getStaticMethod(key);
    setFnParam(method, param1);
    setFnParam(method, param2);
    setFnParam(method, param3);
    setFnParam(method, param4);
    method();

    return getFnValue(method);
}

bool Object::hasMethod(CellI& key)
{
    return __type__().has(w.id.methods) && __type__()[w.id.methods].has(w.id.index) && __type__()[w.id.methods][w.id.index].has(key);
}

CellI& Object::getMethod(CellI& key)
{
    if (__type__().has(w.id.methods)) {
        CellI& methodsIndex = __type__()[w.id.methods][w.id.index];
        if (methodsIndex.has(key)) {
            CellI& method = methodsIndex[key][w.id.value];
            createStack(method);
            initLocalVars(method);
            setSelf(method);
            return method;
        }
    }

    throw "No such method!";
}

CellI& Object::getStaticMethod(CellI& key)
{
    if (has(w.id.methods)) {
        CellI& methodsIndex = (*this)[w.id.methods][w.id.index];
        if (methodsIndex.has(key)) {
            CellI& method = methodsIndex[key][w.id.value];
            createStack(method);
            initLocalVars(method);
            setSelf(method);
            return method;
        }
    }

    throw "No such method!";
}

void Object::createStack(CellI& method)
{
    Object& inputIndex    = *new Object(w, w.std.Index, "StackFrame1.InputIndex");
    Object& stackFrame    = *new Object(w, w.std.StackFrame, "StackFrame1");
    Object& stackListNode0 = *new Object(w, w.std.ListNode, "StackListNode0");
    Object& stackListNode1 = *new Object(w, w.std.ListNode, "StackListNode1");
    stackFrame.set(w.id.method, method);
    stackFrame.set(w.id.input, inputIndex);

    stackListNode0.set(w.id.next, stackListNode1);
    stackListNode1.set(w.id.value, stackFrame);
    stackListNode1.set(w.id.previous, stackListNode0);
    method.set(w.id.stack, stackListNode1);
}

void Object::clearStack(CellI& method)
{
    CellI* stackListNode1 = &method["stack"];
    CellI* stackListNode0 = &(*stackListNode1)["previous"];
    CellI* stackFrame     = &(*stackListNode1)["value"];
    CellI* inputIndex     = &(*stackFrame)["input"];
    if (method.has("localVars")) {
        CellI* localVarsIndex = &(*stackFrame)["localVars"];
        delete localVarsIndex;
        // TODO
    }

    delete stackListNode1;
    delete stackListNode0;
    delete stackFrame;
    delete inputIndex;

    method.erase("stack");
    method.erase("value");
}

void Object::initLocalVars(CellI& method)
{
    if (method.missing(w.id.localVars)) {
        return;
    }

    CellI& localVarsList   = method[w.id.localVars].slotList();
    Object& localVarsIndex = *new Object(w, method[w.id.localVars].__type__(), "LocalVarsIndex");
    CellI& stackFrame      = method[w.id.stack][w.id.value];
    stackFrame.set(w.id.localVars, localVarsIndex);
    for (CellI& slot : localVarsList) {
        auto& key        = slot[w.id.key];
        Object& localVar = *new Object(w, w.std.op.Var, fmt::format("var {}", key.label()));
        localVar.set(w.id.type, slot[w.id.type]);
        localVarsIndex.set(key, localVar);
    }
}

CellI& Object::getFnValue(CellI& method)
{
    if (method.has(w.id.returnType)) {
        return method[w.id.value];
    }

    return w.id.emptyObject;
}

void Object::setSelf(CellI& method)
{
    setFnParam(method, { w.id.self, *this });
}

void Object::setFnParam(CellI& fn, Param param)
{
    if (fn.has(w.id.parameters)) {
        CellI& inputsIndex = fn[w.id.parameters][w.id.index];
        if (inputsIndex.has(param.key)) {
            fn[w.id.stack][w.id.value][w.id.input].set(param.key, param.value);
        } else {
            throw "No such param!";
        }
    }
}

#pragma endregion
#pragma region List::Node
// ============================================================================
List::Node::Node(World& w, List& list, CellI& value) :
    CellI(w),
    m_list(list),
    m_value(value)
{
}

bool List::Node::has(CellI& key)
{
    if (&key == &w.id.__type__ || &key == &w.id.value) {
        return true;
    }
    if (&key == &w.id.previous && m_previous) {
        return true;
    }
    if (&key == &w.id.next && m_next) {
        return true;
    }

    return false;
}

void List::Node::set(CellI& key, CellI& value)
{
    // Do nothing
}

void List::Node::erase(CellI& key)
{
    if (&key == &w.id.next) {
        m_next = nullptr;
    }
    if (&key == &w.id.previous) {
        m_previous = nullptr;
    }
    throw "No such key!";
}

void List::Node::operator()()
{
    // Do nothing
}

CellI& List::Node::operator[](CellI& key)
{
    if (&key == &w.id.__type__) {
        if (w.initPhase() != InitPhase::FullyConstructed) {
            // we give back a generic List before compilation
            return w.std.List;
        }
        if (!m_selfType) {
            m_selfType = &w.getStruct(w.templateId("std::ListNode", w.id.valueType, m_list.m_valueType));
        }
        return *m_selfType;
    }
    if (&key == &w.id.previous) {
        if (m_previous)
            return *m_previous;
        else
            throw "No such key!";
    }
    if (&key == &w.id.next) {
        if (m_next)
            return *m_next;
        else
            throw "No such key!";
    }
    if (&key == &w.id.value) {
        return m_value;
    }

    throw "No such key!";
}
#pragma endregion
#pragma region List
// ============================================================================
List::List(World& w, CellI& valueType, const std::string& label) :
    CellI(w, label),
    m_valueType(valueType)
{
}

bool List::has(CellI& key)
{
    if (&key == &w.id.__type__ || &key == &w.id.size) {
        return true;
    }
    if (&key == &w.id.first && m_firstNode) {
        return true;
    }
    if (&key == &w.id.last && m_lastNode) {
        return true;
    }

    return false;
}

void List::set(CellI& key, CellI& value)
{
    throw "Not supported";
}

void List::erase(CellI& key)
{
    throw "Not supported";
}

void List::operator()()
{
    // Do nothing, this is a data cell
}

CellI& List::operator[](CellI& key)
{
    if (&key == &w.id.__type__) {
        if (!m_selfType) {
            m_selfType = &w.getStruct(w.templateId("std::List", w.id.valueType, m_valueType));
        }
        return *m_selfType;
    }
    if (&key == &w.id.first) {
        return *m_firstNode;
    }
    if (&key == &w.id.last) {
        return *m_lastNode;
    }
    if (&key == &w.id.size) {
        int size = (int)m_size;

        return w.pools.numbers.get(size);
    }

    throw "No such key!";
}

List::Node* List::add(CellI& value)
{
    Node* node = new Node(w, *this, value);
    if (m_lastNode) {
        m_lastNode->m_next = node;
        node->m_previous   = m_lastNode;
    }
    m_lastNode = node;
    if (!m_firstNode) {
        m_firstNode = m_lastNode;
    }
    ++m_size;

    return node;
}


List::Node* List::addFront(CellI& value)
{
    Node* node = new Node(w, *this, value);
    if (m_firstNode) {
        m_firstNode->m_previous = node;
        node->m_next            = m_firstNode;
    }
    m_firstNode = node;
    if (!m_lastNode) {
        m_lastNode = node;
    }
    ++m_size;

    return node;
}

void List::remove(Node* node)
{
    if (node->m_previous) {
        node->m_previous->m_next = node->m_next;
    } else {
        m_firstNode = node->m_next;
    }
    if (node->m_next) {
        node->m_next->m_previous = node->m_previous;
    } else {
        m_lastNode = node->m_previous;
    }
    --m_size;
}

bool List::empty() const
{
    return m_size == 0;
}

int List::size()
{
    return m_size;
}

void List::clear()
{
    if (empty()) {
        return;
    }
    for (CellI* node = m_firstNode; node;) {
        CellI& currentNode = *node;
        CellI* nextNode    = nullptr;
        if (currentNode.has(w.id.next)) {
            nextNode = &currentNode[w.id.next];
        }
        delete node;
        node = nextNode;
    }
}

#pragma endregion
#pragma region Struct
// ============================================================================
Struct::Struct(World& w, const std::string& label) :
    CellI(w, label),
    m_slots(*new Map(w, w.std.Cell, w.std.ast.Slot))
{
}

Struct::Struct(World& w, WithRecursiveType recursiveType, const std::string& label) :
    CellI(w, label),
    m_slots(*new Map(w, w.std.Cell, w.std.ast.Slot, *this))
{
}

bool Struct::has(CellI& key)
{
    if (&key == &w.id.__type__) {
        return true;
    }
    if (&key == &w.id.name) {
        return true;
    }
    if (&key == &w.id.slots) {
        return true;
    }
    if (&key == &w.id.typeAliases) {
        return m_typeAliases;
    }
    if (&key == &w.id.memberOf) {
        return m_memberOf;
    }
    if (&key == &w.id.asts) {
        return m_asts;
    }
    if (&key == &w.id.methods) {
        return m_methods;
    }

    return false;
}

void Struct::set(CellI& key, CellI& value)
{
    throw "Not supported";
}

void Struct::erase(CellI& key)
{
    throw "Not supported";
}

void Struct::operator()()
{
    // Do nothing, this is a data cell
}

CellI& Struct::operator[](CellI& key)
{
    if (&key == &w.id.__type__) {
        return w.std.Struct;
    }
    if (&key == &w.id.name) {
        if (m_name) {
            return *m_name;
        } else {
            m_name = &w.name(label());
            return *m_name;
        }
    }
    if (&key == &w.id.slots) {
        return m_slots;
    }
    if (&key == &w.id.typeAliases) {
        return *m_typeAliases;
    }
    if (&key == &w.id.memberOf) {
        return *m_memberOf;
    }
    if (&key == &w.id.asts) {
        return *m_asts;
    }
    if (&key == &w.id.methods) {
        return *m_methods;
    }

    throw "No such key!";
}

void Struct::addSlot(CellI& key, CellI& slot)
{
    m_slots.add(key, slot);
}

bool Struct::hasSlot(CellI& key)
{
    return m_slots.hasKey(key);
}

void Struct::removeSlot(CellI& key)
{
    m_slots.remove(key);
}
#pragma endregion
#pragma region Index
// ============================================================================
Index::Index(World& w, const std::string& label) :
    CellI(w, label),
    m_type(new Struct(w, Struct::WithRecursiveType::Yes))
{
}

Index::Index(World& w, Struct& indexType, const std::string& label) :
    CellI(w, label),
    m_type(&indexType),
    m_recursiveType(true)
{
}

bool Index::has(CellI& key)
{
    if (&key == &w.id.__type__) {
        return true;
    }
    if (m_slots.find(&key) != m_slots.end()) {
        return true;
    }

    return false;
}

void Index::set(CellI& key, CellI& value)
{
    if (&key == &w.id.__type__) {
        throw "The type key can not be changed!";
    }
    m_slots[&key] = &value;
}

void Index::erase(CellI& key)
{
    if (!m_type->hasSlot(key)) {
        return;
    }
    m_slots.erase(&key);
    m_type->removeSlot(key);
}

void Index::operator()()
{
    // Do nothing, this is a data cell
}

CellI& Index::operator[](CellI& key)
{
    if (&key == &w.id.__type__) {
        return *m_type;
    }
    auto slotIt = m_slots.find(&key);
    if (slotIt != m_slots.end()) {
        return *slotIt->second;
    }

    throw "No such key!";
}

void Index::insert(CellI& key, CellI& value)
{
    if (&key == &w.id.__type__) {
        throw "The type key can not be changed!";
    }
    m_slots[&key] = &value;
    if (m_recursiveType) {
        return;
    }
    Object& slot = *new Object(w, w.std.ast.Slot);
    slot.set(w.id.key, key);
    slot.set(w.id.type, w.std.ast.Slot);
    m_type->addSlot(key, slot);
}

bool Index::empty() const
{
    return m_slots.empty();
}

int Index::size()
{
    return (int)m_slots.size();
}
#pragma endregion
#pragma region Map
// ============================================================================
Map::Map(World& w, CellI& keyType, CellI& valueType, const std::string& label) :
    CellI(w, label),
    m_list(w, valueType),
    m_index(w),
    m_keyType(keyType),
    m_valueType(valueType)
{
}

Map::Map(World& w, CellI& keyType, CellI& valueType, Struct& indexType, const std::string& label) :
    CellI(w, label),
    m_list(w, valueType),
    m_index(w, indexType),
    m_keyType(keyType),
    m_valueType(valueType)
{
}

bool Map::has(CellI& key)
{
    if (&key == &w.id.__type__) {
        return true;
    }
    if (&key == &w.id.list) {
        return true;
    }
    if (&key == &w.id.index) {
        return true;
    }
    if (&key == &w.id.size) {
        return true;
    }
    if (&key == &w.id.first && m_list.has(key)) {
        return true;
    }
    if (&key == &w.id.last && m_list.has(key)) {
        return true;
    }

    return false;
}

void Map::set(CellI& key, CellI& value)
{
    if (!hasKey(key)) {
        add(key, value);
        return;
    }
    m_index.set(key, value);
}

void Map::erase(CellI& key)
{
    throw "Not supported";
}

void Map::operator()()
{
    // Do nothing, this is a data cell
}

CellI& Map::operator[](CellI& key)
{
    if (&key == &w.id.__type__) {
        if (!m_selfType) {
            if (&m_keyType == &w.std.Struct && &m_keyType == &w.std.Struct) {
                m_selfType = &w.std.Map_Struct_Struct;
            } else {
                m_selfType = &w.getStruct(w.templateId("std::Map", w.id.keyType, m_keyType, w.id.valueType, m_valueType));
            }
        }
        return *m_selfType;
    }
    if (&key == &w.id.list) {
        return m_list;
    }
    if (&key == &w.id.index) {
        return m_index;
    }
    if (&key == &w.id.size) {
        return w.pools.numbers.get(m_size);
    }
    if (&key == &w.id.first) {
        return m_list.get(key);
    }
    if (&key == &w.id.last) {
        return m_list.get(key);
    }

    throw "No such key!";
}

bool Map::hasKey(CellI& key)
{
    return m_index.has(key);
}

CellI& Map::getValue(CellI& key)
{
    if (m_index.has(key)) {
        return m_index[key][w.id.value];
    }
    throw "No such key!";
}

void Map::add(CellI& key, CellI& value)
{
    if (&key == &w.id.__type__) {
        throw "id.type can not be stored in a map!";
    }
    if (m_index.has(key)) {
        throw "A value already registered with this key";
    }
    List::Node& node = *m_list.add(value);
    m_index.insert(key, node);
    ++m_size;
}

void Map::addFront(CellI& key, CellI& value)
{
    if (&key == &w.id.__type__) {
        throw "id.type can not be stored in a map!";
    }
    if (m_index.has(key)) {
        throw "A value already registered with this key";
    }
    List::Node& node = *m_list.addFront(value);
    m_index.insert(key, node);
    ++m_size;
}

void Map::remove(CellI& key)
{
    if (!m_index.has(key)) {
        return;
    }
    List::Node* node = &static_cast<List::Node&>(m_index[key]);
    m_list.remove(node);
    m_index.erase(key);
    --m_size;
}

bool Map::empty() const
{
    return m_size == 0;
}

int Map::size()
{
    return m_size;
}
#pragma endregion
#pragma region TrieMap
// ============================================================================
TrieMap::TrieMap(World& w, CellI& keyType, CellI& valueType, const std::string& label) :
    CellI(w, label),
    m_list(w, valueType),
    m_rootNode(w, w.std.TrieMapNode, "TrieNode_Root"),
    m_keyType(keyType),
    m_valueType(valueType)
{
}

bool TrieMap::has(CellI& key)
{
    if (&key == &w.id.__type__) {
        return true;
    }
    if (&key == &w.id.list) {
        return true;
    }
    if (&key == &w.id.index) {
        return true;
    }
    if (&key == &w.id.size) {
        return true;
    }
    if (&key == &w.id.first && m_list.has(key)) {
        return true;
    }
    if (&key == &w.id.last && m_list.has(key)) {
        return true;
    }

    return false;
}

void TrieMap::set(CellI& key, CellI& value)
{
    throw "Not supported";
}

void TrieMap::erase(CellI& key)
{
    throw "Not supported";
}

void TrieMap::operator()()
{
    // Do nothing, this is a data cell
}

CellI& TrieMap::operator[](CellI& key)
{
    if (&key == &w.id.__type__) {
        if (!m_selfType) {
            m_selfType = &w.getStruct(w.templateId("std::TrieMap", w.id.keyType, m_keyType, w.id.valueType, m_valueType));
        }
        return *m_selfType;
    }
    if (&key == &w.id.list) {
        return m_list;
    }
    if (&key == &w.id.rootNode) {
        return m_rootNode;
    }
    if (&key == &w.id.size) {
        return w.pools.numbers.get(m_size);
    }
    if (&key == &w.id.first) {
        return m_list.get(key);
    }
    if (&key == &w.id.last) {
        return m_list.get(key);
    }

    throw "No such key!";
}

// assuming key is a List
bool TrieMap::hasKey(CellI& key)
{
    CellI* currentNode = &m_rootNode;

    for (CellI& keyNode : key) {
        CellI* children = nullptr;
        if (currentNode->missing(w.id.children)) {
            currentNode = nullptr;
            break;
        }
        Index& childrenIndex = static_cast<Index&>(currentNode->get(w.id.children));
        if (childrenIndex.has(keyNode)) {
            children = &childrenIndex.get(keyNode);
        } else {
            currentNode = nullptr;
            break;
        }
        currentNode = children;
    }

    if (!currentNode || currentNode->missing(w.id.data)) {
        return false;
    }

    return true;
}

// assuming key is a List
CellI& TrieMap::getValue(CellI& key)
{
    CellI* currentNode = &m_rootNode;

    for (CellI& keyNode : key) {
        CellI* children = nullptr;
        if (currentNode->missing(w.id.children)) {
            currentNode = nullptr;
            break;
        }
        Index& childrenIndex = static_cast<Index&>(currentNode->get(w.id.children));
        if (childrenIndex.has(keyNode)) {
            children = &childrenIndex.get(keyNode);
        } else {
            currentNode = nullptr;
            break;
        }
        currentNode = children;
    }

    if (!currentNode || currentNode->missing(w.id.data)) {
        throw "No such key!";
    }

    return (*currentNode)[w.id.data][w.id.value][w.id.value];
}

// key can be anything we itarate over the members
bool TrieMap::hasValueWithDataKey(CellI& key)
{
    CellI* currentNode = &m_rootNode;

    for (CellI& slot : key.slotList()) {
        CellI& keyNode  = key[slot[w.id.key]];
        CellI* children = nullptr;
        if (currentNode->missing(w.id.children)) {
            currentNode = nullptr;
            break;
        }
        Index& childrenIndex = static_cast<Index&>(currentNode->get(w.id.children));
        if (childrenIndex.has(keyNode)) {
            children = &childrenIndex.get(keyNode);
        } else {
            currentNode = nullptr;
            break;
        }
        currentNode = children;
    }

    if (!currentNode || currentNode->missing(w.id.data)) {
        return false;
    }

    return true;
}

// key can be anything we itarate over the members
CellI& TrieMap::getValueWithDataKey(CellI& key)
{
    CellI* currentNode = &m_rootNode;

    for (CellI& slot : key.slotList()) {
        CellI& keyNode  = key[slot[w.id.key]];
        CellI* children = nullptr;
        if (currentNode->missing(w.id.children)) {
            currentNode = nullptr;
            break;
        }
        Index& childrenIndex = static_cast<Index&>(currentNode->get(w.id.children));
        if (childrenIndex.has(keyNode)) {
            children = &childrenIndex.get(keyNode);
        } else {
            currentNode = nullptr;
            break;
        }
        currentNode = children;
    }

    if (!currentNode || currentNode->missing(w.id.data)) {
        throw "No such key!";
    }

    return (*currentNode)[w.id.data][w.id.value][w.id.value];
}

// key can be anything we itarate over the members
void TrieMap::addWithDataKey(CellI& key, CellI& value)
{
    CellI* currentNode = &m_rootNode;

    for (CellI& slot : key.slotList()) {
        CellI& keyNode = key[slot[w.id.key]];
        CellI* child = nullptr;
        if (currentNode->missing(w.id.children)) {
            currentNode->set(w.id.children, *new Index(w));
        }
        Index& childrenIndex = static_cast<Index&>(currentNode->get(w.id.children));
        if (childrenIndex.has(keyNode)) {
            child = &childrenIndex.get(keyNode);
        } else {
            child = new Object(w, w.std.TrieMapNode);
            child->set(w.id.parent, *currentNode);
            childrenIndex.insert(keyNode, *child);
        }
        currentNode = child;
    }

    List::Node& node = *m_list.add(w.std.kvPair(key, value));
    currentNode->set(w.id.data, node);
    ++m_size;
}

// assuming key is a List
void TrieMap::add(CellI& key, CellI& value)
{
    CellI* currentNode = &m_rootNode;

    for (CellI& keyNode : key) {
        CellI* child = nullptr;
        if (currentNode->missing(w.id.children)) {
            currentNode->set(w.id.children, *new Index(w));
        }
        Index& childrenIndex = static_cast<Index&>(currentNode->get(w.id.children));
        if (childrenIndex.has(keyNode)) {
            child = &childrenIndex.get(keyNode);
        } else {
            child = new Object(w, w.std.TrieMapNode);
            child->set(w.id.parent, *currentNode);
            childrenIndex.insert(keyNode, *child);
        }
        currentNode = child;
    }

    List::Node& node = *m_list.add(w.std.kvPair(key, value));
    currentNode->set(w.id.data, node);
    ++m_size;
}

// assuming key is a List
void TrieMap::remove(CellI& key)
{
    if (&key[w.id.size] == &w._0_) {
        return;
    }

    CellI* currentNode = &m_rootNode;

    for (CellI& keyNode : key) {
        CellI* children = nullptr;
        if (currentNode->missing(w.id.children)) {
            currentNode = nullptr;
            break;
        }
        Index& childrenIndex = static_cast<Index&>(currentNode->get(w.id.children));
        if (childrenIndex.has(keyNode)) {
            children = &childrenIndex.get(keyNode);
        } else {
            currentNode = nullptr;
            break;
        }
        currentNode = children;
    }

    if (!currentNode || currentNode->missing(w.id.data)) {
        return;
    }
    List::Node* valueNode = &static_cast<List::Node&>((*currentNode)[w.id.data]);
    currentNode->erase(w.id.data);

    CellI* keyNodePtr = &key[w.id.last];
    while (currentNode->has(w.id.parent)) {
        CellI& keyNode = *keyNodePtr;
        CellI& parent = currentNode->get(w.id.parent);
        CellI& child = *currentNode;
        if (child.missing(w.id.data)) {
            if (child.missing(w.id.children) || ( child.has(w.id.children) && static_cast<Index&>(child[w.id.children]).empty())) {
                delete currentNode;
                parent[w.id.children].erase(keyNode[w.id.value]);
            }
        }
        currentNode = &parent;
        if (keyNode.has(w.id.previous)) {
            keyNodePtr = &keyNode[w.id.previous];
        } else {
            break;
        }
    }
    if (!valueNode) {
        return;
    }
    m_list.remove(valueNode);
    --m_size;
}

bool TrieMap::empty() const
{
    return m_size == 0;
}

int TrieMap::size()
{
    return m_size;
}
#pragma endregion
#pragma region Set
// ============================================================================
Set::Set(World& w, CellI& valueType, const std::string& label) :
    CellI(w, label),
    m_valueType(valueType),
    m_index(w)
{
}

bool Set::has(CellI& key)
{
    if (&key == &w.id.__type__ || &key == &w.id.size) {
        return true;
    }
    if (&key == &w.id.index) {
        return true;
    }

    return false;
}

void Set::set(CellI& key, CellI& value)
{
    throw "Not supported";
}

void Set::erase(CellI& key)
{
    throw "Not supported";
}

void Set::operator()()
{
    // Do nothing, this is a data cell
}

CellI& Set::operator[](CellI& key)
{
    if (&key == &w.id.__type__) {
        if (!m_selfType) {
            m_selfType = &w.getStruct(w.templateId("std::Set", w.id.valueType, m_valueType));
        }
        return *m_selfType;
    }
    if (&key == &w.id.index) {
        return m_index;
    }
    if (&key == &w.id.size) {
        int size = (int)m_size;

        return w.pools.numbers.get(size);
    }

    throw "No such key!";
}

bool Set::contains(CellI& key)
{
    return m_index.has(key);
}

void Set::add(CellI& value)
{
    if (m_index.has(value)) {
        return;
//        throw "A value already registered with this key";
    }
    m_index.insert(value, value);
    ++m_size;
}

void Set::remove(CellI& key)
{
    if (!m_index.has(key)) {
        return;
    }
    m_index.erase(key);
    --m_size;
}

bool Set::empty() const
{
    return m_size == 0;
}

int Set::size()
{
    return m_size;
}

CellI& Set::first()
{
    return m_index.slotList()["first"]["value"]["key"];
}
#pragma endregion
#pragma region Number
// ============================================================================
Number::Number(World& w, int value) :
    CellI(w),
    m_value(value)
{
}

bool Number::has(CellI& key)
{
    if (&key == &w.id.__type__ || &key == &w.id.value) {
        return true;
    }
    if (&key == &w.id.sign) {
        return m_value != 0;
    }

    return false;
}

void Number::set(CellI& key, CellI& value)
{
    throw "Changing a hybrid number cell is not possible!";
}

void Number::erase(CellI& key)
{
    throw "Changing a hybrid number cell is not possible!";
}

void Number::operator()()
{
    // Do nothing, this is a data cell
}

CellI& Number::operator[](CellI& key)
{
    if (&key == &w.id.__type__) {
        return w.std.Number;
    }

    if (&key == &w.id.sign && m_value != 0) {
        return m_value > 0 ? w.std.NumberSign.positive : w.std.NumberSign.negative;
    }

    if (&key == &w.id.value) {
        if (m_digits.empty()) {
            calculateDigits();
            m_digitsList.reset(new List(w, m_digits));
        }

        return *m_digitsList;
    }

    throw "No such key!";
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

void Number::increase()
{
    ++m_value;
    m_digitsList.reset();
}

void Number::calculateDigits()
{
    if (m_value == 0) {
        m_digits.push_back(&w.pools.digits[0]);
        return;
    }
    int value = m_value;
    while (value) {
        m_digits.push_back(&w.pools.digits[value % 10]);
        value /= 10;
    }
    std::reverse(m_digits.begin(), m_digits.end());
}
#pragma endregion
#pragma region String
// ============================================================================
String::String(World& w, const std::string& str) :
    CellI(w),
    m_value(str)
{
}

bool String::has(CellI& key)
{
    calculateCharacters();

    if (&key == &w.id.__type__ || &key == &w.id.value) {
        return true;
    }
    if (&key == &w.id.first && m_charactersList->has(key)) {
        return true;
    }
    if (&key == &w.id.last && m_charactersList->has(key)) {
        return true;
    }

    return false;
}

void String::set(CellI& key, CellI& value)
{
    throw "Changing a hybrid string cell is not possible!";
}

void String::erase(CellI& key)
{
    throw "Changing a hybrid string cell is not possible!";
}

void String::operator()()
{
    // Do nothing, this is a data cell
}

CellI& String::operator[](CellI& key)
{
    calculateCharacters();

    if (&key == &w.id.__type__) {
        return w.std.String;
    }
    if (&key == &w.id.first) {
        return m_charactersList->get(key);
    }
    if (&key == &w.id.last) {
        return m_charactersList->get(key);
    }
    if (&key == &w.id.size) {
        return m_charactersList->get(key);
    }
    if (&key == &w.id.value) {
        return *m_charactersList;
    }

    throw "No such key!";
}

const std::string& String::value() const
{
    return m_value;
}

void String::calculateCharacters()
{
    if (!m_characters.empty()) {
        return;
    }
    utf8::iterator<const char*> valueIt(m_value.data(), m_value.data(), m_value.data() + m_value.size());
    utf8::iterator<const char*> valueEndIt(m_value.data() + m_value.size(), m_value.data(), m_value.data() + m_value.size());

    for (auto& it = valueIt; it != valueEndIt; ++valueIt) {
        char32_t unicodeValue = *it;
        CellI& character      = w.pools.chars.get(unicodeValue);
        CellI& test = character.__type__();
        m_characters.push_back(&w.pools.chars.get(unicodeValue));
    }

    m_charactersList.reset(new List(w, m_characters, label()));
}
#pragma endregion
namespace hybrid {

// ============================================================================
ActivationPointer::ActivationPointer(World& w) :
    CellI(w), m_time(w)
{
}

bool ActivationPointer::has(CellI& key)
{
    if (&key == &w.id.__type__) {
        return true;
    }
    if (&key == &w.id.cell || &key == &w.id.previous) {
        return true;
    }

    return false;
}

void ActivationPointer::set(CellI& key, CellI& value)
{
    throw "Changing the activation pointer cell is not possible!";
}

void ActivationPointer::erase(CellI& key)
{
    throw "Changing the activation pointer cell is not possible!";
}

void ActivationPointer::operator()()
{
    // Do nothing
}

CellI& ActivationPointer::operator[](CellI& key)
{
    if (&key == &w.id.__type__) {
        return w.std.Cell; // TODO
    }
    if (&key == &w.id.cell) {
        return *m_currentCell;
    }
    if (&key == &w.id.previous) {
        return *m_previousCell;
    }

    throw "No such key!";
}

} // namespace hybrid

#if 0
void forEach(CellI& list, std::function<void(CellI& value, int i, bool& stop)> visitFn)
{
    class ListIterator
    {
    public:
        ListIterator(CellI& list) :
            w(list.w), m_list(list), m_nodePtr(nullptr)
        {
        }

        bool isContainerEmpty() {
            return m_list.missing(w.id.first);
        }

        void goToFirstNode()
        {
            m_nodePtr = &m_list[w.id.first];
        }

        CellI& getCurrentNodeValue()
        {
            return (*m_nodePtr)[w.id.value];
        }

        bool hasNextNode()
        {
            return (*m_nodePtr).has(w.id.next);
        }

        void goToNextNode()
        {
            m_nodePtr = &(*m_nodePtr)[w.id.next];
        }

    private:
        World& w;
        CellI& m_list;
        CellI* m_nodePtr;
    };

    ListIterator iterator(list);

    if (iterator.isContainerEmpty()) {
        return;
    } else {
        iterator.goToFirstNode();
    }

    int i = 0;
    do {
        bool stop = false;
        visitFn(iterator.getCurrentNodeValue(), i++, stop);
        if (stop) {
            return;
        }
        if (iterator.hasNextNode()) {
            iterator.goToNextNode();
        } else {
            return;
        }
    } while (true);
}
#endif

} // namespace cells
} // namespace infocell
