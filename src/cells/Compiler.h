#pragma once
#include "Cells.h"
#include "Brain.h"
#include "ToolFinder.h"

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
    ToolFinder* getToolFinder();

protected:
    void registerEarlyStructs(TrieMap& unknownStructs, TrieMap& unknownInstances);
    void resolveEarlyStructsInScope(Ast::Scope& scope, TrieMap& unknownStructs, TrieMap& unknownInstances, Ast::Scope& resolvedScope);

    Ast::Base& resolveType(CellI& typeAst, CellI& resolveState);
    CellI& getCompiledTypeFromResolvedType(CellI& ast);
    CellI& getResolvedTypeById(CellI& id, bool isInstance, CellI& resolveState);
    Ast::Base& resolveTypeName(Ast::Scope& scope, CellI& name);
    Ast::StructT& resolveFullTemplateId(Ast::Scope& scope, CellI& scopeList, CellI& name);

    Ast::Scope& resolveTypesInScope(Ast::Scope& scope, CellI& compilerState);
    Ast::Function& resolveTypesInFunction(Ast::Function& function, CellI& state);
    Ast::Base& resolveTypesInFunctionCode(CellI& resolveState, CellI& ast);
    Ast::Struct& resolveTypesInStruct(Ast::Struct& struct_, CellI& state);
    Ast::Enum& resolveTypesInEnum(Ast::Enum& enum_, CellI& state);
    CellI& resolveTypeInEnumValue(CellI& ast);

    int instantiateTemplateInstances(TrieMap& unknownInstances, Object& compileState, Ast::Scope& resolvedScope);
    Ast::Struct& instantiateStructT(Ast::StructT& structT, List& inputParams, CellI& state);
    CellI& instantiateTemplateParamType(CellI& param, CellI& selfType, Map& inputParameters, CellI& state);
    Ast::Base& instantiateAst(CellI& ast, CellI& selfType, Map& inputParameters, CellI& state);

    CellI& resolveId(CellI& id, CellI& containerId, CellI& unknownContainerId, CellI& resolveState, std::function<CellI&(CellI&)> unknownCb);
    CellI& resolveStructName(CellI& structName, CellI& resolveState);
    Ast::Base& resolveTypeNameAsAst(CellI& structName, CellI& resolveState);
    Ast::Base& resolveSubTypeNameAsAst(CellI& subTypeName, CellI& resolveState);
    CellI& resolveTemplateInstanceId(CellI& structId, CellI& idScope, CellI& resolveState, CellI& ast, CellI& templateParams);
    Ast::Struct& resolveTemplateInstanceIdAsAst(CellI& structId, CellI& idScope, CellI& resolveState, CellI& ast, CellI& templateParams);
    Ast::Base& resolveTemplatedType(CellI& ast, CellI& resolveState);

    CellI& getFullyQualifiedName(Ast::Base& base);
    List& generateTemplateId(CellI& id, CellI& parameters, CellI& resolveState, List& resolvedParams);
    Ast::ResolvedType& resolvedType(CellI& astType, CellI& compiledType);
    Ast::Enum* resolveFullEnumName(Ast::Scope& scope, CellI& scopeList, CellI& name);
    Ast::Struct* resolveFullStructName(Ast::Scope& scope, CellI& scopeList, CellI& name);
    Ast::Base* resolveFullNameInAllScope(Ast::Scope& scope, CellI& scopeList, CellI& id, std::function<bool(Ast::Scope& currentScope)> hasCb, std::function<Ast::Base*(Ast::Scope& currentScope)> getCb);
    Ast::Base* resolveFullNameInOneScope(Ast::Scope* currentScope, CellI& scopeList, std::function<bool(Ast::Scope& currentScope)> hasCb, std::function<Ast::Base*(Ast::Scope& currentScope)> getCb);

    void compileScope(Ast::Scope& scope, Ast::Scope& resolvedScope, CellI& programData, CellI& state);
    CellI& compileStruct(Ast::Struct& struct_, CellI& state);
    CellI& compileEnum(Ast::Enum& enum_, CellI& state);
    CellI& compileFunction(Ast::Function& function, CellI& state);
    std::string shortFunctionName(Ast::Function& function);
    void compileFunctionParams(Ast::Function& astFunction, cells::Object& compiledFunction, cells::Map& functionSlots, cells::Map& subTypesMap, CellI& state);
    CellI& compileFunctionAst(Ast::Function& astFunction, CellI& ast, cells::Object& function, CellI& state);
    void checkMethodCall(CellI& astType, CellI& astMethodId, CellI& state);

    void processDescriptionsInScope(Ast::Scope& scope, CellI& programData, CellI& state);

    Brain& kb;
    TrieMap m_earlyStructs;
    ToolFinder* m_toolFinder = nullptr;
};

} // namespace cells
} // namespace infocell
