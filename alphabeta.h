/*
    Alpha-Beta Tree Search (c) 2014 George M. Tzoumas

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

#ifndef _ALPHABETA_H_
#define _ALPHABETA_H_

// Alpha-Beta Tree Search

//#include<iostream>

//class AlphaBetaDebug;

template<class State>
typename State::score_type alpha_beta(  State s, typename State::score_type alpha, typename State::score_type beta,
                                        bool max, bool second_player, int max_depth, int cur_depth = 0, State *best = 0, int *moves = 0)
{
//    AlphaBetaDebug foo(max,cur_depth);
    if (cur_depth == max_depth || s.is_terminal())
    {
        //typename State::score_type score = s()*(3.0/(3.0+cur_depth));
        auto score = s();
        return second_player ? -score : score;
    }
    State sa, sb;
    typename State::iterator it(s);
    while (it.hasNext())
    {
        State r = it.next();
        if (moves)
        {
            ++(*moves);
//            if (((*moves) & ((1<<20)-1)) == 0) std::cerr << (*moves) << std::endl;
        }
//        r.dump(std::cerr);
        auto val = alpha_beta<State>(r, alpha, beta, !max, second_player,
                                                          max_depth, cur_depth+1, 0, moves);
//        std::cerr << val << std::endl;
        if (max)
        {
            if (val > alpha) alpha = val, sa = r;
            if (beta <= alpha)
            {
                if (best) *best = sa;
                return alpha;
            }
        }
        else
        {
            if (val < beta) beta = val, sb = r;
            if (beta <= alpha)
            {
                if (best) *best = sb;
                return beta;
            }
        }
    }
    if (best) *best = max ? sa : sb;
    return max ? alpha: beta;
}

/*
class AlphaBetaDebug {
    bool max;
    int depth;
public:
    AlphaBetaDebug(bool max, int depth): max(max), depth(depth) {
        std::cerr << "enter AB: max = " << max << " depth = " << depth << std::endl;
    }
    ~AlphaBetaDebug() {  
        std::cerr << "exit AB: max = " << max << " depth = " << depth << std::endl;
    }
};
*/

#endif // _ALPHABETA_H_
