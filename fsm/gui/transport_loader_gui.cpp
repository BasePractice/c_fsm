#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <iostream>
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

#define WINDOW_HEIGHT  40
#define WINDOW_WIDTH   34

static int map[] = {
        20, 19, 12, 21, 28, 11, 13, 29, 27, 36,
        37, 45, 44, 79, 0, 0, 0, 0, 0, 0,
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

//static size_t path[] = {
//        0, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
//        7, 7, 7, 7, 7, 7, 7
//};

struct Context {
    [[nodiscard]] virtual bool is_collision(Rectangle rectangle) const = 0;

    [[nodiscard]] virtual bool is_collision(Vector2 center, float radius) const = 0;

    [[nodiscard]] virtual int rfid_point(Vector2 center, float radius) const = 0;

    [[nodiscard]] virtual bool rfid_point(float x, float y) const = 0;
};

class Object {
protected:
    float x;
    float y;
    bool visible;
public:
    Object() : x(0), y(0), visible(false) {}

    virtual ~Object() = default;

    [[nodiscard]] float get_x() const {
        return x;
    }

    [[nodiscard]] float get_y() const {
        return y;
    }

    void location(float cx, float cy) {
        x = cx;
        y = cy;
    }

    void visibility(bool v) {
        visible = v;
    }

    virtual void update(Context *, int) {
    }

    virtual void render() const {
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

class Cow final : public Object {
    int count;
public:
    Cow(const Resource &resource) : resource(resource) {
    }

    void update(Context *, int time) override {
        if (time % 66 == 0) {
            ++count;
            if (count > 1) {
                count = 0;
            }
        }
    }

    void render() const override {
        if (visible)
            render(count);
    }

private:
    void render(int i) const {
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

class Loader : public Object {
    const float width = 32;
    const float height = 64;
    float angel = 0.f;
    bool collision = false;
    struct Color collision_color = {135, 60, 190, 150};

    struct UV {
        Vector2 center;
        float radius;
    };
    struct UV top_left = {.center = {.x = 0, .y = 0}, .radius = 30};
    struct UV top_right = {.center = {.x = 0, .y = 0}, .radius = 30};
    struct UV bottom_left = {.center = {.x = 0, .y = 0}, .radius = 30};
    struct UV bottom_right = {.center = {.x = 0, .y = 0}, .radius = 30};

    float rfid_radius = 5;
    int point = -1;
    bool point_center = false;
public:
    [[nodiscard]] float get_angel() const {
        return angel;
    }

    [[nodiscard]] int get_point() const {
        return point;
    }

    [[nodiscard]] bool is_point_center() const {
        return point_center;
    }

    void update(Context *context, int time) override {
        float dx = x;
        float dy = y;
        (void) time;

        if (IsKeyDown(KEY_UP)) {
            dy -= 0.5;
            if (dy < 0) {
                dy = 0;
            }
        }
        if (IsKeyDown(KEY_LEFT)) {
            dx -= 0.5;
            if (dx < 0) {
                dx = 0;
            }
        }
        if (IsKeyDown(KEY_DOWN)) {
            dy += 0.5;
            if (dy > WINDOW_WIDTH * TILE_SIZE) {
                dy = WINDOW_WIDTH * TILE_SIZE - MARGIN_SIZE;
            }
        }
        if (IsKeyDown(KEY_RIGHT)) {
            dx += 0.5;
            if (dx > WINDOW_WIDTH * TILE_SIZE) {
                dx = WINDOW_WIDTH * TILE_SIZE - MARGIN_SIZE;
            }
        }

        if (IsKeyDown(KEY_D)) {
            angel += 0.4;
            if (angel >= 359.8) {
                angel = 0;
            }
        }
        if (IsKeyDown(KEY_A)) {
            angel -= 0.4;
            if (angel < 0.2) {
                angel = 360;
            }
        }
        collision = context->is_collision(rect(dx, dy));
        if (!collision) {
            x = dx;
            y = dy;
        }
        top_left.center.x = x - width / 2;
        top_left.center.y = y - height / 2;
        top_right.center.x = x + width - width / 2;
        top_right.center.y = y - height / 2;

        bottom_left.center.x = x + width / 2;
        bottom_left.center.y = y + height / 2;

        bottom_right.center.x = x - width / 2;
        bottom_right.center.y = y + height / 2;

        point = context->rfid_point(Vector2{.x = x, .y = y}, rfid_radius);
        point_center = context->rfid_point(x, y);
    }

    void render() const override {
        const Rectangle &rectangle = rect();

        Camera2D screen = {{x, y}, {x, y}, angel, 1};
        BeginMode2D(screen);
        if (collision) {
            DrawRectangleLinesEx(rectangle, 2, RED);
            DrawCircle(rectangle.x + rectangle.width / 2, rectangle.y, 2, RED);
            DrawRectangleLinesEx(Rectangle{
                    .x = ((rectangle.x - TILE_SIZE) / TILE_SIZE) * TILE_SIZE,
                    .y = ((rectangle.y - TILE_SIZE) / TILE_SIZE) * TILE_SIZE,
                    .width = rectangle.width + TILE_SIZE + TILE_SIZE,
                    .height = rectangle.height + TILE_SIZE + TILE_SIZE,
            }, 1, collision_color);
        } else {
            DrawRectangleLinesEx(rectangle, 2, BLUE);
            DrawCircle(rectangle.x + rectangle.width / 2, rectangle.y, 2, BLUE);
        }
        DrawCircleLines((int) top_left.center.x, (int) top_left.center.y, top_left.radius, collision_color);
        DrawCircleLines((int) top_right.center.x, (int) top_right.center.y, top_right.radius, collision_color);
        DrawCircleLines((int) bottom_left.center.x, (int) bottom_left.center.y, bottom_left.radius, collision_color);
        DrawCircleLines((int) bottom_right.center.x, (int) bottom_right.center.y, bottom_right.radius, collision_color);
        DrawCircleLines((int) x, (int) y, rfid_radius, collision_color);
        EndMode2D();
    }


private:
    [[nodiscard]] Rectangle rect() const {
        return rect(x, y);
    }

    [[nodiscard]] Rectangle rect(float x, float y) const {
        return {.x = x - width / 2, .y = y - height / 2, .width = width, .height = height};
    }
};

class Game final : public Context {
    std::vector<Object *> objects;
    std::vector<Resource> resources;
    struct Configuration configuration;
    int tile_index = 0, resource_index = 0, time = 0;
    Vector2 mouse;
    bool visible_debug = true;
    const float height_delta = WINDOW_HEIGHT * 13.5;

    struct Color color_ground = {190, 33, 55, 50};
    struct Color color_gud = RED;

    Loader *loader = nullptr;
public:
    enum ResourceId {
        Grass, Dirt, Things, Cow
    };

    explicit Game(const char *const configuration_file) : resources(4), configuration{}, mouse{} {
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

    void add_loader(float x, float y) {
        assert(loader == nullptr);
        loader = new class Loader;
        loader->location(x, y);
        loader->visibility(true);
        add(loader);
    }

    void render() const {
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
            object->update(this, time);
        }

        if (IsKeyPressed(KEY_LEFT) && IsKeyDown(KEY_R)) {
            resource_index++;
            if (resource_index >= (int) resources.size()) {
                resource_index = 0;
            }
        } else if (IsKeyPressed(KEY_RIGHT) && IsKeyDown(KEY_R)) {
            resource_index--;
            if (resource_index < 0) {
                resource_index = (int) resources.size() - 1;
            }
        }
        if (resource_index >= (int) resources.size())
            return;

        const Resource &at = resources.at(resource_index);
        if (IsKeyPressed(KEY_UP) && IsKeyDown(KEY_R)) {
            tile_index++;
            if (tile_index >= at.count())
                tile_index = 0;
        } else if (IsKeyPressed(KEY_DOWN) && IsKeyDown(KEY_R)) {
            tile_index--;
            if (tile_index < 0) {
                tile_index = at.count() - 1;
            }
        }
    }

    [[nodiscard]] const Resource &resource(ResourceId id) const {
        return resources[id];
    }

    [[nodiscard]] bool is_collision(Rectangle rectangle) const override {
        auto di = get_tail(rectangle.x, rectangle.y);
        if (di < 0 || is_ground(di)) {
            return true;
        }
        di = get_tail(rectangle.x + rectangle.width, rectangle.y);
        if (di < 0 || is_ground(di)) {
            return true;
        }
        di = get_tail(rectangle.x, rectangle.y + rectangle.height);
        if (di < 0 || is_ground(di)) {
            return true;
        }
        di = get_tail(rectangle.x + rectangle.width, rectangle.y + rectangle.height);
        if (di < 0 || is_ground(di)) {
            return true;
        }
        return false;
    }

    [[nodiscard]] bool is_collision(Vector2 center, float radius) const override {
        (void) center;
        (void) radius;
        return false;
    }

    [[nodiscard]] int rfid_point(Vector2 center, float radius) const override {
        size_t col = center.x / TILE_SIZE;
        size_t row = center.y / TILE_SIZE;
        auto tail = (size_t) matrix_get(&configuration.paths, row, col);
        if (tail > 0 && tail < 20) {
            float dx = (col * TILE_SIZE) + TILE_SIZE / 2;
            float dy = (row * TILE_SIZE) + TILE_SIZE / 2;
            bool under = CheckCollisionCircles(
                    center, radius, Vector2{.x = dx, .y = dy}, 0.5f);
            return under ? tail : 0;
        }
        return -1;
    }

    [[nodiscard]] bool rfid_point(float x, float y) const override {
        size_t col = x / TILE_SIZE;
        size_t row = y / TILE_SIZE;
        float dx = (col * TILE_SIZE) + TILE_SIZE / 2;
        float dy = (row * TILE_SIZE) + TILE_SIZE / 2;
        auto tail = (size_t) matrix_get(&configuration.paths, row, col);
        return tail > 0 && tail < 20 && dx == x && dy == y;
    }

private:
    void draw_gud() const {
        int row = MARGIN_SIZE;

        DrawText(TextFormat("FPS: %i", GetFPS()), 2 + MARGIN_SIZE,
                 row + height_delta + MARGIN_SIZE, 16, color_gud);
        DrawText(TextFormat("TLE: %i", tile_index), 70 + MARGIN_SIZE,
                 row + height_delta + MARGIN_SIZE, 16, color_gud);
        DrawText(TextFormat("RSC: %i", resource_index), 130 + MARGIN_SIZE,
                 row + height_delta + MARGIN_SIZE, 16, color_gud);
        const Resource &c = resource(static_cast<ResourceId>(resource_index));
        c.draw(tile_index, 190 + MARGIN_SIZE, row + height_delta + MARGIN_SIZE);
        DrawRectangleLinesEx(
                Rectangle{.x = 190 + MARGIN_SIZE, .y = row + height_delta + MARGIN_SIZE,
                        .width = TILE_SIZE, .height = TILE_SIZE}, 1, color_gud);
        DrawText(TextFormat("TIM: %i", time), 220 + MARGIN_SIZE,
                 row + height_delta + MARGIN_SIZE, 16, color_gud);
        DrawText(TextFormat("MSE: (%03i, %03i)", (int) mouse.x, (int) mouse.y), 425 + MARGIN_SIZE,
                 row + height_delta + MARGIN_SIZE, 16, color_gud);


        row += MARGIN_SIZE + 15;
        DrawText("R + <Up>/<Down>   : TLE", 2 + MARGIN_SIZE,
                 row + height_delta + MARGIN_SIZE, 16, color_gud);
        row += MARGIN_SIZE + 15;
        DrawText("R + <Left>/<Right>: RSC", 2 + MARGIN_SIZE,
                 row + height_delta + MARGIN_SIZE, 16, color_gud);
        if (loader != nullptr) {
            row += MARGIN_SIZE + 15;
            DrawText(TextFormat("LDR: [%d, %d],  ANG: %f, POT: %d",
                                (int) loader->get_x(), (int) loader->get_y(), loader->get_angel(), loader->get_point()),
                     2 + MARGIN_SIZE,
                     row + height_delta + MARGIN_SIZE, 16, color_gud);
        }
    }

    void draw_resources() const {
        const Resource &grass = resource(Grass);
        for (size_t row = 0; row < configuration.map.size; ++row) {
            for (size_t col = 0; col < configuration.map.size; ++col) {
                auto idx = (size_t) matrix_get(&configuration.map, col, row);
                float cx = (float) row * grass.size();
                float cy = (float) col * grass.size();
                grass.draw(map[idx], cx, cy);
                if (visible_debug) {
                    if (is_ground(idx)) {
                        struct Rectangle ground = {cx, cy, TILE_SIZE, TILE_SIZE};
                        DrawRectangleLinesEx(ground, 1, color_ground);
                    }
                }
            }
        }
        draw_resources(&configuration.things, Things);
//        draw_resources(&configuration.paths, Dirt, [](size_t x) { return path[x]; });

        if (visible_debug) {
            for (size_t row = 0; row < configuration.paths.size; ++row) {
                for (size_t col = 0; col < configuration.paths.size; ++col) {
                    auto idx = (size_t) matrix_get(&configuration.paths, col, row);
                    if (idx == 0) {
                        continue;
                    }
                    float dx = (row * TILE_SIZE) + TILE_SIZE / 2;
                    float dy = (col * TILE_SIZE) + TILE_SIZE / 2;
                    DrawCircleLines(dx, dy, 5, color_ground);
                }
            }
        }
    }

    void draw_resources(const struct MapMatrix *mat, const ResourceId id,
                        size_t (*transform)(size_t) = [](size_t x) { return x; }) const {
        const auto &res = resource(id);
        for (size_t row = 0; row < mat->size; ++row) {
            for (size_t col = 0; col < mat->size; ++col) {
                auto idx = (size_t) matrix_get(mat, col, row);
                auto i = (*transform)(idx);
                if (i == 0) {
                    continue;
                }
                res.draw(i - 1, row * res.size(), col * res.size());
            }
        }
    }

    static bool is_ground(size_t id) {
        return id > 0 && id < 13;
    }

    int get_tail(float x, float y) const {
        size_t col = x / TILE_SIZE;
        size_t row = y / TILE_SIZE;
        if (configuration.map.size <= row || configuration.map.size <= col)
            return -1;
        MapMatrixValue get = matrix_get(&configuration.map, row, col);
        return reinterpret_cast<size_t>(get);
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
    InitWindow(WINDOW_WIDTH * TILE_SIZE + MARGIN_SIZE, WINDOW_HEIGHT * TILE_SIZE + MARGIN_SIZE, "Транспортер");
    SetTargetFPS(60);
    Game g("factory.json");
    g.cow(50, 50);
    g.cow(200, 300);
    g.cow(430, 500);
    g.add_loader(55, 425);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        g.update();
        g.render();
        EndDrawing();
        if (IsKeyPressed(KEY_C)) {
            TakeScreenshot("export.png");
//            Image screen = LoadImageFromScreen();
//            ExportImage(screen, "export.png");
        }
    }
    CloseWindow();
    matrix_destroy(&configuration.map);
    return EXIT_SUCCESS;
}
