typedef struct
{ v3 position[1];  // x, y, rotation
  v3 velocity[1];  // ...
  v2 direction[1]; // tank input
} player_t;

void update_player(player_t * player, Rectangle screen_shape)
{   int key[4]    = { KEY_W, KEY_S, KEY_A, KEY_D }; //nswe
    v2  speed[1]  = {0.35, 0.2};
    v2  dampen[1] = {0.91, 0.97};

   #define nullcancel(axis, prograde, retrograde) do \
    { if (IsKeyPressed (prograde)                                 ) player->direction->axis = 1; \
      if (IsKeyReleased(prograde)   && player->direction->axis > 0) player->direction->axis = IsKeyDown(retrograde) ? -1 : 0; \
      if (IsKeyPressed (retrograde)                               ) player->direction->axis = -1; \
      if (IsKeyReleased(retrograde) && player->direction->axis < 0) player->direction->axis = IsKeyDown(prograde) ? 1 : 0; \
    } while (0);
    #define polar(north, south, west, east) do \
      { nullcancel(x, east, west);             \
        nullcancel(y, north, south);           \
      } while (0);

    polar(key[0], key[1], key[2], key[3]);

    player->velocity->y += player->direction->y * speed->y;
    player->velocity->x += player->direction->x * speed->x;
    player->velocity->y *= dampen->y;
    player->velocity->x *= dampen->x;

    player->position->z += player->velocity->x;
    player->position->x += cosf(DEG2RAD * player->position->z) * player->velocity->y;
    player->position->y += sinf(DEG2RAD * player->position->z) * player->velocity->y;

    wrap((v2*)player, &screen_shape);
}

void draw_player(player_t * player, float diameter, Rectangle screen_shape)
{ DrawCircleWrapped((v2) {player->position->x, player->position->y }, diameter/2, screen_shape, GREEN);
}
