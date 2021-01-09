// File with bunch of code snippets that somewhen got coded, but aren't in use
// right now. Should go without saying that this is nowhere near compilable.

//-----------------------------------------------------------------------------
// Code for finding if an rvalue ref (syntactically with &&) is a forwarding/
// universal ref,
// copied from clang/lib/Sema/SemaTemplateDeduction.cpp

// https://clang.llvm.org/doxygen/SemaTemplateDeduction_8cpp_source.html#l01182
/// Get the index of the first template parameter that was originally from the
/// innermost template-parameter-list. This is 0 except when we concatenate
/// the template parameter lists of a class template and a constructor template
/// when forming an implicit deduction guide.
static unsigned getFirstInnerIndex(FunctionTemplateDecl *FTD) {
  auto *Guide = dyn_cast<CXXDeductionGuideDecl>(FTD->getTemplatedDecl());
  if (!Guide || !Guide->isImplicit())
 return 0;
  return Guide->getDeducedTemplate()->getTemplateParameters()->size();
}
// https://clang.llvm.org/doxygen/SemaTemplateDeduction_8cpp_source.html#l01190
// For our purposes FirstInnerIndex = 0 is fine.
static bool isForwardingReference(QualType Param, unsigned FirstInnerIndex) {
    // C++1z [temp.deduct.call]p3:
    //   A forwarding reference is an rvalue reference to a cv-unqualified
    //   template parameter that does not represent a template parameter of a
    //   class template.
    if (auto *ParamRef = Param->getAs<RValueReferenceType>()) {
        if (ParamRef->getPointeeType().getQualifiers())
            return false;
        auto *TypeParm = ParamRef->getPointeeType()->getAs<TemplateTypeParmType>();
        return TypeParm && TypeParm->getIndex() >= FirstInnerIndex;
   }
   return false;
 }

//-----------------------------------------------------------------------------
// Old code for trying to figure out if universal reference or not
for(auto match : FunctionTemplatesDeclsWithrValueRef){
    std::cout << match.node->getType().getAsString() << std::endl;
    std::cout << match.node->getType().getTypePtr()->getTypeClassName() << std::endl;
}
DeclarationMatcher d = functionTemplateDecl(isExpansionInMainFile(),
    has(templateTypeParmDecl().bind("typename")),
    has(functionDecl(hasDescendant(
        parmVarDecl().bind("parm")
))));
auto h = Extractor.extract2(*Context, d);
auto typeparms = getASTNodes<TemplateTypeParmDecl>(h, "typename");
auto params = getASTNodes<ParmVarDecl>(h, "parm");
printMatches("", typeparms);
printMatches("", params);
for (auto match : params){
    // getOriginalType gives QualType, is a ParmVarDecl public method
    auto t = match.node->getOriginalType();
    std::cout << t.getAsString() << std::endl;
    std::cout << t.hasQualifiers() << std::endl;
    std::cout << t.getTypePtr()->isRValueReferenceType() << std::endl;
    std::cout << isForwardingReference(t, 0) << std::endl;
    std::cout << "----" << std::endl;
}

//-----------------------------------------------------------------------------
// Code to test printing JSON objects without knowing index ids
std::cout << Result["rvalue ref parms"]["c"][0].dump(4) << std::endl;
// how to avoid indexing using decl name?
std::cout << Result["rvalue ref parms"].dump(4) << std::endl;
// can also index using iterators:
for(auto entriesForASingleVarName : Result["rvalue ref parms"]){
    std::cout << varName << std::endl;
    // for(auto loc : varName){
        // std::cout << varName << "@ " << loc << std::endl;
    // }
}

//-----------------------------------------------------------------------------
// Interesting type information one can get from nodes
std::cout << Node->getType().getTypePtr()->isDependentType() << std::endl;
std::cout << Node->getType().getTypePtr()->hasIntegerRepresentation() << std::endl;
std::cout << Node->getType().getTypePtr()->isInstantiationDependentType() << std::endl;
std::cout << Node->getType().getCanonicalType().getAsString() << std::endl;
std::cout << Node->getType().getTypePtr()->getCanonicalTypeInternal().getAsString() << std::endl; // gives the same as line above, but without qualifiers since stripped away

//-----------------------------------------------------------------------------
//
const char* StmtKind = m.node->getDeclKindName();

// Type matcher, does not yet work with my extractor framework because Location
// cannot be extracted
TypeMatcher stdtypes = type().bind("t");
auto t = Extr.extract("t", stdtypes);
for(auto match : t){
    std::cout << match.node->getAsString()
        << " @ " << match.location << std::endl;
}

//-----------------------------------------------------------------------------
