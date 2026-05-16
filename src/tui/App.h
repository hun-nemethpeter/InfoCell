#pragma once

#include "Logging.h"
#include "arc/Color.h"
#include "ftxui/component/component.hpp"

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace infocell {
namespace tui {

class App
{
public:
    App() :
        solverLogger(m_solvingLogs) { }
    void init(int argc, char* argv[]);
    void run();
    static const std::array<ftxui::Color, 11> arcColors;

private:
    std::string getArcFilePathFromIndex(int index);
    void loadArcFile(const std::string& filename);
    void loadArcFileByFileIndex();
    void renderArcTaskDemonstration();
    void renderArcTestInputGrid();
    void solve();
    void doUnitTests();

    std::vector<std::string> m_arcFileNames;
    std::vector<LogMessage> m_solvingLogs;
    Logger solverLogger;

    nlohmann::json m_arcJsonTask;
    std::string m_arcFilePath;
    ftxui::Element m_arcTaskDemonstration;
    ftxui::Element m_arcTestInputGrid;
    int m_selectedArcFileIndex        = 0;
    int m_previusSelectedArcFileIndex = 0;
};

} // namespace tui
} // namespace infocell
