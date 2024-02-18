#include <iostream>
#include <regex>
#include <string>
#include <fstream>

using namespace std;

std::string get_regex_from_file(std::string filename)
{
    std::fstream fin(filename);
    std::string line;
    getline(fin, line);
    return line;
}

void test()
{
    //std::string str = "master";
    //regex reg("m(?:s(?:ys|db)|aster)");
    std::string str = "UID:2000136919\n任务ID：d57a489f-23af-414e-94a5-512b1843d701\n报错信息：\t{\"ErrType\":\"SourceError\",\"ErrMessage\":\"show Create Table Error: thread is 0 table name is lfrms2_promotion_online.pro_md_program the error is database(or table) database()Mapping error\"}\n麻烦帮忙看下这个表名不匹配问题是为什么，以及如何处理规避下，是否需要客户配合修改源库";
    regex reg("(?i:\b(?:(?:m(?:s(?:ys(?:ac(?:cess(?:objects|storage|xml)|es)|(?:relationship|object|querie)s|modules2?)|db)|aster\.\.sysdatabases|ysql\.db)|pg_(?:catalog|toast)|information_schema|northwind|tempdb)\b|s(?:(?:ys(?:\.database_name|aux)|qlite(?:_temp)?_master)\b|chema(?:_name\b|\W*\())|d(?:atabas|b_nam)e\W*\())");
    smatch smat;
    if (regex_match(str, smat, reg))
    {
        cout << "匹配：" << smat.str() << endl;
        cout << "字符串长度：" << smat.length() << endl;
        cout << "子匹配个数：" << smat.size() << endl;
        cout << endl;
    }
}

int main()
{
    test();
    return 0;
    std::fstream fin("raw.txt");
    std::string line;
    //regex reg("(?i:\b(?:(?:m(?:s(?:ys(?:ac(?:cess(?:objects|storage|xml)|es)|(?:relationship|object|querie)s|modules2?)|db)|aster\.\.sysdatabases|ysql\.db)|pg_(?:catalog|toast)|information_schema|northwind|tempdb)\b|s(?:(?:ys(?:\.database_name|aux)|qlite(?:_temp)?_master)\b|chema(?:_name\b|\W*\())|d(?:atabas|b_nam)e\W*\())");
    std::string re = get_regex_from_file("reg.txt");
    cout << re << std::endl;
    regex reg(re);

    while (getline(fin, line)) {
        smatch smat;
        if (regex_match(line, smat, reg))
        {
            cout << "匹配：" << smat.str() << endl;
            cout << "字符串长度：" << smat.length() << endl;
            cout << "子匹配个数：" << smat.size() << endl;
            cout << endl;
        }
    }


    return 0;
}
