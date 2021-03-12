// RUN: rm %t1.ast.json || true
// RUN: clang++ %s -emit-ast -o %t1.ast -std=c++14
// RUN: %cxx-langstat --analyses=msa -emit-features -in %t1.ast -out %t1.ast.json -- -std=c++14
// RUN: diff %t1.ast.json %s.json

// Are passing field like c.d or temporary like c.f() reported correctly?
//
//

class D {
public:
    ~D();
};

class C{
public:
    D d;
    D f(){
        return D();
    }
};

void funcc(C c);

void funcd(D d);

int main(int argc, char** argv){
    C c;
    funcc(c);
    funcd(c.d);
    funcd(c.f()); // Copied since we're using C++14 here

    // C c2 = C();
}
