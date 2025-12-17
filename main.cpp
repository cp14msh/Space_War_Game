#include <iostream>
#include <vector>
#include <string>
#include <conio.h>
#include <windows.h>
#include <chrono>
#include <thread>
using namespace std::chrono;

using namespace std;

const int WIDTH = 40;  // عرض صفحه
const int HEIGHT = 20; // طول صفحه

class Player
{
public:
    int x, y;
    int width, height;    // عرض و ارتفاع سفینه
    vector<string> shape; // ساخت سفینه

    Player(int startX, int startY) // ساخت پیلر که ایکس و وای میگره
    {
        x = startX;
        y = startY;

        // تعریف شکل سفینه
        shape = {
            "  ^  ",
            " /|\\ ",
            " |_| ",
            " / \\ "};

        height = shape.size();     // ارتفاع سفینه
        width = shape[0].length(); // عرض سفینه
    }

    void moveLeft()
    {
        if (x > 0) // برای اینکه نذاره از بازی بره بیرون
            x--;   // دیوار چپ
    }

    void moveRight()
    {
        if (x + width < WIDTH) // برای اینکه نذاره از بازی بره بیرون
            x++;
    }
};

int main()
{
    Player player(WIDTH / 2 - 2, HEIGHT - 5); // سفینه در وسط بازی و پایین ترین نقطه اسپان میشه

    bool gameOver = false; // وقتی بخوایم بازه این رو ترو می کنیم

    while (!gameOver)
    {
        // #include <conio.h>
        // برای این کتابخونه است که منتظر اینتر نمی مونه
        if (_kbhit())
        {
            char key = _getch();          // حرفی که می زنیم رو در کی می ریزیم
            if (key == 'a' || key == 'A') // اگر آ بود میره جپ
                player.moveLeft();
            if (key == 'd' || key == 'D') // اگر دی بود میره راست
                player.moveRight();
            if (key == 'x' || key == 'X') // متغیری تعریف کردیم تا با زدنش بتونه از بازی خارج بشه
                gameOver = true;
        }

        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE); // ریموت کنترلِ پنجره خروجی (همین صفحه سیاه کنسول) را بده به من تا بتوانم کنترلش کنم.
        COORD ptr = {0, 0};                            // COORD = Coordinate and set x and y to 0,0
        SetConsoleCursorPosition(hOut, ptr);           // با استفاده از آن ریموت کنترل نشانگر چشمک‌زن تایپ را ببر بگذار دقیقاً در نقطه شروع
        // وقتی نشانگر می‌رود اول صفحه، دستورات جدید، روی نوشته‌های قبلی بازنویسی می‌شوند.

        // کشیدن صفحه با دوتا حلقه فور
        for (int y = 0; y < HEIGHT; y++)
        {
            for (int x = 0; x < WIDTH; x++)
            {

                // فرمول ریاضی: آیا مختصات فعلی (x,y) داخل مستطیل سفینه هست؟
                bool insideX = (x >= player.x && x < player.x + player.width);  // ایکس باید از جایی شروع بشه که عرض سفینه جا بشه
                bool insideY = (y >= player.y && y < player.y + player.height); // ایکس باید از جایی شروع بشه که ارتفاع سفینه جا بشه

                if (insideX && insideY)
                {
                    // محاسبه میکنیم کجای سفینه رو باید بکشیم
                    int shipRow = y - player.y;
                    int shipCol = x - player.x;
                    cout << player.shape[shipRow][shipCol];
                }
                else
                {
                    cout << "."; // فضای خالی
                }
            }
            cout << endl;
        }

        // راهنما برای کاربر
        cout << "Control: A (Left), D (Right), X (Exit)" << endl;

        static auto last_frame = steady_clock::now(); // Persistent across loops
        const milliseconds frame_duration(33);        // ~30 FPS target

        auto now = steady_clock::now();
        auto elapsed = duration_cast<milliseconds>(now - last_frame);

        if (elapsed < frame_duration)
        {
            // Sleep ONLY the remaining time
            std::this_thread::sleep_for(frame_duration - elapsed);
        }
        last_frame = now; // Update for next frame
    }

    return 0;
}