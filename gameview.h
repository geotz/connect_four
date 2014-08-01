/*
    Connect Four 2014 (c) 2014 George M. Tzoumas

    This file is part of Connect Four 2014.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Foobar is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// game view (interface)

#ifndef _GAMEVIEW_H_
#define _GAMEVIEW_H_

#include <string>
#include "connect4.h"
#include <SFML/Graphics.hpp>

class Game;

struct ViewBase {
    virtual void update(Game *g) = 0;
    virtual void render() const = 0;
    virtual bool is_fullscreen() const = 0;
    virtual void setFullscreen(bool enabled) = 0;
    void toggleFullscreen() { setFullscreen(!is_fullscreen()); }
};

struct DummyView: public ViewBase {
    void update(Game *g) const { }
	void render() const { }
    bool is_fullscreen() const { return false; }
    void setFullscreen(bool enabled) { }
};

class GameView: public ViewBase {
public:
    GameView(bool fullscreen = false);
    void update(Game *g);
    void render() const;
    bool is_fullscreen() const { return _fullscreen; }
    void setFullscreen(bool enabled);

    sf::RenderWindow *getWindow() const { return win; }

    sf::Vector2i getGrid(sf::Vector2f pos);

private:
	struct Player {
		int id;
        std::string splay, swin, sthink, shuman, scomp;
		sf::Color color, hicolor, locolor;
		Player() { }
		Player(int id);
	};
	sf::RenderWindow *win;
    sf::RectangleShape rect;
    std::vector<std::vector<sf::CircleShape> > column;
    Player player[2];
    sf::Font font;
    sf::Text txInfo;
    sf::Text txColNum[7];
    sf::Text txHelp;
    sf::Text txPType[2], txVs;
    sf::Texture backgroundTexture;
    sf::Sprite background; // (gpu) memory consumption??
    bool _fullscreen;

    void updateGeometry();
    void mark_pos(int row, int col, int pl);
};

#endif
