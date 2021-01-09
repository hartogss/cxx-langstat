// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %S/../../build/cxx-langstat --analyses=tia --store %t1.ast --
// RUN: diff %t1.ast.json %s.json


// #include<optional>

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

template<typename T>
class Widget {
    // std::vector<char> v; // works too
};

template<typename T>
T f(){
    return {1,2};
}

template<typename T>
std::vector<T> Tvec;

int main(int argc, char** argv){
    // Never a problem to get instantiations of custom templates in main file
    // Widget<double> w;

    // #################### Standard library containers ####################
    // Assuming isExpansionInMainFile() is not used:
    // √
    std::array<int, 1> arr;
    std::array<int, 1> arr2;
    // √
    std::vector<int> vec;
    // √
    std::forward_list<int> flist;
    // √
    std::list<int> list;
    // Causes extra pair<const T, U>
    std::map<int, int> map;
    // Causes extra pair<const T, U>
    std::multimap<int, int> mmap;
    // √
    std::set<int> set;
    // √
    std::multiset<int> mset;
    // Causes extra pair<const T, U>, unique_ptr<__hash_node_base, __bucket_list_deallocator>
    std::unordered_map<int, int> umap;
    // Causes extra pair<const T, U>, unique_ptr<__hash_node_base, __bucket_list_deallocator>
    std::unordered_multimap<int, int> ummap;
    // Causes unique_ptr<__hash_node_base, __bucket_list_deallocator>
    std::unordered_set<int> uset;
    // Causes unique_ptr<__hash_node_base, __bucket_list_deallocator>
    std::unordered_multiset<int> umset;
    // Causes extra deque<T, allocator>
    std::queue<int> q;
    // Causes extra vector<U>
    std::priority_queue<int> pq;
    // Causes extra deque<T, allocator>
    std::stack<int> s;
    //  √
    std::deque<int> d;

    // Test with std container used in function template and variable template
    f<std::vector<double>>();
    f<std::vector<int>>();
    f<std::vector<int>>();
    Tvec<float>;
    Tvec<int>;
    Tvec<int>;

    Widget<int> w1;


    // #################### Standard library utilities ####################
    //
    // √
    std::pair<int, int> p;
    std::pair<int, int> p2; // should cause 2nd JSON entry
    // √
    std::tuple<int,int, int> t;
    std::tuple<char, int> t1;

    // Dynamic memory management
    // √
    std::unique_ptr<int> uptr;
    // √
    std::shared_ptr<int> sptr;
    // √
    std::weak_ptr<int> wptr;


    // //
    // std::pair<Widget<float>, Widget<float>> opt; // nicely, find CTSD of name 'pair'
    // std::vector<Widget<float>> vec2; // somewhat nicely, alias somewhere I think
    // std::unique_ptr<Widget<float>> ptr; // somewhat nice if we ignore the deleter
    // // but has extra match with void type
    // std::array<int, 4> arr2; // perfect
}

template std::vector<int> Tvec<int>;
