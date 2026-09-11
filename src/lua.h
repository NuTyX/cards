#pragma once

#include "file_utils.h"
#include "process.h"

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <errno.h>
#include <string.h>

namespace cards {

struct State;

void luaRegister(lua_State *L);

}
