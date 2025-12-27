#include <iostream>
#include <vector>
#include <string>
#include <conio.h>
#include <windows.h>
#include <chrono>
#include <thread>
#include <time.h>

using namespace std;
using namespace std::chrono;

// --- Constants --- //
const int WIDTH = 60;
const int HEIGHT = 40;
const int FRAME_DURATION_MS = 33; // Target ~30 FPS

// --- Structures & Classes ---//

struct Bullet
{
    int x, y;
    bool active;
};

struct Enemy
{
    int x, y;
    bool active;
    steady_clock::time_point lastMoveTime;
};

class Enemy2
{
public:
    int x, y;
    int health_enemy2;
    int width, height;
    bool active;

    vector<string> shape_enemy2;
    steady_clock::time_point lastMoveTime;

    Enemy2()
    {
        shape_enemy2 = {"<0>"};
        height = shape_enemy2.size();
        width = shape_enemy2[0].length();
        health_enemy2 = 2;
    }
};

class Enemy3
{
public:
    int x, y;
    int health_enemy3;
    int width, height;
    bool active;

    vector<string> shape_enemy3;
    steady_clock::time_point lastMoveTime;
    steady_clock::time_point lastShootTime;

    Enemy3()
    {
        shape_enemy3 = {
            " /M\\ ",
            " \\W/ "};
        height = shape_enemy3.size();
        width = shape_enemy3[0].length();
        health_enemy3 = 5;
        lastShootTime = steady_clock::now();
    }
};

class Question_Box
{
public:
    int x, y;
    int width, height;
    bool active;
    int q_r;

    vector<string> shape_box;
    steady_clock::time_point lastMoveTime;

    Question_Box()
    {
        shape_box =
            {
                "+---+",
                "|Q B|",
                "+---+"};
        height = shape_box.size();
        width = shape_box[0].length();
    }
};

class Player
{
public:
    int x, y;
    int width, height;
    vector<string> shape;
    int hp = 3;
    int weaponLevel = 1;

    Player(int startX, int startY)
    {
        x = startX;
        y = startY;
        shape = {
            "  ^  ",
            " /|\\ ",
            " |_| ",
            " / \\ "};
        height = shape.size();
        width = shape[0].length();
    }

    void moveLeft()
    {
        if (x > 0)
            x--;
    }

    void moveRight()
    {
        if (x + width < WIDTH)
            x++;
    }
};

void ShowGameOverScreen(int score)
{

    system("cls");

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
    cout << "\n\n\n\n";
    cout << "\t\t\t#########################################\n";
    cout << "\t\t\t#                                       #\n";
    cout << "\t\t\t#              GAME OVER!               #\n";
    cout << "\t\t\t#                                       #\n";
    cout << "\t\t\t#########################################\n";
    cout << "\t\t\t#                                       #\n";
    cout << "\t\t\t#          FINAL SCORE: " << score << "\t\t#\n";
    cout << "\t\t\t#                                       #\n";
    cout << "\t\t\t#########################################\n";
    cout << "\n";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    cout << "\t\t\t      Press 'Y' to Retry or 'N' to Exit\n";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}

int main()
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hOut, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(hOut, &cursorInfo);

    srand(time(0));
    bool keepPlaying = true;

    while (keepPlaying)
    {
        // --- Initialization ---//
        Player player(WIDTH / 2 - 2, HEIGHT - 5);

        vector<Bullet> bullets;
        vector<Enemy> enemies;
        vector<Enemy2> enemies2;
        vector<Enemy3> enemies3;
        vector<Bullet> enemyBullets;
        vector<Question_Box> box;

        bool gameOver = false;
        int hits = 0;

        // Timers
        auto lastShotTime = steady_clock::now();
        auto lastFrameTime = steady_clock::now();
        auto lastEnemySpawnTime = steady_clock::now();
        auto lastEnemySpawnTime2 = steady_clock::now();
        auto lastEnemySpawnTime3 = steady_clock::now();
        auto lastEnemySpawnTime_b = steady_clock::now();
        auto lastEnemyBulletMoveTime = steady_clock::now();

        while (!gameOver)
        {
            // ========================
            // 1. INPUT
            // ========================
            if (GetAsyncKeyState('A') & 0x8000)
                player.moveLeft();
            if (GetAsyncKeyState('D') & 0x8000)
                player.moveRight();
            if (GetAsyncKeyState('X') & 0x8000)
                gameOver = true;

            if (GetAsyncKeyState(VK_SPACE) & 0x8000)
            {
                auto now = steady_clock::now();
                if (duration_cast<milliseconds>(now - lastShotTime).count() > 200)
                {

                    if (player.weaponLevel == 1)
                    {
                        bullets.push_back({player.x + 2, player.y - 1, true});
                    }
                    else if (player.weaponLevel == 2)
                    {
                        bullets.push_back({player.x + 2, player.y - 1, true});
                        bullets.push_back({player.x + 3, player.y - 1, true});
                    }
                    else if (player.weaponLevel == 3)
                    {
                        bullets.push_back({player.x + 1, player.y - 1, true});
                        bullets.push_back({player.x + 2, player.y - 1, true});
                        bullets.push_back({player.x + 3, player.y - 1, true});
                    }
                    else
                    {
                        bullets.push_back({player.x + 1, player.y - 1, true});
                        bullets.push_back({player.x + 2, player.y - 1, true});
                        bullets.push_back({player.x + 3, player.y - 1, true});
                        bullets.push_back({player.x + 4, player.y - 1, true});
                    }

                    lastShotTime = now;
                }
            }
            // ========================
            // 2. LOGIC
            // ========================

            // --- Player Bullets ---
            for (auto &b : bullets)
            {
                b.y--;
                if (b.y < 0)
                    b.active = false;
            }

            for (size_t i = 0; i < bullets.size();)
            {
                if (!bullets[i].active)
                    bullets.erase(bullets.begin() + i);
                else
                    i++;
            }

            // --- Enemy 1 Logic ---//
            auto now = steady_clock::now();
            if (hits >= 0 && hits <= 100)
            {
                if (duration_cast<milliseconds>(now - lastEnemySpawnTime).count() > 1000)
                {
                    Enemy e;
                    e.y = 1;
                    e.x = rand() % (WIDTH - 2) + 1;
                    e.active = true;
                    e.lastMoveTime = now;
                    enemies.push_back(e);
                    lastEnemySpawnTime = now;
                }
            }
            else if (hits > 100)
            {
                if (duration_cast<milliseconds>(now - lastEnemySpawnTime).count() > 2000)
                {
                    Enemy e;
                    e.y = 1;
                    e.x = rand() % (WIDTH - 2) + 1;
                    e.active = true;
                    e.lastMoveTime = now;
                    enemies.push_back(e);
                    lastEnemySpawnTime = now;
                }
            }

            for (auto &e : enemies)
            {
                if (duration_cast<milliseconds>(now - e.lastMoveTime).count() > 150)
                {
                    e.y++;
                    e.lastMoveTime = now;
                }
                if (e.y > HEIGHT)
                    e.active = false;

                // Collision with Player
                if (e.active && e.x >= player.x && e.x < player.x + player.width && e.y >= player.y && e.y < player.y + player.height)
                {
                    if (player.weaponLevel > 1)
                    {
                        player.weaponLevel--;
                    }
                    else
                    {
                        player.hp--;
                    }
                    e.active = false;
                    Beep(500, 50);
                }
            }

            for (size_t i = 0; i < enemies.size();)
            {
                if (!enemies[i].active)
                    enemies.erase(enemies.begin() + i);
                else
                    i++;
            }

            // --- Enemy 2 Logic ---//
            auto now2 = steady_clock::now();
            if (hits > 20)
            {
                if (duration_cast<milliseconds>(now2 - lastEnemySpawnTime2).count() > 2000)
                {
                    Enemy2 e2;
                    e2.y = 1;
                    e2.x = rand() % (WIDTH - 4) + 1;
                    e2.active = true;
                    e2.lastMoveTime = now2;
                    enemies2.push_back(e2);
                    lastEnemySpawnTime2 = now2;
                }
            }

            for (auto &e2 : enemies2)
            {
                if (duration_cast<milliseconds>(now2 - e2.lastMoveTime).count() > 250)
                {
                    e2.y++;
                    e2.lastMoveTime = now2;
                }
                if (e2.y > HEIGHT)
                    e2.active = false;

                // Collision with Player
                if (e2.active && player.x < e2.x + e2.width && player.x + player.width > e2.x && player.y < e2.y + e2.height && player.y + player.height > e2.y)
                {
                    if (player.weaponLevel > 1)
                    {
                        player.weaponLevel--;
                    }
                    else
                    {
                        player.hp--;
                    }
                    e2.active = false;
                    Beep(500, 100);
                }
            }

            for (size_t i = 0; i < enemies2.size();)
            {
                if (!enemies2[i].active)
                    enemies2.erase(enemies2.begin() + i);
                else
                    i++;
            }

            // --- Enemy 3 Logic (The Shooter) ---//
            auto now3 = steady_clock::now();
            if (hits > 40)
            {
                if (duration_cast<milliseconds>(now3 - lastEnemySpawnTime3).count() > 5000)
                {
                    Enemy3 e3;
                    e3.y = 1;
                    e3.x = rand() % (WIDTH - 6) + 1;
                    e3.active = true;
                    e3.lastMoveTime = now3;
                    enemies3.push_back(e3);
                    lastEnemySpawnTime3 = now3;
                }
            }

            for (auto &e3 : enemies3)
            {

                if (duration_cast<milliseconds>(now3 - e3.lastMoveTime).count() > 250)
                {
                    e3.y++;
                    e3.lastMoveTime = now3;
                }
                if (e3.y > HEIGHT)
                    e3.active = false;

                if (e3.active && player.x < e3.x + e3.width && player.x + player.width > e3.x && player.y < e3.y + e3.height && player.y + player.height > e3.y)
                {
                    if (player.weaponLevel > 2)
                    {
                        player.weaponLevel -= 2;
                    }
                    else if (player.weaponLevel == 2)
                    {
                        player.weaponLevel--;
                        player.hp--;
                    }
                    else if (player.weaponLevel == 1)
                    {
                        player.hp -= 2;
                    }
                    e3.active = false;
                    Beep(500, 100);
                }

                // *** Enemy3 shooting ***
                if (e3.active && duration_cast<milliseconds>(now3 - e3.lastShootTime).count() > 2000) // هر 2 ثانیه
                {
                    Bullet eb;
                    eb.x = e3.x + (e3.width / 2);
                    eb.y = e3.y + e3.height;
                    eb.active = true;
                    enemyBullets.push_back(eb);
                    e3.lastShootTime = now3;
                }
            }

            for (size_t i = 0; i < enemies3.size();)
            {
                if (!enemies3[i].active)
                    enemies3.erase(enemies3.begin() + i);
                else
                    i++;
            }

            // --- Enemy Bullets Movement & Logic ---//
            auto nowBullet = steady_clock::now();
            if (duration_cast<milliseconds>(nowBullet - lastEnemyBulletMoveTime).count() > 80) // سرعت تیر دشمن (سریع)
            {
                for (auto &eb : enemyBullets)
                {
                    eb.y++;
                    if (eb.y >= HEIGHT)
                        eb.active = false;
                }
                lastEnemyBulletMoveTime = nowBullet;
            }

            // Did the player get hit by enemy3 bullets?
            for (auto &eb : enemyBullets)
            {
                if (!eb.active)
                    continue;

                if (eb.x >= player.x && eb.x < player.x + player.width && eb.y >= player.y && eb.y < player.y + player.height)
                {
                    if (player.weaponLevel > 1)
                    {
                        player.weaponLevel--;
                    }
                    else
                    {
                        player.hp--;
                    }
                    eb.active = false;
                    Beep(1000, 50);
                }
            }

            for (size_t i = 0; i < enemyBullets.size();)
            {
                if (!enemyBullets[i].active)
                    enemyBullets.erase(enemyBullets.begin() + i);
                else
                    i++;
            }

            // --- B Q Logic ---//
            auto now_b = steady_clock::now();
            if (hits > 70)
            {
                if (duration_cast<milliseconds>(now_b - lastEnemySpawnTime_b).count() > 30000)
                {
                    Question_Box b;
                    b.y = 1;
                    b.x = rand() % (WIDTH - 6) + 1;
                    b.active = true;
                    b.lastMoveTime = now_b;
                    box.push_back(b);
                    lastEnemySpawnTime_b = now_b;
                }
            }
            for (auto &b : box)
            {
                if (duration_cast<milliseconds>(now_b - b.lastMoveTime).count() > 500)
                {
                    b.y++;
                    b.lastMoveTime = now_b;
                }
                if (b.y > HEIGHT)
                    b.active = false;

                // Collision with Player
                if (b.active && player.x < b.x + b.width && player.x + player.width > b.x && player.y < b.y + b.height && player.y + player.height > b.y)
                {
                    b.q_r = rand() % (5) + 1;
                    b.active = false;
                    Beep(1000, 100);
                    switch (b.q_r)
                    {
                    case 1:
                        player.hp = 1;
                        break;

                    case 2:
                        if (player.hp > 1)
                        {
                            player.hp -= 1;
                        }
                        break;

                    case 3:
                        if (player.hp < 3)
                        {
                            player.hp += 1;
                        }
                        break;

                    case 4:
                        if (player.weaponLevel < 4)
                        {
                            player.weaponLevel++;
                            Beep(600, 50);
                            Beep(800, 50);
                        }
                        break;

                    case 5:
                        hits += 10;
                        break;
                    }
                }
            }

            for (size_t i = 0; i < box.size();)
            {
                if (!box[i].active)
                    box.erase(box.begin() + i);
                else
                    i++;
            }

            // --- Collisions: Player Bullets vs Enemies ---//
            for (auto &b : bullets)
            {
                if (!b.active)
                    continue;

                // vs Enemy 1
                for (auto &e : enemies)
                {
                    if (e.active && b.x == e.x && (b.y == e.y || b.y == e.y + 1 || b.y == e.y - 1))
                    {
                        b.active = false;
                        e.active = false;
                        hits++;
                        Beep(700, 20);
                        break;
                    }
                }

                if (!b.active)
                    continue;

                // vs Enemy 2
                for (auto &e2 : enemies2)
                {
                    if (e2.active && b.x >= e2.x && b.x < e2.x + e2.width && b.y >= e2.y && b.y < e2.y + e2.height)
                    {
                        b.active = false;
                        if (e2.health_enemy2 <= 1)
                        {
                            e2.active = false;
                            hits += 2;
                            Beep(700, 20);
                        }
                        else
                        {
                            e2.health_enemy2--;
                            Beep(200, 20);
                        }
                        break;
                    }
                }

                if (!b.active)
                    continue;

                // vs Enemy 3
                for (auto &e3 : enemies3)
                {
                    if (e3.active && b.x >= e3.x && b.x < e3.x + e3.width && b.y >= e3.y && b.y < e3.y + e3.height)
                    {
                        b.active = false;
                        if (e3.health_enemy3 <= 1)
                        {
                            e3.active = false;
                            hits += 5;
                            Beep(800, 20);
                        }
                        else
                        {
                            e3.health_enemy3--;
                            Beep(200, 20);
                        }
                        break;
                    }
                }
            }

            if (player.hp <= 0)
                gameOver = true;

            // ========================
            // 3. RENDER
            // ========================
            SetConsoleCursorPosition(hOut, {0, 0});
            string buffer = "\n\n";
            buffer += "  ";

            for (int k = 0; k < WIDTH + 2; k++)
                buffer += "#";
            const string MARGIN_LEFT = "  |";

            for (int y = 0; y < HEIGHT; y++)
            {
                if (y != 0)
                    buffer += MARGIN_LEFT;

                for (int x = 0; x < WIDTH; x++)
                {
                    bool isBullet = false;
                    for (const auto &b : bullets)
                        if (b.x == x && b.y == y)
                        {
                            isBullet = true;
                            break;
                        }

                    bool isEnemyBullet = false;

                    for (const auto &eb : enemyBullets)
                        if (eb.x == x && eb.y == y)
                        {
                            isEnemyBullet = true;
                            break;
                        }

                    bool isEnemy = false;

                    for (auto &e : enemies)
                        if (e.x == x && e.y == y)
                        {
                            isEnemy = true;
                            break;
                        }

                    // Render Enemy 2
                    bool isEnemy2 = false;
                    char enemy2Char = ' ';

                    for (auto &e2 : enemies2)
                    {
                        if (x >= e2.x && x < e2.x + e2.width && y >= e2.y && y < e2.y + e2.height)
                        {
                            isEnemy2 = true;
                            enemy2Char = e2.shape_enemy2[y - e2.y][x - e2.x];
                            break;
                        }
                    }

                    // Render Enemy 3
                    bool isEnemy3 = false;
                    char enemy3Char = ' ';

                    for (auto &e3 : enemies3)
                    {
                        if (x >= e3.x && x < e3.x + e3.width && y >= e3.y && y < e3.y + e3.height)
                        {
                            isEnemy3 = true;
                            enemy3Char = e3.shape_enemy3[y - e3.y][x - e3.x];
                            break;
                        }
                    }

                    bool isbox = false;
                    char boxChar = ' ';

                    for (const auto &h : box)
                    {
                        bool insideX = (x >= h.x && x < h.x + h.width);
                        bool insideY = (y >= h.y && y < h.y + h.height);

                        if (insideX && insideY)
                        {
                            isbox = true;
                            boxChar = h.shape_box[y - h.y][x - h.x];
                            break;
                        }
                    }

                    bool insideX = (x >= player.x && x < player.x + player.width);
                    bool insideY = (y >= player.y && y < player.y + player.height);

                    if (insideX && insideY)
                        buffer += player.shape[y - player.y][x - player.x];

                    else if (isBullet)
                        buffer += "|";
                    else if (isEnemyBullet)
                        buffer += "o";
                    else if (isEnemy)
                        buffer += "M";
                    else if (isEnemy2)
                        buffer += enemy2Char;
                    else if (isEnemy3)
                        buffer += enemy3Char;
                    else if (isbox)
                    {
                        buffer += boxChar;
                    }

                    else
                        buffer += " ";

                    if (x == WIDTH - 1)
                        buffer += "|";
                    if (y == 1 && x == WIDTH - 1)
                        buffer += " HEALTH: " + to_string(player.hp);
                    if (y == 2 && x == WIDTH - 1)
                        buffer += " SCORE:  " + to_string(hits);
                }
                buffer += "\n";
            }

            cout << buffer;
            cout << "\n \n \n";
            cout << "__________________________________________________" << endl;
            cout << "|Controls: A/D to Move, SPACE to Shoot, X to Quit|" << endl;

            auto endnow = steady_clock::now();
            auto elapsed = duration_cast<milliseconds>(endnow - lastFrameTime);

            if (elapsed < milliseconds(FRAME_DURATION_MS))
                std::this_thread::sleep_for(milliseconds(FRAME_DURATION_MS) - elapsed);

            lastFrameTime = steady_clock::now();
        }

        ShowGameOverScreen(hits);
        FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));

        char input;
        cin >> input;
        if (input == 'y' || input == 'Y')
            keepPlaying = true;
        else
            keepPlaying = false;

        system("cls");
    }
    return 0;
}