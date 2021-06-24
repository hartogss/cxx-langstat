// RUN: rm %t1.ast.json || true
// RUN: %clangxx %s -emit-ast -o %t1.ast
// RUN: %cxx-langstat --analyses=ua -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json

// Small test to see if typedefs and aliases are reported in both function
// templates and in an instantiation thereof.
//

template<typename T>
void func(){
    typedef T type;
    using type2 = T;
}

template void func<int>();
