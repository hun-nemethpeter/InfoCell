#include "arc/hybridcells/Frame.h"
#include "arc/hybridcells/Grid.h"
#include "arc/Grid.h"
#include "arc/Task.h"
#include "EdgeDetector.h"

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include "util/Log.h"

using namespace infocell::cells;

namespace nativearc = infocell::arc::native;
namespace hybridarc = infocell::cells::arc;

namespace infocell {
namespace arc {

EdgeDetector::EdgeDetector(World& w) :
    NodeBase(w),
    FrameStruct(getStruct("arc::Frame")),
    ShapeStruct(getStruct("arc::Shape")),
    ShapePointStruct(getStruct("arc::ShapePoint")),
    ShapePixelStruct(getStruct("arc::ShapePixel")),
    ShapeEdgeStruct(getStruct("arc::ShapeEdge")),
    ShapeEdgeJointStruct(getStruct("arc::ShapeEdgeJoint")),
    ShapeEdgeNodeStruct(getStruct("arc::ShapeEdgeNode")),
    ShapeEdgeRotationCornersStruct(getStruct("arc::ShapeEdgeRotationCorners")),
    ShapeEdgeMirroringCornersStruct(getStruct("arc::ShapeEdgeMirroringCorners")),
    ExternalEdgeEV(getVariable("arc::ShapeEdgeKind::ExternalEdge")),
    InternalEdgeEV(getVariable("arc::ShapeEdgeKind::InternalEdge")),
    DirectionUpEV(getVariable("arc::Directions::up")),
    DirectionDownEV(getVariable("arc::Directions::down")),
    DirectionLeftEV(getVariable("arc::Directions::left")),
    DirectionRightEV(getVariable("arc::Directions::right")),
    Degree_0(getVariable("arc::RotationDir::Degree_0")),
    Degree_90(getVariable("arc::RotationDir::Degree_90")),
    Degree_180(getVariable("arc::RotationDir::Degree_180")),
    Degree_270(getVariable("arc::RotationDir::Degree_270")),
    Symmetry_Horizontal(getVariable("arc::LineSymmetry::horizontal")),
    Symmetry_Vertical(getVariable("arc::LineSymmetry::vertical"))
{
    auto& TableRowStruct = getStruct(w.templateId("std::Map", id.keyType, w.std.Number, id.valueType, ShapeStruct));
    auto& TableStruct    = getStruct(w.templateId("std::Map", id.keyType, w.std.Number, id.valueType, TableRowStruct));
}

void EdgeDetector::detect(const std::string& jsonStr)
{
    m_inputGrid       = std::make_unique<nativearc::Grid>("inputGrid", jsonStr);
    m_inputHybridGrid = std::make_unique<hybridarc::Grid>(w, *m_inputGrid);
    detect();
}

void EdgeDetector::detect(hybridarc::Grid& inputHybridGrid)
{
    m_inputHybridGridPtr = &inputHybridGrid;
    detect();
}

void EdgeDetector::detect()
{
    frameProcess();
    sortShapePixelsAndCreateShapePoints();
    sortShapePoints();
    calculateEdgesForShapes();
    processEdgeNodes();
    findRotationCornersUpLeft();
    findRotationCornersUpRight();
    findRotationCornersDownLeft();
    findRotationCornersDownRight();
    findMirroringCornersUpRight();
    findMirroringCornersDownLeft();
}

void EdgeDetector::frameProcess()
{
#if 1
    auto& TableRowStruct = getStruct(w.templateId("std::Map", id.keyType, w.std.Number, id.valueType, ShapeStruct));
    m_hybridFrame        = std::make_unique<hybridarc::Frame>(w, inputHybridGrid(), ShapeStruct, TableRowStruct);
    m_hybridFrame->process();
    return;
#endif
    m_frame = std::make_unique<Object>(w, FrameStruct, w.name("constructor"), Param { "grid", inputHybridGrid() });
    m_frame->method("process");
}

void EdgeDetector::addEdgeToShape(CellI& shape, CellI& newEdgeId, CellI& newEdge)
{
    TRACE(edge, "Shape id: {} add edge {}", shape["id"].label(), newEdgeId.label());
    if (m_hybridFrame) {
        Map& edges = static_cast<Map&>(shape["edges"]);
        edges.add(newEdgeId, newEdge);
    } else {
        Object& map = static_cast<Object&>(shape["edges"]);
        map.method(w.name("add"), { id.key, newEdgeId }, { id.value, newEdge });
    }
}

int EdgeDetector::getShapeEdgesSize(CellI& shape)
{
    if (m_hybridFrame) {
        return static_cast<Map&>(shape["edges"]).size();
    } else {
        Object& map = static_cast<Object&>(shape["edges"]);
        return static_cast<Number&>(map.method(w.name("size"))).value();
    }
}

CellI& EdgeDetector::frame()
{
    if (m_hybridFrame) {
        return *m_hybridFrame;
    }
    return *m_frame;
}

hybridarc::Grid& EdgeDetector::inputHybridGrid()
{
    if (m_inputHybridGridPtr) {
        return *m_inputHybridGridPtr;
    } else if (m_inputHybridGrid) {
        return *m_inputHybridGrid;
    }
    throw "No inputHybridGrid!";
}

void EdgeDetector::sortShapePixelsAndCreateShapePoints()
{
    // so after frame::Process we have shapes and we have Frame::shapePixels where every x, y coordinates a shepe is registered
    // here we go from 0..grid.height() for x, and from 0..grid.width() for y and connect ever shape pixel in directions
    // up, down, left and right, and also creating shape Point in every pixel corner. Every shape point can point to adjacent shape pixels in derections
    // downRight, downLeft, upRight, upLeft
    //      Pixel   Pixel
    //    upLeft \ / upRight
    //            .
    //  downLeft / \ downRight
    //      Pixel   Pixel
    // also, during the process, we adding the individual pixels to the shape object own shapePixel list, so it will have an
    // ordered list of pixels
    Object& shapePixels    = static_cast<Object&>(frame()["shapePixels"]);
    CellI* previousUpPixel = nullptr;
    CellI* upPixel         = nullptr;
    CellI* leftPixel       = nullptr;
    CellI* firstShapePixel = nullptr;
    for (int y = 0; y < inputHybridGrid().height(); ++y) {
        Object& colX = static_cast<Object&>(shapePixels.method(w.name("getValue"), { w.id.key, toCellNumber(y) }));
        for (int x = 0; x < inputHybridGrid().width(); ++x) {
            CellI& shapePixel = colX.method(w.name("getValue"), { w.id.key, toCellNumber(x) });
            if (!firstShapePixel) {
                firstShapePixel = &shapePixel;
            }
            if (leftPixel) {
                (*leftPixel).set("right", shapePixel);
                shapePixel.set("left", *leftPixel);
            }
            if (upPixel) {
                (*upPixel).set("down", shapePixel);
                shapePixel.set("up", *upPixel);
            }

            CellI& currentShape      = shapePixel["shape"];
            CellI& pixel             = shapePixel["pixel"];
            CellI* upLeftPointPtr    = nullptr;
            CellI* downLeftPointPtr  = nullptr;
            CellI* upRightPointPtr   = nullptr;
            CellI* downRightPointPtr = new Object(w, ShapePointStruct);

            bool upLeftPointCreated    = false;
            bool upRightPointCreated   = false;
            bool downLeftPointCreated  = false;
            bool downRightPointCreated = true;

            if (leftPixel && upPixel) {
                upLeftPointPtr   = &(*leftPixel)["upRightPoint"];
                upRightPointPtr  = &(*upPixel)["downRightPoint"];
                downLeftPointPtr = &(*leftPixel)["downRightPoint"];
            } else if (leftPixel && !upPixel) {
                upLeftPointPtr   = &(*leftPixel)["upRightPoint"];
                upRightPointPtr  = new Object(w, ShapePointStruct);
                downLeftPointPtr = &(*leftPixel)["downRightPoint"];

                upRightPointCreated = true;

                (*upLeftPointPtr).set("right", *upRightPointPtr);
                (*upRightPointPtr).set("left", *upLeftPointPtr);

                (*upRightPointPtr).set("x", w.pools.numbers.get(static_cast<Number&>(pixel["x"]).value() + 1));
                (*upRightPointPtr).set("y", pixel["y"]);
            } else if (!leftPixel && upPixel) {
                upLeftPointPtr   = &(*upPixel)["downLeftPoint"];
                upRightPointPtr  = &(*upPixel)["downRightPoint"];
                downLeftPointPtr = new Object(w, ShapePointStruct);

                downLeftPointCreated = true;

                (*upLeftPointPtr).set("down", *downLeftPointPtr);
                (*downLeftPointPtr).set("up", *upLeftPointPtr);

                (*downLeftPointPtr).set("x", pixel["x"]);
                (*downLeftPointPtr).set("y", w.pools.numbers.get(static_cast<Number&>(pixel["y"]).value() + 1));
            } else if (!leftPixel && !upPixel) {
                upLeftPointPtr   = new Object(w, ShapePointStruct);
                upRightPointPtr  = new Object(w, ShapePointStruct);
                downLeftPointPtr = new Object(w, ShapePointStruct);

                upLeftPointCreated   = true;
                upRightPointCreated  = true;
                downLeftPointCreated = true;

                (*upLeftPointPtr).set("right", *upRightPointPtr);
                (*upRightPointPtr).set("left", *upLeftPointPtr);

                (*upLeftPointPtr).set("down", *downLeftPointPtr);
                (*downLeftPointPtr).set("up", *upLeftPointPtr);

                (*upLeftPointPtr).set("x", pixel["x"]);
                (*upLeftPointPtr).set("y", pixel["y"]);

                (*upRightPointPtr).set("x", w.pools.numbers.get(static_cast<Number&>(pixel["x"]).value() + 1));
                (*upRightPointPtr).set("y", pixel["y"]);

                (*downLeftPointPtr).set("x", pixel["x"]);
                (*downLeftPointPtr).set("y", w.pools.numbers.get(static_cast<Number&>(pixel["y"]).value() + 1));
            }
            (*upRightPointPtr).set("down", *downRightPointPtr);
            (*downRightPointPtr).set("up", *upRightPointPtr);

            (*downLeftPointPtr).set("right", *downRightPointPtr);
            (*downRightPointPtr).set("left", *downLeftPointPtr);

            (*downRightPointPtr).set("x", w.pools.numbers.get(static_cast<Number&>(pixel["x"]).value() + 1));
            (*downRightPointPtr).set("y", w.pools.numbers.get(static_cast<Number&>(pixel["y"]).value() + 1));

            (*upLeftPointPtr).set("downRightPixel", shapePixel);
            (*upRightPointPtr).set("downLeftPixel", shapePixel);
            (*downLeftPointPtr).set("upRightPixel", shapePixel);
            (*downRightPointPtr).set("upLeftPixel", shapePixel);

            shapePixel.set("upLeftPoint", *upLeftPointPtr);
            shapePixel.set("downLeftPoint", *downLeftPointPtr);
            shapePixel.set("upRightPoint", *upRightPointPtr);
            shapePixel.set("downRightPoint", *downRightPointPtr);

            if (currentShape.missing("shapePixels")) {
                currentShape.set("shapePixels", *new List(w, ShapePixelStruct));
            }
            List& shapePixelList = static_cast<List&>(currentShape["shapePixels"]);
            shapePixelList.add(shapePixel);

            // set corners
            if (x == 0 && y == 0) {
                frame().set("upLeftPoint", shapePixel["upLeftPoint"]);
            }
            if (x == inputHybridGrid().width() - 1 && y == 0) {
                frame().set("upRightPoint", shapePixel["upRightPoint"]);
            }
            if (x == 0 && y == inputHybridGrid().height() - 1) {
                frame().set("downLeftPoint", shapePixel["downLeftPoint"]);
            }
            if (x == inputHybridGrid().width() - 1 && y == inputHybridGrid().height() - 1) {
                frame().set("downRightPoint", shapePixel["downRightPoint"]);
            }

            // stepping
            if (pixel.missing("left")) {
                // first column
                previousUpPixel = &shapePixel;
            }
            if (pixel.has("right")) {
                leftPixel = &shapePixel;
                if (upPixel) {
                    upPixel = &(*upPixel)["right"];
                }
            } else {
                // last column
                leftPixel = nullptr;
                if (previousUpPixel) {
                    upPixel = previousUpPixel;
                }
            }
        }
    }
}

void EdgeDetector::sortShapePoints()
{
    Visitor::visitList(frame()["shapes"], [this](CellI& currentShape, int, bool&) {
        TRACE(edge, "Shape id: {}, points:", currentShape["id"].label());

        ScanLineState scanLineState = ScanLineState::Up;

        List& shapePoints = *new List(w, ShapePointStruct);
        currentShape.set("shapePoints", shapePoints);

        CellI* currentPixelItemPtr      = &currentShape["shapePixels"][w.id.first];
        CellI* currentMiddleRowListItem = nullptr;
        CellI* upMiddleRowListItem      = nullptr;
        CellI* downMiddleRowListItem    = nullptr;
        CellI* firstColumnPixelItem     = currentPixelItemPtr;
        CellI& firstPixel               = (*currentPixelItemPtr)[w.id.value];

        bool isUpperLine          = false;
        bool hasMoreUp            = false;
        bool hasMoreDown          = false;
        int upMiddleColumnIndex   = -1;
        int downMiddleColumnIndex = -1;
        int pointX                = -1;
        int pointY                = static_cast<Number&>(firstPixel["pixel"]["y"]).value();

        std::stringstream ss;
        ss << "  ";
        while (currentPixelItemPtr) {
            switch (scanLineState) {
            case ScanLineState::Up: {
                CellI& currentPixelItem = *currentPixelItemPtr;
                CellI& currentPixel     = currentPixelItem[w.id.value];
                CellI& currentPoint     = currentPixel["upLeftPoint"];
                int upLeftPointX        = static_cast<Number&>(currentPoint["x"]).value();

                if (upLeftPointX > pointX) {
                    // two new point added
                    // |  |
                    // v  v
                    // ∙──►
                    // │██│
                    // └──┘
                    // ^^^ first pixel in a row
                    //
                    //    two new point added
                    //    |  |
                    //    v  v
                    // ┌──∙──►
                    // │  │██│
                    // └──┴──┘
                    ss << fmt::format("({},{}) ", upLeftPointX, pointY);
                    shapePoints.add(currentPoint);
                } else {
                    //    just one new point added
                    //       |
                    //       v
                    // ┌──∙──►
                    // │██│██│
                    // └──┴──┘
                    // the up left point was created in the previous iteration so we do nothing here
                }
                pointX = upLeftPointX + 1;
                ss << fmt::format("({},{}) ", pointX, pointY);
                shapePoints.add(currentPoint["right"]);
                CellI* nextPixelItem            = currentPixelItem.has(w.id.next) ? &currentPixelItem[w.id.next] : nullptr;
                bool isNextPixelIsInTheSameLine = nextPixelItem ? &(*firstColumnPixelItem)["value"]["pixel"]["y"] == &(*nextPixelItem)["value"]["pixel"]["y"] : false;

                if (nextPixelItem) {
                    if (isNextPixelIsInTheSameLine) {
                        // same line
                        currentPixelItemPtr = nextPixelItem;
                    } else {
                        // new line
                        upMiddleRowListItem   = firstColumnPixelItem;
                        downMiddleRowListItem = nextPixelItem;
                        firstColumnPixelItem  = downMiddleRowListItem;
                        upMiddleColumnIndex   = static_cast<Number&>((*upMiddleRowListItem)["value"]["pixel"]["x"]).value();
                        downMiddleColumnIndex = static_cast<Number&>((*downMiddleRowListItem)["value"]["pixel"]["x"]).value();
                        isUpperLine           = upMiddleColumnIndex <= downMiddleColumnIndex;
                        currentPixelItemPtr   = isUpperLine ? upMiddleRowListItem : downMiddleRowListItem;
                        hasMoreUp             = true;
                        hasMoreDown           = true;
                        pointX                = -1;
                        ++pointY;
                        scanLineState = ScanLineState::Middle;
                        TRACE(edge, "{} Up -> Middle", ss.str());
                        ss.str("");
                        ss << "  ";
                    }
                } else {
                    // no more pixel, but still the bottom points row must be created
                    currentPixelItemPtr = firstColumnPixelItem;
                    pointX              = -1;
                    ++pointY;
                    scanLineState = ScanLineState::Down;
                    TRACE(edge, "{} Up -> Down", ss.str());
                    ss.str("");
                    ss << "  ";
                }
            } break;
            case ScanLineState::Middle: {
                // we have at least two rows of pixels
                CellI& currentPixelItem = *currentPixelItemPtr;
                CellI& currentPixel     = currentPixelItem[w.id.value];
                CellI& currentPoint     = currentPixel[isUpperLine ? "downLeftPoint" : "upLeftPoint"];
                int currentPointX       = static_cast<Number&>(currentPoint["x"]).value();
                int currentPointY       = static_cast<Number&>(currentPoint["y"]).value();
                if (currentPointX > pointX) {
                    ss << fmt::format("({},{}) ", currentPointX, pointY);
                    shapePoints.add(currentPoint);
                }
                pointX = currentPointX + 1;
                ss << fmt::format("({},{}) ", pointX, pointY);
                shapePoints.add(currentPoint["right"]);

                // stepping
                CellI* nextUpListItem   = nullptr;
                CellI* nextDownListItem = nullptr;
                if (hasMoreUp && hasMoreDown) {
                    if (upMiddleColumnIndex < downMiddleColumnIndex) {
                        // ┌──┬──┐
                        // │██│xx│ <- up-line
                        // ∙──►──┤
                        // │  │xx│ <- down-line
                        // └──┴──┘
                        // step up-line iter only
                        nextUpListItem   = &(*upMiddleRowListItem)[w.id.next];
                        nextDownListItem = downMiddleRowListItem;
                    } else if (upMiddleColumnIndex == downMiddleColumnIndex) {
                        // ┌──┬──┐
                        // │██│xx│ <- up line
                        // ∙──►──┤
                        // │██│xx│ <- down line
                        // └──┴──┘
                        // step up- and down-line iters
                        nextUpListItem   = &(*upMiddleRowListItem)[w.id.next];
                        nextDownListItem = (*downMiddleRowListItem).has(w.id.next) ? &(*downMiddleRowListItem)[w.id.next] : nullptr;
                    } else {
                        // ┌──┬──┐
                        // │  │xx│ <- up-line
                        // ∙──►──┤
                        // │██│xx│ <- down-line
                        // └──┴──┘
                        // step down-line iter only
                        nextUpListItem   = upMiddleRowListItem;
                        nextDownListItem = (*downMiddleRowListItem).has(w.id.next) ? &(*downMiddleRowListItem)[w.id.next] : nullptr;
                    }
                } else if (hasMoreUp && !hasMoreDown) {
                    // ┌──┬──┐
                    // │xx│██│ <- up-line
                    // ∙──►──┤
                    // │xx│  │ <- down-line, no more pixel in this line
                    // └──┴──┘
                    // step up line iter only
                    nextUpListItem   = &(*upMiddleRowListItem)[w.id.next];
                    nextDownListItem = downMiddleRowListItem;
                } else if (!hasMoreUp && hasMoreDown) {
                    // ┌──┐
                    // │xx│ <- up-line, no more pixel in this line
                    // ∙──►
                    // │xx│ <- down-line
                    // └──┘
                    // step down line iter only
                    nextUpListItem   = upMiddleRowListItem;
                    nextDownListItem = (*downMiddleRowListItem).has(w.id.next) ? &(*downMiddleRowListItem)[w.id.next] : nullptr;
                }

                hasMoreUp       = (nextUpListItem != firstColumnPixelItem);
                hasMoreDown     = nextDownListItem ? static_cast<Number&>((*nextDownListItem)["value"]["pixel"]["y"]).value() == pointY : false;
                bool isLastLine = !nextDownListItem;

                if (isLastLine && !hasMoreUp && !hasMoreDown) {
                    upMiddleRowListItem   = nullptr;
                    downMiddleRowListItem = nullptr;
                    currentPixelItemPtr   = firstColumnPixelItem;
                    pointX                = -1;
                    ++pointY;
                    scanLineState = ScanLineState::Down;
                    isUpperLine   = false;
                    TRACE(edge, "{} Middle -> Down", ss.str());
                    ss.str("");
                    ss << "  ";
                } else if (!hasMoreUp && !hasMoreDown) {
                    pointX = -1;
                    ++pointY;
                    upMiddleRowListItem   = nextUpListItem;
                    downMiddleRowListItem = nextDownListItem;
                    firstColumnPixelItem  = nextDownListItem;
                    upMiddleColumnIndex   = static_cast<Number&>((*upMiddleRowListItem)["value"]["pixel"]["x"]).value();
                    downMiddleColumnIndex = static_cast<Number&>((*downMiddleRowListItem)["value"]["pixel"]["x"]).value();
                    isUpperLine           = upMiddleColumnIndex <= downMiddleColumnIndex;
                    currentPixelItemPtr   = isUpperLine ? upMiddleRowListItem : downMiddleRowListItem;
                    hasMoreUp             = true;
                    hasMoreDown           = true;
                    TRACE(edge, "{} Middle -> Middle", ss.str());
                    ss.str("");
                    ss << "  ";
                } else {
                    upMiddleRowListItem   = nextUpListItem;
                    downMiddleRowListItem = nextDownListItem;
                    if (hasMoreUp) {
                        upMiddleColumnIndex = static_cast<Number&>((*upMiddleRowListItem)["value"]["pixel"]["x"]).value();
                    }
                    if (hasMoreDown) {
                        downMiddleColumnIndex = static_cast<Number&>((*downMiddleRowListItem)["value"]["pixel"]["x"]).value();
                    }
                    if (hasMoreUp && hasMoreDown) {
                        isUpperLine         = upMiddleColumnIndex <= downMiddleColumnIndex;
                        currentPixelItemPtr = isUpperLine ? upMiddleRowListItem : downMiddleRowListItem;
                    } else if (!hasMoreUp) {
                        currentPixelItemPtr = downMiddleRowListItem;
                        isUpperLine         = false;
                    } else {
                        currentPixelItemPtr = upMiddleRowListItem;
                        isUpperLine         = true;
                    }
                }
            } break;
            case ScanLineState::Down: {
                CellI& currentPixelItem = *currentPixelItemPtr;
                CellI& currentPixel     = currentPixelItem[w.id.value];
                CellI& currentPoint     = currentPixel["downLeftPoint"];
                int downLeftPointX      = static_cast<Number&>(currentPoint["x"]).value();

                if (downLeftPointX > pointX) {
                    ss << fmt::format("({},{}) ", downLeftPointX, pointY);
                    shapePoints.add(currentPoint);
                }
                pointX = downLeftPointX + 1;
                ss << fmt::format("({},{}) ", pointX, pointY);
                shapePoints.add(currentPoint["right"]);

                currentPixelItemPtr = currentPixelItem.has(w.id.next) ? &currentPixelItem[w.id.next] : nullptr;
            } break;
            }
        }
        TRACE(edge, ss.str());
    });
}

void EdgeDetector::calculateEdgesForShapes()
{
    DEBUG(edge, "calculateEdgesForShapes");
    Visitor::visitList(frame()["shapes"], [this](CellI& currentShape, int, bool&) {
        TRACE(edge, "  Shape id: {}", currentShape["id"].label());

        enum class ProcessingMode
        {
            ExternalEdgeStart,
            ExternalEdgeDetect,
            ExternalEdgeStep,
            InternalEdgeStart,
            InternalEdgeDetect,
            InternalEdgeStep,
            Searching
        };

        ProcessingMode processingMode = ProcessingMode::ExternalEdgeStart;
        CellI* processingDirectionPtr = &DirectionRightEV;
        CellI* currentListItemPtr     = &currentShape["shapePoints"][w.id.first];
        CellI* previousListItemPtr    = nullptr;
        CellI* firstColumnPointItem   = currentListItemPtr;

        CellI* newEdgePtr  = nullptr;
        List* edgeNodesPtr = nullptr;

        const char* toDirectionStr             = "";
        const char* toEdgeDirectionStr         = "";
        const char* toOppositeEdgeDirectionStr = "";

        CellI* currentShapePointPtr = nullptr;
        CellI* firstShapePointPtr   = nullptr;
        CellI* previousEdgeNodePtr  = nullptr;
        int startPointX             = -1;
        int startPointY             = -1;

        while (currentListItemPtr || currentShapePointPtr) {
            CellI& currentListItem = *currentListItemPtr;
            CellI& shapePoint      = currentListItemPtr ? (*currentListItemPtr)[w.id.value] : *currentShapePointPtr;
            int pointX             = static_cast<Number&>(shapePoint["x"]).value();
            int pointY             = static_cast<Number&>(shapePoint["y"]).value();
#if 0
  0 1 2 3 4 5 6 7 8
0 .................
1 .................     |
2 ....XX...........   --.--.
3 .................     |XX|
4 .................     .--.
5 .................
6 .................
7 .................

For leftToRight direction edge from point middle
 1 0000 Invalid state, can not happen
 2 1000 Skip
 3 0100 Skip
 4 1100 Skip
 5 0010 Skip
 6 1010 Skip
 7 0110 Special
 8 1110 Start internal edge
 9 0001 Start external edge
10 1001 Skip
11 0101 Skip
12 1101 Skip
13 0011 Skip
14 1011 Skip
15 0111 Skip
16 1111 Skip

 Invalid  Skip     Skip     Skip     Skip     Skip     Special  New int  New ext Skip      Skip     Skip     Skip     Skip     Skip     Skip
 1        2        3        4        5        6        7        8        9        10       11       12       13       14       15       16
 0🡬 0🡭   1🡬 0🡭   0🡬 1🡭   1🡬 1🡭  0🡬 0🡭   1🡬 0🡭   0🡬 1🡭   1🡬 1🡭   0🡬 0🡭  1🡬 0🡭   0🡬 1🡭   1🡬 1🡭   0🡬 0🡭   1🡬 0🡭  0🡬 1🡭   1🡬 1🡭
 0🡯 0🡮   0🡯 0🡮   0🡯 0🡮   0🡯 0🡮  1🡯 0🡮   1🡯 0🡮   1🡯 0🡮   1🡯 0🡮   0🡯 1🡮  0🡯 1🡮   0🡯 1🡮   0🡯 1🡮   1🡯 1🡮   1🡯 1🡮  1🡯 1🡮   1🡯 1🡮
 ┌──┬──┐  ┌──┬──┐  ┌──┬──┐  ┌──┬──┐  ┌──┬──┐  ┌──┬──┐  ┌──┬──┐  ┌──┬──┐  ┌──┬──┐  ┌──┬──┐  ┌──┬──┐  ┌──┬──┐  ┌──┬──┐  ┌──┬──┐  ┌──┬──┐  ┌──┬──┐
 │  │  │  │██│  │  │  │██│  │██│██│  │  │  │  │██│  │  │  │██│  │██│██│  │  │  │  │██│  │  │  │██│  │██│██│  │  │  │  │██│  │  │  │██│  │██│██│
 ├──∙──►  ├──∙──►  ├──∙──►  ├──∙──►  ├──∙──►  ├──∙──►  ├──∙──►  ├──∙──►  ├──∙──►  ├──∙──►  ├──∙──►  ├──∙──►  ├──∙──►  ├──∙──►  ├──∙──►  ├──∙──►
 │  │  │  │  │  │  │  │  │  │  │  │  │██│  │  │██│  │  │██│  │  │██│  │  │  │██│  │  │██│  │  │██│  │  │██│  │██│██│  │██│██│  │██│██│  │██│██│
 └──┴──┘  └──┴──┘  └──┴──┘  └──┴──┘  └──┴──┘  └──┴──┘  └──┴──┘  └──┴──┘  └──┴──┘  └──┴──┘  └──┴──┘  └──┴──┘  └──┴──┘  └──┴──┘  └──┴──┘  └──┴──┘
#endif

            bool hasUpLeft    = shapePoint.has("upLeftPixel") && (&shapePoint["upLeftPixel"]["shape"] == &currentShape);       // 🡬
            bool hasUpRight   = shapePoint.has("upRightPixel") && (&shapePoint["upRightPixel"]["shape"] == &currentShape);     // 🡭
            bool hasDownLeft  = shapePoint.has("downLeftPixel") && (&shapePoint["downLeftPixel"]["shape"] == &currentShape);   // 🡯
            bool hasDownRight = shapePoint.has("downRightPixel") && (&shapePoint["downRightPixel"]["shape"] == &currentShape); // 🡮

            int caseNum = 1 + (int)hasUpLeft + ((int)hasUpRight * 2) + ((int)hasDownLeft * 4) + ((int)hasDownRight * 8);
            if (caseNum == 1) {
                ERROR(edge, "Invalid pixel state");
            }

            switch (processingMode) {
            case ProcessingMode::ExternalEdgeStart:
            case ProcessingMode::InternalEdgeStart: {
                // create new edge
                CellI& newEdge   = *new Object(w, ShapeEdgeStruct);
                List& edgeNodes  = *new List(w, ShapeEdgeNodeStruct);
                CellI& newEdgeId = w.pools.numbers.get(static_cast<Number&>(currentShape["lastEdgeId"]).value() + 1);
                currentShape.set("lastEdgeId", newEdgeId);
                newEdge.set("shape", currentShape);
                newEdge.set("edgeNodes", edgeNodes);
                newEdge.set("id", newEdgeId);
                newEdge.set("rotationCorners", *new Object(w, ShapeEdgeRotationCornersStruct));
                newEdge.set("mirroringCorners", *new Object(w, ShapeEdgeMirroringCornersStruct));

                addEdgeToShape(currentShape, newEdge["id"], newEdge);

                previousListItemPtr  = currentListItemPtr;
                currentListItemPtr   = nullptr;
                currentShapePointPtr = &shapePoint;
                firstShapePointPtr   = currentShapePointPtr;
                previousEdgeNodePtr  = nullptr;
                newEdgePtr           = &newEdge;
                edgeNodesPtr         = &edgeNodes;

                if (processingMode == ProcessingMode::ExternalEdgeStart) {
                    newEdge.set("kind", ExternalEdgeEV);
                    processingMode = ProcessingMode::ExternalEdgeDetect;
                    startPointX    = pointX;
                    startPointY    = pointY;
                } else {
                    newEdge.set("kind", InternalEdgeEV);
                    processingMode   = ProcessingMode::InternalEdgeDetect;
                    CellI& distanceX = w.pools.numbers.get(pointX - startPointX);
                    CellI& distanceY = w.pools.numbers.get(pointY - startPointY);
                    newEdge.set("fromExternalX", distanceX);
                    newEdge.set("fromExternalY", distanceY);
                    CellI* internalEdgesPtr = nullptr;
                    if (currentShape.missing("internalEdges")) {
                        static CellI& InternalEdgeLookup = w.getStruct("arc::Shape::InternalEdgeLookup");

                        internalEdgesPtr = new Map(w, w.std.Number, InternalEdgeLookup);
                        currentShape.set("internalEdges", *internalEdgesPtr);
                    } else {
                        internalEdgesPtr = &currentShape["internalEdges"];
                    }
                    Map& internalEdges = static_cast<Map&>(*internalEdgesPtr);
                    CellI* colXPtr     = nullptr;
                    if (!internalEdges.hasKey(distanceY)) {
                        static CellI& InternalEdgeLookupRow = w.getStruct(w.templateId("std::Map", w.id.keyType, w.std.Number, w.id.valueType, ShapeEdgeStruct));

                        colXPtr = new Map(w, w.std.Number, InternalEdgeLookupRow);
                        internalEdges.add(distanceY, *colXPtr);
                    } else {
                        colXPtr = &internalEdges.getValue(distanceY);
                    }
                    Map& colX = static_cast<Map&>(*colXPtr);
                    colX.add(distanceX, newEdge);
                }
            } break;

            case ProcessingMode::ExternalEdgeDetect: {
                if (processingDirectionPtr == &DirectionRightEV) {
                    if (hasUpLeft && !hasDownLeft) {
                        // ┌──┬──┐
                        // │██│xx│
                        // ∙──►──┤
                        // │  │xx│
                        // └──┴──┘
                        ERROR(edge, "External edge detection error!");
                    }
                    if (hasUpRight && hasDownRight) {
                        // ┌──▲──┐
                        // │  │██│
                        // ∙──►──┤
                        // │██│██│
                        // └──┴──┘
                        processingDirectionPtr = &DirectionUpEV;
                    } else if (hasUpRight && !hasDownRight) {
                        // ┌──▲──┐
                        // │  │██│
                        // ∙──►──┤
                        // │██│  │
                        // └──┴──┘
                        processingDirectionPtr = &DirectionUpEV;
                    } else if (!hasUpRight && hasDownRight) {
                        // ┌──┬──┐
                        // │  │  │
                        // ∙──►──►
                        // │██│██│
                        // └──┴──┘
                        processingDirectionPtr = &DirectionRightEV;
                    } else if (!hasUpRight && !hasDownRight) {
                        // ┌──┬──┐
                        // │  │  │
                        // ∙──►──┤
                        // │██│  │
                        // └──▼──┘
                        processingDirectionPtr = &DirectionDownEV;
                    }
                } else if (processingDirectionPtr == &DirectionLeftEV) {
                    if (!hasUpRight && hasDownRight) {
                        // ┌──┬──┐
                        // │xx│  │
                        // ├──◄──∙
                        // │xx│██│
                        // └──┴──┘
                        ERROR(edge, "External edge detection error!");
                    }
                    if (hasUpLeft && hasDownLeft) {
                        // ┌──┬──┐
                        // │██│██│
                        // ├──◄──∙
                        // │██│  │
                        // └──▼──┘
                        processingDirectionPtr = &DirectionDownEV;
                    } else if (hasUpLeft && !hasDownLeft) {
                        // ┌──┬──┐
                        // │██│██│
                        // ◄──◄──∙
                        // │  │  │
                        // └──┴──┘
                        processingDirectionPtr = &DirectionLeftEV;
                    } else if (!hasUpLeft && hasDownLeft) {
                        // ┌──┬──┐
                        // │  │██│
                        // ├──◄──∙
                        // │██│  │
                        // └──▼──┘
                        processingDirectionPtr = &DirectionDownEV;
                    } else if (!hasUpLeft && !hasDownLeft) {
                        // ┌──▲──┐
                        // │  │██│
                        // ├──◄──∙
                        // │  │  │
                        // └──┴──┘
                        processingDirectionPtr = &DirectionUpEV;
                    }
                } else if (processingDirectionPtr == &DirectionUpEV) {
                    if (hasDownLeft && !hasDownRight) {
                        // ┌──┬──┐
                        // │xx│xx│
                        // ├──▲──┤
                        // │██│  │
                        // └──∙──┘
                        ERROR(edge, "External edge detection error!");
                    }
                    if (hasUpLeft && hasUpRight) {
                        // ┌──┬──┐
                        // │██│██│
                        // ◄──▲──┤
                        // │  │██│
                        // └──∙──┘
                        processingDirectionPtr = &DirectionLeftEV;
                    } else if (hasUpLeft && !hasUpRight) {
                        // ┌──┬──┐
                        // │██│  │
                        // ◄──▲──┤
                        // │  │██│
                        // └──∙──┘
                        processingDirectionPtr = &DirectionLeftEV;
                    } else if (!hasUpLeft && hasUpRight) {
                        // ┌──▲──┐
                        // │  │██│
                        // ├──▲──┤
                        // │  │██│
                        // └──∙──┘
                        processingDirectionPtr = &DirectionUpEV;
                    } else if (!hasUpLeft && !hasUpRight) {
                        // ┌──┬──┐
                        // │  │  │
                        // ├──▲──►
                        // │  │██│
                        // └──∙──┘
                        processingDirectionPtr = &DirectionRightEV;
                    }
                } else if (processingDirectionPtr == &DirectionDownEV) {
                    if (!hasUpLeft && hasUpRight) {
                        // ┌──∙──┐
                        // │  │██│
                        // ├──▼──┤
                        // │xx│xx│
                        // └──┴──┘
                        ERROR(edge, "External edge detection error!");
                    }
                    if (hasDownLeft && hasDownRight) {
                        // ┌──∙──┐
                        // │██│  │
                        // ├──▼──►
                        // │██│██│
                        // └──┴──┘
                        processingDirectionPtr = &DirectionRightEV;
                    } else if (hasDownLeft && !hasDownRight) {
                        // ┌──∙──┐
                        // │██│  │
                        // ├──▼──┤
                        // │██│  │
                        // └──▼──┘
                        processingDirectionPtr = &DirectionDownEV;
                    } else if (!hasDownLeft && hasDownRight) {
                        // ┌──∙──┐
                        // │██│  │
                        // ├──▼──►
                        // │  │██│
                        // └──┴──┘
                        processingDirectionPtr = &DirectionRightEV;
                    } else if (!hasDownLeft && !hasDownRight) {
                        // .--.--.
                        // |XX|  |
                        // <--v--.
                        // |  |  |
                        // .--.--.
                        // ┌──∙──┐
                        // │██│  │
                        // ◄──▼──┤
                        // │  │  │
                        // └──┴──┘
                        processingDirectionPtr = &DirectionLeftEV;
                    }
                }
                if (processingDirectionPtr == &DirectionRightEV) {
                    toDirectionStr             = "right";
                    toEdgeDirectionStr         = "rightDown";
                    toOppositeEdgeDirectionStr = "leftDown";
                } else if (processingDirectionPtr == &DirectionLeftEV) {
                    toDirectionStr             = "left";
                    toEdgeDirectionStr         = "leftUp";
                    toOppositeEdgeDirectionStr = "rightUp";
                } else if (processingDirectionPtr == &DirectionUpEV) {
                    toDirectionStr             = "up";
                    toEdgeDirectionStr         = "upRight";
                    toOppositeEdgeDirectionStr = "downRight";
                } else if (processingDirectionPtr == &DirectionDownEV) {
                    toDirectionStr             = "down";
                    toEdgeDirectionStr         = "downLeft";
                    toOppositeEdgeDirectionStr = "upLeft";
                }
                processingMode = ProcessingMode::ExternalEdgeStep;
            } break;

            case ProcessingMode::InternalEdgeDetect: {
                if (processingDirectionPtr == &DirectionRightEV) {
                    if (!hasUpLeft && hasDownLeft) {
                        // ┌──┬──┐
                        // │  │xx│
                        // ├──∙──┤
                        // │██│xx│
                        // └──┴──┘
                        if (hasUpRight && !hasDownRight) {
                            // ┌──┬──┐
                            // │  e██│
                            // ├─e▲──►
                            // │██│  │
                            // └──∙──┘
                            processingDirectionPtr = &DirectionRightEV;
                        } else {
                            ERROR(edge, "Internal edge detection error! S{}({},{})", currentShape["id"].label(), pointX, pointY);
                        }
                    } else {
                        if (hasUpRight && hasDownRight) {
                            // ┌──┬──┐
                            // │██│██│
                            // ∙──►──┤
                            // │  │██│
                            // └──▼──┘
                            processingDirectionPtr = &DirectionDownEV;
                        } else if (hasUpRight && !hasDownRight) {
                            // ┌──┬──┐
                            // │██│██│
                            // ∙──►──►
                            // │  │  │
                            // └──┴──┘
                            processingDirectionPtr = &DirectionRightEV;
                        } else if (!hasUpRight && hasDownRight) {
                            // ┌──▲──┐
                            // │██│  │
                            // ∙──►──┤
                            // │  │██│
                            // └──┴──┘
                            processingDirectionPtr = &DirectionUpEV;
                            if (shapePoint.has("edgeJoint")) {
                                CellI& edgeJoint = shapePoint["edgeJoint"];
                                if (edgeJoint.has("upLeft") && edgeJoint.has("rightDown")) {
                                    CellI& upEdgeNode    = edgeJoint["upLeft"];
                                    CellI& rightEdgeNode = edgeJoint["rightDown"];
                                    if ((&upEdgeNode["edge"]["kind"] == &ExternalEdgeEV) && (&rightEdgeNode["edge"]["kind"] == &ExternalEdgeEV)) {
                                        // ┌──┬──┐
                                        // │██e  │
                                        // ∙──►e─┤
                                        // │  │██│
                                        // └──▼──┘
                                        processingDirectionPtr = &DirectionDownEV;
                                    }
                                }
                            }
                        } else if (!hasUpRight && !hasDownRight) {
                            // ┌──▲──┐
                            // │██│  │
                            // ∙──►──┤
                            // │  │  │
                            // └──┴──┘
                            processingDirectionPtr = &DirectionUpEV;
                        }
                    }
                } else if (processingDirectionPtr == &DirectionLeftEV) {
                    if (hasUpRight && !hasDownRight) {
                        // ┌──┬──┐
                        // │xx│██│
                        // ├──∙──┤
                        // │xx│  │
                        // └──┴──┘
                        if (!hasUpLeft && hasDownLeft) {
                            // ┌──∙──┐
                            // │  │██│
                            // ◄──▼e─┤
                            // │██e  │
                            // └──┴──┘
                            processingDirectionPtr = &DirectionLeftEV;
                        } else {
                            ERROR(edge, "Internal edge detection error! S{}({},{})", currentShape["id"].label(), pointX, pointY);
                        }
                    } else {
                        if (hasUpLeft && hasDownLeft) {
                            // ┌──▲──┐
                            // │██│  │
                            // ├──◄──∙
                            // │██│██│
                            // └──┴──┘
                            processingDirectionPtr = &DirectionUpEV;
                        } else if (hasUpLeft && !hasDownLeft) {
                            // ┌──┬──┐
                            // │██│  │
                            // ├──◄──∙
                            // │  │██│
                            // └──▼──┘
                            processingDirectionPtr = &DirectionDownEV;
                            if (shapePoint.has("edgeJoint")) {
                                CellI& edgeJoint = shapePoint["edgeJoint"];
                                if (edgeJoint.has("leftUp") && edgeJoint.has("downRight")) {
                                    CellI& downEdgeNode = edgeJoint["downRight"];
                                    CellI& leftEdgeNode = edgeJoint["leftUp"];
                                    if ((&downEdgeNode["edge"]["kind"] == &ExternalEdgeEV) && (&leftEdgeNode["edge"]["kind"] == &ExternalEdgeEV)) {
                                        // ┌──▲──┐
                                        // │██│  │
                                        // ├─e◄──∙
                                        // │  e██│
                                        // └──┴──┘
                                        processingDirectionPtr = &DirectionUpEV;
                                    }
                                }
                            }
                        } else if (!hasUpLeft && hasDownLeft) {
                            // ┌──┬──┐
                            // │  │  │
                            // ◄──◄──∙
                            // │██│██│
                            // └──┴──┘
                            processingDirectionPtr = &DirectionLeftEV;
                        } else if (!hasUpLeft && !hasDownLeft) {
                            // ┌──┬──┐
                            // │  │  │
                            // ├──◄──∙
                            // │  │██│
                            // └──▼──┘
                            processingDirectionPtr = &DirectionDownEV;
                        }
                    }
                } else if (processingDirectionPtr == &DirectionUpEV) {
                    if (!hasDownLeft && hasDownRight) {
                        // ┌──┬──┐
                        // │xx│xx│
                        // ├──∙──┤
                        // │  │██│
                        // └──┴──┘
                        if (hasUpRight && !hasUpLeft) {
                            // ┌──▲──┐
                            // │██│  │
                            // ├─e◄──∙
                            // │  e██│
                            // └──┴──┘
                            processingDirectionPtr = &DirectionUpEV;
                        } else {
                            ERROR(edge, "Internal edge detection error! S{}({},{})", currentShape["id"].label(), pointX, pointY);
                        }
                    } else {
                        if (hasUpLeft && hasUpRight) {
                            // ┌──┬──┐
                            // │██│██│
                            // ├──▲──►
                            // │██│  │
                            // └──∙──┘
                            processingDirectionPtr = &DirectionRightEV;
                        } else if (hasUpLeft && !hasUpRight) {
                            // ┌──▲──┐
                            // │██│  │
                            // ├──▲──┤
                            // │██│  │
                            // └──∙──┘
                            processingDirectionPtr = &DirectionUpEV;
                        } else if (!hasUpLeft && hasUpRight) {
                            // ┌──┬──┐
                            // │  │██│
                            // ◄──▲──┤
                            // │██│  │
                            // └──∙──┘
                            processingDirectionPtr = &DirectionLeftEV;
                            if (shapePoint.has("edgeJoint")) {
                                CellI& edgeJoint = shapePoint["edgeJoint"];
                                if (edgeJoint.has("leftDown") && edgeJoint.has("upRight")) {
                                    CellI& upEdgeNode   = edgeJoint["upRight"];
                                    CellI& leftEdgeNode = edgeJoint["leftDown"];
                                    if ((&upEdgeNode["edge"]["kind"] == &ExternalEdgeEV) && (&leftEdgeNode["edge"]["kind"] == &ExternalEdgeEV)) {
                                        // ┌──┬──┐
                                        // │  e██│
                                        // ├─e▲──►
                                        // │██│  │
                                        // └──∙──┘
                                        processingDirectionPtr = &DirectionRightEV;
                                    }
                                }
                            }
                        } else if (!hasUpLeft && !hasUpRight) {
                            // ┌──┬──┐
                            // │  │  │
                            // ◄──▲──┤
                            // │██│  │
                            // └──∙──┘
                            processingDirectionPtr = &DirectionLeftEV;
                        }
                    }
                } else if (processingDirectionPtr == &DirectionDownEV) {
                    if (hasUpLeft && !hasUpRight) {
                        // ┌──┬──┐
                        // │██│  │
                        // ├──∙──┤
                        // │xx│xx│
                        // └──┴──┘
                        if (!hasDownLeft && hasDownRight) {
                            // ┌──┬──┐
                            // │██e  │
                            // ∙──►e─┤
                            // │  │██│
                            // └──▼──┘
                            processingDirectionPtr = &DirectionDownEV;
                        } else {
                            ERROR(edge, "Internal edge detection error! S{}({},{})", currentShape["id"].label(), pointX, pointY);
                        }
                    } else {
                        if (hasDownLeft && hasDownRight) {
                            // ┌──∙──┐
                            // │  │██│
                            // ◄──▼──┤
                            // │██│██│
                            // └──┴──┘
                            processingDirectionPtr = &DirectionLeftEV;
                        } else if (hasDownLeft && !hasDownRight) {
                            // ┌──∙──┐
                            // │  │██│
                            // ├──▼──►
                            // │██│  │
                            // └──┴──┘
                            processingDirectionPtr = &DirectionRightEV;
                            if (shapePoint.has("edgeJoint")) {
                                CellI& edgeJoint = shapePoint["edgeJoint"];
                                if (edgeJoint.has("rightUp") && edgeJoint.has("downLeft")) {
                                    CellI& downEdgeNode  = edgeJoint["downLeft"];
                                    CellI& rightEdgeNode = edgeJoint["rightUp"];
                                    if ((&downEdgeNode["edge"]["kind"] == &ExternalEdgeEV) && (&rightEdgeNode["edge"]["kind"] == &ExternalEdgeEV)) {
                                        // ┌──∙──┐
                                        // │  │██│
                                        // ◄──▼e─┤
                                        // │██e  │
                                        // └──┴──┘
                                        processingDirectionPtr = &DirectionLeftEV;
                                    }
                                }
                            }
                        } else if (!hasDownLeft && hasDownRight) {
                            // ┌──∙──┐
                            // │  │██│
                            // ├──▼──┤
                            // │  │██│
                            // └──▼──┘
                            processingDirectionPtr = &DirectionDownEV;
                        } else if (!hasDownLeft && !hasDownRight) {
                            // ┌──∙──┐
                            // │  │██│
                            // ├──▼──►
                            // │  │  │
                            // └──┴──┘
                            processingDirectionPtr = &DirectionRightEV;
                        }
                    }
                }
                if (processingDirectionPtr == &DirectionRightEV) {
                    toDirectionStr             = "right";
                    toEdgeDirectionStr         = "rightUp";
                    toOppositeEdgeDirectionStr = "leftUp";
                } else if (processingDirectionPtr == &DirectionLeftEV) {
                    toDirectionStr             = "left";
                    toEdgeDirectionStr         = "leftDown";
                    toOppositeEdgeDirectionStr = "rightDown";
                } else if (processingDirectionPtr == &DirectionUpEV) {
                    toDirectionStr             = "up";
                    toEdgeDirectionStr         = "upLeft";
                    toOppositeEdgeDirectionStr = "downLeft";
                } else if (processingDirectionPtr == &DirectionDownEV) {
                    toDirectionStr             = "down";
                    toEdgeDirectionStr         = "downRight";
                    toOppositeEdgeDirectionStr = "upRight";
                }
                processingMode = ProcessingMode::InternalEdgeStep;
            } break;

            case ProcessingMode::ExternalEdgeStep:
            case ProcessingMode::InternalEdgeStep: {
                CellI& toShapePoint = shapePoint[toDirectionStr];
                CellI& newEdge      = *newEdgePtr;
                List& edgeNodes     = *edgeNodesPtr;

                // from joint
                CellI* fromEdgeJointPtr = nullptr;
                if (shapePoint.has("edgeJoint")) {
                    fromEdgeJointPtr = &shapePoint["edgeJoint"];
                } else {
                    fromEdgeJointPtr = new Object(w, ShapeEdgeJointStruct);
                    shapePoint.set("edgeJoint", *fromEdgeJointPtr);
                }
                CellI& fromEdgeJoint = *fromEdgeJointPtr;
                if (fromEdgeJoint.has(toEdgeDirectionStr)) {
                    ERROR(edge, "Edge processing error: edgeNode already exists!");
                }

                // to joint
                CellI* toEdgeJointPtr = nullptr;
                if (toShapePoint.has("edgeJoint")) {
                    toEdgeJointPtr = &toShapePoint["edgeJoint"];
                } else {
                    toEdgeJointPtr = new Object(w, ShapeEdgeJointStruct);
                    toShapePoint.set("edgeJoint", *toEdgeJointPtr);
                }

                CellI& toEdgeJoint = *toEdgeJointPtr;
                if (toEdgeJoint.has(toOppositeEdgeDirectionStr)) {
                    ERROR(edge, "Edge processing error: edgeNode already exists!");
                }

                // new edge node
                CellI& newEdgeNode = *new Object(w, ShapeEdgeNodeStruct);
                newEdgeNode.set("edge", newEdge);
                newEdgeNode.set("from", shapePoint);
                newEdgeNode.set("direction", *processingDirectionPtr);
                if (previousEdgeNodePtr) {
                    CellI& previousEdgeNode = *previousEdgeNodePtr;
                    previousEdgeNode.set("next", newEdgeNode);
                    newEdgeNode.set("previous", previousEdgeNode);
                }
                edgeNodes.add(newEdgeNode);
                fromEdgeJoint.set(toEdgeDirectionStr, newEdgeNode);
                toEdgeJoint.set(toOppositeEdgeDirectionStr, newEdgeNode);

                // stepping
                previousEdgeNodePtr  = &newEdgeNode;
                currentShapePointPtr = &toShapePoint;
                if (currentShapePointPtr != firstShapePointPtr) {
                    processingMode = (processingMode == ProcessingMode::ExternalEdgeStep) ? ProcessingMode::ExternalEdgeDetect : ProcessingMode::InternalEdgeDetect;
                } else {
                    CellI& firstEdgeNode = edgeNodes["first"]["value"];
                    firstEdgeNode.set("previous", newEdgeNode);
                    newEdgeNode.set("next", firstEdgeNode);
                    processingMode       = ProcessingMode::Searching;
                    currentListItemPtr   = previousListItemPtr;
                    currentShapePointPtr = nullptr;
                }
            } break;

            case ProcessingMode::Searching: {
                bool isUnprocessedEdge = false;

                if (hasUpLeft && hasUpRight && hasDownLeft && !hasDownRight) {
                    // ┌──┬──┐
                    // │██│██│
                    // ├──∙──►
                    // │██│  │
                    // └──┴──┘
                    isUnprocessedEdge = true;
                    if (shapePoint.has("edgeJoint") && shapePoint["edgeJoint"].has("rightUp")) {
                        isUnprocessedEdge = false;
                    }
                } else if (!hasUpLeft && hasUpRight && hasDownLeft && !hasDownRight) {
                    // ┌──┬──┐
                    // │  e██│
                    // ├─e∙──►
                    // │██│  │
                    // └──┴──┘
                    if (shapePoint.has("edgeJoint")) {
                        CellI& edgeJoint = shapePoint["edgeJoint"];
                        if (edgeJoint.has("leftDown") && edgeJoint.has("upRight")) {
                            CellI& upRightEdgeNode  = edgeJoint["upRight"];
                            CellI& leftDownEdgeNode = edgeJoint["leftDown"];
                            bool rightUpEdgeNode    = edgeJoint.has("rightUp");
                            if (!rightUpEdgeNode && (&upRightEdgeNode["edge"]["kind"] == &ExternalEdgeEV) && (&leftDownEdgeNode["edge"]["kind"] == &ExternalEdgeEV)) {
                                isUnprocessedEdge = true;
                            }
                        }
                    }
                }
                if (isUnprocessedEdge) {
                    processingDirectionPtr = &DirectionRightEV;
                    processingMode         = ProcessingMode::InternalEdgeStart;
                } else {
                    currentListItemPtr = currentListItem.has(w.id.next) ? &currentListItem[w.id.next] : nullptr;
                }
            } break;
            } // switch processinMode
        } // while has more shapePoints
    }); // visit shapePoints
}

void EdgeDetector::processEdgeNodes()
{
    CellI* firstColumnPointPtr = &(*firstShapePixelPtr())["upLeftPoint"];
    CellI* shapePointPtr       = firstColumnPointPtr;
    CellI* lastShapeEdgeInLine = nullptr;
    List internalEdges(w, ShapeEdgeStruct);
    List::Item* lastInternalEdgeItem = nullptr;
    enum class ProcessingDirection
    {
        LeftToRight,
        UpToDown
    };

    ProcessingDirection processingDirection = ProcessingDirection::LeftToRight;

    auto setExternalShape = [this](CellI& shapeEdgeNode) {
        CellI& shapePoint        = shapeEdgeNode["from"];
        CellI* externalDirection = &shapeEdgeNode["edge"]["kind"] == &ExternalEdgeEV ? &DirectionLeftEV : &DirectionRightEV;

        // rotate 🡪 == clockwise
        // rotate 🡨 == counterclockwise
        const char* shapePixelDirection = nullptr;
        if (&shapeEdgeNode["direction"] == &DirectionRightEV) {
            if (externalDirection == &DirectionLeftEV) {
                shapePixelDirection = "upRightPixel"; // 🡪 rotate 🡨 = 🡭
            } else {
                shapePixelDirection = "downRightPixel"; // 🡪 rotate 🡪 = 🡮
            }
        } else if (&shapeEdgeNode["direction"] == &DirectionLeftEV) {
            if (externalDirection == &DirectionLeftEV) {
                shapePixelDirection = "downLeftPixel"; // 🡨 rotate 🡨 = 🡯
            } else {
                shapePixelDirection = "upLeftPixel"; // 🡨 rotate 🡪 = 🡬
            }
        } else if (&shapeEdgeNode["direction"] == &DirectionUpEV) {
            if (externalDirection == &DirectionLeftEV) {
                shapePixelDirection = "upLeftPixel"; // 🡩 rotate 🡨 = 🡬
            } else {
                shapePixelDirection = "upRightPixel"; // 🡩 rotate 🡪 = 🡭
            }
        } else if (&shapeEdgeNode["direction"] == &DirectionDownEV) {
            if (externalDirection == &DirectionLeftEV) {
                shapePixelDirection = "downRightPixel"; // 🡫 rotate 🡨 = 🡮
            } else {
                shapePixelDirection = "downLeftPixel"; // 🡫 rotate 🡪 = 🡯
            }
        }
        if (shapePoint.has(shapePixelDirection)) {
            CellI& externalShape = shapePoint[shapePixelDirection]["shape"];
            shapeEdgeNode.set("externalShape", externalShape);
        }
    };

    while (shapePointPtr) {
        CellI& shapePoint = *shapePointPtr;
        const int x       = static_cast<Number&>(shapePoint["x"]).value();
        const int y       = static_cast<Number&>(shapePoint["y"]).value();

        if (shapePoint.has("edgeJoint")) {
            CellI& edgeJoint = shapePoint["edgeJoint"];
            switch (processingDirection) {
            case ProcessingDirection::LeftToRight: {
                if (edgeJoint.has("rightUp")) {
                    setExternalShape(edgeJoint["rightUp"]);
                }
                if (edgeJoint.has("rightDown")) {
                    setExternalShape(edgeJoint["rightDown"]);
                }
            } break;
            case ProcessingDirection::UpToDown: {
                if (edgeJoint.has("downLeft")) {
                    CellI& shapeEdgeNode = edgeJoint["downLeft"];
                    setExternalShape(shapeEdgeNode);
                    CellI& shapeEdge = shapeEdgeNode["edge"];
                    if (&shapeEdge["kind"] == &InternalEdgeEV) {
                        // entering an internal edge
                        lastInternalEdgeItem = internalEdges.add(shapeEdge);
                    }
                }
                if (edgeJoint.has("downRight")) {
                    CellI& shapeEdgeNode = edgeJoint["downRight"];
                    setExternalShape(shapeEdgeNode);
                    CellI& shapeEdge = shapeEdgeNode["edge"];
                    if (&shapeEdge["kind"] == &InternalEdgeEV) {
                        // leaving an internal edge
                        internalEdges.remove(lastInternalEdgeItem);
                        if (!internalEdges.empty()) {
                            lastInternalEdgeItem = &static_cast<List::Item&>(internalEdges["last"]);
                        }
                    }
                    lastShapeEdgeInLine = &shapeEdge;
                }
                if (shapePoint.has("downRightPixel")) {
                    CellI* edgePixelListPtr = nullptr;
                    CellI& shapeEdge        = *lastShapeEdgeInLine;
                    if (shapeEdge.missing("shapePixels")) {
                        List& edgeNodes = *new List(w, ShapePixelStruct);
                        shapeEdge.set("shapePixels", edgeNodes);
                        edgePixelListPtr = &edgeNodes;
                    } else {
                        edgePixelListPtr = &shapeEdge["shapePixels"];
                    }
                    List& edgePixelList = static_cast<List&>(*edgePixelListPtr);
                    CellI& shapePixel   = shapePoint["downRightPixel"];
                    edgePixelList.add(shapePixel);
                    if (!internalEdges.empty()) {
                        CellI& lastInternalEdge = internalEdges["last"]["value"];
                        CellI* shapeSetPtr      = nullptr;
                        if (lastInternalEdge.missing("shapes")) {
                            Set& newShapesSet = *new Set(w, ShapeStruct);
                            lastInternalEdge.set("shapes", newShapesSet);
                            shapeSetPtr = &newShapesSet;
                        } else {
                            shapeSetPtr = &lastInternalEdge["shapes"];
                        }
                        Set& shapesSet = static_cast<Set&>(*shapeSetPtr);
                        CellI& shape   = shapePixel["shape"];
                        if (!shapesSet.contains(shape)) {
                            shapesSet.add(shapePixel["shape"]);
                        }
                    }
                }
            } // case
            } // switch
        }

        // stepping
        if (shapePoint.has("right")) {
            shapePointPtr = &shapePoint["right"];
        } else if (shapePoint.has("down")) {
            switch (processingDirection) {
            case ProcessingDirection::LeftToRight:
                processingDirection = ProcessingDirection::UpToDown;
                shapePointPtr       = firstColumnPointPtr;
                break;
            case ProcessingDirection::UpToDown:
                processingDirection = ProcessingDirection::LeftToRight;
                shapePointPtr       = &(*firstColumnPointPtr)["down"];
                firstColumnPointPtr = shapePointPtr;
                break;
            }
        } else {
            shapePointPtr = nullptr;
        }
    }
}

void EdgeDetector::findRotationCornersUpLeft()
{
    CellI* firstColumnPointPtr = &frame()["upLeftPoint"];
    CellI* shapePointPtr       = firstColumnPointPtr;
    while (shapePointPtr) {
        CellI& shapePoint = *shapePointPtr;

        if (shapePoint.has("edgeJoint")) {
            CellI& edgeJoint = shapePoint["edgeJoint"];
            if (edgeJoint.has("rightDown")) {
                CellI& node = edgeJoint["rightDown"];
                CellI& edge = node["edge"];
                if (&edge["kind"] == &ExternalEdgeEV) {
                    CellI& corners = edge["rotationCorners"];
                    if (corners.missing("upLeftNode")) {
                        corners.set("upLeftNode", node);
                    }
                }
            }
            if (edgeJoint.has("rightUp")) {
                CellI& node = edgeJoint["rightUp"];
                CellI& edge = node["edge"];
                if (&edge["kind"] == &InternalEdgeEV) {
                    CellI& corners = edge["rotationCorners"];
                    if (corners.missing("upLeftNode")) {
                        corners.set("upLeftNode", node);
                    }
                }
            }
        }

        // stepping
        if (shapePoint.has("right")) {
            shapePointPtr = &shapePoint["right"];
        } else if (shapePoint.has("down")) {
            shapePointPtr       = &(*firstColumnPointPtr)["down"];
            firstColumnPointPtr = shapePointPtr;
        } else {
            shapePointPtr = nullptr;
        }
    }
}

void EdgeDetector::findRotationCornersUpRight()
{
    DEBUG(shapeCorners, "findCornersUpRight");

    CellI* lastColumnPointPtr = &frame()["upRightPoint"];
    CellI* shapePointPtr      = lastColumnPointPtr;
    while (shapePointPtr) {
        CellI& shapePoint = *shapePointPtr;
        if (shapePoint.has("edgeJoint")) {
            CellI& edgeJoint = shapePoint["edgeJoint"];
            if (edgeJoint.has("downLeft")) {
                CellI& node = edgeJoint["downLeft"];
                CellI& edge = node["edge"];
                if (&edge["kind"] == &ExternalEdgeEV) {
                    CellI& corners = edge["rotationCorners"];
                    if (corners.missing("upRightNode")) {
                        corners.set("upRightNode", node);
                        TRACE(shapeCorners, "upRight: {}({}, {})", node["edge"]["shape"]["id"].label(), node["from"]["x"].label(), node["from"]["y"].label());
                    }
                }
            }
            if (edgeJoint.has("downRight")) {
                CellI& node = edgeJoint["downRight"];
                CellI& edge = node["edge"];
                if (&edge["kind"] == &InternalEdgeEV) {
                    CellI& corners = edge["rotationCorners"];
                    if (corners.missing("upRightNode")) {
                        corners.set("upRightNode", node);
                        TRACE(shapeCorners, "upRight: {}({}, {})", node["edge"]["shape"]["id"].label(), node["from"]["x"].label(), node["from"]["y"].label());
                    }
                }
            }
        }

        // stepping
        if (shapePoint.has("down")) {
            shapePointPtr = &shapePoint["down"];
        } else if (shapePoint.has("left")) {
            shapePointPtr      = &(*lastColumnPointPtr)["left"];
            lastColumnPointPtr = shapePointPtr;
        } else {
            shapePointPtr = nullptr;
        }
    }
}

void EdgeDetector::findRotationCornersDownLeft()
{
    CellI* firstColumnPointPtr = &frame()["downLeftPoint"];
    CellI* shapePointPtr       = firstColumnPointPtr;
    while (shapePointPtr) {
        CellI& shapePoint = *shapePointPtr;

        if (shapePoint.has("edgeJoint")) {
            CellI& edgeJoint = shapePoint["edgeJoint"];
            if (edgeJoint.has("upRight")) {
                CellI& node = edgeJoint["upRight"];
                CellI& edge = node["edge"];
                if (&edge["kind"] == &ExternalEdgeEV) {
                    CellI& corners = edge["rotationCorners"];
                    if (corners.missing("downLeftNode")) {
                        corners.set("downLeftNode", node);
                    }
                }
            }
            if (edgeJoint.has("upLeft")) {
                CellI& node = edgeJoint["upLeft"];
                CellI& edge = node["edge"];
                if (&edge["kind"] == &InternalEdgeEV) {
                    CellI& corners = edge["rotationCorners"];
                    if (corners.missing("downLeftNode")) {
                        corners.set("downLeftNode", node);
                    }
                }
            }
        }

        // stepping
        if (shapePoint.has("up")) {
            shapePointPtr = &shapePoint["up"];
        } else if (shapePoint.has("right")) {
            shapePointPtr       = &(*firstColumnPointPtr)["right"];
            firstColumnPointPtr = shapePointPtr;
        } else {
            shapePointPtr = nullptr;
        }
    }
}

void EdgeDetector::findRotationCornersDownRight()
{
    CellI* lastColumnPointPtr = &frame()["downRightPoint"];
    CellI* shapePointPtr      = lastColumnPointPtr;
    while (shapePointPtr) {
        CellI& shapePoint = *shapePointPtr;
        if (shapePoint.has("edgeJoint")) {
            CellI& edgeJoint = shapePoint["edgeJoint"];
            if (edgeJoint.has("leftUp")) {
                CellI& node = edgeJoint["leftUp"];
                CellI& edge = node["edge"];
                if (&edge["kind"] == &ExternalEdgeEV) {
                    CellI& corners = edge["rotationCorners"];
                    if (corners.missing("downRightNode")) {
                        corners.set("downRightNode", node);
                    }
                }
            }
            if (edgeJoint.has("leftDown")) {
                CellI& node = edgeJoint["leftDown"];
                CellI& edge = node["edge"];
                if (&edge["kind"] == &InternalEdgeEV) {
                    CellI& corners = edge["rotationCorners"];
                    if (corners.missing("downRightNode")) {
                        corners.set("downRightNode", node);
                    }
                }
            }
        }

        // stepping
        if (shapePoint.has("left")) {
            shapePointPtr = &shapePoint["left"];
        } else if (shapePoint.has("up")) {
            shapePointPtr      = &(*lastColumnPointPtr)["up"];
            lastColumnPointPtr = shapePointPtr;
        } else {
            shapePointPtr = nullptr;
        }
    }
}

void EdgeDetector::findMirroringCornersUpRight()
{
    CellI* firstColumnPointPtr = &frame()["upRightPoint"];
    CellI* shapePointPtr       = firstColumnPointPtr;
    while (shapePointPtr) {
        CellI& shapePoint = *shapePointPtr;

        if (shapePoint.has("edgeJoint")) {
            CellI& edgeJoint = shapePoint["edgeJoint"];

            if (edgeJoint.has("leftDown")) {
                CellI& node = edgeJoint["leftDown"];
                CellI& edge = node["edge"];
                if (&edge["kind"] == &ExternalEdgeEV) {
                    CellI& corners = node["edge"]["mirroringCorners"];
                    if (corners.missing("upRightNode")) {
                        corners.set("upRightNode", node);
                    }
                }
            }

            if (edgeJoint.has("leftUp")) {
                CellI& node = edgeJoint["leftUp"];
                CellI& edge = node["edge"];
                if (&edge["kind"] == &InternalEdgeEV) {
                    CellI& corners = node["edge"]["mirroringCorners"];
                    if (corners.missing("upRightNode")) {
                        corners.set("upRightNode", node);
                    }
                }
            }
        }

        // stepping
        if (shapePoint.has("left")) {
            shapePointPtr = &shapePoint["left"];
        } else if (shapePoint.has("down")) {
            shapePointPtr       = &(*firstColumnPointPtr)["down"];
            firstColumnPointPtr = shapePointPtr;
        } else {
            shapePointPtr = nullptr;
        }
    }
}

void EdgeDetector::findMirroringCornersDownLeft()
{
    CellI* firstColumnPointPtr = &frame()["downLeftPoint"];
    CellI* shapePointPtr       = firstColumnPointPtr;
    while (shapePointPtr) {
        CellI& shapePoint = *shapePointPtr;

        if (shapePoint.has("edgeJoint")) {
            CellI& edgeJoint = shapePoint["edgeJoint"];

            if (edgeJoint.has("rightUp")) {
                CellI& node = edgeJoint["rightUp"];
                CellI& edge = node["edge"];
                if (&edge["kind"] == &ExternalEdgeEV) {
                    CellI& corners = node["edge"]["mirroringCorners"];
                    if (corners.missing("downLeftNode")) {
                        corners.set("downLeftNode", node);
                    }
                }
            }

            if (edgeJoint.has("rightDown")) {
                CellI& node = edgeJoint["rightDown"];
                CellI& edge = node["edge"];
                if (&edge["kind"] == &InternalEdgeEV) {
                    CellI& corners = node["edge"]["mirroringCorners"];
                    if (corners.missing("downLeftNode")) {
                        corners.set("downLeftNode", node);
                    }
                }
            }
        }

        // stepping
        if (shapePoint.has("right")) {
            shapePointPtr = &shapePoint["right"];
        } else if (shapePoint.has("up")) {
            shapePointPtr       = &(*firstColumnPointPtr)["up"];
            firstColumnPointPtr = shapePointPtr;
        } else {
            shapePointPtr = nullptr;
        }
    }
}

CellI* EdgeDetector::firstShapePixelPtr()
{
    Object& shapePixels = static_cast<Object&>(frame()["shapePixels"]);
    Object& colX        = static_cast<Object&>(shapePixels.method(w.name("getValue"), { w.id.key, _0_ }));
    CellI& shapePixel   = colX.method(w.name("getValue"), { w.id.key, _0_ });
    return &shapePixel;
}

void EdgeDetector::findPossibleBackgroundWithShapes()
{
    DEBUG(shapeRelations, "findPossibleBackgroundWithShapes");

    List& shapesList = static_cast<List&>(frame()["shapes"]);
    int shapesCount  = shapesList.size();
    if (shapesCount < 2) {
        TRACE(shapeRelations, "  there isn't enough shape");
        return;
    }
    const int targetContainedShapeCount = shapesCount - 1;
    CellI* backgroundShapePtr           = nullptr;
    Visitor::visitList(shapesList, [this, targetContainedShapeCount, &backgroundShapePtr](CellI& shape, int, bool&) {
        int edgesCount = getShapeEdgesSize(shape);
        if (edgesCount == 1) {
            return;
        }
        int containedShapeCount = 0;
        Visitor::visitList(shape["edges"]["list"], [this, &containedShapeCount](CellI& edge, int, bool&) {
            if (!(&edge["kind"] == &InternalEdgeEV && edge.has("shapes"))) {
                return;
            }
            containedShapeCount += static_cast<Set&>(edge["shapes"]).size();
        });
        if (targetContainedShapeCount == containedShapeCount) {
            TRACE(shapeRelations, "Perfect match for a backround");
            backgroundShapePtr = &shape;
        } else if (containedShapeCount > 1) {
            // some heuristic about the longest border
            CellI& externalEdge = shape["edges"]["list"]["first"]["value"];
            std::map<int, int> longestBorder;
            Visitor::visitList(externalEdge["edgeNodes"], [this, &longestBorder](CellI& edgeNode, int i, bool& stop) {
                int externalShapeId;
                if (edgeNode.has("externalShape")) {
                    externalShapeId = static_cast<Number&>(edgeNode["externalShape"]["id"]).value();
                } else {
                    externalShapeId = 0;
                }
                longestBorder[externalShapeId]++;
            });
            int longestBorderLength = 0;
            int selectedShapeId     = -1;
            std::set<int> lengthStat;
            for (const auto& pair : longestBorder) {
                if (pair.second > longestBorderLength) {
                    selectedShapeId     = pair.first;
                    longestBorderLength = pair.second;
                }
                lengthStat.insert(pair.second);
            }
            int secondPlace = 0;
            if (lengthStat.size() >= 2) {
                secondPlace = *(++(lengthStat.rbegin()));
            }
            TRACE(shapeRelations, "Longest border is with shape id: {}:{} second: {}", selectedShapeId, longestBorderLength, secondPlace);
        }
    });
    if (backgroundShapePtr) {
        CellI& backgroundShape = *backgroundShapePtr;
        INFO(shapeRelations, "    shape id {} can be a background as it contains all other shapes!", backgroundShape["id"].label());
    }
#if 0
        Next approach:
        We need a shape assessment process

            - a shape can be inside an other shape
                0   1  2
              ┌──┬──┬──┐
            0 │❶ │❶ │❶ │
              ├──∙──∙──┤
            1 │❶ │❷ │❶ │
              ├──∙──∙──┤
            2 │❶ │❶ │❶ │
              └──┴──┴──┘
            - a shape can contains an other one
                0   1  2    3  4
              ┌──┬──┬──┬──┬──┐
            0 │❶ │❶ │❷ │❶ │❶ │
              ├──∙──∙──∙──∙──┤
            1 │❶ │❷ │❸ │❷ │❶ │
              ├──∙──∙──∙──∙──┤
            2 │❶ │❶ │❷ │❶ │❶ │
              ├──∙──∙──∙──∙──┤
            3 │❶ │❶ │❶ │❶ │❶ │
              └──┴──┴──┴──┴──┘
            - backround can extend if the color is the same
                0   1  2    3  4
              ┌──┬──┬──┬──┬──┐
            0 │❶ │❶ │❶ │❶ │❶ │
              ├──∙──∙──∙──∙──┤
            1 │❷ │❶ │❷ │❶ │❶ │
              ├──∙──∙──∙──∙──┤
            2 │❶ │❸ │❶ │❶ │❶ │
              ├──∙──∙──∙──∙──┤
            3 │❷ │❶ │❷ │❶ │❶ │
              └──┴──┴──┴──┴──┘
        An other idea:
          if a shape contains other ones, lets examine it even more
          so if this shape touch four external same edge (or missing external edges)
              then calculate the conatined (between external edges) shapes also
              - maybe just assume that the external edge is, where it has the most contact when doesn't contain it?
          also we can shortcut in this case and pretend, that the external shape edge is the full edge
              we need a frame object wich has the edge of the input grid

       Ok, so looks like there isn't any perfect algorithm to detect a background, but we collect clues about it

       One heuristic algo is, to predict the background real external edges by take into account the most "touched" edges.

       An other clue is that what is fix and what is changing.

#endif
}
class Vector : public cells::CellI
{
public:
    Vector(World& w, Number& x, Number& y) :
        CellI(w, "Vector"),
        m_x(x),
        m_y(y)
    { }

    bool has(CellI& key) override
    {
        if (&key == &w.id.__type__) {
            return true;
        }
        if (&key == &w.id.coordinates.x) {
            return true;
        }
        if (&key == &w.id.coordinates.y) {
            return true;
        }

        return false;
    }

    void set(CellI& key, CellI& value) override
    {
        throw "Changing a hybrid offset cell is not possible!";
    }

    void erase(CellI& key) override
    {
        throw "Changing a hybrid offset cell is not possible!";
    }

    void operator()() override
    {
        // Do nothing, this is a data cell
    }

    CellI& operator[](CellI& key) override
    {
        if (&key == &w.id.__type__) {
            static CellI& VectorStruct = w.getStruct("arc::Vector");

            return VectorStruct;
        }
        if (&key == &w.id.coordinates.x) {
            return m_x;
        }
        if (&key == &w.id.coordinates.y) {
            return m_y;
        }

        throw "No such key!";
    }

    void accept(Visitor& visitor) override
    {
        visitor.visit(*this);
    }

    Number& m_x;
    Number& m_y;
};

class Shape : public cells::CellI
{
public:
    Shape(World& w, cells::CellI& color, List& externalEdgeLine, cells::TrieMap& internalEdges) :
        cells::CellI(w),
        m_color(color),
        m_externalEdgeLine(externalEdgeLine),
        m_internalEdges(internalEdges) { }

    using CellI::get;
    using CellI::has;
    using CellI::missing;
    using CellI::set;
    using CellI::erase;
    using CellI::operator[];

    bool has(CellI& key) override
    {
        if (&key == &w.id.__type__) {
            return true;
        }
        if (&key == &w.name("externalEdgeLine")) {
            return true;
        }
        if (&key == &w.id.color) {
            return true;
        }
        if (&key == &w.name("internalEdges")) {
            return true;
        }

        return false;
    }

    void set(CellI& key, CellI& value) override
    {
        throw "Changing a hybrid offset cell is not possible!";
    }

    void erase(CellI& key) override
    {
        throw "Changing a hybrid offset cell is not possible!";
    }

    void operator()() override
    {
        // Do nothing, this is a data cell
    }

    CellI& operator[](CellI& key) override
    {
        if (&key == &w.id.__type__) {
            return w.std.Cell; // TODO
        }
        if (&key == &w.name("externalEdgeLine")) {
            return m_externalEdgeLine;
        }
        if (&key == &w.id.color) {
            return m_color;
        }
        if (&key == &w.name("internalEdges")) {
            return m_internalEdges;
        }

        throw "No such key!";
    }

    void accept(Visitor& visitor) override
    {
        visitor.visit(*this);
    }

    // getShape(pos) == shape
    void addInternalEdge(Vector& offset, List& edge)
    {
        m_internalEdges.addWithDataKey(offset, edge);
    }

    List& m_externalEdgeLine;
    cells::CellI& m_color;
    cells::TrieMap& m_internalEdges;
};

class RootFrame : public cells::CellI
{
public:
    RootFrame(World& w, int width, int height) :
        cells::CellI(w),
        m_width(w, width),
        m_height(w, height),
        m_shapesMap(w,w.std.Cell, w.std.Cell) // TODO
    { }

    using CellI::erase;
    using CellI::get;
    using CellI::has;
    using CellI::missing;
    using CellI::set;
    using CellI::operator[];

    bool has(CellI& key) override
    {
        if (&key == &w.id.__type__) {
            return true;
        }
        if (&key == &w.id.width) {
            return true;
        }
        if (&key == &w.id.height) {
            return true;
        }
        if (&key == &w.name("shapesMap")) {
            return true;
        }

        return false;
    }

    void set(CellI& key, CellI& value) override
    {
        throw "Changing a hybrid offset cell is not possible!";
    }

    void erase(CellI& key) override
    {
        throw "Changing a hybrid offset cell is not possible!";
    }

    void operator()() override
    {
        // Do nothing, this is a data cell
    }

    CellI& operator[](CellI& key) override
    {
        if (&key == &w.id.__type__) {
            return w.std.Cell; // TODO
        }
        if (&key == &w.id.width) {
            return m_width;
        }
        if (&key == &w.id.height) {
            return m_height;
        }
        if (&key == &w.name("shapesMap")) {
            return m_shapesMap;
        }

        throw "No such key!";
    }

    void accept(Visitor& visitor) override
    {
        visitor.visit(*this);
    }

    // getShape(pos) == shape
    void addShape(Vector& offset, Shape& shape)
    {
        m_shapesMap.addWithDataKey(offset, shape);
    }
    Shape& getShape(const Vector& offset);

    Number m_width;
    Number m_height;
    cells::TrieMap m_shapesMap;
};

void EdgeDetector::createResult()
{
    int width = static_cast<Number&>(frame().get(w.id.width)).value();
    int height = static_cast<Number&>(frame().get(w.id.height)).value();
    RootFrame rootFrame(w, width, height);

    Visitor::visitList(frame()["shapes"], [this, &rootFrame](CellI& currentShape, int, bool&) {
        TRACE(edge, "Shape id: {}, points:", currentShape["id"].label());
        static CellI& ArcDirections = w.getStruct("arc::Directions");

        // offset
        CellI& firstPoint      = currentShape["shapePoints"][w.id.first][w.id.value];
        Number& x              = static_cast<Number&>(firstPoint["x"]);
        Number& y              = static_cast<Number&>(firstPoint["y"]);
        Vector& offset         = *new Vector(w, x, y);
        CellI& edgesList       = currentShape["edges"]["list"];
        List& externalEdgeLine = *new List(w, ArcDirections);
        TrieMap& internalEdges = *new TrieMap(w, w.std.Cell, w.std.Cell);

        Visitor::visitList(edgesList, [this, &externalEdgeLine, &internalEdges](CellI& currentEdge, int i, bool&) {
            List* outEdgePtr = nullptr;
            // the first edge is the external edge
            if (i == 0) {
                outEdgePtr = &externalEdgeLine;
            } else {
                Number& x                  = static_cast<Number&>(currentEdge["fromExternalX"]);
                Number& y                  = static_cast<Number&>(currentEdge["fromExternalY"]);
                Vector& offset             = *new Vector(w, x, y);
                List& internalEdgeLine     = *new List(w, ArcDirections);
                internalEdges.addWithDataKey(offset, internalEdgeLine);
                outEdgePtr = &internalEdgeLine;
            }
            List& outEdge = *outEdgePtr;

            Visitor::visitList(currentEdge["edgeNodes"], [this, &outEdge](CellI& node, int i, bool&) {
                outEdge.add(node["direction"]);
            });
        });
        Shape& shape = *new Shape(w, currentShape["color"], externalEdgeLine, internalEdges);
        rootFrame.addShape(offset, shape);
    });
    auto printDirection = [this](CellI& direction) {
        if (&direction == &DirectionUpEV) {
            std::cout << "🡩 ";
        } else if (&direction == &DirectionDownEV) {
            std::cout << "🡫 ";
        } else if (&direction == &DirectionLeftEV) {
            std::cout << "🡨 ";
        } else if (&direction == &DirectionRightEV) {
            std::cout << "🡪 ";
        }
    };
    std::cout << "RootFrame rootFrame(width: " << static_cast<Number&>(rootFrame.m_width).value() << ", height: " << static_cast<Number&>(rootFrame.m_height).value() << ");" << std::endl;
    class RootFrameMaker : public AstHelper
    {
    public:
        Base* ast = nullptr;
        RootFrameMaker(World& w, Number& width, Number& height) :
            AstHelper(w)
        {
            ast = &(var_("rootFrame") = new_("RootFrame", "new")("width", _(width))("height", _(height)));
        }
    } rootFrameMaker(w, rootFrame.m_width, rootFrame.m_height);
    CellI& astNewRootFrame = *rootFrameMaker.ast;
    CellI& shapesMap = rootFrame["shapesMap"];
    Visitor::visitList(shapesMap[id.list], [this, &printDirection, &shapesMap](CellI& kvPair, int, bool&) {
        Vector& offset = static_cast<Vector&>(kvPair[w.id.key]);
        Shape& shape   = static_cast<Shape&>(kvPair[w.id.value]);
        std::cout << "rootFrame.addShape({ " << shape.m_color.label() << ", [" << offset.m_x.value() << ", " << offset.m_y.value() << "], { ";

        class GetShapeAst : public AstHelper
        {
        public:
            Base* ast = nullptr;
            GetShapeAst(World& w, CellI& offset, CellI& shape) :
                AstHelper(w)
            {
                Base& var = equal(var_("rootFrame")("getShape")("offset", _(offset)), _(shape));
            }
        } getShapeAst(w, offset, shape);

        Visitor::visitList(shape["externalEdgeLine"], [this, &printDirection](CellI& direction, int, bool&) {
            printDirection(direction);
        });
        std::cout << "}";
        if (!shape.m_internalEdges.empty()) {
            std::cout << ", inEdges: {";
        }
        Visitor::visitList(shape["internalEdges"][id.list], [this, &printDirection, &shape](CellI& kvPair, int, bool&) {
            Vector& offset     = static_cast<Vector&>(kvPair[w.id.key]);
            List& internalEdge = static_cast<List&>(kvPair[w.id.value]);
            std::cout << "[" << offset.m_x.value() << ", " << offset.m_y.value() << "], { ";
            Visitor::visitList(internalEdge, [this, &printDirection](CellI& direction, int, bool&) {
                printDirection(direction);
            });
            std::cout << "}";
        });
        if (!shape.m_internalEdges.empty()) {
            std::cout << "}";
        }
        std::cout << "});" << std::endl;
    });

    ToolFinder& toolFinder = *w.globalScope.m_toolFinder;
    toolFinder.findConversionTools(w._2_, w._4_);
    toolFinder.findConversionTools(w.false_, w.true_);

#if 0
┌──┬──┬──┬──┐
│  │██│██│██│
├──┼──┼──┼──┤
│██│██│  │██│
├──┼──┼──┼──┤
│  │██│██│██│
└──┴──┴──┴──┘
RootFrame rootFrame(width: 4, height: 3);
rootFrame.addShape({[0, 0], { Color::black,  🡪 🡫 🡨 🡩 });
rootFrame.addShape({[1, 0], { Color::orange, 🡪 🡪 🡪 🡫 🡫 🡫 🡨 🡨 🡨 🡩 🡨 🡩 🡪 🡩 }, inEdges: {[1, 1], { 🡪 🡫 🡨 🡩 }});
rootFrame.addShape({[2, 1], { Color::black,  🡪 🡫 🡨 🡩 });
rootFrame.addShape({[0, 2], { Color::black,  🡪 🡫 🡨 🡩 });

// rootFrame = new RootFrame(.width = 4, .height = 3)
// this.getShape([0, 0]) == { Color::black,  🡪 🡫 🡨 🡩 };
// this.getShape([1, 0]) == { Color::orange, 🡪 🡫 🡨 🡩 }, inEdges: {[1, 1], { 🡪 🡫 🡨 🡩 }};
// this.getShape([2, 1]) == { Color::black,  🡪 🡫 🡨 🡩 };
// this.getShape([0, 2]) == { Color::black,  🡪 🡫 🡨 🡩 };

description {
var pos1 = new Point(1, 2);

pos1.get(x) == 1;
pos1.get(y) == 2;
shape1.get(ids.color) == arc::Color::orange;
shape1.get(ids.shapes) == { up, right, down, left }
internalEdge.getShape(pos1) == shape1;
internalEdge.getShape(new_("Vector", "constructor")("x", m_("x"))("y", m_("y"))) == shape1;

// m_parentInternalEdge.getShape(m_pos) == m_shape
struct SubShape {
    InternalEdge m_parentInternalEdge;
    Pos m_pos;
    Shape m_shape;
};

}
#endif
}

} // namespace arc
} // namespace infocell

    /*
Tasks

1. I want itarate through objects which contains elements. So I want to handle objects as containers.
   Main use case is, the ArcGrid which contains ArcPixels. Connected same color ArcPixels can form some shape (line, box, etc..)
   Other use case, processing elemnts in list.

   So I want a cells::Iterator and cells::Iterable like thing which has at least a Next method (which gives back the current element AND steps the iterator).

     - in Rust cells::Iterable is std::iter::IntoIterator trait which contains a fn into_iter(self) -> std::iter::Iterator<Item = Self::Item>
     - in C# it is called IEnumerable which contains a IEnumerator GetEnumerator() method
     - in Java it is called Iterable<T> which contains a Iterator<T> iterator() method

   Three type of interfaces are possible.

     - Normal interface without any extra type input
     - Generic or templated interface which expects a type during usage and constructing, so it can create a unique type name for it
     - Associated type parameter or type constructor where you can create Interface objects which are types. Every type instance will have the same type name.

Interfaces

   We can add an extra "interfaces" key to the struct description beside the "methods"
   Current struct:
    astScope.add<Struct>("Struct")
        .members(
            member("name", "std::Cell"),
            ...
            member("methods", MapOf(std.Cell, std.ast.Function)),

    astScope.add<Struct>("Struct")
        .members(
            member("name", "std::Cell"),
            ...
            member("interfaces", MapOf(std.Cell, std.ast.Struct)), // interface name to Struct
            member("methods", MapOf(std.Cell, std.ast.Function)),

    I think the interface can be a struct in first, maybe we can change it later
    If two interfaces require to implement the same method name then we shouldn't allow it now, as resolve it takes too much time now, I think this a nice to have feature
      ... or it can be an enum like thing, so methods becames MapOf(std.Cell, enum<std.ast.Function, List<std.ast.Function>))

Object uniqueness

   So we should create a primaryKey()->List or contentList()->List like method maybe behind an interface. Or every object should inherit from an object interface.
   Can we autogenerate this, by indicate the uniquness of objects somehow?

   Primary use case to be able to put elements to a containers. So we have to ArcGrid with the same size. One of them contains extra pixels.
   So the strategy is to put every pixels to a container and look up those pixels from the other grid. Maybe the ArcGrid should be a container already?


*/

/*
The strategy is to interpret the input and output as a set of objects which objects has a variety of properties. The start state is obviously a set of pixels
where the pixels has coordinates. The pixels are in a set called grid. We can interpret (parse) the objects to other thing, for example 4 pixels in a line
is actually can be interpreted (parse) as a line object.
   Input:
      Grid { Pixel1, Pixel2, ... , PixelLast }
         Interpretations: Line { startPixel, endPixel, size }

What is looks like very important is to able to observe, how the observed objects are relative to each other.
So in the input we have an input grid which contains pixels.
In a next layer we need Shapes which contains shape-pixels. A shape pixel has a shape id and a pixel.
   Input:
      Grid { Pixel1, Pixel2, ... , PixelLast }
          Shapes { Shape1 { ShapePixel1 { shape: Shape, left: ShapePixel, up: ShapePixel,  ... }

So we interpret the input and output as a set of objects, now the challenge is to find the transformation algorithm. The strategy here is to make as many observation about the change as possible
and find those which are true for every case.

We need an ObservationMaker object which creates Observations

Observations between input and output:
  Same
   - sourceObjectDef: object definition
  ResultOperation: Copy

  RelativeObjectAdd, RelativeObjectRemove
  Relative to
   - sourceObjectDef: object definition
   - distance: vector distance (x, y)
   - targetObjectDef: object definition
   - operation: appeared, disappeared
  ResultOperation: SetPixel

  Move
  Relative to
   - object definition
   - vector distance (x, y)
  ResultOperation: SetPixel()

  Pattern
   - object definition
   - vector distance (x, y)
   - count

  Symmetry

  Rotation


We can categorize the result of a set difference algorithm:
  1. the two start state grid has the same size, and the output grid contains only extra pixels.

     In this case we try to observe how the new pixels relate to the original one.
     - The new pixels can be relative to the original objects:
        - new pixels are at the border of the original grid
        - new pixels are at the border of one of the original object on the grid

  2. the two start state grid has the same size, and the output grid has some missing pixels

  3. the two start state grid has the same size
     input pixel missing but same number pixel appear on output, so the pixel is moving
     We need a moving observer



Example

BB = Blue pixel
OO = Orange pixel
RR = Red pixel
YY = Yellow pixel
.. = Black pixel

    Input                    Output
  0 1 2 3 4 5 6 7 8x       0 1 2 3 4 5 6 7 8x
0 .................      0 .................
1 .................      1 .................
2 .................      2 ..YY..YY.........
3 ....RR...........      3 ....RR...........
4 .................  =>  4 ..YY..YY.........
5 .................      5 ............OO...
6 ............BB...      6 ..........OOBBOO.
7 .................      7 ............OO...
8 .................      8 .................
y                        y

Grid1 9 x 9 => Grid 9 x 9

If black Pixel is categorized as background, then
  The input grid contains
     - Pixel { red, x=2, y=3 }
     - Pixel { blue, x=6, y=6 }
  The output grid contains
     - Pixel { red, x=2, y=3 }
     - Pixel { blue, x=6, y=6 }
     - Pixel { yellow, x=1, y=2 }
     - Pixel { yellow, x=3, y=2 }
     - Pixel { yellow, x=1, y=4 }
     - Pixel { yellow, x=3, y=4 }
     - Pixel { orange, x=6, y=5 }
     - Pixel { orange, x=5, y=6 }
     - Pixel { orange, x=7, y=6 }
     - Pixel { orange, x=6, y=7 }

 The Pixel { red, x=2, y=3 } and Pixel { blue, x=6, y=6 } are in the same position, so only extra pixels on the screen. Orange can be interpreted as shape also.

 New pixels relative position from { red, x=2, y=3 } pixel :
     - Vector { x = -1, y = -1 } is Pixel { yellow, x=1, y=2 }
     - Vector { x = 1,  y = -1 } is Pixel { yellow, x=3, y=2 }
     - Vector { x = -1, y = 1  } is Pixel { yellow, x=1, y=4 }
     - Vector { x = 1,  y = 1  } is Pixel { yellow, x=3, y=4 }
     - Vector { x = 4, y = 2 }  is Pixel { orange, x=6, y=5 }
     - Vector { x = 3, y = 3 }  is Pixel { orange, x=5, y=6 }
     - Vector { x = 5, y = 3 }  is Pixel { orange, x=7, y=6 }
     - Vector { x = 4, y = 4 }  is Pixel { orange, x=6, y=7 }
     So these are the RelativePositionObservations
     RelativePositionObservation
     - fromObject: Pixel { red, x=2, y=3 }
     - relativeDistance: Vector { x = -1, y = 1  }
     - toObject: Pixel { yellow, x=1, y=4 }
 New pixels relative position from { blue, x=6, y=6 } pixel :
     - Vector { x = 0,  y = 1  } is Pixel { orange, x=6, y=7 }
     - Vector { x = -1, y = 0  } is Pixel { orange, x=5, y=6 }
     - Vector { x = 1,  y = 0  } is Pixel { orange, x=7, y=6 }
     - Vector { x = 0,  y = -1 } is Pixel { orange, x=6, y=5 }
     - Vector { x = -5, y = -4 } is Pixel { yellow, x=1, y=2 }
     - Vector { x = -5, y = -2 } is Pixel { yellow, x=1, y=4 }
     - Vector { x = -3, y = -4 } is Pixel { yellow, x=3, y=2 }
     - Vector { x = -3, y = -2 } is Pixel { yellow, x=3, y=4 }

Generalized true rules
 From Pixel { red }
     - Vector { x = -1, y = 1  } is Pixel { yellow }
     - Vector { x = -1, y = -1 } is Pixel { yellow }
     - Vector { x = 1,  y = 1  } is Pixel { yellow }
     - Vector { x = 1,  y = -1 } is Pixel { yellow }
From Pixel { blue }
     - Vector { x = 0,  y = 1  } is Pixel { orange }
     - Vector { x = -1, y = 0  } is Pixel { orange }
     - Vector { x = 1,  y = 0  } is Pixel { orange }
     - Vector { x = 0,  y = -1 } is Pixel { orange }

We should somehow conclude that
From a red Pixel to distance { x = -1, y = 1  } put a yellow Pixel
From a blue Pixel to distance { x = 0, y = 1  } put an orange Pixel

 So here is an object { described by these properties: color, x, y, blabla }
and here is an observation which state that the distance is descibed by a distance object { vector, x, y }
   there is an other object { described by these properties: color, x, y, blabla }

Can we generalize this observation to at least one property?
*/
