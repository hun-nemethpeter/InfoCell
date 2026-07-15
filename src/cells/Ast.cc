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
    set("key", key);
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

Ast::Slot::Slot(World& w, CellI& key) :
    BaseT<Slot>(w, w.std.ast.Slot, "ast.Slot")
{
    set(w.id.key, key);
}

Ast::Slot::Slot(World& w, CellI& key, CellI& type) :
    BaseT<Slot>(w, w.std.ast.Slot, "std.Slot")
{
    set(w.id.key, key);
    set(w.id.type, type);
}

Ast::Call::Call(World& w, CellI& cell, CellI& method) :
    BaseT<Call>(w, w.std.ast.Call, "ast.call")
{
    set(w.id.cell, cell);
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

Ast::Call& Ast::Call::operator()(const std::string& nameStr, CellI& value)
{
    Slot& parameter = w.ast.parameterInit(w.name(nameStr), value);
    if (missing(w.id.parameters)) {
        set(w.id.parameters, w.list(parameter));
    } else {
        parameters().add(parameter);
    }
    return *this;
}

Ast::Call& Ast::Call::operator()(const std::string& nameStr, const std::string& valueStr)
{
    return operator()(nameStr, w._(valueStr));
}

List& Ast::Call::parameters()
{
    return static_cast<List&>(get(w.id.parameters));
}

Ast::StaticCall::StaticCall(World& w, CellI& cell, CellI& method) :
    BaseT<StaticCall>(w, w.std.ast.StaticCall, "ast.staticCall")
{
    set(w.id.cell, cell);
    set(w.id.method, method);
}

Ast::StaticCall& Ast::StaticCall::operator()(const std::string& nameStr, CellI& value)
{
    Slot& slot = Slot::New(w, w.name(nameStr), value);
    if (missing(w.id.parameters)) {
        set(w.id.parameters, w.list(slot));
    } else {
        List& paramList = static_cast<List&>(get(w.id.parameters));
        paramList.add(slot);
    }
    return *this;
}

Ast::Cell::Cell(World& w, CellI& value) :
    BaseT<Cell>(w, w.std.ast.Cell, "ast.cell")
{
    set(w.id.value, value);
}

Ast::Call& Ast::Cell::operator/(Base& key)
{
    return w.ast.get(*this, key);
}

Ast::Call& Ast::Cell::operator/(const std::string& key)
{
    return w.ast.get(*this, key);
}

Ast::TypeName::TypeName(World& w, CellI& name) :
    BaseT<TypeName>(w, w.std.ast.TypeName, "ast.typeName")
{
    set(w.id.name, name);
}

Ast::Self::Self(World& w) :
    BaseT<Self>(w, w.std.ast.Self, "ast.self")
{
}

Ast::Call& Ast::Self::operator()(const std::string& method)
{
    return w.ast.call(*this, method);
}

Ast::SelfType::SelfType(World& w) :
    BaseT<SelfType>(w, w.std.ast.SelfType, "ast.selfType")
{
}

Ast::Continue::Continue(World& w) :
    BaseT<Continue>(w, w.std.ast.Continue, "ast.continue")
{
}

Ast::Break::Break(World& w) :
    BaseT<Break>(w, w.std.ast.Break, "ast.break")
{
}

Ast::Try::Try(World& w, Base& tryBranch, Base& catchBranch) :
    BaseT<Try>(w, w.std.ast.Try, "ast.try")
{
    set("tryBranch", tryBranch);
    set("catchBranch", catchBranch);
}

Ast::Throw::Throw(World& w) :
    BaseT<Throw>(w, w.std.ast.Throw, "ast.throw")
{
}

Ast::Throw::Throw(World& w, Base& value) :
    BaseT<Throw>(w, w.std.ast.Throw, "ast.throw")
{
    set(w.id.value, value);
}

Ast::PrimitiveToolName::PrimitiveToolName(World& w, CellI& name) :
    BaseT<PrimitiveToolName>(w, w.std.ast.PrimitiveToolName, "ast.primitiveToolName")
{
    set(w.id.name, name);
}

Ast::Return::Return(World& w) :
    BaseT<Return>(w, w.std.ast.Return, "ast.return")
{
}

Ast::Return::Return(World& w, CellI& value) :
    BaseT<Return>(w, w.std.ast.Return, "ast.return")
{
    set(w.id.value, value);
}

Ast::Call& Ast::Return::operator()(const std::string& method)
{
    return w.ast.call(*this, method);
}

Ast::Block::Block(World& w, List& list) :
    BaseT<Block>(w, w.std.ast.Block, "ast.block")
{
    set(w.id.asts, list);
}

void Ast::Block::mergeFrom(Base& statement)
{
    List& toAstList = static_cast<List&>((*this)[w.id.asts]);
    if (&statement.__type__() == &w.std.ast.Block) {
        List& fromAstList = static_cast<List&>(statement[w.id.asts]);
        forEach(fromAstList, [&toAstList](CellI& ast, int, bool&) {
            toAstList.add(ast);
        });
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

    while (currentScope->has("scope")) {
        currentScope = &static_cast<Scope&>(currentScope->get("scope"));
    }

    return *currentScope;
}

Ast::Scope& Ast::Scope::createLink()
{
    Scope& ret = Scope::New(w, get(w.id.name).label());
    ret.set("link", *this);

    return ret;
}

Ast::Scope* Ast::Scope::getLinkedScope()
{
    if (missing("link")) {
        return nullptr;
    }
    return &static_cast<Scope&>(get("link"));
}

void Ast::Scope::mergeTo(Scope& targetScope, MergeMode mergeMode)
{
    Scope& from = *this;
    Scope& to   = targetScope;

    TrieMap& toScopeMap = to.items<Ast::Scope>();

    forEach(from.items<Ast::Scope>()[w.id.list], [this, &mergeMode, &toScopeMap](CellI& kvPair, int i, bool& stop) {
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
    });
}

Ast::StructBase::StructBase(World& w, CellI& astType, CellI& name, const std::string& nameStr) :
    Base(w, astType, nameStr)
{
    set("name", name);
}

Ast::Function& Ast::StructBase::addPrimitiveFunction(CellI& ast, CellI& op, const std::string& nameStr)
{
    Ast::Function& method = static_cast<Ast::Function&>(ast);
    method.label(nameStr);
    method.set("name", w.name(nameStr));
    method.set("primitiveTool", w.true_);
    List& fullyQualifiedName = *new List(w, w.std.Cell);
    fullyQualifiedName.add(op);
    method.set("fullyQualifiedName", fullyQualifiedName);
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
        Slot& selfSlot = w.ast.slot(w.id.self, *selfTypePtr);
        if (method.has(w.id.parameters)) {
            method.parameters().addFront(selfSlot);
        } else {
            method.parameters(selfSlot);
        }
    }

    if (missing("methods")) {
        set("methods", *new Map(w, w.std.Cell, w.std.ast.Function, "Map<Cell, Type::Ast::Function>(...)"));
    }
    if (methods().hasKey(name)) {
        throw "Already registered!";
    }
    method.set("structType", *this);
    methods().add(name, method);
}

Ast::TraitImpl& Ast::StructBase::addTraitImpl(const std::string& nameStr)
{
    Ast::TraitImpl& traitImpl = *new Ast::TraitImpl(w, nameStr);
    traitImpl.associatedTypes(w.ast.slot(w.id.self, w.ast.selfType()));
    addTraitImpl(traitImpl);

    return traitImpl;
}

void Ast::StructBase::addTraitImpl(Ast::TraitImpl& traitImpl)
{
    auto& name = traitImpl[w.id.name];

    if (missing("traitImpls")) {
        set("traitImpls", *new Map(w, w.std.Cell, w.std.ast.TraitImpl, "Map<Cell, Type::Ast::TraitImpl>(...)"));
    }
    if (traitImpls().hasKey(name)) {
        throw "Already registered!";
    }
    traitImpl.set("structType", *this);
    traitImpls().add(name, traitImpl);
}

Ast::StructBase& Ast::StructBase::returnType(CellI& type)
{
    if (missing("primitiveTool")) {
        throw "Setting return type on a node which is not a primitive tool!";
    }
    set("returnType", type);

    return *this;
}

Ast::StructBase& Ast::StructBase::members(Slot& slot)
{
    if (missing("members")) {
        set("members", *new Map(w, w.std.Cell, w.std.ast.Slot));
    }
    members().add(slot[w.id.key], slot);

    return *this;
}

Ast::StructBase& Ast::StructBase::typeAliases(Slot& slot)
{
    if (missing("typeAliases")) {
        set("typeAliases", *new Map(w, w.std.Cell, w.std.ast.Base));
    }
    CellI& key = slot[w.id.key];
    CellI& type = slot[w.id.type];

    typeAliases().add(key, slot);

    return *this;
}

Ast::StructBase& Ast::StructBase::memberOf(CellI& type)
{
    if (missing("memberOf")) {
        set("memberOf", *new List(w, w.std.Struct));
    }
    memberOf().add(type);

    return *this;
}

Map& Ast::StructBase::methods()
{
    if (missing("methods")) {
        throw "No methods!";
    } else {
        return static_cast<Map&>(get("methods"));
    }
}

Map& Ast::StructBase::traitImpls()
{
    if (missing("traitImpls")) {
        throw "No traitImpls!";
    } else {
        return static_cast<Map&>(get("traitImpls"));
    }
}

Map& Ast::StructBase::members()
{
    if (missing("members")) {
        throw "No members!";
    } else {
        return static_cast<Map&>(get("members"));
    }
}

Map& Ast::StructBase::typeAliases()
{
    if (missing("typeAliases")) {
        throw "No typeAliases!";
    } else {
        return static_cast<Map&>(get("typeAliases"));
    }
}

List& Ast::StructBase::memberOf()
{
    if (missing("memberOf")) {
        throw "No memberOf!";
    } else {
        return static_cast<List&>(get("memberOf"));
    }
}

CellI& Ast::StructBase::name()
{
    return get("name");
}

Ast::Base& Ast::StructBase::getTypeAlias(CellI& name)
{
    return static_cast<Ast::Base&>(typeAliases().getValue(name)[w.id.type]);
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

Ast::StructT& Ast::StructT::templateParams(Slot& slot)
{
    if (missing("templateParams")) {
        set("templateParams", *new Map(w, w.std.Cell, w.std.Struct));
    }
    CellI& key  = slot[w.id.key];
    CellI& type = slot[w.id.type];
    if (!(&type.__type__() == &w.std.ast.Cell || &type.__type__() == &w.std.ast.TemplatedType)) {
        throw "Invalid template param type!";
    }
    CellI* paramType = nullptr;
    if (&type.__type__() == &w.std.ast.Cell) {
        paramType = &type[w.id.value];
    } else {
        throw "TODO";
    }
    templateParams().add(key, *paramType);

    return *this;
}

Map& Ast::StructT::templateParams()
{
    if (missing("templateParams")) {
        throw "No templateParams!";
    } else {
        return static_cast<Map&>(get("templateParams"));
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

Ast::Trait& Ast::Trait::templateParams(Slot& slot)
{
    // TODO

    return *this;
}

Ast::Trait& Ast::Trait::associatedTypes(Slot& slot)
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

Ast::TraitImpl& Ast::TraitImpl::templateParams(Slot& slot)
{
    // TODO

    return *this;
}

Ast::TraitImpl& Ast::TraitImpl::associatedTypes(Slot& slot)
{
    if (missing("associatedTypes")) {
        set("associatedTypes", *new Map(w, w.std.Cell, w.std.ast.Base));
    }
    CellI& key  = slot[w.id.key];
    CellI& type = slot[w.id.type];

    associatedTypes().add(key, type);

    return *this;
}

Map& Ast::TraitImpl::associatedTypes()
{
    if (missing("associatedTypes")) {
        throw "No associatedTypes!";
    } else {
        return static_cast<Map&>(get("associatedTypes"));
    }
}

Ast::EnumValue::EnumValue(World& w, const std::string& name) :
    BaseT<EnumValue>(w, w.std.ast.EnumValue, name)
{
    set("name", w.name(name));
    label(name);
}

Ast::EnumValue::EnumValue(World& w, const std::string& name, CellI& value) :
    BaseT<EnumValue>(w, w.std.ast.EnumValue, name)
{
    set("name", w.name(name));
    set("value", value);
    label(name);
}

Ast::TypedEnumValue::TypedEnumValue(World& w, const std::string& nameStr, CellI& enumType) :
    BaseT<TypedEnumValue>(w, w.std.ast.TypedEnumValue, nameStr)
{
    set("name", w.name(nameStr));
    set("enumType", enumType);
    label(nameStr);
}

Ast::TypedEnumValue::TypedEnumValue(World& w, CellI& name, CellI& enumType) :
    BaseT<TypedEnumValue>(w, w.std.ast.TypedEnumValue, name.label())
{
    set("name", name);
    set("enumType", enumType);
    label(name.label());
}

Ast::TypedEnumValue::TypedEnumValue(World& w, const std::string& nameStr, CellI& enumType, CellI& value) :
    BaseT<TypedEnumValue>(w, w.std.ast.TypedEnumValue, nameStr)
{
    set("name", w.name(nameStr));
    set("value", value);
    set("enumType", enumType);
    label(nameStr);
}

Ast::Enum::Enum(World& w, CellI& name) :
    StructBase(w, w.std.ast.Enum, name, name.label())
{
    set("name", name);
    label(name.label());
}

Ast::Enum::Enum(World& w, const std::string& nameStr) :
    StructBase(w, w.std.ast.Enum, w.name(nameStr), nameStr)
{
    set("name", w.name(nameStr));
    label(nameStr);
}

Ast::Enum& Ast::Enum::values(Base& value)
{
    if (missing("values")) {
        set("values", *new Map(w, w.std.Cell, w.std.ast.Base));
    }
    values().add(value["name"], value);
    value.set("enum", *this);

    return *this;
}
CellI& Ast::Enum::resolveEnumValue(CellI& ast)
{
    if (&ast.__type__() == &w.std.ast.Cell) {
        return ast[w.id.value];
    }

    throw "Unknown enum value!";
}

TrieMap& Ast::Enum::values()
{
    if (missing("values")) {
        throw "No values!";
    } else {
        return static_cast<TrieMap&>(get("values"));
    }
}

Ast::Function::Function(World& w, CellI& name) :
    BaseT<Function>(w, w.std.ast.Function, name.label())
{
    set("name", name);
    label(name.label());
}

Ast::Function::Function(World& w, const std::string& nameStr) :
    BaseT<Function>(w, w.std.ast.Function, nameStr)
{
    set("name", w.name(nameStr));
    label(nameStr);
}

Ast::Function& Ast::Function::memberMapping(KVPair& mapping)
{
    if (missing(w.id.memberMapping)) {
        set(w.id.memberMapping, *new Map(w, w.std.String, w.std.ast.Base));
    }
    memberMapping().add(mapping[w.id.key], mapping[w.id.value]);

    return *this;
}

Ast::Function& Ast::Function::parameters(Slot& param)
{
    if (missing("parameters")) {
        set("parameters", *new List(w, w.std.ast.Slot));
    }
    parameters().add(param);

    return *this;
}

Ast::Function& Ast::Function::returnType(CellI& type)
{
    set("returnType", type);

    return *this;
}

Ast::Function& Ast::Function::returnType(const std::string& typeStr)
{
    return returnType(w.ast.typeName(typeStr));
}

void Ast::Function::addDescriptionBlock(Block& block)
{
    set(w.id.description, block);
}

void Ast::Function::addInstructionBlock(Block& block)
{
    set(w.id.instructions, block);
}

Map& Ast::Function::memberMapping()
{
    if (missing(w.id.memberMapping)) {
        throw "No member mapping!";
    } else {
        return static_cast<Map&>(get(w.id.memberMapping));
    }
}

List& Ast::Function::parameters()
{
    if (missing(w.id.parameters)) {
        throw "No parameters!";
    } else {
        return static_cast<List&>(get(w.id.parameters));
    }
}

CellI& Ast::Function::returnType()
{
    if (missing(w.id.returnType)) {
        throw "No returnType!";
    } else {
        return get(w.id.returnType);
    }
}

Ast::Base& Ast::Function::instructions()
{
    if (missing(w.id.instructions)) {
        throw "No instructions!";
    } else {
        return static_cast<Ast::Base&>(get(w.id.instructions));
    }
}

Ast::Base& Ast::Function::description()
{
    if (missing(w.id.description)) {
        throw "No description!";
    } else {
        return static_cast<Ast::Base&>(get(w.id.description));
    }
}

Ast::FunctionT::FunctionT(World& w, CellI& name, const std::string& nameStr) :
    BaseT<FunctionT>(w, w.std.ast.FunctionT, nameStr)
{
    set(w.id.name, name);
    label(nameStr);
}

Ast::FunctionT::FunctionT(World& w, const std::string& nameStr) :
    BaseT<FunctionT>(w, w.std.ast.FunctionT, nameStr)
{
    set(w.id.name, w.name(nameStr));
    label(nameStr);
}

void Ast::FunctionT::parameters(Slot& param)
{
    if (missing(w.id.parameters)) {
        set(w.id.parameters, *new List(w, w.std.ast.Slot));
    }
    parameters().add(param);
}

void Ast::FunctionT::returnType(CellI& type)
{
    set(w.id.returnType, type);
}

void Ast::FunctionT::addBlock(Block& block)
{
    set(w.id.instructions, block);
}

List& Ast::FunctionT::parameters()
{
    if (missing(w.id.parameters)) {
        throw "No parameters!";
    } else {
        return static_cast<List&>(get(w.id.parameters));
    }
}

CellI& Ast::FunctionT::returnType()
{
    if (missing(w.id.returnType)) {
        throw "No returnType!";
    } else {
        return get(w.id.returnType);
    }
}

Ast::Base& Ast::FunctionT::instructions()
{
    if (missing(w.id.instructions)) {
        throw "No instructions!";
    } else {
        return static_cast<Ast::Base&>(get(w.id.instructions));
    }
}

Ast::If::If(World& w, Base& condition) :
    BaseT<If>(w, w.std.ast.If, "ast.if")
{
    set(w.id.condition, condition);
}

Ast::If::If(World& w, Base& condition, Base& thenBranch) :
    BaseT<If>(w, w.std.ast.If, "ast.if")
{
    set(w.id.condition, condition);
    set(w.id.then, thenBranch);
}

Ast::If::If(World& w, Base& condition, Base& thenBranch, Base& elseBranch) :
    BaseT<If>(w, w.std.ast.If, "ast.ifElse")
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
    BaseT<Match>(w, w.std.ast.Match, "ast.match")
{
    set("enum", enum_);
}

Ast::Match& Ast::Match::case_(CellI& memberName, Base& op)
{
    if (missing("cases")) {
        set("cases", *new TrieMap(w, w.std.List, w.std.ast.Base));
    }
    auto& casesMap = static_cast<TrieMap&>(get("cases"));
    casesMap.add(memberName, op);

    return *this;
}

Ast::Match& Ast::Match::case_(const std::string& memberStr, Base& op)
{
    return case_(w.name(memberStr), op);
}

Ast::Do::Do(World& w, Base& statement) :
    BaseT<Do>(w, w.std.ast.Do, "ast.do")
{
    set(w.id.statement, statement);
}

Ast::Do& Ast::Do::while_(Base& condition)
{
    set(w.id.condition, condition);
    return *this;
}

Ast::While::While(World& w, Base& condition) :
    BaseT<While>(w, w.std.ast.While, "ast.while")
{
    set(w.id.condition, condition);
}

Ast::While& Ast::While::do_(Base& statement)
{
    set(w.id.statement, statement);
    return *this;
}

Ast::For::For(World& w, Base& var) :
    BaseT<For>(w, w.std.ast.For, "ast.for")
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
    return w.ast.set(*this, w._(w.id.value), value);
}

Ast::Call& Ast::Var::operator*()
{
    return w.ast.get(*this, w._(w.id.value));
}

Ast::Call& Ast::Var::operator()(const std::string& method)
{
    return w.ast.call(*(*this), method);
}

Ast::Member::Member(World& w, CellI& key) :
    BaseT<Member>(w, w.std.ast.Member, "ast.member")
{
    set(w.id.key, key);
}

Ast::Call& Ast::Member::operator=(Base& value)
{
    return w.ast.set(w.ast.self(), w._(get(w.id.key)), value);
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
    return w.ast.has(w.ast.self(), w._(get(w.id.key)));
}

Ast::Call& Ast::Member::missing()
{
    return w.ast.missing(w.ast.self(), Cell::New(w, get(w.id.key)));
}

Ast::Call& Ast::Member::operator()(const std::string& method)
{
    return w.ast.call(*this, method);
}

Ast::TypeAlias::TypeAlias(World& w, CellI& name) :
    BaseT<TypeAlias>(w, w.std.ast.TypeAlias, "ast.typeAlias")
{
    set(w.id.name, name);
}

Ast::TemplatedType::TemplatedType(World& w, CellI& id, CellI& typeList) :
    BaseT<TemplatedType>(w, w.std.ast.TemplatedType, "ast.templatedType")
{
    set(w.id.id, id);
    set(w.id.parameters, typeList);
}

void Ast::TemplatedType::addParam(const std::string& key, CellI& type)
{
    List& paramList = static_cast<List&>(get(w.id.parameters));
    paramList.add(w.ast.slot(key, type));
}

void Ast::TemplatedType::addParam(const std::string& key, const std::string& type)
{
    addParam(key, w.ast.typeName(type));
}

Ast::TemplateParam::TemplateParam(World& w, CellI& key) :
    BaseT<TemplateParam>(w, w.std.ast.TemplateParam, "ast.templateParam")
{
    set(w.id.key, key);
}

Ast::AssociatedType::AssociatedType(World& w, CellI& key) :
    BaseT<AssociatedType>(w, w.std.ast.AssociatedType, "ast.associatedType")
{
    set(w.id.key, key);
}

Ast::New::New(World& w, Base& objectType) :
    BaseT<New>(w, w.std.ast.New, "ast.new")
{
    set(w.id.objectType, objectType);
}

Ast::New::New(World& w, Base& objectType, Base& constructor) :
    BaseT<New>(w, w.std.ast.New, "ast.new()")
{
    set(w.id.objectType, objectType);
    set(w.id.constructor, constructor);
}

Ast::New& Ast::New::operator()(const std::string& nameStr, CellI& value)
{
    Slot& parameter = w.ast.parameterInit(w.name(nameStr), value);
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

Ast::Ast(World& w) :
    w(w)
{
}

Ast::Cell& Ast::cell(CellI& cell)
{
    return Cell::New(w, cell);
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

Ast::Continue& Ast::continue_()
{
    return Continue::New(w);
}

Ast::Break& Ast::break_()
{
    return Break::New(w);
}

Ast::Throw& Ast::throw_()
{
    return Throw::New(w);
}

Ast::Throw& Ast::throw_(Base& value)
{
    return Throw::New(w, value);
}

Ast::Try& Ast::try_(Base& tryBranch, Base& catchBranch)
{
    return Try::New(w, tryBranch, catchBranch);
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

Ast::Slot& Ast::parameterInit(CellI& key, CellI& value)
{
    auto& ret = Slot::New(w, key);
    ret.set(w.id.value, value);

    return ret;
}

Ast::PrimitiveToolName& Ast::primitiveToolName(CellI& key)
{
    return PrimitiveToolName::New(w, key);
}

Ast::Slot& Ast::slot(const std::string& key, CellI& type)
{
    return Slot::New(w, w.name(key), type);
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

Ast::Slot& Ast::slot(CellI& key, CellI& type)
{
    return Slot::New(w, key, type);
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

Ast::Call& Ast::call(CellI& object, const std::string& method)
{
    return Call::New(w, object, w._(method));
}

Ast::Call& Ast::call(CellI& cell, CellI& method)
{
    return Call::New(w, cell, method);
}

Ast::StaticCall& Ast::scall(CellI& cell, CellI& method)
{
    return StaticCall::New(w, cell, method);
}

Ast::StaticCall& Ast::scall(CellI& type, const std::string& method)
{
    return scall(type, w._(method));
}

Ast::Call& Ast::delete_(Base& ast)
{
    return w.ast.call(ast, w.ast.primitiveToolName(w.std.op.Delete));
}

Ast::Call& Ast::set(Base& cell, Base& key, Base& value)
{
    return w.ast.call(cell, w.ast.primitiveToolName(w.std.op.Set))("key", key)("value", value);
}

Ast::Call& Ast::set(Base& cell, const std::string& key, Base& value)
{
    return set(cell, w._(key), value);
}

Ast::Call& Ast::erase(Base& cell, Base& key)
{
    return w.ast.call(cell, w.ast.primitiveToolName(w.std.op.Erase))("key", key);
}

Ast::Call& Ast::erase(Base& cell, const std::string& key)
{
    return erase(cell, w._(key));
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
    return For::New(w, var(varName));
}

Ast::Var& Ast::var(CellI& name)
{
    return Var::New(w, name);
}

Ast::Var& Ast::var(const std::string& nameStr)
{
    return Var::New(w, nameStr);
}

Ast::Member& Ast::member(CellI& key)
{
    return Member::New(w, key);
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

Ast::New& Ast::new_(Base& objectType)
{
    return New::NewT<Ast::New>::New(w, objectType);
}

Ast::New& Ast::new_(Base& objectType, const std::string& constructor)
{
    return New::NewT<Ast::New>::New(w, objectType, w._(constructor));
}

Ast::New& Ast::new_(Base& objectType, Base& constructor)
{
    return New::NewT<Ast::New>::New(w, objectType, constructor);
}

Ast::New& Ast::new_(const std::string& objectType, const std::string& constructor)
{
    return New::NewT<Ast::New>::New(w, w.ast.typeName(w.name(objectType)), w._(constructor));
}

Ast::Call& Ast::same(Base& lhs, Base& rhs)
{
    return w.ast.call(lhs, w.ast.primitiveToolName(w.std.op.Same))("other", rhs);
}

Ast::Call& Ast::notSame(Base& lhs, Base& rhs)
{
    return w.ast.call(lhs, w.ast.primitiveToolName(w.std.op.NotSame))("other", rhs);
}

Ast::Call& Ast::equal(CellI& lhs, Base& rhs)
{
    return w.ast.call(lhs, w.ast.primitiveToolName(w.std.op.Equal))("other", rhs);
}

Ast::Call& Ast::notEqual(Base& lhs, Base& rhs)
{
    return w.ast.call(lhs, w.ast.primitiveToolName(w.std.op.NotEqual))("other", rhs);
}

Ast::Call& Ast::has(Base& cell, Base& key)
{
    return w.ast.call(cell, w.ast.primitiveToolName(w.std.op.Has))("key", key);
}

Ast::Call& Ast::has(Base& cell, const std::string& key)
{
    return has(cell, w._(key));
}

Ast::Call& Ast::missing(Base& cell, Base& key)
{
    return w.ast.call(cell, w.ast.primitiveToolName(w.std.op.Missing))("key", key);
}

Ast::Call& Ast::missing(Base& cell, const std::string& key)
{
    return missing(cell, w._(key));
}

Ast::Call& Ast::get(Base& cell, const std::string& key)
{
    return get(cell, w._(key));
}

Ast::Call& Ast::get(Base& cell, Base& key)
{
    return w.ast.call(cell, w.ast.primitiveToolName(w.std.op.Get))("key", key);
}

Ast::Call& Ast::and_(Base& lhs, Base& rhs)
{
    return w.ast.call(lhs, w.ast.primitiveToolName(w.std.op.And))("other", rhs);
}

Ast::Call& Ast::or_(Base& lhs, Base& rhs)
{
    return w.ast.call(lhs, w.ast.primitiveToolName(w.std.op.Or))("other", rhs);
}

Ast::Call& Ast::not_(Base& input)
{
    return w.ast.call(input, w.ast.primitiveToolName(w.std.op.Not));
}

Ast::Call& Ast::add(Base& lhs, Base& rhs)
{
    return w.ast.call(lhs, w.ast.primitiveToolName(w.std.op.Add))("other", rhs);
}

Ast::Call& Ast::subtract(Base& lhs, Base& rhs)
{
    return w.ast.call(lhs, w.ast.primitiveToolName(w.std.op.Subtract))("other", rhs);
}

Ast::Call& Ast::multiply(Base& lhs, Base& rhs)
{
    return w.ast.call(lhs, w.ast.primitiveToolName(w.std.op.Multiply))("other", rhs);
}

Ast::Call& Ast::divide(Base& lhs, Base& rhs)
{
    return w.ast.call(lhs, w.ast.primitiveToolName(w.std.op.Divide))("other", rhs);
}

Ast::Call& Ast::lessThan(Base& lhs, Base& rhs)
{
    return w.ast.call(lhs, w.ast.primitiveToolName(w.std.op.LessThan))("other", rhs);
}

Ast::Call& Ast::lessThanOrEqual(Base& lhs, Base& rhs)
{
    return w.ast.call(lhs, w.ast.primitiveToolName(w.std.op.LessThanOrEqual))("other", rhs);
}

Ast::Call& Ast::greaterThan(Base& lhs, Base& rhs)
{
    return w.ast.call(lhs, w.ast.primitiveToolName(w.std.op.GreaterThan))("other", rhs);
}

Ast::Call& Ast::greaterThanOrEqual(Base& lhs, Base& rhs)
{
    return w.ast.call(lhs, w.ast.primitiveToolName(w.std.op.GreaterThanOrEqual))("other", rhs);
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
        auto& namespaceList = *new List(w, w.std.Cell, "namespaces");
        obj.set("scopes", namespaceList);
        for (int i = 0; i < sliced.size() - 1; ++i) {
            const auto& currentId = sliced[i];
            namespaceList.add(w.name(currentId));
        }
    }

    return obj;
}

AstHelper::AstHelper(World& w) :
    Ast(w),
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

CellI& AstHelper::name(const std::string& str)
{
    return w.name(str);
}

Ast::Cell& AstHelper::_(CellI& cell)
{
    return this->cell(cell);
}

Ast::Cell& AstHelper::_(const std::string& nameStr)
{
    return cell(name(nameStr));
}

Ast::Cell& AstHelper::true_()
{
    return _(w.true_);
}

Ast::Cell& AstHelper::false_()
{
    return _(w.false_);
}

Ast::Parameter& AstHelper::p_(const std::string& nameStr)
{
    return Ast::parameter(name(nameStr));
}

Ast::Slot& AstHelper::p_(const std::string& nameStr, CellI& value)
{
    return parameter(nameStr, value);
}

Ast::Slot& AstHelper::parameter(const std::string& nameStr, CellI& value)
{
    return slot(name(nameStr), value);
}

Ast::Slot& AstHelper::parameter(const std::string& nameStr, const std::string& typeStr)
{
    return slot(name(nameStr), __type__(typeStr));
}

Ast::Member& AstHelper::m_(const std::string& nameStr)
{
    return Ast::member(name(nameStr));
}

Ast::Var& AstHelper::var_(const std::string& nameStr)
{
    return var(nameStr);
}

Ast::Slot& AstHelper::member(const std::string& nameStr, const std::string& typeStr)
{
    return slot(name(nameStr), __type__(typeStr));
}

Ast::Slot& AstHelper::member(const std::string& nameStr, CellI& type)
{
    return slot(name(nameStr), type);
}

Ast::Slot& AstHelper::typeAlias(const std::string& nameStr, const std::string& typeStr)
{
    return slot(name(nameStr), __type__(typeStr));
}

Ast::Slot& AstHelper::typeAlias(const std::string& nameStr, CellI& type)
{
    return slot(name(nameStr), type);
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
    return typedEnumValue(nameStr, type, w.id.emptyObject); // TODO, we need a value_("name") thing with an Ast:: ValueName type
}

Ast::TypedEnumValue& AstHelper::tev_(const std::string& nameStr, const std::string& typeStr, CellI& value)
{
    return typedEnumValue(nameStr, __type__(typeStr), value);
}

Ast::TypedEnumValue& AstHelper::tev_(const std::string& nameStr, const std::string& typeStr, const std::string& valueStr)
{
    return typedEnumValue(nameStr, __type__(typeStr), w.id.emptyObject); // TODO, we need a value_("name") thing with an Ast:: ValueName type
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
