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

// game view (implementation)

#include "gameview.h"
#include "game.h"
#include<cmath>

GameView::GameView(bool fullscreen): _fullscreen(fullscreen), column(7)
{
    if (_fullscreen) {
        win = new sf::RenderWindow(sf::VideoMode::getFullscreenModes()[0],
                                  "Connect Four 2014", sf::Style::Fullscreen);
    } else {
        win = new sf::RenderWindow(sf::VideoMode(840, 600), "Connect Four 2014");
    }
    win->setVerticalSyncEnabled(true);

    font.loadFromFile("res/JLSSpaceGothicR_NC.otf");
    player[0] = Player(0);
    player[1] = Player(1);

    rect.setFillColor(sf::Color::Blue);
    rect.setOutlineColor(sf::Color::Cyan);
    rect.setOutlineThickness(5);

    for (int i = 0; i < 7; i++) {
        column[i].reserve(6);
        for (int j = 0; j < 6; j++) {
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
        txPType[i].setColor(player[i].hicolor);
    }

    txVs.setFont(font);
    txVs.setCharacterSize(20);
    txVs.setStyle(sf::Text::Regular);
    txVs.setString(std::string("vs"));

    txHelp.setFont(font);
    txHelp.setCharacterSize(20);
    txHelp.setStyle(sf::Text::Regular);
    txHelp.setString("Connect Four 2014\n"
                     "   (c) 2014\n"
                     "George M. Tzoumas\n\n"

                     "1-7 ... Column to play\n"
                     "   $ ... Restart\n"
                     "  @ ... 2-player (human)\n"
                     "   C ... Compute\n"
                     "   D ... Demo\n"
                     "   F ... Forward\n"
                     "   T ... Takeback\n\n"

                     "  Q or Esc\n"
                     "      ... Quit\n\n"

                     "    S ...  Sound on/off\n\n"

                     "  Sys-F or Alt-Enter\n"
                     "      ... Toggle fullscreen\n"
                     );

    for (int i = 0; i < 7; i++) {
        txColNum[i].setFont(font);
        txColNum[i].setCharacterSize(24);
        txColNum[i].setStyle(sf::Text::Regular);
        txColNum[i].setColor(sf::Color::Cyan);
        txColNum[i].setString(std::string(1,'1'+i));
    }

    backgroundTexture.loadFromFile("res/galaxy.jpg");
    background.setTexture(backgroundTexture);

    updateGeometry();
}

void GameView::updateGeometry()
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
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 6; j++) {
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

//    bool demo[2] = {false, true};
//    updateState(State(),demo);

    float dx = (column[1][0].getPosition().x-column[0][0].getPosition().x)/2;
    for (int i = 0; i < 7; i++) {
        sf::Vector2f v = column[i][0].getPosition();
        txColNum[i].setPosition(v.x+dx-12,size.y+45);
    }

    sf::Vector2f v1(win->getSize());
    sf::Vector2f v2(backgroundTexture.getSize());
    background.setScale(sf::Vector2f(v1.x/v2.x, v1.y/v2.y));
}

sf::Vector2i GameView::getGrid(sf::Vector2f pos)
{
    float dist = 1.0e6;
    int row = -1, col = -1;
    float dx = (column[1][0].getPosition().x-column[0][0].getPosition().x)/2;
    float rad = column[0][0].getRadius();
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 6; j++) {
            sf::Vector2f p = column[i][j].getPosition();
            sf::Vector2f center(p.x+dx,p.y+dx);
            float d = std::sqrt(std::pow(pos.x-center.x,2)+std::pow(pos.y-center.y,2));
            if (d <= rad && d < dist) dist = d, row = j, col = i;
        }
    }
    return sf::Vector2i(col,row);
}

void GameView::update(Game *g)
{
    State s = g->state();
    for (int i = 0; i < 2; i++) {
        txPType[i].setString(g->is_demo(i) ? player[i].scomp : player[i].shuman);
    }
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 6; j++) {
			sf::Color color = sf::Color::Black;
            int c = s.get(j,i);
			if (c >= 0) color = player[c].color;
//            else color = sf::Color::White;
			sf::CircleShape &t = column[i][j];
//            t.setTexture(&backgroundTexture);
			t.setFillColor(color);
			t.setOutlineThickness(2);
			t.setOutlineColor(sf::Color::Black);
		}
	}
    if (!s.is_empty()) {
        int l = s.last_column();
        int h = s.column_height(l);
        mark_pos(s.column_height(l)-1, s.last_column(), s.last_player());
	}
    if (s.is_terminal()) {
        int p = s.last_player();
        int w = s.winner_info();
		if (w == 2) txInfo.setString("DRAW");
		else txInfo.setString(player[p].swin);
        if ((w & 3) < 2) txInfo.setColor(player[p].color);
        else txInfo.setColor(sf::Color::White);
        if ((w & 3) < 2) {
			int dir = w >> 8;
			int row = (w >> 2) & 7;
			int col = (w >> 5) & 7;
			if (dir == 0) {
				for (int i = 0; i < 4; i++) mark_pos(row, col+i, p);
			} else if (dir == 1) {
				for (int i = 0; i < 4; i++) mark_pos(row+i, col, p);
			} else if (dir == 2) {
				for (int i = 0; i < 4; i++) mark_pos(row+i, col+i, p);
			} else if (dir == 3) {
				for (int i = 0; i < 4; i++) mark_pos(row+i, col-i, p);
			}
		}
	} else {
        int p = s.next_player();
        if (g->is_demo(p)) txInfo.setString(player[p].sthink);
        else txInfo.setString(player[p].splay);
		txInfo.setColor(player[p].hicolor);
	}	
}
    
void GameView::render() const 
{
    win->clear();
    //    win->pushGLStates();
    win->draw(background);
    win->draw(rect);
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 6; j++) {
			win->draw(column[i][j]);
		}
		win->draw(txColNum[i]);
	}
    win->draw(txInfo);
    win->draw(txHelp);
    win->draw(txPType[0]);
    win->draw(txVs);
    win->draw(txPType[1]);
//    win->popGLStates();
    win->display();
}

void GameView::setFullscreen(bool enabled)
{
    win->close();
    if (enabled) win->create(sf::VideoMode::getFullscreenModes()[7],
                             "Connect Four 2014", sf::Style::Fullscreen);
    else win->create(sf::VideoMode(840, 600), "Connect Four 2014");
    updateGeometry();
    render();
    _fullscreen = enabled;
}

GameView::Player::Player(int id): id(id) 
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

void GameView::mark_pos(int row, int col, int pl) 
{
	sf::CircleShape &t = column[col][row];
	t.setOutlineThickness(3);
	t.setOutlineColor(sf::Color::White);
	t.setFillColor(player[pl].locolor);
}
