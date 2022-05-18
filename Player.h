#pragma once
#include "FastLED.h"

#define bitSet64(value, bit)((value) |= (bit < 32 ? 1UL : 1ULL) << (bit))
#define PADD_1 7
#define PADD_2 9
#define SKIP_PADD_1 14
#define SKIP_PADD_2 18

class Player {
  protected:
    struct MOVE {
      int rowS;
      int colS;
      int rowD;
      int colD;
    };
  void appendMoveToArray(int, int, int, int, MOVE * , int * );
  int movable(Player * , int, int, int, int, bool);
  int score;
  int dir;
  uint64_t cells_regular;
  uint64_t cells_king;
  public:
    uint64_t getRegularCells();
  uint64_t getKingCells();
  int getDir();
  int getScore();
  Player(int, int, int);
  bool isOccupied(uint64_t);
  bool canEat(Player * , int, int);
  int getMoves(Player * , MOVE * );
  int playMove(Player * , int, int, int, int, bool);

};
