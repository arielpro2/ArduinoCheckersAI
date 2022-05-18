#include "Player.h"
#include "GameManager.h"

Player::Player(int srow, int erow, int dir) {
  this -> dir = dir;
  this -> score = 0;
  this -> cells_regular = 0;
  this -> cells_king = 0;
  for (int i = srow; i <= erow; i++) {
    for (int j = 0; j < BOARD_SIZE; j++) {
      if ((i + j) & 1)
        bitSet64(this -> cells_regular, i * BOARD_SIZE + j);
    }
  }
}

int Player::playMove(Player * enemy, int rowS, int colS, int rowD, int colD, bool eatStreak) {
  int STATUS = movable(enemy, rowS, colS, rowD, colD, eatStreak);
  if (!STATUS & 1)
    return STATUS;

  int indexS = rowS * BOARD_SIZE + colS;
  int indexD = rowD * BOARD_SIZE + colD;

  uint64_t cellS = 1ULL << indexS;
  uint64_t cellD = 1ULL << indexD;

  if (STATUS & (1 << 1)) {
    this -> cells_king &= ~cellS;
    this -> cells_king |= cellD;
  } else {
    //if destination cell is on border, replace with king
    int border_mask = dir == 1 ? BOARD_SIZE * BOARD_SIZE : BOARD_SIZE;
    if (indexD >= border_mask - BOARD_SIZE && indexD < border_mask) {
      this -> cells_regular &= ~cellS;
      this -> cells_king |= cellD;
    } else {
      this -> cells_regular &= ~cellS;
      this -> cells_regular |= cellD;
    }
  }
  if (STATUS & (1 << 2)) {
    //puck eating
    int indexM = (rowS + rowD) / 2 * BOARD_SIZE + (colS + colD) / 2;
    uint64_t cellM = 1ULL << indexM;
    enemy -> cells_king &= ~cellM;
    enemy -> cells_regular &= ~cellM;
    score++;
  }
  return STATUS;

}
int Player::movable(Player * enemy, int rowS, int colS, int rowD, int colD, bool eatStreak) {

  int indexS = rowS * BOARD_SIZE + colS;
  int indexD = rowD * BOARD_SIZE + colD;
  int indexM = (rowS + rowD) / 2 * BOARD_SIZE + (colS + colD) / 2;

  uint64_t cellS = 1ULL << indexS;
  uint64_t cellD = 1ULL << indexD;
  uint64_t cellM = 1ULL << indexM;

  // If Source cell is empty
  if (!this -> isOccupied(cellS)) {
    return 0;
  }
  // If destination cell is empty
  if (enemy -> isOccupied(cellD) || this -> isOccupied(cellD)) {
    return 0;
  }
  if (this -> cells_king & cellS) {
    if ((abs(indexD - indexS) == PADD_1 || abs(indexD - indexS) == PADD_2) && !eatStreak) {
      //king moving
      return 0b011;
    } else if ((abs(indexD - indexS) == SKIP_PADD_1 || abs(indexD - indexS) == SKIP_PADD_2) && enemy -> isOccupied(cellM)) {
      //king eating
      return 0b111;
    }

  } else {
    if ((indexD - indexS == PADD_1 * dir || indexD - indexS == PADD_2 * dir) && !eatStreak) {
      //regular moving
      return 0b001;
    }
    if ((indexD - indexS == SKIP_PADD_1 * dir || indexD - indexS == SKIP_PADD_2 * dir) && enemy -> isOccupied(cellM)) {
      //regular eating
      return 0b101;
    }
  }
  return 0;
}

bool Player::isOccupied(uint64_t mask) {
  return this -> cells_regular & mask || this -> cells_king & mask;
}

bool Player::canEat(Player * enemy, int row, int col) {
  int indexS = row * BOARD_SIZE + col;
  uint64_t cellS = 1ULL << indexS;
  //Original direction of player
  for (int i = 0; i < 2; i++) {
    int erow = row + dir;
    int drow = row + dir * 2;
    int ecol = col + (i ? 1 : -1);
    int dcol = col + (i ? 1 : -1) * 2;
    if (erow & BOARD_SIZE || drow & BOARD_SIZE || ecol & BOARD_SIZE || dcol & BOARD_SIZE)
      continue;
    uint64_t cellE = 1ULL << erow * BOARD_SIZE + ecol;
    uint64_t cellD = 1ULL << drow * BOARD_SIZE + dcol;
    if (enemy -> isOccupied(cellE) && !this -> isOccupied(cellD) && !enemy -> isOccupied(cellD))
      return true;
  }
  //if king then check also inverted direction
  if (this -> cells_king & cellS) {
    for (int i = 0; i < 2; i++) {
      int erow = row - dir;
      int drow = row - dir * 2;
      int ecol = col + (i ? 1 : -1);
      int dcol = col + (i ? 1 : -1) * 2;
      if (erow & BOARD_SIZE || drow & BOARD_SIZE || ecol & BOARD_SIZE || dcol & BOARD_SIZE)
        continue;
      uint64_t cellE = 1ULL << erow * BOARD_SIZE + ecol;
      uint64_t cellD = 1ULL << drow * BOARD_SIZE + dcol;
      if (enemy -> isOccupied(cellE) && !this -> isOccupied(cellD) && !enemy -> isOccupied(cellD))
        return true;
    }
  }
  return false;
}

uint64_t Player::getKingCells() {
  return this -> cells_king;
}

uint64_t Player::getRegularCells() {
  return this -> cells_regular;
}
void Player::appendMoveToArray(int rowS, int colS, int rowD, int colD, MOVE * pMoves, int * pSize) {
  MOVE move;
  move.rowS = rowS;
  move.colS = colS;
  move.rowD = rowD;
  move.colD = colD;
  pMoves[*pSize] = move;
  ++*pSize;
}
int Player::getMoves(Player * enemy, MOVE * moves) {
  int size = 0;
  for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++) {
    int rowS = i / BOARD_SIZE;
    int colS = i % BOARD_SIZE;
    int rowD = 0;
    int colD = 0;
    if (isOccupied(1ULL << i)) {
      for (int j = 0; j < 2; j++) {
        rowD = rowS + dir;
        colD = colS + (j ? 1 : -1);
        if (!(rowD & BOARD_SIZE) && !(colD & BOARD_SIZE) && movable(enemy, rowS, colS, rowD, colD, false) & 1) {
          appendMoveToArray(rowS, colS, rowD, colD, moves, & size);
        }
        rowD = rowS + dir * 2;
        colD = colS + (j ? 1 : -1) * 2;
        if (!(rowD & BOARD_SIZE) && !(colD & BOARD_SIZE) && movable(enemy, rowS, colS, rowD, colD, false) & 1) {
          appendMoveToArray(rowS, colS, rowD, colD, moves, & size);
        }
      }
      if (cells_king & (1ULL << i)) {
        for (int j = 0; j < 2; j++) {
          rowD = rowS - dir;
          colD = colS + (j ? 1 : -1);
          if (!(rowD & BOARD_SIZE) && !(colD & BOARD_SIZE) && movable(enemy, rowS, colS, rowD, colD, false) & 1) {
            appendMoveToArray(rowS, colS, rowD, colD, moves, & size);
          }
          rowD = rowS - dir * 2;
          colD = colS + (j ? 1 : -1) * 2;
          if (!(rowD & BOARD_SIZE) && !(colD & BOARD_SIZE) && movable(enemy, rowS, colS, rowD, colD, false) & 1) {
            appendMoveToArray(rowS, colS, rowD, colD, moves, & size);
          }
        }
      }
    }
  }
  return size;
}
int Player::getDir() {
  return this -> dir;
}
int Player::getScore() {
  int calculatedScore = 0;
  int AggregatedDistance = 0;
  int count = 0;
  for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++) {
    int row = i / BOARD_SIZE;
    int col = i % BOARD_SIZE;

    if (cells_regular & (1ULL << i)) {
      if (dir == 1) {
        //Aggregated distance of the pawns to promotion line;
        AggregatedDistance += BOARD_SIZE - row - 1;
        //Number of occupied fields on promotion line
        if (row == 0)
          calculatedScore++;
      } else {
        //Aggregated distance of the pawns to promotion line;
        AggregatedDistance += row;
        //Number of occupied fields on promotion line
        if (row == 7)
          calculatedScore++;
      }
      //Count of pawns
      count++;
      //Number of pawns
      calculatedScore += 2;
      //Number of safe pawns
      if (!col || col == 7)
        calculatedScore++;
    } else if (cells_king & (1ULL << i)) {
      if (dir == 1) {
        //Number of occupied fields on promotion line
        if (row == 0)
          calculatedScore++;
      } else {
        //Number of occupied fields on promotion line
        if (row == 7)
          calculatedScore++;
      }
      //Number of kings
      calculatedScore += 3;
      //Number of safe kings
      if (!col || col == 7)
        calculatedScore += 2;
    }
  }
  //Aggregated distance of the pawns to promotion line;
  calculatedScore -= (AggregatedDistance / count) / 2;
  return calculatedScore;
}
