#pragma once
#include <fmt/core.h>
#include <functional>

#include "Cells.h"

namespace infocell {
namespace cells {

class World;
class ToolFinder;

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

template <class T, class EnumValue, class TypedEnumValue>
concept EnumValueConcept = std::is_same<T, EnumValue>::value || std::is_same<T, TypedEnumValue>::value;

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
        Base(World& w, CellI& classCell, const std::string& label = "");
    };

    class Equal;

    template <typename T>
    class BaseT : public Base,
                  public NewT<T>
    {
    public:
        BaseT(World& w, CellI& classCell, const std::string& label) :
            Base(w, classCell, label)
        {
        }
        Equal& operator==(Base& rhs) const
        {
            return Equal::New(w, const_cast<BaseT<T>&>(*this), rhs);
        }
        Equal& operator!=(Base& rhs) const;
    };

    class Get;
    class Cell : public BaseT<Cell>
    {
    public:
        Cell(World& w, CellI& value);
        Get& operator/(Base& key);
        Get& operator/(const std::string& key);
    };
    class StructName : public BaseT<StructName>
    {
    public:
        StructName(World& w, CellI& name);
    };
    class Call;
    class Self : public BaseT<Self>
    {
    public:
        Self(World& w);
        Call& operator()(const std::string& method);
    };
    class SelfFn : public BaseT<SelfFn>
    {
    public:
        SelfFn(World& w);
    };
    class Continue : public BaseT<Continue>
    {
    public:
        Continue(World& w);
    };
    class Break : public BaseT<Break>
    {
    public:
        Break(World& w);
    };
    class Try : public BaseT<Try>
    {
    public:
        Try(World& w, Base& tryBranch, Base& catchBranch);
    };
    class Throw : public BaseT<Throw>
    {
    public:
        Throw(World& w);
        Throw(World& w, Base& value);
    };
    class Return : public BaseT<Return>
    {
    public:
        Return(World& w);
        Return(World& w, CellI& value);
    };
    class Parameter : public BaseT<Parameter>
    {
    public:
        Parameter(World& w, CellI& key);
        Get& operator/(Base& key);
        Get& operator/(const std::string& key);
        Call& operator()(const std::string& method);
    };
    class ResolvedType : public BaseT<ResolvedType>
    {
    public:
        ResolvedType(World& w, CellI& astType, CellI& compiledType);
    };
    class Slot : public BaseT<Slot>
    {
    public:
        Slot(World& w, CellI& key, CellI& value);
    };
    class Call : public BaseT<Call>
    {
    public:
        Call(World& w, CellI& cell, CellI& method);

        Call& operator()(const std::string& nameStr, CellI& value);
    };

    class StaticCall : public BaseT<StaticCall>
    {
    public:
        StaticCall(World& w, CellI& cell, CellI& method);

        StaticCall& operator()(const std::string& nameStr, CellI& value);
    };

    class Block : public BaseT<Block>
    {
    public:
        Block(World& w, List& list);
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
        Items(World& w, const std::string& mapName, Base& parent);

        bool has(CellI& id);
        TAst& get(const std::string& nameStr);
        TAst& get(CellI& name);
        TAst& add(const std::string& nameStr);
        void add(TAst& obj);
        MapType& items();

        World& w;
        const std::string m_mapName;
        Base& m_parent;
    };

    class Scope : public BaseT<Scope>
    {
    public:
        enum class MergeMode
        {
            Link,
            Copy
        };

        Scope(World& w, const std::string& nameStr);

        Scope& getRootScope();
        Scope& createLink();
        Scope* getLinkedScope();
        void mergeTo(Scope& targetScope, MergeMode mergeMode);

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

        ToolFinder* m_toolFinder = nullptr;

    protected:

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
        StructBase(World& w, CellI& astType, CellI& name, const std::string& nameStr);

        Function& addMethod(const std::string& nameStr);
        void addMethod(Function& method);

    public:
        StructBase& primitiveTool();
        StructBase& returnType(CellI& type);

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
        Map& methods();
        Map& members();
        Map& subTypes();
        List& memberOf();

    protected:
        void addBlock(Block& block);

        Items<Map, Function> methodsImpl;
    };

    class Struct : public StructBase,
                   public NewT<Struct>
    {
    public:
        Struct(World& w, const std::string& nameStr);
        Struct(World& w, CellI& id);
    };

    class StructT : public StructBase,
                    public NewT<StructT>
    {
    public:
        using StructBase::w;
        StructT(World& w, const std::string& nameStr);
        StructT(World& w, CellI& id);

        StructT& templateParams(Slot& param);

        template <typename... Args>
        StructT& templateParams(Slot& param, Args&&... args)
        {
            templateParams(param);
            templateParams(std::forward<Args>(args)...);

            return *this;
        }

        Map& templateParams();
    };

    class Trait : public StructBase,
                  public NewT<Trait>
    {
    public:
        using StructBase::w;
        Trait(World& w, const std::string& nameStr);
        Trait(World& w, CellI& id);

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
        using StructBase::w;
        TraitImpl(World& w, const std::string& nameStr);
        TraitImpl(World& w, CellI& id);

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
        EnumValue(World& w, const std::string& nameStr);
        EnumValue(World& w, const std::string& nameStr, CellI& value);
    };

    class TypedEnumValue : public BaseT<TypedEnumValue>
    {
    public:
        TypedEnumValue(World& w, CellI& name, CellI& type);
        TypedEnumValue(World& w, const std::string& nameStr, CellI& type);
        TypedEnumValue(World& w, const std::string& nameStr, CellI& type, CellI& value);
    };

    class Enum : public StructBase,
                 public NewT<Struct>
    {
    public:
        Enum(World& w, CellI& name);
        Enum(World& w, const std::string& nameStr);

        Enum& values(Base& value);

        template <class T, class... Args>
        Enum& values(T& value, Args&&... args)
            requires EnumValueConcept<T, EnumValue, TypedEnumValue>
        {
            values(static_cast<Base&>(value));
            values(std::forward<Args>(args)...);
            return *this;
        }
        TrieMap& values();

    private:
        CellI& resolveEnumValue(CellI& ast);
    };

    class Function : public BaseT<Function>
    {
    public:
        Function(World& w, CellI& name);
        Function(World& w, const std::string& nameStr);

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

        List& parameters();
        CellI& returnType();
        Base& instructions();

    protected:
        void addBlock(Block& block);
    };

    class FunctionT : public BaseT<FunctionT>
    {
    public:
        FunctionT(World& w, CellI& name, const std::string& label);
        FunctionT(World& w, const std::string& name);

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

    protected:
        void addBlock(Block& block);
        List& parameters();
        CellI& returnType();
        Base& instructions();
    };

    class Delete : public BaseT<Delete>
    {
    public:
        Delete(World& w, Base& cell);
    };
    class Set : public BaseT<Set>
    {
    public:
        Set(World& w, Base& cell, Base& key, Base& value);
    };
    class Erase : public BaseT<Erase>
    {
    public:
        Erase(World& w, Base& cell, Base& key);
    };
    class If : public BaseT<If>
    {
    public:
        If(World& w, Base& condition);
        If(World& w, Base& condition, Base& thenBranch);
        If(World& w, Base& condition, Base& thenBranch, Base& elseBranch);

        If& then_(Base& thenBranch);
        If& else_(Base& elseBranch);
    };
    class Match : public BaseT<Match>
    {
    public:
        Match(World& w, Base& enum_);

        Match& case_(CellI& memberName, Base& op);
        Match& case_(const std::string& memberStr, Base& op);
        Match& default_(Base& value);
    };
    class Do : public BaseT<Do>
    {
    public:
        Do(World& w, Base& statement);
        Do& while_(Base& condition);
    };
    class While : public BaseT<While>
    {
    public:
        While(World& w, Base& condition);
        While& do_(Base& statement);
    };
    class For : public BaseT<For>
    {
    public:
        For(World& w, const std::string& varName);
        For& in(Base& container);
        For& operator()(Base& statement);
    };
    class Var : public BaseT<Var>
    {
    public:
        Var(World& w, const std::string& nameStr);
        Var(World& w, CellI& name);

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
        Member(World& w, CellI& key);
        Set& operator=(Base& value);
        Get& operator/(Base& key);
        Get& operator/(const std::string& key);
        Has& exist();
        Missing& missing();
        Call& operator()(const std::string& method);
    };
    class SubType : public BaseT<SubType>
    {
    public:
        SubType(const SubType&) = delete;
        SubType(World& w, CellI& name);
    };
    class TemplatedType : public BaseT<TemplatedType>
    {
    public:
        TemplatedType(const TemplatedType&) = delete;
        TemplatedType(World& w, CellI& id, CellI& typeList);

        void addParam(const std::string& key, CellI& type);
        void addParam(const std::string& key, const std::string& type);

        template <typename... Args>
        void addParam(const std::string& key, CellI& type, Args&&... args)
        {
            addParam(key, type);
            addParam(std::forward<Args>(args)...);
        }
        template <typename... Args>
        void addParam(const std::string& key, const std::string& type, Args&&... args)
        {
            addParam(key, type);
            addParam(std::forward<Args>(args)...);
        }
    };
    class TemplateParam : public BaseT<TemplateParam>
    {
    public:
        TemplateParam(const TemplateParam&) = delete;
        TemplateParam(World& w, CellI& key);
    };
    class AssociatedType : public BaseT<AssociatedType>
    {
    public:
        AssociatedType(const AssociatedType&) = delete;
        AssociatedType(World& w, CellI& key);
    };
    class New : public BaseT<New>
    {
    public:
        New(World& w, Base& objectType);
        New(World& w, Base& objectType, Base& constructor);

        New& operator()(const std::string& nameStr, CellI& value);
    };
    class Same : public BaseT<Same>
    {
    public:
        Same(World& w, Base& lhs, Base& rhs);
    };
    class NotSame : public BaseT<NotSame>
    {
    public:
        NotSame(World& w, Base& lhs, Base& rhs);
    };
    class Equal : public BaseT<Equal>
    {
    public:
        Equal(World& w, Base& lhs, Base& rhs);
    };
    class NotEqual : public BaseT<NotEqual>
    {
    public:
        NotEqual(World& w, Base& lhs, Base& rhs);
    };
    class Has : public BaseT<Has>
    {
    public:
        Has(World& w, Base& cell, Base& key);
    };
    class Missing : public BaseT<Missing>
    {
    public:
        Missing(World& w, Base& cell, Base& key);
    };
    class Get : public BaseT<Get>
    {
    public:
        Get(World& w, Base& cell, Base& key);
        Get& operator/(Base& key);
        Get& operator/(const std::string& key);
        Call& operator()(const std::string& method);
    };
    class And : public BaseT<And>
    {
    public:
        And(World& w, Base& lhs, Base& rhs);
    };
    class Or : public BaseT<Or>
    {
    public:
        Or(World& w, Base& lhs, Base& rhs);
    };
    class Not : public BaseT<Not>
    {
    public:
        Not(World& w, Base& input);
    };
    class Add : public BaseT<Add>
    {
    public:
        Add(World& w, Base& lhs, Base& rhs);
    };
    class Subtract : public BaseT<Subtract>
    {
    public:
        Subtract(World& w, Base& lhs, Base& rhs);
    };
    class Multiply : public BaseT<Multiply>
    {
    public:
        Multiply(World& w, Base& lhs, Base& rhs);
    };
    class Divide : public BaseT<Divide>
    {
    public:
        Divide(World& w, Base& lhs, Base& rhs);
    };
    class LessThan : public BaseT<LessThan>
    {
    public:
        LessThan(World& w, Base& lhs, Base& rhs);
    };
    class LessThanOrEqual : public BaseT<LessThanOrEqual>
    {
    public:
        LessThanOrEqual(World& w, Base& lhs, Base& rhs);
    };
    class GreaterThan : public BaseT<GreaterThan>
    {
    public:
        GreaterThan(World& w, Base& lhs, Base& rhs);
    };
    class GreaterThanOrEqual : public BaseT<GreaterThanOrEqual>
    {
    public:
        GreaterThanOrEqual(World& w, Base& lhs, Base& rhs);
    };

    Ast(World& w);

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
    Return& return_(CellI& value);
    Parameter& parameter(CellI& key);
    Slot& slot(const std::string& key, CellI& type);
    Slot& slot(CellI& key, CellI& type);
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
    Set& set(Base& cell, Base& key, Base& value);
    Set& set(Base& cell, const std::string& key, Base& value);
    Erase& erase(Base& cell, Base& key);
    Erase& erase(Base& cell, const std::string& key);
    If& if_(Base& condition);
    Match& match_(Base& enum_);
    Do& do_(Base& condition);
    While& while_(Base& condition);
    For& for_(const std::string& varName);
    Var& var(CellI& name);
    Var& var(const std::string& nameStr);
    Member& member(CellI& key);
    SubType& subType(CellI& key);
    TemplatedType& templatedType(const std::string& id, CellI& type);
    template <typename... Args>
    TemplatedType& templatedType(const std::string& id, const std::string& key, CellI& type, Args&&... args);
    template <typename... Args>
    TemplatedType& templatedType(const std::string& id, const std::string& key, const std::string& type, Args&&... args);

    TemplateParam& templateParam(CellI& key);
    AssociatedType& associatedType(CellI& key);
    New& new_(Base& objectType);
    New& new_(Base& objectType, const std::string& constructor);
    New& new_(Base& objectType, Base& constructor);
    New& new_(const std::string& objectType, const std::string& constructor);
    Same& same(Base& lhs, Base& rhs);
    NotSame& notSame(Base& lhs, Base& rhs);
    Equal& equal(Base& lhs, Base& rhs);
    NotEqual& notEqual(Base& lhs, Base& rhs);
    Has& has(Base& cell, Base& key);
    Has& has(Base& cell, const std::string& key);
    Missing& missing(Base& cell, Base& key);
    Missing& missing(Base& cell, const std::string& id);
    Get& get(Base& cell, const std::string& key);
    Get& get(Base& cell, Base& key);
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
    World& w;
};

class ID;
class Std;
class Directions;
class Coordinates;

class AstHelper : public Ast
{
public:
    AstHelper(World& w);

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

} // namespace cells
} // namespace infocell
