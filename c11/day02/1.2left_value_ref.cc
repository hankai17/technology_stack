f(10);  //右值

int x = 10;
f(x); //左值

template<typename T>
void f(T&& param);
/*
para的类型是未定的 这取决于是怎样初始化的 所以para是一个未定类型的引用
注意只有在自动类型推到时 即template auto时  &&才是未定引用
*/

/*
右值引用折叠规则如下：
右值引用叠加到右值引用仍是右值引用
所有的其他引用叠加都变成左值引用
*/

int w1, w2;
auto&& v1 = w1;
decltype(w1)&& v2 = w2;

/*
v1是未定引用 被一个左值初始化所以为左值
v2是右值引用 被一个左值初始化是错误的 改为decltype(w1)&& v2 = std::move(w2);
*/
