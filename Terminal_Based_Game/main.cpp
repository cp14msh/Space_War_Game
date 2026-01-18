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

struct NewEnemy
{
    int x, y;
    bool active;
    int health_enemy;
    int width, height;
    steady_clock::time_point lastMoveTime;
    vector<string> shape_enemy;
    steady_clock::time_point lastShootTime;
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

void enemy_erase(vector<NewEnemy> &enemies)
{
    for (size_t i = 0; i < enemies.size();)
    {
        if (!enemies[i].active)
            enemies.erase(enemies.begin() + i);
        else
            i++;
    }
}

void SpawnEnemyType(int MinHits, int MaxHits, int spawnInterval, int health_enemy, int Hits, int width, vector<NewEnemy> &enemies,
                    vector<string> shape, steady_clock::time_point &lastSpawnTime, steady_clock::time_point &now)
{
    if (MaxHits >= Hits && Hits >= MinHits && duration_cast<milliseconds>(now - lastSpawnTime).count() > spawnInterval)
    {
        NewEnemy e;
        e.y = 1;
        e.x = rand() % (WIDTH - width) + 1;
        e.active = true;
        e.health_enemy = health_enemy;
        e.lastMoveTime = now;
        e.shape_enemy = shape;
        e.width = e.shape_enemy[0].length();
        e.height = e.shape_enemy.size();
        enemies.push_back(e);
        lastSpawnTime = now;
    }
}

void enemy_get_hit_bullets(vector<Bullet> &bullets, vector<NewEnemy> &enemies, int &hits, int point)
{
    for (auto &b : bullets)
    {
        if (!b.active)
            continue;
        for (auto &e : enemies)
        {
            if (e.active && b.x >= e.x && b.x < e.x + e.width && b.y >= e.y && b.y < e.y + e.height)
            {
                b.active = false;
                e.active = false;
                hits = point + hits;
                Beep(700, 20);
                break;
            }
        }
        if (!b.active)
            continue;
    }
}

void enemy_render(vector<NewEnemy> &enemies, bool &isEnemy, int x, int y, char &enemyChar)
{

    for (auto &e : enemies)
    {
        if (x >= e.x && x < e.x + e.width && y >= e.y && y < e.y + e.height)
        {
            isEnemy = true;
            enemyChar = e.shape_enemy[y - e.y][x - e.x];
            break;
        }
    }
}

void Enemy_Movement__Collision(vector<NewEnemy> &enemies, Player &player, steady_clock::time_point now)
{
    for (auto &e : enemies)
    {
        if (!e.active)
            continue;
        if (duration_cast<milliseconds>(now - e.lastMoveTime).count() > 250)
        {
            e.y++;
            e.lastMoveTime = now;
        }
        if (e.y > HEIGHT)
            e.active = false;

        // 2. Collision with Player
        if (e.active && player.x < e.x + e.width && player.x + player.width > e.x &&
            player.y < e.y + e.height && player.y + player.height > e.y)
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
            Beep(500, 100);
        }
    }
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
        vector<NewEnemy> enemies;
        vector<NewEnemy> enemies2;
        vector<NewEnemy> enemies3;
        vector<NewEnemy> enemies4;
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
        auto lastEnemySpawnTime4 = steady_clock::now();
        auto lastEnemySpawnTime_bq = steady_clock::now();
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

            SpawnEnemyType(0, 100, 1000, 1, hits, 2, enemies, {"A"}, lastEnemySpawnTime, now);
            SpawnEnemyType(100, 1000000, 2000, 1, hits, 2, enemies, {"A"}, lastEnemySpawnTime, now);

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

            enemy_erase(enemies);

            // --- Enemy 2 Logic ---//
            auto now2 = steady_clock::now();
            SpawnEnemyType(20, 1000000, 2000, 2, hits, 4, enemies2, {"<0>"}, lastEnemySpawnTime2, now2);

            Enemy_Movement__Collision(enemies2, player, now2);

            enemy_erase(enemies2);

            // --- Enemy 3 Logic (The Shooter1) ---//
            auto now3 = steady_clock::now();
            SpawnEnemyType(40, 1000000, 5000, 5, hits, 6, enemies3, {" /M\\ ", " \\W/ "}, lastEnemySpawnTime3, now3);

            Enemy_Movement__Collision(enemies3, player, now3);

            for (auto &e3 : enemies3)
            {
                // *** Enemy3 shooting ***
                if (e3.active && duration_cast<milliseconds>(now3 - e3.lastShootTime).count() > 2000)
                {
                    Bullet eb;
                    eb.x = e3.x + (e3.width / 2);
                    eb.y = e3.y + e3.height;
                    eb.active = true;
                    enemyBullets.push_back(eb);
                    e3.lastShootTime = now3;
                }
            }

            enemy_erase(enemies3);

            // --- Enemy Bullets Movement & Logic ---//
            auto nowBullet = steady_clock::now();
            if (duration_cast<milliseconds>(nowBullet - lastEnemyBulletMoveTime).count() > 80)
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

            // --- Enemy 4 Logic (The Shooter2) ---//
            auto now4 = steady_clock::now();
            SpawnEnemyType(100, 1000000, 5000, 8, hits, 6, enemies4, {" /TT\\ ", " \\VV/ "}, lastEnemySpawnTime4, now4);

            Enemy_Movement__Collision(enemies4, player, now4);

            for (auto &e4 : enemies4)
            {
                // *** Enemy4 shooting (Dual Shot) ***
                if (e4.active && duration_cast<milliseconds>(now4 - e4.lastShootTime).count() > 2000)
                {
                    Bullet eb1;
                    eb1.x = e4.x + 1;
                    eb1.y = e4.y + e4.height;
                    eb1.active = true;
                    enemyBullets.push_back(eb1);

                    Bullet eb2;
                    eb2.x = e4.x + e4.width - 2;
                    eb2.y = e4.y + e4.height;
                    eb2.active = true;
                    enemyBullets.push_back(eb2);

                    e4.lastShootTime = now4;
                }
            }

            enemy_erase(enemies4);

            // --- Enemy4 Bullets Movement & Logic ---//
            auto nowBullet2 = steady_clock::now();
            if (duration_cast<milliseconds>(nowBullet2 - lastEnemyBulletMoveTime).count() > 80) // سرعت تیر دشمن (سریع)
            {
                for (auto &eb : enemyBullets)
                {
                    eb.y++;
                    if (eb.y >= HEIGHT)
                        eb.active = false;
                }
                lastEnemyBulletMoveTime = nowBullet2;
            }

            // Did the player get hit by enemy4 bullets?
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
                if (duration_cast<milliseconds>(now_b - lastEnemySpawnTime_bq).count() > 30000)
                {
                    Question_Box b;
                    b.y = 1;
                    b.x = rand() % (WIDTH - 6) + 1;
                    b.active = true;
                    b.lastMoveTime = now_b;
                    box.push_back(b);
                    lastEnemySpawnTime_bq = now_b;
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

            // vs Enemy 1
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
            }
            // vs Enemy 2
            enemy_get_hit_bullets(bullets, enemies2, hits, 2);

            // vs Enemy 3
            enemy_get_hit_bullets(bullets, enemies3, hits, 5);

            // vs Enemy 4
            enemy_get_hit_bullets(bullets, enemies4, hits, 8);

            int score = hits;

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

                    enemy_render(enemies2, isEnemy2, x, y, enemy2Char);

                    // Render Enemy 3
                    bool isEnemy3 = false;
                    char enemy3Char = ' ';

                    enemy_render(enemies3, isEnemy3, x, y, enemy3Char);

                    // Render Enemy 4
                    bool isEnemy4 = false;
                    char enemy4Char = ' ';

                    enemy_render(enemies4, isEnemy4, x, y, enemy4Char);

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
                    else if (isEnemy4)
                        buffer += enemy4Char;
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
                    if (y == 3 && x == WIDTH - 1)
                        buffer += " WeaponLevel:  " + to_string(player.weaponLevel);
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