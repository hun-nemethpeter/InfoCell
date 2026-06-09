#include "Brain.h"
#include "Compiler.h"

#include "Arc.h"
#include "StdLib.h"

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
