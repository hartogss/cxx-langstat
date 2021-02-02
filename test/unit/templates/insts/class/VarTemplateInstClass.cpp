// RUN: rm %t1.ast.json || true
// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %S/../../../../../build/cxx-langstat --analyses=tia -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json

// Test file to check if instantiations of variable templates "aliasing" class
// templates cause implicit class instantiations.
//

// Class template and an implicit instantiation thru a global var.
template<typename T>
class C{};

// Variable template
template<typename T>
C<T> c2;
template C<bool> c2<bool>;
void c(){
    c2<float>;
}

// class D {
//     template<typename T>
//     static C<T> d2;
// };
//
// C<int>* t = new C<int>;
// template<>
// C<int> D::d2<int> = *t;
