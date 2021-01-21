#include <iostream>
#include <vector>

#include "llvm/Support/raw_ostream.h"

#include "cxx-langstat/Analyses/StdlibAnalysis2.h"
#include "cxx-langstat/Analyses/TemplateInstantiationAnalysis.h"
#include "cxx-langstat/Utils.h"

using namespace clang;
using namespace clang::ast_matchers;
using ordered_json = nlohmann::ordered_json;

//-----------------------------------------------------------------------------
// How often were constructs from standard library used (like vector, array,
// map, list, unordered_map, set etc.). Were they used directly as type,
// or as part of another constructs? What behavior can we see when they are
// passed around? What sizes do they occur (#elements, constexpr)?
// Usage in templates and TMP?

// Construct a SLA by constructing a more constrained TIA.
StdlibAnalysis2::StdlibAnalysis2() : TemplateInstantiationAnalysis(true,
    hasAnyName(
        // Standard library containers, Copied from UseAutoCheck.cpp
        "std::array", "std::vector",
        "std::forward_list", "std::list",
        "std::map", "std::multimap",
        "std::set", "std::multiset",
        "std::unordered_map", "std::unordered_multimap",
        "std::unordered_set", "std::unordered_multiset",
        "std::queue", "std::priority_queue", "std::stack", "std::deque",
        // Standard library utilities
        // vocabulary types
        "std::pair", "std::tuple",
        // Dynamic memory
        "std::unique_ptr", "std::shared_ptr", "std::weak_ptr"
        )
){
    std::cout << "SLA2 ctor\n";
}

ordered_json containerPrevalence(ordered_json Statistics, ordered_json j){
    std::map<std::string, unsigned> m;
    for(const auto& [containertype, containeroccurrences] : j["implicit class insts"].items()){
        m.try_emplace(containertype, containeroccurrences.size());
    }
    std::string desc = "container type prevalences";
    Statistics[desc] = m;
    return Statistics;
}

// WIP: find popular types for containers
// ordered_json containedType(ordered_json Statistics, ordered_json j){
//     std::map<std::string, std::map<std::string, unsigned>> m;
//     for(const auto& [containertype, containeroccurrences] : j["implicit class insts"].items()){
//         m.try_emplace(containertype, containeroccurrences.size());
//     }
//     std::string desc = "contained type prevalences";
//     Statistics[desc] = m;
//     return Statistics;
// }

void StdlibAnalysis2::processFeatures(nlohmann::ordered_json j){
    Statistics = containerPrevalence(Statistics, j);

}

//-----------------------------------------------------------------------------
