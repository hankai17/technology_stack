#include <boost/static_assert.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_float.hpp>

template <class T, class Enable = void>
class data_processor {
public:
    double process(const T&, const T&, const T&) {
        BOOST_STATIC_ASSERT((boost::is_same<const char*, T>::value));
        return 0.0;
    }
};

template <class T>
class data_processor<T, typename boost::enable_if_c<boost::is_integral<T>::value >::type> { // 只有当enable_if成立则才向下编译 防止编译器变傻
public:
    typedef int fast_int_t;
    double process(fast_int_t, fast_int_t, fast_int_t) {
        BOOST_STATIC_ASSERT((boost::is_same<int, T>::value || 
                             boost::is_same<short, T>::value));
        return 0.0;
    }
};

template <class T>
class data_processor<T, typename boost::enable_if_c<boost::is_float<T>::value >::type> {
public:
    double process(double, double, double) {
        BOOST_STATIC_ASSERT((boost::is_same<double, T>::value || 
                             boost::is_same<float, T>::value));
        return 0.0;
    }
};

template <class T>
double example_func(T v1, T v2, T v3) {
    data_processor<T> proc;
    return proc.process(v1, v2, v3);
}

int main () {
    example_func(1, 2, 3);
    short s = 0;
    example_func(s, s, s);
    example_func(1.0, 2.0, 3.0);
    example_func(1.0f, 2.0f, 3.0f);
    example_func("Hello", "word", "processing");
}

