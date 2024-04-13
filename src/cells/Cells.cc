#include "Cells.h"
#include "Brain.h"

#include <format>
#include <sstream>
#include <utility>

#pragma warning(disable : 4996)
#include <utf8.h>


namespace synth {
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

bool CellI::missing(CellI& role)
{
    return !has(role);
}

CellI& CellI::get(CellI& role)
{
    return (*this)[role];
}

CellI& CellI::type()
{
    return (*this)[kb.id.type];
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
    return &type() == &ptype || (has(kb.id.memberOf) && (*this)[kb.id.memberOf][kb.id.index].has(ptype));
}

bool CellI::isA(CellI& cell, CellI& type) const
{
    return &cell == &type || (cell.has(kb.id.memberOf) && cell[kb.id.memberOf][kb.id.index].has(type));
}

bool CellI::operator==(CellI& rhs)
{
    if (&type() != &rhs.type()) {
        return false;
    }
    CellI& slotList    = type()[kb.id.slots][kb.id.list];
    CellI* slotItemPtr = slotList.has(kb.id.first) ? &slotList[kb.id.first] : nullptr;
    while (slotItemPtr) {
        CellI& slotItem = *slotItemPtr;
        CellI& slot     = slotItem[kb.id.value];
        CellI& role     = slot[kb.id.slotRole];

        bool hasLeftSlot = has(role);
        if (hasLeftSlot != rhs.has(role)) {
            return false;
        }
        if (hasLeftSlot && (&(*this)[role] != &rhs[role])) {
            return false;
        }

        slotItemPtr = slotItem.has(kb.id.next) ? &slotItem[kb.id.next] : nullptr;
    }

    return true;
}

bool CellI::operator!=(CellI& rhs)
{
    return !((*this) == rhs);
}
#pragma endregion
#pragma region Object
int Object::s_indent = 0;
// ============================================================================
Object::Object(brain::Brain& kb, CellI& type, const std::string& label) :
    CellI(kb, label),
    m_type(type)
{
    m_slots[&kb.id.type] = &type;
    if (kb.initPhase() == InitPhase::Init) {
        return;
    }
}

Object::Object(brain::Brain& kb, CellI& type, CellI& constructor, const std::string& label) :
    CellI(kb, label),
    m_type(type)
{
    m_slots[&kb.id.type] = &type;
    getMethod(constructor)();
}

Object::Object(brain::Brain& kb, CellI& type, CellI& constructor, Param param1, const std::string& label) :
    CellI(kb, label),
    m_type(type)
{
    m_slots[&kb.id.type] = &type;

    CellI& method = getMethod(constructor);
    setFnParam(method, param1);
    method();
}

Object::Object(brain::Brain& kb, CellI& type, CellI& constructor, Param param1, Param param2, const std::string& label) :
    CellI(kb, label),
    m_type(type)
{
    m_slots[&kb.id.type] = &type;

    CellI& method = getMethod(constructor);
    setFnParam(method, param1);
    setFnParam(method, param2);
    method();
}

Object::Object(brain::Brain& kb, CellI& type, CellI& constructor, Param param1, Param param2, Param param3, const std::string& label) :
    CellI(kb, label),
    m_type(type)
{
    m_slots[&kb.id.type] = &type;

    CellI& method = getMethod(constructor);
    setFnParam(method, param1);
    setFnParam(method, param2);
    setFnParam(method, param3);
    method();
}


Object::Object(brain::Brain& kb, CellI& type, CellI& constructor, Param param1, Param param2, Param param3, Param param4, const std::string& label) :
    CellI(kb, label),
    m_type(type)
{
    m_slots[&kb.id.type] = &type;

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
    if (!hasMethod(kb.id.destructor)) {
        return;
    }
    destructor();
}

bool Object::has(CellI& role)
{
    if (&role == &kb.id.type)
        return true;

    return m_slots.find(&role) != m_slots.end();
}

void Object::set(CellI& role, CellI& value)
{
    if ((&role == &kb.id.type) && !((&type() == &kb.type.Index) || (type().label() == "Index"))) {
        throw "Type change not allowed.";
    }
    if ((&role == &kb.id.type) && (&type() == &kb.type.Index)) {
        std::cout << "";
    }

    if (kb.initPhase() == InitPhase::Init) {
        m_slots[&role] = &value;
        return;
    }
    auto is = [this](CellI& rhsType) -> bool { return &type() == &rhsType || (type().has(kb.id.memberOf) && type()[kb.id.memberOf][kb.id.index].has(rhsType)); };
    if (is(kb.type.Index) || type().label() == "Index" || type()[kb.id.slots][kb.id.index].has(role)) {
        m_slots[&role] = &value;
    } else {
        throw "The type doesn't contains this role.";
    }
}

void Object::erase(CellI& role)
{
    if (&role == &kb.id.type) {
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
    static bool s_debugFunctionCalls = false;

    if (&m_type == &kb.type.op.Block) {
        Visitor::visitList(get(kb.id.ops), [this](CellI& op, int, bool& stop) {
            if (&op.type() == &kb.type.op.Return) {
                op();
                set(kb.id.status, kb.id.stop);
                stop = true;
                return;
            }
            set(kb.id.status, kb.id.continue_);
            op();
            if (op.has(kb.id.status) && &op[kb.id.status] == &kb.id.stop) {
                set(kb.id.status, kb.id.stop);
                stop = true;
                return;
            }
        });
    } else if (&m_type == &kb.type.op.Return) {
        if (has(kb.id.result)) {
            CellI& result = get(kb.id.result);
            result();
        }
    } else if (&m_type == &kb.type.op.EvalVar) {
        CellI& value = get(kb.id.value)[kb.id.value];
        value();
    } else if (&m_type == &kb.type.op.Function || (m_type.has(kb.id.memberOf) && m_type[kb.id.memberOf][kb.id.index].has(kb.type.op.Function))) {
        if (has(kb.id.op)) {
            CellI& op = get(kb.id.op);
            const std::string& functionName = label();

            CellI& stackBefore          = get(kb.id.stack);
            CellI& stackFrameBefore     = get(kb.id.stack)[kb.id.value];
            CellI& stackFrameBeforeSelf = get(kb.id.stack)[kb.id.value][kb.id.input][kb.id.self];
            if (s_debugFunctionCalls) {
                printIndent();
                s_indent++;
                std::cout << label() << std::endl;
            }
            op();
            if (s_debugFunctionCalls) {
                s_indent--;
            }
            CellI& stackAfter      = get(kb.id.stack);
            CellI& stackFrameAfter = get(kb.id.stack)[kb.id.value];
        }
    } else if (&m_type == &kb.type.op.Delete) {
        CellI& input = get(kb.id.input);
        input();
        CellI* cell = &input[kb.id.value];
        delete cell;
    } else if (&m_type == &kb.type.op.Set) {
        CellI& inputCell = get(kb.id.cell);
        inputCell();
        CellI& cell      = inputCell[kb.id.value];
        CellI& inputRole = get(kb.id.role);
        inputRole();
        CellI& role       = inputRole[kb.id.value];
        CellI& inputValue = get(kb.id.value);
        inputValue();
        CellI& value = inputValue[kb.id.value];
        if (label() == "Call { storeMethod; }") {
//            std::cout << "DDDD storeMethod " << value.label() << std::endl;
        }
        if (label() == "Call { setStackToOld; }") {
//            std::cout << "DDDD setStackToOld " << cell.label() << " to " << &value << std::endl;
        }
        if (label() == "Call { setStackToNew; }") {
//            std::cout << "DDDD setStackToNew " << cell.label() << " to " << &value << std::endl;
        }
        if (value.label().starts_with("fn Map::add")) {
            std::cout << "";
        }
        cell.set(role, value);
    } else if (&m_type == &kb.type.op.Erase) {
        CellI& inputCell = get(kb.id.cell);
        inputCell();
        CellI& cell      = inputCell[kb.id.value];
        CellI& inputRole = get(kb.id.role);
        inputRole();
        CellI& role       = inputRole[kb.id.value];

        cell.erase(role);
    } else if (&m_type == &kb.type.op.If) {
        CellI& inputCondition = get(kb.id.condition);
        inputCondition();
        set(kb.id.status, kb.id.continue_);
        CellI* branchPtr = nullptr;
        bool condition   = &inputCondition[kb.id.value] == &kb.boolean.true_;
        if (condition) {
            branchPtr = &get(kb.id.then);
        } else {
            if (missing(kb.id.else_)) {
                return;
            }
            branchPtr = &get(kb.id.else_);
        }
        CellI& branch = *branchPtr;
        branch();
        if (&branch.type() == &kb.type.op.Return || (branch.has(kb.id.status) && &branch[kb.id.status] == &kb.id.stop)) {
            set(kb.id.status, kb.id.stop);
        }
    } else if (&m_type == &kb.type.op.Do) {
        bool condition = false;
        set(kb.id.status, kb.id.continue_);
        do {
            CellI& statement      = get(kb.id.statement);
            CellI& inputCondition = get(kb.id.condition);
            statement();
            if (&statement.type() == &kb.type.op.Return || (statement.has(kb.id.status) && &statement[kb.id.status] == &kb.id.stop)) {
                set(kb.id.status, kb.id.stop);
                return;
            }
            inputCondition();
            condition = &inputCondition[kb.id.value] == &kb.boolean.true_;
        } while (condition);
    } else if (&m_type == &kb.type.op.While) {
        bool condition = false;
        set(kb.id.status, kb.id.continue_);
        CellI& inputCondition = get(kb.id.condition);
        CellI& statement      = get(kb.id.statement);
        inputCondition();
        condition = &inputCondition[kb.id.value] == &kb.boolean.true_;
        while (condition) {
            statement();
            if (&statement.type() == &kb.type.op.Return || (statement.has(kb.id.status) && &statement[kb.id.status] == &kb.id.stop)) {
                set(kb.id.status, kb.id.stop);
                return;
            }
            inputCondition();
            condition = &inputCondition[kb.id.value] == &kb.boolean.true_;
        };
    } else if (&m_type == &kb.type.op.New) {
        CellI& inputObjectType = get(kb.id.objectType);
        inputObjectType();
        CellI& objectType = inputObjectType[kb.id.value];

        if (&objectType == &kb.type.Number) {
            set(kb.id.value, *new Number(kb));
        } else if (&objectType == &kb.type.String) {
            set(kb.id.value, *new String(kb));
        } else {
            set(kb.id.value, *new Object(kb, objectType));
        }
    } else if (&m_type == &kb.type.op.Same) {
        CellI& inputLhs = get(kb.id.lhs);
        inputLhs();
        CellI* lhs      = &inputLhs[kb.id.value];
        CellI& inputRhs = get(kb.id.rhs);
        inputRhs();
        CellI* rhs = &inputRhs[kb.id.value];
        set(kb.id.value, kb.toKbBool(lhs == rhs));
    } else if (&m_type == &kb.type.op.NotSame) {
        CellI& inputLhs = get(kb.id.lhs);
        inputLhs();
        CellI* lhs      = &inputLhs[kb.id.value];
        CellI& inputRhs = get(kb.id.rhs);
        inputRhs();
        CellI* rhs = &inputRhs[kb.id.value];
        set(kb.id.value, kb.toKbBool(lhs != rhs));
    } else if (&m_type == &kb.type.op.Equal) {
        CellI& inputLhs = get(kb.id.lhs);
        inputLhs();
        CellI& lhs      = inputLhs[kb.id.value];
        CellI& inputRhs = get(kb.id.rhs);
        inputRhs();
        CellI& rhs = inputRhs[kb.id.value];
        set(kb.id.value, kb.toKbBool(lhs == rhs));
    } else if (&m_type == &kb.type.op.NotEqual) {
        CellI& inputLhs = get(kb.id.lhs);
        inputLhs();
        CellI& lhs      = inputLhs[kb.id.value];
        CellI& inputRhs = get(kb.id.rhs);
        inputRhs();
        CellI& rhs = inputRhs[kb.id.value];
        set(kb.id.value, kb.toKbBool(lhs != rhs));
    } else if (&m_type == &kb.type.op.Has) {
        CellI& inputCell = get(kb.id.cell);
        inputCell();
        CellI& cell      = inputCell[kb.id.value];
        CellI& inputRole = get(kb.id.role);
        inputRole();
        CellI& role = inputRole[kb.id.value];
        set(kb.id.value, kb.toKbBool(cell.has(role)));
    } else if (&m_type == &kb.type.op.Missing) {
        CellI& inputCell = get(kb.id.cell);
        inputCell();
        CellI& cell      = inputCell[kb.id.value];
        CellI& inputRole = get(kb.id.role);
        inputRole();
        CellI& role = inputRole[kb.id.value];
        set(kb.id.value, kb.toKbBool(cell.missing(role)));
    } else if (&m_type == &kb.type.op.Get) {
        if (label() == "self") {
            CellI& debugCell1Cell = get(kb.id.cell)[kb.id.cell];
            CellI& debugCell1Role = get(kb.id.cell)[kb.id.role][kb.id.value];
            CellI& debugCell2Cell = get(kb.id.cell)[kb.id.cell][kb.id.cell];
            CellI& debugCell2Role = get(kb.id.cell)[kb.id.cell][kb.id.role][kb.id.value];
            CellI& debugCell3Cell = get(kb.id.cell)[kb.id.cell][kb.id.cell][kb.id.cell][kb.id.value];
            CellI& debugCell3Role = get(kb.id.cell)[kb.id.cell][kb.id.cell][kb.id.role][kb.id.value];
        }
        CellI& inputCell = get(kb.id.cell);
        inputCell();
        CellI& cell      = inputCell[kb.id.value];
        CellI& inputRole = get(kb.id.role);
        inputRole();
        CellI& role = inputRole[kb.id.value];
        set(kb.id.value, cell[role]);
    } else if (&m_type == &kb.type.op.And) {
        CellI& inputLhs = get(kb.id.lhs);
        inputLhs();
        bool lhs        = &inputLhs[kb.id.value] == &kb.boolean.true_;
        // shortcut, if the left hand side already false we don't evaluate the right hand side
        if (lhs) {
            CellI& inputRhs = get(kb.id.rhs);
            inputRhs();
            bool rhs = &inputRhs[kb.id.value] == &kb.boolean.true_;
            set(kb.id.value, kb.toKbBool(lhs && rhs));
        } else {
            set(kb.id.value, kb.toKbBool(false));
        }
    } else if (&m_type == &kb.type.op.Or) {
        CellI& inputLhs = get(kb.id.lhs);
        inputLhs();
        bool lhs        = &inputLhs[kb.id.value] == &kb.boolean.true_;
        CellI& inputRhs = get(kb.id.rhs);
        inputRhs();
        bool rhs = &inputRhs[kb.id.value] == &kb.boolean.true_;
        set(kb.id.value, kb.toKbBool(lhs || rhs));

    } else if (&m_type == &kb.type.op.Not) {
        CellI& input = get(kb.id.input);
        input();
        bool res = &input[kb.id.value] == &kb.boolean.true_;
        set(kb.id.value, kb.toKbBool(!res));
    } else if (&m_type == &kb.type.op.Add) {
        CellI& inputLhs = get(kb.id.lhs);
        inputLhs();
        int lhs         = static_cast<Number&>(inputLhs[kb.id.value]).value();
        CellI& inputRhs = get(kb.id.rhs);
        inputRhs();
        int rhs = static_cast<Number&>(inputRhs[kb.id.value]).value();
        set(kb.id.value, kb.pools.numbers.get(lhs + rhs));
    } else if (&m_type == &kb.type.op.Subtract) {
        CellI& inputLhs = get(kb.id.lhs);
        inputLhs();
        int lhs         = static_cast<Number&>(inputLhs[kb.id.value]).value();
        CellI& inputRhs = get(kb.id.rhs);
        inputRhs();
        int rhs = static_cast<Number&>(inputRhs[kb.id.value]).value();
        set(kb.id.value, kb.pools.numbers.get(lhs - rhs));
    } else if (&m_type == &kb.type.op.Multiply) {
        CellI& inputLhs = get(kb.id.lhs);
        inputLhs();
        int lhs         = static_cast<Number&>(inputLhs[kb.id.value]).value();
        CellI& inputRhs = get(kb.id.rhs);
        inputRhs();
        int rhs = static_cast<Number&>(inputRhs[kb.id.value]).value();
        set(kb.id.value, kb.pools.numbers.get(lhs * rhs));
    } else if (&m_type == &kb.type.op.Divide) {
        CellI& inputLhs = get(kb.id.lhs);
        inputLhs();
        int lhs         = static_cast<Number&>(inputLhs[kb.id.value]).value();
        CellI& inputRhs = get(kb.id.rhs);
        inputRhs();
        int rhs = static_cast<Number&>(inputRhs[kb.id.value]).value();
        set(kb.id.value, kb.pools.numbers.get(lhs / rhs));
    } else if (&m_type == &kb.type.op.LessThan) {
        CellI& inputLhs = get(kb.id.lhs);
        inputLhs();
        int lhs         = static_cast<Number&>(inputLhs[kb.id.value]).value();
        CellI& inputRhs = get(kb.id.rhs);
        inputRhs();
        int rhs = static_cast<Number&>(inputRhs[kb.id.value]).value();
        set(kb.id.value, lhs < rhs ? kb.boolean.true_ : kb.boolean.false_);
    } else if (&m_type == &kb.type.op.LessThanOrEqual) {
        CellI& inputLhs = get(kb.id.lhs);
        inputLhs();
        int lhs         = static_cast<Number&>(inputLhs[kb.id.value]).value();
        CellI& inputRhs = get(kb.id.rhs);
        inputRhs();
        int rhs = static_cast<Number&>(inputRhs[kb.id.value]).value();
        set(kb.id.value, lhs <= rhs ? kb.boolean.true_ : kb.boolean.false_);
    } else if (&m_type == &kb.type.op.GreaterThan) {
        CellI& inputLhs = get(kb.id.lhs);
        inputLhs();
        int lhs         = static_cast<Number&>(inputLhs[kb.id.value]).value();
        CellI& inputRhs = get(kb.id.rhs);
        inputRhs();
        int rhs = static_cast<Number&>(inputRhs[kb.id.value]).value();
        set(kb.id.value, lhs > rhs ? kb.boolean.true_ : kb.boolean.false_);
    } else if (&m_type == &kb.type.op.GreaterThanOrEqual) {
        CellI& inputLhs = get(kb.id.lhs);
        inputLhs();
        int lhs         = static_cast<Number&>(inputLhs[kb.id.value]).value();
        CellI& inputRhs = get(kb.id.rhs);
        inputRhs();
        int rhs = static_cast<Number&>(inputRhs[kb.id.value]).value();
        set(kb.id.value, lhs >= rhs ? kb.boolean.true_ : kb.boolean.false_);
    }
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
    getMethod(kb.id.destructor)();
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
    return type().has(kb.id.methods) && type()[kb.id.methods].has(kb.id.index) && type()[kb.id.methods][kb.id.index].has(role);
}

CellI& Object::getMethod(CellI& role)
{
    resetIndent();
    if (type().has(kb.id.methods)) {
        CellI& methodsIndex = type()[kb.id.methods][kb.id.index];
        if (methodsIndex.has(role)) {
            CellI& method = methodsIndex[role][kb.id.value];
            Object& inputIndex = *new Object(kb, kb.type.Index);
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
    if (has(kb.id.methods)) {
        CellI& methodsIndex = (*this)[kb.id.methods][kb.id.index];
        if (methodsIndex.has(role)) {
            CellI& method = methodsIndex[role][kb.id.value];
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
    Object& inputIndex    = *new Object(kb, kb.type.Index, "StackFrame1.InputIndex");
    Object& stackFrame    = *new Object(kb, kb.type.StackFrame, "StackFrame1");
    Object& stackListItem = *new Object(kb, kb.type.ListItem, "StackListItem1");
    stackFrame.set(kb.id.method, method);
    stackFrame.set(kb.id.input, inputIndex);

    if (method.type()[kb.id.subTypes][kb.id.index].has(kb.id.returnType)) {
        Object& varResult   = *new Object(kb, kb.type.op.Var, "varResult");
        Object& outputIndex = *new Object(kb, kb.type.Index, "ResultIndex");
        outputIndex.set(kb.id.value, varResult);
        stackFrame.set(kb.id.output, outputIndex);
    }
    stackListItem.set(kb.id.value, stackFrame);
    method.set(kb.id.stack, stackListItem);
}

void Object::initLocalVars(CellI& method)
{
    if (method.type()[kb.id.subTypes][kb.id.index].missing(kb.id.localVars)) {
        return;
    }
    CellI& localVarsType   = method.type()[kb.id.subTypes][kb.id.index][kb.id.localVars][kb.id.value];
    Object& localVarsIndex = *new Object(kb, localVarsType, "LocalVarsIndex");
    CellI& stackFrame      = method[kb.id.stack][kb.id.value];
    stackFrame.set(kb.id.localVars, localVarsIndex);
    Visitor::visitList(localVarsType[kb.id.slots][kb.id.list], [this, &localVarsIndex](CellI& slot, int i, bool&) {
        Object& localVar = *new Object(kb, kb.type.op.Var, "localVar");
        localVar.set(kb.id.objectType, slot[kb.id.slotType]);
        localVarsIndex.set(slot[kb.id.slotRole], localVar);
    });
}

CellI& Object::getFnValue(CellI& method)
{
    if (method.type()[kb.id.subTypes][kb.id.index].has(kb.id.returnType)) {
        return method[kb.id.stack][kb.id.value][kb.id.output][kb.id.value];
    }

    return kb.id.emptyObject;
}

void Object::setSelf(CellI& method)
{
    setFnParam(method, { kb.id.self, *this });
}

void Object::setFnParam(CellI& fn, Param param)
{
    if (fn.type()[kb.id.subTypes][kb.id.index][kb.id.parameters][kb.id.value].has(kb.id.slots)) {
        CellI& inputsIndex = fn.type()[kb.id.subTypes][kb.id.index][kb.id.parameters][kb.id.value][kb.id.slots][kb.id.index];
        if (inputsIndex.has(param.role)) {
            fn[kb.id.stack][kb.id.value][kb.id.input].set(param.role, param.value);
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
    if (&role == &kb.id.type || &role == &kb.id.value) {
        return true;
    }
    if (&role == &kb.id.previous && m_previous) {
        return true;
    }
    if (&role == &kb.id.next && m_next) {
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
    if (&role == &kb.id.next) {
        m_next = nullptr;
    }
    if (&role == &kb.id.previous) {
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
    if (&role == &kb.id.type) {
        return kb.type.ListOf(m_list.m_valueType)[kb.id.subTypes][kb.id.index][kb.id.itemType];
    }
    if (&role == &kb.id.previous) {
        if (m_previous)
            return *m_previous;
        else
            throw "No such role!";
    }
    if (&role == &kb.id.next) {
        if (m_next)
            return *m_next;
        else
            throw "No such role!";
    }
    if (&role == &kb.id.value) {
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
    if (&role == &kb.id.type || &role == &kb.id.size) {
        return true;
    }
    if (&role == &kb.id.first && m_firstItem) {
        return true;
    }
    if (&role == &kb.id.last && m_lastItem) {
        return true;
    }
    if (&role == &kb.id.objectType) {
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
    if (&role == &kb.id.type) {
        return kb.type.ListOf(m_valueType);
    }
    if (&role == &kb.id.first) {
        return *m_firstItem;
    }
    if (&role == &kb.id.last) {
        return *m_lastItem;
    }
    if (&role == &kb.id.size) {
        int size = (int)m_size;

        return kb.pools.numbers.get(size);
    }
    if (&role == &kb.id.objectType) {
        return m_valueType;
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

void List::removeItem(Item* item)
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
#pragma region Type
// ============================================================================
Type::Type(brain::Brain& kb, const std::string& label) :
    CellI(kb, label),
    m_slots(*new Map(kb, kb.type.Cell, kb.type.Slot))
{
}

Type::Type(brain::Brain& kb, WithRecursiveType recursiveType, const std::string& label) :
    CellI(kb, label),
    m_slots(*new Map(kb, kb.type.Cell, kb.type.Slot, *this))
{
}

bool Type::has(CellI& role)
{
    if (&role == &kb.id.type) {
        return true;
    }
    if (&role == &kb.id.slots) {
        return true;
    }
    if (&role == &kb.id.subTypes) {
        return m_subTypes;
    }
    if (&role == &kb.id.memberOf) {
        return m_memberOf;
    }
    if (&role == &kb.id.asts) {
        return m_asts;
    }
    if (&role == &kb.id.methods) {
        return m_methods;
    }

    return false;
}

void Type::set(CellI& key, CellI& value)
{
    throw "Not supported";
}

void Type::erase(CellI& role)
{
    throw "Not supported";
}

void Type::operator()()
{
    // Do nothing, this is a data cell
}

CellI& Type::operator[](CellI& role)
{
    if (&role == &kb.id.type) {
        return kb.type.Type_;
    }
    if (&role == &kb.id.slots) {
        return m_slots;
    }
    if (&role == &kb.id.subTypes) {
        return *m_subTypes;
    }
    if (&role == &kb.id.memberOf) {
        return *m_memberOf;
    }
    if (&role == &kb.id.asts) {
        return *m_asts;
    }
    if (&role == &kb.id.methods) {
        return *m_methods;
    }

    throw "No such role!";
}

void Type::addSlot(CellI& role, CellI& slot)
{
    m_slots.add(role, slot);
}

bool Type::hasSlot(CellI& role)
{
    return m_slots.hasKey(role);
}

void Type::removeSlot(CellI& role)
{
    m_slots.remove(role);
}

void Type::accept(Visitor& visitor)
{
    visitor.visit(*this);
}
#pragma endregion
#pragma region Index
// ============================================================================
Index::Index(brain::Brain& kb, const std::string& label) :
    CellI(kb, label),
    m_type(new Type(kb, Type::WithRecursiveType::Yes))
{
}

Index::Index(brain::Brain& kb, Type& indexType, const std::string& label) :
    CellI(kb, label),
    m_type(&indexType),
    m_recursiveType(true)
{
}

bool Index::has(CellI& role)
{
    if (&role == &kb.id.type) {
        return true;
    }
    if (m_slots.find(&role) != m_slots.end()) {
        return true;
    }

    return false;
}

void Index::set(CellI& key, CellI& value)
{
    if (&key == &kb.id.type) {
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
    if (&role == &kb.id.type) {
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
    if (&key == &kb.id.type) {
        throw "The type key can not be changed!";
    }
    m_slots[&key] = &value;
    if (m_recursiveType) {
        return;
    }
    Object& slot = *new Object(kb, kb.type.Slot);
    slot.set(kb.id.slotRole, key);
    slot.set(kb.id.slotType, kb.type.Slot);
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

Map::Map(brain::Brain& kb, CellI& keyType, CellI& valueType, Type& indexType, const std::string& label) :
    CellI(kb, label),
    m_list(kb, valueType),
    m_index(kb, indexType),
    m_keyType(keyType),
    m_valueType(valueType)
{
}

bool Map::has(CellI& role)
{
    if (&role == &kb.id.type) {
        return true;
    }
    if (&role == &kb.id.list) {
        return true;
    }
    if (&role == &kb.id.listType) {
        return true;
    }
    if (&role == &kb.id.index) {
        return true;
    }
    if (&role == &kb.id.keyType) {
        return true;
    }
    if (&role == &kb.id.objectType) {
        return true;
    }
    if (&role == &kb.id.size) {
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
    if (&role == &kb.id.type) {
        return kb.type.MapOf(m_keyType, m_valueType);
    }
    if (&role == &kb.id.list) {
        return m_list;
    }
    if (&role == &kb.id.index) {
        return m_index;
    }
    if (&role == &kb.id.listType) {
        return kb.type.ListOf(kb.type.Slot);
    }
    if (&role == &kb.id.keyType) {
        return m_keyType;
    }
    if (&role == &kb.id.objectType) {
        return m_valueType;
    }
    if (&role == &kb.id.size) {
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
        return m_index[key][kb.id.value];
    }
    throw "No such role!";
}

void Map::add(CellI& key, CellI& value)
{
    if (&key == &kb.id.type) {
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
    m_list.removeItem(item);
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
    m_rootNode(kb, kb.type.TrieMapNode, "TrieNode_Root"),
    m_keyType(keyType),
    m_valueType(valueType)
{
}

bool TrieMap::has(CellI& role)
{
    if (&role == &kb.id.type) {
        return true;
    }
    if (&role == &kb.id.list) {
        return true;
    }
    if (&role == &kb.id.listType) {
        return true;
    }
    if (&role == &kb.id.index) {
        return true;
    }
    if (&role == &kb.id.keyType) {
        return true;
    }
    if (&role == &kb.id.objectType) {
        return true;
    }
    if (&role == &kb.id.size) {
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
    if (&role == &kb.id.type) {
        return kb.type.MapOf(m_keyType, m_valueType);
    }
    if (&role == &kb.id.list) {
        return m_list;
    }
    if (&role == &kb.id.rootNode) {
        return m_rootNode;
    }
    if (&role == &kb.id.listType) {
        return kb.type.ListOf(kb.type.Slot);
    }
    if (&role == &kb.id.keyType) {
        return m_keyType;
    }
    if (&role == &kb.id.objectType) {
        return m_valueType;
    }
    if (&role == &kb.id.size) {
        return kb.pools.numbers.get(m_size);
    }

    throw "No such role!";
}

bool TrieMap::hasKey(CellI& key)
{
    CellI* currentNode = &m_rootNode;

    if (isA(key, kb.type.List)) {
        throw "Key is not a list!";
    }

    Visitor::visitList(key, [this, &currentNode](CellI& keyItem, int i, bool& stop) {
        CellI* children = nullptr;
        if (currentNode->missing(kb.id.children)) {
            stop        = true;
            currentNode = nullptr;
            return;
        }
        Index& childrenIndex = static_cast<Index&>(currentNode->get(kb.id.children));
        if (childrenIndex.has(keyItem)) {
            children = &childrenIndex.get(keyItem);
        } else {
            stop        = true;
            currentNode = nullptr;
            return;
        }
        currentNode = children;
    });

    if (!currentNode || currentNode->missing(kb.id.data)) {
        return false;
    }

    return true;
}

CellI& TrieMap::getValue(CellI& key)
{
    if (isA(key, kb.type.List)) {
        throw "Key is not a list!";
    }

    CellI* currentNode = &m_rootNode;

    Visitor::visitList(key, [this, &currentNode](CellI& keyItem, int i, bool& stop) {
        CellI* children = nullptr;
        if (currentNode->missing(kb.id.children)) {
            stop        = true;
            currentNode = nullptr;
            return;
        }
        Index& childrenIndex = static_cast<Index&>(currentNode->get(kb.id.children));
        if (childrenIndex.has(keyItem)) {
            children = &childrenIndex.get(keyItem);
        } else {
            stop        = true;
            currentNode = nullptr;
            return;
        }
        currentNode = children;
    });

    if (!currentNode || currentNode->missing(kb.id.data)) {
        throw "No such key!";
    }

    return (*currentNode)[kb.id.data][kb.id.value];
}

void TrieMap::add(CellI& key, CellI& value)
{
    if (isA(key, kb.type.List)) {
        throw "Key is not a list!";
    }

    CellI* currentNode = &m_rootNode;

    Visitor::visitList(key, [this, &currentNode](CellI& keyItem, int i, bool& stop) {
        CellI* child = nullptr;
        if (currentNode->missing(kb.id.children)) {
            currentNode->set(kb.id.children, *new Index(kb));
        }
        Index& childrenIndex = static_cast<Index&>(currentNode->get(kb.id.children));
        if (childrenIndex.has(keyItem)) {
            child = &childrenIndex.get(keyItem);
        } else {
            child = new Object(kb, kb.type.TrieMapNode);
            child->set(kb.id.parent, *currentNode);
            childrenIndex.insert(keyItem, *child);
        }
        currentNode = child;
    });

    List::Item& item = *m_list.add(value);
    currentNode->set(kb.id.data, item);
    ++m_size;
}

void TrieMap::remove(CellI& key)
{
    if (isA(key, kb.type.List)) {
        throw "Key is not a list!";
    }

    if (&key[kb.id.size] == &kb._0_) {
        return;
    }

    CellI* currentNode    = &m_rootNode;

    Visitor::visitList(key, [this, &currentNode](CellI& keyItem, int i, bool& stop) {
        CellI* children = nullptr;
        if (currentNode->missing(kb.id.children)) {
            stop        = true;
            currentNode = nullptr;
            return;
        }
        Index& childrenIndex = static_cast<Index&>(currentNode->get(kb.id.children));
        if (childrenIndex.has(keyItem)) {
            children = &childrenIndex.get(keyItem);
        } else {
            stop        = true;
            currentNode = nullptr;
            return;
        }
        currentNode = children;
    });

    if (!currentNode || currentNode->missing(kb.id.data)) {
        return;
    }
    List::Item* valueItem = &static_cast<List::Item&>((*currentNode)[kb.id.data]);
    currentNode->erase(kb.id.data);

    CellI* keyItemPtr = &key[kb.id.last];
    while (currentNode->has(kb.id.parent)) {
        CellI& keyItem = *keyItemPtr;
        CellI& parent = currentNode->get(kb.id.parent);
        CellI& child = *currentNode;
        if (child.missing(kb.id.data)) {
            if (child.missing(kb.id.children) || ( child.has(kb.id.children) && static_cast<Index&>(child[kb.id.children]).empty())) {
                delete currentNode;
                parent[kb.id.children].erase(keyItem[kb.id.value]);
            }
        }
        currentNode = &parent;
        if (keyItem.has(kb.id.previous)) {
            keyItemPtr = &keyItem[kb.id.previous];
        } else {
            break;
        }
    }
    if (!valueItem) {
        return;
    }
    m_list.removeItem(valueItem);
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
    if (&role == &kb.id.type || &role == &kb.id.size) {
        return true;
    }
    if (&role == &kb.id.index) {
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
    if (&role == &kb.id.type) {
        return kb.type.SetOf(m_valueType);
    }
    if (&role == &kb.id.index) {
        return m_index;
    }
    if (&role == &kb.id.size) {
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
        throw "A value already registered with this role";
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
    if (&role == &kb.id.type || &role == &kb.id.value) {
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
    if (&role == &kb.id.type) {
        return kb.type.Number;
    }

    if (&role == &kb.numbers.sign && m_value != 0) {
        return m_value > 0 ? kb.numbers.positive : kb.numbers.negative;
    }

    if (&role == &kb.id.value) {
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

bool String::has(CellI& role)
{
    if (&role == &kb.id.type || &role == &kb.id.value) {
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
    if (&role == &kb.id.type) {
        return kb.type.String;
    } else if (&role == &kb.id.value) {
        if (m_characters.empty()) {
            calculateCharacters();
            m_charactersList.reset(new List(kb, m_characters, label()));
        }

        return *m_charactersList;
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
    if (&role == &kb.id.type) {
        return true;
    }
    if (&role == &kb.colors.red || &role == &kb.colors.green || &role == &kb.colors.blue) {
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
    if (&role == &kb.id.type) {
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
    if (&role == &kb.id.type) {
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
    if (&role == &kb.id.color) {
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
    if (&role == &kb.id.type) {
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
    if (&role == &kb.id.color) {
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
    m_pixelsList.reset(new List(kb, m_pixels));
}

bool Picture::has(CellI& role)
{
    if (&role == &kb.id.type || &role == &kb.id.width || &role == &kb.id.height || &role == &kb.id.pixels) {
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
    if (&role == &kb.id.type) {
        return kb.type.Picture;
    }
    if (&role == &kb.id.width) {
        return m_widthCell;
    }
    if (&role == &kb.id.height) {
        return m_heightCell;
    }
    if (&role == &kb.id.pixels) {
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
} // namespace hybrid

void Visitor::visitList(CellI& list, std::function<void(CellI& value, int i, bool& stop)> visitFn)
{
    brain::Brain& kb = list.kb;
    int i            = 0;

    CellI* currentListItemPtr = list.has(kb.id.first) ? &list[kb.id.first] : nullptr;
    while (currentListItemPtr) {
        CellI& currentListItem = *currentListItemPtr;
        CellI& value           = currentListItem[kb.id.value];
        bool stop              = false;

        visitFn(value, i++, stop);
        if (stop) {
            return;
        }

        currentListItemPtr = currentListItem.has(kb.id.next) ? &currentListItem[kb.id.next] : nullptr;
    }
}

bool tryVisitWith(CellI& cell, Visitor& visitor)
{
    brain::Brain& kb = cell.kb;

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
