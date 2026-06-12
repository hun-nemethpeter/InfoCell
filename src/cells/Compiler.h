#pragma once
#include "World.h"
#include "ToolFinder.h"

namespace infocell {
namespace cells {

class Compiler
{
    Compiler(const Compiler&) = delete;

public:
    Compiler(World& w);

    CellI& compile(Ast::Scope& scope);
    CellI& reigisterStructBeforeCompilation(CellI& id);
    void registerBuiltInStruct(const std::string& fullName, CellI& compiledStruct);
    ToolFinder& getToolFinder();

protected:
    void registerEarlyStructs();
    void resolveEarlyStructsInScope(Ast::Scope& scope, Ast::Scope& resolvedScope);

    Ast::Scope& resolveTypesInScope(Ast::Scope& scope);
    Ast::Function& resolveTypesInFunction(Ast::Function& function);
    Ast::Base& resolveTypesInFunctionCode(CellI& ast);
    Ast::Struct& resolveTypesInStruct(Ast::Struct& struct_);
    Ast::Enum& resolveTypesInEnum(Ast::Enum& enum_);
    CellI& resolveTypeInEnumValue(CellI& ast);

    Ast::ResolvedType& createResolvedType(CellI& astType, CellI& compiledType);
    CellI& getFullyQualifiedName(Ast::Base& base);
    List& generateTemplateId(CellI& id, CellI& parameters, List& resolvedParams);

    Ast::Base& resolveType(CellI& typeAst);
    CellI& getCompiledTypeFromResolvedType(CellI& ast);
    CellI& getResolvedTypeById(CellI& id, bool isInstance);
    CellI& resolveId(CellI& id, TrieMap& container, TrieMap& unresolvedContainer, std::function<CellI&(CellI& structReference)> unknownCb);
    CellI& getOrCreateStructReference(CellI& structId, TrieMap& unresolvedContainer, std::function<CellI&(CellI& structReference)> unknownCb);
    CellI& resolveStructName(CellI& structName);
    CellI& resolveTemplateInstanceId(CellI& structId, CellI& idScope, CellI& ast, CellI& templateParams);
    Ast::Struct& resolveTemplateInstanceIdAsAst(CellI& structId, CellI& idScope, CellI& ast, CellI& templateParams);
    Ast::Base& resolveTemplatedType(CellI& ast);

    Ast::Base& findEnumOrStructByAstStructName(Ast::Scope& scope, CellI& astStructName);
    Ast::Enum* findEnumByNameInScopes(Ast::Scope& scope, CellI& scopeList, CellI& name);
    Ast::Struct* findStructByNameInScopes(Ast::Scope& scope, CellI& scopeList, CellI& name);
    Ast::StructT& findTemplateByNameInScopes(Ast::Scope& scope, CellI& scopeList, CellI& name);
    Ast::Base* findAstByNameInAllScope(Ast::Scope& scope, CellI& scopeList, CellI& id, std::function<bool(Ast::Scope& currentScope)> hasCb, std::function<Ast::Base*(Ast::Scope& currentScope)> getCb);
    Ast::Base* findAstByNameInOneScope(Ast::Scope* currentScope, CellI& scopeList, std::function<bool(Ast::Scope& currentScope)> hasCb, std::function<Ast::Base*(Ast::Scope& currentScope)> getCb);

    void instantiateTemplateInstances();
    Ast::Struct& instantiateStructT(Ast::StructT& structT, List& inputParams);
    CellI& instantiateTemplateParamType(CellI& param, CellI& selfType, Map& inputParameters);
    Ast::Base& instantiateAst(CellI& ast, CellI& selfType, Map& inputParameters);

    void compileScope(Ast::Scope& scope, Ast::Scope& resolvedScope);
    CellI& compileStruct(Ast::Struct& struct_);
    CellI& compileEnum(Ast::Enum& enum_);
    CellI& compileFunction(Ast::Function& function);
    std::string shortFunctionName(Ast::Function& function);
    void compileFunctionParams(Ast::Function& astFunction, cells::Object& compiledFunction, cells::Map& functionSlots, cells::Map& subTypesMap);
    CellI& compileFunctionAst(Ast::Function& astFunction, CellI& ast, cells::Object& function);
    void checkMethodCall(CellI& astType, CellI& astMethodId);

    void processDescriptionsInScope(Ast::Scope& scope);

    World& w;
    ToolFinder& m_toolFinder;

    Ast::Function* m_currentFn  = nullptr;
    Ast::Base* m_currentStruct  = nullptr;
    CellI* m_lastBlock          = nullptr;
    Ast::Scope* m_scope         = nullptr;
    Ast::Scope* m_resolvedScope = nullptr;

    TrieMap m_earlyStructs;

    TrieMap& m_compiledFunctions;
    TrieMap& m_compiledStructs;
    TrieMap& m_compiledVariables;

    List& m_functions;
    TrieMap& m_structs;
    TrieMap& m_unknownStructs;
    TrieMap& m_unknownInstances;
    TrieMap& m_unknownInstanceAsts;

    Object& m_programData;
};

} // namespace cells
} // namespace infocell
