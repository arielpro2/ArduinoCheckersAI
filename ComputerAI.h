#pragma once
#include "Player.h"

#define BOARD_SIZE 8
#define INT_MIN - 2147483648
#define INT_MAX 2147483647
class ComputerAI: public Player {
  private: int difficulty;
  Player * enemy;
  int alphabeta(Player, Player, int, int, int, MOVE * , bool maximizingPlayer = true);
  public: ComputerAI(int, int, int, Player * , int);
  void optimalMove();

};
