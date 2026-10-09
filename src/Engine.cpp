#include "Engine.hpp"
#include <algorithm>

int Engine::evaluateBoard(const Board& board) {
    int score = 0;
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            const Piece& p = board.getPiece(r, c);
            if (p.isEmpty()) continue;
            
            int val = 0;
            switch(p.type) {
                case PieceType::Pawn: val = 100; break;
                case PieceType::Knight: val = 320; break;
                case PieceType::Bishop: val = 330; break;
                case PieceType::Rook: val = 500; break;
                case PieceType::Queen: val = 900; break;
                case PieceType::King: val = 20000; break;
                default: break;
            }
            
            // Basic piece-square bonus (center control)
            int centerBonus = 0;
            if (p.type == PieceType::Knight || p.type == PieceType::Pawn || p.type == PieceType::Bishop) {
                int distToCenter = std::abs(r - 3) + std::abs(c - 3);
                centerBonus = (6 - distToCenter) * 5;
            }
            
            if (p.color == PieceColor::White) {
                score += val + centerBonus;
            } else {
                score -= val + centerBonus;
            }
        }
    }
    return score;
}

int Engine::minimax(Board& board, int depth, int alpha, int beta, bool isMaximizing) {
    if (depth == 0) {
        return evaluateBoard(board);
    }
    
    PieceColor color = isMaximizing ? PieceColor::White : PieceColor::Black;
    std::vector<Move> legalMoves = MoveGenerator::generateLegalMoves(board, color);
    
    std::sort(legalMoves.begin(), legalMoves.end(), [](const Move& a, const Move& b) {
        int scoreA = 0, scoreB = 0;
        if (a.capturedPiece.type != PieceType::None) scoreA += 10;
        if (a.isPromotion) scoreA += 5;
        if (b.capturedPiece.type != PieceType::None) scoreB += 10;
        if (b.isPromotion) scoreB += 5;
        return scoreA > scoreB;
    });
    
    if (legalMoves.empty()) {
        if (MoveGenerator::isInCheck(board, color)) {
            return isMaximizing ? -30000 : 30000; // Checkmate
        }
        return 0; // Stalemate
    }
    
    if (isMaximizing) {
        int maxEval = -1000000;
        for (const Move& move : legalMoves) {
            board.makeMove(move);
            int eval = minimax(board, depth - 1, alpha, beta, false);
            board.undoMove(move);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) break; // Beta cutoff
        }
        return maxEval;
    } else {
        int minEval = 1000000;
        for (const Move& move : legalMoves) {
            board.makeMove(move);
            int eval = minimax(board, depth - 1, alpha, beta, true);
            board.undoMove(move);
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) break; // Alpha cutoff
        }
        return minEval;
    }
}

Move Engine::getBestMove(Board& board, int depth, PieceColor aiColor) {
    std::vector<Move> legalMoves = MoveGenerator::generateLegalMoves(board, aiColor);
    
    std::sort(legalMoves.begin(), legalMoves.end(), [](const Move& a, const Move& b) {
        int scoreA = 0, scoreB = 0;
        if (a.capturedPiece.type != PieceType::None) scoreA += 10;
        if (a.isPromotion) scoreA += 5;
        if (b.capturedPiece.type != PieceType::None) scoreB += 10;
        if (b.isPromotion) scoreB += 5;
        return scoreA > scoreB;
    });

    if (legalMoves.empty()) {
        return {-1, -1, -1, -1, {PieceType::None, PieceColor::White}}; 
    }
    
    Move bestMove = legalMoves[0];
    bool isMaximizing = (aiColor == PieceColor::White);
    int bestValue = isMaximizing ? -1000000 : 1000000;
    
    for (const Move& move : legalMoves) {
        board.makeMove(move);
        int boardValue = minimax(board, depth - 1, -1000000, 1000000, !isMaximizing);
        board.undoMove(move);
        
        if (isMaximizing) {
            if (boardValue > bestValue) {
                bestValue = boardValue;
                bestMove = move;
            }
        } else {
            if (boardValue < bestValue) {
                bestValue = boardValue;
                bestMove = move;
            }
        }
    }
    return bestMove;
}
