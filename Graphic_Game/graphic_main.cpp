// g++ graphic_main.cpp resource.o -I C:\SFML-3.0.2\include -L C:\SFML-3.0.2\lib -o graphic_main.exe -lsfml-audio -lsfml-graphics -lsfml-window -lsfml-system -mwindows

#include <iostream>
#include <optional>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <map>

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>

using namespace std;
using namespace sf;

struct Enemy2
{
    Sprite sprite;
    int health;
    Enemy2(const Texture &texture) : sprite(texture), health(2) {}
};

struct Enemy3
{
    Sprite sprite;
    int health;
    Enemy3(const Texture &texture) : sprite(texture), health(4) {}
};

struct Enemy4
{
    Sprite sprite;
    int health;
    Enemy4(const Texture &texture) : sprite(texture), health(8) {}
};

struct PowerUp
{
    Sprite sprite;
    PowerUp(const Texture &texture) : sprite(texture) {}
};

struct PowerDown
{
    Sprite sprite;
    PowerDown(const Texture &texture) : sprite(texture) {}
};

map<string, Texture> textures;
void loadimage(const string &name, const string &location)
{
    Texture texture;
    if (!texture.loadFromFile(location))
    {
        cerr << "Error: Could not load " << location << endl;
    }
    textures[name] = texture;
}

map<string, SoundBuffer> buffers;
void loadsound(const string &name, const string &location)
{
    SoundBuffer buffer;
    if (!buffer.loadFromFile(location))
    {
        cerr << "Error: Could not load " << location << endl;
        return;
    }
    buffers[name] = buffer;
}

int main()
{

    srand(static_cast<unsigned>(time(NULL)));

    // make window for game
    RenderWindow window(VideoMode({800, 600}), "Space War Game", Style::Titlebar | Style::Close | Style::Resize);
    window.setMouseCursorVisible(false);
    window.setFramerateLimit(60);

    // -------------------------------------------------
    // Player
    // -------------------------------------------------
    loadimage("spaceship", "spaceship.png");

    // Paste the image into the sprite
    Sprite player(textures["spaceship"]);
    player.setScale({1.3f, 1.3f});

    // set position for player
    player.setPosition({350.f, 500.f});

    // player_hp
    int hp = 3;
    loadimage("hpTexture", "heart.png");
    Sprite player_hp(textures["hpTexture"]);
    player_hp.setScale({1.1f, 1.1f});

    // set position for player_hp
    player_hp.setPosition({5.f, 5.f});

    // player_score
    int score = 0;
    loadimage("scoreTexture", "score.png");
    Sprite player_score(textures["scoreTexture"]);
    player_score.setScale({0.15f, 0.15f});

    // set position for player_hp
    player_score.setPosition({12.f, 45.f});

    // player_weapon_level
    int weapon_level = 1;
    loadimage("weaponlevelTexture", "weapon_level.png");
    Sprite player_weapon_level(textures["weaponlevelTexture"]);
    player_weapon_level.setScale({0.18f, 0.18f});

    // set position for player_weapon_level
    player_weapon_level.setPosition({10.f, 77.f});

    // -------------------------------------------------
    // Enemy1
    // -------------------------------------------------
    loadimage("EnemyTexture", "spritesheet.png");
    Sprite enemy(textures["EnemyTexture"]);
    vector<Sprite> enemies;
    Clock enemySpawnTimer;

    // -------------------------------------------------
    // Enemy2
    // -------------------------------------------------
    loadimage("Enemy2Texture", "enemy2.png");
    Sprite enemy2(textures["Enemy2Texture"]);
    Clock enemy2SpawnTimer;
    vector<Enemy2> enemies2;

    // -------------------------------------------------
    // Enemy3
    // -------------------------------------------------
    loadimage("Enemy3Texture", "enemy3.png");
    Sprite enemy3(textures["Enemy3Texture"]);
    vector<Enemy3> enemies3;
    Clock enemy3SpawnTimer;

    // -------------------------------------------------
    // Enemy4
    // -------------------------------------------------
    loadimage("Enemy4Texture", "enemy4.png");
    Sprite enemy4(textures["Enemy4Texture"]);
    vector<Enemy4> enemies4;
    Clock enemy4SpawnTimer;

    // -------------------------------------------------
    // PowerUp
    // -------------------------------------------------
    loadimage("powerUpTexture", "obituary.png");
    vector<PowerUp> powerUps;
    Clock powerUpSpawnTimer;
    const float POWERUP_SPAWN_RATE = 20.0f;

    // -------------------------------------------------
    // PowerDown
    // -------------------------------------------------
    loadimage("powerDownTexture", "power_low.png");
    vector<PowerDown> powerDowns;
    Clock powerDownSpawnTimer;
    const float POWERDOWN_SPAWN_RATE = 15.0f;

    // -------------------------------------------------
    // Bullets
    // -------------------------------------------------
    vector<RectangleShape> bullets;
    Clock shootTimer;

    loadimage("bullets2Texture", "bullets_2.png");
    Sprite bullet_2(textures["bullets2Texture"]);
    vector<Sprite> bullets_2;
    Clock bullets2_shootTimer;

    loadimage("Enemy3_BulletsTexture", "enemy3_bullets.png");
    Sprite enemy3_bullet(textures["Enemy3_BulletsTexture"]);
    vector<Sprite> enemy3_bullets;
    Clock enemy3_shootTimer;

    loadimage("Enemy4_BulletsTexture", "enemy4_bullets.png");
    Sprite enemy4_bullet(textures["Enemy4_BulletsTexture"]);
    vector<Sprite> enemy4_bullets;
    Clock enemy4_shootTimer;

    // -------------------------------------------------
    // Text
    // -------------------------------------------------
    Font font;
    if (!font.openFromFile("font.ttf"))
    {
        cerr << "Error: Could not load font.ttf" << endl;
        return -1;
    }

    Text scoreText(font);
    scoreText.setCharacterSize(15);
    scoreText.setFillColor(Color::White);
    scoreText.setPosition({41.f, 46.f});
    scoreText.setString(to_string(score));

    Text hpText(font);
    hpText.setCharacterSize(15);
    hpText.setFillColor(Color::White);
    hpText.setStyle(Text::Style::Bold);
    hpText.setPosition({40.f, 14.f});
    hpText.setString(to_string(hp));

    Text weapon_levelText(font);
    weapon_levelText.setCharacterSize(15);
    weapon_levelText.setFillColor(Color::White);
    weapon_levelText.setPosition({42.f, 78.5f});
    weapon_levelText.setString(to_string(weapon_level));

    // -------------------------------------------------
    // Sound Effects
    // -------------------------------------------------
    loadsound("explosionBuffer", "explosion.wav");
    Sound explosionSound(buffers["explosionBuffer"]);

    loadsound("shootingBuffer", "shooting.wav");
    Sound shootingSound(buffers["shootingBuffer"]);

    loadsound("hit_enemy1Buffer", "hit01.wav");
    Sound hit_enemy1Sound(buffers["hit_enemy1Buffer"]);

    loadsound("game_over_Buffer", "Game_Over.wav");
    Sound game_over_sound(buffers["game_over_Buffer"]);

    loadsound("YES_Buffer", "YES.flac");
    Sound YES_sound(buffers["YES_Buffer"]);

    loadsound("PowerDownS", "poweDown.mp3");
    Sound PowerDown_sound(buffers["PowerDownS"]);

    loadsound("PowerUP_s", "powerUp.wav");
    Sound PowerUp_sound(buffers["PowerUP_s"]);

    /// -------------------------------------------------
    // POINTER
    // -------------------------------------------------
    loadimage("pointerTexture", "pointer.png");
    Sprite pointer(textures["pointerTexture"]);
    pointer.setScale({0.1f, 0.1f});

    /// -------------------------------------------------
    // GAME_Board
    // -------------------------------------------------
    loadimage("boardTexture", "game_over.jpg");
    Sprite board(textures["boardTexture"]);
    board.setScale({0.39f, 0.42f});
    board.setPosition({0.f, 0.f});

    loadimage("board2Texture", "game_board.png");
    Sprite board2(textures["board2Texture"]);
    board2.setScale({0.39f, 0.42f});
    board2.setPosition({0.f, 0.f});

    /// -------------------------------------------------
    // GAME LOOP
    // -------------------------------------------------
    bool isGameOver = false;
    while (window.isOpen())
    {

        while (const optional event = window.pollEvent())
        {
            // As long as the game is not closed
            if (event->is<Event::Closed>())
            {
                window.close();
            }
        }

        Vector2i mousePos = Mouse::getPosition(window);
        pointer.setPosition({(float)mousePos.x, (float)mousePos.y});

        if (!isGameOver)
        {
            // Checking for keyboard key presses
            if (Keyboard::isKeyPressed(Keyboard::Key::D))
            {
                player.move({5.f, 0.f});
            }

            if (Keyboard::isKeyPressed(Keyboard::Key::A))
            {
                player.move({-5.f, 0.f});
            }

            if (Keyboard::isKeyPressed(Keyboard::Key::W))
            {
                player.move({0.f, -5.f});
            }

            if (Keyboard::isKeyPressed(Keyboard::Key::S))
            {
                player.move({0.f, 5.f});
            }

            // vector2f give two variable
            Vector2f pos = player.getPosition();

            // Get the player's frame (the hypothetical rectangle around the image)
            FloatRect bounds = player.getGlobalBounds();
            float playerWidth = bounds.size.x;
            float playerHeight = bounds.size.y;

            // Checking to make sure the player doesn't exit the game screen
            if (pos.x < 0.f)
                player.setPosition({0.f, pos.y});

            if (pos.y < 0.f)
                player.setPosition({pos.x, 0.f});

            if (pos.x + playerWidth > 800.f)
                player.setPosition({800.f - playerWidth, pos.y});

            if (pos.y + playerHeight > 600.f)
                player.setPosition({pos.x, 600.f - playerHeight});

            // -------------------------------------------------
            // Shooting Logic
            // -------------------------------------------------
            if (Keyboard::isKeyPressed(Keyboard::Key::Space) && shootTimer.getElapsedTime().asSeconds() > 0.4f && weapon_level == 1)
            {
                shootingSound.play();

                RectangleShape bullet({3.f, 20.f});
                bullet.setFillColor(Color::Yellow);

                float bulletX = player.getPosition().x + (bounds.size.x / 2) - 2.5f;
                float bulletY = player.getPosition().y;
                bullet.setPosition({bulletX, bulletY});

                bullets.push_back(bullet);

                shootTimer.restart();
            }

            if (Keyboard::isKeyPressed(Keyboard::Key::Space) && bullets2_shootTimer.getElapsedTime().asSeconds() > 0.4f && weapon_level == 2)
            {
                shootingSound.play();

                Sprite bullet2(textures["bullets2Texture"]);

                float bullet2X = player.getPosition().x + (bounds.size.x / 2) - 5.0f;
                float bullet2Y = player.getPosition().y;
                bullet2.setPosition({bullet2X, bullet2Y});

                bullets_2.push_back(bullet2);

                bullets2_shootTimer.restart();
            }

            for (size_t i = 0; i < bullets.size(); i++)
            {
                bullets[i].move({0.f, -10.f});

                // remove bullets from vector
                if (bullets[i].getPosition().y < -20.f)
                {
                    bullets.erase(bullets.begin() + i);
                    i--;
                }
            }

            for (size_t i = 0; i < bullets_2.size(); i++)
            {
                bullets_2[i].move({0.f, -10.f});

                // remove bullets from vector
                if (bullets_2[i].getPosition().y < -20.f)
                {
                    bullets_2.erase(bullets_2.begin() + i);
                    i--;
                }
            }

            // -------------------------------------------------
            // Enemy1 logic
            // -------------------------------------------------

            if (enemySpawnTimer.getElapsedTime().asSeconds() > 2.0f)
            {
                Sprite enemy(textures["EnemyTexture"]);
                enemy.setScale({1.0f, 1.0f});

                float enemyWidth = enemy.getGlobalBounds().size.x;
                float randomX = static_cast<float>(rand() % static_cast<int>(800 - enemyWidth));

                enemy.setPosition({randomX, -50.f});

                enemies.push_back(enemy);

                enemySpawnTimer.restart();
            }

            // Enemy movement
            for (size_t i = 0; i < enemies.size(); i++)
            {
                enemies[i].move({0.f, 2.f});

                // Clearing enemies that are off the screen
                if (enemies[i].getPosition().y > 600.f)
                {
                    enemies.erase(enemies.begin() + i);
                    i--;
                }
            }

            // Checking to see if the enemy has been hit by a bullet
            if (weapon_level == 1)
            {
                for (size_t i = 0; i < bullets.size(); i++)
                {
                    for (size_t j = 0; j < enemies.size(); j++)
                    {

                        if (bullets[i].getGlobalBounds().findIntersection(enemies[j].getGlobalBounds()))
                        {
                            hit_enemy1Sound.play();
                            score += 1;
                            scoreText.setString(to_string(score));

                            enemies.erase(enemies.begin() + j);

                            bullets.erase(bullets.begin() + i);

                            i--;

                            break;
                        }
                    }
                }
            }

            if (weapon_level == 2)
            {
                for (size_t i = 0; i < bullets_2.size(); i++)
                {
                    for (size_t j = 0; j < enemies.size(); j++)
                    {

                        if (bullets_2[i].getGlobalBounds().findIntersection(enemies[j].getGlobalBounds()))
                        {
                            hit_enemy1Sound.play();
                            score += 1;
                            scoreText.setString(to_string(score));

                            enemies.erase(enemies.begin() + j);

                            bullets_2.erase(bullets_2.begin() + i);

                            i--;

                            break;
                        }
                    }
                }
            }

            // Hitting the enemy with a ship

            for (size_t i = 0; i < enemies.size(); i++)
            {
                if (player.getGlobalBounds().findIntersection(enemies[i].getGlobalBounds()))
                {
                    enemies.erase(enemies.begin() + i);
                    explosionSound.play();
                    if (weapon_level > 1)
                    {
                        weapon_level -= 1;
                        weapon_levelText.setString(to_string(weapon_level));
                    }
                    else if (hp == 1)
                    {
                        isGameOver = true;
                        game_over_sound.play();
                    }
                    else if (hp > 1)
                    {
                        hp -= 1;
                        hpText.setString(to_string(hp));
                        break;
                    }

                    i--;
                }
            }

            // -------------------------------------------------
            // Enemy2 logic
            // -------------------------------------------------

            if (enemy2SpawnTimer.getElapsedTime().asSeconds() > 3.0f && score >= 15)
            {
                Enemy2 newEnemy2(textures["Enemy2Texture"]);
                newEnemy2.sprite.setScale({1.1f, 1.1f});

                float enemy2Width = newEnemy2.sprite.getGlobalBounds().size.x;
                float randomX = static_cast<float>(rand() % static_cast<int>(800 - enemy2Width));

                newEnemy2.sprite.setPosition({randomX, -50.f});
                newEnemy2.health = 2;

                enemies2.push_back(newEnemy2);
                enemy2SpawnTimer.restart();
            }

            // Enemy2 movement
            for (size_t i = 0; i < enemies2.size(); i++)
            {
                enemies2[i].sprite.move({0.f, 2.f});

                // Clearing enemies that are off the screen
                if (enemies2[i].sprite.getPosition().y > 600.f)
                {
                    enemies2.erase(enemies2.begin() + i);
                    i--;
                }
            }

            // Checking to see if the enemy2 has been hit by a bullet
            if (weapon_level == 1)
            {
                for (size_t i = 0; i < bullets.size(); i++)
                {
                    for (size_t j = 0; j < enemies2.size(); j++)
                    {

                        if (bullets[i].getGlobalBounds().findIntersection(enemies2[j].sprite.getGlobalBounds()))
                        {
                            hit_enemy1Sound.play();

                            score += 2;
                            scoreText.setString(to_string(score));

                            enemies2[j].health--;
                            if (enemies2[j].health <= 0)
                            {
                                enemies2.erase(enemies2.begin() + j);
                                j--;
                            }

                            bullets.erase(bullets.begin() + i);
                            i--;

                            break;
                        }
                    }
                }
            }
            else if (weapon_level == 2)
            {
                for (size_t i = 0; i < bullets_2.size(); i++)
                {
                    for (size_t j = 0; j < enemies2.size(); j++)
                    {

                        if (bullets_2[i].getGlobalBounds().findIntersection(enemies2[j].sprite.getGlobalBounds()))
                        {
                            hit_enemy1Sound.play();

                            score += 2;
                            scoreText.setString(to_string(score));

                            enemies2.erase(enemies2.begin() + j);
                            j--;

                            bullets_2.erase(bullets_2.begin() + i);
                            i--;

                            break;
                        }
                    }
                }
            }

            // Hitting the enemy2 with a ship

            for (size_t i = 0; i < enemies2.size(); i++)
            {
                if (player.getGlobalBounds().findIntersection(enemies2[i].sprite.getGlobalBounds()))
                {
                    enemies2.erase(enemies2.begin() + i);
                    explosionSound.play();
                    if (weapon_level > 1)
                    {
                        weapon_level -= 1;
                        weapon_levelText.setString(to_string(weapon_level));
                    }
                    else if (hp == 1 && weapon_level == 1)
                    {
                        isGameOver = true;
                        game_over_sound.play();
                    }
                    else if (hp > 1 && weapon_level == 1)
                    {
                        hp -= 1;
                        hpText.setString(to_string(hp));
                        break;
                    }

                    i--;
                }
            }

            // -------------------------------------------------
            // Enemy3 logic
            // -------------------------------------------------

            if (enemy3SpawnTimer.getElapsedTime().asSeconds() > 3.0f && score >= 40)
            {
                Enemy3 newEnemy3(textures["Enemy3Texture"]);
                newEnemy3.sprite.setScale({1.3f, 1.3f});

                float enemy3Width = newEnemy3.sprite.getGlobalBounds().size.x;
                float randomX = static_cast<float>(rand() % static_cast<int>(800 - enemy3Width));

                newEnemy3.sprite.setPosition({randomX, -75.f});
                newEnemy3.health = 4;

                enemies3.push_back(newEnemy3);

                enemy3SpawnTimer.restart();
            }

            // Enemy3 movement
            for (size_t i = 0; i < enemies3.size(); i++)
            {
                enemies3[i].sprite.move({0.f, 1.f});

                // Clearing enemies that are off the screen
                if (enemies3[i].sprite.getPosition().y > 600.f)
                {
                    enemies3.erase(enemies3.begin() + i);
                    i--;
                }
            }

            // Checking to see if the enemy3 has been hit by a bullet
            if (weapon_level == 1)
            {
                for (size_t i = 0; i < bullets.size(); i++)
                {
                    for (size_t j = 0; j < enemies3.size(); j++)
                    {

                        if (bullets[i].getGlobalBounds().findIntersection(enemies3[j].sprite.getGlobalBounds()))
                        {
                            hit_enemy1Sound.play();

                            score += 3;
                            scoreText.setString(to_string(score));

                            enemies3[j].health--;
                            if (enemies3[j].health <= 0)
                            {
                                enemies3.erase(enemies3.begin() + j);
                                j--;
                            }

                            bullets.erase(bullets.begin() + i);

                            i--;

                            break;
                        }
                    }
                }
            }
            else if (weapon_level == 2)
            {
                for (size_t i = 0; i < bullets_2.size(); i++)
                {
                    for (size_t j = 0; j < enemies3.size(); j++)
                    {

                        if (bullets_2[i].getGlobalBounds().findIntersection(enemies3[j].sprite.getGlobalBounds()))
                        {
                            hit_enemy1Sound.play();

                            score += 3;
                            scoreText.setString(to_string(score));

                            enemies3[j].health -= 2;
                            if (enemies3[j].health <= 0)
                            {
                                enemies3.erase(enemies3.begin() + j);
                                j--;
                            }

                            bullets_2.erase(bullets_2.begin() + i);

                            i--;

                            break;
                        }
                    }
                }
            }

            // Hitting the enemy3 with a ship

            for (size_t i = 0; i < enemies3.size(); i++)
            {
                if (player.getGlobalBounds().findIntersection(enemies3[i].sprite.getGlobalBounds()))
                {
                    enemies3.erase(enemies3.begin() + i);
                    explosionSound.play();
                    if (weapon_level > 1)
                    {
                        weapon_level -= 1;
                        weapon_levelText.setString(to_string(weapon_level));
                    }
                    else if (hp == 1 && weapon_level == 1)
                    {
                        isGameOver = true;
                        game_over_sound.play();
                    }
                    else if (hp > 1 && weapon_level == 1)
                    {
                        hp -= 1;
                        hpText.setString(to_string(hp));
                        break;
                    }

                    i--;
                }
            }

            FloatRect enemy3_bounds = enemy3.getGlobalBounds();

            // enemy3_shooting

            if (enemy3_shootTimer.getElapsedTime().asSeconds() > 3.2f)
            {
                for (size_t i = 0; i < enemies3.size(); i++)
                {
                    Sprite bullet(textures["Enemy3_BulletsTexture"]);
                    float bulletX = enemies3[i].sprite.getPosition().x + enemy3_bounds.size.x / 2 - 4;
                    float bulletY = enemies3[i].sprite.getPosition().y + enemy3_bounds.size.y - 4;
                    bullet.setPosition({bulletX, bulletY});
                    enemy3_bullets.push_back(bullet);
                    enemy3_shootTimer.restart();
                }
                enemy3_shootTimer.restart();
            }

            // hiting player with enemy bullets
            for (size_t i = 0; i < enemy3_bullets.size(); i++)
            {
                if (player.getGlobalBounds().findIntersection(enemy3_bullets[i].getGlobalBounds()))
                {
                    enemy3_bullets.erase(enemy3_bullets.begin() + i);
                    explosionSound.play();
                    if (weapon_level > 1)
                    {
                        weapon_level -= 1;
                        weapon_levelText.setString(to_string(weapon_level));
                    }
                    else if (hp == 1 && weapon_level == 1)
                    {
                        isGameOver = true;
                        game_over_sound.play();
                    }
                    else if (hp > 1 && weapon_level == 1)
                    {
                        hp -= 1;
                        hpText.setString(to_string(hp));
                        break;
                    }

                    i--;
                }
            }

            // remove enemy3_bullets from vector
            for (size_t i = 0; i < enemy3_bullets.size(); i++)
            {
                enemy3_bullets[i].move({0.f, 3.f});
                if (enemy3_bullets[i].getPosition().y > 620.f)
                {
                    enemy3_bullets.erase(enemy3_bullets.begin() + i);
                    i--;
                }
            }

            // -------------------------------------------------
            // Enemy4 logic
            // -------------------------------------------------

            if (enemy4SpawnTimer.getElapsedTime().asSeconds() > 3.0f && score >= 60)
            {
                Enemy4 newEnemy4(textures["Enemy4Texture"]);
                newEnemy4.sprite.setScale({1.3f, 1.3f});

                float enemy4Width = enemy4.getGlobalBounds().size.x;
                float randomX = static_cast<float>(rand() % static_cast<int>(800 - enemy4Width));

                newEnemy4.sprite.setPosition({randomX, -50.f});
                newEnemy4.health = 8;

                enemies4.push_back(newEnemy4);

                enemy4SpawnTimer.restart();
            }

            // Enemy4 movement
            for (size_t i = 0; i < enemies4.size(); i++)
            {
                enemies4[i].sprite.move({0.f, 1.f});

                // Clearing enemies that are off the screen
                if (enemies4[i].sprite.getPosition().y > 600.f)
                {
                    enemies4.erase(enemies4.begin() + i);
                    i--;
                }
            }

            // Checking to see if the enemy4 has been hit by a bullet
            if (weapon_level == 1)
            {
                for (size_t i = 0; i < bullets.size(); i++)
                {
                    for (size_t j = 0; j < enemies4.size(); j++)
                    {

                        if (bullets[i].getGlobalBounds().findIntersection(enemies4[j].sprite.getGlobalBounds()))
                        {
                            hit_enemy1Sound.play();

                            score += 5;
                            scoreText.setString(to_string(score));

                            enemies4[j].health--;
                            if (enemies4[j].health <= 0)
                            {
                                enemies4.erase(enemies4.begin() + j);
                                j--;
                            }

                            bullets.erase(bullets.begin() + i);

                            i--;

                            break;
                        }
                    }
                }
            }
            else if (weapon_level == 2)
            {
                for (size_t i = 0; i < bullets_2.size(); i++)
                {
                    for (size_t j = 0; j < enemies4.size(); j++)
                    {

                        if (bullets_2[i].getGlobalBounds().findIntersection(enemies4[j].sprite.getGlobalBounds()))
                        {
                            hit_enemy1Sound.play();

                            score += 5;
                            scoreText.setString(to_string(score));

                            enemies4[j].health -= 2;
                            if (enemies4[j].health <= 0)
                            {
                                enemies4.erase(enemies4.begin() + j);
                                j--;
                            }

                            bullets_2.erase(bullets_2.begin() + i);

                            i--;

                            break;
                        }
                    }
                }
            }

            // Hitting the enemy4 with a ship
            for (size_t i = 0; i < enemies4.size(); i++)
            {
                if (player.getGlobalBounds().findIntersection(enemies4[i].sprite.getGlobalBounds()))
                {
                    enemies4.erase(enemies4.begin() + i);
                    explosionSound.play();
                    if (weapon_level > 1)
                    {
                        weapon_level -= 1;
                        weapon_levelText.setString(to_string(weapon_level));
                    }
                    else if (hp == 1 && weapon_level == 1)
                    {
                        isGameOver = true;
                        game_over_sound.play();
                    }
                    else if (hp > 1 && weapon_level == 1)
                    {
                        hp -= 1;
                        hpText.setString(to_string(hp));
                        break;
                    }

                    i--;
                }
            }

            FloatRect enemy4_bounds = enemy4.getGlobalBounds();

            // enemy4_shooting

            if (enemy4_shootTimer.getElapsedTime().asSeconds() > 3.2f)
            {
                for (size_t i = 0; i < enemies4.size(); i++)
                {
                    Sprite bullet_l(textures["Enemy4_BulletsTexture"]);
                    Sprite bullet_r(textures["Enemy4_BulletsTexture"]);

                    float bulletX_l = enemies4[i].sprite.getPosition().x + enemy4_bounds.size.x / 2 - 17;
                    float bulletX_r = enemies4[i].sprite.getPosition().x + enemy4_bounds.size.x / 2 + 10;

                    float bulletY = enemies4[i].sprite.getPosition().y + enemy4_bounds.size.y - 4;

                    bullet_l.setPosition({bulletX_l, bulletY});
                    bullet_r.setPosition({bulletX_r, bulletY});

                    enemy4_bullets.push_back(bullet_l);
                    enemy4_bullets.push_back(bullet_r);

                    enemy4_shootTimer.restart();
                }
                enemy4_shootTimer.restart();
            }

            // hiting player with enemy bullets
            for (size_t i = 0; i < enemy4_bullets.size(); i++)
            {
                if (player.getGlobalBounds().findIntersection(enemy4_bullets[i].getGlobalBounds()))
                {
                    enemy4_bullets.erase(enemy4_bullets.begin() + i);
                    explosionSound.play();
                    if (weapon_level > 1)
                    {
                        weapon_level -= 1;
                        weapon_levelText.setString(to_string(weapon_level));
                    }
                    else if (hp == 1 && weapon_level == 1)
                    {
                        isGameOver = true;
                        game_over_sound.play();
                    }
                    else if (hp > 1 && weapon_level == 1)
                    {
                        hp -= 1;
                        hpText.setString(to_string(hp));
                        break;
                    }

                    i--;
                }
            }

            // remove enemy4_bullets from vector
            for (size_t i = 0; i < enemy4_bullets.size(); i++)
            {
                enemy4_bullets[i].move({0.f, 3.f});
                if (enemy4_bullets[i].getPosition().y > 620.f)
                {
                    enemy4_bullets.erase(enemy4_bullets.begin() + i);
                    i--;
                }
            }

            // -------------------------------------------------
            // PowerUp Logic
            // -------------------------------------------------
            if (powerUpSpawnTimer.getElapsedTime().asSeconds() > POWERUP_SPAWN_RATE && score > 40)
            {
                PowerUp newPowerUp(textures["powerUpTexture"]);
                newPowerUp.sprite.setScale({0.2f, 0.2f});

                float powerUpWidth = newPowerUp.sprite.getGlobalBounds().size.x;
                float randomX = static_cast<float>(rand() % static_cast<int>(800 - powerUpWidth));

                newPowerUp.sprite.setPosition({randomX, -50.f});

                powerUps.push_back(newPowerUp);
                powerUpSpawnTimer.restart();
            }

            // PowerUp movement and collision
            for (size_t i = 0; i < powerUps.size(); i++)
            {
                powerUps[i].sprite.move({0.f, 0.8f});

                if (player.getGlobalBounds().findIntersection(powerUps[i].sprite.getGlobalBounds()))
                {
                    PowerUp_sound.play();
                    powerUps.erase(powerUps.begin() + i);
                    if (weapon_level < 5)
                    {
                        weapon_level += 1;
                        weapon_levelText.setString(to_string(weapon_level));
                    }
                    i--;
                    continue;
                }

                // Clearing powerUps that are off the screen
                if (powerUps[i].sprite.getPosition().y > 600.f)
                {
                    powerUps.erase(powerUps.begin() + i);
                    i--;
                }
            }

            // -------------------------------------------------
            // PowerDown Logic
            // -------------------------------------------------
            if (powerDownSpawnTimer.getElapsedTime().asSeconds() > POWERDOWN_SPAWN_RATE && score > 45)
            {
                PowerDown newpowerDown(textures["powerDownTexture"]);
                newpowerDown.sprite.setScale({0.3f, 0.3f});

                float powerDownWidth = newpowerDown.sprite.getGlobalBounds().size.x;
                float randomX = static_cast<float>(rand() % static_cast<int>(800 - powerDownWidth));

                newpowerDown.sprite.setPosition({randomX, -50.f});

                powerDowns.push_back(newpowerDown);
                powerDownSpawnTimer.restart();
            }

            // PowerDown movement and collision
            for (size_t i = 0; i < powerDowns.size(); i++)
            {
                powerDowns[i].sprite.move({0.f, 0.8f});

                if (player.getGlobalBounds().findIntersection(powerDowns[i].sprite.getGlobalBounds()))
                {
                    powerDowns.erase(powerDowns.begin() + i);
                    PowerDown_sound.play();
                    if (weapon_level >= 2)
                    {
                        weapon_level -= 1;
                        weapon_levelText.setString(to_string(weapon_level));
                    }
                    i--;
                    continue;
                }

                // Clearing powerDowns that are off the screen
                if (powerDowns[i].sprite.getPosition().y > 600.f)
                {
                    powerDowns.erase(powerDowns.begin() + i);
                    i--;
                }
            }
        }

        // game render
        window.clear(Color::Black);
        if (isGameOver)
        {
            window.draw(board);
            window.draw(player_score);
            window.draw(scoreText);
            if (Mouse::isButtonPressed(Mouse::Button::Left))
            {
                Vector2f mouseF = {(float)mousePos.x, (float)mousePos.y};

                FloatRect yesRect({295.f, 346.f}, {85.f, 37.f});

                if (yesRect.contains(mouseF))
                {
                    YES_sound.play();
                    isGameOver = false;
                    hp = 3;
                    score = 0;
                    enemies.clear();
                    enemies2.clear();
                    enemies3.clear();
                    enemies4.clear();
                    bullets.clear();
                    enemy3_bullets.clear();
                    enemy4_bullets.clear();

                    scoreText.setString("0");
                    score = 0;
                    hpText.setString("3");
                    hp = 3;
                    player.setPosition({350.f, 500.f});
                }

                FloatRect noRect({416.f, 347.f}, {93.f, 37.f});

                if (noRect.contains(mouseF))
                {
                    window.close();
                }
            }
        }
        else
        {
            window.draw(board2);

            for (const auto &bullet : bullets)
                window.draw(bullet);

            for (const auto &bullet : bullets_2)
                window.draw(bullet);

            window.draw(player);

            for (const auto &enemy : enemies)
                window.draw(enemy);

            for (const auto &enemy2 : enemies2)
                window.draw(enemy2.sprite);

            for (const auto &enemy3 : enemies3)
                window.draw(enemy3.sprite);

            for (const auto &enemy3_bullet : enemy3_bullets)
                window.draw(enemy3_bullet);

            for (const auto &enemy4 : enemies4)
                window.draw(enemy4.sprite);

            for (const auto &enemy4_bullet : enemy4_bullets)
                window.draw(enemy4_bullet);

            for (const auto &pu : powerUps)
                window.draw(pu.sprite);

            for (const auto &pd : powerDowns)
                window.draw(pd.sprite);

            window.draw(player_hp);
            window.draw(player_score);
            window.draw(scoreText);
            window.draw(hpText);
            window.draw(weapon_levelText);
            window.draw(player_weapon_level);
        }
        window.draw(pointer);
        window.display();
    }
}