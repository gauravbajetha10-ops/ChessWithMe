#pragma once
#include <vector>
#include "Board.hpp"

class MoveGenerator {
public:
    // Generate moves that do not leave the king in check
    static std::vector<Move> generateLegalMoves(Board& board, PieceColor color);
    
    // Generate moves without checking if they leave the king in check
    static std::vector<Move> generatePseudoLegalMoves(const Board& board, PieceColor color, bool attacksOnly = false);
    
    // Check if a square is attacked by a given color
    static bool isSquareAttacked(const Board& board, int rank, int file, PieceColor attackerColor);
    
    // Check if a given color's king is currently in check
    static bool isInCheck(const Board& board, PieceColor color);

private:
    static Move createMove(const Board& board, int startRank, int startFile, int endRank, int endFile, Piece capturedPiece, bool isCastling = false, bool isEnPassant = false, bool isPromotion = false, PieceType promotionType = PieceType::Queen);
    static void addSlidingMoves(const Board& board, int rank, int file, int dRank, int dFile, PieceColor color, std::vector<Move>& moves);
};
