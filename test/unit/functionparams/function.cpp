// RUN: rm %t1.ast.json || true
// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %S/../../../build/cxx-langstat --analyses=msa -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json
// Test to see if correctly for each kind of function (non-template) declaration
// parameter kind the right functions are found and all parmeters are listed
// correctly.

class C{};

void f0(); //

void f1(C c); // a C
void f1(C c1, C c2);
void f2(C& c); // lvalue-ref to a C
void f3(const C& c); // lvalue-ref to a const C
void f4(C&& c); // rvalue-ref to a C
void f4_2(C&& c, C&& c2){
    void f5(C&& c);
};


void frp1(C* c); // pointer to C
void frp2(C*& c); // lvalue-ref to pointer to C
void frp3(const C*& c); // lvalue-ref to pointer to const C
void frp3_1(C* const & c); // lvalue-ref to constant pointer to C
void frp3_2(const C* const & c); // lvalue-ref to constant pointer to constant C
void frp4(C*&& c);


void fp1(C* c);
void fp2(const C* c); // pointer to a const C
void fp3(C* const c); // const pointer to a C
void fp4(const C* const c); // const pointer to a const C
