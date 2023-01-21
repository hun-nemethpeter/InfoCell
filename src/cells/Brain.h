#pragma once

namespace synth {
namespace cells {
namespace brain {
namespace type {

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

class Types;
class TypeInit
{
public:
    TypeInit(Types& types);
};

class Types
{
public:
    Types(brain::Brain& kb);
    Type& ListOf(Type& type);
    Type& ListItemOf(Type& type);

protected:
    brain::Brain& kb;
    std::map<Type*, Type> m_listTypes;
    std::map<Type*, Type> m_listItemTypes;
    TypeInit m_init;
    friend class TypeInit;

public:
    Type Type_;
    Type Slot;
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

    type::Operations op;
    type::Pipelines pipeline;
};

class Cells
{
public:
    Cells(brain::Brain& kb, Type& voidType, Type& anyType);
    Object type;
    Object slots;
    Object slotType;
    Object slotName;
    Object slotRole;
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
    Object else_;
    Object input;
    Object name;
    Object objectType;
    Object op;
    Object output;
    Object result;
    Object role;
    Object statement;
    Object then;
    Object value;
};

class Sequence
{
public:
    Sequence(brain::Brain& kb, Type& anyType);
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
    Object sign;
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
