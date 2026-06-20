#pragma once
#include <fmt/core.h>
#include <functional>

#include "Ast.h"
#include "Cells.h"
#include "IDs.h"
#include "StdLib.h"
#include "ToolFinder.h"

namespace spdlog {
class logger;
}

namespace infocell {
namespace cells {

class Compiler;
class Library;
class ArcLib;
class StdLib;
class World;

class Pools
{
public:
    class Chars
    {
    public:
        Chars(World& w);
        Object& get(char32_t utf32Char);

    protected:
        void registerUnicodeBlock(char32_t from, char32_t to);
        std::map<char32_t, Object> m_characters;
        World& w;
    };

    class Digits
    {
    public:
        Digits(World& w);
        Object& operator[](int digit);

    protected:
        std::vector<Object> m_digits;
    };

    class Numbers
    {
    public:
        Numbers(World& w);
        Number& get(int number);

    protected:
        std::map<int, Number> m_numbers;
        World& w;
    };

    class Strings
    {
    public:
        Strings(World& w, const std::map<List*, const char*>& ids);
        String& get(const std::string& str);
        List& getCharList(const std::string& str);

    protected:
        std::map<std::string, String> m_strings;
        World& w;
    };

    Pools(World& w);

    Chars chars;
    Digits digits;
    Numbers numbers;
    Strings strings;
};

class World
{
public:
    class Logger
    {
    public:
        Logger(std::function<void()> loggerLevelInit);
        ~Logger();
        static std::shared_ptr<spdlog::logger> createLogger(const std::string& name);

    private:
        void registerLogger(const std::string& name);
        std::vector<std::string> m_loggerNames;
    };

    enum class InitPhase
    {
        Init,
        Compiling,
        FullyConstructed,
        DestructBegin
    };

protected:
    InitPhase m_initPhase = InitPhase::Init;
    friend class Std;

public:
    Ast::Cell& _(CellI& cell);
    Ast::Cell& _(const std::string& id);
    template <typename... Args>
    Ast::TemplatedType& tt_(const std::string& name, Args&&... args);
    Ast::StructName& __type__(const std::string& name);

public:
    World(std::function<void()> loggerLevelInit = []() {});
    ~World();

    Logger logger;
    ID id;
    Pools pools;
    Ast::Scope globalScope;
    Std std;
    Ast ast;
    cells::hybrid::ActivationPointer ap;

    CellI& _0_;
    CellI& _1_;
    CellI& _2_;
    CellI& _3_;
    CellI& _4_;
    CellI& _5_;
    CellI& _6_;
    CellI& _7_;
    CellI& _8_;
    CellI& _9_;

private:
    std::unique_ptr<Compiler> m_stdCompiler;
    std::unique_ptr<StdLib> m_stdLib;
    std::unique_ptr<ArcLib> m_arcLib;

public:
    Library& arcLib();
    Library& stdLib();

    CellI& getStruct(const std::string& nameStr);
    CellI& getStruct(CellI& name);
    CellI& getVariable(const std::string& nameStr);
    CellI& getVariable(CellI& name);
    List& name(const std::string& str);
    template <typename... Args>
    CellI& templateId(const std::string& str, Args&&... args);
    CellI& ListOf(CellI& type);
    CellI& MapOf(CellI& keyType, CellI& valueType);

    CellI& toCellBool(bool value);

    template <typename... Args>
    List& list(CellI& value, Args&&... args);

    InitPhase initPhase();
};

void splitNamespacedString(std::vector<std::string>& out, const std::string& input);

#pragma region World
template <typename... Args>
Ast::TemplatedType& World::tt_(const std::string& nameStr, Args&&... args)
{
    return ast.templatedType(nameStr, std::forward<Args>(args)...);
}

template <typename... Args>
CellI& World::templateId(const std::string& nameStr, Args&&... args)
{
    List& idCell = *new List(*this, std.Cell);
    for (const auto& character : nameStr) {
        idCell.add(pools.chars.get(character));
    }
    idCell.add(std::forward<Args>(args)...);

    return idCell;
}

template <typename... Args>
List& World::list(CellI& value, Args&&... args)
{
    List& ret = *new List(*this, value.w.std.Cell);
    ret.add(value);
    if constexpr (sizeof...(Args) > 0) {
        ret.add(std::forward<Args>(args)...);
    }

    return ret;
}

#pragma endregion

} // namespace cells
} // namespace infocell
#include "AstImpl.h"