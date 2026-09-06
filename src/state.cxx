#include "state.h"

int state(const char* fileName)
{
    lua_State *L = luaL_newstate();

    if (!L)
        return 1;

    luaL_openlibs(L);

    int result = luaL_dofile(L, fileName);

    if (result != LUA_OK) {
        const char *error = lua_tostring(L, -1);
        fprintf(stderr, "Lua error: %s\n",
                error ? error : "unknown error");

    }
    lua_close(L);

    return result;
}
