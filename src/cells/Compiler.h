#pragma once
#include "Cells.h"
#include "Brain.h"

namespace infocell {
namespace cells {

class Compiler
{
    Compiler(const Compiler&) = delete;

public:
    Compiler(Brain& kb);

    CellI& compile(Ast::Scope& scope);
    CellI& reigisterStructBeforeCompilation(CellI& id);
    void registerBuiltInStruct(const std::string& fullName, CellI& compiledStruct);

protected:
    Ast::Base& resolveType(CellI& typeAst, CellI& resolveState);
    CellI& getCompiledTypeFromResolvedType(CellI& ast);
    CellI& getResolvedTypeById(CellI& id, bool isInstance, CellI& resolveState);
    Ast::Base& resolveTypeName(Ast::Scope& scope, CellI& name);
    Ast::StructT& resolveFullTemplateId(Ast::Scope& scope, CellI& scopeList, CellI& name);
    void registerEarlyStructs(TrieMap& unknownStructs, TrieMap& unknownInstances);
    Ast::Scope& resolveTypesInScope(Ast::Scope& scope, CellI& compilerState);
    void resolveEarlyStructsInScope(Ast::Scope& scope, TrieMap& unknownStructs, TrieMap& unknownInstances, Ast::Scope& resolvedScope);
    int instantiateTemplateInstances(TrieMap& unknownInstances, Object& compileState, Ast::Scope& resolvedScope);
    CellI& resolveId(CellI& id, CellI& containerId, CellI& unknownContainerId, CellI& resolveState, std::function<CellI&(CellI&)> unknownCb);
    CellI& resolveStructName(CellI& structName, CellI& resolveState);
    Ast::Base& resolveTypeNameAsAst(CellI& structName, CellI& resolveState);
    Ast::Base& resolveSubTypeNameAsAst(CellI& subTypeName, CellI& resolveState);
    CellI& resolveTemplateInstanceId(CellI& structId, CellI& idScope, CellI& resolveState, CellI& ast, CellI& templateParams);
    Ast::Struct& resolveTemplateInstanceIdAsAst(CellI& structId, CellI& idScope, CellI& resolveState, CellI& ast, CellI& templateParams);
    Ast::Base& resolveTemplatedType(CellI& ast, CellI& resolveState);
    List& generateTemplateId(CellI& id, CellI& parameters, CellI& resolveState, List& resolvedParams);
    Ast::ResolvedType& resolvedType(CellI& astType, CellI& compiledType);
    Ast::Enum* resolveFullEnumName(Ast::Scope& scope, CellI& scopeList, CellI& name);
    Ast::Struct* resolveFullStructName(Ast::Scope& scope, CellI& scopeList, CellI& name);
    Ast::Base* resolveFullNameInAllScope(Ast::Scope& scope, CellI& scopeList, CellI& id, std::function<bool(Ast::Scope& currentScope)> hasCb, std::function<Ast::Base*(Ast::Scope& currentScope)> getCb);
    Ast::Base* resolveFullNameInOneScope(Ast::Scope* currentScope, CellI& scopeList, std::function<bool(Ast::Scope& currentScope)> hasCb, std::function<Ast::Base*(Ast::Scope& currentScope)> getCb);

    void compileTheResolvedAsts(CellI& programData, CellI& state);
    void processDescriptionsInAsts(CellI& programData, CellI& state);

    Brain& kb;
    TrieMap m_earlyStructs;
};

} // namespace cells
} // namespace infocell
