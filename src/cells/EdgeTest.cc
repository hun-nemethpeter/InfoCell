#include "CellTestBase.h"
#include "Arc.h"

#include "Config.h"
#include "app/App.h"
#include "util/ArcTask.h"

#include <fstream>
#include <nlohmann/json.hpp>

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include "Log.h"
#include <fmt/color.h>

using namespace infocell;
using namespace infocell::cells;
using nlohmann::json;
using infocell::cells::test::CellTest;

static spdlog::logger* s_logger = nullptr;

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
        CellTest([]() {
            infocell::cells::brain::Brain::Logger::createLogger("edge");
            infocell::cells::brain::Brain::Logger::createLogger("shapeRelations");
            infocell::cells::brain::Brain::Logger::createLogger("shapeIdGrid");
            infocell::cells::brain::Brain::Logger::createLogger("grid");

            spdlog::get("cells")->set_level(spdlog::level::trace);
            spdlog::get("compileStruct")->set_level(spdlog::level::off);
            spdlog::get("compiledSymbols")->set_level(spdlog::level::off);
            spdlog::get("edge")->set_level(spdlog::level::off);
            spdlog::get("shapeIdGrid")->set_level(spdlog::level::off);
            spdlog::get("grid")->set_level(spdlog::level::trace);
            spdlog::get("shapeRelations")->set_level(spdlog::level::trace);
        }),
        ShaperStruct(getStruct("arc::Shaper")),
        ShapeStruct(getStruct("arc::Shape")),
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
        auto& TableRowStruct = getStruct(kb.templateId("std::Map", ids.keyType, kb.std.Number, ids.valueType, ShapeStruct));
        auto& TableStruct    = getStruct(kb.templateId("std::Map", ids.keyType, kb.std.Number, ids.valueType, TableRowStruct));
    }

    void setOutputSVGName(const std::string& fileName)
    {
        m_outputSVGFileName = fileName;
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
        printInputHybridGrid();
        shaperProcess();
        sortShapePixelsAndCreateShapePoints();
//        printEveryShapePixels();
        printAndTestShapePixels();
        sortShapePoints();
//        printAllShapePoints();
        calculateEdgesForShapes();
        processEdgeNodes();
        validateEdgePoints();
//        drawSvgFromShapePointEdgeJoints();
        printShapeIdGrid();
        printShapeIdGridAsJson();
//        printEdges();
        printShapeRelations();
        findPossibleBackgroundWithShapes();
    }

    void setInputGrid(const std::string& jsonStr)
    {
        m_inputGrid       = std::make_unique<input::Grid>("inputGrid", jsonStr);
        m_inputHybridGrid = std::make_unique<cells::hybrid::arc::Grid>(kb, *m_inputGrid);
    }

    static std::string colorTile(int color)
    {
        static std::vector<std::string> colors(
            {
                fmt::format(fmt::bg(fmt::rgb(0x00, 0x00, 0x00)), "  "), /* 0 black */
                fmt::format(fmt::bg(fmt::rgb(0x00, 0x74, 0xD9)), "  "), /* 1 blue */
                fmt::format(fmt::bg(fmt::rgb(0xFF, 0x41, 0x36)), "  "), /* 2 red */
                fmt::format(fmt::bg(fmt::rgb(0x2E, 0xCC, 0x40)), "  "), /* 3 green */
                fmt::format(fmt::bg(fmt::rgb(0xFF, 0xDC, 0x00)), "  "), /* 4 yellow */
                fmt::format(fmt::bg(fmt::rgb(0xAA, 0xAA, 0xAA)), "  "), /* 5 grey */
                fmt::format(fmt::bg(fmt::rgb(0xF0, 0x12, 0xBE)), "  "), /* 6 fuschia */
                fmt::format(fmt::bg(fmt::rgb(0xFF, 0x85, 0x1B)), "  "), /* 7 orange */
                fmt::format(fmt::bg(fmt::rgb(0x7F, 0xDB, 0xFF)), "  "), /* 8 teal */
                fmt::format(fmt::bg(fmt::rgb(0x87, 0x0C, 0x25)), "  ")  /* 9 brown */
            });

        return colors[color];
    }

    void printInputHybridGrid()
    {
        cells::hybrid::arc::Grid& grid = inputHybridGrid();
        ftxui::Elements boardLines;
        for (int y = 0; y < grid.height(); ++y) {
            std::stringstream ss;
            for (int x = 0; x < grid.width(); ++x) {
                hybrid::arc::Pixel& pixel = grid.getPixel(x, y);
                ss << colorTile(pixel.color());
            }
            TRACE(grid, ss.str());
        }
    }

    void printShapeRelations()
    {
        DEBUG(shapeRelations, "printShapeRelations");
        Visitor::visitList(shaper()["shapes"], [this](CellI& shape, int, bool&) {
            int edgesCount = static_cast<Map&>(shape["edges"]).size();
            if (edgesCount == 1) {
                TRACE(shapeRelations, "  shape id {} has only external edge", shape["id"].label());
            } else {
                TRACE(shapeRelations, "  shape id {} has internal edge(s)", shape["id"].label());
                Visitor::visitList(shape["edges"]["list"], [this](CellI& edge, int, bool&) {
                    if (&edge["kind"] == &InternalEdgeEV) {
                        if (edge.has("shapes")) {
                            std::stringstream ss;
                            Visitor::visitList(edge["shapes"]["index"]["struct"]["slots"]["list"], [this, &ss](CellI& slot, int, bool&) {
                                CellI& shape = slot["slotRole"];
                                ss << fmt::format("shape({}) ", shape["id"].label());
                            });
                            TRACE(shapeRelations, "    edge id {} internal and contains: {}", edge["id"].label(), ss.str());
                        } else {
                            TRACE(shapeRelations, "    edge id {} internal", edge["id"].label());
                        }
                    }
                });
            }
        });
    }

    void setInputGrid(hybrid::arc::Grid& inputHybridGrid)
    {
        m_inputHybridGridPtr = &inputHybridGrid;
    }

    void shaperProcess()
    {
        auto& TableRowStruct = getStruct(kb.templateId("std::Map", ids.keyType, kb.std.Number, ids.valueType, ShapeStruct));
        m_hybridShaper       = std::make_unique<hybrid::arc::Shaper>(kb, inputHybridGrid(), ShapeStruct, TableRowStruct);
        m_hybridShaper->process();
        return;
        m_shaper = std::make_unique<Object>(kb, ShaperStruct, kb.name("constructor"), Param { "grid", inputHybridGrid() });
        m_shaper->method("process");
    }

    CellI& shaper()
    {
        if (m_hybridShaper) {
            return *m_hybridShaper;
        }
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
        TRACE(edge, "printEveryShapePixels");
        Visitor::visitList(shaper()["shapes"], [this](CellI& currentShape, int, bool&) {
            TRACE(edge, "Shape id: {}, pixels: ", currentShape["id"].label());
            Visitor::visitList(currentShape["shapePixels"], [this](CellI& shapePixel, int, bool&) {
                TRACE(edge, "[{}, {}]", shapePixel["pixel"]["x"].label(), shapePixel["pixel"]["y"].label());
            });
        });
    }

    void printAndTestShapePixels()
    {
        CellI* firstColumnPixelPtr  = firstShapePixelPtr();
        CellI* currentShapePixelPtr = firstShapePixelPtr();

        DEBUG(edge, "printAndTestShapePixels");
        int referenceX = 0;
        int referenceY = 0;
        std::stringstream ss;
        ss << "  ";
        while (currentShapePixelPtr) {
            hybrid::arc::Pixel& currentArcPixel = static_cast<hybrid::arc::Pixel&>((*currentShapePixelPtr)["pixel"]);
            const int x                         = currentArcPixel.m_x.value();
            const int y                         = currentArcPixel.m_y.value();
            EXPECT_EQ(x, referenceX);
            EXPECT_EQ(y, referenceY);
            ss << "[" << x << "," << y << "]";
            if (x == 0) {
                EXPECT_FALSE(currentShapePixelPtr->has("left"));
            } else {
                hybrid::arc::Pixel& otherArcPixel = static_cast<hybrid::arc::Pixel&>((*currentShapePixelPtr)["left"]["pixel"]);
                EXPECT_EQ(x - 1, otherArcPixel.m_x.value());
            }
            if (x == inputHybridGrid().width() - 1) {
                EXPECT_FALSE(currentShapePixelPtr->has("right"));
                TRACE(edge, ss.str());
                ss.str("");
                ss << "  ";
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

    void drawSvgFromShapePointEdgeJoints()
    {
        static std::map<int, std::string> arcColors = {
            { 0, "#000000" },
            { 1, "#0074D9" },
            { 2, "#FF4136" },
            { 3, "#2ECC40" },
            { 4, "#FFDC00" },
            { 5, "#AAAAAA" },
            { 6, "#F012BE" },
            { 7, "#FF851B" },
            { 8, "#7FDBFF" },
            { 9, "#870C25" }
        };
        std::string outputSVGFileName;
        if (m_outputSVGFileName.empty()) {
            outputSVGFileName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
        } else {
            outputSVGFileName = m_outputSVGFileName;
        }

        const std::filesystem::path& path = outputSVGFileName + ".svg";
        std::ofstream svgFile(path);
        svgFile << fmt::format("<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"{}\" height=\"{}\">\n", 40 + inputHybridGrid().width() * 120, 50 + inputHybridGrid().height() * 120);
        svgFile << R"-(
<style>
    .arrowText {
        font: 8px sans-serif;
     }
	 .pointCoordinateText {
        font: 5px sans-serif;
        fill: #d79b00;
	 }
	 .pixelCoordinateText {
        font: 8px sans-serif;
        fill: white;
		stroke: black;
		stroke-width: 1px;
		paint-order: stroke;
	 }
     .pixelShapeIdText {
        font: 16px sans-serif;
        fill: white;
		stroke: black;
		stroke-width: 2px;
		paint-order: stroke;
     }
</style>
<marker
      id="arrow"
      viewBox="0 0 10 10"
      refX="5"
      refY="5"
      markerWidth="6"
      markerHeight="6"
      orient="auto-start-reverse">
      <path fill="context-stroke" d="M 0 0 L 10 5 L 0 10 z" />
</marker>
<g transform="translate(10 10)
              scale(1 1)">
)-";

        CellI* currentShapePointPtr = &(*firstShapePixelPtr())["upLeftPoint"];
        CellI* firstColumnPointPtr  = currentShapePointPtr;
        while (currentShapePointPtr) {
            CellI& shapePoint      = *currentShapePointPtr;
            int pointX             = static_cast<Number&>(shapePoint["x"]).value();
            int pointY             = static_cast<Number&>(shapePoint["y"]).value();
            svgFile << fmt::format("    <!-- point({}, {}) -->\n", pointX, pointY);
            svgFile << fmt::format("    <ellipse cx=\"{}\" cy=\"{}\" rx=\"3\" ry=\"3\" fill=\"black\" stroke=\"grey\"/>\n", 10 + pointX * 120, 30 + pointY * 120);
            svgFile << fmt::format("    <!-- edgeJoint -->\n");
            if (shapePoint.has("edgeJoint")) {
                CellI& edgeJoint = shapePoint["edgeJoint"];
                svgFile << fmt::format("    <path d=\"M {} {} L {} {}\" fill=\"none\" stroke=\"#d79b00\" stroke-miterlimit=\"10\" stroke-dasharray=\"1 1\"/>\n", 10 + pointX * 120, 20 + pointY * 120, 10 + pointX * 120, 30 + pointY * 120);
                svgFile << fmt::format("    <rect x=\"{}\" y=\"{}\" width=\"20\" height=\"20\" fill=\"#ffe6cc\" stroke=\"#d79b00\"/>\n", pointX * 120, pointY * 120);
                svgFile << fmt::format("    <text x=\"{}\" y=\"{}\" class=\"pointCoordinateText\">({}, {})</text>\n", 1 + pointX * 120, 8 + pointY * 120, pointX, pointY);

                auto drawArrow = [this, &svgFile](int pointX, int pointY, CellI& edgeJoint, const std::string& edgeJointSlotName) {
                    const int arrowLineLength = 30;
                    const int arrowLength = 3;

                    CellI& edgeNode = edgeJoint[edgeJointSlotName];
                    CellI& direction = edgeNode["direction"];
                    int color        = static_cast<Number&>(edgeNode["edge"]["shape"]["color"]).value();
                    std::string debugText;

                    int startX    = -1;
                    int startY    = -1;
                    int lineFromX = -1;
                    int lineFromY = -1;
                    int lineToX   = -1;
                    int lineToY   = -1;

                    int textFromX = -1;
                    int textFromY = -1;

                    if (edgeJointSlotName == "rightDown") {
                        debugText = "right down";
                        textFromX = 30 + pointX * 120;
                        textFromY = 18 + pointY * 120;
                        lineFromX = 20 + pointX * 120;
                        lineFromY = 20 + pointY * 120;
                        lineToX   = lineFromX + arrowLineLength;
                        lineToY   = lineFromY;
                    } else if (edgeJointSlotName == "rightUp") {
                        debugText = "right up";
                        textFromX = 30 + pointX * 120;
                        textFromY = -2 + pointY * 120;
                        lineFromX = 20 + pointX * 120;
                        lineFromY = pointY * 120;
                        lineToX   = lineFromX + arrowLineLength;
                        lineToY   = lineFromY;
                    } else if (edgeJointSlotName == "leftDown") {
                        debugText = "left down";
                        textFromX = -20 + pointX * 120;
                        textFromY = 18 + pointY * 120;
                        lineFromX = -30 + pointX * 120;
                        lineFromY = 20 + pointY * 120;
                        lineToX   = lineFromX + arrowLineLength;
                        lineToY   = lineFromY;
                    } else if (edgeJointSlotName == "leftUp") {
                        debugText = "left up";
                        textFromX = -20 + pointX * 120;
                        textFromY = -2 + pointY * 120;
                        lineFromX = -30 + pointX * 120;
                        lineFromY = pointY * 120;
                        lineToX   = lineFromX + arrowLineLength;
                        lineToY   = lineFromY;
                    } else if (edgeJointSlotName == "downLeft") {
                        debugText = "down left";
                        startX    = pointX * 120;
                        startY    = 20 + pointY * 120;
                        textFromX = 15;
                        textFromY = -2;
                        lineFromX = 0;
                        lineFromY = 0;
                        lineToX   = arrowLineLength;
                        lineToY   = 0;
                    } else if (edgeJointSlotName == "downRight") {
                        debugText = "down right";
                        startX    = 20 + pointX * 120;
                        startY    = 20 + pointY * 120;
                        textFromX = 15;
                        textFromY = -2;
                        lineFromX = 0;
                        lineFromY = 0;
                        lineToX   = arrowLineLength;
                        lineToY   = 0;
                    } else if (edgeJointSlotName == "upLeft") {
                        debugText = "up left";
                        startX    = pointX * 120;
                        startY    = -30 + pointY * 120;
                        textFromX = 10;
                        textFromY = -2;
                        lineFromX = 0;
                        lineFromY = 0;
                        lineToX   = arrowLineLength;
                        lineToY   = 0;
                    } else if (edgeJointSlotName == "upRight") {
                        debugText = "up right";
                        startX    = 20 + pointX * 120;
                        startY    = -30 + pointY * 120;
                        textFromX = 10;
                        textFromY = -2;
                        lineFromX = 0;
                        lineFromY = 0;
                        lineToX   = arrowLineLength;
                        lineToY   = 0;
                    }
                    if (&direction == &DirectionLeftEV && (edgeJointSlotName == "rightDown" || edgeJointSlotName == "rightUp")) {
                        lineFromX += arrowLength;
                        lineToX += arrowLength;
                    }
                    if (&direction == &DirectionRightEV && (edgeJointSlotName == "leftDown" || edgeJointSlotName == "leftUp")) {
                        lineFromX -= arrowLength;
                        lineToX -= arrowLength;
                    }
                    if (&direction == &DirectionUpEV && (edgeJointSlotName == "downLeft" || edgeJointSlotName == "downRight")) {
                        lineFromX += 3;
                        lineToX += 3;
                    }
                    if (&direction == &DirectionDownEV && (edgeJointSlotName == "upLeft" || edgeJointSlotName == "upRight")) {
                        lineFromX -= 3;
                        lineToX -= 3;
                    }
                    if (&direction == &DirectionLeftEV || &direction == &DirectionUpEV) {
                        std::swap(lineFromX, lineToX);
                        std::swap(lineFromY, lineToY);
                    }
                    svgFile << fmt::format("      <!-- edgeJoint {} -->\n", debugText);
                    std::string externalShapeId;
                    if (edgeNode.has("externalShape")) {
                        externalShapeId = fmt::format("({})", edgeNode["externalShape"]["id"].label());
                    }
                    if (&direction == &DirectionRightEV || &direction == &DirectionLeftEV) {
                        svgFile << fmt::format("      <text x=\"{}\" y=\"{}\" fill=\"{}\" class=\"arrowText\">{}{}</text>\n", textFromX, textFromY, arcColors[color], edgeNode["edge"]["id"].label(), externalShapeId);
                        svgFile << fmt::format("      <line x1=\"{}\" y1=\"{}\" x2=\"{}\" y2=\"{}\" stroke=\"{}\" marker-end=\"url(#arrow)\"/>\n", lineFromX, lineFromY, lineToX, lineToY, arcColors[color]);
                    } else {
                        svgFile << fmt::format("      <g transform=\"translate({} {}) rotate(90)\">\n", startX, startY);
                        svgFile << fmt::format("        <text x=\"{}\" y=\"{}\" fill=\"{}\" class=\"arrowText\">{}{}</text>\n", textFromX, textFromY, arcColors[color], edgeNode["edge"]["id"].label(), externalShapeId);
                        svgFile << fmt::format("        <line x1=\"{}\" y1=\"{}\" x2=\"{}\" y2=\"{}\" stroke=\"{}\" marker-end=\"url(#arrow)\"/>\n", lineFromX, lineFromY, lineToX, lineToY, arcColors[color]);
                        svgFile << fmt::format("      </g>\n");
                    }
                };

                for (const auto& direction : { "rightDown", "rightUp", "leftDown", "leftUp", "downLeft", "downRight", "upLeft", "upRight" }) {
                    if (edgeJoint.has(direction)) {
                        drawArrow(pointX, pointY, edgeJoint, direction);
                    }
                }
            }
            if (shapePoint.has("downRightPixel")) {
                svgFile << fmt::format("    <!-- pixel -->\n");
                hybrid::arc::Pixel& pixel = static_cast<hybrid::arc::Pixel&>(shapePoint["downRightPixel"]["pixel"]);
                svgFile << fmt::format("    <rect x=\"{}\" y=\"{}\" width=\"40\" height=\"40\" fill=\"{}\" stroke=\"black\"/>\n", 50 + pointX * 120, 70 + pointY * 120, arcColors[pixel.color()]);
                int shapeId = static_cast<Number&>(shapePoint["downRightPixel"]["shape"]["id"]).value();
                int startX  = 0;
                if (shapeId < 10) {
                    startX = 66;
                } else if (shapeId < 100) {
                    startX = 62;
                } else {
                    startX = 57;
                }
                svgFile << fmt::format("    <text x=\"{}\" y=\"{}\" class=\"pixelCoordinateText\">({}, {})</text>\n", 52 + pointX * 120, 78 + pointY * 120, pixel.m_x.value(), pixel.m_y.value());
                svgFile << fmt::format("    <text x=\"{}\" y=\"{}\" class=\"pixelShapeIdText\">{}</text>\n", startX + pointX * 120, 98 + pointY * 120, shapeId);
            }
            svgFile << "\n";
            if (shapePoint.has("right")) {
                currentShapePointPtr = &shapePoint["right"];
            } else if (shapePoint.has("down")) {
                currentShapePointPtr = &(*firstColumnPointPtr)["down"];
                firstColumnPointPtr  = currentShapePointPtr;
            } else {
                currentShapePointPtr = nullptr;
            }
        }

        svgFile << "</g>\n</svg>";
    }

    void sortShapePoints()
    {
        Visitor::visitList(shaper()["shapes"], [this](CellI& currentShape, int, bool&) {
            TRACE(edge, "Shape id: {}, points:", currentShape["id"].label());

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

            std::stringstream ss;
            ss << "  ";
            while (currentListItemPtr) {
                switch (scanLineState) {
                case ScanLineState::Up: {
                    CellI& currentListItem = *currentListItemPtr;
                    CellI& shapePixel      = currentListItem[kb.ids.value];
                    CellI& shapePoint      = shapePixel["upLeftPoint"];
                    int upLeftPointX       = static_cast<Number&>(shapePoint["x"]).value();

                    if (upLeftPointX > pointX) {
                        ss << fmt::format("({},{}) ", upLeftPointX, pointY);
                        shapePoints.add(shapePoint);
                    }
                    pointX = upLeftPointX + 1;
                    ss << fmt::format("({},{}) ", pointX, pointY);
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
                            TRACE(edge, "{} Up -> Middle", ss.str());
                            ss.str("");
                            ss << "  ";
                        }
                    } else {
                        currentListItemPtr = firstColumnPixelItem;
                        pointX             = -1;
                        ++pointY;
                        scanLineState = ScanLineState::Down;
                        TRACE(edge, "{} Up -> Down", ss.str());
                        ss.str("");
                        ss << "  ";
                    }
                } break;
                case ScanLineState::Middle: {
                    CellI& currentListItem = *currentListItemPtr;
                    CellI& shapePixel      = currentListItem[kb.ids.value];
                    CellI& shapePoint      = shapePixel[isUpperLine ? "downLeftPoint" : "upLeftPoint"];
                    int currentPointX      = static_cast<Number&>(shapePoint["x"]).value();
                    int currentPointY      = static_cast<Number&>(shapePoint["y"]).value();
                    if (currentPointX > pointX) {
                        ss << fmt::format("({},{}) ", currentPointX, pointY);
                        shapePoints.add(shapePoint);
                    }
                    pointX = currentPointX + 1;
                    ss << fmt::format("({},{}) ", pointX, pointY);
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
                        currentListItemPtr    = isUpperLine ? upMiddleRowListItem : downMiddleRowListItem;
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
                        ss << fmt::format("({},{}) ", downLeftPointX, pointY);
                        shapePoints.add(shapePoint);
                    }
                    pointX = downLeftPointX + 1;
                    ss << fmt::format("({},{}) ", pointX, pointY);
                    shapePoints.add(shapePoint["right"]);

                    currentListItemPtr = currentListItem.has(kb.ids.next) ? &currentListItem[kb.ids.next] : nullptr;
                } break;
                }
            }
            TRACE(edge, ss.str());
        });
    }

    void printAllShapePoints()
    {
        DEBUG(edge, "printAllShapePoints");
        Visitor::visitList(shaper()["shapes"], [this](CellI& currentShape, int, bool&) {
            std::stringstream ss;
            int lastY = static_cast<Number&>(currentShape["shapePoints"]["first"]["value"]["y"]).value();
            TRACE(edge, "Shape id: {}", currentShape["id"].label());
            Visitor::visitList(currentShape["shapePoints"], [this, &ss, &lastY](CellI& shapePoint, int, bool&) {
                const int y = static_cast<Number&>(shapePoint["y"]).value();
                if (lastY != y) {
                    TRACE(edge, "    {}", ss.str());
                    ss.str("");
                    lastY = y;
                }
                ss << fmt::format("({},{}) ", static_cast<Number&>(shapePoint["x"]).value(), y);
            });
            TRACE(edge, "    {}", ss.str());
        });
    }

    void calculateEdgesForShapes()
    {
        DEBUG(edge, "calculateEdgesForShapes");
        Visitor::visitList(shaper()["shapes"], [this](CellI& currentShape, int, bool&) {
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
            CellI* currentListItemPtr     = &currentShape["shapePoints"][kb.ids.first];
            CellI* previousListItemPtr    = nullptr;
            CellI* firstColumnPointItem   = currentListItemPtr;

            CellI* newEdgePtr  = nullptr;
            List* edgeNodesPtr = nullptr;

            const char* toDirectionStr             = "";
            const char* toEdgeDirectionStr         = "";
            const char* toOppositeEdgeDirectionStr = "";

            CellI* currentShapePointPtr            = nullptr;
            CellI* firstShapePointPtr              = nullptr;
            CellI* previousEdgeNodePtr             = nullptr;

            while (currentListItemPtr || currentShapePointPtr) {
                CellI& currentListItem = *currentListItemPtr;
                CellI& shapePoint      = currentListItemPtr ? (*currentListItemPtr)[kb.ids.value] : *currentShapePointPtr;
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
                case ProcessingMode::InternalEdgeStart:
                {
                    // create new edge
                    CellI& newEdge  = *new Object(kb, ShapeEdgeStruct);
                    List& edgeNodes = *new List(kb, ShapeEdgeNodeStruct);
                    CellI& newEdgeId = kb.pools.numbers.get(static_cast<Number&>(currentShape["lastEdgeId"]).value() + 1);
                    currentShape.set("lastEdgeId", newEdgeId);
                    newEdge.set("shape", currentShape);
                    newEdge.set("edgeNodes", edgeNodes);
                    newEdge.set("id", newEdgeId);

                    Map& edges = static_cast<Map&>(currentShape["edges"]);
                    edges.add(newEdge["id"], newEdge);

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
                    } else {
                        newEdge.set("kind", InternalEdgeEV);
                        processingMode = ProcessingMode::InternalEdgeDetect;
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
                        fromEdgeJointPtr = new Object(kb, ShapeEdgeJointStruct);
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
                        toEdgeJointPtr = new Object(kb, ShapeEdgeJointStruct);
                        toShapePoint.set("edgeJoint", *toEdgeJointPtr);
                    }

                    CellI& toEdgeJoint = *toEdgeJointPtr;
                    if (toEdgeJoint.has(toOppositeEdgeDirectionStr)) {
                        ERROR(edge, "Edge processing error: edgeNode already exists!");
                    }

                    // new edge node
                    CellI& newEdgeNode = *new Object(kb, ShapeEdgeNodeStruct);
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
                                CellI& upRightEdgeNode   = edgeJoint["upRight"];
                                CellI& leftDownEdgeNode = edgeJoint["leftDown"];
                                bool rightUpEdgeNode = edgeJoint.has("rightUp");
                                if (!rightUpEdgeNode && (&upRightEdgeNode["edge"]["kind"] == &ExternalEdgeEV) && (&leftDownEdgeNode["edge"]["kind"] == &ExternalEdgeEV)) {
                                    isUnprocessedEdge = true;
                                }
                            }
                        }
                    }
                    if (isUnprocessedEdge) {
                        processingDirectionPtr = &DirectionRightEV;
                        processingMode = ProcessingMode::InternalEdgeStart;
                    } else {
                        currentListItemPtr = currentListItem.has(kb.ids.next) ? &currentListItem[kb.ids.next] : nullptr;
                    }
                } break;
                } // switch processinMode
            } // while has more shapePoints
        }); // visit shapePoints
    }

    void processEdgeNodes()
    {
        CellI* firstColumnPointPtr = &(*firstShapePixelPtr())["upLeftPoint"];
        CellI* shapePointPtr       = firstColumnPointPtr;
        CellI* lastShapeEdgeInLine = nullptr;
        List internalEdges(kb, ShapeEdgeStruct);
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
                            List& edgeNodes = *new List(kb, ShapePixelStruct);
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
                                Set& newShapesSet = *new Set(kb, ShapeStruct);
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
                    processingDirection  = ProcessingDirection::UpToDown;
                    shapePointPtr = firstColumnPointPtr;
                    break;
                case ProcessingDirection::UpToDown:
                    processingDirection  = ProcessingDirection::LeftToRight;
                    shapePointPtr = &(*firstColumnPointPtr)["down"];
                    firstColumnPointPtr  = shapePointPtr;
                    break;
                }
            } else {
                shapePointPtr = nullptr;
            }
        }
    }

    void validateEdgePoints()
    {
        CellI* firstColumnPointPtr  = &(*firstShapePixelPtr())["upLeftPoint"];
        CellI* currentShapePointPtr = firstColumnPointPtr;

        while (currentShapePointPtr) {
            CellI& currentShapePoint = *currentShapePointPtr;
            const int x              = static_cast<Number&>(currentShapePoint["x"]).value();
            const int y              = static_cast<Number&>(currentShapePoint["y"]).value();
            bool hasEdgeJoint        = currentShapePoint.has("edgeJoint");

            if (hasEdgeJoint) {
                CellI& edgeJoint = currentShapePoint["edgeJoint"];
                if (currentShapePoint.missing("up")) {
                    EXPECT_TRUE(edgeJoint.missing("upLeft"));
                    EXPECT_TRUE(edgeJoint.missing("upRight"));
                }
                if (currentShapePoint.missing("down")) {
                    EXPECT_TRUE(edgeJoint.missing("downLeft"));
                    EXPECT_TRUE(edgeJoint.missing("downRight"));
                }
                if (currentShapePoint.missing("left")) {
                    EXPECT_TRUE(edgeJoint.missing("leftUp"));
                    EXPECT_TRUE(edgeJoint.missing("leftDown"));
                }
                if (currentShapePoint.missing("right")) {
                    EXPECT_TRUE(edgeJoint.missing("rightUp"));
                    EXPECT_TRUE(edgeJoint.missing("rightDown"));
                }
                if (currentShapePoint.has("up") && currentShapePoint.has("down") && currentShapePoint.has("left") && currentShapePoint.has("right")) {
                    for (const auto& directionPair : { std::pair("rightDown", "rightUp"),
                                                       std::pair("leftDown", "leftUp"),
                                                       std::pair("downLeft", "downRight"),
                                                       std::pair("upLeft", "upRight") }) {
                        if (edgeJoint.has(directionPair.first) || edgeJoint.has(directionPair.second)) {
                            EXPECT_TRUE(edgeJoint.has(directionPair.first));
                            EXPECT_TRUE(edgeJoint.has(directionPair.second));
                            CellI& edgeNodeFirst = edgeJoint[directionPair.first];
                            CellI& edgeNodeSecond = edgeJoint[directionPair.second];
                            EXPECT_EQ(&edgeNodeFirst["externalShape"], &edgeNodeSecond["edge"]["shape"]);
                            EXPECT_EQ(&edgeNodeSecond["externalShape"], &edgeNodeFirst["edge"]["shape"]);
                        }
                    }
                }
            }

            if (currentShapePoint.has("right")) {
                currentShapePointPtr = &currentShapePoint["right"];
            } else if (currentShapePoint.has("down")) {
                currentShapePointPtr = &(*firstColumnPointPtr)["down"];
                firstColumnPointPtr  = currentShapePointPtr;
            } else {
                currentShapePointPtr = nullptr;
            }
        }
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
        DEBUG(shapeIdGrid, "printShapeIdGrid");

        ScanLineState scanLineState = ScanLineState::Up;
        CellI* firstColumnPixelPtr  = firstShapePixelPtr();
        CellI* currentShapePixelPtr = firstShapePixelPtr();
        int shapesCount             = static_cast<List&>(shaper()["shapes"]).size();
        int digits                  = 1;
        if (shapesCount > 9 && shapesCount < 100) {
            digits = 2;
        } else if (shapesCount > 99) {
            digits = 3;
        }
        auto printShapeId = [&digits](CellI& shape) -> std::string {
            return fmt::format("{:>{}}", shape["id"].label(), digits);
        };

        std::stringstream ss;
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
                            ss << fmt::format("{0:-^{1}}", "", digits + 1);
                        } else if (currentShapePixel.missing("left")) {
                            ss << fmt::format("+{0:-^{1}}", "", digits);
                        } else {
                            ss << fmt::format("{0:-^{1}}", "", digits + 1);
                        }
                    } else {
                        if (currentShapePixel.missing("right")) {
                            ss << fmt::format("{0:.^{1}}", "", digits + 1);
                        } else if (currentShapePixel.missing("left")) {
                            ss << fmt::format("|{0:.^{1}}", "", digits);
                        } else {
                            if (currentShapePixel.has("left") && &currentShapePixel["left"]["shape"] != &currentShape) {
                                ss << fmt::format("+{0:.^{1}}", "", digits);
                            } else if (currentShapePixel.has("left") && currentShapePixel.has("up") && &currentShapePixel["left"]["up"]["shape"] != &currentShape) {
                                ss << fmt::format("+{0:.^{1}}", "", digits);
                            } else {
                                ss << fmt::format("{0:.^{1}}", "", digits + 1);
                            }
                        }
                    }
                } else {
                    if (currentShapePixel.missing("right")) {
                        ss << fmt::format("{0:-^{1}}", "", digits + 1);
                    } else if (currentShapePixel.missing("left")) {
                        ss << fmt::format("+{0:-^{1}}", "", digits);
                    } else {
                        ss << fmt::format("{0:-^{1}}", "", digits + 1);
                    }
                }
                if (currentShapePixel.missing("right")) {
                    if (currentShapePixel.missing("up")) {
                        ss << "+";
                    } else {
                        ss << "|";
                    }
                }
                break;
            case ScanLineState::Middle:
                if (currentShapePixel.has("left")) {
                    CellI& leftShape = currentShapePixel["left"]["shape"];
                    if (&currentShape != &leftShape) {
                        ss << "|" << printShapeId(currentShape);
                    } else {
                        ss << "." << printShapeId(currentShape);
                    }
                } else {
                    ss << "|" << printShapeId(currentShape);
                }
                if (currentShapePixel.missing("right")) {
                    ss << "|";
                }
                break;
            case ScanLineState::Down:
                if (currentShapePixel.missing("right")) {
                    ss << fmt::format("{0:-^{1}}+", "", digits + 1);
                } else if (currentShapePixel.missing("left")) {
                    ss << fmt::format("+{0:-^{1}}", "", digits);
                } else {
                    ss << fmt::format("{0:-^{1}}", "", digits + 1);
                }
                break;
            }

            if (currentShapePixelPtr->has("right")) {
                currentShapePixelPtr = &(*currentShapePixelPtr)["right"];
            } else if (scanLineState == ScanLineState::Up) {
                scanLineState        = ScanLineState::Middle;
                currentShapePixelPtr = firstColumnPixelPtr;
                TRACE(shapeIdGrid, "{}", ss.str());
                ss.str("");
            } else if (scanLineState == ScanLineState::Middle) {
                if (currentShapePixel.has("down")) {
                    scanLineState        = ScanLineState::Up;
                    currentShapePixelPtr = &(*firstColumnPixelPtr)["down"];
                    firstColumnPixelPtr  = currentShapePixelPtr;
                } else {
                    scanLineState        = ScanLineState::Down;
                    currentShapePixelPtr = firstColumnPixelPtr;
                }
                TRACE(shapeIdGrid, "{}", ss.str());
                ss.str("");
            } else if (scanLineState == ScanLineState::Down) {
                scanLineState        = ScanLineState::Up;
                currentShapePixelPtr = nullptr;
            }
        }
    }

    void printShapeIdGridAsJson()
    {
        DEBUG(shapeIdGrid, "printShapeIdGridAsJson");

        int shapesCount = static_cast<List&>(shaper()["shapes"]).size();
        int digits      = 1;
        if (shapesCount > 9 && shapesCount < 100) {
            digits = 2;
        } else if (shapesCount > 99) {
            digits = 3;
        }

        std::stringstream ss;
        TRACE(shapeIdGrid, "[");
        Object& shapePixels = static_cast<Object&>(shaper()["shapePixels"]);
        for (int y = 0; y < inputHybridGrid().height(); ++y) {
            ss << "  [";
            bool firstColumn = true;
            Object& colX = static_cast<Object&>(shapePixels.method(kb.name("getValue"), { kb.ids.key, toCellNumber(y) }));
            for (int x = 0; x < inputHybridGrid().width(); ++x) {
                if (firstColumn) {
                    firstColumn = false;
                } else {
                    ss << ", ";
                }
                firstColumn               = false;
                CellI& shapePixel         = colX.method(kb.name("getValue"), { kb.ids.key, toCellNumber(x) });
                CellI& shape              = shapePixel["shape"];
                ss << fmt::format("{:>{}}", shape["id"].label(), digits);
            }
            ss << "]";
            if (y != inputHybridGrid().height() - 1) {
                ss << ",";
            }
            TRACE(shapeIdGrid, ss.str());
            ss.str("");
        }
        TRACE(shapeIdGrid, "]");
    }

    void printEdges()
    {
        DEBUG(edge, "printEdges");

        ScanLineState scanLineState = ScanLineState::Up;
        CellI* currentShapePointPtr = &(*firstShapePixelPtr())["upLeftPoint"];
        CellI* firstColumnPointPtr  = currentShapePointPtr;
        std::stringstream ss;
        while (currentShapePointPtr) {
            CellI& shapePoint = *currentShapePointPtr;
            int pointX        = static_cast<Number&>(shapePoint["x"]).value();
            int pointY        = static_cast<Number&>(shapePoint["y"]).value();

            switch (scanLineState) {
            case ScanLineState::Up:
                if (shapePoint.has("edgeJoint")) {
                    CellI& edgeJoint = shapePoint["edgeJoint"];
                    bool hasUp       = edgeJoint.has("upLeft") || edgeJoint.has("upRight");
                    bool hasDown     = edgeJoint.has("downLeft") || edgeJoint.has("downRight");
                    bool hasLeft     = edgeJoint.has("leftUp") || edgeJoint.has("leftDown");
                    bool hasRight    = edgeJoint.has("rightUp") || edgeJoint.has("rightDown");

#if 0
                    0000 00
                    0001 01
                    0010 02
                    0011 03
                    0100 04
                    0101 05
                    0110 06
                    0111 07
                    1000 08
                    1001 09
                    1010 10
                    1011 11
                    1100 12
                    1101 13
                    1110 14
                    1111 15
#endif
                    if (!hasUp && !hasDown && !hasLeft && !hasRight) { // 0
                        throw "error";
                    } else if (!hasUp && !hasDown && !hasLeft && hasRight) { //  1
                        throw "error";
                    } else if (!hasUp && !hasDown && hasLeft && !hasRight) { //  2
                        throw "error";
                    } else if (!hasUp && !hasDown && hasLeft && hasRight) { //  3
                        ss << "─";
                    } else if (!hasUp && hasDown && !hasLeft && !hasRight) { //  4
                        throw "error";
                    } else if (!hasUp && hasDown && !hasLeft && hasRight) { //  5
                        ss << "┌";
                    } else if (!hasUp && hasDown && hasLeft && !hasRight) { //  6
                        ss << "┐";
                    } else if (!hasUp && hasDown && hasLeft && hasRight) { //  7
                        ss << "┬";
                    } else if (hasUp && !hasDown && !hasLeft && !hasRight) { //  8
                        throw "error";
                    } else if (hasUp && !hasDown && !hasLeft && hasRight) { //  9
                        ss << "└";
                    } else if (hasUp && !hasDown && hasLeft && !hasRight) { // 10
                        ss << "┘";
                    } else if (hasUp && !hasDown && hasLeft && hasRight) { // 11
                        ss << "┴";
                    } else if (hasUp && hasDown && !hasLeft && !hasRight) { // 12
                        ss << "│";
                    } else if (hasUp && hasDown && !hasLeft && hasRight) { // 13
                        ss << "├";
                    } else if (hasUp && hasDown && hasLeft && !hasRight) { // 14
                        ss << "┤";
                    } else if (hasUp && hasDown && hasLeft && hasRight) { // 15
                        ss << "┼";
                    }
                    if (!hasUp && !hasDown && !hasLeft && !hasRight) { // 0
                        throw "error";
                    } else if (!hasUp && !hasDown && !hasLeft && hasRight) { //  1
                        throw "error";
                    } else if (!hasUp && !hasDown && hasLeft && !hasRight) { //  2
                        throw "error";
                    } else if (!hasUp && !hasDown && hasLeft && hasRight) { //  3
                        ss << "──";
                    } else if (!hasUp && hasDown && !hasLeft && !hasRight) { //  4
                        throw "error";
                    } else if (!hasUp && hasDown && !hasLeft && hasRight) { //  5
                        ss << "──";
                    } else if (!hasUp && hasDown && hasLeft && !hasRight) { //  6
                        ss << "  ";
                    } else if (!hasUp && hasDown && hasLeft && hasRight) { //  7
                        ss << "──";
                    } else if (hasUp && !hasDown && !hasLeft && !hasRight) { //  8
                        throw "error";
                    } else if (hasUp && !hasDown && !hasLeft && hasRight) { //  9
                        ss << "──";
                    } else if (hasUp && !hasDown && hasLeft && !hasRight) { // 10
                        ss << "  ";
                    } else if (hasUp && !hasDown && hasLeft && hasRight) { // 11
                        ss << "──";
                    } else if (hasUp && hasDown && !hasLeft && !hasRight) { // 12
                        ss << "  ";
                    } else if (hasUp && hasDown && !hasLeft && hasRight) { // 13
                        ss << "──";
                    } else if (hasUp && hasDown && hasLeft && !hasRight) { // 14
                        ss << "  ";
                    } else if (hasUp && hasDown && hasLeft && hasRight) { // 15
                        ss << "──";
                    }
                } else {
                    ss << "   ";
                }
                break;
            case ScanLineState::Middle:
                if (shapePoint.has("edgeJoint")) {
                    CellI& edgeJoint = shapePoint["edgeJoint"];
                    bool hasUp       = edgeJoint.has("upLeft") || edgeJoint.has("upRight");
                    bool hasDown     = edgeJoint.has("downLeft") || edgeJoint.has("downRight");
                    bool hasLeft     = edgeJoint.has("leftUp") || edgeJoint.has("leftDown");
                    bool hasRight    = edgeJoint.has("rightUp") || edgeJoint.has("rightDown");
                    if (!hasUp && !hasDown && !hasLeft && !hasRight) { // 0
                        throw "error";
                    } else if (!hasUp && !hasDown && !hasLeft && hasRight) { //  1
                        throw "error";
                    } else if (!hasUp && !hasDown && hasLeft && !hasRight) { //  2
                        throw "error";
                    } else if (!hasUp && !hasDown && hasLeft && hasRight) { //  3
                        ss << "   ";
                    } else if (!hasUp && hasDown && !hasLeft && !hasRight) { //  4
                        throw "error";
                    } else if (!hasUp && hasDown && !hasLeft && hasRight) { //  5
                        ss << "│  ";
                    } else if (!hasUp && hasDown && hasLeft && !hasRight) { //  6
                        ss << "│  ";
                    } else if (!hasUp && hasDown && hasLeft && hasRight) { //  7
                        ss << "│  ";
                    } else if (hasUp && !hasDown && !hasLeft && !hasRight) { //  8
                        throw "error";
                    } else if (hasUp && !hasDown && !hasLeft && hasRight) { //  9
                        ss << "   ";
                    } else if (hasUp && !hasDown && hasLeft && !hasRight) { // 10
                        ss << "   ";
                    } else if (hasUp && !hasDown && hasLeft && hasRight) { // 11
                        ss << "   ";
                    } else if (hasUp && hasDown && !hasLeft && !hasRight) { // 12
                        ss << "│  ";
                    } else if (hasUp && hasDown && !hasLeft && hasRight) { // 13
                        ss << "│  ";
                    } else if (hasUp && hasDown && hasLeft && !hasRight) { // 14
                        ss << "│  ";
                    } else if (hasUp && hasDown && hasLeft && hasRight) { // 15
                        ss << "│  ";
                    }
                } else {
                    ss << "   ";
                }
                break;
            }

            if (shapePoint.has("right")) {
                currentShapePointPtr = &shapePoint["right"];
            } else if (shapePoint.has("down")) {
                if (scanLineState == ScanLineState::Up) {
                    currentShapePointPtr = firstColumnPointPtr;
                    scanLineState = ScanLineState::Middle;
                } else {
                    currentShapePointPtr = &(*firstColumnPointPtr)["down"];
                    firstColumnPointPtr  = currentShapePointPtr;
                    scanLineState        = ScanLineState::Up;
                }
                TRACE(edge, ss.str());
                ss.str("");
            } else {
                currentShapePointPtr = nullptr;
                TRACE(edge, ss.str());
                ss.str("");
            }
        }
    }

    void expectedShapeIds(const std::string& jsonStr)
    {
        nlohmann::json arcMatrix = nlohmann::json::parse(jsonStr);
        size_t matrixHeight      = arcMatrix.size();
        size_t matrixWidth       = arcMatrix[0].size();
        Object& shapePixels      = static_cast<Object&>(shaper()["shapePixels"]);

        int x = 0;
        int y = 0;
        for (auto inputRowIt = arcMatrix.begin(); inputRowIt != arcMatrix.end(); ++inputRowIt) {
            for (const int id : *inputRowIt) {
                CellI& currentId = kb.pools.numbers.get(id);
                Object& colX      = static_cast<Object&>(shapePixels.method(kb.name("getValue"), { kb.ids.key, kb.pools.numbers.get(y) }));
                CellI& shapePixel = colX.method(kb.name("getValue"), { kb.ids.key, kb.pools.numbers.get(x) });
                EXPECT_EQ(&shapePixel["shape"]["id"], &currentId);
                x += 1;
            }
            x = 0;
            y += 1;
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

    void findPossibleBackgroundWithShapes()
    {
        DEBUG(shapeRelations, "findPossibleBackgroundWithShapes");

        List& shapesList = static_cast<List&>(shaper()["shapes"]);
        int shapesCount  = shapesList.size();
        if (shapesCount < 2) {
            TRACE(shapeRelations, "  there isn't enough shape");
            return;
        }
        const int targetContainedShapeCount = shapesCount - 1;
        CellI* backgroundShapePtr     = nullptr;
        Visitor::visitList(shapesList, [this, targetContainedShapeCount, &backgroundShapePtr](CellI& shape, int, bool&) {
            Map& edgesMap  = static_cast<Map&>(shape["edges"]);
            int edgesCount = edgesMap.size();
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

    CellI& ShaperStruct;
    CellI& ShapeStruct;
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
    std::unique_ptr<cells::hybrid::arc::Shaper> m_hybridShaper;
    std::string m_outputSVGFileName;
};

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

    expectedShapeIds(R"([
                          [1, 1, 1, 1, 1, 1, 1, 1, 1],
                          [1, 1, 1, 1, 1, 1, 1, 1, 1],
                          [1, 1, 1, 1, 1, 1, 1, 1, 1],
                          [1, 1, 2, 1, 1, 1, 1, 1, 1],
                          [1, 1, 1, 1, 1, 1, 1, 1, 1],
                          [1, 1, 1, 1, 1, 1, 1, 1, 1],
                          [1, 1, 1, 1, 1, 1, 3, 1, 1],
                          [1, 1, 1, 1, 1, 1, 1, 1, 1],
                          [1, 1, 1, 1, 1, 1, 1, 1, 1]
                        ])");
    expectedShapesCount(3);
    expectedShapeEdgeCounts({ { 1, 3 }, { 2, 1 }, { 3, 1 } });
}

TEST_F(EdgeTester, EdgeTestWithArc_0ca9ddb6_Train1Output)
{
    testEdges(R"([[0,0,0,0,0,0,0,0,0],
                  [0,0,0,0,0,0,0,0,0],
                  [0,4,0,4,0,0,0,0,0],
                  [0,0,2,0,0,0,0,0,0],
                  [0,4,0,4,0,0,0,0,0],
                  [0,0,0,0,0,0,7,0,0],
                  [0,0,0,0,0,7,1,7,0],
                  [0,0,0,0,0,0,7,0,0],
                  [0,0,0,0,0,0,0,0,0]])");
#if 0
  The input grid contains
     - Pixel { red, x=2, y=3 }
     - Pixel { blue, x=6, y=6 }

shape id 1
    edge id 2 internal and contains: shape2 { Pixel{red, x=2, y=3}}
    edge id 3 internal and contains: shape3 { Pixel{blue, x=6, y=6}}

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

shape id 1
    edge id 2 contains: shape(2) shape(3) shape(4) shape(5) shape(6)
    edge id 3 contains: shape(7)
shape id 7
    edge id 2 contains: shape(8)
#endif

    expectedShapeIds(R"([
                          [ 1,  1,  1,  1,  1,  1,  1,  1,  1],
                          [ 1,  1,  1,  1,  1,  1,  1,  1,  1],
                          [ 1,  2,  1,  3,  1,  1,  1,  1,  1],
                          [ 1,  1,  4,  1,  1,  1,  1,  1,  1],
                          [ 1,  5,  1,  6,  1,  1,  1,  1,  1],
                          [ 1,  1,  1,  1,  1,  1,  7,  1,  1],
                          [ 1,  1,  1,  1,  1,  8,  9, 10,  1],
                          [ 1,  1,  1,  1,  1,  1, 11,  1,  1],
                          [ 1,  1,  1,  1,  1,  1,  1,  1,  1]
                        ])");
    expectedShapesCount(11);
    expectedShapeEdgeCounts({ { 1, 3 }, { 2, 1 }, { 3, 1 }, { 4, 1 }, { 5, 1 }, { 6, 1 }, { 7, 1 }, { 8, 1 }, { 9, 1 }, { 10, 1 }, { 11, 1 } });
}

TEST_F(EdgeTester, EdgeTestWithArc_0ca9ddb6_Train2Input)
{
    testEdges(R"([[0,0,0,8,0,0,0,0,0],
                  [0,0,0,0,0,0,0,0,0],
                  [0,0,0,0,0,0,2,0,0],
                  [0,0,1,0,0,0,0,0,0],
                  [0,0,0,0,0,0,0,0,0],
                  [0,0,0,0,0,0,0,0,0],
                  [0,0,0,0,0,0,1,0,0],
                  [0,2,0,0,0,0,0,0,0],
                  [0,0,0,0,0,0,0,0,0]])");

    expectedShapeIds(R"([
                          [1, 1, 1, 2, 1, 1, 1, 1, 1],
                          [1, 1, 1, 1, 1, 1, 1, 1, 1],
                          [1, 1, 1, 1, 1, 1, 3, 1, 1],
                          [1, 1, 4, 1, 1, 1, 1, 1, 1],
                          [1, 1, 1, 1, 1, 1, 1, 1, 1],
                          [1, 1, 1, 1, 1, 1, 1, 1, 1],
                          [1, 1, 1, 1, 1, 1, 5, 1, 1],
                          [1, 6, 1, 1, 1, 1, 1, 1, 1],
                          [1, 1, 1, 1, 1, 1, 1, 1, 1]
                        ])");
    expectedShapesCount(6);
    expectedShapeEdgeCounts({ { 1, 5 }, { 2, 1 }, { 3, 1 }, { 4, 1 }, { 5, 1 }, { 6, 1 } });
}

TEST_F(EdgeTester, EdgeTestWithArc_0ca9ddb6_Train2Output)
{
    testEdges(R"([[0,0,0,8,0,0,0,0,0],
                  [0,0,0,0,0,4,0,4,0],
                  [0,0,7,0,0,0,2,0,0],
                  [0,7,1,7,0,4,0,4,0],
                  [0,0,7,0,0,0,0,0,0],
                  [0,0,0,0,0,0,7,0,0],
                  [4,0,4,0,0,7,1,7,0],
                  [0,2,0,0,0,0,7,0,0],
                  [4,0,4,0,0,0,0,0,0]])");

    expectedShapesCount(24);
}

TEST_F(EdgeTester, EdgeTestWithArc_0ca9ddb6_Train3Input)
{
    testEdges(R"([[0,0,0,0,0,0,0,0,0],
                  [0,0,0,0,0,0,0,0,0],
                  [0,0,2,0,0,0,0,0,0],
                  [0,0,0,0,0,0,0,0,0],
                  [0,0,0,0,0,0,0,0,0],
                  [0,0,0,0,0,0,6,0,0],
                  [0,0,0,0,0,0,0,0,0],
                  [0,0,0,1,0,0,0,0,0],
                  [0,0,0,0,0,0,0,0,0]])");
}

TEST_F(EdgeTester, EdgeTestWithArc_0ca9ddb6_Train3Output)
{
    testEdges(R"([[0,0,0,0,0,0,0,0,0],
                  [0,4,0,4,0,0,0,0,0],
                  [0,0,2,0,0,0,0,0,0],
                  [0,4,0,4,0,0,0,0,0],
                  [0,0,0,0,0,0,0,0,0],
                  [0,0,0,0,0,0,6,0,0],
                  [0,0,0,7,0,0,0,0,0],
                  [0,0,7,1,7,0,0,0,0],
                  [0,0,0,7,0,0,0,0,0]])");

}

TEST_F(EdgeTester, EdgeTestWithArc_0ca9ddb6_Test1Input)
{
    testEdges(R"([[0,0,0,0,0,0,0,0,0],
                  [0,0,0,0,0,0,0,0,0],
                  [0,0,0,0,0,0,1,0,0],
                  [0,0,2,0,0,0,0,0,0],
                  [0,0,0,0,0,0,0,0,0],
                  [0,0,0,0,0,8,0,0,0],
                  [0,0,0,0,0,0,0,0,0],
                  [0,6,0,0,0,0,0,2,0],
                  [0,0,0,0,0,0,0,0,0]])");
}

TEST_F(EdgeTester, EdgeTestWithArc_2dd70a9a_Test1Input)
{
    exit(0);
    testEdges(R"([[8,8,8,8,0,0,0,0,0,8,8,0,0],
                  [8,0,0,0,0,8,2,2,0,0,0,0,0],
                  [0,8,0,0,8,8,0,0,0,0,0,0,0],
                  [0,0,8,0,0,0,0,0,8,0,0,0,8],
                  [0,0,8,0,0,0,8,0,0,0,0,0,8],
                  [0,0,0,8,0,0,0,0,8,0,8,0,0],
                  [0,0,0,0,0,0,0,0,0,0,8,0,0],
                  [8,0,8,3,3,0,0,0,0,0,8,0,0],
                  [0,8,8,0,0,8,0,0,0,0,8,0,0],
                  [0,0,0,0,0,0,8,8,0,0,0,0,0],
                  [0,8,8,0,0,0,8,0,0,0,0,0,0],
                  [0,0,0,8,0,0,0,0,0,0,0,0,0],
                  [0,0,0,0,0,0,0,0,0,0,0,8,0]])");
}

TEST_F(EdgeTester, EdgeTestWithArc_00d62c1b_Train5Input)
{
    testEdges(R"([[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],
                  [0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0],
                  [0,0,0,0,3,3,3,3,0,3,3,0,0,0,0,0,0,0,0,0],
                  [0,0,0,0,0,0,0,0,3,0,3,0,0,0,0,0,0,0,3,0],
                  [0,0,0,0,0,0,0,0,3,3,3,3,3,3,3,3,0,0,0,0],
                  [0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,3,0,0,0,0],
                  [0,0,0,0,3,0,0,0,3,0,0,0,0,0,0,3,0,0,0,0],
                  [0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,3,0,0,0,0],
                  [0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,3,0,0,0,0],
                  [0,0,3,0,0,0,0,0,3,3,3,3,3,3,3,3,0,0,0,0],
                  [0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0],
                  [0,0,0,0,0,0,0,0,3,3,3,0,0,0,0,3,0,3,0,0],
                  [0,0,0,0,0,0,3,3,0,0,3,0,0,3,0,0,0,0,0,0],
                  [0,0,0,0,0,0,0,3,0,0,3,3,0,0,3,0,0,3,0,0],
                  [0,0,0,0,0,0,0,3,3,3,3,0,3,0,0,3,3,3,0,0],
                  [0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,3,0,3,0,0],
                  [0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,3,3,3,0,0],
                  [0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0],
                  [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],
                  [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]])");
}

TEST_F(EdgeTester, EdgeTestWithArc_00d62c1b_Train5Output)
{
    testEdges(R"([[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],
                  [0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0],
                  [0,0,0,0,3,3,3,3,4,3,3,0,0,0,0,0,0,0,0,0],
                  [0,0,0,0,0,0,0,0,3,4,3,0,0,0,0,0,0,0,3,0],
                  [0,0,0,0,0,0,0,0,3,3,3,3,3,3,3,3,0,0,0,0],
                  [0,0,0,0,0,0,0,0,3,4,4,4,4,4,4,3,0,0,0,0],
                  [0,0,0,0,3,0,0,0,3,4,4,4,4,4,4,3,0,0,0,0],
                  [0,0,0,0,0,0,0,0,3,4,4,4,4,4,4,3,0,0,0,0],
                  [0,0,0,0,0,0,0,0,3,4,4,4,4,4,4,3,0,0,0,0],
                  [0,0,3,0,0,0,0,0,3,3,3,3,3,3,3,3,0,0,0,0],
                  [0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0],
                  [0,0,0,0,0,0,0,0,3,3,3,0,0,0,0,3,0,3,0,0],
                  [0,0,0,0,0,0,3,3,4,4,3,0,0,3,0,0,0,0,0,0],
                  [0,0,0,0,0,0,0,3,4,4,3,3,0,0,3,0,0,3,0,0],
                  [0,0,0,0,0,0,0,3,3,3,3,0,3,0,0,3,3,3,0,0],
                  [0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,3,4,3,0,0],
                  [0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,3,3,3,0,0],
                  [0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0],
                  [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],
                  [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]])");
}

TEST_F(EdgeTester, EdgeTestWithArc_0b148d64_minified_Train1Input)
{
    testEdges(R"([[8,8,8,8,8,0,8,8,8,8,0],
                  [8,0,0,8,0,8,0,8,8,8,0],
                  [8,8,8,0,0,0,8,8,8,8,0],
                  [8,8,0,8,8,8,8,0,8,8,0],
                  [8,8,8,8,0,8,8,0,8,8,0],
                  [0,0,0,8,8,0,8,0,0,8,0],
                  [8,8,8,8,0,0,8,0,8,0,0],
                  [8,0,0,8,0,0,8,8,0,8,0],
                  [8,8,8,8,8,8,0,8,0,0,0],
                  [0,0,0,0,0,0,0,0,0,0,0]])");
}

TEST_F(EdgeTester, EdgeTestCase7Horizontal)
{
    testEdges(R"([[0,0,0,0,0],
                  [0,0,1,0,0],
                  [0,1,8,1,0],
                  [0,0,1,0,0],
                  [0,0,0,0,0]])");
}

TEST_F(EdgeTester, EdgeTestInternalEdges)
{
    testEdges(R"([[0,0,0,0,0,0,0],
                  [0,1,1,1,1,1,0],
                  [0,1,8,8,8,1,0],
                  [0,1,8,2,8,1,0],
                  [0,1,8,8,8,1,0],
                  [0,1,1,1,1,1,0],
                  [0,0,0,0,0,0,0]])");
}

TEST_F(EdgeTester, EdgeTestMinimal)
{
    testEdges(R"([[0, 7, 7],
                  [7, 7, 7],
                  [7, 7, 7]])");
    CellI& shape        = static_cast<Object&>(shaper()["shapeMap"]).method(kb.name("getValue"), { kb.ids.key, kb.pools.numbers.get(2) });
    Map& shapeEdges     = static_cast<Map&>(shape["edges"]);
    CellI& externalEdge = shapeEdges.getValue(_1_);

    CellI* firstColumnPointPtr  = &(*firstShapePixelPtr())["upLeftPoint"];
    CellI* currentShapePointPtr = firstColumnPointPtr;
    CellI* previousShapePointPtr = nullptr;

    EXPECT_TRUE((*currentShapePointPtr).has("edgeJoint"));
    EXPECT_EQ(&(*currentShapePointPtr)["x"], &_0_);
    EXPECT_EQ(&(*currentShapePointPtr)["y"], &_0_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("upLeft"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("upRight"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("downRight"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("downLeft"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["downRight"]["edge"]["shape"]["id"], &_1_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("leftUp"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("leftDown"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("rightUp"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("rightDown"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["rightDown"]["edge"]["shape"]["id"], &_1_);

    previousShapePointPtr = currentShapePointPtr;
    currentShapePointPtr = &(*currentShapePointPtr)["right"];
    EXPECT_EQ(&(*currentShapePointPtr)["x"], &_1_);
    EXPECT_EQ(&(*currentShapePointPtr)["y"], &_0_);
    EXPECT_TRUE((*currentShapePointPtr).has("edgeJoint"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("upLeft"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("upRight"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("downLeft"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["downLeft"]["edge"]["shape"]["id"], &_1_);
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("downRight"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["downRight"]["edge"]["shape"]["id"], &_2_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("leftUp"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("leftDown"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["leftDown"]["from"], previousShapePointPtr);
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["leftDown"]["direction"], &DirectionRightEV);
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["leftDown"]["edge"]["shape"]["id"], &_1_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("rightUp"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("rightDown"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["rightDown"]["from"], currentShapePointPtr);
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["rightDown"]["direction"], &DirectionRightEV);
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["rightDown"]["edge"]["shape"]["id"], &_2_);

    previousShapePointPtr = currentShapePointPtr;
    currentShapePointPtr  = &(*currentShapePointPtr)["right"];
    EXPECT_EQ(&(*currentShapePointPtr)["x"], &_2_);
    EXPECT_EQ(&(*currentShapePointPtr)["y"], &_0_);
    EXPECT_TRUE((*currentShapePointPtr).has("edgeJoint"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("upLeft"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("upRight"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("downLeft"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("downRight"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("leftUp"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("leftDown"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["leftDown"]["edge"]["shape"]["id"], &_2_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("rightUp"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("rightDown"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["rightDown"]["from"], currentShapePointPtr);
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["rightDown"]["direction"], &DirectionRightEV);
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["rightDown"]["edge"]["shape"]["id"], &_2_);

    previousShapePointPtr = currentShapePointPtr;
    currentShapePointPtr  = &(*currentShapePointPtr)["right"];
    EXPECT_EQ(&(*currentShapePointPtr)["x"], &_3_);
    EXPECT_EQ(&(*currentShapePointPtr)["y"], &_0_);
    EXPECT_TRUE((*currentShapePointPtr).has("edgeJoint"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("upLeft"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("upRight"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("downLeft"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["downLeft"]["edge"]["shape"]["id"], &_2_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("downRight"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("leftUp"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("leftDown"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["leftDown"]["edge"]["shape"]["id"], &_2_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("rightUp"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("rightDown"));

    previousShapePointPtr = nullptr;
    currentShapePointPtr  = &(*firstColumnPointPtr)["down"];
    firstColumnPointPtr   = currentShapePointPtr;
    EXPECT_EQ(&(*currentShapePointPtr)["x"], &_0_);
    EXPECT_EQ(&(*currentShapePointPtr)["y"], &_1_);
    EXPECT_TRUE((*currentShapePointPtr).has("edgeJoint"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("upLeft"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("upRight"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["upRight"]["edge"]["shape"]["id"], &_1_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("downLeft"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("downRight"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["downRight"]["edge"]["shape"]["id"], &_2_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("leftUp"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("leftDown"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("rightUp"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["rightUp"]["edge"]["shape"]["id"], &_1_);
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("rightDown"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["rightDown"]["edge"]["shape"]["id"], &_2_);

    previousShapePointPtr = currentShapePointPtr;
    currentShapePointPtr  = &(*currentShapePointPtr)["right"];
    EXPECT_EQ(&(*currentShapePointPtr)["x"], &_1_);
    EXPECT_EQ(&(*currentShapePointPtr)["y"], &_1_);
    EXPECT_TRUE((*currentShapePointPtr).has("edgeJoint"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("upLeft"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["upLeft"]["edge"]["shape"]["id"], &_1_);
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("upRight"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["upRight"]["edge"]["shape"]["id"], &_2_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("downLeft"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("downRight"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("leftUp"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["leftUp"]["edge"]["shape"]["id"], &_1_);
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("leftDown"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["leftDown"]["edge"]["shape"]["id"], &_2_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("rightUp"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("rightDown"));

    previousShapePointPtr = currentShapePointPtr;
    currentShapePointPtr  = &(*currentShapePointPtr)["right"];
    EXPECT_EQ(&(*currentShapePointPtr)["x"], &_2_);
    EXPECT_EQ(&(*currentShapePointPtr)["y"], &_1_);
    EXPECT_FALSE((*currentShapePointPtr).has("edgeJoint"));

    previousShapePointPtr = currentShapePointPtr;
    currentShapePointPtr  = &(*currentShapePointPtr)["right"];
    EXPECT_EQ(&(*currentShapePointPtr)["x"], &_3_);
    EXPECT_EQ(&(*currentShapePointPtr)["y"], &_1_);
    EXPECT_TRUE((*currentShapePointPtr).has("edgeJoint"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("upLeft"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["upLeft"]["edge"]["shape"]["id"], &_2_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("upRight"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("downLeft"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["downLeft"]["edge"]["shape"]["id"], &_2_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("downRight"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("leftUp"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("leftDown"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("rightUp"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("rightDown"));

    previousShapePointPtr = nullptr;
    currentShapePointPtr  = &(*firstColumnPointPtr)["down"];
    firstColumnPointPtr   = currentShapePointPtr;
    EXPECT_EQ(&(*currentShapePointPtr)["x"], &_0_);
    EXPECT_EQ(&(*currentShapePointPtr)["y"], &_2_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("upLeft"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("upRight"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["upRight"]["edge"]["shape"]["id"], &_2_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("downLeft"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("downRight"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["downRight"]["edge"]["shape"]["id"], &_2_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("leftUp"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("leftDown"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("rightUp"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("rightDown"));

    previousShapePointPtr = currentShapePointPtr;
    currentShapePointPtr  = &(*currentShapePointPtr)["right"];
    EXPECT_EQ(&(*currentShapePointPtr)["x"], &_1_);
    EXPECT_EQ(&(*currentShapePointPtr)["y"], &_2_);
    EXPECT_FALSE((*currentShapePointPtr).has("edgeJoint"));

    previousShapePointPtr = currentShapePointPtr;
    currentShapePointPtr  = &(*currentShapePointPtr)["right"];
    EXPECT_EQ(&(*currentShapePointPtr)["x"], &_2_);
    EXPECT_EQ(&(*currentShapePointPtr)["y"], &_2_);
    EXPECT_FALSE((*currentShapePointPtr).has("edgeJoint"));

    previousShapePointPtr = currentShapePointPtr;
    currentShapePointPtr  = &(*currentShapePointPtr)["right"];
    EXPECT_EQ(&(*currentShapePointPtr)["x"], &_3_);
    EXPECT_EQ(&(*currentShapePointPtr)["y"], &_2_);
    EXPECT_TRUE((*currentShapePointPtr).has("edgeJoint"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("upLeft"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["upLeft"]["edge"]["shape"]["id"], &_2_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("upRight"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("downLeft"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["downLeft"]["edge"]["shape"]["id"], &_2_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("downRight"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("leftUp"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("leftDown"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("rightUp"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("rightDown"));

    previousShapePointPtr = nullptr;
    currentShapePointPtr  = &(*firstColumnPointPtr)["down"];
    firstColumnPointPtr   = currentShapePointPtr;
    EXPECT_EQ(&(*currentShapePointPtr)["x"], &_0_);
    EXPECT_EQ(&(*currentShapePointPtr)["y"], &_3_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("upLeft"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("upRight"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["upRight"]["edge"]["shape"]["id"], &_2_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("downLeft"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("downRight"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("leftUp"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("leftDown"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("rightUp"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["rightUp"]["edge"]["shape"]["id"], &_2_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("rightDown"));

    previousShapePointPtr = currentShapePointPtr;
    currentShapePointPtr  = &(*currentShapePointPtr)["right"];
    EXPECT_EQ(&(*currentShapePointPtr)["x"], &_1_);
    EXPECT_EQ(&(*currentShapePointPtr)["y"], &_3_);
    EXPECT_TRUE((*currentShapePointPtr).has("edgeJoint"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("upLeft"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("upRight"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("downLeft"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("downRight"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("leftUp"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["leftUp"]["edge"]["shape"]["id"], &_2_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("leftDown"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("rightUp"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["rightUp"]["edge"]["shape"]["id"], &_2_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("rightDown"));

    previousShapePointPtr = currentShapePointPtr;
    currentShapePointPtr  = &(*currentShapePointPtr)["right"];
    EXPECT_EQ(&(*currentShapePointPtr)["x"], &_2_);
    EXPECT_EQ(&(*currentShapePointPtr)["y"], &_3_);
    EXPECT_TRUE((*currentShapePointPtr).has("edgeJoint"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("upLeft"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("upRight"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("downLeft"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("downRight"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("leftUp"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["leftUp"]["edge"]["shape"]["id"], &_2_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("leftDown"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("rightUp"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["rightUp"]["edge"]["shape"]["id"], &_2_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("rightDown"));

    previousShapePointPtr = currentShapePointPtr;
    currentShapePointPtr  = &(*currentShapePointPtr)["right"];
    EXPECT_EQ(&(*currentShapePointPtr)["x"], &_3_);
    EXPECT_EQ(&(*currentShapePointPtr)["y"], &_3_);
    EXPECT_TRUE((*currentShapePointPtr).has("edgeJoint"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("upLeft"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["upLeft"]["edge"]["shape"]["id"], &_2_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("upRight"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("downLeft"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("downRight"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("leftUp"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["leftUp"]["edge"]["shape"]["id"], &_2_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("leftDown"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("rightUp"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("rightDown"));

    expectedShapeIds(R"([
                          [1, 2, 2],
                          [2, 2, 2],
                          [2, 2, 2]
                        ])");
    expectedShapeEdgeCounts({ { 1, 1 }, { 2, 1 } });
}

TEST_F(EdgeTester, EdgeTest)
{
    testEdges(R"([[0, 0, 0, 0, 0],
                  [0, 4, 0, 4, 0],
                  [0, 0, 2, 0, 0],
                  [0, 4, 0, 4, 0],
                  [0, 0, 0, 0, 0]])");
    CellI& shape    = static_cast<Object&>(shaper()["shapeMap"]).method(kb.name("getValue"), { kb.ids.key, kb.pools.numbers.get(1) });
    Map& shapeEdges = static_cast<Map&>(shape["edges"]);
    CellI& internalEdge = shapeEdges.getValue(_2_);
    List& edgeNodes     = static_cast<List&>(internalEdge["edgeNodes"]);
    expectedShapeIds(R"([
                          [1, 1, 1, 1, 1],
                          [1, 2, 1, 3, 1],
                          [1, 1, 4, 1, 1],
                          [1, 5, 1, 6, 1],
                          [1, 1, 1, 1, 1]
                        ])");
    expectedShapeEdgeCounts({ { 1, 2 }, { 2, 1 }, { 3, 1 }, { 4, 1 }, { 5, 1 }, { 6, 1 } });
}

TEST_F(EdgeTester, EdgeTestTwoLeftCorners)
{
    testEdges(R"([[0, 7, 7, 7],
                  [7, 7, 7, 7],
                  [0, 7, 7, 7]])");

    expectedShapeIds(R"([
                          [1, 2, 2, 2],
                          [2, 2, 2, 2],
                          [3, 2, 2, 2]
                        ])");
    expectedShapesCount(3);
    expectedShapeEdgeCounts({ { 1, 1 }, { 2, 1 }, { 3, 1 } });
}

TEST_F(EdgeTester, EdgeTestWithFourCorners)
{
    testEdges(R"([[0, 7, 7, 0],
                  [7, 7, 7, 7],
                  [0, 7, 7, 0]])");

    expectedShapeIds(R"([
                          [1, 2, 2, 3],
                          [2, 2, 2, 2],
                          [4, 2, 2, 5]
                        ])");
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

    expectedShapeIds(R"([
                          [1, 2, 2, 2, 2],
                          [3, 4, 2, 2, 2],
                          [3, 3, 5, 2, 2],
                          [3, 3, 3, 6, 2],
                          [3, 3, 3, 3, 7]
                        ])");
    expectedShapesCount(7);
    expectedShapeEdgeCounts({ { 1, 1 }, { 2, 1 }, { 3, 1 }, { 4, 1 }, { 5, 1 }, { 6, 1 }, { 7, 1 } });
}

TEST_F(EdgeTester, EdgeTestWithLineDiagonalFromUpRight)
{
    testEdges(R"([[0, 0, 0, 0, 7],
                  [0, 0, 0, 7, 0],
                  [0, 0, 7, 0, 0],
                  [0, 7, 0, 0, 0],
                  [7, 0, 0, 0, 0]])");

    expectedShapeIds(R"([
                          [1, 1, 1, 1, 2],
                          [1, 1, 1, 3, 4],
                          [1, 1, 5, 4, 4],
                          [1, 6, 4, 4, 4],
                          [7, 4, 4, 4, 4]
                        ])");
    expectedShapesCount(7);
    expectedShapeEdgeCounts({ { 1, 1 }, { 2, 1 }, { 3, 1 }, { 4, 1 }, { 5, 1 }, { 6, 1 }, { 7, 1 } });
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

TEST_F(EdgeTester, DISABLED_EdgeTestWithAllArcTask)
{
    TaskSet taskSet(kb, INFOCELL_ARCPRIZE_PATH INFOCELL_ARC_PRIZE_TRAINING_CHALLENGES_FILENAME);
//    TaskSet taskSet(kb, INFOCELL_ARCPRIZE_PATH INFOCELL_ARC_PRIZE_EVALUATION_CHALLENGES_FILENAME);
    for (auto& task : taskSet.m_tasks) {
        INFO(grid, fmt::format("id: {}, examples num: {}, tests num: {}", task.first, static_cast<List&>(task.second.m_cellExamplesList).size(), static_cast<List&>(task.second.m_cellTestsList).size()));
        TRACE(grid, "   examples:");
        Visitor::visitList(task.second.m_cellExamplesList, [this, &task](CellI& example, int i, bool&) {
            const int humanIndex = i + 1;
            INFO(grid, fmt::format("id: {}, example input: {}", task.first, humanIndex));
            TRACE(grid, fmt::format("id: {}, example input: {}", task.first, humanIndex));
            TRACE(grid, fmt::format("id: {}, example input: {}", task.first, humanIndex));
            setOutputSVGName(fmt::format("EdgeTestWithArc_{}_{}{}{}", task.first, "Train", humanIndex, "Input"));
            testEdges(static_cast<hybrid::arc::Grid&>(example["input"]));
            INFO(grid, fmt::format("id: {}, example output: {}", task.first, humanIndex));
            TRACE(grid, fmt::format("id: {}, example output: {}", task.first, humanIndex));
            TRACE(grid, fmt::format("id: {}, example output: {}", task.first, humanIndex));
            setOutputSVGName(fmt::format("EdgeTestWithArc_{}_{}{}{}", task.first, "Train", humanIndex, "Output"));
            testEdges(static_cast<hybrid::arc::Grid&>(example["output"]));

        });
        TRACE(grid, "   tests:");
        Visitor::visitList(task.second.m_cellTestsList, [this, &task](CellI& example, int i, bool&) {
            const int humanIndex = i + 1;
            INFO(grid, fmt::format("id: {}, test input: {}", task.first, humanIndex));
            setOutputSVGName(fmt::format("EdgeTestWithArc_{}_{}{}{}", task.first, "Test", humanIndex, "Input"));
            testEdges(static_cast<hybrid::arc::Grid&>(example["input"]));
        });
    }
}

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

#ifdef _WIN32
#include "Windows.h"
#endif

int main(int argc, char** argv)
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    DEBUG(cells, "Constructed: {}, destructed: {}, live: {}", CellI::s_constructed, CellI::s_destructed, CellI::s_constructed - CellI::s_destructed);

    return ret;
}