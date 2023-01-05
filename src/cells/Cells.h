#pragma once
#include <array>
#include <iostream>
#include <map>
#include <set>
#include <span>
#include <sstream>
#include <vector>

#include "app/Picture.h"

namespace synth {
namespace cells {

// ============================================================================
class Type;
class Printer;
class CellI
{
public:
    virtual bool has(CellI& role)                 = 0;
    virtual void set(CellI& role, CellI& value)   = 0;
    virtual void operator()()                     = 0;
    virtual CellI& operator[](CellI& role)        = 0;
    virtual Type& type()                          = 0;
    virtual std::string printAs(Printer& printer) = 0;
    virtual std::string name() const              = 0;
};

// ============================================================================
class String;
class SlotRef // Only exists to bypass the non-movable std::initalizer_list limitations
{
public:
    SlotRef(const std::string& name, Type& type, CellI& role);

    const std::string& m_name;
    Type& m_type;
    CellI& m_role;
};

class Slot : public CellI
{
public:
    Slot(const std::string& name, Type& type, CellI& role);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    std::string printAs(Printer& printer) override;
    std::string name() const override;

    static void staticInit();
    static void staticInitMembers();
    static Type& t();
    static Slot& slotSlotType();
    static Slot& slotSlotName();
    static Slot& slotSlotRole();

    Type& slotType();
    CellI& slotRole();

protected:
    static std::unique_ptr<Type> s_type;
    static Slot* s_slotSlotType;
    static Slot* s_slotSlotName;
    static Slot* s_slotSlotRole;
    std::string m_name;
    std::unique_ptr<String> m_slotNameString;
    Type& m_slotType;
    CellI& m_slotRole;
};

// ============================================================================
class List;
class Type : public CellI
{
public:
    explicit Type(const std::string& name);
    Type(const std::string& name, std::initializer_list<SlotRef> slots);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    std::string printAs(Printer& printer) override;
    std::string name() const override;

    static void staticInit();
    static void staticInitMembers();
    void addSlots(std::initializer_list<SlotRef> slots);
    Slot& createSlot(const std::string& name, Type& type, CellI& role);
    void referenceSlot(const std::string& name, Slot& slotCell);

    bool has(const std::string& name) const;

    bool hasSlot(CellI& role);
    bool hasSlot(const std::string& name);
    Slot& getSlot(CellI& role);
    Slot& getSlot(const std::string& name);

    std::map<std::string, Slot*>& slots();
    static Type& t();
    static Slot& slotType();
    static Slot& slotSlots();
    static Type& anyType();

protected:
    void registerTypeSlot();

    static std::unique_ptr<Type> s_type;
    static Slot* s_slotType;
    static Slot* s_slotSlots;
    static std::unique_ptr<Type> s_anyType;
    std::map<std::string, Slot> m_slots;
    std::map<std::string, Slot*> m_slotRefs;
    std::map<CellI*, Slot*> m_roles;
    std::string m_name;
    std::unique_ptr<List> m_slotsList;
};

// ============================================================================
class Object : public CellI
{
public:
    Object(Type& classCell);
    Object(const std::string& name, Type& classCell);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    std::string printAs(Printer& printer) override;
    std::string name() const override;

    std::map<CellI*, CellI*>& roles();

    static void staticInit();
    static Object& emptyObject();

protected:
    static std::unique_ptr<Object> s_emptyObject;
    std::string m_name;
    Type& m_type;
    std::map<CellI*, CellI*> m_roles;
};

// ============================================================================
class ListItem : public CellI
{
public:
    ListItem(Type& type);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    std::string printAs(Printer& printer) override;
    std::string name() const override;

    CellI& prev();
    void prev(ListItem* p);

    CellI& next();
    void next(ListItem* n);

    CellI& value();
    void value(CellI* v);

    Slot& slotPrev();
    Slot& slotNext();
    Slot& slotValue();

protected:
    Slot* m_slotPrev;
    Slot* m_slotNext;
    Slot* m_slotValue;

    Type& m_type;
    CellI* m_prev  = nullptr;
    CellI* m_next  = nullptr;
    CellI* m_value = nullptr;
};

// ============================================================================
class List : public CellI
{
public:
    template <typename T>
    List(const std::vector<T>& values);

    template <typename T>
    List(std::map<std::string, T>& values);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    std::string printAs(Printer& printer) override;
    std::string name() const override;

    std::vector<ListItem>& items();

    Slot& slotFirst();
    Slot& slotLast();
    Slot& slotSize();

protected:
    Slot* m_slotFirst;
    Slot* m_slotLast;
    Slot* m_slotSize;
    Type m_listType;
    Type m_itemType;
    std::vector<ListItem> m_items;
};

// ============================================================================
class Digits
{
public:
    static void staticInit();
    static std::unique_ptr<Type> s_digitClassCell;
    static std::vector<Object> s_digits;
};

// ============================================================================
class Number : public CellI
{
public:
    Number(int value);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    std::string printAs(Printer& printer) override;
    std::string name() const override;

    int value() const;

    static void staticInit();
    static Type& t();
    static Slot& slotSign();
    static Slot& slotValue();

protected:
    void calculateDigits();

    static std::unique_ptr<Type> s_type;
    static Slot* s_slotSign;
    static Slot* s_slotValue;

    int m_value;
    std::vector<Object*> m_digits;
    std::unique_ptr<List> m_digitsList;
};

// ============================================================================
class Numbers
{
public:
    static Number& get(int number);

protected:
    static std::map<int, Number> s_numbers;
};

// ============================================================================
class Chars
{
public:
    static void staticInit();
    static Object& get(char32_t utf32Char);
    static Type& type();

protected:
    static void registerUnicodeBlock(char32_t from, char32_t to);
    static std::unique_ptr<Type> s_type;
    static std::map<char32_t, Object> s_characters;
};

// ============================================================================
class String : public CellI
{
public:
    String(const std::string& str);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    std::string printAs(Printer& printer) override;
    std::string name() const override;

    const std::string& value() const;

    static void staticInit();
    static Type& t();
    static Slot& slotCharacters();

protected:
    void calculateCharacters();

    static std::unique_ptr<Type> s_type;
    static Slot* s_slotCharacters;

    std::string m_value;
    std::vector<Object*> m_characters;
    std::unique_ptr<List> m_charactersList;
};

namespace hybrid {

class Color : public CellI
{
public:
    Color(const input::Color& inputColor);
    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    std::string printAs(Printer& printer) override;
    std::string name() const override;

    static Type& t();

    const input::Color& color() const;

private:
    const input::Color& m_inputColor;
};

// ============================================================================
class Pixel : public CellI
{
public:
    Pixel(int x, int y, const input::Color& inputColor);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    std::string printAs(Printer& printer) override;
    std::string name() const override;

    static Type& t();

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
class Sensor : public CellI
{
public:
    Sensor(input::Picture& screen);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    Type& type() override;
    std::string printAs(Printer& printer) override;
    std::string name() const override;

    static Type& t();

    Pixel& getPixel(int x, int y);
    const Pixel& getPixel(int x, int y) const;

    int currentIndex(int x, int y) const;
    bool isInRange(int x, int y) const;
    Pixel* upPixel(int x, int y);
    Pixel* downPixel(int x, int y);
    Pixel* leftPixel(int x, int y);
    Pixel* rightPixel(int x, int y);
    int width() const;
    int height() const;

protected:
    std::string m_name;
    int m_width;
    int m_height;
    Number& m_widthCell;
    Number& m_heightCell;
    std::vector<Pixel> m_pixels;
};

} // namespace hybrid

// ============================================================================

namespace type
{
extern Type Color;
extern Type Pixel;
extern Type Sensor;
};

namespace data {
extern Object slotType;
extern Object slotName;
extern Object slotRole;

extern Object first;
extern Object last;
extern Object previous;
extern Object next;

namespace directions {
extern Object up;
extern Object down;
extern Object left;
extern Object right;
} // namespace directions

namespace coordinates {
extern Object x;
extern Object y;
} // namespace coordinates

extern Object color;
namespace colors {
extern Object red;
extern Object green;
extern Object blue;
} // namespace colors

extern Object width;
extern Object height;

extern Object size;
extern Object value;
extern Object type;
extern Object slots;
extern Object sign;
} // namespace cells

class Printer
{
public:
    virtual std::string print(Slot& cell)     = 0;
    virtual std::string print(Type& cell)     = 0;
    virtual std::string print(Object& cell)   = 0;
    virtual std::string print(ListItem& cell) = 0;
    virtual std::string print(List& cell)     = 0;
    virtual std::string print(Number& cell)   = 0;
    virtual std::string print(String& cell)   = 0;
    virtual std::string print(hybrid::Color& cell) = 0;
    virtual std::string print(hybrid::Pixel& cell) = 0;
    virtual std::string print(hybrid::Sensor& cell)   = 0;
};

class CellValuePrinter : public Printer
{
public:
    std::string print(Slot& cell) override;
    std::string print(Type& cell) override;
    std::string print(Object& cell) override;
    std::string print(ListItem& cell) override;
    std::string print(List& cell) override;
    std::string print(Number& cell) override;
    std::string print(String& cell) override;
    std::string print(hybrid::Color& cell) override;
    std::string print(hybrid::Pixel& cell) override;
    std::string print(hybrid::Sensor& cell) override;
};

class CellStructPrinter : public Printer
{
public:
    std::string print(Slot& cell) override;
    std::string print(Type& cell) override;
    std::string print(Object& cell) override;
    std::string print(ListItem& cell) override;
    std::string print(List& cell) override;
    std::string print(Number& cell) override;
    std::string print(String& cell) override;
    std::string print(hybrid::Color& cell) override;
    std::string print(hybrid::Pixel& cell) override;
    std::string print(hybrid::Sensor& cell) override;

    std::string printImpl(CellI& cell);
};

void StaticInitializations();

} // namespace cells
} // namespace synth