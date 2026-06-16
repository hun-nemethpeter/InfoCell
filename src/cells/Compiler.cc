#include "Compiler.h"

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include "util/Log.h"

#include <sstream>

namespace infocell {
namespace cells {

Compiler::Compiler(World& w) :
    w(w),
    m_toolFinder(*new ToolFinder(w)),
    m_earlyStructs(w, w.std.Cell, w.std.Cell, "earlyStructs"),
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
  - where templated types is used, for example tt_("List", ids.valueType, _(type.Slot))
    it must be resolved to a StructName, with id L,i,s,t,ids.valueType,type.Slot
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

    m_libraryPtr->set(w.ids.scope, scope);
    m_libraryPtr->set(w.ids.resolvedScope, resolvedScope);

    return *m_libraryPtr;
}

CellI& Compiler::reigisterStructBeforeCompilation(CellI& structAst)
{
    CellI* structIdPtr = nullptr;
    if (&structAst.__type__() == &w.std.ast.TemplatedType) {
        List& idCell = *new List(w, w.std.Cell);
        structIdPtr  = &idCell;
        std::stringstream ss;
        Visitor::visitList(structAst[w.ids.scopes], [this, &idCell, &ss](CellI& scope, int i, bool&) {
            Visitor::visitList(scope, [this, &idCell, &ss](CellI& character, int, bool&) {
                idCell.add(character);
                ss << character.label();
            });
            idCell.add(w.pools.chars.get(':'));
            idCell.add(w.pools.chars.get(':'));
            ss << "::";
        });
        Visitor::visitList(structAst[w.ids.id], [this, &idCell, &ss](CellI& character, int, bool&) {
            idCell.add(character);
            ss << character.label();
        });

        ss << "<";
        Visitor::visitList(structAst[w.ids.parameters], [this, &idCell, &ss](CellI& slot, int i, bool&) {
            if (i != 0) {
                ss << ", ";
            }
            CellI& key              = slot[w.ids.key];
            CellI& type             = slot[w.ids.type];
            CellI& compiledSlotType = reigisterStructBeforeCompilation(type);

            idCell.add(key);
            idCell.add(compiledSlotType);
            ss << fmt::format("{}={}", key.label(), compiledSlotType.label());
        });
        ss << ">";
        idCell.label(ss.str());
    } else if (&structAst.__type__() == &w.std.ast.StructName) {
        structIdPtr = &structAst[w.ids.name];
    } else if (&structAst.__type__() == &w.std.ast.Cell) {
        return structAst[w.ids.value];
    } else {
        throw "Unsupported type!";
    }
    CellI& structId = *structIdPtr;
    if (m_earlyStructs.hasKey(structId)) {
        return m_earlyStructs.getValue(structId);
    } else {
        auto& unresolvedStruct = *new Object(w, w.std.Struct, fmt::format("{}", structId.label()));
        unresolvedStruct.set("incomplete", w.boolean.true_);

        m_earlyStructs.add(structId, w.std.slot(structAst, unresolvedStruct));
        return unresolvedStruct;
    }
}

void Compiler::registerBuiltInStruct(const std::string& fullName, CellI& compiledStruct)
{
    std::vector<std::string> sliced;
    splitNamespacedString(sliced, fullName);

    if (sliced.empty()) {
        throw "Invalid struct ID!";
    }
    std::stringstream ss;
    List& idCell             = *new List(w, w.std.Cell);
    const auto& structName   = sliced.back();
    Ast::Scope* currentScope = &w.globalScope;
    if (sliced.size() > 1) {
        for (int i = 0; i < sliced.size() - 1; ++i) {
            const auto& scopeName = sliced[i];
            currentScope          = &currentScope->getItem<Ast::Scope>(scopeName);
            Visitor::visitList((*currentScope)["name"], [this, &idCell, &ss](CellI& character, int, bool&) {
                idCell.add(character);
                ss << character.label();
            });
            idCell.add(w.pools.chars.get(':'));
            idCell.add(w.pools.chars.get(':'));
            ss << "::";
        }
    }
    Ast::Struct& structAst = currentScope->getItem<Ast::Struct>(structName);
    Visitor::visitList(structAst[w.ids.name], [this, &idCell, &ss](CellI& character, int, bool&) {
        idCell.add(character);
        ss << character.label();
    });
    idCell.label(ss.str());
    m_earlyStructs.add(idCell, w.std.slot(w.__type__(fullName), compiledStruct));
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
    Visitor::visitList(m_earlyStructs[w.ids.list], [this](CellI& earlyStructKV, int i, bool& stop) {
        auto& structId       = earlyStructKV[w.ids.key];
        auto& structRefAst   = earlyStructKV[w.ids.value][w.ids.key];
        auto& compiledStruct = earlyStructKV[w.ids.value][w.ids.type];

        TRACE(compileStruct, "early struct: {}", earlyStructKV[w.ids.key].label());

        auto& structReference = *new Object(w, w.std.StructReference);
        structReference.set(w.ids.value, compiledStruct);
        structReference.set(w.ids.id, structId);

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
    auto& stdScope         = scope.getItem<Ast::Scope>("std");
    auto& resolvedStdScope = resolvedScope.getItem<Ast::Scope>("std");

    Visitor::visitList(m_earlyStructs[w.ids.list], [this, &stdScope, &resolvedStdScope](CellI& earlyStructKV, int i, bool& stop) {
        auto& structId       = earlyStructKV[w.ids.key];
        auto& structRefAst   = earlyStructKV[w.ids.value][w.ids.key];
        auto& compiledStruct = earlyStructKV[w.ids.value][w.ids.type];

        TRACE(compileStruct, "resolve early struct: {}", earlyStructKV[w.ids.key].label());

        if (&structRefAst.__type__() == &w.std.ast.TemplatedType) {
            if (m_unknownInstances.hasKey(structId)) {
                CellI& structReference = m_unknownInstances.getValue(structId);
                structReference.set("scope", stdScope);
                structReference.set("resolvedScope", resolvedStdScope);
                structReference.set("templateId", structRefAst["id"]);
                structReference.set(w.ids.templateParams, structRefAst[w.ids.parameters]);
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
    scope.set(w.ids.resolvedScope, resolvedScope);
    m_scope = &scope;
    m_resolvedScope = &resolvedScope;

    if (scope.has("functions")) {
        m_currentStruct = nullptr;
        Visitor::visitList(scope.items<Ast::Function>()[w.ids.list], [this, &resolvedScope](CellI& origAstFunctionCell, int i, bool& stop) {
            Ast::Function& origAstFunction     = static_cast<Ast::Function&>(origAstFunctionCell[w.ids.value]);
            Ast::Function& resolvedAstFunction = resolveTypesInFunction(origAstFunction);
            resolvedScope.add<Ast::Function>(resolvedAstFunction);
        });
    }
    if (scope.has("structs")) {
        Visitor::visitList(scope.items<Ast::Struct>()[w.ids.list], [this, &resolvedScope](CellI& origAstStructCell, int i, bool& stop) {
            Ast::Struct& origAstStruct     = static_cast<Ast::Struct&>(origAstStructCell[w.ids.value]);
            Ast::Struct& resolvedAstStruct = resolveTypesInStruct(origAstStruct);
            resolvedScope.add<Ast::Struct>(resolvedAstStruct);
        });
    }
    if (scope.has("enums")) {
        Visitor::visitList(scope.items<Ast::Enum>()[w.ids.list], [this, &resolvedScope](CellI& origAstEnumCell, int i, bool& stop) {
            Ast::Enum& origAstEnum     = static_cast<Ast::Enum&>(origAstEnumCell[w.ids.value]);
            Ast::Enum& resolvedAstEnum = resolveTypesInEnum(origAstEnum);
            resolvedScope.add<Ast::Enum>(resolvedAstEnum);
        });
    }
    if (scope.has("variables")) {
        Visitor::visitList(scope.items<Ast::Var>()[w.ids.list], [this, &resolvedScope](CellI& origAstVarCell, int i, bool& stop) {
            Ast::Var& origAstVar = static_cast<Ast::Var&>(origAstVarCell[w.ids.value]);
            resolvedScope.add<Ast::Var>(origAstVar);
        });
    }
    if (scope.has("scopes")) {
        Visitor::visitList(scope.items<Ast::Scope>()[w.ids.list], [this, &scope, &resolvedScope](CellI& origAstScopeCell, int i, bool& stop) {
            Ast::Scope& origAstScope     = static_cast<Ast::Scope&>(origAstScopeCell[w.ids.value]);
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
        Visitor::visitList(function.parameters(), [this, &ret, &ss](CellI& param, int i, bool& stop) {
            CellI& paramId           = param[w.ids.key];
            CellI& paramType         = param[w.ids.type];
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
        CellI& objectType                = ast[w.ids.objectType];
        Ast::Base* resolvedObjectTypePtr = nullptr;
        if (&objectType.__type__() == &w.std.ast.Member) {
            resolvedObjectTypePtr = &static_cast<Ast::Base&>(objectType);
        } else {
            resolvedObjectTypePtr = &resolveType(objectType);
        }
        Ast::Base& resolvedObjectType = *resolvedObjectTypePtr;

        if (ast.has("constructor")) {
            auto& constructor = ast[w.ids.constructor];
            Ast::Base& ret    = w.ast.new_(resolvedObjectType, static_cast<Ast::Base&>(ast[w.ids.constructor]));

            if (ast.has("parameters")) {
                auto& newParameters = *new cells::List(w, w.std.ast.Slot);
                Visitor::visitList(ast[w.ids.parameters], [this, &newParameters, &resolveNode](CellI& slot, int, bool&) {
                    newParameters.add(w.ast.slot(slot[w.ids.key], resolveNode(slot[w.ids.type])));
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
            retPtr = &w.ast.call(resolveNode(ast[w.ids.cell]), ast[w.ids.method]);
        } else {
            retPtr = &w.ast.scall(resolveType(ast[w.ids.cell]), ast[w.ids.method]);
        }
        Ast::Base& ret = *retPtr;
        if (ast.has("parameters")) {
            auto& newParameters = *new cells::List(w, w.std.ast.Slot);
            Visitor::visitList(ast[w.ids.parameters], [this, &newParameters, &resolveNode](CellI& slot, int, bool&) {
                newParameters.add(w.ast.slot(slot[w.ids.key], resolveNode(slot[w.ids.type])));
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
        Visitor::visitList(ast[w.ids.asts], [this, &instantiedAsts, &resolveNode](CellI& ast, int, bool&) {
            instantiedAsts.add(resolveNode(ast));
        });
        return *new Ast::Block(w, instantiedAsts);
    } else if (&ast.__type__() == &w.std.ast.Cell) {
        return w.ast.cell(ast[w.ids.value]);
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
        if (ast.has(w.ids.value)) {
            return w.ast.throw_(resolveNode(ast[w.ids.value]));
        } else {
            return w.ast.throw_();
        }
    } else if (&ast.__type__() == &w.std.ast.Parameter) {
        return w.ast.parameter(ast[w.ids.key]);
    } else if (&ast.__type__() == &w.std.ast.Var) {
        return w.ast.var(ast[w.ids.name]);
    } else if (&ast.__type__() == &w.std.ast.Delete) {
        return w.ast.delete_(resolveNode(ast[w.ids.cell]));
    } else if (&ast.__type__() == &w.std.ast.Set) {
        return w.ast.set(resolveNode(ast[w.ids.cell]), resolveNode(ast[w.ids.key]), resolveNode(ast[w.ids.value]));
    } else if (&ast.__type__() == &w.std.ast.Erase) {
        return w.ast.erase(resolveNode(ast[w.ids.cell]), resolveNode(ast[w.ids.key]));
    } else if (&ast.__type__() == &w.std.ast.If) {
        if (ast.has("else_")) {
            return w.ast.if_(resolveNode(ast[w.ids.condition]))
                .then_(resolveNode(ast[w.ids.then]))
                .else_(resolveNode(ast[w.ids.else_]));
        } else {
            return w.ast.if_(resolveNode(ast[w.ids.condition])).then_(resolveNode(ast[w.ids.then]));
        }
    } else if (&ast.__type__() == &w.std.ast.Match) {
        auto& ret = w.ast.match_(static_cast<Ast::Base&>(ast["enum"]));
        Visitor::visitList(ast["cases"][w.ids.list], [this, &resolveNode, &ret](CellI& kvpair, int, bool&) {
            auto& key = kvpair[w.ids.key];
            auto& op  = resolveNode(kvpair[w.ids.value]);
            ret.case_(key, op);
        });
        return ret;
    } else if (&ast.__type__() == &w.std.ast.Do) {
        return w.ast.do_(resolveNode(ast[w.ids.statement])).while_(resolveNode(ast[w.ids.condition]));
    } else if (&ast.__type__() == &w.std.ast.While) {
        return w.ast.while_(resolveNode(ast[w.ids.condition])).do_(resolveNode(ast[w.ids.statement]));
    } else if (&ast.__type__() == &w.std.ast.And) {
        return w.ast.and_(resolveNode(ast[w.ids.lhs]), resolveNode(ast[w.ids.rhs]));
    } else if (&ast.__type__() == &w.std.ast.Or) {
        return w.ast.or_(resolveNode(ast[w.ids.lhs]), resolveNode(ast[w.ids.rhs]));
    } else if (&ast.__type__() == &w.std.ast.Not) {
        return w.ast.not_(resolveNode(ast[w.ids.input]));
    } else if (&ast.__type__() == &w.std.ast.Add) {
        return w.ast.add(resolveNode(ast[w.ids.lhs]), resolveNode(ast[w.ids.rhs]));
    } else if (&ast.__type__() == &w.std.ast.Subtract) {
        return w.ast.subtract(resolveNode(ast[w.ids.lhs]), resolveNode(ast[w.ids.rhs]));
    } else if (&ast.__type__() == &w.std.ast.Multiply) {
        return w.ast.multiply(resolveNode(ast[w.ids.lhs]), resolveNode(ast[w.ids.rhs]));
    } else if (&ast.__type__() == &w.std.ast.Divide) {
        return w.ast.divide(resolveNode(ast[w.ids.lhs]), resolveNode(ast[w.ids.rhs]));
    } else if (&ast.__type__() == &w.std.ast.LessThan) {
        return w.ast.lessThan(resolveNode(ast[w.ids.lhs]), resolveNode(ast[w.ids.rhs]));
    } else if (&ast.__type__() == &w.std.ast.LessThanOrEqual) {
        return w.ast.lessThanOrEqual(resolveNode(ast[w.ids.lhs]), resolveNode(ast[w.ids.rhs]));
    } else if (&ast.__type__() == &w.std.ast.GreaterThan) {
        return w.ast.greaterThan(resolveNode(ast[w.ids.lhs]), resolveNode(ast[w.ids.rhs]));
    } else if (&ast.__type__() == &w.std.ast.GreaterThanOrEqual) {
        return w.ast.greaterThanOrEqual(resolveNode(ast[w.ids.lhs]), resolveNode(ast[w.ids.rhs]));
    } else if (&ast.__type__() == &w.std.ast.Same) {
        return w.ast.same(resolveNode(ast[w.ids.lhs]), resolveNode(ast[w.ids.rhs]));
    } else if (&ast.__type__() == &w.std.ast.NotSame) {
        return w.ast.notSame(resolveNode(ast[w.ids.lhs]), resolveNode(ast[w.ids.rhs]));
    } else if (&ast.__type__() == &w.std.ast.Equal) {
        return w.ast.equal(resolveNode(ast[w.ids.lhs]), resolveNode(ast[w.ids.rhs]));
    } else if (&ast.__type__() == &w.std.ast.NotEqual) {
        return w.ast.notEqual(resolveNode(ast[w.ids.lhs]), resolveNode(ast[w.ids.rhs]));
    } else if (&ast.__type__() == &w.std.ast.Has) {
        return w.ast.has(resolveNode(ast[w.ids.cell]), resolveNode(ast[w.ids.key]));
    } else if (&ast.__type__() == &w.std.ast.Missing) {
        return w.ast.missing(resolveNode(ast[w.ids.cell]), resolveNode(ast[w.ids.key]));
    } else if (&ast.__type__() == &w.std.ast.Get) {
        return w.ast.get(resolveNode(ast[w.ids.cell]), resolveNode(ast[w.ids.key]));
    } else if (&ast.__type__() == &w.std.ast.Member) {
        return w.ast.member(ast[w.ids.key]);
    } else if (&ast.__type__() == &w.std.ast.Return) {
        if (ast.has("value")) {
            return w.ast.return_(static_cast<Ast::Base&>(resolveNode(ast[w.ids.value])));
        }
        return w.ast.return_();
    }

    throw "Unknown AST to instantiate!";
}

Ast::Struct& Compiler::resolveTypesInStruct(Ast::Struct& astStruct)
{
    auto& structName = astStruct.get(w.ids.name);
    Ast::Struct& ret = *new Ast::Struct(w, structName);

    if (astStruct.has("instanceOf")) {
        ret.set("instanceOf", astStruct.get("instanceOf"));
        ret.set("templateParams", astStruct.get("templateParams"));
        ret.set("scope", astStruct.get("scope"));
    }

    auto& fullyQualifiedName = getFullyQualifiedName(astStruct);
    CellI* compiledStructPtr = nullptr;
    if (m_unknownStructs.hasKey(fullyQualifiedName)) {
        CellI& unknownStruct = m_unknownStructs.getValue(fullyQualifiedName);
        compiledStructPtr    = &unknownStruct["value"];
        m_unknownStructs.remove(fullyQualifiedName);
    } else {
        compiledStructPtr = new Object(w, w.std.Struct, fmt::format("{}", fullyQualifiedName.label()));
    }
    auto& compiledStruct = *compiledStructPtr;
    m_structs.add(fullyQualifiedName, compiledStruct);
    ret.set("compiledStruct", compiledStruct);
    ret.set("fullyQualifiedName", fullyQualifiedName);

    m_currentStruct = &ret;

    std::stringstream ss;
    std::vector<std::string> typeAliasesStrs;

    if (IS_LOG_ENABLED) {
    }

    // resolve sub types
    if (astStruct.has("typeAliases")) {
        Visitor::visitList(astStruct.typeAliases()[w.ids.list], [this, &ret, &typeAliasesStrs](CellI& typeAliasSlot, int i, bool& stop) {
            CellI& alias        = typeAliasSlot[w.ids.key];
            CellI& type         = typeAliasSlot[w.ids.type];
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
        Visitor::visitList(astStruct.memberOf(), [this, &ret, &ss](CellI& membershipType, int i, bool& stop) {
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
        Visitor::visitList(astStruct.methods()[w.ids.list], [this, &ret](CellI& origAstFunctionCell, int i, bool& stop) {
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
        CellI& membersList = astStruct.members()[w.ids.list];
        Visitor::visitList(membersList, [this, &astStruct, &ret](CellI& memberCell, int i, bool& stop) {
            CellI& memberId           = memberCell[w.ids.key];
            CellI& memberType         = memberCell[w.ids.type];
            CellI& resolvedMemberType = resolveType(memberType);
            ret.members(w.ast.slot(memberId, resolvedMemberType));
            TRACE(compileStruct, "    {}: {};", memberId.label(), getCompiledTypeFromResolvedType(resolvedMemberType).label());
        });
    }

    TRACE(compileStruct, "}");
    TRACE(compileStruct, "");

    return ret;
}

Ast::Enum& Compiler::resolveTypesInEnum(Ast::Enum& enum_)
{
    auto& enumName       = enum_.get("name");
    Ast::Enum& ret       = *new Ast::Enum(w, enumName);

    if (enum_.has("instanceOf")) {
        ret.set("instanceOf", enum_.get("instanceOf"));
        ret.set("templateParams", enum_.get("templateParams"));
        ret.set("scope", enum_.get("scope"));
    }

    auto& fullyQualifiedName = getFullyQualifiedName(enum_);
    CellI* resolvedStructPtr = nullptr;
    if (m_unknownStructs.hasKey(fullyQualifiedName)) {
        CellI& unknownStruct = m_unknownStructs.getValue(fullyQualifiedName);
        resolvedStructPtr    = &unknownStruct["value"];
        m_unknownStructs.remove(fullyQualifiedName);
    } else {
        resolvedStructPtr = new Object(w, w.std.Struct, fmt::format("{}", fullyQualifiedName.label()));
    }
    auto& resolvedStruct = *resolvedStructPtr;
    m_structs.add(fullyQualifiedName, resolvedStruct);

    m_currentStruct = &ret;

    TRACE(compileStruct, "enum {} {{", enum_.label());

    // resolve methods
    if (enum_.has("methods")) {
        Visitor::visitList(enum_.methods()[w.ids.list], [this, &ret](CellI& origAstFunctionCell, int i, bool& stop) {
            auto& origAstFunction     = static_cast<Ast::Function&>(origAstFunctionCell);
            auto& resolvedAstFunction = resolveTypesInFunction(origAstFunction);
            ret.addMethod(resolvedAstFunction);
            TRACE(compileStruct, "    {};", shortFunctionName(resolvedAstFunction));
        });
    }

    // resolve values
    if (enum_.has("values")) {
        CellI& valuesList = enum_.values()[w.ids.list];
        Visitor::visitList(valuesList, [this, &ret](CellI& kvPair, int i, bool& stop) {
            CellI& valueCell = kvPair[w.ids.value];
            CellI& valueName = valueCell[w.ids.name];
            std::stringstream ss;
            if (valueCell.has("enumType")) {
                CellI& valueType                    = valueCell["enumType"];
                CellI& resolvedValueType            = resolveType(valueType);
                Ast::TypedEnumValue& typedEnumValue = *new Ast::TypedEnumValue(w, valueName, resolvedValueType);
                if (valueCell.has(w.ids.value)) {
                    typedEnumValue.set(w.ids.value, valueCell[w.ids.value]);
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
                if (valueCell.has(w.ids.value)) {
                    ss << fmt::format(" = {}", resolveTypeInEnumValue(valueCell[w.ids.value]).label());
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
        return ast[w.ids.value];
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
    if (base.has(w.ids.scope)) {
        Ast::Scope& scope          = static_cast<Ast::Scope&>(base.get(w.ids.scope));
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
    Visitor::visitList(scopeFullyQualifiedName, [this, &fullyQualifiedName](CellI& character, int i, bool& stop) {
        fullyQualifiedName.add(character);
    });
    if (!fullyQualifiedName.empty()) {
        fullyQualifiedName.add(w.pools.chars.get(':'));
        fullyQualifiedName.add(w.pools.chars.get(':'));
    }
    if (!isEmptyName) {
        auto& name = base.get(w.ids.name);
        std::cout << "";
        Visitor::visitList(name, [this, &fullyQualifiedName](CellI& character, int i, bool& stop) {
            fullyQualifiedName.add(character);
        });
    }
    std::stringstream ss;
    int templateParamPrintModeFromCharIndex = fullyQualifiedName.size();
    if (base.has("instanceOf")) {
        int paramsLength                    = static_cast<List&>(base.get(w.ids.templateParams)).size();
        templateParamPrintModeFromCharIndex = fullyQualifiedName.size() - paramsLength * 2;
    }
    Visitor::visitList(fullyQualifiedName, [this, &fullyQualifiedName, &ss, &templateParamPrintModeFromCharIndex](CellI& character, int i, bool& stop) {
        if (i == templateParamPrintModeFromCharIndex) {
            stop = true;
            return;
        }
        ss << character.label();
    });
    if (base.has("instanceOf")) {
        ss << "<";
        Visitor::visitList(base.get(w.ids.templateParams), [this, &ss](CellI& slot, int i, bool& stop) {
            CellI& key  = slot[w.ids.key];
            CellI& type = slot[w.ids.type];
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
    while (childScopePtr->has(w.ids.scope)) {
        childScopePtr = &static_cast<Ast::Scope&>(childScopePtr->get(w.ids.scope));
        if (childScopePtr->has(w.ids.scope)) {
            scopes.push_front(childScopePtr);
        }
    }

    for (Ast::Scope* currentScopePtr : scopes) {
        Ast::Scope& currentScope = *currentScopePtr;
        CellI& scopeName         = currentScope[w.ids.name];
        Visitor::visitList(scopeName, [this, &idCell, &ss](CellI& character, int, bool&) {
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
    Visitor::visitList(name, [this, &idCell, &ss, &nameLength](CellI& character, int i, bool&) {
        if (i >= nameLength) {
            return;
        }
        idCell.add(character);
        ss << character.label();
    });
    ss << "<";
    Visitor::visitList(templateParams, [this, &idCell, &ss](CellI& slot, int i, bool&) {
        CellI& key          = slot[w.ids.key];
        CellI& type         = slot[w.ids.type];
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
    Visitor::visitList(id, [this, &idCell, &ss](CellI& character, int, bool&) {
        idCell.add(character);
        ss << character.label();
    });

    ss << "<";
    Visitor::visitList(parameters, [this, &idCell, &ss, &resolvedParams](CellI& slot, int i, bool&) {
        if (i != 0) {
            ss << ", ";
        }
        CellI& key              = slot[w.ids.key];
        CellI& type             = slot[w.ids.type];
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
        auto& cellValue = typeAst[w.ids.value];
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
        auto& name               = resolveAstStruct[w.ids.name];
        auto& fullyQualifiedName = getFullyQualifiedName(resolveAstStruct);

        if (compiledStructs().hasKey(fullyQualifiedName)) {
            return w.ast.cell(compiledStructs().getValue(fullyQualifiedName));
        }

        auto& resolveCompiledStruct = resolveStructName(name, fullyQualifiedName);
        auto& reslvedTypeNode       = createResolvedType(resolveAstStruct, resolveCompiledStruct);

        return reslvedTypeNode;
    }
    if (&typeAst.__type__() == &w.std.ast.TypeAlias) {
        auto& resolveAstStruct = static_cast<Ast::Struct&>(*m_currentStruct).getTypeAlias(typeAst[w.ids.name]);
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
        return resolvedTypeAst[w.ids.value];
    } else if (resolvedTypeAst.isA(w.std.ast.ResolvedType)) {
        return resolvedTypeAst[w.ids.compiled];
    } else {
        throw "Unexpected AST type!";
    }
}


CellI& Compiler::getResolvedTypeById(CellI& id, bool isInstance)
{
    if (isInstance) {
        return m_unknownInstances.getValue(id)[w.ids.value];
    } else {
        return m_structs.getValue(id);
    }
}

CellI& Compiler::getOrCreateStructReference(CellI& structId, TrieMap& unresolvedContainer, std::function<CellI&(CellI& structReference)> unknownCb)
{
    if (unresolvedContainer.hasKey(structId)) {
        return unresolvedContainer.getValue(structId)[w.ids.value];
    } else {
        auto& structReference  = *new Object(w, w.std.StructReference);
        auto& unresolvedStruct = unknownCb(structReference);
        structReference.set(w.ids.value, unresolvedStruct);
        structReference.set(w.ids.id, structId);
        structReference.set(w.ids.scope, *m_scope);
        structReference.set(w.ids.resolvedScope, *m_resolvedScope);

        if (m_currentFn) {
            structReference.set(w.ids.currentFn, *m_currentFn);
        }
        if (m_currentStruct) {
            structReference.set(w.ids.currentStruct, *m_currentStruct);
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
            unresolvedStruct.set("incomplete", w.boolean.true_);

            return unresolvedStruct;
        });
    }
}

CellI& Compiler::resolveTemplateInstanceId(CellI& name, CellI& fullyQualifiedName, CellI& idScope, CellI& ast, CellI& templateParams)
{
    auto& templateId = ast[w.ids.id];
    return getOrCreateStructReference(fullyQualifiedName, m_unknownInstances, [this, &templateId, &name, &fullyQualifiedName, &templateParams, &ast, &idScope](CellI& structReference) -> CellI& {
        structReference.set(w.ids.templateId, templateId);
        structReference.set(w.ids.templateParams, templateParams);
        if (ast.has(w.ids.scopes)) {
            structReference.set("idScope", idScope);
        }
        auto& unresolvedStruct = *new Object(w, w.std.Struct, fmt::format("{}", name.label()));
        unresolvedStruct.set("fullyQualifiedName", fullyQualifiedName);
        unresolvedStruct.set("incomplete", w.boolean.true_);

        return unresolvedStruct;
    });
}

Ast::Base& Compiler::resolveTemplatedType(CellI& ast)
{
    auto& templateId     = ast[w.ids.id];
    auto& templateParams = static_cast<List&>(ast[w.ids.parameters]);

    Ast::Scope* scopePtr = m_scope;
    if (ast.has(w.ids.scopes)) {
        auto& scopeList          = ast[w.ids.scopes];
        Ast::StructT& astStructT = findTemplateByNameInScopes(*scopePtr, scopeList, templateId);
        scopePtr                 = &static_cast<Ast::Scope&>(astStructT[w.ids.scope]);
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
        CellI& unknownInstance = m_unknownInstances.getValue(fullyQualifiedStructName)[w.ids.value];
        if (unknownInstance.has(w.ids.ast)) {
            return createResolvedType(unknownInstance[w.ids.ast], unknownInstance);
        } else {
            unknownInstancePtr = &unknownInstance;
        }
    }

    auto& resolvedAstInstance = *new Ast::Struct(w, structName);
    resolvedAstInstance.set("fullyQualifiedName", fullyQualifiedStructName);
    resolvedAstInstance.set(w.ids.scope, scope);
    resolvedAstInstance.set("instanceOf", scope.getItem<Ast::StructT>(templateId));
    resolvedAstInstance.set("templateParams", resolvedTemplateParams);
    if (unknownInstancePtr) {
        unknownInstancePtr->set(w.ids.ast, resolvedAstInstance);
        return createResolvedType(resolvedAstInstance, *unknownInstancePtr);
    }
    auto& resolvedCompiledInstance = resolveTemplateInstanceId(structName, fullyQualifiedStructName, scope, ast, resolvedTemplateParams);
    resolvedCompiledInstance.set(w.ids.ast, resolvedAstInstance);

    return createResolvedType(resolvedAstInstance, resolvedCompiledInstance);
}

Ast::Base& Compiler::findEnumOrStructByAstStructName(Ast::Scope& scope, CellI& astStructName)
{
    auto& name = astStructName[w.ids.name];
    static List emptyList(w, w.std.Cell, "empty list");
    CellI* scopeListPtr = astStructName.has(w.ids.scopes) ? &astStructName[w.ids.scopes] : &emptyList;
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
        if (currentScope->has(w.ids.scope)) {
            currentScope = &static_cast<Ast::Scope&>(currentScope->get(w.ids.scope));
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
    Visitor::visitList(scopeList, [this, &currentScope](CellI& scopeId, int, bool& stop) {
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
    Visitor::visitList(m_unknownStructs[w.ids.list], [this](CellI& unknownStruct, int i, bool& stop) {
        WARN(compileStruct, "unknown struct: {}", unknownStruct[w.ids.value][w.ids.value].label());
    });

    int instantiedNum = 0;
    Visitor::visitList(m_unknownInstances[w.ids.list], [this, &instantiedNum](CellI& unknownInstanceSlot, int i, bool& stop) {
        CellI& unknownInstance  = unknownInstanceSlot[w.ids.value];
        auto& unknownInstanceId = unknownInstance[w.ids.id];

        if (IS_LOG_ENABLED) {
            TRACE(compileStruct, "unknown instance: {}", unknownInstanceId.label());
            if (unknownInstance.has("currentStruct")) {
                TRACE(compileStruct, "     from struct: {}", unknownInstance[w.ids.currentStruct].label());
            }
            if (unknownInstance.has("currentFn")) {
                TRACE(compileStruct, "   from function: {}", unknownInstance[w.ids.currentFn].label());
            }
        }

        std::stringstream ss;

        CellI& templateId     = unknownInstance[w.ids.templateId];
        auto& templateParams  = static_cast<List&>(unknownInstance[w.ids.templateParams]);
        auto& scope           = static_cast<Ast::Scope&>(unknownInstance[w.ids.scope]);
        auto& idScope         = unknownInstance.has(w.name("idScope")) ? static_cast<Ast::Scope&>(unknownInstance[w.name("idScope")]) : scope;

        ss << fmt::format("        in scope: {}", getFullyQualifiedName(idScope).label());
        ss << fmt::format("  instantiate id: {}<", templateId.label());
        Visitor::visitList(templateParams, [this, &ss](CellI& param, int i, bool& stop) {
            CellI& paramId   = param[w.ids.key];
            CellI& paramType = param[w.ids.type];
            if (i > 0) {
                ss << ", ";
            }
            ss << fmt::format("{}: {}", paramId.label(), getCompiledTypeFromResolvedType(paramType).label());
        });
        ss << ">";
        TRACE(compileStruct, ss.str());

        // we instantiate the template where we found it not where it belongs to
        // so std::List<Test> will not be instantiated in the std:: resolved scope
        auto& resolvedIdScope  = static_cast<Ast::Scope&>(unknownInstance[w.ids.resolvedScope]);
        auto& compiledStruct   = static_cast<Ast::Struct&>(unknownInstance[w.ids.value][w.ids.ast]);
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

    Visitor::visitList(inputParams, [this, &structT, &inputParameters, &ss](CellI& slot, int i, bool& stop) {
        CellI& key  = slot[w.ids.key];
        CellI& type = slot[w.ids.type];
        inputParameters.add(key, type);
    });

    Ast::Struct& ret = compiledStruct;
    ret.set("instanceOf", structT);
    ret.set("templateParams", inputParams);
    ret.set("scope", static_cast<Ast::Scope&>(structT.get("scope")));

    // instantiate type aliases
    if (structT.has("typeAliases")) {
        Map& instantiatedTypeAliases = *new Map(w, w.std.Cell, w.std.ast.Base);
        Visitor::visitList(structT.typeAliases()[w.ids.list], [this, &inputParameters, &instantiatedTypeAliases, &ret](CellI& slot, int i, bool& stop) {
            CellI& key               = slot[w.ids.key];
            CellI& type              = slot[w.ids.type];
            CellI& instantiatedParam = instantiateTemplateParamType(type, ret, inputParameters);
            instantiatedTypeAliases.add(key, w.ast.slot(key, instantiatedParam));
        });
        ret.set("typeAliases", instantiatedTypeAliases);
    }

    // instantiate methods
    if (structT.has("methods")) {
        Visitor::visitList(structT.methods()[w.ids.list], [this, &inputParameters, &ret](CellI& astFunction, int i, bool& stop) {
            Ast::Function& instantiedFunction = *new Ast::Function(w, astFunction[w.ids.name]);
            ret.addMethod(instantiedFunction);

            // parameters
            if (astFunction.has("parameters")) {
                List& instantiatedParameters = *new List(w, w.std.Slot);
                Visitor::visitList(astFunction[w.ids.parameters], [this, &inputParameters, &instantiatedParameters, &ret](CellI& slot, int i, bool& stop) {
                    CellI& key               = slot[w.ids.key];
                    CellI& type              = slot[w.ids.type];
                    CellI& instantiatedParam = instantiateTemplateParamType(type, ret, inputParameters);
                    instantiatedParameters.add(w.ast.slot(key, instantiatedParam));
                });
                instantiedFunction.set("parameters", instantiatedParameters);
            }
            // return type
            if (astFunction.has("returnType")) {
                CellI& returnType             = astFunction[w.ids.returnType];
                CellI& instantiatedReturnType = instantiateTemplateParamType(returnType, ret, inputParameters);
                instantiedFunction.set("returnType", instantiatedReturnType);
            }
            // instructions
            if (astFunction.has("instructions")) {
                instantiedFunction.set("instructions", instantiateAst(astFunction[w.ids.instructions], ret, inputParameters));
            }
        });
    }

    // instantiate members
    if (structT.has("members")) {
        Map& instantiatedMembers = *new Map(w, w.std.Cell, w.std.Slot);
        Visitor::visitList(structT.members()[w.ids.list], [this, &inputParameters, &instantiatedMembers, &ret](CellI& slot, int i, bool& stop) {
            CellI& key               = slot[w.ids.key];
            CellI& type              = slot[w.ids.type];
            CellI& instantiatedParam = instantiateTemplateParamType(type, ret, inputParameters);
            instantiatedMembers.add(key, w.ast.slot(key, instantiatedParam));
        });
        ret.set("members", instantiatedMembers);
    }

    // instantiate memberOf list
    if (structT.has("memberOf")) {
        List& instantiatedMemberOfs = *new List(w, w.std.Struct);
        Visitor::visitList(structT.memberOf(), [this, &inputParameters, &instantiatedMemberOfs, &ret](CellI& membershipType, int i, bool& stop) {
            CellI& instantiatedParam = instantiateTemplateParamType(membershipType, ret, inputParameters);
            instantiatedMemberOfs.add(instantiatedParam);
        });
        ret.set("memberOf", instantiatedMemberOfs);
    }

    return ret;
}

CellI& Compiler::instantiateTemplateParamType(CellI& param, CellI& selfType, Map& inputParameters)
{
    if (&param.__type__() == &w.std.ast.TemplateParam) {
        CellI& paramValue = param[w.ids.key];
        if (!inputParameters.hasKey(paramValue)) {
            throw "Instantiating with unknown template parameter!";
        }
        return inputParameters.getValue(paramValue);
    }
    if (&param.__type__() == &w.std.ast.TemplatedType) {
        List& resolvedParameterList = *new List(w, w.std.Slot);
        auto& ret                   = *new Ast::TemplatedType(w, param[w.ids.id], resolvedParameterList);
        auto& parametersList        = param[w.ids.parameters];

        Visitor::visitList(parametersList, [this, &resolvedParameterList, &selfType, &inputParameters](CellI& slot, int, bool&) {
            CellI& key              = slot[w.ids.key];
            CellI& type             = slot[w.ids.type];
            CellI& resolvedSlotType = instantiateTemplateParamType(type, selfType, inputParameters);
            resolvedParameterList.add(w.ast.slot(key, resolvedSlotType));
        });

        return ret;
    }
    if (&param.__type__() == &w.std.ast.Cell || &param.__type__() == &w.std.ast.StructName || &param.__type__() == &w.std.ast.TypeAlias) {
        return param;
    }

    throw "Unknown template parameter!";
}

Ast::Base& Compiler::instantiateAst(CellI& ast, CellI& selfType, Map& inputParameters)
{
    auto instantiate = [this, &selfType, &inputParameters](CellI& ast) -> Ast::Base& { return instantiateAst(ast, selfType, inputParameters); };

    if (&ast.__type__() == &w.std.ast.New) {
        auto* objectTypePtr = &ast[w.ids.objectType];
        if (&(*objectTypePtr).__type__() == &w.std.ast.TemplatedType) {
            CellI& resolvedObjectType = instantiateTemplateParamType(*objectTypePtr, selfType, inputParameters);
            objectTypePtr             = &resolvedObjectType;
        }
        auto& objectType = *static_cast<Ast::Base*>(objectTypePtr);
        if (ast.has("constructor")) {
            auto& constructor = ast[w.ids.constructor];
            Ast::Base& ret    = w.ast.new_(objectType, static_cast<Ast::Base&>(constructor));
            if (ast.has("parameters")) {
                auto& newParameters = *new cells::List(w, w.std.ast.Slot);
                Visitor::visitList(ast[w.ids.parameters], [this, &newParameters, &instantiate](CellI& slot, int, bool&) {
                    newParameters.add(w.ast.slot(slot[w.ids.key], instantiate(slot[w.ids.type])));
                });
                ret.set("parameters", newParameters);
            }
            return ret;
        } else {
            return w.ast.new_(objectType);
        }
    } else if (&ast.__type__() == &w.std.ast.Call) {
        Ast::Base& ret = w.ast.call(instantiate(ast[w.ids.cell]), ast[w.ids.method]);
        if (ast.has("parameters")) {
            // TODO process parameters
            ret.set("parameters", ast[w.ids.parameters]);
        }
        return ret;
    } else if (&ast.__type__() == &w.std.ast.StaticCall) {
        Ast::Base& ret = w.ast.scall(instantiate(ast[w.ids.cell]), ast[w.ids.method]);
        if (ast.has("parameters")) {
            // TODO process parameters
            ret.set("parameters", ast[w.ids.parameters]);
        }
        return ret;
    }

    // do nothing just traverse and copy the AST nodes
    if (&ast.__type__() == &w.std.ast.Block) {
        auto& instantiedAsts = *new cells::List(w, w.std.ast.Base);
        Visitor::visitList(ast[w.ids.asts], [this, &instantiedAsts, &instantiate](CellI& ast, int, bool&) {
            instantiedAsts.add(instantiate(ast));
        });
        return *new Ast::Block(w, instantiedAsts);
    } else if (&ast.__type__() == &w.std.ast.Cell) {
        return w.ast.cell(ast[w.ids.value]);
    } else if (&ast.__type__() == &w.std.ast.StructName) {
        auto& ret = w.ast.structName(ast[w.ids.value]);
        if (ast.has(w.ids.scopes)) {
            ret.set(w.ids.scopes, ast[w.ids.scopes]);
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
        if (ast.has(w.ids.value)) {
            return w.ast.throw_(instantiate(ast[w.ids.value]));
        } else {
            return w.ast.throw_();
        }
    } else if (&ast.__type__() == &w.std.ast.Parameter) {
        return w.ast.parameter(ast[w.ids.key]);
    } else if (&ast.__type__() == &w.std.ast.Var) {
        return w.ast.var(ast[w.ids.name]);
    } else if (&ast.__type__() == &w.std.ast.Delete) {
        return w.ast.delete_(instantiate(ast[w.ids.cell]));
    } else if (&ast.__type__() == &w.std.ast.Set) {
        return w.ast.set(instantiate(ast[w.ids.cell]), instantiate(ast[w.ids.key]), instantiate(ast[w.ids.value]));
    } else if (&ast.__type__() == &w.std.ast.Erase) {
        return w.ast.erase(instantiate(ast[w.ids.cell]), instantiate(ast[w.ids.key]));
    } else if (&ast.__type__() == &w.std.ast.If) {
        if (ast.has("else_")) {
            return w.ast.if_(instantiate(ast[w.ids.condition]))
                .then_(instantiate(ast[w.ids.then]))
                .else_(instantiate(ast[w.ids.else_]));
        } else {
            return w.ast.if_(instantiate(ast[w.ids.condition])).then_(instantiate(ast[w.ids.then]));
        }
    } else if (&ast.__type__() == &w.std.ast.Do) {
        return w.ast.do_(instantiate(ast[w.ids.statement])).while_(instantiate(ast[w.ids.condition]));
    } else if (&ast.__type__() == &w.std.ast.While) {
        return w.ast.while_(instantiate(ast[w.ids.condition])).do_(instantiate(ast[w.ids.statement]));
    } else if (&ast.__type__() == &w.std.ast.And) {
        return w.ast.and_(instantiate(ast[w.ids.lhs]), instantiate(ast[w.ids.rhs]));
    } else if (&ast.__type__() == &w.std.ast.Or) {
        return w.ast.or_(instantiate(ast[w.ids.lhs]), instantiate(ast[w.ids.rhs]));
    } else if (&ast.__type__() == &w.std.ast.Not) {
        return w.ast.not_(instantiate(ast[w.ids.input]));
    } else if (&ast.__type__() == &w.std.ast.Add) {
        return w.ast.add(instantiate(ast[w.ids.lhs]), instantiate(ast[w.ids.rhs]));
    } else if (&ast.__type__() == &w.std.ast.Subtract) {
        return w.ast.subtract(instantiate(ast[w.ids.lhs]), instantiate(ast[w.ids.rhs]));
    } else if (&ast.__type__() == &w.std.ast.Multiply) {
        return w.ast.multiply(instantiate(ast[w.ids.lhs]), instantiate(ast[w.ids.rhs]));
    } else if (&ast.__type__() == &w.std.ast.Divide) {
        return w.ast.divide(instantiate(ast[w.ids.lhs]), instantiate(ast[w.ids.rhs]));
    } else if (&ast.__type__() == &w.std.ast.LessThan) {
        return w.ast.lessThan(instantiate(ast[w.ids.lhs]), instantiate(ast[w.ids.rhs]));
    } else if (&ast.__type__() == &w.std.ast.LessThanOrEqual) {
        return w.ast.lessThanOrEqual(instantiate(ast[w.ids.lhs]), instantiate(ast[w.ids.rhs]));
    } else if (&ast.__type__() == &w.std.ast.GreaterThan) {
        return w.ast.greaterThan(instantiate(ast[w.ids.lhs]), instantiate(ast[w.ids.rhs]));
    } else if (&ast.__type__() == &w.std.ast.GreaterThanOrEqual) {
        return w.ast.greaterThanOrEqual(instantiate(ast[w.ids.lhs]), instantiate(ast[w.ids.rhs]));
    } else if (&ast.__type__() == &w.std.ast.Same) {
        return w.ast.same(instantiate(ast[w.ids.lhs]), instantiate(ast[w.ids.rhs]));
    } else if (&ast.__type__() == &w.std.ast.NotSame) {
        return w.ast.notSame(instantiate(ast[w.ids.lhs]), instantiate(ast[w.ids.rhs]));
    } else if (&ast.__type__() == &w.std.ast.Equal) {
        return w.ast.equal(instantiate(ast[w.ids.lhs]), instantiate(ast[w.ids.rhs]));
    } else if (&ast.__type__() == &w.std.ast.NotEqual) {
        return w.ast.notEqual(instantiate(ast[w.ids.lhs]), instantiate(ast[w.ids.rhs]));
    } else if (&ast.__type__() == &w.std.ast.Has) {
        return w.ast.has(instantiate(ast[w.ids.cell]), instantiate(ast[w.ids.key]));
    } else if (&ast.__type__() == &w.std.ast.Missing) {
        return w.ast.missing(instantiate(ast[w.ids.cell]), instantiate(ast[w.ids.key]));
    } else if (&ast.__type__() == &w.std.ast.Get) {
        return w.ast.get(instantiate(ast[w.ids.cell]), instantiate(ast[w.ids.key]));
    } else if (&ast.__type__() == &w.std.ast.Member) {
        return w.ast.member(ast[w.ids.key]);
    } else if (&ast.__type__() == &w.std.ast.Return) {
        if (ast.has("value")) {
            return w.ast.return_(static_cast<Ast::Base&>(instantiate(ast[w.ids.value])));
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
        Visitor::visitList(resolvedScope.items<Ast::Function>()[w.ids.list], [this](CellI& function, int i, bool& stop) {
            Ast::Function& astFunction = static_cast<Ast::Function&>(function[w.ids.value]);
            auto& compiledFunction = compileFunction(astFunction);
            compiledFunctions().add(getFullyQualifiedName(astFunction), compiledFunction);
        });
    }
    if (scope.has("structs")) {
        Visitor::visitList(resolvedScope.items<Ast::Struct>()[w.ids.list], [this](CellI& kvPair, int i, bool& stop) {
            Ast::Struct& astStruct = static_cast<Ast::Struct&>(kvPair[w.ids.value]);
            auto& compiledStruct   = compileStruct(astStruct);
            CellI& structFullName  = getFullyQualifiedName(astStruct);
            if (compiledStruct.has("typeAliases")) {
                CellI& typeAliasesIndex = compiledStruct["typeAliases"][w.ids.index];
                Visitor::visitList(typeAliasesIndex[w.ids.__type__][w.ids.slots][w.ids.list], [this, &structFullName, &typeAliasesIndex](CellI& typeAlias, int i, bool& stop) {
                    CellI& key      = typeAlias["key"];
                    CellI& value    = typeAliasesIndex[key][w.ids.value];
                    List& aliasName = *new List(w, w.std.Char);
                    Visitor::visitList(structFullName, [&aliasName](CellI& character, int i, bool& stop) {
                        aliasName.add(character);
                    });
                    aliasName.add(w.pools.chars.get(':'));
                    aliasName.add(w.pools.chars.get(':'));
                    Visitor::visitList(key, [&aliasName](CellI& character, int i, bool& stop) {
                        aliasName.add(character);
                    });
                    aliasName.label(structFullName.label() + "::" + key.label());
                    if (IS_LOG_ENABLED) {
                        TRACE(compileStruct, "{}: {}\n", aliasName.label(), value.label());
                    }
                    compiledStructs().add(aliasName, value);
                });
            }
            compiledStructs().add(structFullName, compiledStruct);
        });
    }
    if (scope.has("enums")) {
        Visitor::visitList(resolvedScope.items<Ast::Enum>()[w.ids.list], [this](CellI& enum_, int i, bool& stop) {
            Ast::Enum& astEnum   = static_cast<Ast::Enum&>(enum_[w.ids.value]);
            auto& compiledStruct = compileEnum(astEnum);
            compiledStructs().add(getFullyQualifiedName(astEnum), compiledStruct);
        });
    }
    if (scope.has("variables")) {
        Visitor::visitList(resolvedScope.items<Ast::Var>()[w.ids.list], [this](CellI& var, int i, bool& stop) {
            Ast::Var& astVar       = static_cast<Ast::Var&>(var[w.ids.value]);
            auto& varName          = getFullyQualifiedName(astVar);
            auto& compiledVariable = *new Object(w, w.std.op.Var, fmt::format("var {}", astVar.label()));
            compiledVariables().add(varName, compiledVariable);
        });
    }
    if (scope.has("scopes")) {
        Visitor::visitList(scope.items<Ast::Scope>()[w.ids.list], [this, &scope, &resolvedScope](CellI& scopeItem, int i, bool& stop) {
            Ast::Scope& nextScope = static_cast<Ast::Scope&>(scopeItem[w.ids.value]);
            if (nextScope.has("link")) {
                return;
            }
            auto& nextResolvedScope = resolvedScope.getItem<Ast::Scope>(nextScope[w.ids.name]);

            compileScope(nextScope, nextResolvedScope);
            m_scope         = &scope;
            m_resolvedScope = &resolvedScope;
        });
    }
}

CellI& Compiler::compileStruct(Ast::Struct& astStruct)
{
    CellI& compiledStruct = getResolvedTypeById(getFullyQualifiedName(astStruct), astStruct.has("instanceOf"));
    compiledStruct.erase("incomplete");

    // compile sub types
    if (astStruct.has("typeAliases")) {
        Map& compiledTypeAliases = *new Map(w, w.std.Cell, w.std.Struct, "typeAliases Map<Cell, Type>(...)");
        Visitor::visitList(astStruct.typeAliases()[w.ids.list], [this, &compiledTypeAliases](CellI& slot, int i, bool& stop) {
            CellI& key             = slot[w.ids.key];
            CellI& type            = slot[w.ids.type];
            auto& compiledSlotType = getCompiledTypeFromResolvedType(type);
            compiledTypeAliases.add(key, compiledSlotType);
        });
        compiledStruct.set("typeAliases", compiledTypeAliases);
    }

    // compile methods
    if (astStruct.has("methods")) {
        Map& compiledMethods = *new Map(w, w.std.Cell, w.std.ast.Function);
        Visitor::visitList(astStruct.methods()[w.ids.list], [this, &compiledMethods](CellI& astFunction, int i, bool& stop) {
            auto& compiledFunction = compileFunction(static_cast<Ast::Function&>(astFunction));
            compiledMethods.add(astFunction[w.ids.name], compiledFunction);
        });
        compiledStruct.set("methods", compiledMethods);
    }

    // compile members
    if (astStruct.has("members")) {
        Map& compiledMembers = *new Map(w, w.std.Cell, w.std.Slot, "members Map<Cell, Slot>(...)");
        Visitor::visitList(astStruct.members()[w.ids.list], [this, &compiledMembers, &compiledStruct](CellI& slot, int i, bool& stop) {
            CellI& key             = slot[w.ids.key];
            CellI& type            = slot[w.ids.type];
            auto& compiledSlotType = getCompiledTypeFromResolvedType(type);
            compiledMembers.add(key, w.std.slot(key, compiledSlotType));
        });
        compiledStruct.set("slots", compiledMembers);
    }

    // compile memberOf list
    if (astStruct.has("memberOf")) {
        Map& compiledMemberOfs = *new Map(w, w.std.Struct, w.std.Struct, "memberOf Map<Type, Type>(...)");
        Visitor::visitList(astStruct.memberOf(), [this, &compiledMemberOfs](CellI& membershipType, int i, bool& stop) {
            auto& compiledMembershipType = getCompiledTypeFromResolvedType(membershipType);
            compiledMemberOfs.add(compiledMembershipType, compiledMembershipType);
        });
        compiledStruct.set("memberOf", compiledMemberOfs);
    }

    return compiledStruct;
}

CellI& Compiler::compileEnum(Ast::Enum& enum_)
{
    CellI& compiledStruct   = getResolvedTypeById(getFullyQualifiedName(enum_), enum_.has("instanceOf"));

    compiledStruct.erase("incomplete");
    compiledStruct.set("enum", w.boolean.true_);

    // compile methods
    if (enum_.has("methods")) {
        Map& compiledMethods = *new Map(w, w.std.Cell, w.std.ast.Function);
        Visitor::visitList(enum_.methods()[w.ids.list], [this, &compiledMethods](CellI& astFunction, int i, bool& stop) {
            auto& compiledFunction = compileFunction(static_cast<Ast::Function&>(astFunction));
            compiledMethods.add(astFunction[w.ids.name], compiledFunction);
        });
        compiledStruct.set("methods", compiledMethods);
    }

    // compile values
    if (enum_.has("values")) {
        Map& compiledMembers = *new Map(w, w.std.Cell, w.std.Slot, "members Map<Cell, Slot>(...)");
        compiledMembers.add(w.ids.tag, w.std.Cell);
        compiledStruct.set("slots", compiledMembers);
        Visitor::visitList(enum_.values()[w.ids.list], [this, &enum_, &compiledMembers, &compiledStruct](CellI& kvPair, int i, bool& stop) {
            CellI& valueKey  = kvPair[w.ids.key];
            CellI& valueCell = kvPair[w.ids.value];
            CellI& valueName = valueCell[w.ids.name];
            if (&valueCell.__type__() == &w.std.ast.EnumValue) {
                auto& enumValue = static_cast<Ast::EnumValue&>(valueCell);
                auto& fullName  = getFullyQualifiedName(enumValue);
                if (valueCell.has(w.ids.value)) {
                    auto& value         = enumValue[w.ids.value];
                    auto& resolvedValue = resolveTypeInEnumValue(value);
                    auto& valueType     = resolvedValue.__type__();

                    compiledMembers.add(valueKey, valueType);
                    auto& compiledValue = *new Object(w, compiledStruct, fmt::format("{}::{}", enum_.label(), enumValue.label()));
                    compiledValue.set("tag", valueName);
                    compiledValue.set(valueName, value[w.ids.value]);
                    compiledVariables().add(fullName, compiledValue);
                } else {
                    compiledMembers.add(valueKey, compiledStruct);
                    auto& compiledValue = *new Object(w, compiledStruct, fmt::format("{}::{}", enum_.label(), enumValue.label()));
                    compiledValue.set("tag", valueName);
                    compiledValue.set(valueName, w.ids.emptyObject);
                    compiledVariables().add(fullName, compiledValue);
                }
            } else if (&valueCell.__type__() == &w.std.ast.TypedEnumValue) {
                auto& enumValue             = static_cast<Ast::TypedEnumValue&>(valueCell);
                auto& enumValueType         = valueCell["enumType"];
                auto& compiledEnumValueType = getCompiledTypeFromResolvedType(enumValueType);
                auto& fullName              = getFullyQualifiedName(enumValue);
                compiledMembers.add(valueKey, compiledEnumValueType);
            }
        });
    }

    return compiledStruct;
}

CellI& Compiler::compileFunction(Ast::Function& astFunction)
{
    cells::Object& compiledFunction = *new cells::Object(w, w.std.op.Function);
    compiledFunction.set(w.ids.name, astFunction.get(w.ids.name));

    // is this function a method?
    if (astFunction.has(w.ids.structType)) {
        auto& currentStruct = static_cast<Ast::Struct&>(*m_currentStruct);
        auto& structType    = astFunction.get(w.ids.structType);
        compiledFunction.set(w.ids.objectType, structType["compiledStruct"]);
    }
    compileFunctionParams(astFunction, compiledFunction);

    compiledFunction.set(w.ids.ast, astFunction);
    compiledFunction.set(w.ids.op, compileFunctionAst(astFunction, astFunction.instructions(), compiledFunction));
    if (astFunction.has(w.ids.static_)) {
        compiledFunction.set(w.ids.static_, astFunction.get(w.ids.static_));
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
            parameters.add(w.ids.self, w.std.slot("self", type));
            structTypeStr = fmt::format("{}::", type.label());
        }
        if (astFunction.has(w.ids.parameters)) {
            Visitor::visitList(astFunction.parameters(), [this, &parameters, &iss](CellI& slot, int i, bool& stop) {
                if (i > 0) {
                    iss << ", ";
                }
                auto& key          = slot[w.ids.key];
                auto& type         = slot[w.ids.type];
                auto& compiledType = getCompiledTypeFromResolvedType(type);
                iss << "p_" << key.label() << ": " << compiledType.label();
                parameters.add(key, w.std.slot(key, compiledType));
            });
        }
        compiledFunction.set(w.ids.parameters, parameters);
    }
    if (astFunction.has(w.ids.returnType)) {
        auto& astReturnType      = astFunction.returnType();
        auto& compiledReturnType = getCompiledTypeFromResolvedType(astReturnType);
        oss << compiledReturnType.label();
        compiledFunction.set(w.ids.returnType, compiledReturnType);
    }
    if (astFunction.has(w.ids.returnType)) {
        compiledFunction.label(fmt::format("fn {}{}({}) -> {}", structTypeStr, astFunction.get(w.ids.name).label(), iss.str(), oss.str()));
    } else {
        compiledFunction.label(fmt::format("fn {}{}({})", structTypeStr, astFunction.get(w.ids.name).label(), iss.str()));
    }
}

std::string Compiler::shortFunctionName(Ast::Function& function)
{
    std::stringstream iss;
    std::stringstream oss;
    if (function.has(w.ids.parameters) || function.has(w.ids.structType)) {
        if (function.has(w.ids.parameters)) {
            Visitor::visitList(function.parameters(), [this, &iss](CellI& slot, int i, bool& stop) {
                if (i > 0) {
                    iss << ", ";
                }
                iss << "p_" << slot[w.ids.key].label() << ": " << getCompiledTypeFromResolvedType(slot[w.ids.type]).label();
            });
        }
    }
    if (function.has(w.ids.returnType)) {
        oss << getCompiledTypeFromResolvedType(function.returnType()).label();
    }
    if (function.has(w.ids.returnType)) {
        return fmt::format("fn {}({}) -> {}", function.get(w.ids.name).label(), iss.str(), oss.str());
    } else {
        return fmt::format("fn {}({})", function.get(w.ids.name).label(), iss.str());
    }
}

CellI& Compiler::compileFunctionAst(Ast::Function& astFunction, CellI& ast, cells::Object& function)
{
    auto compile = [this, &astFunction, &function](CellI& ast) -> CellI& { return compileFunctionAst(astFunction, ast, function); };
    const auto _ = [this](auto& cell) -> Ast::Cell& { return w._(cell); };

    if (&ast.__type__() == &w.std.ast.Block) {
        CellI& list      = ast[w.ids.asts];
        CellI* prevBlock = nullptr;
        if (m_lastBlock) {
            prevBlock = m_lastBlock;
        }
        CellI* firstOpBlockNode   = nullptr;
        CellI* currentOpBlockNode = nullptr;
        Object& opBlock           = *new Object(w, w.std.op.Block);
        m_lastBlock = &opBlock;
        Visitor::visitList(list, [this, &compile, &opBlock, &firstOpBlockNode, &currentOpBlockNode](CellI& ast, int, bool&) {
            CellI& newOpBlockNode = *new Object(w, w.std.op.Activate);
            newOpBlockNode.set(w.ids.cell, compile(ast));
            newOpBlockNode.set(w.ids.parent, opBlock);

            if (!firstOpBlockNode) {
                firstOpBlockNode = &newOpBlockNode;
            } else {
                (*currentOpBlockNode).set(w.ids.next, newOpBlockNode);
            }
            currentOpBlockNode = &newOpBlockNode;
        });
        opBlock.set(w.ids.ast, ast);
        if (firstOpBlockNode) {
            opBlock.set(w.ids.ops, *firstOpBlockNode);
        }

        if (prevBlock) {
            m_lastBlock = prevBlock;
        } else {
            m_lastBlock = nullptr;
        }

        return opBlock;
    } else if (&ast.__type__() == &w.std.ast.Cell) {
        Object& constVar = *new Object(w, w.std.op.ConstVar);
        constVar.set(w.ids.ast, ast);
        constVar.set(w.ids.value, ast[w.ids.value]);
        return constVar;
    } else if (&ast.__type__() == &w.std.ast.ResolvedType) {
        Object& constVar = *new Object(w, w.std.op.ConstVar);
        constVar.set(w.ids.ast, ast);
        constVar.set(w.ids.value, ast[w.ids.compiled]);
        return constVar;
    } else if (&ast.__type__() == &w.std.ast.SelfFn) {
        Object& constVar = *new Object(w, w.std.op.ConstVar);
        constVar.set(w.ids.ast, ast);
        constVar.set(w.ids.value, function);
        return constVar;
    } else if (&ast.__type__() == &w.std.ast.Self) {
        CellI& retOp = compile(w.ast.get(_(function), _(w.ids.stack)) / _(w.ids.value) / _(w.ids.input) / _(w.ids.self));
        retOp.set(w.ids.ast, ast);
        retOp.label("self");
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Continue) {
        if (!m_lastBlock) {
            throw "No statement to break!";
        }
        CellI& lastBlock = *m_lastBlock;
        CellI& retOp     = compile(w.ast.set(_(lastBlock), _(w.ids.status), _(w.ids.continue_)));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Break) {
        if (!m_lastBlock) {
            throw "No statement to break!";
        }
        CellI& lastBlock = *m_lastBlock;
        CellI& retOp     = compile(w.ast.set(_(lastBlock), _(w.ids.status), _(w.ids.break_)));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Throw) {
        if (!m_lastBlock) {
            throw "No statement to break!";
        }
        CellI& lastBlock = *m_lastBlock;
        CellI& retOp     = compile(w.ast.set(_(lastBlock), _(w.ids.status), _(w.ids.throw_)));
        if (ast.has("value")) {
            retOp.set(w.ids.result, compile(w.ast.set(_(lastBlock), _(w.ids.value), static_cast<Ast::Base&>(ast[w.ids.value]))));
        }
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Parameter) {
        CellI& retOp = compile(w.ast.get(_(function), _(w.ids.stack)) / _(w.ids.value) / _(w.ids.input) / _(ast[w.ids.key]));
        retOp.set(w.ids.ast, ast);
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Member) {
        CellI& member = compile(w.ast.get(w.ast.self(), w.ast.cell(ast[w.ids.key])));
        member.set(w.ids.ast, ast);
        return member;
    } else if (&ast.__type__() == &w.std.ast.Return) {
        Object& retOp = *new Object(w, w.std.op.Return, "op.return");
        retOp.set(w.ids.ast, ast);
        if (ast.has(w.ids.value)) {
            retOp.set(w.ids.result, compile(w.ast.set(_(function), _(w.ids.value), static_cast<Ast::Base&>(ast[w.ids.value]))));
        }
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Var) {
        Index* localVarsIndexPtr = nullptr;
        if (function.missing(w.ids.localVars)) {
            localVarsIndexPtr = new Index(w, fmt::format("LocalVarsIndex of {}", function.label()));
            function.set(w.ids.localVars, *localVarsIndexPtr);
        } else {
            localVarsIndexPtr = &static_cast<Index&>(function[w.ids.localVars]);
        }

        Index &localVarsIndex = *localVarsIndexPtr;
        if (!localVarsIndex.has(ast[w.ids.name])) {
            localVarsIndex.insert(ast[w.ids.name], w.std.slot(ast[w.ids.name], w.std.op.Var));
        }

        CellI& retOp = compile(w.ast.get(_(function), _(w.ids.stack)) / _(w.ids.value) / _(w.ids.localVars) / _(ast[w.ids.name]));
        retOp.set(w.ids.ast, ast);
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Delete) {
        Object& retOp = *new Object(w, w.std.op.Delete);
        retOp.set(w.ids.ast, ast);
        retOp.set(w.ids.input, compile(ast[w.ids.cell]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Set) {
        Object& retOp = *new Object(w, w.std.op.Set);
        retOp.set(w.ids.ast, ast);
        retOp.set(w.ids.cell, compile(ast[w.ids.cell]));
        retOp.set(w.ids.key, compile(ast[w.ids.key]));
        retOp.set(w.ids.value, compile(ast[w.ids.value]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Erase) {
        Object& retOp = *new Object(w, w.std.op.Erase);
        retOp.set(w.ids.ast, ast);
        retOp.set(w.ids.cell, compile(ast[w.ids.cell]));
        retOp.set(w.ids.key, compile(ast[w.ids.key]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.If) {
        Object& retOp = *new Object(w, w.std.op.If);
        retOp.set(w.ids.ast, ast);
        retOp.set(w.ids.condition, compile(ast[w.ids.condition]));
        retOp.set(w.ids.then, compile(ast[w.ids.then]));
        if (ast.has(w.ids.else_)) {
            retOp.set(w.ids.else_, compile(ast[w.ids.else_]));
            return retOp;
        } else {
            return retOp;
        }
    } else if (&ast.__type__() == &w.std.ast.Match) {
        auto& enumObj        = static_cast<Ast::Base&>(ast["enum"]);
        auto& caseList       = ast["cases"][w.ids.list];
        auto& astCases       = *new cells::List(w, w.std.ast.Base);
        Ast::Block& astBlock = *new Ast::Block(w, astCases);

        Visitor::visitList(caseList, [this, &compile, &ast, &function, &enumObj, &astCases](CellI& kvpair, int, bool&) {
            auto& kind    = kvpair[w.ids.key];
            auto& op      = kvpair[w.ids.value];
            auto& oneCase = w.ast.if_(w.ast.same(w.ast.get(enumObj, "tag"), w._(kind))).then_(static_cast<Ast::Base&>(op));
            astCases.add(oneCase);
        });
        auto& retOp = compile(astBlock);
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Do) {
        Object& retOp = *new Object(w, w.std.op.Do);
        retOp.set(w.ids.ast, ast);
        retOp.set(w.ids.condition, compile(ast[w.ids.condition]));
        retOp.set(w.ids.statement, compile(ast[w.ids.statement]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.While) {
        Object& retOp = *new Object(w, w.std.op.While);
        retOp.set(w.ids.ast, ast);
        retOp.set(w.ids.condition, compile(ast[w.ids.condition]));
        retOp.set(w.ids.statement, compile(ast[w.ids.statement]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.New) {
        CellI* firstOpBlockNode = nullptr;
        auto& astObjectType     = ast[w.ids.objectType];

        Object& block = *new Object(w, w.std.op.Block);
        block.set(w.ids.ast, ast);

        Object& opSet = *new Object(w, w.std.op.Set, "New { block.value = new objectType(); }");
        opSet.set(w.ids.ast, ast);
        opSet.set(w.ids.cell, compile(w.ast.cell(block)));
        opSet.set(w.ids.key, compile(w.ast.cell(w.ids.value)));

        Object& opNew = *new Object(w, w.std.op.New);
        opNew.set(w.ids.ast, ast);
        opNew.set(w.ids.objectType, compile(astObjectType));
        opSet.set(w.ids.value, opNew);

        CellI& newOpBlockNode = *new Object(w, w.std.op.Activate);
        newOpBlockNode.set(w.ids.cell, opSet);
        newOpBlockNode.set(w.ids.parent, block);
        firstOpBlockNode = &newOpBlockNode;

        if (ast.has(w.ids.constructor)) {
            Object& callAst = *new Object(w, w.std.ast.Call);
            callAst.set(w.ids.cell, w.ast.get(w.ast.cell(block), w.ast.cell(w.ids.value)));
            callAst.set(w.ids.method, ast[w.ids.constructor]);
            if (ast.has(w.ids.parameters)) {
                callAst.set(w.ids.parameters, ast[w.ids.parameters]);
            }
            CellI& callConstructor = compile(callAst);
            callConstructor.label("New { call constructor; }");
            CellI& newOpBlockNode = *new Object(w, w.std.op.Activate);
            newOpBlockNode.set(w.ids.cell, callConstructor);
            newOpBlockNode.set(w.ids.parent, block);
            (*firstOpBlockNode).set(w.ids.next, newOpBlockNode);
        }
        block.set(w.ids.ops, *firstOpBlockNode);
        return block;
    } else if (&ast.__type__() == &w.std.ast.Call || &ast.__type__() == &w.std.ast.StaticCall) {
        Ast::Base& astCell   = static_cast<Ast::Base&>(ast[w.ids.cell]);
        Ast::Base& astMethod = static_cast<Ast::Base&>(ast[w.ids.method]);
        auto& astMethodId    = astMethod[w.ids.value];
#if 1
        bool checked = false;
        // New<T>.constructor(...)
        std::stringstream ss;
        if (&astCell.__type__() == &w.std.ast.Get && &astCell[w.ids.cell].__type__() == &w.std.ast.Cell && &astCell[w.ids.cell][w.ids.value].__type__() == &w.std.op.Block) {
            auto& opBlock    = astCell[w.ids.cell][w.ids.value];
            auto& opBlockAst = opBlock[w.ids.ast];
            if (&opBlockAst.__type__() == &w.std.ast.New) {
                auto& astObjectTypeStruct = opBlockAst[w.ids.objectType];
                checkMethodCall(astObjectTypeStruct, astMethodId);
                checked = true;
            }
            // var_xxx.call(...)
        } else if (&astCell.__type__() == &w.std.ast.Get && &astCell[w.ids.cell].__type__() == &w.std.ast.Var) {
            auto& astVar = astCell[w.ids.cell];
            ss << "In " << astVar.label() << " ";
            // m_xxx.method(...)
        } else if (&astCell.__type__() == &w.std.ast.Member) {
            auto& astMemberId    = astCell[w.ids.key];
            auto& astMembersType = function[w.ids.ast][w.ids.structType];
            if (&astMemberId == &w.ids.__type__) {
                // std::cout << "DDDD " << astMembersType.label();
                auto& stdScope = w.globalScope.getItem<Ast::Scope>("std");
                auto& type     = stdScope.getItem<Ast::Struct>("Struct");
                checkMethodCall(type, astMethodId);
                checked = true;
            } else {
                auto& astMembers = static_cast<Map&>(function[w.ids.ast][w.ids.structType][w.ids.members]);
                if (astMembers.hasKey(astMemberId)) {
                    auto& slot = static_cast<Ast::Slot&>(astMembers.getValue(astMemberId));
                    auto& type = slot[w.ids.type];
                    checkMethodCall(type, astMethodId);
                    checked = true;
                } else {
                    throw "Unknown member name!";
                }
            }
        } else if (&astCell.__type__() == &w.std.ast.Self) {
            auto& astSelfType = function[w.ids.ast][w.ids.structType];
            checkMethodCall(astSelfType, astMethodId);
            checked = true;
        } else if (&astCell.__type__() == &w.std.ast.Parameter) {
            auto& parameterRole                 = astCell[w.ids.key];
            auto& astFunctionParameters         = function[w.ids.ast][w.ids.parameters];
            Ast::Slot* astFunctionParameterSlot = nullptr;
            Visitor::visitList(astFunctionParameters, [this, &parameterRole, &astFunctionParameterSlot](CellI& slot, int i, bool& stop) {
                CellI& key  = slot[w.ids.key];
                CellI& type = slot[w.ids.type];
                if (&key == &parameterRole) {
                    astFunctionParameterSlot = &static_cast<Ast::Slot&>(slot);
                    stop                     = true;
                    return;
                }
            });
            if (astFunctionParameterSlot) {
                Ast::Slot& slot        = *astFunctionParameterSlot;
                auto& astParameterType = slot[w.ids.type];
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
        retOp.set(w.ids.ast, ast);
        retOp.set(w.ids.cell, compile(ast[w.ids.cell]));
        retOp.set(w.ids.method, compile(ast[w.ids.method]));
        retOp.set(w.ids.stack, compile(w.ast.get(_(function), _(w.ids.stack))));
        if (ast.has(w.ids.parameters)) {
            List& parameters = *new List(w, w.std.Slot);
            Visitor::visitList(ast[w.ids.parameters], [this, &parameters, &compile, &_](CellI& param, int, bool&) {
                CellI& slot = *new Object(w, w.std.Slot);
                slot.set(w.ids.key, param[w.ids.key]);
                slot.set(w.ids.type, compile(param[w.ids.type]));
                parameters.add(slot);
            });
            retOp.set(w.ids.parameters, parameters);
        }

        return retOp;
    } else if (&ast.__type__() == &w.std.ast.And) {
        Object& retOp = *new Object(w, w.std.op.And);
        retOp.set(w.ids.ast, ast);
        retOp.set(w.ids.lhs, compile(ast[w.ids.lhs]));
        retOp.set(w.ids.rhs, compile(ast[w.ids.rhs]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Or) {
        Object& retOp = *new Object(w, w.std.op.Or);
        retOp.set(w.ids.ast, ast);
        retOp.set(w.ids.lhs, compile(ast[w.ids.lhs]));
        retOp.set(w.ids.rhs, compile(ast[w.ids.rhs]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Not) {
        Object& retOp = *new Object(w, w.std.op.Not);
        retOp.set(w.ids.ast, ast);
        retOp.set(w.ids.input, compile(ast[w.ids.input]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Add) {
        Object& retOp = *new Object(w, w.std.op.Add);
        retOp.set(w.ids.ast, ast);
        retOp.set(w.ids.lhs, compile(ast[w.ids.lhs]));
        retOp.set(w.ids.rhs, compile(ast[w.ids.rhs]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Subtract) {
        Object& retOp = *new Object(w, w.std.op.Subtract);
        retOp.set(w.ids.ast, ast);
        retOp.set(w.ids.lhs, compile(ast[w.ids.lhs]));
        retOp.set(w.ids.rhs, compile(ast[w.ids.rhs]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Multiply) {
        Object& retOp = *new Object(w, w.std.op.Multiply);
        retOp.set(w.ids.ast, ast);
        retOp.set(w.ids.lhs, compile(ast[w.ids.lhs]));
        retOp.set(w.ids.rhs, compile(ast[w.ids.rhs]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Divide) {
        Object& retOp = *new Object(w, w.std.op.Divide);
        retOp.set(w.ids.ast, ast);
        retOp.set(w.ids.lhs, compile(ast[w.ids.lhs]));
        retOp.set(w.ids.rhs, compile(ast[w.ids.rhs]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.LessThan) {
        Object& retOp = *new Object(w, w.std.op.LessThan);
        retOp.set(w.ids.ast, ast);
        retOp.set(w.ids.lhs, compile(ast[w.ids.lhs]));
        retOp.set(w.ids.rhs, compile(ast[w.ids.rhs]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.LessThanOrEqual) {
        Object& retOp = *new Object(w, w.std.op.LessThanOrEqual);
        retOp.set(w.ids.ast, ast);
        retOp.set(w.ids.lhs, compile(ast[w.ids.lhs]));
        retOp.set(w.ids.rhs, compile(ast[w.ids.rhs]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.GreaterThan) {
        Object& retOp = *new Object(w, w.std.op.GreaterThan);
        retOp.set(w.ids.ast, ast);
        retOp.set(w.ids.lhs, compile(ast[w.ids.lhs]));
        retOp.set(w.ids.rhs, compile(ast[w.ids.rhs]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.GreaterThanOrEqual) {
        Object& retOp = *new Object(w, w.std.op.GreaterThanOrEqual);
        retOp.set(w.ids.ast, ast);
        retOp.set(w.ids.lhs, compile(ast[w.ids.lhs]));
        retOp.set(w.ids.rhs, compile(ast[w.ids.rhs]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Same) {
        Object& retOp = *new Object(w, w.std.op.Same);
        retOp.set(w.ids.ast, ast);
        retOp.set(w.ids.lhs, compile(ast[w.ids.lhs]));
        retOp.set(w.ids.rhs, compile(ast[w.ids.rhs]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.NotSame) {
        Object& retOp = *new Object(w, w.std.op.NotSame);
        retOp.set(w.ids.ast, ast);
        retOp.set(w.ids.lhs, compile(ast[w.ids.lhs]));
        retOp.set(w.ids.rhs, compile(ast[w.ids.rhs]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Equal) {
        Object& retOp = *new Object(w, w.std.op.Equal);
        retOp.set(w.ids.ast, ast);
        retOp.set(w.ids.lhs, compile(ast[w.ids.lhs]));
        retOp.set(w.ids.rhs, compile(ast[w.ids.rhs]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.NotEqual) {
        Object& retOp = *new Object(w, w.std.op.NotEqual);
        retOp.set(w.ids.ast, ast);
        retOp.set(w.ids.lhs, compile(ast[w.ids.lhs]));
        retOp.set(w.ids.rhs, compile(ast[w.ids.rhs]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Has) {
        Object& retOp = *new Object(w, w.std.op.Has);
        retOp.set(w.ids.ast, ast);
        retOp.set(w.ids.cell, compile(ast[w.ids.cell]));
        retOp.set(w.ids.key, compile(ast[w.ids.key]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Missing) {
        Object& retOp = *new Object(w, w.std.op.Missing);
        retOp.set(w.ids.ast, ast);
        retOp.set(w.ids.cell, compile(ast[w.ids.cell]));
        retOp.set(w.ids.key, compile(ast[w.ids.key]));
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Get) {
        Object& retOp = *new Object(w, w.std.op.Get);
        retOp.set(w.ids.ast, ast);
        retOp.set(w.ids.cell, compile(ast[w.ids.cell]));
        retOp.set(w.ids.key, compile(ast[w.ids.key]));
        return retOp;
    }

    throw "Unknown function AST!";
}

void Compiler::checkMethodCall(CellI& astType, CellI& astMethodId)
{
    CellI* typePtr  = nullptr;
    Map* methodsPtr = nullptr;
    if (astType.isA(w.std.ast.Cell)) {
        auto& cellValue = astType[w.ids.value];
        typePtr         = &cellValue;
        methodsPtr      = &static_cast<Map&>(cellValue[w.ids.methods]);
    } else if (astType.isA(w.std.ast.Struct)) {
        typePtr    = &astType;
        methodsPtr = &static_cast<Map&>(astType[w.ids.methods]);
    } else if (astType.isA(w.std.ast.ResolvedType)) {
        typePtr    = &astType[w.ids.compiled];
        methodsPtr = &static_cast<Map&>(astType[w.ids.ast][w.ids.methods]);
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
        Visitor::visitList(scope.items<Ast::Struct>()[w.ids.list], [this](CellI& astStruct, int i, bool& stop) {
            auto& tool = static_cast<Ast::Struct&>(astStruct[w.ids.value]);
            if (tool.has(w.ids.description) && tool[w.ids.description].has(w.ids.asts)) {
                auto& compiledAstStruct = compiledStructs().getValue(getFullyQualifiedName(tool));
                m_toolFinder.add(tool, compiledAstStruct);
            }
        });
    }
    if (scope.has("scopes")) {
        Visitor::visitList(scope.items<Ast::Scope>()[w.ids.list], [this, &scope](CellI& scopeItem, int i, bool& stop) {
            Ast::Scope& nextScope = static_cast<Ast::Scope&>(scopeItem[w.ids.value]);
            processDescriptionsInScope(nextScope);
            m_scope = &scope;
        });
    }
}

} // namespace cells
} // namespace infocell