#include "Logging.h"

namespace synth {

LoggerStream Logger::log(LogLevel level)
{
    return LoggerStream(*this, level);
}

LoggerStream::~LoggerStream()
{
    const std::string message = stream.str();
    if (!message.empty())
        logger.log(level, message);
}

} // namespace synth