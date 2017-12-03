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

#define DEPTH 4

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

typedef enum { false, true } bool;

int tryMove(ull moves, int moveNum, int color, Board *b) {
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

    // Set the third parameter to 1, to echo disks flipped.
    int nflips = FlipDisks(m, b, color, 1, 1);

    if (nflips > 0)
        PlaceOrFlip(m, b, color);

    return nflips;
}

int minimax(Board *b, int depth, int color, bool maximizingPlayer)
{
    Board legal_moves;
    int num_moves = EnumerateLegalMoves(*b, color, &legal_moves);
    if (depth <= 0 || num_moves <= 0)
        return CountBitsOnBoard(b, color);

    ull moves = legal_moves.disks[color];

    if (maximizingPlayer) {
        int bestValue = INT_MIN;
        for (int moveNum = 0; moveNum < num_moves; moveNum++) {
            int numFlips = tryMove(moves, moveNum, color, b);
            if (numFlips > 0) {
                int v = minimax(b, depth - 1, color, false);
                bestValue = MAX(bestValue, v);
            }
        }
        return bestValue;
    }
    else {
        int bestValue = INT_MAX;
        for (int moveNum = 0; moveNum < num_moves; moveNum++) {
            int numFlips = tryMove(moves, moveNum, color, b); // should we pass a clone of 'b'?
            if (numFlips > 0) {
                int v = minimax(b, depth - 1, color, true);
                bestValue = MIN(bestValue, v);
            }
        }
        return bestValue;
    }
}

int GoodAITurnSequential(Board *b, int color)
{
    int num_disks = minimax(b, DEPTH, color, true);
    printf("color %d has %d disks.\n", color, num_disks);

    int num_moves = EnumerateLegalMoves(*b, color, b);
    return num_moves > 0 ? 1 : 0;
}


int GoodAITurnParallel(Board *b, int color)
{
    // Put your code for your parallel minimax algorithm here.

    return 0;
}
