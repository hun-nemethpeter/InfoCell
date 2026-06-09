#include "Frame.h"
#include "Shape.h"

namespace infocell {
namespace cells {
namespace arc {

Frame::Frame(World& w, cells::arc::Grid& grid, CellI& ShapeStruct, CellI& TableRowStruct) :
    CellI(w),
    m_width(grid.widthCell()),
    m_height(grid.heightCell()),
    m_grid(grid),
    m_shapePixels(w, w.std.Number, TableRowStruct),
    m_shapes(w, ShapeStruct),
    m_shapeMap(w, w.std.Number, ShapeStruct),
    m_inputPixels(w, w.std.Pixel)
{
    static CellI& ShapeEdgeNodeStruct = w.getStruct("arc::ShapeEdgeNode");
    m_frameEdgeNodes                  = new List(w, ShapeEdgeNodeStruct);
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
    static CellI& name_grid        = w.name("grid");
    static CellI& name_shapePixels = w.name("shapePixels");
    static CellI& name_shapes      = w.name("shapes");
    static CellI& name_shapeMap    = w.name("shapeMap");
    static CellI& name_inputPixels = w.name("inputPixels");
    static CellI& name_upLeftPoint    = w.name("upLeftPoint");
    static CellI& name_upRightPoint   = w.name("upRightPoint");
    static CellI& name_downLeftPoint  = w.name("downLeftPoint");
    static CellI& name_downRightPoint = w.name("downRightPoint");

    if (&role == &w.ids.struct_) {
        return true;
    }
    if (&role == &w.ids.width) {
        return true;
    }
    if (&role == &w.ids.height) {
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
    static CellI& FrameStruct         = w.getStruct("arc::Frame");
    static CellI& name_grid           = w.name("grid");
    static CellI& name_shapePixels    = w.name("shapePixels");
    static CellI& name_shapes         = w.name("shapes");
    static CellI& name_shapeMap       = w.name("shapeMap");
    static CellI& name_inputPixels    = w.name("inputPixels");
    static CellI& name_upLeftPoint    = w.name("upLeftPoint");
    static CellI& name_upRightPoint   = w.name("upRightPoint");
    static CellI& name_downLeftPoint  = w.name("downLeftPoint");
    static CellI& name_downRightPoint = w.name("downRightPoint");

    if (missing(role)) {
        throw "No such role!";
    }
    if (&role == &w.ids.struct_) {
        return FrameStruct;
    }
    if (&role == &w.ids.width) {
        return m_width;
    }
    if (&role == &w.ids.height) {
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
    static CellI& name_upLeftPoint    = w.name("upLeftPoint");
    static CellI& name_upRightPoint   = w.name("upRightPoint");
    static CellI& name_downLeftPoint  = w.name("downLeftPoint");
    static CellI& name_downRightPoint = w.name("downRightPoint");

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
        CellI& shape = *new Shape(w, w.pools.numbers.get(shapeId), firstPixel["color"], m_width, m_height);
        shapeId           = shapeId + 1;
        Set checkPixels(w, w.std.Pixel);
        checkPixels.add(firstPixel);
        while (!checkPixels.empty()) {
            CellI& checkPixel = checkPixels.first();
            processPixel(shape, checkPixels, checkPixel);
            checkPixels.remove(checkPixel);
        }
    }
    int y = 0;
    while (y < height) {
        Map& colX = static_cast<Map&>(m_shapePixels.getValue(w.pools.numbers.get(y)));
        int x     = 0;
        while (x < width) {
            CellI& shapePixel = colX.getValue(w.pools.numbers.get(x));
            Shape& shape      = static_cast<Shape&>(shapePixel["shape"]);
            CellI& pixel      = shapePixel["pixel"];
            if (!m_shapeMap.hasKey(shape[w.ids.id])) {
                m_shapeMap.add(shape[w.ids.id], shape);
                m_shapes.add(shape);
            }
            x = x + 1;
        }
        y = y + 1;
    }
}

void Frame::processPixel(CellI& shape, Set& checkPixels, CellI& checkPixel)
{
    static CellI& ShapeStruct      = w.getStruct("arc::Shape");
    static CellI& ShapePixelStruct = w.getStruct("arc::ShapePixel");
    static CellI& TableRowStruct   = w.getStruct(w.templateId("std::Map", w.ids.keyType, w.std.Number, w.ids.valueType, ShapeStruct));

    if (!m_shapePixels.hasKey(checkPixel["y"])) {
        m_shapePixels.add(checkPixel["y"], *new Map(w, w.std.Number, TableRowStruct));
    }
    Map& colX = static_cast<Map&>(m_shapePixels.getValue(checkPixel["y"]));
    CellI& shapePixel = *new Object(w, ShapePixelStruct);
    shapePixel.set("shape", shape);
    shapePixel.set("pixel", checkPixel);
    colX.add(checkPixel["x"], shapePixel);
    m_inputPixels.remove(checkPixel);

    processAdjacentPixel(w.directions.up, shape, checkPixels, checkPixel);
    processAdjacentPixel(w.directions.down, shape, checkPixels, checkPixel);
    processAdjacentPixel(w.directions.left, shape, checkPixels, checkPixel);
    processAdjacentPixel(w.directions.right, shape, checkPixels, checkPixel);
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
    if (&pixel["color"] == &p_shape["color"]) {
        checkPixels.add(pixel);
    }
}

} // namespace arc
} // namespace cells
} // namespace infocell