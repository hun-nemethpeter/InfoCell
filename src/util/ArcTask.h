#pragma once

#include <nlohmann/json.hpp>
#include "app/Screen.h"
#include "cells/Cells.h"

namespace synth {

class ArcDemonstration
{
public:
    ArcDemonstration(const std::string& input, const std::string& output);

    input::Screen m_inputScreen;
    input::Screen m_outputScreen;
    cells::Sensor m_input;
    cells::Sensor m_output;
};

class ArcTask
{
public:
    ArcTask(const nlohmann::json& jsonArcFile);

    std::vector<ArcDemonstration> m_demonstrations;
    input::Screen m_inputScreen;
    input::Screen m_outputScreen;
    cells::Sensor m_testInput;
    cells::Sensor m_testSolution;
};

} // namespace synth