#include "Cells.h"
#include "Brain.h"
#include "Arc.h"

namespace infocell {
namespace cells {
namespace hybrid {
namespace arc {

EdgeRelation compareEdges(CellI& lhs, CellI& rhs, CellI& transformation)
{
    brain::Brain& kb = lhs.kb;

    static CellI& Rotate_degree_0   = kb.getVariable("arc::RotationDir::Degree_0");
    static CellI& Rotate_degree_90  = kb.getVariable("arc::RotationDir::Degree_90");
    static CellI& Rotate_degree_180 = kb.getVariable("arc::RotationDir::Degree_180");
    static CellI& Rotate_degree_270 = kb.getVariable("arc::RotationDir::Degree_270");
    static CellI& Mirror_horizontal = kb.getVariable("arc::LineSymmetry::horizontal");
    static CellI& Mirror_vertical   = kb.getVariable("arc::LineSymmetry::vertical");
    static CellI& DirectionUpEV     = kb.getVariable("arc::Directions::up");
    static CellI& DirectionDownEV   = kb.getVariable("arc::Directions::down");
    static CellI& DirectionLeftEV   = kb.getVariable("arc::Directions::left");
    static CellI& DirectionRightEV  = kb.getVariable("arc::Directions::right");
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
        CellI* lhsEdgeNodePtr   = &lhsEdgeNodes[kb.ids.first][kb.ids.value];
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
            lhsEdgeNodePtr = &lhsEdgeNode[kb.ids.next];
            rhsEdgeNodePtr = &rhsEdgeNode[kb.ids.next];
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
        CellI* lhsEdgeNodePtr   = &lhsEdgeNodes[kb.ids.first][kb.ids.value];
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
            lhsEdgeNodePtr = &lhsEdgeNode[kb.ids.next];
            rhsEdgeNodePtr = &rhsEdgeNode[kb.ids.previous];
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
    brain::Brain& kb = lhs.kb;

    static CellI& Rotate_degree_0   = kb.getVariable("arc::RotationDir::Degree_0");
    static CellI& Rotate_degree_90  = kb.getVariable("arc::RotationDir::Degree_90");
    static CellI& Rotate_degree_180 = kb.getVariable("arc::RotationDir::Degree_180");
    static CellI& Rotate_degree_270 = kb.getVariable("arc::RotationDir::Degree_270");
    static CellI& Mirror_horizontal = kb.getVariable("arc::LineSymmetry::horizontal");
    static CellI& Mirror_vertical   = kb.getVariable("arc::LineSymmetry::vertical");

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
    brain::Brain& kb = lhs.kb;

    static CellI& Rotate_degree_0   = kb.getVariable("arc::RotationDir::Degree_0");
    static CellI& Rotate_degree_90  = kb.getVariable("arc::RotationDir::Degree_90");
    static CellI& Rotate_degree_180 = kb.getVariable("arc::RotationDir::Degree_180");
    static CellI& Rotate_degree_270 = kb.getVariable("arc::RotationDir::Degree_270");
    static CellI& Mirror_horizontal = kb.getVariable("arc::LineSymmetry::horizontal");
    static CellI& Mirror_vertical   = kb.getVariable("arc::LineSymmetry::vertical");

    ShapeRelation result;
    List& lhsEdges = static_cast<List&>(lhs["edges"]["list"]);
    List& rhsEdges = static_cast<List&>(rhs["edges"]["list"]);

    if (lhsEdges.empty() || rhsEdges.empty()) {
        return result;
    }

    auto getEdge = [&kb](CellI& transformation, CellI& lhsEdge, CellI& firstLhsEdge, CellI& rhsShape) -> CellI* {
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

                fromExternalX = &kb.pools.numbers.get(rotatedVectorX);
                fromExternalY = &kb.pools.numbers.get(rotatedVectorY);
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

                fromExternalX = &kb.pools.numbers.get(rotatedVectorX);
                fromExternalY = &kb.pools.numbers.get(rotatedVectorY);
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

                fromExternalX = &kb.pools.numbers.get(rotatedVectorX);
                fromExternalY = &kb.pools.numbers.get(rotatedVectorY);
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

                fromExternalX = &kb.pools.numbers.get(rotatedVectorX);
                fromExternalY = &kb.pools.numbers.get(rotatedVectorY);
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

                fromExternalX = &kb.pools.numbers.get(rotatedVectorX);
                fromExternalY = &kb.pools.numbers.get(rotatedVectorY);
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
        Visitor::visitList(lhsEdges, [&kb, &rhs, &getEdge, &result, &outerLhsEdge, &transformation](CellI& lhsEdge, int i, bool& stop) {
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

Shape::Shape(brain::Brain& kb, Number& id, Number& color, Number& width, Number& height, CellI& PixelStruct) :
    CellI(kb),
    m_id(id),
    m_color(color),
    m_width(width),
    m_height(height),
    m_lastEdgeId(&kb.pools.numbers.get(0)),
    m_hybridPixels(kb, kb.std.Pixel),
    m_pixels(kb, PixelStruct)
{
    static CellI& ShapeEdgeStruct = kb.getStruct("arc::ShapeEdge");
    m_edges                       = new Map(kb, kb.std.Number, ShapeEdgeStruct);
}

bool Shape::has(CellI& role)
{
    static CellI& name_lastEdgeId    = kb.name("lastEdgeId");
    static CellI& name_hybridPixels  = kb.name("hybridPixels");
    static CellI& name_pixels        = kb.name("pixels");
    static CellI& name_shapePixels   = kb.name("shapePixels");
    static CellI& name_shapePoints   = kb.name("shapePoints");
    static CellI& name_edges         = kb.name("edges");
    static CellI& name_internalEdges = kb.name("internalEdges");

    if (&role == &kb.ids.struct_) {
        return true;
    }
    if (&role == &kb.ids.id) {
        return true;
    }
    if (&role == &kb.ids.color) {
        return true;
    }
    if (&role == &kb.ids.width) {
        return true;
    }
    if (&role == &kb.ids.height) {
        return true;
    }
    if (&role == &name_lastEdgeId) {
        return true;
    }
    if (&role == &name_hybridPixels) {
        return true;
    }
    if (&role == &name_pixels) {
        return true;
    }
    if (&role == &name_shapePixels && m_shapePixels) {
        return true;
    }
    if (&role == &name_shapePoints && m_shapePoints) {
        return true;
    }
    if (&role == &name_edges && m_edges) {
        return true;
    }
    if (&role == &name_internalEdges && m_internalEdges) {
        return true;
    }

    return false;
}


void Shape::set(CellI& role, CellI& value)
{
    static CellI& name_lastEdgeId    = kb.name("lastEdgeId");
    static CellI& name_shapePixels   = kb.name("shapePixels");
    static CellI& name_shapePoints   = kb.name("shapePoints");
    static CellI& name_edges         = kb.name("edges");
    static CellI& name_internalEdges = kb.name("internalEdges");

    if (&role == &name_lastEdgeId) {
        m_lastEdgeId = &value;
        return;
    }
    if (&role == &name_shapePixels) {
        m_shapePixels = &value;
        return;
    }
    if (&role == &name_shapePoints) {
        m_shapePoints = &value;
        return;
    }
    if (&role == &name_edges) {
        m_edges = &value;
        return;
    }
    if (&role == &name_internalEdges) {
        m_internalEdges = &value;
        return;
    }

    throw "Not implemented!";
}

void Shape::erase(CellI& role)
{
    throw "Changing a hybrid shape cell is not possible!";
}

void Shape::operator()()
{
    // Do nothing
}

CellI& Shape::operator[](CellI& role)
{
    static CellI& ShapeStruct        = kb.getStruct("arc::Shape");
    static CellI& name_lastEdgeId    = kb.name("lastEdgeId");
    static CellI& name_hybridPixels  = kb.name("hybridPixels");
    static CellI& name_pixels        = kb.name("pixels");
    static CellI& name_shapePixels   = kb.name("shapePixels");
    static CellI& name_shapePoints   = kb.name("shapePoints");
    static CellI& name_edges         = kb.name("edges");
    static CellI& name_internalEdges = kb.name("internalEdges");

    if (&role == &kb.ids.struct_) {
        return ShapeStruct;
    }
    if (&role == &kb.ids.id) {
        return m_id;
    }
    if (&role == &kb.ids.color) {
        return m_color;
    }
    if (&role == &kb.ids.width) {
        return m_width;
    }
    if (&role == &kb.ids.height) {
        return m_height;
    }
    if (&role == &name_lastEdgeId) {
        return *m_lastEdgeId;
    }
    if (&role == &name_hybridPixels) {
        return m_hybridPixels;
    }
    if (&role == &name_pixels) {
        return m_pixels;
    }
    if (&role == &name_shapePixels && m_shapePixels) {
        return *m_shapePixels;
    }
    if (&role == &name_shapePoints && m_shapePoints) {
        return *m_shapePoints;
    }
    if (&role == &name_edges && m_edges) {
        return *m_edges;
    }
    if (&role == &name_internalEdges && m_internalEdges) {
        return *m_internalEdges;
    }

    throw "No such role!";
}

void Shape::accept(Visitor& visitor)
{
    //    visitor.visit(*this);
}

void Shape::addPixel(CellI& pixel)
{
    static CellI& PixelStruct = kb.getStruct("arc::Pixel");

    CellI& arcPixel = *new Object(kb, PixelStruct);
    arcPixel.set("x", pixel["x"]);
    arcPixel.set("y", pixel["y"]);
    m_pixels.add(pixel);
    m_hybridPixels.add(pixel);
}

Shaper::Shaper(brain::Brain& kb, cells::hybrid::arc::Grid& grid, CellI& ShapeStruct, CellI& TableRowStruct) :
    CellI(kb),
    m_width(grid.widthCell()),
    m_height(grid.heightCell()),
    m_grid(grid),
    m_shapePixels(kb, kb.std.Number, TableRowStruct),
    m_shapes(kb, ShapeStruct),
    m_shapeMap(kb, kb.std.Number, ShapeStruct),
    m_inputPixels(kb, kb.std.Pixel)
{
    static CellI& ShapeEdgeNodeStruct = kb.getStruct("arc::ShapeEdgeNode");
    m_frameEdgeNodes                  = new List(kb, ShapeEdgeNodeStruct);
    processInputPixels();
}

void Shaper::processInputPixels()
{
    std::vector<cells::hybrid::arc::Pixel>& pixels = m_grid.pixels();
    for (auto& pixel : pixels) {
        m_inputPixels.add(pixel);
    }
}

bool Shaper::has(CellI& role)
{
    static CellI& name_grid        = kb.name("grid");
    static CellI& name_shapePixels = kb.name("shapePixels");
    static CellI& name_shapes      = kb.name("shapes");
    static CellI& name_shapeMap    = kb.name("shapeMap");
    static CellI& name_inputPixels = kb.name("inputPixels");
    static CellI& name_upLeftPoint    = kb.name("upLeftPoint");
    static CellI& name_upRightPoint   = kb.name("upRightPoint");
    static CellI& name_downLeftPoint  = kb.name("downLeftPoint");
    static CellI& name_downRightPoint = kb.name("downRightPoint");

    if (&role == &kb.ids.struct_) {
        return true;
    }
    if (&role == &kb.ids.width) {
        return true;
    }
    if (&role == &kb.ids.height) {
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

CellI& Shaper::operator[](CellI& role)
{
    static CellI& ShaperStruct        = kb.getStruct("arc::Shaper");
    static CellI& name_grid           = kb.name("grid");
    static CellI& name_shapePixels    = kb.name("shapePixels");
    static CellI& name_shapes         = kb.name("shapes");
    static CellI& name_shapeMap       = kb.name("shapeMap");
    static CellI& name_inputPixels    = kb.name("inputPixels");
    static CellI& name_upLeftPoint    = kb.name("upLeftPoint");
    static CellI& name_upRightPoint   = kb.name("upRightPoint");
    static CellI& name_downLeftPoint  = kb.name("downLeftPoint");
    static CellI& name_downRightPoint = kb.name("downRightPoint");

    if (&role == &kb.ids.struct_) {
        return ShaperStruct;
    }
    if (&role == &kb.ids.width) {
        return m_width;
    }
    if (&role == &kb.ids.height) {
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

void Shaper::set(CellI& role, CellI& value)
{
    static CellI& name_upLeftPoint    = kb.name("upLeftPoint");
    static CellI& name_upRightPoint   = kb.name("upRightPoint");
    static CellI& name_downLeftPoint  = kb.name("downLeftPoint");
    static CellI& name_downRightPoint = kb.name("downRightPoint");

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

void Shaper::erase(CellI& role)
{
    throw "Changing a hybrid shape cell is not possible!";
}

void Shaper::operator()()
{
    // Do nothing
}

void Shaper::accept(Visitor& visitor)
{
    //    visitor.visit(*this);
}

void Shaper::process()
{
    static CellI& PixelStruct = kb.getStruct("arc::Pixel");

    const int height = m_height.value();
    const int width  = m_width.value();
    int shapeId      = 1;

    while (!m_inputPixels.empty()) {
        CellI& firstPixel = m_inputPixels.first();
        CellI& shape = *new Shape(kb, kb.pools.numbers.get(shapeId), static_cast<Number&>(firstPixel["color"]), m_width, m_height, PixelStruct);
        shapeId           = shapeId + 1;
        Set checkPixels(kb, kb.std.Pixel);
        checkPixels.add(firstPixel);
        while (!checkPixels.empty()) {
            CellI& checkPixel = checkPixels.first();
            processPixel(shape, checkPixels, checkPixel);
            checkPixels.remove(checkPixel);
        }
    }
    int y = 0;
    while (y < height) {
        Map& colX = static_cast<Map&>(m_shapePixels.getValue(kb.pools.numbers.get(y)));
        int x     = 0;
        while (x < width) {
            CellI& shapePixel = colX.getValue(kb.pools.numbers.get(x));
            Shape& shape      = static_cast<Shape&>(shapePixel["shape"]);
            CellI& pixel      = shapePixel["pixel"];
            shape.addPixel(pixel);
            if (!m_shapeMap.hasKey(shape[kb.ids.id])) {
                m_shapeMap.add(shape[kb.ids.id], shape);
                m_shapes.add(shape);
            }
            x = x + 1;
        }
        y = y + 1;
    }
}

void Shaper::processPixel(CellI& shape, Set& checkPixels, CellI& checkPixel)
{
    static CellI& ShapeStruct      = kb.getStruct("arc::Shape");
    static CellI& ShapePixelStruct = kb.getStruct("arc::ShapePixel");
    static CellI& TableRowStruct   = kb.getStruct(kb.templateId("std::Map", kb.ids.keyType, kb.std.Number, kb.ids.valueType, ShapeStruct));

    if (!m_shapePixels.hasKey(checkPixel["y"])) {
        m_shapePixels.add(checkPixel["y"], *new Map(kb, kb.std.Number, TableRowStruct));
    }
    Map& colX = static_cast<Map&>(m_shapePixels.getValue(checkPixel["y"]));
    CellI& shapePixel = *new Object(kb, ShapePixelStruct);
    shapePixel.set("shape", shape);
    shapePixel.set("pixel", checkPixel);
    colX.add(checkPixel["x"], shapePixel);
    m_inputPixels.remove(checkPixel);

    processAdjacentPixel(kb.directions.up, shape, checkPixels, checkPixel);
    processAdjacentPixel(kb.directions.down, shape, checkPixels, checkPixel);
    processAdjacentPixel(kb.directions.left, shape, checkPixels, checkPixel);
    processAdjacentPixel(kb.directions.right, shape, checkPixels, checkPixel);
}

void Shaper::processAdjacentPixel(CellI& direction, CellI& p_shape, Set& checkPixels, CellI& checkPixel)
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
    if (static_cast<Number&>(pixel["color"]).value() == static_cast<Number&>(p_shape["color"]).value()) {
        checkPixels.add(pixel);
    }
}

} // namespace arc
} // namespace hybrid
} // namespace cells
} // namespace infocell