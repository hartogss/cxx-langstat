#include <iostream>

#include "llvm/Support/Commandline.h"

#include "cxx-langstat/Options.h"

#include <dirent.h>



// Global variables
// Options in CLI specific to cxx-langstat
llvm::cl::OptionCategory CXXLangstatCategory("cxx-langstat options", "");
// llvm::cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);
// llvm::cl::extrahelp MoreHelp("\nMore help text coming soon...\n");
// CL options

// # probably going to retire soon
// Accepts comma-separated string of analyses
llvm::cl::opt<std::string> AnalysesOption(
    "analyses",
    llvm::cl::desc("Comma-separated list of analyses"),
    llvm::cl::cat(CXXLangstatCategory));

// 2 flags:
// --emit-features: analysis stops after writing features to file after reading in .ast
// --emit-statistics: read in JSON with features and compute statistics
// no flag at all: compute features but don't write them to disk, compute
// statistics and emit them
llvm::cl::opt<Stage> PipelineStage(
    llvm::cl::desc("Stage: "),
    llvm::cl::values(
        clEnumValN(emit_features, "emit-features",
            "Stop after emitting features.\n"
            "If output files are specified, \n "
            "on output for every input file \n"
            "has to be specified. If no output \n"
            "is specified, output files will \n"
            "will be put at the working directory"),
        clEnumValN(emit_statistics, "emit-statistics", "Read in .json files "
            "each containing features extracted from an AST and compute "
            "statistics. Requires either no output files or only a single one")),
    llvm::cl::cat(CXXLangstatCategory));

llvm::cl::opt<std::string> InputDirOption(
    "indir",
    llvm::cl::desc("<ast0> [... <astN>]"),
    llvm::cl::Required,
    llvm::cl::cat(CXXLangstatCategory));

// what to do with this? some -p option already there by default, but parser fails on it
static llvm::cl::opt<std::string> BuildPath("p", llvm::cl::desc("Build path, but really"),
   llvm::cl::Optional, llvm::cl::cat(CXXLangstatCategory));

bool hasSuitableExtension(llvm::StringRef s){
    return s.equals(".cpp") || s.equals(".cc") || s.equals(".cxx") || s.equals(".C")
        || s.equals(".hpp") || s.equals(".hh") || s.equals(".hxx") || s.equals(".H")
        || s.equals(".c++") || s.equals(".h++")
        || s.equals(".c") || s.equals(".h") // C file formats
        || s.equals(".ast"); // AST file
}


std::vector<std::string> getFiles(std::string s){
    // http://www.martinbroadhurst.com/list-the-files-in-a-directory-in-c.html
    // Trust me, would prefer to use <filesystem> too.
    DIR* dirp = opendir(s.c_str());
    if(dirp){
        // std::cout << "dir: " << s.c_str() << std::endl;
        struct dirent* dp;
        std::vector<std::string> files;
        std::vector<std::string> res;
        while ((dp = readdir(dirp)) != NULL) {
            files.emplace_back(dp->d_name);
        }
        closedir(dirp);
        std::vector<std::string> dirfiles;
        for(auto file : files){
            if(!llvm::StringRef(file).consume_front(".")){ //ignore hidden files & dirs
                if(hasSuitableExtension(llvm::sys::path::extension(file))){
                    res.emplace_back(file);
                } else {
                    if(!llvm::sys::path::filename(file).equals(".") && !llvm::sys::path::filename(file).equals(".."))
                        dirfiles.emplace_back(s+"/"+file);
                }
            }
        }
        for(auto dirfile : dirfiles) {
            auto files = getFiles(dirfile);
            for(auto file : files)
                res.emplace_back(file);
        }
        return res;
    }
    return {};
}


//
int main(int argc, char** argv){
    llvm::cl::ParseCommandLineOptions(argc, argv, "cxx-langstat is a clang-based"
        "tool for computing statistics on C/C++ code on the clang AST level");

    auto files = getFiles(InputDirOption);
    for(auto file : files){
        std::cout << file << std::endl;
    }






    return 0;
}
