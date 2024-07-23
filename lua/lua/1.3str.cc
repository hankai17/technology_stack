//str = "I am so cool"  
//tbl = {name = "shun", id = 20114442}  
//function add(a,b)  
//    return a + b  
//end
//我们写一个test.cpp来读取它：

#include <iostream>  
#include <string.h>  
using namespace std;  
   
extern "C"  
{  
    #include "lua.h"  
    #include "lauxlib.h"  
    #include "lualib.h"  
}  
void main()  
{  
    //1.创建Lua状态  
    lua_State *L = luaL_newstate();  
    if (L == NULL)  
    {  
        return ;  
    }  
   
    //2.加载Lua文件  
    int bRet = luaL_loadfile(L,"hello.lua");  
    if(bRet)  
    {  
        cout<<"load file error"<<endl;  
        return ;  
    }  
   
    //3.运行Lua文件  
    bRet = lua_pcall(L,0,0,0);  
    if(bRet)  
    {  
        cout<<"pcall error"<<endl;  
        return ;  
    }  
   
    //4.读取变量  
    lua_getglobal(L,"str");  
    string str = lua_tostring(L,-1);  
    cout<<"str = "<<str.c_str()<<endl;        //str = I am so cool~  
   
    //5.读取table  
    lua_getglobal(L,"tbl");   
    lua_getfield(L,-1,"name");  
    str = lua_tostring(L,-1);  
    cout<<"tbl:name = "<<str.c_str()<<endl; //tbl:name = shun  
   
    //6.读取函数  
    lua_getglobal(L, "add");        // 获取函数，压入栈中  
    lua_pushnumber(L, 10);          // 压入第一个参数  
    lua_pushnumber(L, 20);          // 压入第二个参数  
    int iRet= lua_pcall(L, 2, 1, 0);// 调用函数，调用完成以后，会将返回值压入栈中 同时两个参数将会出栈，2表示参数个数，1表示返回结果个数。  
    if (iRet)                       // 调用出错  
    {  
        const char *pErrorMsg = lua_tostring(L, -1);  
        cout << pErrorMsg << endl;  
        lua_close(L);  
        return ;  
    }  
    if (lua_isnumber(L, -1))        //取值输出  
    {  
        double fValue = lua_tonumber(L, -1);  
        cout << "Result is " << fValue << endl;  
    }  
   
    //至此，栈中的情况是：  
    //=================== 栈顶 ===================   
    //  索引  类型      值  
    //   4   int：      30   
    //   3   string：   shun   
    //   2   table:     tbl  
    //   1   string:    I am so cool~  
    //=================== 栈底 ===================   

	// 将需要设置的值设置到栈中  
	lua_pushstring(L, "我是一个大帅锅～");  
	// 将这个值设置到table中（此时tbl在栈的位置为2）  
	lua_setfield(L, 2, "name"); // "我是一个大帅锅～" 将自动出栈 // 栈情况与上面一样


	// 创建一个新的table，并压入栈  
	lua_newtable(L);  
	// 往table中设置值  
	lua_pushstring(L, "Give me a girl friend !"); //将值压入栈  
    //=================== 栈顶 ===================   
    //  索引  类型      值  
    //   6(-1) string   "Give me a girl friend !"
	//   5(-2) table    newtable
    //   4   int：      30   
    //   3   string：   shun   
    //   2   table:     tbl  
    //   1   string:    I am so cool~  
    //=================== 栈底 ===================   
	lua_setfield(L, -2, "str"); //将值设置到table中，并将Give me a girl friend 出栈
	// 需要注意的是：堆栈操作是基于栈顶的，就是说它只会去操作栈顶的值。
    //=================== 栈顶 ===================   
    //  索引  类型      值  
	//   5(-2) table    newtable
    //   4   int：      30   
    //   3   string：   shun   
    //   2   table:     tbl  
    //   1   string:    I am so cool~  
    //=================== 栈底 ===================   

   
    //7.关闭state  
    lua_close(L);  
    return ;  
}

/*
int   lua_gettop (lua_State *L);            //返回栈顶索引（即栈长度）  
void  lua_settop (lua_State *L, int idx);   //                
void  lua_pushvalue (lua_State *L, int idx);//将idx索引上的值的副本压入栈顶  
void  lua_remove (lua_State *L, int idx);   //移除idx索引上的值  
void  lua_insert (lua_State *L, int idx);   //弹出栈顶元素，并插入索引idx位置  
void  lua_replace (lua_State *L, int idx);  //弹出栈顶元素，并替换索引idx位置的值
*/

