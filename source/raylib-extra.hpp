typedef Vector2 v2;
typedef Vector3 v3;
typedef Vector4 v4;


v2 xy2v2(int x, int y) {
    return (v2) {
        (float)x,
        (float)y,
    };
}

typedef RenderTexture2D rl_screen;
typedef Texture2D rl_texture;
typedef Sound rl_sound;
typedef Font rl_font;
// meat.hpp overlaps heavily with snake casing

void log_v2(v2 v) {
    TraceLog(LOG_INFO, "%f %f", v.x, v.y);
}
