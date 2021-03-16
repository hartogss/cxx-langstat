// RUN: rm %t1.ast.json || true
// RUN: clang++ %s -emit-ast -o %t1.ast -std=c++17
// RUN: %cxx-langstat --analyses=msa -emit-features -in %t1.ast -out %t1.ast.json -- -std=c++17
// RUN: diff %t1.ast.json %s.json

// Small test to ensure that with forEachArgumentWithParam matcher we analyze
// all parm-arg pair and all relevant arguments' construction is output.

class C{
};

void func(C c, C& c_r, C c2);

int main(int argc, char** argv){
    C c;
    func(c, c, c);
    C d = c;
}
