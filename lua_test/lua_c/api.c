#include <lua.h>
#include <stdlib.h>
#include <string.h>
#include <lauxlib.h>
#include <lualib.h>
#include <math.h>

#define lua_lightdata_mask(key) \
	&lua_##key

static char lua_socket_tcp_metatable_key;
static char lua_socket_udp_metatable_key;

static char lua_socket_tcp_pattern_udata_metatable_key;
static char lua_socket_udp_pattern_udata_metatable_key;

static int 
lua_socket_tcp_send(lua_State *L)
{
	printf("tcp send\n");
	return 1;
}

static int 
lua_socket_tcp_receive(lua_State *L)
{
	printf("tcp receive\n");
	return 1;
}

static int
lua_socket_tcp_connect(lua_State *L)
{
	printf("tcp connect\n");
	return 1;
}

static int 
lua_socket_tcp_close(lua_State *L)
{
	printf("tcp close\n");
	return 1;
}

static int 
lua_socket_tcp_destroy(lua_State *L)
{
	printf("tcp destroy\n");
	return 1;
}

static int
lua_socket_udp_send(lua_State *L)
{
	printf("udp send\n");
	return 1;
}

static int
lua_socket_udp_receive(lua_State *L)
{
	printf("udp receive\n");
	return 1;
}


static int 
lua_socket_udp_destroy(lua_State *L)
{
	printf("udp destroy\n");
	return 1;
}

static int 
lua_socket_tcp_api(lua_State *L)
{
	if (lua_gettop(L) != 0) {
		return luaL_error(L, "expecting zero arguments, but got %d",
						  lua_gettop(L));
	}

	lua_createtable(L, 5, 1);
	lua_pushlightuserdata(L, lua_lightdata_mask(
						  socket_tcp_metatable_key));
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_setmetatable(L, -2);

	return 1;
}

static int 
lua_socket_udp_api(lua_State *L)
{
	if (lua_gettop(L) != 0) {
		return luaL_error(L, "expecting zero arguments, but got %d",
						 lua_gettop(L));
	}

	lua_createtable(L, 5, 1);
	lua_pushlightuserdata(L, lua_lightdata_mask(
						  socket_udp_metatable_key));
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_setmetatable(L, -2);

	return 1;
}

void 
lua_inject_socket_udp_api(lua_State *L)
{
	lua_getfield(L, -1, "socket");
	lua_pushcfunction(L, lua_socket_udp_api);
	lua_setfield(L, -2, "udp");


	lua_pushlightuserdata(L, lua_lightdata_mask(
						  socket_udp_metatable_key));
	lua_createtable(L, 0, 4);

	lua_pushcfunction(L, lua_socket_udp_send);
	lua_setfield(L, -2, "send");

	lua_pushcfunction(L, lua_socket_udp_receive);
	lua_setfield(L, -2, "receive");

	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");

	lua_rawset(L, LUA_REGISTRYINDEX);



	lua_pushlightuserdata(L, lua_lightdata_mask(
						  socket_udp_pattern_udata_metatable_key));
	lua_createtable(L, 0, 1);

	lua_pushcfunction(L, lua_socket_udp_destroy);
	lua_setfield(L, -2,  "__gc");

	lua_rawset(L, LUA_REGISTRYINDEX);

	lua_pop(L, 1);
}

void 
lua_inject_socket_tcp_api(lua_State *L)
{
	lua_createtable(L, 0, 13);

	lua_pushcfunction(L, lua_socket_tcp_api);
	lua_pushvalue(L, -1);

	lua_setfield(L, -3, "tcp");
	lua_setfield(L, -2, "stream");

	lua_setfield(L, -2, "socket");
	

	lua_pushlightuserdata(L, lua_lightdata_mask(
					      socket_tcp_metatable_key));
	lua_createtable(L, 0, 4);

	lua_pushcfunction(L, lua_socket_tcp_connect);
	lua_setfield(L, -2, "connect");

	lua_pushcfunction(L, lua_socket_tcp_send);
	lua_setfield(L, -2, "send");

	lua_pushcfunction(L, lua_socket_tcp_receive);
	lua_setfield(L, -2, "receive");

	lua_pushcfunction(L, lua_socket_tcp_close);
	lua_setfield(L, -2, "close");

	//self.index = self
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");

	lua_rawset(L, LUA_REGISTRYINDEX);


	lua_pushlightuserdata(L, lua_lightdata_mask(
						  socket_tcp_pattern_udata_metatable_key));
	lua_createtable(L, 0, 1);
	lua_pushcfunction(L, lua_socket_tcp_destroy);
	lua_setfield(L, -2, "__gc");
	lua_rawset(L, LUA_REGISTRYINDEX);

}

static void 
lua_inject_project_api(lua_State *L)
{
	lua_createtable(L, 0, 113);
	lua_inject_socket_tcp_api(L);
	lua_inject_socket_udp_api(L);
	lua_setfield(L, LUA_GLOBALSINDEX, "myapi");
}

void
luaopen_myapi(lua_State *L)
{
	lua_inject_project_api(L);
}
