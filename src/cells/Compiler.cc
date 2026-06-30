#include "Compiler.h"

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include "util/Log.h"

#include <sstream>

struct Line {
    int left;
    int right;
};

namespace infocell {
namespace cells {

Compiler::Compiler(World& w) :
    w(w),
    m_toolFinder(*new ToolFinder(w)),
    m_earlyStructs(w, w.std.Cell, w.std.Cell, "earlyStructs"),
    m_earlyEnumValues(w, w.std.Cell, w.std.Cell, "earlyEnumValues"),
    m_structs(*new TrieMap(w, w.std.Cell, w.std.Struct, "structs")),
    m_unknownStructs(*new TrieMap(w, w.std.Cell, w.std.Struct, "unknownStructs")),
    m_unknownInstances(*new TrieMap(w, w.std.Cell, w.std.Struct, "unknownInstances"))
{
}

void Compiler::compile(Library& library)
{
    m_libraryPtr = &library;
    compile(library.scope());
}

/*
Compiler steps:
Resolve template related references in normal functions or structs:
  - where templated types is used, for example tt_("List", id.valueType, _(type.Slot))
    it must be resolved to a StructName, with id L,i,s,t,id.valueType,type.Slot
  - create a shadow ast tree with subtituted nodes
  - create a list of candidates for template instantiation with method names
    instantiate structT without methods
    instantiate structT with listed method
  - templates are instantied to a dedicated place
*/
Library& Compiler::compile(Ast::Scope& scope)
{
    if (!m_libraryPtr) {
        m_libraryPtr = new Library(w, scope);
    }

    registerEarlyStructs();

    // Creating a shadow AST tree where templated thing are resolved
    auto& resolvedScope = resolveTypesInScope(scope);

    resolveEarlyStructsInScope(scope, resolvedScope);

    // Processing the shadow AST tree and instantiating templates
    instantiateTemplateInstances();

    // The actual compilation
    compileScope(scope, resolvedScope);

    // Process the descriptions
    processDescriptionsInScope(scope);

    m_libraryPtr->set(w.id.scope, scope);
    m_libraryPtr->set(w.id.resolvedScope, resolvedScope);
    m_libraryPtr->m_toolFinderPtr = &m_toolFinder;

    return *m_libraryPtr;
}

CellI& Compiler::reigisterStructBeforeCompilation(CellI& structAst)
{
    CellI* structIdPtr = nullptr;
    if (&structAst.__type__() == &w.std.ast.TemplatedType) {
        List& idCell = *new List(w, w.std.Cell);
        structIdPtr  = &idCell;
        std::stringstream ss;
        forEach(structAst[w.id.scopes], [this, &idCell, &ss](CellI& scope, int i, bool&) {
            forEach(scope, [this, &idCell, &ss](CellI& character, int, bool&) {
                idCell.add(character);
                ss << character.label();
            });
            idCell.add(w.pools.chars.get(':'));
            idCell.add(w.pools.chars.get(':'));
            ss << "::";
        });
        forEach(structAst[w.id.id], [this, &idCell, &ss](CellI& character, int, bool&) {
            idCell.add(character);
            ss << character.label();
        });

        ss << "<";
        forEach(structAst[w.id.parameters], [this, &idCell, &ss](CellI& slot, int i, bool&) {
            if (i != 0) {
                ss << ", ";
            }
            CellI& key              = slot[w.id.key];
            CellI& type             = slot[w.id.type];
            CellI& compiledSlotType = reigisterStructBeforeCompilation(type);

            idCell.add(key);
            idCell.add(compiledSlotType);
            ss << fmt::format("{}={}", key.label(), compiledSlotType.label());
        });
        ss << ">";
        idCell.label(ss.str());
    } else if (&structAst.__type__() == &w.std.ast.StructName) {
        structIdPtr = &structAst[w.id.name];
    } else if (&structAst.__type__() == &w.std.ast.Cell) {
        return structAst[w.id.value];
    } else {
        throw "Unsupported type!";
    }
    CellI& structId = *structIdPtr;
    if (m_earlyStructs.hasKey(structId)) {
        return m_earlyStructs.getValue(structId);
    } else {
        auto& unresolvedStruct = *new Object(w, w.std.Struct, fmt::format("{}", structId.label()));
        unresolvedStruct.set("incomplete", w.true_);

        m_earlyStructs.add(structId, w.std.slot(structAst, unresolvedStruct));
        return unresolvedStruct;
    }
}

void Compiler::registerBuiltInStruct(const std::string& fullName, CellI& compiledStruct, Ast::Scope* parentScope)
{
    std::vector<std::string> sliced;
    splitNamespacedString(sliced, fullName);

    if (sliced.empty()) {
        throw "Invalid struct ID!";
    }
    std::stringstream ss;
    List& idCell             = *new List(w, w.std.Cell);
    const auto& structName   = sliced.back();
    Ast::Scope* currentScope = parentScope ? parentScope : &w.globalScope;
    if (sliced.size() > 1) {
        for (int i = 0; i < sliced.size() - 1; ++i) {
            const auto& scopeName = sliced[i];
            currentScope          = &currentScope->getItem<Ast::Scope>(scopeName);
            forEach((*currentScope)["name"], [this, &idCell, &ss](CellI& character, int, bool&) {
                idCell.add(character);
                ss << character.label();
            });
            idCell.add(w.pools.chars.get(':'));
            idCell.add(w.pools.chars.get(':'));
            ss << "::";
        }
    }
    Ast::StructBase* structBaseAstPtr = nullptr;
    if (&compiledStruct[w.id.__type__] == &w.std.Struct) {
        Ast::Struct& structAst = currentScope->getItem<Ast::Struct>(structName);
        structBaseAstPtr       = &structAst;
    } else if (&compiledStruct[w.id.__type__] == &w.std.Enum) {
        Ast::Enum& enumAst = currentScope->getItem<Ast::Enum>(structName);
        structBaseAstPtr   = &enumAst;
    } else {
        throw "The compiled type not a struct or enum!";
    }
    Ast::StructBase& structBaseAst = *structBaseAstPtr;

    forEach(structBaseAst[w.id.name], [this, &idCell, &ss](CellI& character, int, bool&) {
        idCell.add(character);
        ss << character.label();
    });
    idCell.label(ss.str());
    compiledStruct.set("incomplete", w.true_);
    m_earlyStructs.add(idCell, w.std.slot(w.__type__(fullName), compiledStruct));
}

void Compiler::registerBuiltInEnumValue(const std::string& fullName, CellI& compiledEnumValue)
{
    std::vector<std::string> sliced;
    splitNamespacedString(sliced, fullName);

    if (sliced.size() < 2) {
        throw "Invalid enum value ID!";
    }
    const auto& enumName      = sliced[sliced.size() - 2];
    const auto& enumValueName = sliced.back();
    List& enumIdCell          = *new List(w, w.std.Cell);
    List& enumValueIdCell     = w.name(enumValueName);
    Ast::Scope* currentScope  = &w.globalScope;

    if (sliced.size() > 1) {
        for (int i = 0; i < sliced.size() - 2; ++i) {
            const auto& scopeName = sliced[i];
            currentScope          = &currentScope->getItem<Ast::Scope>(scopeName);
            forEach((*currentScope)["name"], [this, &enumIdCell](CellI& character, int, bool&) {
                enumIdCell.add(character);
            });
            enumIdCell.add(w.pools.chars.get(':'));
            enumIdCell.add(w.pools.chars.get(':'));
        }
    }
    Ast::Enum& enumAst = currentScope->getItem<Ast::Enum>(enumName);
    if (!enumAst.values().hasKey(enumValueIdCell)) {
        throw "Invalid enum value!";
    }
    Ast::EnumValue& enumValueAst = static_cast<Ast::EnumValue&>(enumAst.values().getValue(enumValueIdCell));
    forEach(enumAst[w.id.name], [this, &enumIdCell](CellI& character, int, bool&) {
        enumIdCell.add(character);
    });
    enumIdCell.add(w.pools.chars.get(':'));
    enumIdCell.add(w.pools.chars.get(':'));
    for (char ch : enumValueName) {
        enumIdCell.add(w.pools.chars.get(ch));
    }

    enumIdCell.label(fullName);
    m_earlyEnumValues.add(enumIdCell, compiledEnumValue);
}

ToolFinder& Compiler::getToolFinder()
{
    return m_toolFinder;
}

Library& Compiler::library()
{
    return *m_libraryPtr;
}

TrieMap& Compiler::compiledFunctions()
{
    return library().functions();
}

TrieMap& Compiler::compiledStructs()
{
    return library().structs();
}
TrieMap& Compiler::compiledVariables()
{
    return library().variables();
}

void Compiler::registerEarlyStructs()
{
    forEach(m_earlyStructs[w.id.list], [this](CellI& earlyStructKV, int i, bool& stop) {
        auto& structId       = earlyStructKV[w.id.key];
        auto& structRefAst   = earlyStructKV[w.id.value][w.id.key];
        auto& compiledStruct = earlyStructKV[w.id.value][w.id.type];

        TRACE(compileStruct, "early struct: {}", earlyStructKV[w.id.key].label());

        auto& structReference = *new Object(w, w.std.StructReference);
        structReference.set(w.id.value, compiledStruct);
        structReference.set(w.id.id, structId);

        if (&structRefAst.__type__() == &w.std.ast.TemplatedType) {
            m_unknownInstances.add(structId, structReference);
        } else if (&structRefAst.__type__() == &w.std.ast.StructName) {
            m_unknownStructs.add(structId, structReference);
        }
    });
}

void Compiler::resolveEarlyStructsInScope(Ast::Scope& scope, Ast::Scope& resolvedScope)
{
    if (m_earlyStructs.empty()) {
        return;
    }
    forEach(m_earlyStructs[w.id.list], [this](CellI& earlyStructKV, int i, bool& stop) {
        auto& structId       = earlyStructKV[w.id.key];
        auto& structRefAst   = earlyStructKV[w.id.value][w.id.key];
        auto& compiledStruct = earlyStructKV[w.id.value][w.id.type];

        TRACE(compileStruct, "resolve early struct: {}", earlyStructKV[w.id.key].label());

        if (&structRefAst.__type__() == &w.std.ast.TemplatedType) {
            if (m_unknownInstances.hasKey(structId)) {
                CellI& structReference = m_unknownInstances.getValue(structId);

                auto& scope         = structReference[w.id.value][w.id.ast][w.id.scope];
                auto& resolvedScope = scope[w.id.resolvedScope];

                structReference.set("scope", scope);
                structReference.set("resolvedScope", resolvedScope);
                structReference.set("templateId", structRefAst["id"]);
                structReference.set(w.id.templateParams, structRefAst[w.id.parameters]);
            }
        } else if (&structRefAst.__type__() == &w.std.ast.StructName) {
            if (m_unknownStructs.hasKey(structId)) {
                CellI& structReference = m_unknownStructs.getValue(structId);
            }
        }
    });
}

Ast::Scope& Compiler::resolveTypesInScope(Ast::Scope& scope)
{
    auto& resolvedScope = *new Ast::Scope(w, scope.label());
    scope.set(w.id.resolvedScope, resolvedScope);
    m_scope = &scope;
    m_resolvedScope = &resolvedScope;

    if (scope.has("functions")) {
        m_currentStruct = nullptr;
        forEach(scope.items<Ast::Function>()[w.id.list], [this, &resolvedScope](CellI& origAstFunctionCell, int i, bool& stop) {
            Ast::Function& origAstFunction     = static_cast<Ast::Function&>(origAstFunctionCell[w.id.value]);
            Ast::Function& resolvedAstFunction = resolveTypesInFunction(origAstFunction);
            resolvedScope.add<Ast::Function>(resolvedAstFunction);
        });
    }
    if (scope.has("structs")) {
        forEach(scope.items<Ast::Struct>()[w.id.list], [this, &resolvedScope](CellI& origAstStructCell, int i, bool& stop) {
            Ast::Struct& origAstStruct     = static_cast<Ast::Struct&>(origAstStructCell[w.id.value]);
            Ast::Struct& resolvedAstStruct = resolveTypesInStruct(origAstStruct);
            resolvedScope.add<Ast::Struct>(resolvedAstStruct);
        });
    }
    if (scope.has("enums")) {
        forEach(scope.items<Ast::Enum>()[w.id.list], [this, &resolvedScope](CellI& origAstEnumCell, int i, bool& stop) {
            Ast::Enum& origAstEnum     = static_cast<Ast::Enum&>(origAstEnumCell[w.id.value]);
            Ast::Enum& resolvedAstEnum = resolveTypesInEnum(origAstEnum);
            resolvedScope.add<Ast::Enum>(resolvedAstEnum);
        });
    }
    if (scope.has("variables")) {
        forEach(scope.items<Ast::Var>()[w.id.list], [this, &resolvedScope](CellI& origAstVarCell, int i, bool& stop) {
            Ast::Var& origAstVar = static_cast<Ast::Var&>(origAstVarCell[w.id.value]);
            resolvedScope.add<Ast::Var>(origAstVar);
        });
    }
    if (scope.has("scopes")) {
        forEach(scope.items<Ast::Scope>()[w.id.list], [this, &scope, &resolvedScope](CellI& origAstScopeCell, int i, bool& stop) {
            Ast::Scope& origAstScope     = static_cast<Ast::Scope&>(origAstScopeCell[w.id.value]);
            if (origAstScope.has("link")) {
                return;
            }
            Ast::Scope& resolvedAstScope = resolveTypesInScope(origAstScope);
            resolvedScope.add<Ast::Scope>(resolvedAstScope);
            m_scope = &scope;
        });
    }

    return resolvedScope;
}

Ast::Function& Compiler::resolveTypesInFunction(Ast::Function& function)
{
    Ast::Function& ret = *new Ast::Function(w, function.get("name"));
    m_currentFn = &ret;
    std::stringstream ss;

    if (function.has("static_")) {
        ret.set("static_", function.get("static_"));
        ss << "static ";
    }
    ss << "fn";
    if (function.has("structType")) {
        auto& structType = function.get("structType");
        ret.set("structType", structType);
        ss << fmt::format(" {}::{}(", structType.label(), function.get("name").label());
    } else {
        ss << fmt::format(" {}(", function.get("name").label());
    }

    if (function.has("parameters")) {
        forEach(function.parameters(), [this, &ret, &ss](CellI& param, int i, bool& stop) {
            CellI& paramId           = param[w.id.key];
            CellI& paramType         = param[w.id.type];
            CellI& resolvedParamType = resolveType(paramType);
            CellI& compiledParamType = getCompiledTypeFromResolvedType(resolvedParamType);
            if (i > 0) {
                ss << ", ";
            }
            ss << fmt::format("{}: {}", paramId.label(), compiledParamType.label());
            ret.parameters(w.ast.slot(paramId, resolvedParamType));
        });
    }
    ss << ")";
    if (function.has("returnType")) {
        ss << " -> ";
        CellI& retType         = function.returnType();
        CellI& resolvedRetType = resolveType(retType);
        CellI& compiledRetType = getCompiledTypeFromResolvedType(resolvedRetType);
        ss << compiledRetType.label();
        ret.returnType(resolvedRetType);
    }
    ret.label(ss.str());

    CellI& resolvedinstructionsAst = resolveTypesInFunctionCode(function.instructions());
    ret.set("instructions", resolvedinstructionsAst);

    return ret;
}

Ast::Base& Compiler::resolveTypesInFunctionCode(CellI& ast)
{
    auto resolveNode = [this](CellI& ast) -> Ast::Base& { return resolveTypesInFunctionCode(ast); };

    if (&ast.__type__() == &w.std.ast.New) {
        CellI& objectType                = ast[w.id.objectType];
        Ast::Base* resolvedObjectTypePtr = nullptr;
        if (&objectType.__type__() == &w.std.ast.Member) {
            resolvedObjectTypePtr = &static_cast<Ast::Base&>(objectType);
        } else {
            resolvedObjectTypePtr = &resolveType(objectType);
        }
        Ast::Base& resolvedObjectType = *resolvedObjectTypePtr;

        if (ast.has("constructor")) {
            auto& constructor = ast[w.id.constructor];
            Ast::Base& ret    = w.ast.new_(resolvedObjectType, static_cast<Ast::Base&>(ast[w.id.constructor]));

            if (ast.has("parameters")) {
                auto& newParameters = *new cells::List(w, w.std.ast.Slot);
                forEach(ast[w.id.parameters], [this, &newParameters, &resolveNode](CellI& slot, int, bool&) {
                    newParameters.add(w.ast.slot(slot[w.id.key], resolveNode(slot[w.id.type])));
                });
                ret.set("parameters", newParameters);
            }
            return ret;
        } else {
            return w.ast.new_(resolvedObjectType);
        }
    }

    if (&ast.__type__() == &w.std.ast.Call || &ast.__type__() == &w.std.ast.StaticCall) {

        Ast::Base* retPtr = nullptr;
        if (&ast.__type__() == &w.std.ast.Call) {
            retPtr = &w.ast.call(resolveNode(ast[w.id.cell]), ast[w.id.method]);
        } else {
            retPtr = &w.ast.scall(resolveType(ast[w.id.cell]), ast[w.id.method]);
        }
        Ast::Base& ret = *retPtr;
        if (ast.has("parameters")) {
            auto& newParameters = *new cells::List(w, w.std.ast.Slot);
            forEach(ast[w.id.parameters], [this, &newParameters, &resolveNode](CellI& slot, int, bool&) {
                newParameters.add(w.ast.slot(slot[w.id.key], resolveNode(slot[w.id.type])));
            });
            ret.set("parameters", newParameters);
        }
        return ret;
    } else if (&ast.__type__() == &w.std.ast.StructName) {
        return resolveType(ast);
    }

    // do nothing just traverse and copy the AST nodes
    if (&ast.__type__() == &w.std.ast.Block) {
        auto& instantiedAsts = *new cells::List(w, w.std.ast.Base);
        forEach(ast[w.id.asts], [this, &instantiedAsts, &resolveNode](CellI& ast, int, bool&) {
            instantiedAsts.add(resolveNode(ast));
        });
        return *new Ast::Block(w, instantiedAsts);
    } else if (&ast.__type__() == &w.std.ast.Cell) {
        return w.ast.cell(ast[w.id.value]);
    } else if (&ast.__type__() == &w.std.ast.SelfFn) {
        return w.ast.selfFn();
    } else if (&ast.__type__() == &w.std.ast.Self) {
        return w.ast.self();
    } else if (&ast.__type__() == &w.std.ast.Continue) {
        return w.ast.continue_();
    } else if (&ast.__type__() == &w.std.ast.Break) {
        return w.ast.break_();
    } else if (&ast.__type__() == &w.std.ast.Try) {
        return w.ast.try_(resolveNode(ast["tryBranch"]), resolveNode(ast["catchBranch"]));
    } else if (&ast.__type__() == &w.std.ast.Throw) {
        if (ast.has(w.id.value)) {
            return w.ast.throw_(resolveNode(ast[w.id.value]));
        } else {
            return w.ast.throw_();
        }
    } else if (&ast.__type__() == &w.std.ast.Parameter) {
        return w.ast.parameter(ast[w.id.key]);
    } else if (&ast.__type__() == &w.std.ast.Var) {
        return w.ast.var(ast[w.id.name]);
    } else if (&ast.__type__() == &w.std.ast.Delete) {
        return w.ast.delete_(resolveNode(ast[w.id.cell]));
    } else if (&ast.__type__() == &w.std.ast.Set) {
        return w.ast.set(resolveNode(ast[w.id.cell]), resolveNode(ast[w.id.key]), resolveNode(ast[w.id.value]));
    } else if (&ast.__type__() == &w.std.ast.Erase) {
        return w.ast.erase(resolveNode(ast[w.id.cell]), resolveNode(ast[w.id.key]));
    } else if (&ast.__type__() == &w.std.ast.If) {
        if (ast.has("else_")) {
            return w.ast.if_(resolveNode(ast[w.id.condition]))
                .then_(resolveNode(ast[w.id.then]))
                .else_(resolveNode(ast[w.id.else_]));
        } else {
            return w.ast.if_(resolveNode(ast[w.id.condition])).then_(resolveNode(ast[w.id.then]));
        }
    } else if (&ast.__type__() == &w.std.ast.Match) {
        auto& ret = w.ast.match_(static_cast<Ast::Base&>(ast["enum"]));
        forEach(ast["cases"][w.id.list], [this, &resolveNode, &ret](CellI& kvPair, int, bool&) {
            auto& key = kvPair[w.id.key];
            auto& op  = resolveNode(kvPair[w.id.value]);
            ret.case_(key, op);
        });
        return ret;
    } else if (&ast.__type__() == &w.std.ast.Do) {
        return w.ast.do_(resolveNode(ast[w.id.statement])).while_(resolveNode(ast[w.id.condition]));
    } else if (&ast.__type__() == &w.std.ast.While) {
        return w.ast.while_(resolveNode(ast[w.id.condition])).do_(resolveNode(ast[w.id.statement]));
    } else if (&ast.__type__() == &w.std.ast.For) {
        return w.ast.for_(resolveNode(ast[w.id.variable])).in(resolveNode(ast[w.id.container]))(resolveNode(ast[w.id.statement]));
    } else if (&ast.__type__() == &w.std.ast.And) {
        return w.ast.and_(resolveNode(ast[w.id.lhs]), resolveNode(ast[w.id.rhs]));
    } else if (&ast.__type__() == &w.std.ast.Or) {
        return w.ast.or_(resolveNode(ast[w.id.lhs]), resolveNode(ast[w.id.rhs]));
    } else if (&ast.__type__() == &w.std.ast.Not) {
        return w.ast.not_(resolveNode(ast[w.id.input]));
    } else if (&ast.__type__() == &w.std.ast.Add) {
        return w.ast.add(resolveNode(ast[w.id.lhs]), resolveNode(ast[w.id.rhs]));
    } else if (&ast.__type__() == &w.std.ast.Subtract) {
        return w.ast.subtract(resolveNode(ast[w.id.lhs]), resolveNode(ast[w.id.rhs]));
    } else if (&ast.__type__() == &w.std.ast.Multiply) {
        return w.ast.multiply(resolveNode(ast[w.id.lhs]), resolveNode(ast[w.id.rhs]));
    } else if (&ast.__type__() == &w.std.ast.Divide) {
        return w.ast.divide(resolveNode(ast[w.id.lhs]), resolveNode(ast[w.id.rhs]));
    } else if (&ast.__type__() == &w.std.ast.LessThan) {
        return w.ast.lessThan(resolveNode(ast[w.id.lhs]), resolveNode(ast[w.id.rhs]));
    } else if (&ast.__type__() == &w.std.ast.LessThanOrEqual) {
        return w.ast.lessThanOrEqual(resolveNode(ast[w.id.lhs]), resolveNode(ast[w.id.rhs]));
    } else if (&ast.__type__() == &w.std.ast.GreaterThan) {
        return w.ast.greaterThan(resolveNode(ast[w.id.lhs]), resolveNode(ast[w.id.rhs]));
    } else if (&ast.__type__() == &w.std.ast.GreaterThanOrEqual) {
        return w.ast.greaterThanOrEqual(resolveNode(ast[w.id.lhs]), resolveNode(ast[w.id.rhs]));
    } else if (&ast.__type__() == &w.std.ast.Same) {
        return w.ast.same(resolveNode(ast[w.id.lhs]), resolveNode(ast[w.id.rhs]));
    } else if (&ast.__type__() == &w.std.ast.NotSame) {
        return w.ast.notSame(resolveNode(ast[w.id.lhs]), resolveNode(ast[w.id.rhs]));
    } else if (&ast.__type__() == &w.std.ast.Equal) {
        return w.ast.equal(resolveNode(ast[w.id.lhs]), resolveNode(ast[w.id.rhs]));
    } else if (&ast.__type__() == &w.std.ast.NotEqual) {
        return w.ast.notEqual(resolveNode(ast[w.id.lhs]), resolveNode(ast[w.id.rhs]));
    } else if (&ast.__type__() == &w.std.ast.Has) {
        return w.ast.has(resolveNode(ast[w.id.cell]), resolveNode(ast[w.id.key]));
    } else if (&ast.__type__() == &w.std.ast.Missing) {
        return w.ast.missing(resolveNode(ast[w.id.cell]), resolveNode(ast[w.id.key]));
    } else if (&ast.__type__() == &w.std.ast.Get) {
        return w.ast.get(resolveNode(ast[w.id.cell]), resolveNode(ast[w.id.key]));
    } else if (&ast.__type__() == &w.std.ast.Member) {
        return w.ast.member(ast[w.id.key]);
    } else if (&ast.__type__() == &w.std.ast.Return) {
        if (ast.has("value")) {
            return w.ast.return_(static_cast<Ast::Base&>(resolveNode(ast[w.id.value])));
        }
        return w.ast.return_();
    }

    throw "Unknown AST to instantiate!";
}

Ast::Struct& Compiler::resolveTypesInStruct(Ast::Struct& astStruct)
{
    auto& structName = astStruct.get(w.id.name);
    Ast::Struct& ret = *new Ast::Struct(w, structName);
    bool isInstance  = astStruct.has("instanceOf");

    if (isInstance) {
        ret.set("instanceOf", astStruct.get("instanceOf"));
        ret.set("templateParams", astStruct.get("templateParams"));
        ret.set("scope", astStruct.get("scope"));
    }

    auto& fullyQualifiedName = getFullyQualifiedName(astStruct);
    CellI* compiledStructPtr = nullptr;
    if (compiledStructs().hasKey(fullyQualifiedName)) {
        compiledStructPtr = &compiledStructs().getValue(fullyQualifiedName);
    } else if (isInstance && m_unknownInstances.hasKey(fullyQualifiedName)) {
        CellI& unknownInstance = m_unknownInstances.getValue(fullyQualifiedName);
        compiledStructPtr      = &unknownInstance["value"];
    } else if(m_unknownStructs.hasKey(fullyQualifiedName)) {
        CellI& unknownStruct = m_unknownStructs.getValue(fullyQualifiedName);
        compiledStructPtr    = &unknownStruct["value"];
        m_unknownStructs.remove(fullyQualifiedName);
    } else {
        compiledStructPtr = new Object(w, w.std.Struct, fmt::format("{}", fullyQualifiedName.label()));
        compiledStructPtr->set("incomplete", w.true_);
    }
    m_structs.add(fullyQualifiedName, *compiledStructPtr);
    ret.set("compiledStruct", *compiledStructPtr);
    if (compiledStructPtr->label() == "std::List<valueType=Number>") {
        std::cout << "";
    }
    ret.set("fullyQualifiedName", fullyQualifiedName);

    m_currentStruct = &ret;

    std::stringstream ss;
    std::vector<std::string> typeAliasesStrs;

    if (IS_LOG_ENABLED) {
    }

    // resolve sub types
    if (astStruct.has("typeAliases")) {
        forEach(astStruct.typeAliases()[w.id.list], [this, &ret, &typeAliasesStrs](CellI& typeAliasSlot, int i, bool& stop) {
            CellI& alias        = typeAliasSlot[w.id.key];
            CellI& type         = typeAliasSlot[w.id.type];
            CellI& resolvedType = resolveType(type);
            ret.typeAliases(w.ast.slot(alias, resolvedType));
            if (IS_LOG_ENABLED) {
                typeAliasesStrs.push_back(fmt::format("    type {} = {};", alias.label(), getCompiledTypeFromResolvedType(resolvedType).label()));
            }
        });
    }

    // resolve memberOf list
    if (astStruct.has("memberOf")) {
        ss << " : ";
        forEach(astStruct.memberOf(), [this, &ret, &ss](CellI& membershipType, int i, bool& stop) {
            CellI& resolvedMembershipType = resolveType(membershipType);
            if (i > 0) {
                ss << ", ";
            }
            ss << getCompiledTypeFromResolvedType(resolvedMembershipType).label();
            ret.memberOf(resolvedMembershipType);
        });
    }
    if (IS_LOG_ENABLED) {
        TRACE(compileStruct, "struct {}{}", astStruct.label(), ss.str());
        TRACE(compileStruct, "{");
        for (const auto& typeAliasStr : typeAliasesStrs) {
            TRACE(compileStruct, typeAliasStr);
        }
        if (!typeAliasesStrs.empty() && (astStruct.has("methods") || astStruct.has("members"))) {
            TRACE(compileStruct, "");
        }
    }

    // resolve methods
    if (astStruct.has("methods")) {
        forEach(astStruct.methods()[w.id.list], [this, &ret](CellI& origAstFunctionCell, int i, bool& stop) {
            auto& origAstFunction     = static_cast<Ast::Function&>(origAstFunctionCell);
            auto& resolvedAstFunction = resolveTypesInFunction(origAstFunction);
            ret.addMethod(resolvedAstFunction);
            TRACE(compileStruct, "    {};", shortFunctionName(resolvedAstFunction));
        });
        if (IS_LOG_ENABLED) {
            if (astStruct.has("members")) {
                TRACE(compileStruct, "");
            }
        }
    }

    // resolve members
    if (astStruct.has("members")) {
        CellI& membersList = astStruct.members()[w.id.list];
        forEach(membersList, [this, &astStruct, &ret](CellI& memberCell, int i, bool& stop) {
            CellI& memberId           = memberCell[w.id.key];
            CellI& memberType         = memberCell[w.id.type];
            CellI& resolvedMemberType = resolveType(memberType);
            ret.members(w.ast.slot(memberId, resolvedMemberType));
            TRACE(compileStruct, "    {}: {};", memberId.label(), getCompiledTypeFromResolvedType(resolvedMemberType).label());
        });
    }

    TRACE(compileStruct, "}");
    TRACE(compileStruct, "");

    return ret;
}

Ast::Enum& Compiler::resolveTypesInEnum(Ast::Enum& astEnum)
{
    auto& enumName = astEnum.get("name");
    Ast::Enum& ret = *new Ast::Enum(w, enumName);
    bool isInstance = astEnum.has("instanceOf");

    if (isInstance) {
        ret.set("instanceOf", astEnum.get("instanceOf"));
        ret.set("templateParams", astEnum.get("templateParams"));
        ret.set("scope", astEnum.get("scope"));
    }

    auto& fullyQualifiedName = getFullyQualifiedName(astEnum);
    CellI* compiledStructPtr = nullptr;
    if (compiledStructs().hasKey(fullyQualifiedName)) {
        compiledStructPtr = &compiledStructs().getValue(fullyQualifiedName);
    } else if (isInstance && m_unknownInstances.hasKey(fullyQualifiedName)) {
        CellI& unknownInstance = m_unknownInstances.getValue(fullyQualifiedName);
        compiledStructPtr      = &unknownInstance["value"];
    } else if (m_unknownStructs.hasKey(fullyQualifiedName)) {
        CellI& unknownStruct = m_unknownStructs.getValue(fullyQualifiedName);
        compiledStructPtr    = &unknownStruct["value"];
        m_unknownStructs.remove(fullyQualifiedName);
        m_structs.add(fullyQualifiedName, *compiledStructPtr);
    } else {
        compiledStructPtr = new Object(w, w.std.Struct, fmt::format("{}", fullyQualifiedName.label()));
        compiledStructPtr->set("incomplete", w.true_);
        m_structs.add(fullyQualifiedName, *compiledStructPtr);
    }
    ret.set("compiledStruct", *compiledStructPtr);
    ret.set("fullyQualifiedName", fullyQualifiedName);

    m_currentStruct = &ret;

    TRACE(compileStruct, "enum {} {{", astEnum.label());

    // resolve methods
    if (astEnum.has("methods")) {
        forEach(astEnum.methods()[w.id.list], [this, &ret](CellI& origAstFunctionCell, int i, bool& stop) {
            auto& origAstFunction     = static_cast<Ast::Function&>(origAstFunctionCell);
            auto& resolvedAstFunction = resolveTypesInFunction(origAstFunction);
            ret.addMethod(resolvedAstFunction);
            TRACE(compileStruct, "    {};", shortFunctionName(resolvedAstFunction));
        });
    }

    // resolve values
    if (astEnum.has("values")) {
        CellI& valuesList = astEnum.values()[w.id.list];
        forEach(valuesList, [this, &ret](CellI& kvPair, int i, bool& stop) {
            CellI& valueCell = kvPair[w.id.value];
            CellI& valueName = valueCell[w.id.name];
            std::stringstream ss;
            if (valueCell.has("enumType")) {
                CellI& valueType                    = valueCell["enumType"];
                CellI& resolvedValueType            = resolveType(valueType);
                Ast::TypedEnumValue& typedEnumValue = *new Ast::TypedEnumValue(w, valueName, resolvedValueType);
                if (valueCell.has(w.id.value)) {
                    typedEnumValue.set(w.id.value, valueCell[w.id.value]);
                }
                ret.values(typedEnumValue);
                if (IS_LOG_ENABLED) {
                    ss << fmt::format("    {}({})", valueName.label(), getCompiledTypeFromResolvedType(resolvedValueType).label());
                }
            } else {
                ret.values(static_cast<Ast::Base&>(valueCell));
                ss << fmt::format("    {}", valueName.label());
            }
            if (IS_LOG_ENABLED) {
                if (valueCell.has(w.id.value)) {
                    ss << fmt::format(" = {}", resolveTypeInEnumValue(valueCell[w.id.value]).label());
                }
                ss << ",";
                TRACE(compileStruct, ss.str());
            }
        });
    }
    TRACE(compileStruct, "}");

    return ret;
}

CellI& Compiler::resolveTypeInEnumValue(CellI& ast)
{
    if (&ast.__type__() == &w.std.ast.Cell) {
        return ast[w.id.value];
    }

    throw "Unknown enum value!";
}

Ast::ResolvedType& Compiler::createResolvedType(CellI& astType, CellI& compiledType)
{
    return Ast::ResolvedType::New(w, astType, compiledType);
}

CellI& Compiler::getFullyQualifiedName(Ast::Base& base)
{
    if (base.has("fullyQualifiedName")) {
        return base.get("fullyQualifiedName");
    }
    bool isEmptyName                  = false;
    CellI* scopeFullyQualifiedNamePtr = nullptr;
    if (base.has(w.id.scope)) {
        Ast::Scope& scope          = static_cast<Ast::Scope&>(base.get(w.id.scope));
        scopeFullyQualifiedNamePtr = &getFullyQualifiedName(scope);
    } else if (base.has("enum")) {
        Ast::Enum& enum_           = static_cast<Ast::Enum&>(base.get("enum"));
        scopeFullyQualifiedNamePtr = &getFullyQualifiedName(enum_);
    } else {
        static List& emptyName     = *new List(w, w.std.Char);
        scopeFullyQualifiedNamePtr = &emptyName;
        isEmptyName                = true;
    }
    CellI& scopeFullyQualifiedName = *scopeFullyQualifiedNamePtr;

    List& fullyQualifiedName = *new List(w, w.std.Char);
    forEach(scopeFullyQualifiedName, [this, &fullyQualifiedName](CellI& character, int i, bool& stop) {
        fullyQualifiedName.add(character);
    });
    if (!fullyQualifiedName.empty()) {
        fullyQualifiedName.add(w.pools.chars.get(':'));
        fullyQualifiedName.add(w.pools.chars.get(':'));
    }
    if (!isEmptyName) {
        auto& name = base.get(w.id.name);
        std::cout << "";
        forEach(name, [this, &fullyQualifiedName](CellI& character, int i, bool& stop) {
            fullyQualifiedName.add(character);
        });
    }
    std::stringstream ss;
    int templateParamPrintModeFromCharIndex = fullyQualifiedName.size();
    if (base.has("instanceOf")) {
        int paramsLength                    = static_cast<List&>(base.get(w.id.templateParams)).size();
        templateParamPrintModeFromCharIndex = fullyQualifiedName.size() - paramsLength * 2;
    }
    forEach(fullyQualifiedName, [this, &fullyQualifiedName, &ss, &templateParamPrintModeFromCharIndex](CellI& character, int i, bool& stop) {
        if (i == templateParamPrintModeFromCharIndex) {
            stop = true;
            return;
        }
        ss << character.label();
    });
    if (base.has("instanceOf")) {
        ss << "<";
        forEach(base.get(w.id.templateParams), [this, &ss](CellI& slot, int i, bool& stop) {
            CellI& key  = slot[w.id.key];
            CellI& type = slot[w.id.type];
            if (i != 0) {
                ss << ", ";
            }
            ss << fmt::format("{}={}", key.label(), getCompiledTypeFromResolvedType(type).label());
        });
        ss << ">";
    }
    fullyQualifiedName.label(ss.str());
    base.set("fullyQualifiedName", fullyQualifiedName);

    return fullyQualifiedName;
}

List& Compiler::generateFullyQualifiedIdFromTemplateId(Ast::Scope& scope, List& name, List& templateParams)
{
    List& idCell = *new List(w, w.std.Cell);
    std::stringstream ss;
    std::list<Ast::Scope*> scopes;
    scopes.push_front(&scope);

    Ast::Scope* childScopePtr = &scope;
    while (childScopePtr->has(w.id.scope)) {
        childScopePtr = &static_cast<Ast::Scope&>(childScopePtr->get(w.id.scope));
        if (childScopePtr->has(w.id.scope)) {
            scopes.push_front(childScopePtr);
        }
    }

    for (Ast::Scope* currentScopePtr : scopes) {
        Ast::Scope& currentScope = *currentScopePtr;
        CellI& scopeName         = currentScope[w.id.name];
        forEach(scopeName, [this, &idCell, &ss](CellI& character, int, bool&) {
            idCell.add(character);
            ss << character.label();
        });
        idCell.add(w.pools.chars.get(':'));
        idCell.add(w.pools.chars.get(':'));
        ss << "::";
    }

    // The generated struct name for a template consist of the template name + parameters
    // Here the name contains the template name + (param1 + value1)+
    int nameLength = name.size() - templateParams.size() * 2;
    forEach(name, [this, &idCell, &ss, &nameLength](CellI& character, int i, bool&) {
        if (i >= nameLength) {
            return;
        }
        idCell.add(character);
        ss << character.label();
    });
    ss << "<";
    forEach(templateParams, [this, &idCell, &ss](CellI& slot, int i, bool&) {
        CellI& key          = slot[w.id.key];
        CellI& type         = slot[w.id.type];
        CellI& compiledType = getCompiledTypeFromResolvedType(type);

        idCell.add(key);
        idCell.add(compiledType);
        if (i != 0) {
            ss << ", ";
        }
        ss << fmt::format("{}={}", key.label(), compiledType.label());
    });
    ss << ">";

    idCell.label(ss.str());

    return idCell;
}

List& Compiler::generateTemplateId(CellI& id, CellI& parameters, List& resolvedParams)
{
    List& idCell = *new List(w, w.std.Cell);
    std::stringstream ss;
    forEach(id, [this, &idCell, &ss](CellI& character, int, bool&) {
        idCell.add(character);
        ss << character.label();
    });

    ss << "<";
    forEach(parameters, [this, &idCell, &ss, &resolvedParams](CellI& slot, int i, bool&) {
        if (i != 0) {
            ss << ", ";
        }
        CellI& key              = slot[w.id.key];
        CellI& type             = slot[w.id.type];
        CellI& resolvedSlotType = resolveType(type);
        CellI& compiledSlotType = getCompiledTypeFromResolvedType(resolvedSlotType);

        idCell.add(key);
        idCell.add(compiledSlotType);
        resolvedParams.add(w.ast.slot(key, resolvedSlotType));
        ss << fmt::format("{}={}", key.label(), compiledSlotType.label());
    });
    ss << ">";
    idCell.label(ss.str());

    return idCell;
}

Ast::Base& Compiler::resolveType(CellI& typeAst)
{
    if (&typeAst.__type__() == &w.std.ast.Cell) {
        auto& cell      = static_cast<Ast::Cell&>(typeAst);
        auto& cellValue = typeAst[w.id.value];
        if (!cellValue.isA(w.std.Struct) && !cellValue.isA(w.std.Enum)) {
            throw "Type AST referencing a non-type!";
        }
        return cell;
    }
    if (&typeAst.__type__() == &w.std.ast.ResolvedType) {
        return static_cast<Ast::ResolvedType&>(typeAst);
    }
    if (&typeAst.__type__() == &w.std.ast.StructName) {
        auto& resolveAstStruct   = findEnumOrStructByAstStructName(*m_scope, typeAst);
        auto& name               = resolveAstStruct[w.id.name];
        auto& fullyQualifiedName = getFullyQualifiedName(resolveAstStruct);

        if (compiledStructs().hasKey(fullyQualifiedName)) {
            return w.ast.cell(compiledStructs().getValue(fullyQualifiedName));
        }

        auto& resolveCompiledStruct = resolveStructName(name, fullyQualifiedName);
        auto& reslvedTypeNode       = createResolvedType(resolveAstStruct, resolveCompiledStruct);

        return reslvedTypeNode;
    }
    if (&typeAst.__type__() == &w.std.ast.TypeAlias) {
        auto& resolveAstStruct = static_cast<Ast::Struct&>(*m_currentStruct).getTypeAlias(typeAst[w.id.name]);
        return resolveAstStruct;
    }
    if (&typeAst.__type__() == &w.std.ast.TemplatedType) {
        return resolveTemplatedType(typeAst);
    }

    throw "Unknown ast type node!";
}

CellI& Compiler::getCompiledTypeFromResolvedType(CellI& resolvedTypeAst)
{
    if (resolvedTypeAst.isA(w.std.ast.Cell)) {
        return resolvedTypeAst[w.id.value];
    } else if (resolvedTypeAst.isA(w.std.ast.ResolvedType)) {
        return resolvedTypeAst[w.id.compiled];
    } else {
        throw "Unexpected AST type!";
    }
}

CellI& Compiler::getOrCreateStructReference(CellI& structId, TrieMap& unresolvedContainer, std::function<CellI&(CellI& structReference)> unknownCb)
{
    if (unresolvedContainer.hasKey(structId)) {
        return unresolvedContainer.getValue(structId)[w.id.value];
    } else {
        auto& structReference  = *new Object(w, w.std.StructReference);
        auto& unresolvedStruct = unknownCb(structReference);
        structReference.set(w.id.value, unresolvedStruct);
        structReference.set(w.id.id, structId);
        structReference.set(w.id.scope, *m_scope);
        structReference.set(w.id.resolvedScope, *m_resolvedScope);

        if (m_currentFn) {
            structReference.set(w.id.currentFn, *m_currentFn);
        }
        if (m_currentStruct) {
            structReference.set(w.id.currentStruct, *m_currentStruct);
        }
        unresolvedContainer.add(structId, structReference);

        return unresolvedStruct;
    }
}

CellI& Compiler::resolveStructName(CellI& name, CellI& fullyQualifiedName)
{
    if (m_structs.hasKey(fullyQualifiedName)) {
        return m_structs.getValue(fullyQualifiedName);
    } else {
        return getOrCreateStructReference(fullyQualifiedName, m_unknownStructs, [this, &name](CellI& structReference) -> CellI& {
            auto& unresolvedStruct = *new Object(w, w.std.Struct, fmt::format("{}", name.label()));
            unresolvedStruct.set("incomplete", w.true_);

            return unresolvedStruct;
        });
    }
}

CellI& Compiler::resolveTemplateInstanceId(CellI& name, CellI& fullyQualifiedName, CellI& idScope, CellI& ast, CellI& templateParams)
{
    auto& templateId = ast[w.id.id];
    return getOrCreateStructReference(fullyQualifiedName, m_unknownInstances, [this, &templateId, &name, &fullyQualifiedName, &templateParams, &ast, &idScope](CellI& structReference) -> CellI& {
        structReference.set(w.id.templateId, templateId);
        structReference.set(w.id.templateParams, templateParams);
        if (ast.has(w.id.scopes)) {
            structReference.set("idScope", idScope);
        }
        auto& unresolvedStruct = *new Object(w, w.std.Struct, fmt::format("{}", name.label()));
        unresolvedStruct.set("fullyQualifiedName", fullyQualifiedName);
        unresolvedStruct.set("incomplete", w.true_);

        return unresolvedStruct;
    });
}

Ast::Base& Compiler::resolveTemplatedType(CellI& ast)
{
    auto& templateId     = ast[w.id.id];
    auto& templateParams = static_cast<List&>(ast[w.id.parameters]);

    Ast::Scope* scopePtr = m_scope;
    if (ast.has(w.id.scopes)) {
        auto& scopeList          = ast[w.id.scopes];
        Ast::StructT& astStructT = findTemplateByNameInScopes(*scopePtr, scopeList, templateId);
        scopePtr                 = &static_cast<Ast::Scope&>(astStructT[w.id.scope]);
    }
    auto& scope = *scopePtr;
    if (!scope.hasItem<Ast::StructT>(templateId)) {
        std::cerr << fmt::format("Unknown template {}", templateId.label()) << std::endl;
        std::cerr << fmt::format("Current scope: {}", scope.label()) << std::endl;
        if (m_currentStruct) {
            std::cerr << fmt::format("Current struct: {}", m_currentStruct->label()) << std::endl;
        }
        if (m_currentFn) {
            std::cerr << fmt::format("Current function: {}", m_currentFn->label()) << std::endl;
        }
        throw fmt::format("Unknown template {}", templateId.label());
    }

    List& resolvedTemplateParams   = *new List(w, w.std.Cell, "resolvedTemplateParams");
    List& structName               = generateTemplateId(templateId, templateParams, resolvedTemplateParams);
    List& fullyQualifiedStructName = generateFullyQualifiedIdFromTemplateId(scope, structName, resolvedTemplateParams);

    if (compiledStructs().hasKey(fullyQualifiedStructName)) {
        return w.ast.cell(compiledStructs().getValue(fullyQualifiedStructName));
    }
    CellI* unknownInstancePtr = nullptr;
    if (m_unknownInstances.hasKey(fullyQualifiedStructName)) {
        CellI& unknownInstance = m_unknownInstances.getValue(fullyQualifiedStructName)[w.id.value];
        if (unknownInstance.has(w.id.ast)) {
            return createResolvedType(unknownInstance[w.id.ast], unknownInstance);
        } else {
            unknownInstancePtr = &unknownInstance;
        }
    }

    auto& resolvedAstInstance = *new Ast::Struct(w, structName);
    resolvedAstInstance.set("fullyQualifiedName", fullyQualifiedStructName);
    resolvedAstInstance.set(w.id.scope, scope);
    resolvedAstInstance.set("instanceOf", scope.getItem<Ast::StructT>(templateId));
    resolvedAstInstance.set("templateParams", resolvedTemplateParams);
    if (unknownInstancePtr) {
        unknownInstancePtr->set(w.id.ast, resolvedAstInstance);
        return createResolvedType(resolvedAstInstance, *unknownInstancePtr);
    }
    auto& resolvedCompiledInstance = resolveTemplateInstanceId(structName, fullyQualifiedStructName, scope, ast, resolvedTemplateParams);
    resolvedCompiledInstance.set(w.id.ast, resolvedAstInstance);

    return createResolvedType(resolvedAstInstance, resolvedCompiledInstance);
}

Ast::Base& Compiler::findEnumOrStructByAstStructName(Ast::Scope& scope, CellI& astStructName)
{
    auto& name = astStructName[w.id.name];
    static List emptyList(w, w.std.Cell, "empty list");
    CellI* scopeListPtr = astStructName.has(w.id.scopes) ? &astStructName[w.id.scopes] : &emptyList;
    auto& scopes        = *scopeListPtr;

    if (Ast::Base* astStruct = findStructByNameInScopes(scope, scopes, name)) {
        return *astStruct;
    }
    if (auto* enum_ = findEnumByNameInScopes(scope, scopes, name)) {
        return *enum_;
    }

    throw "Unknown type name!";
}

Ast::Enum* Compiler::findEnumByNameInScopes(Ast::Scope& scope, CellI& scopeList, CellI& name)
{
    const auto& hasCb = [&name](Ast::Scope& currentScope) -> bool {
        return currentScope.hasItem<Ast::Enum>(name);
    };
    const auto& getCb = [&name](Ast::Scope& currentScope) -> Ast::Enum* {
        return &currentScope.getItem<Ast::Enum>(name);
    };

    return static_cast<Ast::Enum*>(findAstByNameInAllScope(scope, scopeList, hasCb, getCb));
}

Ast::Struct* Compiler::findStructByNameInScopes(Ast::Scope& scope, CellI& scopeList, CellI& name)
{
    const auto& hasCb = [&name](Ast::Scope& currentScope) -> bool {
        return currentScope.hasItem<Ast::Struct>(name);
    };
    const auto& getCb = [&name](Ast::Scope& currentScope) -> Ast::Struct* {
        return &currentScope.getItem<Ast::Struct>(name);
    };

    return static_cast<Ast::Struct*>(findAstByNameInAllScope(scope, scopeList, hasCb, getCb));
}

Ast::StructT& Compiler::findTemplateByNameInScopes(Ast::Scope& scope, CellI& scopeList, CellI& name)
{
    const auto& hasCb = [&name](Ast::Scope& currentScope) -> bool {
        return currentScope.hasItem<Ast::StructT>(name);
    };
    const auto& getCb = [&name](Ast::Scope& currentScope) -> Ast::StructT* {
        return &currentScope.getItem<Ast::StructT>(name);
    };
    Ast::Base* resolvedAst = findAstByNameInAllScope(scope, scopeList, hasCb, getCb);
    if (!resolvedAst) {
        throw "Unknown template name!";
    }

    return static_cast<Ast::StructT&>(*resolvedAst);
}

Ast::Base* Compiler::findAstByNameInAllScope(Ast::Scope& scope, CellI& scopeList, std::function<bool(Ast::Scope&)> hasCb, std::function<Ast::Base*(Ast::Scope&)> getCb)
{
    Ast::Scope* currentScope = &scope;
    if (Ast::Scope* linkedScope = currentScope->getLinkedScope()) {
        currentScope = linkedScope;
    }

    while (currentScope) {
        Ast::Base* ret = findAstByNameInOneScope(currentScope, scopeList, hasCb, getCb);
        if (ret) {
            return ret;
        }
        // resolve in parent scope
        if (currentScope->has(w.id.scope)) {
            currentScope = &static_cast<Ast::Scope&>(currentScope->get(w.id.scope));
            if (Ast::Scope* linkedScope = currentScope->getLinkedScope()) {
                currentScope = linkedScope;
            }
        } else {
            currentScope = nullptr;
        }
    }

    return nullptr;
}

Ast::Base* Compiler::findAstByNameInOneScope(Ast::Scope* currentScope, CellI& scopeList, std::function<bool(Ast::Scope&)> hasCb, std::function<Ast::Base*(Ast::Scope&)> getCb)
{
    // resolve in local scope
    forEach(scopeList, [this, &currentScope](CellI& scopeId, int, bool& stop) {
        if (currentScope->hasItem<Ast::Scope>(scopeId)) {
            currentScope = &currentScope->getItem<Ast::Scope>(scopeId);
            if (Ast::Scope* linkedScope = currentScope->getLinkedScope()) {
                currentScope = linkedScope;
            }
        } else {
            currentScope = nullptr;
            stop         = true;
        }
    });
    if (currentScope && hasCb(*currentScope)) {
        return getCb(*currentScope);
    }

    return nullptr;
}

void Compiler::instantiateTemplateInstances()
{
    // Sanity check we still referencing an unknown struct
    // Print all unknown references before bail out
    forEach(m_unknownStructs[w.id.list], [this](CellI& unknownStruct, int i, bool& stop) {
        WARN(compileStruct, "unknown struct: {}", unknownStruct[w.id.value][w.id.value].label());
    });

    int instantiedNum = 0;
    forEach(m_unknownInstances[w.id.list], [this, &instantiedNum](CellI& unknownInstanceSlot, int i, bool& stop) {
        CellI& unknownInstance  = unknownInstanceSlot[w.id.value];
        auto& unknownInstanceId = unknownInstance[w.id.id];

        if (IS_LOG_ENABLED) {
            TRACE(compileStruct, "unknown instance: {}", unknownInstanceId.label());
            if (unknownInstance.has("currentStruct")) {
                TRACE(compileStruct, "     from struct: {}", unknownInstance[w.id.currentStruct].label());
            }
            if (unknownInstance.has("currentFn")) {
                TRACE(compileStruct, "   from function: {}", unknownInstance[w.id.currentFn].label());
            }
        }

        std::stringstream ss;

        CellI& templateId     = unknownInstance[w.id.templateId];
        auto& templateParams  = static_cast<List&>(unknownInstance[w.id.templateParams]);
        auto& scope           = static_cast<Ast::Scope&>(unknownInstance[w.id.scope]);
        auto& idScope         = unknownInstance.has(w.name("idScope")) ? static_cast<Ast::Scope&>(unknownInstance[w.name("idScope")]) : scope;

        ss << fmt::format("        in scope: {}", getFullyQualifiedName(idScope).label());
        ss << fmt::format("  instantiate id: {}<", templateId.label());
        forEach(templateParams, [this, &ss](CellI& param, int i, bool& stop) {
            CellI& paramId   = param[w.id.key];
            CellI& paramType = param[w.id.type];
            if (i > 0) {
                ss << ", ";
            }
            ss << fmt::format("{}: {}", paramId.label(), getCompiledTypeFromResolvedType(paramType).label());
        });
        ss << ">";
        TRACE(compileStruct, ss.str());

        // we instantiate the template where we found it not where it belongs to
        // so std::List<Test> will not be instantiated in the std:: resolved scope
        auto& resolvedIdScope  = static_cast<Ast::Scope&>(unknownInstance[w.id.resolvedScope]);
        auto& compiledStruct   = static_cast<Ast::Struct&>(unknownInstance[w.id.value][w.id.ast]);
        m_scope                = &idScope;
        auto& structT          = idScope.getItem<Ast::StructT>(templateId);
        auto& instantiedStruct = instantiateStructT(structT, compiledStruct, templateParams);
        auto& resolvedStruct   = resolveTypesInStruct(instantiedStruct);
        resolvedIdScope.add<Ast::Struct>(resolvedStruct);
        instantiedNum = i + 1;
    });

    if (m_unknownStructs.size() > 0 || m_unknownInstances.size() != instantiedNum) {
        throw "Referencing an unknown type!";
    }
}

Ast::Struct& Compiler::instantiateStructT(Ast::StructT& structT, Ast::Struct& compiledStruct, List& inputParams)
{
    // process input parameters
    if (!inputParams.empty() && structT.missing("templateParams")) {
        throw "No template parameter was given!";
    }
    std::stringstream ss;
    Map inputParameters(w, w.std.Cell, w.std.Cell);

    forEach(inputParams, [this, &structT, &inputParameters, &ss](CellI& slot, int i, bool& stop) {
        CellI& key  = slot[w.id.key];
        CellI& type = slot[w.id.type];
        inputParameters.add(key, type);
    });

    Ast::Struct& ret = compiledStruct;
    ret.set("instanceOf", structT);
    ret.set("templateParams", inputParams);
    ret.set("scope", static_cast<Ast::Scope&>(structT.get("scope")));

    // instantiate type aliases
    if (structT.has("typeAliases")) {
        Map& instantiatedTypeAliases = *new Map(w, w.std.Cell, w.std.ast.Base);
        forEach(structT.typeAliases()[w.id.list], [this, &inputParameters, &instantiatedTypeAliases, &ret](CellI& slot, int i, bool& stop) {
            CellI& key               = slot[w.id.key];
            CellI& type              = slot[w.id.type];
            CellI& instantiatedParam = instantiateTemplateParamType(type, ret, inputParameters);
            instantiatedTypeAliases.add(key, w.ast.slot(key, instantiatedParam));
        });
        ret.set("typeAliases", instantiatedTypeAliases);
    }

    // instantiate methods
    if (structT.has("methods")) {
        forEach(structT.methods()[w.id.list], [this, &inputParameters, &ret](CellI& astFunctionRef, int i, bool& stop) {
            auto& astFunction = static_cast<Ast::Function&>(astFunctionRef);
            instantiateFunctionInStructT(astFunction, ret, inputParameters);
         });
    }

    // instantiate members
    if (structT.has("members")) {
        Map& instantiatedMembers = *new Map(w, w.std.Cell, w.std.Slot);
        forEach(structT.members()[w.id.list], [this, &inputParameters, &instantiatedMembers, &ret](CellI& slot, int i, bool& stop) {
            CellI& key               = slot[w.id.key];
            CellI& type              = slot[w.id.type];
            CellI& instantiatedParam = instantiateTemplateParamType(type, ret, inputParameters);
            instantiatedMembers.add(key, w.ast.slot(key, instantiatedParam));
        });
        ret.set("members", instantiatedMembers);
    }

    // instantiate memberOf list
    if (structT.has("memberOf")) {
        List& instantiatedMemberOfs = *new List(w, w.std.Struct);
        forEach(structT.memberOf(), [this, &inputParameters, &instantiatedMemberOfs, &ret](CellI& membershipType, int i, bool& stop) {
            CellI& instantiatedParam = instantiateTemplateParamType(membershipType, ret, inputParameters);
            instantiatedMemberOfs.add(instantiatedParam);
        });
        ret.set("memberOf", instantiatedMemberOfs);
    }

    // trait implementations
    if (structT.has("traitImpls")) {
        CellI& traitImplsList = structT.traitImpls()[w.id.list];

        forEach(traitImplsList, [this, &structT, &ret, &inputParameters](CellI& traitImpl, int i, bool& stop) {
            forEach(traitImpl[w.id.methods][w.id.list], [this, &structT, &ret, &inputParameters, &traitImpl](CellI& astFunctionRef, int i, bool& stop) {
                Map* associatedTypesPtr = nullptr;
                if (traitImpl.has("associatedTypes")) {
                    associatedTypesPtr = &static_cast<Map&>(traitImpl["associatedTypes"]);
                }

                auto& astFunction = static_cast<Ast::Function&>(astFunctionRef);
                instantiateFunctionInStructT(astFunction, ret, inputParameters, associatedTypesPtr);
            });
        });
    }

    return ret;
}

void Compiler::instantiateFunctionInStructT(Ast::Function& astFunction, Ast::Struct& compiledStruct, Map& inputParameters, Map* associatedTypesPtr)
{
    Ast::Function& instantiedFunction = *new Ast::Function(w, astFunction[w.id.name]);
    compiledStruct.addMethod(instantiedFunction);

    // parameters
    if (astFunction.has("parameters")) {
        List& instantiatedParameters = *new List(w, w.std.Slot);
        forEach(astFunction[w.id.parameters], [this, &inputParameters, &associatedTypesPtr, &instantiatedParameters, &compiledStruct](CellI& slot, int i, bool& stop) {
            CellI& key               = slot[w.id.key];
            CellI& type              = slot[w.id.type];
            CellI& instantiatedParam = instantiateTemplateParamType(type, compiledStruct, inputParameters, associatedTypesPtr);
            instantiatedParameters.add(w.ast.slot(key, instantiatedParam));
        });
        instantiedFunction.set("parameters", instantiatedParameters);
    }
    // return type
    if (astFunction.has("returnType")) {
        CellI& returnType             = astFunction[w.id.returnType];
        CellI& instantiatedReturnType = instantiateTemplateParamType(returnType, compiledStruct, inputParameters, associatedTypesPtr);
        instantiedFunction.set("returnType", instantiatedReturnType);
    }
    // instructions
    if (astFunction.has("instructions")) {
        instantiedFunction.set("instructions", instantiateAst(astFunction[w.id.instructions], compiledStruct, inputParameters, associatedTypesPtr));
    }
}

CellI& Compiler::instantiateTemplateParamType(CellI& param, CellI& selfType, Map& inputParameters, Map* associatedTypesPtr)
{
    if (&param.__type__() == &w.std.ast.TemplateParam) {
        CellI& paramValue = param[w.id.key];
        if (!inputParameters.hasKey(paramValue)) {
            throw "Instantiating with unknown template parameter!";
        }
        return inputParameters.getValue(paramValue);
    }
    if (&param.__type__() == &w.std.ast.TemplatedType) {
        List& resolvedParameterList = *new List(w, w.std.Slot);
        auto& ret                   = *new Ast::TemplatedType(w, param[w.id.id], resolvedParameterList);
        auto& parametersList        = param[w.id.parameters];

        forEach(parametersList, [this, &resolvedParameterList, &selfType, &inputParameters, &associatedTypesPtr](CellI& slot, int, bool&) {
            CellI& key              = slot[w.id.key];
            CellI& type             = slot[w.id.type];
            CellI& resolvedSlotType = instantiateTemplateParamType(type, selfType, inputParameters, associatedTypesPtr);
            resolvedParameterList.add(w.ast.slot(key, resolvedSlotType));
        });

        return ret;
    }
    if (&param.__type__() == &w.std.ast.AssociatedType && associatedTypesPtr) {
        Map& associatedTypes = *associatedTypesPtr;
        CellI& paramValue = param[w.id.key];
        if (!associatedTypes.hasKey(paramValue)) {
            throw "Instantiating with unknown associated type parameter!";
        }
        return instantiateTemplateParamType(associatedTypes.getValue(paramValue), selfType, inputParameters, associatedTypesPtr);
    }

    if (&param.__type__() == &w.std.ast.Cell || &param.__type__() == &w.std.ast.StructName || &param.__type__() == &w.std.ast.TypeAlias) {
        return param;
    }

    throw "Unknown template parameter!";
}

Ast::Base& Compiler::instantiateAst(CellI& ast, CellI& selfType, Map& inputParameters, Map* associatedTypesPtr)
{
    auto instantiate = [this, &selfType, &inputParameters, associatedTypesPtr](CellI& ast) -> Ast::Base& { return instantiateAst(ast, selfType, inputParameters, associatedTypesPtr); };

    if (&ast.__type__() == &w.std.ast.New) {
        auto* objectTypePtr = &ast[w.id.objectType];
        if (&(*objectTypePtr).__type__() == &w.std.ast.TemplatedType || &(*objectTypePtr).__type__() == &w.std.ast.AssociatedType) {
            CellI& resolvedObjectType = instantiateTemplateParamType(*objectTypePtr, selfType, inputParameters, associatedTypesPtr);
            objectTypePtr             = &resolvedObjectType;
        }
        auto& objectType = *static_cast<Ast::Base*>(objectTypePtr);
        if (ast.has("constructor")) {
            auto& constructor = ast[w.id.constructor];
            Ast::Base& ret    = w.ast.new_(objectType, static_cast<Ast::Base&>(constructor));
            if (ast.has("parameters")) {
                auto& newParameters = *new cells::List(w, w.std.ast.Slot);
                forEach(ast[w.id.parameters], [this, &newParameters, &instantiate](CellI& slot, int, bool&) {
                    newParameters.add(w.ast.slot(slot[w.id.key], instantiate(slot[w.id.type])));
                });
                ret.set("parameters", newParameters);
            }
            return ret;
        } else {
            return w.ast.new_(objectType);
        }
    } else if (&ast.__type__() == &w.std.ast.Call) {
        Ast::Base& ret = w.ast.call(instantiate(ast[w.id.cell]), ast[w.id.method]);
        if (ast.has("parameters")) {
            // TODO process parameters
            ret.set("parameters", ast[w.id.parameters]);
        }
        return ret;
    } else if (&ast.__type__() == &w.std.ast.StaticCall) {
        Ast::Base& ret = w.ast.scall(instantiate(ast[w.id.cell]), ast[w.id.method]);
        if (ast.has("parameters")) {
            // TODO process parameters
            ret.set("parameters", ast[w.id.parameters]);
        }
        return ret;
    }

    // do nothing just traverse and copy the AST nodes
    if (&ast.__type__() == &w.std.ast.Block) {
        auto& instantiedAsts = *new cells::List(w, w.std.ast.Base);
        forEach(ast[w.id.asts], [this, &instantiedAsts, &instantiate](CellI& ast, int, bool&) {
            instantiedAsts.add(instantiate(ast));
        });
        return *new Ast::Block(w, instantiedAsts);
    } else if (&ast.__type__() == &w.std.ast.Cell) {
        return w.ast.cell(ast[w.id.value]);
    } else if (&ast.__type__() == &w.std.ast.StructName) {
        auto& ret = w.ast.structName(ast[w.id.value]);
        if (ast.has(w.id.scopes)) {
            ret.set(w.id.scopes, ast[w.id.scopes]);
        }
        return ret;
    } else if (&ast.__type__() == &w.std.ast.SelfFn) {
        return w.ast.selfFn();
    } else if (&ast.__type__() == &w.std.ast.Self) {
        return w.ast.self();
    } else if (&ast.__type__() == &w.std.ast.Continue) {
        return w.ast.continue_();
    } else if (&ast.__type__() == &w.std.ast.Break) {
        return w.ast.break_();
    } else if (&ast.__type__() == &w.std.ast.Try) {
        return w.ast.try_(instantiate(ast["tryBranch"]), instantiate(ast["catchBranch"]));
    } else if (&ast.__type__() == &w.std.ast.Throw) {
        if (ast.has(w.id.value)) {
            return w.ast.throw_(instantiate(ast[w.id.value]));
        } else {
            return w.ast.throw_();
        }
    } else if (&ast.__type__() == &w.std.ast.Parameter) {
        return w.ast.parameter(ast[w.id.key]);
    } else if (&ast.__type__() == &w.std.ast.Var) {
        return w.ast.var(ast[w.id.name]);
    } else if (&ast.__type__() == &w.std.ast.Delete) {
        return w.ast.delete_(instantiate(ast[w.id.cell]));
    } else if (&ast.__type__() == &w.std.ast.Set) {
        return w.ast.set(instantiate(ast[w.id.cell]), instantiate(ast[w.id.key]), instantiate(ast[w.id.value]));
    } else if (&ast.__type__() == &w.std.ast.Erase) {
        return w.ast.erase(instantiate(ast[w.id.cell]), instantiate(ast[w.id.key]));
    } else if (&ast.__type__() == &w.std.ast.If) {
        if (ast.has("else_")) {
            return w.ast.if_(instantiate(ast[w.id.condition]))
                .then_(instantiate(ast[w.id.then]))
                .else_(instantiate(ast[w.id.else_]));
        } else {
            return w.ast.if_(instantiate(ast[w.id.condition])).then_(instantiate(ast[w.id.then]));
        }
    } else if (&ast.__type__() == &w.std.ast.Do) {
        return w.ast.do_(instantiate(ast[w.id.statement])).while_(instantiate(ast[w.id.condition]));
    } else if (&ast.__type__() == &w.std.ast.While) {
        return w.ast.while_(instantiate(ast[w.id.condition])).do_(instantiate(ast[w.id.statement]));
    } else if (&ast.__type__() == &w.std.ast.For) {
        return w.ast.for_(instantiate(ast[w.id.variable])).in(instantiate(ast[w.id.container]))(instantiate(ast[w.id.statement]));
    } else if (&ast.__type__() == &w.std.ast.And) {
        return w.ast.and_(instantiate(ast[w.id.lhs]), instantiate(ast[w.id.rhs]));
    } else if (&ast.__type__() == &w.std.ast.Or) {
        return w.ast.or_(instantiate(ast[w.id.lhs]), instantiate(ast[w.id.rhs]));
    } else if (&ast.__type__() == &w.std.ast.Not) {
        return w.ast.not_(instantiate(ast[w.id.input]));
    } else if (&ast.__type__() == &w.std.ast.Add) {
        return w.ast.add(instantiate(ast[w.id.lhs]), instantiate(ast[w.id.rhs]));
    } else if (&ast.__type__() == &w.std.ast.Subtract) {
        return w.ast.subtract(instantiate(ast[w.id.lhs]), instantiate(ast[w.id.rhs]));
    } else if (&ast.__type__() == &w.std.ast.Multiply) {
        return w.ast.multiply(instantiate(ast[w.id.lhs]), instantiate(ast[w.id.rhs]));
    } else if (&ast.__type__() == &w.std.ast.Divide) {
        return w.ast.divide(instantiate(ast[w.id.lhs]), instantiate(ast[w.id.rhs]));
    } else if (&ast.__type__() == &w.std.ast.LessThan) {
        return w.ast.lessThan(instantiate(ast[w.id.lhs]), instantiate(ast[w.id.rhs]));
    } else if (&ast.__type__() == &w.std.ast.LessThanOrEqual) {
        return w.ast.lessThanOrEqual(instantiate(ast[w.id.lhs]), instantiate(ast[w.id.rhs]));
    } else if (&ast.__type__() == &w.std.ast.GreaterThan) {
        return w.ast.greaterThan(instantiate(ast[w.id.lhs]), instantiate(ast[w.id.rhs]));
    } else if (&ast.__type__() == &w.std.ast.GreaterThanOrEqual) {
        return w.ast.greaterThanOrEqual(instantiate(ast[w.id.lhs]), instantiate(ast[w.id.rhs]));
    } else if (&ast.__type__() == &w.std.ast.Same) {
        return w.ast.same(instantiate(ast[w.id.lhs]), instantiate(ast[w.id.rhs]));
    } else if (&ast.__type__() == &w.std.ast.NotSame) {
        return w.ast.notSame(instantiate(ast[w.id.lhs]), instantiate(ast[w.id.rhs]));
    } else if (&ast.__type__() == &w.std.ast.Equal) {
        return w.ast.equal(instantiate(ast[w.id.lhs]), instantiate(ast[w.id.rhs]));
    } else if (&ast.__type__() == &w.std.ast.NotEqual) {
        return w.ast.notEqual(instantiate(ast[w.id.lhs]), instantiate(ast[w.id.rhs]));
    } else if (&ast.__type__() == &w.std.ast.Has) {
        return w.ast.has(instantiate(ast[w.id.cell]), instantiate(ast[w.id.key]));
    } else if (&ast.__type__() == &w.std.ast.Missing) {
        return w.ast.missing(instantiate(ast[w.id.cell]), instantiate(ast[w.id.key]));
    } else if (&ast.__type__() == &w.std.ast.Get) {
        return w.ast.get(instantiate(ast[w.id.cell]), instantiate(ast[w.id.key]));
    } else if (&ast.__type__() == &w.std.ast.Member) {
        return w.ast.member(ast[w.id.key]);
    } else if (&ast.__type__() == &w.std.ast.Return) {
        if (ast.has("value")) {
            return w.ast.return_(static_cast<Ast::Base&>(instantiate(ast[w.id.value])));
        }
        return w.ast.return_();
    }

    throw "Unknown AST to instantiate!";
}

void Compiler::compileScope(Ast::Scope& scope, Ast::Scope& resolvedScope)
{
    m_scope         = &scope;
    m_resolvedScope = &resolvedScope;

    if (scope.has("functions")) {
        forEach(resolvedScope.items<Ast::Function>()[w.id.list], [this](CellI& kvPair, int i, bool& stop) {
            Ast::Function& astFunction = static_cast<Ast::Function&>(kvPair[w.id.value]);
            auto& compiledFunction = compileFunction(astFunction);
            compiledFunctions().add(getFullyQualifiedName(astFunction), compiledFunction);
        });
    }
    if (scope.has("structs")) {
        forEach(resolvedScope.items<Ast::Struct>()[w.id.list], [this](CellI& kvPair, int i, bool& stop) {
            Ast::Struct& astStruct = static_cast<Ast::Struct&>(kvPair[w.id.value]);
            compileStruct(astStruct);
        });
    }
    if (scope.has("enums")) {
        forEach(resolvedScope.items<Ast::Enum>()[w.id.list], [this](CellI& kvPair, int i, bool& stop) {
            Ast::Enum& astEnum = static_cast<Ast::Enum&>(kvPair[w.id.value]);
            compileEnum(astEnum);
        });
    }
    if (scope.has("variables")) {
        forEach(resolvedScope.items<Ast::Var>()[w.id.list], [this](CellI& kvPair, int i, bool& stop) {
            Ast::Var& astVar = static_cast<Ast::Var&>(kvPair[w.id.value]);
            auto& varName    = getFullyQualifiedName(astVar);
            if (compiledVariables().hasKey(varName)) {
                return;
            }
            auto& compiledVariable = *new Object(w, w.std.op.Var, fmt::format("var {}", astVar.label()));
            compiledVariables().add(varName, compiledVariable);
        });
    }
    if (scope.has("scopes")) {
        forEach(scope.items<Ast::Scope>()[w.id.list], [this, &scope, &resolvedScope](CellI& kvPair, int i, bool& stop) {
            Ast::Scope& nextScope = static_cast<Ast::Scope&>(kvPair[w.id.value]);
            if (nextScope.has("link")) {
                return;
            }
            auto& nextResolvedScope = resolvedScope.getItem<Ast::Scope>(nextScope[w.id.name]);

            compileScope(nextScope, nextResolvedScope);

            m_scope         = &scope;
            m_resolvedScope = &resolvedScope;
        });
    }
}

void Compiler::compileStruct(Ast::Struct& astStruct)
{
    CellI& compiledStruct = astStruct["compiledStruct"];
    if (compiledStruct.missing("incomplete")) {
        return;
    }
    compiledStruct.erase("incomplete");

    CellI& compiledStructName = getFullyQualifiedName(astStruct);

    // compile sub types
    if (astStruct.has("typeAliases")) {
        Map& compiledTypeAliases = *new Map(w, w.std.Cell, w.std.Struct, "typeAliases Map<Cell, Type>(...)");
        forEach(astStruct.typeAliases()[w.id.list], [this, &compiledTypeAliases](CellI& slot, int i, bool& stop) {
            CellI& key             = slot[w.id.key];
            CellI& type            = slot[w.id.type];
            auto& compiledSlotType = getCompiledTypeFromResolvedType(type);
            compiledTypeAliases.add(key, compiledSlotType);
        });
        compiledStruct.set("typeAliases", compiledTypeAliases);
        CellI& typeAliasesIndex = compiledTypeAliases[w.id.index];
        forEach(typeAliasesIndex.slotList(), [this, &compiledStructName, &typeAliasesIndex](CellI& typeAlias, int i, bool& stop) {
            CellI& key      = typeAlias["key"];
            CellI& value    = typeAliasesIndex[key][w.id.value];
            List& aliasName = *new List(w, w.std.Char);
            forEach(compiledStructName, [&aliasName](CellI& character, int i, bool& stop) {
                aliasName.add(character);
            });
            aliasName.add(w.pools.chars.get(':'));
            aliasName.add(w.pools.chars.get(':'));
            forEach(key, [&aliasName](CellI& character, int i, bool& stop) {
                aliasName.add(character);
            });
            aliasName.label(compiledStructName.label() + "::" + key.label());
            if (IS_LOG_ENABLED) {
                TRACE(compileStruct, "type alias {}: {}", aliasName.label(), value.label());
            }
            compiledStructs().add(aliasName, value);
        });
    }

    // compile methods
    if (astStruct.has("methods")) {
        Map& compiledMethods = *new Map(w, w.std.Cell, w.std.ast.Function);
        forEach(astStruct.methods()[w.id.list], [this, &compiledMethods](CellI& astFunction, int i, bool& stop) {
            auto& compiledFunction = compileFunction(static_cast<Ast::Function&>(astFunction));
            compiledMethods.add(astFunction[w.id.name], compiledFunction);
        });
        compiledStruct.set("methods", compiledMethods);
    }

    // compile members
    if (astStruct.has("members")) {
        Map& compiledMembers = *new Map(w, w.std.Cell, w.std.Slot, "members Map<Cell, Slot>(...)");
        forEach(astStruct.members()[w.id.list], [this, &compiledMembers, &compiledStruct](CellI& slot, int i, bool& stop) {
            CellI& key             = slot[w.id.key];
            CellI& type            = slot[w.id.type];
            auto& compiledSlotType = getCompiledTypeFromResolvedType(type);
            compiledMembers.add(key, w.std.slot(key, compiledSlotType));
        });
        compiledStruct.set("slots", compiledMembers);
    }

    // compile memberOf list
    if (astStruct.has("memberOf")) {
        Map& compiledMemberOfs = *new Map(w, w.std.Struct, w.std.Struct, "memberOf Map<Type, Type>(...)");
        forEach(astStruct.memberOf(), [this, &compiledMemberOfs](CellI& membershipType, int i, bool& stop) {
            auto& compiledMembershipType = getCompiledTypeFromResolvedType(membershipType);
            compiledMemberOfs.add(compiledMembershipType, compiledMembershipType);
        });
        compiledStruct.set("memberOf", compiledMemberOfs);
    }

    compiledStructs().add(compiledStructName, compiledStruct);
}

void Compiler::compileEnum(Ast::Enum& astEnum)
{
    CellI& compiledStruct = astEnum["compiledStruct"];
    if (compiledStruct.missing("incomplete")) {
        return;
    }
    compiledStruct.erase("incomplete");
    compiledStruct.set(w.id.enum_, w.true_);

    // compile methods
    if (astEnum.has("methods")) {
        Map& compiledMethods = *new Map(w, w.std.Cell, w.std.ast.Function);
        forEach(astEnum.methods()[w.id.list], [this, &compiledMethods](CellI& astFunction, int i, bool& stop) {
            auto& compiledFunction = compileFunction(static_cast<Ast::Function&>(astFunction));
            compiledMethods.add(astFunction[w.id.name], compiledFunction);
        });
        compiledStruct.set("methods", compiledMethods);
    }

    // compile values
    if (astEnum.has("values")) {
        Map& compiledMembers = *new Map(w, w.std.Cell, w.std.Slot, "members Map<Cell, Slot>(...)");
        compiledMembers.add(w.id.tag, w.std.slot(w.id.tag, w.std.Cell));
        compiledStruct.set("slots", compiledMembers);
        forEach(astEnum.values()[w.id.list], [this, &astEnum, &compiledMembers, &compiledStruct](CellI& kvPair, int i, bool& stop) {
            CellI& valueKey  = kvPair[w.id.key];
            CellI& valueCell = kvPair[w.id.value];
            CellI& valueName = valueCell[w.id.name];
            if (&valueCell.__type__() == &w.std.ast.EnumValue) {
                auto& enumValue = static_cast<Ast::EnumValue&>(valueCell);
                auto& fullName  = getFullyQualifiedName(enumValue);
                CellI* compiledValuePtr = nullptr;
                if (m_earlyEnumValues.hasKey(fullName)) {
                    compiledValuePtr = &m_earlyEnumValues.getValue(fullName);
                } else {
                    compiledValuePtr = new Object(w, compiledStruct, fmt::format("{}::{}", astEnum.label(), enumValue.label()));
                }
                auto& compiledValue = *compiledValuePtr;
                compiledValue.set("tag", valueName);
                compiledVariables().add(fullName, compiledValue);

                if (valueCell.has(w.id.value)) {
                    auto& value         = enumValue[w.id.value];
                    auto& resolvedValue = resolveTypeInEnumValue(value);
                    auto& valueType     = resolvedValue.__type__();

                    compiledMembers.add(valueKey, w.std.slot(valueKey, valueType));
                    compiledValue.set(valueName, value[w.id.value]);
                } else {
                    compiledMembers.add(valueKey, w.std.slot(valueKey, compiledStruct));
                    compiledValue.set(valueName, w.id.emptyObject);
                }
            } else if (&valueCell.__type__() == &w.std.ast.TypedEnumValue) {
                auto& enumValue             = static_cast<Ast::TypedEnumValue&>(valueCell);
                auto& enumValueType         = valueCell["enumType"];
                auto& compiledEnumValueType = getCompiledTypeFromResolvedType(enumValueType);
                auto& fullName              = getFullyQualifiedName(enumValue);
                compiledMembers.add(valueKey, w.std.slot(valueKey, compiledEnumValueType));
            }
        });
    }
    compiledStructs().add(getFullyQualifiedName(astEnum), compiledStruct);
}

CellI& Compiler::compileFunction(Ast::Function& astFunction)
{
    cells::Object& compiledFunction = *new cells::Object(w, w.std.op.Function);
    compiledFunction.set(w.id.name, astFunction.get(w.id.name));

    // is this function a method?
    if (astFunction.has(w.id.structType)) {
        auto& currentStruct = static_cast<Ast::Struct&>(*m_currentStruct);
        auto& structType    = astFunction.get(w.id.structType);
        compiledFunction.set(w.id.objectType, structType["compiledStruct"]);
    }
    compileFunctionParams(astFunction, compiledFunction);

    compiledFunction.set(w.id.ast, astFunction);
    compiledFunction.set(w.id.op, compileFunctionAst(astFunction, astFunction.instructions(), compiledFunction));
    if (astFunction.has(w.id.static_)) {
        compiledFunction.set(w.id.static_, astFunction.get(w.id.static_));
    }

    return compiledFunction;
}

void Compiler::compileFunctionParams(Ast::Function& astFunction, cells::Object& compiledFunction)
{
    std::stringstream iss;
    std::stringstream oss;
    std::string structTypeStr;
    if (astFunction.has("parameters") || astFunction.has("structType")) {
        Map& parameters = *new Map(w, w.std.Cell, w.std.Slot);
        if (astFunction.has("structType")) {
            CellI& type = astFunction.get("structType");
            Object& var = *new Object(w, w.std.op.Var, "self");
            var.set("valueType", type);
            parameters.add(w.id.self, w.std.slot("self", type));
            structTypeStr = fmt::format("{}::", type.label());
        }
        if (astFunction.has(w.id.parameters)) {
            forEach(astFunction.parameters(), [this, &parameters, &iss](CellI& slot, int i, bool& stop) {
                if (i > 0) {
                    iss << ", ";
                }
                auto& key          = slot[w.id.key];
                auto& type         = slot[w.id.type];
                auto& compiledType = getCompiledTypeFromResolvedType(type);
                iss << "p_" << key.label() << ": " << compiledType.label();
                parameters.add(key, w.std.slot(key, compiledType));
            });
        }
        compiledFunction.set(w.id.parameters, parameters);
    }
    if (astFunction.has(w.id.returnType)) {
        auto& astReturnType      = astFunction.returnType();
        auto& compiledReturnType = getCompiledTypeFromResolvedType(astReturnType);
        oss << compiledReturnType.label();
        compiledFunction.set(w.id.returnType, compiledReturnType);
    }
    if (astFunction.has(w.id.returnType)) {
        compiledFunction.label(fmt::format("fn {}{}({}) -> {}", structTypeStr, astFunction.get(w.id.name).label(), iss.str(), oss.str()));
    } else {
        compiledFunction.label(fmt::format("fn {}{}({})", structTypeStr, astFunction.get(w.id.name).label(), iss.str()));
    }
}

std::string Compiler::shortFunctionName(Ast::Function& function)
{
    std::stringstream iss;
    std::stringstream oss;
    if (function.has(w.id.parameters) || function.has(w.id.structType)) {
        if (function.has(w.id.parameters)) {
            forEach(function.parameters(), [this, &iss](CellI& slot, int i, bool& stop) {
                if (i > 0) {
                    iss << ", ";
                }
                iss << "p_" << slot[w.id.key].label() << ": " << getCompiledTypeFromResolvedType(slot[w.id.type]).label();
            });
        }
    }
    if (function.has(w.id.returnType)) {
        oss << getCompiledTypeFromResolvedType(function.returnType()).label();
    }
    if (function.has(w.id.returnType)) {
        return fmt::format("fn {}({}) -> {}", function.get(w.id.name).label(), iss.str(), oss.str());
    } else {
        return fmt::format("fn {}({})", function.get(w.id.name).label(), iss.str());
    }
}

CellI& Compiler::compileFunctionAst(Ast::Function& astFunction, CellI& ast, cells::Object& function)
{
    auto compile = [this, &astFunction, &function](CellI& ast) -> CellI& { return compileFunctionAst(astFunction, ast, function); };
    const auto _ = [this](auto& cell) -> Ast::Cell& { return w._(cell); };

    if (&ast.__type__() == &w.std.ast.Block) {
        CellI& list      = ast[w.id.asts];
        CellI* prevBlock = nullptr;
        if (m_lastBlock) {
            prevBlock = m_lastBlock;
        }
        CellI* firstOpBlockNode   = nullptr;
        CellI* currentOpBlockNode = nullptr;
        Object& opBlock           = *new Object(w, w.std.op.Block);
        m_lastBlock = &opBlock;
        forEach(list, [this, &compile, &opBlock, &firstOpBlockNode, &currentOpBlockNode](CellI& ast, int, bool&) {
            CellI& newOpBlockNode = *new Object(w, w.std.op.Activate);
            newOpBlockNode.set(w.id.cell, compile(ast));
            newOpBlockNode.set(w.id.parent, opBlock);

            if (!firstOpBlockNode) {
                firstOpBlockNode = &newOpBlockNode;
            } else {
                (*currentOpBlockNode).set(w.id.next, newOpBlockNode);
            }
            currentOpBlockNode = &newOpBlockNode;
        });
        opBlock.set(w.id.ast, ast);
        if (firstOpBlockNode) {
            opBlock.set(w.id.ops, *firstOpBlockNode);
        }

        if (prevBlock) {
            m_lastBlock = prevBlock;
        } else {
            m_lastBlock = nullptr;
        }

        return opBlock;
    } else if (&ast.__type__() == &w.std.ast.Cell) {
        Object& constVar = *new Object(w, w.std.op.ConstVar);
        constVar.set(w.id.ast, ast);
        constVar.set(w.id.value, ast[w.id.value]);
        return constVar;
    } else if (&ast.__type__() == &w.std.ast.ResolvedType) {
        Object& constVar = *new Object(w, w.std.op.ConstVar);
        constVar.set(w.id.ast, ast);
        constVar.set(w.id.value, ast[w.id.compiled]);
        return constVar;
    } else if (&ast.__type__() == &w.std.ast.SelfFn) {
        Object& constVar = *new Object(w, w.std.op.ConstVar);
        constVar.set(w.id.ast, ast);
        constVar.set(w.id.value, function);
        return constVar;
    } else if (&ast.__type__() == &w.std.ast.Self) {
        CellI& retOp = compile(w.ast.get(_(function), _(w.id.stack)) / _(w.id.value) / _(w.id.input) / _(w.id.self));
        retOp.set(w.id.ast, ast);
        retOp.label("self");
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Continue) {
        if (!m_lastBlock) {
            throw "No statement to break!";
        }
        CellI& lastBlock = *m_lastBlock;
        CellI& retOp     = compile(w.ast.set(_(lastBlock), _(w.id.status), _(w.id.continue_)));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Break) {
        if (!m_lastBlock) {
            throw "No statement to break!";
        }
        CellI& lastBlock = *m_lastBlock;
        CellI& retOp     = compile(w.ast.set(_(lastBlock), _(w.id.status), _(w.id.break_)));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Throw) {
        if (!m_lastBlock) {
            throw "No statement to break!";
        }
        CellI& lastBlock = *m_lastBlock;
        CellI& retOp     = compile(w.ast.set(_(lastBlock), _(w.id.status), _(w.id.throw_)));
        if (ast.has("value")) {
            retOp.set(w.id.result, compile(w.ast.set(_(lastBlock), _(w.id.value), static_cast<Ast::Base&>(ast[w.id.value]))));
        }
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Parameter) {
        CellI& retOp = compile(w.ast.get(_(function), _(w.id.stack)) / _(w.id.value) / _(w.id.input) / _(ast[w.id.key]));
        retOp.set(w.id.ast, ast);
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Member) {
        CellI& member = compile(w.ast.get(w.ast.self(), w.ast.cell(ast[w.id.key])));
        member.set(w.id.ast, ast);
        return member;
    } else if (&ast.__type__() == &w.std.ast.Return) {
        Object& retOp = *new Object(w, w.std.op.Return, "op.return");
        retOp.set(w.id.ast, ast);
        if (ast.has(w.id.value)) {
            retOp.set(w.id.result, compile(w.ast.set(_(function), _(w.id.value), static_cast<Ast::Base&>(ast[w.id.value]))));
        }
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Var) {
        Index* localVarsIndexPtr = nullptr;
        if (function.missing(w.id.localVars)) {
            localVarsIndexPtr = new Index(w, fmt::format("LocalVarsIndex of {}", function.label()));
            function.set(w.id.localVars, *localVarsIndexPtr);
        } else {
            localVarsIndexPtr = &static_cast<Index&>(function[w.id.localVars]);
        }

        Index &localVarsIndex = *localVarsIndexPtr;
        if (!localVarsIndex.has(ast[w.id.name])) {
            localVarsIndex.insert(ast[w.id.name], w.std.slot(ast[w.id.name], w.std.op.Var));
        }

        CellI& retOp = compile(w.ast.get(_(function), _(w.id.stack)) / _(w.id.value) / _(w.id.localVars) / _(ast[w.id.name]));
        retOp.set(w.id.ast, ast);
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Delete) {
        Object& retOp = *new Object(w, w.std.op.Delete);
        retOp.set(w.id.ast, ast);
        retOp.set(w.id.input, compile(ast[w.id.cell]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Set) {
        Object& retOp = *new Object(w, w.std.op.Set);
        retOp.set(w.id.ast, ast);
        retOp.set(w.id.cell, compile(ast[w.id.cell]));
        retOp.set(w.id.key, compile(ast[w.id.key]));
        retOp.set(w.id.value, compile(ast[w.id.value]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Erase) {
        Object& retOp = *new Object(w, w.std.op.Erase);
        retOp.set(w.id.ast, ast);
        retOp.set(w.id.cell, compile(ast[w.id.cell]));
        retOp.set(w.id.key, compile(ast[w.id.key]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.If) {
        Object& retOp = *new Object(w, w.std.op.If);
        retOp.set(w.id.ast, ast);
        retOp.set(w.id.condition, compile(ast[w.id.condition]));
        retOp.set(w.id.then, compile(ast[w.id.then]));
        if (ast.has(w.id.else_)) {
            retOp.set(w.id.else_, compile(ast[w.id.else_]));
            return retOp;
        } else {
            return retOp;
        }
    } else if (&ast.__type__() == &w.std.ast.Match) {
        auto& enumObj        = static_cast<Ast::Base&>(ast["enum"]);
        auto& caseList       = ast["cases"][w.id.list];
        auto& astCases       = *new cells::List(w, w.std.ast.Base);
        Ast::Block& astBlock = *new Ast::Block(w, astCases);

        forEach(caseList, [this, &compile, &ast, &function, &enumObj, &astCases](CellI& kvPair, int, bool&) {
            auto& kind    = kvPair[w.id.key];
            auto& op      = kvPair[w.id.value];
            auto& oneCase = w.ast.if_(w.ast.same(w.ast.get(enumObj, "tag"), w._(kind))).then_(static_cast<Ast::Base&>(op));
            astCases.add(oneCase);
        });
        auto& retOp = compile(astBlock);
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Do) {
        Object& retOp = *new Object(w, w.std.op.Do);
        retOp.set(w.id.ast, ast);
        retOp.set(w.id.condition, compile(ast[w.id.condition]));
        retOp.set(w.id.statement, compile(ast[w.id.statement]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.While) {
        Object& retOp = *new Object(w, w.std.op.While);
        retOp.set(w.id.ast, ast);
        retOp.set(w.id.condition, compile(ast[w.id.condition]));
        retOp.set(w.id.statement, compile(ast[w.id.statement]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.For) {
        auto& valueVar  = static_cast<Ast::Var&>(ast[w.id.variable]);
        auto& statement = static_cast<Ast::Base&>(ast[w.id.statement]);

        Ast::Block& forLoopBlock =
            w.ast.block(
                valueVar = w.ast.call(*w.ast.var("iterator"), "getCurrentNodeValue"));

        Ast::Base& forLoopBlockEnd =
            w.ast.if_(w.ast.same(w.ast.call(*w.ast.var("iterator"), "hasNextNode"), w._(w.true_)))
                            .then_(w.ast.call(*w.ast.var("iterator"), "goToNextNode"))
                            .else_(w.ast.break_());

        forLoopBlock.mergeFrom(statement);
        forLoopBlock.mergeFrom(forLoopBlockEnd);

        Ast::Block& retAst = w.ast.block(
            w.ast.var("iterator") = w.ast.call(ast[w.id.container], "iterator"),
            w.ast.if_(w.ast.same(w.ast.call(*w.ast.var("iterator"), "isContainerEmpty"), w._(w.false_)))
                .then_(w.ast.block(
                    w.ast.call(*w.ast.var("iterator"), "goToFirstNode"),
                    w.ast.do_(forLoopBlock)
                        .while_(w._(w.true_))))
        );
        CellI& retOp = compile(retAst);
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.New) {
        CellI* firstOpBlockNode = nullptr;
        auto& astObjectType     = ast[w.id.objectType];

        Object& block = *new Object(w, w.std.op.Block);
        block.set(w.id.ast, ast);

        Object& opSet = *new Object(w, w.std.op.Set, "New { block.value = new objectType(); }");
        opSet.set(w.id.ast, ast);
        opSet.set(w.id.cell, compile(w.ast.cell(block)));
        opSet.set(w.id.key, compile(w.ast.cell(w.id.value)));

        Object& opNew = *new Object(w, w.std.op.New);
        opNew.set(w.id.ast, ast);
        opNew.set(w.id.objectType, compile(astObjectType));
        opSet.set(w.id.value, opNew);

        CellI& newOpBlockNode = *new Object(w, w.std.op.Activate);
        newOpBlockNode.set(w.id.cell, opSet);
        newOpBlockNode.set(w.id.parent, block);
        firstOpBlockNode = &newOpBlockNode;

        if (ast.has(w.id.constructor)) {
            Object& callAst = *new Object(w, w.std.ast.Call);
            callAst.set(w.id.cell, w.ast.get(w.ast.cell(block), w.ast.cell(w.id.value)));
            callAst.set(w.id.method, ast[w.id.constructor]);
            if (ast.has(w.id.parameters)) {
                callAst.set(w.id.parameters, ast[w.id.parameters]);
            }
            CellI& callConstructor = compile(callAst);
            callConstructor.label("New { call constructor; }");
            CellI& newOpBlockNode = *new Object(w, w.std.op.Activate);
            newOpBlockNode.set(w.id.cell, callConstructor);
            newOpBlockNode.set(w.id.parent, block);
            (*firstOpBlockNode).set(w.id.next, newOpBlockNode);
        }
        block.set(w.id.ops, *firstOpBlockNode);
        return block;
    } else if (&ast.__type__() == &w.std.ast.Call || &ast.__type__() == &w.std.ast.StaticCall) {
        Ast::Base& astCell   = static_cast<Ast::Base&>(ast[w.id.cell]);
        Ast::Base& astMethod = static_cast<Ast::Base&>(ast[w.id.method]);
        auto& astMethodId    = astMethod[w.id.value];
#if 1
        bool checked = false;
        // New<T>.constructor(...)
        std::stringstream ss;
        if (&astCell.__type__() == &w.std.ast.Get && &astCell[w.id.cell].__type__() == &w.std.ast.Cell && &astCell[w.id.cell][w.id.value].__type__() == &w.std.op.Block) {
            auto& opBlock    = astCell[w.id.cell][w.id.value];
            auto& opBlockAst = opBlock[w.id.ast];
            if (&opBlockAst.__type__() == &w.std.ast.New) {
                auto& astObjectTypeStruct = opBlockAst[w.id.objectType];
                checkMethodCall(astObjectTypeStruct, astMethodId);
                checked = true;
            }
            // var_xxx.call(...)
        } else if (&astCell.__type__() == &w.std.ast.Get && &astCell[w.id.cell].__type__() == &w.std.ast.Var) {
            auto& astVar = astCell[w.id.cell];
            ss << "In " << astVar.label() << " ";
            // m_xxx.method(...)
        } else if (&astCell.__type__() == &w.std.ast.Member) {
            auto& astMemberId    = astCell[w.id.key];
            auto& astMembersType = function[w.id.ast][w.id.structType];
            if (&astMemberId == &w.id.__type__) {
                // std::cout << "DDDD " << astMembersType.label();
                auto& stdScope = w.globalScope.getItem<Ast::Scope>("std");
                auto& type     = stdScope.getItem<Ast::Struct>("Struct");
                checkMethodCall(type, astMethodId);
                checked = true;
            } else {
                auto& astMembers = static_cast<Map&>(function[w.id.ast][w.id.structType][w.id.members]);
                if (astMembers.hasKey(astMemberId)) {
                    auto& slot = static_cast<Ast::Slot&>(astMembers.getValue(astMemberId));
                    auto& type = slot[w.id.type];
                    checkMethodCall(type, astMethodId);
                    checked = true;
                } else {
                    throw "Unknown member name!";
                }
            }
        } else if (&astCell.__type__() == &w.std.ast.Self) {
            auto& astSelfType = function[w.id.ast][w.id.structType];
            checkMethodCall(astSelfType, astMethodId);
            checked = true;
        } else if (&astCell.__type__() == &w.std.ast.Parameter) {
            auto& parameterRole                 = astCell[w.id.key];
            auto& astFunctionParameters         = function[w.id.ast][w.id.parameters];
            Ast::Slot* astFunctionParameterSlot = nullptr;
            forEach(astFunctionParameters, [this, &parameterRole, &astFunctionParameterSlot](CellI& slot, int i, bool& stop) {
                CellI& key  = slot[w.id.key];
                CellI& type = slot[w.id.type];
                if (&key == &parameterRole) {
                    astFunctionParameterSlot = &static_cast<Ast::Slot&>(slot);
                    stop                     = true;
                    return;
                }
            });
            if (astFunctionParameterSlot) {
                Ast::Slot& slot        = *astFunctionParameterSlot;
                auto& astParameterType = slot[w.id.type];
                checkMethodCall(astParameterType, astMethodId);
                checked = true;
            } else {
                throw "Unknown parameter name!";
            }
        }
        if (!checked) {
            WARN(compileStruct, "{} Unchecked method call {} in {}", ss.str(), astMethodId.label(), function.label());
        }
#endif
        Object& retOp = *new Object(w, w.std.op.Call);
        retOp.set(w.id.ast, ast);
        retOp.set(w.id.cell, compile(ast[w.id.cell]));
        retOp.set(w.id.method, compile(ast[w.id.method]));
        retOp.set(w.id.stack, compile(w.ast.get(_(function), _(w.id.stack))));
        if (ast.has(w.id.parameters)) {
            List& parameters = *new List(w, w.std.Slot);
            forEach(ast[w.id.parameters], [this, &parameters, &compile, &_](CellI& param, int, bool&) {
                CellI& slot = *new Object(w, w.std.Slot);
                slot.set(w.id.key, param[w.id.key]);
                slot.set(w.id.type, compile(param[w.id.type]));
                parameters.add(slot);
            });
            retOp.set(w.id.parameters, parameters);
        }

        return retOp;
    } else if (&ast.__type__() == &w.std.ast.And) {
        Object& retOp = *new Object(w, w.std.op.And);
        retOp.set(w.id.ast, ast);
        retOp.set(w.id.lhs, compile(ast[w.id.lhs]));
        retOp.set(w.id.rhs, compile(ast[w.id.rhs]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Or) {
        Object& retOp = *new Object(w, w.std.op.Or);
        retOp.set(w.id.ast, ast);
        retOp.set(w.id.lhs, compile(ast[w.id.lhs]));
        retOp.set(w.id.rhs, compile(ast[w.id.rhs]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Not) {
        Object& retOp = *new Object(w, w.std.op.Not);
        retOp.set(w.id.ast, ast);
        retOp.set(w.id.input, compile(ast[w.id.input]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Add) {
        Object& retOp = *new Object(w, w.std.op.Add);
        retOp.set(w.id.ast, ast);
        retOp.set(w.id.lhs, compile(ast[w.id.lhs]));
        retOp.set(w.id.rhs, compile(ast[w.id.rhs]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Subtract) {
        Object& retOp = *new Object(w, w.std.op.Subtract);
        retOp.set(w.id.ast, ast);
        retOp.set(w.id.lhs, compile(ast[w.id.lhs]));
        retOp.set(w.id.rhs, compile(ast[w.id.rhs]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Multiply) {
        Object& retOp = *new Object(w, w.std.op.Multiply);
        retOp.set(w.id.ast, ast);
        retOp.set(w.id.lhs, compile(ast[w.id.lhs]));
        retOp.set(w.id.rhs, compile(ast[w.id.rhs]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Divide) {
        Object& retOp = *new Object(w, w.std.op.Divide);
        retOp.set(w.id.ast, ast);
        retOp.set(w.id.lhs, compile(ast[w.id.lhs]));
        retOp.set(w.id.rhs, compile(ast[w.id.rhs]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.LessThan) {
        Object& retOp = *new Object(w, w.std.op.LessThan);
        retOp.set(w.id.ast, ast);
        retOp.set(w.id.lhs, compile(ast[w.id.lhs]));
        retOp.set(w.id.rhs, compile(ast[w.id.rhs]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.LessThanOrEqual) {
        Object& retOp = *new Object(w, w.std.op.LessThanOrEqual);
        retOp.set(w.id.ast, ast);
        retOp.set(w.id.lhs, compile(ast[w.id.lhs]));
        retOp.set(w.id.rhs, compile(ast[w.id.rhs]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.GreaterThan) {
        Object& retOp = *new Object(w, w.std.op.GreaterThan);
        retOp.set(w.id.ast, ast);
        retOp.set(w.id.lhs, compile(ast[w.id.lhs]));
        retOp.set(w.id.rhs, compile(ast[w.id.rhs]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.GreaterThanOrEqual) {
        Object& retOp = *new Object(w, w.std.op.GreaterThanOrEqual);
        retOp.set(w.id.ast, ast);
        retOp.set(w.id.lhs, compile(ast[w.id.lhs]));
        retOp.set(w.id.rhs, compile(ast[w.id.rhs]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Same) {
        Object& retOp = *new Object(w, w.std.op.Same);
        retOp.set(w.id.ast, ast);
        retOp.set(w.id.lhs, compile(ast[w.id.lhs]));
        retOp.set(w.id.rhs, compile(ast[w.id.rhs]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.NotSame) {
        Object& retOp = *new Object(w, w.std.op.NotSame);
        retOp.set(w.id.ast, ast);
        retOp.set(w.id.lhs, compile(ast[w.id.lhs]));
        retOp.set(w.id.rhs, compile(ast[w.id.rhs]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Equal) {
        Object& retOp = *new Object(w, w.std.op.Equal);
        retOp.set(w.id.ast, ast);
        retOp.set(w.id.lhs, compile(ast[w.id.lhs]));
        retOp.set(w.id.rhs, compile(ast[w.id.rhs]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.NotEqual) {
        Object& retOp = *new Object(w, w.std.op.NotEqual);
        retOp.set(w.id.ast, ast);
        retOp.set(w.id.lhs, compile(ast[w.id.lhs]));
        retOp.set(w.id.rhs, compile(ast[w.id.rhs]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Has) {
        Object& retOp = *new Object(w, w.std.op.Has);
        retOp.set(w.id.ast, ast);
        retOp.set(w.id.cell, compile(ast[w.id.cell]));
        retOp.set(w.id.key, compile(ast[w.id.key]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Missing) {
        Object& retOp = *new Object(w, w.std.op.Missing);
        retOp.set(w.id.ast, ast);
        retOp.set(w.id.cell, compile(ast[w.id.cell]));
        retOp.set(w.id.key, compile(ast[w.id.key]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Get) {
        Object& retOp = *new Object(w, w.std.op.Get);
        retOp.set(w.id.ast, ast);
        retOp.set(w.id.cell, compile(ast[w.id.cell]));
        retOp.set(w.id.key, compile(ast[w.id.key]));
        return retOp;
    }

    throw "Unknown function AST!";
}

void Compiler::checkMethodCall(CellI& astType, CellI& astMethodId)
{
    CellI* typePtr  = nullptr;
    Map* methodsPtr = nullptr;
    if (astType.isA(w.std.ast.Cell)) {
        auto& cellValue = astType[w.id.value];
        typePtr         = &cellValue;
        methodsPtr      = &static_cast<Map&>(cellValue[w.id.methods]);
    } else if (astType.isA(w.std.ast.Struct)) {
        typePtr    = &astType;
        methodsPtr = &static_cast<Map&>(astType[w.id.methods]);
    } else if (astType.isA(w.std.ast.ResolvedType)) {
        typePtr    = &astType[w.id.compiled];
        methodsPtr = &static_cast<Map&>(astType[w.id.ast][w.id.methods]);
    } else {
        throw "Unexpected AST type";
    }
    CellI& type = *typePtr;
    if (&type.__type__() != &w.std.ast.Struct && &type.__type__() != &w.std.Struct && type.label() != "Struct") {
        throw "Resolved type must be a type!";
    }
    auto& methods = *methodsPtr;
    if (!methods.hasKey(astMethodId)) {
        std::cerr << fmt::format("Method '{}' doesn't exist in type {}", astMethodId.label(), type.label()) << std::endl;
        throw "Method doesn't exist in type!";
    }
}

void Compiler::processDescriptionsInScope(Ast::Scope& scope)
{
    if (scope.has("structs")) {
        forEach(scope.items<Ast::Struct>()[w.id.list], [this](CellI& astStruct, int i, bool& stop) {
            auto& tool = static_cast<Ast::Struct&>(astStruct[w.id.value]);
            if (tool.has(w.id.description) && tool[w.id.description].has(w.id.asts)) {
                auto& compiledAstStruct = compiledStructs().getValue(getFullyQualifiedName(tool));
                m_toolFinder.add(tool, compiledAstStruct);
            }
        });
    }
    if (scope.has("scopes")) {
        forEach(scope.items<Ast::Scope>()[w.id.list], [this, &scope](CellI& scopeItem, int i, bool& stop) {
            Ast::Scope& nextScope = static_cast<Ast::Scope&>(scopeItem[w.id.value]);
            processDescriptionsInScope(nextScope);
            m_scope = &scope;
        });
    }
}

} // namespace cells
} // namespace infocell