#ifndef CELL_H
#define CELL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <ncurses.h>

// Terminal colors
#define TERM_RESET "\033[0m"
#define TERM_RED   "\033[31m"
#define PRINT_ERROR(err) printf("%sERROR:%s %s\n", TERM_RED, TERM_RESET, err);

#define MAX_SURROUNDINGS 8

// NCurses colors:
#define NC_RED       1
#define NC_RED_BG    2
#define NC_BLUE      3
#define NC_BLUE_BG   4
#define NC_GREEN     5
#define NC_GREEN_BG  6
#define NC_YELLOW    7
#define NC_YELLOW_BG 8
#define NC_MAGENTA   9
#define NC_WHITE_BG  10

typedef struct board_t board_t;

typedef struct {
    uint8_t row, column;
} position_t;

typedef struct {
    bool bomb, hidden, flagged, marked;
    uint8_t nearby_bombs;
    position_t position;
    board_t* p_board;
} cell_t;

/*
 * The following function puts default parameters on a given cell.
 * Input: A pointer to the cell, it's position and the board it
 * belongs to.
 * Output: None.
 */
void init_cell(cell_t* p_cell, position_t position, board_t* p_board);

/*
 * The following function puts all the cells surrounding a given cell
 * in an array.
 * Input: A pointer to the cell and an array to store it's surroundings.
 * Output: The amount of cells around the given cell.
 */
uint8_t get_surrounding_cells(cell_t* p_cell, cell_t* surroundings[MAX_SURROUNDINGS]);

/*
 * The following function prints out a specific cell
 * according to it's fields.
 * Input: The cell and a pointer to the ncurses window.
 * Output: None.
 */
void display_cell(cell_t cell, WINDOW* window);

#endif
