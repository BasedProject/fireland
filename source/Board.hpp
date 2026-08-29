/* Tile map where each tile stores whether there is a road on each side
 *
 *            ABOVE
 *          +------+
 *          |      |
 *  ON_LEFT |      | ON_RIGHT
 *          |      |
 *          +------+
 *            BELOW
 *
 * Each tile is always a building.
 * Having parks or such that you could drive throughm would be fun, but we dont have the time.
 * Having multiple kinds of buildings (e.g. different texture, burnability, tendency to explode)
 *  would be fun, but we dont have the time.
 *
 * The way I recommend you to draw this is that each cell is identical and spaced with a gap,
 *  depending on the road status, either a road is placed to cut between two cells,
 *  or a connector texture is placed.
 * This however has edge cases, unless you create a list of buildings,
 *  and walk around each separetelly.
 * I wanted to do this for you, but I must sleepmaxx.
 *
 * For collisions,
 *  I would stick to taking the 4 corners of the truck
 *  and testing each against the road of the cell its currently in.
 * As for the actual physics, idk man.
 */
class Board {
  public:
    struct Cell {
        typedef enum {
            ON_RIGHT,
            ON_LEFT,
            ABOVE,
            BELOW,
            END
        } road_e;

        static constexpr
        bool is_horizontal(road_e i) {
            return i == ABOVE
                || i == BELOW
            ;
        }

        static constexpr
        road_e opposite(road_e i) {
            switch (i) {
                case ON_RIGHT: return ON_LEFT;
                case ON_LEFT:  return ON_RIGHT;
                case ABOVE:    return BELOW;
                case BELOW:    return ABOVE;
                default: throw out_of_range("Cell road over indexed");
            }
        }

        bool has_road_right = false;
        bool has_road_left  = false;
        bool has_road_above = false;
        bool has_road_below = false;
        bool is_on_fire     = false;

        bool & has_road(road_e i) {
            switch (i) {
                case ON_RIGHT: return has_road_right;
                case ON_LEFT:  return has_road_left;
                case ABOVE:    return has_road_above;
                case BELOW:    return has_road_below;
                default: throw out_of_range("Cell road over indexed");
            }
        }

        const bool & has_road(road_e i) const {
            switch (i) {
                case ON_RIGHT: return has_road_right;
                case ON_LEFT:  return has_road_left;
                case ABOVE:    return has_road_above;
                case BELOW:    return has_road_below;
                default: throw out_of_range("Cell road over indexed");
            }
        }
    };

    Cell * cells;
    int width;
    int height;
    int seed;

    Board(int w_, int h_, int seed_ = 0)
    : cells(new Cell[w_ * h_])
    , width(w_)
    , height(h_)
    , seed(seed_)
    {
        ;
    }

    ~Board() {
        delete[] cells;
    }

    /* Funfact: 
     *   C++ allows for operator[x, y], but its useless inside the class.
     *   You get to refer to it such as:
     *     operator[](x, y)
     *     (*this)[x, y]
     *   All of those being strictly longer than "at(x, y)".
     */

    void assert_in_bounds(int x, int y) const {
        //TraceLog(LOG_INFO, "%d %d", x, y);
        assert(x >= 0 && x < width);
        assert(y >= 0 && y < height);
    }

    Cell & at(int x, int y) {
        assert_in_bounds(x, y);
        return cells[y * width + x];
    }

    const Cell & at(int x, int y) const {
        assert_in_bounds(x, y);
        return cells[y * width + x];
    }

    void set_road(int x, int y, Cell::road_e i, bool road) {
        assert_in_bounds(x, y);

        Cell & c = at(x, y);

        c.has_road(i) = road;

        int neighbour_x = x;
        int neighbour_y = y;

        switch (i) {
            case Cell::ON_RIGHT: {
                neighbour_x = x + 1;
            } break;
            case Cell::ON_LEFT: {
                neighbour_x = x - 1;
            } break;
            case Cell::ABOVE: {
                neighbour_y = y - 1;
            } break;
            case Cell::BELOW: {
                neighbour_y = y + 1;
            } break;
            default: throw out_of_range("Cell connection over indexed");
        }

        if (neighbour_x >= 0
        &&  neighbour_x < width
        &&  neighbour_y >= 0
        &&  neighbour_y < height) {
            Cell & n = at(neighbour_x, neighbour_y);
            n.has_road(Cell::opposite(i)) = road;
        }
    }

    void set_long_road(int x1, int y1, int x2, int y2, Cell::road_e i, bool road) {
        assert_in_bounds(x1, y1);
        assert_in_bounds(x2, y2);

        if (Cell::is_horizontal(i)) {
            assert(y1 == y2);
            assert(x1 <= x2);
        } else {
            assert(x1 == x2);
            assert(y1 <= y2);
        }

        int x = x1;
        int y = y1;
        while (x <= x2 && y <= y2) {
            set_road(x, y, i, road);
            if (Cell::is_horizontal(i)) {
                ++x;
            } else {
                ++y;
            }
        }
    }
};
