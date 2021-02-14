#ifndef TEMPLATEINSTANTIATIONANALYSIS_H
#define TEMPLATEINSTANTIATIONANALYSIS_H

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

enum class InstKind {
    Class, Function, Variable, Any
};

class TemplateInstantiationAnalysis : public Analysis {
public:
    // Ctor to let TIA look for any kind of instantiations
    TemplateInstantiationAnalysis();
    // Ctor to instrument TIA to look only for instantiations of kind IK
    // named any of "Names".
    TemplateInstantiationAnalysis(InstKind IK,
        clang::ast_matchers::internal::Matcher<clang::NamedDecl> Names,
        std::string HeaderRegex);
    ~TemplateInstantiationAnalysis(){
        std::cout << "TIA dtor\n";
    }
private:
    InstKind IK;
    clang::ast_matchers::internal::Matcher<clang::NamedDecl> Names;
    // CTSDs that appeared in explicit instantiation.
    Matches<clang::ClassTemplateSpecializationDecl> ClassImplicitInsts;
    // CTSDs that appeared in implicit instantiation or where used by a variable
    // that has CTSD type.
    Matches<clang::ClassTemplateSpecializationDecl> ClassExplicitInsts;
    // Declarations that declare a variable of type CTSD from the list above.
    Matches<clang::DeclaratorDecl> Variables;
    // Function instantiations referenced by calls.
    Matches<clang::FunctionDecl> FuncInsts;
    // Call exprs that cause an implicit function instantiation or refer to it.
    Matches<clang::CallExpr> Callers;
    //
    Matches<clang::VarTemplateSpecializationDecl> VarInsts;
    // Responsible to fill vectors of matches defined above
    void extractFeatures();
    void analyzeFeatures() override;
    void processFeatures(nlohmann::ordered_json j) override;
    // Get location of instantiation
    template<typename T>
    unsigned getInstantiationLocation(const Match<T>& Match, bool isImplicit);
    // Get location of class instantiation, we need to be a bit more careful
    unsigned getInstantiationLocation
        (const Match<clang::ClassTemplateSpecializationDecl>& Match,
            bool isImplicit);

    unsigned getInstantiationLocation(const Match<clang::FunctionDecl>& Match,
            bool isImplicit);
    // Given matches representing the instantiations of some kind, gather
    // for each instantiation the instantiation arguments.
    template<typename T>
    void gatherInstantiationData(Matches<T>& Insts, std::string InstKind,
        bool AreImplicit);
    //
    unsigned VariablesCounter = 0;
    unsigned CallersCounter = 0;
    // Regex specifying header names from which the template whose instantiations
    // we're looking from has to come from.
    // Don't go specify here two headers that specify something with the same
    // name, like utility|algorithm and then expect your statistics for std::move
    // to make sense.
    std::string HeaderRegex;
};

template<typename T>
using StringMap = std::map<std::string, T>;

// For "Type", gets how many type arguments of the instantiation we want to know.
int getNumRelevantTypes(llvm::StringRef Type, const StringMap<int>& SM);

// For "Type", gets from "Types" all relevant types of the
// instantantiation.
std::string getRelevantTypesAsString(llvm::StringRef Type,
    nlohmann::json Types, const StringMap<int>& SM);

// For each template in "in", computes how often it was used by variables.
void typePrevalence(const nlohmann::ordered_json& in,
    nlohmann::ordered_json& out);

// For each template in "in", computes for each set of template type arguments
// how often a particular instantiation was used by a (member) variable.
void instantiationTypeArgs(const nlohmann::ordered_json& in,
    nlohmann::ordered_json& out, const StringMap<int>& SM);

//-----------------------------------------------------------------------------

#endif // TEMPLATEINSTANTIATIONANALYSIS_H
