//1
struct tuple {
    int x;
    int y;
};

typedef tuple pair_typedef; // typedef
using pair_alias = tuple; // alias


//2
template<typename T>
struct tuple2 {
    T x;
    T y;
};

// typedef & alias
typedef tuple2<int> intpair_typedef; //typedef
using intpair_alias = tuple2<int>; // alias

// "typedef template"
template<typename T>
struct Tpair_typedef {
    typedef tuple2<T> type;
};
// not templatized typedefs
// since either contain other non-typedef field or no typedef field at all
template<typename T>
struct Tpair_typedef2 {
    int x;
    typedef tuple2<T> type;
};
template<typename T>
struct empty {

};
template<typename T>
struct Tpair_typedef3 {
    void method();
    typedef tuple2<T> type;
};
// alias template
template<typename T>
using Tpair_alias = tuple2<T>; // alias template



int main(int argc, char** argv){

}
