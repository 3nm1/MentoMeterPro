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