// RUN: rm %t1.ast.json || true
// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %cxx-langstat --analyses=cla,ula -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json
// Test to see if evert variable using standard library class types is
// correctly seen by SLA2(which is build on top of TIA).
//

// Comment indicates instantiations caused by library, which we don't want to
// see in our JSON report.
// isExpansionInMainFile() use avoids those.
#include <array> // unique_ptr<void>, pair<ulong, ulong>
#include <vector> // unique_ptr<void>, pair<ulong, ulong>
#include <forward_list> // unique_ptr<void>, pair<ulong, ulong>
#include <list> // unique_ptr<void>, pair<ulong, ulong>
#include <map> // unique_ptr<void>, pair<ulong, ulong>
#include <set> // unique_ptr<void>, pair<ulong, ulong>
#include <unordered_map> // unique_ptr<void>, pair<ulong, ulong>
#include <unordered_set> // unique_ptr<void>, pair<ulong, ulong>
#include <queue> // unique_ptr<void>, pair<ulong, ulong>
#include <stack> // unique_ptr<void>, pair<ulong, ulong>
#include <deque> // unique_ptr<void>, pair<ulong, ulong>

#include <tuple> // pair<ulong, ulong>

#include <memory> // unique_ptr<void>, pair<ulong, ulong>

// #include<optional>

int main(int argc, char** argv){
    // #################### Standard library containers ####################
    std::array<int, 1> arr;
    std::array<int, 1> arr2; // should cause 2nd JSON entry
    std::vector<int> vec;
    std::forward_list<int> flist;
    std::list<int> list;
    std::map<int, int> map;
    std::multimap<int, int> mmap;
    std::set<int> set;
    std::multiset<int> mset;
    std::unordered_map<int, int> umap;
    std::unordered_multimap<int, int> ummap;
    std::unordered_set<int> uset;
    std::unordered_multiset<int> umset;
    std::queue<int> q;
    std::priority_queue<int> pq;
    std::stack<int> s;
    std::deque<int> d;

    // #################### Standard library utilities ####################
    std::pair<int, int> p;
    std::pair<int, int> p2; // should cause 2nd JSON entry
    std::tuple<int,int, int> t;
    std::tuple<char, int> t1;

    // Dynamic memory management
    std::unique_ptr<int> uptr;
    std::shared_ptr<int> sptr;
    std::weak_ptr<int> wptr;
}

class C {
    std::vector<int> eins;
    std::vector<double> zwei;
protected:
    std::unique_ptr<C> cptr;
};
