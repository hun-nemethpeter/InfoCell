#include <array>
#include <fmt/core.h>
#include <fstream>

#include "cells/Brain.h"

#include "Grid.h"
#include "Task.h"

using namespace nlohmann;

namespace infocell {
namespace arc {

Task::IOPair::IOPair(cells::brain::Brain& kb, int number, const std::string& input, const std::string& output) :
    m_number(number),
    m_inputGrid(fmt::format("Train input {}", number)),
    m_outputGrid(std::make_unique<native::Grid>(fmt::format("Train output {}", number))),
    m_input(kb, m_inputGrid.loadFromJsonArray(input)),
    m_output(std::make_unique<cells::arc::Grid>(kb, m_outputGrid->loadFromJsonArray(output)))
{
}

Task::IOPair::IOPair(cells::brain::Brain& kb, int number, const std::string& input) :
    m_number(number),
    m_inputGrid(fmt::format("Train input {}", number)),
    m_input(kb, m_inputGrid.loadFromJsonArray(input))
{
}

Task::Task(cells::brain::Brain& kb, const nlohmann::json& arcJsonTask) :
    Task(kb, "", arcJsonTask)
{

}

Task::Task(cells::brain::Brain& kb, const std::string& id, const nlohmann::json& jsonTask) :
    m_id(id),
    m_cellTaskStruct(kb.getStruct("arc::Task")),
    m_cellExampleStruct(kb.getStruct("arc::Example")),
    m_cellTask(kb, m_cellTaskStruct),
    m_cellExamplesList(kb, m_cellExampleStruct),
    m_cellTestsList(kb, m_cellExampleStruct)
{
    const nlohmann::json& jsonTrainSet = jsonTask.at("train");
    m_examples.reserve(jsonTrainSet.size());
    m_cellExamples.reserve(jsonTrainSet.size());
    int trainExampleNumber = 1;
    for (const auto& trainExample : jsonTrainSet) {
        m_examples.emplace_back(kb, trainExampleNumber++, to_string(trainExample.at("input")), to_string(trainExample.at("output")));
        IOPair& ioPair = m_examples.back();
        m_cellExamples.emplace_back(kb, m_cellExampleStruct);
        cells::Object& exampleObject = m_cellExamples.back();
        exampleObject.set("input", ioPair.m_input);
        exampleObject.set("output", *ioPair.m_output);
        m_cellExamplesList.add(exampleObject);
    }
    const nlohmann::json& jsonTestSet = jsonTask.at("test");
    m_tests.reserve(jsonTestSet.size());
    m_cellTests.reserve(jsonTrainSet.size());
    int testExampleNumber = 1;
    for (const auto& testExample : jsonTestSet) {
        m_tests.emplace_back(kb, testExampleNumber++, to_string(testExample.at("input")));
        IOPair& ioPair = m_tests.back();
        m_cellTests.emplace_back(kb, m_cellExampleStruct);
        cells::Object& exampleObject = m_cellTests.back();
        exampleObject.set("input", ioPair.m_input);
        m_cellTestsList.add(exampleObject);
    }

    m_cellTask.set("examples", m_cellExamplesList);
    m_cellTask.set("tests", m_cellTestsList);
}

TaskSet::TaskSet(cells::brain::Brain& kb, const std::string& filePath)
{
    auto allTasks = json::parse(std::ifstream(filePath));
    for (json::const_iterator it = allTasks.begin(); it != allTasks.end(); ++it) {
        m_tasks.emplace(std::piecewise_construct,
                        std::forward_as_tuple(it.key()),
                        std::forward_as_tuple(kb, it.key(), it.value()));
    }
}

} // namespace arc
} // namespace infocell