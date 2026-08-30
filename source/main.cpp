#include <assert.h>
#include <stddef.h>
#include <math.h>
#include <vector>
#include <numeric>
#include <stdexcept>
#include <raylib.h>
#include <raymath.h>
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


#include "meat.hpp"

const float road_diameter  = 50;
const float block_diameter = 20;
const float cell_diameter  = road_diameter + block_diameter;
const float road_radius  = road_diameter / 2;

int TICK = 0;
int TICKS_PER_SECOND = 60;
bool is_paused;

#include "Board.hpp"
#include "board_generation.hpp"
#include "fire.hpp"
#include "player.hpp"
#include "draw.hpp"

int main(int ac, char ** av)
{   (void)ac;
    char * program_name = av[0];
    rl_screen screen[1];
    v2 screen_area[1];
    v2 physical_area[1];
    v2 mapshape[1] = {v2{12,8}};
    player_t player = {};
    player.position = {0, 0};
    Board board = Board(mapshape->x, mapshape->y);
    // Texture texture[TEXTURE_END];
    // Sound sound[SOUND_END];

    // change_directory(program_name);

    screen_area[0] = get_board_display_size(board);
    meat_init(program_name, screen, screen_area, physical_area);
    SetTargetFPS(TICKS_PER_SECOND);

    randomize_board(board);
    random_fires(board, 3);

    while (!WindowShouldClose()) {
        // Input
        if (IsKeyPressed(KEY_ESCAPE)) {
            is_paused = !is_paused;
        }

        // Update
        if (is_paused) {
            goto draw;
        }

        update_fire_spread(board);
        physical_area[0] = rl_get_render_area();
        update_player(player, rl_v2_rectangle_wh(screen_area[0]));
        ++TICK;

        // Draw
        draw:
        do {
            BeginTextureMode(screen[0]);
            ClearBackground(GREEN);
            draw_board(board);
            draw_debug_fire(board);
            draw_debug_collision_rectangles(board);
            draw_debug_grid();
            draw_player(player, 50, rl_v2_rectangle_wh(screen_area[0]));
            EndTextureMode();
        } while (0);

        do {
            BeginDrawing();
            ClearBackground(BLACK);
            DrawTexturePro(
                screen[0].texture,
                (Rectangle){
                    0,
                    -screen_area->y,
                    screen_area->x,
                    -screen_area->y
                },
                rl_v4_rectangle(
                    rl_fit_centered(screen_area[0], physical_area[0])
                ),
                (v2){ 0, 0 },
                0,
                WHITE
            );
            draw_debug_rotation(player.wheel_rotation);
            EndDrawing();
        } while (0);

        do {
            if (!is_paused) {
                break;
            }
            // transparent black over physical_area
            // display large text "PAUSED"
        } while (0);
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
