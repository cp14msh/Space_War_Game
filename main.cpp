#include <iostream>
#include <vector> // making array without giving size
#include <string>
#include <conio.h>   // time management
#include <windows.h> // console management
#include <chrono>    // for move to grab the buttons
#include <thread>

using namespace std;
using namespace std::chrono;

// Game board dimensions
const int WIDTH = 60;
const int HEIGHT = 40;

// making bullet struct
struct Bullet
{
    int x, y;
    bool active; // until the bullet is in game board is active
};

class Player
{
public:
    int x, y;
    int width, height;
    vector<string> shape; // difine vectoer for spaceship shape

    Player(int startX, int startY)
    {
        x = startX;
        y = startY;
        shape = {
            "  ^  ",
            " /|\\ ",
            " |_| ",
            " / \\ "};
        height = shape.size();     // returns elements number
        width = shape[0].length(); // returns the string length
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

int main()
{
    Player player(WIDTH / 2 - 2, HEIGHT - 5); // Spaceship spawn location
    vector<Bullet> bullets;                   // making vector for bullet
    bool gameOver = false;

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE); // Get control of console output
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hOut, &cursorInfo);
    cursorInfo.bVisible = false; // hide the cursor
    SetConsoleCursorInfo(hOut, &cursorInfo);
    /*
    GetConsoleCursorInfo(...) → Windows API: “Tell me the current cursor config”
    hOut → Which console? : the one we got in line 1
    &cursorInfo → Address of cursorInfo → “store the data here”

    ✅ Before this line: cursorInfo = garbage
    ✅ After this line: cursorInfo = actual current settings (e.g., bVisible = TRUE, dwSize = 25)

    1. Get remote: hOut = GetStdHandle(...)
    2. Prepare config sheet: CONSOLE_CURSOR_INFO cursorInfo;
    3. Load current settings: GetConsoleCursorInfo(hOut, &cursorInfo)
        → cursorInfo = { dwSize:25, bVisible:TRUE }
    4. Modify: cursorInfo.bVisible = false
        → cursorInfo = { dwSize:25, bVisible:FALSE }
    5. Apply: SetConsoleCursorInfo(hOut, &cursorInfo)
        → Console updates: cursor hidden ✅
    */

    auto lastShotTime = steady_clock::now(); // for don't can shoot too much in second

    auto last_frame = steady_clock::now(); // for loading page

    while (!gameOver)
    {
        string buffer = "";

        if (GetAsyncKeyState('A') & 0x8000)
            player.moveLeft();
        if (GetAsyncKeyState('D') & 0x8000)
            player.moveRight();
        if (GetAsyncKeyState('X') & 0x8000)
            gameOver = true;

        if (GetAsyncKeyState(VK_SPACE) & 0x8000)
        /*
         Why Not Just if (GetAsyncKeyState('A'))?
        Because GetAsyncKeyState can return 0x0001 (just pressed, but now released),
        which is non-zero → true, even if the key is not currently held.
        */
        {
            auto now = steady_clock::now();
            if (duration_cast<milliseconds>(now - lastShotTime).count() > 200)
            /*
            now - lastShotTime :  how much time passed between them.
            duration_cast<milliseconds> : Converts to milliseconds.
            .count() : Extracts the numeric value from the duration.
            */
            {
                Bullet b;             // define sturct name
                b.x = player.x + 2;   // setting bullet spawn location
                b.y = player.y - 1;   // setting bullet spawn location
                b.active = true;      // bullet is active
                bullets.push_back(b); // add bullet to vector
                lastShotTime = now;
            }
        }

        // bullet moving
        for (int i = 0; i < bullets.size(); i++)
        {
            bullets[i].y--;
            if (bullets[i].y < 0)
                bullets[i].active = false; // Disable the which bullet in out if board game
        }

        // removing bullet form vectore
        for (int i = 0; i < bullets.size();)
        {
            if (!bullets[i].active)
                bullets.erase(bullets.begin() + i);
            /*
            bullets.begin() → iterator to the first element (index 0)
            bullets.begin() + i → iterator to the element at index i
            */
            else
                i++;
        }
        COORD ptr = {0, 0}; // the place where printing in console
        SetConsoleCursorPosition(hOut, ptr);

        for (int y = 0; y < HEIGHT; y++)
        {
            for (int x = 0; x < WIDTH; x++)
            {
                bool isBullet = false;
                for (auto &b : bullets) //// b is a reference to bullets[0], then bullets[1], ...
                {
                    if (b.x == x && b.y == y)
                    {
                        isBullet = true;
                        break;
                    }
                }

                bool insideX = (x >= player.x && x < player.x + player.width);  // Calculations for whether the ship is out of the game
                bool insideY = (y >= player.y && y < player.y + player.height); // Calculations for whether the ship is out of the game
                bool isPlayer = insideX && insideY;

                if (isPlayer)
                    buffer += player.shape[y - player.y][x - player.x];
                else if (isBullet)
                    buffer += "|";
                else
                    buffer += " ";
            }
            buffer += "\n";
        }

        cout << buffer; // printing game board
        cout << "Controls: A/D to Move, SPACE to Shoot" << endl;

        // Target frame duration for 30 FPS
        // Using fixed time step ensures consistent game speed across different machines
        const milliseconds frame_duration(33);

        // Get current time.
        auto now = steady_clock::now();

        // Compute how much time has passed since the last frame update
        auto elapsed = duration_cast<milliseconds>(now - last_frame);

        if (elapsed < frame_duration)
        {
            // Sleep precisely to hit target frame time.
            std::this_thread::sleep_for(frame_duration - elapsed);
        }

        last_frame = steady_clock::now();
    }
    return 0;
}