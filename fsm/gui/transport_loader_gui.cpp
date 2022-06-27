#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <transport_loader.h>
#include <common.h>
#include <raylib.h>
#include <raymath.h>

#define RAYGUI_IMPLEMENTATION

#include <raygui.h>
#include <cassert>
#include <memory>
#include <vector>
#include <sstream>
#include <raymath.h>

struct Configuration configuration;
static bool visible_debug = true;

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

struct Readable {
    [[nodiscard]] virtual bool rotate_right() const {
        return IsKeyDown(KEY_D);
    }

    [[nodiscard]] virtual bool rotate_left() const {
        return IsKeyDown(KEY_A);
    }

    [[nodiscard]] virtual bool do_gas() const {
        return IsKeyDown(KEY_UP);
    }

    [[nodiscard]] virtual bool do_break() const {
        return IsKeyDown(KEY_DOWN);
    }

    [[nodiscard]] virtual bool do_left() const {
        return IsKeyDown(KEY_LEFT);
    }

    [[nodiscard]] virtual bool do_right() const {
        return IsKeyDown(KEY_RIGHT);
    }
};

struct Writable {
    virtual void rfid_point(int point) {
    }

    virtual void on_line(bool line) {
    }

    virtual void axis_angle(float angle) {
    }

    virtual void distance_top(float value) {}

    virtual void distance_bottom(float value) {}

    virtual void distance_left(float value) {}

    virtual void distance_right(float value) {}
};

struct Engine {
    Engine() : _default_readable(new Readable), _read(new Readable), _write(new Writable) {}

    explicit Engine(Readable *read, Writable *write) : _enable_default_readable(true), _default_readable(new Readable),
                                                       _read(read), _write(write) {}

    void set_default_readable(bool value) {
        _enable_default_readable = value;
    }

    virtual void tick() {

    }

    [[nodiscard]] virtual Readable *read() const {
        if (_default_readable)
            return _default_readable;
        return _read;
    }

    [[nodiscard]] virtual Writable *write() const {
        return _write;
    }

protected:
    bool _enable_default_readable;
    Readable *const _default_readable;
private:
    Readable *const _read;
    Writable *const _write;
};

struct EngineStated : public Engine, public Readable, public Writable {
    bool _rotate_right = false;
    bool _rotate_left = false;
    bool _do_gas = false;
    bool _do_break = false;
    bool _do_left = false;
    bool _do_right = false;

    float _distance_top = -1;
    float _distance_bottom = -1;
    float _distance_left = -1;
    float _distance_right = -1;

    bool _on_line = false;
    int _rfid_point = -1;

    float _angle = 0.f;

    explicit EngineStated(void (*tf)(EngineStated *engine) = [](EngineStated *engine) {}) : _tf(tf) {}

    [[nodiscard]] bool rotate_right() const override {
        return _rotate_right;
    }

    [[nodiscard]] bool rotate_left() const override {
        return _rotate_left;
    }

    [[nodiscard]] bool do_gas() const override {
        return _do_gas;
    }

    [[nodiscard]] bool do_break() const override {
        return _do_break;
    }

    [[nodiscard]] bool do_left() const override {
        return _do_left;
    }

    [[nodiscard]] bool do_right() const override {
        return _do_right;
    }

    [[nodiscard]] Readable *read() const override {
        if (_enable_default_readable)
            return _default_readable;
        return (Readable *) this;
    }

    [[nodiscard]] Writable *write() const override {
        return (Writable *) this;
    }

    void rfid_point(int point) override {
        _rfid_point = point;
    }

    void on_line(bool line) override {
        _on_line = line;
    }

    void axis_angle(float angle) override {
        _angle = angle;
    }

    void distance_top(float value) override {
        _distance_top = value;
    }

    void distance_bottom(float value) override {
        _distance_bottom = value;
    }

    void distance_left(float value) override {
        _distance_left = value;
    }

    void distance_right(float value) override {
        _distance_right = value;
    }

    void tick() override {
        (*_tf)(this);
    }

private:
    void reset() {
        _on_line = false;
        _rfid_point = -1;
        _rotate_right = false;
        _rotate_left = false;
        _do_gas = false;
        _do_break = false;
        _do_left = false;
        _do_right = false;
        _distance_top = -1;
        _distance_bottom = -1;
        _distance_left = -1;
        _distance_right = -1;
    }

    void (*_tf)(EngineStated *engine);
};

struct Context {
    Context() : _engine(new Engine) {}

    explicit Context(Engine *engine) : _engine(engine) {}

    [[nodiscard]] virtual bool is_collision(Rectangle rectangle) const = 0;

    [[nodiscard]] virtual bool is_collision(Vector2 center, float radius) const = 0;

    [[nodiscard]] virtual bool is_collision(Vector2 start, Vector2 stop) const = 0;

    [[nodiscard]] virtual int rfid_point(Vector2 center, float radius) const = 0;

    [[nodiscard]] virtual bool rfid_point(float x, float y) const = 0;

    [[nodiscard]] virtual bool line_point(float x, float y) const = 0;

    [[nodiscard]] virtual bool is_collision(Vector2 start, Vector2 stop, Vector2 &intersection) const = 0;

    [[nodiscard]] virtual std::shared_ptr<Engine> engine() const {
        return _engine;
    }

private:
    const std::shared_ptr<Engine> _engine;
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

class [[maybe_unused]] Cow final : public Object {
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

struct Device {
    Vector2 d1, d2;
    float distance;
    Vector2 intersect;
};

class Loader : public Object {
    const float width = 32;
    const float height = 64;
    float angle = 0.f;
    bool collision = false;
    struct Color collision_color = {135, 60, 190, 150};


    struct Vector2 top_left = {.x = 0, .y = 0};
    struct Vector2 top_right = {.x = 0, .y = 0};
    struct Vector2 bottom_left = {.x = 0, .y = 0};
    struct Vector2 bottom_right = {.x = 0, .y = 0};

    struct Device device_top = {.d1 = {.x = 0, .y = 0}, .d2 = {.x = 0, .y = 0}, .distance = 0, .intersect = {.x = 0, .y = 0}};
    struct Device device_bottom = {.d1 = {.x = 0, .y = 0}, .d2 = {.x = 0, .y = 0}, .distance = 0, .intersect = {.x = 0, .y = 0}};
    struct Device device_left = {.d1 = {.x = 0, .y = 0}, .d2 = {.x = 0, .y = 0}, .distance = 0, .intersect = {.x = 0, .y = 0}};
    struct Device device_right = {.d1 = {.x = 0, .y = 0}, .d2 = {.x = 0, .y = 0}, .distance = 0, .intersect = {.x = 0, .y = 0}};

    float rfid_radius = 5;
    int point = -1;
    bool on_line = false;
public:
    [[nodiscard]] struct Device get_uv_top() const {
        return device_top;
    }

    [[nodiscard]] struct Device get_uv_bottom() const {
        return device_bottom;
    }

    [[nodiscard]] struct Device get_uv_left() const {
        return device_left;
    }

    [[nodiscard]] struct Device get_uv_right() const {
        return device_right;
    }

    [[nodiscard]] float get_angel() const {
        return angle;
    }

    [[nodiscard]] int get_point() const {
        return point;
    }

    [[nodiscard]] bool get_on_line() const {
        return on_line;
    }

    void update(Context *context, int time) override {
        const float delta = 0.5;
        float dx = x;
        float dy = y;
        float rotate_angle = angle;
        (void) time;
        auto engine = context->engine();
        auto input = engine->read();
        auto write = engine->write();

        if (input->rotate_right()) {
            rotate_angle += 0.5;
            if (rotate_angle >= 359.8) {
                rotate_angle = 0;
            }
        }
        if (input->rotate_left()) {
            rotate_angle -= 0.5;
            if (rotate_angle < 0.2) {
                rotate_angle = 360;
            }
        }

        float ddx = sin(rotate_angle * (PI / 180));
        float ddy = cos(rotate_angle * (PI / 180));
        float direction = 1;
        if (input->do_gas()) {
            if ((rotate_angle >= 80 && rotate_angle < 100) || (rotate_angle >= 250 && rotate_angle < 290))
                direction = -direction;
            dx += ddx * -direction;
//            dx -= sin(rotate_angle);
            dy += ddy * -direction;
//            dy -= cos(rotate_angle);
        } else if (input->do_break()) {
            if ((rotate_angle >= 80 && rotate_angle < 100) || (rotate_angle >= 250 && rotate_angle < 290))
                direction = -direction;
            dx += ddx * direction;
//            dx += sin(rotate_angle);
            dy += ddy * direction;
//            dy += cos(rotate_angle);
        }

        if (input->do_right()) {
            dx += delta;
        } else if (input->do_left()) {
            dx -= delta;
        }

        if (dy < 0) {
            dy = 0;
        }
        if (dx < 0) {
            dx = 0;
        }
        if (dy > WINDOW_WIDTH * TILE_SIZE) {
            dy = WINDOW_WIDTH * TILE_SIZE - MARGIN_SIZE;
        }
        if (dx > WINDOW_WIDTH * TILE_SIZE) {
            dx = WINDOW_WIDTH * TILE_SIZE - MARGIN_SIZE;
        }

        if (dx != x || dy != y || angle != rotate_angle) {
            auto dtl = rotate(Vector2{.x = dx - width / 2, .y = dy - height / 2}, Vector2{.x = dx, .y = dy},
                              rotate_angle);
            auto dtr = rotate(Vector2{.x = dx + width / 2, .y = dy - height / 2}, Vector2{.x = dx, .y = dy},
                              rotate_angle);
            auto dbl = rotate(Vector2{.x = dx - width / 2, .y = dy + height / 2}, Vector2{.x = dx, .y = dy},
                              rotate_angle);
            auto dbr = rotate(Vector2{.x = dx + width / 2, .y = dy + height / 2}, Vector2{.x = dx, .y = dy},
                              rotate_angle);

            collision = context->is_collision(dtl, dtr);
            collision |= context->is_collision(dtr, dbr);
            collision |= context->is_collision(dbr, dbl);
            collision |= context->is_collision(dbl, dtl);
            if (!collision) {
                x = dx;
                y = dy;
            }
            angle = rotate_angle;
        }
        top_left = rotate(Vector2{.x = x - width / 2, .y = y - height / 2}, Vector2{.x = x, .y = y}, angle);
        top_right = rotate(Vector2{.x = x + width / 2, .y = y - height / 2}, Vector2{.x = x, .y = y}, angle);
        bottom_left = rotate(Vector2{.x = x - width / 2, .y = y + height / 2}, Vector2{.x = x, .y = y}, angle);
        bottom_right = rotate(Vector2{.x = x + width / 2, .y = y + height / 2}, Vector2{.x = x, .y = y}, angle);

        device_top = device_calculate(top_left, top_right, width / 2 + 5);
        device_bottom = device_calculate(bottom_right, bottom_left, width / 2 + 5);

        device_left = device_calculate(bottom_left, top_left, height / 2 + 2);
        device_right = device_calculate(top_right, bottom_right, height / 2 + 2);

        point = context->rfid_point(Vector2{.x = x, .y = y}, rfid_radius);
        on_line = context->line_point(x, y);

        {
            if (context->is_collision(device_top.d1, device_top.d2, device_top.intersect)) {
                device_top.distance = Vector2Distance(device_top.d1, device_top.intersect);
            } else {
                device_top.distance = -1;
                device_top.intersect = Vector2Zero();
            }

            if (context->is_collision(device_bottom.d1, device_bottom.d2, device_bottom.intersect)) {
                device_bottom.distance = Vector2Distance(device_bottom.d1, device_bottom.intersect);
            } else {
                device_bottom.distance = -1;
                device_bottom.intersect = Vector2Zero();
            }

            if (context->is_collision(device_left.d1, device_left.d2, device_left.intersect)) {
                device_left.distance = Vector2Distance(device_left.d1, device_left.intersect);
            } else {
                device_left.distance = -1;
                device_left.intersect = Vector2Zero();
            }

            if (context->is_collision(device_right.d1, device_right.d2, device_right.intersect)) {
                device_right.distance = Vector2Distance(device_right.d1, device_right.intersect);
            } else {
                device_right.distance = -1;
                device_right.intersect = Vector2Zero();
            }
        }
    }

    void render() const override {
        auto color = BLUE;
        auto thick = 1.0f;
        auto top = Vector2{.x = (top_left.x + top_right.x) / 2, .y = (top_left.y + top_right.y) / 2};

        if (collision) {
            color = RED;
            thick = 2.f;
        }
        DrawCircle((int) top.x, (int) top.y, 2.f, color);

        DrawLineEx(top_left, top_right, thick, color);
        DrawLineEx(top_right, bottom_right, thick, color);
        DrawLineEx(bottom_right, bottom_left, thick, color);
        DrawLineEx(bottom_left, top_left, thick, color);

        if (visible_debug) {
            DrawLineEx(Vector2{.x = device_top.d1.x, .y = device_top.d1.y},
                       Vector2{.x = device_top.d2.x, .y = device_top.d2.y}, thick, collision_color);
            DrawLineEx(Vector2{.x = device_bottom.d1.x, .y = device_bottom.d1.y},
                       Vector2{.x = device_bottom.d2.x, .y = device_bottom.d2.y}, thick, collision_color);
            DrawLineEx(Vector2{.x = device_left.d1.x, .y = device_left.d1.y},
                       Vector2{.x = device_left.d2.x, .y = device_left.d2.y}, thick, collision_color);
            DrawLineEx(Vector2{.x = device_right.d1.x, .y = device_right.d1.y},
                       Vector2{.x = device_right.d2.x, .y = device_right.d2.y}, thick, collision_color);
            DrawCircleLines((int) x, (int) y, rfid_radius, collision_color);
        }
    }


private:
    static Vector2 rotate(Vector2 pt, Vector2 center, float angle) {
        angle = angle * (PI / 180);
        return {.x = (float) ((pt.x - center.x) * cos(angle) - (pt.y - center.y) * sin(angle) + center.x),
                .y = (float) ((pt.x - center.x) * sin(angle) + (pt.y - center.y) * cos(angle) + center.y)};
    }

    static Device device_calculate(Vector2 side1, Vector2 side2, float radius) {
        float r1 = radius, r2 = radius;
        float x0 = side1.x, x1 = side2.x;
        float y0 = side1.y, y1 = side2.y;
        float d = sqrtf((side1.x - side2.x) * (side1.x - side2.x) +
                        (side1.y - side2.y) * (side1.y - side2.y));
        float a = (r1 * r1 - r2 * r2 + d * d) / (2 * d);
        float h = sqrtf(r1 * r1 - a * a);
        float x2 = x0 + a * (x1 - x0) / d, y2 = y0 + a * (y1 - y0) / d;
        float x3 = x2 + h * (y1 - y0) / d, y3 = y2 - h * (x1 - x0) / d;
        return Device{.d1 = {.x = x2, .y = y2}, .d2 = {.x = x3, .y = y3}, .distance = -1, .intersect = {.x = -1, .y = -1}};
    }
};

class Game final : public Context {
    std::vector<Object *> objects;
    std::vector<Resource> resources;
    struct Configuration configuration;
    int tile_index = 0, resource_index = 0, time = 0;
    Vector2 mouse;
    static std::string last_collision;
    const float height_delta = WINDOW_HEIGHT * 13.5;

    struct Color color_ground = {190, 33, 55, 50};
    struct Color color_gud = RED;
    bool go = false;
    bool default_readable = true;

    Loader *loader = nullptr;
public:
    enum ResourceId {
        Grass, Dirt, Things, Cow
    };

    explicit Game(Engine *engine, const char *const configuration_file) : Context(engine), resources(4),
                                                                          configuration{},
                                                                          mouse{} {
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

    void render() {
        draw_resources();
        for (auto &object: objects) {
            object->render();
        }
        draw_gud();
        if (visible_debug) {
            draw_other();
        }
    }

    void update() {
        auto engine = this->engine();
        auto writable = engine->write();
        if (go) {
            engine->tick();
        }

        time = (int) GetTime() * 100;
        mouse = GetMousePosition();

        for (auto object: objects) {
            object->update(this, time);
        }
        if (loader != nullptr) {
            writable->on_line(loader->get_on_line());
            writable->rfid_point(loader->get_point());
            writable->axis_angle(loader->get_angel());
            writable->distance_top(loader->get_uv_top().distance);
            writable->distance_bottom(loader->get_uv_bottom().distance);
            writable->distance_left(loader->get_uv_left().distance);
            writable->distance_right(loader->get_uv_right().distance);
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
        engine->set_default_readable(default_readable);
    }

    [[nodiscard]] const Resource &resource(ResourceId id) const {
        return resources[id];
    }

    [[nodiscard]] bool is_collision(Vector2 start, Vector2 stop) const override {
        auto di = get_tail(start.x, stop.y);
        if (di < 0 || is_ground(di)) {
            return true;
        }
        di = get_tail(stop.x, stop.y);
        if (di < 0 || is_ground(di)) {
            return true;
        }
        return false;
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

    [[nodiscard]] bool is_collision(Vector2 start, Vector2 stop, Vector2 &intersect) const override {
        auto di = get_tail(start.x, start.y);

        intersect.x = -1;
        intersect.y = -1;
        if (is_ground(di)) {
            auto rtl = Vector2{.x = (float) ((int) (start.x / TILE_SIZE) * TILE_SIZE), .y = (float) (
                    (int) (start.y / TILE_SIZE) * TILE_SIZE)};
            auto rtr = Vector2{.x = rtl.x + TILE_SIZE, .y = rtl.y};
            auto tbr = Vector2{.x = rtr.x, .y = rtr.y + TILE_SIZE};
            auto tbl = Vector2{.x = rtl.x, .y = rtr.y + TILE_SIZE};

            if (collision(start, stop, rtl, rtr, intersect)) {
                return true;
            }
            if (collision(start, stop, rtr, tbr, intersect)) {
                return true;
            }
            if (collision(start, stop, tbr, tbl, intersect)) {
                return true;
            }
            if (collision(start, stop, tbl, rtl, intersect)) {
                return true;
            }
        }
        di = get_tail(stop.x, stop.y);

        intersect.x = -1;
        intersect.y = -1;
        if (is_ground(di)) {
            auto rtl = Vector2{.x = (float) ((int) (stop.x / TILE_SIZE) * TILE_SIZE), .y = (float) (
                    (int) (stop.y / TILE_SIZE) * TILE_SIZE)};
            auto rtr = Vector2{.x = rtl.x + TILE_SIZE, .y = rtl.y};
            auto tbl = Vector2{.x = rtr.x, .y = rtr.y + TILE_SIZE};
            auto tbr = Vector2{.x = rtl.x, .y = rtr.y + TILE_SIZE};

            if (collision(start, stop, rtl, rtr, intersect)) {
                return true;
            }
            if (collision(start, stop, rtr, tbr, intersect)) {
                return true;
            }
            if (collision(start, stop, tbr, tbl, intersect)) {
                return true;
            }
            if (collision(start, stop, tbl, rtl, intersect)) {
                return true;
            }
        }
        return false;
    }

    [[nodiscard]] bool is_collision(Vector2 center, float radius) const override {
        (void) center;
        (void) radius;
        return false;
    }

    [[nodiscard]] int rfid_point(Vector2 center, float radius) const override {
        auto col = (size_t) (center.x / TILE_SIZE);
        auto row = (size_t) (center.y / TILE_SIZE);
        auto tail = (size_t) matrix_get(&configuration.paths, row, col);
        if (tail > 0 && tail < 20) {
            auto dx = (float) (((float) col * TILE_SIZE) + ((float) TILE_SIZE / 2));
            auto dy = (float) (((float) row * TILE_SIZE) + ((float) TILE_SIZE / 2));
            bool under = CheckCollisionCircles(
                    center, radius, Vector2{.x = dx, .y = dy}, 0.5f);
            return under ? (int) tail : 0;
        }
        return -1;
    }

    [[nodiscard]] bool rfid_point(float x, float y) const override {
        auto col = (size_t) (x / TILE_SIZE);
        auto row = (size_t) (y / TILE_SIZE);
        auto dx = (float) (((float) col * TILE_SIZE) + ((float) TILE_SIZE / 2));
        auto dy = (float) (((float) row * TILE_SIZE) + ((float) TILE_SIZE / 2));
        auto tail = (size_t) matrix_get(&configuration.paths, row, col);
        return tail > 0 && tail < 20 && dx == x && dy == y;
    }

    [[nodiscard]] bool line_point(float x, float y) const override {
        auto col = (size_t) (x / TILE_SIZE);
        auto row = (size_t) (y / TILE_SIZE);
        auto dx = (float) (col * TILE_SIZE);
        auto dy = (float) (row * TILE_SIZE);
        auto tail = (size_t) matrix_get(&configuration.paths, row, col);
        if (tail == 91 || tail == 92) {
            Vector2 sx = {.x = tail == 91 ? dx : dx + (float) TILE_SIZE / 2,
                    .y = tail == 91 ? dy + (float) TILE_SIZE / 2 : dy};
            Vector2 ex = {.x = tail == 91 ? dx + (float) TILE_SIZE : dx + (float) TILE_SIZE / 2,
                    .y = tail == 91 ? dy + (float) TILE_SIZE / 2 : dy + (float) TILE_SIZE};
            return CheckCollisionPointLine(Vector2{.x = x, .y = y}, sx, ex, 1);
        }
        return false;
    }

private:
    void draw_other() const {
        if (loader != nullptr && visible_debug) {
            auto device = loader->get_uv_top();
            if (device.distance >= 0) {
                DrawCircleLines((int) device.intersect.x, (int) device.intersect.y, 2.f, RED);
            }
            device = loader->get_uv_bottom();
            if (device.distance >= 0) {
                DrawCircleLines((int) device.intersect.x, (int) device.intersect.y, 2.f, RED);
            }
            device = loader->get_uv_left();
            if (device.distance >= 0) {
                DrawCircleLines((int) device.intersect.x, (int) device.intersect.y, 2.f, RED);
            }
            device = loader->get_uv_right();
            if (device.distance >= 0) {
                DrawCircleLines((int) device.intersect.x, (int) device.intersect.y, 2.f, RED);
            }
        }
    }

    int draw_gud() {
        int row = MARGIN_SIZE;
        float button_offset = 2 + MARGIN_SIZE;

        if (visible_debug) {
            DrawText(TextFormat("FPS: %i", GetFPS()), 2 + MARGIN_SIZE,
                     row + (int) height_delta + MARGIN_SIZE, 16, color_gud);
            DrawText(TextFormat("TLE: %i", tile_index), 70 + MARGIN_SIZE,
                     row + (int) height_delta + MARGIN_SIZE, 16, color_gud);
            DrawText(TextFormat("RSC: %i", resource_index), 130 + MARGIN_SIZE,
                     row + (int) height_delta + MARGIN_SIZE, 16, color_gud);
            const Resource &c = resource(static_cast<ResourceId>(resource_index));
            c.draw(tile_index, 190 + MARGIN_SIZE, (float) row + height_delta + MARGIN_SIZE);
            DrawRectangleLinesEx(
                    Rectangle{.x = 190 + MARGIN_SIZE, .y = (float) row + height_delta + MARGIN_SIZE,
                            .width = TILE_SIZE, .height = TILE_SIZE}, 1, color_gud);
            DrawText(TextFormat("TIM: %i", time), 220 + MARGIN_SIZE,
                     row + (int) height_delta + MARGIN_SIZE, 16, color_gud);
            DrawText(TextFormat("MSE: (%03i, %03i)", (int) mouse.x, (int) mouse.y), 425 + MARGIN_SIZE,
                     row + (int) height_delta + MARGIN_SIZE, 16, color_gud);


            row += MARGIN_SIZE + 15;
            DrawText("R + <Up>/<Down>   : TLE", 2 + MARGIN_SIZE,
                     row + (int) height_delta + MARGIN_SIZE, 16, color_gud);
            row += MARGIN_SIZE + 15;
            DrawText("R + <Left>/<Right>: RSC", 2 + MARGIN_SIZE,
                     row + (int) height_delta + MARGIN_SIZE, 16, color_gud);
            button_offset += 190;
        }
        go = GuiToggle(Rectangle{.x = button_offset, .y = (float) row + height_delta +
                                                          MARGIN_SIZE, .width = 30, .height = 16}, "POS", go);
        visible_debug = GuiToggle(Rectangle{.x = button_offset + 40,
                                          .y = (float) row + height_delta + MARGIN_SIZE, .width = 30, .height = 16},
                                  "DBG", visible_debug);
        default_readable = GuiToggle(Rectangle{.x = button_offset + 80,
                                             .y = (float) row + height_delta + MARGIN_SIZE, .width = 30, .height = 16},
                                     "RDR", default_readable);
        if (loader != nullptr && visible_debug) {
            row += MARGIN_SIZE + 15;
            DrawText(TextFormat("LDR: [%d, %d],  ANG: %f, POT: %d, ONL: %s",
                                (int) loader->get_x(), (int) loader->get_y(), loader->get_angel(), loader->get_point(),
                                loader->get_on_line() ? "true" : "false"),
                     2 + MARGIN_SIZE,
                     row + (int) height_delta + MARGIN_SIZE, 16, color_gud);
            row += MARGIN_SIZE + 15;
            DrawText(TextFormat("UV. T: %f, B: %f, L: %f, R: %f",
                                loader->get_uv_top().distance, loader->get_uv_bottom().distance,
                                loader->get_uv_left().distance, loader->get_uv_right().distance),
                     2 + MARGIN_SIZE,
                     row + (int) height_delta + MARGIN_SIZE, 16, color_gud);
        }
        return row;
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
                    if (idx == 0 || idx >= 90) {
                        continue;
                    }
                    int dx = (int) (row * TILE_SIZE) + TILE_SIZE / 2;
                    int dy = (int) (col * TILE_SIZE) + TILE_SIZE / 2;
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
                res.draw(i - 1, (float) row * res.size(), (float) col * res.size());
            }
        }
    }

    static bool is_ground(size_t id) {
        return id < 13;
    }

    [[nodiscard]] int get_tail(float x, float y) const {
        auto col = (size_t) (x / TILE_SIZE);
        auto row = (size_t) (y / TILE_SIZE);
        if (configuration.map.size <= row || configuration.map.size <= col)
            return -1;
        MapMatrixValue get = matrix_get(&configuration.map, row, col);
        return static_cast<int>(reinterpret_cast<size_t>(get));
    }


    static bool collision(Vector2 start, Vector2 end, Vector2 ground_start, Vector2 ground_end, Vector2 &intersect) {
#if 0
        float A1 = end.y - start.y;
        float B1 = start.x - end.x;
        float C1 = A1 * start.x + B1 * start.y;
        float A2 = ground_end.y - ground_start.y;
        float B2 = ground_start.x - ground_end.x;
        float C2 = A2 * ground_start.x + B2 * ground_start.y;
        float det = A1 * B2 - A2 * B1;
        if (det != 0) {
            intersect.x = (B2 * C1 - B1 * C2) / det;
            intersect.y = (A1 * C2 - A2 * C1) / det;
            return true;
        }
#else

        float uA = ((ground_end.x - ground_start.x) * (start.y - ground_start.y) -
                    (ground_end.y - ground_start.y) * (start.x - ground_start.x)) /
                   ((ground_end.y - ground_start.y) * (end.x - start.x) -
                    (ground_end.x - ground_start.x) * (end.y - start.y));
        float uB = ((end.x - start.x) * (start.y - ground_start.y) - (end.y - start.y) * (start.x - ground_start.x)) /
                   ((ground_end.y - ground_start.y) * (end.x - start.x) -
                    (ground_end.x - ground_start.x) * (end.y - start.y));
        if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1) {

            intersect.x = start.x + (uA * (end.x - start.x));
            intersect.y = start.y + (uA * (end.y - start.y));
            if (visible_debug) {
                std::stringstream ss;
                std::string collision;

                ss << "uA: " << uA << ", uB: " << uB
                   << ", sX: " << start.x << ", sY: " << start.y
                   << ", eX: " << end.x << ", eY: " << end.y
                   << ", gsX: " << ground_start.x << ", eY: " << ground_start.y
                   << ", geX: " << ground_end.x << ", geY: " << ground_end.y;
                collision = ss.str();
                if (last_collision != collision) {
                    std::cout << collision << std::endl;
                    last_collision = collision;
                }
            }
            return true;
        }
#endif
        intersect.x = -1;
        intersect.y = -1;
        return false;
    }

public:
    virtual ~Game() {
        for (auto it = objects.begin(); it != objects.end(); ++it) {
            delete *it;
            objects.erase(it);
        }
    }
};

std::string Game::last_collision = {};

int
main(int argc, char **argv) {
    (void) argc;
    (void) argv;
    auto engine = new EngineStated([](EngineStated *engine) {
        std::cout << "EngineStated: tick" << std::endl;
    });

    InitWindow(WINDOW_WIDTH * TILE_SIZE + MARGIN_SIZE, WINDOW_HEIGHT * TILE_SIZE + MARGIN_SIZE, "Транспортер");
    SetTargetFPS(60);
    Game g(engine, "factory.json");
    g.cow(50, 50);
    g.cow(200, 300);
    g.cow(430, 500);
    g.add_loader(136, 424);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        g.update();
        g.render();
        EndDrawing();
        if (IsKeyPressed(KEY_C)) {
            TakeScreenshot("export.png");
        }
    }
    CloseWindow();
    matrix_destroy(&configuration.map);
    return EXIT_SUCCESS;
}
