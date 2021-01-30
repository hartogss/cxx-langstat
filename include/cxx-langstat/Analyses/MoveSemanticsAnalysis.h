#ifndef MOVESEMANTICSANALYSIS_H
#define MOVESEMANTICSANALYSIS_H

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------
// https://stackoverflow.com/questions/7312745/static-cast-for-user-defined-types
struct Base {
    Base() = default;
    Base(int Location, std::string Identifier) : Location(Location),
        Identifier(Identifier){}
    int Location;
    std::string Identifier;
};
struct FunctionInfo : public Base {
    FunctionInfo() = default;
    int ByValue = 0;
    int ByNonConstLvalueRef = 0;
    int ByConstLvalueRef = 0;
    int ByRvalueRef = 0;
};
struct FunctionTemplateInfo : public FunctionInfo {
    int ByUniversalRef = 0;
};
enum class ParmKind {
    Value, NonConstLValueRef, ConstLValueRef, RValueRef, UniversalRef
};
struct ParmInfo : public Base {
    ParmInfo() = default;
    ParmInfo(int Location, std::string Identifier) : Base(Location, Identifier){}
    ParmKind Kind;
};

class MoveSemanticsAnalysis : public Analysis {
public:
    MoveSemanticsAnalysis(){
        std::cout << "MSA ctor\n";
    }
    ~MoveSemanticsAnalysis(){
        std::cout << "MSA dtor\n";
    }
private:
    std::vector<FunctionInfo> Functions;
    std::vector<FunctionTemplateInfo> FunctionTemplates;
    std::vector<ParmInfo> Parms;
    // Helper functions for analyzeFeatures
    void addFunction(const Match<clang::FunctionDecl>& match,
        std::map<std::string, bool> ParmMap);
    void addFunction(const Match<clang::FunctionTemplateDecl>& match,
        std::map<std::string, bool> ParmMap);
    template<typename T>
    // Helper function for extractFeatures: for each match figures if it has any
    // of the 4 kinds of parameters and puts it in one of the above vectors
    void associateParameters(const Matches<T>& Matches);
    void extractFeatures();
    void analyzeFeatures() override;
    void processFeatures(nlohmann::ordered_json j) override;
    // Helper function to gather data about functions or parameters into vector
    template<typename T>
    void gatherData(std::string DeclKind,
        const std::vector<T>& fs);
    void ResetAnalysis() override;
};

#endif // MOVESEMANTICSANALYSIS_H

//-----------------------------------------------------------------------------
