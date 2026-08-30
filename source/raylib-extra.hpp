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

bool CheckCollisionSegmentRect(v2 a, v2 b, Rectangle r) {
    if (CheckCollisionPointRec(a, r) ||
        CheckCollisionPointRec(b, r)) {
        return true;
    }

    v2 tl = { r.x,             r.y };
    v2 tr = { r.x + r.width,   r.y };
    v2 bl = { r.x,             r.y + r.height };
    v2 br = { r.x + r.width,   r.y + r.height };

    return CheckCollisionLines(a, b, tl, tr, nullptr)
        || CheckCollisionLines(a, b, tr, br, nullptr)
        || CheckCollisionLines(a, b, br, bl, nullptr)
        || CheckCollisionLines(a, b, bl, tl, nullptr)
    ;
}

bool CheckCollisionSegmentRect(v4 line, Rectangle rect) {
    v2 a = { line.x, line.y };
    v2 b = { line.z, line.w };

    if (CheckCollisionPointRec(a, rect) ||
        CheckCollisionPointRec(b, rect)) {
        return true;
    }

    v2 tl = { rect.x,              rect.y };
    v2 tr = { rect.x + rect.width, rect.y };
    v2 bl = { rect.x,              rect.y + rect.height };
    v2 br = { rect.x + rect.width, rect.y + rect.height };

    return CheckCollisionLines(a, b, tl, tr, nullptr)
        || CheckCollisionLines(a, b, tr, br, nullptr)
        || CheckCollisionLines(a, b, br, bl, nullptr)
        || CheckCollisionLines(a, b, bl, tl, nullptr)
    ;
}
