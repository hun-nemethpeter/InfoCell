#pragma once

#include "cells/Brain.h"
#include "cells/Cells.h"

namespace infocell {
namespace cells {
namespace arc {

class Shape : public CellI
{
public:
    Shape(brain::Brain& kb, Number& id, Number& color, Number& width, Number& height);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void erase(CellI& role) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;
    void accept(Visitor& visitor) override;

    Number& m_id;
    Number& m_color;
    Number& m_width;
    Number& m_height;
    CellI* m_lastEdgeId;
    CellI* m_shapePixels   = nullptr;
    CellI* m_shapePoints   = nullptr;
    CellI* m_edges         = nullptr;
    CellI* m_internalEdges = nullptr;
};

} // namespace arc
} // namespace cells
} // namespace infocell