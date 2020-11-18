// Typedef & alias
struct tuple_int {
    int x;
    int y;
};

typedef tuple_int pair_typedef; // typedef
using pair_alias = tuple_int; // alias
pair_typedef simpletest;

//-----------------------------------------------------------------------------

// "typedef templates" & alias templates
template<typename T>
struct tuple {
    T x;
    T y;
};

// typedef & alias
typedef tuple<int> intpair_typedef; //typedef
using intpair_alias = tuple<int>; // alias

// Best example of a "typedef template". typedef public by default
template<typename T>
struct Tpair_typedef {
    typedef tuple<T> type;
};

// Sure, why won't you have 2 typedefs in there?
template<typename T>
struct Tpair_typedef1 {
    typedef tuple<T> type;
    typedef tuple<T> type2;
};

// Not a "typedef template" since it contains no tyedef
template<typename T>
struct empty {
};

// Not a "typedef template" because it contains a variable declaration
template<typename T>
struct Tpair_typedef2 {
    typedef tuple<T> type;
    int x;
};

// Not a "typedef template" because it contains a method declaration
template<typename T>
struct Tpair_typedef3 {
    typedef tuple<T> type;
    void method();
};

// "typedef template" since access specifier is public
// Also a good example of a "typedef template".
template<typename T>
class Tpair_typedef4 {
public:
    typedef tuple<T> type;
};
Tpair_typedef4<int>::type test;

// "typedef template" since itself and derived classes can use it.
// Derived classes can use any way they like, itself can only use it to typedef
// other types, otherwise it isn't a "typedef" template anymore.
// Untypical example
template<typename T>
class Tpair_typedef5 {
protected:
    typedef tuple<T> type;
};
// Tpair_typedef5<int>::type test51; illegal, type is protected
template<typename T>
class der : public Tpair_typedef5<T> {
    Tpair_typedef5<int>::type type2;
};

// Considered a "typedef template", because can declare a second public typedef
// inside of class which can then be seen a normal "template typedef" free
// to be used by anyone.
// Untypical example
template<typename T>
class Tpair_typedef6 {
private:
    typedef tuple<T> type;
public:
    typedef Tpair_typedef6<T>::type type2;
};
// Tpair_typedef6<float>::type2 test61; // illegal, type2 is private
Tpair_typedef6<float>::type2 test62;

// alias template
template<typename T>
using Tpair_alias = tuple<T>;
Tpair_alias<int> anothertest;


int main(int argc, char** argv){

}
