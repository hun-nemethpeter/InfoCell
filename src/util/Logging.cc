#include <fstream>
#include "Logging.h"

namespace synth {

LoggerStream Logger::log(LogLevel level)
{
    return LoggerStream(*this, level);
}

void Logger::log(LogLevel level, const std::string& message)
{
    std::ofstream logfile("F:\\Devel\\ARC\\synth\\build\\log\\logfile.txt", std::ios_base::app);
    logfile << message << "\n";
    m_messages.push_back(message);
}

void Logger::clearLogFile()
{
    std::ofstream logfile("F:\\Devel\\ARC\\synth\\build\\log\\logfile.txt");
}

LoggerStream::~LoggerStream()
{
    const std::string message = stream.str();
    if (!message.empty())
        logger.log(level, message);
}

} // namespace synth