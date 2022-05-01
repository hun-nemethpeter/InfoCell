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
        solverLogger(m_solveMessages) { }
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

    std::vector<std::string> m_arcFileNames;
    std::vector<std::string> m_solveMessages;
    Logger solverLogger;

    nlohmann::json m_arcDb;
    std::string m_arcFilePath;
    ftxui::Element m_arcTaskDemonstration;
    ftxui::Element m_arcTestInputGrid;
    int m_selectedArcFileIndex        = 0;
    int m_previusSelectedArcFileIndex = 0;
};

} // namespace synth