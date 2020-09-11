#include <lua.h>
#include <stdlib.h>
#include <string.h>
#include <lauxlib.h>
#include <lualib.h>
#include <math.h>
static char lua_light_user_data_key;

int main()
{
	lua_State  *L = luaL_newstate();
	lua_State *co;
	int base;

	luaL_openlibs(L);
	lua_pushlightuserdata(L, &lua_light_user_data_key);
	lua_createtable(L, 0, 4);
	
	lua_pushstring(L, "hello");
	lua_setfield(L, -2, "world");

	lua_pushstring(L, "apple");
	lua_rawseti(L, -2, 1);

	lua_pushstring(L, "name");
	lua_rawseti(L, -2, 2);
	
	lua_pushstring(L, "age");
	lua_rawseti(L, -2, 3);

//	lua_pushinteger(L, 2);
//	lua_rawseti(L, -2, 2);
	lua_rawset(L, LUA_REGISTRYINDEX);

	base = lua_gettop(L);
	
	lua_pushlightuserdata(L, &lua_light_user_data_key);
	lua_rawget(L, LUA_REGISTRYINDEX);
	co = lua_newthread(L);
	int ref = luaL_ref(L, -2);
	printf("ref value %d\n", ref);
	
	lua_pushlightuserdata(L, &lua_light_user_data_key);
	lua_rawget(L, LUA_REGISTRYINDEX);
	ref = luaL_ref(L, -1);
	printf("ref value %d\n", ref);
	
	lua_pushlightuserdata(L, &lua_light_user_data_key);
	lua_rawget(L, LUA_REGISTRYINDEX);
	ref = luaL_ref(L, -1);
	printf("ref value %d\n", ref);

	lua_pushlightuserdata(L, &lua_light_user_data_key);
	lua_rawget(L, LUA_REGISTRYINDEX);
	ref = luaL_ref(L, -1);
	printf("ref value %d\n", ref);

	lua_pushlightuserdata(L, &lua_light_user_data_key);
	lua_rawget(L, LUA_REGISTRYINDEX);
	luaL_unref(L, -1, 4);
	luaL_unref(L, -1, 6);
	ref = luaL_ref(L, -1);
	printf("ref value %d\n", ref);

	lua_pushlightuserdata(L, &lua_light_user_data_key);
	lua_rawget(L, LUA_REGISTRYINDEX);
	ref = luaL_ref(L, -1);
	printf("ref value %d\n", ref);

	
	//lua_rawgeti(L, -2, ref); 

	//lua_pushlightuserdata(L, &lua_light_user_data_key);
	//lua_rawget(L, LUA_REGISTRYINDEX);

	//ref = lua_ref(L, -2);

//	lua_pushlightuserdata(L, &lua_light_user_data_key);
//	lua_rawget(L, LUA_REGISTRYINDEX);

	lua_settop(L, base);

#if 0
	lua_newtable(L);
	lua_pushinteger(L, 10);
	lua_rawseti(L, -2, 1);
	lua_pushinteger(L, 20);
	lua_rawseti(L, -2, 2);
	
	printf("objlen:%ld\n", lua_objlen(L, -1));
	ref = luaL_ref(L, -1);
	printf("%d\n", ref);
#endif

}
