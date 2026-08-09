#include "OpActivator.h"

namespace infocell {
namespace cells {

OpActivator::OpActivator(CellI& currentCell) :
    w(currentCell.w),
    id(w.id),
    std(w.std),
    m_currentCell(&currentCell),
    m_previousCell(&w.id.emptyObject)
{
}

void OpActivator::activate()
{
    resetIndent();
    int tick = 0;
    do {
        CellI& self    = *m_currentCell;
        CellI& type    = self.__type__();
        m_currentState = &self[id.state];

        if (&type == &std.op.Activate) {
            activateOpActivate();
        } else if (&type == &std.op.Add) {
            activateOpAdd();
        } else if (&type == &std.op.And) {
            activateOpAnd();
        } else if (&type == &std.op.Block) {
            activateOpBlock();
        } else if (&type == &w.std.op.Call) {
            activateOpCall();
        } else if (&type == &w.std.op.Delete) {
            activateOpDelete();
        } else if (&type == &w.std.op.Divide) {
            activateOpDivide();
        } else if (&type == &w.std.op.Do) {
            activateOpDo();
        } else if (&type == &w.std.op.Equal) {
            activateOpEqual();
        } else if (&type == &w.std.op.Erase) {
            activateOpErase();
        } else if (&type == &w.std.op.Function) {
            activateOpFunction();
        } else if (&type == &w.std.op.Get) {
            activateOpGet();
        } else if (&type == &w.std.op.GreaterThan) {
            activateOpGreaterThan();
        } else if (&type == &w.std.op.GreaterThanOrEqual) {
            activateOpGreaterThanOrEqual();
        } else if (&type == &w.std.op.Has) {
            activateOpHas();
        } else if (&type == &w.std.op.If) {
            activateOpIf();
        } else if (&type == &w.std.op.LessThan) {
            activateOpLessThan();
        } else if (&type == &w.std.op.LessThanOrEqual) {
            activateOpLessThanOrEqual();
        } else if (&type == &w.std.op.Missing) {
            activateOpMissing();
        } else if (&type == &w.std.op.Multiply) {
            activateOpMultiply();
        } else if (&type == &w.std.op.New) {
            activateOpNew();
        } else if (&type == &w.std.op.Not) {
            activateOpNot();
        } else if (&type == &w.std.op.NotEqual) {
            activateOpNotEqual();
        } else if (&type == &w.std.op.NotSame) {
            activateOpNotSame();
        } else if (&type == &w.std.op.Or) {
            activateOpOr();
        } else if (&type == &w.std.op.Return) {
            activateOpReturn();
        } else if (&type == &w.std.op.Same) {
            activateOpSame();
        } else if (&type == &w.std.op.Set) {
            activateOpSet();
        } else if (&type == &w.std.op.Subtract) {
            activateOpSubtract();
        } else if (&type == &w.std.op.While) {
            activateOpWhile();
        } else {
            // assuming it is a datacell, so do nothing
            std::swap(m_currentCell, m_previousCell);
        }

        w.ap.m_currentCell  = m_currentCell;
        w.ap.m_previousCell = m_previousCell;
        ++tick;
    } while (m_currentCell != &w.id.emptyObject);
    w.ap.m_time.value(w.ap.m_time.value() + tick);
}

void OpActivator::printIndent()
{
    static const int s_tabSize = 2;
    for (int i = 0; i < m_indent; ++i) {
        for (int j = 0; j < s_tabSize; ++j) {
            std::cout << " ";
        }
    }
}

void OpActivator::resetIndent()
{
    m_indent = 0;
}

void OpActivator::activateOpActivate()
{
    CellI& self = *m_currentCell;
    auto& state = std.op.Activate.State;

    if (m_currentState == &state.ready) {
        self.set(id.previous, *m_previousCell);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[id.input];
        self.set(id.state, state.activateInput);
    } else if (m_currentState == &state.activateInput) {
        m_previousCell   = m_currentCell;
        CellI& inputCell = self[id.input];
        CellI* status    = &id.process;

        if (self.has(id.parent)) {
            CellI& parent = self[id.parent];
            if (&inputCell.__type__() == &std.op.Return || (inputCell.has(id.status) && (&inputCell[id.status] == &id.return_))) {
                parent.set(id.status, id.return_);
                status = &id.return_;
            } else if (&parent[id.status] == &id.continue_ || &parent[id.status] == &id.break_) {
                status = &parent[id.status];
            } else if (self.has(id.status)) {
                if (&self[id.status] == &id.return_ || &self[id.status] == &id.continue_ || &self[id.status] == &id.break_) {
                    parent.set(id.status, self[id.status]);
                    status = &self[id.status];
                }
            }
        }
        if (status == &id.process && self.has(id.next)) {
            m_currentCell = &self[id.next];
        } else {
            if (self.has(id.parent)) {
                m_currentCell = &self[id.parent];
            } else {
                m_currentCell = &self[id.previous];
            }
        }
        self.set(id.state, state.ready);
    }
}

void OpActivator::activateOpAdd()
{
    CellI& self = *m_currentCell;
    auto& state = std.op.Add.State;

    if (m_currentState == &state.ready) {
        self.set(id.previous, *m_previousCell);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[id.lhs];
        self.set(id.state, state.activateLhs);
    } else if (m_currentState == &state.activateLhs) {
        m_previousCell = m_currentCell;
        m_currentCell  = &self[id.rhs];
        self.set(id.state, state.activateRhs);
    } else if (m_currentState == &state.activateRhs) {
        int lhs = static_cast<Number&>(self[id.lhs][id.value]).value();
        int rhs = static_cast<Number&>(self[id.rhs][id.value]).value();

        self.set(id.value, w.pools.numbers.get(lhs + rhs));
        self.set(id.state, state.ready);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[id.previous];
    }
}

void OpActivator::activateOpAnd()
{
    CellI& self = *m_currentCell;
    auto& state = std.op.And.State;

    if (m_currentState == &state.ready) {
        self.set(id.previous, *m_previousCell);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[id.lhs];
        self.set(id.state, state.activateLhs);
    } else if (m_currentState == &state.activateLhs) {
        bool lhs = &self[id.lhs][id.value] == &w.true_;
        // shortcut, if the left hand side already false we don't evaluate the right hand side
        if (lhs == false) {
            self.set(id.value, w.false_);
            self.set(id.state, state.ready);
            m_previousCell = m_currentCell;
            m_currentCell  = &self[id.previous];
        } else {
            m_previousCell = m_currentCell;
            m_currentCell  = &self[id.rhs];
            self.set(id.state, state.activateRhs);
        }
    } else if (m_currentState == &state.activateRhs) {
        bool lhs = &self[id.lhs][id.value] == &w.true_;
        bool rhs = &self[id.rhs][id.value] == &w.true_;

        self.set(id.value, w.toCellBool(lhs && rhs));
        self.set(id.state, state.ready);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[id.previous];
    }
}

void OpActivator::activateOpBlock()
{
    CellI& self = *m_currentCell;
    auto& state = std.op.Block.State;

    if (m_currentState == &state.ready) {
        self.set(id.previous, *m_previousCell);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[id.input];
        self.set(id.status, id.process);
        self.set(id.state, state.activateInput);
    } else if (m_currentState == &state.activateInput) {
        m_previousCell = m_currentCell;
        m_currentCell  = &self[id.previous];
        self.set(id.state, state.ready);
    }
}

void OpActivator::activateOpCall()
{
    CellI& self = *m_currentCell;
    auto& state = std.op.Call.State;

    //    std::cout << "evalOpCall self: " << &self << ", state: " << self[w.id.state].label() << std::endl;
    if (m_currentState == &state.ready) {
        self.erase(w.id.value);
        self.set(w.id.previous, *m_previousCell);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.method];
        if (self.has(w.id.parameters)) {
            self.set(w.id.state, state.activateParameters);
        } else {
            self.set(w.id.state, state.stackPushAndCall);
        }
    } else if (m_currentState == &state.activateParameters) {
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
            m_previousCell = m_currentCell;
            m_currentCell  = &param;
        } else {
            self.set(w.id.state, state.stackPushAndCall);
        }
    } else if (m_currentState == &state.stackPushAndCall) {
        CellI& methodName = self[w.id.method][w.id.value];
        CellI& stack      = self[w.id.parentFunction][w.id.stack];

        CellI* methodPtr = nullptr;
        if (&methodName.__type__() == &w.std.String) {
            auto& parametersMap = static_cast<Map&>(self[w.id.parameters]);
            auto& selfSlot      = parametersMap.getValue(id.self);
            CellI& inputSelf    = selfSlot[w.id.value];
            auto& methodsMap    = static_cast<Map&>(inputSelf[w.id.__type__][w.id.methods]);
            methodPtr           = &methodsMap.getValue(methodName);
            // printIndent();
            // std::cout << "inputSelf type: " << inputSelf[w.id.__type__].label() << std::endl;
        } else {
            methodPtr = &self[w.id.method][w.id.value];
        }
        CellI& method = (*methodPtr);

        CellI& stackFrame = *new Object(w, w.std.StackFrame);
        stackFrame.set(w.id.method, method);

        CellI& inputIndex = *new Object(w, w.std.Index);
        if (self.has(w.id.parameters)) {
            for (CellI& parameter : self[w.id.parameters]) {
                inputIndex.set(parameter[w.id.key], parameter[w.id.value][w.id.value]);
                // printIndent();
                // std::cout << parameter[w.id.key].label() << ":" << parameter[w.id.value][w.id.value].label() << std::endl;
            }
        }
        stackFrame.set(w.id.input, inputIndex);

        if (method.has(w.id.localVars)) {
            CellI& localVarsList = method[w.id.localVars].membersList();
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
        self.set(w.id.state, state.stackPop);
        previousMethod.set(w.id.lastOp, self);

        if (method.has(w.id.state) && (&method[w.id.state] != &state.ready)) {
            //            std::cout << "recursive call for " << method.__type__().label() << std::endl;
            List& cellPath = *new List(w, w.std.op.Base);
            CellI& lastOp  = method[w.id.lastOp];
            for (CellI* currentOp = &lastOp; currentOp != &method; currentOp = (*currentOp).has(w.id.parent) ? &(*currentOp)[w.id.parent] : &(*currentOp)[w.id.previous]) {
                CellI& op = *currentOp;
                //                std::cout << "         [" << op.__type__().label() << ":" << op[w.id.state].label() << "]" << std::endl;
                saveOpState(cellPath, op);
            }
            saveOpState(cellPath, method);
            method.set(w.id.state, state.ready);
            stackFrame.set(w.id.ops, cellPath);
            //            std::cout << std::endl;
        }
        m_previousCell = m_currentCell;
        m_currentCell  = &method;
    } else if (m_currentState == &state.stackPop) {
        CellI& methodName = self[w.id.method][w.id.value];
        CellI* methodPtr = nullptr;
        if (&methodName.__type__() == &w.std.String) {
            auto& parametersMap = static_cast<Map&>(self[w.id.parameters]);
            auto& selfSlot      = parametersMap.getValue(id.self);
            CellI& inputSelf    = selfSlot[w.id.value];
            auto& methodsMap    = static_cast<Map&>(inputSelf[w.id.__type__][w.id.methods]);
            methodPtr           = &methodsMap.getValue(methodName);
            // TODO: cache the method obj
        } else {
            methodPtr = &self[w.id.method][w.id.value];
        }
        CellI& method = (*methodPtr);
        if (method.has(w.id.value)) {
            self.set(w.id.value, method[w.id.value]);
            // printIndent();
            // std::cout << "return " << method[w.id.value].label() << std::endl;
        }

        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.previous];
        self.set(w.id.state, state.ready);
    }
}

void OpActivator::activateOpDelete()
{
    CellI& self = *m_currentCell;
    auto& state = std.op.Delete.State;

    if (m_currentState == &state.ready) {
        self.set(w.id.previous, *m_previousCell);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.input];
        self.set(w.id.state, state.activateInput);
    } else if (m_currentState == &state.activateInput) {
        CellI& input = self[w.id.input];
        CellI* cell  = &input[w.id.value];

        delete cell;
        input.erase(w.id.value);
        self.set(w.id.state, state.ready);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.previous];
    }
}

void OpActivator::activateOpDivide()
{
    CellI& self = *m_currentCell;
    auto& state = std.op.Divide.State;

    if (m_currentState == &state.ready) {
        self.set(w.id.previous, *m_previousCell);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.lhs];
        self.set(w.id.state, state.activateLhs);
    } else if (m_currentState == &state.activateLhs) {
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.rhs];
        self.set(w.id.state, state.activateRhs);
    } else if (m_currentState == &state.activateRhs) {
        int lhs = static_cast<Number&>(self[w.id.lhs][w.id.value]).value();
        int rhs = static_cast<Number&>(self[w.id.rhs][w.id.value]).value();

        self.set(w.id.value, w.pools.numbers.get(lhs / rhs));
        self.set(w.id.state, state.ready);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.previous];
    }
}

void OpActivator::activateOpDo()
{
    CellI& self = *m_currentCell;
    auto& state = std.op.Do.State;

    if (m_currentState == &state.ready) {
        self.set(id.previous, *m_previousCell);
        self.set(id.status, id.process);
        m_previousCell   = m_currentCell;
        m_currentCell    = &self[id.statement];
        self.set(id.state, state.activateStatement);
    } else if (m_currentState == &state.activateStatement) {
        CellI& statement = self[id.statement];
        if (&statement.__type__() == &std.op.Return) {
            self.set(id.status, id.return_);
        } else if (statement.has(id.status)) {
            self.set(id.status, statement[id.status]);
        }
        m_previousCell = m_currentCell;
        m_currentCell  = &self[id.condition];
        self.set(id.state, state.activateCondition);
    } else if (m_currentState == &state.activateCondition) {
        m_previousCell = m_currentCell;
        if (self.has(id.status) && ((&self[id.status] == &id.return_) || (&self[id.status] == &id.break_))) {
            m_currentCell = &self[id.previous];
            self.set(id.state, state.ready);
        } else {
            self.set(id.status, id.process);
            bool condition = &self[id.condition][id.value] == &w.true_;
            if (condition) {
                m_currentCell = &self[id.statement];
                self.set(id.state, state.activateStatement);
            } else {
                m_currentCell = &self[id.previous];
                self.set(id.state, state.ready);
            }
        }
    }
}

void OpActivator::activateOpEqual()
{
    CellI& self = *m_currentCell;
    auto& state = std.op.Equal.State;

    if (m_currentState == &state.ready) {
        self.set(w.id.previous, *m_previousCell);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.lhs];
        self.set(w.id.state, state.activateLhs);
    } else if (m_currentState == &state.activateLhs) {
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.rhs];
        self.set(w.id.state, state.activateRhs);
    } else if (m_currentState == &state.activateRhs) {
        CellI& lhs = self[w.id.lhs][w.id.value];
        CellI& rhs = self[w.id.rhs][w.id.value];

        self.set(w.id.value, w.toCellBool(lhs == rhs));
        self.set(w.id.state, state.ready);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.previous];
    }
}

void OpActivator::activateOpErase()
{
    CellI& self = *m_currentCell;
    auto& state = std.op.Erase.State;

    if (m_currentState == &state.ready) {
        self.set(w.id.previous, *m_previousCell);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.cell];
        self.set(w.id.state, state.activateCell);
    } else if (m_currentState == &state.activateCell) {
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.key];
        self.set(w.id.state, state.activateKey);
    } else if (m_currentState == &state.activateKey) {
        CellI& cell = self[w.id.cell][w.id.value];
        CellI& key  = self[w.id.key][w.id.value];

        cell.erase(key);
        self.set(w.id.state, state.ready);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.previous];
    }
}

void OpActivator::activateOpFunction()
{
    CellI& self = *m_currentCell;
    auto& state = std.op.Function.State;
    if (m_currentState == &state.ready) {
        if (m_debugFunctionCalls) {
            printIndent();
            m_indent++;
            std::cout << self.label() << std::endl;
        }
        self.set(w.id.previous, *m_previousCell);
        CellI& op = self[w.id.op];
        if (&op[w.id.state] != &state.ready) {
            throw "Error: function contains non-clean op.";
        }
        m_previousCell = m_currentCell;
        m_currentCell  = &op;
        self.set(w.id.state, state.activateOp);
    } else if (m_currentState == &state.activateOp) {
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.previous];

        CellI& stackNode  = self[w.id.stack];
        CellI& stackFrame = stackNode[w.id.value];
        // printIndent();
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
            self.set(w.id.state, state.ready);
        }
        if (self[w.id.stack][w.id.previous].has(w.id.value)) {
            CellI& inputIndex         = stackFrame[w.id.input];
            CellI& previousStackNode  = self[w.id.stack][w.id.previous];
            CellI& previousStackFrame = previousStackNode[w.id.value];
            CellI& previousMethod     = previousStackFrame[w.id.method];
            previousMethod.set(w.id.stack, previousStackNode);
            previousStackNode.erase(w.id.next);
            delete &inputIndex;
            if (stackFrame.has(w.id.localVars)) {
                CellI& localVarsList  = self[w.id.localVars].membersList();
                CellI& localVarsIndex = stackFrame[w.id.localVars];
                for (CellI& slot : localVarsList) {
                    delete &localVarsIndex[slot[w.id.key]];
                }
                delete &localVarsIndex;
            }
            delete &stackFrame;
            delete &stackNode;
            if (m_currentCell == &w.id.emptyObject) {
                std::cout << "";
            }
        } else {
            self.set(w.id.state, state.ready);
            m_currentCell = &w.id.emptyObject;
        }
        if (m_debugFunctionCalls) {
            m_indent--;
        }
    }
}

void OpActivator::activateOpGet()
{
    CellI& self = *m_currentCell;
    auto& state = std.op.Get.State;

    if (m_currentState == &state.ready) {
        self.set(w.id.previous, *m_previousCell);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.cell];
        self.set(w.id.state, state.activateCell);
    } else if (m_currentState == &state.activateCell) {
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.key];
        self.set(w.id.state, state.activateKey);
    } else if (m_currentState == &state.activateKey) {
        CellI& cell = self[w.id.cell][w.id.value];
        CellI& key  = self[w.id.key][w.id.value];

        self.set(w.id.value, cell[key]);
        self.set(w.id.state, state.ready);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.previous];
    }
}

void OpActivator::activateOpGreaterThan()
{
    CellI& self = *m_currentCell;
    auto& state = std.op.GreaterThan.State;

    if (m_currentState == &state.ready) {
        self.set(w.id.previous, *m_previousCell);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.lhs];
        self.set(w.id.state, state.activateLhs);
    } else if (m_currentState == &state.activateLhs) {
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.rhs];
        self.set(w.id.state, state.activateRhs);
    } else if (m_currentState == &state.activateRhs) {
        int lhs = static_cast<Number&>(self[w.id.lhs][w.id.value]).value();
        int rhs = static_cast<Number&>(self[w.id.rhs][w.id.value]).value();

        self.set(w.id.value, lhs > rhs ? w.true_ : w.false_);
        self.set(w.id.state, state.ready);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.previous];
    }
}

void OpActivator::activateOpGreaterThanOrEqual()
{
    CellI& self = *m_currentCell;
    auto& state = std.op.GreaterThanOrEqual.State;

    if (m_currentState == &state.ready) {
        self.set(w.id.previous, *m_previousCell);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.lhs];
        self.set(w.id.state, state.activateLhs);
    } else if (m_currentState == &state.activateLhs) {
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.rhs];
        self.set(w.id.state, state.activateRhs);
    } else if (m_currentState == &state.activateRhs) {
        int lhs = static_cast<Number&>(self[w.id.lhs][w.id.value]).value();
        int rhs = static_cast<Number&>(self[w.id.rhs][w.id.value]).value();

        self.set(w.id.value, lhs >= rhs ? w.true_ : w.false_);
        //        std::cout << "      " << lhs << " >= " << rhs << std::endl;
        self.set(w.id.state, state.ready);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.previous];
    }
}

void OpActivator::activateOpHas()
{
    CellI& self = *m_currentCell;
    auto& state = std.op.Has.State;

    if (m_currentState == &state.ready) {
        self.set(w.id.previous, *m_previousCell);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.cell];
        self.set(w.id.state, state.activateCell);
    } else if (m_currentState == &state.activateCell) {
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.key];
        self.set(w.id.state, state.activateKey);
    } else if (m_currentState == &state.activateKey) {
        CellI& cell = self[w.id.cell][w.id.value];
        CellI& key  = self[w.id.key][w.id.value];

        self.set(w.id.value, w.toCellBool(cell.has(key)));
        self.set(w.id.state, state.ready);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.previous];
    }
}

void OpActivator::activateOpIf()
{
    CellI& self = *m_currentCell;
    auto& state = std.op.If.State;

    if (m_currentState == &state.ready) {
        self.set(w.id.previous, *m_previousCell);

        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.condition];
        self.set(w.id.state, state.activateCondition);
    } else if (m_currentState == &state.activateCondition) {
        self.set(w.id.status, w.id.process);
        CellI* branchPtr = nullptr;
        bool condition   = &self[w.id.condition][w.id.value] == &w.true_;
        if (condition) {
            branchPtr = &self[w.id.then];
            self.set(w.id.state, state.activateThen);
        } else if (self.has(w.id.else_)) {
            branchPtr = &self[w.id.else_];
            self.set(w.id.state, state.activateElse);
        }

        m_previousCell = m_currentCell;
        if (branchPtr) {
            m_currentCell = branchPtr;
        } else {
            m_currentCell = &self[w.id.previous];
            self.set(w.id.state, state.ready);
        }
    } else if (m_currentState == &state.activateThen || m_currentState == &state.activateElse) {
        CellI& branch = m_currentState == &state.activateThen ? self[w.id.then] : self[w.id.else_];
        if (&branch.__type__() == &w.std.op.Return) {
            self.set(w.id.status, w.id.return_);
        } else if (branch.has(w.id.status)) {
            self.set(w.id.status, branch[w.id.status]);
        }

        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.previous];
        self.set(w.id.state, state.ready);
    }
}

void OpActivator::activateOpLessThan()
{
    CellI& self = *m_currentCell;
    auto& state = std.op.LessThan.State;

    if (m_currentState == &state.ready) {
        self.set(w.id.previous, *m_previousCell);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.lhs];
        self.set(w.id.state, state.activateLhs);
    } else if (m_currentState == &state.activateLhs) {
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.rhs];
        self.set(w.id.state, state.activateRhs);
    } else if (m_currentState == &state.activateRhs) {
        int lhs = static_cast<Number&>(self[w.id.lhs][w.id.value]).value();
        int rhs = static_cast<Number&>(self[w.id.rhs][w.id.value]).value();

        self.set(w.id.value, lhs < rhs ? w.true_ : w.false_);
        self.set(w.id.state, state.ready);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.previous];
    }
}

void OpActivator::activateOpLessThanOrEqual()
{
    CellI& self = *m_currentCell;
    auto& state = std.op.LessThanOrEqual.State;

    if (m_currentState == &state.ready) {
        self.set(w.id.previous, *m_previousCell);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.lhs];
        self.set(w.id.state, state.activateLhs);
    } else if (m_currentState == &state.activateLhs) {
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.rhs];
        self.set(w.id.state, state.activateRhs);
    } else if (m_currentState == &state.activateRhs) {
        int lhs = static_cast<Number&>(self[w.id.lhs][w.id.value]).value();
        int rhs = static_cast<Number&>(self[w.id.rhs][w.id.value]).value();

        self.set(w.id.value, lhs <= rhs ? w.true_ : w.false_);
        self.set(w.id.state, state.ready);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.previous];
    }
}

void OpActivator::activateOpMissing()
{
    CellI& self = *m_currentCell;
    auto& state = std.op.Missing.State;

    if (m_currentState == &state.ready) {
        self.set(w.id.previous, *m_previousCell);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.cell];
        self.set(w.id.state, state.activateCell);
    } else if (m_currentState == &state.activateCell) {
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.key];
        self.set(w.id.state, state.activateKey);
    } else if (m_currentState == &state.activateKey) {
        CellI& cell = self[w.id.cell][w.id.value];
        CellI& key  = self[w.id.key][w.id.value];

        self.set(w.id.value, w.toCellBool(cell.missing(key)));
        self.set(w.id.state, state.ready);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.previous];
    }
}

void OpActivator::activateOpMultiply()
{
    CellI& self = *m_currentCell;
    auto& state = std.op.Multiply.State;

    if (m_currentState == &state.ready) {
        self.set(w.id.previous, *m_previousCell);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.lhs];
        self.set(w.id.state, state.activateLhs);
    } else if (m_currentState == &state.activateLhs) {
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.rhs];
        self.set(w.id.state, state.activateRhs);
    } else if (m_currentState == &state.activateRhs) {
        int lhs = static_cast<Number&>(self[w.id.lhs][w.id.value]).value();
        int rhs = static_cast<Number&>(self[w.id.rhs][w.id.value]).value();

        self.set(w.id.value, w.pools.numbers.get(lhs * rhs));
        //        std::cout << "      " << lhs << " * " << rhs << std::endl;
        self.set(w.id.state, state.ready);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.previous];
    }
}

void OpActivator::activateOpNew()
{
    CellI& self = *m_currentCell;
    auto& state = std.op.New.State;

    if (m_currentState == &state.ready) {
        self.set(w.id.previous, *m_previousCell);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.objectType];
        self.set(w.id.state, state.activateObjectType);
    } else if (m_currentState == &state.activateObjectType) {
        CellI& objectType = self[w.id.objectType][w.id.value];

        if (&objectType == &w.std.Number) {
            self.set(w.id.value, *new Number(w));
        } else if (&objectType == &w.std.String) {
            self.set(w.id.value, *new String(w));
        } else {
            self.set(w.id.value, *new Object(w, objectType));
        }
        self.set(w.id.state, state.ready);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.previous];
    }
}

void OpActivator::activateOpNot()
{
    CellI& self = *m_currentCell;
    auto& state = std.op.Not.State;

    if (m_currentState == &state.ready) {
        self.set(w.id.previous, *m_previousCell);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.input];
        self.set(w.id.state, state.activateInput);
    } else if (m_currentState == &state.activateInput) {
        bool res = &self[w.id.input][w.id.value] == &w.true_;

        self.set(w.id.value, w.toCellBool(!res));
        self.set(w.id.state, state.ready);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.previous];
    }
}

void OpActivator::activateOpNotEqual()
{
    CellI& self = *m_currentCell;
    auto& state = std.op.NotEqual.State;

    if (m_currentState == &state.ready) {
        self.set(w.id.previous, *m_previousCell);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.lhs];
        self.set(w.id.state, state.activateLhs);
    } else if (m_currentState == &state.activateLhs) {
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.rhs];
        self.set(w.id.state, state.activateRhs);
    } else if (m_currentState == &state.activateRhs) {
        CellI& lhs = self[w.id.lhs][w.id.value];
        CellI& rhs = self[w.id.rhs][w.id.value];

        self.set(w.id.value, w.toCellBool(lhs != rhs));
        self.set(w.id.state, state.ready);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.previous];
    }
}

void OpActivator::activateOpNotSame()
{
    CellI& self = *m_currentCell;
    auto& state = std.op.NotSame.State;

    if (m_currentState == &state.ready) {
        self.set(w.id.previous, *m_previousCell);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.lhs];
        self.set(w.id.state, state.activateLhs);
    } else if (m_currentState == &state.activateLhs) {
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.rhs];
        self.set(w.id.state, state.activateRhs);
    } else if (m_currentState == &state.activateRhs) {
        CellI* lhs = &self[w.id.lhs][w.id.value];
        CellI* rhs = &self[w.id.rhs][w.id.value];

        self.set(w.id.value, w.toCellBool(lhs != rhs));
        self.set(w.id.state, state.ready);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.previous];
    }
}

void OpActivator::activateOpOr()
{
    CellI& self = *m_currentCell;
    auto& state = std.op.Or.State;

    if (m_currentState == &state.ready) {
        self.set(w.id.previous, *m_previousCell);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.lhs];
        self.set(w.id.state, state.activateLhs);
    } else if (m_currentState == &state.activateLhs) {
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.rhs];
        self.set(w.id.state, state.activateRhs);
    } else if (m_currentState == &state.activateRhs) {
        bool lhs = &self[w.id.lhs][w.id.value] == &w.true_;
        bool rhs = &self[w.id.rhs][w.id.value] == &w.true_;

        self.set(w.id.value, w.toCellBool(lhs || rhs));
        self.set(w.id.state, state.ready);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.previous];
    }
}

void OpActivator::activateOpReturn()
{
    CellI& self = *m_currentCell;
    auto& state = std.op.Return.State;

    if (self.has(w.id.result)) {
        if (m_currentState == &state.ready) {
            self.set(w.id.previous, *m_previousCell);
            m_previousCell = m_currentCell;
            m_currentCell  = &self[w.id.result];
            self.set(w.id.state, state.activateResult);
        } else if (m_currentState == &state.activateResult) {
            m_previousCell = m_currentCell;
            m_currentCell  = &self[w.id.previous];
            self.set(w.id.state, state.ready);
        }
    } else {
        std::swap(m_currentCell, m_previousCell);
    }
}

void OpActivator::activateOpSame()
{
    CellI& self = *m_currentCell;
    auto& state = std.op.Same.State;

    if (m_currentState == &state.ready) {
        self.set(w.id.previous, *m_previousCell);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.lhs];
        self.set(w.id.state, state.activateLhs);
    } else if (m_currentState == &state.activateLhs) {
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.rhs];
        self.set(w.id.state, state.activateRhs);
    } else if (m_currentState == &state.activateRhs) {
        CellI* lhs = &self[w.id.lhs][w.id.value];
        CellI* rhs = &self[w.id.rhs][w.id.value];

        self.set(w.id.value, w.toCellBool(lhs == rhs));
        self.set(w.id.state, state.ready);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.previous];
    }
}

void OpActivator::activateOpSet()
{
    CellI& self = *m_currentCell;
    auto& state = std.op.Set.State;

    if (m_currentState == &state.ready) {
        self.set(w.id.previous, *m_previousCell);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.cell];
        self.set(w.id.state, state.activateCell);
    } else if (m_currentState == &state.activateCell) {
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.key];
        self.set(w.id.state, state.activateKey);
    } else if (m_currentState == &state.activateKey) {
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.value];
        self.set(w.id.state, state.activateValue);
    } else if (m_currentState == &state.activateValue) {
        CellI& cell  = self[w.id.cell][w.id.value];
        CellI& key   = self[w.id.key][w.id.value];
        CellI& value = self[w.id.value][w.id.value];

        cell.set(key, value);
        self.set(w.id.state, state.ready);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.previous];
    }
}

void OpActivator::activateOpSubtract()
{
    CellI& self = *m_currentCell;
    auto& state = std.op.Subtract.State;

    if (m_currentState == &state.ready) {
        self.set(w.id.previous, *m_previousCell);
        m_previousCell    = m_currentCell;
        m_currentCell     = &self[w.id.lhs];
        self.set(w.id.state, state.activateLhs);
    } else if (m_currentState == &state.activateLhs) {
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.rhs];
        self.set(w.id.state, state.activateRhs);
    } else if (m_currentState == &state.activateRhs) {
        int lhs = static_cast<Number&>(self[w.id.lhs][w.id.value]).value();
        int rhs = static_cast<Number&>(self[w.id.rhs][w.id.value]).value();

        self.set(w.id.value, w.pools.numbers.get(lhs - rhs));
        //        std::cout << "      " << lhs << " - " << rhs << std::endl;
        self.set(w.id.state, state.ready);
        m_previousCell = m_currentCell;
        m_currentCell  = &self[w.id.previous];
    }
}

void OpActivator::activateOpWhile()
{
    CellI& self = *m_currentCell;
    auto& state = std.op.While.State;

    if (m_currentState == &state.ready) {
        self.set(w.id.previous, *m_previousCell);
        self.set(w.id.status, w.id.process);
        CellI& inputCondition = self[w.id.condition];
        m_previousCell          = m_currentCell;
        m_currentCell           = &inputCondition;
        self.set(w.id.state, state.activateCondition);
    } else if (m_currentState == &state.activateCondition) {
        m_previousCell = m_currentCell;
        if (self.has(w.id.status) && ((&self[w.id.status] == &w.id.return_) || (&self[w.id.status] == &w.id.break_))) {
            m_currentCell = &self[w.id.previous];
            self.set(w.id.state, state.ready);
        } else {
            self.set(w.id.status, w.id.process);
            bool condition = &self[w.id.condition][w.id.value] == &w.true_;
            if (condition) {
                m_currentCell = &self[w.id.statement];
                self.set(w.id.state, state.activateStatement);
            } else {
                m_currentCell = &self[w.id.previous];
                self.set(w.id.state, state.ready);
            }
        }
    } else if (m_currentState == &state.activateStatement) {
        CellI& statement = self[w.id.statement];
        if (&statement.__type__() == &w.std.op.Return) {
            self.set(w.id.status, w.id.return_);
        } else if (statement.has(w.id.status)) {
            self.set(w.id.status, statement[w.id.status]);
        }
        CellI& inputCondition = self[w.id.condition];
        m_previousCell        = m_currentCell;
        m_currentCell         = &inputCondition;
        self.set(w.id.state, state.activateCondition);
    }
}

void OpActivator::saveOpState(List& opStates, CellI& op)
{
    if (&op == &id.emptyObject) {
        return;
    }
    CellI& type     = op.__type__();
    Object& opState = *new Object(w, std.op.SavedState);

    opState.set(id.op, op);
    opState.set(id.state, id.state);
    opState.set(id.value, op[id.state]);
    opStates.add(opState);
    if (op.has(id.status)) {
        Object& opState = *new Object(w, std.op.SavedState);
        opState.set(id.op, op);
        opState.set(id.state, id.status);
        opState.set(id.value, op[id.status]);
        opStates.add(opState);
    }
    if (op.has(id.previous)) {
        Object& opState = *new Object(w, std.op.SavedState);
        opState.set(id.op, op);
        opState.set(id.state, id.previous);
        opState.set(id.value, op[id.previous]);
        opStates.add(opState);
    }
    if (&type == &std.op.Call) {
        if (op[id.method].has(id.value)) {
            Object& opState = *new Object(w, std.op.SavedState);
            opState.set(id.op, op);
            opState.set(id.state, id.method);
            opState.set(id.value, op[id.method][id.value]);
            opStates.add(opState);
        }
    }
    if (&type == &std.op.Set || &type == &std.op.Get) {
        if (op[id.cell].has(id.value)) {
            Object& opState = *new Object(w, std.op.SavedState);
            opState.set(id.op, op);
            opState.set(id.state, id.cell);
            opState.set(id.value, op[id.cell][id.value]);
            opStates.add(opState);
        }
        if (op[id.key].has(id.value)) {
            Object& opState = *new Object(w, std.op.SavedState);
            opState.set(id.op, op);
            opState.set(id.state, id.key);
            opState.set(id.value, op[id.key][id.value]);
            opStates.add(opState);
        }
    }

    if (&type == &std.op.Same ||
        &type == &std.op.NotSame ||
        &type == &std.op.Equal ||
        &type == &std.op.NotEqual ||
        &type == &std.op.LessThan ||
        &type == &std.op.LessThanOrEqual ||
        &type == &std.op.GreaterThan ||
        &type == &std.op.GreaterThanOrEqual ||
        &type == &std.op.And ||
        &type == &std.op.Or ||
        &type == &std.op.Add ||
        &type == &std.op.Subtract ||
        &type == &std.op.Multiply ||
        &type == &std.op.Divide) {
        Object& opState = *new Object(w, std.op.SavedState);
        opState.set(id.op, op);
        opState.set(id.state, id.lhs);
        opState.set(id.value, op[id.lhs][id.value]);
        opStates.add(opState);
    }
    op.set(id.state, w.std.op.State.ready);
}

void OpActivator::loadOpState(CellI& opState)
{
    CellI& op    = opState[id.op];
    CellI& state = opState[id.state];
    CellI& value = opState[id.value];
    CellI& type  = op.__type__();

    if ((&type == &std.op.Set || &type == &std.op.Get) && ((&state == &id.cell) || (&state == &id.key))) {
        op[state].set(id.value, value);
    } else if ((&type == &std.op.Call) && ((&state == &id.self) || (&state == &id.method) || (&state == &id.stack))) {
        op[state].set(id.value, value);
    } else if ((&type == &std.op.Same ||
        &type == &std.op.NotSame ||
        &type == &std.op.Equal ||
        &type == &std.op.NotEqual ||
        &type == &std.op.LessThan ||
        &type == &std.op.LessThanOrEqual ||
        &type == &std.op.GreaterThan ||
        &type == &std.op.GreaterThanOrEqual ||
        &type == &std.op.And ||
        &type == &std.op.Or ||
        &type == &std.op.Add ||
        &type == &std.op.Subtract ||
        &type == &std.op.Multiply ||
        &type == &std.op.Divide) && (&state == &id.lhs)) {
        op[state].set(id.value, value);
    } else {
        op.set(state, value);
    }
//    std::cout << "         set (" << &op << ")" << op.__type__().label() << "[" << state.label() << ":" << value.label() << "]" << std::endl;
}

} // namespace cells
} // namespace infocell
