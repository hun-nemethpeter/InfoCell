#include "Brain.h"
#include "Cells.h"

namespace synth {
namespace cells {
namespace hybrid {
namespace arc {

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
    CellI* m_shapePixels = nullptr;
    CellI* m_shapePoints = nullptr;
    CellI* m_edges = nullptr;
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
    CellI* processAdjacentPixel(CellI& direction, CellI& shape, Set& checkPixels, CellI& checkPixel);

    Number& m_width;
    Number& m_height;
    cells::hybrid::arc::Grid& m_grid;
    Map m_shapePixels;
    List m_shapes;
    Map m_shapeMap;
    Set m_inputPixels;
};

} // namespace arc
} // namespace hybrid
} // namespace cells
} // namespace synth