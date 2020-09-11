#include <lua.h>
#include <stdlib.h>
#include <string.h>
#include <lauxlib.h>
#include <lualib.h>
#include <math.h>

static char lua_light_user_data_key;
static char lua_table1_key;
static char lua_table2_key;

static int lua_my_newtable(lua_State *L)
{	

#if 0
	lua_pushlightuserdata(L, &lua_table1_key);
	lua_pushstring(L, "apple");
	lua_rawset(L, LUA_REGISTRYINDEX);

	lua_pushlightuserdata(L, &lua_table2_key);
	lua_pushstring(L, "banana");
	lua_rawset(L, LUA_REGISTRYINDEX);

	lua_pushlightuserdata(L, &lua_table1_key);
	lua_rawget(L, LUA_REGISTRYINDEX);

	lua_pushlightuserdata(L, &lua_table2_key);
	lua_rawget(L, LUA_REGISTRYINDEX);
#endif
	lua_pushlightuserdata(L, &lua_table1_key);
	lua_createtable(L, 0, 32);
	lua_rawset(L, LUA_REGISTRYINDEX);

	lua_pushlightuserdata(L, &lua_table2_key);
	lua_createtable(L, 0, 4);
	lua_rawset(L, LUA_REGISTRYINDEX);

	lua_pushlightuserdata(L, &lua_table1_key);
	lua_rawget(L, LUA_REGISTRYINDEX);

	lua_pushstring(L, "val1");
	lua_rawseti(L, -2, 1);

	lua_pushstring(L, "val2");
	lua_rawseti(L, -2, 2);

	return 1;
}

static int lua_split(lua_State *L)
{
	const char *src, *sep, *p, *last; 
	int idx, seplen;

	src = luaL_checkstring(L, 1);
	sep = luaL_checkstring(L, 2); 

	idx = 1;
	seplen = strlen(sep);
	last = src + strlen(src);
	
	lua_newtable(L);

	while(p = strstr(src, sep)) {
		lua_pushlstring(L, src, p - src); 
		lua_rawseti(L, -2, idx++); 
		src = p + seplen;
		if (src == last) {
			return 1;
		}
	}

	lua_pushstring(L, src);
	lua_rawseti(L, -2, idx);

	return 1;
}

static int lua_error_msg(lua_State *L)
{

	printf("error msg:");
}


static int lua_func(lua_State *L)
{
	int upvalue1 = lua_tointeger(L, lua_upvalueindex(1));
	int upvalue2 = lua_tointeger(L, lua_upvalueindex(2));
	printf("closure:%d, %d\n", upvalue1, upvalue2);

	lua_pushinteger(L, ++upvalue1);
	lua_pushvalue(L, -1);
	lua_replace(L, lua_upvalueindex(1));

	upvalue2 += 10;
	lua_pushinteger(L, upvalue2); 
	lua_pushvalue(L, -1);
	lua_replace(L,lua_upvalueindex(2));

	return 2;
}

static int lua_closure(lua_State *L)
{
	lua_pushinteger(L, 200);
	lua_pushcclosure(L, lua_func, 2);
	return 1;
}

static int lua_array_walk(lua_State *L)
{
	luaL_checktype(L, 1, LUA_TTABLE);
//	size_t tab_len = lua_objlen(L, 1);
	if (!lua_istable(L, 1)) {
		fprintf(stderr, "%s", "first is not table");
	}
	int n = luaL_getn(L, 1);
	luaL_checktype(L, 2, LUA_TFUNCTION);

	lua_pushcfunction(L, lua_error_msg); 
	for (int i = 1; i <= n; i++) {
		//lua_pushvalue() copy位置的元素插入栈顶
		lua_pushvalue(L, 2);
#if 0
		lua_rawgeti(L, 1, i);
#endif
		lua_pushnumber(L, i);
		lua_rawget(L, 1);
		printf("\n");
		//printf("array[%d]=%ld\n", i, lua_tointeger(L, -1));
		//printf("before call %d\n", lua_gettop(L));
		//lua_call(L, 1, 1);
		if (lua_pcall(L, 1, 5, 3) != 0) {
			fprintf(stderr, "lua_call()\n");
		}
	//	printf("before call %d\n", lua_gettop(L));
#if 0
		lua_rawseti(L, 1, i);
#endif
		lua_pushnumber(L, i);
		lua_insert(L, -2);
		lua_rawset(L, 1);
	//	printf("array size:%ld\n", lua_objlen(L, 1));
	//	printf("after call %d\n", lua_gettop(L));
	}

	return 0;
}


static int lua_plus(lua_State *L){
	lua_Integer a = luaL_checkinteger(L, 1);
	lua_Integer b = luaL_checkinteger(L, 2);

	lua_pushinteger(L, a + b);
	lua_insert(L, 2);
	//lua_replace(L, 1);
	//	lua_settop(L, 10);
	//	lua_settop(L, 0);
	
	for (int i = lua_gettop(L); i >= 1 ; i--) {
		printf("%ld\t", lua_tointeger(L, i));
	}
	lua_pushinteger(L, 1000);
	lua_pushinteger(L, 2000);

	return 1;
}

typedef struct data_s{
	int id;
	int age;
	char *name;
}data_t;

static int garbage(lua_State *L)
{
	printf("garbage\n");
	return 1;
}

static int lua_data(lua_State *L) 
{
	data_t *data =lua_newuserdata(L, sizeof(data_t));
	data->id = 10;
	data->age = 20;

	lua_createtable(L, 0, 1);
	lua_pushcfunction(L, garbage); 
	lua_setfield(L, -2, "__gc");

	lua_setmetatable(L, -2);
	return 1;
}

static int lua_getid(lua_State *L)
{
	data_t *data = (data_t *)lua_touserdata(L, 1);
	luaL_argcheck(L, data != NULL, 1, "wrong paramter");
	lua_pushinteger(L, data->id);
	return 1;
}

static int lua_setid(lua_State *L)
{
	data_t *data = (data_t *)lua_touserdata(L, 1);
	luaL_argcheck(L, data != NULL, 1, "wrong parameter");
	data->id = luaL_checkinteger(L, 2);

	return 1;
}

static int lua_getage(lua_State *L)
{
	printf("%d\n", lua_gettop(L));
	data_t *data = (data_t *)lua_touserdata(L, 1);
	printf("%d\n", lua_gettop(L));
	
	lua_pushinteger(L, data->age);
	return 1;
}

static int lua_setage(lua_State *L)
{
	data_t *data = (data_t *)lua_touserdata(L, 1);
	luaL_argcheck(L, data != NULL, 1, "wrong parameter");
	data->age = luaL_checkinteger(L, 2);

	return 1;
}

int lua_light_data(lua_State *L)
{

	lua_pushlightuserdata(L, (void *) &lua_light_user_data_key); 
	lua_pushstring(L, "light user data");
	lua_settable(L, LUA_REGISTRYINDEX);
	return 0;
}

int lua_get_light_data(lua_State *L)
{
	lua_pushlightuserdata(L, (void *)&lua_light_user_data_key);
	lua_gettable(L, LUA_REGISTRYINDEX);
	return 1;
}

static const struct luaL_Reg mylib[] = {
	{"new", lua_data},
	{"add", lua_plus},
	{"getid", lua_getid},
	{"setid", lua_setid},
	{"getage", lua_getage},
	{"setage", lua_setage},
	{"light_data", lua_light_data},
	{"get_light_data", lua_get_light_data},
	{"array_walk", lua_array_walk},
	{"closure", lua_closure},
	{"split", lua_split},
	{"newtable", lua_my_newtable}, 
	{NULL, NULL}
};

int luaopen_mylib(lua_State *L)
{
	luaL_register(L, "mylib", mylib);
	return 1;
}

#if 0
int main()
{
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);
	lua_pushcfunction(L, lua_plus);
	lua_setglobal(L, "myplus");
	if (luaL_dostring(L, "print(myplus(2,2))")) {
		printf("hello world");
		lua_close(L);
	}
	lua_close(L);

	return 0;
}
#endif

int lua_echo_type(lua_State *L)
{
	int top = lua_gettop(L);
	printf("\tsize:%d\n", top);
	printf("\t");
	for (int i = 1; i <= top; i++) {
		switch (lua_type(L, i)) {

		case LUA_TTABLE:
			printf("table:%s\n", lua_typename(L, i));
			break;

		case LUA_TBOOLEAN:
			printf("%s\n", lua_toboolean(L, i) ? "true" : "false");
			break;

		case LUA_TSTRING:
			printf("%s\n", lua_tostring(L, i));
			break;
			
		case LUA_TNUMBER:
			printf("%g\n", lua_tonumber(L, i));
			break;
		
		default:
			printf("%s\n", lua_typename(L, i));
			break;
		}

		printf("\t");
	}
	return 0;
}

int main()
{
	lua_State  *L = luaL_newstate();
	lua_State *co;
	int base;

	luaL_openlibs(L);

#if 0
	lua_register(L,  "add", lua_plus);
	lua_register(L, "echo_type", lua_echo_type);
#if 0
	if (luaL_dostring(L, "print(add(1, 2))") < 0) {
		printf("luaL_dostring()\n");
	}
#endif

//	if (luaL_dostring(L, "echo_type(2,4.0,false)"))
	if (luaL_dostring(L, " tab = {key=\"name\", 2} echo_type(tab, \"hello world\")") != 0)
	{
		printf("luaL_dostring()\n");
	}
#endif
	lua_close(L);
}
