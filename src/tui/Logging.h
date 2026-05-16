#pragma once

#include <vector>
#include <string>
#include <sstream>

#include "LogMessage.h"

namespace infocell {

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

    LoggerStream(Logger& logger, LogLevel level, LogType type) :
        logger(logger), level(level), type(type)
    {
    }

    Logger& logger;
    LogLevel level;
    LogType type;
    std::ostringstream stream;
};

class Logger
{
public:
    Logger(std::vector<LogMessage>& messages) :
        m_messages(messages) { }
    LoggerStream log(LogLevel level);
    LoggerStream logBoard(LogLevel level);
    void log(LogLevel level, LogType type, const std::string& message);
    void clearLogFile();

    std::vector<LogMessage>& m_messages;
};

extern Logger* loggerPtr;

} // namespace infocell