// RUN: rm %t1.ast.json || true
// RUN: %clangxx %s -emit-ast -o %t1.ast
// RUN: %cxx-langstat --analyses=tia -emit-features -in %t1.ast -out %t1.ast.json --
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
