#pragma once

#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <vector>

#include "app/Picture.h"

namespace synth {
namespace cells {
namespace brain {
class Brain;
} // namespace brain

// ============================================================================
class Visitor;
class CellI
{
public:
    CellI(brain::Brain& kb);
    CellI(brain::Brain& kb, const std::string& label);
    CellI(const CellI& rhs);
    virtual ~CellI();

    virtual bool has(CellI& role)               = 0;
    virtual void set(CellI& role, CellI& value) = 0;
    virtual void erase(CellI& role)             = 0;
    virtual void operator()()                   = 0;
    virtual CellI& operator[](CellI& role)      = 0;
    virtual void accept(Visitor& visitor)       = 0;
    CellI& call(CellI& method);
    CellI& call(CellI& method, CellI& param1Role, CellI& param1Value);

    bool missing(CellI& role);
    CellI& get(CellI& role);
    CellI& type();
    void eval();

    std::string label() const;
    void label(const std::string& label);

    bool isA(CellI& type);
    bool isA(CellI& cell, CellI& type) const;

    bool operator==(CellI& rhs);
    bool operator!=(CellI& rhs);

    brain::Brain& kb;    // knowledge base
    std::string m_label; // for comments

    static int s_constructed;
    static int s_destructed;
};

struct Param
{
    Param(CellI& role, CellI& value) :
        role(role), value(value) { }

    CellI& role;
    CellI& value;
};

// ============================================================================
class Object : public CellI
{
public:
    Object(brain::Brain& kb, CellI& type, const std::string& label = "");
    Object(brain::Brain& kb, CellI& type, CellI& constructor, const std::string& label = "");
    Object(brain::Brain& kb, CellI& type, CellI& constructor, Param param1, const std::string& label = "");
    Object(brain::Brain& kb, CellI& type, CellI& constructor, Param param1, Param param2, const std::string& label = "");
    Object(brain::Brain& kb, CellI& type, CellI& constructor, Param param1, Param param2, Param param3, const std::string& label = "");
    Object(brain::Brain& kb, CellI& type, CellI& constructor, Param param1, Param param2, Param param3, Param param4, const std::string& label = "");
    ~Object();

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void erase(CellI& role) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

    CellI& method(CellI& role);
    CellI& method(CellI& role, Param param1);
    CellI& method(CellI& role, Param param1, Param param2);
    CellI& method(CellI& role, Param param1, Param param2, Param param3);
    CellI& method(CellI& role, Param param1, Param param2, Param param3, Param param4);

    CellI& smethod(CellI& role);
    CellI& smethod(CellI& role, Param param1);
    CellI& smethod(CellI& role, Param param1, Param param2);
    CellI& smethod(CellI& role, Param param1, Param param2, Param param3);
    CellI& smethod(CellI& role, Param param1, Param param2, Param param3, Param param4);
    void resetIndent();

protected:
    void destructor();

    bool hasMethod(CellI& role);
    CellI& getMethod(CellI& role);
    CellI& getStaticMethod(CellI& role);
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
    void printIndent();

    static int s_indent;
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
    class Item : public CellI
    {
    public:
        Item(brain::Brain& kb, List& list, CellI& value);

        bool has(CellI& role) override;
        void set(CellI& role, CellI& value) override;
        void erase(CellI& role) override;
        void operator()() override;
        CellI& operator[](CellI& role) override;
        void accept(Visitor& visitor) override;

        List& m_list;
        CellI& m_value;
        Item* m_previous = nullptr;
        Item* m_next     = nullptr;
    };

    List(brain::Brain& kb, CellI& valueType, const std::string& label = "");

    template <typename T>
    List(brain::Brain& kb, std::vector<T>& values, const std::string& label = "") :
        List(kb, util::ref(values.front()).type(), label)
    {
        for (auto& valueT : values) {
            add(util::ref(valueT));
        }
    }

    template <typename Key, typename Value>
    List(brain::Brain& kb, std::map<Key, Value>& values) :
        List(kb, util::ref((*values.begin())).second.type())
    {
        for (auto& valuePairs : values) {
            add(util::ref(valuePairs.second));
        }
    }

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void erase(CellI& role) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

    Item* add(CellI& value);
    template <typename... Args>
    void add(CellI& value, Args&&... args)
    {
        add(value);
        add(std::forward<Args>(args)...);
    }
    void removeItem(Item* item);
    bool empty() const;
    int size();

protected:
    CellI& m_valueType;
    Item* m_firstItem = nullptr;
    Item* m_lastItem  = nullptr;
    int m_size     = 0;
};
// ============================================================================
class Map;
class Type : public CellI
{
public:
    enum class WithRecursiveType
    {
        Yes
    };
    Type(brain::Brain& kb, const std::string& label = "");
    Type(brain::Brain& kb, WithRecursiveType m_recursiveType, const std::string& label = "");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void erase(CellI& role) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

    void addSlot(CellI& role, CellI& slot);
    bool hasSlot(CellI& role);
    void removeSlot(CellI& role);

    Map& m_slots;
    Map* m_subTypes = nullptr;
    Map* m_memberOf = nullptr;
    Map* m_asts     = nullptr;
    Map* m_methods  = nullptr;
};
// ============================================================================
class Index : public CellI
{
public:
    Index(brain::Brain& kb, const std::string& label = "");
    Index(brain::Brain& kb, Type& indexType, const std::string& label = "");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void erase(CellI& role) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

    void insert(CellI& key, CellI& value);
    bool empty() const;
    int size();

    Type* m_type;
    bool m_recursiveType = false;
    std::map<CellI*, CellI*> m_slots;
};

// ============================================================================
class Map : public CellI
{
public:
    Map(brain::Brain& kb, CellI& keyType, CellI& valueType, const std::string& label = "");
    Map(brain::Brain& kb, CellI& keyType, CellI& valueType, Type& indexType, const std::string& label = "");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void erase(CellI& role) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
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
    int m_size = 0;
};

// ============================================================================
class TrieMap : public CellI
{
public:
    TrieMap(brain::Brain& kb, CellI& keyType, CellI& valueType, const std::string& label = "");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void erase(CellI& role) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
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
    Object m_rootNode;
    CellI& m_keyType;
    CellI& m_valueType;
    int m_size = 0;
};

// ============================================================================
class Set : public CellI
{
public:
    Set(brain::Brain& kb, CellI& valueType, const std::string& label = "");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void erase(CellI& role) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
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

protected:
    CellI& m_valueType;
    Index m_index;
    int m_size = 0;
};

// ============================================================================
class Number : public CellI
{
public:
    explicit Number(brain::Brain& kb, int value = 0);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void erase(CellI& role) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

    int value() const;
    void value(int newValue);

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
    explicit String(brain::Brain& kb, const std::string& str = "");

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void erase(CellI& role) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

    const std::string& value() const;

protected:
    void calculateCharacters();

    std::string m_value;
    std::vector<Object*> m_characters;
    std::unique_ptr<List> m_charactersList;
};

namespace hybrid {

// ============================================================================
class Color : public CellI
{
public:
    Color(brain::Brain& kb, const input::Color& inputColor);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void erase(CellI& role) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

    const input::Color& color() const;

private:
    const input::Color& m_inputColor;
};

// ============================================================================
class Pixel : public CellI
{
public:
    Pixel(brain::Brain& kb, int x, int y, const input::Color& inputColor);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void erase(CellI& role) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

    const input::Color& color() const;

    Pixel* m_up    = nullptr;
    Pixel* m_down  = nullptr;
    Pixel* m_left  = nullptr;
    Pixel* m_right = nullptr;
    Number& m_x;
    Number& m_y;

private:
    Color m_color;
    const input::Color& m_inputColor;
};

// ============================================================================
class Picture : public CellI
{
public:
    Picture(brain::Brain& kb, input::Picture& screen);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void erase(CellI& role) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

    Pixel& getPixel(int x, int y);
    const Pixel& getPixel(int x, int y) const;

    int currentIndex(int x, int y) const;
    bool isInRange(int x, int y) const;
    Pixel* upPixel(int x, int y);
    Pixel* downPixel(int x, int y);
    Pixel* leftPixel(int x, int y);
    Pixel* rightPixel(int x, int y);
    std::vector<Pixel>& pixels();

    int width() const;
    int height() const;

protected:
    int m_width;
    int m_height;
    Number& m_widthCell;
    Number& m_heightCell;
    std::vector<Pixel> m_pixels;
    std::unique_ptr<List> m_pixelsList;
};

} // namespace hybrid

// ============================================================================
class Visitor
{
public:
    virtual void visit(Object&) = 0;

    virtual void visit(Number&) = 0;
    virtual void visit(String&) = 0;

    virtual void visit(List::Item&) = 0;
    virtual void visit(List&)       = 0;
    virtual void visit(Type&)       = 0;
    virtual void visit(Index&)      = 0;
    virtual void visit(Map&)        = 0;
    virtual void visit(TrieMap&)    = 0;
    virtual void visit(Set&)        = 0;

    virtual void visit(hybrid::Color&)   = 0;
    virtual void visit(hybrid::Pixel&)   = 0;
    virtual void visit(hybrid::Picture&) = 0;

    static void visitList(CellI& list, std::function<void(CellI& value, int i, bool& stop)> fn);
};

bool tryVisitWith(CellI& cell, Visitor& visitor);

} // namespace cells
} // namespace synth