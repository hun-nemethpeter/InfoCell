#pragma once

#include "Library.h"

namespace infocell {
namespace cells {

class World;
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
        Object AssociatedType;
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
    Object Direction;
    Object Enum;
    Object Grid;
    Object Index;
    Object KVPair;
    Object Library;
    Object List;
    Object ListItem;
    Object Map;
    Object Number;
    Object NumberSign;
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

    Object true_;
    Object false_;

    Object positive;
    Object negative;

    Op op;
    Ast ast;
};

class StdEnumValues
{
public:
    class EBoolean
    {
        EBoolean(World& w);
        friend class StdEnumValues;

    public:
        Object false_;
        Object true_;
    };

    class EDirection
    {
        EDirection(World& w);
        friend class StdEnumValues;

    public:
        Object up;
        Object down;
        Object left;
        Object right;
    };

    class ENumberSign
    {
        ENumberSign(World& w);
        friend class StdEnumValues;

    public:
        Object positive;
        Object negative;
    };

    StdEnumValues(World& w);

protected:
    World& w;

public:
    EBoolean Boolean;
    EDirection Direction;
    ENumberSign NumberSign;
};


class Compiler;
class StdLib : public Library
{
public:
    StdLib(World& w, Ast::Scope& parentScope, Compiler& compiler);
};

} // namespace cells
} // namespace infocell