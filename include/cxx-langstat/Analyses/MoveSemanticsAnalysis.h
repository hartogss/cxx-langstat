#ifndef MOVESEMANTICSANALYSIS_H
#define MOVESEMANTICSANALYSIS_H

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------
// https://stackoverflow.com/questions/7312745/static-cast-for-user-defined-types
// Basic information we want to capture both about functions and parameters
struct BasicInfo {
    BasicInfo() = default;
    BasicInfo(int Location, std::string Identifier, std::string Type) :
        Location(Location),
        Identifier(Identifier),
        Type(Type){}
    int Location;
    std::string Identifier;
    std::string Type;
};
// Holds for each kind of parameter how often it occured in function decl.
struct FunctionInfo : public BasicInfo {
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
// Holds for what kind of passing is used for that parameter.
struct ParmInfo : public BasicInfo {
    ParmInfo() = default;
    ParmInfo(int Location, std::string Identifier, std::string Type) :
        BasicInfo(Location, Identifier, Type){}
    ParmKind Kind;
    bool isInstantiationDependent;
};

//-----------------------------------------------------------------------------

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
