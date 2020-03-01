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

#include <iostream>

#include <unordered_map>
#include <utility>

//class AlphaBetaDebug;

template<class State>
struct NoPolicy
{
    using Score = typename State::score_type;
    void insert(State, Score, int) { }
    std::pair<Score,bool> lookup(State, int) { return std::make_pair(Score(),false); }
};

template<class State>
struct DefaultPolicy
{
    using Score = typename State::score_type;

    struct CacheEntry
    {
        Score score;
        int depth;
    };

    std::pair<Score,bool> lookup(State s, int depth) const {
        auto it = _cache.find(s);
        if (it == _cache.end())
        {
            State sym = s.symmetric();
            it = _cache.find(sym);
            if (it == _cache.end())
            {
                return std::make_pair(Score(), false);
            }
        }
//        if ((*it).second.depth != depth) return std::make_pair(Score(), false);
        if ((*it).second.depth > depth) return std::make_pair(Score(), false);
        return std::make_pair((*it).second.score, true);
    }

    void insert(State s, typename State::score_type score, int depth) { _cache[s] = CacheEntry{score, depth}; }

private:
    std::unordered_map<State,CacheEntry,typename State::Hasher> _cache;
};

template<class State, class CachingPolicy>
typename State::score_type alpha_beta_cache(State s, CachingPolicy &cache, typename State::score_type alpha, typename State::score_type beta,
                                        bool max, bool second_player, int max_depth, int cur_depth = 0, State *best = 0, int *moves = 0)
{
//    AlphaBetaDebug foo(max,cur_depth);
    auto res = cache.lookup(s, cur_depth);
    if (res.second) {
//        std::cerr << "cache hit for state! " << s.hash_value() << std::endl;
        return second_player ? -res.first : res.first;
    }
    if (cur_depth == max_depth || s.is_terminal())
    {
        //typename State::score_type score = s()*(3.0/(3.0+cur_depth));
        auto score = s();
        cache.insert(s, score, cur_depth);
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
        auto val = alpha_beta_cache<State>(r, cache, alpha, beta, !max, second_player,
                                                          max_depth, cur_depth+1, 0, moves);
//        std::cerr << val << std::endl;
        if (max)
        {
            if (val > alpha) alpha = val, sa = r;
            if (beta <= alpha)
            {
                if (best) *best = sa;
                cache.insert(r, second_player ? -alpha : alpha, cur_depth);
                return alpha;
            }
        }
        else
        {
            if (val < beta) beta = val, sb = r;
            if (beta <= alpha)
            {
                if (best) *best = sb;
                cache.insert(r, second_player ? -beta : beta, cur_depth);
                return beta;
            }
        }
    }
    if (best) *best = max ? sa : sb;
    auto rval = max ? alpha: beta;
    cache.insert(s, second_player ? -rval : rval, cur_depth);
    return rval;
}

template<class State, class CachingPolicy = NoPolicy<State> >
auto alpha_beta(  State s, typename State::score_type alpha, typename State::score_type beta,
                                        bool max, bool second_player, int max_depth, int cur_depth = 0, State *best = 0, int *moves = 0 )
{
    CachingPolicy tmp;
    return alpha_beta_cache( s, tmp, alpha, beta, max, second_player, max_depth, cur_depth, best, moves );
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
