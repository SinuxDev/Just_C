#include "ttt.h"

#include <stdlib.h>

static void position_to_cell(int position, int *row, int *col) {
    *row = (position - 1) / 3;
    *col = (position - 1) % 3;
}

TttGame *ttt_create(void) {
    TttGame *game = calloc(1, sizeof(TttGame));
    if (game) {
        game->turn = CELL_X;
    }
    return game;
}

void ttt_free(TttGame *game) {
    free(game);
}

static Cell line_winner(Cell a, Cell b, Cell c) {
    if (a != CELL_EMPTY && a == b && b == c) {
        return a;
    }
    return CELL_EMPTY;
}

Cell ttt_winner(const TttGame *game) {
    if (!game) {
        return CELL_EMPTY;
    }

    for (int i = 0; i < 3; i++) {
        Cell row = line_winner(game->board[i][0], game->board[i][1],
                               game->board[i][2]);
        if (row != CELL_EMPTY) {
            return row;
        }
        Cell col = line_winner(game->board[0][i], game->board[1][i],
                               game->board[2][i]);
        if (col != CELL_EMPTY) {
            return col;
        }
    }

    Cell main_diag =
        line_winner(game->board[0][0], game->board[1][1], game->board[2][2]);
    if (main_diag != CELL_EMPTY) {
        return main_diag;
    }
    return line_winner(game->board[0][2], game->board[1][1], game->board[2][0]);
}

int ttt_is_full(const TttGame *game) {
    if (!game) {
        return 0;
    }

    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            if (game->board[r][c] == CELL_EMPTY) {
                return 0;
            }
        }
    }
    return 1;
}

int ttt_is_over(const TttGame *game) {
    return ttt_winner(game) != CELL_EMPTY || ttt_is_full(game);
}

int ttt_play(TttGame *game, int position) {
    if (!game || position < 1 || position > 9 || ttt_is_over(game)) {
        return 0;
    }

    int row = 0;
    int col = 0;
    position_to_cell(position, &row, &col);
    if (game->board[row][col] != CELL_EMPTY) {
        return 0;
    }

    game->board[row][col] = game->turn;
    game->turn = (game->turn == CELL_X) ? CELL_O : CELL_X;
    return 1;
}
