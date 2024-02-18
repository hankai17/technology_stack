#include <boost/regex.hpp>
#include <iostream>

int main() {
    boost::regex::flag_type flag;
    flag = boost::regex::perl; //boost::regex::perl|boost::regex::icase;
    flag |= boost::regex::no_except;

    //std::string regex = "(?i:\\b(?:(?:m(?:s(?:ys(?:ac(?:cess(?:objects|storage|xml)|es)|(?:relationship|object|querie)s|modules2?)|db)|aster\\.\\.sysdatabases|ysql\\.db)|pg_(?:catalog|toast)|information_schema|northwind|tempdb)\\b|s(?:(?:ys(?:\\.database_name|aux)|qlite(?:_temp)?_master)\\b|chema(?:_name\\b|\\W*\\())|d(?:atabas|b_nam)e\\W*\\())";

    std::string regex = "(?i:\\b(?:(?:m(?:s(?:ys(?:ac(?:cess(?:objects|storage|xml)|es)|(?:relationship|object|querie)s|modules2?)|db)|aster\\.\\.sysdatabases|ysql\\.db)|pg_(?:catalog|toast)|information_schema|northwind|tempdb)\\b|s(?:(?:ys(?:\\.database_name|aux)|qlite(?:_temp)?_master)\\b|chema(?:_name\\b|\\W*\\())|(database|db_name)\(\)))";

    std::string str = "UID:2000136919\r\n任务ID：d57a489f-23af-414e-94a5-512b1843d701\r\n报错信息：  {\"ErrType\":\"SourceError\",\"ErrMessage\":\"show Create Table Error: thread is 0 table name is lfrms2_promotion_online.pro_md_program the error is database(or table) Mapping error\"}\r\n麻烦帮忙看下这个表名不匹配问题是为什么，以及如何处理规避下，是否需要客户配合修改源库\"";
    
    const boost::regex e(regex, flag);
    if (e.status()) {
        std::cout << "Incorrect regex pattern!\n";
        return 0;
    }
    
    boost::smatch results;
    bool matched = boost::regex_search(str, results, e);
    std::cout << (matched ? "MATCH\n" : "DOES NOT MATCH\n");
    if (matched) {
        //std::copy(
        //    results.begin(), 
        //    results.end(), 
        //    std::ostream_iterator<std::string>( std::cout, ", ")
        //);
        for (size_t i = 0; i < results.size(); i++) {
            std::cout << results[i] << std::endl;
        }
    }
                
    return 0;
}

//g++ 2.2regext_test.cc -L/root/file/boost_1_72_0/stage/lib -lboost_regex 

