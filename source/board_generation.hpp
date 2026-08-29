#pragma once

#include <cstdlib>
#include <climits>
#include <vector>
#include <algorithm>

// This header assumes a unity build where Board.hpp (declaring `class Board`,
// its `Cell` type, `cells`/`width`/`height`/`seed`, `at()`, `assert_in_bounds()`,
// `set_road()` and `set_long_road()`) has already been included. Only
// set_road()/set_long_road() remain members of Board itself - everything
// else here is a free function that generates a road network and building
// layout onto an existing Board instance.

// A leaf block produced by BSP subdivision, inclusive coordinates.
struct Rect { int x1, y1, x2, y2; };

// Tuning knobs for random_network()/randomize_board().
struct GenerationParams {
    // BSP subdivision
    int min_block_size = 1;             // smallest allowed block dimension, in cells
    int max_block_size = 5;       // hard cap on a leaf's width/height - see partition_into_chunks()
    float stop_probability = 0.21f;     // chance to stop subdividing a region early, for size variety
    float split_position_jitter = 0.6f; // fraction of the splittable range the cut is drawn from (centered)

    // Lot merging (produces L/T/Z/U-shaped buildings)
    float merge_probability = 0.35f;  // chance a given leaf attempts a merge
    int max_merges_per_building = 3;  // cap on how many leaves can fuse into one building
};

// Maps board cells to merged-building ids, returned by random_network() so
// callers can query which building a cell belongs to after generation.
struct BuildingMap {
    int width;
    std::vector<int> cell_leaf;  // width*height, which leaf index each cell belongs to
    std::vector<int> lot_parent; // union-find over leaves, post path-compression

    int lot_root(int i) {
        while (lot_parent[i] != i) {
            lot_parent[i] = lot_parent[lot_parent[i]]; // path compression
            i = lot_parent[i];
        }
        return i;
    }

    // Which merged building (x, y) belongs to, as a stable id (a leaf index).
    int building_id(int x, int y) {
        return lot_root(cell_leaf[y * width + x]);
    }
};

// set_road() and set_long_road() remain member functions on Board itself
// (declared in Board.hpp) - everything below calls board.set_road(...) /
// board.set_long_road(...) rather than defining free versions here.

// free roads around the whole map
inline void set_main_donut_road(Board &board, bool road) {
    board.set_long_road(0, 0, 0, board.height-1, Board::Cell::ON_LEFT, road);
    board.set_long_road(board.width-1, 0, board.width-1, board.height-1, Board::Cell::ON_RIGHT, road);
    board.set_long_road(0, 0, board.width-1, 0, Board::Cell::ABOVE, road);
    board.set_long_road(0, board.height-1, board.width-1, board.height-1, Board::Cell::BELOW, road);
}

// Recursively subdivides the rectangle [x1,x2] x [y1,y2] (inclusive) into
// smaller rectangular blocks, cutting a full road across the region on each
// split. Every leaf block ends up bounded by road on all four sides by
// construction. Leaves are recorded into `leaves` for the merge pass.
inline void subdivide(Board &board, const GenerationParams &params, std::vector<Rect> &leaves,
                       int x1, int y1, int x2, int y2) {
    int w = x2 - x1 + 1;
    int h = y2 - y1 + 1;

    bool can_split_vertical   = (w >= 2 * params.min_block_size); // cut along x, road spans full height
    bool can_split_horizontal = (h >= 2 * params.min_block_size); // cut along y, road spans full width

    if (!can_split_vertical && !can_split_horizontal) {
        leaves.push_back({x1, y1, x2, y2}); // too small to cut further - it's a leaf block
        return;
    }

    if (((float)rand() / (float)RAND_MAX) < params.stop_probability) {
        leaves.push_back({x1, y1, x2, y2}); // deliberately kept as one (larger) block
        return;
    }

    bool split_vertical;
    if (can_split_vertical && can_split_horizontal) {
        // bias toward cutting the longer axis, so blocks trend squarish
        float p_vertical = (float)w / (float)(w + h);
        split_vertical = ((float)rand() / (float)RAND_MAX) < p_vertical;
    } else {
        split_vertical = can_split_vertical;
    }

    if (split_vertical) {
        int lo = x1 + params.min_block_size - 1;
        int hi = x2 - params.min_block_size;
        int mid = (lo + hi) / 2;
        int half_range = (int)((hi - lo) * params.split_position_jitter / 2.0f);
        int cut = mid + (half_range > 0 ? (rand() % (2 * half_range + 1) - half_range) : 0);
        if (cut < lo) cut = lo;
        if (cut > hi) cut = hi;

        board.set_long_road(cut, y1, cut, y2, Board::Cell::ON_RIGHT, true);

        subdivide(board, params, leaves, x1, y1, cut, y2);
        subdivide(board, params, leaves, cut + 1, y1, x2, y2);
    } else {
        int lo = y1 + params.min_block_size - 1;
        int hi = y2 - params.min_block_size;
        int mid = (lo + hi) / 2;
        int half_range = (int)((hi - lo) * params.split_position_jitter / 2.0f);
        int cut = mid + (half_range > 0 ? (rand() % (2 * half_range + 1) - half_range) : 0);
        if (cut < lo) cut = lo;
        if (cut > hi) cut = hi;

        board.set_long_road(x1, cut, x2, cut, Board::Cell::BELOW, true);

        subdivide(board, params, leaves, x1, y1, x2, cut);
        subdivide(board, params, leaves, x1, cut + 1, x2, y2);
    }
}

// --- union-find over leaves, for tracking which leaves merged into one building ---

inline int lot_root(std::vector<int> &lot_parent, int i) {
    while (lot_parent[i] != i) {
        lot_parent[i] = lot_parent[lot_parent[i]]; // path compression
        i = lot_parent[i];
    }
    return i;
}

inline bool lot_union(std::vector<int> &lot_parent, std::vector<int> &lot_size,
                       int max_merges_per_building, int a, int b) {
    int ra = lot_root(lot_parent, a), rb = lot_root(lot_parent, b);
    if (ra == rb) return false;
    if (lot_size[ra] + lot_size[rb] > max_merges_per_building + 1) return false;
    lot_parent[rb] = ra;
    lot_size[ra] += lot_size[rb];
    return true;
}

// leaves tile the board exactly, so this is a direct fill, no flood-fill needed
inline std::vector<int> build_cell_leaf_map(const Board &board, const std::vector<Rect> &leaves) {
    std::vector<int> cell_leaf(board.width * board.height, -1);
    for (size_t li = 0; li < leaves.size(); ++li) {
        const Rect &r = leaves[li];
        for (int y = r.y1; y <= r.y2; ++y)
            for (int x = r.x1; x <= r.x2; ++x)
                cell_leaf[y * board.width + x] = (int)li;
    }
    return cell_leaf;
}

// Merges pairs of adjacent leaves whose shared border is only a *partial*
// overlap (offset, not flush). That offset is what turns the union of two
// rectangles into a concave (L/T/Z/U-shaped) footprint instead of a bigger
// plain rectangle. A leaf can chain into more than one merge (up to
// max_merges_per_building), producing richer shapes than a plain L.
inline void merge_lots(Board &board, const GenerationParams &params, const std::vector<Rect> &leaves,
                        std::vector<int> &lot_parent, std::vector<int> &lot_size) {
    std::vector<int> order(leaves.size());
    for (size_t i = 0; i < order.size(); ++i) order[i] = (int)i;
    for (size_t i = order.size(); i > 1; --i) {
        std::swap(order[i-1], order[rand() % i]);
    }

    for (int i : order) {
        if (((float)rand() / (float)RAND_MAX) >= params.merge_probability) continue;

        const Rect &a = leaves[i];
        struct Candidate { int j; int lo, hi; Board::Cell::road_e dir; };
        std::vector<Candidate> candidates;

        for (int j = 0; j < (int)leaves.size(); ++j) {
            if (j == i) continue;
            const Rect &b = leaves[j];

            if (a.x2 + 1 == b.x1) { // b is to the right of a
                int lo = std::max(a.y1, b.y1), hi = std::min(a.y2, b.y2);
                if (lo <= hi && !(a.y1 == b.y1 && a.y2 == b.y2))
                    candidates.push_back({j, lo, hi, Board::Cell::ON_RIGHT});
            } else if (a.x1 - 1 == b.x2) { // b is to the left of a
                int lo = std::max(a.y1, b.y1), hi = std::min(a.y2, b.y2);
                if (lo <= hi && !(a.y1 == b.y1 && a.y2 == b.y2))
                    candidates.push_back({j, lo, hi, Board::Cell::ON_LEFT});
            } else if (a.y2 + 1 == b.y1) { // b is below a
                int lo = std::max(a.x1, b.x1), hi = std::min(a.x2, b.x2);
                if (lo <= hi && !(a.x1 == b.x1 && a.x2 == b.x2))
                    candidates.push_back({j, lo, hi, Board::Cell::BELOW});
            } else if (a.y1 - 1 == b.y2) { // b is above a
                int lo = std::max(a.x1, b.x1), hi = std::min(a.x2, b.x2);
                if (lo <= hi && !(a.x1 == b.x1 && a.x2 == b.x2))
                    candidates.push_back({j, lo, hi, Board::Cell::ABOVE});
            }
        }

        if (candidates.empty()) continue;
        const Candidate &c = candidates[rand() % candidates.size()];

        if (!lot_union(lot_parent, lot_size, params.max_merges_per_building, i, c.j)) continue;

        // clear the road only along the overlapping segment
        if (Board::Cell::is_horizontal(c.dir)) {
            for (int x = c.lo; x <= c.hi; ++x) {
                int y = (c.dir == Board::Cell::BELOW) ? a.y2 : a.y1;
                board.set_road(x, y, c.dir, false);
            }
        } else {
            for (int y = c.lo; y <= c.hi; ++y) {
                int x = (c.dir == Board::Cell::ON_RIGHT) ? a.x2 : a.x1;
                board.set_road(x, y, c.dir, false);
            }
        }
    }
}

// Splits [0, length) into as-even-as-possible chunks, none wider than
// max_size. Returns the chunk boundary positions: n+1 values, bounds[0] == 0
// and bounds[n] == length, chunk i spans [bounds[i], bounds[i+1]).
inline std::vector<int> chunk_boundaries(int length, int max_size) {
    // max_size >= length means "no real cap needed" - treat as a single
    // chunk. This also guards against overflow: with the default
    // max_size == INT_MAX, `length + max_size - 1` would overflow an int
    // and (via UB wraparound) divide by zero below.
    int n = (max_size <= 0 || max_size >= length) ? 1 : (length + max_size - 1) / max_size;
    int base = length / n;
    int remainder = length % n;

    std::vector<int> bounds;
    bounds.push_back(0);
    int pos = 0;
    for (int i = 0; i < n; ++i) {
        pos += base + (i < remainder ? 1 : 0); // spread the remainder over the first few chunks
        bounds.push_back(pos);
    }
    return bounds;
}

// subdivide()'s stop_probability means a leaf can randomly stop splitting
// while still larger than desired - it bounds the *minimum* block size, not
// the maximum. To get a hard maximum too, first carve the board into a grid
// of chunks no wider/taller than params.max_block_size (with internal roads
// between them), then run subdivide() independently inside each chunk. Every
// leaf is then guaranteed <= max_block_size, since that's the worst case if
// a chunk never subdivides further at all.
inline void partition_into_chunks(Board &board, const GenerationParams &params, std::vector<Rect> &leaves) {
    std::vector<int> xb = chunk_boundaries(board.width, params.max_block_size);
    std::vector<int> yb = chunk_boundaries(board.height, params.max_block_size);

    // internal vertical roads between column chunks, full board height
    for (size_t i = 1; i + 1 < xb.size(); ++i) {
        int cut = xb[i] - 1;
        board.set_long_road(cut, 0, cut, board.height - 1, Board::Cell::ON_RIGHT, true);
    }
    // internal horizontal roads between row chunks, full board width
    for (size_t j = 1; j + 1 < yb.size(); ++j) {
        int cut = yb[j] - 1;
        board.set_long_road(0, cut, board.width - 1, cut, Board::Cell::BELOW, true);
    }

    for (size_t j = 0; j + 1 < yb.size(); ++j) {
        for (size_t i = 0; i + 1 < xb.size(); ++i) {
            subdivide(board, params, leaves, xb[i], yb[j], xb[i+1] - 1, yb[j+1] - 1);
        }
    }
}

// Carves a city-block road network into `board` via a chunk grid capped at
// params.max_block_size, BSP-subdivided within each chunk, then merges some
// adjacent blocks into non-rectangular (L/T/Z/U-shaped) buildings. Returns a
// BuildingMap so callers can look up which building each cell belongs to.
inline BuildingMap random_network(Board &board, const GenerationParams &params = {}) {
    std::vector<Rect> leaves;
    partition_into_chunks(board, params, leaves);

    std::vector<int> lot_parent(leaves.size());
    std::vector<int> lot_size(leaves.size(), 1);
    for (size_t i = 0; i < leaves.size(); ++i) lot_parent[i] = (int)i;

    std::vector<int> cell_leaf = build_cell_leaf_map(board, leaves);

    merge_lots(board, params, leaves, lot_parent, lot_size);

    return BuildingMap{board.width, std::move(cell_leaf), std::move(lot_parent)};
}

// Resets `board` to a fresh randomly generated city layout: seeds the RNG,
// lays the boundary road, then generates blocks/buildings via random_network().
inline BuildingMap randomize_board(Board &board) {
    srand(board.seed);
    set_main_donut_road(board, true);
    return random_network(board);
}
