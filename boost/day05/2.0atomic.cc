#include <cassert>
#include <cstddef>
#include <boost/thread/thread.hpp>
#include <boost/atomic.hpp>
#include <boost/static_assert.hpp>

boost::atomic<int> shared_i(0);

void do_inc() {
    for (std::size_t i = 0; i < 30000; ++i) {
        const int i_snapshot = ++shared_i;
        (void) i_snapshot;
    }
}

void do_dec() {
    for (std::size_t i = 0; i < 30000; ++i) {
        const int i_snapshot = --shared_i;
        (void) i_snapshot;
    }
}


int main() {
    boost::thread t1(&do_inc);
    boost::thread t2(&do_dec);

    t1.join();
    t2.join();

    assert(shared_i == 0);
    assert(shared_i.is_lock_free());
    return 0;
}

BOOST_STATIC_ASSERT(BOOST_ATOMIC_INT_LOCK_FREE == 2);

