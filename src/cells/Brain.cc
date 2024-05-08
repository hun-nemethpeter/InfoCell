#include "Brain.h"

#include <boost/algorithm/string/regex.hpp>
#include <format>
#include <sstream>

namespace synth {
namespace cells {
namespace brain {

ID::ID(brain::Brain& kb) :
    kb(kb),
    argument(kb, kb.std.Char, "argument"),
    ast(kb, kb.std.Char, "ast"),
    asts(kb, kb.std.Char, "asts"),
    blue(kb, kb.std.Char, "blue"),
    break_(kb, kb.std.Char, "break_"),
    cell(kb, kb.std.Char, "cell"),
    children(kb, kb.std.Char, "children"),
    code(kb, kb.std.Char, "code"),
    color(kb, kb.std.Char, "color"),
    compiled(kb, kb.std.Char, "compiled"),
    condition(kb, kb.std.Char, "condition"),
    constructor(kb, kb.std.Char, "constructor"),
    continue_(kb, kb.std.Char, "continue_"),
    currentFn(kb, kb.std.Char, "currentFn"),
    currentStruct(kb, kb.std.Char, "currentStruct"),
    data(kb, kb.std.Char, "data"),
    destructor(kb, kb.std.Char, "destructor"),
    else_(kb, kb.std.Char, "else_"),
    emptyObject(kb, kb.std.Char, "emptyObject"),
    first(kb, kb.std.Char, "first"),
    functions(kb, kb.std.Char, "functions"),
    globalScope(kb, kb.std.Char, "globalScope"),
    green(kb, kb.std.Char, "green"),
    height(kb, kb.std.Char, "height"),
    id(kb, kb.std.Char, "id"),
    index(kb, kb.std.Char, "index"),
    input(kb, kb.std.Char, "input"),
    instances(kb, kb.std.Char, "instances"),
    item(kb, kb.std.Char, "item"),
    itemType(kb, kb.std.Char, "itemType"),
    key(kb, kb.std.Char, "key"),
    keyType(kb, kb.std.Char, "keyType"),
    last(kb, kb.std.Char, "last"),
    lhs(kb, kb.std.Char, "lhs"),
    list(kb, kb.std.Char, "list"),
    listType(kb, kb.std.Char, "listType"),
    localVars(kb, kb.std.Char, "localVars"),
    memberOf(kb, kb.std.Char, "memberOf"),
    members(kb, kb.std.Char, "members"),
    method(kb, kb.std.Char, "method"),
    methods(kb, kb.std.Char, "methods"),
    name(kb, kb.std.Char, "name"),
    next(kb, kb.std.Char, "next"),
    objectType(kb, kb.std.Char, "objectType"),
    op(kb, kb.std.Char, "op"),
    ops(kb, kb.std.Char, "ops"),
    output(kb, kb.std.Char, "output"),
    parameters(kb, kb.std.Char, "parameters"),
    parent(kb, kb.std.Char, "parent"),
    pixels(kb, kb.std.Char, "pixels"),
    previous(kb, kb.std.Char, "previous"),
    process(kb, kb.std.Char, "process"),
    red(kb, kb.std.Char, "red"),
    resolvedScope(kb, kb.std.Char, "resolvedScope"),
    result(kb, kb.std.Char, "result"),
    return_(kb, kb.std.Char, "return_"),
    returnType(kb, kb.std.Char, "returnType"),
    rhs(kb, kb.std.Char, "rhs"),
    role(kb, kb.std.Char, "role"),
    rootNode(kb, kb.std.Char, "rootNode"),
    scope(kb, kb.std.Char, "scope"),
    scopes(kb, kb.std.Char, "scopes"),
    self(kb, kb.std.Char, "self"),
    size(kb, kb.std.Char, "size"),
    slotRole(kb, kb.std.Char, "slotRole"),
    slots(kb, kb.std.Char, "slots"),
    slotType(kb, kb.std.Char, "slotType"),
    stack(kb, kb.std.Char, "stack"),
    statement(kb, kb.std.Char, "statement"),
    static_(kb, kb.std.Char, "static_"),
    status(kb, kb.std.Char, "status"),
    structs(kb, kb.std.Char, "structs"),
    structType(kb, kb.std.Char, "structType"),
    subTypes(kb, kb.std.Char, "subTypes"),
    templateId(kb, kb.std.Char, "templateId"),
    templateParams(kb, kb.std.Char, "templateParams"),
    then(kb, kb.std.Char, "then"),
    throw_(kb, kb.std.Char, "throw"),
    type(kb, kb.std.Char, "type"),
    unknownInstances(kb, kb.std.Char, "unknownInstances"),
    unknownStructs(kb, kb.std.Char, "unknownStructs"),
    value(kb, kb.std.Char, "value"),
    valueType(kb, kb.std.Char, "valueType"),
    variables(kb, kb.std.Char, "variables"),
    width(kb, kb.std.Char, "width")
{
}

namespace type {

Op::Op(brain::Brain& kb) :
    kb(kb),
    Add(kb, kb.std.Type_, "op::Add"),
    And(kb, kb.std.Type_, "op::And"),
    Base(kb, kb.std.Type_, "op::Base"),
    Block(kb, kb.std.Type_, "op::Block"),
    ConstVar(kb, kb.std.Type_, "op::ConstVar"),
    Delete(kb, kb.std.Type_, "op::Delete"),
    Divide(kb, kb.std.Type_, "op::Divide"),
    Do(kb, kb.std.Type_, "op::Do"),
    Equal(kb, kb.std.Type_, "op::Equal"),
    Erase(kb, kb.std.Type_, "op::Erase"),
    EvalVar(kb, kb.std.Type_, "op::EvalVar"),
    Function(kb, kb.std.Type_, "op::Function"),
    Get(kb, kb.std.Type_, "op::Get"),
    GreaterThan(kb, kb.std.Type_, "op::GreaterThan"),
    GreaterThanOrEqual(kb, kb.std.Type_, "op::GreaterThanOrEqual"),
    Has(kb, kb.std.Type_, "op::Has"),
    If(kb, kb.std.Type_, "op::If"),
    LessThan(kb, kb.std.Type_, "op::LessThan"),
    LessThanOrEqual(kb, kb.std.Type_, "op::LessThanOrEqual"),
    Missing(kb, kb.std.Type_, "op::Missing"),
    Multiply(kb, kb.std.Type_, "op::Multiply"),
    New(kb, kb.std.Type_, "op::New"),
    Not(kb, kb.std.Type_, "op::Not"),
    NotEqual(kb, kb.std.Type_, "op::NotEqual"),
    NotSame(kb, kb.std.Type_, "op::NotSame"),
    Or(kb, kb.std.Type_, "op::Or"),
    Return(kb, kb.std.Type_, "op::Return"),
    Same(kb, kb.std.Type_, "op::Same"),
    Set(kb, kb.std.Type_, "op::Set"),
    Subtract(kb, kb.std.Type_, "op::Subtract"),
    Var(kb, kb.std.Type_, "op::Var"),
    While(kb, kb.std.Type_, "op::While")
{
}

Ast::Ast(brain::Brain& kb) :
    kb(kb),
    Add(kb, kb.std.Type_, "ast::Add"),
    And(kb, kb.std.Type_, "ast::And"),
    Base(kb, kb.std.Type_, "ast::Base"),
    Block(kb, kb.std.Type_, "ast::Block"),
    Break(kb, kb.std.Type_, "ast::Break"),
    Call(kb, kb.std.Type_, "ast::Call"),
    Cell(kb, kb.std.Type_, "ast::Cell"),
    Continue(kb, kb.std.Type_, "ast::Continue"),
    Delete(kb, kb.std.Type_, "ast::Delete"),
    Divide(kb, kb.std.Type_, "ast::Divide"),
    Do(kb, kb.std.Type_, "ast::Do"),
    Equal(kb, kb.std.Type_, "ast::Equal"),
    Erase(kb, kb.std.Type_, "ast::Erase"),
    Function(kb, kb.std.Type_, "ast::Function"),
    FunctionT(kb, kb.std.Type_, "ast::FunctionT"),
    Get(kb, kb.std.Type_, "ast::Get"),
    GreaterThan(kb, kb.std.Type_, "ast::GreaterThan"),
    GreaterThanOrEqual(kb, kb.std.Type_, "ast::GreaterThanOrEqual"),
    Has(kb, kb.std.Type_, "ast::Has"),
    If(kb, kb.std.Type_, "ast::If"),
    LessThan(kb, kb.std.Type_, "ast::LessThan"),
    LessThanOrEqual(kb, kb.std.Type_, "ast::LessThanOrEqual"),
    Member(kb, kb.std.Type_, "ast::Member"),
    Missing(kb, kb.std.Type_, "ast::Missing"),
    Multiply(kb, kb.std.Type_, "ast::Multiply"),
    New(kb, kb.std.Type_, "ast::New"),
    Not(kb, kb.std.Type_, "ast::Not"),
    NotEqual(kb, kb.std.Type_, "ast::NotEqual"),
    NotSame(kb, kb.std.Type_, "ast::NotSame"),
    Or(kb, kb.std.Type_, "ast::Or"),
    Parameter(kb, kb.std.Type_, "ast::Parameter"),
    ResolvedType(kb, kb.std.Type_, "ast::ResolvedType"),
    Return(kb, kb.std.Type_, "ast::Return"),
    Same(kb, kb.std.Type_, "ast::Same"),
    Scope(kb, kb.std.Type_, "ast::Scope"),
    Self(kb, kb.std.Type_, "ast::Self"),
    SelfFn(kb, kb.std.Type_, "ast::SelfFn"),
    Set(kb, kb.std.Type_, "ast::Set"),
    Slot(kb, kb.std.Type_, "ast::Slot"),
    StaticCall(kb, kb.std.Type_, "ast::StaticCall"),
    Struct(kb, kb.std.Type_, "ast::Struct"),
    StructName(kb, kb.std.Type_, "ast::StructName"),
    StructT(kb, kb.std.Type_, "ast::StructT"),
    SubTypeName(kb, kb.std.Type_, "ast::SubTypeName"),
    Subtract(kb, kb.std.Type_, "ast::Subtract"),
    TemplatedType(kb, kb.std.Type_, "ast::TemplatedType"),
    TemplateParam(kb, kb.std.Type_, "ast::TemplateParam"),
    Throw(kb, kb.std.Type_, "ast::Throw"),
    Try(kb, kb.std.Type_, "ast::Try"),
    Var(kb, kb.std.Type_, "ast::Var"),
    While(kb, kb.std.Type_, "ast::While")
{
}

} // namespace type

Std::Std(brain::Brain& kb) :
    kb(kb),
    Cell(kb, kb.std.Type_, "Cell"),
    Slot(kb, kb.std.Type_, "Slot"),
    Type_(kb, kb.std.Type_, "Type"),
    Enum(kb, kb.std.Type_, "Enum"),
    Container(kb, kb.std.Type_, "Conatainer"),
    List(kb, kb.std.Type_, "List"),
    ListItem(kb, kb.std.Type_, "ListItem"),
    KVPair(kb, kb.std.Type_, "KVPair"),
    Map(kb, kb.std.Type_, "Map"),
    Index(kb, kb.std.Type_, "Index"),
    TrieMap(kb, kb.std.Type_, "TrieMap"),
    TrieMapNode(kb, kb.std.Type_, "TrieMapNode"),
    Boolean(kb, kb.std.Type_, "Boolean"),
    Char(kb, kb.std.Type_, "Char"),
    Digit(kb, kb.std.Type_, "Digit"),
    Number(kb, kb.std.Type_, "Number"),
    String(kb, kb.std.Type_, "String"),
    Color(kb, kb.std.Type_, "Color"),
    Pixel(kb, kb.std.Type_, "Pixel"),
    Picture(kb, kb.std.Type_, "Picture"),
    Stack(kb, kb.std.Type_, "Stack"),
    StackFrame(kb, kb.std.Type_, "StackFrame"),
    Program(kb, kb.std.Type_, "Program"),
    ProgramData(kb, kb.std.Type_, "ProgramData"),
    StructReference(kb, kb.std.Type_, "StructReference"),
    CompileState(kb, kb.std.Type_, "CompileState"),
    Directions(kb, kb.std.Enum, "Directions"),
    op(kb),
    ast(kb)
{
}

cells::CellI& Std::slot(cells::CellI& role, cells::CellI& type)
{
    CellI& ret = *new Object(kb, kb.std.Slot);
    ret.set("slotRole", role);
    ret.set("slotType", type);

    return ret;
}

cells::CellI& Std::slot(const std::string& role, cells::CellI& type)
{
    CellI& ret = *new Object(kb, kb.std.Slot);
    ret.set("slotRole", kb.id(role));
    ret.set("slotType", type);

    return ret;
}

cells::CellI& Std::kvPair(cells::CellI& key, cells::CellI& value)
{
    CellI& ret = *new Object(kb, kb.std.KVPair);
    ret.set("key", key);
    ret.set("value", value);

    return ret;
}

// ============================================================================
Ast::Base::Base(brain::Brain& kb, CellI& classCell, const std::string& label) :
    Object(kb, classCell, label)
{
}

Ast::Base& Ast::Base::resolveType(CellI& typeAst, CellI& resolveState)
{
    if (&typeAst.type() == &kb.std.ast.Cell) {
        auto& cell      = static_cast<Ast::Cell&>(typeAst);
        auto& cellValue = typeAst[kb.ids.value];
        if (!cellValue.isA(kb.std.Type_) && !cellValue.isA(kb.std.Enum)) {
            throw "Type AST referencing a non-type!";
        }
        return cell;
    }
    if (&typeAst.type() == &kb.std.ast.ResolvedType) {
        return static_cast<Ast::ResolvedType&>(typeAst);
    }
    if (&typeAst.type() == &kb.std.ast.StructName) {
        auto& resolveAstStruct      = resolveStructNameAsAst(typeAst, resolveState);
        auto& resolveCompiledStruct = resolveStructName(resolveAstStruct.getFullId(), resolveState);
        auto& reslvedTypeNode       = resolvedType(resolveAstStruct, resolveCompiledStruct);

        return reslvedTypeNode;
    }
    if (&typeAst.type() == &kb.std.ast.SubTypeName) {
        auto& resolveAstStruct = resolveSubTypeNameAsAst(typeAst, resolveState);

        return resolveAstStruct;
    }
    if (&typeAst.type() == &kb.std.ast.TemplatedType) {
        return resolveTemplatedType(typeAst, resolveState);
    }

    throw "Unknown ast type node!";
}

CellI& Ast::Base::getCompiledTypeFromResolvedType(CellI& resolvedTypeAst)
{
    if (resolvedTypeAst.isA(kb.std.ast.Cell)) {
        return resolvedTypeAst[kb.ids.value];
    } else if (resolvedTypeAst.isA(kb.std.ast.ResolvedType)) {
        return resolvedTypeAst[kb.ids.compiled];
    } else {
        throw "Unexpected AST type!";
    }
}

CellI& Ast::Base::getResolvedTypeById(CellI& id, bool isInstance, CellI& resolveState)
{
    if (isInstance) {
        auto& instances = static_cast<TrieMap&>(resolveState[kb.ids.instances]);
        if (instances.hasKey(id)) {
            return instances.getValue(id);
        } else {
            auto& unknownInstances = static_cast<TrieMap&>(resolveState[kb.ids.unknownInstances]);
            return unknownInstances.getValue(id)[kb.ids.value];
        }
    } else {
        auto& structs = static_cast<TrieMap&>(resolveState[kb.ids.structs]);
        return structs.getValue(id);
    }
}

CellI& Ast::Base::resolveId(CellI& structId, CellI& containerId, CellI& unknownContainerId, CellI& resolveState, std::function<CellI&(CellI& structReference)> unknownCb)
{
    auto& container = static_cast<TrieMap&>(resolveState[containerId]);

    if (container.hasKey(structId)) {
        return container.getValue(structId);
    } else {
        auto& unresolvedContainer = static_cast<TrieMap&>(resolveState[unknownContainerId]);
        if (unresolvedContainer.hasKey(structId)) {
            return unresolvedContainer.getValue(structId)[kb.ids.value];
        } else {

            auto& structReference = *new Object(kb, kb.std.StructReference);
            auto& unresolvedStruct = unknownCb(structReference);
            structReference.set("value", unresolvedStruct);
            structReference.set("id", structId);
            structReference.set("scope", resolveState[kb.ids.scope]);
            structReference.set("resolvedScope", resolveState[kb.ids.resolvedScope]);

            if (resolveState.has("currentFn")) {
                structReference.set("currentFn", resolveState[kb.ids.currentFn]);
            }
            if (resolveState.has("currentStruct")) {
                structReference.set("currentStruct", resolveState[kb.ids.currentStruct]);
            }
            unresolvedContainer.add(structId, structReference);

            return unresolvedStruct;
        }
    }
}

CellI& Ast::Base::resolveTemplateInstanceId(CellI& structId, CellI& idScope, CellI& resolveState, CellI& ast, CellI& templateParams)
{
    auto& templateId = ast[kb.ids.id];
    return resolveId(structId, kb.id("instances"), kb.id("unknownInstances"), resolveState, [this, &resolveState, &templateId, &structId ,&templateParams, &ast, &idScope](CellI& structReference) -> CellI& {
        structReference.set("templateId", templateId);
        structReference.set("templateParams", templateParams);
        if (ast.has("scopes")) {
            structReference.set(kb.id("idScope"), idScope);
        }
        auto& unresolvedStruct = *new Object(kb, kb.std.Type_, std::format("{}", structId.label()));
        unresolvedStruct.set("incomplete", kb.boolean.true_);

        return unresolvedStruct;
    });
}


Ast::Struct& Ast::Base::resolveTemplateInstanceIdAsAst(CellI& structId, CellI& idScope, CellI& resolveState, CellI& ast, CellI& templateParams)
{
    auto& templateId = ast[kb.ids.id];
    auto& ret = resolveId(structId, kb.id("instanceAsts"), kb.id("unknownInstanceAsts"), resolveState, [this, &resolveState, &templateId, &structId, &templateParams, &ast, &idScope](CellI& structReference) -> CellI& {
        structReference.set("templateId", templateId);
        structReference.set("templateParams", templateParams);
        if (ast.has("scopes")) {
            structReference.set(kb.id("idScope"), idScope);
        }
        auto& unresolvedStruct = *new Ast::Struct(kb, structId, std::format("{}", structId.label()));

        return unresolvedStruct;
    });
    if (ret.missing("scope")) {
        ret.set("scope", idScope);
    }

    return static_cast<Ast::Struct&>(ret);
}

CellI& Ast::Base::resolveStructName(CellI& structId, CellI& resolveState)
{
    return resolveId(structId, kb.ids.structs, kb.ids.unknownStructs, resolveState, [this, &structId](CellI& structReference) -> CellI& {
        auto& unresolvedStruct = *new Object(kb, kb.std.Type_, std::format("{}", structId.label()));
        unresolvedStruct.set("incomplete", kb.boolean.true_);

        return unresolvedStruct;
    });
}

Ast::Struct& Ast::Base::resolveStructNameAsAst(CellI& structName, CellI& resolveState)
{
    Scope& currentScope = static_cast<Scope&>(resolveState[kb.ids.scope]);
    return currentScope.resolveStructName(structName);

}

Ast::Base& Ast::Base::resolveSubTypeNameAsAst(CellI& subTypeName, CellI& resolveState)
{
    Struct& currentStruct = static_cast<Struct&>(resolveState[kb.ids.currentStruct]);
    auto& subTypeAst      = currentStruct.getSubType(subTypeName[kb.ids.name]);
    return subTypeAst;
}

Ast::Base& Ast::Base::resolveTemplatedType(CellI& ast, CellI& resolveState)
{
    auto& templateId     = ast[kb.ids.id];
    auto& templateParams = ast[kb.ids.parameters];

    Scope* scopePtr = &static_cast<Scope&>(resolveState[kb.ids.scope]);
    if (ast.has("scopes")) {
        auto& scopeList = ast[kb.ids.scopes];
        StructT& astStructT = scopePtr->resolveFullTemplateId(scopeList, templateId);
        scopePtr = &static_cast<Scope&>(astStructT[kb.ids.scope]);
    }
    auto& scope = *scopePtr;
    if (!scope.hasStructT(templateId)) {
        std::cerr << std::format("Unknown template {}", templateId.label()) << std::endl;
        std::cerr << std::format("Current scope: {}", scope.label()) << std::endl;
        if (resolveState.has("currentStruct")) {
            std::cerr << std::format("Current struct: {}", resolveState[kb.ids.currentStruct].label()) << std::endl;
        }
        if (resolveState.has("currentFn")) {
            std::cerr << std::format("Current function: {}", resolveState[kb.ids.currentFn].label()) << std::endl;
        }
        throw std::format("Unknown template {}", templateId.label());
    }

    List& resolvedTemplateParams   = *new List(kb, kb.std.Cell, "resolvedTemplateParams");
    List& idCell                   = generateTemplateId(templateId, templateParams, resolveState, resolvedTemplateParams);
    auto& resolvedAstInstance      = resolveTemplateInstanceIdAsAst(idCell, scope, resolveState, ast, resolvedTemplateParams);
    resolvedAstInstance.set("instanceOf", scope.getStructT(templateId));
    resolvedAstInstance.set("templateParams", resolvedTemplateParams);
    auto& resolvedCompiledInstance = resolveTemplateInstanceId(resolvedAstInstance.getFullId(), scope, resolveState, ast, resolvedTemplateParams);

    // std::cout << std::format("DDDD {} resolved at {:p}\n", idCell.label(), (void*)&resolvedCompiledInstance) << std::endl;

    return resolvedType(resolvedAstInstance, resolvedCompiledInstance);
}

List& Ast::Base::generateTemplateId(CellI& id, CellI& parameters, CellI& resolveState, List& resolvedParams)
{
    List& idCell = *new List(kb, kb.std.Cell);
    std::stringstream ss;
    Visitor::visitList(id, [this, &idCell, &ss](CellI& character, int, bool&) {
        idCell.add(character);
        ss << character.label();
    });

    ss << "<";
    Visitor::visitList(parameters, [this, &idCell, &ss, &resolveState, &resolvedParams](CellI& slot, int i, bool&) {
        if (i != 0) {
            ss << ", ";
        }
        CellI& slotRole         = slot[kb.ids.slotRole];
        CellI& slotType         = slot[kb.ids.slotType];
        CellI& resolvedSlotType = resolveType(slotType, resolveState);
        CellI& compiledSlotType = getCompiledTypeFromResolvedType(resolvedSlotType);

        idCell.add(slotRole);
        idCell.add(compiledSlotType);
        resolvedParams.add(kb.ast.slot(slotRole, resolvedSlotType));
        ss << std::format("{}={}", slotRole.label(), compiledSlotType.label());
    });
    ss << ">";
    idCell.label(ss.str());

    return idCell;
}

Ast::ResolvedType& Ast::Base::resolvedType(CellI& astType, CellI& compiledType)
{
    return ResolvedType::New(kb, astType, compiledType);
}

Ast::Parameter::Parameter(brain::Brain& kb, CellI& role) :
    BaseT<Parameter>(kb, kb.std.ast.Parameter)
{
    set("role", role);
}

Ast::ResolvedType::ResolvedType(brain::Brain& kb, CellI& astType, CellI& compiledType) :
    BaseT<ResolvedType>(kb, kb.std.ast.ResolvedType)
{
    set(kb.ids.ast, astType);
    set(kb.ids.compiled, compiledType);
}

Ast::Get& Ast::Parameter::operator/(Base& role)
{
    return Get::New(kb, *this, role);
}

Ast::Get& Ast::Parameter::operator/(const std::string& role)
{
    return Get::New(kb, *this, kb._(role));
}

Ast::Slot::Slot(brain::Brain& kb, CellI& role, CellI& type) :
    BaseT<Slot>(kb, kb.std.ast.Slot, "ast.slot")
{
    set("slotRole", role);
    set("slotType", type);
}

Ast::Call::Call(brain::Brain& kb, CellI& cell, CellI& method) :
    BaseT<Call>(kb, kb.std.ast.Call, "ast.call")
{
    set("cell", cell);
    set("method", method);
}

void Ast::Call::addParam(Slot& slot)
{
    if (missing("parameters")) {
        set("parameters", kb.list(slot));
    } else {
        List& paramList = static_cast<List&>(get("parameters"));
        paramList.add(slot);
    }
}

Ast::StaticCall::StaticCall(brain::Brain& kb, CellI& cell, CellI& method) :
    BaseT<StaticCall>(kb, kb.std.ast.StaticCall, "ast.staticCall")
{
    set("cell", cell);
    set("method", method);
}

void Ast::StaticCall::addParam(Slot& slot)
{
    if (missing("parameters")) {
        set("parameters", kb.list(slot));
    } else {
        List& paramList = static_cast<List&>(get("parameters"));
        paramList.add(slot);
    }
}

Ast::Cell::Cell(brain::Brain& kb, CellI& value) :
    BaseT<Cell>(kb, kb.std.ast.Cell)
{
    set("value", value);
}

Ast::Get& Ast::Cell::operator/(Base& role)
{
    return Get::New(kb, *this, role);
}

Ast::Get& Ast::Cell::operator/(const std::string& role)
{
    return Get::New(kb, *this, kb._(role));
}
Ast::StructName::StructName(brain::Brain& kb, CellI& name) :
    BaseT<StructName>(kb, kb.std.ast.StructName)
{
    set("name", name);
}

Ast::Self::Self(brain::Brain& kb) :
    BaseT<Self>(kb, kb.std.ast.Self, "ast.self")
{
}

Ast::Call& Ast::Self::call(const std::string& method)
{
    return kb.ast.call(*this, method);
}

Ast::SelfFn::SelfFn(brain::Brain& kb) :
    BaseT<SelfFn>(kb, kb.std.ast.SelfFn, "ast.selfFn")
{
}

Ast::Continue::Continue(brain::Brain& kb) :
    BaseT<Continue>(kb, kb.std.ast.Continue, "ast.continue")
{
}

Ast::Break::Break(brain::Brain& kb) :
    BaseT<Break>(kb, kb.std.ast.Break, "ast.break")
{
}

Ast::Try::Try(brain::Brain& kb, Base& tryBranch, Base& catchBranch) :
    BaseT<Try>(kb, kb.std.ast.Try, "ast.try")
{
    set("tryBranch", tryBranch);
    set("catchBranch", catchBranch);
}

Ast::Throw::Throw(brain::Brain& kb) :
    BaseT<Throw>(kb, kb.std.ast.Throw, "ast.throw")
{
}

Ast::Throw::Throw(brain::Brain& kb, Base& value) :
    BaseT<Throw>(kb, kb.std.ast.Throw, "ast.throw")
{
    set("value", value);
}

Ast::Return::Return(brain::Brain& kb) :
    BaseT<Return>(kb, kb.std.ast.Return, "ast.return")
{
}

Ast::Return::Return(brain::Brain& kb, CellI& value) :
    BaseT<Return>(kb, kb.std.ast.Return, "ast.return")
{
    set("value", value);
}

Ast::Block::Block(brain::Brain& kb, List& list) :
    BaseT<Block>(kb, kb.std.ast.Block, "ast.block")
{
    set("asts", list);
}

Ast::Scope::Scope(brain::Brain& kb, const std::string& name) :
    BaseT<Scope>(kb, kb.std.ast.Scope, name)
{
    set("id", kb.id(name));
}

bool Ast::Scope::hasScope(CellI& id)
{
    if (missing("scopes")) {
        return false;
    }

    return scopes().hasKey(id);
}

Ast::Scope& Ast::Scope::getScope(const std::string& name)
{
    return getScope(kb.id(name));
}

Ast::Scope& Ast::Scope::getScope(CellI& id)
{
    if (missing("scopes")) {
        throw "No such scope";
    }

    if (scopes().hasKey(id)) {
        return static_cast<Ast::Scope&>(scopes().getValue(id));
    }

    throw "No such scope";
}

Ast::Scope& Ast::Scope::addScope(const std::string& name)
{
    auto& scope = *new Ast::Scope(kb, name);
    addScope(scope);

    return scope;
}

void Ast::Scope::addScope(Scope& scope)
{
    CellI& id = scope[kb.ids.id];

    if (missing("scopes")) {
        set("scopes", *new TrieMap(kb, kb.std.Cell, kb.std.ast.Scope, "TrieMap<Cell, Type::Ast::Scope>(...)"));
    }
    if (scopes().hasKey(id)) {
        throw "Already registered!";
    }

    scope.set("parent", *this);
    scopes().add(id, scope);
}

Ast::Function& Ast::Scope::addFunction(const std::string& name)
{
    return addFunction(kb.id(name), name);
}

Ast::Function& Ast::Scope::addFunction(CellI& id, const std::string& label)
{
    auto& function = *new Ast::Function(kb, id, label);
    addFunction(function);

    return function;
}

void Ast::Scope::addFunction(Function& function)
{
    auto& name = function[kb.ids.name];

    if (missing("functions")) {
        set("functions", *new TrieMap(kb, kb.std.Cell, kb.std.ast.Function, "TrieMap<Cell, Type::Ast::Function>(...)"));
    }
    if (functions().hasKey(name)) {
        throw "Already registered!";
    }
    function.set("scope", *this);
    functions().add(name, function);
}

Ast::FunctionT& Ast::Scope::addFunctionT(const std::string& name)
{
    return addFunctionT(kb.id(name), name);
}

Ast::FunctionT& Ast::Scope::addFunctionT(CellI& id, const std::string& label)
{
    if (missing("functionTs")) {
        set("functionTs", *new Map(kb, kb.std.Cell, kb.std.ast.FunctionT, "Map<Cell, Type::Ast::FunctionT>(...)"));
    }
    if (functionTs().hasKey(id)) {
        throw "Already registered!";
    }
    auto& functionT = *new Ast::FunctionT(kb, id, label);
    functionTs().add(id, functionT);
    functionT.set("scope", *this);

    return functionT;
}

bool Ast::Scope::hasVariable(CellI& id)
{
    if (missing("variables")) {
        if (missing("parent")) {
            return false;
        }
    } else if (variables().hasKey(id)) {
        return true;
    }

    if (missing("parent")) {
        return false;
    } else {
        return static_cast<Scope&>(get("parent")).hasVariable(id);
    }
}

Ast::Var& Ast::Scope::getVariable(CellI& id)
{
    if (missing("variables")) {
        if (missing("parent")) {
            throw "No such variable!";
        }
    } else if (variables().hasKey(id)) {
        return static_cast<Ast::Var&>(variables().getValue(id));
    }

    if (missing("parent")) {
        throw "No such variable!";
    } else {
        return static_cast<Scope&>(get("parent")).getVariable(id);
    }
}

Ast::Var& Ast::Scope::addVariable(CellI& id)
{
    if (missing("variables")) {
        set("variables", *new Map(kb, kb.std.Cell, kb.std.ast.Var, "Map<Cell, Type::Ast::Var>(...)"));
    }
    if (variables().hasKey(id)) {
        throw "Already registered!";
    }
    auto& var = *new Ast::Var(kb, id);
    variables().add(id, var);
    var.set("scope", *this);

    return var;
}

Ast::Struct& Ast::Scope::resolveStructName(CellI& structName)
{
    auto& name = structName[kb.ids.name];
    if (structName.has(kb.ids.scopes)) {
        auto& scopes = structName[kb.ids.scopes];
        return resolveFullStructId(scopes, name);
    } else {
        return resolveStructNameImpl(name);
    }
}

Ast::Struct& Ast::Scope::resolveStructNameImpl(CellI& name)
{
    if (missing("structs")) {
        if (missing("parent")) {
            throw "No struct in scope!";
        }
    } else if (structs().hasKey(name)) {
        return static_cast<Ast::Struct&>(structs().getValue(name));
    }

    if (missing("parent")) {
        throw "No struct in scope!";
    } else {
        return static_cast<Scope&>(get("parent")).resolveStructNameImpl(name);
    }
}


bool Ast::Scope::hasStruct(CellI& id)
{
    if (missing("structs")) {
        return false;
    }
    return (structs().hasKey(id));
}

Ast::Struct& Ast::Scope::getStruct(const std::string& name)
{
    return getStruct(kb.id(name));
}

Ast::Struct& Ast::Scope::getStruct(CellI& id)
{
    if (missing("structs")) {
        throw "No such struct";
    }
    return static_cast<Ast::Struct&>(structs().getValue(id));
}

void Ast::Scope::addStruct(Struct& struct_)
{
    CellI& id = struct_[kb.ids.id];

    if (missing("structs")) {
        set("structs", *new TrieMap(kb, kb.std.Cell, kb.std.ast.Struct, "TrieMap<Cell, Type::Ast::Struct>(...)"));
    }
    if (structs().hasKey(id)) {
        throw "Already registered!";
    }
    structs().add(id, struct_);
    struct_.set("scope", *this);
}

Ast::Struct& Ast::Scope::addStruct(const std::string& name)
{
    CellI& id = kb.id(name);

    auto& struct_ = *new Ast::Struct(kb, id, name);
    addStruct(struct_);

    return struct_;
}

bool Ast::Scope::hasStructT(CellI& id)
{
    if (missing("structTs")) {
        return false;
    }
    return structTs().hasKey(id);
}

Ast::StructT& Ast::Scope::getStructT(CellI& id)
{
    return static_cast<Ast::StructT&>(structTs().getValue(id));
}

Ast::StructT& Ast::Scope::addStructT(const std::string& name)
{
    CellI& id = kb.id(name);
    if (missing("structTs")) {
        set("structTs", *new TrieMap(kb, kb.std.Cell, kb.std.ast.StructT, "TrieMap<Cell, Type::Ast::StructT>(...)"));
    }
    if (structTs().hasKey(id)) {
        throw "Already registered!";
    }
    auto& structT = *new Ast::StructT(kb, id, name);
    structTs().add(id, structT);
    structT.set("scope", *this);

    return structT;
}

Ast::Struct& Ast::Scope::resolveFullStructId(CellI& scopeList, CellI& id)
{
    const auto& hasCb = [&id](Ast::Scope& currentScope) -> bool {
        return currentScope.hasStruct(id);
    };
    const auto& getCb = [&id](Ast::Scope& currentScope) -> Struct* {
        return &currentScope.getStruct(id);
    };

    return static_cast<Struct&>(resolveFullIdInAllScope(scopeList, id, hasCb, getCb));
}

Ast::StructT& Ast::Scope::resolveFullTemplateId(CellI& scopeList, CellI& id)
{
    const auto& hasCb = [&id](Ast::Scope& currentScope) -> bool {
        return currentScope.hasStructT(id);
    };
    const auto& getCb = [&id](Ast::Scope& currentScope) -> StructT* {
        return &currentScope.getStructT(id);
    };

    return static_cast<StructT&>(resolveFullIdInAllScope(scopeList, id, hasCb, getCb));
}

Ast::Base& Ast::Scope::resolveFullIdInAllScope(CellI& scopeList, CellI& id, std::function<bool(Ast::Scope& currentScope)> hasCb, std::function<Base*(Ast::Scope& currentScope)> getCb)
{
    // resolve in local scope
    Base* ret = nullptr;
    if (ret = resolveFullIdInOneScope(this, scopeList, hasCb, getCb)) {
        return *ret;
    }

    // resolve in parent scope
    if (missing(kb.ids.parent)) {
        throw "Unknown Struct!";
    }
    Scope* rootScope   = &getRootScope();
    Scope* parentScope = &static_cast<Scope&>(get("parent"));
    if (parentScope != rootScope) {
        if (ret = resolveFullIdInOneScope(parentScope, scopeList, hasCb, getCb)) {
            return *ret;
        }
    }

    // resolve in root scope
    if (ret = resolveFullIdInOneScope(rootScope, scopeList, hasCb, getCb)) {
        return *ret;
    }

    throw "Unknown Struct!";
}

Ast::Base* Ast::Scope::resolveFullIdInOneScope(Scope* currentScope, CellI& scopeList, std::function<bool(Ast::Scope& currentScope)> hasCb, std::function<Base*(Ast::Scope& currentScope)> getCb)
{
    // resolve in local scope
    Visitor::visitList(scopeList, [this, &currentScope](CellI& scopeId, int, bool& stop) {
        if (currentScope->hasScope(scopeId)) {
            currentScope = &currentScope->getScope(scopeId);
        } else {
            currentScope = nullptr;
            stop         = true;
        }
    });
    if (currentScope && hasCb(*currentScope)) {
        return getCb(*currentScope);
    }

    return nullptr;
}

Ast::Scope& Ast::Scope::getRootScope()
{
    auto* currentScope = this;

    while (currentScope->has("parent")) {
        currentScope = &static_cast<Scope&>(currentScope->get("parent"));
    }

    return *currentScope;
}

CellI& Ast::Scope::getFullId()
{
    if (has("fullId")) {
        return get("fullId");
    }
    Scope& rootScope   = getRootScope();
    auto* currentScope = this;
    List& fullId = *new List(kb, kb.std.Char);
    while (currentScope && currentScope != &rootScope) {
        auto& currentScopeId = (*currentScope)[kb.ids.id];
        if (!fullId.empty()) {
            fullId.addFront(kb.pools.chars.get(':'));
            fullId.addFront(kb.pools.chars.get(':'));
        }
        Visitor::visitListInReverse(currentScopeId, [this, &fullId](CellI& character, int i, bool& stop) {
            fullId.addFront(character);
        });
        currentScope = currentScope->has("parent") ? &static_cast<Scope&>(currentScope->get("parent")) : nullptr;
    };
    std::stringstream ss;
    Visitor::visitList(fullId, [this, &fullId, &ss](CellI& character, int i, bool& stop) {
        ss << character.label();
    });
    fullId.label(ss.str());

    return fullId;
}

/*
Compiler steps:
Resolve template related references in normal functions or structs:
  - where templated types is used, for example tt_("List", ids.valueType, _(type.Slot))
    it must be resolved to a StructName, with id L,i,s,t,ids.valueType,type.Slot
  - create a shadow ast tree with subtituted nodes
  - create a list of candidates for template instantiation with method names
    instantiate structT without methods
    instantiate structT with listed method
  - templates are instantied to a dedicated place
*/
static bool debugCompiledStructs = true;
CellI& Ast::Scope::compile(TrieMap& earlyStructs)
{
    auto& program     = *new Object(kb, kb.std.Program, "Program");
    auto& programData = *new Object(kb, kb.std.ProgramData, "ProgramData");
    program.set("data", programData);

    auto& resolvedScope = *new Ast::Scope(kb, label());
    set("resolvedScope", resolvedScope);

    auto& compileState      = *new Object(kb, kb.std.CompileState, "CompileState");
    auto& compiledFunctions = *new TrieMap(kb, kb.std.Cell, kb.std.op.Function, "Functions");
    auto& compiledStructs   = *new TrieMap(kb, kb.std.Cell, kb.std.Type_, "Types");

    auto& functions           = *new List(kb, kb.std.op.Function, "Functions");
    auto& structs             = *new TrieMap(kb, kb.std.Cell, kb.std.Type_, "structs");
    auto& unknownStructs      = *new TrieMap(kb, kb.std.Cell, kb.std.Type_, "unknownStructs");
    auto& instances           = *new TrieMap(kb, kb.std.Cell, kb.std.Type_, "instances");
    auto& unknownInstances    = *new TrieMap(kb, kb.std.Cell, kb.std.Type_, "unknownInstances");
    auto& instanceAsts        = *new TrieMap(kb, kb.std.Cell, kb.std.Type_, "instanceAsts");
    auto& unknownInstanceAsts = *new TrieMap(kb, kb.std.Cell, kb.std.Type_, "unknownInstanceAsts");
    auto& compiledVariables   = *new TrieMap(kb, kb.std.Cell, kb.std.op.Var, "Variables");

    programData.set("functions", compiledFunctions);
    programData.set("structs", compiledStructs);
    programData.set("variables", compiledVariables);

    compileState.set("functions", functions);
    compileState.set("structs", structs);
    compileState.set("unknownStructs", unknownStructs);
    compileState.set("instances", instances);
    compileState.set("unknownInstances", unknownInstances);
    compileState.set("instanceAsts", instanceAsts);
    compileState.set("unknownInstanceAsts", unknownInstanceAsts);
    compileState.set("variables", compiledVariables);
    compileState.set("resolvedScope", resolvedScope);
    compileState.set("globalScope", *this);
    compileState.set("globalResolvedScope", resolvedScope);

    Visitor::visitList(earlyStructs[kb.ids.list], [this, &unknownStructs, &unknownInstances](CellI& earlyStructKV, int i, bool& stop) {
        auto& structId       = earlyStructKV[kb.ids.key];
        auto& structRefAst   = earlyStructKV[kb.ids.value][kb.ids.slotRole];
        auto& compiledStruct = earlyStructKV[kb.ids.value][kb.ids.slotType];
        if (debugCompiledStructs) {
            std::cout << "early struct: " << earlyStructKV[kb.ids.key].label() << std::endl;
        }
        auto& structReference = *new Object(kb, kb.std.StructReference);
        structReference.set("value", compiledStruct);
        structReference.set("id", structId);

        if (&structRefAst.type() == &kb.std.ast.TemplatedType) {
            unknownInstances.add(structId, structReference);
        } else if (&structRefAst.type() == &kb.std.ast.StructName) {
            unknownStructs.add(structId, structReference);
        }
    });

    resolveTypes(compileState);

    auto& stdScope = getScope("std");
    auto& resolvedStdScope = resolvedScope.getScope("std");

    Visitor::visitList(earlyStructs[kb.ids.list], [this, &unknownStructs, &unknownInstances, &stdScope, &resolvedStdScope](CellI& earlyStructKV, int i, bool& stop) {
        auto& structId       = earlyStructKV[kb.ids.key];
        auto& structRefAst   = earlyStructKV[kb.ids.value][kb.ids.slotRole];
        auto& compiledStruct = earlyStructKV[kb.ids.value][kb.ids.slotType];
        if (debugCompiledStructs) {
            std::cout << "resolve early struct: " << earlyStructKV[kb.ids.key].label() << std::endl;
        }

        if (&structRefAst.type() == &kb.std.ast.TemplatedType) {
            if (unknownInstances.hasKey(structId)) {
                CellI& structReference = unknownInstances.getValue(structId);
                structReference.set("scope", stdScope);
                structReference.set("resolvedScope", stdScope);
                structReference.set("templateId", structRefAst["id"]);
                structReference.set(kb.ids.templateParams, structRefAst[kb.ids.parameters]);
            }
        } else if (&structRefAst.type() == &kb.std.ast.StructName) {
            if (unknownStructs.hasKey(structId)) {
                CellI& structReference = unknownStructs.getValue(structId);
            }
        }
    });

    Visitor::visitList(unknownStructs[kb.ids.list], [this](CellI& unknownStruct, int i, bool& stop) {
        if (debugCompiledStructs) {
            std::cout << "unknown struct: " << unknownStruct[kb.ids.value][kb.ids.value].label() << std::endl;
        }
    });
    int instantiedNum = 0;
    Visitor::visitList(unknownInstances[kb.ids.list], [this, &compileState, &instantiedNum](CellI& unknownInstanceSlot, int i, bool& stop) {
        CellI& unknownInstance  = unknownInstanceSlot[kb.ids.value];
        auto& unknownInstanceId = unknownInstance[kb.ids.id];
        if (debugCompiledStructs) {
            std::cout << "unknown instance: " << unknownInstanceId.label() << std::endl;
            if (unknownInstance.has("currentStruct")) {
                std::cout << "     from struct: " << unknownInstance[kb.ids.currentStruct].label() << std::endl;
            }
            if (unknownInstance.has("currentFn")) {
                std::cout << "   from function: " << unknownInstance[kb.ids.currentFn].label() << std::endl;
            }
        }
        std::stringstream ss;

        CellI& templateId     = unknownInstance[kb.ids.templateId];
        CellI& templateParams = unknownInstance[kb.ids.templateParams];
        auto& scope           = static_cast<Scope&>(unknownInstance[kb.ids.scope]);
        auto& idScope         = unknownInstance.has(kb.id("idScope")) ? static_cast<Scope&>(unknownInstance[kb.id("idScope")]) : scope;

        ss << std::format("        in scope: {}\n", idScope.getFullId().label());
        ss << std::format("  instantiate id: {}<", templateId.label());
        Visitor::visitList(templateParams, [this, &ss, &compileState](CellI& param, int i, bool& stop) {
            CellI& paramId   = param[kb.ids.slotRole];
            CellI& paramType = param[kb.ids.slotType];
            if (i > 0) {
                ss << ", ";
            }
            ss << std::format("{}: {}", paramId.label(), getCompiledTypeFromResolvedType(paramType).label());
        });
        ss << ">";
        if (debugCompiledStructs) {
            std::cout << ss.str() << "\n"
                      << std::endl;
        }

        auto& resolvedIdScope = static_cast<Scope&>(idScope[kb.ids.resolvedScope]);
        compileState.set("scope", idScope);
        auto& structT          = idScope.getStructT(templateId);
        auto& instantiedStruct = structT.instantiateWith(static_cast<List&>(templateParams), compileState);
        auto& resolvedStruct = instantiedStruct.resolveTypes(compileState);
        resolvedIdScope.addStruct(resolvedStruct);
        instantiedNum = i + 1;
    });
    if (unknownStructs.size() > 0 || unknownInstances.size() != instantiedNum) {
        throw "Referencing to an unknown type!";
    }
    compileState.set("scope", *this);
    compileState.set("resolvedScope", resolvedScope);

    compileTheResolvedAsts(programData, compileState);

    return program;
}

void Ast::Scope::compileTheResolvedAsts(CellI& programData, CellI& state)
{
    auto& scope             = static_cast<Scope&>(state[kb.ids.scope]);
    auto& resolvedScope     = static_cast<Scope&>(state[kb.ids.resolvedScope]);
    auto& compiledFunctions = static_cast<TrieMap&>(programData[kb.ids.functions]);
    auto& compiledStructs   = static_cast<TrieMap&>(programData[kb.ids.structs]);
    auto& compiledVariables = static_cast<TrieMap&>(programData[kb.ids.variables]);

    if (scope.has("functions")) {
        Visitor::visitList(resolvedScope.functions()[kb.ids.list], [this, &state, &compiledFunctions](CellI& function, int i, bool& stop) {
            Ast::Function& astFunction = static_cast<Ast::Function&>(function[kb.ids.value]);
            auto& compiledFunction     = astFunction.compile(state);
            compiledFunctions.add(astFunction.getFullId(), compiledFunction);
        });
    }
    if (scope.has("structs")) {
        Visitor::visitList(resolvedScope.structs()[kb.ids.list], [this, &state, &compiledStructs](CellI& struct_, int i, bool& stop) {
            Ast::Struct& astStruct = static_cast<Ast::Struct&>(struct_[kb.ids.value]);
            auto& compiledStruct = astStruct.compile(state);
            compiledStructs.add(astStruct.getFullId(), compiledStruct);
        });
    }
    if (scope.has("variables")) {
        Visitor::visitList(resolvedScope.variables()[kb.ids.list], [this, &compiledVariables](CellI& var, int i, bool& stop) {
            Ast::Var& astVar       = static_cast<Ast::Var&>(var[kb.ids.value]);
            auto& varName    = astVar[kb.ids.role];
            auto& compiledVariable = *new Object(kb, kb.std.op.Var, std::format("var {}", astVar.label()));
            compiledVariables.add(varName, compiledVariable);
        });
    }
    if (scope.has("scopes")) {
        Visitor::visitList(scopes()[kb.ids.list], [this, &programData, &state, &resolvedScope](CellI& scope, int i, bool& stop) {
            Ast::Scope& astScope = static_cast<Ast::Scope&>(scope[kb.ids.value]);
            state.set("scope", astScope);
            state.set("resolvedScope", resolvedScope.getScope(astScope[kb.ids.id]));
            astScope.compileTheResolvedAsts(programData, state);
            state.set("scope", *this);
            state.set("resolvedScope", resolvedScope);
        });
    }
}

void Ast::Scope::resolveTypes(CellI& state)
{
    state.set("scope", *this);
    Ast::Scope& resolvedScope = static_cast<Ast::Scope&>(state[kb.ids.resolvedScope]);

    if (has("functions")) {
        state.erase("currentStruct");
        Visitor::visitList(functions()[kb.ids.list], [this, &state, &resolvedScope](CellI& origAstFunctionCell, int i, bool& stop) {
            Ast::Function& origAstFunction     = static_cast<Ast::Function&>(origAstFunctionCell[kb.ids.value]);
            Ast::Function& resolvedAstFunction = origAstFunction.resolveTypes(state);
            resolvedScope.addFunction(resolvedAstFunction);
            std::cout << resolvedAstFunction.label() << std::endl;
        });
    }
    if (has("structs")) {
        Visitor::visitList(structs()[kb.ids.list], [this, &state, &resolvedScope](CellI& origAstStructCell, int i, bool& stop) {
            Ast::Struct& origAstStruct     = static_cast<Ast::Struct&>(origAstStructCell[kb.ids.value]);
            Ast::Struct& resolvedAstStruct = origAstStruct.resolveTypes(state);
            resolvedScope.addStruct(resolvedAstStruct);
        });
    }
    if (has("variables")) {
        Visitor::visitList(variables()[kb.ids.list], [this, &state, &resolvedScope](CellI& origAstVarCell, int i, bool& stop) {
            Ast::Var& origAstVar = static_cast<Ast::Var&>(origAstVarCell[kb.ids.value]);
            resolvedScope.addVariable(origAstVar);
        });
    }
    if (has("scopes")) {
        Visitor::visitList(scopes()[kb.ids.list], [this, &state, &resolvedScope](CellI& origAstScopeCell, int i, bool& stop) {
            Ast::Scope& origAstScope = static_cast<Ast::Scope&>(origAstScopeCell[kb.ids.value]);
            auto& newResolvedScope   = *new Ast::Scope(kb, origAstScope.label());
            origAstScope.set("resolvedScope", newResolvedScope);
            resolvedScope.addScope(newResolvedScope);
            state.set("resolvedScope", newResolvedScope);
            origAstScope.resolveTypes(state);
            state.set("scope", *this);
            state.set("resolvedScope", resolvedScope);
        });
    }
}

TrieMap& Ast::Scope::variables()
{
    if (missing("variables")) {
        throw "No variables!";
    } else {
        return static_cast<TrieMap&>(get("variables"));
    }
}

TrieMap& Ast::Scope::scopes()
{
    if (missing("scopes")) {
        throw "No scopes!";
    } else {
        return static_cast<TrieMap&>(get("scopes"));
    }
}

TrieMap& Ast::Scope::functions()
{
    if (missing("functions")) {
        throw "No functions!";
    } else {
        return static_cast<TrieMap&>(get("functions"));
    }
}

TrieMap& Ast::Scope::functionTs()
{
    if (missing("functionTs")) {
        throw "No functions!";
    } else {
        return static_cast<TrieMap&>(get("functionTs"));
    }
}

TrieMap& Ast::Scope::structs()
{
    if (missing("structs")) {
        throw "No structs!";
    } else {
        return static_cast<TrieMap&>(get("structs"));
    }
}

TrieMap& Ast::Scope::structTs()
{
    if (missing("structTs")) {
        throw "No structs!";
    } else {
        return static_cast<TrieMap&>(get("structTs"));
    }
}

Ast::StructBase::StructBase(brain::Brain& kb, CellI& astType, CellI& id, const std::string& label) :
    Base(kb, astType, label)
{
    set("id", id);
}

Ast::StructBase::StructBase(brain::Brain& kb, CellI& astType, const std::string& name) :
    Base(kb, astType, name)
{
    set("id", kb.id(name));
}

Ast::Function& Ast::StructBase::addMethod(const std::string& name)
{
    auto& id = kb.id(name);
    Ast::Function& method = *new Ast::Function(kb, id, name);
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

Ast::StructBase& Ast::StructBase::members(Slot& slot)
{
    if (missing("members")) {
        set("members", *new Map(kb, kb.std.Cell, kb.std.ast.Slot));
    }
    members().add(slot[kb.ids.slotRole], slot);

    return *this;
}

Ast::StructBase& Ast::StructBase::subTypes(Slot& slot)
{
    if (missing("subTypes")) {
        set("subTypes", *new Map(kb, kb.std.Cell, kb.std.ast.Base));
    }
    CellI& slotRole = slot[kb.ids.slotRole];
    CellI& slotType = slot[kb.ids.slotType];

    subTypes().add(slotRole, slot);

    return *this;
}

Ast::StructBase& Ast::StructBase::memberOf(CellI& type)
{
    if (missing("memberOf")) {
        set("memberOf", *new List(kb, kb.std.Type_));
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

CellI& Ast::StructBase::id()
{
    return get("id");
}

Ast::Base& Ast::StructBase::getSubType(CellI& name)
{
    return static_cast<Ast::Base&>(subTypes().getValue(name)[kb.ids.slotType]);
}

Ast::Struct::Struct(brain::Brain& kb, CellI& id, const std::string& label) :
    StructBase(kb, kb.std.ast.Struct, id, label)
{
}

Ast::Struct::Struct(brain::Brain& kb, const std::string& name) :
    StructBase(kb, kb.std.ast.Struct, name)
{
    getFullId();
}

CellI& Ast::Struct::getFullId()
{
    if (has("fullId")) {
        return get("fullId");
    }
    Scope& scope = static_cast<Scope&>(get("scope"));
    CellI& scopeFullId = scope.getFullId();

    List& fullId = *new List(kb, kb.std.Char);
    Visitor::visitList(scopeFullId, [this, &fullId](CellI& character, int i, bool& stop) {
        fullId.add(character);
    });
    if (!fullId.empty()) {
        fullId.add(kb.pools.chars.get(':'));
        fullId.add(kb.pools.chars.get(':'));
    }
    auto& id = get(kb.ids.id);
    Visitor::visitList(id, [this, &fullId](CellI& character, int i, bool& stop) {
        fullId.add(character);
    });
    std::stringstream ss;
    int templateParamPrintModeFromCharIndex = fullId.size();
    if (has("instanceOf")) {
        int paramsLength = static_cast<List&>(get("templateParams")).size();
        templateParamPrintModeFromCharIndex = fullId.size() - paramsLength * 2;
    }
    Visitor::visitList(fullId, [this, &fullId, &ss, &templateParamPrintModeFromCharIndex](CellI& character, int i, bool& stop) {
        if (i == templateParamPrintModeFromCharIndex) {
            stop = true;
            return;
        }
        ss << character.label();
    });
    if (has("instanceOf")) {
        ss << "<";
        Visitor::visitList(get("templateParams"), [this, &ss](CellI& slot, int i, bool& stop) {
            CellI& slotRole = slot[kb.ids.slotRole];
            CellI& slotType = slot[kb.ids.slotType];
            if (i != 0) {
                ss << ", ";
            }
            ss << std::format("{}={}", slotRole.label(), getCompiledTypeFromResolvedType(slotType).label());
        });
        ss << ">";
    }
    fullId.label(ss.str());
    set("fullId", fullId);

    return fullId;
}

Ast::Struct& Ast::Struct::resolveTypes(CellI& state)
{
    auto& structId       = get("id");
    auto& structs        = static_cast<TrieMap&>(state[kb.ids.structs]);
    auto& unknownStructs = static_cast<TrieMap&>(state[kb.ids.unknownStructs]);
    Ast::Struct& ret     = *new Ast::Struct(kb, structId, std::format("{}", label()));

    if (has("instanceOf")) {
        ret.set("instanceOf", get("instanceOf"));
        ret.set("templateParams", get("templateParams"));
        ret.set("scope", get("scope"));
    }

    auto& fullId = getFullId();
    CellI* resolvedStructPtr = nullptr;
    if (unknownStructs.hasKey(fullId)) {
        CellI& unknownStruct = unknownStructs.getValue(fullId);
        resolvedStructPtr    = &unknownStruct["value"];
        unknownStructs.remove(fullId);
    } else {
        resolvedStructPtr = new Object(kb, kb.std.Type_, std::format("{}", fullId.label()));
    }
    auto& resolvedStruct = *resolvedStructPtr;
    structs.add(getFullId(), resolvedStruct);

    state.set("currentStruct", ret);

    std::stringstream ss;
    std::stringstream subTypesSs;
    if (debugCompiledStructs) {
        ss << std::format("struct {}", label());
    }

    // resolve sub types
    if (has("subTypes")) {
        Visitor::visitList(subTypes()[kb.ids.list], [this, &ret, &state, &subTypesSs](CellI& subTypeCell, int i, bool& stop) {
            CellI& subTypeId           = subTypeCell[kb.ids.slotRole];
            CellI& subTypeType         = subTypeCell[kb.ids.slotType];
            CellI& resolvedSubTypeType = resolveType(subTypeType, state);
            ret.subTypes(kb.ast.slot(subTypeId, resolvedSubTypeType));
            if (debugCompiledStructs) {
                subTypesSs << std::format("    alias {} = {};", subTypeId.label(), getCompiledTypeFromResolvedType(resolvedSubTypeType).label()) << std::endl;
            }
        });
        if (debugCompiledStructs) {
            if (has("methods") || has("members")) {
                subTypesSs << std::endl;
            }
        }
    }

    // resolve memberOf list
    if (has("memberOf")) {
        ss << " : ";
        Visitor::visitList(memberOf(), [this, &ret, &state, &ss](CellI& membershipType, int i, bool& stop) {
            CellI& resolvedMembershipType = resolveType(membershipType, state);
            if (i > 0) {
                ss << ", ";
            }
            ss << getCompiledTypeFromResolvedType(resolvedMembershipType).label();
            ret.memberOf(resolvedMembershipType);
        });
    }
    if (debugCompiledStructs) {
        std::cout << ss.str() << " {" << std::endl;
        std::cout << subTypesSs.str();
    }

    // resolve methods
    if (has("methods")) {
        Visitor::visitList(methods()[kb.ids.list], [this, &ret, &state](CellI& origAstFunctionCell, int i, bool& stop) {
            auto& origAstFunction     = static_cast<Ast::Function&>(origAstFunctionCell);
            auto& resolvedAstFunction = origAstFunction.resolveTypes(state);
            ret.addMethod(resolvedAstFunction);
            if (debugCompiledStructs) {
                std::cout << std::format("    {};\n", resolvedAstFunction.shortName());
            }
        });
        if (debugCompiledStructs) {
            if (has("members")) {
                std::cout << std::endl;
            }
        }
    }
    // resolve members
    if (has("members")) {
        CellI& membersList = members()[kb.ids.list];
        Visitor::visitList(membersList, [this, &ret, &state](CellI& memberCell, int i, bool& stop) {
            CellI& memberId   = memberCell[kb.ids.slotRole];
            CellI& memberType = memberCell[kb.ids.slotType];
            CellI& resolvedMemberType = resolveType(memberType, state);
            ret.members(kb.ast.slot(memberId, resolvedMemberType));
            if (debugCompiledStructs) {
                std::cout << std::format("    {}: {};", memberId.label(), getCompiledTypeFromResolvedType(resolvedMemberType).label()) << std::endl;
            }
        });
    }

    if (debugCompiledStructs) {
        std::cout << "}" << std::endl;
    }

    return ret;
}

CellI& Ast::Struct::compile(CellI& state)
{
    CellI& compiledStruct = getResolvedTypeById(getFullId(), has("instanceOf"), state);
    compiledStruct.erase("incomplete");
    // std::cout << std::format("DDDD compile {} resolved at {:p}\n", getFullId().label(), (void*)&compiledStruct) << std::endl;

    // compile sub types
    if (has("subTypes")) {
        Map& compiledSubTypes = *new Map(kb, kb.std.Cell, kb.std.Type_, "subTypes Map<Cell, Type>(...)");
        Visitor::visitList(subTypes()[kb.ids.list], [this, &compiledSubTypes](CellI& slot, int i, bool& stop) {
            CellI& slotRole        = slot[kb.ids.slotRole];
            CellI& slotType        = slot[kb.ids.slotType];
            auto& compiledSlotType = getCompiledTypeFromResolvedType(slotType);
            compiledSubTypes.add(slotRole, compiledSlotType);
        });
        compiledStruct.set("subTypes", compiledSubTypes);
    }

    // compile methods
    if (has("methods")) {
        Map& compiledMethods = *new Map(kb, kb.std.Cell, kb.std.ast.Function);
        Visitor::visitList(methods()[kb.ids.list], [this, &compiledMethods, &state](CellI& astFunction, int i, bool& stop) {
            auto& compiledFunction = static_cast<Ast::Function&>(astFunction).compile(state);
            compiledMethods.add(astFunction[kb.ids.name], compiledFunction);
        });
        compiledStruct.set("methods", compiledMethods);
    }

    // compile members
    if (has("members")) {
        Map& compiledMembers = *new Map(kb, kb.std.Cell, kb.std.Slot, "members Map<Cell, Slot>(...)");
        Visitor::visitList(members()[kb.ids.list], [this, &compiledMembers, &compiledStruct, &state](CellI& slot, int i, bool& stop) {
            CellI& slotRole          = slot[kb.ids.slotRole];
            CellI& slotType          = slot[kb.ids.slotType];
            auto& compiledSlotType   = getCompiledTypeFromResolvedType(slotType);
            compiledMembers.add(slotRole, kb.std.slot(slotRole, compiledSlotType));
        });
        compiledStruct.set("slots", compiledMembers);
    }

    // compile memberOf list
    if (has("memberOf")) {
        Map& compiledMemberOfs = *new Map(kb, kb.std.Type_, kb.std.Type_, "memberOf Map<Type, Type>(...)");
        Visitor::visitList(memberOf(), [this, &compiledMemberOfs](CellI& membershipType, int i, bool& stop) {
            auto& compiledMembershipType = getCompiledTypeFromResolvedType(membershipType);
            compiledMemberOfs.add(compiledMembershipType, compiledMembershipType);
        });
        compiledStruct.set("memberOf", compiledMemberOfs);
    }

    return compiledStruct;
}

Ast::StructT::StructT(brain::Brain& kb, CellI& id, const std::string& label) :
    StructBase(kb, kb.std.ast.StructT, id, label)
{
}

Ast::StructT::StructT(brain::Brain& kb, const std::string& name) :
    StructBase(kb, kb.std.ast.Struct, name)
{
}

Ast::StructT& Ast::StructT::templateParams(Slot& slot)
{
    if (missing("templateParams")) {
        set("templateParams", *new Map(kb, kb.std.Cell, kb.std.Type_));
    }
    CellI& slotRole = slot[kb.ids.slotRole];
    CellI& slotType = slot[kb.ids.slotType];
    if (!(&slotType.type() == &kb.std.ast.Cell || &slotType.type() == &kb.std.ast.TemplatedType)) {
        throw "Invalid template param type!";
    }
    CellI* paramType = nullptr;
    if (&slotType.type() == &kb.std.ast.Cell) {
        paramType = &slotType[kb.ids.value];
    } else {
        throw "TODO";
    }
    templateParams().add(slotRole, *paramType);

    return *this;
}

Ast::Struct& Ast::StructT::instantiateWith(List& inputParams, CellI& state)
{
    // process input parameters
    if (!inputParams.empty() && missing("templateParams")) {
        throw "No template parameter was given!";
    }
    std::stringstream ss;
    Map inputParameters(kb, kb.std.Cell, kb.std.Cell);
    List& idCell = *new List(kb, kb.std.Cell);
    Visitor::visitList(id(), [this, &idCell](CellI& character, int i, bool& stop) {
        idCell.add(character);
    });

    Visitor::visitList(inputParams, [this, &inputParameters, &ss, &idCell](CellI& slot, int i, bool& stop) {
        CellI& slotRole = slot[kb.ids.slotRole];
        CellI& slotType = slot[kb.ids.slotType];
        inputParameters.add(slotRole, slotType);
        CellI& compiledSlotType = getCompiledTypeFromResolvedType(slotType);
        if (i != 0) {
            ss << ", ";
        }
        ss << slotRole.label() << "=" << compiledSlotType.label();
        if (!templateParams().hasKey(slotRole)) {
            throw "Instantiating with unknown template parameter!";
        }
        idCell.add(slotRole);
        idCell.add(compiledSlotType);
    });
    idCell.label(std::format("{}<{}>", id().label(), ss.str()));
    Ast::Struct* retPtr = nullptr;
    auto& unknownInstanceAsts = static_cast<TrieMap&>(state["unknownInstanceAsts"]);
    if (unknownInstanceAsts.hasKey(idCell)) {
        retPtr = &static_cast<Ast::Struct&>(unknownInstanceAsts.getValue(idCell)[kb.ids.value]);
     } else {
        auto& instanceAsts = static_cast<TrieMap&>(state["instanceAsts"]);
        retPtr             = new Ast::Struct(kb, idCell, std::format("{}<{}>", id().label(), ss.str()));
        instanceAsts.add(idCell, *retPtr);
    }
    Ast::Struct& ret = *retPtr;
    ret.set("instanceOf", *this);
    ret.set("templateParams", inputParams);
    ret.set("scope", static_cast<Scope&>(get("scope")));

    // instantiate sub types
    if (has("subTypes")) {
        Map& instantiatedSubTypes = *new Map(kb, kb.std.Cell, kb.std.ast.Base);
        Visitor::visitList(subTypes()[kb.ids.list], [this, &inputParameters, &instantiatedSubTypes, &ret, &state](CellI& slot, int i, bool& stop) {
            CellI& slotRole          = slot[kb.ids.slotRole];
            CellI& slotType          = slot[kb.ids.slotType];
            CellI& instantiatedParam = instantiateTemplateParamType(slotType, ret, inputParameters, state);
            instantiatedSubTypes.add(slotRole, kb.ast.slot(slotRole, instantiatedParam));
        });
        ret.set("subTypes", instantiatedSubTypes);
    }

    // instantiate methods
    if (has("methods")) {
        Visitor::visitList(methods()[kb.ids.list], [this, &inputParameters, &ret, &state](CellI& astFunction, int i, bool& stop) {
            Ast::Function& instantiedFunction = *new Function(kb, astFunction[kb.ids.name], astFunction[kb.ids.name].label());
            ret.addMethod(instantiedFunction);

            // parameters
            if (astFunction.has("parameters")) {
                List& instantiatedParameters = *new List(kb, kb.std.Slot);
                Visitor::visitList(astFunction[kb.ids.parameters], [this, &inputParameters, &instantiatedParameters, &ret, &state](CellI& slot, int i, bool& stop) {
                    CellI& slotRole          = slot[kb.ids.slotRole];
                    CellI& slotType          = slot[kb.ids.slotType];
                    CellI& instantiatedParam = instantiateTemplateParamType(slotType, ret, inputParameters, state);
                    instantiatedParameters.add(kb.ast.slot(slotRole, instantiatedParam));
                });
                instantiedFunction.set("parameters", instantiatedParameters);
            }
            // return type
            if (astFunction.has("returnType")) {
                CellI& returnType             = astFunction[kb.ids.returnType];
                CellI& instantiatedReturnType = instantiateTemplateParamType(returnType, ret, inputParameters, state);
                instantiedFunction.set("returnType", instantiatedReturnType);
            }
            // code
            if (astFunction.has("code")) {
                instantiedFunction.set("code", instantiateAst(astFunction[kb.ids.code], ret, inputParameters, state));
            }
        });
    }

    // instantiate members
    if (has("members")) {
        Map& instantiatedMembers = *new Map(kb, kb.std.Cell, kb.std.Slot);
        Visitor::visitList(members()[kb.ids.list], [this, &inputParameters, &instantiatedMembers, &ret, &state](CellI& slot, int i, bool& stop) {
            CellI& slotRole = slot[kb.ids.slotRole];
            CellI& slotType = slot[kb.ids.slotType];
            CellI& instantiatedParam = instantiateTemplateParamType(slotType, ret, inputParameters, state);
            instantiatedMembers.add(slotRole, kb.ast.slot(slotRole, instantiatedParam));
        });
        ret.set("members", instantiatedMembers);
    }

    // instantiate memberOf list
    if (has("memberOf")) {
        List& instantiatedMemberOfs = *new List(kb, kb.std.Type_);
        Visitor::visitList(memberOf(), [this, &inputParameters, &instantiatedMemberOfs, &ret, &state](CellI& membershipType, int i, bool& stop) {
            CellI& instantiatedParam = instantiateTemplateParamType(membershipType, ret, inputParameters, state);
            instantiatedMemberOfs.add(instantiatedParam);
        });
        ret.set("memberOf", instantiatedMemberOfs);
    }

    return ret;
}

CellI& Ast::StructT::instantiateTemplateParamType(CellI& param, CellI& selfType, Map& inputParameters, CellI& state)
{
    if (&param.type() == &kb.std.ast.TemplateParam) {
        CellI& paramValue = param[kb.ids.role];
        if (!inputParameters.hasKey(paramValue)) {
            throw "Instantiating with unknown template parameter!";
        }
        return inputParameters.getValue(paramValue);
    }
    if (&param.type() == &kb.std.ast.TemplatedType) {
        List& resolvedParameterList = *new List(kb, kb.std.Slot);
        auto& ret                   = *new TemplatedType(kb, param[kb.ids.id], resolvedParameterList);
        auto& parametersList        = param[kb.ids.parameters];

        Visitor::visitList(parametersList, [this, &resolvedParameterList, &selfType, &inputParameters, &state](CellI& slot, int, bool&) {
            CellI& slotRole         = slot[kb.ids.slotRole];
            CellI& slotType         = slot[kb.ids.slotType];
            CellI& resolvedSlotType = instantiateTemplateParamType(slotType, selfType, inputParameters, state);
            resolvedParameterList.add(kb.ast.slot(slotRole, resolvedSlotType));
        });

        return ret;
    }
    if (&param.type() == &kb.std.ast.Cell || &param.type() == &kb.std.ast.StructName || &param.type() == &kb.std.ast.SubTypeName) {
        return param;
    }

    throw "Unknown template parameter!";
}

Ast::Base& Ast::StructT::instantiateAst(CellI& ast, CellI& selfType, Map& inputParameters, CellI& state)
{
    auto instantiate = [this, &selfType, &inputParameters, &state](CellI& ast) -> Ast::Base& { return instantiateAst(ast, selfType, inputParameters, state); };

    if (&ast.type() == &kb.std.ast.New) {
        auto* objectTypePtr = &ast[kb.ids.objectType];
        if (&(*objectTypePtr).type() == &kb.std.ast.TemplatedType) {
            CellI& resolvedObjectType = instantiateTemplateParamType(*objectTypePtr, selfType, inputParameters, state);
            objectTypePtr             = &resolvedObjectType;
        }
        auto& objectType = *static_cast<Base*>(objectTypePtr);
        if (ast.has("constructor")) {
            auto& constructor = ast[kb.ids.constructor];
            Ast::Base& ret    = kb.ast.new_(objectType, static_cast<Base&>(constructor));
            if (ast.has("parameters")) {
                auto& newParameters = *new cells::List(kb, kb.std.ast.Slot);
                Visitor::visitList(ast[kb.ids.parameters], [this, &newParameters, &instantiate](CellI& slot, int, bool&) {
                    newParameters.add(kb.ast.slot(slot[kb.ids.slotRole], instantiate(slot[kb.ids.slotType])));
                });
                ret.set("parameters", newParameters);
            }
            return ret;
        } else {
            return kb.ast.new_(objectType);
        }
    } else if (&ast.type() == &kb.std.ast.Call) {
        Ast::Base& ret = kb.ast.call(instantiate(ast[kb.ids.cell]), ast[kb.ids.method]);
        if (ast.has("parameters")) {
            // TODO process parameters
            ret.set("parameters", ast[kb.ids.parameters]);
        }
        return ret;
    } else if (&ast.type() == &kb.std.ast.StaticCall) {
        Ast::Base& ret = kb.ast.scall(instantiate(ast[kb.ids.cell]), ast[kb.ids.method]);
        if (ast.has("parameters")) {
            // TODO process parameters
            ret.set("parameters", ast[kb.ids.parameters]);
        }
        return ret;
    } 

    // do nothing just traverse and copy the AST nodes
    if (&ast.type() == &kb.std.ast.Block) {
        auto& instantiedAsts = *new cells::List(kb, kb.std.ast.Base);
        Visitor::visitList(ast[kb.ids.asts], [this, &instantiedAsts, &instantiate](CellI& ast, int, bool&) {
            instantiedAsts.add(instantiate(ast));
        });
        return *new Block(kb, instantiedAsts);
    } else if (&ast.type() == &kb.std.ast.Cell) {
        return kb.ast.cell(ast[kb.ids.value]);
    } else if (&ast.type() == &kb.std.ast.StructName) {
        auto& ret = kb.ast.structName(ast[kb.ids.value]);
        if (ast.has(kb.ids.scopes)) {
            ret.set(kb.ids.scopes, ast[kb.ids.scopes]);
        }
        return ret;
    } else if (&ast.type() == &kb.std.ast.SelfFn) {
        return kb.ast.selfFn();
    } else if (&ast.type() == &kb.std.ast.Self) {
        return kb.ast.self();
    } else if (&ast.type() == &kb.std.ast.Continue) {
        return kb.ast.continue_();
    } else if (&ast.type() == &kb.std.ast.Break) {
        return kb.ast.break_();
    } else if (&ast.type() == &kb.std.ast.Try) {
        return kb.ast.try_(instantiate(ast["tryBranch"]), instantiate(ast["catchBranch"]));
    } else if (&ast.type() == &kb.std.ast.Throw) {
        if (ast.has(kb.ids.value)) {
            return kb.ast.throw_(instantiate(ast[kb.ids.value]));
        } else {
            return kb.ast.throw_();
        }
    } else if (&ast.type() == &kb.std.ast.Parameter) {
        return kb.ast.parameter(ast[kb.ids.role]);
    } else if (&ast.type() == &kb.std.ast.Var) {
        return kb.ast.var(ast[kb.ids.role]);
    } else if (&ast.type() == &kb.std.ast.Delete) {
        return kb.ast.delete_(instantiate(ast[kb.ids.cell]));
    } else if (&ast.type() == &kb.std.ast.Set) {
        return kb.ast.set(instantiate(ast[kb.ids.cell]), instantiate(ast[kb.ids.role]), instantiate(ast[kb.ids.value]));
    } else if (&ast.type() == &kb.std.ast.Erase) {
        return kb.ast.erase(instantiate(ast[kb.ids.cell]), instantiate(ast[kb.ids.role]));
    } else if (&ast.type() == &kb.std.ast.If) {
        if (ast.has("else_")) {
            return kb.ast.if_(instantiate(ast[kb.ids.condition]), instantiate(ast[kb.ids.then]), instantiate(ast[kb.ids.else_]));
        } else {
            return kb.ast.if_(instantiate(ast[kb.ids.condition]), instantiate(ast[kb.ids.then]));
        }
    } else if (&ast.type() == &kb.std.ast.Do) {
        return kb.ast.do_(instantiate(ast[kb.ids.statement]), instantiate(ast[kb.ids.condition]));
    } else if (&ast.type() == &kb.std.ast.While) {
        return kb.ast.while_(instantiate(ast[kb.ids.condition]), instantiate(ast[kb.ids.statement]));
    } else if (&ast.type() == &kb.std.ast.And) {
        return kb.ast.and_(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.std.ast.Or) {
        return kb.ast.or_(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.std.ast.Not) {
        return kb.ast.not_(instantiate(ast[kb.ids.input]));
    } else if (&ast.type() == &kb.std.ast.Add) {
        return kb.ast.add(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.std.ast.Subtract) {
        return kb.ast.subtract(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.std.ast.Multiply) {
        return kb.ast.multiply(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.std.ast.Divide) {
        return kb.ast.divide(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.std.ast.LessThan) {
        return kb.ast.lessThan(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.std.ast.LessThanOrEqual) {
        return kb.ast.lessThanOrEqual(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.std.ast.GreaterThan) {
        return kb.ast.greaterThan(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.std.ast.GreaterThanOrEqual) {
        return kb.ast.greaterThanOrEqual(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.std.ast.Same) {
        return kb.ast.same(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.std.ast.NotSame) {
        return kb.ast.notSame(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.std.ast.Equal) {
        return kb.ast.equal(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.std.ast.NotEqual) {
        return kb.ast.notEqual(instantiate(ast[kb.ids.lhs]), instantiate(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.std.ast.Has) {
        return kb.ast.has(instantiate(ast[kb.ids.cell]), instantiate(ast[kb.ids.role]));
    } else if (&ast.type() == &kb.std.ast.Missing) {
        return kb.ast.missing(instantiate(ast[kb.ids.cell]), instantiate(ast[kb.ids.role]));
    } else if (&ast.type() == &kb.std.ast.Get) {
        return kb.ast.get(instantiate(ast[kb.ids.cell]), instantiate(ast[kb.ids.role]));
    } else if (&ast.type() == &kb.std.ast.Member) {
        return kb.ast.member(ast[kb.ids.role]);
    } else if (&ast.type() == &kb.std.ast.Return) {
        if (ast.has("value")) {
            return kb.ast.return_(static_cast<Base&>(instantiate(ast[kb.ids.value])));
        }
        return kb.ast.return_();
    }

    throw "Unknown AST to instantiate!";
}

Map& Ast::StructT::templateParams()
{
    if (missing("templateParams")) {
        throw "No templateParams!";
    } else {
        return static_cast<Map&>(get("templateParams"));
    }
}

Ast::Function::Function(brain::Brain& kb, CellI& name, const std::string& label) :
    BaseT<Function>(kb, kb.std.ast.Function, label)
{
    set("name", name);
    this->label(label);
}

Ast::Function::Function(brain::Brain& kb, const std::string& name) :
    BaseT<Function>(kb, kb.std.ast.Function, name)
{
    set("id", kb.pools.strings.get(name)[kb.ids.value]);
    label(name);
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
    set("code", block);
}

Ast::Function& Ast::Function::resolveTypes(CellI& state)
{
    Ast::Function& ret = *new Ast::Function(kb, get("name"), std::format("{}", label()));
    state.set("currentFn", ret);
    std::stringstream ss;

    if (has("static_")) {
        ret.set("static_", get("static_"));
        ss << "static ";
    }
    ss << "fn";
    if (has("structType")) {
        auto& structType = get("structType");
        ret.set("structType", structType);
        ss << std::format(" {}::{}(", structType.label(), get("name").label());
    } else {
        ss << std::format(" {}(", get("name").label());
    }

    if (has("parameters")) {
        Visitor::visitList(parameters(), [this, &state, &ret, &ss](CellI& param, int i, bool& stop) {
            CellI& paramId           = param[kb.ids.slotRole];
            CellI& paramType         = param[kb.ids.slotType];
            CellI& resolvedParamType = resolveType(paramType, state);
            CellI& compiledParamType = getCompiledTypeFromResolvedType(resolvedParamType);
            if (i > 0) {
                ss << ", ";
            }
            ss << std::format("{}: {}", paramId.label(), compiledParamType.label());
            ret.parameters(kb.ast.slot(paramId, resolvedParamType));
        });
    }
    ss << ")";
    if (has("returnType")) {
        ss << " -> ";
        CellI& retType = returnType();
        CellI& resolvedRetType = resolveType(retType, state);
        CellI& compiledRetType = getCompiledTypeFromResolvedType(resolvedRetType);
        ss << compiledRetType.label();
        ret.returnType(resolvedRetType);
    }
    ret.label(ss.str());

    CellI& resolvedCodeAst = resolveTypesInCode(state, code());
    ret.set("code", resolvedCodeAst);

    return ret;
}

Ast::Base& Ast::Function::resolveTypesInCode(CellI& resolveState, CellI& ast)
{
    auto resolveNode = [this, &resolveState](CellI& ast) -> Ast::Base& { return resolveTypesInCode(resolveState, ast); };

    if (&ast.type() == &kb.std.ast.New) {
        CellI& objectType                = ast[kb.ids.objectType];
        Ast::Base* resolvedObjectTypePtr = nullptr;
        if (&objectType.type() == &kb.std.ast.Member) {
            resolvedObjectTypePtr = &static_cast<Ast::Base&>(objectType);
        } else {
            resolvedObjectTypePtr = &resolveType(objectType, resolveState);
        }
        Ast::Base& resolvedObjectType = *resolvedObjectTypePtr;

        if (ast.has("constructor")) {
            auto& constructor = ast[kb.ids.constructor];
            Ast::Base& ret    = kb.ast.new_(resolvedObjectType, static_cast<Base&>(ast[kb.ids.constructor]));

            if (ast.has("parameters")) {
                auto& newParameters = *new cells::List(kb, kb.std.ast.Slot);
                Visitor::visitList(ast[kb.ids.parameters], [this, &newParameters, &resolveNode, &resolveState](CellI& slot, int, bool&) {
                    newParameters.add(kb.ast.slot(slot[kb.ids.slotRole], resolveNode(slot[kb.ids.slotType])));
                });
                ret.set("parameters", newParameters);
            }
            return ret;
        } else {
            return kb.ast.new_(resolvedObjectType);
        }
    }

    if (&ast.type() == &kb.std.ast.Call || &ast.type() == &kb.std.ast.StaticCall) {

        Ast::Base* retPtr = nullptr;
        if (&ast.type() == &kb.std.ast.Call) {
            retPtr = &kb.ast.call(resolveNode(ast[kb.ids.cell]), ast[kb.ids.method]);
        } else {
            retPtr = &kb.ast.scall(resolveType(ast[kb.ids.cell], resolveState), ast[kb.ids.method]);
        }
        Ast::Base& ret = *retPtr;
        if (ast.has("parameters")) {
            auto& newParameters = *new cells::List(kb, kb.std.ast.Slot);
            Visitor::visitList(ast[kb.ids.parameters], [this, &newParameters, &resolveNode, &resolveState](CellI& slot, int, bool&) {
                newParameters.add(kb.ast.slot(slot[kb.ids.slotRole], resolveNode(slot[kb.ids.slotType])));
            });
            ret.set("parameters", newParameters);
        }
        return ret;
    } else if (&ast.type() == &kb.std.ast.StructName) {
        return resolveType(ast, resolveState);
    }

    // do nothing just traverse and copy the AST nodes
    if (&ast.type() == &kb.std.ast.Block) {
        auto& instantiedAsts = *new cells::List(kb, kb.std.ast.Base);
        Visitor::visitList(ast[kb.ids.asts], [this, &instantiedAsts, &resolveNode](CellI& ast, int, bool&) {
            instantiedAsts.add(resolveNode(ast));
        });
        return *new Block(kb, instantiedAsts);
    } else if (&ast.type() == &kb.std.ast.Cell) {
        return kb.ast.cell(ast[kb.ids.value]);
    } else if (&ast.type() == &kb.std.ast.SelfFn) {
        return kb.ast.selfFn();
    } else if (&ast.type() == &kb.std.ast.Self) {
        return kb.ast.self();
    } else if (&ast.type() == &kb.std.ast.Continue) {
        return kb.ast.continue_();
    } else if (&ast.type() == &kb.std.ast.Break) {
        return kb.ast.break_();
    } else if (&ast.type() == &kb.std.ast.Try) {
        return kb.ast.try_(resolveNode(ast["tryBranch"]), resolveNode(ast["catchBranch"]));
    } else if (&ast.type() == &kb.std.ast.Throw) {
        if (ast.has(kb.ids.value)) {
            return kb.ast.throw_(resolveNode(ast[kb.ids.value]));
        } else {
            return kb.ast.throw_();
        }
    } else if (&ast.type() == &kb.std.ast.Parameter) {
        return kb.ast.parameter(ast[kb.ids.role]);
    } else if (&ast.type() == &kb.std.ast.Var) {
        return kb.ast.var(ast[kb.ids.role]);
    } else if (&ast.type() == &kb.std.ast.Delete) {
        return kb.ast.delete_(resolveNode(ast[kb.ids.cell]));
    } else if (&ast.type() == &kb.std.ast.Set) {
        return kb.ast.set(resolveNode(ast[kb.ids.cell]), resolveNode(ast[kb.ids.role]), resolveNode(ast[kb.ids.value]));
    } else if (&ast.type() == &kb.std.ast.Erase) {
        return kb.ast.erase(resolveNode(ast[kb.ids.cell]), resolveNode(ast[kb.ids.role]));
    } else if (&ast.type() == &kb.std.ast.If) {
        if (ast.has("else_")) {
            return kb.ast.if_(resolveNode(ast[kb.ids.condition]), resolveNode(ast[kb.ids.then]), resolveNode(ast[kb.ids.else_]));
        } else {
            return kb.ast.if_(resolveNode(ast[kb.ids.condition]), resolveNode(ast[kb.ids.then]));
        }
    } else if (&ast.type() == &kb.std.ast.Do) {
        return kb.ast.do_(resolveNode(ast[kb.ids.statement]), resolveNode(ast[kb.ids.condition]));
    } else if (&ast.type() == &kb.std.ast.While) {
        return kb.ast.while_(resolveNode(ast[kb.ids.condition]), resolveNode(ast[kb.ids.statement]));
    } else if (&ast.type() == &kb.std.ast.And) {
        return kb.ast.and_(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.std.ast.Or) {
        return kb.ast.or_(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.std.ast.Not) {
        return kb.ast.not_(resolveNode(ast[kb.ids.input]));
    } else if (&ast.type() == &kb.std.ast.Add) {
        return kb.ast.add(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.std.ast.Subtract) {
        return kb.ast.subtract(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.std.ast.Multiply) {
        return kb.ast.multiply(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.std.ast.Divide) {
        return kb.ast.divide(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.std.ast.LessThan) {
        return kb.ast.lessThan(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.std.ast.LessThanOrEqual) {
        return kb.ast.lessThanOrEqual(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.std.ast.GreaterThan) {
        return kb.ast.greaterThan(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.std.ast.GreaterThanOrEqual) {
        return kb.ast.greaterThanOrEqual(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.std.ast.Same) {
        return kb.ast.same(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.std.ast.NotSame) {
        return kb.ast.notSame(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.std.ast.Equal) {
        return kb.ast.equal(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.std.ast.NotEqual) {
        return kb.ast.notEqual(resolveNode(ast[kb.ids.lhs]), resolveNode(ast[kb.ids.rhs]));
    } else if (&ast.type() == &kb.std.ast.Has) {
        return kb.ast.has(resolveNode(ast[kb.ids.cell]), resolveNode(ast[kb.ids.role]));
    } else if (&ast.type() == &kb.std.ast.Missing) {
        return kb.ast.missing(resolveNode(ast[kb.ids.cell]), resolveNode(ast[kb.ids.role]));
    } else if (&ast.type() == &kb.std.ast.Get) {
        return kb.ast.get(resolveNode(ast[kb.ids.cell]), resolveNode(ast[kb.ids.role]));
    } else if (&ast.type() == &kb.std.ast.Member) {
        return kb.ast.member(ast[kb.ids.role]);
    } else if (&ast.type() == &kb.std.ast.Return) {
        if (ast.has("value")) {
            return kb.ast.return_(static_cast<Base&>(resolveNode(ast[kb.ids.value])));
        }
        return kb.ast.return_();
    }

    throw "Unknown AST to instantiate!";
}

CellI& Ast::Function::compile(CellI& state)
{
    // TODO hack!
    // find a better way to create type during compilation
    cells::Object& functionType = *new cells::Object(kb, kb.std.Type_);
    functionType.set("memberOf", kb.map(kb.std.Type_, kb.std.Type_, kb.std.op.Function, kb.std.op.Function));
    cells::Map& subTypesMap = kb.map(kb.std.Cell, kb.std.Type_,
                                     kb.ids.name, get("name"));
    functionType.set("subTypes", subTypesMap);

    CellI* map = &kb.slots(
        kb.std.slot("ast", kb.std.ast.Base),
        kb.std.slot("stack", kb.std.Stack),
        kb.std.slot("op", kb.ListOf(kb.std.op.Base)),
        kb.std.slot("static_", kb.std.Boolean));
    functionType.set("slots", *map);

    cells::Object& function = *new cells::Object(kb, functionType);
    compileParams(function, subTypesMap, state);
    functionType.label(std::format("Type for {}", function.label()));
    function.set("ast", *this);
    function.set("op", compileAst(code(), function, state));
    if (has("static_")) {
        function.set("static_", get("static_"));
    }

    return function;
}

std::string Ast::Function::shortName()
{
    std::stringstream iss;
    std::stringstream oss;
    if (has("parameters") || has("structType")) {
        if (has("parameters")) {
            Visitor::visitList(parameters(), [this, &iss](CellI& slot, int i, bool& stop) {
                if (i > 0) {
                    iss << ", ";
                }
                iss << "p_" << slot[kb.ids.slotRole].label() << ": " << getCompiledTypeFromResolvedType(slot[kb.ids.slotType]).label();
            });
        }
    }
    if (has("returnType")) {
        oss << getCompiledTypeFromResolvedType(returnType()).label();
    }
    if (has("returnType")) {
        return std::format("fn {}({}) -> {}", get("name").label(), iss.str(), oss.str());
    } else {
        return std::format("fn {}({})", get("name").label(), iss.str());
    }
}

CellI& Ast::Function::getFullId()
{
    if (has("fullId")) {
        return get("fullId");
    }

    List& fullId = *new List(kb, kb.std.Cell);
    if (has("structType")) {
        Struct& structType = static_cast<Struct&>(get("structType"));
        Visitor::visitList(structType.getFullId(), [this, &fullId](CellI& character, int i, bool& stop) {
            fullId.add(character);
        });
    } else {
        Scope& scope       = static_cast<Scope&>(get("scope"));
        CellI& scopeFullId = scope.getFullId();

        Visitor::visitList(scopeFullId, [this, &fullId](CellI& character, int i, bool& stop) {
            fullId.add(character);
        });
    }
    if (!fullId.empty()) {
        fullId.add(kb.pools.chars.get(':'));
        fullId.add(kb.pools.chars.get(':'));
    }
    Visitor::visitList(get(kb.ids.name), [this, &fullId](CellI& character, int i, bool& stop) {
        fullId.add(character);
    });
    std::stringstream ss;
    Visitor::visitList(fullId, [this, &fullId, &ss](CellI& character, int i, bool& stop) {
        ss << character.label();
    });
    fullId.label(ss.str());

    return fullId;
}

void Ast::Function::compileParams(cells::Object& function, cells::Map& subTypesMap, CellI& state)
{
    std::stringstream iss;
    std::stringstream oss;
    std::string structTypeStr;
    if (has("parameters") || has("structType")) {
        cells::Object& parametersType = *new cells::Object(kb, kb.std.Type_);
        Map& slots                    = *new Map(kb, kb.std.Cell, kb.std.Slot);
        if (has("structType")) {
            CellI& type = get("structType");
            Object& var = *new Object(kb, kb.std.op.Var, "self");
            var.set("valueType", type);
            slots.add(kb.id("self"), kb.std.slot("self", type));
            structTypeStr = std::format("{}::", type.label());
        }
        if (has("parameters")) {
            Visitor::visitList(parameters(), [this, &slots, &iss](CellI& slot, int i, bool& stop) {
                if (i > 0) {
                    iss << ", ";
                }
                auto& slotRole         = slot[kb.ids.slotRole];
                auto& slotType         = slot[kb.ids.slotType];
                auto& compiledSlotType = getCompiledTypeFromResolvedType(slotType);
                iss << "p_" << slotRole.label() << ": " << compiledSlotType.label();
                slots.add(slotRole, kb.std.slot(slotRole, compiledSlotType));
            });
        }
        parametersType.set("slots", slots);
        subTypesMap.add(kb.id("parameters"), parametersType);
    }
    if (has("returnType")) {
        auto& asrReturnType      = returnType();
        auto& compiledReturnType = getCompiledTypeFromResolvedType(asrReturnType);
        oss << compiledReturnType.label();
        subTypesMap.add(kb.id("returnType"), compiledReturnType);
    }
    if (has("returnType")) {
        function.label(std::format("fn {}{}({}) -> {}", structTypeStr, get("name").label(), iss.str(), oss.str()));
    } else {
        function.label(std::format("fn {}{}({})", structTypeStr, get("name").label(), iss.str()));
    }
}

CellI& Ast::Function::compileAst(CellI& ast, cells::Object& function, CellI& state)
{
    auto compile = [this, &function, &state](CellI& ast) -> CellI& { return compileAst(ast, function, state); };
    const auto _ = [this](auto& cell) -> Ast::Cell& { return kb._(cell); };

    if (&ast.type() == &kb.std.ast.Block) {
        CellI& list        = ast[kb.ids.asts];
        CellI* prevBlock = nullptr;
        if (state.has("lastBlock")) {
            prevBlock = &state["lastBlock"];
        }
        auto& compiledAsts = *new cells::List(kb, kb.std.op.Base);
        Object& opBlock    = *new Object(kb, kb.std.op.Block);
        state.set("lastBlock", opBlock);
        Visitor::visitList(list, [this, &compile, &compiledAsts, &ast, &function](CellI& ast, int, bool&) {
            compiledAsts.add(compile(ast));
        });
        opBlock.set("ast", ast);
        opBlock.set("ops", compiledAsts);

        if (prevBlock) {
            state.set("lastBlock", *prevBlock);
        } else {
            state.erase("lastBlock");
        }

        return opBlock;
    } else if (&ast.type() == &kb.std.ast.Cell) {
        Object& constVar = *new Object(kb, kb.std.op.ConstVar);
        constVar.set("ast", ast);
        constVar.set("value", ast[kb.ids.value]);
        return constVar;
    } else if (&ast.type() == &kb.std.ast.ResolvedType) {
        Object& constVar = *new Object(kb, kb.std.op.ConstVar);
        constVar.set("ast", ast);
        constVar.set("value", ast[kb.ids.compiled]);
        return constVar;
    } else if (&ast.type() == &kb.std.ast.SelfFn) {
        Object& constVar = *new Object(kb, kb.std.op.ConstVar);
        constVar.set("ast", ast);
        constVar.set("value", function);
        return constVar;
    } else if (&ast.type() == &kb.std.ast.Self) {
        CellI& retOp = compile(kb.ast.get(_(function), _("stack")) / "value" / "input" / "self");
        retOp.set("ast", ast);
        retOp.label("self");
        return retOp;
    } else if (&ast.type() == &kb.std.ast.Continue) {
        if (state.missing("lastBlock")) {
            throw "No statement to break!";
        }
        CellI& lastBlock = state["lastBlock"];
        CellI& retOp     = compile(kb.ast.set(_(lastBlock), "status", _(kb.ids.continue_)));
        return retOp;
    } else if (&ast.type() == &kb.std.ast.Break) {
        if (state.missing("lastBlock")) {
            throw "No statement to break!";
        }
        CellI& lastBlock = state["lastBlock"];
        CellI& retOp     = compile(kb.ast.set(_(lastBlock), "status", _(kb.ids.break_)));
        return retOp;
    } else if (&ast.type() == &kb.std.ast.Throw) {
        if (state.missing("lastBlock")) {
            throw "No statement to break!";
        }
        CellI& lastBlock = state["lastBlock"];
        CellI& retOp     = compile(kb.ast.set(_(lastBlock), "status", _(kb.ids.throw_)));
        if (ast.has("value")) {
            retOp.set("result", compile(kb.ast.set(_(lastBlock), "value", static_cast<Ast::Base&>(ast[kb.ids.value]))));
        }
        return retOp;
    } else if (&ast.type() == &kb.std.ast.Parameter) {
        CellI& retOp = compile(kb.ast.get(_(function), "stack") / "value" / "input" / _(ast[kb.ids.role]));
        retOp.set("ast", ast);
        return retOp;
    } else if (&ast.type() == &kb.std.ast.Member) {
        CellI& member = compile(kb.ast.get(kb.ast.self(), kb.ast.cell(ast[kb.ids.role])));
        member.set("ast", ast);
        return member;
    } else if (&ast.type() == &kb.std.ast.Return) {
        Object& retOp = *new Object(kb, kb.std.op.Return, "op.return");
        retOp.set("ast", ast);
        if (ast.has("value")) {
            retOp.set("result", compile(kb.ast.set(kb.ast.get(_(function), "stack") / "value" / "output", "value", static_cast<Ast::Base&>(ast[kb.ids.value]))));
        }
        return retOp;
    } else if (&ast.type() == &kb.std.ast.Var) {
        if (function.type()[kb.ids.subTypes][kb.ids.index].missing("localVars")) {
            cells::Object& functionLocalVarsType = *new cells::Object(kb, kb.std.Type_, std::format("LocalVarsType of {}", function.label()));
            functionLocalVarsType.set("memberOf", kb.map(kb.std.Type_, kb.std.Type_, kb.std.Index, kb.std.Index));
            static_cast<Map&>(function.type()[kb.ids.subTypes]).add(kb.id("localVars"), functionLocalVarsType);
        }
        CellI& localVarsType = function.type()[kb.ids.subTypes][kb.ids.index][kb.ids.localVars][kb.ids.value];
        if (localVarsType.missing("slots")) {
            localVarsType.set("slots", *new Map(kb, kb.std.Cell, kb.std.Slot));
        }
        auto& slotsMap = static_cast<Map&>(localVarsType[kb.ids.slots]);
        if (!slotsMap.hasKey(ast[kb.ids.role])) {
            slotsMap.add(ast[kb.ids.role], kb.std.slot(ast[kb.ids.role], kb.std.op.Var));
        }
        CellI& retOp = compile(kb.ast.get(_(function), _("stack")) / "value" / "localVars" / _(ast[kb.ids.role]));
        retOp.set("ast", ast);
        return retOp;
    } else if (&ast.type() == &kb.std.ast.Delete) {
        Object& retOp = *new Object(kb, kb.std.op.Delete);
        retOp.set("ast", ast);
        retOp.set("input", compile(ast[kb.ids.cell]));
        return retOp;
    } else if (&ast.type() == &kb.std.ast.Set) {
        Object& retOp = *new Object(kb, kb.std.op.Set);
        retOp.set("ast", ast);
        retOp.set("cell", compile(ast[kb.ids.cell]));
        retOp.set("role", compile(ast[kb.ids.role]));
        retOp.set("value", compile(ast[kb.ids.value]));
        return retOp;
    } else if (&ast.type() == &kb.std.ast.Erase) {
        Object& retOp = *new Object(kb, kb.std.op.Erase);
        retOp.set("ast", ast);
        retOp.set("cell", compile(ast[kb.ids.cell]));
        retOp.set("role", compile(ast[kb.ids.role]));
        return retOp;
    } else if (&ast.type() == &kb.std.ast.If) {
        Object& retOp = *new Object(kb, kb.std.op.If);
        retOp.set("ast", ast);
        retOp.set("condition", compile(ast[kb.ids.condition]));
        retOp.set("then", compile(ast[kb.ids.then]));
        if (ast.has("else_")) {
            retOp.set("else_", compile(ast[kb.ids.else_]));
            return retOp;
        } else {
            return retOp;
        }
    } else if (&ast.type() == &kb.std.ast.Do) {
        Object& retOp = *new Object(kb, kb.std.op.Do);
        retOp.set("ast", ast);
        retOp.set("condition", compile(ast[kb.ids.condition]));
        retOp.set("statement", compile(ast[kb.ids.statement]));
        return retOp;
    } else if (&ast.type() == &kb.std.ast.While) {
        Object& retOp = *new Object(kb, kb.std.op.While);
        retOp.set("ast", ast);
        retOp.set("condition", compile(ast[kb.ids.condition]));
        retOp.set("statement", compile(ast[kb.ids.statement]));
        return retOp;
    } else if (&ast.type() == &kb.std.ast.New) {
        auto& compiledAsts = *new cells::List(kb, kb.std.op.Base);
        auto& astObjectType = ast[kb.ids.objectType];

        Object& block = *new Object(kb, kb.std.op.Block);
        block.set("ast", ast);
        block.set("ops", compiledAsts);

        Object& opSet = *new Object(kb, kb.std.op.Set, "New { block.value = new objectType(); }");
        opSet.set("ast", ast);
        opSet.set("cell", compile(kb.ast.cell(block)));
        opSet.set("role", compile(kb.ast.cell(kb.id("value"))));

        Object& opNew = *new Object(kb, kb.std.op.New);
        opNew.set("ast", ast);
        opNew.set("objectType", compile(astObjectType));
        opSet.set("value", opNew);

        compiledAsts.add(opSet);
        if (ast.has("constructor")) {
            Object& callAst = *new Object(kb, kb.std.ast.Call);
            callAst.set("cell", kb.ast.get(kb.ast.cell(block), kb.ast.cell(kb.id("value"))));
            callAst.set("method", ast[kb.ids.constructor]);
            if (ast.has("parameters")) {
                callAst.set("parameters", ast[kb.ids.parameters]);
            }
            CellI& callConstructor = compile(callAst);
            callConstructor.label("New { call constructor; }");
            compiledAsts.add(callConstructor);
        }
        return block;
    } else if (&ast.type() == &kb.std.ast.Call || &ast.type() == &kb.std.ast.StaticCall) {
/*
block {
    var_method.value  = m_list.type.methods.index.add;
    var_method.value.stack = fn Map::add(self: Map, in_key: Cell, in_value: Cell).stack;
    var_newStackItem.value = new ListItem;
    var_newStackFrame.value = new StackFrame;
    var_newStackItem.value.value = var_newStackFrame.value;
    var_newStackItem.value.previous = var_method.value.stack;
    var_inputIndex.value = new Index;
    if var_method.value.type.subTypes.index has localVars then {
        var_localVars.value = new var_method.value.type.subTypes.index.localVars;
        var_newStackFrame.value.localVars = var_localVars.value;
        var_localVarsListItem.value = var_method.value.type.subTypes.index.localVars.slots.list.first;
        do {
            var_localVarsListItemHasNext.value = true;
            var_localVars.value.var_localVarsListItem.value.value.slotRole = new op::Var;
            if var_localVarsListItem.value has next then
                var_localVarsListItem.value = var_localVarsListItem.value.next;
             else
                var_localVarsListItemHasNext.value = false;
        } (var_localVarsListItemHasNext.value is true);
    };
    var_newStackFrame.value.input = var_inputIndex.value;
    if var_method.value.type.subTypes.index.output has slots then
        if var_method.value.type.subTypes.index.output.slots.index has value then {
        var_newStackFrame.value.output = new var_method.value.type.subTypes.index.output;
        var_newStackFrame.value.output.value = new op::Var;
    };
    var_inputIndex.value.self = m_list;
    var_inputIndex.value.value = in_value;
    var_method.value.stack = var_newStackItem.value;
    eval(var_method);
    if var_method.value.type.subTypes.index.output has slots then
        if var_method.value.type.subTypes.index.output.slots.index has value then
            block.value = var_method.value.stack.value.output.value.value;
    var_method.value.stack = var_method.value.stack.previous;
 }
*/
        CellI* prevBlock = nullptr;
        if (state.has("lastBlock")) {

            prevBlock = &state["lastBlock"];
            state.erase("lastBlock");
        }
        const char* blockName  = &ast.type() == &kb.std.ast.Call ? "Call { ... }" : "SCall { ... }";
        Ast::Base& astCell     = static_cast<Ast::Base&>(ast[kb.ids.cell]);
        Ast::Base& astMethod   = static_cast<Ast::Base&>(ast[kb.ids.method]);
        auto& astMethodId      = astMethod[kb.ids.value];
#if 1
        bool checked = false;
        // New<T>.constructor(...)
        if (&astCell.type() == &kb.std.ast.Get && &astCell[kb.ids.cell].type() == &kb.std.ast.Cell && &astCell[kb.ids.cell][kb.ids.value].type() == &kb.std.op.Block) {
            auto& opBlock = astCell[kb.ids.cell][kb.ids.value];
            auto& opBlockAst = opBlock[kb.ids.ast];
            if (&opBlockAst.type() == &kb.std.ast.New) {
                auto& astObjectTypeStruct = opBlockAst[kb.ids.objectType];
                checkMethodCall(astObjectTypeStruct, astMethodId, state);
                checked = true;
            }
        // var_xxx.call(...)
        } else if (&astCell.type() == &kb.std.ast.Get && &astCell[kb.ids.cell].type() == &kb.std.ast.Var) {
            auto& astVar = astCell[kb.ids.cell];
            std::cout << "In " << astVar.label() << " ";
        // m_xxx.method(...)
        } else if (&astCell.type() == &kb.std.ast.Member) {
            auto& astMemberId = astCell[kb.ids.role];
            auto& astMembersType = function[kb.ids.ast][kb.ids.structType];
            if (&astMemberId == &kb.ids.type) {
                // std::cout << "DDDD " << astMembersType.label();
                auto& stdScope = static_cast<Scope&>(state[kb.ids.globalScope]).getScope("std");
                auto& type     = stdScope.getStruct("Type");
                checkMethodCall(type, astMethodId, state);
                checked        = true;
            } else {
                auto& astMembers = static_cast<Map&>(function[kb.ids.ast][kb.ids.structType][kb.ids.members]);
                if (astMembers.hasKey(astMemberId)) {
                    Slot& slot = static_cast<Slot&>(astMembers.getValue(astMemberId));
                    auto& slotType = slot[kb.ids.slotType];
                    checkMethodCall(slotType, astMethodId, state);
                    checked = true;
                } else {
                    throw "Unknown member name!";
                }
            }
        } else if (&astCell.type() == &kb.std.ast.Self) {
            auto& astSelfType = function[kb.ids.ast][kb.ids.structType];
            checkMethodCall(astSelfType, astMethodId, state);
            checked = true;
        } else if (&astCell.type() == &kb.std.ast.Parameter) {
            auto& parameterRole         = astCell[kb.ids.role];
            auto& astFunctionParameters = function[kb.ids.ast][kb.ids.parameters];
            Slot* astFunctionParameterSlot = nullptr;
            Visitor::visitList(astFunctionParameters, [this, &parameterRole, &astFunctionParameterSlot](CellI& slot, int i, bool& stop) {
                CellI& slotRole = slot[kb.ids.slotRole];
                CellI& slotType = slot[kb.ids.slotType];
                if (&slotRole == &parameterRole) {
                    astFunctionParameterSlot = &static_cast<Slot&>(slot);
                    stop = true;
                    return;
                }
            });
            if (astFunctionParameterSlot) {
                Slot& slot             = *astFunctionParameterSlot;
                auto& astParameterType = slot[kb.ids.slotType];
                checkMethodCall(astParameterType, astMethodId, state);
                checked = true;
            } else {
                throw "Unknown parameter name!";
            }
        }
        if (!checked) {
            std::cout << "Unchecked method call " << astMethodId.label() << " in " << function.label() << std::endl;
        }
#endif
        Ast::Get* getMethodPtr = nullptr;
        if (&ast.type() == &kb.std.ast.Call) {
            getMethodPtr = &(kb.ast.get(astCell, _("type")) / "methods");
        } else {
            getMethodPtr = &(kb.ast.get(astCell, _("methods")));
        }
        Ast::Get& getMethod = (*getMethodPtr) / "index" / astMethod / "value";
        auto& compiledAsts  = *new cells::List(kb, kb.std.op.Base);
        Object& block       = *new Object(kb, kb.std.op.Block, blockName);
        block.set("ast", ast);
        block.set("ops", compiledAsts);
        Object& varMethod = *new Object(kb, kb.std.op.Var, "Call { var_method; }");
        varMethod.set("valueType", kb.std.op.Function);
        CellI& storeMethod     = compile(kb.ast.set(_(varMethod), _("value"), getMethod));
        CellI& setCurrentStack = compile(kb.ast.set(_(varMethod) / "value", _("stack"), kb.ast.get(_(function), _("stack"))));

        Object& varNewStackItem = *new Object(kb, kb.std.op.Var, "Call { var_newStackItem; }");
        varNewStackItem.set("valueType", kb.std.ListItem);
        Object& varNewStackFrame = *new Object(kb, kb.std.op.Var, "Call { var_newStackFrame; }");
        varNewStackFrame.set("valueType", kb.std.StackFrame);

        Object& varInputIndex = *new Object(kb, kb.std.Index, "Call { var_inputIndex; }");
        Object& varLocalVars = *new Object(kb, kb.std.op.Var, "Call { var_localVars; }");
        varLocalVars.set("valueType", kb.std.Index);
        Object& varLocalVarsList = *new Object(kb, kb.std.op.Var, "Call { var_localVarsList; }");
        varLocalVarsList.set("valueType", kb.std.List);
        Object& varLocalVarsListItem = *new Object(kb, kb.std.op.Var, "Call { var_localVarsListItem; }");
        varLocalVarsListItem.set("valueType", kb.std.ListItem);
        Object& varLocalVarsListItemHasNext = *new Object(kb, kb.std.op.Var, "Call { var_localVarsListItemHasNext; }");
        varLocalVarsListItemHasNext.set("valueType", kb.std.Boolean);

        CellI& storeStackItem     = compile(kb.ast.set(_(varNewStackItem), _("value"), kb.ast.new_(_(kb.std.ListItem))));
        CellI& storeStackFrame    = compile(kb.ast.set(_(varNewStackFrame), _("value"), kb.ast.new_(_(kb.std.StackFrame))));
        CellI& setListItem        = compile(kb.ast.set(_(varNewStackItem) / "value", _("value"), _(varNewStackFrame) / "value"));
        CellI& setListItemPrev    = compile(kb.ast.set(_(varNewStackItem) / "value", _("previous"), _(varMethod) / "value" / "stack"));
        CellI& setListItemNext    = compile(kb.ast.set(_(varMethod) / "value" / "stack", _("next"), _(varNewStackItem) / "value"));
        CellI& storeInputIndex    = compile(kb.ast.set(_(varInputIndex), _("value"), kb.ast.new_(_(kb.std.Index))));
        CellI& setMethod          = compile(kb.ast.set(_(varNewStackFrame) / "value", _("method"), _(varMethod) / "value"));

        CellI& createNewLocalVars = compile(
            kb.ast.if_(kb.ast.has(_(varMethod) / "value" / "type" / "subTypes" / "index", _("localVars")),
                       kb.ast.block(
                           kb.ast.set(_(varLocalVars), _("value"), kb.ast.new_(_(varMethod) / "value" / "type" / "subTypes" / "index" / "localVars" / "value")),
                           kb.ast.set(_(varNewStackFrame) / "value", _("localVars"), _(varLocalVars) / "value"),
                           kb.ast.set(_(varLocalVarsListItem), _("value"), _(varMethod) / "value" / "type" / "subTypes" / "index" / "localVars" / "value" / "slots" / "list" / "first"),
                           kb.ast.do_(kb.ast.block(
                                          kb.ast.set(_(varLocalVarsListItemHasNext), _("value"), _(kb.boolean.true_)),
                                          kb.ast.set(_(varLocalVars) / "value", _(varLocalVarsListItem) / "value" / "value" / "slotRole", kb.ast.new_(_(kb.std.op.Var))),
                                          kb.ast.if_(kb.ast.has(_(varLocalVarsListItem) / _("value"), _("next")),
                                                     kb.ast.set(_(varLocalVarsListItem), _("value"), _(varLocalVarsListItem) / "value" / "next"),
                                                     kb.ast.set(_(varLocalVarsListItemHasNext), _("value"), _(kb.boolean.false_)))),
                                      kb.ast.same(_(varLocalVarsListItemHasNext) / "value", _(kb.boolean.true_))))));

        CellI& setInput      = compile(kb.ast.set(_(varNewStackFrame) / "value", _("input"), _(varInputIndex) / "value"));
        CellI& setSelf       = compile(kb.ast.set(_(varInputIndex) / "value", _("self"), astCell));
        CellI& setStackNext  = compile(kb.ast.set(_(varMethod) / "value" / "stack" / "previous", "next", _(varNewStackItem) / "value"));
        CellI& setStackToNew = compile(kb.ast.set(_(varMethod) / "value", "stack", _(varNewStackItem) / "value"));
        CellI& setRetValue   = compile(kb.ast.if_(kb.ast.has(_(varMethod) / "value" / "type" / "subTypes" / "index", _("returnType")),
                                                  kb.ast.block(kb.ast.set(_(varNewStackFrame) / "value", _("output"), kb.ast.new_(_(kb.std.op.Var))),
                                                               kb.ast.set(_(varNewStackFrame) / "value" / "output", _("valueType"), _(varMethod) / "value" / "type" / "subTypes" / "index" / "returnType" / "value"))));
        CellI& getResult     = compile(kb.ast.if_(kb.ast.has(_(varMethod) / _("value") / _("type") / _("subTypes") / _("index"), _("returnType")),
                                                  kb.ast.set(_(block), _("value"), _(varMethod) / "value" / "stack" / "value" / "output" / "value")));
        CellI& setStackToOld = compile(kb.ast.set(_(varMethod) / "value" / "stack" / "previous" / "value" / "method", _("stack"), _(varMethod) / "value" / "stack" / "previous"));

        compiledAsts.add(storeMethod);
        compiledAsts.add(setCurrentStack);
        compiledAsts.add(storeStackItem);
        compiledAsts.add(storeStackFrame);
        compiledAsts.add(setListItem);
        compiledAsts.add(setListItemPrev);
        compiledAsts.add(setListItemNext);
        compiledAsts.add(storeInputIndex);
        compiledAsts.add(createNewLocalVars);
        compiledAsts.add(setMethod);
        compiledAsts.add(setInput);
        compiledAsts.add(setRetValue);
        compiledAsts.add(setSelf);


        if (ast.has("parameters")) {
            Visitor::visitList(ast[kb.ids.parameters], [this, &compiledAsts, &compile, &varInputIndex, &_](CellI& param, int, bool&) {
                Ast::Base& paramRole  = static_cast<Ast::Base&>(param[kb.ids.slotRole]);
                Ast::Base& paramValue = static_cast<Ast::Base&>(param[kb.ids.slotType]);
                CellI& setParam       = compile(kb.ast.set(_(varInputIndex) / "value", _(paramRole), paramValue));
                setParam.label("Call { setParam; }");
                compiledAsts.add(setParam);
            });
        }
        CellI& evalMethod = *new Object(kb, kb.std.op.EvalVar, std::format("{}::Call {{ evalVar; }}", function.label()));
        evalMethod.set("value", varMethod);
        compiledAsts.add(setStackNext);
        compiledAsts.add(setStackToNew);
        compiledAsts.add(evalMethod);
        compiledAsts.add(getResult);
        compiledAsts.add(setStackToOld);

        getMethod.label("Call { getMethod; }");
        storeMethod.label("Call { storeMethod; }");
        setCurrentStack.label("Call  { setCurrentStack; }");
        storeStackItem.label("Call { storeStackFrame; }");
        storeStackFrame.label("Call { storeStackFrame; }");
        setListItem.label("Call { setListItem; }");
        setListItemPrev.label("Call { setListItemPrev; }");
        setListItemPrev.label("Call { setListItemNext; }");
        storeInputIndex.label("Call { storeInputIndex; }");
        createNewLocalVars.label("Call { createNewLocalVars; }");
        setMethod.label("Call { setMethod; }");
        setInput.label("Call { setInput; }");
        setRetValue.label("Call { setRetValue; }");
        setSelf.label("Call { setSelf; }");
        setStackNext.label("Call { setStackNext; }");
        setStackToNew.label("Call { setStackToNew; }");
        getResult.label("Call { getResult; }");
        setStackToOld.label("Call { setStackToOld; }");

        if (prevBlock) {
            state.set("lastBlock", *prevBlock);
        }

        return block;
    } else if (&ast.type() == &kb.std.ast.And) {
        Object& retOp = *new Object(kb, kb.std.op.And);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast[kb.ids.lhs]));
        retOp.set("rhs", compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.std.ast.Or) {
        Object& retOp = *new Object(kb, kb.std.op.Or);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast[kb.ids.lhs]));
        retOp.set("rhs", compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.std.ast.Not) {
        Object& retOp = *new Object(kb, kb.std.op.Not);
        retOp.set("ast", ast);
        retOp.set("input", compile(ast[kb.ids.input]));
        return retOp;
    } else if (&ast.type() == &kb.std.ast.Add) {
        Object& retOp = *new Object(kb, kb.std.op.Add);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast[kb.ids.lhs]));
        retOp.set("rhs", compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.std.ast.Subtract) {
        Object& retOp = *new Object(kb, kb.std.op.Subtract);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast[kb.ids.lhs]));
        retOp.set("rhs", compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.std.ast.Multiply) {
        Object& retOp = *new Object(kb, kb.std.op.Multiply);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast[kb.ids.lhs]));
        retOp.set("rhs", compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.std.ast.Divide) {
        Object& retOp = *new Object(kb, kb.std.op.Divide);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast[kb.ids.lhs]));
        retOp.set("rhs", compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.std.ast.LessThan) {
        Object& retOp = *new Object(kb, kb.std.op.LessThan);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast[kb.ids.lhs]));
        retOp.set("rhs", compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.std.ast.LessThanOrEqual) {
        Object& retOp = *new Object(kb, kb.std.op.LessThanOrEqual);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast[kb.ids.lhs]));
        retOp.set("rhs", compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.std.ast.GreaterThan) {
        Object& retOp = *new Object(kb, kb.std.op.GreaterThan);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast[kb.ids.lhs]));
        retOp.set("rhs", compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.std.ast.GreaterThanOrEqual) {
        Object& retOp = *new Object(kb, kb.std.op.GreaterThanOrEqual);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast[kb.ids.lhs]));
        retOp.set("rhs", compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.std.ast.Same) {
        Object& retOp = *new Object(kb, kb.std.op.Same);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast[kb.ids.lhs]));
        retOp.set("rhs", compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.std.ast.NotSame) {
        Object& retOp = *new Object(kb, kb.std.op.NotSame);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast[kb.ids.lhs]));
        retOp.set("rhs", compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.std.ast.Equal) {
        Object& retOp = *new Object(kb, kb.std.op.Equal);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast[kb.ids.lhs]));
        retOp.set("rhs", compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.std.ast.NotEqual) {
        Object& retOp = *new Object(kb, kb.std.op.NotEqual);
        retOp.set("ast", ast);
        retOp.set("lhs", compile(ast[kb.ids.lhs]));
        retOp.set("rhs", compile(ast[kb.ids.rhs]));
        return retOp;
    } else if (&ast.type() == &kb.std.ast.Has) {
        Object& retOp = *new Object(kb, kb.std.op.Has);
        retOp.set("ast", ast);
        retOp.set("cell", compile(ast[kb.ids.cell]));
        retOp.set("role", compile(ast[kb.ids.role]));
        return retOp;
    } else if (&ast.type() == &kb.std.ast.Missing) {
        Object& retOp = *new Object(kb, kb.std.op.Missing);
        retOp.set("ast", ast);
        retOp.set("cell", compile(ast[kb.ids.cell]));
        retOp.set("role", compile(ast[kb.ids.role]));
        return retOp;
    } else if (&ast.type() == &kb.std.ast.Get) {
        Object& retOp = *new Object(kb, kb.std.op.Get);
        retOp.set("ast", ast);
        retOp.set("cell", compile(ast[kb.ids.cell]));
        retOp.set("role", compile(ast[kb.ids.role]));
        return retOp;
    }

    throw "Unknown function AST!";
}

void Ast::Function::checkMethodCall(CellI& astType, CellI& astMethodId, CellI& state)
{
    CellI* typePtr = nullptr;
    Map* methodsPtr = nullptr;
    if (astType.isA(kb.std.ast.Cell)) {
        auto& cellValue = astType[kb.ids.value];
        typePtr         = &cellValue;
        methodsPtr      = &static_cast<Map&>(cellValue[kb.ids.methods]);
    } else if (astType.isA(kb.std.ast.Struct)) {
        typePtr    = &astType;
        methodsPtr = &static_cast<Map&>(astType[kb.ids.methods]);
    } else if (astType.isA(kb.std.ast.ResolvedType)) {
        typePtr    = &astType[kb.ids.compiled];
        methodsPtr = &static_cast<Map&>(astType[kb.ids.ast][kb.ids.methods]);
    } else {
        throw "Unexpected AST type";
    }
    CellI& type = *typePtr;
    if (&type.type() != &kb.std.ast.Struct && &type.type() != &kb.std.Type_ && type.label() != "Type") {
        throw "Resolved type must be a type!";
    }
    auto& methods = *methodsPtr;
    if (!methods.hasKey(astMethodId)) {
        std::cerr << std::format("Method '{}' doesn't exist in type {}", astMethodId.label(), type.label()) << std::endl;
        throw "Method doesn't exist in type!";
    }
}

List& Ast::Function::parameters()
{
    if (missing("parameters")) {
        throw "No parameters!";
    } else {
        return static_cast<List&>(get("parameters"));
    }
}

CellI& Ast::Function::returnType()
{
    if (missing("returnType")) {
        throw "No returnType!";
    } else {
        return get("returnType");
    }
}

Ast::Base& Ast::Function::code()
{
    if (missing("code")) {
        throw "No code!";
    } else {
        return static_cast<Ast::Base&>(get("code"));
    }
}

Ast::FunctionT::FunctionT(brain::Brain& kb, CellI& name, const std::string& label) :
    BaseT<FunctionT>(kb, kb.std.ast.FunctionT, label)
{
    set("name", name);
    this->label(label);
}

Ast::FunctionT::FunctionT(brain::Brain& kb, const std::string& name) :
    BaseT<FunctionT>(kb, kb.std.ast.FunctionT, name)
{
    set("id", kb.pools.strings.get(name)[kb.ids.value]);
    label(name);
}

void Ast::FunctionT::parameters(Slot& param)
{
    if (missing("parameters")) {
        set("parameters", *new List(kb, kb.std.Slot));
    }
    parameters().add(param);
}

void Ast::FunctionT::returnType(CellI& type)
{
    set("returnType", type);
}

void Ast::FunctionT::addBlock(Block& block)
{
    set("code", block);
}

List& Ast::FunctionT::parameters()
{
    if (missing("parameters")) {
        throw "No parameters!";
    } else {
        return static_cast<List&>(get("parameters"));
    }
}

CellI& Ast::FunctionT::returnType()
{
    if (missing("returnType")) {
        throw "No returnType!";
    } else {
        return get("returnType");
    }
}

Ast::Base& Ast::FunctionT::code()
{
    if (missing("code")) {
        throw "No code!";
    } else {
        return static_cast<Ast::Base&>(get("code"));
    }
}

Ast::Delete::Delete(brain::Brain& kb, Base& cell) :
    BaseT<Delete>(kb, kb.std.ast.Delete, "ast.delete")
{
    set("cell", cell);
}

Ast::Set::Set(brain::Brain& kb, Base& cell, Base& role, Base& value) :
    BaseT<Set>(kb, kb.std.ast.Set, "ast.set")
{
    set("cell", cell);
    set("role", role);
    set("value", value);
}

Ast::Erase::Erase(brain::Brain& kb, Base& cell, Base& role) :
    BaseT<Erase>(kb, kb.std.ast.Erase, "ast.erase")
{
    set("cell", cell);
    set("role", role);
}

Ast::If::If(brain::Brain& kb, Base& condition, Base& thenBranch) :
    BaseT<If>(kb, kb.std.ast.If, "ast.if")
{
    set("condition", condition);
    set("then", thenBranch);
}

Ast::If::If(brain::Brain& kb, Base& condition, Base& thenBranch, Base& elseBranch) :
    BaseT<If>(kb, kb.std.ast.If, "ast.ifElse")
{
    set("condition", condition);
    set("then", thenBranch);
    set("else_", elseBranch);
}

Ast::Do::Do(brain::Brain& kb, Base& statement, Base& condition) :
    BaseT<Do>(kb, kb.std.ast.Do, "ast.do")
{
    set("condition", condition);
    set("statement", statement);
}

Ast::While::While(brain::Brain& kb, Base& condition, Base& statement) :
    BaseT<While>(kb, kb.std.ast.While, "ast.while")
{
    set("condition", condition);
    set("statement", statement);
}

Ast::Var::Var(brain::Brain& kb, CellI& role) :
    BaseT<Var>(kb, kb.std.ast.Var, role.label())
{
    set("role", role);
}

Ast::Set& Ast::Var::operator=(Base& value)
{
    return Set::New(kb, *this, Cell::New(kb, kb.ids.value), value);
}

Ast::Get& Ast::Var::operator*()
{
    return Get::New(kb, *this, Cell::New(kb, kb.ids.value));
}

Ast::Member::Member(brain::Brain& kb, CellI& role) :
    BaseT<Member>(kb, kb.std.ast.Member, "ast.member")
{
    set("role", role);
}

Ast::Set& Ast::Member::operator=(Base& value)
{
    Ast::Set& ret = Set::New(kb, Self::New(kb), Cell::New(kb, get("role")), value);
    return ret;
}

Ast::Get& Ast::Member::operator/(Base& role)
{
    return Get::New(kb, *this, role);
}

Ast::Get& Ast::Member::operator/(const std::string& role)
{
    return Get::New(kb, *this, kb._(role));
}

Ast::Has& Ast::Member::exist()
{
    return Has::New(kb, Self::New(kb), Cell::New(kb, get("role")));
}

Ast::Missing& Ast::Member::missing()
{
    return Missing::New(kb, Self::New(kb), Cell::New(kb, get("role")));
}

Ast::Call& Ast::Member::call(const std::string& method) {
    return kb.ast.call(*this, method);
}

Ast::SubType::SubType(brain::Brain& kb, CellI& name) :
    BaseT<SubType>(kb, kb.std.ast.SubTypeName, "ast.subTypeName")
{
    set("name", name);
}

Ast::TemplatedType::TemplatedType(brain::Brain& kb, CellI& id, CellI& typeList) :
    BaseT<TemplatedType>(kb, kb.std.ast.TemplatedType, "ast.templatedType")
{
    set("id", id);
    set("parameters", typeList);
}

void Ast::TemplatedType::addParam(const std::string& role, CellI& type)
{
    List& paramList = static_cast<List&>(get("parameters"));
    paramList.add(kb.ast.slot(role, type));
}

void Ast::TemplatedType::addParam(const std::string& role, const std::string& type)
{
    addParam(role, kb.ast.structName(type));
}

Ast::TemplateParam::TemplateParam(brain::Brain& kb, CellI& role) :
    BaseT<TemplateParam>(kb, kb.std.ast.TemplateParam, "ast.templateParam")
{
    set("role", role);
}

Ast::New::New(brain::Brain& kb, Base& objectType) :
    BaseT<New>(kb, kb.std.ast.New, "ast.new")
{
    set("objectType", objectType);
}

Ast::New::New(brain::Brain& kb, Base& objectType, Base& constructor) :
    BaseT<New>(kb, kb.std.ast.New, "ast.new()")
{
    set("objectType", objectType);
    set("constructor", constructor);
}

void Ast::New::addParam(Slot& slot)
{
    if (missing("parameters")) {
        set("parameters", kb.list(slot));
    } else {
        List& paramList = static_cast<List&>(get("parameters"));
        paramList.add(slot);
    }
}

Ast::Same::Same(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Same>(kb, kb.std.ast.Same, "ast.same")
{
    set("lhs", lhs);
    set("rhs", rhs);
}

Ast::NotSame::NotSame(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<NotSame>(kb, kb.std.ast.NotSame, "ast.notSame")
{
    set("lhs", lhs);
    set("rhs", rhs);
}

Ast::Equal::Equal(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Equal>(kb, kb.std.ast.Equal, "ast.equal")
{
    set("lhs", lhs);
    set("rhs", rhs);
}

Ast::NotEqual::NotEqual(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<NotEqual>(kb, kb.std.ast.NotEqual, "ast.notEqual")
{
    set("lhs", lhs);
    set("rhs", rhs);
}

Ast::Has::Has(brain::Brain& kb, Base& cell, Base& role) :
    BaseT<Has>(kb, kb.std.ast.Has, "ast.has")
{
    set("cell", cell);
    set("role", role);
}

Ast::Missing::Missing(brain::Brain& kb, Base& cell, Base& role) :
    BaseT<Missing>(kb, kb.std.ast.Missing, "ast.missing")
{
    set("cell", cell);
    set("role", role);
}

Ast::Get::Get(brain::Brain& kb, Base& cell, Base& role) :
    BaseT<Get>(kb, kb.std.ast.Get, "ast.get")
{
    set("cell", cell);
    set("role", role);
}

Ast::Get& Ast::Get::operator/(Base& role)
{
    return Get::New(kb, *this, role);
}
Ast::Get& Ast::Get::operator/(const std::string& role)
{
    return Get::New(kb, *this, kb._(role));
}

Ast::And::And(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<And>(kb, kb.std.ast.And, "ast.and")
{
    set("lhs", lhs);
    set("rhs", rhs);
}

Ast::Or::Or(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Or>(kb, kb.std.ast.Or, "ast.or")
{
    set("lhs", lhs);
    set("rhs", rhs);
}

Ast::Not::Not(brain::Brain& kb, Base& input) :
    BaseT<Not>(kb, kb.std.ast.Not, "ast.not")
{
    set("input", input);
}

Ast::Add::Add(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Add>(kb, kb.std.ast.Add, "ast.add")
{
    set("lhs", lhs);
    set("rhs", rhs);
}

Ast::Subtract::Subtract(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Subtract>(kb, kb.std.ast.Subtract, "ast.subtract")
{
    set("lhs", lhs);
    set("rhs", rhs);
}

Ast::Multiply::Multiply(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Multiply>(kb, kb.std.ast.Multiply, "ast.multiply")
{
    set("lhs", lhs);
    set("rhs", rhs);
}

Ast::Divide::Divide(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<Divide>(kb, kb.std.ast.Divide, "ast.divide")
{
    set("lhs", lhs);
    set("rhs", rhs);
}

Ast::LessThan::LessThan(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<LessThan>(kb, kb.std.ast.LessThan, "ast.lessThan")
{
    set("lhs", lhs);
    set("rhs", rhs);
}

Ast::LessThanOrEqual::LessThanOrEqual(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<LessThanOrEqual>(kb, kb.std.ast.LessThanOrEqual, "ast.lessThanOrEqual")
{
    set("lhs", lhs);
    set("rhs", rhs);
}

Ast::GreaterThan::GreaterThan(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<GreaterThan>(kb, kb.std.ast.GreaterThan, "ast.greaterThan")
{
    set("lhs", lhs);
    set("rhs", rhs);
}

Ast::GreaterThanOrEqual::GreaterThanOrEqual(brain::Brain& kb, Base& lhs, Base& rhs) :
    BaseT<GreaterThanOrEqual>(kb, kb.std.ast.GreaterThanOrEqual, "ast.greaterThanOrEqual")
{
    set("lhs", lhs);
    set("rhs", rhs);
}

Ast::Ast(brain::Brain& kb) :
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
        return StructName::New(kb, kb.id(outName));
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

Ast::Parameter& Ast::parameter(CellI& role)
{
    auto& ast = kb.ast;
    return Parameter::New(kb, role);
}

Ast::Slot& Ast::slot(const std::string& role, CellI& type)
{
    return Slot::New(kb, kb.id(role), type);
}

Ast::Slot& Ast::slot(CellI& role, CellI& type)
{
    return Slot::New(kb, role, type);
}

Ast::Call& Ast::call(CellI& object, const std::string& method)
{
    return Call::New(kb, object, kb.ast.cell(kb.id(method)));
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
    return scall(type, kb.ast.cell(kb.id(method)));
}

Ast::Delete& Ast::delete_(Base& ast)
{
    return Delete::New(kb, ast);
}

Ast::Set& Ast::set(Base& cell, Base& role, Base& value)
{
    return Set::New(kb, cell, role, value);
}

Ast::Set& Ast::set(Base& cell, const std::string& role, Base& value)
{
    return Set::New(kb, cell, kb._(role), value);
}

Ast::Erase& Ast::erase(Base& cell, Base& role)
{
    return Erase::New(kb, cell, role);
}

Ast::Erase& Ast::erase(Base& cell, const std::string& role)
{
    return Erase::New(kb, cell, kb._(role));
}

Ast::If& Ast::if_(Base& condition, Base& thenBranch)
{
    return If::New(kb, condition, thenBranch);
}

Ast::If& Ast::if_(Base& condition, Base& thenBranch, Base& elseBranch)
{
    return If::New(kb, condition, thenBranch, elseBranch);
}

Ast::Do& Ast::do_(Base& statement, Base& condition)
{
    return Do::New(kb, statement, condition);
}

Ast::While& Ast::while_(Base& condition, Base& statement)
{
    return While::New(kb, condition, statement);
}

Ast::Var& Ast::var(CellI& role)
{
    return Var::New(kb, role);
}

Ast::Member& Ast::member(CellI& role)
{
    return Member::New(kb, role);
}

Ast::SubType& Ast::subType(CellI& role)
{
    return SubType::New(kb, role);
}

Ast::TemplatedType& Ast::templatedType(const std::string& idStr, CellI& type)
{
    CellI& ret = processNamespacedName(idStr, [this, &type](const std::string& outName)->CellI& {
        return TemplatedType::New(kb, kb.id(outName), kb.list(type));
    });

    return static_cast<Ast::TemplatedType&>(ret);
}

Ast::TemplateParam& Ast::templateParam(CellI& role)
{
    return TemplateParam::New(kb, role);
}

Ast::New& Ast::new_(Base& objectType)
{
    return New::NewT<Ast::New>::New(kb, objectType);
}

Ast::New& Ast::new_(Base& objectType, Base& constructor)
{
    return New::NewT<Ast::New>::New(kb, objectType, constructor);
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

Ast::Has& Ast::has(Base& cell, Base& role)
{
    return Has::New(kb, cell, role);
}

Ast::Has& Ast::has(Base& cell, const std::string& role)
{
    return Has::New(kb, cell, kb._(role));
}

Ast::Missing& Ast::missing(Base& cell, Base& role)
{
    return Missing::New(kb, cell, role);
}

Ast::Missing& Ast::missing(Base& cell, const std::string& role)
{
    return Missing::New(kb, cell, kb._(role));
}

Ast::Get& Ast::get(Base& cell, const std::string& role)
{
    return Get::New(kb, cell, kb._(role));
}

Ast::Get& Ast::get(Base& cell, Base& role)
{
    return Get::New(kb, cell, role);
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

CellI& Ast::processNamespacedName(const std::string& inputName, std::function<CellI&(const std::string& outName)> createCb)
{
    std::vector<std::string> sliced;
    boost::algorithm::split_regex(sliced, inputName, boost::regex("::"));

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
            namespaceList.add(kb.id(currentId));
        }
    }

    return obj;
}

Directions::Directions(brain::Brain& kb) :
    up(kb, kb.std.Directions, "up"),
    down(kb, kb.std.Directions, "down"),
    left(kb, kb.std.Directions, "left"),
    right(kb, kb.std.Directions, "right")
{
    kb.std.Directions.set("members", kb.list(up, down, left, right));
}

Coordinates::Coordinates(brain::Brain& kb) :
    x(kb, kb.std.Cell, "x"),
    y(kb, kb.std.Cell, "y")
{
}

Boolean::Boolean(brain::Brain& kb) :
    true_(kb, kb.std.Boolean, "true"),
    false_(kb, kb.std.Boolean, "false")
{
}

Numbers::Numbers(brain::Brain& kb) :
    sign(kb, kb.std.Cell, kb.std.Cell, "sign"),
    positive(kb, kb.std.Cell, "positive"),
    negative(kb, kb.std.Cell, "negative")

{
    sign.add(positive, positive);
    sign.add(negative, negative);
}

namespace pools {

// ============================================================================
Chars::Chars(brain::Brain& kb) :
    kb(kb)
{
    // These are enough for me currently
    registerUnicodeBlock(0x020, 0x07e); // Basic Latin - without the DEL (0x7f) control character
    registerUnicodeBlock(0x080, 0x0ff); // Latin-1 Supplement
    registerUnicodeBlock(0x100, 0x17f); // Latin Extended-A
}

Object& Chars::get(char32_t utf32Char)
{
    auto unicodeCellIt = m_characters.find(utf32Char);
    if (unicodeCellIt != m_characters.end()) {
        return unicodeCellIt->second;
    } else {
        throw "Unknown char!";
    }
}

void Chars::registerUnicodeBlock(char32_t from, char32_t to)
{
    for (char32_t unicodeValue = from; unicodeValue <= to; ++unicodeValue) {
        std::string characterName;
        if (unicodeValue == 32) {
            characterName = "(space)";
        } else if (unicodeValue > 32 && unicodeValue < 127) {
            characterName = " ";
            characterName[0] = unicodeValue;
        } else {
            characterName = std::format("Unicode_{:#04x}", (int)unicodeValue);
        }
        auto it = m_characters.emplace(std::piecewise_construct,
                             std::forward_as_tuple(unicodeValue),
                             std::forward_as_tuple(kb, kb.std.Char, characterName));
        auto& value = it.first->second;
    }
}

// ============================================================================
Digits::Digits(brain::Brain& kb)
{
    m_digits.reserve(10);
    for (int i = 0; i < 10; ++i) {
        std::string digitName = "Digit_" + std::to_string(i);
        m_digits.emplace_back(kb, kb.std.Digit, digitName);
    }
}

Object& Digits::operator[](int digit)
{
    return m_digits[digit];
}

// ============================================================================
Numbers::Numbers(brain::Brain& kb) :
    m_kb(kb)
{
}

Number& Numbers::get(int number)
{
    auto numberIt = m_numbers.find(number);
    if (numberIt != m_numbers.end()) {
        return numberIt->second;
    } else {
        auto it           = m_numbers.emplace(std::piecewise_construct,
                                              std::forward_as_tuple(number),
                                              std::forward_as_tuple(m_kb, number));
        Number& newNumber = it.first->second;
        newNumber.label(std::to_string(number));
        return newNumber;
    }
}


struct StringInit
{
    StringInit(const std::string& str, List& list) :
        str(str), list(list) {}
    std::string str;
    List& list;
};

// ============================================================================
Strings::Strings(brain::Brain& kb) :
    kb(kb)
{
    StringInit reservedStrings[] = {
        { "argument", kb.ids.argument },
        { "ast", kb.ids.ast },
        { "asts", kb.ids.asts },
        { "blue", kb.ids.blue },
        { "cell", kb.ids.cell },
        { "children", kb.ids.children },
        { "code", kb.ids.code },
        { "color", kb.ids.color },
        { "compiled", kb.ids.compiled },
        { "condition", kb.ids.condition },
        { "constructor", kb.ids.constructor },
        { "continue_", kb.ids.continue_ },
        { "currentFn", kb.ids.currentFn },
        { "currentStruct", kb.ids.currentStruct },
        { "data", kb.ids.data },
        { "destructor", kb.ids.destructor },
        { "else_", kb.ids.else_ },
        { "emptyObject", kb.ids.emptyObject },
        { "first", kb.ids.first },
        { "functions", kb.ids.functions },
        { "globalScope", kb.ids.globalScope },
        { "green", kb.ids.green },
        { "height", kb.ids.height },
        { "id", kb.ids.id },
        { "index", kb.ids.index },
        { "input", kb.ids.input },
        { "instances", kb.ids.instances },
        { "item", kb.ids.item },
        { "itemType", kb.ids.itemType },
        { "key", kb.ids.key },
        { "keyType", kb.ids.keyType },
        { "last", kb.ids.last },
        { "lhs", kb.ids.lhs },
        { "list", kb.ids.list },
        { "listType", kb.ids.listType },
        { "localVars", kb.ids.localVars },
        { "memberOf", kb.ids.memberOf },
        { "members", kb.ids.members },
        { "method", kb.ids.method },
        { "methods", kb.ids.methods },
        { "name", kb.ids.name },
        { "next", kb.ids.next },
        { "objectType", kb.ids.objectType },
        { "op", kb.ids.op },
        { "ops", kb.ids.ops },
        { "output", kb.ids.output },
        { "parameters", kb.ids.parameters },
        { "parent", kb.ids.parent },
        { "pixels", kb.ids.pixels },
        { "previous", kb.ids.previous },
        { "red", kb.ids.red },
        { "resolvedScope", kb.ids.resolvedScope },
        { "result", kb.ids.result },
        { "returnType", kb.ids.returnType },
        { "rhs", kb.ids.rhs },
        { "role", kb.ids.role },
        { "rootNode", kb.ids.rootNode },
        { "scope", kb.ids.scope },
        { "scopes", kb.ids.scopes },
        { "self", kb.ids.self },
        { "size", kb.ids.size },
        { "slotRole", kb.ids.slotRole },
        { "slots", kb.ids.slots },
        { "slotType", kb.ids.slotType },
        { "stack", kb.ids.stack },
        { "statement", kb.ids.statement },
        { "static_", kb.ids.static_ },
        { "status", kb.ids.status },
        { "structs", kb.ids.structs },
        { "structType", kb.ids.structType },
        { "subTypes", kb.ids.subTypes },
        { "templateId", kb.ids.templateId },
        { "templateParams", kb.ids.templateParams },
        { "then", kb.ids.then },
        { "throw", kb.ids.throw_ },
        { "type", kb.ids.type },
        { "unknownInstances", kb.ids.unknownInstances },
        { "unknownStructs", kb.ids.unknownStructs },
        { "value", kb.ids.value },
        { "valueType", kb.ids.valueType },
        { "variables", kb.ids.variables },
        { "width", kb.ids.width }
    };
    for (auto& reservedString : reservedStrings) {
        auto& str = reservedString.str;
        m_strings.emplace(std::piecewise_construct,
                          std::forward_as_tuple(reservedString.str),
                          std::forward_as_tuple(kb, reservedString.list, reservedString.str));

    }
}

String& Strings::get(const std::string& str)
{
    auto numberIt = m_strings.find(str);
    if (numberIt != m_strings.end()) {
        return numberIt->second;
    } else {
        auto it           = m_strings.emplace(std::piecewise_construct,
                                              std::forward_as_tuple(str),
                                              std::forward_as_tuple(kb, str));
        String& newString = it.first->second;
        newString.label(str);
        return newString;
    }
}

List& Strings::getCharList(const std::string& str)
{
    auto& string = get(str);
    auto& ret = static_cast<List&>(string[string.kb.ids.value]);

    return ret;
}

} // namespace pools

Pools::Pools(brain::Brain& kb) :
    chars(kb),
    digits(kb),
    numbers(kb),
    strings(kb)
{
}

Ast::Cell& Brain::_(CellI& cell)
{
    return ast.cell(cell);
}

Ast::Cell& Brain::_(const std::string& name)
{
    return ast.cell(id(name));
}

Ast::Cell& Brain::true_()
{
    return _(boolean.true_);
}

Ast::Cell& Brain::false_()
{
    return _(boolean.false_);
}

Ast::Parameter& Brain::p_(const std::string& name)
{
    return ast.parameter(id(name));
}

Ast::Member& Brain::m_(const std::string& name)
{
    return ast.member(id(name));
}

Ast::Var& Brain::var_(const std::string& name)
{
    return ast.var(id(name));
}

Ast::Slot& Brain::param(const std::string& name, CellI& value)
{
    return ast.slot(id(name), value);
}

Ast::Slot& Brain::member(const std::string& name, const std::string& type)
{
    return ast.slot(id(name), struct_(type));
}

Ast::Slot& Brain::member(const std::string& name, CellI& type)
{
    return ast.slot(id(name), type);
}

Ast::TemplateParam& Brain::tp_(const std::string& name)
{
    return ast.templateParam(id(name));
}

Ast::StructName& Brain::struct_(const std::string& name)
{
    return ast.structName(name);
}

void Brain::createOp(Ast::Scope& stdScope)
{
    auto& opScope = stdScope.addScope("op");
    opScope.addStruct("Base");
    opScope.addStruct("Add")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("value", "std::Number"));

    opScope.addStruct("And")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("value", "std::Boolean"));

    opScope.addStruct("Block")
        .members(
            member("ast", "ast::Base"),
            member("status", "std::Cell"),
            member("ops", "std::Cell"),
            member("value", "std::Cell"));

    opScope.addStruct("ConstVar")
        .members(
            member("ast", "ast::Base"),
            member("value", "std::Cell"));

    opScope.addStruct("Delete")
        .members(
            member("ast", "ast::Base"),
            member("input", "Base"));

    opScope.addStruct("Divide")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("value", "std::Number"));

    opScope.addStruct("Do")
        .members(
            member("ast", "ast::Base"),
            member("status", "std::Cell"),
            member("condition", "Base"),
            member("statement", "Base"));

    opScope.addStruct("Equal")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("value", "std::Boolean"));

    opScope.addStruct("Erase")
        .members(
            member("ast", "ast::Base"),
            member("cell", "Base"),
            member("role", "Base"),
            member("value", "Base"));

    opScope.addStruct("EvalVar")
        .members(
            member("ast", "ast::Base"),
            member("value", "Var"));

    opScope.addStruct("Function")
        .members(
            member("ast", "ast::Base"),
            member("stack", "Stack"),
            member("op", tt_("std::List", "valueType", "Base")),
            member("static_", "std::Boolean"));

    opScope.addStruct("Get")
        .members(
            member("ast", "ast::Base"),
            member("cell", "Base"),
            member("role", "Base"),
            member("value", "std::Cell"));

    opScope.addStruct("GreaterThan")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("value", "std::Boolean"));

    opScope.addStruct("GreaterThanOrEqual")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("value", "std::Boolean"));

    opScope.addStruct("Has")
        .members(
            member("ast", "ast::Base"),
            member("cell", "Base"),
            member("role", "Base"),
            member("value", "std::Boolean"));

    opScope.addStruct("If")
        .members(
            member("ast", "ast::Base"),
            member("status", "std::Cell"),
            member("condition", "Base"),
            member("then", "Base"),
            member("else_", "Base"));

    opScope.addStruct("LessThan")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("value", "std::Boolean"));

    opScope.addStruct("LessThanOrEqual")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("value", "std::Boolean"));

    opScope.addStruct("Missing")
        .members(
            member("ast", "ast::Base"),
            member("cell", "Base"),
            member("role", "Base"),
            member("value", "std::Boolean"));

    opScope.addStruct("Multiply")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("value", "std::Number"));

    opScope.addStruct("New")
        .members(
            member("ast", "ast::Base"),
            member("value", "std::Cell"),
            member("objectType", "Base"));

    opScope.addStruct("Not")
        .members(
            member("ast", "ast::Base"),
            member("input", "Base"),
            member("value", "std::Boolean"));

    opScope.addStruct("NotEqual")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("value", "std::Boolean"));

    opScope.addStruct("NotSame")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("value", "std::Boolean"));

    opScope.addStruct("Or")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("value", "std::Boolean"));

    opScope.addStruct("Return")
        .members(
            member("ast", "ast::Base"),
            member("result", "ast::Base"));

    opScope.addStruct("Same")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("value", "std::Boolean"));

    opScope.addStruct("Set")
        .members(
            member("ast", "ast::Base"),
            member("cell", "Base"),
            member("role", "Base"),
            member("value", "Base"));

    opScope.addStruct("Subtract")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("value", "std::Number"));

    opScope.addStruct("Var")
        .members(
            member("ast", "ast::Base"),
            member("valueType", "std::Type"),
            member("value", "std::Cell"));

    opScope.addStruct("While")
        .members(
            member("ast", "ast::Base"),
            member("status", "std::Cell"),
            member("condition", "Base"),
            member("statement", "Base"));
}

void Brain::createAst(Ast::Scope& stdScope)
{
    auto& astScope = stdScope.addScope("ast");
    astScope.addStruct("Base");
    astScope.addStruct("Add")
        .members(
            member("lhs", "Base"),
            member("rhs", "Base"));

    astScope.addStruct("And")
        .members(
            member("lhs", "Base"),
            member("rhs", "Base"));

    astScope.addStruct("Block")
        .members(
            member("asts", "std::Cell"));

    astScope.addStruct("Break");

    astScope.addStruct("Call")
        .members(
            member("cell", "Base"),
            member("method", "Base"),
            member("parameters", ListOf(std.Slot)));

    astScope.addStruct("Cell")
        .members(
            member("value", "std::Cell"));

    astScope.addStruct("Continue");

    astScope.addStruct("Delete")
        .members(
            member("cell", "Base"));

    astScope.addStruct("Divide")
        .members(
            member("lhs", "Base"),
            member("rhs", "Base"));

    astScope.addStruct("Do")
        .members(
            member("condition", "Base"),
            member("statement", "Base"));

    astScope.addStruct("Equal")
        .members(
            member("lhs", "Base"),
            member("rhs", "Base"));

    astScope.addStruct("Erase")
        .members(
            member("cell", "Base"),
            member("role", "Base"));

    astScope.addStruct("Function")
        .members(
            member("name", "std::Cell"),
            member("fullId", "std::Cell"),
            member("structType", "std::Cell"),
            member("parameters", ListOf(std.Slot)),
            member("returnType", "std::Type"),
            member("code", "Base"),
            member("scope", "Base"),
            member("static_", "std::Boolean"));

    astScope.addStruct("FunctionT")
        .members(
            member("name", "std::Cell"),
            member("parameters", ListOf(std.Slot)),
            member("returnType", "std::Type"),
            member("code", "Base"),
            member("scope", "Base"),
            member("static_", "std::Boolean"));

    astScope.addStruct("Get")
        .members(
            member("cell", "Base"),
            member("role", "Base"));

    astScope.addStruct("GreaterThan")
        .members(
            member("lhs", "Base"),
            member("rhs", "Base"));

    astScope.addStruct("GreaterThanOrEqual")
        .members(
            member("lhs", "Base"),
            member("rhs", "Base"));

    astScope.addStruct("Has")
        .members(
            member("cell", "Base"),
            member("role", "Base"));

    astScope.addStruct("If")
        .members(
            member("condition", "Base"),
            member("then", "Base"),
            member("else_", "Base"));

    astScope.addStruct("LessThan")
        .members(
            member("lhs", "Base"),
            member("rhs", "Base"));

    astScope.addStruct("LessThanOrEqual")
        .members(
            member("lhs", "Base"),
            member("rhs", "Base"));

    astScope.addStruct("Member")
        .members(
            member("role", "Base"));

    astScope.addStruct("Missing")
        .members(
            member("cell", "Base"),
            member("role", "Base"));

    astScope.addStruct("Multiply")
        .members(
            member("lhs", "Base"),
            member("rhs", "Base"));

    astScope.addStruct("New")
        .members(
            member("objectType", "Base"),
            member("constructor", "Base"),
            member("parameters", ListOf(std.ast.Slot)));

    astScope.addStruct("Not")
        .members(
            member("input", "Base"));

    astScope.addStruct("NotEqual")
        .members(
            member("lhs", "Base"),
            member("rhs", "Base"));

    astScope.addStruct("NotSame")
        .members(
            member("lhs", "Base"),
            member("rhs", "Base"));

    astScope.addStruct("Or")
        .members(
            member("lhs", "Base"),
            member("rhs", "Base"));

    astScope.addStruct("Parameter")
        .members(
            member("role", "std::Cell"));

    astScope.addStruct("ResolvedType")
        .members(
            member("ast", "std::Type"),
            member("compiled", "std::Type"));

    astScope.addStruct("Return")
        .members(
            member("value", "std::Cell"));

    astScope.addStruct("Same")
        .members(
            member("lhs", "Base"),
            member("rhs", "Base"));

    astScope.addStruct("Scope")
        .members(
            member("id", "std::Cell"),
            member("fullId", "std::Cell"),
            member("scopes", "std::TrieMap"),
            member("resolvedScope", "Scope"),
            member("parent", "Scope"),
            member("functions", "std::TrieMap"),
            member("structs", "std::TrieMap"),
            member("structTs", "std::TrieMap"),
            member("variables", ListOf(std.ast.Slot)));

    astScope.addStruct("Self");

    astScope.addStruct("SelfFn");

    astScope.addStruct("Set")
        .members(
            member("cell", "Base"),
            member("role", "Base"),
            member("value", "Base"));

    astScope.addStruct("Slot")
        .members(
            member("slotRole", "Base"),
            member("slotType", "Base"));

    astScope.addStruct("StaticCall")
        .members(
            member("cell", "Base"),
            member("method", "Base"),
            member("parameters", ListOf(std.Slot)));

    astScope.addStruct("Struct")
        .members(
            member("id", "std::Cell"),
            member("fullId", "std::Cell"),
            member("incomplete", "std::Boolean"),
            member("instanceOf", "Base"),
            member("templateParams", "std::List"),
            member("scope", "Scope"),
            member("methods", MapOf(std.Cell, std.ast.Function)),
            member("members", MapOf(std.Cell, std.ast.Slot)),
            member("subTypes", ListOf(std.ast.Slot)),
            member("memberOf", ListOf(std.Type_)));

    astScope.addStruct("StructName")
        .members(
            member("name", "std::Cell"),
            member("scopes", "std::List"));

    astScope.addStruct("StructT")
        .members(
            member("id", "std::Cell"),
            member("scope", "Base"),
            member("methods", MapOf(std.Cell, std.ast.Function)),
            member("members", ListOf(std.ast.Slot)),
            member("subTypes", ListOf(std.ast.Slot)),
            member("memberOf", ListOf(std.Type_)),
            member("templateParams", MapOf(std.Cell, std.Type_)));

    astScope.addStruct("SubTypeName")
        .members(
            member("name", "std::Cell"));

    astScope.addStruct("Subtract")
        .members(
            member("lhs", "Base"),
            member("rhs", "Base"));

    astScope.addStruct("TemplatedType")
        .members(
            member("id", "Base"),
            member("scopes", "std::List"),
            member("parameters", ListOf(std.Slot)));

    astScope.addStruct("TemplateParam")
        .members(
            member("role", "std::Cell"));

    astScope.addStruct("Throw")
        .members(
            member("value", "Base"));

    astScope.addStruct("Try")
        .members(
            member("tryBranch", "Base"),
            member("catchBranch", "Base"));

    astScope.addStruct("Var")
        .members(
            member("role", "Base"),
            member("scope", "Scope"));

    astScope.addStruct("While")
        .members(
            member("condition", "Base"),
            member("statement", "Base"));
}

void Brain::createStd()
{
    auto& stdScope = globalScope.addScope("std");
    createOp(stdScope);
    createAst(stdScope);

    stdScope.addStruct("Cell");
    stdScope.addStruct("Slot")
        .members(
            member("slotRole", "Cell"),
            member("slotType", "Type"));

    stdScope.addStruct("Enum")
        .members(
            member("members", tt_("List", "valueType", "Cell")));

    stdScope.addStruct("Container");
    stdScope.addStruct("Boolean");
    stdScope.addStruct("Char");
    stdScope.addStruct("Digit");
    stdScope.addStruct("Number");
    stdScope.addStruct("String");

    stdScope.addStruct("Color")
        .members(
            member("red", "Number"),
            member("green", "Number"),
            member("blue", "Number"));

    stdScope.addStruct("Pixel");
    stdScope.addStruct("Picture");
    stdScope.addStruct("Stack");

    stdScope.addStruct("StackFrame")
        .members(
            member("method", "op::Function"),
            member("input", "Index"),
            member("output", "op::Var"),
            member("localVars", "Index"));

    stdScope.addStruct("Program")
        .members(
            member("data", "ProgramData"),
            member("code", "op::Base"),
            member("stack", "ListItem"));

    stdScope.addStruct("ProgramData")
        .members(
            member("functions", tt_("TrieMap", "keyType", "Cell", "valueType", "op::Function")),
            member("structs", tt_("TrieMap", "keyType", "Cell", "valueType", "Type")),
            member("variables", tt_("TrieMap", "keyType", "Cell", "valueType", "op::Var")));

    stdScope.addStruct("StructReference")
        .members(
            member("id", tt_("List", "valueType", "Char")),
            member("idScope", "ast::Scope"),
            member("scope", "ast::Scope"),
            member("resolvedScope", "ast::Scope"),
            member("currentFn", "ast::Function"),
            member("currentStruct", "ast::Struct"),
            member("templateId", tt_("List", "valueType", "Cell")),
            member("templateParams", tt_("List", "valueType", "ast::Base")),
            member("value", "Type"));

    stdScope.addStruct("CompileState")
        .members(
            member("currentFn", "ast::Function"),
            member("currentStruct", "ast::Struct"),
            member("lastBlock", "ast::Block"),
            member("scope", "ast::Scope"),
            member("resolvedScope", "ast::Scope"),
            member("globalScope", "ast::Scope"),
            member("globalResolvedScope", "ast::Scope"),
            member("functions", tt_("List", "valueType", "op::Function")),
            member("structs", tt_("TrieMap", "keyType", "Cell", "valueType", "Type")),
            member("unknownStructs", tt_("TrieMap", "keyType", "Cell", "valueType", "Type")),
            member("instances", tt_("TrieMap", "keyType", "Cell", "valueType", "Type")),
            member("unknownInstances", tt_("TrieMap", "keyType", "Cell", "valueType", "Type")),
            member("instanceAsts", tt_("TrieMap", "keyType", "Cell", "valueType", "Type")),
            member("unknownInstanceAsts", tt_("TrieMap", "keyType", "Cell", "valueType", "Type")),
            member("variables", tt_("TrieMap", "keyType", "Cell", "valueType", "op::Var")));

    stdScope.addStruct("Directions");

#pragma region ListItem
    stdScope.addStruct("ListItem")
        .subTypes(
            param("valueType", struct_("Cell")))
        .members(
            member("previous", "ListItem"),
            member("next", "ListItem"),
            member("value", "Cell"));

    auto& listItemStructT
        = stdScope.addStructT("ListItem")
              .templateParams(
                  param("valueType", _(std.Type_)))
              .memberOf(
                  _(std.ListItem))
              .subTypes(
                  param("valueType", tp_("valueType")))
              .members(
                  member("previous", tt_("ListItem", "valueType", tp_("valueType"))),
                  member("next", tt_("ListItem", "valueType", tp_("valueType"))),
                  member("value", tp_("valueType")));

    listItemStructT.addMethod("constructor")
        .parameters(
            param("value", tp_("valueType")))
        .code(
            m_("value") = p_("value"));
#pragma endregion
#pragma region List
    stdScope.addStruct("List")
        .subTypes(
            param("itemType", struct_("ListItem")),
            param("valueType", struct_("Cell")))
        .members(
            member("first", "ListItem"),
            member("last", "ListItem"),
            member("size", _(std.Number)));
    auto& listStructT
        = stdScope.addStructT("List")
              .templateParams(
                  param("valueType", _(std.Type_)))
              .memberOf(
                  struct_("Container"),
                  struct_("List"))
              .subTypes(
                  param("itemType", tt_("ListItem", "valueType", tp_("valueType"))),
                  param("valueType", tp_("valueType")))
              .members(
                  member("first", st_("itemType")),
                  member("last", st_("itemType")),
                  member("size", _(std.Number)));

    listStructT.addMethod("constructor")
        .code(
            m_("size") = _(_0_));

    listStructT.addMethod("add")
        .parameters(
            param("value", tp_("valueType")))
        .returnType(st_("itemType"))
        .code(
            var_("item") = ast.new_(st_("itemType"), "constructor", param("value", p_("value"))),
            ast.if_(ast.not_(m_("first").exist()),
                    m_("first") = *var_("item"),                          // then
                    ast.block(ast.set(m_("last"), "next", *var_("item")), // else
                              ast.set(*var_("item"), "previous", m_("last")))),
            m_("last") = *var_("item"),
            m_("size") = ast.add(m_("size"), _(_1_)),
            ast.return_(*var_("item")));

    /*
    void List::removeItem(Item* item)
    {
        if (item->m_previous) {
            item->m_previous->m_next = item->m_next;
        } else {
            m_firstItem = item->m_next;
        }
        if (item->m_next) {
            item->m_next->m_previous = item->m_previous;
        } else {
            m_lastItem = item->m_previous;
        }
        --m_size;
    }
    */
    listStructT.addMethod("remove")
        .parameters(
            param("item", _(std.Cell)))
        .code(
            ast.if_(ast.has(p_("item"), "previous"),
                    ast.if_(ast.has(p_("item"), "next"),
                            ast.set(p_("item") / "previous", "next", p_("item") / "next"),
                            ast.erase(p_("item") / "previous", "next")),
                    ast.if_(ast.has(p_("item"), "next"),
                            m_("first") = p_("item") / "next",
                            ast.erase(ast.self(), "first"))),
            ast.if_(ast.has(p_("item"), "next"),
                    ast.if_(ast.has(p_("item"), "previous"),
                            ast.set(p_("item") / "next", "previous", p_("item") / "previous"),
                            ast.erase(p_("item") / "next", "previous")),
                    ast.if_(ast.has(p_("item"), "previous"),
                            m_("last") = p_("item") / "previous",
                            ast.erase(ast.self(), "last"))),
            m_("size") = ast.subtract(m_("size"), _(_1_)));

    listStructT.addMethod("size")
        .returnType(_(std.Number))
        .code(
            ast.return_(m_("size")));

    listStructT.addMethod("empty")
        .returnType(_(std.Boolean))
        .code(
            ast.return_(ast.equal(m_("size"), _(_0_))));

    listStructT.addMethod("first")
        .returnType(tp_("valueType"))
        .code(
            ast.return_(m_("first") / "value"));

    listStructT.addMethod("last")
        .returnType(tp_("valueType"))
        .code(
            ast.return_(m_("last") / "value"));

    listStructT.addMethod("begin")
        .returnType(st_("itemType"))
        .code(
            ast.return_(m_("first")));

    listStructT.addMethod("end")
        .returnType(st_("itemType"))
        .code(
            ast.return_(m_("last")));
#pragma endregion
#pragma region Type
    auto& typeStruct
        = stdScope.addStruct("Type")
              .members(
                  member("name", tt_("List", "valueType", _(std.Char))),
                  member("slots", tt_("Map", "keyType", _(std.Cell), "valueType", _(std.Slot))),
                  member("incomplete", _(std.Boolean)),
                  member("sharedObject", _(std.Slot)),
                  member("subTypes", tt_("Map", "keyType", _(std.Cell), "valueType", "Type")),
                  member("memberOf", tt_("Map", "keyType", "Type", "valueType", "Type")),
                  member("asts", tt_("Map", "keyType", _(std.Cell), "valueType", _(std.ast.Function))),
                  member("methods", tt_("Map", "keyType", _(std.Cell), "valueType", _(std.op.Function))));

    typeStruct.addMethod("constructor")
        .code(
            m_("slots") = ast.new_(tt_("Map", "keyType", _(std.Cell), "valueType", _(std.Slot)), "constructor"));

    typeStruct.addMethod("constructorWithRecursiveType")
        .code(
            m_("slots") = ast.new_(tt_("Map", "keyType", _(std.Cell), "valueType", _(std.Slot)), "constructorWithIndexType", param("indexType", ast.self())));

    typeStruct.addMethod("addSubType")
        .parameters(
            param("slotRole", _(std.Cell)),
            param("slotType", _(std.Type_)))
        .code(
            ast.if_(m_("subTypes").missing(),
                    m_("subTypes") = ast.new_(tt_("Map", "keyType", _(std.Cell), "valueType", "Type"), "constructor")),
            m_("subTypes").call("add", param("key", p_("slotRole")), param("value", p_("slotType"))));

    typeStruct.addMethod("addMembership")
        .parameters(
            param("cell", _(std.Type_)))
        .code(
            ast.if_(m_("memberOf").missing(),
                    m_("memberOf") = ast.new_(tt_("Map", "keyType", "Type", "valueType", "Type"), "constructor")),
            m_("memberOf").call("add", param("key", p_("cell")), param("value", p_("cell"))));

    typeStruct.addMethod("addSlot")
        .parameters(
            param("slotRole", _(std.Cell)),
            param("slotType", _(std.Slot)))
        .code(
            ast.if_(m_("slots").missing(),
                    m_("slots") = ast.new_(tt_("Map", "keyType", _(std.Cell), "valueType", _(std.Slot)), "constructor")),
            var_("slot") = ast.new_(_(std.Slot)),
            ast.set(*var_("slot"), "slotRole", p_("slotRole")),
            ast.set(*var_("slot"), "slotType", p_("slotType")),
            m_("slots").call("add", param("key", p_("slotRole")), param("value", *var_("slot"))));

    typeStruct.addMethod("addSlots")
        .parameters(
            param("list", tt_("List", "valueType", _(std.Slot))))
        .code(
            ast.if_(ast.equal(p_("list") / "size", _(_0_)),
                    ast.return_()),
            var_("item") = p_("list") / "first",
            ast.if_(m_("slots").missing(),
                    m_("slots") = ast.new_(tt_("Map", "keyType", _(std.Cell), "valueType", _(std.Slot)), "constructor")),
            ast.do_(ast.block(
                        var_("next") = true_(),
                        m_("slots").call("add", param("key", *var_("item") / "value" / "slotRole"), param("value", *var_("item") / "value")),
                        ast.if_(ast.has(*var_("item"), "next"),
                                var_("item") = *var_("item") / "next",
                                var_("next") = false_())),
                    ast.same(*var_("next"), true_())));

    typeStruct.addMethod("hasSlot")
        .parameters(
            param("slotRole", _(std.Cell)))
        .returnType(_(std.Boolean))
        .code(
            ast.if_(m_("slots").missing(),
                    ast.return_(false_())),
            ast.return_(m_("slots").call("hasKey", param("key", p_("slotRole")))));

    typeStruct.addMethod("removeSlot")
        .parameters(
            param("slotRole", _(std.Cell)))
        .code(
            ast.if_(m_("slots").missing(), ast.return_()),
            m_("slots").call("remove", param("key", p_("slotRole"))));
#pragma endregion
#pragma region Index
    auto& indexStruct
        = stdScope.addStruct("Index")
              .memberOf(_(std.Type_));

    indexStruct.addMethod("constructor")
        .code(
            ast.set(ast.self(), "type", ast.new_("Type", "constructorWithRecursiveType")),
            ast.set(m_("type"), "methods", ast.get(struct_("Index"), _("methods"))),
            ast.set(m_("type"), "memberOf", _(map(std.Type_, std.Type_, std.Index, std.Index))));

    indexStruct.addMethod("constructorWithSelfType")
        .parameters(
            param("indexType", _(std.Type_)))
        .code(
            ast.if_(ast.missing(p_("indexType"), _("sharedObject")),
                    ast.block(ast.set(p_("indexType"), "sharedObject", ast.new_(_(std.Slot))),
                              ast.set(p_("indexType") / "sharedObject", "slotRole", ast.self()),
                              ast.set(p_("indexType") / "sharedObject", "slotType", struct_("Index")))),
            ast.set(ast.self(), "type", p_("indexType")));

    /*
    void Index::insert(CellI& key, CellI& value)
    {
        if (&key == &"type") {
            throw "The type key can not be changed!";
        }
        m_slots[&key] = &value;
        if (m_recursiveType) {
            return;
        }
        Object& slot = *new Object(kb, kb.type.Slot);
        slot.set("slotRole", key);
        slot.set("slotType", kb.type.Slot);
        m_type->addSlot(key, slot);
    }
    */
    indexStruct.addMethod("insert")
        .parameters(
            param("key", _(std.Cell)),
            param("value", _(std.Cell)))
        .code(
            ast.if_(ast.same(p_("key"), _("type")),
                    ast.return_()),
            ast.set(ast.self(), p_("key"), p_("value")),
            ast.if_(ast.and_(ast.has(m_("type"), "sharedObject"), ast.same(m_("type") / "sharedObject" / "slotRole", ast.self())),
                    ast.return_()),
            m_("type").call("addSlot", param("slotRole", p_("key")), param("slotType", _(std.Slot))));

    indexStruct.addMethod("empty")
        .returnType(_(std.Boolean))
        .code(
            ast.return_(ast.call(m_("type") / "slots", "empty")));

    /*
    void Index::erase(CellI& role)
    {
        if (!m_type->hasSlot(role)) {
            return;
        }
        m_slots.erase(&role);
        m_type->removeSlot(role);
    }
    */
    indexStruct.addMethod("remove")
        .parameters(
            param("key", _(std.Cell)))
        .code(
            ast.if_(ast.not_(m_("type").call("hasSlot", param("slotRole", p_("key")))),
                    ast.return_()),
            ast.erase(ast.self(), p_("key")),
            m_("type").call("removeSlot", param("slotRole", p_("key"))));

    indexStruct.addMethod("size")
        .returnType(_(std.Number))
        .code(
            ast.return_(ast.call(m_("type") / "slots", "size")));
#pragma endregion
#pragma region Map
    stdScope.addStruct("Map")
        .subTypes(
            param("keyType", struct_("Cell")),
            param("valueType", struct_("Cell")),
            param("listType", tt_("List", "valueType", struct_("Cell"))))
        .memberOf(struct_("Container"))
        .members(
            member("list", st_("listType")),
            member("index", "Index"),
            member("size", _(std.Number)));

    auto& mapStructT
        = stdScope.addStructT("Map")
              .templateParams(
                  param("keyType", _(std.Type_)),
                  param("valueType", _(std.Type_)))
              .subTypes(
                  param("keyType", tp_("keyType")),
                  param("valueType", tp_("valueType")),
                  param("listType", tt_("List", "valueType", tp_("valueType"))))
              .memberOf(struct_("Map"))
              .members(
                  member("list", st_("listType")),
                  member("index", "Index"),
                  member("size", _(std.Number)));

    mapStructT.addMethod("constructor")
        .code(
            m_("size")  = _(_0_),
            m_("list")  = ast.new_(st_("listType"), "constructor"),
            m_("index") = ast.new_("Index", "constructor"));

    mapStructT.addMethod("constructorWithIndexType")
        .parameters(
            param("indexType", _(std.Type_)))
        .code(
            m_("size")  = _(_0_),
            m_("list")  = ast.new_(st_("listType"), "constructor"),
            m_("index") = ast.new_("Index", "constructorWithSelfType", param("indexType", p_("indexType"))));

    /*
    bool Map::hasKey(CellI& key)
    {
        return m_index.has(key);
    }
    */
    mapStructT.addMethod("hasKey")
        .parameters(
            param("key", tp_("keyType")))
        .returnType(_(std.Boolean))
        .code(
            ast.return_(ast.has(m_("index"), p_("key"))));

    /*
    CellI& Map::getValue(CellI& key)
    {
        if (m_index.has(key)) {
            return m_index[key][kb.ids.value];
        }
        throw "No such role!";
    }
    */
    mapStructT.addMethod("getValue")
        .parameters(
            param("key", tp_("keyType")))
        .returnType(tp_("valueType"))
        .code(
            ast.if_(ast.has(m_("index"), p_("key")),
                    ast.return_(m_("index") / p_("key") / "value"),
                    ast.return_(_("emptyObject"))));

    /*
    void Map::add(CellI& key, CellI& value)
    {
        if (&key == &"type") {
            throw "ids.type can not be stored in a map!";
        }
        if (m_index.has(key)) {
            throw "A value already registered with this role";
        }
        List::Item& item = *m_list.add(value);
        m_index.insert(key, item);
        ++m_size;
    }
    */
    mapStructT.addMethod("add")
        .parameters(
            param("key", tp_("keyType")),
            param("value", tp_("valueType")))
        .code(
            ast.if_(ast.same(p_("key"), _("type")),
                    ast.return_()),
            ast.if_(ast.has(m_("index"), p_("key")),
                    ast.return_()),
            m_("size")   = ast.add(m_("size"), _(_1_)),
            var_("item") = m_("list").call("add", param("value", p_("value"))),
            m_("index").call("insert", param("key", p_("key")), param("value", *var_("item"))));

    /*
    void Map::remove(CellI& key)
    {
        if (!m_index.has(key)) {
            return;
        }
        List::Item* item = &static_cast<List::Item&>(m_index[key]);
        m_list.removeItem(item);
        m_index.erase(key);
        --m_size;
    }
    */
    mapStructT.addMethod("remove")
        .parameters(
            param("key", tp_("keyType")))
        .code(
            ast.if_(ast.missing(m_("index"), p_("key")),
                    ast.return_()),
            m_("list").call("remove", param("item", m_("index") / p_("key"))),
            m_("index").call("remove", param("key", p_("key"))),
            m_("size") = ast.subtract(m_("size"), _(_1_)));

    mapStructT.addMethod("size")
        .returnType(_(std.Number))
        .code(
            ast.return_(m_("size")));

    mapStructT.addMethod("empty")
        .returnType(_(std.Boolean))
        .code(
            ast.return_(ast.equal(m_("size"), _(_0_))));

    mapStructT.addMethod("first")
        .returnType(tp_("valueType"))
        .code(
            ast.return_(m_("list") / "first" / "value"));

    mapStructT.addMethod("last")
        .returnType(tp_("valueType"))
        .code(
            ast.return_(m_("list") / "last" / "value"));

    mapStructT.addMethod("begin")
        .returnType(tt_("ListItem", "valueType", tp_("valueType")))
        .code(
            ast.return_(m_("list") / "first"));

    mapStructT.addMethod("end")
        .returnType(tt_("ListItem", "valueType", tp_("valueType")))
        .code(
            ast.return_(m_("list") / "last"));
#pragma endregion
#pragma region TrieMap
    stdScope.addStruct("KVPair")
        .members(
            member("key", "Cell"),
            member("value", "Cell"));

    auto& kvPairT
        = stdScope.addStructT("KVPair")
              .templateParams(
                  param("keyType", _(std.Type_)),
                  param("valueType", _(std.Type_)))
              .subTypes(
                  param("keyType", tp_("keyType")),
                  param("valueType", tp_("valueType")))
              .memberOf(struct_("KVPair"))
              .members(
                  member("key", tp_("keyType")),
                  member("value", tp_("valueType")));

    kvPairT.addMethod("constructor")
        .parameters(
            param("key", tp_("keyType")),
            param("value", tp_("valueType")))
        .code(
            m_("key")   = p_("key"),
            m_("value") = p_("value"));

    // TODO This can be a template but nevermind ...
    stdScope.addStruct("TrieMapNode")
        .members(
            member("children", "Index"),
            member("data", "ListItem"),
            member("parent", "TrieMapNode"));


    stdScope.addStruct("TrieMap")
        .subTypes(
            param("keyType", struct_("Cell")),
            param("valueType", struct_("Cell")),
            param("pairType", tt_("KVPair", "keyType", struct_("Cell"), "valueType", struct_("Cell"))),
            param("listType", tt_("List", "valueType", st_("pairType"))))
        .memberOf(struct_("Container"))
        .members(
            member("list", st_("listType")),
            member("rootNode", struct_("TrieMapNode")),
            member("size", _(std.Number)));

    auto& trieMapStructT
        = stdScope.addStructT("TrieMap")
              .templateParams(
                  param("keyType", _(std.Type_)),
                  param("valueType", _(std.Type_)))
              .subTypes(
                  param("keyType", tp_("keyType")),
                  param("valueType", tp_("valueType")),
                  param("pairType", tt_("KVPair", "keyType", tp_("keyType"), "valueType", tp_("valueType"))),
                  param("listType", tt_("List", "valueType", st_("pairType"))))
              .memberOf(_(std.Container), _(std.TrieMap))
              .members(
                  member("list", st_("listType")),
                  member("rootNode", _(std.TrieMapNode)),
                  member("size", _(std.Number)));

    trieMapStructT.addMethod("constructor")
        .code(
            m_("list")  = ast.new_(st_("listType"), "constructor"),
            m_("rootNode") = ast.new_(_(std.TrieMapNode)),
            m_("size")  = _(_0_));

    /*
    bool TrieMap::hasKey(CellI& key)
    {
        CellI* currentNode = &m_rootNode;

        if (isA(key, kb.type.List)) {
            throw "Key is not a list!";
        }

        Visitor::visitList(key, [this, &currentNode](CellI& keyItem, int i, bool& stop) {
            CellI* children = nullptr;
            if (currentNode->missing(kb.ids.children)) {
                stop        = true;
                currentNode = nullptr;
                return;
            }
            Index& childrenIndex = static_cast<Index&>(currentNode->get(kb.ids.children));
            if (childrenIndex.has(keyItem)) {
                children = &childrenIndex.get(keyItem);
            } else {
                stop        = true;
                currentNode = nullptr;
                return;
            }
            currentNode = children;
        });

        if (!currentNode || currentNode->missing(kb.ids.data)) {
            return false;
        }

        return true;
    }
    */
    trieMapStructT.addMethod("hasKey")
        .parameters(
            param("key", tp_("keyType")))
        .returnType(_(std.Boolean))
        .code(
            var_("currentNode") = m_("rootNode"),
            var_("keyItem")     = _(ids.emptyObject),
            ast.if_(ast.has(p_("key"), "first"),
                    var_("keyItem") = p_("key") / "first"),
            ast.while_(ast.notSame(*var_("keyItem"), _(ids.emptyObject)),
                       ast.block(
                           var_("keyItemObj") = *var_("keyItem") / "value",
                           var_("child")      = _(ids.emptyObject),
                           ast.if_(ast.missing(*var_("currentNode"), "children"),
                                   ast.return_(false_())),
                           var_("childrenIndex") = *var_("currentNode") / "children",
                           ast.if_(ast.has(*var_("childrenIndex"), *var_("keyItemObj")),
                                   var_("child") = *var_("childrenIndex") / *var_("keyItemObj"),
                                   ast.return_(false_())),
                           var_("currentNode") = *var_("child"),
                           ast.if_(ast.has(*var_("keyItem"), "next"),
                                   var_("keyItem") = *var_("keyItem") / "next",
                                   var_("keyItem") = _(ids.emptyObject)))),
            ast.if_(ast.missing(*var_("currentNode"), "data"),
                    ast.return_(false_())),
            ast.return_(true_()));

    /*
    CellI& TrieMap::getValue(CellI& key)
    {
        if (isA(key, kb.type.List)) {
            throw "Key is not a list!";
        }

        CellI* currentNode = &m_rootNode;

        Visitor::visitList(key, [this, &currentNode](CellI& keyItem, int i, bool& stop) {
            CellI* children = nullptr;
            if (currentNode->missing(kb.ids.children)) {
                stop        = true;
                currentNode = nullptr;
                return;
            }
            Index& childrenIndex = static_cast<Index&>(currentNode->get(kb.ids.children));
            if (childrenIndex.has(keyItem)) {
                children = &childrenIndex.get(keyItem);
            } else {
                stop        = true;
                currentNode = nullptr;
                return;
            }
            currentNode = children;
        });

        if (!currentNode || currentNode->missing(kb.ids.data)) {
            throw "No such key!";
        }

        return (*currentNode)[kb.ids.data][kb.ids.value][kb.ids.value];
    }
    */
    trieMapStructT.addMethod("getValue")
        .parameters(
            param("key", tp_("keyType")))
        .returnType(tp_("valueType"))
        .code(
            var_("currentNode") = m_("rootNode"),
            var_("keyItem")     = _(ids.emptyObject),
            ast.if_(ast.has(p_("key"), "first"),
                    var_("keyItem") = p_("key") / "first"),
            ast.while_(ast.notSame(*var_("keyItem"), _(ids.emptyObject)),
                       ast.block(
                           var_("keyItemObj") = *var_("keyItem") / "value",
                           var_("child")      = _(ids.emptyObject),
                           ast.if_(ast.missing(*var_("currentNode"), "children"),
                                   ast.return_(_(ids.emptyObject))),
                           var_("childrenIndex") = *var_("currentNode") / "children",
                           ast.if_(ast.has(*var_("childrenIndex"), *var_("keyItemObj")),
                                   var_("child") = *var_("childrenIndex") / *var_("keyItemObj"),
                                   ast.return_(_(ids.emptyObject))),
                           var_("currentNode") = *var_("child"),
                           ast.if_(ast.has(*var_("keyItem"), "next"),
                                   var_("keyItem") = *var_("keyItem") / "next",
                                   var_("keyItem") = _(ids.emptyObject)))),
            ast.if_(ast.missing(*var_("currentNode"), "data"),
                    ast.return_(_(ids.emptyObject))),
            ast.return_(*var_("currentNode") / "data" / "value" / "value"));

    /*
    void TrieMap::add(CellI& key, CellI& value)
    {
        if (isA(key, kb.type.List)) {
            throw "Key is not a list!";
        }

        CellI* currentNode = &m_rootNode;

        Visitor::visitList(key, [this, &currentNode](CellI& keyItem, int i, bool& stop) {
            CellI* child = nullptr;
            if (currentNode->missing(kb.ids.children)) {
                currentNode->set(kb.ids.children, *new Index(kb));
            }
            Index& childrenIndex = static_cast<Index&>(currentNode->get(kb.ids.children));
            if (childrenIndex.has(keyItem)) {
                child = &childrenIndex.get(keyItem);
            } else {
                child = new Object(kb, kb.type.TrieMapNode);
                child->set(kb.ids.parent, *currentNode);
                childrenIndex.insert(keyItem, *child);
            }
            currentNode = child;
        });

        List::Item& item = *m_list.add(kb.type.kvPair(key, value));
        currentNode->set(kb.ids.data, item);
        ++m_size;
    }
    */
    trieMapStructT.addMethod("add")
        .parameters(
            param("key", tp_("keyType")),
            param("value", tp_("valueType")))
        .code(
            var_("currentNode") = m_("rootNode"),
            var_("keyItem")     = _(ids.emptyObject),
            ast.if_(ast.has(p_("key"), "first"),
                    var_("keyItem") = p_("key") / "first"),
            ast.while_(ast.notSame(*var_("keyItem"), _(ids.emptyObject)),
                       ast.block(
                           var_("keyItemObj") = *var_("keyItem") / "value",
                           var_("child")      = _(ids.emptyObject),
                           ast.if_(ast.missing(*var_("currentNode"), "children"),
                                   ast.set(*var_("currentNode"), "children", ast.new_("Index", "constructor"))),
                           var_("childrenIndex") = *var_("currentNode") / "children",
                           ast.if_(ast.has(*var_("childrenIndex"), *var_("keyItemObj")),
                                   var_("child") = *var_("childrenIndex") / *var_("keyItemObj")),
                           ast.block(
                               var_("child") = ast.new_(_(std.TrieMapNode)),
                               ast.set(*var_("child"), "parent", *var_("currentNode")),
                               ast.call(*var_("childrenIndex"), "insert", param("key", *var_("keyItemObj")), param("value", *var_("child")))),
                           var_("currentNode") = *var_("child"),
                           ast.if_(ast.has(*var_("keyItem"), "next"),
                                   var_("keyItem") = *var_("keyItem") / "next",
                                   var_("keyItem") = _(ids.emptyObject))
                           )),
            var_("item") = m_("list").call("add", param("value", ast.new_(st_("pairType"), "constructor", param("key", p_("key")), param("value", p_("value"))))),
            ast.set(*var_("currentNode"), "data", *var_("item")),
            m_("size") = ast.add(m_("size"), _(_1_)));

    /*
    void TrieMap::remove(CellI& key)
    {
        if (isA(key, kb.type.List)) {
            throw "Key is not a list!";
        }

        if (&key[kb.ids.size] == &kb._0_) {
            return;
        }

        CellI* currentNode    = &m_rootNode;

        Visitor::visitList(key, [this, &currentNode](CellI& keyItem, int i, bool& stop) {
            CellI* children = nullptr;
            if (currentNode->missing(kb.ids.children)) {
                stop        = true;
                currentNode = nullptr;
                return;
            }
            Index& childrenIndex = static_cast<Index&>(currentNode->get(kb.ids.children));
            if (childrenIndex.has(keyItem)) {
                children = &childrenIndex.get(keyItem);
            } else {
                stop        = true;
                currentNode = nullptr;
                return;
            }
            currentNode = children;
        });

        if (!currentNode || currentNode->missing(kb.ids.data)) {
            return;
        }
        List::Item* valueItem = &static_cast<List::Item&>((*currentNode)[kb.ids.data]);
        currentNode->erase(kb.ids.data);

        CellI* keyItemPtr = &key[kb.ids.last];
        while (currentNode->has(kb.ids.parent)) {
            CellI& keyItem = *keyItemPtr;
            CellI& parent = currentNode->get(kb.ids.parent);
            CellI& child = *currentNode;
            if (child.missing(kb.ids.data)) {
                if (child.missing(kb.ids.children) || ( child.has(kb.ids.children) && static_cast<Index&>(child[kb.ids.children]).empty())) {
                    delete currentNode;
                    parent[kb.ids.children].erase(keyItem[kb.ids.value]);
                }
            }
            currentNode = &parent;
            if (keyItem.has(kb.ids.previous)) {
                keyItemPtr = &keyItem[kb.ids.previous];
            } else {
                break;
            }
        }
        if (!valueItem) {
            return;
        }
        m_list.removeItem(valueItem);
        --m_size;
    }
    */
    trieMapStructT.addMethod("remove")
        .parameters(
            param("key", tp_("keyType")))
        .code(
            var_("currentNode") = m_("rootNode"),
            var_("keyItem")     = _(ids.emptyObject),
            ast.if_(ast.has(p_("key"), "first"),
                    var_("keyItem") = p_("key") / "first"),
            ast.while_(ast.notSame(*var_("keyItem"), _(ids.emptyObject)),
                       ast.block(
                           var_("keyItemObj") = *var_("keyItem") / "value",
                           var_("child")      = _(ids.emptyObject),
                           ast.if_(ast.missing(*var_("currentNode"), "children"),
                                   ast.return_()),
                           var_("childrenIndex") = *var_("currentNode") / "children",
                           ast.if_(ast.has(*var_("childrenIndex"), *var_("keyItemObj")),
                                   var_("child") = *var_("childrenIndex") / *var_("keyItemObj"),
                                   ast.return_()),
                           var_("currentNode") = *var_("child"),
                           ast.if_(ast.has(*var_("keyItem"), "next"),
                                   var_("keyItem") = *var_("keyItem") / "next",
                                   var_("keyItem") = _(ids.emptyObject)))),
            ast.if_(ast.missing(*var_("currentNode"), "data"),
                    ast.return_()),
            var_("valueItem") = *var_("currentNode") / "data",
            ast.erase(*var_("currentNode"), "data"),
            var_("keyItem") = p_("key") / "last",
            ast.while_(ast.has(*var_("currentNode"), "parent"),
                       ast.block(
                           var_("parent") = *var_("currentNode") / "parent",
                           var_("child")  = *var_("currentNode"),
                           ast.if_(ast.missing(*var_("child"), "data"),
                                   ast.if_(ast.or_(ast.missing(*var_("child"), "children"), ast.and_(ast.has(*var_("child"), "children"), ast.call(*var_("child") / "children", "empty"))),
                                           ast.block(
                                               ast.delete_(*var_("currentNode")),
                                               ast.erase(*var_("parent") / "children", *var_("keyItem") / "value")))),
                           var_("currentNode") = *var_("parent"),
                           ast.if_(ast.has(*var_("keyItem"), "previous"),
                                   var_("keyItem") = *var_("keyItem") / "previous",
                                   ast.break_())
                           )),
            m_("list").call("remove", param("item", *var_("valueItem"))),
            m_("size") = ast.subtract(m_("size"), _(_1_)));

    trieMapStructT.addMethod("size")
        .returnType(_(std.Number))
        .code(
            ast.return_(m_("size")));

    trieMapStructT.addMethod("empty")
        .returnType(_(std.Boolean))
        .code(
            ast.return_(ast.equal(m_("size"), _(_0_))));

    trieMapStructT.addMethod("first")
        .returnType(tp_("valueType"))
        .code(
            ast.return_(m_("list") / "first" / "value"));

    trieMapStructT.addMethod("last")
        .returnType(tp_("valueType"))
        .code(
            ast.return_(m_("list") / "last" / "value"));

    trieMapStructT.addMethod("begin")
        .returnType(tt_("ListItem", "valueType", tp_("valueType")))
        .code(
            ast.return_(m_("list") / "first"));

    trieMapStructT.addMethod("end")
        .returnType(tt_("ListItem", "valueType", tp_("valueType")))
        .code(
            ast.return_(m_("list") / "last"));
#pragma endregion
#pragma region Set
    auto& setStructT
        = stdScope.addStructT("Set")
              .templateParams(
                  param("keyType", _(std.Type_)),
                  param("valueType", _(std.Type_)))
              .subTypes(
                  param("valueType", tp_("valueType")),
                  param("listType", tt_("List", "valueType", tp_("valueType"))))
              .memberOf(_(std.Container))
              .members(
                  member("index", struct_("Index")),
                  member("size", _(std.Number)));

    setStructT.addMethod("constructor")
        .code(
            m_("size")  = _(_0_),
            m_("index") = ast.new_(struct_("Index"), "constructor"));

    setStructT.addMethod("add")
        .parameters(
            param("value", tp_("valueType")))
        .code(
            ast.if_(ast.has(m_("index"), p_("value")),
                    ast.return_()),
            ast.call(m_("index"), "insert", param("key", p_("value")), param("value", p_("value"))),
            m_("size") = ast.add(m_("size"), _(_1_)));

    setStructT.addMethod("contains")
        .parameters(
            param("value", tp_("valueType")))
        .returnType(_(std.Boolean))
        .code(
            ast.return_(ast.has(m_("index"), p_("value"))));

    setStructT.addMethod("remove")
        .parameters(
            param("value", tp_("valueType")))
        .code(
            ast.if_(ast.missing(m_("index"), p_("value")),
                    ast.return_()),
            ast.call(m_("index"), "remove", param("key", p_("value"))),
            m_("size") = ast.subtract(m_("size"), _(_1_)));
#
    setStructT.addMethod("first")
        .returnType(tp_("valueType"))
        .code(
            ast.return_(m_("index") / "type" / "slots" / "list" / "first" / "value" / "slotRole"));

    setStructT.addMethod("last")
        .returnType(tp_("valueType"))
        .code(
            ast.return_(m_("index") / "type" / "slots" / "list" / "last" / "value" / "slotRole"));

    setStructT.addMethod("begin")
        .returnType(tt_("ListItem", "valueType", tp_("valueType")))
        .code(
            ast.return_(m_("index") / "type" / "slots" / "list" / "last"));

    setStructT.addMethod("end")
        .returnType(tt_("ListItem", "valueType", tp_("valueType")))
        .code(
            ast.return_(m_("list") / "last"));

    setStructT.addMethod("size")
        .returnType(_(std.Number))
        .code(
            ast.return_(m_("size")));

    setStructT.addMethod("empty")
        .returnType(_(std.Boolean))
        .code(ast.return_(ast.equal(m_("size"), _(_0_))));
#pragma endregion
}

void Brain::createArcSolver()
{
    auto& arcScope = globalScope.addScope("arc");

    auto& demonstrationStruct
        = arcScope.addStruct("Demonstration")
              .members(
                  member("input", _(std.Picture)),
                  member("output", _(std.Picture)));

    auto& taskStruct
        = arcScope.addStruct("Task")
              .members(
                  member("examples", tt_("std::List", "valueType", "Demonstration")),
                  member("challenge", _(std.Picture)),
                  member("solution", _(std.Picture)));

    auto& colorStruct
        = arcScope.addStruct("Color")
              .members(
                  member("red", _(std.Number)),
                  member("green", _(std.Number)),
                  member("blue", _(std.Number)));

    // struct Pixel
    auto& pixelStruct
        = arcScope.addStruct("Pixel")
              .members(
                  member("x", _(std.Number)),
                  member("y", _(std.Number)));

    pixelStruct.addMethod("constructor")
        .parameters(
            param("x", _(std.Number)),
            param("y", _(std.Number)))
        .code(
            m_("x") = p_("x"),
            m_("y") = p_("y"));

    // struct Vector
    auto& vectorStruct
        = arcScope.addStruct("Vector")
              .members(
                  member("x", _(std.Number)),
                  member("y", _(std.Number)));

    vectorStruct.addMethod("constructor")
        .parameters(
            param("x", _(std.Number)),
            param("y", _(std.Number)))
        .code(
            m_("x") = p_("x"),
            m_("y") = p_("y"));

    // struct VectorShape
    auto& vectorShapeStruct
        = arcScope.addStruct("VectorShape")
              .members(
                  member("color", _(std.Color)),
                  member("vectors", tt_("std::List", "valueType", "Vector")),
                  member("firstPixel", "Pixel"));

    vectorShapeStruct.addMethod("constructor")
        .parameters(
            param("color", _(std.Color)))
        .code(
            m_("color") = p_("color"),
            m_("vectors") = ast.new_(tt_("std::List", "valueType", "Vector"), "constructor"));

    /*
     void VectorShape::fromPixels(const std::vector<Pixel>& pixels)
     {
         m_vectors.clear();
         firstPixel(pixels.front());
         const Pixel* prevPixel = &pixels.front();
         bool isFirstPixel      = true;

         for (const auto& pixel : pixels) {
             if (isFirstPixel) {
                 isFirstPixel = false;
                 continue;
             }
             const Pixel* currPixel = &pixel;
             m_vectors.push_back({ currPixel->x - prevPixel->x, currPixel->y - prevPixel->y });
             prevPixel = currPixel;
         }
     }
    */
    vectorShapeStruct.addMethod("fromPixels")
        .parameters(
            param("pixels", tt_("std::List", "valueType", "Pixel")))
        .code(
            m_("firstPixel") = p_("pixels") / "first" / "value",
            var_("prevPixel") = m_("firstPixel"),
            var_("isFirstPixel") = true_(),
            var_("pixel") = _(ids.emptyObject),
            ast.if_(ast.has(p_("pixels"), "first"),
                    var_("pixel") = p_("pixels") / "first"),
            ast.while_(ast.notSame(*var_("pixel"), _(ids.emptyObject)),
                       ast.block(
                           ast.if_(ast.same(*var_("isFirstPixel"), true_()),
                                   ast.block(
                                       var_("isFirstPixel") = false_(),
                                       ast.if_(ast.has(*var_("pixel"), "next"),
                                               var_("pixel") = *var_("pixel") / "next",
                                               var_("pixel") = _(ids.emptyObject)),
                                       ast.continue_())),
                           var_("vector") = ast.new_("Vector", "constructor", param("x", ast.subtract(*var_("pixel") / "value" / "x", *var_("prevPixel") / "x")), param("y", ast.subtract(*var_("pixel") / "value" / "y", *var_("prevPixel") / "y"))),
                           m_("vectors").call("add", param("value", *var_("vector"))),
                           var_("prevPixel") = *var_("pixel") / "value",
                           ast.if_(ast.has(*var_("pixel"), "next"),
                                   var_("pixel") = *var_("pixel") / "next",
                                   var_("pixel") = _(ids.emptyObject)))));

    // struct ShapePixel
    auto& shapePixelStruct
        = arcScope.addStruct("ShapePixel")
              .members(
                  member("shape", "Shape"),
                  member("pixel", _(std.Pixel)));

    shapePixelStruct.addMethod("constructor")
        .parameters(
            param("shape", struct_("Shape")),
            param("pixel", _(std.Pixel)))
        .code(
            m_("shape") = p_("shape"),
            m_("pixel") = p_("pixel"));

    // struct Shape
    auto& shapeStruct
        = arcScope.addStruct("Shape")
              .members(
                  member("id", _(std.Number)),
                  member("color", "Color"),
                  member("width", _(std.Number)),
                  member("height", _(std.Number)),
                  member("hybridPixels", tt_("std::Set", "valueType", _(std.Pixel))),
                  member("pixels", tt_("std::List", "valueType", "Pixel")));

    /*
    Shape(int id, input::Color color, int width, int height) :
        m_id(id), m_color(color), m_width(width), m_height(height) { }
    */
    shapeStruct.addMethod("constructor")
        .parameters(
            param("id", _(std.Number)),
            param("color", struct_("Color")),
            param("width", _(std.Number)),
            param("height", _(std.Number)))
        .code(
            m_("id")           = p_("id"),
            m_("color")        = p_("color"),
            m_("width")        = p_("width"),
            m_("height")       = p_("height"),
            m_("hybridPixels") = ast.new_(tt_("std::Set", "valueType", _(std.Pixel)), "constructor"),
            m_("pixels")       = ast.new_(tt_("std::List", "valueType", "Pixel"), "constructor"));

    /*
    void Shape::addPixel(cells::hybrid::Pixel& pixel)
    {
        m_pixels.push_back({ pixel.m_x.value(), pixel.m_y.value() });
        m_hybridPixels.insert(&pixel);
    }
    */
    shapeStruct.addMethod("addPixel")
        .parameters(
            param("pixel", struct_("Pixel")))
        .code(
            m_("pixels").call("add", param("value", ast.new_("Pixel", "constructor", param("x", p_("pixel") / _(coordinates.x)), param("y", p_("pixel") / _(coordinates.y))))),
            m_("hybridPixels").call("add", param("value", p_("pixel"))));

    /*
    bool Shape::hasPixel(cells::hybrid::Pixel& pixel) const
    {
        return m_hybridPixels.find(&pixel) != m_hybridPixels.end();
    }
    */
    shapeStruct.addMethod("hasPixel")
        .parameters(
            param("pixel", _(std.Pixel)))
        .returnType(_(std.Boolean))
        .code(
            ast.return_(m_("hybridPixels").call("contains", param("value", p_("pixel")))));

    shapeStruct.addMethod("toVectorShape")
        .returnType(struct_("VectorShape"))
        .code(
            var_("ret") = ast.new_("VectorShape", "constructor", param("color", m_("color"))),
            ast.call(*var_("ret"), "fromPixels", param("pixels", m_("pixels"))),
            ast.return_(*var_("ret")));

    // struct Shaper
    auto& shaperStruct
        = arcScope.addStruct("Shaper")
              .subTypes(
                  param("tableType", tt_("std::Map", "keyType", _(std.Number), "valueType", tt_("std::Map", "keyType", _(std.Number), "valueType", "Shape"))))
              .members(
                  member("width", _(std.Number)),
                  member("height", _(std.Number)),
                  member("picture", _(std.Picture)),
                  member("shapePixels", st_("tableType")),
                  member("shapes", tt_("std::List", "valueType", "Shape")),
                  member("shapeSet", tt_("std::Set", "valueType", "Shape")),
                  member("inputPixels", tt_("std::Set", "valueType", _(std.Pixel))));

    /*
    Shaper::Shaper(const cells::hybrid::Picture& picture) :
        m_width(picture.width()),
        m_height(picture.height()),
        m_picture(picture),
        kb(picture.kb)
    {
        processInputPixels();
    }
    */
    shaperStruct.addMethod("constructor")
        .parameters(
            param("picture", _(std.Picture)))
        .code(
            m_("picture")     = p_("picture"),
            m_("width")       = p_("picture") / "width",
            m_("height")      = p_("picture") / "height",
            m_("shapes")      = ast.new_(tt_("std::List", "valueType", "Shape"), "constructor"),
            m_("shapePixels") = ast.new_(st_("tableType"), "constructor"),
            m_("shapeSet")    = ast.new_(tt_("std::Set", "valueType", "Shape"), "constructor"),
            m_("inputPixels") = ast.new_(tt_("std::Set", "valueType", _(std.Pixel)), "constructor"),
            ast.self().call("processInputPixels"));
    /*
    void Shaper::processInputPixels()
    {
        std::vector<cells::hybrid::Pixel>& pixels = const_cast<cells::hybrid::Picture&>(m_picture).pixels();
        for (cells::hybrid::Pixel& pixel : pixels) {
            m_inputPixels.insert(&pixel);
        }
    }
    */
    shaperStruct.addMethod("processInputPixels")
        .code(
            var_("pixels") = m_("picture") / "pixels",
            var_("pixel")  = _(ids.emptyObject),
            ast.if_(ast.has(*var_("pixels"), "first"),
                    var_("pixel") = *var_("pixels") / "first"),
            ast.while_(ast.notSame(*var_("pixel"), _(ids.emptyObject)),
                       ast.block(
                           m_("inputPixels").call("add", param("value", *var_("pixel") / "value")),
                           ast.if_(ast.has(*var_("pixel"), "next"),
                                   var_("pixel") = *var_("pixel") / "next",
                                   var_("pixel") = _(ids.emptyObject)))));

    /*
    void Shaper::process()
    {
        int shapeId = 1;
        while (!m_inputPixels.empty()) {
            cells::hybrid::Pixel& firstPixel = **m_inputPixels.begin();
            m_shapes.push_back(std::make_shared<Shape>(shapeId++, firstPixel.color(), m_width, m_height));
            Shape& shape = *m_shapes.back();
            std::set<cells::hybrid::Pixel*> checkPixels;
            checkPixels.insert(&firstPixel);
            while (!checkPixels.empty()) {
                auto checkPixelIt                = checkPixels.begin();
                cells::hybrid::Pixel& checkPixel = **checkPixelIt;
                processPixel(shape, checkPixels, checkPixel);
                checkPixels.erase(checkPixelIt);
            }
            shape.sortPixels();
        }
        std::sort(m_shapes.begin(), m_shapes.end(),
            [](const std::shared_ptr<Shape>& lhs, const std::shared_ptr<Shape>& rhs)
            { return *lhs < *rhs; }
        );
    }
    */
    shaperStruct.addMethod("process")
        .code(
            var_("shapeId") = _(_1_),
            ast.while_(ast.not_(m_("inputPixels").call("empty")),
                       ast.block(
                           var_("firstPixel")  = m_("inputPixels").call("first"),
                           var_("shape")       = ast.new_("Shape", "constructor", param("id", *var_("shapeId")), param("color", *var_("firstPixel") / "color"), param("width", m_("width")), param("height", m_("height"))),
                           var_("shapeId")     = ast.add(*var_("shapeId"), _(_1_)),
                           var_("checkPixels") = ast.new_(tt_("std::Set", "valueType", _(std.Pixel)), "constructor"),
                           ast.call(*var_("checkPixels"), "add", param("value", *var_("firstPixel"))),
                           ast.while_(ast.not_(ast.call(*var_("checkPixels"), _("empty"))),
                                      ast.block(
                                          var_("checkPixel") = ast.call(*var_("checkPixels"), "first"),
                                          ast.self().call("processPixel", param("shape", *var_("shape")), param("checkPixels", *var_("checkPixels")), param("checkPixel", *var_("checkPixel"))),
                                          ast.call(*var_("checkPixels"), "remove", param("value", *var_("checkPixel"))))))),
            var_("y") = _(_0_),
            ast.while_(ast.lessThan(*var_("y"), m_("height")),
                       ast.block(
                           var_("colX") = m_("shapePixels").call("getValue", param("key", *var_("y"))),
                           var_("x")    = _(_0_),
                           ast.while_(ast.lessThan(*var_("x"), m_("width")),
                                      ast.block(
                                          var_("shapePixel") = ast.call(*var_("colX"), "getValue", param("key", *var_("x"))),
                                          var_("shape")      = *var_("shapePixel") / "shape",
                                          var_("pixel")      = *var_("shapePixel") / "pixel",
                                          ast.call(*var_("shape"), "addPixel", param("pixel", *var_("pixel"))),
                                          ast.if_(ast.not_(m_("shapeSet").call("contains", param("value", *var_("shape")))),
                                                  ast.block(
                                                      m_("shapeSet").call("add", param("value", *var_("shape"))),
                                                      m_("shapes").call("add", param("value", *var_("shape"))))),
                                          var_("x") = ast.add(*var_("x"), _(_1_)))),
                           var_("y") = ast.add(*var_("y"), _(_1_)))));

    /*
    void Shaper::processPixel(Shape& shape, std::set<cells::hybrid::Pixel*>& checkPixels, cells::hybrid::Pixel& checkPixel)
    {
        shape.addPixel(checkPixel);
        m_inputPixels.erase(&checkPixel);

        if (cells::hybrid::Pixel* pixel = processAdjacentPixel(kb.directions.up, shape, checkPixels, checkPixel)) {
            processAdjacentPixel(kb.directions.left, shape, checkPixels, *pixel);
            processAdjacentPixel(kb.directions.right, shape, checkPixels, *pixel);
        }
        if (cells::hybrid::Pixel* pixel = processAdjacentPixel(kb.directions.down, shape, checkPixels, checkPixel)) {
            processAdjacentPixel(kb.directions.left, shape, checkPixels, *pixel);
            processAdjacentPixel(kb.directions.right, shape, checkPixels, *pixel);
        }
        processAdjacentPixel(kb.directions.left, shape, checkPixels, checkPixel);
        processAdjacentPixel(kb.directions.right, shape, checkPixels, checkPixel);
    }
    */
    shaperStruct.addMethod("processPixel")
        .parameters(
            param("shape", struct_("Shape")),
            param("checkPixels", tt_("std::Set", "valueType", "Pixel")),
            param("checkPixel", struct_("Pixel")))
        .code(
            ast.if_(ast.not_(m_("shapePixels").call("hasKey", param("key", p_("checkPixel") / _(coordinates.y)))),
                    m_("shapePixels").call("add", param("key", p_("checkPixel") / _(coordinates.y)), param("value", ast.new_(st_("tableType"), "constructor")))),
            var_("colX") = m_("shapePixels").call("getValue", param("key", p_("checkPixel") / _(coordinates.y))),
            ast.call(*var_("colX"), "add", param("key", p_("checkPixel") / _(coordinates.x)), param("value", ast.new_("ShapePixel", "constructor", param("shape", p_("shape")), param("pixel", p_("checkPixel"))))),
            m_("inputPixels").call("remove", param("value", p_("checkPixel"))),
            var_("pixel") = ast.self().call("processAdjacentPixel", param("direction", _(directions.up)), param("shape", p_("shape")), param("checkPixels", p_("checkPixels")), param("checkPixel", p_("checkPixel"))),
            ast.if_(ast.notSame(*var_("pixel"), _(ids.emptyObject)),
                    ast.block(
                        ast.self().call("processAdjacentPixel", param("direction", _(directions.left)), param("shape", p_("shape")), param("checkPixels", p_("checkPixels")), param("checkPixel", *var_("pixel"))),
                        ast.self().call("processAdjacentPixel", param("direction", _(directions.right)), param("shape", p_("shape")), param("checkPixels", p_("checkPixels")), param("checkPixel", *var_("pixel"))))),
            var_("pixel") = ast.self().call("processAdjacentPixel", param("direction", _(directions.down)), param("shape", p_("shape")), param("checkPixels", p_("checkPixels")), param("checkPixel", p_("checkPixel"))),
            ast.if_(ast.notSame(*var_("pixel"), _(ids.emptyObject)),
                    ast.block(
                        ast.self().call("processAdjacentPixel", param("direction", _(directions.left)), param("shape", p_("shape")), param("checkPixels", p_("checkPixels")), param("checkPixel", *var_("pixel"))),
                        ast.self().call("processAdjacentPixel", param("direction", _(directions.right)), param("shape", p_("shape")), param("checkPixels", p_("checkPixels")), param("checkPixel", *var_("pixel"))))),
            ast.self().call("processAdjacentPixel", param("direction", _(directions.left)), param("shape", p_("shape")), param("checkPixels", p_("checkPixels")), param("checkPixel", p_("checkPixel"))),
            ast.self().call("processAdjacentPixel", param("direction", _(directions.right)), param("shape", p_("shape")), param("checkPixels", p_("checkPixels")), param("checkPixel", p_("checkPixel"))));

    /*
    cells::hybrid::Pixel* Shaper::processAdjacentPixel(cells::CellI& direction, Shape& shape, std::set<cells::hybrid::Pixel*>& checkPixels, cells::hybrid::Pixel& checkPixel)
    {
        if (checkPixel.has(direction)) {
            cells::hybrid::Pixel& pixel = static_cast<cells::hybrid::Pixel&>(checkPixel[direction]);
            if (pixel.color() == shape.color() && !shape.hasPixel(pixel)) {
                checkPixels.insert(&pixel);
            }
            return &pixel;
        }

        return nullptr;
    }
    */
    shaperStruct.addMethod("processAdjacentPixel")
        .parameters(
            param("direction", _(std.Directions)),
            param("shape", struct_("Shape")),
            param("checkPixels", tt_("std::Set", "valueType", _(std.Pixel))),
            param("checkPixel", _(std.Pixel)))
        .returnType(_(std.Pixel))
        .code(
            ast.if_(ast.has(p_("checkPixel"), p_("direction")),
                    ast.block(
                        var_("pixel") = p_("checkPixel") / p_("direction"),
                        ast.if_(m_("shapePixels").call("hasKey", param("key", *var_("pixel") / _(coordinates.y))),
                                ast.block(
                                    var_("colX") = m_("shapePixels").call("getValue", param("key", *var_("pixel") / _(coordinates.y))),
                                    ast.if_(ast.call(*var_("colX"), "hasKey", param("key", *var_("pixel") / _(coordinates.x))),
                                            ast.block(
                                                var_("shape") = ast.get(ast.call(*var_("colX"), "getValue", param("key", *var_("pixel") / _(coordinates.x))), "shape"),
                                                ast.if_(ast.same(p_("shape"), *var_("shape")),
                                                        ast.return_(*var_("pixel"))))))),
                        ast.if_(ast.equal(*var_("pixel") / "color", p_("shape") / "color"),
                                ast.call(p_("checkPixels"), "add", param("value", *var_("pixel")))),
                        ast.return_(*var_("pixel"))),
                    ast.return_(_(ids.emptyObject))));
}

void Brain::createTests()
{
    auto& testScope = globalScope.addScope("test");

    auto& testFunction = testScope.addFunction("testFunction");
    testFunction.code(
        var_("result") = ast.new_(struct_("std::Index")));

    auto& testVariable = testScope.addVariable(id("testVariable"));

    auto& testStruct = testScope.addStruct("Test");
    CellI& fullId    = testStruct.getFullId();

    testStruct.addMethod("testCreateNewListOfNumbers")
        .code(
            var_("result") = ast.new_(struct_("std::Index")),
            var_("result") = ast.new_(tt_("std::List", "valueType", _(std.Number))),
            var_("result") = ast.new_(tt_("std::List", "valueType", _(std.Cell))),
            var_("result") = ast.new_(tt_("std::List", "valueType", _(std.Pixel))),
            var_("result") = ast.new_(tt_("std::Set", "valueType", _(std.Number))),
            var_("result") = ast.new_(tt_("std::Map", "keyType", _(std.Number), "valueType", _(std.Color))),
            var_("result") = ast.new_(tt_("std::TrieMap", "keyType", _(std.Number), "valueType", _(std.Color))));

    CellI* mapPtr = nullptr;
    mapPtr        = &slots(std.slot("value", ListOf(std.Digit)),
                           std.slot("sign", std.Number)); // TODO sign has no class currently
    std.Number.set("slots", *mapPtr);

    testStruct.addMethod("factorial")
        .parameters(
            param("input", _(std.Number)))
        .returnType(_(std.Number))
        .code(
            ast.if_(ast.greaterThanOrEqual(p_("input"), _(_1_)),
                    ast.return_(ast.multiply(p_("input"), ast.self().call("factorial", param("input", ast.subtract(p_("input"), _(_1_)))))),
                    ast.return_(_(_1_))));

    // TODO
    //    type.String.method(ids.addSlots, { ids.list, list(type.slot(ids.value, type.ListOf(type.Char))) });
    // try/catch: almost the same as break/continue/return it can go through function calls. We need an op::Catch node
    // output: we need some kind of output, maybe a console thing first. Maybe just a new hybrid cell is needed
    // SlotType should hold an std::Type which can be a std::Struct, std::Enum or similar
    // Iterators, range-based-for
    // Variable scopes
    //
}

Brain::Brain() :
    m_initPhase(InitPhase::Init),
    pools(*this),
    ids(*this),
    std(*this),
    ast(*this),
    directions(*this),
    coordinates(*this),
    boolean(*this),
    numbers(*this),
    _0_(pools.numbers.get(0)),
    _1_(pools.numbers.get(1)),
    _2_(pools.numbers.get(2)),
    _3_(pools.numbers.get(3)),
    _4_(pools.numbers.get(4)),
    _5_(pools.numbers.get(5)),
    _6_(pools.numbers.get(6)),
    _7_(pools.numbers.get(7)),
    _8_(pools.numbers.get(8)),
    _9_(pools.numbers.get(9)),
    globalScope(Ast::Scope(*this, "global")),
    earlyStructs(*this, std.Cell, std.Cell, "earlyStructs")
{
    createStd();
    createArcSolver();
    createTests();
    reigisterStructBeforeCompilation(tt_("std::List", "valueType", _(std.Char))); // TODO instantiate on demand in getStruct
    registerBuiltInStruct("std::op::Add", std.op.Add);
    registerBuiltInStruct("std::op::And", std.op.And);
    registerBuiltInStruct("std::op::Base", std.op.Base);
    registerBuiltInStruct("std::op::Block", std.op.Block);
    registerBuiltInStruct("std::op::ConstVar", std.op.ConstVar);
    registerBuiltInStruct("std::op::Delete", std.op.Delete);
    registerBuiltInStruct("std::op::Divide", std.op.Divide);
    registerBuiltInStruct("std::op::Do", std.op.Do);
    registerBuiltInStruct("std::op::Equal", std.op.Equal);
    registerBuiltInStruct("std::op::Erase", std.op.Erase);
    registerBuiltInStruct("std::op::EvalVar", std.op.EvalVar);
    registerBuiltInStruct("std::op::Function", std.op.Function);
    registerBuiltInStruct("std::op::Get", std.op.Get);
    registerBuiltInStruct("std::op::GreaterThan", std.op.GreaterThan);
    registerBuiltInStruct("std::op::GreaterThanOrEqual", std.op.GreaterThanOrEqual);
    registerBuiltInStruct("std::op::Has", std.op.Has);
    registerBuiltInStruct("std::op::If", std.op.If);
    registerBuiltInStruct("std::op::LessThan", std.op.LessThan);
    registerBuiltInStruct("std::op::LessThanOrEqual", std.op.LessThanOrEqual);
    registerBuiltInStruct("std::op::Missing", std.op.Missing);
    registerBuiltInStruct("std::op::Multiply", std.op.Multiply);
    registerBuiltInStruct("std::op::New", std.op.New);
    registerBuiltInStruct("std::op::Not", std.op.Not);
    registerBuiltInStruct("std::op::NotEqual", std.op.NotEqual);
    registerBuiltInStruct("std::op::NotSame", std.op.NotSame);
    registerBuiltInStruct("std::op::Or", std.op.Or);
    registerBuiltInStruct("std::op::Return", std.op.Return);
    registerBuiltInStruct("std::op::Same", std.op.Same);
    registerBuiltInStruct("std::op::Set", std.op.Set);
    registerBuiltInStruct("std::op::Subtract", std.op.Subtract);
    registerBuiltInStruct("std::op::Var", std.op.Var);
    registerBuiltInStruct("std::op::While", std.op.While);
    registerBuiltInStruct("std::ast::Add", std.ast.Add);
    registerBuiltInStruct("std::ast::And", std.ast.And);
    registerBuiltInStruct("std::ast::Base", std.ast.Base);
    registerBuiltInStruct("std::ast::Block", std.ast.Block);
    registerBuiltInStruct("std::ast::Break", std.ast.Break);
    registerBuiltInStruct("std::ast::Call", std.ast.Call);
    registerBuiltInStruct("std::ast::Cell", std.ast.Cell);
    registerBuiltInStruct("std::ast::Continue", std.ast.Continue);
    registerBuiltInStruct("std::ast::Delete", std.ast.Delete);
    registerBuiltInStruct("std::ast::Divide", std.ast.Divide);
    registerBuiltInStruct("std::ast::Do", std.ast.Do);
    registerBuiltInStruct("std::ast::Equal", std.ast.Equal);
    registerBuiltInStruct("std::ast::Erase", std.ast.Erase);
    registerBuiltInStruct("std::ast::Function", std.ast.Function);
    registerBuiltInStruct("std::ast::FunctionT", std.ast.FunctionT);
    registerBuiltInStruct("std::ast::Get", std.ast.Get);
    registerBuiltInStruct("std::ast::GreaterThan", std.ast.GreaterThan);
    registerBuiltInStruct("std::ast::GreaterThanOrEqual", std.ast.GreaterThanOrEqual);
    registerBuiltInStruct("std::ast::Has", std.ast.Has);
    registerBuiltInStruct("std::ast::If", std.ast.If);
    registerBuiltInStruct("std::ast::LessThan", std.ast.LessThan);
    registerBuiltInStruct("std::ast::LessThanOrEqual", std.ast.LessThanOrEqual);
    registerBuiltInStruct("std::ast::Member", std.ast.Member);
    registerBuiltInStruct("std::ast::Missing", std.ast.Missing);
    registerBuiltInStruct("std::ast::Multiply", std.ast.Multiply);
    registerBuiltInStruct("std::ast::New", std.ast.New);
    registerBuiltInStruct("std::ast::Not", std.ast.Not);
    registerBuiltInStruct("std::ast::NotEqual", std.ast.NotEqual);
    registerBuiltInStruct("std::ast::NotSame", std.ast.NotSame);
    registerBuiltInStruct("std::ast::Or", std.ast.Or);
    registerBuiltInStruct("std::ast::Parameter", std.ast.Parameter);
    registerBuiltInStruct("std::ast::ResolvedType", std.ast.ResolvedType);
    registerBuiltInStruct("std::ast::Return", std.ast.Return);
    registerBuiltInStruct("std::ast::Same", std.ast.Same);
    registerBuiltInStruct("std::ast::Scope", std.ast.Scope);
    registerBuiltInStruct("std::ast::Self", std.ast.Self);
    registerBuiltInStruct("std::ast::SelfFn", std.ast.SelfFn);
    registerBuiltInStruct("std::ast::Set", std.ast.Set);
    registerBuiltInStruct("std::ast::Slot", std.ast.Slot);
    registerBuiltInStruct("std::ast::StaticCall", std.ast.StaticCall);
    registerBuiltInStruct("std::ast::Struct", std.ast.Struct);
    registerBuiltInStruct("std::ast::StructName", std.ast.StructName);
    registerBuiltInStruct("std::ast::StructT", std.ast.StructT);
    registerBuiltInStruct("std::ast::Subtract", std.ast.Subtract);
    registerBuiltInStruct("std::ast::SubTypeName", std.ast.SubTypeName);
    registerBuiltInStruct("std::ast::TemplatedType", std.ast.TemplatedType);
    registerBuiltInStruct("std::ast::TemplateParam", std.ast.TemplateParam);
    registerBuiltInStruct("std::ast::Var", std.ast.Var);
    registerBuiltInStruct("std::ast::While", std.ast.While);
    registerBuiltInStruct("std::Cell", std.Cell);
    registerBuiltInStruct("std::Slot", std.Slot);
    registerBuiltInStruct("std::Type", std.Type_);
    registerBuiltInStruct("std::Enum", std.Enum);
    registerBuiltInStruct("std::Container", std.Container);
    registerBuiltInStruct("std::List", std.List);
    registerBuiltInStruct("std::ListItem", std.ListItem);
    registerBuiltInStruct("std::KVPair", std.KVPair);
    registerBuiltInStruct("std::Map", std.Map);
    registerBuiltInStruct("std::Index", std.Index);
    registerBuiltInStruct("std::TrieMap", std.TrieMap);
    registerBuiltInStruct("std::TrieMapNode", std.TrieMapNode);
    registerBuiltInStruct("std::Boolean", std.Boolean);
    registerBuiltInStruct("std::Char", std.Char);
    registerBuiltInStruct("std::Digit", std.Digit);
    registerBuiltInStruct("std::Number", std.Number);
    registerBuiltInStruct("std::String", std.String);
    registerBuiltInStruct("std::Color", std.Color);
    registerBuiltInStruct("std::Pixel", std.Pixel);
    registerBuiltInStruct("std::Picture", std.Picture);
    registerBuiltInStruct("std::Stack", std.Stack);
    registerBuiltInStruct("std::StackFrame", std.StackFrame);
    registerBuiltInStruct("std::Program", std.Program);
    registerBuiltInStruct("std::ProgramData", std.ProgramData);
    registerBuiltInStruct("std::StructReference", std.StructReference);
    registerBuiltInStruct("std::CompileState", std.CompileState);
    registerBuiltInStruct("std::Directions", std.Directions);

    auto& compiledGlobalScope = globalScope.compile(earlyStructs);
    compiledGlobalScopePtr    = &compiledGlobalScope[ids.data];
    m_initPhase               = InitPhase::FullyConstructed;

    // Test should be removed from here
    auto& compiledStructs        = static_cast<TrieMap&>(compiledGlobalScope[ids.data][ids.structs]);
    Visitor::visitList(compiledStructs[ids.list], [this](CellI& kv, int, bool&) {
        std::cout << kv[ids.key].label() << std::endl;
    });
    auto& compiledFunctions = static_cast<TrieMap&>(compiledGlobalScope[ids.data][ids.functions]);
    Visitor::visitList(compiledFunctions[ids.list], [this](CellI& kv, int, bool&) {
        std::cout << kv[ids.key].label() << " : " << kv[ids.value].label() << std::endl;
    });
    auto& compiledVariables = static_cast<TrieMap&>(compiledGlobalScope[ids.data][ids.variables]);
    Visitor::visitList(compiledVariables[ids.list], [this](CellI& kv, int, bool&) {
        std::cout << kv[ids.key].label() << " : " << kv[ids.value].label() << std::endl;
    });
    auto& compiledListItemStruct = static_cast<TrieMap&>(compiledGlobalScope[ids.data][ids.structs]).getValue(templateId("std::ListItem", id("valueType"), std.Cell));
    auto& compiledListStruct     = static_cast<TrieMap&>(compiledGlobalScope[ids.data][ids.structs]).getValue(templateId("std::List", id("valueType"), std.Cell));
    auto& compiledTypeStruct     = static_cast<TrieMap&>(compiledGlobalScope[ids.data][ids.structs]).getValue(id("std::Type"));
    auto& compiledIndexStruct    = static_cast<TrieMap&>(compiledGlobalScope[ids.data][ids.structs]).getValue(id("std::Index"));
    std.ListItem.set("methods", compiledListItemStruct[ids.methods]);
    std.List.set("methods", compiledListStruct[ids.methods]);
    std.Type_.set("methods", compiledTypeStruct[ids.methods]);

    Object testType(*this, compiledTypeStruct, id("constructor"), "testType");
    Object testRecursiveType(*this, compiledTypeStruct, id("constructorWithRecursiveType"), "testRecursiveType");

    Object testIndex(*this, compiledIndexStruct, id("constructor"), "testIndex");
    testIndex.method(id("insert"), { "key", _1_ }, { "value", _2_ });
}

Brain::~Brain()
{
    m_initPhase = InitPhase::DestructBegin;
}

CellI& Brain::getStruct(const std::string& name)
{
    return getStruct(id(name));
}

CellI& Brain::getStruct(CellI& id)
{
    switch (m_initPhase) {
    case InitPhase::Init:
        throw "Get struct before compilation is not possible";
    case InitPhase::Compiling:
        throw "Get struct during compilation is not possible";
    case InitPhase::FullyConstructed:
        return static_cast<TrieMap&>((*compiledGlobalScopePtr)[ids.structs]).getValue(id);
    case InitPhase::DestructBegin:
        return ids.emptyObject;
    }
    throw "Unhandled state!";
}

CellI& Brain::reigisterStructBeforeCompilation(CellI& structAst)
{
    CellI* structIdPtr = nullptr;
    if (&structAst.type() == &std.ast.TemplatedType) {
        List& idCell = *new List(*this, std.Cell);
        structIdPtr = &idCell;
        std::stringstream ss;
        Visitor::visitList(structAst[ids.scopes], [this, &idCell, &ss](CellI& scope, int i, bool&) {
            Visitor::visitList(scope, [this, &idCell, &ss](CellI& character, int, bool&) {
                idCell.add(character);
                ss << character.label();
            });
            idCell.add(pools.chars.get(':'));
            idCell.add(pools.chars.get(':'));
            ss << "::";
        });
        Visitor::visitList(structAst[ids.id], [this, &idCell, &ss](CellI& character, int, bool&) {
            idCell.add(character);
            ss << character.label();
        });

        ss << "<";
        Visitor::visitList(structAst[ids.parameters], [this, &idCell, &ss](CellI& slot, int i, bool&) {
            if (i != 0) {
                ss << ", ";
            }
            CellI& slotRole = slot[ids.slotRole];
            CellI& slotType = slot[ids.slotType];
            CellI& compiledSlotType = reigisterStructBeforeCompilation(slotType);

            idCell.add(slotRole);
            idCell.add(compiledSlotType);
            ss << std::format("{}={}", slotRole.label(), compiledSlotType.label());
        });
        ss << ">";
        idCell.label(ss.str());
    } else if (&structAst.type() == &std.ast.StructName) {
        structIdPtr = &structAst[ids.name];
    } else if (&structAst.type() == &std.ast.Cell) {
        return structAst[ids.value];
    } else {
        throw "Unsupported type!";
    }
    CellI& structId = *structIdPtr;
    if (earlyStructs.hasKey(structId)) {
        return earlyStructs.getValue(structId);
    } else {
        auto& unresolvedStruct = *new Object(*this, std.Type_, std::format("{}", structId.label()));
        unresolvedStruct.set("incomplete", boolean.true_);

        earlyStructs.add(structId, std.slot(structAst, unresolvedStruct));
        return unresolvedStruct;
    }
}

void Brain::registerBuiltInStruct(const std::string& fullName, CellI& compiledStruct)
{
    std::vector<std::string> sliced;
    boost::algorithm::split_regex(sliced, fullName, boost::regex("::"));

    if (sliced.empty()) {
        throw "Invalid struct ID!";
    }
    std::stringstream ss;
    List& idCell             = *new List(*this, std.Cell);
    const auto& structName   = sliced.back();
    Ast::Scope* currentScope = &globalScope;
    if (sliced.size() > 1) {
        for (int i = 0; i < sliced.size() - 1; ++i) {
            const auto& scopeName = sliced[i];
            currentScope          = &currentScope->getScope(scopeName);
            Visitor::visitList((*currentScope)["id"], [this, &idCell, &ss](CellI& character, int, bool&) {
                idCell.add(character);
                ss << character.label();
            });
            idCell.add(pools.chars.get(':'));
            idCell.add(pools.chars.get(':'));
            ss << "::";
        }
    }
    Ast::Struct& structAst = currentScope->getStruct(structName);
    Visitor::visitList(structAst[ids.id], [this, &idCell, &ss](CellI& character, int, bool&) {
        idCell.add(character);
        ss << character.label();
    });
    idCell.label(ss.str());
    earlyStructs.add(idCell, std.slot(struct_(fullName), compiledStruct));
}

CellI& Brain::id(const std::string& str)
{
    return pools.strings.getCharList(str);
}

CellI& Brain::ListOf(CellI& valueType)
{
    switch (m_initPhase) {
    case InitPhase::Init: {
        auto& ret = tt_("std::List", "valueType", _(valueType));
        reigisterStructBeforeCompilation(ret);

        return ret;
    }
    case InitPhase::Compiling:
        throw "Get struct during compilation is not possible";
    case InitPhase::FullyConstructed: {
        return getStruct(templateId("std::List", ids.valueType, valueType));
    }
    case InitPhase::DestructBegin:
        return ids.emptyObject;
    }
    throw "Unhandled state!";
}

CellI& Brain::MapOf(CellI& keyType, CellI& valueType)
{
    switch (m_initPhase) {
    case InitPhase::Init: {
        auto& ret = tt_("std::Map", "keyType", _(keyType), "valueType", _(valueType));
        reigisterStructBeforeCompilation(ret);

        return ret;
    }
    case InitPhase::Compiling:
        throw "Get struct during compilation is not possible";
    case InitPhase::FullyConstructed: {
        return getStruct(templateId("std::Map", ids.keyType, keyType, ids.valueType, valueType));
    }
    case InitPhase::DestructBegin:
        return ids.emptyObject;
    }
    throw "Unhandled state!";
}

CellI& Brain::toKbBool(bool value)
{
    return value ? boolean.true_ : boolean.false_;
}

Brain::InitPhase Brain::initPhase()
{
    return m_initPhase;
}

} // namespace brain
} // namespace cells
} // namespace synth
