#include <assert.h>
#include <deque>
#include <boost/function.hpp>

typedef boost::function<void(int)> fobject_t;

void process_integers(const fobject_t& f) {
    static const int data[] = {1, 2, 3, 4, 5, 200, 0};
    std::for_each(data, data + 6, f);
}

int main() {
    process_integers([](int){});

    std::deque<int> deq;
    process_integers([&deq](int i){
        deq.push_back(i);
    });

    std::size_t match_count = 0;
    process_integers([deq, &match_count](int i) mutable {
        if (deq.front() == i) {
           ++match_count;
        }
        deq.pop_front();
    });

    assert(match_count == 6);
}

