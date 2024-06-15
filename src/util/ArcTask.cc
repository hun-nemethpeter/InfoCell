#include "ArcTask.h"

#include "app/Picture.h"
#include <array>
#include <fmt/core.h>

using namespace nlohmann;
using namespace synth::input;

namespace synth {

static const std::array<Color, 10> cellColors = {
    Color(0x00, 0x00, 0x00), /* black */
    Color(0x00, 0x74, 0xD9), /* blue */
    Color(0xFF, 0x41, 0x36), /* red */
    Color(0x2E, 0xCC, 0x40), /* green */
    Color(0xFF, 0xDC, 0x00), /* yellow */
    Color(0xAA, 0xAA, 0xAA), /* grey */
    Color(0xF0, 0x12, 0xBE), /* fuschia */
    Color(0xFF, 0x85, 0x1B), /* orange */
    Color(0x7F, 0xDB, 0xFF), /* teal */
    Color(0x87, 0x0C, 0x25)  /* brown */
};

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

} // namespace synth