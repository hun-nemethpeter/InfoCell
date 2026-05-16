#include <fstream>
#include "Logging.h"

namespace infocell {
Logger* loggerPtr = nullptr;

LoggerStream Logger::log(LogLevel level)
{
    return LoggerStream(*this, level, STRING);
}

LoggerStream Logger::logBoard(LogLevel level)
{
    return LoggerStream(*this, level, BOARD);
}

const std::string logFileName = "F:\\Devel\\ARC\\infocell\\build\\log\\logfile.txt";

void Logger::log(LogLevel level, LogType type, const std::string& message)
{
    std::ofstream logfile(logFileName, std::ios_base::app);
    logfile << message << "\n";
    m_messages.push_back({ level, type, message });
}

void Logger::clearLogFile()
{
    std::ofstream logfile(logFileName);
}

LoggerStream::~LoggerStream()
{
    const std::string message = stream.str();
    if (!message.empty())
        logger.log(level, type, message);
}

} // namespace infocell