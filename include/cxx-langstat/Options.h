#ifndef OPTIONS_H
#define OPTIONS_H

// CL options

// Options in CLI specific/nongeneric to clang-stat
llvm::cl::OptionCategory ClangStatCategory("clang-stat options");
llvm::cl::extrahelp CommonHelp(clang::tooling::CommonOptionsParser::HelpMessage);
llvm::cl::extrahelp MoreHelp("\nMore help text coming soon...\n");

#endif // OPTIONS_H
