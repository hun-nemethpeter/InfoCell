#pragma once

#include "arc/hybridcells/Grid.h"
#include <nlohmann/json.hpp>

namespace infocell {
namespace arc {

class Task
{
public:
    class IOPair
    {
    public:
        IOPair(cells::brain::Brain& kb, int number, const std::string& input);
        IOPair(cells::brain::Brain& kb, int number, const std::string& input, const std::string& output);

        const int m_number;
        native::Grid m_inputGrid;
        std::unique_ptr<native::Grid> m_outputGrid;
        cells::arc::Grid m_input;
        std::unique_ptr<cells::arc::Grid> m_output;
    };

    Task(cells::brain::Brain& kb, const nlohmann::json& arcJsonTask);
    Task(cells::brain::Brain& kb, const std::string& id, const nlohmann::json& jsonTask);

    std::string m_id;
    std::vector<IOPair> m_examples;
    std::vector<IOPair> m_tests;

    cells::CellI& m_cellTaskStruct;
    cells::CellI& m_cellExampleStruct;
    cells::Object m_cellTask;

    std::vector<cells::Object> m_cellExamples;
    std::vector<cells::Object> m_cellTests;

    cells::List m_cellExamplesList;
    cells::List m_cellTestsList;
};

class TaskSet
{
public:
    TaskSet(cells::brain::Brain& kb, const std::string& filePath);

    std::map<std::string, Task> m_tasks;
};

} // namespace arc
} // namespace infocell
