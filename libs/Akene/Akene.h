/*
This file is part of Akene library.
Visit <http://snootlab.com>
Copyright (C) 2013-2015 Snootlab. All rights reserved.

Akene is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Akene is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Akene.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef AKENE_H
#define AKENE_H

#include <Arduino.h>
#include <SoftwareSerial.h>

class Akene_ {
    public:
        Akene_();
        ~Akene_();
        void begin();
        bool isReady();
        bool send(const void* data, uint8_t len);
        uint8_t getRev();
        unsigned long getID();
        bool setPower(uint8_t power);

        enum RETURN_CODE {
            OK = 'O',
            KO = 'K',
            SENT = 'S'
        };

    private:
        SoftwareSerial _serial;
        unsigned long _lastSend;

        uint8_t _nextReturn();
};

extern Akene_ Akene;

#endif