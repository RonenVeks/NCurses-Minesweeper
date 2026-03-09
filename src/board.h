#ifndef BOARD_H
#define BOARD_H

#include "cell.h"

typedef enum {
    EASY =   9,
    MEDIUM = 16,
    HARD =   25
} board_size_t;

typedef struct board_t{
    cell_t** matrix, **bombs, *mark;
    board_size_t size;
    uint8_t bombs_amount;
    WINDOW* nc_win;
} board_t;

/*
 * The following function initializes a new board according
 * to a given size.
 * Input: The size of the board.
 * Output: A pointer to the new board.
 */
board_t* create_board(board_size_t board_size);

/*
 * The following function frees the allocation of a given
 * board and all of it's fields.
 * Input: A pointer to the board.
 * Output: None.
 */
void free_board(board_t* p_board);

/*
 * The following function prints out the board on a given window.
 * Input: The board and the NCurses window.
 * Ouput: None.
 */
void display_board(board_t board, WINDOW* window);

/*
 * The following function un-hides all the cells on a given board.
 * Input: A pointer to the board.
 * Output: None.
 */
void reveal_board(board_t* p_board);

/*
 * The following function moves a given board's mark to a different
 * given position.
 * Input: A pointer to the board, the new row and the new column.
 * Output: None.
 */
void move_mark(board_t* p_board, uint8_t row, uint8_t column);

/*
 * The following function check whether or not there's a flag
 * on each and every one of the given board's bombs.
 * Input: A pointer to the board.
 * Output: A boolean function that indicates whether or not
 * the game ended.
 */
bool check_win(board_t* p_board);

/*
 * The following function opens a cell according to it's fields.
 * Input: A pointer to the player's board, and a pointer to the cell.
 * Output: A boolean value that indicates whether or not the
 * cell openning went successfully (player didn't lose).
 */
bool open_cell(board_t* p_board, cell_t* p_cell);

#endif
