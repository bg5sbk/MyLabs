#include <stdio.h>
#include <stdlib.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h" 

int main (int argc, char *argv[])
{
  char c;

  lua_State *ls;

  ls = luaL_newstate();

  luaL_openlibs(ls);

  luaL_dofile(ls, "main.lua");

  printf("> ");

  while ((c = getchar()) != 'q') {
    if (c == '\r' || c == '\n')
      continue;
    
    if (c == 'r')
      luaL_dofile(ls, "main.lua");

    lua_getglobal(ls, "mod_player");
    lua_getfield(ls, -1, "login");
    lua_call(ls, 0, 1);
    lua_pop(ls, 2);

    printf("> ");
  }

  lua_close(ls);

  return 0;
}
