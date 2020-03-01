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

// game view (interface)

#ifndef _GAMEVIEW_H_
#define _GAMEVIEW_H_

#include <memory>
#include <utility>

namespace sf
{
    class RenderWindow;
}

class Game;

struct ViewBase
{
    virtual ~ViewBase();
    virtual void update(Game *g) = 0;
    virtual void render() const = 0;
    virtual bool isFullscreen() const = 0;
    virtual void setFullscreen(bool enabled) = 0;
    void toggleFullscreen() { setFullscreen(!isFullscreen()); }
};

struct DummyView: public ViewBase
{
    void update(Game *g);
    void render() const;
    bool isFullscreen() const;
    void setFullscreen(bool enabled);
};

class GameView: public ViewBase
{
public:
    GameView(bool fullscreen = false);
    ~GameView();

    void update(Game *g);
    void render() const;
    bool isFullscreen() const;
    void setFullscreen(bool enabled);

    sf::RenderWindow* getWindow() const;

    std::pair<int,int> getGrid(float x, float y);
    void setFpsString( double fps );

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;
};

#endif
