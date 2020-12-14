#include <iostream>
#include <vector>

// #include "llvm/Support/raw_ostream.h"

#include "cxx-langstat/Analyses/TemplateParameterAnalysis.h"
#include "cxx-langstat/Utils.h"

using namespace clang;
using namespace clang::ast_matchers;

//-----------------------------------------------------------------------------
// For each kind of template (class, function, variable, alias) individually:
// √ How many templates use parameter packs/are variadic?
// √ Give distribution of the parameters used: nontype, type, template.

// TODO: Other possible statistics:
// - How many templates use template template parameters?
// - Statistics on parameter pack 'contents'.
// - Apply TPA to template template parameters.


void TemplateParameterAnalysis::extract(){

    // Should be defined already according to matcher reference, but doesn't
    // compile nor work in clang-query. File bug report soon.
    internal::VariadicDynCastAllOfMatcher<Decl, TemplateTemplateParmDecl>
    templateTemplateParmDecl;

    internal::VariadicDynCastAllOfMatcher<Decl, VarTemplateDecl> varTemplateDecl;

    auto tt = forEach(decl(anyOf(
        nonTypeTemplateParmDecl().bind("nontypes"),
        templateTypeParmDecl().bind("types"),
        templateTemplateParmDecl().bind("templates"))));
    auto CTParmMatcher = classTemplateDecl(
        isExpansionInMainFile(), tt);
    auto FTParmMatcher = functionTemplateDecl(
        isExpansionInMainFile(), tt);
    auto VTParmMatcher = varTemplateDecl(
        isExpansionInMainFile(), tt);
    auto ATParmMatcher = typeAliasTemplateDecl(
        isExpansionInMainFile(), tt);

    auto CTResults = Extractor.extract2(*Context,
        classTemplateDecl(isExpansionInMainFile())
    .bind("ct"));
    auto CTParmResults = Extractor.extract2(*Context, CTParmMatcher);
    ClassTemplates = getASTNodes<Decl>(CTResults, "ct");
    ClassTemplateNonTypeParameters = getASTNodes<Decl>(CTParmResults,
        "nontypes");
    ClassTemplateTypeParameters = getASTNodes<Decl>(CTParmResults,
        "types");
    ClassTemplateTemplateParameters = getASTNodes<Decl>(CTParmResults,
        "templates");

    auto FTResults = Extractor.extract2(*Context,
        functionTemplateDecl(isExpansionInMainFile())
    .bind("ft"));
    auto FTParmResults = Extractor.extract2(*Context, FTParmMatcher);
    FunctionTemplates = getASTNodes<Decl>(FTResults, "ft");
    FunctionTemplateNonTypeParameters = getASTNodes<Decl>(FTParmResults,
        "nontypes");
    FunctionTemplateTypeParameters = getASTNodes<Decl>(FTParmResults,
        "types");
    FunctionTemplateTemplateParameters = getASTNodes<Decl>(FTParmResults,
        "templates");

    auto VTResults = Extractor.extract2(*Context,
        varTemplateDecl(isExpansionInMainFile())
    .bind("vt"));
    auto VTParmResults = Extractor.extract2(*Context, VTParmMatcher);
    VariableTemplates = getASTNodes<Decl>(VTResults, "vt");
    VariableTemplateNonTypeParameters = getASTNodes<Decl>(VTParmResults,
        "nontypes");
    VariableTemplateTypeParameters = getASTNodes<Decl>(VTParmResults,
        "types");
    VariableTemplateTemplateParameters = getASTNodes<Decl>(VTParmResults,
        "templates");

    auto ATResults = Extractor.extract2(*Context,
        typeAliasTemplateDecl(isExpansionInMainFile())
    .bind("at"));
    auto ATParmResults = Extractor.extract2(*Context, ATParmMatcher);
    AliasTemplates = getASTNodes<Decl>(ATResults, "at");
    AliasTemplateNonTypeParameters = getASTNodes<Decl>(ATParmResults,
        "nontypes");
    AliasTemplateTypeParameters = getASTNodes<Decl>(ATParmResults,
        "types");
    AliasTemplateTemplateParameters = getASTNodes<Decl>(ATParmResults,
        "templates");
}
using DM = Matches<Decl>;
void printStats(std::string text, DM t, DM tnonty, DM tty, DM ttemp){
    printMatches(text, t);
    auto WithoutParamPack = 0;
    auto WithParamPack = 0;
    for(auto match : t){
        // Cast cannot fail, upcast from guaranteed some of class-,func-,var-,
        // alias-templatedecl
        auto Node = cast<TemplateDecl>(match.node);
        auto TemplateParametersPtr = Node->getTemplateParameters();
        if(TemplateParametersPtr->hasParameterPack()){
            WithParamPack++;
        } else {
            WithoutParamPack++;
        }
        // unsigned numTemplateParams = TemplateParametersPtr->size();
        // for(unsigned idx=0; idx<numTemplateParams; idx++)
        //     auto ParamDeclPtr = TemplateParametersPtr->getParam(idx);
    }
    std::cout << "#Non-type template parameters: " << tnonty.size() << std::endl;
    std::cout << "#Type template parameters: " << tty.size() << std::endl;
    std::cout << "#Template template parameters: "<< ttemp.size() << std::endl;
    std::cout << "Parameter packs used: " << WithParamPack << "/"
        << WithoutParamPack + WithParamPack << std::endl;
}
void TemplateParameterAnalysis::analyze(){
    printStats("Class Templates", ClassTemplates,
        ClassTemplateNonTypeParameters,
        ClassTemplateTypeParameters,
        ClassTemplateTemplateParameters);
    printStats("Function Templates", FunctionTemplates,
        FunctionTemplateNonTypeParameters,
        FunctionTemplateTypeParameters,
        FunctionTemplateTemplateParameters);
    printStats("Variable Templates", VariableTemplates,
        VariableTemplateNonTypeParameters,
        VariableTemplateTypeParameters,
        VariableTemplateTemplateParameters);
    printStats("Alias Templates", AliasTemplates,
        AliasTemplateNonTypeParameters,
        AliasTemplateTypeParameters,
        AliasTemplateTemplateParameters);
}

void TemplateParameterAnalysis::run(llvm::StringRef InFile,
    clang::ASTContext& Context){
        this->Context = &Context;
        std::cout << "\033[32mRunning template parameter analysis:\033[0m\n";
        extract();
        analyze();
}

//-----------------------------------------------------------------------------
