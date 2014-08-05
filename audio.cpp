/*
    Connect Four 2014 (c) 2014 George M. Tzoumas

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

// audio interface implementation

#include "audio.h"
#include <iostream>

AudioInterface::AudioInterface()
{
    _enabled = true;
	load(RESTART, "res/restart.ogg");
	load(DROP1, "res/drop1.wav");
	load(DROP2, "res/drop2.wav");
	load(DROP3, "res/drop3.wav");
	load(DROP4, "res/drop4.wav");
	load(DROP5, "res/drop5.wav");
	load(DROP6, "res/drop6.wav");
	load(ERROR, "res/err.wav");
	load(INFO, "res/info.wav");
	load(WARNING, "res/warn.ogg");
	load(LOSER, "res/loser.ogg");
	load(WINNER, "res/winner.ogg");
	for (int i = 0; i < NUM_EFFECTS; ++i) 
		effect[i].setBuffer(buffer[i]);
}

void AudioInterface::load(int pos, const std::string& filename)
{
	if (!buffer[pos].loadFromFile(filename)) {
		std::cerr << "error loading audio file " << filename << std::endl;
	}
}
