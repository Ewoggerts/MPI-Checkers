#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>  // For tolower()/toupper()
#include "structs.h"
#include <mpi.h>
#include <time.h>  // for seeding randomness

/*
#define BOARD_SIZE 8  // Standard checkers board size

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
 */

Piece create_piece(char color, int row, int col) {
    Piece p;
    p.color = color;
    p.row = row;
    p.col = col;
    return p;
}

void remove_piece(Board *board, int row, int col){
    board->board[row][col] = '.';
}

void initializePieceList(PieceList* list) {
    list->count = 0; // Initialize count to 0
}

void addPieceToList(PieceList* list, Piece newPiece) {
    if (list->count < 12) {
        list->pieces[list->count] = newPiece; // Store structure, not pointer
        list->count++;
    } else {
        printf("Piece list is full! Cannot add more pieces.\n");
    }
}

void add_piece_to_board(Board *board, int row, int col, char color){
    board->board[row][col] = color;
}

Board initial_board() {
    Board board;
    
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            remove_piece(&board, row, col);
        }
    }

    for (int row = 5; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if ((row + col) % 2 == 1) {  
                add_piece_to_board(&board, row, col, 'b');
            }
        }
    }

    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 8; col++) {
            if ((row + col) % 2 == 1) {  
                add_piece_to_board(&board, row, col, 'r');
            }
        }
    }

    return board;
}

Board initial_blank_board() {
    Board board;
    
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            remove_piece(&board, row, col);
        }
    }
    
    return board;
}

Board copy_board(Board *original) {
    Board new_board;
    memcpy(&new_board.board, &original->board, BOARD_SIZE * BOARD_SIZE * sizeof(char));
    return new_board;
}

void print_board(Board *board) {
    // Print column numbers
    printf("   ");
    for (int col = 0; col < 8; col++) {
        printf("%d ", col);
    }
    printf("\n");

    // Print row numbers and the grid
    for (int row = 0; row < 8; row++) {
        printf("%d  ", row);  // Row numbers
        for (int col = 0; col < 8; col++) {
            printf("%c ", board->board[row][col]);
        }
        printf("\n");
    }
}

void print_board_file(Board *board, char *message, size_t max_len) {
    int pos = 0;

    // Column headers
    pos += snprintf(message + pos, max_len - pos, "   ");
    for (int col = 0; col < 8; col++) {
        pos += snprintf(message + pos, max_len - pos, "%d ", col);
    }
    pos += snprintf(message + pos, max_len - pos, "\n");

    // Board rows
    for (int row = 0; row < 8; row++) {
        pos += snprintf(message + pos, max_len - pos, "%d  ", row);
        for (int col = 0; col < 8; col++) {
            pos += snprintf(message + pos, max_len - pos, "%c ", board->board[row][col]);
        }
        pos += snprintf(message + pos, max_len - pos, "\n");
    }
}


void move_piece(Board *board, Piece *piece, int new_row, int new_col){
    int row = piece->row;
    int col = piece->col;
    char color = piece->color;

    // Move the piece's position
    piece->row = new_row;
    piece->col = new_col;
    
    board->board[row][col] = '.';
    board->board[new_row][new_col] = color;
}

void init_board_list(BoardList* list, int initial_capacity) {
    list->boards = (Board**)malloc(initial_capacity * sizeof(Board*));
    list->count = 0;
    list->capacity = initial_capacity;
}

void free_board_list(BoardList* list) {
    for (unsigned int i = 0; i < list->count; ++i) {
        free(list->boards[i]);  
    }
    free(list->boards);
    list->boards = NULL;
    list->count = 0;
    list->capacity = 0;
}

void add_to_board_list(BoardList* list, Board* board) {
    if (list->count >= list->capacity) {
        list->capacity *= 2;
        list->boards = (Board**)realloc(list->boards, list->capacity * sizeof(Board*));
    }
    Board* copy = (Board*)malloc(sizeof(Board));
    memcpy(copy, board, sizeof(Board));
    list->boards[list->count++] = copy;
}

PieceList index_pieces(Board board, char color) {
    PieceList pieces; 
    initializePieceList(&pieces);

    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            if (board.board[row][col] == color || board.board[row][col] == toupper(color)) {
                Piece newPiece = {row, col, board.board[row][col]};
                addPieceToList(&pieces, newPiece);
            }
        }
    }
    
    return pieces;
}

void generate_random_checkers_board(Board *board, int seed) {
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            board->board[row][col] = '.';
        }
    }

    int redCount = 0;
    int blackCount = 0;
    int maxPieces = 12;

    srand(seed); 

    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            if ((row + col) % 2 == 1) {
                int randNum = rand() % 100;
                if (randNum < 20 && redCount < maxPieces) {
                    if(row == 0){
                        board->board[row][col] = 'R';
                    }
                    else{
                        board->board[row][col] = 'r';
                    }
                    redCount++; 
                } else if (randNum >= 20 && randNum < 40 && blackCount < maxPieces) {
                    if(row == BOARD_SIZE-1){
                        board->board[row][col] = 'B';
                    }
                    else{
                        board->board[row][col] = 'b';
                    }
                    blackCount++;
                } else {
                    board->board[row][col] = '.';
                }
            }
        }
    }
}

int isValidPos(int row, int col) {
    return row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE;
}

void single_captured_possibilities(Board board, Piece piece, BoardList *capture_results) {
    char opponentColor = (piece.color == 'b') ? 'r' : 'b';  
   
    int directions[4][2] = {
        {-2, -2}, {-2, 2},  // top left and top right
        {2, -2}, {2, 2}     // bottom left and bottom right
    };
    
    // Modify the directions if the piece color is 'b'
    if (piece.color == 'b') {
        // Swap the directions for the black pieces
        directions[0][0] = 2;  directions[0][1] = -2;  // bottom left
        directions[1][0] = 2;  directions[1][1] = 2;   // bottom right
        directions[2][0] = -2; directions[2][1] = -2;  // top left
        directions[3][0] = -2; directions[3][1] = 2;   // top right
    }

    // Checks if piece is a king, king's can move backwards
    int lim = board.board[piece.row][piece.col] == toupper(piece.color) ? 4 : 2; 

    for (int j = 0; j < lim; j++) {
        int newRow = piece.row + directions[j][0];
        int newCol = piece.col + directions[j][1];
        int middleRow = piece.row + directions[j][0] / 2;
        int middleCol = piece.col + directions[j][1] / 2;

        // Valid capture
        if (isValidPos(newRow, newCol) && isValidPos(middleRow, middleCol)) {
            if ((board.board[middleRow][middleCol] == opponentColor || 
                board.board[middleRow][middleCol] == toupper(opponentColor)) && board.board[newRow][newCol] == '.') {
                    
                Board new_board = copy_board(&board);
                Piece new_piece = create_piece(piece.color, piece.row, piece.col);

                if (newRow == 0 || newRow == BOARD_SIZE - 1){
                    new_piece = create_piece(toupper(piece.color), piece.row, piece.col);
                }

                move_piece(&new_board, &new_piece, newRow,newCol);
                remove_piece(&new_board, middleRow, middleCol);
                add_to_board_list(capture_results, &new_board);
                single_captured_possibilities(new_board, new_piece, capture_results);
            }
        }
    }
}

void all_capture_possibilties(Board board, char color, BoardList *all_capture_results){
    PieceList all_pieces = index_pieces(board, color);
    for (unsigned int i = 0; i < all_pieces.count; i++){
        single_captured_possibilities(board, all_pieces.pieces[i], all_capture_results);
    }
}

void generate_nojump_possibilities(Board board, Piece piece, BoardList *capture_results){
   
    int directions[4][2] = {
        {-1, -1}, {-1, 1},  // top left and top right
        {1, -1}, {1, 1}     // bottom left and bottom right
    };
    
    // Modify the directions if the piece color is 'b'
    if (piece.color == 'b') {
        // Swap the directions for the black pieces
        directions[0][0] = 1;  directions[0][1] = -1;  // bottom left
        directions[1][0] = 1;  directions[1][1] = 1;   // bottom right
        directions[2][0] = -1; directions[2][1] = -1;  // top left
        directions[3][0] = -1; directions[3][1] = 1;   // top right
    }

    // Checks if piece is a king, king's can move backwards
    int lim = board.board[piece.row][piece.col] == toupper(piece.color) ? 4 : 2; 

    for (int j = 0; j < lim; j++) {
        int newRow = piece.row + directions[j][0];
        int newCol = piece.col + directions[j][1];

        // Valid move
        if (isValidPos(newRow, newCol) && board.board[newRow][newCol] == '.') {
            Board new_board = copy_board(&board);
            Piece new_piece = create_piece(piece.color, piece.row, piece.col);
            if (newRow == 0 || newRow == BOARD_SIZE - 1){
                new_piece = create_piece(toupper(piece.color), piece.row, piece.col);
            }
            move_piece(&new_board, &new_piece, newRow,newCol);
            add_to_board_list(capture_results, &new_board);
        }
    }
}

void all_nojump_posibilities(Board board, char color, BoardList *all_nojump_results){
    PieceList all_pieces = index_pieces(board, color);
    for (unsigned int i = 0; i < all_pieces.count; i++){
        generate_nojump_possibilities(board, all_pieces.pieces[i], all_nojump_results);
    }
}

// For simplicities sake, red moves first then black
// 1 move ahead is two plies
void predict_all_moves(int moves_ahead, Board inital_board, BoardList final_possibilities){
    BoardList current_turn;
    add_to_board_list(&current_turn, &inital_board);
    for(int turn; turn < moves_ahead; turn++){
        BoardList first_ply;
        for(unsigned int i; i < current_turn.count; i++){
            all_nojump_posibilities(*current_turn.boards[i], 'r', &first_ply);
            all_capture_possibilties(*current_turn.boards[i], 'r', &first_ply);
        }
        BoardList second_ply;
        for(unsigned int i; i < first_ply.count; i++){
            all_nojump_posibilities(*first_ply.boards[i], 'b', &second_ply);
            all_capture_possibilties(*first_ply.boards[i], 'b', &second_ply);
        }
        current_turn = second_ply;
    }
    final_possibilities = current_turn;
}

void getAllMovesAhead(int movesAhead, Board initBoard, BoardList* finalBoards, char startColor){
    if(movesAhead<=0){
        return;
    }

    // Find all the moves that can be made
    // Assume you have to jump if you can
    BoardList immediateMoves;
    init_board_list(&immediateMoves, 5);
    all_capture_possibilties(initBoard, startColor, &immediateMoves);
    if (immediateMoves.count == 0) {
        all_nojump_posibilities(initBoard, startColor, &immediateMoves);
    }
    if (immediateMoves.count == 0) {
        add_to_board_list(finalBoards, &initBoard); 
    }

    //Either add the boards to the results or recurse further
    char nextColor;
    if(startColor == 'r'){
        nextColor = 'b';
    } else{
        nextColor = 'r';
    }
    if (movesAhead == 1) {
        for (unsigned int i = 0; i < immediateMoves.count; i++) {
            add_to_board_list(finalBoards, immediateMoves.boards[i]); 
        }
    } 
    else {
        for (unsigned int i = 0; i < immediateMoves.count; i++) {
            getAllMovesAhead(movesAhead - 1, *immediateMoves.boards[i], finalBoards, nextColor);
        }
    }

    free_board_list(&immediateMoves);
}


// Import Cuda aspect of program
extern void runCudaAnalysis(BoardList *boards, int *likelihood);

int main() {
    // Initialize the board
    Board board = initial_board();
    printf("Initial Board:\n");
    print_board(&board);

    // Index all red pieces
    PieceList red_pieces = index_pieces(board, 'r');
    printf("Indexed %d red pieces:\n\n", red_pieces.count);
    for (unsigned int i = 0; i < red_pieces.count; i++) {
        printf("Red Piece %d: (%d, %d)\n", i + 1, red_pieces.pieces[i].row, red_pieces.pieces[i].col);
    }

    printf("\n");

    // Index all black pieces
    PieceList black_pieces = index_pieces(board, 'b');
    printf("Indexed %d black pieces:\n\n", black_pieces.count);
    for (unsigned int i = 0; i < black_pieces.count; i++) {
        printf("Black Piece %d: (%d, %d)\n", i + 1, black_pieces.pieces[i].row, black_pieces.pieces[i].col);
    }

    // Test moving a piece
    if (red_pieces.count > 0) {
        Piece test_piece = red_pieces.pieces[0];
        printf("\nMoving first red piece from (%d, %d)...\n", test_piece.row, test_piece.col);
        
        int new_row = test_piece.row + 1;
        int new_col = test_piece.col + 1;

        if (isValidPos(new_row, new_col)) {
            move_piece(&board, &test_piece, new_row, new_col);
            printf("Board after moving piece:\n");
            print_board(&board);
        }
    }

    // Test removing a piece
    if (black_pieces.count > 0) {
        Piece test_piece = black_pieces.pieces[0];
        printf("\nRemoving first black piece at (%d, %d)...\n", test_piece.row, test_piece.col);
        remove_piece(&board, test_piece.row, test_piece.col);
        printf("Board after removing piece:\n");
        print_board(&board);
    }

    printf("\n----------------------------------------------------------------------------\n");

    MPI_Init(NULL, NULL);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    printf("MPI Rank %d of %d starting...\n", rank, size);

    Board random_board;
    generate_random_checkers_board(&random_board, rank);

    MPI_File fh;
    MPI_Offset offset = rank * 1024;
    MPI_File_open(MPI_COMM_WORLD, "boards_output.txt", MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fh);

    printf("Rank %d custom board initialized.\n", rank);
    print_board(&random_board);


    char board_output[1024];
    print_board_file(&board, board_output, sizeof(board_output));
    
    // Test capturing possibilities
    BoardList board_results;
    init_board_list(&board_results, 5);

    //printf("\nTEST Board:\n");
    //print_board(&board);

    getAllMovesAhead(6, random_board, &board_results, 'r');
    //printf("%d results\n", board_results.count);
    printf("Rank %d generated %d boards.\n", rank, board_results.count);

    int likelihood = 0 ;
    runCudaAnalysis(&board_results, &likelihood);

    //printf("Output: %d\n", likelihood);
    printf("Rank %d Cuda Analysis Output: %d\n", rank, likelihood);

    snprintf(board_output + strlen(board_output), sizeof(board_output) - strlen(board_output), "\nRank %d Cuda Analysis Output: %d\n", rank, likelihood);

    MPI_File_write_at(fh, offset, board_output, strlen(board_output), MPI_CHAR, MPI_STATUS_IGNORE);
    MPI_File_close(&fh);

    free_board_list(&board_results);

    MPI_Finalize();
    return 0;
}

