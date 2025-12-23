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

int main()
{
    // --- Initialization ---
    Player player(WIDTH / 2 - 2, HEIGHT - 5);
    vector<Bullet> bullets;
    vector<Enemy> enemies;
    bool gameOver = false;
    int hits = 0;

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
    srand(time(0));

    // --- Main Game Loop ---
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

        // 3. Rendering
        // Reset cursor to top-left (0,0) to overwrite previous frame
        SetConsoleCursorPosition(hOut, {0, 0});

        string buffer = ""; // Double buffering technique (construct string, then print once)

        for (int y = 0; y < HEIGHT; y++)
        {
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
                // Check if current (x,y) contains a enemy
                bool isEnemy = false;
                for (auto &e : enemies)
                {
                    if (e.x == x && e.y == y)
                    {
                        isEnemy = true;
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
                else
                    buffer += " ";

                if (x == WIDTH - 1)
                {
                    buffer += "|";
                }

                if (y == 0 && x == WIDTH - 1)
                {
                    buffer += "HEALTH:  " + to_string(player.hp);
                }

                if (y == 1 && x == WIDTH - 1)
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

    return 0;
}