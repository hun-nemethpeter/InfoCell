#include <fmt/core.h>

#include "ValuePrinter.h"
#include "cells/World.h"

namespace infocell {
namespace cells {

void CellValuePrinter::visit(CellI& cell)
{
    printImpl(cell);
}

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
    World& w = cell.w;
    CellI& ast       = cell[w.ids.ast];
    if (&ast.struct_() == &w.std.ast.Call || &ast.struct_() == &w.std.ast.StaticCall) {
        if (&ast[w.ids.cell].struct_() == &w.std.ast.Get && cell.label() != "New { call constructor; }") {
            printImpl(ast[w.ids.cell]);
        }
        if (&ast[w.ids.cell].struct_() == &w.std.ast.Self) {
            m_ss << "self";
        }
        if (&ast[w.ids.cell].struct_() == &w.std.ast.Cell) {
            m_ss << ast[w.ids.cell][w.ids.value].label();
        }
        if (&ast[w.ids.cell].struct_() == &w.std.ast.Member) {
            m_ss << "m_" << ast[w.ids.cell][w.ids.key].label();
        }
        if (&ast[w.ids.cell].struct_() == &w.std.ast.Parameter) {
            m_ss << "p_" << ast[w.ids.cell][w.ids.key].label();
        }
        if (&cell.struct_() == &w.std.op.Call) {
            printOpCall(cell);
            if (0) {
                if (&ast.struct_() == &w.std.ast.Call) {
                    m_ss << ".";
                } else {
                    m_ss << "::";
                }
                m_ss << ast[w.ids.method][w.ids.value].label();
                m_ss << "(";
                if (cell.has(w.ids.parameters)) {
                    int paramNum = 0;
                    Visitor::visitList(cell[w.ids.parameters], [this, &w, &paramNum](CellI& parameter, int, bool& stop) {
                        if (paramNum++ > 0) {
                            m_ss << ", ";
                        }
                        auto& paramKey   = parameter[w.ids.key];
                        auto& paramValue = parameter[w.ids.type];
                        m_ss << paramKey.label();
                        m_ss << ": ";
                        printImpl(paramValue);
                    });
                }
                m_ss << ")";
            }
        }
        return;
    }
    if (&ast.struct_() == &w.std.ast.New) {
        m_ss << "new ";
        CellI* objectTypePtr = nullptr;
        if (ast[w.ids.objectType].isA(w.std.ast.Cell)) {
            objectTypePtr = &ast[w.ids.objectType][w.ids.value];
            m_ss << objectTypePtr->label();
        } else if (ast[w.ids.objectType].isA(w.std.ast.ResolvedType)) {
            objectTypePtr = &ast[w.ids.objectType][w.ids.compiled];
            m_ss << objectTypePtr->label();
        } else {
            throw "Unexpected AST type!";
        }
        CellI& objectType = *objectTypePtr;
        if (ast.has(w.ids.constructor)) {
            CellI& constructorOps = cell[w.ids.ops][w.ids.next][w.ids.cell];
            printOpBlock(constructorOps);
        }
        return;
    }
    m_ss << "{\n";
    m_indent++;
    CellI* activeNodePtr = &cell[w.ids.ops];
    while (activeNodePtr) {
        CellI& activeNode = *activeNodePtr;
        printIndent();
        printImpl(activeNode[w.ids.cell]);
        m_ss << ";\n";
        activeNodePtr = activeNode.has(w.ids.next) ? &activeNode[w.ids.next] : nullptr;
    }
#if 0
    Visitor::visitList(cell[w.ids.ops], [this](CellI& op, int, bool&) {
        printIndent();
        printImpl(op);
        m_ss << ";\n";
    });
#endif
    m_indent--;
    printIndent();
    m_ss << "}";
}

void CellValuePrinter::printOpActivate(CellI& cell)
{
    World& w = cell.w;
    m_ss << "activate (";
    printImpl(cell[w.ids.cell]);
    m_ss << ")";
}

void CellValuePrinter::printOpFunction(CellI& cell)
{
    World& w = cell.w;
    std::stringstream iss;
    std::stringstream oss;
    CellI& subTypesIndex = cell.struct_()[w.ids.subTypes][w.ids.index];
    bool hasReturnValue  = false;
    if (subTypesIndex.has(w.ids.parameters)) {
        CellI& inType       = subTypesIndex[w.ids.parameters][w.ids.value];
        if (inType.has(w.ids.slots)) {
            Visitor::visitList(inType[w.ids.slots][w.ids.list], [this, &iss, &w](CellI& slot, int i, bool& stop) {
                if (i > 0) {
                    iss << ", ";
                }
                if (&slot[w.ids.key] != &w.ids.self) {
                    iss << "p_";
                }
                iss << slot[w.ids.key].label() << ": " << slot[w.ids.type].label();
            });
        }
    }
    if (subTypesIndex.has(w.ids.returnType)) {
        CellI& outType = subTypesIndex[w.ids.returnType][w.ids.value];
        hasReturnValue      = true;
        oss << outType.label();
    }
    const std::string& className = subTypesIndex.has(w.ids.objectType) ? subTypesIndex[w.ids.objectType][w.ids.value].label() : "";
    std::string label            = className;
    if (!className.empty()) {
        label += "::";
    }
    label += subTypesIndex[w.ids.name][w.ids.value].label();
    bool isStatic = cell.has(w.ids.static_);
    std::string staticStr = isStatic ? "static " : "";
    std::string newLabel;

    if (hasReturnValue) {
        newLabel = fmt::format("fn {}{}({}) -> {}\n", staticStr, label, iss.str(), oss.str());
    } else {
        newLabel = fmt::format("fn {}{}({})\n", staticStr, label, iss.str());
    }
    m_ss << newLabel;

    printImpl(cell[w.ids.op]);
}

void CellValuePrinter::printOpCall(CellI& cell)
{
    World& w = cell.w;
    CellI& ast       = cell[w.ids.ast];

    if (&ast[w.ids.cell].struct_() == &w.std.ast.Get && cell.label() != "New { call constructor; }") {
        printImpl(ast[w.ids.cell]);
    }
    if (&ast[w.ids.cell].struct_() == &w.std.ast.Self) {
        m_ss << "self";
    }
    if (&ast[w.ids.cell].struct_() == &w.std.ast.Cell) {
        m_ss << ast[w.ids.cell][w.ids.value].label();
    }
    if (&ast[w.ids.cell].struct_() == &w.std.ast.Member) {
        m_ss << "m_" << ast[w.ids.cell][w.ids.key].label();
    }
    if (&ast[w.ids.cell].struct_() == &w.std.ast.Parameter) {
        m_ss << "p_" << ast[w.ids.cell][w.ids.key].label();
    }
    if (&ast.struct_() == &w.std.ast.Call) {
        m_ss << ".";
    } else {
        m_ss << "::";
    }
    m_ss << ast[w.ids.method][w.ids.value].label();
    m_ss << "(";
    if (cell.has(w.ids.parameters)) {
        int paramNum = 0;
        Visitor::visitList(cell[w.ids.parameters], [this, &w, &paramNum](CellI& parameter, int, bool& stop) {
            if (paramNum++ > 0) {
                m_ss << ", ";
            }
            auto& paramRole  = parameter[w.ids.key];
            auto& paramValue = parameter[w.ids.type];
            m_ss << paramRole.label();
            m_ss << ": ";
            printImpl(paramValue);
        });
    }
    m_ss << ")";
}

void CellValuePrinter::printOpDelete(CellI& cell)
{
    World& w = cell.w;
    m_ss << "delete (";
    printImpl(cell[w.ids.input]);
    m_ss << ")";
}

void CellValuePrinter::printOpSet(CellI& cell)
{
    World& w = cell.w;
    if ((&cell[w.ids.ast][w.ids.cell].struct_() == &w.std.ast.Self) && (&cell[w.ids.ast][w.ids.key].struct_() == &w.std.ast.Cell)) {
        m_ss << "m_";
    } else {
        printImpl(cell[w.ids.cell]);
        m_ss << ".";
    }
    printImpl(cell[w.ids.key]);
    m_ss << " = ";
    printImpl(cell[w.ids.value]);
}

void CellValuePrinter::printOpErase(CellI& cell)
{
    World& w = cell.w;
    printImpl(cell[w.ids.cell]);
    m_ss << ".erase(";
    printImpl(cell[w.ids.key]);
    m_ss << ")";
}

void CellValuePrinter::printOpIf(CellI& cell)
{
    World& w = cell.w;

    auto isBlock = [this, &w, &cell](CellI& ast) -> bool {
        if (&ast.struct_() == &w.std.op.Block) {
            auto& astType = ast[w.ids.ast].struct_();
            if (&astType == &w.std.ast.Call || &astType == &w.std.ast.StaticCall || &astType == &w.std.ast.New) {
                return false;
            }
            return true;
        }
        return false;
    };
    m_ss << "if ";
    printImpl(cell[w.ids.condition]);
    m_ss << " then ";
    if (!isBlock(cell[w.ids.then])) {
        m_ss << "\n";
        m_indent++;
        printIndent();
    }
    printImpl(cell[w.ids.then]);
    if (!isBlock(cell[w.ids.then])) {
        m_indent--;
    }

    if (cell.has(w.ids.else_)) {
        if (!isBlock(cell[w.ids.then])) {
            m_ss << ";\n";
            printIndent();
        }
        m_ss << " else ";
        if (!isBlock(cell[w.ids.else_])) {
            m_ss << "\n";
            m_indent++;
            printIndent();
        }
        printImpl(cell[w.ids.else_]);
        if (!isBlock(cell[w.ids.else_])) {
            m_indent--;
        }
    }
}

void CellValuePrinter::printOpDo(CellI& cell)
{
    World& w = cell.w;
    m_ss << "do ";
    printImpl(cell[w.ids.statement]);
    m_ss << " (";
    printImpl(cell[w.ids.condition]);
    m_ss << ")";
}

void CellValuePrinter::printOpWhile(CellI& cell)
{
    World& w = cell.w;
    m_ss << "while (";
    printImpl(cell[w.ids.condition]);
    m_ss << ") ";
    printImpl(cell[w.ids.statement]);
}

void CellValuePrinter::printOpConstVar(CellI& cell)
{
    World& w = cell.w;
    if (cell.has(w.ids.ast)) {
        CellI& ast = cell[w.ids.ast];
        if (&ast.struct_() == &w.std.ast.Cell) {
            m_ss << ast[w.ids.value].label();
            return;
        }
        if (&ast.struct_() == &w.std.ast.Var) {
            m_ss << "var_" << ast[w.ids.key].label();
            return;
        }
    }
    if (cell.has(w.ids.value)) {
        m_ss << cell[w.ids.value].label();
    } else {
        m_ss << "<empty>";
    }
}

void CellValuePrinter::printOpVar(CellI& cell)
{
    World& w = cell.w;
    cell.label().empty() ? (m_ss << "<empty>") : (m_ss << cell.label());
}

void CellValuePrinter::printOpNew(CellI& cell)
{
    World& w = cell.w;
    m_ss << "new ";
    printImpl(cell[w.ids.objectType]);
}

void CellValuePrinter::printOpSame(CellI& cell)
{
    World& w = cell.w;
    printImpl(cell[w.ids.lhs]);
    m_ss << " is ";
    printImpl(cell[w.ids.rhs]);
}

void CellValuePrinter::printOpNotSame(CellI& cell)
{
    World& w = cell.w;
    printImpl(cell[w.ids.lhs]);
    m_ss << " is not ";
    printImpl(cell[w.ids.rhs]);
}

void CellValuePrinter::printOpEqual(CellI& cell)
{
    World& w = cell.w;
    printImpl(cell[w.ids.lhs]);
    m_ss << " == ";
    printImpl(cell[w.ids.rhs]);
}

void CellValuePrinter::printOpNotEqual(CellI& cell)
{
    World& w = cell.w;
    printImpl(cell[w.ids.lhs]);
    m_ss << " != ";
    printImpl(cell[w.ids.rhs]);
}

void CellValuePrinter::printOpHas(CellI& cell)
{
    World& w = cell.w;
    printImpl(cell[w.ids.cell]);
    m_ss << " has ";
    printImpl(cell[w.ids.key]);
}

void CellValuePrinter::printOpMissing(CellI& cell)
{
    World& w = cell.w;
    printImpl(cell[w.ids.cell]);
    m_ss << " missing ";
    printImpl(cell[w.ids.key]);
}

void CellValuePrinter::printOpGet(CellI& cell)
{
    World& w = cell.w;
    if (&cell[w.ids.ast].struct_() == &w.std.ast.Member) {
        m_ss << "m_" << cell[w.ids.ast][w.ids.key].label();
        return;
    }
    if (&cell[w.ids.ast].struct_() == &w.std.ast.Self) {
        m_ss << "self";
        return;
    }
    if (&cell[w.ids.ast].struct_() == &w.std.ast.Parameter) {
        m_ss << "p_" << cell[w.ids.ast][w.ids.key].label();
        return;
    }
    if (&cell[w.ids.ast].struct_() == &w.std.ast.Var) {
        m_ss << "var_" << cell[w.ids.ast][w.ids.name].label();
        return;
    }
    printImpl(cell[w.ids.cell]);
    m_ss << ".";
    printImpl(cell[w.ids.key]);
}

void CellValuePrinter::printOpAnd(CellI& cell)
{
    World& w = cell.w;
    printImpl(cell[w.ids.lhs]);
    m_ss << " and ";
    printImpl(cell[w.ids.rhs]);
}

void CellValuePrinter::printOpOr(CellI& cell)
{
    World& w = cell.w;
    printImpl(cell[w.ids.lhs]);
    m_ss << " or ";
    printImpl(cell[w.ids.rhs]);
}

void CellValuePrinter::printOpNot(CellI& cell)
{
    World& w = cell.w;
    m_ss << "not(";
    printImpl(cell[w.ids.input]);
    m_ss << ")";
}

void CellValuePrinter::printOpAdd(CellI& cell)
{
    World& w = cell.w;
    printImpl(cell[w.ids.lhs]);
    m_ss << " + ";
    printImpl(cell[w.ids.rhs]);
}

void CellValuePrinter::printOpSubtract(CellI& cell)
{
    World& w = cell.w;
    printImpl(cell[w.ids.lhs]);
    m_ss << " - ";
    printImpl(cell[w.ids.rhs]);
}

void CellValuePrinter::printOpMultiply(CellI& cell)
{
    World& w = cell.w;
    printImpl(cell[w.ids.lhs]);
    m_ss << " * ";
    printImpl(cell[w.ids.rhs]);
}

void CellValuePrinter::printOpDivide(CellI& cell)
{
    World& w = cell.w;
    printImpl(cell[w.ids.lhs]);
    m_ss << " / ";
    printImpl(cell[w.ids.rhs]);
}

void CellValuePrinter::printOpLessThan(CellI& cell)
{
    World& w = cell.w;
    printImpl(cell[w.ids.lhs]);
    m_ss << " < ";
    printImpl(cell[w.ids.rhs]);
}

void CellValuePrinter::printOpLessThanOrEqual(CellI& cell)
{
    World& w = cell.w;
    printImpl(cell[w.ids.lhs]);
    m_ss << " <= ";
    printImpl(cell[w.ids.rhs]);
}

void CellValuePrinter::printOpGreaterThan(CellI& cell)
{
    World& w = cell.w;
    printImpl(cell[w.ids.lhs]);
    m_ss << " > ";
    printImpl(cell[w.ids.rhs]);
}

void CellValuePrinter::printOpGreaterThanOrEqual(CellI& cell)
{
    World& w = cell.w;
    printImpl(cell[w.ids.lhs]);
    m_ss << " >= ";
    printImpl(cell[w.ids.rhs]);
}

void CellValuePrinter::printAstCell(CellI& cell)
{
    World& w = cell.w;
    m_ss << cell[w.ids.value].label();
}

void CellValuePrinter::printAstGet(CellI& cell)
{
    World& w = cell.w;
    if (&cell.struct_() == &w.std.ast.Member) {
        m_ss << "m_" << cell[w.ids.ast][w.ids.key].label();
        return;
    }
    if (&cell.struct_() == &w.std.ast.Self) {
        m_ss << "self";
        return;
    }
    if (&cell.struct_() == &w.std.ast.Parameter) {
        m_ss << "p_" << cell[w.ids.ast][w.ids.key].label();
        return;
    }
    if (&cell.struct_() == &w.std.ast.Var) {
        m_ss << "var_" << cell[w.ids.ast][w.ids.key].label();
        return;
    }
    printImpl(cell[w.ids.cell]);
    m_ss << ".";
    printImpl(cell[w.ids.key]);
}

void CellValuePrinter::printAstParameter(CellI& cell)
{
    World& w = cell.w;
    m_ss << "p_" << cell[w.ids.key].label();
}

void CellValuePrinter::printAstVar(CellI& cell)
{
    World& w = cell.w;
    m_ss << "var_" << cell[w.ids.name].label();
}

void CellValuePrinter::printAstMember(CellI& cell)
{
    World& w = cell.w;
    m_ss << "m_" << cell[w.ids.key].label();
}

void CellValuePrinter::printAstSubtract(CellI& cell)
{
    World& w = cell.w;
    printImpl(cell[w.ids.lhs]);
    m_ss << " - ";
    printImpl(cell[w.ids.rhs]);
}

void CellValuePrinter::printOpReturn(CellI& cell)
{
    World& w = cell.w;
    m_ss << "return";
    if (cell.has(w.ids.result)) {
        m_ss << " ";
        printImpl(cell[w.ids.result][w.ids.value]);
    }
}

void CellValuePrinter::printTypeName(CellI& cell)
{
    World& w = cell.w;
    auto isA          = [this, &cell, &w](CellI& type) -> bool { return &cell == &type || (cell.has(w.ids.memberOf) && cell[w.ids.memberOf][w.ids.index].has(type)); };
    if (isA(w.std.Map)) {
        m_ss << fmt::format("Map<{}, {}>", cell[w.ids.subTypes][w.ids.index][w.ids.keyType][w.ids.value].label(), cell[w.ids.subTypes][w.ids.index][w.ids.valueType][w.ids.value].label());
        return;
    } else if (isA(w.std.ListItem)) {
        if (&cell == &w.std.ListItem) {
            m_ss << "ListItem";
            return;
        }
        m_ss << fmt::format("ListItem<{}>", cell[w.ids.subTypes][w.ids.index][w.ids.valueType][w.ids.value].label());
        return;
    } else if (isA(w.std.List)) {
        if (&cell == &w.std.List) {
            m_ss << "List";
            return;
        }
        m_ss << fmt::format("List<{}>", cell[w.ids.subTypes][w.ids.index][w.ids.valueType][w.ids.value].label());
        return;
    }
    m_ss << cell.label();
}

void CellValuePrinter::printImpl(CellI& cell)
{
    World& w = cell.w;
    auto is          = [this, &cell, &w](CellI& type) -> bool { return &cell.struct_() == &type || (cell.struct_().has(w.ids.memberOf) && cell.struct_()[w.ids.memberOf][w.ids.index].has(type)); };

    if (is(w.std.Slot)) {
        CellI& cellType = cell[w.ids.type];
        m_ss << cell[w.ids.key].label() << ": ";
        if (cellType.struct_().has(w.ids.memberOf) && cellType.struct_()[w.ids.memberOf][w.ids.index].has(w.std.ListItem)) {
            m_ss << "ListItem<" << cellType.struct_()[w.ids.slots][w.ids.index][w.ids.value][w.ids.value][w.ids.type].label() << ">";
        } else {
            m_ss << cellType.label();
        }
        return;
    } else if (is(w.std.List)) {
        if (&cell.struct_()[w.ids.subTypes][w.ids.index][w.ids.valueType][w.ids.value] == &w.std.Char) {
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
    } else if (is(w.std.Map)) {
        if (!cell.has(w.ids.list)) {
            m_ss << "{}";
            return;
        }
        printTypeName(cell.struct_());
        m_ss << "{";
        visitList(cell[w.ids.index][w.ids.struct_][w.ids.slots][w.ids.list], [this, &w, &cell](CellI& value, int i, bool&) {
            if (i != 0) {
                m_ss << ", ";
            }
            value[w.ids.key].accept(*this);
            m_ss << ": {";
            cell[w.ids.index][value[w.ids.key]][w.ids.value].accept(*this);
            m_ss << "}";
        });
        m_ss << "}";
        return;
    } else if (is(w.std.Struct)) {
        CellI& type = cell;
        m_ss << "Type ";
        printTypeName(cell);
        if (type.has(w.ids.memberOf)) {
            visitList(type[w.ids.memberOf][w.ids.list], [this, &w](CellI& member, int i, bool&) {
                if (i != 0) {
                    m_ss << ", ";
                } else {
                    m_ss << " : ";
                }
                m_ss << member.label();
            });
        }
        m_ss << " { ";
        if (type.has(w.ids.slots)) {
            visitList(type[w.ids.slots][w.ids.list], [this, &w](CellI& slot, int i, bool&) {
                if (i != 0) {
                    m_ss << ", ";
                }
                m_ss << slot[w.ids.key].label() << ": ";
                printTypeName(slot[w.ids.type]);
            });
        }
        m_ss << " }";
        return;
    } else if (&cell == &w.ids.red || &cell == &w.ids.green || &cell == &w.ids.blue) {
        m_ss << cell.label();
        return;
    } else if (is(w.std.op.Block)) {
        printOpBlock(cell);
        return;
    } else if (is(w.std.op.Activate)) {
        printOpActivate(cell);
        return;
    } else if (is(w.std.op.Function)) {
        printOpFunction(cell);
        return;
    } else if (is(w.std.op.Call)) {
        printOpCall(cell);
        return;
    } else if (is(w.std.op.Delete)) {
        printOpDelete(cell);
        return;
    } else if (is(w.std.op.Set)) {
        printOpSet(cell);
        return;
    } else if (is(w.std.op.Erase)) {
        printOpErase(cell);
        return;
    } else if (is(w.std.op.If)) {
        printOpIf(cell);
        return;
    } else if (is(w.std.op.Do)) {
        printOpDo(cell);
        return;
    } else if (is(w.std.op.While)) {
        printOpWhile(cell);
        return;
    } else if (is(w.std.op.ConstVar)) {
        printOpConstVar(cell);
        return;
    } else if (is(w.std.op.Var)) {
        printOpVar(cell);
        return;
    } else if (is(w.std.op.New)) {
        printOpNew(cell);
        return;
    } else if (is(w.std.op.Same)) {
        printOpSame(cell);
        return;
    } else if (is(w.std.op.NotSame)) {
        printOpNotSame(cell);
        return;
    } else if (is(w.std.op.Equal)) {
        printOpEqual(cell);
        return;
    } else if (is(w.std.op.NotEqual)) {
        printOpNotEqual(cell);
        return;
    } else if (is(w.std.op.Has)) {
        printOpHas(cell);
        return;
    } else if (is(w.std.op.Missing)) {
        printOpMissing(cell);
        return;
    } else if (is(w.std.op.Get)) {
        printOpGet(cell);
        return;
    } else if (is(w.std.op.And)) {
        printOpAnd(cell);
        return;
    } else if (is(w.std.op.Or)) {
        printOpOr(cell);
        return;
    } else if (is(w.std.op.Not)) {
        printOpNot(cell);
        return;
    } else if (is(w.std.op.Add)) {
        printOpAdd(cell);
        return;
    } else if (is(w.std.op.Subtract)) {
        printOpSubtract(cell);
        return;
    } else if (is(w.std.op.Multiply)) {
        printOpMultiply(cell);
        return;
    } else if (is(w.std.op.Divide)) {
        printOpDivide(cell);
        return;
    } else if (is(w.std.op.LessThan)) {
        printOpLessThan(cell);
        return;
    } else if (is(w.std.op.LessThanOrEqual)) {
        printOpLessThanOrEqual(cell);
        return;
    } else if (is(w.std.op.GreaterThan)) {
        printOpGreaterThan(cell);
        return;
    } else if (is(w.std.op.GreaterThanOrEqual)) {
        printOpGreaterThanOrEqual(cell);
        return;
    } else if (is(w.std.op.Return)) {
        printOpReturn(cell);
        return;
    } else if (is(w.std.ast.Cell)) {
        printAstCell(cell);
        return;
    } else if (is(w.std.ast.Get)) {
        printAstGet(cell);
        return;
    } else if (is(w.std.ast.Parameter)) {
        printAstParameter(cell);
        return;
    } else if (is(w.std.ast.Var)) {
        printAstVar(cell);
        return;
    } else if (is(w.std.ast.Member)) {
        printAstMember(cell);
        return;
    } else if (is(w.std.ast.Subtract)) {
        printAstSubtract(cell);
        return;
    }

    if (!cell.label().empty()) {
        m_ss << cell.label() << ": ";
    }
    m_ss << cell.struct_().label() << " { ";

    if (cell.struct_().has(w.ids.slots)) {
        visitList(cell.struct_()[w.ids.slots][w.ids.list], [this, &w](CellI& slot, int i, bool&) {
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

#if 0 // TODO
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
#endif

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
} // namespace infocell