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
    ToolFinder* getToolFinder();

protected:
    void registerEarlyStructs();
    void resolveEarlyStructsInScope(Ast::Scope& scope, Ast::Scope& resolvedScope);

    Ast::Base& resolveType(CellI& typeAst);
    CellI& getCompiledTypeFromResolvedType(CellI& ast);
    CellI& getResolvedTypeById(CellI& id, bool isInstance);
    Ast::Base& resolveTypeName(Ast::Scope& scope, CellI& name);
    Ast::StructT& resolveFullTemplateId(Ast::Scope& scope, CellI& scopeList, CellI& name);

    Ast::Scope& resolveTypesInScope(Ast::Scope& scope);
    Ast::Function& resolveTypesInFunction(Ast::Function& function);
    Ast::Base& resolveTypesInFunctionCode(CellI& ast);
    Ast::Struct& resolveTypesInStruct(Ast::Struct& struct_);
    Ast::Enum& resolveTypesInEnum(Ast::Enum& enum_);
    CellI& resolveTypeInEnumValue(CellI& ast);

    int instantiateTemplateInstances(Ast::Scope& resolvedScope);
    Ast::Struct& instantiateStructT(Ast::StructT& structT, List& inputParams);
    CellI& instantiateTemplateParamType(CellI& param, CellI& selfType, Map& inputParameters);
    Ast::Base& instantiateAst(CellI& ast, CellI& selfType, Map& inputParameters);

    CellI& resolveId(CellI& id, TrieMap& container, TrieMap& unresolvedContainer, std::function<CellI&(CellI&)> unknownCb);
    CellI& resolveStructName(CellI& structName);
    CellI& resolveTemplateInstanceId(CellI& structId, CellI& idScope, CellI& ast, CellI& templateParams);
    Ast::Struct& resolveTemplateInstanceIdAsAst(CellI& structId, CellI& idScope, CellI& ast, CellI& templateParams);
    Ast::Base& resolveTemplatedType(CellI& ast);

    CellI& getFullyQualifiedName(Ast::Base& base);
    List& generateTemplateId(CellI& id, CellI& parameters, List& resolvedParams);
    Ast::ResolvedType& resolvedType(CellI& astType, CellI& compiledType);
    Ast::Enum* resolveFullEnumName(Ast::Scope& scope, CellI& scopeList, CellI& name);
    Ast::Struct* resolveFullStructName(Ast::Scope& scope, CellI& scopeList, CellI& name);
    Ast::Base* resolveFullNameInAllScope(Ast::Scope& scope, CellI& scopeList, CellI& id, std::function<bool(Ast::Scope& currentScope)> hasCb, std::function<Ast::Base*(Ast::Scope& currentScope)> getCb);
    Ast::Base* resolveFullNameInOneScope(Ast::Scope* currentScope, CellI& scopeList, std::function<bool(Ast::Scope& currentScope)> hasCb, std::function<Ast::Base*(Ast::Scope& currentScope)> getCb);

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
    TrieMap m_earlyStructs;
    ToolFinder* m_toolFinder = nullptr;

    Ast::Function* m_currentFn        = nullptr;
    Ast::Base* m_currentStruct        = nullptr;
    CellI* m_lastBlock                = nullptr;
    Ast::Scope* m_scope               = nullptr;
    Ast::Scope* m_resolvedScope       = nullptr;
    Ast::Scope* m_globalScope         = nullptr;
    Ast::Scope* m_globalResolvedScope = nullptr;

    TrieMap& m_compiledFunctions;
    TrieMap& m_compiledStructs;

    List& m_functions;
    TrieMap& m_structs;
    TrieMap& m_unknownStructs;
    TrieMap& m_instances;
    TrieMap& m_unknownInstances;
    TrieMap& m_instanceAsts;
    TrieMap& m_unknownInstanceAsts;
    TrieMap& m_compiledVariables;
};

} // namespace cells
} // namespace infocell
