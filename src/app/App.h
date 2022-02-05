#include "ArcDb.h"
#include "ftxui/component/component.hpp" // for Slider

#include <vector>
#include <string>

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
    void init(int argc, char* argv[]);
    void run();
    static const std::array<ftxui::Color, 10> arcColors;

private:
    std::string getArcFilePathFromIndex(int index);
    void loadArcFileByFileIndex();
    void renderArcTaskDemonstration();
    void renderArcTestInputGrid();

    std::vector<std::string> arcFileNames;
    ArcDb arcDb;
    std::string arcDbPath;
    ftxui::Element arcTaskDemonstration;
    ftxui::Element arcTestInputGrid;
    int selectedArcFileIndex        = 0;
    int previusSelectedArcFileIndex = 0;
};

} // namespace synth