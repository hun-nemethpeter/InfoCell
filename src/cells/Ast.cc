#include "Ast.h"

#include "Brain.h"

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include "util/Log.h"

namespace infocell {
namespace cells {

// ============================================================================
Ast::Base::Base(Brain& kb, CellI& classCell, const std::string& label) :
    Object(kb, classCell, label)
{
}

Ast::Parameter::Parameter(Brain& kb, CellI& key) :
    BaseT<Parameter>(kb, kb.std.ast.Parameter, key.label())
{
    set("key", key);
}

Ast::ResolvedType::ResolvedType(Brain& kb, CellI& astType, CellI& compiledType) :
    BaseT<ResolvedType>(kb, kb.std.ast.ResolvedType, astType.label())
{
    set(kb.ids.ast, astType);
    set(kb.ids.compiled, compiledType);
}

Ast::Get& Ast::Parameter::operator/(Base& key)
{
    return Get::New(kb, *this, key);
}

Ast::Get& Ast::Parameter::operator/(const std::string& key)
{
    return Get::New(kb, *this, kb._(key));
}

Ast::Call& Ast::Parameter::operator()(const std::string& method)
{
    return kb.ast.call(*this, method);
}

Ast::Slot::Slot(Brain& kb, CellI& key, CellI& type) :
    BaseT<Slot>(kb, kb.std.ast.Slot, "ast.slot")
{
    set(kb.ids.key, key);
    set(kb.ids.type, type);
}

Ast::Call::Call(Brain& kb, CellI& cell, CellI& method) :
    BaseT<Call>(kb, kb.std.ast.Call, "ast.call")
{
    set(kb.ids.cell, cell);
    set(kb.ids.method, method);
}

Ast::Call& Ast::Call::operator()(const std::string& nameStr, CellI& value)
{
    Slot& slot = Slot::New(kb, kb.name(nameStr), value);
    if (missing(kb.ids.parameters)) {
        set(kb.ids.parameters, kb.list(slot));
    } else {
        List& paramList = static_cast<List&>(get(kb.ids.parameters));
        paramList.add(slot);
    }
    return *this;
}

Ast::StaticCall::StaticCall(Brain& kb, CellI& cell, CellI& method) :
    BaseT<StaticCall>(kb, kb.std.ast.StaticCall, "ast.staticCall")
{
    set(kb.ids.cell, cell);
    set(kb.ids.method, method);
}

Ast::StaticCall& Ast::StaticCall::operator()(const std::string& nameStr, CellI& value)
{
    Slot& slot = Slot::New(kb, kb.name(nameStr), value);
    if (missing(kb.ids.parameters)) {
        set(kb.ids.parameters, kb.list(slot));
    } else {
        List& paramList = static_cast<List&>(get(kb.ids.parameters));
        paramList.add(slot);
    }
    return *this;
}

Ast::Cell::Cell(Brain& kb, CellI& value) :
    BaseT<Cell>(kb, kb.std.ast.Cell, "ast.cell")
{
    set(kb.ids.value, value);
}

Ast::Get& Ast::Cell::operator/(Base& key)
{
    return Get::New(kb, *this, key);
}

Ast::Get& Ast::Cell::operator/(const std::string& key)
{
    return Get::New(kb, *this, kb._(key));
}
Ast::StructName::StructName(Brain& kb, CellI& name) :
    BaseT<StructName>(kb, kb.std.ast.StructName, "ast.structName")
{
    set(kb.ids.name, name);
}

Ast::Self::Self(Brain& kb) :
    BaseT<Self>(kb, kb.std.ast.Self, "ast.self")
{
}

Ast::Call& Ast::Self::operator()(const std::string& method)
{
    return kb.ast.call(*this, method);
}

Ast::SelfFn::SelfFn(Brain& kb) :
    BaseT<SelfFn>(kb, kb.std.ast.SelfFn, "ast.selfFn")
{
}

Ast::Continue::Continue(Brain& kb) :
    BaseT<Continue>(kb, kb.std.ast.Continue, "ast.continue")
{
}

Ast::Break::Break(Brain& kb) :
    BaseT<Break>(kb, kb.std.ast.Break, "ast.break")
{
}

Ast::Try::Try(Brain& kb, Base& tryBranch, Base& catchBranch) :
    BaseT<Try>(kb, kb.std.ast.Try, "ast.try")
{
    set("tryBranch", tryBranch);
    set("catchBranch", catchBranch);
}

Ast::Throw::Throw(Brain& kb) :
    BaseT<Throw>(kb, kb.std.ast.Throw, "ast.throw")
{
}

Ast::Throw::Throw(Brain& kb, Base& value) :
    BaseT<Throw>(kb, kb.std.ast.Throw, "ast.throw")
{
    set(kb.ids.value, value);
}

Ast::Return::Return(Brain& kb) :
    BaseT<Return>(kb, kb.std.ast.Return, "ast.return")
{
}

Ast::Return::Return(Brain& kb, CellI& value) :
    BaseT<Return>(kb, kb.std.ast.Return, "ast.return")
{
    set(kb.ids.value, value);
}

Ast::Block::Block(Brain& kb, List& list) :
    BaseT<Block>(kb, kb.std.ast.Block, "ast.block")
{
    set(kb.ids.asts, list);
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
Ast::Items<TrieMap, Ast::TraitImpl>& Ast::Scope::getItemMember()
{
    return traitImplsImpl;
}

template <>
Ast::Items<TrieMap, Ast::Enum>& Ast::Scope::getItemMember()
{
    return enumsImpl;
}

Ast::Scope::Scope(Brain& kb, const std::string& nameStr) :
    BaseT<Scope>(kb, kb.std.ast.Scope, nameStr),
    scopesImpl(kb, "scopes", *this),
    functionsImpl(kb, "functions", *this),
    functionTsImpl(kb, "functionTs", *this),
    variablesImpl(kb, "variables", *this),
    structsImpl(kb, "structs", *this),
    structTsImpl(kb, "structTs", *this),
    traitsImpl(kb, "traits", *this),
    traitImplsImpl(kb, "traitImpls", *this),
    enumsImpl(kb, "enums", *this),
    earlyStructs(kb, kb.std.Cell, kb.std.Cell, "earlyStructs")
{
    set(kb.ids.name, kb.name(nameStr));
}

Ast::Scope& Ast::Scope::getRootScope()
{
    auto* currentScope = this;

    while (currentScope->has("scope")) {
        currentScope = &static_cast<Scope&>(currentScope->get("scope"));
    }

    return *currentScope;
}

Ast::StructBase::StructBase(Brain& kb, CellI& astType, CellI& name, const std::string& nameStr) :
    Base(kb, astType, nameStr),
    methodsImpl(kb, "methods", *this)

{
    set("name", name);
}

Ast::Function& Ast::StructBase::addMethod(const std::string& nameStr)
{
    Ast::Function& method = *new Ast::Function(kb, nameStr);
    addMethod(method);

    return method;
}

void Ast::StructBase::addMethod(Function& method)
{
    auto& name = method[kb.ids.name];

    if (missing("methods")) {
        set("methods", *new Map(kb, kb.std.Cell, kb.std.ast.Function, "Map<Cell, Type::Ast::Function>(...)"));
    }
    if (methods().hasKey(name)) {
        throw "Already registered!";
    }
    method.set("structType", *this);
    methods().add(name, method);
}

Ast::StructBase& Ast::StructBase::primitiveTool()
{
    if (missing("primitiveTool")) {
        set("primitiveTool", kb.boolean.true_);
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
        set("members", *new Map(kb, kb.std.Cell, kb.std.ast.Slot));
    }
    members().add(slot[kb.ids.key], slot);

    return *this;
}

Ast::StructBase& Ast::StructBase::subTypes(Slot& slot)
{
    if (missing("subTypes")) {
        set("subTypes", *new Map(kb, kb.std.Cell, kb.std.ast.Base));
    }
    CellI& key = slot[kb.ids.key];
    CellI& type = slot[kb.ids.type];

    subTypes().add(key, slot);

    return *this;
}

Ast::StructBase& Ast::StructBase::memberOf(CellI& type)
{
    if (missing("memberOf")) {
        set("memberOf", *new List(kb, kb.std.Struct));
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

Map& Ast::StructBase::members()
{
    if (missing("members")) {
        throw "No members!";
    } else {
        return static_cast<Map&>(get("members"));
    }
}

Map& Ast::StructBase::subTypes()
{
    if (missing("subTypes")) {
        throw "No subTypes!";
    } else {
        return static_cast<Map&>(get("subTypes"));
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

Ast::Base& Ast::StructBase::getSubType(CellI& name)
{
    return static_cast<Ast::Base&>(subTypes().getValue(name)[kb.ids.type]);
}

void Ast::StructBase::addBlock(Block& block)
{
    set(kb.ids.description, block);
}

Ast::Struct::Struct(Brain& kb, const std::string& nameStr) :
    StructBase(kb, kb.std.ast.Struct, kb.name(nameStr), nameStr)
{
}

Ast::Struct::Struct(Brain& kb, CellI& name) :
    StructBase(kb, kb.std.ast.Struct, name, name.label())
{
}

Ast::StructT::StructT(Brain& kb, CellI& name) :
    StructBase(kb, kb.std.ast.StructT, name, name.label())
{
}

Ast::StructT::StructT(Brain& kb, const std::string& nameStr) :
    StructBase(kb, kb.std.ast.StructT, kb.name(nameStr), nameStr)
{
}

Ast::StructT& Ast::StructT::templateParams(Slot& slot)
{
    if (missing("templateParams")) {
        set("templateParams", *new Map(kb, kb.std.Cell, kb.std.Struct));
    }
    CellI& key  = slot[kb.ids.key];
    CellI& type = slot[kb.ids.type];
    if (!(&type.struct_() == &kb.std.ast.Cell || &type.struct_() == &kb.std.ast.TemplatedType)) {
        throw "Invalid template param type!";
    }
    CellI* paramType = nullptr;
    if (&type.struct_() == &kb.std.ast.Cell) {
        paramType = &type[kb.ids.value];
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

Ast::Trait::Trait(Brain& kb, CellI& name) :
    StructBase(kb, kb.std.ast.Trait, name, name.label())
{
}

Ast::Trait::Trait(Brain& kb, const std::string& nameStr) :
    StructBase(kb, kb.std.ast.Trait, kb.name(nameStr), nameStr)
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


Ast::TraitImpl::TraitImpl(Brain& kb, CellI& name) :
    StructBase(kb, kb.std.ast.TraitImpl, name, name.label())
{
}

Ast::TraitImpl::TraitImpl(Brain& kb, const std::string& nameStr) :
    StructBase(kb, kb.std.ast.TraitImpl, kb.name(nameStr), nameStr)
{
}

Ast::TraitImpl& Ast::TraitImpl::templateParams(Slot& slot)
{
    // TODO

    return *this;
}

Ast::TraitImpl& Ast::TraitImpl::implementedFor(CellI& structType)
{
    // TODO

    return *this;
}

Ast::TraitImpl& Ast::TraitImpl::associatedTypes(Slot& slot)
{
    // TODO

    return *this;
}

Ast::EnumValue::EnumValue(Brain& kb, const std::string& name) :
    BaseT<EnumValue>(kb, kb.std.ast.EnumValue, name)
{
    set("name", kb.name(name));
    label(name);
}

Ast::EnumValue::EnumValue(Brain& kb, const std::string& name, CellI& value) :
    BaseT<EnumValue>(kb, kb.std.ast.EnumValue, name)
{
    set("name", kb.name(name));
    set("value", value);
    label(name);
}

Ast::TypedEnumValue::TypedEnumValue(Brain& kb, const std::string& nameStr, CellI& enumType) :
    BaseT<TypedEnumValue>(kb, kb.std.ast.TypedEnumValue, nameStr)
{
    set("name", kb.name(nameStr));
    set("enumType", enumType);
    label(nameStr);
}

Ast::TypedEnumValue::TypedEnumValue(Brain& kb, CellI& name, CellI& enumType) :
    BaseT<TypedEnumValue>(kb, kb.std.ast.TypedEnumValue, name.label())
{
    set("name", name);
    set("enumType", enumType);
    label(name.label());
}

Ast::TypedEnumValue::TypedEnumValue(Brain& kb, const std::string& nameStr, CellI& enumType, CellI& value) :
    BaseT<TypedEnumValue>(kb, kb.std.ast.TypedEnumValue, nameStr)
{
    set("name", kb.name(nameStr));
    set("value", value);
    set("enumType", enumType);
    label(nameStr);
}

Ast::Enum::Enum(Brain& kb, CellI& name) :
    BaseT<Enum>(kb, kb.std.ast.Enum, name.label())
{
    set("name", name);
    label(name.label());
}

Ast::Enum::Enum(Brain& kb, const std::string& nameStr) :
    BaseT<Enum>(kb, kb.std.ast.Enum, nameStr)
{
    set("name", kb.name(nameStr));
    label(nameStr);
}

Ast::Enum& Ast::Enum::values(Base& value)
{
    if (missing("values")) {
        set("values", *new Map(kb, kb.std.Cell, kb.std.ast.Base));
    }
    values().add(value["name"], value);
    value.set("enum", *this);

    return *this;
}
CellI& Ast::Enum::resolveEnumValue(CellI& ast)
{
    if (&ast.struct_() == &kb.std.ast.Cell) {
        return ast[kb.ids.value];
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

Ast::Function::Function(Brain& kb, CellI& name) :
    BaseT<Function>(kb, kb.std.ast.Function, name.label())
{
    set("name", name);
    label(name.label());
}

Ast::Function::Function(Brain& kb, const std::string& nameStr) :
    BaseT<Function>(kb, kb.std.ast.Function, nameStr)
{
    set("name", kb.name(nameStr));
    label(nameStr);
}

Ast::Function& Ast::Function::parameters(Slot& param)
{
    if (missing("parameters")) {
        set("parameters", *new List(kb, kb.std.Slot));
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
    set(kb.ids.instructions, block);
}

List& Ast::Function::parameters()
{
    if (missing(kb.ids.parameters)) {
        throw "No parameters!";
    } else {
        return static_cast<List&>(get(kb.ids.parameters));
    }
}

CellI& Ast::Function::returnType()
{
    if (missing(kb.ids.returnType)) {
        throw "No returnType!";
    } else {
        return get(kb.ids.returnType);
    }
}

Ast::Base& Ast::Function::instructions()
{
    if (missing(kb.ids.instructions)) {
        throw "No instructions!";
    } else {
        return static_cast<Ast::Base&>(get(kb.ids.instructions));
    }
}

Ast::FunctionT::FunctionT(Brain& kb, CellI& name, const std::string& nameStr) :
    BaseT<FunctionT>(kb, kb.std.ast.FunctionT, nameStr)
{
    set(kb.ids.name, name);
    label(nameStr);
}

Ast::FunctionT::FunctionT(Brain& kb, const std::string& nameStr) :
    BaseT<FunctionT>(kb, kb.std.ast.FunctionT, nameStr)
{
    set(kb.ids.name, kb.name(nameStr));
    label(nameStr);
}

void Ast::FunctionT::parameters(Slot& param)
{
    if (missing(kb.ids.parameters)) {
        set(kb.ids.parameters, *new List(kb, kb.std.Slot));
    }
    parameters().add(param);
}

void Ast::FunctionT::returnType(CellI& type)
{
    set(kb.ids.returnType, type);
}

void Ast::FunctionT::addBlock(Block& block)
{
    set(kb.ids.instructions, block);
}

List& Ast::FunctionT::parameters()
{
    if (missing(kb.ids.parameters)) {
        throw "No parameters!";
    } else {
        return static_cast<List&>(get(kb.ids.parameters));
    }
}

CellI& Ast::FunctionT::returnType()
{
    if (missing(kb.ids.returnType)) {
        throw "No returnType!";
    } else {
        return get(kb.ids.returnType);
    }
}

Ast::Base& Ast::FunctionT::instructions()
{
    if (missing(kb.ids.instructions)) {
        throw "No instructions!";
    } else {
        return static_cast<Ast::Base&>(get(kb.ids.instructions));
    }
}

Ast::Delete::Delete(Brain& kb, Base& cell) :
    BaseT<Delete>(kb, kb.std.ast.Delete, "ast.delete")
{
    set(kb.ids.cell, cell);
}

Ast::Set::Set(Brain& kb, Base& cell, Base& key, Base& value) :
    BaseT<Set>(kb, kb.std.ast.Set, "ast.set")
{
    set(kb.ids.cell, cell);
    set(kb.ids.key, key);
    set(kb.ids.value, value);
}

Ast::Erase::Erase(Brain& kb, Base& cell, Base& key) :
    BaseT<Erase>(kb, kb.std.ast.Erase, "ast.erase")
{
    set(kb.ids.cell, cell);
    set(kb.ids.key, key);
}

Ast::If::If(Brain& kb, Base& condition) :
    BaseT<If>(kb, kb.std.ast.If, "ast.if")
{
    set(kb.ids.condition, condition);
}

Ast::If::If(Brain& kb, Base& condition, Base& thenBranch) :
    BaseT<If>(kb, kb.std.ast.If, "ast.if")
{
    set(kb.ids.condition, condition);
    set(kb.ids.then, thenBranch);
}

Ast::If::If(Brain& kb, Base& condition, Base& thenBranch, Base& elseBranch) :
    BaseT<If>(kb, kb.std.ast.If, "ast.ifElse")
{
    set(kb.ids.condition, condition);
    set(kb.ids.then, thenBranch);
    set(kb.ids.else_, elseBranch);
}

Ast::If& Ast::If::then_(Base& thenBranch)
{
    set(kb.ids.then, thenBranch);
    return *this;
}

Ast::If& Ast::If::else_(Base& elseBranch)
{
    set(kb.ids.else_, elseBranch);
    return *this;
}

Ast::Match::Match(Brain& kb, Base& enum_) :
    BaseT<Match>(kb, kb.std.ast.Match, "ast.match")
{
    set("enum", enum_);
}

Ast::Match& Ast::Match::case_(CellI& memberName, Base& op)
{
    if (missing("cases")) {
        set("cases", *new TrieMap(kb, kb.std.List, kb.std.ast.Base));
    }
    auto& casesMap = static_cast<TrieMap&>(get("cases"));
    casesMap.add(memberName, op);

    return *this;
}

Ast::Match& Ast::Match::case_(const std::string& memberStr, Base& op)
{
    return case_(kb.name(memberStr), op);
}

Ast::Do::Do(Brain& kb, Base& statement) :
    BaseT<Do>(kb, kb.std.ast.Do, "ast.do")
{
    set(kb.ids.statement, statement);
}

Ast::Do& Ast::Do::while_(Base& condition)
{
    set(kb.ids.condition, condition);
    return *this;
}

Ast::While::While(Brain& kb, Base& condition) :
    BaseT<While>(kb, kb.std.ast.While, "ast.while")
{
    set(kb.ids.condition, condition);
}

Ast::While& Ast::While::do_(Base& statement)
{
    set(kb.ids.statement, statement);
    return *this;
}

Ast::For::For(Brain& kb, const std::string& varName) :
    BaseT<For>(kb, kb.std.ast.For, "ast.For")
{
    set(kb.ids.variable, kb.name(varName));
}

Ast::For& Ast::For::in(Base& container)
{
    set(kb.ids.container, container);
    return *this;
}

Ast::For& Ast::For::operator()(Base& statement)
{
    set(kb.ids.statement, statement);
    return *this;
}

Ast::Var::Var(Brain& kb, const std::string& nameStr) :
    BaseT<Var>(kb, kb.std.ast.Var, nameStr)
{
    set(kb.ids.name, kb.name(nameStr));
}

Ast::Var::Var(Brain& kb, CellI& name) :
    BaseT<Var>(kb, kb.std.ast.Var, name.label())
{
    set(kb.ids.name, name);
}

Ast::Set& Ast::Var::operator=(Base& value)
{
    return Set::New(kb, *this, Cell::New(kb, kb.ids.value), value);
}

Ast::Get& Ast::Var::operator*()
{
    return Get::New(kb, *this, Cell::New(kb, kb.ids.value));
}

Ast::Call& Ast::Var::operator()(const std::string& method)
{
    return kb.ast.call(*(*this), method);
}

Ast::Member::Member(Brain& kb, CellI& key) :
    BaseT<Member>(kb, kb.std.ast.Member, "ast.member")
{
    set(kb.ids.key, key);
}

Ast::Set& Ast::Member::operator=(Base& value)
{
    Ast::Set& ret = Set::New(kb, Self::New(kb), Cell::New(kb, get(kb.ids.key)), value);
    return ret;
}

Ast::Get& Ast::Member::operator/(Base& key)
{
    return Get::New(kb, *this, key);
}

Ast::Get& Ast::Member::operator/(const std::string& key)
{
    return Get::New(kb, *this, kb._(key));
}

Ast::Has& Ast::Member::exist()
{
    return Has::New(kb, Self::New(kb), Cell::New(kb, get(kb.ids.key)));
}

Ast::Missing& Ast::Member::missing()
{
    return Missing::New(kb, Self::New(kb), Cell::New(kb, get(kb.ids.key)));
}

Ast::Call& Ast::Member::operator()(const std::string& method)
{
    return kb.ast.call(*this, method);
}

Ast::SubType::SubType(Brain& kb, CellI& name) :
    BaseT<SubType>(kb, kb.std.ast.SubTypeName, "ast.subTypeName")
{
    set(kb.ids.name, name);
}

Ast::TemplatedType::TemplatedType(Brain& kb, CellI& id, CellI& typeList) :
    BaseT<TemplatedType>(kb, kb.std.ast.TemplatedType, "ast.templatedType")
{
    set(kb.ids.id, id);
    set(kb.ids.parameters, typeList);
}

void Ast::TemplatedType::addParam(const std::string& key, CellI& type)
{
    List& paramList = static_cast<List&>(get(kb.ids.parameters));
    paramList.add(kb.ast.slot(key, type));
}

void Ast::TemplatedType::addParam(const std::string& key, const std::string& type)
{
    addParam(key, kb.ast.structName(type));
}

Ast::TemplateParam::TemplateParam(Brain& kb, CellI& key) :
    BaseT<TemplateParam>(kb, kb.std.ast.TemplateParam, "ast.templateParam")
{
    set(kb.ids.key, key);
}

Ast::AssociatedType::AssociatedType(Brain& kb, CellI& key) :
    BaseT<AssociatedType>(kb, kb.std.ast.TemplateParam, "ast.associatedType")
{
    set(kb.ids.key, key);
}

Ast::New::New(Brain& kb, Base& objectType) :
    BaseT<New>(kb, kb.std.ast.New, "ast.new")
{
    set(kb.ids.objectType, objectType);
}

Ast::New::New(Brain& kb, Base& objectType, Base& constructor) :
    BaseT<New>(kb, kb.std.ast.New, "ast.new()")
{
    set(kb.ids.objectType, objectType);
    set(kb.ids.constructor, constructor);
}

Ast::New& Ast::New::operator()(const std::string& nameStr, CellI& value)
{
    Slot& slot = Slot::New(kb, kb.name(nameStr), value);
    if (missing(kb.ids.parameters)) {
        set(kb.ids.parameters, kb.list(slot));
    } else {
        List& paramList = static_cast<List&>(get(kb.ids.parameters));
        paramList.add(slot);
    }

    return *this;
}

Ast::Same::Same(Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Same>(kb, kb.std.ast.Same, "ast.same")
{
    set(kb.ids.lhs, lhs);
    set(kb.ids.rhs, rhs);
}

Ast::NotSame::NotSame(Brain& kb, Base& lhs, Base& rhs) :
    BaseT<NotSame>(kb, kb.std.ast.NotSame, "ast.notSame")
{
    set(kb.ids.lhs, lhs);
    set(kb.ids.rhs, rhs);
}

Ast::Equal::Equal(Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Equal>(kb, kb.std.ast.Equal, "ast.equal")
{
    set(kb.ids.lhs, lhs);
    set(kb.ids.rhs, rhs);
}

Ast::NotEqual::NotEqual(Brain& kb, Base& lhs, Base& rhs) :
    BaseT<NotEqual>(kb, kb.std.ast.NotEqual, "ast.notEqual")
{
    set(kb.ids.lhs, lhs);
    set(kb.ids.rhs, rhs);
}

Ast::Has::Has(Brain& kb, Base& cell, Base& key) :
    BaseT<Has>(kb, kb.std.ast.Has, "ast.has")
{
    set(kb.ids.cell, cell);
    set(kb.ids.key, key);
}

Ast::Missing::Missing(Brain& kb, Base& cell, Base& key) :
    BaseT<Missing>(kb, kb.std.ast.Missing, "ast.missing")
{
    set(kb.ids.cell, cell);
    set(kb.ids.key, key);
}

Ast::Get::Get(Brain& kb, Base& cell, Base& key) :
    BaseT<Get>(kb, kb.std.ast.Get, "ast.get")
{
    set(kb.ids.cell, cell);
    set(kb.ids.key, key);
}

Ast::Get& Ast::Get::operator/(Base& key)
{
    return Get::New(kb, *this, key);
}
Ast::Get& Ast::Get::operator/(const std::string& key)
{
    return Get::New(kb, *this, kb._(key));
}

Ast::Call& Ast::Get::operator()(const std::string& method)
{
    return kb.ast.call(*this, method);
}

Ast::And::And(Brain& kb, Base& lhs, Base& rhs) :
    BaseT<And>(kb, kb.std.ast.And, "ast.and")
{
    set(kb.ids.lhs, lhs);
    set(kb.ids.rhs, rhs);
}

Ast::Or::Or(Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Or>(kb, kb.std.ast.Or, "ast.or")
{
    set(kb.ids.lhs, lhs);
    set(kb.ids.rhs, rhs);
}

Ast::Not::Not(Brain& kb, Base& input) :
    BaseT<Not>(kb, kb.std.ast.Not, "ast.not")
{
    set(kb.ids.input, input);
}

Ast::Add::Add(Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Add>(kb, kb.std.ast.Add, "ast.add")
{
    set(kb.ids.lhs, lhs);
    set(kb.ids.rhs, rhs);
}

Ast::Subtract::Subtract(Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Subtract>(kb, kb.std.ast.Subtract, "ast.subtract")
{
    set(kb.ids.lhs, lhs);
    set(kb.ids.rhs, rhs);
}

Ast::Multiply::Multiply(Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Multiply>(kb, kb.std.ast.Multiply, "ast.multiply")
{
    set(kb.ids.lhs, lhs);
    set(kb.ids.rhs, rhs);
}

Ast::Divide::Divide(Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Divide>(kb, kb.std.ast.Divide, "ast.divide")
{
    set(kb.ids.lhs, lhs);
    set(kb.ids.rhs, rhs);
}

Ast::LessThan::LessThan(Brain& kb, Base& lhs, Base& rhs) :
    BaseT<LessThan>(kb, kb.std.ast.LessThan, "ast.lessThan")
{
    set(kb.ids.lhs, lhs);
    set(kb.ids.rhs, rhs);
}

Ast::LessThanOrEqual::LessThanOrEqual(Brain& kb, Base& lhs, Base& rhs) :
    BaseT<LessThanOrEqual>(kb, kb.std.ast.LessThanOrEqual, "ast.lessThanOrEqual")
{
    set(kb.ids.lhs, lhs);
    set(kb.ids.rhs, rhs);
}

Ast::GreaterThan::GreaterThan(Brain& kb, Base& lhs, Base& rhs) :
    BaseT<GreaterThan>(kb, kb.std.ast.GreaterThan, "ast.greaterThan")
{
    set(kb.ids.lhs, lhs);
    set(kb.ids.rhs, rhs);
}

Ast::GreaterThanOrEqual::GreaterThanOrEqual(Brain& kb, Base& lhs, Base& rhs) :
    BaseT<GreaterThanOrEqual>(kb, kb.std.ast.GreaterThanOrEqual, "ast.greaterThanOrEqual")
{
    set(kb.ids.lhs, lhs);
    set(kb.ids.rhs, rhs);
}

Ast::Ast(Brain& kb) :
    kb(kb)
{
}

Ast::Cell& Ast::cell(CellI& cell)
{
    return Cell::New(kb, cell);
}

Ast::StructName& Ast::structName(CellI& id)
{
    return StructName::New(kb, id);
}

Ast::StructName& Ast::structName(const std::string& idStr)
{
    CellI& ret = processNamespacedName(idStr, [this](const std::string& outName) -> CellI& {
        return StructName::New(kb, kb.name(outName));
    });

    return static_cast<Ast::StructName&>(ret);
}

Ast::Self& Ast::self()
{
    return Self::New(kb);
}

Ast::Continue& Ast::continue_()
{
    return Continue::New(kb);
}

Ast::Break& Ast::break_()
{
    return Break::New(kb);
}

Ast::Throw& Ast::throw_()
{
    return Throw::New(kb);
}

Ast::Throw& Ast::throw_(Base& value)
{
    return Throw::New(kb, value);
}

Ast::Try& Ast::try_(Base& tryBranch, Base& catchBranch)
{
    return Try::New(kb, tryBranch, catchBranch);
}

Ast::SelfFn& Ast::selfFn()
{
    return SelfFn::New(kb);
}

Ast::Return& Ast::return_()
{
    return Return::New(kb);
}

Ast::Return& Ast::return_(Base& value)
{
    return Return::New(kb, value);
}

Ast::Parameter& Ast::parameter(CellI& key)
{
    auto& ast = kb.ast;
    return Parameter::New(kb, key);
}

Ast::Slot& Ast::slot(const std::string& key, CellI& type)
{
    return Slot::New(kb, kb.name(key), type);
}

Ast::Slot& Ast::slot(CellI& key, CellI& type)
{
    return Slot::New(kb, key, type);
}

Ast::EnumValue& Ast::enumValue(const std::string& nameStr)
{
    return EnumValue::New(kb, nameStr);
}

Ast::EnumValue& Ast::enumValue(const std::string& nameStr, CellI& init)
{
    return EnumValue::New(kb, nameStr, init);
}

Ast::TypedEnumValue& Ast::typedEnumValue(const std::string& nameStr, CellI& type)
{
    return TypedEnumValue::New(kb, nameStr, type);
}

Ast::TypedEnumValue& Ast::typedEnumValue(const std::string& nameStr, CellI& type, CellI& value)
{
    return TypedEnumValue::New(kb, nameStr, type, value);
}

Ast::Call& Ast::call(CellI& object, const std::string& method)
{
    return Call::New(kb, object, kb.ast.cell(kb.name(method)));
}

Ast::Call& Ast::call(CellI& cell, CellI& method)
{
    return Call::New(kb, cell, method);
}

Ast::StaticCall& Ast::scall(CellI& cell, CellI& method)
{
    return StaticCall::New(kb, cell, method);
}

Ast::StaticCall& Ast::scall(CellI& type, const std::string& method)
{
    return scall(type, kb.ast.cell(kb.name(method)));
}

Ast::Delete& Ast::delete_(Base& ast)
{
    return Delete::New(kb, ast);
}

Ast::Set& Ast::set(Base& cell, Base& key, Base& value)
{
    return Set::New(kb, cell, key, value);
}

Ast::Set& Ast::set(Base& cell, const std::string& key, Base& value)
{
    return Set::New(kb, cell, kb._(key), value);
}

Ast::Erase& Ast::erase(Base& cell, Base& key)
{
    return Erase::New(kb, cell, key);
}

Ast::Erase& Ast::erase(Base& cell, const std::string& key)
{
    return Erase::New(kb, cell, kb._(key));
}

Ast::If& Ast::if_(Base& condition)
{
    return If::New(kb, condition);
}

Ast::Match& Ast::match_(Base& enum_)
{
    return Match::New(kb, enum_);
}

Ast::Do& Ast::do_(Base& statement)
{
    return Do::New(kb, statement);
}

Ast::While& Ast::while_(Base& condition)
{
    return While::New(kb, condition);
}

Ast::For& Ast::for_(const std::string& varName)
{
    return For::New(kb, varName);
}

Ast::Var& Ast::var(CellI& name)
{
    return Var::New(kb, name);
}

Ast::Var& Ast::var(const std::string& nameStr)
{
    return Var::New(kb, nameStr);
}

Ast::Member& Ast::member(CellI& key)
{
    return Member::New(kb, key);
}

Ast::SubType& Ast::subType(CellI& key)
{
    return SubType::New(kb, key);
}

Ast::TemplatedType& Ast::templatedType(const std::string& idStr, CellI& type)
{
    CellI& ret = processNamespacedName(idStr, [this, &type](const std::string& outName)->CellI& {
        return TemplatedType::New(kb, kb.name(outName), kb.list(type));
    });

    return static_cast<Ast::TemplatedType&>(ret);
}

Ast::TemplateParam& Ast::templateParam(CellI& key)
{
    return TemplateParam::New(kb, key);
}

Ast::AssociatedType& Ast::associatedType(CellI& key)
{
    return AssociatedType::New(kb, key);
}

Ast::New& Ast::new_(Base& objectType)
{
    return New::NewT<Ast::New>::New(kb, objectType);
}

Ast::New& Ast::new_(Base& objectType, const std::string& constructor)
{
    return New::NewT<Ast::New>::New(kb, objectType, kb.ast.cell(kb.name(constructor)));
}

Ast::New& Ast::new_(Base& objectType, Base& constructor)
{
    return New::NewT<Ast::New>::New(kb, objectType, constructor);
}

Ast::New& Ast::new_(const std::string& objectType, const std::string& constructor)
{
    return New::NewT<Ast::New>::New(kb, kb.ast.structName(kb.name(objectType)), kb.ast.cell(kb.name(constructor)));
}

Ast::Same& Ast::same(Base& lhs, Base& rhs)
{
    return Same::New(kb, lhs, rhs);
}

Ast::NotSame& Ast::notSame(Base& lhs, Base& rhs)
{
    return NotSame::New(kb, lhs, rhs);
}

Ast::Equal& Ast::equal(Base& lhs, Base& rhs)
{
    return Equal::New(kb, lhs, rhs);
}

Ast::NotEqual& Ast::notEqual(Base& lhs, Base& rhs)
{
    return NotEqual::New(kb, lhs, rhs);
}

Ast::Has& Ast::has(Base& cell, Base& key)
{
    return Has::New(kb, cell, key);
}

Ast::Has& Ast::has(Base& cell, const std::string& key)
{
    return Has::New(kb, cell, kb._(key));
}

Ast::Missing& Ast::missing(Base& cell, Base& key)
{
    return Missing::New(kb, cell, key);
}

Ast::Missing& Ast::missing(Base& cell, const std::string& key)
{
    return Missing::New(kb, cell, kb._(key));
}

Ast::Get& Ast::get(Base& cell, const std::string& key)
{
    return Get::New(kb, cell, kb._(key));
}

Ast::Get& Ast::get(Base& cell, Base& key)
{
    return Get::New(kb, cell, key);
}

Ast::And& Ast::and_(Base& lhs, Base& rhs)
{
    return And::New(kb, lhs, rhs);
}

Ast::Or& Ast::or_(Base& lhs, Base& rhs)
{
    return Or::New(kb, lhs, rhs);
}

Ast::Not& Ast::not_(Base& input)
{
    return Not::New(kb, input);
}

Ast::Add& Ast::add(Base& lhs, Base& rhs)
{
    return Add::New(kb, lhs, rhs);
}

Ast::Subtract& Ast::subtract(Base& lhs, Base& rhs)
{
    return Subtract::New(kb, lhs, rhs);
}

Ast::Multiply& Ast::multiply(Base& lhs, Base& rhs)
{
    return Multiply::New(kb, lhs, rhs);
}

Ast::Divide& Ast::divide(Base& lhs, Base& rhs)
{
    return Divide::New(kb, lhs, rhs);
}

Ast::LessThan& Ast::lessThan(Base& lhs, Base& rhs)
{
    return LessThan::New(kb, lhs, rhs);
}

Ast::LessThanOrEqual& Ast::lessThanOrEqual(Base& lhs, Base& rhs)
{
    return LessThanOrEqual::New(kb, lhs, rhs);
}

Ast::GreaterThan& Ast::greaterThan(Base& lhs, Base& rhs)
{
    return GreaterThan::New(kb, lhs, rhs);
}

Ast::GreaterThanOrEqual& Ast::greaterThanOrEqual(Base& lhs, Base& rhs)
{
    return GreaterThanOrEqual::New(kb, lhs, rhs);
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
        auto& namespaceList = *new List(kb, kb.std.Cell, "namespaces");
        obj.set("scopes", namespaceList);
        for (int i = 0; i < sliced.size() - 1; ++i) {
            const auto& currentId = sliced[i];
            namespaceList.add(kb.name(currentId));
        }
    }

    return obj;
}

AstHelper::AstHelper(Brain& kb) :
    Ast(kb),
    globalScope(kb.globalScope),
    ids(kb.ids),
    std(kb.std),
    directions(kb.directions),
    coordinates(kb.coordinates),
    _0_(kb._0_),
    _1_(kb._1_),
    _2_(kb._2_),
    _3_(kb._3_),
    _4_(kb._4_),
    _5_(kb._5_),
    _6_(kb._6_),
    _7_(kb._7_),
    _8_(kb._8_),
    _9_(kb._9_)
{
}

CellI& AstHelper::name(const std::string& str)
{
    return kb.name(str);
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
    return _(kb.boolean.true_);
}

Ast::Cell& AstHelper::false_()
{
    return _(kb.boolean.false_);
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

Ast::Member& AstHelper::m_(const std::string& nameStr)
{
    return Ast::member(name(nameStr));
}

Ast::Var& AstHelper::var_(const std::string& nameStr)
{
    return var(nameStr);
}

Ast::Slot& AstHelper::member(const std::string& nameStr, const std::string& type)
{
    return slot(name(nameStr), struct_(type));
}

Ast::Slot& AstHelper::member(const std::string& nameStr, CellI& type)
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
    return typedEnumValue(nameStr, struct_(typeStr));
}

Ast::TypedEnumValue& AstHelper::tev_(const std::string& nameStr, CellI& type, CellI& value)
{
    return typedEnumValue(nameStr, type, value);
}

Ast::TypedEnumValue& AstHelper::tev_(const std::string& nameStr, CellI& type, const std::string& valueStr)
{
    return typedEnumValue(nameStr, type, kb.ids.emptyObject); // TODO, we need a value_("name") thing with an Ast:: ValueName type
}

Ast::TypedEnumValue& AstHelper::tev_(const std::string& nameStr, const std::string& typeStr, CellI& value)
{
    return typedEnumValue(nameStr, struct_(typeStr), value);
}

Ast::TypedEnumValue& AstHelper::tev_(const std::string& nameStr, const std::string& typeStr, const std::string& valueStr)
{
    return typedEnumValue(nameStr, struct_(typeStr), kb.ids.emptyObject); // TODO, we need a value_("name") thing with an Ast:: ValueName type
}

Ast::TemplateParam& AstHelper::tp_(const std::string& nameStr)
{
    return templateParam(name(nameStr));
}

Ast::AssociatedType& AstHelper::at_(const std::string& nameStr)
{
    return associatedType(name(nameStr));
}

Ast::StructName& AstHelper::struct_(const std::string& nameStr)
{
    return structName(nameStr);
}

CellI& AstHelper::ListOf(CellI& type)
{
    return kb.ListOf(type);
}

CellI& AstHelper::MapOf(CellI& keyType, CellI& valueType)
{
    return kb.MapOf(keyType, valueType);
}

} // namespace cells
} // namespace infocell
