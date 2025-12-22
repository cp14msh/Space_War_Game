#include <iostream>
#include <vector>
#include <string>
#include <conio.h>
#include <windows.h>
#include <chrono>
#include <thread>

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

class Player
{
public:
    int x, y;
    int width, height;
    vector<string> shape;

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
    bool gameOver = false;

    // Console Setup: Hide cursor to avoid flickering artifacts
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hOut, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(hOut, &cursorInfo);

    // Timing variables
    auto lastShotTime = steady_clock::now();
    auto lastFrameTime = steady_clock::now();

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

                // Check collision/overlap with player coordinates
                bool insideX = (x >= player.x && x < player.x + player.width);
                bool insideY = (y >= player.y && y < player.y + player.height);

                if (insideX && insideY)
                    buffer += player.shape[y - player.y][x - player.x]; // Draw ship part
                else if (isBullet)
                    buffer += "|";
                else
                    buffer += " ";
            }
            buffer += "\n";
        }

        cout << buffer;
        cout << "Controls: A/D to Move, SPACE to Shoot, X to Quit" << endl;

        // 4. Frame Pacing (FPS Cap)
        // Ensures consistent speed across different hardware
        auto now = steady_clock::now();
        auto elapsed = duration_cast<milliseconds>(now - lastFrameTime);

        if (elapsed < milliseconds(FRAME_DURATION_MS))
        {
            std::this_thread::sleep_for(milliseconds(FRAME_DURATION_MS) - elapsed);
        }
        lastFrameTime = steady_clock::now();
    }

    return 0;
}