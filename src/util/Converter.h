#pragma once

#include <nlohmann/json.hpp>
#include "cells/Cells.h"

namespace synth {

class JsonMatrixToCellConverter
{
public:
    JsonMatrixToCellConverter(const nlohmann::json& jsonDb, const std::string& path);

    void convert();

    const cells::Sensor& sensor() const;

private:
    cells::Sensor m_sensor;
    const nlohmann::json& m_jsonDb;
    const std::string& m_path;
    const nlohmann::json& m_jMatrix;
};

} // namespace synth