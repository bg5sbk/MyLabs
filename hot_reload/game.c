#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#include "game.h"
#include "game_core.h"

void test(void);

game_api api;

void *game_core_handle;

int main(int argc, char **argv)
{

  test();
  getchar();
  test();

  return 0;
}

void test(void)
{
  if (game_core_handle != NULL)
    dlclose(game_core_handle);

  game_core_handle = dlopen("./game_core.so", RTLD_LAZY);

  if (game_core_handle == NULL) {
    printf("open game_core.so failed: %s\n", dlerror());
    exit(-1);
  }

  void *(*init)(game_api *) = (void *(*)(game_api *))dlsym(game_core_handle, "game_core_init");

  init(&api);

  api.login("dada", "ooxxooxxooxx");
}
