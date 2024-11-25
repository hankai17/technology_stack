#include <boost/regex.hpp>
#include <iostream>
#include <sys/time.h>

int global_hit = 0;

int main() {
    boost::regex::flag_type flag;
    flag = boost::regex::perl; //boost::regex::perl|boost::regex::icase;
    flag |= boost::regex::no_except;

    //std::string regex = "hello|world";
    std::string regex = "world";

    std::string str = "world";
    
    const boost::regex e(regex, flag);
    if (e.status()) {
        std::cout << "Incorrect regex pattern!\n";
        return 0;
    }
    
    boost::smatch results;

    struct  timeval   tv_begin,tv_end;
    gettimeofday(&tv_begin,NULL);

    int i = 0;
    while (i < 1000000 * 10) {
        bool matched = boost::regex_search(str, results, e);
        if (matched) {
            global_hit++;
        }
        i++;
    }

    gettimeofday(&tv_end,NULL);
    float time_use = (tv_end.tv_sec-tv_begin.tv_sec)*1000000+(tv_end.tv_usec-tv_begin.tv_usec);
    printf("time use: %fn us\n", time_use);
                
    printf("global_hit: %d\n", global_hit);
    return 0;
}

//g++ 2.2regext_test.cc -L/root/file/boost_1_72_0/stage/lib -lboost_regex 
