#include "World.h"
#include "Compiler.h"

#include "ArcLib.h"
#include "StdLib.h"

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include "util/Log.h"

namespace infocell {
namespace cells {

ID::Directions::Directions(World& w) :
    up(w, w.std.Char, "up"),
    down(w, w.std.Char, "down"),
    left(w, w.std.Char, "left"),
    right(w, w.std.Char, "right")
{
}

ID::Coordinates::Coordinates(World& w) :
    x(w, w.std.Char, "x"),
    y(w, w.std.Char, "y")
{
}

ID::ID(World& w) :
    w(w),
    directions(w),
    coordinates(w),
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
    m_ids = {
        { &__type__, "__type__" },
        { &argument, "argument" },
        { &ast, "ast" },
        { &asts, "asts" },
        { &blue, "blue" },
        { &break_, "break_" },
        { &cell, "cell" },
        { &children, "children" },
        { &color, "color" },
        { &compiled, "compiled" },
        { &condition, "condition" },
        { &constructor, "constructor" },
        { &container, "container" },
        { &continue_, "continue_" },
        { &currentFn, "currentFn" },
        { &currentParam, "currentParam" },
        { &currentStruct, "currentStruct" },
        { &data, "data" },
        { &description, "description" },
        { &destructor, "destructor" },
        { &else_, "else_" },
        { &enum_, "enum_" },
        { &emptyObject, "emptyObject" },
        { &first, "first" },
        { &functions, "functions" },
        { &globalScope, "globalScope" },
        { &green, "green" },
        { &height, "height" },
        { &id, "id" },
        { &index, "index" },
        { &input, "input" },
        { &instances, "instances" },
        { &instructions, "instructions" },
        { &item, "item" },
        { &itemType, "itemType" },
        { &key, "key" },
        { &keyType, "keyType" },
        { &last, "last" },
        { &lastOp, "lastOp" },
        { &lhs, "lhs" },
        { &list, "list" },
        { &listType, "listType" },
        { &localVars, "localVars" },
        { &memberOf, "memberOf" },
        { &members, "members" },
        { &method, "method" },
        { &methods, "methods" },
        { &name, "name" },
        { &next, "next" },
        { &objectType, "objectType" },
        { &op, "op" },
        { &ops, "ops" },
        { &output, "output" },
        { &parameters, "parameters" },
        { &parent, "parent" },
        { &pixels, "pixels" },
        { &pixelsMap, "pixelsMap" },
        { &pop, "pop" },
        { &previous, "previous" },
        { &process, "process" },
        { &push, "push" },
        { &red, "red" },
        { &resolvedScope, "resolvedScope" },
        { &result, "result" },
        { &return_, "return_" },
        { &returnType, "returnType" },
        { &rhs, "rhs" },
        { &rootNode, "rootNode" },
        { &scope, "scope" },
        { &scopes, "scopes" },
        { &self, "self" },
        { &size, "size" },
        { &slots, "slots" },
        { &stack, "stack" },
        { &state, "state" },
        { &stateCondition, "stateCondition" },
        { &stateElse, "stateElse" },
        { &stateLhs, "stateLhs" },
        { &statement, "statement" },
        { &stateParam1, "stateParam1" },
        { &stateParam2, "stateParam2" },
        { &stateParam3, "stateParam3" },
        { &stateParamEval, "stateParamEval" },
        { &stateParamInit, "stateParamInit" },
        { &stateRhs, "stateRhs" },
        { &stateStackCall, "stateStackCall" },
        { &stateStatement, "stateStatement" },
        { &stateThen, "stateThen" },
        { &static_, "static_" },
        { &status, "status" },
        { &structs, "structs" },
        { &structType, "structType" },
        { &typeAliases, "typeAliases" },
        { &tag, "tag" },
        { &templateId, "templateId" },
        { &templateParams, "templateParams" },
        { &then, "then" },
        { &throw_, "throw" },
        { &type, "type" },
        { &unknownInstances, "unknownInstances" },
        { &unknownStructs, "unknownStructs" },
        { &value, "value" },
        { &valueType, "valueType" },
        { &variable, "variable" },
        { &variables, "variables" },
        { &width, "width" },

        { &directions.up, "up" },
        { &directions.down, "down" },
        { &directions.left, "left" },
        { &directions.right, "right" },

        { &coordinates.x, "x" },
        { &coordinates.y, "y"  }
    };
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

// ============================================================================
Pools::Strings::Strings(World& w, const std::map<List*, const char*>& ids) :
    w(w)
{
    for (auto& listToStr : ids) {
        List& list = *listToStr.first;
        const char * str  = listToStr.second;
        m_strings.emplace(std::piecewise_construct,
                          std::forward_as_tuple(str),
                          std::forward_as_tuple(w, list, str));
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
    auto& ret = static_cast<List&>(string[string.w.id.value]);

    return ret;
}

Pools::Pools(World& w) :
    chars(w),
    digits(w),
    numbers(w),
    strings(w, w.id.m_ids)
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
    id(*this),
    pools(*this),
    globalScope(Ast::Scope(*this, "global")),
    std(*this),
    ast(*this),
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
    m_stdLib(std::make_unique<StdLib>(*this, globalScope, *m_stdCompiler)),
    m_arcLib(std::make_unique<ArcLib>(*this, globalScope))
{
    Compiler& compiler = *m_stdCompiler;
    compiler.compile(stdLib());

    globalScope.m_toolFinder  = &compiler.getToolFinder();
    m_initPhase               = InitPhase::FullyConstructed;

    arcLib().include(stdLib());
    Compiler arcCompiler(*this);
    arcCompiler.compile(arcLib());

    if (IS_LOG_ENABLED) {
        TRACE(compiledSymbols, "All compiled symbols:");

        TRACE(compiledSymbols, "  structs:");
        Visitor::visitList(stdLib().structs()[id.list], [this](CellI& kv, int, bool&) {
            TRACE(compiledSymbols, "    {}", kv[id.key].label());
        });

        TRACE(compiledSymbols, "  functions:");
        Visitor::visitList(stdLib().functions()[id.list], [this](CellI& kv, int, bool&) {
            TRACE(compiledSymbols, "    {} : {}", kv[id.key].label(), kv[id.value].label());
        });

        TRACE(compiledSymbols, "  variables:");
        Visitor::visitList(stdLib().variables()[id.list], [this](CellI& kv, int, bool&) {
            TRACE(compiledSymbols, "    {} : {}", kv[id.key].label(), kv[id.value].label());
        });
    }

    // TODO hack: std.List is a "baseclass" now so we just set its method to std.List<Cell>
    // we need to implement the trait system properly
    auto& compiledListItemStruct = getStruct(templateId("std::ListItem", id.valueType, std.Cell));
    auto& compiledListStruct     = getStruct(templateId("std::List", id.valueType, std.Cell));
    auto& compiledStructStruct   = getStruct("std::Struct");
    std.ListItem.set("methods", compiledListItemStruct[id.methods]);
    std.List.set("methods", compiledListStruct[id.methods]);
    std.Struct.set("methods", compiledStructStruct[id.methods]);
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
        return id.emptyObject;
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
        return id.emptyObject;
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
        return getStruct(templateId("std::List", id.valueType, valueType));
    }
    case InitPhase::DestructBegin:
        return id.emptyObject;
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
        return getStruct(templateId("std::Map", id.keyType, keyType, id.valueType, valueType));
    }
    case InitPhase::DestructBegin:
        return id.emptyObject;
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
