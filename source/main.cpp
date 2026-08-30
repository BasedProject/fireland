#include <assert.h>
#include <stddef.h>
#include <vector>
#include <stdexcept>
#include <raylib.h>

using namespace std;

#include "raylib-extra.hpp"

const float road_diameter  = 20;
const float block_diameter = 70;
const int W = 1200;
const int H = 800;

#include "Board.hpp"
#include "board_generation.hpp"
#include "draw.hpp"
#include "meat.hpp"

int main(int ac, char ** av)
{   rl_screen screen[1];
    v2 screen_area[1], physical_area[1];
    // Texture texture[TEXTURE_END];
    // Sound sound[SOUND_END];

    meat_init(av[0], screen, screen_area, physical_area);

    Board board = Board(12, 8);
    randomize_board(board);

    while (!WindowShouldClose()) {
      physical_area[0] = rl_get_render_area();
      {   BeginTextureMode(screen[0]);
          draw_board(board);
          EndTextureMode();
      }
      {   BeginDrawing();
          DrawTexturePro(screen->texture,
                         rl_screen_shape(screen),
                         rl_v4_rectangle(rl_fit_centered(screen_area[0], physical_area[0])),
                         (v2){ 0, 0 },
                         0,
                         WHITE);
          ClearBackground(BLACK);
          EndDrawing();
      }
    }

    UnloadRenderTexture(screen[0]);
    // for (size_t i = 0; i < TEND; ++i)
    // { UnloadTexture(texture[i]);
    // }
    // for (size_t i = 0; i < SEND; ++i)
    // { UnloadSound(sound[i]);
    // }

    CloseWindow();
    return 0;
}
