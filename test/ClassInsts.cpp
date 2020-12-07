// Class template 1
template<typename T>
class Widget1 {

};
// Explicit specialization
// Not what we want to match here
template<>
class Widget1<int> {
    int x;
};
// Explicit instantiation
template class Widget1<float>;
// template class Widget1<float>; // not possible
// Implicit instantiations
// As VarDecl
Widget1<double> w1;
Widget1<double> w1_1; // second entry. Should be.
Widget1<Widget1<bool>> w2; // subinstantiation?
// As ParmVarDecls, which is also a VarDecl, so no extra work needed
void f(Widget1<char> w3){
}
// As FieldDecl
class Helper {
    Widget1<void> wf;
};

class Helper2 {
    Widget1<void> wf;
};

// Class template 2
template<typename ...Ts>
class Widget1_p {};
// Explicit
template class Widget1_p<int, int>;
template class Widget1_p<Widget1<double>>;
template class Widget1_p<int, double, float, unsigned, Widget1<int>>;
// Implicit
Widget1_p<double, Widget1<int>> w1p;

template<int N, int ...Ms>
class Widget2_p{};
template class Widget2_p<3,1,4>;

constexpr int call(){
    return 3;
}
template class Widget2_p<call()>;
// template class Widget2_p<NULL>;
