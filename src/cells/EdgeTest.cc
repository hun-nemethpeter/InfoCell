#include "CellTestBase.h"
#include "Arc.h"

#include "Config.h"
#include "app/App.h"
#include "util/ArcTask.h"

#include <fstream>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <nlohmann/json.hpp>

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#define TRACE(msg, ...) SPDLOG_LOGGER_TRACE(s_logger, msg, __VA_ARGS__)
#define DEBUG(msg, ...) SPDLOG_LOGGER_DEBUG(s_logger, msg, __VA_ARGS__)
#define INFO(msg, ...) SPDLOG_LOGGER_INFO(s_logger, msg, __VA_ARGS__)
#define WARN(msg, ...) SPDLOG_LOGGER_WARN(s_logger, msg, __VA_ARGS__)
#define ERROR(msg, ...) SPDLOG_LOGGER_ERROR(s_logger, msg, __VA_ARGS__)

using namespace synth;
using namespace synth::cells;
using nlohmann::json;
using synth::cells::test::CellTest;

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
//        debugShapePointEdgeJoints();
        sortEdges();
//        printShapeIdGrid();
//        printEdges();
        printShapeRelations();
    }

    void setInputGrid(const std::string& jsonStr)
    {
        m_inputGrid       = std::make_unique<input::Grid>("inputGrid", jsonStr);
        m_inputHybridGrid = std::make_unique<cells::hybrid::arc::Grid>(kb, *m_inputGrid);
    }

    static ftxui::Element colorTile(const ftxui::Color& p_color)
    {
        static ftxui::Color ftxAlphaColor(255, 255, 255);
        if (&p_color == &ftxAlphaColor)
            return ftxui::text(L"╳╳") | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 2) | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 1) | ftxui::color(ftxui::Color::GrayDark) | ftxui::bgcolor(ftxui::Color::GrayLight);
        return ftxui::text("") | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 2) | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 1) | bgcolor(p_color);
    }

    void printInputHybridGrid()
    {
        cells::hybrid::arc::Grid& grid = inputHybridGrid();
        CellI& pixelList               = grid[kb.ids.pixels];
        ftxui::Elements boardLines;
        for (int y = 0; y < grid.height(); ++y) {
            ftxui::Elements arcSetInputLine;
            for (int x = 0; x < grid.width(); ++x) {
                hybrid::arc::Pixel& pixel = grid.getPixel(x, y);
                arcSetInputLine.push_back(colorTile(synth::App::arcColors[pixel.color()]));
            }
            boardLines.push_back(hbox(arcSetInputLine));
        }

        ftxui::Element board = vbox(boardLines);

        auto document = ftxui::hbox(board) | ftxui::border;
        auto screen          = ftxui::Screen::Create(
            ftxui::Dimension::Fit(document), // Width
            ftxui::Dimension::Fit(document)  // Height
        );
        ftxui::Render(screen, document);
        screen.Print();
        std::cout << "\n";
    }

    void printShapeRelations()
    {
        DEBUG("printShapeRelations");
        Visitor::visitList(shaper()["shapes"], [this](CellI& shape, int, bool&) {
            int edgesCount = static_cast<Map&>(shape["edges"]).size();
            if (edgesCount == 1) {
                TRACE("  shape id {} has only external edge", shape["id"].label());
            } else {
                TRACE("  shape id {} has internal edge(s)", shape["id"].label());
                Visitor::visitList(shape["edges"]["list"], [this](CellI& edge, int, bool&) {
                    if (&edge["kind"] == &InternalEdgeEV) {
                        if (edge.has("shapes")) {
                            std::stringstream ss;
                            Visitor::visitList(edge["shapes"]["index"]["struct"]["slots"]["list"], [this, &ss](CellI& slot, int, bool&) {
                                CellI& shape = slot["slotRole"];
                                ss << fmt::format("shape({}) ", shape["id"].label());
                            });
                            TRACE("    edge id {} internal and contains: {}", edge["id"].label(), ss.str());
                        } else {
                            TRACE("    edge id {} internal", edge["id"].label());
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
        TRACE("printEveryShapePixels");
        Visitor::visitList(shaper()["shapes"], [this](CellI& currentShape, int, bool&) {
            TRACE("Shape id: {}, pixels: ", currentShape["id"].label());
            Visitor::visitList(currentShape["shapePixels"], [this](CellI& shapePixel, int, bool&) {
                TRACE("[{}, {}]", shapePixel["pixel"]["x"].label(), shapePixel["pixel"]["y"].label());
            });
        });
    }

    void printAndTestShapePixels()
    {
        CellI* firstColumnPixelPtr  = firstShapePixelPtr();
        CellI* currentShapePixelPtr = firstShapePixelPtr();

        DEBUG("printAndTestShapePixels");
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
            ss << "[" << x << ", " << y << "]";
            if (x == 0) {
                EXPECT_FALSE(currentShapePixelPtr->has("left"));
            } else {
                hybrid::arc::Pixel& otherArcPixel = static_cast<hybrid::arc::Pixel&>((*currentShapePixelPtr)["left"]["pixel"]);
                EXPECT_EQ(x - 1, otherArcPixel.m_x.value());
            }
            if (x == inputHybridGrid().width() - 1) {
                EXPECT_FALSE(currentShapePixelPtr->has("right"));
                TRACE(ss.str());
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

    void debugShapePointEdgeJoints()
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

                if (edgeJoint.has("right")) {
                    svgFile << fmt::format("      <!-- edgeJoint right -->\n");
                    CellI& rightEdgeNode = edgeJoint["right"];
                    if (rightEdgeNode.has("rightSide")) {
                        int color = static_cast<Number&>(rightEdgeNode["rightSide"]["shape"]["color"]).value();
                        svgFile << fmt::format("      <!-- rightSide -->\n");
                        svgFile << fmt::format("      <text x=\"{}\" y=\"{}\" fill=\"{}\" class=\"arrowText\">{}</text>\n", 30 + pointX * 120, 18 + pointY * 120, arcColors[color], rightEdgeNode["rightSide"]["id"].label());
                        svgFile << fmt::format("      <line x1=\"{}\" y1=\"{}\" x2=\"{}\" y2=\"{}\" stroke=\"{}\" marker-end=\"url(#arrow)\"/>\n", 20 + pointX * 120, 20 + pointY * 120, 50 + pointX * 120, 20 + pointY * 120, arcColors[color]);
                    }
                    if (rightEdgeNode.has("leftSide")) {
                        int color = static_cast<Number&>(rightEdgeNode["leftSide"]["shape"]["color"]).value();
                        svgFile << fmt::format("      <!-- leftSide -->\n");
                        svgFile << fmt::format("      <text x=\"{}\" y=\"{}\" fill=\"{}\" class=\"arrowText\">{}</text>\n", 30 + pointX * 120, -2 + pointY * 120, arcColors[color], rightEdgeNode["leftSide"]["id"].label());
                        svgFile << fmt::format("      <line x1=\"{}\" y1=\"{}\" x2=\"{}\" y2=\"{}\" stroke=\"{}\" marker-end=\"url(#arrow)\"/>\n", 20 + pointX * 120, pointY * 120, 50 + pointX * 120, pointY * 120, arcColors[color]);
                    }
                }
                if (edgeJoint.has("down")) {
                    svgFile << fmt::format("      <!-- edgeJoint down -->\n");
                    CellI& downEdgeNode = edgeJoint["down"];
                    if (downEdgeNode.has("rightSide")) {
                        int color = static_cast<Number&>(downEdgeNode["rightSide"]["shape"]["color"]).value();
                        svgFile << fmt::format("      <!-- rightSide -->\n");
                        svgFile << fmt::format("      <g transform=\"translate({} {}) rotate(90)\">\n", pointX * 120, 20 + pointY * 120, 20 + pointX * 120, 50 + pointY * 120, arcColors[color]);
                        svgFile << fmt::format("        <text x=\"15\" y=\"-2\" fill=\"{}\" class=\"arrowText\">{}</text>\n", arcColors[color], downEdgeNode["rightSide"]["id"].label());
                        svgFile << fmt::format("        <line x1=\"0\" y1=\"0\" x2=\"30\" y2=\"0\" stroke=\"{}\" marker-end=\"url(#arrow)\"/>\n", arcColors[color]);
                        svgFile << fmt::format("      </g>\n");
                    }
                    if (downEdgeNode.has("leftSide")) {
                        int color = static_cast<Number&>(downEdgeNode["leftSide"]["shape"]["color"]).value();
                        svgFile << fmt::format("      <!-- leftSide -->\n");
                        svgFile << fmt::format("      <g transform=\"translate({} {}) rotate(90)\">\n", 20 + pointX * 120, 20 + pointY * 120, 20 + pointX * 120, 50 + pointY * 120, arcColors[color]);
                        svgFile << fmt::format("        <text x=\"15\" y=\"-2\" fill=\"{}\" class=\"arrowText\">{}</text>\n", arcColors[color], downEdgeNode["leftSide"]["id"].label());
                        svgFile << fmt::format("        <line x1=\"0\" y1=\"0\" x2=\"30\" y2=\"0\" stroke=\"{}\" marker-end=\"url(#arrow)\"/>\n", arcColors[color]);
                        svgFile << fmt::format("      </g>\n");
                    }
                }
                if (edgeJoint.has("left")) {
                    CellI& leftEdgeNode = edgeJoint["left"];
                    svgFile << fmt::format("      <!-- edgeJoint left -->\n");
                    if (leftEdgeNode.has("rightSide")) {
                        int color = static_cast<Number&>(leftEdgeNode["rightSide"]["shape"]["color"]).value();
                        svgFile << fmt::format("      <!-- rightSide -->\n");
                        svgFile << fmt::format("      <text x=\"{}\" y=\"{}\" fill=\"{}\" class=\"arrowText\">{}</text>\n", -20 + pointX * 120, 18 + pointY * 120, arcColors[color], leftEdgeNode["rightSide"]["id"].label());
                        svgFile << fmt::format("      <line x1=\"{}\" y1=\"{}\" x2=\"{}\" y2=\"{}\" stroke=\"{}\" marker-end=\"url(#arrow)\"/>\n", -30 + pointX * 120, 20 + pointY * 120, pointX * 120, 20 + pointY * 120, arcColors[color]);
                    }
                    if (leftEdgeNode.has("leftSide")) {
                        int color = static_cast<Number&>(leftEdgeNode["leftSide"]["shape"]["color"]).value();
                        svgFile << fmt::format("      <!-- leftSide -->\n");
                        svgFile << fmt::format("      <text x=\"{}\" y=\"{}\" fill=\"{}\" class=\"arrowText\">{}</text>\n", -20 + pointX * 120, -2 + pointY * 120, arcColors[color], leftEdgeNode["leftSide"]["id"].label());
                        svgFile << fmt::format("      <line x1=\"{}\" y1=\"{}\" x2=\"{}\" y2=\"{}\" stroke=\"{}\" marker-end=\"url(#arrow)\"/>\n", -30 + pointX * 120, pointY * 120, pointX * 120, pointY * 120, arcColors[color]);
                    }
                }
                if (edgeJoint.has("up")) {
                    svgFile << fmt::format("      <!-- edgeJoint up -->\n");
                    CellI& upEdgeNode = edgeJoint["up"];
                    if (upEdgeNode.has("rightSide")) {
                        int color = static_cast<Number&>(upEdgeNode["rightSide"]["shape"]["color"]).value();
                        svgFile << fmt::format("      <!-- rightSide -->\n");
                        svgFile << fmt::format("      <g transform=\"translate({} {}) rotate(90)\">\n", pointX * 120, -30 + pointY * 120, arcColors[color]);
                        svgFile << fmt::format("        <text x=\"10\" y=\"-2\" fill=\"{}\" class=\"arrowText\">{}</text>\n", arcColors[color], upEdgeNode["rightSide"]["id"].label());
                        svgFile << fmt::format("        <line x1=\"0\" y1=\"0\" x2=\"30\" y2=\"0\" stroke=\"{}\" marker-end=\"url(#arrow)\"/>\n", arcColors[color]);
                        svgFile << fmt::format("      </g>\n");
                    }
                    if (upEdgeNode.has("leftSide")) {
                        int color = static_cast<Number&>(upEdgeNode["leftSide"]["shape"]["color"]).value();
                        svgFile << fmt::format("      <!-- leftSide -->\n");
                        svgFile << fmt::format("      <g transform=\"translate({} {}) rotate(90)\">\n", 20 + pointX * 120, -30 + pointY * 120, arcColors[color]);
                        svgFile << fmt::format("        <text x=\"10\" y=\"-2\" fill=\"{}\" class=\"arrowText\">{}</text>\n", arcColors[color], upEdgeNode["leftSide"]["id"].label());
                        svgFile << fmt::format("        <line x1=\"0\" y1=\"0\" x2=\"30\" y2=\"0\" stroke=\"{}\" marker-end=\"url(#arrow)\"/>\n", arcColors[color]);
                        svgFile << fmt::format("      </g>\n");
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
            TRACE("Shape id: {}, points:", currentShape["id"].label());

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
                            TRACE("{} Up -> Middle", ss.str());
                            ss.str("");
                            ss << "  ";
                        }
                    } else {
                        currentListItemPtr = firstColumnPixelItem;
                        pointX             = -1;
                        ++pointY;
                        scanLineState = ScanLineState::Down;
                        TRACE("{} Up -> Down", ss.str());
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
                        TRACE("{} Middle -> Down", ss.str());
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
                        TRACE("{} Middle -> Middle", ss.str());
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
            TRACE(ss.str());
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
        DEBUG("calculateEdgesForShapes");
        Visitor::visitList(shaper()["shapes"], [this](CellI& currentShape, int, bool&) {
            TRACE("  Shape id: {}", currentShape["id"].label());

            enum class ProcessingMode
            {
                ExternalEdge,
                InternalEdge,
                Searching
            };

            ProcessingMode processingMode = ProcessingMode::ExternalEdge;
            CellI* processingDirectionPtr = &DirectionRightEV;
            CellI* currentListItemPtr     = &currentShape["shapePoints"][kb.ids.first];
            CellI* firstColumnPointItem   = currentListItemPtr;
            while (currentListItemPtr) {
                CellI& currentListItem = *currentListItemPtr;
                CellI& shapePoint      = currentListItem[kb.ids.value];
                int pointX             = static_cast<Number&>((*currentListItemPtr)["value"]["x"]).value();
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
 3 0100 Skip
 4 1100 Skip
 5 0010 Skip
 6 1010 Skip
 7 0110 Skip
 8 1110 Start internal edge
 9 0001 Start external edge
10 1001 Skip
11 0101 Skip
12 1101 Skip
13 0011 Skip
14 1011 Skip
15 0111 Skip
16 1111 Skip

Invalid   Skip     Continue  Continue Skip     Skip      Special  New edge New edge Special   Skip     Skip     Continue Continue  Skip     Skip
 1        2         3        4        5        6         7        8        9        10        11       12       13        14       15       16
 0🡬 0🡭   1🡬 0🡭   0🡬 1🡭   1🡬 1🡭   0🡬 0🡭   1🡬 0🡭   0🡬 1🡭   1🡬 1🡭   🡬 0🡭    1🡬 0🡭   0🡬 1🡭   1🡬 1🡭   0🡬 0🡭   1🡬 0🡭   0🡬 1🡭   1🡬 1🡭
 0🡯 0🡮   0🡯 0🡮   0🡯 0🡮   0🡯 0🡮   1🡯 0🡮   1🡯 0🡮   1🡯 0🡮   1🡯 0🡮   🡯 1🡮    0🡯 1🡮   0🡯 1🡮   0🡯 1🡮   1🡯 1🡮   1🡯 1🡮   1🡯 1🡮   1🡯 1🡮
 .--.--.  .--.--.  .--.--.   .--.--.  .--.--.  .--.--.   .--.--.  .--.--.  .--.--.  .--.--.   .--.--.  .--.--.  .--.--.  .--.--.   .--.--.  .--.--.
 |  |  |  |XX|  |  |  |XX|   |XX|XX|  |  |  |  |XX|  |   |  |XX|  |XX|XX|  |  |  |  |XX|  |   |  |XX|  |XX|XX|  |  |  |  |XX|  |   |  |XX|  |XX|XX|
 .--o->.  .--o->.  .--o->.   .--o->.  .--o->.  .--o->.   .--o->.  .--o->.  .--o->.  .--o->.   .--o->.  .--o->.  .--o->.  .--o->.   .--o->.  .--o->.
 |  |  |  |  |  |  |  |  |   |  |  |  |XX|  |  |XX|  |   |XX|  |  |XX|  |  |  |XX|  |  |XX|   |  |XX|  |  |XX|  |XX|XX|  |XX|XX|   |XX|XX|  |XX|XX|
 .--.--.  .--.--.  .--.--.   .--.--.  .--.--.  .--.--.   .--.--.  .--.--.  .--.--.  .--.--.   .--.--.  .--.--.  .--.--.  .--.--.   .--.--.  .--.--.
#endif

                bool hasUpLeft    = shapePoint.has("upLeftPixel") && (&shapePoint["upLeftPixel"]["shape"] == &currentShape);       // 🡬
                bool hasUpRight   = shapePoint.has("upRightPixel") && (&shapePoint["upRightPixel"]["shape"] == &currentShape);     // 🡭
                bool hasDownLeft  = shapePoint.has("downLeftPixel") && (&shapePoint["downLeftPixel"]["shape"] == &currentShape);   // 🡯
                bool hasDownRight = shapePoint.has("downRightPixel") && (&shapePoint["downRightPixel"]["shape"] == &currentShape); // 🡮

                int caseNum = 1 + (int)hasUpLeft + ((int)hasUpRight * 2) + ((int)hasDownLeft * 4) + ((int)hasDownRight * 8);
                if (caseNum == 1) {
                    ERROR("Invalid pixel state");
                    throw "Invalid pixel state";
                } else {
                    std::cout << "";
                }

                switch (processingMode) {
                case ProcessingMode::ExternalEdge: {
                    // create new edge
                    CellI& newEdge  = *new Object(kb, ShapeEdgeStruct);
                    List& edgeNodes = *new List(kb, ShapeEdgeNodeStruct);
                    newEdge.set("shape", currentShape);
                    newEdge.set("edgeNodes", edgeNodes);
                    Map* edgesPtr = nullptr;
                    if (currentShape.missing("edges")) {
                        Map& newEdges = *new Map(kb, kb.std.Number, ShapeEdgeStruct);
                        currentShape.set("edges", newEdges);
                        newEdge.set("id", _1_);
                        newEdge.set("kind", ExternalEdgeEV);
                        edgesPtr = &newEdges;
                    } else {
                        std::cerr << "There is already an external edge!";
                        throw "Edge detection error!";
                    }
                    Map& edges = *edgesPtr;
                    edges.add(newEdge["id"], newEdge);

                    CellI* currentShapePointPtr = &shapePoint;
                    bool firstIteration         = true;

                    while ((currentShapePointPtr != &shapePoint) || firstIteration) {
                        firstIteration    = false;
                        CellI& shapePoint = *currentShapePointPtr;

                        if (processingDirectionPtr == &DirectionRightEV) {
                            bool hasUpRight   = shapePoint.has("upRightPixel") && (&shapePoint["upRightPixel"]["shape"] == &currentShape);     // 🡭
                            bool hasDownRight = shapePoint.has("downRightPixel") && (&shapePoint["downRightPixel"]["shape"] == &currentShape); // 🡮

                            if (hasUpRight && hasDownRight) {
                                // .--^--.
                                // |  |XX|
                                // .-->--.
                                // |XX|XX|
                                // .--.--.
                                processingDirectionPtr = &DirectionUpEV;
                            } else if (hasUpRight && !hasDownRight) {
                                // .--^--.
                                // |  |XX|
                                // .-->--.
                                // |XX|  |
                                // .--.--.
                                processingDirectionPtr = &DirectionUpEV;
                            } else if (!hasUpRight && hasDownRight) {
                                // .--.--.
                                // |  |  |
                                // .-->-->
                                // |XX|XX|
                                // .--.--.
                                processingDirectionPtr = &DirectionRightEV;
                            } else if (!hasUpRight && !hasDownRight) {
                                // .--.--.
                                // |  |  |
                                // .-->--.
                                // |XX|  |
                                // .--v--.
                                processingDirectionPtr = &DirectionDownEV;
                            }
                        } else if (processingDirectionPtr == &DirectionLeftEV) {
                            bool hasUpLeft   = shapePoint.has("upLeftPixel") && (&shapePoint["upLeftPixel"]["shape"] == &currentShape);     // 🡬
                            bool hasDownLeft = shapePoint.has("downLeftPixel") && (&shapePoint["downLeftPixel"]["shape"] == &currentShape); // 🡯

                            if (hasUpLeft && hasDownLeft) {
                                // .--.--.
                                // |XX|XX|
                                // .--<--.
                                // |XX|  |
                                // .--v--.
                                processingDirectionPtr = &DirectionDownEV;
                            } else if (hasUpLeft && !hasDownLeft) {
                                // .--.--.
                                // |XX|XX|
                                // <--<--.
                                // |  |  |
                                // .--.--.
                                processingDirectionPtr = &DirectionLeftEV;
                            } else if (!hasUpLeft && hasDownLeft) {
                                // .--.--.
                                // |  |XX|
                                // .--<--.
                                // |XX|  |
                                // .--v--.
                                processingDirectionPtr = &DirectionDownEV;
                            } else if (!hasUpLeft && !hasDownLeft) {
                                // .--^--.
                                // |  |XX|
                                // .--<--.
                                // |  |  |
                                // .--.--.
                                processingDirectionPtr = &DirectionUpEV;
                            }
                        } else if (processingDirectionPtr == &DirectionUpEV) {
                            bool hasUpLeft  = shapePoint.has("upLeftPixel") && (&shapePoint["upLeftPixel"]["shape"] == &currentShape);   // 🡬
                            bool hasUpRight = shapePoint.has("upRightPixel") && (&shapePoint["upRightPixel"]["shape"] == &currentShape); // 🡭

                            if (hasUpLeft && hasUpRight) {
                                // .--.--.
                                // |XX|XX|
                                // <--^--.
                                // |  |XX|
                                // .--.--.
                                processingDirectionPtr = &DirectionLeftEV;
                            } else if (hasUpLeft && !hasUpRight) {
                                // .--.--.
                                // |XX|  |
                                // <--^--.
                                // |  |XX|
                                // .--.--.
                                processingDirectionPtr = &DirectionLeftEV;
                            } else if (!hasUpLeft && hasUpRight) {
                                // .--^--.
                                // |  |XX|
                                // .--^--.
                                // |  |XX|
                                // .--.--.
                                processingDirectionPtr = &DirectionUpEV;
                            } else if (!hasUpLeft && !hasUpRight) {
                                // .--.--.
                                // |  |  |
                                // .--^-->
                                // |  |XX|
                                // .--.--.
                                processingDirectionPtr = &DirectionRightEV;
                            }
                        } else if (processingDirectionPtr == &DirectionDownEV) {
                            bool hasDownLeft  = shapePoint.has("downLeftPixel") && (&shapePoint["downLeftPixel"]["shape"] == &currentShape);   // 🡯
                            bool hasDownRight = shapePoint.has("downRightPixel") && (&shapePoint["downRightPixel"]["shape"] == &currentShape); // 🡮

                            if (hasDownLeft && hasDownRight) {
                                // .--.--.
                                // |XX|  |
                                // .--v-->
                                // |XX|XX|
                                // .--.--.
                                processingDirectionPtr = &DirectionRightEV;
                            } else if (hasDownLeft && !hasDownRight) {
                                // .--.--.
                                // |XX|  |
                                // .--v--.
                                // |XX|  |
                                // .--v--.
                                processingDirectionPtr = &DirectionDownEV;
                            } else if (!hasDownLeft && hasDownRight) {
                                // .--.--.
                                // |XX|  |
                                // .--v-->
                                // |  |XX|
                                // .--.--.
                                processingDirectionPtr = &DirectionRightEV;
                            } else if (!hasDownLeft && !hasDownRight) {
                                // .--.--.
                                // |XX|  |
                                // <--v--.
                                // |  |  |
                                // .--.--.
                                processingDirectionPtr = &DirectionLeftEV;
                            }
                        }
                        const char* toDirectionStr = "";
                        if (processingDirectionPtr == &DirectionRightEV) {
                            toDirectionStr = "right";
                        } else if (processingDirectionPtr == &DirectionLeftEV) {
                            toDirectionStr = "left";
                        } else if (processingDirectionPtr == &DirectionUpEV) {
                            toDirectionStr = "up";
                        } else if (processingDirectionPtr == &DirectionDownEV) {
                            toDirectionStr = "down";
                        }
                        const char* oppositeDirectionStr = "";
                        if (processingDirectionPtr == &DirectionRightEV) {
                            oppositeDirectionStr = "left";
                        } else if (processingDirectionPtr == &DirectionLeftEV) {
                            oppositeDirectionStr = "right";
                        } else if (processingDirectionPtr == &DirectionUpEV) {
                            oppositeDirectionStr = "down";
                        } else if (processingDirectionPtr == &DirectionDownEV) {
                            oppositeDirectionStr = "up";
                        }
                        CellI& toShapePoint = shapePoint[toDirectionStr];

                        // from joint
                        CellI* fromEdgeJointPtr = nullptr;
                        if (shapePoint.has("edgeJoint")) {
                            fromEdgeJointPtr = &shapePoint["edgeJoint"];
                        } else {
                            fromEdgeJointPtr = new Object(kb, ShapeEdgeJointStruct);
                            shapePoint.set("edgeJoint", *fromEdgeJointPtr);
                        }
                        CellI& fromEdgeJoint = *fromEdgeJointPtr;

                        CellI* edgeNodePtr      = nullptr;
                        if (fromEdgeJoint.has(toDirectionStr)) {
                            CellI& oldEdgeNode = fromEdgeJoint[toDirectionStr];
                            if (processingDirectionPtr == &DirectionRightEV || processingDirectionPtr == &DirectionDownEV) {
                                if (oldEdgeNode.has("rightSide")) {
                                    ERROR("Edge processing error> oldEdgeNode.has(\"rightSide\")");
                                    throw "Edge processing error";
                                }
                            } else if (processingDirectionPtr == &DirectionLeftEV || processingDirectionPtr == &DirectionUpEV) {
                                if (oldEdgeNode.has("leftSide")) {
                                    ERROR("Edge processing error> oldEdgeNode.has(\"leftSide\")");
                                    throw "Edge processing error";
                                }
                            }
                            edgeNodePtr = &oldEdgeNode;
                        } else {
                            CellI& newEdgeNode = *new Object(kb, ShapeEdgeNodeStruct);
                            if (processingDirectionPtr == &DirectionRightEV || processingDirectionPtr == &DirectionDownEV) {
                                newEdgeNode.set("from", shapePoint);
                                newEdgeNode.set("direction", *processingDirectionPtr);
                                newEdgeNode.set("rightSide", newEdge);
                            } else {
                                newEdgeNode.set("from", toShapePoint);
                                newEdgeNode.set("direction", processingDirectionPtr == &DirectionLeftEV ? DirectionRightEV : DirectionDownEV);
                                newEdgeNode.set("leftSide", newEdge);
                            }
                            edgeNodePtr = &newEdgeNode;
                        }
                        CellI& edgeNode = *edgeNodePtr;
                        if (processingDirectionPtr == &DirectionRightEV || processingDirectionPtr == &DirectionDownEV) {
                            edgeNode.set("rightSide", newEdge);
                        } else {
                            edgeNode.set("leftSide", newEdge);
                        }

                        edgeNodes.add(edgeNode);

                        // to joint
                        CellI* toEdgeJointPtr = nullptr;
                        if (toShapePoint.has("edgeJoint")) {
                            toEdgeJointPtr = &toShapePoint["edgeJoint"];
                        } else {
                            toEdgeJointPtr = new Object(kb, ShapeEdgeJointStruct);
                            toShapePoint.set("edgeJoint", *toEdgeJointPtr);
                        }

                        CellI& toEdgeJoint = *toEdgeJointPtr;
                        currentShapePointPtr = &toShapePoint;

                        fromEdgeJoint.set(toDirectionStr, edgeNode);
                        toEdgeJoint.set(oppositeDirectionStr, edgeNode);
                    }

                    processingMode = ProcessingMode::Searching;
                } break;

                case ProcessingMode::InternalEdge: {
                    // create new edge
                    CellI& newEdge  = *new Object(kb, ShapeEdgeStruct);
                    List& edgeNodes = *new List(kb, ShapeEdgeNodeStruct);
                    newEdge.set("shape", currentShape);
                    newEdge.set("edgeNodes", edgeNodes);

                    Map& edges = static_cast<Map&>(currentShape["edges"]);
                    CellI& newEdgeId = kb.pools.numbers.get(static_cast<Number&>(currentShape["lastEdgeId"]).value() + 1);
                    currentShape.set("lastEdgeId", newEdgeId);
                    newEdge.set("id", newEdgeId);
                    newEdge.set("kind", InternalEdgeEV);
                    edges.add(newEdge["id"], newEdge);

                    CellI* currentShapePointPtr = &shapePoint;
                    bool firstIteration         = true;

                    while ((currentShapePointPtr != &shapePoint) || firstIteration) {
                        firstIteration    = false;
                        CellI& shapePoint = *currentShapePointPtr;

                        if (processingDirectionPtr == &DirectionRightEV) {
                            bool hasUpRight   = shapePoint.has("upRightPixel") && (&shapePoint["upRightPixel"]["shape"] == &currentShape);     // 🡭
                            bool hasDownRight = shapePoint.has("downRightPixel") && (&shapePoint["downRightPixel"]["shape"] == &currentShape); // 🡮

                            if (hasUpRight && hasDownRight) {
                                // .--.--.
                                // |XX|XX|
                                // .-->--.
                                // |  |XX|
                                // .--v--.
                                processingDirectionPtr = &DirectionDownEV;
                            } else if (hasUpRight && !hasDownRight) {
                                // .--.--.
                                // |XX|XX|
                                // .-->-->
                                // |  |  |
                                // .--.--.
                                processingDirectionPtr = &DirectionRightEV;
                            } else if (!hasUpRight && hasDownRight) {
                                // .--^--.
                                // |XX|  |
                                // .-->--.
                                // |  |XX|
                                // .--.--.
                                processingDirectionPtr = &DirectionUpEV;
                                if (shapePoint.has("edgeJoint")) {
                                    CellI& fromEdgeJointPtr = shapePoint["edgeJoint"];
                                    if (shapePoint["edgeJoint"].has("up") && shapePoint["edgeJoint"].has("right")) {
                                        CellI& upEdgeNode   = shapePoint["edgeJoint"]["up"];
                                        CellI& rightEdgeNode = shapePoint["edgeJoint"]["right"];
                                        if (upEdgeNode.has("rightSide") && (&upEdgeNode["rightSide"]["kind"] == &ExternalEdgeEV) && rightEdgeNode.has("rightSide") && (&rightEdgeNode["rightSide"]["kind"] == &ExternalEdgeEV)) {
                                            processingDirectionPtr = &DirectionDownEV;
                                        }
                                    }
                                }
                            } else if (!hasUpRight && !hasDownRight) {
                                // .--^--.
                                // |XX|  |
                                // .-->--.
                                // |  |  |
                                // .--.--.
                                processingDirectionPtr = &DirectionUpEV;
                            }
                        } else if (processingDirectionPtr == &DirectionLeftEV) {
                            bool hasUpLeft   = shapePoint.has("upLeftPixel") && (&shapePoint["upLeftPixel"]["shape"] == &currentShape);     // 🡬
                            bool hasDownLeft = shapePoint.has("downLeftPixel") && (&shapePoint["downLeftPixel"]["shape"] == &currentShape); // 🡯

                            if (hasUpLeft && hasDownLeft) {
                                // .--^--.
                                // |XX|  |
                                // .--<--.
                                // |XX|XX|
                                // .--.--.
                                processingDirectionPtr = &DirectionUpEV;
                            } else if (hasUpLeft && !hasDownLeft) {
                                // .--.--.
                                // |XX|  |
                                // .--<--.
                                // |  |XX|
                                // .--v--.
                                processingDirectionPtr = &DirectionDownEV;
                                if (shapePoint.has("edgeJoint")) {
                                    CellI& fromEdgeJointPtr = shapePoint["edgeJoint"];
                                    if (shapePoint["edgeJoint"].has("down") && shapePoint["edgeJoint"].has("left")) {
                                        CellI& downEdgeNode  = shapePoint["edgeJoint"]["down"];
                                        CellI& leftEdgeNode = shapePoint["edgeJoint"]["left"];
                                        if (downEdgeNode.has("leftSide") && (&downEdgeNode["leftSide"]["kind"] == &ExternalEdgeEV) && leftEdgeNode.has("leftSide") && (&leftEdgeNode["leftSide"]["kind"] == &ExternalEdgeEV)) {
                                            processingDirectionPtr = &DirectionUpEV;
                                        }
                                    }
                                }
                            } else if (!hasUpLeft && hasDownLeft) {
                                // .--.--.
                                // |  |  |
                                // <--<--.
                                // |XX|XX|
                                // .--.--.
                                processingDirectionPtr = &DirectionLeftEV;
                            } else if (!hasUpLeft && !hasDownLeft) {
                                // .--.--.
                                // |  |  |
                                // .--<--.
                                // |  |XX|
                                // .--v--.
                                processingDirectionPtr = &DirectionDownEV;
                            }
                        } else if (processingDirectionPtr == &DirectionUpEV) {
                            bool hasUpLeft  = shapePoint.has("upLeftPixel") && (&shapePoint["upLeftPixel"]["shape"] == &currentShape);   // 🡬
                            bool hasUpRight = shapePoint.has("upRightPixel") && (&shapePoint["upRightPixel"]["shape"] == &currentShape); // 🡭

                            if (hasUpLeft && hasUpRight) {
                                // .--.--.
                                // |XX|XX|
                                // .--^-->
                                // |XX|  |
                                // .--.--.
                                processingDirectionPtr = &DirectionRightEV;
                            } else if (hasUpLeft && !hasUpRight) {
                                // .--^--.
                                // |XX|  |
                                // .--^--.
                                // |XX|  |
                                // .--.--.
                                processingDirectionPtr = &DirectionUpEV;
                            } else if (!hasUpLeft && hasUpRight) {
                                // .--.--.
                                // |  |XX|
                                // <--^--.
                                // |XX|  |
                                // .--.--.
                                processingDirectionPtr = &DirectionLeftEV;
                                if (shapePoint.has("edgeJoint")) {
                                    CellI& fromEdgeJointPtr = shapePoint["edgeJoint"];
                                    if (shapePoint["edgeJoint"].has("up") && shapePoint["edgeJoint"].has("left")) {
                                        CellI& upEdgeNode    = shapePoint["edgeJoint"]["up"];
                                        CellI& leftEdgeNode = shapePoint["edgeJoint"]["left"];
                                        if (upEdgeNode.has("leftSide") && (&upEdgeNode["leftSide"]["kind"] == &ExternalEdgeEV) && leftEdgeNode.has("rightSide") && (&leftEdgeNode["rightSide"]["kind"] == &ExternalEdgeEV)) {
                                            processingDirectionPtr = &DirectionRightEV;
                                        }
                                    }
                                }
                            } else if (!hasUpLeft && !hasUpRight) {
                                // .--.--.
                                // |  |  |
                                // <--^--.
                                // |XX|  |
                                // .--.--.
                                processingDirectionPtr = &DirectionLeftEV;
                            }
                        } else if (processingDirectionPtr == &DirectionDownEV) {
                            bool hasDownLeft  = shapePoint.has("downLeftPixel") && (&shapePoint["downLeftPixel"]["shape"] == &currentShape);   // 🡯
                            bool hasDownRight = shapePoint.has("downRightPixel") && (&shapePoint["downRightPixel"]["shape"] == &currentShape); // 🡮

                            if (hasDownLeft && hasDownRight) {
                                // .--.--.
                                // |  |XX|
                                // <--v--.
                                // |XX|XX|
                                // .--.--.
                                processingDirectionPtr = &DirectionLeftEV;
                            } else if (hasDownLeft && !hasDownRight) {
                                // .--.--.
                                // |  |XX|
                                // .--v-->
                                // |XX|  |
                                // .--.--.
                                processingDirectionPtr = &DirectionRightEV;
                                if (shapePoint.has("edgeJoint")) {
                                    CellI& fromEdgeJointPtr = shapePoint["edgeJoint"];
                                    if (shapePoint["edgeJoint"].has("down") && shapePoint["edgeJoint"].has("right")) {
                                        CellI& downEdgeNode  = shapePoint["edgeJoint"]["down"];
                                        CellI& rightEdgeNode = shapePoint["edgeJoint"]["right"];
                                        if (downEdgeNode.has("rightSide") && (&downEdgeNode["rightSide"]["kind"] == &ExternalEdgeEV) && rightEdgeNode.has("leftSide") && (&rightEdgeNode["leftSide"]["kind"] == &ExternalEdgeEV)) {
                                            processingDirectionPtr = &DirectionLeftEV;
                                        }
                                    }
                                }
                            } else if (!hasDownLeft && hasDownRight) {
                                // .--.--.
                                // |  |XX|
                                // .--v--.
                                // |  |XX|
                                // .--v--.
                                processingDirectionPtr = &DirectionDownEV;
                            } else if (!hasDownLeft && !hasDownRight) {
                                // .--.--.
                                // |  |XX|
                                // .--v-->
                                // |  |  |
                                // .--.--.
                                processingDirectionPtr = &DirectionRightEV;
                            }
                        }
                        const char* toDirectionStr = "";
                        if (processingDirectionPtr == &DirectionRightEV) {
                            toDirectionStr = "right";
                        } else if (processingDirectionPtr == &DirectionLeftEV) {
                            toDirectionStr = "left";
                        } else if (processingDirectionPtr == &DirectionUpEV) {
                            toDirectionStr = "up";
                        } else if (processingDirectionPtr == &DirectionDownEV) {
                            toDirectionStr = "down";
                        }
                        const char* oppositeDirectionStr = "";
                        if (processingDirectionPtr == &DirectionRightEV) {
                            oppositeDirectionStr = "left";
                        } else if (processingDirectionPtr == &DirectionLeftEV) {
                            oppositeDirectionStr = "right";
                        } else if (processingDirectionPtr == &DirectionUpEV) {
                            oppositeDirectionStr = "down";
                        } else if (processingDirectionPtr == &DirectionDownEV) {
                            oppositeDirectionStr = "up";
                        }
                        CellI& toShapePoint = shapePoint[toDirectionStr];

                        // from joint
                        CellI* fromEdgeJointPtr = nullptr;
                        if (shapePoint.has("edgeJoint")) {
                            fromEdgeJointPtr = &shapePoint["edgeJoint"];
                        } else {
                            fromEdgeJointPtr = new Object(kb, ShapeEdgeJointStruct);
                            shapePoint.set("edgeJoint", *fromEdgeJointPtr);
                        }
                        CellI& fromEdgeJoint = *fromEdgeJointPtr;

                        CellI* edgeNodePtr = nullptr;
                        if (fromEdgeJoint.has(toDirectionStr)) {
                            CellI& oldEdgeNode = fromEdgeJoint[toDirectionStr];
                            if (processingDirectionPtr == &DirectionRightEV || processingDirectionPtr == &DirectionDownEV) {
                                if (oldEdgeNode.has("leftSide")) {
                                    ERROR("Edge processing error> oldEdgeNode.has(\"leftSide\")");
                                    throw "Edge processing error";
                                }
                            } else if (processingDirectionPtr == &DirectionLeftEV || processingDirectionPtr == &DirectionUpEV) {
                                if (oldEdgeNode.has("rightSide")) {
                                    ERROR("Edge processing error> oldEdgeNode.has(\"rightSide\")");
                                    throw "Edge processing error";
                                }
                            }
                            edgeNodePtr = &oldEdgeNode;
                        } else {
                            CellI& newEdgeNode = *new Object(kb, ShapeEdgeNodeStruct);
                            if (processingDirectionPtr == &DirectionRightEV || processingDirectionPtr == &DirectionDownEV) {
                                newEdgeNode.set("from", shapePoint);
                                newEdgeNode.set("direction", *processingDirectionPtr);
                                newEdgeNode.set("leftSide", newEdge);
                            } else {
                                newEdgeNode.set("from", toShapePoint);
                                newEdgeNode.set("direction", processingDirectionPtr == &DirectionLeftEV ? DirectionRightEV : DirectionDownEV);
                                newEdgeNode.set("rightSide", newEdge);
                            }
                            edgeNodePtr = &newEdgeNode;
                        }
                        CellI& edgeNode = *edgeNodePtr;
                        if (processingDirectionPtr == &DirectionRightEV || processingDirectionPtr == &DirectionDownEV) {
                            edgeNode.set("leftSide", newEdge);
                        } else {
                            edgeNode.set("rightSide", newEdge);
                        }

                        edgeNodes.add(edgeNode);

                        // to joint
                        CellI* toEdgeJointPtr = nullptr;
                        if (toShapePoint.has("edgeJoint")) {
                            toEdgeJointPtr = &toShapePoint["edgeJoint"];
                        } else {
                            toEdgeJointPtr = new Object(kb, ShapeEdgeJointStruct);
                            toShapePoint.set("edgeJoint", *toEdgeJointPtr);
                        }

                        CellI& toEdgeJoint   = *toEdgeJointPtr;
                        currentShapePointPtr = &toShapePoint;

                        fromEdgeJoint.set(toDirectionStr, edgeNode);
                        toEdgeJoint.set(oppositeDirectionStr, edgeNode);
                    }
                    currentListItemPtr = currentListItem.has(kb.ids.next) ? &currentListItem[kb.ids.next] : nullptr;
                    processingMode = ProcessingMode::Searching;
                } break;
                case ProcessingMode::Searching: {
                    bool isUnprocessedEdge = false;

                    if (hasUpLeft && hasUpRight && hasDownLeft && !hasDownRight) {
                        // 1🡬 1🡭
                        // 1🡯 0🡮
                        // .--.--.
                        // |XX|XX|
                        // .--o->.
                        // |XX|  |
                        // .--.--.
                        if (shapePoint.has("edgeJoint")) {
                            CellI& fromEdgeJointPtr = shapePoint["edgeJoint"];
                            if (shapePoint["edgeJoint"].has("right")) {
                                CellI& edgeNode = shapePoint["edgeJoint"]["right"];
                                if (edgeNode.missing("leftSide")) {
                                    isUnprocessedEdge = true;
                                }
                            } else {
                                isUnprocessedEdge = true;
                            }
                        } else {
                            isUnprocessedEdge = true;
                        }
                    } else if (!hasUpLeft && hasUpRight && hasDownLeft && !hasDownRight) {
                        // 0🡬 1🡭
                        // 1🡯 0🡮
                        // .--A--.
                        // |  |XX|
                        // B--o-->
                        // |XX|  |
                        // .--.--.
                        if (shapePoint.has("edgeJoint")) {
                            CellI& fromEdgeJointPtr = shapePoint["edgeJoint"];
                            if (shapePoint["edgeJoint"].has("left") && shapePoint["edgeJoint"].has("up") && shapePoint["edgeJoint"].missing("right")) {
                                CellI& upEdgeNode   = shapePoint["edgeJoint"]["up"];
                                CellI& leftEdgeNode = shapePoint["edgeJoint"]["left"];
                                if (upEdgeNode.has("leftSide") && (&upEdgeNode["leftSide"]["kind"] == &ExternalEdgeEV) && leftEdgeNode.has("rightSide") && (&leftEdgeNode["rightSide"]["kind"] == &ExternalEdgeEV)) {
                                    isUnprocessedEdge = true;
                                }
                            }
                        }
                    }
                    if (isUnprocessedEdge) {
                        processingDirectionPtr = &DirectionRightEV;
                        processingMode = ProcessingMode::InternalEdge;
                    } else {
                        currentListItemPtr = currentListItem.has(kb.ids.next) ? &currentListItem[kb.ids.next] : nullptr;
                    }
                } break;
                } // switch processinMode
            } // while has more shapePoints
        }); // visit shapePoints
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

    void sortEdges()
    {
        CellI* firstColumnPointPtr  = &(*firstShapePixelPtr())["upLeftPoint"];
        CellI* currentShapePointPtr = firstColumnPointPtr;
        CellI* lastShapeEdgeInLine  = nullptr;
        List internalEdges(kb, ShapeEdgeStruct);
        List::Item* lastInternalEdgeItem = nullptr;
        enum class ProcessingDirection
        {
            LeftToRight,
            UpToDown
        };

        ProcessingDirection processingDirection = ProcessingDirection::LeftToRight;

        while (currentShapePointPtr) {
            CellI& currentShapePoint = *currentShapePointPtr;
            const int x              = static_cast<Number&>(currentShapePoint["x"]).value();
            const int y              = static_cast<Number&>(currentShapePoint["y"]).value();
            switch (processingDirection) {
            case ProcessingDirection::LeftToRight: {
                if (currentShapePoint.has("edgeJoint") && currentShapePoint["edgeJoint"].has("right")) {
                    CellI& edgeJoint       = currentShapePoint["edgeJoint"];
                    CellI& edgeNodeAtRight = edgeJoint["right"];
                    if (edgeNodeAtRight.has("leftSide")) {
                        CellI& shapeEdge           = edgeNodeAtRight["leftSide"];
                        CellI* orderedEdgeNodesPtr = nullptr;
                        if (shapeEdge.missing("orderedEdgeNodes")) {
                            List& edgeNodes = *new List(kb, ShapeEdgeNodeStruct);
                            shapeEdge.set("orderedEdgeNodes", edgeNodes);
                            orderedEdgeNodesPtr = &edgeNodes;
                        } else {
                            orderedEdgeNodesPtr = &shapeEdge["orderedEdgeNodes"];
                        }
                        List& orderedEdgeNodes = static_cast<List&>(*orderedEdgeNodesPtr);
                        orderedEdgeNodes.add(edgeNodeAtRight);
                    }
                    if (edgeNodeAtRight.has("rightSide")) {
                        CellI& shapeEdge = edgeNodeAtRight["rightSide"];
                        CellI* orderedEdgeNodesPtr = nullptr;
                        if (shapeEdge.missing("orderedEdgeNodes")) {
                            List& edgeNodes = *new List(kb, ShapeEdgeNodeStruct);
                            shapeEdge.set("orderedEdgeNodes", edgeNodes);
                            orderedEdgeNodesPtr = &edgeNodes;
                        } else {
                            orderedEdgeNodesPtr = &shapeEdge["orderedEdgeNodes"];
                        }
                        List& orderedEdgeNodes = static_cast<List&>(*orderedEdgeNodesPtr);
                        orderedEdgeNodes.add(edgeNodeAtRight);
                    }
                }
            } break;
            case ProcessingDirection::UpToDown: {
                if (currentShapePoint.has("edgeJoint") && currentShapePoint["edgeJoint"].has("down")) {
                    CellI& edgeJoint = currentShapePoint["edgeJoint"];
                    if (edgeJoint.has("down")) {
                        CellI& edgeNodeAtDown = edgeJoint["down"];
                        if (edgeNodeAtDown.has("rightSide")) {
                            CellI& shapeEdge           = edgeNodeAtDown["rightSide"];
                            if (&shapeEdge["kind"] == &InternalEdgeEV) {
                                // entering an internal edge
                                lastInternalEdgeItem = internalEdges.add(shapeEdge);
                            }
                            CellI* orderedEdgeNodesPtr = nullptr;
                            if (shapeEdge.missing("orderedEdgeNodes")) {
                                List& edgeNodes = *new List(kb, ShapeEdgeNodeStruct);
                                shapeEdge.set("orderedEdgeNodes", edgeNodes);
                                orderedEdgeNodesPtr = &edgeNodes;
                            } else {
                                orderedEdgeNodesPtr = &shapeEdge["orderedEdgeNodes"];
                            }
                            List& orderedEdgeNodes = static_cast<List&>(*orderedEdgeNodesPtr);
                            orderedEdgeNodes.add(edgeNodeAtDown);
                        }
                        if (edgeNodeAtDown.has("leftSide")) {
                            CellI& shapeEdge           = edgeNodeAtDown["leftSide"];
                            if (&shapeEdge["kind"] == &InternalEdgeEV) {
                                // leaving an internal edge
                                internalEdges.remove(lastInternalEdgeItem);
                                if (!internalEdges.empty()) {
                                    lastInternalEdgeItem = &static_cast<List::Item&>(internalEdges["last"]);
                                }
                            }
                            lastShapeEdgeInLine        = &shapeEdge;
                            CellI* orderedEdgeNodesPtr = nullptr;
                            if (shapeEdge.missing("orderedEdgeNodes")) {
                                List& edgeNodes = *new List(kb, ShapeEdgeNodeStruct);
                                shapeEdge.set("orderedEdgeNodes", edgeNodes);
                                orderedEdgeNodesPtr = &edgeNodes;
                            } else {
                                orderedEdgeNodesPtr = &shapeEdge["orderedEdgeNodes"];
                            }
                            List& orderedEdgeNodes = static_cast<List&>(*orderedEdgeNodesPtr);
                            orderedEdgeNodes.add(edgeNodeAtDown);
                        }
                    }
                }
                if (currentShapePoint.has("downRightPixel")) {
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
                    CellI& shapePixel   = currentShapePoint["downRightPixel"];
                    edgePixelList.add(shapePixel);
                    if (!internalEdges.empty()) {
                        CellI& lastInternalEdge = internalEdges["last"]["value"];
                        CellI* shapeSetPtr = nullptr;
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
            }
            }


            if (currentShapePoint.has("right")) {
                currentShapePointPtr = &currentShapePoint["right"];
            } else if (currentShapePoint.has("down")) {
                switch (processingDirection) {
                case ProcessingDirection::LeftToRight:
                    processingDirection  = ProcessingDirection::UpToDown;
                    currentShapePointPtr = firstColumnPointPtr;
                    break;
                case ProcessingDirection::UpToDown:
                    processingDirection  = ProcessingDirection::LeftToRight;
                    currentShapePointPtr = &(*firstColumnPointPtr)["down"];
                    firstColumnPointPtr  = currentShapePointPtr;
                    break;
                }
            } else {
                currentShapePointPtr = nullptr;
            }
        }
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
     - Shape {
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

    expectedShapeIds(R"(111111111
                        111111111
                        121311111
                        114111111
                        151611111
                        111111711
                        111117871
                        111111711)");
    expectedShapesCount(8);
    expectedShapeEdgeCounts({ { 1, 3 }, { 2, 1 }, { 3, 1 }, { 4, 1 }, { 5, 1 }, { 6, 1 }, { 7, 2 }, { 8, 1 } });
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

    expectedShapeIds(R"(111211111
                        111111111
                        111111311
                        114111111
                        111111111
                        111111111
                        111111511
                        161111111
                        111111111)");
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

    expectedShapesCount(16);
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

TEST_F(EdgeTester, EdgeTestWithArc_0b148d64_minified_Train0Input)
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

    std::cout << "Before sort: " << std::endl;
    Visitor::visitList(externalEdge["edgeNodes"], [this](CellI& edgeNode, int, bool&) {
        std::cout << fmt::format("({},{}){}", static_cast<Number&>(edgeNode["from"]["x"]).value(), static_cast<Number&>(edgeNode["from"]["y"]).value(), &edgeNode["direction"] == &DirectionRightEV ? "-" : "|") << std::endl;
    });
    std::cout << "After sort: " << std::endl;
    Visitor::visitList(externalEdge["orderedEdgeNodes"], [this](CellI& edgeNode, int, bool&) {
        std::cout << fmt::format("({},{}){}", static_cast<Number&>(edgeNode["from"]["x"]).value(), static_cast<Number&>(edgeNode["from"]["y"]).value(), &edgeNode["direction"] == &DirectionRightEV ? "-" : "|") << std::endl;
    });

    CellI* firstColumnPointPtr  = &(*firstShapePixelPtr())["upLeftPoint"];
    CellI* currentShapePointPtr = firstColumnPointPtr;
    CellI* previousShapePointPtr = nullptr;

    EXPECT_TRUE((*currentShapePointPtr).has("edgeJoint"));
    EXPECT_EQ(&(*currentShapePointPtr)["x"], &_0_);
    EXPECT_EQ(&(*currentShapePointPtr)["y"], &_0_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("up"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("down"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"]["down"].has("rightSide"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"]["down"].has("leftSide"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["down"]["leftSide"]["shape"]["id"], &_1_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("left"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("right"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"]["right"].has("rightSide"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["right"]["rightSide"]["shape"]["id"], &_1_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"]["right"].has("leftSide"));

    previousShapePointPtr = currentShapePointPtr;
    currentShapePointPtr = &(*currentShapePointPtr)["right"];
    EXPECT_EQ(&(*currentShapePointPtr)["x"], &_1_);
    EXPECT_EQ(&(*currentShapePointPtr)["y"], &_0_);
    EXPECT_TRUE((*currentShapePointPtr).has("edgeJoint"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("up"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("down"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"]["down"].has("rightSide"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["down"]["rightSide"]["shape"]["id"], &_1_);
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"]["down"].has("leftSide"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["down"]["leftSide"]["shape"]["id"], &_2_);
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("left"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["left"]["from"], previousShapePointPtr);
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["left"]["direction"], &DirectionRightEV);
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"]["left"].has("rightSide"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["left"]["rightSide"]["shape"]["id"], &_1_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"]["left"].has("leftSide"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("right"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["right"]["from"], currentShapePointPtr);
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["right"]["direction"], &DirectionRightEV);
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"]["right"].has("rightSide"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["right"]["rightSide"]["shape"]["id"], &_2_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"]["right"].has("leftSide"));

    previousShapePointPtr = currentShapePointPtr;
    currentShapePointPtr  = &(*currentShapePointPtr)["right"];
    EXPECT_EQ(&(*currentShapePointPtr)["x"], &_2_);
    EXPECT_EQ(&(*currentShapePointPtr)["y"], &_0_);
    EXPECT_TRUE((*currentShapePointPtr).has("edgeJoint"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("up"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("down"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("left"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"]["left"].has("rightSide"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["left"]["rightSide"]["shape"]["id"], &_2_);
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("right"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"]["right"].has("rightSide"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["right"]["rightSide"]["shape"]["id"], &_2_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"]["right"].has("leftSide"));

    previousShapePointPtr = currentShapePointPtr;
    currentShapePointPtr  = &(*currentShapePointPtr)["right"];
    EXPECT_EQ(&(*currentShapePointPtr)["x"], &_3_);
    EXPECT_EQ(&(*currentShapePointPtr)["y"], &_0_);
    EXPECT_TRUE((*currentShapePointPtr).has("edgeJoint"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("up"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("down"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"]["down"].has("rightSide"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["down"]["rightSide"]["shape"]["id"], &_2_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"]["down"].has("leftSide"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("left"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("right"));

    previousShapePointPtr = nullptr;
    currentShapePointPtr  = &(*firstColumnPointPtr)["down"];
    firstColumnPointPtr   = currentShapePointPtr;
    EXPECT_EQ(&(*currentShapePointPtr)["x"], &_0_);
    EXPECT_EQ(&(*currentShapePointPtr)["y"], &_1_);
    EXPECT_TRUE((*currentShapePointPtr).has("edgeJoint"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("up"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"]["up"].has("rightSide"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"]["up"].has("leftSide"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["up"]["leftSide"]["shape"]["id"], &_1_);
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("down"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"]["down"].has("rightSide"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"]["down"].has("leftSide"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["down"]["leftSide"]["shape"]["id"], &_2_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("left"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("right"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"]["right"].has("rightSide"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["right"]["rightSide"]["shape"]["id"], &_2_);
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"]["right"].has("leftSide"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["right"]["leftSide"]["shape"]["id"], &_1_);

    previousShapePointPtr = currentShapePointPtr;
    currentShapePointPtr  = &(*currentShapePointPtr)["right"];
    EXPECT_EQ(&(*currentShapePointPtr)["x"], &_1_);
    EXPECT_EQ(&(*currentShapePointPtr)["y"], &_1_);
    EXPECT_TRUE((*currentShapePointPtr).has("edgeJoint"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("up"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"]["up"].has("rightSide"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["up"]["rightSide"]["shape"]["id"], &_1_);
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"]["up"].has("leftSide"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["up"]["leftSide"]["shape"]["id"], &_2_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("down"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("left"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"]["left"].has("rightSide"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["left"]["rightSide"]["shape"]["id"], &_2_);
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"]["left"].has("leftSide"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["left"]["leftSide"]["shape"]["id"], &_1_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("right"));

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
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("up"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"]["up"].has("rightSide"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["up"]["rightSide"]["shape"]["id"], &_2_);
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("down"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"]["down"].has("rightSide"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["down"]["rightSide"]["shape"]["id"], &_2_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"]["down"].has("leftSide"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("left"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("right"));

    previousShapePointPtr = nullptr;
    currentShapePointPtr  = &(*firstColumnPointPtr)["down"];
    firstColumnPointPtr   = currentShapePointPtr;
    EXPECT_EQ(&(*currentShapePointPtr)["x"], &_0_);
    EXPECT_EQ(&(*currentShapePointPtr)["y"], &_2_);
    EXPECT_TRUE((*currentShapePointPtr).has("edgeJoint"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("up"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"]["up"].has("rightSide"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"]["up"].has("leftSide"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["up"]["leftSide"]["shape"]["id"], &_2_);
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("down"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"]["down"].has("rightSide"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"]["down"].has("leftSide"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["down"]["leftSide"]["shape"]["id"], &_2_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("left"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("right"));

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
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("up"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"]["up"].has("rightSide"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["up"]["rightSide"]["shape"]["id"], &_2_);
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("down"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"]["down"].has("rightSide"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["down"]["rightSide"]["shape"]["id"], &_2_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"]["down"].has("leftSide"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("left"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("right"));

    previousShapePointPtr = nullptr;
    currentShapePointPtr  = &(*firstColumnPointPtr)["down"];
    firstColumnPointPtr   = currentShapePointPtr;
    EXPECT_EQ(&(*currentShapePointPtr)["x"], &_0_);
    EXPECT_EQ(&(*currentShapePointPtr)["y"], &_3_);
    EXPECT_TRUE((*currentShapePointPtr).has("edgeJoint"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("up"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"]["up"].has("rightSide"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"]["up"].has("leftSide"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["up"]["leftSide"]["shape"]["id"], &_2_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("down"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("left"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("right"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"]["right"].has("rightSide"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"]["right"].has("leftSide"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["right"]["leftSide"]["shape"]["id"], &_2_);

    previousShapePointPtr = currentShapePointPtr;
    currentShapePointPtr  = &(*currentShapePointPtr)["right"];
    EXPECT_EQ(&(*currentShapePointPtr)["x"], &_1_);
    EXPECT_EQ(&(*currentShapePointPtr)["y"], &_3_);
    EXPECT_TRUE((*currentShapePointPtr).has("edgeJoint"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("up"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("down"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("left"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"]["left"].has("rightSide"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"]["left"].has("leftSide"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["left"]["leftSide"]["shape"]["id"], &_2_);
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("right"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"]["right"].has("rightSide"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"]["right"].has("leftSide"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["right"]["leftSide"]["shape"]["id"], &_2_);

    previousShapePointPtr = currentShapePointPtr;
    currentShapePointPtr  = &(*currentShapePointPtr)["right"];
    EXPECT_EQ(&(*currentShapePointPtr)["x"], &_2_);
    EXPECT_EQ(&(*currentShapePointPtr)["y"], &_3_);
    EXPECT_TRUE((*currentShapePointPtr).has("edgeJoint"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("up"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("down"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("left"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"]["left"].has("rightSide"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"]["left"].has("leftSide"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["left"]["leftSide"]["shape"]["id"], &_2_);
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("right"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"]["right"].has("rightSide"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"]["right"].has("leftSide"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["right"]["leftSide"]["shape"]["id"], &_2_);

    previousShapePointPtr = currentShapePointPtr;
    currentShapePointPtr  = &(*currentShapePointPtr)["right"];
    EXPECT_EQ(&(*currentShapePointPtr)["x"], &_3_);
    EXPECT_EQ(&(*currentShapePointPtr)["y"], &_3_);
    EXPECT_TRUE((*currentShapePointPtr).has("edgeJoint"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("up"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"]["up"].has("rightSide"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["up"]["rightSide"]["shape"]["id"], &_2_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("down"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"].has("left"));
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"]["left"].has("rightSide"));
    EXPECT_TRUE((*currentShapePointPtr)["edgeJoint"]["left"].has("leftSide"));
    EXPECT_EQ(&(*currentShapePointPtr)["edgeJoint"]["left"]["leftSide"]["shape"]["id"], &_2_);
    EXPECT_FALSE((*currentShapePointPtr)["edgeJoint"].has("right"));

    expectedShapeIds(R"(122
                        222
                        222)");
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
    std::cout << "Before sort: " << std::endl;
    Visitor::visitList(internalEdge["edgeNodes"], [this](CellI& edgeNode, int, bool&) {
        std::cout << fmt::format("({},{}){}", static_cast<Number&>(edgeNode["from"]["x"]).value(), static_cast<Number&>(edgeNode["from"]["y"]).value(), &edgeNode["direction"] == &DirectionRightEV ? "-" : "|") << std::endl;
    });
    std::cout << "After sort: " << std::endl;
    Visitor::visitList(internalEdge["orderedEdgeNodes"], [this](CellI& edgeNode, int, bool&) {
        std::cout << fmt::format("({},{}){}", static_cast<Number&>(edgeNode["from"]["x"]).value(), static_cast<Number&>(edgeNode["from"]["y"]).value(), &edgeNode["direction"] == &DirectionRightEV ? "-" : "|") << std::endl;
    });

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
    expectedShapeEdgeCounts({ { 1, 2 }, { 2, 1 } });
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
    TaskSet taskSet(kb, SYNTH_ARCPRIZE_PATH SYNTH_ARC_PRIZE_TRAINING_CHALLENGES_FILENAME);
//    TaskSet taskSet(kb, SYNTH_ARCPRIZE_PATH SYNTH_ARC_PRIZE_EVALUATION_CHALLENGES_FILENAME);
    for (auto& task : taskSet.m_tasks) {
        std::cout << "id: " << task.first << ", examples num:" << static_cast<List&>(task.second.m_cellExamplesList).size() << ", tests num:" << static_cast<List&>(task.second.m_cellTestsList).size() << std::endl;
        std::cout << "   examples:" << std::endl;
        Visitor::visitList(task.second.m_cellExamplesList, [this, &task](CellI& example, int i, bool&) {
            std::cout << "id: " << task.first << ", example input:" << i << std::endl;
            setOutputSVGName(std::format("EdgeTestWithArc_{}_{}{}{}", task.first, "Train", i, "Input"));
            testEdges(static_cast<hybrid::arc::Grid&>(example["input"]));
            std::cout << "id: " << task.first << ", example output:" << i << std::endl;
            setOutputSVGName(std::format("EdgeTestWithArc_{}_{}{}{}", task.first, "Train", i, "Output"));
            testEdges(static_cast<hybrid::arc::Grid&>(example["output"]));

        });
        std::cout << "   tests:" << std::endl;
        Visitor::visitList(task.second.m_cellTestsList, [this, &task](CellI& example, int i, bool&) {
            std::cout << "id: " << task.first << ", test input:" << i << std::endl;
            setOutputSVGName(std::format("EdgeTestWithArc_{}_{}{}{}", task.first, "Test", i, "Input"));
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
     - Shape {
         - Pixel { orange, x=6, y=5 }
         - Pixel { orange, x=5, y=6 }
         - Pixel { orange, x=7, y=6 }
         - Pixel { orange, x=6, y=7 }
       }

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


int main(int argc, char** argv)
{
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_st>();
    console_sink->set_level(spdlog::level::trace);
    console_sink->set_pattern("[%n] %v");

    spdlog::logger logger("edge", console_sink);
    logger.set_level(spdlog::level::trace);
    s_logger = &logger;

    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    std::cout << "Constructed: " << CellI::s_constructed << ", destructed: " << CellI::s_destructed << ", live: " << CellI::s_constructed - CellI::s_destructed << std::endl;

    return ret;
}