#pragma once

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

class Pipelines
{
public:
    Pipelines(brain::Brain& kb);
    Type Base;
    Type Void;
    Type Input;
    Type New;
    Type Fork;
    Type Delete;
    Type Node;
    Type IfThen;
    Type DoWhile;
    Type While;
};

namespace op {
class Logic
{
public:
    Logic(brain::Brain& kb);
    Type And;
    Type Or;
    Type Not;
};

class Math
{
public:
    Math(brain::Brain& kb);
    Type Add;
    Type Subtract;
    Type Multiply;
    Type Divide;
    Type LessThan;
    Type GreaterThan;
};
} // namespace op

class Operations
{
public:
    Operations(brain::Brain& kb);
    Type Base;

    Type Same;
    Type NotSame;
    Type Equal;
    Type NotEqual;

    Type Has;
    Type Get;
    Type Set;

    op::Logic logic;
    op::Math math;
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
    type::Operations op;
    type::Pipelines pipeline;
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
    Object branch;
    Object cell;
    Object condition;
    Object container;
    Object else_;
    Object input;
    Object item;
    Object label;
    Object objectType;
    Object op;
    Object output;
    Object parameter;
    Object parameters;
    Object result;
    Object role;
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

class SelfType : public CellDescriptionT<SelfType>
{
public:
    SelfType(brain::Brain& kb);
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
    templates::SelfType& selfType();

    brain::Brain& kb;
    Template list;
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

    CellI& toKbBool(bool value);
    bool isInitialized();
};

} // namespace brain
} // namespace cells
} // namespace synth
