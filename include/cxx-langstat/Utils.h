#ifndef UTILS_H
#define UTILS_H

//-----------------------------------------------------------------------------

std::string getDeclName(Match<clang::Decl> Match);

void printStatistics(std::string text, Matches<clang::Decl> matches);

//-----------------------------------------------------------------------------

#endif // UTILS_H
