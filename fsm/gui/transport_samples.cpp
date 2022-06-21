#include <math.h>
#include <raylib.h>
#include <raymath.h>

int main() {
    InitWindow(500, 500, "Hello World");
    SetTargetFPS(60);

    Vector2 player = {30.0f, 70.0f};
    float playerAngle;

    Vector2 enemy = {300.0f, 300.f};
    int hp = 20;

    while (!WindowShouldClose()) {
        if (IsKeyDown(KEY_A)) player.x -= 2.0f;
        if (IsKeyDown(KEY_D)) player.x += 2.0f;
        if (IsKeyDown(KEY_W)) player.y -= 2.0f;
        if (IsKeyDown(KEY_S)) player.y += 2.0f;

        playerAngle = atanf(player.y / player.x) * RAD2DEG;

        BeginDrawing();
        ClearBackground(BLACK);

        DrawCircleV(player, 20.0f, DARKGREEN);
        DrawLineV(Vector2Zero(), player, GREEN);

        DrawCircleSectorLines(Vector2Zero(), 30, 90, (int) (90 - playerAngle), 100, DARKBLUE);
        DrawText(TextFormat("%.03f°", playerAngle), (int) Vector2Zero().x + 30 + 5, 15, 10, DARKBLUE);

        if (hp > 0) {
            DrawText(TextFormat("HP: %d", hp), 330, 30, 20, DARKPURPLE);
            DrawCircleV(enemy, 20.0f, DARKPURPLE);
            if (Vector2Subtract(enemy, player).x <= 80.0f && Vector2Subtract(enemy, player).x >= -80.0f && Vector2Subtract(enemy, player).y <= 80.0f && Vector2Subtract(enemy, player).y >= -80.0f)
                DrawLineV(player, enemy, PURPLE);

            DrawText(TextFormat("Relation: %.03f", Vector2DotProduct(Vector2Normalize((Vector2){enemy.x + 5, enemy.y}), Vector2Normalize(Vector2Subtract(enemy, player)))), 330, 50, 20, DARKPURPLE);
        }

        DrawText(TextFormat("Player vector (%.03f, %.03f)", player.x, player.y), 20, 400, 20, LIGHTGRAY);
        DrawText(TextFormat("Player vector length from the origin %.03f", Vector2Length(player)), 20, 430, 20, LIGHTGRAY);
        DrawText(TextFormat("Distance between player and enemy %.03f", Vector2Length(Vector2Subtract(enemy, player))), 20, 460, 20, LIGHTGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}