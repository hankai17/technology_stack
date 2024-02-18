#include <cassert>
#include <cstddef>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/recursive_mutex.hpp>

// In previous recipe we included
// <boost/thread.hpp>, which includes all
// the classes of Boost.Thread
#include <boost/thread/thread.hpp>

namespace without_sync {
    int shared_i = 0;
    void do_inc() {
        for (std::size_t i = 0; i < 30000; ++i) {
            int i_snapshot = ++shared_i;
            (void) i_snapshot;
        }
    }
    void do_dec() {
        for (std::size_t i = 0; i < 30000; ++i) {
            int i_snapshot = --shared_i;
            (void) i_snapshot;
        }
    }
    void run() {
        boost::thread t1(&do_inc);
        boost::thread t2(&do_dec);
        t1.join();
        t2.join();
        //assert(shared_i == 0);
        std::cout << "shared_i == " << shared_i;
    }
}

namespace with_sync {
    int shared_i = 0;
    boost::mutex i_mutex;
    void do_inc() {
        for (std::size_t i = 0; i < 30000; ++i) {
            int i_snapshot;
            {
                boost::lock_guard<boost::mutex> lock(i_mutex);
                i_snapshot = ++shared_i;
            }
            (void)i_snapshot;
        }
    }
    void do_dec() {
        for (std::size_t i = 0; i < 30000; ++i) {
            int i_snapshot;
            {
                boost::lock_guard<boost::mutex> lock(i_mutex);
                i_snapshot = --shared_i;
            }
            (void) i_snapshot;
        }
    }
    void run() {
        boost::thread t1(&do_inc);
        boost::thread t2(&do_dec);
        t1.join();
        t2.join();
        assert(shared_i == 0);
        std::cout << "shared_i == " << shared_i;
    }
}


int main() {
    without_sync::run();
    with_sync::run();
    return 0;
}

