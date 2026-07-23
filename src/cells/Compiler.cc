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
    it must be resolved to a TypeName, with id L,i,s,t,id.valueType,type.Slot
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
    compileInstructionsInScope(scope, resolvedScope);
    compileDescriptionInScope(scope, resolvedScope);

    m_libraryPtr->set(w.id.scope, scope);
    m_libraryPtr->set(w.id.resolvedScope, resolvedScope);
    m_libraryPtr->m_toolFinderPtr = &m_toolFinder;

    return *m_libraryPtr;
}

Object& Compiler::compileAsPrompt(Ast::Function& prompt)
{
    auto& resolvedPrompt = resolveTypesInFunction(prompt);

    instantiateTemplateInstances();
    Object& compiledPrompt = *new Object(w, w.std.op.Function);

    return static_cast<Object&>(compileDescriptionInFunctionAst(prompt[w.id.instructions][w.id.asts][w.id.first][w.id.value], compiledPrompt));
}


CellI& Compiler::reigisterStructBeforeCompilation(CellI& structAst)
{
    CellI* structIdPtr = nullptr;
    if (&structAst.__type__() == &w.std.ast.TemplatedType) {
        List& idCell = *new List(w, w.std.Cell);
        structIdPtr  = &idCell;
        std::stringstream ss;
        for (CellI& scope : structAst[w.id.scopes]) {
            for (CellI& character : scope) {
                idCell.add(character);
                ss << character.label();
            }
            idCell.add(w.pools.chars.get(':'));
            idCell.add(w.pools.chars.get(':'));
            ss << "::";
        }
        for (CellI& character : structAst[w.id.id]) {
            idCell.add(character);
            ss << character.label();
        }

        ss << "<";
        int i = 0;
        for (CellI& slot : structAst[w.id.parameters]) {
            if (i++ != 0) {
                ss << ", ";
            }
            CellI& key              = slot[w.id.key];
            CellI& type             = slot[w.id.type];
            CellI& compiledSlotType = reigisterStructBeforeCompilation(type);

            idCell.add(key);
            idCell.add(compiledSlotType);
            ss << fmt::format("{}={}", key.label(), compiledSlotType.label());
        }
        ss << ">";
        idCell.label(ss.str());
    } else if (&structAst.__type__() == &w.std.ast.TypeName) {
        structIdPtr = &structAst[w.id.name];
    } else if (&structAst.__type__() == &w.std.ast.ConstVar) {
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

        m_earlyStructs.add(structId, w.ast.slot(structAst, unresolvedStruct));
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
    const auto& typeName     = sliced.back();
    Ast::Scope* currentScope = parentScope ? parentScope : &w.globalScope;
    if (sliced.size() > 1) {
        for (int i = 0; i < sliced.size() - 1; ++i) {
            const auto& scopeName = sliced[i];
            currentScope          = &currentScope->getItem<Ast::Scope>(scopeName);
            for (CellI& character : (*currentScope)["name"]) {
                idCell.add(character);
                ss << character.label();
            }
            idCell.add(w.pools.chars.get(':'));
            idCell.add(w.pools.chars.get(':'));
            ss << "::";
        }
    }
    Ast::StructBase* structBaseAstPtr = nullptr;
    if (&compiledStruct[w.id.__type__] == &w.std.Struct) {
        Ast::Struct& structAst = currentScope->getItem<Ast::Struct>(typeName);
        structBaseAstPtr       = &structAst;
    } else if (&compiledStruct[w.id.__type__] == &w.std.Enum) {
        Ast::Enum& enumAst = currentScope->getItem<Ast::Enum>(typeName);
        structBaseAstPtr   = &enumAst;
    } else {
        throw "The compiled type not a struct or enum!";
    }
    Ast::StructBase& structBaseAst = *structBaseAstPtr;

    for (CellI& character : structBaseAst[w.id.name]) {
        idCell.add(character);
        ss << character.label();
    }
    idCell.label(ss.str());
    compiledStruct.set("incomplete", w.true_);
    m_earlyStructs.add(idCell, w.ast.slot(w.__type__(fullName), compiledStruct));
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
    String& enumValueIdCell   = w.name(enumValueName);
    Ast::Scope* currentScope  = &w.globalScope;

    if (sliced.size() > 1) {
        for (int i = 0; i < sliced.size() - 2; ++i) {
            const auto& scopeName = sliced[i];
            currentScope          = &currentScope->getItem<Ast::Scope>(scopeName);
            for (CellI& character : (*currentScope)["name"]) {
                enumIdCell.add(character);
            }
            enumIdCell.add(w.pools.chars.get(':'));
            enumIdCell.add(w.pools.chars.get(':'));
        }
    }
    Ast::Enum& enumAst = currentScope->getItem<Ast::Enum>(enumName);
    if (!enumAst.values().hasKey(enumValueIdCell)) {
        throw "Invalid enum value!";
    }
    Ast::EnumValue& enumValueAst = static_cast<Ast::EnumValue&>(enumAst.values().getValue(enumValueIdCell));
    for (CellI& character : enumAst[w.id.name]) {
        enumIdCell.add(character);
    }
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
    for (CellI& earlyStructKV : m_earlyStructs) {
        auto& structId       = earlyStructKV[w.id.key];
        auto& structRefAst   = earlyStructKV[w.id.value][w.id.key];
        auto& compiledStruct = earlyStructKV[w.id.value][w.id.type];

        TRACE(compileStruct, "early struct: {}", earlyStructKV[w.id.key].label());

        auto& structReference = *new Object(w, w.std.StructReference);
        structReference.set(w.id.value, compiledStruct);
        structReference.set(w.id.id, structId);

        if (&structRefAst.__type__() == &w.std.ast.TemplatedType) {
            m_unknownInstances.add(structId, structReference);
        } else if (&structRefAst.__type__() == &w.std.ast.TypeName) {
            m_unknownStructs.add(structId, structReference);
        }
    }
}

void Compiler::resolveEarlyStructsInScope(Ast::Scope& scope, Ast::Scope& resolvedScope)
{
    if (m_earlyStructs.empty()) {
        return;
    }
    for (CellI& earlyStructKV : m_earlyStructs) {
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
        } else if (&structRefAst.__type__() == &w.std.ast.TypeName) {
            if (m_unknownStructs.hasKey(structId)) {
                CellI& structReference = m_unknownStructs.getValue(structId);
            }
        }
    }
}

Ast::Scope& Compiler::resolveTypesInScope(Ast::Scope& scope)
{
    auto& resolvedScope = *new Ast::Scope(w, scope.label());
    scope.set(w.id.resolvedScope, resolvedScope);
    m_scope = &scope;
    m_resolvedScope = &resolvedScope;

    if (scope.has("variables")) {
        for (CellI& origAstVarCell : scope.items<Ast::Var>()) {
            Ast::Var& origAstVar = static_cast<Ast::Var&>(origAstVarCell[w.id.value]);
            resolvedScope.add<Ast::Var>(origAstVar);
        }
    }
    if (scope.has("structs")) {
        for (CellI& origAstStructCell : scope.items<Ast::Struct>()) {
            Ast::Struct& origAstStruct     = static_cast<Ast::Struct&>(origAstStructCell[w.id.value]);
            Ast::Struct& resolvedAstStruct = resolveTypesInStruct(origAstStruct);
            resolvedScope.add<Ast::Struct>(resolvedAstStruct);
        }
    }
    if (scope.has("enums")) {
        for (CellI& origAstEnumCell : scope.items<Ast::Enum>()) {
            Ast::Enum& origAstEnum     = static_cast<Ast::Enum&>(origAstEnumCell[w.id.value]);
            Ast::Enum& resolvedAstEnum = resolveTypesInEnum(origAstEnum);
            resolvedScope.add<Ast::Enum>(resolvedAstEnum);
        }
    }
    if (scope.has("functions")) {
        m_currentStruct = nullptr;
        for (CellI& origAstFunctionCell : scope.items<Ast::Function>()) {
            Ast::Function& origAstFunction     = static_cast<Ast::Function&>(origAstFunctionCell[w.id.value]);
            Ast::Function& resolvedAstFunction = resolveTypesInFunction(origAstFunction);
            resolvedScope.add<Ast::Function>(resolvedAstFunction);
        }
    }
    if (scope.has("scopes")) {
        for (CellI& origAstScopeCell : scope.items<Ast::Scope>()) {
            Ast::Scope& origAstScope     = static_cast<Ast::Scope&>(origAstScopeCell[w.id.value]);
            if (origAstScope.has("link")) {
                continue;
            }
            Ast::Scope& resolvedAstScope = resolveTypesInScope(origAstScope);
            resolvedScope.add<Ast::Scope>(resolvedAstScope);
            m_scope = &scope;
        }
    }

    return resolvedScope;
}

Ast::Function& Compiler::resolveTypesInFunction(Ast::Function& function, Ast::StructBase* astStruct)
{
    Ast::Function& ret = *new Ast::Function(w, function.get("name"));
    m_currentFn = &ret;
    std::stringstream ss;

    if (function.has(w.id.description)) {
        ret.set(w.id.description, function.get(w.id.description));
    }
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
        int i = 0;
        for (CellI& param : function.parameters()) {
            CellI& paramId           = param[w.id.key];
            CellI& paramType         = param[w.id.type];
            CellI& resolvedParamType = resolveType(paramType);
            CellI& compiledParamType = getCompiledTypeFromResolvedType(resolvedParamType);
            if (i > 0) {
                ss << ", ";
            }
            ss << fmt::format("{}: {}", paramId.label(), compiledParamType.label());
            ret.addParameter(w.ast.parameterDeclaration(paramId, resolvedParamType));
        }
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

    // we have now enough information to auto-generate the instruction(s) for primitve tools here
    if (function.has("primitiveTool")) {
        ret.set("primitiveTool", function["primitiveTool"]);
        ret.set(w.id.memberMapping, function[w.id.memberMapping]);

        Ast::Call& instructionCall = w.ast.call(w.ast.self(), w.ast.primitiveToolName(function["primitiveTool"]));
        if (function.has(w.id.parameters)) {
            for (CellI& parameter : function[w.id.parameters]) {
                CellI& key = parameter[w.id.key];
                instructionCall(key.label(), w.ast.parameter(key));
            }
        }
        CellI* generatedInstructionsPtr = nullptr;
        if (function.has(w.id.returnType)) {
            Ast::Return& returnInstruction = w.ast.return_(instructionCall);
            generatedInstructionsPtr       = &returnInstruction;
        } else {
            generatedInstructionsPtr = &instructionCall;
        }
        CellI& resolvedinstructionsAst = resolveTypesInFunctionCode(*generatedInstructionsPtr, ret, astStruct);
        ret.set("instructions", resolvedinstructionsAst);
    }
    if (function.has("instructions")) {
        CellI& resolvedinstructionsAst = resolveTypesInFunctionCode(function.instructions(), ret, astStruct);
        ret.set("instructions", resolvedinstructionsAst);
    }

    return ret;
}

Ast::Base& Compiler::resolveTypesInFunctionCode(CellI& ast, Ast::Function& astFunction, Ast::StructBase* astStructPtr)
{
    auto resolve = [this, &astFunction, &astStructPtr](CellI& ast) -> Ast::Base& { return resolveTypesInFunctionCode(ast, astFunction, astStructPtr); };

    if (&ast.__type__() == &w.std.ast.Block) {
        // do nothing just traverse and copy the AST nodes
        auto& instantiedAsts = *new List(w, w.std.ast.Base);
        for (CellI& ast : ast[w.id.asts]) {
            instantiedAsts.add(resolve(ast));
        }
        return *new Ast::Block(w, instantiedAsts);
    } else if (&ast.__type__() == &w.std.ast.TypeName) {
        return resolveType(ast);
    } else if (&ast.__type__() == &w.std.ast.Self) {
        if (!astStructPtr) {
            throw "Referencing self pointer, but this function doesn't associated with a struct/enum!";
        }
        auto& ret = w.ast.self();
        ret.set(w.id.type, (*astStructPtr)["compiledStruct"]);
        return ret;
    } else if (&ast.__type__() == &w.std.ast.Member) {
        if (!astStructPtr) {
            throw "Referencing a member, but this function doesn't associated with a struct/enum!";
        }
        Ast::StructBase& astStruct = *astStructPtr;
        CellI& memberKey           = ast[w.id.key];
        if (&memberKey == &w.id.__type__) {
            CellI& compiledStruct = astStruct["compiledStruct"];
            if (&compiledStruct == &w.std.Index) {
                return w.ast.member(ast[w.id.key]);
            } else {
                return w.ast._(compiledStruct);
            }
        }
        auto& member = static_cast<Ast::Member&>(astStruct.members().getValue(memberKey));
        return member;
    } else if (&ast.__type__() == &w.std.ast.Parameter) {
        Map& parameters = static_cast<Map&>(astFunction[w.id.parameters]);
        auto& key       = ast[w.id.key];
        auto& ret       = w.ast.parameter(key);
        auto& parameter = parameters.getValue(key);
        auto& type      = getCompiledTypeFromResolvedType(parameter[w.id.type]);
        ret.set(w.id.type, type);
        return ret;
    } else if (&ast.__type__() == &w.std.ast.ConstVar) {
        auto& ret = w.ast.constVar(ast[w.id.value]);
        ret.set(w.id.type, ast[w.id.value].__type__());
        return ret;
    } else if (&ast.__type__() == &w.std.ast.Var) {
        return w.ast.var(ast[w.id.name]);
    } else if (&ast.__type__() == &w.std.ast.Continue) {
        return w.ast.continue_();
    } else if (&ast.__type__() == &w.std.ast.Break) {
        return w.ast.break_();
    } else if (&ast.__type__() == &w.std.ast.Return) {
        if (ast.has("value")) {
            return w.ast.return_(resolve(ast[w.id.value]));
        }
        return w.ast.return_();
    } else if (&ast.__type__() == &w.std.ast.If) {
        if (ast.has("else_")) {
            return w.ast.if_(resolve(ast[w.id.condition]))
                .then_(resolve(ast[w.id.then]))
                .else_(resolve(ast[w.id.else_]));
        } else {
            return w.ast.if_(resolve(ast[w.id.condition])).then_(resolve(ast[w.id.then]));
        }
    } else if (&ast.__type__() == &w.std.ast.Match) {
        auto& ret = w.ast.match_(static_cast<Ast::Base&>(ast[w.id.enum_]));
        for (CellI& kvPair : ast["cases"]) {
            auto& key = kvPair[w.id.key];
            auto& op  = resolve(kvPair[w.id.value]);
            ret.case_(key, op);
        }
        return ret;
    } else if (&ast.__type__() == &w.std.ast.Do) {
        return w.ast.do_(resolve(ast[w.id.statement])).while_(resolve(ast[w.id.condition]));
    } else if (&ast.__type__() == &w.std.ast.While) {
        return w.ast.while_(resolve(ast[w.id.condition])).do_(resolve(ast[w.id.statement]));
    } else if (&ast.__type__() == &w.std.ast.For) {
        return w.ast.for_(resolve(ast[w.id.variable])).in(resolve(ast[w.id.container]))(resolve(ast[w.id.statement]));
    } else if (&ast.__type__() == &w.std.ast.New) {
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
                auto& newParameters = *new List(w, w.std.ast.Parameter);
                for (CellI& parameter : ast[w.id.parameters]) {
                    newParameters.add(w.ast.parameterInit(parameter[w.id.key], resolve(parameter[w.id.value])));
                }
                ret.set("parameters", newParameters);
            }
            return ret;
        } else {
            return w.ast.new_(resolvedObjectType);
        }
    } else if (&ast.__type__() == &w.std.ast.Call || &ast.__type__() == &w.std.ast.StaticCall) {
        Ast::Base* retPtr = nullptr;
        if (&ast.__type__() == &w.std.ast.Call) {
            retPtr = &w.ast.call(resolve(ast[w.id.self]), ast[w.id.method]);
        } else {
            retPtr = &w.ast.scall(resolveType(ast[w.id.self]), ast[w.id.method]);
        }
        Ast::Base& ret = *retPtr;
        if (ast.has("parameters")) {
            auto& newParameters = *new List(w, w.std.ast.Parameter);
            for (CellI& parameter : ast[w.id.parameters]) {
                newParameters.add(w.ast.parameterInit(parameter[w.id.key], resolve(parameter[w.id.value])));
            }
            ret.set("parameters", newParameters);
        }
        return ret;
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
        for (CellI& typeAliasSlot : astStruct.typeAliases()) {
            CellI& alias        = typeAliasSlot[w.id.key];
            CellI& type         = typeAliasSlot[w.id.type];
            CellI& resolvedType = resolveType(type);
            ret.typeAliases(w.ast.slot(alias, resolvedType));
            if (IS_LOG_ENABLED) {
                typeAliasesStrs.push_back(fmt::format("    type {} = {};", alias.label(), getCompiledTypeFromResolvedType(resolvedType).label()));
            }
        }
    }

    // resolve memberOf list
    if (astStruct.has("memberOf")) {
        ss << " : ";
        int i = 0;
        for (CellI& membershipType : astStruct.memberOf()) {
            CellI& resolvedMembershipType = resolveType(membershipType);
            if (i++ > 0) {
                ss << ", ";
            }
            ss << getCompiledTypeFromResolvedType(resolvedMembershipType).label();
            ret.memberOf(resolvedMembershipType);
        }
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

    // resolve members
    if (astStruct.has("members")) {
        CellI& membersList = astStruct.members();
        for (CellI& memberCell : membersList) {
            CellI& memberId           = memberCell[w.id.key];
            CellI& memberType         = memberCell[w.id.type];
            CellI& resolvedMemberType = resolveType(memberType);
            ret.addMember(w.ast.member(memberId, resolvedMemberType));
            TRACE(compileStruct, "    {}: {};", memberId.label(), getCompiledTypeFromResolvedType(resolvedMemberType).label());
        }
    }

    // resolve methods
    if (astStruct.has("methods")) {
        for (CellI& origAstFunctionCell : astStruct.methods()) {
            auto& origAstFunction     = static_cast<Ast::Function&>(origAstFunctionCell);
            auto& resolvedAstFunction = resolveTypesInFunction(origAstFunction, &ret);
            ret.addMethod(resolvedAstFunction, Ast::StructBase::ParameterModification::NoChange);
            TRACE(compileStruct, "    {};", shortFunctionName(resolvedAstFunction));
        }
        if (IS_LOG_ENABLED) {
            if (astStruct.has("members")) {
                TRACE(compileStruct, "");
            }
        }
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

    // resolve values
    if (astEnum.has("values")) {
        for (CellI& kvPair : astEnum.values()) {
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
        }
    }

    // resolve methods
    if (astEnum.has("methods")) {
        for (CellI& origAstFunctionCell : astEnum.methods()) {
            auto& origAstFunction     = static_cast<Ast::Function&>(origAstFunctionCell);
            auto& resolvedAstFunction = resolveTypesInFunction(origAstFunction, &ret);
            ret.addMethod(resolvedAstFunction, Ast::StructBase::ParameterModification::NoChange);
            TRACE(compileStruct, "    {};", shortFunctionName(resolvedAstFunction));
        }
    }

    TRACE(compileStruct, "}");

    return ret;
}

CellI& Compiler::resolveTypeInEnumValue(CellI& ast)
{
    if (&ast.__type__() == &w.std.ast.ConstVar) {
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
    for (CellI& character : scopeFullyQualifiedName) {
        fullyQualifiedName.add(character);
    }
    if (!fullyQualifiedName.empty()) {
        fullyQualifiedName.add(w.pools.chars.get(':'));
        fullyQualifiedName.add(w.pools.chars.get(':'));
    }
    if (!isEmptyName) {
        auto& name = base.get(w.id.name);
        for (CellI& character : name) {
            fullyQualifiedName.add(character);
        }
    }
    std::stringstream ss;
    int templateParamPrintModeFromCharIndex = fullyQualifiedName.size();
    if (base.has("instanceOf")) {
        int paramsLength                    = static_cast<List&>(base.get(w.id.templateParams)).size();
        templateParamPrintModeFromCharIndex = fullyQualifiedName.size() - paramsLength * 2;
    }
    int i = 0;
    for (CellI& character : fullyQualifiedName) {
        if (i++ == templateParamPrintModeFromCharIndex) {
            break;
        }
        ss << character.label();
    }
    if (base.has("instanceOf")) {
        ss << "<";
        int i = 0;
        for (CellI& slot : base.get(w.id.templateParams)) {
            CellI& key  = slot[w.id.key];
            CellI& type = slot[w.id.type];
            if (i++ != 0) {
                ss << ", ";
            }
            ss << fmt::format("{}={}", key.label(), getCompiledTypeFromResolvedType(type).label());
        }
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
        for (CellI& character : scopeName) {
            idCell.add(character);
            ss << character.label();
        }
        idCell.add(w.pools.chars.get(':'));
        idCell.add(w.pools.chars.get(':'));
        ss << "::";
    }

    // The generated struct name for a template consist of the template name + parameters
    // Here the name contains the template name + (param1 + value1)+
    int nameLength = name.size() - templateParams.size() * 2;
    int i          = 0;
    for (CellI& character : name) {
        if (i++ >= nameLength) {
            break;
        }
        idCell.add(character);
        ss << character.label();
    }
    ss << "<";
    i = 0;
    for (CellI& slot : templateParams) {
        CellI& key          = slot[w.id.key];
        CellI& type         = slot[w.id.type];
        CellI& compiledType = getCompiledTypeFromResolvedType(type);

        idCell.add(key);
        idCell.add(compiledType);
        if (i++ != 0) {
            ss << ", ";
        }
        ss << fmt::format("{}={}", key.label(), compiledType.label());
    }
    ss << ">";

    idCell.label(ss.str());

    return idCell;
}

List& Compiler::generateTemplateId(CellI& id, CellI& parameters, List& resolvedParams)
{
    List& idCell = *new List(w, w.std.Cell);
    std::stringstream ss;
    for (CellI& character : id) {
        idCell.add(character);
        ss << character.label();
    }

    ss << "<";
    int i = 0;
    for (CellI& slot : parameters) {
        if (i++ != 0) {
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
    }
    ss << ">";
    idCell.label(ss.str());

    return idCell;
}

Ast::Base& Compiler::resolveType(CellI& typeAst)
{
    if (&typeAst.__type__() == &w.std.ast.ConstVar) {
        auto& cell      = static_cast<Ast::ConstVar&>(typeAst);
        auto& cellValue = typeAst[w.id.value];
        if (!cellValue.isA(w.std.Struct) && !cellValue.isA(w.std.Enum)) {
            throw "Type AST referencing a non-type!";
        }
        return cell;
    }
    if (&typeAst.__type__() == &w.std.ast.ResolvedType) {
        return static_cast<Ast::ResolvedType&>(typeAst);
    }
    if (&typeAst.__type__() == &w.std.ast.SelfType) {
        return w.ast._(*m_currentStruct);
    }
    if (&typeAst.__type__() == &w.std.ast.TypeName) {
        auto& resolveAstStruct   = findEnumOrStructByAstStructName(*m_scope, typeAst);
        auto& name               = resolveAstStruct[w.id.name];
        auto& fullyQualifiedName = getFullyQualifiedName(resolveAstStruct);

        if (compiledStructs().hasKey(fullyQualifiedName)) {
            return w.ast.constVar(compiledStructs().getValue(fullyQualifiedName));
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
    if (resolvedTypeAst.isA(w.std.ast.ConstVar)) {
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
        return w.ast.constVar(compiledStructs().getValue(fullyQualifiedStructName));
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
    for (CellI& scopeId : scopeList) {
        if (currentScope->hasItem<Ast::Scope>(scopeId)) {
            currentScope = &currentScope->getItem<Ast::Scope>(scopeId);
            if (Ast::Scope* linkedScope = currentScope->getLinkedScope()) {
                currentScope = linkedScope;
            }
        } else {
            currentScope = nullptr;
            break;
        }
    }
    if (currentScope && hasCb(*currentScope)) {
        return getCb(*currentScope);
    }

    return nullptr;
}

void Compiler::instantiateTemplateInstances()
{
    // Sanity check we still referencing an unknown struct
    // Print all unknown references before bail out
    for (CellI& unknownStruct : m_unknownStructs) {
        WARN(compileStruct, "unknown struct: {}", unknownStruct[w.id.value][w.id.value].label());
    }

    int instantiedNum      = 0;
    int unknownInstanceNum = 0;
    for (CellI& unknownInstanceSlot : m_unknownInstances) {
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

        int paramCount = 0;
        for (CellI& param : templateParams) {
            CellI& paramId   = param[w.id.key];
            CellI& paramType = param[w.id.type];
            if (paramCount++ > 0) {
                ss << ", ";
            }
            ss << fmt::format("{}: {}", paramId.label(), getCompiledTypeFromResolvedType(paramType).label());
        }
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
        instantiedNum = unknownInstanceNum++ + 1;
    }

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

    for (CellI& slot : inputParams) {
        CellI& key  = slot[w.id.key];
        CellI& type = slot[w.id.type];
        inputParameters.add(key, type);
    }

    Ast::Struct& ret = compiledStruct;
    ret.set("instanceOf", structT);
    ret.set("templateParams", inputParams);
    ret.set("scope", static_cast<Ast::Scope&>(structT.get("scope")));

    // instantiate type aliases
    if (structT.has("typeAliases")) {
        Map& instantiatedTypeAliases = *new Map(w, w.std.Cell, w.std.ast.Base);
        for (CellI& slot : structT.typeAliases()) {
            CellI& key               = slot[w.id.key];
            CellI& type              = slot[w.id.type];
            CellI& instantiatedParam = instantiateTemplateParamType(type, ret, inputParameters);
            instantiatedTypeAliases.add(key, w.ast.slot(key, instantiatedParam));
        }
        ret.set("typeAliases", instantiatedTypeAliases);
    }

    // instantiate methods
    if (structT.has("methods")) {
        for (CellI& astFunctionRef : structT.methods()) {
            auto& astFunction = static_cast<Ast::Function&>(astFunctionRef);
            instantiateFunctionInStructT(astFunction, ret, inputParameters);
         }
    }

    // instantiate members
    if (structT.has("members")) {
        Map& instantiatedMembers = *new Map(w, w.std.Cell, w.std.ast.Slot);
        for (CellI& slot : structT.members()) {
            CellI& key               = slot[w.id.key];
            CellI& type              = slot[w.id.type];
            CellI& instantiatedParam = instantiateTemplateParamType(type, ret, inputParameters);
            instantiatedMembers.add(key, w.ast.slot(key, instantiatedParam));
        }
        ret.set("members", instantiatedMembers);
    }

    // instantiate memberOf list
    if (structT.has("memberOf")) {
        List& instantiatedMemberOfs = *new List(w, w.std.Struct);
        for (CellI& membershipType : structT.memberOf()) {
            CellI& instantiatedParam = instantiateTemplateParamType(membershipType, ret, inputParameters);
            instantiatedMemberOfs.add(instantiatedParam);
        }
        ret.set("memberOf", instantiatedMemberOfs);
    }

    // trait implementations
    if (structT.has("traitImpls")) {
        for (CellI& traitImpl : structT.traitImpls()) {
            for (CellI& astFunctionRef : traitImpl[w.id.methods]) {
                Map* associatedTypesPtr = nullptr;
                if (traitImpl.has("associatedTypes")) {
                    associatedTypesPtr = &static_cast<Map&>(traitImpl["associatedTypes"]);
                }

                auto& astFunction = static_cast<Ast::Function&>(astFunctionRef);
                instantiateFunctionInStructT(astFunction, ret, inputParameters, associatedTypesPtr);
            }
        }
    }

    return ret;
}

void Compiler::instantiateFunctionInStructT(Ast::Function& astFunction, Ast::Struct& compiledStruct, Map& inputParameters, Map* associatedTypesPtr)
{
    Ast::Function& instantiedFunction = *new Ast::Function(w, astFunction[w.id.name]);
    compiledStruct.addMethod(instantiedFunction, Ast::StructBase::ParameterModification::NoChange);

    // parameters
    if (astFunction.has("parameters")) {
        List& instantiatedParameters = *new List(w, w.std.ast.Slot);
        for (CellI& slot : astFunction[w.id.parameters]) {
            CellI& key               = slot[w.id.key];
            CellI& type              = slot[w.id.type];
            CellI& instantiatedParam = instantiateTemplateParamType(type, compiledStruct, inputParameters, associatedTypesPtr);
            instantiatedParameters.add(w.ast.slot(key, instantiatedParam));
        }
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
        List& resolvedParameterList = *new List(w, w.std.ast.Slot);
        auto& ret                   = *new Ast::TemplatedType(w, param[w.id.id], resolvedParameterList);
        auto& parametersList        = param[w.id.parameters];

        for (CellI& slot : parametersList) {
            CellI& key              = slot[w.id.key];
            CellI& type             = slot[w.id.type];
            CellI& resolvedSlotType = instantiateTemplateParamType(type, selfType, inputParameters, associatedTypesPtr);
            resolvedParameterList.add(w.ast.slot(key, resolvedSlotType));
        }

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

    if (&param.__type__() == &w.std.ast.ConstVar || &param.__type__() == &w.std.ast.TypeName || &param.__type__() == &w.std.ast.TypeAlias || &param.__type__() == &w.std.ast.SelfType) {
        return param;
    }

    throw "Unknown template parameter!";
}

Ast::Base& Compiler::instantiateAst(CellI& ast, CellI& selfType, Map& inputParameters, Map* associatedTypesPtr)
{
    auto instantiate = [this, &selfType, &inputParameters, associatedTypesPtr](CellI& ast) -> Ast::Base& { return instantiateAst(ast, selfType, inputParameters, associatedTypesPtr); };

    // do nothing just traverse and copy the AST nodes
    if (&ast.__type__() == &w.std.ast.Block) {
        auto& instantiedAsts = *new List(w, w.std.ast.Base);
        for (CellI& ast : ast[w.id.asts]) {
            instantiedAsts.add(instantiate(ast));
        }
        return *new Ast::Block(w, instantiedAsts);
    } else if (&ast.__type__() == &w.std.ast.TypeName) {
        auto& ret = w.ast.typeName(ast[w.id.value]);
        if (ast.has(w.id.scopes)) {
            ret.set(w.id.scopes, ast[w.id.scopes]);
        }
        return ret;
    } else if (&ast.__type__() == &w.std.ast.Self) {
        return w.ast.self();
    } else if (&ast.__type__() == &w.std.ast.Member) {
        return w.ast.member(ast[w.id.key]);
    } else if (&ast.__type__() == &w.std.ast.Parameter) {
        return w.ast.parameter(ast[w.id.key]);
    } else if (&ast.__type__() == &w.std.ast.ConstVar) {
        return w.ast.constVar(ast[w.id.value]);
    } else if (&ast.__type__() == &w.std.ast.Var) {
        return w.ast.var(ast[w.id.name]);
    } else if (&ast.__type__() == &w.std.ast.Continue) {
        return w.ast.continue_();
    } else if (&ast.__type__() == &w.std.ast.Break) {
        return w.ast.break_();
    } else if (&ast.__type__() == &w.std.ast.Return) {
        if (ast.has("value")) {
            return w.ast.return_(static_cast<Ast::Base&>(instantiate(ast[w.id.value])));
        }
        return w.ast.return_();
    } else if (&ast.__type__() == &w.std.ast.If) {
        if (ast.has("else_")) {
            return w.ast.if_(instantiate(ast[w.id.condition]))
                .then_(instantiate(ast[w.id.then]))
                .else_(instantiate(ast[w.id.else_]));
        } else {
            return w.ast.if_(instantiate(ast[w.id.condition])).then_(instantiate(ast[w.id.then]));
        }
    } else if (&ast.__type__() == &w.std.ast.Match) {
        auto& ret = w.ast.match_(static_cast<Ast::Base&>(ast[w.id.enum_]));
        for (CellI& kvPair : ast["cases"]) {
            auto& key = kvPair[w.id.key];
            auto& op  = instantiate(kvPair[w.id.value]);
            ret.case_(key, op);
        }
        return ret;
    } else if (&ast.__type__() == &w.std.ast.Do) {
        return w.ast.do_(instantiate(ast[w.id.statement])).while_(instantiate(ast[w.id.condition]));
    } else if (&ast.__type__() == &w.std.ast.While) {
        return w.ast.while_(instantiate(ast[w.id.condition])).do_(instantiate(ast[w.id.statement]));
    } else if (&ast.__type__() == &w.std.ast.For) {
        return w.ast.for_(instantiate(ast[w.id.variable])).in(instantiate(ast[w.id.container]))(instantiate(ast[w.id.statement]));
    } else if (&ast.__type__() == &w.std.ast.New) {
        auto* objectTypePtr = &ast[w.id.objectType];
        if (&(*objectTypePtr).__type__() == &w.std.ast.TemplatedType || &(*objectTypePtr).__type__() == &w.std.ast.AssociatedType) {
            CellI& resolvedObjectType = instantiateTemplateParamType(*objectTypePtr, selfType, inputParameters, associatedTypesPtr);
            objectTypePtr             = &resolvedObjectType;
        }
        auto& objectType = *static_cast<Ast::Base*>(objectTypePtr);
        if (ast.has("constructor")) {
            auto& constructor = ast[w.id.constructor];
            Ast::Base& ret    = w.ast.new_(objectType, static_cast<Ast::Base&>(constructor));
            if (ast.has(w.id.parameters)) {
                auto& newParameters = *new List(w, w.std.ast.Slot);
                for (CellI& slot : ast[w.id.parameters]) {
                    newParameters.add(w.ast.parameterInit(slot[w.id.key], instantiate(slot[w.id.value])));
                }
                ret.set(w.id.parameters, newParameters);
            }
            return ret;
        } else {
            return w.ast.new_(objectType);
        }
    } else if (&ast.__type__() == &w.std.ast.Call) {
        Ast::Base& ret = w.ast.call(instantiate(ast[w.id.self]), ast[w.id.method]);
        if (ast.has(w.id.parameters)) {
            // TODO process parameters
            ret.set(w.id.parameters, ast[w.id.parameters]);
        }
        return ret;
    } else if (&ast.__type__() == &w.std.ast.StaticCall) {
        Ast::Base& ret = w.ast.scall(instantiate(ast[w.id.self]), ast[w.id.method]);
        if (ast.has(w.id.parameters)) {
            // TODO process parameters
            ret.set(w.id.parameters, ast[w.id.parameters]);
        }
        return ret;
    }

    throw "Unknown AST to instantiate!";
}

void Compiler::compileInstructionsInScope(Ast::Scope& scope, Ast::Scope& resolvedScope)
{
    m_scope         = &scope;
    m_resolvedScope = &resolvedScope;

    if (scope.has("variables")) {
        for (CellI& kvPair : resolvedScope.items<Ast::Var>()) {
            Ast::Var& astVar = static_cast<Ast::Var&>(kvPair[w.id.value]);
            auto& varName    = getFullyQualifiedName(astVar);
            if (compiledVariables().hasKey(varName)) {
                return;
            }
            auto& compiledVariable = *new Object(w, w.std.op.Var, fmt::format("var {}", astVar.label()));
            compiledVariables().add(varName, compiledVariable);
        }
    }
    if (scope.has("structs")) {
        for (CellI& kvPair : resolvedScope.items<Ast::Struct>()) {
            Ast::Struct& astStruct = static_cast<Ast::Struct&>(kvPair[w.id.value]);
            compileInstructionsInStruct(astStruct);
        }
    }
    if (scope.has("enums")) {
        for (CellI& kvPair : resolvedScope.items<Ast::Enum>()) {
            Ast::Enum& astEnum = static_cast<Ast::Enum&>(kvPair[w.id.value]);
            compileInstructionsInEnum(astEnum);
        }
    }
    if (scope.has("functions")) {
        for (CellI& kvPair : resolvedScope.items<Ast::Function>()) {
            Ast::Function& astFunction = static_cast<Ast::Function&>(kvPair[w.id.value]);
            auto& compiledFunction     = compileInstructionsInFunction(astFunction);
            compiledFunctions().add(getFullyQualifiedName(astFunction), compiledFunction);
        }
    }
    if (scope.has("scopes")) {
        for (CellI& kvPair : scope.items<Ast::Scope>()) {
            Ast::Scope& nextScope = static_cast<Ast::Scope&>(kvPair[w.id.value]);
            if (nextScope.has("link")) {
                continue;
            }
            auto& nextResolvedScope = resolvedScope.getItem<Ast::Scope>(nextScope[w.id.name]);

            compileInstructionsInScope(nextScope, nextResolvedScope);

            m_scope         = &scope;
            m_resolvedScope = &resolvedScope;
        }
    }
}

void Compiler::compileInstructionsInStruct(Ast::Struct& astStruct)
{
    CellI& compiledStruct = astStruct["compiledStruct"];
    if (compiledStruct.missing("incomplete")) {
        return;
    }
    compiledStruct.erase("incomplete");

    CellI& compiledStructName = getFullyQualifiedName(astStruct);

    // compile sub types
    if (astStruct.has("typeAliases")) {
        Map& compiledTypeAliases = *new Map(w, w.std.Cell, w.std.Struct, "typeAliases Map<ConstVar, Type>(...)");
        for (CellI& slot : astStruct.typeAliases()) {
            CellI& key             = slot[w.id.key];
            CellI& type            = slot[w.id.type];
            auto& compiledSlotType = getCompiledTypeFromResolvedType(type);
            compiledTypeAliases.add(key, compiledSlotType);
        }
        compiledStruct.set("typeAliases", compiledTypeAliases);
        CellI& typeAliasesIndex = compiledTypeAliases[w.id.index];
        for (CellI& typeAlias : typeAliasesIndex.slotList()) {
            CellI& key      = typeAlias["key"];
            CellI& value    = typeAliasesIndex[key][w.id.value];
            List& aliasName = *new List(w, w.std.Char);
            for (CellI& character : compiledStructName) {
                aliasName.add(character);
            }
            aliasName.add(w.pools.chars.get(':'));
            aliasName.add(w.pools.chars.get(':'));
            for (CellI& character : key) {
                aliasName.add(character);
            }
            aliasName.label(compiledStructName.label() + "::" + key.label());
            if (IS_LOG_ENABLED) {
                TRACE(compileStruct, "type alias {}: {}", aliasName.label(), value.label());
            }
            compiledStructs().add(aliasName, value);
        }
    }

    // compile members
    if (astStruct.has("members")) {
        Map& compiledMembers = *new Map(w, w.std.Cell, w.std.ast.Slot, "members Map<ConstVar, Slot>(...)");
        for (CellI& slot : astStruct.members()) {
            CellI& key             = slot[w.id.key];
            CellI& type            = slot[w.id.type];
            auto& compiledSlotType = getCompiledTypeFromResolvedType(type);
            compiledMembers.add(key, w.ast.slot(key, compiledSlotType));
        }
        compiledStruct.set("slots", compiledMembers);
    }

    // compile memberOf list
    if (astStruct.has("memberOf")) {
        Map& compiledMemberOfs = *new Map(w, w.std.Struct, w.std.Struct, "memberOf Map<Type, Type>(...)");
        for (CellI& membershipType : astStruct.memberOf()) {
            auto& compiledMembershipType = getCompiledTypeFromResolvedType(membershipType);
            compiledMemberOfs.add(compiledMembershipType, compiledMembershipType);
        }
        compiledStruct.set("memberOf", compiledMemberOfs);
    }

    // compile methods
    if (astStruct.has("methods")) {
        Map& compiledMethods = *new Map(w, w.std.Cell, w.std.ast.Function);
        for (CellI& astFunction : astStruct.methods()) {
            auto& compiledFunction = compileInstructionsInFunction(static_cast<Ast::Function&>(astFunction));
            compiledMethods.add(astFunction[w.id.name], compiledFunction);
        }
        compiledStruct.set("methods", compiledMethods);
    }

    compiledStructs().add(compiledStructName, compiledStruct);
}

void Compiler::compileInstructionsInEnum(Ast::Enum& astEnum)
{
    CellI& compiledStruct = astEnum["compiledStruct"];
    if (compiledStruct.missing("incomplete")) {
        return;
    }
    compiledStruct.erase("incomplete");
    compiledStruct.set(w.id.enum_, w.true_);

    // compile values
    if (astEnum.has("values")) {
        Map& compiledMembers = *new Map(w, w.std.Cell, w.std.ast.Slot, "members Map<ConstVar, Slot>(...)");
        compiledMembers.add(w.id.tag, w.ast.slot(w.id.tag, w.std.Cell));
        compiledStruct.set("slots", compiledMembers);
        for (CellI& kvPair : astEnum.values()) {
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

                    compiledMembers.add(valueKey, w.ast.slot(valueKey, valueType));
                    compiledValue.set(valueName, value[w.id.value]);
                } else {
                    compiledMembers.add(valueKey, w.ast.slot(valueKey, compiledStruct));
                    compiledValue.set(valueName, w.id.emptyObject);
                }
            } else if (&valueCell.__type__() == &w.std.ast.TypedEnumValue) {
                auto& enumValue             = static_cast<Ast::TypedEnumValue&>(valueCell);
                auto& enumValueType         = valueCell["enumType"];
                auto& compiledEnumValueType = getCompiledTypeFromResolvedType(enumValueType);
                auto& fullName              = getFullyQualifiedName(enumValue);
                compiledMembers.add(valueKey, w.ast.slot(valueKey, compiledEnumValueType));
            }
        }
    }

    // compile methods
    if (astEnum.has("methods")) {
        Map& compiledMethods = *new Map(w, w.std.Cell, w.std.ast.Function);
        for (CellI& astFunction : astEnum.methods()) {
            auto& compiledFunction = compileInstructionsInFunction(static_cast<Ast::Function&>(astFunction));
            compiledMethods.add(astFunction[w.id.name], compiledFunction);
        }
        compiledStruct.set("methods", compiledMethods);
    }

    compiledStructs().add(getFullyQualifiedName(astEnum), compiledStruct);
}

CellI& Compiler::compileInstructionsInFunction(Ast::Function& astFunction)
{
    Object& compiledFunction = *new Object(w, w.std.op.Function);
    compiledFunction.set(w.id.name, astFunction.get(w.id.name));
    astFunction.set("compiledType", compiledFunction);

    // is this function a method?
    if (astFunction.has(w.id.structType)) {
        auto& currentStruct = static_cast<Ast::Struct&>(*m_currentStruct);
        auto& structType    = astFunction.get(w.id.structType);
        compiledFunction.set(w.id.objectType, structType["compiledStruct"]);
    }
    compileFunctionParams(astFunction, compiledFunction);

    compiledFunction.set(w.id.ast, astFunction);

    if (astFunction.has(w.id.instructions)) {
        compiledFunction.set(w.id.op, compileInstructionsInFunctionAst(astFunction, astFunction.instructions(), compiledFunction));
    }
    if (astFunction.has(w.id.static_)) {
        compiledFunction.set(w.id.static_, astFunction.get(w.id.static_));
    }

    return compiledFunction;
}

void Compiler::compileFunctionParams(Ast::Function& astFunction, Object& compiledFunction)
{
    std::stringstream iss;
    std::stringstream oss;
    std::string structTypeStr;
    if (astFunction.has("parameters") || astFunction.has("structType")) {
        Map& parameters = *new Map(w, w.std.Cell, w.std.ast.Slot);
        if (astFunction.has("structType")) {
            CellI& type = astFunction.get("structType");
            structTypeStr = fmt::format("{}::", type.label());
        }
        if (astFunction.has(w.id.parameters)) {
            int i = 0;
            for (CellI& slot : astFunction.parameters()) {
                if (i++ > 0) {
                    iss << ", ";
                }
                auto& key          = slot[w.id.key];
                auto& type         = slot[w.id.type];
                auto& compiledType = getCompiledTypeFromResolvedType(type);
                iss << "p_" << key.label() << ": " << compiledType.label();
                parameters.add(key, w.ast.slot(key, compiledType));
            }
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
            int i = 0;
            for (CellI& slot : function.parameters()) {
                if (i++ > 0) {
                    iss << ", ";
                }
                iss << "p_" << slot[w.id.key].label() << ": " << getCompiledTypeFromResolvedType(slot[w.id.type]).label();
            }
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

CellI& Compiler::compileInstructionsInFunctionAst(Ast::Function& astFunction, CellI& ast, Object& function)
{
    auto compile = [this, &astFunction, &function](CellI& ast) -> CellI& { return compileInstructionsInFunctionAst(astFunction, ast, function); };
    const auto _ = [this](auto& cell) -> Ast::ConstVar& { return w.ast._(cell); };

    if (&ast.__type__() == &w.std.ast.Block) {
        CellI& list = ast[w.id.asts];
        if (&list[w.id.size] == &w._1_) {
            return compile(list[w.id.first][w.id.value]);
        }
        CellI* prevBlock = nullptr;
        if (m_lastBlock) {
            prevBlock = m_lastBlock;
        }
        CellI* firstOpBlockNode   = nullptr;
        CellI* currentOpBlockNode = nullptr;
        Object& opBlock           = *new Object(w, w.std.op.Block);
        m_lastBlock = &opBlock;
        for (CellI& ast : list) {
            CellI& newOpBlockNode = *new Object(w, w.std.op.Activate);
            newOpBlockNode.set(w.id.cell, compile(ast));
            newOpBlockNode.set(w.id.parent, opBlock);

            if (!firstOpBlockNode) {
                firstOpBlockNode = &newOpBlockNode;
            } else {
                (*currentOpBlockNode).set(w.id.next, newOpBlockNode);
            }
            currentOpBlockNode = &newOpBlockNode;
        }
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
    } else if (&ast.__type__() == &w.std.ast.ResolvedType) {
        Object& constVar = *new Object(w, w.std.op.ConstVar);
        constVar.set(w.id.ast, ast);
        constVar.set(w.id.value, ast[w.id.compiled]);
        return constVar;
    } else if (&ast.__type__() == &w.std.ast.Self) {
        CellI& retOp = compile(w.ast.get(_(function), _(w.id.stack)) / _(w.id.value) / _(w.id.input) / _(w.id.self));
        retOp.set(w.id.ast, ast);
        retOp.label("self");
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Member) {
        CellI& member = compile(w.ast.get(w.ast.self(), w.ast.constVar(ast[w.id.key])));
        member.set(w.id.ast, ast);
        return member;
    } else if (&ast.__type__() == &w.std.ast.Parameter) {
        CellI& retOp = compile(w.ast.get(_(function), _(w.id.stack)) / _(w.id.value) / _(w.id.input) / _(ast[w.id.key]));
        retOp.set(w.id.ast, ast);
        return retOp;
    } else if (&ast.__type__() == &w.std.ast.ConstVar) {
        Object& constVar = *new Object(w, w.std.op.ConstVar);
        constVar.set(w.id.ast, ast);
        constVar.set(w.id.value, ast[w.id.value]);
        return constVar;
    } else if (&ast.__type__() == &w.std.ast.Var) {
        Index* localVarsIndexPtr = nullptr;
        if (function.missing(w.id.localVars)) {
            localVarsIndexPtr = new Index(w, fmt::format("LocalVarsIndex of {}", function.label()));
            function.set(w.id.localVars, *localVarsIndexPtr);
        } else {
            localVarsIndexPtr = &static_cast<Index&>(function[w.id.localVars]);
        }

        Index& localVarsIndex = *localVarsIndexPtr;
        if (!localVarsIndex.has(ast[w.id.name])) {
            localVarsIndex.insert(ast[w.id.name], w.ast.slot(ast[w.id.name], w.std.op.Var));
        }

        CellI& retOp = compile(w.ast.get(_(function), _(w.id.stack)) / _(w.id.value) / _(w.id.localVars) / _(ast[w.id.name]));
        retOp.set(w.id.ast, ast);
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
    } else if (&ast.__type__() == &w.std.ast.Return) {
        Object& retOp = *new Object(w, w.std.op.Return, "op.return");
        retOp.set(w.id.ast, ast);
        if (ast.has(w.id.value)) {
            retOp.set(w.id.result, compile(w.ast.set(_(function), _(w.id.value), static_cast<Ast::Base&>(ast[w.id.value]))));
        }
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
        auto& enumObj        = static_cast<Ast::Base&>(ast[w.id.enum_]);
        auto& caseList       = ast["cases"];
        auto& astCases       = *new List(w, w.std.ast.Base);
        Ast::Block& astBlock = *new Ast::Block(w, astCases);

        for (CellI& kvPair : caseList) {
            auto& kind    = kvPair[w.id.key];
            auto& op      = kvPair[w.id.value];
            auto& oneCase = w.ast.if_(w.ast.same(w.ast.get(enumObj, "tag"), _(kind))).then_(static_cast<Ast::Base&>(op));
            astCases.add(oneCase);
        }
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
            w.ast.if_(w.ast.same(w.ast.call(*w.ast.var("iterator"), "hasNextNode"), _(w.true_)))
                            .then_(w.ast.call(*w.ast.var("iterator"), "goToNextNode"))
                            .else_(w.ast.break_());

        forLoopBlock.mergeFrom(statement);
        forLoopBlock.mergeFrom(forLoopBlockEnd);

        Ast::Block& retAst = w.ast.block(
            w.ast.var("iterator") = w.ast.call(ast[w.id.container], "iterator"),
            w.ast.if_(w.ast.same(w.ast.call(*w.ast.var("iterator"), "isContainerEmpty"), _(w.false_)))
                .then_(w.ast.block(
                    w.ast.call(*w.ast.var("iterator"), "goToFirstNode"),
                    w.ast.do_(forLoopBlock)
                        .while_(_(w.true_))))
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
        opSet.set(w.id.cell, compile(w.ast.constVar(block)));
        opSet.set(w.id.key, compile(w.ast.constVar(w.id.value)));

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
            callAst.set(w.id.self, w.ast.get(w.ast.constVar(block), w.ast.constVar(w.id.value)));
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
    } else if ((&ast.__type__() == &w.std.ast.Call) && (&ast[w.id.method].__type__() == &w.std.ast.PrimitiveToolName)) {
        CellI& primitiveTool = ast[w.id.method][w.id.name];
        Object& retOp        = *new Object(w, primitiveTool);
        retOp.set(w.id.ast, ast);

        Map& membersMapping = static_cast<Map&>(primitiveTool[w.id.ast][w.id.memberMapping]);

        CellI& self = ast[w.id.self];
        retOp.set(membersMapping.getValue(w.id.self), compile(self));

        if (ast.has(w.id.parameters)) {
            for (CellI& slot : ast[w.id.parameters]) {
                CellI& key   = slot[w.id.key];
                CellI& value = slot[w.id.value];
                retOp.set(membersMapping.getValue(key), compile(value));
            }
        }

        return retOp;
    } else if (&ast.__type__() == &w.std.ast.Call || &ast.__type__() == &w.std.ast.StaticCall) {
        Ast::Base& astSelf   = static_cast<Ast::Base&>(ast[w.id.self]);
        Ast::Base& astMethod = static_cast<Ast::Base&>(ast[w.id.method]);
        auto& astMethodId    = astMethod[w.id.value];
#if 1
        bool checked = false;
        // New<T>.constructor(...)
        std::stringstream ss;
#if 0
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
        } else
#endif
        if (&astSelf.__type__() == &w.std.ast.Member) {
            auto& astMemberId    = astSelf[w.id.key];
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
        } else if (&astSelf.__type__() == &w.std.ast.Self) {
            auto& astSelfType = function[w.id.ast][w.id.structType];
            checkMethodCall(astSelfType, astMethodId);
            checked = true;
        } else if (&astSelf.__type__() == &w.std.ast.Parameter) {
            auto& parameterName                 = astSelf[w.id.key];
            auto& astFunctionParameters         = function[w.id.ast][w.id.parameters];
            Ast::Slot* astFunctionParameterSlot = nullptr;
            for (CellI& slot : astFunctionParameters) {
                CellI& key  = slot[w.id.key];
                CellI& type = slot[w.id.type];
                if (&key == &parameterName) {
                    astFunctionParameterSlot = &static_cast<Ast::Slot&>(slot);
                    break;
                }
            }
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
        retOp.set(w.id.self, compile(ast[w.id.self]));
        retOp.set(w.id.method, compile(ast[w.id.method]));
        retOp.set(w.id.stack, function);
        if (ast.has(w.id.parameters)) {
            Map& parameters  = *new Map(w, w.std.Cell, w.std.ast.Slot);
            for (CellI& param : ast[w.id.parameters]) {
                CellI& slot = *new Object(w, w.std.ast.Slot);
                CellI& key   = param[w.id.key];
                CellI& value = param[w.id.value];
                slot.set(w.id.key, key);
                slot.set(w.id.value, compile(value));
                parameters.add(key, slot);
            }
            retOp.set(w.id.parameters, parameters);
        }

        return retOp;
    }

    throw "Unknown function AST!";
}

void Compiler::checkMethodCall(CellI& astType, CellI& astMethodId)
{
    CellI* typePtr  = nullptr;
    Map* methodsPtr = nullptr;
    if (astType.isA(w.std.ast.ConstVar)) {
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

void Compiler::compileDescriptionInScope(Ast::Scope& scope, Ast::Scope& resolvedScope)
{
    if (scope.has("functions")) {
        for (CellI& kvPair : resolvedScope.items<Ast::Function>()) {
            Ast::Function& astFunction = static_cast<Ast::Function&>(kvPair[w.id.value]);
            compileDescriptionInFunction(astFunction);
        }
    }
    if (scope.has("structs")) {
        for (CellI& kvPair : resolvedScope.items<Ast::Struct>()) {
            Ast::Struct& astStruct = static_cast<Ast::Struct&>(kvPair[w.id.value]);
            if (astStruct.has("methods")) {
                Map& compiledMethods = *new Map(w, w.std.Cell, w.std.ast.Function);
                for (CellI& astFunction : astStruct.methods()) {
                    compileDescriptionInFunction(static_cast<Ast::Function&>(astFunction));
                }
            }
        }
    }
    if (scope.has("enums")) {
        for (CellI& kvPair : resolvedScope.items<Ast::Enum>()) {
            Ast::Enum& astEnum = static_cast<Ast::Enum&>(kvPair[w.id.value]);
            if (astEnum.has("methods")) {
                Map& compiledMethods = *new Map(w, w.std.Cell, w.std.ast.Function);
                for (CellI& astFunction : astEnum.methods()) {
                    compileDescriptionInFunction(static_cast<Ast::Function&>(astFunction));
                }
            }
        }
    }
    if (scope.has("scopes")) {
        for (CellI& kvPair : scope.items<Ast::Scope>()) {
            Ast::Scope& nextScope = static_cast<Ast::Scope&>(kvPair[w.id.value]);
            if (nextScope.has("link")) {
                continue;
            }
            auto& nextResolvedScope = resolvedScope.getItem<Ast::Scope>(nextScope[w.id.name]);

            compileDescriptionInScope(nextScope, nextResolvedScope);

            m_scope         = &scope;
            m_resolvedScope = &resolvedScope;
        }
    }
}

void Compiler::compileDescriptionInFunction(Ast::Function& astFunction)
{
    if (astFunction.missing("compiledType")) {
        return;
    }
    Object& compiledFunction = static_cast<Object&>(astFunction["compiledType"]);

    if (astFunction.has(w.id.description)) {
        compiledFunction.set(w.id.description, compileDescriptionInFunctionAst(astFunction.description(), compiledFunction));
        m_toolFinder.add(compiledFunction);
    }
}

CellI& Compiler::compileDescriptionInFunctionAst(CellI& ast, Object& compiledFunction)
{
    auto compile = [this, &compiledFunction](CellI& astFunction) -> CellI& { return compileDescriptionInFunctionAst(astFunction, compiledFunction); };

    if (&ast.__type__() == &w.std.ast.Block) {
        CellI& list   = ast[w.id.asts];
        List& retList = *new List(w, w.std.Cell, "description");
        for (CellI& ast : list) {
            retList.add(compile(ast));
        }

        return retList;
    } else if (&ast.__type__() == &w.std.ast.ConstVar) {
        Object& constVar = *new Object(w, w.std.op.ConstVar);
        constVar.set(w.id.ast, ast);
        constVar.set(w.id.value, ast[w.id.value]);
        return constVar;
    } else if (&ast.__type__() == &w.std.ast.ResolvedType) {
        Object& constVar = *new Object(w, w.std.op.ConstVar);
        constVar.set(w.id.ast, ast);
        constVar.set(w.id.value, ast[w.id.compiled]);
        return constVar;
    } else if (&ast.__type__() == &w.std.ast.Self) {
        return ast;
    } else if (&ast.__type__() == &w.std.ast.Parameter) {
        return ast;
    } else if (&ast.__type__() == &w.std.ast.Return) {
        return ast;
    } else if ((&ast.__type__() == &w.std.ast.Call) && (&ast[w.id.method].__type__() == &w.std.ast.PrimitiveToolName)) {
        Object& retOp = *new Object(w, w.std.op.Call);
        retOp.set(w.id.ast, ast);
        retOp.set(w.id.self, compile(ast[w.id.self]));
        retOp.set(w.id.method, ast[w.id.method][w.id.name]);
        if (ast.has(w.id.parameters)) {
            Map& parameters = *new Map(w, w.std.Cell, w.std.ast.Slot);
            for (CellI& param : ast[w.id.parameters]) {
                CellI& slot  = *new Object(w, w.std.ast.Slot);
                CellI& key   = param[w.id.key];
                CellI& value = param[w.id.value];
                slot.set(w.id.key, key);
                // slot.set(w.id.type, compile(param[w.id.type])); TODO
                slot.set(w.id.value, compile(value));
                parameters.add(key, slot);
            }
            retOp.set(w.id.parameters, parameters);
        }

        return retOp;
    }

    throw "Unknown function AST!";
}

} // namespace cells
} // namespace infocell