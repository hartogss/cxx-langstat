#include <iostream>

#include "llvm/ADT/StringRef.h"

//-----------------------------------------------------------------------------

std::string getFileForStatDump(llvm::StringRef InFile) {
    return InFile.str() += ".json";
}

//-----------------------------------------------------------------------------
