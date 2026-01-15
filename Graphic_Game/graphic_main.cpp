// g++ graphic_main.cpp resource.o -I C:\SFML-3.0.2\include -L C:\SFML-3.0.2\lib -o graphic_main.exe -lsfml-audio -lsfml-graphics -lsfml-window -lsfml-system -mwindows

#include <iostream>
#include <optional>
#include <vector>
#include <cstdlib>
#include <ctime>

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
    Texture spaceshipTexture;
    if (!spaceshipTexture.loadFromFile("spaceship.png"))
    {
        cerr << "Error: Could not load spaceship.png!" << endl;
        return -1;
    }

    // Paste the image into the sprite
    Sprite player(spaceshipTexture);
    player.setScale({1.3f, 1.3f});

    // set position for player
    player.setPosition({350.f, 500.f});

    // player_hp
    int hp = 3;
    Texture hpTexture;
    if (!hpTexture.loadFromFile("heart.png"))
    {
        cerr << "Error: Could not load heart.png!" << endl;
        return -1;
    }

    Sprite player_hp(hpTexture);
    player_hp.setScale({1.1f, 1.1f});

    // set position for player_hp
    player_hp.setPosition({5.f, 5.f});

    // player_score
    int score = 0;
    Texture scoreTexture;
    if (!scoreTexture.loadFromFile("score.png"))
    {
        cerr << "Error: Could not load score.png!" << endl;
        return -1;
    }

    Sprite player_score(scoreTexture);
    player_score.setScale({0.15f, 0.15f});

    // set position for player_hp
    player_score.setPosition({12.f, 45.f});

    // -------------------------------------------------
    // Enemy1
    // -------------------------------------------------
    Texture EnemyTexture;
    if (!EnemyTexture.loadFromFile("spritesheet.png"))
        return -1;
    Sprite enemy(EnemyTexture);

    vector<Sprite> enemies;
    Clock enemySpawnTimer;

    // -------------------------------------------------
    // Enemy2
    // -------------------------------------------------
    Texture Enemy2Texture;
    if (!Enemy2Texture.loadFromFile("enemy2.png"))
        return -1;
    Sprite enemy2(Enemy2Texture);

    Clock enemy2SpawnTimer;

    vector<Enemy2> enemies2;

    // -------------------------------------------------
    // Enemy3
    // -------------------------------------------------
    Texture Enemy3Texture;
    if (!Enemy3Texture.loadFromFile("enemy3.png"))
        return -1;
    Sprite enemy3(Enemy3Texture);

    vector<Enemy3> enemies3;
    Clock enemy3SpawnTimer;

    // -------------------------------------------------
    // Enemy4
    // -------------------------------------------------
    Texture Enemy4Texture;
    if (!Enemy4Texture.loadFromFile("enemy4.png"))
        return -1;
    Sprite enemy4(Enemy4Texture);

    vector<Sprite> enemies4;
    Clock enemy4SpawnTimer;

    // -------------------------------------------------
    // Bullets
    // -------------------------------------------------
    vector<RectangleShape> bullets;
    Clock shootTimer;

    Texture Enemy3_BulletsTexture;
    if (!Enemy3_BulletsTexture.loadFromFile("enemy2_bullets.png"))
        return -1;
    Sprite enemy3_bullet(Enemy3_BulletsTexture);
    vector<Sprite> enemy3_bullets;
    Clock enemy3_shootTimer;

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

    // -------------------------------------------------
    // Sound Effects
    // -------------------------------------------------
    SoundBuffer explosionBuffer;
    if (!explosionBuffer.loadFromFile("explosion.wav"))
    {
        return -1;
    }
    Sound explosionSound(explosionBuffer);

    SoundBuffer shootingBuffer;
    if (!shootingBuffer.loadFromFile("shooting.wav"))
    {
        return -1;
    }
    Sound shootingSound(shootingBuffer);

    SoundBuffer hit_enemy1Buffer;
    if (!hit_enemy1Buffer.loadFromFile("hit01.wav"))
    {
        return -1;
    }
    Sound hit_enemy1Sound(hit_enemy1Buffer);

    SoundBuffer game_over_Buffer;
    if (!game_over_Buffer.loadFromFile("Game_Over.wav"))
    {
        return -1;
    }
    Sound game_over_sound(game_over_Buffer);

    SoundBuffer YES_Buffer;
    if (!YES_Buffer.loadFromFile("YES.flac"))
    {
        return -1;
    }
    Sound YES_sound(YES_Buffer);

    /// -------------------------------------------------
    // POINTER
    // -------------------------------------------------
    Texture pointerTexture;
    if (!pointerTexture.loadFromFile("pointer.png"))
    {
        cerr << "Error: Could not load pointer.png!" << endl;
        return -1;
    }

    // Paste the image into the sprite
    Sprite pointer(pointerTexture);
    pointer.setScale({0.1f, 0.1f});

    /// -------------------------------------------------
    // GAME_Board
    // -------------------------------------------------
    Texture boardTexture;
    if (!boardTexture.loadFromFile("game_over.jpg"))
        return -1;
    Sprite board(boardTexture);

    board.setScale({0.39f, 0.42f});
    board.setPosition({0.f, 0.f});

    Texture board2Texture;
    if (!board2Texture.loadFromFile("game_board.png"))
        return -1;
    Sprite board2(board2Texture);

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

            if (Keyboard::isKeyPressed(Keyboard::Key::Space) && shootTimer.getElapsedTime().asSeconds() > 0.4f)
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

            // -------------------------------------------------
            // Enemy1 logic
            // -------------------------------------------------

            if (enemySpawnTimer.getElapsedTime().asSeconds() > 2.0f)
            {
                Sprite enemy(EnemyTexture);
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

            // Hitting the enemy with a ship

            for (size_t i = 0; i < enemies.size(); i++)
            {
                if (player.getGlobalBounds().findIntersection(enemies[i].getGlobalBounds()))
                {
                    enemies.erase(enemies.begin() + i);
                    explosionSound.play();
                    if (hp == 1)
                    {
                        isGameOver = true;
                        game_over_sound.play();
                    }
                    else
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
                Enemy2 newEnemy2(Enemy2Texture);
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

            // Hitting the enemy2 with a ship

            for (size_t i = 0; i < enemies2.size(); i++)
            {
                if (player.getGlobalBounds().findIntersection(enemies2[i].sprite.getGlobalBounds()))
                {
                    enemies2.erase(enemies2.begin() + i);
                    explosionSound.play();
                    if (hp == 1)
                    {
                        isGameOver = true;
                        game_over_sound.play();
                    }
                    else
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
                Enemy3 newEnemy3(Enemy3Texture);
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

            // Hitting the enemy3 with a ship

            for (size_t i = 0; i < enemies3.size(); i++)
            {
                if (player.getGlobalBounds().findIntersection(enemies3[i].sprite.getGlobalBounds()))
                {
                    enemies3.erase(enemies3.begin() + i);
                    explosionSound.play();
                    if (hp == 2 || hp == 1)
                    {
                        isGameOver = true;
                        game_over_sound.play();
                    }
                    else if (hp == 3)
                    {
                        hp -= 2;
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
                    Sprite bullet(Enemy3_BulletsTexture);
                    float bulletX = enemies3[i].sprite.getPosition().x + enemy3_bounds.size.x / 2 - 4;
                    float bulletY = enemies3[i].sprite.getPosition().y + enemy3_bounds.size.y - 4;
                    bullet.setPosition({bulletX, bulletY});
                    enemy3_bullets.push_back(bullet);
                    enemy3_shootTimer.restart();
                }
                enemy3_shootTimer.restart();
            }

            for (size_t i = 0; i < enemy3_bullets.size(); i++)
            {
                if (player.getGlobalBounds().findIntersection(enemy3_bullets[i].getGlobalBounds()))
                {
                    enemy3_bullets.erase(enemy3_bullets.begin() + i);
                    explosionSound.play();
                    if (hp == 1)
                    {
                        isGameOver = true;
                        game_over_sound.play();
                    }
                    else
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

            if (enemy4SpawnTimer.getElapsedTime().asSeconds() > 3.0f && score >= 80)
            {
                Sprite enemy4(Enemy4Texture);
                enemy4.setScale({1.3f, 1.3f});

                float enemy4Width = enemy4.getGlobalBounds().size.x;
                float randomX = static_cast<float>(rand() % static_cast<int>(800 - enemy4Width));

                enemy4.setPosition({randomX, -50.f});

                enemies4.push_back(enemy4);

                enemy4SpawnTimer.restart();
            }

            // Enemy4 movement
            for (size_t i = 0; i < enemies4.size(); i++)
            {
                enemies4[i].move({0.f, 1.f});

                // Clearing enemies that are off the screen
                if (enemies4[i].getPosition().y > 600.f)
                {
                    enemies4.erase(enemies4.begin() + i);
                    i--;
                }
            }

            // Checking to see if the enemy4 has been hit by a bullet
            for (size_t i = 0; i < bullets.size(); i++)
            {
                for (size_t j = 0; j < enemies4.size(); j++)
                {

                    if (bullets[i].getGlobalBounds().findIntersection(enemies4[j].getGlobalBounds()))
                    {
                        hit_enemy1Sound.play();

                        score += 3;
                        scoreText.setString(to_string(score));

                        enemies4.erase(enemies4.begin() + j);

                        bullets.erase(bullets.begin() + i);

                        i--;

                        break;
                    }
                }
            }

            // Hitting the enemy4 with a ship

            for (size_t i = 0; i < enemies4.size(); i++)
            {
                if (player.getGlobalBounds().findIntersection(enemies4[i].getGlobalBounds()))
                {
                    enemies4.erase(enemies4.begin() + i);
                    explosionSound.play();
                    if (hp == 1)
                    {
                        isGameOver = true;
                        game_over_sound.play();
                    }
                    else
                    {
                        hp -= 1;
                        hpText.setString(to_string(hp));
                        break;
                    }

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
            window.draw(player);

            for (const auto &bullet : bullets)
                window.draw(bullet);

            for (const auto &enemy : enemies)
                window.draw(enemy);

            for (const auto &enemy2 : enemies2)
                window.draw(enemy2.sprite);

            for (const auto &enemy3 : enemies3)
                window.draw(enemy3.sprite);

            for (const auto &enemy3_bullet : enemy3_bullets)
                window.draw(enemy3_bullet);

            for (const auto &enemy4 : enemies4)
                window.draw(enemy4);

            window.draw(player_hp);
            window.draw(player_score);
            window.draw(scoreText);
            window.draw(hpText);
        }
        window.draw(pointer);
        window.display();
    }
}