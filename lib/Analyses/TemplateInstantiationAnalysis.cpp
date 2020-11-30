#include <iostream>
#include <vector>

#include "llvm/Support/raw_ostream.h"
#include "clang/AST/TemplateBase.h"

#include "cxx-langstat/Analyses/TemplateInstantiationAnalysis.h"
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
// Goal: for each class, function and variable templates, report:
// - Number of instantiations
// - for individually non-types, types, templates parameters report how often
// they occured.
// - what those arguments actually were:
//      - non-types: what types did those parameters have, possible range of values used?
//      - types: what types were templates instantiated with?
//      - templates: nothing.

std::string get(const Type* t);

TemplateInstantiationAnalysis::TemplateInstantiationAnalysis
(clang::ASTContext& Context) : Analysis(Context) {
}
void TemplateInstantiationAnalysis::extract() {

    auto m1 = cxxRecordDecl(isExpansionInMainFile(), isTemplateInstantiation()).bind("classinsts");
    auto ClassInsts = Extr.extract("classinsts", m1);
    for(auto m : ClassInsts){
        std::cout << getDeclName(m) << std::endl;
        // auto specKind = cast<FunctionDecl>(m.node)->getTemplateSpecializationKindForInstantiation();
        // std::cout << specKind << std::endl;
        // auto TALptr = cast<FunctionDecl>(m.node)->getTemplateSpecializationArgs();
        // for(unsigned idx=0; idx<TALptr->size(); idx++){
            // std::string res;
            // auto Targ = TALptr->get(idx);
            // llvm::raw_string_ostream OS(res);
            // Targ.dump(OS);
            // std::cout << res << "\n";
            // std::cout << dyn_cast<CXXRecordDecl>(m.node)->getType().getTypePtr()->getTypeClassName() << std::endl;
            // std::cout << "2: " << getInnerType(dyn_cast<FunctionDecl>(m.node)) << std::endl;
        // }
    }
    std::cout << "--------" << std::endl;

    unsigned FTInstNonTypeArguments = 0;
    unsigned FTInstTypeArguments = 0;
    unsigned FTInstTemplateArguments = 0;

    auto m2 = functionDecl(isExpansionInMainFile(), isTemplateInstantiation()).bind("funcinsts");
    auto FuncInsts = Extr.extract("funcinsts", m2);
    for(auto m : FuncInsts){
        std::cout << getDeclName(m) << std::endl;
        auto specKind = cast<FunctionDecl>(m.node)->getTemplateSpecializationKindForInstantiation();
        std::cout << "specialization kind: " << specKind << std::endl;
        auto TALptr = cast<FunctionDecl>(m.node)->getTemplateSpecializationArgs();
        for(unsigned idx=0; idx<TALptr->size(); idx++){
            std::string res;
            auto Targ = TALptr->get(idx);
            llvm::raw_string_ostream OS(res);
            Targ.dump(OS);
            std::cout << res << "\n";
            switch (Targ.getKind()){
                case TemplateArgument::Null:
                case TemplateArgument::NullPtr:
                    break;
                case TemplateArgument::Type:
                    FTInstTypeArguments++; break;
                case TemplateArgument::Declaration:
                case TemplateArgument::Integral:
                    FTInstNonTypeArguments++; break;
                case TemplateArgument::Template:
                    FTInstTemplateArguments++; break;
                // in case that it is a pack, can that be a pack of templates?
                // parameter pack can be of anything
            }
            // std::cout << dyn_cast<FunctionDecl>(m.node)->getType().getTypePtr()->getTypeClassName() << std::endl;
            // std::cout << "2: " << getInnerType(dyn_cast<FunctionDecl>(m.node)) << std::endl;
        }
    }
    std::cout << FTInstNonTypeArguments << ", " << FTInstTypeArguments << ", "
        << FTInstTemplateArguments << "\n";

    // auto allvars = Extr.extract("vars", varDecl(isExpansionInMainFile()).bind("vars"));
    // printStatistics("all vars: ", allvars);
    //
    // std::cout << "--------" << std::endl;
    //
    // auto m3 = varDecl(isExpansionInMainFile(), isTemplateInstantiation()).bind("varinsts");
    // auto VarInsts = Extr.extract("varinsts", m3);
    //
    // unsigned VTInstNonTypeArguments = 0;
    // unsigned VTInstTypeArguments = 0;
    // unsigned VTInstTemplateArguments = 0;
    //
    // for(auto m : VarInsts){
    //     std::cout << getDeclName(m) << std::endl;
    //
    //     if(auto Node = cast<VarDecl>(m.node)){
    //
    //         // Node->getType().dump();
    //
    //         auto specKind = Node->getTemplateSpecializationKindForInstantiation();
    //         std::cout << "specialization kind: " << specKind << std::endl;
    //
    //         std::cout << Node->getType().getTypePtr()->getTypeClassName() << std::endl;
    //         std::cout << "test: " << get(Node->getType().getTypePtr()) << std::endl;
    //     }
    //     std::cout << "--------" << std::endl;
    // }
}

std::string get(const Type* t){
    if(auto tprime = dyn_cast<ElaboratedType>(t)){
        auto subt = tprime->getNamedType().getTypePtr();
        return get(subt);
    }
    if(auto tprime = dyn_cast<TemplateSpecializationType>(t)){
        auto numTargs = tprime->getNumArgs();
        std::string res = "";
        if(numTargs>1)
            res.append("(");
        for(unsigned idx=0; idx<numTargs; idx++){
            if(idx)
                res.append(",");
            auto Targ = tprime->getArg(idx);
            llvm::raw_string_ostream OS(res);
            Targ.dump(OS);
        }
        if(numTargs>1)
            res.append(")");
        return res;
    }
    if(auto tprime = dyn_cast<SubstTemplateTypeParmType>(t)){
        return tprime->getReplacementType().getAsString();
        // auto TTPT = n->getReplacedParameter();
        // std::cout << "pp " << TTPT->isParameterPack() << std::endl;
    }
    if(auto tprime = dyn_cast<BuiltinType>(t)){
        return tprime->desugar().getAsString();
    }
    return "fail";
}

void TemplateInstantiationAnalysis::analyze(){
}
void TemplateInstantiationAnalysis::run(){
    std::cout << "\033[32mRunning template instantiation analysis:\033[0m" << std::endl;
    extract();
    analyze();
}

//-----------------------------------------------------------------------------
