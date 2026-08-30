static const float spread_chance_same_building = 0.0001f;
static const float spread_chance_across_road   = spread_chance_same_building / 2;
static const float spread_growth_rate          = 0.02f;
static const float spread_max_multiplier       = 3.2f;

static float burn_multiplieriplier(int burning_ticks) {
    float multiplier = 1.0f + burning_ticks * spread_growth_rate;
    return std::min(multiplier, spread_max_multiplieriplier);
}

void update_fire_spread(Board & board) {
    auto roll = []() -> float {
        return (float) GetRandomValue(0, 1000000) / 1000000.0f;
    };

    std::vector<v2> newly_on_fire;

    for (int y = 0; y < board.height; y++) {
        for (int x = 0; x < board.width; x++) {
            const Board::Cell & cell = board.at(x, y);

            if (!cell.is_on_fire) {
                continue;
            }

            int burning_ticks = TICK - cell.lit_tick;
            float multiplier = burn_multiplieriplier(burning_ticks);

            for (int i = 0; i < Board::Cell::END; i++) {
                Board::Cell::road_e dir = (Board::Cell::road_e) i;

                int nx = x;
                int ny = y;
                switch (dir) {
                    case Board::Cell::ON_RIGHT: nx++; break;
                    case Board::Cell::ON_LEFT:  nx--; break;
                    case Board::Cell::ABOVE:    ny--; break;
                    case Board::Cell::BELOW:    ny++; break;
                    default: continue;
                }

                if (nx < 0
                ||  nx >= board.width
                ||  ny < 0
                ||  ny >= board.height) {
                    continue;
                }

                const Board::Cell & neighbor = board.at(nx, ny);
                if (neighbor.is_on_fire) continue;

                float base_chance = cell.has_road(dir)
                    ? spread_chance_across_road
                    : spread_chance_same_building
                ;

                float chance = base_chance * multiplier;

                if (roll() < chance) {
                    newly_on_fire.push_back(xy2v2(nx, ny));
                }
            }
        }
    }

    for (auto & [nx, ny] : newly_on_fire) {
        Board::Cell & c = board.at(nx, ny);
        c.is_on_fire = true;
        c.lit_tick   = TICK;
    }
}
