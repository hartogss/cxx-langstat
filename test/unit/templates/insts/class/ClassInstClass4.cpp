// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %S/../../../../../build/cxx-langstat --analyses=tia --out Output/ %t1.ast --
// RUN: diff %t1.ast.json %s.json

// Instantiations: One Super<int> as ClassTemplateSpecializationDecl (explicit)
// one Super<int> as VarDecl that has that CTSD type and again one Sub<int>
// as FieldDecl as part of the CTSD.

template<typename T>
class Sub {};

template<typename T>
class Super {
    Sub<T> w;
};

template class Super<int>;
Super<int> s;
