#include "ttt.h"

#include <stdio.h>
#include <stdlib.h>

static int g_passed = 0;
static int g_failed = 0;

#define EXPECT(cond, msg)                                                      \
    do {                                                                       \
        if (cond) {                                                            \
            g_passed++;                                                        \
        } else {                                                               \
            g_failed++;                                                        \
            fprintf(stderr, "FAIL: %s (%s:%d)\n", msg, __FILE__, __LINE__);    \
        }                                                                      \
    } while (0)

static void test_create_empty_board(void) {
    TttGame *game = ttt_create();

    EXPECT(game != NULL, "create returns a game");
    if (!game) {
        return;
    }

    EXPECT(game->turn == CELL_X, "X moves first");
    EXPECT(game->board[0][0] == CELL_EMPTY, "top-left starts empty");
    EXPECT(game->board[1][1] == CELL_EMPTY, "center starts empty");
    EXPECT(ttt_winner(game) == CELL_EMPTY, "no winner at start");
    EXPECT(ttt_is_full(game) == 0, "board is not full");
    EXPECT(ttt_is_over(game) == 0, "game is not over");
    ttt_free(game);
}

static void test_play_places_and_switches_turn(void) {
    TttGame *game = ttt_create();
    EXPECT(game != NULL, "create returns a game");
    if (!game) {
        return;
    }

    EXPECT(ttt_play(game, 5) == 1, "center move succeeds");
    EXPECT(game->board[1][1] == CELL_X, "center is X");
    EXPECT(game->turn == CELL_O, "turn switches to O");
    EXPECT(ttt_play(game, 1) == 1, "top-left move succeeds");
    EXPECT(game->board[0][0] == CELL_O, "top-left is O");
    EXPECT(game->turn == CELL_X, "turn switches back to X");

    ttt_free(game);
}

static void test_play_rejects_bad_moves(void) {
    TttGame *game = ttt_create();
    EXPECT(game != NULL, "create returns a game");
    if (!game) {
        return;
    }

    EXPECT(ttt_play(game, 0) == 0, "position 0 is rejected");
    EXPECT(ttt_play(game, 10) == 0, "position 10 is rejected");
    EXPECT(ttt_play(game, 1) == 1, "first move on 1 succeeds");
    EXPECT(ttt_play(game, 1) == 0, "occupied cell is rejected");
    EXPECT(game->board[0][0] == CELL_X, "cell stays X");

    ttt_free(game);
}

static void test_row_column_and_diagonal_wins(void) {
    TttGame *game = ttt_create();
    EXPECT(game != NULL, "create returns a game");
    if (!game) {
        return;
    }

    /* X: 1,2,3  O: 4,5 */
    ttt_play(game, 1);
    ttt_play(game, 4);
    ttt_play(game, 2);
    ttt_play(game, 5);
    ttt_play(game, 3);
    EXPECT(ttt_winner(game) == CELL_X, "top row win for X");
    EXPECT(ttt_is_over(game) == 1, "game is over after a win");
    EXPECT(ttt_play(game, 6) == 0, "moves are rejected after a win");
    ttt_free(game);

    game = ttt_create();
    EXPECT(game != NULL, "create returns a game");
    if (!game) {
        return;
    }
    /* X: 1,5,9  O: 2,3 — diagonal */
    ttt_play(game, 1);
    ttt_play(game, 2);
    ttt_play(game, 5);
    ttt_play(game, 3);
    ttt_play(game, 9);
    EXPECT(ttt_winner(game) == CELL_X, "diagonal win for X");
    ttt_free(game);

    game = ttt_create();
    EXPECT(game != NULL, "create returns a game");
    if (!game) {
        return;
    }
    /* O wins column 1: X 5,6,9  O 1,4,7 */
    ttt_play(game, 5);
    ttt_play(game, 1);
    ttt_play(game, 6);
    ttt_play(game, 4);
    ttt_play(game, 9);
    ttt_play(game, 7);
    EXPECT(ttt_winner(game) == CELL_O, "left column win for O");
    ttt_free(game);
}

static void test_draw(void) {
    TttGame *game = ttt_create();
    EXPECT(game != NULL, "create returns a game");
    if (!game) {
        return;
    }

    /* X O X
       X X O
       O X O */
    int moves[] = {1, 2, 3, 6, 4, 7, 5, 9, 8};
    for (int i = 0; i < 9; i++) {
        EXPECT(ttt_play(game, moves[i]) == 1, "draw-path move succeeds");
    }

    EXPECT(ttt_winner(game) == CELL_EMPTY, "draw has no winner");
    EXPECT(ttt_is_full(game) == 1, "board is full");
    EXPECT(ttt_is_over(game) == 1, "draw ends the game");

    ttt_free(game);
}

int main(void) {
    test_create_empty_board();
    test_play_places_and_switches_turn();
    test_play_rejects_bad_moves();
    test_row_column_and_diagonal_wins();
    test_draw();

    printf("%d passed, %d failed\n", g_passed, g_failed);
    return g_failed == 0 ? 0 : 1;
}
