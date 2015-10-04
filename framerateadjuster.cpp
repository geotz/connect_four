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

// frame rate adjuster (implementation)

#include "framerateadjuster.h"

#include<iostream>
#include<thread>

FrameRateAdjuster::FrameRateAdjuster(int fps, double rel_win):
    _rwin(rel_win), _sleep{2}, _count{0}
{
    set_fps(fps);
}


void FrameRateAdjuster::set_fps(int fps)
{
    _fps = fps;
    _t0 = steady_clock::now();
}

void FrameRateAdjuster::set_relWin(double rel_win)
{
    _rwin = rel_win;
}

void FrameRateAdjuster::operator()()
{
    std::this_thread::sleep_for(_sleep);
    if (++_count == _fps)
    {
        auto now = std::chrono::steady_clock::now();
        auto dur = std::chrono::duration_cast<ns_t>(now - _t0);
        _t0 = now;
        double real_fps = _count / std::chrono::duration_cast<std::chrono::duration<double>>(dur).count();
        if (real_fps > _fps * ( 1.0 + _rwin) || real_fps < _fps * ( 1.0 - _rwin))
        { // too fast or too slow
            _sleep = ns_t(static_cast<long long>(_sleep.count() * real_fps / _fps));
//            std::cerr << "\nadjusted sleep = " << _sleep.count() << " ns" << std::endl;
        }
        _count = 0;
        std::cerr << real_fps << "fps                       \r";
    }
}
