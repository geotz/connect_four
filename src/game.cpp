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

// game model implementation

#include<sstream>

#include"game.h"
#include"alphabeta.hpp"
#include"mcts.hpp"

Game::Game(ViewBase* view, AudioBase* audio):
    view(view), audio(audio), msg(" "), think_algo(1)
{
    ac_restart();
}

State Game::state() const
{
    if (move) return history[move-1];
    return State();
}

void Game::ac_restart()
{
    demo[0] = false; demo[1] = true;
    move = 0; max_move = 0;
    depth = 10;
    audio->play(AudioBase::RESTART);
    view->update(this);
}

State Game::alphabeta_think()
{
    static const int MIN_MOVES = 2000000;
    int moves = 0;
    State s = state();
    State q;
    using Policy = DefaultPolicy<State>;
//    using Policy = NoPolicy<State>;
    State::score_type val = alpha_beta<State,Policy>(s, State::MINUS_INFINITY, State::PLUS_INFINITY, true,
                                       s.next_player(), depth, 0, &q, &moves);
    std::stringstream ss;
    ss << "AB(" << s.next_player() << ")" << std::endl
       << "moves = " << moves << std::endl
       << "depth = " << depth << std::endl << "score = " << val;
    msg = ss.str();
    view->update(this);
    if (moves < MIN_MOVES) ++depth;
    return q;
}

State Game::mcts_think()
{
    static const int NUM_SAMPLES = 10000;
    State::score_type val;
    State s = state();
    auto t0 = std::chrono::steady_clock::now();

    mcts::Tree<State,42> tree(s);

    State q = mcts::naive_analyze<7>(s, NUM_SAMPLES, s.next_player(), &val);

    auto sel = mcts::select<7>( tree );
    sel.state.dump( std::cerr );
    auto sel2 = mcts::expand( tree, sel );
    sel2.dump( std::cerr );
    std::cerr << tree << '\n';
    auto sim = mcts::simulate(sel2, 1000);
    std::cerr << "SIMULATION = " << sim << std::endl;

    auto t1 = std::chrono::steady_clock::now();
    std::chrono::duration<double> dur = t1-t0;
    std::stringstream ss;
    ss << "MC(" << s.next_player() << ") " << dur.count() << 's' << std::endl
       << "samples = " << NUM_SAMPLES << std::endl << "score = " << val;
    msg = ss.str();
    view->update(this);
    return q;
}

bool Game::ac_play(int where)
{
    State s = state();
    if (s.is_terminal()) return false;
    if (demo[s.next_player()]) { // computer play
        State q;
        if (s == State()) q = s.make_move(3, s.next_player()); // center heuristic
        else q = (think_algo & (1 << s.next_player())) ? mcts_think() : alphabeta_think();
        auto sampleIndex = static_cast<AudioBase::e_sample>(s.column_height(q.last_column())+1);
        audio->play(sampleIndex);
        history[move++] = q;
        max_move = move;
        if (q.winner() == q.last_player() && !demo[q.next_player()])
            audio->play(AudioBase::LOSER);
        view->update(this);
        return true;
    } else if (s.column_height(where) < 6) { // human play
        auto sampleIndex = static_cast<AudioBase::e_sample>(s.column_height(where)+1);
        audio->play(sampleIndex);
        history[move++] = s.make_move(where, s.next_player());
        max_move = move;
        State q = state();
        if (q.winner() == q.last_player() && demo[q.next_player()])
            audio->play(AudioBase::WINNER);
//        std::cerr << "heuristic = " << q() << std::endl;
        view->update(this);
        return true;
    }
    audio->play(AudioBase::ERROR);
    return false;
}

bool Game::ac_play(int row, int col)
{
//    State s = state();
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

void Game::ac_toggle_algo()
{
    think_algo = (think_algo + 1) & 3;
    view->update(this);
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
