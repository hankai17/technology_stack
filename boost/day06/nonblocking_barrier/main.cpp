#include <cstddef>
#include <boost/array.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/barrier.hpp>
#include <boost/atomic.hpp>
#include "./tasks_processor_base/tasks_processor_base.hpp"

static const std::size_t data_length = 10000;

struct vector_type : 
    public boost::array<std::size_t, data_length> {
    void* alignment;
};

typedef boost::array<vector_type, 4> data_t;

void fill_data(vector_type& data) {
    for (std::size_t i = 0; i < vector_type::static_size; ++ i) {
        data[i] = i;
        data.alignment = 0;
    }
}

void compute_send_data(data_t& data) {
    for (std::size_t i = 0; i < vector_type::static_size; ++i) {
        for (std::size_t j = 0; j < data_t::static_size; ++j) {
            assert(data[j][i] == i);
            data[0][i] *= data[j][i];
        }
    }
}

void runner(std::size_t thread_index, boost::barrier& data_barrier, data_t& data) {
    for (std::size_t i = 0; i < 1000; ++ i) {
        fill_data(data.at(thread_index));
        data_barrier.wait();

        if (!thread_index) {
            compute_send_data(data);
        }
        data_barrier.wait();
    }
}

void clever_implementation() {
    atomic_count_t counter(0);
    data_t data;

    tasks_processor& tp = tasks_processor::get();
    for (std::size_t i = 0; i < data_t::static_size; ++i) {
        tp.push_task(
            boost::bind(&clever_runner, i, 0, boost::ref(counter), boost::ref(data))
        );
    }
    tp.start();
}

int main() {
    data_t data;
    boost::barrier data_barrier(data_t::static_size);

    boost::thread_group tg;
    for (std::size_t i = 0; i < data_t::static_size; ++i) {
        tg.create_thread(
            boost::bind(&runner, i, boost::ref(data_barrier), boost::ref(data))
        );
    }
    tg.join_all();
    clever_implementation();
}

using namespace tp_base;

// Part of tasks_processor class from
// tasks_processor_base.hpp, that must be defined
// Somewhere in source file
tasks_processor& tasks_processor::get() {
    static tasks_processor proc;
    return proc;
}

typedef boost::atomic<unsigned int> atomic_count_t;

void clever_runner(
        std::size_t thread_index,
        std::size_t iteration,
        atomic_count_t& counter,
        data_t& data)
{
    fill_data(data.at(thread_index));

    if (++counter == data_t::static_size) {
        compute_send_data(data);

        ++ iteration;
        if (iteration == 1000) {
            // exiting, because 1000 iterations are done
            tasks_processor::get().stop();
            return;
        }

        counter = 0;
        for (std::size_t i = 0; i < data_t::static_size; ++ i) {
            tasks_processor::get().push_task(boost::bind(
                clever_runner, 
                i,
                iteration, 
                boost::ref(counter), 
                boost::ref(data)
            ));
        }
    }
}

