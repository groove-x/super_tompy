#include <Arduino.h>
#include <M5Stack.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

#include <Const.h>
#include <RhythmServo.h>
#include <ChordServo.h>
#include <AnimationServo.h>

using namespace Music;

SoftwareSerial mySoftwareSerial(25, 26); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

// #define ENABLE_LCD

enum State
{
    Stop = 0,
    Pause,
    Play,
    NumState
};
State state = Stop;

int beatIndex = 0;
int patternIndex = 0;
unsigned long lastUpdate = 0;
unsigned long printLastUpdate = 0;
bool lastWasReset = false;

RhythmServo rhythm_servos[Swing::Pin::NUM]
 = {RhythmServo(Swing::Pin::DOGMA_HAND_R,  beatInterval, 90, Minus), 
    RhythmServo(Swing::Pin::DOGMA_HAND_L,  beatInterval, 95, Plus),                                               
    RhythmServo(Swing::Pin::DOGMA_FOOT_R,  beatInterval, 90, Plus),
    RhythmServo(Swing::Pin::DOGMA_FOOT_L,  beatInterval, 95, Plus),
    RhythmServo(Swing::Pin::SIGMA_HAND_R,  beatInterval, 95, Plus),
    RhythmServo(Swing::Pin::MAGMA_HAND_R,  beatInterval, 95, Plus)};

ChordServo chord_servos[Chord::Pin::NUM]
 = {ChordServo(Chord::Pin::SIGMA_HAND_L,  beatInterval, 95),
    ChordServo(Chord::Pin::MAGMA_HAND_L,  beatInterval, 95)};

AnimationServo anim_servos[Anim::Pin::NUM]
 = {AnimationServo(Anim::Pin::DOGMA_HEAD_PITCH, 90, Anim::neck_pitch_frames, Anim::dram_pitch_len),
    AnimationServo(Anim::Pin::DOGMA_HEAD_ROLL, 90, Anim::neck_roll_frames, Anim::dram_roll_len),
    AnimationServo(Anim::Pin::SIGMA_HEAD_PITCH, 90, Anim::neck_pitch_frames, Anim::vocal_pitch_len),
    AnimationServo(Anim::Pin::SIGMA_HEAD_ROLL, 90, Anim::neck_roll_frames, Anim::vocal_pitch_len)};

void servo_reset()
{
    for(int i=0; i<Swing::Pin::NUM;++i)
    {
        rhythm_servos[i].Reset();
    }
    for(int i=0; i<Chord::Pin::NUM;++i)
    {
        chord_servos[i].Reset();
    }
    for(int i=0; i<Anim::Pin::NUM;++i)
    {
        anim_servos[i].Reset();
    }
}

void servo_set()
{
    for(int i=0; i<Swing::Pin::NUM;++i)
    {
        rhythm_servos[i].Set();
    }
    for(int i=0; i<Chord::Pin::NUM;++i)
    {
        chord_servos[i].Set();
    }
}

void servo_update()
{
    unsigned long current_time = millis();
    if(current_time - lastUpdate > beatInterval){
        lastUpdate = current_time;
        beatIndex = (beatIndex+1) % beat_len;
        if(beatIndex == 0){
            patternIndex = (patternIndex+1) % Main::pattern_len; 
            // state = Pause;  // only play 1 pattern
            lastUpdate = millis();
        }
    }

    for(int i=0; i<Swing::Pin::NUM;++i)
    {
        rhythm_servos[i].Update(Swing::getMainBeat(patternIndex, i, beatIndex));
    }
    for(int i=0; i<Chord::Pin::NUM;++i)
    {
        chord_servos[i].Update(Chord::getMainBeat(patternIndex, i, beatIndex));
    }
    for(int i=0; i<Anim::Pin::NUM;++i)
    {
        anim_servos[i].Update();
    }
}

void setup() {
    mySoftwareSerial.begin(9600);
    M5.begin(true, false, true);

    Serial.println();
    Serial.println(F("DFRobot DFPlayer Mini Demo"));
    Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
    if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
        Serial.println(F("Unable to begin:"));
        Serial.println(F("1.Please recheck the connection!"));
        Serial.println(F("2.Please insert the SD card!"));
        while(true){
            delay(1); // Code to compatible with ESP8266 watch dog.
        }
    }
    Serial.println(F("DFPlayer Mini online."));
 
    M5.Lcd.println("Super tompy");
    M5.Lcd.println("BtnA: Start/Pause");
    M5.Lcd.println("BtnB: Pause and set base/target position");
    M5.Lcd.println("BtnC: Pause and reset");

    Wire.begin(21, 22, 100000);

    servo_reset();
    delay(1000);

    myDFPlayer.volume(30);  //Set volume value. From 0 to 30
    // myDFPlayer.playMp3Folder(1); //play specific mp3 in SD:/MP3/0001.mp3; File Name(0~65535)
}

void display()
{
    unsigned long currentTime = millis();
    if(currentTime - printLastUpdate > 100){
        M5.Lcd.startWrite();

        printLastUpdate = currentTime;
        M5.Lcd.setCursor(0, 150);
        for(int i=0; i<Swing::Pin::NUM;++i)
        {
            M5.Lcd.printf("%d: %+4d, %d, %d, %d\n", rhythm_servos[i].Pin(), rhythm_servos[i].Pos(), 
                rhythm_servos[i].BasePos(), rhythm_servos[i].TargetAng(), rhythm_servos[i].RotateDirection());
        }
        if (state == Play)
            M5.Lcd.println("Play ");
        else if(state == Pause)
            M5.Lcd.println("Pause");        
        else if(state == Stop)
            M5.Lcd.println("Stop ");        
        M5.Lcd.printf("patternIndex: %3d, beatIndex: %3d\n", patternIndex, beatIndex);
        M5.Lcd.printf("beatInterval: %3d\n", beatInterval);
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
        if(state == Play)
        {
            state = Pause;
            myDFPlayer.pause();
        }else if(state == Stop)
        {
            state = Play;
            lastUpdate = millis();
            myDFPlayer.playMp3Folder(1);
        }else if(state == Pause)
        {
            state = Play;
            lastUpdate = millis();
            myDFPlayer.play();
        }
    }
    if(M5.BtnB.wasPressed())
    {
        state = Pause;
        myDFPlayer.pause();
        if(lastWasReset)
            servo_set();
        else
            servo_reset();
        lastWasReset = !lastWasReset;
    }
    if(M5.BtnC.wasPressed())
    {
        state = Stop;
        myDFPlayer.pause();
        servo_reset();
        beatIndex = 0;
        patternIndex = 0;
    }

    if (myDFPlayer.available()) {
        uint8_t type = myDFPlayer.readType();
        int value = myDFPlayer.read();
        printDetail(type, value); //Print the detail message from DFPlayer to handle different errors and states.
        if(type == DFPlayerPlayFinished)
        {
            myDFPlayer.next();
        }
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
#ifdef ENABLE_LCD
    // if (state != Play)
    // {
        display();
    // }
#endif
    input();
    update();
}

void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}