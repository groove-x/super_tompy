#ifndef _H_RHYTHM_SERVO_
#define _H_RHYTHM_SERVO_

#include <Wire.h>
#define SERVO_ADDR 0x53

typedef enum{
    Plus = 1,
    Minus = -1,
} RotateDirection;

class RhythmServo
{
    uint8_t _pin;
    int _pos;
    int _baseAng;
    int _targetAng;
    int _rotate_direction;
    int _increment;
    int _updateInterval;
    unsigned long _lastUpdate;

    public:
        RhythmServo(uint8_t pin, int beatInterval, int baseAng, RotateDirection dir)
        {
            _pin = pin;
            _pos = _baseAng = baseAng;
            _rotate_direction = dir;
            _targetAng = baseAng + beatInterval / 10 * _rotate_direction;         // 0 -> 20 -> 0 can be 100msec
            _increment = dir;
            _updateInterval = 5;  // 動作速度：0.3秒/60度 https://www.amazon.co.jp/dp/B07TYYLMVY
        }

        void Reset()
        {
            _write_angle(_baseAng);
            _pos = _baseAng;
        }

        void Update(uint8_t canPlay)
        {
            unsigned long currentTime = millis();
            if((currentTime - _lastUpdate) > _updateInterval && canPlay)
            {
                _lastUpdate = currentTime;

                _pos += _increment;
                _write_angle(_pos);

                if (_rotate_direction > 0 && (_pos >= _targetAng || _pos <= _baseAng))
                {// e.g. ang 90 -> 100 -> 90
                    // reverse direction
                    _increment = -_increment;
                }
                else if (_rotate_direction < 0 && (_pos >= _baseAng || _pos <= _targetAng))
                {// e.g. ang 100 -> 90 -> 100
                    // reverse direction
                    _increment = -_increment;
                }
            }
        }

        int Pos()
        {
            return _pos;
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

        uint8_t Pin()
        {
            return _pin;
        }

        // addr 0x01 means "control the number 1 servo by us"
        void _write_us(uint16_t us) {
            Wire.beginTransmission(SERVO_ADDR);
            Wire.write(0x00 | _pin);
            Wire.write(us & 0x00ff);
            Wire.write(us >> 8 & 0x00ff);
            Wire.endTransmission();
        }

        // addr 0x11 means "control the number 1 servo by angle"
        void _write_angle(uint8_t angle) {
            
            Wire.beginTransmission(SERVO_ADDR);
            Wire.write(0x10 | _pin);
            Wire.write(angle);
            Wire.endTransmission();
        }        
};

#endif // _H_RHYTHM_SERVO_