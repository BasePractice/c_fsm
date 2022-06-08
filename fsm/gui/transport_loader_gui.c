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

#define RESOURCE_GRASS 0
#define RESOURCE_DIRT  1

static int map[] = {
        24, 23, 14, 25, 34, 13, 15, 35, 33, 44,
        45, 55, 54, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
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
    int rects_size;
};

static void resource_load(struct Resource *resource, const char *filename) {
    int i, j, k = 0;

    resource->texture = LoadTexture(filename);
    resource->rects_size = (resource->texture.height / 16) * (resource->texture.width / 16);
    resource->rects = (Rectangle *) calloc(resource->rects_size, sizeof(Rectangle));
    for (i = 0; i < resource->texture.height / 16; ++i) {
        for (j = 0; j < resource->texture.width / 16; ++j) {
            resource->rects[k].x = (float) (i * TILE_SIZE);
            resource->rects[k].y = (float) (j * TILE_SIZE);
            resource->rects[k].height = (float) (TILE_SIZE);
            resource->rects[k].width = (float) (TILE_SIZE);
            ++k;
        }
    }
    SetTextureFilter(resource->texture, TEXTURE_FILTER_TRILINEAR);
}

int
main(int argc, char **argv) {
    struct Resource grass = {0},dirt = {0};
    struct Resource *resources[] = {&grass, &dirt}, *selected = 0;
    size_t row, col;
    int tile_index = 0, resource_index = 0;
    Vector2 origin = {.x = 0.0f, .y = 0.0f};

    (void) argc;
    (void) argv;
    read_configuration(&configuration, "factory.json");
    InitWindow((int) configuration.map.size * TILE_SIZE + MARGIN_SIZE, (int) configuration.map.size * TILE_SIZE + MARGIN_SIZE, "Transport");

    resource_load(&grass, "assets/Tilesets/Grass.png");
    resource_load(&dirt, "assets/Tilesets/Tilled.Dirt.png");

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (IsKeyPressed(KEY_LEFT)) {
            resource_index++;
            if (resource_index > 1) {
                resource_index = 0;
            }
        } else if (IsKeyPressed(KEY_RIGHT)) {
            resource_index--;
            if (resource_index < 0) {
                resource_index = 1;
            }
        }
        selected = resources[resource_index];

        if (IsKeyPressed(KEY_UP)) {
            tile_index++;
            if (tile_index >= selected->rects_size)
                tile_index = 0;
        } else if (IsKeyPressed(KEY_DOWN)) {
            tile_index--;
            if (tile_index < 0) {
                tile_index = selected->rects_size - 1;
            }
        }

        for (row = 0; row < configuration.map.size; ++row) {
            for (col = 0; col < configuration.map.size; ++col) {
                Rectangle dest = {(float) (row * TILE_SIZE), (float) (col * TILE_SIZE), TILE_SIZE, TILE_SIZE};
                struct Resource *c;
                size_t idx = (size_t) matrix_get(&configuration.map, col, row);
                int    real;

                if (idx > 50) {
                    c = resources[RESOURCE_DIRT];
                    real = map[idx];
                } else {
                    real = map[idx];
                    c = resources[RESOURCE_GRASS];
                }

                Rectangle src = c->rects[real];
                if (real == 0) {
                    c = selected;
                    src = selected->rects[tile_index];
                }
                DrawTextureTiled(c->texture, src, dest, origin, 0.f, 1.f, RAYWHITE);
            }
        }
        DrawText(TextFormat("%i FPS", GetFPS()), 2 + MARGIN_SIZE, 2 + MARGIN_SIZE, 8, RED);
        DrawText(TextFormat("%i tile", tile_index), 2 + MARGIN_SIZE, 10 + MARGIN_SIZE, 8, RED);
        DrawText(TextFormat("%i resource", resource_index), 2 + MARGIN_SIZE, 18 + MARGIN_SIZE, 8, RED);
        EndDrawing();
    }
    UnloadTexture(grass.texture);
    CloseWindow();
    matrix_destroy(&configuration.map);
    return EXIT_SUCCESS;
}
