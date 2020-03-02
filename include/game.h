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

// game model (interface)

#ifndef _GAME_H_
#define _GAME_H_

#include "connect4.h"

#include <string>
#include <memory>

class ViewBase;
class AudioBase;

class Game
{
public:
    int think_algo; // 0..3, bit 1: player X, bit 0: 0=AB 1=MC

    Game(ViewBase& _view, AudioBase& _audio);

    bool is_demo(int p) const { return _demo[p]; }
    State state() const;
    const std::string& get_msg() const { return _msg; }

    void acRestart();
    bool acPlay(int where = 0);
    bool acPlay(int row, int col);
    void acCompute();
    void acToggleAlgo();
    bool acTakeback();
    void ac2Player();
    void acDemo();
    bool acForward();
    void acToggleSound();
    void acToggleFullscreen();
    void render() const;

private:
    State alphabeta_think();
    State mcts_think();

    int         _move;
    int         _max_move;
    State       _history[42];
    bool        _demo[2];
    ViewBase&   _view;
    AudioBase&  _audio;
    int         _depth;
    std::string _msg; // algorithm stats
};

#endif
