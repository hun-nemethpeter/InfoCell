#include "Frame.h"
#include "Shape.h"

namespace infocell {
namespace cells {
namespace arc {

Frame::Frame(brain::Brain& kb, cells::arc::Grid& grid, CellI& ShapeStruct, CellI& TableRowStruct) :
    CellI(kb),
    m_width(grid.widthCell()),
    m_height(grid.heightCell()),
    m_grid(grid),
    m_shapePixels(kb, kb.std.Number, TableRowStruct),
    m_shapes(kb, ShapeStruct),
    m_shapeMap(kb, kb.std.Number, ShapeStruct),
    m_inputPixels(kb, kb.std.Pixel)
{
    static CellI& ShapeEdgeNodeStruct = kb.getStruct("arc::ShapeEdgeNode");
    m_frameEdgeNodes                  = new List(kb, ShapeEdgeNodeStruct);
    processInputPixels();
}

void Frame::processInputPixels()
{
    std::vector<cells::arc::Pixel>& pixels = m_grid.pixels();
    for (auto& pixel : pixels) {
        m_inputPixels.add(pixel);
    }
}

bool Frame::has(CellI& role)
{
    static CellI& name_grid        = kb.name("grid");
    static CellI& name_shapePixels = kb.name("shapePixels");
    static CellI& name_shapes      = kb.name("shapes");
    static CellI& name_shapeMap    = kb.name("shapeMap");
    static CellI& name_inputPixels = kb.name("inputPixels");
    static CellI& name_upLeftPoint    = kb.name("upLeftPoint");
    static CellI& name_upRightPoint   = kb.name("upRightPoint");
    static CellI& name_downLeftPoint  = kb.name("downLeftPoint");
    static CellI& name_downRightPoint = kb.name("downRightPoint");

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
    if (&role == &name_upLeftPoint) {
        return m_upLeftPoint;
    }
    if (&role == &name_upRightPoint) {
        return m_upRightPoint;
    }
    if (&role == &name_downLeftPoint) {
        return m_downLeftPoint;
    }
    if (&role == &name_downRightPoint) {
        return m_downRightPoint;
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

CellI& Frame::operator[](CellI& role)
{
    static CellI& FrameStruct         = kb.getStruct("arc::Frame");
    static CellI& name_grid           = kb.name("grid");
    static CellI& name_shapePixels    = kb.name("shapePixels");
    static CellI& name_shapes         = kb.name("shapes");
    static CellI& name_shapeMap       = kb.name("shapeMap");
    static CellI& name_inputPixels    = kb.name("inputPixels");
    static CellI& name_upLeftPoint    = kb.name("upLeftPoint");
    static CellI& name_upRightPoint   = kb.name("upRightPoint");
    static CellI& name_downLeftPoint  = kb.name("downLeftPoint");
    static CellI& name_downRightPoint = kb.name("downRightPoint");

    if (missing(role)) {
        throw "No such role!";
    }
    if (&role == &kb.ids.struct_) {
        return FrameStruct;
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
    if (&role == &name_upLeftPoint) {
        return *m_upLeftPoint;
    }
    if (&role == &name_upRightPoint) {
        return *m_upRightPoint;
    }
    if (&role == &name_downLeftPoint) {
        return *m_downLeftPoint;
    }
    if (&role == &name_downRightPoint) {
        return *m_downRightPoint;
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

void Frame::set(CellI& role, CellI& value)
{
    static CellI& name_upLeftPoint    = kb.name("upLeftPoint");
    static CellI& name_upRightPoint   = kb.name("upRightPoint");
    static CellI& name_downLeftPoint  = kb.name("downLeftPoint");
    static CellI& name_downRightPoint = kb.name("downRightPoint");

    if (&role == &name_upLeftPoint) {
        m_upLeftPoint = &value;
    }
    if (&role == &name_upRightPoint) {
        m_upRightPoint = &value;
    }
    if (&role == &name_downLeftPoint) {
        m_downLeftPoint = &value;
    }
    if (&role == &name_downRightPoint) {
        m_downRightPoint = &value;
    }
}

void Frame::erase(CellI& role)
{
    throw "Changing a hybrid shape cell is not possible!";
}

void Frame::operator()()
{
    // Do nothing
}

void Frame::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

void Frame::process()
{
    const int height = m_height.value();
    const int width  = m_width.value();
    int shapeId      = 1;

    while (!m_inputPixels.empty()) {
        CellI& firstPixel = m_inputPixels.first();
        CellI& shape = *new Shape(kb, kb.pools.numbers.get(shapeId), static_cast<Number&>(firstPixel["color"]), m_width, m_height);
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
            if (!m_shapeMap.hasKey(shape[kb.ids.id])) {
                m_shapeMap.add(shape[kb.ids.id], shape);
                m_shapes.add(shape);
            }
            x = x + 1;
        }
        y = y + 1;
    }
}

void Frame::processPixel(CellI& shape, Set& checkPixels, CellI& checkPixel)
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

    processAdjacentPixel(kb.directions.up, shape, checkPixels, checkPixel);
    processAdjacentPixel(kb.directions.down, shape, checkPixels, checkPixel);
    processAdjacentPixel(kb.directions.left, shape, checkPixels, checkPixel);
    processAdjacentPixel(kb.directions.right, shape, checkPixels, checkPixel);
}

void Frame::processAdjacentPixel(CellI& direction, CellI& p_shape, Set& checkPixels, CellI& checkPixel)
{
    if (checkPixel.missing(direction)) {
        return;
    }
    CellI& pixel = checkPixel[direction];
    if (m_shapePixels.hasKey(pixel["y"])) {
        Map& colX = static_cast<Map&>(m_shapePixels.getValue(pixel["y"]));
        if (colX.hasKey(pixel["x"])) {
            CellI& shape = colX.getValue(pixel["x"])["shape"];
            if (&p_shape == &shape) {
                return;
            }
        }
    }
    if (static_cast<Number&>(pixel["color"]).value() == static_cast<Number&>(p_shape["color"]).value()) {
        checkPixels.add(pixel);
    }
}

} // namespace arc
} // namespace cells
} // namespace infocell