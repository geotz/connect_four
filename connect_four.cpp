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
 *      choose algorithm for AI
 *      track depth along with history (so that takeback does not leave big depth)
 *      setString ... change only on different string (now done manually not everywhere)
 */

#include<vector>
#include<iostream>
#include<string>
#include<cstdlib>
#include<ctime>
#include<cstring>

#include <SFML/Graphics.hpp>

#include "gameview.h"
#include "audio.h"
#include "game.h"
#include "framerateadjuster.h"

int main(int argc, char **argv)
{
//    std::vector<sf::VideoMode> VModes = sf::VideoMode::getFullscreenModes();
//    for (unsigned i=0; i<VModes.size(); i++) {
//        std::cout <<  VModes[i].width << 'x' << VModes[i].height << ' '  << VModes[i].bitsPerPixel << std::endl;
//    }
    bool wnd = argc > 1 && std::strcmp(argv[1], "--windowed") == 0;
    GameView gv(!wnd);
	AudioInterface aud;
    Game game(&gv, &aud);

    std::srand(std::time(NULL));
    sf::RenderWindow *win = gv.getWindow();
    FrameRateAdjuster fps{15};
//    win->setFramerateLimit(0);
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
							game.ac_restart();
							break;
						case '@':
							game.ac_2player();
							break;
							
						case '1':
						case '2':
						case '3':
						case '4':
						case '5':
						case '6':
						case '7':
							game.ac_play(c-'1');
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
                            game.ac_compute();
                            break;
                        case sf::Keyboard::A:
                            game.ac_toggle_algo();
                            break;
                        case sf::Keyboard::D:
                            game.ac_demo();
                            break;
                        case sf::Keyboard::F:
                            if (event.key.system) game.ac_toggle_fullscreen();
                            else game.ac_forward();
                            break;
                        case sf::Keyboard::S:
                            game.ac_toggle_sound();
                            break;
                        case sf::Keyboard::T:
                            game.ac_takeback();
                            break;
                        case sf::Keyboard::Return:
                            if (event.key.alt) game.ac_toggle_fullscreen();
                            break;
                        default:
                            break;
                    }
                    break;
					
                case sf::Event::MouseButtonPressed: 
                {
                    sf::Vector2f v = win->mapPixelToCoords(sf::Vector2i(event.mouseButton.x,event.mouseButton.y));
                    //std::cerr << "x = " << v.x << " y = " << v.y << std::endl;
                    sf::Vector2i grid = gv.getGrid(v);
                    //std::cerr << "row = " << grid.y << " col = " << grid.x << std::endl;
                    game.ac_play(grid.y, grid.x);
                }
                    break;
                default:
                    break;
            }
        }
        
        game.render();
        fps();
        if (game.is_demo(game.state().next_player())) game.ac_play();
    }
    return 0;
}

