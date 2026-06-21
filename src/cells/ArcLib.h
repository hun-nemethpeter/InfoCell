#pragma once

#include "Library.h"

namespace infocell {
namespace cells {

class World;
class Arc
{
public:
    class EColor : public Object
    {
        EColor(World& w, CellI& type, const std::string& label, Arc& arc);
        friend class Arc;

    public:
        Object black;
        Object blue;
        Object red;
        Object green;
        Object yellow;
        Object grey;
        Object fuschia;
        Object orange;
        Object teal;
        Object brown;
    };

    class EDirection : public Object
    {
        EDirection(World& w, CellI& type, const std::string& label, Arc& arc);
        friend class Arc;

    public:
        Object up;
        Object upRight;
        Object right;
        Object downRight;
        Object down;
        Object downLeft;
        Object left;
        Object upLeft;
    };

    class ELineSymmetry : public Object
    {
        ELineSymmetry(World& w, CellI& type, const std::string& label, Arc& arc);
        friend class Arc;

    public:
        Object horizontal;
        Object vertical;
        Object diagonalLowerLeft;
        Object diagonalUpperLeft;
    };

    class ERotationDir : public Object
    {
        ERotationDir(World& w, CellI& type, const std::string& label, Arc& arc);
        friend class Arc;

    public:
        Object Degree_0;
        Object Degree_45;
        Object Degree_90;
        Object Degree_135;
        Object Degree_180;
        Object Degree_225;
        Object Degree_270;
        Object Degree_315;
    };

    class EShapeEdgeKind : public Object
    {
        EShapeEdgeKind(World& w, CellI& type, const std::string& label, Arc& arc);
        friend class Arc;

    public:
        Object ExternalEdge;
        Object InternalEdge;
    };

    Arc(World& w);

protected:
    World& w;

public:
    EColor Color;
    EDirection Direction;
    ELineSymmetry LineSymmetry;
    ERotationDir RotationDir;
    EShapeEdgeKind ShapeEdgeKind;

    Object Example;
    Object Frame;
    Object Pixel;
    Object Shape;
    Object ShapeEdge;
    Object ShapeEdgeJoint;
    Object ShapeEdgeMirroringCorners;
    Object ShapeEdgeNode;
    Object ShapeEdgeRotationCorners;
    Object ShapePixel;
    Object ShapePoint;
    Object Task;
    Object Vector;
    Object VectorShape;
};

class Compiler;
class ArcLib : public Library
{
public:
    ArcLib(World& w, Ast::Scope& parentScope, Compiler& compiler);
};

} // namespace cells
} // namespace infocell