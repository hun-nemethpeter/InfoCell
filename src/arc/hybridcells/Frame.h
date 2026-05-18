#pragma once

#include "Grid.h"
#include "cells/Brain.h"
#include "cells/Cells.h"

namespace infocell {
namespace cells {
namespace arc {

class Frame : public CellI
{
public:
    Frame(brain::Brain& kb, cells::arc::Grid& grid, CellI& ShapeStruct, CellI& TableRowStruct);

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
    cells::arc::Grid& m_grid;
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
} // namespace cells
} // namespace infocell