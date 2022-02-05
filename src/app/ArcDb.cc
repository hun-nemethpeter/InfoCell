#include "ArcDb.h"

#include <iostream>
#include <fstream>

using namespace nlohmann;

namespace synth {

void ArcDb::load(const std::string& filename)
{
    std::ifstream ifs(filename);
    jf = json::parse(ifs);
#if 0
    //    std::cout << jf.dump(4) << std::endl;
    std::cout << "Input:" << std::endl;
    for (const auto& input : jf.at("train")) {
        for (const auto& col : input.at("input")) {
            std::cout << col << std::endl;
        }
        std::cout << "---" << std::endl;
    }
#endif
}

} // namespace synth