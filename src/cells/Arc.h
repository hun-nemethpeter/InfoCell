#include "Brain.h"
#include "Cells.h"

namespace infocell {
namespace cells {
namespace hybrid {
namespace arc {

class EdgeRelation
{
public:
    bool isRelated() const
    {
        return m_rotatedWith || m_isHorizontallyMirrored || m_isVerticallyMirrored;
    }

    bool isUnrelated() const
    {
        return !m_rotatedWith && !m_isHorizontallyMirrored && !m_isVerticallyMirrored;
    }

    CellI* m_rotatedWith          = nullptr;
    bool m_isHorizontallyMirrored = false;
    bool m_isVerticallyMirrored   = false;
};

class ShapeRelation
{
public:
    std::vector<EdgeRelation> m_edgeRelations; // TODO
};

// compare two ShapeEdge
EdgeRelation compareEdges(CellI& lhs, CellI& rhs);

// compare two Shape
ShapeRelation compareShapes(CellI& lhs, CellI& rhs);

class Shape : public CellI
{
public:
    Shape(brain::Brain& kb, Number& id, Number& color, Number& width, Number& height, CellI& PixelStruct);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void erase(CellI& role) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

    void addPixel(CellI& pixel);


    Number& m_id;
    Number& m_color;
    Number& m_width;
    Number& m_height;
    CellI* m_lastEdgeId;
    Set m_hybridPixels;
    List m_pixels;
    CellI* m_shapePixels   = nullptr;
    CellI* m_shapePoints   = nullptr;
    CellI* m_edges         = nullptr;
    CellI* m_internalEdges = nullptr;
};

class Shaper : public CellI
{
public:
    Shaper(brain::Brain& kb, cells::hybrid::arc::Grid& grid, CellI& ShapeStruct, CellI& TableRowStruct);

    void process();

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void erase(CellI& role) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

protected:
    void processInputPixels();
    void processPixel(CellI& shape, Set& checkPixels, CellI& checkPixel);
    void processAdjacentPixel(CellI& direction, CellI& shape, Set& checkPixels, CellI& checkPixel);

    Number& m_width;
    Number& m_height;
    cells::hybrid::arc::Grid& m_grid;
    List* m_frameEdgeNodes = nullptr;
    Map m_shapePixels;
    List m_shapes;
    Map m_shapeMap;
    Set m_inputPixels;
    CellI* m_upLeftPoint = nullptr;
    CellI* m_upRightPoint = nullptr;
    CellI* m_downLeftPoint = nullptr;
    CellI* m_downRightPoint = nullptr;
};

} // namespace arc
} // namespace hybrid
} // namespace cells
} // namespace infocell