#ifndef TTT_H
#define TTT_H

typedef enum {
    CELL_EMPTY = 0,
    CELL_X = 1,
    CELL_O = 2
} Cell;

typedef struct {
    Cell board[3][3];
    Cell turn;
} TttGame;

TttGame *ttt_create(void);
void ttt_free(TttGame *game);
int ttt_play(TttGame *game, int position);
Cell ttt_winner(const TttGame *game);
int ttt_is_full(const TttGame *game);
int ttt_is_over(const TttGame *game);

#endif
