#pragma once

#include "Vector.h"
#include "cells/World.h"

namespace infocell {
namespace cells {
namespace arc {

class RenderedShape : public CellI
{
public:
    RenderedShape(World& w, Number& id, CellI& color, Number& width, Number& height);

    bool has(CellI& role) override;
    void set(CellI& role, CellI& value) override;
    void erase(CellI& role) override;
    void operator()() override;
    CellI& operator[](CellI& role) override;

    Number& m_id;
    CellI& m_color;
    Number& m_width;
    Number& m_height;
    CellI* m_lastEdgeId;
    CellI* m_shapePixels   = nullptr;
    CellI* m_shapePoints   = nullptr;
    CellI* m_edges         = nullptr;
    CellI* m_internalEdges = nullptr;
};

class Shape : public cells::CellI
{
public:
    Shape(World& w, cells::CellI& color, List& externalEdgeLine, cells::TrieMap& internalEdges);

    using CellI::erase;
    using CellI::get;
    using CellI::has;
    using CellI::missing;
    using CellI::set;
    using CellI::operator[];

    bool has(CellI& key) override;
    void set(CellI& key, CellI& value) override;
    void erase(CellI& key) override;
    void operator()() override;
    CellI& operator[](CellI& key) override;

    bool operator==(Shape& rhs);
    void addInternalEdge(Vector& offset, List& edge);

    cells::CellI& m_color;
    List& m_externalEdgeLine;
    cells::TrieMap& m_internalEdges;
};

} // namespace arc
} // namespace cells
} // namespace infocell