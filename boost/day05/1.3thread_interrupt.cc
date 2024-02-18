#include <boost/thread.hpp>

bool stop_parsing = true;
bool not_end_of_parsing = true;

void do_parse() {
    while (not_end_of_parsing) {
        boost::this_thread::interruption_point();
    }
    assert(false);
}

int main() {
    boost::thread parser_thread(&do_parse);
    if (stop_parsing) {
        parser_thread.interrupt(); // 通知线程 线程在point时退出
    }

    parser_thread.join();
    return 0;
}

