#ifndef TASK_PROCESSOR_MULTITHREAD_HPP
#define TASK_PROCESSOR_MULTITHREAD_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "../1.2tasks_processor_network/tasks_processor_network.hpp"

namespace tp_multithread {
    class tasks_processor: public tp_network::tasks_processor {
    public:
        static tasks_processor& get();
    
        void start_multiple(std::size_t threads_count = 0) {
            if (!threads_count) {
                threads_count = (std::max)(static_cast<int>(boost::thread::hardware_concurrency()), 1);
            }
            --threads_count; // one thread is the current thread
            boost::thread_group tg;
            for (std::size_t i = 0; i < threads_count; ++i) {
                tg.create_thread(boost::bind(&boost::asio::io_service::run, boost::ref(ios_)));
            }
            ios_.run();
            tg.join_all();
        }
    };
}

#endif

