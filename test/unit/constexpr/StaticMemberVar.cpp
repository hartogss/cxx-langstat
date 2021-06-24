// RUN: rm %t1.ast.json || true
// RUN: %clangxx %s -emit-ast -o %t1.ast
// RUN: %cxx-langstat --analyses=cea -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json

// Should report one variable that is constexpr, namely "yes".
//
//

int main(int argc, char** argv){

}

struct C {
    // Not reported, as not initialized.
    static int no;
    // Reported as not constexpr (good), but twice: once here, and once below
    // at the definition. Do we want this?
    static int no2;
    // Not reported, as not init'ed.
    static const int no3;
    // Corectly reported as constexpr
    static constexpr int yes = 1;
};

int C::no2 = 2;
