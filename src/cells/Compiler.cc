#include "Compiler.h"

#include "util/Panic.h"
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
    id(w.id),
    std(w.std),
    m_toolFinder(*new ToolFinder(w)),
    m_earlyStructs(w, std.Cell, std.KVPair, "earlyStructs"),
    m_earlyEnumValues(w, std.Cell, std.Cell, "earlyEnumValues"),
    m_structs(*new TrieMap(w, std.Cell, std.Struct, "structs")),
    m_unknownStructs(*new TrieMap(w, std.Cell, std.Struct, "unknownStructs")),
    m_unknownInstances(*new TrieMap(w, std.Cell, std.Struct, "unknownInstances"))
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

    m_libraryPtr->set(id.scope, scope);
    m_libraryPtr->set(id.resolvedScope, resolvedScope);
    m_libraryPtr->m_toolFinderPtr = &m_toolFinder;

    return *m_libraryPtr;
}

Object& Compiler::compileAsPrompt(Ast::Function& prompt)
{
    auto& resolvedPrompt = resolveTypesInFunction(prompt);

    instantiateTemplateInstances();

    return static_cast<Object&>(compilePromptInFunctionAst(resolvedPrompt.description(), resolvedPrompt));
}


CellI& Compiler::reigisterStructBeforeCompilation(CellI& structAst)
{
    CellI* structIdPtr = nullptr;
    if (&structAst.__type__() == &std.ast.TemplatedType) {
        List& idCell = *new List(w, std.Cell);
        structIdPtr  = &idCell;
        std::stringstream ss;
        for (CellI& scope : structAst[id.scopes]) {
            for (CellI& character : scope) {
                idCell.add(character);
                ss << character.label();
            }
            idCell.add(w.pools.chars.get(':'));
            idCell.add(w.pools.chars.get(':'));
            ss << "::";
        }
        for (CellI& character : structAst[id.id]) {
            idCell.add(character);
            ss << character.label();
        }

        ss << "<";
        int i = 0;
        for (CellI& slot : structAst[id.parameters]) {
            if (i++ != 0) {
                ss << ", ";
            }
            CellI& key              = slot[id.key];
            CellI& type             = slot[id.type];
            CellI& compiledSlotType = reigisterStructBeforeCompilation(type);

            idCell.add(key);
            idCell.add(compiledSlotType);
            ss << fmt::format("{}={}", key.label(), compiledSlotType.label());
        }
        ss << ">";
        idCell.label(ss.str());
    } else if (&structAst.__type__() == &std.ast.TypeName) {
        structIdPtr = &structAst[id.name];
    } else if (&structAst.__type__() == &std.ast.ConstVar) {
        return structAst[id.value];
    } else {
        panic("Unsupported type!");
    }
    CellI& structId = *structIdPtr;
    if (m_earlyStructs.hasKey(structId)) {
        return m_earlyStructs.getValue(structId)[id.type];
    } else {
        auto& unresolvedStruct = *new Object(w, std.Struct, fmt::format("{}", structId.label()));
        unresolvedStruct.set(id.incomplete, w.true_);

        m_earlyStructs.add(structId, w.ast.slot(structAst, unresolvedStruct));
        return unresolvedStruct;
    }
}

void Compiler::registerBuiltInStruct(const std::string& fullName, CellI& compiledStruct, Ast::Scope* parentScope)
{
    std::vector<std::string> sliced;
    splitNamespacedString(sliced, fullName);

    if (sliced.empty()) {
        panic("Invalid struct ID!");
    }
    std::stringstream ss;
    List& idCell             = *new List(w, std.Cell);
    const auto& typeName     = sliced.back();
    Ast::Scope* currentScope = parentScope ? parentScope : &w.globalScope;
    if (sliced.size() > 1) {
        for (int i = 0; i < sliced.size() - 1; ++i) {
            const auto& scopeName = sliced[i];
            currentScope          = &currentScope->getItem<Ast::Scope>(scopeName);
            for (CellI& character : (*currentScope)[id.name]) {
                idCell.add(character);
                ss << character.label();
            }
            idCell.add(w.pools.chars.get(':'));
            idCell.add(w.pools.chars.get(':'));
            ss << "::";
        }
    }
    Ast::StructBase* structBaseAstPtr = nullptr;
    if (&compiledStruct[id.__type__] == &std.Struct) {
        Ast::Struct& structAst = currentScope->getItem<Ast::Struct>(typeName);
        structBaseAstPtr       = &structAst;
    } else if (&compiledStruct[id.__type__] == &std.Enum) {
        Ast::Enum& enumAst = currentScope->getItem<Ast::Enum>(typeName);
        structBaseAstPtr   = &enumAst;
    } else {
        panic("The compiled type not a struct or enum!");
    }
    Ast::StructBase& structBaseAst = *structBaseAstPtr;

    for (CellI& character : structBaseAst[id.name]) {
        idCell.add(character);
        ss << character.label();
    }
    idCell.label(ss.str());
    compiledStruct.set(id.incomplete, w.true_);
    m_earlyStructs.add(idCell, w.ast.slot(w.__type__(fullName), compiledStruct));
}

void Compiler::registerBuiltInEnumValue(const std::string& fullName, CellI& compiledEnumValue)
{
    std::vector<std::string> sliced;
    splitNamespacedString(sliced, fullName);

    if (sliced.size() < 2) {
        panic("Invalid enum value ID!");
    }
    const auto& enumName      = sliced[sliced.size() - 2];
    const auto& enumValueName = sliced.back();
    List& enumIdCell          = *new List(w, std.Cell);
    String& enumValueIdCell   = w.name(enumValueName);
    Ast::Scope* currentScope  = &w.globalScope;

    if (sliced.size() > 1) {
        for (int i = 0; i < sliced.size() - 2; ++i) {
            const auto& scopeName = sliced[i];
            currentScope          = &currentScope->getItem<Ast::Scope>(scopeName);
            for (CellI& character : (*currentScope)[id.name]) {
                enumIdCell.add(character);
            }
            enumIdCell.add(w.pools.chars.get(':'));
            enumIdCell.add(w.pools.chars.get(':'));
        }
    }
    Ast::Enum& enumAst = currentScope->getItem<Ast::Enum>(enumName);
    if (!enumAst.values().hasKey(enumValueIdCell)) {
        panic("Invalid enum value!");
    }
    Ast::EnumValue& enumValueAst = static_cast<Ast::EnumValue&>(enumAst.values().getValue(enumValueIdCell));
    for (CellI& character : enumAst[id.name]) {
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
        auto& structId       = earlyStructKV[id.key];
        auto& structRefAst   = earlyStructKV[id.value][id.key];
        auto& compiledStruct = earlyStructKV[id.value][id.type];

        TRACE(compileStruct, "early struct: {}", earlyStructKV[id.key].label());

        auto& structReference = *new Object(w, std.StructReference);
        structReference.set(id.value, compiledStruct);
        structReference.set(id.id, structId);

        if (&structRefAst.__type__() == &std.ast.TemplatedType) {
            m_unknownInstances.add(structId, structReference);
        } else if (&structRefAst.__type__() == &std.ast.TypeName) {
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
        auto& structId       = earlyStructKV[id.key];
        auto& structRefAst   = earlyStructKV[id.value][id.key];
        auto& compiledStruct = earlyStructKV[id.value][id.type];

        TRACE(compileStruct, "resolve early struct: {}", earlyStructKV[id.key].label());

        if (&structRefAst.__type__() == &std.ast.TemplatedType) {
            if (m_unknownInstances.hasKey(structId)) {
                CellI& structReference = m_unknownInstances.getValue(structId);

                auto& scope         = structReference[id.value][id.ast][id.scope];
                auto& resolvedScope = scope[id.resolvedScope];

                structReference.set(id.scope, scope);
                structReference.set(id.resolvedScope, resolvedScope);
                structReference.set(id.templateId, structRefAst[id.id]);
                structReference.set(id.templateParams, structRefAst[id.parameters]);
            }
        } else if (&structRefAst.__type__() == &std.ast.TypeName) {
            if (m_unknownStructs.hasKey(structId)) {
                CellI& structReference = m_unknownStructs.getValue(structId);
            }
        }
    }
}

Ast::Scope& Compiler::resolveTypesInScope(Ast::Scope& scope)
{
    auto& resolvedScope = *new Ast::Scope(w, scope.label());
    scope.set(id.resolvedScope, resolvedScope);
    m_scope = &scope;
    m_resolvedScope = &resolvedScope;

    if (scope.has(id.variables)) {
        for (CellI& origAstVarCell : scope.items<Ast::Var>()) {
            Ast::Var& origAstVar = static_cast<Ast::Var&>(origAstVarCell[id.value]);
            resolvedScope.add<Ast::Var>(origAstVar);
        }
    }
    if (scope.has(id.structs)) {
        for (CellI& origAstStructCell : scope.items<Ast::Struct>()) {
            Ast::Struct& origAstStruct     = static_cast<Ast::Struct&>(origAstStructCell[id.value]);
            Ast::Struct& resolvedAstStruct = resolveTypesInStruct(origAstStruct);
            resolvedScope.add<Ast::Struct>(resolvedAstStruct);
        }
    }
    if (scope.has(id.enums)) {
        for (CellI& origAstEnumCell : scope.items<Ast::Enum>()) {
            Ast::Enum& origAstEnum     = static_cast<Ast::Enum&>(origAstEnumCell[id.value]);
            Ast::Enum& resolvedAstEnum = resolveTypesInEnum(origAstEnum);
            resolvedScope.add<Ast::Enum>(resolvedAstEnum);
        }
    }
    if (scope.has(id.functions)) {
        m_currentStruct = nullptr;
        for (CellI& origAstFunctionCell : scope.items<Ast::Function>()) {
            Ast::Function& origAstFunction     = static_cast<Ast::Function&>(origAstFunctionCell[id.value]);
            Ast::Function& resolvedAstFunction = resolveTypesInFunction(origAstFunction);
            resolvedScope.add<Ast::Function>(resolvedAstFunction);
        }
    }
    if (scope.has(id.scopes)) {
        for (CellI& origAstScopeCell : scope.items<Ast::Scope>()) {
            Ast::Scope& origAstScope     = static_cast<Ast::Scope&>(origAstScopeCell[id.value]);
            if (origAstScope.has(id.link)) {
                continue;
            }
            Ast::Scope& resolvedAstScope = resolveTypesInScope(origAstScope);
            resolvedScope.add<Ast::Scope>(resolvedAstScope);
            m_scope = &scope;
        }
    }

    return resolvedScope;
}

Ast::Function& Compiler::resolveTypesInFunction(Ast::Function& function, Ast::StructBase* astStructPtr)
{
    Ast::Function& ret = *new Ast::Function(w, function[id.name]);
    m_currentFn = &ret;
    std::stringstream ss;

    if (function.has(id.isConstructor)) {
        ret.set(id.isConstructor, function[id.isConstructor]);
    }
    if (function.has(id.static_)) {
        ret.set(id.static_, function[id.static_]);
        ss << "static ";
    }
    ss << "fn ";

    if (function.has(id.parameters)) {
        int i = 0;
        for (CellI& paramKV : function.parameters()) {
            CellI& param             = paramKV[id.value];
            CellI& paramKey          = param[id.key];
            CellI& paramType         = param[id.type];
            CellI& resolvedParamType = resolveType(paramType);
            CellI& compiledParamType = getCompiledTypeFromResolvedType(resolvedParamType);
            if (i++ > 0) {
                ss << ", ";
            }
            if (&paramKey == &id.self) {
                if (i != 1) {
                    panic("The self parameter must be the first!");
                }
                ss << fmt::format("{}::", compiledParamType.label());
            }
            if (i == 1) {
                ss << fmt::format("{}(", function[id.name].label());
            }
            ss << fmt::format("{}: {}", paramKey.label(), compiledParamType.label());
            ret.addParameter(w.ast.parameterDeclaration(paramKey, resolvedParamType));
        }
    }
    ss << ")";
    if (function.has(id.returnType)) {
        ss << " -> ";
        CellI& retType         = function.returnType();
        CellI& resolvedRetType = resolveType(retType);
        CellI& compiledRetType = getCompiledTypeFromResolvedType(resolvedRetType);
        ss << compiledRetType.label();
        ret.returnType(resolvedRetType);
    }
    ret.label(ss.str());

    CellI* instructionsPtr = nullptr;
    if (function.has(id.primitiveTool)) {
        // we have now enough information to auto-generate the instruction(s) for primitve tools here
        ret.set(id.primitiveTool, function[id.primitiveTool]);
        ret.set(id.memberMapping, function[id.memberMapping]);

        Ast::Call& instructionCall = w.ast.call(w.ast.self(), w.ast.primitiveToolName(function[id.primitiveTool]));
        if (function.has(id.parameters)) {
            for (CellI& parameter : function[id.parameters]) {
                CellI& key = parameter[id.key];
                instructionCall(key.label(), w.ast.parameter(key));
            }
        }
        instructionsPtr = &instructionCall;
    } else if (function.has(id.instructions)) {
        instructionsPtr = &function.instructions();
    }

    if (instructionsPtr) {
        CellI& resolvedinstructionsAst = resolveTypesInFunctionCode(*instructionsPtr, ret, astStructPtr);
        ret.set(id.instructions, resolvedinstructionsAst);
    }

    if (function.has(id.description)) {
        auto& resolvedDescriptionAst = resolveDescriptionTypesInFunctionCode(function.description(), ret, astStructPtr);
        ret.set(id.description, resolvedDescriptionAst);
    }

    return ret;
}

Ast::Base& Compiler::resolveTypesInFunctionCode(CellI& ast, Ast::Function& astFunction, Ast::StructBase* astStructPtr)
{
    auto resolve = [this, &astFunction, &astStructPtr](CellI& ast) -> Ast::Base& { return resolveTypesInFunctionCode(ast, astFunction, astStructPtr); };

    if (&ast.__type__() == &std.ast.Block) {
        // do nothing just traverse and copy the AST nodes
        auto& instantiedAsts = *new List(w, std.ast.Base);
        for (CellI& ast : ast[id.asts]) {
            instantiedAsts.add(resolve(ast));
        }
        return *new Ast::Block(w, instantiedAsts);
    } else if (&ast.__type__() == &std.ast.TypeName) {
        return resolveType(ast);
    } else if (&ast.__type__() == &std.ast.Self) {
        if (!astStructPtr) {
            panic("Referencing self pointer, but this function doesn't associated with a struct/enum!");
        }
        auto& ret = w.ast.self();
        ret.set(id.type, (*astStructPtr)[id.compiledStruct]);
        return ret;
    } else if (&ast.__type__() == &std.ast.Member) {
        if (!astStructPtr) {
            panic("Referencing a member, but this function doesn't associated with a struct/enum!");
        }
        Ast::StructBase& astStruct = *astStructPtr;
        CellI& memberKey           = ast[id.key];
        if (&memberKey == &id.__type__) {
            CellI& compiledStruct = astStruct[id.compiledStruct];
            if (&compiledStruct == &std.Index) {
                return w.ast.member(ast[id.key]);
            } else {
                return w.ast._(compiledStruct);
            }
        }
        auto& member = static_cast<Ast::Member&>(astStruct.members().getValue(memberKey));
        return member;
    } else if (&ast.__type__() == &std.ast.Parameter) {
        Map& parameters = static_cast<Map&>(astFunction[id.parameters]);
        auto& key       = ast[id.key];
        auto& ret       = w.ast.parameter(key);
        auto& parameter = parameters.getValue(key);
        auto& type      = getCompiledTypeFromResolvedType(parameter[id.type]);
        ret.set(id.type, type);
        return ret;
    } else if (&ast.__type__() == &std.ast.ConstVar) {
        auto& ret = w.ast.const_(ast[id.value]);
        ret.set(id.type, ast[id.value].__type__());
        return ret;
    } else if (&ast.__type__() == &std.ast.Var) {
        return w.ast.var_(ast[id.name]);
    } else if (&ast.__type__() == &std.ast.Continue) {
        return w.ast.continue_();
    } else if (&ast.__type__() == &std.ast.Break) {
        return w.ast.break_();
    } else if (&ast.__type__() == &std.ast.Return) {
        if (ast.has(id.value)) {
            return w.ast.return_(resolve(ast[id.value]));
        }
        return w.ast.return_();
    } else if (&ast.__type__() == &std.ast.If) {
        if (ast.has(id.else_)) {
            return w.ast.if_(resolve(ast[id.condition]))
                .then_(resolve(ast[id.then]))
                .else_(resolve(ast[id.else_]));
        } else {
            return w.ast.if_(resolve(ast[id.condition])).then_(resolve(ast[id.then]));
        }
    } else if (&ast.__type__() == &std.ast.Match) {
        auto& ret = w.ast.match_(static_cast<Ast::Base&>(ast[id.enum_]));
        for (CellI& kvPair : ast[id.cases]) {
            auto& key = kvPair[id.key];
            auto& op  = resolve(kvPair[id.value]);
            ret.case_(key, op);
        }
        return ret;
    } else if (&ast.__type__() == &std.ast.Do) {
        return w.ast.do_(resolve(ast[id.statement])).while_(resolve(ast[id.condition]));
    } else if (&ast.__type__() == &std.ast.While) {
        return w.ast.while_(resolve(ast[id.condition])).do_(resolve(ast[id.statement]));
    } else if (&ast.__type__() == &std.ast.For) {
        return w.ast.for_(resolve(ast[id.variable])).in(resolve(ast[id.container]))(resolve(ast[id.statement]));
    } else if (&ast.__type__() == &std.ast.New) {
        CellI& objectType                = ast[id.objectType];
        Ast::Base* resolvedObjectTypePtr = nullptr;
        if (&objectType.__type__() == &std.ast.Member) {
            resolvedObjectTypePtr = &static_cast<Ast::Base&>(objectType);
        } else {
            resolvedObjectTypePtr = &resolveType(objectType);
        }
        Ast::Base& resolvedObjectType = *resolvedObjectTypePtr;

        if (ast.has(id.constructor)) {
            auto& constructor = ast[id.constructor];
            Ast::Base& ret    = w.ast.new_(resolvedObjectType, static_cast<Ast::Base&>(ast[id.constructor]));

            if (ast.has(id.parameters)) {
                auto& newParameters = *new List(w, std.ast.Parameter);
                for (CellI& parameter : ast[id.parameters]) {
                    newParameters.add(w.ast.parameterInit(parameter[id.key], resolve(parameter[id.value])));
                }
                ret.set(id.parameters, newParameters);
            }
            return ret;
        } else {
            return w.ast.new_(resolvedObjectType);
        }
    } else if (&ast.__type__() == &std.ast.Call) {
        Ast::Base* retPtr = nullptr;
#if 0
            Ast::StructBase& astStruct = *astStructPtr;
            CellI& compiledStruct      = astStruct[id.compiledStruct];
            if (&compiledStruct == &std.Index) {
            } else {
                CellI& resolvedSelf = resolve(ast[id.self]);
//                CellI& selfType     = resolvedSelf[id.type];
            }
#endif
        retPtr = &w.ast.call(ast[id.method]);
        Ast::Base& ret = *retPtr;
        if (ast.has(id.parameters)) {
            auto& newParameters = *new List(w, std.ast.Parameter);
            for (CellI& parameter : ast[id.parameters]) {
                newParameters.add(w.ast.parameterInit(parameter[id.key], resolve(parameter[id.value])));
            }
            ret.set(id.parameters, newParameters);
        }
        return ret;
    }

    panic("Unknown AST to instantiate!");
}

Ast::Struct& Compiler::resolveTypesInStruct(Ast::Struct& astStruct)
{
    auto& structName = astStruct[id.name];
    Ast::Struct& ret = *new Ast::Struct(w, structName);
    bool isInstance  = astStruct.has(id.instanceOf);

    if (isInstance) {
        ret.set(id.instanceOf, astStruct[id.instanceOf]);
        ret.set(id.templateParams, astStruct[id.templateParams]);
        ret.set(id.scope, astStruct[id.scope]);
    }

    auto& fullyQualifiedName = getFullyQualifiedName(astStruct);
    CellI* compiledStructPtr = nullptr;
    if (compiledStructs().hasKey(fullyQualifiedName)) {
        compiledStructPtr = &compiledStructs().getValue(fullyQualifiedName);
    } else if (isInstance && m_unknownInstances.hasKey(fullyQualifiedName)) {
        CellI& unknownInstance = m_unknownInstances.getValue(fullyQualifiedName);
        compiledStructPtr      = &unknownInstance[id.value];
    } else if(m_unknownStructs.hasKey(fullyQualifiedName)) {
        CellI& unknownStruct = m_unknownStructs.getValue(fullyQualifiedName);
        compiledStructPtr    = &unknownStruct[id.value];
        m_unknownStructs.remove(fullyQualifiedName);
    } else {
        compiledStructPtr = new Object(w, std.Struct, fmt::format("{}", fullyQualifiedName.label()));
        compiledStructPtr->set(id.incomplete, w.true_);
    }
    m_structs.add(fullyQualifiedName, *compiledStructPtr);
    ret.set(id.compiledStruct, *compiledStructPtr);
    if (compiledStructPtr->label() == "std::List<valueType=Number>") {
        std::cout << "";
    }
    ret.set(id.fullyQualifiedName, fullyQualifiedName);

    m_currentStruct = &ret;

    std::stringstream ss;
    std::vector<std::string> typeAliasesStrs;

    if (IS_LOG_ENABLED) {
    }

    // resolve sub types
    if (astStruct.has(id.typeAliases)) {
        for (CellI& typeAliasKV : astStruct.typeAliases()) {
            CellI& alias        = typeAliasKV[id.key];
            CellI& type         = typeAliasKV[id.value];
            CellI& resolvedType = resolveType(type);
            ret.typeAliases(w.ast.kvPair(alias, resolvedType));
            if (IS_LOG_ENABLED) {
                typeAliasesStrs.push_back(fmt::format("    type {} = {};", alias.label(), getCompiledTypeFromResolvedType(resolvedType).label()));
            }
        }
    }

    // resolve memberOf list
    if (astStruct.has(id.memberOf)) {
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
        if (!typeAliasesStrs.empty() && (astStruct.has(id.methods) || astStruct.has(id.members))) {
            TRACE(compileStruct, "");
        }
    }

    // resolve members
    if (astStruct.has(id.members)) {
        for (CellI& memberKV : astStruct.members()) {
            CellI& member             = memberKV[id.value];
            CellI& memberId           = member[id.key];
            CellI& memberType         = member[id.type];
            CellI& resolvedMemberType = resolveType(memberType);
            ret.addMember(w.ast.member(memberId, resolvedMemberType));
            TRACE(compileStruct, "    {}: {};", memberId.label(), getCompiledTypeFromResolvedType(resolvedMemberType).label());
        }
    }

    // resolve methods
    if (astStruct.has(id.methods)) {
        if (IS_LOG_ENABLED) {
            if (astStruct.has(id.members)) {
                TRACE(compileStruct, "");
            }
        }
        for (CellI& origAstFunctionKV : astStruct.methods()) {
            auto& origAstFunction     = static_cast<Ast::Function&>(origAstFunctionKV[id.value]);
            auto& resolvedAstFunction = resolveTypesInFunction(origAstFunction, &ret);
            ret.addMethod(resolvedAstFunction, Ast::StructBase::ParameterModification::NoChange);
            TRACE(compileStruct, "    {};", shortFunctionName(resolvedAstFunction));
        }
    }

    TRACE(compileStruct, "}");
    TRACE(compileStruct, "");

    return ret;
}

Ast::Enum& Compiler::resolveTypesInEnum(Ast::Enum& astEnum)
{
    auto& enumName  = astEnum[id.name];
    Ast::Enum& ret  = *new Ast::Enum(w, enumName);
    bool isInstance = astEnum.has(id.instanceOf);

    if (isInstance) {
        ret.set(id.instanceOf, astEnum[id.instanceOf]);
        ret.set(id.templateParams, astEnum[id.templateParams]);
        ret.set(id.scope, astEnum[id.scope]);
    }

    auto& fullyQualifiedName = getFullyQualifiedName(astEnum);
    CellI* compiledStructPtr = nullptr;
    if (compiledStructs().hasKey(fullyQualifiedName)) {
        compiledStructPtr = &compiledStructs().getValue(fullyQualifiedName);
    } else if (isInstance && m_unknownInstances.hasKey(fullyQualifiedName)) {
        CellI& unknownInstance = m_unknownInstances.getValue(fullyQualifiedName);
        compiledStructPtr      = &unknownInstance[id.value];
    } else if (m_unknownStructs.hasKey(fullyQualifiedName)) {
        CellI& unknownStruct = m_unknownStructs.getValue(fullyQualifiedName);
        compiledStructPtr    = &unknownStruct[id.value];
        m_unknownStructs.remove(fullyQualifiedName);
        m_structs.add(fullyQualifiedName, *compiledStructPtr);
    } else {
        compiledStructPtr = new Object(w, std.Struct, fmt::format("{}", fullyQualifiedName.label()));
        compiledStructPtr->set(id.incomplete, w.true_);
        m_structs.add(fullyQualifiedName, *compiledStructPtr);
    }
    ret.set(id.compiledStruct, *compiledStructPtr);
    ret.set(id.fullyQualifiedName, fullyQualifiedName);

    m_currentStruct = &ret;

    TRACE(compileStruct, "enum {} {{", astEnum.label());

    // resolve values
    if (astEnum.has(id.values)) {
        for (CellI& kvPair : astEnum.values()) {
            CellI& valueCell = kvPair[id.value];
            CellI& valueName = valueCell[id.name];
            std::stringstream ss;
            if (valueCell.has(id.enumType)) {
                CellI& valueType                    = valueCell[id.enumType];
                CellI& resolvedValueType            = resolveType(valueType);
                Ast::TypedEnumValue& typedEnumValue = *new Ast::TypedEnumValue(w, valueName, resolvedValueType);
                if (valueCell.has(id.value)) {
                    typedEnumValue.set(id.value, valueCell[id.value]);
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
                if (valueCell.has(id.value)) {
                    ss << fmt::format(" = {}", resolveTypeInEnumValue(valueCell[id.value]).label());
                }
                ss << ",";
                TRACE(compileStruct, ss.str());
            }
        }
    }

    // resolve methods
    if (astEnum.has(id.methods)) {
        for (CellI& origAstFunctionKV : astEnum.methods()) {
            auto& origAstFunction     = static_cast<Ast::Function&>(origAstFunctionKV[id.value]);
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
    if (&ast.__type__() == &std.ast.ConstVar) {
        return ast[id.value];
    }

    panic("Unknown enum value!");
}

Ast::ResolvedType& Compiler::createResolvedType(CellI& astType, CellI& compiledType)
{
    return Ast::ResolvedType::New(w, astType, compiledType);
}

CellI& Compiler::getFullyQualifiedName(Ast::Base& base)
{
    if (base.has(id.fullyQualifiedName)) {
        return base[id.fullyQualifiedName];
    }
    bool isEmptyName                  = false;
    CellI* scopeFullyQualifiedNamePtr = nullptr;
    if (base.has(id.scope)) {
        Ast::Scope& scope          = static_cast<Ast::Scope&>(base[id.scope]);
        scopeFullyQualifiedNamePtr = &getFullyQualifiedName(scope);
    } else if (base.has(id.enum_)) {
        Ast::Enum& enum_           = static_cast<Ast::Enum&>(base[id.enum_]);
        scopeFullyQualifiedNamePtr = &getFullyQualifiedName(enum_);
    } else {
        static List& emptyName     = *new List(w, std.Char);
        scopeFullyQualifiedNamePtr = &emptyName;
        isEmptyName                = true;
    }
    CellI& scopeFullyQualifiedName = *scopeFullyQualifiedNamePtr;

    List& fullyQualifiedName = *new List(w, std.Char);
    for (CellI& character : scopeFullyQualifiedName) {
        fullyQualifiedName.add(character);
    }
    if (!fullyQualifiedName.empty()) {
        fullyQualifiedName.add(w.pools.chars.get(':'));
        fullyQualifiedName.add(w.pools.chars.get(':'));
    }
    if (!isEmptyName) {
        auto& name = base[id.name];
        for (CellI& character : name) {
            fullyQualifiedName.add(character);
        }
    }
    std::stringstream ss;
    int templateParamPrintModeFromCharIndex = fullyQualifiedName.size();
    if (base.has(id.instanceOf)) {
        int paramsLength                    = static_cast<List&>(base[id.templateParams]).size();
        templateParamPrintModeFromCharIndex = fullyQualifiedName.size() - paramsLength * 2;
    }
    int i = 0;
    for (CellI& character : fullyQualifiedName) {
        if (i++ == templateParamPrintModeFromCharIndex) {
            break;
        }
        ss << character.label();
    }
    if (base.has(id.instanceOf)) {
        ss << "<";
        int i = 0;
        for (CellI& slot : base[id.templateParams]) {
            CellI& key  = slot[id.key];
            CellI& type = slot[id.type];
            if (i++ != 0) {
                ss << ", ";
            }
            ss << fmt::format("{}={}", key.label(), getCompiledTypeFromResolvedType(type).label());
        }
        ss << ">";
    }
    fullyQualifiedName.label(ss.str());
    base.set(id.fullyQualifiedName, fullyQualifiedName);

    return fullyQualifiedName;
}

List& Compiler::generateFullyQualifiedIdFromTemplateId(Ast::Scope& scope, List& name, List& templateParams)
{
    List& idCell = *new List(w, std.Cell);
    std::stringstream ss;
    std::list<Ast::Scope*> scopes;
    scopes.push_front(&scope);

    Ast::Scope* childScopePtr = &scope;
    while (childScopePtr->has(id.scope)) {
        childScopePtr = &static_cast<Ast::Scope&>(childScopePtr->get(id.scope));
        if (childScopePtr->has(id.scope)) {
            scopes.push_front(childScopePtr);
        }
    }

    for (Ast::Scope* currentScopePtr : scopes) {
        Ast::Scope& currentScope = *currentScopePtr;
        CellI& scopeName         = currentScope[id.name];
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
        CellI& key          = slot[id.key];
        CellI& type         = slot[id.type];
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

List& Compiler::generateTemplateId(CellI& templateId, CellI& parameters, List& resolvedParams)
{
    List& idCell = *new List(w, std.Cell);
    std::stringstream ss;
    for (CellI& character : templateId) {
        idCell.add(character);
        ss << character.label();
    }

    ss << "<";
    int i = 0;
    for (CellI& param : parameters) {
        if (i++ != 0) {
            ss << ", ";
        }
        CellI& key              = param[id.key];
        CellI& type             = param[id.type];
        CellI& resolvedParamType = resolveType(type);
        CellI& compiledParamType = getCompiledTypeFromResolvedType(resolvedParamType);

        idCell.add(key);
        idCell.add(compiledParamType);
        resolvedParams.add(w.ast.parameterDeclaration(key, resolvedParamType));
        ss << fmt::format("{}={}", key.label(), compiledParamType.label());
    }
    ss << ">";
    idCell.label(ss.str());

    return idCell;
}

Ast::Base& Compiler::resolveType(CellI& typeAst)
{
    if (&typeAst.__type__() == &std.ast.ConstVar) {
        auto& cell      = static_cast<Ast::ConstVar&>(typeAst);
        auto& cellValue = typeAst[id.value];
        if (!cellValue.isA(std.Struct) && !cellValue.isA(std.Enum)) {
            panic("Type AST referencing a non-type!");
        }
        return cell;
    }
    if (&typeAst.__type__() == &std.ast.ResolvedType) {
        return static_cast<Ast::ResolvedType&>(typeAst);
    }
    if (&typeAst.__type__() == &std.ast.SelfType) {
        return w.ast._(*m_currentStruct);
    }
    if (&typeAst.__type__() == &std.ast.TypeName) {
        auto& resolveAstStruct   = findEnumOrStructByAstStructName(*m_scope, typeAst);
        auto& name               = resolveAstStruct[id.name];
        auto& fullyQualifiedName = getFullyQualifiedName(resolveAstStruct);

        if (compiledStructs().hasKey(fullyQualifiedName)) {
            return w.ast.const_(compiledStructs().getValue(fullyQualifiedName));
        }

        auto& resolveCompiledStruct = resolveStructName(name, fullyQualifiedName);
        auto& reslvedTypeNode       = createResolvedType(resolveAstStruct, resolveCompiledStruct);

        return reslvedTypeNode;
    }
    if (&typeAst.__type__() == &std.ast.TypeAlias) {
        auto& resolveAstStruct = static_cast<Ast::Struct&>(*m_currentStruct).getTypeAlias(typeAst[id.name]);
        return resolveAstStruct;
    }
    if (&typeAst.__type__() == &std.ast.TemplatedType) {
        return resolveTemplatedType(typeAst);
    }

    panic("Unknown ast type node!");
}

CellI& Compiler::getCompiledTypeFromResolvedType(CellI& resolvedTypeAst)
{
    if (resolvedTypeAst.isA(std.ast.ConstVar)) {
        return resolvedTypeAst[id.value];
    } else if (resolvedTypeAst.isA(std.ast.ResolvedType)) {
        return resolvedTypeAst[id.compiled];
    } else {
        panic("Unexpected AST type!");
    }
}

CellI& Compiler::getOrCreateStructReference(CellI& structId, TrieMap& unresolvedContainer, std::function<CellI&(CellI& structReference)> unknownCb)
{
    if (unresolvedContainer.hasKey(structId)) {
        return unresolvedContainer.getValue(structId)[id.value];
    } else {
        auto& structReference  = *new Object(w, std.StructReference);
        auto& unresolvedStruct = unknownCb(structReference);
        structReference.set(id.value, unresolvedStruct);
        structReference.set(id.id, structId);
        structReference.set(id.scope, *m_scope);
        structReference.set(id.resolvedScope, *m_resolvedScope);

        if (m_currentFn) {
            structReference.set(id.currentFn, *m_currentFn);
        }
        if (m_currentStruct) {
            structReference.set(id.currentStruct, *m_currentStruct);
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
            auto& unresolvedStruct = *new Object(w, std.Struct, fmt::format("{}", name.label()));
            unresolvedStruct.set(id.incomplete, w.true_);

            return unresolvedStruct;
        });
    }
}

CellI& Compiler::resolveTemplateInstanceId(CellI& name, CellI& fullyQualifiedName, CellI& idScope, CellI& ast, CellI& templateParams)
{
    auto& templateId = ast[id.id];
    return getOrCreateStructReference(fullyQualifiedName, m_unknownInstances, [this, &templateId, &name, &fullyQualifiedName, &templateParams, &ast, &idScope](CellI& structReference) -> CellI& {
        structReference.set(id.templateId, templateId);
        structReference.set(id.templateParams, templateParams);
        if (ast.has(id.scopes)) {
            structReference.set(id.idScope, idScope);
        }
        auto& unresolvedStruct = *new Object(w, std.Struct, fmt::format("{}", name.label()));
        unresolvedStruct.set(id.fullyQualifiedName, fullyQualifiedName);
        unresolvedStruct.set(id.incomplete, w.true_);

        return unresolvedStruct;
    });
}

Ast::Base& Compiler::resolveTemplatedType(CellI& ast)
{
    auto& templateId     = ast[id.id];
    auto& templateParams = static_cast<List&>(ast[id.parameters]);

    Ast::Scope* scopePtr = m_scope;
    if (ast.has(id.scopes)) {
        auto& scopeList          = ast[id.scopes];
        Ast::StructT& astStructT = findTemplateByNameInScopes(*scopePtr, scopeList, templateId);
        scopePtr                 = &static_cast<Ast::Scope&>(astStructT[id.scope]);
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

    List& resolvedTemplateParams   = *new List(w, std.Cell, "resolvedTemplateParams");
    List& structName               = generateTemplateId(templateId, templateParams, resolvedTemplateParams);
    List& fullyQualifiedStructName = generateFullyQualifiedIdFromTemplateId(scope, structName, resolvedTemplateParams);

    if (compiledStructs().hasKey(fullyQualifiedStructName)) {
        return w.ast.const_(compiledStructs().getValue(fullyQualifiedStructName));
    }
    CellI* unknownInstancePtr = nullptr;
    if (m_unknownInstances.hasKey(fullyQualifiedStructName)) {
        CellI& unknownInstance = m_unknownInstances.getValue(fullyQualifiedStructName)[id.value];
        if (unknownInstance.has(id.ast)) {
            return createResolvedType(unknownInstance[id.ast], unknownInstance);
        } else {
            unknownInstancePtr = &unknownInstance;
        }
    }

    auto& resolvedAstInstance = *new Ast::Struct(w, structName);
    resolvedAstInstance.set(id.fullyQualifiedName, fullyQualifiedStructName);
    resolvedAstInstance.set(id.scope, scope);
    resolvedAstInstance.set(id.instanceOf, scope.getItem<Ast::StructT>(templateId));
    resolvedAstInstance.set(id.templateParams, resolvedTemplateParams);
    if (unknownInstancePtr) {
        unknownInstancePtr->set(id.ast, resolvedAstInstance);
        return createResolvedType(resolvedAstInstance, *unknownInstancePtr);
    }
    auto& resolvedCompiledInstance = resolveTemplateInstanceId(structName, fullyQualifiedStructName, scope, ast, resolvedTemplateParams);
    resolvedCompiledInstance.set(id.ast, resolvedAstInstance);

    return createResolvedType(resolvedAstInstance, resolvedCompiledInstance);
}

Ast::Base& Compiler::findEnumOrStructByAstStructName(Ast::Scope& scope, CellI& astStructName)
{
    auto& name = astStructName[id.name];
    static List emptyList(w, std.Cell, "empty list");
    CellI* scopeListPtr = astStructName.has(id.scopes) ? &astStructName[id.scopes] : &emptyList;
    auto& scopes        = *scopeListPtr;

    if (Ast::Base* astStruct = findStructByNameInScopes(scope, scopes, name)) {
        return *astStruct;
    }
    if (auto* enum_ = findEnumByNameInScopes(scope, scopes, name)) {
        return *enum_;
    }

    panic("Unknown type name!");
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
        panic("Unknown template name!");
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
        if (currentScope->has(id.scope)) {
            currentScope = &static_cast<Ast::Scope&>(currentScope->get(id.scope));
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
        WARN(compileStruct, "unknown struct: {}", unknownStruct[id.value][id.value].label());
    }

    int instantiedNum      = 0;
    int unknownInstanceNum = 0;
    for (CellI& unknownInstanceSlot : m_unknownInstances) {
        CellI& unknownInstance  = unknownInstanceSlot[id.value];
        auto& unknownInstanceId = unknownInstance[id.id];

        if (IS_LOG_ENABLED) {
            TRACE(compileStruct, "unknown instance: {}", unknownInstanceId.label());
            if (unknownInstance.has(id.currentStruct)) {
                TRACE(compileStruct, "     from struct: {}", unknownInstance[id.currentStruct].label());
            }
            if (unknownInstance.has(id.currentFn)) {
                TRACE(compileStruct, "   from function: {}", unknownInstance[id.currentFn].label());
            }
        }

        std::stringstream ss;

        CellI& templateId     = unknownInstance[id.templateId];
        auto& templateParams  = static_cast<List&>(unknownInstance[id.templateParams]);
        auto& scope           = static_cast<Ast::Scope&>(unknownInstance[id.scope]);
        auto& idScope         = unknownInstance.has(id.idScope) ? static_cast<Ast::Scope&>(unknownInstance[id.idScope]) : scope;

        ss << fmt::format("        in scope: {}", getFullyQualifiedName(idScope).label());
        ss << fmt::format("  instantiate id: {}<", templateId.label());

        int paramCount = 0;
        for (CellI& param : templateParams) {
            CellI& paramId   = param[id.key];
            CellI& paramType = param[id.type];
            if (paramCount++ > 0) {
                ss << ", ";
            }
            ss << fmt::format("{}: {}", paramId.label(), getCompiledTypeFromResolvedType(paramType).label());
        }
        ss << ">";
        TRACE(compileStruct, ss.str());

        // we instantiate the template where we found it not where it belongs to
        // so std::List<Test> will not be instantiated in the std:: resolved scope
        auto& resolvedIdScope  = static_cast<Ast::Scope&>(unknownInstance[id.resolvedScope]);
        auto& compiledStruct   = static_cast<Ast::Struct&>(unknownInstance[id.value][id.ast]);
        m_scope                = &idScope;
        auto& structT          = idScope.getItem<Ast::StructT>(templateId);
        auto& instantiedStruct = instantiateStructT(structT, compiledStruct, templateParams);
        auto& resolvedStruct   = resolveTypesInStruct(instantiedStruct);
        resolvedIdScope.add<Ast::Struct>(resolvedStruct);
        instantiedNum = unknownInstanceNum++ + 1;
    }

    if (m_unknownStructs.size() > 0 || m_unknownInstances.size() != instantiedNum) {
        panic("Referencing an unknown type!");
    }
}

Ast::Struct& Compiler::instantiateStructT(Ast::StructT& structT, Ast::Struct& compiledStruct, List& inputParams)
{
    // process input parameters
    if (!inputParams.empty() && structT.missing(id.templateParams)) {
        panic("No template parameter was given!");
    }
    std::stringstream ss;
    Map inputParameters(w, std.Cell, std.Cell);

    for (CellI& slot : inputParams) {
        CellI& key  = slot[id.key];
        CellI& type = slot[id.type];
        inputParameters.add(key, type);
    }

    Ast::Struct& ret = compiledStruct;
    ret.set(id.instanceOf, structT);
    ret.set(id.templateParams, inputParams);
    ret.set(id.scope, static_cast<Ast::Scope&>(structT[id.scope]));

    // instantiate type aliases
    if (structT.has(id.typeAliases)) {
        Map& instantiatedTypeAliases = *new Map(w, std.Cell, std.ast.Base);
        for (CellI& typeAliasKV : structT.typeAliases()) {
            CellI& key               = typeAliasKV[id.key];
            CellI& type              = typeAliasKV[id.value];
            CellI& instantiatedParam = instantiateTemplateParamType(type, ret, inputParameters);
            instantiatedTypeAliases.add(key, instantiatedParam);
        }
        ret.set(id.typeAliases, instantiatedTypeAliases);
    }

    // instantiate methods
    if (structT.has(id.methods)) {
        for (CellI& astFunctionKV : structT.methods()) {
            auto& astFunction = static_cast<Ast::Function&>(astFunctionKV[id.value]);
            instantiateFunctionInStructT(astFunction, ret, inputParameters);
         }
    }

    // instantiate members
    if (structT.has(id.members)) {
        for (CellI& memberKV : structT.members()) {
            CellI& member                = memberKV[id.value];
            CellI& key                   = member[id.key];
            CellI& type                  = member[id.type];
            CellI& instantiatedParamType = instantiateTemplateParamType(type, ret, inputParameters);
            ret.addMember(w.ast.member(key, instantiatedParamType));
        }
    }

    // instantiate memberOf list
    if (structT.has(id.memberOf)) {
        List& instantiatedMemberOfs = *new List(w, std.Struct);
        for (CellI& membershipType : structT.memberOf()) {
            CellI& instantiatedParam = instantiateTemplateParamType(membershipType, ret, inputParameters);
            instantiatedMemberOfs.add(instantiatedParam);
        }
        ret.set(id.memberOf, instantiatedMemberOfs);
    }

    // trait implementations
    if (structT.has(id.traitImpls)) {
        for (CellI& traitImplKV : structT.traitImpls()) {
            CellI& traitImpl = traitImplKV[id.value];
            for (CellI& astFunctionKV : traitImpl[id.methods]) {
                CellI& astFunctionRef   = astFunctionKV[id.value];
                Map* associatedTypesPtr = nullptr;
                if (traitImpl.has(id.associatedTypes)) {
                    associatedTypesPtr = &static_cast<Map&>(traitImpl[id.associatedTypes]);
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
    Ast::Function& instantiedFunction = *new Ast::Function(w, astFunction[id.name]);
    compiledStruct.addMethod(instantiedFunction, Ast::StructBase::ParameterModification::NoChange);

    // parameters
    if (astFunction.has(id.parameters)) {
        Map& instantiatedParameters = *new Map(w, w.std.String, w.std.ast.Base);
        for (CellI& paramKV : astFunction.parameters()) {
            CellI& param                 = paramKV[id.value];
            CellI& key                   = param[id.key];
            CellI& type                  = param[id.type];
            CellI& instantiatedParamType = instantiateTemplateParamType(type, compiledStruct, inputParameters, associatedTypesPtr);
            instantiatedParameters.add(key, w.ast.parameterDeclaration(key, instantiatedParamType));
        }
        instantiedFunction.set(id.parameters, instantiatedParameters);
    }
    // return type
    if (astFunction.has(id.returnType)) {
        CellI& returnType             = astFunction[id.returnType];
        CellI& instantiatedReturnType = instantiateTemplateParamType(returnType, compiledStruct, inputParameters, associatedTypesPtr);
        instantiedFunction.set(id.returnType, instantiatedReturnType);
    }
    // instructions
    if (astFunction.has(id.instructions)) {
        instantiedFunction.set(id.instructions, instantiateAst(astFunction[id.instructions], compiledStruct, inputParameters, associatedTypesPtr));
    }
}

CellI& Compiler::instantiateTemplateParamType(CellI& param, CellI& selfType, Map& inputParameters, Map* associatedTypesPtr)
{
    if (&param.__type__() == &std.ast.TemplateParam) {
        CellI& paramValue = param[id.key];
        if (!inputParameters.hasKey(paramValue)) {
            panic("Instantiating with unknown template parameter!");
        }
        return inputParameters.getValue(paramValue);
    }
    if (&param.__type__() == &std.ast.TemplatedType) {
        List& resolvedParameterList = *new List(w, std.ast.Parameter);
        auto& ret                   = *new Ast::TemplatedType(w, param[id.id], resolvedParameterList);
        auto& parametersList        = param[id.parameters];

        for (CellI& slot : parametersList) {
            CellI& key              = slot[id.key];
            CellI& type             = slot[id.type];
            CellI& resolvedSlotType = instantiateTemplateParamType(type, selfType, inputParameters, associatedTypesPtr);
            resolvedParameterList.add(w.ast.parameterDeclaration(key, resolvedSlotType));
        }

        return ret;
    }
    if (&param.__type__() == &std.ast.AssociatedType && associatedTypesPtr) {
        Map& associatedTypes = *associatedTypesPtr;
        CellI& paramValue = param[id.key];
        if (!associatedTypes.hasKey(paramValue)) {
            panic("Instantiating with unknown associated type parameter!");
        }
        return instantiateTemplateParamType(associatedTypes.getValue(paramValue), selfType, inputParameters, associatedTypesPtr);
    }

    if (&param.__type__() == &std.ast.ConstVar || &param.__type__() == &std.ast.TypeName || &param.__type__() == &std.ast.TypeAlias || &param.__type__() == &std.ast.SelfType) {
        return param;
    }

    panic("Unknown template parameter!");
}

Ast::Base& Compiler::instantiateAst(CellI& ast, CellI& selfType, Map& inputParameters, Map* associatedTypesPtr)
{
    auto instantiate = [this, &selfType, &inputParameters, associatedTypesPtr](CellI& ast) -> Ast::Base& { return instantiateAst(ast, selfType, inputParameters, associatedTypesPtr); };

    // do nothing just traverse and copy the AST nodes
    if (&ast.__type__() == &std.ast.Block) {
        auto& instantiedAsts = *new List(w, std.ast.Base);
        for (CellI& ast : ast[id.asts]) {
            instantiedAsts.add(instantiate(ast));
        }
        return *new Ast::Block(w, instantiedAsts);
    } else if (&ast.__type__() == &std.ast.TypeName) {
        auto& ret = w.ast.typeName(ast[id.value]);
        if (ast.has(id.scopes)) {
            ret.set(id.scopes, ast[id.scopes]);
        }
        return ret;
    } else if (&ast.__type__() == &std.ast.Self) {
        return w.ast.self();
    } else if (&ast.__type__() == &std.ast.Member) {
        return w.ast.member(ast[id.key]);
    } else if (&ast.__type__() == &std.ast.Parameter) {
        return w.ast.parameter(ast[id.key]);
    } else if (&ast.__type__() == &std.ast.ConstVar) {
        return w.ast.const_(ast[id.value]);
    } else if (&ast.__type__() == &std.ast.Var) {
        return w.ast.var_(ast[id.name]);
    } else if (&ast.__type__() == &std.ast.Continue) {
        return w.ast.continue_();
    } else if (&ast.__type__() == &std.ast.Break) {
        return w.ast.break_();
    } else if (&ast.__type__() == &std.ast.Return) {
        if (ast.has(id.value)) {
            return w.ast.return_(static_cast<Ast::Base&>(instantiate(ast[id.value])));
        }
        return w.ast.return_();
    } else if (&ast.__type__() == &std.ast.If) {
        if (ast.has(id.else_)) {
            return w.ast.if_(instantiate(ast[id.condition]))
                .then_(instantiate(ast[id.then]))
                .else_(instantiate(ast[id.else_]));
        } else {
            return w.ast.if_(instantiate(ast[id.condition])).then_(instantiate(ast[id.then]));
        }
    } else if (&ast.__type__() == &std.ast.Match) {
        auto& ret = w.ast.match_(static_cast<Ast::Base&>(ast[id.enum_]));
        for (CellI& kvPair : ast[id.cases]) {
            auto& key = kvPair[id.key];
            auto& op  = instantiate(kvPair[id.value]);
            ret.case_(key, op);
        }
        return ret;
    } else if (&ast.__type__() == &std.ast.Do) {
        return w.ast.do_(instantiate(ast[id.statement])).while_(instantiate(ast[id.condition]));
    } else if (&ast.__type__() == &std.ast.While) {
        return w.ast.while_(instantiate(ast[id.condition])).do_(instantiate(ast[id.statement]));
    } else if (&ast.__type__() == &std.ast.For) {
        return w.ast.for_(instantiate(ast[id.variable])).in(instantiate(ast[id.container]))(instantiate(ast[id.statement]));
    } else if (&ast.__type__() == &std.ast.New) {
        auto* objectTypePtr = &ast[id.objectType];
        if (&(*objectTypePtr).__type__() == &std.ast.TemplatedType || &(*objectTypePtr).__type__() == &std.ast.AssociatedType) {
            CellI& resolvedObjectType = instantiateTemplateParamType(*objectTypePtr, selfType, inputParameters, associatedTypesPtr);
            objectTypePtr             = &resolvedObjectType;
        }
        auto& objectType = *static_cast<Ast::Base*>(objectTypePtr);
        if (ast.has(id.constructor)) {
            auto& constructor = ast[id.constructor];
            Ast::Base& ret    = w.ast.new_(objectType, static_cast<Ast::Base&>(constructor));
            if (ast.has(id.parameters)) {
                auto& newParameters = *new List(w, std.ast.Slot);
                for (CellI& slot : ast[id.parameters]) {
                    newParameters.add(w.ast.parameterInit(slot[id.key], instantiate(slot[id.value])));
                }
                ret.set(id.parameters, newParameters);
            }
            return ret;
        } else {
            return w.ast.new_(objectType);
        }
    } else if (&ast.__type__() == &std.ast.Call) {
        Ast::Base& ret = w.ast.call(ast[id.method]);
        if (ast.has(id.parameters)) {
            List& parameters = *new List(w, std.ast.Parameter, "parameters");
            for (auto& parameter : ast[id.parameters]) {
                parameters.add(w.ast.parameterInit(parameter[id.key], instantiate(parameter[id.value])));
            }
            ret.set(id.parameters, parameters);
        }
        return ret;
    }

    panic("Unknown AST to instantiate!");
}

void Compiler::compileInstructionsInScope(Ast::Scope& scope, Ast::Scope& resolvedScope)
{
    m_scope         = &scope;
    m_resolvedScope = &resolvedScope;

    if (scope.has(id.variables)) {
        for (CellI& kvPair : resolvedScope.items<Ast::Var>()) {
            Ast::Var& astVar = static_cast<Ast::Var&>(kvPair[id.value]);
            auto& varName    = getFullyQualifiedName(astVar);
            if (compiledVariables().hasKey(varName)) {
                return;
            }
            auto& compiledVariable = *new Object(w, std.op.Var, fmt::format("var {}", astVar.label()));
            compiledVariables().add(varName, compiledVariable);
        }
    }
    if (scope.has(id.structs)) {
        for (CellI& kvPair : resolvedScope.items<Ast::Struct>()) {
            Ast::Struct& astStruct = static_cast<Ast::Struct&>(kvPair[id.value]);
            compileInstructionsInStruct(astStruct);
        }
    }
    if (scope.has(id.enums)) {
        for (CellI& kvPair : resolvedScope.items<Ast::Enum>()) {
            Ast::Enum& astEnum = static_cast<Ast::Enum&>(kvPair[id.value]);
            compileInstructionsInEnum(astEnum);
        }
    }
    if (scope.has(id.functions)) {
        for (CellI& kvPair : resolvedScope.items<Ast::Function>()) {
            Ast::Function& astFunction = static_cast<Ast::Function&>(kvPair[id.value]);
            auto& compiledFunction     = compileInstructionsInFunction(astFunction);
            compiledFunctions().add(getFullyQualifiedName(astFunction), compiledFunction);
        }
    }
    if (scope.has(id.scopes)) {
        for (CellI& kvPair : scope.items<Ast::Scope>()) {
            Ast::Scope& nextScope = static_cast<Ast::Scope&>(kvPair[id.value]);
            if (nextScope.has(id.link)) {
                continue;
            }
            auto& nextResolvedScope = resolvedScope.getItem<Ast::Scope>(nextScope[id.name]);

            compileInstructionsInScope(nextScope, nextResolvedScope);

            m_scope         = &scope;
            m_resolvedScope = &resolvedScope;
        }
    }
}

void Compiler::compileInstructionsInStruct(Ast::Struct& astStruct)
{
    CellI& compiledStruct = astStruct[id.compiledStruct];
    if (compiledStruct.missing(id.incomplete)) {
        return;
    }
    compiledStruct.erase(id.incomplete);

    CellI& compiledStructName = getFullyQualifiedName(astStruct);

    // TODO: this is a hack now, we have to put it to stdlib
    if (&compiledStruct == &std.op.Call) {
        List& callSlotKeyList = *new List(w, std.ast.Slot);
        callSlotKeyList.add(id.__type__);
        callSlotKeyList.add(id.method);
        callSlotKeyList.add(id.parameters);
        compiledStruct.set(id.slotKeyList, callSlotKeyList);
    }

    // compile sub types
    if (astStruct.has(id.typeAliases)) {
        Map& compiledTypeAliases = *new Map(w, std.Cell, std.Struct, "typeAliases Map<ConstVar, Type>(...)");
        for (CellI& typeAliasKV : astStruct.typeAliases()) {
            CellI& key             = typeAliasKV[id.key];
            CellI& type            = typeAliasKV[id.value];
            auto& compiledSlotType = getCompiledTypeFromResolvedType(type);
            compiledTypeAliases.add(key, compiledSlotType);
        }
        compiledStruct.set(id.typeAliases, compiledTypeAliases);
        CellI& typeAliasesIndex = compiledTypeAliases[id.index];
        for (CellI& typeAlias : typeAliasesIndex.slotList()) {
            CellI& key      = typeAlias[id.key];
            CellI& value    = typeAliasesIndex[key][id.value];
            List& aliasName = *new List(w, std.Char);
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
    if (astStruct.has(id.members)) {
        Map& compiledMembers = *new Map(w, std.Cell, std.ast.Slot, "members Map<ConstVar, Slot>(...)");
        for (CellI& memberKV : astStruct.members()) {
            CellI& member          = memberKV[id.value];
            CellI& key             = member[id.key];
            CellI& type            = member[id.type];
            auto& compiledSlotType = getCompiledTypeFromResolvedType(type);
            compiledMembers.add(key, w.ast.slot(key, compiledSlotType));
        }
        compiledStruct.set(id.slots, compiledMembers);
    }

    // compile memberOf list
    if (astStruct.has(id.memberOf)) {
        Map& compiledMemberOfs = *new Map(w, std.Struct, std.Struct, "memberOf Map<Type, Type>(...)");
        for (CellI& membershipType : astStruct.memberOf()) {
            auto& compiledMembershipType = getCompiledTypeFromResolvedType(membershipType);
            compiledMemberOfs.add(compiledMembershipType, compiledMembershipType);
        }
        compiledStruct.set(id.memberOf, compiledMemberOfs);
    }

    // compile methods
    if (astStruct.has(id.methods)) {
        Map& compiledMethods = *new Map(w, std.Cell, std.ast.Function);
        for (CellI& methodKV : astStruct.methods()) {
            CellI& astFunction     = methodKV[id.value];
            auto& compiledFunction = compileInstructionsInFunction(static_cast<Ast::Function&>(astFunction));
            compiledMethods.add(astFunction[id.name], compiledFunction);
        }
        compiledStruct.set(id.methods, compiledMethods);
    }

    compiledStructs().add(compiledStructName, compiledStruct);
}

void Compiler::compileInstructionsInEnum(Ast::Enum& astEnum)
{
    CellI& compiledStruct = astEnum[id.compiledStruct];
    if (compiledStruct.missing(id.incomplete)) {
        return;
    }
    compiledStruct.erase(id.incomplete);
    compiledStruct.set(id.enum_, w.true_);

    // compile values
    if (astEnum.has(id.values)) {
        Map& compiledMembers = *new Map(w, std.Cell, std.ast.Slot, "members Map<ConstVar, Slot>(...)");
        compiledMembers.add(id.tag, w.ast.slot(id.tag, std.Cell));
        compiledStruct.set(id.slots, compiledMembers);
        for (CellI& kvPair : astEnum.values()) {
            CellI& valueKey  = kvPair[id.key];
            CellI& valueCell = kvPair[id.value];
            CellI& valueName = valueCell[id.name];
            if (&valueCell.__type__() == &std.ast.EnumValue) {
                auto& enumValue = static_cast<Ast::EnumValue&>(valueCell);
                auto& fullName  = getFullyQualifiedName(enumValue);
                CellI* compiledValuePtr = nullptr;
                if (m_earlyEnumValues.hasKey(fullName)) {
                    compiledValuePtr = &m_earlyEnumValues.getValue(fullName);
                } else {
                    compiledValuePtr = new Object(w, compiledStruct, fmt::format("{}::{}", astEnum.label(), enumValue.label()));
                }
                auto& compiledValue = *compiledValuePtr;
                compiledValue.set(id.tag, valueName);
                compiledVariables().add(fullName, compiledValue);

                if (valueCell.has(id.value)) {
                    auto& value         = enumValue[id.value];
                    auto& resolvedValue = resolveTypeInEnumValue(value);
                    auto& valueType     = resolvedValue.__type__();

                    compiledMembers.add(valueKey, w.ast.slot(valueKey, valueType));
                    compiledValue.set(valueName, value[id.value]);
                } else {
                    compiledMembers.add(valueKey, w.ast.slot(valueKey, compiledStruct));
                    compiledValue.set(valueName, id.emptyObject);
                }
            } else if (&valueCell.__type__() == &std.ast.TypedEnumValue) {
                auto& enumValue             = static_cast<Ast::TypedEnumValue&>(valueCell);
                auto& enumValueType         = valueCell[id.enumType];
                auto& compiledEnumValueType = getCompiledTypeFromResolvedType(enumValueType);
                auto& fullName              = getFullyQualifiedName(enumValue);
                compiledMembers.add(valueKey, w.ast.slot(valueKey, compiledEnumValueType));
            }
        }
    }

    // compile methods
    if (astEnum.has(id.methods)) {
        Map& compiledMethods = *new Map(w, std.Cell, std.ast.Function);
        for (CellI& methodKV : astEnum.methods()) {
            CellI& astFunction     = methodKV[id.value];
            auto& compiledFunction = compileInstructionsInFunction(static_cast<Ast::Function&>(astFunction));
            compiledMethods.add(astFunction[id.name], compiledFunction);
        }
        compiledStruct.set(id.methods, compiledMethods);
    }

    compiledStructs().add(getFullyQualifiedName(astEnum), compiledStruct);
}

CellI& Compiler::compileInstructionsInFunction(Ast::Function& astFunction)
{
    CellI* compiledFunctionPtr = nullptr;
    bool isPrimitiveTool       = astFunction.has(id.primitiveTool);
    if (isPrimitiveTool) {
        compiledFunctionPtr = &astFunction[id.primitiveTool];
        (*compiledFunctionPtr).set(id.primitiveTool, w.true_);
        List& slotKeyList = *new List(w, std.Cell, "slotKeyList");
        slotKeyList.add(id.__type__);
        for (auto& mappingKV : astFunction[id.memberMapping]) {
            slotKeyList.add(mappingKV[id.value]);
        }
        (*compiledFunctionPtr).set(id.slotKeyList, slotKeyList);
        std::cout << "";
    } else {
        compiledFunctionPtr = new Object(w, std.op.Function);
    }
    CellI& compiledFunction = *compiledFunctionPtr;
    compiledFunction.set(id.name, astFunction[id.name]);
    astFunction.set(id.compiledType, compiledFunction);

    compileFunctionParams(astFunction, compiledFunction);

    compiledFunction.set(id.ast, astFunction);

    if (!isPrimitiveTool && astFunction.has(id.instructions)) {
        compiledFunction.set(id.state, std.op.State.ready);
        compiledFunction.set(id.op, compileInstructionsInFunctionAst(astFunction, astFunction.instructions(), compiledFunction));
    }
    if (astFunction.has(id.static_)) {
        compiledFunction.set(id.static_, astFunction[id.static_]);
    }

    return compiledFunction;
}

void Compiler::compileFunctionParams(Ast::Function& astFunction, CellI& compiledFunction)
{
    std::stringstream iss;
    std::stringstream oss;
    std::string structTypeStr;
    if (astFunction.has(id.parameters)) {
        Map& parameters = *new Map(w, std.Cell, std.ast.Slot);

        if (astFunction.has(id.parameters)) {
            const auto _ = [this](auto& cell) -> Ast::ConstVar& { return w.ast._(cell); };
            int i        = 0;
            for (CellI& paramKV : astFunction.parameters()) {
                if (i++ > 1) {
                    iss << ", ";
                }
                auto& param        = paramKV[id.value];
                auto& key          = param[id.key];
                auto& type         = param[id.type];
                auto& compiledType = getCompiledTypeFromResolvedType(type);
                parameters.add(key, w.ast.slot(key, compiledType));
                if (&key == &id.self) {
                    if (i != 1) {
                        panic("The self parameter must be the first!");
                    }
                    structTypeStr = fmt::format("{}::", compiledType.label());
                } else {
                    iss << "p_" << key.label() << ": " << compiledType.label();
                }
            }
        }
        compiledFunction.set(id.parameters, parameters);
    }
    if (astFunction.has(id.returnType)) {
        auto& astReturnType      = astFunction.returnType();
        auto& compiledReturnType = getCompiledTypeFromResolvedType(astReturnType);
        oss << " -> " << compiledReturnType.label();
        compiledFunction.set(id.returnType, compiledReturnType);
    }
    if (compiledFunction.missing(id.primitiveTool)) {
        compiledFunction.label(fmt::format("fn {}{}({}){}", structTypeStr, astFunction[id.name].label(), iss.str(), oss.str()));
    }
}

std::string Compiler::shortFunctionName(Ast::Function& function)
{
    std::stringstream iss;
    std::stringstream oss;
    if (function.has(id.parameters)) {
        int i = 0;
        for (CellI& paramKV : function.parameters()) {
            CellI& key   = paramKV[id.key];
            if (&key == &id.self) {
                continue;
            }
            CellI& param = paramKV[id.value];
            if (i++ > 0) {
                iss << ", ";
            }
            iss << "p_" << param[id.key].label() << ": " << getCompiledTypeFromResolvedType(param[id.type]).label();
        }
    }
    if (function.has(id.returnType)) {
        oss << " -> " << getCompiledTypeFromResolvedType(function.returnType()).label();
    }
    return fmt::format("fn {}({}){}", function[id.name].label(), iss.str(), oss.str());
}

CellI& Compiler::compileInstructionsInFunctionAst(Ast::Function& astFunction, CellI& ast, CellI& function)
{
    auto compile = [this, &astFunction, &function](CellI& ast) -> CellI& { return compileInstructionsInFunctionAst(astFunction, ast, function); };
    const auto _ = [this](auto& cell) -> Ast::ConstVar& { return w.ast._(cell); };

    if (&ast.__type__() == &std.ast.Block) {
        CellI& list = ast[id.asts];
        if (&list[id.size] == &w._1_) {
            return compile(list[id.first][id.value]);
        }
        CellI* prevBlock = nullptr;
        if (m_lastBlock) {
            prevBlock = m_lastBlock;
        }
        CellI* firstOpBlockNode   = nullptr;
        CellI* currentOpBlockNode = nullptr;
        Object& opBlock           = *new Object(w, std.op.Block);
        m_lastBlock = &opBlock;
        for (CellI& ast : list) {
            CellI& newOpBlockNode = *new Object(w, std.op.Activate);
            newOpBlockNode.set(id.state, std.op.State.ready);
            newOpBlockNode.set(id.input, compile(ast));
            newOpBlockNode.set(id.parent, opBlock);

            if (!firstOpBlockNode) {
                firstOpBlockNode = &newOpBlockNode;
            } else {
                (*currentOpBlockNode).set(id.next, newOpBlockNode);
            }
            currentOpBlockNode = &newOpBlockNode;
        }
        opBlock.set(id.ast, ast);
        opBlock.set(id.state, std.op.State.ready);
        if (firstOpBlockNode) {
            opBlock.set(id.input, *firstOpBlockNode);
        }

        if (prevBlock) {
            m_lastBlock = prevBlock;
        } else {
            m_lastBlock = nullptr;
        }

        return opBlock;
    } else if (&ast.__type__() == &std.ast.ResolvedType) {
        Object& constVar = *new Object(w, std.op.ConstVar);
        constVar.set(id.ast, ast);
        constVar.set(id.state, std.op.State.ready);
        constVar.set(id.value, ast[id.compiled]);
        return constVar;
    } else if (&ast.__type__() == &std.ast.Self) {
        CellI& retOp = compile(w.ast.get(_(function), _(id.stack)) / _(id.value) / _(id.input) / _(id.self));
        retOp.set(id.ast, ast);
        retOp.label("self");
        return retOp;
    } else if (&ast.__type__() == &std.ast.Member) {
        CellI& member = compile(w.ast.get(w.ast.self(), w.ast.const_(ast[id.key])));
        member.set(id.ast, ast);
        return member;
    } else if (&ast.__type__() == &std.ast.Parameter) {
        CellI& retOp = compile(w.ast.get(_(function), _(id.stack)) / _(id.value) / _(id.input) / _(ast[id.key]));
        retOp.set(id.ast, ast);
        return retOp;
    } else if (&ast.__type__() == &std.ast.ConstVar) {
        Object& constVar = *new Object(w, std.op.ConstVar);
        constVar.set(id.ast, ast);
        constVar.set(id.state, std.op.State.ready);
        constVar.set(id.value, ast[id.value]);
        return constVar;
    } else if (&ast.__type__() == &std.ast.Var) {
        Index* localVarsIndexPtr = nullptr;
        if (function.missing(id.localVars)) {
            localVarsIndexPtr = new Index(w, fmt::format("LocalVarsIndex of {}", function.label()));
            function.set(id.localVars, *localVarsIndexPtr);
        } else {
            localVarsIndexPtr = &static_cast<Index&>(function[id.localVars]);
        }

        Index& localVarsIndex = *localVarsIndexPtr;
        if (!localVarsIndex.has(ast[id.name])) {
            localVarsIndex.insert(ast[id.name], w.ast.slot(ast[id.name], std.op.Var));
        }

        CellI& retOp = compile(w.ast.get(_(function), _(id.stack)) / _(id.value) / _(id.localVars) / _(ast[id.name]));
        retOp.set(id.ast, ast);
        return retOp;
    } else if (&ast.__type__() == &std.ast.Continue) {
        if (!m_lastBlock) {
            panic("No statement to break!");
        }
        CellI& lastBlock = *m_lastBlock;
        CellI& retOp     = compile(w.ast.set(_(lastBlock), _(id.status), _(id.continue_)));
        retOp.label("__continue__");
        retOp.set(id.state, std.op.State.ready);
        return retOp;
    } else if (&ast.__type__() == &std.ast.Break) {
        if (!m_lastBlock) {
            panic("No statement to break!");
        }
        CellI& lastBlock = *m_lastBlock;
        CellI& retOp     = compile(w.ast.set(_(lastBlock), _(id.status), _(id.break_)));
        retOp.label("__break__");
        retOp.set(id.state, std.op.State.ready);
        return retOp;
    } else if (&ast.__type__() == &std.ast.Return) {
        Object& retOp = *new Object(w, std.op.Return, "op.return");
        retOp.set(id.ast, ast);
        retOp.set(id.state, std.op.State.ready);
        if (ast.has(id.value)) {
            retOp.set(id.result, compile(w.ast.set(_(function), _(id.value), static_cast<Ast::Base&>(ast[id.value]))));
        }
        return retOp;
    } else if (&ast.__type__() == &std.ast.If) {
        Object& retOp = *new Object(w, std.op.If);
        retOp.set(id.ast, ast);
        retOp.set(id.state, std.op.State.ready);
        retOp.set(id.condition, compile(ast[id.condition]));
        retOp.set(id.then, compile(ast[id.then]));
        if (ast.has(id.else_)) {
            retOp.set(id.else_, compile(ast[id.else_]));
            return retOp;
        } else {
            return retOp;
        }
    } else if (&ast.__type__() == &std.ast.Match) {
        auto& enumObj        = static_cast<Ast::Base&>(ast[id.enum_]);
        auto& caseList       = ast[id.cases];
        auto& astCases       = *new List(w, std.ast.Base);
        Ast::Block& astBlock = *new Ast::Block(w, astCases);

        for (CellI& kvPair : caseList) {
            auto& kind    = kvPair[id.key];
            auto& op      = kvPair[id.value];
            auto& oneCase = w.ast.if_(w.ast.same(w.ast.get(enumObj, "tag"), _(kind))).then_(static_cast<Ast::Base&>(op));
            astCases.add(oneCase);
        }
        auto& retOp = compile(astBlock);
        return retOp;
    } else if (&ast.__type__() == &std.ast.Do) {
        Object& retOp = *new Object(w, std.op.Do);
        retOp.set(id.ast, ast);
        retOp.set(id.state, std.op.State.ready);
        retOp.set(id.statement, compile(ast[id.statement]));
        retOp.set(id.condition, compile(ast[id.condition]));
        return retOp;
    } else if (&ast.__type__() == &std.ast.While) {
        Object& retOp = *new Object(w, std.op.While);
        retOp.set(id.ast, ast);
        retOp.set(id.state, std.op.State.ready);
        retOp.set(id.condition, compile(ast[id.condition]));
        retOp.set(id.statement, compile(ast[id.statement]));
        return retOp;
    } else if (&ast.__type__() == &std.ast.For) {
        auto& valueVar  = static_cast<Ast::Var&>(ast[id.variable]);
        auto& statement = static_cast<Ast::Base&>(ast[id.statement]);

        Ast::Block& forLoopBlock =
            w.ast.block(
                valueVar = w.ast.call(*w.ast.var_("iterator"), "getCurrentNodeValue"));

        Ast::Base& forLoopBlockEnd =
            w.ast.if_(w.ast.same(w.ast.call(*w.ast.var_("iterator"), "hasNextNode"), _(w.true_)))
                            .then_(w.ast.call(*w.ast.var_("iterator"), "goToNextNode"))
                            .else_(w.ast.break_());

        forLoopBlock.mergeFrom(statement);
        forLoopBlock.mergeFrom(forLoopBlockEnd);

        Ast::Block& retAst = w.ast.block(
            w.ast.var_("iterator") = w.ast.call(ast[id.container], "iterator"),
            w.ast.if_(w.ast.same(w.ast.call(*w.ast.var_("iterator"), "isContainerEmpty"), _(w.false_)))
                .then_(w.ast.block(
                    w.ast.call(*w.ast.var_("iterator"), "goToFirstNode"),
                    w.ast.do_(forLoopBlock)
                        .while_(_(w.true_))))
        );
        CellI& retOp = compile(retAst);
        return retOp;
    } else if (&ast.__type__() == &std.ast.New) {
        CellI* firstOpBlockNode = nullptr;
        auto& astObjectType     = ast[id.objectType];

        Object& block = *new Object(w, std.op.Block);
        block.set(id.ast, ast);
        block.set(id.state, std.op.State.ready);

        Object& opSet = *new Object(w, std.op.Set, "New { block.value = new objectType(); }");
        opSet.set(id.ast, ast);
        opSet.set(id.state, std.op.State.ready);
        opSet.set(id.cell, compile(w.ast.const_(block)));
        opSet.set(id.key, compile(w.ast.const_(id.value)));

        Object& opNew = *new Object(w, std.op.New);
        opNew.set(id.ast, ast);
        opNew.set(id.state, std.op.State.ready);
        opNew.set(id.objectType, compile(astObjectType));
        opSet.set(id.value, opNew);

        CellI& newOpBlockNode = *new Object(w, std.op.Activate);
        newOpBlockNode.set(id.state, std.op.State.ready);
        newOpBlockNode.set(id.input, opSet);
        newOpBlockNode.set(id.parent, block);
        firstOpBlockNode = &newOpBlockNode;

        if (ast.has(id.constructor)) {
            Object& callAst = *new Object(w, std.ast.Call);
            List& parameters = w.list(w.ast.parameterInit(id.self, w.ast.get(w.ast.const_(block), w.ast.const_(id.value))));
            callAst.set(id.parameters, parameters);
            callAst.set(id.method, ast[id.constructor]);
            if (ast.has(id.parameters)) {
                for (auto& parameter : ast[id.parameters]) {
                    parameters.add(parameter);
                }
            }
            CellI& callConstructor = compile(callAst);
            callConstructor.label("New { call constructor; }");
            CellI& newOpBlockNode = *new Object(w, std.op.Activate);
            newOpBlockNode.set(id.state, std.op.State.ready);
            newOpBlockNode.set(id.input, callConstructor);
            newOpBlockNode.set(id.parent, block);
            (*firstOpBlockNode).set(id.next, newOpBlockNode);
        }
        block.set(id.input, *firstOpBlockNode);
        return block;
    } else if ((&ast.__type__() == &std.ast.Call) && (&ast[id.method].__type__() == &std.ast.PrimitiveToolName)) {
        CellI& primitiveTool = ast[id.method][id.name];
        Object& retOp        = *new Object(w, primitiveTool);
        retOp.set(id.ast, ast);
        retOp.set(id.state, std.op.State.ready);

        Map& membersMapping = static_cast<Map&>(primitiveTool[id.ast][id.memberMapping]);

        if (ast.has(id.parameters)) {
            for (CellI& slot : ast[id.parameters]) {
                CellI& key   = slot[id.key];
                CellI& value = slot[id.value];
                retOp.set(membersMapping.getValue(key), compile(value));
            }
        }

        return retOp;
    } else if (&ast.__type__() == &std.ast.Call) {
        CellI& selfType   = ast[id.parameters][id.first][id.value];
        CellI& astMethod  = ast[id.method];
        auto& astMethodId = astMethod[id.value];
#if 1
        bool checked = false;
        // New<T>.constructor(...)
        std::stringstream ss;
#if 0
        if (&astCell.__type__() == &std.ast.Get && &astCell[id.cell].__type__() == &std.ast.Cell && &astCell[id.cell][id.value].__type__() == &std.op.Block) {
            auto& opBlock    = astCell[id.cell][id.value];
            auto& opBlockAst = opBlock[id.ast];
            if (&opBlockAst.__type__() == &std.ast.New) {
                auto& astObjectTypeStruct = opBlockAst[id.objectType];
                checkMethodCall(astObjectTypeStruct, astMethodId);
                checked = true;
            }
            // var_xxx.call(...)
        } else if (&astCell.__type__() == &std.ast.Get && &astCell[id.cell].__type__() == &std.ast.Var) {
            auto& astVar = astCell[id.cell];
            ss << "In " << astVar.label() << " ";
            // m_xxx.method(...)
        } else
#endif
        if (&selfType.__type__() == &std.ast.Member) {
            auto& astMemberId = selfType[id.key];
            if (&astMemberId == &id.__type__) {
                // std::cout << "DDDD " << astMembersType.label();
                auto& stdScope = w.globalScope.getItem<Ast::Scope>("std");
                auto& type     = stdScope.getItem<Ast::Struct>("Struct");
                checkMethodCall(type, astMethodId);
                checked = true;
            } else {
                auto& astMembers = static_cast<Map&>(selfType[id.members]);
                if (astMembers.hasKey(astMemberId)) {
                    auto& slot = static_cast<Ast::Slot&>(astMembers.getValue(astMemberId));
                    auto& type = slot[id.type];
                    checkMethodCall(type, astMethodId);
                    checked = true;
                } else {
                    panic("Unknown member name!");
                }
            }
        } else if (&selfType.__type__() == &std.ast.Self) {
            auto& paramsMap   = static_cast<Map&>(function[id.parameters]);
            auto& astSelfType = paramsMap.getValue(id.self)[id.type];
            checkMethodCall(astSelfType, astMethodId);
            checked = true;
        } else if (&selfType.__type__() == &std.ast.Parameter && selfType.missing(id.value)) {
            // if the value is missing we referencing a parameter from the parent function
            // if the value is filled, we just pass the value, so nothing to check
            auto& parameterName                 = selfType[id.key];
            auto& astFunctionParameters         = function[id.ast][id.parameters];
            Ast::Slot* astFunctionParameterSlot = nullptr;
            for (CellI& slot : astFunctionParameters) {
                CellI& key  = slot[id.key];
                CellI& type = slot[id.type];
                if (&key == &parameterName) {
                    astFunctionParameterSlot = &static_cast<Ast::Slot&>(slot);
                    break;
                }
            }
            if (astFunctionParameterSlot) {
                Ast::Slot& slot        = *astFunctionParameterSlot;
                auto& astParameterType = slot[id.type];
                checkMethodCall(astParameterType, astMethodId);
                checked = true;
            } else {
                panic("Unknown parameter name!");
            }
        }
        if (!checked) {
            WARN(compileStruct, "{} Unchecked method call {} in {}", ss.str(), astMethodId.label(), function.label());
        }
#endif
        Object& retOp = *new Object(w, std.op.Call);
        retOp.set(id.ast, ast);
        retOp.set(id.state, std.op.State.ready);
        retOp.set(id.method, compile(ast[id.method]));
        retOp.set(id.parentFunction, function);
        if (ast.has(id.parameters)) {
            Map& parameters = *new Map(w, std.Cell, std.Cell);
            for (CellI& param : ast[id.parameters]) {
                CellI& key   = param[id.key];
                CellI& value = param[id.value];
                parameters.add(key, compile(value));
            }
            retOp.set(id.parameters, parameters);
        }

        return retOp;
    }

    panic("Unknown function AST!");
}

void Compiler::checkMethodCall(CellI& astType, CellI& astMethodId)
{
    CellI* typePtr  = nullptr;
    Map* methodsPtr = nullptr;
    if (astType.isA(std.ast.ConstVar)) {
        auto& cellValue = astType[id.value];
        typePtr         = &cellValue;
        methodsPtr      = &static_cast<Map&>(cellValue[id.methods]);
    } else if (astType.isA(std.ast.Struct)) {
        typePtr    = &astType;
        methodsPtr = &static_cast<Map&>(astType[id.methods]);
    } else if (astType.isA(std.ast.ResolvedType)) {
        typePtr    = &astType[id.compiled];
        methodsPtr = &static_cast<Map&>(astType[id.ast][id.methods]);
    } else {
        panic("Unexpected AST type");
    }
    CellI& type = *typePtr;
    if (&type.__type__() != &std.ast.Struct && &type.__type__() != &std.Struct && type.label() != "Struct") {
        panic("Resolved type must be a type!");
    }
    auto& methods = *methodsPtr;
    if (!methods.hasKey(astMethodId)) {
        std::cerr << fmt::format("Method '{}' doesn't exist in type {}", astMethodId.label(), type.label()) << std::endl;
        panic("Method doesn't exist in type!");
    }
}

void Compiler::compileDescriptionInScope(Ast::Scope& scope, Ast::Scope& resolvedScope)
{
    if (scope.has(id.functions)) {
        for (CellI& kvPair : resolvedScope.items<Ast::Function>()) {
            Ast::Function& astFunction = static_cast<Ast::Function&>(kvPair[id.value]);
            compileDescriptionInFunction(astFunction);
        }
    }
    if (scope.has(id.structs)) {
        for (CellI& kvPair : resolvedScope.items<Ast::Struct>()) {
            Ast::Struct& astStruct = static_cast<Ast::Struct&>(kvPair[id.value]);
            if (astStruct.has(id.methods)) {
                Map& compiledMethods = *new Map(w, std.Cell, std.ast.Function);
                for (CellI& astFunctionKV : astStruct.methods()) {
                    compileDescriptionInFunction(static_cast<Ast::Function&>(astFunctionKV[id.value]), &astStruct);
                }
            }
        }
    }
    if (scope.has(id.enums)) {
        for (CellI& kvPair : resolvedScope.items<Ast::Enum>()) {
            Ast::Enum& astEnum = static_cast<Ast::Enum&>(kvPair[id.value]);
            if (astEnum.has(id.methods)) {
                Map& compiledMethods = *new Map(w, std.Cell, std.ast.Function);
                for (CellI& astFunctionKV : astEnum.methods()) {
                    compileDescriptionInFunction(static_cast<Ast::Function&>(astFunctionKV[id.value]), &astEnum);
                }
            }
        }
    }
    if (scope.has(id.scopes)) {
        for (CellI& kvPair : scope.items<Ast::Scope>()) {
            Ast::Scope& nextScope = static_cast<Ast::Scope&>(kvPair[id.value]);
            if (nextScope.has(id.link)) {
                continue;
            }
            auto& nextResolvedScope = resolvedScope.getItem<Ast::Scope>(nextScope[id.name]);

            compileDescriptionInScope(nextScope, nextResolvedScope);

            m_scope         = &scope;
            m_resolvedScope = &resolvedScope;
        }
    }
}

void Compiler::compileDescriptionInFunction(Ast::Function& astFunction, Ast::StructBase* astStructPtr)
{
    if (astFunction.missing(id.compiledType)) {
        return;
    }
    Object& compiledFunction = static_cast<Object&>(astFunction[id.compiledType]);

    if (astFunction.has(id.description)) {
        compiledFunction.set(id.description, compileDescriptionInFunctionAst(astFunction.description(), astFunction, astStructPtr));
        m_toolFinder.add(compiledFunction);
    }
}

Ast::Base& Compiler::resolveDescriptionTypesInFunctionCode(CellI& ast, Ast::Function& astFunction, Ast::StructBase* astStructPtr)
{
    auto resolve = [this, &astFunction, &astStructPtr](CellI& ast) -> Ast::Base& { return resolveDescriptionTypesInFunctionCode(ast, astFunction, astStructPtr); };

    if (&ast.__type__() == &std.ast.Block) {
        // do nothing just traverse and copy the AST nodes
        auto& instantiedAsts = *new List(w, std.ast.Base);
        for (CellI& ast : ast[id.asts]) {
            instantiedAsts.add(resolve(ast));
        }
        return *new Ast::Block(w, instantiedAsts);
    } else if (&ast.__type__() == &std.ast.TypeName) {
        return resolveType(ast);
    } else if (&ast.__type__() == &std.ast.Self) {
        if (!astStructPtr) {
            panic("Referencing the self pointer, but this function doesn't associated with a struct/enum!");
        }
        auto& ret = w.ast.self();
        ret.set(id.type, (*astStructPtr)[id.compiledStruct]);
        return ret;
    } else if (&ast.__type__() == &std.ast.Member) {
        if (!astStructPtr) {
            panic("Referencing a member, but this function doesn't associated with a struct/enum!");
        }
        Ast::StructBase& astStruct = *astStructPtr;
        CellI& memberKey           = ast[id.key];
        if (&memberKey == &id.__type__) {
            CellI& compiledStruct = astStruct[id.compiledStruct];
            if (&compiledStruct == &std.Index) {
                return w.ast.member(ast[id.key]);
            } else {
                return w.ast._(compiledStruct);
            }
        }
        auto& member = static_cast<Ast::Member&>(astStruct.members().getValue(memberKey));
        return member;
    } else if (&ast.__type__() == &std.ast.Parameter) {
        Map& parameters = static_cast<Map&>(astFunction[id.parameters]);
        auto& key       = ast[id.key];
        auto& ret       = w.ast.parameter(key);
        auto& parameter = parameters.getValue(key);
        auto& type      = getCompiledTypeFromResolvedType(parameter[id.type]);
        ret.set(id.type, type);
        return ret;
    } else if (&ast.__type__() == &std.ast.ConstVar) {
        auto& ret = w.ast.const_(ast[id.value]);
        ret.set(id.type, ast[id.value].__type__());
        return ret;
    } else if (&ast.__type__() == &std.ast.UnknownVar) {
        auto& ret = w.ast.unknown_(ast[id.value]);
        ret.set(id.type, ast[id.value].__type__());
        return ret;
    } else if (&ast.__type__() == &std.ast.Return) {
        if (ast.has(id.value)) {
            return w.ast.return_(resolve(ast[id.value]));
        }
        return w.ast.return_();
    } else if (&ast.__type__() == &std.ast.Call) {
        Ast::Base* retPtr = nullptr;
#if 0
        Ast::StructBase& astStruct = *astStructPtr;
        CellI& compiledStruct      = astStruct[id.compiledStruct];
        if (&compiledStruct == &std.Index) {
        } else {
            CellI& resolvedSelf = resolve(ast[id.self]);
            // CellI& selfType     = resolvedSelf[id.type];
        }
#endif
        auto& ret = w.ast.call(ast[id.method]);
        if (ast.has(id.parameters)) {
            auto& newParameters = *new List(w, std.ast.Parameter);
            for (CellI& parameter : ast[id.parameters]) {
                newParameters.add(w.ast.parameterInit(parameter[id.key], resolve(parameter[id.value])));
            }
            ret.set(id.parameters, newParameters);
        }
        return ret;
    }

    panic("Unknown AST to instantiate!");
}

CellI& Compiler::compileDescriptionInFunctionAst(CellI& ast, Ast::Function& astFunction, Ast::StructBase* astStructPtr)
{
    auto compile = [this, &astFunction, &astStructPtr](CellI& ast) -> CellI& { return compileDescriptionInFunctionAst(ast, astFunction, astStructPtr); };

    if (&ast.__type__() == &std.ast.Block) {
        CellI& list = ast[id.asts];
        if (&list[id.size] == &w._1_) {
            return compile(list[id.first][id.value]);
        }
        List& retList = *new List(w, std.Cell, "description");
        for (CellI& ast : list) {
            retList.add(compile(ast));
        }

        return retList;
    } else if (&ast.__type__() == &std.ast.ConstVar) {
        Object& constVar = *new Object(w, std.op.ConstVar);
        constVar.set(id.ast, ast);
        constVar.set(id.type, ast[id.type]);
        constVar.set(id.value, ast[id.value]);
        return constVar;
    } else if (&ast.__type__() == &std.ast.UnknownVar) {
        Object& unknownVar = *new Object(w, std.op.UnknownVar);
        unknownVar.set(id.ast, ast);
        unknownVar.set(id.value, ast[id.value]);
        unknownVar.set(id.type, ast[id.type]);
        return unknownVar;
    } else if (&ast.__type__() == &std.ast.ResolvedType) {
        Object& constVar = *new Object(w, std.op.ConstVar);
        constVar.set(id.ast, ast);
        constVar.set(id.value, ast[id.compiled]);
        return constVar;
    } else if (&ast.__type__() == &std.ast.Self) {
        return ast;
    } else if (&ast.__type__() == &std.ast.Parameter) {
        return ast;
    } else if (&ast.__type__() == &std.ast.Return) {
        return ast;
    } else if ((&ast.__type__() == &std.ast.Call) && (&ast[id.method].__type__() == &std.ast.PrimitiveToolName)) {
        CellI& primitiveTool = ast[id.method][id.name];
        Object& retOp        = *new Object(w, primitiveTool);
        retOp.set(id.ast, ast);

        Map& membersMapping = static_cast<Map&>(primitiveTool[id.ast][id.memberMapping]);

        if (ast.has(id.parameters)) {
            for (CellI& slot : ast[id.parameters]) {
                CellI& key   = slot[id.key];
                CellI& value = slot[id.value];
                retOp.set(membersMapping.getValue(key), compile(value));
            }
        }

        return retOp;
    }

    panic("Unknown function AST!");
}


CellI& Compiler::compilePromptInFunctionAst(CellI& ast, Ast::Function& astFunction)
{
    auto compile = [this, &astFunction](CellI& ast) -> CellI& { return compileDescriptionInFunctionAst(ast, astFunction); };

    if (&ast.__type__() == &std.ast.Block) {
        CellI& list = ast[id.asts];
        if (&list[id.size] == &w._1_) {
            return compile(list[id.first][id.value]);
        }
        List& retList = *new List(w, std.Cell, "description");
        for (CellI& ast : list) {
            retList.add(compile(ast));
        }

        return retList;
    } else if (&ast.__type__() == &std.ast.ConstVar) {
        Object& constVar = *new Object(w, std.op.ConstVar);
        constVar.set(id.ast, ast);
        constVar.set(id.state, std.op.State.ready);
        constVar.set(id.type, ast[id.type]);
        constVar.set(id.value, ast[id.value]);
        return constVar;
    } else if (&ast.__type__() == &std.ast.UnknownVar) {
        Object& unknownVar = *new Object(w, std.op.UnknownVar);
        unknownVar.set(id.ast, ast);
        unknownVar.set(id.state, std.op.State.missingInput);
        unknownVar.set(id.value, ast[id.value]);
        unknownVar.set(id.type, ast[id.type]);
        return unknownVar;
    } else if (&ast.__type__() == &std.ast.ResolvedType) {
        Object& constVar = *new Object(w, std.op.ConstVar);
        constVar.set(id.ast, ast);
        constVar.set(id.value, ast[id.compiled]);
        return constVar;
    } else if ((&ast.__type__() == &std.ast.Call) && (&ast[id.method].__type__() == &std.ast.PrimitiveToolName)) {
        CellI& primitiveTool = ast[id.method][id.name];
        Object& retOp        = *new Object(w, primitiveTool);
        retOp.set(id.ast, ast);

        Map& membersMapping = static_cast<Map&>(primitiveTool[id.ast][id.memberMapping]);

        if (ast.has(id.parameters)) {
            bool hasMissingInput = false;
            bool isConstructor   = ast.has("isConstructor");
            for (CellI& slot : ast[id.parameters]) {
                CellI& key   = slot[id.key];
                CellI& value = slot[id.value];
                CellI& compiledValue = compile(value);
                retOp.set(membersMapping.getValue(key), compiledValue);
                if (isConstructor) {
                    isConstructor = false;
                    continue;
                }
                if (&compiledValue[id.state] == &std.op.State.missingInput) {
                    hasMissingInput = true;
                }
            }
            CellI& calculatedState = hasMissingInput ? std.op.State.missingInput : std.op.State.ready;
            retOp.set(id.state, calculatedState);
        }

        return retOp;
    }

    panic("Unknown function AST!");
}

} // namespace cells
} // namespace infocell