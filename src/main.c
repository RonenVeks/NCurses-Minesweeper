#include "board.h"

/*
 * The following function just initalizes all NCurses
 * necessary color pairs for the program.
 * Input: None.
 * Ouput: None.
 */
static inline void init_ncurses_pairs() {
    init_pair(NC_RED,       COLOR_RED, COLOR_BLACK);
    init_pair(NC_RED_BG,    COLOR_BLACK, COLOR_RED);
    init_pair(NC_BLUE,      COLOR_BLUE, COLOR_BLACK);
    init_pair(NC_BLUE_BG,   COLOR_BLACK, COLOR_BLUE);
    init_pair(NC_GREEN,     COLOR_GREEN, COLOR_BLACK);
    init_pair(NC_GREEN_BG,  COLOR_BLACK, COLOR_GREEN);
    init_pair(NC_YELLOW,    COLOR_YELLOW, COLOR_BLACK);
    init_pair(NC_YELLOW_BG, COLOR_BLACK, COLOR_YELLOW);
    init_pair(NC_MAGENTA,   COLOR_MAGENTA, COLOR_BLACK);
    init_pair(NC_WHITE_BG,  COLOR_BLACK, COLOR_WHITE);
}

/*
 * The following function prints out the logo ascii art.
 * Input: The window on which the logo will be drawn upon.
 * Output: None.
 */
static inline void print_logo(WINDOW* window) {
    const uint8_t LOGO_HEIGHT = 7;
	const char* logo[] = {
		" ___ ___  ____  ____     ___  _____   ___    ___  ____    ___  ____  ",
		"|   T   Tl    j|    \\   /  _]/ ___/  /  _]  /  _]|    \\  /  _]|    \\ ",
		"| _   _ | |  T |  _  Y /  [_(   \\_  /  [_  /  [_ |  o  )/  [_ |  D  )",
		"|  \\_/  | |  | |  |  |Y    _]\\__  TY    _]Y    _]|   _/Y    _]|    / ",
		"|   |   | |  | |  |  ||   [_ /  \\ ||   [_ |   [_ |  |  |   [_ |    \\ ",
		"|   |   | j  l |  |  ||     T\\    ||     T|     T|  |  |     T|  .  Y",
		"l___j___j|____jl__j__jl_____j \\___jl_____jl_____jl__j  l_____jl__j\\_j"
	};

    wattron(window, COLOR_PAIR(NC_MAGENTA));
    for (uint8_t i = 0; i < LOGO_HEIGHT; i++)
        mvwprintw(window, i + 1, 26, "%s", logo[i]);
    wattroff(window, COLOR_PAIR(NC_MAGENTA));
}

/*
 * The following function displays a nice menu in which
 * the user can choose their preferred game difficulty.
 * Input: None.
 * Ouput: The user's preferred size for the board.
 */
board_size_t get_board_size() {
    bool chose = false;
    uint8_t option = 1;
    int screen_height, screen_width, key;
    const int WINDOW_HEIGHT = 20, WINDOW_WIDTH = 100;

    board_t* example_board = create_board(EASY);
    if (!example_board) return 0;
    reveal_board(example_board);

    // Initialize ncurses window
    getmaxyx(stdscr, screen_height, screen_width);
    WINDOW* window = newwin(
        WINDOW_HEIGHT,
        WINDOW_WIDTH,
        screen_height / 2 - WINDOW_HEIGHT / 2,
        screen_width / 2 - WINDOW_WIDTH / 2
    );
    if (!window) {
        free_board(example_board);
        PRINT_ERROR("Memory allocation failed");
        return 0;
    }

    refresh();
    box(window, 0, 0);

    while (!chose) {
        display_board(*example_board, window);
        print_logo(window);

        // Displaying board sizes
        wattron(window, COLOR_PAIR(NC_YELLOW_BG));
        mvwprintw(window, 15, 1, "Game Modes:");
        wattroff(window, COLOR_PAIR(NC_YELLOW_BG));
        for (uint8_t i = 1; i <= 3; i++) {
            wattron(window, COLOR_PAIR(option == i ? NC_RED_BG : NC_RED));
            mvwprintw(window, 15 + i, 5, "%d)", i);
            wattroff(window, COLOR_PAIR(option == i ? NC_RED_BG : NC_RED));

            wattron(window, COLOR_PAIR(option == i ? NC_GREEN_BG: NC_GREEN));
            wprintw(window, i == 1 ? " Easy - 9 cells" : i == 2 ? " Medium - 16 cells" : " Hard - 25 cells");
            wattroff(window, COLOR_PAIR(option == i ? NC_GREEN_BG: NC_GREEN));
        }

        wrefresh(window);

        // User input
        key = getch();
        switch(key) {
        case KEY_UP:
            if (option > 1) option--;
            break;
        case KEY_DOWN:
            if (option < 3) option++;
            break;
        case KEY_ENTER:
        case '\n':
            chose = true;
            break;
        }
    }

    free_board(example_board);

    delwin(window);
    touchwin(stdscr);

    switch(option) {
    case 1:  return EASY;
    case 2:  return MEDIUM;
    case 3:  return HARD;
    default: return 1;
    }
}

/*
 * The following function is the game loop.
 * Input: A pointer to the player's board.
 * Ouput: None.
 */
void game_loop(board_t* p_board) {
    bool game = true, won = false;
    uint8_t flags_left = p_board->bombs_amount;
    int screen_height, screen_width, center_y, center_x, key, y_diff, x_diff;
    WINDOW* window = p_board->nc_win;

    getmaxyx(stdscr, screen_height, screen_width);
    center_y = screen_height / 2;
    center_x = screen_width / 2;

    while (game) {
        y_diff = x_diff = 0; // Reset movement difference variables

        display_board(*p_board, window);
        wrefresh(window);

        // Display remaining bombs amount
        attron(COLOR_PAIR(NC_RED));
        mvprintw(center_y - (p_board->size + 2) / 2 - 1, center_x - (p_board->size * 2  + 3) / 2, "Flags Left: %d ", flags_left);
        attroff(COLOR_PAIR(NC_RED));

        // Display addtional functions instructions
        attron(COLOR_PAIR(NC_BLUE));
        mvprintw(center_y + (p_board->size + 2) / 2 + 1, center_x - (p_board->size * 2 + 3) / 2, "(O) Open Cell ");
        attroff(COLOR_PAIR(NC_BLUE));
        attron(COLOR_PAIR(NC_GREEN));
        printw("(F) Put Flag ");
        attroff(COLOR_PAIR(NC_GREEN));
        attron(COLOR_PAIR(NC_MAGENTA));
        printw("(Q) Exit Game");
        attroff(COLOR_PAIR(NC_MAGENTA));
        refresh();

        key = getch();
        switch (key) {
        // Exit game
        case 'Q':
        case 'q':
            return;

        // Movement
        case KEY_UP:
            if (p_board->mark->position.row > 0) y_diff = -1;
            break;
        case KEY_DOWN:
            if (p_board->mark->position.row < p_board->size - 1) y_diff = 1;
            break;
        case KEY_RIGHT:
            if (p_board->mark->position.column < p_board->size - 1) x_diff = 1;
            break;
        case KEY_LEFT:
            if (p_board->mark->position.column > 0) x_diff = -1;
            break;

        // Put flag
        case 'F':
        case 'f':
            if (p_board->mark->flagged) {
                flags_left++;
                p_board->mark->flagged = false;
            } else if (p_board->mark->hidden && flags_left > 0) {
                flags_left--;
                p_board->mark->flagged = true;
                
                if (flags_left == 0 && check_win(p_board)) {
                    game = false;
                    won = true;
                }
            }
            break;

        // Open cell
        case 'O':
        case 'o':
            game = open_cell(p_board, p_board->mark);
            break;
        }

        move_mark(p_board, p_board->mark->position.row + y_diff,
                  p_board->mark->position.column + x_diff);
    }

    werase(stdscr);
    reveal_board(p_board);
    display_board(*p_board, window);
    box(window, 0, 0);

    attron(COLOR_PAIR(won ? NC_GREEN : NC_RED));
    mvprintw(center_y - (p_board->size + 2) / 2 - 1,
             center_x - (p_board->size * 2  + 3) / 2,
             won ? "YOU WON!!!" : "You lost...");
    attroff(COLOR_PAIR(won ? NC_GREEN : NC_RED));
    
    mvprintw(center_y + (p_board->size + 2) / 2 + 1,
             center_x - (p_board->size * 2 + 3) / 2,
             "Press any key to exit.");

    wnoutrefresh(stdscr);
    wnoutrefresh(window);
    doupdate();

    flushinp();
    nodelay(stdscr, false);
    getch();
}

int main(void) {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);
    start_color();

    init_ncurses_pairs();

    board_size_t board_size = get_board_size();
    if (board_size == 1) {
        PRINT_ERROR("False input");
        return 1;
    } else if (board_size == 0) return 1;
    refresh();

    board_t* p_board = create_board(board_size);
    if (!p_board) return 1;
    
    game_loop(p_board);

    free_board(p_board);

    endwin();
    return 0;
}
