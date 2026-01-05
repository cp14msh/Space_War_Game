#include <iostream>
#include <vector>
#include <conio.h>
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include <algorithm>
using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int WIDTH = 40;
const int HEIGHT = 20;
struct Bullet {
    float x, y;
    int height, width, speed;
};
struct Player {
    float x, y;
    int height, width, speed;
};
struct Enemy {
    float x, y;
    int height, width, speed;
};
void handleInput(Player &player, vector<Bullet> &bullets)
{
    if (_kbhit()) {              // آیا کلیدی زده شده؟
        char key = _getch();     // کلید چی بوده؟

        if (key == 'a') {        // چپ
            player.x -= player.speed;
        }
        else if (key == 'd') {   // راست
            player.x += player.speed;
        }
        else if (key == 'w') {   // بالا
            player.y -= player.speed;
        }
        else if (key == 's') {   // پایین
            player.y += player.speed;
        }
        else if (key == ' ') {   // Space → شلیک
            bullets.push_back({player.x + 20, player.y, 4, 10, 8});
        }
    }
}
bool checkCollisionPlayerEnemy(Player &player, Enemy &e) {//خورده یا نخورده (true/false)
    return (
        player.x < e.x + e.width &&
        player.x + player.width > e.x &&
        player.y < e.y + e.height &&
        player.y + player.height > e.y
    );
}
bool checkCollisionBulletsEnemy(Bullet &bullets, Enemy &e) {//خورده یا نخورده (true/false)
    return (
        bullets.x < e.x + e.width &&
        bullets.x + bullets.width > e.x &&
        bullets.y < e.y + e.height &&
        bullets.y + bullets.height > e.y
    );
}
void clearScreen(char screen[HEIGHT][WIDTH]) {//پاک کردن buffer در ابتدای هر حلقه:
    for (int y = 0; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++)
            screen[y][x] = ' ';
}
void drawPlayer(char screen[HEIGHT][WIDTH], Player &player) {//تغییر جدید
    int x = (int)(player.x / (SCREEN_WIDTH / WIDTH));
    int y = (int)(player.y / (SCREEN_HEIGHT / HEIGHT));

    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        screen[y][x] = 'A';  // A = سفینه
    }
}
void drawBullets(char screen[HEIGHT][WIDTH], vector<Bullet> &bullets) {//تغییر جدید : مقیاس پدیری موقعیت ها با صفحه نمایش
    for (auto &b : bullets) {
        int x = (int)(b.x / (SCREEN_WIDTH / WIDTH));
        int y = (int)(b.y / (SCREEN_HEIGHT / HEIGHT));

        if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT)
            screen[y][x] = '|';
    }
}
void drawEnemies(char screen[HEIGHT][WIDTH], vector<Enemy> &enemies) {//تغییر جدید 
    for (auto &e : enemies) {
        int x = (int)(e.x / (SCREEN_WIDTH / WIDTH));
        int y = (int)(e.y / (SCREEN_HEIGHT / HEIGHT));

        if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT)
            screen[y][x] = 'X';
    }
}
void moveCursorToTop() {
    COORD coord = {0, 0};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
void hideCursor() {
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(console, &info);
}
void render(char screen[HEIGHT][WIDTH], int score) {
    moveCursorToTop();   // 👈 کلیدی‌ترین خط

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            cout << screen[y][x];
        }
        cout << '\n';
    }

    cout << "Score: " << score << "      \n";
}

int main() {
    hideCursor();
    Player player;
        player.x = SCREEN_WIDTH / 2;
        player.y = SCREEN_HEIGHT - 50;
        player.height = 20;
        player.width = 40;
        player.speed = 40;
    char screen[HEIGHT][WIDTH];
    vector<Bullet> bullets; // وکتوری از نوع struct Bullet
    vector<Enemy> enemies;
    srand(time(0));
    int enemySpawnCounter = 0;
    int score = 0;
    int enemyBaseSpeed = 2;
    bool gameover = false;
    while (!gameover) {
        enemySpawnCounter++;
        if (score > 10) {
            enemyBaseSpeed = 3;
        }
        if (score > 20) {
            enemyBaseSpeed = 4;
        }
        if (enemySpawnCounter > 50) {
            Enemy e;
            e.x = rand() % 760;
            e.y = -20;
            e.width = 40;
            e.height = 20;
            e.speed = enemyBaseSpeed;
            enemies.push_back(e);
            enemySpawnCounter = 0;
        }
        if (player.x < 0) {// محدودیت حرکت سفینه
        player.x = 0;
        }
        if (player.x + player.width > SCREEN_WIDTH) {
            player.x = SCREEN_WIDTH - player.width;
        }
        handleInput(player, bullets);
        for (auto &e : enemies)
        {
            e.y += e.speed;
           if (checkCollisionPlayerEnemy(player, e)) {
                gameover = true;
                break;
            }
        }
        if (gameover) {
            render(screen, score); // آخرین وضعیت بازی
            Sleep(500); 
            break;
        }
        for (auto &b : bullets)
        {
            b.y -= b.speed;
        }
        for (int i = 0; i < bullets.size(); i++) {
            for (int j = 0; j < enemies.size(); j++) {

                if (checkCollisionBulletsEnemy(bullets[i], enemies[j])) {
                    // حذف تیر
                    bullets.erase(bullets.begin() + i);
                    i--;
                    // حذف دشمن
                    enemies.erase(enemies.begin() + j);
                    j--;
                    score++;
                    break;
                }
            }
        }
        // حذف تیرهای خارج صفحه
        bullets.erase(
            std::remove_if(bullets.begin(), bullets.end(),[](Bullet &b){ return b.y < 0; }),
            bullets.end()
        );

        // حذف دشمنان خارج صفحه
        enemies.erase(
            std::remove_if(enemies.begin(), enemies.end(),[](Enemy &e){ return e.y > SCREEN_HEIGHT; }),
            enemies.end()
        );
        //آماده‌سازی Back Buffer
        clearScreen(screen);
        drawPlayer(screen, player);
        drawBullets(screen, bullets);
        drawEnemies(screen, enemies);
        //system("cls");               جایگزینی با کروسر
        render(screen, score);
        Sleep(30);
    }
        // حذف تیرهای خارج صفحه
        bullets.erase(
            std::remove_if(bullets.begin(), bullets.end(),[](Bullet &b){ return b.y < 0; }),
            bullets.end()
        );

        // حذف دشمنان خارج صفحه
        enemies.erase(
            std::remove_if(enemies.begin(), enemies.end(),[](Enemy &e){ return e.y > SCREEN_HEIGHT; }),
            enemies.end()
        );
    system("cls");
    cout << "GAME OVER ☠️" << endl;
    cout << "Final Score: " << score << endl;
    return 0;
}