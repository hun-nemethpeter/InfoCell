#pragma once
#include "Cells.h"

namespace synth {
namespace cells {

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

namespace brain {
namespace type {

class Template
{
public:
    Template(brain::Brain& kb);
    Type ParameterDecl;
    Type Slot;
    Type Descriptor;
    Type Cell;
    Type Parameter;
    Type TemplateOf;
    Type Self;
};

class Control
{
public:
    Control(brain::Brain& kb);

    Type Base;
    Type Block;
    Type Function;
    Type Delete;
    Type Set;
    Type If;
    Type Do;
    Type While;
    Type Expression;
    Type Input;
    Type New;
    Type Same;
    Type NotSame;
    Type Equal;
    Type NotEqual;
    Type Has;
    Type Get;
    Type And;
    Type Or;
    Type Not;
    Type Add;
    Type Subtract;
    Type Multiply;
    Type Divide;
    Type LessThan;
    Type GreaterThan;
};

class Ast
{
public:
    Ast(brain::Brain& kb);

protected:
    brain::Brain& kb;

public:
    Type Base;

    Type Parameter;
    Type ParameterDecl;
    Type Cell;
    Type HasMember;
    Type GetMember;
    Type SetMember;
    Type SetVar;
    Type GetVar;
    Type Self;

    Type Block;
    Type Function;
    Type Delete;
    Type Set;
    Type If;
    Type Do;
    Type While;
    Type Expression;
    Type Input;
    Type New;
    Type Same;
    Type NotSame;
    Type Equal;
    Type NotEqual;
    Type Has;
    Type Get;
    Type And;
    Type Or;
    Type Not;
    Type Add;
    Type Subtract;
    Type Multiply;
    Type Divide;
    Type LessThan;
    Type GreaterThan;
};

} // namespace type

class Types
{
public:
    Types(brain::Brain& kb);

    Type& ListOf(CellI& type);
    Type& MapOf(CellI& type);

protected:
    brain::Brain& kb;
    std::map<CellI*, Type> m_listTypes;
    std::map<CellI*, Type> m_mapTypes;
    friend class TypeInit;

public:
    Type Type_;
    Type Slot;
    Type Container;
    Type Iterator;
    Type List;
    Type Map;
    Type Index;
    Type Void;
    Type Any;
    Type Boolean;
    Type Char;
    Type Digit;
    Type Number;
    Type String;
    Type Color;
    Type Pixel;
    Type Picture;
    Type Template;

    type::Template template_;
    type::Control control;
    type::Ast ast;
};

class Cells
{
public:
    Cells(brain::Brain& kb, Type& voidType, Type& anyType);
    cells::Slot& slot(cells::CellI& role, cells::CellI& type);

    brain::Brain& kb;
    Object type;
    Object slots;
    Object slotType;
    Object slotRole;
    Object subTypes;
    Object index;
    Object list;
    Object memberOf;
    Object emptyObject;
};

class Coding
{
public:
    Coding(brain::Brain& kb, Type& anyType);
    Object argument;
    Object ast;
    Object branch;
    Object cell;
    Object condition;
    Object container;
    Object else_;
    Object input;
    Object item;
    Object label;
    Object lhs;
    Object objectType;
    Object op;
    Object output;
    Object parameter;
    Object parameters;
    Object result;
    Object rhs;
    Object role;
    Object self;
    Object statement;
    Object template_;
    Object then;
    Object value;
};

namespace templates {

class CellDescription : public Object
{
public:
    CellDescription(brain::Brain& kb, CellI& classCell, const std::string& label = "");
};

class ParameterDecl : public Object,
                      public NewT<ParameterDecl>
{
public:
    ParameterDecl(brain::Brain& kb, CellI& role, CellI& type);
};

class Slot : public Object,
             public NewT<Slot>
{
public:
    Slot(brain::Brain& kb, templates::CellDescription& role, templates::CellDescription& type);
};

template <typename T>
class CellDescriptionT : public CellDescription,
                         public NewT<T>
{
public:
    CellDescriptionT<T>(brain::Brain& kb, CellI& classCell, const std::string& label = "") :
        CellDescription(kb, classCell, label)
    {
    }
};

class Cell : public CellDescriptionT<Cell>
{
public:
    Cell(brain::Brain& kb, CellI& cell);
};

class Parameter : public CellDescriptionT<Parameter>
{
public:
    Parameter(brain::Brain& kb, CellI& paramRole);
};

class TemplateOf : public CellDescriptionT<TemplateOf>
{
public:
    TemplateOf(brain::Brain& kb, Template& templateOf, CellDescription& paramDescription, CellDescription& valueDescription);
};

class Self : public CellDescriptionT<Self>
{
public:
    Self(brain::Brain& kb);
};
} // namespace templates

class Templates
{
public:
    Templates(brain::Brain& kb);
    templates::ParameterDecl& parameterDecl(CellI& role, CellI& type);
    templates::Slot& slot(templates::CellDescription& role, templates::CellDescription& type);
    templates::Cell& cell(CellI& cell);
    templates::Parameter& parameter(CellI& paramRole);
    templates::TemplateOf& templateOf(Template& templateOf, templates::CellDescription& paramDescription, templates::CellDescription& valueDescription);
    templates::Self& self();

protected:
    brain::Brain& kb;

public:
    Template list;
};

class Ast
{
public:
    class Base : public Object
    {
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
    class Parameter : public BaseT<Parameter>
    {
    public:
        Parameter(brain::Brain& kb, CellI& role);
    };
    class ParameterDecl : public BaseT<ParameterDecl>
    {
    public:
        ParameterDecl(brain::Brain& kb, CellI& role, CellI& type);
    };
    class Cell : public BaseT<Cell>
    {
    public:
        Cell(brain::Brain& kb, CellI& value);
    };
    class HasMember : public BaseT<HasMember>
    {
    public:
        HasMember(brain::Brain& kb, Base& role);
    };
    class GetMember : public BaseT<GetMember>
    {
    public:
        GetMember(brain::Brain& kb, Base& role);
    };
    class SetMember : public BaseT<SetMember>
    {
    public:
        SetMember(brain::Brain& kb, Base& role, Base& value);
    };
    class SetVar : public BaseT<SetVar>
    {
    public:
        SetVar(brain::Brain& kb, CellI& role, Base& value);
    };
    class GetVar : public BaseT<GetVar>
    {
    public:
        GetVar(brain::Brain& kb, CellI& role);
    };
    class Self : public BaseT<Self>
    {
    public:
        Self(brain::Brain& kb);
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
        void addAsts(Block& ast);
        CellI& inputType();
        CellI& outputType();
        CellI& compile(CellI& parameters);

    protected:
        CellI& compileAst(CellI& ast, CellI& param, CellI& self);
        List& inputs();
        List& outputs();
        Block& asts();

        List* m_inputs  = nullptr;
        List* m_outputs = nullptr;
        Block* m_asts   = nullptr;

        std::unique_ptr<Type> m_inputType;
        std::unique_ptr<Type> m_outputType;
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
        Do(brain::Brain& kb, Base& condition, Base& statement);
    };
    class While : public BaseT<While>
    {
    public:
        While(brain::Brain& kb, Base& condition, Base& statement);
    };
    class Input : public BaseT<Input>
    {
    public:
        Input(brain::Brain& kb, CellI& value);
    };
    class New : public BaseT<New>
    {
    public:
        New(brain::Brain& kb, Base& objectType);
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
    class Get : public BaseT<Get>
    {
    public:
        Get(brain::Brain& kb, Base& cell, Base& role);
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
        Not(brain::Brain& kb, Base& value);
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
    class GreaterThan : public BaseT<GreaterThan>
    {
    public:
        GreaterThan(brain::Brain& kb, Base& lhs, Base& rhs);
    };

    Ast(brain::Brain& kb);

    Parameter& parameter(CellI& role);
    ParameterDecl& parameterDecl(CellI& role, CellI& type);
    Cell& cell(CellI& value);
    HasMember& hasMember(Base& role);
    GetMember& getMember(Base& role);
    SetMember& setMember(Base& role, Base& value);
    SetVar& setVar(CellI& role, Base& value);
    GetVar& getVar(CellI& role);
    Self& self();

    template <typename... Args>
    Block& block(Base& ast, Args&&... args);
    Function& function(List& inputs, Block& asts);
    Function& function(List& inputs, Block& asts, List& outputs);
    Delete& delete_(Base& cell);
    Set& set(Base& cell, Base& role, Base& value);
    If& if_(Base& condition, Base& thenBranch);
    If& if_(Base& condition, Base& thenBranch, Base& elseBranch);
    Do& do_(Base& condition, Base& statement);
    While& while_(Base& condition, Base& statement);
    Input& input(CellI& value);
    New& new_(Base& objectType);
    Same& same(Base& lhs, Base& rhs);
    NotSame& notSame(Base& lhs, Base& rhs);
    Equal& equal(Base& lhs, Base& rhs);
    NotEqual& notEqual(Base& lhs, Base& rhs);
    Has& has(Base& cell, Base& role);
    Get& get(Base& cell, Base& role);
    And& and_(Base& lhs, Base& rhs);
    Or& or_(Base& lhs, Base& rhs);
    Not& not_(Base& value);
    Add& add(Base& lhs, Base& rhs);
    Subtract& subtract(Base& lhs, Base& rhs);
    Multiply& multiply(Base& lhs, Base& rhs);
    Divide& divide(Base& lhs, Base& rhs);
    LessThan& lessThan(Base& lhs, Base& rhs);
    GreaterThan& greaterThan(Base& lhs, Base& rhs);

protected:
    brain::Brain& kb;
};

class Sequence
{
public:
    Sequence(brain::Brain& kb);
    Object first;
    Object last;
    Object previous;
    Object next;
    Object current;
};

class Equation
{
public:
    Equation(brain::Brain& kb, Type& anyType);
    Object lhs;
    Object rhs;
};

class Directions
{
public:
    Directions(brain::Brain& kb, Type& anyType);
    Object up;
    Object down;
    Object left;
    Object right;
};

class Coordinates
{
public:
    Coordinates(brain::Brain& kb, Type& anyType);
    Object x;
    Object y;
};

class Colors
{
public:
    Colors(brain::Brain& kb, Type& anyType);
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

class Dimensions
{
public:
    Dimensions(brain::Brain& kb, Type& anyType);
    Object width;
    Object height;
    Object size;
};

class Visualization
{
public:
    Visualization(brain::Brain& kb, Type& anyType);
    Object color;
    Object pixels;
};

class Numbers
{
public:
    Numbers(brain::Brain& kb, Type& anyType);
    Map sign;
    Object positive;
    Object negative;
};

namespace pools {

class Chars
{
public:
    Chars(brain::Brain& kb, Type& charType, Object& emptyObject);
    Object& get(char32_t utf32Char);
    Type& type();

protected:
    void registerUnicodeBlock(char32_t from, char32_t to);
    std::map<char32_t, Object> s_characters;
    Type& m_charType;
    Object& m_emptyObject;
    brain::Brain& m_kb;
};

class Digits
{
public:
    Digits(brain::Brain& kb, Type& digit);
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
    Pools(brain::Brain& kb, Type& charType, Object& emptyObject, Type& digit);
    pools::Chars chars;
    pools::Digits digits;
    pools::Numbers numbers;
};

class Arc
{
public:
    Arc(brain::Brain& kb);

    Type Demonstration;
    Type Task;
    Object examples;
};

class Brain
{
protected:
    bool m_initialized = false;

public:
    Brain();
    Types type;
    Cells cells;
    Pools pools;
    Coding coding;
    Templates templates;
    Ast ast;
    Sequence sequence;
    Equation equation;
    Directions directions;
    Coordinates coordinates;
    Colors colors;
    Boolean boolean;
    Dimensions dimensions;
    Visualization visualization;
    Numbers numbers;
    Arc arc;

    Ast::Function listAdd;

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
        Map& ret = *new Map(*this, value.type());
        ret.add(key, value);
        if constexpr (sizeof...(Args) > 0) {
            ret.add(std::forward<Args>(args)...);
        }

        return ret;
    }

    bool isInitialized();
};

template <typename... Args>
Ast::Block& Ast::block(Base& ast, Args&&... args)
{
    return *new Block(kb, kb.list(std::forward<Args>(args)...));
}

} // namespace brain
} // namespace cells
} // namespace synth
