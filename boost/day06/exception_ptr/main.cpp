#include <iostream>
#include <stdexcept>
#include <boost/exception_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include "../tasks_processor_base/tasks_processor_base.hpp"
using namespace tp_base;

tasks_processor& tasks_processor::get() {
    static tasks_processor proc;
    return proc;
}

void func_test2() {
    try {
        BOOST_THROW_EXCEPTION(std::logic_error("Some fatal logic error"));
    } catch (...) {
        tasks_processor::get().push_task(
            boost::bind(&process_exception, boost::current_exception())
        );
    }
}

void process_exception(const boost::exception_ptr& exc) {
    try {
        boost::rethrow_exception(exc);
    } catch (const boost::bad_lexical_cast&) {
        std::cout << "Lexical cast exception detected\n" << std::endl;
        tasks_processor::get().push_task(&func_test2);
    } catch (...) {
        std::cout << "Can not handle such exceptions:\n" 
            << boost::current_exception_diagnostic_information() 
            << std::endl;
        tasks_processor::get().stop();
    }
}

void func_test1() {
    try {
        boost::lexical_cast<int>("oops!");
    } catch (...) {
        tasks_processor::get().push_task(
            boost::bind(&process_exception, boost::current_exception())
        );
    }
}

void run_throw(boost::exception_ptr& ptr) {
    try {
    } catch (...) {
        ptr = boost::current_exception();
    }
}

int main () {
    tasks_processor::get().push_task(&func_test1);
    tasks_processor::get().start();

    boost::exception_ptr ptr;
    boost::thread t(boost::bind(&run_throw, boost::ref(ptr)));
    t.join();

    if (ptr) {
        boost::rethrow_exception(ptr);
    }
    return 0;
}

