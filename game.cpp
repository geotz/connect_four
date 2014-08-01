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

// game model implementation

#include"game.h"

Game::Game(ViewBase* view, AudioBase* audio): move(0), max_move(0), view(view), audio(audio)
{
    demo[0] = false; demo[1] = true;
    audio->play(AudioBase::RESTART);
    view->update(this);
}

State Game::state() const
{
    if (move) return history[move-1];
    return State();
}

void Game::ac_restart()
{
    move = 0; max_move = 0;
    audio->play(AudioBase::RESTART);
    demo[0] = false; demo[1] = true;
    view->update(this);
}

bool Game::ac_play(int where)
{
    State s = state();
    if (s.is_terminal()) return false;
    if (demo[s.next_player()]) { // computer play
        State q;
        if (s.next_player()) {
            q = mcts_analyze(s, 25000, s.next_player());
        } else {
            q = mcts_analyze(s, 50000, s.next_player());
            //                alpha_beta(s, State::MINUS_INFINITY, State::PLUS_INFINITY, true, false,
            //                           6, 0, &q);
        }
        audio->play(s.column_height(q.last_column())+1);
        history[move++] = q;
        max_move = move;
        if (q.winner() == q.last_player() && !demo[q.next_player()])
            audio->play(AudioBase::LOSER);
        view->update(this);
        return true;
    } else if (s.column_height(where) < 6) { // human play
        audio->play(s.column_height(where)+1);
        history[move++] = s.make_move(where, s.next_player());
        max_move = move;
        State q = state();
        if (q.winner() == q.last_player() && demo[q.next_player()])
            audio->play(AudioBase::WINNER);
        view->update(this);
        return true;
    }
    audio->play(AudioBase::ERROR);
    return false;
}

bool Game::ac_play(int row, int col)
{
    State s = state();
    if (col >= 0) return ac_play(col); // && s.column_height(col) == row
    audio->play(AudioBase::ERROR);
    return false;
}

void Game::ac_compute()
{
    State s = state();
    if (s.is_terminal()) return;
    demo[s.next_player()] = true;
    demo[s.last_player()] = false;
    view->update(this);
//        ac_play(); // called from outside, in main loop
}

bool Game::ac_takeback()
{
    if (move) {
        --move;
        int p = state().next_player();
        if (demo[p]) demo[p] = false, demo[!p] = true;
        audio->play(AudioBase::WARNING);
        view->update(this);
        return true;
    }
    audio->play(AudioBase::ERROR);
    return false;
}

void Game::ac_2player()
{
    demo[0] = demo[1] = false;
    view->update(this);
}

void Game::ac_demo()
{
    demo[0] = demo[1] = true;
    view->update(this);
}

bool Game::ac_forward()
{
    if (move < max_move) {
        ++move;
        int p = state().next_player();
        if (demo[p]) demo[p] = false, demo[!p] = true;
        audio->play(AudioBase::WARNING);
        view->update(this);
        return true;
    }
    audio->play(AudioBase::ERROR);
    return false;
}

void Game::ac_toggle_sound()
{
    audio->toggle();
    audio->play(AudioBase::INFO);
}

void Game::ac_toggle_fullscreen()
{
    view->toggleFullscreen();
}
