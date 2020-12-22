//
// Created by miki on 22.12.2020.
//

#include "GameProcess.h"
#include <cmath>

#include <SFML/Graphics.hpp>

[[noreturn]] void GameProcess::run()
{
    // create empty messages
    auto* mes_in = (GameMes*)malloc(sizeof(GameMes));
    auto* mes_out = (LogMes*)malloc(sizeof(LogMes));

    // setup the renderer and open the window
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Pointer");
    sf::CircleShape shape(5, 16);
    shape.setFillColor(sf::Color::Green);

    sf::Event event;

    while (window.isOpen())
    {
        while (window.pollEvent(event));

        // get a message
        auto* temp = mes_in;
        game_q.receiveMes(temp);

        // store the time when the message has arrived
        auto end = std::chrono::system_clock::now();

        if (temp)
        {
#ifndef NDEBUG
            std::cout << "GameProcess (receiving): " << mes_in->id << ": " << mes_in->x << ' ' << mes_in->y << '\n';
#endif
            // create a message to send to the logger
            mes_out->id = mes_in->id;
            mes_out->begin = mes_in->timestamp;
            mes_out->end = end;

            // send the message
            log_q.sendMes(mes_out);

            // set the radius of the circular pointer
            int rad = sqrt(mes_in->scale)/4;

            if(rad < LOWER_SCALE_BOUND) rad = LOWER_SCALE_BOUND;
            if(rad > UPPER_SCALE_BOUND) rad = UPPER_SCALE_BOUND;

            //update the display
            shape.setPosition(mes_in->x - rad / 2, mes_in->y - rad / 2);
            shape.setRadius(rad);
            window.clear();
            window.draw(shape);
            window.display();
        }
    }
    delete mes_in;
    delete mes_out;
}