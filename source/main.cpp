#include <assert.h>
#include <stddef.h>
#include <vector>
#include <numeric>
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

int main(void) {
    InitWindow(W, H, "Raylib - Hello World");
    SetTargetFPS(60);

    Board board = Board(12, 8);
    randomize_board(board);
    random_fires(board, 3);

    while (!WindowShouldClose()) {
      BeginDrawing();
        ClearBackground(BLACK);
        DrawText("Hello, world!", 190, 200, 20, DARKGRAY);
        draw_board(board);
        draw_debug_fire(board);
        draw_debug_grid();
      EndDrawing();
    }

    CloseWindow();
    return 0;
}
