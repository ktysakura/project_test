#include <lua.h>
#include <stdlib.h>
#include <string.h>
#include <lauxlib.h>
#include <lualib.h>
#include <math.h>

#define lua_lightudata_mask(key)						\
	&(lua_##key)

char lua_socket_pool_key;
char lua_coroutines_key;
char lua_code_cache_key;

static char lua_tcp_metatable_key;
static char lua_udp_metatable_key;

static char lua_tcp_udata_metatable_key;
static char lua_udp_udata_metatable_key;

struct tcp_t
{	
	int a;
	int b;
};

static void lua_init_register(lua_State *L)
{
	lua_pushlightuserdata(L, lua_lightudata_mask(
						 socket_pool_key));
	lua_createtable(L, 0, 32);
	lua_rawset(L, LUA_REGISTRYINDEX);

	lua_pushlightuserdata(L, lua_lightudata_mask(
						  coroutines_key));
	lua_createtable(L, 0, 128);
	lua_rawset(L, LUA_REGISTRYINDEX);

	lua_pushlightuserdata(L, lua_lightudata_mask(
						  code_cache_key));
	lua_createtable(L, 0, 4);
	lua_rawset(L, LUA_REGISTRYINDEX);
}


static int lua_socket_tcp_receive(lua_State *L)
{
	printf("tcp receive()\n");
	return 1;
}

static int lua_socket_tcp_connect(lua_State *L)
{
	int n;
	int pool_size;	
	int backup;
	const char *msg;
	int port;
	
	pool_size = 0;
	
	n = lua_gettop(L);
	if (n != 2 || n != 3 || n != 4) {
		return luaL_error(L, "ngx.socket connect: expecting 2, 3, or 4"
						 "arguments (incluing the object), but seen %d", n);
	}

	luaL_checktype(L, 1, LUA_TTABLE);

	if (lua_istable(L, n)) {

		lua_getfield(L, n, "pool_size");

		if (lua_isnumber(L, -1)) {
			pool_size = lua_tointeger(L, -1);

			if (pool_size <= 0) {
				msg = lua_pushfstring(L, "bad \"pool_size\" option value: %i", 
									  pool_size);
				return luaL_argerror(L, n, msg);
			} 
		
		} else if (!lua_isnil(L, -1)) {
			msg = lua_pushfstring(L, "bad \"pool_size\" option type: %s", 			
								  lua_typename(L, lua_type(L, -1))); 
			return luaL_argerror(L, n, msg);
		}

		lua_pop(L, 1);

		lua_getfield(L, n, "backlog");

		if (lua_isnumber(L, -1)) {
			backup = lua_tointeger(L, -1);

			if (backup <= 0) {
				msg = lua_pushfstring(L, "bad \"backlog\" option value: %i",
									  backup);
				return luaL_argerror(L, n, msg);
			}

		} else if (!lua_isnil(L, -1)) {
			msg = lua_pushfstring(L, "bad \"backlog\" option type: %s",
								  lua_typename(L, lua_type(L, -1)));
			return luaL_argerror(L, n, msg);
		}

		lua_pop(L, 1);

	//	lua_getfield(L, "pool");

		switch(lua_type(L, -1)) {
				
		case LUA_TNUMBER:
			lua_tostring(L, -1);
			/* fall through*/
			printf("number");

		case LUA_TSTRING:
			printf("string");
			break;

		case LUA_TNIL:
			lua_pop(L, 1);
			break;

		default:	
			msg = lua_pushfstring(L, "bad \"pool\" option type: %s", 
								  lua_typename(L, lua_type(L, -1)));
			return luaL_argerror(L, n, msg);
		}

		n--;
	}

	/*the fourth argument is not a table*/
	if (n == 4) {
		lua_pop(L, 1);
		n--;
	}

	if (n == 3) {
		port = luaL_checkinteger(L, 3);

		if (port < 0 || port > 65535) {
			lua_pushnil(L);
			lua_pushfstring(L, "bad port number: %i", port);
			return 2;
		}
	} else {
		port = 0;
	}

	return 1;
}
static int lua_socket_tcp_settimeout(lua_State *L)
{
	return 1;
}

static int lua_socket_tcp_send(lua_State *L)
{
	printf("tcp send()\n");
	return 1;
}

static int lua_socket_tcp_settimeouts(lua_State *L)
{
	printf("tcp settimeouts\n");
	return 1;
}

static int lua_socket_tcp_close(lua_State *L)
{
	printf("tcp close\n");
	return 1;
}

static int lua_socket_udp_send(lua_State *L)
{
	printf("udp send()\n");
	return 1;
}

static int lua_socket_udp_receive(lua_State *L)
{
	printf("udp receive\n");
	return 1;
}

static int lua_socket_udp_settimeout(lua_State *L)
{
	printf("udp settimeout()\n");
	return 1;
}

static int lua_socket_tcp_destrory(lua_State *L)
{
	printf("tcp destroy()\n");
	return 1;
}

static int lua_socket_tcp(lua_State *L)
{
	lua_createtable(L, 5, 1);


	lua_pushlightuserdata(L, lua_lightudata_mask(
						  tcp_metatable_key));
	lua_rawget(L, LUA_REGISTRYINDEX);

	lua_setmetatable(L, -2);

	return 1;
}

static int lua_socket_udp(lua_State *L)
{

	lua_createtable(L, 3, 1);

	lua_pushlightuserdata(L, lua_lightudata_mask(
					     udp_metatable_key));
	lua_rawget(L, LUA_REGISTRYINDEX);

	lua_setmetatable(L, -2);
	
	return 1;
}

static int lua_socket_tcp_destroy(lua_State *L)
{
	printf("tcp destroy\n");
	return 1;
}
static void lua_inject_socket_tcp_api(lua_State *L)
{
	lua_createtable(L, 0, 4);

	lua_pushcfunction(L, lua_socket_tcp); 
	lua_pushvalue(L, -1);

	lua_setfield(L, -3, "tcp");
	lua_setfield(L, -2, "stream");

	lua_setfield(L, -2, "socket");

	lua_pushlightuserdata(L, lua_lightudata_mask(
						  tcp_metatable_key));

	lua_createtable(L, 0, 13);

	lua_pushcclosure(L, lua_socket_tcp_connect,  0);
	lua_setfield(L, -2, "connect");

	
	lua_pushcclosure(L, lua_socket_tcp_receive, 0);
	lua_setfield(L, -2, "receive");

	lua_pushcclosure(L, lua_socket_tcp_send, 0);
	lua_setfield(L, -2, "send");

	lua_pushcclosure(L, lua_socket_tcp_settimeout, 0);
	lua_setfield(L, -2, "settimeout");

	lua_pushcclosure(L, lua_socket_tcp_settimeouts, 0);
	lua_setfield(L, -2, "settimeouts");

	lua_pushcclosure(L, lua_socket_tcp_close, 0);
	lua_setfield(L, -2, "close");

	lua_pushvalue(L, -1);
	//self.index = self
	lua_setfield(L, -2, "__index");

	lua_rawset(L, LUA_REGISTRYINDEX);

	lua_pushlightuserdata(L, lua_lightudata_mask(
						  tcp_udata_metatable_key));
	lua_createtable(L, 0, 1);
	lua_pushcfunction(L, lua_socket_tcp_destroy);
	lua_setfield(L, -2, "__gc");

	lua_rawset(L, LUA_REGISTRYINDEX);

}

static void lua_inject_socket_udp_api(lua_State *L)
{
	lua_getfield(L, -1, "socket");
	lua_pushcfunction(L, lua_socket_udp);
	lua_setfield(L, -2, "udp");

	lua_pushlightuserdata(L, lua_lightudata_mask(
						  udp_metatable_key));
	lua_createtable(L, 0, 4);

	lua_pushstring(L, "settimeout");
	lua_pushcfunction(L, lua_socket_udp_settimeout);
	lua_settable(L, -3);

	lua_pushstring(L, "send");
	lua_pushcfunction(L, lua_socket_udp_send);
	lua_settable(L, -3);

	lua_pushstring(L, "receive");
	lua_pushcfunction(L, lua_socket_udp_receive);
	lua_settable(L, -3);

	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");

	lua_rawset(L, LUA_REGISTRYINDEX);

	lua_pop(L, 1);
	
}

static void lua_inject_api(lua_State *L)
{
	lua_createtable(L, 0, 113);
	lua_inject_socket_tcp_api(L);
	lua_inject_socket_udp_api(L);
	lua_setfield(L, LUA_GLOBALSINDEX, "myapi");
}

int luaopen_myapi(lua_State *L)
{
	lua_inject_api(L);
	return 1;
}

int lua_init_vm()
{
//	lua_State *L = lua_newstate();
//	lua_init_register(L);
//	lua_inject_api(L);
	return 1;
}
