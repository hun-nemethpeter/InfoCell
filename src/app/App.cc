#include "App.h"

#include <array>
#include <filesystem>
#include <iostream>
#include <fstream>

#include "cells/Cells.h"
#include "Solver.h"
#include "util/Converter.h"

#include "ftxui/component/captured_mouse.hpp"     // for ftxui
#include "ftxui/component/component.hpp"          // for Slider
#include "ftxui/component/screen_interactive.hpp" // for ScreenInteractive
#include <ftxui/dom/table.hpp>

namespace fs = std::filesystem;
using namespace nlohmann;
using namespace ftxui;

namespace synth {

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

void App::loadArcFile(const std::string& filename)
{
    std::ifstream ifs(filename);
    arcDb = json::parse(ifs);
#if 0
    //    std::cout << jf.dump(4) << std::endl;
    std::cout << "Input:" << std::endl;
    for (const auto& input : jf.at("train")) {
        for (const auto& col : input.at("input")) {
            std::cout << col << std::endl;
        }
        std::cout << "---" << std::endl;
    }
#endif
}

void App::loadArcFileByFileIndex()
{
    loadArcFile(getArcFilePathFromIndex(selectedArcFileIndex));
    renderArcTaskDemonstration();
    renderArcTestInputGrid();
}

void App::renderArcTaskDemonstration()
{
    auto& jTrain = arcDb.at("train");
    int i        = 0;
    std::vector<Elements> arcTaskDemonstrationTableData;
    arcTaskDemonstrationTableData.push_back({
        text("Input") | center,
        text("Output") | center,
    });
    for (const auto& train : arcDb.at("train")) {
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
    auto& inputRow = arcDb["/test/0/input"_json_pointer];

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
    auto buttonSolve = Button("Solve", [&] {
        solveMessages.clear();
        solve();
        depth = 1;
    });
    auto buttonQuit  = Button("Quit", [&] { exit(0); });

    auto container = Container::Vertical({
        buttonSolve,
        buttonQuit,
        menu
    });

    FlexboxConfig flexConfig;
    flexConfig.direction       = FlexboxConfig::Direction::Row;
    flexConfig.wrap            = FlexboxConfig::Wrap::NoWrap;
    flexConfig.justify_content = FlexboxConfig::JustifyContent::FlexStart;
    flexConfig.align_items     = FlexboxConfig::AlignItems::FlexStart;
    flexConfig.align_content   = FlexboxConfig::AlignContent::FlexStart;

    FlexboxConfig flexButtonsConfig;
    flexButtonsConfig.direction = FlexboxConfig::Direction::Row;
    flexButtonsConfig.wrap      = FlexboxConfig::Wrap::NoWrap;
    flexButtonsConfig.justify_content = FlexboxConfig::JustifyContent::SpaceAround;
    flexButtonsConfig.align_items     = FlexboxConfig::AlignItems::Center;
    flexButtonsConfig.align_content   = FlexboxConfig::AlignContent::SpaceEvenly;


    auto mainScreenRenderer = Renderer(container, [&] {
        if (previusSelectedArcFileIndex != selectedArcFileIndex) {
            loadArcFileByFileIndex();
            previusSelectedArcFileIndex = selectedArcFileIndex;
        }

        auto ret = flexbox({ vbox({ hbox(text("selected = "), text(arcFileNames[selectedArcFileIndex])),
                                    separator(),
                                    flexbox({ buttonSolve->Render() | xflex_grow, separator() | yflex_grow, buttonQuit->Render() | xflex_grow }, flexButtonsConfig),
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

void App::solve()
{
    JsonMatrixToCellConverter input(arcDb, "/train/0/input");
    JsonMatrixToCellConverter output(arcDb, "/train/0/output");
    Solver solver(solverLogger, input.sensor(), output.sensor());
}

} // namespace synth