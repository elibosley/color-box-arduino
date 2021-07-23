#include <Arduino.h>
#include <FastLed.h>
#include <Servo.h>
#include <Buzzer.h>

int melody[] = {
    NOTE_FS5, NOTE_FS5, NOTE_D5, NOTE_B4, NOTE_B4, NOTE_E5,
    NOTE_E5, NOTE_E5, NOTE_GS5, NOTE_GS5, NOTE_A5, NOTE_B5,
    NOTE_A5, NOTE_A5, NOTE_A5, NOTE_E5, NOTE_D5, NOTE_FS5,
    NOTE_FS5, NOTE_FS5, NOTE_E5, NOTE_E5, NOTE_FS5, NOTE_E5};

// The note duration, 8 = 8th note, 4 = quarter note, etc.
int durations[] = {
    8, 8, 8, 4, 4, 4,
    4, 5, 8, 8, 8, 8,
    8, 8, 8, 4, 4, 4,
    4, 5, 8, 8, 8, 8};
// determine the length of the arrays to use in the loop iteration
int songLength = sizeof(melody) / sizeof(melody[0]);

#define BUZZER_PIN 5

#define LED_PIN 7
#define NUM_LEDS 12

#define sw1 13
#define sw2 12
#define sw3 11
#define sw4 10
#define sw5 9
#define sw6 8

#define SERVO_PIN 6
#define UNLOCKED 110
#define LOCKED 30

Servo lockingServo;

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
        if (arr1[i] != arr2[i])
        {
            return false;
        }
    }
    return true;
}

void lockBox()
{
    lockingServo.attach(SERVO_PIN);
    lockingServo.write(LOCKED);
    delay(200);
    lockingServo.detach();
}

void unlockBox()
{
    lockingServo.attach(SERVO_PIN);
    lockingServo.write(UNLOCKED);
    delay(200);
    lockingServo.detach();
}

void wonSong() {
    for (int thisNote = 0; thisNote < songLength; thisNote++)
    {
        // determine the duration of the notes that the computer understands
        // divide 1000 by the value, so the first note lasts for 1000/8 milliseconds
        int duration = 1000 / durations[thisNote];
        tone(BUZZER_PIN, melody[thisNote], duration);
        // pause between notes
        int pause = duration * 1.3;
        delay(pause);
        // stop the tone
        noTone(8);
    }
}

class Game
{
public:
    int currStage = 0;
    const static int totalStages = 5;
    GameStatus currStatus = NEW;
    bool hasPlayedWinLossAnimation = false;
    // Stages defines each stage. As switches are pressed, currStage is incremented if they match, and reset if they do not
    int stages[totalStages][NUM_LEDS / 2] = {
        {0},
        {0, 0, 0, 1, 0, 0},
        {0, 0, 0, 1, 0, 1},
        {0, 1, 0, 1, 0, 1},
        {0, 1, 1, 1, 0, 1}};
    GameStatus playRound(int arr[NUM_LEDS / 2])
    {
        Serial.print("Status: ");
        Serial.print(currStatus);
        Serial.print(" Stage");
        Serial.println(currStage);
        if (currStatus == WON || currStatus == LOST)
        {
            if (hasPlayedWinLossAnimation)
            {
                // Wait to reset until all switches are flipped back down
                if (compareArr(arr, stages[0]))
                {
                    currStatus = NEW;
                    currStage = 0;
                    lockBox();
                    hasPlayedWinLossAnimation = false;
                }
            }
            else
            {
                Serial.println("Won or Lost!");
                if (currStatus == WON)
                {
                    unlockBox();
                }
                playWinLossAnimation(currStatus == WON);
            }

            return currStatus;
        }
        if (currStage >= totalStages)
        {
            Serial.print("---- CURR STAGE ----");
            Serial.print(currStage);
            Serial.println("Setting to won");
            currStatus = WON;
        }
        else if (compareArr(stages[currStage], arr) == true)
        {
            Serial.println("Advancing Stage");
            currStatus = PLAYING;
            currStage += 1;
        }
        else if (currStage > 0 && compareArr(stages[currStage - 1], arr) == true)
        {
            currStatus = PLAYING;
        }
        else
        {
            Serial.println("Lost Game");
            currStatus = LOST;
        }
        return currStatus;
    }

    void playWinLossAnimation(bool won)
    {
        CRGB ledColor = won ? CRGB(0, 255, 0) : CRGB(255, 0, 0);
        if (!hasPlayedWinLossAnimation)
        {
            for (int i = 0; i <= NUM_LEDS; i += 2)
            {
                leds[i] = ledColor;
                FastLED.show();
                delay(150);
            }
            if (won) {
                wonSong();
            }
            hasPlayedWinLossAnimation = true;
        }
    }
};

Game mainGame;

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
    if (state == 1)
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
    lockBox();
    mainGame = Game();
}

void loop()
{
    val1 = !digitalRead(sw1);
    val2 = !digitalRead(sw2);
    val3 = !digitalRead(sw3);
    val4 = !digitalRead(sw4);
    val5 = !digitalRead(sw5);
    val6 = !digitalRead(sw6);
    if (mainGame.currStatus != WON && mainGame.currStatus != LOST)
    {
        setLightForSwitch(sw1, val1);
        setLightForSwitch(sw2, val2);
        setLightForSwitch(sw3, val3);
        setLightForSwitch(sw4, val4);
        setLightForSwitch(sw5, val5);
        setLightForSwitch(sw6, val6);
    }
    int roundVals[NUM_LEDS / 2] = {val1, val2, val3, val4, val5, val6};
    mainGame.playRound(roundVals);
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
    delay(100);
}
