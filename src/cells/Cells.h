#pragma once

#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <vector>

namespace infocell {
namespace cells {
class World;

// ============================================================================
class Visitor;
class CellI
{
public:
    CellI(World& w);
    CellI(World& w, const std::string& label);
    CellI(const CellI& rhs);
    virtual ~CellI();

    virtual bool has(CellI& key)               = 0;
    virtual void set(CellI& key, CellI& value) = 0;
    virtual void erase(CellI& key)             = 0;
    virtual void operator()()                  = 0;
    virtual CellI& operator[](CellI& key)      = 0;
    virtual void accept(Visitor& visitor)      = 0;

    bool has(const std::string& key);
    void set(const std::string& key, CellI& value);
    void erase(const std::string& key);
    CellI& operator[](const std::string& key);
    bool missing(const std::string& key);
    CellI& get(const std::string& key);

    bool missing(CellI& key);
    CellI& get(CellI& key);
    CellI& __type__();
    CellI& slotList();
    void eval();

    std::string label() const;
    void label(const std::string& label);

    bool isA(CellI& type);
    bool isA(CellI& cell, CellI& type) const;

    bool operator==(CellI& rhs);
    bool operator!=(CellI& rhs);

    World& w;            // world model
    std::string m_label; // for comments

    static int s_constructed;
    static int s_destructed;
};

struct Param
{
    Param(CellI& key, CellI& value) :
        key(key), value(value) { }

    Param(const std::string& key, CellI& value);

    CellI& key;
    CellI& value;
};

// ============================================================================
class Object : public CellI
{
public:
    Object(World& w, CellI& type, const std::string& label = "");
    Object(World& w, CellI& type, CellI& constructor, const std::string& label = "");
    Object(World& w, CellI& type, CellI& constructor, Param param1, const std::string& label = "");
    Object(World& w, CellI& type, CellI& constructor, Param param1, Param param2, const std::string& label = "");
    Object(World& w, CellI& type, CellI& constructor, Param param1, Param param2, Param param3, const std::string& label = "");
    Object(World& w, CellI& type, CellI& constructor, Param param1, Param param2, Param param3, Param param4, const std::string& label = "");
    ~Object();

    using CellI::has;
    using CellI::set;
    using CellI::erase;
    using CellI::get;
    using CellI::missing;
    using CellI::operator[];

    bool has(CellI& key) override;
    void set(CellI& key, CellI& value) override;
    void erase(CellI& key) override;
    void operator()() override;
    CellI& operator[](CellI& key) override;
    void accept(Visitor& visitor) override;

    CellI& method(const std::string& key);

    CellI& method(CellI& key);
    CellI& method(CellI& key, Param param1);
    CellI& method(CellI& key, Param param1, Param param2);
    CellI& method(CellI& key, Param param1, Param param2, Param param3);
    CellI& method(CellI& key, Param param1, Param param2, Param param3, Param param4);

    CellI& smethod(CellI& key);
    CellI& smethod(CellI& key, Param param1);
    CellI& smethod(CellI& key, Param param1, Param param2);
    CellI& smethod(CellI& key, Param param1, Param param2, Param param3);
    CellI& smethod(CellI& key, Param param1, Param param2, Param param3, Param param4);
    void printIndent();
    void resetIndent();
    void createSelfStack();
    static void clearStack(CellI& method);

protected:
    void destructor();

    bool hasMethod(CellI& key);
    CellI& getMethod(CellI& key);
    CellI& getStaticMethod(CellI& key);
    void createStack(CellI& method);
    void initLocalVars(CellI& method);
    CellI& getFnValue(CellI& fn);

    void setSelf(CellI& fn);
    void setFnParam(CellI& fn, Param param);

    template <typename... Args>
    void setFnParam(CellI& fn, Param param, Args&&... args)
    {
        setFnParam(fn, param);
        setFnParam(fn, std::forward<Args>(args)...);
    }

    static int s_indent;
    static bool s_debugFunctionCalls;
    CellI& m_type;
    std::map<CellI*, CellI*> m_slots;
};

namespace util {

template <typename T>
T& ref(T& obj) { return obj; }

template <typename T>
T& ref(T* obj) { return *obj; }

} // namespace util

// ============================================================================
class List : public CellI
{
public:
    class Node : public CellI
    {
    public:
        Node(World& w, List& list, CellI& value);

        using CellI::get;
        using CellI::has;
        using CellI::missing;
        using CellI::set;
        using CellI::erase;
        using CellI::operator[];

        bool has(CellI& key) override;
        void set(CellI& key, CellI& value) override;
        void erase(CellI& key) override;
        void operator()() override;
        CellI& operator[](CellI& key) override;
        void accept(Visitor& visitor) override;

        List& m_list;
        CellI& m_value;
        Node* m_previous  = nullptr;
        Node* m_next      = nullptr;
        CellI* m_selfType = nullptr;
    };

    List(World& w, CellI& valueType, const std::string& label = "");

    using CellI::get;
    using CellI::has;
    using CellI::missing;
    using CellI::set;
    using CellI::erase;
    using CellI::operator[];

    template <typename T>
    List(World& w, std::vector<T>& values, const std::string& label = "") :
        List(w, util::ref(values.front()).__type__(), label)
    {
        for (auto& valueT : values) {
            add(util::ref(valueT));
        }
    }

    template <typename Key, typename Value>
    List(World& w, std::map<Key, Value>& values) :
        List(w, util::ref((*values.begin())).second.type())
    {
        for (auto& valuePairs : values) {
            add(util::ref(valuePairs.second));
        }
    }

    bool has(CellI& key) override;
    void set(CellI& key, CellI& value) override;
    void erase(CellI& key) override;
    void operator()() override;
    CellI& operator[](CellI& key) override;
    void accept(Visitor& visitor) override;

    Node* add(CellI& value);
    template <typename... Args>
    void add(CellI& value, Args&&... args)
    {
        add(value);
        add(std::forward<Args>(args)...);
    }
    Node* addFront(CellI& value);
    void remove(Node* node);
    bool empty() const;
    int size();
    void clear();

protected:
    CellI& m_valueType;
    Node* m_firstNode = nullptr;
    Node* m_lastNode  = nullptr;
    CellI* m_selfType = nullptr;
    int m_size        = 0;
};
// ============================================================================
class Map;
class Struct : public CellI
{
public:
    enum class WithRecursiveType
    {
        Yes
    };
    Struct(World& w, const std::string& label = "");
    Struct(World& w, WithRecursiveType m_recursiveType, const std::string& label = "");

    using CellI::get;
    using CellI::has;
    using CellI::missing;
    using CellI::set;
    using CellI::erase;
    using CellI::operator[];

    bool has(CellI& key) override;
    void set(CellI& key, CellI& value) override;
    void erase(CellI& key) override;
    void operator()() override;
    CellI& operator[](CellI& key) override;
    void accept(Visitor& visitor) override;

    void addSlot(CellI& key, CellI& slot);
    bool hasSlot(CellI& key);
    void removeSlot(CellI& key);

    CellI* m_name = nullptr;
    Map& m_slots;
    Map* m_typeAliases = nullptr;
    Map* m_memberOf    = nullptr;
    Map* m_asts        = nullptr;
    Map* m_methods     = nullptr;
};

// ============================================================================
class Index : public CellI
{
public:
    Index(World& w, const std::string& label = "");
    Index(World& w, Struct& indexType, const std::string& label = "");

    using CellI::get;
    using CellI::has;
    using CellI::missing;
    using CellI::set;
    using CellI::erase;
    using CellI::operator[];

    bool has(CellI& key) override;
    void set(CellI& key, CellI& value) override;
    void erase(CellI& key) override;
    void operator()() override;
    CellI& operator[](CellI& key) override;
    void accept(Visitor& visitor) override;

    void insert(CellI& key, CellI& value);
    bool empty() const;
    int size();

    Struct* m_type;
    bool m_recursiveType = false;
    std::map<CellI*, CellI*> m_slots;
};

// ============================================================================
class Map : public CellI
{
public:
    Map(World& w, CellI& keyType, CellI& valueType, const std::string& label = "");
    Map(World& w, CellI& keyType, CellI& valueType, Struct& indexType, const std::string& label = "");

    using CellI::get;
    using CellI::has;
    using CellI::missing;
    using CellI::set;
    using CellI::erase;
    using CellI::operator[];

    bool has(CellI& key) override;
    void set(CellI& key, CellI& value) override;
    void erase(CellI& key) override;
    void operator()() override;
    CellI& operator[](CellI& key) override;
    void accept(Visitor& visitor) override;

    bool hasKey(CellI& key);
    CellI& getValue(CellI& key);

    void add(CellI& key, CellI& value);
    template <typename... Args>
    void add(CellI& key, CellI& value, Args&&... args)
    {
        add(key, value);
        add(std::forward<Args>(args)...);
    }
    void remove(CellI& key);
    bool empty() const;
    int size();

private:
    List m_list;
    Index m_index;
    CellI& m_keyType;
    CellI& m_valueType;
    CellI* m_selfType = nullptr;
    int m_size        = 0;
};

// ============================================================================
class TrieMap : public CellI
{
public:
    TrieMap(World& w, CellI& keyType, CellI& valueType, const std::string& label = "");

    using CellI::get;
    using CellI::has;
    using CellI::missing;
    using CellI::set;
    using CellI::erase;
    using CellI::operator[];

    bool has(CellI& key) override;
    void set(CellI& key, CellI& value) override;
    void erase(CellI& key) override;
    void operator()() override;
    CellI& operator[](CellI& key) override;
    void accept(Visitor& visitor) override;

    bool hasKey(CellI& key);
    CellI& getValue(CellI& key);

    bool hasValueWithDataKey(CellI& key);
    CellI& getValueWithDataKey(CellI& key);
    void addWithDataKey(CellI& key, CellI& value);

    void add(CellI& key, CellI& value);
    template <typename... Args>
    void add(CellI& key, CellI& value, Args&&... args)
    {
        add(key, value);
        add(std::forward<Args>(args)...);
    }
    void remove(CellI& key);
    bool empty() const;
    int size();

private:

    List m_list;
    Object m_rootNode;
    CellI& m_keyType;
    CellI& m_valueType;
    CellI* m_selfType = nullptr;
    int m_size        = 0;
};

// ============================================================================
class Set : public CellI
{
public:
    Set(World& w, CellI& valueType, const std::string& label = "");

    bool has(CellI& key) override;
    void set(CellI& key, CellI& value) override;
    void erase(CellI& key) override;
    void operator()() override;
    CellI& operator[](CellI& key) override;
    void accept(Visitor& visitor) override;

    bool contains(CellI& key);

    void add(CellI& value);
    template <typename... Args>
    void add(CellI& value, Args&&... args)
    {
        add(value);
        add(std::forward<Args>(args)...);
    }
    void remove(CellI& key);
    bool empty() const;
    int size();
    CellI& first();

protected:
    CellI& m_valueType;
    Index m_index;
    CellI* m_selfType = nullptr;
    int m_size        = 0;
};

// ============================================================================
class Number : public CellI
{
public:
    explicit Number(World& w, int value = 0);

    bool has(CellI& key) override;
    void set(CellI& key, CellI& value) override;
    void erase(CellI& key) override;
    void operator()() override;
    CellI& operator[](CellI& key) override;
    void accept(Visitor& visitor) override;

    int value() const;
    void value(int newValue);
    void increase();

protected:
    void calculateDigits();

    int m_value;
    std::vector<Object*> m_digits;
    std::unique_ptr<List> m_digitsList;
};

// ============================================================================
class String : public CellI
{
public:
    explicit String(World& w, const std::string& str = "");
    String(World& w, List& list, const std::string& str);

    using CellI::get;
    using CellI::has;
    using CellI::missing;
    using CellI::set;
    using CellI::erase;
    using CellI::operator[];

    bool has(CellI& key) override;
    void set(CellI& key, CellI& value) override;
    void erase(CellI& key) override;
    void operator()() override;
    CellI& operator[](CellI& key) override;
    void accept(Visitor& visitor) override;

    const std::string& value() const;

protected:
    void calculateCharacters();

    std::string m_value;
    std::vector<Object*> m_characters;
    List* m_charactersListPtr = nullptr;
    std::unique_ptr<List> m_charactersList;
};

namespace hybrid {
// ============================================================================
class ActivationPointer : public CellI
{
public:
    ActivationPointer(World& w);

    bool has(CellI& key) override;
    void set(CellI& key, CellI& value) override;
    void erase(CellI& key) override;
    void operator()() override;
    CellI& operator[](CellI& key) override;
    void accept(Visitor& visitor) override;

    CellI* m_currentCell  = nullptr;
    CellI* m_previousCell = nullptr;
    Number m_time;
};

} // namespace hybrid

// ============================================================================
class Visitor
{
public:
    virtual void visit(CellI&)      = 0;
    virtual void visit(Object&)     = 0;
    virtual void visit(Number&)     = 0;
    virtual void visit(String&)     = 0;
    virtual void visit(List::Node&) = 0;
    virtual void visit(List&)       = 0;
    virtual void visit(Struct&)     = 0;
    virtual void visit(Index&)      = 0;
    virtual void visit(Map&)        = 0;
    virtual void visit(TrieMap&)    = 0;
    virtual void visit(Set&)        = 0;

    static void visitList(CellI& list, std::function<void(CellI& value, int i, bool& stop)> fn);
    static void visitListInReverse(CellI& list, std::function<void(CellI& value, int i, bool& stop)> fn);
};

bool tryVisitWith(CellI& cell, Visitor& visitor);

} // namespace cells
} // namespace infocell