#include <riv.h>

// sprite IDs for each symbol
enum symbol_t
{
    SYMBOL_X = 1,
    SYMBOL_O = 2,
    SYMBOL_NONE = 999,
};

// identifiers for each type of winning line
enum game_state_t
{
    STATE_LINE_HORIZONTAL_0 = 1,
    STATE_LINE_HORIZONTAL_1 = 2,
    STATE_LINE_HORIZONTAL_2 = 3,
    STATE_LINE_VERTICAL_0 = 4,
    STATE_LINE_VERTICAL_1 = 5,
    STATE_LINE_VERTICAL_2 = 6,
    STATE_LINE_MAIN_DIAGONAL = 7,
    STATE_LINE_OPPOSITE_DIAGONAL = 8,
    STATE_OUT_OF_TILES = 9,
    STATE_ONGOING = 999,
};

// game graphics constants 
#define TARGET_FPS 30
#define SCREEN_SIZE 64
#define TILE_SIZE 16
#define TILE_MARGIN 2
#define FRAME_THICKNESS 1
#define BOARD_SIZE (3 * TILE_SIZE + 2 * (2 * TILE_MARGIN + FRAME_THICKNESS))
#define BOARD_MARGIN ((SCREEN_SIZE - BOARD_SIZE) / 2)

// game state
enum game_state_t state = STATE_ONGOING;
bool curr_player_is_circle = false;
int symbols_count = 0;
enum symbol_t tiles[3][3] = {
    {SYMBOL_NONE, SYMBOL_NONE, SYMBOL_NONE},
    {SYMBOL_NONE, SYMBOL_NONE, SYMBOL_NONE},
    {SYMBOL_NONE, SYMBOL_NONE, SYMBOL_NONE},
};

// game rendering
int sprites_id;

void setup() {
    riv->width = SCREEN_SIZE;
    riv->height = SCREEN_SIZE;
    riv->target_fps = TARGET_FPS;
    riv->tracked_keys[RIV_KEYCODE_1] = true;
    riv->tracked_keys[RIV_KEYCODE_2] = true;
    riv->tracked_keys[RIV_KEYCODE_3] = true;
    riv->tracked_keys[RIV_KEYCODE_4] = true;
    riv->tracked_keys[RIV_KEYCODE_5] = true;
    riv->tracked_keys[RIV_KEYCODE_6] = true;
    riv->tracked_keys[RIV_KEYCODE_7] = true;
    riv->tracked_keys[RIV_KEYCODE_8] = true;
    riv->tracked_keys[RIV_KEYCODE_9] = true;
    sprites_id = riv_make_spritesheet(riv_make_image("sprites.png", 255), 16, 16);
}

void draw_symbol(enum symbol_t symbol, riv_vec2i pos)
{
    int sprite_id;
    switch (symbol)
    {
        case SYMBOL_X: sprite_id = 0; break;
        case SYMBOL_O: sprite_id = 1; break;
        case SYMBOL_NONE: return;
    }
    int64_t x0 = BOARD_MARGIN + pos.x * (TILE_SIZE + TILE_MARGIN + FRAME_THICKNESS + TILE_MARGIN);
    int64_t y0 = BOARD_MARGIN + pos.y * (TILE_SIZE + TILE_MARGIN + FRAME_THICKNESS + TILE_MARGIN);
    riv_draw_sprite(sprite_id, sprites_id, x0, y0, 1, 1, 1, 1);
}

enum symbol_t get_tile_symbol(int64_t x, int64_t y)
{
    return tiles[y][x];
}

void set_tile_symbol(int64_t x, int64_t y, enum symbol_t symbol)
{
    tiles[y][x] = symbol;
}

void draw()
{
    // bg
    riv_clear(RIV_COLOR_DARKSLATE);

    // #
    {
        int64_t x0 = BOARD_MARGIN;
        int64_t w = BOARD_SIZE;
        int64_t h = FRAME_THICKNESS;
        for (int i = 0; i < 2; ++i)
        {
            int64_t y0 = BOARD_MARGIN + TILE_SIZE + TILE_MARGIN + i * (FRAME_THICKNESS + TILE_MARGIN + TILE_SIZE + TILE_MARGIN);
            riv_draw_rect_fill(x0, y0, w, h, RIV_COLOR_WHITE);
            riv_draw_rect_fill(y0, x0, h, w, RIV_COLOR_WHITE);
        }
    }

    // Xs and Os
    {
        riv_vec2i pos;
        for (pos.x = 0; pos.x < 3; ++pos.x)
        {
            for (pos.y = 0; pos.y < 3; ++pos.y)
            {
                enum symbol_t symbol = get_tile_symbol(pos.x, pos.y);
                draw_symbol(symbol, pos);
            }
        }
    }

    // winning line
    {
        if (state >= STATE_LINE_HORIZONTAL_0 && state <= STATE_LINE_HORIZONTAL_2)
        {
            int i = state - STATE_LINE_HORIZONTAL_0;
            int64_t x0 = BOARD_MARGIN;
            int64_t x1 = SCREEN_SIZE - BOARD_MARGIN;
            int64_t y = BOARD_MARGIN + TILE_SIZE / 2 + i * (TILE_SIZE + 2 * TILE_MARGIN + FRAME_THICKNESS);
            riv_draw_line(x0, y, x1, y, RIV_COLOR_RED);
        }
        else if (state >= STATE_LINE_VERTICAL_0 && state <= STATE_LINE_VERTICAL_2)
        {
            int j = state - STATE_LINE_VERTICAL_0;
            int64_t x = BOARD_MARGIN + TILE_SIZE / 2 + j * (TILE_SIZE + 2 * TILE_MARGIN + FRAME_THICKNESS);
            int64_t y0 = BOARD_MARGIN;
            int64_t y1 = SCREEN_SIZE - BOARD_MARGIN;
            riv_draw_line(x, y0, x, y1, RIV_COLOR_RED);
        }
        else if (state == STATE_LINE_MAIN_DIAGONAL)
        {
            int64_t x0 = BOARD_MARGIN;
            int64_t x1 = SCREEN_SIZE - BOARD_MARGIN;
            int64_t y0 = BOARD_MARGIN;
            int64_t y1 = SCREEN_SIZE - BOARD_MARGIN;
            riv_draw_line(x0, y0, x1, y1, RIV_COLOR_RED);
        }
        else if (state == STATE_LINE_OPPOSITE_DIAGONAL)
        {
            int64_t x0 = SCREEN_SIZE - BOARD_MARGIN;
            int64_t x1 = BOARD_MARGIN;
            int64_t y0 = BOARD_MARGIN;
            int64_t y1 = SCREEN_SIZE - BOARD_MARGIN;
            riv_draw_line(x0, y0, x1, y1, RIV_COLOR_RED);
        }
        else if (state == STATE_OUT_OF_TILES)
        {
            riv_draw_text("GAME OVER", RIV_SPRITESHEET_FONT_5X7, RIV_CENTER, SCREEN_SIZE / 2 + 1, SCREEN_SIZE / 2 + 1, 1, RIV_COLOR_DARKRED);
            riv_draw_text("GAME OVER", RIV_SPRITESHEET_FONT_5X7, RIV_CENTER, SCREEN_SIZE / 2, SCREEN_SIZE / 2, 1, RIV_COLOR_RED);
        }
    }
}

bool selected_tile(riv_vec2i* tile)
{
    for (riv_key_code keycode = RIV_KEYCODE_1; keycode <= RIV_KEYCODE_9; ++keycode)
    {
        if (riv->keys[keycode].press)
        {
            int64_t tile_index = keycode - RIV_KEYCODE_1;
            *tile = (riv_vec2i){ tile_index % 3, tile_index / 3 };
            return true;
        }
    }
    return false;
}

enum symbol_t get_curr_player_symbol()
{
    return curr_player_is_circle ? SYMBOL_O : SYMBOL_X;
}

void end_turn()
{
    curr_player_is_circle = !curr_player_is_circle;
}

enum game_state_t check_placed_symbol(int64_t x, int64_t y, enum symbol_t symbol)
{
    // horizontal
    if (get_tile_symbol(0, y) == symbol &&
        get_tile_symbol(1, y) == symbol &&
        get_tile_symbol(2, y) == symbol)
    {
        return STATE_LINE_HORIZONTAL_0 + y;
    }

    // vertical
    if (get_tile_symbol(x, 0) == symbol &&
        get_tile_symbol(x, 1) == symbol &&
        get_tile_symbol(x, 2) == symbol)
    {
        return STATE_LINE_VERTICAL_0 + x;
    }

    // main diagonal
    if (x == y)
    {
        if (get_tile_symbol(0, 0) == symbol &&
            get_tile_symbol(1, 1) == symbol &&
            get_tile_symbol(2, 2) == symbol)
        {
            return STATE_LINE_MAIN_DIAGONAL;
        }
    }

    // opposite diagonal
    if (x + y == 2)
    {
        if (get_tile_symbol(0, 2) == symbol &&
            get_tile_symbol(1, 1) == symbol &&
            get_tile_symbol(2, 0) == symbol)
        {
            return STATE_LINE_OPPOSITE_DIAGONAL;
        }
    }

    if (symbols_count == 9)
    {
        return STATE_OUT_OF_TILES;
    }

    return STATE_ONGOING;
}

struct outcard_t
{
    int score;
    char winner[2];
};

struct outcard_t create_outcard()
{
    struct outcard_t outcard;

    if (state == STATE_OUT_OF_TILES)
    {
        outcard.score = 0;
        outcard.winner[0] = '\0';
    }
    else
    {
        outcard.score = 10 - symbols_count;
        outcard.winner[0] = curr_player_is_circle ? 'O' : 'X';
        outcard.winner[1] = '\0';
    }

    return outcard;
}

void write_outcard(struct outcard_t const outcard)
{
    riv->outcard_len = riv_snprintf(
        (char*)riv->outcard,
        RIV_SIZE_OUTCARD,
        "JSON{\"score\": %d, \"winner\": \"%s\"}",
        outcard.score,
        outcard.winner);
}

void end_game()
{
    struct outcard_t outcard = create_outcard();
    write_outcard(outcard);

    riv->stop_frame = riv->frame + riv->target_fps;
}

void update()
{
    if (state == STATE_ONGOING)
    {
        riv_vec2i tile;
        if (selected_tile(&tile))
        {
            if (get_tile_symbol(tile.x, tile.y) == SYMBOL_NONE)
            {
                enum symbol_t symbol = get_curr_player_symbol();
                set_tile_symbol(tile.x, tile.y, symbol);
                ++symbols_count;

                enum game_state_t new_state = check_placed_symbol(tile.x, tile.y, symbol);
                if (new_state != STATE_ONGOING)
                {
                    state = new_state;
                    end_game();
                }
                else
                {
                    end_turn();
                }
            }
        }
    }
}

int main()
{
    setup();

    do
    {
        update();
        draw();
    }
    while (riv_present());

    return 0;
}
