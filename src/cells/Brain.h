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

    List argument;
    List ast;
    List asts;
    List break_;
    List cell;
    List children;
    List code;
    List color;
    List compiled;
    List condition;
    List constructor;
    List continue_;
    List currentFn;
    List currentStruct;
    List data;
    List destructor;
    List else_;
    List emptyObject;
    List first;
    List functions;
    List globalScope;
    List height;
    List id;
    List index;
    List input;
    List instances;
    List item;
    List itemType;
    List key;
    List keyType;
    List last;
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
    List previous;
    List process;
    List resolvedScope;
    List result;
    List return_;
    List returnType;
    List rhs;
    List role;
    List rootNode;
    List scope;
    List scopes;
    List self;
    List size;
    List slotRole;
    List slots;
    List slotType;
    List stack;
    List statement;
    List static_;
    List status;
    List structs;
    List structType;
    List subTypes;
    List templateId;
    List templateParams;
    List then;
    List type;
    List unknownInstances;
    List unknownStructs;
    List value;
    List valueType;
    List variables;
    List width;
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
    Object Break;
    Object Call;
    Object Cell;
    Object Continue;
    Object Delete;
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
    Object SubTypeName;
    Object TemplatedType;
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

    cells::CellI& slot(const std::string& role, cells::CellI& type);
    cells::CellI& slot(cells::CellI& role, cells::CellI& type);
    cells::CellI& kvPair(cells::CellI& key, cells::CellI& value);
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
    Object KVPair;
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
    Object Program;
    Object StructReference;
    Object CompileState;
    Object ScopeData;
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
    class Scope;
    class Struct;
    class ResolvedType;
    class Base : public Object
    {
        Base(const Base&) = delete;
    public:
        Base(brain::Brain& kb, CellI& classCell, const std::string& label = "");

        Base& resolveType(CellI& typeAst, CellI& resolveState);
        CellI& getCompiledTypeFromResolvedType(CellI& ast);
        CellI& getResolvedTypeById(CellI& id, bool isInstance, CellI& resolveState);

    protected:
        CellI& resolveId(CellI& id, CellI& containerId, CellI& unknownContainerId, CellI& resolveState, std::function<CellI&(CellI& structReference)> unknownCb);
        CellI& resolveStructName(CellI& structName, CellI& resolveState);
        Struct& resolveStructNameAsAst(CellI& structName, CellI& resolveState);
        Base& resolveSubTypeNameAsAst(CellI& subTypeName, CellI& resolveState);
        CellI& resolveTemplateInstanceId(CellI& structId, CellI& idScope, CellI& resolveState, CellI& ast, CellI& templateParams);
        Struct& resolveTemplateInstanceIdAsAst(CellI& structId, CellI& idScope, CellI& resolveState, CellI& ast, CellI& templateParams);
        Base& resolveTemplatedType(CellI& ast, CellI& resolveState);
        List& generateTemplateId(CellI& id, CellI& parameters, CellI& resolveState, List& resolvedParams);
        ResolvedType& resolvedType(CellI& astType, CellI& compiledType);
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
        Get& operator/(const std::string& role);
    };
    class StructName : public BaseT<StructName>
    {
    public:
        StructName(brain::Brain& kb, CellI& name);
    };
    class Call;
    class Self : public BaseT<Self>
    {
    public:
        Self(brain::Brain& kb);
        Call& call(const std::string& method);
        template <typename... Args>
        Call& call(const std::string& method, Args&&... args);
    };
    class SelfFn : public BaseT<SelfFn>
    {
    public:
        SelfFn(brain::Brain& kb);
    };
    class Continue : public BaseT<Continue>
    {
    public:
        Continue(brain::Brain& kb);
    };
    class Break : public BaseT<Break>
    {
    public:
        Break(brain::Brain& kb);
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
        Get& operator/(const std::string& role);
    };
    class ResolvedType : public BaseT<ResolvedType>
    {
    public:
        ResolvedType(brain::Brain& kb, CellI& astType, CellI& compiledType);
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

        void addParam(Slot& slot);

        template <typename... Args>
        void addParam(Slot& slot, Args&&... args)
        {
            addParam(slot);
            addParam(std::forward<Args>(args)...);
        }
    };

    class StaticCall : public BaseT<StaticCall>
    {
    public:
        StaticCall(brain::Brain& kb, CellI& cell, CellI& method);

        void addParam(Slot& slot);

        template <typename... Args>
        void addParam(Slot& slot, Args&&... args)
        {
            addParam(slot);
            addParam(std::forward<Args>(args)...);
        }
    };

    class Block : public BaseT<Block>
    {
    public:
        Block(brain::Brain& kb, List& list);
    };

    class Var;
    class Function;
    class FunctionT;
    class Struct;
    class StructT;
    class Scope : public BaseT<Scope>
    {
    public:
        Scope(brain::Brain& kb, const std::string& name);

        bool hasScope(CellI& id);
        Scope& getScope(const std::string& name);
        Scope& getScope(CellI& id);
        Scope& addScope(const std::string& name);
        void addScope(Scope& scope);

        Function& addFunction(const std::string& name);
        Function& addFunction(CellI& id, const std::string& label);
        void addFunction(Function& function);

        FunctionT& addFunctionT(const std::string& name);
        FunctionT& addFunctionT(CellI& id, const std::string& label);

        bool hasVariable(CellI& id);
        Var& getVariable(CellI& id);
        Var& addVariable(CellI& id);

        Ast::Struct& resolveStructName(CellI& name);
        Ast::Struct& resolveStructNameImpl(CellI& name);
        bool hasStruct(CellI& id);
        Struct& getStruct(const std::string& name);
        Struct& getStruct(CellI& id);
        void addStruct(Struct& struct_);
        Struct& addStruct(const std::string& name);

        bool hasStructT(CellI& id);
        StructT& getStructT(CellI& id);
        StructT& addStructT(const std::string& name);

        Struct& resolveFullStructId(CellI& scopeList, CellI& id);
        Scope* resolveFullTemplateId(CellI& scopeList, CellI& id);
        Scope& getRootScope();

        CellI& getFullId();

        CellI& compile();

    protected:
        void resolveTypes(CellI& state);
        void compileTheResolvedAsts(CellI& programData, CellI& state);

        TrieMap& variables();
        TrieMap& scopes();
        TrieMap& functions();
        TrieMap& functionTs();
        TrieMap& structs();
        TrieMap& structTs();
    };

    class StructBase : public Base
    {
    public:
        StructBase(brain::Brain& kb, CellI& astType, CellI& cell, const std::string& label);
        StructBase(brain::Brain& kb, CellI& astType, const std::string& name);

        Function& addMethod(const std::string& name);
        void addMethod(Function& method);

    public:
        StructBase& members(Slot& slot);
        template <typename... Args>
        StructBase& members(Slot& slot, Args&&... args)
        {
            members(slot);
            members(std::forward<Args>(args)...);
            return *this;
        }

        StructBase& subTypes(Slot& param);
        template <typename... Args>
        StructBase& subTypes(Slot& param, Args&&... args)
        {
            subTypes(param);
            subTypes(std::forward<Args>(args)...);
            return *this;
        }

        StructBase& memberOf(CellI& type);
        template <typename... Args>
        StructBase& memberOf(CellI& type, Args&&... args)
        {
            memberOf(type);
            memberOf(std::forward<Args>(args)...);
            return *this;
        }

        CellI& id();

        Base& getSubType(CellI& name);

    protected:
        Map& methods();
        Map& members();
        Map& subTypes();
        List& memberOf();
    };

    class Struct : public StructBase,
                   public NewT<Struct>
    {
    public:
        Struct(brain::Brain& kb, CellI& id, const std::string& label = "ast.struct");
        Struct(brain::Brain& kb, const std::string& name);

        CellI& getFullId();
        Struct& resolveTypes(CellI& resolveState);
        CellI& compile(CellI& state);
    };

    class StructT : public StructBase,
                    public NewT<StructT>
    {
    public:
        using StructBase::kb;
        StructT(brain::Brain& kb, CellI& id, const std::string& label = "ast.structT");
        StructT(brain::Brain& kb, const std::string& name);

        Ast::StructT& templateParams(Slot& param);

        template <typename... Args>
        Ast::StructT& templateParams(Slot& param, Args&&... args)
        {
            templateParams(param);
            templateParams(std::forward<Args>(args)...);

            return *this;
        }

        Struct& instantiateWith(List& slotList, CellI& state);

    protected:
        CellI& instantiateTemplateParamType(CellI& param, CellI& selfType, Map& inputParameters, CellI& state);
        Base& instantiateAst(CellI& ast, CellI& selfType, Map& inputParameters, CellI& state);
        Map& templateParams();
    };

    class Function : public BaseT<Function>
    {
    public:
        Function(brain::Brain& kb, CellI& name, const std::string& label = "ast.function");
        Function(brain::Brain& kb, const std::string& name);

        Function& parameters(Slot& param);

        template <typename... Args>
        Function& parameters(Slot& param, Args&&... args)
        {
            parameters(param);
            parameters(std::forward<Args>(args)...);

            return *this;
        }

        Function& returnType(CellI& type);

        template <typename... Args>
        void code(Args&&... args);

        Ast::Function& resolveTypes(CellI& resolveState);
        CellI& compile(CellI& state);
        std::string shortName();
        CellI& getFullId();

    protected:
        Ast::Base& resolveTypesInCode(CellI& resolveState, CellI& ast);
        void addBlock(Block& block);
        void compileParams(cells::Object& function, cells::Map& subTypesMap, CellI& state);
        CellI& compileAst(CellI& ast, cells::Object& function, CellI& state);
        void checkMethodCall(CellI& astType, CellI& astMethodId, CellI& state);
        List& parameters();
        CellI& returnType();
        Base& code();
    };

    class FunctionT : public BaseT<FunctionT>
    {
    public:
        FunctionT(brain::Brain& kb, CellI& name, const std::string& label = "ast.functionT");
        FunctionT(brain::Brain& kb, const std::string& name);

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
        Get& operator/(const std::string& role);
        Has& exist();
        Missing& missing();
        Call& call(const std::string& method);
        template <typename... Args>
        Call& call(const std::string& method, Args&&... args);
    };
    class SubType : public BaseT<SubType>
    {
    public:
        SubType(const SubType&) = delete;
        SubType(brain::Brain& kb, CellI& name);
    };
    class TemplatedType : public BaseT<TemplatedType>
    {
    public:
        TemplatedType(const TemplatedType&) = delete;
        TemplatedType(brain::Brain& kb, CellI& id, CellI& typeList);

        void addParam(const std::string& role, CellI& type);
        void addParam(const std::string& role, const std::string& type);

        template <typename... Args>
        void addParam(const std::string& role, CellI& type, Args&&... args)
        {
            addParam(role, type);
            addParam(std::forward<Args>(args)...);
        }
        template <typename... Args>
        void addParam(const std::string& role, const std::string& type, Args&&... args)
        {
            addParam(role, type);
            addParam(std::forward<Args>(args)...);
        }
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

        void addParam(Slot& slot);

        template <typename... Args>
        void addParam(Slot& slot, Args&&... args)
        {
            addParam(slot);
            addParam(std::forward<Args>(args)...);
        }
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
        Get& operator/(const std::string& role);
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
    StructName& structName(CellI& id);
    StructName& structName(const std::string& idStr);
    Self& self();
    SelfFn& selfFn();
    Continue& continue_();
    Break& break_();
    Return& return_();
    Return& return_(Base& value);
    Parameter& parameter(CellI& role);
    Slot& slot(const std::string& role, CellI& type);
    Slot& slot(CellI& role, CellI& type);

    Call& call(CellI& object, const std::string& method);
    Call& call(CellI& object, CellI& method);
    template <typename... Args>
    Call& call(CellI& object, const std::string& method, Args&&... args);

    StaticCall& scall(CellI& type, const std::string& method);
    StaticCall& scall(CellI& type, CellI& method);
    template <typename... Args>
    StaticCall& scall(CellI& type, const std::string& method, Args&&... args);


    template <typename... Args>
    Block& block(Args&&... args);
    Delete& delete_(Base& cell);
    Set& set(Base& cell, Base& role, Base& value);
    Set& set(Base& cell, const std::string& role, Base& value);
    Erase& erase(Base& cell, Base& role);
    Erase& erase(Base& cell, const std::string& role);
    If& if_(Base& condition, Base& thenBranch);
    If& if_(Base& condition, Base& thenBranch, Base& elseBranch);
    Do& do_(Base& condition, Base& statement);
    While& while_(Base& condition, Base& statement);
    Var& var(CellI& role);
    Member& member(CellI& role);
    SubType& subType(CellI& role);
    TemplatedType& templatedType(const std::string& id, CellI& type);
    template <typename... Args>
    TemplatedType& templatedType(const std::string& id, const std::string& role, CellI& type, Args&&... args);
    template <typename... Args>
    TemplatedType& templatedType(const std::string& id, const std::string& role, const std::string& type, Args&&... args);

    TemplateParam& templateParam(CellI& role);
    New& new_(Base& objectType);
    New& new_(Base& objectType, Base& constructor);

    template<typename... Args>
    New& new_(Base& objectType, const std::string& constructor, Args&&... params);

    template <typename... Args>
    New& new_(const std::string& objectType, const std::string& constructor, Args&&... params);

    Same& same(Base& lhs, Base& rhs);
    NotSame& notSame(Base& lhs, Base& rhs);
    Equal& equal(Base& lhs, Base& rhs);
    NotEqual& notEqual(Base& lhs, Base& rhs);
    Has& has(Base& cell, Base& role);
    Has& has(Base& cell, const std::string& role);
    Missing& missing(Base& cell, Base& role);
    Missing& missing(Base& cell, const std::string& id);
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
    CellI& processNamespacedName(const std::string& inputName, std::function<CellI&(const std::string& outName)> createCb);
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
    std::map<char32_t, Object> m_characters;
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

class Strings
{
public:
    Strings(brain::Brain& kb);
    String& get(const std::string& str);
    List& getCharList(const std::string& str);

protected:
    std::map<std::string, String> m_strings;
    brain::Brain& kb;
};

} // namespace pools

class Pools
{
public:
    Pools(brain::Brain& kb);

    pools::Chars chars;
    pools::Digits digits;
    pools::Numbers numbers;
    pools::Strings strings;
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
    void createStd();
    void createArcSolver();
    void createTests();

public:
    Ast::Cell& _(CellI& cell);
    Ast::Cell& _(const std::string& id);
    Ast::Parameter& p_(const std::string& name);
    Ast::Member& m_(const std::string& name);
    Ast::Var& var_(const std::string& name);
    Ast::Slot& param(const std::string& name, CellI& value);
    Ast::Slot& member(const std::string& name, CellI& type);
    template <typename... Args>
    Ast::SubType& st_(const std::string& name, Args&&... args);
    Ast::TemplateParam& tp_(const std::string& name);
    template <typename... Args>
    Ast::TemplatedType& tt_(const std::string& name, Args&&... args);
    Ast::StructName& struct_(const std::string& name);

public:
    Brain();
    ~Brain();

    Pools pools;
    ID ids;
    Types type;
    Ast ast;
    Directions directions;
    Coordinates coordinates;
    Colors colors;
    Boolean boolean;
    Numbers numbers;
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

    Ast::Scope globalScope;
    CellI* compiledGlobalScopePtr = nullptr;

public:
    CellI& getStruct(const std::string& name);
    CellI& getStruct(CellI& id);
    CellI& id(const std::string& str);
    template <typename... Args>
    CellI& templateId(const std::string& str, Args&&... args);

    CellI& toKbBool(bool value);

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

#pragma region Brain
template <typename... Args>
Ast::SubType& Brain::st_(const std::string& name, Args&&... args)
{
    return ast.subType(id(name), std::forward<Args>(args)...);
}

template <typename... Args>
Ast::TemplatedType& Brain::tt_(const std::string& name, Args&&... args)
{
    return ast.templatedType(name, std::forward<Args>(args)...);
}

template <typename... Args>
CellI& Brain::templateId(const std::string& str, Args&&... args)
{
    List& idCell = *new List(*this, type.Cell);
    for (const auto& character : str) {
        idCell.add(pools.chars.get(character));
    }
    idCell.add(std::forward<Args>(args)...);

    return idCell;
}

template <typename... Args>
List& Brain::list(CellI& value, Args&&... args)
{
    List& ret = *new List(*this, value.type());
    ret.add(value);
    if constexpr (sizeof...(Args) > 0) {
        ret.add(std::forward<Args>(args)...);
    }

    return ret;
}

template <typename... Args>
Map& Brain::map(CellI& key, CellI& value, Args&&... args)
{
    Map& ret = *new Map(*this, key.type(), value.type(), std::format("Map<{}, {}>(...)", key.type().label(), value.type().label()));
    if constexpr (sizeof...(Args) > 0) {
        ret.add(std::forward<Args>(args)...);
    }

    return ret;
}

template <typename... Args>
Set& Brain::set(CellI& value, Args&&... args)
{
    Set& ret = *new Set(*this, value.type(), std::format("Map<{}, {}>(...)", value.type().label()));
    if constexpr (sizeof...(Args) > 0) {
        ret.add(std::forward<Args>(args)...);
    }

    return ret;
}

template <typename... Args>
void Brain::addSlots(Map& map, CellI& value, Args&&... args)
{
    map.add(value["slotRole"], value);
    addSlots(map, std::forward<Args>(args)...);
}

template <typename... Args>
Map& Brain::slots(CellI& value, Args&&... args)
{
    Map& ret = *new Map(*this, type.Cell, type.Slot, "Map<Cell, Slot>(...)");
    addSlots(ret, value, std::forward<Args>(args)...);

    return ret;
}
#pragma endregion
#pragma region Ast
template <typename... Args>
Ast::Block& Ast::block(Args&&... args)
{
    return *new Block(kb, kb.list(std::forward<Args>(args)...));
}


template <typename... Args>
Ast::New& Ast::new_(Base& objectType, const std::string& constructor, Args&&... args)
{
    auto& ret = new_(objectType, kb.ast.cell(kb.id(constructor)));
    if constexpr (sizeof...(Args) > 0) {
        ret.addParam(std::forward<Args>(args)...);
    }
    return ret;
}

template <typename... Args>
Ast::New& Ast::new_(const std::string& objectType, const std::string& constructor, Args&&... args)
{
    auto& ret = new_(kb.ast.structName(kb.id(objectType)), kb.ast.cell(kb.id(constructor)));
    if constexpr (sizeof...(Args) > 0) {
        ret.addParam(std::forward<Args>(args)...);
    }
    return ret;
}

template <typename... Args>
Ast::Call& Ast::Self::call(const std::string& method, Args&&... args)
{
    return kb.ast.call(*this, method, std::forward<Args>(args)...);
}

template <typename... Args>
Ast::Call& Ast::Member::call(const std::string& method, Args&&... args)
{
    return kb.ast.call(*this, method, std::forward<Args>(args)...);
}

template <typename... Args>
Ast::Call& Ast::call(CellI& object, const std::string& method, Args&&... args)
{
    auto& ret = call(object, method);
    if constexpr (sizeof...(Args) > 0) {
        ret.addParam(std::forward<Args>(args)...);
    }
    return ret;
}

template <typename... Args>
Ast::StaticCall& Ast::scall(CellI& object, const std::string& method, Args&&... args)
{
    auto& ret = scall(object, method);
    if constexpr (sizeof...(Args) > 0) {
        ret.addParam(std::forward<Args>(args)...);
    }
    return ret;
}

template <typename... Args>
Ast::TemplatedType& Ast::templatedType(const std::string& id, const std::string& role, CellI& type, Args&&... args)
{
    auto& ret = templatedType(id, kb.ast.slot(role, type));
    if constexpr (sizeof...(Args) > 0) {
        ret.addParam(std::forward<Args>(args)...);
    }
    return ret;
}

template <typename... Args>
Ast::TemplatedType& Ast::templatedType(const std::string& id, const std::string& role, const std::string& type, Args&&... args)
{
    auto& ret  = templatedType(id, kb.ast.slot(role, kb.ast.structName(kb.id(type))));
    if constexpr (sizeof...(Args) > 0) {
        ret.addParam(std::forward<Args>(args)...);
    }
    return ret;
}

template <typename... Args>
void Ast::Function::code(Args&&... args)
{
    addBlock(*new Block(kb, kb.list(std::forward<Args>(args)...)));
}
#pragma endregion

} // namespace brain
} // namespace cells
} // namespace synth
