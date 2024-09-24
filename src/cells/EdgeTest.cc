#include "CellTestBase.h"

using namespace synth;
using namespace synth::cells;

using synth::cells::test::CellTest;

class EdgeLine;
class Shape
{
public:
    Shape(CellI& id) :
        id(id) { }
    void addEdge(const EdgeLine& edge);
    CellI& id;
    std::map<int, std::list<EdgeLine>> edges;
    std::map<int, std::map<int, EdgeLine*>> edgeIndex;
};
enum class Direction
{
    Horizontal,
    Vertical
};
class EdgeLine
{
public:
    EdgeLine(int x, int y, Direction direction) :
        x(x), y(x), direction(direction)
    {
    }
    int x, y;
    Direction direction;
};

void Shape::addEdge(const EdgeLine& edge)
{
    int endX  = edge.x;
    int endY  = edge.y;
    if (edge.direction == Direction::Horizontal) {
        endX += 1;
    } else {
        endY += 1;
    }
    EdgeLine* previousStartEdge = nullptr;
    EdgeLine* previousEndEdge = nullptr;

    auto colX = edgeIndex.find(edge.x);
    if (colX != edgeIndex.end()) {
        auto edgeIt = colX->second.find(edge.y);
        if (edgeIt != colX->second.end()) {
            previousStartEdge = edgeIt->second;
        }
    }
    colX = edgeIndex.find(endX);
    if (colX != edgeIndex.end()) {
        auto edgeIt = colX->second.find(endY);
        if (edgeIt != colX->second.end()) {
            previousEndEdge = edgeIt->second;
        }
    }
    if (previousStartEdge && previousEndEdge) {

    }
}

class ShapePoint
{
public:
    Number& x;
    Number& y;
    ShapePoint* up;
    ShapePoint* down;
    ShapePoint* left;
    ShapePoint* right;
};

TEST_F(CellTest, EdgeTest)
{
    auto& ShaperStruct = getStruct("arc::Shaper");

    const auto& printPixels = [this](CellI& pixelList) -> std::string {
        std::stringstream ss;
        Visitor::visitList(pixelList, [this, &ss](CellI& arcPixel, int, bool&) {
            ss << fmt::format("[{}, {}]", arcPixel["x"].label(), arcPixel["y"].label());
        });

        return ss.str();
    };

    // 0 7 7 7
    // 7 7 7 7
    // 0 7 7 7
    input::Grid inputGrid1("inputGrid1", "[[0, 7, 7, 7], [7, 7, 7, 7], [0, 7, 7, 7]]");
    cells::hybrid::arc::Grid grid1(kb, inputGrid1);
//    printGrid(grid1);
    Object shaper(kb, ShaperStruct, kb.name("constructor"), { "grid", grid1 });
    shaper.method("process");
    printAs.value(shaper["shapes"]["size"], "shaper[shapes][size]");
    EXPECT_EQ(&shaper["shapes"]["size"], &_3_);
    EXPECT_EQ(&shaper["shapes"]["first"]["value"]["pixels"]["size"], &_1_);
    CellI& shape1 = static_cast<Object&>(shaper["shapes"]["first"]["value"]);
    CellI& shape2 = static_cast<Object&>(shaper["shapes"]["first"]["next"]["value"]);
    CellI& shape3 = static_cast<Object&>(shaper["shapes"]["first"]["next"]["next"]["value"]);
    printAs.value(shape2["pixels"]["size"], R"("shape2["pixels"]["size"])");
    EXPECT_EQ(&shape2["pixels"]["size"], &toCellNumber(10));
    EXPECT_EQ(&toCellNumber(inputGrid1.width()), &shaper["width"]);
    EXPECT_EQ(&toCellNumber(inputGrid1.height()), &shaper["height"]);
    Object& shapePixels = static_cast<Object&>(shaper["shapePixels"]);

    auto& ShapePointStruct     = getStruct("arc::ShapePoint");
    auto& ShapePixelStruct     = getStruct("arc::ShapePixel");
    auto& ShapeEdgeStruct      = getStruct("arc::ShapeEdge");
    auto& ShapeEdgeJointStruct = getStruct("arc::ShapeEdgeJoint");
    auto& ShapeEdgeNodeStruct  = getStruct("arc::ShapeEdgeNode");
    auto& ExternalEdgeEV       = getVariable("arc::ShapeEdgeKind::ExternalEdge");
    auto& InternalEdgeEV       = getVariable("arc::ShapeEdgeKind::InternalEdge");
    auto& DirectionUpEV        = getVariable("arc::Directions::up");
    auto& DirectionDownEV      = getVariable("arc::Directions::down");
    auto& DirectionLeftEV      = getVariable("arc::Directions::left");
    auto& DirectionRightEV     = getVariable("arc::Directions::right");

    CellI* previousUpPixel = nullptr;
    CellI* upPixel         = nullptr;
    CellI* leftPixel       = nullptr;
    CellI* firstShapePixel = nullptr;
    for (int y = 0; y < inputGrid1.height(); ++y) {
        Object& colX = static_cast<Object&>(shapePixels.method(kb.name("getValue"), { kb.ids.key, toCellNumber(y) }));
        for (int x = 0; x < inputGrid1.width(); ++x) {
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
            if (pixel.missing("left")) {
                // first column
                previousUpPixel = &shapePixel;
            }
        }
    }
    CellI* firstColumnPixelPtr = firstShapePixel;
    CellI* currentShapePixelPtr = firstShapePixel;

    Visitor::visitList(shaper["shapes"], [this](CellI& currentShape, int, bool&) {
        std::cout << "Shape id:" << currentShape["id"].label() << ", pixels: ";
        Visitor::visitList(currentShape["shapePixels"], [this](CellI& shapePixel, int, bool&) {
            std::cout << fmt::format("[{}, {}]", shapePixel["pixel"]["x"].label(), shapePixel["pixel"]["y"].label());
        });
        std::cout << std::endl;
    });
    std::cout << std::endl;
    enum class ScanLineState
    {
        Up,
        Middle,
        Down
    };

    Visitor::visitList(shaper["shapes"], [this, &ShapePointStruct](CellI& currentShape, int, bool&) {
        std::cout << "Shape id:" << currentShape["id"].label() << ", points:\n";

        ScanLineState scanLineState = ScanLineState::Up;

        List& shapePoints = *new List(kb, ShapePointStruct);
        currentShape.set("shapePoints", shapePoints);

        CellI* currentListItemPtr    = &currentShape["shapePixels"][kb.ids.first];
        CellI* upMiddleRowListItem   = nullptr;
        CellI* downMiddleRowListItem = nullptr;
        CellI* firstColumnPixelItem  = currentListItemPtr;
        CellI& firstShapePixel       = (*currentListItemPtr)[kb.ids.value];

        int upMiddleColumnIndex   = -1;
        int downMiddleColumnIndex = -1;
        int pointX                = -1;
        int pointY                = static_cast<Number&>(firstShapePixel["pixel"]["y"]).value();

        while (currentListItemPtr || upMiddleRowListItem || downMiddleRowListItem) {
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
                        pointX                = -1;
                        ++pointY;
                        scanLineState         = ScanLineState::Middle;
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
                bool isUpperLine       = upMiddleColumnIndex <= downMiddleColumnIndex;
                CellI& currentListItem = isUpperLine ? *upMiddleRowListItem : *downMiddleRowListItem;
                CellI& shapePixel      = currentListItem[kb.ids.value];
                CellI& shapePoint      = shapePixel[isUpperLine ? "downLeftPoint" : "upLeftPoint"];
                int currentPointX      = static_cast<Number&>(shapePoint["x"]).value();
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

                bool hasMoreUp   = (nextUpListItem != firstColumnPixelItem);
                bool hasMoreDown = nextDownListItem ? static_cast<Number&>((*nextDownListItem)["value"]["pixel"]["y"]).value() == pointY : false;
                bool isLastLine  = !nextDownListItem;

                if (isLastLine) {
                    upMiddleRowListItem   = nullptr;
                    downMiddleRowListItem = nullptr;
                    currentListItemPtr    = firstColumnPixelItem;
                    pointX                = -1;
                    ++pointY;
                    scanLineState = ScanLineState::Down;
                    std::cout << " Middle -> Down" << std::endl;
                } else if (!hasMoreUp && !hasMoreDown) {
                    pointX = -1;
                    ++pointY;
                    upMiddleRowListItem   = nextUpListItem;
                    downMiddleRowListItem = nextDownListItem;
                    firstColumnPixelItem  = nextDownListItem;
                    upMiddleColumnIndex   = static_cast<Number&>((*upMiddleRowListItem)["value"]["pixel"]["x"]).value();
                    downMiddleColumnIndex = static_cast<Number&>((*downMiddleRowListItem)["value"]["pixel"]["x"]).value();
                    std::cout << " Middle -> Middle" << std::endl;
                } else {
                    if (hasMoreUp) {
                        upMiddleRowListItem = nextUpListItem;
                        upMiddleColumnIndex = static_cast<Number&>((*upMiddleRowListItem)["value"]["pixel"]["x"]).value();
                    }
                    if (hasMoreDown) {
                        downMiddleRowListItem = nextDownListItem;
                        downMiddleColumnIndex = static_cast<Number&>((*downMiddleRowListItem)["value"]["pixel"]["x"]).value();
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

    Visitor::visitList(shaper["shapes"], [this](CellI& currentShape, int, bool&) {
        std::cout << "Shape id:" << currentShape["id"].label() << ", points:\n";

        Visitor::visitList(currentShape["shapePoints"], [this](CellI& shapePoint, int, bool&) {
            std::cout << fmt::format("({},{}) ", static_cast<Number&>(shapePoint["x"]).value(), static_cast<Number&>(shapePoint["y"]).value());
        });
        std::cout << std::endl;
    });

    // calculate edges
    std::cout << "\ncalculate edges" << std::endl;
    Visitor::visitList(shaper["shapes"], [this, &ShapeEdgeStruct, &ShapeEdgeJointStruct, &ShapeEdgeNodeStruct, &ExternalEdgeEV, &InternalEdgeEV, &DirectionUpEV, &DirectionDownEV, &DirectionLeftEV, &DirectionRightEV](CellI& currentShape, int, bool&) {
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
 7 0110
 8 1110 New edgeJoint or continue from right
 9 0001 New edgeJoint
10 1001
11 0101 Skip
12 1101 Skip
13 0011 Continue edge from left
14 1011 Continue edge from up
15 0111 Skip
16 1111 Skip

Invalid   Skip     Continue  Continue Skip     Skip              New edge*  New edge           Skip     Skip     Continue Continue  Skip     Skip
 1        2         3        4        5        6         7        8         9        10        11       12       13        14       15       16
 0🡬 0🡭   1🡬 0🡭   0🡬 1🡭   1🡬 1🡭   0🡬 0🡭   1🡬 0🡭   0🡬 1🡭   1🡬 1🡭    0🡬 0🡭   1🡬 0🡭   0🡬 1🡭   1🡬 1🡭   0🡬 0🡭   1🡬 0🡭   0🡬 1🡭   1🡬 1🡭
 0🡯 0🡮   0🡯 0🡮   0🡯 0🡮   0🡯 0🡮   1🡯 0🡮   1🡯 0🡮   1🡯 0🡮   1🡯 0🡮    0🡯 1🡮   0🡯 1🡮   0🡯 1🡮   0🡯 1🡮   1🡯 1🡮   1🡯 1🡮   1🡯 1🡮   1🡯 1🡮
 .--.--.  .--.--.  .--.--.   .--.--.  .--.--.  .--.--.   .--.--.  .--.--.   .--.--.  .--.--.   .--.--.  .--.--.  .--.--.  .--.--.   .--.--.  .--.--.
 |  |  |  |XX|  |  |  |XX|   |XX|XX|  |  |  |  |XX|  |   |  |XX|  |XX|XX|   |  |  |  |XX|  |   |  |XX|  |XX|XX|  |  |  |  |XX|  |   |  |XX|  |XX|XX|
 .--o->.  .--o->.  .--o->.   .--o->.  .--o->.  .--o->.   .--o->.  .--o->.   .--o->.  .--o->.   .--o->.  .--o->.  .--o->.  .--o->.   .--o->.  .--o->.
 |  |  |  |  |  |  |  |  |   |  |  |  |XX|  |  |XX|  |   |XX|  |  |XX|  |   |  |XX|  |  |XX|   |  |XX|  |  |XX|  |XX|XX|  |XX|XX|   |XX|XX|  |XX|XX|
 .--.--.  .--.--.  .--.--.   .--.--.  .--.--.  .--.--.   .--.--.  .--.--.   .--.--.  .--.--.   .--.--.  .--.--.  .--.--.  .--.--.   .--.--.  .--.--.

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
            } else {
                std::cout << "";
            }

            CellI* toDirectionPtr   = nullptr;
            CellI* previousEdgeDir  = nullptr;
            if (processingDirection == ProcessingDirection::LeftToRight) {
                std::cout << "[" << caseNum << "-]";

                // Skip      Skip     Skip     Skip     Skip     Skip     Skip
                //  2        5        6        11       12       15       16
                // 1🡬 0🡭    0🡬 0🡭   1🡬 0🡭  0🡬 1🡭   1🡬 1🡭   0🡬 1🡭   1🡬 1🡭
                // 0🡯 0🡮    1🡯 0🡮   1🡯 0🡮  0🡯 1🡮   0🡯 1🡮   1🡯 1🡮   1🡯 1🡮
                //  .--.--.  .--.--.  .--.--.  .--.--.  .--.--.  .--.--.  .--.--.
                //  |XX|  |  |  |  |  |XX|  |  |  |XX|  |XX|XX|  |  |XX|  |XX|XX|
                //  .--o->.  .--o->.  .--o->.  .--o->.  .--o->.  .--o->.  .--o->.
                //  |  |  |  |XX|  |  |XX|  |  |  |XX|  |  |XX|  |XX|XX|  |XX|XX|
                //  .--.--.  .--.--.  .--.--.  .--.--.  .--.--.  .--.--.  .--.--.
                if ((hasUpLeft && !hasUpRight && !hasDownLeft && !hasDownRight) ||
                    (!hasUpLeft && !hasUpRight && hasDownLeft && !hasDownRight) ||
                    (hasUpLeft && !hasUpRight && hasDownLeft && !hasDownRight) ||
                    (!hasUpLeft && hasUpRight && !hasDownLeft && hasDownRight) ||
                    (hasUpLeft && hasUpRight && !hasDownLeft && hasDownRight) ||
                    (!hasUpLeft && hasUpRight && hasDownLeft && hasDownRight) ||
                    (hasUpLeft && hasUpRight && hasDownLeft && hasDownRight)) {
                    // Skip, do nothing
                    std::cout << "";
                }

                // Continue  Continue
                // 3         14
                // 0🡬 1🡭    1🡬 0🡭
                // 0🡯 0🡮    1🡯 1🡮
                // .--.--.   .--.--.
                // |  |XX|   |XX|  |
                // .--o->.   .--o->.
                // |  |  |   |XX|XX|
                // .--.--.   .--.--.
                if ((!hasUpLeft && hasUpRight && !hasDownLeft && !hasDownRight) || (hasUpLeft && !hasUpRight && hasDownLeft && hasDownRight)) {
                    toDirectionPtr  = &DirectionRightEV;
                    previousEdgeDir = &DirectionUpEV;
                }
                // Continue Continue
                // 4        13
                // 1🡬 1🡭   0🡬 0🡭
                // 0🡯 0🡮   1🡯 1🡮
                // .--.--.  .--.--.
                // |XX|XX|  |  |  |
                // .--o->.  .--o->.
                // |  |  |  |XX|XX|
                // .--.--.  .--.--.
                if ((hasUpLeft && hasUpRight && !hasDownLeft && !hasDownRight) || (!hasUpLeft && !hasUpRight && hasDownLeft && hasDownRight)) {
                    toDirectionPtr  = &DirectionRightEV;
                    previousEdgeDir = &DirectionLeftEV;
                }

                // New edge
                // 9
                // 0🡬 0🡭
                // 0🡯 1🡮
                // .--.--.
                // |  |  |
                // .--o->.
                // |  |XX|
                // .--.--.
                if (!hasUpLeft && !hasUpRight && !hasDownLeft && hasDownRight) {
                    CellI& edgeJoint    = *new Object(kb, ShapeEdgeJointStruct);
                    CellI& newEdge      = *new Object(kb, ShapeEdgeStruct);
                    CellI& toShapePoint = shapePoint["right"];

                    CellI& edgeNode = *new Object(kb, ShapeEdgeNodeStruct);
                    edgeNode.set("from", shapePoint);
                    edgeNode.set("direction", DirectionRightEV);

                    if (toShapePoint.missing("edgeJoint")) {
                        CellI& toEdgeJoint = *new Object(kb, ShapeEdgeJointStruct);
                        toEdgeJoint.set("left", newEdge);
                        toShapePoint.set("edgeJoint", toEdgeJoint);
                    }
                    List& edgeNodes = *new List(kb, ShapeEdgeNodeStruct);
                    edgeNodes.add(edgeNode);
                    newEdge.set("edgeNodes", edgeNodes);
                    edgeJoint.set("right", newEdge);
                    shapePoint.set("edgeJoint", edgeJoint);
                    List* edgesPtr = nullptr;
                    if (currentShape.missing("edges")) {
                        List& newEdges = *new List(kb, ShapeEdgeStruct);
                        currentShape.set("edges", newEdges);
                        newEdge.set("id", _1_);
                        newEdge.set("kind", ExternalEdgeEV);
                        edgesPtr = &newEdges;
                    } else {
                        edgesPtr = &static_cast<List&>(currentShape["edges"]);
                        newEdge.set("id", kb.pools.numbers.get(edgesPtr->size() + 1));
                        newEdge.set("kind", InternalEdgeEV);
                    }
                    List& edges = *edgesPtr;
                    edges.add(newEdge);
                }
            } else {
                std::cout << "[" << caseNum << "|]";
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
                if ((hasUpLeft && !hasUpRight && !hasDownLeft && !hasDownRight) ||
                    (!hasUpLeft && hasUpRight && !hasDownLeft && !hasDownRight) ||
                    (hasUpLeft && hasUpRight && !hasDownLeft && !hasDownRight) ||
                    (!hasUpLeft && !hasUpRight && hasDownLeft && hasDownRight) ||
                    (hasUpLeft && !hasUpRight && hasDownLeft && hasDownRight) ||
                    (!hasUpLeft && hasUpRight && hasDownLeft && hasDownRight) ||
                    (hasUpLeft && hasUpRight && hasDownLeft && hasDownRight)) {
                    // Skip, do nothing
                }

                //  6 1010 Continue from up
                // 11 0101 Continue from up
                //
                // 6        11
                // 1🡬 0🡭  0🡬 1🡭
                // 1🡯 0🡮  0🡯 1🡮
                // .--.--.  .--.--.
                // |XX|  |  |  |XX|
                // .--o->.  .--o->.
                // |XX|  |  |  |XX|
                // .--.--.  .--.--.
                if ((hasUpLeft && !hasUpRight && hasDownLeft && !hasDownRight) ||
                    (!hasUpLeft && hasUpRight && !hasDownLeft && hasDownRight)) {
                    toDirectionPtr  = &DirectionDownEV;
                    previousEdgeDir = &DirectionUpEV;
                }

                //  5 0010 Continue from left
                // 10 1001 Continue from left
                // 12 1101 Continue from left
                //
                // 5       10       12
                // 0🡬 0🡭  1🡬 0🡭  1🡬 1🡭
                // 1🡯 0🡮  0🡯 1🡮  0🡯 1🡮
                // .--.--. .--.--.  .--.--.
                // |  |  | |XX|  |  |XX|XX|
                // .--o--. .--o--.  .--o--.
                // |XX|  | |  |XX|  |  |XX|
                // .--v--. .--v--.  .--v--.
                if ((!hasUpLeft && !hasUpRight && hasDownLeft && !hasDownRight) ||
                    (hasUpLeft && !hasUpRight && !hasDownLeft && hasDownRight) ||
                    (hasUpLeft && hasUpRight && !hasDownLeft && hasDownRight)) {
                    toDirectionPtr  = &DirectionDownEV;
                    previousEdgeDir = &DirectionLeftEV;
                }

                //  7 0110 Continue from right
                //  8 1110 Continue from right
                //  9 0001 Continue from right
                //
                // 7        8         9
                // 0🡬 1🡭   1🡬 1🡭   0🡬 0🡭
                // 1🡯 0🡮   1🡯 0🡮   0🡯 1🡮
                // .--.--.  .--.--.  .--.--.
                // |  |XX|  |XX|XX|  |  |  |
                // .--o--.  .--o--.  .--o--.
                // |XX|  |  |XX|  |  |  |XX|
                // .--v--.  .--v--.  .--v--.
                if ((!hasUpLeft && hasUpRight && hasDownLeft && !hasDownRight) ||
                    (hasUpLeft && hasUpRight && hasDownLeft && !hasDownRight) ||
                    (!hasUpLeft && !hasUpRight && !hasDownLeft && hasDownRight)) {
                    toDirectionPtr  = &DirectionDownEV;
                    previousEdgeDir = &DirectionRightEV;
                }
            }

            if (toDirectionPtr && previousEdgeDir) {
                CellI* toShapePointPtr = toDirectionPtr == &DirectionRightEV ? &shapePoint["right"] : &shapePoint["down"];
                CellI& toShapePoint    = *toShapePointPtr;
                CellI& newEdgeNode     = *new Object(kb, ShapeEdgeNodeStruct);
                newEdgeNode.set("from", shapePoint);
                newEdgeNode.set("direction", *toDirectionPtr);

                // from joint
                CellI* fromEdgeJointPtr = nullptr;
                if (shapePoint.has("edgeJoint")) {
                    fromEdgeJointPtr = &shapePoint["edgeJoint"];
                } else {
                    fromEdgeJointPtr = new Object(kb, ShapeEdgeJointStruct);
                    shapePoint.set("edgeJoint", *fromEdgeJointPtr);
                }
                CellI& fromEdgeJoint = *fromEdgeJointPtr;

                CellI* previousEdgePtr = nullptr;
                if (previousEdgeDir == &DirectionUpEV) {
                    previousEdgePtr = &fromEdgeJoint["up"];
                } else if (previousEdgeDir == &DirectionLeftEV) {
                    previousEdgePtr = &fromEdgeJoint["left"];
                } else if (previousEdgeDir == &DirectionRightEV) {
                    previousEdgePtr = &fromEdgeJoint["right"];
                }
                CellI& previousEdge = *previousEdgePtr;
                List& edgesNodes    = static_cast<List&>(previousEdge["edgeNodes"]);
                edgesNodes.add(newEdgeNode);

                // to joint
                CellI* toEdgeJointPtr = nullptr;
                if (toShapePoint.has("edgeJoint")) {
                    toEdgeJointPtr = &toShapePoint["edgeJoint"];
                } else {
                    toEdgeJointPtr = new Object(kb, ShapeEdgeJointStruct);
                    toShapePoint.set("edgeJoint", *toEdgeJointPtr);
                }
                CellI& toEdgeJoint = *toEdgeJointPtr;

                fromEdgeJoint.set(toDirectionPtr == &DirectionRightEV ? "right" : "down", previousEdge);
                toEdgeJoint.set(toDirectionPtr == &DirectionRightEV ? "left" : "up", previousEdge);
            }
            std::cout << fmt::format("({},{}) ", static_cast<Number&>(shapePoint["x"]).value(), static_cast<Number&>(shapePoint["y"]).value());

            CellI* nextListItemPtr       = currentListItem.has(kb.ids.next) ? &currentListItem[kb.ids.next] : nullptr;
            int nextPointY               = nextListItemPtr ? static_cast<Number&>((*nextListItemPtr)["value"]["y"]).value() : -1;
            bool isNextItemInTheSameLine = nextPointY == pointY;

            if (isNextItemInTheSameLine) {
                currentListItemPtr = nextListItemPtr;
            } else if (processingDirection == ProcessingDirection::LeftToRight) {
                if (nextListItemPtr) {
                    currentListItemPtr = firstColumnPointItem;
                    processingDirection = ProcessingDirection::UpToDown;
                } else {
                    currentListItemPtr = nextListItemPtr;
                }
            } else {
                currentListItemPtr   = nextListItemPtr;
                firstColumnPointItem = nextListItemPtr;
                processingDirection  = ProcessingDirection::LeftToRight;
            }
        }
        std::cout << std::endl;
    });

    // 1. row
    EXPECT_FALSE(currentShapePixelPtr->has("up"));
    EXPECT_TRUE(currentShapePixelPtr->has("down"));
    EXPECT_FALSE(currentShapePixelPtr->has("left"));
    EXPECT_TRUE(currentShapePixelPtr->has("right"));
    EXPECT_EQ(&(*currentShapePixelPtr)["pixel"]["x"], &_0_);
    EXPECT_EQ(&(*currentShapePixelPtr)["pixel"]["y"], &_0_);
    EXPECT_EQ(static_cast<hybrid::arc::Pixel&>((*currentShapePixelPtr)["pixel"]).m_x.value(), 0);
    EXPECT_EQ(static_cast<hybrid::arc::Pixel&>((*currentShapePixelPtr)["pixel"]).m_y.value(), 0);
    EXPECT_TRUE(currentShapePixelPtr->has("shape"));
    EXPECT_EQ(&(*currentShapePixelPtr)["shape"]["id"], &_1_);

    currentShapePixelPtr = &currentShapePixelPtr->get("right");
    EXPECT_FALSE(currentShapePixelPtr->has("up"));
    EXPECT_TRUE(currentShapePixelPtr->has("down"));
    EXPECT_TRUE(currentShapePixelPtr->has("left"));
    EXPECT_TRUE(currentShapePixelPtr->has("right"));
    EXPECT_TRUE(currentShapePixelPtr->has("shape"));
    EXPECT_EQ(&(*currentShapePixelPtr)["shape"]["id"], &_2_);

    currentShapePixelPtr = &currentShapePixelPtr->get("right");
    EXPECT_FALSE(currentShapePixelPtr->has("up"));
    EXPECT_TRUE(currentShapePixelPtr->has("down"));
    EXPECT_TRUE(currentShapePixelPtr->has("left"));
    EXPECT_TRUE(currentShapePixelPtr->has("right"));

    currentShapePixelPtr = &currentShapePixelPtr->get("right");
    EXPECT_FALSE(currentShapePixelPtr->has("up"));
    EXPECT_TRUE(currentShapePixelPtr->has("down"));
    EXPECT_TRUE(currentShapePixelPtr->has("left"));
    EXPECT_FALSE(currentShapePixelPtr->has("right"));

    // 2. row
    currentShapePixelPtr = &firstColumnPixelPtr->get("down");
    firstColumnPixelPtr = currentShapePixelPtr;

    EXPECT_TRUE(currentShapePixelPtr->has("up"));
    EXPECT_TRUE(currentShapePixelPtr->has("down"));
    EXPECT_FALSE(currentShapePixelPtr->has("left"));
    EXPECT_TRUE(currentShapePixelPtr->has("right"));

    currentShapePixelPtr = &currentShapePixelPtr->get("right");
    EXPECT_TRUE(currentShapePixelPtr->has("up"));
    EXPECT_TRUE(currentShapePixelPtr->has("down"));
    EXPECT_TRUE(currentShapePixelPtr->has("left"));
    EXPECT_TRUE(currentShapePixelPtr->has("right"));

    currentShapePixelPtr = &currentShapePixelPtr->get("right");
    EXPECT_TRUE(currentShapePixelPtr->has("up"));
    EXPECT_TRUE(currentShapePixelPtr->has("down"));
    EXPECT_TRUE(currentShapePixelPtr->has("left"));
    EXPECT_TRUE(currentShapePixelPtr->has("right"));

    currentShapePixelPtr = &currentShapePixelPtr->get("right");
    EXPECT_TRUE(currentShapePixelPtr->has("up"));
    EXPECT_TRUE(currentShapePixelPtr->has("down"));
    EXPECT_TRUE(currentShapePixelPtr->has("left"));
    EXPECT_FALSE(currentShapePixelPtr->has("right"));

    // 3. row
    currentShapePixelPtr = &firstColumnPixelPtr->get("down");
    EXPECT_TRUE(currentShapePixelPtr->has("up"));
    EXPECT_FALSE(currentShapePixelPtr->has("down"));
    EXPECT_FALSE(currentShapePixelPtr->has("left"));
    EXPECT_TRUE(currentShapePixelPtr->has("right"));

    currentShapePixelPtr = &currentShapePixelPtr->get("right");
    EXPECT_TRUE(currentShapePixelPtr->has("up"));
    EXPECT_FALSE(currentShapePixelPtr->has("down"));
    EXPECT_TRUE(currentShapePixelPtr->has("left"));
    EXPECT_TRUE(currentShapePixelPtr->has("right"));

    currentShapePixelPtr = &currentShapePixelPtr->get("right");
    EXPECT_TRUE(currentShapePixelPtr->has("up"));
    EXPECT_FALSE(currentShapePixelPtr->has("down"));
    EXPECT_TRUE(currentShapePixelPtr->has("left"));
    EXPECT_TRUE(currentShapePixelPtr->has("right"));

    currentShapePixelPtr = &currentShapePixelPtr->get("right");
    EXPECT_TRUE(currentShapePixelPtr->has("up"));
    EXPECT_FALSE(currentShapePixelPtr->has("down"));
    EXPECT_TRUE(currentShapePixelPtr->has("left"));
    EXPECT_FALSE(currentShapePixelPtr->has("right"));

    currentShapePixelPtr = firstShapePixel;
    firstColumnPixelPtr = firstShapePixel;

    std::cout << "All shape pixel:" << std::endl;
    while (currentShapePixelPtr) {
        hybrid::arc::Pixel& currentArcPixel = static_cast<hybrid::arc::Pixel&>((*currentShapePixelPtr)["pixel"]);
        const int x                         = currentArcPixel.m_x.value();
        const int y                         = currentArcPixel.m_y.value();
        std::cout << "[" << x << ", " << y << "]";
        if (x == 0) {
            EXPECT_FALSE(currentShapePixelPtr->has("left"));
        } else {
            hybrid::arc::Pixel& otherArcPixel = static_cast<hybrid::arc::Pixel&>((*currentShapePixelPtr)["left"]["pixel"]);
            EXPECT_EQ(x - 1, otherArcPixel.m_x.value());
        }
        if (x == inputGrid1.width() - 1) {
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
        if (y == inputGrid1.height() - 1) {
            EXPECT_FALSE(currentShapePixelPtr->has("down"));
        } else {
            hybrid::arc::Pixel& otherArcPixel = static_cast<hybrid::arc::Pixel&>((*currentShapePixelPtr)["down"]["pixel"]);
            EXPECT_EQ(y + 1, otherArcPixel.m_y.value());
        }
        if (currentShapePixelPtr->has("right")) {
            currentShapePixelPtr = &(*currentShapePixelPtr)["right"];
        } else if (firstColumnPixelPtr->has("down")) {
            currentShapePixelPtr = &(*firstColumnPixelPtr)["down"];
            firstColumnPixelPtr  = currentShapePixelPtr;
        } else {
            currentShapePixelPtr = nullptr;
            std::cout << std::endl;
        }
    }

    ScanLineState scanLineState = ScanLineState::Up;
    currentShapePixelPtr = firstShapePixel;
    firstColumnPixelPtr  = firstShapePixel;
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
                scanLineState = ScanLineState::Up;
                currentShapePixelPtr = &(*firstColumnPixelPtr)["down"];
                firstColumnPixelPtr  = currentShapePixelPtr;
            } else {
                scanLineState = ScanLineState::Down;
                currentShapePixelPtr = firstColumnPixelPtr;
            }
            std::cout << std::endl;
        } else if (scanLineState == ScanLineState::Down) {
            scanLineState = ScanLineState::Up;
            currentShapePixelPtr = nullptr;
        }
    }
    std::cout << std::endl;

    scanLineState = ScanLineState::Up;
    currentShapePixelPtr        = firstShapePixel;
    firstColumnPixelPtr         = firstShapePixel;

    Shape cppShape1(_1_);
    Shape cppShape2(_2_);
    Shape cppShape3(_3_);
    while (currentShapePixelPtr) {
        CellI& currentShapePixel            = *currentShapePixelPtr;
        hybrid::arc::Pixel& currentArcPixel = static_cast<hybrid::arc::Pixel&>(currentShapePixel["pixel"]);
        const int x                         = currentArcPixel.m_x.value();
        const int y                         = currentArcPixel.m_y.value();

        CellI& currentShape = currentShapePixel["shape"];
        Shape* currentCppShapePtr = nullptr;
        if (&currentShape["id"] == &cppShape1.id) {
            currentCppShapePtr = &cppShape1;
        } else if (&currentShape["id"] == &cppShape2.id) {
            currentCppShapePtr = &cppShape2;
        } else if (&currentShape["id"] == &cppShape3.id) {
            currentCppShapePtr = &cppShape3;
        }
        Shape& currentCppShape = *currentCppShapePtr;

        switch (scanLineState) {
        case ScanLineState::Up:
            if (currentShapePixel.missing("up") || &currentShapePixel["up"]["shape"] != &currentShape) {
                EdgeLine newEdge(x, y, Direction::Horizontal);
                currentCppShape.addEdge(newEdge);
                // add edge
                std::cout << "--";
            } else {
                std::cout << "  ";
            }
            break;
        case ScanLineState::Middle:
            if (currentShapePixel.missing("left") || &currentShapePixel["left"]["shape"] != &currentShape) {
                EdgeLine newEdge(x, y, Direction::Vertical);
                currentCppShape.addEdge(newEdge);
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

    for (int y = 0; y < inputGrid1.height(); ++y) {
        Object& colX = static_cast<Object&>(shapePixels.method(kb.name("getValue"), { kb.ids.key, toCellNumber(y) }));
        for (int x = 0; x < inputGrid1.width(); ++x) {
            CellI& shapePixel         = colX.method(kb.name("getValue"), { kb.ids.key, toCellNumber(x) });
            CellI& shape              = shapePixel["shape"];
            hybrid::arc::Pixel& pixel = static_cast<hybrid::arc::Pixel&>(shapePixel["pixel"]);
            EXPECT_EQ(x, pixel.m_x.value());
            EXPECT_EQ(y, pixel.m_y.value());
        }
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    std::cout << "Constructed: " << CellI::s_constructed << ", destructed: " << CellI::s_destructed << ", live: " << CellI::s_constructed - CellI::s_destructed << std::endl;

    return ret;
}