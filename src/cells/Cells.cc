#include "Cells.h"
#include "Brain.h"

#include <utility>

#pragma warning(disable : 4996)
#include <utf8.h>


namespace infocell {
namespace cells {
using InitPhase = Brain::InitPhase;
#pragma region CellI
// ============================================================================
int CellI::s_constructed = 0;
int CellI::s_destructed  = 0;

CellI::CellI(Brain& kb) :
    kb(kb)
{
    s_constructed += 1;
}

CellI::CellI(Brain& kb, const std::string& label) :
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

bool CellI::has(const std::string& key)
{
    return has(kb.name(key));
}

void CellI::set(const std::string& key, CellI& value)
{
    set(kb.name(key), value);
}

void CellI::erase(const std::string& key)
{
    erase(kb.name(key));
}

CellI& CellI::operator[](const std::string& key)
{
    return (*this)[kb.name(key)];
}

bool CellI::missing(const std::string& key)
{
    return !has(kb.name(key));
}

CellI& CellI::get(const std::string& key)
{
    return (*this)[kb.name(key)];
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
    return (*this)[kb.ids.struct_];
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
    return &struct_() == &ptype || (has(kb.ids.memberOf) && (*this)[kb.ids.memberOf][kb.ids.index].has(ptype));
}

bool CellI::isA(CellI& cell, CellI& type) const
{
    return &cell == &type || (cell.has(kb.ids.memberOf) && cell[kb.ids.memberOf][kb.ids.index].has(type));
}

bool CellI::operator==(CellI& rhs)
{
    if (this == &rhs) {
        return true;
    }
    if (&struct_() != &rhs.struct_()) {
        return false;
    }
    CellI& slotList    = struct_()[kb.ids.slots][kb.ids.list];
    CellI* slotItemPtr = slotList.has(kb.ids.first) ? &slotList[kb.ids.first] : nullptr;
    while (slotItemPtr) {
        CellI& slotItem = *slotItemPtr;
        CellI& slot     = slotItem[kb.ids.value];
        CellI& key      = slot[kb.ids.key];

        bool hasLeftSlot = has(key);
        if (hasLeftSlot != rhs.has(key)) {
            return false;
        }
        if (hasLeftSlot && (&(*this)[key] != &rhs[key])) {
            return false;
        }

        slotItemPtr = slotItem.has(kb.ids.next) ? &slotItem[kb.ids.next] : nullptr;
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
    key(value.kb.name(key)), value(value) { }

int Object::s_indent = 0;
// ============================================================================
Object::Object(Brain& kb, CellI& type, const std::string& label) :
    CellI(kb, label),
    m_type(type)
{
    m_slots[&kb.ids.struct_] = &type;
}

Object::Object(Brain& kb, CellI& type, CellI& constructor, const std::string& label) :
    CellI(kb, label),
    m_type(type)
{
    m_slots[&kb.ids.struct_] = &type;
    getMethod(constructor)();
}

Object::Object(Brain& kb, CellI& type, CellI& constructor, Param param1, const std::string& label) :
    CellI(kb, label),
    m_type(type)
{
    m_slots[&kb.ids.struct_] = &type;

    CellI& method = getMethod(constructor);
    setFnParam(method, param1);
    method();
}

Object::Object(Brain& kb, CellI& type, CellI& constructor, Param param1, Param param2, const std::string& label) :
    CellI(kb, label),
    m_type(type)
{
    m_slots[&kb.ids.struct_] = &type;

    CellI& method = getMethod(constructor);
    setFnParam(method, param1);
    setFnParam(method, param2);
    method();
}

Object::Object(Brain& kb, CellI& type, CellI& constructor, Param param1, Param param2, Param param3, const std::string& label) :
    CellI(kb, label),
    m_type(type)
{
    m_slots[&kb.ids.struct_] = &type;

    CellI& method = getMethod(constructor);
    setFnParam(method, param1);
    setFnParam(method, param2);
    setFnParam(method, param3);
    method();
}

bool Object::s_debugFunctionCalls = false;

Object::Object(Brain& kb, CellI& type, CellI& constructor, Param param1, Param param2, Param param3, Param param4, const std::string& label) :
    CellI(kb, label),
    m_type(type)
{
    m_slots[&kb.ids.struct_] = &type;

    CellI& method = getMethod(constructor);
    setFnParam(method, param1);
    setFnParam(method, param2);
    setFnParam(method, param3);
    setFnParam(method, param4);
    method();
}

Object::~Object()
{
    if (kb.initPhase() == InitPhase::Init || kb.initPhase() == InitPhase::DestructBegin) {
        return;
    }
    if (!hasMethod(kb.ids.destructor)) {
        return;
    }
    destructor();
}

bool Object::has(CellI& key)
{
    if (&key == &kb.ids.struct_)
        return true;

    return m_slots.find(&key) != m_slots.end();
}

void Object::set(CellI& key, CellI& value)
{
    if ((&key == &kb.ids.struct_) && !((&struct_() == &kb.std.Index))) {
        throw "Type change not allowed.";
    }
    if ((&key == &kb.ids.struct_) && (&struct_() == &kb.std.Index)) {
        std::cout << "";
    }

    if (kb.initPhase() == InitPhase::Init) {
        m_slots[&key] = &value;
        return;
    }
    auto is = [this](CellI& rhsType) -> bool { return &struct_() == &rhsType || (struct_().has(kb.ids.memberOf) && struct_()[kb.ids.memberOf][kb.ids.index].has(rhsType)); };
    if (is(kb.std.Index) || struct_()[kb.ids.slots][kb.ids.index].has(key)) {
        m_slots[&key] = &value;
    } else {
        throw "The type doesn't contains this key.";
    }
}

void Object::erase(CellI& key)
{
    if (&key == &kb.ids.struct_) {
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
    Brain& kb = self.kb;
    if (self.missing(kb.ids.state)) {
        self.set(kb.ids.state, kb.ids.stateParamInit);
    }
    CellI& state = self[kb.ids.state];
    if (&state == &kb.ids.stateParamInit) {
        self.set(kb.ids.previous, *previousCell);
        CellI& inputCell = self[kb.ids.cell];
        previousCell     = currentCell;
        currentCell      = &inputCell;
        self.set(kb.ids.state, kb.ids.stateParam1);
    } else if (&state == &kb.ids.stateParam1) {
        CellI& inputRole = self[kb.ids.key];
        previousCell     = currentCell;
        currentCell      = &inputRole;
        self.set(kb.ids.state, kb.ids.stateParam2);
    } else if (&state == &kb.ids.stateParam2) {
        CellI& cell = self[kb.ids.cell][kb.ids.value];
        CellI& key = self[kb.ids.key][kb.ids.value];

        self.set(kb.ids.value, cell[key]);
        self.set(kb.ids.state, kb.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self[kb.ids.previous];
    }
}

static void evalOpSet(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    Brain& kb = self.kb;
    if (self.missing(kb.ids.state)) {
        self.set(kb.ids.state, kb.ids.stateParamInit);
    }
    CellI& state = self[kb.ids.state];
    if (&state == &kb.ids.stateParamInit) {
        self.set(kb.ids.previous, *previousCell);
        CellI& inputCell = self[kb.ids.cell];
        previousCell     = currentCell;
        currentCell      = &inputCell;
        self.set(kb.ids.state, kb.ids.stateParam1);
    } else if (&state == &kb.ids.stateParam1) {
        CellI& inputRole = self[kb.ids.key];
        previousCell     = currentCell;
        currentCell      = &inputRole;
        self.set(kb.ids.state, kb.ids.stateParam2);
    } else if (&state == &kb.ids.stateParam2) {
        CellI& inputValue = self[kb.ids.value];
        previousCell      = currentCell;
        currentCell       = &inputValue;
        self.set(kb.ids.state, kb.ids.stateParam3);
    } else if (&state == &kb.ids.stateParam3) {
        CellI& cell  = self[kb.ids.cell][kb.ids.value];
        CellI& key  = self[kb.ids.key][kb.ids.value];
        CellI& value = self[kb.ids.value][kb.ids.value];

        cell.set(key, value);
        self.set(kb.ids.state, kb.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self[kb.ids.previous];
    }
}

static void evalOpHas(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    Brain& kb = self.kb;
    if (self.missing(kb.ids.state)) {
        self.set(kb.ids.state, kb.ids.stateParamInit);
    }
    CellI& state = self[kb.ids.state];
    if (&state == &kb.ids.stateParamInit) {
        self.set(kb.ids.previous, *previousCell);
        CellI& inputCell = self[kb.ids.cell];
        previousCell     = currentCell;
        currentCell      = &inputCell;
        self.set(kb.ids.state, kb.ids.stateParam1);
    } else if (&state == &kb.ids.stateParam1) {
        CellI& inputRole = self[kb.ids.key];
        previousCell     = currentCell;
        currentCell      = &inputRole;
        self.set(kb.ids.state, kb.ids.stateParam2);
    } else if (&state == &kb.ids.stateParam2) {
        CellI& cell = self[kb.ids.cell][kb.ids.value];
        CellI& key = self[kb.ids.key][kb.ids.value];

        self.set(kb.ids.value, kb.toKbBool(cell.has(key)));
        self.set(kb.ids.state, kb.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self[kb.ids.previous];
    }
}

static void evalOpMissing(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    Brain& kb = self.kb;
    if (self.missing(kb.ids.state)) {
        self.set(kb.ids.state, kb.ids.stateParamInit);
    }
    CellI& state = self[kb.ids.state];
    if (&state == &kb.ids.stateParamInit) {
        self.set(kb.ids.previous, *previousCell);
        CellI& inputCell = self[kb.ids.cell];
        previousCell     = currentCell;
        currentCell      = &inputCell;
        self.set(kb.ids.state, kb.ids.stateParam1);
    } else if (&state == &kb.ids.stateParam1) {
        CellI& inputRole = self[kb.ids.key];
        previousCell     = currentCell;
        currentCell      = &inputRole;
        self.set(kb.ids.state, kb.ids.stateParam2);
    } else if (&state == &kb.ids.stateParam2) {
        CellI& cell = self[kb.ids.cell][kb.ids.value];
        CellI& key = self[kb.ids.key][kb.ids.value];

        self.set(kb.ids.value, kb.toKbBool(cell.missing(key)));
        self.set(kb.ids.state, kb.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self[kb.ids.previous];
    }
}

static void evalOpErase(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    Brain& kb = self.kb;
    if (self.missing(kb.ids.state)) {
        self.set(kb.ids.state, kb.ids.stateParamInit);
    }
    CellI& state = self[kb.ids.state];
    if (&state == &kb.ids.stateParamInit) {
        self.set(kb.ids.previous, *previousCell);
        CellI& inputCell = self[kb.ids.cell];
        previousCell     = currentCell;
        currentCell      = &inputCell;
        self.set(kb.ids.state, kb.ids.stateParam1);
    } else if (&state == &kb.ids.stateParam1) {
        CellI& inputRole = self[kb.ids.key];
        previousCell     = currentCell;
        currentCell      = &inputRole;
        self.set(kb.ids.state, kb.ids.stateParam2);
    } else if (&state == &kb.ids.stateParam2) {
        CellI& cell = self[kb.ids.cell][kb.ids.value];
        CellI& key = self[kb.ids.key][kb.ids.value];

        cell.erase(key);
        self.set(kb.ids.state, kb.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self[kb.ids.previous];
    }
}

static void evalOpNew(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    Brain& kb = self.kb;
    if (self.missing(kb.ids.state)) {
        self.set(kb.ids.state, kb.ids.stateParamInit);
    }
    CellI& state = self[kb.ids.state];
    if (&state == &kb.ids.stateParamInit) {
        self.set(kb.ids.previous, *previousCell);
        CellI& inputObjectType = self[kb.ids.objectType];
        previousCell           = currentCell;
        currentCell            = &inputObjectType;
        self.set(kb.ids.state, kb.ids.stateParam1);
    } else if (&state == &kb.ids.stateParam1) {
        CellI& objectType = self[kb.ids.objectType][kb.ids.value];

        if (&objectType == &kb.std.Number) {
            self.set(kb.ids.value, *new Number(kb));
        } else if (&objectType == &kb.std.String) {
            self.set(kb.ids.value, *new String(kb));
        } else {
            self.set(kb.ids.value, *new Object(kb, objectType));
        }
        self.set(kb.ids.state, kb.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self[kb.ids.previous];
    }
}

static void evalOpDelete(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    Brain& kb = self.kb;
    if (self.missing(kb.ids.state)) {
        self.set(kb.ids.state, kb.ids.stateParamInit);
    }
    CellI& state = self[kb.ids.state];
    if (&state == &kb.ids.stateParamInit) {
        self.set(kb.ids.previous, *previousCell);
        CellI& input = self[kb.ids.input];
        previousCell = currentCell;
        currentCell  = &input;
        self.set(kb.ids.state, kb.ids.stateParam1);
    } else if (&state == &kb.ids.stateParam1) {
        CellI& input = self[kb.ids.input];
        CellI* cell  = &input[kb.ids.value];

        delete cell;
        input.erase(kb.ids.value);
        self.set(kb.ids.state, kb.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self[kb.ids.previous];
    }
}

// code running
static void evalOpActivate(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    Brain& kb = self.kb;
    if (self.missing(kb.ids.state)) {
        self.set(kb.ids.state, kb.ids.stateParamInit);
    }
    CellI& state = self[kb.ids.state];
    if (&state == &kb.ids.stateParamInit) {
        self.set(kb.ids.previous, *previousCell);
        CellI& inputCell = self[kb.ids.cell];

        previousCell = currentCell;
        currentCell  = &inputCell;
        self.set(kb.ids.state, kb.ids.stateParam1);
    } else if (&state == &kb.ids.stateParam1) {
        previousCell     = currentCell;
        CellI& inputCell = self[kb.ids.cell];
        CellI* status    = &kb.ids.process;

        if (self.has(kb.ids.parent)) {
            CellI& parent = self[kb.ids.parent];
            if (&inputCell.struct_() == &kb.std.op.Return || (inputCell.has(kb.ids.status) && (&inputCell[kb.ids.status] == &kb.ids.return_))) {
                parent.set(kb.ids.status, kb.ids.return_);
                status = &kb.ids.return_;
            } else if (&parent[kb.ids.status] == &kb.ids.continue_ || &parent[kb.ids.status] == &kb.ids.break_) {
                status = &parent[kb.ids.status];
            } else if (self.has(kb.ids.status)) {
                if (&self[kb.ids.status] == &kb.ids.return_ || &self[kb.ids.status] == &kb.ids.continue_ || &self[kb.ids.status] == &kb.ids.break_) {
                    parent.set(kb.ids.status, self[kb.ids.status]);
                    status = &self[kb.ids.status];
                }
            }
        }
        if (status == &kb.ids.process && self.has(kb.ids.next)) {
            CellI& nextCell = self[kb.ids.next];
            currentCell     = &nextCell;
        } else {
            if (self.has(kb.ids.parent)) {
                currentCell = &self[kb.ids.parent];
            } else {
                currentCell = &self[kb.ids.previous];
            }
        }
        self.set(kb.ids.state, kb.ids.stateParamInit);
    }
}

static void saveOpState(List& opStates, CellI& op)
{
    Brain& kb = op.kb;

    if (&op == &kb.ids.emptyObject) {
        return;
    }
    CellI& type      = op.struct_();
    Object& opState  = *new Object(kb, kb.std.OpState);

    opState.set(kb.ids.op, op);
    opState.set(kb.ids.state, kb.ids.state);
    opState.set(kb.ids.value, op[kb.ids.state]);
    opStates.add(opState);
    if (op.has(kb.ids.status)) {
        Object& opState = *new Object(kb, kb.std.OpState);
        opState.set(kb.ids.op, op);
        opState.set(kb.ids.state, kb.ids.status);
        opState.set(kb.ids.value, op[kb.ids.status]);
        opStates.add(opState);
    }
    if (op.has(kb.ids.previous)) {
        Object& opState = *new Object(kb, kb.std.OpState);
        opState.set(kb.ids.op, op);
        opState.set(kb.ids.state, kb.ids.previous);
        opState.set(kb.ids.value, op[kb.ids.previous]);
        opStates.add(opState);
    }
    if (&type == &kb.std.op.Call) {
        if (op[kb.ids.cell].has(kb.ids.value)) {
            Object& opState = *new Object(kb, kb.std.OpState);
            opState.set(kb.ids.op, op);
            opState.set(kb.ids.state, kb.ids.cell);
            opState.set(kb.ids.value, op[kb.ids.cell][kb.ids.value]);
            opStates.add(opState);
        }
        if (op[kb.ids.method].has(kb.ids.value)) {
            Object& opState = *new Object(kb, kb.std.OpState);
            opState.set(kb.ids.op, op);
            opState.set(kb.ids.state, kb.ids.method);
            opState.set(kb.ids.value, op[kb.ids.method][kb.ids.value]);
            opStates.add(opState);
        }
        if (op[kb.ids.stack].has(kb.ids.value)) {
            Object& opState = *new Object(kb, kb.std.OpState);
            opState.set(kb.ids.op, op);
            opState.set(kb.ids.state, kb.ids.stack);
            opState.set(kb.ids.value, op[kb.ids.stack][kb.ids.value]);
            opStates.add(opState);
        }
    }
    if (&type == &kb.std.op.Set || &type == &kb.std.op.Get) {
        if (op[kb.ids.cell].has(kb.ids.value)) {
            Object& opState = *new Object(kb, kb.std.OpState);
            opState.set(kb.ids.op, op);
            opState.set(kb.ids.state, kb.ids.cell);
            opState.set(kb.ids.value, op[kb.ids.cell][kb.ids.value]);
            opStates.add(opState);
        }
        if (op[kb.ids.key].has(kb.ids.value)) {
            Object& opState = *new Object(kb, kb.std.OpState);
            opState.set(kb.ids.op, op);
            opState.set(kb.ids.state, kb.ids.key);
            opState.set(kb.ids.value, op[kb.ids.key][kb.ids.value]);
            opStates.add(opState);
        }
    }

    if (&type == &kb.std.op.Same ||
        &type == &kb.std.op.NotSame ||
        &type == &kb.std.op.Equal ||
        &type == &kb.std.op.NotEqual ||
        &type == &kb.std.op.LessThan ||
        &type == &kb.std.op.LessThanOrEqual ||
        &type == &kb.std.op.GreaterThan ||
        &type == &kb.std.op.GreaterThanOrEqual ||
        &type == &kb.std.op.And ||
        &type == &kb.std.op.Or ||
        &type == &kb.std.op.Add ||
        &type == &kb.std.op.Subtract ||
        &type == &kb.std.op.Multiply ||
        &type == &kb.std.op.Divide) {
        Object& opState = *new Object(kb, kb.std.OpState);
        opState.set(kb.ids.op, op);
        opState.set(kb.ids.state, kb.ids.lhs);
        opState.set(kb.ids.value, op[kb.ids.lhs][kb.ids.value]);
        opStates.add(opState);
    }
    op.set(kb.ids.state, kb.ids.stateParamInit);
}

static void loadOpState(CellI& opState)
{
    Brain& kb = opState.kb;
    CellI& op        = opState[kb.ids.op];
    CellI& type      = op.struct_();
    CellI& state     = opState[kb.ids.state];
    CellI& value     = opState[kb.ids.value];

    if ((&type == &kb.std.op.Set || &type == &kb.std.op.Get) && ((&state == &kb.ids.cell) || (&state == &kb.ids.key))) {
        op[state].set(kb.ids.value, value);
    } else if ((&type == &kb.std.op.Call) && ((&state == &kb.ids.cell) || (&state == &kb.ids.method) || (&state == &kb.ids.stack))) {
        op[state].set(kb.ids.value, value);
    } else if ((&type == &kb.std.op.Same ||
        &type == &kb.std.op.NotSame ||
        &type == &kb.std.op.Equal ||
        &type == &kb.std.op.NotEqual ||
        &type == &kb.std.op.LessThan ||
        &type == &kb.std.op.LessThanOrEqual ||
        &type == &kb.std.op.GreaterThan ||
        &type == &kb.std.op.GreaterThanOrEqual ||
        &type == &kb.std.op.And ||
        &type == &kb.std.op.Or ||
        &type == &kb.std.op.Add ||
        &type == &kb.std.op.Subtract ||
        &type == &kb.std.op.Multiply ||
        &type == &kb.std.op.Divide) && (&state == &kb.ids.lhs)) {
        op[state].set(kb.ids.value, value);
    } else {
        op.set(state, value);
    }
//    std::cout << "         set (" << &op << ")" << op.struct_().label() << "[" << state.label() << ":" << value.label() << "]" << std::endl;
}

static void evalOpCall(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    Brain& kb = self.kb;
    if (self.missing(kb.ids.state)) {
        self.set(kb.ids.state, kb.ids.stateParamInit);
    }
//    std::cout << "evalOpCall self: " << &self << ", state: " << self[kb.ids.state].label() << std::endl;
    CellI& state = self[kb.ids.state];
    if (&state == &kb.ids.stateParamInit) {
        self.set(kb.ids.previous, *previousCell);
        CellI& inputCell = self[kb.ids.cell];
        previousCell     = currentCell;
        currentCell      = &inputCell;
        self.set(kb.ids.state, kb.ids.stateParam1);
    } else if (&state == &kb.ids.stateParam1) {
        CellI& inputMethod = self[kb.ids.method];
        previousCell       = currentCell;
        currentCell        = &inputMethod;
        self.set(kb.ids.state, kb.ids.stateParam2);
    } else if (&state == &kb.ids.stateParam2) {
        CellI& inputStack = self[kb.ids.stack];
        previousCell      = currentCell;
        currentCell       = &inputStack;
        if (self.has(kb.ids.parameters)) {
            self.set(kb.ids.state, kb.ids.stateParamEval);
        } else {
            self.set(kb.ids.state, kb.ids.stateParam3);
        }
    } else if (&state == &kb.ids.stateParamEval) {
        CellI* paramNodePtr = nullptr;
        if (self.missing(kb.ids.currentParam)) {
            CellI& paramList = self[kb.ids.parameters];
            paramNodePtr     = &paramList[kb.ids.first];
        } else {
            CellI& paramNode = self[kb.ids.currentParam];
            if (paramNode.has(kb.ids.next)) {
                paramNodePtr = &paramNode[kb.ids.next];
            } else {
                self.erase(kb.ids.currentParam);
            }
        }
        if (paramNodePtr) {
            CellI& param = (*paramNodePtr)[kb.ids.value][kb.ids.type];
            self.set(kb.ids.currentParam, *paramNodePtr);
            previousCell = currentCell;
            currentCell  = &param;
        } else {
            self.set(kb.ids.state, kb.ids.stateParam3);
        }
    } else if (&state == &kb.ids.stateParam3) {
        CellI& cell       = self[kb.ids.cell][kb.ids.value];
        CellI& methodName = self[kb.ids.method][kb.ids.value];
        CellI& stack      = self[kb.ids.stack][kb.ids.value];

        CellI* methodPtr = nullptr;
        if (&self[kb.ids.ast].struct_() == &kb.std.ast.Call) {
            methodPtr = &cell[kb.ids.struct_][kb.ids.methods];
        } else {
            methodPtr = &cell[kb.ids.methods];
        }
        CellI& method = (*methodPtr)[kb.ids.index][methodName][kb.ids.value];

        CellI& stackFrame = *new Object(kb, kb.std.StackFrame);
        stackFrame.set(kb.ids.method, method);

        CellI& inputIndex = *new Object(kb, kb.std.Index);
        inputIndex.set(kb.ids.self, cell);
        if (self.has(kb.ids.parameters)) {
            Visitor::visitList(self[kb.ids.parameters], [&self, &kb, &inputIndex](CellI& parameter, int, bool& stop) {
                inputIndex.set(parameter[kb.ids.key], parameter[kb.ids.type][kb.ids.value]);
//                static_cast<Object&>(self).printIndent();
//                std::cout << parameter[kb.ids.key].label() << ":" << parameter[kb.ids.type][kb.ids.value].label() << std::endl;
            });
        }
        stackFrame.set(kb.ids.input, inputIndex);

        if (method.struct_()[kb.ids.subTypes][kb.ids.index].has(kb.ids.localVars)) {
            CellI& localVarsList  = method.struct_()[kb.ids.subTypes][kb.ids.index][kb.ids.localVars][kb.ids.value][kb.ids.slots][kb.ids.list];
            Index& localVarsIndex = *new Index(kb /*, method.struct_()[kb.ids.subTypes][kb.ids.index][kb.ids.localVars][kb.ids.value] */);
            if (method.struct_()[kb.ids.subTypes][kb.ids.index].has(kb.ids.localVars)) {
                Visitor::visitList(localVarsList, [&self, &kb, &localVarsIndex](CellI& slot, int, bool& stop) {
                    localVarsIndex.set(slot[kb.ids.key], *new Object(kb, kb.std.op.Var));
                });
                stackFrame.set(kb.ids.localVars, localVarsIndex);
            }
        }
        CellI& newStackListItem = *new Object(kb, kb.std.ListItem);
        newStackListItem.set(kb.ids.value, stackFrame);
        newStackListItem.set(kb.ids.previous, stack);
        stack.set(kb.ids.next, newStackListItem);

        CellI& previousMethod = stack[kb.ids.value][kb.ids.method];
//        std::cout << "previous method " << previousMethod.label() << std::endl;
//        std::cout << "next     method " << method.label() << std::endl;

        method.set(kb.ids.stack, newStackListItem);
        self.set(kb.ids.state, kb.ids.stateStackCall);
        previousMethod.set(kb.ids.lastOp, self);

        if (method.has(kb.ids.state) && (&method[kb.ids.state] != &kb.ids.stateParamInit)) {
//            std::cout << "recursive call for " << method.struct_().label() << std::endl;
            List& cellPath = *new List(kb, kb.std.op.Base);
            CellI& lastOp  = method[kb.ids.lastOp];
            for (CellI* currentOp = &lastOp; currentOp != &method; currentOp = (*currentOp).has(kb.ids.parent) ? &(*currentOp)[kb.ids.parent] : &(*currentOp)[kb.ids.previous]) {
                CellI& op = *currentOp;
//                std::cout << "         [" << op.struct_().label() << ":" << op[kb.ids.state].label() << "]" << std::endl;
                saveOpState(cellPath, op);
            }
            saveOpState(cellPath, method);
            method.set(kb.ids.state, kb.ids.stateParamInit);
            stackFrame.set(kb.ids.ops, cellPath);
//            std::cout << std::endl;
        }
        previousCell = currentCell;
        currentCell  = &method;
    } else if (&state == &kb.ids.stateStackCall) {
        CellI& cell       = self[kb.ids.cell][kb.ids.value];
        CellI& methodName = self[kb.ids.method][kb.ids.value];

        CellI* methodPtr = nullptr;
        if (&self[kb.ids.ast].struct_() == &kb.std.ast.Call) {
            methodPtr = &cell[kb.ids.struct_][kb.ids.methods];
        } else {
            methodPtr = &cell[kb.ids.methods];
        }
        // TODO: cache the method obj
        CellI& method = (*methodPtr)[kb.ids.index][methodName][kb.ids.value];
        if (method.has(kb.ids.value)) {
            self.set(kb.ids.value, method[kb.ids.value]);
//            static_cast<Object&>(self).printIndent();
//            std::cout << "return " << method[kb.ids.value].label() << std::endl;
        }

        previousCell = currentCell;
        currentCell  = &self[kb.ids.previous];
        self.set(kb.ids.state, kb.ids.stateParamInit);
    }
}

static void evalOpFunction(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    Brain& kb = self.kb;
    if (self.missing(kb.ids.state)) {
        self.set(kb.ids.state, kb.ids.stateParamInit);
    }
    CellI& state = self[kb.ids.state];
    if (&state == &kb.ids.stateParamInit) {
        self.set(kb.ids.previous, *previousCell);
        CellI& op = self[kb.ids.op];
        if (op.has(kb.ids.state) && (&op[kb.ids.state] != &kb.ids.stateParamInit)) {
            throw "Error: function contains non-clean op.";
        }
        previousCell = currentCell;
        currentCell  = &op;
        self.set(kb.ids.state, kb.ids.stateParam1);
    } else if (&state == &kb.ids.stateParam1) {
        previousCell = currentCell;
        currentCell  = &self[kb.ids.previous];

        CellI& stackNode = self[kb.ids.stack];
        CellI& stackFrame = stackNode[kb.ids.value];
        static_cast<Object&>(self).printIndent();
//        std::cout << "return " << std::endl;
//        std::cout << "return " << self.label() << std::endl;
        if (stackFrame.has(kb.ids.ops)) {
            Visitor::visitList(stackFrame[kb.ids.ops], [&kb](CellI& opState, int, bool& stop) {
                loadOpState(opState);
                delete &opState;
            });
            static_cast<List&>(stackFrame[kb.ids.ops]).clear();
            delete &stackFrame[kb.ids.ops];
            stackFrame.erase(kb.ids.ops);
        } else {
            self.set(kb.ids.state, kb.ids.stateParamInit);
        }
        //
        if (self[kb.ids.stack][kb.ids.previous].has(kb.ids.value)) {
            CellI& inputIndex         = stackFrame[kb.ids.input];
            CellI& previousStackNode  = self[kb.ids.stack][kb.ids.previous];
            CellI& previousStackFrame = previousStackNode[kb.ids.value];
            CellI& previousMethod     = previousStackFrame[kb.ids.method];
            previousMethod.set(kb.ids.stack, previousStackNode);
            previousStackNode.erase(kb.ids.next);
            delete &inputIndex;
            if (stackFrame.has(kb.ids.localVars)) {
                CellI& localVarsList  = self.struct_()[kb.ids.subTypes][kb.ids.index][kb.ids.localVars][kb.ids.value][kb.ids.slots][kb.ids.list];
                CellI& localVarsIndex = stackFrame[kb.ids.localVars];
                Visitor::visitList(localVarsList, [&self, &kb, &localVarsIndex](CellI& slot, int, bool& stop) {
                    delete &localVarsIndex[slot[kb.ids.key]];
                });
                delete &localVarsIndex;
            }
            delete &stackFrame;
            delete &stackNode;
            if (currentCell == &kb.ids.emptyObject) {
                std::cout << "";
            }
        } else {
            self.set(kb.ids.state, kb.ids.stateParamInit);
            currentCell = &kb.ids.emptyObject;
        }
    }
}

static void evalOpIf(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    Brain& kb = self.kb;
    if (self.missing(kb.ids.state)) {
        self.set(kb.ids.state, kb.ids.stateParamInit);
    }
    CellI& state = self[kb.ids.state];
    if (&state == &kb.ids.stateParamInit) {
        self.set(kb.ids.previous, *previousCell);
        CellI& inputCondition = self[kb.ids.condition];

        previousCell = currentCell;
        currentCell  = &inputCondition;
        self.set(kb.ids.state, kb.ids.stateParam1);
    } else if (&state == &kb.ids.stateParam1) {
        self.set(kb.ids.status, kb.ids.process);
        CellI* branchPtr = nullptr;
        bool condition   = &self[kb.ids.condition][kb.ids.value] == &kb.boolean.true_;
        if (condition) {
            branchPtr = &self[kb.ids.then];
            self.set(kb.ids.state, kb.ids.stateThen);
        } else if (self.has(kb.ids.else_)) {
            branchPtr = &self[kb.ids.else_];
            self.set(kb.ids.state, kb.ids.stateElse);
        }
        previousCell = currentCell;
        if (branchPtr) {
            currentCell = branchPtr;
        } else {
            currentCell = &self[kb.ids.previous];
            self.set(kb.ids.state, kb.ids.stateParamInit);
        }
    }
 else if (&state == &kb.ids.stateThen || &state == &kb.ids.stateElse) {
     CellI& branch = &state == &kb.ids.stateThen ? self[kb.ids.then] : self[kb.ids.else_];
     if (&branch.struct_() == &kb.std.op.Return) {
         self.set(kb.ids.status, kb.ids.return_);
     }
     else if (branch.has(kb.ids.status)) {
         self.set(kb.ids.status, branch[kb.ids.status]);
     }

     previousCell = currentCell;
     currentCell = &self[kb.ids.previous];
     self.set(kb.ids.state, kb.ids.stateParamInit);
    }
}

static void evalOpDo(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    Brain& kb = self.kb;
    if (self.missing(kb.ids.state)) {
        self.set(kb.ids.state, kb.ids.stateParamInit);
    }
    CellI& state = self[kb.ids.state];
    if (&state == &kb.ids.stateParamInit) {
        self.set(kb.ids.previous, *previousCell);
        self.set(kb.ids.status, kb.ids.process);
        CellI& statement = self[kb.ids.statement];
        previousCell = currentCell;
        currentCell = &statement;
        self.set(kb.ids.state, kb.ids.stateStatement);
    } else if (&state == &kb.ids.stateStatement) {
        CellI& statement = self[kb.ids.statement];
        if (&statement.struct_() == &kb.std.op.Return) {
            self.set(kb.ids.status, kb.ids.return_);
        }
        else if (statement.has(kb.ids.status)) {
            self.set(kb.ids.status, statement[kb.ids.status]);
        }
        CellI& inputCondition = self[kb.ids.condition];
        previousCell = currentCell;
        currentCell = &inputCondition;
        self.set(kb.ids.state, kb.ids.stateCondition);
    } else if (&state == &kb.ids.stateCondition) {
        previousCell = currentCell;
        if (self.has(kb.ids.status) && (&self[kb.ids.status] == &kb.ids.return_)) {
            currentCell = &self[kb.ids.previous];
            self.set(kb.ids.state, kb.ids.stateParamInit);
        } else {
            self.set(kb.ids.status, kb.ids.process);
            bool condition = &self[kb.ids.condition][kb.ids.value] == &kb.boolean.true_;
            if (condition) {
                currentCell = &self[kb.ids.statement];
                self.set(kb.ids.state, kb.ids.stateStatement);
            } else {
                currentCell = &self[kb.ids.previous];
                self.set(kb.ids.state, kb.ids.stateParamInit);
            }
        }
    }
}

static void evalOpWhile(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    Brain& kb = self.kb;
    if (self.missing(kb.ids.state)) {
        self.set(kb.ids.state, kb.ids.stateParamInit);
    }
    CellI& state = self[kb.ids.state];
    if (&state == &kb.ids.stateParamInit) {
        self.set(kb.ids.previous, *previousCell);
        self.set(kb.ids.status, kb.ids.process);
        CellI& inputCondition = self[kb.ids.condition];
        previousCell = currentCell;
        currentCell = &inputCondition;
        self.set(kb.ids.state, kb.ids.stateCondition);
    } else if (&state == &kb.ids.stateStatement) {
        CellI& statement = self[kb.ids.statement];
        if (&statement.struct_() == &kb.std.op.Return) {
            self.set(kb.ids.status, kb.ids.return_);
        }
        else if (statement.has(kb.ids.status)) {
            self.set(kb.ids.status, statement[kb.ids.status]);
        }
        CellI& inputCondition = self[kb.ids.condition];
        previousCell = currentCell;
        currentCell = &inputCondition;
        self.set(kb.ids.state, kb.ids.stateCondition);
    } else if (&state == &kb.ids.stateCondition) {
        previousCell = currentCell;
        if (self.has(kb.ids.status) && (&self[kb.ids.status] == &kb.ids.return_)) {
            currentCell = &self[kb.ids.previous];
            self.set(kb.ids.state, kb.ids.stateParamInit);
        } else {
            self.set(kb.ids.status, kb.ids.process);
            bool condition = &self[kb.ids.condition][kb.ids.value] == &kb.boolean.true_;
            if (condition) {
                currentCell = &self[kb.ids.statement];
                self.set(kb.ids.state, kb.ids.stateStatement);
            } else {
                currentCell = &self[kb.ids.previous];
                self.set(kb.ids.state, kb.ids.stateParamInit);
            }
        }
    }
}

static void evalOpBlock(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    Brain& kb = self.kb;
    if (self.missing(kb.ids.state)) {
        self.set(kb.ids.state, kb.ids.stateParamInit);
    }
    CellI& state = self[kb.ids.state];
    if (&state == &kb.ids.stateParamInit) {
        self.set(kb.ids.previous, *previousCell);
        CellI& ops   = self[kb.ids.ops];
        previousCell = currentCell;
        currentCell  = &ops;
        self.set(kb.ids.status, kb.ids.process);
        self.set(kb.ids.state, kb.ids.stateParam1);
    } else if (&state == &kb.ids.stateParam1) {
        previousCell = currentCell;
        currentCell  = &self[kb.ids.previous];
        self.set(kb.ids.state, kb.ids.stateParamInit);
    }
}

static void evalOpReturn(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    Brain& kb = self.kb;
    if (self.missing(kb.ids.state)) {
        self.set(kb.ids.state, kb.ids.stateParamInit);
    }
    CellI& state = self[kb.ids.state];
    if (self.has(kb.ids.result)) {
        if (&state == &kb.ids.stateParamInit) {
            self.set(kb.ids.previous, *previousCell);
            CellI& result = self[kb.ids.result];
            previousCell  = currentCell;
            currentCell   = &result;
            self.set(kb.ids.state, kb.ids.stateParam1);
        } else if (&state == &kb.ids.stateParam1) {
            previousCell = currentCell;
            currentCell  = &self[kb.ids.previous];
            self.set(kb.ids.state, kb.ids.stateParamInit);
        }
    } else {
        std::swap(currentCell, previousCell);
    }
}

// compare
static void evalOpSame(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    Brain& kb = self.kb;
    if (self.missing(kb.ids.state)) {
        self.set(kb.ids.state, kb.ids.stateParamInit);
    }
    CellI& state = self[kb.ids.state];
    if (&state == &kb.ids.stateParamInit) {
        self.set(kb.ids.previous, *previousCell);
        CellI& inputLhs  = self[kb.ids.lhs];
        previousCell     = currentCell;
        currentCell      = &inputLhs;
        self.set(kb.ids.state, kb.ids.stateLhs);
    } else if (&state == &kb.ids.stateLhs) {
        CellI& inputRhs  = self[kb.ids.rhs];
        previousCell     = currentCell;
        currentCell      = &inputRhs;
        self.set(kb.ids.state, kb.ids.stateRhs);
    } else if (&state == &kb.ids.stateRhs) {
        CellI* lhs = &self[kb.ids.lhs][kb.ids.value];
        CellI* rhs = &self[kb.ids.rhs][kb.ids.value];

        self.set(kb.ids.value, kb.toKbBool(lhs == rhs));
        self.set(kb.ids.state, kb.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self[kb.ids.previous];
    }
}

static void evalOpNotSame(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    Brain& kb = self.kb;
    if (self.missing(kb.ids.state)) {
        self.set(kb.ids.state, kb.ids.stateParamInit);
    }
    CellI& state = self[kb.ids.state];
    if (&state == &kb.ids.stateParamInit) {
        self.set(kb.ids.previous, *previousCell);
        CellI& inputLhs = self[kb.ids.lhs];
        previousCell    = currentCell;
        currentCell     = &inputLhs;
        self.set(kb.ids.state, kb.ids.stateLhs);
    } else if (&state == &kb.ids.stateLhs) {
        CellI& inputRhs = self[kb.ids.rhs];
        previousCell    = currentCell;
        currentCell     = &inputRhs;
        self.set(kb.ids.state, kb.ids.stateRhs);
    } else if (&state == &kb.ids.stateRhs) {
        CellI* lhs = &self[kb.ids.lhs][kb.ids.value];
        CellI* rhs = &self[kb.ids.rhs][kb.ids.value];

        self.set(kb.ids.value, kb.toKbBool(lhs != rhs));
        self.set(kb.ids.state, kb.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self[kb.ids.previous];
    }
}

static void evalOpEqual(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    Brain& kb = self.kb;
    if (self.missing(kb.ids.state)) {
        self.set(kb.ids.state, kb.ids.stateParamInit);
    }
    CellI& state = self.get(kb.ids.state);
    if (&state == &kb.ids.stateParamInit) {
        self.set(kb.ids.previous, *previousCell);
        CellI& inputLhs = self.get(kb.ids.lhs);
        previousCell    = currentCell;
        currentCell     = &inputLhs;
        self.set(kb.ids.state, kb.ids.stateLhs);
    } else if (&state == &kb.ids.stateLhs) {
        CellI& inputRhs = self.get(kb.ids.rhs);
        previousCell    = currentCell;
        currentCell     = &inputRhs;
        self.set(kb.ids.state, kb.ids.stateRhs);
    } else if (&state == &kb.ids.stateRhs) {
        CellI& lhs = self[kb.ids.lhs][kb.ids.value];
        CellI& rhs = self[kb.ids.rhs][kb.ids.value];

        self.set(kb.ids.value, kb.toKbBool(lhs == rhs));
        self.set(kb.ids.state, kb.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self.get(kb.ids.previous);
    }
}

static void evalOpNotEqual(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    Brain& kb = self.kb;
    if (self.missing(kb.ids.state)) {
        self.set(kb.ids.state, kb.ids.stateParamInit);
    }
    CellI& state = self.get(kb.ids.state);
    if (&state == &kb.ids.stateParamInit) {
        self.set(kb.ids.previous, *previousCell);
        CellI& inputLhs = self.get(kb.ids.lhs);
        previousCell    = currentCell;
        currentCell     = &inputLhs;
        self.set(kb.ids.state, kb.ids.stateLhs);
    } else if (&state == &kb.ids.stateLhs) {
        CellI& inputRhs = self.get(kb.ids.rhs);
        previousCell    = currentCell;
        currentCell     = &inputRhs;
        self.set(kb.ids.state, kb.ids.stateRhs);
    } else if (&state == &kb.ids.stateRhs) {
        CellI& lhs = self[kb.ids.lhs][kb.ids.value];
        CellI& rhs = self[kb.ids.rhs][kb.ids.value];

        self.set(kb.ids.value, kb.toKbBool(lhs != rhs));
        self.set(kb.ids.state, kb.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self.get(kb.ids.previous);
    }
}

static void evalOpLessThan(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    Brain& kb = self.kb;
    if (self.missing(kb.ids.state)) {
        self.set(kb.ids.state, kb.ids.stateParamInit);
    }
    CellI& state = self.get(kb.ids.state);
    if (&state == &kb.ids.stateParamInit) {
        self.set(kb.ids.previous, *previousCell);
        CellI& inputLhs = self.get(kb.ids.lhs);
        previousCell    = currentCell;
        currentCell     = &inputLhs;
        self.set(kb.ids.state, kb.ids.stateLhs);
    } else if (&state == &kb.ids.stateLhs) {
        CellI& inputRhs = self.get(kb.ids.rhs);
        previousCell    = currentCell;
        currentCell     = &inputRhs;
        self.set(kb.ids.state, kb.ids.stateRhs);
    } else if (&state == &kb.ids.stateRhs) {
        int lhs = static_cast<Number&>(self[kb.ids.lhs][kb.ids.value]).value();
        int rhs = static_cast<Number&>(self[kb.ids.rhs][kb.ids.value]).value();

        self.set(kb.ids.value, lhs < rhs ? kb.boolean.true_ : kb.boolean.false_);
        self.set(kb.ids.state, kb.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self.get(kb.ids.previous);
    }
}

static void evalOpLessThanOrEqual(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    Brain& kb = self.kb;
    if (self.missing(kb.ids.state)) {
        self.set(kb.ids.state, kb.ids.stateParamInit);
    }
    CellI& state = self.get(kb.ids.state);
    if (&state == &kb.ids.stateParamInit) {
        self.set(kb.ids.previous, *previousCell);
        CellI& inputLhs = self.get(kb.ids.lhs);
        previousCell    = currentCell;
        currentCell     = &inputLhs;
        self.set(kb.ids.state, kb.ids.stateLhs);
    } else if (&state == &kb.ids.stateLhs) {
        CellI& inputRhs = self.get(kb.ids.rhs);
        previousCell    = currentCell;
        currentCell     = &inputRhs;
        self.set(kb.ids.state, kb.ids.stateRhs);
    } else if (&state == &kb.ids.stateRhs) {
        int lhs = static_cast<Number&>(self[kb.ids.lhs][kb.ids.value]).value();
        int rhs = static_cast<Number&>(self[kb.ids.rhs][kb.ids.value]).value();

        self.set(kb.ids.value, lhs <= rhs ? kb.boolean.true_ : kb.boolean.false_);
        self.set(kb.ids.state, kb.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self.get(kb.ids.previous);
    }
}

static void evalOpGreaterThan(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    Brain& kb = self.kb;
    if (self.missing(kb.ids.state)) {
        self.set(kb.ids.state, kb.ids.stateParamInit);
    }
    CellI& state = self.get(kb.ids.state);
    if (&state == &kb.ids.stateParamInit) {
        self.set(kb.ids.previous, *previousCell);
        CellI& inputLhs = self.get(kb.ids.lhs);
        previousCell    = currentCell;
        currentCell     = &inputLhs;
        self.set(kb.ids.state, kb.ids.stateLhs);
    } else if (&state == &kb.ids.stateLhs) {
        CellI& inputRhs = self.get(kb.ids.rhs);
        previousCell    = currentCell;
        currentCell     = &inputRhs;
        self.set(kb.ids.state, kb.ids.stateRhs);
    } else if (&state == &kb.ids.stateRhs) {
        int lhs = static_cast<Number&>(self[kb.ids.lhs][kb.ids.value]).value();
        int rhs = static_cast<Number&>(self[kb.ids.rhs][kb.ids.value]).value();

        self.set(kb.ids.value, lhs > rhs ? kb.boolean.true_ : kb.boolean.false_);
        self.set(kb.ids.state, kb.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self.get(kb.ids.previous);
    }
}

static void evalOpGreaterThanOrEqual(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    Brain& kb = self.kb;
    if (self.missing(kb.ids.state)) {
        self.set(kb.ids.state, kb.ids.stateParamInit);
    }
    CellI& state = self.get(kb.ids.state);
    if (&state == &kb.ids.stateParamInit) {
        self.set(kb.ids.previous, *previousCell);
        CellI& inputLhs = self.get(kb.ids.lhs);
        previousCell    = currentCell;
        currentCell     = &inputLhs;
        self.set(kb.ids.state, kb.ids.stateLhs);
    } else if (&state == &kb.ids.stateLhs) {
        CellI& inputRhs = self.get(kb.ids.rhs);
        previousCell    = currentCell;
        currentCell     = &inputRhs;
        self.set(kb.ids.state, kb.ids.stateRhs);
    } else if (&state == &kb.ids.stateRhs) {
        int lhs = static_cast<Number&>(self[kb.ids.lhs][kb.ids.value]).value();
        int rhs = static_cast<Number&>(self[kb.ids.rhs][kb.ids.value]).value();

        self.set(kb.ids.value, lhs >= rhs ? kb.boolean.true_ : kb.boolean.false_);
        self.set(kb.ids.state, kb.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self.get(kb.ids.previous);
    }
}
// logic
static void evalOpAnd(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    Brain& kb = self.kb;
    if (self.missing(kb.ids.state)) {
        self.set(kb.ids.state, kb.ids.stateParamInit);
    }
    CellI& state = self.get(kb.ids.state);
    if (&state == &kb.ids.stateParamInit) {
        self.set(kb.ids.previous, *previousCell);
        CellI& inputLhs = self.get(kb.ids.lhs);
        previousCell    = currentCell;
        currentCell     = &inputLhs;
        self.set(kb.ids.state, kb.ids.stateLhs);
    } else if (&state == &kb.ids.stateLhs) {
        bool lhs = &self[kb.ids.lhs][kb.ids.value] == &kb.boolean.true_;
        // shortcut, if the left hand side already false we don't evaluate the right hand side
        if (lhs == false) {
            self.set(kb.ids.value, kb.boolean.false_);
            self.set(kb.ids.state, kb.ids.stateParamInit);
            previousCell = currentCell;
            currentCell  = &self.get(kb.ids.previous);
        } else {
            CellI& inputRhs = self.get(kb.ids.rhs);
            previousCell    = currentCell;
            currentCell     = &inputRhs;
            self.set(kb.ids.state, kb.ids.stateRhs);
        }
    } else if (&state == &kb.ids.stateRhs) {
        bool lhs = &self[kb.ids.lhs][kb.ids.value] == &kb.boolean.true_;
        bool rhs = &self[kb.ids.rhs][kb.ids.value] == &kb.boolean.true_;

        self.set(kb.ids.value, kb.toKbBool(lhs && rhs));
        self.set(kb.ids.state, kb.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self.get(kb.ids.previous);
    }
}

static void evalOpOr(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    Brain& kb = self.kb;
    if (self.missing(kb.ids.state)) {
        self.set(kb.ids.state, kb.ids.stateParamInit);
    }
    CellI& state = self.get(kb.ids.state);
    if (&state == &kb.ids.stateParamInit) {
        self.set(kb.ids.previous, *previousCell);
        CellI& inputLhs = self.get(kb.ids.lhs);
        previousCell    = currentCell;
        currentCell     = &inputLhs;
        self.set(kb.ids.state, kb.ids.stateLhs);
    } else if (&state == &kb.ids.stateLhs) {
        CellI& inputRhs = self.get(kb.ids.rhs);
        previousCell    = currentCell;
        currentCell     = &inputRhs;
        self.set(kb.ids.state, kb.ids.stateRhs);
    } else if (&state == &kb.ids.stateRhs) {
        bool lhs = &self[kb.ids.lhs][kb.ids.value] == &kb.boolean.true_;
        bool rhs = &self[kb.ids.rhs][kb.ids.value] == &kb.boolean.true_;

        self.set(kb.ids.value, kb.toKbBool(lhs || rhs));
        self.set(kb.ids.state, kb.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self.get(kb.ids.previous);
    }
}

static void evalOpNot(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    Brain& kb = self.kb;
    if (self.missing(kb.ids.state)) {
        self.set(kb.ids.state, kb.ids.stateParamInit);
    }
    CellI& state = self[kb.ids.state];
    if (&state == &kb.ids.stateParamInit) {
        self.set(kb.ids.previous, *previousCell);
        CellI& input = self[kb.ids.input];
        previousCell = currentCell;
        currentCell  = &input;
        self.set(kb.ids.state, kb.ids.stateParam1);
    } else if (&state == &kb.ids.stateParam1) {
        bool res = &self[kb.ids.input][kb.ids.value] == &kb.boolean.true_;

        self.set(kb.ids.value, kb.toKbBool(!res));
        self.set(kb.ids.state, kb.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self.get(kb.ids.previous);
    }
}

// math
static void evalOpAdd(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    Brain& kb = self.kb;
    if (self.missing(kb.ids.state)) {
        self.set(kb.ids.state, kb.ids.stateParamInit);
    }
    CellI& state = self.get(kb.ids.state);
    if (&state == &kb.ids.stateParamInit) {
        self.set(kb.ids.previous, *previousCell);
        CellI& inputLhs = self.get(kb.ids.lhs);
        previousCell    = currentCell;
        currentCell     = &inputLhs;
        self.set(kb.ids.state, kb.ids.stateLhs);
    } else if (&state == &kb.ids.stateLhs) {
        CellI& inputRhs = self.get(kb.ids.rhs);
        previousCell    = currentCell;
        currentCell     = &inputRhs;
        self.set(kb.ids.state, kb.ids.stateRhs);
    } else if (&state == &kb.ids.stateRhs) {
        int lhs = static_cast<Number&>(self[kb.ids.lhs][kb.ids.value]).value();
        int rhs = static_cast<Number&>(self[kb.ids.rhs][kb.ids.value]).value();

        self.set(kb.ids.value, kb.pools.numbers.get(lhs + rhs));
        self.set(kb.ids.state, kb.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self.get(kb.ids.previous);
    }
}

static void evalOpSubtract(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    Brain& kb = self.kb;
    if (self.missing(kb.ids.state)) {
        self.set(kb.ids.state, kb.ids.stateParamInit);
    }
    CellI& state = self.get(kb.ids.state);
    if (&state == &kb.ids.stateParamInit) {
        self.set(kb.ids.previous, *previousCell);
        CellI& inputLhs = self.get(kb.ids.lhs);
        previousCell    = currentCell;
        currentCell     = &inputLhs;
        self.set(kb.ids.state, kb.ids.stateLhs);
    } else if (&state == &kb.ids.stateLhs) {
        CellI& inputRhs = self.get(kb.ids.rhs);
        previousCell    = currentCell;
        currentCell     = &inputRhs;
        self.set(kb.ids.state, kb.ids.stateRhs);
    } else if (&state == &kb.ids.stateRhs) {
        int lhs = static_cast<Number&>(self[kb.ids.lhs][kb.ids.value]).value();
        int rhs = static_cast<Number&>(self[kb.ids.rhs][kb.ids.value]).value();

        self.set(kb.ids.value, kb.pools.numbers.get(lhs - rhs));
        self.set(kb.ids.state, kb.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self.get(kb.ids.previous);
    }
}

static void evalOpMultiply(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    Brain& kb = self.kb;
    if (self.missing(kb.ids.state)) {
        self.set(kb.ids.state, kb.ids.stateParamInit);
    }
    CellI& state = self.get(kb.ids.state);
    if (&state == &kb.ids.stateParamInit) {
        self.set(kb.ids.previous, *previousCell);
        CellI& inputLhs = self.get(kb.ids.lhs);
        previousCell    = currentCell;
        currentCell     = &inputLhs;
        self.set(kb.ids.state, kb.ids.stateLhs);
    } else if (&state == &kb.ids.stateLhs) {
        CellI& inputRhs = self.get(kb.ids.rhs);
        previousCell    = currentCell;
        currentCell     = &inputRhs;
        self.set(kb.ids.state, kb.ids.stateRhs);
    } else if (&state == &kb.ids.stateRhs) {
        int lhs = static_cast<Number&>(self[kb.ids.lhs][kb.ids.value]).value();
        int rhs = static_cast<Number&>(self[kb.ids.rhs][kb.ids.value]).value();

        self.set(kb.ids.value, kb.pools.numbers.get(lhs * rhs));
        self.set(kb.ids.state, kb.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self.get(kb.ids.previous);
    }
}

static void evalOpDivide(CellI& self, CellI*& currentCell, CellI*& previousCell)
{
    Brain& kb = self.kb;
    if (self.missing(kb.ids.state)) {
        self.set(kb.ids.state, kb.ids.stateParamInit);
    }
    CellI& state = self.get(kb.ids.state);
    if (&state == &kb.ids.stateParamInit) {
        self.set(kb.ids.previous, *previousCell);
        CellI& inputLhs = self.get(kb.ids.lhs);
        previousCell    = currentCell;
        currentCell     = &inputLhs;
        self.set(kb.ids.state, kb.ids.stateLhs);
    } else if (&state == &kb.ids.stateLhs) {
        CellI& inputRhs = self.get(kb.ids.rhs);
        previousCell    = currentCell;
        currentCell     = &inputRhs;
        self.set(kb.ids.state, kb.ids.stateRhs);
    } else if (&state == &kb.ids.stateRhs) {
        int lhs = static_cast<Number&>(self[kb.ids.lhs][kb.ids.value]).value();
        int rhs = static_cast<Number&>(self[kb.ids.rhs][kb.ids.value]).value();

        self.set(kb.ids.value, kb.pools.numbers.get(lhs / rhs));
        self.set(kb.ids.state, kb.ids.stateParamInit);
        previousCell = currentCell;
        currentCell  = &self.get(kb.ids.previous);
    }
}

void Object::operator()()
{
    s_debugFunctionCalls = false; // Turn on / off debug here

    CellI* currentCell  = this;
    CellI* previousCell = &kb.ids.emptyObject;
    kb.ap.m_currentCell  = currentCell;
    kb.ap.m_previousCell = previousCell;
    int tick = 0;
    //    std::cout << "Object::operator()()" << std::endl;

    do {
        CellI& self = *currentCell;
        CellI& type = self.struct_();
        if (!(&type == &kb.std.op.Function || (type.has(kb.ids.memberOf) && type[kb.ids.memberOf][kb.ids.index].has(kb.std.op.Function)))) {
//            std::cout << self.struct_().label() << ":" << (self.has(kb.ids.state) ? self[kb.ids.state].label() : "(empty)") << std::endl;
        }
        if (&type == &kb.std.op.Get) {
            evalOpGet(self, currentCell, previousCell);
        } else if (&type == &kb.std.op.Set) {
            evalOpSet(self, currentCell, previousCell);
        } else if (&type == &kb.std.op.Has) {
            evalOpHas(self, currentCell, previousCell);
        } else if (&type == &kb.std.op.Missing) {
            evalOpMissing(self, currentCell, previousCell);
        } else if (&type == &kb.std.op.Erase) {
            evalOpErase(self, currentCell, previousCell);
        } else if (&type == &kb.std.op.New) {
            evalOpNew(self, currentCell, previousCell);
        } else if (&type == &kb.std.op.Delete) {
            evalOpDelete(self, currentCell, previousCell);
        } else if (&type == &kb.std.op.Activate) {
            evalOpActivate(self, currentCell, previousCell);
        } else if (&type == &kb.std.op.Call) {
            evalOpCall(self, currentCell, previousCell);
        } else if (&type == &kb.std.op.Function || (type.has(kb.ids.memberOf) && type[kb.ids.memberOf][kb.ids.index].has(kb.std.op.Function))) {
            if (s_debugFunctionCalls && (self.missing(kb.ids.state) || (&self[kb.ids.state] == &kb.ids.stateParamInit))) {
                printIndent();
                s_indent++;
                std::cout << self.label() << std::endl;
            }
            if (s_debugFunctionCalls && (self.has(kb.ids.state) && (&self[kb.ids.state] == &kb.ids.stateParam1))) {
                s_indent--;
            }
            evalOpFunction(self, currentCell, previousCell);
        } else if (&type == &kb.std.op.Return) {
            evalOpReturn(self, currentCell, previousCell);
        } else if (&type == &kb.std.op.Same) {
            evalOpSame(self, currentCell, previousCell);
        } else if (&type == &kb.std.op.NotSame) {
            evalOpNotSame(self, currentCell, previousCell);
        } else if (&type == &kb.std.op.Equal) {
            evalOpEqual(self, currentCell, previousCell);
        } else if (&type == &kb.std.op.NotEqual) {
            evalOpNotEqual(self, currentCell, previousCell);
        } else if (&type == &kb.std.op.LessThan) {
            evalOpLessThan(self, currentCell, previousCell);
        } else if (&type == &kb.std.op.LessThanOrEqual) {
            evalOpLessThanOrEqual(self, currentCell, previousCell);
        } else if (&type == &kb.std.op.GreaterThan) {
            evalOpGreaterThan(self, currentCell, previousCell);
        } else if (&type == &kb.std.op.GreaterThanOrEqual) {
            evalOpGreaterThanOrEqual(self, currentCell, previousCell);
        } else if (&type == &kb.std.op.And) {
            evalOpAnd(self, currentCell, previousCell);
        } else if (&type == &kb.std.op.Or) {
            evalOpOr(self, currentCell, previousCell);
        } else if (&type == &kb.std.op.Not) {
            evalOpNot(self, currentCell, previousCell);
        } else if (&type == &kb.std.op.If) {
            evalOpIf(self, currentCell, previousCell);
        } else if (&type == &kb.std.op.Do) {
            evalOpDo(self, currentCell, previousCell);
        } else if (&type == &kb.std.op.While) {
            evalOpWhile(self, currentCell, previousCell);
        } else if (&type == &kb.std.op.Block) {
            evalOpBlock(self, currentCell, previousCell);
        } else if (&type == &kb.std.op.Add) {
            evalOpAdd(self, currentCell, previousCell);
        } else if (&type == &kb.std.op.Subtract) {
            evalOpSubtract(self, currentCell, previousCell);
        } else if (&type == &kb.std.op.Multiply) {
            evalOpMultiply(self, currentCell, previousCell);
        } else if (&type == &kb.std.op.Divide) {
            evalOpDivide(self, currentCell, previousCell);
        } else {
            // assuming it is a datacell, so do nothing
            std::swap(currentCell, previousCell);
        }
        kb.ap.m_currentCell = currentCell;
        kb.ap.m_previousCell = previousCell;
        ++tick;
    } while (currentCell != &kb.ids.emptyObject);
    kb.ap.m_time.value(kb.ap.m_time.value() + tick);
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
    getMethod(kb.ids.destructor)();
}

CellI& Object::method(const std::string& key)
{
    return method(kb.name(key));
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
    return struct_().has(kb.ids.methods) && struct_()[kb.ids.methods].has(kb.ids.index) && struct_()[kb.ids.methods][kb.ids.index].has(key);
}

CellI& Object::getMethod(CellI& key)
{
    resetIndent();
    if (struct_().has(kb.ids.methods)) {
        CellI& methodsIndex = struct_()[kb.ids.methods][kb.ids.index];
        if (methodsIndex.has(key)) {
            CellI& method = methodsIndex[key][kb.ids.value];
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
    if (has(kb.ids.methods)) {
        CellI& methodsIndex = (*this)[kb.ids.methods][kb.ids.index];
        if (methodsIndex.has(key)) {
            CellI& method = methodsIndex[key][kb.ids.value];
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
    Object& inputIndex    = *new Object(kb, kb.std.Index, "StackFrame1.InputIndex");
    Object& stackFrame    = *new Object(kb, kb.std.StackFrame, "StackFrame1");
    Object& stackListItem0 = *new Object(kb, kb.std.ListItem, "StackListItem0");
    Object& stackListItem1 = *new Object(kb, kb.std.ListItem, "StackListItem1");
    stackFrame.set(kb.ids.method, method);
    stackFrame.set(kb.ids.input, inputIndex);

    stackListItem0.set(kb.ids.next, stackListItem1);
    stackListItem1.set(kb.ids.value, stackFrame);
    stackListItem1.set(kb.ids.previous, stackListItem0);
    method.set(kb.ids.stack, stackListItem1);
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
    if (method.struct_()[kb.ids.subTypes][kb.ids.index].missing(kb.ids.localVars)) {
        return;
    }
    CellI& localVarsType   = method.struct_()[kb.ids.subTypes][kb.ids.index][kb.ids.localVars][kb.ids.value];
    Object& localVarsIndex = *new Object(kb, localVarsType, "LocalVarsIndex");
    CellI& stackFrame      = method[kb.ids.stack][kb.ids.value];
    stackFrame.set(kb.ids.localVars, localVarsIndex);
    Visitor::visitList(localVarsType[kb.ids.slots][kb.ids.list], [this, &localVarsIndex](CellI& slot, int i, bool&) {
        auto& key        = slot[kb.ids.key];
        Object& localVar = *new Object(kb, kb.std.op.Var, fmt::format("var {}", key.label()));
        localVar.set(kb.ids.valueType, slot[kb.ids.type]);
        localVarsIndex.set(key, localVar);
    });
}

CellI& Object::getFnValue(CellI& method)
{
    if (method.struct_()[kb.ids.subTypes][kb.ids.index].has(kb.ids.returnType)) {
        return method[kb.ids.value];
    }

    return kb.ids.emptyObject;
}

void Object::setSelf(CellI& method)
{
    setFnParam(method, { kb.ids.self, *this });
}

void Object::setFnParam(CellI& fn, Param param)
{
    if (fn.struct_()[kb.ids.subTypes][kb.ids.index][kb.ids.parameters][kb.ids.value].has(kb.ids.slots)) {
        CellI& inputsIndex = fn.struct_()[kb.ids.subTypes][kb.ids.index][kb.ids.parameters][kb.ids.value][kb.ids.slots][kb.ids.index];
        if (inputsIndex.has(param.key)) {
            fn[kb.ids.stack][kb.ids.value][kb.ids.input].set(param.key, param.value);
        } else {
            throw "No such param!";
        }
    }
}

#pragma endregion
#pragma region List::Item
// ============================================================================
List::Item::Item(Brain& kb, List& list, CellI& value) :
    CellI(kb),
    m_list(list),
    m_value(value)
{
}

bool List::Item::has(CellI& key)
{
    if (&key == &kb.ids.struct_ || &key == &kb.ids.value) {
        return true;
    }
    if (&key == &kb.ids.previous && m_previous) {
        return true;
    }
    if (&key == &kb.ids.next && m_next) {
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
    if (&key == &kb.ids.next) {
        m_next = nullptr;
    }
    if (&key == &kb.ids.previous) {
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
    if (&key == &kb.ids.struct_) {
        if (!m_selfType) {
            m_selfType = &kb.getStruct(kb.templateId("std::ListItem", kb.ids.valueType, m_list.m_valueType));
        }
        return *m_selfType;
    }
    if (&key == &kb.ids.previous) {
        if (m_previous)
            return *m_previous;
        else
            throw "No such key!";
    }
    if (&key == &kb.ids.next) {
        if (m_next)
            return *m_next;
        else
            throw "No such key!";
    }
    if (&key == &kb.ids.value) {
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
List::List(Brain& kb, CellI& valueType, const std::string& label) :
    CellI(kb, label),
    m_valueType(valueType)
{
}

bool List::has(CellI& key)
{
    if (&key == &kb.ids.struct_ || &key == &kb.ids.size) {
        return true;
    }
    if (&key == &kb.ids.first && m_firstItem) {
        return true;
    }
    if (&key == &kb.ids.last && m_lastItem) {
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
    if (&key == &kb.ids.struct_) {
        if (!m_selfType) {
            m_selfType = &kb.getStruct(kb.templateId("std::List", kb.ids.valueType, m_valueType));
        }
        return *m_selfType;
    }
    if (&key == &kb.ids.first) {
        return *m_firstItem;
    }
    if (&key == &kb.ids.last) {
        return *m_lastItem;
    }
    if (&key == &kb.ids.size) {
        int size = (int)m_size;

        return kb.pools.numbers.get(size);
    }

    throw "No such key!";
}

void List::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

List::Item* List::add(CellI& value)
{
    Item* item = new Item(kb, *this, value);
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
    Item* item = new Item(kb, *this, value);
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
        if (currentNode.has(kb.ids.next)) {
            nextNode = &currentNode[kb.ids.next];
        }
        delete node;
        node = nextNode;
    }
}

#pragma endregion
#pragma region Struct
// ============================================================================
Struct::Struct(Brain& kb, const std::string& label) :
    CellI(kb, label),
    m_slots(*new Map(kb, kb.std.Cell, kb.std.Slot))
{
}

Struct::Struct(Brain& kb, WithRecursiveType recursiveType, const std::string& label) :
    CellI(kb, label),
    m_slots(*new Map(kb, kb.std.Cell, kb.std.Slot, *this))
{
}

bool Struct::has(CellI& key)
{
    if (&key == &kb.ids.struct_) {
        return true;
    }
    if (&key == &kb.ids.name) {
        return true;
    }
    if (&key == &kb.ids.slots) {
        return true;
    }
    if (&key == &kb.ids.subTypes) {
        return m_subTypes;
    }
    if (&key == &kb.ids.memberOf) {
        return m_memberOf;
    }
    if (&key == &kb.ids.asts) {
        return m_asts;
    }
    if (&key == &kb.ids.methods) {
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
    if (&key == &kb.ids.struct_) {
        return kb.std.Struct;
    }
    if (&key == &kb.ids.name) {
        if (m_name) {
            return *m_name;
        } else {
            m_name = &kb.name(label());
            return *m_name;
        }
    }
    if (&key == &kb.ids.slots) {
        return m_slots;
    }
    if (&key == &kb.ids.subTypes) {
        return *m_subTypes;
    }
    if (&key == &kb.ids.memberOf) {
        return *m_memberOf;
    }
    if (&key == &kb.ids.asts) {
        return *m_asts;
    }
    if (&key == &kb.ids.methods) {
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
Index::Index(Brain& kb, const std::string& label) :
    CellI(kb, label),
    m_type(new Struct(kb, Struct::WithRecursiveType::Yes))
{
}

Index::Index(Brain& kb, Struct& indexType, const std::string& label) :
    CellI(kb, label),
    m_type(&indexType),
    m_recursiveType(true)
{
}

bool Index::has(CellI& key)
{
    if (&key == &kb.ids.struct_) {
        return true;
    }
    if (m_slots.find(&key) != m_slots.end()) {
        return true;
    }

    return false;
}

void Index::set(CellI& key, CellI& value)
{
    if (&key == &kb.ids.struct_) {
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
    if (&key == &kb.ids.struct_) {
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
    if (&key == &kb.ids.struct_) {
        throw "The type key can not be changed!";
    }
    m_slots[&key] = &value;
    if (m_recursiveType) {
        return;
    }
    Object& slot = *new Object(kb, kb.std.Slot);
    slot.set(kb.ids.key, key);
    slot.set(kb.ids.type, kb.std.Slot);
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
Map::Map(Brain& kb, CellI& keyType, CellI& valueType, const std::string& label) :
    CellI(kb, label),
    m_list(kb, valueType),
    m_index(kb),
    m_keyType(keyType),
    m_valueType(valueType)
{
}

Map::Map(Brain& kb, CellI& keyType, CellI& valueType, Struct& indexType, const std::string& label) :
    CellI(kb, label),
    m_list(kb, valueType),
    m_index(kb, indexType),
    m_keyType(keyType),
    m_valueType(valueType)
{
}

bool Map::has(CellI& key)
{
    if (&key == &kb.ids.struct_) {
        return true;
    }
    if (&key == &kb.ids.list) {
        return true;
    }
    if (&key == &kb.ids.index) {
        return true;
    }
    if (&key == &kb.ids.size) {
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
    if (&key == &kb.ids.struct_) {
        if (!m_selfType) {
            m_selfType = &kb.getStruct(kb.templateId("std::Map", kb.ids.keyType, m_keyType, kb.ids.valueType, m_valueType));
        }
        return *m_selfType;
    }
    if (&key == &kb.ids.list) {
        return m_list;
    }
    if (&key == &kb.ids.index) {
        return m_index;
    }
    if (&key == &kb.ids.size) {
        return kb.pools.numbers.get(m_size);
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
        return m_index[key][kb.ids.value];
    }
    throw "No such key!";
}

void Map::add(CellI& key, CellI& value)
{
    if (&key == &kb.ids.struct_) {
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
TrieMap::TrieMap(Brain& kb, CellI& keyType, CellI& valueType, const std::string& label) :
    CellI(kb, label),
    m_list(kb, valueType),
    m_rootNode(kb, kb.std.TrieMapNode, "TrieNode_Root"),
    m_keyType(keyType),
    m_valueType(valueType)
{
}

bool TrieMap::has(CellI& key)
{
    if (&key == &kb.ids.struct_) {
        return true;
    }
    if (&key == &kb.ids.list) {
        return true;
    }
    if (&key == &kb.ids.index) {
        return true;
    }
    if (&key == &kb.ids.size) {
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
    if (&key == &kb.ids.struct_) {
        if (!m_selfType) {
            m_selfType = &kb.getStruct(kb.templateId("std::TrieMap", kb.ids.keyType, m_keyType, kb.ids.valueType, m_valueType));
        }
        return *m_selfType;
    }
    if (&key == &kb.ids.list) {
        return m_list;
    }
    if (&key == &kb.ids.rootNode) {
        return m_rootNode;
    }
    if (&key == &kb.ids.size) {
        return kb.pools.numbers.get(m_size);
    }

    throw "No such key!";
}

bool TrieMap::hasKey(CellI& key)
{
    CellI* currentNode = &m_rootNode;

    if (isA(key, kb.std.List)) {
        throw "Key is not a list!";
    }

    Visitor::visitList(key, [this, &currentNode](CellI& keyItem, int i, bool& stop) {
        CellI* children = nullptr;
        if (currentNode->missing(kb.ids.children)) {
            stop        = true;
            currentNode = nullptr;
            return;
        }
        Index& childrenIndex = static_cast<Index&>(currentNode->get(kb.ids.children));
        if (childrenIndex.has(keyItem)) {
            children = &childrenIndex.get(keyItem);
        } else {
            stop        = true;
            currentNode = nullptr;
            return;
        }
        currentNode = children;
    });

    if (!currentNode || currentNode->missing(kb.ids.data)) {
        return false;
    }

    return true;
}

CellI& TrieMap::getValue(CellI& key)
{
    if (isA(key, kb.std.List)) {
        throw "Key is not a list!";
    }

    CellI* currentNode = &m_rootNode;

    Visitor::visitList(key, [this, &currentNode](CellI& keyItem, int i, bool& stop) {
        CellI* children = nullptr;
        if (currentNode->missing(kb.ids.children)) {
            stop        = true;
            currentNode = nullptr;
            return;
        }
        Index& childrenIndex = static_cast<Index&>(currentNode->get(kb.ids.children));
        if (childrenIndex.has(keyItem)) {
            children = &childrenIndex.get(keyItem);
        } else {
            stop        = true;
            currentNode = nullptr;
            return;
        }
        currentNode = children;
    });

    if (!currentNode || currentNode->missing(kb.ids.data)) {
        throw "No such key!";
    }

    return (*currentNode)[kb.ids.data][kb.ids.value][kb.ids.value];
}

CellI& TrieMap::getValueWithDataKey(CellI& key)
{
    CellI* currentNode = &m_rootNode;

    CellI& test = key.struct_();
    Visitor::visitList(key.struct_()[kb.ids.slots][kb.ids.list], [this, &currentNode, &key](CellI& slot, int i, bool& stop) {
        CellI& keyItem  = key[slot[kb.ids.key]];
        CellI* children = nullptr;
        if (currentNode->missing(kb.ids.children)) {
            stop        = true;
            currentNode = nullptr;
            return;
        }
        Index& childrenIndex = static_cast<Index&>(currentNode->get(kb.ids.children));
        if (childrenIndex.has(keyItem)) {
            children = &childrenIndex.get(keyItem);
        } else {
            stop        = true;
            currentNode = nullptr;
            return;
        }
        currentNode = children;
    });

    if (!currentNode || currentNode->missing(kb.ids.data)) {
        throw "No such key!";
    }

    return (*currentNode)[kb.ids.data][kb.ids.value][kb.ids.value];
}

void TrieMap::addWithDataKey(CellI& key, CellI& value)
{
    CellI* currentNode = &m_rootNode;

    Visitor::visitList(key.struct_()[kb.ids.slots][kb.ids.list], [this, &currentNode, &key](CellI& slot, int i, bool& stop) {
        CellI& keyItem = key[slot[kb.ids.key]];
        CellI* child = nullptr;
        if (currentNode->missing(kb.ids.children)) {
            currentNode->set(kb.ids.children, *new Index(kb));
        }
        Index& childrenIndex = static_cast<Index&>(currentNode->get(kb.ids.children));
        if (childrenIndex.has(keyItem)) {
            child = &childrenIndex.get(keyItem);
        } else {
            child = new Object(kb, kb.std.TrieMapNode);
            child->set(kb.ids.parent, *currentNode);
            childrenIndex.insert(keyItem, *child);
        }
        currentNode = child;
    });

    List::Item& item = *m_list.add(kb.std.kvPair(key, value));
    currentNode->set(kb.ids.data, item);
    ++m_size;
}

void TrieMap::add(CellI& key, CellI& value)
{
    if (isA(key, kb.std.List)) {
        throw "Key is not a list!";
    }

    CellI* currentNode = &m_rootNode;

    Visitor::visitList(key, [this, &currentNode](CellI& keyItem, int i, bool& stop) {
        CellI* child = nullptr;
        if (currentNode->missing(kb.ids.children)) {
            currentNode->set(kb.ids.children, *new Index(kb));
        }
        Index& childrenIndex = static_cast<Index&>(currentNode->get(kb.ids.children));
        if (childrenIndex.has(keyItem)) {
            child = &childrenIndex.get(keyItem);
        } else {
            child = new Object(kb, kb.std.TrieMapNode);
            child->set(kb.ids.parent, *currentNode);
            childrenIndex.insert(keyItem, *child);
        }
        currentNode = child;
    });

    List::Item& item = *m_list.add(kb.std.kvPair(key, value));
    currentNode->set(kb.ids.data, item);
    ++m_size;
}

void TrieMap::remove(CellI& key)
{
    if (isA(key, kb.std.List)) {
        throw "Key is not a list!";
    }

    if (&key[kb.ids.size] == &kb._0_) {
        return;
    }

    CellI* currentNode    = &m_rootNode;

    Visitor::visitList(key, [this, &currentNode](CellI& keyItem, int i, bool& stop) {
        CellI* children = nullptr;
        if (currentNode->missing(kb.ids.children)) {
            stop        = true;
            currentNode = nullptr;
            return;
        }
        Index& childrenIndex = static_cast<Index&>(currentNode->get(kb.ids.children));
        if (childrenIndex.has(keyItem)) {
            children = &childrenIndex.get(keyItem);
        } else {
            stop        = true;
            currentNode = nullptr;
            return;
        }
        currentNode = children;
    });

    if (!currentNode || currentNode->missing(kb.ids.data)) {
        return;
    }
    List::Item* valueItem = &static_cast<List::Item&>((*currentNode)[kb.ids.data]);
    currentNode->erase(kb.ids.data);

    CellI* keyItemPtr = &key[kb.ids.last];
    while (currentNode->has(kb.ids.parent)) {
        CellI& keyItem = *keyItemPtr;
        CellI& parent = currentNode->get(kb.ids.parent);
        CellI& child = *currentNode;
        if (child.missing(kb.ids.data)) {
            if (child.missing(kb.ids.children) || ( child.has(kb.ids.children) && static_cast<Index&>(child[kb.ids.children]).empty())) {
                delete currentNode;
                parent[kb.ids.children].erase(keyItem[kb.ids.value]);
            }
        }
        currentNode = &parent;
        if (keyItem.has(kb.ids.previous)) {
            keyItemPtr = &keyItem[kb.ids.previous];
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
Set::Set(Brain& kb, CellI& valueType, const std::string& label) :
    CellI(kb, label),
    m_valueType(valueType),
    m_index(kb)
{
}

bool Set::has(CellI& key)
{
    if (&key == &kb.ids.struct_ || &key == &kb.ids.size) {
        return true;
    }
    if (&key == &kb.ids.index) {
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
    if (&key == &kb.ids.struct_) {
        if (!m_selfType) {
            m_selfType = &kb.getStruct(kb.templateId("std::Set", kb.ids.valueType, m_valueType));
        }
        return *m_selfType;
    }
    if (&key == &kb.ids.index) {
        return m_index;
    }
    if (&key == &kb.ids.size) {
        int size = (int)m_size;

        return kb.pools.numbers.get(size);
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
Number::Number(Brain& kb, int value) :
    CellI(kb),
    m_value(value)
{
}

bool Number::has(CellI& key)
{
    if (&key == &kb.ids.struct_ || &key == &kb.ids.value) {
        return true;
    }
    if (&key == &kb.numbers.sign) {
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
    if (&key == &kb.ids.struct_) {
        return kb.std.Number;
    }

    if (&key == &kb.numbers.sign && m_value != 0) {
        return m_value > 0 ? kb.numbers.positive : kb.numbers.negative;
    }

    if (&key == &kb.ids.value) {
        if (m_digits.empty()) {
            calculateDigits();
            m_digitsList.reset(new List(kb, m_digits));
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
#pragma endregion
#pragma region String
// ============================================================================
String::String(Brain& kb, const std::string& str) :
    CellI(kb),
    m_value(str)
{
}

String::String(Brain& kb, List& list, const std::string& str) :
    CellI(kb),
    m_value(str),
    m_charactersListPtr(&list)
{
}

bool String::has(CellI& key)
{
    if (&key == &kb.ids.struct_ || &key == &kb.ids.value) {
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
    if (&key == &kb.ids.struct_) {
        return kb.std.String;
    } else if (&key == &kb.ids.value) {
        if (m_characters.empty()) {
            calculateCharacters();
            if (m_charactersListPtr) {
                for (auto& character : m_characters) {
                    m_charactersListPtr->add(*character);
                }
                return *m_charactersListPtr;
            }
            m_charactersList.reset(new List(kb, m_characters, label()));
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
        m_characters.push_back(&kb.pools.chars.get(unicodeValue));
    }
}
#pragma endregion
namespace hybrid {

// ============================================================================
ActivationPointer::ActivationPointer(Brain& kb) :
    CellI(kb), m_time(kb)
{
}

bool ActivationPointer::has(CellI& key)
{
    if (&key == &kb.ids.struct_) {
        return true;
    }
    if (&key == &kb.ids.cell || &key == &kb.ids.previous) {
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
    if (&key == &kb.ids.struct_) {
        return kb.std.Color; // TODO
    }
    if (&key == &kb.ids.cell) {
        return *m_currentCell;
    }
    if (&key == &kb.ids.previous) {
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
    Brain& kb = list.kb;
    int i            = 0;

    if (list.missing(kb.ids.first)) {
        return;
    }

    for (CellI* currentListItemPtr = &list[kb.ids.first];;) {
        CellI& currentListItem = *currentListItemPtr;
        CellI& value           = currentListItem[kb.ids.value];
        bool stop              = false;

        visitFn(value, i++, stop);
        if (stop || currentListItem.missing(kb.ids.next)) {
            return;
        }

        currentListItemPtr = &currentListItem[kb.ids.next];
    };
}
#else

void Visitor::visitList(CellI& list, std::function<void(CellI& value, int i, bool& stop)> visitFn)
{
    class ListIterator
    {
    public:
        ListIterator(CellI& list) :
            kb(list.kb), m_list(list), m_nodePtr(nullptr)
        {
        }

        bool isEmpty() {
            return m_list.missing(kb.ids.first);
        }

        void setFirstValue()
        {
            m_nodePtr = &m_list[kb.ids.first];
        }

        CellI& getCurrentValue()
        {
            return (*m_nodePtr)[kb.ids.value];
        }

        bool hasNextValue()
        {
            return (*m_nodePtr).has(kb.ids.next);
        }

        void setNextValue()
        {
            m_nodePtr = &(*m_nodePtr)[kb.ids.next];
        }

    private:
        Brain& kb;
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
    Brain& kb = list.kb;
    int i            = 0;

    if (list.missing(kb.ids.last)) {
        return;
    }

    for (CellI* currentListItemPtr = &list[kb.ids.last];;) {
        CellI& currentListItem = *currentListItemPtr;
        CellI& value           = currentListItem[kb.ids.value];
        bool stop              = false;

        visitFn(value, i++, stop);
        if (stop || currentListItem.missing(kb.ids.previous)) {
            return;
        }

        currentListItemPtr = &currentListItem[kb.ids.previous];
    }
}

bool tryVisitWith(CellI& cell, Visitor& visitor)
{
    Brain& kb = cell.kb;

    if (&cell.struct_() == &kb.std.Number) {
        visitor.visit(static_cast<Number&>(cell));
        return true;
    }
    if (&cell.struct_() == &kb.std.String) {
        visitor.visit(static_cast<String&>(cell));
        return true;
    }
#if 0 // TODO
    if (&cell.struct_() == &kb.std.Color) {
        visitor.visit(static_cast<hybrid::Color&>(cell));
        return true;
    }
    if (&cell.struct_() == &kb.std.Pixel) {
        visitor.visit(static_cast<hybrid::Pixel&>(cell));
        return true;
    }
    if (&cell.struct_() == &kb.std.Grid) {
        visitor.visit(static_cast<hybrid::Picture&>(cell));
        return true;
    }
#endif

    return false;
}

} // namespace cells
} // namespace infocell
