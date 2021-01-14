// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %S/../../../../../build/cxx-langstat --analyses=tia --store %t1.ast --
// RUN: diff %t1.ast.json %s.json

// Instantiations: C::fc<int>, f<bool>, D<bool>.
// Question: Do we want to report the instantiation D<bool>::fd ?
//


// Here the class doesn't actually instantiate, we instantiate the method
// explicitly.
class C {
public:
    template<typename T>
    void fc(){}
};
void callerc(){
    C c;
    c.fc<int>();
}
template void C::fc<bool>();


// Non-templated methods of class templates, however, are "instantiated" when
// the class template is instantiated.
// I've decided not to report those, because they don't have any template
// parameters themselves, and thus really nothing interesting to report.
template<typename T>
class D {
    void fd(){}
};
template class D<bool>;


// Same as ClassInstFunc: explicit instantiation of class does not instantiate
// member (in this case a member function)
template<typename T>
class E {
public:
    template<typename U>
    void fe(){}
    template<typename R>
    void fe2(T t, R r){}
};
template class E<bool>;
template void E<bool>::fe<bool>(); // here is an explicit one
template void E<int>::fe2<short>(int, short); // om het af te leren
// Note how the method instantiation does not cause an instantiation of the
// encompassing class.
void callere(){
    E<int> e;
    e.fe2(3,3); // causes ::fe2<int> to be deduced
}
