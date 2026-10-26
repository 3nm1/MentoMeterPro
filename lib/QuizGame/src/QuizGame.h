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