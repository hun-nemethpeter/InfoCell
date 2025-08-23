#include "ArcTask.h"

#include "app/Picture.h"
#include <array>
#include <fmt/core.h>
#include <fstream>

using namespace nlohmann;
using namespace infocell::input;

namespace infocell {

ArcDemonstration::ArcDemonstration(cells::brain::Brain& kb, int number, const std::string& input, const std::string& output) :
    m_number(number),
    m_inputPicture(fmt::format("Train input {}", number)),
    m_outputPicture(fmt::format("Train output {}", number)),
    m_input(kb, m_inputPicture.loadFromJsonArray(input)),
    m_output(kb, m_outputPicture.loadFromJsonArray(output))
{
}

ArcTask::ArcTask(cells::brain::Brain& kb, const nlohmann::json& jsonArcFile) :
    m_inputPicture("Test input"),
    m_outputPicture("Test solution"),
    m_challenge(kb, m_inputPicture.loadFromJsonArray(to_string(jsonArcFile["/test/0/input"_json_pointer]))),
    m_solution(kb, m_outputPicture.loadFromJsonArray(to_string(jsonArcFile["/test/0/output"_json_pointer]))),
    m_taskStruct(kb.getStruct("arc::Task")),
    m_demonstrationStruct(kb.getStruct("arc::Demonstration")),
    m_task(kb, m_taskStruct),
    m_examples(kb, m_demonstrationStruct)
{
    const nlohmann::json& jsonTrainSet = jsonArcFile.at("train");
    m_demonstrations.reserve(jsonTrainSet.size());
    m_exampleObjects.reserve(jsonTrainSet.size());
    int number = 1;
    for (const auto& train : jsonTrainSet) {
        m_demonstrations.emplace_back(kb, number++, to_string(train.at("input")), to_string(train.at("output")));
        ArcDemonstration& arcDemonstration = m_demonstrations.back();
        m_exampleObjects.emplace_back(kb, m_demonstrationStruct);
        cells::Object& exampleObject = m_exampleObjects.back();
        exampleObject.set("input", arcDemonstration.m_input);
        exampleObject.set("output", arcDemonstration.m_output);
        m_examples.add(exampleObject);
    }
    m_task.set("examples", m_examples);
    m_task.set("challenge", m_challenge);
    m_task.set("solution", m_solution);
}

ArcTaskExample::ArcTaskExample(cells::brain::Brain& kb, int number, const std::string& input, const std::string& output) :
    m_number(number),
    m_inputGrid(fmt::format("Train input {}", number)),
    m_outputGrid(std::make_unique<input::Grid>(fmt::format("Train output {}", number))),
    m_input(kb, m_inputGrid.loadFromJsonArray(input)),
    m_output(std::make_unique<cells::hybrid::arc::Grid>(kb, m_outputGrid->loadFromJsonArray(output)))
{
}

ArcTaskExample::ArcTaskExample(cells::brain::Brain& kb, int number, const std::string& input) :
    m_number(number),
    m_inputGrid(fmt::format("Train input {}", number)),
    m_input(kb, m_inputGrid.loadFromJsonArray(input))
{
}

ArcPrizeTask::ArcPrizeTask(cells::brain::Brain& kb, const std::string& id, const nlohmann::json& jsonTask) :
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
        ArcTaskExample& arcDemonstration = m_examples.back();
        m_cellExamples.emplace_back(kb, m_cellExampleStruct);
        cells::Object& exampleObject = m_cellExamples.back();
        exampleObject.set("input", arcDemonstration.m_input);
        exampleObject.set("output", *arcDemonstration.m_output);
        m_cellExamplesList.add(exampleObject);
    }
    const nlohmann::json& jsonTestSet = jsonTask.at("test");
    m_tests.reserve(jsonTestSet.size());
    m_cellTests.reserve(jsonTrainSet.size());
    int testExampleNumber = 1;
    for (const auto& testExample : jsonTestSet) {
        m_tests.emplace_back(kb, testExampleNumber++, to_string(testExample.at("input")));
        ArcTaskExample& arcDemonstration = m_tests.back();
        m_cellTests.emplace_back(kb, m_cellExampleStruct);
        cells::Object& exampleObject = m_cellTests.back();
        exampleObject.set("input", arcDemonstration.m_input);
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

} // namespace infocell