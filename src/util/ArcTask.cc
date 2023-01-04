#include "ArcTask.h"

#include "app/Screen.h"
#include <array>

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

ArcDemonstration::ArcDemonstration(const std::string& input, const std::string& output) :
    m_inputScreen("input"),
    m_outputScreen("output"),
    m_input(m_inputScreen.loadFromJsonArray(input)),
    m_output(m_outputScreen.loadFromJsonArray(output))
{
}

ArcTask::ArcTask(const nlohmann::json& jsonArcFile) :
    m_inputScreen("input"),
    m_outputScreen("output"),
    m_testInput(m_inputScreen.loadFromJsonArray(to_string(jsonArcFile["/test/0/input"_json_pointer]))),
    m_testSolution(m_outputScreen.loadFromJsonArray(to_string(jsonArcFile["/test/0/output"_json_pointer])))
{
    const nlohmann::json& jsonTrainSet = jsonArcFile.at("train");
    int size                           = jsonTrainSet.size();
    m_demonstrations.reserve(size);
    for (const auto& train : jsonTrainSet) {
        m_demonstrations.emplace_back(to_string(train.at("input")), to_string(train.at("output")));
    }
}

} // namespace synth