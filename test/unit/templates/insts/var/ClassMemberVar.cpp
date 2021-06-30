// RUN: rm %t1.ast.json || true
// RUN: %clangxx %s -emit-ast -o %t1.ast
// RUN: %cxx-langstat --analyses=tia -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json
// The second & last occurence of variable templates: static class member vars.
//
//

// Basic test with static class member variable template
// class Widget {
// public:
//     template<typename T>
//     static T data; // declaration
// };
// // Implicit instantiation works fine
// void caller(){
//     Widget::data<int> = 3;
// }

// Basic test with static class member variable template
class Widget2 {
public:
    template<typename T>
    static T data; // declaration
};
template<typename T>
T Widget2::data = 3; // definition

template float Widget2::data<float>; // instantiation


// Basic test with static class member variable template
// class Widget3 {
// public:
//     template<typename T>
//     constexpr static T data=3; // declaration
// };
// template<typename T>
// T Widget2::data = 3; // definition

// template const float Widget3::data<float>; // instantiation










// template<typename T>
// T Widget::data = 3; // definition
