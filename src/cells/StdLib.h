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

        class EActivate : public Object
        {
        public:
            class EState : public Object
            {
            public:
                EState(World& w, CellI& type, const std::string& label);

                Object& missingInput;
                Object& ready;
                Object activateInput;
            };
            EActivate(World& w, CellI& type, const std::string& label);

            EState State;
        };

        class EAdd : public Object
        {
        public:
            class EState : public Object
            {
            public:
                EState(World& w, CellI& type, const std::string& label);

                Object& missingInput;
                Object& ready;
                Object activateLhs;
                Object activateRhs;
            };
            EAdd(World& w, CellI& type, const std::string& label);

            EState State;
        };

        class EAnd : public Object
        {
        public:
            class EState : public Object
            {
            public:
                EState(World& w, CellI& type, const std::string& label);

                Object& missingInput;
                Object& ready;
                Object activateLhs;
                Object activateRhs;
            };
            EAnd(World& w, CellI& type, const std::string& label);

            EState State;
        };

        class EBlock : public Object
        {
        public:
            class EState : public Object
            {
            public:
                EState(World& w, CellI& type, const std::string& label);

                Object& missingInput;
                Object& ready;
                Object activateInput;
            };
            EBlock(World& w, CellI& type, const std::string& label);

            EState State;
        };

        class ECall : public Object
        {
        public:
            class EState : public Object
            {
            public:
                EState(World& w, CellI& type, const std::string& label);

                Object& missingInput;
                Object& ready;
                Object activateParameters;
                Object stackPushAndCall;
                Object stackPop;
            };
            ECall(World& w, CellI& type, const std::string& label);

            EState State;
        };

        class EDelete : public Object
        {
        public:
            class EState : public Object
            {
            public:
                EState(World& w, CellI& type, const std::string& label);

                Object& missingInput;
                Object& ready;
                Object activateInput;
            };
            EDelete(World& w, CellI& type, const std::string& label);

            EState State;
        };

        class EDivide : public Object
        {
        public:
            class EState : public Object
            {
            public:
                EState(World& w, CellI& type, const std::string& label);

                Object& missingInput;
                Object& ready;
                Object activateLhs;
                Object activateRhs;
            };
            EDivide(World& w, CellI& type, const std::string& label);

            EState State;
        };

        class EDo : public Object
        {
        public:
            class EState : public Object
            {
            public:
                EState(World& w, CellI& type, const std::string& label);

                Object& missingInput;
                Object& ready;
                Object activateStatement;
                Object activateCondition;
            };
            EDo(World& w, CellI& type, const std::string& label);

            EState State;
        };

        class EEqual : public Object
        {
        public:
            class EState : public Object
            {
            public:
                EState(World& w, CellI& type, const std::string& label);

                Object& missingInput;
                Object& ready;
                Object activateLhs;
                Object activateRhs;
            };
            EEqual(World& w, CellI& type, const std::string& label);

            EState State;
        };

        class EErase : public Object
        {
        public:
            class EState : public Object
            {
            public:
                EState(World& w, CellI& type, const std::string& label);

                Object& missingInput;
                Object& ready;
                Object activateCell;
                Object activateKey;
            };
            EErase(World& w, CellI& type, const std::string& label);

            EState State;
        };

        class EFunction : public Object
        {
        public:
            class EState : public Object
            {
            public:
                EState(World& w, CellI& type, const std::string& label);

                Object& missingInput;
                Object& ready;
                Object activateOp;
            };
            EFunction(World& w, CellI& type, const std::string& label);

            EState State;
        };

        class EGet : public Object
        {
        public:
            class EState : public Object
            {
            public:
                EState(World& w, CellI& type, const std::string& label);

                Object& missingInput;
                Object& ready;
                Object activateCell;
                Object activateKey;
            };
            EGet(World& w, CellI& type, const std::string& label);

            EState State;
        };

        class EGreaterThan : public Object
        {
        public:
            class EState : public Object
            {
            public:
                EState(World& w, CellI& type, const std::string& label);

                Object& missingInput;
                Object& ready;
                Object activateLhs;
                Object activateRhs;
            };
            EGreaterThan(World& w, CellI& type, const std::string& label);

            EState State;
        };

        class EGreaterThanOrEqual : public Object
        {
        public:
            class EState : public Object
            {
            public:
                EState(World& w, CellI& type, const std::string& label);

                Object& missingInput;
                Object& ready;
                Object activateLhs;
                Object activateRhs;
            };
            EGreaterThanOrEqual(World& w, CellI& type, const std::string& label);

            EState State;
        };

        class EHas : public Object
        {
        public:
            class EState : public Object
            {
            public:
                EState(World& w, CellI& type, const std::string& label);

                Object& missingInput;
                Object& ready;
                Object activateCell;
                Object activateKey;
            };
            EHas(World& w, CellI& type, const std::string& label);

            EState State;
        };

        class EIf : public Object
        {
        public:
            class EState : public Object
            {
            public:
                EState(World& w, CellI& type, const std::string& label);

                Object& missingInput;
                Object& ready;
                Object activateCondition;
                Object activateThen;
                Object activateElse;
            };
            EIf(World& w, CellI& type, const std::string& label);

            EState State;
        };

        class ELessThan : public Object
        {
        public:
            class EState : public Object
            {
            public:
                EState(World& w, CellI& type, const std::string& label);

                Object& missingInput;
                Object& ready;
                Object activateLhs;
                Object activateRhs;
            };
            ELessThan(World& w, CellI& type, const std::string& label);

            EState State;
        };

        class ELessThanOrEqual : public Object
        {
        public:
            class EState : public Object
            {
            public:
                EState(World& w, CellI& type, const std::string& label);

                Object& missingInput;
                Object& ready;
                Object activateLhs;
                Object activateRhs;
            };
            ELessThanOrEqual(World& w, CellI& type, const std::string& label);

            EState State;
        };

        class EMissing : public Object
        {
        public:
            class EState : public Object
            {
            public:
                EState(World& w, CellI& type, const std::string& label);

                Object& missingInput;
                Object& ready;
                Object activateCell;
                Object activateKey;
            };
            EMissing(World& w, CellI& type, const std::string& label);

            EState State;
        };

        class EMultiply : public Object
        {
        public:
            class EState : public Object
            {
            public:
                EState(World& w, CellI& type, const std::string& label);

                Object& missingInput;
                Object& ready;
                Object activateLhs;
                Object activateRhs;
            };
            EMultiply(World& w, CellI& type, const std::string& label);

            EState State;
        };

        class ENew : public Object
        {
        public:
            class EState : public Object
            {
            public:
                EState(World& w, CellI& type, const std::string& label);

                Object& missingInput;
                Object& ready;
                Object activateObjectType;
            };
            ENew(World& w, CellI& type, const std::string& label);

            EState State;
        };

        class ENot : public Object
        {
        public:
            class EState : public Object
            {
            public:
                EState(World& w, CellI& type, const std::string& label);

                Object& missingInput;
                Object& ready;
                Object activateInput;
            };
            ENot(World& w, CellI& type, const std::string& label);

            EState State;
        };

        class ENotEqual : public Object
        {
        public:
            class EState : public Object
            {
            public:
                EState(World& w, CellI& type, const std::string& label);

                Object& missingInput;
                Object& ready;
                Object activateLhs;
                Object activateRhs;
            };
            ENotEqual(World& w, CellI& type, const std::string& label);

            EState State;
        };

        class ENotSame : public Object
        {
        public:
            class EState : public Object
            {
            public:
                EState(World& w, CellI& type, const std::string& label);

                Object& missingInput;
                Object& ready;
                Object activateLhs;
                Object activateRhs;
            };
            ENotSame(World& w, CellI& type, const std::string& label);

            EState State;
        };

        class EOr : public Object
        {
        public:
            class EState : public Object
            {
            public:
                EState(World& w, CellI& type, const std::string& label);

                Object& missingInput;
                Object& ready;
                Object activateLhs;
                Object activateRhs;
            };
            EOr(World& w, CellI& type, const std::string& label);

            EState State;
        };

        class EReturn : public Object
        {
        public:
            class EState : public Object
            {
            public:
                EState(World& w, CellI& type, const std::string& label);

                Object& missingInput;
                Object& ready;
                Object activateResult;
            };
            EReturn(World& w, CellI& type, const std::string& label);

            EState State;
        };

        class ESame : public Object
        {
        public:
            class EState : public Object
            {
            public:
                EState(World& w, CellI& type, const std::string& label);

                Object& missingInput;
                Object& ready;
                Object activateLhs;
                Object activateRhs;
            };
            ESame(World& w, CellI& type, const std::string& label);

            EState State;
        };

        class ESet : public Object
        {
        public:
            class EState : public Object
            {
            public:
                EState(World& w, CellI& type, const std::string& label);

                Object& missingInput;
                Object& ready;
                Object activateCell;
                Object activateKey;
                Object activateValue;
            };
            ESet(World& w, CellI& type, const std::string& label);

            EState State;
        };

        class ESubtract : public Object
        {
        public:
            class EState : public Object
            {
            public:
                EState(World& w, CellI& type, const std::string& label);

                Object& missingInput;
                Object& ready;
                Object activateLhs;
                Object activateRhs;
            };
            ESubtract(World& w, CellI& type, const std::string& label);

            EState State;
        };

        class EWhile : public Object
        {
        public:
            class EState : public Object
            {
            public:
                EState(World& w, CellI& type, const std::string& label);

                Object& missingInput;
                Object& ready;
                Object activateCondition;
                Object activateStatement;
            };
            EWhile(World& w, CellI& type, const std::string& label);

            EState State;
        };

        class EMembership : public Object
        {
        public:
            EMembership(World& w, CellI& type, const std::string& label);

            Object internal;
            Object external;
        };

        class EState : public Object
        {
        public:
            EState(World& w, CellI& type, const std::string& label);

            Object missingInput;
            Object ready;
        };

    protected:
        World& w;

    public:
        EActivate Activate;
        EAdd Add;
        EAnd And;
        Object Base;
        EBlock Block;
        ECall Call;
        Object ConstVar;
        EDelete Delete;
        EDivide Divide;
        EDo Do;
        EEqual Equal;
        EErase Erase;
        EFunction Function;
        EGet Get;
        EGreaterThan GreaterThan;
        EGreaterThanOrEqual GreaterThanOrEqual;
        EHas Has;
        EIf If;
        ELessThan LessThan;
        ELessThanOrEqual LessThanOrEqual;
        Object Member;
        EMembership Membership;
        EMissing Missing;
        EMultiply Multiply;
        ENew New;
        ENot Not;
        ENotEqual NotEqual;
        ENotSame NotSame;
        EOr Or;
        Object Parameter;
        EReturn Return;
        ESame Same;
        Object SavedState;
        ESet Set;
        EState State;
        ESubtract Subtract;
        Object UnknownVar;
        Object Var;
        EWhile While;
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
        Object ConstVar;
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
        Object Struct;
        Object StructT;
        Object TemplatedType;
        Object TemplateParam;
        Object Trait;
        Object TraitImpl;
        Object TypeAlias;
        Object TypedEnumValue;
        Object TypeName;
        Object UnknownVar;
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
    Object Map_Struct_Struct;
    SNumber Number;
    ENumberSign NumberSign;
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