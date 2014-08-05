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

// audio interface

#ifndef _AUDIO_H_
#define _AUDIO_H_

#include <SFML/Audio.hpp>
#include <string>

struct AudioBase {
	enum { RESTART = 0, DROP1, DROP2, DROP3, DROP4, DROP5, DROP6, 
		   ERROR, INFO, WARNING, LOSER, WINNER, NUM_EFFECTS };
    virtual ~AudioBase() { }
    virtual void play(int) = 0;
    virtual void setEnabled(bool) = 0;
    virtual bool is_enabled() const = 0;
    virtual void toggle() = 0;
};

struct DummyAudioInterface: public AudioBase {
    void play(int) const { }
    void setEnabled(bool) const { }
    bool is_enabled() const { return false; }
    void toggle() const { }
};

class AudioInterface: public AudioBase {
	sf::SoundBuffer buffer[NUM_EFFECTS];
	sf::Sound effect[NUM_EFFECTS];
    bool _enabled;
public:
	AudioInterface();
    void play(int n) { if (_enabled) effect[n].play(); }
    void setEnabled(bool enabled) { _enabled = enabled; }
    bool is_enabled() const { return _enabled; }
    void toggle() { _enabled = !_enabled; }
private:
	void load(int pos, const std::string& filename);
};

#endif

