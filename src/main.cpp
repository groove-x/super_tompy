#ifdef MAIN
#include <Arduino.h>
#include <M5Stack.h>
#include <RhythmServo.h>
#include <AnimationServo.h>

#define RHYTHM_SERVO_NUM 6
#define ANIMATION_SERVO_NUM 6

#define BEAT_LEN 16
#define PATTERN_NUM 9
#define PATTERN_LEN 20

#define KEYFRAME_LEN 8

typedef enum {
    Pause = 0,
    Play,
    NumState
} State;
State state = Pause;

uint8_t beat[PATTERN_NUM][RHYTHM_SERVO_NUM][BEAT_LEN]
 = {{
    {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0},
    {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0} 
   },{
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}
   },{
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}
   },{
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
   },{
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
   },{
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
   },{
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
   },{
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
   },{
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
   }};

// uint8_t patterns[PATTERN_LEN] = {0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1};
// uint8_t patterns[PATTERN_LEN] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t patterns[PATTERN_LEN] = {0, 1, 1, 1, 1, 2, 2, 2, 3, 1, 1, 1, 4, 1, 5, 6, 7, 7, 7, 8};

int beatIndex = 0;
int patternIndex = 0;

// int beatInterval = (60. / 120.) / 4. * 1000; // 120 bpm -> 2 beat / 1sec -> 500msec / 4つ打ち (500 / 4) = 125
int beatInterval = (60. / 140) / 4. * 1000;  // 140 bpm 
// int beatInterval = (60. / 85.) / 4. * 1000;  // 85 bpm 
// int beatInterval = (60. / 81.4) / 4. * 1000; // 81.4 bpm 
// int beatInterval = (60. / 60.) / 4. * 1000;  // 60 bpm
// int beatInterval = (60. / 150.) / 4. * 1000;  // 60 bpm
// int beatInterval = (60. / 75.) / 4. * 1000;  // 60 bpm
// int beatInterval = (60. / 30.) / 4. * 1000;  // 30 bpm


unsigned long lastUpdate = 0;
unsigned long printLastUpdate = 0;
bool lastWasReset = false;

RhythmServo rhythm_servos[RHYTHM_SERVO_NUM] = {RhythmServo(1,  beatInterval, 90, Minus), 
                                               RhythmServo(3,  beatInterval, 95, Plus),                                               
                                               RhythmServo(5,  beatInterval, 90, Plus),
                                               RhythmServo(7,  beatInterval, 95, Plus),
                                               RhythmServo(9,  beatInterval, 95, Plus),
                                               RhythmServo(11, beatInterval, 95, Plus)};

KeyFrame keyframes[KEYFRAME_LEN] = { {110, 200}, {90, 200}, {110, 200}, {90, 200}, {120, 100}, {90, 100}, {120, 100}, {90, 100}};
AnimationServo anim_servos[ANIMATION_SERVO_NUM] = {AnimationServo(0, 90, keyframes, KEYFRAME_LEN),
                                                   AnimationServo(2, 90, keyframes, KEYFRAME_LEN),
                                                   AnimationServo(4, 90, keyframes, KEYFRAME_LEN),
                                                   AnimationServo(6, 90, keyframes, KEYFRAME_LEN),
                                                   AnimationServo(8, 90, keyframes, KEYFRAME_LEN),
                                                   AnimationServo(10, 90, keyframes, KEYFRAME_LEN)};

void servo_reset()
{
    for(int i=0; i<RHYTHM_SERVO_NUM;++i)
    {
        rhythm_servos[i].Reset();
    }
    for(int i=0; i<ANIMATION_SERVO_NUM;++i)
    {
        anim_servos[i].Reset();
    }
}

void servo_set()
{
    for(int i=0; i<RHYTHM_SERVO_NUM;++i)
    {
        rhythm_servos[i].Set();
    }
}

void servo_update()
{
    for(int i=0; i<RHYTHM_SERVO_NUM;++i)
    {
        rhythm_servos[i].Update(beat[patterns[patternIndex]][i][beatIndex]);
    }

    for(int i=0; i<ANIMATION_SERVO_NUM;++i)
    {
        anim_servos[i].Update();
    }

    unsigned long current_time = millis();
    if(current_time - lastUpdate > beatInterval){
        lastUpdate = current_time;
        beatIndex = (beatIndex+1) % BEAT_LEN;
        if(beatIndex == 0)
            patternIndex = (patternIndex+1) % PATTERN_LEN; 
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
        for(int i=0; i<RHYTHM_SERVO_NUM;++i)
        {
            M5.Lcd.printf("%d: %+4d, %d, %d, %d\n", rhythm_servos[i].Pin(), rhythm_servos[i].Pos(), 
                rhythm_servos[i].BasePos(), rhythm_servos[i].TargetAng(), rhythm_servos[i].RotateDirection());
        }
        if (state == Play)
            M5.Lcd.println("Play ");
        else if(state == Pause)
            M5.Lcd.println("Pause");        
        M5.Lcd.printf("patternIndex: %3d, beatIndex: %3d\n", patternIndex, beatIndex);
        M5.Lcd.printf("beatInterval: %3d\n", beatInterval);
        for(int i=0; i<ANIMATION_SERVO_NUM;++i)
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
    display();
    input();
    update();
}
#else
/***************************************************
DFPlayer - A Mini MP3 Player For Arduino
 <https://www.dfrobot.com/product-1121.html>
 
 ***************************************************
 This example shows the basic function of library for DFPlayer.
 
 Created 2016-12-07
 By [Angelo qiao](Angelo.qiao@dfrobot.com)
 
 GNU Lesser General Public License.
 See <http://www.gnu.org/licenses/> for details.
 All above must be included in any redistribution
 ****************************************************/

/***********Notice and Trouble shooting***************
 1.Connection and Diagram can be found here
 <https://www.dfrobot.com/wiki/index.php/DFPlayer_Mini_SKU:DFR0299#Connection_Diagram>
 2.This code is tested on Arduino Uno, Leonardo, Mega boards.
 ****************************************************/

#include <M5Stack.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

SoftwareSerial mySoftwareSerial(25, 26); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

// HardwareSerial mySerial2(2);

void setup()
{
  mySoftwareSerial.begin(9600);
  Serial.begin(115200);
  Serial2.begin(9600);
  
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online."));
  
  myDFPlayer.volume(10);  //Set volume value. From 0 to 30
  // myDFPlayer.play(1);  //Play the first mp3
  myDFPlayer.playMp3Folder(1); //play specific mp3 in SD:/MP3/0001.mp3; File Name(0~65535)
}

void loop()
{
  // static unsigned long timer = millis();
  // if (millis() - timer > 30000) {
  //   timer = millis();
  //   myDFPlayer.next();  //Play next mp3 every 3 second.
  // }
  
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

#endif