#ifndef STATS_H
#define STATS_H

#include <nlohmann/json.hpp>

//-----------------------------------------------------------------------------

nlohmann::ordered_json add(nlohmann::ordered_json&& lhs,
    const nlohmann::ordered_json& rhs);

//-----------------------------------------------------------------------------

#endif // STATS_H
