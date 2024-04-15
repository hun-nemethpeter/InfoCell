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

void CellValuePrinter::visit(Type& cell)
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
    if (&ast.type() == &kb.type.ast.Call || &ast.type() == &kb.type.ast.StaticCall) {
        if (&ast[kb.ids.cell].type() == &kb.type.ast.Get) {
            printImpl(ast[kb.ids.cell]);
        }
        if (&ast[kb.ids.cell].type() == &kb.type.ast.Self) {
            m_ss << "self";
        }
        if (&ast[kb.ids.cell].type() == &kb.type.ast.Cell) {
            m_ss << ast[kb.ids.cell][kb.ids.value].label();
        }
        if (&ast[kb.ids.cell].type() == &kb.type.ast.Member) {
            m_ss << "m_" << ast[kb.ids.cell][kb.ids.role].label();
        }
        if (ast.has(kb.ids.method)) {
            if (&ast.type() == &kb.type.ast.Call) {
                m_ss << ".";
            } else {
                m_ss << "::";
            }
            m_ss << ast[kb.ids.method][kb.ids.value].label();
            m_ss << "(";
            if (ast.has(kb.ids.parameters)) {
                visitList(ast[kb.ids.parameters], [this, &kb](CellI& slot, int i, bool&) {
                    if (i != 0) {
                        m_ss << ", ";
                    }
                    m_ss << slot[kb.ids.slotRole][kb.ids.value].label();
                    m_ss << ": ";
                    printImpl(slot[kb.ids.slotType]);
                });
            }
            m_ss << ")";
        }
        return;
    }
    if (&ast.type() == &kb.type.ast.New) {
        m_ss << "new ";
        printImpl(ast[kb.ids.objectType]);
        if (ast.has(kb.ids.constructor)) {
            m_ss << ".";
            m_ss << ast[kb.ids.constructor][kb.ids.value].label();
            m_ss << "(";
            if (ast.has(kb.ids.parameters)) {
                visitList(ast[kb.ids.parameters], [this, &kb](CellI& slot, int i, bool&) {
                    if (i != 0) {
                        m_ss << ", ";
                    }
                    m_ss << slot[kb.ids.slotRole][kb.ids.value].label();
                    m_ss << ": ";
                    printImpl(slot[kb.ids.slotType]);
                });

            }
            m_ss << ")";
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
    CellI& subTypesIndex = cell.type()[kb.ids.subTypes][kb.ids.index];
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
    const std::string& className = subTypesIndex.has(kb.ids.objectType) ? subTypesIndex[kb.ids.objectType].label() : "";
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
    if ((&cell[kb.ids.ast][kb.ids.cell].type() == &kb.type.ast.Self) && (&cell[kb.ids.ast][kb.ids.role].type() == &kb.type.ast.Cell)) {
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
    m_ss << "if ";
    printImpl(cell[kb.ids.condition]);
    m_ss << " then ";
    if (&cell[kb.ids.then].type() != &kb.type.op.Block) {
        m_ss << "\n";
        m_indent++;
        printIndent();
        m_indent--;
    }
    printImpl(cell[kb.ids.then]);
    if (cell.has(kb.ids.else_)) {
        if (&cell[kb.ids.then].type() != &kb.type.op.Block) {
            m_ss << ";\n";
            printIndent();
        }
        m_ss << " else ";
        if (&cell[kb.ids.else_].type() != &kb.type.op.Block) {
            m_ss << "\n";
            m_indent++;
            printIndent();
            m_indent--;
        }
        printImpl(cell[kb.ids.else_]);
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
    m_ss << "while )";
    printImpl(cell[kb.ids.condition]);
    m_ss << ") ";
    printImpl(cell[kb.ids.statement]);
}

void CellValuePrinter::printOpConstVar(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    if (cell.has(kb.ids.ast)) {
        CellI& ast = cell[kb.ids.ast];
        if (&ast.type() == &kb.type.ast.Cell) {
            m_ss << ast[kb.ids.value].label();
            return;
        }
        if (&ast.type() == &kb.type.ast.Var) {
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
    if (&cell[kb.ids.ast].type() == &kb.type.ast.Member) {
        m_ss << "m_" << cell[kb.ids.ast][kb.ids.role].label();
        return;
    }
    if (&cell[kb.ids.ast].type() == &kb.type.ast.Self) {
        m_ss << "self";
        return;
    }
    if (&cell[kb.ids.ast].type() == &kb.type.ast.Parameter) {
        m_ss << "p_" << cell[kb.ids.ast][kb.ids.role].label();
        return;
    }
    if (&cell[kb.ids.ast].type() == &kb.type.ast.Var) {
        m_ss << "var_" << cell[kb.ids.ast][kb.ids.role].label();
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
    if (&cell.type() == &kb.type.ast.Member) {
        m_ss << "m_" << cell[kb.ids.ast][kb.ids.role].label();
        return;
    }
    if (&cell.type() == &kb.type.ast.Self) {
        m_ss << "self";
        return;
    }
    if (&cell.type() == &kb.type.ast.Parameter) {
        m_ss << "p_" << cell[kb.ids.ast][kb.ids.role].label();
        return;
    }
    if (&cell.type() == &kb.type.ast.Var) {
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
    m_ss << "var_" << cell[kb.ids.role].label();
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
    if (isA(kb.type.Map)) {
        m_ss << std::format("Map<{}, {}>", cell[kb.ids.subTypes][kb.ids.index][kb.ids.keyType][kb.ids.value].label(), cell[kb.ids.subTypes][kb.ids.index][kb.ids.objectType][kb.ids.value].label());
        return;
    } else if (isA(kb.type.ListItem)) {
        if (&cell == &kb.type.ListItem) {
            m_ss << "ListItem";
            return;
        }
        m_ss << std::format("ListItem<{}>", cell[kb.ids.subTypes][kb.ids.index][kb.ids.objectType][kb.ids.value].label());
        return;
    } else if (isA(kb.type.List)) {
        if (&cell == &kb.type.List) {
            m_ss << "List";
            return;
        }
        m_ss << std::format("List<{}>", cell[kb.ids.subTypes][kb.ids.index][kb.ids.objectType][kb.ids.value].label());
        return;
    }
    m_ss << cell.label();
}

void CellValuePrinter::printImpl(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    auto is          = [this, &cell, &kb](CellI& type) -> bool { return &cell.type() == &type || (cell.type().has(kb.ids.memberOf) && cell.type()[kb.ids.memberOf][kb.ids.index].has(type)); };

    if (is(kb.type.Slot)) {
        CellI& cellType = cell[kb.ids.slotType];
        m_ss << cell[kb.ids.slotRole].label() << ": ";
        if (cellType.type().has(kb.ids.memberOf) && cellType.type()[kb.ids.memberOf][kb.ids.index].has(kb.type.ListItem)) {
            m_ss << "ListItem<" << cellType.type()[kb.ids.slots][kb.ids.index][kb.ids.value][kb.ids.value][kb.ids.slotType].label() << ">";
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
        if (!cell.has(kb.ids.list)) {
            m_ss << "{}";
            return;
        }
        printTypeName(cell.type());
        m_ss << "{";
        visitList(cell[kb.ids.index][kb.ids.type][kb.ids.slots][kb.ids.list], [this, &kb, &cell](CellI& value, int i, bool&) {
            if (i != 0) {
                m_ss << ", ";
            }
            value[kb.ids.slotRole].accept(*this);
            m_ss << ": ";
            cell[kb.ids.index][value[kb.ids.slotRole]][kb.ids.value].accept(*this);
        });
        m_ss << "}";
        return;
    } else if (is(kb.type.Type_)) {
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
    } else if (is(kb.type.ast.Parameter)) {
        printAstParameter(cell);
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

    if (cell.type().has(kb.ids.slots)) {
        visitList(cell.type()[kb.ids.slots][kb.ids.list], [this, &kb](CellI& slot, int i, bool&) {
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