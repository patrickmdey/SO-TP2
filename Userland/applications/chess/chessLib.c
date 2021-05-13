#include <chessLib.h>
#include <keys.h>
#include <lib.h>
#include <registers.h>
#include <stdint.h>
#include <stringLib.h>
#include <systemCalls.h>
#include <utils.h>

static char bitmaps[7][210] = {
"______________\n______________\n______________\n______________\n______________\n______________\n______________\n______________\n______________\n______________\n______________\n______________\n______________\n______________", //EMPTY
"______________\n______________\n______________\n______________\n_____XXXX_____\n____XXXXXX____\n_____XXXX_____\n___XXXXXXXX___\n_____XXXX_____\n_____XXXX_____\n_____XXXX_____\n____XXXXXX____\n__XXXXXXXXXX__\n______________", //PEON
"______________\n______________\n__XX__XX__XX__\n__XX__XX__XX__\n__XXXXXXXXXX__\n__XXXXXXXXXX__\n___XXXXXXXX___\n___XXXXXXXX___\n___XXXXXXXX___\n___XXXXXXXX___\n___XXXXXXXX___\n_XXXXXXXXXXXX_\n_XXXXXXXXXXXX_\n______________", //TORRE
"______________\n___XX__XXX____\n_XXXXXXXXXXX__\n_XXXXXXXX_XXX_\n_XXXXX_XXXXXX_\n_XXXXX___XXX__\n_XXXXXXX______\n_XXXXXXXX_____\n__XXXXXXXXX___\n__XXXXXXXXX___\n__XXXXXXXXXX__\n_XXXXXXXXXXXX_\n_XXXXXXXXXXXX_\n______________", //CABALLO
"______________\n______XX______\n_____XXXX_____\n____X_XXXX____\n___XXX_XXXX___\n____XXX_XX____\n______XX______\n______XX______\n_____XXXX_____\n_____XXXX_____\n___XXXXXXXX___\n___XXXXXXXX___\n__XXXXXXXXXX__\n______________", //ALFIL
"______XX______\n_____XXXX_____\n______XX______\n____XXXXXX____\n___X_XXXX_X___\n____XXXXXX____\n______XX______\n______XX______\n_____XXXX_____\n_____XXXX_____\n____XXXXXX____\n___XXXXXXXX___\n__XXXXXXXXXX__\n______________", //REINA
"______________\n______XX______\n____XXXXXX____\n______XX______\n___XXXXXXXX___\n____X_XX_X____\n_____XXXX_____\n______XX______\n_____XXXX_____\n_____XXXX_____\n___XXXXXXXX___\n___XXXXXXXX___\n__XXXXXXXXXX__\n______________" //REY
};

static int validatePos(char pos[2]);
static int move(t_chessData *chessData, int fromRow, int fromCol, int toRow, int toCol);
static int exchangePiece(t_chessData *chessData, int fromRow, int fromCol, int toRow, int toCol);

int processCommand(t_chessData * chessData) {
    int argc = 0;
    char from[BUFFER_SIZE] = {0}, to[BUFFER_SIZE] = {0};

    char* argv[2] = {from, to};


    strtok(0, 0, ' ');                                      //
    strtok(chessData->buffer.buffer, argv[argc], ' ');    //parse buffer
    strtok(0, argv[argc++], ' ');                           //parse buffer
    strtok(0, argv[argc], ' ');                           //parse buffer


    if(argv[0][0] == 0 || argv[1][0] == 0){
        return 0;
    }

    if( !(validatePos(argv[0]) && validatePos(argv[1])) ){
        return 0;
	}
    if(argv[0][0] >= 'A' && argv[0][0] <='H'){

        char c = argv[0][0] + 'a' - 'A';
        argv[0][0] = c;
    }
    if(argv[1][0] >= 'A' && argv[1][0] <='H'){
        char c = argv[1][0] + 'a' - 'A';
        argv[1][0] = c;
    } 
    int fromRow, fromCol, toRow, toCol;
    fromCol=argv[0][0]-'a';
    fromRow='8'-argv[0][1];

    toCol=argv[1][0]-'a';
    toRow='8'-argv[1][1];
    return move(chessData, fromRow, fromCol, toRow, toCol);

}

void drawBoard(t_chessData *chessData){


    moveCursorTo(704,20);
    printString("~~CHESS GAME~~");
    moveCursorTo(704, 104);
    printString("~~MOVE LOG~~");
    moveCursorTo(chessData->chrono1.x,chessData->chrono1.y);
    printString(chessData->player1); printString(" > ");
    chronoTime(chessData->chrono1.time);

    moveCursorTo(chessData->chrono2.x,chessData->chrono2.y);
    printString(chessData->player2); printString(" > ");
    chronoTime(chessData->chrono2.time);

    char bitmap[] = "X";
    moveCursorTo(0,0);
    int mult = 70;
       for(int i = 0; i<BOARD_SIZE;i++){
         for(int j = 0; j<BOARD_SIZE;j++){
             if((i + chessData->orientation) % 2 == 0){
                 if(j%2==0)
                     draw(bitmap, LIGHT_BROWN, mult);
                 else
                     draw(bitmap, DARK_BROWN, mult);   
             }
             else{
                 if(j%2==0)
                     draw(bitmap, DARK_BROWN, mult);
                 else
                     draw(bitmap, LIGHT_BROWN, mult);
             }
             moveCursor(mult,0);
         }
        moveCursor(5,32);
        switch(chessData->orientation){
            case 0: printInt(8-i); break;       //ORIGINAL
            case 1: putchar('H'-i); break;      //BLANCO A LA DERECHA
            case 2: printInt(1+i); break;       //BLANCO ARRIBA
            case 3: putchar('A'+i); break;      //BLANCO A LA IZQ
            default: break;
        }  
         moveCursor(-BOARD_SIZE*mult-8-5,mult-16*2);
       }

    moveCursor(0,5);
    for(int i = 0; i<8;i++){
        moveCursor(35,0);
        switch(chessData->orientation){
            case 0: putchar('A'+i); break;
            case 1: printInt(8-i); break;
            case 2: putchar('H'-i); break;
            case 3: printInt(1+i); break;
            default: break;
        }
        moveCursor(27,0);
    }
    moveCursorTo(0,0);
    
    switch(chessData->orientation){

         case 0:
            for(int i = 0; i<BOARD_SIZE;i++){
                for(int j = 0; j<BOARD_SIZE;j++){
                    t_piece aux = chessData->board[i][j];
                    draw(bitmaps[aux.type], aux.colour, 5);
                    moveCursor(5*14,0);
                }
            moveCursor(-8*5*14,5*14);
            }
            break;

        case 1:
            for(int j = BOARD_SIZE-1; j>=0; j--){
                for(int i = 0; i<BOARD_SIZE;i++){
                    t_piece aux = chessData->board[i][j];
                    draw(bitmaps[aux.type], aux.colour, 5);
                    moveCursor(5*14,0);
                }
            moveCursor(-8*5*14,5*14);
            }
            break;

        case 2:
            for(int i = BOARD_SIZE-1; i>=0; i--){
                for(int j = BOARD_SIZE-1; j>=0; j--){
                    t_piece aux = chessData->board[i][j];
                    draw(bitmaps[aux.type], aux.colour, 5);
                    moveCursor(5*14,0);
                }
            moveCursor(-8*5*14,5*14);
            }
            break;

        case 3:
            for(int j = 0; j<BOARD_SIZE; j++){
                for(int i = BOARD_SIZE-1; i>=0;i--){
                    t_piece aux = chessData->board[i][j];
                    draw(bitmaps[aux.type], aux.colour, 5);
                    moveCursor(5*14,0);
                }
            moveCursor(-8*5*14,5*14);
            }
            break;
        default:
            break;
     }
    moveCursor(-8*5*14,5*14+32);
    putchar('\n');
    putchar('\n');
}


//Link donde obtuvimos validación para movimiento de las piezas
//https://stackoverflow.com/questions/54972474/how-to-determine-valid-chess-moves#:~:text=Valid%20Bishop%20move%2C%20if%20the,%3DX1%20or%20Y2%3DY1.

static int move(t_chessData *chessData, int fromRow, int fromCol, int toRow, int toCol){
	if(fromRow < 0 || fromRow >= BOARD_SIZE || toRow < 0 || toRow >= BOARD_SIZE || fromCol < 0 || fromCol >= BOARD_SIZE ||
     toCol < 0 || toCol >= BOARD_SIZE || chessData->board[fromRow][fromCol].type == 0 || (fromRow == toRow && fromCol == toCol))
		return 0;

	t_piece piece = chessData->board[fromRow][fromCol];

    if(chessData->currentPlayer == chessData->player1 && piece.colour != WHITE)
        return 0;
    if(chessData->currentPlayer == chessData->player2 && piece.colour != BLACK)
        return 0;

    
	switch(piece.type){
		case 1://PEÓN
			if(piece.colour == WHITE){
                if(fromRow < toRow || (piece.firstMove && fromRow-toRow >1) || (!piece.firstMove && fromRow-toRow > 2))
                    return 0;
                if(fromCol != toCol){
                    if(abs(fromCol-toCol)!=1 || fromRow-toRow > 1 || chessData->board[toRow][toCol].type == EMPTY)		
                    return 0;
                }
                else{
                    for(int i=fromRow-1; i>=toRow;i--)
                        if(chessData->board[i][fromCol].type != EMPTY)
                            return 0;
                }
			}
			else{
                if(fromRow > toRow || (piece.firstMove && toRow-fromRow >1) || (!piece.firstMove && toRow-fromRow > 2))
                    return 0;
                if(fromCol != toCol){
                    if(abs(fromCol-toCol)!=1 || toRow-fromRow > 1 || chessData->board[toRow][toCol].type == EMPTY)		
                        return 0;
                }
                else{
                    for(int i=fromRow+1; i<=toRow;i++)
                        if(chessData->board[i][fromCol].type != EMPTY)
                            return 0;
                }
			}

            chessData->board[fromRow][fromCol].firstMove = 1;
			break;

		case 2://TORRE
			if (fromRow != toRow && fromCol != toCol)
					return 0;
            if(fromRow == toRow){
                int from = (fromCol < toCol)? (fromCol):(toCol);
                int to = (fromCol < toCol)? (toCol):(fromCol);
                for(int i = from+1; i<to;i++)
                    if(chessData->board[fromRow][i].type != EMPTY)
                        return 0;
            }
            if(fromCol == toCol){
                int from = (fromRow < toRow)? (fromRow):(toRow);
                int to = (fromRow < toRow)? (toRow):(fromRow);
                for(int i = from+1; i<to;i++)
                    if(chessData->board[i][fromCol].type != EMPTY)
                        return 0;
            }
			break;

		case 3://CABALLO
			if (abs((fromCol - toCol) * (fromRow - toRow)) != 2)         
					return 0;
			break;

		case 4:{//ALFIL
			if (abs(toCol-fromCol) != abs(toRow-fromRow))
					return 0;
            int fromR, fromC;

            if(fromRow < toRow && fromCol < toCol){ //ABAJO DERECHA
                fromR=fromRow+1;
                fromC=fromCol+1;

                while(fromR < toRow && fromC < toCol){
                    if(chessData->board[fromR][fromC].type != EMPTY)
                        return 0;
                    fromR++;
                    fromC++;
                }
            }

            else if(fromRow < toRow && fromCol > toCol){ //ABAJO IZQ
                fromR=fromRow+1;
                fromC=fromCol-1;

                while(fromR < toRow && fromC > toCol){
                    if(chessData->board[fromR][fromC].type != EMPTY)
                        return 0;
                    fromR++;
                    fromC--;
                }
            }
            else if(fromRow > toRow && fromCol < toCol){ //ARRIBA DERECHA
                fromR=fromRow-1;
                fromC=fromCol+1;

                while(fromR > toRow && fromC < toCol){
                    if(chessData->board[fromR][fromC].type != EMPTY)
                        return 0;
                    fromR--;
                    fromC++;
                }
            }
            else{
                fromR=fromRow-1;
                fromC=fromCol-1;

                while(fromR > toRow && fromC > toCol){
                    if(chessData->board[fromR][fromC].type != EMPTY)
                        return 0;
                    fromR--;
                    fromC--;
                }
            }
        }
			break;

		case 5:{                                        //REINA
            int notLinearMove = 1;
            if(fromRow == toRow){
                int from = (fromCol < toCol)? (fromCol):(toCol);
                int to = (fromCol < toCol)? (toCol):(fromCol);
                for(int i = from+1; i<to;i++)
                    if(chessData->board[fromRow][i].type != EMPTY)
                        return 0;
                notLinearMove = 0;
            }
            
            if(fromCol == toCol){
                int from = (fromRow < toRow)? (fromRow):(toRow);
                int to = (fromRow < toRow)? (toRow):(fromRow);
                for(int i = from+1; i<to;i++)
                    if(chessData->board[i][fromCol].type != EMPTY)
                        return 0;
                notLinearMove = 0;
            }
            if (abs(toCol-fromCol) != abs(toRow-fromRow) && notLinearMove)
					return 0;
            int fromR, fromC;

            if(fromRow < toRow && fromCol < toCol){ //ABAJO DERECHA
                fromR=fromRow+1;
                fromC=fromCol+1;

                while(fromR < toRow && fromC < toCol){
                    if(chessData->board[fromR][fromC].type != EMPTY)
                        return 0;
                    fromR++;
                    fromC++;
                }
            }

            else if(fromRow < toRow && fromCol > toCol){ //ABAJO IZQ
                fromR=fromRow+1;
                fromC=fromCol-1;

                while(fromR < toRow && fromC > toCol){
                    if(chessData->board[fromR][fromC].type != EMPTY)
                        return 0;
                    fromR++;
                    fromC--;
                }
            }
            else if(fromRow > toRow && fromCol < toCol){ //ARRIBA DERECHA
                fromR=fromRow-1;
                fromC=fromCol+1;

                while(fromR > toRow && fromC < toCol){
                    if(chessData->board[fromR][fromC].type != EMPTY)
                        return 0;
                    fromR--;
                    fromC++;
                }
            }
            else{
                fromR=fromRow-1;
                fromC=fromCol-1;

                while(fromR > toRow && fromC > toCol){
                    if(chessData->board[fromR][fromC].type != EMPTY)
                        return 0;
                    fromR--;
                    fromC--;
                }
            }
        }

        break;

		case 6://REY
        if(fromRow == toRow && fromCol != toCol)
            if(abs(fromCol - toCol) != 1) return 0;
        if(fromRow != toRow && fromCol == toCol)
            if(abs(fromRow - toRow) != 1) return 0;
        if(fromRow != toRow && fromCol != toCol)
            if(abs(fromRow - toRow) != 1 || abs(fromCol - toCol) != 1) return 0;
		break;
	}

    return exchangePiece(chessData, fromRow,fromCol,toRow,toCol);
}

static int validatePos(char pos[2]){
    if(strlen(pos) != 2)
        return 0;   
    return ((pos[0] >= 'a' && pos[0] <='h') || (pos[0] >= 'A' && pos[0] <='H')) && (pos[1] >='1' && pos[1]<='8');
}

static int exchangePiece(t_chessData *chessData, int fromRow, int fromCol, int toRow, int toCol){
	//MUEVO A UN ESPACIO VACIO
	if(chessData->board[toRow][toCol].type == EMPTY){
        chessData->board[fromRow][fromCol].firstMove = 1;
		t_piece aux = chessData->board[toRow][toCol];
		chessData->board[toRow][toCol] = chessData->board[fromRow][fromCol];
		chessData->board[fromRow][fromCol] = aux;
		return 1;
	}

    int flag = 0;
	//COMO AL REY OPUESTO
	if(chessData->board[toRow][toCol].type == KING)
        flag = 1;
    //HAGO ENROQUE
    if(chessData->board[fromRow][fromCol].type == TOWER && chessData->board[toRow][toCol].type == KING){
        if(chessData->board[fromRow][fromCol].firstMove == 1 || chessData->board[toRow][toCol].firstMove == 1)
            return 0;
        if(fromCol < toCol){
            for(int i = fromCol+1; i < toCol; i++)
                if(chessData->board[fromRow][i].type != EMPTY)
                    return 0;
            chessData->board[fromRow][fromCol].firstMove = 1;
            chessData->board[toRow][toCol].firstMove = 1;    
            t_piece aux = chessData->board[toRow][toCol-2];
            chessData->board[toRow][toCol-2] = chessData->board[toRow][toCol];
            chessData->board[toRow][toCol] = aux;

            aux = chessData->board[toRow][toCol-1];
            chessData->board[toRow][toCol-1] = chessData->board[fromRow][fromCol];
            chessData->board[fromRow][fromCol] = aux;

            return 1;
        }else{
            for(int i = fromCol-1; i > toCol; i--)
                if(chessData->board[fromRow][i].type != EMPTY)
                    return 0;
            chessData->board[fromRow][fromCol].firstMove = 1;
            chessData->board[toRow][toCol].firstMove = 1;      
            t_piece aux = chessData->board[toRow][toCol+2];
            chessData->board[toRow][toCol+2] = chessData->board[toRow][toCol];
            chessData->board[toRow][toCol] = aux;

            aux = chessData->board[toRow][toCol+1];
            chessData->board[toRow][toCol+1] = chessData->board[fromRow][fromCol];
            chessData->board[fromRow][fromCol] = aux;
            return 1;
        }

    }
    //MUEVO AL LUGAR DE UNA PIEZA MIA
    if(chessData->board[fromRow][fromCol].colour == chessData->board[toRow][toCol].colour)
        return 0;

	//COMO UNA PIEZA CONTRARIA
    chessData->board[fromRow][fromCol].firstMove = 1;
	t_piece empty = {EMPTY, WHITE};
	chessData->board[toRow][toCol] = chessData->board[fromRow][fromCol];
	chessData->board[fromRow][fromCol] = empty;

    if(flag)
        return 2;

	return 1;
}

void cleanChessScreen(t_chessData *chessData){
    syscall(CLEAR,0,0,(70*BOARD_SIZE)+24,768,0,0);
    cleanBuffer(&chessData->buffer);
}

void rotateBoard(t_chessData *chessData){
    chessData->orientation++;
    if(chessData->orientation == 4)
        chessData->orientation = 0;
    drawBoard(chessData);
}

void setBoard(t_chessData *chessData){

    for(int i = 0; i<BOARD_SIZE; i++){
        for(int j = 0; j<BOARD_SIZE;j++){
            if(i%2 == 0){
                if(j%2==0)
                    chessData->boardColor[i][j] = LIGHT_BROWN;
                else
                    chessData->boardColor[i][j] = DARK_BROWN;      
            }
            else{
                if(j%2==0)
                    chessData->boardColor[i][j] = DARK_BROWN;
                else
                    chessData->boardColor[i][j] = LIGHT_BROWN;
            }
        }
    }
    
    chessData->board[0][0] = (t_piece) {TOWER,BLACK};
    chessData->board[0][1] = (t_piece) {KNIGHT,BLACK};
    chessData->board[0][2] = (t_piece) {BISHOP,BLACK};
    chessData->board[0][3] = (t_piece) {QUEEN,BLACK};
    chessData->board[0][4] = (t_piece) {KING,BLACK};
    chessData->board[0][5] = (t_piece) {BISHOP,BLACK};
    chessData->board[0][6] = (t_piece) {KNIGHT,BLACK};
    chessData->board[0][7] = (t_piece) {TOWER,BLACK};

    for(int i = 0; i<8; i++){
        chessData->board[1][i] = (t_piece){PAWN,BLACK};
    }

    for(int i = 2; i<6;i++){
        for(int j = 0; j<8;j++){
            if(i%2 == 0){
                if(j%2==0)
                    chessData->board[i][j] = (t_piece){EMPTY,LIGHT_BROWN};
                else
                    chessData->board[i][j] = (t_piece){EMPTY,DARK_BROWN};      
            }
            else{
                if(j%2==0)
                    chessData->board[i][j] = (t_piece){EMPTY,DARK_BROWN};
                else
                    chessData->board[i][j] = (t_piece){EMPTY,LIGHT_BROWN};
            }
        }
    }

    for(int i = 0; i<8; i++){
        chessData->board[6][i] = (t_piece){PAWN,WHITE};
    }

    chessData->board[7][0] = (t_piece){TOWER,WHITE};
    chessData->board[7][1] = (t_piece){KNIGHT,WHITE};
    chessData->board[7][2] = (t_piece){BISHOP,WHITE};
    chessData->board[7][3] = (t_piece){QUEEN,WHITE};
    chessData->board[7][4] = (t_piece){KING,WHITE};
    chessData->board[7][5] = (t_piece){BISHOP,WHITE};
    chessData->board[7][6] = (t_piece){KNIGHT,WHITE};
    chessData->board[7][7] = (t_piece){TOWER,WHITE};
}

void resetGame(t_chessData *chessData){
    chessData->lost = 0;
    setBoard(chessData);
    syscall(CLEAR, 0, 0, 0, 0, 0, 0);
    chessData->currentPlayer = chessData->player1;
    chessData->nextPlayer = chessData->player2;

    chessData->chrono1.time = 0;      //5 minutos
    chessData->chrono1.x = 700;
    chessData->chrono1.y = 45;
    chessData->currentChrono = &(chessData->chrono1);
    chessData->chrono2.time = 0;      //5 minutos
    chessData->chrono2.x = 700;
    chessData->chrono2.y = 72;
    chessData->nextChrono = &(chessData->chrono2);

    drawBoard(chessData);
}

void chronoTime(int time){
    int mins, secs;
    mins = time/60;
    secs = time%60;

    char timeFormat[2][3];
    if(mins == 0)
        intToStr(mins, timeFormat[0], 1);
    else
        intToStr(mins, timeFormat[0], 2);
    intToStr(secs, timeFormat[1], 2);

    printString(timeFormat[0]);
    putchar(':');
    printString(timeFormat[1]);
}
