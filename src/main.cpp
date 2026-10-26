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
