#include <string>
#include <nlohmann/json.hpp>


namespace synth {

class ArcDb
{
public:
    void load(const std::string& filename);
    nlohmann::json jf;
};

} // namespace synth