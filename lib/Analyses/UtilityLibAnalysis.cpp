#include <iostream>
#include <vector>

#include "llvm/Support/raw_ostream.h"

#include "cxx-langstat/Analyses/UtilityLibAnalysis.h"
#include "cxx-langstat/Analyses/TemplateInstantiationAnalysis.h"
#include "cxx-langstat/Utils.h"

using namespace clang;
using namespace clang::ast_matchers;
using json = nlohmann::json;
using ordered_json = nlohmann::ordered_json;

//-----------------------------------------------------------------------------
// How often were constructs from standard library used (like vector, array,
// map, list, unordered_map, set etc.). Were they used directly as type,
// or as part of another constructs? What behavior can we see when they are
// passed around? What sizes do they occur (#elements, constexpr)?
// Usage in templates and TMP?

// Construct a SLA by constructing a more constrained TIA.
UtilityLibAnalysis::UtilityLibAnalysis() : TemplateInstantiationAnalysis(
    InstKind::Class,
    hasAnyName(
        // Standard library utilities
        // vocabulary types
        "std::pair", "std::tuple",
        "std::bitset",
        // Dynamic memory
        "std::unique_ptr", "std::shared_ptr", "std::weak_ptr"
    ),
    "utility|tuple|bitset|memory"){
    std::cout << "ULA ctor\n";
}

namespace {

const StringMap<int> NumRelTypes = { // no constexpr support for map
    // ** Utilities **
    // pairs and tuples
    {"std::pair", 2}, {"std::tuple", -1},
    // bitset
    {"std::bitset", 0},
    // smart pointers
    {"std::unique_ptr", 1}, {"std::shared_ptr", 1}, {"std::weak_ptr", 1}
};

} // namespace


// Gathers data on how often each utility template was used.
void utilityPrevalence(ordered_json& Statistics, ordered_json j){
    ordered_json res;
    typePrevalence(j, res);
    Statistics["utility type prevalence"] = res;
}

// For each container template, gives statistics on how often each instantiation
// was used by a (member) variable.
void utilityInstantiationTypeArgs(ordered_json& Statistics, ordered_json j){
    ordered_json res;
    instantiationTypeArgs(j, res, NumRelTypes);
    Statistics["utility instantiation type arguments"] = res;
}

void UtilityLibAnalysis::processFeatures(ordered_json j){
    if(j.contains("implicit class insts")){
        utilityPrevalence(Statistics, j.at("implicit class insts"));
        utilityInstantiationTypeArgs(Statistics, j.at("implicit class insts"));
    }
}

//-----------------------------------------------------------------------------
