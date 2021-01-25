// RUN: rm %t1.ast.json || true
// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %S/../../../build/cxx-langstat --analyses=lka -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json
// Test to ensure LKA does correctly distinguish range-based for loops

int main(int argc, char** argv){
    int arr[] = {1,2,3};
    for(auto el : arr){
        // do something
    }

    for(int i=0; i<10; i++){
        // do something else
    }
}
