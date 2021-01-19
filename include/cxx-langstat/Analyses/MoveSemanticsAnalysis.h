#ifndef MOVESEMANTICSANALYSIS_H
#define MOVESEMANTICSANALYSIS_H

#include "cxx-langstat/Analysis.h"

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
    // Vector containing all function having at least one certain kind of param
    Matches<clang::FunctionDecl> FuncsWithValueParm;
    Matches<clang::FunctionDecl> FuncsWithNonConstLValueRefParm;
    Matches<clang::FunctionDecl> FuncsWithConstLValueRefParm;
    Matches<clang::FunctionDecl> FuncsWithRValueRefParm;
    Matches<clang::FunctionTemplateDecl> FuncTemplatesWithValueParm;
    Matches<clang::FunctionTemplateDecl> FuncTemplatesWithNonConstLValueRefParm;
    Matches<clang::FunctionTemplateDecl> FuncTemplatesWithConstLValueRefParm;
    Matches<clang::FunctionTemplateDecl> FuncTemplatesWithRValueRefParm;
    Matches<clang::FunctionTemplateDecl> FuncTemplatesWithUniversalRefParm;
    // Vector storing the params
    Matches<clang::ParmVarDecl> ValueParms;
    Matches<clang::ParmVarDecl> NonConstLValueRefParms;
    Matches<clang::ParmVarDecl> ConstLValueRefParms;
    Matches<clang::ParmVarDecl> RValueRefParms;
    Matches<clang::ParmVarDecl> UniversalRefParms;
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
    void gatherData(std::string DeclKind, std::string PassKind,
        const Matches<T>& Matches);
};

#endif // MOVESEMANTICSANALYSIS_H

//-----------------------------------------------------------------------------
