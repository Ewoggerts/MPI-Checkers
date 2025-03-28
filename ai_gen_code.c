#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>  // For tolower()/toupper()

#define BOARD_SIZE 8  // Standard checkers board size

/* 
 * Board structure - represents a checkers board state
 * Uses a 2D array of characters:
 * - 'b' = black piece
 * - 'B' = black king
 * - 'r' = red piece
 * - 'R' = red king
 * - '.' = empty square
 */
typedef struct {
    char board[BOARD_SIZE][BOARD_SIZE];
} Board;

/*
 * BoardList structure - dynamic array to store multiple boards
 * Contains:
 * - boards: pointer to array of Board structures
 * - count: current number of boards stored
 * - capacity: maximum capacity before needing to resize
 */
typedef struct {
    Board* boards;
    int count;
    int capacity;
} BoardList;

/*
 * Initialize a BoardList with specified initial capacity
 * @param list: Pointer to BoardList to initialize
 * @param initial_capacity: Starting size of the array
 */
void init_board_list(BoardList* list, int initial_capacity) {
    list->boards = (Board*)malloc(initial_capacity * sizeof(Board));
    list->count = 0;
    list->capacity = initial_capacity;
}

/*
 * Free memory allocated for a BoardList
 * @param list: Pointer to BoardList to free
 */
void free_board_list(BoardList* list) {
    free(list->boards);
    list->boards = NULL;
    list->count = 0;
    list->capacity = 0;
}

/*
 * Add a board to a BoardList, resizing if necessary
 * @param list: Pointer to BoardList to modify
 * @param board: Board to add to the list
 */
void add_to_board_list(BoardList* list, Board board) {
    // Double capacity if needed
    if (list->count >= list->capacity) {
        list->capacity *= 2;
        list->boards = (Board*)realloc(list->boards, list->capacity * sizeof(Board));
    }
    list->boards[list->count++] = board;
}

/*
 * Create and return the initial checkers board setup
 * @return: Board structure with starting position
 */
Board initial_board() {
    Board board;
    
    // Initialize all squares as empty
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            board.board[row][col] = '.';
        }
    }

    // Place black pieces ('b') on dark squares in rows 5-7
    for (int row = 5; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if ((row + col) % 2 == 1) {  // Dark squares only
                board.board[row][col] = 'b';
            }
        }
    }

    // Place red pieces ('r') on dark squares in rows 0-2
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 8; col++) {
            if ((row + col) % 2 == 1) {  // Dark squares only
                board.board[row][col] = 'r';
            }
        }
    }

    return board;
}

/*
 * Create a deep copy of a board
 * @param original: Board to copy
 * @return: New independent copy of the board
 */
Board copy_board(Board original) {
    Board new_board;
    memcpy(&new_board.board, &original.board, BOARD_SIZE * BOARD_SIZE * sizeof(char));
    return new_board;
}

/*
 * Compare two boards for equality
 * @param a: First board to compare
 * @param b: Second board to compare
 * @return: true if boards are identical, false otherwise
 */
bool boards_equal(Board a, Board b) {
    return memcmp(a.board, b.board, BOARD_SIZE * BOARD_SIZE * sizeof(char)) == 0;
}

/*
 * Print a board to console
 * @param board: Board to display
 */
void print_board(Board board) {
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            printf("%c ", board.board[row][col]);
        }
        printf("\n");
    }
    printf("\n");
}

/*
 * Recursively find all capture moves from a specific position
 * @param board: Current board state
 * @param start_row: Row of piece to move
 * @param start_col: Column of piece to move
 * @param player: Current player ('b' or 'r')
 * @param captures: BoardList to store resulting board states
 */
void get_captures_from_position(Board board, int start_row, int start_col, char player, BoardList* captures) {
    char piece = board.board[start_row][start_col];
    bool is_king = (piece >= 'A' && piece <= 'Z');  // Kings are uppercase
    char opponent = (player == 'b') ? 'r' : 'b';

    // Possible movement directions (4 for kings, 2 for regular pieces)
    int directions[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
    int num_directions = is_king ? 4 : 2;

    for (int i = 0; i < num_directions; i++) {
        int dr = directions[i][0];
        int dc = directions[i][1];

        // Adjacent and jump positions
        int adj_row = start_row + dr;
        int adj_col = start_col + dc;
        int jump_row = start_row + 2 * dr;
        int jump_col = start_col + 2 * dc;

        // Check if positions are within bounds
        if (adj_row < 0 || adj_row >= BOARD_SIZE || adj_col < 0 || adj_col >= BOARD_SIZE) continue;
        if (jump_row < 0 || jump_row >= BOARD_SIZE || jump_col < 0 || jump_col >= BOARD_SIZE) continue;

        // Check if adjacent piece is opponent and jump position is empty
        char adj_piece = board.board[adj_row][adj_col];
        if (tolower(adj_piece) != opponent || board.board[jump_row][jump_col] != '.') continue;

        // Create new board state after capture
        Board new_board = copy_board(board);
        new_board.board[start_row][start_col] = '.';  // Remove moving piece
        new_board.board[adj_row][adj_col] = '.';      // Remove captured piece

        // King promotion if reaching opponent's first row
        char new_piece = piece;
        if ((player == 'b' && jump_row == 0) || (player == 'r' && jump_row == 7)) {
            new_piece = toupper(player);
        }
        new_board.board[jump_row][jump_col] = new_piece;

        // Recursively check for further captures
        BoardList further_captures;
        init_board_list(&further_captures, 10);
        get_captures_from_position(new_board, jump_row, jump_col, player, &further_captures);

        if (further_captures.count > 0) {
            // Add all further capture sequences
            for (int j = 0; j < further_captures.count; j++) {
                add_to_board_list(captures, further_captures.boards[j]);
            }
        } else {
            // Add this capture if no further captures available
            add_to_board_list(captures, new_board);
        }

        free_board_list(&further_captures);
    }
}

/*
 * Generate all possible capture moves for a player
 * @param board: Current board state
 * @param player: Player to generate moves for ('b' or 'r')
 * @param captures: BoardList to store resulting board states
 */
void generate_captures(Board board, char player, BoardList* captures) {
    // Check every board position
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            char piece = board.board[row][col];
            if (tolower(piece) == player) {  // Check if piece belongs to player
                get_captures_from_position(board, row, col, player, captures);
            }
        }
    }
}

/*
 * Generate all possible regular (non-capture) moves for a player
 * @param board: Current board state
 * @param player: Player to generate moves for ('b' or 'r')
 * @param moves: BoardList to store resulting board states
 */
void generate_regular_moves(Board board, char player, BoardList* moves) {
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            char piece = board.board[row][col];
            if (tolower(piece) != player) continue;  // Skip non-player pieces

            bool is_king = (piece >= 'A' && piece <= 'Z');
            int directions[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
            int num_directions = is_king ? 4 : 2;

            // Adjust directions for regular pieces based on color
            if (!is_king) {
                if (player == 'b') {
                    // Black moves upward
                    directions[0][0] = -1; directions[0][1] = -1;
                    directions[1][0] = -1; directions[1][1] = 1;
                } else {
                    // Red moves downward
                    directions[0][0] = 1; directions[0][1] = -1;
                    directions[1][0] = 1; directions[1][1] = 1;
                }
            }

            // Check each possible direction
            for (int i = 0; i < num_directions; i++) {
                int dr = directions[i][0];
                int dc = directions[i][1];
                int new_row = row + dr;
                int new_col = col + dc;

                // Check if new position is valid and empty
                if (new_row < 0 || new_row >= BOARD_SIZE || new_col < 0 || new_col >= BOARD_SIZE) continue;
                if (board.board[new_row][new_col] != '.') continue;

                // Create new board state after move
                Board new_board = copy_board(board);
                new_board.board[row][col] = '.';  // Remove piece from old position

                // King promotion if reaching opponent's first row
                char new_piece = piece;
                if ((player == 'b' && new_row == 0) || (player == 'r' && new_row == 7)) {
                    new_piece = toupper(player);
                }
                new_board.board[new_row][new_col] = new_piece;

                add_to_board_list(moves, new_board);
            }
        }
    }
}

/*
 * Generate all possible moves for a player (captures first if available)
 * @param board: Current board state
 * @param player: Player to generate moves for ('b' or 'r')
 * @param result: BoardList to store resulting board states
 */
void get_all_possible_moves(Board board, char player, BoardList* result) {
    BoardList captures;
    init_board_list(&captures, 10);
    generate_captures(board, player, &captures);

    // Check if any captures exist (mandatory capture rule)
    if (captures.count > 0) {
        for (int i = 0; i < captures.count; i++) {
            add_to_board_list(result, captures.boards[i]);
        }
    } else {
        // Only generate regular moves if no captures available
        generate_regular_moves(board, player, result);
    }

    free_board_list(&captures);
}

/*
 * Generate all possible board states after a specified number of moves
 * Uses BFS to explore all possible move sequences
 * @param initial_board: Starting board state
 * @param num_moves: Number of moves to simulate
 * @param result: BoardList to store resulting board states
 */
void generate_boards_after_moves(Board initial_board, int num_moves, BoardList* result) {
    // Queue item for BFS
    typedef struct {
        Board board;
        int depth;
    } QueueItem;

    // Initialize queue
    int queue_capacity = 1000;
    QueueItem* queue = (QueueItem*)malloc(queue_capacity * sizeof(QueueItem));
    int front = 0, rear = 0;

    // Start with initial board at depth 0
    queue[rear++] = (QueueItem){initial_board, 0};
    if (rear >= queue_capacity) {
        queue_capacity *= 2;
        queue = (QueueItem*)realloc(queue, queue_capacity * sizeof(QueueItem));
    }

    // Process queue
    while (front < rear) {
        QueueItem current = queue[front++];
        
        // If reached desired depth, add to results
        if (current.depth == num_moves) {
            add_to_board_list(result, current.board);
            continue;
        }

        // Determine current player (alternates with depth)
        char current_player = (current.depth % 2 == 0) ? 'b' : 'r';
        
        // Generate all possible next moves
        BoardList next_boards;
        init_board_list(&next_boards, 10);
        get_all_possible_moves(current.board, current_player, &next_boards);

        // Enqueue all resulting boards
        for (int i = 0; i < next_boards.count; i++) {
            if (rear >= queue_capacity) {
                queue_capacity *= 2;
                queue = (QueueItem*)realloc(queue, queue_capacity * sizeof(QueueItem));
            }
            queue[rear++] = (QueueItem){next_boards.boards[i], current.depth + 1};
        }

        free_board_list(&next_boards);
    }

    free(queue);
}

int main() {
    // Initialize and display starting board
    Board board = initial_board();
    printf("Initial board:\n");
    print_board(board);

    // Generate possible boards after specified moves
    int num_moves = 3;
    BoardList possible_boards;
    init_board_list(&possible_boards, 100);
    generate_boards_after_moves(board, num_moves, &possible_boards);

    // Display results
    printf("Number of possible boards after %d moves: %d\n", num_moves, possible_boards.count);
    
    // Print first few boards for demonstration
    int boards_to_print = (possible_boards.count < 3) ? possible_boards.count : 3;
    for (int i = 0; i < boards_to_print; i++) {
        printf("Board %d:\n", i+1);
        print_board(possible_boards.boards[i]);
    }

    // Clean up
    free_board_list(&possible_boards);
    return 0;
}