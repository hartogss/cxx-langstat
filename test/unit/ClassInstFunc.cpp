// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %S/../../build/cxx-langstat --analyses=tia --store %t1.ast --
// RUN: diff %t1.ast.json %s.json

// Instantiations: f<int>, f<bool>, E<short>, F<unsigned>, fK<unsigned>.
// Test to see how classes declarations or class instantiations cause function
// instantiations.

template<typename T>
int f();

// Class implicitly instantiations function through call
class C {
    void fc(){
        f<int>();
    }
};

// Class implicitly insts through call to create var member
class D {
    int a = f<bool>();
};

// Explicit class instantiation does not seem to instantiate members...no f<short>
template<typename T>
class E {
    int a = f<T>();
    static int a2;
};
template class E<short>;
// According to https://en.cppreference.com/w/cpp/language/class_template,
// a static data member of a class template should be explicitly instantiable,
// how?

// Implicit class inst does however
template<typename T>
class F {
    int a = f<T>();
};
F<unsigned> f_;
