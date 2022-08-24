#pragma once

#include "cells/Cells.h"
#include "util/ArcTask.h"
#include "util/Logging.h"

namespace synth {

class Solver
{
public:
    Solver(Logger& logger, const ArcTask& arcTask);

    void solve();
    void solveOne(const cells::Sensor& sensor);

    Logger& logger;
    const ArcTask& m_arcTask;
};

} // namespace synth