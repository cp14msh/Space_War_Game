#include <iostream>
#include <vector>
#include <string>
#include <conio.h>
#include <windows.h>

using namespace std;

const int WIDTH = 100; // عرض صفحه رو بیشتر کردم که سفینه جا بشه
const int HEIGHT = 40;

class Player
{
public:
    int x, y;
    int width, height;    // عرض و ارتفاع سفینه
    vector<string> shape; // شکل سفینه

    Player(int startX, int startY)
    {
        x = startX;
        y = startY;

        // تعریف شکل سفینه
        shape = {
            "  ^  ",
            " /|\\ ",
            " |_| ",
            " / \\ "};

        height = shape.size();     // ارتفاع میشه تعداد خط‌ها (4)
        width = shape[0].length(); // عرض میشه طول اولین خط (5)
    }

    void moveLeft()
    {
        if (x > 0)
            x--; // دیوار چپ
    }

    void moveRight()
    {
        // دیوار راست (دقت کن: x + width نباید از کادر بزنه بیرون)
        if (x + width < WIDTH)
            x++;
    }
};

int main()
{
    // سفینه رو می‌ذاریم وسط و پایین (منهای ارتفاعش تا نره بیرون)
    Player player(WIDTH / 2 - 2, HEIGHT - 5);

    bool gameOver = false;

    while (!gameOver)
    {
        // --- 1. INPUT ---
        if (_kbhit())
        {
            char key = _getch();
            if (key == 'a')
                player.moveLeft();
            if (key == 'd')
                player.moveRight();
            if (key == 'x')
                gameOver = true;
        }

        // --- 2. LOGIC ---
        // فعلا خالی

        // --- 3. RENDER ---
        // نشانگر ماوس رو ببر اول صفحه (این بهتر از cls هست چون پرپر نمیکنه)
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        COORD ptr = {0, 0};
        SetConsoleCursorPosition(hOut, ptr);

        // حلقه کشیدن کل صفحه
        for (int y = 0; y < HEIGHT; y++)
        {
            for (int x = 0; x < WIDTH; x++)
            {

                // فرمول ریاضی: آیا مختصات فعلی (x,y) داخل مستطیل سفینه هست؟
                bool insideX = (x >= player.x && x < player.x + player.width);
                bool insideY = (y >= player.y && y < player.y + player.height);

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

        Sleep(40); // سرعت بازی
    }

    return 0;
}