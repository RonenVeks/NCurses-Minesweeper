#include "cell.h"
#include "board.h"

void init_cell(cell_t* p_cell, position_t position, board_t* p_board) {
    p_cell->bomb = false;
    p_cell->hidden = true;
    p_cell->flagged = false;
    p_cell->nearby_bombs = 0;
    p_cell->position = position;
    p_cell->p_board = p_board;
}

uint8_t get_surrounding_cells(cell_t* p_cell, cell_t* surroundings[MAX_SURROUNDINGS]) {
    uint8_t found = 0, board_size = p_cell->p_board->size;
    int8_t fac_row, fac_column;

    for (int8_t row = -1; row <= 1; row++)
        for (int8_t column = -1; column <= 1; column++) {
            if (row == 0 && column == 0) continue;

            fac_row = p_cell->position.row + row;
            fac_column = p_cell->position.column + column;

            if (fac_row >= 0 && fac_row < board_size &&
                fac_column >= 0 && fac_column < board_size)
                surroundings[found++] = &p_cell->p_board->matrix[fac_row][fac_column];
        }

    return found;
}

void display_cell(cell_t cell, WINDOW* window) {
    if (cell.hidden) {
        if (cell.flagged) {
            wattron(window, COLOR_PAIR(cell.marked ? NC_RED_BG : NC_RED));
            wprintw(window, "F");
            wattroff(window, COLOR_PAIR(cell.marked ? NC_RED_BG : NC_RED));
        } else {
            if (cell.marked) wattron(window, COLOR_PAIR(NC_WHITE_BG));
            wprintw(window, " ");
            if (cell.marked) wattroff(window, COLOR_PAIR(NC_WHITE_BG));
        }
    } else if (cell.bomb) {
        wattron(window, COLOR_PAIR(cell.marked ? NC_RED_BG : NC_RED));
        wprintw(window, "#");
        wattroff(window, COLOR_PAIR(cell.marked ? NC_RED_BG : NC_RED));
    } else if (cell.nearby_bombs == 0) {
        wattron(window, COLOR_PAIR(cell.marked ? NC_BLUE_BG : NC_BLUE));
        wprintw(window, "~");
        wattroff(window, COLOR_PAIR(cell.marked ? NC_BLUE_BG : NC_BLUE));
    } else {
        wattron(window, COLOR_PAIR(cell.marked ? NC_GREEN_BG : NC_GREEN));
        wprintw(window, "%d", cell.nearby_bombs);
        wattroff(window, COLOR_PAIR(cell.marked ? NC_GREEN_BG : NC_GREEN));
    }
    wattron(window, COLOR_PAIR(NC_YELLOW));
    wprintw(window, "|");
    wattroff(window, COLOR_PAIR(NC_YELLOW));
}
