#include "Cells.h"
#include "Brain.h"
#include "app/Colors.h"

#include <fmt/core.h>
#include <sstream>
#include <utility>

#pragma warning(disable : 4996)
#include <utf8.h>


namespace infocell {
namespace cells {
using InitPhase = brain::Brain::InitPhase;
#pragma region CellI
// ============================================================================
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

bool CellI::has(const std::string& role)
{
    return has(kb.name(role));
}

void CellI::set(const std::string& role, CellI& value)
{
    set(kb.name(role), value);
}

void CellI::erase(const std::string& role)
{
    erase(kb.name(role));
}

CellI& CellI::operator[](const std::string& role)
{
    return (*this)[kb.name(role)];
}

bool CellI::missing(const std::string& role)
{
    return !has(kb.name(role));
}

CellI& CellI::get(const std::string& role)
{
    return (*this)[kb.name(role)];
}

bool CellI::missing(CellI& role)
{
    return !has(role);
}

CellI& CellI::get(CellI& role)
{
    return (*this)[role];
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
        CellI& role     = slot[kb.ids.slotRole];

        bool hasLeftSlot = has(role);
        if (hasLeftSlot != rhs.has(role)) {
            return false;
        }
        if (hasLeftSlot && (&(*this)[role] != &rhs[role])) {
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
Param::Param(const std::string& role, CellI& value) :
    role(value.kb.name(role)), value(value) { }

int Object::s_indent = 0;
// ============================================================================
Object::Object(brain::Brain& kb, CellI& type, const std::string& label) :
    CellI(kb, label),
    m_type(type)
{
    m_slots[&kb.ids.struct_] = &type;
}

Object::Object(brain::Brain& kb, CellI& type, CellI& constructor, const std::string& label) :
    CellI(kb, label),
    m_type(type)
{
    m_slots[&kb.ids.struct_] = &type;
    getMethod(constructor)();
}

Object::Object(brain::Brain& kb, CellI& type, CellI& constructor, Param param1, const std::string& label) :
    CellI(kb, label),
    m_type(type)
{
    m_slots[&kb.ids.struct_] = &type;

    CellI& method = getMethod(constructor);
    setFnParam(method, param1);
    method();
}

Object::Object(brain::Brain& kb, CellI& type, CellI& constructor, Param param1, Param param2, const std::string& label) :
    CellI(kb, label),
    m_type(type)
{
    m_slots[&kb.ids.struct_] = &type;

    CellI& method = getMethod(constructor);
    setFnParam(method, param1);
    setFnParam(method, param2);
    method();
}

Object::Object(brain::Brain& kb, CellI& type, CellI& constructor, Param param1, Param param2, Param param3, const std::string& label) :
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

Object::Object(brain::Brain& kb, CellI& type, CellI& constructor, Param param1, Param param2, Param param3, Param param4, const std::string& label) :
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

bool Object::has(CellI& role)
{
    if (&role == &kb.ids.struct_)
        return true;

    return m_slots.find(&role) != m_slots.end();
}

void Object::set(CellI& role, CellI& value)
{
    if ((&role == &kb.ids.struct_) && !((&struct_() == &kb.std.Index))) {
        throw "Type change not allowed.";
    }
    if ((&role == &kb.ids.struct_) && (&struct_() == &kb.std.Index)) {
        std::cout << "";
    }

    if (kb.initPhase() == InitPhase::Init) {
        m_slots[&role] = &value;
        return;
    }
    auto is = [this](CellI& rhsType) -> bool { return &struct_() == &rhsType || (struct_().has(kb.ids.memberOf) && struct_()[kb.ids.memberOf][kb.ids.index].has(rhsType)); };
    if (is(kb.std.Index) || struct_()[kb.ids.slots][kb.ids.index].has(role)) {
        m_slots[&role] = &value;
    } else {
        throw "The type doesn't contains this role.";
    }
}

void Object::erase(CellI& role)
{
    if (&role == &kb.ids.struct_) {
        throw "Type change not allowed.";
    }

    auto slotIt = m_slots.find(&role);
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

void Object::operator()()
{
    s_debugFunctionCalls = false; // Turn on / off debug here

    if (&m_type == &kb.std.op.Get) {
        opGet();
    } else if (&m_type == &kb.std.op.Set) {
        opSet();
    } else if (&m_type == &kb.std.op.Has) {
        opHas();
    } else if (&m_type == &kb.std.op.Missing) {
        opMissing();
    } else if (&m_type == &kb.std.op.Erase) {
        opErase();
    } else if (&m_type == &kb.std.op.New) {
        opNew();
    } else if (&m_type == &kb.std.op.Delete) {
        opDelete();
    } else if (&m_type == &kb.std.op.Activate) {
        opActivate();
    } else if (&m_type == &kb.std.op.Call) {
        opCall();
    } else if (&m_type == &kb.std.op.Function || (m_type.has(kb.ids.memberOf) && m_type[kb.ids.memberOf][kb.ids.index].has(kb.std.op.Function))) {
        opFunction();
    } else if (&m_type == &kb.std.op.Return) {
        opReturn();
    } else if (&m_type == &kb.std.op.Same) {
        opSame();
    } else if (&m_type == &kb.std.op.NotSame) {
        opNotSame();
    } else if (&m_type == &kb.std.op.Equal) {
        opEqual();
    } else if (&m_type == &kb.std.op.NotEqual) {
        opNotEqual();
    } else if (&m_type == &kb.std.op.LessThan) {
        opLessThan();
    } else if (&m_type == &kb.std.op.LessThanOrEqual) {
        opLessThanOrEqual();
    } else if (&m_type == &kb.std.op.GreaterThan) {
        opGreaterThan();
    } else if (&m_type == &kb.std.op.GreaterThanOrEqual) {
        opGreaterThanOrEqual();
    } else if (&m_type == &kb.std.op.And) {
        opAnd();
    } else if (&m_type == &kb.std.op.Or) {
        opOr();
    } else if (&m_type == &kb.std.op.Not) {
        opNot();
    } else if (&m_type == &kb.std.op.If) {
        opIf();
    } else if (&m_type == &kb.std.op.Do) {
        opDo();
    } else if (&m_type == &kb.std.op.While) {
        opWhile();
    } else if (&m_type == &kb.std.op.Block) {
        opBlock();
    } else if (&m_type == &kb.std.op.Add) {
        opAdd();
    } else if (&m_type == &kb.std.op.Subtract) {
        opSubtract();
    } else if (&m_type == &kb.std.op.Multiply) {
        opMultiply();
    } else if (&m_type == &kb.std.op.Divide) {
        opDivide();
    }
}

// core data handling
void Object::opGet()
{
    CellI& inputCell = get(kb.ids.cell);
    inputCell();
    CellI& cell = inputCell[kb.ids.value];

    CellI& inputRole = get(kb.ids.role);
    inputRole();
    CellI& role = inputRole[kb.ids.value];

    set(kb.ids.value, cell[role]);
}

void Object::opSet()
{
    CellI& inputCell = get(kb.ids.cell);
    inputCell();
    CellI& cell = inputCell[kb.ids.value];

    CellI& inputRole = get(kb.ids.role);
    inputRole();
    CellI& role = inputRole[kb.ids.value];

    CellI& inputValue = get(kb.ids.value);
    inputValue();
    CellI& value = inputValue[kb.ids.value];

    cell.set(role, value);
}

void Object::opHas()
{
    CellI& inputCell = get(kb.ids.cell);
    inputCell();
    CellI& cell = inputCell[kb.ids.value];

    CellI& inputRole = get(kb.ids.role);
    inputRole();
    CellI& role = inputRole[kb.ids.value];

    set(kb.ids.value, kb.toKbBool(cell.has(role)));
}

void Object::opMissing()
{
    CellI& inputCell = get(kb.ids.cell);
    inputCell();
    CellI& cell = inputCell[kb.ids.value];

    CellI& inputRole = get(kb.ids.role);
    inputRole();
    CellI& role = inputRole[kb.ids.value];

    set(kb.ids.value, kb.toKbBool(cell.missing(role)));
}

void Object::opErase()
{
    CellI& inputCell = get(kb.ids.cell);
    inputCell();
    CellI& cell = inputCell[kb.ids.value];

    CellI& inputRole = get(kb.ids.role);
    inputRole();
    CellI& role = inputRole[kb.ids.value];

    cell.erase(role);
}

void Object::opNew()
{
    CellI& inputObjectType = get(kb.ids.objectType);
    inputObjectType();
    CellI& objectType = inputObjectType[kb.ids.value];

    if (&objectType == &kb.std.Number) {
        set(kb.ids.value, *new Number(kb));
    } else if (&objectType == &kb.std.String) {
        set(kb.ids.value, *new String(kb));
    } else {
        set(kb.ids.value, *new Object(kb, objectType));
    }
}

void Object::opDelete()
{
    CellI& input = get(kb.ids.input);
    input();
    CellI* cell = &input[kb.ids.value];

    delete cell;
    input.erase(kb.ids.value);
}

// code running
void Object::opActivate()
{
    CellI& inputCell = get(kb.ids.cell);
    inputCell();
    CellI& cell = inputCell[kb.ids.value];

    cell();
}

void Object::opCall()
{
    CellI& inputCell = get(kb.ids.cell);
    inputCell();
    CellI& cell = inputCell[kb.ids.value];

    CellI& inputMethod = get(kb.ids.method);
    inputMethod();
    CellI& methodName = inputMethod[kb.ids.value];

    CellI* methodPtr = nullptr;
    if (&get(kb.ids.ast).struct_() == &kb.std.ast.Call) {
        methodPtr = &cell[kb.ids.struct_][kb.ids.methods];
    } else {
        methodPtr = &cell[kb.ids.methods];
    }
    CellI& method = (*methodPtr)[kb.ids.index][methodName][kb.ids.value];

    CellI& inputStack = get(kb.ids.stack);
    inputStack();
    CellI& stack = inputStack[kb.ids.value];

    CellI& stackFrame = *new Object(kb, kb.std.StackFrame);
    stackFrame.set(kb.ids.method, method);

    CellI& inputIndex = *new Object(kb, kb.std.Index);
    inputIndex.set(kb.ids.self, cell);
    if (has(kb.ids.parameters)) {
        Visitor::visitList(get(kb.ids.parameters), [this, &inputIndex](CellI& parameter, int, bool& stop) {
            parameter[kb.ids.slotType]();
            inputIndex.set(parameter[kb.ids.slotRole], parameter[kb.ids.slotType][kb.ids.value]);
        });
    }
    stackFrame.set(kb.ids.input, inputIndex);

    if (method.struct_()[kb.ids.subTypes][kb.ids.index].has(kb.ids.localVars)) {
        CellI& localVarsList  = method.struct_()[kb.ids.subTypes][kb.ids.index][kb.ids.localVars][kb.ids.value][kb.ids.slots][kb.ids.list];
        Index& localVarsIndex = *new Index(kb /*, method.struct_()[kb.ids.subTypes][kb.ids.index][kb.ids.localVars][kb.ids.value] */);
        if (method.struct_()[kb.ids.subTypes][kb.ids.index].has(kb.ids.localVars)) {
            Visitor::visitList(localVarsList, [this, &localVarsIndex](CellI& slot, int, bool& stop) {
                localVarsIndex.set(slot[kb.ids.slotRole], *new Object(kb, kb.std.op.Var));
            });
            stackFrame.set(kb.ids.localVars, localVarsIndex);
        }
    }
    if (method.struct_()[kb.ids.subTypes][kb.ids.index].has(kb.ids.returnType)) {
        CellI& returnVar = *new Object(kb, kb.std.op.Var);
        returnVar.set(kb.ids.valueType, method.struct_()[kb.ids.subTypes][kb.ids.index][kb.ids.returnType][kb.ids.value]);
        stackFrame.set(kb.ids.output, returnVar);
    }

    CellI& newStackListItem = *new Object(kb, kb.std.ListItem);
    newStackListItem.set(kb.ids.value, stackFrame);
    newStackListItem.set(kb.ids.previous, stack);
    stack.set(kb.ids.next, newStackListItem);

    CellI* oldStackItem = method.has(kb.ids.stack) ? &method[kb.ids.stack] : nullptr;
    method.set(kb.ids.stack, newStackListItem);

    method();

    if (oldStackItem) {
        method.set(kb.ids.stack, *oldStackItem);
    }
    stack.erase(kb.ids.next);
    if (stackFrame.has(kb.ids.output)) {
        set(kb.ids.value, stackFrame[kb.ids.output][kb.ids.value]);
        delete &stackFrame[kb.ids.output];
    }
    delete &inputIndex;
    if (stackFrame.has(kb.ids.localVars)) {
        CellI& localVarsList  = method.struct_()[kb.ids.subTypes][kb.ids.index][kb.ids.localVars][kb.ids.value][kb.ids.slots][kb.ids.list];
        CellI& localVarsIndex = stackFrame[kb.ids.localVars];
        Visitor::visitList(localVarsList, [this, &localVarsIndex](CellI& slot, int, bool& stop) {
            delete &localVarsIndex[slot[kb.ids.slotRole]];
        });
        delete &localVarsIndex;
    }
    delete &newStackListItem;
    delete &stackFrame;
}

void Object::opFunction()
{
    if (has(kb.ids.op)) {
        CellI& op                       = get(kb.ids.op);
        const std::string& functionName = label();

        CellI& stackBefore          = get(kb.ids.stack);
        CellI& stackFrameBefore     = get(kb.ids.stack)[kb.ids.value];
        CellI& stackFrameBeforeSelf = get(kb.ids.stack)[kb.ids.value][kb.ids.input][kb.ids.self];
        if (s_debugFunctionCalls) {
            printIndent();
            s_indent++;
            std::cout << label() << std::endl;
        }
        op();
        if (s_debugFunctionCalls) {
            s_indent--;
        }
        CellI& stackAfter      = get(kb.ids.stack);
        CellI& stackFrameAfter = get(kb.ids.stack)[kb.ids.value];
    }
}

void Object::opReturn()
{
    if (has(kb.ids.result)) {
        CellI& result = get(kb.ids.result);
        result();
    }
}

// compare
void Object::opSame()
{
    CellI& inputLhs = get(kb.ids.lhs);
    inputLhs();
    CellI* lhs = &inputLhs[kb.ids.value];

    CellI& inputRhs = get(kb.ids.rhs);
    inputRhs();
    CellI* rhs = &inputRhs[kb.ids.value];

    set(kb.ids.value, kb.toKbBool(lhs == rhs));
}

void Object::opNotSame()
{
    CellI& inputLhs = get(kb.ids.lhs);
    inputLhs();
    CellI* lhs = &inputLhs[kb.ids.value];

    CellI& inputRhs = get(kb.ids.rhs);
    inputRhs();
    CellI* rhs = &inputRhs[kb.ids.value];

    set(kb.ids.value, kb.toKbBool(lhs != rhs));
}

void Object::opEqual()
{
    CellI& inputLhs = get(kb.ids.lhs);
    inputLhs();
    CellI& lhs = inputLhs[kb.ids.value];

    CellI& inputRhs = get(kb.ids.rhs);
    inputRhs();
    CellI& rhs = inputRhs[kb.ids.value];

    set(kb.ids.value, kb.toKbBool(lhs == rhs));
}

void Object::opNotEqual()
{
    CellI& inputLhs = get(kb.ids.lhs);
    inputLhs();
    CellI& lhs = inputLhs[kb.ids.value];

    CellI& inputRhs = get(kb.ids.rhs);
    inputRhs();
    CellI& rhs = inputRhs[kb.ids.value];

    set(kb.ids.value, kb.toKbBool(lhs != rhs));
}

void Object::opLessThan()
{
    CellI& inputLhs = get(kb.ids.lhs);
    inputLhs();
    int lhs = static_cast<Number&>(inputLhs[kb.ids.value]).value();

    CellI& inputRhs = get(kb.ids.rhs);
    inputRhs();
    int rhs = static_cast<Number&>(inputRhs[kb.ids.value]).value();

    set(kb.ids.value, lhs < rhs ? kb.boolean.true_ : kb.boolean.false_);
}

void Object::opLessThanOrEqual()
{
    CellI& inputLhs = get(kb.ids.lhs);
    inputLhs();
    int lhs = static_cast<Number&>(inputLhs[kb.ids.value]).value();

    CellI& inputRhs = get(kb.ids.rhs);
    inputRhs();
    int rhs = static_cast<Number&>(inputRhs[kb.ids.value]).value();

    set(kb.ids.value, lhs <= rhs ? kb.boolean.true_ : kb.boolean.false_);
}

void Object::opGreaterThan()
{
    CellI& inputLhs = get(kb.ids.lhs);
    inputLhs();
    int lhs = static_cast<Number&>(inputLhs[kb.ids.value]).value();

    CellI& inputRhs = get(kb.ids.rhs);
    inputRhs();
    int rhs = static_cast<Number&>(inputRhs[kb.ids.value]).value();

    set(kb.ids.value, lhs > rhs ? kb.boolean.true_ : kb.boolean.false_);
}

void Object::opGreaterThanOrEqual()
{
    CellI& inputLhs = get(kb.ids.lhs);
    inputLhs();
    int lhs = static_cast<Number&>(inputLhs[kb.ids.value]).value();

    CellI& inputRhs = get(kb.ids.rhs);
    inputRhs();
    int rhs = static_cast<Number&>(inputRhs[kb.ids.value]).value();

    set(kb.ids.value, lhs >= rhs ? kb.boolean.true_ : kb.boolean.false_);
}

// logic
void Object::opAnd()
{
    CellI& inputLhs = get(kb.ids.lhs);
    inputLhs();
    bool lhs = &inputLhs[kb.ids.value] == &kb.boolean.true_;
    // shortcut, if the left hand side already false we don't evaluate the right hand side
    if (lhs) {
        CellI& inputRhs = get(kb.ids.rhs);
        inputRhs();
        bool rhs = &inputRhs[kb.ids.value] == &kb.boolean.true_;
        set(kb.ids.value, kb.toKbBool(lhs && rhs));
    } else {
        set(kb.ids.value, kb.toKbBool(false));
    }
}

void Object::opOr()
{
    CellI& inputLhs = get(kb.ids.lhs);
    inputLhs();
    bool lhs = &inputLhs[kb.ids.value] == &kb.boolean.true_;

    CellI& inputRhs = get(kb.ids.rhs);
    inputRhs();
    bool rhs = &inputRhs[kb.ids.value] == &kb.boolean.true_;

    set(kb.ids.value, kb.toKbBool(lhs || rhs));
}

void Object::opNot()
{
    CellI& input = get(kb.ids.input);
    input();
    bool res = &input[kb.ids.value] == &kb.boolean.true_;
    set(kb.ids.value, kb.toKbBool(!res));
}

// branching
void Object::opIf()
{
    CellI& inputCondition = get(kb.ids.condition);
    inputCondition();
    set(kb.ids.status, kb.ids.process);
    CellI* branchPtr = nullptr;
    bool condition   = &inputCondition[kb.ids.value] == &kb.boolean.true_;
    if (condition) {
        branchPtr = &get(kb.ids.then);
    } else {
        if (missing(kb.ids.else_)) {
            return;
        }
        branchPtr = &get(kb.ids.else_);
    }
    CellI& branch = *branchPtr;
    branch();
    if (&branch.struct_() == &kb.std.op.Return) {
        set(kb.ids.status, kb.ids.return_);
    } else if (branch.has(kb.ids.status)) {
        set(kb.ids.status, branch[kb.ids.status]);
    }
}

void Object::opDo()
{
    bool condition = false;
    set(kb.ids.status, kb.ids.process);
    do {
        CellI& statement      = get(kb.ids.statement);
        CellI& inputCondition = get(kb.ids.condition);
        statement();
        if (&statement.struct_() == &kb.std.op.Return || (statement.has(kb.ids.status) && &statement[kb.ids.status] == &kb.ids.return_)) {
            set(kb.ids.status, kb.ids.return_);
            return;
        } else if (statement.has(kb.ids.status) && &statement[kb.ids.status] == &kb.ids.break_) {
            set(kb.ids.status, kb.ids.process);
            return;
        } else if (statement.has(kb.ids.status) && &statement[kb.ids.status] == &kb.ids.continue_) {
            set(kb.ids.status, kb.ids.process);
        }
        inputCondition();
        condition = &inputCondition[kb.ids.value] == &kb.boolean.true_;
    } while (condition);
}

void Object::opWhile()
{
    bool condition = false;
    set(kb.ids.status, kb.ids.process);
    CellI& inputCondition = get(kb.ids.condition);
    CellI& statement      = get(kb.ids.statement);
    inputCondition();
    condition = &inputCondition[kb.ids.value] == &kb.boolean.true_;
    while (condition) {
        statement();
        if (&statement.struct_() == &kb.std.op.Return || (statement.has(kb.ids.status) && &statement[kb.ids.status] == &kb.ids.return_)) {
            set(kb.ids.status, kb.ids.return_);
            return;
        } else if (statement.has(kb.ids.status) && &statement[kb.ids.status] == &kb.ids.break_) {
            set(kb.ids.status, kb.ids.process);
            return;
        } else if (statement.has(kb.ids.status) && &statement[kb.ids.status] == &kb.ids.continue_) {
            set(kb.ids.status, kb.ids.process);
        }
        inputCondition();
        condition = &inputCondition[kb.ids.value] == &kb.boolean.true_;
    };
}

void Object::opBlock()
{
    CellI& opsList        = get(kb.ids.ops);
    CellI* opsListItemPtr = opsList.has(kb.ids.first) ? &opsList[kb.ids.first] : nullptr;
    while (opsListItemPtr) {
        CellI& opsListItem = *opsListItemPtr;
        CellI& op          = opsListItem[kb.ids.value];
        bool stop          = false;

        if (&op.struct_() == &kb.std.op.Return) {
            op();
            set(kb.ids.status, kb.ids.return_);
            break;
        }
        set(kb.ids.status, kb.ids.process);
        op();
        if (&(*this)[kb.ids.status] == &kb.ids.continue_) {
            break;
        }
        if (&(*this)[kb.ids.status] == &kb.ids.continue_ || &(*this)[kb.ids.status] == &kb.ids.break_) {
            break;
        }
        if (op.has(kb.ids.status)) {
            if (&op[kb.ids.status] == &kb.ids.return_ || &op[kb.ids.status] == &kb.ids.continue_ || &op[kb.ids.status] == &kb.ids.break_) {
                set(kb.ids.status, op[kb.ids.status]);
                break;
            }
        }

        opsListItemPtr = opsListItem.has(kb.ids.next) ? &opsListItem[kb.ids.next] : nullptr;
    }
}

// math
void Object::opAdd()
{
    CellI& inputLhs = get(kb.ids.lhs);
    inputLhs();
    int lhs = static_cast<Number&>(inputLhs[kb.ids.value]).value();

    CellI& inputRhs = get(kb.ids.rhs);
    inputRhs();
    int rhs = static_cast<Number&>(inputRhs[kb.ids.value]).value();

    set(kb.ids.value, kb.pools.numbers.get(lhs + rhs));
}

void Object::opSubtract()
{
    CellI& inputLhs = get(kb.ids.lhs);
    inputLhs();
    int lhs = static_cast<Number&>(inputLhs[kb.ids.value]).value();

    CellI& inputRhs = get(kb.ids.rhs);
    inputRhs();
    int rhs = static_cast<Number&>(inputRhs[kb.ids.value]).value();

    set(kb.ids.value, kb.pools.numbers.get(lhs - rhs));
}

void Object::opMultiply()
{
    CellI& inputLhs = get(kb.ids.lhs);
    inputLhs();
    int lhs = static_cast<Number&>(inputLhs[kb.ids.value]).value();

    CellI& inputRhs = get(kb.ids.rhs);
    inputRhs();
    int rhs = static_cast<Number&>(inputRhs[kb.ids.value]).value();

    set(kb.ids.value, kb.pools.numbers.get(lhs * rhs));
}

void Object::opDivide()
{
    CellI& inputLhs = get(kb.ids.lhs);
    inputLhs();
    int lhs = static_cast<Number&>(inputLhs[kb.ids.value]).value();

    CellI& inputRhs = get(kb.ids.rhs);
    inputRhs();
    int rhs = static_cast<Number&>(inputRhs[kb.ids.value]).value();

    set(kb.ids.value, kb.pools.numbers.get(lhs / rhs));
}

CellI& Object::operator[](CellI& role)
{
    auto findIt = m_slots.find(&role);
    if (findIt == m_slots.end())
        throw "No such role!";

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

CellI& Object::method(const std::string& role)
{
    return method(kb.name(role));
}

CellI& Object::method(CellI& role)
{
    CellI& method = getMethod(role);
    method();

    return getFnValue(method);
}

CellI& Object::method(CellI& role, Param param1)
{
    CellI& method = getMethod(role);
    setFnParam(method, param1);
    method();

    return getFnValue(method);
}

CellI& Object::method(CellI& role, Param param1, Param param2)
{
    CellI& method = getMethod(role);
    setFnParam(method, param1);
    setFnParam(method, param2);
    method();

    return getFnValue(method);
}

CellI& Object::method(CellI& role, Param param1, Param param2, Param param3)
{
    CellI& method = getMethod(role);
    setFnParam(method, param1);
    setFnParam(method, param2);
    setFnParam(method, param3);
    method();

    return getFnValue(method);
}

CellI& Object::method(CellI& role, Param param1, Param param2, Param param3, Param param4)
{
    CellI& method = getMethod(role);
    setFnParam(method, param1);
    setFnParam(method, param2);
    setFnParam(method, param3);
    setFnParam(method, param4);
    method();

    return getFnValue(method);
}

CellI& Object::smethod(CellI& role)
{
    CellI& method = getStaticMethod(role);
    method();

    return getFnValue(method);
}

CellI& Object::smethod(CellI& role, Param param1)
{
    CellI& method = getStaticMethod(role);
    setFnParam(method, param1);
    method();

    return getFnValue(method);
}

CellI& Object::smethod(CellI& role, Param param1, Param param2)
{
    CellI& method = getStaticMethod(role);
    setFnParam(method, param1);
    setFnParam(method, param2);
    method();

    return getFnValue(method);
}

CellI& Object::smethod(CellI& role, Param param1, Param param2, Param param3)
{
    CellI& method = getStaticMethod(role);
    setFnParam(method, param1);
    setFnParam(method, param2);
    setFnParam(method, param3);
    method();

    return getFnValue(method);
}

CellI& Object::smethod(CellI& role, Param param1, Param param2, Param param3, Param param4)
{
    CellI& method = getStaticMethod(role);
    setFnParam(method, param1);
    setFnParam(method, param2);
    setFnParam(method, param3);
    setFnParam(method, param4);
    method();

    return getFnValue(method);
}

bool Object::hasMethod(CellI& role)
{
    return struct_().has(kb.ids.methods) && struct_()[kb.ids.methods].has(kb.ids.index) && struct_()[kb.ids.methods][kb.ids.index].has(role);
}

CellI& Object::getMethod(CellI& role)
{
    resetIndent();
    if (struct_().has(kb.ids.methods)) {
        CellI& methodsIndex = struct_()[kb.ids.methods][kb.ids.index];
        if (methodsIndex.has(role)) {
            CellI& method = methodsIndex[role][kb.ids.value];
            createStack(method);
            initLocalVars(method);
            setSelf(method);
            return method;
        }
    }

    throw "No such method!";
}

CellI& Object::getStaticMethod(CellI& role)
{
    if (has(kb.ids.methods)) {
        CellI& methodsIndex = (*this)[kb.ids.methods][kb.ids.index];
        if (methodsIndex.has(role)) {
            CellI& method = methodsIndex[role][kb.ids.value];
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

    if (method.struct_()[kb.ids.subTypes][kb.ids.index].has(kb.ids.returnType)) {
        Object& outputIndex = *new Object(kb, kb.std.Index, "ResultIndex");
        stackFrame.set(kb.ids.output, outputIndex);
    }
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
    if (method.struct_()["subTypes"]["index"].has("returnType")) {
        CellI* outputIndex = &(*stackFrame)["output"];
        delete outputIndex;
    }
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
        auto& slotRole   = slot[kb.ids.slotRole];
        Object& localVar = *new Object(kb, kb.std.op.Var, fmt::format("var {}", slotRole.label()));
        localVar.set(kb.ids.valueType, slot[kb.ids.slotType]);
        localVarsIndex.set(slotRole, localVar);
    });
}

CellI& Object::getFnValue(CellI& method)
{
    if (method.struct_()[kb.ids.subTypes][kb.ids.index].has(kb.ids.returnType)) {
        return method[kb.ids.stack][kb.ids.value][kb.ids.output][kb.ids.value];
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
        if (inputsIndex.has(param.role)) {
            fn[kb.ids.stack][kb.ids.value][kb.ids.input].set(param.role, param.value);
        } else {
            throw "No such param!";
        }
    }
}

#pragma endregion
#pragma region List::Item
// ============================================================================
List::Item::Item(brain::Brain& kb, List& list, CellI& value) :
    CellI(kb),
    m_list(list),
    m_value(value)
{
}

bool List::Item::has(CellI& role)
{
    if (&role == &kb.ids.struct_ || &role == &kb.ids.value) {
        return true;
    }
    if (&role == &kb.ids.previous && m_previous) {
        return true;
    }
    if (&role == &kb.ids.next && m_next) {
        return true;
    }

    return false;
}

void List::Item::set(CellI& role, CellI& value)
{
    // Do nothing
}

void List::Item::erase(CellI& role)
{
    if (&role == &kb.ids.next) {
        m_next = nullptr;
    }
    if (&role == &kb.ids.previous) {
        m_previous = nullptr;
    }
    throw "No such role!";
}

void List::Item::operator()()
{
    // Do nothing
}

CellI& List::Item::operator[](CellI& role)
{
    if (&role == &kb.ids.struct_) {
        if (!m_selfType) {
            m_selfType = &kb.getStruct(kb.templateId("std::ListItem", kb.ids.valueType, m_list.m_valueType));
        }
        return *m_selfType;
    }
    if (&role == &kb.ids.previous) {
        if (m_previous)
            return *m_previous;
        else
            throw "No such role!";
    }
    if (&role == &kb.ids.next) {
        if (m_next)
            return *m_next;
        else
            throw "No such role!";
    }
    if (&role == &kb.ids.value) {
        return m_value;
    }

    throw "No such role!";
}

void List::Item::accept(Visitor& visitor)
{
    visitor.visit(*this);
}
#pragma endregion
#pragma region List
// ============================================================================
List::List(brain::Brain& kb, CellI& valueType, const std::string& label) :
    CellI(kb, label),
    m_valueType(valueType)
{
}

bool List::has(CellI& role)
{
    if (&role == &kb.ids.struct_ || &role == &kb.ids.size) {
        return true;
    }
    if (&role == &kb.ids.first && m_firstItem) {
        return true;
    }
    if (&role == &kb.ids.last && m_lastItem) {
        return true;
    }

    return false;
}

void List::set(CellI& role, CellI& value)
{
    throw "Not supported";
}

void List::erase(CellI& role)
{
    throw "Not supported";
}

void List::operator()()
{
    // Do nothing, this is a data cell
}

CellI& List::operator[](CellI& role)
{
    if (&role == &kb.ids.struct_) {
        if (!m_selfType) {
            m_selfType = &kb.getStruct(kb.templateId("std::List", kb.ids.valueType, m_valueType));
        }
        return *m_selfType;
    }
    if (&role == &kb.ids.first) {
        return *m_firstItem;
    }
    if (&role == &kb.ids.last) {
        return *m_lastItem;
    }
    if (&role == &kb.ids.size) {
        int size = (int)m_size;

        return kb.pools.numbers.get(size);
    }

    throw "No such role!";
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
#pragma endregion
#pragma region Struct
// ============================================================================
Struct::Struct(brain::Brain& kb, const std::string& label) :
    CellI(kb, label),
    m_slots(*new Map(kb, kb.std.Cell, kb.std.Slot))
{
}

Struct::Struct(brain::Brain& kb, WithRecursiveType recursiveType, const std::string& label) :
    CellI(kb, label),
    m_slots(*new Map(kb, kb.std.Cell, kb.std.Slot, *this))
{
}

bool Struct::has(CellI& role)
{
    if (&role == &kb.ids.struct_) {
        return true;
    }
    if (&role == &kb.ids.name) {
        return true;
    }
    if (&role == &kb.ids.slots) {
        return true;
    }
    if (&role == &kb.ids.subTypes) {
        return m_subTypes;
    }
    if (&role == &kb.ids.memberOf) {
        return m_memberOf;
    }
    if (&role == &kb.ids.asts) {
        return m_asts;
    }
    if (&role == &kb.ids.methods) {
        return m_methods;
    }

    return false;
}

void Struct::set(CellI& key, CellI& value)
{
    throw "Not supported";
}

void Struct::erase(CellI& role)
{
    throw "Not supported";
}

void Struct::operator()()
{
    // Do nothing, this is a data cell
}

CellI& Struct::operator[](CellI& role)
{
    if (&role == &kb.ids.struct_) {
        return kb.std.Struct;
    }
    if (&role == &kb.ids.name) {
        if (m_name) {
            return *m_name;
        } else {
            m_name = &kb.name(label());
            return *m_name;
        }
    }
    if (&role == &kb.ids.slots) {
        return m_slots;
    }
    if (&role == &kb.ids.subTypes) {
        return *m_subTypes;
    }
    if (&role == &kb.ids.memberOf) {
        return *m_memberOf;
    }
    if (&role == &kb.ids.asts) {
        return *m_asts;
    }
    if (&role == &kb.ids.methods) {
        return *m_methods;
    }

    throw "No such role!";
}

void Struct::addSlot(CellI& role, CellI& slot)
{
    m_slots.add(role, slot);
}

bool Struct::hasSlot(CellI& role)
{
    return m_slots.hasKey(role);
}

void Struct::removeSlot(CellI& role)
{
    m_slots.remove(role);
}

void Struct::accept(Visitor& visitor)
{
    visitor.visit(*this);
}
#pragma endregion
#pragma region Index
// ============================================================================
Index::Index(brain::Brain& kb, const std::string& label) :
    CellI(kb, label),
    m_type(new Struct(kb, Struct::WithRecursiveType::Yes))
{
}

Index::Index(brain::Brain& kb, Struct& indexType, const std::string& label) :
    CellI(kb, label),
    m_type(&indexType),
    m_recursiveType(true)
{
}

bool Index::has(CellI& role)
{
    if (&role == &kb.ids.struct_) {
        return true;
    }
    if (m_slots.find(&role) != m_slots.end()) {
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

void Index::erase(CellI& role)
{
    if (!m_type->hasSlot(role)) {
        return;
    }
    m_slots.erase(&role);
    m_type->removeSlot(role);
}

void Index::operator()()
{
    // Do nothing, this is a data cell
}

CellI& Index::operator[](CellI& role)
{
    if (&role == &kb.ids.struct_) {
        return *m_type;
    }
    auto slotIt = m_slots.find(&role);
    if (slotIt != m_slots.end()) {
        return *slotIt->second;
    }

    throw "No such role!";
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
    slot.set(kb.ids.slotRole, key);
    slot.set(kb.ids.slotType, kb.std.Slot);
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
Map::Map(brain::Brain& kb, CellI& keyType, CellI& valueType, const std::string& label) :
    CellI(kb, label),
    m_list(kb, valueType),
    m_index(kb),
    m_keyType(keyType),
    m_valueType(valueType)
{
}

Map::Map(brain::Brain& kb, CellI& keyType, CellI& valueType, Struct& indexType, const std::string& label) :
    CellI(kb, label),
    m_list(kb, valueType),
    m_index(kb, indexType),
    m_keyType(keyType),
    m_valueType(valueType)
{
}

bool Map::has(CellI& role)
{
    if (&role == &kb.ids.struct_) {
        return true;
    }
    if (&role == &kb.ids.list) {
        return true;
    }
    if (&role == &kb.ids.index) {
        return true;
    }
    if (&role == &kb.ids.size) {
        return true;
    }

    return false;
}

void Map::set(CellI& key, CellI& value)
{
    throw "Not supported";
}

void Map::erase(CellI& role)
{
    throw "Not supported";
}

void Map::operator()()
{
    // Do nothing, this is a data cell
}

CellI& Map::operator[](CellI& role)
{
    if (&role == &kb.ids.struct_) {
        if (!m_selfType) {
            m_selfType = &kb.getStruct(kb.templateId("std::Map", kb.ids.keyType, m_keyType, kb.ids.valueType, m_valueType));
        }
        return *m_selfType;
    }
    if (&role == &kb.ids.list) {
        return m_list;
    }
    if (&role == &kb.ids.index) {
        return m_index;
    }
    if (&role == &kb.ids.size) {
        return kb.pools.numbers.get(m_size);
    }

    throw "No such role!";
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
    throw "No such role!";
}

void Map::add(CellI& key, CellI& value)
{
    if (&key == &kb.ids.struct_) {
        throw "id.type can not be stored in a map!";
    }
    if (m_index.has(key)) {
        throw "A value already registered with this role";
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
TrieMap::TrieMap(brain::Brain& kb, CellI& keyType, CellI& valueType, const std::string& label) :
    CellI(kb, label),
    m_list(kb, valueType),
    m_rootNode(kb, kb.std.TrieMapNode, "TrieNode_Root"),
    m_keyType(keyType),
    m_valueType(valueType)
{
}

bool TrieMap::has(CellI& role)
{
    if (&role == &kb.ids.struct_) {
        return true;
    }
    if (&role == &kb.ids.list) {
        return true;
    }
    if (&role == &kb.ids.index) {
        return true;
    }
    if (&role == &kb.ids.size) {
        return true;
    }

    return false;
}

void TrieMap::set(CellI& key, CellI& value)
{
    throw "Not supported";
}

void TrieMap::erase(CellI& role)
{
    throw "Not supported";
}

void TrieMap::operator()()
{
    // Do nothing, this is a data cell
}

CellI& TrieMap::operator[](CellI& role)
{
    if (&role == &kb.ids.struct_) {
        if (!m_selfType) {
            m_selfType = &kb.getStruct(kb.templateId("std::TrieMap", kb.ids.keyType, m_keyType, kb.ids.valueType, m_valueType));
        }
        return *m_selfType;
    }
    if (&role == &kb.ids.list) {
        return m_list;
    }
    if (&role == &kb.ids.rootNode) {
        return m_rootNode;
    }
    if (&role == &kb.ids.size) {
        return kb.pools.numbers.get(m_size);
    }

    throw "No such role!";
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
Set::Set(brain::Brain& kb, CellI& valueType, const std::string& label) :
    CellI(kb, label),
    m_valueType(valueType),
    m_index(kb)
{
}

bool Set::has(CellI& role)
{
    if (&role == &kb.ids.struct_ || &role == &kb.ids.size) {
        return true;
    }
    if (&role == &kb.ids.index) {
        return true;
    }

    return false;
}

void Set::set(CellI& role, CellI& value)
{
    throw "Not supported";
}

void Set::erase(CellI& role)
{
    throw "Not supported";
}

void Set::operator()()
{
    // Do nothing, this is a data cell
}

CellI& Set::operator[](CellI& role)
{
    if (&role == &kb.ids.struct_) {
        if (!m_selfType) {
            m_selfType = &kb.getStruct(kb.templateId("std::Set", kb.ids.valueType, m_valueType));
        }
        return *m_selfType;
    }
    if (&role == &kb.ids.index) {
        return m_index;
    }
    if (&role == &kb.ids.size) {
        int size = (int)m_size;

        return kb.pools.numbers.get(size);
    }

    throw "No such role!";
}

bool Set::contains(CellI& key)
{
    return m_index.has(key);
}

void Set::add(CellI& value)
{
    if (m_index.has(value)) {
        return;
//        throw "A value already registered with this role";
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
    return m_index["struct"]["slots"]["list"]["first"]["value"]["slotRole"];
}

void Set::accept(Visitor& visitor)
{
    visitor.visit(*this);
}
#pragma endregion
#pragma region Number
// ============================================================================
Number::Number(brain::Brain& kb, int value) :
    CellI(kb),
    m_value(value)
{
}

bool Number::has(CellI& role)
{
    if (&role == &kb.ids.struct_ || &role == &kb.ids.value) {
        return true;
    }
    if (&role == &kb.numbers.sign) {
        return m_value != 0;
    }

    return false;
}

void Number::set(CellI& role, CellI& value)
{
    throw "Changing a hybrid number cell is not possible!";
}

void Number::erase(CellI& role)
{
    throw "Changing a hybrid number cell is not possible!";
}

void Number::operator()()
{
    // Do nothing, this is a data cell
}

CellI& Number::operator[](CellI& role)
{
    if (&role == &kb.ids.struct_) {
        return kb.std.Number;
    }

    if (&role == &kb.numbers.sign && m_value != 0) {
        return m_value > 0 ? kb.numbers.positive : kb.numbers.negative;
    }

    if (&role == &kb.ids.value) {
        if (m_digits.empty()) {
            calculateDigits();
            m_digitsList.reset(new List(kb, m_digits));
        }

        return *m_digitsList;
    }

    throw "No such role!";
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
#pragma endregion
#pragma region String
// ============================================================================
String::String(brain::Brain& kb, const std::string& str) :
    CellI(kb),
    m_value(str)
{
}

String::String(brain::Brain& kb, List& list, const std::string& str) :
    CellI(kb),
    m_value(str),
    m_charactersListPtr(&list)
{
}

bool String::has(CellI& role)
{
    if (&role == &kb.ids.struct_ || &role == &kb.ids.value) {
        return true;
    }
    return false;
}

void String::set(CellI& role, CellI& value)
{
    throw "Changing a hybrid string cell is not possible!";
}

void String::erase(CellI& role)
{
    throw "Changing a hybrid string cell is not possible!";
}

void String::operator()()
{
    // Do nothing, this is a data cell
}

CellI& String::operator[](CellI& role)
{
    if (&role == &kb.ids.struct_) {
        return kb.std.String;
    } else if (&role == &kb.ids.value) {
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
        throw "No such role!";
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
#pragma region Color
// ============================================================================
Color::Color(brain::Brain& kb, const input::Color& inputColor) :
    CellI(kb),
    m_inputColor(inputColor)
{
}

bool Color::has(CellI& role)
{
    if (&role == &kb.ids.struct_) {
        return true;
    }
    if (&role == &kb.ids.red || &role == &kb.ids.green || &role == &kb.ids.blue) {
        return true;
    }

    return false;
}

void Color::set(CellI& role, CellI& value)
{
    throw "Changing a hybrid color cell is not possible!";
}

void Color::erase(CellI& role)
{
    throw "Changing a hybrid color cell is not possible!";
}

void Color::operator()()
{
    // Do nothing
}

CellI& Color::operator[](CellI& role)
{
    if (&role == &kb.ids.struct_) {
        return kb.std.Color;
    }
    if (&role == &kb.ids.red) {
        return kb.pools.numbers.get(m_inputColor.m_red);
    }
    if (&role == &kb.ids.green) {
        return kb.pools.numbers.get(m_inputColor.m_green);
    }
    if (&role == &kb.ids.blue) {
        return kb.pools.numbers.get(m_inputColor.m_blue);
    }

    throw "No such role!";
}

void Color::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

const input::Color& Color::color() const
{
    return m_inputColor;
}
#pragma endregion
#pragma region Pixel
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
    if (&role == &kb.ids.struct_) {
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
    if (&role == &kb.ids.color) {
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
    throw "Changing a hybrid pixel cell is not possible!";
}

void Pixel::erase(CellI& role)
{
    throw "Changing a hybrid pixel cell is not possible!";
}

void Pixel::operator()()
{
    // Do nothing
}

CellI& Pixel::operator[](CellI& role)
{
    if (&role == &kb.ids.struct_) {
        return kb.std.Pixel;
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
    if (&role == &kb.ids.color) {
        return m_color;
    }
    if (&role == &kb.coordinates.x) {
        return m_x;
    }
    if (&role == &kb.coordinates.y) {
        return m_y;
    }

    throw "No such role!";
}

void Pixel::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

const input::Color& Pixel::color() const
{
    return m_inputColor;
}
#pragma endregion
#pragma region Picture
// ============================================================================
Picture::Picture(brain::Brain& kb, input::Grid& picture) :
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
    m_pixelsList.reset(new List(kb, m_pixels));
}

bool Picture::has(CellI& role)
{
    if (&role == &kb.ids.struct_ || &role == &kb.ids.width || &role == &kb.ids.height || &role == &kb.ids.pixels) {
        return true;
    }

    return false;
}

void Picture::set(CellI& role, CellI& value)
{
    throw "Changing a hybrid picture cell is not possible!";
}

void Picture::erase(CellI& role)
{
    throw "Changing a hybrid picture cell is not possible!";
}

void Picture::operator()()
{
}

CellI& Picture::operator[](CellI& role)
{
    if (&role == &kb.ids.struct_) {
        return kb.std.Grid;
    }
    if (&role == &kb.ids.width) {
        return m_widthCell;
    }
    if (&role == &kb.ids.height) {
        return m_heightCell;
    }
    if (&role == &kb.ids.pixels) {
        return *m_pixelsList;
    }

    throw "No such role!";
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
#pragma endregion

namespace arc {
#pragma region Pixel
// ============================================================================
Pixel::Pixel(brain::Brain& kb, int x, int y, int arcColor, Grid& grid) :
    CellI(kb),
    m_x(kb.pools.numbers.get(x)),
    m_y(kb.pools.numbers.get(y)),
    m_arcColor(kb.pools.numbers.get(arcColor)),
    m_grid(grid)
{
}

bool Pixel::has(CellI& role)
{
    if (&role == &kb.ids.struct_) {
        return true;
    }
    if (&role == &kb.directions.up && m_grid.hasPixel(m_x.value(), m_y.value() - 1)) {
        return true;
    }
    if (&role == &kb.directions.down && m_grid.hasPixel(m_x.value(), m_y.value() + 1)) {
        return true;
    }
    if (&role == &kb.directions.left && m_grid.hasPixel(m_x.value() - 1, m_y.value())) {
        return true;
    }
    if (&role == &kb.directions.right && m_grid.hasPixel(m_x.value() + 1, m_y.value())) {
        return true;
    }
    if (&role == &kb.ids.color) {
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
    throw "Changing a hybrid pixel cell is not possible!";
}

void Pixel::erase(CellI& role)
{
    throw "Changing a hybrid pixel cell is not possible!";
}

void Pixel::operator()()
{
    // Do nothing
}

CellI& Pixel::operator[](CellI& role)
{
    if (&role == &kb.ids.struct_) {
        return kb.std.Pixel;
    }
    if (&role == &kb.directions.up && m_grid.hasPixel(m_x.value(), m_y.value() - 1)) {
        return m_grid.getPixel(m_x.value(), m_y.value() - 1);
    }
    if (&role == &kb.directions.down && m_grid.hasPixel(m_x.value(), m_y.value() + 1)) {
        return m_grid.getPixel(m_x.value(), m_y.value() + 1);
    }
    if (&role == &kb.directions.left && m_grid.hasPixel(m_x.value() - 1, m_y.value())) {
        return m_grid.getPixel(m_x.value() - 1, m_y.value());
    }
    if (&role == &kb.directions.right && m_grid.hasPixel(m_x.value() + 1, m_y.value())) {
        return m_grid.getPixel(m_x.value() + 1, m_y.value());
    }
    if (&role == &kb.ids.color) {
        return m_arcColor;
    }
    if (&role == &kb.coordinates.x) {
        return m_x;
    }
    if (&role == &kb.coordinates.y) {
        return m_y;
    }

    throw "No such role!";
}

void Pixel::accept(Visitor& visitor)
{
//    visitor.visit(*this);
}

const int Pixel::color() const
{
    return m_arcColor.value();
}
#pragma endregion
#pragma region Picture
int getArcColorId(const input::Color& color)
{
    static input::Color black(0x00, 0x00, 0x00);
    static input::Color blue(0x00, 0x74, 0xD9);
    static input::Color red(0xFF, 0x41, 0x36);
    static input::Color green(0x2E, 0xCC, 0x40);
    static input::Color yellow(0xFF, 0xDC, 0x00);
    static input::Color grey(0xAA, 0xAA, 0xAA);
    static input::Color fuschia(0xF0, 0x12, 0xBE);
    static input::Color orange(0xFF, 0x85, 0x1B);
    static input::Color teal(0x7F, 0xDB, 0xFF);
    static input::Color brown(0x87, 0x0C, 0x25);

    static std::map<input::Color, infocell::arc::Colors> arcColorNames = {
        { black, infocell::arc::Colors::black },
        { blue, infocell::arc::Colors::blue },
        { red, infocell::arc::Colors::red },
        { green, infocell::arc::Colors::green },
        { yellow, infocell::arc::Colors::yellow },
        { grey, infocell::arc::Colors::grey },
        { fuschia, infocell::arc::Colors::fuschia },
        { orange, infocell::arc::Colors::orange },
        { teal, infocell::arc::Colors::teal },
        { brown, infocell::arc::Colors::brown }
    };

    return static_cast<int>(arcColorNames.find({ color.red(), color.green(), color.blue() })->second);
}
// ============================================================================
Grid::Grid(brain::Brain& kb, input::Grid& picture) :
    CellI(kb, picture.label()),
    m_width(picture.width()),
    m_height(picture.height()),
    m_widthCell(kb.pools.numbers.get(m_width)),
    m_heightCell(kb.pools.numbers.get(m_height)),
    m_pixelsMap(kb, kb.std.Cell, kb.std.Pixel, "PixelsMap")
{
    const int gridSize = m_height * m_width;

    m_pixels.clear();
    m_pixels.reserve(gridSize);

    int x = 0;
    int y = 0;

    for (const input::Color& color : picture.pixels()) {
        int colorId = getArcColorId(color);
        m_pixels.emplace_back(kb, x, y, colorId, *this);
        List pixelContent(kb, kb.std.Pixel);
        pixelContent.add(kb.pools.numbers.get(x));
        pixelContent.add(kb.pools.numbers.get(y));
        m_pixelsMap.add(pixelContent, kb.pools.numbers.get(colorId));
        x = x + 1;
        if (x == m_width) {
            x = 0;
            y += 1;
        }
    }

    m_pixelsList.reset(new List(kb, m_pixels));
}

bool Grid::has(CellI& role)
{
    if (&role == &kb.ids.struct_ || &role == &kb.ids.width || &role == &kb.ids.height || &role == &kb.ids.pixels || &role == &kb.ids.pixelsMap) {
        return true;
    }

    return false;
}

void Grid::set(CellI& role, CellI& value)
{
    throw "Changing a hybrid picture cell is not possible!";
}

void Grid::erase(CellI& role)
{
    throw "Changing a hybrid picture cell is not possible!";
}

void Grid::operator()()
{
}

CellI& Grid::operator[](CellI& role)
{
    if (&role == &kb.ids.struct_) {
        return kb.std.Grid;
    }
    if (&role == &kb.ids.width) {
        return m_widthCell;
    }
    if (&role == &kb.ids.height) {
        return m_heightCell;
    }
    if (&role == &kb.ids.pixels) {
        return *m_pixelsList;
    }
    if (&role == &kb.ids.pixelsMap) {
        return m_pixelsMap;
    }

    throw "No such role!";
}

void Grid::accept(Visitor& visitor)
{
//    visitor.visit(*this);
}

Pixel& Grid::getPixel(int x, int y)
{
    return m_pixels[currentIndex(x, y)];
}

const Pixel& Grid::getPixel(int x, int y) const
{
    return m_pixels.at(currentIndex(x, y));
}

int Grid::currentIndex(int x, int y) const
{
    return y * m_width + x;
}

bool Grid::hasPixel(int x, int y) const
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

 x=0, y=4 is 20

 upPixel(4, 4) = 19
 3 * 5 + 4

 */

std::vector<Pixel>& Grid::pixels()
{
    return m_pixels;
}

int Grid::width() const
{
    return m_width;
}

int Grid::height() const
{
    return m_height;
}

Number& Grid::widthCell()
{
    return m_widthCell;
}

Number& Grid::heightCell()
{
    return m_heightCell;
}

#pragma endregion
} // namespace arc

} // namespace hybrid

#if 0
void Visitor::visitList(CellI& list, std::function<void(CellI& value, int i, bool& stop)> visitFn)
{
    brain::Brain& kb = list.kb;
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
        brain::Brain& kb;
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
    brain::Brain& kb = list.kb;
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
    brain::Brain& kb = cell.kb;

    if (&cell.struct_() == &kb.std.Number) {
        visitor.visit(static_cast<Number&>(cell));
        return true;
    }
    if (&cell.struct_() == &kb.std.String) {
        visitor.visit(static_cast<String&>(cell));
        return true;
    }
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

    return false;
}

} // namespace cells
} // namespace infocell
