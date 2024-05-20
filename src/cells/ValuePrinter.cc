#include <format>

#include "ValuePrinter.h"
#include "Brain.h"

namespace synth {
namespace cells {

void CellValuePrinter::visit(Object& object)
{
    printImpl(object);
}

void CellValuePrinter::visit(List::Item& cell)
{
    printImpl(cell);
}

void CellValuePrinter::visit(List& cell)
{
    printImpl(cell);
}

void CellValuePrinter::visit(Struct& cell)
{
    printImpl(cell);
}

void CellValuePrinter::visit(Index& cell)
{
    printImpl(cell);
}

void CellValuePrinter::visit(Map& cell)
{
    printImpl(cell);
}

void CellValuePrinter::visit(TrieMap& cell)
{
    printImpl(cell);
}

void CellValuePrinter::visit(Set& cell)
{
    printImpl(cell);
}

void CellValuePrinter::printOpBlock(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    CellI& ast       = cell[kb.ids.ast];
    if (&ast.struct_() == &kb.std.ast.Call || &ast.struct_() == &kb.std.ast.StaticCall) {
        if (&ast[kb.ids.cell].struct_() == &kb.std.ast.Get && cell.label() != "New { call constructor; }") {
            printImpl(ast[kb.ids.cell]);
        }
        if (&ast[kb.ids.cell].struct_() == &kb.std.ast.Self) {
            m_ss << "self";
        }
        if (&ast[kb.ids.cell].struct_() == &kb.std.ast.Cell) {
            m_ss << ast[kb.ids.cell][kb.ids.value].label();
        }
        if (&ast[kb.ids.cell].struct_() == &kb.std.ast.Member) {
            m_ss << "m_" << ast[kb.ids.cell][kb.ids.role].label();
        }
        if (&ast[kb.ids.cell].struct_() == &kb.std.ast.Parameter) {
            m_ss << "p_" << ast[kb.ids.cell][kb.ids.role].label();
        }
        if (ast.has(kb.ids.method)) {
            if (&ast.struct_() == &kb.std.ast.Call) {
                m_ss << ".";
            } else {
                m_ss << "::";
            }
            m_ss << ast[kb.ids.method][kb.ids.value].label();
            m_ss << "(";
            if (ast.has(kb.ids.parameters)) {
                auto& opsList = cell[kb.ids.ops];
                int paramNum  = 0;
                visitList(opsList, [this, &kb, &paramNum](CellI& op, int i, bool&) {
                    if (op.label() == "Call { setParam; }") {
                        if (paramNum++ > 0) {
                            m_ss << ", ";
                        }
                        auto& paramRole = op[kb.ids.role][kb.ids.value];
                        auto& paramValue = op[kb.ids.value];
                        m_ss << paramRole.label();
                        m_ss << ": ";
                        printImpl(paramValue);
                    }
                });
            }
            m_ss << ")";
        }
        return;
    }
    if (&ast.struct_() == &kb.std.ast.New) {
        m_ss << "new ";
        CellI* objectTypePtr = nullptr;
        if (ast[kb.ids.objectType].isA(kb.std.ast.Cell)) {
            objectTypePtr = &ast[kb.ids.objectType][kb.ids.value];
            m_ss << objectTypePtr->label();
        } else if (ast[kb.ids.objectType].isA(kb.std.ast.ResolvedType)) {
            objectTypePtr = &ast[kb.ids.objectType][kb.ids.compiled];
            m_ss << objectTypePtr->label();
        } else {
            throw "Unexpected AST type!";
        }
        CellI& objectType = *objectTypePtr;
        if (ast.has(kb.ids.constructor)) {
            CellI& constructorOps = cell[kb.ids.ops][kb.ids.first][kb.ids.next][kb.ids.value];
            printOpBlock(constructorOps);
        }
        return;
    }
    m_ss << "{\n";
    m_indent++;
    Visitor::visitList(cell[kb.ids.ops], [this](CellI& op, int, bool&) {
        printIndent();
        printImpl(op);
        m_ss << ";\n";
    });
    m_indent--;
    printIndent();
    m_ss << "}";
}

void CellValuePrinter::printOpEvalVar(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    m_ss << "eval (";
    printImpl(cell[kb.ids.value]);
    m_ss << ")";
}

void CellValuePrinter::printOpFunction(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    std::stringstream iss;
    std::stringstream oss;
    CellI& subTypesIndex = cell.struct_()[kb.ids.subTypes][kb.ids.index];
    CellI& inType        = subTypesIndex[kb.ids.parameters][kb.ids.value];
    bool hasReturnValue = false;
    if (inType.has(kb.ids.slots)) {
        Visitor::visitList(inType[kb.ids.slots][kb.ids.list], [this, &iss, &kb](CellI& slot, int i, bool& stop) {
            if (i > 0) {
                iss << ", ";
            }
            if (&slot[kb.ids.slotRole] != &kb.ids.self) {
                iss << "p_";
            }
            iss << slot[kb.ids.slotRole].label() << ": " << slot[kb.ids.slotType].label();
        });
    }
    if (subTypesIndex.has(kb.ids.returnType)) {
        CellI& outType = subTypesIndex[kb.ids.returnType][kb.ids.value];
        hasReturnValue      = true;
        oss << outType.label();
    }
    const std::string& className = subTypesIndex.has(kb.ids.objectType) ? subTypesIndex[kb.ids.objectType][kb.ids.value].label() : "";
    std::string label            = className;
    if (!className.empty()) {
        label += "::";
    }
    label += subTypesIndex[kb.ids.name][kb.ids.value].label();
    bool isStatic = cell.has(kb.ids.static_);
    std::string staticStr = isStatic ? "static " : "";
    std::string newLabel;

    if (hasReturnValue) {
        newLabel = std::format("fn {}{}({}) -> {}\n", staticStr, label, iss.str(), oss.str());
    } else {
        newLabel = std::format("fn {}{}({})\n", staticStr, label, iss.str());
    }
    m_ss << newLabel;

    printImpl(cell[kb.ids.op]);
}

void CellValuePrinter::printOpDelete(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    m_ss << "delete (";
    printImpl(cell[kb.ids.input]);
    m_ss << ")";
}

void CellValuePrinter::printOpSet(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    if ((&cell[kb.ids.ast][kb.ids.cell].struct_() == &kb.std.ast.Self) && (&cell[kb.ids.ast][kb.ids.role].struct_() == &kb.std.ast.Cell)) {
        m_ss << "m_";
    } else {
        printImpl(cell[kb.ids.cell]);
        m_ss << ".";
    }
    printImpl(cell[kb.ids.role]);
    m_ss << " = ";
    printImpl(cell[kb.ids.value]);
}

void CellValuePrinter::printOpErase(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.ids.cell]);
    m_ss << ".erase(";
    printImpl(cell[kb.ids.role]);
    m_ss << ")";
}

void CellValuePrinter::printOpIf(CellI& cell)
{
    brain::Brain& kb = cell.kb;

    auto isBlock = [this, &kb, &cell](CellI& ast) -> bool {
        if (&ast.struct_() == &kb.std.op.Block) {
            auto& astType = ast[kb.ids.ast].struct_();
            if (&astType == &kb.std.ast.Call || &astType == &kb.std.ast.StaticCall || &astType == &kb.std.ast.New) {
                return false;
            }
            return true;
        }
        return false;
    };
    m_ss << "if ";
    printImpl(cell[kb.ids.condition]);
    m_ss << " then ";
    if (!isBlock(cell[kb.ids.then])) {
        m_ss << "\n";
        m_indent++;
        printIndent();
    }
    printImpl(cell[kb.ids.then]);
    if (!isBlock(cell[kb.ids.then])) {
        m_indent--;
    }

    if (cell.has(kb.ids.else_)) {
        if (!isBlock(cell[kb.ids.then])) {
            m_ss << ";\n";
            printIndent();
        }
        m_ss << " else ";
        if (!isBlock(cell[kb.ids.else_])) {
            m_ss << "\n";
            m_indent++;
            printIndent();
        }
        printImpl(cell[kb.ids.else_]);
        if (!isBlock(cell[kb.ids.else_])) {
            m_indent--;
        }
    }
}

void CellValuePrinter::printOpDo(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    m_ss << "do ";
    printImpl(cell[kb.ids.statement]);
    m_ss << " (";
    printImpl(cell[kb.ids.condition]);
    m_ss << ")";
}

void CellValuePrinter::printOpWhile(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    m_ss << "while (";
    printImpl(cell[kb.ids.condition]);
    m_ss << ") ";
    printImpl(cell[kb.ids.statement]);
}

void CellValuePrinter::printOpConstVar(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    if (cell.has(kb.ids.ast)) {
        CellI& ast = cell[kb.ids.ast];
        if (&ast.struct_() == &kb.std.ast.Cell) {
            m_ss << ast[kb.ids.value].label();
            return;
        }
        if (&ast.struct_() == &kb.std.ast.Var) {
            m_ss << "var_" << ast[kb.ids.role].label();
            return;
        }
    }
    if (cell.has(kb.ids.value)) {
        m_ss << cell[kb.ids.value].label();
    } else {
        m_ss << "<empty>";
    }
}

void CellValuePrinter::printOpVar(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    cell.label().empty() ? (m_ss << "<empty>") : (m_ss << cell.label());
}

void CellValuePrinter::printOpNew(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    m_ss << "new ";
    printImpl(cell[kb.ids.objectType]);
}

void CellValuePrinter::printOpSame(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.ids.lhs]);
    m_ss << " is ";
    printImpl(cell[kb.ids.rhs]);
}

void CellValuePrinter::printOpNotSame(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.ids.lhs]);
    m_ss << " is not ";
    printImpl(cell[kb.ids.rhs]);
}

void CellValuePrinter::printOpEqual(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.ids.lhs]);
    m_ss << " == ";
    printImpl(cell[kb.ids.rhs]);
}

void CellValuePrinter::printOpNotEqual(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.ids.lhs]);
    m_ss << " != ";
    printImpl(cell[kb.ids.rhs]);
}

void CellValuePrinter::printOpHas(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.ids.cell]);
    m_ss << " has ";
    printImpl(cell[kb.ids.role]);
}

void CellValuePrinter::printOpMissing(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.ids.cell]);
    m_ss << " missing ";
    printImpl(cell[kb.ids.role]);
}

void CellValuePrinter::printOpGet(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    if (&cell[kb.ids.ast].struct_() == &kb.std.ast.Member) {
        m_ss << "m_" << cell[kb.ids.ast][kb.ids.role].label();
        return;
    }
    if (&cell[kb.ids.ast].struct_() == &kb.std.ast.Self) {
        m_ss << "self";
        return;
    }
    if (&cell[kb.ids.ast].struct_() == &kb.std.ast.Parameter) {
        m_ss << "p_" << cell[kb.ids.ast][kb.ids.role].label();
        return;
    }
    if (&cell[kb.ids.ast].struct_() == &kb.std.ast.Var) {
        m_ss << "var_" << cell[kb.ids.ast][kb.ids.name].label();
        return;
    }
    printImpl(cell[kb.ids.cell]);
    m_ss << ".";
    printImpl(cell[kb.ids.role]);
}

void CellValuePrinter::printOpAnd(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.ids.lhs]);
    m_ss << " and ";
    printImpl(cell[kb.ids.rhs]);
}

void CellValuePrinter::printOpOr(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.ids.lhs]);
    m_ss << " or ";
    printImpl(cell[kb.ids.rhs]);
}

void CellValuePrinter::printOpNot(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    m_ss << "not(";
    printImpl(cell[kb.ids.input]);
    m_ss << ")";
}

void CellValuePrinter::printOpAdd(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.ids.lhs]);
    m_ss << " + ";
    printImpl(cell[kb.ids.rhs]);
}

void CellValuePrinter::printOpSubtract(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.ids.lhs]);
    m_ss << " - ";
    printImpl(cell[kb.ids.rhs]);
}

void CellValuePrinter::printOpMultiply(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.ids.lhs]);
    m_ss << " * ";
    printImpl(cell[kb.ids.rhs]);
}

void CellValuePrinter::printOpDivide(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.ids.lhs]);
    m_ss << " / ";
    printImpl(cell[kb.ids.rhs]);
}

void CellValuePrinter::printOpLessThan(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.ids.lhs]);
    m_ss << " < ";
    printImpl(cell[kb.ids.rhs]);
}

void CellValuePrinter::printOpLessThanOrEqual(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.ids.lhs]);
    m_ss << " <= ";
    printImpl(cell[kb.ids.rhs]);
}

void CellValuePrinter::printOpGreaterThan(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.ids.lhs]);
    m_ss << " > ";
    printImpl(cell[kb.ids.rhs]);
}

void CellValuePrinter::printOpGreaterThanOrEqual(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.ids.lhs]);
    m_ss << " >= ";
    printImpl(cell[kb.ids.rhs]);
}

void CellValuePrinter::printAstCell(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    m_ss << cell[kb.ids.value].label();
}

void CellValuePrinter::printAstGet(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    if (&cell.struct_() == &kb.std.ast.Member) {
        m_ss << "m_" << cell[kb.ids.ast][kb.ids.role].label();
        return;
    }
    if (&cell.struct_() == &kb.std.ast.Self) {
        m_ss << "self";
        return;
    }
    if (&cell.struct_() == &kb.std.ast.Parameter) {
        m_ss << "p_" << cell[kb.ids.ast][kb.ids.role].label();
        return;
    }
    if (&cell.struct_() == &kb.std.ast.Var) {
        m_ss << "var_" << cell[kb.ids.ast][kb.ids.role].label();
        return;
    }
    printImpl(cell[kb.ids.cell]);
    m_ss << ".";
    printImpl(cell[kb.ids.role]);
}

void CellValuePrinter::printAstParameter(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    m_ss << "p_" << cell[kb.ids.role].label();
}

void CellValuePrinter::printAstVar(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    m_ss << "var_" << cell[kb.ids.name].label();
}

void CellValuePrinter::printAstMember(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    m_ss << "m_" << cell[kb.ids.role].label();
}

void CellValuePrinter::printAstSubtract(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.ids.lhs]);
    m_ss << " - ";
    printImpl(cell[kb.ids.rhs]);
}

void CellValuePrinter::printOpReturn(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    m_ss << "return";
    if (cell.has(kb.ids.result)) {
        m_ss << " ";
        printImpl(cell[kb.ids.result][kb.ids.value]);
    }
}

void CellValuePrinter::printTypeName(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    auto isA          = [this, &cell, &kb](CellI& type) -> bool { return &cell == &type || (cell.has(kb.ids.memberOf) && cell[kb.ids.memberOf][kb.ids.index].has(type)); };
    if (isA(kb.std.Map)) {
        m_ss << std::format("Map<{}, {}>", cell[kb.ids.subTypes][kb.ids.index][kb.ids.keyType][kb.ids.value].label(), cell[kb.ids.subTypes][kb.ids.index][kb.ids.valueType][kb.ids.value].label());
        return;
    } else if (isA(kb.std.ListItem)) {
        if (&cell == &kb.std.ListItem) {
            m_ss << "ListItem";
            return;
        }
        m_ss << std::format("ListItem<{}>", cell[kb.ids.subTypes][kb.ids.index][kb.ids.valueType][kb.ids.value].label());
        return;
    } else if (isA(kb.std.List)) {
        if (&cell == &kb.std.List) {
            m_ss << "List";
            return;
        }
        m_ss << std::format("List<{}>", cell[kb.ids.subTypes][kb.ids.index][kb.ids.valueType][kb.ids.value].label());
        return;
    }
    m_ss << cell.label();
}

void CellValuePrinter::printImpl(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    auto is          = [this, &cell, &kb](CellI& type) -> bool { return &cell.struct_() == &type || (cell.struct_().has(kb.ids.memberOf) && cell.struct_()[kb.ids.memberOf][kb.ids.index].has(type)); };

    if (is(kb.std.Slot)) {
        CellI& cellType = cell[kb.ids.slotType];
        m_ss << cell[kb.ids.slotRole].label() << ": ";
        if (cellType.struct_().has(kb.ids.memberOf) && cellType.struct_()[kb.ids.memberOf][kb.ids.index].has(kb.std.ListItem)) {
            m_ss << "ListItem<" << cellType.struct_()[kb.ids.slots][kb.ids.index][kb.ids.value][kb.ids.value][kb.ids.slotType].label() << ">";
        } else {
            m_ss << cellType.label();
        }
        return;
    } else if (is(kb.std.List)) {
        if (&cell.struct_()[kb.ids.subTypes][kb.ids.index][kb.ids.valueType][kb.ids.value] == &kb.std.Char) {
            m_ss << cell.label();
            return;
        }
        printTypeName(cell.struct_());
        m_ss << "[";
        visitList(cell, [this](CellI& value, int i, bool&) {
            if (i != 0) {
                m_ss << ", ";
            }
            value.accept(*this);
        });
        m_ss << "]";
        return;
    } else if (is(kb.std.Map)) {
        if (!cell.has(kb.ids.list)) {
            m_ss << "{}";
            return;
        }
        printTypeName(cell.struct_());
        m_ss << "{";
        visitList(cell[kb.ids.index][kb.ids.struct_][kb.ids.slots][kb.ids.list], [this, &kb, &cell](CellI& value, int i, bool&) {
            if (i != 0) {
                m_ss << ", ";
            }
            value[kb.ids.slotRole].accept(*this);
            m_ss << ": {";
            cell[kb.ids.index][value[kb.ids.slotRole]][kb.ids.value].accept(*this);
            m_ss << "}";
        });
        m_ss << "}";
        return;
    } else if (is(kb.std.Struct)) {
        CellI& type = cell;
        m_ss << "Type ";
        printTypeName(cell);
        if (type.has(kb.ids.memberOf)) {
            visitList(type[kb.ids.memberOf][kb.ids.list], [this, &kb](CellI& member, int i, bool&) {
                if (i != 0) {
                    m_ss << ", ";
                } else {
                    m_ss << " : ";
                }
                m_ss << member.label();
            });
        }
        m_ss << " { ";
        if (type.has(kb.ids.slots)) {
            visitList(type[kb.ids.slots][kb.ids.list], [this, &kb](CellI& slot, int i, bool&) {
                if (i != 0) {
                    m_ss << ", ";
                }
                m_ss << slot[kb.ids.slotRole].label() << ": ";
                printTypeName(slot[kb.ids.slotType]);
            });
        }
        m_ss << " }";
        return;
    } else if (&cell == &kb.ids.red || &cell == &kb.ids.green || &cell == &kb.ids.blue) {
        m_ss << cell.label();
        return;
    } else if (is(kb.std.op.Block)) {
        printOpBlock(cell);
        return;
    } else if (is(kb.std.op.EvalVar)) {
        printOpEvalVar(cell);
        return;
    } else if (is(kb.std.op.Function)) {
        printOpFunction(cell);
        return;
    } else if (is(kb.std.op.Delete)) {
        printOpDelete(cell);
        return;
    } else if (is(kb.std.op.Set)) {
        printOpSet(cell);
        return;
    } else if (is(kb.std.op.Erase)) {
        printOpErase(cell);
        return;
    } else if (is(kb.std.op.If)) {
        printOpIf(cell);
        return;
    } else if (is(kb.std.op.Do)) {
        printOpDo(cell);
        return;
    } else if (is(kb.std.op.While)) {
        printOpWhile(cell);
        return;
    } else if (is(kb.std.op.ConstVar)) {
        printOpConstVar(cell);
        return;
    } else if (is(kb.std.op.Var)) {
        printOpVar(cell);
        return;
    } else if (is(kb.std.op.New)) {
        printOpNew(cell);
        return;
    } else if (is(kb.std.op.Same)) {
        printOpSame(cell);
        return;
    } else if (is(kb.std.op.NotSame)) {
        printOpNotSame(cell);
        return;
    } else if (is(kb.std.op.Equal)) {
        printOpEqual(cell);
        return;
    } else if (is(kb.std.op.NotEqual)) {
        printOpNotEqual(cell);
        return;
    } else if (is(kb.std.op.Has)) {
        printOpHas(cell);
        return;
    } else if (is(kb.std.op.Missing)) {
        printOpMissing(cell);
        return;
    } else if (is(kb.std.op.Get)) {
        printOpGet(cell);
        return;
    } else if (is(kb.std.op.And)) {
        printOpAnd(cell);
        return;
    } else if (is(kb.std.op.Or)) {
        printOpOr(cell);
        return;
    } else if (is(kb.std.op.Not)) {
        printOpNot(cell);
        return;
    } else if (is(kb.std.op.Add)) {
        printOpAdd(cell);
        return;
    } else if (is(kb.std.op.Subtract)) {
        printOpSubtract(cell);
        return;
    } else if (is(kb.std.op.Multiply)) {
        printOpMultiply(cell);
        return;
    } else if (is(kb.std.op.Divide)) {
        printOpDivide(cell);
        return;
    } else if (is(kb.std.op.LessThan)) {
        printOpLessThan(cell);
        return;
    } else if (is(kb.std.op.LessThanOrEqual)) {
        printOpLessThanOrEqual(cell);
        return;
    } else if (is(kb.std.op.GreaterThan)) {
        printOpGreaterThan(cell);
        return;
    } else if (is(kb.std.op.GreaterThanOrEqual)) {
        printOpGreaterThanOrEqual(cell);
        return;
    } else if (is(kb.std.op.Return)) {
        printOpReturn(cell);
        return;
    } else if (is(kb.std.ast.Cell)) {
        printAstCell(cell);
        return;
    } else if (is(kb.std.ast.Get)) {
        printAstGet(cell);
        return;
    } else if (is(kb.std.ast.Parameter)) {
        printAstParameter(cell);
        return;
    } else if (is(kb.std.ast.Var)) {
        printAstVar(cell);
        return;
    } else if (is(kb.std.ast.Member)) {
        printAstMember(cell);
        return;
    } else if (is(kb.std.ast.Subtract)) {
        printAstSubtract(cell);
        return;
    }

    if (!cell.label().empty()) {
        m_ss << cell.label() << ": ";
    }
    m_ss << cell.struct_().label() << " { ";

    if (cell.struct_().has(kb.ids.slots)) {
        visitList(cell.struct_()[kb.ids.slots][kb.ids.list], [this, &kb](CellI& slot, int i, bool&) {
            if (i != 0) {
                m_ss << ", ";
            }
            m_ss << ".";
            slot.accept(*this);
        });
    }

    m_ss << " }";
}

void CellValuePrinter::visit(Number& cell)
{
    m_ss << cell.value();
}

void CellValuePrinter::visit(String& cell)
{
    m_ss << "(String) \"" << cell.value() << "\"";
}

void CellValuePrinter::visit(hybrid::Color& cell)
{
    m_ss << "Color(" << (int)cell.color().m_red << "," << (int)cell.color().m_green << "," << (int)cell.color().m_blue << ")";
}

void CellValuePrinter::visit(hybrid::Pixel& cell)
{
    m_ss << "Pixel[" << (int)cell.color().m_red << "," << (int)cell.color().m_green << "," << (int)cell.color().m_blue << "]";
}

void CellValuePrinter::visit(hybrid::Picture& cell)
{
    m_ss << "(Picture)" << cell.label() << "[" << cell.width() << ", " << cell.height() << "]";
}

void CellValuePrinter::printIndent()
{
    for (int i = 0; i < m_indent; ++i) {
        for (int j = 0; j < m_tabSize; ++j) {
            m_ss << " ";
        }
    }
}

std::string CellValuePrinter::print() const
{
    m_indent = 0;
    return m_ss.str();
}

} // namespace cells
} // namespace synth