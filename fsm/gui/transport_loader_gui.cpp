#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <configuration.h>
#include <transport_loader.h>
#include <common.h>
#include <raylib.h>
#include <cassert>
#include <memory>
#include <vector>

struct Configuration configuration;

#define TILE_SIZE     16
#define MARGIN_SIZE   2

#define RESOURCE_GRASS   0
#define RESOURCE_DIRT    1
#define RESOURCE_THINGS  2
#define RESOURCE_COW     3

static int map[] = {
        20, 19, 12, 21, 28, 11, 13, 29, 27, 36,
        37, 45, 44, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        /** Dirt */
        0, 0, 0, 0, 0, 0, 6, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        /** Things */
        0, 0, 0, 0, 0, 5, 6, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        /** Cow */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

class Object {
    float x;
    float y;
    bool visible;
private:
    virtual void update() {
    }

    virtual void render() {
        (void) x;
        (void) y;
        (void) visible;
    }

public:
    Object() : x(0), y(0), visible(false) {}

    virtual ~Object() = default;
};

class Resource final {
    std::shared_ptr<Rectangle[]> rects;
    Texture texture{};
    int rects_length{};
    int rect_size{};

public:
    void load(const char *filename, int size = TILE_SIZE) {
        int i, j, k = 0;

        rect_size = size;
        texture = LoadTexture(filename);
        rects_length = (texture.height / size) * (texture.width / size);
        rects = std::shared_ptr<Rectangle[]>(new Rectangle[rects_length]);
        for (i = 0; i < texture.width / size; ++i) {
            for (j = 0; j < texture.height / size; ++j) {
                rects[k].x = (float) (i * size);
                rects[k].y = (float) (j * size);
                rects[k].height = (float) (size);
                rects[k].width = (float) (size);
                ++k;
            }
        }
        SetTextureFilter(texture, TEXTURE_FILTER_TRILINEAR);
    }

    [[nodiscard]] const Rectangle &rect(int index) const {
        assert(index >= 0 && index < rects_length);
        return rects[index];
    }

    [[nodiscard]] float size() const {
        return (float) rect_size;
    }

    [[nodiscard]] int count() const {
        return rects_length;
    }

    void draw(int id, float x, float y) const {
        Rectangle src = rect(id), dest =
                {.x = x, .y = y, .width = size(), .height = size()};
        Vector2 origin = {.x = 0.0f, .y = 0.0f};
        DrawTextureTiled(texture, src, dest, origin, 0.f, 1.f, RAYWHITE);
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

class Game {
    std::vector<Object> objects;
    std::vector<Resource> resources;
    struct Configuration configuration;
public:
    enum ResourceId {
        Grass, Dirt, Things, Cow
    };

    Game(struct Configuration conf): resources(4), configuration(conf) {
        resources[Grass] = Resource("assets/Tilesets/Grass.png");
        resources[Dirt] = Resource("assets/Tilesets/Tilled.Dirt.png");
        resources[Things] = Resource("assets/Objects/Basic.Grass.Biom.Things.1.png");
        resources[Cow] = Resource("assets/Characters/Free.Cow.Sprites.png");
    }

    void render() {
        draw_resources();
    }

    [[nodiscard]] const Resource &resource(ResourceId id) const {
        return resources[id];
    }
private:
    void draw_resources() {
//        draw_resources(&configuration.map, Grass, [](size_t x) { return (size_t)map[x]; });
        draw_resources(&configuration.things, Things);
        draw_resources(&configuration.paths, Dirt);
    }
    void draw_resources(struct MapMatrix *mat, const ResourceId id, size_t (*transform)(size_t) = [](size_t x) { return x; }) {
        const auto &res = resource(id);
        for (float row = 0; row < mat->size; ++row) {
            for (float col = 0; col < mat->size; ++col) {
                auto idx = (size_t) matrix_get(mat, col, row);
                if (idx == 0)
                    continue;
                idx = (*transform)(idx);
                res.draw(idx - 1, row * res.size(), col * res.size());
            }
        }
    }
};

static void write_cow(const Resource &cow, float x, float y, int i) {
    auto size = cow.size();
    switch (i) {
        default:
        case 0: {
            cow.draw(0, x, y);
            cow.draw(4, x + size, y);
            cow.draw(1, x, y + size);
            cow.draw(5, x + size, y + size);
            break;
        }
        case 1: {
            cow.draw(8, x, y);
            cow.draw(12, x + size, y);
            cow.draw(9, x, y + size);
            cow.draw(13, x + size, y + size);
            break;
        }
        case 2: {
            cow.draw(16, x, y);
            cow.draw(20, x + size, y);
            cow.draw(17, x, y + size);
            cow.draw(21, x + size, y + size);
            break;
        }
        case 3: {
            break;
        }
        case 4: {
            break;
        }
    }

}

int
main(int argc, char **argv) {
    Resource grass, dirt, things, cow;
    Resource *resources[] = {&grass, &dirt, &things, &cow}, *selected;
    struct Rectangle player = {55, 395, 32, 64};
    size_t row, col;
    int tile_index = 0, resource_index = 0, resources_size =
            sizeof(resources) / sizeof(resources[0]), cow_count = 0, time = 0;
    Vector2 mouse;

    (void) argc;
    (void) argv;
    read_configuration(&configuration, "factory.json");
//    Game g(configuration);
    InitWindow((int) configuration.map.size * TILE_SIZE + MARGIN_SIZE,
               (int) configuration.map.size * TILE_SIZE + MARGIN_SIZE, "Transport");

    grass.load("assets/Tilesets/Grass.png");
    dirt.load("assets/Tilesets/Tilled.Dirt.png");
    things.load("assets/Objects/Basic.Grass.Biom.Things.1.png");
    cow.load("assets/Characters/Free.Cow.Sprites.png");

    SetTargetFPS(60);
    ShowCursor();
    while (!WindowShouldClose()) {
        mouse = GetMousePosition();
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (IsKeyPressed(KEY_LEFT)) {
            resource_index++;
            if (resource_index >= resources_size) {
                resource_index = 0;
            }
        } else if (IsKeyPressed(KEY_RIGHT)) {
            resource_index--;
            if (resource_index < 0) {
                resource_index = resources_size - 1;
            }
        }
        selected = resources[resource_index];

        if (IsKeyPressed(KEY_UP)) {
            tile_index++;
            if (tile_index >= selected->count())
                tile_index = 0;
        } else if (IsKeyPressed(KEY_DOWN)) {
            tile_index--;
            if (tile_index < 0) {
                tile_index = selected->count() - 1;
            }
        }

        for (row = 0; row < configuration.map.size; ++row) {
            for (col = 0; col < configuration.map.size; ++col) {
                Resource *c;
                auto idx = (size_t) matrix_get(&configuration.map, col, row);
                int real;

                real = map[idx];
                if (idx > 50 && idx < 100) {
                    c = resources[RESOURCE_DIRT];
                } else if (idx >= 100 && idx < 150) {
                    c = resources[RESOURCE_THINGS];
                } else if (idx >= 150) {
                    c = resources[RESOURCE_COW];
                } else {
                    c = resources[RESOURCE_GRASS];
                }

                if (real == 0) {
                    c = selected;
                    real = tile_index;
                }

                c->draw(real, row * c->size(), col * c->size());
            }
        }
//        g.render();

        time = GetTime() * 100;
        if (time % 66 == 0) {
            ++cow_count;
            if (cow_count > 1) {
                cow_count = 0;
            }
        }
        write_cow(cow, 50, 50, cow_count);
        write_cow(cow, 200, 300, cow_count);
        write_cow(cow, 430, 500, cow_count);

        DrawText(TextFormat("FPS: %i", GetFPS()), 2 + MARGIN_SIZE, 2 + MARGIN_SIZE, 16, GRAY);
        DrawText(TextFormat("TLE: %i", tile_index), 70 + MARGIN_SIZE, 2 + MARGIN_SIZE, 16, GRAY);
        DrawText(TextFormat("RSC: %i", resource_index), 130 + MARGIN_SIZE, 2 + MARGIN_SIZE, 16, GRAY);
        DrawText(TextFormat("TIM: %i", time), 190 + MARGIN_SIZE, 2 + MARGIN_SIZE, 16, GRAY);
        DrawText(TextFormat("MSE: (%03i, %03i)", (int) mouse.x, (int) mouse.y), 410 + MARGIN_SIZE, 2 + MARGIN_SIZE, 16,
                 GRAY);

        DrawRectangleLinesEx(player, 2, RED);

        EndDrawing();

        if (IsKeyPressed(KEY_C)) {
            Image screen = LoadImageFromScreen();
            ExportImage(screen, "export.png");
        }
    }
    CloseWindow();
    matrix_destroy(&configuration.map);
    return EXIT_SUCCESS;
}
