// RUN: rm %t1.ast.json || true
// RUN: %clangxx %s -emit-ast -o %t1.ast
// RUN: %cxx-langstat --analyses=vta -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json


//-----------------------------------------------------------------------------

// Old way Nr. 1:
// Class template with static data (CTSD).
// 'data' should be static because everybody using this should see the same?
// Access specifier doesn't matter, similar reasoning to "template typedef" idiom.
// Could also be struct instead.
template<typename T>
class s1 {
public:
    static T data;
};
// Also CTSD. We report both static variables.
template<typename T>
class s2 {
    static T data;
    static T data_prime;
};
// Not CTSD, contains nonstatic data.
template<typename T>
class s3 {
    static T data;
    int d2;
};

template<>
int s1<int>::data = 42;
// template<>
// int s3<double>::data = 41.0;

//-----------------------------------------------------------------------------

// Old way Nr. 2:
// Constexpr function template (CFT)
template<typename T>
constexpr T f1(){
    T data = 3;
    return data;
}
// Not CFT as not constexpr
template<typename T>
T f2(){
    T data = 3;
    return data;
}
// Not CFT as not templated
constexpr int f3(){
    int data = 3;
    return data;
}
// CFT although it does not return the data it defines
template<typename T>
constexpr T f4(){
    T data = 3;
    return 0;
}
// CFT, even if no variable declaration, can just return literal of something else immediately
template<typename T>
constexpr T f5(){
    return 0;
}
// Not CFT, doesn't return
template<typename T>
constexpr T f6(){
    T data = 3;
}
// Not CFT, has function call
template<typename T>
constexpr T f7(){
    T data;
    f1<T>();
    return data;
}
// CFT, although returns something different than data (irrelevant that it's a call)
template<typename T>
constexpr T f8(){
    T data;
    return f1<T>();
}
// Not CFT, contains declStmt that contains typedef
template<typename T>
constexpr T f9(){
    T data;
    typedef T T_prime;
    return data;
}
// Debatable. Considered CFT for now.
template<typename T>
constexpr T f10(){
    T data;
    T data_prime;
    return data;
}

//-----------------------------------------------------------------------------

// Now: variable templates (since C++14)
template<typename T>
constexpr T pi = T(3.14); // T() is the cxxUnresolvedConstructExpr
// pi is a T constructed by T(3.14), where T() is a constructor call?

template<typename T>
T pi2;

class vtc {
    template<typename T>
    static constexpr T pi3 = T(3);
    // void method(){
    //     template<typename T>
    //     static constexpr T pi2 = T(3);
    // }
    // constexpr int t = 3;
};

int main(int argc, char** argv){
    // s<int> s1;
    // s1.data = 3;
    // s<int> s2;
    pi<int>;
    pi2<int>;
    // std::cout << pi<int> << std::endl;
    // std::cout << pi2<bool> << std::endl;
    // int x = 3;
    // std::cout << s<int>::data << std::endl;
    // std::cout << s<double>::data << std::endl;
    // std::cout << s1.data << std::endl;
    // std::cout << s2.data << std::endl;

}
