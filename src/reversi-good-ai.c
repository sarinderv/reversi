/*
    This file was adapted from template files provided by David Wood.
    This was sourced from Homework 3 of his CS 758 at UW - Madison.
    http://pages.cs.wisc.edu/~david/courses/cs758/Fall2016/wiki/index.php?n=Main.Homework3

    That, in turn, was based off an assignment by John Mellor-Crummey at Rice University.
*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "reversi.h"

#define DEPTH 2

typedef enum { false, true } bool;


Move toMove(ull moves, int moveNum)
{
    // clz returns the bit number of the first bit set, counting from the MSB.
    int highestBit = __builtin_clzll(moves);

    for (int i = 0; i < moveNum; i++)
    {
        // Remove the highest bit from possible moves.
        moves -= ((ull) 1) << (63 - highestBit);
        // Get the next highest bit.
        highestBit = __builtin_clzll(moves);
    }

    Move m = BIT_TO_MOVE(highestBit);
    return m;
}

int tryMove(ull moves, int moveNum, int color, Board *b)
{
    Move m = toMove(moves, moveNum);

    // Set the third parameter to 1, to echo disks flipped.
    int nflips = FlipDisks(m, b, color, 0, 1);
    if (nflips > 0) {
        PlaceOrFlip(m, b, color);
    }

    return nflips;
}

int minimax(Board *b, int depth, int startColor, bool maximizingPlayer, int p)
{
    // alternate color depending on the depth
    int color = maximizingPlayer ? startColor : OTHERCOLOR(startColor);

    Board legal_moves;
    int num_moves = EnumerateLegalMoves(*b, color, &legal_moves);
    if (depth <= 0 || num_moves <= 0) {
        if (maximizingPlayer)
            return CountBitsOnBoard(b, color) - CountBitsOnBoard(b, OTHERCOLOR(color));
        else
            return CountBitsOnBoard(b, OTHERCOLOR(color)) - CountBitsOnBoard(b, color);
    }
//    printf("at depth %d, %c has %d possible moves\n", depth, color ==  X_BLACK ? 'X' : 'O', num_moves);

    ull moves = legal_moves.disks[color];
    int bestMoveNum = 0, bestValue = maximizingPlayer ? INT_MIN : INT_MAX;
    int v[num_moves];
    Board c[num_moves*depth];

    for (int moveNum = 0; moveNum < num_moves; moveNum++) {
        c[num_moves*depth] = *b; // copy /// RACE
        int nFlips = tryMove(moves, moveNum, color, &c[num_moves*depth]);
        if (nFlips != 0) {
            if (p) {
                //int vt; // temp var to avoid race condition
                v[moveNum] = cilk_spawn minimax(&c[num_moves*depth], depth - 1, startColor, !maximizingPlayer, p);  ///RACE
                //v[moveNum] = vt;
            }
            else {
                v[moveNum] = minimax(&c[num_moves*depth], depth - 1, startColor, !maximizingPlayer, p);
            }
        }
    }
    if (p)
        cilk_sync;

    for (int moveNum = 0; moveNum < num_moves; moveNum++) {
        if ((maximizingPlayer && v[moveNum] >= bestValue) ||
           (!maximizingPlayer && v[moveNum] <= bestValue)) {
            bestValue = v[moveNum];
            bestMoveNum = moveNum;
        }
    }

//    Move move = toMove(moves, bestMoveNum);
//    printf("at depth %d, move(%d,%d) for %c yields %s value=%d\n",
//           depth, move.row, move.col,
//           color ==  X_BLACK ? 'X' : 'O', maximizingPlayer ? "max" : "min", bestValue);

    if (depth == DEPTH) // reached the end, update the board now
        tryMove(moves, bestMoveNum, color, b);

    return bestValue;
}

int GoodAITurn(Board *b, int color, int p)
{
    minimax(b, DEPTH, color, true, p);

    Board legal_moves;
    int num_moves = EnumerateLegalMoves(*b, color, &legal_moves);
    return num_moves > 0 ? 1 : 0;
}
