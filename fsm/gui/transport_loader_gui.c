#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <configuration.h>
#include <transport_loader.h>
#include <common.h>
#include <raylib.h>

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

struct Resource {
    Texture texture;
    Rectangle *rects;
    int rects_length;
    int rect_size;
};

static void resource_load(struct Resource *resource, const char *filename, int size) {
    int i, j, k = 0;

    resource->rect_size = size;
    resource->texture = LoadTexture(filename);
    resource->rects_length = (resource->texture.height / size) * (resource->texture.width / size);
    resource->rects = (Rectangle *) calloc(resource->rects_length, sizeof(Rectangle));
    for (i = 0; i < resource->texture.width / size; ++i) {
        for (j = 0; j < resource->texture.height / size; ++j) {
            resource->rects[k].x = (float) (i * size);
            resource->rects[k].y = (float) (j * size);
            resource->rects[k].height = (float) (size);
            resource->rects[k].width = (float) (size);
            ++k;
        }
    }
    SetTextureFilter(resource->texture, TEXTURE_FILTER_TRILINEAR);
}

static void write_cow(struct Resource *cow, int x, int y, int i) {
    Rectangle src, dest;
    Vector2 origin = {.x = 0.0f, .y = 0.0f};

    switch (i) {
        default:
        case 0: {
            src = cow->rects[0];
            dest.x = (float) x;
            dest.y = (float) y;
            dest.width = (float)cow->rect_size;
            dest.height = (float)cow->rect_size;
            DrawTextureTiled(cow->texture, src, dest, origin, 0.f, 1.f, RAYWHITE);

            src = cow->rects[4];
            dest.x = (float) (x + cow->rect_size);
            dest.y = (float) y;
            dest.width = (float)cow->rect_size;
            dest.height = (float)cow->rect_size;
            DrawTextureTiled(cow->texture, src, dest, origin, 0.f, 1.f, RAYWHITE);

            src = cow->rects[1];
            dest.x = (float) x;
            dest.y = (float) (y + cow->rect_size);
            dest.width = (float)cow->rect_size;
            dest.height = (float)cow->rect_size;
            DrawTextureTiled(cow->texture, src, dest, origin, 0.f, 1.f, RAYWHITE);

            src = cow->rects[5];
            dest.x = (float) (x + cow->rect_size);
            dest.y = (float) (y + cow->rect_size);
            dest.width = (float)cow->rect_size;
            dest.height = (float)cow->rect_size;
            DrawTextureTiled(cow->texture, src, dest, origin, 0.f, 1.f, RAYWHITE);
            break;
        }
        case 1: {
            src = cow->rects[8];
            dest.x = (float) x;
            dest.y = (float) y;
            dest.width = (float)cow->rect_size;
            dest.height = (float)cow->rect_size;
            DrawTextureTiled(cow->texture, src, dest, origin, 0.f, 1.f, RAYWHITE);

            src = cow->rects[12];
            dest.x = (float) (x + cow->rect_size);
            dest.y = (float) y;
            dest.width = (float)cow->rect_size;
            dest.height = (float)cow->rect_size;
            DrawTextureTiled(cow->texture, src, dest, origin, 0.f, 1.f, RAYWHITE);

            src = cow->rects[9];
            dest.x = (float) x;
            dest.y = (float) (y + cow->rect_size);
            dest.width = (float)cow->rect_size;
            dest.height = (float)cow->rect_size;
            DrawTextureTiled(cow->texture, src, dest, origin, 0.f, 1.f, RAYWHITE);

            src = cow->rects[13];
            dest.x = (float) (x + cow->rect_size);
            dest.y = (float) (y + cow->rect_size);
            dest.width = (float)cow->rect_size;
            dest.height = (float)cow->rect_size;
            DrawTextureTiled(cow->texture, src, dest, origin, 0.f, 1.f, RAYWHITE);
            break;
        }
        case 2: {
            src = cow->rects[16];
            dest.x = (float) x;
            dest.y = (float) y;
            dest.width = (float)cow->rect_size;
            dest.height = (float)cow->rect_size;
            DrawTextureTiled(cow->texture, src, dest, origin, 0.f, 1.f, RAYWHITE);

            src = cow->rects[20];
            dest.x = (float) (x + cow->rect_size);
            dest.y = (float) y;
            dest.width = (float)cow->rect_size;
            dest.height = (float)cow->rect_size;
            DrawTextureTiled(cow->texture, src, dest, origin, 0.f, 1.f, RAYWHITE);

            src = cow->rects[17];
            dest.x = (float) x;
            dest.y = (float) (y + cow->rect_size);
            dest.width = (float)cow->rect_size;
            dest.height = (float)cow->rect_size;
            DrawTextureTiled(cow->texture, src, dest, origin, 0.f, 1.f, RAYWHITE);

            src = cow->rects[21];
            dest.x = (float) (x + cow->rect_size);
            dest.y = (float) (y + cow->rect_size);
            dest.width = (float)cow->rect_size;
            dest.height = (float)cow->rect_size;
            DrawTextureTiled(cow->texture, src, dest, origin, 0.f, 1.f, RAYWHITE);
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
    struct Resource grass = {0},dirt = {0}, things = {0}, cow = {0};
    struct Resource *resources[] = {&grass, &dirt, &things, &cow}, *selected;
    size_t row, col;
    int tile_index = 0, resource_index = 0, resources_size = sizeof(resources)/ sizeof(resources[0]), cow_count = 0, time = 0;
    Vector2 origin = {.x = 0.0f, .y = 0.0f}, mouse;

    (void) argc;
    (void) argv;
    read_configuration(&configuration, "factory.json");
    InitWindow((int) configuration.map.size * TILE_SIZE + MARGIN_SIZE, (int) configuration.map.size * TILE_SIZE + MARGIN_SIZE, "Transport");

    resource_load(&grass, "assets/Tilesets/Grass.png", TILE_SIZE);
    resource_load(&dirt, "assets/Tilesets/Tilled.Dirt.png", TILE_SIZE);
    resource_load(&things, "assets/Objects/Basic.Grass.Biom.Things.1.png", TILE_SIZE);
    resource_load(&cow, "assets/Characters/Free.Cow.Sprites.png", TILE_SIZE);

    SetTargetFPS(60);

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
            if (tile_index >= selected->rects_length)
                tile_index = 0;
        } else if (IsKeyPressed(KEY_DOWN)) {
            tile_index--;
            if (tile_index < 0) {
                tile_index = selected->rects_length - 1;
            }
        }
        
        for (row = 0; row < configuration.map.size; ++row) {
            for (col = 0; col < configuration.map.size; ++col) {
                Rectangle dest;
                struct Resource *c;
                size_t idx = (size_t) matrix_get(&configuration.map, col, row);
                int    real;

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

                Rectangle src = c->rects[real];
                if (real == 0) {
                    c = selected;
                    src = selected->rects[tile_index];
                }

                dest.x = (float) (row * c->rect_size);
                dest.y = (float) (col * c->rect_size);
                dest.width = (float)c->rect_size;
                dest.height = (float)c->rect_size;

                DrawTextureTiled(c->texture, src, dest, origin, 0.f, 1.f, RAYWHITE);
            }
        }
        for (row = 0; row < configuration.things.size; ++row) {
            for (col = 0; col < configuration.things.size; ++col) {
                size_t idx = (size_t) matrix_get(&configuration.things, col, row);
                if (idx == 0)
                    continue;
                Rectangle dest;
                Rectangle src = things.rects[idx - 1];
                dest.x = (float) (row * things.rect_size);
                dest.y = (float) (col * things.rect_size);
                dest.width = (float)things.rect_size;
                dest.height = (float)things.rect_size;

                DrawTextureTiled(things.texture, src, dest, origin, 0.f, 1.f, RAYWHITE);
            }
        }
        for (row = 0; row < configuration.paths.size; ++row) {
            for (col = 0; col < configuration.paths.size; ++col) {
                size_t idx = (size_t) matrix_get(&configuration.paths, col, row);
                if (idx == 0)
                    continue;
                Rectangle dest;
                Rectangle src = dirt.rects[idx - 1];
                dest.x = (float) (row * dirt.rect_size);
                dest.y = (float) (col * dirt.rect_size);
                dest.width = (float)dirt.rect_size;
                dest.height = (float)dirt.rect_size;

                DrawTextureTiled(dirt.texture, src, dest, origin, 0.f, 1.f, RAYWHITE);
            }
        }

        time = GetTime() * 100;
        if (time % 66 == 0) {
            ++cow_count;
            if (cow_count > 1) {
                cow_count = 0;
            }
        }
        write_cow(&cow, 50, 50, cow_count);
        write_cow(&cow, 200, 300, cow_count);
        write_cow(&cow, 430, 500, cow_count);

        DrawText(TextFormat("%i FPS", GetFPS()), 2 + MARGIN_SIZE, 2 + MARGIN_SIZE, 8, RED);
        DrawText(TextFormat("%i tile", tile_index), 2 + MARGIN_SIZE, 10 + MARGIN_SIZE, 8, RED);
        DrawText(TextFormat("%i resource", resource_index), 2 + MARGIN_SIZE, 18 + MARGIN_SIZE, 8, RED);
        DrawText(TextFormat("%i time", time), 2 + MARGIN_SIZE, 26 + MARGIN_SIZE, 8, RED);
        DrawText(TextFormat("Mouse: (%f, %f)", mouse.x, mouse.y), 2 + MARGIN_SIZE, 32 + MARGIN_SIZE, 8, RED);

        EndDrawing();

        if (IsKeyPressed(KEY_C)) {
            Image screen = LoadImageFromScreen();
            ExportImage(screen, "export.png");
        }
    }
    UnloadTexture(grass.texture);
    CloseWindow();
    matrix_destroy(&configuration.map);
    return EXIT_SUCCESS;
}
