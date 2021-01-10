// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %S/../../build/cxx-langstat --analyses=tia --store %t1.ast --
// RUN: diff %t1.ast.json %s.json

// Instantiations: C::fc<int>, f<bool>, D<bool>. Do we want to count the instantiation
// D::fd<bool> ?
//


// Here the class doesn't actually instantiate, we instantiate the method
// explicitly.
class C {
public:
    template<typename T>
    void fc(){
    }
};
void callerc(){
    C c;
    c.fc<int>();
}
template void C::fc<bool>();


// Non-templated methods of class templates, however, seem to be "instantiated" when
// the class template is explicity instantiated.
// Do we want to report this member function instantiation? it really only
// depends on class template args, not function templates'.
template<typename T>
class D {
    // D(){

    // }
    void fd(){
    }
};
// template class D<bool>;
// template void D<int>::fd();

// Same as ClassInstFunc: explicit instantiation of class does not instantiate
// member function
template<typename T>
class E {
    template<typename U>
    void fe(){
    }
    template<typename R>
    void fe2(T t, R r){

    }
};
template class E<bool>;
template void E<bool>::fe<bool>(); // here is an explicit one
template void E<int>::fe<bool>(); // another one
template void E<int>::fe2<short>(int, short); // om het af te leren
// Note how the method instantiation does not cause an instantiation of the
// encompassing class.
