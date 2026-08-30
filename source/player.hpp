typedef struct {
    v2 position;
    float facing;
    float wheel_rotation;
    float speed;
    v2 direction; // tank input
    float length_radius;
    float width_radius;
} player_t;

float float_in_tile_space(float v) {
    const int cell_diameter = block_diameter + road_diameter;
    return v / (float) cell_diameter;
}
 
v2 float_point_in_tile_space(float x, float y) {
    return (v2) {
        float_in_tile_space(x),
        float_in_tile_space(y),
    };
}
 
v2 float_point_relative_to_containing_tile(float x, float y) {
    const int cell_diameter = block_diameter + road_diameter;
    v2 tile = float_point_in_tile_space(x, y);
    return (v2) {
        (tile.x - floorf(tile.x)) * (float) cell_diameter,
        (tile.y - floorf(tile.y)) * (float) cell_diameter,
    };
}
 
void get_collision_rectangles(const Board & board, int x, int y, bool * r) {
    // The map wraps, so a tile index can legitimately be negative or
    // past width/height (e.g. a corner point that's crossed the wrap
    // seam) -- wrap it into range rather than reading out of bounds.
    const int w = board.width;
    const int h = board.height;
    const int wx = ((x % w) + w) % w;
    const int wy = ((y % h) + h) % h;
 
    const Board::Cell & cell = board.at(wx, wy);
 
    const bool road_above = cell.has_road_above;
    const bool road_below = cell.has_road_below;
    const bool road_left  = cell.has_road_left;
    const bool road_right = cell.has_road_right;
 
    r[0] = !road_above && !road_left;   // top-left corner
    r[1] = !road_above;                 // top edge
    r[2] = !road_above && !road_right;  // top-right corner
    r[3] = !road_left;                  // left edge
    r[4] = true;                        // center (the block)
    r[5] = !road_right;                 // right edge
    r[6] = !road_below && !road_left;   // bottom-left corner
    r[7] = !road_below;                 // bottom edge
    r[8] = !road_below && !road_right;  // bottom-right corner
}

const Rectangle collision_rectangles[9] = {
    [0] = (Rectangle) {
        0,
        0,
        road_radius,
        road_radius,
    },
    [1] = (Rectangle) {
        road_radius,
        0,
        block_diameter,
        road_radius,
    },
    [2] = (Rectangle) {
        road_radius + block_diameter,
        0,
        road_radius,
        road_radius,
    },
    // ---
    [3] = (Rectangle) {
        0,
        road_radius,
        road_radius,
        road_radius,
    },
    [4] = (Rectangle) {
        road_radius,
        road_radius,
        block_diameter,
        road_radius,
    },
    [5] = (Rectangle) {
        road_radius + block_diameter,
        road_radius,
        road_radius,
        road_radius,
    },
    // ---
    [6] = (Rectangle) {
        0,
        road_radius + block_diameter,
        road_radius,
        road_radius,
    },
    [7] = (Rectangle) {
        road_radius,
        road_radius + block_diameter,
        block_diameter,
        road_radius,
    },
    [8] = (Rectangle) {
        road_radius + block_diameter,
        road_radius + block_diameter,
        road_radius,
        road_radius,
    },
};

void update_player(player_t & player, Rectangle screen_shape) {
    int key[4] = { KEY_W, KEY_S, KEY_A, KEY_D }; //nswe
    const float dt = 1.0f / TICKS_PER_SECOND;
    #define nullcancel(axis, prograde, retrograde) do \
    { if (IsKeyPressed (prograde)                                 ) player.direction.axis = 1; \
      if (IsKeyReleased(prograde)   && player.direction.axis > 0) player.direction.axis = IsKeyDown(retrograde) ? -1 : 0; \
      if (IsKeyPressed (retrograde)                               ) player.direction.axis = -1; \
      if (IsKeyReleased(retrograde) && player.direction.axis < 0) player.direction.axis = IsKeyDown(prograde) ? 1 : 0; \
    } while (0);
    #define polar(north, south, west, east) do \
      { nullcancel(x, east, west);             \
        nullcancel(y, north, south);           \
      } while (0);
    polar(key[0], key[1], key[2], key[3]);
    const float wheel_max_deviation  = 0.6f;    // radians, max wheel offset from facing
    const float steer_rate           = 3.0f;    // rad/sec turning the wheel at zero speed
    const float steer_falloff        = 0.003f;  // how quickly steering sensitivity drops off with speed
    const float facing_converge_rate = 4.0f;    // how fast facing chases the wheel direction

    const float max_speed_forward  = 350.0f;     // units/sec
    const float max_speed_reverse  = 180.0f;     // units/sec, reversing is slower
    const float accel_rate_forward =  70.0f;     // units/sec^2, accelerating forward from rest or while already forward
    const float accel_rate_reverse =  40.0f;     // units/sec^2, accelerating backward from rest or while already reversing
    const float brake_rate         = 300.0f;     // units/sec^2, input opposing current motion (braking)
    const float friction           = 180.0f;     // units/sec^2, decel with no input at all

    const float collision_penalty = 0.8f; // speed penalty for frontal collisions

    do {
        float steer_sensitivity = 1.0f / (1.0f + fabsf(player.speed) * steer_falloff);
        player.wheel_rotation  += player.direction.x * steer_rate * steer_sensitivity * dt;
        player.wheel_rotation   = clamp(player.wheel_rotation, -wheel_max_deviation, wheel_max_deviation);
    } while (0);

    do {
        bool has_input = player.direction.y != 0;
        if (has_input) {
            bool braking = (player.direction.y > 0 && player.speed < 0)
                         || (player.direction.y < 0 && player.speed > 0);
            float rate;
            if      (braking)                rate = brake_rate;
            else if (player.direction.y > 0) rate = accel_rate_forward;
            else                             rate = accel_rate_reverse;
            player.speed += player.direction.y * rate * dt;
        } else {
            float decel = friction * dt;
            if (fabsf(player.speed) <= decel) player.speed = 0.0f;
            else                              player.speed -= (player.speed > 0 ? decel : -decel);
        }
        player.speed = clamp(player.speed, -max_speed_reverse, max_speed_forward);
    } while (0);

    do {
        float travel_direction = player.facing; // + player.wheel_rotation;
        v2 candidate_position;
        candidate_position.x = player.position.x + cosf(travel_direction) * player.speed * dt;
        candidate_position.y = player.position.y + sinf(travel_direction) * player.speed * dt;

        v2 player_tile_position = float_point_in_tile_space(player.position.x, player.position.y);
        bool has_collision_rectangle[9] = {0};
     
        const float c = cosf(player.facing);
        const float s = sinf(player.facing);

        const v2 forward = {  c, s };
        const v2 right   = { -s, c };

        const v2 center = float_point_in_tile_space(candidate_position.x, candidate_position.y);

        const v2 front_left  = center + forward * player.length_radius - right * player.width_radius;
        const v2 front_right = center + forward * player.length_radius + right * player.width_radius;
        const v2 rear_left   = center - forward * player.length_radius - right * player.width_radius;
        const v2 rear_right  = center - forward * player.length_radius + right * player.width_radius;

        v4 front_bumper = (v4) {
            front_left.x,
            front_left.y,
            front_right.x,
            front_right.y,
        };

        v4 rear_bumper = (v4) {
            rear_left.x,
            rear_left.y,
            rear_right.x,
            rear_right.y,
        };

        v4 left_side = (v4) {
            front_left.x,
            front_left.y,
            rear_left.x,
            rear_left.y,
        };

        v4 right_side = (v4) {
            front_right.x,
            front_right.y,
            rear_right.x,
            rear_right.y,
        };

        // frontal collision
        do {
            break;
            bool has_collided = false;

            for (int r = 0; r < 9; r++) {
                if (!has_collision_rectangle[r]) {
                    continue;
                }

                if (CheckCollisionSegmentRect(front_bumper, collision_rectangles[r])
                ||  CheckCollisionSegmentRect(rear_bumper, collision_rectangles[r])) {
                    has_collided = true;
                    break;
                }
            }

            if (has_collided) {
                player.wheel_rotation = 0;
                player.direction = (v2) { -player.direction.x, -player.direction.y };
                player.speed *= collision_penalty;
                goto out;
            }
        } while (0);

        // side collision
        do {
            // XXX
        } while (0);

        player.position = candidate_position;
    } while (0);
    out: // XXX

    do {
        if (player.speed == 0) {
            break;
        }

        float facing_step = player.wheel_rotation * facing_converge_rate * dt;
        if (fabsf(facing_step) > fabsf(player.wheel_rotation)) {
            facing_step = player.wheel_rotation;
        }
        player.facing         += facing_step;
        player.wheel_rotation -= facing_step;
    } while (0);

    // map wrapping
    wrap(&player.position, &screen_shape);
    #undef nullcancel
    #undef polar
}
