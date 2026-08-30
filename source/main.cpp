#include <assert.h>
#include <stddef.h>
#include <vector>
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

// ripped from meat
#define die(...)                  \
  do {                            \
    warn(__VA_ARGS__);            \
    exit(1);                      \
  } while (0)

#define warn(...) fprintf(stderr, __VA_ARGS__)

typedef RenderTexture2D rl_screen;

v4 rl_fit_centered(v2 fit, v2 inside)
{ float scale =
    inside.x / fit.x < inside.y / fit.y
    ? inside.x / fit.x
    : inside.y / fit.y;
  float width = fit.x * scale;
  float height = fit.y * scale;
  return
    (v4){ (inside.x - width) * 0.5f,
          (inside.y - height) * 0.5f,
          width, height };
}

Rectangle rl_v4_rectangle(v4 v)
{ return (Rectangle) { v.x, v.y, v.z, v.w }; }

void rl_init_window_v2(const v2 area_maybe, const char * name)
{ InitWindow(area_maybe.x, area_maybe.y, name); }

bool rl_init_window2(const v2 area_maybe, const char * name)
{ rl_init_window_v2(area_maybe, name);
  return IsWindowReady();
}

v2 rl_get_render_area(void)
{ return (v2) {(float)GetRenderWidth(), (float)GetRenderHeight() }; }

rl_screen rl_load_render_texture_v2(const v2 area)
{ return LoadRenderTexture(area.x, area.y); }

bool rl_init_audio_device2(void)
{ InitAudioDevice();
  return IsAudioDeviceReady();
}

Rectangle rl_screen_shape(rl_screen * screen)
{ return (Rectangle){ 0, 0, (float)screen->texture.width, (float)-screen->texture.height };
}

void meat_init(char * program_name, rl_screen * screen, v2 * screen_area, v2 * physical_area)
{   screen_area[0] = (v2) {1920, 1080 };

    SetTraceLogLevel(LOG_ERROR);
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_RESIZABLE);

    if (rl_init_window2(screen_area[0], program_name))
    { atexit(CloseWindow);
      /* rl_toggle_fullscreen(); */
    }
    else
    { die("No Window :(\n");
    }

    if (rl_init_audio_device2())
    { atexit(CloseAudioDevice);
      SetMasterVolume(0.2f);
    }
    else
    { warn("No Audio :(\n");
    }

    screen[0] = rl_load_render_texture_v2(screen_area[0]);
    if (IsTextureValid(screen->texture))
    { // ...
    }
    else
    { die("No Render Texture :(\n");
    }
}
// ---


int main(int ac, char ** av)
{   rl_screen screen[1];
    v2 screen_area[1], physical_area[1];
    // Texture texture[TEXTURE_END];
    // Sound sound[SOUND_END];

    meat_init(av[0], screen, screen_area, physical_area);

    Board board = Board(12, 8);
    randomize_board(board);

    while (!WindowShouldClose()) {
      physical_area[0] = rl_get_render_area();
      {   BeginTextureMode(screen[0]);
          draw_board(board);
          EndTextureMode();
      }
      {   BeginDrawing();
          DrawTexturePro(screen->texture,
                         rl_screen_shape(screen),
                         rl_v4_rectangle(rl_fit_centered(screen_area[0], physical_area[0])),
                         (v2){ 0, 0 },
                         0,
                         WHITE);
          ClearBackground(BLACK);
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
