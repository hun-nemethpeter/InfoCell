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

Ast::Get& Ast::Parameter::operator/(Base& key)
{
    return Get::New(w, *this, key);
}

Ast::Get& Ast::Parameter::operator/(const std::string& key)
{
    return Get::New(w, *this, w._(key));
}

Ast::Call& Ast::Parameter::operator()(const std::string& method)
{
    return w.ast.call(*this, method);
}

Ast::Slot::Slot(World& w, CellI& key, CellI& type) :
    BaseT<Slot>(w, w.std.ast.Slot, "ast.slot")
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

Ast::Call& Ast::Call::operator()(const std::string& nameStr, CellI& value)
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

Ast::Get& Ast::Cell::operator/(Base& key)
{
    return Get::New(w, *this, key);
}

Ast::Get& Ast::Cell::operator/(const std::string& key)
{
    return Get::New(w, *this, w._(key));
}
Ast::StructName::StructName(World& w, CellI& name) :
    BaseT<StructName>(w, w.std.ast.StructName, "ast.structName")
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

Ast::SelfFn::SelfFn(World& w) :
    BaseT<SelfFn>(w, w.std.ast.SelfFn, "ast.selfFn")
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

Ast::Return::Return(World& w) :
    BaseT<Return>(w, w.std.ast.Return, "ast.return")
{
}

Ast::Return::Return(World& w, CellI& value) :
    BaseT<Return>(w, w.std.ast.Return, "ast.return")
{
    set(w.id.value, value);
}

Ast::Block::Block(World& w, List& list) :
    BaseT<Block>(w, w.std.ast.Block, "ast.block")
{
    set(w.id.asts, list);
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

    Visitor::visitList(from.items<Ast::Scope>()[w.id.list], [this, &mergeMode, &toScopeMap](CellI& kvPair, int i, bool& stop) {
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

Ast::Function& Ast::StructBase::addMethod(const std::string& nameStr)
{
    Ast::Function& method = *new Ast::Function(w, nameStr);
    addMethod(method);

    return method;
}

void Ast::StructBase::addMethod(Function& method)
{
    auto& name = method[w.id.name];

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

Ast::StructBase& Ast::StructBase::primitiveTool()
{
    if (missing("primitiveTool")) {
        set("primitiveTool", w.true_);
    }

    return *this;
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

void Ast::StructBase::addBlock(Block& block)
{
    set(w.id.description, block);
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

Ast::Function& Ast::Function::parameters(Slot& param)
{
    if (missing("parameters")) {
        set("parameters", *new List(w, w.std.Slot));
    }
    parameters().add(param);

    return *this;
}

Ast::Function& Ast::Function::returnType(CellI& type)
{
    set("returnType", type);

    return *this;
}

void Ast::Function::addBlock(Block& block)
{
    set(w.id.instructions, block);
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
        set(w.id.parameters, *new List(w, w.std.Slot));
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

Ast::Delete::Delete(World& w, Base& cell) :
    BaseT<Delete>(w, w.std.ast.Delete, "ast.delete")
{
    set(w.id.cell, cell);
}

Ast::Set::Set(World& w, Base& cell, Base& key, Base& value) :
    BaseT<Set>(w, w.std.ast.Set, "ast.set")
{
    set(w.id.cell, cell);
    set(w.id.key, key);
    set(w.id.value, value);
}

Ast::Erase::Erase(World& w, Base& cell, Base& key) :
    BaseT<Erase>(w, w.std.ast.Erase, "ast.erase")
{
    set(w.id.cell, cell);
    set(w.id.key, key);
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

Ast::For::For(World& w, const std::string& varName) :
    BaseT<For>(w, w.std.ast.For, "ast.For")
{
    set(w.id.variable, w.name(varName));
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

Ast::Set& Ast::Var::operator=(Base& value)
{
    return Set::New(w, *this, Cell::New(w, w.id.value), value);
}

Ast::Get& Ast::Var::operator*()
{
    return Get::New(w, *this, Cell::New(w, w.id.value));
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

Ast::Set& Ast::Member::operator=(Base& value)
{
    Ast::Set& ret = Set::New(w, Self::New(w), Cell::New(w, get(w.id.key)), value);
    return ret;
}

Ast::Get& Ast::Member::operator/(Base& key)
{
    return Get::New(w, *this, key);
}

Ast::Get& Ast::Member::operator/(const std::string& key)
{
    return Get::New(w, *this, w._(key));
}

Ast::Has& Ast::Member::exist()
{
    return Has::New(w, Self::New(w), Cell::New(w, get(w.id.key)));
}

Ast::Missing& Ast::Member::missing()
{
    return Missing::New(w, Self::New(w), Cell::New(w, get(w.id.key)));
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
    addParam(key, w.ast.structName(type));
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
    Slot& slot = Slot::New(w, w.name(nameStr), value);
    if (missing(w.id.parameters)) {
        set(w.id.parameters, w.list(slot));
    } else {
        List& paramList = static_cast<List&>(get(w.id.parameters));
        paramList.add(slot);
    }

    return *this;
}

Ast::Same::Same(World& w, Base& lhs, Base& rhs) :
    BaseT<Same>(w, w.std.ast.Same, "ast.same")
{
    set(w.id.lhs, lhs);
    set(w.id.rhs, rhs);
}

Ast::NotSame::NotSame(World& w, Base& lhs, Base& rhs) :
    BaseT<NotSame>(w, w.std.ast.NotSame, "ast.notSame")
{
    set(w.id.lhs, lhs);
    set(w.id.rhs, rhs);
}

Ast::Equal::Equal(World& w, Base& lhs, Base& rhs) :
    BaseT<Equal>(w, w.std.ast.Equal, "ast.equal")
{
    set(w.id.lhs, lhs);
    set(w.id.rhs, rhs);
}

Ast::NotEqual::NotEqual(World& w, Base& lhs, Base& rhs) :
    BaseT<NotEqual>(w, w.std.ast.NotEqual, "ast.notEqual")
{
    set(w.id.lhs, lhs);
    set(w.id.rhs, rhs);
}

Ast::Has::Has(World& w, Base& cell, Base& key) :
    BaseT<Has>(w, w.std.ast.Has, "ast.has")
{
    set(w.id.cell, cell);
    set(w.id.key, key);
}

Ast::Missing::Missing(World& w, Base& cell, Base& key) :
    BaseT<Missing>(w, w.std.ast.Missing, "ast.missing")
{
    set(w.id.cell, cell);
    set(w.id.key, key);
}

Ast::Get::Get(World& w, Base& cell, Base& key) :
    BaseT<Get>(w, w.std.ast.Get, "ast.get")
{
    set(w.id.cell, cell);
    set(w.id.key, key);
}

Ast::Get& Ast::Get::operator/(Base& key)
{
    return Get::New(w, *this, key);
}
Ast::Get& Ast::Get::operator/(const std::string& key)
{
    return Get::New(w, *this, w._(key));
}

Ast::Call& Ast::Get::operator()(const std::string& method)
{
    return w.ast.call(*this, method);
}

Ast::And::And(World& w, Base& lhs, Base& rhs) :
    BaseT<And>(w, w.std.ast.And, "ast.and")
{
    set(w.id.lhs, lhs);
    set(w.id.rhs, rhs);
}

Ast::Or::Or(World& w, Base& lhs, Base& rhs) :
    BaseT<Or>(w, w.std.ast.Or, "ast.or")
{
    set(w.id.lhs, lhs);
    set(w.id.rhs, rhs);
}

Ast::Not::Not(World& w, Base& input) :
    BaseT<Not>(w, w.std.ast.Not, "ast.not")
{
    set(w.id.input, input);
}

Ast::Add::Add(World& w, Base& lhs, Base& rhs) :
    BaseT<Add>(w, w.std.ast.Add, "ast.add")
{
    set(w.id.lhs, lhs);
    set(w.id.rhs, rhs);
}

Ast::Subtract::Subtract(World& w, Base& lhs, Base& rhs) :
    BaseT<Subtract>(w, w.std.ast.Subtract, "ast.subtract")
{
    set(w.id.lhs, lhs);
    set(w.id.rhs, rhs);
}

Ast::Multiply::Multiply(World& w, Base& lhs, Base& rhs) :
    BaseT<Multiply>(w, w.std.ast.Multiply, "ast.multiply")
{
    set(w.id.lhs, lhs);
    set(w.id.rhs, rhs);
}

Ast::Divide::Divide(World& w, Base& lhs, Base& rhs) :
    BaseT<Divide>(w, w.std.ast.Divide, "ast.divide")
{
    set(w.id.lhs, lhs);
    set(w.id.rhs, rhs);
}

Ast::LessThan::LessThan(World& w, Base& lhs, Base& rhs) :
    BaseT<LessThan>(w, w.std.ast.LessThan, "ast.lessThan")
{
    set(w.id.lhs, lhs);
    set(w.id.rhs, rhs);
}

Ast::LessThanOrEqual::LessThanOrEqual(World& w, Base& lhs, Base& rhs) :
    BaseT<LessThanOrEqual>(w, w.std.ast.LessThanOrEqual, "ast.lessThanOrEqual")
{
    set(w.id.lhs, lhs);
    set(w.id.rhs, rhs);
}

Ast::GreaterThan::GreaterThan(World& w, Base& lhs, Base& rhs) :
    BaseT<GreaterThan>(w, w.std.ast.GreaterThan, "ast.greaterThan")
{
    set(w.id.lhs, lhs);
    set(w.id.rhs, rhs);
}

Ast::GreaterThanOrEqual::GreaterThanOrEqual(World& w, Base& lhs, Base& rhs) :
    BaseT<GreaterThanOrEqual>(w, w.std.ast.GreaterThanOrEqual, "ast.greaterThanOrEqual")
{
    set(w.id.lhs, lhs);
    set(w.id.rhs, rhs);
}

Ast::Ast(World& w) :
    w(w)
{
}

Ast::Cell& Ast::cell(CellI& cell)
{
    return Cell::New(w, cell);
}

Ast::StructName& Ast::structName(CellI& id)
{
    return StructName::New(w, id);
}

Ast::StructName& Ast::structName(const std::string& idStr)
{
    CellI& ret = processNamespacedName(idStr, [this](const std::string& outName) -> CellI& {
        return StructName::New(w, w.name(outName));
    });

    return static_cast<Ast::StructName&>(ret);
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

Ast::SelfFn& Ast::selfFn()
{
    return SelfFn::New(w);
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
    auto& ast = w.ast;
    return Parameter::New(w, key);
}

Ast::Slot& Ast::slot(const std::string& key, CellI& type)
{
    return Slot::New(w, w.name(key), type);
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
    return Call::New(w, object, w.ast.cell(w.name(method)));
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
    return scall(type, w.ast.cell(w.name(method)));
}

Ast::Delete& Ast::delete_(Base& ast)
{
    return Delete::New(w, ast);
}

Ast::Set& Ast::set(Base& cell, Base& key, Base& value)
{
    return Set::New(w, cell, key, value);
}

Ast::Set& Ast::set(Base& cell, const std::string& key, Base& value)
{
    return Set::New(w, cell, w._(key), value);
}

Ast::Erase& Ast::erase(Base& cell, Base& key)
{
    return Erase::New(w, cell, key);
}

Ast::Erase& Ast::erase(Base& cell, const std::string& key)
{
    return Erase::New(w, cell, w._(key));
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

Ast::For& Ast::for_(const std::string& varName)
{
    return For::New(w, varName);
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
    return New::NewT<Ast::New>::New(w, objectType, w.ast.cell(w.name(constructor)));
}

Ast::New& Ast::new_(Base& objectType, Base& constructor)
{
    return New::NewT<Ast::New>::New(w, objectType, constructor);
}

Ast::New& Ast::new_(const std::string& objectType, const std::string& constructor)
{
    return New::NewT<Ast::New>::New(w, w.ast.structName(w.name(objectType)), w.ast.cell(w.name(constructor)));
}

Ast::Same& Ast::same(Base& lhs, Base& rhs)
{
    return Same::New(w, lhs, rhs);
}

Ast::NotSame& Ast::notSame(Base& lhs, Base& rhs)
{
    return NotSame::New(w, lhs, rhs);
}

Ast::Equal& Ast::equal(Base& lhs, Base& rhs)
{
    return Equal::New(w, lhs, rhs);
}

Ast::NotEqual& Ast::notEqual(Base& lhs, Base& rhs)
{
    return NotEqual::New(w, lhs, rhs);
}

Ast::Has& Ast::has(Base& cell, Base& key)
{
    return Has::New(w, cell, key);
}

Ast::Has& Ast::has(Base& cell, const std::string& key)
{
    return Has::New(w, cell, w._(key));
}

Ast::Missing& Ast::missing(Base& cell, Base& key)
{
    return Missing::New(w, cell, key);
}

Ast::Missing& Ast::missing(Base& cell, const std::string& key)
{
    return Missing::New(w, cell, w._(key));
}

Ast::Get& Ast::get(Base& cell, const std::string& key)
{
    return Get::New(w, cell, w._(key));
}

Ast::Get& Ast::get(Base& cell, Base& key)
{
    return Get::New(w, cell, key);
}

Ast::And& Ast::and_(Base& lhs, Base& rhs)
{
    return And::New(w, lhs, rhs);
}

Ast::Or& Ast::or_(Base& lhs, Base& rhs)
{
    return Or::New(w, lhs, rhs);
}

Ast::Not& Ast::not_(Base& input)
{
    return Not::New(w, input);
}

Ast::Add& Ast::add(Base& lhs, Base& rhs)
{
    return Add::New(w, lhs, rhs);
}

Ast::Subtract& Ast::subtract(Base& lhs, Base& rhs)
{
    return Subtract::New(w, lhs, rhs);
}

Ast::Multiply& Ast::multiply(Base& lhs, Base& rhs)
{
    return Multiply::New(w, lhs, rhs);
}

Ast::Divide& Ast::divide(Base& lhs, Base& rhs)
{
    return Divide::New(w, lhs, rhs);
}

Ast::LessThan& Ast::lessThan(Base& lhs, Base& rhs)
{
    return LessThan::New(w, lhs, rhs);
}

Ast::LessThanOrEqual& Ast::lessThanOrEqual(Base& lhs, Base& rhs)
{
    return LessThanOrEqual::New(w, lhs, rhs);
}

Ast::GreaterThan& Ast::greaterThan(Base& lhs, Base& rhs)
{
    return GreaterThan::New(w, lhs, rhs);
}

Ast::GreaterThanOrEqual& Ast::greaterThanOrEqual(Base& lhs, Base& rhs)
{
    return GreaterThanOrEqual::New(w, lhs, rhs);
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

Ast::StructName& AstHelper::__type__(const std::string& nameStr)
{
    return structName(nameStr);
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
