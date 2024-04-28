#pragma once

#include <nlohmann/json.hpp>
#include "app/Picture.h"
#include "cells/Brain.h"

namespace synth {
namespace cells {
namespace brain {
class Brain;
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
    std::vector<cells::Object> m_exampleObjects;
    input::Picture m_inputPicture;
    input::Picture m_outputPicture;
    cells::hybrid::Picture m_challenge;
    cells::hybrid::Picture m_solution;
    cells::CellI& m_taskStruct;
    cells::CellI& m_demonstrationStruct;
    cells::Object m_task;
    cells::List m_examples;
};

} // namespace synth