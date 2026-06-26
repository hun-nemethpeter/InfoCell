#include "ShapeField.h"

namespace infocell {
namespace cells {
namespace arc {

ShapeField::ShapeField(World& w, int width, int height) :
    cells::CellI(w),
    m_width(w, width),
    m_height(w, height),
    m_shapesMap(w, w.arc.Vector, w.arc.Shape)
{
}

bool ShapeField::has(CellI& key)
{
    if (&key == &w.id.__type__) {
        return true;
    }
    if (&key == &w.id.width) {
        return true;
    }
    if (&key == &w.id.height) {
        return true;
    }
    if (&key == &w.name("shapesMap")) {
        return true;
    }

    return false;
}

void ShapeField::set(CellI& key, CellI& value)
{
    throw "Changing a hybrid ShapeField cell is not possible!";
}

void ShapeField::erase(CellI& key)
{
    throw "Changing a hybrid ShapeField cell is not possible!";
}

void ShapeField::operator()()
{
    // Do nothing, this is a data cell
}

CellI& ShapeField::operator[](CellI& key)
{
    if (&key == &w.id.__type__) {
        return w.arc.ShapeField;
    }
    if (&key == &w.id.width) {
        return m_width;
    }
    if (&key == &w.id.height) {
        return m_height;
    }
    if (&key == &w.name("shapesMap")) {
        return m_shapesMap;
    }

    throw "No such key!";
}

void ShapeField::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

// getShape(pos) == shape
void ShapeField::addShape(Vector& offset, Shape& shape)
{
    m_shapesMap.addWithDataKey(offset, shape);
}

bool ShapeField::hasShape(Vector& offset, Shape& shape)
{
    if (m_shapesMap.hasValueWithDataKey(offset)) {
        Shape& storedShape = static_cast<Shape&>(m_shapesMap.getValueWithDataKey(offset));
        if (storedShape == shape) {
            return true;
        }
    }

    return false;
}

} // namespace arc
} // namespace cells
} // namespace infocell