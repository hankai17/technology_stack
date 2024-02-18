/*************************************************************************
  > File Name: 1hello.cpp
  > Author: Bill
  > Mail: hankai17@126.com 
  > Created Time: 2017-01-03 13:48:46
 ************************************************************************/

#include<iostream>
using namespace std;

#include <iostream>  
#include <string.h>  
using namespace std;  

extern "C"  
{  
#include "lua.h"  
#include "lauxlib.h"  
#include "lualib.h"  
}  
int main()  
{  
    lua_State *L = luaL_newstate();             // 创建Lua状态  
    if (L == NULL) {  
        return 0;  
    }  

    int bRet = luaL_loadfile(L, "1hello.lua");  // 加载Lua文件  
    //int bRet = luaL_loadfile(L,"domain-white-list");  
    //int bRet = luaL_loadfile(L,"domain-white-list");  
    if (bRet) {
        cout<<"load file error"<<endl;  
        return 0;  
    }  

    bRet = lua_pcall(L,0,0,0);                  // 运行Lua文件  
    if(bRet) {
        cout<<"pcall error"<<endl;  
        return 0;  
    }  

    // 读取table  
    lua_getglobal(L, "ser");                    // 把全局变量ser压入堆栈   

//       栈顶 
//   -1      table
//       栈底
    lua_getfield(L, -1, "name");                // 把table中的'name'压栈到-1处 此后-2是table -1是当前取的值
    string str = lua_tostring(L, -1);           // 取name的第一个值
    cout << "ser:name "<< str.c_str() << endl;   

//       栈顶 
//   -1      name
//   -2      table
//       栈底
    lua_getfield(L, -2, "name1");               // 把table中的'name1'压栈到-1处
    str = lua_tostring(L, -1);
    cout<< "ser:name1 "<< str.c_str() <<endl;  

//       栈顶 
//   -1      name1
//   -2      name
//   -3      table
//       栈底
    lua_getfield(L, -3, "name2");               // 把table中的'name1'压栈到-1处
    str = lua_tostring(L, -1);
    cout <<"ser:name2 "<< str.c_str() <<endl;  

    lua_close(L);  
    return 0;  
}

// g++ 1hello.cpp /usr/local/lib/libluajit-5.1.a -I/usr/include/lua-5.1 -lm -ldl

//lua_getfield 即 取lua虚拟栈中指定位置的表中的指定字段的值，并放入lua虚拟栈中。
//lua_setfield 即 取出lua虚拟栈中的栈顶作为value,参数为key构成表字段放置到虚拟栈的指定位置的表中。

