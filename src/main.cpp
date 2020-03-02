/*
    Connect Four 2014 (c) 2014 George M. Tzoumas

    This file is part of Connect Four 2014.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// main program

/*
 *  TODO:
 *      track depth along with history (so that takeback does not leave big depth)
 *      toggle sound from cmdline
 */

#include "gameview.h"
#include "audio.h"
#include "game.h"
#include "frameratecontroller.h"
#include "arguments.hpp"
#include "connect4.hpp"

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <vector>
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>


struct Options
{
    bool windowed = false;
    int frame_rate = 15;
    bool demo = false;
};

int main(int argc, char **argv)
{
//    std::vector<sf::VideoMode> VModes = sf::VideoMode::getFullscreenModes();
//    for (unsigned i=0; i<VModes.size(); ++i) {
//        std::cout <<  VModes[i].width << 'x' << VModes[i].height << ' '  << VModes[i].bitsPerPixel << std::endl;
//    }


    Options opt;
    bool status = parse_argv(argc, argv,
                             Arg<bool>("--nofs", opt.windowed, true),
                             Arg<int>("--fps", opt.frame_rate, 15, true),
                             Arg<bool>("--demo", opt.demo, true)
                             );
    if (!status) return 1;

    GameView gv(!opt.windowed);
	AudioInterface aud;
    Game game(gv, aud);

    std::srand(std::time(NULL));

    auto win = gv.getWindow();
    FrameRateController fps{opt.frame_rate};
//    win->setFramerateLimit(0);

    if (opt.demo) game.acDemo();
    while (win->isOpen())
    {
        sf::Event event;
        while (win->pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    win->close();
                    break;
//                case sf::Event::Resized:
//                    std::cout << "new width: " << event.size.width << std::endl;
//                    std::cout << "new height: " << event.size.height << std::endl;
//                    break;
                case sf::Event::TextEntered:
					switch (int c = event.text.unicode) {
						case '$':
							game.acRestart();
							break;
						case '@':
							game.ac2Player();
							break;
							
						case '1':
						case '2':
						case '3':
						case '4':
						case '5':
						case '6':
						case '7':
							game.acPlay(c-'1');
							break;
					}
					break;
					
                case sf::Event::KeyPressed:
                    switch (event.key.code) {
                        case sf::Keyboard::Escape: 
                        case sf::Keyboard::Q:
                            win->close();
                            break;
                        case sf::Keyboard::C:
                            game.acCompute();
                            break;
                        case sf::Keyboard::A:
                            game.acToggleAlgo();
                            break;
                        case sf::Keyboard::D:
                            game.acDemo();
                            break;
                        case sf::Keyboard::F:
                            if (event.key.system) game.acToggleFullscreen();
                            else game.acForward();
                            break;
                        case sf::Keyboard::S:
                            game.acToggleSound();
                            break;
                        case sf::Keyboard::T:
                            game.acTakeback();
                            break;
                        case sf::Keyboard::Return:
                            if (event.key.alt) game.acToggleFullscreen();
                            break;
                        default:
                            break;
                    }
                    break;
					
                case sf::Event::MouseButtonPressed: 
                {
                    sf::Vector2f v = win->mapPixelToCoords(sf::Vector2i(event.mouseButton.x,event.mouseButton.y));
                    //std::cerr << "x = " << v.x << " y = " << v.y << std::endl;
                    auto [x,y] = gv.getGrid(v.x, v.y);
                    //std::cerr << "row = " << y << " col = " << x << std::endl;
                    game.acPlay(y, x);
                }
                    break;
                default:
                    break;
            }
        }

        gv.setFpsString( fps.real_fps() );
        game.render();
        fps();
        if (game.is_demo(game.state().next_player())) game.acPlay();
    }
    return 0;
}

