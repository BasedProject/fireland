#include <assert.h>
#include <stddef.h>
#include <math.h>
#include <vector>
#include <numeric>
#include <stdexcept>
#include <raylib.h>
extern "C" {
  #define __STDC_VERSION__TEMP __STDC_VERSION__
  #undef __STDC_VERSION__
  #define __STDC_VERSION__ 999999L
  #include <terry.h>
  #undef __STDC_VERSION__
  #define __STDC_VERSION__ __STDC_VERSION__TEMP
}


using namespace std;

#include "raylib-extra.hpp"

v2 screen_area[1];

#include "meat.hpp"

float road_diameter  = 50;
float block_diameter = 20;

int TICK = 0;

#include "Board.hpp"
#include "board_generation.hpp"
#include "fire.hpp"
#include "draw.hpp"
#include "player.hpp"

int main(int ac, char ** av)
{   char * program_name = av[0];
    rl_screen screen[1];
    v2 physical_area[1];
    // Texture texture[TEXTURE_END];
    // Sound sound[SOUND_END];

    // change_directory(program_name);

    meat_init(program_name, screen, screen_area, physical_area);

    v2 mapshape[1] = {12,8};

    player_t player[0] = {};
    Board board = Board(mapshape->x, mapshape->y); // AM: thoughts on v2 being split on input in C++ (primarily C++ at all)
    randomize_board(board);
    random_fires(board, 3);

    v2 screen_space_used_by_board = get_board_display_size(board);

    while (!WindowShouldClose()) {
        // Update
        update_fire_spread(board);
        physical_area[0] = rl_get_render_area();
        // update_player(player, rl_screen_shape(screen));
        ++TICK;

        // Draw
        {   BeginTextureMode(screen[0]);
            ClearBackground(GREEN);
            draw_board(board);
            // draw_player(player, 50, rl_screen_shape(screen));
            draw_debug_fire(board);
            draw_debug_grid();
            EndTextureMode();
        }

        {   BeginDrawing();
            ClearBackground(BLACK);
            DrawTexturePro(
                screen[0].texture,
                (Rectangle){
                    0,
                    -screen_space_used_by_board.y,
                    screen_space_used_by_board.x,
                    -screen_space_used_by_board.y
                },
                rl_v4_rectangle(
                    rl_fit_centered(screen_space_used_by_board, screen_area[0])
                ),
                (v2){ 0, 0 },
                0,
                WHITE
            );
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
