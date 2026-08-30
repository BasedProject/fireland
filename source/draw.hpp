void draw_board(const Board & board) {
    const int cell_diameter = block_diameter + road_diameter;

    Rectangle background = (Rectangle) {
        0,
        0,
        board.width * block_diameter +
            (board.width + 1) * road_diameter,
        board.height * block_diameter +
            (board.height + 1) * road_diameter,
    };

    DrawRectangleRec(background, DARKGRAY);

    for (int y = 0; y < board.height; y++) {
        for (int x = 0; x < board.width; x++) {
            const Board::Cell & cell = board.at(x, y);

            const int px = road_diameter + x * cell_diameter;
            const int py = road_diameter + y * cell_diameter;

            // Draw the block.
            DrawRectangle(
                px,
                py,
                block_diameter,
                block_diameter,
                DARKGRAY
            );

            // No connection means there is a road on that side.
            // Roads extend the full block width/height so that they
            // overlap at intersections.

            if (cell.has_road_left) {
                DrawRectangle(
                    px - road_diameter,
                    py - road_diameter,
                    road_diameter,
                    block_diameter + 2 * road_diameter,
                    WHITE
                );
            }

            if (cell.has_road_right) {
                DrawRectangle(
                    px + block_diameter,
                    py - road_diameter,
                    road_diameter,
                    block_diameter + 2 * road_diameter,
                    WHITE
                );
            }

            if (cell.has_road_above) {
                DrawRectangle(
                    px - road_diameter,
                    py - road_diameter,
                    block_diameter + 2 * road_diameter,
                    road_diameter,
                    WHITE
                );
            }

            if (cell.has_road_below) {
                DrawRectangle(
                    px - road_diameter,
                    py + block_diameter,
                    block_diameter + 2 * road_diameter,
                    road_diameter,
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

    // vertical lines, through the middle of each road
    for (int x = 0; x <= cols; x++) {
        int line_x = road_diameter / 2 + x * cell_diameter;
        DrawLine(line_x, 0, line_x, screen_h, GREEN);
    }
    // horizontal lines, through the middle of each road
    for (int y = 0; y <= rows; y++) {
        int line_y = road_diameter / 2 + y * cell_diameter;
        DrawLine(0, line_y, screen_w, line_y, GREEN);
    }

    // number each cell
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            int px = road_diameter / 2 + x * cell_diameter;
            int py = road_diameter / 2 + y * cell_diameter;
            DrawText(TextFormat("%d,%d", x, y), px + 2, py + 2, 10, GREEN);
        }
    }
}

void draw_debug_fire(const Board & board) {
    const int cell_diameter = block_diameter + road_diameter;
    Color fire_overlay = (Color) { 255, 0, 0, 100 };
    for (int y = 0; y < board.height; y++) {
        for (int x = 0; x < board.width; x++) {
            const Board::Cell & cell = board.at(x, y);
            if (cell.is_on_fire) {
                const int px = road_diameter + x * cell_diameter;
                const int py = road_diameter + y * cell_diameter;
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
