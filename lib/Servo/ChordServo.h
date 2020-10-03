#ifndef _H_CHORD_SERVO_
#define _H_CHORD_SERVO_

#include "Servo.h"

enum ChordPos{
    HEAD = 0,
    MID_1,
    MID_2,
    BRIDGE,
    NUM
} RotateDirection;


class ChordServo: public Servo
{
    int _initialPos;
    int _chordPositions[ChordPos::NUM];

    public:
        ChordServo(uint8_t pin, int beatInterval, int initialPos)
         : Servo(pin, beatInterval, initialPos)
        {
            _initialPos = initialPos;
            for(int i=0; i<ChordPos::NUM; ++i)
                _chordPositions[i] = _initialPos - i * beatInterval / 10;
            _updateInterval = beatInterval;
            _lastUpdate = 0;
        }

        void Reset()
        {
            if(_pos != _initialPos)
            {
                _write_angle(_initialPos);
                _pos = _initialPos;
            }
        }

        void Set()
        {
            if(_pos != _chordPositions[ChordPos::NUM-1])
            {
                _write_angle(_chordPositions[ChordPos::NUM-1]);
                _pos = _chordPositions[ChordPos::NUM-1];
            }
        }

        void Update(uint8_t chordPos)
        {
            unsigned long currentTime = millis();
            if((currentTime - _lastUpdate) > _updateInterval)
            { 
                _lastUpdate = currentTime;
            }
            else
            {// base to target
                _pos = _chordPositions[chordPos];
                _write_angle(_pos);
            }
            // if(_pin == 1)
            //     Serial.printf("[%lu - %lu = %lu], %d, %d\n", currentTime, _lastUpdate, currentTime - _lastUpdate, _pos, _updateInterval);
        }

        int InitialPos()
        {
            return _initialPos;
        }

        int EndPos()
        {
            return _chordPositions[ChordPos::NUM-1];
        }
};

#endif // _H_CHORD_SERVO_