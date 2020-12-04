// #include<tuple>
// #include<optional>

#include <array>
#include <vector>
#include <forward_list>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <stack>
#include <deque>

#include <tuple>

template<typename T>
class Widget {
    // std::vector<char> v; // works too
};

int main(int argc, char** argv){
    // Never a problem to get instantiations of custom templates in main file
    Widget<double> w;

    // #################### Standard library containers ####################
    // Assuming isExpansionInMainFile() is not used:
    // √
    std::array<int, 1> arr;
    // √
    std::vector<int> vec;
    // √
    std::forward_list<int> flist;
    // √
    std::list<int> list;
    // Causes 2 extra pair instances
    std::map<int, int> map;
    // Causes 2 extra pair instances
    std::multimap<int, int> mmap;
    // √
    std::set<int> set;
    // √
    std::multiset<int> mset;
    // Causes 2 extra pair instances
    std::unordered_map<int, int> umap;
    // Causes 2 extra pair instances
    std::unordered_multimap<int, int> ummap;
    // √
    std::unordered_set<int> uset;
    // √
    std::unordered_multiset<int> umset;
    // Causes additional deque instantiation, allocator residue
    std::queue<int> q;
    // √
    std::priority_queue<int> pq;
    // Causes additional deque instantiation
    std::stack<int> s;
    //  √
    std::deque<int> d;


    // #################### Standard library utilities ####################
    //
    // √
    std::pair<int, int> p;
    // √
    std::tuple<int,int, int> t;

    // Dynamic memory management
    // Causes 3 extra found instances
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
