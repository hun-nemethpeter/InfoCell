#include <sstream>

#include "Vector.h"
#include "cells/World.h"

namespace infocell {
namespace cells {
namespace arc {

Vector::Vector(World& w, Number& x, Number& y) :
    CellI(w, "Vector"),
    m_x(x),
    m_y(y)
{
}

bool Vector::has(CellI& key)
{
    if (&key == &w.id.__type__) {
        return true;
    }
    if (&key == &w.id.coordinates.x) {
        return true;
    }
    if (&key == &w.id.coordinates.y) {
        return true;
    }

    return false;
}

void Vector::set(CellI& key, CellI& value)
{
    throw "Changing a hybrid offset cell is not possible!";
}

void Vector::erase(CellI& key)
{
    throw "Changing a hybrid offset cell is not possible!";
}

void Vector::operator()()
{
    // Do nothing, this is a data cell
}

CellI& Vector::operator[](CellI& key)
{
    if (&key == &w.id.__type__) {
        return w.arc.Vector;
    }
    if (&key == &w.id.coordinates.x) {
        return m_x;
    }
    if (&key == &w.id.coordinates.y) {
        return m_y;
    }

    throw "No such key!";
}

std::string Vector::toString() const
{
    std::stringstream ss;
    ss << "(" << m_x.value() << ", " << m_y.value() << ")";
    return ss.str();
}

std::ostream& operator<<(std::ostream& os, const Vector& shape)
{
    os << shape.toString();
    return os;
}
} // namespace arc
} // namespace cells
} // namespace infocell