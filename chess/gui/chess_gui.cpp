#include <iostream>
#include <functional>
#include <memory>
#include <vector>
#include <cassert>
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
        DrawTexturePro(texture, src, dest, origin, 0.f, RAYWHITE);
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
    Pawn = 0, Knight, Rook, Bishop, Queen, King, Empty = 255
};
#define COLOR_WHITE    0
#define COLOR_BLACK    7
#define W(p)           ((p) + COLOR_WHITE)
#define B(p)           ((p) + COLOR_BLACK)

static uint8_t board_pieces[8][8] = {
        {B(Rook), B(Knight), B(Bishop), B(Queen), B(King), B(Bishop), B(Knight), B(Rook)},
        {B(Pawn), B(Pawn),   B(Pawn),   B(Pawn),  B(Pawn), B(Pawn),   B(Pawn),   B(Pawn)},
        {Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty},
        {Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty},
        {Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty},
        {Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty},
        {W(Pawn), W(Pawn),   W(Pawn),   W(Pawn),  W(Pawn), W(Pawn),   W(Pawn),   W(Pawn)},
        {W(Rook), W(Knight), W(Bishop), W(Queen), W(King), W(Bishop), W(Knight), W(Rook)}
};

static Vector2 select(Vector2 point, float factor, float border_size) {
    auto row = (int) ((point.x - border_size) / (TILE_SIZE * factor));
    auto col = (int) ((point.y - border_size) / (TILE_SIZE * factor));
    if (col >= 0 && col < 8 && row >= 0 && row < 8) {
        return {.x = (float) col, .y = (float) row};
    }
    return {.x = (float) -1, .y = (float) -1};
}

static Rectangle select_rect(Vector2 id, float factor, float border_size) {
    if (id.x >= 0) {
        return {.x = ((float) id.y * (TILE_SIZE * factor) + MARGIN_SIZE) + border_size,
                .y = ((float) id.x * (TILE_SIZE * factor) + MARGIN_SIZE + border_size),
                .width = TILE_SIZE * factor, .height = TILE_SIZE * factor};
    }
    return {.x = 0, .y = 0, .width = 0, .height = 0};
}

int main() {
    Resource board, black, white;
    Rectangle selected_rect, selected_empty = {.x = 0, .y = 0, .width = 0, .height = 0};
    Vector2 mouse, selected_id;
    bool selected = false;
    float factor = 2.f, border_size = factor * 6;

    InitWindow((WINDOW_WIDTH * TILE_SIZE + MARGIN_SIZE) * factor,
               (WINDOW_HEIGHT * TILE_SIZE + MARGIN_SIZE) * factor, "Шахматы");
    SetTargetFPS(60);

    board.load("assets/boards/board_plain_03.png", 142);
    black.load("assets/16x16/BlackPieces_Simplified.png");
    white.load("assets/16x16/WhitePieces_Simplified.png");
    while (!WindowShouldClose()) {
        mouse = GetMousePosition();
        BeginDrawing();
        ClearBackground(RAYWHITE);

        selected_id = select(mouse, factor, border_size);
        if (selected_id.x >= 0 && !selected) {
            int d = board_pieces[(int) selected_id.x][(int) selected_id.y];
            if (d != Empty) {
                selected_rect = select_rect(selected_id, factor, border_size);
                selected = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
            } else {
                selected_rect = selected_empty;
            }
//            std::cout << "[" << selected.x << ", " << selected.y << "] = " << std::hex << std::uppercase << d << std::dec << std::endl;
        }
        board.draw(0, 0, 0, factor);
        for (int x = 0; x < 8; ++x) {
            for (int y = 0; y < 8; ++y) {
                int d = board_pieces[y][x];
                if (d == Empty)
                    continue;
                if (d >= COLOR_BLACK) {
                    black.draw(d - COLOR_BLACK, (x * TILE_SIZE * factor) + border_size,
                               (y * TILE_SIZE * factor) + border_size, factor);
                } else if (d >= 0) {
                    white.draw(d, (x * TILE_SIZE * factor) + border_size, (y * TILE_SIZE * factor) + border_size,
                               factor);
                }
            }
        }
        if (selected_rect.x > MARGIN_SIZE)
            DrawRectangleLinesEx(selected_rect, 2.f, selected ? RED : LIGHTGRAY);
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            selected = false;
        }
        EndDrawing();
        if (IsKeyPressed(KEY_C)) {
            TakeScreenshot("export.png");
        }
    }
    CloseWindow();
    return 0;
}