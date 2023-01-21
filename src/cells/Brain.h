#pragma once

namespace synth {
namespace cells {
namespace brain {
namespace type {

struct Pipelines
{
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
struct Logic
{
    Logic(brain::Brain& kb);
    Type And;
    Type Or;
    Type Not;
};

struct Math
{
    Math(brain::Brain& kb);
    Type Add;
    Type Subtract;
    Type Multiply;
    Type Divide;
    Type LessThan;
    Type GreaterThan;
};
} // namespace op

struct Operations
{
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
struct Types
{
    Types(brain::Brain& kb);
    Type& ListOf(Type& type);

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

protected:
    brain::Brain& m_kb;
    std::map<Type*, List> m_listTypes;
};

struct Cells
{
Cells(brain::Brain& kb, Type& voidType, Type& anyType);
    Object type;
    Object slots;
    Object slotType;
    Object slotName;
    Object slotRole;
    Object emptyObject;
};

struct Coding
{
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

struct Sequence
{
Sequence(brain::Brain& kb, Type& anyType);
    Object first;
    Object last;
    Object previous;
    Object next;
    Object current;
};

struct Equation
{
Equation(brain::Brain& kb, Type& anyType);
    Object lhs;
    Object rhs;
};

struct Directions
{
Directions(brain::Brain& kb, Type& anyType);
    Object up;
    Object down;
    Object left;
    Object right;
};

struct Coordinates
{
Coordinates(brain::Brain& kb, Type& anyType);
    Object x;
    Object y;
};

struct Colors
{
Colors(brain::Brain& kb, Type& anyType);
    Object red;
    Object green;
    Object blue;
};

struct Boolean
{
Boolean(brain::Brain& kb);
    Object true_;
    Object false_;
};

struct Dimensions
{
Dimensions(brain::Brain& kb, Type& anyType);
    Object width;
    Object height;
    Object size;
};

struct Visualization
{
Visualization(brain::Brain& kb, Type& anyType);
    Object color;
    Object pixels;
};

struct Numbers
{
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

struct Pools
{
    Pools(brain::Brain& kb, Type& charType, Object& emptyObject, Type& digit);
    pools::Chars chars;
    pools::Digits digits;
    pools::Numbers numbers;
};

struct Brain
{
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

    CellI& toKbBool(bool value);
};

} // namespace brain
} // namespace cells
} // namespace synth
