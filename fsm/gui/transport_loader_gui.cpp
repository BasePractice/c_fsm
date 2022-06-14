#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <configuration.h>
#include <transport_loader.h>
#include <common.h>
#include <raylib.h>
#include <cassert>
#include <memory>
#include <utility>
#include <vector>
#include <memory>

struct Configuration configuration;

#define TILE_SIZE     16
#define MARGIN_SIZE   2

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
protected:
    float x;
    float y;
    bool visible;
public:
    Object() : x(0), y(0), visible(false) {}

    virtual ~Object() = default;

    void location(float cx, float cy) {
        x = cx;
        y = cy;
    }

    void visibility(bool v) {
        visible = v;
    }

    virtual void update(int) {
    }

    virtual void render() {
        (void) x;
        (void) y;
        (void) visible;
    }
};

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

class Cow final: public Object {
    int count;
public:
    Cow(const Resource &resource) : resource(resource) {
    }

    void update(int time) override {
        if (time % 66 == 0) {
            ++count;
            if (count > 1) {
                count = 0;
            }
        }
    }

    void render() override {
        if (visible)
            render(count);
    }

private:
    void render(int i) {
        auto size = resource.size();
        switch (i) {
            default:
            case 0: {
                resource.draw(0, x, y);
                resource.draw(4, x + size, y);
                resource.draw(1, x, y + size);
                resource.draw(5, x + size, y + size);
                break;
            }
            case 1: {
                resource.draw(8, x, y);
                resource.draw(12, x + size, y);
                resource.draw(9, x, y + size);
                resource.draw(13, x + size, y + size);
                break;
            }
            case 2: {
                resource.draw(16, x, y);
                resource.draw(20, x + size, y);
                resource.draw(17, x, y + size);
                resource.draw(21, x + size, y + size);
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

    const Resource &resource;
};

class Player: public Object {
    void render() override {
        struct Rectangle player = {x, y, 32, 64};
        DrawRectangleLinesEx(player, 2, RED);
    }
};

class Game {
    std::vector<Object *> objects;
    std::vector<Resource> resources;
    struct Configuration configuration;
    int tile_index = 0, resource_index = 0, time = 0;
    Vector2 mouse;
public:
    enum ResourceId {
        Grass, Dirt, Things, Cow
    };

    explicit Game(const char *const configuration_file) : resources(4), configuration{} {
        resources[Grass].load("assets/Tilesets/Grass.png");
        resources[Dirt].load("assets/Tilesets/Tilled.Dirt.png");
        resources[Things].load("assets/Objects/Basic.Grass.Biom.Things.1.png");
        resources[Cow].load("assets/Characters/Free.Cow.Sprites.png");
        read_configuration(&configuration, configuration_file);
    }

    Game &add(Object *object) {
        objects.insert(objects.cbegin(), object);
        return *this;
    }

    void cow(float x, float y) {
        auto cow = new class Cow(resource(Cow));
        cow->location(x, y);
        cow->visibility(true);
        add(cow);
    }

    void player(float x, float y) {
        auto player = new class Player;
        player->location(x, y);
        player->visibility(true);
        add(player);
    }

    void render() {
        draw_resources();
        for (auto &object: objects) {
            object->render();
        }
        draw_gud();
    }

    void update() {
        time = GetTime() * 100;
        mouse = GetMousePosition();

        for (auto object: objects) {
            object->update(time);
        }

        if (IsKeyPressed(KEY_LEFT)) {
            resource_index++;
            if (resource_index >= (int) resources.size()) {
                resource_index = 0;
            }
        } else if (IsKeyPressed(KEY_RIGHT)) {
            resource_index--;
            if (resource_index < 0) {
                resource_index = (int) resources.size() - 1;
            }
        }
        if (resource_index >= (int) resources.size())
            return;

        const Resource &at = resources.at(resource_index);
        if (IsKeyPressed(KEY_UP)) {
            tile_index++;
            if (tile_index >= at.count())
                tile_index = 0;
        } else if (IsKeyPressed(KEY_DOWN)) {
            tile_index--;
            if (tile_index < 0) {
                tile_index = at.count() - 1;
            }
        }
    }

    [[nodiscard]] const Resource &resource(ResourceId id) const {
        return resources[id];
    }

private:
    void draw_gud() const {
        DrawText(TextFormat("FPS: %i", GetFPS()), 2 + MARGIN_SIZE, 2 + MARGIN_SIZE, 16, GRAY);
        DrawText(TextFormat("TLE: %i", tile_index), 70 + MARGIN_SIZE, 2 + MARGIN_SIZE, 16, GRAY);
        DrawText(TextFormat("RSC: %i", resource_index), 130 + MARGIN_SIZE, 2 + MARGIN_SIZE, 16, GRAY);
        DrawText(TextFormat("TIM: %i", time), 190 + MARGIN_SIZE, 2 + MARGIN_SIZE, 16, GRAY);
        DrawText(TextFormat("MSE: (%03i, %03i)", (int) mouse.x, (int) mouse.y), 410 + MARGIN_SIZE, 2 + MARGIN_SIZE, 16,
                 GRAY);
    }

    void draw_resources() {
        for (size_t row = 0; row < configuration.map.size; ++row) {
            for (size_t col = 0; col < configuration.map.size; ++col) {
                Resource &c = resources[Grass];
                auto idx = (size_t) matrix_get(&configuration.map, col, row);
                int real;

                real = map[idx];
                if (idx > 50 && idx < 100) {
                    c = resources[Dirt];
                } else if (idx >= 100 && idx < 150) {
                    c = resources[Things];
                } else if (idx >= 150) {
                    c = resources[Cow];
                } else {
                    c = resources[Grass];
                }

                if (real == 0) {
                    c = resource(static_cast<ResourceId>(resource_index));
                    real = tile_index;
                }

                c.draw(real, row * c.size(), col * c.size());
            }
        }
        draw_resources(&configuration.things, Things);
        draw_resources(&configuration.paths, Dirt);
    }

    void draw_resources(struct MapMatrix *mat, const ResourceId id) {
        const auto &res = resource(id);
        for (size_t row = 0; row < mat->size; ++row) {
            for (size_t col = 0; col < mat->size; ++col) {
                auto idx = (size_t) matrix_get(mat, col, row);
                if (idx == 0) {
                    continue;
                }
                res.draw(idx - 1, row * res.size(), col * res.size());
            }
        }
    }

public:
    virtual ~Game() {
        for (auto it = objects.begin(); it != objects.end(); ++it) {
            delete *it;
            objects.erase(it);
        }
    }
};

int
main(int argc, char **argv) {
    (void) argc;
    (void) argv;
    InitWindow((int) 34 * TILE_SIZE + MARGIN_SIZE,
               (int) 34 * TILE_SIZE + MARGIN_SIZE, "Transport");
    SetTargetFPS(60);
    ShowCursor();
    Game g("factory.json");
    g.cow(50, 50);
    g.cow(200, 300);
    g.cow(430, 500);
    g.player(55, 395);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        g.update();
        g.render();
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
