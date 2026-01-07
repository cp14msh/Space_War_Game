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
    // Enemy
    // -------------------------------------------------
    Texture EnemyTexture;
    if (!EnemyTexture.loadFromFile("spritesheet.png"))
        return -1;
    Sprite enemy(EnemyTexture);

    vector<Sprite> enemies;
    Clock enemySpawnTimer;

    // -------------------------------------------------
    // Bullets
    // -------------------------------------------------
    vector<RectangleShape> bullets;
    Clock shootTimer;

    // -------------------------------------------------
    // Text
    // -------------------------------------------------
    Font font;
    if (!font.openFromFile("font.ttf"))
    {
        cerr << "Error: Could not load font.ttf" << endl;
        return -1;
    }

    Text gameOverText(font);
    gameOverText.setString("GAME OVER");
    gameOverText.setCharacterSize(80);
    gameOverText.setFillColor(Color::Red);
    gameOverText.setStyle(Text::Style::Bold);

    gameOverText.setPosition({150.f, 250.f});

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
    // GAME_OVER
    // -------------------------------------------------
    Texture boardTexture;
    if (!boardTexture.loadFromFile("game_over.jpg"))
        return -1;
    Sprite board(boardTexture);

    board.setScale({0.39f, 0.42f});
    board.setPosition({0.f, 0.f});

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

            if (Keyboard::isKeyPressed(Keyboard::Key::Space) && shootTimer.getElapsedTime().asSeconds() > 0.2f)
            {
                shootingSound.play();

                RectangleShape bullet({3.f, 20.f});
                bullet.setFillColor(Color::Red);

                float bulletX = player.getPosition().x + (bounds.size.x / 2) - 2.5f;
                float bulletY = player.getPosition().y;
                bullet.setPosition({bulletX, bulletY});

                bullets.push_back(bullet);

                shootTimer.restart();
            }

            // remove bullets from vector
            for (size_t i = 0; i < bullets.size(); i++)
            {
                bullets[i].move({0.f, -10.f});

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

            // -------------------------------------------------
            // Hitting the enemy with a ship
            // -------------------------------------------------

            for (size_t i = 0; i < enemies.size(); i++)
            {
                if (player.getGlobalBounds().findIntersection(enemies[i].getGlobalBounds()))
                {
                    enemies.erase(enemies.begin() + i);
                    explosionSound.play();
                    if (hp == 1)
                    {
                        isGameOver = true;
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
        }
        else
        {

            window.draw(player);

            for (const auto &bullet : bullets)
                window.draw(bullet);

            for (const auto &enemy : enemies)
                window.draw(enemy);

            window.draw(player_hp);
            window.draw(player_score);
            window.draw(scoreText);
            window.draw(hpText);
        }
        window.draw(pointer);
        window.display();
    }
}