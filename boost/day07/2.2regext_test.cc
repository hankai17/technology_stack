#include <boost/regex.hpp>
#include <iostream>

int main() {
    boost::regex::flag_type flag;
    flag = boost::regex::perl; //boost::regex::perl|boost::regex::icase;
    flag |= boost::regex::no_except;

    //std::string regex = "(?i:\\b(?:(?:m(?:s(?:ys(?:ac(?:cess(?:objects|storage|xml)|es)|(?:relationship|object|querie)s|modules2?)|db)|aster\\.\\.sysdatabases|ysql\\.db)|pg_(?:catalog|toast)|information_schema|northwind|tempdb)\\b|s(?:(?:ys(?:\\.database_name|aux)|qlite(?:_temp)?_master)\\b|chema(?:_name\\b|\\W*\\())|d(?:atabas|b_nam)e\\W*\\())";
    //std::string regex = "(?i:\\b(?:(?:m(?:s(?:ys(?:ac(?:cess(?:objects|storage|xml)|es)|(?:relationship|object|querie)s|modules2?)|db)|aster\\.\\.sysdatabases|ysql\\.db)|pg_(?:catalog|toast)|information_schema|northwind|tempdb)\\b|s(?:(?:ys(?:\\.database_name|aux)|qlite(?:_temp)?_master)\\b|chema(?:_name\\b|\\W*\\())|(database|db_name)\(\)))";
    std::string regex = "\\b(?:and|or|\\|\\||&&)(?:/\\*|\\s|\\r|\\n|\\*/|\\(?:|'|\"|\\x0b|\\a0)+(?:.*)[/\\*|\\s|\\r|\\n|\\*/|.|\\(?:|'|\"]*=";
    //std::string regex = "\\b(?:and|or)(?:/\\*|\\s|\\*/|\\(?:|')+(?:.*)";
    //std::string regex =   "\\b(?:or)(?:/\\*|\\s|\\*/|\\(?:|')+(?:.*)";

    //std::string str = "UID:2000136919\r\n任务ID：d57a489f-23af-414e-94a5-512b1843d701\r\n报错信息：  {\"ErrType\":\"SourceError\",\"ErrMessage\":\"show Create Table Error: thread is 0 table name is lfrms2_promotion_online.pro_md_program the error is database(or table) Mapping error\"}\r\n麻烦帮忙看下这个表名不匹配问题是为什么，以及如何处理规避下，是否需要客户配合修改源库\"";
    std::string str = "ready h5sdk error: {\"errorCode\":333448,\"errorMessage\":\"not in safe domain or path: https://oa.m.ksyun.com/sp***************************x.html?_rdm=1691740825081\",\"errno\":2601002,\"errString\":\"Authentication failed. not in safe domain or path: https://oa.m.ksyun.com/sp***************************x.html?_rdm=1691740825081 (error code: 20448)\"} https://oa.m.ksyun.com/spa/workflow/static4form/index.html?_rdm=1691740825081#/main/workflow/req?em_auth_userid=8347&resourcetype=0&resourceid=425467&requestid=425467&em_client_lang=zh&from=single_chat_link&em_auth_code=440facee-2d70-4c5f-96f9-dc8c9641a16e&isFormMsg=1&em_client_type=pc&outsysid=b114f9ce3e4946b8a6503d1f83658a28&_key=9t25db";

    
    const boost::regex e(regex, flag);
    if (e.status()) {
        std::cout << "Incorrect regex pattern!\n";
        return 0;
    }
    
    boost::smatch results;
    bool matched = boost::regex_search(str, results, e);
    std::cout << "regex: " << regex << std::endl;
    //std::cout << (matched ? "MATCH\n" : "DOES NOT MATCH\n");
    if (matched) {
        //std::copy(
        //    results.begin(), 
        //    results.end(), 
        //    std::ostream_iterator<std::string>( std::cout, ", ")
        //);

        std::cout << "size: " << results.size() << std::endl;
        for (size_t i = 0; i < results.size(); i++) {
            std::cout << results[i] << std::endl;
        }
    }
                
    return 0;
}

//g++ 2.2regext_test.cc -L/root/file/boost_1_72_0/stage/lib -lboost_regex 
