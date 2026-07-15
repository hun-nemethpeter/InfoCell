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
        Object AssociatedType;
        Object Base;
        Object Block;
        Object Break;
        Object Call;
        Object Cell;
        Object Continue;
        Object Do;
        Object Enum;
        Object EnumValue;
        Object For;
        Object Function;
        Object FunctionT;
        Object If;
        Object Match;
        Object Member;
        Object New;
        Object Parameter;
        Object PrimitiveToolName;
        Object ResolvedType;
        Object Return;
        Object Scope;
        Object Self;
        Object SelfType;
        Object Slot;
        Object StaticCall;
        Object Struct;
        Object StructT;
        Object TemplatedType;
        Object TemplateParam;
        Object Throw;
        Object Trait;
        Object TraitImpl;
        Object Try;
        Object TypeAlias;
        Object TypedEnumValue;
        Object TypeName;
        Object Var;
        Object While;
    };

    class EBoolean : public Object
    {
        EBoolean(World& w, CellI& type, const std::string& label);
        friend class Std;

    public:
        Object false_;
        Object true_;

        Object And;
        Object Not;
        Object Or;
    };

    class SCell : public Object
    {
        SCell(World& w, CellI& type, const std::string& label);
        friend class Std;

    public:
        Object Delete;
        Object Equal;
        Object Erase;
        Object Get;
        Object Has;
        Object Missing;
        Object NotEqual;
        Object NotSame;
        Object Same;
        Object Set;
    };

    class EDirection : public Object
    {
        EDirection(World& w, CellI& type, const std::string& label);
        friend class Std;

    public:
        Object up;
        Object down;
        Object left;
        Object right;
    };

    class ENumberSign : public Object
    {
        ENumberSign(World& w, CellI& type, const std::string& label);
        friend class Std;

    public:
        Object positive;
        Object negative;
    };

    class SNumber : public Object
    {
        SNumber(World& w, CellI& type, const std::string& label);
        friend class Std;

    public:
        Object Add;
        Object Divide;
        Object GreaterThan;
        Object GreaterThanOrEqual;
        Object LessThan;
        Object LessThanOrEqual;
        Object Multiply;
        Object Subtract;
    };

    Std(World& w);

    cells::CellI& kvPair(cells::CellI& key, cells::CellI& value);

protected:
    World& w;

public:
    EBoolean Boolean;
    SCell Cell;
    Object Char;
    Object Container;
    Object Digit;
    EDirection Direction;
    Object Enum;
    Object Index;
    Object KVPair;
    Object Library;
    Object List;
    Object ListNode;
    Object Map;
    SNumber Number;
    ENumberSign NumberSign;
    Object OpState;
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

class Compiler;
class StdLib : public Library
{
public:
    StdLib(World& w, Ast::Scope& parentScope, Compiler& compiler);
};

} // namespace cells
} // namespace infocell