#pragma once

#include "Library.h"
#include "World.h"
#include "ToolFinder.h"

namespace infocell {
namespace cells {

class Compiler
{
    Compiler(const Compiler&) = delete;

public:
    Compiler(World& w);

    void compile(Library& library);
    Library& compile(Ast::Scope& scope);

    CellI& reigisterStructBeforeCompilation(CellI& id);
    void registerBuiltInStruct(const std::string& fullName, CellI& compiledStruct, Ast::Scope* parentScope = nullptr);
    void registerBuiltInEnumValue(const std::string& fullName, CellI& compiledEnumValue);
    ToolFinder& getToolFinder();

protected:
    Library& library();
    TrieMap& compiledFunctions();
    TrieMap& compiledStructs();
    TrieMap& compiledVariables();

    void registerEarlyStructs();
    void resolveEarlyStructsInScope(Ast::Scope& scope, Ast::Scope& resolvedScope);

    Ast::Scope& resolveTypesInScope(Ast::Scope& scope);
    Ast::Function& resolveTypesInFunction(Ast::Function& function);
    Ast::Base& resolveTypesInFunctionCode(CellI& ast);
    Ast::Struct& resolveTypesInStruct(Ast::Struct& astStruct);
    Ast::Enum& resolveTypesInEnum(Ast::Enum& enum_);
    CellI& resolveTypeInEnumValue(CellI& ast);

    Ast::ResolvedType& createResolvedType(CellI& astType, CellI& compiledType);
    CellI& getFullyQualifiedName(Ast::Base& base);
    List& generateTemplateId(CellI &id, CellI& parameters, List& resolvedParams);
    List& generateFullyQualifiedIdFromTemplateId(Ast::Scope& scope, List& name, List& templateParams);

    Ast::Base& resolveType(CellI& typeAst);
    CellI& getCompiledTypeFromResolvedType(CellI& ast);
    CellI& getOrCreateStructReference(CellI& structId, TrieMap& unresolvedContainer, std::function<CellI&(CellI& structReference)> unknownCb);
    CellI& resolveStructName(CellI& name, CellI& fullyQualifiedName);
    CellI& resolveTemplateInstanceId(CellI& name, CellI& fullyQualifiedName, CellI& idScope, CellI& ast, CellI& templateParams);
    Ast::Base& resolveTemplatedType(CellI& ast);

    Ast::Base& findEnumOrStructByAstStructName(Ast::Scope& scope, CellI& astStructName);
    Ast::Enum* findEnumByNameInScopes(Ast::Scope& scope, CellI& scopeList, CellI& name);
    Ast::Struct* findStructByNameInScopes(Ast::Scope& scope, CellI& scopeList, CellI& name);
    Ast::StructT& findTemplateByNameInScopes(Ast::Scope& scope, CellI& scopeList, CellI& name);
    Ast::Base* findAstByNameInAllScope(Ast::Scope& scope, CellI& scopeList, std::function<bool(Ast::Scope& )> hasCb, std::function<Ast::Base*(Ast::Scope&)> getCb);
    Ast::Base* findAstByNameInOneScope(Ast::Scope* currentScope, CellI& scopeList, std::function<bool(Ast::Scope&)> hasCb, std::function<Ast::Base*(Ast::Scope&)> getCb);

    void instantiateTemplateInstances();
    Ast::Struct& instantiateStructT(Ast::StructT& structT, Ast::Struct& compiledStruct, List& inputParams);
    void instantiateFunctionInStructT(Ast::Function& astFunction, Ast::Struct& compiledStruct, Map& inputParameters, Map* associatedTypesPtr = nullptr);
    CellI& instantiateTemplateParamType(CellI& param, CellI& selfType, Map& inputParameters, Map* associatedTypesPtr = nullptr);
    Ast::Base& instantiateAst(CellI& ast, CellI& selfType, Map& inputParameters, Map* associatedTypesPtr = nullptr);

    void compileInstructionsInScope(Ast::Scope& scope, Ast::Scope& resolvedScope);
    void compileInstructionsInStruct(Ast::Struct& __type__);
    void compileInstructionsInEnum(Ast::Enum& enum_);
    CellI& compileInstructionsInFunction(Ast::Function& function);
    void compileFunctionParams(Ast::Function& astFunction, cells::Object& compiledFunction);
    std::string shortFunctionName(Ast::Function& function);
    CellI& compileInstructionsInFunctionAst(Ast::Function& astFunction, CellI& ast, cells::Object& function);
    void checkMethodCall(CellI& astType, CellI& astMethodId);

    void compileDescriptionInScope(Ast::Scope& scope, Ast::Scope& resolvedScope);
    void compileDescriptionInFunction(Ast::Function& function);
    CellI& compileDescriptionInFunctionAst(Ast::Function& astFunction, CellI& ast, cells::Object& function);

    World& w;
    ToolFinder& m_toolFinder;

    Ast::Function* m_currentFn  = nullptr;
    Ast::Base* m_currentStruct  = nullptr;
    CellI* m_lastBlock          = nullptr;
    Ast::Scope* m_scope         = nullptr;
    Ast::Scope* m_resolvedScope = nullptr;

    TrieMap m_earlyStructs;
    TrieMap m_earlyEnumValues;

    TrieMap& m_structs;
    TrieMap& m_unknownStructs;
    TrieMap& m_unknownInstances;

    Library* m_libraryPtr           = nullptr;
    TrieMap* m_compiledFunctionsPtr = nullptr;
    TrieMap* m_compiledStructsPtr   = nullptr;
    TrieMap* m_compiledVariablesPtr = nullptr;
};

} // namespace cells
} // namespace infocell
