#include "Brain.h"
#include "Compiler.h"

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include "util/Log.h"

#include <sstream>

namespace infocell {
namespace cells {

ID::ID(Brain& kb) :
    kb(kb),
    argument(kb, kb.std.Char, "argument"),
    ast(kb, kb.std.Char, "ast"),
    asts(kb, kb.std.Char, "asts"),
    blue(kb, kb.std.Char, "blue"),
    break_(kb, kb.std.Char, "break_"),
    cell(kb, kb.std.Char, "cell"),
    children(kb, kb.std.Char, "children"),
    color(kb, kb.std.Char, "color"),
    compiled(kb, kb.std.Char, "compiled"),
    condition(kb, kb.std.Char, "condition"),
    constructor(kb, kb.std.Char, "constructor"),
    container(kb, kb.std.Char, "container"),
    continue_(kb, kb.std.Char, "continue_"),
    currentFn(kb, kb.std.Char, "currentFn"),
    currentParam(kb, kb.std.Char, "currentParam"),
    currentStruct(kb, kb.std.Char, "currentStruct"),
    data(kb, kb.std.Char, "data"),
    description(kb, kb.std.Char, "description"),
    destructor(kb, kb.std.Char, "destructor"),
    else_(kb, kb.std.Char, "else_"),
    enum_(kb, kb.std.Char, "enum_"),
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
    instructions(kb, kb.std.Char, "instructions"),
    item(kb, kb.std.Char, "item"),
    itemType(kb, kb.std.Char, "itemType"),
    key(kb, kb.std.Char, "key"),
    keyType(kb, kb.std.Char, "keyType"),
    last(kb, kb.std.Char, "last"),
    lastOp(kb, kb.std.Char, "lastOp"),
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
    pixelsMap(kb, kb.std.Char, "pixelsMap"),
    pop(kb, kb.std.Char, "pop"),
    previous(kb, kb.std.Char, "previous"),
    process(kb, kb.std.Char, "process"),
    push(kb, kb.std.Char, "push"),
    red(kb, kb.std.Char, "red"),
    resolvedScope(kb, kb.std.Char, "resolvedScope"),
    result(kb, kb.std.Char, "result"),
    return_(kb, kb.std.Char, "return_"),
    returnType(kb, kb.std.Char, "returnType"),
    rhs(kb, kb.std.Char, "rhs"),
    rootNode(kb, kb.std.Char, "rootNode"),
    scope(kb, kb.std.Char, "scope"),
    scopes(kb, kb.std.Char, "scopes"),
    self(kb, kb.std.Char, "self"),
    size(kb, kb.std.Char, "size"),
    slots(kb, kb.std.Char, "slots"),
    stack(kb, kb.std.Char, "stack"),
    state(kb, kb.std.Char, "state"),
    stateCondition(kb, kb.std.Char, "stateCondition"),
    stateElse(kb, kb.std.Char, "stateElse"),
    stateLhs(kb, kb.std.Char, "stateLhs"),
    statement(kb, kb.std.Char, "statement"),
    stateParam1(kb, kb.std.Char, "stateParam1"),
    stateParam2(kb, kb.std.Char, "stateParam2"),
    stateParam3(kb, kb.std.Char, "stateParam3"),
    stateParamEval(kb, kb.std.Char, "stateParamEval"),
    stateParamInit(kb, kb.std.Char, "stateParamInit"),
    stateRhs(kb, kb.std.Char, "stateRhs"),
    stateStackCall(kb, kb.std.Char, "stateStackCall"),
    stateStatement(kb, kb.std.Char, "stateStatement"),
    stateThen(kb, kb.std.Char, "stateThen"),
    static_(kb, kb.std.Char, "static_"),
    status(kb, kb.std.Char, "status"),
    struct_(kb, kb.std.Char, "struct"),
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
    variable(kb, kb.std.Char, "variable"),
    variables(kb, kb.std.Char, "variables"),
    width(kb, kb.std.Char, "width")
{
}

namespace type {

Op::Op(Brain& kb) :
    kb(kb),
    Activate(kb, kb.std.Struct, "op::Activate"),
    Add(kb, kb.std.Struct, "op::Add"),
    And(kb, kb.std.Struct, "op::And"),
    Base(kb, kb.std.Struct, "op::Base"),
    Block(kb, kb.std.Struct, "op::Block"),
    Call(kb, kb.std.Struct, "op::Call"),
    ConstVar(kb, kb.std.Struct, "op::ConstVar"),
    Delete(kb, kb.std.Struct, "op::Delete"),
    Divide(kb, kb.std.Struct, "op::Divide"),
    Do(kb, kb.std.Struct, "op::Do"),
    Equal(kb, kb.std.Struct, "op::Equal"),
    Erase(kb, kb.std.Struct, "op::Erase"),
    Function(kb, kb.std.Struct, "op::Function"),
    Get(kb, kb.std.Struct, "op::Get"),
    GreaterThan(kb, kb.std.Struct, "op::GreaterThan"),
    GreaterThanOrEqual(kb, kb.std.Struct, "op::GreaterThanOrEqual"),
    Has(kb, kb.std.Struct, "op::Has"),
    If(kb, kb.std.Struct, "op::If"),
    LessThan(kb, kb.std.Struct, "op::LessThan"),
    LessThanOrEqual(kb, kb.std.Struct, "op::LessThanOrEqual"),
    Missing(kb, kb.std.Struct, "op::Missing"),
    Multiply(kb, kb.std.Struct, "op::Multiply"),
    New(kb, kb.std.Struct, "op::New"),
    Not(kb, kb.std.Struct, "op::Not"),
    NotEqual(kb, kb.std.Struct, "op::NotEqual"),
    NotSame(kb, kb.std.Struct, "op::NotSame"),
    Or(kb, kb.std.Struct, "op::Or"),
    Return(kb, kb.std.Struct, "op::Return"),
    Same(kb, kb.std.Struct, "op::Same"),
    Set(kb, kb.std.Struct, "op::Set"),
    Subtract(kb, kb.std.Struct, "op::Subtract"),
    Var(kb, kb.std.Struct, "op::Var"),
    While(kb, kb.std.Struct, "op::While")
{
}

Ast::Ast(Brain& kb) :
    kb(kb),
    Add(kb, kb.std.Struct, "ast::Add"),
    And(kb, kb.std.Struct, "ast::And"),
    Base(kb, kb.std.Struct, "ast::Base"),
    Block(kb, kb.std.Struct, "ast::Block"),
    Break(kb, kb.std.Struct, "ast::Break"),
    Call(kb, kb.std.Struct, "ast::Call"),
    Cell(kb, kb.std.Struct, "ast::Cell"),
    Continue(kb, kb.std.Struct, "ast::Continue"),
    Delete(kb, kb.std.Struct, "ast::Delete"),
    Divide(kb, kb.std.Struct, "ast::Divide"),
    Do(kb, kb.std.Struct, "ast::Do"),
    Enum(kb, kb.std.Struct, "ast::Enum"),
    EnumValue(kb, kb.std.Struct, "ast::EnumValue"),
    Equal(kb, kb.std.Struct, "ast::Equal"),
    Erase(kb, kb.std.Struct, "ast::Erase"),
    For(kb, kb.std.Struct, "ast::For"),
    Function(kb, kb.std.Struct, "ast::Function"),
    FunctionT(kb, kb.std.Struct, "ast::FunctionT"),
    Get(kb, kb.std.Struct, "ast::Get"),
    GreaterThan(kb, kb.std.Struct, "ast::GreaterThan"),
    GreaterThanOrEqual(kb, kb.std.Struct, "ast::GreaterThanOrEqual"),
    Has(kb, kb.std.Struct, "ast::Has"),
    If(kb, kb.std.Struct, "ast::If"),
    LessThan(kb, kb.std.Struct, "ast::LessThan"),
    LessThanOrEqual(kb, kb.std.Struct, "ast::LessThanOrEqual"),
    Match(kb, kb.std.Struct, "ast::Match"),
    Member(kb, kb.std.Struct, "ast::Member"),
    Missing(kb, kb.std.Struct, "ast::Missing"),
    Multiply(kb, kb.std.Struct, "ast::Multiply"),
    New(kb, kb.std.Struct, "ast::New"),
    Not(kb, kb.std.Struct, "ast::Not"),
    NotEqual(kb, kb.std.Struct, "ast::NotEqual"),
    NotSame(kb, kb.std.Struct, "ast::NotSame"),
    Or(kb, kb.std.Struct, "ast::Or"),
    Parameter(kb, kb.std.Struct, "ast::Parameter"),
    ResolvedType(kb, kb.std.Struct, "ast::ResolvedType"),
    Return(kb, kb.std.Struct, "ast::Return"),
    Same(kb, kb.std.Struct, "ast::Same"),
    Scope(kb, kb.std.Struct, "ast::Scope"),
    Self(kb, kb.std.Struct, "ast::Self"),
    SelfFn(kb, kb.std.Struct, "ast::SelfFn"),
    Set(kb, kb.std.Struct, "ast::Set"),
    Slot(kb, kb.std.Struct, "ast::Slot"),
    StaticCall(kb, kb.std.Struct, "ast::StaticCall"),
    Struct(kb, kb.std.Struct, "ast::Struct"),
    StructName(kb, kb.std.Struct, "ast::StructName"),
    StructT(kb, kb.std.Struct, "ast::StructT"),
    SubTypeName(kb, kb.std.Struct, "ast::SubTypeName"),
    Subtract(kb, kb.std.Struct, "ast::Subtract"),
    TemplatedType(kb, kb.std.Struct, "ast::TemplatedType"),
    TemplateParam(kb, kb.std.Struct, "ast::TemplateParam"),
    Throw(kb, kb.std.Struct, "ast::Throw"),
    Trait(kb, kb.std.Struct, "ast::Trait"),
    TraitImpl(kb, kb.std.Struct, "ast::TraitImpl"),
    Try(kb, kb.std.Struct, "ast::Try"),
    TypedEnumValue(kb, kb.std.Struct, "ast::TypedEnumValue"),
    Var(kb, kb.std.Struct, "ast::Var"),
    While(kb, kb.std.Struct, "ast::While")
{
}

} // namespace type

Std::Std(Brain& kb) :
    kb(kb),
    Cell(kb, kb.std.Struct, "Cell"),
    Slot(kb, kb.std.Struct, "Slot"),
    Struct(kb, kb.std.Struct, "Struct"),
    Enum(kb, kb.std.Struct, "Enum"),
    OpState(kb, kb.std.Struct, "OpState"),
    Container(kb, kb.std.Struct, "Conatainer"),
    List(kb, kb.std.Struct, "List"),
    ListItem(kb, kb.std.Struct, "ListItem"),
    KVPair(kb, kb.std.Struct, "KVPair"),
    Map(kb, kb.std.Struct, "Map"),
    Index(kb, kb.std.Struct, "Index"),
    TrieMap(kb, kb.std.Struct, "TrieMap"),
    TrieMapNode(kb, kb.std.Struct, "TrieMapNode"),
    Boolean(kb, kb.std.Struct, "Boolean"),
    Char(kb, kb.std.Struct, "Char"),
    Digit(kb, kb.std.Struct, "Digit"),
    Number(kb, kb.std.Struct, "Number"),
    String(kb, kb.std.Struct, "String"),
    Color(kb, kb.std.Struct, "Color"),
    Pixel(kb, kb.std.Struct, "Pixel"),
    Grid(kb, kb.std.Struct, "Grid"),
    Stack(kb, kb.std.Struct, "Stack"),
    StackFrame(kb, kb.std.Struct, "StackFrame"),
    Program(kb, kb.std.Struct, "Program"),
    ProgramData(kb, kb.std.Struct, "ProgramData"),
    StructReference(kb, kb.std.Struct, "StructReference"),
    CompileState(kb, kb.std.Struct, "CompileState"),
    Directions(kb, kb.std.Enum, "Directions"),
    op(kb),
    ast(kb)
{
}

cells::CellI& Std::slot(cells::CellI& key, cells::CellI& type)
{
    CellI& ret = *new Object(kb, kb.std.Slot);
    ret.set(kb.ids.key, key);
    ret.set(kb.ids.type, type);

    return ret;
}

cells::CellI& Std::slot(const std::string& key, cells::CellI& type)
{
    CellI& ret = *new Object(kb, kb.std.Slot);
    ret.set(kb.ids.key, kb.name(key));
    ret.set(kb.ids.type, type);

    return ret;
}

cells::CellI& Std::kvPair(cells::CellI& key, cells::CellI& value)
{
    CellI& ret = *new Object(kb, kb.std.KVPair);
    ret.set(kb.ids.key, key);
    ret.set(kb.ids.value, value);

    return ret;
}

Directions::Directions(Brain& kb) :
    up(kb, kb.std.Char, "up"),
    down(kb, kb.std.Char, "down"),
    left(kb, kb.std.Char, "left"),
    right(kb, kb.std.Char, "right")
{
}

Coordinates::Coordinates(Brain& kb) :
    x(kb, kb.std.Char, "x"),
    y(kb, kb.std.Char, "y")
{
}

Boolean::Boolean(Brain& kb) :
    true_(kb, kb.std.Boolean, "true"),
    false_(kb, kb.std.Boolean, "false")
{
}

Numbers::Numbers(Brain& kb) :
    sign(kb, kb.std.Cell, kb.std.Cell, "sign"),
    positive(kb, kb.std.Cell, "positive"),
    negative(kb, kb.std.Cell, "negative")

{
    sign.add(positive, positive);
    sign.add(negative, negative);
}

// ============================================================================
Pools::Chars::Chars(Brain& kb) :
    kb(kb)
{
    // These are enough for me currently
    registerUnicodeBlock(0x020, 0x07e); // Basic Latin - without the DEL (0x7f) control character
    registerUnicodeBlock(0x080, 0x0ff); // Latin-1 Supplement
    registerUnicodeBlock(0x100, 0x17f); // Latin Extended-A
}

Object& Pools::Chars::get(char32_t utf32Char)
{
    auto unicodeCellIt = m_characters.find(utf32Char);
    if (unicodeCellIt != m_characters.end()) {
        return unicodeCellIt->second;
    } else {
        throw "Unknown char!";
    }
}

void Pools::Chars::registerUnicodeBlock(char32_t from, char32_t to)
{
    for (char32_t unicodeValue = from; unicodeValue <= to; ++unicodeValue) {
        std::string characterName;
        if (unicodeValue == 32) {
            characterName = "(space)";
        } else if (unicodeValue > 32 && unicodeValue < 127) {
            characterName = " ";
            characterName[0] = unicodeValue;
        } else {
            characterName = fmt::format("Unicode_{:#04x}", (int)unicodeValue);
        }
        auto it = m_characters.emplace(std::piecewise_construct,
                             std::forward_as_tuple(unicodeValue),
                             std::forward_as_tuple(kb, kb.std.Char, characterName));
        auto& value = it.first->second;
    }
}

// ============================================================================
Pools::Digits::Digits(Brain& kb)
{
    m_digits.reserve(10);
    for (int i = 0; i < 10; ++i) {
        std::string digitName = "Digit_" + std::to_string(i);
        m_digits.emplace_back(kb, kb.std.Digit, digitName);
    }
}

Object& Pools::Digits::operator[](int digit)
{
    return m_digits[digit];
}

// ============================================================================
Pools::Numbers::Numbers(Brain& kb) :
    m_kb(kb)
{
}

Number& Pools::Numbers::get(int number)
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
Pools::Strings::Strings(Brain& kb) :
    kb(kb)
{
    StringInit reservedStrings[] = {
        { "argument", kb.ids.argument },
        { "ast", kb.ids.ast },
        { "asts", kb.ids.asts },
        { "blue", kb.ids.blue },
        { "cell", kb.ids.cell },
        { "children", kb.ids.children },
        { "color", kb.ids.color },
        { "compiled", kb.ids.compiled },
        { "condition", kb.ids.condition },
        { "constructor", kb.ids.constructor },
        { "container", kb.ids.container },
        { "continue_", kb.ids.continue_ },
        { "currentFn", kb.ids.currentFn },
        { "currentParam", kb.ids.currentParam },
        { "currentStruct", kb.ids.currentStruct },
        { "data", kb.ids.data },
        { "description", kb.ids.description },
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
        { "instructions", kb.ids.instructions },
        { "item", kb.ids.item },
        { "itemType", kb.ids.itemType },
        { "key", kb.ids.key },
        { "keyType", kb.ids.keyType },
        { "last", kb.ids.last },
        { "lastOp", kb.ids.lastOp },
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
        { "pixelsMap", kb.ids.pixelsMap },
        { "previous", kb.ids.previous },
        { "red", kb.ids.red },
        { "resolvedScope", kb.ids.resolvedScope },
        { "result", kb.ids.result },
        { "returnType", kb.ids.returnType },
        { "rhs", kb.ids.rhs },
        { "key", kb.ids.key },
        { "rootNode", kb.ids.rootNode },
        { "scope", kb.ids.scope },
        { "scopes", kb.ids.scopes },
        { "self", kb.ids.self },
        { "size", kb.ids.size },
        { "key", kb.ids.key },
        { "slots", kb.ids.slots },
        { "stack", kb.ids.stack },
        { "state", kb.ids.state },
        { "stateCondition", kb.ids.stateCondition },
        { "stateElse", kb.ids.stateElse },
        { "stateLhs", kb.ids.stateLhs },
        { "statement", kb.ids.statement },
        { "stateParam1", kb.ids.stateParam1 },
        { "stateParam2", kb.ids.stateParam2 },
        { "stateParam3", kb.ids.stateParam3 },
        { "stateParamEval", kb.ids.stateParamEval },
        { "stateParamInit", kb.ids.stateParamInit },
        { "stateRhs", kb.ids.stateRhs },
        { "stateStackCall", kb.ids.stateStackCall },
        { "stateStatement", kb.ids.stateStatement },
        { "stateThen", kb.ids.stateThen },
        { "static_", kb.ids.static_ },
        { "status", kb.ids.status },
        { "struct", kb.ids.struct_ },
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
        { "variable", kb.ids.variable },
        { "variables", kb.ids.variables },
        { "width", kb.ids.width },
        { "up", kb.directions.up },
        { "down", kb.directions.down },
        { "left", kb.directions.left },
        { "right", kb.directions.right },
        { "x", kb.coordinates.x },
        { "y", kb.coordinates.y }
    };
    for (auto& reservedString : reservedStrings) {
        auto& str = reservedString.str;
        m_strings.emplace(std::piecewise_construct,
                          std::forward_as_tuple(reservedString.str),
                          std::forward_as_tuple(kb, reservedString.list, reservedString.str));

    }
}

String& Pools::Strings::get(const std::string& str)
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

List& Pools::Strings::getCharList(const std::string& str)
{
    auto& string = get(str);
    auto& ret = static_cast<List&>(string[string.kb.ids.value]);

    return ret;
}

Pools::Pools(Brain& kb) :
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

Ast::Cell& Brain::_(const std::string& nameStr)
{
    return ast.cell(name(nameStr));
}

Ast::StructName& Brain::struct_(const std::string& nameStr)
{
    return ast.structName(nameStr);
}


class AstStd : public AstHelper
{
public:
    AstStd(Brain& kb);

private:
    void createOp();
    void createAst();

    Scope& stdScope;
};

void AstStd::createOp()
{
    auto& opScope = stdScope.add<Scope>("op");
    opScope.add<Struct>("Base");
    opScope.add<Struct>("Activate")
        .members(
            member("ast", "ast::Base"),
            member("cell", "Base"),
            member("previous", "std::Cell"),
            member("state", "std::Cell"));

    opScope.add<Struct>("Add")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "std::Number"));

    opScope.add<Struct>("And")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "std::Boolean"));

    opScope.add<Struct>("Block")
        .members(
            member("ast", "ast::Base"),
            member("previous", "std::Cell"),
            member("state", "std::Cell"),
            member("status", "std::Cell"),
            member("ops", "std::Cell"),
            member("value", "std::Cell"));

    opScope.add<Struct>("Call")
        .members(
            member("ast", "ast::Base"),
            member("cell", "ast::Base"),
            member("method", "ast::Base"),
            member("currentParam", "std::Cell"),
            member("parameters", tt_("std::List", "valueType", "std::Slot")),
            member("stack", "ast::Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "std::Cell"));

    opScope.add<Struct>("ConstVar")
        .members(
            member("ast", "ast::Base"),
            member("value", "std::Cell"));

    opScope.add<Struct>("Delete")
        .members(
            member("ast", "ast::Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("input", "Base"));

    opScope.add<Struct>("Divide")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "std::Number"));

    opScope.add<Struct>("Do")
        .members(
            member("ast", "ast::Base"),
            member("status", "std::Cell"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("condition", "Base"),
            member("statement", "Base"));

    opScope.add<Struct>("Equal")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "std::Boolean"));

    opScope.add<Struct>("Erase")
        .members(
            member("ast", "ast::Base"),
            member("cell", "Base"),
            member("key", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "Base"));

    opScope.add<Struct>("Function")
        .members(
            member("ast", "ast::Base"),
            member("stack", "Stack"),
            member("op", tt_("std::List", "valueType", "Base")),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("static_", "std::Boolean"));

    opScope.add<Struct>("Get")
        .members(
            member("ast", "ast::Base"),
            member("cell", "Base"),
            member("key", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "std::Cell"));

    opScope.add<Struct>("GreaterThan")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "std::Boolean"));

    opScope.add<Struct>("GreaterThanOrEqual")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "std::Boolean"));

    opScope.add<Struct>("Has")
        .members(
            member("ast", "ast::Base"),
            member("cell", "Base"),
            member("key", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "std::Boolean"));

    opScope.add<Struct>("If")
        .members(
            member("ast", "ast::Base"),
            member("status", "std::Cell"),
            member("condition", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("then", "Base"),
            member("else_", "Base"));

    opScope.add<Struct>("LessThan")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "std::Boolean"));

    opScope.add<Struct>("LessThanOrEqual")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "std::Boolean"));

    opScope.add<Struct>("Missing")
        .members(
            member("ast", "ast::Base"),
            member("cell", "Base"),
            member("key", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "std::Boolean"));

    opScope.add<Struct>("Multiply")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "std::Number"));

    opScope.add<Struct>("New")
        .members(
            member("ast", "ast::Base"),
            member("value", "std::Cell"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("objectType", "Base"));

    opScope.add<Struct>("Not")
        .members(
            member("ast", "ast::Base"),
            member("input", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "std::Boolean"));

    opScope.add<Struct>("NotEqual")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "std::Boolean"));

    opScope.add<Struct>("NotSame")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "std::Boolean"));

    opScope.add<Struct>("Or")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "std::Boolean"));

    opScope.add<Struct>("Return")
        .members(
            member("ast", "ast::Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("result", "ast::Base"));

    opScope.add<Struct>("Same")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "std::Boolean"));

    opScope.add<Struct>("Set")
        .members(
            member("ast", "ast::Base"),
            member("cell", "Base"),
            member("key", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "Base"));

    opScope.add<Struct>("Subtract")
        .members(
            member("ast", "ast::Base"),
            member("lhs", "Base"),
            member("rhs", "Base"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("value", "std::Number"));

    opScope.add<Struct>("Var")
        .description(
            var_("x")("isA")("struct", m_("valueType")),
            self()("hasA")("member", member("ast", "ast::Base")),
            self()("stores")("place", m_("value"))("value", var_("x")))
        .members(
            member("ast", "ast::Base"),
            member("valueType", "std::Struct"),
            member("value", "std::Cell"));

    opScope.add<Struct>("While")
        .members(
            member("ast", "ast::Base"),
            member("status", "std::Cell"),
            member("state", "std::Cell"),
            member("previous", "std::Cell"),
            member("condition", "Base"),
            member("statement", "Base"));
}

void AstStd::createAst()
{
    auto& astScope = stdScope.add<Scope>("ast");
    astScope.add<Struct>("Base");
    astScope.add<Struct>("Add")
        .memberOf(
            _(std.ast.Base))
        .primitiveTool()
        .returnType(_(std.Number))
        .description(
            subtract(return_(), m_("rhs")) == m_("lhs"),
            subtract(return_(), m_("lhs")) == m_("rhs"),
            return_(add(m_("lhs"), m_("rhs"))),
            return_(add(m_("rhs"), m_("lhs"))))
        .members(
            member("lhs", _(std.Number)),
            member("rhs", _(std.Number)));

    astScope.add<Struct>("And")
        .memberOf(
            _(std.ast.Base))
        .primitiveTool()
        .returnType(_(std.Boolean))
        .description(
            return_(and_(m_("lhs"), m_("rhs"))))
        .members(
            member("lhs", _(std.Boolean)),
            member("rhs", _(std.Boolean)));

    astScope.add<Struct>("Block")
        .members(
            member("asts", "std::Cell"));

    astScope.add<Struct>("Break");

    astScope.add<Struct>("Call")
        .members(
            member("cell", "Base"),
            member("method", "Base"),
            member("parameters", ListOf(std.Slot)));

    astScope.add<Struct>("Cell")
        .members(
            member("value", "std::Cell"));

    astScope.add<Struct>("Continue");

    astScope.add<Struct>("Delete")
        .members(
            member("cell", "Base"));

    astScope.add<Struct>("Divide")
        .memberOf(
            _(std.ast.Base))
        .primitiveTool()
        .returnType(_(std.Number))
        .description(
            // TODO check rhs != 0
            multiply(return_(), m_("rhs")) == m_("lhs"),
            return_(divide(m_("lhs"), m_("rhs"))))
        .members(
            member("lhs", _(std.Number)),
            member("rhs", _(std.Number)));

    astScope.add<Struct>("Do")
        .members(
            member("condition", _(std.Boolean)),
            member("statement", "Base"));

    astScope.add<Struct>("Enum")
        .members(
            member("name", "std::Cell"),
            member("fullyQualifiedName", "std::Cell"),
            member("scope", "Scope"),
            member("values", "TrieMap"));

    astScope.add<Struct>("EnumValue")
        .members(
            member("name", "std::Cell"),
            member("enum", "Enum"),
            member("value", "std::Cell"));

    astScope.add<Struct>("Equal")
        .memberOf(
            _(std.ast.Base))
        .primitiveTool()
        .returnType(_(std.Boolean))
        .description(
            return_(m_("lhs") == m_("rhs")))
        .members(
            member("lhs", "Base"),
            member("rhs", "Base"));

    astScope.add<Struct>("Erase")
        .description(
            has(m_("cell"), m_("key")) == false_())
        .members(
            member("cell", "Base"),
            member("key", "Base"));

    astScope.add<Struct>("For")
        .members(
            member("variable", "std::Cell"),
            member("container", "Base"),
            member("statement", "Base"));

    astScope.add<Struct>("Function")
        .members(
            member("name", "std::Cell"),
            member("fullyQualifiedName", "std::Cell"),
            member("structType", "std::Cell"),
            member("parameters", ListOf(std.Slot)),
            member("returnType", "std::Struct"),
            member("instructions", "Base"),
            member("scope", "Base"),
            member("static_", "std::Boolean"));

    astScope.add<Struct>("FunctionT")
        .members(
            member("name", "std::Cell"),
            member("parameters", ListOf(std.Slot)),
            member("returnType", "std::Struct"),
            member("instructions", "Base"),
            member("scope", "Base"),
            member("static_", "std::Boolean"));

    astScope.add<Struct>("Get")
        .memberOf(
            _(std.ast.Base))
        .primitiveTool()
        .returnType(_(std.ast.Base))
        .description(
            return_(m_("cell") / m_("key")))
        .members(
            member("cell", "Base"),
            member("key", "Base"));

    astScope.add<Struct>("GreaterThan")
        .memberOf(
            _(std.ast.Base))
        .primitiveTool()
        .returnType(_(std.Boolean))
        .description(
            lessThan(subtract(m_("rhs"), m_("lhs")), _(kb._0_)),
            return_(greaterThan(m_("lhs"), m_("rhs"))))
        .members(
            member("lhs", _(std.Number)),
            member("rhs", _(std.Number)));

    astScope.add<Struct>("GreaterThanOrEqual")
        .memberOf(
            _(std.ast.Base))
        .primitiveTool()
        .returnType(_(std.Boolean))
        .description(
            lessThanOrEqual(subtract(m_("rhs"), m_("lhs")), _(kb._0_)),
            return_(greaterThanOrEqual(m_("lhs"), m_("rhs"))))
        .members(
            member("lhs", _(std.Number)),
            member("rhs", _(std.Number)));

    astScope.add<Struct>("Has")
        .memberOf(
            _(std.ast.Base))
        .primitiveTool()
        .returnType(_(std.Boolean))
        .description(
            return_(has(m_("cell"), m_("key"))))
        .members(
            member("cell", "Base"),
            member("key", "Base"));

    astScope.add<Struct>("If")
#if 0 // for the CellTrie we have to reference _every_ member but the else_ here is optional so this is a TODO
        .description(
//            equal(_("ActivationPointer") / _("currentCell"), _("condition")), // do we need this?!
            if_(m_("condition")).then_(m_("then")) // is this enough?!
            )
#endif
        .members(
            member("condition", "Base"),
            member("then", "Base"),
            member("else_", "Base"));

    astScope.add<Struct>("LessThan")
        .memberOf(
            _(std.ast.Base))
        .primitiveTool()
        .returnType(_(std.Boolean))
        .description(
            greaterThan(subtract(m_("rhs"), m_("lhs")), _(kb._0_)),
            return_(lessThan(m_("lhs"), m_("rhs"))))
        .members(
            member("lhs", _(std.Number)),
            member("rhs", _(std.Number)));

    astScope.add<Struct>("LessThanOrEqual")
        .memberOf(
            _(std.ast.Base))
        .primitiveTool()
        .returnType(_(std.Boolean))
        .description(
            greaterThanOrEqual(subtract(m_("rhs"), m_("lhs")), _(kb._0_)),
            return_(lessThanOrEqual(m_("lhs"), m_("rhs"))))
        .members(
            member("lhs", _(std.Number)),
            member("rhs", _(std.Number)));

    astScope.add<Struct>("Member")
        .members(
            member("key", "Base"));

    astScope.add<Struct>("Missing")
        .memberOf(
            _(std.ast.Base))
        .primitiveTool()
        .returnType(_(std.Boolean))
        .description(
            return_(missing(m_("cell"), m_("key"))))
        .members(
            member("cell", "Base"),
            member("key", "Base"));

    astScope.add<Struct>("Multiply")
        .memberOf(
            _(std.ast.Base))
        .primitiveTool()
        .returnType(_(std.Number))
        .description(
#if 0 // we need a precondition secton for this if block
            if_(notSame(m_("lhs"), _(kb._0_))).then_(
#endif
            divide(return_(), m_("lhs")) == m_("rhs")
#if 0 // we need a precondition secton for this if block
            )
#else
            ,
#endif
            return_(multiply(m_("lhs"), m_("rhs"))))
        .members(
            member("lhs", _(std.Number)),
            member("rhs", _(std.Number)));

    astScope.add<Struct>("New")
        .members(
            member("objectType", "Base"),
            member("constructor", "Base"),
            member("parameters", ListOf(std.ast.Slot)));

    astScope.add<Struct>("Not")
        .memberOf(
            _(std.ast.Base))
        .primitiveTool()
        .returnType(_(std.Boolean))
        .description(
            return_(not_(m_("input"))))
        .members(
            member("input", _(std.Boolean)));

    astScope.add<Struct>("NotEqual")
        .memberOf(
            _(std.ast.Base))
        .primitiveTool()
        .returnType(_(std.Boolean))
        .description(
            return_(notEqual(m_("lhs"), m_("rhs"))))
        .members(
            member("lhs", "Base"),
            member("rhs", "Base"));

    astScope.add<Struct>("NotSame")
        .memberOf(
            _(std.ast.Base))
        .primitiveTool()
        .returnType(_(std.Boolean))
        .description(
            return_(notSame(m_("lhs"), m_("rhs"))))
        .members(
            member("lhs", "Base"),
            member("rhs", "Base"));

    astScope.add<Struct>("Or")
        .memberOf(
            _(std.ast.Base))
        .primitiveTool()
        .returnType(_(std.Boolean))
        .description(
            return_(or_(m_("lhs"), m_("rhs"))))
        .members(
            member("lhs", _(std.Boolean)),
            member("rhs", _(std.Boolean)));

    astScope.add<Struct>("Parameter")
        .members(
            member("key", "std::Cell"));

    astScope.add<Struct>("ResolvedType")
        .members(
            member("ast", "std::Struct"),
            member("compiled", "std::Struct"));

    astScope.add<Struct>("Return")
        .memberOf(
            _(std.ast.Base))
        .members(
            member("value", "std::Cell"));

    astScope.add<Struct>("Same")
        .memberOf(
            _(std.ast.Base))
        .primitiveTool()
        .returnType(_(std.Boolean))
        .description(
            return_(same(m_("lhs"), m_("rhs"))))
        .members(
            member("lhs", "Base"),
            member("rhs", "Base"));

    astScope.add<Struct>("Scope")
        .members(
            member("name", "std::Cell"),
            member("fullyQualifiedName", "std::Cell"),
            member("scopes", "std::TrieMap"),
            member("resolvedScope", "Scope"),
            member("scope", "Scope"),
            member("functions", "std::TrieMap"),
            member("structs", "std::TrieMap"),
            member("structTs", "std::TrieMap"),
            member("enums", "std::TrieMap"),
            member("variables", ListOf(std.ast.Slot)));

    astScope.add<Struct>("Self");

    astScope.add<Struct>("SelfFn");

    astScope.add<Struct>("Set")
        .memberOf(
            _(std.ast.Base))
        .primitiveTool()
        .description(
            m_("cell") / m_("key") == m_("value"))
        .members(
            member("cell", "Base"),
            member("key", "Base"),
            member("value", "Base"));

    astScope.add<Struct>("Slot")
        .members(
            member("key", "Base"),
            member("type", "Base"));

    astScope.add<Struct>("StaticCall")
        .members(
            member("cell", "Base"),
            member("method", "Base"),
            member("parameters", ListOf(std.Slot)));

    astScope.add<Struct>("Struct")
        .members(
            member("name", "std::Cell"),
            member("compiledStruct", "std::op::Base"),
            member("fullyQualifiedName", "std::Cell"),
            member("incomplete", "std::Boolean"),
            member("instanceOf", "Base"),
            member("templateParams", "std::List"),
            member("scope", "Scope"),
            member("primitiveTool", _(std.Boolean)),
            member("returnType", "Base"),
            member("methods", MapOf(std.Cell, std.ast.Function)),
            member("members", MapOf(std.Cell, std.ast.Slot)),
            member("subTypes", ListOf(std.ast.Slot)),
            member("memberOf", ListOf(std.Struct)));

    astScope.add<Struct>("StructName")
        .members(
            member("name", "std::Cell"),
            member("scopes", "std::List"));

    astScope.add<Struct>("StructT")
        .members(
            member("name", "std::Cell"),
            member("scope", "Base"),
            member("methods", MapOf(std.Cell, std.ast.Function)),
            member("members", ListOf(std.ast.Slot)),
            member("subTypes", ListOf(std.ast.Slot)),
            member("memberOf", ListOf(std.Struct)),
            member("templateParams", MapOf(std.Cell, std.Struct)));

    astScope.add<Struct>("SubTypeName")
        .members(
            member("name", "std::Cell"));

    astScope.add<Struct>("Subtract")
        .memberOf(
            _(std.ast.Base))
        .primitiveTool()
        .returnType(_(std.Number))
        .description(
            add(return_(), m_("rhs")) == m_("lhs"),
            add(m_("rhs"), return_()) == m_("lhs"),
//            equal(m_("lhs"), add(return_(), m_("rhs"))),
            return_(subtract(m_("lhs"), m_("rhs"))))
        .members(
            member("lhs", _(std.Number)),
            member("rhs", _(std.Number)));

    astScope.add<Struct>("TemplatedType")
        .members(
            member("id", "Base"),
            member("scopes", "std::List"),
            member("parameters", ListOf(std.Slot)));

    astScope.add<Struct>("TemplateParam")
        .members(
            member("key", "std::Cell"));

    astScope.add<Struct>("Throw")
        .members(
            member("value", "Base"));

    astScope.add<Struct>("Trait")
        .members(
            member("name", "std::Cell"),
            member("scope", "Base"),
            member("methods", MapOf(std.Cell, std.ast.Function)),
            member("associatedTypes", ListOf(std.ast.Slot)),
            member("subTypes", ListOf(std.ast.Slot)),
            member("templateParams", MapOf(std.Cell, std.Struct)));

    astScope.add<Struct>("TraitImpl")
        .members(
            member("name", "std::Cell"),
            member("scope", "Base"),
            member("methods", MapOf(std.Cell, std.ast.Function)),
            member("associatedTypes", ListOf(std.ast.Slot)),
            member("subTypes", ListOf(std.ast.Slot)),
            member("templateParams", MapOf(std.Cell, std.Struct)));

    astScope.add<Struct>("Try")
        .members(
            member("tryBranch", "Base"),
            member("catchBranch", "Base"));

    astScope.add<Struct>("TypedEnumValue")
        .members(
            member("name", "std::Cell"),
            member("enum", "Enum"),
            member("enumType", "Struct"),
            member("value", "std::Cell"));

    astScope.add<Struct>("Var")
        .members(
            member("name", "std::Cell"),
            member("scope", "Scope"));

    astScope.add<Struct>("While")
        .members(
            member("condition", _(std.Boolean)),
            member("statement", "Base"));
}

AstStd::AstStd(Brain& kb) :
    AstHelper(kb),
    stdScope(kb.globalScope.add<Scope>("std"))
{
    createOp();
    createAst();

        /*
     * enum type
     *   tag: roleId
     *   roleId: value
     */
    stdScope.add<Enum>("Boolean")
        .values(
            ev_("true"),
            ev_("false"));

    stdScope.add<Struct>("Cell");
    stdScope.add<Struct>("Void");
    stdScope.add<Struct>("Slot")
        .members(
            member("key", "Cell"),
            member("type", "Struct"));

    stdScope.add<Struct>("Enum")
        .members(
            member("values", tt_("Map", "keyType", "Cell", "valueType", "Struct")));

    stdScope.add<Struct>("OpState")
        .members(
            member("op", "op::Base"),
            member("state", "Cell"),
            member("value", "Cell"));

    stdScope.add<Struct>("Container");
    stdScope.add<Struct>("Boolean");
    stdScope.add<Struct>("Char");
    stdScope.add<Struct>("Digit");

    stdScope.add<Enum>("NumberSign")
        .values(
            ev_("positive"),
            ev_("negative"));

    stdScope.add<Struct>("Number")
        .members(
            member("value", ListOf(std.Digit)),
            member("sign", "NumberSign"));

    stdScope.add<Struct>("String");

    stdScope.add<Struct>("Color")
        .members(
            member("red", "Number"),
            member("green", "Number"),
            member("blue", "Number"));

    stdScope.add<Struct>("Pixel");
    stdScope.add<Struct>("Grid");
    stdScope.add<Struct>("Stack");

    stdScope.add<Struct>("StackFrame")
        .members(
            member("method", "op::Function"),
            member("ops", "List"),
            member("input", "Index"),
            member("localVars", "Index"));

    stdScope.add<Struct>("Program")
        .members(
            member("data", "ProgramData"),
            member("instructions", "op::Base"),
            member("stack", "ListItem"));

    stdScope.add<Struct>("ProgramData")
        .members(
            member("functions", tt_("TrieMap", "keyType", "Cell", "valueType", "op::Function")),
            member("structs", tt_("TrieMap", "keyType", "Cell", "valueType", "Struct")),
            member("variables", tt_("TrieMap", "keyType", "Cell", "valueType", "op::Var")));

    stdScope.add<Struct>("StructReference")
        .members(
            member("id", tt_("List", "valueType", "Char")),
            member("idScope", "ast::Scope"),
            member("scope", "ast::Scope"),
            member("resolvedScope", "ast::Scope"),
            member("currentFn", "ast::Function"),
            member("currentStruct", "ast::Struct"),
            member("templateId", tt_("List", "valueType", "Cell")),
            member("templateParams", tt_("List", "valueType", "ast::Base")),
            member("value", "Struct"));

    stdScope.add<Struct>("CompileState")
        .members(
            member("currentFn", "ast::Function"),
            member("currentStruct", "ast::Struct"),
            member("lastBlock", "ast::Block"),
            member("scope", "ast::Scope"),
            member("resolvedScope", "ast::Scope"),
            member("globalScope", "ast::Scope"),
            member("globalResolvedScope", "ast::Scope"),
            member("functions", tt_("List", "valueType", "op::Function")),
            member("structs", tt_("TrieMap", "keyType", "Cell", "valueType", "Struct")),
            member("unknownStructs", tt_("TrieMap", "keyType", "Cell", "valueType", "Struct")),
            member("instances", tt_("TrieMap", "keyType", "Cell", "valueType", "Struct")),
            member("unknownInstances", tt_("TrieMap", "keyType", "Cell", "valueType", "Struct")),
            member("instanceAsts", tt_("TrieMap", "keyType", "Cell", "valueType", "Struct")),
            member("unknownInstanceAsts", tt_("TrieMap", "keyType", "Cell", "valueType", "Struct")),
            member("variables", tt_("TrieMap", "keyType", "Cell", "valueType", "op::Var")));

    stdScope.add<Struct>("Directions");

#pragma region ListItem
    stdScope.add<Struct>("ListItem")
        .subTypes(
            p_("ValueType", struct_("Cell")))
        .members(
            member("previous", "ListItem"),
            member("next", "ListItem"),
            member("value", st_("ValueType")));

    auto& listItemStructT
        = stdScope.add<StructT>("ListItem")
              .templateParams(
                  parameter("valueType", _(std.Struct)))
              .memberOf(
                  _(std.ListItem))
              .subTypes(
                  parameter("valueType", tp_("valueType")))
              .members(
                  member("previous", tt_("ListItem", "valueType", tp_("valueType"))),
                  member("next", tt_("ListItem", "valueType", tp_("valueType"))),
                  member("value", tp_("valueType")));

    listItemStructT.addMethod("constructor")
        .parameters(
            parameter("value", tp_("valueType")))
        .instructions(
            m_("value") = p_("value"));
#pragma endregion
#pragma region List
    stdScope.add<Struct>("List")
        .subTypes(
            parameter("itemType", struct_("ListItem")),
            parameter("valueType", struct_("Cell")))
        .members(
            member("first", "ListItem"),
            member("last", "ListItem"),
            member("size", _(std.Number)));

    auto& listStructT
        = stdScope.add<StructT>("List")
              .templateParams(
                  parameter("valueType", _(std.Struct)))
              .memberOf(
                  struct_("Container"),
                  struct_("List"))
              .subTypes(
                  parameter("itemType", tt_("ListItem", "valueType", tp_("valueType"))),
                  parameter("valueType", tp_("valueType")))
              .members(
                  member("first", st_("itemType")),
                  member("last", st_("itemType")),
                  member("size", _(std.Number)));

    auto& listIteratorStructT
        = stdScope.add<StructT>("ListIterator")
              .templateParams(
                  parameter("valueType", _(std.Struct)))
              .members(
                  member("list", tt_("List", "valueType", tp_("valueType"))),
                  member("node", tp_("valueType")));

    listIteratorStructT.addMethod("constructor")
        .parameters(
            parameter("list", tt_("List", "valueType", tp_("valueType"))))
        .instructions(
            m_("list") = p_("list"));
    /*
    trait Iterable {
        type Iterator: std::Iterator;

        fn iterator() -> Self::Iterator;
    }
    */
    auto& iterableTrait
        = stdScope.add<Trait>("Iterable")
              .associatedTypes(
                  parameter("Iterator", _("Iterator")))
              .addMethod("iterator").returnType(at_("Iterator"));

    /*
    impl<T> trait Iterable for List<T> {
        type Iterator = ListIterator<T>;

        fn iterator() -> Self::Iterator {
            return new Self::Iterator(list=self())
        }
    }
    */
    auto& implIterableTraitForListT
        = stdScope.add<TraitImpl>("Iterable")
              .templateParams(
                  parameter("valueType", _(std.Struct)))
              .implementedFor(tt_("List", "valueType", tp_("valueType")))
              .associatedTypes(
                  parameter("Iterator", tt_("ListIterator", tp_("valueType"))));

    implIterableTraitForListT.addMethod("iterator")
        .returnType(at_("ValueType"))
        .instructions(
            return_(new_(at_("Iterator"), "constructor")("list", self())));

    /*
    trait Iterator
    {
        type ValueType;

        bool isEmpty();
        void setFirstValue();
        ValueType getCurrentValue();
        bool hasNextValue();
        void setNextValue();
    }
    */
    auto& iteratorTrait
        = stdScope.add<Trait>("Iterator")
              .associatedTypes(parameter("ValueType", _(std.Struct)));

    iteratorTrait.addMethod("isEmpty").returnType(_(std.Boolean));
    iteratorTrait.addMethod("setFirstValue");
    iteratorTrait.addMethod("getCurrentValue").returnType(_(std.Boolean));
    iteratorTrait.addMethod("hasNextValue").returnType(_(std.Boolean));
    iteratorTrait.addMethod("setNextValue");

    auto& implIteratorTraitForListT
        = stdScope.add<TraitImpl>("Iterator")
              .templateParams(
                  parameter("ValueType", _(std.Struct)))
              .implementedFor(tt_("ListIterator", "valueType", tp_("ValueType")))
              .associatedTypes(
                  parameter("NodeType", tp_("ValueType")))
              .members(
                  member("node", tp_("ValueType")));

    implIteratorTraitForListT.addMethod("isEmpty")
        .returnType(_(std.Boolean))
        .instructions(
            return_(equal(m_("size"), _(_0_))));

    implIteratorTraitForListT.addMethod("setFirstValue")
        .instructions(
            set(m_("node"), _(ids.value), m_("first")));

    implIteratorTraitForListT.addMethod("getCurrentValue")
        .returnType(st_("NodeType"))
        .instructions(
            return_(m_("node")));

    implIteratorTraitForListT.addMethod("hasNextValue")
        .returnType(_(std.Boolean))
        .instructions(
            return_(has(m_("node"), "next")));

    implIteratorTraitForListT.addMethod("setNextValue")
        .instructions(
            m_("node") = m_("node") / "next");

    listStructT.addMethod("constructor")
        .instructions(
            m_("size") = _(_0_));

    listStructT.addMethod("add")
        .parameters(
            parameter("value", tp_("valueType")))
        .returnType(st_("itemType"))
        .instructions(
            var_("item") = new_(st_("itemType"), "constructor")("value", p_("value")),
            if_(not_(m_("first").exist()))
                .then_(m_("first") = *var_("item"))
                .else_(block(
                    set(m_("last"), "next", *var_("item")),
                    set(*var_("item"), "previous", m_("last")))),
            m_("last") = *var_("item"),
            m_("size") = add(m_("size"), _(_1_)),
            return_(*var_("item")));

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
            parameter("item", _(std.Cell)))
        .instructions(
            if_(has(p_("item"), "previous"))
                .then_(
                    if_(has(p_("item"), "next"))
                        .then_(set(p_("item") / "previous", "next", p_("item") / "next"))
                        .else_(erase(p_("item") / "previous", "next")))
                .else_(
                    if_(has(p_("item"), "next"))
                        .then_(m_("first") = p_("item") / "next")
                        .else_(erase(self(), "first"))),
            if_(has(p_("item"), "next"))
                .then_(
                    if_(has(p_("item"), "previous"))
                        .then_(set(p_("item") / "next", "previous", p_("item") / "previous"))
                        .else_(erase(p_("item") / "next", "previous")))
                .else_(
                    if_(has(p_("item"), "previous"))
                        .then_(m_("last") = p_("item") / "previous")
                        .else_(erase(self(), "last"))),
            m_("size") = subtract(m_("size"), _(_1_)));

    listStructT.addMethod("size")
        .returnType(_(std.Number))
        .instructions(
            return_(m_("size")));

    listStructT.addMethod("empty")
        .returnType(_(std.Boolean))
        .instructions(
            return_(equal(m_("size"), _(_0_))));

    listStructT.addMethod("first")
        .returnType(tp_("valueType"))
        .instructions(
            return_(m_("first") / "value"));

    listStructT.addMethod("last")
        .returnType(tp_("valueType"))
        .instructions(
            return_(m_("last") / "value"));

    listStructT.addMethod("begin")
        .returnType(st_("itemType"))
        .instructions(
            return_(m_("first")));

    listStructT.addMethod("end")
        .returnType(st_("itemType"))
        .instructions(
            return_(m_("last")));
#pragma endregion
#pragma region Struct
    auto& structStruct
        = stdScope.add<Struct>("Struct")
              .members(
                  member("name", tt_("List", "valueType", "Char")),
                  member("slots", tt_("Map", "keyType", "Cell", "valueType", "Slot")),
                  member("enum", "Boolean"),
                  member("incomplete", "Boolean"),
                  member("sharedObject", "Slot"),
                  member("subTypes", tt_("Map", "keyType", "Cell", "valueType", "Struct")),
                  member("memberOf", tt_("Map", "keyType", "Struct", "valueType", "Struct")),
                  member("asts", tt_("Map", "keyType", "Cell", "valueType", "ast::Function")),
                  member("methods", tt_("Map", "keyType", "Cell", "valueType", "op::Function")));

    structStruct.addMethod("constructor")
        .instructions(
            m_("slots") = new_(tt_("Map", "keyType", "Cell", "valueType", "Slot"), "constructor"));

    structStruct.addMethod("constructorWithRecursiveType")
        .instructions(
            m_("slots") = new_(tt_("Map", "keyType", "Cell", "valueType", "Slot"), "constructorWithIndexType")("indexType", self()));

    structStruct.addMethod("addSubType")
        .parameters(
            parameter("key", _(std.Cell)),
            parameter("type", _(std.Struct)))
        .instructions(
            if_(m_("subTypes").missing())
                .then_(m_("subTypes") = new_(tt_("Map", "keyType", _(std.Cell), "valueType", "Struct"), "constructor")),
            m_("subTypes")("add")("key", p_("key"))("value", p_("type")));

    structStruct.addMethod("addMembership")
        .parameters(
            parameter("cell", _(std.Struct)))
        .instructions(
            if_(m_("memberOf").missing())
                .then_(m_("memberOf") = new_(tt_("Map", "keyType", "Struct", "valueType", "Struct"), "constructor")),
            m_("memberOf")("add")("key", p_("cell"))("value", p_("cell")));

    structStruct.addMethod("addSlot")
        .parameters(
            parameter("key", _(std.Cell)),
            parameter("type", _(std.Slot)))
        .instructions(
            if_(m_("slots").missing())
                .then_(m_("slots") = new_(tt_("Map", "keyType", _(std.Cell), "valueType", _(std.Slot)), "constructor")),
            var_("slot") = new_(_(std.Slot)),
            set(*var_("slot"), "key", p_("key")),
            set(*var_("slot"), "type", p_("type")),
            m_("slots")("add")("key", p_("key"))("value", *var_("slot")));

    structStruct.addMethod("addSlots")
        .parameters(
            parameter("list", tt_("List", "valueType", _(std.Slot))))
        .instructions(
            if_(equal(p_("list") / "size", _(_0_)))
                .then_(return_()),
            var_("item") = p_("list") / "first",
            if_(m_("slots").missing())
                .then_(m_("slots") = new_(tt_("Map", "keyType", _(std.Cell), "valueType", _(std.Slot)), "constructor")),
            do_(block(
                        var_("next") = true_(),
                        m_("slots")("add")("key", *var_("item") / "value" / "key")("value", *var_("item") / "value"),
                        if_(has(*var_("item"), "next"))
                            .then_(var_("item") = *var_("item") / "next")
                            .else_(var_("next") = false_())))
                .while_(same(*var_("next"), true_())));

    structStruct.addMethod("hasSlot")
        .parameters(
            parameter("key", _(std.Cell)))
        .returnType(_(std.Boolean))
        .instructions(
            if_(m_("slots").missing())
                .then_(return_(false_())),
            return_(m_("slots")("hasKey")("key", p_("key"))));

    structStruct.addMethod("removeSlot")
        .parameters(
            parameter("key", _(std.Cell)))
        .instructions(
            if_(m_("slots").missing())
                .then_(return_()),
            m_("slots")("remove")("key", p_("key")));
#pragma endregion
#pragma region Index
    auto& indexStruct
        = stdScope.add<Struct>("Index")
              .memberOf(_(std.Struct));

    indexStruct.addMethod("constructor")
        .instructions(
            set(self(), "struct", new_("Struct", "constructorWithRecursiveType")),
            set(m_("struct"), "methods", get(struct_("Index"), _("methods"))),
            set(m_("struct"), "memberOf", _(map(std.Struct, std.Struct, std.Index, std.Index))));

    indexStruct.addMethod("constructorWithSelfType")
        .parameters(
            parameter("indexType", _(std.Struct)))
        .instructions(
            if_(missing(p_("indexType"), _("sharedObject")))
                .then_(block(set(p_("indexType"), "sharedObject", new_(_(std.Slot))),
                                 set(p_("indexType") / "sharedObject", "key", self()),
                                 set(p_("indexType") / "sharedObject", "type", struct_("Index")))),
            set(p_("indexType"), "methods", m_("struct") / "methods"),
            set(self(), "struct", p_("indexType")));

    /*
    void Index::insert(CellI& key, CellI& value)
    {
        if (&key == &"struct") {
            throw "The type key can not be changed!";
        }
        m_slots[&key] = &value;
        if (m_recursiveType) {
            return;
        }
        Object& slot = *new Object(kb, kb.type.Slot);
        slot.set("key", key);
        slot.set("type", kb.type.Slot);
        m_type->addSlot(key, slot);
    }
    */
    indexStruct.addMethod("insert")
        .parameters(
            parameter("key", _(std.Cell)),
            parameter("value", _(std.Cell)))
        .instructions(
            if_(same(p_("key"), _("struct")))
                .then_(return_()),
            set(self(), p_("key"), p_("value")),
            if_(and_(has(m_("struct"), "sharedObject"), same(m_("struct") / "sharedObject" / "key", self())))
                .then_(return_()),
            m_("struct")("addSlot")("key", p_("key"))("type", _(std.Slot)));

    indexStruct.addMethod("empty")
        .returnType(_(std.Boolean))
        .instructions(
            return_((m_("struct") / "slots")("empty")));

    /*
    void Index::erase(CellI& key)
    {
        if (!m_type->hasSlot(key)) {
            return;
        }
        m_slots.erase(&key);
        m_type->removeSlot(key);
    }
    */
    indexStruct.addMethod("remove")
        .parameters(
            parameter("key", _(std.Cell)))
        .instructions(
            if_(not_(m_("struct")("hasSlot")("key", p_("key"))))
                .then_(return_()),
            erase(self(), p_("key")),
            m_("struct")("removeSlot")("key", p_("key")));

    indexStruct.addMethod("size")
        .returnType(_(std.Number))
        .instructions(
            return_((m_("struct") / "slots")("size")));
#pragma endregion
#pragma region Map
    stdScope.add<Struct>("Map")
        .subTypes(
            parameter("keyType", struct_("Cell")),
            parameter("valueType", struct_("Cell")),
            parameter("listType", tt_("List", "valueType", struct_("Cell"))))
        .memberOf(struct_("Container"))
        .members(
            member("list", st_("listType")),
            member("index", "Index"),
            member("size", _(std.Number)));

    auto& mapStructT
        = stdScope.add<StructT>("Map")
              .templateParams(
                  parameter("keyType", _(std.Struct)),
                  parameter("valueType", _(std.Struct)))
              .subTypes(
                  parameter("keyType", tp_("keyType")),
                  parameter("valueType", tp_("valueType")),
                  parameter("listType", tt_("List", "valueType", tp_("valueType"))))
              .memberOf(struct_("Map"))
              .members(
                  member("list", st_("listType")),
                  member("index", "Index"),
                  member("size", _(std.Number)));

    mapStructT.addMethod("constructor")
        .instructions(
            m_("size")  = _(_0_),
            m_("list")  = new_(st_("listType"), "constructor"),
            m_("index") = new_("Index", "constructor"));

    mapStructT.addMethod("constructorWithIndexType")
        .parameters(
            parameter("indexType", _(std.Struct)))
        .instructions(
            m_("size")  = _(_0_),
            m_("list")  = new_(st_("listType"), "constructor"),
            m_("index") = new_("Index", "constructorWithSelfType")("indexType", p_("indexType")));

    /*
    bool Map::hasKey(CellI& key)
    {
        return m_index.has(key);
    }
    */
    mapStructT.addMethod("hasKey")
        .parameters(
            parameter("key", tp_("keyType")))
        .returnType(_(std.Boolean))
        .instructions(
            return_(has(m_("index"), p_("key"))));

    /*
    CellI& Map::getValue(CellI& key)
    {
        if (m_index.has(key)) {
            return m_index[key][kb.ids.value];
        }
        throw "No such key!";
    }
    */
    mapStructT.addMethod("getValue")
        .parameters(
            parameter("key", tp_("keyType")))
        .returnType(tp_("valueType"))
        .instructions(
            if_(has(m_("index"), p_("key")))
                .then_(return_(m_("index") / p_("key") / "value"))
                .else_(return_(_("emptyObject"))));

    /*
    void Map::add(CellI& key, CellI& value)
    {
        if (&key == &"struct") {
            throw "ids.type can not be stored in a map!";
        }
        if (m_index.has(key)) {
            throw "A value already registered with this key";
        }
        List::Item& item = *m_list.add(value);
        m_index.insert(key, item);
        ++m_size;
    }
    */
    mapStructT.addMethod("add")
        .parameters(
            parameter("key", tp_("keyType")),
            parameter("value", tp_("valueType")))
        .instructions(
            if_(same(p_("key"), _("struct")))
                .then_(return_()),
            if_(has(m_("index"), p_("key")))
                .then_(return_()),
            m_("size")   = add(m_("size"), _(_1_)),
            var_("item") = m_("list")("add")("value", p_("value")),
            m_("index")("insert")("key", p_("key"))("value", *var_("item")));

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
            parameter("key", tp_("keyType")))
        .instructions(
            if_(missing(m_("index"), p_("key")))
                .then_(return_()),
            m_("list")("remove")("item", m_("index") / p_("key")),
            m_("index")("remove")("key", p_("key")),
            m_("size") = subtract(m_("size"), _(_1_)));

    mapStructT.addMethod("size")
        .returnType(_(std.Number))
        .instructions(
            return_(m_("size")));

    mapStructT.addMethod("empty")
        .returnType(_(std.Boolean))
        .instructions(
            return_(equal(m_("size"), _(_0_))));

    mapStructT.addMethod("first")
        .returnType(tp_("valueType"))
        .instructions(
            return_(m_("list") / "first" / "value"));

    mapStructT.addMethod("last")
        .returnType(tp_("valueType"))
        .instructions(
            return_(m_("list") / "last" / "value"));

    mapStructT.addMethod("begin")
        .returnType(tt_("ListItem", "valueType", tp_("valueType")))
        .instructions(
            return_(m_("list") / "first"));

    mapStructT.addMethod("end")
        .returnType(tt_("ListItem", "valueType", tp_("valueType")))
        .instructions(
            return_(m_("list") / "last"));
#pragma endregion
#pragma region TrieMap
    stdScope.add<Struct>("KVPair")
        .members(
            member("key", "Cell"),
            member("value", "Cell"));

    auto& kvPairT
        = stdScope.add<StructT>("KVPair")
              .templateParams(
                  parameter("keyType", _(std.Struct)),
                  parameter("valueType", _(std.Struct)))
              .subTypes(
                  parameter("keyType", tp_("keyType")),
                  parameter("valueType", tp_("valueType")))
              .memberOf(struct_("KVPair"))
              .members(
                  member("key", tp_("keyType")),
                  member("value", tp_("valueType")));

    kvPairT.addMethod("constructor")
        .parameters(
            parameter("key", tp_("keyType")),
            parameter("value", tp_("valueType")))
        .instructions(
            m_("key")   = p_("key"),
            m_("value") = p_("value"));

    // TODO This can be a template but nevermind ...
    stdScope.add<Struct>("TrieMapNode")
        .members(
            member("children", "Index"),
            member("data", "ListItem"),
            member("parent", "TrieMapNode"));

    stdScope.add<Struct>("TrieMap")
        .subTypes(
            parameter("keyType", struct_("Cell")),
            parameter("valueType", struct_("Cell")),
            parameter("pairType", tt_("KVPair", "keyType", struct_("Cell"), "valueType", struct_("Cell"))),
            parameter("listType", tt_("List", "valueType", st_("pairType"))))
        .memberOf(struct_("Container"))
        .members(
            member("list", st_("listType")),
            member("rootNode", struct_("TrieMapNode")),
            member("size", _(std.Number)));

    auto& trieMapStructT
        = stdScope.add<StructT>("TrieMap")
              .templateParams(
                  parameter("keyType", _(std.Struct)),
                  parameter("valueType", _(std.Struct)))
              .subTypes(
                  parameter("keyType", tp_("keyType")),
                  parameter("valueType", tp_("valueType")),
                  parameter("pairType", tt_("KVPair", "keyType", tp_("keyType"), "valueType", tp_("valueType"))),
                  parameter("listType", tt_("List", "valueType", st_("pairType"))))
              .memberOf(_(std.Container), _(std.TrieMap))
              .members(
                  member("list", st_("listType")),
                  member("rootNode", _(std.TrieMapNode)),
                  member("size", _(std.Number)));

    trieMapStructT.addMethod("constructor")
        .instructions(
            m_("list")     = new_(st_("listType"), "constructor"),
            m_("rootNode") = new_(_(std.TrieMapNode)),
            m_("size")     = _(_0_));

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
            parameter("key", tp_("keyType")))
        .returnType(_(std.Boolean))
        .instructions(
            var_("currentNode") = m_("rootNode"),
            var_("keyItem")     = _(ids.emptyObject),
            if_(has(p_("key"), "first"))
                .then_(var_("keyItem") = p_("key") / "first"),
            while_(notSame(*var_("keyItem"), _(ids.emptyObject)))
                .do_(block(
                    var_("keyItemObj") = *var_("keyItem") / "value",
                    var_("child")      = _(ids.emptyObject),
                    if_(missing(*var_("currentNode"), "children"))
                        .then_(return_(false_())),
                    var_("childrenIndex") = *var_("currentNode") / "children",
                    if_(has(*var_("childrenIndex"), *var_("keyItemObj")))
                        .then_(var_("child") = *var_("childrenIndex") / *var_("keyItemObj"))
                        .else_(return_(false_())),
                    var_("currentNode") = *var_("child"),
                    if_(has(*var_("keyItem"), "next"))
                        .then_(var_("keyItem") = *var_("keyItem") / "next")
                        .else_(var_("keyItem") = _(ids.emptyObject)))),
            if_(missing(*var_("currentNode"), "data"))
                .then_(return_(false_())),
            return_(true_()));

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
            parameter("key", tp_("keyType")))
        .returnType(tp_("valueType"))
        .instructions(
            var_("currentNode") = m_("rootNode"),
            var_("keyItem")     = _(ids.emptyObject),
            if_(has(p_("key"), "first"))
                .then_(var_("keyItem") = p_("key") / "first"),
            while_(notSame(*var_("keyItem"), _(ids.emptyObject)))
                .do_(block(
                    var_("keyItemObj") = *var_("keyItem") / "value",
                    var_("child")      = _(ids.emptyObject),
                    if_(missing(*var_("currentNode"), "children"))
                        .then_(return_(_(ids.emptyObject))),
                    var_("childrenIndex") = *var_("currentNode") / "children",
                    if_(has(*var_("childrenIndex"), *var_("keyItemObj")))
                        .then_(var_("child") = *var_("childrenIndex") / *var_("keyItemObj"))
                        .else_(return_(_(ids.emptyObject))),
                    var_("currentNode") = *var_("child"),
                    if_(has(*var_("keyItem"), "next"))
                        .then_(var_("keyItem") = *var_("keyItem") / "next")
                        .else_(var_("keyItem") = _(ids.emptyObject)))),
            if_(missing(*var_("currentNode"), "data"))
                .then_(return_(_(ids.emptyObject))),
            return_(*var_("currentNode") / "data" / "value" / "value"));

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
            parameter("key", tp_("keyType")),
            parameter("value", tp_("valueType")))
        .instructions(
            var_("currentNode") = m_("rootNode"),
            var_("keyItem")     = _(ids.emptyObject),
            if_(has(p_("key"), "first"))
                .then_(var_("keyItem") = p_("key") / "first"),
            while_(notSame(*var_("keyItem"), _(ids.emptyObject)))
                .do_(block(
                    var_("keyItemObj") = *var_("keyItem") / "value",
                    var_("child")      = _(ids.emptyObject),
                    if_(missing(*var_("currentNode"), "children"))
                        .then_(set(*var_("currentNode"), "children", new_("Index", "constructor"))),
                    var_("childrenIndex") = *var_("currentNode") / "children",
                    if_(has(*var_("childrenIndex"), *var_("keyItemObj")))
                        .then_(var_("child") = *var_("childrenIndex") / *var_("keyItemObj"))
                        .else_(block(
                            var_("child") = new_(_(std.TrieMapNode)),
                            set(*var_("child"), "parent", *var_("currentNode")),
                            var_("childrenIndex")("insert")("key", *var_("keyItemObj"))("value", *var_("child")))),
                    var_("currentNode") = *var_("child"),
                    if_(has(*var_("keyItem"), "next"))
                        .then_(var_("keyItem") = *var_("keyItem") / "next")
                        .else_(var_("keyItem") = _(ids.emptyObject)))),
            var_("item") = m_("list")("add")("value", new_(st_("pairType"), "constructor")("key", p_("key"))("value", p_("value"))),
            set(*var_("currentNode"), "data", *var_("item")),
            m_("size") = add(m_("size"), _(_1_)));

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
            parameter("key", tp_("keyType")))
        .instructions(
            var_("currentNode") = m_("rootNode"),
            var_("keyItem")     = _(ids.emptyObject),
            if_(has(p_("key"), "first"))
                .then_(var_("keyItem") = p_("key") / "first"),
            while_(notSame(*var_("keyItem"), _(ids.emptyObject)))
                .do_(block(
                    var_("keyItemObj") = *var_("keyItem") / "value",
                    var_("child")      = _(ids.emptyObject),
                    if_(missing(*var_("currentNode"), "children"))
                        .then_(return_()),
                    var_("childrenIndex") = *var_("currentNode") / "children",
                    if_(has(*var_("childrenIndex"), *var_("keyItemObj")))
                        .then_(var_("child") = *var_("childrenIndex") / *var_("keyItemObj"))
                        .else_(return_()),
                    var_("currentNode") = *var_("child"),
                    if_(has(*var_("keyItem"), "next"))
                        .then_(var_("keyItem") = *var_("keyItem") / "next")
                        .else_(var_("keyItem") = _(ids.emptyObject)))),
            if_(missing(*var_("currentNode"), "data"))
                .then_(return_()),
            var_("valueItem") = *var_("currentNode") / "data",
            erase(*var_("currentNode"), "data"),
            var_("keyItem") = p_("key") / "last",
            while_(has(*var_("currentNode"), "parent"))
                .do_(block(
                    var_("parent") = *var_("currentNode") / "parent",
                    var_("child")  = *var_("currentNode"),
                    if_(missing(*var_("child"), "data"))
                        .then_(
                            if_(or_(missing(*var_("child"), "children"), and_(has(*var_("child"), "children"), ((*var_("child") / "children")("empty")))))
                                .then_(block(
                                    delete_(*var_("currentNode")),
                                    erase(*var_("parent") / "children", *var_("keyItem") / "value")))),
                    var_("currentNode") = *var_("parent"),
                    if_(has(*var_("keyItem"), "previous"))
                        .then_(var_("keyItem") = *var_("keyItem") / "previous")
                        .else_(break_()))),
            m_("list")("remove")("item", *var_("valueItem")),
            m_("size") = subtract(m_("size"), _(_1_)));

    trieMapStructT.addMethod("size")
        .returnType(_(std.Number))
        .instructions(
            return_(m_("size")));

    trieMapStructT.addMethod("empty")
        .returnType(_(std.Boolean))
        .instructions(
            return_(equal(m_("size"), _(_0_))));

    trieMapStructT.addMethod("first")
        .returnType(tp_("valueType"))
        .instructions(
            return_(m_("list") / "first" / "value"));

    trieMapStructT.addMethod("last")
        .returnType(tp_("valueType"))
        .instructions(
            return_(m_("list") / "last" / "value"));

    trieMapStructT.addMethod("begin")
        .returnType(tt_("ListItem", "valueType", tp_("valueType")))
        .instructions(
            return_(m_("list") / "first"));

    trieMapStructT.addMethod("end")
        .returnType(tt_("ListItem", "valueType", tp_("valueType")))
        .instructions(
            return_(m_("list") / "last"));
#pragma endregion
#pragma region Set
    auto& setStructT
        = stdScope.add<StructT>("Set")
              .templateParams(
                  parameter("keyType", _(std.Struct)),
                  parameter("valueType", _(std.Struct)))
              .subTypes(
                  parameter("valueType", tp_("valueType")),
                  parameter("listType", tt_("List", "valueType", tp_("valueType"))))
              .memberOf(_(std.Container))
              .members(
                  member("index", struct_("Index")),
                  member("size", _(std.Number)));

    setStructT.addMethod("constructor")
        .instructions(
            m_("size")  = _(_0_),
            m_("index") = new_(struct_("Index"), "constructor"));

    setStructT.addMethod("add")
        .parameters(
            parameter("value", tp_("valueType")))
        .instructions(
            if_(has(m_("index"), p_("value")))
                .then_(return_()),
            m_("index")("insert")("key", p_("value"))("value", p_("value")),
            m_("size") = add(m_("size"), _(_1_)));

    setStructT.addMethod("contains")
        .parameters(
            parameter("value", tp_("valueType")))
        .returnType(_(std.Boolean))
        .instructions(
            return_(has(m_("index"), p_("value"))));

    setStructT.addMethod("remove")
        .parameters(
            parameter("value", tp_("valueType")))
        .instructions(
            if_(missing(m_("index"), p_("value")))
                .then_(return_()),
            m_("index")("remove")("key", p_("value")),
            m_("size") = subtract(m_("size"), _(_1_)));
#
    setStructT.addMethod("first")
        .returnType(tp_("valueType"))
        .instructions(
            return_(m_("index") / "struct" / "slots" / "list" / "first" / "value" / "key"));

    setStructT.addMethod("last")
        .returnType(tp_("valueType"))
        .instructions(
            return_(m_("index") / "struct" / "slots" / "list" / "last" / "value" / "key"));

    setStructT.addMethod("begin")
        .returnType(tt_("ListItem", "valueType", tp_("valueType")))
        .instructions(
            return_(m_("index") / "struct" / "slots" / "list" / "last"));

    setStructT.addMethod("end")
        .returnType(tt_("ListItem", "valueType", tp_("valueType")))
        .instructions(
            return_(m_("list") / "last"));

    setStructT.addMethod("size")
        .returnType(_(std.Number))
        .instructions(
            return_(m_("size")));

    setStructT.addMethod("empty")
        .returnType(_(std.Boolean))
        .instructions(return_(equal(m_("size"), _(_0_))));
#pragma endregion
}

class AstArc : public AstHelper
{
public:
    AstArc(Brain& kb);
};

AstArc::AstArc(Brain& kb) :
    AstHelper(kb)
{
    auto& arcScope = globalScope.add<Scope>("arc");

    auto& exampleStruct
        = arcScope.add<Struct>("Example")
              .members(
                  member("input", _(std.Grid)),
                  member("output", _(std.Grid)));

    auto& taskStruct
        = arcScope.add<Struct>("Task")
              .members(
                  member("examples", tt_("std::List", "valueType", "Example")),
                  member("tests", tt_("std::List", "valueType", "Example")),
                  member("solution", _(std.Grid)));

    arcScope.add<Enum>("Color")
        .values(
            ev_("black", _(_0_)),
            ev_("blue", _(_1_)),
            ev_("red", _(_2_)),
            ev_("green", _(_3_)),
            ev_("yellow", _(_4_)),
            ev_("grey", _(_5_)),
            ev_("fuschia", _(_6_)),
            ev_("orange", _(_7_)),
            ev_("teal", _(_8_)),
            ev_("brown", _(_9_)));

    arcScope.add<Enum>("RotationDir")
        .values(
            ev_("Degree_0"),   // 🡩
            ev_("Degree_45"),  // 🡭
            ev_("Degree_90"),  // 🡪
            ev_("Degree_135"), // 🡮
            ev_("Degree_180"), // 🡫
            ev_("Degree_225"), // 🡯
            ev_("Degree_270"), // 🡨
            ev_("Degree_315")  // 🡬
        );

    arcScope.add<Enum>("Directions")
        .values(
            ev_("up"),        // 🡩
            ev_("upRight"),   // 🡭
            ev_("right"),     // 🡪
            ev_("downRight"), // 🡮
            ev_("down"),      // 🡫
            ev_("downLeft"),  // 🡯
            ev_("left"),      // 🡨
            ev_("upLeft"));   // 🡬

    arcScope.add<Enum>("LineSymmetry")
        .values(
            ev_("horizontal"),         // │
            ev_("vertical"),           // ──
            ev_("diagonalLowerLeft"),  // /
            ev_("diagonalUpperLeft")); // \

    auto& colorStruct
        = arcScope.add<Struct>("Color")
              .members(
                  member("red", _(std.Number)),
                  member("green", _(std.Number)),
                  member("blue", _(std.Number)));

    // struct Pixel
    auto& pixelStruct
        = arcScope.add<Struct>("Pixel")
              .members(
                  member("x", _(std.Number)),
                  member("y", _(std.Number)));

    pixelStruct.addMethod("constructor")
        .parameters(
            parameter("x", _(std.Number)),
            parameter("y", _(std.Number)))
        .instructions(
            m_("x") = p_("x"),
            m_("y") = p_("y"));

    // struct Vector
    auto& vectorStruct
        = arcScope.add<Struct>("Vector")
              .members(
                  member("x", _(std.Number)),
                  member("y", _(std.Number)));

    vectorStruct.addMethod("constructor")
        .parameters(
            parameter("x", _(std.Number)),
            parameter("y", _(std.Number)))
        .instructions(
            m_("x") = p_("x"),
            m_("y") = p_("y"));

    // Vector rotate(RotationDir rotationDir) const;
    vectorStruct.addMethod("rotate")
        .parameters(
            parameter("rotationDir", struct_("RotationDir")))
        .returnType(struct_("Vector"))
        .instructions(
            var_("ret") = new_("Vector", "constructor")("x", m_("x"))("y", m_("y")),
            match_(p_("rotationDir"))
                // 🡩🡩🡩🡩🡩🡩🡩🡩🡩🡩🡩🡩🡩🡩🡩
                .case_(
                    "Degree_0",
                    return_(*var_("ret")))
                // 🡭🡭🡭🡭🡭🡭🡭🡭🡭🡭🡭🡭🡭🡭🡭
                .case_(
                    "Degree_45",
                    block(
                        set(*var_("ret"), "x", subtract(m_("x"), m_("y"))),
                        set(*var_("ret"), "y", add(m_("y"), m_("x")))))
                // 🡪🡪🡪🡪🡪🡪🡪🡪🡪🡪🡪🡪🡪🡪🡪
                .case_(
                    "Degree_90",
                    block(
                        set(*var_("ret"), "x", subtract(_(_0_), m_("y"))),
                        set(*var_("ret"), "y", m_("x"))))
                // 🡮🡮🡮🡮🡮🡮🡮🡮🡮🡮🡮🡮🡮🡮🡮
                .case_(
                    "Degree_135",
                    block(
                        set(*var_("ret"), "x", subtract(subtract(_(_0_), m_("x")), m_("y"))), // TODO maybe ast.negative(m_("x")) ?
                        set(*var_("ret"), "y", subtract(m_("x"), m_("y")))))
                // 🡫🡫🡫🡫🡫🡫🡫🡫🡫🡫🡫🡫🡫🡫🡫
                .case_(
                    "Degree_180",
                    block(
                        set(*var_("ret"), "x", subtract(_(_0_), m_("x"))),
                        set(*var_("ret"), "y", subtract(_(_0_), m_("y")))))
                // 🡯🡯🡯🡯🡯🡯🡯🡯🡯🡯🡯🡯🡯🡯🡯
                .case_(
                    "Degree_225",
                    block(
                        set(*var_("ret"), "x", add(subtract(_(_0_), m_("x")), m_("y"))),
                        set(*var_("ret"), "y", subtract(subtract(_(_0_), m_("x")), m_("y")))))
                // 🡨🡨🡨🡨🡨🡨🡨🡨🡨🡨🡨🡨🡨🡨🡨
                .case_(
                    "Degree_270",
                    block(
                        set(*var_("ret"), "x", m_("y")),
                        set(*var_("ret"), "y", subtract(_(_0_), m_("x")))))
                // 🡬🡬🡬🡬🡬🡬🡬🡬🡬🡬🡬🡬🡬🡬🡬
                .case_(
                    "Degree_315",
                    block(
                        set(*var_("ret"), "x", add(m_("x"), m_("y"))),
                        set(*var_("ret"), "y", add(subtract(_(_0_), m_("x")), m_("y"))))),

            return_(*var_("ret")));

    // struct VectorShape
    auto& vectorShapeStruct
        = arcScope.add<Struct>("VectorShape")
              .members(
                  member("color", _(std.Color)),
                  member("vectors", tt_("std::List", "valueType", "Vector")),
                  member("firstPixel", "Pixel"));

    vectorShapeStruct.addMethod("constructor")
        .parameters(
            parameter("color", _(std.Color)))
        .instructions(
            m_("color")   = p_("color"),
            m_("vectors") = new_(tt_("std::List", "valueType", "Vector"), "constructor"));

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
            parameter("pixels", tt_("std::List", "valueType", "Pixel")))
        .instructions(
            m_("firstPixel")     = p_("pixels") / "first" / "value",
            var_("prevPixel")    = m_("firstPixel"),
            var_("isFirstPixel") = true_(),
            var_("pixel")        = _(ids.emptyObject),
            if_(has(p_("pixels"), "first"))
                .then_(var_("pixel") = p_("pixels") / "first"),
            while_(notSame(*var_("pixel"), _(ids.emptyObject)))
                .do_(block(
                    if_(same(*var_("isFirstPixel"), true_()))
                        .then_(block(
                            var_("isFirstPixel") = false_(),
                            if_(has(*var_("pixel"), "next"))
                                .then_(var_("pixel") = *var_("pixel") / "next")
                                .else_(var_("pixel") = _(ids.emptyObject)),
                            continue_())),
                    var_("vector") = new_("Vector", "constructor")("x", subtract(*var_("pixel") / "value" / "x", *var_("prevPixel") / "x"))("y", subtract(*var_("pixel") / "value" / "y", *var_("prevPixel") / "y")),
                    m_("vectors")("add")("value", *var_("vector")),
                    var_("prevPixel") = *var_("pixel") / "value",
                    if_(has(*var_("pixel"), "next"))
                        .then_(var_("pixel") = *var_("pixel") / "next")
                        .else_(var_("pixel") = _(ids.emptyObject)))));

    /*
    VectorShape VectorShape::rotate(RotationDir rotationDir) const
    {
        std::vector<Vector> rotatedVectors;

        for (const Vector& vector : m_vectors) {
            Vector newVector = vector.rotate(rotationDir);
            rotatedVectors.push_back(newVector);
            //            loggerPtr->log(DEBUG) << " rotate vector [" << vector.x << ", " << vector.y << "] => [" << newVector.x << ", " << newVector.y << "]";
        }

        return VectorShape(std::move(rotatedVectors), m_color, m_firstPixel);
    }
    */
    vectorShapeStruct.addMethod("rotate")
        .parameters(
            parameter("rotationDir", struct_("RotationDir")))
        .returnType(struct_("VectorShape"))
        .instructions(
            var_("ret")            = new_(struct_("VectorShape")),
            var_("rotatedVectors") = new_(tt_("std::List", "valueType", "Vector"), "constructor"),
            set(*var_("ret"), "color", m_("color")),
            set(*var_("ret"), "firstPixel", m_("firstPixel")),
            set(*var_("ret"), "vectors", *var_("rotatedVectors")),
            var_("vector") = _(ids.emptyObject),
            if_(has(m_("vectors"), "first"))
                .then_(var_("vector") = m_("vectors") / "first"),
            while_(notSame(*var_("vector"), _(ids.emptyObject)))
                .do_(block(
                    var_("newVector") = ((*var_("vector") / "value")("rotate")("rotationDir", p_("rotationDir"))),
                    var_("rotatedVectors")("add")("value", *var_("newVector")),
                    if_(has(*var_("vector"), "next"))
                        .then_(var_("vector") = *var_("vector") / "next")
                        .else_(var_("vector") = _(ids.emptyObject)))),
            return_(*var_("ret")));

    // struct ShapeEdgeKind
    arcScope.add<Enum>("ShapeEdgeKind")
        .values(
            ev_("ExternalEdge"),
            ev_("InternalEdge"));

    // struct ShapeEdgeRotationCorners
    auto& ShapeEdgeRotationCornersStruct
        = arcScope.add<Struct>("ShapeEdgeRotationCorners")
              .members(
                  member("upLeftNode", "ShapeEdgeNode"),
                  member("upRightNode", "ShapeEdgeNode"),
                  member("downLeftNode", "ShapeEdgeNode"),
                  member("downRightNode", "ShapeEdgeNode"));

    // struct ShapeEdgeMirroringCorners
    auto& ShapeEdgeMirroringCornersStruct
        = arcScope.add<Struct>("ShapeEdgeMirroringCorners")
              .members(
                  member("upRightNode", "ShapeEdgeNode"),
                  member("downLeftNode", "ShapeEdgeNode"));

    // struct ShapeEdge
    auto& ShapeEdgeStruct
        = arcScope.add<Struct>("ShapeEdge")
              .members(
                  member("id", _(std.Number)),
                  member("shape", "Shape"),
                  member("kind", "ShapeEdgeKind"),
                  member("fromExternalX", _(std.Number)),
                  member("fromExternalY", _(std.Number)),
                  member("shapes", tt_("std::Set", "valueType", "Shape")),
                  member("shapePixels", tt_("std::List", "valueType", "ShapePixel")),
                  member("rotationCorners", "ShapeEdgeRotationCorners"),
                  member("mirroringCorners", "ShapeEdgeMirroringCorners"),
                  member("upLeftNode", "ShapeEdgeNode"),
                  member("upRightNode", "ShapeEdgeNode"),
                  member("downLeftNode", "ShapeEdgeNode"),
                  member("downRightNode", "ShapeEdgeNode"),
                  member("edgeNodes", tt_("std::List", "valueType", "ShapeEdgeNode")));

    // struct ShapeEdgeNode
    auto& ShapeEdgeNodeStruct
        = arcScope.add<Struct>("ShapeEdgeNode")
              .members(
                  member("edge", "ShapeEdge"),
                  member("from", "ShapePoint"),
                  member("direction", "Directions"),
                  member("externalShape", "Shape"),
                  member("next", "ShapeEdgeNode"),
                  member("previous", "ShapeEdgeNode"));

    // struct ShapeEdge
    auto& ShapeEdgeJointStruct
        = arcScope.add<Struct>("ShapeEdgeJoint")
              .members(
                  member("upLeft", "ShapeEdgeNode"),
                  member("upRight", "ShapeEdgeNode"),
                  member("downLeft", "ShapeEdgeNode"),
                  member("downRight", "ShapeEdgeNode"),
                  member("leftUp", "ShapeEdgeNode"),
                  member("leftDown", "ShapeEdgeNode"),
                  member("rightUp", "ShapeEdgeNode"),
                  member("rightDown", "ShapeEdgeNode"));

    // struct ShapePoint
    auto& shapePointStruct
        = arcScope.add<Struct>("ShapePoint")
              .members(
                  member("edgeJoint", "ShapeEdgeJoint"),
                  member("up", "ShapePoint"),
                  member("down", "ShapePoint"),
                  member("left", "ShapePoint"),
                  member("right", "ShapePoint"),
                  member("upLeftPixel", "ShapePixel"),
                  member("upRightPixel", "ShapePixel"),
                  member("downLeftPixel", "ShapePixel"),
                  member("downRightPixel", "ShapePixel"),
                  member("x", _(std.Number)),
                  member("y", _(std.Number)));

    // struct ShapePixel
    auto& shapePixelStruct
        = arcScope.add<Struct>("ShapePixel")
              .members(
                  member("shape", "Shape"),
                  member("up", "ShapePixel"),
                  member("down", "ShapePixel"),
                  member("left", "ShapePixel"),
                  member("right", "ShapePixel"),
                  member("upLeftPoint", "ShapePoint"),
                  member("upRightPoint", "ShapePoint"),
                  member("downLeftPoint", "ShapePoint"),
                  member("downRightPoint", "ShapePoint"),
                  member("pixel", _(std.Pixel)));

    shapePixelStruct.addMethod("constructor")
        .parameters(
            parameter("shape", struct_("Shape")),
            parameter("pixel", _(std.Pixel)))
        .instructions(
            m_("shape") = p_("shape"),
            m_("pixel") = p_("pixel"));

    // struct Shape
    auto& shapeStruct
        = arcScope.add<Struct>("Shape")
              .subTypes(
                  parameter("InternalEdgeLookup", tt_("std::Map", "keyType", _(std.Number), "valueType", tt_("std::Map", "keyType", _(std.Number), "valueType", "ShapeEdge"))))
              .members(
                  member("id", _(std.Number)),
                  member("color", "Color"),
                  member("width", _(std.Number)),
                  member("height", _(std.Number)),
                  member("lastEdgeId", _(std.Number)),
                  member("edges", tt_("std::Map", "keyType", _(std.Number), "valueType", "ShapeEdge")),
                  member("internalEdges", st_("InternalEdgeLookup")),
                  member("pixels", tt_("std::List", "valueType", "Pixel")),
                  member("shapePixels", tt_("std::List", "valueType", "ShapePixel")),
                  member("shapePoints", tt_("std::List", "valueType", "ShapePoint")));

    /*
    Shape(int id, input::Color color, int width, int height) :
        m_id(id), m_color(color), m_width(width), m_height(height) { }
    */
    shapeStruct.addMethod("constructor")
        .parameters(
            parameter("id", _(std.Number)),
            parameter("color", struct_("Color")),
            parameter("width", _(std.Number)),
            parameter("height", _(std.Number)))
        .instructions(
            m_("id")           = p_("id"),
            m_("color")        = p_("color"),
            m_("width")        = p_("width"),
            m_("height")       = p_("height"),
            m_("lastEdgeId")   = _(_0_),
            m_("edges")        = new_(tt_("std::Map", "keyType", _(std.Number), "valueType", "ShapeEdge"), "constructor"));

    shapeStruct.addMethod("toVectorShape")
        .returnType(struct_("VectorShape"))
        .instructions(
            var_("ret") = new_("VectorShape", "constructor")("color", m_("color")),
            var_("ret")("fromPixels")("pixels", m_("pixels")),
            return_(*var_("ret")));

    // struct Frame
    auto& frameStruct
        = arcScope.add<Struct>("Frame")
              .subTypes(
                  parameter("tableType", tt_("std::Map", "keyType", _(std.Number), "valueType", tt_("std::Map", "keyType", _(std.Number), "valueType", "Shape"))))
              .members(
                  member("width", _(std.Number)),
                  member("height", _(std.Number)),
                  member("grid", _(std.Grid)),
                  member("upLeftPoint", "ShapePoint"),
                  member("upRightPoint", "ShapePoint"),
                  member("downLeftPoint", "ShapePoint"),
                  member("downRightPoint", "ShapePoint"),
                  member("shapes", tt_("std::List", "valueType", "Shape")),
                  member("shapeMap", tt_("std::Map", "keyType", _(std.Number), "valueType", "Shape")),
                  member("shapePixels", st_("tableType")),
                  member("inputPixels", tt_("std::Set", "valueType", _(std.Pixel))));

    // Frame::Frame
    frameStruct.addMethod("constructor")
        .parameters(
            parameter("grid", _(std.Grid)))
        .instructions(
            m_("grid")        = p_("grid"),
            m_("width")       = p_("grid") / "width",
            m_("height")      = p_("grid") / "height",
            m_("shapes")      = new_(tt_("std::List", "valueType", "Shape"), "constructor"),
            m_("shapePixels") = new_(st_("tableType"), "constructor"),
            m_("shapeMap")    = new_(tt_("std::Map", "keyType", _(std.Number), "valueType", "Shape"), "constructor"),
            m_("inputPixels") = new_(tt_("std::Set", "valueType", _(std.Pixel)), "constructor"),
            self()("processInputPixels"));

    // Frame::processInputPixels
    frameStruct.addMethod("processInputPixels")
        .instructions(
            var_("pixels") = m_("grid") / "pixels",
            if_(missing(*var_("pixels"), "first"))
                .then_(return_()),
            var_("pixel") = *var_("pixels") / "first",
            while_(true_())
                .do_(block(
                    m_("inputPixels")("add")("value", *var_("pixel") / "value"),
                   if_(missing(*var_("pixel"), "next"))
                       .then_(return_()),
                   var_("pixel") = *var_("pixel") / "next")));

#if 0
    // Frame::processInputPixels
    frameStruct.addMethod("processInputPixelsTest")
        .instructions(
            for_("pixel").in(m_("grid") / "pixels")(block(
                m_("inputPixels")("add")("value", *var_("pixel"))
            )));
#endif

    // Frame::process
    frameStruct.addMethod("process")
        .instructions(
            var_("shapeId") = _(_1_),
            while_(not_(m_("inputPixels")("empty")))
                .do_(block(
                    var_("firstPixel")  = m_("inputPixels")("first"),
                    var_("shape")       = new_("Shape", "constructor")("id", *var_("shapeId"))("color", *var_("firstPixel") / "color")("width", m_("width"))("height", m_("height")),
                    var_("shapeId")     = add(*var_("shapeId"), _(_1_)),
                    var_("checkPixels") = new_(tt_("std::Set", "valueType", _(std.Pixel)), "constructor"),
                    var_("checkPixels")("add")("value", *var_("firstPixel")),
                    while_(not_(var_("checkPixels")("empty")))
                        .do_(block(
                            var_("checkPixel") = var_("checkPixels")("first"),
                            self()("processPixel")("shape", *var_("shape"))("checkPixels", *var_("checkPixels"))("checkPixel", *var_("checkPixel")),
                            var_("checkPixels")("remove")("value", *var_("checkPixel")))))),
            var_("y") = _(_0_),
            while_(lessThan(*var_("y"), m_("height")))
                .do_(block(
                    var_("colX") = m_("shapePixels")("getValue")("key", *var_("y")),
                    var_("x")    = _(_0_),
                    while_(lessThan(*var_("x"), m_("width")))
                        .do_(block(
                            var_("shapePixel") = var_("colX")("getValue")("key", *var_("x")),
                            var_("shape")      = *var_("shapePixel") / "shape",
                            var_("pixel")      = *var_("shapePixel") / "pixel",
                            if_(not_(m_("shapeMap")("hasKey")("key", *var_("shape") / "id")))
                                .then_(block(
                                    m_("shapeMap")("add")("key", *var_("shape") / "id")("value", *var_("shape")),
                                    m_("shapes")("add")("value", *var_("shape")))),
                            var_("x") = add(*var_("x"), _(_1_)))),
                    var_("y") = add(*var_("y"), _(_1_)))));

    // Frame::processPixel
    frameStruct.addMethod("processPixel")
        .parameters(
            parameter("shape", struct_("Shape")),
            parameter("checkPixels", tt_("std::Set", "valueType", "Pixel")),
            parameter("checkPixel", struct_("Pixel")))
        .instructions(
            if_(not_(m_("shapePixels")("hasKey")("key", p_("checkPixel") / _(coordinates.y))))
                .then_(m_("shapePixels")("add")("key", p_("checkPixel") / _(coordinates.y))("value", new_(st_("tableType"), "constructor"))), // TODO just a TableRow, not a full TableType
            var_("colX") = m_("shapePixels")("getValue")("key", p_("checkPixel") / _(coordinates.y)),
            var_("colX")("add")("key", p_("checkPixel") / _(coordinates.x))("value", new_("ShapePixel", "constructor")("shape", p_("shape"))("pixel", p_("checkPixel"))),
            m_("inputPixels")("remove")("value", p_("checkPixel")),
            self()("processAdjacentPixel")("direction", _(directions.up))("shape", p_("shape"))("checkPixels", p_("checkPixels"))("checkPixel", p_("checkPixel")),
            self()("processAdjacentPixel")("direction", _(directions.down))("shape", p_("shape"))("checkPixels", p_("checkPixels"))("checkPixel", p_("checkPixel")),
            self()("processAdjacentPixel")("direction", _(directions.left))("shape", p_("shape"))("checkPixels", p_("checkPixels"))("checkPixel", p_("checkPixel")),
            self()("processAdjacentPixel")("direction", _(directions.right))("shape", p_("shape"))("checkPixels", p_("checkPixels"))("checkPixel", p_("checkPixel")));

    // Frame::processAdjacentPixel
    frameStruct.addMethod("processAdjacentPixel")
        .parameters(
            parameter("direction", _(std.Directions)),
            parameter("shape", struct_("Shape")),
            parameter("checkPixels", tt_("std::Set", "valueType", _(std.Pixel))),
            parameter("checkPixel", _(std.Pixel)))
        .instructions(
            if_(has(p_("checkPixel"), p_("direction")))
                .then_(block(
                    var_("pixel") = p_("checkPixel") / p_("direction"),
                    if_(m_("shapePixels")("hasKey")("key", *var_("pixel") / _(coordinates.y)))
                        .then_(block(
                            var_("colX") = m_("shapePixels")("getValue")("key", *var_("pixel") / _(coordinates.y)),
                            if_(var_("colX")("hasKey")("key", *var_("pixel") / _(coordinates.x)))
                                .then_(block(
                                    var_("shape") = get(var_("colX")("getValue")("key", *var_("pixel") / _(coordinates.x)), "shape"),
                                    if_(same(p_("shape"), *var_("shape")))
                                        .then_(return_()))))),
                    if_(same(*var_("pixel") / "color", p_("shape") / "color"))
                        .then_(p_("checkPixels")("add")("value", *var_("pixel"))))));
}


class AstTest : public AstHelper
{
public:
    AstTest(Brain& kb);
};

AstTest::AstTest(Brain& kb) :
    AstHelper(kb)
{
    auto& testScope = globalScope.add<Scope>("test");

    auto& testFunction = testScope.add<Function>("testFunction");
    testFunction.instructions(
        var_("result") = new_(struct_("std::Index")));

    auto& testVariable = testScope.add<Var>("testVariable");
    auto& testStruct   = testScope.add<Struct>("TestStruct");

    testStruct.addMethod("testCreateNewListOfNumbers")
        .instructions(
            var_("result") = new_(struct_("std::Index")),
            var_("result") = new_(tt_("std::List", "valueType", _(std.Number))),
            var_("result") = new_(tt_("std::List", "valueType", _(std.Cell))),
            var_("result") = new_(tt_("std::List", "valueType", _(std.Pixel))),
            var_("result") = new_(tt_("std::Set", "valueType", _(std.Number))),
            var_("result") = new_(tt_("std::Map", "keyType", _(std.Number), "valueType", _(std.Color))),
            var_("result") = new_(tt_("std::TrieMap", "keyType", _(std.Number), "valueType", _(std.Color))));

    testStruct.addMethod("factorial")
        .parameters(
            parameter("input", _(std.Number)))
        .returnType(_(std.Number))
        .instructions(
            if_(greaterThanOrEqual(p_("input"), _(_1_)))
                .then_(return_(multiply(p_("input"), self()("factorial")("input", subtract(p_("input"), _(_1_))))))
                .else_(return_(_(_1_))));

    testScope.add<Enum>("TestEnum")
        .values(
            ev_("value1"), // init with Void
            ev_("value2"));

    testScope.add<Enum>("TestEnumWithValues")
        .values(
            ev_("value1", _(_1_)), // init with a value
            ev_("value2", _(_2_)));

    testScope.add<Enum>("TestEnumTyped")
        .values(
            tev_("value1", struct_("TestStruct")), // init with value
            tev_("value2", "TestStruct"));

    testScope.add<Enum>("TestEnumTypedWithValues")
        .values(
            tev_("value1", "TestStruct", _(_1_)), // init with value
            tev_("value2", "TestStruct", _(_2_)));

    // TODO
    //    type.String.method(ids.addSlots, { ids.list, list(type.slot(ids.value, type.ListOf(type.Char))) });
    // try/catch: almost the same as break/continue/return it can go through function calls. We need an op::Catch node
    // output: we need some kind of output, maybe a console thing first. Maybe just a new hybrid cell is needed
    // Type should hold an std::Type which can be a std::Struct, std::Enum or similar
    // Iterators, range-based-for
    // Variable scopes
    //
}

void Brain::createContent()
{
    AstStd astStd(*this);
    AstArc astArc(*this);
    AstTest astTest(*this);
}

Brain::Brain(std::function<void()> loggerLevelInit) :
    m_initPhase(InitPhase::Init),
    logger(loggerLevelInit),
    pools(*this),
    ids(*this),
    globalScope(Ast::Scope(*this, "global")),
    std(*this),
    ast(*this),
    directions(*this),
    coordinates(*this),
    boolean(*this),
    numbers(*this),
    ap(*this),
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
    m_compiler(std::make_unique<Compiler>(*this))
{
    createContent();

    Compiler& compiler = *m_compiler;
    compiler.reigisterStructBeforeCompilation(tt_("std::List", "valueType", _(std.Char)));    // TODO instantiate on demand in getStruct
    compiler.registerBuiltInStruct("std::op::Activate", std.op.Activate);
    compiler.registerBuiltInStruct("std::op::Add", std.op.Add);
    compiler.registerBuiltInStruct("std::op::And", std.op.And);
    compiler.registerBuiltInStruct("std::op::Base", std.op.Base);
    compiler.registerBuiltInStruct("std::op::Block", std.op.Block);
    compiler.registerBuiltInStruct("std::op::Call", std.op.Call);
    compiler.registerBuiltInStruct("std::op::ConstVar", std.op.ConstVar);
    compiler.registerBuiltInStruct("std::op::Delete", std.op.Delete);
    compiler.registerBuiltInStruct("std::op::Divide", std.op.Divide);
    compiler.registerBuiltInStruct("std::op::Do", std.op.Do);
    compiler.registerBuiltInStruct("std::op::Equal", std.op.Equal);
    compiler.registerBuiltInStruct("std::op::Erase", std.op.Erase);
    compiler.registerBuiltInStruct("std::op::Function", std.op.Function);
    compiler.registerBuiltInStruct("std::op::Get", std.op.Get);
    compiler.registerBuiltInStruct("std::op::GreaterThan", std.op.GreaterThan);
    compiler.registerBuiltInStruct("std::op::GreaterThanOrEqual", std.op.GreaterThanOrEqual);
    compiler.registerBuiltInStruct("std::op::Has", std.op.Has);
    compiler.registerBuiltInStruct("std::op::If", std.op.If);
    compiler.registerBuiltInStruct("std::op::LessThan", std.op.LessThan);
    compiler.registerBuiltInStruct("std::op::LessThanOrEqual", std.op.LessThanOrEqual);
    compiler.registerBuiltInStruct("std::op::Missing", std.op.Missing);
    compiler.registerBuiltInStruct("std::op::Multiply", std.op.Multiply);
    compiler.registerBuiltInStruct("std::op::New", std.op.New);
    compiler.registerBuiltInStruct("std::op::Not", std.op.Not);
    compiler.registerBuiltInStruct("std::op::NotEqual", std.op.NotEqual);
    compiler.registerBuiltInStruct("std::op::NotSame", std.op.NotSame);
    compiler.registerBuiltInStruct("std::op::Or", std.op.Or);
    compiler.registerBuiltInStruct("std::op::Return", std.op.Return);
    compiler.registerBuiltInStruct("std::op::Same", std.op.Same);
    compiler.registerBuiltInStruct("std::op::Set", std.op.Set);
    compiler.registerBuiltInStruct("std::op::Subtract", std.op.Subtract);
    compiler.registerBuiltInStruct("std::op::Var", std.op.Var);
    compiler.registerBuiltInStruct("std::op::While", std.op.While);
    compiler.registerBuiltInStruct("std::ast::Add", std.ast.Add);
    compiler.registerBuiltInStruct("std::ast::And", std.ast.And);
    compiler.registerBuiltInStruct("std::ast::Base", std.ast.Base);
    compiler.registerBuiltInStruct("std::ast::Block", std.ast.Block);
    compiler.registerBuiltInStruct("std::ast::Break", std.ast.Break);
    compiler.registerBuiltInStruct("std::ast::Call", std.ast.Call);
    compiler.registerBuiltInStruct("std::ast::Cell", std.ast.Cell);
    compiler.registerBuiltInStruct("std::ast::Continue", std.ast.Continue);
    compiler.registerBuiltInStruct("std::ast::Delete", std.ast.Delete);
    compiler.registerBuiltInStruct("std::ast::Divide", std.ast.Divide);
    compiler.registerBuiltInStruct("std::ast::Do", std.ast.Do);
    compiler.registerBuiltInStruct("std::ast::Enum", std.ast.Enum);
    compiler.registerBuiltInStruct("std::ast::EnumValue", std.ast.EnumValue);
    compiler.registerBuiltInStruct("std::ast::Equal", std.ast.Equal);
    compiler.registerBuiltInStruct("std::ast::Erase", std.ast.Erase);
    compiler.registerBuiltInStruct("std::ast::Function", std.ast.Function);
    compiler.registerBuiltInStruct("std::ast::FunctionT", std.ast.FunctionT);
    compiler.registerBuiltInStruct("std::ast::Get", std.ast.Get);
    compiler.registerBuiltInStruct("std::ast::GreaterThan", std.ast.GreaterThan);
    compiler.registerBuiltInStruct("std::ast::GreaterThanOrEqual", std.ast.GreaterThanOrEqual);
    compiler.registerBuiltInStruct("std::ast::Has", std.ast.Has);
    compiler.registerBuiltInStruct("std::ast::If", std.ast.If);
    compiler.registerBuiltInStruct("std::ast::LessThan", std.ast.LessThan);
    compiler.registerBuiltInStruct("std::ast::LessThanOrEqual", std.ast.LessThanOrEqual);
    compiler.registerBuiltInStruct("std::ast::Member", std.ast.Member);
    compiler.registerBuiltInStruct("std::ast::Missing", std.ast.Missing);
    compiler.registerBuiltInStruct("std::ast::Multiply", std.ast.Multiply);
    compiler.registerBuiltInStruct("std::ast::New", std.ast.New);
    compiler.registerBuiltInStruct("std::ast::Not", std.ast.Not);
    compiler.registerBuiltInStruct("std::ast::NotEqual", std.ast.NotEqual);
    compiler.registerBuiltInStruct("std::ast::NotSame", std.ast.NotSame);
    compiler.registerBuiltInStruct("std::ast::Or", std.ast.Or);
    compiler.registerBuiltInStruct("std::ast::Parameter", std.ast.Parameter);
    compiler.registerBuiltInStruct("std::ast::ResolvedType", std.ast.ResolvedType);
    compiler.registerBuiltInStruct("std::ast::Return", std.ast.Return);
    compiler.registerBuiltInStruct("std::ast::Same", std.ast.Same);
    compiler.registerBuiltInStruct("std::ast::Scope", std.ast.Scope);
    compiler.registerBuiltInStruct("std::ast::Self", std.ast.Self);
    compiler.registerBuiltInStruct("std::ast::SelfFn", std.ast.SelfFn);
    compiler.registerBuiltInStruct("std::ast::Set", std.ast.Set);
    compiler.registerBuiltInStruct("std::ast::Slot", std.ast.Slot);
    compiler.registerBuiltInStruct("std::ast::StaticCall", std.ast.StaticCall);
    compiler.registerBuiltInStruct("std::ast::Struct", std.ast.Struct);
    compiler.registerBuiltInStruct("std::ast::StructName", std.ast.StructName);
    compiler.registerBuiltInStruct("std::ast::StructT", std.ast.StructT);
    compiler.registerBuiltInStruct("std::ast::Subtract", std.ast.Subtract);
    compiler.registerBuiltInStruct("std::ast::SubTypeName", std.ast.SubTypeName);
    compiler.registerBuiltInStruct("std::ast::TemplatedType", std.ast.TemplatedType);
    compiler.registerBuiltInStruct("std::ast::TemplateParam", std.ast.TemplateParam);
    compiler.registerBuiltInStruct("std::ast::Trait", std.ast.Trait);
    compiler.registerBuiltInStruct("std::ast::TraitImpl", std.ast.TraitImpl);
    compiler.registerBuiltInStruct("std::ast::TypedEnumValue", std.ast.TypedEnumValue);
    compiler.registerBuiltInStruct("std::ast::Var", std.ast.Var);
    compiler.registerBuiltInStruct("std::ast::While", std.ast.While);
    compiler.registerBuiltInStruct("std::Cell", std.Cell);
    compiler.registerBuiltInStruct("std::Slot", std.Slot);
    compiler.registerBuiltInStruct("std::Struct", std.Struct);
    compiler.registerBuiltInStruct("std::Enum", std.Enum);
    compiler.registerBuiltInStruct("std::OpState", std.OpState);
    compiler.registerBuiltInStruct("std::Container", std.Container);
    compiler.registerBuiltInStruct("std::List", std.List);
    compiler.registerBuiltInStruct("std::ListItem", std.ListItem);
    compiler.registerBuiltInStruct("std::KVPair", std.KVPair);
    compiler.registerBuiltInStruct("std::Map", std.Map);
    compiler.registerBuiltInStruct("std::Index", std.Index);
    compiler.registerBuiltInStruct("std::TrieMap", std.TrieMap);
    compiler.registerBuiltInStruct("std::TrieMapNode", std.TrieMapNode);
    compiler.registerBuiltInStruct("std::Boolean", std.Boolean);
    compiler.registerBuiltInStruct("std::Char", std.Char);
    compiler.registerBuiltInStruct("std::Digit", std.Digit);
    compiler.registerBuiltInStruct("std::Number", std.Number);
    compiler.registerBuiltInStruct("std::String", std.String);
    compiler.registerBuiltInStruct("std::Color", std.Color);
    compiler.registerBuiltInStruct("std::Pixel", std.Pixel);
    compiler.registerBuiltInStruct("std::Grid", std.Grid);
    compiler.registerBuiltInStruct("std::Stack", std.Stack);
    compiler.registerBuiltInStruct("std::StackFrame", std.StackFrame);
    compiler.registerBuiltInStruct("std::Program", std.Program);
    compiler.registerBuiltInStruct("std::ProgramData", std.ProgramData);
    compiler.registerBuiltInStruct("std::StructReference", std.StructReference);
    compiler.registerBuiltInStruct("std::CompileState", std.CompileState);
    compiler.registerBuiltInStruct("std::Directions", std.Directions);
    auto& compiledGlobalScope = compiler.compile(globalScope);
    globalScope.m_toolFinder  = compiler.getToolFinder();
    compiledGlobalScopePtr    = &compiledGlobalScope[ids.data];
    m_initPhase               = InitPhase::FullyConstructed;

    // Test should be removed from here
    TRACE(compiledSymbols, "All compiled symbols:");

    TRACE(compiledSymbols, "  structs:");
    auto& compiledStructs = static_cast<TrieMap&>(compiledGlobalScope[ids.data][ids.structs]);
    Visitor::visitList(compiledStructs[ids.list], [this](CellI& kv, int, bool&) {
        TRACE(compiledSymbols, "    {}", kv[ids.key].label());
    });

    TRACE(compiledSymbols, "  functions:");
    auto& compiledFunctions = static_cast<TrieMap&>(compiledGlobalScope[ids.data][ids.functions]);
    Visitor::visitList(compiledFunctions[ids.list], [this](CellI& kv, int, bool&) {
        TRACE(compiledSymbols, "    {} : {}", kv[ids.key].label(), kv[ids.value].label());
    });

    TRACE(compiledSymbols, "  variables:");
    auto& compiledVariables = static_cast<TrieMap&>(compiledGlobalScope[ids.data][ids.variables]);
    Visitor::visitList(compiledVariables[ids.list], [this](CellI& kv, int, bool&) {
        TRACE(compiledSymbols, "    {} : {}", kv[ids.key].label(), kv[ids.value].label());
    });
    auto& compiledListItemStruct = static_cast<TrieMap&>(compiledGlobalScope[ids.data][ids.structs]).getValue(templateId("std::ListItem", name("valueType"), std.Cell));
    auto& compiledListStruct     = static_cast<TrieMap&>(compiledGlobalScope[ids.data][ids.structs]).getValue(templateId("std::List", name("valueType"), std.Cell));
    auto& compiledTypeStruct     = static_cast<TrieMap&>(compiledGlobalScope[ids.data][ids.structs]).getValue(name("std::Struct"));
    auto& compiledIndexStruct    = static_cast<TrieMap&>(compiledGlobalScope[ids.data][ids.structs]).getValue(name("std::Index"));
    std.ListItem.set("methods", compiledListItemStruct[ids.methods]);
    std.List.set("methods", compiledListStruct[ids.methods]);
    std.Struct.set("methods", compiledTypeStruct[ids.methods]);

    Object testType(*this, compiledTypeStruct, name("constructor"), "testType");
    Object testRecursiveType(*this, compiledTypeStruct, name("constructorWithRecursiveType"), "testRecursiveType");

    Object testIndex(*this, compiledIndexStruct, name("constructor"), "testIndex");
    testIndex.method(name("insert"), { "key", _1_ }, { "value", _2_ });
}

Brain::~Brain()
{
    m_initPhase = InitPhase::DestructBegin;
}

CellI& Brain::getStruct(const std::string& nameStr)
{
    return getStruct(name(nameStr));
}

CellI& Brain::getStruct(CellI& name)
{
    switch (m_initPhase) {
    case InitPhase::Init:
        throw "Get struct before compilation is not possible";
    case InitPhase::Compiling:
        throw "Get struct during compilation is not possible";
    case InitPhase::FullyConstructed:
        return static_cast<TrieMap&>((*compiledGlobalScopePtr)[ids.structs]).getValue(name);
    case InitPhase::DestructBegin:
        return ids.emptyObject;
    }
    throw "Unhandled state!";
}

CellI& Brain::getVariable(const std::string& nameStr)
{
    return getVariable(name(nameStr));
}

CellI& Brain::getVariable(CellI& name)
{
    switch (m_initPhase) {
    case InitPhase::Init:
        throw "Get variable before compilation is not possible";
    case InitPhase::Compiling:
        throw "Get variable during compilation is not possible";
    case InitPhase::FullyConstructed:
        return static_cast<TrieMap&>((*compiledGlobalScopePtr)[ids.variables]).getValue(name);
    case InitPhase::DestructBegin:
        return ids.emptyObject;
    }
    throw "Unhandled state!";
}

CellI& Brain::name(const std::string& str)
{
    return pools.strings.getCharList(str);
}

CellI& Brain::ListOf(CellI& valueType)
{
    switch (m_initPhase) {
    case InitPhase::Init: {
        auto& ret = tt_("std::List", "valueType", _(valueType));
        m_compiler->reigisterStructBeforeCompilation(ret);

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
        m_compiler->reigisterStructBeforeCompilation(ret);

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

Brain::Logger::Logger(std::function<void()> loggerLevelInit)
{
    registerLogger("cells");
    registerLogger("compileStruct");
    registerLogger("symbolResolver");
    registerLogger("compiledSymbols");
    registerLogger("toolFinder");
    loggerLevelInit();
}

Brain::Logger::~Logger()
{
    for (const auto& name : m_loggerNames) {
        spdlog::drop(name);
    }
}

void Brain::Logger::registerLogger(const std::string& name)
{
    // handle if multiple Brain instance is created
    auto logger = spdlog::get(name);
    if (logger) {
        return;
    }
    createLogger(name);
    m_loggerNames.push_back(name);
}

std::shared_ptr<spdlog::logger> Brain::Logger::createLogger(const std::string& name)
{
    static auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_st>();
    console_sink->set_level(spdlog::level::trace);
    console_sink->set_pattern("[%n][%^%L%$] %v");

    auto logger = std::make_shared<spdlog::logger>(name, console_sink);
    logger->set_level(spdlog::level::trace);
    spdlog::register_logger(logger);

    return logger;
}

} // namespace cells
} // namespace infocell
