#include "Cells.h"
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

int Object::s_indent = 0;
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

void Object::printIndent()
{
    static const int s_tabSize = 2;
    for (int i = 0; i < s_indent; ++i) {
        for (int j = 0; j < s_tabSize; ++j) {
            std::cout << " ";
        }
    }
}

void Object::resetIndent()
{
    s_indent = 0;
}

void Object::createSelfStack()
{
    createStack(*this);
    initLocalVars(*this);
}

void Object::runAsCall()
{
    Object& method = static_cast<Object&>(get(w.id.method)[w.id.value]);
    method.createSelfStack();
    set(w.id.stack, method);

    (*this)();
}

// core data handling
static void evalOpGet(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.id.state)) {
        self.set(w.id.state, w.id.stateParamInit);
    }
    CellI& state = self[w.id.state];
    if (&state == &w.id.stateParamInit) {
        self.set(w.id.previous, *previousCell);
        CellI& inputCell = self[w.id.cell];
        previousCell     = currentCell;
        currentCell      = &inputCell;
        self.set(w.id.state, w.id.stateParam1);
    } else if (&state == &w.id.stateParam1) {
        CellI& inputRole = self[w.id.key];
        previousCell     = currentCell;
        currentCell      = &inputRole;
        self.set(w.id.state, w.id.stateParam2);
    } else if (&state == &w.id.stateParam2) {
        CellI& cell = self[w.id.cell][w.id.value];
        CellI& key = self[w.id.key][w.id.value];

        self.set(w.id.value, cell[key]);
        self.set(w.id.state, w.id.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self[w.id.previous];
    }
}

static void evalOpSet(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.id.state)) {
        self.set(w.id.state, w.id.stateParamInit);
    }
    CellI& state = self[w.id.state];
    if (&state == &w.id.stateParamInit) {
        self.set(w.id.previous, *previousCell);
        CellI& inputCell = self[w.id.cell];
        previousCell     = currentCell;
        currentCell      = &inputCell;
        self.set(w.id.state, w.id.stateParam1);
    } else if (&state == &w.id.stateParam1) {
        CellI& inputRole = self[w.id.key];
        previousCell     = currentCell;
        currentCell      = &inputRole;
        self.set(w.id.state, w.id.stateParam2);
    } else if (&state == &w.id.stateParam2) {
        CellI& inputValue = self[w.id.value];
        previousCell      = currentCell;
        currentCell       = &inputValue;
        self.set(w.id.state, w.id.stateParam3);
    } else if (&state == &w.id.stateParam3) {
        CellI& cell  = self[w.id.cell][w.id.value];
        CellI& key  = self[w.id.key][w.id.value];
        CellI& value = self[w.id.value][w.id.value];

        cell.set(key, value);
        self.set(w.id.state, w.id.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self[w.id.previous];
    }
}

static void evalOpHas(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.id.state)) {
        self.set(w.id.state, w.id.stateParamInit);
    }
    CellI& state = self[w.id.state];
    if (&state == &w.id.stateParamInit) {
        self.set(w.id.previous, *previousCell);
        CellI& inputCell = self[w.id.cell];
        previousCell     = currentCell;
        currentCell      = &inputCell;
        self.set(w.id.state, w.id.stateParam1);
    } else if (&state == &w.id.stateParam1) {
        CellI& inputRole = self[w.id.key];
        previousCell     = currentCell;
        currentCell      = &inputRole;
        self.set(w.id.state, w.id.stateParam2);
    } else if (&state == &w.id.stateParam2) {
        CellI& cell = self[w.id.cell][w.id.value];
        CellI& key = self[w.id.key][w.id.value];

        self.set(w.id.value, w.toCellBool(cell.has(key)));
        self.set(w.id.state, w.id.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self[w.id.previous];
    }
}

static void evalOpMissing(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.id.state)) {
        self.set(w.id.state, w.id.stateParamInit);
    }
    CellI& state = self[w.id.state];
    if (&state == &w.id.stateParamInit) {
        self.set(w.id.previous, *previousCell);
        CellI& inputCell = self[w.id.cell];
        previousCell     = currentCell;
        currentCell      = &inputCell;
        self.set(w.id.state, w.id.stateParam1);
    } else if (&state == &w.id.stateParam1) {
        CellI& inputRole = self[w.id.key];
        previousCell     = currentCell;
        currentCell      = &inputRole;
        self.set(w.id.state, w.id.stateParam2);
    } else if (&state == &w.id.stateParam2) {
        CellI& cell = self[w.id.cell][w.id.value];
        CellI& key = self[w.id.key][w.id.value];

        self.set(w.id.value, w.toCellBool(cell.missing(key)));
        self.set(w.id.state, w.id.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self[w.id.previous];
    }
}

static void evalOpErase(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.id.state)) {
        self.set(w.id.state, w.id.stateParamInit);
    }
    CellI& state = self[w.id.state];
    if (&state == &w.id.stateParamInit) {
        self.set(w.id.previous, *previousCell);
        CellI& inputCell = self[w.id.cell];
        previousCell     = currentCell;
        currentCell      = &inputCell;
        self.set(w.id.state, w.id.stateParam1);
    } else if (&state == &w.id.stateParam1) {
        CellI& inputRole = self[w.id.key];
        previousCell     = currentCell;
        currentCell      = &inputRole;
        self.set(w.id.state, w.id.stateParam2);
    } else if (&state == &w.id.stateParam2) {
        CellI& cell = self[w.id.cell][w.id.value];
        CellI& key = self[w.id.key][w.id.value];

        cell.erase(key);
        self.set(w.id.state, w.id.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self[w.id.previous];
    }
}

static void evalOpNew(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.id.state)) {
        self.set(w.id.state, w.id.stateParamInit);
    }
    CellI& state = self[w.id.state];
    if (&state == &w.id.stateParamInit) {
        self.set(w.id.previous, *previousCell);
        CellI& inputObjectType = self[w.id.objectType];
        previousCell           = currentCell;
        currentCell            = &inputObjectType;
        self.set(w.id.state, w.id.stateParam1);
    } else if (&state == &w.id.stateParam1) {
        CellI& objectType = self[w.id.objectType][w.id.value];

        if (&objectType == &w.std.Number) {
            self.set(w.id.value, *new Number(w));
        } else if (&objectType == &w.std.String) {
            self.set(w.id.value, *new String(w));
        } else {
            self.set(w.id.value, *new Object(w, objectType));
        }
        self.set(w.id.state, w.id.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self[w.id.previous];
    }
}

static void evalOpDelete(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.id.state)) {
        self.set(w.id.state, w.id.stateParamInit);
    }
    CellI& state = self[w.id.state];
    if (&state == &w.id.stateParamInit) {
        self.set(w.id.previous, *previousCell);
        CellI& input = self[w.id.input];
        previousCell = currentCell;
        currentCell  = &input;
        self.set(w.id.state, w.id.stateParam1);
    } else if (&state == &w.id.stateParam1) {
        CellI& input = self[w.id.input];
        CellI* cell  = &input[w.id.value];

        delete cell;
        input.erase(w.id.value);
        self.set(w.id.state, w.id.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self[w.id.previous];
    }
}

// code running
static void evalOpActivate(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.id.state)) {
        self.set(w.id.state, w.id.stateParamInit);
    }
    CellI& state = self[w.id.state];
    if (&state == &w.id.stateParamInit) {
        self.set(w.id.previous, *previousCell);
        CellI& inputCell = self[w.id.cell];

        previousCell = currentCell;
        currentCell  = &inputCell;
        self.set(w.id.state, w.id.stateParam1);
    } else if (&state == &w.id.stateParam1) {
        previousCell     = currentCell;
        CellI& inputCell = self[w.id.cell];
        CellI* status    = &w.id.process;

        if (self.has(w.id.parent)) {
            CellI& parent = self[w.id.parent];
            if (&inputCell.__type__() == &w.std.op.Return || (inputCell.has(w.id.status) && (&inputCell[w.id.status] == &w.id.return_))) {
                parent.set(w.id.status, w.id.return_);
                status = &w.id.return_;
            } else if (&parent[w.id.status] == &w.id.continue_ || &parent[w.id.status] == &w.id.break_) {
                status = &parent[w.id.status];
            } else if (self.has(w.id.status)) {
                if (&self[w.id.status] == &w.id.return_ || &self[w.id.status] == &w.id.continue_ || &self[w.id.status] == &w.id.break_) {
                    parent.set(w.id.status, self[w.id.status]);
                    status = &self[w.id.status];
                }
            }
        }
        if (status == &w.id.process && self.has(w.id.next)) {
            CellI& nextCell = self[w.id.next];
            currentCell     = &nextCell;
        } else {
            if (self.has(w.id.parent)) {
                currentCell = &self[w.id.parent];
            } else {
                currentCell = &self[w.id.previous];
            }
        }
        self.set(w.id.state, w.id.stateParamInit);
    }
}

static void saveOpState(List& opStates, CellI& op)
{
    World& w = op.w;

    if (&op == &w.id.emptyObject) {
        return;
    }
    CellI& type      = op.__type__();
    Object& opState  = *new Object(w, w.std.OpState);

    opState.set(w.id.op, op);
    opState.set(w.id.state, w.id.state);
    opState.set(w.id.value, op[w.id.state]);
    opStates.add(opState);
    if (op.has(w.id.status)) {
        Object& opState = *new Object(w, w.std.OpState);
        opState.set(w.id.op, op);
        opState.set(w.id.state, w.id.status);
        opState.set(w.id.value, op[w.id.status]);
        opStates.add(opState);
    }
    if (op.has(w.id.previous)) {
        Object& opState = *new Object(w, w.std.OpState);
        opState.set(w.id.op, op);
        opState.set(w.id.state, w.id.previous);
        opState.set(w.id.value, op[w.id.previous]);
        opStates.add(opState);
    }
    if (&type == &w.std.op.Call) {
        if (op[w.id.self].has(w.id.value)) {
            Object& opState = *new Object(w, w.std.OpState);
            opState.set(w.id.op, op);
            opState.set(w.id.state, w.id.self);
            opState.set(w.id.value, op[w.id.self][w.id.value]);
            opStates.add(opState);
        }
        if (op[w.id.method].has(w.id.value)) {
            Object& opState = *new Object(w, w.std.OpState);
            opState.set(w.id.op, op);
            opState.set(w.id.state, w.id.method);
            opState.set(w.id.value, op[w.id.method][w.id.value]);
            opStates.add(opState);
        }
#if 0
        if (op[w.id.stack].has(w.id.value)) {
            Object& opState = *new Object(w, w.std.OpState);
            opState.set(w.id.op, op);
            opState.set(w.id.state, w.id.stack);
            opState.set(w.id.value, op[w.id.stack][w.id.value]);
            opStates.add(opState);
            std::cout << "SAVE " << op[w.id.stack][w.id.value].label() << std::endl;
        }
#endif
    }
    if (&type == &w.std.op.Set || &type == &w.std.op.Get) {
        if (op[w.id.cell].has(w.id.value)) {
            Object& opState = *new Object(w, w.std.OpState);
            opState.set(w.id.op, op);
            opState.set(w.id.state, w.id.cell);
            opState.set(w.id.value, op[w.id.cell][w.id.value]);
            opStates.add(opState);
        }
        if (op[w.id.key].has(w.id.value)) {
            Object& opState = *new Object(w, w.std.OpState);
            opState.set(w.id.op, op);
            opState.set(w.id.state, w.id.key);
            opState.set(w.id.value, op[w.id.key][w.id.value]);
            opStates.add(opState);
        }
    }

    if (&type == &w.std.op.Same ||
        &type == &w.std.op.NotSame ||
        &type == &w.std.op.Equal ||
        &type == &w.std.op.NotEqual ||
        &type == &w.std.op.LessThan ||
        &type == &w.std.op.LessThanOrEqual ||
        &type == &w.std.op.GreaterThan ||
        &type == &w.std.op.GreaterThanOrEqual ||
        &type == &w.std.op.And ||
        &type == &w.std.op.Or ||
        &type == &w.std.op.Add ||
        &type == &w.std.op.Subtract ||
        &type == &w.std.op.Multiply ||
        &type == &w.std.op.Divide) {
        Object& opState = *new Object(w, w.std.OpState);
        opState.set(w.id.op, op);
        opState.set(w.id.state, w.id.lhs);
        opState.set(w.id.value, op[w.id.lhs][w.id.value]);
        opStates.add(opState);
    }
    op.set(w.id.state, w.id.stateParamInit);
}

static void loadOpState(CellI& opState)
{
    World& w = opState.w;
    CellI& op        = opState[w.id.op];
    CellI& type      = op.__type__();
    CellI& state     = opState[w.id.state];
    CellI& value     = opState[w.id.value];

    if ((&type == &w.std.op.Set || &type == &w.std.op.Get) && ((&state == &w.id.cell) || (&state == &w.id.key))) {
        op[state].set(w.id.value, value);
    } else if ((&type == &w.std.op.Call) && ((&state == &w.id.self) || (&state == &w.id.method) || (&state == &w.id.stack))) {
        op[state].set(w.id.value, value);
    } else if ((&type == &w.std.op.Same ||
        &type == &w.std.op.NotSame ||
        &type == &w.std.op.Equal ||
        &type == &w.std.op.NotEqual ||
        &type == &w.std.op.LessThan ||
        &type == &w.std.op.LessThanOrEqual ||
        &type == &w.std.op.GreaterThan ||
        &type == &w.std.op.GreaterThanOrEqual ||
        &type == &w.std.op.And ||
        &type == &w.std.op.Or ||
        &type == &w.std.op.Add ||
        &type == &w.std.op.Subtract ||
        &type == &w.std.op.Multiply ||
        &type == &w.std.op.Divide) && (&state == &w.id.lhs)) {
        op[state].set(w.id.value, value);
    } else {
        op.set(state, value);
    }
//    std::cout << "         set (" << &op << ")" << op.__type__().label() << "[" << state.label() << ":" << value.label() << "]" << std::endl;
}

static void evalOpCall(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.id.state)) {
        self.set(w.id.state, w.id.stateParamInit);
    }
//    std::cout << "evalOpCall self: " << &self << ", state: " << self[w.id.state].label() << std::endl;
    CellI& state = self[w.id.state];
    if (&state == &w.id.stateParamInit) {
        self.erase(w.id.value);
        self.set(w.id.previous, *previousCell);
        CellI& inputSelf = self[w.id.self];
        previousCell     = currentCell;
        currentCell      = &inputSelf;
        self.set(w.id.state, w.id.stateParam1);
    } else if (&state == &w.id.stateParam1) {
        CellI& inputMethod = self[w.id.method];
        previousCell       = currentCell;
        currentCell        = &inputMethod;
        if (self.has(w.id.parameters)) {
            self.set(w.id.state, w.id.stateParamEval);
        } else {
            self.set(w.id.state, w.id.stateParam2);
        }
    } else if (&state == &w.id.stateParamEval) {
        CellI* paramNodePtr = nullptr;
        if (self.missing(w.id.currentParam)) {
            CellI& paramList = self[w.id.parameters][w.id.list];
            paramNodePtr     = &paramList[w.id.first];
        } else {
            CellI& paramNode = self[w.id.currentParam];
            if (paramNode.has(w.id.next)) {
                paramNodePtr = &paramNode[w.id.next];
            } else {
                self.erase(w.id.currentParam);
            }
        }
        if (paramNodePtr) {
            CellI& param = (*paramNodePtr)[w.id.value][w.id.value];
            self.set(w.id.currentParam, *paramNodePtr);
            previousCell = currentCell;
            currentCell  = &param;
        } else {
            self.set(w.id.state, w.id.stateParam2);
        }
    } else if (&state == &w.id.stateParam2) {
        CellI& inputSelf  = self[w.id.self][w.id.value];
        CellI& methodName = self[w.id.method][w.id.value];
        CellI& stack      = self[w.id.stack][w.id.stack];

        CellI* methodPtr = nullptr;
        if (&methodName.__type__() == &w.std.String) {
            if (&self[w.id.ast].__type__() == &w.std.ast.Call) {
                methodPtr = &inputSelf[w.id.__type__][w.id.methods];
            } else {
                methodPtr = &inputSelf[w.id.methods];
            }
            methodPtr = &(*methodPtr)[w.id.index][methodName][w.id.value];
        } else {
            methodPtr = &self[w.id.method][w.id.value];
        }
        CellI& method = (*methodPtr);

        CellI& stackFrame = *new Object(w, w.std.StackFrame);
        stackFrame.set(w.id.method, method);

        CellI& inputIndex = *new Object(w, w.std.Index);
        inputIndex.set(w.id.self, inputSelf);
        if (self.has(w.id.parameters)) {
            for (CellI& parameter : self[w.id.parameters][w.id.list]) {
                inputIndex.set(parameter[w.id.key], parameter[w.id.value][w.id.value]);
                // static_cast<Object&>(self).printIndent();
                // std::cout << parameter[w.id.key].label() << ":" << parameter[w.id.value][w.id.value].label() << std::endl;
            }
        }
        stackFrame.set(w.id.input, inputIndex);

        if (method.has(w.id.localVars)) {
            CellI& localVarsList  = method[w.id.localVars].slotList();
            auto& localVarsIndex = *new Object(w, method[w.id.localVars].__type__(), "StackLocalVarsIndex");
            for (CellI& slot : localVarsList) {
                localVarsIndex.set(slot[w.id.key], *new Object(w, w.std.op.Var));
            }
            stackFrame.set(w.id.localVars, localVarsIndex);
        }

        CellI& newStackListNode = *new Object(w, w.std.ListNode);
        newStackListNode.set(w.id.value, stackFrame);
        newStackListNode.set(w.id.previous, stack);
        stack.set(w.id.next, newStackListNode);

        CellI& previousMethod = stack[w.id.value][w.id.method];
//        std::cout << "previous method " << previousMethod.label() << std::endl;
//        std::cout << "next     method " << method.label() << std::endl;

//        std::cout << "erase" << std::endl;
        method.erase(w.id.value);
        method.set(w.id.stack, newStackListNode);
        self.set(w.id.state, w.id.stateStackCall);
        previousMethod.set(w.id.lastOp, self);

        if (method.has(w.id.state) && (&method[w.id.state] != &w.id.stateParamInit)) {
//            std::cout << "recursive call for " << method.__type__().label() << std::endl;
            List& cellPath = *new List(w, w.std.op.Base);
            CellI& lastOp  = method[w.id.lastOp];
            for (CellI* currentOp = &lastOp; currentOp != &method; currentOp = (*currentOp).has(w.id.parent) ? &(*currentOp)[w.id.parent] : &(*currentOp)[w.id.previous]) {
                CellI& op = *currentOp;
//                std::cout << "         [" << op.__type__().label() << ":" << op[w.id.state].label() << "]" << std::endl;
                saveOpState(cellPath, op);
            }
            saveOpState(cellPath, method);
            method.set(w.id.state, w.id.stateParamInit);
            stackFrame.set(w.id.ops, cellPath);
//            std::cout << std::endl;
        }
        previousCell = currentCell;
        currentCell  = &method;
    } else if (&state == &w.id.stateStackCall) {
        CellI& inputSelf  = self[w.id.self][w.id.value];
        CellI& methodName = self[w.id.method][w.id.value];

        CellI* methodPtr = nullptr;
        if (&methodName.__type__() == &w.std.String) {
            if (&self[w.id.ast].__type__() == &w.std.ast.Call) {
                methodPtr = &inputSelf[w.id.__type__][w.id.methods];
            } else {
                methodPtr = &inputSelf[w.id.methods];
            }
            // TODO: cache the method obj
            methodPtr = &(*methodPtr)[w.id.index][methodName][w.id.value];
        } else {
            methodPtr = &self[w.id.method][w.id.value];
        }
        CellI& method = (*methodPtr);
        if (method.has(w.id.value)) {
            self.set(w.id.value, method[w.id.value]);
//            static_cast<Object&>(self).printIndent();
//            std::cout << "return " << method[w.id.value].label() << std::endl;
        }

        previousCell = currentCell;
        currentCell  = &self[w.id.previous];
        self.set(w.id.state, w.id.stateParamInit);
    }
}

static void evalOpFunction(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.id.state)) {
        self.set(w.id.state, w.id.stateParamInit);
    }
    CellI& state = self[w.id.state];
    if (&state == &w.id.stateParamInit) {
        self.set(w.id.previous, *previousCell);
        CellI& op = self[w.id.op];
        if (op.has(w.id.state) && (&op[w.id.state] != &w.id.stateParamInit)) {
            throw "Error: function contains non-clean op.";
        }
        previousCell = currentCell;
        currentCell  = &op;
        self.set(w.id.state, w.id.stateParam1);
    } else if (&state == &w.id.stateParam1) {
        previousCell = currentCell;
        currentCell  = &self[w.id.previous];

        CellI& stackNode = self[w.id.stack];
        CellI& stackFrame = stackNode[w.id.value];
        static_cast<Object&>(self).printIndent();
//        std::cout << "return " << std::endl;
//        std::cout << "return " << self.label() << std::endl;
        if (stackFrame.has(w.id.ops)) {
            for (CellI& opState : stackFrame[w.id.ops]) {
                loadOpState(opState);
                delete &opState;
            }
            static_cast<List&>(stackFrame[w.id.ops]).clear();
            delete &stackFrame[w.id.ops];
            stackFrame.erase(w.id.ops);
        } else {
            self.set(w.id.state, w.id.stateParamInit);
        }
        //
        if (self[w.id.stack][w.id.previous].has(w.id.value)) {
            CellI& inputIndex         = stackFrame[w.id.input];
            CellI& previousStackNode  = self[w.id.stack][w.id.previous];
            CellI& previousStackFrame = previousStackNode[w.id.value];
            CellI& previousMethod     = previousStackFrame[w.id.method];
            previousMethod.set(w.id.stack, previousStackNode);
            previousStackNode.erase(w.id.next);
            delete &inputIndex;
            if (stackFrame.has(w.id.localVars)) {
                CellI& localVarsList  = self[w.id.localVars].slotList();
                CellI& localVarsIndex = stackFrame[w.id.localVars];
                for (CellI& slot : localVarsList) {
                    delete &localVarsIndex[slot[w.id.key]];
                }
                delete &localVarsIndex;
            }
            delete &stackFrame;
            delete &stackNode;
            if (currentCell == &w.id.emptyObject) {
                std::cout << "";
            }
        } else {
            self.set(w.id.state, w.id.stateParamInit);
            currentCell = &w.id.emptyObject;
        }
    }
}

static void evalOpIf(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.id.state)) {
        self.set(w.id.state, w.id.stateParamInit);
    }
    CellI& state = self[w.id.state];
    if (&state == &w.id.stateParamInit) {
        self.set(w.id.previous, *previousCell);
        CellI& inputCondition = self[w.id.condition];

        previousCell = currentCell;
        currentCell  = &inputCondition;
        self.set(w.id.state, w.id.stateParam1);
    } else if (&state == &w.id.stateParam1) {
        self.set(w.id.status, w.id.process);
        CellI* branchPtr = nullptr;
        bool condition   = &self[w.id.condition][w.id.value] == &w.true_;
        if (condition) {
            branchPtr = &self[w.id.then];
            self.set(w.id.state, w.id.stateThen);
        } else if (self.has(w.id.else_)) {
            branchPtr = &self[w.id.else_];
            self.set(w.id.state, w.id.stateElse);
        }
        previousCell = currentCell;
        if (branchPtr) {
            currentCell = branchPtr;
        } else {
            currentCell = &self[w.id.previous];
            self.set(w.id.state, w.id.stateParamInit);
        }
    }
 else if (&state == &w.id.stateThen || &state == &w.id.stateElse) {
     CellI& branch = &state == &w.id.stateThen ? self[w.id.then] : self[w.id.else_];
     if (&branch.__type__() == &w.std.op.Return) {
         self.set(w.id.status, w.id.return_);
     }
     else if (branch.has(w.id.status)) {
         self.set(w.id.status, branch[w.id.status]);
     }

     previousCell = currentCell;
     currentCell = &self[w.id.previous];
     self.set(w.id.state, w.id.stateParamInit);
    }
}

static void evalOpDo(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.id.state)) {
        self.set(w.id.state, w.id.stateParamInit);
    }
    CellI& state = self[w.id.state];
    if (&state == &w.id.stateParamInit) {
        self.set(w.id.previous, *previousCell);
        self.set(w.id.status, w.id.process);
        CellI& statement = self[w.id.statement];
        previousCell = currentCell;
        currentCell = &statement;
        self.set(w.id.state, w.id.stateStatement);
    } else if (&state == &w.id.stateStatement) {
        CellI& statement = self[w.id.statement];
        if (&statement.__type__() == &w.std.op.Return) {
            self.set(w.id.status, w.id.return_);
        }
        else if (statement.has(w.id.status)) {
            self.set(w.id.status, statement[w.id.status]);
        }
        CellI& inputCondition = self[w.id.condition];
        previousCell = currentCell;
        currentCell = &inputCondition;
        self.set(w.id.state, w.id.stateCondition);
    } else if (&state == &w.id.stateCondition) {
        previousCell = currentCell;
        if (self.has(w.id.status) && ((&self[w.id.status] == &w.id.return_) || (&self[w.id.status] == &w.id.break_))) {
            currentCell = &self[w.id.previous];
            self.set(w.id.state, w.id.stateParamInit);
        } else {
            self.set(w.id.status, w.id.process);
            bool condition = &self[w.id.condition][w.id.value] == &w.true_;
            if (condition) {
                currentCell = &self[w.id.statement];
                self.set(w.id.state, w.id.stateStatement);
            } else {
                currentCell = &self[w.id.previous];
                self.set(w.id.state, w.id.stateParamInit);
            }
        }
    }
}

static void evalOpWhile(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.id.state)) {
        self.set(w.id.state, w.id.stateParamInit);
    }
    CellI& state = self[w.id.state];
    if (&state == &w.id.stateParamInit) {
        self.set(w.id.previous, *previousCell);
        self.set(w.id.status, w.id.process);
        CellI& inputCondition = self[w.id.condition];
        previousCell = currentCell;
        currentCell = &inputCondition;
        self.set(w.id.state, w.id.stateCondition);
    } else if (&state == &w.id.stateStatement) {
        CellI& statement = self[w.id.statement];
        if (&statement.__type__() == &w.std.op.Return) {
            self.set(w.id.status, w.id.return_);
        }
        else if (statement.has(w.id.status)) {
            self.set(w.id.status, statement[w.id.status]);
        }
        CellI& inputCondition = self[w.id.condition];
        previousCell = currentCell;
        currentCell = &inputCondition;
        self.set(w.id.state, w.id.stateCondition);
    } else if (&state == &w.id.stateCondition) {
        previousCell = currentCell;
        if (self.has(w.id.status) && ((&self[w.id.status] == &w.id.return_) || (&self[w.id.status] == &w.id.break_))) {
            currentCell = &self[w.id.previous];
            self.set(w.id.state, w.id.stateParamInit);
        } else {
            self.set(w.id.status, w.id.process);
            bool condition = &self[w.id.condition][w.id.value] == &w.true_;
            if (condition) {
                currentCell = &self[w.id.statement];
                self.set(w.id.state, w.id.stateStatement);
            } else {
                currentCell = &self[w.id.previous];
                self.set(w.id.state, w.id.stateParamInit);
            }
        }
    }
}

static void evalOpBlock(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.id.state)) {
        self.set(w.id.state, w.id.stateParamInit);
    }
    CellI& state = self[w.id.state];
    if (&state == &w.id.stateParamInit) {
        self.set(w.id.previous, *previousCell);
        CellI& ops   = self[w.id.ops];
        previousCell = currentCell;
        currentCell  = &ops;
        self.set(w.id.status, w.id.process);
        self.set(w.id.state, w.id.stateParam1);
    } else if (&state == &w.id.stateParam1) {
        previousCell = currentCell;
        currentCell  = &self[w.id.previous];
        self.set(w.id.state, w.id.stateParamInit);
    }
}

static void evalOpReturn(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.id.state)) {
        self.set(w.id.state, w.id.stateParamInit);
    }
    CellI& state = self[w.id.state];
    if (self.has(w.id.result)) {
        if (&state == &w.id.stateParamInit) {
            self.set(w.id.previous, *previousCell);
            CellI& result = self[w.id.result];
            previousCell  = currentCell;
            currentCell   = &result;
            self.set(w.id.state, w.id.stateParam1);
        } else if (&state == &w.id.stateParam1) {
            previousCell = currentCell;
            currentCell  = &self[w.id.previous];
            self.set(w.id.state, w.id.stateParamInit);
        }
    } else {
        std::swap(currentCell, previousCell);
    }
}

// compare
static void evalOpSame(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.id.state)) {
        self.set(w.id.state, w.id.stateParamInit);
    }
    CellI& state = self[w.id.state];
    if (&state == &w.id.stateParamInit) {
        self.set(w.id.previous, *previousCell);
        CellI& inputLhs  = self[w.id.lhs];
        previousCell     = currentCell;
        currentCell      = &inputLhs;
        self.set(w.id.state, w.id.stateLhs);
    } else if (&state == &w.id.stateLhs) {
        CellI& inputRhs  = self[w.id.rhs];
        previousCell     = currentCell;
        currentCell      = &inputRhs;
        self.set(w.id.state, w.id.stateRhs);
    } else if (&state == &w.id.stateRhs) {
        CellI* lhs = &self[w.id.lhs][w.id.value];
        CellI* rhs = &self[w.id.rhs][w.id.value];

        self.set(w.id.value, w.toCellBool(lhs == rhs));
        self.set(w.id.state, w.id.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self[w.id.previous];
    }
}

static void evalOpNotSame(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.id.state)) {
        self.set(w.id.state, w.id.stateParamInit);
    }
    CellI& state = self[w.id.state];
    if (&state == &w.id.stateParamInit) {
        self.set(w.id.previous, *previousCell);
        CellI& inputLhs = self[w.id.lhs];
        previousCell    = currentCell;
        currentCell     = &inputLhs;
        self.set(w.id.state, w.id.stateLhs);
    } else if (&state == &w.id.stateLhs) {
        CellI& inputRhs = self[w.id.rhs];
        previousCell    = currentCell;
        currentCell     = &inputRhs;
        self.set(w.id.state, w.id.stateRhs);
    } else if (&state == &w.id.stateRhs) {
        CellI* lhs = &self[w.id.lhs][w.id.value];
        CellI* rhs = &self[w.id.rhs][w.id.value];

        self.set(w.id.value, w.toCellBool(lhs != rhs));
        self.set(w.id.state, w.id.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self[w.id.previous];
    }
}

static void evalOpEqual(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.id.state)) {
        self.set(w.id.state, w.id.stateParamInit);
    }
    CellI& state = self.get(w.id.state);
    if (&state == &w.id.stateParamInit) {
        self.set(w.id.previous, *previousCell);
        CellI& inputLhs = self.get(w.id.lhs);
        previousCell    = currentCell;
        currentCell     = &inputLhs;
        self.set(w.id.state, w.id.stateLhs);
    } else if (&state == &w.id.stateLhs) {
        CellI& inputRhs = self.get(w.id.rhs);
        previousCell    = currentCell;
        currentCell     = &inputRhs;
        self.set(w.id.state, w.id.stateRhs);
    } else if (&state == &w.id.stateRhs) {
        CellI& lhs = self[w.id.lhs][w.id.value];
        CellI& rhs = self[w.id.rhs][w.id.value];

        self.set(w.id.value, w.toCellBool(lhs == rhs));
        self.set(w.id.state, w.id.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self.get(w.id.previous);
    }
}

static void evalOpNotEqual(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.id.state)) {
        self.set(w.id.state, w.id.stateParamInit);
    }
    CellI& state = self.get(w.id.state);
    if (&state == &w.id.stateParamInit) {
        self.set(w.id.previous, *previousCell);
        CellI& inputLhs = self.get(w.id.lhs);
        previousCell    = currentCell;
        currentCell     = &inputLhs;
        self.set(w.id.state, w.id.stateLhs);
    } else if (&state == &w.id.stateLhs) {
        CellI& inputRhs = self.get(w.id.rhs);
        previousCell    = currentCell;
        currentCell     = &inputRhs;
        self.set(w.id.state, w.id.stateRhs);
    } else if (&state == &w.id.stateRhs) {
        CellI& lhs = self[w.id.lhs][w.id.value];
        CellI& rhs = self[w.id.rhs][w.id.value];

        self.set(w.id.value, w.toCellBool(lhs != rhs));
        self.set(w.id.state, w.id.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self.get(w.id.previous);
    }
}

static void evalOpLessThan(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.id.state)) {
        self.set(w.id.state, w.id.stateParamInit);
    }
    CellI& state = self.get(w.id.state);
    if (&state == &w.id.stateParamInit) {
        self.set(w.id.previous, *previousCell);
        CellI& inputLhs = self.get(w.id.lhs);
        previousCell    = currentCell;
        currentCell     = &inputLhs;
        self.set(w.id.state, w.id.stateLhs);
    } else if (&state == &w.id.stateLhs) {
        CellI& inputRhs = self.get(w.id.rhs);
        previousCell    = currentCell;
        currentCell     = &inputRhs;
        self.set(w.id.state, w.id.stateRhs);
    } else if (&state == &w.id.stateRhs) {
        int lhs = static_cast<Number&>(self[w.id.lhs][w.id.value]).value();
        int rhs = static_cast<Number&>(self[w.id.rhs][w.id.value]).value();

        self.set(w.id.value, lhs < rhs ? w.true_ : w.false_);
        self.set(w.id.state, w.id.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self.get(w.id.previous);
    }
}

static void evalOpLessThanOrEqual(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.id.state)) {
        self.set(w.id.state, w.id.stateParamInit);
    }
    CellI& state = self.get(w.id.state);
    if (&state == &w.id.stateParamInit) {
        self.set(w.id.previous, *previousCell);
        CellI& inputLhs = self.get(w.id.lhs);
        previousCell    = currentCell;
        currentCell     = &inputLhs;
        self.set(w.id.state, w.id.stateLhs);
    } else if (&state == &w.id.stateLhs) {
        CellI& inputRhs = self.get(w.id.rhs);
        previousCell    = currentCell;
        currentCell     = &inputRhs;
        self.set(w.id.state, w.id.stateRhs);
    } else if (&state == &w.id.stateRhs) {
        int lhs = static_cast<Number&>(self[w.id.lhs][w.id.value]).value();
        int rhs = static_cast<Number&>(self[w.id.rhs][w.id.value]).value();

        self.set(w.id.value, lhs <= rhs ? w.true_ : w.false_);
        self.set(w.id.state, w.id.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self.get(w.id.previous);
    }
}

static void evalOpGreaterThan(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.id.state)) {
        self.set(w.id.state, w.id.stateParamInit);
    }
    CellI& state = self.get(w.id.state);
    if (&state == &w.id.stateParamInit) {
        self.set(w.id.previous, *previousCell);
        CellI& inputLhs = self.get(w.id.lhs);
        previousCell    = currentCell;
        currentCell     = &inputLhs;
        self.set(w.id.state, w.id.stateLhs);
    } else if (&state == &w.id.stateLhs) {
        CellI& inputRhs = self.get(w.id.rhs);
        previousCell    = currentCell;
        currentCell     = &inputRhs;
        self.set(w.id.state, w.id.stateRhs);
    } else if (&state == &w.id.stateRhs) {
        int lhs = static_cast<Number&>(self[w.id.lhs][w.id.value]).value();
        int rhs = static_cast<Number&>(self[w.id.rhs][w.id.value]).value();

        self.set(w.id.value, lhs > rhs ? w.true_ : w.false_);
        self.set(w.id.state, w.id.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self.get(w.id.previous);
    }
}

static void evalOpGreaterThanOrEqual(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.id.state)) {
        self.set(w.id.state, w.id.stateParamInit);
    }
    CellI& state = self.get(w.id.state);
    if (&state == &w.id.stateParamInit) {
        self.set(w.id.previous, *previousCell);
        CellI& inputLhs = self.get(w.id.lhs);
        previousCell    = currentCell;
        currentCell     = &inputLhs;
        self.set(w.id.state, w.id.stateLhs);
    } else if (&state == &w.id.stateLhs) {
        CellI& inputRhs = self.get(w.id.rhs);
        previousCell    = currentCell;
        currentCell     = &inputRhs;
        self.set(w.id.state, w.id.stateRhs);
    } else if (&state == &w.id.stateRhs) {
        int lhs = static_cast<Number&>(self[w.id.lhs][w.id.value]).value();
        int rhs = static_cast<Number&>(self[w.id.rhs][w.id.value]).value();

        self.set(w.id.value, lhs >= rhs ? w.true_ : w.false_);
//        std::cout << "      " << lhs << " >= " << rhs << std::endl;
        self.set(w.id.state, w.id.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self.get(w.id.previous);
    }
}
// logic
static void evalOpAnd(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.id.state)) {
        self.set(w.id.state, w.id.stateParamInit);
    }
    CellI& state = self.get(w.id.state);
    if (&state == &w.id.stateParamInit) {
        self.set(w.id.previous, *previousCell);
        CellI& inputLhs = self.get(w.id.lhs);
        previousCell    = currentCell;
        currentCell     = &inputLhs;
        self.set(w.id.state, w.id.stateLhs);
    } else if (&state == &w.id.stateLhs) {
        bool lhs = &self[w.id.lhs][w.id.value] == &w.true_;
        // shortcut, if the left hand side already false we don't evaluate the right hand side
        if (lhs == false) {
            self.set(w.id.value, w.false_);
            self.set(w.id.state, w.id.stateParamInit);
            previousCell = currentCell;
            currentCell  = &self.get(w.id.previous);
        } else {
            CellI& inputRhs = self.get(w.id.rhs);
            previousCell    = currentCell;
            currentCell     = &inputRhs;
            self.set(w.id.state, w.id.stateRhs);
        }
    } else if (&state == &w.id.stateRhs) {
        bool lhs = &self[w.id.lhs][w.id.value] == &w.true_;
        bool rhs = &self[w.id.rhs][w.id.value] == &w.true_;

        self.set(w.id.value, w.toCellBool(lhs && rhs));
        self.set(w.id.state, w.id.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self.get(w.id.previous);
    }
}

static void evalOpOr(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.id.state)) {
        self.set(w.id.state, w.id.stateParamInit);
    }
    CellI& state = self.get(w.id.state);
    if (&state == &w.id.stateParamInit) {
        self.set(w.id.previous, *previousCell);
        CellI& inputLhs = self.get(w.id.lhs);
        previousCell    = currentCell;
        currentCell     = &inputLhs;
        self.set(w.id.state, w.id.stateLhs);
    } else if (&state == &w.id.stateLhs) {
        CellI& inputRhs = self.get(w.id.rhs);
        previousCell    = currentCell;
        currentCell     = &inputRhs;
        self.set(w.id.state, w.id.stateRhs);
    } else if (&state == &w.id.stateRhs) {
        bool lhs = &self[w.id.lhs][w.id.value] == &w.true_;
        bool rhs = &self[w.id.rhs][w.id.value] == &w.true_;

        self.set(w.id.value, w.toCellBool(lhs || rhs));
        self.set(w.id.state, w.id.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self.get(w.id.previous);
    }
}

static void evalOpNot(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.id.state)) {
        self.set(w.id.state, w.id.stateParamInit);
    }
    CellI& state = self[w.id.state];
    if (&state == &w.id.stateParamInit) {
        self.set(w.id.previous, *previousCell);
        CellI& input = self[w.id.input];
        previousCell = currentCell;
        currentCell  = &input;
        self.set(w.id.state, w.id.stateParam1);
    } else if (&state == &w.id.stateParam1) {
        bool res = &self[w.id.input][w.id.value] == &w.true_;

        self.set(w.id.value, w.toCellBool(!res));
        self.set(w.id.state, w.id.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self.get(w.id.previous);
    }
}

// math
static void evalOpAdd(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.id.state)) {
        self.set(w.id.state, w.id.stateParamInit);
    }
    CellI& state = self.get(w.id.state);
    if (&state == &w.id.stateParamInit) {
        self.set(w.id.previous, *previousCell);
        CellI& inputLhs = self.get(w.id.lhs);
        previousCell    = currentCell;
        currentCell     = &inputLhs;
        self.set(w.id.state, w.id.stateLhs);
    } else if (&state == &w.id.stateLhs) {
        CellI& inputRhs = self.get(w.id.rhs);
        previousCell    = currentCell;
        currentCell     = &inputRhs;
        self.set(w.id.state, w.id.stateRhs);
    } else if (&state == &w.id.stateRhs) {
        int lhs = static_cast<Number&>(self[w.id.lhs][w.id.value]).value();
        int rhs = static_cast<Number&>(self[w.id.rhs][w.id.value]).value();

        self.set(w.id.value, w.pools.numbers.get(lhs + rhs));
        self.set(w.id.state, w.id.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self.get(w.id.previous);
    }
}

static void evalOpSubtract(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.id.state)) {
        self.set(w.id.state, w.id.stateParamInit);
    }
    CellI& state = self.get(w.id.state);
    if (&state == &w.id.stateParamInit) {
        self.set(w.id.previous, *previousCell);
        CellI& inputLhs = self.get(w.id.lhs);
        previousCell    = currentCell;
        currentCell     = &inputLhs;
        self.set(w.id.state, w.id.stateLhs);
    } else if (&state == &w.id.stateLhs) {
        CellI& inputRhs = self.get(w.id.rhs);
        previousCell    = currentCell;
        currentCell     = &inputRhs;
        self.set(w.id.state, w.id.stateRhs);
    } else if (&state == &w.id.stateRhs) {
        int lhs = static_cast<Number&>(self[w.id.lhs][w.id.value]).value();
        int rhs = static_cast<Number&>(self[w.id.rhs][w.id.value]).value();

        self.set(w.id.value, w.pools.numbers.get(lhs - rhs));
//        std::cout << "      " << lhs << " - " << rhs << std::endl;
        self.set(w.id.state, w.id.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self.get(w.id.previous);
    }
}

static void evalOpMultiply(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.id.state)) {
        self.set(w.id.state, w.id.stateParamInit);
    }
    CellI& state = self.get(w.id.state);
    if (&state == &w.id.stateParamInit) {
        self.set(w.id.previous, *previousCell);
        CellI& inputLhs = self.get(w.id.lhs);
        previousCell    = currentCell;
        currentCell     = &inputLhs;
        self.set(w.id.state, w.id.stateLhs);
    } else if (&state == &w.id.stateLhs) {
        CellI& inputRhs = self.get(w.id.rhs);
        previousCell    = currentCell;
        currentCell     = &inputRhs;
        self.set(w.id.state, w.id.stateRhs);
    } else if (&state == &w.id.stateRhs) {
        int lhs = static_cast<Number&>(self[w.id.lhs][w.id.value]).value();
        int rhs = static_cast<Number&>(self[w.id.rhs][w.id.value]).value();

        self.set(w.id.value, w.pools.numbers.get(lhs * rhs));
//        std::cout << "      " << lhs << " * " << rhs << std::endl;
        self.set(w.id.state, w.id.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self.get(w.id.previous);
    }
}

static void evalOpDivide(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.id.state)) {
        self.set(w.id.state, w.id.stateParamInit);
    }
    CellI& state = self.get(w.id.state);
    if (&state == &w.id.stateParamInit) {
        self.set(w.id.previous, *previousCell);
        CellI& inputLhs = self.get(w.id.lhs);
        previousCell    = currentCell;
        currentCell     = &inputLhs;
        self.set(w.id.state, w.id.stateLhs);
    } else if (&state == &w.id.stateLhs) {
        CellI& inputRhs = self.get(w.id.rhs);
        previousCell    = currentCell;
        currentCell     = &inputRhs;
        self.set(w.id.state, w.id.stateRhs);
    } else if (&state == &w.id.stateRhs) {
        int lhs = static_cast<Number&>(self[w.id.lhs][w.id.value]).value();
        int rhs = static_cast<Number&>(self[w.id.rhs][w.id.value]).value();

        self.set(w.id.value, w.pools.numbers.get(lhs / rhs));
        self.set(w.id.state, w.id.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self.get(w.id.previous);
    }
}

void Object::operator()()
{
    s_debugFunctionCalls = false; // Turn on / off debug here

    CellI* currentCell  = this;
    CellI* previousCell = &w.id.emptyObject;
    w.ap.m_currentCell  = currentCell;
    w.ap.m_previousCell = previousCell;
    int tick = 0;

    do {
        CellI& self = *currentCell;
        CellI& type = self.__type__();

        if (&type == &w.std.op.Get) {
            evalOpGet(self, currentCell, previousCell);
        } else if (&type == &w.std.op.Set) {
            evalOpSet(self, currentCell, previousCell);
        } else if (&type == &w.std.op.Has) {
            evalOpHas(self, currentCell, previousCell);
        } else if (&type == &w.std.op.Missing) {
            evalOpMissing(self, currentCell, previousCell);
        } else if (&type == &w.std.op.Erase) {
            evalOpErase(self, currentCell, previousCell);
        } else if (&type == &w.std.op.New) {
            evalOpNew(self, currentCell, previousCell);
        } else if (&type == &w.std.op.Delete) {
            evalOpDelete(self, currentCell, previousCell);
        } else if (&type == &w.std.op.Activate) {
            evalOpActivate(self, currentCell, previousCell);
        } else if (&type == &w.std.op.Call) {
            evalOpCall(self, currentCell, previousCell);
        } else if (&type == &w.std.op.Function) {
            if (s_debugFunctionCalls && (self.missing(w.id.state) || (&self[w.id.state] == &w.id.stateParamInit))) {
                printIndent();
                s_indent++;
                std::cout << self.label() << std::endl;
            }
            if (s_debugFunctionCalls && (self.has(w.id.state) && (&self[w.id.state] == &w.id.stateParam1))) {
                s_indent--;
            }
            evalOpFunction(self, currentCell, previousCell);
        } else if (&type == &w.std.op.Return) {
            evalOpReturn(self, currentCell, previousCell);
        } else if (&type == &w.std.op.Same) {
            evalOpSame(self, currentCell, previousCell);
        } else if (&type == &w.std.op.NotSame) {
            evalOpNotSame(self, currentCell, previousCell);
        } else if (&type == &w.std.op.Equal) {
            evalOpEqual(self, currentCell, previousCell);
        } else if (&type == &w.std.op.NotEqual) {
            evalOpNotEqual(self, currentCell, previousCell);
        } else if (&type == &w.std.op.LessThan) {
            evalOpLessThan(self, currentCell, previousCell);
        } else if (&type == &w.std.op.LessThanOrEqual) {
            evalOpLessThanOrEqual(self, currentCell, previousCell);
        } else if (&type == &w.std.op.GreaterThan) {
            evalOpGreaterThan(self, currentCell, previousCell);
        } else if (&type == &w.std.op.GreaterThanOrEqual) {
            evalOpGreaterThanOrEqual(self, currentCell, previousCell);
        } else if (&type == &w.std.op.And) {
            evalOpAnd(self, currentCell, previousCell);
        } else if (&type == &w.std.op.Or) {
            evalOpOr(self, currentCell, previousCell);
        } else if (&type == &w.std.op.Not) {
            evalOpNot(self, currentCell, previousCell);
        } else if (&type == &w.std.op.If) {
            evalOpIf(self, currentCell, previousCell);
        } else if (&type == &w.std.op.Do) {
            evalOpDo(self, currentCell, previousCell);
        } else if (&type == &w.std.op.While) {
            evalOpWhile(self, currentCell, previousCell);
        } else if (&type == &w.std.op.Block) {
            evalOpBlock(self, currentCell, previousCell);
        } else if (&type == &w.std.op.Add) {
            evalOpAdd(self, currentCell, previousCell);
        } else if (&type == &w.std.op.Subtract) {
            evalOpSubtract(self, currentCell, previousCell);
        } else if (&type == &w.std.op.Multiply) {
            evalOpMultiply(self, currentCell, previousCell);
        } else if (&type == &w.std.op.Divide) {
            evalOpDivide(self, currentCell, previousCell);
        } else {
            // assuming it is a datacell, so do nothing
            std::swap(currentCell, previousCell);
        }
        w.ap.m_currentCell = currentCell;
        w.ap.m_previousCell = previousCell;
        ++tick;
    } while (currentCell != &w.id.emptyObject);
    w.ap.m_time.value(w.ap.m_time.value() + tick);
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
    resetIndent();
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
        localVar.set(w.id.valueType, slot[w.id.type]);
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
