#pragma once

#include "cells/Cells.h"

namespace infocell {
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

    cells::CellI* m_rotatedWith   = nullptr;
    bool m_isHorizontallyMirrored = false;
    bool m_isVerticallyMirrored   = false;
};

class ShapeRelation
{
public:
    std::vector<EdgeRelation> m_edgeRelations; // TODO
};

cells::CellI* isSymmetric(cells::CellI& edge);

// compare two ShapeEdge
EdgeRelation compareEdges(cells::CellI& lhs, cells::CellI& rhs);

// compare two Shape
ShapeRelation compareShapes(cells::CellI& lhs, cells::CellI& rhs);

} // namespace arc
} // namespace infocell