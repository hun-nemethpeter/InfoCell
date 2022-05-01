#pragma once

#include <vector>
#include <string>
#include <sstream>

namespace synth {

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

} // namespace synth