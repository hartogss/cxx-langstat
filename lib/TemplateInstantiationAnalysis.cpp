#include <iostream>
#include <vector>

#include "llvm/Support/raw_ostream.h"

#include "cxx-langstat/TemplateInstantiationAnalysis.h"
#include "cxx-langstat/Utils.h"

using namespace clang;
using namespace clang::ast_matchers;

//-----------------------------------------------------------------------------
// Compute statistics on arguments used to instantiate templates.
// Should be divided into 3 categories:
// - classes,
// - functions (including member methods), &
// - variables (including class static member variables, (not
// class field, those cannot be templated if they're not static))
// Template instantiations counted should stem from either explicit instantiations
// written by programmers or from implicit ones through 'natural usage'.
//
// Remember that template parameters can be non-types, types or templates.


TemplateInstantiationAnalysis::TemplateInstantiationAnalysis
(clang::ASTContext& Context) : Analysis(Context) {
}
void TemplateInstantiationAnalysis::extract() {
    // auto m1 = cxxRecordDecl(isTemplateInstantiation()).bind("classinsts");

    auto m2 = functionDecl(isTemplateInstantiation()).bind("funcinsts");
    auto FuncInsts = Extr.extract("funcinsts", m2);
    for(auto m : FuncInsts){
        std::cout << getDeclName(m) << std::endl;
        // auto specKind = cast<FunctionDecl>(m.node)->getTemplateSpecializationKindForInstantiation();
        // std::cout << specKind << std::endl;
        auto TALptr = cast<FunctionDecl>(m.node)->getTemplateSpecializationArgs();
        for(unsigned idx=0; idx<TALptr->size(); idx++){
            std::string res;
            auto Targ = TALptr->get(idx);
            llvm::raw_string_ostream OS(res);
            Targ.dump(OS);
            std::cout << res << "\n";
        }

    }
}
void TemplateInstantiationAnalysis::analyze(){
}
void TemplateInstantiationAnalysis::run(){
    std::cout << "\033[32mRunning template instantiation analysis:\033[0m" << std::endl;
    extract();
    analyze();
}

//-----------------------------------------------------------------------------
