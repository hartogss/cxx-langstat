// rm Output/nlt.ast.json || true
// rm Output/empty.ast.json || true
// RUN: clang++ %s -emit-ast -o Output/nlt.ast
// RUN: clang++ empty.cpp -emit-ast -o Output/empty.ast
// RUN: %S/../../../../build/cxx-langstat --analyses=ala,cca,cla,lda,lka,msa,tia,tpa,ua,ula,vta -emit-features -in Output/nlt.ast -in Output/empty.ast -outdir Output/ --
// RUN: diff Output/empty.ast.json empty.cpp.json

// Test to test that when input are multiple files, that features from first
// file don't accidentally transfer to features of second file. Analyses retain
// their state until they run on a new file. This used to go south
// with LDA, MSA because those analyses didn't correctly overwrite old state.

#include <vector>
#include <utility>
#include <algorithm>


template<int N> // tpa
class C {};
template class C<0>; // tia

int main(int argc, char** argv){ // cca, msa
    for(;;){ // lda, lka

    }
    typedef int newint; // ua
    std::vector<int> v; // cla
    std::pair<int, int> twoints; // ula
    int a = std::max(twoints.first, twoints.second); // ala

}

template<int N> // vta
int n = N;
