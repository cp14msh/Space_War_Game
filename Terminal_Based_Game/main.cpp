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
    int type;
};

struct Question_Box
{
    int x, y;
    int width, height;
    bool active;
    int q_r;

    vector<string> shape_box;
    steady_clock::time_point lastMoveTime;

    Question_Box() // Constructor
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

struct Player
{
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

    void moveUp()
    {
        if (y > 0)
            y--;
    }

    void moveDown()
    {
        if (y + height < HEIGHT)
            y++;
    }
};

// --- Helper Functions --- //

void ShowInstructions()
{
    system("cls");
    cout << "INSTRUCTIONS:\n";
    cout << "- Use 'W', 'A', 'S', 'D' to move your spaceship\n";
    cout << "- Press SPACE to shoot bullets\n";
    cout << "- Avoid enemies and shoot them\n";
    cout << "\nPress any key to return to menu...";
    _getch(); // conio.h
}

int ShowMenu()
{
    int choice;
    do
    {
        system("cls");
        cout << "SPACE WAR GAME\n";
        cout << "==================\n";
        cout << "1. Start Game\n";
        cout << "2. Instructions\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice < 1 || choice > 3)
        {
            cout << "Invalid choice! Try again.\n";
            this_thread::sleep_for(milliseconds(1000));
        }

    } while (choice < 1 || choice > 3);

    return choice;
}

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
    cout << "\t\t\t      Press 'Y' to Retry or 'N' to Menu\n";
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
                    vector<string> shape, steady_clock::time_point &lastSpawnTime, steady_clock::time_point &now, int type)
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
        e.type = type;
        enemies.push_back(e);
        lastSpawnTime = now;
    }
}

void enemy_get_hit_bullets(vector<Bullet> &bullets, vector<NewEnemy> &enemies, int &hits)
{
    for (auto &b : bullets)
    {
        if (!b.active)
            continue;
        for (auto &e : enemies)
        {
            if (e.active && b.x >= e.x && b.x < e.x + e.width && b.y >= e.y - 1 && b.y < e.y + e.height + 1)
            {
                b.active = false;

                e.health_enemy--;
                if (e.health_enemy <= 0)
                {
                    e.active = false;

                    int points = 1;
                    switch (e.type)
                    {
                    case 1:
                        points = 1;
                        break;
                    case 2:
                        points = 2;
                        break;
                    case 3:
                        points = 5;
                        break;
                    case 4:
                        points = 8;
                        break;
                    }
                    hits += points;
                    Beep(700, 20);
                }
                else
                {
                    Beep(400, 10);
                }
                break;
            }
        }
        if (!b.active)
            continue;
    }
}

int main()
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE); // give console
    CONSOLE_CURSOR_INFO cursorInfo;                // dwSize, bVisible
    GetConsoleCursorInfo(hOut, &cursorInfo);       // hOut ==> cursorInfo
    cursorInfo.bVisible = false;                   //
    SetConsoleCursorInfo(hOut, &cursorInfo);       // cursorInfo ==> hOut

    srand(time(0));
    bool appRunning = true;

    while (appRunning)
    {
        int menuChoice = ShowMenu();

        if (menuChoice == 3)
            break;
        if (menuChoice == 2)
        {
            ShowInstructions();
            continue;
        }

        if (menuChoice == 1)
        {
            // --- Loading Screen Logic ---
            system("cls");
            bool cancelLoading = false;
            auto startLoad = steady_clock::now();

            while (duration_cast<seconds>(steady_clock::now() - startLoad).count() < 5)
            {
                // Simple centering for Loading text
                cout << "\n\n\r\t      LOADING... " << 5 - duration_cast<seconds>(steady_clock::now() - startLoad).count() << "s";
                cout << "\n\n\n\tPress any key to back to menu...";

                if (_kbhit()) //_kbhit() ==> yes/no
                {
                    _getch(); // take key
                    cancelLoading = true;
                    break;
                }
                this_thread::sleep_for(milliseconds(100));
                system("cls");
            }

            if (cancelLoading)
                continue;
        }

        bool keepPlaying = true;
        while (keepPlaying)
        {
            Player player(WIDTH / 2 - 2, HEIGHT - 5);
            vector<Bullet> bullets;
            vector<NewEnemy> enemies;
            vector<Bullet> enemyBullets;
            vector<Question_Box> box;

            bool gameOver = false;
            int hits = 0;

            auto lastShotTime = steady_clock::now();
            auto lastFrameTime = steady_clock::now();
            auto lastEnemySpawnTime = steady_clock::now();
            auto lastEnemySpawnTime2 = steady_clock::now();
            auto lastEnemySpawnTime3 = steady_clock::now();
            auto lastEnemySpawnTime4 = steady_clock::now();
            auto lastEnemySpawnTime_bq = steady_clock::now();
            auto lastEnemyBulletMoveTime = steady_clock::now();

            // --- Game Logic --- //
            while (!gameOver)
            {
                if (GetAsyncKeyState('A') & 0x8000)
                    player.moveLeft();
                if (GetAsyncKeyState('D') & 0x8000)
                    player.moveRight();
                if (GetAsyncKeyState('W') & 0x8000)
                    player.moveUp();
                if (GetAsyncKeyState('S') & 0x8000)
                    player.moveDown();
                if (GetAsyncKeyState('X') & 0x8000)
                    gameOver = true;

                if (GetAsyncKeyState(VK_SPACE) & 0x8000)
                {
                    // player shooting
                    auto now = steady_clock::now();
                    if (duration_cast<milliseconds>(now - lastShotTime).count() > 200)
                    {
                        if (player.weaponLevel == 1)
                            bullets.push_back({player.x + 2, player.y - 1, true});
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

                // player bullet moving
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

                auto now = steady_clock::now();
                SpawnEnemyType(0, 100, 1000, 1, hits, 2, enemies, {"A"}, lastEnemySpawnTime, now, 1);
                SpawnEnemyType(100, 1000000, 2000, 1, hits, 2, enemies, {"A"}, lastEnemySpawnTime, now, 1);
                SpawnEnemyType(20, 1000000, 2000, 2, hits, 4, enemies, {"<0>"}, lastEnemySpawnTime2, now, 2);
                SpawnEnemyType(40, 1000000, 5000, 5, hits, 6, enemies, {" /M\\ ", " \\W/ "}, lastEnemySpawnTime3, now, 3);
                SpawnEnemyType(100, 1000000, 5000, 8, hits, 6, enemies, {" /TT\\ ", " \\VV/ "}, lastEnemySpawnTime4, now, 4);

                for (auto &e : enemies)
                {
                    if (!e.active)
                        continue;
                    int moveSpeed = (e.type == 1) ? 200 : 250;
                    if (duration_cast<milliseconds>(now - e.lastMoveTime).count() > moveSpeed)
                    {
                        e.y++;
                        e.lastMoveTime = now;
                    }
                    if (e.y > HEIGHT)
                        e.active = false;

                    if (e.active && player.x < e.x + e.width && player.x + player.width > e.x &&
                        player.y < e.y + e.height && player.y + player.height > e.y)
                    {
                        if (player.weaponLevel > 1)
                            player.weaponLevel--;
                        else
                            player.hp--;
                        e.active = false;
                        Beep(500, 50);
                    }

                    // enemy shooting
                    switch (e.type)
                    {
                    case 3:
                        if (e.active && duration_cast<milliseconds>(now - e.lastShootTime).count() > 2000)
                        {
                            enemyBullets.push_back({e.x + (e.width / 2), e.y + e.height, true});
                            e.lastShootTime = now;
                        }
                        break;
                    case 4:
                        if (e.active && duration_cast<milliseconds>(now - e.lastShootTime).count() > 2000)
                        {
                            enemyBullets.push_back({e.x + 1, e.y + e.height, true});
                            enemyBullets.push_back({e.x + e.width - 2, e.y + e.height, true});
                            e.lastShootTime = now;
                        }
                        break;
                    }
                }

                enemy_erase(enemies);

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

                for (auto &eb : enemyBullets)
                {
                    if (eb.active && eb.x >= player.x && eb.x < player.x + player.width && eb.y >= player.y && eb.y < player.y + player.height)
                    {
                        if (player.weaponLevel > 1)
                            player.weaponLevel--;
                        else
                            player.hp--;
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

                auto now_b = steady_clock::now();
                if (hits > 70 && duration_cast<milliseconds>(now_b - lastEnemySpawnTime_bq).count() > 30000)
                {
                    Question_Box b;
                    b.y = 1;
                    b.x = rand() % (WIDTH - 6) + 1;
                    b.active = true;
                    b.lastMoveTime = now_b;
                    box.push_back(b);
                    lastEnemySpawnTime_bq = now_b;
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
                    if (b.active && player.x < b.x + b.width && player.x + player.width > b.x && player.y < b.y + b.height && player.y + player.height > b.y)
                    {
                        b.q_r = rand() % 5 + 1;
                        b.active = false;
                        Beep(1000, 100);
                        if (b.q_r == 1)
                            player.hp = 1;
                        else if (b.q_r == 2 && player.hp > 1)
                            player.hp--;
                        else if (b.q_r == 3 && player.hp < 3)
                            player.hp++;
                        else if (b.q_r == 4 && player.weaponLevel < 4)
                        {
                            player.weaponLevel++;
                            Beep(600, 50);
                            Beep(800, 50);
                        }
                        else if (b.q_r == 5)
                            hits += 10;
                    }
                }
                for (size_t i = 0; i < box.size();)
                {
                    if (!box[i].active)
                        box.erase(box.begin() + i);
                    else
                        i++;
                }

                enemy_get_hit_bullets(bullets, enemies, hits);

                if (player.hp <= 0)
                    gameOver = true;

                // --- Rendring --- //
                SetConsoleCursorPosition(hOut, {0, 0});
                string buffer = "\n\n  ";
                for (int k = 0; k < WIDTH + 2; k++)
                    buffer += "#";
                for (int y = 0; y < HEIGHT; y++)
                {
                    buffer += "\n  |";
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

                        // enemy render
                        bool isEnemy = false;
                        char enemyChar = ' ';
                        for (auto &e : enemies)
                            if (x >= e.x && x < e.x + e.width && y >= e.y && y < e.y + e.height)
                            {
                                isEnemy = true;
                                enemyChar = e.shape_enemy[y - e.y][x - e.x];
                                break;
                            }

                        // question box render
                        bool isBox = false;
                        char bChar = ' ';
                        for (const auto &h : box)
                            if (x >= h.x && x < h.x + h.width && y >= h.y && y < h.y + h.height)
                            {
                                isBox = true;
                                bChar = h.shape_box[y - h.y][x - h.x];
                                break;
                            }

                        // player render
                        if (x >= player.x && x < player.x + player.width && y >= player.y && y < player.y + player.height)
                            buffer += player.shape[y - player.y][x - player.x];

                        else if (isBullet)
                            buffer += "|";
                        else if (isEnemyBullet)
                            buffer += "o";
                        else if (isEnemy)
                            buffer += (enemyChar == 'A' ? 'M' : enemyChar);
                        else if (isBox)
                            buffer += bChar;
                        else
                            buffer += " ";

                        if (x == WIDTH - 1)
                        {
                            buffer += "|";
                            if (y == 1)
                                buffer += " HEALTH: " + to_string(player.hp);
                            if (y == 2)
                                buffer += " SCORE:  " + to_string(hits);
                            if (y == 3)
                                buffer += " WeaponLevel: " + to_string(player.weaponLevel);
                        }
                    }
                }
                cout << buffer << "\n\n\n__________________________________________________\n|Controls: W/A/S/D to Move, SPACE to Shoot, X to Quit|";

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
            keepPlaying = (input == 'y' || input == 'Y');
            system("cls");
        }
    }
    return 0;
}