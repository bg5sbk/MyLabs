#include <stdio.h>

#include "game.h"
#include "game_core.h"

void login(char *username, char *hashcode);

void game_core_init(game_api *api)
{
  api->login = login;
}

void login(char *username, char *hashcode)
{
  printf("login(\"%s\", \"%s\") ... failed\n", username, hashcode);
}
