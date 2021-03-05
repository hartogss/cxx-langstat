#include <iostream>
#include <vector>

#include "cxx-langstat/Analyses/UtilityLibAnalysis.h"
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

// Construct a ULA by constructing a more constrained TIA.
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
    // libc++:
    // pair \in <utility>, tuple \in <tuple>, bitset \in <bitset>
    // all smart pointers \in <memory>
    "utility|tuple|bitset|memory|"
    // libstdc++
    // pair \in <bits/stl_pair.h>, tuple \in <tuple>, bitset \in <bitset>
    // unique_ptr \in <bits/unique_ptr.h>
    // shared_ptr, weak_ptr \in <bits/shared_ptr.h>
    "bits/stl_pair.h|bits/unique_ptr.h|bits/shared_ptr.h"){
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
void utilityPrevalence(const ordered_json& in, ordered_json& out){
    templatePrevalence(in, out);
}

// For each container template, gives statistics on how often each instantiation
// was used by a (member) variable.
void utilitytemplateTypeArgPrevalence(const ordered_json& in, ordered_json& out){
    templateTypeArgPrevalence(in, out, NumRelTypes);
}

void UtilityLibAnalysis::processFeatures(ordered_json j){
    if(j.contains(ImplicitClassKey)){
        ordered_json res1;
        ordered_json res2;
        utilityPrevalence(j.at(ImplicitClassKey), res1);
        utilitytemplateTypeArgPrevalence(j.at(ImplicitClassKey), res2);
        Statistics[UtilityPrevalenceKey] = res1;
        Statistics[UtilitiedTypesPrevalenceKey] = res2;
    }
}

//-----------------------------------------------------------------------------
