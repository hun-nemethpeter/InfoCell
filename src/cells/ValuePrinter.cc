#include <format>

#include "ValuePrinter.h"
#include "Brain.h"

namespace synth {
namespace cells {

void CellValuePrinter::visit(Map::Index::Type::Slots::SlotList::Item& cell)
{
    printImpl(cell);
}

void CellValuePrinter::visit(Map::Index::Type::Slots::SlotList& cell)
{
    printImpl(cell);
}

void CellValuePrinter::visit(Map::Index::Type::Slots::SlotIndex& cell)
{
    printImpl(cell);
}

void CellValuePrinter::visit(Map::Index::Type::Slots& cell)
{
    printImpl(cell);
}

void CellValuePrinter::visit(Map::Index::Type::Slot& cell)
{
    printImpl(cell);
}

void CellValuePrinter::visit(Map::Index::Type& cell)
{
    printImpl(cell);
}

void CellValuePrinter::visit(Map::Index& cell)
{
    printImpl(cell);
}

void CellValuePrinter::visit(Map& cell)
{
    printImpl(cell);
}

void CellValuePrinter::visit(nextgen::List::Item& cell)
{
    printImpl(cell);
}

void CellValuePrinter::visit(nextgen::List& cell)
{
    printImpl(cell);
}

void CellValuePrinter::visit(nextgen::Type& cell)
{
    printImpl(cell);
}

void CellValuePrinter::visit(nextgen::Index& cell)
{
    printImpl(cell);
}

void CellValuePrinter::visit(nextgen::Map& cell)
{
    printImpl(cell);
}

void CellValuePrinter::visit(nextgen::Set& cell)
{
    printImpl(cell);
}

void CellValuePrinter::visit(Object& object)
{
    printImpl(object);
}

void CellValuePrinter::printOpBlock(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    CellI& ast       = cell[kb.id.ast];
    if (&ast.type() == &kb.type.ast.Call || &ast.type() == &kb.type.ast.StaticCall) {
        if (&ast[kb.id.cell].type() == &kb.type.ast.Get) {
            printImpl(ast[kb.id.cell]);
        }
        if (&ast[kb.id.cell].type() == &kb.type.ast.Self) {
            m_ss << "self";
        }
        if (&ast[kb.id.cell].type() == &kb.type.ast.Cell) {
            m_ss << ast[kb.id.cell][kb.id.value].label();
        }
        if (&ast[kb.id.cell].type() == &kb.type.ast.Member) {
            m_ss << "m_" << ast[kb.id.cell][kb.id.role].label();
        }
        if (ast.has(kb.id.method)) {
            if (&ast.type() == &kb.type.ast.Call) {
                m_ss << ".";
            } else {
                m_ss << "::";
            }
            m_ss << ast[kb.id.method][kb.id.value].label();
            m_ss << "(";
            if (ast.has(kb.id.parameters)) {
                visitList(ast[kb.id.parameters], [this, &kb](CellI& slot, int i, bool&) {
                    if (i != 0) {
                        m_ss << ", ";
                    }
                    m_ss << slot[kb.id.slotRole][kb.id.value].label();
                    m_ss << ": ";
                    printImpl(slot[kb.id.slotType]);
                });
            }
            m_ss << ")";
        }
        return;
    }
    if (&ast.type() == &kb.type.ast.New) {
        m_ss << "new ";
        printImpl(ast[kb.id.objectType]);
        if (ast.has(kb.id.constructor)) {
            m_ss << ".";
            m_ss << ast[kb.id.constructor][kb.id.value].label();
            m_ss << "(";
            if (ast.has(kb.id.parameters)) {
                visitList(ast[kb.id.parameters], [this, &kb](CellI& slot, int i, bool&) {
                    if (i != 0) {
                        m_ss << ", ";
                    }
                    m_ss << slot[kb.id.slotRole][kb.id.value].label();
                    m_ss << ": ";
                    printImpl(slot[kb.id.slotType]);
                });

            }
            m_ss << ")";
        }
        return;
    }
    m_ss << "{\n";
    m_indent++;
    Visitor::visitList(cell[kb.id.ops], [this](CellI& op, int, bool&) {
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
    printImpl(cell[kb.id.value]);
    m_ss << ")";
}

void CellValuePrinter::printOpFunction(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    std::stringstream iss;
    std::stringstream oss;
    CellI& subTypesIndex = cell.type()[kb.id.subTypes][kb.id.index];
    CellI& inType        = subTypesIndex[kb.id.input];
    CellI& outType       = subTypesIndex[kb.id.output];
    bool hasOutput = false;
    if (inType.has(kb.id.slots)) {
        Visitor::visitList(inType[kb.id.slots][kb.id.list], [this, &iss, &kb](CellI& slot, int i, bool& stop) {
            if (i > 0) {
                iss << ", ";
            }
            if (&slot[kb.id.slotRole] != &kb.id.self) {
                iss << "in_";
            }
            iss << slot[kb.id.slotRole].label() << ": " << slot[kb.id.slotType].label();
        });
    }
    if (outType.has(kb.id.slots)) {
        hasOutput = true;
        Visitor::visitList(outType[kb.id.slots][kb.id.list], [this, &oss, &kb](CellI& slot, int i, bool& stop) {
            if (i > 0) {
                oss << ", ";
            }
            oss << "out_" << slot[kb.id.slotRole].label() << ": " << slot[kb.id.slotType].label();
        });
    }
    const std::string& className = subTypesIndex.has(kb.id.objectType) ? subTypesIndex[kb.id.objectType].label() : "";
    std::string label            = className;
    if (!className.empty()) {
        label += "::";
    }
    label += subTypesIndex[kb.id.name].label();
    bool isStatic = cell.has(kb.id.static_);
    std::string staticStr = isStatic ? "static " : "";
    std::string newLabel;

    if (hasOutput) {
        newLabel = std::format("fn {}{}({}) -> ({})\n", staticStr, label, iss.str(), oss.str());
    } else {
        newLabel = std::format("fn {}{}({})\n", staticStr, label, iss.str());
    }
    m_ss << newLabel;

    printImpl(cell[kb.id.op]);
}

void CellValuePrinter::printOpDelete(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    m_ss << "delete (";
    printImpl(cell[kb.id.input]);
    m_ss << ")";
}

void CellValuePrinter::printOpSet(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    if ((&cell[kb.id.ast][kb.id.cell].type() == &kb.type.ast.Self) && (&cell[kb.id.ast][kb.id.role].type() == &kb.type.ast.Cell)) {
        m_ss << "m_";
    } else {
        printImpl(cell[kb.id.cell]);
        m_ss << ".";
    }
    printImpl(cell[kb.id.role]);
    m_ss << " = ";
    printImpl(cell[kb.id.value]);
}

void CellValuePrinter::printOpErase(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.id.cell]);
    m_ss << ".erase(";
    printImpl(cell[kb.id.role]);
    m_ss << ")";
}

void CellValuePrinter::printOpIf(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    m_ss << "if ";
    printImpl(cell[kb.id.condition]);
    m_ss << " then ";
    if (&cell[kb.id.then].type() != &kb.type.op.Block) {
        m_ss << "\n";
        m_indent++;
        printIndent();
        m_indent--;
    }
    printImpl(cell[kb.id.then]);
    if (cell.has(kb.id.else_)) {
        if (&cell[kb.id.then].type() != &kb.type.op.Block) {
            m_ss << ";\n";
            printIndent();
        }
        m_ss << " else ";
        if (&cell[kb.id.else_].type() != &kb.type.op.Block) {
            m_ss << "\n";
            m_indent++;
            printIndent();
            m_indent--;
        }
        printImpl(cell[kb.id.else_]);
    }
}

void CellValuePrinter::printOpDo(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    m_ss << "do ";
    printImpl(cell[kb.id.statement]);
    m_ss << " (";
    printImpl(cell[kb.id.condition]);
    m_ss << ")";
}

void CellValuePrinter::printOpWhile(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    m_ss << "while )";
    printImpl(cell[kb.id.condition]);
    m_ss << ") ";
    printImpl(cell[kb.id.statement]);
}

void CellValuePrinter::printOpConstVar(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    if (cell.has(kb.id.ast)) {
        CellI& ast = cell[kb.id.ast];
        if (&ast.type() == &kb.type.ast.Cell) {
            m_ss << ast[kb.id.value].label();
            return;
        }
        if (&ast.type() == &kb.type.ast.Var) {
            m_ss << "var_" << ast[kb.id.role].label();
            return;
        }
    }
    if (cell.has(kb.id.value)) {
        m_ss << cell[kb.id.value].label();
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
    printImpl(cell[kb.id.objectType]);
}

void CellValuePrinter::printOpSame(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.id.lhs]);
    m_ss << " is ";
    printImpl(cell[kb.id.rhs]);
}

void CellValuePrinter::printOpNotSame(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.id.lhs]);
    m_ss << " is not ";
    printImpl(cell[kb.id.rhs]);
}

void CellValuePrinter::printOpEqual(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.id.lhs]);
    m_ss << " == ";
    printImpl(cell[kb.id.rhs]);
}

void CellValuePrinter::printOpNotEqual(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.id.lhs]);
    m_ss << " != ";
    printImpl(cell[kb.id.rhs]);
}

void CellValuePrinter::printOpHas(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.id.cell]);
    m_ss << " has ";
    printImpl(cell[kb.id.role]);
}

void CellValuePrinter::printOpMissing(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.id.cell]);
    m_ss << " missing ";
    printImpl(cell[kb.id.role]);
}

void CellValuePrinter::printOpGet(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    if (&cell[kb.id.ast].type() == &kb.type.ast.Member) {
        m_ss << "m_" << cell[kb.id.ast][kb.id.role].label();
        return;
    }
    if (&cell[kb.id.ast].type() == &kb.type.ast.Self) {
        m_ss << "self";
        return;
    }
    if (&cell[kb.id.ast].type() == &kb.type.ast.Input) {
        m_ss << "in_" << cell[kb.id.ast][kb.id.role].label();
        return;
    }
    if (&cell[kb.id.ast].type() == &kb.type.ast.Output) {
        m_ss << "out_" << cell[kb.id.ast][kb.id.role].label();
        return;
    }
    if (&cell[kb.id.ast].type() == &kb.type.ast.Var) {
        m_ss << "var_" << cell[kb.id.ast][kb.id.role].label();
        return;
    }
    printImpl(cell[kb.id.cell]);
    m_ss << ".";
    printImpl(cell[kb.id.role]);
}

void CellValuePrinter::printOpAnd(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.id.lhs]);
    m_ss << " and ";
    printImpl(cell[kb.id.rhs]);
}

void CellValuePrinter::printOpOr(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.id.lhs]);
    m_ss << " or ";
    printImpl(cell[kb.id.rhs]);
}

void CellValuePrinter::printOpNot(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    m_ss << "not(";
    printImpl(cell[kb.id.input]);
    m_ss << ")";
}

void CellValuePrinter::printOpAdd(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.id.lhs]);
    m_ss << " + ";
    printImpl(cell[kb.id.rhs]);
}

void CellValuePrinter::printOpSubtract(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.id.lhs]);
    m_ss << " - ";
    printImpl(cell[kb.id.rhs]);
}

void CellValuePrinter::printOpMultiply(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.id.lhs]);
    m_ss << " * ";
    printImpl(cell[kb.id.rhs]);
}

void CellValuePrinter::printOpDivide(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.id.lhs]);
    m_ss << " / ";
    printImpl(cell[kb.id.rhs]);
}

void CellValuePrinter::printOpLessThan(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.id.lhs]);
    m_ss << " < ";
    printImpl(cell[kb.id.rhs]);
}

void CellValuePrinter::printOpLessThanOrEqual(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.id.lhs]);
    m_ss << " <= ";
    printImpl(cell[kb.id.rhs]);
}

void CellValuePrinter::printOpGreaterThan(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.id.lhs]);
    m_ss << " > ";
    printImpl(cell[kb.id.rhs]);
}

void CellValuePrinter::printOpGreaterThanOrEqual(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.id.lhs]);
    m_ss << " >= ";
    printImpl(cell[kb.id.rhs]);
}

void CellValuePrinter::printAstCell(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    m_ss << cell[kb.id.value].label();
}

void CellValuePrinter::printAstGet(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    if (&cell.type() == &kb.type.ast.Member) {
        m_ss << "m_" << cell[kb.id.ast][kb.id.role].label();
        return;
    }
    if (&cell.type() == &kb.type.ast.Self) {
        m_ss << "self";
        return;
    }
    if (&cell.type() == &kb.type.ast.Input) {
        m_ss << "in_" << cell[kb.id.ast][kb.id.role].label();
        return;
    }
    if (&cell.type() == &kb.type.ast.Output) {
        m_ss << "out_" << cell[kb.id.ast][kb.id.role].label();
        return;
    }
    if (&cell.type() == &kb.type.ast.Var) {
        m_ss << "var_" << cell[kb.id.ast][kb.id.role].label();
        return;
    }
    printImpl(cell[kb.id.cell]);
    m_ss << ".";
    printImpl(cell[kb.id.role]);
}

void CellValuePrinter::printAstInput(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    m_ss << "in_" << cell[kb.id.role].label();
}

void CellValuePrinter::printAstOutput(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    m_ss << "out_" << cell[kb.id.role].label();
}

void CellValuePrinter::printAstVar(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    m_ss << "var_" << cell[kb.id.role].label();
}

void CellValuePrinter::printAstMember(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    m_ss << "m_" << cell[kb.id.role].label();
}

void CellValuePrinter::printAstSubtract(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.id.lhs]);
    m_ss << " - ";
    printImpl(cell[kb.id.rhs]);
}

void CellValuePrinter::printOpReturn(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    m_ss << "return";
    if (cell.has(kb.id.result)) {
        m_ss << " ";
        printImpl(cell[kb.id.result][kb.id.value]);
    }
}

void CellValuePrinter::printTypeName(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    auto isA          = [this, &cell, &kb](CellI& type) -> bool { return &cell == &type || (cell.has(kb.id.memberOf) && cell[kb.id.memberOf][kb.id.index].has(type)); };
    if (isA(kb.type.Map)) {
        m_ss << std::format("Map<{}, {}>", cell[kb.id.subTypes][kb.id.index][kb.id.keyType].label(), cell[kb.id.subTypes][kb.id.index][kb.id.objectType].label());
        return;
    } else if (isA(kb.type.ListItem)) {
        m_ss << std::format("ListItem<{}>", cell[kb.id.slots][kb.id.index][kb.id.value][kb.id.slotType].label());
        return;
    } else if (isA(kb.type.List)) {
        m_ss << std::format("List<{}>", cell[kb.id.slots][kb.id.index][kb.id.objectType][kb.id.slotType].label());
        return;
    }
    m_ss << cell.label();
}

void CellValuePrinter::printImpl(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    auto is          = [this, &cell, &kb](CellI& type) -> bool { return &cell.type() == &type || (cell.type().has(kb.id.memberOf) && cell.type()[kb.id.memberOf][kb.id.index].has(type)); };

    if (is(kb.type.Slot)) {
        CellI& cellType = cell[kb.id.slotType];
        m_ss << cell[kb.id.slotRole].label() << ": ";
        if (cellType.type().has(kb.id.memberOf) && cellType.type()[kb.id.memberOf][kb.id.index].has(kb.type.ListItem)) {
            m_ss << "ListItem<" << cellType.type()[kb.id.slots][kb.id.index][kb.id.value][kb.id.slotType].label() << ">";
        } else {
            m_ss << cellType.label();
        }
        return;
    } else if (is(kb.type.List)) {
        printTypeName(cell.type());
        m_ss << "[";
        visitList(cell, [this](CellI& value, int i, bool&) {
            if (i != 0) {
                m_ss << ", ";
            }
            value.accept(*this);
        });
        m_ss << "]";
        return;
    } else if (is(kb.type.Map)) {
        if (!cell.has(kb.id.list)) {
            m_ss << "{}";
            return;
        }
        printTypeName(cell.type());
        m_ss << "{";
        visitList(cell[kb.id.index][kb.id.type][kb.id.slots][kb.id.list], [this, &kb, &cell](CellI& value, int i, bool&) {
            if (i != 0) {
                m_ss << ", ";
            }
            value[kb.id.slotRole].accept(*this);
            m_ss << ": ";
            cell[kb.id.index][value[kb.id.slotRole]].accept(*this);
        });
        m_ss << "}";
        return;
    } else if (is(kb.type.Type_)) {
        CellI& type = cell;
        m_ss << "Type ";
        printTypeName(cell);
        if (type.has(kb.id.memberOf)) {
            visitList(type[kb.id.memberOf][kb.id.list], [this, &kb](CellI& member, int i, bool&) {
                if (i != 0) {
                    m_ss << ", ";
                } else {
                    m_ss << " : ";
                }
                m_ss << member.label();
            });
        }
        m_ss << " { ";
        if (type.has(kb.id.slots)) {
            visitList(type[kb.id.slots][kb.id.list], [this, &kb](CellI& slot, int i, bool&) {
                if (i != 0) {
                    m_ss << ", ";
                }
                m_ss << slot[kb.id.slotRole].label() << ": ";
                printTypeName(slot[kb.id.slotType]);
            });
        }
        m_ss << " }";
        return;
    } else if (&cell == &kb.colors.red || &cell == &kb.colors.green || &cell == &kb.colors.blue) {
        m_ss << cell.label();
        return;
    } else if (is(kb.type.op.Block)) {
        printOpBlock(cell);
        return;
    } else if (is(kb.type.op.EvalVar)) {
        printOpEvalVar(cell);
        return;
    } else if (is(kb.type.op.Function)) {
        printOpFunction(cell);
        return;
    } else if (is(kb.type.op.Delete)) {
        printOpDelete(cell);
        return;
    } else if (is(kb.type.op.Set)) {
        printOpSet(cell);
        return;
    } else if (is(kb.type.op.Erase)) {
        printOpErase(cell);
        return;
    } else if (is(kb.type.op.If)) {
        printOpIf(cell);
        return;
    } else if (is(kb.type.op.Do)) {
        printOpDo(cell);
        return;
    } else if (is(kb.type.op.While)) {
        printOpWhile(cell);
        return;
    } else if (is(kb.type.op.ConstVar)) {
        printOpConstVar(cell);
        return;
    } else if (is(kb.type.op.Var)) {
        printOpVar(cell);
        return;
    } else if (is(kb.type.op.New)) {
        printOpNew(cell);
        return;
    } else if (is(kb.type.op.Same)) {
        printOpSame(cell);
        return;
    } else if (is(kb.type.op.NotSame)) {
        printOpNotSame(cell);
        return;
    } else if (is(kb.type.op.Equal)) {
        printOpEqual(cell);
        return;
    } else if (is(kb.type.op.NotEqual)) {
        printOpNotEqual(cell);
        return;
    } else if (is(kb.type.op.Has)) {
        printOpHas(cell);
        return;
    } else if (is(kb.type.op.Missing)) {
        printOpMissing(cell);
        return;
    } else if (is(kb.type.op.Get)) {
        printOpGet(cell);
        return;
    } else if (is(kb.type.op.And)) {
        printOpAnd(cell);
        return;
    } else if (is(kb.type.op.Or)) {
        printOpOr(cell);
        return;
    } else if (is(kb.type.op.Not)) {
        printOpNot(cell);
        return;
    } else if (is(kb.type.op.Add)) {
        printOpAdd(cell);
        return;
    } else if (is(kb.type.op.Subtract)) {
        printOpSubtract(cell);
        return;
    } else if (is(kb.type.op.Multiply)) {
        printOpMultiply(cell);
        return;
    } else if (is(kb.type.op.Divide)) {
        printOpDivide(cell);
        return;
    } else if (is(kb.type.op.LessThan)) {
        printOpLessThan(cell);
        return;
    } else if (is(kb.type.op.LessThanOrEqual)) {
        printOpLessThanOrEqual(cell);
        return;
    } else if (is(kb.type.op.GreaterThan)) {
        printOpGreaterThan(cell);
        return;
    } else if (is(kb.type.op.GreaterThanOrEqual)) {
        printOpGreaterThanOrEqual(cell);
        return;
    } else if (is(kb.type.op.Return)) {
        printOpReturn(cell);
        return;
    } else if (is(kb.type.ast.Cell)) {
        printAstCell(cell);
        return;
    } else if (is(kb.type.ast.Get)) {
        printAstGet(cell);
        return;
    } else if (is(kb.type.ast.Input)) {
        printAstInput(cell);
        return;
    } else if (is(kb.type.ast.Output)) {
        printAstOutput(cell);
        return;
    } else if (is(kb.type.ast.Var)) {
        printAstVar(cell);
        return;
    } else if (is(kb.type.ast.Member)) {
        printAstMember(cell);
        return;
    } else if (is(kb.type.ast.Subtract)) {
        printAstSubtract(cell);
        return;
    }

    if (!cell.label().empty()) {
        m_ss << cell.label() << ": ";
    }
    m_ss << cell.type().label() << " { ";

    if (cell.type().has(kb.id.slots)) {
        visitList(cell.type()[kb.id.slots][kb.id.list], [this, &kb](CellI& slot, int i, bool&) {
            if (i != 0) {
                m_ss << ", ";
            }
            m_ss << ".";
            slot.accept(*this);
        });
    }

    m_ss << " }";
}

void CellValuePrinter::visit(List::Item& listItemCell)
{
    brain::Brain& kb = listItemCell.kb;
    m_ss << "[ ";
    if (!listItemCell[kb.id.value].label().empty()) {
        m_ss << listItemCell[kb.id.value].label() << " ";
    }
    m_ss << "]";

}

void CellValuePrinter::visit(List& list)
{
    m_ss << "[";
    visitList(list, [this](CellI& value, int i, bool&) {
        if (i != 0) {
            m_ss << ",";
        }
        m_ss << " ";
        value.accept(*this);
    });
    m_ss << " ]";
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

void visit(nextgen::List::Item&)
{
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