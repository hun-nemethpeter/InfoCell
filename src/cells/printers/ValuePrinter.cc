#include <fmt/core.h>

#include "ValuePrinter.h"
#include "cells/World.h"

namespace infocell {
namespace cells {


CellValuePrinter::CellValuePrinter(World& w) :
    NodeBase(w)
{
}

bool CellValuePrinter::prefixByAstVariableType(CellI& astVariable)
{
    if (&astVariable.__type__() == &w.std.ast.Self) {
        m_ss << "self";
        return true;
    } else if (&astVariable.__type__() == &w.std.ast.ConstVar) {
        m_ss << astVariable[w.id.value].label();
        return true;
    } else if (&astVariable.__type__() == &w.std.ast.Member) {
        m_ss << "m_" << astVariable[w.id.key].label();
        return true;
    } else if (&astVariable.__type__() == &w.std.ast.Parameter) {
        m_ss << "p_" << astVariable[w.id.key].label();
        return true;
    } else if (&astVariable.__type__() == &w.std.ast.Var) {
        m_ss << "var_" << astVariable[w.id.name].label();
        return true;
    }

    return false;
}

void CellValuePrinter::printOpBlock(CellI& cell)
{
    CellI& ast = cell[w.id.ast];
    if (&ast.__type__() == &w.std.ast.Call) {
        if (ast.has(w.id.parameters) && (&ast[w.id.parameters][w.id.first][w.id.value][w.id.key] == &w.id.self)) {
            CellI& selfParameterValue = ast[w.id.parameters][w.id.first][w.id.value][w.id.value];
            if (isThisCallAGetter(ast) && cell.label() != "New { call constructor; }") {
                CellI& compiledSelfValue = cell[w.id.parameters][w.id.list][w.id.first][w.id.value][w.id.value];
                printImpl(cell[w.id.parameters][w.id.list][w.id.first][w.id.value]);
            }
            prefixByAstVariableType(selfParameterValue);
        }
        if (&cell.__type__() == &w.std.op.Call) {
            printOpCall(cell);
        }
        return;
    }
    if (&ast.__type__() == &w.std.ast.New) {
        m_ss << "new ";
        CellI* objectTypePtr = nullptr;
        if (ast[w.id.objectType].isA(w.std.ast.ConstVar)) {
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
            CellI& constructorOps = cell[w.id.input][w.id.next][w.id.input];
            printOpBlock(constructorOps);
        }
        return;
    }
    m_ss << "{\n";
    m_indent++;
    CellI* activeNodePtr = &cell[w.id.input];
    while (activeNodePtr) {
        CellI& activeNode = *activeNodePtr;
        printIndent();
        printImpl(activeNode[w.id.input]);
        m_ss << ";\n";
        activeNodePtr = activeNode.getNextOrNullptr();
    }
    m_indent--;
    printIndent();
    m_ss << "}";
}

void CellValuePrinter::printOpActivate(CellI& cell)
{
    m_ss << "activate (";
    printImpl(cell[w.id.cell]);
    m_ss << ")";
}

void CellValuePrinter::printOpFunction(CellI& cell)
{
    std::stringstream iss;
    std::stringstream oss;

    std::string label;
    if (cell.has(w.id.parameters)) {
        int i = 0;
        for (CellI& parameterKV : cell[w.id.parameters]) {
            CellI& parameterKey = parameterKV[w.id.key];
            CellI& slot         = parameterKV[w.id.value];
            if (&parameterKey == &w.id.self) {
                label += slot[w.id.type].label();
                label += "::";
            } else {
                if (i++ > 0) {
                    iss << ", ";
                }
                iss << "p_";
                iss << slot[w.id.key].label() << ": " << slot[w.id.type].label();
            }
        }
    }
    bool hasReturnValue = false;
    if (cell.has(w.id.returnType)) {
        CellI& outType = cell[w.id.returnType];
        hasReturnValue = true;
        oss << " -> " << outType.label();
    }
    label += cell[w.id.name].label();

    bool isStatic = cell.has(w.id.static_);
    std::string staticStr = isStatic ? "static " : "";
    std::string newLabel;

    newLabel = fmt::format("fn {}{}({}){}\n", staticStr, label, iss.str(), oss.str());
    m_ss << newLabel;

    if (&cell[w.id.op].__type__() == &w.std.op.Block) {
        printImpl(cell[w.id.op]);
    } else {
        m_ss << "{\n";
        m_indent++;
        printIndent();
        printImpl(cell[w.id.op]);
        m_ss << ";\n";
        m_indent--;
        printIndent();
        m_ss << "}";
    }
}

bool CellValuePrinter::isThisCallAGetter(CellI& callAst)
{
    if (callAst.missing(w.id.parameters)) {
        return false;
    }
    CellI& selfParameter = callAst[w.id.parameters][w.id.first][w.id.value];
    if (&selfParameter[w.id.key] != &w.id.self) {
        return false;
    }
    if (&selfParameter[w.id.value].__type__() != &w.std.ast.Call) {
        return false;
    }
    CellI& cellMethod = selfParameter[w.id.value][w.id.method];
    if (&cellMethod.__type__() != &w.std.ast.PrimitiveToolName) {
        return false;
    }
    if (&cellMethod[w.id.name] == &w.std.op.Get) {
        return true;
    }

    return false;
}

void CellValuePrinter::printOpCall(CellI& cell)
{
    int paramNum = 0;
    if (cell.has(w.id.ast) && cell[w.id.ast].has(w.id.parameters)) {
        CellI& ast       = cell[w.id.ast];
        CellI& selfParam = ast[w.id.parameters][w.id.first][w.id.value];
        if ((&selfParam[w.id.key] == &w.id.self)) {
            if (isThisCallAGetter(ast) && cell.label() != "New { call constructor; }") {
                CellI& compiledSelfValue = cell[w.id.parameters][w.id.list][w.id.first][w.id.value][w.id.value];
                printImpl(compiledSelfValue);
            }
            prefixByAstVariableType(selfParam[w.id.value]);
        }
        m_ss << ".";
        m_ss << ast[w.id.method][w.id.value].label();
        m_ss << "(";
    } else {
        m_ss << cell[w.id.method].label();
        m_ss << "(";
    }
    if (cell.has(w.id.parameters)) {
        for (CellI& parameter : cell[w.id.parameters]) {
            auto& paramKey   = parameter[w.id.key];
            auto& paramValue = parameter[w.id.value];
            if (&paramKey == &w.id.self) {
                continue;
            }
            if (paramNum++ > 0) {
                m_ss << ", ";
            }
            m_ss << paramKey.label();
            m_ss << "=";
            printImpl(paramValue);
        }
    }
    m_ss << ")";
}

void CellValuePrinter::printOpDelete(CellI& cell)
{
    m_ss << "delete (";
    printImpl(cell[w.id.input]);
    m_ss << ")";
}

void CellValuePrinter::printOpErase(CellI& cell)
{
    printImpl(cell[w.id.cell]);
    m_ss << ".erase(";
    printImpl(cell[w.id.key]);
    m_ss << ")";
}

void CellValuePrinter::printOpIf(CellI& cell)
{
    auto isBlock = [this, &cell](CellI& ast) -> bool {
        if (&ast.__type__() == &w.std.op.Block) {
            auto& astType = ast[w.id.ast].__type__();
            if (&astType == &w.std.ast.Call || &astType == &w.std.ast.New) {
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
        m_ss << "else ";
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
    m_ss << "do ";
    printImpl(cell[w.id.statement]);
    m_ss << " (";
    printImpl(cell[w.id.condition]);
    m_ss << ")";
}

void CellValuePrinter::printOpWhile(CellI& cell)
{
    m_ss << "while (";
    printImpl(cell[w.id.condition]);
    m_ss << ") ";
    printImpl(cell[w.id.statement]);
}

void CellValuePrinter::printOpConstVar(CellI& cell)
{
    if (cell.has(w.id.ast)) {
        CellI& ast = cell[w.id.ast];
        if (&ast.__type__() == &w.std.ast.ConstVar) {
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

void CellValuePrinter::printOpUnknownVar(CellI& cell)
{
    cell[id.value].label().empty() ? (m_ss << "<empty>") : (m_ss << cell[id.value].label());
}

void CellValuePrinter::printOpVar(CellI& cell)
{
    cell.label().empty() ? (m_ss << "<empty>") : (m_ss << cell.label());
}

void CellValuePrinter::printOpNew(CellI& cell)
{
    m_ss << "new ";
    printImpl(cell[w.id.objectType]);
}

void CellValuePrinter::printOpSame(CellI& cell)
{
    printImpl(cell[w.id.lhs]);
    m_ss << " is ";
    printImpl(cell[w.id.rhs]);
}

void CellValuePrinter::printOpNotSame(CellI& cell)
{
    printImpl(cell[w.id.lhs]);
    m_ss << " is not ";
    printImpl(cell[w.id.rhs]);
}

void CellValuePrinter::printOpEqual(CellI& cell)
{
    printImpl(cell[w.id.lhs]);
    m_ss << " == ";
    printImpl(cell[w.id.rhs]);
}

void CellValuePrinter::printOpNotEqual(CellI& cell)
{
    printImpl(cell[w.id.lhs]);
    m_ss << " != ";
    printImpl(cell[w.id.rhs]);
}

void CellValuePrinter::printOpHas(CellI& cell)
{
    if (cell.has(w.id.ast) && prefixByAstVariableType(cell[w.id.ast])) {
        return;
    }
    printImpl(cell[w.id.cell]);
    m_ss << ".has(";
    printImpl(cell[w.id.key]);
    m_ss << ")";
}

void CellValuePrinter::printOpMissing(CellI& cell)
{
    if (cell.has(w.id.ast) && prefixByAstVariableType(cell[w.id.ast])) {
        return;
    }
    printImpl(cell[w.id.cell]);
    m_ss << ".missing(";
    printImpl(cell[w.id.key]);
    m_ss << ")";
}

void CellValuePrinter::printOpSet(CellI& cell)
{

    if (cell.has(w.id.ast)) {
        CellI& astCell = cell[w.id.ast];
        if (cell[id.cell].has(id.ast) && cell[id.key].has(id.ast) && (&cell[id.cell][id.ast].__type__() == &w.std.ast.Self) && (&cell[id.key][w.id.ast].__type__() == &w.std.ast.ConstVar)) {
            CellI& keyAst = cell[id.key][w.id.ast];
            m_ss << "m_" << keyAst[w.id.value].label() << " = ";
            printImpl(cell[w.id.value]);
            return;
        } else if (prefixByAstVariableType(astCell)) {
            return;
        }
    }
    if (cell.label() == "__break__") {
        m_ss << "break";
        return;
    }
    if (cell.label() == "__continue__") {
        m_ss << "continue";
        return;
    }
    printImpl(cell[w.id.cell]);
    m_ss << ".set(";
    printImpl(cell[w.id.key]);
    m_ss << ", ";
    printImpl(cell[w.id.value]);
    m_ss << ")";
}

void CellValuePrinter::printOpGet(CellI& cell)
{
    if (cell.has(w.id.ast) && prefixByAstVariableType(cell[w.id.ast])) {
        return;
    }
    printImpl(cell[w.id.cell]);
    m_ss << ".get(";
    printImpl(cell[w.id.key]);
    m_ss << ")";
}

void CellValuePrinter::printOpAnd(CellI& cell)
{
    printImpl(cell[w.id.lhs]);
    m_ss << " and ";
    printImpl(cell[w.id.rhs]);
}

void CellValuePrinter::printOpOr(CellI& cell)
{
    printImpl(cell[w.id.lhs]);
    m_ss << " or ";
    printImpl(cell[w.id.rhs]);
}

void CellValuePrinter::printOpNot(CellI& cell)
{
    m_ss << "not(";
    printImpl(cell[w.id.input]);
    m_ss << ")";
}

void CellValuePrinter::printOpAdd(CellI& cell)
{
    printImpl(cell[w.id.lhs]);
    m_ss << " + ";
    printImpl(cell[w.id.rhs]);
}

void CellValuePrinter::printOpSubtract(CellI& cell)
{
    printImpl(cell[w.id.lhs]);
    m_ss << " - ";
    printImpl(cell[w.id.rhs]);
}

void CellValuePrinter::printOpMultiply(CellI& cell)
{
    printImpl(cell[w.id.lhs]);
    m_ss << " * ";
    printImpl(cell[w.id.rhs]);
}

void CellValuePrinter::printOpDivide(CellI& cell)
{
    printImpl(cell[w.id.lhs]);
    m_ss << " / ";
    printImpl(cell[w.id.rhs]);
}

void CellValuePrinter::printOpLessThan(CellI& cell)
{
    printImpl(cell[w.id.lhs]);
    m_ss << " < ";
    printImpl(cell[w.id.rhs]);
}

void CellValuePrinter::printOpLessThanOrEqual(CellI& cell)
{
    printImpl(cell[w.id.lhs]);
    m_ss << " <= ";
    printImpl(cell[w.id.rhs]);
}

void CellValuePrinter::printOpGreaterThan(CellI& cell)
{
    printImpl(cell[w.id.lhs]);
    m_ss << " > ";
    printImpl(cell[w.id.rhs]);
}

void CellValuePrinter::printOpGreaterThanOrEqual(CellI& cell)
{
    printImpl(cell[w.id.lhs]);
    m_ss << " >= ";
    printImpl(cell[w.id.rhs]);
}

void CellValuePrinter::printAstCell(CellI& cell)
{
    m_ss << cell[w.id.value].label();
}

void CellValuePrinter::printAstGet(CellI& cell)
{
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
    m_ss << "p_" << cell[w.id.key].label();
}

void CellValuePrinter::printAstVar(CellI& cell)
{
    m_ss << "var_" << cell[w.id.name].label();
}

void CellValuePrinter::printAstMember(CellI& cell)
{
    m_ss << "m_" << cell[w.id.key].label();
}

void CellValuePrinter::printAstSubtract(CellI& cell)
{
    printImpl(cell[w.id.lhs]);
    m_ss << " - ";
    printImpl(cell[w.id.rhs]);
}

void CellValuePrinter::printAstReturn(CellI& cell)
{
    m_ss << "return";
    if (cell.has(w.id.value)) {
        m_ss << " ";
        printImpl(cell[w.id.value]);
    }
}

void CellValuePrinter::printAstSet(CellI& cell)
{
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
    m_ss << "set(";
    printImpl(cell[w.id.cell]);
    m_ss << ", ";
    printImpl(cell[w.id.key]);
    m_ss << ", ";
    printImpl(cell[w.id.value]);
    m_ss << ")";
}

void CellValuePrinter::printOpReturn(CellI& cell)
{
    m_ss << "return";
    if (cell.has(w.id.result)) {
        m_ss << " ";
        printImpl(cell[w.id.result][w.id.value]);
    }
}

void CellValuePrinter::printTypeName(CellI& cell)
{
    m_ss << cell.label();
}

void CellValuePrinter::printStdStruct(CellI& cell)
{
    std::stringstream ss;
    std::vector<std::string> typeAliasesStrs;

    if (cell.has(id.typeAliases)) {
        for (CellI& typeAliasKV : cell[id.typeAliases]) {
            CellI& alias        = typeAliasKV[id.key];
            CellI& type         = typeAliasKV[id.value];
            typeAliasesStrs.push_back(fmt::format("    type {} = {};\n", alias.label(), type.label()));
        }
    }
    if (cell.has(id.memberOf)) {
        ss << " : ";
        int i = 0;
        for (CellI& membershipTypeKV : cell[id.memberOf]) {
            if (i++ > 0) {
                ss << ", ";
            }
            ss << membershipTypeKV[id.key].label();
        }
        ss << " ";
    }
    m_ss << fmt::format("struct {}{}\n", cell.label(), ss.str());
    m_ss << "{\n";
    for (const auto& typeAliasStr : typeAliasesStrs) {
        m_ss << typeAliasStr;
    }
    if (!typeAliasesStrs.empty() && (cell.has(id.methods) || cell.has(id.members))) {
        m_ss << "\n";
    }
    if (cell.has(id.slots)) {
        for (CellI& memberKV : cell[id.slots]) {
            CellI& slot = memberKV[id.value];
            m_ss << fmt::format("    m_{}: {};\n", memberKV[id.key].label(), slot[id.type].label());
        }
    }
    if (cell.has(id.methods)) {
        if (cell.has(id.slots)) {
            m_ss << "\n";
        }
        for (CellI& methodKV : cell[id.methods]) {
            CellI& method = methodKV[id.value];
            m_ss << fmt::format("    {};\n", printShortMethodName(method));
        }
    }
    m_ss << "}\n";
}

std::string CellValuePrinter::printShortMethodName(CellI& method)
{
    std::stringstream iss;
    std::stringstream oss;
    if (method.has(id.parameters)) {
        int i = 0;
        for (CellI& paramKV : method[id.parameters]) {
            CellI& key = paramKV[id.key];
            if (&key == &id.self) {
                continue;
            }
            CellI& param = paramKV[id.value];
            if (i++ > 0) {
                iss << ", ";
            }
            iss << "p_" << param[id.key].label() << ": " << param[id.type].label();
        }
    }
    if (method.has(id.returnType)) {
        oss << " -> " << method[id.returnType].label();
    }
    return fmt::format("fn {}({}){}", method[id.name].label(), iss.str(), oss.str());
}

void CellValuePrinter::printImpl(CellI& cell)
{
    if (cell.isA(w.std.Number)) {
        m_ss << cell.label();
        return;
    } else if (cell.isA(w.std.ast.Slot)) {
        CellI& cellType = cell[w.id.type];
        m_ss << cell[w.id.key].label() << ": ";
        if (cellType.__type__().has(w.id.memberOf) && cellType.__type__()[w.id.memberOf][w.id.index].has(w.std.ListNode)) {
            m_ss << "ListNode<" << cellType.__type__()[w.id.slots][w.id.index][w.id.value][w.id.value][w.id.type].label() << ">";
        } else {
            m_ss << cellType.label();
        }
        return;
    } else if (cell.isA(w.std.List)) {
        if (&cell.__type__()[w.id.typeAliases][w.id.index][w.id.valueType][w.id.value] == &w.std.Char) {
            m_ss << cell.label();
            return;
        }
        printTypeName(cell.__type__());
        m_ss << "[";
        int i = 0;
        for (CellI& value : cell) {
            if (i++ != 0) {
                m_ss << ", ";
            }
            printImpl(value);
        }
        m_ss << "]";
        return;
    } else if (cell.isA(w.std.Map)) {
        if (!cell.has(w.id.list)) {
            m_ss << "{}";
            return;
        }
        printTypeName(cell.__type__());
        m_ss << "{";
        int i = 0;
        for (CellI& value : cell[w.id.index].slotList()) {
            if (i++ != 0) {
                m_ss << ", ";
            }
            printImpl(value[w.id.key]);
            m_ss << ": {";
            printImpl(cell[w.id.index][value[w.id.key]][w.id.value]);
            m_ss << "}";
        }
        m_ss << "}";
        return;
    } else if (cell.isA(std.Struct)) {
        printStdStruct(cell);
        return;
    } else if (cell.__type__().has(w.id.enum_)) {
        m_ss << cell.label();
        return;
    } else if (&cell == &w.id.red || &cell == &w.id.green || &cell == &w.id.blue) {
        m_ss << cell.label();
        return;
    } else if (cell.isA(w.std.op.Block)) {
        printOpBlock(cell);
        return;
    } else if (cell.isA(w.std.op.Activate)) {
        printOpActivate(cell);
        return;
    } else if (cell.isA(w.std.op.Function)) {
        printOpFunction(cell);
        return;
    } else if (cell.isA(w.std.op.Call)) {
        printOpCall(cell);
        return;
    } else if (cell.isA(w.std.op.Delete)) {
        printOpDelete(cell);
        return;
    } else if (cell.isA(w.std.op.Set)) {
        printOpSet(cell);
        return;
    } else if (cell.isA(w.std.op.Erase)) {
        printOpErase(cell);
        return;
    } else if (cell.isA(w.std.op.If)) {
        printOpIf(cell);
        return;
    } else if (cell.isA(w.std.op.Do)) {
        printOpDo(cell);
        return;
    } else if (cell.isA(w.std.op.While)) {
        printOpWhile(cell);
        return;
    } else if (cell.isA(w.std.op.ConstVar)) {
        printOpConstVar(cell);
        return;
    } else if (cell.isA(w.std.op.UnknownVar)) {
        printOpUnknownVar(cell);
        return;
    } else if (cell.isA(w.std.op.Var)) {
        printOpVar(cell);
        return;
    } else if (cell.isA(w.std.op.New)) {
        printOpNew(cell);
        return;
    } else if (cell.isA(w.std.op.Same)) {
        printOpSame(cell);
        return;
    } else if (cell.isA(w.std.op.NotSame)) {
        printOpNotSame(cell);
        return;
    } else if (cell.isA(w.std.op.Equal)) {
        printOpEqual(cell);
        return;
    } else if (cell.isA(w.std.op.NotEqual)) {
        printOpNotEqual(cell);
        return;
    } else if (cell.isA(w.std.op.Has)) {
        printOpHas(cell);
        return;
    } else if (cell.isA(w.std.op.Missing)) {
        printOpMissing(cell);
        return;
    } else if (cell.isA(w.std.op.Get)) {
        printOpGet(cell);
        return;
    } else if (cell.isA(w.std.op.And)) {
        printOpAnd(cell);
        return;
    } else if (cell.isA(w.std.op.Or)) {
        printOpOr(cell);
        return;
    } else if (cell.isA(w.std.op.Not)) {
        printOpNot(cell);
        return;
    } else if (cell.isA(w.std.op.Add)) {
        printOpAdd(cell);
        return;
    } else if (cell.isA(w.std.op.Subtract)) {
        printOpSubtract(cell);
        return;
    } else if (cell.isA(w.std.op.Multiply)) {
        printOpMultiply(cell);
        return;
    } else if (cell.isA(w.std.op.Divide)) {
        printOpDivide(cell);
        return;
    } else if (cell.isA(w.std.op.LessThan)) {
        printOpLessThan(cell);
        return;
    } else if (cell.isA(w.std.op.LessThanOrEqual)) {
        printOpLessThanOrEqual(cell);
        return;
    } else if (cell.isA(w.std.op.GreaterThan)) {
        printOpGreaterThan(cell);
        return;
    } else if (cell.isA(w.std.op.GreaterThanOrEqual)) {
        printOpGreaterThanOrEqual(cell);
        return;
    } else if (cell.isA(w.std.op.Return)) {
        printOpReturn(cell);
        return;
    } else if (cell.isA(w.std.ast.ConstVar)) {
        printAstCell(cell);
        return;
    } else if (cell.isA(w.std.ast.Parameter)) {
        printAstParameter(cell);
        return;
    } else if (cell.isA(w.std.ast.Var)) {
        printAstVar(cell);
        return;
    } else if (cell.isA(w.std.ast.Member)) {
        printAstMember(cell);
        return;
    } else if (cell.isA(w.std.ast.Return)) {
        printAstReturn(cell);
        return;
    }

    if (!cell.label().empty()) {
        m_ss << cell.label() << ": ";
    }
    m_ss << cell.__type__().label() << " { ";

    if (cell.__type__().has(w.id.slots)) {
        int i = 0;
        for (CellI& slotKV : cell.slotList()) {
            if (i != 0) {
                m_ss << ", ";
            }
            m_ss << ".";
            printImpl(slotKV[id.value]);
        }
    }

    m_ss << " }";
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

std::string CellValuePrinter::print(CellI& cell)
{
    m_indent = 0;
    printImpl(cell);
    return m_ss.str();
}

} // namespace cells
} // namespace infocell