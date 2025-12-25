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

// --- Constants ---
const int WIDTH = 60;
const int HEIGHT = 40;
const int FRAME_DURATION_MS = 33; // Target ~30 FPS

// --- Structures & Classes ---

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
        // ASCII Art for the spaceship
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

    Enemy3()
    {
        // ASCII Art for the spaceship
        shape_enemy3 = {
            " /M\\ ",
            " \\W/ "};
        height = shape_enemy3.size();
        width = shape_enemy3[0].length();
        health_enemy3 = 5;
    }
};

class Player
{
public:
    int x, y;
    int width, height;
    vector<string> shape;
    int hp = 3;

    Player(int startX, int startY)
    {
        x = startX;
        y = startY;
        // ASCII Art for the spaceship
        shape = {
            "  ^  ",
            " /|\\ ",
            " |_| ",
            " / \\ "};
        height = shape.size();
        width = shape[0].length();
    }

    // Boundary checks are included in movement logic
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

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Change the text color (and background) for all future console output to color code

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
    // Console Setup: Hide cursor to avoid flickering artifacts
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hOut, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(hOut, &cursorInfo);

    // Timing variables
    auto lastShotTime = steady_clock::now();
    auto lastFrameTime = steady_clock::now();
    auto lastEnemySpawnTime = steady_clock::now();
    auto lastEnemySpawnTime2 = steady_clock::now();
    auto lastEnemySpawnTime3 = steady_clock::now();

    srand(time(0));

    // --- Main Game Loop ---
    bool keepPlaying = true;
    while (keepPlaying)
    {

        // --- Initialization ---
        Player player(WIDTH / 2 - 2, HEIGHT - 5);
        vector<Bullet> bullets;
        vector<Enemy> enemies;
        vector<Enemy2> enemies2;
        vector<Enemy3> enemies3;
        bool gameOver = false;
        int hits = 0;

        while (!gameOver)
        {

            // 1. Input Handling
            // 0x8000 checks the high-order bit (key is currently down)
            if (GetAsyncKeyState('A') & 0x8000)
                player.moveLeft();
            if (GetAsyncKeyState('D') & 0x8000)
                player.moveRight();
            if (GetAsyncKeyState('X') & 0x8000)
                gameOver = true;

            if (GetAsyncKeyState(VK_SPACE) & 0x8000)
            {
                auto now = steady_clock::now();
                // Fire rate limiter: 200ms cooldown
                if (duration_cast<milliseconds>(now - lastShotTime).count() > 200)
                {
                    bullets.push_back({player.x + 2, player.y - 1, true});
                    lastShotTime = now;
                }
            }

            // 2. Game Logic (Update)
            // Move bullets up
            for (auto &b : bullets)
            {
                b.y--;
                if (b.y < 0)
                    b.active = false;
            }

            // Remove inactive bullets (Erase-Remove idiom logic)
            for (size_t i = 0; i < bullets.size();)
            {
                if (!bullets[i].active)
                    bullets.erase(bullets.begin() + i);
                else
                    i++;
            }

            // Enemy spawn limiter: 1000ms
            auto now = steady_clock::now();
            if (duration_cast<milliseconds>(now - lastEnemySpawnTime).count() > 1000)
            {
                Enemy e;
                e.y = 1;

                e.x = rand() % (WIDTH - 2) + 1;
                e.active = true;
                e.lastMoveTime = steady_clock::now();

                enemies.push_back(e);
                lastEnemySpawnTime = now;
            }

            // Move enemies down
            for (int i = 0; i < enemies.size(); i++)
            {
                auto now = steady_clock::now();

                if (duration_cast<milliseconds>(now - enemies[i].lastMoveTime).count() > 150)
                {
                    enemies[i].y++;
                    enemies[i].lastMoveTime = now;
                }

                if (enemies[i].y > HEIGHT)
                    enemies[i].active = false;

                if (enemies[i].active &&
                    enemies[i].x >= player.x &&
                    enemies[i].x < player.x + player.width &&
                    enemies[i].y >= player.y &&
                    enemies[i].y < player.y + player.height)
                {
                    player.hp -= 1;
                    enemies[i].active = false;
                    Beep(500, 50); // Beep( Frequency , Duration );
                }

                if (player.hp == 0)
                {
                    gameOver = true;
                }
            }

            // Remove inactive enemies
            for (int i = 0; i < enemies.size();)
            {
                if (!enemies[i].active)
                {
                    enemies.erase(enemies.begin() + i);
                }
                else
                {
                    i++;
                }
            }

            // Enemy2 spawn limiter: 1000ms
            auto now2 = steady_clock::now();
            if (hits > 20)
            {
                if (duration_cast<milliseconds>(now2 - lastEnemySpawnTime2).count() > 2000)
                {
                    Enemy2 e2;
                    e2.y = 1;
                    e2.x = rand() % (WIDTH - e2.width - 2) + 1;
                    e2.lastMoveTime = steady_clock::now();
                    enemies2.push_back(e2);
                    lastEnemySpawnTime2 = now2;
                    e2.active = true;
                }

                // Move enemies down
                for (int i = 0; i < enemies2.size(); i++)
                {
                    auto now2 = steady_clock::now();

                    if (duration_cast<milliseconds>(now2 - enemies2[i].lastMoveTime).count() > 250)
                    {
                        enemies2[i].y++;
                        enemies2[i].lastMoveTime = now2;
                    }

                    if (enemies2[i].y > HEIGHT)
                        enemies2[i].active = false;

                    if (enemies2[i].active &&
                        player.x < enemies2[i].x + enemies2[i].width &&
                        player.x + player.width > enemies2[i].x &&
                        player.y < enemies2[i].y + enemies2[i].height &&
                        player.y + player.height > enemies2[i].y)
                    {
                        player.hp -= 1;
                        enemies2[i].active = false;
                        Beep(500, 100);
                    }

                    if (player.hp == 0)
                    {
                        gameOver = true;
                    }
                }
            }

            // Remove inactive enemies2
            for (int i = 0; i < enemies2.size();)
            {
                if (!enemies2[i].active)
                {
                    enemies2.erase(enemies2.begin() + i);
                }
                else
                {
                    i++;
                }
            }
            ///////////////////

            // Enemy3 spawn limiter: 1000ms
            auto now3 = steady_clock::now();
            if (hits > 40)
            {
                if (duration_cast<milliseconds>(now3 - lastEnemySpawnTime3).count() > 3000)
                {
                    Enemy3 e3;
                    e3.y = 1;
                    e3.x = rand() % (WIDTH - e3.width - 2) + 1;
                    e3.lastMoveTime = steady_clock::now();
                    enemies3.push_back(e3);
                    lastEnemySpawnTime3 = now3;
                    e3.active = true;
                }

                // Move enemies3 down
                for (int i = 0; i < enemies3.size(); i++)
                {
                    auto now3 = steady_clock::now();

                    if (duration_cast<milliseconds>(now3 - enemies3[i].lastMoveTime).count() > 250)
                    {
                        enemies3[i].y++;
                        enemies3[i].lastMoveTime = now2;
                    }

                    if (enemies3[i].y > HEIGHT)
                        enemies3[i].active = false;

                    if (enemies3[i].active &&
                        player.x < enemies3[i].x + enemies3[i].width &&
                        player.x + player.width > enemies3[i].x &&
                        player.y < enemies3[i].y + enemies3[i].height &&
                        player.y + player.height > enemies3[i].y)
                    {
                        player.hp -= 2;
                        enemies3[i].active = false;
                        Beep(500, 100);
                    }

                    if (player.hp == 0)
                    {
                        gameOver = true;
                    }
                }
            }

            // Remove inactive enemies3
            for (int i = 0; i < enemies3.size();)
            {
                if (!enemies3[i].active)
                {
                    enemies3.erase(enemies3.begin() + i);
                }
                else
                {
                    i++;
                }
            }
            ////////////////////

            // Did the bullet hit the enemy?
            for (int i = 0; i < bullets.size(); i++)
            {
                if (!bullets[i].active)
                    continue;

                for (int j = 0; j < enemies.size(); j++)
                {
                    if (!enemies[j].active)
                        continue;

                    if (bullets[i].x == enemies[j].x &&
                        (bullets[i].y == enemies[j].y || bullets[i].y == enemies[j].y + 1 || bullets[i].y == enemies[j].y - 1))
                    {
                        bullets[i].active = false;
                        enemies[j].active = false;
                        Beep(700, 20);
                        hits += 1;
                        break;
                    }
                }
            }

            // Did the bullet hit the enemy2?
            for (int i = 0; i < bullets.size(); i++)
            {
                if (!bullets[i].active)
                    continue;

                for (int j = 0; j < enemies2.size(); j++)
                {
                    if (!enemies2[j].active)
                        continue;

                    bool hitX = (bullets[i].x >= enemies2[j].x && bullets[i].x < enemies2[j].x + enemies2[j].width);
                    bool hitY = (bullets[i].y >= enemies2[j].y && bullets[i].y < enemies2[j].y + enemies2[j].height);

                    if (hitX && hitY)
                    {
                        bullets[i].active = false;

                        if (enemies2[j].health_enemy2 <= 1)
                        {
                            enemies2[j].active = false;
                            bullets[i].active = false;
                            Beep(700, 20);
                            hits += 2;
                        }
                        else
                        {
                            enemies2[j].health_enemy2--;
                            Beep(200, 20);
                        }

                        break;
                    }
                }
            }

            // Did the bullet hit the enemy3?
            for (int i = 0; i < bullets.size(); i++)
            {
                if (!bullets[i].active)
                    continue;

                for (int j = 0; j < enemies3.size(); j++)
                {
                    if (!enemies3[j].active)
                        continue;

                    bool hitX = (bullets[i].x >= enemies3[j].x && bullets[i].x < enemies3[j].x + enemies3[j].width);
                    bool hitY = (bullets[i].y >= enemies3[j].y && bullets[i].y < enemies3[j].y + enemies3[j].height);

                    if (hitX && hitY)
                    {
                        bullets[i].active = false;

                        if (enemies3[j].health_enemy3 <= 1)
                        {
                            enemies3[j].active = false;
                            bullets[i].active = false;
                            Beep(800, 20);
                            hits += 5;
                        }
                        else
                        {
                            enemies3[j].health_enemy3--;
                            Beep(200, 20);
                        }

                        break;
                    }
                }
            }

            // 3. Rendering
            // Reset cursor to top-left (0,0) to overwrite previous frame
            SetConsoleCursorPosition(hOut, {0, 0});

            string buffer = "\n\n"; // Double buffering technique (construct string, then print once)
            buffer += "  ";

            for (int k = 0; k < WIDTH + 2; k++)
                buffer += "#";
            const string MARGIN_LEFT = "  |";

            for (int y = 0; y < HEIGHT; y++)
            {
                if (y != 0)
                {
                    buffer += MARGIN_LEFT;
                }
                for (int x = 0; x < WIDTH; x++)
                {
                    // Check if current (x,y) contains a bullet
                    bool isBullet = false;
                    for (const auto &b : bullets)
                    {
                        if (b.x == x && b.y == y)
                        {
                            isBullet = true;
                            break;
                        }
                    }
                    // Check if current (x,y) contains a enemy2
                    bool isEnemy = false;
                    bool isEnemy2 = false;
                    bool isEnemy3 = false;

                    char enemy2Char = ' ';
                    char enemy3Char = ' ';

                    for (auto &e : enemies)
                    {
                        if (e.x == x && e.y == y)
                        {
                            isEnemy = true;
                            break;
                        }
                    }

                    for (auto &e2 : enemies2)
                    {
                        bool insideX = (x >= e2.x && x < e2.x + e2.width);
                        bool insideY = (y >= e2.y && y < e2.y + e2.height);

                        if (insideX && insideY)
                        {
                            isEnemy2 = true;
                            enemy2Char = e2.shape_enemy2[y - e2.y][x - e2.x];
                            break;
                        }
                    }

                    for (auto &e3 : enemies3)
                    {
                        bool insideX = (x >= e3.x && x < e3.x + e3.width);
                        bool insideY = (y >= e3.y && y < e3.y + e3.height);

                        if (insideX && insideY)
                        {
                            isEnemy3 = true;
                            enemy3Char = e3.shape_enemy3[y - e3.y][x - e3.x];
                            break;
                        }
                    }

                    // Check collision/overlap with player coordinates
                    bool insideX = (x >= player.x && x < player.x + player.width);
                    bool insideY = (y >= player.y && y < player.y + player.height);

                    if (insideX && insideY)
                        buffer += player.shape[y - player.y][x - player.x]; // Draw ship part
                    else if (isBullet)
                        buffer += "|";
                    else if (isEnemy)
                        buffer += "M";
                    else if (isEnemy2)
                        buffer += enemy2Char;
                    else if (isEnemy3)
                        buffer += enemy3Char;
                    else
                        buffer += " ";

                    if (x == WIDTH - 1)
                    {
                        buffer += "|";
                    }

                    if (y == 1 && x == WIDTH - 1)
                    {
                        buffer += "HEALTH:  " + to_string(player.hp);
                    }

                    if (y == 2 && x == WIDTH - 1)
                    {
                        buffer += "Hits:  " + to_string(hits);
                    }
                }
                buffer += "\n";
            }

            cout << buffer;
            cout << "\n \n \n";
            cout << "__________________________________________________" << endl;
            cout << "|Controls: A/D to Move, SPACE to Shoot, X to Quit|" << endl;

            // 4. Frame Pacing (FPS Cap)
            // Ensures consistent speed across different hardware
            auto endnow = steady_clock::now();
            auto elapsed = duration_cast<milliseconds>(endnow - lastFrameTime);

            if (elapsed < milliseconds(FRAME_DURATION_MS))
            {
                std::this_thread::sleep_for(milliseconds(FRAME_DURATION_MS) - elapsed);
            }
            lastFrameTime = steady_clock::now();
        }

        ShowGameOverScreen(hits);
        FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE)); // delete buffer for etAsyncKeyState

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