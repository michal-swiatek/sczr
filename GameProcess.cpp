//
// Created by miki on 22.12.2020.
//

#include "GameProcess.h"

#include <SFML/Graphics.hpp>

[[noreturn]] void GameProcess::run()
{
    auto* mes_in = (GameMes*)malloc(sizeof(GameMes));
    auto* mes_out = (LogMes*)malloc(sizeof(LogMes));
    sf::RenderWindow window(sf::VideoMode(640, 480), "SFML works!");
    sf::CircleShape shape(5, 16);
    shape.setFillColor(sf::Color::Green);

    while (window.isOpen())
    {
        auto* temp = mes_in;
        game_q.receiveMes(temp);
        auto end = std::chrono::system_clock::now();

        if (temp)
        {
#ifndef NDEBUG
            std::cout << "GameProcess (receiving): " << mes_in->id << ": " << mes_in->x << ' ' << mes_in->y << '\n';
#endif
            mes_out->id = mes_in->id;
            mes_out->begin = mes_in->timestamp;
            mes_out->end = end;
            log_q.sendMes(mes_out);
            shape.setPosition(mes_in->x, mes_in->y);
            window.clear();
            window.draw(shape);
            window.display();
        }
    }
}