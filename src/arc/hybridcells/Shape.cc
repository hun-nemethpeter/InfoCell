#include "Shape.h"

namespace infocell {
namespace cells {
namespace arc {

Shape::Shape(brain::Brain& kb, Number& id, CellI& color, Number& width, Number& height) :
    CellI(kb),
    m_id(id),
    m_color(color),
    m_width(width),
    m_height(height),
    m_lastEdgeId(&kb.pools.numbers.get(0))
{
    static CellI& ShapeEdgeStruct = kb.getStruct("arc::ShapeEdge");
    m_edges                       = new Map(kb, kb.std.Number, ShapeEdgeStruct);
}

bool Shape::has(CellI& role)
{
    static CellI& name_lastEdgeId    = kb.name("lastEdgeId");
    static CellI& name_hybridPixels  = kb.name("hybridPixels");
    static CellI& name_pixels        = kb.name("pixels");
    static CellI& name_shapePixels   = kb.name("shapePixels");
    static CellI& name_shapePoints   = kb.name("shapePoints");
    static CellI& name_edges         = kb.name("edges");
    static CellI& name_internalEdges = kb.name("internalEdges");

    if (&role == &kb.ids.struct_) {
        return true;
    }
    if (&role == &kb.ids.id) {
        return true;
    }
    if (&role == &kb.ids.color) {
        return true;
    }
    if (&role == &kb.ids.width) {
        return true;
    }
    if (&role == &kb.ids.height) {
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
    static CellI& name_lastEdgeId    = kb.name("lastEdgeId");
    static CellI& name_shapePixels   = kb.name("shapePixels");
    static CellI& name_shapePoints   = kb.name("shapePoints");
    static CellI& name_edges         = kb.name("edges");
    static CellI& name_internalEdges = kb.name("internalEdges");

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
    static CellI& ShapeStruct        = kb.getStruct("arc::Shape");
    static CellI& name_lastEdgeId    = kb.name("lastEdgeId");
    static CellI& name_shapePixels   = kb.name("shapePixels");
    static CellI& name_shapePoints   = kb.name("shapePoints");
    static CellI& name_edges         = kb.name("edges");
    static CellI& name_internalEdges = kb.name("internalEdges");

    if (&role == &kb.ids.struct_) {
        return ShapeStruct;
    }
    if (&role == &kb.ids.id) {
        return m_id;
    }
    if (&role == &kb.ids.color) {
        return m_color;
    }
    if (&role == &kb.ids.width) {
        return m_width;
    }
    if (&role == &kb.ids.height) {
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