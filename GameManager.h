#pragma once
#include "Player.h"
#include "ComputerAI.h"
#include "FastLED.h"

#define Brightness 18
#define BOARD_SIZE 8

//----------------------------------------
//   
class GameManager {
  public:
       CRGB leds[BOARD_SIZE * BOARD_SIZE];
       Player * player1;
  ComputerAI * player2;
  int turn;
  int mode;
  bool eatStreak;
  GameManager(int);
  //-------  METHODS  --------
  int gameLoop(int, int, int, int);

  void printBoard();
  int gameEnded();

};
