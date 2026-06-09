#include "Shape.h"

namespace infocell {
namespace cells {
namespace arc {

Shape::Shape(World& w, Number& id, CellI& color, Number& width, Number& height) :
    CellI(w),
    m_id(id),
    m_color(color),
    m_width(width),
    m_height(height),
    m_lastEdgeId(&w.pools.numbers.get(0))
{
    static CellI& ShapeEdgeStruct = w.getStruct("arc::ShapeEdge");
    m_edges                       = new Map(w, w.std.Number, ShapeEdgeStruct);
}

bool Shape::has(CellI& role)
{
    static CellI& name_lastEdgeId    = w.name("lastEdgeId");
    static CellI& name_hybridPixels  = w.name("hybridPixels");
    static CellI& name_pixels        = w.name("pixels");
    static CellI& name_shapePixels   = w.name("shapePixels");
    static CellI& name_shapePoints   = w.name("shapePoints");
    static CellI& name_edges         = w.name("edges");
    static CellI& name_internalEdges = w.name("internalEdges");

    if (&role == &w.ids.struct_) {
        return true;
    }
    if (&role == &w.ids.id) {
        return true;
    }
    if (&role == &w.ids.color) {
        return true;
    }
    if (&role == &w.ids.width) {
        return true;
    }
    if (&role == &w.ids.height) {
        return true;
    }
    if (&role == &name_lastEdgeId) {
        return true;
    }
    if (&role == &name_hybridPixels) {
        return true;
    }
    if (&role == &name_pixels) {
        return true;
    }
    if (&role == &name_shapePixels && m_shapePixels) {
        return true;
    }
    if (&role == &name_shapePoints && m_shapePoints) {
        return true;
    }
    if (&role == &name_edges && m_edges) {
        return true;
    }
    if (&role == &name_internalEdges && m_internalEdges) {
        return true;
    }

    return false;
}


void Shape::set(CellI& role, CellI& value)
{
    static CellI& name_lastEdgeId    = w.name("lastEdgeId");
    static CellI& name_shapePixels   = w.name("shapePixels");
    static CellI& name_shapePoints   = w.name("shapePoints");
    static CellI& name_edges         = w.name("edges");
    static CellI& name_internalEdges = w.name("internalEdges");

    if (&role == &name_lastEdgeId) {
        m_lastEdgeId = &value;
        return;
    }
    if (&role == &name_shapePixels) {
        m_shapePixels = &value;
        return;
    }
    if (&role == &name_shapePoints) {
        m_shapePoints = &value;
        return;
    }
    if (&role == &name_edges) {
        m_edges = &value;
        return;
    }
    if (&role == &name_internalEdges) {
        m_internalEdges = &value;
        return;
    }

    throw "Not implemented!";
}

void Shape::erase(CellI& role)
{
    throw "Changing a hybrid shape cell is not possible!";
}

void Shape::operator()()
{
    // Do nothing
}

CellI& Shape::operator[](CellI& role)
{
    static CellI& ShapeStruct        = w.getStruct("arc::Shape");
    static CellI& name_lastEdgeId    = w.name("lastEdgeId");
    static CellI& name_shapePixels   = w.name("shapePixels");
    static CellI& name_shapePoints   = w.name("shapePoints");
    static CellI& name_edges         = w.name("edges");
    static CellI& name_internalEdges = w.name("internalEdges");

    if (&role == &w.ids.struct_) {
        return ShapeStruct;
    }
    if (&role == &w.ids.id) {
        return m_id;
    }
    if (&role == &w.ids.color) {
        return m_color;
    }
    if (&role == &w.ids.width) {
        return m_width;
    }
    if (&role == &w.ids.height) {
        return m_height;
    }
    if (&role == &name_lastEdgeId) {
        return *m_lastEdgeId;
    }
    if (&role == &name_shapePixels && m_shapePixels) {
        return *m_shapePixels;
    }
    if (&role == &name_shapePoints && m_shapePoints) {
        return *m_shapePoints;
    }
    if (&role == &name_edges && m_edges) {
        return *m_edges;
    }
    if (&role == &name_internalEdges && m_internalEdges) {
        return *m_internalEdges;
    }

    throw "No such role!";
}

void Shape::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

} // namespace arc
} // namespace cells
} // namespace infocell