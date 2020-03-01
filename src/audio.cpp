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
#include <string>

#include <SFML/Audio.hpp>

AudioBase::~AudioBase() = default;

DummyAudioInterface::~DummyAudioInterface() = default;
void DummyAudioInterface::play(e_sample) {}
void DummyAudioInterface::setEnabled(bool) {}
bool DummyAudioInterface::isEnabled() const { return false; }
void DummyAudioInterface::toggle() {}

struct AudioInterface::Impl
{
    sf::SoundBuffer buffer[NUM_EFFECTS];
    sf::Sound effect[NUM_EFFECTS];
    bool enabled;

    void load(e_sample pos, const std::string& filename)
    {
        if (!buffer[pos].loadFromFile(filename))
        {
            std::cerr << "error loading audio file " << filename << std::endl;
        }
    }

    Impl()
    {
        enabled = true;
        static const std::string resource_path = "./res/";

        load(RESTART, resource_path + "restart.ogg");
        load(DROP1, resource_path + "drop1.wav");
        load(DROP2, resource_path + "drop2.wav");
        load(DROP3, resource_path + "drop3.wav");
        load(DROP4, resource_path + "drop4.wav");
        load(DROP5, resource_path + "drop5.wav");
        load(DROP6, resource_path + "drop6.wav");
        load(ERROR, resource_path + "err.wav");
        load(INFO, resource_path + "info.wav");
        load(WARNING, resource_path + "warn.ogg");
        load(LOSER, resource_path + "loser.ogg");
        load(WINNER, resource_path + "winner.ogg");

        for (int i = 0; i < NUM_EFFECTS; ++i)
            effect[i].setBuffer(buffer[i]);
    }
};

AudioInterface::~AudioInterface() = default;

AudioInterface::AudioInterface():
    _impl{ std::make_unique<Impl>() }
{}

void AudioInterface::play(e_sample n)
{
    if (_impl->enabled) _impl->effect[n].play();
}

void AudioInterface::setEnabled(bool enabled)
{
    _impl->enabled = enabled;
}

bool AudioInterface::isEnabled() const
{
    return _impl->enabled;
}

void AudioInterface::toggle()
{
    _impl->enabled = !_impl->enabled;
}
