# MentometerPro - Komplett PlatformIO-projekt

## Projektstruktur

```text
MentometerPro/
├── platformio.ini
├── include/
│   ├── Config.h
│   └── Teams.h
├── src/
│   └── main.cpp
├── lib/
│   └── QuizGame/
│       ├── QuizGame.h
│       └── QuizGame.cpp
└── test/
```

---

## platformio.ini

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
```

---

## include/Config.h

```cpp
#pragma once

const uint8_t BUTTON_1 = 2;
const uint8_t BUTTON_2 = 4;
const uint8_t BUTTON_3 = 5;
const uint8_t BUTTON_4 = 18;

const uint8_t RESET_BUTTON = 19;

const uint8_t LED_1 = 25;
const uint8_t LED_2 = 26;
const uint8_t LED_3 = 27;
const uint8_t LED_4 = 32;

const uint8_t BUZZER = 33;
```

---

## include/Teams.h

```cpp
#pragma once

const char* TEAM_NAMES[4] =
{
    "Räv",
    "Björn",
    "Örn",
    "Varg"
};
```

---

## lib/QuizGame/QuizGame.h

```cpp
#pragma once

#include <Arduino.h>

struct Player
{
    uint8_t buttonPin;
    uint8_t ledPin;
};

class QuizGame
{
public:

    void begin();
    void update();

    static void IRAM_ATTR player1ISR();
    static void IRAM_ATTR player2ISR();
    static void IRAM_ATTR player3ISR();
    static void IRAM_ATTR player4ISR();

private:

    static volatile int winner;
    static volatile uint32_t winnerTime;

    static volatile uint32_t lastInterrupt[4];

    static Player players[4];

    bool ledState = false;
    uint32_t blinkTimer = 0;

    void processWinner();
    void blinkWinner();
    void resetGame();

    static void handleButton(uint8_t player);
};
```

---

## lib/QuizGame/QuizGame.cpp

```cpp
#include <Arduino.h>
#include "QuizGame.h"
#include "Config.h"
#include "Teams.h"

volatile int QuizGame::winner = -1;
volatile uint32_t QuizGame::winnerTime = 0;

volatile uint32_t QuizGame::lastInterrupt[4] =
{
    0,0,0,0
};

Player QuizGame::players[4] =
{
    {BUTTON_1, LED_1},
    {BUTTON_2, LED_2},
    {BUTTON_3, LED_3},
    {BUTTON_4, LED_4}
};

void QuizGame::begin()
{
    for(int i=0;i<4;i++)
    {
        pinMode(players[i].buttonPin, INPUT_PULLUP);
        pinMode(players[i].ledPin, OUTPUT);
        digitalWrite(players[i].ledPin, LOW);
    }

    pinMode(RESET_BUTTON, INPUT_PULLUP);
    pinMode(BUZZER, OUTPUT);

    attachInterrupt(
        digitalPinToInterrupt(BUTTON_1),
        player1ISR,
        FALLING);

    attachInterrupt(
        digitalPinToInterrupt(BUTTON_2),
        player2ISR,
        FALLING);

    attachInterrupt(
        digitalPinToInterrupt(BUTTON_3),
        player3ISR,
        FALLING);

    attachInterrupt(
        digitalPinToInterrupt(BUTTON_4),
        player4ISR,
        FALLING);

    Serial.println("Mentometer redo");
}

void QuizGame::update()
{
    if(digitalRead(RESET_BUTTON) == LOW)
    {
        resetGame();

        while(digitalRead(RESET_BUTTON) == LOW)
        {
            delay(10);
        }
    }

    if(winner != -1)
    {
        processWinner();
        blinkWinner();
    }
}

void QuizGame::processWinner()
{
    static int lastWinner = -1;

    if(lastWinner == winner)
    {
        return;
    }

    lastWinner = winner;

    Serial.println();
    Serial.println("==================");
    Serial.println("VINNARE");
    Serial.println(TEAM_NAMES[winner]);

    Serial.print("Tidpunkt: ");
    Serial.println(winnerTime);

    tone(BUZZER,1500,300);
}

void QuizGame::blinkWinner()
{
    if(millis() - blinkTimer < 300)
    {
        return;
    }

    blinkTimer = millis();
    ledState = !ledState;

    digitalWrite(
        players[winner].ledPin,
        ledState);
}

void QuizGame::resetGame()
{
    noTone(BUZZER);

    for(int i=0;i<4;i++)
    {
        digitalWrite(players[i].ledPin, LOW);
    }

    winner = -1;
    winnerTime = 0;

    ledState = false;

    Serial.println();
    Serial.println("Ny fråga");
}

void QuizGame::handleButton(uint8_t player)
{
    uint32_t now = millis();

    if(now - lastInterrupt[player] < 50)
    {
        return;
    }

    lastInterrupt[player] = now;

    if(winner == -1)
    {
        winner = player;
        winnerTime = micros();
    }
}

void IRAM_ATTR QuizGame::player1ISR()
{
    handleButton(0);
}

void IRAM_ATTR QuizGame::player2ISR()
{
    handleButton(1);
}

void IRAM_ATTR QuizGame::player3ISR()
{
    handleButton(2);
}

void IRAM_ATTR QuizGame::player4ISR()
{
    handleButton(3);
}
```

---

## src/main.cpp

```cpp
#include <Arduino.h>
#include "QuizGame.h"

QuizGame game;

void setup()
{
    Serial.begin(115200);

    Serial.println();
    Serial.println("====================");
    Serial.println("Mentometer Pro");
    Serial.println("====================");

    game.begin();
}

void loop()
{
    game.update();
}
```

---

## Möjliga förbättringar

### Version 2
- OLED-display
- Visning av vinnare
- Antal frågor

### Version 3
- ESP-NOW
- Trådlösa knappar

### Version 4
- Webbgränssnitt
- Resultatlista
- Poängräkning

### Version 5 (Scoutkår Edition)
- Laghantering
- Flera omgångar
- Export till CSV
- Home Assistant-integration
