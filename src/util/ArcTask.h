#pragma once

#include <nlohmann/json.hpp>
#include "app/Picture.h"
#include "cells/Cells.h"

namespace synth {
namespace cells {
namespace brain {
struct Brain;
} // namespace brain
} // namespace cells

class ArcDemonstration
{
public:
    ArcDemonstration(cells::brain::Brain& kb, int number, const std::string& input, const std::string& output);

    const int m_number;
    input::Picture m_inputPicture;
    input::Picture m_outputPicture;
    cells::hybrid::Picture m_input;
    cells::hybrid::Picture m_output;
};

class ArcTask
{
public:
    ArcTask(cells::brain::Brain& kb, const nlohmann::json& jsonArcFile);

    std::vector<ArcDemonstration> m_demonstrations;
    input::Picture m_inputPicture;
    input::Picture m_outputPicture;
    cells::hybrid::Picture m_testInput;
    cells::hybrid::Picture m_testSolution;
};

} // namespace synth