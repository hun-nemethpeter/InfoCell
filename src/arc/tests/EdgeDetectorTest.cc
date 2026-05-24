#include "arc/hybridcells/Frame.h"
#include "arc/hybridcells/Grid.h"
#include "arc/Grid.h"
#include "arc/Observers.h"

#include "cells/tests/CellTestBase.h"

#include "Config.h"
#include "arc/EdgeDetector.h"
#include "arc/Task.h"

#include <fstream>
#include <ftxui/component/component.hpp>
#include <nlohmann/json.hpp>

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include "util/Log.h"
#include <fmt/color.h>

using namespace infocell;
using namespace infocell::cells;
using infocell::cells::test::TestBase;
using nlohmann::json;

namespace nativearc = infocell::arc::native;
namespace hybridarc = infocell::cells::arc;

static spdlog::logger* s_logger = nullptr;

namespace infocell {
class EdgeDetectorTest : public TestBase,
                         public arc::EdgeDetector
{
public:
    EdgeDetectorTest() :
        arc::EdgeDetector(getKb())
    {
    }

    void setOutputSVGName(const std::string& fileName)
    {
        m_outputSVGFileName = fileName;
    }

    void frameProcess() override
    {
        printInputHybridGrid();
        EdgeDetector::frameProcess();
    }

    void sortShapePixelsAndCreateShapePoints() override
    {
        EdgeDetector::sortShapePixelsAndCreateShapePoints();
        // printEveryShapePixels();
        printAndTestShapePixels();
    }

    void sortShapePoints() override
    {
        EdgeDetector::sortShapePoints();
        // printAllShapePoints();
    }

    void calculateEdgesForShapes() override
    {
        EdgeDetector::calculateEdgesForShapes();
    }

    void processEdgeNodes() override
    {
        EdgeDetector::processEdgeNodes();
        validateEdgePoints();
        // drawSvgFromShapePointEdgeJoints();
        printShapeIdGrid();
        printShapeIdGridAsJson();
        // printEdges();
        printShapeRelations();
        findPossibleBackgroundWithShapes();
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
        hybridarc::Grid& grid = inputHybridGrid();
        ftxui::Elements boardLines;
        for (int y = 0; y < grid.height(); ++y) {
            std::stringstream ss;
            for (int x = 0; x < grid.width(); ++x) {
                hybridarc::Pixel& pixel = grid.getPixel(x, y);
                ss << colorTile(pixel.color());
            }
            TRACE(grid, ss.str());
        }
    }

    void printShapeRelations()
    {
        DEBUG(shapeRelations, "printShapeRelations");
        Visitor::visitList(frame()["shapes"], [this](CellI& shape, int, bool&) {
            int edgesCount = getShapeEdgesSize(shape);
            if (edgesCount == 1) {
                TRACE(shapeRelations, "  shape id {} has only external edge", shape["id"].label());
            } else {
                TRACE(shapeRelations, "  shape id {} has internal edge(s)", shape["id"].label());
                Visitor::visitList(shape["edges"]["list"], [this](CellI& edge, int, bool&) {
                    if (&edge["kind"] == &InternalEdgeEV) {
                        if (edge.has("shapes")) {
                            std::stringstream ss;
                            Visitor::visitList(edge["shapes"]["index"]["struct"]["slots"]["list"], [this, &ss](CellI& slot, int, bool&) {
                                CellI& shape = slot["key"];
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

    CellI& getEdgeFromShape(CellI& shape, CellI& edgeId)
    {
        if (m_hybridFrame) {
            return static_cast<Map&>(shape["edges"]).getValue(edgeId);
        } else {
            Object& map = static_cast<Object&>(shape["edges"]);
            return map.method(kb.name("getValue"), { ids.key, edgeId });
        }
    }

    int getShapeEdgesSize(CellI& shape)
    {
        if (m_hybridFrame) {
            return static_cast<Map&>(shape["edges"]).size();
        } else {
            Object& map = static_cast<Object&>(shape["edges"]);
            return static_cast<Number&>(map.method(kb.name("size"))).value();
        }
    }

    void printEveryShapePixels()
    {
        TRACE(edge, "printEveryShapePixels");
        Visitor::visitList(frame()["shapes"], [this](CellI& currentShape, int, bool&) {
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
            hybridarc::Pixel& currentArcPixel = static_cast<hybridarc::Pixel&>((*currentShapePixelPtr)["pixel"]);
            const int x                         = currentArcPixel.m_x.value();
            const int y                         = currentArcPixel.m_y.value();
            EXPECT_EQ(x, referenceX);
            EXPECT_EQ(y, referenceY);
            ss << "[" << x << "," << y << "]";
            if (x == 0) {
                EXPECT_FALSE(currentShapePixelPtr->has("left"));
            } else {
                hybridarc::Pixel& otherArcPixel = static_cast<hybridarc::Pixel&>((*currentShapePixelPtr)["left"]["pixel"]);
                EXPECT_EQ(x - 1, otherArcPixel.m_x.value());
            }
            if (x == inputHybridGrid().width() - 1) {
                EXPECT_FALSE(currentShapePixelPtr->has("right"));
                TRACE(edge, ss.str());
                ss.str("");
                ss << "  ";
            } else {
                hybridarc::Pixel& otherArcPixel = static_cast<hybridarc::Pixel&>((*currentShapePixelPtr)["right"]["pixel"]);
                EXPECT_EQ(x + 1, otherArcPixel.m_x.value());
            }
            if (y == 0) {
                EXPECT_FALSE(currentShapePixelPtr->has("up"));
            } else {
                hybridarc::Pixel& otherArcPixel = static_cast<hybridarc::Pixel&>((*currentShapePixelPtr)["up"]["pixel"]);
                EXPECT_EQ(y - 1, otherArcPixel.m_y.value());
            }
            if (y == inputHybridGrid().height() - 1) {
                EXPECT_FALSE(currentShapePixelPtr->has("down"));
            } else {
                hybridarc::Pixel& otherArcPixel = static_cast<hybridarc::Pixel&>((*currentShapePixelPtr)["down"]["pixel"]);
                EXPECT_EQ(y + 1, otherArcPixel.m_y.value());
            }
            if (currentShapePixelPtr->has("right")) {
                currentShapePixelPtr = &(*currentShapePixelPtr)["right"];
                referenceX += 1;
            } else if (firstColumnPixelPtr->has("down")) {
                currentShapePixelPtr = &(*firstColumnPixelPtr)["down"];
                firstColumnPixelPtr  = currentShapePixelPtr;
                referenceX           = 0;
                referenceY += 1;
            } else {
                currentShapePixelPtr = nullptr;
            }
        }

        Object& shapePixels = static_cast<Object&>(frame()["shapePixels"]);
        for (int y = 0; y < inputHybridGrid().height(); ++y) {
            Object& colX = static_cast<Object&>(shapePixels.method(kb.name("getValue"), { kb.ids.key, toCellNumber(y) }));
            for (int x = 0; x < inputHybridGrid().width(); ++x) {
                CellI& shapePixel         = colX.method(kb.name("getValue"), { kb.ids.key, toCellNumber(x) });
                CellI& shape              = shapePixel["shape"];
                hybridarc::Pixel& pixel = static_cast<hybridarc::Pixel&>(shapePixel["pixel"]);
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
            CellI& shapePoint = *currentShapePointPtr;
            int pointX        = static_cast<Number&>(shapePoint["x"]).value();
            int pointY        = static_cast<Number&>(shapePoint["y"]).value();
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
                    const int arrowLength     = 3;

                    CellI& edgeNode       = edgeJoint[edgeJointSlotName];
                    CellI& direction      = edgeNode["direction"];
                    CellI& edgeShapeColor = edgeNode["edge"]["shape"]["color"];
                    CellI& colorTag       = edgeShapeColor["tag"];
                    CellI& colorValue     = edgeShapeColor[colorTag];
                    int color             = static_cast<Number&>(colorValue).value();
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
                hybridarc::Pixel& pixel = static_cast<hybridarc::Pixel&>(shapePoint["downRightPixel"]["pixel"]);
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

    void printAllShapePoints()
    {
        DEBUG(edge, "printAllShapePoints");
        Visitor::visitList(frame()["shapes"], [this](CellI& currentShape, int, bool&) {
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
                            CellI& edgeNodeFirst  = edgeJoint[directionPair.first];
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

    void printShapeIdGrid()
    {
        DEBUG(shapeIdGrid, "printShapeIdGrid");

        ScanLineState scanLineState = ScanLineState::Up;
        CellI* firstColumnPixelPtr  = firstShapePixelPtr();
        CellI* currentShapePixelPtr = firstShapePixelPtr();
        int shapesCount             = static_cast<List&>(frame()["shapes"]).size();
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
            hybridarc::Pixel& currentArcPixel = static_cast<hybridarc::Pixel&>(currentShapePixel["pixel"]);
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

        int shapesCount = static_cast<List&>(frame()["shapes"]).size();
        int digits      = 1;
        if (shapesCount > 9 && shapesCount < 100) {
            digits = 2;
        } else if (shapesCount > 99) {
            digits = 3;
        }

        std::stringstream ss;
        TRACE(shapeIdGrid, "[");
        Object& shapePixels = static_cast<Object&>(frame()["shapePixels"]);
        for (int y = 0; y < inputHybridGrid().height(); ++y) {
            ss << "  [";
            bool firstColumn = true;
            Object& colX     = static_cast<Object&>(shapePixels.method(kb.name("getValue"), { kb.ids.key, toCellNumber(y) }));
            for (int x = 0; x < inputHybridGrid().width(); ++x) {
                if (firstColumn) {
                    firstColumn = false;
                } else {
                    ss << ", ";
                }
                firstColumn       = false;
                CellI& shapePixel = colX.method(kb.name("getValue"), { kb.ids.key, toCellNumber(x) });
                CellI& shape      = shapePixel["shape"];
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
                    scanLineState        = ScanLineState::Middle;
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
        Object& shapePixels      = static_cast<Object&>(frame()["shapePixels"]);

        int x = 0;
        int y = 0;
        for (auto inputRowIt = arcMatrix.begin(); inputRowIt != arcMatrix.end(); ++inputRowIt) {
            for (const int id : *inputRowIt) {
                CellI& currentId  = kb.pools.numbers.get(id);
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
        int shapeCount = static_cast<Number&>(frame()["shapes"]["size"]).value();
        EXPECT_EQ(shapeCount, expectedCount);
    }

    void expectedShapeEdgeCounts(const std::map<int, int>& shapeEdgeCounts)
    {
        for (const auto& edgeCountPair : shapeEdgeCounts) {
            int shapeId     = edgeCountPair.first;
            int edgeCount   = edgeCountPair.second;
            CellI& shape    = static_cast<Object&>(frame()["shapeMap"]).method(kb.name("getValue"), { kb.ids.key, kb.pools.numbers.get(shapeId) });
            EXPECT_EQ(edgeCount, getShapeEdgesSize(shape));
        }
    }

    std::string m_outputSVGFileName;
};

}

TEST_F(EdgeDetectorTest, ShapeWithHoleCompareExactMatch)
{
    const std::string& frame1 = R"([[0,7,7,7],
                                    [7,7,0,7],
                                    [0,7,7,7]])";

    const std::string& frame2 = R"([[0,0,7,7,7],
                                    [0,7,7,0,7],
                                    [0,0,7,7,7]])";
    detect(frame1);
    CellI& shape1       = static_cast<Object&>(frame()["shapeMap"]).method(kb.name("getValue"), { kb.ids.key, _2_ });
    CellI& shape1_edge2 = getEdgeFromShape(shape1, _2_);
    createResult();

    detect(frame2);
    CellI& shape2       = static_cast<Object&>(frame()["shapeMap"]).method(kb.name("getValue"), { kb.ids.key, _2_ });
    CellI& shape2_edge2 = getEdgeFromShape(shape2, _2_);

    infocell::arc::ShapeRelation shapeRelation = infocell::arc::compareShapes(shape1, shape2);
    EXPECT_EQ(shapeRelation.m_edgeRelations.size(), 2);
    EXPECT_EQ(shapeRelation.m_edgeRelations[0].m_rotatedWith, &Degree_0);
    EXPECT_EQ(shapeRelation.m_edgeRelations[1].m_rotatedWith, &Degree_0);
}

TEST_F(EdgeDetectorTest, ShapeWithHoleCompareRotate90)
{
    const std::string& frame1 = R"([[0,7,7,7],
                                    [7,7,0,7],
                                    [0,7,7,7]])";

    const std::string& frame2 = R"([[0,0,0],
                                    [0,7,0],
                                    [7,7,7],
                                    [7,0,7],
                                    [7,7,7]])";
    detect(frame1);
    CellI& shape1       = static_cast<Object&>(frame()["shapeMap"]).method(kb.name("getValue"), { kb.ids.key, _2_ });
    CellI& shape1_edge2 = getEdgeFromShape(shape1, _2_);

    detect(frame2);
    CellI& shape2       = static_cast<Object&>(frame()["shapeMap"]).method(kb.name("getValue"), { kb.ids.key, _2_ });
    CellI& shape2_edge2 = getEdgeFromShape(shape2, _2_);

    infocell::arc::ShapeRelation shapeRelation = infocell::arc::compareShapes(shape1, shape2);
    EXPECT_EQ(shapeRelation.m_edgeRelations.size(), 2);
    EXPECT_EQ(shapeRelation.m_edgeRelations[0].m_rotatedWith, &Degree_90);
    EXPECT_EQ(shapeRelation.m_edgeRelations[1].m_rotatedWith, &Degree_90);
}

TEST_F(EdgeDetectorTest, ShapeWithHoleCompareRotate180)
{
    const std::string& frame1 = R"([[0,7,0],
                                    [7,7,7],
                                    [7,0,7],
                                    [7,7,7]])";

    const std::string& frame2 = R"([[0,0,0],
                                    [7,7,7],
                                    [7,0,7],
                                    [7,7,7],
                                    [0,7,0]])";
    detect(frame1);
    CellI& shape1       = static_cast<Object&>(frame()["shapeMap"]).method(kb.name("getValue"), { kb.ids.key, _2_ });
    CellI& shape1_edge2 = getEdgeFromShape(shape1, _2_);

    detect(frame2);
    CellI& shape2       = static_cast<Object&>(frame()["shapeMap"]).method(kb.name("getValue"), { kb.ids.key, _2_ });
    CellI& shape2_edge2 = getEdgeFromShape(shape2, _2_);

    infocell::arc::ShapeRelation shapeRelation = infocell::arc::compareShapes(shape1, shape2);
    EXPECT_EQ(shapeRelation.m_edgeRelations.size(), 2);
    EXPECT_EQ(shapeRelation.m_edgeRelations[0].m_rotatedWith, &Degree_180);
    EXPECT_EQ(shapeRelation.m_edgeRelations[1].m_rotatedWith, &Degree_180);
}

TEST_F(EdgeDetectorTest, ShapeWithHoleCompareRotate270)
{
    const std::string& frame1 = R"([[0,7,0],
                                    [7,7,7],
                                    [7,0,7],
                                    [7,7,7]])";

    const std::string& frame2 = R"([[0,0,7,7,7],
                                    [0,7,7,0,7],
                                    [0,0,7,7,7]])";
    detect(frame1);
    CellI& shape1       = static_cast<Object&>(frame()["shapeMap"]).method(kb.name("getValue"), { kb.ids.key, _2_ });
    CellI& shape1_edge2 = getEdgeFromShape(shape1, _2_);

    detect(frame2);
    CellI& shape2       = static_cast<Object&>(frame()["shapeMap"]).method(kb.name("getValue"), { kb.ids.key, _2_ });
    CellI& shape2_edge2 = getEdgeFromShape(shape2, _2_);

    infocell::arc::ShapeRelation shapeRelation = infocell::arc::compareShapes(shape1, shape2);
    EXPECT_EQ(shapeRelation.m_edgeRelations.size(), 2);
    EXPECT_EQ(shapeRelation.m_edgeRelations[0].m_rotatedWith, &Degree_270);
    EXPECT_EQ(shapeRelation.m_edgeRelations[1].m_rotatedWith, &Degree_270);
}

TEST_F(EdgeDetectorTest, ShapeWithHoleCompare_Mirror_Horizontal)
{
    const std::string& frame1 = R"([[0,7,7,7,7,7],
                                    [0,0,0,7,0,7],
                                    [7,7,7,7,7,7],
                                    [7,0,7,0,0,0],
                                    [7,7,7,0,0,0]])";

    const std::string& frame2 = R"([[7,7,7,0,0,0],
                                    [7,0,7,0,0,0],
                                    [7,7,7,7,7,7],
                                    [0,0,0,7,0,7],
                                    [0,7,7,7,7,7]])";

    detect(frame1);
    CellI& shape1 = static_cast<Object&>(frame()["shapeMap"]).method(kb.name("getValue"), { kb.ids.key, _2_ });
    CellI& edge1  = getEdgeFromShape(shape1, _1_);

    detect(frame2);
    CellI& shape2 = static_cast<Object&>(frame()["shapeMap"]).method(kb.name("getValue"), { kb.ids.key, _1_ });
    CellI& edge2  = getEdgeFromShape(shape2, _1_);

    infocell::arc::EdgeRelation edgeRelation = infocell::arc::compareEdges(edge1, edge2);
    EXPECT_EQ(edgeRelation.m_rotatedWith, nullptr);
    EXPECT_TRUE(edgeRelation.m_isHorizontallyMirrored);
    EXPECT_FALSE(edgeRelation.m_isVerticallyMirrored);

    infocell::arc::ShapeRelation shapeRelation = infocell::arc::compareShapes(shape1, shape2);
    EXPECT_EQ(shapeRelation.m_edgeRelations.size(), 3);

    EXPECT_EQ(shapeRelation.m_edgeRelations[0].m_rotatedWith, nullptr);
    EXPECT_TRUE(shapeRelation.m_edgeRelations[0].m_isHorizontallyMirrored);
    EXPECT_FALSE(shapeRelation.m_edgeRelations[0].m_isVerticallyMirrored);

    EXPECT_EQ(shapeRelation.m_edgeRelations[1].m_rotatedWith, nullptr);
    EXPECT_TRUE(shapeRelation.m_edgeRelations[1].m_isHorizontallyMirrored);
    EXPECT_FALSE(shapeRelation.m_edgeRelations[1].m_isVerticallyMirrored);

    EXPECT_EQ(shapeRelation.m_edgeRelations[1].m_rotatedWith, nullptr);
    EXPECT_TRUE(shapeRelation.m_edgeRelations[1].m_isHorizontallyMirrored);
    EXPECT_FALSE(shapeRelation.m_edgeRelations[1].m_isVerticallyMirrored);
}

TEST_F(EdgeDetectorTest, ShapeWithHoleCompare_Mirror_Vertical)
{
    const std::string& frame1 = R"([[0,7,7,7,7,7],
                                    [0,0,0,7,0,7],
                                    [7,7,7,7,7,7],
                                    [7,0,7,0,0,0],
                                    [7,7,7,0,0,0]])";

    const std::string& frame2 = R"([[0,7,7,7,7,7,0],
                                    [0,7,0,7,0,0,0],
                                    [0,7,7,7,7,7,7],
                                    [0,0,0,0,7,0,7],
                                    [0,0,0,0,7,7,7]])";

    detect(frame1);
    CellI& shape1 = static_cast<Object&>(frame()["shapeMap"]).method(kb.name("getValue"), { kb.ids.key, _2_ });
    CellI& edge1  = getEdgeFromShape(shape1, _1_);

    detect(frame2);
    CellI& shape2 = static_cast<Object&>(frame()["shapeMap"]).method(kb.name("getValue"), { kb.ids.key, _2_ });
    CellI& edge2  = getEdgeFromShape(shape2, _1_);

    infocell::arc::EdgeRelation edgeRelation = infocell::arc::compareEdges(edge1, edge2);
    EXPECT_EQ(edgeRelation.m_rotatedWith, nullptr);
    EXPECT_FALSE(edgeRelation.m_isHorizontallyMirrored);
    EXPECT_TRUE(edgeRelation.m_isVerticallyMirrored);

    infocell::arc::ShapeRelation shapeRelation = infocell::arc::compareShapes(shape1, shape2);
    EXPECT_EQ(shapeRelation.m_edgeRelations.size(), 3);

    EXPECT_EQ(shapeRelation.m_edgeRelations[0].m_rotatedWith, nullptr);
    EXPECT_FALSE(shapeRelation.m_edgeRelations[0].m_isHorizontallyMirrored);
    EXPECT_TRUE(shapeRelation.m_edgeRelations[0].m_isVerticallyMirrored);

    EXPECT_EQ(shapeRelation.m_edgeRelations[1].m_rotatedWith, nullptr);
    EXPECT_FALSE(shapeRelation.m_edgeRelations[1].m_isHorizontallyMirrored);
    EXPECT_TRUE(shapeRelation.m_edgeRelations[1].m_isVerticallyMirrored);

    EXPECT_EQ(shapeRelation.m_edgeRelations[2].m_rotatedWith, nullptr);
    EXPECT_FALSE(shapeRelation.m_edgeRelations[2].m_isHorizontallyMirrored);
    EXPECT_TRUE(shapeRelation.m_edgeRelations[2].m_isVerticallyMirrored);
}

TEST_F(EdgeDetectorTest, ShapeWithHoleCompare_Mirror_Horizontal_And_Vertical)
{
    const std::string& frame1 = R"([[0,7,7],
                                    [7,7,7],
                                    [7,7,7],
                                    [7,7,0]])";

    const std::string& frame2 = R"([[7,7,0],
                                    [7,7,7],
                                    [7,7,7],
                                    [0,7,7]])";

    detect(frame1);
    CellI& shape1 = static_cast<Object&>(frame()["shapeMap"]).method(kb.name("getValue"), { kb.ids.key, _2_ });
    CellI& edge1  = getEdgeFromShape(shape1, _1_);

    detect(frame2);
    CellI& shape2 = static_cast<Object&>(frame()["shapeMap"]).method(kb.name("getValue"), { kb.ids.key, _1_ });
    CellI& edge2  = getEdgeFromShape(shape2, _1_);

    infocell::arc::EdgeRelation edgeRelation = infocell::arc::compareEdges(edge1, edge2);
    EXPECT_EQ(edgeRelation.m_rotatedWith, nullptr);
    EXPECT_TRUE(edgeRelation.m_isHorizontallyMirrored);
    EXPECT_TRUE(edgeRelation.m_isVerticallyMirrored);

    infocell::arc::ShapeRelation shapeRelation = infocell::arc::compareShapes(shape1, shape2);
    EXPECT_EQ(shapeRelation.m_edgeRelations.size(), 1);
    EXPECT_EQ(shapeRelation.m_edgeRelations[0].m_rotatedWith, nullptr);
    EXPECT_TRUE(shapeRelation.m_edgeRelations[0].m_isHorizontallyMirrored);
    EXPECT_TRUE(shapeRelation.m_edgeRelations[0].m_isVerticallyMirrored);
}

TEST_F(EdgeDetectorTest, ShapeCompareExactMatch)
{
    const std::string& frame1 = R"([[0,7,7,7],
                                    [7,7,7,7],
                                    [0,7,7,7]])";

    const std::string& frame2 = R"([[0,0,7,7,7],
                                    [0,7,7,7,7],
                                    [0,0,7,7,7]])";
    detect(frame1);
    CellI& shape1FirstPixel = static_cast<Object&>(frame()["shapeMap"]).method(kb.name("getValue"), { kb.ids.key, _1_ });
    CellI& edge1FirstPixel  = getEdgeFromShape(shape1FirstPixel, _1_);

    CellI& shape1 = static_cast<Object&>(frame()["shapeMap"]).method(kb.name("getValue"), { kb.ids.key, _2_ });
    CellI& edge1  = getEdgeFromShape(shape1, _1_);

    detect(frame2);
    CellI& shape2 = static_cast<Object&>(frame()["shapeMap"]).method(kb.name("getValue"), { kb.ids.key, _2_ });
    CellI& edge2  = getEdgeFromShape(shape2, _1_);

    EXPECT_TRUE(edge1["rotationCorners"].has("upLeftNode"));
    EXPECT_TRUE(edge2["rotationCorners"].has("upLeftNode"));

    EXPECT_NE(infocell::arc::compareEdges(edge1FirstPixel, edge2).m_rotatedWith, &Degree_0);
    EXPECT_EQ(infocell::arc::compareEdges(edge1, edge2).m_rotatedWith, &Degree_0);

    infocell::arc::ShapeRelation shapeRelation = infocell::arc::compareShapes(shape1, shape2);
    EXPECT_EQ(shapeRelation.m_edgeRelations.size(), 1);
    EXPECT_EQ(shapeRelation.m_edgeRelations[0].m_rotatedWith, &Degree_0);
}

TEST_F(EdgeDetectorTest, ShapeCompareRotate90)
{
    const std::string& frame1 = R"([[0,7,0],
                                    [7,7,7],
                                    [7,7,7],
                                    [7,7,7]])";

    const std::string& frame2 = R"([[0,7,7,7,0],
                                    [0,7,7,7,7],
                                    [0,7,7,7,0]])";

    detect(frame1);
    CellI& shape1 = static_cast<Object&>(frame()["shapeMap"]).method(kb.name("getValue"), { kb.ids.key, kb.pools.numbers.get(2) });
    CellI& edge1  = getEdgeFromShape(shape1, _1_);

    detect(frame2);
    CellI& shape2 = static_cast<Object&>(frame()["shapeMap"]).method(kb.name("getValue"), { kb.ids.key, kb.pools.numbers.get(2) });
    CellI& edge2  = getEdgeFromShape(shape2, _1_);

    infocell::arc::EdgeRelation edgeRelation = infocell::arc::compareEdges(edge1, edge2);
    EXPECT_EQ(edgeRelation.m_rotatedWith, &Degree_90);

    infocell::arc::ShapeRelation shapeRelation = infocell::arc::compareShapes(shape1, shape2);
    EXPECT_EQ(shapeRelation.m_edgeRelations.size(), 1);
    EXPECT_EQ(shapeRelation.m_edgeRelations[0].m_rotatedWith, &Degree_90);
}

TEST_F(EdgeDetectorTest, ShapeCompareRotate180)
{
    const std::string& frame1 = R"([[0,7,7],
                                    [7,7,7],
                                    [7,7,7],
                                    [7,7,7]])";

    const std::string& frame2 = R"([[7,7,7],
                                    [7,7,7],
                                    [7,7,7],
                                    [7,7,0]])";

    detect(frame1);
    CellI& shape1 = static_cast<Object&>(frame()["shapeMap"]).method(kb.name("getValue"), { kb.ids.key, kb.pools.numbers.get(2) });
    CellI& edge1  = getEdgeFromShape(shape1, _1_);

    detect(frame2);
    CellI& shape2 = static_cast<Object&>(frame()["shapeMap"]).method(kb.name("getValue"), { kb.ids.key, kb.pools.numbers.get(1) });
    CellI& edge2  = getEdgeFromShape(shape2, _1_);

    infocell::arc::EdgeRelation edgeRelation = infocell::arc::compareEdges(edge1, edge2);
    EXPECT_EQ(edgeRelation.m_rotatedWith, &Degree_180);

    infocell::arc::ShapeRelation shapeRelation = infocell::arc::compareShapes(shape1, shape2);
    EXPECT_EQ(shapeRelation.m_edgeRelations.size(), 1);
    EXPECT_EQ(shapeRelation.m_edgeRelations[0].m_rotatedWith, &Degree_180);
}

TEST_F(EdgeDetectorTest, ShapeCompareRotate270)
{
    const std::string& frame1 = R"([[0,7,0],
                                    [7,7,7],
                                    [7,7,7],
                                    [7,7,7]])";

    const std::string& frame2 = R"([[0,0,7,7,7],
                                    [0,7,7,7,7],
                                    [0,0,7,7,7]])";

    detect(frame1);
    CellI& shape1 = static_cast<Object&>(frame()["shapeMap"]).method(kb.name("getValue"), { kb.ids.key, kb.pools.numbers.get(2) });
    CellI& edge1  = getEdgeFromShape(shape1, _1_);

    detect(frame2);
    CellI& shape2 = static_cast<Object&>(frame()["shapeMap"]).method(kb.name("getValue"), { kb.ids.key, kb.pools.numbers.get(2) });
    CellI& edge2  = getEdgeFromShape(shape2, _1_);

    infocell::arc::EdgeRelation edgeRelation = infocell::arc::compareEdges(edge1, edge2);
    EXPECT_EQ(edgeRelation.m_rotatedWith, &Degree_270);

    infocell::arc::ShapeRelation shapeRelation = infocell::arc::compareShapes(shape1, shape2);
    EXPECT_EQ(shapeRelation.m_edgeRelations.size(), 1);
    EXPECT_EQ(shapeRelation.m_edgeRelations[0].m_rotatedWith, &Degree_270);
}

TEST_F(EdgeDetectorTest, ShapeCompare_Mirror_Horizontal)
{
    const std::string& frame1 = R"([[0,7,7],
                                    [7,7,7],
                                    [7,0,0],
                                    [7,0,0]])";

    const std::string& frame2 = R"([[7,0,0],
                                    [7,0,0],
                                    [7,7,7],
                                    [0,7,7]])";

    detect(frame1);
    CellI& shape1 = static_cast<Object&>(frame()["shapeMap"]).method(kb.name("getValue"), { kb.ids.key, kb.pools.numbers.get(2) });
    CellI& edge1  = getEdgeFromShape(shape1, _1_);

    detect(frame2);
    CellI& shape2 = static_cast<Object&>(frame()["shapeMap"]).method(kb.name("getValue"), { kb.ids.key, kb.pools.numbers.get(1) });
    CellI& edge2  = getEdgeFromShape(shape2, _1_);

    infocell::arc::EdgeRelation edgeRelation = infocell::arc::compareEdges(edge1, edge2);
    EXPECT_EQ(edgeRelation.m_rotatedWith, nullptr);
    EXPECT_TRUE(edgeRelation.m_isHorizontallyMirrored);
    EXPECT_FALSE(edgeRelation.m_isVerticallyMirrored);

    infocell::arc::ShapeRelation shapeRelation = infocell::arc::compareShapes(shape1, shape2);
    EXPECT_EQ(shapeRelation.m_edgeRelations.size(), 1);
    EXPECT_EQ(shapeRelation.m_edgeRelations[0].m_rotatedWith, nullptr);
    EXPECT_TRUE(shapeRelation.m_edgeRelations[0].m_isHorizontallyMirrored);
    EXPECT_FALSE(shapeRelation.m_edgeRelations[0].m_isVerticallyMirrored);
}

TEST_F(EdgeDetectorTest, ShapeCompare_Mirror_Vertical)
{
    const std::string& frame1 = R"([[7,0,7],
                                    [7,0,7],
                                    [7,7,7],
                                    [0,7,7]])";

    const std::string& frame2 = R"([[7,0,7],
                                    [7,0,7],
                                    [7,7,7],
                                    [7,7,0]])";

    detect(frame1);
    CellI& shape1 = static_cast<Object&>(frame()["shapeMap"]).method(kb.name("getValue"), { kb.ids.key, kb.pools.numbers.get(1) });
    CellI& edge1  = getEdgeFromShape(shape1, _1_);

    detect(frame2);
    CellI& shape2 = static_cast<Object&>(frame()["shapeMap"]).method(kb.name("getValue"), { kb.ids.key, kb.pools.numbers.get(1) });
    CellI& edge2  = getEdgeFromShape(shape2, _1_);

    infocell::arc::EdgeRelation edgeRelation = infocell::arc::compareEdges(edge1, edge2);
    EXPECT_EQ(edgeRelation.m_rotatedWith, nullptr);
    EXPECT_FALSE(edgeRelation.m_isHorizontallyMirrored);
    EXPECT_TRUE(edgeRelation.m_isVerticallyMirrored);

    infocell::arc::ShapeRelation shapeRelation = infocell::arc::compareShapes(shape1, shape2);
    EXPECT_EQ(shapeRelation.m_edgeRelations.size(), 1);
    EXPECT_EQ(shapeRelation.m_edgeRelations[0].m_rotatedWith, nullptr);
    EXPECT_FALSE(shapeRelation.m_edgeRelations[0].m_isHorizontallyMirrored);
    EXPECT_TRUE(shapeRelation.m_edgeRelations[0].m_isVerticallyMirrored);
}

TEST_F(EdgeDetectorTest, ShapeCompare_Mirror_Horizontal_And_Vertical)
{
    const std::string& frame1 = R"([[0,7,7],
                                    [7,7,7],
                                    [7,7,7],
                                    [7,7,0]])";

    const std::string& frame2 = R"([[7,7,0],
                                    [7,7,7],
                                    [7,7,7],
                                    [0,7,7]])";

    detect(frame1);
    CellI& shape1 = static_cast<Object&>(frame()["shapeMap"]).method(kb.name("getValue"), { kb.ids.key, kb.pools.numbers.get(2) });
    CellI& edge1  = getEdgeFromShape(shape1, _1_);

    detect(frame2);
    CellI& shape2 = static_cast<Object&>(frame()["shapeMap"]).method(kb.name("getValue"), { kb.ids.key, kb.pools.numbers.get(1) });
    CellI& edge2  = getEdgeFromShape(shape2, _1_);

    infocell::arc::EdgeRelation edgeRelation = infocell::arc::compareEdges(edge1, edge2);
    EXPECT_EQ(edgeRelation.m_rotatedWith, nullptr);
    EXPECT_TRUE(edgeRelation.m_isHorizontallyMirrored);
    EXPECT_TRUE(edgeRelation.m_isVerticallyMirrored);

    infocell::arc::ShapeRelation shapeRelation = infocell::arc::compareShapes(shape1, shape2);
    EXPECT_EQ(shapeRelation.m_edgeRelations.size(), 1);
    EXPECT_EQ(shapeRelation.m_edgeRelations[0].m_rotatedWith, nullptr);
    EXPECT_TRUE(shapeRelation.m_edgeRelations[0].m_isHorizontallyMirrored);
    EXPECT_TRUE(shapeRelation.m_edgeRelations[0].m_isVerticallyMirrored);
}

TEST_F(EdgeDetectorTest, EdgeTestWithArc_0ca9ddb6_Train1Input)
{
    detect(R"([[0,0,0,0,0,0,0,0,0],
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

TEST_F(EdgeDetectorTest, EdgeTestWithArc_0ca9ddb6_Train1Output)
{
    detect(R"([[0,0,0,0,0,0,0,0,0],
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

TEST_F(EdgeDetectorTest, EdgeTestWithArc_0ca9ddb6_Train2Input)
{
    detect(R"([[0,0,0,8,0,0,0,0,0],
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

TEST_F(EdgeDetectorTest, EdgeTestWithArc_0ca9ddb6_Train2Output)
{
    detect(R"([[0,0,0,8,0,0,0,0,0],
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

TEST_F(EdgeDetectorTest, EdgeTestWithArc_0ca9ddb6_Train3Input)
{
    detect(R"([[0,0,0,0,0,0,0,0,0],
               [0,0,0,0,0,0,0,0,0],
               [0,0,2,0,0,0,0,0,0],
               [0,0,0,0,0,0,0,0,0],
               [0,0,0,0,0,0,0,0,0],
               [0,0,0,0,0,0,6,0,0],
               [0,0,0,0,0,0,0,0,0],
               [0,0,0,1,0,0,0,0,0],
               [0,0,0,0,0,0,0,0,0]])");
}

TEST_F(EdgeDetectorTest, EdgeTestWithArc_0ca9ddb6_Train3Output)
{
    detect(R"([[0,0,0,0,0,0,0,0,0],
               [0,4,0,4,0,0,0,0,0],
               [0,0,2,0,0,0,0,0,0],
               [0,4,0,4,0,0,0,0,0],
               [0,0,0,0,0,0,0,0,0],
               [0,0,0,0,0,0,6,0,0],
               [0,0,0,7,0,0,0,0,0],
               [0,0,7,1,7,0,0,0,0],
               [0,0,0,7,0,0,0,0,0]])");
}

TEST_F(EdgeDetectorTest, EdgeTestWithArc_0ca9ddb6_Test1Input)
{
    detect(R"([[0,0,0,0,0,0,0,0,0],
               [0,0,0,0,0,0,0,0,0],
               [0,0,0,0,0,0,1,0,0],
               [0,0,2,0,0,0,0,0,0],
               [0,0,0,0,0,0,0,0,0],
               [0,0,0,0,0,8,0,0,0],
               [0,0,0,0,0,0,0,0,0],
               [0,6,0,0,0,0,0,2,0],
               [0,0,0,0,0,0,0,0,0]])");
}

TEST_F(EdgeDetectorTest, EdgeTestWithArc_2dd70a9a_Test1Input)
{
    detect(R"([[8,8,8,8,0,0,0,0,0,8,8,0,0],
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

TEST_F(EdgeDetectorTest, EdgeTestWithArc_00d62c1b_Train5Input)
{
    detect(R"([[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],
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

TEST_F(EdgeDetectorTest, EdgeTestWithArc_00d62c1b_Train5Output)
{
    detect(R"([[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],
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

TEST_F(EdgeDetectorTest, EdgeTestWithArc_0b148d64_minified_Train1Input)
{
    detect(R"([[8,8,8,8,8,0,8,8,8,8,0],
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

TEST_F(EdgeDetectorTest, EdgeTestCase7Horizontal)
{
    detect(R"([[0,0,0,0,0],
               [0,0,1,0,0],
               [0,1,8,1,0],
               [0,0,1,0,0],
               [0,0,0,0,0]])");
}

TEST_F(EdgeDetectorTest, EdgeTestInternalEdges)
{
    detect(R"([[0,0,0,0,0,0,0],
               [0,1,1,1,1,1,0],
               [0,1,8,8,8,1,0],
               [0,1,8,2,8,1,0],
               [0,1,8,8,8,1,0],
               [0,1,1,1,1,1,0],
               [0,0,0,0,0,0,0]])");
}

TEST_F(EdgeDetectorTest, EdgeTestMinimal)
{
    detect(R"([[0, 7, 7],
               [7, 7, 7],
               [7, 7, 7]])");
    CellI& shape        = static_cast<Object&>(frame()["shapeMap"]).method(kb.name("getValue"), { kb.ids.key, kb.pools.numbers.get(2) });
    CellI& externalEdge = getEdgeFromShape(shape, _1_);

    CellI* firstColumnPointPtr   = &(*firstShapePixelPtr())["upLeftPoint"];
    CellI* currentShapePointPtr  = firstColumnPointPtr;
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
    currentShapePointPtr  = &(*currentShapePointPtr)["right"];
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

TEST_F(EdgeDetectorTest, EdgeTest)
{
    detect(R"([[0, 0, 0, 0, 0],
               [0, 4, 0, 4, 0],
               [0, 0, 2, 0, 0],
               [0, 4, 0, 4, 0],
               [0, 0, 0, 0, 0]])");
    CellI& shape        = static_cast<Object&>(frame()["shapeMap"]).method(kb.name("getValue"), { kb.ids.key, kb.pools.numbers.get(1) });
    CellI& internalEdge = getEdgeFromShape(shape, _2_);
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

TEST_F(EdgeDetectorTest, EdgeTestTwoLeftCorners)
{
    detect(R"([[0, 7, 7, 7],
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

TEST_F(EdgeDetectorTest, EdgeTestWithFourCorners)
{
    detect(R"([[0, 7, 7, 0],
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

TEST_F(EdgeDetectorTest, EdgeTestWithLineDiagonalFromUpLeft)
{
    detect(R"([[7, 0, 0, 0, 0],
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

TEST_F(EdgeDetectorTest, EdgeTestWithLineDiagonalFromUpRight)
{
    detect(R"([[0, 0, 0, 0, 7],
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

TEST_F(EdgeDetectorTest, EdgeTestWithArc_4be741c5_Train3Input)
{
    detect(R"([[6, 6, 6, 6, 6, 6, 6, 6, 6],
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

TEST_F(EdgeDetectorTest, EdgeTestWithArc_4be741c5_Train3Output)
{
    detect(R"([[6],
               [4],
               [2],
               [3]])");
}

TEST_F(EdgeDetectorTest, EdgeTestWithAllArcTask)
{
    infocell::arc::TaskSet taskSet(kb, INFOCELL_ARCPRIZE_PATH INFOCELL_ARC_PRIZE_TRAINING_CHALLENGES_FILENAME);
    // TaskSet taskSet(kb, INFOCELL_ARCPRIZE_PATH INFOCELL_ARC_PRIZE_EVALUATION_CHALLENGES_FILENAME);
    for (auto& task : taskSet.m_tasks) {
        INFO(grid, fmt::format("id: {}, examples num: {}, tests num: {}", task.first, static_cast<List&>(task.second.m_cellExamplesList).size(), static_cast<List&>(task.second.m_cellTestsList).size()));
        TRACE(grid, "   examples:");
        Visitor::visitList(task.second.m_cellExamplesList, [this, &task](CellI& example, int i, bool&) {
            const int humanIndex = i + 1;
            INFO(grid, fmt::format("id: {}, example input: {}", task.first, humanIndex));
            TRACE(grid, fmt::format("id: {}, example input: {}", task.first, humanIndex));
            TRACE(grid, fmt::format("id: {}, example input: {}", task.first, humanIndex));
            setOutputSVGName(fmt::format("EdgeTestWithArc_{}_{}{}{}", task.first, "Train", humanIndex, "Input"));
            detect(static_cast<hybridarc::Grid&>(example["input"]));
            INFO(grid, fmt::format("id: {}, example output: {}", task.first, humanIndex));
            TRACE(grid, fmt::format("id: {}, example output: {}", task.first, humanIndex));
            TRACE(grid, fmt::format("id: {}, example output: {}", task.first, humanIndex));
            setOutputSVGName(fmt::format("EdgeTestWithArc_{}_{}{}{}", task.first, "Train", humanIndex, "Output"));
            detect(static_cast<hybridarc::Grid&>(example["output"]));
        });
        TRACE(grid, "   tests:");
        Visitor::visitList(task.second.m_cellTestsList, [this, &task](CellI& example, int i, bool&) {
            const int humanIndex = i + 1;
            INFO(grid, fmt::format("id: {}, test input: {}", task.first, humanIndex));
            setOutputSVGName(fmt::format("EdgeTestWithArc_{}_{}{}{}", task.first, "Test", humanIndex, "Input"));
            detect(static_cast<hybridarc::Grid&>(example["input"]));
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
    TestBase::createKb([]() {
        brain::Brain::Logger::createLogger("edge");
        brain::Brain::Logger::createLogger("shapeCorners");
        brain::Brain::Logger::createLogger("shapeRelations");
        brain::Brain::Logger::createLogger("shapeIdGrid");
        brain::Brain::Logger::createLogger("grid");

        spdlog::get("cells")->set_level(spdlog::level::trace);
        spdlog::get("compileStruct")->set_level(spdlog::level::off);
        spdlog::get("compiledSymbols")->set_level(spdlog::level::off);
        spdlog::get("edge")->set_level(spdlog::level::off);
        spdlog::get("shapeCorners")->set_level(spdlog::level::off);
        spdlog::get("shapeIdGrid")->set_level(spdlog::level::off);
        spdlog::get("grid")->set_level(spdlog::level::trace);
        spdlog::get("shapeRelations")->set_level(spdlog::level::off);
    });
    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    DEBUG(cells, "Constructed: {}, destructed: {}, live: {}", CellI::s_constructed, CellI::s_destructed, CellI::s_constructed - CellI::s_destructed);
    TestBase::freeKb();

    return ret;
}
