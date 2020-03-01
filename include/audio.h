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

#include <memory>

struct AudioBase
{
    enum e_sample {
        RESTART = 0,
        DROP1, DROP2, DROP3, DROP4, DROP5, DROP6,
        ERROR, INFO, WARNING, LOSER, WINNER,
        NUM_EFFECTS
    };

    AudioBase()                         = default;

    AudioBase( const AudioBase& )       = delete;
    AudioBase& operator=( const AudioBase& ) = delete;
    AudioBase( AudioBase&& )            = delete;
    AudioBase& operator=( AudioBase&& ) = delete;

    virtual ~AudioBase();

    virtual void play(e_sample)         = 0;
    virtual void setEnabled(bool)       = 0;
    virtual bool isEnabled() const      = 0;
    virtual void toggle()               = 0;
};

struct DummyAudioInterface: public AudioBase
{
    ~DummyAudioInterface();
    void play(e_sample);
    void setEnabled(bool);
    bool isEnabled() const;
    void toggle();
};

class AudioInterface: public AudioBase
{
public:
	AudioInterface();
    ~AudioInterface();

    void play(e_sample);
    void setEnabled(bool enabled);
    bool isEnabled() const;
    void toggle();

private:

    struct Impl;
    std::unique_ptr<Impl> _impl;
};

#endif

