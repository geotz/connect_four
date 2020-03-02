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

#include <sstream>

#include "game.h"
#include "gameview.h"
#include "audio.h"
#include "connect4.hpp"

#include "alphabeta.hpp"
#include "mcts.hpp"

Game::Game(ViewBase& view, AudioBase& audio):
    think_algo(1),
    _view(view),
    _audio(audio),
    _msg(" ")
{
    acRestart();
}

State Game::state() const
{
    if (_move) return _history[_move-1];
    return State();
}

void Game::acRestart()
{
    _demo[0] = false; _demo[1] = true;
    _move = 0; _max_move = 0;
    _depth = 10;
    _audio.play(AudioBase::RESTART);
    _view.update(this);
}

State Game::alphabeta_think()
{
    static constexpr int MIN_MOVES = 2000000;
    int moves = 0;
    State s = state();
    State q;
    using Policy = DefaultPolicy<State>;
//    using Policy = NoPolicy<State>;
    State::score_type val = alpha_beta<State,Policy>(s, State::MINUS_INFINITY, State::PLUS_INFINITY, true,
                                       s.next_player(), _depth, 0, &q, &moves);
    std::stringstream ss;
    ss << "AB(" << s.next_player() << ")" << std::endl
       << "moves = " << moves << std::endl
       << "depth = " << _depth << std::endl << "score = " << val;
    _msg = ss.str();
    _view.update(this);
    if (moves < MIN_MOVES) ++_depth;
    return q;
}

State Game::mcts_think()
{
    static constexpr int NUM_SAMPLES = 10000;
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
    _msg = ss.str();
    _view.update(this);
    return q;
}

bool Game::acPlay(int where)
{
    State s = state();
    if (s.is_terminal()) return false;
    if (_demo[s.next_player()]) { // computer play
        State q;
        if (s == State()) q = s.make_move(3, s.next_player()); // center heuristic
        else q = (think_algo & (1 << s.next_player())) ? mcts_think() : alphabeta_think();
        auto sampleIndex = static_cast<AudioBase::e_sample>(s.column_height(q.last_column())+1);
        _audio.play(sampleIndex);
        _history[_move++] = q;
        _max_move = _move;
        if (q.winner() == q.last_player() && !_demo[q.next_player()])
            _audio.play(AudioBase::LOSER);
        _view.update(this);
        return true;
    } else if (s.column_height(where) < 6) { // human play
        auto sampleIndex = static_cast<AudioBase::e_sample>(s.column_height(where)+1);
        _audio.play(sampleIndex);
        _history[_move++] = s.make_move(where, s.next_player());
        _max_move = _move;
        State q = state();
        if (q.winner() == q.last_player() && _demo[q.next_player()])
            _audio.play(AudioBase::WINNER);
//        std::cerr << "heuristic = " << q() << std::endl;
        _view.update(this);
        return true;
    }
    _audio.play(AudioBase::ERROR);
    return false;
}

bool Game::acPlay(int row, int col)
{
//    State s = state();
    if (col >= 0) return acPlay(col); // && s.column_height(col) == row
    _audio.play(AudioBase::ERROR);
    return false;
}

void Game::acCompute()
{
    State s = state();
    if (s.is_terminal()) return;
    _demo[s.next_player()] = true;
    _demo[s.last_player()] = false;
    _view.update(this);
//        ac_play(); // called from outside, in main loop
}

void Game::acToggleAlgo()
{
    think_algo = (think_algo + 1) & 3;
    _view.update(this);
}

bool Game::acTakeback()
{
    if (_move) {
        --_move;
        int p = state().next_player();
        if (_demo[p]) _demo[p] = false, _demo[!p] = true;
        _audio.play(AudioBase::WARNING);
        _view.update(this);
        return true;
    }
    _audio.play(AudioBase::ERROR);
    return false;
}

void Game::ac2Player()
{
    _demo[0] = _demo[1] = false;
    _view.update(this);
}

void Game::acDemo()
{
    _demo[0] = _demo[1] = true;
    _view.update(this);
}

bool Game::acForward()
{
    if (_move < _max_move) {
        ++_move;
        int p = state().next_player();
        if (_demo[p]) _demo[p] = false, _demo[!p] = true;
        _audio.play(AudioBase::WARNING);
        _view.update(this);
        return true;
    }
    _audio.play(AudioBase::ERROR);
    return false;
}

void Game::acToggleSound()
{
    _audio.toggle();
    _audio.play(AudioBase::INFO);
}

void Game::acToggleFullscreen()
{
    _view.toggleFullscreen();
}

void Game::render() const
{
    _view.render();
}
