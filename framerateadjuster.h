/*
    Connect Four 2014 (c) 2014-2015 George M. Tzoumas

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

// frame rate adjuster (interface)

#include<chrono>

#ifndef _FRAMERATEADJUSTER_H_
#define _FRAMERATEADJUSTER_H_

class FrameRateAdjuster
{
    static constexpr int UNIT { 1000000000 };
    static constexpr int DEF_FPS { 50 };
    static constexpr double DEF_REL_WIN { 0.05 };
public:
    FrameRateAdjuster(int fps = DEF_FPS, double rel_win = DEF_REL_WIN);

    int fps() const { return _fps; }
    double relWin() const { return _rwin; }

    void set_fps(int fps);
    void set_relWin(double rel_win);

    void operator()();

private:
    typedef std::chrono::steady_clock steady_clock;
    typedef std::chrono::nanoseconds ns_t;

    double _rwin;
    steady_clock::time_point _t0;
    int _fps;
    ns_t _sleep;
    int _count;
};

#endif // FRAMERATEADJUSTER_H
