#include <stdio.h>
#include <stdlib.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h" 

int main (int argc, char *argv[])
{
  lua_State *ls;

  ls = luaL_newstate();

  luaL_openlibs(ls);

  luaL_dofile(ls, "main.lua");

  lua_close(ls);

  return 0;
}
