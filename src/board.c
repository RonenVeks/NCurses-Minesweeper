#include "board.h"
#include <time.h>

/*
 * The following function acts as a map, translating
 * board sizes to the according bombs amount.
 * Output: The correct amount of bombs according
 * to the size of the board.
 */
static uint8_t size_to_bombs(board_size_t board_size) {
    switch (board_size) {
    case EASY:   return 10;
    case MEDIUM: return 40;
    case HARD:   return 99;
    default: return 1;
    }
}

/*
 * The following function puts bombs on a given board.
 * Input: A pointer to the board.
 * Output: None.
 */
static void put_bombs(board_t* p_board) {
    cell_t* pot_bomb, *surroundings[MAX_SURROUNDINGS];
    uint8_t bombs_put = 0;
    int8_t cells_found;

    srand(time(NULL)); // Refresh rand() funciton seed
    while (bombs_put < p_board->bombs_amount) {
        pot_bomb = &p_board->matrix[rand() % p_board->size][rand() % p_board->size];

        if (!pot_bomb->bomb) {
            pot_bomb->bomb = true;
            cells_found = get_surrounding_cells(pot_bomb, surroundings);
            while (cells_found > 0) {
                surroundings[cells_found - 1]->nearby_bombs++;
                cells_found--;
            }
            p_board->bombs[bombs_put++] = pot_bomb;
        }
    }
}

board_t* create_board(board_size_t board_size) {
    int screen_height, screen_width;
    const int WINDOW_HEIGHT = board_size + 2, WINDOW_WIDTH = board_size * 2 + 3;
    board_t* new_board = (board_t*)malloc(sizeof(board_t));
    if (!new_board) goto alloc_failure;

    new_board->size = board_size;
    new_board->bombs_amount = size_to_bombs(board_size);
    if (new_board->bombs_amount == 1) {
        free(new_board);
        PRINT_ERROR("False input");
        return NULL;
    }

    new_board->bombs = (cell_t**)calloc(new_board->bombs_amount, sizeof(cell_t*));
    if (!new_board->bombs) goto free_board;

    getmaxyx(stdscr, screen_height, screen_width);
    new_board->nc_win = newwin(WINDOW_HEIGHT,
                               WINDOW_WIDTH,
                               screen_height / 2 - WINDOW_HEIGHT / 2,
                               screen_width / 2 - WINDOW_WIDTH / 2);
    if (!new_board->nc_win) goto free_bombs;
    box(new_board->nc_win, 0, 0);

    new_board->matrix = (cell_t**)calloc((uint8_t)board_size, sizeof(cell_t*));
    if (!new_board->matrix) goto free_nc_window;
    
    uint8_t column;
    for (uint8_t row = 0; row < board_size; row++) {
        new_board->matrix[row] = (cell_t*)calloc((uint8_t)board_size, sizeof(cell_t));
        if (!new_board->matrix[row]) {
            for (uint8_t temp = 0; temp < row; temp++)
                free(new_board->matrix[temp]);
            free(new_board->matrix);
            goto free_nc_window;
        }

        for (column = 0; column < board_size; column++)
            init_cell(&new_board->matrix[row][column], (position_t){ row, column }, new_board);
    }

    // Initial marked cell
    new_board->mark = &new_board->matrix[0][0];
    new_board->mark->marked = true;

    put_bombs(new_board);
    return new_board;

free_nc_window:
    delwin(new_board->nc_win);
    touchwin(stdscr);
free_bombs:
    free(new_board->bombs);
free_board:
    free(new_board);
alloc_failure:
    PRINT_ERROR("Memory allocation failed");
    return NULL;
}

void free_board(board_t* p_board) {
    for (uint8_t row = 0; row < p_board->size; row++)
        free(p_board->matrix[row]);
    free(p_board->matrix);
    delwin(p_board->nc_win);
    touchwin(stdscr);
    free(p_board->bombs);
    free(p_board);
}

void display_board(board_t board, WINDOW* window) {
    uint8_t column;
    
    for (uint8_t row = 0; row < board.size; row++) {
        wmove(window, row + 1, 1);
        wattron(window, COLOR_PAIR(NC_YELLOW));
        mvwprintw(window, row + 1, 1, "|");
        wattroff(window, COLOR_PAIR(NC_YELLOW));
        for (column = 0; column < board.size; column++)
            display_cell(board.matrix[row][column], window);
    }
}

void reveal_board(board_t* p_board) {
    uint8_t column;

    p_board->mark->marked = false;

    for (uint8_t row = 0; row < p_board->size; row++)
        for (column = 0; column < p_board->size; column++)
            p_board->matrix[row][column].hidden = false;
}

void move_mark(board_t* p_board, uint8_t row, uint8_t column) {
    p_board->mark->marked = false;
    p_board->mark = &p_board->matrix[row][column];
    p_board->mark->marked = true;
}

bool check_win(board_t* p_board) {
    for (uint8_t bomb = 0; bomb < p_board->bombs_amount; bomb++)
        if (!p_board->bombs[bomb]->flagged)
            return false;
    return true;
}

/*
 * The following function opens water area recursively.
 * Input: A pointer to the board and a pointer to the openning cell.
 * Output: None.
 */
static void open_water_cell(board_t* p_board, cell_t* p_cell) {
    if (p_cell->hidden && !p_cell->flagged) {
        p_cell->hidden = false;

        if (p_cell->nearby_bombs == 0) {
            cell_t* surroundings[MAX_SURROUNDINGS];
            uint8_t found = get_surrounding_cells(p_cell, surroundings);

            for (uint8_t cell = 0; cell < found; cell++)
                if (surroundings[cell]->hidden)
                    open_water_cell(p_board, surroundings[cell]);
        }
    }
}

static bool is_correctly_flagged(cell_t* p_cell) {
    cell_t* surroundings[MAX_SURROUNDINGS];
    uint8_t count = 0, found = get_surrounding_cells(p_cell, surroundings);

    for (uint8_t cell = 0; cell < found; cell++)
        if (surroundings[cell]->flagged) count++;

    return count == p_cell->nearby_bombs;
}

bool open_cell(board_t* p_board, cell_t* p_cell) {
    if (p_cell->flagged) return true;

    if (p_cell->bomb) return false;

    if (p_cell->hidden)
        p_cell->nearby_bombs == 0 ? open_water_cell(p_board, p_cell) : (p_cell->hidden = false);

    else if (is_correctly_flagged(p_cell)) {
        cell_t* surroundings[MAX_SURROUNDINGS];
        uint8_t found = get_surrounding_cells(p_cell, surroundings);

        for (uint8_t cell = 0; cell < found; cell++)
            if (surroundings[cell]->hidden && !surroundings[cell]->flagged)
                if (!open_cell(p_board, surroundings[cell])) return false;
    }

    return true;
}
