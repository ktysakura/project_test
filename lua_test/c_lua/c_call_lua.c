#include <stdlib.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <stdio.h>
static void
call_fun(lua_State *L, const char *funstr)
{
    lua_getglobal(L, funstr);
    lua_call(L, 0, 0);
}

static void
call_multi_fun(lua_State *L, const char *funstr)
{
	lua_getglobal(L, funstr);
	lua_getfield(L, LUA_GLOBALSINDEX, "age");

	if (!lua_isfunction(L, -2) || !lua_isnumber(L, -1)) {
		fprintf(stderr, "error:is wrong paramter\n");
		return ;
	}

	lua_pushnumber(L, 10);
	lua_pushnumber(L, 29);
	lua_pushnumber(L, 39);
	lua_pushnumber(L, 49);
	lua_call(L, 5, LUA_MULTRET);
	printf("%ld\n", lua_tointeger(L, 1));
	printf("%ld\n", lua_tointeger(L, 2));

	
}

const char * lua_get_myfield_string(lua_State *L, int idx, const char *k)
{
	const char *p;

	lua_pushstring(L, k);
	lua_gettable(L, idx - 1);
	if (!lua_isstring(L, -1)) {
		fprintf(stderr, "get string\n");
	}

	p = lua_tostring(L, -1);
	lua_pop(L, 1);
	return p;
}
		
void get_path(lua_State *L)
{
	const char *path;
	size_t		id;
	size_t		path_len;

	lua_getfield(L, LUA_GLOBALSINDEX, "package");

#if 1 
	path = lua_get_myfield_string(L, -1, "name");
	lua_getfield(L, -1, "id");
	id = lua_tonumber(L, -1); 
#endif
	printf("%s, %ld\n", path, id);

}

void set_path(lua_State *L)
{

#if 1
	lua_newtable(L);
	if (!lua_istable(L, -1)) {
		fprintf(stderr, "bucket\n");
	}
#endif

	lua_pushstring(L, "world");
	lua_setfield(L, -2, "hello");

	lua_pushstring(L, "value");
	lua_setfield(L, -2, "key");

	lua_setglobal(L, "bucket");

	lua_getglobal(L, "bucket");
	lua_getfield(L, -1, "hello");
	
	if (!lua_isstring(L, -1)) {
		fprintf(stderr, "error: lua_isstring\n");
		return;
	}

	const char * strval = lua_tostring(L, -1); 

	printf("%s\n", strval);

	lua_pop(L, 1);
}

int main(int argc, char *argv)
{
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
  
    if (0 != luaL_dofile(L, "./test-vm.lua")) {
        const char *err = lua_tostring(L, -1);
        fprintf(stderr, "%s", err);
    }

    call_fun(L, "init");
    call_fun(L, "dowork");
    call_multi_fun(L, "finalize");
//	get_path(L);
	lua_pop(L, -1);
//	set_path(L);


    return 0;
}
