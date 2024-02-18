#ifndef TASK_PROCESSOR_BASE_HPP
#define TASK_PROCESSOR_BASE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/thread/thread.hpp>
#include <boost/asio/io_service.hpp>

namespace detail {
    template <class T>
    struct task_wrapped {
    private:
        T task_unwrapped_; // 类型擦除器
    
    public:
        explicit task_wrapped(const T& task_unwrapped)
            : task_unwrapped_(task_unwrapped)
        {}
    
        void operator()() const { // 对外暴露operator()
            try {
                boost::this_thread::interruption_point();
            } catch(const boost::thread_interrupted&){}
    
            try {
                task_unwrapped_();  // 模板类必须提供operator()
            } catch (const std::exception& e) {
                std::cerr<< "Exception: " << e.what() << '\n';
            } catch (const boost::thread_interrupted&) {
                std::cerr<< "Thread interrupted\n";
            } catch (...) {
                std::cerr<< "Unknown exception\n";
            }
        }
    };
    
    template <class T>
    inline task_wrapped<T> make_task_wrapped(const T& task_unwrapped) {
        return task_wrapped<T>(task_unwrapped);
    }
}

namespace tp_base {
    class tasks_processor: private boost::noncopyable {
    protected:
        boost::asio::io_service         ios_;
        boost::asio::io_service::work   work_;
    
        tasks_processor()
            : ios_()
            , work_(ios_)
        {}
    
    public:
        static tasks_processor& get();
    
        template <class T>
        inline void push_task(const T& task_unwrapped) {
            ios_.post(detail::make_task_wrapped(task_unwrapped)); // asio/detail/impl/strand_service.hpp:strand_service::post
        }
    
        void start() {
            ios_.run();
        }
    
        void stop() {
            ios_.stop();
        }
    };
}

#endif

