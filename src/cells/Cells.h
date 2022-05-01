#pragma once

#include <array>
#include <map>
#include <sstream>
#include <vector>
#include <iostream>

namespace synth {
namespace cells {

class Cell
{
public:
    enum class InternalType
    {
        Data,
        Code,
        InternalData,
        InternalCode
    };

    Cell(InternalType internalType) :
        m_internalType(internalType)
    {
    }

    virtual ~Cell() { }

    virtual const std::string nameAsString() const
    {
        static std::string s_name = "Cell";
        return s_name;
    }

    virtual const std::string valueAsString() const
    {
        return nameAsString();
    }

    virtual Cell& member(Cell& member);

    const Cell& member(Cell& member) const
    {
        return const_cast<Cell&>(*this).member(member);
    }

    static std::vector<Cell*> membersData;
    virtual std::vector<Cell*>& members()
    {
        return membersData;
    }

    virtual Cell* membervalues(unsigned index)
    {
        abort();
    }

    InternalType internalType() const
    {
        return m_internalType;
    }

protected:
    InternalType m_internalType;
};

class DataCell : public Cell
{
public:
    DataCell() :
        Cell(Cell::InternalType::Data)
    {
    }

    const std::string nameAsString() const override
    {
        static std::string s_name = "DataCell";
        return s_name;
    }
};

class InternalDataCell : public Cell
{
public:
    InternalDataCell() :
        Cell(Cell::InternalType::InternalData)
    {
    }

    const std::string nameAsString() const override
    {
        static std::string s_name = "InternalDataCell";
        return s_name;
    }
};

#define DATA_CELL(_object_)                             \
    class C##_object_ : public DataCell                 \
    {                                                   \
        const std::string nameAsString() const override \
        {                                               \
            static std::string s_name = #_object_;      \
            return s_name;                              \
        }                                               \
    };                                                  \
    extern C##_object_ _object_;

namespace types {
DATA_CELL(Empty);
DATA_CELL(Color);
DATA_CELL(Pixel);
} // namespace types

namespace members {
DATA_CELL(self);
DATA_CELL(type);
namespace colors {
DATA_CELL(red);
DATA_CELL(green);
DATA_CELL(blue);
} // namespace colors
namespace directions {
DATA_CELL(up);
DATA_CELL(down);
DATA_CELL(left);
DATA_CELL(right);
} // namespace directions
DATA_CELL(color);
DATA_CELL(next);
DATA_CELL(prev);
DATA_CELL(first);
DATA_CELL(last);
DATA_CELL(size);
DATA_CELL(value);
DATA_CELL(input);
DATA_CELL(output);
DATA_CELL(list);
} // namespace members
#undef DATA_CELL

template <class T>
std::string toStringvalues(T& cell);

template <class T>
std::string toString(T& cell)
{
    std::stringstream ss;
    ss << cell.nameAsString();
    if (cell.internalType() == Cell::InternalType::Data) {
        ss << " {\n";
        unsigned index = 0;
        for (Cell* member : cell.members()) {
            Cell* value = cell.membervalues(index);
            if (!value) {
                value = &types::Empty;
            }
            if (index > 0) {
                ss << ",\n";
            }
            ss << "    " << member->nameAsString() << ": " << value->nameAsString() << "" << toStringvalues(*value);
            ++index;
        }
        ss << "\n}\n";
    } else if (cell.internalType() == Cell::InternalType::InternalData) {
        ss << "(" << cell.valueAsString() << ")\n";
    }

    return ss.str();
}

template <class T>
std::string toStringvalues(T& cell)
{
    std::stringstream ss;
    if (cell.internalType() == Cell::InternalType::Data) {
        ss << " {";
        unsigned index = 0;
        for (Cell* member : cell.members()) {
            Cell* value = cell.membervalues(index);
            if (!value) {
                value = &types::Empty;
            }
            if (index > 0) {
                ss << ",";
            }
            ss << " " << member->nameAsString() << ": " << value->valueAsString();
            ++index;
        }
        ss << " }";
    } else if (cell.internalType() == Cell::InternalType::InternalData) {
        ss << "(" << cell.valueAsString() << ")";
    }

    return ss.str();
}

class IntValue : public InternalDataCell
{
public:
    IntValue(int64_t value = 0) :
        value(value) { }

    const std::string nameAsString() const override
    {
        static std::string s_name = "IntValue";
        return s_name;
    }

    const std::string valueAsString() const override
    {
        return string();
    }

    static std::vector<Cell*> membersData;
    std::vector<Cell*>& members() override
    {
        return membersData;
    }
    Cell* membervalues(unsigned index) override
    {
        switch (index) {
        case 0:
            return this;
        default:
            abort();
        }
    }
    std::string string() const;

    int64_t value;
};

class Color : public DataCell
{
public:
    Color() :
        red(redD), green(greenD), blue(blueD) { }

    Color(int64_t _red, int64_t _green, int64_t _blue) :
        red(_red), green(_green), blue(_blue) { }

    const std::string nameAsString() const override
    {
        static std::string s_name = "Color";
        return s_name;
    }

    static std::vector<Cell*> membersData;
    std::vector<Cell*>& members() override
    {
        return membersData;
    }

    Cell* membervalues(unsigned index) override
    {
        switch (index) {
        case 0:
            return &red;
        case 1:
            return &green;
        case 2:
            return &blue;
        default:
            abort();
        }
    }

    int64_t redD   = 0;
    int64_t greenD = 0;
    int64_t blueD  = 0;
    IntValue red;
    IntValue green;
    IntValue blue;
};

class Pixel : public DataCell
{
public:
    typedef DataCell base;

    Cell& member(Cell& member) override
    {
        if (&member == &members::directions::up) {
            return *up;
        } else if (&member == &members::directions::down) {
            return *down;
        } else if (&member == &members::directions::left) {
            return *left;
        } else if (&member == &members::directions::right) {
            return *right;
        } else if (&member == &members::type) {
            return *type;
        } else {
            return types::Empty;
        }

        return *this;
    }

    const std::string nameAsString() const override
    {
        static std::string s_name = "Pixel";
        return s_name;
    }

    static std::vector<Cell*> membersData;
    std::vector<Cell*>& members() override
    {
        return membersData;
    }

    Cell* membervalues(unsigned index) override
    {
        switch (index) {
        case 0:
            return type;
        case 1:
            return up;
        case 2:
            return down;
        case 3:
            return left;
        case 4:
            return right;
        case 5:
            return &color;
        default:
            abort();
        }
    }

    DataCell* type = &types::Pixel;
    Pixel* up      = nullptr;
    Pixel* down    = nullptr;
    Pixel* left    = nullptr;
    Pixel* right   = nullptr;
    Color color;
};

/*
*   Color() : (red: Int, green: Int, blue: Int)
*   Pixel() : (up: Pixel, down: Pixel, left: Pixel, right: Pixel, color: Color)
*   Sensor(x: Int, y: Int) : (Pixel | Empty)
*/

struct StringChar : DataCell
{
    StringChar(int64_t charvalue = 0) :
        value(charvalue) { }
    Cell& member(Cell& member) override
    {
        if (&member == &members::next) {
            return *next;
        } else if (&member == &members::prev) {
            return *prev;
        } else if (&member == &members::value) {
            return value;
        } else {
            return types::Empty;
        }

        return *this;
    }

    const std::string nameAsString() const override
    {
        static std::string s_name = "StringChar";
        return s_name;
    }

    static std::vector<Cell*> membersData;
    std::vector<Cell*>& members() override
    {
        return membersData;
    }

    Cell* membervalues(unsigned index) override
    {
        switch (index) {
        case 0:
            return next;
        case 1:
            return prev;
        case 2:
            return &value;
        default:
            abort();
        }
    }

    StringChar* next = nullptr;
    StringChar* prev = nullptr;
    IntValue value;
};

class String : public DataCell
{
public:
    Cell& member(Cell& member) override
    {
        if (&member == &members::first) {
            return *first;
        } else if (&member == &members::last) {
            return *last;
        } else if (&member == &members::size) {
            return m_size;
        } else {
            return types::Empty;
        }

        return *this;
    }

    const std::string nameAsString() const override
    {
        static std::string s_name = "String";
        return s_name;
    }

    const std::string valueAsString() const override
    {
        std::stringstream ss;
        ss << "\"" << string() << "\"";
        return ss.str();
    }

    static std::vector<Cell*> membersData;
    std::vector<Cell*>& members() override
    {
        return membersData;
    }

    Cell* membervalues(unsigned index) override
    {
        switch (index) {
        case 0:
            return first;
        case 1:
            return last;
        case 2:
            return &m_size;
        default:
            abort();
        }
    }

    std::string string() const;
    String substr(int pos, int len) const;
    size_t size() const;
    bool operator==(const String& rhs) const;
    String& addChar(char32_t utf32Char);

    StringChar* first = nullptr;
    StringChar* last  = nullptr;
    IntValue m_size   = 0;
};

/*
List

ListItem<T> {
    list->List*;      // TODO
    next->ListItem*;
    prev->ListItem*;
    value->T;
};

List<T> {
    Iterator {};
    begin()->Iterator;
    end()->Iterator;
    first->ListItem<T>*;
    last->ListItem<T>*;
    size->IntValue;
};


*/

template <typename DataT>
class List;

template <typename DataT>
class ListItem : public DataCell
{
public:
    ListItem<DataT>(DataT* value) :
        value(value) { }

    const std::string nameAsString() const override
    {
        static DataT dataT;
        std::string ret = "ListItem<";
        ret += dataT.nameAsString();
        ret += ">";

        return ret;
    }

    static std::vector<Cell*> membersData;
    std::vector<Cell*>& members() override
    {
        return membersData;
    }

    Cell* membervalues(unsigned index) override
    {
        switch (index) {
        case 0:
            return list;
        case 1:
            return next;
        case 2:
            return prev;
        case 3:
            return value;
        default:
            abort();
        }
    }

    Cell& member(Cell& member) override
    {
        if (&member == &members::prev) {
            return *prev;
        } else if (&member == &members::next) {
            return *next;
        } else {
            return types::Empty;
        }

        return *this;
    }

    List<DataT>* list     = nullptr;
    ListItem<DataT>* next = nullptr;
    ListItem<DataT>* prev = nullptr;
    DataT* value;
};

template <typename DataT>
std::vector<Cell*> ListItem<DataT>::membersData = { &members::list, &members::next, &members::prev, &members::value };

template <typename DataT>
class List : public DataCell
{
public:
    class Iterator
    {
        Iterator(ListItem<DataT>* listItem) :
            ListItem(listItem) { }
        Iterator next()
        {
            return Iterator(listItem->next);
        }

    private:
        ListItem<DataT>* listItem;
    };

    const std::string nameAsString() const override
    {
        static DataT dataT;
        std::string ret = "List<";
        ret += dataT.nameAsString();
        ret += ">";

        return ret;
    }

    static std::vector<Cell*> membersData;
    std::vector<Cell*>& members() override
    {
        return membersData;
    }

    Cell* membervalues(unsigned index) override
    {
        switch (index) {
        case 0:
            return first;
        case 1:
            return last;
        case 2:
            return &m_size;
        default:
            abort();
        }
    }

    Iterator begin()
    {
        return Iterator(first);
    }

    Iterator end()
    {
        return Iterator(nullptr);
    }

    Cell& member(Cell& member) override
    {
        if (&member == &members::first) {
            return *first;
        } else if (&member == &members::last) {
            return *last;
        } else if (&member == &members::size) {
            return m_size;
        } else {
            return types::Empty;
        }

        return *this;
    }

    void addItem(DataT* data)
    {
        auto* listItem = new ListItem<DataT>(data);
        listItem->list = this;
        if (m_size.value == 0) {
            first = listItem;
        } else {
            last->next     = listItem;
            listItem->prev = last;
        }
        last = listItem;
        ++m_size.value;
    }

    ListItem<DataT>* first = nullptr;
    ListItem<DataT>* last  = nullptr;
    IntValue m_size        = 0;
};

template <typename DataT>
std::vector<Cell*> List<DataT>::membersData = { &members::first, &members::last, &members::size };

template <typename T>
struct CellComparator
{
};

template <>
struct CellComparator<IntValue>
{
    bool operator()(const IntValue* lhs, const IntValue* rhs) const
    {
        return lhs->value < rhs->value;
    }
};

template <>
struct CellComparator<String>
{
    bool operator()(const String* lhs, const String* rhs) const
    {
        StringChar* lhsChar = lhs->first;
        StringChar* rhsChar = rhs->first;

        if (!lhsChar)
            return true;

        if (!rhsChar)
            return false;

        while (lhsChar != nullptr && rhsChar != nullptr) {
            if (lhsChar->value.value < rhsChar->value.value)
                return true;

            if (lhsChar->value.value > rhsChar->value.value)
                return false;

            lhsChar = lhsChar->next;
            rhsChar = rhsChar->next;
        }
        if (!lhsChar && rhsChar)
            return true;

        return false;
    }
};

template <class InputT, class OutputT>
class Map : public DataCell
{
public:
    const std::string nameAsString() const override
    {
        static InputT inputT;
        static OutputT outputT;
        std::string ret = "Map<";
        ret += inputT.nameAsString();
        ret += ", ";
        ret += outputT.nameAsString();
        ret += ">";

        return ret;
    }

    Cell& member(Cell& member) override
    {
        return types::Empty;
    }

    OutputT* find(const InputT& key) const
    {
        auto it = m_map.find(&key);
        return it == m_map.end() ? nullptr : it->second;
    }

    OutputT*& operator[](const InputT* key)
    {
        return m_map[key];
    }
    std::map<const InputT*, OutputT*, CellComparator<InputT>> m_map;
};

template <class InputT, class OutputT>
class IOCell : public DataCell
{
public:
    IOCell<InputT, OutputT>(InputT* input = nullptr, OutputT* output = nullptr) :
        input(input), output(output) { }

    const std::string nameAsString() const override
    {
        static InputT inputT;
        static OutputT outputT;
        std::string ret = "IOCell<";
        ret += inputT.nameAsString();
        ret += ", ";
        ret += outputT.nameAsString();
        ret += ">";

        return ret;
    }

    static std::vector<Cell*> membersData;
    std::vector<Cell*>& members() override
    {
        return membersData;
    }

    Cell* membervalues(unsigned index) override
    {
        switch (index) {
        case 0:
            return input;
        case 1:
            return output;
        default:
            abort();
        }
    }

    Cell& member(Cell& member) override
    {
        if (&member == &members::input) {
            return *input;
        } else if (&member == &members::output) {
            return *output;
        } else {
            return types::Empty;
        }

        return *this;
    }
    InputT* input   = nullptr;
    OutputT* output = nullptr;
};

template <typename InputT, typename OutputT>
std::vector<Cell*> IOCell<InputT, OutputT>::membersData = { &members::input, &members::output };
struct Sensor
{
    int m_width  = 8;
    int m_height = 8;
    Sensor()
    {
    }

    Sensor& width(int value)
    {
        m_width = value;
        return *this;
    }

    Sensor& height(int value)
    {
        m_height = value;
        return *this;
    }

    void init()
    {
//        std::cout << "Size of pixel cell is " << sizeof(Pixel) << " bytes\n";
//        std::cout << "Allocating vector .. ";
        const int senzorSize = m_height * m_width;
        pixels.reserve(senzorSize);
//        std::cout << "done\n";
//        std::cout << "Creating cells 0%..";
        int percentage = 0;
        for (int y = 0; y < m_height; ++y) {
            for (int x = 0; x < m_width; ++x) {
                pixels.emplace_back();
                int newPercentage = ((int64_t(y) * m_width + x) * 100) / senzorSize;
                if ((newPercentage % 10 == 0) && (newPercentage - percentage) > 9) {
//                    std::cout << newPercentage << "%..";
                    percentage = newPercentage;
                }
            }
        }
//        std::cout << "100% done\n";
//        std::cout << "Initializeing cells .. ";
        for (int y = 0; y < m_height; ++y) {
            for (int x = 0; x < m_width; ++x) {
                Pixel& pixel = pixels[currentIndex(x, y)];
                pixel.up     = upPixel(x, y);
                pixel.down   = downPixel(x, y);
                pixel.left   = leftPixel(x, y);
                pixel.right  = rightPixel(x, y);
            }
        }
//        std::cout << "done" << std::endl;
    }

    Pixel& getPixel(int x, int y)
    {
        return pixels[currentIndex(x, y)];
    }

    int currentIndex(int x, int y)
    {
        return y * m_width + x;
    }

    bool isInRange(int x, int y)
    {
        if (y < 0 || x < 0 || x > m_width - 1 || y > m_height - 1) {
            return false;
        }

        return true;
    }

    /*
           5 x 5
    m_width  = 5
    m_height = 5

         0  1  2  3  4
     0  00 01 02 03 04
     1  05 06 07 08 09
     2  10 11 12 13 14
     3  15 16 17 18 19
     4  20 21 22 23 24

     upPixel(4, 4) = 19
     3 * 5 + 4

     */

    Pixel* upPixel(int x, int y)
    {
        if (!isInRange(x, y) || y == 0) {
            return nullptr;
        }
        int upIndex = (y - 1) * m_width + x;
        return &pixels[upIndex];
    }

    Pixel* downPixel(int x, int y)
    {
        if (!isInRange(x, y) || y == m_height - 1) {
            return nullptr;
        } else {
            int downIndex = (y + 1) * m_width + x;
            return &pixels[downIndex];
        }
    }

    Pixel* leftPixel(int x, int y)
    {
        if (!isInRange(x, y) || x == 0) {
            return nullptr;
        } else {
            return &pixels[y * m_width + x - 1];
        }
    }

    Pixel* rightPixel(int x, int y)
    {
        if (!isInRange(x, y) || x == m_width - 1) {
            return nullptr;
        } else {
            return &pixels[y * m_width + x + 1];
        }
    }

    std::vector<Pixel> pixels;
};

struct Item
{
    Item(Cell& member, Cell& type) :
        m_member(member), m_type(&type)
    { }

    Cell& m_member;
    Cell* m_type;
};

struct Rule
{
    Rule& operator<<(const Item& item)
    {
        m_items.push_back(item);
        return *this;
    }
    bool match(const Cell& startCell)
    {
        unsigned itemIndex = 0;
        const Cell* cell   = nullptr;
        bool matched       = false;

        for (auto& item : m_items) {
            if (itemIndex == 0) {
                cell = &startCell;
            } else {
                cell = &cell->member(item.m_member);
            }

            if (&cell->member(members::type) == item.m_type) {
                ++itemIndex;
                matched = true;
            } else {
                matched = false;
                break;
            }
        }

        return matched;
    }

    std::vector<Item> m_items;
};

/*
*  ruleA = a
*  ruleA = b
*  ruleA = c
*  ruleA = d
*
*  to match a | b | c
*  version 1:
*  a | b | c => rulaA.or(ruleB).or(ruleC);
*  version 2:
*  a | b | c => rulaA.joinOr(ruleB).joinOr(ruleC);
*
*  to match abc
*  version 1:
*  a b c => rulaA.joinAfter(Member::Next, ruleB).joinAfter(Member::Next, ruleC);
*  version 2:
*  a b c => rulaA.joinAnd(Member::Next, ruleB).joinAnd(Member::Next, ruleC);
*
*  to match in 2D
*  +-+-+-+-+
*  |b| | | |
*  +-+-+-+-+
*  |a|c| | |
*  +-+-+-+-+
*  |d| | | |
*  +-+-+-+-+
*
*  version 1:
*  ruleA.joinBefore(Member::Up, ruleB).joinBefore(Member::Right, ruleC).joinBefore(Member::Down, ruleD)
*  version 2:
*  ruleA.join(Member::Up, ruleB).join(Member::Right, ruleC).join(Member::Down, ruleD)
*/
} // namespace cells
} // namespace synth