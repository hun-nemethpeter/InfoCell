#include "cells/World.h"

#include "Observers.h"

using namespace infocell::cells;

namespace infocell {
namespace arc {

cells::CellI* isSymmetric(cells::CellI& edge)
{
    World& w = edge.w;

    // TODO

    return nullptr;
}

static EdgeRelation compareEdges(CellI& lhs, CellI& rhs, CellI& transformation)
{
    World& w = lhs.w;
    Arc& arc = w.arc;

    EdgeRelation result;

    List& lhsEdgeNodes = static_cast<List&>(lhs["edgeNodes"]);
    List& rhsEdgeNodes = static_cast<List&>(rhs["edgeNodes"]);
    if (lhsEdgeNodes.size() != rhsEdgeNodes.size()) {
        return result;
    }
    // we expect at least a pixel which has 4 sides
    if (lhsEdgeNodes.size() < 4) {
        return result;
    }

    auto rotateDirection = [&arc](CellI& direction, CellI& degree) -> CellI& {
        if (&degree == &arc.RotationDir.Degree_0) {
            return direction;
        } else if (&degree == &arc.RotationDir.Degree_90) {
            if (&direction == &arc.Direction.up) {
                return arc.Direction.right;
            } else if (&direction == &arc.Direction.right) {
                return arc.Direction.down;
            } else if (&direction == &arc.Direction.down) {
                return arc.Direction.left;
            } else if (&direction == &arc.Direction.left) {
                return arc.Direction.up;
            }
        } else if (&degree == &arc.RotationDir.Degree_180) {
            if (&direction == &arc.Direction.up) {
                return arc.Direction.down;
            } else if (&direction == &arc.Direction.right) {
                return arc.Direction.left;
            } else if (&direction == &arc.Direction.down) {
                return arc.Direction.up;
            } else if (&direction == &arc.Direction.left) {
                return arc.Direction.right;
            }
        } else if (&degree == &arc.RotationDir.Degree_270) {
            if (&direction == &arc.Direction.up) {
                return arc.Direction.left;
            } else if (&direction == &arc.Direction.right) {
                return arc.Direction.up;
            } else if (&direction == &arc.Direction.down) {
                return arc.Direction.right;
            } else if (&direction == &arc.Direction.left) {
                return arc.Direction.down;
            }
        }
        throw "error";
    };

    auto mirrorDirection = [&arc](CellI& direction, CellI& mirroringMode) -> CellI& {
        if (&mirroringMode == &arc.RotationDir.Degree_0) {
            return direction;
        } else if (&mirroringMode == &arc.LineSymmetry.horizontal) {
            if (&direction == &arc.Direction.up) {
                return arc.Direction.up;
            } else if (&direction == &arc.Direction.right) {
                return arc.Direction.left;
            } else if (&direction == &arc.Direction.down) {
                return arc.Direction.down;
            } else if (&direction == &arc.Direction.left) {
                return arc.Direction.right;
            }
        } else if (&mirroringMode == &arc.LineSymmetry.vertical) {
            if (&direction == &arc.Direction.up) {
                return arc.Direction.down;
            } else if (&direction == &arc.Direction.right) {
                return arc.Direction.right;
            } else if (&direction == &arc.Direction.down) {
                return arc.Direction.up;
            } else if (&direction == &arc.Direction.left) {
                return arc.Direction.left;
            }
        }
        throw "error";
    };

    if (&transformation == &arc.RotationDir.Degree_0 || &transformation == &arc.RotationDir.Degree_90 || &transformation == &arc.RotationDir.Degree_180 || &transformation == &arc.RotationDir.Degree_270) {
        CellI& rotationDegree = transformation;
        const char* firstCorner  = "";
        if (&transformation == &arc.RotationDir.Degree_0) {
            firstCorner = "upLeftNode";
        } else if (&transformation == &arc.RotationDir.Degree_90) {
            firstCorner = "upRightNode";
        } else if (&transformation == &arc.RotationDir.Degree_180) {
            firstCorner = "downRightNode";
        } else if (&transformation == &arc.RotationDir.Degree_270) {
            firstCorner = "downLeftNode";
        }
        CellI* lhsEdgeNodePtr   = &lhsEdgeNodes[w.id.first][w.id.value];
        CellI* rhsEdgeNodePtr   = &rhs["rotationCorners"][firstCorner];
        CellI* firstNodePtr     = lhsEdgeNodePtr;
        bool found              = true;
        do {
            CellI& lhsEdgeNode         = *lhsEdgeNodePtr;
            CellI& rhsEdgeNode         = *rhsEdgeNodePtr;
            CellI& rotatedLhsDirection = rotateDirection(lhsEdgeNode["direction"], rotationDegree);
            // std::cout << fmt::format("rhs: ({},{}), lhs: ({},{})\n", rhsEdgeNode["from"]["x"].label(), rhsEdgeNode["from"]["y"].label(), lhsEdgeNode["from"]["x"].label(), lhsEdgeNode["from"]["y"].label());
            // std::cout << fmt::format("{} {}->{} {}\n", rotationDegree.label(), lhsEdgeNode["direction"].label(), rotatedLhsDirection.label(), rhsEdgeNode["direction"].label());
            if (&rotatedLhsDirection != &rhsEdgeNode["direction"]) {
                found = false;
                break;
            }
            lhsEdgeNodePtr = &lhsEdgeNode[w.id.next];
            rhsEdgeNodePtr = &rhsEdgeNode[w.id.next];
        } while (lhsEdgeNodePtr != firstNodePtr);

        if (found) {
            result.m_rotatedWith = &rotationDegree;
            return result;
        }

        return result;
    }

    if (&transformation == &arc.LineSymmetry.horizontal || &transformation == &arc.LineSymmetry.vertical) {
        CellI& lineSymmetry     = transformation;
        const char* firstCorner = "";
        if (&transformation == &arc.LineSymmetry.horizontal) {
            firstCorner = "downLeftNode";
        } else if (&transformation == &arc.LineSymmetry.vertical) {
            firstCorner = "upRightNode";
        }
        CellI* lhsEdgeNodePtr   = &lhsEdgeNodes[w.id.first][w.id.value];
        CellI* rhsEdgeNodePtr   = &rhs["mirroringCorners"][firstCorner];
        CellI* firstNodePtr     = lhsEdgeNodePtr;
        bool found              = true;
        do {
            CellI& lhsEdgeNode          = *lhsEdgeNodePtr;
            CellI& rhsEdgeNode          = *rhsEdgeNodePtr;
            CellI& mirroredLhsDirection = mirrorDirection(lhsEdgeNode["direction"], lineSymmetry);
            // std::cout << fmt::format("90 ({},{})\n", rhsEdgeNode["from"]["x"].label(), rhsEdgeNode["from"]["y"].label());
            // std::cout << fmt::format("{} {}->{} {}\n", lineSymmetry.label(), lhsEdgeNode["direction"].label(), mirroredLhsDirection.label(), rhsEdgeNode["direction"].label());
            if (&mirroredLhsDirection != &rhsEdgeNode["direction"]) {
                found = false;
                break;
            }
            lhsEdgeNodePtr = &lhsEdgeNode[w.id.next];
            rhsEdgeNodePtr = &rhsEdgeNode[w.id.previous];
        } while (lhsEdgeNodePtr != firstNodePtr);

        if (found) {
            if (&lineSymmetry == &arc.LineSymmetry.horizontal) {
                result.m_isHorizontallyMirrored = true;
            }
            if (&lineSymmetry == &arc.LineSymmetry.vertical) {
                result.m_isVerticallyMirrored = true;
            }
        }
        return result;
    }

    return result;
}

EdgeRelation compareEdges(CellI& lhs, CellI& rhs)
{
    World& w = lhs.w;
    Arc& arc = w.arc;

    EdgeRelation result;

    for (CellI* transformation : { &arc.RotationDir.Degree_0, &arc.RotationDir.Degree_90, &arc.RotationDir.Degree_180, &arc.RotationDir.Degree_270 }) {
        EdgeRelation result = compareEdges(lhs, rhs, *transformation);
        if (result.m_rotatedWith) {
            return result;
        }
    }
    for (CellI* transformation : { &arc.LineSymmetry.horizontal, &arc.LineSymmetry.vertical }) {
        EdgeRelation mirrorResult = compareEdges(lhs, rhs, *transformation);
        if (mirrorResult.m_isHorizontallyMirrored) {
            result.m_isHorizontallyMirrored = true;
        }
        if (mirrorResult.m_isVerticallyMirrored) {
            result.m_isVerticallyMirrored = true;
        }
    }

    return result;
}

ShapeRelation compareShapes(CellI& lhs, CellI& rhs)
{
    World& w = lhs.w;
    Arc& arc = w.arc;

    ShapeRelation result;
    List& lhsEdges = static_cast<List&>(lhs["edges"]["list"]);
    List& rhsEdges = static_cast<List&>(rhs["edges"]["list"]);

    if (lhsEdges.empty() || rhsEdges.empty()) {
        return result;
    }

    auto getEdge = [&w, &arc](CellI& transformation, CellI& lhsEdge, CellI& firstLhsEdge, CellI& rhsShape) -> CellI* {
        CellI* fromExternalX = nullptr;
        CellI* fromExternalY = nullptr;

        if (&transformation == &arc.RotationDir.Degree_0) {
            fromExternalX = &lhsEdge["fromExternalX"];
            fromExternalY = &lhsEdge["fromExternalY"];
        } else {
            if (&transformation == &arc.RotationDir.Degree_90) {
                int firstEdgeCornerX   = static_cast<Number&>(firstLhsEdge["rotationCorners"]["downLeftNode"]["from"]["x"]).value();
                int firstEdgeCornerY   = static_cast<Number&>(firstLhsEdge["rotationCorners"]["downLeftNode"]["from"]["y"]).value();
                int currentEdgeCornerX = static_cast<Number&>(lhsEdge["rotationCorners"]["downLeftNode"]["from"]["x"]).value();
                int currentEdgeCornerY = static_cast<Number&>(lhsEdge["rotationCorners"]["downLeftNode"]["from"]["y"]).value();

                int vectorX = currentEdgeCornerX - firstEdgeCornerX;
                int vectorY = currentEdgeCornerY - firstEdgeCornerY;

                // rotate vector 90
                int rotatedVectorX = -vectorY;
                int rotatedVectorY = vectorX;

                fromExternalX = &w.pools.numbers.get(rotatedVectorX);
                fromExternalY = &w.pools.numbers.get(rotatedVectorY);
            } else if (&transformation == &arc.RotationDir.Degree_180) {
                int firstEdgeCornerX   = static_cast<Number&>(firstLhsEdge["rotationCorners"]["downRightNode"]["from"]["x"]).value();
                int firstEdgeCornerY   = static_cast<Number&>(firstLhsEdge["rotationCorners"]["downRightNode"]["from"]["y"]).value();
                int currentEdgeCornerX = static_cast<Number&>(lhsEdge["rotationCorners"]["downRightNode"]["from"]["x"]).value();
                int currentEdgeCornerY = static_cast<Number&>(lhsEdge["rotationCorners"]["downRightNode"]["from"]["y"]).value();

                int vectorX = currentEdgeCornerX - firstEdgeCornerX;
                int vectorY = currentEdgeCornerY - firstEdgeCornerY;

                // rotate vector 180
                int rotatedVectorX = -vectorX;
                int rotatedVectorY = -vectorY;

                fromExternalX = &w.pools.numbers.get(rotatedVectorX);
                fromExternalY = &w.pools.numbers.get(rotatedVectorY);
            } else if (&transformation == &arc.RotationDir.Degree_270) {
                int firstEdgeCornerX   = static_cast<Number&>(firstLhsEdge["rotationCorners"]["upRightNode"]["from"]["x"]).value();
                int firstEdgeCornerY   = static_cast<Number&>(firstLhsEdge["rotationCorners"]["upRightNode"]["from"]["y"]).value();
                int currentEdgeCornerX = static_cast<Number&>(lhsEdge["rotationCorners"]["upRightNode"]["from"]["x"]).value();
                int currentEdgeCornerY = static_cast<Number&>(lhsEdge["rotationCorners"]["upRightNode"]["from"]["y"]).value();

                int vectorX = currentEdgeCornerX - firstEdgeCornerX;
                int vectorY = currentEdgeCornerY - firstEdgeCornerY;

                // rotate vector 270
                int rotatedVectorX = vectorY;
                int rotatedVectorY = -vectorX;

                fromExternalX = &w.pools.numbers.get(rotatedVectorX);
                fromExternalY = &w.pools.numbers.get(rotatedVectorY);
            } else if (&transformation == &arc.LineSymmetry.horizontal) {
                int firstEdgeCornerX = static_cast<Number&>(firstLhsEdge["rotationCorners"]["downLeftNode"]["from"]["x"]).value();
                int firstEdgeCornerY = static_cast<Number&>(firstLhsEdge["rotationCorners"]["downLeftNode"]["from"]["y"]).value();
                int currentEdgeCornerX = static_cast<Number&>(lhsEdge["rotationCorners"]["downLeftNode"]["from"]["x"]).value();
                int currentEdgeCornerY = static_cast<Number&>(lhsEdge["rotationCorners"]["downLeftNode"]["from"]["y"]).value();

                int vectorX = currentEdgeCornerX - firstEdgeCornerX;
                int vectorY = currentEdgeCornerY - firstEdgeCornerY;

                // mirror vector horizontally
                int rotatedVectorX = vectorX;
                int rotatedVectorY = -vectorY;

                fromExternalX = &w.pools.numbers.get(rotatedVectorX);
                fromExternalY = &w.pools.numbers.get(rotatedVectorY);
            } else if (&transformation == &arc.LineSymmetry.vertical) {
                int firstEdgeCornerX   = static_cast<Number&>(firstLhsEdge["mirroringCorners"]["upRightNode"]["from"]["x"]).value();
                int firstEdgeCornerY   = static_cast<Number&>(firstLhsEdge["mirroringCorners"]["upRightNode"]["from"]["y"]).value();
                int currentEdgeCornerX = static_cast<Number&>(lhsEdge["mirroringCorners"]["upRightNode"]["from"]["x"]).value();
                int currentEdgeCornerY = static_cast<Number&>(lhsEdge["mirroringCorners"]["upRightNode"]["from"]["y"]).value();

                int vectorX = currentEdgeCornerX - firstEdgeCornerX;
                int vectorY = currentEdgeCornerY - firstEdgeCornerY;

                // mirror vector vertically
                int rotatedVectorX = -vectorX;
                int rotatedVectorY = vectorY;

                fromExternalX = &w.pools.numbers.get(rotatedVectorX);
                fromExternalY = &w.pools.numbers.get(rotatedVectorY);
            } else {
                throw "error";
            }
        }

        Map& internalEdgeLookup = static_cast<Map&>(rhsShape["internalEdges"]);
        if (internalEdgeLookup.hasKey(*fromExternalY)) {
            Map& colX = static_cast<Map&>(internalEdgeLookup.getValue(*fromExternalY));
            if (colX.hasKey(*fromExternalX)) {
                return &colX.getValue(*fromExternalX);
            } else {
                return nullptr;
            }
        } else {
            return nullptr;
        }
    };

    CellI& outerLhsEdge = lhsEdges["first"]["value"];
    CellI& outerRhsEdge = rhsEdges["first"]["value"];

    EdgeRelation outerResult = compareEdges(outerLhsEdge, outerRhsEdge);
    result.m_edgeRelations.push_back(outerResult);
    if (lhsEdges.size() == 1) {
        return result;
    }

    if (outerResult.isRelated()) {
        CellI* transformationPtr = nullptr;
        if (outerResult.m_rotatedWith) {
            transformationPtr = outerResult.m_rotatedWith;
        } else if (outerResult.m_isHorizontallyMirrored) {
            transformationPtr = &arc.LineSymmetry.horizontal;
        } else if (outerResult.m_isVerticallyMirrored) {
            transformationPtr = &arc.LineSymmetry.vertical;
        }
        CellI& transformation = *transformationPtr;
        for (CellI& lhsEdge : lhsEdges) {
            if (&lhsEdge == &outerLhsEdge) {
                continue;
            }
            CellI* rhsEdgePtr = getEdge(transformation, lhsEdge, outerLhsEdge, rhs);
            if (!rhsEdgePtr) {
                continue;
            }
            CellI& rhsEdge                  = *rhsEdgePtr;
            EdgeRelation internalEdgeResult = compareEdges(lhsEdge, rhsEdge, transformation);
            result.m_edgeRelations.push_back(internalEdgeResult);
        }
    }

    return result;
}

} // namespace arc
} // namespace infocell