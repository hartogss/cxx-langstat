#include <iostream>
#include <vector>

#include "llvm/Support/raw_ostream.h"

#include "cxx-langstat/Analyses/AlgorithmLibraryAnalysis.h"
#include "cxx-langstat/Analyses/TemplateInstantiationAnalysis.h"
#include "cxx-langstat/Utils.h"

using namespace clang;
using namespace clang::ast_matchers;
using json = nlohmann::json;
using ordered_json = nlohmann::ordered_json;

//-----------------------------------------------------------------------------
// Construct a ALA by constructing a more constrained TIA.
AlgorithmLibraryAnalysis::AlgorithmLibraryAnalysis() : TemplateInstantiationAnalysis(
    InstKind::Function,
    hasAnyName(
        // https://en.cppreference.com/w/cpp/algorithm
        // Minimum/maximum operations
        "std::max", "std::max_element", "std::min", "std::min_element",
        "std::minmax", "std::minmax_element", "std::clamp",
        // Modifying sequence operations
        "std::move"
    ),
    "algorithm"
    ){
    std::cout << "ALA ctor\n";
}

// Gathers data on how often each algorithm template was used.
void algorithmPrevalence(ordered_json& Statistics, ordered_json j){
    ordered_json res;
    typePrevalence(j, res);
    Statistics["algorithm type prevalence"] = res;
}

void AlgorithmLibraryAnalysis::processFeatures(ordered_json j){
    algorithmPrevalence(Statistics, j.at("func insts"));
}

//-----------------------------------------------------------------------------
