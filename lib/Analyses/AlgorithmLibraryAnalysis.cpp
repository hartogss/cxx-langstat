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
        "std::minmax", "std::minmax_element", "std::clamp")
    ){
    std::cout << "ALA ctor\n";
}

namespace {

// Gathers data on how often standard library types were implicitly instantiated.
void algorithmPrevalence(ordered_json& Statistics, ordered_json j){
    std::map<std::string, unsigned> m;
    std::cout << j.dump(4) << std::endl;
    for(const auto& [Type, Insts] : j.at("func insts").items()){
        m.try_emplace(Type, Insts.size());
        std::cout << Type << ", " << Insts.size() << std::endl;
    }
    std::string desc = "algorithm prevalence";
    Statistics[desc] = m;
}

} // namespace

void AlgorithmLibraryAnalysis::processFeatures(ordered_json j){
    algorithmPrevalence(Statistics, j);
}

//-----------------------------------------------------------------------------
