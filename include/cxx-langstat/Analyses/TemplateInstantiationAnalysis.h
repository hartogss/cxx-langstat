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
        clang::ast_matchers::internal::Matcher<clang::NamedDecl> Names);
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
    void ResetAnalysis() override;
    //
    unsigned VariablesCounter=0;
    unsigned CallersCounter=0;
};

//-----------------------------------------------------------------------------

#endif // TEMPLATEINSTANTIATIONANALYSIS_H
