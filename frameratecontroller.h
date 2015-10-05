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

// frame rate controller (interface)

#include<chrono>

#ifndef _FRAMERATECONTROLLER_H_
#define _FRAMERATECONTROLLER_H_

class FrameRateController
{
    static constexpr double UNIT { 1.0 }; // 1 second
    static constexpr int DEF_FPS { 50 };
    static constexpr double DEF_REL_WIN { 0.05 };
public:
    FrameRateController(int fps = DEF_FPS, double rwin = DEF_REL_WIN);

    int fps() const { return _fps; }
    double rwin() const { return _rwin; }

    void set_fps(int fps);
    void set_rwin(double rwin);

    void operator()();

private:
    typedef std::chrono::steady_clock steady_clock;
    typedef std::chrono::duration<double> dur_t;

    double _rwin;
    steady_clock::time_point _t0;
    int _fps;
    dur_t _sleep;
    int _count;
};

#endif // FRAMERATEADJUSTER_H
