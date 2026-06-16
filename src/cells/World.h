#pragma once
#include <fmt/core.h>
#include <functional>

#include "Ast.h"
#include "Cells.h"
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

class ID
{
    World& w;

public:
    ID(World& w);

    List __type__;
    List argument;
    List ast;
    List asts;
    List blue;
    List break_;
    List cell;
    List children;
    List color;
    List compiled;
    List condition;
    List constructor;
    List container;
    List continue_;
    List currentFn;
    List currentParam;
    List currentStruct;
    List data;
    List description;
    List destructor;
    List else_;
    List enum_;
    List emptyObject;
    List first;
    List functions;
    List globalScope;
    List green;
    List height;
    List id;
    List index;
    List input;
    List instances;
    List instructions;
    List item;
    List itemType;
    List key;
    List keyType;
    List last;
    List lastOp;
    List lhs;
    List list;
    List listType;
    List localVars;
    List memberOf;
    List members;
    List method;
    List methods;
    List name;
    List next;
    List objectType;
    List op;
    List ops;
    List output;
    List parameters;
    List parent;
    List pixels;
    List pixelsMap;
    List pop;
    List previous;
    List process;
    List push;
    List red;
    List resolvedScope;
    List result;
    List return_;
    List returnType;
    List rhs;
    List rootNode;
    List scope;
    List scopes;
    List self;
    List size;
    List slots;
    List stack;
    List state;
    List stateCondition;
    List stateElse;
    List stateLhs;
    List statement;
    List stateParam1;
    List stateParam2;
    List stateParam3;
    List stateParamEval;
    List stateParamInit;
    List stateRhs;
    List stateStackCall;
    List stateStatement;
    List stateThen;
    List static_;
    List status;
    List structs;
    List structType;
    List typeAliases;
    List tag;
    List templateId;
    List templateParams;
    List then;
    List throw_;
    List type;
    List unknownInstances;
    List unknownStructs;
    List value;
    List valueType;
    List variable;
    List variables;
    List width;
};

class Directions
{
public:
    Directions(World& w);
    List up;
    List down;
    List left;
    List right;
};

class Coordinates
{
public:
    Coordinates(World& w);
    List x;
    List y;
};

class Boolean
{
public:
    Boolean(World& w);
    Object true_;
    Object false_;
};

class Numbers
{
public:
    Numbers(World& w);
    Map sign;
    Object positive;
    Object negative;
};

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
        Strings(World& w);
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
    Pools pools;
    ID id;
    Ast::Scope globalScope;
    Std std;
    Ast ast;
    Directions directions;
    Coordinates coordinates;
    Boolean boolean;
    Numbers numbers;
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
    CellI& name(const std::string& str);
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