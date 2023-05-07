#pragma once
#include <format>
#include "Cells.h"

namespace synth {
namespace cells {
namespace brain {

class Sequence
{
public:
    Sequence(brain::Brain& kb);
    Object first;
    Object last;
    Object previous;
    Object next;
    Object current;
    Object add;
    Object empty;
};

class Dimensions
{
public:
    Dimensions(brain::Brain& kb);
    Object width;
    Object height;
    Object size;
};

class Coding
{
    brain::Brain& kb;
public:
    Coding(brain::Brain& kb);
    cells::CellI& slot(cells::CellI& role, cells::CellI& type);

    Object argument;
    Object ast;
    Object asts;
    Object branch;
    Object cell;
    Object condition;
    Object constructor;
    Object container;
    Object continue_;
    Object destructor;
    Object else_;
    Object emptyObject;
    Object eval;
    Object index;
    Object indexType;
    Object input;
    Object item;
    Object key;
    Object keyType;
    Object label;
    Object lhs;
    Object list;
    Object localVars;
    Object memberOf;
    Object method;
    Object methods;
    Object objectType;
    Object op;
    Object ops;
    Object output;
    Object parameter;
    Object parameters;
    Object result;
    Object rhs;
    Object role;
    Object self;
    Object slotRole;
    Object slots;
    Object slotType;
    Object stack;
    Object statement;
    Object status;
    Object stop;
    Object subTypes;
    Object template_;
    Object then;
    Object type;
    Object value;
};

class Methods
{
    brain::Brain& kb;

public:
    Methods(brain::Brain& kb);

    Object addMembership;
    Object addMethod;
    Object addSlot;
    Object addSlots;
    Object addSubType;
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
    Object Divide;
    Object Do;
    Object Equal;
    Object Function;
    Object Get;
    Object GreaterThan;
    Object GreaterThanOrEqual;
    Object Has;
    Object If;
    Object Input;
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
    Object Output;
    Object Return;
    Object Same;
    Object Self;
    Object SelfFn;
    Object Set;
    Object Slot;
    Object StaticCall;
    Object Subtract;
    Object Var;
    Object While;
};

} // namespace type

class Types
{
public:
    Types(brain::Brain& kb);

    Object& ListOf(CellI& type);
    Object& MapOf(CellI& keyType, CellI& valueType);

protected:
    brain::Brain& kb;
    std::map<CellI*, Object> m_listTypes;
    std::map<CellI*, std::map<CellI*, Object>> m_mapTypes;
    friend class TypeInit;

public:
    Object Type_;
    Object Cell;
    Object Slot;
    Object Container;
    Object Iterator;
    Object List;
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

    type::Op op;
    type::Ast ast;
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
            Base(kb, classCell, label), kb(kb)
        {
        }

        brain::Brain& kb;
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
    class Input : public BaseT<Input>
    {
    public:
        Input(brain::Brain& kb, CellI& role);
        Get& operator/(Base& role);
    };
    class Output : public BaseT<Output>
    {
    public:
        Output(brain::Brain& kb, CellI& role);
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
    class Function : public BaseT<Function>
    {
    public:
        Function(brain::Brain& kb, const std::string& label = "Function");

        void addInputs(List& input);
        void addOutputs(List& output);
        void addBlock(Block& ast);
        void toMethod(Object& type);
        CellI& compile();
        CellI& compile(CellI& type);

    protected:
        CellI& compileImpl(CellI* type);
        void compileParams(cells::Object& function, CellI* type);
        CellI& compileAst(CellI& ast, cells::Object& function, CellI* type);
        List& inputs();
        List& outputs();
        Block& asts();

        List* m_inputs  = nullptr;
        List* m_outputs = nullptr;
        Block* m_asts   = nullptr;

        std::unique_ptr<Object> m_inputType;
        std::unique_ptr<Object> m_outputType;
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
    Input& input(CellI& role);
    Output& output(CellI& role);
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
    If& if_(Base& condition, Base& thenBranch);
    If& if_(Base& condition, Base& thenBranch, Base& elseBranch);
    Do& do_(Base& condition, Base& statement);
    While& while_(Base& condition, Base& statement);
    Var& var(CellI& role);
    Member& member(CellI& role);
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

class Visualization
{
public:
    Visualization(brain::Brain& kb);
    Object color;
    Object pixels;
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
    Sequence sequence;
    Dimensions dimensions;
    Coding coding;
    Methods methods;
    Types type;
    Pools pools;
    Ast ast;
    Directions directions;
    Coordinates coordinates;
    Colors colors;
    Boolean boolean;
    Visualization visualization;
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

    InitPhase initPhase();
};

template <typename... Args>
Ast::Block& Ast::block(Args&&... args)
{
    return *new Block(kb, kb.list(std::forward<Args>(args)...));
}

} // namespace brain
} // namespace cells
} // namespace synth
