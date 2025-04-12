#include "raylib.h"

#define MAX_BLOCKS 36
#define MAX_ITEMS 10
//아이템 전역 변수 & 상수
#define ITEM_DURATION 300   // 5초 (60FPS 기준)
typedef struct {
    Vector2 position;
    bool active;
} Item;
Item items[MAX_ITEMS] = { 0 };
int itemTimer = 0;
bool paddleExpanded = false;

//bool은 true 값과 false 값만 받음 / vector는x y축으로 움직이는 속도 값 움직이는 애들 값이 들어옴 
//Rectangle 은 말 그대로 그냥 사각형 사이즈 받아서 보여주는거 / int 는 변수 정수 값 받는거 

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
    // 점수랑 목숨 초기화
    *score = 0;
    *lives = 3;
    // bool 값으로 줘서 0 과 1 True / false 값으로 저장해두고 true 값이 되면 밑에 if 문에서 사용할수 있게 설정
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
    // 초기 점수랑 목숨
    int score = 0;
    int lives = 3;
    // 초기 게임 클리어 밑 게임 오버 bool 값 초기화
    bool gameClear = false;
    bool gameOver = false;
    // 패들과 공
    Rectangle paddle = { 350, 550, 100, 20 };
    Vector2 ball = { 400, 300 };
    Vector2 speed = { 5, -5 };
    float radius = 10;


    // 블록들 색 위치 설정
    Rectangle blocks[MAX_BLOCKS];
    Color colors[6] = { RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE };
    bool active[MAX_BLOCKS] = { 0 };
    for (int i = 0; i < MAX_BLOCKS; i++) {
        blocks[i] = (Rectangle){ 20 + (i % 12) * 63, 50 + (i / 12) * 30, 60, 20 };
        active[i] = true;
    }
    // 리스타트 버튼 영역 (게임 종료 시 버튼으로 사용)
    Rectangle restartButton = { 350, 350, 100, 40 };
    // Restart 버튼 아래에 Exit 버튼 위치
    Rectangle exitButton = { 350, 400, 100, 40 }; 


    while (!WindowShouldClose()) {
        //게임 오버 또는 클리어 bool 값이 false 일경우 실행되는 if문
        if (!gameClear && !gameOver) {
            // 패들 이동
            if (IsKeyDown(KEY_LEFT)) paddle.x -= 8;
            if (IsKeyDown(KEY_RIGHT)) paddle.x += 8;

            // 공 이동
            ball.x += speed.x;
            ball.y += speed.y;

            // 벽과 공이 충돌 했을때 실행되는 if 문
            if (ball.x < radius || ball.x > 800 - radius) speed.x *= -1;
            if (ball.y < radius) speed.y *= -1;

            // 바닥에 닿을때 목숨을 하나 빼고 만약에 목숨이 0보다 작거나 같으면 bool값이 들어있는 gameOver를 true로 변경
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

            // 모든 블록 제거 시 게임 클리어 처리 (bool 값인 gameClear를 true로 변경
            bool allCleared = true;
            for (int i = 0; i < MAX_BLOCKS; i++) {
                if (active[i]) { allCleared = false; break; }
            }
            if (allCleared) gameClear = true;

            // 패들과 공이 충돌 했을때 실행되는 if 문
            if (CheckCollisionCircleRec(ball, radius, paddle)) speed.y *= -1;

            // 블록과 공이 충돌했을때 실행되는 if 문
            for (int i = 0; i < MAX_BLOCKS; i++) {
                if (active[i] && CheckCollisionCircleRec(ball, radius, blocks[i])) {
                    active[i] = false;
                    speed.y *= -1;
                    score++;

                    // 50% 확률로 아이템 생성해주는 if 문
                    if (GetRandomValue(0, 1) == 1) {
                        for (int j = 0; j < MAX_ITEMS; j++) {
                            if (!items[j].active) {
                                items[j].position = (Vector2){ blocks[i].x + blocks[i].width / 2, blocks[i].y + blocks[i].height };
                                items[j].active = true;
                                break;
                            }
                        }
                    }


                    break;
                }
            }


            // 아이템 떨어뜨리기
            for (int i = 0; i < MAX_ITEMS; i++) {
                if (items[i].active) {
                    items[i].position.y += 3.0f; // 아이템 떨어지는 속도

                    Rectangle itemRect = { items[i].position.x - 10, items[i].position.y - 10, 20, 20 };
                    // 아이템과 paddle이 서로 충돌하면 실행되는 if문
                    if (CheckCollisionRecs(itemRect, paddle)) {
                        items[i].active = false;
                        paddle.width = 150.0f;
                        paddleExpanded = true;
                        itemTimer = ITEM_DURATION;
                    }
                    if (items[i].position.y > 600) items[i].active = false;
                }
            }

            // 패들 확장 타이머 처리
            if (paddleExpanded) {
                itemTimer--;
                if (itemTimer <= 0) {
                    paddle.width = 100.0f;
                    paddleExpanded = false;
                }
            }

        }
        else {
            // 게임오버 또는 게임클리어 상태일 때: 마우스 입력으로 리스타트 버튼 처리
            Vector2 mousePoint = GetMousePosition();
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mousePoint, restartButton)) {
                ResetGame(&paddle, &ball, &speed, blocks, active, &score, &lives, &gameClear, &gameOver);
            }
            // while 루프를 빠져나가서 프로그램 종료 게임 종료 / 게임 종려시키는 코드
            else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mousePoint, exitButton)) {
                break; 
            }
        }


        // 그리기
        BeginDrawing();

        ClearBackground(BLACK);
        // 목숨 표시
        DrawText(TextFormat("Lives: %d", lives), 10, 20, 20, RAYWHITE);
        //아이템 설명
        DrawText(TextFormat("ITEM : Paddle size up"), 300, 20, 20, RAYWHITE);

        //paddle 늘리기 추가 5초가량 paddle 사이즈 증가
        for (int i = 0; i < MAX_ITEMS; i++) {
            if (items[i].active) {
                DrawCircleV(items[i].position, 10, SKYBLUE);
            }
        }


        // 게임 클리어 메시지 출력
        if (gameClear) {
            const char* clearMessage = "GAME CLEAR!";
            int fontSize = 40;
            int textWidth = MeasureText(clearMessage, fontSize);
            DrawText(clearMessage, (800 - textWidth) / 2, 250 - fontSize / 2, fontSize, GREEN);
        }
        // 게임 오버 화면 출력
        if (gameOver) {
            const char* overMessage = "GAME OVER!";
            int fontSize = 40;
            int textWidth = MeasureText(overMessage, fontSize);
            DrawText(overMessage, (800 - textWidth) / 2, 250 - fontSize / 2, fontSize, RED);
        }

        //  or 연산자로 둘중 하나라도 활성화 된다면 화면에 버튼들 출력
        //게임 종료 상태일 때 리스타트 버튼 표시
        if (gameOver || gameClear) {
            DrawRectangleRec(restartButton, GRAY);
            int btnFontSize = 20;
            int btnTextWidth = MeasureText("Restart", btnFontSize);
            DrawText("Restart", restartButton.x + (restartButton.width - btnTextWidth) / 2,
                restartButton.y + (restartButton.height - btnFontSize) / 2,
                btnFontSize, WHITE);

            // Exit 버튼 출력
            DrawRectangleRec(exitButton, DARKGRAY);
            int exitTextWidth = MeasureText("Exit", btnFontSize);
            DrawText("Exit", exitButton.x + (exitButton.width - exitTextWidth) / 2,
                exitButton.y + (exitButton.height - btnFontSize) / 2, btnFontSize, WHITE);
        }

        // 점수 표시
        DrawText(TextFormat("Score: %d", score), 650, 20, 20, WHITE);

        // 페들 그리기
        DrawRectangleRec(paddle, WHITE);
        //공 그리기
        DrawCircleV(ball, radius, WHITE);
        for (int i = 0; i < MAX_BLOCKS; i++)
            if (active[i]) DrawRectangleRec(blocks[i], colors[i / 12]);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}