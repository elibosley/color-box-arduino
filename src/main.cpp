#include <Arduino.h>
#include <FastLed.h>

#define LED_PIN 7
#define NUM_LEDS 12

#define sw1 13
#define sw2 12
#define sw3 11
#define sw4 10
#define sw5 9
#define sw6 8

int val1;
int val2;
int val3;
int val4;
int val5;
int val6;

String value = "";
// Every other LED is valid, (LED + 1) % 2 = 0
CRGB leds[NUM_LEDS];

CRGB colors[NUM_LEDS / 2] = {
    CRGB(255, 0, 240),
    CRGB(50, 0, 255),
    CRGB(79, 204, 41),
    CRGB(255, 5, 5),
    CRGB(255, 230, 0),
    CRGB(15, 0, 219)};

enum GameStatus
{
    NEW,
    PLAYING,
    WON,
    LOST
};

bool compareArr(int arr1[NUM_LEDS / 2], int arr2[NUM_LEDS / 2])
{

    for (int i = 0; i < (NUM_LEDS / 2); ++i)
    {
       if (arr1[i] != arr2[i]) {
           return false;
       }
    }
    return true;
}

class Game
{
public:
    int currStage = 0;
    int totalStages = 5;
    GameStatus currStatus = NEW;
    int currLossIterations = 0;
    int maxLossIterations = 200;
    // Stages defines each stage. As switches are pressed, currStage is incremented if they match, and reset if they do not
    int stages[5][NUM_LEDS / 2] = {
        {0},
        {0, 0, 0, 1, 0, 0},
        {0, 0, 0, 1, 0, 1},
        {0, 1, 0, 1, 0, 1},
        {0, 1, 1, 1, 0, 1}};
    GameStatus playRound(int arr [NUM_LEDS / 2])
    {
        if (compareArr(stages[currStage], arr) == false) {
            currStage = 0;
            currStatus = LOST;
        } else if (currStage >= totalStages - 1) {
            currStatus = WON;
        } else {

            currStatus = PLAYING;
            currStage += 1;
        }
        return currStatus;
    }

    void playLossAnimation() {
        if (currStatus == LOST && currLossIterations < maxLossIterations) {
            if (maxLossIterations % 2 == 0) {

            }
            maxLossIterations += 1;
        }
        
    }
};

Game *mainGame;


int swIdx(int sw)
{
    switch (sw)
    {
    case sw1:
        return 0;
    case sw2:
        return 1;
    case sw3:
        return 2;
    case sw4:
        return 3;
    case sw5:
        return 4;
    case sw6:
        return 5;
    }
    return 0;
}

CRGB &getLed(int sw)
{
    return leds[swIdx(sw) * 2];
}

void setLightForSwitch(int sw, int state)
{
    // Off is On
    if (state == 0)
    {
        getLed(sw) = colors[swIdx(sw)];
    }
    else
    {
        getLed(sw) = CRGB(0, 0, 0);
    }

    FastLED.show();
}
void setup()
{
    Serial.begin(9600);

    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
    pinMode(sw1, INPUT_PULLUP);
    pinMode(sw2, INPUT_PULLUP);
    pinMode(sw3, INPUT_PULLUP);
    pinMode(sw4, INPUT_PULLUP);
    pinMode(sw5, INPUT_PULLUP);
    pinMode(sw6, INPUT_PULLUP);
    mainGame = new Game();
}

void loop()
{
    val1 = digitalRead(sw1);
    val2 = digitalRead(sw2);
    val3 = digitalRead(sw3);
    val4 = digitalRead(sw4);
    val5 = digitalRead(sw5);
    val6 = digitalRead(sw6);
    setLightForSwitch(sw1, val1);
    setLightForSwitch(sw2, val2);
    setLightForSwitch(sw3, val3);
    setLightForSwitch(sw4, val4);
    setLightForSwitch(sw5, val5);
    setLightForSwitch(sw6, val6);
    delay(100);
    int roundVals[NUM_LEDS / 2] = {val1, val2, val3, val4, val5, val6};
    mainGame->playRound(roundVals);
    Serial.println(mainGame->currStage);
    Serial.println(mainGame->currStatus);
    value = "";
    value.concat(val1);
    value.concat(", ");
    value.concat(val2);
    value.concat(", ");
    value.concat(val3);
    value.concat(", ");
    value.concat(val4);
    value.concat(", ");
    value.concat(val5);
    value.concat(", ");
    value.concat(val6);
    // Serial.println(value);

    /*
    for (int i = 0; i <= NUM_LEDS; i+=2)
    {
        leds[i] = CRGB(0, 0, 255);
        FastLED.show();
        delay(100);
  }
  for (int i = NUM_LEDS; i >= 0; i-=2) {
    leds[i] = CRGB ( 255, 0, 0);
    FastLED.show();
    delay(100);
  }
    for (int i = 0; i <= NUM_LEDS; i+=2) {
    leds[i] = CRGB ( 0, 255, 0);
    FastLED.show();
    delay(100);
  } 
  */
    // put your main code here, to run repeatedly:
}
