#ifndef STATS_H
#define STATS_H

#include <nlohmann/json.hpp>

//-----------------------------------------------------------------------------
// Given two JSON objects, where lhs describes the statistics of a single file
// or summary statistics of multiple files, and rhs describes the statistics
// of a single file, combine them to a single summary statistic.
nlohmann::ordered_json add(nlohmann::ordered_json&& lhs,
    const nlohmann::ordered_json& rhs);

//-----------------------------------------------------------------------------

#endif // STATS_H
