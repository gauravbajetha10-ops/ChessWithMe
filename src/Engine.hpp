#pragma once
#include "Board.hpp"
#include "MoveGenerator.hpp"

class Engine {
public:
    static Move getBestMove(Board& board, int depth, PieceColor aiColor);

private:
    static int evaluateBoard(const Board& board);
    static int minimax(Board& board, int depth, int alpha, int beta, bool isMaximizing);
};
