// stuff ripped from prior projects

// Meat
#define die(...)                  \
  do {                            \
    warn(__VA_ARGS__);            \
    exit(1);                      \
  } while (0)

#define warn(...) fprintf(stderr, __VA_ARGS__)

v4 rl_fit_centered(v2 fit, v2 inside) {
    float scale = inside.x / fit.x < inside.y / fit.y
        ? inside.x / fit.x
        : inside.y / fit.y
    ;
    float width  = fit.x * scale;
    float height = fit.y * scale;
    return (v4){
        (inside.x -  width) * 0.5f,
        (inside.y - height) * 0.5f,
        width,
        height,
    };
}

Rectangle rl_v4_rectangle(v4 v)
{ return (Rectangle) { v.x, v.y, v.z, v.w }; }

Rectangle rl_v2_rectangle_wh(v2 v)
{ return (Rectangle) { 0, 0, v.x, v.y }; }

Rectangle rl_v2_rectangle_xy(v2 v)
{ return (Rectangle) { v.x, v.y, 0, 0 }; }

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
{   SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_RESIZABLE);

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

    physical_area[0] = rl_get_render_area();
}
// ---

// Acelata
void
DrawCentered(Texture * texture, Rectangle source, Rectangle dest, float degrees, Color color)
{ Vector2 origin = { dest.width / 2.0f, dest.height / 2.0f };
  DrawTexturePro(*texture, source, dest, origin, degrees, color);
}

Vector2
WrapOffset(Vector2 center, float hw, float hh, Rectangle area)
{ float ox = center.x - hw < area.x             ? area.width
           : center.x + hw > area.x + area.width ? -area.width
           : 0;
  float oy = center.y - hh < area.y              ? area.height
           : center.y + hh > area.y + area.height ? -area.height
           : 0;
  return (Vector2){ ox, oy };
}

void
DrawCenteredWrapped(Texture * texture, Rectangle source, Rectangle dest, Rectangle area, float degrees, Color color)
{ float rad = degrees * (PI / 180.0f);
  float c  = fabsf(cosf(rad));
  float s  = fabsf(sinf(rad));
  float hw = dest.width  / 2.0f;
  float hh = dest.height / 2.0f;
  float mx = hw * c + hh * s;   /* rotated half-width  */
  float my = hw * s + hh * c;   /* rotated half-height */
  Vector2 o = WrapOffset((Vector2){ dest.x, dest.y }, mx, my, area);

                  DrawCentered(texture, source, dest, degrees, color);
  if (o.x)        DrawCentered(texture, source, (Rectangle){ dest.x + o.x, dest.y,       dest.width, dest.height }, degrees, color);
  if (o.y)        DrawCentered(texture, source, (Rectangle){ dest.x,       dest.y + o.y, dest.width, dest.height }, degrees, color);
  if (o.x && o.y) DrawCentered(texture, source, (Rectangle){ dest.x + o.x, dest.y + o.y, dest.width, dest.height }, degrees, color);
}

void
DrawCircleWrapped(Vector2 center, float radius, Rectangle area, Color color)
{ Vector2 o = WrapOffset(center, radius, radius, area);

                  DrawCircleV(center, radius, color);
  if (o.x)        DrawCircleV((Vector2){ center.x + o.x, center.y       }, radius, color);
  if (o.y)        DrawCircleV((Vector2){ center.x,       center.y + o.y }, radius, color);
  if (o.x && o.y) DrawCircleV((Vector2){ center.x + o.x, center.y + o.y }, radius, color);
}

void wrap(v2 * position, Rectangle * screen) {
  if (position->x > screen->width)  position->x -= screen->width;
  if (position->x < 0)              position->x += screen->width;
  if (position->y > screen->height) position->y -= screen->height;
  if (position->y < 0)              position->y += screen->height;
}

// ---
