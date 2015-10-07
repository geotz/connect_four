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

// frame rate controller (implementation)

#include "frameratecontroller.h"

//#include<iostream>
#include<thread>
#include<limits>

FrameRateController::FrameRateController(int fps, double rel_win):
    _real_fps{std::numeric_limits<double>::signaling_NaN()}, _rwin(rel_win), _sleep{2e-9}, _count{0}
{
    set_fps(fps);
}


void FrameRateController::set_fps(int fps)
{
    _fps = fps;
    _t0 = steady_clock::now();
}

void FrameRateController::set_rwin(double rwin)
{
    _rwin = rwin;
}

void FrameRateController::operator()()
{
    std::this_thread::sleep_for(_sleep);
    if (++_count == _fps)
    {
        const auto now = std::chrono::steady_clock::now();
        const auto dur = std::chrono::duration_cast<dur_t>(now - _t0);
        _t0 = now;
        _real_fps = _fps / dur.count();
        const double ratio = 1.0 / dur.count();
        if (ratio > 1.0 + _rwin || ratio < 1.0 - _rwin)
        { // too fast or too slow
            _sleep = dur_t(_sleep.count() * ratio);
//            std::cerr << "\nadjusted sleep = " << _sleep.count() << " ns" << std::endl;
        } else {
            // finetuning -- quadratic
            if (ratio > 1.0) {
                _sleep = dur_t(_sleep.count() * (1.0 + (ratio - 1.0)*(ratio - 1.0)));
            } else if (ratio < 1.0) {
                _sleep = dur_t(_sleep.count() * (1.0 - (1.0 - ratio)*(1.0 - ratio)));
            }
//            std::cerr << "\nfinetuned sleep = " << _sleep.count() << " ns" << std::endl;
        }
        _count = 0;
//        std::cerr << _real_fps << "fps                       \r";
    }
}
