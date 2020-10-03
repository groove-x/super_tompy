#include <Arduino.h>
#include <M5Stack.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

#include <Const.h>
#include <RhythmServo.h>
#include <ChordServo.h>
#include <AnimationServo.h>
#include <DogmaPitch.h>
#include <DogmaRoll.h>
#include <ShigmaPitch.h>
#include <ShigmaRoll.h>

using namespace Music;

SoftwareSerial mySoftwareSerial(25, 26); // RX, TX
SoftwareSerial mySoftwareSerial2(12, 13); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

// #define ENABLE_LCD

enum State
{
    Stop = 0,
    // for アニメーション開発用モード
    Pause,
    TestPlay,
    // for 試技用モード
    Playing,      // 演奏中
    PlayingRiff,  // 終奏（アーム移動待ち)
    Waiting,      // 瓦割り待ち
    Applauding,   // 拍手喝采を浴びてる
};
State state = Stop;

enum Mode
{
    Develop = 0,
    Demo1,        // 試技1回目
    Demo2         // 試技2回目
};
Mode mode = Demo1;

int beatIndex = 0;
int patternIndex = 0;
unsigned long lastUpdate = 0;
unsigned long printLastUpdate = 0;
bool lastWasReset = false;

RhythmServo rhythm_servos[Swing::Pin::NUM]
 = {RhythmServo(Swing::Pin::DOGMA_HAND_R,  beatInterval, 90, Minus), 
    RhythmServo(Swing::Pin::DOGMA_HAND_L,  beatInterval, 95, Plus),                                               
    RhythmServo(Swing::Pin::DOGMA_FOOT_R,  beatInterval, 90, Minus),
    RhythmServo(Swing::Pin::DOGMA_FOOT_L,  beatInterval, 95, Plus),
    RhythmServo(Swing::Pin::SIGMA_HAND_R,  beatInterval, 95, Plus),
    RhythmServo(Swing::Pin::MAGMA_HAND_R,  beatInterval, 95, Plus)};

ChordServo chord_servos[Chord::Pin::NUM]
 = {ChordServo(Chord::Pin::SIGMA_HAND_L,  beatInterval, 95),
    ChordServo(Chord::Pin::MAGMA_HAND_L,  beatInterval, 95)};

AnimationServo anim_servos[Anim::Pin::NUM]
 = {AnimationServo(Anim::Pin::DOGMA_HEAD_PITCH, dogma_pitch_frames, dogma_pitch_len),
    AnimationServo(Anim::Pin::DOGMA_HEAD_ROLL, dogma_roll_frames, dogma_roll_len),
    AnimationServo(Anim::Pin::SIGMA_HEAD_PITCH, shigma_pitch_frames, dogma_roll_len),
    AnimationServo(Anim::Pin::SIGMA_HEAD_ROLL, shigma_roll_frames, shigma_roll_len)};

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
            if(state == PlayingRiff)
                patternIndex = (patternIndex+1) % Riff::pattern_len; 
            else
                patternIndex = (patternIndex+1) % Main::pattern_len; 
            // state = Pause;  // only play 1 pattern
            lastUpdate = millis();
        }
    }

    if(state == PlayingRiff)
    {
        for(int i=0; i<Swing::Pin::NUM;++i)
        {
            rhythm_servos[i].Update(Swing::getRiffBeat(patternIndex, i, beatIndex));
        }
        for(int i=0; i<Chord::Pin::NUM;++i)
        {
            chord_servos[i].Update(Chord::getRiffBeat(patternIndex, i, beatIndex));
        }
        for(int i=0; i<Anim::Pin::NUM;++i)
        {
            anim_servos[i].Update();
        }

    }else{
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
}

void setup() {
    mySoftwareSerial.begin(9600);
    mySoftwareSerial2.begin(9600);
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
 
    M5.Lcd.setTextSize(2);
    switch(mode){
        case Develop:
            M5.Lcd.printf("Super tompy: Develop\n\n");
            M5.Lcd.println("BtnA: Start/Pause");
            M5.Lcd.println("BtnB: Set base/target pos");
            M5.Lcd.println("BtnC: Pause and reset");
            break;
        case Demo1:
        case Demo2:
            M5.Lcd.printf("Super tompy: Demo%d\n", mode);
    }

    Wire.begin(21, 22, 100000);

    servo_reset();
    delay(1000);

    myDFPlayer.volume(10);  //Set volume value. From 0 to 30
    if(mode == Demo2)
    {
        myDFPlayer.playMp3Folder(AncolSound); //play specific mp3 in SD:/MP3/0001.mp3; File Name(0~65535)
    }
    myDFPlayer.volume(20);  //Set volume value. From 0 to 30
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
        if (state == TestPlay)
            M5.Lcd.println("TestPlay ");
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

    //TODO: 実際に接続するUARTのポートを設定する
    unsigned char readBuffer[] = " ";
    int serialLength = mySoftwareSerial2.available();
    if(serialLength > 0){
      mySoftwareSerial2.readBytes(readBuffer, serialLength);
      readBuffer[serialLength] = '\0';
      Serial.printf("%s\n", readBuffer);
    }

    if(mode == Develop)
    {
        if(M5.BtnA.wasPressed() || readBuffer[0]=='1')
        {
            if(state == TestPlay)
            {
                state = Pause;
                myDFPlayer.pause();
            }else if(state == Stop)
            {
                state = TestPlay;
                lastUpdate = millis();
                myDFPlayer.playMp3Folder(1);
            }else if(state == Pause)
            {
                state = TestPlay;
                lastUpdate = millis();
                myDFPlayer.play();
            }
        }
        if(M5.BtnB.wasPressed() || readBuffer[0]=='2')
        {
            state = Pause;
            myDFPlayer.pause();
            if(lastWasReset)
                servo_set();
            else
                servo_reset();
            lastWasReset = !lastWasReset;
        }
        if(M5.BtnC.wasPressed() || readBuffer[0]=='3')
        {
            state = Stop;
            myDFPlayer.pause();
            servo_reset();
            beatIndex = 0;
            patternIndex = 0;
        }
    }else if(mode == Demo1 || mode == Demo2)
    {
        if(state == Stop && readBuffer[0]=='1')
        {// stop -> play 演奏を開始する
            state = Playing;
            lastUpdate = millis();
            myDFPlayer.playMp3Folder(MainSound);
        }
        if(state == PlayingRiff && readBuffer[0]=='2')
        {// riff -> hold 演奏を止めて動かない
            state = Waiting;
            myDFPlayer.pause();
        }
        if(state == Waiting && readBuffer[0]=='3')
        {// hold -> approuse 歓声があがって動かない
            state = Applauding;
            if(mode == Demo1)
                myDFPlayer.playMp3Folder(ApplauseSound1);
            else if(mode == Demo2)
                myDFPlayer.playMp3Folder(ApplauseSound2);

        }
    }

    if (myDFPlayer.available()) {
        uint8_t type = myDFPlayer.readType();
        int value = myDFPlayer.read();
        printDetail(type, value); //Print the detail message from DFPlayer to handle different errors and states.
        if(type == DFPlayerPlayFinished)
        {
            if(state == Playing){
                // 終奏を繰り返しつつ腕の準備を待つ
                myDFPlayer.playMp3Folder(RiffSound);
                state = PlayingRiff;
                servo_reset();
                beatIndex = 0;
                patternIndex = 0;
            }
        }
    }
}

void update()
{
    if (state == TestPlay || state == Playing || state == PlayingRiff)
    {
        servo_update();
    }
}

void loop()
{
#ifdef ENABLE_LCD
    // if (state != TestPlay)
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