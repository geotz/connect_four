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

#include <chrono>

#ifndef _FRAMERATECONTROLLER_H_
#define _FRAMERATECONTROLLER_H_

class FrameRateController
{
    static constexpr double unit_ { 1.0 }; // 1 second
    static constexpr int def_fps_ { 50 };
    static constexpr double def_rel_win_ { 0.05 };
public:
    FrameRateController(int fps = def_fps_, double rwin = def_rel_win_);

    double real_fps() const { return _real_fps; }
    int fps() const { return _fps; }
    double rwin() const { return _rwin; }

    void set_fps(int fps);
    void set_rwin(double rwin);

    void operator()();

private:
    using hr_clock = std::chrono::high_resolution_clock;
    using dur_t = std::chrono::duration<double>;

    double      _real_fps;
    double      _rwin;
    hr_clock::time_point _t0;
    int         _fps;
    dur_t       _sleep;
    int         _count;
};

#endif
