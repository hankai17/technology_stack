#include "stdafx.h" 
#include<stdio.h> 
 
extern "C" { //如不用extern会出现连接错误，编译成了C++文件 
#include <lua.h> 
#include <lualib.h> 
#include <lauxlib.h> 
} 
 
int average(lua_State *L){ 
 int n= lua_gettop(L); 
 int i; 
 double sum=0; 
 for(i=0;i<n;i++){ 
  sum+=lua_tonumber(L,-1); 
  lua_pop(L,1); 
 } 
 lua_pushnumber(L,sum/n); 
 if(!lua_isnumber(L,-1)) 
  luaL_error(L,"push error!\n"); 
 return 1; 
} 
 
int _tmain(int argc, _TCHAR* argv[]) 
{ 
 lua_State* L = lua_open(); 
 luaL_openlibs(L); //新版本库添加的方法 
 
 lua_register(L,"avg",average); 
 luaL_dofile(L,"cof.lua"); 
 
 getchar(); 
 return 0; 
} 

// print("avg is ",avg(1,2,3,4,5,6,7,8))  
