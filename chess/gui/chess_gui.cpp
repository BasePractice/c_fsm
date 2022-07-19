#include <iostream>
#include <functional>
#include <raylib.h>

#define TILE_SIZE     16
#define MARGIN_SIZE   2

#define WINDOW_HEIGHT  9
#define WINDOW_WIDTH   9

class Resource final {
    std::shared_ptr<Rectangle[]> rects;
    Texture texture;
    int rects_length;
    int rect_size;

public:
    void load(const char *filename, int size = TILE_SIZE) {
        int i, j, k = 0;

        rect_size = size;
        texture = LoadTexture(filename);
        if (texture.height == size && texture.width == size) {
            rects_length = 1;
            rects = std::shared_ptr<Rectangle[]>(new Rectangle[rects_length], [](Rectangle *r) { delete[] r; });
            rects[0].y = 0.f;
            rects[0].x = 0.f;
            rects[0].height = (float) (size);
            rects[0].width = (float) (size);
        } else {
            rects_length = (texture.height / size) * (texture.width / size);
            rects = std::shared_ptr<Rectangle[]>(new Rectangle[rects_length], [](Rectangle *r) { delete[] r; });
            for (i = 0; i < texture.width / size; ++i) {
                for (j = 0; j < texture.height / size; ++j) {
                    rects[k].x = (float) (i * size);
                    rects[k].y = (float) (j * size);
                    rects[k].height = (float) (size);
                    rects[k].width = (float) (size);
                    ++k;
                }
            }
        }

        SetTextureFilter(texture, TEXTURE_FILTER_TRILINEAR);
    }

    [[nodiscard]] const Rectangle &rect(size_t index) const {
        assert(index >= 0 && index < rects_length);
        return rects[(std::ptrdiff_t) index];
    }

    [[nodiscard]] float size() const {
        return (float) rect_size;
    }

    [[nodiscard]] int count() const {
        return rects_length;
    }

    void draw(size_t id, float x, float y) const {
        draw(id, x, y, 1.f);
    }

    void draw(size_t id, float x, float y, float factor) const {
        Rectangle src = rect(id), dest =
                {.x = x, .y = y, .width = size() * factor, .height = size() * factor};
        Vector2 origin = {.x = 0.0f, .y = 0.0f};
        DrawTextureTiled(texture, src, dest, origin, 0.f, factor, RAYWHITE);
    }

    Resource() : rects(), texture(), rects_length(0), rect_size(0) {}

    explicit Resource(const char *filename) : Resource() {
        load(filename);
    }

    Resource(const Resource &) = default;

    Resource &operator=(const Resource &that) = default;

    virtual ~Resource() {
        UnloadTexture(texture);
    }
};

enum Pieces {
    Pawn = 0, Knight, Rook, Bishop, Queen, King, Empty = -1
};
#define COLOR_WHITE    0
#define COLOR_BLACK    7
#define W(p)           ((p) + COLOR_WHITE)
#define B(p)           ((p) + COLOR_BLACK)

static int board_state[8][8] = {
        {B(Rook), B(Knight), B(Bishop), B(Queen), B(King), B(Bishop), B(Knight), B(Rook)},
        {B(Pawn), B(Pawn),   B(Pawn),   B(Pawn),  B(Pawn), B(Pawn),   B(Pawn),   B(Pawn)},
        {Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty},
        {Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty},
        {Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty},
        {Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty},
        {W(Pawn), W(Pawn),   W(Pawn),   W(Pawn),  W(Pawn), W(Pawn),   W(Pawn),   W(Pawn)},
        {B(Rook), B(Knight), B(Bishop), B(Queen), B(King), B(Bishop), B(Knight), B(Rook)}
};

int main() {
    Resource board, black, white;
    float factor = 2.f;

    InitWindow((WINDOW_WIDTH * TILE_SIZE + MARGIN_SIZE) * factor,
               (WINDOW_HEIGHT * TILE_SIZE + MARGIN_SIZE) * factor, "Chess");
    SetTargetFPS(60);

    board.load("assets/boards/board_plain_01.png", 142);
    black.load("assets/16x16/BlackPieces.png");
    white.load("assets/16x16/WhitePieces.png");
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        board.draw(0, 0, 0, factor);
        for (int x = 0; x < 8; ++x) {
            for (int y = 0; y < 8; ++y) {
                int d = board_state[y][x];
                if (d >= COLOR_BLACK) {
                    black.draw(d - COLOR_BLACK, x * TILE_SIZE * factor, y * TILE_SIZE * factor, factor);
                } else if (d >= 0) {
                    white.draw(d, x * TILE_SIZE * factor, y * TILE_SIZE * factor, factor);
                }
            }
        }
        EndDrawing();
        if (IsKeyPressed(KEY_C)) {
            TakeScreenshot("export.png");
        }
    }
    CloseWindow();
    return 0;
}