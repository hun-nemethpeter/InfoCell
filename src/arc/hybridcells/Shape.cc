#include "Shape.h"

namespace infocell {
namespace cells {
namespace arc {

RenderedShape::RenderedShape(World& w, Number& id, CellI& color, Number& width, Number& height) :
    CellI(w),
    m_id(id),
    m_color(color),
    m_width(width),
    m_height(height),
    m_lastEdgeId(&w.pools.numbers.get(0))
{
    m_edges = new Map(w, w.std.Number, w.arc.ShapeEdge);
}

bool RenderedShape::has(CellI& role)
{
    static CellI& name_lastEdgeId    = w.name("lastEdgeId");
    static CellI& name_hybridPixels  = w.name("hybridPixels");
    static CellI& name_pixels        = w.name("pixels");
    static CellI& name_shapePixels   = w.name("shapePixels");
    static CellI& name_shapePoints   = w.name("shapePoints");
    static CellI& name_edges         = w.name("edges");
    static CellI& name_internalEdges = w.name("internalEdges");

    if (&role == &w.id.__type__) {
        return true;
    }
    if (&role == &w.id.id) {
        return true;
    }
    if (&role == &w.id.color) {
        return true;
    }
    if (&role == &w.id.width) {
        return true;
    }
    if (&role == &w.id.height) {
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


void RenderedShape::set(CellI& role, CellI& value)
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

void RenderedShape::erase(CellI& role)
{
    throw "Changing a hybrid shape cell is not possible!";
}

void RenderedShape::operator()()
{
    // Do nothing
}

CellI& RenderedShape::operator[](CellI& role)
{
    static CellI& name_lastEdgeId    = w.name("lastEdgeId");
    static CellI& name_shapePixels   = w.name("shapePixels");
    static CellI& name_shapePoints   = w.name("shapePoints");
    static CellI& name_edges         = w.name("edges");
    static CellI& name_internalEdges = w.name("internalEdges");

    if (&role == &w.id.__type__) {
        return w.arc.RenderedShape;
    }
    if (&role == &w.id.id) {
        return m_id;
    }
    if (&role == &w.id.color) {
        return m_color;
    }
    if (&role == &w.id.width) {
        return m_width;
    }
    if (&role == &w.id.height) {
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

Shape::Shape(World& w, cells::CellI& color, List& externalEdgeLine, cells::TrieMap& internalEdges) :
    cells::CellI(w),
    m_color(color),
    m_externalEdgeLine(externalEdgeLine),
    m_internalEdges(internalEdges)
{
}

bool Shape::has(CellI& key)
{
    if (&key == &w.id.__type__) {
        return true;
    }
    if (&key == &w.name("externalEdgeLine")) {
        return true;
    }
    if (&key == &w.id.color) {
        return true;
    }
    if (&key == &w.name("internalEdges")) {
        return true;
    }

    return false;
}

void Shape::set(CellI& key, CellI& value)
{
    throw "Changing a hybrid offset cell is not possible!";
}

void Shape::erase(CellI& key)
{
    throw "Changing a hybrid offset cell is not possible!";
}

void Shape::operator()()
{
    // Do nothing, this is a data cell
}

CellI& Shape::operator[](CellI& key)
{
    if (&key == &w.id.__type__) {
        return w.arc.Shape;
    }
    if (&key == &w.name("externalEdgeLine")) {
        return m_externalEdgeLine;
    }
    if (&key == &w.id.color) {
        return m_color;
    }
    if (&key == &w.name("internalEdges")) {
        return m_internalEdges;
    }

    throw "No such key!";
}

bool Shape::operator==(Shape& rhs)
{
    if (&m_color != &rhs.m_color) {
        return false;
    }
    if (m_externalEdgeLine.size() != rhs.m_externalEdgeLine.size()) {
        return false;
    }
    if (m_internalEdges.size() != rhs.m_internalEdges.size()) {
        return false;
    }
    List& rhsExternalEdgeLine = rhs.m_externalEdgeLine;
    CellI* rhsDirNodePtr      = &rhsExternalEdgeLine[w.id.first];
    bool result               = true;
    forEach(m_externalEdgeLine, [this, &rhsExternalEdgeLine, &rhsDirNodePtr, &result](CellI& dir, int i, bool& stop) {
        CellI& rhsDir = (*rhsDirNodePtr)[w.id.value];
        if (&dir != &rhsDir) {
            stop   = true;
            result = false;
            return;
        }
        rhsDirNodePtr = &(*rhsDirNodePtr)[w.id.next];
    });

    return true;
}

// getShape(pos) == shape
void Shape::addInternalEdge(Vector& offset, List& edge)
{
    m_internalEdges.addWithDataKey(offset, edge);
}

} // namespace arc
} // namespace cells
} // namespace infocell