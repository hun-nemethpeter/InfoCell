#include <string>

namespace infocell {

enum class LogLevel
{
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR
};

enum LogType
{
    STRING,
    BOARD
};

class LogMessage
{
public:
    LogLevel level;
    LogType type;
    std::string text;
};

} // namespace infocell