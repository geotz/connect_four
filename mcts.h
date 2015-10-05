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
#include<vector>
#include<iterator>

template<class State>
typename State::score_type mcts(State s, int num_samples) 
{    
    int w = s.winner();
    if (w == 2) return 0;
    if (w == 0) return 1000;
    if (w == 1) return -1000;
    
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
    return (nw[0]-nw[1])*1000.0/num_samples; // nw[2] is eliminated
}

template<class State>
State mcts_analyze(State s, int num_samples, bool second_player = false, typename State::score_type *score = 0)
{
    typedef typename State::score_type score_type;
    typename State::iterator it(s);
    std::vector<State> vs;
    vs.reserve(7);
    std::vector<score_type> vp;
    vp.reserve(7);
    while (it.hasNext())
    {
        State r = it.next();
        vs.push_back(r);
        score_type p = mcts(r,num_samples);
        vp.push_back(p);
        //        std::cerr << r.last_column() << ":(" << p.p0 << "," << p.p1 << "," << p.px << ")" << std::endl;
    }
    //    std::cerr << std::endl;
//    std::copy(vp.begin(), vp.end(), std::ostream_iterator<score_type>(std::cerr, " "));
//    std::cerr << std::endl;
    typename std::vector<score_type>::iterator mit;
    if (!second_player)
    {
        mit = std::max_element(vp.begin(),vp.end());
        if (score) *score = *mit;
//        std::cerr << "score = " << *mit << std::endl;
    }
    else
    {
        mit = std::min_element(vp.begin(),vp.end());
        if (score) *score = -(*mit);
//        std::cerr << "score = " <<- (*mit) << std::endl;
    }
    return vs[std::distance(vp.begin(),mit)];
}

#endif // _MCTS_H_
