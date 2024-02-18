#include <set>
#include <algorithm>
#include <cassert>
#include <boost/bind.hpp>
#include <boost/type_traits/remove_pointer.hpp>

template <class T>
struct ptr_cmp: public std::binary_function<T, T, bool> {
    bool operator()(const  T& v1, const  T& v2) const {
        return std::less<T>()(v1, v2);
    }
    template <class T1>
    bool operator()(const T1& v1, const T1& v2) const {
        return operator ()(*v1, *v2);
    }
};

template <class T>
struct ptr_deleter: public std::unary_function<T, void> {
    void operator()(T* ptr) {
        delete ptr;
    }
};

void example1() {
    std::set<int*, ptr_cmp<int> > s;
    s.insert(new int(1));
    s.insert(new int(0));

    assert(**s.begin() == 0);
    std::for_each(s.begin(), s.end(), ptr_deleter<int>()); // 遍历删除 如果抛异常 有可能内存泄漏
}

//void example2_a() {
//    typedef std::auto_ptr<int> int_aptr_t;
//    std::set<int_aptr_t, ptr_cmp<int> > s;
//    s.insert(int_aptr_t(new int(1)));
//    s.insert(int_aptr_t(new int(0)));
//    // ...
//    assert(**s.begin() == 0);
//    // resources will be deallocated by auto_ptr<>
//}

void example2_b() {
    typedef std::unique_ptr<int> int_uptr_t;
    std::set<int_uptr_t, ptr_cmp<int> > s;
    s.insert(int_uptr_t(new int(1)));
    s.insert(int_uptr_t(new int(0)));
    assert(**s.begin() == 0);
    // resources will be deallocated by unique_ptr<>
}

#include <boost/shared_ptr.hpp>
void example3() {
    typedef boost::shared_ptr<int> int_sptr_t;
    std::set<int_sptr_t, ptr_cmp<int> > s;
    s.insert(int_sptr_t(new int(1)));
    s.insert(int_sptr_t(new int(0)));
    assert(**s.begin() == 0);
    // resources will be deallocated by shared_ptr<>
}

#include <boost/ptr_container/ptr_set.hpp>
void correct_impl() {
    boost::ptr_set<int> s;
    s.insert(new int(1));
    s.insert(new int(0));
    assert(*s.begin() == 0);
    // resources will be deallocated by container itself
}

#include <boost/ptr_container/clone_allocator.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
void theres_more_example() {
    boost::ptr_vector<int> v;
    v.resize(10, new int(100));
    assert(v.size() == 10);
    assert(v.back() == 100);
}

int main() {
    example1();
//  example2_a();
    example2_b();
    example3();
    correct_impl();
    theres_more_example();
}

// 类似的还有很多ptr容器  ptr_vec ptr_array ptr_set ptr_multimap
// 都是容器里盛放 通用的ptr   即使发生了异常也会把所有ptr都释放掉 不会发生内存泄漏·

