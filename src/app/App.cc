#include "App.h"

#include <array>
#include <filesystem>
#include <iostream>
#include <fstream>

#include <ftxui/component/captured_mouse.hpp>     // for ftxui
#include <ftxui/component/component.hpp>          // for Slider
#include <ftxui/component/screen_interactive.hpp> // for ScreenInteractive
#include <ftxui/dom/table.hpp>

#include "Scroller.h"
#include "Solver.h"
#include "tests/UnitTester.h"

namespace fs = std::filesystem;
using namespace nlohmann;
using namespace ftxui;

namespace synth {

const std::array<ftxui::Color, 11> App::arcColors = {
    Color(0x00, 0x00, 0x00), /* black */
    Color(0x00, 0x74, 0xD9), /* blue */
    Color(0xFF, 0x41, 0x36), /* red */
    Color(0x2E, 0xCC, 0x40), /* green */
    Color(0xFF, 0xDC, 0x00), /* yellow */
    Color(0xAA, 0xAA, 0xAA), /* grey */
    Color(0xF0, 0x12, 0xBE), /* fuschia */
    Color(0xFF, 0x85, 0x1B), /* orange */
    Color(0x7F, 0xDB, 0xFF), /* teal */
    Color(0x87, 0x0C, 0x25), /* brown */
    Color(0xFF, 0xFF, 0xFF)  /* alpha - white */
};

void App::init(int argc, char* argv[])
{
    if (argc == 1) {
        m_arcFilePath = "F:\\Devel\\ARC\\ARC\\data\\training\\";
        // m_arcFilePath = "F:\\Devel\\ARC\\ARC\\data\\evaluation\\";
        // m_arcFilePath = "F:\\Devel\\ARC\\pqa-dataset\\closure-filling\\";
        // m_arcFilePath = "F:\\Devel\\ARC\\pqa-dataset\\proximity-identification\\";
    } else {
        m_arcFilePath = argv[1];
    }

    m_arcFileNames.clear();
    for (const auto& entry : fs::directory_iterator(m_arcFilePath)) {
        m_arcFileNames.push_back(entry.path().filename().string());
    }

    loadArcFileByFileIndex();
}

Element colorTile(Color p_color)
{
    return text("") | size(WIDTH, EQUAL, 2) | size(HEIGHT, EQUAL, 1) | bgcolor(p_color);
}

Element colorTile(arc::Colors arcColor)
{
    if (arcColor == arc::Colors::alpha)
        return text(L"╳╳") | size(WIDTH, EQUAL, 2) | size(HEIGHT, EQUAL, 1) | color(Color::GrayDark) | bgcolor(Color::GrayLight); // App::arcColors[(int)arcColor]);
    return colorTile(App::arcColors[(int)arcColor]);
}

std::string App::getArcFilePathFromIndex(int index)
{
    return m_arcFilePath + m_arcFileNames[m_selectedArcFileIndex];
}

void App::loadArcFile(const std::string& filename)
{
    std::ifstream ifs(filename);
    m_arcJsonTask = json::parse(ifs);
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
    loadArcFile(getArcFilePathFromIndex(m_selectedArcFileIndex));
    renderArcTaskDemonstration();
    renderArcTestInputGrid();
}

static Element renderJsonBoard(const nlohmann::json& inputRow)
{
    Elements boardLines;
    for (auto inputRowIt = inputRow.begin(); inputRowIt != inputRow.end(); ++inputRowIt) {
        Elements arcSetInputLine;
        for (const int val : *inputRowIt) {
            arcSetInputLine.push_back(colorTile((arc::Colors)val));
        }
        boardLines.push_back(hbox(arcSetInputLine));
    }

    return vbox(boardLines);
}

static std::vector<std::string> split(const std::string& txt, char ch = ' ')
{
    std::vector<std::string> strs;
    size_t pos        = txt.find(ch);
    size_t initialPos = 0;

    // Decompose statement
    while (pos != std::string::npos) {
        strs.push_back(txt.substr(initialPos, pos - initialPos));
        initialPos = pos + 1;

        pos = txt.find(ch, initialPos);
    }

    // Add the last one
    strs.push_back(txt.substr(initialPos, std::min(pos, txt.size()) - initialPos + 1));

    return strs;
}

static Element renderBoardFromLog(const std::string& boardStr)
{
    auto boardParts          = split(boardStr);
    const int width          = std::stoi(boardParts[0]);
    const int height         = std::stoi(boardParts[1]);
    const std::string& board = boardParts[2];

    Elements boardLines;
    for (int y = 0; y < height; ++y) {
        Elements arcSetInputLine;
        for (int x = 0; x < width; ++x) {
            char boardChar             = board[y * width + x];
            const arc::Colors colorIndex = boardChar == '.' ? arc::Colors::alpha : (arc::Colors)(boardChar - '0');
            arcSetInputLine.push_back(colorTile(colorIndex));
        }
        boardLines.push_back(hbox(arcSetInputLine));
    }

    return vbox(boardLines);
}

void App::renderArcTaskDemonstration()
{
    auto& jTrain = m_arcJsonTask.at("train");
    std::vector<Elements> arcTaskDemonstrationTableData;
    arcTaskDemonstrationTableData.push_back({
        text("Input") | center,
        text("Output") | center,
    });
    for (const auto& train : m_arcJsonTask.at("train")) {
        Element arcSetInputGrid  = renderJsonBoard(train.at("input"));
        Element arcSetOutputGrid = renderJsonBoard(train.at("output"));

        arcTaskDemonstrationTableData.push_back({ arcSetInputGrid, arcSetOutputGrid });
    }

    Table arcTaskDemonstrationTable(arcTaskDemonstrationTableData);
    for (int rowIndex = 0; rowIndex + 1 < arcTaskDemonstrationTableData.size(); ++rowIndex) {
        arcTaskDemonstrationTable.SelectRow(rowIndex).BorderBottom(LIGHT);
    }
    arcTaskDemonstrationTable.SelectColumn(0).BorderRight(LIGHT);

    m_arcTaskDemonstration = arcTaskDemonstrationTable.Render();
}

void App::renderArcTestInputGrid()
{
    auto& inputRow = m_arcJsonTask["/test/0/input"_json_pointer];

    Elements arcSetInputLines;
    for (auto inputRowIt = inputRow.begin(); inputRowIt != inputRow.end(); ++inputRowIt) {
        Elements arcSetInputLine;
        for (const int val : *inputRowIt) {
            arcSetInputLine.push_back(colorTile((arc::Colors)val));
        }
        arcSetInputLines.push_back(hbox(arcSetInputLine));
    }
    m_arcTestInputGrid = vbox(text("Test") | center, separator(), vbox(arcSetInputLines));
}

void App::run()
{
    int depth        = 0;
    auto menu        = Menu(&m_arcFileNames, &m_selectedArcFileIndex);

    float focus_x = 0.0f;
    float focus_y = 0.0f;
    auto slider_x = Slider("", &focus_x, 0.f, 1.f, 0.002f);
    auto slider_y = Slider("", &focus_y, 0.f, 1.f, 0.002f);

    auto buttonSolve = Button("Solve", [&] {
        focus_y = 0.0f;
        m_solvingLogs.clear();
        solve();
        depth = 1;
    });
    auto buttonTest = Button("Tests", [&] {
        focus_y = 0.0f;
        m_solvingLogs.clear();
        doUnitTests();
        depth = 1;
    });
    auto buttonQuit  = Button("Quit", [&] { exit(0); });

    auto container = Container::Vertical({
        buttonSolve,
        buttonTest,
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
        if (m_previusSelectedArcFileIndex != m_selectedArcFileIndex) {
            loadArcFileByFileIndex();
            m_previusSelectedArcFileIndex = m_selectedArcFileIndex;
        }

        auto ret = flexbox({ vbox(hbox(text("selected = "), text(m_arcFileNames[m_selectedArcFileIndex])),
                                    separator(),
                                    menu->Render() | vscroll_indicator | frame | size(HEIGHT, LESS_THAN, 25),
                                    separator(),
                                    flexbox({ buttonSolve->Render() | xflex_grow, separator() | yflex_grow, buttonTest->Render() | xflex_grow, separator() | yflex_grow, buttonQuit->Render() | xflex_grow }, flexButtonsConfig)
                                  ) | border,
                             m_arcTaskDemonstration | border,
                             m_arcTestInputGrid | border }, flexConfig);
        return ret;
    });

    int logIndex = 0;
    auto solveQuitBtn     = Button("Ok", [&] { depth = 0; });

    auto solveLogMessages = Renderer([&] {
        Elements logItems;
        for (const auto& logMessage : m_solvingLogs) {
            switch (logMessage.type) {
            case STRING:
                logItems.push_back(paragraph(logMessage.text));
                break;
            case BOARD:
                logItems.push_back(renderBoardFromLog(logMessage.text));
                break;
            }
        }

        return vbox(logItems) | focusPositionRelative(focus_x, focus_y) | vscroll_indicator | yframe | flex;
    });

    auto scroller = Scroller(solveLogMessages);

    auto solveContainer = Container::Vertical({ slider_y,
//                                                solveLogMessages,
                                                scroller,
                                                solveQuitBtn });

    auto solverScreenRenderer = Renderer(solveContainer, [&] {
        return vbox({
//                   slider_y->Render(),
//                   solveLogMessages->Render() | flex_grow | size(WIDTH, GREATER_THAN, 120),
                   scroller->Render() | flex_grow | size(WIDTH, GREATER_THAN, 120),
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
        Element document = mainScreenRenderer->Render() | size(WIDTH, GREATER_THAN, 200);

        if (depth == 1) {
            document = dbox({
                document,
                solverScreenRenderer->Render() | clear_under | align_right,
            });
        }
        return document;
    });

    auto screen = ScreenInteractive::FitComponent();
    screen.Loop(renderer);
}

void App::solve()
{
    cells::brain::Brain kb;
    ArcTask arcTask(kb, m_arcJsonTask);
    Solver solver(solverLogger, arcTask);
}

void App::doUnitTests()
{
    UnitTester unitTester(solverLogger);
}

} // namespace synth