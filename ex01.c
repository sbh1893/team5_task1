#include "raylib.h"

#define MAX_BLOCKS 36

//게임 초기화 함수
int ResetGame(Rectangle* paddle, Vector2* ball, Vector2* speed, Rectangle blocks[], bool active[], int* score, int* lives, bool* gameClear, bool* gameOver) {
    // 패들, 공, 속도, 점수, 목숨, 상태 초기화
    paddle->x = 350.0f;
    paddle->y = 550.0f;
    paddle->width = 100.0f;
    paddle->height = 20.0f;

    ball->x = 400.0f;
    ball->y = 300.0f;

    speed->x = 5.0f;
    speed->y = -5.0f;

    *score = 0;
    *lives = 3;
    *gameClear = false;
    *gameOver = false;

    // 블록과 활성 상태 초기화
    for (int i = 0; i < MAX_BLOCKS; i++) {
        blocks[i].x = 20.0f + (i % 12) * 63.0f;
        blocks[i].y = 50.0f + (i / 12) * 30.0f;
        blocks[i].width = 60.0f;
        blocks[i].height = 20.0f;
        active[i] = true;
    }

    return 0; // 함수는 int형이므로 return 필요
}




int main() {
    InitWindow(800, 600, "Block");
    SetTargetFPS(60);
    int score = 0;
    int lives = 3;
    bool gameClear = false;
    bool gameOver = false;
    // 패들과 공
    Rectangle paddle = { 350, 550, 100, 20 };
    Vector2 ball = { 400, 300 };
    Vector2 speed = { 5, -5 };
    float radius = 10;


    // 블록들
    Rectangle blocks[MAX_BLOCKS];
    Color colors[6] = { RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE };
    bool active[MAX_BLOCKS] = { 0 };
    for (int i = 0; i < MAX_BLOCKS; i++) {
        blocks[i] = (Rectangle){ 20 + (i % 12) * 63, 50 + (i / 12) * 30, 60, 20 };
        active[i] = true;
    }
    // 리스타트 버튼 영역 (게임 종료 시 버튼으로 사용)
    Rectangle restartButton = { 350, 350, 100, 40 };

    while (!WindowShouldClose()) {
        if (!gameClear && !gameOver) {
            // 패들 이동
            if (IsKeyDown(KEY_LEFT)) paddle.x -= 8;
            if (IsKeyDown(KEY_RIGHT)) paddle.x += 8;

            // 공 이동
            ball.x += speed.x;
            ball.y += speed.y;

            // 벽 충돌
            if (ball.x < radius || ball.x > 800 - radius) speed.x *= -1;
            if (ball.y < radius) speed.y *= -1;

            // 바닥 목숨이 없을경우 게임 오버 시키기
            if (ball.y > 600) {
                lives--;
                if (lives <= 0) {
                    gameOver = true;
                    lives = 0;
                }
                else {
                    ball = (Vector2){ 400, 300 };
                }
            }
            
            // 모든 블록 제거 시 게임 클리어 처리
            bool allCleared = true;
            for (int i = 0; i < MAX_BLOCKS; i++) {
                if (active[i]) { allCleared = false; break; }
            }
            if (allCleared) gameClear = true;
            
           

            // 패들 충돌
            if (CheckCollisionCircleRec(ball, radius, paddle)) speed.y *= -1;

            // 블록 충돌
            for (int i = 0; i < MAX_BLOCKS; i++) {
                if (active[i] && CheckCollisionCircleRec(ball, radius, blocks[i])) {
                    active[i] = false;
                    speed.y *= -1;
                    score ++;
                    break;
                }
            } 
      
        }
        else {
            // 게임오버 또는 게임클리어 상태일 때: 마우스 입력으로 리스타트 버튼 처리
            Vector2 mousePoint = GetMousePosition();
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mousePoint, restartButton)) {
                ResetGame(&paddle, &ball, &speed, blocks, active, &score, &lives, &gameClear, &gameOver);
            }
        }


         

        // 그리기
        BeginDrawing();

        ClearBackground(BLACK);
        // 목숨 표시
        DrawText(TextFormat("Lives: %d", lives), 10, 20, 20, RAYWHITE);


        // 게임 클리어와 게임 오버 메시지 출력
        if (gameClear) {
            const char* clearMessage = "GAME CLEAR!";
            int fontSize = 40;
            int textWidth = MeasureText(clearMessage, fontSize);
            DrawText(clearMessage, (800 - textWidth) / 2, 250 - fontSize / 2, fontSize, GREEN);
        }
        if (gameOver) {
            const char* overMessage = "GAME OVER!";
            int fontSize = 40;
            int textWidth = MeasureText(overMessage, fontSize);
            DrawText(overMessage, (800 - textWidth) / 2, 250 - fontSize / 2, fontSize, RED);
        }

        // 게임 종료 상태일 때 리스타트 버튼 표시
        if (gameOver || gameClear) {
            DrawRectangleRec(restartButton, GRAY);
            int btnFontSize = 20;
            int btnTextWidth = MeasureText("Restart", btnFontSize);
            DrawText("Restart", restartButton.x + (restartButton.width - btnTextWidth) / 2,
                restartButton.y + (restartButton.height - btnFontSize) / 2,
                btnFontSize, WHITE);
        }

        // 점수 표시
        DrawText(TextFormat("Score: %d", score), 650, 20, 20, WHITE);


        DrawRectangleRec(paddle, WHITE);
        DrawCircleV(ball, radius, WHITE);
        for (int i = 0; i < MAX_BLOCKS; i++)
            if (active[i]) DrawRectangleRec(blocks[i], colors[i / 12]);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}   