#include <format>

#include "ValuePrinter.h"
#include "Brain.h"

namespace synth {
namespace cells {

void CellValuePrinter::visit(Type& cell)
{
    printImpl(cell);
}

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
    brain::Brain& kb = cell.kb;
    m_ss << cell[kb.coding.slotRole].label() << ": " << cell[kb.coding.slotType].label();
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

void CellValuePrinter::visit(Object& object)
{
    printImpl(object);
}

void CellValuePrinter::printOpBlock(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    CellI& ast       = cell[kb.coding.ast];
    if (&ast.type() == &kb.type.ast.Call) {
        if (&ast[kb.coding.cell].type() == &kb.type.ast.Get) {
            m_ss << "(*var_" << ast[kb.coding.cell][kb.coding.cell][kb.coding.cell][kb.coding.role].label() << ")";
        }
        if (&ast[kb.coding.cell].type() == &kb.type.ast.Input) {
            m_ss << "in_" << ast[kb.coding.cell][kb.coding.role].label();
        }
        if (&ast[kb.coding.cell].type() == &kb.type.ast.Cell) {
            m_ss << ast[kb.coding.cell][kb.coding.value].label();
        }
        if (&ast[kb.coding.cell].type() == &kb.type.ast.Member) {
            m_ss << "self." << ast[kb.coding.cell][kb.coding.role].label();
        }
        if (ast.has(kb.coding.method)) {
            m_ss << ".";
            m_ss << ast[kb.coding.method][kb.coding.value].label();
            m_ss << "(";
            if (ast.has(kb.coding.parameters)) {
                visitList(ast[kb.coding.parameters], [this, &kb](CellI& slot, int i) {
                    if (i != 0) {
                        m_ss << ", ";
                    }
                    m_ss << slot[kb.coding.slotRole][kb.coding.value].label();
                    m_ss << " = ";
                    printImpl(slot[kb.coding.slotType]);
                });
            }
            m_ss << ")";
        }
        return;
    }
    if (&ast.type() == &kb.type.ast.StaticCall) {
        if (&ast[kb.coding.cell].type() == &kb.type.ast.Cell) {
            m_ss << ast[kb.coding.cell][kb.coding.value].label();
        }
        if (&ast[kb.coding.cell].type() == &kb.type.ast.Member) {
            m_ss << "self." << ast[kb.coding.cell][kb.coding.role][kb.coding.value].label();
        }
        if (ast.has(kb.coding.method)) {
            m_ss << "::";
            m_ss << ast[kb.coding.method][kb.coding.value].label();
            m_ss << "(";
            if (ast.has(kb.coding.parameters)) {
                visitList(ast[kb.coding.parameters], [this, &kb](CellI& slot, int i) {
                    if (i != 0) {
                        m_ss << ", ";
                    }
                    m_ss << slot[kb.coding.slotRole][kb.coding.value].label();
                    m_ss << " = ";
                    printImpl(slot[kb.coding.slotType]);
                });
            }
            m_ss << ")";
        }
        return;
    }
    if (&ast.type() == &kb.type.ast.New) {
        m_ss << "new ";
        if (&ast[kb.coding.objectType].type() == &kb.type.ast.Cell) {
            m_ss << ast[kb.coding.objectType][kb.coding.value].label();
        }
        if (&ast[kb.coding.objectType].type() == &kb.type.ast.Member) {
            m_ss << "self." << ast[kb.coding.objectType][kb.coding.role].label();
        }
        if (ast.has(kb.coding.constructor)) {
            m_ss << ".";
            m_ss << ast[kb.coding.constructor][kb.coding.value].label();
            m_ss << "(";
            if (ast.has(kb.coding.parameters)) {
                visitList(ast[kb.coding.parameters], [this, &kb](CellI& slot, int i) {
                    if (i != 0) {
                        m_ss << ", ";
                    }
                    m_ss << slot[kb.coding.slotRole][kb.coding.value].label();
                    m_ss << " = ";
                    printImpl(slot[kb.coding.slotType]);
                });

            }
            m_ss << ")";
        }
        return;
    }
    m_ss << "{\n";
    m_indent++;
    Visitor::visitList(cell[kb.coding.ops], [this](CellI& op, int) {
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
    printImpl(cell[kb.coding.value]);
    m_ss << ")";
}

void CellValuePrinter::printOpFunction(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    m_ss << cell.label() << " ";
    printImpl(cell[kb.coding.op]);
}

void CellValuePrinter::printOpDelete(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    m_ss << "delete (";
    printImpl(cell[kb.coding.input]);
    m_ss << ")";
}

void CellValuePrinter::printOpSet(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.coding.cell]);
    m_ss << ".";
    printImpl(cell[kb.coding.role]);
    m_ss << " = ";
    printImpl(cell[kb.coding.value]);
}

void CellValuePrinter::printOpIf(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    m_ss << "if ";
    printImpl(cell[kb.coding.condition]);
    m_ss << " then ";
    if (&cell[kb.coding.then].type() != &kb.type.op.Block) {
        m_ss << "\n";
        m_indent++;
        printIndent();
        m_indent--;
    }
    printImpl(cell[kb.coding.then]);
    if (&cell[kb.coding.then].type() != &kb.type.op.Block) {
        m_ss << ";\n";
        printIndent();
    }
    if (cell.has(kb.coding.else_)) {
        m_ss << " else ";
        if (&cell[kb.coding.else_].type() != &kb.type.op.Block) {
            m_ss << "\n";
            m_indent++;
            printIndent();
            m_indent--;
        }
        printImpl(cell[kb.coding.else_]);
    }
}

void CellValuePrinter::printOpDo(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    m_ss << "do ";
    printImpl(cell[kb.coding.statement]);
    m_ss << " (";
    printImpl(cell[kb.coding.condition]);
    m_ss << ")";
}

void CellValuePrinter::printOpWhile(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    m_ss << "while )";
    printImpl(cell[kb.coding.condition]);
    m_ss << ") ";
    printImpl(cell[kb.coding.statement]);
}

void CellValuePrinter::printOpConstVar(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    if (cell.has(kb.coding.ast)) {
        CellI& ast = cell[kb.coding.ast];
        if (&ast.type() == &kb.type.ast.Cell) {
            m_ss << ast[kb.coding.value].label();
            return;
        }
        if (&ast.type() == &kb.type.ast.Var) {
            m_ss << "var_" << ast[kb.coding.role].label();
            return;
        }
    }
    if (cell.has(kb.coding.value)) {
        m_ss << cell[kb.coding.value].label();
    } else {
        m_ss << "<empty>";
    }
}

void CellValuePrinter::printOpVar(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    if (cell.has(kb.coding.value)) {
        CellI* value = &cell[kb.coding.value];
        m_ss << value << std::endl;
        printImpl(*value);
    } else {
        cell.label().empty() ? (m_ss << "empty") : (m_ss << cell.label());
    }
}

void CellValuePrinter::printOpNew(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    m_ss << "new ";
    printImpl(cell[kb.coding.objectType]);
}

void CellValuePrinter::printOpSame(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.coding.lhs]);
    m_ss << " same ";
    printImpl(cell[kb.coding.rhs]);
}

void CellValuePrinter::printOpNotSame(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.coding.lhs]);
    m_ss << " not same ";
    printImpl(cell[kb.coding.rhs]);
}

void CellValuePrinter::printOpEqual(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.coding.lhs]);
    m_ss << " == ";
    printImpl(cell[kb.coding.rhs]);
}

void CellValuePrinter::printOpNotEqual(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.coding.lhs]);
    m_ss << " != ";
    printImpl(cell[kb.coding.rhs]);
}

void CellValuePrinter::printOpHas(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.coding.cell]);
    m_ss << " has ";
    printImpl(cell[kb.coding.role]);
}

void CellValuePrinter::printOpGet(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.coding.cell]);
    m_ss << "->";
    printImpl(cell[kb.coding.role]);
}

void CellValuePrinter::printOpAnd(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.coding.lhs]);
    m_ss << " and ";
    printImpl(cell[kb.coding.rhs]);
}

void CellValuePrinter::printOpOr(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.coding.lhs]);
    m_ss << " or ";
    printImpl(cell[kb.coding.rhs]);
}

void CellValuePrinter::printOpNot(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    m_ss << "not ";
    printImpl(cell[kb.coding.input]);
}

void CellValuePrinter::printOpAdd(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.coding.lhs]);
    m_ss << " + ";
    printImpl(cell[kb.coding.rhs]);
}

void CellValuePrinter::printOpSubtract(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.coding.lhs]);
    m_ss << " - ";
    printImpl(cell[kb.coding.rhs]);
}

void CellValuePrinter::printOpMultiply(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.coding.lhs]);
    m_ss << " * ";
    printImpl(cell[kb.coding.rhs]);
}

void CellValuePrinter::printOpDivide(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.coding.lhs]);
    m_ss << " / ";
    printImpl(cell[kb.coding.rhs]);
}

void CellValuePrinter::printOpLessThan(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.coding.lhs]);
    m_ss << " < ";
    printImpl(cell[kb.coding.rhs]);
}

void CellValuePrinter::printOpGreaterThan(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.coding.lhs]);
    m_ss << " > ";
    printImpl(cell[kb.coding.rhs]);
}

void CellValuePrinter::printAstCell(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    m_ss << cell[kb.coding.value].label();
}

void CellValuePrinter::printAstGet(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    printImpl(cell[kb.coding.cell]);
    m_ss << ".";
    printImpl(cell[kb.coding.role]);
}

void CellValuePrinter::printAstInput(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    m_ss << "in_" << cell[kb.coding.role].label();
}

void CellValuePrinter::printAstOutput(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    m_ss << "out_" << cell[kb.coding.role].label();
}

void CellValuePrinter::printAstVar(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    m_ss << "var_" << cell[kb.coding.role].label();
}

void CellValuePrinter::printAstMember(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    m_ss << "m_" << cell[kb.coding.role].label();
}


void CellValuePrinter::printImpl(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    auto is          = [this, &cell, &kb](CellI& type) -> bool { return cell.type()[kb.coding.memberOf][kb.coding.index].has(type) || &cell.type() == &type; };

    if (is(kb.type.Slot)) {
        CellI& cellType = cell[kb.coding.slotType];
        m_ss << cell[kb.coding.slotRole].label() << ": ";
        if (cellType.type()[kb.coding.memberOf][kb.coding.index].has(kb.type.ListItem)) {
            m_ss << "ListItem<" << cellType.type()[kb.coding.slots][kb.coding.index][kb.coding.value][kb.coding.slotType].label() << ">";
        } else {
            m_ss << cellType.label();
        }
        return;
    } else if (is(kb.type.Type_)) {
        CellI& type = cell;
        m_ss << "Type " << type.label();
        visitList(type[kb.coding.memberOf][kb.coding.list], [this, &kb](CellI& member, int i) {
            if (i != 0) {
                m_ss << ", ";
            } else {
                m_ss << " : ";
            }
            m_ss << member.label();
        });
        m_ss << " { ";
        visitList(type[kb.coding.slots][kb.coding.list], [this, &kb](CellI& slot, int i) {
            if (i != 0) {
                m_ss << ", ";
            }
            m_ss << slot[kb.coding.slotRole].label() << ": " << slot[kb.coding.slotType].label();
        });
        m_ss << " }";
        return;
    } else if (is(kb.type.List)) {
        m_ss << "[";
        visitList(cell, [this](CellI& value, int i) {
            if (i != 0) {
                m_ss << ",";
            }
            m_ss << " ";
            value.accept(*this);
        });
        m_ss << " ]";
        return;
    } else if (is(kb.type.Map)) {
        if (!cell.has(kb.coding.list)) {
            m_ss << "{}";
            return;
        }
        m_ss << "{";
        visitList(cell[kb.coding.list], [this](CellI& value, int i) {
            if (i != 0) {
                m_ss << ",";
            }
            m_ss << " ";
            value.accept(*this);
        });
        m_ss << " }";
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
    } else if (is(kb.type.op.GreaterThan)) {
        printOpGreaterThan(cell);
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
    }

    if (!cell.label().empty()) {
        m_ss << cell.label() << ": ";
    }
    m_ss << cell.type().label() << " { ";

    visitList(cell.type()[kb.coding.slots][kb.coding.list], [this, &kb](CellI& slot, int i) {
        if (i != 0) {
            m_ss << ", ";
        }
        m_ss << ".";
        slot.accept(*this);
    });

    m_ss << " }";
}

void CellValuePrinter::visit(List::Item& listItemCell)
{
    brain::Brain& kb = listItemCell.kb;
    m_ss << "[ ";
    if (!listItemCell[kb.coding.value].label().empty()) {
        m_ss << listItemCell[kb.coding.value].label() << " ";
    }
    m_ss << "]";

}

void CellValuePrinter::visit(List& list)
{
    m_ss << "[";
    visitList(list, [this](CellI& value, int i) {
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
    m_ss << "(Number) " << cell.value();
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

void CellValuePrinter::visit(op::Function& cell)
{
    printImpl(cell);
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