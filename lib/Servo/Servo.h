#ifndef _H_SERVO_
#define _H_SERVO_

#include <Wire.h>
#define SERVO_ADDR 0x53

class Servo
{
    protected:
        uint8_t _pin;
        int _pos;
        int _updateInterval;
        unsigned long _lastUpdate;

    public:
        Servo(uint8_t pin, int interval, int pos)
        {
            _pin = pin;
            _updateInterval = interval;
            _pos = pos;
            _lastUpdate = 0;
        }
        virtual void Reset(void)=0;
        virtual void Set(void)=0;

        virtual void Update(uint8_t)=0;

        uint8_t Pin()
        {
            return _pin;
        }
        int Pos()
        {
            return _pos;
        }
        int UpdateInterval()
        {
            return _updateInterval;
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

#endif // _H_SERVO_
