// g++ graphic_main.cpp -I C:\SFML-3.0.2\include -L C:\SFML-3.0.2\lib -o graphic_main.exe -lsfml-graphics -lsfml-window -lsfml-system

#include <iostream>
#include <optional>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>

using namespace std;
using namespace sf;

int main()
{
    // make window for game
    RenderWindow window(VideoMode({800, 600}), "Space War Game", Style::Titlebar | Style::Close | Style::Resize);
    window.setFramerateLimit(60);

    // -------------------------------------------------
    //
    // -------------------------------------------------

    // palyer shape
    CircleShape player(50.f);
    player.setFillColor(Color::Green);

    // set position for player
    player.setPosition({350.f, 250.f});

    // game loop
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

        // Calculating player height
        float playerSize = player.getRadius() * 2;

        // Checking to make sure the player doesn't exit the game screen
        if (pos.x < 0.f)
        {
            player.setPosition({0.f, pos.y});
        }
        if (pos.y < 0.f)
        {
            player.setPosition({pos.x, 0.f});
        }
        if (pos.x + playerSize > 800.f)
        {
            player.setPosition({800.f - playerSize, pos.y});
        }
        if (pos.y + playerSize > 600.f)
        {
            player.setPosition({pos.x, 600.f - playerSize});
        }

        // game render
        window.clear(Color::Black);

        window.draw(player);

        window.display();
    }
}