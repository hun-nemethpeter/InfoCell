#pragma once
#include <fmt/core.h>
#include "Cells.h"

namespace spdlog {
class logger;
}

namespace infocell {
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
    List blue;
    List break_;
    List cell;
    List children;
    List color;
    List compiled;
    List condition;
    List constructor;
    List container;
    List continue_;
    List currentFn;
    List currentStruct;
    List data;
    List description;
    List destructor;
    List else_;
    List enum_;
    List emptyObject;
    List first;
    List functions;
    List globalScope;
    List green;
    List height;
    List id;
    List index;
    List input;
    List instances;
    List instructions;
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
    List pixelsMap;
    List previous;
    List process;
    List red;
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
    List state;
    List stateElse;
    List stateLhs;
    List statement;
    List stateParam1;
    List stateParam2;
    List stateParam3;
    List stateParamInit;
    List stateRhs;
    List stateThen;
    List static_;
    List status;
    List struct_;
    List structs;
    List structType;
    List subTypes;
    List templateId;
    List templateParams;
    List then;
    List throw_;
    List unknownInstances;
    List unknownStructs;
    List value;
    List valueType;
    List variable;
    List variables;
    List width;
};

namespace type {

class Op
{
public:
    Op(brain::Brain& kb);

protected:
    brain::Brain& kb;

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
    Object SubTypeName;
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

} // namespace type

class Std
{
public:
    Std(brain::Brain& kb);

    cells::CellI& slot(const std::string& role, cells::CellI& type);
    cells::CellI& slot(cells::CellI& role, cells::CellI& type);
    cells::CellI& kvPair(cells::CellI& key, cells::CellI& value);

protected:
    brain::Brain& kb;

public:
    Object Cell;
    Object Slot;
    Object Struct;
    Object Enum;
    Object Container;
    Object List;
    Object ListItem;
    Object KVPair;
    Object Map;
    Object Index;
    Object TrieMap;
    Object TrieMapNode;
    Object Boolean;
    Object Char;
    Object Digit;
    Object Number;
    Object String;
    Object Color;
    Object Pixel;
    Object Grid;
    Object Stack;
    Object StackFrame;
    Object Program;
    Object ProgramData;
    Object StructReference;
    Object CompileState;
    Object Directions;

    type::Op op;
    type::Ast ast;
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

#ifdef __cpp_concepts
template <class T, class EnumValue, class TypedEnumValue>
concept EnumValueConcept = std::is_same<T, EnumValue>::value || std::is_same<T, TypedEnumValue>::value;
#endif

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
        CellI& getFullyQualifiedNameImpl();
        CellI& resolveId(CellI& id, CellI& containerId, CellI& unknownContainerId, CellI& resolveState, std::function<CellI&(CellI& structReference)> unknownCb);
        CellI& resolveStructName(CellI& structName, CellI& resolveState);
        Base& resolveTypeNameAsAst(CellI& structName, CellI& resolveState);
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
        BaseT(brain::Brain& kb, CellI& classCell, const std::string& label) :
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
        Call& operator()(const std::string& method);
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
    class Try : public BaseT<Try>
    {
    public:
        Try(brain::Brain& kb, Base& tryBranch, Base& catchBranch);
    };
    class Throw : public BaseT<Throw>
    {
    public:
        Throw(brain::Brain& kb);
        Throw(brain::Brain& kb, Base& value);
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
        Call& operator()(const std::string& method);
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

        Call& operator()(const std::string& nameStr, CellI& value);
    };

    class StaticCall : public BaseT<StaticCall>
    {
    public:
        StaticCall(brain::Brain& kb, CellI& cell, CellI& method);

        StaticCall& operator()(const std::string& nameStr, CellI& value);
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
    class Enum;
    class Trait;
    class TraitImpl;

    template <class MapType, class TAst>
    class Items
    {
    public:
        Items(brain::Brain& kb, const std::string& mapName, Base& parent);

        bool has(CellI& id);
        TAst& get(const std::string& nameStr);
        TAst& get(CellI& name);
        TAst& add(const std::string& nameStr);
        void add(TAst& obj);
        MapType& items();

        brain::Brain& kb;
        const std::string m_mapName;
        Base& m_parent;
    };

    class Scope : public BaseT<Scope>
    {
    public:
        Scope(brain::Brain& kb, const std::string& nameStr);

        Base& resolveTypeName(CellI& name);
        StructT& resolveFullTemplateId(CellI& scopeList, CellI& name);
        Scope& getRootScope();
        CellI& getFullyQualifiedName();
        CellI& compile();
        CellI& reigisterStructBeforeCompilation(CellI& id);
        void registerBuiltInStruct(const std::string& fullName, CellI& compiledStruct);

        template <typename TAst>
        bool hasItem(CellI& name)
        {
            return getItemMember<TAst>().has(name);
        }

        template <typename TAst>
        TAst& getItem(const std::string& nameStr)
        {
            return getItemMember<TAst>().get(nameStr);
        }

        template <typename TAst>
        TAst& getItem(CellI& name)
        {
            return getItemMember<TAst>().get(name);
        }

        template <typename TAst>
        TAst& add(const std::string& nameStr)
        {
            return getItemMember<TAst>().add(nameStr);
        }

        template <typename TAst>
        void add(TAst& scope)
        {
            getItemMember<TAst>().add(scope);
        }

        template <typename TAst>
        TrieMap& items()
        {
            return getItemMember<TAst>().items();
        }

    protected:
        void registerEarlyStructs(TrieMap& unknownStructs, TrieMap& unknownInstances);
        void resolveEarlyStructs(TrieMap& unknownStructs, TrieMap& unknownInstances, Scope& resolvedScope);
        int instantiateTemplateInstances(TrieMap& unknownInstances, Object& compileState, Scope& resolvedScope);
        Enum* resolveFullEnumName(CellI& scopeList, CellI& name);
        Struct* resolveFullStructName(CellI& scopeList, CellI& name);
        Base* resolveFullNameInAllScope(CellI& scopeList, CellI& id, std::function<bool(Ast::Scope& currentScope)> hasCb, std::function<Base*(Ast::Scope& currentScope)> getCb);
        Base* resolveFullNameInOneScope(Scope* currentScope, CellI& scopeList, std::function<bool(Ast::Scope& currentScope)> hasCb, std::function<Base*(Ast::Scope& currentScope)> getCb);
        Ast::Scope& resolveTypes(CellI& state);
        void compileTheResolvedAsts(CellI& programData, CellI& state);

        template<class TAst>
        Items<TrieMap, TAst>& getItemMember();

        TrieMap earlyStructs;
        Items<TrieMap, Scope> scopesImpl;
        Items<TrieMap, Function> functionsImpl;
        Items<TrieMap, FunctionT> functionTsImpl;
        Items<TrieMap, Var> variablesImpl;
        Items<TrieMap, Struct> structsImpl;
        Items<TrieMap, StructT> structTsImpl;
        Items<TrieMap, Trait> traitsImpl;
        Items<TrieMap, TraitImpl> traitImplsImpl;
        Items<TrieMap, Enum> enumsImpl;
    };

    class StructBase : public Base
    {
    public:
        StructBase(brain::Brain& kb, CellI& astType, CellI& name, const std::string& nameStr);

        Function& addMethod(const std::string& nameStr);
        void addMethod(Function& method);

    public:
        template <typename... Args>
        StructBase& description(Args&&... args);

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

        CellI& name();

        Base& getSubType(CellI& name);

    protected:
        void addBlock(Block& block);

        Items<Map, Function> methodsImpl;
        Map& methods();
        Map& members();
        Map& subTypes();
        List& memberOf();
    };

    class Struct : public StructBase,
                   public NewT<Struct>
    {
    public:
        Struct(brain::Brain& kb, const std::string& nameStr);
        Struct(brain::Brain& kb, CellI& id);

        CellI& getFullyQualifiedName();
        Struct& resolveTypes(CellI& resolveState);
        CellI& compile(CellI& state);
    };

    class StructT : public StructBase,
                    public NewT<StructT>
    {
    public:
        using StructBase::kb;
        StructT(brain::Brain& kb, const std::string& nameStr);
        StructT(brain::Brain& kb, CellI& id);

        StructT& templateParams(Slot& param);

        template <typename... Args>
        StructT& templateParams(Slot& param, Args&&... args)
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

    class Trait : public StructBase,
                  public NewT<Trait>
    {
    public:
        using StructBase::kb;
        Trait(brain::Brain& kb, const std::string& nameStr);
        Trait(brain::Brain& kb, CellI& id);

        Trait& templateParams(Slot& param);

        template <typename... Args>
        Trait& templateParams(Slot& param, Args&&... args)
        {
            templateParams(param);
            templateParams(std::forward<Args>(args)...);

            return *this;
        }

        Trait& associatedTypes(Slot& param);
        template <typename... Args>
        Trait& associatedTypes(Slot& param, Args&&... args)
        {
            associatedTypes(param);
            associatedTypes(std::forward<Args>(args)...);
            return *this;
        }
    };

    class TraitImpl : public StructBase,
                      public NewT<TraitImpl>
    {
    public:
        using StructBase::kb;
        TraitImpl(brain::Brain& kb, const std::string& nameStr);
        TraitImpl(brain::Brain& kb, CellI& id);

        TraitImpl& templateParams(Slot& param);

        template <typename... Args>
        TraitImpl& templateParams(Slot& param, Args&&... args)
        {
            templateParams(param);
            templateParams(std::forward<Args>(args)...);

            return *this;
        }

        TraitImpl& implementedFor(CellI& structType);

        TraitImpl& associatedTypes(Slot& param);

        template <typename... Args>
        TraitImpl& associatedTypes(Slot& param, Args&&... args)
        {
            associatedTypes(param);
            associatedTypes(std::forward<Args>(args)...);

            return *this;
        }
    };

    class EnumValue : public BaseT<EnumValue>
    {
    public:
        EnumValue(brain::Brain& kb, const std::string& nameStr);
        EnumValue(brain::Brain& kb, const std::string& nameStr, CellI& value);

        CellI& getFullyQualifiedName();
    };

    class TypedEnumValue : public BaseT<TypedEnumValue>
    {
    public:
        TypedEnumValue(brain::Brain& kb, CellI& name, CellI& type);
        TypedEnumValue(brain::Brain& kb, const std::string& nameStr, CellI& type);
        TypedEnumValue(brain::Brain& kb, const std::string& nameStr, CellI& type, CellI& value);

        CellI& getFullyQualifiedName();
    };

    class Enum : public BaseT<Enum>
    {
    public:
        Enum(brain::Brain& kb, CellI& name);
        Enum(brain::Brain& kb, const std::string& nameStr);

        CellI& getFullyQualifiedName();
        Enum& resolveTypes(CellI& resolveState);
        CellI& compile(CellI& state);

        Enum& values(Base& value);

        template <class T, class... Args>
        Enum& values(T& value, Args&&... args)
#ifdef __cpp_concepts
            requires EnumValueConcept<T, EnumValue, TypedEnumValue>
#endif
        {
            values(static_cast<Base&>(value));
            values(std::forward<Args>(args)...);
            return *this;
        }

    private:
        CellI& resolveEnumValue(CellI& ast);
        TrieMap& values();
    };

    class Function : public BaseT<Function>
    {
    public:
        Function(brain::Brain& kb, CellI& name);
        Function(brain::Brain& kb, const std::string& nameStr);

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
        void instructions(Args&&... args);

        template <typename... Args>
        void description(Args&&... args);

        Ast::Function& resolveTypes(CellI& resolveState);
        CellI& compile(CellI& state);
        std::string shortName();
        CellI& getFullyQualifiedName();

    protected:
        Ast::Base& resolveTypesInCode(CellI& resolveState, CellI& ast);
        void addBlock(Block& block);
        void compileParams(cells::Object& function, cells::Map& subTypesMap, CellI& state);
        CellI& compileAst(CellI& ast, cells::Object& function, CellI& state);
        void checkMethodCall(CellI& astType, CellI& astMethodId, CellI& state);
        List& parameters();
        CellI& returnType();
        Base& instructions();
    };

    class FunctionT : public BaseT<FunctionT>
    {
    public:
        FunctionT(brain::Brain& kb, CellI& name, const std::string& label);
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
        Base& instructions();
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
        If(brain::Brain& kb, Base& condition);
        If(brain::Brain& kb, Base& condition, Base& thenBranch);
        If(brain::Brain& kb, Base& condition, Base& thenBranch, Base& elseBranch);

        If& then_(Base& thenBranch);
        If& else_(Base& elseBranch);
    };
    class Match : public BaseT<Match>
    {
    public:
        Match(brain::Brain& kb, Base& enum_);

        Match& case_(CellI& memberName, Base& op);
        Match& case_(const std::string& memberStr, Base& op);
        Match& default_(Base& value);
    };
    class Do : public BaseT<Do>
    {
    public:
        Do(brain::Brain& kb, Base& statement);
        Do& while_(Base& condition);
    };
    class While : public BaseT<While>
    {
    public:
        While(brain::Brain& kb, Base& condition);
        While& do_(Base& statement);
    };
    class For : public BaseT<For>
    {
    public:
        For(brain::Brain& kb, const std::string& varName);
        For& in(Base& container);
        For& operator()(Base& statement);
    };
    class Var : public BaseT<Var>
    {
    public:
        Var(brain::Brain& kb, const std::string& nameStr);
        Var(brain::Brain& kb, CellI& name);

        CellI& getFullyQualifiedName();
        Set& operator=(Base& value);
        Get& operator*();
        Call& operator()(const std::string& method);
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
        Call& operator()(const std::string& method);
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
    class AssociatedType : public BaseT<AssociatedType>
    {
    public:
        AssociatedType(const AssociatedType&) = delete;
        AssociatedType(brain::Brain& kb, CellI& role);
    };
    class New : public BaseT<New>
    {
    public:
        New(brain::Brain& kb, Base& objectType);
        New(brain::Brain& kb, Base& objectType, Base& constructor);

        New& operator()(const std::string& nameStr, CellI& value);
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
        Call& operator()(const std::string& method);
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
    Throw& throw_();
    Throw& throw_(Base& value);
    Try& try_(Base& tryBranch, Base& catchBranch);
    Return& return_();
    Return& return_(Base& value);
    Parameter& parameter(CellI& role);
    Slot& slot(const std::string& role, CellI& type);
    Slot& slot(CellI& role, CellI& type);
    EnumValue& enumValue(const std::string& nameStr);
    EnumValue& enumValue(const std::string& nameStr, CellI& init);
    TypedEnumValue& typedEnumValue(const std::string& nameStr, CellI& value);
    TypedEnumValue& typedEnumValue(const std::string& nameStr, CellI& type, CellI& value);
    Call& call(CellI& object, const std::string& method);
    Call& call(CellI& object, CellI& method);
    StaticCall& scall(CellI& type, const std::string& method);
    StaticCall& scall(CellI& type, CellI& method);

    template <typename... Args>
    Block& block(Args&&... args);
    Delete& delete_(Base& cell);
    Set& set(Base& cell, Base& role, Base& value);
    Set& set(Base& cell, const std::string& role, Base& value);
    Erase& erase(Base& cell, Base& role);
    Erase& erase(Base& cell, const std::string& role);
    If& if_(Base& condition);
    Match& match_(Base& enum_);
    Do& do_(Base& condition);
    While& while_(Base& condition);
    For& for_(const std::string& varName);
    Var& var(CellI& name);
    Var& var(const std::string& nameStr);
    Member& member(CellI& role);
    SubType& subType(CellI& role);
    TemplatedType& templatedType(const std::string& id, CellI& type);
    template <typename... Args>
    TemplatedType& templatedType(const std::string& id, const std::string& role, CellI& type, Args&&... args);
    template <typename... Args>
    TemplatedType& templatedType(const std::string& id, const std::string& role, const std::string& type, Args&&... args);

    TemplateParam& templateParam(CellI& role);
    AssociatedType& associatedType(CellI& role);
    New& new_(Base& objectType);
    New& new_(Base& objectType, const std::string& constructor);
    New& new_(Base& objectType, Base& constructor);
    New& new_(const std::string& objectType, const std::string& constructor);
    Same& same(Base& lhs, Base& rhs);
    NotSame& notSame(Base& lhs, Base& rhs);
    Equal& equal(Base& lhs, Base& rhs);
    NotEqual& notEqual(Base& lhs, Base& rhs);
    Has& has(Base& cell, Base& role);
    Has& has(Base& cell, const std::string& role);
    Missing& missing(Base& cell, Base& role);
    Missing& missing(Base& cell, const std::string& id);
    Get& get(Base& cell, const std::string& role);
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
    List up;
    List down;
    List left;
    List right;
};

class Coordinates
{
public:
    Coordinates(brain::Brain& kb);
    List x;
    List y;
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


class AstHelper : public Ast
{
public:
    AstHelper(brain::Brain& kb);

protected:
    CellI& name(const std::string& str);
    Ast::Cell& _(CellI& cell);
    Ast::Cell& _(const std::string& id);
    Ast::Cell& true_();
    Ast::Cell& false_();
    Ast::Parameter& p_(const std::string& nameStr);
    Ast::Slot& p_(const std::string& nameStr, CellI& value);
    Ast::Slot& parameter(const std::string& nameStr, CellI& value);
    Ast::Member& m_(const std::string& nameStr);
    Ast::Var& var_(const std::string& nameStr);
    Ast::Slot& member(const std::string& nameStr, const std::string& type);
    Ast::Slot& member(const std::string& nameStr, CellI& type);
    Ast::EnumValue& ev_(const std::string& nameStr);
    Ast::EnumValue& ev_(const std::string& nameStr, CellI& value);
    Ast::TypedEnumValue& tev_(const std::string& nameStr, CellI& type);
    Ast::TypedEnumValue& tev_(const std::string& nameStr, const std::string& typeStr);
    Ast::TypedEnumValue& tev_(const std::string& nameStr, CellI& type, CellI& value);
    Ast::TypedEnumValue& tev_(const std::string& nameStr, const std::string& typeStr, CellI& value);
    Ast::TypedEnumValue& tev_(const std::string& nameStr, CellI& type, const std::string& valueStr);
    Ast::TypedEnumValue& tev_(const std::string& nameStr, const std::string& typeStr, const std::string& valueStr);
    template <typename... Args>
    Ast::SubType& st_(const std::string& nameStr, Args&&... args);
    Ast::TemplateParam& tp_(const std::string& name);
    Ast::AssociatedType& at_(const std::string& name);
    template <typename... Args>
    Ast::TemplatedType& tt_(const std::string& name, Args&&... args);
    Ast::StructName& struct_(const std::string& name);
    CellI& ListOf(CellI& type);
    CellI& MapOf(CellI& keyType, CellI& valueType);
    template <typename... Args>
    List& list(CellI& value, Args&&... args);

    template <typename... Args>
    Map& map(CellI& key, CellI& value, Args&&... args);

    Ast::Scope& globalScope;
    ID& ids;
    Std& std;
    Directions& directions;
    Coordinates& coordinates;
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
};

template <typename... Args>
Ast::SubType& AstHelper::st_(const std::string& nameStr, Args&&... args)
{
    return subType(name(nameStr), std::forward<Args>(args)...);
}

template <typename... Args>
Ast::TemplatedType& AstHelper::tt_(const std::string& nameStr, Args&&... args)
{
    return templatedType(nameStr, std::forward<Args>(args)...);
}


template <typename... Args>
List& AstHelper::list(CellI& value, Args&&... args)
{
    List& ret = *new List(kb, value.struct_());
    ret.add(value);
    if constexpr (sizeof...(Args) > 0) {
        ret.add(std::forward<Args>(args)...);
    }

    return ret;
}

template <typename... Args>
Map& AstHelper::map(CellI& key, CellI& value, Args&&... args)
{
    Map& ret = *new Map(kb, key.struct_(), value.struct_(), fmt::format("Map<{}, {}>(...)", key.struct_().label(), value.struct_().label()));
    if constexpr (sizeof...(Args) > 0) {
        ret.add(std::forward<Args>(args)...);
    }

    return ret;
}

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

#ifdef __cpp_concepts
template <typename T>
concept is_string = std::is_convertible<T, std::string>::value or std::is_bounded_array_v<char[]>;

template <typename T>
concept is_cell = std::is_base_of<T, CellI>::value;

template <typename T>
concept string_or_cell = is_string<T> or is_cell<T>;
#endif

class Brain
{
public:
    class Logger
    {
    public:
        Logger(std::function<void()> loggerLevelInit);
        ~Logger();
        static std::shared_ptr<spdlog::logger> createLogger(const std::string& name);

    private:
        void registerLogger(const std::string& name);
        std::vector<std::string> m_loggerNames;
    };

    enum class InitPhase
    {
        Init,
        Compiling,
        FullyConstructed,
        DestructBegin
    };

protected:
    InitPhase m_initPhase = InitPhase::Init;
    friend class Std;
    void createContent();
    void createTests();

public:
    Ast::Cell& _(CellI& cell);
    Ast::Cell& _(const std::string& id);
    template <typename... Args>
    Ast::TemplatedType& tt_(const std::string& name, Args&&... args);
    Ast::StructName& struct_(const std::string& name);

public:
    Brain(std::function<void()> loggerLevelInit = []() {});
    ~Brain();

    Logger logger;
    Pools pools;
    ID ids;
    Ast::Scope globalScope;
    Std std;
    Ast ast;
    Directions directions;
    Coordinates coordinates;
    Boolean boolean;
    Numbers numbers;

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

    CellI* compiledGlobalScopePtr = nullptr;

public:
    CellI& getStruct(const std::string& nameStr);
    CellI& getStruct(CellI& name);
    CellI& getVariable(const std::string& nameStr);
    CellI& getVariable(CellI& name);
    void registerBuiltInStruct(const std::string& fullName, CellI& compiledStruct);
    CellI& name(const std::string& str);
    template <typename... Args>
    CellI& templateId(const std::string& str, Args&&... args);
    CellI& ListOf(CellI& type);
    CellI& MapOf(CellI& keyType, CellI& valueType);

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
Ast::TemplatedType& Brain::tt_(const std::string& nameStr, Args&&... args)
{
    return ast.templatedType(nameStr, std::forward<Args>(args)...);
}

template <typename... Args>
CellI& Brain::templateId(const std::string& nameStr, Args&&... args)
{
    List& idCell = *new List(*this, std.Cell);
    for (const auto& character : nameStr) {
        idCell.add(pools.chars.get(character));
    }
    idCell.add(std::forward<Args>(args)...);

    return idCell;
}

template <typename... Args>
List& Brain::list(CellI& value, Args&&... args)
{
    List& ret = *new List(*this, value.struct_());
    ret.add(value);
    if constexpr (sizeof...(Args) > 0) {
        ret.add(std::forward<Args>(args)...);
    }

    return ret;
}

template <typename... Args>
Map& Brain::map(CellI& key, CellI& value, Args&&... args)
{
    Map& ret = *new Map(*this, key.struct_(), value.struct_(), fmt::format("Map<{}, {}>(...)", key.struct_().label(), value.struct_().label()));
    if constexpr (sizeof...(Args) > 0) {
        ret.add(std::forward<Args>(args)...);
    }

    return ret;
}

template <typename... Args>
Set& Brain::set(CellI& value, Args&&... args)
{
    Set& ret = *new Set(*this, value.struct_(), fmt::format("Map<{}, {}>(...)", value.struct_().label()));
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
    Map& ret = *new Map(*this, std.Cell, std.Slot, "Map<Cell, Slot>(...)");
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
    auto& ret  = templatedType(id, kb.ast.slot(role, kb.ast.structName(type)));
    if constexpr (sizeof...(Args) > 0) {
        ret.addParam(std::forward<Args>(args)...);
    }
    return ret;
}

template <typename... Args>
Ast::StructBase& Ast::StructBase::description(Args&&... args)
{
    addBlock(*new Block(kb, kb.list(std::forward<Args>(args)...)));
    return *this;
}

template <typename... Args>
void Ast::Function::instructions(Args&&... args)
{
    addBlock(*new Block(kb, kb.list(std::forward<Args>(args)...)));
}
#pragma endregion

template <class MapType, class TAst>
Ast::Items<MapType, TAst>::Items(brain::Brain& kb, const std::string& mapName, Base& parent) :
    kb(kb),
    m_mapName(mapName),
    m_parent(parent)
{
}

template <class MapType, class TAst>
bool Ast::Items<MapType, TAst>::has(CellI& id)
{
    if (m_parent.missing(m_mapName)) {
        return false;
    }

    return items().hasKey(id);
}

template <class MapType, class TAst>
TAst& Ast::Items<MapType, TAst>::get(const std::string& nameStr)
{
    return get(kb.name(nameStr));
}

template <class MapType, class TAst>
TAst& Ast::Items<MapType, TAst>::get(CellI& name)
{
    if (m_parent.missing(m_mapName)) {
        throw "No such scope";
    }

    if (items().hasKey(name)) {
        return static_cast<TAst&>(items().getValue(name));
    }

    throw "No such scope";
}

template <class MapType, class TAst>
TAst& Ast::Items<MapType, TAst>::add(const std::string& nameStr)
{
    TAst& ast = *new TAst(kb, nameStr);
    add(ast);

    return ast;
}

template <class MapType, class TAst>
void Ast::Items<MapType, TAst>::add(TAst& obj)
{
    CellI& name = obj[kb.ids.name];

    if (m_parent.missing(m_mapName)) {
        m_parent.set(m_mapName, *new TrieMap(kb, kb.std.Cell, kb.std.ast.Base, "TrieMap<Cell, Type::Ast::Base>(...)"));
    }
    if (items().hasKey(name)) {
        throw "Already registered!";
    }

    items().add(name, obj);
    obj.set("scope", m_parent);
}

template <class MapType, class TAst>
MapType& Ast::Items<MapType, TAst>::items()
{
    if (m_parent.missing(m_mapName)) {
        throw fmt::format("No {}!", m_mapName);
    } else {
        return static_cast<TrieMap&>(m_parent.get(m_mapName));
    }
}


} // namespace brain
} // namespace cells
} // namespace infocell
