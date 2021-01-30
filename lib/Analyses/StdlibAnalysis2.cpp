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
        "std::array", "std::vector", "std::deque",
        "std::forward_list", "std::list",
        "std::set", "std::map",
        "std::multiset", "std::multimap",
        "std::unordered_set", "std::unordered_map",
        "std::unordered_multiset", "std::unordered_multimap",
        "std::stack", "std::queue", "std::priority_queue",
        // Standard library utilities
        // vocabulary types
        "std::pair", "std::tuple",
        "std::bitset",
        // Dynamic memory
        "std::unique_ptr", "std::shared_ptr", "std::weak_ptr"
        )
){
    std::cout << "SLA2 ctor\n";
}

template<typename T>
using StringMap = std::map<std::string, T>;

// Map that for a stdlib type contains how many instantiation type args are
// intersting to us, e.g. for the instantiation stored in a .json below,
// "std::__1::vector" : {
//     "location": "42"
//     "arguments": {
//         "non-type": [],
//         "type": [
//             "int",
//             "std::__1::allocator<int>"
//         ],
//         "template": []
//     }
// }
// the result is 1, since we only care about the fact that std::vector was
// instantiated with int.
// -1 indicates that we want all arguments interest us.
// Used to analyze types contained in containers, utilities, smart pointer etc.
const StringMap<int> NumRelTypes = { // no constexpr support for map
    // ** Containers **
    // sequential containers
    {"std::array", 1}, {"std::vector", 1}, {"std::deque", 1},
    {"std::forward_list", 1}, {"std::list", 1},
    // associative containers
    {"std::set", 1}, {"std::map", 2},
    {"std::multiset", 1}, {"std::multimap", 2},
    // Same, but unordered
    {"std::unordered_set", 1}, {"std::unordered_map", 2},
    {"std::unordered_multiset", 1}, {"std::unordered_multimap", 2},
    // container adaptors
    {"std::stack", 1}, {"std::queue", 1}, {"std::priority_queue", 1},
    // ** Utilities **
    // pairs and tuples
    {"std::pair", 2}, {"std::tuple", -1},
    // bitset
    {"std::bitset", 0},
    // smart pointers
    {"std::unique_ptr", 1}, {"std::shared_ptr", 1}, {"std::weak_ptr", 1}
};

// For some standard library type, get how many type arguments of instantiations
// are interesting to us. Have to do some extra work in case it is std::__1-
// qualified.
int GetNumRelevantTypes(llvm::StringRef Type){
    if(auto [l,r] = Type.split("::__1"); !r.empty()){
        return NumRelTypes.at((l+r).str());
    }
    return NumRelTypes.at(Type.str());
}

// Actually gets the type arguments and concatenates them into a string.
std::string GetRelevantTypesAsString(llvm::StringRef ContainerType,
    nlohmann::json Types){
        // std::cout << ContainerType.str() << std::endl;
        int n = GetNumRelevantTypes(ContainerType);
        if(n == -1)
            n = Types.end()-Types.begin();
        std::string t;
        for(nlohmann::json::iterator i=Types.begin(); i<Types.begin()+n; i++)
            t = t + (*i).get<std::string>() + ", ";
        if(n)
            return llvm::StringRef(t).drop_back(2).str();
        else
            return "";
}

// Gathers data on how often standard library types were implicitly instantiated.
void stdlibTypePrevalence(ordered_json& Statistics, ordered_json j){
    std::map<std::string, unsigned> m;
    for(const auto& [Type, Insts] : j["implicit class insts"].items()){
        m.try_emplace(Type, Insts.size());
    }
    std::string desc = "stdlib type prevalence";
    Statistics[desc] = m;
}

// For standard library types, gathers data on what types
// they were implicitly instantiated with.
void stdlibInstantiationTypeArgs(ordered_json& Statistics, ordered_json j){
    StringMap<StringMap<unsigned>> m;
    for(const auto& [Type, Insts] : j["implicit class insts"].items()){
        for(const auto& Inst : Insts){
            m.try_emplace(Type, StringMap<unsigned>());
            nlohmann::json ContainedTypes = Inst["arguments"]["type"];
            assert(ContainedTypes.is_array());
            auto TypeString = GetRelevantTypesAsString(Type, ContainedTypes);
            // std::cout << TypeString << std::endl;
            if(!TypeString.empty()){
                m.at(Type).try_emplace(TypeString, 0);
                m.at(Type).at(TypeString)++;
            }
        }
    }
    std::string desc = "stdlib instantiation type arguments";
    Statistics[desc] = m;
}

void StdlibAnalysis2::processFeatures(nlohmann::ordered_json j){
    stdlibTypePrevalence(Statistics, j);
    stdlibInstantiationTypeArgs(Statistics, j);

}

//-----------------------------------------------------------------------------
