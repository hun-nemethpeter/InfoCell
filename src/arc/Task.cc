#include <array>
#include <fmt/core.h>
#include <fstream>

#include "cells/World.h"

#include "Grid.h"
#include "Task.h"

using namespace nlohmann;

namespace infocell {
namespace arc {

Task::GridPair::GridPair(cells::World& w, int number, const std::string& input, const std::string& output) :
    m_number(number),
    m_inputGrid(fmt::format("Train input {}", number)),
    m_outputGrid(std::make_unique<native::Grid>(fmt::format("Train output {}", number))),
    m_input(w, m_inputGrid.loadFromJsonArray(input)),
    m_output(std::make_unique<cells::arc::Grid>(w, m_outputGrid->loadFromJsonArray(output)))
{
}

Task::GridPair::GridPair(cells::World& w, int number, const std::string& input) :
    m_number(number),
    m_inputGrid(fmt::format("Train input {}", number)),
    m_input(w, m_inputGrid.loadFromJsonArray(input))
{
}

Task::Task(cells::World& w, const nlohmann::json& arcJsonTask) :
    Task(w, "", arcJsonTask)
{

}

Task::Task(cells::World& w, const std::string& id, const nlohmann::json& jsonTask) :
    m_id(id),
    m_cellTaskStruct(w.getStruct("arc::Task")),
    m_cellExampleStruct(w.getStruct("arc::Example")),
    m_cellTask(w, m_cellTaskStruct),
    m_cellExamplesList(w, m_cellExampleStruct),
    m_cellTestsList(w, m_cellExampleStruct)
{
    const nlohmann::json& jsonTrainSet = jsonTask.at("train");
    m_examples.reserve(jsonTrainSet.size());
    m_cellExamples.reserve(jsonTrainSet.size());
    int trainExampleNumber = 1;
    for (const auto& trainExample : jsonTrainSet) {
        m_examples.emplace_back(w, trainExampleNumber++, to_string(trainExample.at("input")), to_string(trainExample.at("output")));
        GridPair& gridPair = m_examples.back();
        m_cellExamples.emplace_back(w, m_cellExampleStruct);
        cells::Object& exampleObject = m_cellExamples.back();
        exampleObject.set("input", gridPair.m_input);
        exampleObject.set("output", *gridPair.m_output);
        m_cellExamplesList.add(exampleObject);
    }
    const nlohmann::json& jsonTestSet = jsonTask.at("test");
    m_tests.reserve(jsonTestSet.size());
    m_cellTests.reserve(jsonTrainSet.size());
    int testExampleNumber = 1;
    for (const auto& testExample : jsonTestSet) {
        m_tests.emplace_back(w, testExampleNumber++, to_string(testExample.at("input")));
        GridPair& gridPair = m_tests.back();
        m_cellTests.emplace_back(w, m_cellExampleStruct);
        cells::Object& exampleObject = m_cellTests.back();
        exampleObject.set("input", gridPair.m_input);
        m_cellTestsList.add(exampleObject);
    }

    m_cellTask.set("examples", m_cellExamplesList);
    m_cellTask.set("tests", m_cellTestsList);
}

TaskSet::TaskSet(cells::World& w, const std::string& filePath) :
    w(w)
{
    auto allTasks = json::parse(std::ifstream(filePath));
    for (json::const_iterator it = allTasks.begin(); it != allTasks.end(); ++it) {
        m_tasks.emplace(std::piecewise_construct,
                        std::forward_as_tuple(it.key()),
                        std::forward_as_tuple(w, it.key(), it.value()));
    }
}

void TaskSet::addSolutions(const std::string& filePath)
{
    auto solutions = json::parse(std::ifstream(filePath));
    for (json::const_iterator it = solutions.begin(); it != solutions.end(); ++it) {
        const std::string& taskId = it.key();

        auto findIter = m_tasks.find(taskId);
        if (findIter == m_tasks.end()) {
            throw "No task found for this soltion, maybe wrong file?";
        }

        Task& task = findIter->second;
        const nlohmann::json& jsonSolutionArray = it.value();

        int index = 0;
        for (const auto& testExample : jsonSolutionArray) {
            Task::GridPair& gridPair = task.m_tests[index];
            gridPair.m_outputGrid    = std::make_unique<native::Grid>(fmt::format("Test output {}", index));
            gridPair.m_output        = std::make_unique<cells::arc::Grid>(w, gridPair.m_outputGrid->loadFromJsonArray(to_string(jsonSolutionArray[index])));
            ++index;
        }
    }
}

} // namespace arc
} // namespace infocell