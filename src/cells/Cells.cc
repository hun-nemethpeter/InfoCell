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
    return (*this)[kb.coding.type];
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

bool CellI::operator==(CellI& rhs)
{
    if (&type() != &rhs.type()) {
        return false;
    }
    CellI& slotList    = type()[kb.coding.slots][kb.coding.list];
    CellI* slotItemPtr = slotList.has(kb.sequence.first) ? &slotList[kb.sequence.first] : nullptr;
    while (slotItemPtr) {
        CellI& slotItem = *slotItemPtr;
        CellI& slot     = slotItem[kb.coding.value];
        CellI& role     = slot[kb.coding.slotRole];

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
#pragma endregion
#pragma region Object
// ============================================================================
Object::Object(brain::Brain& kb, CellI& type, const std::string& label) :
    CellI(kb, label),
    m_type(type)
{
    m_slots[&kb.coding.type] = &type;
    if (kb.initPhase() == InitPhase::Init) {
        return;
    }
}

Object::Object(brain::Brain& kb, CellI& type, CellI& constructor, const std::string& label) :
    CellI(kb, label),
    m_type(type)
{
    m_slots[&kb.coding.type] = &type;
    getMethod(constructor)();
}

Object::Object(brain::Brain& kb, CellI& type, CellI& constructor, Param param1, const std::string& label) :
    CellI(kb, label),
    m_type(type)
{
    m_slots[&kb.coding.type] = &type;

    CellI& method = getMethod(constructor);
    setFnParam(method, param1);
    method();
}

Object::Object(brain::Brain& kb, CellI& type, CellI& constructor, Param param1, Param param2, const std::string& label) :
    CellI(kb, label),
    m_type(type)
{
    m_slots[&kb.coding.type] = &type;

    CellI& method = getMethod(constructor);
    setFnParam(method, param1);
    setFnParam(method, param2);
    method();
}

Object::Object(brain::Brain& kb, CellI& type, CellI& constructor, Param param1, Param param2, Param param3, const std::string& label) :
    CellI(kb, label),
    m_type(type)
{
    m_slots[&kb.coding.type] = &type;

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
    m_slots[&kb.coding.type] = &type;

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
    if (!hasMethod(kb.coding.destructor)) {
        return;
    }
    destructor();
}

bool Object::has(CellI& role)
{
    if (&role == &kb.coding.type)
        return true;

    return m_slots.find(&role) != m_slots.end();
}

void Object::set(CellI& role, CellI& value)
{
    if (&role == &kb.coding.type) {
        throw "Type change not allowed.";
    }

    if (kb.initPhase() == InitPhase::Init) {
        m_slots[&role] = &value;
        return;
    }

    if (&type() == &kb.type.Index || type()[kb.coding.slots][kb.coding.index].has(role)) { // TODO Index is a hack
        m_slots[&role] = &value;
    } else {
        throw "The type doesn't contains this role.";
    }
}

void Object::operator()()
{
    if (&m_type == &kb.type.op.Block) {
        Visitor::visitList(get(kb.coding.ops), [this](CellI& op, int, bool& stop) {
            if (&op.type() == &kb.type.op.Return) {
                set(kb.coding.status, kb.coding.stop);
                stop = true;
                return;
            }
            set(kb.coding.status, kb.coding.continue_);
            op();
            if (op.has(kb.coding.status) && &op[kb.coding.status] == &kb.coding.stop) {
                set(kb.coding.status, kb.coding.stop);
                stop = true;
                return;
            }
        });
    } else if (&m_type == &kb.type.op.EvalVar) {
        CellI& value = get(kb.coding.value)[kb.coding.value];
        value();
    } else if (&m_type == &kb.type.op.Function) {
        if (has(kb.coding.op)) {
            CellI& op = get(kb.coding.op);
            op();
        }
    } else if (&m_type == &kb.type.op.Delete) {
        CellI& input = get(kb.coding.input);
        input();
        CellI* cell = &input[kb.coding.value];
        delete cell;
    } else if (&m_type == &kb.type.op.Set) {
        CellI& inputCell = get(kb.coding.cell);
        CellI& inputRole = get(kb.coding.role);
        CellI& inputValue = get(kb.coding.value);
        inputCell();
        inputRole();
        inputValue();
        CellI& cell  = inputCell[kb.coding.value];
        CellI& role  = inputRole[kb.coding.value];
        CellI& value = inputValue[kb.coding.value];

        cell.set(role, value);
    } else if (&m_type == &kb.type.op.If) {
        CellI& inputCondition = get(kb.coding.condition);
        CellI& thenBranch     = get(kb.coding.then);
        inputCondition();
        set(kb.coding.status, kb.coding.continue_);
        bool condition = &inputCondition[kb.coding.value] == &kb.boolean.true_;
        if (condition) {
            if (&thenBranch.type() == &kb.type.op.Return) {
                set(kb.coding.status, kb.coding.stop);
                return;
            }
            thenBranch();
            if (thenBranch.has(kb.coding.status) && &thenBranch[kb.coding.status] == &kb.coding.stop) {
                set(kb.coding.status, kb.coding.stop);
                return;
            }
        } else {
            if (has(kb.coding.else_)) {
                CellI& elseBranch = get(kb.coding.else_);
                if (&elseBranch.type() == &kb.type.op.Return) {
                    set(kb.coding.status, kb.coding.stop);
                    return;
                }
                elseBranch();
                if (elseBranch.has(kb.coding.status) && &elseBranch[kb.coding.status] == &kb.coding.stop) {
                    set(kb.coding.status, kb.coding.stop);
                    return;
                }
            }
        }
    } else if (&m_type == &kb.type.op.Do) {
        bool condition = false;
        set(kb.coding.status, kb.coding.continue_);
        do {
            CellI& statement      = get(kb.coding.statement);
            CellI& inputCondition = get(kb.coding.condition);
            if (&statement.type() == &kb.type.op.Return) {
                set(kb.coding.status, kb.coding.stop);
                return;
            }
            statement();
            if (statement.has(kb.coding.status) && &statement[kb.coding.status] == &kb.coding.stop) {
                set(kb.coding.status, kb.coding.stop);
                return;
            }
            inputCondition();
            condition = &inputCondition[kb.coding.value] == &kb.boolean.true_;
        } while (condition);
    } else if (&m_type == &kb.type.op.While) {
        bool condition = false;
        set(kb.coding.status, kb.coding.continue_);
        CellI& inputCondition = get(kb.coding.condition);
        CellI& statement      = get(kb.coding.statement);
        inputCondition();
        condition = &inputCondition[kb.coding.value] == &kb.boolean.true_;
        while (condition) {
            if (&statement.type() == &kb.type.op.Return) {
                set(kb.coding.status, kb.coding.stop);
                return;
            }
            statement();
            if (statement.has(kb.coding.status) && &statement[kb.coding.status] == &kb.coding.stop) {
                set(kb.coding.status, kb.coding.stop);
                return;
            }
            inputCondition();
            condition = &inputCondition[kb.coding.value] == &kb.boolean.true_;
        };
    } else if (&m_type == &kb.type.op.New) {
        CellI& inputObjectType = get(kb.coding.objectType);
        inputObjectType();
        CellI& objectType = inputObjectType[kb.coding.value];

        if (&objectType == &kb.type.Number) {
            set(kb.coding.value, *new Number(kb));
        } else if (&objectType == &kb.type.String) {
            set(kb.coding.value, *new String(kb));
        } else {
            set(kb.coding.value, *new Object(kb, objectType));
        }
    } else if (&m_type == &kb.type.op.Same) {
        CellI& inputLhs = get(kb.coding.lhs);
        CellI& inputRhs = get(kb.coding.rhs);
        inputLhs();
        inputRhs();
        CellI* lhs = &inputLhs[kb.coding.value];
        CellI* rhs = &inputRhs[kb.coding.value];
        set(kb.coding.value, kb.toKbBool(lhs == rhs));
    } else if (&m_type == &kb.type.op.NotSame) {
        CellI& inputLhs = get(kb.coding.lhs);
        CellI& inputRhs = get(kb.coding.rhs);
        inputLhs();
        inputRhs();
        CellI* lhs = &inputLhs[kb.coding.value];
        CellI* rhs = &inputRhs[kb.coding.value];
        set(kb.coding.value, kb.toKbBool(lhs != rhs));
    } else if (&m_type == &kb.type.op.Equal) {
        CellI& inputLhs = get(kb.coding.lhs);
        CellI& inputRhs = get(kb.coding.rhs);
        inputLhs();
        inputRhs();
        CellI& lhs = inputLhs[kb.coding.value];
        CellI& rhs = inputRhs[kb.coding.value];
        set(kb.coding.value, kb.toKbBool(lhs == rhs));
    } else if (&m_type == &kb.type.op.NotEqual) {
        CellI& inputLhs = get(kb.coding.lhs);
        CellI& inputRhs = get(kb.coding.rhs);
        inputLhs();
        inputRhs();
        CellI& lhs = inputLhs[kb.coding.value];
        CellI& rhs = inputRhs[kb.coding.value];
        set(kb.coding.value, kb.toKbBool(lhs != rhs));
    } else if (&m_type == &kb.type.op.Has) {
        CellI& inputCell = get(kb.coding.cell);
        CellI& inputRole = get(kb.coding.role);
        inputCell();
        inputRole();
        CellI& cell = inputCell[kb.coding.value];
        CellI& role = inputRole[kb.coding.value];
        set(kb.coding.value, kb.toKbBool(cell.has(role)));
    } else if (&m_type == &kb.type.op.Missing) {
        CellI& inputCell = get(kb.coding.cell);
        CellI& inputRole = get(kb.coding.role);
        inputCell();
        inputRole();
        CellI& cell = inputCell[kb.coding.value];
        CellI& role = inputRole[kb.coding.value];
        set(kb.coding.value, kb.toKbBool(cell.missing(role)));
    } else if (&m_type == &kb.type.op.Get) {
        CellI& inputCell = get(kb.coding.cell);
        CellI& inputRole = get(kb.coding.role);
        inputCell();
        inputRole();
        CellI& cell = inputCell[kb.coding.value];
        CellI& role = inputRole[kb.coding.value];
        set(kb.coding.value, cell[role]);
    } else if (&m_type == &kb.type.op.And) {
        CellI& inputLhs = get(kb.coding.lhs);
        CellI& inputRhs = get(kb.coding.rhs);
        inputLhs();
        inputRhs();
        bool lhs   = inputLhs[kb.coding.value] == kb.boolean.true_;
        bool rhs   = inputRhs[kb.coding.value] == kb.boolean.true_;
        set(kb.coding.value, kb.toKbBool(lhs && rhs));
    } else if (&m_type == &kb.type.op.Or) {
        CellI& inputLhs = get(kb.coding.lhs);
        CellI& inputRhs = get(kb.coding.rhs);
        inputLhs();
        inputRhs();
        bool lhs = inputLhs[kb.coding.value] == kb.boolean.true_;
        bool rhs = inputRhs[kb.coding.value] == kb.boolean.true_;
        set(kb.coding.value, kb.toKbBool(lhs || rhs));

    } else if (&m_type == &kb.type.op.Not) {
        CellI& input = get(kb.coding.input);
        input();
        bool res = &input[kb.coding.value] == &kb.boolean.true_;
        set(kb.coding.value, kb.toKbBool(!res));
    } else if (&m_type == &kb.type.op.Add) {
        CellI& inputLhs = get(kb.coding.lhs);
        CellI& inputRhs = get(kb.coding.rhs);
        inputLhs();
        inputRhs();
        int lhs = static_cast<Number&>(inputLhs[kb.coding.value]).value();
        int rhs = static_cast<Number&>(inputRhs[kb.coding.value]).value();
        set(kb.coding.value, kb.pools.numbers.get(lhs + rhs));
    } else if (&m_type == &kb.type.op.Subtract) {
        CellI& inputLhs = get(kb.coding.lhs);
        CellI& inputRhs = get(kb.coding.rhs);
        inputLhs();
        inputRhs();
        int lhs = static_cast<Number&>(inputLhs[kb.coding.value]).value();
        int rhs = static_cast<Number&>(inputRhs[kb.coding.value]).value();
        set(kb.coding.value, kb.pools.numbers.get(lhs - rhs));
    } else if (&m_type == &kb.type.op.Multiply) {
        CellI& inputLhs = get(kb.coding.lhs);
        CellI& inputRhs = get(kb.coding.rhs);
        inputLhs();
        inputRhs();
        int lhs = static_cast<Number&>(inputLhs[kb.coding.value]).value();
        int rhs = static_cast<Number&>(inputRhs[kb.coding.value]).value();
        set(kb.coding.value, kb.pools.numbers.get(lhs * rhs));
    } else if (&m_type == &kb.type.op.Divide) {
        CellI& inputLhs = get(kb.coding.lhs);
        CellI& inputRhs = get(kb.coding.rhs);
        inputLhs();
        inputRhs();
        int lhs = static_cast<Number&>(inputLhs[kb.coding.value]).value();
        int rhs = static_cast<Number&>(inputRhs[kb.coding.value]).value();
        set(kb.coding.value, kb.pools.numbers.get(lhs / rhs));
    } else if (&m_type == &kb.type.op.LessThan) {
        CellI& inputLhs = get(kb.coding.lhs);
        CellI& inputRhs = get(kb.coding.rhs);
        inputLhs();
        inputRhs();
        int lhs = static_cast<Number&>(inputLhs[kb.coding.value]).value();
        int rhs = static_cast<Number&>(inputRhs[kb.coding.value]).value();
        set(kb.coding.value, lhs < rhs ? kb.boolean.true_ : kb.boolean.false_);
    } else if (&m_type == &kb.type.op.GreaterThan) {
        CellI& inputLhs = get(kb.coding.lhs);
        CellI& inputRhs = get(kb.coding.rhs);
        inputLhs();
        inputRhs();
        int lhs = static_cast<Number&>(inputLhs[kb.coding.value]).value();
        int rhs = static_cast<Number&>(inputRhs[kb.coding.value]).value();
        set(kb.coding.value, lhs > rhs ? kb.boolean.true_ : kb.boolean.false_);
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
    getMethod(kb.coding.destructor)();
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
    return type().has(kb.coding.methods) && type()[kb.coding.methods].has(kb.coding.index) && type()[kb.coding.methods][kb.coding.index].has(role);
}

CellI& Object::getMethod(CellI& role)
{
    if (type().has(kb.coding.methods)) {
        CellI& methodsIndex = type()[kb.coding.methods][kb.coding.index];
        if (methodsIndex.has(role)) {
            CellI& method = methodsIndex[role];
            setSelf(method);
            return method;
        }
    }

    throw "No such method!";
}

CellI& Object::getStaticMethod(CellI& role)
{
    if (has(kb.coding.methods)) {
        CellI& methodsIndex = (*this)[kb.coding.methods][kb.coding.index];
        if (methodsIndex.has(role)) {
            CellI& method = methodsIndex[role];
            setSelf(method);
            return method;
        }
    }

    throw "No such method!";
}

CellI& Object::getFnValue(CellI& fn)
{
    if (fn.has(kb.coding.output)) {
        CellI& outputsIndex = fn[kb.coding.output][kb.coding.index];
        if (outputsIndex.has(kb.coding.value)) {
            return outputsIndex[kb.coding.value][kb.coding.value];
        }
    }

    return kb.coding.emptyObject;
}

void Object::setSelf(CellI& fn)
{
    setFnParam(fn, { kb.coding.self, *this });
}

void Object::setFnParam(CellI& fn, Param param)
{
    if (fn.has(kb.coding.input)) {
        CellI& inputsIndex = fn[kb.coding.input][kb.coding.index];
        if (inputsIndex.has(param.role)) {
            inputsIndex[param.role].set(kb.coding.value, param.value);
        } else {
            throw "No such param!";
        }
    }
}

#pragma endregion
#pragma region List::Item
// ============================================================================
List::Item::Item(brain::Brain& kb, Value& value) :
    CellI(kb), m_value(value)
{
}

bool List::Item::has(CellI& role)
{
    if (&role == &kb.coding.type || &role == &kb.coding.value) {
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

void List::Item::set(CellI& role, CellI& value)
{
    // Do nothing
}

void List::Item::operator()()
{
    // Do nothing
}

CellI& List::Item::operator[](CellI& role)
{
    if (&role == &kb.coding.type) {
        return kb.type.ListOf(m_value.m_list.m_valueType)[kb.coding.subTypes][kb.coding.index][kb.coding.objectType];
    }
    if (&role == &kb.sequence.previous) {
        if (m_value.prev())
            return m_value.prev()->m_listItem;
        else
            throw "No such role!";
    }
    if (&role == &kb.sequence.next) {
        if (m_value.next())
            return m_value.next()->m_listItem;
        else
            throw "No such role!";
    }
    if (&role == &kb.coding.value) {
        return m_value.m_value;
    }

    throw "No such role!";
}

void List::Item::accept(Visitor& visitor)
{
    visitor.visit(*this);
}
#pragma endregion
#pragma region List::Value
// ============================================================================
List::Value::Value(List& list, CellI& value) :
    m_list(list),
    m_value(value),
    m_listItem(list.kb, *this)
{
}

List::Value* List::Value::prev()
{
    return m_iterator != m_list.m_items.begin() ? &*std::prev(m_iterator) : nullptr;
}

List::Value* List::Value::next()
{
    return m_iterator != std::prev(m_list.m_items.end()) ? &*std::next(m_iterator) : nullptr;
}
#pragma endregion
#pragma region List
// ============================================================================
List::List(brain::Brain& kb, CellI& valueType) :
    CellI(kb),
    m_valueType(valueType)
{
}

bool List::has(CellI& role)
{
    if (&role == &kb.coding.type || &role == &kb.dimensions.size) {
        return true;
    }
    if ((&role == &kb.sequence.first || &role == &kb.sequence.last) && !m_items.empty()) {
        return true;
    }
    if (&role == &kb.coding.objectType) {
        return true;
    }

    return false;
}

void List::set(CellI& role, CellI& value)
{
    throw "Not supported";
}

void List::operator()()
{
    // Do nothing, this is a data cell
}

CellI& List::operator[](CellI& role)
{
    if (&role == &kb.coding.type) {
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
    if (&role == &kb.coding.objectType) {
        return m_valueType;
    }

    throw "No such role!";
}

void List::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

void List::add(CellI& value)
{
    m_items.emplace_back(*this, value);
    m_items.back().m_iterator = std::prev(m_items.end());
}

bool List::empty() const
{
    return m_items.empty();
}

CellI& List::toNative()
{
    Object& ret = *new Object(kb, get(kb.coding.type));
    ret.set(kb.sequence.first, get(kb.sequence.first));
    ret.set(kb.sequence.last, get(kb.sequence.first));
    ret.set(kb.dimensions.size, get(kb.dimensions.size));

    return ret;
}

#pragma endregion
#pragma region Map::Index::Type::Slots::SlotList::Item
// ============================================================================
Map::Index::Type::Slots::SlotList::Item::Item(brain::Brain& kb, Value& value) :
    CellI(kb),
    m_value(value)
{
}

bool Map::Index::Type::Slots::SlotList::Item::has(CellI& role)
{
    if (&role == &kb.coding.type || &role == &kb.coding.value) {
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

void Map::Index::Type::Slots::SlotList::Item::set(CellI& role, CellI& value)
{
    // Do nothing
}

void Map::Index::Type::Slots::SlotList::Item::operator()()
{
    // Do nothing, this is a data cell
}

CellI& Map::Index::Type::Slots::SlotList::Item::operator[](CellI& role)
{
    if (&role == &kb.coding.type) {
        return kb.type.ListOf(kb.type.Slot)[kb.coding.subTypes][kb.coding.index][kb.coding.objectType];
    }
    if (&role == &kb.sequence.previous) {
        if (m_value.prev())
            return m_value.prev()->m_indexTypeSlotsListItem;
        else
            throw "No such role!";
    }
    if (&role == &kb.sequence.next) {
        if (m_value.next())
            return m_value.next()->m_indexTypeSlotsListItem;
        else
            throw "No such role!";
    }
    if (&role == &kb.coding.value) {
        return m_value.m_indexTypeSlot;
    }

    throw "No such role!";
}

void Map::Index::Type::Slots::SlotList::Item::accept(Visitor& visitor)
{
    visitor.visit(*this);
}
#pragma endregion
#pragma region Map::Index::Type::Slots::SlotList
// ============================================================================
Map::Index::Type::Slots::SlotList::SlotList(brain::Brain& kb, OrderedValues& orderedValues) :
    CellI(kb),
    m_orderedValues(orderedValues)
{
}

bool Map::Index::Type::Slots::SlotList::has(CellI& role)
{
    if (&role == &kb.coding.type || &role == &kb.sequence.first || &role == &kb.sequence.last || &role == &kb.dimensions.size) {
        return true;
    }

    return false;
}

void Map::Index::Type::Slots::SlotList::set(CellI& role, CellI& value)
{
    // Do nothing
}

void Map::Index::Type::Slots::SlotList::operator()()
{
    // Do nothing, this is a data cell
}

CellI& Map::Index::Type::Slots::SlotList::operator[](CellI& role)
{
    if (&role == &kb.coding.type) {
        return kb.type.ListOf(kb.type.Slot);
    }
    if (&role == &kb.sequence.first) {
        if (m_orderedValues.empty()) {
            throw "No such role!";
        }
        return (*m_orderedValues.begin())->m_indexTypeSlotsListItem;
    }
    if (&role == &kb.sequence.last) {
        if (m_orderedValues.empty()) {
            throw "No such role!";
        }
        return (*m_orderedValues.rbegin())->m_indexTypeSlotsListItem;
    }
    if (&role == &kb.dimensions.size) {
        return kb.pools.numbers.get((int)m_orderedValues.size());
    }

    throw "No such role!";
}

void Map::Index::Type::Slots::SlotList::accept(Visitor& visitor)
{
    visitor.visit(*this);
}
#pragma endregion
#pragma region Map::Index::Type::Slots::SlotIndex
// ============================================================================
Map::Index::Type::Slots::SlotIndex::SlotIndex(brain::Brain& kb, IndexedValues& indexedValues, Type& type) :
    CellI(kb),
    m_indexedValues(indexedValues),
    m_type(type)
{
}

bool Map::Index::Type::Slots::SlotIndex::has(CellI& role)
{
    if (&role == &kb.coding.type) {
        return true;
    }
    auto slotIt = m_indexedValues.find(&role);
    if (slotIt != m_indexedValues.end()) {
        return true;
    }

    return false;
}

void Map::Index::Type::Slots::SlotIndex::set(CellI& role, CellI& value)
{
    // Do nothing
}

void Map::Index::Type::Slots::SlotIndex::operator()()
{
    // Do nothing, this is a data cell
}

CellI& Map::Index::Type::Slots::SlotIndex::operator[](CellI& role)
{
    if (&role == &kb.coding.type) {
        return m_type;
    }
    auto slotIt = m_indexedValues.find(&role);
    if (slotIt != m_indexedValues.end()) {
        return slotIt->second.m_indexTypeSlot;
    }

    throw "No such role!";
}

void Map::Index::Type::Slots::SlotIndex::accept(Visitor& visitor)
{
    visitor.visit(*this);
}
#pragma endregion
#pragma region Map::Index::Type::Slot
// ============================================================================
Map::Index::Type::Slot::Slot(brain::Brain& kb, CellI& slotRole) :
    CellI(kb),
    m_slotRole(slotRole)
{
}

bool Map::Index::Type::Slot::has(CellI& role)
{
    if (&role == &kb.coding.type || &role == &kb.coding.slotType || &role == &kb.coding.slotRole) {
        return true;
    }
    return false;
}

void Map::Index::Type::Slot::set(CellI& role, CellI& value)
{
    // Do nothing
}

void Map::Index::Type::Slot::operator()()
{
    // Do nothing, this is a data cell
}

CellI& Map::Index::Type::Slot::operator[](CellI& role)
{
    if (&role == &kb.coding.type) {
        return kb.type.Slot;
    }
    if (&role == &kb.coding.slotType) {
        return kb.type.Slot;
    }
    if (&role == &kb.coding.slotRole) {
        return m_slotRole;
    }

    throw "No such role!";
}

void Map::Index::Type::Slot::accept(Visitor& visitor)
{
    visitor.visit(*this);
}
#pragma endregion
#pragma region Map::Index::Type::Slots
// ============================================================================
Map::Index::Type::Slots::Slots(brain::Brain& kb, IndexedValues& indexedValues, OrderedValues& orderedValues, CellI& valueType, Type& type) :
    CellI(kb),
    m_slotList(kb, orderedValues),
    m_slotIndex(kb, indexedValues, type)
{
    if (&valueType == &kb.type.Slot) {
        label("Index<Slot>::Map");
    } else if (&valueType == &kb.type.ast.Function) {
        label("Index<Ast::Function>::Map");
    } else if (&valueType == &kb.type.op.Function) {
        label("Index<Op::Function>::Map");
    } else {
        label(std::format("Index<{}>::Map", valueType.label()));
    }
}

bool Map::Index::Type::Slots::has(CellI& role)
{
    if (&role == &kb.coding.type || &role == &kb.coding.index || &role == &kb.coding.list) {
        return true;
    }

    return false;
}

void Map::Index::Type::Slots::set(CellI& role, CellI& value)
{
    // Do nothing
}

void Map::Index::Type::Slots::operator()()
{
    // Do nothing, this is a data cell
}

CellI& Map::Index::Type::Slots::operator[](CellI& role)
{
    if (&role == &kb.coding.type) {
        return kb.type.MapCellToSlot;
    }
    if (&role == &kb.coding.index) {
        return m_slotIndex;
    }
    if (&role == &kb.coding.list) {
        return m_slotList;
    }

    throw "No such role!";
}

void Map::Index::Type::Slots::accept(Visitor& visitor)
{
    visitor.visit(*this);
}
#pragma endregion
#pragma region Map::Index::Type
// ============================================================================
Map::Index::Type::Type(brain::Brain& kb, IndexedValues& indexedValues, OrderedValues& orderedValues, CellI& valueType) :
    CellI(kb),
    m_slots(kb, indexedValues, orderedValues, valueType, *this)
{
    if (&valueType == &kb.type.Slot) {
        label("Index<Slot>");
    } else if (&valueType == &kb.type.ast.Function) {
        label("Index<Ast::Function>");
    } else if (&valueType == &kb.type.op.Function) {
        label("Index<Op::Function>");
    } else {
        label(std::format("Index<{}>", valueType.label()));
    }
}

bool Map::Index::Type::has(CellI& role)
{
    if (&role == &kb.coding.type || &role == &kb.coding.slots) {
        return true;
    }

    return false;
}

void Map::Index::Type::set(CellI& role, CellI& value)
{
    // Do nothing
}

void Map::Index::Type::operator()()
{
    // Do nothing, this is a data cell
}

CellI& Map::Index::Type::operator[](CellI& role)
{
    if (&role == &kb.coding.type) {
        return kb.type.Type_;
    }
    if (&role == &kb.coding.slots) {
        return m_slots;
    }
    if (&role == &kb.coding.memberOf) {
        static std::unique_ptr<Map> s_memberOfList;
        if (!s_memberOfList) {
            s_memberOfList = std::make_unique<Map>(kb, kb.type.Cell, kb.coding.type);
            s_memberOfList->add(kb.type.Index, kb.type.Index);
        }
        return *s_memberOfList;
    }

    throw "No such role!";
}

void Map::Index::Type::accept(Visitor& visitor)
{
    visitor.visit(*this);
}
#pragma endregion
#pragma region Map::Index
// ============================================================================
Map::Index::Index(brain::Brain& kb, IndexedValues& indexedValues, OrderedValues& orderedValues, CellI& valueType) :
    CellI(kb),
    m_type(kb, indexedValues, orderedValues, valueType),
    m_indexedValues(indexedValues),
    m_orderedValues(orderedValues)
{
}

bool Map::Index::has(CellI& role)
{
    if (&role == &kb.coding.type) {
        return true;
    }
    auto slotIt = m_indexedValues.find(&role);
    if (slotIt != m_indexedValues.end()) {
        return true;
    }

    return false;
}

void Map::Index::set(CellI& role, CellI& value)
{
    // Do nothing
}

void Map::Index::operator()()
{
    // Do nothing, this is a data cell
}

CellI& Map::Index::operator[](CellI& role)
{
    if (&role == &kb.coding.type) {
        return m_type;
    }
    auto slotIt = m_indexedValues.find(&role);
    if (slotIt != m_indexedValues.end()) {
        return slotIt->second.m_value;
    }

    throw "No such role!";
}

void Map::Index::accept(Visitor& visitor)
{
    visitor.visit(*this);
}
#pragma endregion
#pragma region Map::Value
// ============================================================================
Map::Value::Value(Map& group, CellI& value, CellI& index, size_t listItemIndex) :
    m_group(group),
    m_value(value),
    m_indexTypeSlotsListItem(group.kb, *this),
    m_indexTypeSlot(group.kb, index)
{
}

Map::Value* Map::Value::prev()
{
    return m_iterator != m_group.m_orderedValues.begin() ? *std::prev(m_iterator) : nullptr;
}

Map::Value* Map::Value::next()
{
    return m_iterator != std::prev(m_group.m_orderedValues.end()) ? *std::next(m_iterator) : nullptr;
}
#pragma endregion
#pragma region Map
// ============================================================================
Map::Map(brain::Brain& kb, CellI& keyType, CellI& valueType, const std::string& label) :
    CellI(kb, label),
    m_keyType(keyType),
    m_valueType(valueType),
    m_list(kb, valueType),
    m_index(kb, m_indexedValues, m_orderedValues, valueType)
{
}

bool Map::has(CellI& role)
{
    if (&role == &kb.coding.type || &role == &kb.dimensions.size) {
        return true;
    }
    if (&role == &kb.coding.index && !m_orderedValues.empty()) {
        return true;
    }
    if (&role == &kb.coding.list && !m_orderedValues.empty()) {
        return true;
    }

    return false;
}

void Map::set(CellI& role, CellI& value)
{
    throw "Not supported";
}

void Map::operator()()
{
    // Do nothing, this is a data cell
}

CellI& Map::operator[](CellI& role)
{
    if (&role == &kb.coding.type) {
        return kb.type.MapOf(m_keyType, m_valueType);
    }
    if (&role == &kb.coding.index) {
        return m_index;
    }
    if (&role == &kb.coding.list) {
        return m_list;
    }
    if (&role == &kb.dimensions.size) {
        int size = (int)m_orderedValues.size();

        return kb.pools.numbers.get(size);
    }

    throw "No such role!";
}

void Map::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

#if 0
void Map::add(CellI& value)
{
    add(value, value);
}
#endif

void Map::add(CellI& key, CellI& value)
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

bool Map::empty() const
{
    return m_indexedValues.empty();
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
    if (&role == &kb.coding.type || &role == &kb.coding.value) {
        return true;
    }
    if (&role == &kb.numbers.sign) {
        return m_value != 0;
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
    if (&role == &kb.coding.type) {
        return kb.type.Number;
    }

    if (&role == &kb.numbers.sign && m_value != 0) {
        return m_value > 0 ? kb.numbers.positive : kb.numbers.negative;
    }

    if (&role == &kb.coding.value) {
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
    if (&role == &kb.coding.type || &role == &kb.coding.value) {
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
    if (&role == &kb.coding.type) {
        return kb.type.String;
    } else if (&role == &kb.coding.value) {
        if (m_characters.empty()) {
            calculateCharacters();
            m_charactersList.reset(new List(kb, m_characters));
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
    if (&role == &kb.coding.type) {
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
    if (&role == &kb.coding.type) {
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
    if (&role == &kb.coding.type) {
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
    if (&role == &kb.coding.type) {
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
    if (&role == &kb.coding.type || &role == &kb.dimensions.width || &role == &kb.dimensions.height || &role == &kb.visualization.pixels) {
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
    if (&role == &kb.coding.type) {
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

    CellI* currentListItemPtr = list.has(kb.sequence.first) ? &list[kb.sequence.first] : nullptr;
    while (currentListItemPtr) {
        CellI& currentListItem = *currentListItemPtr;
        CellI& value           = currentListItem[kb.coding.value];
        bool stop              = false;

        visitFn(value, i++, stop);
        if (stop) {
            return;
        }

        currentListItemPtr = currentListItem.has(kb.sequence.next) ? &currentListItem[kb.sequence.next] : nullptr;
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
