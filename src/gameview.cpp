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

// game view (implementation)

#include "gameview.h"
#include "game.h"

#include <cmath>
#include <cstdio>
#include <array>
#include <string>

// include *.* :P
#include <SFML/Graphics.hpp>

ViewBase::~ViewBase() = default;

void DummyView::update(Game *g) {}
void DummyView::render() const {}
bool DummyView::isFullscreen() const { return false; }
void DummyView::setFullscreen(bool enabled) {}

struct GameView::Impl
{
    struct Player
    {
        int id;
        std::string splay, swin, sthink, shuman, scomp;
        sf::Color color, hicolor, locolor;
        Player(int id)
        {
            if (id) {
                splay = "PLAYER 2 TO PLAY";
                swin = "PLAYER 2 WINS";
                sthink = "PLAYER 2 IS THINKING ...";
                color = sf::Color(0xE8,0xE8,0);
                locolor = sf::Color(0xD8,0xD8,0x20);
                hicolor = sf::Color::Yellow;
            } else {
                splay = "PLAYER 1 TO PLAY";
                swin = "PLAYER 1 WINS";
                sthink = "PLAYER 1 IS THINKING ...";
                color = sf::Color(0xE8,0,0);
                locolor = sf::Color(0xC8,0,0x20);
                hicolor = sf::Color::Red;
            }
            shuman = "HUMAN";
            scomp = "COMPUTER";
        }
    };

    std::unique_ptr<sf::RenderWindow> win;
    sf::RectangleShape rect;
    std::vector<std::vector<sf::CircleShape>> column;
    std::array<Player,2> player;

    sf::Font font;
    sf::Font font2;
    sf::Text txInfo;
    sf::Text txMsg;
    sf::Text txPAlg[2];
    sf::Text txColNum[7];
    sf::Text txHelp;
    sf::Text txPType[2], txVs;
    sf::Text txFPS;

    sf::Texture backgroundTexture;
    sf::Sprite background; // (gpu) memory consumption??
    bool fullscreen;

    Impl(bool fullscreen):
        column(7),
        player{ Player(0), Player(1) },
        fullscreen(fullscreen)
    {
        win = fullscreen ? std::make_unique<sf::RenderWindow>(
                                        sf::VideoMode::getFullscreenModes()[0],
                                        "Connect Four 2014",
                                        sf::Style::Fullscreen)
                         : std::make_unique<sf::RenderWindow>(sf::VideoMode(840, 600),
                                                              "Connect Four 2014");
    //    win->setVerticalSyncEnabled(true);

        static const std::string resource_path = "./res/";

        font.loadFromFile(resource_path + "JLSSpaceGothicR_NC.otf");
        font2.loadFromFile(resource_path + "DroidSansMono.ttf");

        rect.setFillColor(sf::Color::Blue);
        rect.setOutlineColor(sf::Color::Cyan);
        rect.setOutlineThickness(5);

        for (int i = 0; i < 7; ++i) {
            column[i].reserve(6);
            for (int j = 0; j < 6; ++j) {
                column[i].push_back(sf::CircleShape());
            }
        }

        txInfo.setFont(font);
        txInfo.setCharacterSize(24);
        txInfo.setStyle(sf::Text::Regular);

        for (int i = 0; i < 2; ++i) {
            txPType[i].setFont(font);
            txPType[i].setCharacterSize(20);
            txPType[i].setStyle(sf::Text::Regular);
            txPType[i].setFillColor(player[i].hicolor);
            txPAlg[i].setFont(font2);
            txPAlg[i].setCharacterSize(15);
            txPAlg[i].setStyle(sf::Text::Regular);
            txPAlg[i].setFillColor(player[i].hicolor);
        }

        txVs.setFont(font);
        txVs.setCharacterSize(20);
        txVs.setStyle(sf::Text::Regular);
        txVs.setString(std::string("vs"));

        txHelp.setFont(font);
        txHelp.setCharacterSize(20);
        txHelp.setFillColor(sf::Color::Yellow);
        txHelp.setStyle(sf::Text::Regular);
        txHelp.setString("Connect Four 2014\n"
                         "   (c) 2014\n"
                         "George M. Tzoumas\n\n"

                         "1-7 ... Column to play\n"
                         "  $ ... Restart\n"
                         "  @ ... 2-player (human)\n"
                         "  A ... Algorithm selection\n"
                         "  C ... Compute\n"
                         "  D ... Demo\n"
                         "  F ... Forward\n"
                         "  T ... Takeback\n\n"

                         "  Q or Esc\n"
                         "    ... Quit\n\n"

                         "  S ...  Sound on/off\n\n"

                         "  Sys-F or Alt-Enter\n"
                         "    ... Toggle fullscreen\n"
                         );

        txMsg.setFont(font2);
        txMsg.setCharacterSize(15);
        txMsg.setFillColor(sf::Color::Cyan);
        txMsg.setStyle(sf::Text::Regular);

        txFPS.setFont(font2);
        txFPS.setCharacterSize(8);
        txFPS.setFillColor(sf::Color::White);
        txFPS.setStyle(sf::Text::Regular);
        txFPS.setPosition(0,0);

        for (int i = 0; i < 7; ++i) {
            txColNum[i].setFont(font);
            txColNum[i].setCharacterSize(24);
            txColNum[i].setStyle(sf::Text::Regular);
            txColNum[i].setFillColor(sf::Color::Cyan);
            txColNum[i].setString(std::string(1,'1'+i));
        }

        backgroundTexture.loadFromFile(resource_path  + "perseus_nasamarshall_3929624300.jpg");
        background.setTexture(backgroundTexture);

        update_geometry();
    }

    void update_geometry();
    void mark_pos(int row, int col, int pl);
};

GameView::GameView(bool fullscreen):
    _impl{ std::make_unique<Impl>(fullscreen) }
{}

GameView::~GameView() = default;

void GameView::Impl::update_geometry()
{
    sf::Vector2f size(win->getSize());
    size.y -= 80;
    float dy = 1.0*size.y/6;
    size.x = dy*7;
    rect.setSize(size);
    //        rect.move((size_.x-size.x)/2,10);
    rect.setPosition(10,40);
    //        column.reserve(7);
    float rad = 0.4*dy;
    sf::Vector2f prect = rect.getPosition();
    for (int i = 0; i < 7; ++i) {
        for (int j = 0; j < 6; ++j) {
            sf::CircleShape &t = column[i][j];
            t.setRadius(rad);
            t.setPosition(prect.x+i*dy+(dy-2*rad)/2,prect.y+(5-j)*dy+(dy-2*rad)/2);
        }
    }

    txInfo.setPosition(10,5);
    txPType[0].setPosition(size.x-220,5);
    txPType[1].setPosition(size.x-100,5);
    txVs.setPosition(size.x-127,5);
    txHelp.setPosition(size.x+30,40);
    txMsg.setPosition(size.x+30,size.y-30);
    txPAlg[0].setPosition(size.x+30,size.y-65);
    txPAlg[1].setPosition(size.x+30,size.y-50);

//    bool demo[2] = {false, true};
//    updateState(State(),demo);

    float dx = (column[1][0].getPosition().x-column[0][0].getPosition().x)/2;
    for (int i = 0; i < 7; ++i) {
        sf::Vector2f v = column[i][0].getPosition();
        txColNum[i].setPosition(v.x+dx-12,size.y+45);
    }

    sf::Vector2f v1(win->getSize());
    sf::Vector2f v2(backgroundTexture.getSize());
    background.setScale(sf::Vector2f(v1.x/v2.x, v1.y/v2.y));
}

std::pair<int,int> GameView::getGrid(float x, float y)
{
    float dist = 1.0e6;
    int row = -1, col = -1;
    float dx = (_impl->column[1][0].getPosition().x-_impl->column[0][0].getPosition().x)/2;
    float rad = _impl->column[0][0].getRadius();
    for (int i = 0; i < 7; ++i) {
        for (int j = 0; j < 6; ++j) {
            sf::Vector2f p = _impl->column[i][j].getPosition();
            sf::Vector2f center(p.x+dx,p.y+dx);
            float d = std::sqrt(std::pow(x-center.x,2)+std::pow(y-center.y,2));
            if (d <= rad && d < dist) dist = d, row = j, col = i;
        }
    }
    return {col,row};
}

void GameView::update(Game *g)
{
    static const std::string ALG[2] = { {"Alpha-Beta (Hard)"}, {"Monte-Carlo (Easy)"} };
    static const std::string DRAW = "DRAW";
    State s = g->state();
    for (int i = 0; i < 2; ++i) {
        _impl->txPType[i].setString(g->is_demo(i) ? _impl->player[i].scomp
                                                  : _impl->player[i].shuman);
        _impl->txPAlg[i].setString(ALG[(g->think_algo & (1<<i)) != 0]);
    }
    for (int i = 0; i < 7; ++i) {
        for (int j = 0; j < 6; ++j) {
			sf::Color color = sf::Color::Black;
            int c = s.get(j,i);
            if (c >= 0) color = _impl->player[c].color;
//            else color = sf::Color::White;
            sf::CircleShape &t = _impl->column[i][j];
//            t.setTexture(&backgroundTexture);
			t.setFillColor(color);
			t.setOutlineThickness(2);
			t.setOutlineColor(sf::Color::Black);
		}
	}
    if (!s.is_empty()) {
        int l = s.last_column();
        _impl->mark_pos(s.column_height(l)-1, s.last_column(), s.last_player());
	}
    if (s.is_terminal()) {
        int p = s.last_player();
        int w = s.winner_info();
        if (w == 2) _impl->txInfo.setString(DRAW);
        else _impl->txInfo.setString(_impl->player[p].swin);
        if ((w & 3) < 2) _impl->txInfo.setFillColor(_impl->player[p].color);
        else _impl->txInfo.setFillColor(sf::Color::White);
        if ((w & 3) < 2) {
			int dir = w >> 8;
			int row = (w >> 2) & 7;
			int col = (w >> 5) & 7;
			if (dir == 0) {
                for (int i = 0; i < 4; ++i) _impl->mark_pos(row, col+i, p);
			} else if (dir == 1) {
                for (int i = 0; i < 4; ++i) _impl->mark_pos(row+i, col, p);
			} else if (dir == 2) {
                for (int i = 0; i < 4; ++i) _impl->mark_pos(row+i, col+i, p);
			} else if (dir == 3) {
                for (int i = 0; i < 4; ++i) _impl->mark_pos(row+i, col-i, p);
			}
		}
	} else {
        int p = s.next_player();
        if (g->is_demo(p)) _impl->txInfo.setString(_impl->player[p].sthink);
        else _impl->txInfo.setString(_impl->player[p].splay);
        _impl->txInfo.setFillColor(_impl->player[p].hicolor);
    }
    _impl->txMsg.setString(g->get_msg());
}

void GameView::render() const 
{
    _impl->win->clear();
    //    win->pushGLStates();
    _impl->win->draw(_impl->background);
    _impl->win->draw(_impl->rect);
    for (int i = 0; i < 7; ++i) {
        for (int j = 0; j < 6; ++j) {
            _impl->win->draw(_impl->column[i][j]);
		}
        _impl->win->draw(_impl->txColNum[i]);
	}
    _impl->win->draw(_impl->txInfo);
    _impl->win->draw(_impl->txHelp);
    _impl->win->draw(_impl->txPType[0]);
    _impl->win->draw(_impl->txVs);
    _impl->win->draw(_impl->txPType[1]);
//    win->popGLStates();
    _impl->win->draw(_impl->txMsg);
    _impl->win->draw(_impl->txPAlg[0]);
    _impl->win->draw(_impl->txPAlg[1]);
    _impl->win->draw(_impl->txFPS);
    _impl->win->display();
}

void GameView::setFullscreen(bool enabled)
{
    _impl->win->close();
    if (enabled) _impl->win->create(sf::VideoMode::getFullscreenModes()[0],
                             "Connect Four 2014", sf::Style::Fullscreen);
    else _impl->win->create(sf::VideoMode(840, 600), "Connect Four 2014");
    _impl->update_geometry();
    render();
    _impl->fullscreen = enabled;
}

bool GameView::isFullscreen() const { return _impl->fullscreen; }

void GameView::Impl::mark_pos(int row, int col, int pl)
{
    auto& t = column[col][row];
	t.setOutlineThickness(3);
	t.setOutlineColor(sf::Color::White);
    t.setFillColor(player[pl].locolor);
}

void GameView::setFpsString(double fps)
{
    char buf[128];
    buf[sizeof(buf)-1] = 0;
    snprintf(buf, sizeof(buf)-1, "%5.2f", fps);
    _impl->txFPS.setString( buf );
    // which one is faster? :)
//    std::ostringstream os;
//    os << fps;
//    _impl->txFPS.setString( os.str() );
}

sf::RenderWindow* GameView::getWindow() const { return _impl->win.get(); }
