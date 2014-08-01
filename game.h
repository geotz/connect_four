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

// game model (interface)

#ifndef _GAME_H_
#define _GAME_H_

#include "connect4.h"
//#include <alphabeta.h>
#include "gameview.h"
#include "audio.h"

class Game {
    int move;
    int max_move;
    State history[42];
    bool demo[2];
    ViewBase *view;
    AudioBase *audio;
public:
    Game(ViewBase* view, AudioBase* audio);

    bool is_demo(int p) const { return demo[p]; }
    State state() const;

    void ac_restart();
    bool ac_play(int where = 0);
    bool ac_play(int row, int col);
    void ac_compute();
    bool ac_takeback();
    void ac_2player();
    void ac_demo();
    bool ac_forward();
    void ac_toggle_sound();
    void ac_toggle_fullscreen();
    void render() const { view->render(); }
};

#endif
