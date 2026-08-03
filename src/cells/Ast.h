#pragma once
#include <fmt/core.h>
#include <functional>

#include "Cells.h"

namespace infocell {
namespace cells {

class World;
class ToolFinder;

template <class T, class EnumValue, class TypedEnumValue>
concept EnumValueConcept = std::is_same<T, EnumValue>::value || std::is_same<T, TypedEnumValue>::value;

// ============================================================================
class Ast
{
public:
    template<class T>
    using NewT = util::NewT<T>;

    class Base : public Object
    {
        Base(const Base&) = delete;
    protected:
        Base(World& w, CellI& classCell, const std::string& label = "");
    };

    class Equal;
    template <typename T>
    class BaseT : public Base,
                  public NewT<T>
    {
    protected:
        BaseT(World& w, CellI& classCell, const std::string& label) :
            Base(w, classCell, label)
        {
        }

    public:
        Equal& operator==(Base& rhs) const
        {
            return Equal::New(w, const_cast<BaseT<T>&>(*this), rhs);
        }
        Equal& operator!=(Base& rhs) const;
    };

// ============================================================================
    class AssociatedType : public BaseT<AssociatedType>
    {
    public:
        AssociatedType(const AssociatedType&) = delete;
        AssociatedType(World& w, CellI& key);
    };

    class Block : public BaseT<Block>
    {
    public:
        Block(World& w, List& list);
        void mergeFrom(Base& statement);
    };

    class Break : public BaseT<Break>
    {
    public:
        Break(World& w);
    };

    class Call : public BaseT<Call>
    {
    public:
        Call(World& w, CellI& method);
        Call(World& w, CellI& self, CellI& method);
        Call& operator/(Base& key);
        Call& operator/(const std::string& key);
        Call& operator()(const std::string& method);
        Call& operator()(const std::string& nameStr, CellI& value);
        Call& operator()(const std::string& nameStr, const std::string& valueStr);

        List& parameters();
    };

    class Continue : public BaseT<Continue>
    {
    public:
        Continue(World& w);
    };

    class ConstVar : public BaseT<ConstVar>
    {
    public:
        ConstVar(World& w, CellI& value);
        Call& operator/(Base& key);
        Call& operator/(const std::string& key);
    };

    class Do : public BaseT<Do>
    {
    public:
        Do(World& w, Base& statement);
        Do& while_(Base& condition);
    };

    class EnumValue : public BaseT<EnumValue>
    {
    public:
        EnumValue(World& w, const std::string& nameStr);
        EnumValue(World& w, const std::string& nameStr, CellI& value);
    };

    class For : public BaseT<For>
    {
    public:
        For(World& w, Base& var);
        For& in(Base& container);
        For& operator()(Base& statement);
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

    class KVPair : public BaseT<KVPair>
    {
    public:
        KVPair(World& w, CellI& key, CellI& value);
    };

    class Match : public BaseT<Match>
    {
    public:
        Match(World& w, Base& enum_);

        Match& case_(CellI& memberName, Base& op);
        Match& case_(const std::string& memberStr, Base& op);
        Match& default_(Base& value);
    };

    class Missing;
    class Set;
    class Member : public BaseT<Member>
    {
    public:
        Member(const Member&) = delete;
        Member(World& w, CellI& key);
        Member(World& w, CellI& key, CellI& type);
        Call& operator=(Base& value);
        Call& operator/(Base& key);
        Call& operator/(const std::string& key);
        Call& exist();
        Call& missing();
        Call& operator()(const std::string& method);
    };

    class New : public BaseT<New>
    {
    public:
        New(World& w, Base& objectType);
        New(World& w, Base& objectType, Base& constructor);

        New& operator()(const std::string& nameStr, CellI& value);
        List& parameters();
    };

    class Parameter : public BaseT<Parameter>
    {
    public:
        Parameter(World& w, CellI& key);
        Call& operator/(Base& key);
        Call& operator/(const std::string& key);
        Call& operator()(const std::string& method);
    };

    class PrimitiveToolName : public BaseT<PrimitiveToolName>
    {
    public:
        PrimitiveToolName(World& w, CellI& name);
    };

    class Return : public BaseT<Return>
    {
    public:
        Return(World& w);
        Return(World& w, CellI& value);
        Call& operator()(const std::string& method);
    };

    class ResolvedType : public BaseT<ResolvedType>
    {
    public:
        ResolvedType(World& w, CellI& astType, CellI& compiledType);
    };

    class Self : public BaseT<Self>
    {
    public:
        Self(World& w);
        Call& operator()(const std::string& method);
    };

    class SelfType : public BaseT<SelfType>
    {
    public:
        SelfType(World& w);
    };

    class Slot : public BaseT<Slot>
    {
    public:
        Slot(World& w, CellI& key);
        Slot(World& w, CellI& key, CellI& value);
    };

    class TypeName : public BaseT<TypeName>
    {
    public:
        TypeName(World& w, CellI& name);
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

    class TypeAlias : public BaseT<TypeAlias>
    {
    public:
        TypeAlias(const TypeAlias&) = delete;
        TypeAlias(World& w, CellI& name);
    };

    class TypedEnumValue : public BaseT<TypedEnumValue>
    {
    public:
        TypedEnumValue(World& w, CellI& name, CellI& type);
        TypedEnumValue(World& w, const std::string& nameStr, CellI& type);
        TypedEnumValue(World& w, const std::string& nameStr, CellI& type, CellI& value);
    };

    class UnknownVar : public BaseT<UnknownVar>
    {
    public:
        UnknownVar(World& w, CellI& value);

        Call& operator/(Base& key);
        Call& operator/(const std::string& key);
    };

    class Var : public BaseT<Var>
    {
    public:
        Var(World& w, const std::string& nameStr);
        Var(World& w, CellI& name);

        Call& operator=(Base& value);
        Call& operator*();
        Call& operator()(const std::string& method);
    };

    class While : public BaseT<While>
    {
    public:
        While(World& w, Base& condition);
        While& do_(Base& statement);
    };

    // ============================================================================
    class Function;
    class TraitImpl;
    class StructBase : public Base
    {
    public:
        enum class ParameterModification
        {
            AddSelfParam,
            NoChange
        };
        StructBase(World& w, CellI& astType, CellI& name, const std::string& nameStr);

        Function& addPrimitiveFunction(CellI& ast, CellI& op, const std::string& nameStr);
        Function& addMethod(const std::string& nameStr);
        void addMethod(Function& method, ParameterModification parameterModification = ParameterModification::AddSelfParam);

        TraitImpl& addTraitImpl(const std::string& nameStr);
        void addTraitImpl(TraitImpl& traitImpl);

    public:
        StructBase& returnType(CellI& type);

        void addMember(Member& member);
        template <typename... Args>
        StructBase& members(Member& member, Args&&... args)
        {
            addMember(member);
            members(std::forward<Args>(args)...);
            return *this;
        }

        StructBase& typeAliases(Slot& param);
        template <typename... Args>
        StructBase& typeAliases(Slot& param, Args&&... args)
        {
            typeAliases(param);
            typeAliases(std::forward<Args>(args)...);
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

        Base& getTypeAlias(CellI& name);
        Map& methods();
        Map& traitImpls();
        Map& members();
        Map& typeAliases();
        List& memberOf();
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

        Function& memberMapping(KVPair& mapping);
        template <typename... Args>
        Function& memberMapping(KVPair& param, Args&&... args)
        {
            memberMapping(param);
            memberMapping(std::forward<Args>(args)...);

            return *this;
        }

        void addParameter(Parameter& parameter);

        template <typename... Args>
        Function& parameters(Parameter& parameter, Args&&... args)
        {
            addParameter(parameter);
            parameters(std::forward<Args>(args)...);

            return *this;
        }

        Function& constructor();
        Function& returnType(CellI& type);
        Function& returnType(const std::string& typeStr);

        template <typename... Args>
        void instructions(Args&&... args);

        template <typename... Args>
        Function& description(Args&&... args);

        Map& memberMapping();
        Map& parameters();
        CellI& returnType();
        Base& instructions();
        Base& description();

    protected:
        void addDescriptionBlock(Block& block);
        void addInstructionBlock(Block& block);
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

        StructT& templateParams(Parameter& parameter);

        template <typename... Args>
        StructT& templateParams(Parameter& parameter, Args&&... args)
        {
            templateParams(parameter);
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

        Trait& templateParams(Parameter& parameter);

        template <typename... Args>
        Trait& templateParams(Parameter& parameter, Args&&... args)
        {
            templateParams(parameter);
            templateParams(std::forward<Args>(args)...);

            return *this;
        }

        Trait& associatedTypes(Parameter& parameter);
        template <typename... Args>
        Trait& associatedTypes(Parameter& parameter, Args&&... args)
        {
            associatedTypes(parameter);
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

        TraitImpl& associatedTypes(Parameter& parameter);

        template <typename... Args>
        TraitImpl& associatedTypes(Parameter& parameter, Args&&... args)
        {
            associatedTypes(parameter);
            associatedTypes(std::forward<Args>(args)...);

            return *this;
        }
        Map& associatedTypes();
    };

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
        Items<TrieMap, Enum> enumsImpl;
    };

// ============================================================================
    Ast(World& w);

    Call& add(Base& lhs, Base& rhs);
    Call& and_(Base& lhs, Base& rhs);
    AssociatedType& associatedType(CellI& key);
    template <typename... Args>
    Block& block(Args&&... args);
    Break& break_();
    Call& call(CellI& method);
    Call& call(CellI& object, CellI& method);
    Call& call(CellI& object, const std::string& method);
    ConstVar& const_(CellI& value);
    ConstVar& const_(const std::string& id);
    ConstVar& const_(int number);
    Continue& continue_();
    Call& delete_(Base& cell);
    Call& divide(Base& lhs, Base& rhs);
    Do& do_(Base& condition);
    EnumValue& enumValue(const std::string& nameStr);
    EnumValue& enumValue(const std::string& nameStr, CellI& init);
    Call& equal(CellI& lhs, Base& rhs);
    Call& erase(Base& cell, Base& key);
    Call& erase(Base& cell, const std::string& key);
    For& for_(Base& var);
    For& for_(const std::string& varName);
    Call& get(Base& cell, Base& key);
    Call& get(Base& cell, const std::string& key);
    Call& greaterThan(Base& lhs, Base& rhs);
    Call& greaterThanOrEqual(Base& lhs, Base& rhs);
    Call& has(Base& cell, Base& key);
    Call& has(Base& cell, const std::string& key);
    If& if_(Base& condition);
    KVPair& kvPair(CellI& key, CellI& value);
    KVPair& kvPair(const std::string& keyStr, CellI& value);
    KVPair& kvPair(CellI& key, const std::string& valueStr);
    KVPair& kvPair(const std::string& keyStr, const std::string& valueStr);
    Call& lessThan(Base& lhs, Base& rhs);
    Call& lessThanOrEqual(Base& lhs, Base& rhs);
    Match& match_(Base& enum_);
    Member& member(CellI& key);
    Member& member(CellI& key, CellI& type);
    Member& member(const std::string& nameStr, const std::string& typeStr);
    Member& member(const std::string& nameStr, CellI& type);
    Call& missing(Base& cell, Base& key);
    Call& missing(Base& cell, const std::string& id);
    Call& multiply(Base& lhs, Base& rhs);
    New& new_(Base& objectType);
    New& new_(Base& objectType, Base& constructor);
    New& new_(Base& objectType, const std::string& constructor);
    New& new_(const std::string& objectType, const std::string& constructor);
    Call& not_(Base& input);
    Call& notEqual(Base& lhs, Base& rhs);
    Call& notSame(Base& lhs, Base& rhs);
    Call& or_(Base& lhs, Base& rhs);
    Parameter& parameter(CellI& key);
    Parameter& parameterDeclaration(CellI& key, CellI& type);
    Parameter& parameterInit(CellI& key, CellI& value);
    PrimitiveToolName& primitiveToolName(CellI& id);
    Return& return_();
    Return& return_(CellI& value);
    Call& same(Base& lhs, Base& rhs);
    Self& self();
    Self& self(CellI& value);
    SelfType& selfType();
    Call& set(Base& cell, Base& key, Base& value);
    Call& set(Base& cell, const std::string& key, Base& value);
    Slot& slot(CellI& key, CellI& type);
    Slot& slot(const std::string& key, CellI& type);
    TypeName& typeName(CellI& id);
    TypeName& typeName(const std::string& idStr);
    Call& subtract(Base& lhs, Base& rhs);
    TemplatedType& templatedType(const std::string& id, CellI& type);
    template <typename... Args>
    TemplatedType& templatedType(const std::string& id, const std::string& key, CellI& type, Args&&... args);
    template <typename... Args>
    TemplatedType& templatedType(const std::string& id, const std::string& key, const std::string& type, Args&&... args);
    TemplateParam& templateParam(CellI& key);
    TypeAlias& typeAlias(CellI& key);
    TypedEnumValue& typedEnumValue(const std::string& nameStr, CellI& type, CellI& value);
    TypedEnumValue& typedEnumValue(const std::string& nameStr, CellI& value);
    UnknownVar& unknown_(CellI& value);
    Var& var_(CellI& name);
    Var& var_(const std::string& nameStr);
    While& while_(Base& condition);

    ConstVar& _(CellI& cell);
    ConstVar& _(const std::string& id);
    ConstVar& _(int number);
    CellI& name(const std::string& str);
    Ast::ConstVar& true_();
    Ast::ConstVar& false_();

protected:
    CellI& processNamespacedName(const std::string& inputName, std::function<CellI&(const std::string& outName)> createCb);
    World& w;
};

class ID;
class Std;
class Arc;

// ============================================================================
class AstHelper : public Ast
{
public:
    AstHelper(World& w);

protected:
    Ast::Parameter& p_(const std::string& nameStr);
    Ast::Parameter& p_(const std::string& nameStr, CellI& value);
    Ast::Parameter& parameter(const std::string& nameStr, const std::string& typeStr);
    Ast::Parameter& parameter(const std::string& nameStr, CellI& type);
    Ast::Member& m_(const std::string& nameStr);
    Ast::Slot& typeAlias(const std::string& nameStr, const std::string& typeStr);
    Ast::Slot& typeAlias(const std::string& nameStr, CellI& type);
    Ast::EnumValue& ev_(const std::string& nameStr);
    Ast::EnumValue& ev_(const std::string& nameStr, CellI& value);
    Ast::TypedEnumValue& tev_(const std::string& nameStr, CellI& type);
    Ast::TypedEnumValue& tev_(const std::string& nameStr, const std::string& typeStr);
    Ast::TypedEnumValue& tev_(const std::string& nameStr, CellI& type, CellI& value);
    Ast::TypedEnumValue& tev_(const std::string& nameStr, const std::string& typeStr, CellI& value);
    Ast::TypedEnumValue& tev_(const std::string& nameStr, CellI& type, const std::string& valueStr);
    Ast::TypedEnumValue& tev_(const std::string& nameStr, const std::string& typeStr, const std::string& valueStr);
    template <typename... Args>
    Ast::TypeAlias& ta_(const std::string& nameStr, Args&&... args);
    Ast::TemplateParam& tp_(const std::string& nameStr);
    Ast::AssociatedType& at_(const std::string& nameStr);
    template <typename... Args>
    Ast::TemplatedType& tt_(const std::string& nameStr, Args&&... args);
    Ast::TypeName& __type__(const std::string& nameStr);
    CellI& ListOf(CellI& type);
    CellI& MapOf(CellI& keyType, CellI& valueType);

    template <typename... Args>
    List& list(CellI& value, Args&&... args);

    template <typename... Args>
    Map& map(CellI& key, CellI& value, Args&&... args);

    ID& id;
    Std& std;
    Arc& arc;

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
