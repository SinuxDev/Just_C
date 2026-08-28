#include "ttt.h"

#include <stdio.h>

static char cell_label(const TttGame *game, int position) {
    int row = (position - 1) / 3;
    int col = (position - 1) % 3;
    Cell cell = game->board[row][col];

    if (cell == CELL_X) {
        return 'X';
    }
    if (cell == CELL_O) {
        return 'O';
    }
    return (char)('0' + position);
}

static void print_board(const TttGame *game) {
    printf("\n");
    for (int r = 0; r < 3; r++) {
        printf(" %c | %c | %c\n", cell_label(game, r * 3 + 1),
               cell_label(game, r * 3 + 2), cell_label(game, r * 3 + 3));
        if (r < 2) {
            printf("---+---+---\n");
        }
    }
    printf("\n");
}

static char player_mark(Cell cell) {
    return cell == CELL_X ? 'X' : 'O';
}

int main(void) {
    TttGame *game = ttt_create();
    if (!game) {
        fprintf(stderr, "Could not start game.\n");
        return 1;
    }

    printf("Tic-tac-toe: enter a number 1-9 to place your mark.\n");
    print_board(game);

    while (!ttt_is_over(game)) {
        int position = 0;
        printf("Player %c, choose 1-9: ", player_mark(game->turn));
        if (scanf("%d", &position) != 1) {
            fprintf(stderr, "Please enter a number.\n");
            ttt_free(game);
            return 1;
        }

        if (!ttt_play(game, position)) {
            printf("That move is not allowed. Try again.\n");
            continue;
        }
        print_board(game);
    }

    Cell winner = ttt_winner(game);
    if (winner == CELL_EMPTY) {
        printf("Draw.\n");
    } else {
        printf("Player %c wins!\n", player_mark(winner));
    }

    ttt_free(game);
    return 0;
}
