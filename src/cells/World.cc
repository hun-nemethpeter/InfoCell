#include "World.h"
#include "Compiler.h"

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include "util/Log.h"

namespace infocell {
namespace cells {

// ============================================================================
Pools::Chars::Chars(World& w) :
    w(w)
{
}

void Pools::Chars::init()
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
Pools::Digits::Digits(World& w) :
    w(w)
{
}

void Pools::Digits::init()
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
Pools::Strings::Strings(World& w) :
    w(w)
{
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

// ============================================================================
Pools::Pools(World& w) :
    chars(w),
    digits(w),
    numbers(w),
    strings(w)
{
}

void Pools::init()
{
    chars.init();
    digits.init();
}

// ============================================================================
World::World(std::function<void()> loggerLevelInit) :
    m_initPhase(InitPhase::Init),
    logger(loggerLevelInit),
    pools(*this),
    id(*this),
    globalScope(Ast::Scope(*this, "global")),
    std(*this),
    arc(*this),
    ast(*this),
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
    m_arcCompiler(std::make_unique<Compiler>(*this)),
    m_stdLib(std::make_unique<StdLib>(*this, globalScope, *m_stdCompiler)),
    m_arcLib(std::make_unique<ArcLib>(*this, globalScope, *m_stdCompiler))
{
    Compiler& compiler = *m_stdCompiler;
    compiler.compile(stdLib());

    globalScope.m_toolFinder  = &compiler.getToolFinder();
    m_initPhase               = InitPhase::FullyConstructed;

    Object::s_worldConstructedAndAlive = true;

    arcLib().include(stdLib());
    m_arcCompiler->compile(arcLib());

    if (IS_LOG_ENABLED) {
        TRACE(compiledSymbols, "All compiled symbols:");

        TRACE(compiledSymbols, "  structs:");
        for (CellI& kv : stdLib().structs()[id.list]) {
            TRACE(compiledSymbols, "    {}", kv[id.key].label());
        }

        TRACE(compiledSymbols, "  functions:");
        for (CellI& kv : stdLib().functions()[id.list]) {
            TRACE(compiledSymbols, "    {} : {}", kv[id.key].label(), kv[id.value].label());
        }

        TRACE(compiledSymbols, "  variables:");
        for (CellI& kv : stdLib().variables()[id.list]) {
            TRACE(compiledSymbols, "    {} : {}", kv[id.key].label(), kv[id.value].label());
        }
    }

    // TODO hack: std.List is a "baseclass" now so we just set its method to std.List<Cell>
    // we need to implement the trait system properly
    auto& compiledListNodeStruct = getStruct(templateId("std::ListNode", id.valueType, std.Cell));
    auto& compiledListStruct     = getStruct(templateId("std::List", id.valueType, std.Cell));
    auto& compiledStructStruct   = getStruct("std::Struct");
    std.ListNode.set("methods", compiledListNodeStruct[id.methods]);
    std.List.set("methods", compiledListStruct[id.methods]);
    std.Struct.set("methods", compiledStructStruct[id.methods]);
}

World::~World()
{
    m_initPhase = InitPhase::DestructBegin;
    Object::s_worldConstructedAndAlive = false;
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

Ast::Cell& World::_(CellI& cell)
{
    return ast.cell(cell);
}

Ast::Cell& World::_(const std::string& nameStr)
{
    return ast.cell(name(nameStr));
}

Ast::Cell& World::_(int number)
{
    return _(pools.numbers.get(number));
}

Ast::TypeName& World::__type__(const std::string& nameStr)
{
    return ast.typeName(nameStr);
}

String& World::name(const std::string& str)
{
    return pools.strings.get(str);
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
    return value ? true_ : false_;
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
    registerLogger("toolFinderLookup");
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
