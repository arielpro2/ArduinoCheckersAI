#include "ComputerAI.h"

ComputerAI::ComputerAI(int srow, int erow, int dir, Player * enemy, int difficulty): Player(srow, erow, dir) {
  this -> difficulty = difficulty;
  this -> enemy = enemy;
}
void ComputerAI::optimalMove() {
  MOVE best = {
    -1,
    -1,
    -1,
    -1
  };
  alphabeta( * this, * enemy, difficulty, INT_MIN, INT_MAX, & best);
  if (best.rowS != -1)
    playMove(enemy, best.rowS, best.colS, best.rowD, best.colD, false);
}
int ComputerAI::alphabeta(Player player, Player enemy, int depth, int alpha, int beta, MOVE * best, bool maximizingPlayer = true) {
  int bestScore, value, moveCount;
  MOVE moves[60];
  MOVE m;
  moveCount = maximizingPlayer ? player.getMoves( & enemy, moves) : enemy.getMoves( & player, moves);
  if (!depth || !moveCount) {
    int score = player.getScore() - enemy.getScore();
    return score;
  }
  bestScore = maximizingPlayer ? INT_MIN : INT_MAX;
  for (int i = 0; i < moveCount; i++) {
    Player playertemp = player;
    Player enemytemp = enemy;
    if (maximizingPlayer)
      playertemp.playMove( & enemytemp, moves[i].rowS, moves[i].colS, moves[i].rowD, moves[i].colD, false);
    else
      enemytemp.playMove( & playertemp, moves[i].rowS, moves[i].colS, moves[i].rowD, moves[i].colD, false);
    value = alphabeta(playertemp, enemytemp, depth - 1, alpha, beta, & m, !maximizingPlayer);
    if (maximizingPlayer ? value >= bestScore : value <= bestScore) {
      bestScore = value;
      * best = moves[i];
    }
    if (maximizingPlayer)
      alpha = max(alpha, bestScore);
    else
      beta = min(beta, bestScore);
    if (alpha >= beta)
      break;
  }
  return bestScore;
}
