#include "World.h"
#include "Compiler.h"

#include "ArcLib.h"
#include "StdLib.h"
#include "TestLib.h"

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include "util/Log.h"

namespace infocell {
namespace cells {

ID::ID(World& w) :
    w(w),
    __type__(w, w.std.Char, "__type__"),
    argument(w, w.std.Char, "argument"),
    ast(w, w.std.Char, "ast"),
    asts(w, w.std.Char, "asts"),
    blue(w, w.std.Char, "blue"),
    break_(w, w.std.Char, "break_"),
    cell(w, w.std.Char, "cell"),
    children(w, w.std.Char, "children"),
    color(w, w.std.Char, "color"),
    compiled(w, w.std.Char, "compiled"),
    condition(w, w.std.Char, "condition"),
    constructor(w, w.std.Char, "constructor"),
    container(w, w.std.Char, "container"),
    continue_(w, w.std.Char, "continue_"),
    currentFn(w, w.std.Char, "currentFn"),
    currentParam(w, w.std.Char, "currentParam"),
    currentStruct(w, w.std.Char, "currentStruct"),
    data(w, w.std.Char, "data"),
    description(w, w.std.Char, "description"),
    destructor(w, w.std.Char, "destructor"),
    else_(w, w.std.Char, "else_"),
    enum_(w, w.std.Char, "enum_"),
    emptyObject(w, w.std.Char, "emptyObject"),
    first(w, w.std.Char, "first"),
    functions(w, w.std.Char, "functions"),
    globalScope(w, w.std.Char, "globalScope"),
    green(w, w.std.Char, "green"),
    height(w, w.std.Char, "height"),
    id(w, w.std.Char, "id"),
    index(w, w.std.Char, "index"),
    input(w, w.std.Char, "input"),
    instances(w, w.std.Char, "instances"),
    instructions(w, w.std.Char, "instructions"),
    item(w, w.std.Char, "item"),
    itemType(w, w.std.Char, "itemType"),
    key(w, w.std.Char, "key"),
    keyType(w, w.std.Char, "keyType"),
    last(w, w.std.Char, "last"),
    lastOp(w, w.std.Char, "lastOp"),
    lhs(w, w.std.Char, "lhs"),
    list(w, w.std.Char, "list"),
    listType(w, w.std.Char, "listType"),
    localVars(w, w.std.Char, "localVars"),
    memberOf(w, w.std.Char, "memberOf"),
    members(w, w.std.Char, "members"),
    method(w, w.std.Char, "method"),
    methods(w, w.std.Char, "methods"),
    name(w, w.std.Char, "name"),
    next(w, w.std.Char, "next"),
    objectType(w, w.std.Char, "objectType"),
    op(w, w.std.Char, "op"),
    ops(w, w.std.Char, "ops"),
    output(w, w.std.Char, "output"),
    parameters(w, w.std.Char, "parameters"),
    parent(w, w.std.Char, "parent"),
    pixels(w, w.std.Char, "pixels"),
    pixelsMap(w, w.std.Char, "pixelsMap"),
    pop(w, w.std.Char, "pop"),
    previous(w, w.std.Char, "previous"),
    process(w, w.std.Char, "process"),
    push(w, w.std.Char, "push"),
    red(w, w.std.Char, "red"),
    resolvedScope(w, w.std.Char, "resolvedScope"),
    result(w, w.std.Char, "result"),
    return_(w, w.std.Char, "return_"),
    returnType(w, w.std.Char, "returnType"),
    rhs(w, w.std.Char, "rhs"),
    rootNode(w, w.std.Char, "rootNode"),
    scope(w, w.std.Char, "scope"),
    scopes(w, w.std.Char, "scopes"),
    self(w, w.std.Char, "self"),
    size(w, w.std.Char, "size"),
    slots(w, w.std.Char, "slots"),
    stack(w, w.std.Char, "stack"),
    state(w, w.std.Char, "state"),
    stateCondition(w, w.std.Char, "stateCondition"),
    stateElse(w, w.std.Char, "stateElse"),
    stateLhs(w, w.std.Char, "stateLhs"),
    statement(w, w.std.Char, "statement"),
    stateParam1(w, w.std.Char, "stateParam1"),
    stateParam2(w, w.std.Char, "stateParam2"),
    stateParam3(w, w.std.Char, "stateParam3"),
    stateParamEval(w, w.std.Char, "stateParamEval"),
    stateParamInit(w, w.std.Char, "stateParamInit"),
    stateRhs(w, w.std.Char, "stateRhs"),
    stateStackCall(w, w.std.Char, "stateStackCall"),
    stateStatement(w, w.std.Char, "stateStatement"),
    stateThen(w, w.std.Char, "stateThen"),
    static_(w, w.std.Char, "static_"),
    status(w, w.std.Char, "status"),
    structs(w, w.std.Char, "structs"),
    structType(w, w.std.Char, "structType"),
    typeAliases(w, w.std.Char, "typeAliases"),
    tag(w, w.std.Char, "tag"),
    templateId(w, w.std.Char, "templateId"),
    templateParams(w, w.std.Char, "templateParams"),
    then(w, w.std.Char, "then"),
    throw_(w, w.std.Char, "throw"),
    type(w, w.std.Char, "type"),
    unknownInstances(w, w.std.Char, "unknownInstances"),
    unknownStructs(w, w.std.Char, "unknownStructs"),
    value(w, w.std.Char, "value"),
    valueType(w, w.std.Char, "valueType"),
    variable(w, w.std.Char, "variable"),
    variables(w, w.std.Char, "variables"),
    width(w, w.std.Char, "width")
{
}

namespace type {

Op::Op(World& w) :
    w(w),
    Activate(w, w.std.Struct, "op::Activate"),
    Add(w, w.std.Struct, "op::Add"),
    And(w, w.std.Struct, "op::And"),
    Base(w, w.std.Struct, "op::Base"),
    Block(w, w.std.Struct, "op::Block"),
    Call(w, w.std.Struct, "op::Call"),
    ConstVar(w, w.std.Struct, "op::ConstVar"),
    Delete(w, w.std.Struct, "op::Delete"),
    Divide(w, w.std.Struct, "op::Divide"),
    Do(w, w.std.Struct, "op::Do"),
    Equal(w, w.std.Struct, "op::Equal"),
    Erase(w, w.std.Struct, "op::Erase"),
    Function(w, w.std.Struct, "op::Function"),
    Get(w, w.std.Struct, "op::Get"),
    GreaterThan(w, w.std.Struct, "op::GreaterThan"),
    GreaterThanOrEqual(w, w.std.Struct, "op::GreaterThanOrEqual"),
    Has(w, w.std.Struct, "op::Has"),
    If(w, w.std.Struct, "op::If"),
    LessThan(w, w.std.Struct, "op::LessThan"),
    LessThanOrEqual(w, w.std.Struct, "op::LessThanOrEqual"),
    Missing(w, w.std.Struct, "op::Missing"),
    Multiply(w, w.std.Struct, "op::Multiply"),
    New(w, w.std.Struct, "op::New"),
    Not(w, w.std.Struct, "op::Not"),
    NotEqual(w, w.std.Struct, "op::NotEqual"),
    NotSame(w, w.std.Struct, "op::NotSame"),
    Or(w, w.std.Struct, "op::Or"),
    Return(w, w.std.Struct, "op::Return"),
    Same(w, w.std.Struct, "op::Same"),
    Set(w, w.std.Struct, "op::Set"),
    Subtract(w, w.std.Struct, "op::Subtract"),
    Var(w, w.std.Struct, "op::Var"),
    While(w, w.std.Struct, "op::While")
{
}

Ast::Ast(World& w) :
    w(w),
    Add(w, w.std.Struct, "ast::Add"),
    And(w, w.std.Struct, "ast::And"),
    Base(w, w.std.Struct, "ast::Base"),
    Block(w, w.std.Struct, "ast::Block"),
    Break(w, w.std.Struct, "ast::Break"),
    Call(w, w.std.Struct, "ast::Call"),
    Cell(w, w.std.Struct, "ast::Cell"),
    Continue(w, w.std.Struct, "ast::Continue"),
    Delete(w, w.std.Struct, "ast::Delete"),
    Divide(w, w.std.Struct, "ast::Divide"),
    Do(w, w.std.Struct, "ast::Do"),
    Enum(w, w.std.Struct, "ast::Enum"),
    EnumValue(w, w.std.Struct, "ast::EnumValue"),
    Equal(w, w.std.Struct, "ast::Equal"),
    Erase(w, w.std.Struct, "ast::Erase"),
    For(w, w.std.Struct, "ast::For"),
    Function(w, w.std.Struct, "ast::Function"),
    FunctionT(w, w.std.Struct, "ast::FunctionT"),
    Get(w, w.std.Struct, "ast::Get"),
    GreaterThan(w, w.std.Struct, "ast::GreaterThan"),
    GreaterThanOrEqual(w, w.std.Struct, "ast::GreaterThanOrEqual"),
    Has(w, w.std.Struct, "ast::Has"),
    If(w, w.std.Struct, "ast::If"),
    LessThan(w, w.std.Struct, "ast::LessThan"),
    LessThanOrEqual(w, w.std.Struct, "ast::LessThanOrEqual"),
    Match(w, w.std.Struct, "ast::Match"),
    Member(w, w.std.Struct, "ast::Member"),
    Missing(w, w.std.Struct, "ast::Missing"),
    Multiply(w, w.std.Struct, "ast::Multiply"),
    New(w, w.std.Struct, "ast::New"),
    Not(w, w.std.Struct, "ast::Not"),
    NotEqual(w, w.std.Struct, "ast::NotEqual"),
    NotSame(w, w.std.Struct, "ast::NotSame"),
    Or(w, w.std.Struct, "ast::Or"),
    Parameter(w, w.std.Struct, "ast::Parameter"),
    ResolvedType(w, w.std.Struct, "ast::ResolvedType"),
    Return(w, w.std.Struct, "ast::Return"),
    Same(w, w.std.Struct, "ast::Same"),
    Scope(w, w.std.Struct, "ast::Scope"),
    Self(w, w.std.Struct, "ast::Self"),
    SelfFn(w, w.std.Struct, "ast::SelfFn"),
    Set(w, w.std.Struct, "ast::Set"),
    Slot(w, w.std.Struct, "ast::Slot"),
    StaticCall(w, w.std.Struct, "ast::StaticCall"),
    Struct(w, w.std.Struct, "ast::Struct"),
    StructName(w, w.std.Struct, "ast::StructName"),
    StructT(w, w.std.Struct, "ast::StructT"),
    TypeAlias(w, w.std.Struct, "ast::TypeAlias"),
    Subtract(w, w.std.Struct, "ast::Subtract"),
    TemplatedType(w, w.std.Struct, "ast::TemplatedType"),
    TemplateParam(w, w.std.Struct, "ast::TemplateParam"),
    Throw(w, w.std.Struct, "ast::Throw"),
    Trait(w, w.std.Struct, "ast::Trait"),
    TraitImpl(w, w.std.Struct, "ast::TraitImpl"),
    Try(w, w.std.Struct, "ast::Try"),
    TypedEnumValue(w, w.std.Struct, "ast::TypedEnumValue"),
    Var(w, w.std.Struct, "ast::Var"),
    While(w, w.std.Struct, "ast::While")
{
}

} // namespace type

Std::Std(World& w) :
    w(w),
    Cell(w, w.std.Struct, "Cell"),
    Slot(w, w.std.Struct, "Slot"),
    Struct(w, w.std.Struct, "Struct"),
    Enum(w, w.std.Struct, "Enum"),
    OpState(w, w.std.Struct, "OpState"),
    Container(w, w.std.Struct, "Conatainer"),
    List(w, w.std.Struct, "List"),
    ListItem(w, w.std.Struct, "ListItem"),
    KVPair(w, w.std.Struct, "KVPair"),
    Map(w, w.std.Struct, "Map"),
    Index(w, w.std.Struct, "Index"),
    TrieMap(w, w.std.Struct, "TrieMap"),
    TrieMapNode(w, w.std.Struct, "TrieMapNode"),
    Boolean(w, w.std.Struct, "Boolean"),
    Char(w, w.std.Struct, "Char"),
    Digit(w, w.std.Struct, "Digit"),
    Number(w, w.std.Struct, "Number"),
    String(w, w.std.Struct, "String"),
    Color(w, w.std.Struct, "Color"),
    Pixel(w, w.std.Struct, "Pixel"),
    Grid(w, w.std.Struct, "Grid"),
    Stack(w, w.std.Struct, "Stack"),
    StackFrame(w, w.std.Struct, "StackFrame"),
    Library(w, w.std.Struct, "Library"),
    StructReference(w, w.std.Struct, "StructReference"),
    Directions(w, w.std.Enum, "Directions"),
    op(w),
    ast(w)
{
}

cells::CellI& Std::slot(cells::CellI& key, cells::CellI& type)
{
    CellI& ret = *new Object(w, w.std.Slot);
    ret.set(w.ids.key, key);
    ret.set(w.ids.type, type);

    return ret;
}

cells::CellI& Std::slot(const std::string& key, cells::CellI& type)
{
    CellI& ret = *new Object(w, w.std.Slot);
    ret.set(w.ids.key, w.name(key));
    ret.set(w.ids.type, type);

    return ret;
}

cells::CellI& Std::kvPair(cells::CellI& key, cells::CellI& value)
{
    CellI& ret = *new Object(w, w.std.KVPair);
    ret.set(w.ids.key, key);
    ret.set(w.ids.value, value);

    return ret;
}

Directions::Directions(World& w) :
    up(w, w.std.Char, "up"),
    down(w, w.std.Char, "down"),
    left(w, w.std.Char, "left"),
    right(w, w.std.Char, "right")
{
}

Coordinates::Coordinates(World& w) :
    x(w, w.std.Char, "x"),
    y(w, w.std.Char, "y")
{
}

Boolean::Boolean(World& w) :
    true_(w, w.std.Boolean, "true"),
    false_(w, w.std.Boolean, "false")
{
}

Numbers::Numbers(World& w) :
    sign(w, w.std.Cell, w.std.Cell, "sign"),
    positive(w, w.std.Cell, "positive"),
    negative(w, w.std.Cell, "negative")

{
    sign.add(positive, positive);
    sign.add(negative, negative);
}

// ============================================================================
Pools::Chars::Chars(World& w) :
    w(w)
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
                             std::forward_as_tuple(w, w.std.Char, characterName));
        auto& value = it.first->second;
    }
}

// ============================================================================
Pools::Digits::Digits(World& w)
{
    m_digits.reserve(10);
    for (int i = 0; i < 10; ++i) {
        std::string digitName = "Digit_" + std::to_string(i);
        m_digits.emplace_back(w, w.std.Digit, digitName);
    }
}

Object& Pools::Digits::operator[](int digit)
{
    return m_digits[digit];
}

// ============================================================================
Pools::Numbers::Numbers(World& w) :
    w(w)
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
                                              std::forward_as_tuple(w, number));
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
Pools::Strings::Strings(World& w) :
    w(w)
{
    StringInit reservedStrings[] = {
        { "__type__", w.ids.__type__ },
        { "argument", w.ids.argument },
        { "ast", w.ids.ast },
        { "asts", w.ids.asts },
        { "blue", w.ids.blue },
        { "cell", w.ids.cell },
        { "children", w.ids.children },
        { "color", w.ids.color },
        { "compiled", w.ids.compiled },
        { "condition", w.ids.condition },
        { "constructor", w.ids.constructor },
        { "container", w.ids.container },
        { "continue_", w.ids.continue_ },
        { "currentFn", w.ids.currentFn },
        { "currentParam", w.ids.currentParam },
        { "currentStruct", w.ids.currentStruct },
        { "data", w.ids.data },
        { "description", w.ids.description },
        { "destructor", w.ids.destructor },
        { "else_", w.ids.else_ },
        { "emptyObject", w.ids.emptyObject },
        { "first", w.ids.first },
        { "functions", w.ids.functions },
        { "globalScope", w.ids.globalScope },
        { "green", w.ids.green },
        { "height", w.ids.height },
        { "id", w.ids.id },
        { "index", w.ids.index },
        { "input", w.ids.input },
        { "instances", w.ids.instances },
        { "instructions", w.ids.instructions },
        { "item", w.ids.item },
        { "itemType", w.ids.itemType },
        { "key", w.ids.key },
        { "keyType", w.ids.keyType },
        { "last", w.ids.last },
        { "lastOp", w.ids.lastOp },
        { "lhs", w.ids.lhs },
        { "list", w.ids.list },
        { "listType", w.ids.listType },
        { "localVars", w.ids.localVars },
        { "memberOf", w.ids.memberOf },
        { "members", w.ids.members },
        { "method", w.ids.method },
        { "methods", w.ids.methods },
        { "name", w.ids.name },
        { "next", w.ids.next },
        { "objectType", w.ids.objectType },
        { "op", w.ids.op },
        { "ops", w.ids.ops },
        { "output", w.ids.output },
        { "parameters", w.ids.parameters },
        { "parent", w.ids.parent },
        { "pixels", w.ids.pixels },
        { "pixelsMap", w.ids.pixelsMap },
        { "previous", w.ids.previous },
        { "red", w.ids.red },
        { "resolvedScope", w.ids.resolvedScope },
        { "result", w.ids.result },
        { "returnType", w.ids.returnType },
        { "rhs", w.ids.rhs },
        { "key", w.ids.key },
        { "rootNode", w.ids.rootNode },
        { "scope", w.ids.scope },
        { "scopes", w.ids.scopes },
        { "self", w.ids.self },
        { "size", w.ids.size },
        { "key", w.ids.key },
        { "slots", w.ids.slots },
        { "stack", w.ids.stack },
        { "state", w.ids.state },
        { "stateCondition", w.ids.stateCondition },
        { "stateElse", w.ids.stateElse },
        { "stateLhs", w.ids.stateLhs },
        { "statement", w.ids.statement },
        { "stateParam1", w.ids.stateParam1 },
        { "stateParam2", w.ids.stateParam2 },
        { "stateParam3", w.ids.stateParam3 },
        { "stateParamEval", w.ids.stateParamEval },
        { "stateParamInit", w.ids.stateParamInit },
        { "stateRhs", w.ids.stateRhs },
        { "stateStackCall", w.ids.stateStackCall },
        { "stateStatement", w.ids.stateStatement },
        { "stateThen", w.ids.stateThen },
        { "static_", w.ids.static_ },
        { "status", w.ids.status },
        { "structs", w.ids.structs },
        { "structType", w.ids.structType },
        { "typeAliases", w.ids.typeAliases },
        { "tag", w.ids.tag },
        { "templateId", w.ids.templateId },
        { "templateParams", w.ids.templateParams },
        { "then", w.ids.then },
        { "throw", w.ids.throw_ },
        { "type", w.ids.type },
        { "unknownInstances", w.ids.unknownInstances },
        { "unknownStructs", w.ids.unknownStructs },
        { "value", w.ids.value },
        { "valueType", w.ids.valueType },
        { "variable", w.ids.variable },
        { "variables", w.ids.variables },
        { "width", w.ids.width },
        { "up", w.directions.up },
        { "down", w.directions.down },
        { "left", w.directions.left },
        { "right", w.directions.right },
        { "x", w.coordinates.x },
        { "y", w.coordinates.y }
    };
    for (auto& reservedString : reservedStrings) {
        auto& str = reservedString.str;
        m_strings.emplace(std::piecewise_construct,
                          std::forward_as_tuple(reservedString.str),
                          std::forward_as_tuple(w, reservedString.list, reservedString.str));

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
                                              std::forward_as_tuple(w, str));
        String& newString = it.first->second;
        newString.label(str);
        return newString;
    }
}

List& Pools::Strings::getCharList(const std::string& str)
{
    auto& string = get(str);
    auto& ret = static_cast<List&>(string[string.w.ids.value]);

    return ret;
}

Pools::Pools(World& w) :
    chars(w),
    digits(w),
    numbers(w),
    strings(w)
{
}

Ast::Cell& World::_(CellI& cell)
{
    return ast.cell(cell);
}

Ast::Cell& World::_(const std::string& nameStr)
{
    return ast.cell(name(nameStr));
}

Ast::StructName& World::__type__(const std::string& nameStr)
{
    return ast.structName(nameStr);
}

World::World(std::function<void()> loggerLevelInit) :
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
    m_stdCompiler(std::make_unique<Compiler>(*this)),
    m_stdLib(std::make_unique<StdLib>(*this, globalScope)),
    m_arcLib(std::make_unique<ArcLib>(*this, globalScope))
{
    Compiler& compiler = *m_stdCompiler;
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
    compiler.registerBuiltInStruct("std::ast::Match", std.ast.Match);
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
    compiler.registerBuiltInStruct("std::ast::TypeAlias", std.ast.TypeAlias);
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
    compiler.registerBuiltInStruct("std::Library", std.Library);
    compiler.registerBuiltInStruct("std::StructReference", std.StructReference);
    compiler.registerBuiltInStruct("std::Directions", std.Directions);

    compiler.compile(stdLib());

    globalScope.m_toolFinder  = &compiler.getToolFinder();
    m_initPhase               = InitPhase::FullyConstructed;

    arcLib().include(stdLib());
    Compiler arcCompiler(*this);
    arcCompiler.compile(arcLib());

    if (IS_LOG_ENABLED) {
        TRACE(compiledSymbols, "All compiled symbols:");

        TRACE(compiledSymbols, "  structs:");
        Visitor::visitList(stdLib().structs()[ids.list], [this](CellI& kv, int, bool&) {
            TRACE(compiledSymbols, "    {}", kv[ids.key].label());
        });

        TRACE(compiledSymbols, "  functions:");
        Visitor::visitList(stdLib().functions()[ids.list], [this](CellI& kv, int, bool&) {
            TRACE(compiledSymbols, "    {} : {}", kv[ids.key].label(), kv[ids.value].label());
        });

        TRACE(compiledSymbols, "  variables:");
        Visitor::visitList(stdLib().variables()[ids.list], [this](CellI& kv, int, bool&) {
            TRACE(compiledSymbols, "    {} : {}", kv[ids.key].label(), kv[ids.value].label());
        });
    }

    // TODO hack: std.List is a "baseclass" now so we just set its method to std.List<Cell>
    // we need to implement the trait system properly
    auto& compiledListItemStruct = getStruct(templateId("std::ListItem", ids.valueType, std.Cell));
    auto& compiledListStruct     = getStruct(templateId("std::List", ids.valueType, std.Cell));
    auto& compiledStructStruct   = getStruct("std::Struct");
    std.ListItem.set("methods", compiledListItemStruct[ids.methods]);
    std.List.set("methods", compiledListStruct[ids.methods]);
    std.Struct.set("methods", compiledStructStruct[ids.methods]);
}

World::~World()
{
    m_initPhase = InitPhase::DestructBegin;
}

Library& World::arcLib()
{
    if (!m_arcLib) {
        throw "Get compiled stdlib before compilation is not possible";
    }
    return *m_arcLib;
}

Library& World::stdLib()
{
    if (!m_stdLib) {
        throw "Get compiled stdlib before compilation is not possible";
    }
    return *m_stdLib;
}

CellI& World::getStruct(const std::string& nameStr)
{
    return getStruct(name(nameStr));
}

CellI& World::getStruct(CellI& name)
{
    switch (m_initPhase) {
    case InitPhase::Init:
        throw "Get struct before compilation is not possible";
    case InitPhase::Compiling:
        throw "Get struct during compilation is not possible";
    case InitPhase::FullyConstructed:
        return stdLib().getStruct(name);
    case InitPhase::DestructBegin:
        return ids.emptyObject;
    }
    throw "Unhandled state!";
}

CellI& World::getVariable(const std::string& nameStr)
{
    return getVariable(name(nameStr));
}

CellI& World::getVariable(CellI& name)
{
    switch (m_initPhase) {
    case InitPhase::Init:
        throw "Get variable before compilation is not possible";
    case InitPhase::Compiling:
        throw "Get variable during compilation is not possible";
    case InitPhase::FullyConstructed:
        return stdLib().getVariable(name);
    case InitPhase::DestructBegin:
        return ids.emptyObject;
    }
    throw "Unhandled state!";
}

CellI& World::name(const std::string& str)
{
    return pools.strings.getCharList(str);
}

CellI& World::ListOf(CellI& valueType)
{
    switch (m_initPhase) {
    case InitPhase::Init: {
        auto& ret = tt_("std::List", "valueType", _(valueType));
        m_stdCompiler->reigisterStructBeforeCompilation(ret);
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

CellI& World::MapOf(CellI& keyType, CellI& valueType)
{
    switch (m_initPhase) {
    case InitPhase::Init: {
        auto& ret = tt_("std::Map", "keyType", _(keyType), "valueType", _(valueType));
        m_stdCompiler->reigisterStructBeforeCompilation(ret);

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

CellI& World::toCellBool(bool value)
{
    return value ? boolean.true_ : boolean.false_;
}

World::InitPhase World::initPhase()
{
    return m_initPhase;
}

World::Logger::Logger(std::function<void()> loggerLevelInit)
{
    registerLogger("cells");
    registerLogger("compileStruct");
    registerLogger("symbolResolver");
    registerLogger("compiledSymbols");
    registerLogger("toolFinder");
    loggerLevelInit();
}

World::Logger::~Logger()
{
    for (const auto& name : m_loggerNames) {
        spdlog::drop(name);
    }
}

void World::Logger::registerLogger(const std::string& name)
{
    // handle if multiple World instance is created
    auto logger = spdlog::get(name);
    if (logger) {
        return;
    }
    createLogger(name);
    m_loggerNames.push_back(name);
}

std::shared_ptr<spdlog::logger> World::Logger::createLogger(const std::string& name)
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
