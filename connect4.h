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

// connect-4 game state (engine)

#ifndef _CONNECT4_H_
#define _CONNECT4_H_

#include<ostream>
#include<algorithm>
#include<strings.h>
#include<mtcs.h>

#ifdef __linux
inline int fls(int k) {
	if (k == 0) return 0;
	return (sizeof(k) << 3) - __builtin_clz(k);
	//int c = 0;
	//while (k) k >>= 1, c++;
	//return c;
}
#endif

class State {
    union {
        unsigned char column[8];
        long long rep;
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
    class iterator {
        int i;
        const State *ref;
    public:
        iterator(const State &s): i(0), ref(&s) { }
        
        bool hasNext() {
            while (i < 7) {
                if (ref->column_height(i) < 6) return true;
                else ++i;
            }
            return false;
        }
        
        State next() {
            State s = ref->make_move(i,ref->next_player());
            ++i;
            return s;
        }
    };
    
    enum { MINUS_INFINITY = -(1<<30), PLUS_INFINITY = 1<<30, MAX_DEPTH = 42 };
    typedef double score_type;
    
    State() { std::fill(column, column+7, 1); column[7] = 0x0F; }
    
    State make_move(int col, int who) const {
        State ret(*this);
        ret.drop(col,who);
        return ret;
    }
        
    State random_move() const {
        char ci[7];
        int nc = 0;
        for (int i = 0; i < 7; i++)
            if (column_height(i) < 6) ci[nc++] = i;
        return make_move(ci[rand() % nc], next_player());
    }
    
    int last_player() const { return column[7] >> 3; }
    
    int next_player() const { return !last_player(); }
    
    void dump(std::ostream& s) {
        for (int i = 5; i >= 0; i--) {
            for (int j = 0; j < 7; j++)
                s << symbol(get(i,j)) << "  ";
            s << std::endl;
        }
        s << "0  1  2  3  4  5  6" << std::endl;
//        for (int i = 0; i < 7; i++)
//            s << column_height(i) << " ";
        s << " last = " << last_column() << " winner = " << winner() << std::endl;
    }

    bool is_terminal() const { return winner() < 3; }
    
    // bits: DDCCCRRRWW
    // W: 3 = non-final state, 0 = X, 1 = O, 2 = draw
    // R: row
    // C: column
    // D: direction 0 = line LtRt, 1 = column DnUp, 2 = diag UR, 3 = diag UL
    int winner_info() const {
        int k;
        
        for (int i = 0; i < 6; i++) for (int j = 0; j < 4; j++)
            if ((k = is_line(i,j)) >= 0) return (j<<5) | (i<<2) | k;
        
        for (int j = 0; j < 7; j++) for (int i = 0; i < column_height(j)-3; i++)
            if ((k = is_column(j,i)) >= 0) return 0x100 | (j<<5) | (i<<2) | k;
        
        for (int i = 0; i < 3; i++) for (int j = 0; j < 4; j++)
            if ((k = is_diag_ur(i,j)) >= 0) return 0x200 | (j<<5) | (i<<2) | k;
        
        for (int i = 0; i < 3; i++) for (int j = 3; j < 7; j++)
            if ((k = is_diag_ul(i,j)) >= 0) return 0x300 | (j<<5) | (i<<2) | k;
        
        if (is_full()) return 2;
        return 3;
    }
    
    int winner() const { return winner_info() & 3; }
    
    // evaluates ALWAYS for player 0 -- should negate result manually
    score_type operator()() const {
        int w = winner();
        if (w == 3){
            return mcts(*this, 100);
        }
        if (w == 2) return 0;
//        if (w == -1) return (9.0-(last_column()-3)*(last_column()-3))/3.0;
        if (w == 0) return 1000; else return -1000;
    }
    
    int column_height(int col) const { return fls(column[col])-1; }
    int last_column() const { return column[7] & 0x07; }
    
    bool operator==(const State &other) const {
        return rep == other.rep;
    }
    bool operator<(const State &other) const {
        return rep < other.rep;
    }
    long long hash_value() const { return rep; }
    
    int empty_space() const {
        return 42 - column_height(0) - column_height(1) - column_height(2) -
        column_height(3) - column_height(4) - column_height(5) -
        column_height(6); 
    }
    
    int get(int row, int col) const {
        if (row >= column_height(col)) return -1;
        return (column[col] & (1 << row)) > 0;
    }
    
    bool is_empty() const {
        return column_height(0) == 0 && column_height(1) == 0 &&
        column_height(2) == 0 && column_height(3) == 0 &&
        column_height(4) == 0 && column_height(5) == 0 &&
        column_height(6) == 0;
    }

    bool is_full() const {
        return column_height(0) == 6 && column_height(1) == 6 &&
        column_height(2) == 6 && column_height(3) == 6 &&
        column_height(4) == 6 && column_height(5) == 6 &&
        column_height(6) == 6;
    }
    
private:
    
    void drop(int col, int who) {
        int h = column_height(col);
        if (h >= 6) return;
        column[col] &= (1 << h) - 1;
        column[col] |= (2+who) << h; 
        column[7] = (who << 3) | col;
    }
    
    static char symbol(int k) {
        if (k == 0) return 'X';
        if (k == 1) return 'O';
        return '.';
    }
    
    
    // -1 = no line, 0 = X, 1 = O
    int is_line(int k, int start) const {
        if (get(k,start) == get(k,start+1) && get(k,start+1) == get(k,start+2) && 
            get(k,start+2) == get(k,start+3)) return get(k,start);
        else return -1;
    }
    
    int get_column(int col) const { return column[col] & ((1 << column_height(col)) - 1); }
    int is_column(int k,int start) const {
        if (start+4 > column_height(k)) return -1;
        int c = get_column(k);
        c = (c >> start) & 0x0F;
        if (c == 0) return 0;
        if (c == 0x0F) return 1;
        return -1;
    }
    int is_diag_ur(int r, int c) const {
        if (get(r,c) == get(r+1,c+1) && get(r+1,c+1) == get(r+2,c+2) && 
            get(r+2,c+2) == get(r+3,c+3)) return get(r,c);
        else return -1;
    }
    int is_diag_ul(int r, int c) const {
        if (get(r,c) == get(r+1,c-1) && get(r+1,c-1) == get(r+2,c-2) && 
            get(r+2,c-2) == get(r+3,c-3)) return get(r,c);
        else return -1;
    }
};
                      
#endif
