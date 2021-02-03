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
    Matches<clang::ClassTemplateSpecializationDecl> ClassImplicitInsts;
    Matches<clang::ClassTemplateSpecializationDecl> ClassExplicitInsts;
    Matches<clang::DeclaratorDecl> ImplicitInsts;
    Matches<clang::FunctionDecl> FuncInsts;
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
    // Given matches representing the instantiations of some kind, gather
    // for each instantiation the instantiation arguments.
    template<typename T>
    void gatherInstantiationData(Matches<T>& Insts, std::string InstKind,
        bool AreImplicit);
    void ResetAnalysis() override;
    //
    unsigned ImplicitInstCounter=0;
};

//-----------------------------------------------------------------------------

#endif // TEMPLATEINSTANTIATIONANALYSIS_H
