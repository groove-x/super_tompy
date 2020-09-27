#include <Arduino.h>
#include <M5Stack.h>

#include <Const.h>
#include <RhythmServo.h>
#include <AnimationServo.h>
#include <Const.h>

// #define ENABLE_LCD

enum State
{
    Pause = 0,
    Play,
    NumState
};
State state = Pause;

int beatIndex = 0;
int patternIndex = 0;
unsigned long lastUpdate = 0;
unsigned long printLastUpdate = 0;
bool lastWasReset = false;

RhythmServo rhythm_servos[Rhythm::Pin::NUM]
 = {RhythmServo(Rhythm::Pin::DOGMA_HAND_R,  Rhythm::beatInterval, 90, Minus), 
    RhythmServo(Rhythm::Pin::DOGMA_HAND_L,  Rhythm::beatInterval, 95, Plus),                                               
    RhythmServo(Rhythm::Pin::DOGMA_FOOT_R,  Rhythm::beatInterval, 90, Plus),
    RhythmServo(Rhythm::Pin::DOGMA_FOOT_L,  Rhythm::beatInterval, 95, Plus),
    RhythmServo(Rhythm::Pin::SIGMA_HAND_R,  Rhythm::beatInterval, 95, Plus),
    RhythmServo(Rhythm::Pin::SIGMA_HAND_L,  Rhythm::beatInterval, 95, Plus),
    RhythmServo(Rhythm::Pin::MAGMA_HAND_R,  Rhythm::beatInterval, 95, Plus),
    RhythmServo(Rhythm::Pin::MAGMA_HAND_L,  Rhythm::beatInterval, 95, Plus)};

AnimationServo anim_servos[Anim::Pin::NUM]
 = {AnimationServo(Anim::Pin::DOGMA_HEAD_PITCH, 90, Anim::neck_pitch_frames, Anim::dram_pitch_len),
    AnimationServo(Anim::Pin::DOGMA_HEAD_ROLL, 90, Anim::neck_roll_frames, Anim::dram_roll_len),
    AnimationServo(Anim::Pin::SIGMA_HEAD_PITCH, 90, Anim::neck_pitch_frames, Anim::vocal_pitch_len),
    AnimationServo(Anim::Pin::SIGMA_HEAD_ROLL, 90, Anim::neck_roll_frames, Anim::vocal_pitch_len)};

void servo_reset()
{
    for(int i=0; i<Rhythm::Pin::NUM;++i)
    {
        rhythm_servos[i].Reset();
    }
    for(int i=0; i<Anim::Pin::NUM;++i)
    {
        anim_servos[i].Reset();
    }
}

void servo_set()
{
    for(int i=0; i<Rhythm::Pin::NUM;++i)
    {
        rhythm_servos[i].Set();
    }
}

void servo_update()
{
    unsigned long current_time = millis();
    if(current_time - lastUpdate > Rhythm::beatInterval){
        lastUpdate = current_time;
        beatIndex = (beatIndex+1) % Rhythm::beat_len;
        if(beatIndex == 0){
            patternIndex = (patternIndex+1) % Rhythm::pattern_len; 
            state = Pause;  // only play 1 pattern
        }
    }

    for(int i=0; i<Rhythm::Pin::NUM;++i)
    {
        rhythm_servos[i].Update(Rhythm::getBeat(patternIndex, i, beatIndex));
    }

    for(int i=0; i<Anim::Pin::NUM;++i)
    {
        anim_servos[i].Update();
    }
}

void setup() {
    M5.begin(true, false, true);
    M5.Lcd.println("Super tompy");
    M5.Lcd.println("BtnA: Start/Pause");
    M5.Lcd.println("BtnB: Pause and set base/target position");
    M5.Lcd.println("BtnC: Pause and reset");

    Wire.begin(21, 22, 100000);

    servo_reset();
    delay(1000);
}

void display()
{
    unsigned long currentTime = millis();
    if(currentTime - printLastUpdate > 100){
        M5.Lcd.startWrite();

        printLastUpdate = currentTime;
        M5.Lcd.setCursor(0, 150);
        for(int i=0; i<Rhythm::Pin::NUM;++i)
        {
            M5.Lcd.printf("%d: %+4d, %d, %d, %d\n", rhythm_servos[i].Pin(), rhythm_servos[i].Pos(), 
                rhythm_servos[i].BasePos(), rhythm_servos[i].TargetAng(), rhythm_servos[i].RotateDirection());
        }
        if (state == Play)
            M5.Lcd.println("Play ");
        else if(state == Pause)
            M5.Lcd.println("Pause");        
        M5.Lcd.printf("patternIndex: %3d, beatIndex: %3d\n", patternIndex, beatIndex);
        M5.Lcd.printf("beatInterval: %3d\n", Rhythm::beatInterval);
        for(int i=0; i<Anim::Pin::NUM;++i)
        {
            M5.Lcd.printf("%d: %+4d, %d, %d\n", anim_servos[i].Pin(), anim_servos[i].Pos(), 
                anim_servos[i].KeyframeIndex(), anim_servos[i].KeyframeLength());
        }
        M5.Lcd.endWrite();
    }
}

void input()
{
    M5.update();
    if(M5.BtnA.wasPressed())
    {
        state = State((state + 1) % NumState);
        if(state==Play)
        {
          lastUpdate = millis();
        }
    }
    if(M5.BtnB.wasPressed())
    {
        state = Pause;
        if(lastWasReset)
            servo_set();
        else
            servo_reset();
        lastWasReset = !lastWasReset;
    }
    if(M5.BtnC.wasPressed())
    {
        state = Pause;
        servo_reset();
        beatIndex = 0;
        patternIndex = 0;
    }
}

void update()
{
    if (state == Play)
    {
        servo_update();
    }
}

void loop()
{
// #ifdef ENABLE_LCD
    // if (state != Play)
    // {
        display();
    // }
// #endif
    input();
    update();
}