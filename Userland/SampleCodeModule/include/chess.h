#ifndef CHESS_H
#define CHESS_H

#define EMPTY 0
#define PAWN 1
#define TOWER 2
#define KNIGHT 3
#define BISHOP 4
#define QUEEN 5
#define KING 6

#define WHITE_PIECE 0
#define BLACK_PIECE 1

#define YELLOW 2
#define BROWN 3

#define BOARD_SIZE 8


#include <buffer.h>
#include <colours.h>


typedef struct t_piece{
    int type;
    t_colour colour;
    int firstMove;
}t_piece;

typedef struct t_chrono{
    int time;
    int x;
    int y;
} t_chrono;


typedef struct t_chessData {
    t_piece board[BOARD_SIZE][BOARD_SIZE];
    t_colour boardColor[BOARD_SIZE][BOARD_SIZE];
    t_buffer buffer;
    char player1[BUFFER_SIZE];
    char player2[BUFFER_SIZE];

    char currentLog[35][17];

    t_chrono chrono1;
    t_chrono chrono2;

    t_chrono *currentChrono;    
    t_chrono *nextChrono; 

    char *currentPlayer;
    char *nextPlayer;

    int lost;
    int orientation;
} t_chessData;


void runChess();


#endif






