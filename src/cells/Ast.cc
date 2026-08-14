#include "Ast.h"

#include "World.h"

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include "util/Log.h"

namespace infocell {
namespace cells {

// ============================================================================
Ast::Base::Base(World& w, CellI& classCell, const std::string& label) :
    Object(w, classCell, label)
{
}

Ast::Parameter::Parameter(World& w, CellI& key) :
    BaseT<Parameter>(w, w.std.ast.Parameter, key.label())
{
    set(w.id.key, key);
}

Ast::ResolvedType::ResolvedType(World& w, CellI& astType, CellI& compiledType) :
    BaseT<ResolvedType>(w, w.std.ast.ResolvedType, astType.label())
{
    set(w.id.ast, astType);
    set(w.id.compiled, compiledType);
}

Ast::Call& Ast::Parameter::operator/(Base& key)
{
    return w.ast.get(*this, key);
}

Ast::Call& Ast::Parameter::operator/(const std::string& key)
{
    return w.ast.get(*this, key);
}

Ast::Call& Ast::Parameter::operator()(const std::string& method)
{
    return w.ast.call(*this, method);
}

Ast::KVPair::KVPair(World& w, CellI& key, CellI& value) :
    BaseT<KVPair>(w, w.std.KVPair, "std.KVPair")
{
    set(w.id.key, key);
    set(w.id.value, value);
}

Ast::Call::Call(World& w, CellI& method) :
    BaseT<Call>(w, w.std.ast.Call, "ast.Call")
{
    set(w.id.method, method);
}

Ast::Call::Call(World& w, CellI& self, CellI& method) :
    BaseT<Call>(w, w.std.ast.Call, "ast.Call")
{
    set(w.id.parameters, w.list(w.ast.parameterInit(w.id.self, self)));
    set(w.id.method, method);
}

Ast::Call& Ast::Call::operator/(Base& key)
{
    return w.ast.get(*this, key);
}

Ast::Call& Ast::Call::operator/(const std::string& key)
{
    return w.ast.get(*this, key);
}

Ast::Call& Ast::Call::operator()(const std::string& method)
{
    return w.ast.call(*this, method);
}

Ast::Call& Ast::Call::operator()(CellI& name, CellI& value)
{
    Parameter& parameter = w.ast.parameterInit(name, value);
    if (missing(w.id.parameters)) {
        set(w.id.parameters, w.list(parameter));
    } else {
        parameters().add(parameter);
    }
    return *this;
}

Ast::Call& Ast::Call::operator()(const std::string& nameStr, CellI& value)
{
    return operator()(w.name(nameStr), value);
}

Ast::Call& Ast::Call::operator()(const std::string& nameStr, const std::string& valueStr)
{
    return operator()(nameStr, w.ast._(valueStr));
}

List& Ast::Call::parameters()
{
    return static_cast<List&>(get(w.id.parameters));
}

Ast::ConstVar::ConstVar(World& w, CellI& value) :
    BaseT<ConstVar>(w, w.std.ast.ConstVar, "ast.ConstVar")
{
    set(w.id.value, value);
}

Ast::Call& Ast::ConstVar::operator/(Base& key)
{
    return w.ast.get(*this, key);
}

Ast::Call& Ast::ConstVar::operator/(const std::string& key)
{
    return w.ast.get(*this, key);
}

Ast::TypeName::TypeName(World& w, CellI& name) :
    BaseT<TypeName>(w, w.std.ast.TypeName, "ast.TypeName")
{
    set(w.id.name, name);
}

Ast::Self::Self(World& w) :
    BaseT<Self>(w, w.std.ast.Self, "ast.Self")
{
}

Ast::Call& Ast::Self::operator()(const std::string& method)
{
    return w.ast.call(*this, method);
}

Ast::SelfType::SelfType(World& w) :
    BaseT<SelfType>(w, w.std.ast.SelfType, "ast.SelfType")
{
}

Ast::Continue::Continue(World& w) :
    BaseT<Continue>(w, w.std.ast.Continue, "ast.Continue")
{
}

Ast::Break::Break(World& w) :
    BaseT<Break>(w, w.std.ast.Break, "ast.Break")
{
}

Ast::PrimitiveToolName::PrimitiveToolName(World& w, CellI& name) :
    BaseT<PrimitiveToolName>(w, w.std.ast.PrimitiveToolName, "ast.PrimitiveToolName")
{
    set(w.id.name, name);
}

Ast::Return::Return(World& w) :
    BaseT<Return>(w, w.std.ast.Return, "ast.Return")
{
}

Ast::Return::Return(World& w, CellI& value) :
    BaseT<Return>(w, w.std.ast.Return, "ast.Return")
{
    set(w.id.value, value);
}

Ast::Call& Ast::Return::operator()(const std::string& method)
{
    return w.ast.call(*this, method);
}

Ast::Block::Block(World& w, List& list) :
    BaseT<Block>(w, w.std.ast.Block, "ast.Block")
{
    set(w.id.asts, list);
}

void Ast::Block::mergeFrom(Base& statement)
{
    List& toAstList = static_cast<List&>((*this)[w.id.asts]);
    if (&statement.__type__() == &w.std.ast.Block) {
        List& fromAstList = static_cast<List&>(statement[w.id.asts]);
        for (CellI& ast : fromAstList) {
            toAstList.add(ast);
        }
    } else {
        toAstList.add(statement);
    }
}

template <>
Ast::Items<TrieMap, Ast::Scope>& Ast::Scope::getItemMember()
{
    return scopesImpl;
}

template <>
Ast::Items<TrieMap, Ast::Function>& Ast::Scope::getItemMember()
{
    return functionsImpl;
}

template <>
Ast::Items<TrieMap, Ast::FunctionT>& Ast::Scope::getItemMember()
{
    return functionTsImpl;
}

template <>
Ast::Items<TrieMap, Ast::Var>& Ast::Scope::getItemMember()
{
    return variablesImpl;
}

template <>
Ast::Items<TrieMap, Ast::Struct>& Ast::Scope::getItemMember()
{
    return structsImpl;
}

template <>
Ast::Items<TrieMap, Ast::StructT>& Ast::Scope::getItemMember()
{
    return structTsImpl;
}

template <>
Ast::Items<TrieMap, Ast::Trait>& Ast::Scope::getItemMember()
{
    return traitsImpl;
}

template <>
Ast::Items<TrieMap, Ast::Enum>& Ast::Scope::getItemMember()
{
    return enumsImpl;
}

Ast::Scope::Scope(World& w, const std::string& nameStr) :
    BaseT<Scope>(w, w.std.ast.Scope, nameStr),
    scopesImpl(w, "scopes", *this),
    functionsImpl(w, "functions", *this),
    functionTsImpl(w, "functionTs", *this),
    variablesImpl(w, "variables", *this),
    structsImpl(w, "structs", *this),
    structTsImpl(w, "structTs", *this),
    traitsImpl(w, "traits", *this),
    enumsImpl(w, "enums", *this),
    earlyStructs(w, w.std.Cell, w.std.Cell, "earlyStructs")
{
    set(w.id.name, w.name(nameStr));
}

Ast::Scope& Ast::Scope::getRootScope()
{
    auto* currentScope = this;

    while (currentScope->has(w.id.scope)) {
        currentScope = &static_cast<Scope&>(currentScope->get(w.id.scope));
    }

    return *currentScope;
}

Ast::Scope& Ast::Scope::createLink()
{
    Scope& ret = Scope::New(w, get(w.id.name).label());
    ret.set(w.id.link, *this);

    return ret;
}

Ast::Scope* Ast::Scope::getLinkedScope()
{
    if (missing(w.id.link)) {
        return nullptr;
    }
    return &static_cast<Scope&>(get(w.id.link));
}

void Ast::Scope::mergeTo(Scope& targetScope, MergeMode mergeMode)
{
    Scope& from = *this;
    Scope& to   = targetScope;

    TrieMap& toScopeMap = to.items<Ast::Scope>();

    for (CellI& kvPair : from.items<Ast::Scope>()) {
        auto& libScopeKey   = kvPair[w.id.key];
        auto& libScopeValue = static_cast<Ast::Scope&>(kvPair[w.id.value]);
        switch (mergeMode) {
        case MergeMode::Copy:
            toScopeMap.add(libScopeKey, libScopeValue);
            break;
        case MergeMode::Link:
            auto& libScopeLink = libScopeValue.createLink();
            toScopeMap.add(libScopeKey, libScopeLink);
            break;
        }
    }
}

Ast::StructBase::StructBase(World& w, CellI& astType, CellI& name, const std::string& nameStr) :
    Base(w, astType, nameStr)
{
    set(w.id.name, name);
}

Ast::Function& Ast::StructBase::addPrimitiveFunction(CellI& ast, CellI& op, const std::string& nameStr)
{
    Ast::Function& method = static_cast<Ast::Function&>(ast);
    method.label(nameStr);
    method.set(w.id.name, w.name(nameStr));
    method.set(w.id.primitiveTool, op);
    List& fullyQualifiedName = *new List(w, w.std.Cell);
    fullyQualifiedName.add(op);
    method.set(w.id.fullyQualifiedName, fullyQualifiedName);
    addMethod(method);
    op.set(w.id.ast, method);

    return method;
}

Ast::Function& Ast::StructBase::addMethod(const std::string& nameStr)
{
    Ast::Function& method = *new Ast::Function(w, nameStr);
    addMethod(method);

    return method;
}

void Ast::StructBase::addMethod(Function& method, ParameterModification parameterModification)
{
    auto& name = method[w.id.name];

    if (parameterModification == ParameterModification::AddSelfParam) {
        CellI* selfTypePtr = nullptr;
        if (&__type__() == &w.std.ast.TraitImpl) {
            selfTypePtr = &w.ast.associatedType(w.id.self);
        } else {
            selfTypePtr = &w.ast.selfType();
        }
        Parameter& selfParameter = w.ast.parameterDeclaration(w.id.self, *selfTypePtr);
        if (method.has(w.id.parameters)) {
            method.parameters().addFront(w.id.self, selfParameter);
        } else {
            method.parameters(selfParameter);
        }
    }

    if (missing(w.id.methods)) {
        set(w.id.methods, *new Map(w, w.std.Cell, w.std.ast.Function, "Map<ConstVar, Type::Ast::Function>(...)"));
    }
    if (methods().hasKey(name)) {
        throw "Already registered!";
    }
    methods().add(name, method);
}

Ast::TraitImpl& Ast::StructBase::addTraitImpl(const std::string& nameStr)
{
    Ast::TraitImpl& traitImpl = *new Ast::TraitImpl(w, nameStr);
    traitImpl.associatedTypes(w.ast.parameterDeclaration(w.id.self, w.ast.selfType()));
    addTraitImpl(traitImpl);

    return traitImpl;
}

void Ast::StructBase::addTraitImpl(Ast::TraitImpl& traitImpl)
{
    auto& name = traitImpl[w.id.name];

    if (missing(w.id.traitImpls)) {
        set(w.id.traitImpls, *new Map(w, w.std.Cell, w.std.ast.TraitImpl, "Map<ConstVar, Type::Ast::TraitImpl>(...)"));
    }
    if (traitImpls().hasKey(name)) {
        throw "Already registered!";
    }
    traitImpls().add(name, traitImpl);
}

Ast::StructBase& Ast::StructBase::returnType(CellI& type)
{
    if (missing(w.id.primitiveTool)) {
        throw "Setting return type on a node which is not a primitive tool!";
    }
    set(w.id.returnType, type);

    return *this;
}

void Ast::StructBase::addMember(Member& member)
{
    if (missing(w.id.members)) {
        set(w.id.members, *new Map(w, w.std.Cell, w.std.ast.Member));
    }
    members().add(member[w.id.key], member);
}

Ast::StructBase& Ast::StructBase::typeAliases(KVPair& kvPair)
{
    if (missing(w.id.typeAliases)) {
        set(w.id.typeAliases, *new Map(w, w.std.Cell, w.std.ast.Base));
    }
    CellI& key  = kvPair[w.id.key];
    CellI& type = kvPair[w.id.value];

    typeAliases().add(key, type);

    return *this;
}

Ast::StructBase& Ast::StructBase::memberOf(CellI& type)
{
    if (missing(w.id.memberOf)) {
        set(w.id.memberOf, *new List(w, w.std.Struct));
    }
    memberOf().add(type);

    return *this;
}

Map& Ast::StructBase::methods()
{
    if (missing(w.id.methods)) {
        throw "No methods!";
    } else {
        return static_cast<Map&>(get(w.id.methods));
    }
}

Map& Ast::StructBase::traitImpls()
{
    if (missing(w.id.traitImpls)) {
        throw "No traitImpls!";
    } else {
        return static_cast<Map&>(get(w.id.traitImpls));
    }
}

Map& Ast::StructBase::members()
{
    if (missing(w.id.members)) {
        throw "No members!";
    } else {
        return static_cast<Map&>(get(w.id.members));
    }
}

Map& Ast::StructBase::typeAliases()
{
    if (missing(w.id.typeAliases)) {
        throw "No typeAliases!";
    } else {
        return static_cast<Map&>(get(w.id.typeAliases));
    }
}

List& Ast::StructBase::memberOf()
{
    if (missing(w.id.memberOf)) {
        throw "No memberOf!";
    } else {
        return static_cast<List&>(get(w.id.memberOf));
    }
}

CellI& Ast::StructBase::name()
{
    return get(w.id.name);
}

Ast::Base& Ast::StructBase::getTypeAlias(CellI& name)
{
    return static_cast<Ast::Base&>(typeAliases().getValue(name));
}

Ast::Struct::Struct(World& w, const std::string& nameStr) :
    StructBase(w, w.std.ast.Struct, w.name(nameStr), nameStr)
{
}

Ast::Struct::Struct(World& w, CellI& name) :
    StructBase(w, w.std.ast.Struct, name, name.label())
{
}

Ast::StructT::StructT(World& w, CellI& name) :
    StructBase(w, w.std.ast.StructT, name, name.label())
{
}

Ast::StructT::StructT(World& w, const std::string& nameStr) :
    StructBase(w, w.std.ast.StructT, w.name(nameStr), nameStr)
{
}

Ast::StructT& Ast::StructT::templateParams(Parameter& parameter)
{
    if (missing(w.id.templateParams)) {
        set(w.id.templateParams, *new Map(w, w.std.Cell, w.std.Struct));
    }
    CellI& key  = parameter[w.id.key];
    CellI& type = parameter[w.id.type];
    if (!(&type.__type__() == &w.std.ast.ConstVar || &type.__type__() == &w.std.ast.TemplatedType)) {
        throw "Invalid template param type!";
    }
    CellI* paramType = nullptr;
    if (&type.__type__() == &w.std.ast.ConstVar) {
        paramType = &type[w.id.value];
    } else {
        throw "TODO";
    }
    templateParams().add(key, *paramType);

    return *this;
}

Map& Ast::StructT::templateParams()
{
    if (missing(w.id.templateParams)) {
        throw "No templateParams!";
    } else {
        return static_cast<Map&>(get(w.id.templateParams));
    }
}

Ast::Trait::Trait(World& w, CellI& name) :
    StructBase(w, w.std.ast.Trait, name, name.label())
{
}

Ast::Trait::Trait(World& w, const std::string& nameStr) :
    StructBase(w, w.std.ast.Trait, w.name(nameStr), nameStr)
{
}

Ast::Trait& Ast::Trait::templateParams(Parameter& parameter)
{
    // TODO

    return *this;
}

Ast::Trait& Ast::Trait::associatedTypes(Parameter& parameter)
{
    // TODO

    return *this;
}


Ast::TraitImpl::TraitImpl(World& w, CellI& name) :
    StructBase(w, w.std.ast.TraitImpl, name, name.label())
{
}

Ast::TraitImpl::TraitImpl(World& w, const std::string& nameStr) :
    StructBase(w, w.std.ast.TraitImpl, w.name(nameStr), nameStr)
{
}

Ast::TraitImpl& Ast::TraitImpl::templateParams(Parameter& parameter)
{
    // TODO

    return *this;
}

Ast::TraitImpl& Ast::TraitImpl::associatedTypes(Parameter& parameter)
{
    if (missing(w.id.associatedTypes)) {
        set(w.id.associatedTypes, *new Map(w, w.std.Cell, w.std.ast.Base));
    }
    CellI& key  = parameter[w.id.key];
    CellI& type = parameter[w.id.type];

    associatedTypes().add(key, type);

    return *this;
}

Map& Ast::TraitImpl::associatedTypes()
{
    if (missing(w.id.associatedTypes)) {
        throw "No associatedTypes!";
    } else {
        return static_cast<Map&>(get(w.id.associatedTypes));
    }
}

Ast::EnumValue::EnumValue(World& w, const std::string& name) :
    BaseT<EnumValue>(w, w.std.ast.EnumValue, name)
{
    set(w.id.name, w.name(name));
    label(name);
}

Ast::EnumValue::EnumValue(World& w, const std::string& name, CellI& value) :
    BaseT<EnumValue>(w, w.std.ast.EnumValue, name)
{
    set(w.id.name, w.name(name));
    set(w.id.value, value);
    label(name);
}

Ast::TypedEnumValue::TypedEnumValue(World& w, const std::string& nameStr, CellI& enumType) :
    BaseT<TypedEnumValue>(w, w.std.ast.TypedEnumValue, nameStr)
{
    set(w.id.name, w.name(nameStr));
    set(w.id.enumType, enumType);
    label(nameStr);
}

Ast::TypedEnumValue::TypedEnumValue(World& w, CellI& name, CellI& enumType) :
    BaseT<TypedEnumValue>(w, w.std.ast.TypedEnumValue, name.label())
{
    set(w.id.name, name);
    set(w.id.enumType, enumType);
    label(name.label());
}

Ast::TypedEnumValue::TypedEnumValue(World& w, const std::string& nameStr, CellI& enumType, CellI& value) :
    BaseT<TypedEnumValue>(w, w.std.ast.TypedEnumValue, nameStr)
{
    set(w.id.name, w.name(nameStr));
    set(w.id.value, value);
    set(w.id.enumType, enumType);
    label(nameStr);
}

Ast::Enum::Enum(World& w, CellI& name) :
    StructBase(w, w.std.ast.Enum, name, name.label())
{
    set(w.id.name, name);
    label(name.label());
}

Ast::Enum::Enum(World& w, const std::string& nameStr) :
    StructBase(w, w.std.ast.Enum, w.name(nameStr), nameStr)
{
    set(w.id.name, w.name(nameStr));
    label(nameStr);
}

Ast::Enum& Ast::Enum::values(Base& value)
{
    if (missing(w.id.values)) {
        set(w.id.values, *new Map(w, w.std.Cell, w.std.ast.Base));
    }
    values().add(value[w.id.name], value);
    value.set(w.id.enum_, *this);

    return *this;
}
CellI& Ast::Enum::resolveEnumValue(CellI& ast)
{
    if (&ast.__type__() == &w.std.ast.ConstVar) {
        return ast[w.id.value];
    }

    throw "Unknown enum value!";
}

TrieMap& Ast::Enum::values()
{
    if (missing(w.id.values)) {
        throw "No values!";
    } else {
        return static_cast<TrieMap&>(get(w.id.values));
    }
}

Ast::FunctionBase::FunctionBase(World& w, CellI& astType, CellI& name, const std::string& nameStr) :
    Base(w, astType, nameStr)
{
    set(w.id.name, name);
}

Ast::Function::Function(World& w, CellI& name) :
    FunctionBase(w, w.std.ast.Function, name, name.label())
{
}

Ast::Function::Function(World& w, const std::string& nameStr) :
    FunctionBase(w, w.std.ast.Function, w.name(nameStr), nameStr)
{
}

Ast::FunctionBase& Ast::FunctionBase::memberMapping(KVPair& mapping)
{
    if (missing(w.id.memberMapping)) {
        set(w.id.memberMapping, *new Map(w, w.std.String, w.std.ast.Base));
    }
    memberMapping().add(mapping[w.id.key], mapping[w.id.value]);

    return *this;
}

void Ast::FunctionBase::addParameter(Parameter& parameter)
{
    if (missing(w.id.parameters)) {
        set(w.id.parameters, *new Map(w, w.std.Cell, w.std.ast.Parameter));
    }
    parameters().add(parameter[w.id.key], parameter);
}

Ast::FunctionBase& Ast::FunctionBase::constructor()
{
    set(w.id.isConstructor, w.true_);

    return *this;
}

Ast::FunctionBase& Ast::FunctionBase::returnType(CellI& type)
{
    set(w.id.returnType, type);

    return *this;
}

Ast::FunctionBase& Ast::FunctionBase::returnType(const std::string& typeStr)
{
    return returnType(w.ast.typeName(typeStr));
}

Ast::Description& Ast::FunctionBase::descriptionBegin()
{
    Description& ret = *new Description(*this);
    set(w.id.description, ret);
    return ret;
}

Ast::Description::Description(World& w) :
    Object(w, w.std.ast.Description, "ast.Description"),
    m_parent(nullptr)
{
}


Ast::Description::Description(FunctionBase& parent) :
    Object(parent.w, parent.w.std.ast.Description, "ast.Description"),
    m_parent(&parent)
{
}

void Ast::Description::addPrompt(Base& prompt)
{
    addPrompt(w.ast.list(prompt));
}

void Ast::Description::addPrompt(List& prompt)
{
    set(w.id.prompt, prompt);
}

void Ast::Description::addConclusions(List& conclusions)
{
    set(w.id.conclusions, conclusions);
}

void Ast::Description::addSelfBuilders(List& selfBuilders)
{
    set(w.id.selfBuilders, selfBuilders);
}

Ast::FunctionBase& Ast::Description::descriptionEnd()
{
    return *m_parent;
}

void Ast::FunctionBase::addInstructionBlock(Block& block)
{
    set(w.id.instructions, block);
}

Map& Ast::FunctionBase::memberMapping()
{
    if (missing(w.id.memberMapping)) {
        throw "No member mapping!";
    } else {
        return static_cast<Map&>(get(w.id.memberMapping));
    }
}

Map& Ast::FunctionBase::parameters()
{
    if (missing(w.id.parameters)) {
        throw "No parameters!";
    } else {
        return static_cast<Map&>(get(w.id.parameters));
    }
}

CellI& Ast::FunctionBase::returnType()
{
    if (missing(w.id.returnType)) {
        throw "No returnType!";
    } else {
        return get(w.id.returnType);
    }
}

Ast::Base& Ast::FunctionBase::instructions()
{
    if (missing(w.id.instructions)) {
        throw "No instructions!";
    } else {
        return static_cast<Ast::Base&>(get(w.id.instructions));
    }
}

Ast::Base& Ast::FunctionBase::description()
{
    if (missing(w.id.description)) {
        throw "No description!";
    } else {
        return static_cast<Ast::Base&>(get(w.id.description));
    }
}

Ast::FunctionT::FunctionT(World& w, CellI& name, const std::string& nameStr) :
    FunctionBase(w, w.std.ast.FunctionT, name, nameStr)
{
}

Ast::FunctionT::FunctionT(World& w, const std::string& nameStr) :
    FunctionBase(w, w.std.ast.FunctionT, w.name(nameStr), nameStr)
{
}

Ast::If::If(World& w, Base& condition) :
    BaseT<If>(w, w.std.ast.If, "ast.If")
{
    set(w.id.condition, condition);
}

Ast::If::If(World& w, Base& condition, Base& thenBranch) :
    BaseT<If>(w, w.std.ast.If, "ast.If")
{
    set(w.id.condition, condition);
    set(w.id.then, thenBranch);
}

Ast::If::If(World& w, Base& condition, Base& thenBranch, Base& elseBranch) :
    BaseT<If>(w, w.std.ast.If, "ast.If")
{
    set(w.id.condition, condition);
    set(w.id.then, thenBranch);
    set(w.id.else_, elseBranch);
}

Ast::If& Ast::If::then_(Base& thenBranch)
{
    set(w.id.then, thenBranch);
    return *this;
}

Ast::If& Ast::If::else_(Base& elseBranch)
{
    set(w.id.else_, elseBranch);
    return *this;
}

Ast::Match::Match(World& w, Base& enum_) :
    BaseT<Match>(w, w.std.ast.Match, "ast.Match")
{
    set(w.id.enum_, enum_);
}

Ast::Match& Ast::Match::case_(CellI& memberName, Base& op)
{
    if (missing(w.id.cases)) {
        set(w.id.cases, *new TrieMap(w, w.std.List, w.std.ast.Base));
    }
    auto& casesMap = static_cast<TrieMap&>(get(w.id.cases));
    casesMap.add(memberName, op);

    return *this;
}

Ast::Match& Ast::Match::case_(const std::string& memberStr, Base& op)
{
    return case_(w.name(memberStr), op);
}

Ast::Do::Do(World& w, Base& statement) :
    BaseT<Do>(w, w.std.ast.Do, "ast.Do")
{
    set(w.id.statement, statement);
}

Ast::Do& Ast::Do::while_(Base& condition)
{
    set(w.id.condition, condition);
    return *this;
}

Ast::While::While(World& w, Base& condition) :
    BaseT<While>(w, w.std.ast.While, "ast.While")
{
    set(w.id.condition, condition);
}

Ast::While& Ast::While::do_(Base& statement)
{
    set(w.id.statement, statement);
    return *this;
}

Ast::For::For(World& w, Base& var) :
    BaseT<For>(w, w.std.ast.For, "ast.For")
{
    set(w.id.variable, var);
}

Ast::For& Ast::For::in(Base& container)
{
    set(w.id.container, container);
    return *this;
}

Ast::For& Ast::For::operator()(Base& statement)
{
    set(w.id.statement, statement);
    return *this;
}

Ast::Var::Var(World& w, const std::string& nameStr) :
    BaseT<Var>(w, w.std.ast.Var, nameStr)
{
    set(w.id.name, w.name(nameStr));
}

Ast::Var::Var(World& w, CellI& name) :
    BaseT<Var>(w, w.std.ast.Var, name.label())
{
    set(w.id.name, name);
}

Ast::Call& Ast::Var::operator=(Base& value)
{
    return w.ast.set(*this, w.ast._(w.id.value), value);
}

Ast::Call& Ast::Var::operator*()
{
    return w.ast.get(*this, w.ast._(w.id.value));
}

Ast::Call& Ast::Var::operator()(const std::string& method)
{
    return w.ast.call(*(*this), method);
}

Ast::UnknownVar::UnknownVar(World& w, CellI& value) :
    BaseT<UnknownVar>(w, w.std.ast.UnknownVar, "ast.UnknownVar")
{
    set(w.id.value, value);
}

Ast::Call& Ast::UnknownVar::operator/(Base& key)
{
    return w.ast.get(*this, key);
}

Ast::Call& Ast::UnknownVar::operator/(const std::string& key)
{
    return w.ast.get(*this, key);
}

Ast::Member::Member(World& w, CellI& key) :
    BaseT<Member>(w, w.std.ast.Member, "ast.Member")
{
    set(w.id.key, key);
}

Ast::Member::Member(World& w, CellI& key, CellI& type) :
    BaseT<Member>(w, w.std.ast.Member, "ast.Member")
{
    set(w.id.key, key);
    set(w.id.type, type);
}

Ast::Call& Ast::Member::operator=(Base& value)
{
    return w.ast.set(w.ast.self(), w.ast._(get(w.id.key)), value);
}

Ast::Call& Ast::Member::operator/(Base& key)
{
    return w.ast.get(*this, key);
}

Ast::Call& Ast::Member::operator/(const std::string& key)
{
    return w.ast.get(*this, key);
}

Ast::Call& Ast::Member::exist()
{
    return w.ast.has(w.ast.self(), w.ast._(get(w.id.key)));
}

Ast::Call& Ast::Member::missing()
{
    return w.ast.missing(w.ast.self(), ConstVar::New(w, get(w.id.key)));
}

Ast::Call& Ast::Member::operator()(const std::string& method)
{
    return w.ast.call(*this, method);
}

Ast::TypeAlias::TypeAlias(World& w, CellI& name) :
    BaseT<TypeAlias>(w, w.std.ast.TypeAlias, "ast.TypeAlias")
{
    set(w.id.name, name);
}

Ast::TemplatedType::TemplatedType(World& w, CellI& id, CellI& typeList) :
    BaseT<TemplatedType>(w, w.std.ast.TemplatedType, "ast.TemplatedType")
{
    set(w.id.id, id);
    set(w.id.parameters, typeList);
}

void Ast::TemplatedType::addParam(const std::string& key, CellI& type)
{
    List& paramList = static_cast<List&>(get(w.id.parameters));
    paramList.add(w.ast.parameterDeclaration(key, type));
}

void Ast::TemplatedType::addParam(const std::string& key, const std::string& type)
{
    addParam(key, w.ast.typeName(type));
}

Ast::TemplateParam::TemplateParam(World& w, CellI& key) :
    BaseT<TemplateParam>(w, w.std.ast.TemplateParam, "ast.TemplateParam")
{
    set(w.id.key, key);
}

Ast::AssociatedType::AssociatedType(World& w, CellI& key) :
    BaseT<AssociatedType>(w, w.std.ast.AssociatedType, "ast.AssociatedType")
{
    set(w.id.key, key);
}

Ast::New::New(World& w, Base& objectType) :
    BaseT<New>(w, w.std.ast.New, "ast.New")
{
    set(w.id.objectType, objectType);
}

Ast::New::New(World& w, Base& objectType, Base& constructor) :
    BaseT<New>(w, w.std.ast.New, "ast.New")
{
    set(w.id.objectType, objectType);
    set(w.id.constructor, constructor);
}

Ast::New& Ast::New::operator()(const std::string& nameStr, CellI& value)
{
    Parameter& parameter = w.ast.parameterInit(w.name(nameStr), value);
    if (missing(w.id.parameters)) {
        set(w.id.parameters, w.list(parameter));
    } else {
        parameters().add(parameter);
    }

    return *this;
}

List& Ast::New::parameters()
{
    return static_cast<List&>(get(w.id.parameters));
}

// ============================================================================
Ast::Ast(World& w) :
    w(w),
    id(w.id),
    std(w.std),
    arc(w.arc),
    _0_(w._0_),
    _1_(w._1_),
    _2_(w._2_),
    _3_(w._3_),
    _4_(w._4_),
    _5_(w._5_),
    _6_(w._6_),
    _7_(w._7_),
    _8_(w._8_),
    _9_(w._9_)
{
}

Ast::ConstVar& Ast::_(CellI& cellRef)
{
    return const_(cellRef);
}

Ast::ConstVar& Ast::_(const std::string& nameStr)
{
    return const_(w.name(nameStr));
}

Ast::ConstVar& Ast::_(int number)
{
    return _(w.pools.numbers.get(number));
}

CellI& Ast::name(const std::string& str)
{
    return w.name(str);
}

Ast::ConstVar& Ast::true_()
{
    return _(w.true_);
}

Ast::ConstVar& Ast::false_()
{
    return _(w.false_);
}

Ast::ConstVar& Ast::const_(CellI& cell)
{
    return ConstVar::New(w, cell);
}

Ast::ConstVar& Ast::const_(const std::string& nameStr)
{
    return const_(w.name(nameStr));
}

Ast::ConstVar& Ast::const_(int number)
{
    return _(w.pools.numbers.get(number));
}

Ast::TypeName& Ast::typeName(CellI& id)
{
    return TypeName::New(w, id);
}

Ast::TypeName& Ast::typeName(const std::string& idStr)
{
    CellI& ret = processNamespacedName(idStr, [this](const std::string& outName) -> CellI& {
        return TypeName::New(w, w.name(outName));
    });

    return static_cast<Ast::TypeName&>(ret);
}

Ast::Self& Ast::self()
{
    return Self::New(w);
}

Ast::Self& Ast::self(CellI& value)
{
    auto& ret = Self::New(w);
    ret.set(id.value, value);
    return ret;
}

Ast::Continue& Ast::continue_()
{
    return Continue::New(w);
}

Ast::Break& Ast::break_()
{
    return Break::New(w);
}

Ast::SelfType& Ast::selfType()
{
    return SelfType::New(w);
}

Ast::Return& Ast::return_()
{
    return Return::New(w);
}

Ast::Return& Ast::return_(CellI& value)
{
    return Return::New(w, value);
}

Ast::Parameter& Ast::parameter(CellI& key)
{
    return Parameter::New(w, key);
}

Ast::Parameter& Ast::parameterDeclaration(CellI& key, CellI& type)
{
    auto& ret = Parameter::New(w, key);
    ret.set(id.type, type);

    return ret;
}

Ast::Parameter& Ast::parameterDeclaration(const std::string& keyStr, CellI& type)
{
    return parameterDeclaration(w.name(keyStr), type);
}

Ast::Parameter& Ast::parameterInit(CellI& key, CellI& value)
{
    auto& ret = Parameter::New(w, key);
    ret.set(id.value, value);

    return ret;
}

Ast::PrimitiveToolName& Ast::primitiveToolName(CellI& key)
{
    return PrimitiveToolName::New(w, key);
}

Ast::KVPair& Ast::kvPair(CellI& key, CellI& value)
{
    return KVPair::New(w, key, value);
}

Ast::KVPair& Ast::kvPair(const std::string& keyStr, CellI& value)
{
    return KVPair::New(w, w.name(keyStr), value);
}

Ast::KVPair& Ast::kvPair(CellI& key, const std::string& valueStr)
{
    return KVPair::New(w, key, w.name(valueStr));
}

Ast::KVPair& Ast::kvPair(const std::string& keyStr, const std::string& valueStr)
{
    return KVPair::New(w, w.name(keyStr), w.name(valueStr));
}

Ast::EnumValue& Ast::enumValue(const std::string& nameStr)
{
    return EnumValue::New(w, nameStr);
}

Ast::EnumValue& Ast::enumValue(const std::string& nameStr, CellI& init)
{
    return EnumValue::New(w, nameStr, init);
}

Ast::TypedEnumValue& Ast::typedEnumValue(const std::string& nameStr, CellI& type)
{
    return TypedEnumValue::New(w, nameStr, type);
}

Ast::TypedEnumValue& Ast::typedEnumValue(const std::string& nameStr, CellI& type, CellI& value)
{
    return TypedEnumValue::New(w, nameStr, type, value);
}

Ast::UnknownVar& Ast::unknown_(CellI& value)
{
    return UnknownVar::New(w, value);
}

Ast::Call& Ast::call(CellI& object, const std::string& method)
{
    return Call::New(w, object, w.ast._(method));
}

Ast::Call& Ast::call(CellI& method)
{
    return Call::New(w, method);
}

Ast::Call& Ast::call(CellI& cell, CellI& method)
{
    return Call::New(w, cell, method);
}

Ast::Call& Ast::delete_(Base& ast)
{
    return w.ast.call(ast, w.ast.primitiveToolName(std.op.Delete));
}

Ast::Call& Ast::set(Base& cell, Base& key, Base& value)
{
    return w.ast.call(cell, w.ast.primitiveToolName(std.op.Set))(id.key, key)(id.value, value);
}

Ast::Call& Ast::set(Base& cell, const std::string& key, Base& value)
{
    return set(cell, w.ast._(key), value);
}

Ast::Call& Ast::erase(Base& cell, Base& key)
{
    return w.ast.call(cell, w.ast.primitiveToolName(std.op.Erase))(id.key, key);
}

Ast::Call& Ast::erase(Base& cell, const std::string& key)
{
    return erase(cell, w.ast._(key));
}

Ast::If& Ast::if_(Base& condition)
{
    return If::New(w, condition);
}

Ast::Match& Ast::match_(Base& enum_)
{
    return Match::New(w, enum_);
}

Ast::Do& Ast::do_(Base& statement)
{
    return Do::New(w, statement);
}

Ast::While& Ast::while_(Base& condition)
{
    return While::New(w, condition);
}

Ast::For& Ast::for_(Base& var)
{
    return For::New(w, var);
}

Ast::For& Ast::for_(const std::string& varName)
{
    return For::New(w, var_(varName));
}

Ast::Var& Ast::var_(CellI& name)
{
    return Var::New(w, name);
}

Ast::Var& Ast::var_(const std::string& nameStr)
{
    return Var::New(w, nameStr);
}

Ast::Member& Ast::member(CellI& key)
{
    return Member::New(w, key);
}

Ast::Member& Ast::member(CellI& key, CellI& type)
{
    return Member::New(w, key, type);
}

Ast::Member& Ast::member(const std::string& nameStr, const std::string& typeStr)
{
    return member(name(nameStr), typeName(typeStr));
}

Ast::Member& Ast::member(const std::string& nameStr, CellI& type)
{
    return member(name(nameStr), type);
}

Ast::TypeAlias& Ast::typeAlias(CellI& key)
{
    return TypeAlias::New(w, key);
}

Ast::TemplatedType& Ast::templatedType(const std::string& idStr, CellI& type)
{
    CellI& ret = processNamespacedName(idStr, [this, &type](const std::string& outName)->CellI& {
        return TemplatedType::New(w, w.name(outName), w.list(type));
    });

    return static_cast<Ast::TemplatedType&>(ret);
}

Ast::TemplateParam& Ast::templateParam(CellI& key)
{
    return TemplateParam::New(w, key);
}

Ast::AssociatedType& Ast::associatedType(CellI& key)
{
    return AssociatedType::New(w, key);
}

Ast::New& Ast::new_(const std::string& typeStr)
{
    return New::NewT<Ast::New>::New(w, w.ast.typeName(typeStr));
}

Ast::New& Ast::new_(Base& objectType)
{
    return New::NewT<Ast::New>::New(w, objectType);
}

Ast::New& Ast::new_(Base& objectType, const std::string& constructor)
{
    return New::NewT<Ast::New>::New(w, objectType, w.ast._(constructor));
}

Ast::New& Ast::new_(Base& objectType, Base& constructor)
{
    return New::NewT<Ast::New>::New(w, objectType, constructor);
}

Ast::New& Ast::new_(const std::string& typeStr, const std::string& constructor)
{
    return New::NewT<Ast::New>::New(w, w.ast.typeName(typeStr), w.ast._(constructor));
}

Ast::Call& Ast::same(Base& lhs, Base& rhs)
{
    return w.ast.call(lhs, w.ast.primitiveToolName(std.op.Same))(id.other, rhs);
}

Ast::Call& Ast::notSame(Base& lhs, Base& rhs)
{
    return w.ast.call(lhs, w.ast.primitiveToolName(std.op.NotSame))(id.other, rhs);
}

Ast::Call& Ast::equal(CellI& lhs, Base& rhs)
{
    return w.ast.call(lhs, w.ast.primitiveToolName(std.op.Equal))(id.other, rhs);
}

Ast::Call& Ast::notEqual(Base& lhs, Base& rhs)
{
    return w.ast.call(lhs, w.ast.primitiveToolName(std.op.NotEqual))(id.other, rhs);
}

Ast::Call& Ast::has(Base& cell, Base& key)
{
    return w.ast.call(cell, w.ast.primitiveToolName(std.op.Has))(id.key, key);
}

Ast::Call& Ast::has(Base& cell, const std::string& key)
{
    return has(cell, w.ast._(key));
}

Ast::Call& Ast::missing(Base& cell, Base& key)
{
    return w.ast.call(cell, w.ast.primitiveToolName(std.op.Missing))(id.key, key);
}

Ast::Call& Ast::missing(Base& cell, const std::string& key)
{
    return missing(cell, w.ast._(key));
}

Ast::Call& Ast::get(Base& cell, const std::string& key)
{
    return get(cell, w.ast._(key));
}

Ast::Call& Ast::get(Base& cell, Base& key)
{
    return w.ast.call(cell, w.ast.primitiveToolName(std.op.Get))(id.key, key);
}

Ast::Call& Ast::and_(Base& lhs, Base& rhs)
{
    return w.ast.call(lhs, w.ast.primitiveToolName(std.op.And))(id.other, rhs);
}

Ast::Call& Ast::or_(Base& lhs, Base& rhs)
{
    return w.ast.call(lhs, w.ast.primitiveToolName(std.op.Or))(id.other, rhs);
}

Ast::Call& Ast::not_(Base& input)
{
    return w.ast.call(input, w.ast.primitiveToolName(std.op.Not));
}

Ast::Call& Ast::add(Base& lhs, Base& rhs)
{
    return w.ast.call(lhs, w.ast.primitiveToolName(std.op.Add))(id.other, rhs);
}

Ast::Call& Ast::subtract(Base& lhs, Base& rhs)
{
    return w.ast.call(lhs, w.ast.primitiveToolName(std.op.Subtract))(id.other, rhs);
}

Ast::Call& Ast::multiply(Base& lhs, Base& rhs)
{
    return w.ast.call(lhs, w.ast.primitiveToolName(std.op.Multiply))(id.other, rhs);
}

Ast::Call& Ast::divide(Base& lhs, Base& rhs)
{
    return w.ast.call(lhs, w.ast.primitiveToolName(std.op.Divide))(id.other, rhs);
}

Ast::Call& Ast::lessThan(Base& lhs, Base& rhs)
{
    return w.ast.call(lhs, w.ast.primitiveToolName(std.op.LessThan))(id.other, rhs);
}

Ast::Call& Ast::lessThanOrEqual(Base& lhs, Base& rhs)
{
    return w.ast.call(lhs, w.ast.primitiveToolName(std.op.LessThanOrEqual))(id.other, rhs);
}

Ast::Call& Ast::greaterThan(Base& lhs, Base& rhs)
{
    return w.ast.call(lhs, w.ast.primitiveToolName(std.op.GreaterThan))(id.other, rhs);
}

Ast::Call& Ast::greaterThanOrEqual(Base& lhs, Base& rhs)
{
    return w.ast.call(lhs, w.ast.primitiveToolName(std.op.GreaterThanOrEqual))(id.other, rhs);
}

void splitNamespacedString(std::vector<std::string>& out, const std::string& input)
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

CellI& Ast::processNamespacedName(const std::string& inputName, std::function<CellI&(const std::string& outName)> createCb)
{
    std::vector<std::string> sliced;
    splitNamespacedString(sliced, inputName);

    if (sliced.empty()) {
        throw "Invalid template ID!";
    }
    const auto& outName = sliced.back();
    auto& obj           = createCb(outName);
    if (sliced.size() > 1) {
        auto& namespaceList = *new List(w, std.Cell, "namespaces");
        obj.set(id.scopes, namespaceList);
        for (int i = 0; i < sliced.size() - 1; ++i) {
            const auto& currentId = sliced[i];
            namespaceList.add(w.name(currentId));
        }
    }

    return obj;
}

AstHelper::AstHelper(World& w) :
    Ast(w)
{
}

Ast::Parameter& AstHelper::p_(const std::string& nameStr)
{
    return Ast::parameter(name(nameStr));
}

Ast::Parameter& AstHelper::p_(const std::string& nameStr, CellI& value)
{
    return parameterInit(name(nameStr), value);
}

Ast::Parameter& AstHelper::parameter(const std::string& nameStr, CellI& type)
{
    return parameterDeclaration(name(nameStr), type);
}

Ast::Parameter& AstHelper::parameter(const std::string& nameStr, const std::string& typeStr)
{
    return parameterDeclaration(name(nameStr), __type__(typeStr));
}

Ast::Member& AstHelper::m_(const std::string& nameStr)
{
    return Ast::member(name(nameStr));
}

Ast::KVPair& AstHelper::typeAlias(const std::string& nameStr, const std::string& typeStr)
{
    return kvPair(name(nameStr), __type__(typeStr));
}

Ast::KVPair& AstHelper::typeAlias(const std::string& nameStr, CellI& type)
{
    return kvPair(name(nameStr), type);
}

Ast::EnumValue& AstHelper::ev_(const std::string& nameStr)
{
    return enumValue(nameStr);
}

Ast::EnumValue& AstHelper::ev_(const std::string& nameStr, CellI& value)
{
    return enumValue(nameStr, value);
}

Ast::TypedEnumValue& AstHelper::tev_(const std::string& nameStr, CellI& type)
{
    return typedEnumValue(nameStr, type);
}

Ast::TypedEnumValue& AstHelper::tev_(const std::string& nameStr, const std::string& typeStr)
{
    return typedEnumValue(nameStr, __type__(typeStr));
}

Ast::TypedEnumValue& AstHelper::tev_(const std::string& nameStr, CellI& type, CellI& value)
{
    return typedEnumValue(nameStr, type, value);
}

Ast::TypedEnumValue& AstHelper::tev_(const std::string& nameStr, CellI& type, const std::string& valueStr)
{
    return typedEnumValue(nameStr, type, id.emptyObject); // TODO, we need a value_("name") thing with an Ast:: ValueName type
}

Ast::TypedEnumValue& AstHelper::tev_(const std::string& nameStr, const std::string& typeStr, CellI& value)
{
    return typedEnumValue(nameStr, __type__(typeStr), value);
}

Ast::TypedEnumValue& AstHelper::tev_(const std::string& nameStr, const std::string& typeStr, const std::string& valueStr)
{
    return typedEnumValue(nameStr, __type__(typeStr), id.emptyObject); // TODO, we need a value_("name") thing with an Ast:: ValueName type
}

Ast::TemplateParam& AstHelper::tp_(const std::string& nameStr)
{
    return templateParam(name(nameStr));
}

Ast::AssociatedType& AstHelper::at_(const std::string& nameStr)
{
    return associatedType(name(nameStr));
}

Ast::TypeName& AstHelper::__type__(const std::string& nameStr)
{
    return typeName(nameStr);
}

CellI& AstHelper::ListOf(CellI& type)
{
    return w.ListOf(type);
}

CellI& AstHelper::MapOf(CellI& keyType, CellI& valueType)
{
    return w.MapOf(keyType, valueType);
}

} // namespace cells
} // namespace infocell
