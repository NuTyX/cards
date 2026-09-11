#include "lua.h"

namespace cards {
static int fsLn(lua_State *L)
{
    const char *target = luaL_checkstring(L, 1);
    const char *linkpath = luaL_checkstring(L, 2);

    if (symlink(target, linkpath) != 0)
        return luaL_error(L, "ln: %s", strerror(errno));

    return 0;
}
static int fsMv(lua_State *L)
{
	const char *source = luaL_checkstring(L, 1);
	const char *destination = luaL_checkstring(L, 2);

	if (rename(source, destination) != 0)
		return luaL_error(L, "mv: %s", strerror(errno));

	return 0;
}
static int fsCp(lua_State *L)
{
    const char *source = luaL_checkstring(L, 1);
    const char *destination = luaL_checkstring(L, 2);

    int result = copyFile(destination, source);
    
    if (result != 0)
        return luaL_error(L, "copy failed");

    return 0;
}
static int fsRm(lua_State *L)
{
    const char *path = luaL_checkstring(L, 1);

	if (!checkFileExist(path))
		return 0;

    if (unlink(path) != 0)
        return luaL_error(L, "rm: %s", strerror(errno));

    return 0;
}
static int depmod(lua_State *L)
{
    const char *version = luaL_checkstring(L, 1);

    process p(DEPMOD, std::string(DEPMOD_ARGS) + version, 0);
    return p.execute();
}
static const luaL_Reg fs[] = {
    { "ln", fsLn },
    { "cp", fsCp },
    { "rm", fsRm },
    { "mv", fsMv },
    { NULL, NULL }
};

static const luaL_Reg cards[] = {
    { "depmod", depmod },
    { NULL, NULL }
};
void luaRegister(lua_State *L)
{
    lua_newtable(L);              // cards

    luaL_setfuncs(L, cards, 0);

    lua_newtable(L);              // cards.fs


    luaL_setfuncs(L, fs, 0);

    lua_setfield(L, -2, "fs");    // cards.fs

    lua_setglobal(L, "cards");    // global cards
}
} // cards namespace
