#include <iostream>
#include <vector>

#include "cxx-langstat/Analyses/AlgorithmLibraryAnalysis.h"
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
        // Non-modifying sequence ooperations
        "std::all_of", "std::any_of", "std::none_of",
        "std::for_each", "std::for_each_n",
        "std::count", "std::count_if",
        "std::mismatch",
        "std::find", "std::find_if", "std::find_if_not",
        "std::find_end", "std::find_first_of", "std::adjacent_find",
        "std::search", "std::search_n",
        // Minimum/maximum operations
        "std::max", "std::max_element", "std::min", "std::min_element",
        "std::minmax", "std::minmax_element", "std::clamp",
        // Modifying sequence operations
        "std::move"
    ),
    // libc++
    "algorithm|"
    // libstdc++
    "bits/stl_algo.h|"
    "bits/stl_algobase.h"
    ){
    std::cout << "ALA ctor\n";
}

// Gathers data on how often each algorithm template was used.
void algorithmPrevalence(const ordered_json& in, ordered_json& out){
    templatePrevalence(in, out);
}

void AlgorithmLibraryAnalysis::processFeatures(ordered_json j){
    if(j.contains(FuncKey)){
        ordered_json res;
        algorithmPrevalence(j.at(FuncKey), res);
        Statistics[AlgorithmPrevalenceKey] = res;
    }
}

//-----------------------------------------------------------------------------
