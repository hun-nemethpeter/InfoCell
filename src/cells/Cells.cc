#include "Cells.h"
#include "World.h"

#include <utility>

#pragma warning(disable : 4996)
#include <utf8.h>


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

CellI& CellI::struct_()
{
    return (*this)[w.ids.struct_];
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

bool CellI::isA(CellI& ptype)
{
    return &struct_() == &ptype || (has(w.ids.memberOf) && (*this)[w.ids.memberOf][w.ids.index].has(ptype));
}

bool CellI::isA(CellI& cell, CellI& type) const
{
    return &cell == &type || (cell.has(w.ids.memberOf) && cell[w.ids.memberOf][w.ids.index].has(type));
}

bool CellI::operator==(CellI& rhs)
{
    if (this == &rhs) {
        return true;
    }
    if (&struct_() != &rhs.struct_()) {
        return false;
    }
    CellI& slotList    = struct_()[w.ids.slots][w.ids.list];
    CellI* slotItemPtr = slotList.has(w.ids.first) ? &slotList[w.ids.first] : nullptr;
    while (slotItemPtr) {
        CellI& slotItem = *slotItemPtr;
        CellI& slot     = slotItem[w.ids.value];
        CellI& key      = slot[w.ids.key];

        bool hasLeftSlot = has(key);
        if (hasLeftSlot != rhs.has(key)) {
            return false;
        }
        if (hasLeftSlot && (&(*this)[key] != &rhs[key])) {
            return false;
        }

        slotItemPtr = slotItem.has(w.ids.next) ? &slotItem[w.ids.next] : nullptr;
    }

    return true;
}

bool CellI::operator!=(CellI& rhs)
{
    return !((*this) == rhs);
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
    m_slots[&w.ids.struct_] = &type;
}

Object::Object(World& w, CellI& type, CellI& constructor, const std::string& label) :
    CellI(w, label),
    m_type(type)
{
    m_slots[&w.ids.struct_] = &type;
    getMethod(constructor)();
}

Object::Object(World& w, CellI& type, CellI& constructor, Param param1, const std::string& label) :
    CellI(w, label),
    m_type(type)
{
    m_slots[&w.ids.struct_] = &type;

    CellI& method = getMethod(constructor);
    setFnParam(method, param1);
    method();
}

Object::Object(World& w, CellI& type, CellI& constructor, Param param1, Param param2, const std::string& label) :
    CellI(w, label),
    m_type(type)
{
    m_slots[&w.ids.struct_] = &type;

    CellI& method = getMethod(constructor);
    setFnParam(method, param1);
    setFnParam(method, param2);
    method();
}

Object::Object(World& w, CellI& type, CellI& constructor, Param param1, Param param2, Param param3, const std::string& label) :
    CellI(w, label),
    m_type(type)
{
    m_slots[&w.ids.struct_] = &type;

    CellI& method = getMethod(constructor);
    setFnParam(method, param1);
    setFnParam(method, param2);
    setFnParam(method, param3);
    method();
}

bool Object::s_debugFunctionCalls = false;

Object::Object(World& w, CellI& type, CellI& constructor, Param param1, Param param2, Param param3, Param param4, const std::string& label) :
    CellI(w, label),
    m_type(type)
{
    m_slots[&w.ids.struct_] = &type;

    CellI& method = getMethod(constructor);
    setFnParam(method, param1);
    setFnParam(method, param2);
    setFnParam(method, param3);
    setFnParam(method, param4);
    method();
}

Object::~Object()
{
    if (w.initPhase() == InitPhase::Init || w.initPhase() == InitPhase::DestructBegin) {
        return;
    }
    if (!hasMethod(w.ids.destructor)) {
        return;
    }
    destructor();
}

bool Object::has(CellI& key)
{
    if (&key == &w.ids.struct_)
        return true;

    return m_slots.find(&key) != m_slots.end();
}

void Object::set(CellI& key, CellI& value)
{
    if ((&key == &w.ids.struct_) && !((&struct_() == &w.std.Index))) {
        throw "Type change not allowed.";
    }
    if (w.initPhase() == InitPhase::Init) {
        m_slots[&key] = &value;
        return;
    }
    auto is = [this](CellI& rhsType) -> bool { return &struct_() == &rhsType || (struct_().has(w.ids.memberOf) && struct_()[w.ids.memberOf][w.ids.index].has(rhsType)); };
    if (is(w.std.Index) || struct_()[w.ids.slots][w.ids.index].has(key)) {
        m_slots[&key] = &value;
    } else {
        throw "The type doesn't contains this key.";
    }
}

void Object::erase(CellI& key)
{
    if (&key == &w.ids.struct_) {
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

// core data handling
static void evalOpGet(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.ids.state)) {
        self.set(w.ids.state, w.ids.stateParamInit);
    }
    CellI& state = self[w.ids.state];
    if (&state == &w.ids.stateParamInit) {
        self.set(w.ids.previous, *previousCell);
        CellI& inputCell = self[w.ids.cell];
        previousCell     = currentCell;
        currentCell      = &inputCell;
        self.set(w.ids.state, w.ids.stateParam1);
    } else if (&state == &w.ids.stateParam1) {
        CellI& inputRole = self[w.ids.key];
        previousCell     = currentCell;
        currentCell      = &inputRole;
        self.set(w.ids.state, w.ids.stateParam2);
    } else if (&state == &w.ids.stateParam2) {
        CellI& cell = self[w.ids.cell][w.ids.value];
        CellI& key = self[w.ids.key][w.ids.value];

        self.set(w.ids.value, cell[key]);
        self.set(w.ids.state, w.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self[w.ids.previous];
    }
}

static void evalOpSet(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.ids.state)) {
        self.set(w.ids.state, w.ids.stateParamInit);
    }
    CellI& state = self[w.ids.state];
    if (&state == &w.ids.stateParamInit) {
        self.set(w.ids.previous, *previousCell);
        CellI& inputCell = self[w.ids.cell];
        previousCell     = currentCell;
        currentCell      = &inputCell;
        self.set(w.ids.state, w.ids.stateParam1);
    } else if (&state == &w.ids.stateParam1) {
        CellI& inputRole = self[w.ids.key];
        previousCell     = currentCell;
        currentCell      = &inputRole;
        self.set(w.ids.state, w.ids.stateParam2);
    } else if (&state == &w.ids.stateParam2) {
        CellI& inputValue = self[w.ids.value];
        previousCell      = currentCell;
        currentCell       = &inputValue;
        self.set(w.ids.state, w.ids.stateParam3);
    } else if (&state == &w.ids.stateParam3) {
        CellI& cell  = self[w.ids.cell][w.ids.value];
        CellI& key  = self[w.ids.key][w.ids.value];
        CellI& value = self[w.ids.value][w.ids.value];

        cell.set(key, value);
        self.set(w.ids.state, w.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self[w.ids.previous];
    }
}

static void evalOpHas(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.ids.state)) {
        self.set(w.ids.state, w.ids.stateParamInit);
    }
    CellI& state = self[w.ids.state];
    if (&state == &w.ids.stateParamInit) {
        self.set(w.ids.previous, *previousCell);
        CellI& inputCell = self[w.ids.cell];
        previousCell     = currentCell;
        currentCell      = &inputCell;
        self.set(w.ids.state, w.ids.stateParam1);
    } else if (&state == &w.ids.stateParam1) {
        CellI& inputRole = self[w.ids.key];
        previousCell     = currentCell;
        currentCell      = &inputRole;
        self.set(w.ids.state, w.ids.stateParam2);
    } else if (&state == &w.ids.stateParam2) {
        CellI& cell = self[w.ids.cell][w.ids.value];
        CellI& key = self[w.ids.key][w.ids.value];

        self.set(w.ids.value, w.toCellBool(cell.has(key)));
        self.set(w.ids.state, w.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self[w.ids.previous];
    }
}

static void evalOpMissing(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.ids.state)) {
        self.set(w.ids.state, w.ids.stateParamInit);
    }
    CellI& state = self[w.ids.state];
    if (&state == &w.ids.stateParamInit) {
        self.set(w.ids.previous, *previousCell);
        CellI& inputCell = self[w.ids.cell];
        previousCell     = currentCell;
        currentCell      = &inputCell;
        self.set(w.ids.state, w.ids.stateParam1);
    } else if (&state == &w.ids.stateParam1) {
        CellI& inputRole = self[w.ids.key];
        previousCell     = currentCell;
        currentCell      = &inputRole;
        self.set(w.ids.state, w.ids.stateParam2);
    } else if (&state == &w.ids.stateParam2) {
        CellI& cell = self[w.ids.cell][w.ids.value];
        CellI& key = self[w.ids.key][w.ids.value];

        self.set(w.ids.value, w.toCellBool(cell.missing(key)));
        self.set(w.ids.state, w.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self[w.ids.previous];
    }
}

static void evalOpErase(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.ids.state)) {
        self.set(w.ids.state, w.ids.stateParamInit);
    }
    CellI& state = self[w.ids.state];
    if (&state == &w.ids.stateParamInit) {
        self.set(w.ids.previous, *previousCell);
        CellI& inputCell = self[w.ids.cell];
        previousCell     = currentCell;
        currentCell      = &inputCell;
        self.set(w.ids.state, w.ids.stateParam1);
    } else if (&state == &w.ids.stateParam1) {
        CellI& inputRole = self[w.ids.key];
        previousCell     = currentCell;
        currentCell      = &inputRole;
        self.set(w.ids.state, w.ids.stateParam2);
    } else if (&state == &w.ids.stateParam2) {
        CellI& cell = self[w.ids.cell][w.ids.value];
        CellI& key = self[w.ids.key][w.ids.value];

        cell.erase(key);
        self.set(w.ids.state, w.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self[w.ids.previous];
    }
}

static void evalOpNew(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.ids.state)) {
        self.set(w.ids.state, w.ids.stateParamInit);
    }
    CellI& state = self[w.ids.state];
    if (&state == &w.ids.stateParamInit) {
        self.set(w.ids.previous, *previousCell);
        CellI& inputObjectType = self[w.ids.objectType];
        previousCell           = currentCell;
        currentCell            = &inputObjectType;
        self.set(w.ids.state, w.ids.stateParam1);
    } else if (&state == &w.ids.stateParam1) {
        CellI& objectType = self[w.ids.objectType][w.ids.value];

        if (&objectType == &w.std.Number) {
            self.set(w.ids.value, *new Number(w));
        } else if (&objectType == &w.std.String) {
            self.set(w.ids.value, *new String(w));
        } else {
            self.set(w.ids.value, *new Object(w, objectType));
        }
        self.set(w.ids.state, w.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self[w.ids.previous];
    }
}

static void evalOpDelete(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.ids.state)) {
        self.set(w.ids.state, w.ids.stateParamInit);
    }
    CellI& state = self[w.ids.state];
    if (&state == &w.ids.stateParamInit) {
        self.set(w.ids.previous, *previousCell);
        CellI& input = self[w.ids.input];
        previousCell = currentCell;
        currentCell  = &input;
        self.set(w.ids.state, w.ids.stateParam1);
    } else if (&state == &w.ids.stateParam1) {
        CellI& input = self[w.ids.input];
        CellI* cell  = &input[w.ids.value];

        delete cell;
        input.erase(w.ids.value);
        self.set(w.ids.state, w.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self[w.ids.previous];
    }
}

// code running
static void evalOpActivate(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.ids.state)) {
        self.set(w.ids.state, w.ids.stateParamInit);
    }
    CellI& state = self[w.ids.state];
    if (&state == &w.ids.stateParamInit) {
        self.set(w.ids.previous, *previousCell);
        CellI& inputCell = self[w.ids.cell];

        previousCell = currentCell;
        currentCell  = &inputCell;
        self.set(w.ids.state, w.ids.stateParam1);
    } else if (&state == &w.ids.stateParam1) {
        previousCell     = currentCell;
        CellI& inputCell = self[w.ids.cell];
        CellI* status    = &w.ids.process;

        if (self.has(w.ids.parent)) {
            CellI& parent = self[w.ids.parent];
            if (&inputCell.struct_() == &w.std.op.Return || (inputCell.has(w.ids.status) && (&inputCell[w.ids.status] == &w.ids.return_))) {
                parent.set(w.ids.status, w.ids.return_);
                status = &w.ids.return_;
            } else if (&parent[w.ids.status] == &w.ids.continue_ || &parent[w.ids.status] == &w.ids.break_) {
                status = &parent[w.ids.status];
            } else if (self.has(w.ids.status)) {
                if (&self[w.ids.status] == &w.ids.return_ || &self[w.ids.status] == &w.ids.continue_ || &self[w.ids.status] == &w.ids.break_) {
                    parent.set(w.ids.status, self[w.ids.status]);
                    status = &self[w.ids.status];
                }
            }
        }
        if (status == &w.ids.process && self.has(w.ids.next)) {
            CellI& nextCell = self[w.ids.next];
            currentCell     = &nextCell;
        } else {
            if (self.has(w.ids.parent)) {
                currentCell = &self[w.ids.parent];
            } else {
                currentCell = &self[w.ids.previous];
            }
        }
        self.set(w.ids.state, w.ids.stateParamInit);
    }
}

static void saveOpState(List& opStates, CellI& op)
{
    World& w = op.w;

    if (&op == &w.ids.emptyObject) {
        return;
    }
    CellI& type      = op.struct_();
    Object& opState  = *new Object(w, w.std.OpState);

    opState.set(w.ids.op, op);
    opState.set(w.ids.state, w.ids.state);
    opState.set(w.ids.value, op[w.ids.state]);
    opStates.add(opState);
    if (op.has(w.ids.status)) {
        Object& opState = *new Object(w, w.std.OpState);
        opState.set(w.ids.op, op);
        opState.set(w.ids.state, w.ids.status);
        opState.set(w.ids.value, op[w.ids.status]);
        opStates.add(opState);
    }
    if (op.has(w.ids.previous)) {
        Object& opState = *new Object(w, w.std.OpState);
        opState.set(w.ids.op, op);
        opState.set(w.ids.state, w.ids.previous);
        opState.set(w.ids.value, op[w.ids.previous]);
        opStates.add(opState);
    }
    if (&type == &w.std.op.Call) {
        if (op[w.ids.cell].has(w.ids.value)) {
            Object& opState = *new Object(w, w.std.OpState);
            opState.set(w.ids.op, op);
            opState.set(w.ids.state, w.ids.cell);
            opState.set(w.ids.value, op[w.ids.cell][w.ids.value]);
            opStates.add(opState);
        }
        if (op[w.ids.method].has(w.ids.value)) {
            Object& opState = *new Object(w, w.std.OpState);
            opState.set(w.ids.op, op);
            opState.set(w.ids.state, w.ids.method);
            opState.set(w.ids.value, op[w.ids.method][w.ids.value]);
            opStates.add(opState);
        }
        if (op[w.ids.stack].has(w.ids.value)) {
            Object& opState = *new Object(w, w.std.OpState);
            opState.set(w.ids.op, op);
            opState.set(w.ids.state, w.ids.stack);
            opState.set(w.ids.value, op[w.ids.stack][w.ids.value]);
            opStates.add(opState);
        }
    }
    if (&type == &w.std.op.Set || &type == &w.std.op.Get) {
        if (op[w.ids.cell].has(w.ids.value)) {
            Object& opState = *new Object(w, w.std.OpState);
            opState.set(w.ids.op, op);
            opState.set(w.ids.state, w.ids.cell);
            opState.set(w.ids.value, op[w.ids.cell][w.ids.value]);
            opStates.add(opState);
        }
        if (op[w.ids.key].has(w.ids.value)) {
            Object& opState = *new Object(w, w.std.OpState);
            opState.set(w.ids.op, op);
            opState.set(w.ids.state, w.ids.key);
            opState.set(w.ids.value, op[w.ids.key][w.ids.value]);
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
        opState.set(w.ids.op, op);
        opState.set(w.ids.state, w.ids.lhs);
        opState.set(w.ids.value, op[w.ids.lhs][w.ids.value]);
        opStates.add(opState);
    }
    op.set(w.ids.state, w.ids.stateParamInit);
}

static void loadOpState(CellI& opState)
{
    World& w = opState.w;
    CellI& op        = opState[w.ids.op];
    CellI& type      = op.struct_();
    CellI& state     = opState[w.ids.state];
    CellI& value     = opState[w.ids.value];

    if ((&type == &w.std.op.Set || &type == &w.std.op.Get) && ((&state == &w.ids.cell) || (&state == &w.ids.key))) {
        op[state].set(w.ids.value, value);
    } else if ((&type == &w.std.op.Call) && ((&state == &w.ids.cell) || (&state == &w.ids.method) || (&state == &w.ids.stack))) {
        op[state].set(w.ids.value, value);
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
        &type == &w.std.op.Divide) && (&state == &w.ids.lhs)) {
        op[state].set(w.ids.value, value);
    } else {
        op.set(state, value);
    }
//    std::cout << "         set (" << &op << ")" << op.struct_().label() << "[" << state.label() << ":" << value.label() << "]" << std::endl;
}

static void evalOpCall(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.ids.state)) {
        self.set(w.ids.state, w.ids.stateParamInit);
    }
//    std::cout << "evalOpCall self: " << &self << ", state: " << self[w.ids.state].label() << std::endl;
    CellI& state = self[w.ids.state];
    if (&state == &w.ids.stateParamInit) {
        self.set(w.ids.previous, *previousCell);
        CellI& inputCell = self[w.ids.cell];
        previousCell     = currentCell;
        currentCell      = &inputCell;
        self.set(w.ids.state, w.ids.stateParam1);
    } else if (&state == &w.ids.stateParam1) {
        CellI& inputMethod = self[w.ids.method];
        previousCell       = currentCell;
        currentCell        = &inputMethod;
        self.set(w.ids.state, w.ids.stateParam2);
    } else if (&state == &w.ids.stateParam2) {
        CellI& inputStack = self[w.ids.stack];
        previousCell      = currentCell;
        currentCell       = &inputStack;
        if (self.has(w.ids.parameters)) {
            self.set(w.ids.state, w.ids.stateParamEval);
        } else {
            self.set(w.ids.state, w.ids.stateParam3);
        }
    } else if (&state == &w.ids.stateParamEval) {
        CellI* paramNodePtr = nullptr;
        if (self.missing(w.ids.currentParam)) {
            CellI& paramList = self[w.ids.parameters];
            paramNodePtr     = &paramList[w.ids.first];
        } else {
            CellI& paramNode = self[w.ids.currentParam];
            if (paramNode.has(w.ids.next)) {
                paramNodePtr = &paramNode[w.ids.next];
            } else {
                self.erase(w.ids.currentParam);
            }
        }
        if (paramNodePtr) {
            CellI& param = (*paramNodePtr)[w.ids.value][w.ids.type];
            self.set(w.ids.currentParam, *paramNodePtr);
            previousCell = currentCell;
            currentCell  = &param;
        } else {
            self.set(w.ids.state, w.ids.stateParam3);
        }
    } else if (&state == &w.ids.stateParam3) {
        CellI& cell       = self[w.ids.cell][w.ids.value];
        CellI& methodName = self[w.ids.method][w.ids.value];
        CellI& stack      = self[w.ids.stack][w.ids.value];

        CellI* methodPtr = nullptr;
        if (&self[w.ids.ast].struct_() == &w.std.ast.Call) {
            methodPtr = &cell[w.ids.struct_][w.ids.methods];
        } else {
            methodPtr = &cell[w.ids.methods];
        }
        CellI& method = (*methodPtr)[w.ids.index][methodName][w.ids.value];

        CellI& stackFrame = *new Object(w, w.std.StackFrame);
        stackFrame.set(w.ids.method, method);

        CellI& inputIndex = *new Object(w, w.std.Index);
        inputIndex.set(w.ids.self, cell);
        if (self.has(w.ids.parameters)) {
            Visitor::visitList(self[w.ids.parameters], [&self, &w, &inputIndex](CellI& parameter, int, bool& stop) {
                inputIndex.set(parameter[w.ids.key], parameter[w.ids.type][w.ids.value]);
//                static_cast<Object&>(self).printIndent();
//                std::cout << parameter[w.ids.key].label() << ":" << parameter[w.ids.type][w.ids.value].label() << std::endl;
            });
        }
        stackFrame.set(w.ids.input, inputIndex);

        if (method.struct_()[w.ids.subTypes][w.ids.index].has(w.ids.localVars)) {
            CellI& localVarsList  = method.struct_()[w.ids.subTypes][w.ids.index][w.ids.localVars][w.ids.value][w.ids.slots][w.ids.list];
            Index& localVarsIndex = *new Index(w /*, method.struct_()[w.ids.subTypes][w.ids.index][w.ids.localVars][w.ids.value] */);
            if (method.struct_()[w.ids.subTypes][w.ids.index].has(w.ids.localVars)) {
                Visitor::visitList(localVarsList, [&self, &w, &localVarsIndex](CellI& slot, int, bool& stop) {
                    localVarsIndex.set(slot[w.ids.key], *new Object(w, w.std.op.Var));
                });
                stackFrame.set(w.ids.localVars, localVarsIndex);
            }
        }
        CellI& newStackListItem = *new Object(w, w.std.ListItem);
        newStackListItem.set(w.ids.value, stackFrame);
        newStackListItem.set(w.ids.previous, stack);
        stack.set(w.ids.next, newStackListItem);

        CellI& previousMethod = stack[w.ids.value][w.ids.method];
//        std::cout << "previous method " << previousMethod.label() << std::endl;
//        std::cout << "next     method " << method.label() << std::endl;

        method.set(w.ids.stack, newStackListItem);
        self.set(w.ids.state, w.ids.stateStackCall);
        previousMethod.set(w.ids.lastOp, self);

        if (method.has(w.ids.state) && (&method[w.ids.state] != &w.ids.stateParamInit)) {
//            std::cout << "recursive call for " << method.struct_().label() << std::endl;
            List& cellPath = *new List(w, w.std.op.Base);
            CellI& lastOp  = method[w.ids.lastOp];
            for (CellI* currentOp = &lastOp; currentOp != &method; currentOp = (*currentOp).has(w.ids.parent) ? &(*currentOp)[w.ids.parent] : &(*currentOp)[w.ids.previous]) {
                CellI& op = *currentOp;
//                std::cout << "         [" << op.struct_().label() << ":" << op[w.ids.state].label() << "]" << std::endl;
                saveOpState(cellPath, op);
            }
            saveOpState(cellPath, method);
            method.set(w.ids.state, w.ids.stateParamInit);
            stackFrame.set(w.ids.ops, cellPath);
//            std::cout << std::endl;
        }
        previousCell = currentCell;
        currentCell  = &method;
    } else if (&state == &w.ids.stateStackCall) {
        CellI& cell       = self[w.ids.cell][w.ids.value];
        CellI& methodName = self[w.ids.method][w.ids.value];

        CellI* methodPtr = nullptr;
        if (&self[w.ids.ast].struct_() == &w.std.ast.Call) {
            methodPtr = &cell[w.ids.struct_][w.ids.methods];
        } else {
            methodPtr = &cell[w.ids.methods];
        }
        // TODO: cache the method obj
        CellI& method = (*methodPtr)[w.ids.index][methodName][w.ids.value];
        if (method.has(w.ids.value)) {
            self.set(w.ids.value, method[w.ids.value]);
//            static_cast<Object&>(self).printIndent();
//            std::cout << "return " << method[w.ids.value].label() << std::endl;
        }

        previousCell = currentCell;
        currentCell  = &self[w.ids.previous];
        self.set(w.ids.state, w.ids.stateParamInit);
    }
}

static void evalOpFunction(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.ids.state)) {
        self.set(w.ids.state, w.ids.stateParamInit);
    }
    CellI& state = self[w.ids.state];
    if (&state == &w.ids.stateParamInit) {
        self.set(w.ids.previous, *previousCell);
        CellI& op = self[w.ids.op];
        if (op.has(w.ids.state) && (&op[w.ids.state] != &w.ids.stateParamInit)) {
            throw "Error: function contains non-clean op.";
        }
        previousCell = currentCell;
        currentCell  = &op;
        self.set(w.ids.state, w.ids.stateParam1);
    } else if (&state == &w.ids.stateParam1) {
        previousCell = currentCell;
        currentCell  = &self[w.ids.previous];

        CellI& stackNode = self[w.ids.stack];
        CellI& stackFrame = stackNode[w.ids.value];
        static_cast<Object&>(self).printIndent();
//        std::cout << "return " << std::endl;
//        std::cout << "return " << self.label() << std::endl;
        if (stackFrame.has(w.ids.ops)) {
            Visitor::visitList(stackFrame[w.ids.ops], [&w](CellI& opState, int, bool& stop) {
                loadOpState(opState);
                delete &opState;
            });
            static_cast<List&>(stackFrame[w.ids.ops]).clear();
            delete &stackFrame[w.ids.ops];
            stackFrame.erase(w.ids.ops);
        } else {
            self.set(w.ids.state, w.ids.stateParamInit);
        }
        //
        if (self[w.ids.stack][w.ids.previous].has(w.ids.value)) {
            CellI& inputIndex         = stackFrame[w.ids.input];
            CellI& previousStackNode  = self[w.ids.stack][w.ids.previous];
            CellI& previousStackFrame = previousStackNode[w.ids.value];
            CellI& previousMethod     = previousStackFrame[w.ids.method];
            previousMethod.set(w.ids.stack, previousStackNode);
            previousStackNode.erase(w.ids.next);
            delete &inputIndex;
            if (stackFrame.has(w.ids.localVars)) {
                CellI& localVarsList  = self.struct_()[w.ids.subTypes][w.ids.index][w.ids.localVars][w.ids.value][w.ids.slots][w.ids.list];
                CellI& localVarsIndex = stackFrame[w.ids.localVars];
                Visitor::visitList(localVarsList, [&self, &w, &localVarsIndex](CellI& slot, int, bool& stop) {
                    delete &localVarsIndex[slot[w.ids.key]];
                });
                delete &localVarsIndex;
            }
            delete &stackFrame;
            delete &stackNode;
            if (currentCell == &w.ids.emptyObject) {
                std::cout << "";
            }
        } else {
            self.set(w.ids.state, w.ids.stateParamInit);
            currentCell = &w.ids.emptyObject;
        }
    }
}

static void evalOpIf(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.ids.state)) {
        self.set(w.ids.state, w.ids.stateParamInit);
    }
    CellI& state = self[w.ids.state];
    if (&state == &w.ids.stateParamInit) {
        self.set(w.ids.previous, *previousCell);
        CellI& inputCondition = self[w.ids.condition];

        previousCell = currentCell;
        currentCell  = &inputCondition;
        self.set(w.ids.state, w.ids.stateParam1);
    } else if (&state == &w.ids.stateParam1) {
        self.set(w.ids.status, w.ids.process);
        CellI* branchPtr = nullptr;
        bool condition   = &self[w.ids.condition][w.ids.value] == &w.boolean.true_;
        if (condition) {
            branchPtr = &self[w.ids.then];
            self.set(w.ids.state, w.ids.stateThen);
        } else if (self.has(w.ids.else_)) {
            branchPtr = &self[w.ids.else_];
            self.set(w.ids.state, w.ids.stateElse);
        }
        previousCell = currentCell;
        if (branchPtr) {
            currentCell = branchPtr;
        } else {
            currentCell = &self[w.ids.previous];
            self.set(w.ids.state, w.ids.stateParamInit);
        }
    }
 else if (&state == &w.ids.stateThen || &state == &w.ids.stateElse) {
     CellI& branch = &state == &w.ids.stateThen ? self[w.ids.then] : self[w.ids.else_];
     if (&branch.struct_() == &w.std.op.Return) {
         self.set(w.ids.status, w.ids.return_);
     }
     else if (branch.has(w.ids.status)) {
         self.set(w.ids.status, branch[w.ids.status]);
     }

     previousCell = currentCell;
     currentCell = &self[w.ids.previous];
     self.set(w.ids.state, w.ids.stateParamInit);
    }
}

static void evalOpDo(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.ids.state)) {
        self.set(w.ids.state, w.ids.stateParamInit);
    }
    CellI& state = self[w.ids.state];
    if (&state == &w.ids.stateParamInit) {
        self.set(w.ids.previous, *previousCell);
        self.set(w.ids.status, w.ids.process);
        CellI& statement = self[w.ids.statement];
        previousCell = currentCell;
        currentCell = &statement;
        self.set(w.ids.state, w.ids.stateStatement);
    } else if (&state == &w.ids.stateStatement) {
        CellI& statement = self[w.ids.statement];
        if (&statement.struct_() == &w.std.op.Return) {
            self.set(w.ids.status, w.ids.return_);
        }
        else if (statement.has(w.ids.status)) {
            self.set(w.ids.status, statement[w.ids.status]);
        }
        CellI& inputCondition = self[w.ids.condition];
        previousCell = currentCell;
        currentCell = &inputCondition;
        self.set(w.ids.state, w.ids.stateCondition);
    } else if (&state == &w.ids.stateCondition) {
        previousCell = currentCell;
        if (self.has(w.ids.status) && (&self[w.ids.status] == &w.ids.return_)) {
            currentCell = &self[w.ids.previous];
            self.set(w.ids.state, w.ids.stateParamInit);
        } else {
            self.set(w.ids.status, w.ids.process);
            bool condition = &self[w.ids.condition][w.ids.value] == &w.boolean.true_;
            if (condition) {
                currentCell = &self[w.ids.statement];
                self.set(w.ids.state, w.ids.stateStatement);
            } else {
                currentCell = &self[w.ids.previous];
                self.set(w.ids.state, w.ids.stateParamInit);
            }
        }
    }
}

static void evalOpWhile(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.ids.state)) {
        self.set(w.ids.state, w.ids.stateParamInit);
    }
    CellI& state = self[w.ids.state];
    if (&state == &w.ids.stateParamInit) {
        self.set(w.ids.previous, *previousCell);
        self.set(w.ids.status, w.ids.process);
        CellI& inputCondition = self[w.ids.condition];
        previousCell = currentCell;
        currentCell = &inputCondition;
        self.set(w.ids.state, w.ids.stateCondition);
    } else if (&state == &w.ids.stateStatement) {
        CellI& statement = self[w.ids.statement];
        if (&statement.struct_() == &w.std.op.Return) {
            self.set(w.ids.status, w.ids.return_);
        }
        else if (statement.has(w.ids.status)) {
            self.set(w.ids.status, statement[w.ids.status]);
        }
        CellI& inputCondition = self[w.ids.condition];
        previousCell = currentCell;
        currentCell = &inputCondition;
        self.set(w.ids.state, w.ids.stateCondition);
    } else if (&state == &w.ids.stateCondition) {
        previousCell = currentCell;
        if (self.has(w.ids.status) && (&self[w.ids.status] == &w.ids.return_)) {
            currentCell = &self[w.ids.previous];
            self.set(w.ids.state, w.ids.stateParamInit);
        } else {
            self.set(w.ids.status, w.ids.process);
            bool condition = &self[w.ids.condition][w.ids.value] == &w.boolean.true_;
            if (condition) {
                currentCell = &self[w.ids.statement];
                self.set(w.ids.state, w.ids.stateStatement);
            } else {
                currentCell = &self[w.ids.previous];
                self.set(w.ids.state, w.ids.stateParamInit);
            }
        }
    }
}

static void evalOpBlock(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.ids.state)) {
        self.set(w.ids.state, w.ids.stateParamInit);
    }
    CellI& state = self[w.ids.state];
    if (&state == &w.ids.stateParamInit) {
        self.set(w.ids.previous, *previousCell);
        CellI& ops   = self[w.ids.ops];
        previousCell = currentCell;
        currentCell  = &ops;
        self.set(w.ids.status, w.ids.process);
        self.set(w.ids.state, w.ids.stateParam1);
    } else if (&state == &w.ids.stateParam1) {
        previousCell = currentCell;
        currentCell  = &self[w.ids.previous];
        self.set(w.ids.state, w.ids.stateParamInit);
    }
}

static void evalOpReturn(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.ids.state)) {
        self.set(w.ids.state, w.ids.stateParamInit);
    }
    CellI& state = self[w.ids.state];
    if (self.has(w.ids.result)) {
        if (&state == &w.ids.stateParamInit) {
            self.set(w.ids.previous, *previousCell);
            CellI& result = self[w.ids.result];
            previousCell  = currentCell;
            currentCell   = &result;
            self.set(w.ids.state, w.ids.stateParam1);
        } else if (&state == &w.ids.stateParam1) {
            previousCell = currentCell;
            currentCell  = &self[w.ids.previous];
            self.set(w.ids.state, w.ids.stateParamInit);
        }
    } else {
        std::swap(currentCell, previousCell);
    }
}

// compare
static void evalOpSame(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.ids.state)) {
        self.set(w.ids.state, w.ids.stateParamInit);
    }
    CellI& state = self[w.ids.state];
    if (&state == &w.ids.stateParamInit) {
        self.set(w.ids.previous, *previousCell);
        CellI& inputLhs  = self[w.ids.lhs];
        previousCell     = currentCell;
        currentCell      = &inputLhs;
        self.set(w.ids.state, w.ids.stateLhs);
    } else if (&state == &w.ids.stateLhs) {
        CellI& inputRhs  = self[w.ids.rhs];
        previousCell     = currentCell;
        currentCell      = &inputRhs;
        self.set(w.ids.state, w.ids.stateRhs);
    } else if (&state == &w.ids.stateRhs) {
        CellI* lhs = &self[w.ids.lhs][w.ids.value];
        CellI* rhs = &self[w.ids.rhs][w.ids.value];

        self.set(w.ids.value, w.toCellBool(lhs == rhs));
        self.set(w.ids.state, w.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self[w.ids.previous];
    }
}

static void evalOpNotSame(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.ids.state)) {
        self.set(w.ids.state, w.ids.stateParamInit);
    }
    CellI& state = self[w.ids.state];
    if (&state == &w.ids.stateParamInit) {
        self.set(w.ids.previous, *previousCell);
        CellI& inputLhs = self[w.ids.lhs];
        previousCell    = currentCell;
        currentCell     = &inputLhs;
        self.set(w.ids.state, w.ids.stateLhs);
    } else if (&state == &w.ids.stateLhs) {
        CellI& inputRhs = self[w.ids.rhs];
        previousCell    = currentCell;
        currentCell     = &inputRhs;
        self.set(w.ids.state, w.ids.stateRhs);
    } else if (&state == &w.ids.stateRhs) {
        CellI* lhs = &self[w.ids.lhs][w.ids.value];
        CellI* rhs = &self[w.ids.rhs][w.ids.value];

        self.set(w.ids.value, w.toCellBool(lhs != rhs));
        self.set(w.ids.state, w.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self[w.ids.previous];
    }
}

static void evalOpEqual(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.ids.state)) {
        self.set(w.ids.state, w.ids.stateParamInit);
    }
    CellI& state = self.get(w.ids.state);
    if (&state == &w.ids.stateParamInit) {
        self.set(w.ids.previous, *previousCell);
        CellI& inputLhs = self.get(w.ids.lhs);
        previousCell    = currentCell;
        currentCell     = &inputLhs;
        self.set(w.ids.state, w.ids.stateLhs);
    } else if (&state == &w.ids.stateLhs) {
        CellI& inputRhs = self.get(w.ids.rhs);
        previousCell    = currentCell;
        currentCell     = &inputRhs;
        self.set(w.ids.state, w.ids.stateRhs);
    } else if (&state == &w.ids.stateRhs) {
        CellI& lhs = self[w.ids.lhs][w.ids.value];
        CellI& rhs = self[w.ids.rhs][w.ids.value];

        self.set(w.ids.value, w.toCellBool(lhs == rhs));
        self.set(w.ids.state, w.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self.get(w.ids.previous);
    }
}

static void evalOpNotEqual(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.ids.state)) {
        self.set(w.ids.state, w.ids.stateParamInit);
    }
    CellI& state = self.get(w.ids.state);
    if (&state == &w.ids.stateParamInit) {
        self.set(w.ids.previous, *previousCell);
        CellI& inputLhs = self.get(w.ids.lhs);
        previousCell    = currentCell;
        currentCell     = &inputLhs;
        self.set(w.ids.state, w.ids.stateLhs);
    } else if (&state == &w.ids.stateLhs) {
        CellI& inputRhs = self.get(w.ids.rhs);
        previousCell    = currentCell;
        currentCell     = &inputRhs;
        self.set(w.ids.state, w.ids.stateRhs);
    } else if (&state == &w.ids.stateRhs) {
        CellI& lhs = self[w.ids.lhs][w.ids.value];
        CellI& rhs = self[w.ids.rhs][w.ids.value];

        self.set(w.ids.value, w.toCellBool(lhs != rhs));
        self.set(w.ids.state, w.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self.get(w.ids.previous);
    }
}

static void evalOpLessThan(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.ids.state)) {
        self.set(w.ids.state, w.ids.stateParamInit);
    }
    CellI& state = self.get(w.ids.state);
    if (&state == &w.ids.stateParamInit) {
        self.set(w.ids.previous, *previousCell);
        CellI& inputLhs = self.get(w.ids.lhs);
        previousCell    = currentCell;
        currentCell     = &inputLhs;
        self.set(w.ids.state, w.ids.stateLhs);
    } else if (&state == &w.ids.stateLhs) {
        CellI& inputRhs = self.get(w.ids.rhs);
        previousCell    = currentCell;
        currentCell     = &inputRhs;
        self.set(w.ids.state, w.ids.stateRhs);
    } else if (&state == &w.ids.stateRhs) {
        int lhs = static_cast<Number&>(self[w.ids.lhs][w.ids.value]).value();
        int rhs = static_cast<Number&>(self[w.ids.rhs][w.ids.value]).value();

        self.set(w.ids.value, lhs < rhs ? w.boolean.true_ : w.boolean.false_);
        self.set(w.ids.state, w.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self.get(w.ids.previous);
    }
}

static void evalOpLessThanOrEqual(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.ids.state)) {
        self.set(w.ids.state, w.ids.stateParamInit);
    }
    CellI& state = self.get(w.ids.state);
    if (&state == &w.ids.stateParamInit) {
        self.set(w.ids.previous, *previousCell);
        CellI& inputLhs = self.get(w.ids.lhs);
        previousCell    = currentCell;
        currentCell     = &inputLhs;
        self.set(w.ids.state, w.ids.stateLhs);
    } else if (&state == &w.ids.stateLhs) {
        CellI& inputRhs = self.get(w.ids.rhs);
        previousCell    = currentCell;
        currentCell     = &inputRhs;
        self.set(w.ids.state, w.ids.stateRhs);
    } else if (&state == &w.ids.stateRhs) {
        int lhs = static_cast<Number&>(self[w.ids.lhs][w.ids.value]).value();
        int rhs = static_cast<Number&>(self[w.ids.rhs][w.ids.value]).value();

        self.set(w.ids.value, lhs <= rhs ? w.boolean.true_ : w.boolean.false_);
        self.set(w.ids.state, w.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self.get(w.ids.previous);
    }
}

static void evalOpGreaterThan(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.ids.state)) {
        self.set(w.ids.state, w.ids.stateParamInit);
    }
    CellI& state = self.get(w.ids.state);
    if (&state == &w.ids.stateParamInit) {
        self.set(w.ids.previous, *previousCell);
        CellI& inputLhs = self.get(w.ids.lhs);
        previousCell    = currentCell;
        currentCell     = &inputLhs;
        self.set(w.ids.state, w.ids.stateLhs);
    } else if (&state == &w.ids.stateLhs) {
        CellI& inputRhs = self.get(w.ids.rhs);
        previousCell    = currentCell;
        currentCell     = &inputRhs;
        self.set(w.ids.state, w.ids.stateRhs);
    } else if (&state == &w.ids.stateRhs) {
        int lhs = static_cast<Number&>(self[w.ids.lhs][w.ids.value]).value();
        int rhs = static_cast<Number&>(self[w.ids.rhs][w.ids.value]).value();

        self.set(w.ids.value, lhs > rhs ? w.boolean.true_ : w.boolean.false_);
        self.set(w.ids.state, w.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self.get(w.ids.previous);
    }
}

static void evalOpGreaterThanOrEqual(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.ids.state)) {
        self.set(w.ids.state, w.ids.stateParamInit);
    }
    CellI& state = self.get(w.ids.state);
    if (&state == &w.ids.stateParamInit) {
        self.set(w.ids.previous, *previousCell);
        CellI& inputLhs = self.get(w.ids.lhs);
        previousCell    = currentCell;
        currentCell     = &inputLhs;
        self.set(w.ids.state, w.ids.stateLhs);
    } else if (&state == &w.ids.stateLhs) {
        CellI& inputRhs = self.get(w.ids.rhs);
        previousCell    = currentCell;
        currentCell     = &inputRhs;
        self.set(w.ids.state, w.ids.stateRhs);
    } else if (&state == &w.ids.stateRhs) {
        int lhs = static_cast<Number&>(self[w.ids.lhs][w.ids.value]).value();
        int rhs = static_cast<Number&>(self[w.ids.rhs][w.ids.value]).value();

        self.set(w.ids.value, lhs >= rhs ? w.boolean.true_ : w.boolean.false_);
        self.set(w.ids.state, w.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self.get(w.ids.previous);
    }
}
// logic
static void evalOpAnd(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.ids.state)) {
        self.set(w.ids.state, w.ids.stateParamInit);
    }
    CellI& state = self.get(w.ids.state);
    if (&state == &w.ids.stateParamInit) {
        self.set(w.ids.previous, *previousCell);
        CellI& inputLhs = self.get(w.ids.lhs);
        previousCell    = currentCell;
        currentCell     = &inputLhs;
        self.set(w.ids.state, w.ids.stateLhs);
    } else if (&state == &w.ids.stateLhs) {
        bool lhs = &self[w.ids.lhs][w.ids.value] == &w.boolean.true_;
        // shortcut, if the left hand side already false we don't evaluate the right hand side
        if (lhs == false) {
            self.set(w.ids.value, w.boolean.false_);
            self.set(w.ids.state, w.ids.stateParamInit);
            previousCell = currentCell;
            currentCell  = &self.get(w.ids.previous);
        } else {
            CellI& inputRhs = self.get(w.ids.rhs);
            previousCell    = currentCell;
            currentCell     = &inputRhs;
            self.set(w.ids.state, w.ids.stateRhs);
        }
    } else if (&state == &w.ids.stateRhs) {
        bool lhs = &self[w.ids.lhs][w.ids.value] == &w.boolean.true_;
        bool rhs = &self[w.ids.rhs][w.ids.value] == &w.boolean.true_;

        self.set(w.ids.value, w.toCellBool(lhs && rhs));
        self.set(w.ids.state, w.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self.get(w.ids.previous);
    }
}

static void evalOpOr(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.ids.state)) {
        self.set(w.ids.state, w.ids.stateParamInit);
    }
    CellI& state = self.get(w.ids.state);
    if (&state == &w.ids.stateParamInit) {
        self.set(w.ids.previous, *previousCell);
        CellI& inputLhs = self.get(w.ids.lhs);
        previousCell    = currentCell;
        currentCell     = &inputLhs;
        self.set(w.ids.state, w.ids.stateLhs);
    } else if (&state == &w.ids.stateLhs) {
        CellI& inputRhs = self.get(w.ids.rhs);
        previousCell    = currentCell;
        currentCell     = &inputRhs;
        self.set(w.ids.state, w.ids.stateRhs);
    } else if (&state == &w.ids.stateRhs) {
        bool lhs = &self[w.ids.lhs][w.ids.value] == &w.boolean.true_;
        bool rhs = &self[w.ids.rhs][w.ids.value] == &w.boolean.true_;

        self.set(w.ids.value, w.toCellBool(lhs || rhs));
        self.set(w.ids.state, w.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self.get(w.ids.previous);
    }
}

static void evalOpNot(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.ids.state)) {
        self.set(w.ids.state, w.ids.stateParamInit);
    }
    CellI& state = self[w.ids.state];
    if (&state == &w.ids.stateParamInit) {
        self.set(w.ids.previous, *previousCell);
        CellI& input = self[w.ids.input];
        previousCell = currentCell;
        currentCell  = &input;
        self.set(w.ids.state, w.ids.stateParam1);
    } else if (&state == &w.ids.stateParam1) {
        bool res = &self[w.ids.input][w.ids.value] == &w.boolean.true_;

        self.set(w.ids.value, w.toCellBool(!res));
        self.set(w.ids.state, w.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self.get(w.ids.previous);
    }
}

// math
static void evalOpAdd(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.ids.state)) {
        self.set(w.ids.state, w.ids.stateParamInit);
    }
    CellI& state = self.get(w.ids.state);
    if (&state == &w.ids.stateParamInit) {
        self.set(w.ids.previous, *previousCell);
        CellI& inputLhs = self.get(w.ids.lhs);
        previousCell    = currentCell;
        currentCell     = &inputLhs;
        self.set(w.ids.state, w.ids.stateLhs);
    } else if (&state == &w.ids.stateLhs) {
        CellI& inputRhs = self.get(w.ids.rhs);
        previousCell    = currentCell;
        currentCell     = &inputRhs;
        self.set(w.ids.state, w.ids.stateRhs);
    } else if (&state == &w.ids.stateRhs) {
        int lhs = static_cast<Number&>(self[w.ids.lhs][w.ids.value]).value();
        int rhs = static_cast<Number&>(self[w.ids.rhs][w.ids.value]).value();

        self.set(w.ids.value, w.pools.numbers.get(lhs + rhs));
        self.set(w.ids.state, w.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self.get(w.ids.previous);
    }
}

static void evalOpSubtract(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.ids.state)) {
        self.set(w.ids.state, w.ids.stateParamInit);
    }
    CellI& state = self.get(w.ids.state);
    if (&state == &w.ids.stateParamInit) {
        self.set(w.ids.previous, *previousCell);
        CellI& inputLhs = self.get(w.ids.lhs);
        previousCell    = currentCell;
        currentCell     = &inputLhs;
        self.set(w.ids.state, w.ids.stateLhs);
    } else if (&state == &w.ids.stateLhs) {
        CellI& inputRhs = self.get(w.ids.rhs);
        previousCell    = currentCell;
        currentCell     = &inputRhs;
        self.set(w.ids.state, w.ids.stateRhs);
    } else if (&state == &w.ids.stateRhs) {
        int lhs = static_cast<Number&>(self[w.ids.lhs][w.ids.value]).value();
        int rhs = static_cast<Number&>(self[w.ids.rhs][w.ids.value]).value();

        self.set(w.ids.value, w.pools.numbers.get(lhs - rhs));
        self.set(w.ids.state, w.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self.get(w.ids.previous);
    }
}

static void evalOpMultiply(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.ids.state)) {
        self.set(w.ids.state, w.ids.stateParamInit);
    }
    CellI& state = self.get(w.ids.state);
    if (&state == &w.ids.stateParamInit) {
        self.set(w.ids.previous, *previousCell);
        CellI& inputLhs = self.get(w.ids.lhs);
        previousCell    = currentCell;
        currentCell     = &inputLhs;
        self.set(w.ids.state, w.ids.stateLhs);
    } else if (&state == &w.ids.stateLhs) {
        CellI& inputRhs = self.get(w.ids.rhs);
        previousCell    = currentCell;
        currentCell     = &inputRhs;
        self.set(w.ids.state, w.ids.stateRhs);
    } else if (&state == &w.ids.stateRhs) {
        int lhs = static_cast<Number&>(self[w.ids.lhs][w.ids.value]).value();
        int rhs = static_cast<Number&>(self[w.ids.rhs][w.ids.value]).value();

        self.set(w.ids.value, w.pools.numbers.get(lhs * rhs));
        self.set(w.ids.state, w.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self.get(w.ids.previous);
    }
}

static void evalOpDivide(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    World& w = self.w;
    if (self.missing(w.ids.state)) {
        self.set(w.ids.state, w.ids.stateParamInit);
    }
    CellI& state = self.get(w.ids.state);
    if (&state == &w.ids.stateParamInit) {
        self.set(w.ids.previous, *previousCell);
        CellI& inputLhs = self.get(w.ids.lhs);
        previousCell    = currentCell;
        currentCell     = &inputLhs;
        self.set(w.ids.state, w.ids.stateLhs);
    } else if (&state == &w.ids.stateLhs) {
        CellI& inputRhs = self.get(w.ids.rhs);
        previousCell    = currentCell;
        currentCell     = &inputRhs;
        self.set(w.ids.state, w.ids.stateRhs);
    } else if (&state == &w.ids.stateRhs) {
        int lhs = static_cast<Number&>(self[w.ids.lhs][w.ids.value]).value();
        int rhs = static_cast<Number&>(self[w.ids.rhs][w.ids.value]).value();

        self.set(w.ids.value, w.pools.numbers.get(lhs / rhs));
        self.set(w.ids.state, w.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self.get(w.ids.previous);
    }
}

void Object::operator()()
{
    s_debugFunctionCalls = false; // Turn on / off debug here

    CellI* currentCell  = this;
    CellI* previousCell = &w.ids.emptyObject;
    w.ap.m_currentCell  = currentCell;
    w.ap.m_previousCell = previousCell;
    int tick = 0;
    //    std::cout << "Object::operator()()" << std::endl;

    do {
        CellI& self = *currentCell;
        CellI& type = self.struct_();
        if (!(&type == &w.std.op.Function || (type.has(w.ids.memberOf) && type[w.ids.memberOf][w.ids.index].has(w.std.op.Function)))) {
//            std::cout << self.struct_().label() << ":" << (self.has(w.ids.state) ? self[w.ids.state].label() : "(empty)") << std::endl;
        }
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
        } else if (&type == &w.std.op.Function || (type.has(w.ids.memberOf) && type[w.ids.memberOf][w.ids.index].has(w.std.op.Function))) {
            if (s_debugFunctionCalls && (self.missing(w.ids.state) || (&self[w.ids.state] == &w.ids.stateParamInit))) {
                printIndent();
                s_indent++;
                std::cout << self.label() << std::endl;
            }
            if (s_debugFunctionCalls && (self.has(w.ids.state) && (&self[w.ids.state] == &w.ids.stateParam1))) {
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
    } while (currentCell != &w.ids.emptyObject);
    w.ap.m_time.value(w.ap.m_time.value() + tick);
}

CellI& Object::operator[](CellI& key)
{
    auto findIt = m_slots.find(&key);
    if (findIt == m_slots.end())
        throw "No such key!";

    return *findIt->second;
}

void Object::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

void Object::destructor()
{
    getMethod(w.ids.destructor)();
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
    return struct_().has(w.ids.methods) && struct_()[w.ids.methods].has(w.ids.index) && struct_()[w.ids.methods][w.ids.index].has(key);
}

CellI& Object::getMethod(CellI& key)
{
    resetIndent();
    if (struct_().has(w.ids.methods)) {
        CellI& methodsIndex = struct_()[w.ids.methods][w.ids.index];
        if (methodsIndex.has(key)) {
            CellI& method = methodsIndex[key][w.ids.value];
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
    if (has(w.ids.methods)) {
        CellI& methodsIndex = (*this)[w.ids.methods][w.ids.index];
        if (methodsIndex.has(key)) {
            CellI& method = methodsIndex[key][w.ids.value];
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
    Object& stackListItem0 = *new Object(w, w.std.ListItem, "StackListItem0");
    Object& stackListItem1 = *new Object(w, w.std.ListItem, "StackListItem1");
    stackFrame.set(w.ids.method, method);
    stackFrame.set(w.ids.input, inputIndex);

    stackListItem0.set(w.ids.next, stackListItem1);
    stackListItem1.set(w.ids.value, stackFrame);
    stackListItem1.set(w.ids.previous, stackListItem0);
    method.set(w.ids.stack, stackListItem1);
}

void Object::clearStack(CellI& method)
{
    CellI* stackListItem1 = &method["stack"];
    CellI* stackListItem0 = &(*stackListItem1)["previous"];
    CellI* stackFrame     = &(*stackListItem1)["value"];
    CellI* inputIndex     = &(*stackFrame)["input"];
    if (method.struct_()["subTypes"]["index"].has("localVars")) {
        CellI* localVarsIndex = &(*stackFrame)["localVars"];
        delete localVarsIndex;
        // TODO
    }

    delete stackListItem1;
    delete stackListItem0;
    delete stackFrame;
    delete inputIndex;
}

void Object::initLocalVars(CellI& method)
{
    if (method.struct_()[w.ids.subTypes][w.ids.index].missing(w.ids.localVars)) {
        return;
    }
    CellI& localVarsType   = method.struct_()[w.ids.subTypes][w.ids.index][w.ids.localVars][w.ids.value];
    Object& localVarsIndex = *new Object(w, localVarsType, "LocalVarsIndex");
    CellI& stackFrame      = method[w.ids.stack][w.ids.value];
    stackFrame.set(w.ids.localVars, localVarsIndex);
    Visitor::visitList(localVarsType[w.ids.slots][w.ids.list], [this, &localVarsIndex](CellI& slot, int i, bool&) {
        auto& key        = slot[w.ids.key];
        Object& localVar = *new Object(w, w.std.op.Var, fmt::format("var {}", key.label()));
        localVar.set(w.ids.valueType, slot[w.ids.type]);
        localVarsIndex.set(key, localVar);
    });
}

CellI& Object::getFnValue(CellI& method)
{
    if (method.struct_()[w.ids.subTypes][w.ids.index].has(w.ids.returnType)) {
        return method[w.ids.value];
    }

    return w.ids.emptyObject;
}

void Object::setSelf(CellI& method)
{
    setFnParam(method, { w.ids.self, *this });
}

void Object::setFnParam(CellI& fn, Param param)
{
    if (fn.struct_()[w.ids.subTypes][w.ids.index][w.ids.parameters][w.ids.value].has(w.ids.slots)) {
        CellI& inputsIndex = fn.struct_()[w.ids.subTypes][w.ids.index][w.ids.parameters][w.ids.value][w.ids.slots][w.ids.index];
        if (inputsIndex.has(param.key)) {
            fn[w.ids.stack][w.ids.value][w.ids.input].set(param.key, param.value);
        } else {
            throw "No such param!";
        }
    }
}

#pragma endregion
#pragma region List::Item
// ============================================================================
List::Item::Item(World& w, List& list, CellI& value) :
    CellI(w),
    m_list(list),
    m_value(value)
{
}

bool List::Item::has(CellI& key)
{
    if (&key == &w.ids.struct_ || &key == &w.ids.value) {
        return true;
    }
    if (&key == &w.ids.previous && m_previous) {
        return true;
    }
    if (&key == &w.ids.next && m_next) {
        return true;
    }

    return false;
}

void List::Item::set(CellI& key, CellI& value)
{
    // Do nothing
}

void List::Item::erase(CellI& key)
{
    if (&key == &w.ids.next) {
        m_next = nullptr;
    }
    if (&key == &w.ids.previous) {
        m_previous = nullptr;
    }
    throw "No such key!";
}

void List::Item::operator()()
{
    // Do nothing
}

CellI& List::Item::operator[](CellI& key)
{
    if (&key == &w.ids.struct_) {
        if (!m_selfType) {
            m_selfType = &w.getStruct(w.templateId("std::ListItem", w.ids.valueType, m_list.m_valueType));
        }
        return *m_selfType;
    }
    if (&key == &w.ids.previous) {
        if (m_previous)
            return *m_previous;
        else
            throw "No such key!";
    }
    if (&key == &w.ids.next) {
        if (m_next)
            return *m_next;
        else
            throw "No such key!";
    }
    if (&key == &w.ids.value) {
        return m_value;
    }

    throw "No such key!";
}

void List::Item::accept(Visitor& visitor)
{
    visitor.visit(*this);
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
    if (&key == &w.ids.struct_ || &key == &w.ids.size) {
        return true;
    }
    if (&key == &w.ids.first && m_firstItem) {
        return true;
    }
    if (&key == &w.ids.last && m_lastItem) {
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
    if (&key == &w.ids.struct_) {
        if (!m_selfType) {
            m_selfType = &w.getStruct(w.templateId("std::List", w.ids.valueType, m_valueType));
        }
        return *m_selfType;
    }
    if (&key == &w.ids.first) {
        return *m_firstItem;
    }
    if (&key == &w.ids.last) {
        return *m_lastItem;
    }
    if (&key == &w.ids.size) {
        int size = (int)m_size;

        return w.pools.numbers.get(size);
    }

    throw "No such key!";
}

void List::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

List::Item* List::add(CellI& value)
{
    Item* item = new Item(w, *this, value);
    if (m_lastItem) {
        m_lastItem->m_next = item;
        item->m_previous   = m_lastItem;
    }
    m_lastItem = item;
    if (!m_firstItem) {
        m_firstItem = m_lastItem;
    }
    ++m_size;

    return item;
}


List::Item* List::addFront(CellI& value)
{
    Item* item = new Item(w, *this, value);
    if (m_firstItem) {
        m_firstItem->m_previous = item;
        item->m_next            = m_firstItem;
    }
    m_firstItem = item;
    if (!m_lastItem) {
        m_lastItem = item;
    }
    ++m_size;

    return item;
}

void List::remove(Item* item)
{
    if (item->m_previous) {
        item->m_previous->m_next = item->m_next;
    } else {
        m_firstItem = item->m_next;
    }
    if (item->m_next) {
        item->m_next->m_previous = item->m_previous;
    } else {
        m_lastItem = item->m_previous;
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
    for (CellI* node = m_firstItem; node;) {
        CellI& currentNode = *node;
        CellI* nextNode    = nullptr;
        if (currentNode.has(w.ids.next)) {
            nextNode = &currentNode[w.ids.next];
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
    m_slots(*new Map(w, w.std.Cell, w.std.Slot))
{
}

Struct::Struct(World& w, WithRecursiveType recursiveType, const std::string& label) :
    CellI(w, label),
    m_slots(*new Map(w, w.std.Cell, w.std.Slot, *this))
{
}

bool Struct::has(CellI& key)
{
    if (&key == &w.ids.struct_) {
        return true;
    }
    if (&key == &w.ids.name) {
        return true;
    }
    if (&key == &w.ids.slots) {
        return true;
    }
    if (&key == &w.ids.subTypes) {
        return m_subTypes;
    }
    if (&key == &w.ids.memberOf) {
        return m_memberOf;
    }
    if (&key == &w.ids.asts) {
        return m_asts;
    }
    if (&key == &w.ids.methods) {
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
    if (&key == &w.ids.struct_) {
        return w.std.Struct;
    }
    if (&key == &w.ids.name) {
        if (m_name) {
            return *m_name;
        } else {
            m_name = &w.name(label());
            return *m_name;
        }
    }
    if (&key == &w.ids.slots) {
        return m_slots;
    }
    if (&key == &w.ids.subTypes) {
        return *m_subTypes;
    }
    if (&key == &w.ids.memberOf) {
        return *m_memberOf;
    }
    if (&key == &w.ids.asts) {
        return *m_asts;
    }
    if (&key == &w.ids.methods) {
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

void Struct::accept(Visitor& visitor)
{
    visitor.visit(*this);
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
    if (&key == &w.ids.struct_) {
        return true;
    }
    if (m_slots.find(&key) != m_slots.end()) {
        return true;
    }

    return false;
}

void Index::set(CellI& key, CellI& value)
{
    if (&key == &w.ids.struct_) {
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
    if (&key == &w.ids.struct_) {
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
    if (&key == &w.ids.struct_) {
        throw "The type key can not be changed!";
    }
    m_slots[&key] = &value;
    if (m_recursiveType) {
        return;
    }
    Object& slot = *new Object(w, w.std.Slot);
    slot.set(w.ids.key, key);
    slot.set(w.ids.type, w.std.Slot);
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

void Index::accept(Visitor& visitor)
{
    visitor.visit(*this);
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
    if (&key == &w.ids.struct_) {
        return true;
    }
    if (&key == &w.ids.list) {
        return true;
    }
    if (&key == &w.ids.index) {
        return true;
    }
    if (&key == &w.ids.size) {
        return true;
    }

    return false;
}

void Map::set(CellI& key, CellI& value)
{
    throw "Not supported";
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
    if (&key == &w.ids.struct_) {
        if (!m_selfType) {
            m_selfType = &w.getStruct(w.templateId("std::Map", w.ids.keyType, m_keyType, w.ids.valueType, m_valueType));
        }
        return *m_selfType;
    }
    if (&key == &w.ids.list) {
        return m_list;
    }
    if (&key == &w.ids.index) {
        return m_index;
    }
    if (&key == &w.ids.size) {
        return w.pools.numbers.get(m_size);
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
        return m_index[key][w.ids.value];
    }
    throw "No such key!";
}

void Map::add(CellI& key, CellI& value)
{
    if (&key == &w.ids.struct_) {
        throw "id.type can not be stored in a map!";
    }
    if (m_index.has(key)) {
        throw "A value already registered with this key";
    }
    List::Item& item = *m_list.add(value);
    m_index.insert(key, item);
    ++m_size;
}

void Map::remove(CellI& key)
{
    if (!m_index.has(key)) {
        return;
    }
    List::Item* item = &static_cast<List::Item&>(m_index[key]);
    m_list.remove(item);
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

void Map::accept(Visitor& visitor)
{
    visitor.visit(*this);
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
    if (&key == &w.ids.struct_) {
        return true;
    }
    if (&key == &w.ids.list) {
        return true;
    }
    if (&key == &w.ids.index) {
        return true;
    }
    if (&key == &w.ids.size) {
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
    if (&key == &w.ids.struct_) {
        if (!m_selfType) {
            m_selfType = &w.getStruct(w.templateId("std::TrieMap", w.ids.keyType, m_keyType, w.ids.valueType, m_valueType));
        }
        return *m_selfType;
    }
    if (&key == &w.ids.list) {
        return m_list;
    }
    if (&key == &w.ids.rootNode) {
        return m_rootNode;
    }
    if (&key == &w.ids.size) {
        return w.pools.numbers.get(m_size);
    }

    throw "No such key!";
}

bool TrieMap::hasKey(CellI& key)
{
    CellI* currentNode = &m_rootNode;

    if (isA(key, w.std.List)) {
        throw "Key is not a list!";
    }

    Visitor::visitList(key, [this, &currentNode](CellI& keyItem, int i, bool& stop) {
        CellI* children = nullptr;
        if (currentNode->missing(w.ids.children)) {
            stop        = true;
            currentNode = nullptr;
            return;
        }
        Index& childrenIndex = static_cast<Index&>(currentNode->get(w.ids.children));
        if (childrenIndex.has(keyItem)) {
            children = &childrenIndex.get(keyItem);
        } else {
            stop        = true;
            currentNode = nullptr;
            return;
        }
        currentNode = children;
    });

    if (!currentNode || currentNode->missing(w.ids.data)) {
        return false;
    }

    return true;
}

CellI& TrieMap::getValue(CellI& key)
{
    if (isA(key, w.std.List)) {
        throw "Key is not a list!";
    }

    CellI* currentNode = &m_rootNode;

    Visitor::visitList(key, [this, &currentNode](CellI& keyItem, int i, bool& stop) {
        CellI* children = nullptr;
        if (currentNode->missing(w.ids.children)) {
            stop        = true;
            currentNode = nullptr;
            return;
        }
        Index& childrenIndex = static_cast<Index&>(currentNode->get(w.ids.children));
        if (childrenIndex.has(keyItem)) {
            children = &childrenIndex.get(keyItem);
        } else {
            stop        = true;
            currentNode = nullptr;
            return;
        }
        currentNode = children;
    });

    if (!currentNode || currentNode->missing(w.ids.data)) {
        throw "No such key!";
    }

    return (*currentNode)[w.ids.data][w.ids.value][w.ids.value];
}

CellI& TrieMap::getValueWithDataKey(CellI& key)
{
    CellI* currentNode = &m_rootNode;

    CellI& test = key.struct_();
    Visitor::visitList(key.struct_()[w.ids.slots][w.ids.list], [this, &currentNode, &key](CellI& slot, int i, bool& stop) {
        CellI& keyItem  = key[slot[w.ids.key]];
        CellI* children = nullptr;
        if (currentNode->missing(w.ids.children)) {
            stop        = true;
            currentNode = nullptr;
            return;
        }
        Index& childrenIndex = static_cast<Index&>(currentNode->get(w.ids.children));
        if (childrenIndex.has(keyItem)) {
            children = &childrenIndex.get(keyItem);
        } else {
            stop        = true;
            currentNode = nullptr;
            return;
        }
        currentNode = children;
    });

    if (!currentNode || currentNode->missing(w.ids.data)) {
        throw "No such key!";
    }

    return (*currentNode)[w.ids.data][w.ids.value][w.ids.value];
}

void TrieMap::addWithDataKey(CellI& key, CellI& value)
{
    CellI* currentNode = &m_rootNode;

    Visitor::visitList(key.struct_()[w.ids.slots][w.ids.list], [this, &currentNode, &key](CellI& slot, int i, bool& stop) {
        CellI& keyItem = key[slot[w.ids.key]];
        CellI* child = nullptr;
        if (currentNode->missing(w.ids.children)) {
            currentNode->set(w.ids.children, *new Index(w));
        }
        Index& childrenIndex = static_cast<Index&>(currentNode->get(w.ids.children));
        if (childrenIndex.has(keyItem)) {
            child = &childrenIndex.get(keyItem);
        } else {
            child = new Object(w, w.std.TrieMapNode);
            child->set(w.ids.parent, *currentNode);
            childrenIndex.insert(keyItem, *child);
        }
        currentNode = child;
    });

    List::Item& item = *m_list.add(w.std.kvPair(key, value));
    currentNode->set(w.ids.data, item);
    ++m_size;
}

void TrieMap::add(CellI& key, CellI& value)
{
    if (isA(key, w.std.List)) {
        throw "Key is not a list!";
    }

    CellI* currentNode = &m_rootNode;

    Visitor::visitList(key, [this, &currentNode](CellI& keyItem, int i, bool& stop) {
        CellI* child = nullptr;
        if (currentNode->missing(w.ids.children)) {
            currentNode->set(w.ids.children, *new Index(w));
        }
        Index& childrenIndex = static_cast<Index&>(currentNode->get(w.ids.children));
        if (childrenIndex.has(keyItem)) {
            child = &childrenIndex.get(keyItem);
        } else {
            child = new Object(w, w.std.TrieMapNode);
            child->set(w.ids.parent, *currentNode);
            childrenIndex.insert(keyItem, *child);
        }
        currentNode = child;
    });

    List::Item& item = *m_list.add(w.std.kvPair(key, value));
    currentNode->set(w.ids.data, item);
    ++m_size;
}

void TrieMap::remove(CellI& key)
{
    if (isA(key, w.std.List)) {
        throw "Key is not a list!";
    }

    if (&key[w.ids.size] == &w._0_) {
        return;
    }

    CellI* currentNode    = &m_rootNode;

    Visitor::visitList(key, [this, &currentNode](CellI& keyItem, int i, bool& stop) {
        CellI* children = nullptr;
        if (currentNode->missing(w.ids.children)) {
            stop        = true;
            currentNode = nullptr;
            return;
        }
        Index& childrenIndex = static_cast<Index&>(currentNode->get(w.ids.children));
        if (childrenIndex.has(keyItem)) {
            children = &childrenIndex.get(keyItem);
        } else {
            stop        = true;
            currentNode = nullptr;
            return;
        }
        currentNode = children;
    });

    if (!currentNode || currentNode->missing(w.ids.data)) {
        return;
    }
    List::Item* valueItem = &static_cast<List::Item&>((*currentNode)[w.ids.data]);
    currentNode->erase(w.ids.data);

    CellI* keyItemPtr = &key[w.ids.last];
    while (currentNode->has(w.ids.parent)) {
        CellI& keyItem = *keyItemPtr;
        CellI& parent = currentNode->get(w.ids.parent);
        CellI& child = *currentNode;
        if (child.missing(w.ids.data)) {
            if (child.missing(w.ids.children) || ( child.has(w.ids.children) && static_cast<Index&>(child[w.ids.children]).empty())) {
                delete currentNode;
                parent[w.ids.children].erase(keyItem[w.ids.value]);
            }
        }
        currentNode = &parent;
        if (keyItem.has(w.ids.previous)) {
            keyItemPtr = &keyItem[w.ids.previous];
        } else {
            break;
        }
    }
    if (!valueItem) {
        return;
    }
    m_list.remove(valueItem);
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

void TrieMap::accept(Visitor& visitor)
{
    visitor.visit(*this);
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
    if (&key == &w.ids.struct_ || &key == &w.ids.size) {
        return true;
    }
    if (&key == &w.ids.index) {
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
    if (&key == &w.ids.struct_) {
        if (!m_selfType) {
            m_selfType = &w.getStruct(w.templateId("std::Set", w.ids.valueType, m_valueType));
        }
        return *m_selfType;
    }
    if (&key == &w.ids.index) {
        return m_index;
    }
    if (&key == &w.ids.size) {
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
    return m_index["struct"]["slots"]["list"]["first"]["value"]["key"];
}

void Set::accept(Visitor& visitor)
{
    visitor.visit(*this);
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
    if (&key == &w.ids.struct_ || &key == &w.ids.value) {
        return true;
    }
    if (&key == &w.numbers.sign) {
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
    if (&key == &w.ids.struct_) {
        return w.std.Number;
    }

    if (&key == &w.numbers.sign && m_value != 0) {
        return m_value > 0 ? w.numbers.positive : w.numbers.negative;
    }

    if (&key == &w.ids.value) {
        if (m_digits.empty()) {
            calculateDigits();
            m_digitsList.reset(new List(w, m_digits));
        }

        return *m_digitsList;
    }

    throw "No such key!";
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

String::String(World& w, List& list, const std::string& str) :
    CellI(w),
    m_value(str),
    m_charactersListPtr(&list)
{
}

bool String::has(CellI& key)
{
    if (&key == &w.ids.struct_ || &key == &w.ids.value) {
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
    if (&key == &w.ids.struct_) {
        return w.std.String;
    } else if (&key == &w.ids.value) {
        if (m_characters.empty()) {
            calculateCharacters();
            if (m_charactersListPtr) {
                for (auto& character : m_characters) {
                    m_charactersListPtr->add(*character);
                }
                return *m_charactersListPtr;
            }
            m_charactersList.reset(new List(w, m_characters, label()));
        }

        return m_charactersListPtr ? *m_charactersListPtr  : *m_charactersList;
    } else {
        throw "No such key!";
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
        m_characters.push_back(&w.pools.chars.get(unicodeValue));
    }
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
    if (&key == &w.ids.struct_) {
        return true;
    }
    if (&key == &w.ids.cell || &key == &w.ids.previous) {
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
    if (&key == &w.ids.struct_) {
        return w.std.Color; // TODO
    }
    if (&key == &w.ids.cell) {
        return *m_currentCell;
    }
    if (&key == &w.ids.previous) {
        return *m_previousCell;
    }

    throw "No such key!";
}

void ActivationPointer::accept(Visitor& visitor)
{
//    visitor.visit(*this);
}

} // namespace hybrid

#if 0
void Visitor::visitList(CellI& list, std::function<void(CellI& value, int i, bool& stop)> visitFn)
{
    Brain& w = list.w;
    int i            = 0;

    if (list.missing(w.ids.first)) {
        return;
    }

    for (CellI* currentListItemPtr = &list[w.ids.first];;) {
        CellI& currentListItem = *currentListItemPtr;
        CellI& value           = currentListItem[w.ids.value];
        bool stop              = false;

        visitFn(value, i++, stop);
        if (stop || currentListItem.missing(w.ids.next)) {
            return;
        }

        currentListItemPtr = &currentListItem[w.ids.next];
    };
}
#else

void Visitor::visitList(CellI& list, std::function<void(CellI& value, int i, bool& stop)> visitFn)
{
    class ListIterator
    {
    public:
        ListIterator(CellI& list) :
            w(list.w), m_list(list), m_nodePtr(nullptr)
        {
        }

        bool isEmpty() {
            return m_list.missing(w.ids.first);
        }

        void setFirstValue()
        {
            m_nodePtr = &m_list[w.ids.first];
        }

        CellI& getCurrentValue()
        {
            return (*m_nodePtr)[w.ids.value];
        }

        bool hasNextValue()
        {
            return (*m_nodePtr).has(w.ids.next);
        }

        void setNextValue()
        {
            m_nodePtr = &(*m_nodePtr)[w.ids.next];
        }

    private:
        World& w;
        CellI& m_list;
        CellI* m_nodePtr;
    };

    ListIterator iterator(list);

    if (iterator.isEmpty()) {
        return;
    } else {
        iterator.setFirstValue();
    }

    int i = 0;
    do {
        bool stop = false;
        visitFn(iterator.getCurrentValue(), i++, stop);
        if (stop) {
            return;
        }
        if (iterator.hasNextValue()) {
            iterator.setNextValue();
        } else {
            return;
        }
    } while (true);
}
#endif

void Visitor::visitListInReverse(CellI& list, std::function<void(CellI& value, int i, bool& stop)> visitFn)
{
    World& w = list.w;
    int i            = 0;

    if (list.missing(w.ids.last)) {
        return;
    }

    for (CellI* currentListItemPtr = &list[w.ids.last];;) {
        CellI& currentListItem = *currentListItemPtr;
        CellI& value           = currentListItem[w.ids.value];
        bool stop              = false;

        visitFn(value, i++, stop);
        if (stop || currentListItem.missing(w.ids.previous)) {
            return;
        }

        currentListItemPtr = &currentListItem[w.ids.previous];
    }
}

bool tryVisitWith(CellI& cell, Visitor& visitor)
{
    World& w = cell.w;

    if (&cell.struct_() == &w.std.Number) {
        visitor.visit(static_cast<Number&>(cell));
        return true;
    }
    if (&cell.struct_() == &w.std.String) {
        visitor.visit(static_cast<String&>(cell));
        return true;
    }
#if 0 // TODO
    if (&cell.struct_() == &w.std.Color) {
        visitor.visit(static_cast<hybrid::Color&>(cell));
        return true;
    }
    if (&cell.struct_() == &w.std.Pixel) {
        visitor.visit(static_cast<hybrid::Pixel&>(cell));
        return true;
    }
    if (&cell.struct_() == &w.std.Grid) {
        visitor.visit(static_cast<hybrid::Picture&>(cell));
        return true;
    }
#endif

    return false;
}

} // namespace cells
} // namespace infocell
