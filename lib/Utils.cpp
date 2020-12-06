#include <iostream>

#include "llvm/ADT/StringRef.h"

//-----------------------------------------------------------------------------

std::string getFileForStatDump(llvm::StringRef InFile) {
    // Other file extensions possible (.cc, .cxx), but discouraged and possibly
    // not ever supported, since we work on AST level anyway
    if(InFile.consume_back(llvm::StringRef(".ast"))){}
    else if(InFile.consume_back(llvm::StringRef(".cpp"))){}
    else {
        std::cerr << "Invalid file extension!\n";
    }
    return InFile.str() += ".json";
}

//-----------------------------------------------------------------------------
