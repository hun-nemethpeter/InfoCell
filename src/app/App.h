#include "ArcDb.h"
#include "ftxui/component/component.hpp" // for Slider

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
enum LogLevel
{
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR
};

class Logger;
class LoggerStream
{
public:
    ~LoggerStream();

    template <typename T>
    LoggerStream& operator<<(const T& value)
    {
        stream << value;
        return (*this);
    }

private:
    friend class Logger;

    LoggerStream(Logger& logger, LogLevel level) :
        logger(logger), level(level)
    {
    }

    Logger& logger;
    LogLevel level;
    std::ostringstream stream;
};

class Logger
{
public:
    Logger(std::vector<std::string>& messages) :
        m_messages(messages) { }
    LoggerStream log(LogLevel level);
    void log(LogLevel level, const std::string& message) { m_messages.push_back(message); }

    std::vector<std::string>& m_messages;
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
    void loadArcFileByFileIndex();
    void renderArcTaskDemonstration();
    void renderArcTestInputGrid();
    void solve();

    std::vector<std::string> arcFileNames;
    std::vector<std::string> solveMessages;
    Logger solverLogger;

    ArcDb arcDb;
    std::string arcDbPath;
    ftxui::Element arcTaskDemonstration;
    ftxui::Element arcTestInputGrid;
    int selectedArcFileIndex        = 0;
    int previusSelectedArcFileIndex = 0;
};

} // namespace synth