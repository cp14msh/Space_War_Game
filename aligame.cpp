#include <iostream>
#include <vector>
#include <conio.h>
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include <algorithm>
#include <fstream>
using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int WIDTH = 40;
const int HEIGHT = 20;
bool gameover = false;
bool paused = false;//یا استفاده از pointer
bool restartgame = false;
enum EnemyType {
    ENEMY_BASIC,
    ENEMY_Intermediate,
    ENEMY_SHOOTER
};
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
    EnemyType type;
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
bool checkCollisionPlayerEnemy(Player &player, Enemy &e) {//خورده یا نخورده (true/false)
    return (
        player.x < e.x + e.width &&
        player.x + player.width > e.x &&
        player.y < e.y + e.height &&
        player.y + player.height > e.y
    );
}
bool checkCollisionBulletsEnemy(Bullet &bullets, Enemy &e) {//خورده یا نخورده (true/false)
    return (
        bullets.x < e.x + e.width &&
        bullets.x + bullets.width > e.x &&
        bullets.y < e.y + e.height &&
        bullets.y + bullets.height > e.y
    );
}
void clearScreen(char screen[HEIGHT][WIDTH]) {//پاک کردن buffer در ابتدای هر حلقه:
    for (int y = 0; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++)
            screen[y][x] = ' ';
}
void drawPlayer(char screen[HEIGHT][WIDTH], Player &player) {//تغییر جدید
    int x = (int)(player.x / (SCREEN_WIDTH / WIDTH));
    int y = (int)(player.y / (SCREEN_HEIGHT / HEIGHT));
    if (y-1 >= 0) {
            screen[y-1][x] = '^';
    }

    if (x-1 >= 0 && x+1 < WIDTH) {
        screen[y][x-1] = '/';
        screen[y][x]   = '|';
        screen[y][x+1] = '\\';
    }

    if (y+1 < HEIGHT && x-2 >= 0 && x+2 < WIDTH) {
        screen[y+1][x-2] = '<';
        screen[y+1][x-1] = '_';
        screen[y+1][x]   = '|';
        screen[y+1][x+1] = '_';
        screen[y+1][x+2] = '>';
    }
}
void drawBullets(char screen[HEIGHT][WIDTH], vector<Bullet> &bullets) {//تغییر جدید : مقیاس پدیری موقعیت ها با صفحه نمایش
    for (auto &b : bullets) {
        int x = (int)(b.x / (SCREEN_WIDTH / WIDTH));
        int y = (int)(b.y / (SCREEN_HEIGHT / HEIGHT));

        if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT)
            screen[y][x] = '|';
    }
}
void drawEnemies(char screen[HEIGHT][WIDTH], vector<Enemy> &enemies) {//تغییر جدید 
    for (auto &e : enemies) {
        int x = (int)(e.x / (SCREEN_WIDTH / WIDTH));
        int y = (int)(e.y / (SCREEN_HEIGHT / HEIGHT));

        const char* shape;
        int shapeWidth;
        switch (e.type) {
            case ENEMY_BASIC:
                shape = "<^>";
                shapeWidth = 3;
                break;

            case ENEMY_Intermediate:
                shape = "/X\\";
                shapeWidth = 3;
                break;

            case ENEMY_SHOOTER:
                shape = "~v~";
                shapeWidth = 3;
                break;

            default:
                continue;
        }
        int startX = x - shapeWidth / 2;
        for (int i = 0; i < shapeWidth; i++) {
            int drawX = startX + i;

            if (drawX >= 0 && drawX < WIDTH && y >= 0 && y < HEIGHT) {
                screen[y][drawX] = shape[i];
            }
        }
    } 
}
void moveCursorToTop() {
    COORD coord = {0, 0};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
void hideCursor() {
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(console, &info);
}
void render(char screen[HEIGHT][WIDTH], int score) {
    moveCursorToTop();   // 👈 کلیدی‌ترین خط

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            cout << screen[y][x];
        }
        cout << '\n';
    }

    cout << "Score: " << score << "      \n";
}
int showMenu() {
    int choice;
    do {
        system("cls");
        cout << "SPACE WAR GAME\n";
        cout << "==================\n";
        cout << "1. Start Game\n";
        cout << "2. Instructions\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice < 1 || choice > 3) {
            cout << "Invalid choice! Try again.\n";
            Sleep(1000); // مکث کوتاه قبل از تکرار
        }

    } while (choice < 1 || choice > 3);

    return choice;
}
// تابع نمایش دستورالعمل‌ها
void showInstructions() {
    system("cls");
    cout << "INSTRUCTIONS:\n";
    cout << "- Use W A S D to move your spaceship\n";
    cout << "- Press SPACE to shoot bullets\n";
    cout << "- Avoid enemies and shoot them\n";
    cout << "\nPress any key to start the game...";
    _getch();
}
// تابع اصلی بازی (حلقه while game loop اصلی)
void startGame() {system("cls");
    cout << "Game is starting...\n";
    cout << "Press any key to return to menu...";
    _getch();
}
int loadHighScore() {
    ifstream file("highscore.txt");
    int highScore = 0;
    if (file.is_open()) {
        file >> highScore;  // خواندن مقدار قبلی
        file.close();
    }
    return highScore;
}
void saveHighScore(int score) {
    ofstream file("highscore.txt");
    if (file.is_open()) {
        file << score;
        file.close();
    }
}

int main() {
    hideCursor();
    Player player;
        player.x = SCREEN_WIDTH / 2;
        player.y = SCREEN_HEIGHT - 50;
        player.height = 20;
        player.width = 40;
        player.speed = 40;
    char screen[HEIGHT][WIDTH];
    vector<Bullet> bullets; // وکتوری از نوع struct Bullet
    vector<Enemy> enemies;
    srand(time(0));
    int enemySpawnCounter = 0;
    int score = 0;
    int framenum = 0;
    int enemyBaseSpeed = 3 + framenum / 1000.0;
    int enemyinterval = max(10, 50 - framenum / 1000); // فاصله ظاهر شدن دشمن‌ها کاهش پیدا می‌کند
    bool startGamestatus = false;
    int highscore = loadHighScore();

     //  وقتی true شد، وارد حلقه بازی می‌شویم
    while (true) {//نشان می‌دهد که برنامه در حالت بازی است یا در حالت منو.
        if (!startGamestatus) { // فقط وقتی بازی شروع نشده، منو نمایش داده می‌شود
            int Choice = showMenu();

            if (Choice == 1) {
                startGamestatus = true; // حلقه بازی اجرا شود
            } 
            else if (Choice == 2) {
                showInstructions();
                startGamestatus = true; // بعد از نمایش دستورالعمل، حلقه بازی اجرا شود
            } 
            else if (Choice == 3) {
                cout << "Exiting game...\n";
                Sleep(500);
                break; // خروج از برنامه
            }
        }
    while (!gameover) {
        enemySpawnCounter++;
        framenum++;

        cout << "High Score: " << highscore << endl;

        if (score > 10) {
            enemyBaseSpeed = 4;
        }
        if (score > 20) {
            enemyBaseSpeed = 5;
        }
        if (enemySpawnCounter > enemyinterval) {
            Enemy e;
            e.x = rand() % SCREEN_WIDTH;
            e.y = 0;
            e.width = 40;
            e.height = 20;
            e.speed = enemyBaseSpeed;
            enemies.push_back(e);
            enemySpawnCounter = 0;
        }
        if (player.x < 0) {// محدودیت حرکت سفینه
        player.x = 0;
        }
        if (player.x + player.width > SCREEN_WIDTH) {
            player.x = SCREEN_WIDTH - player.width;
        }
        handleInput(player, bullets);
                if (paused) {
            Sleep(30);
            continue; // ⏸️  برمی گرده اول حلقه همه‌چی فریز می‌شه
        }
        if (restartgame) {
            // reset everything
            bullets.clear();
            enemies.clear();

            player.x = SCREEN_WIDTH / 2;
            player.y = SCREEN_HEIGHT - 50;

            score = 0;
            enemySpawnCounter = 0;
            gameover = false;
            paused = false;
            restartgame = false;

            continue; // برگرد اول حلقه
        }
        for (auto &e : enemies)
        {
            e.y += e.speed;
           if (checkCollisionPlayerEnemy(player, e)) {
                gameover = true;
                break;
            }
        }
        if (gameover) {
            render(screen, score); // آخرین وضعیت بازی
            Sleep(500); 
            break;
        }
        for (auto &b : bullets)
        {
            b.y -= b.speed;
        }
        for (int i = 0; i < bullets.size(); i++) {
            for (int j = 0; j < enemies.size(); j++) {

                if (checkCollisionBulletsEnemy(bullets[i], enemies[j])) {
                    // حذف تیر
                    bullets.erase(bullets.begin() + i);
                    i--;
                    // حذف دشمن
                    enemies.erase(enemies.begin() + j);
                    j--;
                    score++;
                    break;
                }
            }
        }
        // حذف تیرهای خارج صفحه
        bullets.erase(
            std::remove_if(bullets.begin(), bullets.end(),[](Bullet &b){ return b.y < 0; }),
            bullets.end()
        );

        // حذف دشمنان خارج صفحه
        enemies.erase(
            std::remove_if(enemies.begin(), enemies.end(),[](Enemy &e){ return e.y > SCREEN_HEIGHT; }),
            enemies.end()
        );
        //آماده‌سازی Back Buffer
        clearScreen(screen);
        drawPlayer(screen, player);
        drawBullets(screen, bullets);
        drawEnemies(screen, enemies);
        //system("cls");               جایگزینی با کروسر
        render(screen, score);
        Sleep(30);
    }
        // حذف تیرهای خارج صفحه
        bullets.erase(
            std::remove_if(bullets.begin(), bullets.end(),[](Bullet &b){ return b.y < 0; }),
            bullets.end()
        );

        // حذف دشمنان خارج صفحه
        enemies.erase(
            std::remove_if(enemies.begin(), enemies.end(),[](Enemy &e){ return e.y > SCREEN_HEIGHT; }),
            enemies.end()
        );
    system("cls");
    cout << "GAME OVER ☠️" << endl;
    cout << "Final Score: " << score << endl;
    return 0;
    }
}