#include <stdio.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

int main(int argc, char *argv[]){
    lua_State *L = luaL_newstate(); // 创建一个状态机

    lua_pushnil(L);                 // nil
    int type = lua_type(L, -1);
    printf("nil type = %d\n", type);
    if (lua_isnil(L, -1)) {
        printf("type is nil\n");
    }

    lua_pushboolean(L, 0);          // boolean
    type = lua_type(L, -1);
    printf("boolean type: %d\n", type);
    if (lua_isboolean(L, -1)) {
        printf("type is boolean\n");
    }

    lua_pushlightuserdata(L, NULL); // lightuserdata
    type = lua_type(L, -1);
    printf("lightuserdata type: %d\n", type);
    if (lua_islightuserdata(L, -1)) {
        printf("type is lightuserdata\n");
    }

    lua_pushnumber(L, 10);          // number
    type = lua_type(L, -1);
    printf("number type: %d\n", type);
    if (lua_isnumber(L, -1)) {
        printf("type is number\n");
    }

    lua_pushstring(L, "string");    // string
    type = lua_type(L, -1);
    printf("string type: %d\n", type);
    if (lua_isstring(L, -1)) {
        printf("type is string\n");
    }

    lua_newtable(L);                // table 创建空表 并压入栈
    type = lua_type(L, -1);
    printf("table type: %d\n", type);
    if (lua_istable(L, -1)) {
        printf("type is table\n");
    }

    lua_newuserdata(L, 1024);       // userdata 分配1024大小的内存块 并把内存地址压入栈
    type = lua_type(L, -1);
    printf("userdata type = %d\n", type);
    if (lua_isuserdata(L, -1)) {
        printf("type is userdata\n");
    }

    lua_pushthread(L);              // thread 创建一个lua新线程 并将其压入栈 lua线程不是OS线程
    type = lua_type(L, -1);
    printf("thread type = %d\n", type);
    if(lua_isthread(L, -1)) {
        printf("type is thread\n");
    }

    lua_close(L);                   // 关闭状态机
    return 0;
}

/*
入栈/出栈都围绕栈顶进行
=================== 栈顶 ===================   
4               -1
3               -2
2               -3
1               -4
=================== 栈底 ===================   
*/

// g++ 1hello.cpp /usr/local/lib/libluajit-5.1.a -I/usr/include/lua-5.1 -lm -ldl

