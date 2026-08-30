typedef Vector2 v2;
typedef Vector3 v3;
typedef Vector4 v4;

v2 xy2v2(int x, int y) {
    return (v2) {
        (float)x,
        (float)y,
    };
}
