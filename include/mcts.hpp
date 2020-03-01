/*
    Monte-Carlo Tree Search (c) 2014 George M. Tzoumas

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

#ifndef _MCTS_H_
#define _MCTS_H_

// Monte-Carlo Tree Search

//#include<iostream>
#include<algorithm>
#include<array>
#include<iterator>
#include<unordered_map>
#include<cmath>
#include<tuple>

template<class T,size_t N>
class small_vector
{
    using Rep = std::array<T,N>;

    size_t _size;
    Rep _rep;
public:
    using value_type = T;
    using pointer    = T*;

    small_vector(): _size(0) {}

    template<class U>
    void push_back(U&& x)
    {
        _rep[_size++] = std::forward<U>(x);
    }

    void push_back() { _rep[_size++] = T(); }

    small_vector(const small_vector&) = default;

    small_vector(small_vector&& v):
        _rep(std::move(v._rep)),
        _size(std::exchange(v._size,0))
    {}

    small_vector& operator=(const small_vector&) = default;
    small_vector& operator=(small_vector&& v)
    {
        _rep = std::move(v._rep);
        _size = std::exchange(v._size, 0);
    }

    const T& operator[](size_t i) const { return _rep[i]; }
    T& operator[](size_t i) { return _rep[i]; }

    auto begin() const { return _rep.begin(); }
    auto end() const { return _rep.begin() + _size; }

    const T& front() const  { return _rep.front(); }
    T& front()              { return _rep.front(); }

    const T& back() const  { return _rep[_size-1]; }
    T& back()              { return _rep[_size-1]; }

    size_t size() const { return _size; }
    bool   empty() const { return _size == 0; }
};

inline void hash_combine(size_t & seed, const size_t& hv)
{
    seed ^= hv + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

namespace mcts
{

// stats for player 0
struct NodeData
{
    int   nwins     = 0;
//    int   ndraws    = 0;
    int   nlosses   = 0;
    int   nsamples  = 0;
    double operator()() const
    {
        return double(nwins-nlosses) / nsamples;
        // ndraws is eliminated
    }
};

std::ostream& operator<<(std::ostream& s, const NodeData& x)
{
    s << "NodeData{" << x.nwins << ',' << x.nlosses << ',' << x.nsamples << "}(" << x() << ")\n";
    return s;
}

inline double ucb( const NodeData& node, const NodeData& parent )
{
    static constexpr double C = M_SQRT2;
    return node() +
            C * std::sqrt(std::log2(parent.nsamples) / node.nsamples );
}

template<class State>
struct StateDepth
{
    State state  = State();
    size_t depth = 0;

    struct Hasher
    {
        size_t operator()(const StateDepth& x) const
        {
            size_t seed = 0;
            hash_combine(seed, typename State::Hasher{}(x.state));
            hash_combine(seed, x.depth);
            return seed;
        }
    };
};

template<class State>
bool operator==(const StateDepth<State>& a, const StateDepth<State>& b)
{
    return std::tie(a.state, a.depth) == std::tie(b.state, b.depth);
}

template<class State,size_t MAXD>
class Tree
{
public:
    using Level = std::unordered_map<StateDepth<State>,NodeData,typename StateDepth<State>::Hasher>;
    small_vector<Level,MAXD> levels;

    struct NodeInfo
    {
        State state;
        size_t depth;
        NodeData data;
        NodeInfo():
            state(State{}), depth(0), data(NodeData{})
        {}

        NodeInfo( typename Level::const_iterator it )
        {
            state = it->first.state;
            depth = it->first.depth;
            data = it->second;
        }
    };
    Tree(const State& s = State{})
    {
        levels.push_back();
        levels[0][StateDepth<State>{s,0}] = NodeData();
    }
};

template<class State,size_t MAXD>
std::ostream& operator << (std::ostream& s, const Tree<State,MAXD>& t)
{
    s << "-- TREE BEGIN --\n";
    for (size_t i = 0; i < t.levels.size(); ++i)
    {
        s << "LEVEL(" << i << ")\n";
        for (auto& x: t.levels[i])
        {
            x.first.state.dump(s);
            s << "DEPTH = " << x.first.depth << " INFO = " << x.second << '\n';
        }
    }
    s << "-- TREE END --\n";
    return s;
}

// 1) SELECT

template<size_t BF,size_t MAXD,class State>
StateDepth<State> select( const Tree<State,MAXD>& tree )
{
    using Info = typename Tree<State,MAXD>::NodeInfo;
    Info sel( tree.levels[0].begin() );

    while (true)
    {
        auto it = sel.state.children();
        ++sel.depth;
        if (sel.depth == tree.levels.size())
        {
            break;
        }
        auto& nextLevel = tree.levels[sel.depth];
        small_vector<Info,BF> next;
        while (it.hasNext())
        {
            auto tmp = it.next();
            auto found = nextLevel.find({tmp,sel.depth});
            if (found != nextLevel.end())
            {
                next.push_back( Info(found) );
            }
        }
        if (next.empty())
        {
            break;
        }
        std::cerr << "MC candidates: " << next.size() << " next level = " << sel.depth << std::endl;
        auto smax = std::max_element( next.begin(), next.end(),
                                     [&sel](const auto& a, const auto& b) {
                                        return ucb(a.data, sel.data) < ucb(b.data, sel.data); } );
        sel = *smax;
    }
    return {sel.state,sel.depth};
}

// 2) EXPAND

template<size_t MAXD,class State>
State expand( Tree<State,MAXD>& tree, const StateDepth<State>& aSel )
{
    if (!aSel.state.is_terminal())
    {
        auto it = aSel.state.children();
//        auto nextLevel = aSel.depth; // assert == level ?
        tree.levels.push_back();
        if (!it.hasNext())
        {
            return aSel.state;
        }
        State last;
        do
        {
            last = it.next();
            tree.levels.back()[StateDepth<State>{last,aSel.depth}] = NodeData();
        } while (it.hasNext());
        return last;
    }
    return aSel.state;
}

// 3) SIMULATE
template<class State>
NodeData simulate(const State& s, int num_samples)
{
    int w = s.winner();
    if (w == 2) return {0,0,1};
    if (w == 0) return {1,0,1};
    if (w == 1) return {0,1,1};

    int nw[3] = {0};
    for (int i = 0; i < num_samples; ++i)
    {
        State r = s;
        while (!r.is_terminal())
        {
            r = r.random_move();
            //            r.dump(std::cerr);
        }
        ++nw[r.winner()];
        //        std::cerr << i << std::endl;
    }
    return {nw[0],nw[1],num_samples};
}

template<size_t BF,class State>
State naive_analyze(State s, int num_samples, bool second_player = false, typename State::score_type *score = 0)
{
    using score_type = typename State::score_type;
    auto it = s.children();
    std::array<State,BF> vs;
    std::array<score_type,BF> vp;
    size_t count = 0;
    while (it.hasNext())
    {
        State r = it.next();
        vs[count] = r;
        vp[count] = simulate(r,num_samples)();
        ++count;
        //        std::cerr << r.last_column() << ":(" << p.p0 << "," << p.p1 << "," << p.px << ")" << std::endl;
    }
    //    std::cerr << std::endl;
//    std::copy(vp.begin(), vp.end(), std::ostream_iterator<score_type>(std::cerr, " "));
//    std::cerr << std::endl;
    if (!second_player)
    {
        auto mit = std::max_element(vp.begin(), vp.begin()+count);
        if (score) *score = *mit;
//        std::cerr << "score = " << *mit << std::endl;
        return vs[std::distance(vp.begin(), mit)];
    }
    else
    {
        auto mit = std::min_element(vp.begin(), vp.begin()+count);
        if (score) *score = -(*mit);
//        std::cerr << "score = " <<- (*mit) << std::endl;
        return vs[std::distance(vp.begin(), mit)];
    }
}

}

#endif // _MCTS_H_
