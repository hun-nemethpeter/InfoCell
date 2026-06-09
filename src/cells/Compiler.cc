#include "Compiler.h"

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include "util/Log.h"

#include <sstream>

namespace infocell {
namespace cells {

Compiler::Compiler(Brain& kb) :
    kb(kb),
    m_earlyStructs(kb, kb.std.Cell, kb.std.Cell, "earlyStructs")
{
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
CellI& Compiler::compile(Ast::Scope& scope)
{
    auto& program     = *new Object(kb, kb.std.Program, "Program");
    auto& programData = *new Object(kb, kb.std.ProgramData, "ProgramData");
    program.set(kb.ids.data, programData);

    auto& compileState      = *new Object(kb, kb.std.CompileState, "CompileState");
    auto& compiledFunctions = *new TrieMap(kb, kb.std.Cell, kb.std.op.Function, "Functions");
    auto& compiledStructs   = *new TrieMap(kb, kb.std.Cell, kb.std.Struct, "Types");

    auto& functions           = *new List(kb, kb.std.op.Function, "Functions");
    auto& structs             = *new TrieMap(kb, kb.std.Cell, kb.std.Struct, "structs");
    auto& unknownStructs      = *new TrieMap(kb, kb.std.Cell, kb.std.Struct, "unknownStructs");
    auto& instances           = *new TrieMap(kb, kb.std.Cell, kb.std.Struct, "instances");
    auto& unknownInstances    = *new TrieMap(kb, kb.std.Cell, kb.std.Struct, "unknownInstances");
    auto& instanceAsts        = *new TrieMap(kb, kb.std.Cell, kb.std.Struct, "instanceAsts");
    auto& unknownInstanceAsts = *new TrieMap(kb, kb.std.Cell, kb.std.Struct, "unknownInstanceAsts");
    auto& compiledVariables   = *new TrieMap(kb, kb.std.Cell, kb.std.op.Var, "Variables");

    programData.set(kb.ids.functions, compiledFunctions);
    programData.set(kb.ids.structs, compiledStructs);
    programData.set(kb.ids.variables, compiledVariables);

    compileState.set("functions", functions);
    compileState.set("structs", structs);
    compileState.set("unknownStructs", unknownStructs);
    compileState.set("instances", instances);
    compileState.set("unknownInstances", unknownInstances);
    compileState.set("instanceAsts", instanceAsts);
    compileState.set("unknownInstanceAsts", unknownInstanceAsts);
    compileState.set("variables", compiledVariables);

    registerEarlyStructs(unknownStructs, unknownInstances);

    // Step 1. creating a shadow AST tree where templated thing are resolved
    auto& resolvedScope = resolveTypesInScope(scope, compileState);

    resolveEarlyStructsInScope(scope, unknownStructs, unknownInstances, resolvedScope);

    // Sanity check we still referencing an unknown struct
    // Print all unknown references before bail out
    Visitor::visitList(unknownStructs[kb.ids.list], [this](CellI& unknownStruct, int i, bool& stop) {
        WARN(compileStruct, "unknown struct: {}", unknownStruct[kb.ids.value][kb.ids.value].label());
    });

    // Step 2. process the shadow AST tree and instantiate templates
    int countOfInstantiedTemplates = instantiateTemplateInstances(unknownInstances, compileState, resolvedScope);

    // Stop the compilation as we have unknown references
    if (unknownStructs.size() > 0 || unknownInstances.size() != countOfInstantiedTemplates) {
        throw "Referencing an unknown type!";
    }
    compileState.set("scope", scope);
    compileState.set("resolvedScope", resolvedScope);

#if 0
    // Step 3. actual compilation
    compileTheResolvedAsts(programData, compileState);

    // Step 3. process descriptions
    processDescriptionsInAsts(programData, compileState);
#endif

    return program;
}

void Compiler::registerEarlyStructs(TrieMap& unknownStructs, TrieMap& unknownInstances)
{
    Visitor::visitList(m_earlyStructs[kb.ids.list], [this, &unknownStructs, &unknownInstances](CellI& earlyStructKV, int i, bool& stop) {
        auto& structId       = earlyStructKV[kb.ids.key];
        auto& structRefAst   = earlyStructKV[kb.ids.value][kb.ids.key];
        auto& compiledStruct = earlyStructKV[kb.ids.value][kb.ids.type];

        TRACE(compileStruct, "early struct: {}", earlyStructKV[kb.ids.key].label());

        auto& structReference = *new Object(kb, kb.std.StructReference);
        structReference.set(kb.ids.value, compiledStruct);
        structReference.set(kb.ids.id, structId);

        if (&structRefAst.struct_() == &kb.std.ast.TemplatedType) {
            unknownInstances.add(structId, structReference);
        } else if (&structRefAst.struct_() == &kb.std.ast.StructName) {
            unknownStructs.add(structId, structReference);
        }
    });
}

Ast::Scope& Compiler::resolveTypesInScope(Ast::Scope& scope, CellI& state)
{
    auto& resolvedScope = *new Ast::Scope(kb, scope.label());
    scope.set(kb.ids.resolvedScope, resolvedScope);
    state.set("scope", scope);
    state.set("resolvedScope", resolvedScope);

    if (scope.has("functions")) {
        state.erase("currentStruct");
        Visitor::visitList(scope.items<Ast::Function>()[kb.ids.list], [this, &state, &resolvedScope](CellI& origAstFunctionCell, int i, bool& stop) {
            Ast::Function& origAstFunction     = static_cast<Ast::Function&>(origAstFunctionCell[kb.ids.value]);
            Ast::Function& resolvedAstFunction = origAstFunction.resolveTypes(state);
            resolvedScope.add<Ast::Function>(resolvedAstFunction);
        });
    }
    if (scope.has("structs")) {
        Visitor::visitList(scope.items<Ast::Struct>()[kb.ids.list], [this, &state, &resolvedScope](CellI& origAstStructCell, int i, bool& stop) {
            Ast::Struct& origAstStruct     = static_cast<Ast::Struct&>(origAstStructCell[kb.ids.value]);
            Ast::Struct& resolvedAstStruct = origAstStruct.resolveTypes(state);
            resolvedScope.add<Ast::Struct>(resolvedAstStruct);
        });
    }
    if (scope.has("enums")) {
        Visitor::visitList(scope.items<Ast::Enum>()[kb.ids.list], [this, &state, &resolvedScope](CellI& origAstEnumCell, int i, bool& stop) {
            Ast::Enum& origAstEnum     = static_cast<Ast::Enum&>(origAstEnumCell[kb.ids.value]);
            Ast::Enum& resolvedAstEnum = origAstEnum.resolveTypes(state);
            resolvedScope.add<Ast::Enum>(resolvedAstEnum);
        });
    }
    if (scope.has("variables")) {
        Visitor::visitList(scope.items<Ast::Var>()[kb.ids.list], [this, &state, &resolvedScope](CellI& origAstVarCell, int i, bool& stop) {
            Ast::Var& origAstVar = static_cast<Ast::Var&>(origAstVarCell[kb.ids.value]);
            resolvedScope.add<Ast::Var>(origAstVar);
        });
    }
    if (scope.has("scopes")) {
        Visitor::visitList(scope.items<Ast::Scope>()[kb.ids.list], [this, &scope, &state, &resolvedScope](CellI& origAstScopeCell, int i, bool& stop) {
            Ast::Scope& origAstScope     = static_cast<Ast::Scope&>(origAstScopeCell[kb.ids.value]);
            Ast::Scope& resolvedAstScope = resolveTypesInScope(origAstScope, state);
            resolvedScope.add<Ast::Scope>(resolvedAstScope);
            state.set("scope", scope);
        });
    }

    return resolvedScope;
}

void Compiler::resolveEarlyStructsInScope(Ast::Scope& scope, TrieMap& unknownStructs, TrieMap& unknownInstances, Ast::Scope& resolvedScope)
{
    if (m_earlyStructs.empty()) {
        return;
    }
    auto& stdScope         = scope.getItem<Ast::Scope>("std");
    auto& resolvedStdScope = resolvedScope.getItem<Ast::Scope>("std");

    Visitor::visitList(m_earlyStructs[kb.ids.list], [this, &unknownStructs, &unknownInstances, &stdScope, &resolvedStdScope](CellI& earlyStructKV, int i, bool& stop) {
        auto& structId       = earlyStructKV[kb.ids.key];
        auto& structRefAst   = earlyStructKV[kb.ids.value][kb.ids.key];
        auto& compiledStruct = earlyStructKV[kb.ids.value][kb.ids.type];

        TRACE(compileStruct, "resolve early struct: {}", earlyStructKV[kb.ids.key].label());

        if (&structRefAst.struct_() == &kb.std.ast.TemplatedType) {
            if (unknownInstances.hasKey(structId)) {
                CellI& structReference = unknownInstances.getValue(structId);
                structReference.set("scope", stdScope);
                structReference.set("resolvedScope", stdScope);
                structReference.set("templateId", structRefAst["id"]);
                structReference.set(kb.ids.templateParams, structRefAst[kb.ids.parameters]);
            }
        } else if (&structRefAst.struct_() == &kb.std.ast.StructName) {
            if (unknownStructs.hasKey(structId)) {
                CellI& structReference = unknownStructs.getValue(structId);
            }
        }
    });
}

int Compiler::instantiateTemplateInstances(TrieMap& unknownInstances, Object& compileState, Ast::Scope& resolvedScope)
{
    int instantiedNum = 0;
    Visitor::visitList(unknownInstances[kb.ids.list], [this, &compileState, &instantiedNum](CellI& unknownInstanceSlot, int i, bool& stop) {
        CellI& unknownInstance  = unknownInstanceSlot[kb.ids.value];
        auto& unknownInstanceId = unknownInstance[kb.ids.id];

        if (IS_LOG_ENABLED) {
            TRACE(compileStruct, "unknown instance: {}", unknownInstanceId.label());
            if (unknownInstance.has("currentStruct")) {
                TRACE(compileStruct, "     from struct: {}", unknownInstance[kb.ids.currentStruct].label());
            }
            if (unknownInstance.has("currentFn")) {
                TRACE(compileStruct, "   from function: {}", unknownInstance[kb.ids.currentFn].label());
            }
        }

        std::stringstream ss;

        CellI& templateId     = unknownInstance[kb.ids.templateId];
        CellI& templateParams = unknownInstance[kb.ids.templateParams];
        auto& scope           = static_cast<Ast::Scope&>(unknownInstance[kb.ids.scope]);
        auto& idScope         = unknownInstance.has(kb.name("idScope")) ? static_cast<Ast::Scope&>(unknownInstance[kb.name("idScope")]) : scope;

        ss << fmt::format("        in scope: {}", idScope.getFullyQualifiedName().label());
        ss << fmt::format("  instantiate id: {}<", templateId.label());
        Visitor::visitList(templateParams, [this, &ss, &compileState](CellI& param, int i, bool& stop) {
            CellI& paramId   = param[kb.ids.key];
            CellI& paramType = param[kb.ids.type];
            if (i > 0) {
                ss << ", ";
            }
            ss << fmt::format("{}: {}", paramId.label(), getCompiledTypeFromResolvedType(paramType).label());
        });
        ss << ">";
        TRACE(compileStruct, ss.str());

        auto& resolvedIdScope = static_cast<Ast::Scope&>(idScope[kb.ids.resolvedScope]);
        compileState.set("scope", idScope);
        auto& structT          = idScope.getItem<Ast::StructT>(templateId);
        auto& instantiedStruct = structT.instantiateWith(static_cast<List&>(templateParams), compileState);
        auto& resolvedStruct   = instantiedStruct.resolveTypes(compileState);
        resolvedIdScope.add<Ast::Struct>(resolvedStruct);
        instantiedNum = i + 1;
    });

    return instantiedNum;
}

Ast::Base& Compiler::resolveType(CellI& typeAst, CellI& resolveState)
{
    if (&typeAst.struct_() == &kb.std.ast.Cell) {
        auto& cell      = static_cast<Ast::Cell&>(typeAst);
        auto& cellValue = typeAst[kb.ids.value];
        if (!cellValue.isA(kb.std.Struct) && !cellValue.isA(kb.std.Enum)) {
            throw "Type AST referencing a non-type!";
        }
        return cell;
    }
    if (&typeAst.struct_() == &kb.std.ast.ResolvedType) {
        return static_cast<Ast::ResolvedType&>(typeAst);
    }
    if (&typeAst.struct_() == &kb.std.ast.StructName) {
        Ast::Struct& resolveAstStruct = static_cast<Ast::Struct&>(resolveTypeNameAsAst(typeAst, resolveState));
        auto& resolveCompiledStruct   = resolveStructName(resolveAstStruct.getFullyQualifiedName(), resolveState);
        auto& reslvedTypeNode         = resolvedType(resolveAstStruct, resolveCompiledStruct);

        return reslvedTypeNode;
    }
    if (&typeAst.struct_() == &kb.std.ast.SubTypeName) {
        auto& resolveAstStruct = resolveSubTypeNameAsAst(typeAst, resolveState);

        return resolveAstStruct;
    }
    if (&typeAst.struct_() == &kb.std.ast.TemplatedType) {
        return resolveTemplatedType(typeAst, resolveState);
    }

    throw "Unknown ast type node!";
}

CellI& Compiler::getCompiledTypeFromResolvedType(CellI& resolvedTypeAst)
{
    if (resolvedTypeAst.isA(kb.std.ast.Cell)) {
        return resolvedTypeAst[kb.ids.value];
    } else if (resolvedTypeAst.isA(kb.std.ast.ResolvedType)) {
        return resolvedTypeAst[kb.ids.compiled];
    } else {
        throw "Unexpected AST type!";
    }
}

CellI& Compiler::getResolvedTypeById(CellI& id, bool isInstance, CellI& resolveState)
{
    if (isInstance) {
        auto& instances = static_cast<TrieMap&>(resolveState[kb.ids.instances]);
        if (instances.hasKey(id)) {
            return instances.getValue(id);
        } else {
            auto& unknownInstances = static_cast<TrieMap&>(resolveState[kb.ids.unknownInstances]);
            return unknownInstances.getValue(id)[kb.ids.value];
        }
    } else {
        auto& structs = static_cast<TrieMap&>(resolveState[kb.ids.structs]);
        return structs.getValue(id);
    }
}

CellI& Compiler::resolveId(CellI& structId, CellI& containerId, CellI& unknownContainerId, CellI& resolveState, std::function<CellI&(CellI& structReference)> unknownCb)
{
    auto& container = static_cast<TrieMap&>(resolveState[containerId]);

    if (container.hasKey(structId)) {
        return container.getValue(structId);
    } else {
        auto& unresolvedContainer = static_cast<TrieMap&>(resolveState[unknownContainerId]);
        if (unresolvedContainer.hasKey(structId)) {
            return unresolvedContainer.getValue(structId)[kb.ids.value];
        } else {

            auto& structReference = *new Object(kb, kb.std.StructReference);
            auto& unresolvedStruct = unknownCb(structReference);
            structReference.set(kb.ids.value, unresolvedStruct);
            structReference.set(kb.ids.id, structId);
            structReference.set(kb.ids.scope, resolveState[kb.ids.scope]);
            structReference.set(kb.ids.resolvedScope, resolveState[kb.ids.resolvedScope]);

            if (resolveState.has(kb.ids.currentFn)) {
                structReference.set(kb.ids.currentFn, resolveState[kb.ids.currentFn]);
            }
            if (resolveState.has(kb.ids.currentStruct)) {
                structReference.set(kb.ids.currentStruct, resolveState[kb.ids.currentStruct]);
            }
            unresolvedContainer.add(structId, structReference);

            return unresolvedStruct;
        }
    }
}

CellI& Compiler::resolveTemplateInstanceId(CellI& structId, CellI& idScope, CellI& resolveState, CellI& ast, CellI& templateParams)
{
    auto& templateId = ast[kb.ids.id];
    return resolveId(structId, kb.name("instances"), kb.name("unknownInstances"), resolveState, [this, &resolveState, &templateId, &structId ,&templateParams, &ast, &idScope](CellI& structReference) -> CellI& {
        structReference.set(kb.ids.templateId, templateId);
        structReference.set(kb.ids.templateParams, templateParams);
        if (ast.has(kb.ids.scopes)) {
            structReference.set("idScope", idScope);
        }
        auto& unresolvedStruct = *new Object(kb, kb.std.Struct, fmt::format("{}", structId.label()));
        unresolvedStruct.set("incomplete", kb.boolean.true_);

        return unresolvedStruct;
    });
}


Ast::Struct& Compiler::resolveTemplateInstanceIdAsAst(CellI& structId, CellI& idScope, CellI& resolveState, CellI& ast, CellI& templateParams)
{
    auto& templateId = ast[kb.ids.id];
    auto& ret = resolveId(structId, kb.name("instanceAsts"), kb.name("unknownInstanceAsts"), resolveState, [this, &resolveState, &templateId, &structId, &templateParams, &ast, &idScope](CellI& structReference) -> CellI& {
        structReference.set(kb.ids.templateId, templateId);
        structReference.set(kb.ids.templateParams, templateParams);
        if (ast.has(kb.ids.scopes)) {
            structReference.set("idScope", idScope);
        }
        auto& unresolvedStruct = *new Ast::Struct(kb, structId);

        return unresolvedStruct;
    });
    if (ret.missing(kb.ids.scope)) {
        ret.set(kb.ids.scope, idScope);
    }

    return static_cast<Ast::Struct&>(ret);
}

CellI& Compiler::resolveStructName(CellI& structId, CellI& resolveState)
{
    return resolveId(structId, kb.ids.structs, kb.ids.unknownStructs, resolveState, [this, &structId](CellI& structReference) -> CellI& {
        auto& unresolvedStruct = *new Object(kb, kb.std.Struct, fmt::format("{}", structId.label()));
        unresolvedStruct.set("incomplete", kb.boolean.true_);

        return unresolvedStruct;
    });
}

Ast::Base& Compiler::resolveTypeNameAsAst(CellI& structName, CellI& resolveState)
{
    Ast::Scope& currentScope = static_cast<Ast::Scope&>(resolveState[kb.ids.scope]);
    return resolveTypeName(currentScope, structName);

}

Ast::Base& Compiler::resolveSubTypeNameAsAst(CellI& subTypeName, CellI& resolveState)
{
    Ast::Struct& currentStruct = static_cast<Ast::Struct&>(resolveState[kb.ids.currentStruct]);
    auto& subTypeAst      = currentStruct.getSubType(subTypeName[kb.ids.name]);
    return subTypeAst;
}

Ast::Base& Compiler::resolveTemplatedType(CellI& ast, CellI& resolveState)
{
    auto& templateId     = ast[kb.ids.id];
    auto& templateParams = ast[kb.ids.parameters];

    Ast::Scope* scopePtr = &static_cast<Ast::Scope&>(resolveState[kb.ids.scope]);
    if (ast.has(kb.ids.scopes)) {
        auto& scopeList = ast[kb.ids.scopes];
        Ast::StructT& astStructT = resolveFullTemplateId(*scopePtr, scopeList, templateId);
        scopePtr = &static_cast<Ast::Scope&>(astStructT[kb.ids.scope]);
    }
    auto& scope = *scopePtr;
    if (!scope.hasItem<Ast::StructT>(templateId)) {
        std::cerr << fmt::format("Unknown template {}", templateId.label()) << std::endl;
        std::cerr << fmt::format("Current scope: {}", scope.label()) << std::endl;
        if (resolveState.has(kb.ids.currentStruct)) {
            std::cerr << fmt::format("Current struct: {}", resolveState[kb.ids.currentStruct].label()) << std::endl;
        }
        if (resolveState.has(kb.ids.currentFn)) {
            std::cerr << fmt::format("Current function: {}", resolveState[kb.ids.currentFn].label()) << std::endl;
        }
        throw fmt::format("Unknown template {}", templateId.label());
    }

    List& resolvedTemplateParams   = *new List(kb, kb.std.Cell, "resolvedTemplateParams");
    List& idCell                   = generateTemplateId(templateId, templateParams, resolveState, resolvedTemplateParams);
    auto& resolvedAstInstance      = resolveTemplateInstanceIdAsAst(idCell, scope, resolveState, ast, resolvedTemplateParams);
    resolvedAstInstance.set("instanceOf", scope.getItem<Ast::StructT>(templateId));
    resolvedAstInstance.set("templateParams", resolvedTemplateParams);
    auto& resolvedCompiledInstance = resolveTemplateInstanceId(resolvedAstInstance.getFullyQualifiedName(), scope, resolveState, ast, resolvedTemplateParams);

    // std::cout << fmt::format("DDDD {} resolved at {:p}\n", idCell.label(), (void*)&resolvedCompiledInstance) << std::endl;

    return resolvedType(resolvedAstInstance, resolvedCompiledInstance);
}

List& Compiler::generateTemplateId(CellI& id, CellI& parameters, CellI& resolveState, List& resolvedParams)
{
    List& idCell = *new List(kb, kb.std.Cell);
    std::stringstream ss;
    Visitor::visitList(id, [this, &idCell, &ss](CellI& character, int, bool&) {
        idCell.add(character);
        ss << character.label();
    });

    ss << "<";
    Visitor::visitList(parameters, [this, &idCell, &ss, &resolveState, &resolvedParams](CellI& slot, int i, bool&) {
        if (i != 0) {
            ss << ", ";
        }
        CellI& key              = slot[kb.ids.key];
        CellI& type             = slot[kb.ids.type];
        CellI& resolvedSlotType = resolveType(type, resolveState);
        CellI& compiledSlotType = getCompiledTypeFromResolvedType(resolvedSlotType);

        idCell.add(key);
        idCell.add(compiledSlotType);
        resolvedParams.add(kb.ast.slot(key, resolvedSlotType));
        ss << fmt::format("{}={}", key.label(), compiledSlotType.label());
    });
    ss << ">";
    idCell.label(ss.str());

    return idCell;
}

Ast::ResolvedType& Compiler::resolvedType(CellI& astType, CellI& compiledType)
{
    return Ast::ResolvedType::New(kb, astType, compiledType);
}

Ast::Base& Compiler::resolveTypeName(Ast::Scope& scope, CellI& typeName)
{
    auto& name = typeName[kb.ids.name];
    static List emptyList(kb, kb.std.Cell, "empty list");
    CellI* scopeListPtr = typeName.has(kb.ids.scopes) ? &typeName[kb.ids.scopes] : &emptyList;
    auto& scopes        = *scopeListPtr;

    auto* struct_ = resolveFullStructName(scope, scopes, name);
    if (struct_) {
        return *struct_;
    }
    auto* enum_ = resolveFullEnumName(scope, scopes, name);
    if (enum_) {
        return *enum_;
    }

    throw "Unknown type name!";
}

Ast::Struct* Compiler::resolveFullStructName(Ast::Scope& scope, CellI& scopeList, CellI& id)
{
    const auto& hasCb = [&id](Ast::Scope& currentScope) -> bool {
        return currentScope.hasItem<Ast::Struct>(id);
    };
    const auto& getCb = [&id](Ast::Scope& currentScope) -> Ast::Struct* {
        return &currentScope.getItem<Ast::Struct>(id);
    };

    return static_cast<Ast::Struct*>(resolveFullNameInAllScope(scope, scopeList, id, hasCb, getCb));
}

Ast::Enum* Compiler::resolveFullEnumName(Ast::Scope& scope, CellI& scopeList, CellI& name)
{
    const auto& hasCb = [&name](Ast::Scope& currentScope) -> bool {
        return currentScope.hasItem<Ast::Enum>(name);
    };
    const auto& getCb = [&name](Ast::Scope& currentScope) -> Ast::Enum* {
        return &currentScope.getItem<Ast::Enum>(name);
    };

    return static_cast<Ast::Enum*>(resolveFullNameInAllScope(scope, scopeList, name, hasCb, getCb));
}

Ast::StructT& Compiler::resolveFullTemplateId(Ast::Scope& scope, CellI& scopeList, CellI& name)
{
    const auto& hasCb = [&name](Ast::Scope& currentScope) -> bool {
        return currentScope.hasItem<Ast::StructT>(name);
    };
    const auto& getCb = [&name](Ast::Scope& currentScope) -> Ast::StructT* {
        return &currentScope.getItem<Ast::StructT>(name);
    };
    Ast::Base* resolvedAst = resolveFullNameInAllScope(scope, scopeList, name, hasCb, getCb);
    if (!resolvedAst) {
        throw "Unknown template name!";
    }

    return static_cast<Ast::StructT&>(*resolvedAst);
}

Ast::Base* Compiler::resolveFullNameInAllScope(Ast::Scope& scope, CellI& scopeList, CellI& id, std::function<bool(Ast::Scope& currentScope)> hasCb, std::function<Ast::Base*(Ast::Scope& currentScope)> getCb)
{
    Ast::Scope* currentScope = &scope;

    while (currentScope) {
        Ast::Base* ret = resolveFullNameInOneScope(currentScope, scopeList, hasCb, getCb);
        if (ret) {
            return ret;
        }
        // resolve in parent scope
        if (currentScope->has(kb.ids.scope)) {
            currentScope = &static_cast<Ast::Scope&>(currentScope->get(kb.ids.scope));
        } else {
            currentScope = nullptr;
        }

    }

    return nullptr;
}

Ast::Base* Compiler::resolveFullNameInOneScope(Ast::Scope* currentScope, CellI& scopeList, std::function<bool(Ast::Scope& currentScope)> hasCb, std::function<Ast::Base*(Ast::Scope& currentScope)> getCb)
{
    // resolve in local scope
    Visitor::visitList(scopeList, [this, &currentScope](CellI& scopeId, int, bool& stop) {
        if (currentScope->hasItem<Ast::Scope>(scopeId)) {
            currentScope = &currentScope->getItem<Ast::Scope>(scopeId);
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

CellI& Compiler::reigisterStructBeforeCompilation(CellI& structAst)
{
    CellI* structIdPtr = nullptr;
    if (&structAst.struct_() == &kb.std.ast.TemplatedType) {
        List& idCell = *new List(kb, kb.std.Cell);
        structIdPtr  = &idCell;
        std::stringstream ss;
        Visitor::visitList(structAst[kb.ids.scopes], [this, &idCell, &ss](CellI& scope, int i, bool&) {
            Visitor::visitList(scope, [this, &idCell, &ss](CellI& character, int, bool&) {
                idCell.add(character);
                ss << character.label();
            });
            idCell.add(kb.pools.chars.get(':'));
            idCell.add(kb.pools.chars.get(':'));
            ss << "::";
        });
        Visitor::visitList(structAst[kb.ids.id], [this, &idCell, &ss](CellI& character, int, bool&) {
            idCell.add(character);
            ss << character.label();
        });

        ss << "<";
        Visitor::visitList(structAst[kb.ids.parameters], [this, &idCell, &ss](CellI& slot, int i, bool&) {
            if (i != 0) {
                ss << ", ";
            }
            CellI& key              = slot[kb.ids.key];
            CellI& type             = slot[kb.ids.type];
            CellI& compiledSlotType = reigisterStructBeforeCompilation(type);

            idCell.add(key);
            idCell.add(compiledSlotType);
            ss << fmt::format("{}={}", key.label(), compiledSlotType.label());
        });
        ss << ">";
        idCell.label(ss.str());
    } else if (&structAst.struct_() == &kb.std.ast.StructName) {
        structIdPtr = &structAst[kb.ids.name];
    } else if (&structAst.struct_() == &kb.std.ast.Cell) {
        return structAst[kb.ids.value];
    } else {
        throw "Unsupported type!";
    }
    CellI& structId = *structIdPtr;
    if (m_earlyStructs.hasKey(structId)) {
        return m_earlyStructs.getValue(structId);
    } else {
        auto& unresolvedStruct = *new Object(kb, kb.std.Struct, fmt::format("{}", structId.label()));
        unresolvedStruct.set("incomplete", kb.boolean.true_);

        m_earlyStructs.add(structId, kb.std.slot(structAst, unresolvedStruct));
        return unresolvedStruct;
    }
}

static void splitNamespacedString(std::vector<std::string>& out, const std::string& input)
{
    const std::string delim = "::";
    std::string leftover    = input;

    while (true) {
        int delim_pos = leftover.find(delim);
        if (delim_pos == std::string::npos) {
            out.push_back(leftover);
            break;
        }
        std::string next_token = leftover.substr(0, delim_pos);
        leftover               = leftover.substr(delim_pos + delim.length());
        out.push_back(next_token);
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
    List& idCell             = *new List(kb, kb.std.Cell);
    const auto& structName   = sliced.back();
    Ast::Scope* currentScope = &kb.globalScope;
    if (sliced.size() > 1) {
        for (int i = 0; i < sliced.size() - 1; ++i) {
            const auto& scopeName = sliced[i];
            currentScope          = &currentScope->getItem<Ast::Scope>(scopeName);
            Visitor::visitList((*currentScope)["name"], [this, &idCell, &ss](CellI& character, int, bool&) {
                idCell.add(character);
                ss << character.label();
            });
            idCell.add(kb.pools.chars.get(':'));
            idCell.add(kb.pools.chars.get(':'));
            ss << "::";
        }
    }
    Ast::Struct& structAst = currentScope->getItem<Ast::Struct>(structName);
    Visitor::visitList(structAst[kb.ids.name], [this, &idCell, &ss](CellI& character, int, bool&) {
        idCell.add(character);
        ss << character.label();
    });
    idCell.label(ss.str());
    m_earlyStructs.add(idCell, kb.std.slot(kb.struct_(fullName), compiledStruct));
}

#if 0
void Ast::Scope::compileTheResolvedAsts(CellI& programData, CellI& state)
{
    auto& scope             = static_cast<Scope&>(state[kb.ids.scope]);
    auto& resolvedScope     = static_cast<Scope&>(state[kb.ids.resolvedScope]);
    auto& compiledFunctions = static_cast<TrieMap&>(programData[kb.ids.functions]);
    auto& compiledStructs   = static_cast<TrieMap&>(programData[kb.ids.structs]);
    auto& compiledVariables = static_cast<TrieMap&>(programData[kb.ids.variables]);

    if (scope.has("functions")) {
        Visitor::visitList(resolvedScope.items<Function>()[kb.ids.list], [this, &state, &compiledFunctions](CellI& function, int i, bool& stop) {
            Function& astFunction = static_cast<Function&>(function[kb.ids.value]);
            auto& compiledFunction     = astFunction.compile(state);
            compiledFunctions.add(astFunction.getFullyQualifiedName(), compiledFunction);
        });
    }
    if (scope.has("structs")) {
        Visitor::visitList(resolvedScope.items<Struct>()[kb.ids.list], [this, &state, &compiledStructs](CellI& struct_, int i, bool& stop) {
            Struct& astStruct    = static_cast<Struct&>(struct_[kb.ids.value]);
            auto& compiledStruct = astStruct.compile(state);
            CellI& structFullName = astStruct.getFullyQualifiedName();
            if (compiledStruct.has("subTypes")) {
                CellI& subTypesIndex = compiledStruct["subTypes"][kb.ids.index];
                Visitor::visitList(subTypesIndex[kb.ids.struct_][kb.ids.slots][kb.ids.list], [this, &structFullName, &subTypesIndex, &compiledStructs](CellI& subType, int i, bool& stop) {
                    CellI& key = subType["key"];
                    CellI& value = subTypesIndex[key][kb.ids.value];
                    List& aliasName = *new List(kb, kb.std.Char);
                    Visitor::visitList(structFullName, [&aliasName](CellI& character, int i, bool& stop) {
                        aliasName.add(character);
                    });
                    aliasName.add(kb.pools.chars.get(':'));
                    aliasName.add(kb.pools.chars.get(':'));
                    Visitor::visitList(key, [&aliasName](CellI& character, int i, bool& stop) {
                        aliasName.add(character);
                    });
                    aliasName.label(structFullName.label() + "::" + key.label());
                    if (IS_LOG_ENABLED) {
                        TRACE(compileStruct, "{}: {}\n", aliasName.label(), value.label());
                    }
                    compiledStructs.add(aliasName, value);
                });
            }
            compiledStructs.add(structFullName, compiledStruct);
        });
    }
    if (scope.has("enums")) {
        Visitor::visitList(resolvedScope.items<Enum>()[kb.ids.list], [this, &state, &compiledStructs](CellI& enum_, int i, bool& stop) {
            Enum& astEnum        = static_cast<Enum&>(enum_[kb.ids.value]);
            auto& compiledStruct = astEnum.compile(state);
            compiledStructs.add(astEnum.getFullyQualifiedName(), compiledStruct);
        });
    }
    if (scope.has("variables")) {
        Visitor::visitList(resolvedScope.items<Var>()[kb.ids.list], [this, &compiledVariables](CellI& var, int i, bool& stop) {
            Var& astVar       = static_cast<Var&>(var[kb.ids.value]);
            auto& varName          = astVar.getFullyQualifiedName();
            auto& compiledVariable = *new Object(kb, kb.std.op.Var, fmt::format("var {}", astVar.label()));
            compiledVariables.add(varName, compiledVariable);
        });
    }
    if (scope.has("scopes")) {
        Visitor::visitList(items<Scope>()[kb.ids.list], [this, &programData, &state, &resolvedScope](CellI& scope, int i, bool& stop) {
            Scope& astScope = static_cast<Scope&>(scope[kb.ids.value]);
            state.set("scope", astScope);
            state.set("resolvedScope", resolvedScope.getItem<Scope>(astScope[kb.ids.name]));
            astScope.compileTheResolvedAsts(programData, state);
            state.set("scope", *this);
            state.set("resolvedScope", resolvedScope);
        });
    }
}

void Ast::Scope::processDescriptionsInAsts(CellI& programData, CellI& state)
{
    auto& compiledStructs = static_cast<TrieMap&>(programData[kb.ids.structs]);
    m_toolFinder           = new ToolFinder(kb);
    ToolFinder& toolFinder = *m_toolFinder;
    auto& astScope = getItem<Scope>("std").getItem<Scope>("ast");
    if (astScope.has("structs")) {
        Visitor::visitList(astScope.items<Struct>()[kb.ids.list], [this, &toolFinder, &compiledStructs](CellI& struct_, int i, bool& stop) {
            Struct& tool = static_cast<Struct&>(struct_[kb.ids.value]);
            if (tool.has(kb.ids.description) && tool[kb.ids.description].has(kb.ids.asts)) {
                auto& compiledAstStruct = compiledStructs.getValue(static_cast<Ast::Struct&>(tool).getFullyQualifiedName());
                toolFinder.add(tool, compiledAstStruct);
            }
        });
    }
}

Ast::Struct& Ast::Struct::resolveTypes(CellI& state)
{
    auto& structName     = get("name");
    auto& structs        = static_cast<TrieMap&>(state[kb.ids.structs]);
    auto& unknownStructs = static_cast<TrieMap&>(state[kb.ids.unknownStructs]);
    Ast::Struct& ret     = *new Ast::Struct(kb, structName);

    if (has("instanceOf")) {
        ret.set("instanceOf", get("instanceOf"));
        ret.set("templateParams", get("templateParams"));
        ret.set("scope", get("scope"));
    }

    auto& fullyQualifiedName = getFullyQualifiedName();
    CellI* compiledStructPtr = nullptr;
    if (unknownStructs.hasKey(fullyQualifiedName)) {
        CellI& unknownStruct = unknownStructs.getValue(fullyQualifiedName);
        compiledStructPtr    = &unknownStruct["value"];
        unknownStructs.remove(fullyQualifiedName);
    } else {
        compiledStructPtr = new Object(kb, kb.std.Struct, fmt::format("{}", fullyQualifiedName.label()));
    }
    auto& compiledStruct = *compiledStructPtr;
    structs.add(getFullyQualifiedName(), compiledStruct);
    ret.set("compiledStruct", compiledStruct);

    state.set("currentStruct", ret);

    std::stringstream ss;
    std::vector<std::string> subTypesStrs;

    if (IS_LOG_ENABLED) {
    }

    // resolve sub types
    if (has("subTypes")) {
        Visitor::visitList(subTypes()[kb.ids.list], [this, &ret, &state, &subTypesStrs](CellI& subTypeCell, int i, bool& stop) {
            CellI& subTypeId           = subTypeCell[kb.ids.key];
            CellI& subTypeType         = subTypeCell[kb.ids.type];
            CellI& resolvedSubTypeType = resolveType(subTypeType, state);
            ret.subTypes(kb.ast.slot(subTypeId, resolvedSubTypeType));
            if (IS_LOG_ENABLED) {
                subTypesStrs.push_back(fmt::format("    type {} = {};", subTypeId.label(), getCompiledTypeFromResolvedType(resolvedSubTypeType).label()));
            }
        });
    }

    // resolve memberOf list
    if (has("memberOf")) {
        ss << " : ";
        Visitor::visitList(memberOf(), [this, &ret, &state, &ss](CellI& membershipType, int i, bool& stop) {
            CellI& resolvedMembershipType = resolveType(membershipType, state);
            if (i > 0) {
                ss << ", ";
            }
            ss << getCompiledTypeFromResolvedType(resolvedMembershipType).label();
            ret.memberOf(resolvedMembershipType);
        });
    }
    if (IS_LOG_ENABLED) {
        TRACE(compileStruct, "struct {}{}", label(), ss.str());
        TRACE(compileStruct, "{");
        for (const auto& subTypeStr : subTypesStrs) {
            TRACE(compileStruct, subTypeStr);
        }
        if (!subTypesStrs.empty() && (has("methods") || has("members"))) {
            TRACE(compileStruct, "");
        }
    }

    // resolve methods
    if (has("methods")) {
        Visitor::visitList(methods()[kb.ids.list], [this, &ret, &state](CellI& origAstFunctionCell, int i, bool& stop) {
            auto& origAstFunction     = static_cast<Ast::Function&>(origAstFunctionCell);
            auto& resolvedAstFunction = origAstFunction.resolveTypes(state);
            ret.addMethod(resolvedAstFunction);
            TRACE(compileStruct, "    {};", resolvedAstFunction.shortName());
        });
        if (IS_LOG_ENABLED) {
            if (has("members")) {
                TRACE(compileStruct, "");
            }
        }
    }
    // resolve members
    if (has("members")) {
        CellI& membersList = members()[kb.ids.list];
        Visitor::visitList(membersList, [this, &ret, &state](CellI& memberCell, int i, bool& stop) {
            CellI& memberId           = memberCell[kb.ids.key];
            CellI& memberType         = memberCell[kb.ids.type];
            CellI& resolvedMemberType = resolveType(memberType, state);
            ret.members(kb.ast.slot(memberId, resolvedMemberType));
            if (label() == "ListItem" && memberId.label() == "value")
            {
                CellI& compiledType = getCompiledTypeFromResolvedType(resolvedMemberType);
                std::cout << "";
            }
            TRACE(compileStruct, "    {}: {};", memberId.label(), getCompiledTypeFromResolvedType(resolvedMemberType).label());
        });
    }

    TRACE(compileStruct, "}");
    TRACE(compileStruct, "");

    return ret;
}

CellI& Ast::Struct::compile(CellI& state)
{
    CellI& compiledStruct = getResolvedTypeById(getFullyQualifiedName(), has("instanceOf"), state);
    compiledStruct.erase("incomplete");
    // std::cout << fmt::format("DDDD compile {} resolved at {:p}\n", getFullId().label(), (void*)&compiledStruct) << std::endl;

    // compile sub types
    if (has("subTypes")) {
        Map& compiledSubTypes = *new Map(kb, kb.std.Cell, kb.std.Struct, "subTypes Map<Cell, Type>(...)");
        Visitor::visitList(subTypes()[kb.ids.list], [this, &compiledSubTypes](CellI& slot, int i, bool& stop) {
            CellI& key             = slot[kb.ids.key];
            CellI& type            = slot[kb.ids.type];
            auto& compiledSlotType = getCompiledTypeFromResolvedType(type);
            compiledSubTypes.add(key, compiledSlotType);
        });
        compiledStruct.set("subTypes", compiledSubTypes);
    }

    // compile methods
    if (has("methods")) {
        Map& compiledMethods = *new Map(kb, kb.std.Cell, kb.std.ast.Function);
        Visitor::visitList(methods()[kb.ids.list], [this, &compiledMethods, &state](CellI& astFunction, int i, bool& stop) {
            auto& compiledFunction = static_cast<Ast::Function&>(astFunction).compile(state);
            compiledMethods.add(astFunction[kb.ids.name], compiledFunction);
        });
        compiledStruct.set("methods", compiledMethods);
    }

    // compile members
    if (has("members")) {
        Map& compiledMembers = *new Map(kb, kb.std.Cell, kb.std.Slot, "members Map<Cell, Slot>(...)");
        Visitor::visitList(members()[kb.ids.list], [this, &compiledMembers, &compiledStruct, &state](CellI& slot, int i, bool& stop) {
            CellI& key             = slot[kb.ids.key];
            CellI& type            = slot[kb.ids.type];
            auto& compiledSlotType = getCompiledTypeFromResolvedType(type);
            compiledMembers.add(key, kb.std.slot(key, compiledSlotType));
        });
        compiledStruct.set("slots", compiledMembers);
    }

    // compile memberOf list
    if (has("memberOf")) {
        Map& compiledMemberOfs = *new Map(kb, kb.std.Struct, kb.std.Struct, "memberOf Map<Type, Type>(...)");
        Visitor::visitList(memberOf(), [this, &compiledMemberOfs](CellI& membershipType, int i, bool& stop) {
            auto& compiledMembershipType = getCompiledTypeFromResolvedType(membershipType);
            compiledMemberOfs.add(compiledMembershipType, compiledMembershipType);
        });
        compiledStruct.set("memberOf", compiledMemberOfs);
    }

    return compiledStruct;
}

Ast::Struct& Ast::StructT::instantiateWith(List& inputParams, CellI& state)
{
    // process input parameters
    if (!inputParams.empty() && missing("templateParams")) {
        throw "No template parameter was given!";
    }
    std::stringstream ss;
    Map inputParameters(kb, kb.std.Cell, kb.std.Cell);
    List& idCell = *new List(kb, kb.std.Cell);
    Visitor::visitList(name(), [this, &idCell](CellI& character, int i, bool& stop) {
        idCell.add(character);
    });

    Visitor::visitList(inputParams, [this, &inputParameters, &ss, &idCell](CellI& slot, int i, bool& stop) {
        CellI& key  = slot[kb.ids.key];
        CellI& type = slot[kb.ids.type];
        inputParameters.add(key, type);
        CellI& compiledSlotType = getCompiledTypeFromResolvedType(type);
        if (i != 0) {
            ss << ", ";
        }
        ss << key.label() << "=" << compiledSlotType.label();
        if (!templateParams().hasKey(key)) {
            throw "Instantiating with unknown template parameter!";
        }
        idCell.add(key);
        idCell.add(compiledSlotType);
    });
    idCell.label(fmt::format("{}<{}>", name().label(), ss.str()));
    Ast::Struct* retPtr = nullptr;
    auto& unknownInstanceAsts = static_cast<TrieMap&>(state["unknownInstanceAsts"]);
    if (unknownInstanceAsts.hasKey(idCell)) {
        retPtr = &static_cast<Ast::Struct&>(unknownInstanceAsts.getValue(idCell)[kb.ids.value]);
     } else {
        auto& instanceAsts = static_cast<TrieMap&>(state["instanceAsts"]);
        retPtr             = new Ast::Struct(kb, idCell);
        instanceAsts.add(idCell, *retPtr);
    }
    Ast::Struct& ret = *retPtr;
    ret.set("instanceOf", *this);
    ret.set("templateParams", inputParams);
    ret.set("scope", static_cast<Scope&>(get("scope")));

    // instantiate sub types
    if (has("subTypes")) {
        Map& instantiatedSubTypes = *new Map(kb, kb.std.Cell, kb.std.ast.Base);
        Visitor::visitList(subTypes()[kb.ids.list], [this, &inputParameters, &instantiatedSubTypes, &ret, &state](CellI& slot, int i, bool& stop) {
            CellI& key               = slot[kb.ids.key];
            CellI& type              = slot[kb.ids.type];
            CellI& instantiatedParam = instantiateTemplateParamType(type, ret, inputParameters, state);
            instantiatedSubTypes.add(key, kb.ast.slot(key, instantiatedParam));
        });
        ret.set("subTypes", instantiatedSubTypes);
    }

    // instantiate methods
    if (has("methods")) {
        Visitor::visitList(methods()[kb.ids.list], [this, &inputParameters, &ret, &state](CellI& astFunction, int i, bool& stop) {
            Ast::Function& instantiedFunction = *new Function(kb, astFunction[kb.ids.name]);
            ret.addMethod(instantiedFunction);

            // parameters
            if (astFunction.has("parameters")) {
                List& instantiatedParameters = *new List(kb, kb.std.Slot);
                Visitor::visitList(astFunction[kb.ids.parameters], [this, &inputParameters, &instantiatedParameters, &ret, &state](CellI& slot, int i, bool& stop) {
                    CellI& key               = slot[kb.ids.key];
                    CellI& type              = slot[kb.ids.type];
                    CellI& instantiatedParam = instantiateTemplateParamType(type, ret, inputParameters, state);
                    instantiatedParameters.add(kb.ast.slot(key, instantiatedParam));
                });
                instantiedFunction.set("parameters", instantiatedParameters);
            }
            // return type
            if (astFunction.has("returnType")) {
                CellI& returnType             = astFunction[kb.ids.returnType];
                CellI& instantiatedReturnType = instantiateTemplateParamType(returnType, ret, inputParameters, state);
                instantiedFunction.set("returnType", instantiatedReturnType);
            }
            // instructions
            if (astFunction.has("instructions")) {
                instantiedFunction.set("instructions", instantiateAst(astFunction[kb.ids.instructions], ret, inputParameters, state));
            }
        });
    }

    // instantiate members
    if (has("members")) {
        Map& instantiatedMembers = *new Map(kb, kb.std.Cell, kb.std.Slot);
        Visitor::visitList(members()[kb.ids.list], [this, &inputParameters, &instantiatedMembers, &ret, &state](CellI& slot, int i, bool& stop) {
            CellI& key               = slot[kb.ids.key];
            CellI& type              = slot[kb.ids.type];
            CellI& instantiatedParam = instantiateTemplateParamType(type, ret, inputParameters, state);
            instantiatedMembers.add(key, kb.ast.slot(key, instantiatedParam));
        });
        ret.set("members", instantiatedMembers);
    }

    // instantiate memberOf list
    if (has("memberOf")) {
        List& instantiatedMemberOfs = *new List(kb, kb.std.Struct);
        Visitor::visitList(memberOf(), [this, &inputParameters, &instantiatedMemberOfs, &ret, &state](CellI& membershipType, int i, bool& stop) {
            CellI& instantiatedParam = instantiateTemplateParamType(membershipType, ret, inputParameters, state);
            instantiatedMemberOfs.add(instantiatedParam);
        });
        ret.set("memberOf", instantiatedMemberOfs);
    }

    return ret;
}

CellI& Ast::StructT::instantiateTemplateParamType(CellI& param, CellI& selfType, Map& inputParameters, CellI& state)
{
    if (&param.struct_() == &kb.std.ast.TemplateParam) {
        CellI& paramValue = param[kb.ids.key];
        if (!inputParameters.hasKey(paramValue)) {
            throw "Instantiating with unknown template parameter!";
        }
        return inputParameters.getValue(paramValue);
    }
    if (&param.struct_() == &kb.std.ast.TemplatedType) {
        List& resolvedParameterList = *new List(kb, kb.std.Slot);
        auto& ret                   = *new TemplatedType(kb, param[kb.ids.id], resolvedParameterList);
        auto& parametersList        = param[kb.ids.parameters];

        Visitor::visitList(parametersList, [this, &resolvedParameterList, &selfType, &inputParameters, &state](CellI& slot, int, bool&) {
            CellI& key              = slot[kb.ids.key];
            CellI& type             = slot[kb.ids.type];
            CellI& resolvedSlotType = instantiateTemplateParamType(type, selfType, inputParameters, state);
            resolvedParameterList.add(kb.ast.slot(key, resolvedSlotType));
        });

        return ret;
    }
    if (&param.struct_() == &kb.std.ast.Cell || &param.struct_() == &kb.std.ast.StructName || &param.struct_() == &kb.std.ast.SubTypeName) {
        return param;
    }

    throw "Unknown template parameter!";
}

Ast::Base& Ast::StructT::instantiateAst(CellI& ast, CellI& selfType, Map& inputParameters, CellI& state)
{
    auto instantiate = [this, &selfType, &inputParameters, &state](CellI& ast) -> Ast::Base& { return instantiateAst(ast, selfType, inputParameters, state); };

    if (&ast.struct_() == &kb.std.ast.New) {
        auto* objectTypePtr = &ast[kb.ids.objectType];
        if (&(*objectTypePtr).struct_() == &kb.std.ast.TemplatedType) {
            CellI& resolvedObjectType = instantiateTemplateParamType(*objectTypePtr, selfType, inputParameters, state);
            objectTypePtr             = &resolvedObjectType;
        }
        auto& objectType = *static_cast<Base*>(objectTypePtr);
        if (ast.has("constructor")) {
            auto& constructor = ast[kb.ids.constructor];
            Ast::Base& ret    = kb.ast.new_(objectType, static_cast<Base&>(constructor));
            if (ast.has("parameters")) {
                auto& newParameters = *new cells::List(kb, kb.std.ast.Slot);
                Visitor::visitList(ast[kb.ids.parameters], [this, &newParameters, &instantiate](CellI& slot, int, bool&) {
                    newParameters.add(kb.ast.slot(slot[kb.ids.key], instantiate(slot[kb.ids.type])));
                });
                ret.set("parameters", newParameters);
            }
            return ret;
        } else {
            return kb.ast.new_(objectType);
        }
    } else if (&ast.struct_() == &kb.std.ast.Call) {
        Ast::Base& ret = kb.ast.call(instantiate(ast[kb.ids.cell]), ast[kb.ids.method]);
        if (ast.has("parameters")) {
            // TODO process parameters
            ret.set("parameters", ast[kb.ids.parameters]);
        }
        return ret;
    } else if (&ast.struct_() == &kb.std.ast.StaticCall) {
        Ast::Base& ret = kb.ast.scall(instantiate(ast[kb.ids.cell]), ast[kb.ids.method]);
        if (ast.has("parameters")) {
            // TODO process parameters
            ret.set("parameters", ast[kb.ids.parameters]);
        }
        return ret;
    } 

    // do nothing just traverse and copy the AST nodes
    if (&ast.struct_() == &kb.std.ast.Block) {
        auto& instantiedAsts = *new cells::List(kb, kb.std.ast.Base);
        Visitor::visitList(ast[kb.ids.asts], [this, &instantiedAsts, &instantiate](CellI& ast, int, bool&) {
            instantiedAsts.add(instantiate(ast));
        });
        return *new Block(kb, instantiedAsts);
    } else if (&ast.struct_() == &kb.std.ast.Cell) {
        return kb.ast.cell(ast[kb.ids.value]);
    } else if (&ast.struct_() == &kb.std.ast.StructName) {
        auto& ret = kb.ast.structName(ast[kb.ids.value]);
        if (ast.has(kb.ids.scopes)) {
            ret.set(kb.ids.scopes, ast[kb.ids.scopes]);
        }
        return ret;
    } else if (&ast.struct_() == &kb.std.ast.SelfFn) {
        return kb.ast.selfFn();
    } else if (&ast.struct_() == &kb.std.ast.Self) {
        return kb.ast.self();
    } else if (&ast.struct_() == &kb.std.ast.Continue) {
        return kb.ast.continue_();
    } else if (&ast.struct_() == &kb.std.ast.Break) {
        return kb.ast.break_();
    } else if (&ast.struct_() == &kb.std.ast.Try) {
        return kb.ast.try_(instantiate(ast["tryBranch"]), instantiate(ast["catchBranch"]));
    } else if (&ast.struct_() == &kb.std.ast.Throw) {
        if (ast.has(kb.ids.value)) {
            return kb.ast.throw_(instantiate(ast[kb.ids.value]));
        } else {
            return kb.ast.throw_();
        }
    } else if (&ast.struct_() == &kb.std.ast.Parameter) {
        return kb.ast.parameter(ast[kb.ids.key]);
    } else if (&ast.struct_() == &kb.std.ast.Var) {
        return kb.ast.var(ast[kb.ids.name]);
    } else if (&ast.struct_() == &kb.std.ast.Delete) {
        return kb.ast.delete_(instantiate(ast[kb.ids.cell]));
    } else if (&ast.struct_() == &kb.std.ast.Set) {
        return kb.ast.set(instantiate(ast[kb.ids.cell]), instantiate(ast[kb.ids.key]), instantiate(ast[kb.ids.value]));
    } else if (&ast.struct_() == &kb.std.ast.Erase) {
        return kb.ast.erase(instantiate(ast[kb.ids.cell]), instantiate(ast[kb.ids.key]));
    } else if (&ast.struct_() == &kb.std.ast.If) {
        if (ast.has("else_")) {
            return kb.ast.if_(instantiate(ast[kb.ids.condition]))
                .then_(instantiate(ast[kb.ids.then]))
                .else_(instantiate(ast[kb.ids.else_]));
        } else {
            return kb.ast.if_(instantiate(ast[kb.ids.condition])).then_(instantiate(ast[kb.ids.then]));
        }
    } else if (&ast.struct_() == &kb.std.ast.Do) {
        return kb.ast.do_(instantiate(ast[kb.ids.statement])).while_(instantiate(ast[kb.ids.condition]));
    } else if (&ast.struct_() == &kb.std.ast.While) {
        return kb.ast.while_(instantiate(ast[kb.ids.condition])).do_(instantiate(ast[kb.ids.statement]));
    } else if (&ast.struct_() == &kb.std.ast.And) {
        return kb.ast.and_(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.struct_() == &kb.std.ast.Or) {
        return kb.ast.or_(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.struct_() == &kb.std.ast.Not) {
        return kb.ast.not_(instantiate(ast[kb.ids.input]));
    } else if (&ast.struct_() == &kb.std.ast.Add) {
        return kb.ast.add(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.struct_() == &kb.std.ast.Subtract) {
        return kb.ast.subtract(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.struct_() == &kb.std.ast.Multiply) {
        return kb.ast.multiply(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.struct_() == &kb.std.ast.Divide) {
        return kb.ast.divide(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.struct_() == &kb.std.ast.LessThan) {
        return kb.ast.lessThan(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.struct_() == &kb.std.ast.LessThanOrEqual) {
        return kb.ast.lessThanOrEqual(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.struct_() == &kb.std.ast.GreaterThan) {
        return kb.ast.greaterThan(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.struct_() == &kb.std.ast.GreaterThanOrEqual) {
        return kb.ast.greaterThanOrEqual(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.struct_() == &kb.std.ast.Same) {
        return kb.ast.same(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.struct_() == &kb.std.ast.NotSame) {
        return kb.ast.notSame(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.struct_() == &kb.std.ast.Equal) {
        return kb.ast.equal(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.struct_() == &kb.std.ast.NotEqual) {
        return kb.ast.notEqual(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.struct_() == &kb.std.ast.Has) {
        return kb.ast.has(instantiate(ast[kb.ids.cell]), instantiate(ast[kb.ids.key]));
    } else if (&ast.struct_() == &kb.std.ast.Missing) {
        return kb.ast.missing(instantiate(ast[kb.ids.cell]), instantiate(ast[kb.ids.key]));
    } else if (&ast.struct_() == &kb.std.ast.Get) {
        return kb.ast.get(instantiate(ast[kb.ids.cell]), instantiate(ast[kb.ids.key]));
    } else if (&ast.struct_() == &kb.std.ast.Member) {
        return kb.ast.member(ast[kb.ids.key]);
    } else if (&ast.struct_() == &kb.std.ast.Return) {
        if (ast.has("value")) {
            return kb.ast.return_(static_cast<Base&>(instantiate(ast[kb.ids.value])));
        }
        return kb.ast.return_();
    }

    throw "Unknown AST to instantiate!";
}

Ast::Enum& Ast::Enum::resolveTypes(CellI& state)
{
    auto& enumName       = get("name");
    auto& structs        = static_cast<TrieMap&>(state[kb.ids.structs]);
    auto& unknownStructs = static_cast<TrieMap&>(state[kb.ids.unknownStructs]);
    Ast::Enum& ret       = *new Ast::Enum(kb, enumName);

    if (has("instanceOf")) {
        ret.set("instanceOf", get("instanceOf"));
        ret.set("templateParams", get("templateParams"));
        ret.set("scope", get("scope"));
    }

    auto& fullyQualifiedName = getFullyQualifiedName();
    CellI* resolvedStructPtr = nullptr;
    if (unknownStructs.hasKey(fullyQualifiedName)) {
        CellI& unknownStruct = unknownStructs.getValue(fullyQualifiedName);
        resolvedStructPtr    = &unknownStruct["value"];
        unknownStructs.remove(fullyQualifiedName);
    } else {
        resolvedStructPtr = new Object(kb, kb.std.Struct, fmt::format("{}", fullyQualifiedName.label()));
    }
    auto& resolvedStruct = *resolvedStructPtr;
    structs.add(getFullyQualifiedName(), resolvedStruct);

    state.set("currentStruct", ret);

    TRACE(compileStruct, "enum {} {{", label());

#if 0
    // resolve methods
    if (has("methods")) {
        Visitor::visitList(methods()[kb.ids.list], [this, &ret, &state](CellI& origAstFunctionCell, int i, bool& stop) {
            auto& origAstFunction     = static_cast<Ast::Function&>(origAstFunctionCell);
            auto& resolvedAstFunction = origAstFunction.resolveTypes(state);
            ret.addMethod(resolvedAstFunction);
            if (debugCompiledStructs) {
                std::cout << fmt::format("    {};\n", resolvedAstFunction.shortName());
            }
        });
        if (debugCompiledStructs) {
            if (has("members")) {
                std::cout << std::endl;
            }
        }
    }
#endif
    // resolve values
    if (has("values")) {
        CellI& valuesList = values()[kb.ids.list];
        Visitor::visitList(valuesList, [this, &ret, &state](CellI& kvPair, int i, bool& stop) {
            CellI& valueCell = kvPair[kb.ids.value];
            CellI& valueName = valueCell[kb.ids.name];
            std::stringstream ss;
            if (valueCell.has("enumType")) {
                CellI& valueType               = valueCell["enumType"];
                CellI& resolvedValueType       = resolveType(valueType, state);
                TypedEnumValue& typedEnumValue = *new TypedEnumValue(kb, valueName, resolvedValueType);
                if (valueCell.has(kb.ids.value)) {
                    typedEnumValue.set(kb.ids.value, valueCell[kb.ids.value]);
                }
                ret.values(typedEnumValue);
                if (IS_LOG_ENABLED) {
                    ss << fmt::format("    {}({})", valueName.label(), getCompiledTypeFromResolvedType(resolvedValueType).label());
                }
            } else {
                ret.values(static_cast<Base&>(valueCell));
                ss << fmt::format("    {}", valueName.label());
            }
            if (IS_LOG_ENABLED) {
                if (valueCell.has(kb.ids.value)) {
                    ss << fmt::format(" = {}", resolveEnumValue(valueCell[kb.ids.value]).label());
                }
                ss << ",";
                TRACE(compileStruct, ss.str());
            }
            });
    }
    TRACE(compileStruct, "}");

    return ret;
}

CellI& Ast::Enum::compile(CellI& state)
{
    CellI& compiledStruct = getResolvedTypeById(getFullyQualifiedName(), has("instanceOf"), state);
    auto& compiledVariables = static_cast<TrieMap&>(state[kb.ids.variables]);

    compiledStruct.erase("incomplete");
    compiledStruct.set("enum", kb.boolean.true_);
    // std::cout << fmt::format("DDDD compile {} resolved at {:p}\n", getFullId().label(), (void*)&compiledStruct) << std::endl;

#if 0
    // compile methods
    if (has("methods")) {
        Map& compiledMethods = *new Map(kb, kb.std.Cell, kb.std.ast.Function);
        Visitor::visitList(methods()[kb.ids.list], [this, &compiledMethods, &state](CellI& astFunction, int i, bool& stop) {
            auto& compiledFunction = static_cast<Ast::Function&>(astFunction).compile(state);
            compiledMethods.add(astFunction[kb.ids.name], compiledFunction);
        });
        compiledStruct.set("methods", compiledMethods);
    }
#endif

    // compile values
    if (has("values")) {
        Map& compiledMembers = *new Map(kb, kb.std.Cell, kb.std.Slot, "members Map<Cell, Slot>(...)");
        Visitor::visitList(values()[kb.ids.list], [this, &state, &compiledMembers, &compiledVariables, &compiledStruct](CellI& kvPair, int i, bool& stop) {
            CellI& valueKey  = kvPair[kb.ids.key];
            CellI& valueCell = kvPair[kb.ids.value];
            CellI& valueName = valueCell[kb.ids.name];
            if (&valueCell.struct_() == &kb.std.ast.EnumValue) {
                auto& enumValue     = static_cast<EnumValue&>(valueCell);
                auto& fullName      = enumValue.getFullyQualifiedName();
                if (valueCell.has(kb.ids.value)) {
                    auto& value         = enumValue[kb.ids.value];
                    auto& resolvedValue = resolveEnumValue(value);
                    auto& valueType     = resolvedValue.struct_();

                    auto& compiledValue = *new Object(kb, compiledStruct, fmt::format("{}::{}", label(), enumValue.label()));
                    compiledValue.set("tag", valueName);
                    compiledValue.set(valueName, value[kb.ids.value]);
                    compiledVariables.add(fullName, compiledValue);
                    compiledMembers.add(valueKey, valueType);
                } else {
                    auto& compiledValue = *new Object(kb, compiledStruct, fmt::format("{}::{}", label(), enumValue.label()));
                    compiledValue.set("tag", valueName);
                    compiledValue.set(valueName, kb.ids.emptyObject);
                    compiledVariables.add(fullName, compiledValue);
                    compiledMembers.add(valueKey, compiledStruct);
                }
            } else if (&valueCell.struct_() == &kb.std.ast.TypedEnumValue) {
                auto& enumValue             = static_cast<TypedEnumValue&>(valueCell);
                auto& enumValueType         = valueCell["enumType"];
                auto& compiledEnumValueType = getCompiledTypeFromResolvedType(enumValueType);
                auto& fullName              = enumValue.getFullyQualifiedName();
                compiledMembers.add(valueKey, compiledEnumValueType);
            }
        });
        compiledStruct.set("slots", compiledMembers);
    }

    return compiledStruct;
}

CellI& Ast::Enum::resolveEnumValue(CellI& ast)
{
    if (&ast.struct_() == &kb.std.ast.Cell) {
        return ast[kb.ids.value];
    }

    throw "Unknown enum value!";
}

Ast::Function& Ast::Function::resolveTypes(CellI& state)
{
    Ast::Function& ret = *new Ast::Function(kb, get("name"));
    state.set("currentFn", ret);
    std::stringstream ss;

    if (has("static_")) {
        ret.set("static_", get("static_"));
        ss << "static ";
    }
    ss << "fn";
    if (has("structType")) {
        auto& structType = get("structType");
        ret.set("structType", structType);
        ss << fmt::format(" {}::{}(", structType.label(), get("name").label());
    } else {
        ss << fmt::format(" {}(", get("name").label());
    }

    if (has("parameters")) {
        Visitor::visitList(parameters(), [this, &state, &ret, &ss](CellI& param, int i, bool& stop) {
            CellI& paramId           = param[kb.ids.key];
            CellI& paramType         = param[kb.ids.type];
            CellI& resolvedParamType = resolveType(paramType, state);
            CellI& compiledParamType = getCompiledTypeFromResolvedType(resolvedParamType);
            if (i > 0) {
                ss << ", ";
            }
            ss << fmt::format("{}: {}", paramId.label(), compiledParamType.label());
            ret.parameters(kb.ast.slot(paramId, resolvedParamType));
        });
    }
    ss << ")";
    if (has("returnType")) {
        ss << " -> ";
        CellI& retType = returnType();
        CellI& resolvedRetType = resolveType(retType, state);
        CellI& compiledRetType = getCompiledTypeFromResolvedType(resolvedRetType);
        ss << compiledRetType.label();
        ret.returnType(resolvedRetType);
    }
    ret.label(ss.str());

    CellI& resolvedinstructionsAst = resolveTypesInCode(state, instructions());
    ret.set("instructions", resolvedinstructionsAst);

    return ret;
}

Ast::Base& Ast::Function::resolveTypesInCode(CellI& resolveState, CellI& ast)
{
    auto resolveNode = [this, &resolveState](CellI& ast) -> Ast::Base& { return resolveTypesInCode(resolveState, ast); };

    if (&ast.struct_() == &kb.std.ast.New) {
        CellI& objectType                = ast[kb.ids.objectType];
        Ast::Base* resolvedObjectTypePtr = nullptr;
        if (&objectType.struct_() == &kb.std.ast.Member) {
            resolvedObjectTypePtr = &static_cast<Ast::Base&>(objectType);
        } else {
            resolvedObjectTypePtr = &resolveType(objectType, resolveState);
        }
        Ast::Base& resolvedObjectType = *resolvedObjectTypePtr;

        if (ast.has("constructor")) {
            auto& constructor = ast[kb.ids.constructor];
            Ast::Base& ret    = kb.ast.new_(resolvedObjectType, static_cast<Base&>(ast[kb.ids.constructor]));

            if (ast.has("parameters")) {
                auto& newParameters = *new cells::List(kb, kb.std.ast.Slot);
                Visitor::visitList(ast[kb.ids.parameters], [this, &newParameters, &resolveNode, &resolveState](CellI& slot, int, bool&) {
                    newParameters.add(kb.ast.slot(slot[kb.ids.key], resolveNode(slot[kb.ids.type])));
                });
                ret.set("parameters", newParameters);
            }
            return ret;
        } else {
            return kb.ast.new_(resolvedObjectType);
        }
    }

    if (&ast.struct_() == &kb.std.ast.Call || &ast.struct_() == &kb.std.ast.StaticCall) {

        Ast::Base* retPtr = nullptr;
        if (&ast.struct_() == &kb.std.ast.Call) {
            retPtr = &kb.ast.call(resolveNode(ast[kb.ids.cell]), ast[kb.ids.method]);
        } else {
            retPtr = &kb.ast.scall(resolveType(ast[kb.ids.cell], resolveState), ast[kb.ids.method]);
        }
        Ast::Base& ret = *retPtr;
        if (ast.has("parameters")) {
            auto& newParameters = *new cells::List(kb, kb.std.ast.Slot);
            Visitor::visitList(ast[kb.ids.parameters], [this, &newParameters, &resolveNode, &resolveState](CellI& slot, int, bool&) {
                newParameters.add(kb.ast.slot(slot[kb.ids.key], resolveNode(slot[kb.ids.type])));
            });
            ret.set("parameters", newParameters);
        }
        return ret;
    } else if (&ast.struct_() == &kb.std.ast.StructName) {
        return resolveType(ast, resolveState);
    }

    // do nothing just traverse and copy the AST nodes
    if (&ast.struct_() == &kb.std.ast.Block) {
        auto& instantiedAsts = *new cells::List(kb, kb.std.ast.Base);
        Visitor::visitList(ast[kb.ids.asts], [this, &instantiedAsts, &resolveNode](CellI& ast, int, bool&) {
            instantiedAsts.add(resolveNode(ast));
        });
        return *new Block(kb, instantiedAsts);
    } else if (&ast.struct_() == &kb.std.ast.Cell) {
        return kb.ast.cell(ast[kb.ids.value]);
    } else if (&ast.struct_() == &kb.std.ast.SelfFn) {
        return kb.ast.selfFn();
    } else if (&ast.struct_() == &kb.std.ast.Self) {
        return kb.ast.self();
    } else if (&ast.struct_() == &kb.std.ast.Continue) {
        return kb.ast.continue_();
    } else if (&ast.struct_() == &kb.std.ast.Break) {
        return kb.ast.break_();
    } else if (&ast.struct_() == &kb.std.ast.Try) {
        return kb.ast.try_(resolveNode(ast["tryBranch"]), resolveNode(ast["catchBranch"]));
    } else if (&ast.struct_() == &kb.std.ast.Throw) {
        if (ast.has(kb.ids.value)) {
            return kb.ast.throw_(resolveNode(ast[kb.ids.value]));
        } else {
            return kb.ast.throw_();
        }
    } else if (&ast.struct_() == &kb.std.ast.Parameter) {
        return kb.ast.parameter(ast[kb.ids.key]);
    } else if (&ast.struct_() == &kb.std.ast.Var) {
        return kb.ast.var(ast[kb.ids.name]);
    } else if (&ast.struct_() == &kb.std.ast.Delete) {
        return kb.ast.delete_(resolveNode(ast[kb.ids.cell]));
    } else if (&ast.struct_() == &kb.std.ast.Set) {
        return kb.ast.set(resolveNode(ast[kb.ids.cell]), resolveNode(ast[kb.ids.key]), resolveNode(ast[kb.ids.value]));
    } else if (&ast.struct_() == &kb.std.ast.Erase) {
        return kb.ast.erase(resolveNode(ast[kb.ids.cell]), resolveNode(ast[kb.ids.key]));
    } else if (&ast.struct_() == &kb.std.ast.If) {
        if (ast.has("else_")) {
            return kb.ast.if_(resolveNode(ast[kb.ids.condition]))
                .then_(resolveNode(ast[kb.ids.then]))
                .else_(resolveNode(ast[kb.ids.else_]));
        } else {
            return kb.ast.if_(resolveNode(ast[kb.ids.condition])).then_(resolveNode(ast[kb.ids.then]));
        }
    } else if (&ast.struct_() == &kb.std.ast.Match) {
        auto& ret = kb.ast.match_(static_cast<Base&>(ast["enum"]));
        Visitor::visitList(ast["cases"][kb.ids.list], [this, &resolveNode, &ret](CellI& kvpair, int, bool&) {
            auto& key = kvpair[kb.ids.key];
            auto& op  = resolveNode(kvpair[kb.ids.value]);
            ret.case_(key, op);
        });
        return ret;
    } else if (&ast.struct_() == &kb.std.ast.Do) {
        return kb.ast.do_(resolveNode(ast[kb.ids.statement])).while_(resolveNode(ast[kb.ids.condition]));
    } else if (&ast.struct_() == &kb.std.ast.While) {
        return kb.ast.while_(resolveNode(ast[kb.ids.condition])).do_(resolveNode(ast[kb.ids.statement]));
    } else if (&ast.struct_() == &kb.std.ast.And) {
        return kb.ast.and_(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.struct_() == &kb.std.ast.Or) {
        return kb.ast.or_(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.struct_() == &kb.std.ast.Not) {
        return kb.ast.not_(resolveNode(ast[kb.ids.input]));
    } else if (&ast.struct_() == &kb.std.ast.Add) {
        return kb.ast.add(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.struct_() == &kb.std.ast.Subtract) {
        return kb.ast.subtract(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.struct_() == &kb.std.ast.Multiply) {
        return kb.ast.multiply(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.struct_() == &kb.std.ast.Divide) {
        return kb.ast.divide(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.struct_() == &kb.std.ast.LessThan) {
        return kb.ast.lessThan(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.struct_() == &kb.std.ast.LessThanOrEqual) {
        return kb.ast.lessThanOrEqual(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.struct_() == &kb.std.ast.GreaterThan) {
        return kb.ast.greaterThan(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.struct_() == &kb.std.ast.GreaterThanOrEqual) {
        return kb.ast.greaterThanOrEqual(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.struct_() == &kb.std.ast.Same) {
        return kb.ast.same(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.struct_() == &kb.std.ast.NotSame) {
        return kb.ast.notSame(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.struct_() == &kb.std.ast.Equal) {
        return kb.ast.equal(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.struct_() == &kb.std.ast.NotEqual) {
        return kb.ast.notEqual(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.struct_() == &kb.std.ast.Has) {
        return kb.ast.has(resolveNode(ast[kb.ids.cell]), resolveNode(ast[kb.ids.key]));
    } else if (&ast.struct_() == &kb.std.ast.Missing) {
        return kb.ast.missing(resolveNode(ast[kb.ids.cell]), resolveNode(ast[kb.ids.key]));
    } else if (&ast.struct_() == &kb.std.ast.Get) {
        return kb.ast.get(resolveNode(ast[kb.ids.cell]), resolveNode(ast[kb.ids.key]));
    } else if (&ast.struct_() == &kb.std.ast.Member) {
        return kb.ast.member(ast[kb.ids.key]);
    } else if (&ast.struct_() == &kb.std.ast.Return) {
        if (ast.has("value")) {
            return kb.ast.return_(static_cast<Base&>(resolveNode(ast[kb.ids.value])));
        }
        return kb.ast.return_();
    }

    throw "Unknown AST to instantiate!";
}

CellI& Ast::Function::compile(CellI& state)
{
    // TODO hack!
    // find a better way to create type during compilation
    cells::Object& functionType = *new cells::Object(kb, kb.std.Struct);
    functionType.set("memberOf", kb.map(kb.std.Struct, kb.std.Struct, kb.std.op.Function, kb.std.op.Function));
    cells::Map& subTypesMap = kb.map(kb.std.Cell, kb.std.Struct,
                                     kb.ids.name, get("name"));
    if (has("structType")) {
        Struct& currentStruct = static_cast<Struct&>(state[kb.ids.currentStruct]);
        auto& structType = get("structType");
        subTypesMap.add(kb.ids.objectType, structType["compiledStruct"]);
    }
    functionType.set("subTypes", subTypesMap);

    Map& functionSlots = kb.slots(
        kb.std.slot(kb.ids.ast, kb.std.ast.Base),
        kb.std.slot(kb.ids.state, kb.std.Cell),
        kb.std.slot(kb.ids.previous, kb.std.Cell),
        kb.std.slot(kb.ids.stack, kb.std.Stack),
        kb.std.slot(kb.ids.lastOp, kb.std.op.Base),
        kb.std.slot(kb.ids.op, kb.ListOf(kb.std.op.Base)),
        kb.std.slot(kb.ids.static_, kb.std.Boolean));
    functionType.set(kb.ids.slots, functionSlots);

    cells::Object& function = *new cells::Object(kb, functionType);
    compileParams(function, functionSlots, subTypesMap, state);
    functionType.label(fmt::format("Type for {}", function.label()));
    function.set(kb.ids.ast, *this);
    function.set(kb.ids.op, compileAst(instructions(), function, state));
    if (has(kb.ids.static_)) {
        function.set(kb.ids.static_, get(kb.ids.static_));
    }

    return function;
}

std::string Ast::Function::shortName()
{
    std::stringstream iss;
    std::stringstream oss;
    if (has(kb.ids.parameters) || has(kb.ids.structType)) {
        if (has(kb.ids.parameters)) {
            Visitor::visitList(parameters(), [this, &iss](CellI& slot, int i, bool& stop) {
                if (i > 0) {
                    iss << ", ";
                }
                iss << "p_" << slot[kb.ids.key].label() << ": " << getCompiledTypeFromResolvedType(slot[kb.ids.type]).label();
            });
        }
    }
    if (has(kb.ids.returnType)) {
        oss << getCompiledTypeFromResolvedType(returnType()).label();
    }
    if (has(kb.ids.returnType)) {
        return fmt::format("fn {}({}) -> {}", get(kb.ids.name).label(), iss.str(), oss.str());
    } else {
        return fmt::format("fn {}({})", get(kb.ids.name).label(), iss.str());
    }
}

void Ast::Function::compileParams(cells::Object& function, cells::Map& functionSlots, cells::Map& subTypesMap, CellI& state)
{
    std::stringstream iss;
    std::stringstream oss;
    std::string structTypeStr;
    if (has("parameters") || has("structType")) {
        cells::Object& parametersType = *new cells::Object(kb, kb.std.Struct);
        Map& slots                    = *new Map(kb, kb.std.Cell, kb.std.Slot);
        if (has("structType")) {
            CellI& type = get("structType");
            Object& var = *new Object(kb, kb.std.op.Var, "self");
            var.set("valueType", type);
            slots.add(kb.ids.self, kb.std.slot("self", type));
            structTypeStr = fmt::format("{}::", type.label());
        }
        if (has(kb.ids.parameters)) {
            Visitor::visitList(parameters(), [this, &slots, &iss](CellI& slot, int i, bool& stop) {
                if (i > 0) {
                    iss << ", ";
                }
                auto& key         = slot[kb.ids.key];
                auto& type         = slot[kb.ids.type];
                auto& compiledSlotType = getCompiledTypeFromResolvedType(type);
                iss << "p_" << key.label() << ": " << compiledSlotType.label();
                slots.add(key, kb.std.slot(key, compiledSlotType));
            });
        }
        parametersType.set(kb.ids.slots, slots);
        subTypesMap.add(kb.ids.parameters, parametersType);
    }
    if (has(kb.ids.returnType)) {
        auto& astReturnType      = returnType();
        auto& compiledReturnType = getCompiledTypeFromResolvedType(astReturnType);
        oss << compiledReturnType.label();
        subTypesMap.add(kb.ids.returnType, compiledReturnType);
        functionSlots.add(kb.ids.value, compiledReturnType);
    }
    if (has(kb.ids.returnType)) {
        function.label(fmt::format("fn {}{}({}) -> {}", structTypeStr, get(kb.ids.name).label(), iss.str(), oss.str()));
    } else {
        function.label(fmt::format("fn {}{}({})", structTypeStr, get(kb.ids.name).label(), iss.str()));
    }
}

CellI& Ast::Function::compileAst(CellI& ast, cells::Object& function, CellI& state)
{
    auto compile = [this, &function, &state](CellI& ast) -> CellI& { return compileAst(ast, function, state); };
    const auto _ = [this](auto& cell) -> Ast::Cell& { return kb._(cell); };

    if (&ast.struct_() == &kb.std.ast.Block) {
        CellI& list        = ast[kb.ids.asts];
        CellI* prevBlock = nullptr;
        if (state.has("lastBlock")) {
            prevBlock = &state["lastBlock"];
        }
        CellI* firstOpBlockNode   = nullptr;
        CellI* currentOpBlockNode = nullptr;
        Object& opBlock           = *new Object(kb, kb.std.op.Block);
        state.set("lastBlock", opBlock);
        Visitor::visitList(list, [this, &compile, &opBlock, &firstOpBlockNode, &currentOpBlockNode, &function](CellI& ast, int, bool&) {
            CellI& newOpBlockNode = *new Object(kb, kb.std.op.Activate);
            newOpBlockNode.set(kb.ids.cell, compile(ast));
            newOpBlockNode.set(kb.ids.parent, opBlock);

            if (!firstOpBlockNode) {
                firstOpBlockNode = &newOpBlockNode;
            } else {
                (*currentOpBlockNode).set(kb.ids.next, newOpBlockNode);
            }
            currentOpBlockNode = &newOpBlockNode;
        });
        opBlock.set(kb.ids.ast, ast);
        if (firstOpBlockNode) {
            opBlock.set(kb.ids.ops, *firstOpBlockNode);
        }

        if (prevBlock) {
            state.set("lastBlock", *prevBlock);
        } else {
            state.erase("lastBlock");
        }

        return opBlock;
    } else if (&ast.struct_() == &kb.std.ast.Cell) {
        Object& constVar = *new Object(kb, kb.std.op.ConstVar);
        constVar.set(kb.ids.ast, ast);
        constVar.set(kb.ids.value, ast[kb.ids.value]);
        return constVar;
    } else if (&ast.struct_() == &kb.std.ast.ResolvedType) {
        Object& constVar = *new Object(kb, kb.std.op.ConstVar);
        constVar.set(kb.ids.ast, ast);
        constVar.set(kb.ids.value, ast[kb.ids.compiled]);
        return constVar;
    } else if (&ast.struct_() == &kb.std.ast.SelfFn) {
        Object& constVar = *new Object(kb, kb.std.op.ConstVar);
        constVar.set(kb.ids.ast, ast);
        constVar.set(kb.ids.value, function);
        return constVar;
    } else if (&ast.struct_() == &kb.std.ast.Self) {
        CellI& retOp = compile(kb.ast.get(_(function), _(kb.ids.stack)) / _(kb.ids.value) / _(kb.ids.input) / _(kb.ids.self));
        retOp.set(kb.ids.ast, ast);
        retOp.label("self");
        return retOp;
    } else if (&ast.struct_() == &kb.std.ast.Continue) {
        if (state.missing("lastBlock")) {
            throw "No statement to break!";
        }
        CellI& lastBlock = state["lastBlock"];
        CellI& retOp     = compile(kb.ast.set(_(lastBlock), _(kb.ids.status), _(kb.ids.continue_)));
        return retOp;
    } else if (&ast.struct_() == &kb.std.ast.Break) {
        if (state.missing("lastBlock")) {
            throw "No statement to break!";
        }
        CellI& lastBlock = state["lastBlock"];
        CellI& retOp     = compile(kb.ast.set(_(lastBlock), _(kb.ids.status), _(kb.ids.break_)));
        return retOp;
    } else if (&ast.struct_() == &kb.std.ast.Throw) {
        if (state.missing("lastBlock")) {
            throw "No statement to break!";
        }
        CellI& lastBlock = state["lastBlock"];
        CellI& retOp     = compile(kb.ast.set(_(lastBlock), _(kb.ids.status), _(kb.ids.throw_)));
        if (ast.has("value")) {
            retOp.set(kb.ids.result, compile(kb.ast.set(_(lastBlock), _(kb.ids.value), static_cast<Ast::Base&>(ast[kb.ids.value]))));
        }
        return retOp;
    } else if (&ast.struct_() == &kb.std.ast.Parameter) {
        CellI& retOp = compile(kb.ast.get(_(function), _(kb.ids.stack)) / _(kb.ids.value) / _(kb.ids.input) / _(ast[kb.ids.key]));
        retOp.set(kb.ids.ast, ast);
        return retOp;
    } else if (&ast.struct_() == &kb.std.ast.Member) {
        CellI& member = compile(kb.ast.get(kb.ast.self(), kb.ast.cell(ast[kb.ids.key])));
        member.set(kb.ids.ast, ast);
        return member;
    } else if (&ast.struct_() == &kb.std.ast.Return) {
        Object& retOp = *new Object(kb, kb.std.op.Return, "op.return");
        retOp.set(kb.ids.ast, ast);
        if (ast.has(kb.ids.value)) {
            retOp.set(kb.ids.result, compile(kb.ast.set(_(function), _(kb.ids.value), static_cast<Ast::Base&>(ast[kb.ids.value]))));
        }
        return retOp;
    } else if (&ast.struct_() == &kb.std.ast.Var) {
        if (function.struct_()[kb.ids.subTypes][kb.ids.index].missing(kb.ids.localVars)) {
            cells::Object& functionLocalVarsType = *new cells::Object(kb, kb.std.Struct, fmt::format("LocalVarsType of {}", function.label()));
            functionLocalVarsType.set(kb.ids.memberOf, kb.map(kb.std.Struct, kb.std.Struct, kb.std.Index, kb.std.Index));
            static_cast<Map&>(function.struct_()[kb.ids.subTypes]).add(kb.ids.localVars, functionLocalVarsType);
        }
        CellI& localVarsType = function.struct_()[kb.ids.subTypes][kb.ids.index][kb.ids.localVars][kb.ids.value];
        if (localVarsType.missing(kb.ids.slots)) {
            localVarsType.set(kb.ids.slots, *new Map(kb, kb.std.Cell, kb.std.Slot));
        }
        auto& slotsMap = static_cast<Map&>(localVarsType[kb.ids.slots]);
        if (!slotsMap.hasKey(ast[kb.ids.name])) {
            slotsMap.add(ast[kb.ids.name], kb.std.slot(ast[kb.ids.name], kb.std.op.Var));
        }
        CellI& retOp = compile(kb.ast.get(_(function), _(kb.ids.stack)) / _(kb.ids.value) / _(kb.ids.localVars) / _(ast[kb.ids.name]));
        retOp.set(kb.ids.ast, ast);
        return retOp;
    } else if (&ast.struct_() == &kb.std.ast.Delete) {
        Object& retOp = *new Object(kb, kb.std.op.Delete);
        retOp.set(kb.ids.ast, ast);
        retOp.set(kb.ids.input, compile(ast[kb.ids.cell]));
        return retOp;
    } else if (&ast.struct_() == &kb.std.ast.Set) {
        Object& retOp = *new Object(kb, kb.std.op.Set);
        retOp.set(kb.ids.ast, ast);
        retOp.set(kb.ids.cell, compile(ast[kb.ids.cell]));
        retOp.set(kb.ids.key, compile(ast[kb.ids.key]));
        retOp.set(kb.ids.value, compile(ast[kb.ids.value]));
        return retOp;
    } else if (&ast.struct_() == &kb.std.ast.Erase) {
        Object& retOp = *new Object(kb, kb.std.op.Erase);
        retOp.set(kb.ids.ast, ast);
        retOp.set(kb.ids.cell, compile(ast[kb.ids.cell]));
        retOp.set(kb.ids.key, compile(ast[kb.ids.key]));
        return retOp;
    } else if (&ast.struct_() == &kb.std.ast.If) {
        Object& retOp = *new Object(kb, kb.std.op.If);
        retOp.set(kb.ids.ast, ast);
        retOp.set(kb.ids.condition, compile(ast[kb.ids.condition]));
        retOp.set(kb.ids.then, compile(ast[kb.ids.then]));
        if (ast.has(kb.ids.else_)) {
            retOp.set(kb.ids.else_, compile(ast[kb.ids.else_]));
            return retOp;
        } else {
            return retOp;
        }
    } else if (&ast.struct_() == &kb.std.ast.Match) {
        auto& enumObj   = static_cast<Base&>(ast["enum"]);
        auto& caseList  = ast["cases"][kb.ids.list];
        auto& astCases  = *new cells::List(kb, kb.std.ast.Base);
        Block& astBlock = *new Block(kb, astCases);

        Visitor::visitList(caseList, [this, &compile, &ast, &function, &enumObj, &astCases](CellI& kvpair, int, bool&) {
            auto& kind    = kvpair[kb.ids.key];
            auto& op      = kvpair[kb.ids.value];
            auto& oneCase = kb.ast.if_(kb.ast.same(kb.ast.get(enumObj, "tag"), kb._(kind))).then_(static_cast<Base&>(op));
            astCases.add(oneCase);
        });
        auto& retOp = compile(astBlock);
        return retOp;
    } else if (&ast.struct_() == &kb.std.ast.Do) {
        Object& retOp = *new Object(kb, kb.std.op.Do);
        retOp.set(kb.ids.ast, ast);
        retOp.set(kb.ids.condition, compile(ast[kb.ids.condition]));
        retOp.set(kb.ids.statement, compile(ast[kb.ids.statement]));
        return retOp;
    } else if (&ast.struct_() == &kb.std.ast.While) {
        Object& retOp = *new Object(kb, kb.std.op.While);
        retOp.set(kb.ids.ast, ast);
        retOp.set(kb.ids.condition, compile(ast[kb.ids.condition]));
        retOp.set(kb.ids.statement, compile(ast[kb.ids.statement]));
        return retOp;
    } else if (&ast.struct_() == &kb.std.ast.New) {
        CellI* firstOpBlockNode   = nullptr;
        auto& astObjectType       = ast[kb.ids.objectType];

        Object& block = *new Object(kb, kb.std.op.Block);
        block.set(kb.ids.ast, ast);

        Object& opSet = *new Object(kb, kb.std.op.Set, "New { block.value = new objectType(); }");
        opSet.set(kb.ids.ast, ast);
        opSet.set(kb.ids.cell, compile(kb.ast.cell(block)));
        opSet.set(kb.ids.key, compile(kb.ast.cell(kb.ids.value)));

        Object& opNew = *new Object(kb, kb.std.op.New);
        opNew.set(kb.ids.ast, ast);
        opNew.set(kb.ids.objectType, compile(astObjectType));
        opSet.set(kb.ids.value, opNew);

        CellI& newOpBlockNode = *new Object(kb, kb.std.op.Activate);
        newOpBlockNode.set(kb.ids.cell, opSet);
        newOpBlockNode.set(kb.ids.parent, block);
        firstOpBlockNode = &newOpBlockNode;

        if (ast.has(kb.ids.constructor)) {
            Object& callAst = *new Object(kb, kb.std.ast.Call);
            callAst.set(kb.ids.cell, kb.ast.get(kb.ast.cell(block), kb.ast.cell(kb.ids.value)));
            callAst.set(kb.ids.method, ast[kb.ids.constructor]);
            if (ast.has(kb.ids.parameters)) {
                callAst.set(kb.ids.parameters, ast[kb.ids.parameters]);
            }
            CellI& callConstructor = compile(callAst);
            callConstructor.label("New { call constructor; }");
            CellI& newOpBlockNode = *new Object(kb, kb.std.op.Activate);
            newOpBlockNode.set(kb.ids.cell, callConstructor);
            newOpBlockNode.set(kb.ids.parent, block);
            (*firstOpBlockNode).set(kb.ids.next, newOpBlockNode);
        }
        block.set(kb.ids.ops, *firstOpBlockNode);
        return block;
    } else if (&ast.struct_() == &kb.std.ast.Call || &ast.struct_() == &kb.std.ast.StaticCall) {
        Ast::Base& astCell     = static_cast<Ast::Base&>(ast[kb.ids.cell]);
        Ast::Base& astMethod   = static_cast<Ast::Base&>(ast[kb.ids.method]);
        auto& astMethodId      = astMethod[kb.ids.value];
#if 1
        bool checked = false;
        // New<T>.constructor(...)
        std::stringstream ss;
        if (&astCell.struct_() == &kb.std.ast.Get && &astCell[kb.ids.cell].struct_() == &kb.std.ast.Cell && &astCell[kb.ids.cell][kb.ids.value].struct_() == &kb.std.op.Block) {
            auto& opBlock = astCell[kb.ids.cell][kb.ids.value];
            auto& opBlockAst = opBlock[kb.ids.ast];
            if (&opBlockAst.struct_() == &kb.std.ast.New) {
                auto& astObjectTypeStruct = opBlockAst[kb.ids.objectType];
                checkMethodCall(astObjectTypeStruct, astMethodId, state);
                checked = true;
            }
        // var_xxx.call(...)
        } else if (&astCell.struct_() == &kb.std.ast.Get && &astCell[kb.ids.cell].struct_() == &kb.std.ast.Var) {
            auto& astVar = astCell[kb.ids.cell];
            ss << "In " << astVar.label() << " ";
        // m_xxx.method(...)
        } else if (&astCell.struct_() == &kb.std.ast.Member) {
            auto& astMemberId = astCell[kb.ids.key];
            auto& astMembersType = function[kb.ids.ast][kb.ids.structType];
            if (&astMemberId == &kb.ids.struct_) {
                // std::cout << "DDDD " << astMembersType.label();
                auto& stdScope = kb.globalScope.getItem<Scope>("std");
                auto& type     = stdScope.getItem<Struct>("Struct");
                checkMethodCall(type, astMethodId, state);
                checked        = true;
            } else {
                auto& astMembers = static_cast<Map&>(function[kb.ids.ast][kb.ids.structType][kb.ids.members]);
                if (astMembers.hasKey(astMemberId)) {
                    Slot& slot = static_cast<Slot&>(astMembers.getValue(astMemberId));
                    auto& type = slot[kb.ids.type];
                    checkMethodCall(type, astMethodId, state);
                    checked = true;
                } else {
                    throw "Unknown member name!";
                }
            }
        } else if (&astCell.struct_() == &kb.std.ast.Self) {
            auto& astSelfType = function[kb.ids.ast][kb.ids.structType];
            checkMethodCall(astSelfType, astMethodId, state);
            checked = true;
        } else if (&astCell.struct_() == &kb.std.ast.Parameter) {
            auto& parameterRole         = astCell[kb.ids.key];
            auto& astFunctionParameters = function[kb.ids.ast][kb.ids.parameters];
            Slot* astFunctionParameterSlot = nullptr;
            Visitor::visitList(astFunctionParameters, [this, &parameterRole, &astFunctionParameterSlot](CellI& slot, int i, bool& stop) {
                CellI& key  = slot[kb.ids.key];
                CellI& type = slot[kb.ids.type];
                if (&key == &parameterRole) {
                    astFunctionParameterSlot = &static_cast<Slot&>(slot);
                    stop = true;
                    return;
                }
            });
            if (astFunctionParameterSlot) {
                Slot& slot             = *astFunctionParameterSlot;
                auto& astParameterType = slot[kb.ids.type];
                checkMethodCall(astParameterType, astMethodId, state);
                checked = true;
            } else {
                throw "Unknown parameter name!";
            }
        }
        if (!checked) {
            WARN(compileStruct, "{} Unchecked method call {} in {}", ss.str(), astMethodId.label(), function.label());
        }
#endif
        Object& retOp = *new Object(kb, kb.std.op.Call);
        retOp.set(kb.ids.ast, ast);
        retOp.set(kb.ids.cell, compile(ast[kb.ids.cell]));
        retOp.set(kb.ids.method, compile(ast[kb.ids.method]));
        retOp.set(kb.ids.stack, compile(kb.ast.get(_(function), _(kb.ids.stack))));
        if (ast.has(kb.ids.parameters)) {
            List& parameters = *new List(kb, kb.std.Slot);
            Visitor::visitList(ast[kb.ids.parameters], [this, &parameters, &compile, &_](CellI& param, int, bool&) {
                CellI& slot = *new Object(kb, kb.std.Slot);
                slot.set(kb.ids.key, param[kb.ids.key]);
                slot.set(kb.ids.type, compile(param[kb.ids.type]));
                parameters.add(slot);
            });
            retOp.set(kb.ids.parameters, parameters);
        }

        return retOp;
    } else if (&ast.struct_() == &kb.std.ast.And) {
        Object& retOp = *new Object(kb, kb.std.op.And);
        retOp.set(kb.ids.ast, ast);
        retOp.set(kb.ids.lhs, compile(ast[kb.ids.lhs]));
        retOp.set(kb.ids.rhs, compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.struct_() == &kb.std.ast.Or) {
        Object& retOp = *new Object(kb, kb.std.op.Or);
        retOp.set(kb.ids.ast, ast);
        retOp.set(kb.ids.lhs, compile(ast[kb.ids.lhs]));
        retOp.set(kb.ids.rhs, compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.struct_() == &kb.std.ast.Not) {
        Object& retOp = *new Object(kb, kb.std.op.Not);
        retOp.set(kb.ids.ast, ast);
        retOp.set(kb.ids.input, compile(ast[kb.ids.input]));
        return retOp;
    } else if (&ast.struct_() == &kb.std.ast.Add) {
        Object& retOp = *new Object(kb, kb.std.op.Add);
        retOp.set(kb.ids.ast, ast);
        retOp.set(kb.ids.lhs, compile(ast[kb.ids.lhs]));
        retOp.set(kb.ids.rhs, compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.struct_() == &kb.std.ast.Subtract) {
        Object& retOp = *new Object(kb, kb.std.op.Subtract);
        retOp.set(kb.ids.ast, ast);
        retOp.set(kb.ids.lhs, compile(ast[kb.ids.lhs]));
        retOp.set(kb.ids.rhs, compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.struct_() == &kb.std.ast.Multiply) {
        Object& retOp = *new Object(kb, kb.std.op.Multiply);
        retOp.set(kb.ids.ast, ast);
        retOp.set(kb.ids.lhs, compile(ast[kb.ids.lhs]));
        retOp.set(kb.ids.rhs, compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.struct_() == &kb.std.ast.Divide) {
        Object& retOp = *new Object(kb, kb.std.op.Divide);
        retOp.set(kb.ids.ast, ast);
        retOp.set(kb.ids.lhs, compile(ast[kb.ids.lhs]));
        retOp.set(kb.ids.rhs, compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.struct_() == &kb.std.ast.LessThan) {
        Object& retOp = *new Object(kb, kb.std.op.LessThan);
        retOp.set(kb.ids.ast, ast);
        retOp.set(kb.ids.lhs, compile(ast[kb.ids.lhs]));
        retOp.set(kb.ids.rhs, compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.struct_() == &kb.std.ast.LessThanOrEqual) {
        Object& retOp = *new Object(kb, kb.std.op.LessThanOrEqual);
        retOp.set(kb.ids.ast, ast);
        retOp.set(kb.ids.lhs, compile(ast[kb.ids.lhs]));
        retOp.set(kb.ids.rhs, compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.struct_() == &kb.std.ast.GreaterThan) {
        Object& retOp = *new Object(kb, kb.std.op.GreaterThan);
        retOp.set(kb.ids.ast, ast);
        retOp.set(kb.ids.lhs, compile(ast[kb.ids.lhs]));
        retOp.set(kb.ids.rhs, compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.struct_() == &kb.std.ast.GreaterThanOrEqual) {
        Object& retOp = *new Object(kb, kb.std.op.GreaterThanOrEqual);
        retOp.set(kb.ids.ast, ast);
        retOp.set(kb.ids.lhs, compile(ast[kb.ids.lhs]));
        retOp.set(kb.ids.rhs, compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.struct_() == &kb.std.ast.Same) {
        Object& retOp = *new Object(kb, kb.std.op.Same);
        retOp.set(kb.ids.ast, ast);
        retOp.set(kb.ids.lhs, compile(ast[kb.ids.lhs]));
        retOp.set(kb.ids.rhs, compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.struct_() == &kb.std.ast.NotSame) {
        Object& retOp = *new Object(kb, kb.std.op.NotSame);
        retOp.set(kb.ids.ast, ast);
        retOp.set(kb.ids.lhs, compile(ast[kb.ids.lhs]));
        retOp.set(kb.ids.rhs, compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.struct_() == &kb.std.ast.Equal) {
        Object& retOp = *new Object(kb, kb.std.op.Equal);
        retOp.set(kb.ids.ast, ast);
        retOp.set(kb.ids.lhs, compile(ast[kb.ids.lhs]));
        retOp.set(kb.ids.rhs, compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.struct_() == &kb.std.ast.NotEqual) {
        Object& retOp = *new Object(kb, kb.std.op.NotEqual);
        retOp.set(kb.ids.ast, ast);
        retOp.set(kb.ids.lhs, compile(ast[kb.ids.lhs]));
        retOp.set(kb.ids.rhs, compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.struct_() == &kb.std.ast.Has) {
        Object& retOp = *new Object(kb, kb.std.op.Has);
        retOp.set(kb.ids.ast, ast);
        retOp.set(kb.ids.cell, compile(ast[kb.ids.cell]));
        retOp.set(kb.ids.key, compile(ast[kb.ids.key]));
        return retOp;
    } else if (&ast.struct_() == &kb.std.ast.Missing) {
        Object& retOp = *new Object(kb, kb.std.op.Missing);
        retOp.set(kb.ids.ast, ast);
        retOp.set(kb.ids.cell, compile(ast[kb.ids.cell]));
        retOp.set(kb.ids.key, compile(ast[kb.ids.key]));
        return retOp;
    } else if (&ast.struct_() == &kb.std.ast.Get) {
        Object& retOp = *new Object(kb, kb.std.op.Get);
        retOp.set(kb.ids.ast, ast);
        retOp.set(kb.ids.cell, compile(ast[kb.ids.cell]));
        retOp.set(kb.ids.key, compile(ast[kb.ids.key]));
        return retOp;
    }

    throw "Unknown function AST!";
}

void Ast::Function::checkMethodCall(CellI& astType, CellI& astMethodId, CellI& state)
{
    CellI* typePtr = nullptr;
    Map* methodsPtr = nullptr;
    if (astType.isA(kb.std.ast.Cell)) {
        auto& cellValue = astType[kb.ids.value];
        typePtr         = &cellValue;
        methodsPtr      = &static_cast<Map&>(cellValue[kb.ids.methods]);
    } else if (astType.isA(kb.std.ast.Struct)) {
        typePtr    = &astType;
        methodsPtr = &static_cast<Map&>(astType[kb.ids.methods]);
    } else if (astType.isA(kb.std.ast.ResolvedType)) {
        typePtr    = &astType[kb.ids.compiled];
        methodsPtr = &static_cast<Map&>(astType[kb.ids.ast][kb.ids.methods]);
    } else {
        throw "Unexpected AST type";
    }
    CellI& type = *typePtr;
    if (&type.struct_() != &kb.std.ast.Struct && &type.struct_() != &kb.std.Struct && type.label() != "Struct") {
        throw "Resolved type must be a type!";
    }
    auto& methods = *methodsPtr;
    if (!methods.hasKey(astMethodId)) {
        std::cerr << fmt::format("Method '{}' doesn't exist in type {}", astMethodId.label(), type.label()) << std::endl;
        throw "Method doesn't exist in type!";
    }
}
#endif

} // namespace cells
} // namespace infocell