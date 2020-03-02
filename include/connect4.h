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

// connect-4 game state (engine)

#ifndef _CONNECT4_H_
#define _CONNECT4_H_

#include <iosfwd>
//#include"mcts.h"


class State {
    union {
        unsigned char column[8];
        size_t rep;
    };
    // column bits: 7654|3210
    //              000H.....
    // H = bit pos denoting column height
    // ... = 0 or 1 denoting column contents
    // column[7] bits: 7654|3210
    //                      PMMM
    // MMM = 0..6 last column played, 7 = NONE
    // P = last player played (0 or 1)
public:
    struct Hasher {
        size_t operator()(State s) const;
    };

    class iterator;

    enum { MINUS_INFINITY = -(1<<30),
           PLUS_INFINITY = 1<<30,
           MAX_DEPTH = 42 };

    using score_type = double ;
    
    State();

    iterator children() const;
    State make_move(int col, int who) const;
    State random_move() const;
    State symmetric() const;
    
    int last_player() const;
    int next_player() const;
    
    void dump(std::ostream& s) const;

    bool is_terminal() const;
    int winner_info() const;
    int winner() const;
        
    // evaluates ALWAYS for player 0 -- should negate result manually
    score_type operator()() const;
    
    int column_height(int col) const;
    int last_column() const;
    
    static_assert( sizeof(size_t) == 8 );
    size_t hash_value() const;
    
    State up() const;
    int empty_space() const;
    int get(int row, int col) const;
    bool is_empty() const;
    bool is_full() const;
    
private:
    static constexpr size_t BOARD_MASK = 0x00FFFFFFFFFFFFFF;

    void drop(int col, int who);
    bool undrop();

    static char symbol(int k);
    
    int is_line(int k, int start) const;
    
    int get_column(int col) const;
    int is_column(int k,int start) const;
    int is_diag_ur(int r, int c) const;
    int is_diag_ul(int r, int c) const;

    int line_heuristic(int y1, int x1, int dy, int dx) const;
    int heuristic_value() const;

};

bool operator==(State a, State b);
bool operator<(State a, State b);

#endif
