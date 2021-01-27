#include <nlohmann/json.hpp>

nlohmann::ordered_json add(nlohmann::ordered_json&& lhs,
    const nlohmann::ordered_json& rhs);
