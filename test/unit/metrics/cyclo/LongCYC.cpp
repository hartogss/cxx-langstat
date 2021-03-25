// RUN: rm %t1.ast.json || true
// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %cxx-langstat --analyses=cca -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json

// Test of CCA for a bit of a longer example. This is actually the exact control
// flow of the Runner.cpp::main function from cxx-langstat.
// Notice how all the assert statements also drive up the CYC, which I didn't
// notice at first.



#include <vector>
#include <cassert>
#include <string>

int main(int argc, char** argv){
    // simulate values for options
    std::vector<std::string> InputFiles;
    std::vector<std::string> OutputFiles;
    if(true) {
        for(;;){
            for(;;){}
        }
    } else {
    }
    if(true && true){
        assert(false && "Don't specify input files and directory "
            "at the same time\n");
    }
    if(true && true){}
    if(true && true){}
    if(true){
    } else {
    }

    if(true){
        if(true && true){
            if(true)
                assert(false && "Don't specify an output dir for a single output\n");
            if(true){
            } else {
            }
        } else if((true && true) || true) {
                if(true)
                    assert(false && "Don't specify an output file when multiple outputs are expected\n");
                if(true){ // obliged to specify output dir
                    assert(false && "Please specify an output dir\n");
                } else {
                    for(;;){
                    }
                }
            }
        assert(InputFiles.size() == OutputFiles.size());
    } else if(true){
        if(true)
            assert(false && "Don't specify an output dir for a single output\n");
        if(true){}
        if(true){}
        assert(OutputFiles.size() == 1);
    }
    for(;;){
        if(true){
        }
    }
    if(true){
    } else if(true){
    }
}
