#include <iostream>
#include <vector>
#include <conio.h>
#include <cstdlib>
#include <ctime>
using namespace std;

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

int main() {
    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 600;
    Player player;
        player.x = SCREEN_WIDTH / 2;
        player.y = SCREEN_HEIGHT - 50;
        player.height = 20;
        player.width = 40;
        player.speed = 5;
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
            e.speed = 2;
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
                }
            }
        }
    }
    system("cls");
    cout << "GAME OVER ☠️" << endl;
    cout << "Final Score: " << score << endl;
    return 0;
}