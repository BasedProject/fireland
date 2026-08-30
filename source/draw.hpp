v2 get_board_display_size(const Board & board) {
    return xy2v2(
        board.width  * cell_diameter,
        board.height * cell_diameter
    );
}

void draw_board(const Board & board) {
    const int half_road = road_diameter / 2;

    v2 display_size = get_board_display_size(board);
    Rectangle background = (Rectangle) {
        0,
        0,
        display_size.x,
        display_size.y
    };

    DrawRectangleRec(background, DARKGRAY);

    for (int y = 0; y < board.height; y++) {
        for (int x = 0; x < board.width; x++) {
            const Board::Cell & cell = board.at(x, y);

            const int px = half_road + x * cell_diameter;
            const int py = half_road + y * cell_diameter;

            DrawRectangle(
                px,
                py,
                block_diameter,
                block_diameter,
                DARKGRAY
            );

            const bool at_left_edge   = (x == 0);
            const bool at_right_edge  = (x == board.width  - 1);
            const bool at_top_edge    = (y == 0);
            const bool at_bottom_edge = (y == board.height - 1);

            if (cell.has_road_left) {
                const int width_      = at_left_edge   ? half_road : road_diameter;
                const int ext_top     = at_top_edge    ? half_road : road_diameter;
                const int ext_bottom  = at_bottom_edge ? half_road : road_diameter;
                DrawRectangle(
                    px - width_,
                    py - ext_top,
                    width_,
                    block_diameter + ext_top + ext_bottom,
                    WHITE
                );
            }

            if (cell.has_road_right) {
                const int width_      = at_right_edge  ? half_road : road_diameter;
                const int ext_top     = at_top_edge    ? half_road : road_diameter;
                const int ext_bottom  = at_bottom_edge ? half_road : road_diameter;
                DrawRectangle(
                    px + block_diameter,
                    py - ext_top,
                    width_,
                    block_diameter + ext_top + ext_bottom,
                    WHITE
                );
            }

            if (cell.has_road_above) {
                const int height_     = at_top_edge   ? half_road : road_diameter;
                const int ext_left    = at_left_edge  ? half_road : road_diameter;
                const int ext_right   = at_right_edge ? half_road : road_diameter;
                DrawRectangle(
                    px - ext_left,
                    py - height_,
                    block_diameter + ext_left + ext_right,
                    height_,
                    WHITE
                );
            }

            if (cell.has_road_below) {
                const int height_     = at_bottom_edge ? half_road : road_diameter;
                const int ext_left    = at_left_edge   ? half_road : road_diameter;
                const int ext_right   = at_right_edge  ? half_road : road_diameter;
                DrawRectangle(
                    px - ext_left,
                    py + block_diameter,
                    block_diameter + ext_left + ext_right,
                    height_,
                    WHITE
                );
            }
        }
    }
}

void draw_debug_grid(void) {
    const int cell_diameter = block_diameter + road_diameter;
    const int screen_w = GetScreenWidth();
    const int screen_h = GetScreenHeight();
    const int cols = screen_w / cell_diameter + 1;
    const int rows = screen_h / cell_diameter + 1;
 
    // vertical lines, at each cell/wrap-seam boundary
    for (int x = 0; x <= cols; x++) {
        int line_x = x * cell_diameter;
        DrawLine(line_x, 0, line_x, screen_h, GREEN);
    }
    // horizontal lines, at each cell/wrap-seam boundary
    for (int y = 0; y <= rows; y++) {
        int line_y = y * cell_diameter;
        DrawLine(0, line_y, screen_w, line_y, GREEN);
    }
 
    // number each cell
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            int px = x * cell_diameter;
            int py = y * cell_diameter;
            DrawText(TextFormat("%d,%d", x, y), px + 2, py + 2, 10, GREEN);
        }
    }
}

void draw_debug_fire(const Board & board) {
    const int half_road = road_diameter / 2;
    Color fire_overlay = (Color) { 255, 0, 0, 100 };
    for (int y = 0; y < board.height; y++) {
        for (int x = 0; x < board.width; x++) {
            const Board::Cell & cell = board.at(x, y);
            if (cell.is_on_fire) {
                const int px = half_road + x * cell_diameter;
                const int py = half_road + y * cell_diameter;
                DrawRectangle(
                    px,
                    py,
                    block_diameter,
                    block_diameter,
                    fire_overlay
                );
            }
        }
    }
}
void draw_player(player_t & player, float diameter, Rectangle screen_shape) {
    constexpr float length = 20.0f;
    constexpr float width = 6.0f;

    Vector2 center = {
        player.position.x,
        player.position.y
    };

    DrawRectanglePro(
        (Rectangle) {
            center.x,
            center.y,
            length,
            width
        },
        (Vector2) {
            length / 2.0f,
            width / 2.0f
        },
        player.facing * RAD2DEG,
        RED
    );
}

void draw_debug_rotation(float rotation) {
    constexpr float radius = 30.0f;
    constexpr float outline = 2.0f;

    const Vector2 center = {
        radius + 10.0f,
        radius + 10.0f
    };

    // Circle
    DrawCircleV(center, radius, WHITE);
    DrawCircleLinesV(center, radius, BLACK);

    // Rotating direction rectangle
    constexpr float rect_width = 6.0f;
    constexpr float rect_length = 20.0f;

    // raylib's DrawRectanglePro is convenient for this.
    Rectangle rect = {
        center.x,
        center.y,
        rect_length,
        rect_width
    };

    Vector2 origin = {
        rect_length * 0.5f,
        rect_width * 0.5f
    };

    DrawRectanglePro(
        rect,
        origin,
        rotation * RAD2DEG,
        BLACK
    );

    // Slightly smaller white rectangle gives it a black outline.
    rect.width -= outline * 2.0f;
    rect.height -= outline * 2.0f;

    origin = {
        rect.width * 0.5f,
        rect.height * 0.5f
    };

    DrawRectanglePro(
        rect,
        origin,
        rotation * RAD2DEG,
        WHITE
    );
}

void draw_debug_collision_rectangles(const Board & board) {
    for (int y = 0; y < board.height; y++) {
        for (int x = 0; x < board.width; x++) {
            bool has_collision_rectangle[9];
            get_collision_rectangles(board, x, y, has_collision_rectangle);
 
            const int tile_origin_x = x * cell_diameter;
            const int tile_origin_y = y * cell_diameter;
 
            for (int r = 0; r < 9; r++) {
                if (!has_collision_rectangle[r]) {
                    continue;
                }
 
                Rectangle world_rect = (Rectangle) {
                    tile_origin_x + collision_rectangles[r].x,
                    tile_origin_y + collision_rectangles[r].y,
                    collision_rectangles[r].width,
                    collision_rectangles[r].height,
                };
                DrawRectangleLinesEx(world_rect, 1, GREEN);
            }
        }
    }
}
