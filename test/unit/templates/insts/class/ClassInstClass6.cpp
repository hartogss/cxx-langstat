// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %S/../../../../../build/cxx-langstat --analyses=tia -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json

// Instantiations: Sub<int> as FieldDecl as part of Super ClassTemplateDecl,
// SubSub<int> as FieldDecl as part of Sub ClassTemplateDecl,
// SubSub<int> as FieldDecl as part of Sub<int> ClassTemplateSpecializationDecl

template<typename T>
class SubSub {
    int nix(T par){
        return 0;
    }
};

template<typename T>
class Sub {
    SubSub<int> w;
    void nix();
};

template<typename T>
class Super {
    Sub<int> w;
};

// Note we count instantiation of Sub and SubSub even though Super is never
// instantiated.
