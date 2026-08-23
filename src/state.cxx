#include "state.h"

int state(const char* fileName)
{
    lua_State *L = luaL_newstate();

    if (!L)
        return 1;

    int result = luaL_dofile(L, fileName);

    if (result != LUA_OK)
        return result;

    lua_close(L);

    return result;
}
