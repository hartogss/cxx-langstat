// RUN: rm %t1.ast.json || true
// RUN: %clangxx %s -emit-ast -o %t1.ast
// RUN: %cxx-langstat --analyses=cca -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json

// Basic CCA tests.
//
//

void f1(){
}

int f2(){
    if(true){

    }
    return 0;
}

int f3(){
    if(true){

    } else {
        return 0;
    }
    return 0;
}

int f4(){
    if(true){
        return 0;
    } else {
        return 0;
    }
    return 0;
}

void f5(){
    if(true){
        if(true){

        }
    }
}

void f6(){
    if(true){
        if(true){
            if(true){

            }
        }
    }
}

bool f7(){
    return (true || true); // not sure why this drives up CYC to 2
}

void f8(){
    if(true || false){ // CYC of 2 because of if branch, then goes to 3
    } // intuition: image it were 2 if statements following each other
}     // it it was && instead of || the 2 if statements would be nested
#include <cassert>
void f9(){
    assert(true && false); // CYC of 3 because of assert statement. CFG
    // is control flow of all statements in AST.
}
