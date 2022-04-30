#include <array>
#include <filesystem>
namespace fs = std::filesystem;

#include "App.h"
#include "cells/Cells.h"

#include "ftxui/component/captured_mouse.hpp"     // for ftxui
#include "ftxui/component/component.hpp"          // for Slider
#include "ftxui/component/screen_interactive.hpp" // for ScreenInteractive
#include <ftxui/dom/table.hpp>

using namespace ftxui;

namespace synth {

LoggerStream Logger::log(LogLevel level)
{
    return LoggerStream(*this, level);
}

LoggerStream::~LoggerStream()
{
    const std::string message = stream.str();
    if (!message.empty())
        logger.log(level, message);
}

const std::array<Color, 10> App::arcColors = {
    Color(0x00, 0x00, 0x00), /* black */
    Color(0x00, 0x74, 0xD9), /* blue */
    Color(0xFF, 0x41, 0x36), /* red */
    Color(0x2E, 0xCC, 0x40), /* green */
    Color(0xFF, 0xDC, 0x00), /* yellow */
    Color(0xAA, 0xAA, 0xAA), /* grey */
    Color(0xF0, 0x12, 0xBE), /* fuschia */
    Color(0xFF, 0x85, 0x1B), /* orange */
    Color(0x7F, 0xDB, 0xFF), /* teal */
    Color(0x87, 0x0C, 0x25)  /* brown */
};

void App::init(int argc, char* argv[])
{
    if (argc == 1) {
        arcDbPath = "F:\\Devel\\ARC\\ARC\\data\\training\\";
//        arcDbPath = "F:\\Devel\\ARC\\ARC\\data\\evaluation\\";
    } else {
        arcDbPath = argv[1];
    }

    arcFileNames.clear();
    for (const auto& entry : fs::directory_iterator(arcDbPath)) {
        arcFileNames.push_back(entry.path().filename().string());
    }

    loadArcFileByFileIndex();
}

Element colorTile(Color p_color)
{
    return text("") | size(WIDTH, EQUAL, 2) | size(HEIGHT, EQUAL, 1) | bgcolor(p_color);
}

Element colorTile(ArcColors arcColor)
{
    return colorTile(App::arcColors[(int)arcColor]);
}

std::string App::getArcFilePathFromIndex(int index)
{
    return arcDbPath + arcFileNames[selectedArcFileIndex];
}

void App::loadArcFileByFileIndex()
{
    arcDb.load(getArcFilePathFromIndex(selectedArcFileIndex));
    renderArcTaskDemonstration();
    renderArcTestInputGrid();
}

void App::renderArcTaskDemonstration()
{
    auto& jTrain = arcDb.jf.at("train");
    int i        = 0;
    std::vector<Elements> arcTaskDemonstrationTableData;
    arcTaskDemonstrationTableData.push_back({
        text("Input") | center,
        text("Output") | center,
    });
    for (const auto& train : arcDb.jf.at("train")) {
        auto& inputRow  = train.at("input");
        auto& outputRow = train.at("output");
        Elements arcSet;

        Elements arcSetInputLines;
        for (auto inputRowIt = inputRow.begin(); inputRowIt != inputRow.end(); ++inputRowIt) {
            Elements arcSetInputLine;
            for (const int val : *inputRowIt) {
                arcSetInputLine.push_back(colorTile((ArcColors)val));
            }
            arcSetInputLines.push_back(hbox(arcSetInputLine));
        }
        Element arcSetInputGrid = vbox(arcSetInputLines);

        Elements arcSetOutputLines;
        for (auto outputRowIt = outputRow.begin(); outputRowIt != outputRow.end(); ++outputRowIt) {
            Elements arcSetOutputLine;
            for (const int val : *outputRowIt) {
                arcSetOutputLine.push_back(colorTile((ArcColors)val));
            }
            arcSetOutputLines.push_back(hbox(arcSetOutputLine));
        }
        Element arcSetOutputGrid = vbox(arcSetOutputLines);

        arcTaskDemonstrationTableData.push_back({ arcSetInputGrid, arcSetOutputGrid });
    }

    Table arcTaskDemonstrationTable(arcTaskDemonstrationTableData);
    for (int rowIndex = 0; rowIndex + 1 < arcTaskDemonstrationTableData.size(); ++rowIndex) {
        arcTaskDemonstrationTable.SelectRow(rowIndex).BorderBottom(LIGHT);
    }
    arcTaskDemonstrationTable.SelectColumn(0).BorderRight(LIGHT);

    arcTaskDemonstration = arcTaskDemonstrationTable.Render();
}

void App::renderArcTestInputGrid()
{
    auto& inputRow = arcDb.jf["/test/0/input"_json_pointer];

    Elements arcSetInputLines;
    for (auto inputRowIt = inputRow.begin(); inputRowIt != inputRow.end(); ++inputRowIt) {
        Elements arcSetInputLine;
        for (const int val : *inputRowIt) {
            arcSetInputLine.push_back(colorTile((ArcColors)val));
        }
        arcSetInputLines.push_back(hbox(arcSetInputLine));
    }
    arcTestInputGrid = vbox({ text("Test") | center, separator(), vbox(arcSetInputLines) });
}

void App::run()
{
    int depth        = 0;
    auto menu        = Menu(&arcFileNames, &selectedArcFileIndex);
    auto buttonSolve = Button("Solve", [&] { solve(); depth = 1; });
    auto buttonQuit  = Button("Quit", [&] { exit(0); });

    auto container = Container::Vertical({
        buttonSolve,
        buttonQuit,
        menu
    });

    FlexboxConfig flexConfig;
    flexConfig.direction       = static_cast<FlexboxConfig::Direction>(0);
    flexConfig.wrap            = static_cast<FlexboxConfig::Wrap>(0);
    flexConfig.justify_content = static_cast<FlexboxConfig::JustifyContent>(0);
    flexConfig.align_items     = static_cast<FlexboxConfig::AlignItems>(0);
    flexConfig.align_content   = static_cast<FlexboxConfig::AlignContent>(0);

    auto mainScreenRenderer = Renderer(container, [&] {
        if (previusSelectedArcFileIndex != selectedArcFileIndex) {
            loadArcFileByFileIndex();
            previusSelectedArcFileIndex = selectedArcFileIndex;
        }

        auto ret = flexbox({ vbox({ hbox(text("selected = "), text(arcFileNames[selectedArcFileIndex])),
                                    separator(),
                                    hbox(buttonSolve->Render(), separator(), buttonQuit->Render()),
                                    separator(),
                                    menu->Render() | vscroll_indicator | frame | size(HEIGHT, LESS_THAN, 25) })
                                 | border,
                             arcTaskDemonstration | border,
                             arcTestInputGrid | border }, flexConfig);
        return ret;
    });
//
    auto solveQuitBtn         = Button("Ok", [&] { depth = 0; });
    auto solverScreenRenderer = Renderer(solveQuitBtn, [&] {
        Elements logItems;
        for (const auto& logMessage : solveMessages) {
            logItems.push_back(text(logMessage));
        }
        return vbox({
                   text("The solver is solving ..."),
                   separator(),
                   vbox(logItems),
                   separator(),
                   solveQuitBtn->Render()
               })
            | border;
    });

    auto mainContainer = Container::Tab(
        {
            mainScreenRenderer,
            solverScreenRenderer,
        },
        &depth);

    auto renderer = Renderer(mainContainer, [&] {
        Element document = mainScreenRenderer->Render();

        if (depth == 1) {
            document = dbox({
                document,
                solverScreenRenderer->Render() | clear_under | center,
            });
        }
        return document;
    });
    //
    auto screen = ScreenInteractive::FitComponent();
    screen.Loop(renderer);
}

const std::array<cells::Color, 10> cellColors = {
    cells::Color(0x00, 0x00, 0x00), /* black */
    cells::Color(0x00, 0x74, 0xD9), /* blue */
    cells::Color(0xFF, 0x41, 0x36), /* red */
    cells::Color(0x2E, 0xCC, 0x40), /* green */
    cells::Color(0xFF, 0xDC, 0x00), /* yellow */
    cells::Color(0xAA, 0xAA, 0xAA), /* grey */
    cells::Color(0xF0, 0x12, 0xBE), /* fuschia */
    cells::Color(0xFF, 0x85, 0x1B), /* orange */
    cells::Color(0x7F, 0xDB, 0xFF), /* teal */
    cells::Color(0x87, 0x0C, 0x25)  /* brown */
};

class JsonMatrixToCellConverter
{
public:
    JsonMatrixToCellConverter(const nlohmann::json& jsonDb, const std::string& path) :
        m_jsonDb(jsonDb), m_path(path), m_jMatrix(jsonDb[nlohmann::json::json_pointer(m_path)])
    {
        convert();
    }

    void convert()
    {
        size_t matrixHeight = m_jMatrix.size();
        size_t matrixWidth  = m_jMatrix[0].size();

        m_sensor.height(matrixHeight).width(matrixWidth);
        m_sensor.init();

        int y = 0;
        for (auto inputRowIt = m_jMatrix.begin(); inputRowIt != m_jMatrix.end(); ++inputRowIt) {
            int x = 0;
            for (const int val : *inputRowIt) {
                cells::Pixel& pixel = m_sensor.getPixel(x, y);
                pixel.color         = cellColors[val];
            }
        }
    }

    const cells::Sensor& sensor() const
    {
        return m_sensor;
    }

private:
    cells::Sensor m_sensor;
    const nlohmann::json& m_jsonDb;
    const std::string& m_path;
    const nlohmann::json& m_jMatrix;
};

class Solver
{
public:
    Solver(Logger& logger, const cells::Sensor& input, const cells::Sensor& output) :
        logger(logger), m_input(input),
    m_output(output)
    {
        solve();
    }

    void solve()
    {
        logger.log(INFO) << "Mapping input[" << m_input.m_width << ", " << m_input.m_height << "] to output[" << m_output.m_width << ", " << m_output.m_height << "]";
        logger.log(INFO) << "Mapping input[" << m_input.m_width << ", " << m_input.m_height << "] to output[" << m_output.m_width << ", " << m_output.m_height << "]";
        logger.log(INFO) << "Mapping input[" << m_input.m_width << ", " << m_input.m_height << "] to output[" << m_output.m_width << ", " << m_output.m_height << "]";
        logger.log(INFO) << "Mapping input[" << m_input.m_width << ", " << m_input.m_height << "] to output[" << m_output.m_width << ", " << m_output.m_height << "]";
    }

    Logger& logger;
    const cells::Sensor& m_input;
    const cells::Sensor& m_output;
};

void App::solve()
{
    solveMessages.clear();
    JsonMatrixToCellConverter input(arcDb.jf, "/train/0/input");
    JsonMatrixToCellConverter output(arcDb.jf, "/train/0/output");
    Solver solver(solverLogger, input.sensor(), output.sensor());
}

/*

A program célja, hogy két pixelmátrixból megtalálja a transzformációs algoritmust.
A feladatok során mindig található egy transzformációs algoritmus, ami egyik mátrixból előállítja a másikat.

Első lépés, hogy a pixel mátrixban azonosítsuk az azonos színű alakzatokat. Az alakzat akár 1 pixeles is lehet.
Az egyik kitüntetett alakzat a "háttér" alakzat, ez jellemzően fekete. Sok példában, de nem mindben az alakzatok egy háttér előtt vannak.

Egy 2D alakzatot lehet forgatni, kicsinyíteni, nagyítani, mozgatni, átszínezni valamilyen szabály szerint.

Pixels => Shapes

*/

} // namespace synth