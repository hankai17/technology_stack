#include <iostream>
#include <boost/noncopyable.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/tss.hpp>

class connection: boost::noncopyable {
public:
    connection(): open_count_(0) {
        std::cout << "construct..." << std::endl;
    }

    void open() {
        assert(!open_count_);
        open_count_ = 1;
    }
    void send_result(int result) {}

    int open_count_;
};

boost::thread_specific_ptr<connection> connection_ptr;

connection& get_connection() {
    connection* p = connection_ptr.get();
    if (!p) {
        connection_ptr.reset(new connection); // thread local概念 每个线程有一个
        p = connection_ptr.get();
        p->open();
    }
    return *p;
}

void task() {
    int result = 2;
    get_connection().send_result(result);
}

void run_tasks() {
    for (std::size_t i = 0; i < 10000000; ++i) {
        task();
    }
}

int main() {
    boost::thread t1(&run_tasks);
    boost::thread t2(&run_tasks);
    boost::thread t3(&run_tasks);
    boost::thread t4(&run_tasks);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    return 0;
}

// g++ main.cpp -lboost_thread
