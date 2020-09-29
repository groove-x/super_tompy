#ifndef _H_RHYTHM_SERVO_
#define _H_RHYTHM_SERVO_

#include "Servo.h"

enum RotateDirection{
    Plus = 1,
    Minus = -1,
};

class RhythmServo: public Servo
{
    int _baseAng;
    int _targetAng;
    int _rotate_direction;

    public:
        RhythmServo(uint8_t pin, int beatInterval, int baseAng, RotateDirection dir)
         : Servo(pin, beatInterval, baseAng)
        {
            _baseAng = baseAng;
            _rotate_direction = dir;
            _targetAng = baseAng + _rotate_direction * beatInterval / 10;;         // 0 -> 20 -> 0 can be 100msec
            // 動作速度：0.3秒/60度 https://www.amazon.co.jp/dp/B07TYYLMVY
            // _updateInterval = 5 * beatInterval / 10;  
            _lastUpdate = 0;
        }

        void Reset()
        {
            if(_pos != _baseAng)
            {
                _write_angle(_baseAng);
                _pos = _baseAng;
            }
        }

        void Set()
        {
            if(_pos != _targetAng)
            {
                _write_angle(_targetAng);
                _pos = _targetAng;
            }
        }

        void Update(uint8_t canPlay)
        {
            unsigned long currentTime = millis();
            if(canPlay){
                if((currentTime - _lastUpdate) > _updateInterval)
                { 
                    _lastUpdate = currentTime;
                }
                else if((currentTime - _lastUpdate) > _updateInterval/2)
                {// target to base
                    _pos = _baseAng;
                    _write_angle(_pos);
                }
                else
                {// base to target
                    _pos = _targetAng;
                    _write_angle(_pos);
                }
                // if(_pin == 1)
                //     Serial.printf("[%lu - %lu = %lu], %d, %d\n", currentTime, _lastUpdate, currentTime - _lastUpdate, _pos, _updateInterval);
            }
        }

        int BasePos()
        {
            return _baseAng;
        }

        int TargetAng()
        {
            return _targetAng;
        }

        int RotateDirection()
        {
            return _rotate_direction;
        }
};

#endif // _H_RHYTHM_SERVO_