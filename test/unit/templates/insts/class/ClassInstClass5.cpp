// RUN: rm %t1.ast.json || true
// RUN: %clangxx %s -emit-ast -o %t1.ast
// RUN: %cxx-langstat --analyses=tia -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json
// Instantiations: One Sub<int> FieldDecl that is subtree of the Super Class-
// TemplateDecl.


template<typename T>
class Sub {};

template<typename T>
class Super {
    Sub<int> w;
};
