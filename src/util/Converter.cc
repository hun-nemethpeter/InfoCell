#include "Converter.h"

#include <array>

using namespace nlohmann;

namespace synth {

const std::array<cells::Color, 10> cellColors = {
    cells::Color(0x00, 0x00, 0x00), /* black */
    cells::Color(0x00, 0x74, 0xD9), /* blue */
    cells::Color(0xFF, 0x41, 0x36), /* red */
    cells::Color(0x2E, 0xCC, 0x40), /* green */
    cells::Color(0xFF, 0xDC, 0x00), /* yellow */
    cells::Color(0xAA, 0xAA, 0xAA), /* grey */
    cells::Color(0xF0, 0x12, 0xBE), /* fuschia */
    cells::Color(0xFF, 0x85, 0x1B), /* orange */
    cells::Color(0x7F, 0xDB, 0xFF), /* teal */
    cells::Color(0x87, 0x0C, 0x25)  /* brown */
};

static cells::Sensor convertArcMatrixToSensor(const nlohmann::json& arcMatrix);

ArcTask ConvertJsonToCell(const nlohmann::json& jsonArcFile)
{
    ArcTask arcTask;
    const auto& trainSet = jsonArcFile.at("train");
    for (const auto& train : trainSet) {
        const auto& inputMatrix  = train.at("input");
        const auto& outputMatrix = train.at("output");
        ArcDemonstration arcDemonstration;
        arcDemonstration.m_input = convertArcMatrixToSensor(inputMatrix);
        arcDemonstration.m_output = convertArcMatrixToSensor(outputMatrix);
        arcTask.m_demonstrations.push_back(arcDemonstration);
    }
    arcTask.m_testInput = convertArcMatrixToSensor(jsonArcFile["/test/0/input"_json_pointer]);
    arcTask.m_testSolution = convertArcMatrixToSensor(jsonArcFile["/test/0/output"_json_pointer]);

    return arcTask;
}

static cells::Sensor convertArcMatrixToSensor(const nlohmann::json& arcMatrix)
{
    cells::Sensor sensor;
    size_t matrixHeight = arcMatrix.size();
    size_t matrixWidth  = arcMatrix[0].size();

    sensor.height(matrixHeight).width(matrixWidth);
    sensor.init();

    int y = 0;
    for (auto inputRowIt = arcMatrix.begin(); inputRowIt != arcMatrix.end(); ++inputRowIt) {
        int x = 0;
        for (const int val : *inputRowIt) {
            cells::Pixel& pixel = sensor.getPixel(x++, y);
            pixel.color         = cellColors[val];
        }
        ++y;
    }

    return sensor;
}

} // namespace synth