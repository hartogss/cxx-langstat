// RUN: rm %t1.ast.json || true
// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %cxx-langstat --analyses=vta -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json

// Test to have some extra precaution with classes inside of class templates
// used for the first pre-C++14 variable templates idiom.
//

// CTSD.
template<typename T>
class A {
    static T data;


};

// Not a CTSD.
template<typename T>
class B {
    static T data;

    class C {

    };
};

class C {

};

// -CXXRecordDecl 0x7f8a1905aa10 <line:28:1, line:30:1> line:28:7 class C definition
//   |-DefinitionData pass_in_registers empty aggregate standard_layout trivially_copyable pod trivial literal has_constexpr_non_copy_move_ctor can_const_default_init
//   | |-DefaultConstructor exists trivial constexpr needs_implicit defaulted_is_constexpr
//   | |-CopyConstructor simple trivial has_const_param needs_implicit implicit_has_const_param
//   | |-MoveConstructor exists simple trivial needs_implicit
//   | |-CopyAssignment simple trivial has_const_param needs_implicit implicit_has_const_param
//   | |-MoveAssignment exists simple trivial needs_implicit
//   | `-Destructor simple irrelevant trivial needs_implicit
//   `-CXXRecordDecl 0x7f8a1905ab30 <col:1, col:7> col:7 implicit class C

// We see that C contains another cxxRecordDecl, so allow a class only to be
// CTSD when it does not contain cxxRecordDecl that are not implicit.
