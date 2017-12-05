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

#define DEPTH 1

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
    int nflips = FlipDisks(m, b, color, 0, 1);

    if (nflips > 0) {
        PlaceOrFlip(m, b, color);
    }

    return nflips;
}

int minimax(Board *b, int depth, int startColor, bool maximizingPlayer)
{
    // alternate color depending on the depth
    int color = maximizingPlayer ? startColor : OTHERCOLOR(startColor);
    char c_c = color ==  X_BLACK ? 'X' : 'O';
    Board legal_moves;
    int num_moves = EnumerateLegalMoves(*b, color, &legal_moves);
    if (depth <= 0 || num_moves <= 0) {
        int diff = CountBitsOnBoard(b, startColor) - CountBitsOnBoard(b, OTHERCOLOR(startColor));
        return diff;
    }
    printf("at depth %d, %c has %d possible moves\n", depth, c_c, num_moves);

    ull moves = legal_moves.disks[color];

    if (maximizingPlayer) {
        int bestValue = INT_MIN;
        for (int moveNum = 0; moveNum < num_moves; moveNum++) {
            Board c = *b; // copy
            int nFlips = tryMove(moves, moveNum, color, &c);
            //printf("maximizingPlayer=%c, depth=%d, move=(%d,%d) ... ", c_c, depth, m.row, m.col);
            if (nFlips != 0) {
                int v = minimax(&c, depth - 1, startColor, false);
                if (v > bestValue) {
                    //PrintBoard(*b);
                    //PrintBoard(c);
                    b = &c;
                    bestValue = v;
                    printf("at depth %d, moveNum=%d for %c yielded the bestValue=%d (max)\n", depth, moveNum, c_c, bestValue);
                }
            }
        }
        return bestValue;
    }
    else {
        int bestValue = INT_MAX;
        for (int moveNum = 0; moveNum < num_moves; moveNum++) {
            Board c = *b; // copy
            int nFlips = tryMove(moves, moveNum, color, &c);
            //printf("minimizingPlayer=%c, depth=%d, move=(%d,%d) ... ", c_c, depth, m.row, m.col);
            if (nFlips != 0) {
                int v = minimax(&c, depth - 1, startColor, false);
                if (v < bestValue) {
                    //PrintBoard(*b);
                    //PrintBoard(c);
                    b = &c;
                    bestValue = v;
                    printf("at depth %d, moveNum=%d for %c yielded the bestValue=%d (min)\n", depth, moveNum, c_c, bestValue);
                }
            }
        }
        return bestValue;
    }
}

int GoodAITurnSequential(Board *b, int color)
{
    int num_disks = minimax(b, DEPTH, color, true);

    Board legal_moves;
    int num_moves = EnumerateLegalMoves(*b, color, &legal_moves);
    return num_moves > 0 ? 1 : 0;
}


int GoodAITurnParallel(Board *b, int color)
{
    // Put your code for your parallel minimax algorithm here.

    return 0;
}
