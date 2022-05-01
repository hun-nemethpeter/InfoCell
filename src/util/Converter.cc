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

JsonMatrixToCellConverter::JsonMatrixToCellConverter(const json& jsonDb, const std::string& path) :
    m_jsonDb(jsonDb), m_path(path), m_jMatrix(jsonDb[json::json_pointer(m_path)])
{
    convert();
}

    void JsonMatrixToCellConverter::convert()
{
    size_t matrixHeight = m_jMatrix.size();
    size_t matrixWidth  = m_jMatrix[0].size();

    m_sensor.height(matrixHeight).width(matrixWidth);
    m_sensor.init();

    int y = 0;
    for (auto inputRowIt = m_jMatrix.begin(); inputRowIt != m_jMatrix.end(); ++inputRowIt) {
        int x = 0;
        for (const int val : *inputRowIt) {
            cells::Pixel& pixel = m_sensor.getPixel(x, y);
            pixel.color         = cellColors[val];
        }
    }
}

const cells::Sensor& JsonMatrixToCellConverter::sensor() const
{
    return m_sensor;
}

} // namespace synth