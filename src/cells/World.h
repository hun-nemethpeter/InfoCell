#pragma once
#include <fmt/core.h>
#include <functional>

#include "Ast.h"
#include "Cells.h"
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

class Std
{
public:
    class Op
    {
    public:
        Op(World& w);

    protected:
        World& w;

    public:
        Object Activate;
        Object Add;
        Object And;
        Object Base;
        Object Block;
        Object Call;
        Object ConstVar;
        Object Delete;
        Object Divide;
        Object Do;
        Object Equal;
        Object Erase;
        Object Function;
        Object Get;
        Object GreaterThan;
        Object GreaterThanOrEqual;
        Object Has;
        Object If;
        Object LessThan;
        Object LessThanOrEqual;
        Object Missing;
        Object Multiply;
        Object New;
        Object Not;
        Object NotEqual;
        Object NotSame;
        Object Or;
        Object Return;
        Object Same;
        Object Set;
        Object Subtract;
        Object Var;
        Object While;
    };

    class Ast
    {
    public:
        Ast(World& w);

    protected:
        World& w;

    public:
        Object Add;
        Object And;
        Object Base;
        Object Block;
        Object Break;
        Object Call;
        Object Cell;
        Object Continue;
        Object Delete;
        Object Divide;
        Object Do;
        Object Enum;
        Object EnumValue;
        Object Equal;
        Object Erase;
        Object For;
        Object Function;
        Object FunctionT;
        Object Get;
        Object GreaterThan;
        Object GreaterThanOrEqual;
        Object Has;
        Object If;
        Object LessThan;
        Object LessThanOrEqual;
        Object Match;
        Object Member;
        Object Missing;
        Object Multiply;
        Object New;
        Object Not;
        Object NotEqual;
        Object NotSame;
        Object Or;
        Object Parameter;
        Object ResolvedType;
        Object Return;
        Object Same;
        Object Scope;
        Object Self;
        Object SelfFn;
        Object Set;
        Object Slot;
        Object StaticCall;
        Object Struct;
        Object StructName;
        Object StructT;
        Object Subtract;
        Object TypeAlias;
        Object TemplatedType;
        Object TemplateParam;
        Object Throw;
        Object Trait;
        Object TraitImpl;
        Object Try;
        Object TypedEnumValue;
        Object Var;
        Object While;
    };

    Std(World& w);

    cells::CellI& slot(const std::string& key, cells::CellI& type);
    cells::CellI& slot(cells::CellI& key, cells::CellI& type);
    cells::CellI& kvPair(cells::CellI& key, cells::CellI& value);

protected:
    World& w;

public:
    Object Boolean;
    Object Cell;
    Object Char;
    Object Color;
    Object Container;
    Object Digit;
    Object Directions;
    Object Enum;
    Object Grid;
    Object Index;
    Object KVPair;
    Object Library;
    Object List;
    Object ListItem;
    Object Map;
    Object Number;
    Object OpState;
    Object Pixel;
    Object Slot;
    Object Stack;
    Object StackFrame;
    Object String;
    Object Struct;
    Object StructReference;
    Object TrieMap;
    Object TrieMapNode;

    Op op;
    Ast ast;
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

    template <typename... Args>
    Map& map(CellI& key, CellI& value, Args&&... args);

    template <typename... Args>
    Set& set(CellI& value, Args&&... args);

    void addSlots(Map&)
    {
        // Do nothing
    }

    template <typename... Args>
    void addSlots(Map& map, CellI& value, Args&&... args);

    template <typename... Args>
    Map& slots(CellI& value, Args&&... args);

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

template <typename... Args>
Map& World::map(CellI& key, CellI& value, Args&&... args)
{
    Map& ret = *new Map(*this, key.__type__(), value.__type__(), fmt::format("Map<{}, {}>(...)", key.__type__().label(), value.__type__().label()));
    if constexpr (sizeof...(Args) > 0) {
        ret.add(std::forward<Args>(args)...);
    }

    return ret;
}

template <typename... Args>
Set& World::set(CellI& value, Args&&... args)
{
    Set& ret = *new Set(*this, value.__type__(), fmt::format("Map<{}, {}>(...)", value.__type__().label()));
    if constexpr (sizeof...(Args) > 0) {
        ret.add(std::forward<Args>(args)...);
    }

    return ret;
}

template <typename... Args>
void World::addSlots(Map& map, CellI& value, Args&&... args)
{
    map.add(value["key"], value);
    addSlots(map, std::forward<Args>(args)...);
}

template <typename... Args>
Map& World::slots(CellI& value, Args&&... args)
{
    Map& ret = *new Map(*this, std.Cell, std.Slot, "Map<Cell, Slot>(...)");
    addSlots(ret, value, std::forward<Args>(args)...);

    return ret;
}
#pragma endregion

} // namespace cells
} // namespace infocell
#include "AstImpl.h"