#pragma once

#include "util/Logging.h"
#include "ftxui/component/component.hpp" // for Slider

#include <nlohmann/json.hpp>
#include <vector>
#include <string>
#include <sstream>

namespace synth {

enum class ArcColors
{
    black,
    blue,
    red,
    green,
    yellow,
    grey,
    fuschia,
    orange,
    teal,
    brown
};

class App
{
public:
    App() :
        solverLogger(solveMessages) { }
    void init(int argc, char* argv[]);
    void run();
    static const std::array<ftxui::Color, 10> arcColors;

private:
    std::string getArcFilePathFromIndex(int index);
    void loadArcFile(const std::string& filename);
    void loadArcFileByFileIndex();
    void renderArcTaskDemonstration();
    void renderArcTestInputGrid();
    void solve();

    std::vector<std::string> arcFileNames;
    std::vector<std::string> solveMessages;
    Logger solverLogger;

    nlohmann::json arcDb;
    std::string arcDbPath;
    ftxui::Element arcTaskDemonstration;
    ftxui::Element arcTestInputGrid;
    int selectedArcFileIndex        = 0;
    int previusSelectedArcFileIndex = 0;
};

} // namespace synth