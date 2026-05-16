#pragma once

#include "Logging.h"

namespace infocell {
namespace tui {

class Tester
{
public:
    Tester(Logger& logger);

    void start();

    Logger& logger;
};

} // namespace tui
} // namespace infocell
