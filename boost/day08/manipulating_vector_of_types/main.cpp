#include <boost/mpl/size.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/static_assert.hpp>
#include <boost/mpl/if.hpp>
#include <boost/type_traits/make_unsigned.hpp>
#include <boost/type_traits/add_const.hpp>
#include <boost/mpl/transform.hpp>

struct unsigne; // No typo: `unsigned` is a keyword, we can not use it.
struct no_change;
struct constant;

template <class Types, class Modifiers>
struct do_modifications {
    BOOST_STATIC_ASSERT((boost::is_same<            // 确保模板参数类型是一致的
        typename boost::mpl::size<Types>::type, 
        typename boost::mpl::size<Modifiers>::type 
    >::value));

    BOOST_STATIC_ASSERT((
        boost::mpl::size<Types>::type::value == boost::mpl::size<Modifiers>::type::value    // 确保模板参数默认值是一致的
    ));

    typedef boost::mpl::if_<
        boost::is_same<boost::mpl::_2, unsigne>,
        boost::make_unsigned<boost::mpl::_1>,
        boost::mpl::if_<
            boost::is_same<boost::mpl::_2, constant>,
            boost::add_const<boost::mpl::_1>,
            boost::mpl::_1
        >
    > binary_operator_t;

    // >::type binary_operator_t; // INCORRECT!

    typedef typename boost::mpl::transform<
        Types,
        Modifiers,
        binary_operator_t
    >::type type;
};

/*
void boost_mpl_transform_pseoudocode() {
    vector result;
    for (std::size_t i = 0; i < Types.size(); ++i) {
        result.push_back(
            binary_operator_t(Types[i], Modifiers[i])
        );
    }
    return result;
}
*/

#include <boost/mpl/vector.hpp>
#include <boost/mpl/at.hpp>

typedef boost::mpl::vector<int,     char,       short,      long>       types;
typedef boost::mpl::vector<unsigne, no_change,  constant,   unsigne>    modifiers;
typedef do_modifications<types, modifiers>::type result_type;

BOOST_STATIC_ASSERT((boost::is_same<boost::mpl::at_c<result_type, 0>::type, unsigned int >::value));
BOOST_STATIC_ASSERT((boost::is_same<boost::mpl::at_c<result_type, 1>::type, char>::value));
BOOST_STATIC_ASSERT((boost::is_same<boost::mpl::at_c<result_type, 2>::type, const short>::value));
BOOST_STATIC_ASSERT((boost::is_same<boost::mpl::at_c<result_type, 3>::type, unsigned long>::value));

// GCC4.6 has problems with variadic templates:
// # sorry, unimplemented: cannot expand ‘T ...’ into a fixed-length argument list
// NOTE: Clang compiler defines __GNUC__
#if defined(__clang__) || !(defined(__GNUC__) && (__GNUC__ == 4) && (__GNUC_MINOR__ < 7))

#include <boost/config.hpp>
#ifndef BOOST_NO_CXX11_VARIADIC_TEMPLATES

template <class... T>
struct vt_example {
    typedef typename boost::mpl::vector<T...> type;
};
BOOST_STATIC_ASSERT((boost::is_same<boost::mpl::at_c<vt_example<int, char, short>::type, 0>::type, int >::value));

#endif

#endif // gcc4.6 workaround

#include <iostream>
#include <typeinfo>
int main() {
    std::cout << typeid(boost::mpl::size<types>::type).name() << '\n';
    return 0;
}

