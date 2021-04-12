#ifndef CHESS_LIB_H
#define CHESS_LIB_H

#include <chess.h>

void drawBoard(t_chessData *chessData);
void cleanChessScreen(t_chessData *chessData);
int processCommand(t_chessData * chessData);
void setBoard(t_chessData *chessData);
void rotateBoard(t_chessData *chessData);
void chronoTime(int time);
void resetGame(t_chessData *chessData);
#endif
