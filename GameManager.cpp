#include "GameManager.h"
GameManager::GameManager(int mode) {
  turn = 0;
  this -> mode = mode;
  this -> eatStreak = false;
  player1 = new Player(BOARD_SIZE - 3, BOARD_SIZE - 1, -1);
  if (mode == 0) {
    player2 = new Player(0, 2, 1);
  } else {
    player2 = new ComputerAI(0, 2, 1, player1, mode);
  }
}
int GameManager::gameEnded() {
  if (player1->getRegularCells()==0 && player1->getKingCells() == 0) {
    return 2;
  }
  if (player2 -> getRegularCells() == 0 && player2 -> getKingCells() == 0) {
    return 1;
  }
  return 0;
}
void GameManager::printBoard() {
  uint64_t P1RegularCells = player1 -> getRegularCells();
  uint64_t P1KingCells = player1 -> getKingCells();
  uint64_t P2RegularCells = player2 -> getRegularCells();
  uint64_t P2KingCells = player2 -> getKingCells();
  for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++) {
    leds[i] = P1RegularCells & (1ULL << i) ?
      CRGB::Red : P1KingCells & (1ULL << i) ?
      CRGB::DarkViolet : P2RegularCells & (1ULL << i) ?
      CRGB::Blue : P2KingCells & (1ULL << i) ?
      CRGB::Cyan : CRGB::White;
    leds[i].maximizeBrightness(Brightness);
  }
}
int GameManager::gameLoop(int rowS, int colS, int rowD, int colD) {
  if (mode) {
    int STATUS = player1 -> playMove(player2, rowS, colS, rowD, colD, eatStreak);
    if (STATUS & 1) {
      int winningPlayer = gameEnded();
      if (winningPlayer)
        return winningPlayer;
 //if player can eat again -> exit without switching turns
      if (STATUS & (1 << 2) && player1 -> canEat(player2, rowD, colD)) {
        printBoard();
        eatStreak = true;
        return 0;
      }
      eatStreak = false;
      //CPU Turn
      turn++;
      player2 -> optimalMove();
      turn++;
      //Checking if game ended
      winningPlayer = gameEnded();
      if (winningPlayer)
        return winningPlayer;
      printBoard();
    }
    return 0;
  } else {
    Player * current = turn % 2 == 0 ? player1 : player2;
    Player * enemy = turn % 2 == 0 ? player2 : player1;
    int STATUS = current -> playMove(enemy, rowS, colS, rowD, colD, eatStreak);
    if (STATUS & 1) {
      int winningPlayer = gameEnded();
      if (winningPlayer)
        return winningPlayer;
      //if player can eat again -> exit without switching turns
      if (STATUS & (1 << 2) && current -> canEat(player2, rowD, colD)) {
        printBoard();
        eatStreak = true;
        return 0;
      }
      eatStreak = false;
      turn++;
      //Checking if game ended
      winningPlayer = gameEnded();
      if (winningPlayer)
        return winningPlayer;
      printBoard();
    }
    return 0;
  }}
