#include <iostream>
#include <vector>
#include <conio.h>
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

int main() {
    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 600;
    Player player;
        player.x = SCREEN_WIDTH / 2;
        player.y = SCREEN_HEIGHT - 50;
        player.height = 20;
        player.width = 40;
        player.speed = 5;
    if (player.x < 0) {// محدودیت حرکت سفینه
        player.x = 0;
    }
    if (player.x + player.width > SCREEN_WIDTH) {
        player.x = SCREEN_WIDTH - player.width;
    }
    vector<Bullet> bullets; // وکتوری از نوع struct Bullet
    vector<Enemy> enemies;
    handleInput(player, bullets);
    for (auto &e : enemies)
        {
            e.y += e.speed;
        }
        for (auto &b : bullets)
        {
            b.y -= b.speed;
        }
}