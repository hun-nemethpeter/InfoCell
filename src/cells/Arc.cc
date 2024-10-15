#include "Cells.h"
#include "Brain.h"
#include "Arc.h"

namespace synth {
namespace cells {
namespace hybrid {
namespace arc {

Shape::Shape(brain::Brain& kb, Number& id, Number& color, Number& width, Number& height, CellI& PixelStruct) :
    CellI(kb),
    m_id(id),
    m_color(color),
    m_width(width),
    m_height(height),
    m_lastEdgeId(&kb.pools.numbers.get(1)),
    m_hybridPixels(kb, kb.std.Pixel),
    m_pixels(kb, PixelStruct)
{
}

bool Shape::has(CellI& role)
{
    static CellI& name_lastEdgeId   = kb.name("lastEdgeId");
    static CellI& name_hybridPixels = kb.name("hybridPixels");
    static CellI& name_pixels       = kb.name("pixels");
    static CellI& name_shapePixels  = kb.name("shapePixels");
    static CellI& name_shapePoints  = kb.name("shapePoints");

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

    return false;
}


void Shape::set(CellI& role, CellI& value)
{
    static CellI& name_lastEdgeId  = kb.name("lastEdgeId");
    static CellI& name_shapePixels = kb.name("shapePixels");
    static CellI& name_shapePoints = kb.name("shapePoints");
    static CellI& name_edges       = kb.name("edges");

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
    static CellI& ShapeStruct       = kb.getStruct("arc::Shape");
    static CellI& name_lastEdgeId   = kb.name("lastEdgeId");
    static CellI& name_hybridPixels = kb.name("hybridPixels");
    static CellI& name_pixels       = kb.name("pixels");
    static CellI& name_shapePixels  = kb.name("shapePixels");
    static CellI& name_shapePoints  = kb.name("shapePoints");
    static CellI& name_edges        = kb.name("edges");

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
    if (&role == &name_hybridPixels) {
        return m_hybridPixels;
    }
    if (&role == &name_pixels) {
        return m_pixels;
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

    throw "No such role!";
}

void Shape::accept(Visitor& visitor)
{
    //    visitor.visit(*this);
}

void Shape::addPixel(CellI& pixel)
{
    static CellI& PixelStruct = kb.getStruct("arc::Pixel");

    CellI& arcPixel = *new Object(kb, PixelStruct);
    arcPixel.set("x", pixel["x"]);
    arcPixel.set("y", pixel["y"]);
    m_pixels.add(pixel);
    m_hybridPixels.add(pixel);
}

Shaper::Shaper(brain::Brain& kb, cells::hybrid::arc::Grid& grid, CellI& ShapeStruct, CellI& TableRowStruct) :
    CellI(kb),
    m_width(grid.widthCell()),
    m_height(grid.heightCell()),
    m_grid(grid),
    m_shapePixels(kb, kb.std.Number, TableRowStruct),
    m_shapes(kb, ShapeStruct),
    m_shapeMap(kb, kb.std.Number, ShapeStruct),
    m_inputPixels(kb, kb.std.Pixel)
{
    processInputPixels();
}

void Shaper::processInputPixels()
{
    std::vector<cells::hybrid::arc::Pixel>& pixels = m_grid.pixels();
    for (auto& pixel : pixels) {
        m_inputPixels.add(pixel);
    }
}

bool Shaper::has(CellI& role)
{
    static CellI& name_grid        = kb.name("grid");
    static CellI& name_shapePixels = kb.name("shapePixels");
    static CellI& name_shapes      = kb.name("shapes");
    static CellI& name_shapeMap    = kb.name("shapeMap");
    static CellI& name_inputPixels = kb.name("inputPixels");

    if (&role == &kb.ids.struct_) {
        return true;
    }
    if (&role == &kb.ids.width) {
        return true;
    }
    if (&role == &kb.ids.height) {
        return true;
    }
    if (&role == &name_grid) {
        return true;
    }
    if (&role == &name_shapePixels) {
        return true;
    }
    if (&role == &name_shapes) {
        return true;
    }
    if (&role == &name_shapeMap) {
        return true;
    }
    if (&role == &name_inputPixels) {
        return true;
    }

    return false;
}

CellI& Shaper::operator[](CellI& role)
{
    static CellI& ShaperStruct = kb.getStruct("arc::Shaper");
    static CellI& name_grid        = kb.name("grid");
    static CellI& name_shapePixels = kb.name("shapePixels");
    static CellI& name_shapes      = kb.name("shapes");
    static CellI& name_shapeMap    = kb.name("shapeMap");
    static CellI& name_inputPixels = kb.name("inputPixels");

    if (&role == &kb.ids.struct_) {
        return ShaperStruct;
    }
    if (&role == &kb.ids.width) {
        return m_width;
    }
    if (&role == &kb.ids.height) {
        return m_height;
    }
    if (&role == &name_grid) {
        return m_grid;
    }
    if (&role == &name_shapePixels) {
        return m_shapePixels;
    }
    if (&role == &name_shapes) {
        return m_shapes;
    }
    if (&role == &name_shapeMap) {
        return m_shapeMap;
    }
    if (&role == &name_inputPixels) {
        return m_inputPixels;
    }

    throw "No such role!";
}

void Shaper::set(CellI& role, CellI& value)
{
    throw "Changing a hybrid shape cell is not possible!";
}

void Shaper::erase(CellI& role)
{
    throw "Changing a hybrid shape cell is not possible!";
}

void Shaper::operator()()
{
    // Do nothing
}

void Shaper::accept(Visitor& visitor)
{
    //    visitor.visit(*this);
}

void Shaper::process()
{
    static CellI& PixelStruct = kb.getStruct("arc::Pixel");

    const int height = m_height.value();
    const int width  = m_width.value();
    int shapeId      = 1;

    while (!m_inputPixels.empty()) {
        CellI& firstPixel = m_inputPixels.first();
        CellI& shape = *new Shape(kb, kb.pools.numbers.get(shapeId), static_cast<Number&>(firstPixel["color"]), m_width, m_height, PixelStruct);
        shapeId           = shapeId + 1;
        Set checkPixels(kb, kb.std.Pixel);
        checkPixels.add(firstPixel);
        while (!checkPixels.empty()) {
            CellI& checkPixel = checkPixels.first();
            processPixel(shape, checkPixels, checkPixel);
            checkPixels.remove(checkPixel);
        }
    }
    int y = 0;
    while (y < height) {
        Map& colX = static_cast<Map&>(m_shapePixels.getValue(kb.pools.numbers.get(y)));
        int x     = 0;
        while (x < width) {
            CellI& shapePixel = colX.getValue(kb.pools.numbers.get(x));
            Shape& shape      = static_cast<Shape&>(shapePixel["shape"]);
            CellI& pixel      = shapePixel["pixel"];
            shape.addPixel(pixel);
            if (!m_shapeMap.hasKey(shape[kb.ids.id])) {
                m_shapeMap.add(shape[kb.ids.id], shape);
                m_shapes.add(shape);
            }
            x = x + 1;
        }
        y = y + 1;
    }
}

void Shaper::processPixel(CellI& shape, Set& checkPixels, CellI& checkPixel)
{
    static CellI& ShapeStruct      = kb.getStruct("arc::Shape");
    static CellI& ShapePixelStruct = kb.getStruct("arc::ShapePixel");
    static CellI& TableRowStruct   = kb.getStruct(kb.templateId("std::Map", kb.ids.keyType, kb.std.Number, kb.ids.valueType, ShapeStruct));

    if (!m_shapePixels.hasKey(checkPixel["y"])) {
        m_shapePixels.add(checkPixel["y"], *new Map(kb, kb.std.Number, TableRowStruct));
    }
    Map& colX = static_cast<Map&>(m_shapePixels.getValue(checkPixel["y"]));
    CellI& shapePixel = *new Object(kb, ShapePixelStruct);
    shapePixel.set("shape", shape);
    shapePixel.set("pixel", checkPixel);
    colX.add(checkPixel["x"], shapePixel);
    m_inputPixels.remove(checkPixel);
    CellI* pixel = processAdjacentPixel(kb.directions.up, shape, checkPixels, checkPixel);
    if (pixel) {
        processAdjacentPixel(kb.directions.left, shape, checkPixels, *pixel);
        processAdjacentPixel(kb.directions.right, shape, checkPixels, *pixel);
    }
    pixel = processAdjacentPixel(kb.directions.down, shape, checkPixels, checkPixel);
    if (pixel) {
        processAdjacentPixel(kb.directions.left, shape, checkPixels, *pixel);
        processAdjacentPixel(kb.directions.right, shape, checkPixels, *pixel);
    }
    processAdjacentPixel(kb.directions.left, shape, checkPixels, checkPixel);
    processAdjacentPixel(kb.directions.right, shape, checkPixels, checkPixel);
}

CellI* Shaper::processAdjacentPixel(CellI& direction, CellI& p_shape, Set& checkPixels, CellI& checkPixel)
{
    if (checkPixel.has(direction)) {
        CellI& pixel = checkPixel[direction];
        if (m_shapePixels.hasKey(pixel["y"])) {
            Map& colX = static_cast<Map&>(m_shapePixels.getValue(pixel["y"]));
            if (colX.hasKey(pixel["x"])) {
                CellI& shape = colX.getValue(pixel["x"])["shape"];
                if (&p_shape == &shape) {
                    return &pixel;
                }
            }
        }
        if (static_cast<Number&>(pixel["color"]).value() == static_cast<Number&>(p_shape["color"]).value()) {
            checkPixels.add(pixel);
        }
        return &pixel;
    }

    return nullptr;
}

} // namespace arc
} // namespace hybrid
} // namespace cells
} // namespace synth