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

    static CellI& Rotate_degree_0   = w.getVariable("arc::RotationDir::Degree_0");
    static CellI& Rotate_degree_90  = w.getVariable("arc::RotationDir::Degree_90");
    static CellI& Rotate_degree_180 = w.getVariable("arc::RotationDir::Degree_180");
    static CellI& Rotate_degree_270 = w.getVariable("arc::RotationDir::Degree_270");
    static CellI& Mirror_horizontal = w.getVariable("arc::LineSymmetry::horizontal");
    static CellI& Mirror_vertical   = w.getVariable("arc::LineSymmetry::vertical");
    static CellI& DirectionUpEV     = w.getVariable("arc::Directions::up");
    static CellI& DirectionDownEV   = w.getVariable("arc::Directions::down");
    static CellI& DirectionLeftEV   = w.getVariable("arc::Directions::left");
    static CellI& DirectionRightEV  = w.getVariable("arc::Directions::right");
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

    auto rotateDirection = [](CellI& direction, CellI& degree) -> CellI& {
        if (&degree == &Rotate_degree_0) {
            return direction;
        } else if (&degree == &Rotate_degree_90) {
            if (&direction == &DirectionUpEV) {
                return DirectionRightEV;
            } else if (&direction == &DirectionRightEV) {
                return DirectionDownEV;
            } else if (&direction == &DirectionDownEV) {
                return DirectionLeftEV;
            } else if (&direction == &DirectionLeftEV) {
                return DirectionUpEV;
            }
        } else if (&degree == &Rotate_degree_180) {
            if (&direction == &DirectionUpEV) {
                return DirectionDownEV;
            } else if (&direction == &DirectionRightEV) {
                return DirectionLeftEV;
            } else if (&direction == &DirectionDownEV) {
                return DirectionUpEV;
            } else if (&direction == &DirectionLeftEV) {
                return DirectionRightEV;
            }
        } else if (&degree == &Rotate_degree_270) {
            if (&direction == &DirectionUpEV) {
                return DirectionLeftEV;
            } else if (&direction == &DirectionRightEV) {
                return DirectionUpEV;
            } else if (&direction == &DirectionDownEV) {
                return DirectionRightEV;
            } else if (&direction == &DirectionLeftEV) {
                return DirectionDownEV;
            }
        }
        throw "error";
    };

    auto mirrorDirection = [](CellI& direction, CellI& mirroringMode) -> CellI& {
        if (&mirroringMode == &Rotate_degree_0) {
            return direction;
        } else if (&mirroringMode == &Mirror_horizontal) {
            if (&direction == &DirectionUpEV) {
                return DirectionUpEV;
            } else if (&direction == &DirectionRightEV) {
                return DirectionLeftEV;
            } else if (&direction == &DirectionDownEV) {
                return DirectionDownEV;
            } else if (&direction == &DirectionLeftEV) {
                return DirectionRightEV;
            }
        } else if (&mirroringMode == &Mirror_vertical) {
            if (&direction == &DirectionUpEV) {
                return DirectionDownEV;
            } else if (&direction == &DirectionRightEV) {
                return DirectionRightEV;
            } else if (&direction == &DirectionDownEV) {
                return DirectionUpEV;
            } else if (&direction == &DirectionLeftEV) {
                return DirectionLeftEV;
            }
        }
        throw "error";
    };

    if (&transformation == &Rotate_degree_0 || &transformation == &Rotate_degree_90 || &transformation == &Rotate_degree_180 || &transformation == &Rotate_degree_270) {
        CellI& rotationDegree = transformation;
        const char* firstCorner  = "";
        if (&transformation == &Rotate_degree_0) {
            firstCorner = "upLeftNode";
        } else if (&transformation == &Rotate_degree_90) {
            firstCorner = "upRightNode";
        } else if (&transformation == &Rotate_degree_180) {
            firstCorner = "downRightNode";
        } else if (&transformation == &Rotate_degree_270) {
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

    if (&transformation == &Mirror_horizontal || &transformation == &Mirror_vertical) {
        CellI& lineSymmetry     = transformation;
        const char* firstCorner = "";
        if (&transformation == &Mirror_horizontal) {
            firstCorner = "downLeftNode";
        } else if (&transformation == &Mirror_vertical) {
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
            if (&lineSymmetry == &Mirror_horizontal) {
                result.m_isHorizontallyMirrored = true;
            }
            if (&lineSymmetry == &Mirror_vertical) {
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

    static CellI& Rotate_degree_0   = w.getVariable("arc::RotationDir::Degree_0");
    static CellI& Rotate_degree_90  = w.getVariable("arc::RotationDir::Degree_90");
    static CellI& Rotate_degree_180 = w.getVariable("arc::RotationDir::Degree_180");
    static CellI& Rotate_degree_270 = w.getVariable("arc::RotationDir::Degree_270");
    static CellI& Mirror_horizontal = w.getVariable("arc::LineSymmetry::horizontal");
    static CellI& Mirror_vertical   = w.getVariable("arc::LineSymmetry::vertical");

    EdgeRelation result;

    for (CellI* transformation : { &Rotate_degree_0, &Rotate_degree_90, &Rotate_degree_180, &Rotate_degree_270 }) {
        EdgeRelation result = compareEdges(lhs, rhs, *transformation);
        if (result.m_rotatedWith) {
            return result;
        }
    }
    for (CellI* transformation : { &Mirror_horizontal, &Mirror_vertical }) {
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

    static CellI& Rotate_degree_0   = w.getVariable("arc::RotationDir::Degree_0");
    static CellI& Rotate_degree_90  = w.getVariable("arc::RotationDir::Degree_90");
    static CellI& Rotate_degree_180 = w.getVariable("arc::RotationDir::Degree_180");
    static CellI& Rotate_degree_270 = w.getVariable("arc::RotationDir::Degree_270");
    static CellI& Mirror_horizontal = w.getVariable("arc::LineSymmetry::horizontal");
    static CellI& Mirror_vertical   = w.getVariable("arc::LineSymmetry::vertical");

    ShapeRelation result;
    List& lhsEdges = static_cast<List&>(lhs["edges"]["list"]);
    List& rhsEdges = static_cast<List&>(rhs["edges"]["list"]);

    if (lhsEdges.empty() || rhsEdges.empty()) {
        return result;
    }

    auto getEdge = [&w](CellI& transformation, CellI& lhsEdge, CellI& firstLhsEdge, CellI& rhsShape) -> CellI* {
        CellI* fromExternalX = nullptr;
        CellI* fromExternalY = nullptr;

        if (&transformation == &Rotate_degree_0) {
            fromExternalX = &lhsEdge["fromExternalX"];
            fromExternalY = &lhsEdge["fromExternalY"];
        } else {
            if (&transformation == &Rotate_degree_90) {
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
            } else if (&transformation == &Rotate_degree_180) {
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
            } else if (&transformation == &Rotate_degree_270) {
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
            } else if (&transformation == &Mirror_horizontal) {
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
            } else if (&transformation == &Mirror_vertical) {
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
            transformationPtr = &Mirror_horizontal;
        } else if (outerResult.m_isVerticallyMirrored) {
            transformationPtr = &Mirror_vertical;
        }
        CellI& transformation = *transformationPtr;
        Visitor::visitList(lhsEdges, [&w, &rhs, &getEdge, &result, &outerLhsEdge, &transformation](CellI& lhsEdge, int i, bool& stop) {
            if (&lhsEdge == &outerLhsEdge) {
                return;
            }
            CellI* rhsEdgePtr = getEdge(transformation, lhsEdge, outerLhsEdge, rhs);
            if (!rhsEdgePtr) {
                return;
            }
            CellI& rhsEdge                  = *rhsEdgePtr;
            EdgeRelation internalEdgeResult = compareEdges(lhsEdge, rhsEdge, transformation);
            result.m_edgeRelations.push_back(internalEdgeResult);
        });
    }

    return result;
}


} // namespace arc
} // namespace infocell