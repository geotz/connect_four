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
//#include "ResourcePath.hpp"

AudioInterface::AudioInterface()
{
    _enabled = true;
	load(RESTART, resourcePath() + "restart.ogg");
	load(DROP1, resourcePath() + "drop1.wav");
	load(DROP2, resourcePath() + "drop2.wav");
	load(DROP3, resourcePath() + "drop3.wav");
	load(DROP4, resourcePath() + "drop4.wav");
	load(DROP5, resourcePath() + "drop5.wav");
	load(DROP6, resourcePath() + "drop6.wav");
	load(ERROR, resourcePath() + "err.wav");
	load(INFO, resourcePath() + "info.wav");
	load(WARNING, resourcePath() + "warn.ogg");
	load(LOSER, resourcePath() + "loser.ogg");
	load(WINNER, resourcePath() + "winner.ogg");
	for (int i = 0; i < NUM_EFFECTS; ++i) 
		effect[i].setBuffer(buffer[i]);
}

void AudioInterface::load(int pos, const std::string& filename)
{
	if (!buffer[pos].loadFromFile(filename)) {
		std::cerr << "error loading audio file " << filename << std::endl;
	}
}
