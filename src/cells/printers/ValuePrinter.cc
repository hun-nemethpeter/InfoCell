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

void CellValuePrinter::visit(List::Node& cell)
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
    CellI& ast       = cell[w.id.ast];
    if (&ast.__type__() == &w.std.ast.Call || &ast.__type__() == &w.std.ast.StaticCall) {
        if (&ast[w.id.cell].__type__() == &w.std.ast.Get && cell.label() != "New { call constructor; }") {
            printImpl(ast[w.id.cell]);
        }
        if (&ast[w.id.cell].__type__() == &w.std.ast.Self) {
            m_ss << "self";
        }
        if (&ast[w.id.cell].__type__() == &w.std.ast.Cell) {
            m_ss << ast[w.id.cell][w.id.value].label();
        }
        if (&ast[w.id.cell].__type__() == &w.std.ast.Member) {
            m_ss << "m_" << ast[w.id.cell][w.id.key].label();
        }
        if (&ast[w.id.cell].__type__() == &w.std.ast.Parameter) {
            m_ss << "p_" << ast[w.id.cell][w.id.key].label();
        }
        if (&cell.__type__() == &w.std.op.Call) {
            printOpCall(cell);
            if (0) {
                if (&ast.__type__() == &w.std.ast.Call) {
                    m_ss << ".";
                } else {
                    m_ss << "::";
                }
                m_ss << ast[w.id.method][w.id.value].label();
                m_ss << "(";
                if (cell.has(w.id.parameters)) {
                    int paramNum = 0;
                    Visitor::visitList(cell[w.id.parameters], [this, &w, &paramNum](CellI& parameter, int, bool& stop) {
                        if (paramNum++ > 0) {
                            m_ss << ", ";
                        }
                        auto& paramKey   = parameter[w.id.key];
                        auto& paramValue = parameter[w.id.type];
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
    if (&ast.__type__() == &w.std.ast.New) {
        m_ss << "new ";
        CellI* objectTypePtr = nullptr;
        if (ast[w.id.objectType].isA(w.std.ast.Cell)) {
            objectTypePtr = &ast[w.id.objectType][w.id.value];
            m_ss << objectTypePtr->label();
        } else if (ast[w.id.objectType].isA(w.std.ast.ResolvedType)) {
            objectTypePtr = &ast[w.id.objectType][w.id.compiled];
            m_ss << objectTypePtr->label();
        } else {
            throw "Unexpected AST type!";
        }
        CellI& objectType = *objectTypePtr;
        if (ast.has(w.id.constructor)) {
            CellI& constructorOps = cell[w.id.ops][w.id.next][w.id.cell];
            printOpBlock(constructorOps);
        }
        return;
    }
    m_ss << "{\n";
    m_indent++;
    CellI* activeNodePtr = &cell[w.id.ops];
    while (activeNodePtr) {
        CellI& activeNode = *activeNodePtr;
        printIndent();
        printImpl(activeNode[w.id.cell]);
        m_ss << ";\n";
        activeNodePtr = activeNode.has(w.id.next) ? &activeNode[w.id.next] : nullptr;
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
    printImpl(cell[w.id.cell]);
    m_ss << ")";
}

void CellValuePrinter::printOpFunction(CellI& cell)
{
    World& w = cell.w;
    std::stringstream iss;
    std::stringstream oss;

    if (cell.has(w.id.parameters)) {
        CellI& parametersList = cell[w.id.parameters][w.id.list];
        Visitor::visitList(parametersList, [this, &iss, &w](CellI& slot, int i, bool& stop) {
            if (i > 0) {
                iss << ", ";
            }
            if (&slot[w.id.key] != &w.id.self) {
                iss << "p_";
            }
            iss << slot[w.id.key].label() << ": " << slot[w.id.type].label();
        });
    }
    bool hasReturnValue = false;
    if (cell.has(w.id.returnType)) {
        CellI& outType = cell[w.id.returnType];
        hasReturnValue = true;
        oss << outType.label();
    }
    const std::string& className = cell.has(w.id.objectType) ? cell[w.id.objectType].label() : "";
    std::string label            = className;
    if (!className.empty()) {
        label += "::";
    }
    label += cell[w.id.name].label();

    bool isStatic = cell.has(w.id.static_);
    std::string staticStr = isStatic ? "static " : "";
    std::string newLabel;

    if (hasReturnValue) {
        newLabel = fmt::format("fn {}{}({}) -> {}\n", staticStr, label, iss.str(), oss.str());
    } else {
        newLabel = fmt::format("fn {}{}({})\n", staticStr, label, iss.str());
    }
    m_ss << newLabel;

    printImpl(cell[w.id.op]);
}

void CellValuePrinter::printOpCall(CellI& cell)
{
    World& w = cell.w;
    CellI& ast       = cell[w.id.ast];

    if (&ast[w.id.cell].__type__() == &w.std.ast.Get && cell.label() != "New { call constructor; }") {
        printImpl(ast[w.id.cell]);
    }
    if (&ast[w.id.cell].__type__() == &w.std.ast.Self) {
        m_ss << "self";
    }
    if (&ast[w.id.cell].__type__() == &w.std.ast.Cell) {
        m_ss << ast[w.id.cell][w.id.value].label();
    }
    if (&ast[w.id.cell].__type__() == &w.std.ast.Member) {
        m_ss << "m_" << ast[w.id.cell][w.id.key].label();
    }
    if (&ast[w.id.cell].__type__() == &w.std.ast.Parameter) {
        m_ss << "p_" << ast[w.id.cell][w.id.key].label();
    }
    if (&ast.__type__() == &w.std.ast.Call) {
        m_ss << ".";
    } else {
        m_ss << "::";
    }
    m_ss << ast[w.id.method][w.id.value].label();
    m_ss << "(";
    if (cell.has(w.id.parameters)) {
        int paramNum = 0;
        Visitor::visitList(cell[w.id.parameters], [this, &w, &paramNum](CellI& parameter, int, bool& stop) {
            if (paramNum++ > 0) {
                m_ss << ", ";
            }
            auto& paramRole  = parameter[w.id.key];
            auto& paramValue = parameter[w.id.type];
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
    printImpl(cell[w.id.input]);
    m_ss << ")";
}

void CellValuePrinter::printOpSet(CellI& cell)
{
    World& w = cell.w;
    if ((&cell[w.id.ast][w.id.cell].__type__() == &w.std.ast.Self) && (&cell[w.id.ast][w.id.key].__type__() == &w.std.ast.Cell)) {
        m_ss << "m_";
    } else {
        printImpl(cell[w.id.cell]);
        m_ss << ".";
    }
    printImpl(cell[w.id.key]);
    m_ss << " = ";
    printImpl(cell[w.id.value]);
}

void CellValuePrinter::printOpErase(CellI& cell)
{
    World& w = cell.w;
    printImpl(cell[w.id.cell]);
    m_ss << ".erase(";
    printImpl(cell[w.id.key]);
    m_ss << ")";
}

void CellValuePrinter::printOpIf(CellI& cell)
{
    World& w = cell.w;

    auto isBlock = [this, &w, &cell](CellI& ast) -> bool {
        if (&ast.__type__() == &w.std.op.Block) {
            auto& astType = ast[w.id.ast].__type__();
            if (&astType == &w.std.ast.Call || &astType == &w.std.ast.StaticCall || &astType == &w.std.ast.New) {
                return false;
            }
            return true;
        }
        return false;
    };
    m_ss << "if ";
    printImpl(cell[w.id.condition]);
    m_ss << " then ";
    if (!isBlock(cell[w.id.then])) {
        m_ss << "\n";
        m_indent++;
        printIndent();
    }
    printImpl(cell[w.id.then]);
    if (!isBlock(cell[w.id.then])) {
        m_indent--;
    }

    if (cell.has(w.id.else_)) {
        if (!isBlock(cell[w.id.then])) {
            m_ss << ";\n";
            printIndent();
        }
        m_ss << " else ";
        if (!isBlock(cell[w.id.else_])) {
            m_ss << "\n";
            m_indent++;
            printIndent();
        }
        printImpl(cell[w.id.else_]);
        if (!isBlock(cell[w.id.else_])) {
            m_indent--;
        }
    }
}

void CellValuePrinter::printOpDo(CellI& cell)
{
    World& w = cell.w;
    m_ss << "do ";
    printImpl(cell[w.id.statement]);
    m_ss << " (";
    printImpl(cell[w.id.condition]);
    m_ss << ")";
}

void CellValuePrinter::printOpWhile(CellI& cell)
{
    World& w = cell.w;
    m_ss << "while (";
    printImpl(cell[w.id.condition]);
    m_ss << ") ";
    printImpl(cell[w.id.statement]);
}

void CellValuePrinter::printOpConstVar(CellI& cell)
{
    World& w = cell.w;
    if (cell.has(w.id.ast)) {
        CellI& ast = cell[w.id.ast];
        if (&ast.__type__() == &w.std.ast.Cell) {
            m_ss << ast[w.id.value].label();
            return;
        }
        if (&ast.__type__() == &w.std.ast.Var) {
            m_ss << "var_" << ast[w.id.key].label();
            return;
        }
    }
    if (cell.has(w.id.value)) {
        m_ss << cell[w.id.value].label();
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
    printImpl(cell[w.id.objectType]);
}

void CellValuePrinter::printOpSame(CellI& cell)
{
    World& w = cell.w;
    printImpl(cell[w.id.lhs]);
    m_ss << " is ";
    printImpl(cell[w.id.rhs]);
}

void CellValuePrinter::printOpNotSame(CellI& cell)
{
    World& w = cell.w;
    printImpl(cell[w.id.lhs]);
    m_ss << " is not ";
    printImpl(cell[w.id.rhs]);
}

void CellValuePrinter::printOpEqual(CellI& cell)
{
    World& w = cell.w;
    printImpl(cell[w.id.lhs]);
    m_ss << " == ";
    printImpl(cell[w.id.rhs]);
}

void CellValuePrinter::printOpNotEqual(CellI& cell)
{
    World& w = cell.w;
    printImpl(cell[w.id.lhs]);
    m_ss << " != ";
    printImpl(cell[w.id.rhs]);
}

void CellValuePrinter::printOpHas(CellI& cell)
{
    World& w = cell.w;
    printImpl(cell[w.id.cell]);
    m_ss << " has ";
    printImpl(cell[w.id.key]);
}

void CellValuePrinter::printOpMissing(CellI& cell)
{
    World& w = cell.w;
    printImpl(cell[w.id.cell]);
    m_ss << " missing ";
    printImpl(cell[w.id.key]);
}

void CellValuePrinter::printOpGet(CellI& cell)
{
    World& w = cell.w;
    if (&cell[w.id.ast].__type__() == &w.std.ast.Member) {
        m_ss << "m_" << cell[w.id.ast][w.id.key].label();
        return;
    }
    if (&cell[w.id.ast].__type__() == &w.std.ast.Self) {
        m_ss << "self";
        return;
    }
    if (&cell[w.id.ast].__type__() == &w.std.ast.Parameter) {
        m_ss << "p_" << cell[w.id.ast][w.id.key].label();
        return;
    }
    if (&cell[w.id.ast].__type__() == &w.std.ast.Var) {
        m_ss << "var_" << cell[w.id.ast][w.id.name].label();
        return;
    }
    printImpl(cell[w.id.cell]);
    m_ss << ".";
    printImpl(cell[w.id.key]);
}

void CellValuePrinter::printOpAnd(CellI& cell)
{
    World& w = cell.w;
    printImpl(cell[w.id.lhs]);
    m_ss << " and ";
    printImpl(cell[w.id.rhs]);
}

void CellValuePrinter::printOpOr(CellI& cell)
{
    World& w = cell.w;
    printImpl(cell[w.id.lhs]);
    m_ss << " or ";
    printImpl(cell[w.id.rhs]);
}

void CellValuePrinter::printOpNot(CellI& cell)
{
    World& w = cell.w;
    m_ss << "not(";
    printImpl(cell[w.id.input]);
    m_ss << ")";
}

void CellValuePrinter::printOpAdd(CellI& cell)
{
    World& w = cell.w;
    printImpl(cell[w.id.lhs]);
    m_ss << " + ";
    printImpl(cell[w.id.rhs]);
}

void CellValuePrinter::printOpSubtract(CellI& cell)
{
    World& w = cell.w;
    printImpl(cell[w.id.lhs]);
    m_ss << " - ";
    printImpl(cell[w.id.rhs]);
}

void CellValuePrinter::printOpMultiply(CellI& cell)
{
    World& w = cell.w;
    printImpl(cell[w.id.lhs]);
    m_ss << " * ";
    printImpl(cell[w.id.rhs]);
}

void CellValuePrinter::printOpDivide(CellI& cell)
{
    World& w = cell.w;
    printImpl(cell[w.id.lhs]);
    m_ss << " / ";
    printImpl(cell[w.id.rhs]);
}

void CellValuePrinter::printOpLessThan(CellI& cell)
{
    World& w = cell.w;
    printImpl(cell[w.id.lhs]);
    m_ss << " < ";
    printImpl(cell[w.id.rhs]);
}

void CellValuePrinter::printOpLessThanOrEqual(CellI& cell)
{
    World& w = cell.w;
    printImpl(cell[w.id.lhs]);
    m_ss << " <= ";
    printImpl(cell[w.id.rhs]);
}

void CellValuePrinter::printOpGreaterThan(CellI& cell)
{
    World& w = cell.w;
    printImpl(cell[w.id.lhs]);
    m_ss << " > ";
    printImpl(cell[w.id.rhs]);
}

void CellValuePrinter::printOpGreaterThanOrEqual(CellI& cell)
{
    World& w = cell.w;
    printImpl(cell[w.id.lhs]);
    m_ss << " >= ";
    printImpl(cell[w.id.rhs]);
}

void CellValuePrinter::printAstCell(CellI& cell)
{
    World& w = cell.w;
    m_ss << cell[w.id.value].label();
}

void CellValuePrinter::printAstGet(CellI& cell)
{
    World& w = cell.w;
    if (&cell.__type__() == &w.std.ast.Member) {
        m_ss << "m_" << cell[w.id.ast][w.id.key].label();
        return;
    }
    if (&cell.__type__() == &w.std.ast.Self) {
        m_ss << "self";
        return;
    }
    if (&cell.__type__() == &w.std.ast.Parameter) {
        m_ss << "p_" << cell[w.id.ast][w.id.key].label();
        return;
    }
    if (&cell.__type__() == &w.std.ast.Var) {
        m_ss << "var_" << cell[w.id.ast][w.id.key].label();
        return;
    }
    printImpl(cell[w.id.cell]);
    m_ss << ".";
    printImpl(cell[w.id.key]);
}

void CellValuePrinter::printAstParameter(CellI& cell)
{
    World& w = cell.w;
    m_ss << "p_" << cell[w.id.key].label();
}

void CellValuePrinter::printAstVar(CellI& cell)
{
    World& w = cell.w;
    m_ss << "var_" << cell[w.id.name].label();
}

void CellValuePrinter::printAstMember(CellI& cell)
{
    World& w = cell.w;
    m_ss << "m_" << cell[w.id.key].label();
}

void CellValuePrinter::printAstSubtract(CellI& cell)
{
    World& w = cell.w;
    printImpl(cell[w.id.lhs]);
    m_ss << " - ";
    printImpl(cell[w.id.rhs]);
}

void CellValuePrinter::printOpReturn(CellI& cell)
{
    World& w = cell.w;
    m_ss << "return";
    if (cell.has(w.id.result)) {
        m_ss << " ";
        printImpl(cell[w.id.result][w.id.value]);
    }
}

void CellValuePrinter::printTypeName(CellI& cell)
{
    World& w = cell.w;
    auto isA          = [this, &cell, &w](CellI& type) -> bool { return &cell == &type || (cell.has(w.id.memberOf) && cell[w.id.memberOf][w.id.index].has(type)); };
    if (isA(w.std.Map)) {
        m_ss << fmt::format("Map<{}, {}>", cell[w.id.typeAliases][w.id.index][w.id.keyType][w.id.value].label(), cell[w.id.typeAliases][w.id.index][w.id.valueType][w.id.value].label());
        return;
    } else if (isA(w.std.ListNode)) {
        if (&cell == &w.std.ListNode) {
            m_ss << "ListNode";
            return;
        }
        m_ss << fmt::format("ListNode<{}>", cell[w.id.typeAliases][w.id.index][w.id.valueType][w.id.value].label());
        return;
    } else if (isA(w.std.List)) {
        if (&cell == &w.std.List) {
            m_ss << "List";
            return;
        }
        m_ss << fmt::format("List<{}>", cell[w.id.typeAliases][w.id.index][w.id.valueType][w.id.value].label());
        return;
    }
    m_ss << cell.label();
}

void CellValuePrinter::printImpl(CellI& cell)
{
    World& w = cell.w;
    auto is          = [this, &cell, &w](CellI& type) -> bool { return &cell.__type__() == &type || (cell.__type__().has(w.id.memberOf) && cell.__type__()[w.id.memberOf][w.id.index].has(type)); };

    if (is(w.std.Slot)) {
        CellI& cellType = cell[w.id.type];
        m_ss << cell[w.id.key].label() << ": ";
        if (cellType.__type__().has(w.id.memberOf) && cellType.__type__()[w.id.memberOf][w.id.index].has(w.std.ListNode)) {
            m_ss << "ListNode<" << cellType.__type__()[w.id.slots][w.id.index][w.id.value][w.id.value][w.id.type].label() << ">";
        } else {
            m_ss << cellType.label();
        }
        return;
    } else if (is(w.std.List)) {
        if (&cell.__type__()[w.id.typeAliases][w.id.index][w.id.valueType][w.id.value] == &w.std.Char) {
            m_ss << cell.label();
            return;
        }
        printTypeName(cell.__type__());
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
        if (!cell.has(w.id.list)) {
            m_ss << "{}";
            return;
        }
        printTypeName(cell.__type__());
        m_ss << "{";
        visitList(cell[w.id.index][w.id.__type__][w.id.slots][w.id.list], [this, &w, &cell](CellI& value, int i, bool&) {
            if (i != 0) {
                m_ss << ", ";
            }
            value[w.id.key].accept(*this);
            m_ss << ": {";
            cell[w.id.index][value[w.id.key]][w.id.value].accept(*this);
            m_ss << "}";
        });
        m_ss << "}";
        return;
    } else if (is(w.std.Struct)) {
        CellI& type = cell;
        m_ss << "Type ";
        printTypeName(cell);
        if (type.has(w.id.memberOf)) {
            visitList(type[w.id.memberOf][w.id.list], [this, &w](CellI& member, int i, bool&) {
                if (i != 0) {
                    m_ss << ", ";
                } else {
                    m_ss << " : ";
                }
                m_ss << member.label();
            });
        }
        m_ss << " { ";
        if (type.has(w.id.slots)) {
            visitList(type[w.id.slots][w.id.list], [this, &w](CellI& slot, int i, bool&) {
                if (i != 0) {
                    m_ss << ", ";
                }
                m_ss << slot[w.id.key].label() << ": ";
                printTypeName(slot[w.id.type]);
            });
        }
        m_ss << " }";
        return;
    } else if (&cell == &w.id.red || &cell == &w.id.green || &cell == &w.id.blue) {
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
    m_ss << cell.__type__().label() << " { ";

    if (cell.__type__().has(w.id.slots)) {
        visitList(cell.slotList(), [this, &w](CellI& slot, int i, bool&) {
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