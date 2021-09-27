#include "Cells.h"

#pragma warning( disable : 4996)
#include <utf8.h>

namespace synth {
namespace cells {

#define DATA_CELL(_obj_) \
    C##_obj_ _obj_;

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
#undef CELL_OBJ

Cell& Cell::member(Cell& member)
{
    return types::Empty;
}

std::vector<Cell*> Cell::membersData;
std::vector<Cell*> Color::membersData      = { &members::colors::red, &members::colors::green, &members::colors::blue };
std::vector<Cell*> Pixel::membersData      = { &members::type, &members::directions::up, &members::directions::down, &members::directions::left, &members::directions::right, &members::color };
std::vector<Cell*> IntValue::membersData   = { &members::value };
std::vector<Cell*> StringChar::membersData = { &members::next, &members::prev, &members::value };
std::vector<Cell*> String::membersData     = { &members::first, &members::last, &members::size };

std::string IntValue::string() const
{
    std::string ret;
    ret = std::to_string(value);

    return ret;
}

std::string String::string() const
{
    std::string ret;
    for (auto* charCell = first; charCell; charCell = charCell->next) {
        char32_t utf32Char = static_cast<char32_t>(charCell->value.value);
        utf8::append(utf32Char, ret);
    }

    return ret;
}

String String::substr(int pos, int len) const
{
    String ret;

    if (first == nullptr) {
        return ret;
    }
    StringChar* retChar = first;
    for (int i = 0; i < pos; ++i) {
        retChar = retChar->next;
    }
    ret.addChar(static_cast<char32_t>(retChar->value.value));
    for (int i = 0; i < len - 1; ++i) {
        retChar = retChar->next;
        ret.addChar(static_cast<char32_t>(retChar->value.value));
    }

    return ret;
}

size_t String::size() const
{
    return m_size.value;
}

bool String::operator==(const String& rhs) const
{
    if (size() != rhs.size()) {
        return false;
    }
    StringChar* lhsChar = first;
    StringChar* rhsChar = rhs.first;

    while (lhsChar != nullptr && rhsChar != nullptr) {
        if (lhsChar->value.value != rhsChar->value.value)
            return false;

        lhsChar = lhsChar->next;
        rhsChar = rhsChar->next;
    }

    return true;
}

String& String::addChar(char32_t utf32Char)
{
    auto* cellChar = new cells::StringChar(utf32Char);

    if (m_size.value == 0) {
        first = cellChar;
    } else {
        last->next     = cellChar;
        cellChar->prev = last;
    }
    last = cellChar;
    ++m_size.value;

    return *this;
}

} // namespace cells
} // namespace synth