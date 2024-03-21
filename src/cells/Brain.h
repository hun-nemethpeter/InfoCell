#pragma once
#include <format>
#include "Cells.h"

namespace synth {
namespace cells {
namespace brain {

class ID
{
    brain::Brain& kb;

public:
    ID(brain::Brain& kb);

    Object add;
    Object addMembership;
    Object addMethod;
    Object addPixel;
    Object addSlot;
    Object addSlots;
    Object addSubType;
    Object argument;
    Object ast;
    Object asts;
    Object branch;
    Object cell;
    Object checkPixel;
    Object checkPixels;
    Object children;
    Object code;
    Object color;
    Object condition;
    Object constructor;
    Object constructorWithIndexType;
    Object constructorWithRecursiveType;
    Object constructorWithSelfType;
    Object container;
    Object contains;
    Object continue_;
    Object current;
    Object data;
    Object destructor;
    Object direction;
    Object else_;
    Object empty;
    Object emptyObject;
    Object erase;
    Object eval;
    Object first;
    Object firstPixel;
    Object functions;
    Object functionTs;
    Object getValue;
    Object global;
    Object has;
    Object hasKey;
    Object hasPixel;
    Object hasSlot;
    Object height;
    Object id;
    Object incompleteStructTypes;
    Object index;
    Object indexType;
    Object input;
    Object inputPixels;
    Object insert;
    Object item;
    Object itemType;
    Object key;
    Object keyType;
    Object label;
    Object last;
    Object lhs;
    Object list;
    Object listItem;
    Object listType;
    Object localVars;
    Object memberOf;
    Object members;
    Object method;
    Object methods;
    Object name;
    Object next;
    Object objectType;
    Object op;
    Object ops;
    Object output;
    Object parameter;
    Object parameters;
    Object parent;
    Object picture;
    Object pixel;
    Object pixels;
    Object previous;
    Object process;
    Object processAdjacentPixel;
    Object processInputPixels;
    Object processPixel;
    Object recursiveType;
    Object remove;
    Object removeSlot;
    Object result;
    Object returnType;
    Object returnValue;
    Object rhs;
    Object role;
    Object rootNode;
    Object scope;
    Object scopes;
    Object self;
    Object shape;
    Object shapeId;
    Object shapes;
    Object sharedObject;
    Object size;
    Object slot;
    Object slotRole;
    Object slots;
    Object slotType;
    Object sortPixels;
    Object stack;
    Object statement;
    Object static_;
    Object status;
    Object stop;
    Object structs;
    Object structTInstances;
    Object structTs;
    Object subTypes;
    Object template_;
    Object templateParams;
    Object then;
    Object type;
    Object value;
    Object width;
};

namespace type {

class Op
{
public:
    Op(brain::Brain& kb);

    Object Add;
    Object And;
    Object Base;
    Object Block;
    Object ConstVar;
    Object Delete;
    Object Divide;
    Object Do;
    Object Equal;
    Object Erase;
    Object EvalVar;
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
    Ast(brain::Brain& kb);

protected:
    brain::Brain& kb;

public:
    Object Add;
    Object And;
    Object Base;
    Object Block;
    Object Call;
    Object Cell;
    Object Delete;
    Object DependentType;
    Object Divide;
    Object Do;
    Object Equal;
    Object Erase;
    Object Function;
    Object FunctionT;
    Object Get;
    Object GreaterThan;
    Object GreaterThanOrEqual;
    Object Has;
    Object If;
    Object LessThan;
    Object LessThanOrEqual;
    Object Member;
    Object Missing;
    Object Multiply;
    Object New;
    Object Not;
    Object NotEqual;
    Object NotSame;
    Object Or;
    Object Parameter;
    Object Return;
    Object Same;
    Object Scope;
    Object Self;
    Object SelfFn;
    Object Set;
    Object Slot;
    Object StaticCall;
    Object Struct;
    Object StructT;
    Object Subtract;
    Object TemplateParam;
    Object Var;
    Object While;
};

class Arc
{
public:
    Arc(brain::Brain& kb);

protected:
    brain::Brain& kb;

public:
    Object Demonstration;
    Object Task;
    Object Shape;
    Object Shaper;
};

} // namespace type

class Types
{
public:
    Types(brain::Brain& kb);

    cells::CellI& slot(cells::CellI& role, cells::CellI& type);
    Object& ListOf(CellI& type);
    Object& MapOf(CellI& keyType, CellI& valueType);
    Object& SetOf(CellI& valueType);

protected:
    brain::Brain& kb;
    std::map<CellI*, Object> m_listTypes;
    std::map<CellI*, std::map<CellI*, Object>> m_mapTypes;
    std::map<CellI*, Object> m_setTypes;
    friend class TypeInit;

public:
    Object Type_;
    Object Template;
    Object Struct;
    Object Enum;
    Object Cell;
    Object Slot;
    Object Container;
    Object Iterator;
    Object List;
    Object ListOfSlot;
    Object ListItem;
    Object Map;
    Object MapCellToSlot;
    Object MapCellToType;
    Object MapCellToAstFunction;
    Object MapCellToOpFunction;
    Object MapCellToOpVar;
    Object MapCellToOpBase;
    Object MapTypeToType;
    Object Index;
    Object TrieMap;
    Object TrieMapNode;
    Object Set;
    Object Boolean;
    Object Char;
    Object Digit;
    Object Number;
    Object String;
    Object Color;
    Object Pixel;
    Object Picture;
    Object Stack;
    Object StackFrame;
    Object Directions;
    Object Shape;

    type::Op op;
    type::Ast ast;
    type::Arc arc;
};

template <typename T>
class NewT
{
public:
    template <typename... Args>
    static T& New(Args&&... args)
    {
        return *new T(std::forward<Args>(args)...);
    }
};

class Ast
{
public:
    class Base : public Object
    {
        Base(const Base&) = delete;
    public:
        Base(brain::Brain& kb, CellI& classCell, const std::string& label = "");
    };
    template <typename T>
    class BaseT : public Base,
                  public NewT<T>
    {
    public:
        BaseT<T>(brain::Brain& kb, CellI& classCell, const std::string& label = "") :
            Base(kb, classCell, label)
        {
        }
    };
    class Get;
    class Cell : public BaseT<Cell>
    {
    public:
        Cell(brain::Brain& kb, CellI& value);
        Get& operator/(Base& role);
    };
    class Self : public BaseT<Self>
    {
    public:
        Self(brain::Brain& kb);
    };
    class SelfFn : public BaseT<SelfFn>
    {
    public:
        SelfFn(brain::Brain& kb);
    };
    class Return : public BaseT<Return>
    {
    public:
        Return(brain::Brain& kb);
        Return(brain::Brain& kb, CellI& value);
    };
    class Parameter : public BaseT<Parameter>
    {
    public:
        Parameter(brain::Brain& kb, CellI& role);
        Get& operator/(Base& role);
    };
    class Slot : public BaseT<Slot>
    {
    public:
        Slot(brain::Brain& kb, CellI& role, CellI& value);
    };
    class Call : public BaseT<Call>
    {
    public:
        Call(brain::Brain& kb, CellI& cell, CellI& method);
        Call(brain::Brain& kb, CellI& cell, CellI& method, Slot& slot1);
        Call(brain::Brain& kb, CellI& cell, CellI& method, Slot& slot1, Slot& slot2);
        Call(brain::Brain& kb, CellI& cell, CellI& method, Slot& slot1, Slot& slot2, Slot& slot3);
        Call(brain::Brain& kb, CellI& cell, CellI& method, Slot& slot1, Slot& slot2, Slot& slot3, Slot& slot4);
    };

    class StaticCall : public BaseT<StaticCall>
    {
    public:
        StaticCall(brain::Brain& kb, CellI& cell, CellI& method);
        StaticCall(brain::Brain& kb, CellI& cell, CellI& method, Slot& slot1);
        StaticCall(brain::Brain& kb, CellI& cell, CellI& method, Slot& slot1, Slot& slot2);
        StaticCall(brain::Brain& kb, CellI& cell, CellI& method, Slot& slot1, Slot& slot2, Slot& slot3);
        StaticCall(brain::Brain& kb, CellI& cell, CellI& method, Slot& slot1, Slot& slot2, Slot& slot3, Slot& slot4);
    };

    class Block : public BaseT<Block>
    {
    public:
        Block(brain::Brain& kb, List& list);
    };

    class Function;
    class FunctionT;
    class Struct;
    class StructT;
    class Scope : public BaseT<Scope>
    {
    public:
        Scope(brain::Brain& kb, CellI& id, const std::string& label = "ast.scope");
        Scope& addScope(CellI& id, const std::string& label);
        Function& addFunction(CellI& id, const std::string& label);
        FunctionT& addFunctionT(CellI& id, const std::string& label);
        Struct& addStruct(CellI& id, const std::string& label);
        void addStructTInstance(Struct& astStruct);
        void addIncompleteStruct(Struct& astStruct);
        Struct& addIncompleteStruct(List& id);
        StructT& addStructT(CellI& id, const std::string& label);

        void implicitInstantiation();
        CellI& compile();

        template <typename... Args>
        Struct& instantiateStructT(CellI& id, Args&&... args);

        Struct& instantiateIncompleteStructT(CellI& id, List& parameters);

        Map& scopes();
        Map& functions();
        Map& functionTs();
        Map& structs();
        Map& structTs();
        TrieMap& structTInstances();
        TrieMap& incompleteStructTypes();
    };

    class StructBase : public Base
    {
    public:
        StructBase(brain::Brain& kb, CellI& astType, CellI& cell, const std::string& label);

        Function& addMethod(CellI& id, const std::string& label);

        void members(Slot& param);
        template <typename... Args>
        void members(Slot& param, Args&&... args)
        {
            members(param);
            members(std::forward<Args>(args)...);
        }

        void subTypes(Slot& param);
        template <typename... Args>
        void subTypes(Slot& param, Args&&... args)
        {
            subTypes(param);
            subTypes(std::forward<Args>(args)...);
        }

        void memberOf(CellI& type);
        template <typename... Args>
        void memberOf(CellI& type, Args&&... args)
        {
            memberOf(type);
            memberOf(std::forward<Args>(args)...);
        }

        CellI& id();

    protected:
        Map& methods();
        List& members();
        List& subTypes();
        List& memberOf();
    };

    class Struct : public StructBase,
                   public NewT<Struct>
    {
    public:
        Struct(brain::Brain& kb, CellI& id, const std::string& label = "ast.struct");
        void implicitInstantiation();
    };

    class StructT : public StructBase,
                    public NewT<StructT>
    {
    public:
        using StructBase::kb;
        StructT(brain::Brain& kb, CellI& id, const std::string& label = "ast.structT");

        Type& idType();

        void templateParams(Slot& param);

        template <typename... Args>
        void templateParams(Slot& param, Args&&... args)
        {
            templateParams(param);
            templateParams(std::forward<Args>(args)...);
        }

        Struct& declareType(List& parameters);

        template <typename... Args>
        Struct& instantiate(Scope& scope, Args&&... args);
        Struct& instantiate(Scope& scope, List& parameters);

    protected:
        Struct& instantiateWith(Scope& scope, List& slotList);
        CellI& instantiateTemplateParam(CellI& param, CellI& selfType, Map& inputParameters);
        Base& instantiateAst(CellI& ast, CellI& selfType, Map& inputParameters);
        Map& templateParams();
    };

    class Function : public BaseT<Function>
    {
    public:
        Function(brain::Brain& kb, CellI& name, const std::string& label = "ast.function");
        Function(brain::Brain& kb, CellI& objType, CellI& name, const std::string& label = "ast.function");

        void parameters(Slot& param);

        template <typename... Args>
        void parameters(Slot& param, Args&&... args)
        {
            parameters(param);
            parameters(std::forward<Args>(args)...);
        }
        void returnType(CellI& type);

        template <typename... Args>
        void code(Args&&... args);

        void implicitInstantiation();
        CellI& compile();
        CellI& compile(CellI& type);

    protected:
        void addBlock(Block& block);
        CellI& compileImpl(CellI* type);
        void compileParams(cells::Object& function, cells::Map& subTypesMap, CellI* type);
        void implicitInstantiationInAst(CellI& ast);
        CellI& compileAst(CellI& ast, cells::Object& function, CellI* type);
        List& parameters();
        CellI& returnType();
        Base& code();
    };

    class FunctionT : public BaseT<FunctionT>
    {
    public:
        FunctionT(brain::Brain& kb, CellI& name, const std::string& label = "ast.functionT");

        void templateParams(Slot& param);

        template <typename... Args>
        void templateParams(Slot& param, Args&&... args)
        {
            templateParams(param);
            templateParams(std::forward<Args>(args)...);
        }

        void parameters(Slot& param);

        template <typename... Args>
        void parameters(Slot& param, Args&&... args)
        {
            parameters(param);
            parameters(std::forward<Args>(args)...);
        }
        void returnType(CellI& type);

        template <typename... Args>
        void code(Args&&... args);

        CellI& instantiate();

    protected:
        void addBlock(Block& block);
        List& parameters();
        CellI& returnType();
        Base& code();
    };

    class Delete : public BaseT<Delete>
    {
    public:
        Delete(brain::Brain& kb, Base& cell);
    };
    class Set : public BaseT<Set>
    {
    public:
        Set(brain::Brain& kb, Base& cell, Base& role, Base& value);
    };
    class Erase : public BaseT<Erase>
    {
    public:
        Erase(brain::Brain& kb, Base& cell, Base& role);
    };
    class If : public BaseT<If>
    {
    public:
        If(brain::Brain& kb, Base& condition, Base& thenBranch);
        If(brain::Brain& kb, Base& condition, Base& thenBranch, Base& elseBranch);
    };
    class Do : public BaseT<Do>
    {
    public:
        Do(brain::Brain& kb, Base& statement, Base& condition);
    };
    class While : public BaseT<While>
    {
    public:
        While(brain::Brain& kb, Base& condition, Base& statement);
    };
    class Var : public BaseT<Var>
    {
    public:
        Var(brain::Brain& kb, CellI& role);
        Set& operator=(Base& value);
        Get& operator*();
    };
    class Has;
    class Missing;
    class Member : public BaseT<Member>
    {
    public:
        Member(const Member&) = delete;
        Member(brain::Brain& kb, CellI& role);
        Set& operator=(Base& value);
        Get& operator/(Base& role);
        Has& exist();
        Missing& missing();
        Call& call(CellI& method);
        Call& call(CellI& method, Slot& slot1);
        Call& call(CellI& method, Slot& slot1, Slot& slot2);
        Call& call(CellI& method, Slot& slot1, Slot& slot2, Slot& slot3);
        Call& call(CellI& method, Slot& slot1, Slot& slot2, Slot& slot3, Slot& slot4);
    };
    class DependentType : public BaseT<DependentType>
    {
    public:
        DependentType(const DependentType&) = delete;
        DependentType(brain::Brain& kb, CellI& id, CellI& type);
    };
    class TemplateParam : public BaseT<TemplateParam>
    {
    public:
        TemplateParam(const TemplateParam&) = delete;
        TemplateParam(brain::Brain& kb, CellI& role);
    };
    class New : public BaseT<New>
    {
    public:
        New(brain::Brain& kb, Base& objectType);
        New(brain::Brain& kb, Base& objectType, Base& constructor);
        New(brain::Brain& kb, Base& objectType, Base& constructor, Slot& slot1);
        New(brain::Brain& kb, Base& objectType, Base& constructor, Slot& slot1, Slot& slot2);
        New(brain::Brain& kb, Base& objectType, Base& constructor, Slot& slot1, Slot& slot2, Slot& slot3);
        New(brain::Brain& kb, Base& objectType, Base& constructor, Slot& slot1, Slot& slot2, Slot& slot3, Slot& slot4);
    };
    class Same : public BaseT<Same>
    {
    public:
        Same(brain::Brain& kb, Base& lhs, Base& rhs);
    };
    class NotSame : public BaseT<NotSame>
    {
    public:
        NotSame(brain::Brain& kb, Base& lhs, Base& rhs);
    };
    class Equal : public BaseT<Equal>
    {
    public:
        Equal(brain::Brain& kb, Base& lhs, Base& rhs);
    };
    class NotEqual : public BaseT<NotEqual>
    {
    public:
        NotEqual(brain::Brain& kb, Base& lhs, Base& rhs);
    };
    class Has : public BaseT<Has>
    {
    public:
        Has(brain::Brain& kb, Base& cell, Base& role);
    };
    class Missing : public BaseT<Missing>
    {
    public:
        Missing(brain::Brain& kb, Base& cell, Base& role);
    };
    class Get : public BaseT<Get>
    {
    public:
        Get(brain::Brain& kb, Base& cell, Base& role);
        Get& operator/(Base& role);
    };
    class And : public BaseT<And>
    {
    public:
        And(brain::Brain& kb, Base& lhs, Base& rhs);
    };
    class Or : public BaseT<Or>
    {
    public:
        Or(brain::Brain& kb, Base& lhs, Base& rhs);
    };
    class Not : public BaseT<Not>
    {
    public:
        Not(brain::Brain& kb, Base& input);
    };
    class Add : public BaseT<Add>
    {
    public:
        Add(brain::Brain& kb, Base& lhs, Base& rhs);
    };
    class Subtract : public BaseT<Subtract>
    {
    public:
        Subtract(brain::Brain& kb, Base& lhs, Base& rhs);
    };
    class Multiply : public BaseT<Multiply>
    {
    public:
        Multiply(brain::Brain& kb, Base& lhs, Base& rhs);
    };
    class Divide : public BaseT<Divide>
    {
    public:
        Divide(brain::Brain& kb, Base& lhs, Base& rhs);
    };
    class LessThan : public BaseT<LessThan>
    {
    public:
        LessThan(brain::Brain& kb, Base& lhs, Base& rhs);
    };
    class LessThanOrEqual : public BaseT<LessThanOrEqual>
    {
    public:
        LessThanOrEqual(brain::Brain& kb, Base& lhs, Base& rhs);
    };
    class GreaterThan : public BaseT<GreaterThan>
    {
    public:
        GreaterThan(brain::Brain& kb, Base& lhs, Base& rhs);
    };
    class GreaterThanOrEqual : public BaseT<GreaterThanOrEqual>
    {
    public:
        GreaterThanOrEqual(brain::Brain& kb, Base& lhs, Base& rhs);
    };

    Ast(brain::Brain& kb);

    Cell& cell(CellI& value);
    Self& self();
    SelfFn& selfFn();
    Return& return_();
    Return& return_(Base& value);
    Parameter& parameter(CellI& role);
    Slot& slot(CellI& role, CellI& type);
    Call& call(CellI& cell, CellI& method);
    Call& call(CellI& cell, CellI& method, Slot& slot1);
    Call& call(CellI& cell, CellI& method, Slot& slot1, Slot& slot2);
    Call& call(CellI& cell, CellI& method, Slot& slot1, Slot& slot2, Slot& slot3);
    Call& call(CellI& cell, CellI& method, Slot& slot1, Slot& slot2, Slot& slot3, Slot& slot4);

    StaticCall& scall(CellI& cell, CellI& method);
    StaticCall& scall(CellI& cell, CellI& method, Slot& slot1);
    StaticCall& scall(CellI& cell, CellI& method, Slot& slot1, Slot& slot2);
    StaticCall& scall(CellI& cell, CellI& method, Slot& slot1, Slot& slot2, Slot& slot3);
    StaticCall& scall(CellI& cell, CellI& method, Slot& slot1, Slot& slot2, Slot& slot3, Slot& slot4);

    template <typename... Args>
    Block& block(Args&&... args);
    Function& function(List& inputs, Block& asts);
    Function& function(List& inputs, Block& asts, List& outputs);
    Delete& delete_(Base& cell);
    Set& set(Base& cell, Base& role, Base& value);
    Erase& erase(Base& cell, Base& role);
    If& if_(Base& condition, Base& thenBranch);
    If& if_(Base& condition, Base& thenBranch, Base& elseBranch);
    Do& do_(Base& condition, Base& statement);
    While& while_(Base& condition, Base& statement);
    Var& var(CellI& role);
    Member& member(CellI& role);
    DependentType& dependentType(CellI& role, CellI& type);
    DependentType& dependentType(CellI& role, CellI& type1, CellI& type2);
    TemplateParam& templateParam(CellI& role);
    New& new_(Base& objectType);
    New& new_(Base& objectType, Base& constructor);
    New& new_(Base& objectType, Base& constructor, Slot& slot1);
    New& new_(Base& objectType, Base& constructor, Slot& slot1, Slot& slot2);
    New& new_(Base& objectType, Base& constructor, Slot& slot1, Slot& slot2, Slot& slot3);
    New& new_(Base& objectType, Base& constructor, Slot& slot1, Slot& slot2, Slot& slot3, Slot& slot4);
    Same& same(Base& lhs, Base& rhs);
    NotSame& notSame(Base& lhs, Base& rhs);
    Equal& equal(Base& lhs, Base& rhs);
    NotEqual& notEqual(Base& lhs, Base& rhs);
    Has& has(Base& cell, Base& role);
    Missing& missing(Base& cell, Base& role);
    Get& get(Base& cell, Base& role);
    And& and_(Base& lhs, Base& rhs);
    Or& or_(Base& lhs, Base& rhs);
    Not& not_(Base& input);
    Add& add(Base& lhs, Base& rhs);
    Subtract& subtract(Base& lhs, Base& rhs);
    Multiply& multiply(Base& lhs, Base& rhs);
    Divide& divide(Base& lhs, Base& rhs);
    LessThan& lessThan(Base& lhs, Base& rhs);
    LessThanOrEqual& lessThanOrEqual(Base& lhs, Base& rhs);
    GreaterThan& greaterThan(Base& lhs, Base& rhs);
    GreaterThanOrEqual& greaterThanOrEqual(Base& lhs, Base& rhs);

protected:
    brain::Brain& kb;
};

class Directions
{
public:
    Directions(brain::Brain& kb);
    Object up;
    Object down;
    Object left;
    Object right;
};

class Coordinates
{
public:
    Coordinates(brain::Brain& kb);
    Object x;
    Object y;
};

class Colors
{
public:
    Colors(brain::Brain& kb);
    Object red;
    Object green;
    Object blue;
};

class Boolean
{
public:
    Boolean(brain::Brain& kb);
    Object true_;
    Object false_;
};

class Numbers
{
public:
    Numbers(brain::Brain& kb);
    Map sign;
    Object positive;
    Object negative;
};

namespace pools {

class Chars
{
public:
    Chars(brain::Brain& kb);
    Object& get(char32_t utf32Char);

protected:
    void registerUnicodeBlock(char32_t from, char32_t to);
    std::map<char32_t, Object> s_characters;
    brain::Brain& kb;
};

class Digits
{
public:
    Digits(brain::Brain& kb);
    Object& operator[](int digit);

protected:
    std::vector<Object> m_digits;
};

class Numbers
{
public:
    Numbers(brain::Brain& kb);
    Number& get(int number);

protected:
    std::map<int, Number> m_numbers;
    brain::Brain& m_kb;
};

} // namespace pools

class Pools
{
public:
    Pools(brain::Brain& kb);
    pools::Chars chars;
    pools::Digits digits;
    pools::Numbers numbers;
};

class Arc
{
public:
    Arc(brain::Brain& kb);

    Object Demonstration;
    Object Task;
    Object examples;
};

class Test
{
public:
    Test(brain::Brain& kb);

    Object factorial;
};

class Brain
{
public:
    enum class InitPhase
    {
        Init,
        SlotTypeInitialzed,
        FullyConstructed,
        DestructBegin
    };

protected:
    InitPhase m_initPhase = InitPhase::Init;
    friend class Types;

public:
    Brain();
    ~Brain();
    ID id;
    Types type;
    Pools pools;
    Ast ast;
    Directions directions;
    Coordinates coordinates;
    Colors colors;
    Boolean boolean;
    Numbers numbers;
    Arc arc;
    Test test;

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

    CellI& toKbBool(bool value);

    template <typename... Args>
    List& list(CellI& value, Args&&... args)
    {
        List& ret = *new List(*this, value.type());
        ret.add(value);
        if constexpr (sizeof...(Args) > 0) {
            ret.add(std::forward<Args>(args)...);
        }

        return ret;
    }

    template <typename... Args>
    Map& map(CellI& key, CellI& value, Args&&... args)
    {
        Map& ret = *new Map(*this, key.type(), value.type(), std::format("Map<{}, {}>(...)", key.type().label(), value.type().label()));
        if constexpr (sizeof...(Args) > 0) {
            ret.add(std::forward<Args>(args)...);
        }

        return ret;
    }

    template <typename... Args>
    Set& set(CellI& value, Args&&... args)
    {
        Set& ret = *new Set(*this, value.type(), std::format("Map<{}, {}>(...)", value.type().label()));
        if constexpr (sizeof...(Args) > 0) {
            ret.add(std::forward<Args>(args)...);
        }

        return ret;
    }

    void addSlots(Map&)
    {
        // Do nothing
    }

    template <typename... Args>
    void addSlots(Map& map, CellI& value, Args&&... args)
    {
        map.add(value[id.slotRole], value);
        addSlots(map, std::forward<Args>(args)...);
    }

    template <typename... Args>
    Map& slots(CellI& value, Args&&... args)
    {
        Map& ret = *new Map(*this, type.Cell, type.Slot, "Map<Cell, Slot>(...)");
        addSlots(ret, value, std::forward<Args>(args)...);

        return ret;
    }

    void addMethods(Object&, Map&, Map&)
    {
        // Do nothing
    }

    template <typename... Args>
    void addMethods(Object& class_, Map& asts, Map& methods, CellI& methodId, Ast::Function& method, Args&&... args)
    {
        asts.add(methodId, method);
        methods.add(methodId, method.compile(class_));
        addMethods(class_, asts, methods, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void registerMethods(Object& class_, CellI& methodId, Ast::Function& method, Args&&... args)
    {
        Map& asts = *new Map(*this, type.Cell, type.ast.Function, "Map<Cell, Type::Ast::Function>(...)");
        Map& methods = *new Map(*this, type.Cell, type.op.Function, "Map<Cell, Type::Op::Function>(...)");
        addMethods(class_, asts, methods, methodId, method, std::forward<Args>(args)...);
        class_.set(id.asts, asts);
        class_.set(id.methods, methods);
    }

    InitPhase initPhase();
};

template <typename... Args>
Ast::Block& Ast::block(Args&&... args)
{
    return *new Block(kb, kb.list(std::forward<Args>(args)...));
}

template <typename... Args>
void Ast::Function::code(Args&&... args)
{
    addBlock(*new Block(kb, kb.list(std::forward<Args>(args)...)));
}

template <typename... Args>
Ast::Struct& Ast::Scope::instantiateStructT(CellI& id, Args&&... args)
{
    if (!structTs().hasKey(id)) {
        throw "No such template!";
    }
    auto& structT = static_cast<Ast::StructT&>(structTs().getValue(id));
    Ast::Struct& instance = structT.instantiate(*this, std::forward<Args>(args)...);
    return instance;
}

template <typename... Args>
Ast::Struct& Ast::StructT::instantiate(Ast::Scope& scope, Args&&... args)
{
    return instantiateWith(scope, kb.list(std::forward<Args>(args)...));
}

} // namespace brain
} // namespace cells
} // namespace synth
