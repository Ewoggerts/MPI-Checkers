#ifndef STRUCTS_H
#define STRUCTS_H

#define BOARD_SIZE 8

typedef struct {
    int row;
    int col;
    char color;
} Piece;

typedef struct {
    Piece pieces[12];
    unsigned int count;
} PieceList;

typedef struct {
    char board[BOARD_SIZE][BOARD_SIZE];
} Board;

typedef struct {
    Board** boards;
    unsigned int count;
    unsigned int capacity;
} BoardList;


void print_board(Board *board);
void init_board_list(BoardList* list, int initial_capacity);
void free_board_list(BoardList* list);
void add_to_board_list(BoardList* list, Board* board);
PieceList index_pieces(Board board, char color);
Board copy_board(Board *original);


#endif