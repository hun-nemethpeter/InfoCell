#include "CellTestBase.h"

#include "Config.h"
#include "util/ArcTask.h"

#include <fstream>
#include <nlohmann/json.hpp>

using namespace synth;
using namespace synth::cells;
using nlohmann::json;
using synth::cells::test::CellTest;

class EdgeTester : public CellTest
{
public:
    enum class ScanLineState
    {
        Up,
        Middle,
        Down
    };

    EdgeTester() :
        ShaperStruct(getStruct("arc::Shaper")),
        ShapePointStruct(getStruct("arc::ShapePoint")),
        ShapePixelStruct(getStruct("arc::ShapePixel")),
        ShapeEdgeStruct(getStruct("arc::ShapeEdge")),
        ShapeEdgeJointStruct(getStruct("arc::ShapeEdgeJoint")),
        ShapeEdgeNodeStruct(getStruct("arc::ShapeEdgeNode")),
        ExternalEdgeEV(getVariable("arc::ShapeEdgeKind::ExternalEdge")),
        InternalEdgeEV(getVariable("arc::ShapeEdgeKind::InternalEdge")),
        DirectionUpEV(getVariable("arc::Directions::up")),
        DirectionDownEV(getVariable("arc::Directions::down")),
        DirectionLeftEV(getVariable("arc::Directions::left")),
        DirectionRightEV(getVariable("arc::Directions::right"))
    {
    }

    void testEdges(const std::string& jsonStr)
    {
        setInputGrid(jsonStr);
        testEdgesImpl();
    }

    void testEdges(hybrid::arc::Grid& inputHybridGrid)
    {
        setInputGrid(inputHybridGrid);
        testEdgesImpl();
    }

    void testEdgesImpl()
    {
        shaperProcess();
        sortShapePixelsAndCreateShapePoints();
        printEveryShapePixels();
        printAndTestShapePixels();
        sortShapePoints();
        printAllShapePoints();
        calculateEdgesForShapes();
        printShapeIdGrid();
        printEdges();
    }

    void setInputGrid(const std::string& jsonStr)
    {
        m_inputGrid       = std::make_unique<input::Grid>("inputGrid", jsonStr);
        m_inputHybridGrid = std::make_unique<cells::hybrid::arc::Grid>(kb, *m_inputGrid);
    }

    void setInputGrid(hybrid::arc::Grid& inputHybridGrid)
    {
        m_inputHybridGridPtr = &inputHybridGrid;
    }

    void shaperProcess()
    {
        m_shaper = std::make_unique<Object>(kb, ShaperStruct, kb.name("constructor"), Param { "grid", inputHybridGrid() });
        m_shaper->method("process");
    }

    CellI& shaper()
    {
        return *m_shaper;
    }

    cells::hybrid::arc::Grid& inputHybridGrid()
    {
        if (m_inputHybridGridPtr) {
            return *m_inputHybridGridPtr;
        } else if (m_inputHybridGrid) {
            return *m_inputHybridGrid;
        }
        throw "No inputHybridGrid!";
    }

    void sortShapePixelsAndCreateShapePoints()
    {
        // so after shaper::Process we have shapes and we have Shaper::shapePixels where every x, y coordinates a shepe is registered
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
        Object& shapePixels    = static_cast<Object&>(shaper()["shapePixels"]);
        CellI* previousUpPixel = nullptr;
        CellI* upPixel         = nullptr;
        CellI* leftPixel       = nullptr;
        CellI* firstShapePixel = nullptr;
        for (int y = 0; y < inputHybridGrid().height(); ++y) {
            Object& colX = static_cast<Object&>(shapePixels.method(kb.name("getValue"), { kb.ids.key, toCellNumber(y) }));
            for (int x = 0; x < inputHybridGrid().width(); ++x) {
                CellI& shapePixel = colX.method(kb.name("getValue"), { kb.ids.key, toCellNumber(x) });
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
                CellI* downRightPointPtr = new Object(kb, ShapePointStruct);

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
                    upRightPointPtr  = new Object(kb, ShapePointStruct);
                    downLeftPointPtr = &(*leftPixel)["downRightPoint"];

                    upRightPointCreated = true;

                    (*upLeftPointPtr).set("right", *upRightPointPtr);
                    (*upRightPointPtr).set("left", *upLeftPointPtr);

                    (*upRightPointPtr).set("x", kb.pools.numbers.get(static_cast<Number&>(pixel["x"]).value() + 1));
                    (*upRightPointPtr).set("y", pixel["y"]);
                } else if (!leftPixel && upPixel) {
                    upLeftPointPtr   = &(*upPixel)["downLeftPoint"];
                    upRightPointPtr  = &(*upPixel)["downRightPoint"];
                    downLeftPointPtr = new Object(kb, ShapePointStruct);

                    downLeftPointCreated = true;

                    (*upLeftPointPtr).set("down", *downLeftPointPtr);
                    (*downLeftPointPtr).set("up", *upLeftPointPtr);

                    (*downLeftPointPtr).set("x", pixel["x"]);
                    (*downLeftPointPtr).set("y", kb.pools.numbers.get(static_cast<Number&>(pixel["y"]).value() + 1));
                } else if (!leftPixel && !upPixel) {
                    upLeftPointPtr   = new Object(kb, ShapePointStruct);
                    upRightPointPtr  = new Object(kb, ShapePointStruct);
                    downLeftPointPtr = new Object(kb, ShapePointStruct);

                    upLeftPointCreated   = true;
                    upRightPointCreated  = true;
                    downLeftPointCreated = true;

                    (*upLeftPointPtr).set("right", *upRightPointPtr);
                    (*upRightPointPtr).set("left", *upLeftPointPtr);

                    (*upLeftPointPtr).set("down", *downLeftPointPtr);
                    (*downLeftPointPtr).set("up", *upLeftPointPtr);

                    (*upLeftPointPtr).set("x", pixel["x"]);
                    (*upLeftPointPtr).set("y", pixel["y"]);

                    (*upRightPointPtr).set("x", kb.pools.numbers.get(static_cast<Number&>(pixel["x"]).value() + 1));
                    (*upRightPointPtr).set("y", pixel["y"]);

                    (*downLeftPointPtr).set("x", pixel["x"]);
                    (*downLeftPointPtr).set("y", kb.pools.numbers.get(static_cast<Number&>(pixel["y"]).value() + 1));
                }
                (*upRightPointPtr).set("down", *downRightPointPtr);
                (*downRightPointPtr).set("up", *upRightPointPtr);

                (*downLeftPointPtr).set("right", *downRightPointPtr);
                (*downRightPointPtr).set("left", *downLeftPointPtr);

                (*downRightPointPtr).set("x", kb.pools.numbers.get(static_cast<Number&>(pixel["x"]).value() + 1));
                (*downRightPointPtr).set("y", kb.pools.numbers.get(static_cast<Number&>(pixel["y"]).value() + 1));

                (*upLeftPointPtr).set("downRightPixel", shapePixel);
                (*upRightPointPtr).set("downLeftPixel", shapePixel);
                (*downLeftPointPtr).set("upRightPixel", shapePixel);
                (*downRightPointPtr).set("upLeftPixel", shapePixel);

                shapePixel.set("upLeftPoint", *upLeftPointPtr);
                shapePixel.set("downLeftPoint", *downLeftPointPtr);
                shapePixel.set("upRightPoint", *upRightPointPtr);
                shapePixel.set("downRightPoint", *downRightPointPtr);

                if (currentShape.missing("shapePixels")) {
                    currentShape.set("shapePixels", *new List(kb, ShapePixelStruct));
                }
                List& shapePixelList = static_cast<List&>(currentShape["shapePixels"]);
                shapePixelList.add(shapePixel);

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

    void printEveryShapePixels()
    {
        Visitor::visitList(shaper()["shapes"], [this](CellI& currentShape, int, bool&) {
            std::cout << "Shape id:" << currentShape["id"].label() << ", pixels: ";
            Visitor::visitList(currentShape["shapePixels"], [this](CellI& shapePixel, int, bool&) {
                std::cout << fmt::format("[{}, {}]", shapePixel["pixel"]["x"].label(), shapePixel["pixel"]["y"].label());
            });
            std::cout << std::endl;
        });
        std::cout << std::endl;
    }

    void printAndTestShapePixels()
    {
        CellI* firstColumnPixelPtr  = firstShapePixelPtr();
        CellI* currentShapePixelPtr = firstShapePixelPtr();

        std::cout << "All shape pixel:" << std::endl;
        int referenceX = 0;
        int referenceY = 0;
        while (currentShapePixelPtr) {
            hybrid::arc::Pixel& currentArcPixel = static_cast<hybrid::arc::Pixel&>((*currentShapePixelPtr)["pixel"]);
            const int x                         = currentArcPixel.m_x.value();
            const int y                         = currentArcPixel.m_y.value();
            EXPECT_EQ(x, referenceX);
            EXPECT_EQ(y, referenceY);
            std::cout << "[" << x << ", " << y << "]";
            if (x == 0) {
                EXPECT_FALSE(currentShapePixelPtr->has("left"));
            } else {
                hybrid::arc::Pixel& otherArcPixel = static_cast<hybrid::arc::Pixel&>((*currentShapePixelPtr)["left"]["pixel"]);
                EXPECT_EQ(x - 1, otherArcPixel.m_x.value());
            }
            if (x == inputHybridGrid().width() - 1) {
                EXPECT_FALSE(currentShapePixelPtr->has("right"));
                std::cout << std::endl;
            } else {
                hybrid::arc::Pixel& otherArcPixel = static_cast<hybrid::arc::Pixel&>((*currentShapePixelPtr)["right"]["pixel"]);
                EXPECT_EQ(x + 1, otherArcPixel.m_x.value());
            }
            if (y == 0) {
                EXPECT_FALSE(currentShapePixelPtr->has("up"));
            } else {
                hybrid::arc::Pixel& otherArcPixel = static_cast<hybrid::arc::Pixel&>((*currentShapePixelPtr)["up"]["pixel"]);
                EXPECT_EQ(y - 1, otherArcPixel.m_y.value());
            }
            if (y == inputHybridGrid().height() - 1) {
                EXPECT_FALSE(currentShapePixelPtr->has("down"));
            } else {
                hybrid::arc::Pixel& otherArcPixel = static_cast<hybrid::arc::Pixel&>((*currentShapePixelPtr)["down"]["pixel"]);
                EXPECT_EQ(y + 1, otherArcPixel.m_y.value());
            }
            if (currentShapePixelPtr->has("right")) {
                currentShapePixelPtr = &(*currentShapePixelPtr)["right"];
                referenceX += 1;
            } else if (firstColumnPixelPtr->has("down")) {
                currentShapePixelPtr = &(*firstColumnPixelPtr)["down"];
                firstColumnPixelPtr  = currentShapePixelPtr;
                referenceX = 0;
                referenceY += 1;
            } else {
                currentShapePixelPtr = nullptr;
                std::cout << std::endl;
            }
        }

        Object& shapePixels = static_cast<Object&>(shaper()["shapePixels"]);
        for (int y = 0; y < inputHybridGrid().height(); ++y) {
            Object& colX = static_cast<Object&>(shapePixels.method(kb.name("getValue"), { kb.ids.key, toCellNumber(y) }));
            for (int x = 0; x < inputHybridGrid().width(); ++x) {
                CellI& shapePixel         = colX.method(kb.name("getValue"), { kb.ids.key, toCellNumber(x) });
                CellI& shape              = shapePixel["shape"];
                hybrid::arc::Pixel& pixel = static_cast<hybrid::arc::Pixel&>(shapePixel["pixel"]);
                EXPECT_EQ(x, pixel.m_x.value());
                EXPECT_EQ(y, pixel.m_y.value());
            }
        }
    }

    void sortShapePoints()
    {
        Visitor::visitList(shaper()["shapes"], [this](CellI& currentShape, int, bool&) {
            std::cout << "Shape id:" << currentShape["id"].label() << ", points:\n";

            ScanLineState scanLineState = ScanLineState::Up;

            List& shapePoints = *new List(kb, ShapePointStruct);
            currentShape.set("shapePoints", shapePoints);

            CellI* currentListItemPtr       = &currentShape["shapePixels"][kb.ids.first];
            CellI* currentMiddleRowListItem = nullptr;
            CellI* upMiddleRowListItem      = nullptr;
            CellI* downMiddleRowListItem    = nullptr;
            CellI* firstColumnPixelItem     = currentListItemPtr;
            CellI& firstShapePixel          = (*currentListItemPtr)[kb.ids.value];

            bool isUpperLine          = false;
            bool hasMoreUp            = false;
            bool hasMoreDown          = false;
            int upMiddleColumnIndex   = -1;
            int downMiddleColumnIndex = -1;
            int pointX                = -1;
            int pointY                = static_cast<Number&>(firstShapePixel["pixel"]["y"]).value();

            while (currentListItemPtr) {
                switch (scanLineState) {
                case ScanLineState::Up: {
                    CellI& currentListItem = *currentListItemPtr;
                    CellI& shapePixel      = currentListItem[kb.ids.value];
                    CellI& shapePoint      = shapePixel["upLeftPoint"];
                    int upLeftPointX       = static_cast<Number&>(shapePoint["x"]).value();

                    if (upLeftPointX > pointX) {
                        std::cout << fmt::format("({},{}) ", upLeftPointX, pointY);
                        shapePoints.add(shapePoint);
                    }
                    pointX = upLeftPointX + 1;
                    std::cout << fmt::format("({},{}) ", pointX, pointY);
                    shapePoints.add(shapePoint["right"]);
                    CellI* nextListItem          = currentListItem.has(kb.ids.next) ? &currentListItem[kb.ids.next] : nullptr;
                    bool isNextItemInTheSameLine = nextListItem ? &(*firstColumnPixelItem)["value"]["pixel"]["y"] == &(*nextListItem)["value"]["pixel"]["y"] : false;

                    if (nextListItem) {
                        if (isNextItemInTheSameLine) {
                            currentListItemPtr = nextListItem;
                        } else {
                            upMiddleRowListItem   = firstColumnPixelItem;
                            downMiddleRowListItem = nextListItem;
                            firstColumnPixelItem  = downMiddleRowListItem;
                            upMiddleColumnIndex   = static_cast<Number&>((*upMiddleRowListItem)["value"]["pixel"]["x"]).value();
                            downMiddleColumnIndex = static_cast<Number&>((*downMiddleRowListItem)["value"]["pixel"]["x"]).value();
                            isUpperLine           = upMiddleColumnIndex <= downMiddleColumnIndex;
                            currentListItemPtr    = isUpperLine ? upMiddleRowListItem : downMiddleRowListItem;
                            hasMoreUp             = true;
                            hasMoreDown           = true;
                            pointX                = -1;
                            ++pointY;
                            scanLineState = ScanLineState::Middle;
                            std::cout << " Up -> Middle" << std::endl;
                        }
                    } else {
                        currentListItemPtr = firstColumnPixelItem;
                        pointX             = -1;
                        ++pointY;
                        scanLineState = ScanLineState::Down;
                        std::cout << " Up -> Down" << std::endl;
                    }
                } break;
                case ScanLineState::Middle: {
                    CellI& currentListItem = *currentListItemPtr;
                    CellI& shapePixel      = currentListItem[kb.ids.value];
                    CellI& shapePoint      = shapePixel[isUpperLine ? "downLeftPoint" : "upLeftPoint"];
                    int currentPointX      = static_cast<Number&>(shapePoint["x"]).value();
                    int currentPointY      = static_cast<Number&>(shapePoint["y"]).value();
                    if (currentPointX > pointX) {
                        std::cout << fmt::format("({},{}) ", currentPointX, pointY);
                        shapePoints.add(shapePoint);
                    }
                    pointX = currentPointX + 1;
                    std::cout << fmt::format("({},{}) ", pointX, pointY);
                    shapePoints.add(shapePoint["right"]);

                    // stepping
                    CellI* nextUpListItem   = nullptr;
                    CellI* nextDownListItem = nullptr;
                    if (hasMoreUp && hasMoreDown) {
                        if (upMiddleColumnIndex < downMiddleColumnIndex) {
                            // step up line iter only
                            nextUpListItem   = &(*upMiddleRowListItem)[kb.ids.next];
                            nextDownListItem = downMiddleRowListItem;
                        } else if (upMiddleColumnIndex == downMiddleColumnIndex) {
                            // step up and down line iters
                            nextUpListItem   = &(*upMiddleRowListItem)[kb.ids.next];
                            nextDownListItem = (*downMiddleRowListItem).has(kb.ids.next) ? &(*downMiddleRowListItem)[kb.ids.next] : nullptr;
                        } else {
                            // step down line iter only
                            nextUpListItem   = upMiddleRowListItem;
                            nextDownListItem = (*downMiddleRowListItem).has(kb.ids.next) ? &(*downMiddleRowListItem)[kb.ids.next] : nullptr;
                        }
                    } else if (hasMoreUp && !hasMoreDown) {
                        // step up line iter only
                        nextUpListItem   = &(*upMiddleRowListItem)[kb.ids.next];
                        nextDownListItem = downMiddleRowListItem;
                    } else if (!hasMoreUp && hasMoreDown) {
                        // step down line iter only
                        nextUpListItem   = upMiddleRowListItem;
                        nextDownListItem = (*downMiddleRowListItem).has(kb.ids.next) ? &(*downMiddleRowListItem)[kb.ids.next] : nullptr;
                    }

                    hasMoreUp       = (nextUpListItem != firstColumnPixelItem);
                    hasMoreDown     = nextDownListItem ? static_cast<Number&>((*nextDownListItem)["value"]["pixel"]["y"]).value() == pointY : false;
                    bool isLastLine = !nextDownListItem;

                    if (isLastLine && !hasMoreUp && !hasMoreDown) {
                        upMiddleRowListItem   = nullptr;
                        downMiddleRowListItem = nullptr;
                        currentListItemPtr    = firstColumnPixelItem;
                        pointX                = -1;
                        ++pointY;
                        scanLineState = ScanLineState::Down;
                        isUpperLine   = false;
                        std::cout << " Middle -> Down" << std::endl;
                    } else if (!hasMoreUp && !hasMoreDown) {
                        pointX = -1;
                        ++pointY;
                        upMiddleRowListItem   = nextUpListItem;
                        downMiddleRowListItem = nextDownListItem;
                        firstColumnPixelItem  = nextDownListItem;
                        upMiddleColumnIndex   = static_cast<Number&>((*upMiddleRowListItem)["value"]["pixel"]["x"]).value();
                        downMiddleColumnIndex = static_cast<Number&>((*downMiddleRowListItem)["value"]["pixel"]["x"]).value();
                        isUpperLine           = upMiddleColumnIndex <= downMiddleColumnIndex;
                        currentListItemPtr    = isUpperLine ? upMiddleRowListItem : downMiddleRowListItem;
                        hasMoreUp             = true;
                        hasMoreDown           = true;
                        std::cout << " Middle -> Middle" << std::endl;
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
                            isUpperLine        = upMiddleColumnIndex <= downMiddleColumnIndex;
                            currentListItemPtr = isUpperLine ? upMiddleRowListItem : downMiddleRowListItem;
                        } else if (!hasMoreUp) {
                            currentListItemPtr = downMiddleRowListItem;
                            isUpperLine        = false;
                        } else {
                            currentListItemPtr = upMiddleRowListItem;
                            isUpperLine        = true;
                        }
                    }
                } break;
                case ScanLineState::Down: {
                    CellI& currentListItem = *currentListItemPtr;
                    CellI& shapePixel      = currentListItem[kb.ids.value];
                    CellI& shapePoint      = shapePixel["downLeftPoint"];
                    int downLeftPointX     = static_cast<Number&>(shapePoint["x"]).value();

                    if (downLeftPointX > pointX) {
                        std::cout << fmt::format("({},{}) ", downLeftPointX, pointY);
                        shapePoints.add(shapePoint);
                    }
                    pointX = downLeftPointX + 1;
                    std::cout << fmt::format("({},{}) ", pointX, pointY);
                    shapePoints.add(shapePoint["right"]);

                    currentListItemPtr = currentListItem.has(kb.ids.next) ? &currentListItem[kb.ids.next] : nullptr;
                } break;
                }
            }
            std::cout << std::endl;
        });
    }

    void printAllShapePoints()
    {
        std::cout << "\nPrinting shape points:" << std::endl;
        Visitor::visitList(shaper()["shapes"], [this](CellI& currentShape, int, bool&) {
            std::cout << "Shape id:" << currentShape["id"].label() << ", points:\n";

            Visitor::visitList(currentShape["shapePoints"], [this](CellI& shapePoint, int, bool&) {
                std::cout << fmt::format("({},{}) ", static_cast<Number&>(shapePoint["x"]).value(), static_cast<Number&>(shapePoint["y"]).value());
            });
            std::cout << std::endl;
        });
    }

    void calculateEdgesForShapes()
    {
        // calculate edges
        std::cout << "\ncalculate edges" << std::endl;
        Visitor::visitList(shaper()["shapes"], [this](CellI& currentShape, int, bool&) {
            std::cout << "Shape id:" << currentShape["id"].label() << ", points:\n";

            enum class ProcessingDirection
            {
                LeftToRight,
                UpToDown
            };

            ProcessingDirection processingDirection = ProcessingDirection::LeftToRight;
            CellI* currentListItemPtr               = &currentShape["shapePoints"][kb.ids.first];
            CellI* firstColumnPointItem             = currentListItemPtr;
            while (currentListItemPtr) {
                CellI& currentListItem = *currentListItemPtr;
                CellI& shapePoint      = currentListItem[kb.ids.value];
                int pointY             = static_cast<Number&>((*currentListItemPtr)["value"]["y"]).value();
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
 3 0100 Continue edge from up
 4 1100 Continue edge from left
 5 0010 Skip
 6 1010 Skip
 7 0110 Continue edge from right
 8 1110 New edgeJoint or continue from right
 9 0001 New edgeJoint
10 1001 Continue edge from up
11 0101 Skip
12 1101 Skip
13 0011 Continue edge from left
14 1011 Continue edge from up
15 0111 Skip
16 1111 Skip

Invalid   Skip     Continue  Continue Skip     Skip      New edge New edge New edge           Skip     Skip     Continue Continue  Skip     Skip
 1        2         3        4        5        6         7        8        9        10        11       12       13        14       15       16
 0🡬 0🡭   1🡬 0🡭   0🡬 1🡭   1🡬 1🡭   0🡬 0🡭   1🡬 0🡭   0🡬 1🡭   1🡬 1🡭   🡬 0🡭    1🡬 0🡭   0🡬 1🡭   1🡬 1🡭   0🡬 0🡭   1🡬 0🡭   0🡬 1🡭   1🡬 1🡭
 0🡯 0🡮   0🡯 0🡮   0🡯 0🡮   0🡯 0🡮   1🡯 0🡮   1🡯 0🡮   1🡯 0🡮   1🡯 0🡮   🡯 1🡮    0🡯 1🡮   0🡯 1🡮   0🡯 1🡮   1🡯 1🡮   1🡯 1🡮   1🡯 1🡮   1🡯 1🡮
 .--.--.  .--.--.  .--.--.   .--.--.  .--.--.  .--.--.   .--.--.  .--.--.  .--.--.  .--.--.   .--.--.  .--.--.  .--.--.  .--.--.   .--.--.  .--.--.
 |  |  |  |XX|  |  |  |XX|   |XX|XX|  |  |  |  |XX|  |   |  |XX|  |XX|XX|  |  |  |  |XX|  |   |  |XX|  |XX|XX|  |  |  |  |XX|  |   |  |XX|  |XX|XX|
 .--o->.  .--o->.  .--o->.   .--o->.  .--o->.  .--o->.   .--o->.  .--o->.  .--o->.  .--o->.   .--o->.  .--o->.  .--o->.  .--o->.   .--o->.  .--o->.
 |  |  |  |  |  |  |  |  |   |  |  |  |XX|  |  |XX|  |   |XX|  |  |XX|  |  |  |XX|  |  |XX|   |  |XX|  |  |XX|  |XX|XX|  |XX|XX|   |XX|XX|  |XX|XX|
 .--.--.  .--.--.  .--.--.   .--.--.  .--.--.  .--.--.   .--.--.  .--.--.  .--.--.  .--.--.   .--.--.  .--.--.  .--.--.  .--.--.   .--.--.  .--.--.

For upToDown direction edge from point middle
 1 0000 Invalid state, can not happen
 2 1000 Skip
 3 0100 Skip
 4 1100 Skip
 5 0010 Continue from left
 6 1010 Continue from up
 7 0110 Continue from right
 8 1110 Continue from right
 9 0001 Continue from right
10 1001 Continue from left
11 0101 Continue from up
12 1101 Continue from left
13 0011 Skip
14 1011 Skip
15 0111 Skip
16 1111 Skip

Invalid   Skip      Skip     Skip     Continue Continue  Continue Continue Continue  Continue  Continue Continue Skip      Skip     Skip     Skip
 1        2         3        4        5        6         7        8         9        10        11       12       13        14       15       16
 0🡬 0🡭   1🡬 0🡭   0🡬 1🡭   1🡬 1🡭   0🡬 0🡭   1🡬 0🡭   0🡬 1🡭   1🡬 1🡭    0🡬 0🡭   1🡬 0🡭   0🡬 1🡭   1🡬 1🡭   0🡬 0🡭   1🡬 0🡭   0🡬 1🡭   1🡬 1🡭
 0🡯 0🡮   0🡯 0🡮   0🡯 0🡮   0🡯 0🡮   1🡯 0🡮   1🡯 0🡮   1🡯 0🡮   1🡯 0🡮    0🡯 1🡮   0🡯 1🡮   0🡯 1🡮   0🡯 1🡮   1🡯 1🡮   1🡯 1🡮   1🡯 1🡮   1🡯 1🡮
 .--.--.  .--.--.  .--.--.   .--.--.  .--.--.  .--.--.   .--.--.  .--.--.   .--.--.  .--.--.   .--.--.  .--.--.  .--.--.  .--.--.   .--.--.  .--.--.
 |  |  |  |XX|  |  |  |XX|   |XX|XX|  |  |  |  |XX|  |   |  |XX|  |XX|XX|   |  |  |  |XX|  |   |  |XX|  |XX|XX|  |  |  |  |XX|  |   |  |XX|  |XX|XX|
 .--o--.  .--o--.  .--o--.   .--o--.  .--o--.  .--o--.   .--o--.  .--o--.   .--o--.  .--o--.   .--o--.  .--o--.  .--o--.  .--o--.   .--o--.  .--o--.
 |  |  |  |  |  |  |  |  |   |  |  |  |XX|  |  |XX|  |   |XX|  |  |XX|  |   |  |XX|  |  |XX|   |  |XX|  |  |XX|  |XX|XX|  |XX|XX|   |XX|XX|  |XX|XX|
 .--v--.  .--v--.  .--v--.   .--v--.  .--v--.  .--v--.   .--v--.  .--v--.   .--v--.  .--v--.   .--v--.  .--v--.  .--v--.  .--v--.   .--v--.  .--v--.
#endif
                bool hasUpLeft    = shapePoint.has("upLeftPixel") && (&shapePoint["upLeftPixel"]["shape"] == &currentShape);       // 🡬
                bool hasUpRight   = shapePoint.has("upRightPixel") && (&shapePoint["upRightPixel"]["shape"] == &currentShape);     // 🡭
                bool hasDownLeft  = shapePoint.has("downLeftPixel") && (&shapePoint["downLeftPixel"]["shape"] == &currentShape);   // 🡯
                bool hasDownRight = shapePoint.has("downRightPixel") && (&shapePoint["downRightPixel"]["shape"] == &currentShape); // 🡮
                int caseNum       = 1 + (int)hasUpLeft + ((int)hasUpRight * 2) + ((int)hasDownLeft * 4) + ((int)hasDownRight * 8);
                if (caseNum == 1) {
                    std::cout << "WTF";
                    throw "Invalid pixel state";
                } else {
                    std::cout << "";
                }

                CellI* toDirectionPtr  = nullptr;
                CellI* previousEdgeDir = nullptr;
                CellI* shapeDir        = nullptr;
                if (processingDirection == ProcessingDirection::LeftToRight) {
                    std::cout << std::format("[{:2}-]", caseNum);

                    // Skip      Skip     Skip     Skip     Skip     Skip     Skip
                    //  2        5        6        11       12       15       16
                    // 1🡬 0🡭    0🡬 0🡭   1🡬 0🡭  0🡬 1🡭   1🡬 1🡭   0🡬 1🡭   1🡬 1🡭
                    // 0🡯 0🡮    1🡯 0🡮   1🡯 0🡮  0🡯 1🡮   0🡯 1🡮   1🡯 1🡮   1🡯 1🡮
                    //  .--.--.  .--.--.  .--.--.  .--.--.  .--.--.  .--.--.  .--.--.
                    //  |XX|  |  |  |  |  |XX|  |  |  |XX|  |XX|XX|  |  |XX|  |XX|XX|
                    //  .--o->.  .--o->.  .--o->.  .--o->.  .--o->.  .--o->.  .--o->.
                    //  |  |  |  |XX|  |  |XX|  |  |  |XX|  |  |XX|  |XX|XX|  |XX|XX|
                    //  .--.--.  .--.--.  .--.--.  .--.--.  .--.--.  .--.--.  .--.--.
                    if ((hasUpLeft && !hasUpRight && !hasDownLeft && !hasDownRight) || (!hasUpLeft && !hasUpRight && hasDownLeft && !hasDownRight) || (hasUpLeft && !hasUpRight && hasDownLeft && !hasDownRight) || (!hasUpLeft && hasUpRight && !hasDownLeft && hasDownRight) || (hasUpLeft && hasUpRight && !hasDownLeft && hasDownRight) || (!hasUpLeft && hasUpRight && hasDownLeft && hasDownRight) || (hasUpLeft && hasUpRight && hasDownLeft && hasDownRight)) {
                        // Skip, do nothing
                        std::cout << "";
                    }

                    // Continue
                    // 3
                    // 0🡬 1🡭
                    // 0🡯 0🡮
                    // .--.--.
                    // |  |XX|
                    // .--o->.
                    // |  |  |
                    // .--.--.
                    if (!hasUpLeft && hasUpRight && !hasDownLeft && !hasDownRight) {
                        toDirectionPtr  = &DirectionRightEV;
                        previousEdgeDir = &DirectionUpEV;
                        shapeDir        = &DirectionLeftEV;
                    }

                    // Continue  Continue
                    // 10        14
                    // 1🡬 0🡭   1🡬 0🡭
                    // 0🡯 1🡮   1🡯 1🡮
                    // .--.--.   .--.--.
                    // |XX|  |   |XX|  |
                    // .--o-->   .--o->.
                    // |  |XX|   |XX|XX|
                    // .--.--.   .--.--.
                    if ((hasUpLeft && !hasUpRight && !hasDownLeft && hasDownRight) || (hasUpLeft && !hasUpRight && hasDownLeft && hasDownRight)) {
                        toDirectionPtr  = &DirectionRightEV;
                        previousEdgeDir = &DirectionUpEV;
                        shapeDir        = &DirectionRightEV;
                    }

                    // Continue
                    // 4
                    // 1🡬 1🡭
                    // 0🡯 0🡮
                    // .--.--.
                    // |XX|XX|
                    // .--o->.
                    // |  |  |
                    // .--.--.
                    if (hasUpLeft && hasUpRight && !hasDownLeft && !hasDownRight) {
                        toDirectionPtr  = &DirectionRightEV;
                        previousEdgeDir = &DirectionLeftEV;
                        shapeDir        = &DirectionLeftEV;
                    }

                    // Continue
                    // 13
                    // 0🡬 0🡭
                    // 1🡯 1🡮
                    // .--.--.
                    // |  |  |
                    // .--o->.
                    // |XX|XX|
                    // .--.--.
                    if (!hasUpLeft && !hasUpRight && hasDownLeft && hasDownRight) {
                        toDirectionPtr  = &DirectionRightEV;
                        previousEdgeDir = &DirectionLeftEV;
                        shapeDir        = &DirectionRightEV;
                    }

                    // New edge
                    // 7
                    // 0🡬 1🡭
                    // 1🡯 0🡮
                    // .--.--.
                    // |  |XX|
                    // .--o-->
                    // |XX|  |
                    // .--.--.
                    if (!hasUpLeft && hasUpRight && hasDownLeft && !hasDownRight) {
                        toDirectionPtr  = &DirectionRightEV;
                        previousEdgeDir = &DirectionLeftEV;
                        shapeDir        = &DirectionLeftEV;
                    }

                    // New edge New edge
                    // 8        9
                    // 1🡬 1🡭   0🡬 0🡭
                    // 1🡯 0🡮   0🡯 1🡮
                    // .--.--.  .--.--.
                    // |XX|XX|  |  |  |
                    // .--o->.  .--o->.
                    // |XX|  |  |  |XX|
                    // .--.--.  .--.--.
                    if ((hasUpLeft && hasUpRight && hasDownLeft && !hasDownRight) || (!hasUpLeft && !hasUpRight && !hasDownLeft && hasDownRight)) {
                        toDirectionPtr = &DirectionRightEV;
                        shapeDir       = &DirectionRightEV;
                    }
                } else {
                    std::cout << std::format("[{:2}|]", caseNum);

                    //  2 1000 Skip
                    //  3 0100 Skip
                    //  4 1100 Skip
                    // 13 0011 Skip
                    // 14 1011 Skip
                    // 15 0111 Skip
                    // 16 1111 Skip
                    //
                    // Skip     Skip      Skip     Skip      Skip     Skip     Skip
                    // 2        3         4        13        14       15       16
                    // 1🡬 0🡭   0🡬 1🡭   1🡬 1🡭   0🡬 0🡭   1🡬 0🡭   0🡬 1🡭   1🡬 1🡭
                    // 0🡯 0🡮   0🡯 0🡮   0🡯 0🡮   1🡯 1🡮   1🡯 1🡮   1🡯 1🡮   1🡯 1🡮
                    // .--.--.  .--.--.   .--.--.  .--.--.  .--.--.   .--.--.  .--.--.
                    // |XX|  |  |  |XX|   |XX|XX|  |  |  |  |XX|  |   |  |XX|  |XX|XX|
                    // .--o--.  .--o--.   .--o--.  .--o--.  .--o--.   .--o--.  .--o--.
                    // |  |  |  |  |  |   |  |  |  |XX|XX|  |XX|XX|   |XX|XX|  |XX|XX|
                    // .--v--.  .--v--.   .--v--.  .--v--.  .--v--.   .--v--.  .--v--.
                    if ((hasUpLeft && !hasUpRight && !hasDownLeft && !hasDownRight) || (!hasUpLeft && hasUpRight && !hasDownLeft && !hasDownRight) || (hasUpLeft && hasUpRight && !hasDownLeft && !hasDownRight) || (!hasUpLeft && !hasUpRight && hasDownLeft && hasDownRight) || (hasUpLeft && !hasUpRight && hasDownLeft && hasDownRight) || (!hasUpLeft && hasUpRight && hasDownLeft && hasDownRight) || (hasUpLeft && hasUpRight && hasDownLeft && hasDownRight)) {
                        // Skip, do nothing
                    }

                    // 6 1010 Continue from up
                    // 1🡬 0🡭
                    // 1🡯 0🡮
                    // .--.--.
                    // |XX|  |
                    // .--o--.
                    // |XX|  |
                    // .--v--.
                    if (hasUpLeft && !hasUpRight && hasDownLeft && !hasDownRight) {
                        toDirectionPtr  = &DirectionDownEV;
                        previousEdgeDir = &DirectionUpEV;
                        shapeDir        = &DirectionRightEV;
                    }

                    // 11 0101 Continue from up
                    //
                    // 0🡬 1🡭
                    // 0🡯 1🡮
                    // .--.--.
                    // |  |XX|
                    // .--o--.
                    // |  |XX|
                    // .--v--.
                    if (!hasUpLeft && hasUpRight && !hasDownLeft && hasDownRight) {
                        toDirectionPtr  = &DirectionDownEV;
                        previousEdgeDir = &DirectionUpEV;
                        shapeDir        = &DirectionLeftEV;
                    }

                    // 5 0010 Continue from left
                    //
                    // 0🡬 0🡭
                    // 1🡯 0🡮
                    // .--.--.
                    // |  |  |
                    // .--o--.
                    // |XX|  |
                    // .--v--.
                    if (!hasUpLeft && !hasUpRight && hasDownLeft && !hasDownRight) {
                        toDirectionPtr  = &DirectionDownEV;
                        previousEdgeDir = &DirectionLeftEV;
                        shapeDir        = &DirectionRightEV;
                    }

                    // 10 1001 Continue from left
                    // 12 1101 Continue from left
                    //
                    // 10       12
                    // 1🡬 0🡭  1🡬 1🡭
                    // 0🡯 1🡮  0🡯 1🡮
                    // .--.--.  .--.--.
                    // |XX|  |  |XX|XX|
                    // .--o--.  .--o--.
                    // |  |XX|  |  |XX|
                    // .--v--.  .--v--.
                    if ((hasUpLeft && !hasUpRight && !hasDownLeft && hasDownRight) || (hasUpLeft && hasUpRight && !hasDownLeft && hasDownRight)) {
                        toDirectionPtr  = &DirectionDownEV;
                        previousEdgeDir = &DirectionLeftEV;
                        shapeDir        = &DirectionLeftEV;
                    }

                    //  7 0110 Continue from right
                    //  8 1110 Continue from right
                    //
                    // 7        8
                    // 0🡬 1🡭   1🡬 1🡭
                    // 1🡯 0🡮   1🡯 0🡮
                    // .--.--.  .--.--.
                    // |  |XX|  |XX|XX|
                    // .--o--.  .--o--.
                    // |XX|  |  |XX|  |
                    // .--v--.  .--v--.
                    if ((!hasUpLeft && hasUpRight && hasDownLeft && !hasDownRight) || (hasUpLeft && hasUpRight && hasDownLeft && !hasDownRight)) {
                        toDirectionPtr  = &DirectionDownEV;
                        previousEdgeDir = &DirectionRightEV;
                        shapeDir        = &DirectionRightEV;
                    }

                    // 9 0001 Continue from right
                    //
                    // 0🡬 0🡭
                    // 0🡯 1🡮
                    // .--.--.
                    // |  |  |
                    // .--o--.
                    // |  |XX|
                    // .--v--.
                    if (!hasUpLeft && !hasUpRight && !hasDownLeft && hasDownRight) {
                        toDirectionPtr  = &DirectionDownEV;
                        previousEdgeDir = &DirectionRightEV;
                        shapeDir        = &DirectionLeftEV;
                    }
                }

                if (toDirectionPtr) {
                    CellI* toShapePointPtr = toDirectionPtr == &DirectionRightEV ? &shapePoint["right"] : &shapePoint["down"];
                    CellI& toShapePoint    = *toShapePointPtr;

                    // from joint
                    CellI* fromEdgeJointPtr = nullptr;
                    if (shapePoint.has("edgeJoint")) {
                        fromEdgeJointPtr = &shapePoint["edgeJoint"];
                    } else {
                        fromEdgeJointPtr = new Object(kb, ShapeEdgeJointStruct);
                        shapePoint.set("edgeJoint", *fromEdgeJointPtr);
                    }
                    CellI& fromEdgeJoint = *fromEdgeJointPtr;

                    // to joint
                    CellI* toEdgeJointPtr = nullptr;
                    if (toShapePoint.has("edgeJoint")) {
                        toEdgeJointPtr = &toShapePoint["edgeJoint"];
                    } else {
                        toEdgeJointPtr = new Object(kb, ShapeEdgeJointStruct);
                        toShapePoint.set("edgeJoint", *toEdgeJointPtr);
                    }
                    CellI& toEdgeJoint = *toEdgeJointPtr;

                    CellI* previousEdgePtr = nullptr;

                    const char* shapeDirStr = shapeDir == &DirectionRightEV ? "rightSide" : "leftSide";

                    if (!previousEdgeDir) {
                        std::cout << std::format("N");
                        CellI& newEdge  = *new Object(kb, ShapeEdgeStruct);
                        List& edgeNodes = *new List(kb, ShapeEdgeNodeStruct);
                        newEdge.set("shape", currentShape);
                        newEdge.set("edgeNodes", edgeNodes);
                        previousEdgePtr = &newEdge;
                        Map* edgesPtr  = nullptr;
                        if (currentShape.missing("edges")) {
                            Map& newEdges = *new Map(kb, kb.std.Number, ShapeEdgeStruct);
                            currentShape.set("edges", newEdges);
                            newEdge.set("id", _1_);
                            newEdge.set("kind", ExternalEdgeEV);
                            edgesPtr = &newEdges;
                        } else {
                            edgesPtr = &static_cast<Map&>(currentShape["edges"]);
                            CellI& newEdgeId = kb.pools.numbers.get(static_cast<Number&>(currentShape["lastEdgeId"]).value() + 1);
                            currentShape.set("lastEdgeId", newEdgeId);
                            newEdge.set("id", newEdgeId);
                            newEdge.set("kind", InternalEdgeEV);
                        }
#if 0
[ 9-]New(1)1(0,0) [13-]1(1,0) [13-]1(2,0) [13-]1(3,0) [13-]1(4,0) [ 5-] (5,0)
[ 9|]1(0,0) [13|] (1,0) [13|] (2,0) [13|] (3,0) [13|] (4,0) [ 5|]1(5,0)
[11-] (0,1) [ 8-]New(2)2(1,1) [12-] (2,1) [ 8-]New(3)3(3,1) [12-] (4,1) [ 6-] (5,1)
[11|]1(0,1) [ 8|]2(1,1) [12|]2(2,1) [ 8|]3(3,1) [12|]3(4,1) [ 6|]1(5,1)
[11-] (0,2) [14-]2(1,2) [ 7-]New(4)4Delete(4)(2,2) [10-]3(3,2) [15-] (4,2) [ 6-] (5,2)
[11|]1(0,2) [14|] (1,2) [ 7|]3(2,2) [10|]3(3,2) [15|] (4,2) [ 6|]1(5,2)
[11-] (0,3) [ 8-]New(4)4Delete(4)(1,3) [10-]3(2,3) [ 7-]New(4)4(3,3) [12-] (4,3) [ 6-] (5,3)
[11|]1(0,3) [ 8|]3(1,3) [10|]3(2,3) [ 7|]4(3,3) [12|]4(4,3) [ 6|]1(5,3)
[11-] (0,4) [14-]3(1,4) [15-] (2,4) [14-]4(3,4) [15-] (4,4) [ 6-] (5,4)
[11|]1(0,4) [14|] (1,4) [15|] (2,4) [14|] (3,4) [15|] (4,4) [ 6|]1(5,4)
[ 3-]1(0,5) [ 4-]1(1,5) [ 4-]1(2,5) [ 4-]1(3,5) [ 4-]1(4,5) [ 2-] (5,5) edges: 4

  2
  -
2| |2
  -
  2
#endif
                        Map& edges = *edgesPtr;
                        edges.add(newEdge["id"], newEdge);
                    } else {
                        std::cout << std::format("C");
                        CellI* previousEdgeNodePtr = nullptr;
                        if (previousEdgeDir == &DirectionUpEV) {
                            previousEdgeNodePtr = &fromEdgeJoint["up"];
                        } else if (previousEdgeDir == &DirectionLeftEV) {
                            previousEdgeNodePtr = &fromEdgeJoint["left"];
                        } else if (previousEdgeDir == &DirectionRightEV) {
                            previousEdgeNodePtr = &fromEdgeJoint["right"];
                        }
                        CellI* rightEdgeShapePtr = nullptr;
                        CellI& previousEdgeNode  = *previousEdgeNodePtr;
                        if (previousEdgeNode.has("rightSide")) {
                            rightEdgeShapePtr = &previousEdgeNode["rightSide"]["shape"];
                        }
                        if (rightEdgeShapePtr == &currentShape) {
                            previousEdgePtr = &previousEdgeNode["rightSide"];
                        } else {
                            previousEdgePtr = &previousEdgeNode["leftSide"];
                        }
                    }
                    CellI& previousEdge = *previousEdgePtr;
                    std::cout << previousEdge["id"].label();
                    List& edgesNodes = static_cast<List&>(previousEdge["edgeNodes"]);

                    CellI& newEdgeNode = *new Object(kb, ShapeEdgeNodeStruct);
                    newEdgeNode.set("from", shapePoint);
                    newEdgeNode.set("direction", *toDirectionPtr);
                    newEdgeNode.set(shapeDirStr, previousEdge);

                    edgesNodes.add(newEdgeNode);

                    // the join
                    fromEdgeJoint.set(toDirectionPtr == &DirectionRightEV ? "right" : "down", newEdgeNode);
                    const char* nextJointSlotName = toDirectionPtr == &DirectionRightEV ? "left" : "up";

                    if (toDirectionPtr == &DirectionRightEV && toEdgeJoint.has("up")) {
                        CellI& upEdgeNode     = toEdgeJoint["up"];
                        CellI* upLeftSidePtr  = nullptr;
                        CellI* upRightSidePtr = nullptr;
                        CellI* nextEdgePtr    = nullptr;
                        if (upEdgeNode.has("rightSide")) {
                            upRightSidePtr = &upEdgeNode["rightSide"];
                        }
                        if (upEdgeNode.has("leftSide")) {
                            upLeftSidePtr = &upEdgeNode["leftSide"];
                        }
                        if (upRightSidePtr && (&(*upRightSidePtr)["shape"] == &currentShape)) {
                            // possible join, check edge id
                            nextEdgePtr = &(*upRightSidePtr);
                        }
                        if (!nextEdgePtr && upLeftSidePtr && (&(*upLeftSidePtr)["shape"] == &currentShape)) {
                            // possible join, check edge id
                            nextEdgePtr = &(*upLeftSidePtr);
                        }

                        if (nextEdgePtr) {
                            CellI& nextEdge    = *nextEdgePtr;
                            int nextEdgeId     = static_cast<Number&>(nextEdge["id"]).value();
                            int previousEdgeId = static_cast<Number&>(previousEdge["id"]).value();
                            if (nextEdgeId != previousEdgeId) {
                                CellI* toDeleteEdgePtr = nullptr;
                                CellI* toExtendEdgePtr = nullptr;
                                if (nextEdgeId < previousEdgeId) {
                                    toDeleteEdgePtr = &previousEdge;
                                    toExtendEdgePtr = &nextEdge;
                                } else {
                                    toDeleteEdgePtr = &nextEdge;
                                    toExtendEdgePtr = &previousEdge;
                                }

                                CellI& toDeleteEdge = *toDeleteEdgePtr;
                                CellI& toExtendEdge = *toExtendEdgePtr;
                                std::cout << fmt::format("D{}C{}", toDeleteEdge["id"].label(), toExtendEdge["id"].label());
                                List& toDeleteEdgeNodes = static_cast<List&>(toDeleteEdge["edgeNodes"]);
                                Visitor::visitListInReverse(toDeleteEdgeNodes, [this, &toDeleteEdge, &toDeleteEdgeNodes, &toExtendEdge](CellI& toDeleteEdgeNode, int, bool&) {
                                    List& toExtendEdgeNodes = static_cast<List&>(toExtendEdge["edgeNodes"]);
                                    if (toDeleteEdgeNode.has("rightSide") && (&toDeleteEdgeNode["rightSide"] == &toDeleteEdge)) {
                                        toDeleteEdgeNode.set("rightSide", toExtendEdge);
                                    }
                                    if (toDeleteEdgeNode.has("leftSide") && (&toDeleteEdgeNode["leftSide"] == &toDeleteEdge)) {
                                        toDeleteEdgeNode.set("leftSide", toExtendEdge);
                                    }
                                    toExtendEdgeNodes.addFront(toDeleteEdgeNode);
                                    // toDeleteEdgeNodes.remove(&static_cast<List::Item&>(toDeleteEdgeNode)); TODO
                                });
                                CellI& toDeleteEdgeIdCell = toDeleteEdge["id"];
                                Map& currentShapeEdges    = static_cast<Map&>(currentShape["edges"]);
                                currentShapeEdges.remove(toDeleteEdgeIdCell);
                            } else {
                                std::cout << "    ";
                            }
                        } else {
                            std::cout << "    ";
                        }
                    } else {
                        std::cout << "    ";
                    }
                    toEdgeJoint.set(nextJointSlotName, newEdgeNode);
                } else {
                    std::cout << "      ";
                }
                std::cout << fmt::format("({},{}) ", static_cast<Number&>(shapePoint["x"]).value(), static_cast<Number&>(shapePoint["y"]).value());

                CellI* nextListItemPtr       = currentListItem.has(kb.ids.next) ? &currentListItem[kb.ids.next] : nullptr;
                int nextPointY               = nextListItemPtr ? static_cast<Number&>((*nextListItemPtr)["value"]["y"]).value() : -1;
                bool isNextItemInTheSameLine = nextPointY == pointY;

                if (isNextItemInTheSameLine) {
                    currentListItemPtr = nextListItemPtr;
                } else if (processingDirection == ProcessingDirection::LeftToRight) {
                    if (nextListItemPtr) {
                        currentListItemPtr  = firstColumnPointItem;
                        processingDirection = ProcessingDirection::UpToDown;
                        std::cout << std::endl;
                    } else {
                        currentListItemPtr = nextListItemPtr;
                    }
                } else {
                    currentListItemPtr   = nextListItemPtr;
                    firstColumnPointItem = nextListItemPtr;
                    processingDirection  = ProcessingDirection::LeftToRight;
                    std::cout << std::endl;
                }
            }
            std::cout << "edges: " << static_cast<Map&>(currentShape["edges"]).size() << std::endl;
        });
    }

    CellI* firstShapePixelPtr()
    {
        Object& shapePixels = static_cast<Object&>(shaper()["shapePixels"]);
        Object& colX = static_cast<Object&>(shapePixels.method(kb.name("getValue"), { kb.ids.key, _0_ }));
        CellI& shapePixel = colX.method(kb.name("getValue"), { kb.ids.key, _0_ });
        return &shapePixel;
    }

    void printShapeIdGrid()
    {
        ScanLineState scanLineState = ScanLineState::Up;
        CellI* firstColumnPixelPtr  = firstShapePixelPtr();
        CellI* currentShapePixelPtr = firstShapePixelPtr();

        while (currentShapePixelPtr) {
            CellI& currentShapePixel            = *currentShapePixelPtr;
            hybrid::arc::Pixel& currentArcPixel = static_cast<hybrid::arc::Pixel&>(currentShapePixel["pixel"]);
            const int x                         = currentArcPixel.m_x.value();
            const int y                         = currentArcPixel.m_y.value();

            CellI& currentShape = currentShapePixel["shape"];

            switch (scanLineState) {
            case ScanLineState::Up:
                if (currentShapePixel.has("up")) {
                    CellI& upShape = currentShapePixel["up"]["shape"];
                    if (&currentShape != &upShape) {
                        if (currentShapePixel.missing("right")) {
                            std::cout << "--";
                        } else if (currentShapePixel.missing("left")) {
                            std::cout << "+-";
                        } else {
                            std::cout << "--";
                        }
                    } else {
                        if (currentShapePixel.missing("right")) {
                            std::cout << "..";
                        } else if (currentShapePixel.missing("left")) {
                            std::cout << "|.";
                        } else {
                            if (currentShapePixel.has("left") && &currentShapePixel["left"]["shape"] != &currentShape) {
                                std::cout << "+.";
                            } else if (currentShapePixel.has("left") && currentShapePixel.has("up") && &currentShapePixel["left"]["up"]["shape"] != &currentShape) {
                                std::cout << "+.";
                            } else {
                                std::cout << "..";
                            }
                        }
                    }
                } else {
                    if (currentShapePixel.missing("right")) {
                        std::cout << "--";
                    } else if (currentShapePixel.missing("left")) {
                        std::cout << "+-";
                    } else {
                        std::cout << "--";
                    }
                }
                if (currentShapePixel.missing("right")) {
                    if (currentShapePixel.missing("up")) {
                        std::cout << "+";
                    } else {
                        std::cout << "|";
                    }
                }
                break;
            case ScanLineState::Middle:
                if (currentShapePixel.has("left")) {
                    CellI& leftShape = currentShapePixel["left"]["shape"];
                    if (&currentShape != &leftShape) {
                        std::cout << "|" << currentShape["id"].label();
                    } else {
                        std::cout << "." << currentShape["id"].label();
                    }
                } else {
                    std::cout << "|" << currentShape["id"].label();
                }
                if (currentShapePixel.missing("right")) {
                    std::cout << "|";
                }
                break;
            case ScanLineState::Down:
                if (currentShapePixel.missing("right")) {
                    std::cout << "--+";
                } else if (currentShapePixel.missing("left")) {
                    std::cout << "+-";
                } else {
                    std::cout << "--";
                }
                break;
            }

            if (currentShapePixelPtr->has("right")) {
                currentShapePixelPtr = &(*currentShapePixelPtr)["right"];
            } else if (scanLineState == ScanLineState::Up) {
                scanLineState        = ScanLineState::Middle;
                currentShapePixelPtr = firstColumnPixelPtr;
                std::cout << std::endl;
            } else if (scanLineState == ScanLineState::Middle) {
                if (currentShapePixel.has("down")) {
                    scanLineState        = ScanLineState::Up;
                    currentShapePixelPtr = &(*firstColumnPixelPtr)["down"];
                    firstColumnPixelPtr  = currentShapePixelPtr;
                } else {
                    scanLineState        = ScanLineState::Down;
                    currentShapePixelPtr = firstColumnPixelPtr;
                }
                std::cout << std::endl;
            } else if (scanLineState == ScanLineState::Down) {
                scanLineState        = ScanLineState::Up;
                currentShapePixelPtr = nullptr;
            }
        }
        std::cout << std::endl;
    }

    void printEdges()
    {
        ScanLineState scanLineState = ScanLineState::Up;
        CellI* firstColumnPixelPtr  = firstShapePixelPtr();
        CellI* currentShapePixelPtr = firstShapePixelPtr();

        while (currentShapePixelPtr) {
            CellI& currentShapePixel            = *currentShapePixelPtr;
            hybrid::arc::Pixel& currentArcPixel = static_cast<hybrid::arc::Pixel&>(currentShapePixel["pixel"]);
            const int x                         = currentArcPixel.m_x.value();
            const int y                         = currentArcPixel.m_y.value();

            CellI& currentShape       = currentShapePixel["shape"];

            switch (scanLineState) {
            case ScanLineState::Up:
                if (currentShapePixel.missing("up") || &currentShapePixel["up"]["shape"] != &currentShape) {
                    // add edge
                    std::cout << "--";
                } else {
                    std::cout << "  ";
                }
                break;
            case ScanLineState::Middle:
                if (currentShapePixel.missing("left") || &currentShapePixel["left"]["shape"] != &currentShape) {
                    // add edge
                    std::cout << "| ";
                } else {
                    std::cout << "  ";
                }
                break;
            case ScanLineState::Down:
                std::cout << "--";
                break;
            }
            if (currentShapePixel.missing("right")) {
                std::cout << "|";
            }

            if (currentShapePixelPtr->has("right")) {
                currentShapePixelPtr = &(*currentShapePixelPtr)["right"];
            } else if (scanLineState == ScanLineState::Up) {
                scanLineState        = ScanLineState::Middle;
                currentShapePixelPtr = firstColumnPixelPtr;
                std::cout << std::endl;
            } else if (scanLineState == ScanLineState::Middle) {
                if (currentShapePixel.has("down")) {
                    scanLineState        = ScanLineState::Up;
                    currentShapePixelPtr = &(*firstColumnPixelPtr)["down"];
                    firstColumnPixelPtr  = currentShapePixelPtr;
                } else {
                    scanLineState        = ScanLineState::Down;
                    currentShapePixelPtr = firstColumnPixelPtr;
                }
                std::cout << std::endl;
            } else if (scanLineState == ScanLineState::Down) {
                scanLineState        = ScanLineState::Up;
                currentShapePixelPtr = nullptr;
            }
        }
        std::cout << std::endl;
    }

    void expectedShapeIds(const std::string& shapeIds)
    {
        int x = 0;
        int y = 0;
        Object& shapePixels = static_cast<Object&>(shaper()["shapePixels"]);
        for (char id : shapeIds) {
            switch (id) {
            case '\n': {
                x = 0;
                y += 1;
            } break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9': {
                CellI& currentId = kb.pools.numbers.get(id - '0');
                Object& colX      = static_cast<Object&>(shapePixels.method(kb.name("getValue"), { kb.ids.key, kb.pools.numbers.get(y) }));
                CellI& shapePixel = colX.method(kb.name("getValue"), { kb.ids.key, kb.pools.numbers.get(x) });
                EXPECT_EQ(&shapePixel["shape"]["id"], &currentId);
                x += 1;
            }
            }
        }
    }

    void expectedShapesCount(int expectedCount)
    {
        int shapeCount = static_cast<Number&>(shaper()["shapes"]["size"]).value();
        EXPECT_EQ(shapeCount, expectedCount);
    }

    void expectedShapeEdgeCounts(const std::map<int, int>& shapeEdgeCounts)
    {
        for (const auto& edgeCountPair : shapeEdgeCounts) {
            int shapeId = edgeCountPair.first;
            int edgeCount = edgeCountPair.second;
            CellI& shape  = static_cast<Object&>(shaper()["shapeMap"]).method(kb.name("getValue"), { kb.ids.key, kb.pools.numbers.get(shapeId) });
            Map& shapeEdges = static_cast<Map&>(shape["edges"]);
            EXPECT_EQ(edgeCount, shapeEdges.size());
        }
    }

    CellI& ShaperStruct;
    CellI& ShapePointStruct;
    CellI& ShapePixelStruct;
    CellI& ShapeEdgeStruct;
    CellI& ShapeEdgeJointStruct;
    CellI& ShapeEdgeNodeStruct;
    CellI& ExternalEdgeEV;
    CellI& InternalEdgeEV;
    CellI& DirectionUpEV;
    CellI& DirectionDownEV;
    CellI& DirectionLeftEV;
    CellI& DirectionRightEV;
    std::unique_ptr<input::Grid> m_inputGrid;
    std::unique_ptr<cells::hybrid::arc::Grid> m_inputHybridGrid;
    cells::hybrid::arc::Grid* m_inputHybridGridPtr = nullptr;
    std::unique_ptr<Object> m_shaper;
};

TEST_F(EdgeTester, EdgeTest)
{
    testEdges(R"([[0, 0, 0, 0, 0],
                  [0, 4, 0, 4, 0],
                  [0, 0, 2, 0, 0],
                  [0, 4, 0, 4, 0],
                  [0, 0, 0, 0, 0]])");
    expectedShapeIds(R"(11111
                        12131
                        11411
                        15161
                        11111)");
    expectedShapeEdgeCounts({ { 1, 2 }, { 2, 1 }, { 3, 1 }, { 4, 1 }, { 5, 1 }, { 6, 1 } });
}

TEST_F(EdgeTester, EdgeTestTwoLeftCorners)
{
    testEdges(R"([[0, 7, 7, 7],
                  [7, 7, 7, 7],
                  [0, 7, 7, 7]])");

    expectedShapeIds(R"(1222
                        2222
                        3222)");
    expectedShapesCount(3);
    expectedShapeEdgeCounts({ { 1, 1 }, { 2, 1 }, { 3, 1 } });
}

TEST_F(EdgeTester, EdgeTestWithFourCorners)
{
    testEdges(R"([[0, 7, 7, 0],
                  [7, 7, 7, 7],
                  [0, 7, 7, 0]])");

    expectedShapeIds(R"(1223
                        2222
                        4225)");
    expectedShapesCount(5);
    expectedShapeEdgeCounts({ { 1, 1 }, { 2, 1 }, { 3, 1 }, { 4, 1 }, { 5, 1 } });
}

TEST_F(EdgeTester, EdgeTestWithLineDiagonalFromUpLeft)
{
    testEdges(R"([[7, 0, 0, 0, 0],
                  [0, 7, 0, 0, 0],
                  [0, 0, 7, 0, 0],
                  [0, 0, 0, 7, 0],
                  [0, 0, 0, 0, 7]])");

    expectedShapeIds(R"(12222
                        21222
                        22122
                        22212
                        22221)");
    expectedShapesCount(2);
    expectedShapeEdgeCounts({ { 1, 1 }, { 2, 1 } });
}

TEST_F(EdgeTester, EdgeTestWithLineDiagonalFromUpRight)
{
    testEdges(R"([[0, 0, 0, 0, 7],
                  [0, 0, 0, 7, 0],
                  [0, 0, 7, 0, 0],
                  [0, 7, 0, 0, 0],
                  [7, 0, 0, 0, 0]])");

    expectedShapeIds(R"(11112
                        11121
                        11211
                        12111
                        21111)");
    expectedShapesCount(2);
    expectedShapeEdgeCounts({ { 1, 1 }, { 2, 1 } });
}

TEST_F(EdgeTester, EdgeTestWithArc_0ca9ddb6_Train1Input)
{
    testEdges(R"([[0,0,0,0,0,0,0,0,0],
                  [0,0,0,0,0,0,0,0,0],
                  [0,0,0,0,0,0,0,0,0],
                  [0,0,2,0,0,0,0,0,0],
                  [0,0,0,0,0,0,0,0,0],
                  [0,0,0,0,0,0,0,0,0],
                  [0,0,0,0,0,0,1,0,0],
                  [0,0,0,0,0,0,0,0,0],
                  [0,0,0,0,0,0,0,0,0]])");

    expectedShapeIds(R"(111111111
                        111111111
                        111111111
                        112111111
                        111111111
                        111111111
                        111111311
                        111111111)");
    expectedShapesCount(3);
    expectedShapeEdgeCounts({ { 1, 3 }, { 2, 1 }, { 3, 1 } });
}

TEST_F(EdgeTester, EdgeTestWithArc_0ca9ddb6_Train1Output)
{
    testEdges(R"([[0, 0, 0, 0, 0, 0, 0, 0, 0],
                  [0, 0, 0, 0, 0, 0, 0, 0, 0],
                  [0, 4, 0, 4, 0, 0, 0, 0, 0],
                  [0, 0, 2, 0, 0, 0, 0, 0, 0],
                  [0, 4, 0, 4, 0, 0, 0, 0, 0],
                  [0, 0, 0, 0, 0, 0, 7, 0, 0],
                  [0, 0, 0, 0, 0, 7, 1, 7, 0],
                  [0, 0, 0, 0, 0, 0, 7, 0, 0],
                  [0, 0, 0, 0, 0, 0, 0, 0, 0]])");

    expectedShapeIds(R"(111111111
                        111111111
                        121311111
                        114111111
                        151611111
                        111111711
                        111117871
                        111111711)");
    expectedShapesCount(8);
    expectedShapeEdgeCounts({ { 1, 3 }, { 2, 1 }, { 3, 1 }, { 4, 1 }, { 5, 1 }, { 6, 1 }, { 7, 1 }, { 8, 1 } });
}

TEST_F(EdgeTester, EdgeTestWithArc_4be741c5_Train3Input)
{
    testEdges(R"([[6, 6, 6, 6, 6, 6, 6, 6, 6],
                  [6, 6, 4, 4, 6, 6, 6, 6, 6],
                  [6, 4, 4, 4, 6, 4, 6, 4, 4],
                  [4, 4, 4, 4, 4, 4, 4, 4, 4],
                  [4, 4, 4, 4, 4, 4, 4, 4, 4],
                  [4, 4, 4, 4, 4, 4, 4, 4, 4],
                  [4, 2, 2, 4, 4, 4, 2, 2, 4],
                  [2, 2, 2, 2, 2, 2, 2, 2, 2],
                  [2, 3, 2, 2, 2, 2, 2, 3, 3],
                  [3, 3, 3, 3, 3, 3, 3, 3, 3],
                  [3, 3, 3, 3, 3, 3, 3, 3, 3]])");
}

TEST_F(EdgeTester, EdgeTestWithArc_4be741c5_Train3Output)
{
    testEdges(R"([[6],
                  [4],
                  [2],
                  [3]])");
}

TEST_F(EdgeTester, EdgeTestWithAllArcTask)
{
    static const std::string arcFilePath = SYNTH_ARCPRIZE_PATH SYNTH_ARC_PRIZE_TRAINING_CHALLENGES_FILENAME;
    TaskSet taskSet(kb, SYNTH_ARCPRIZE_PATH SYNTH_ARC_PRIZE_TRAINING_CHALLENGES_FILENAME);
    for (auto& task : taskSet.m_tasks) {
        std::cout << "id: " << task.first << ", examples num:" << static_cast<List&>(task.second.m_cellExamplesList).size() << ", tests num:" << static_cast<List&>(task.second.m_cellTestsList).size() << std::endl;
        std::cout << "   examples:" << std::endl;
        Visitor::visitList(task.second.m_cellExamplesList, [this](CellI& example, int i, bool&) {
            std::cout << "    size " << static_cast<hybrid::arc::Grid&>(example["input"]).width() << "x" << static_cast<hybrid::arc::Grid&>(example["input"]).height() << " -> " << static_cast<hybrid::arc::Grid&>(example["output"]).width() << "x" << static_cast<hybrid::arc::Grid&>(example["output"]).height() << std::endl;
            testEdges(static_cast<hybrid::arc::Grid&>(example["input"]));
            testEdges(static_cast<hybrid::arc::Grid&>(example["output"]));

        });
        std::cout << "   tests:" << std::endl;
        Visitor::visitList(task.second.m_cellTestsList, [](CellI& example, int i, bool&) {
            std::cout << "    size " << static_cast<hybrid::arc::Grid&>(example["input"]).width() << "x" << static_cast<hybrid::arc::Grid&>(example["input"]).height() << std::endl;
        });
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    std::cout << "Constructed: " << CellI::s_constructed << ", destructed: " << CellI::s_destructed << ", live: " << CellI::s_constructed - CellI::s_destructed << std::endl;

    return ret;
}