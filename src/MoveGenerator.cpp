#include "MoveGenerator.hpp"
#include <algorithm>

Move MoveGenerator::createMove(const Board& board, int startRank, int startFile, int endRank, int endFile, Piece capturedPiece, bool isCastling, bool isEnPassant, bool isPromotion, PieceType promotionType) {
    Move m;
    m.startRank = startRank; m.startFile = startFile;
    m.endRank = endRank; m.endFile = endFile;
    m.capturedPiece = capturedPiece;
    m.isCastling = isCastling;
    m.isEnPassant = isEnPassant;
    m.isPromotion = isPromotion;
    m.promotionType = promotionType;
    
    m.prevWhiteCastleK = board.whiteCanCastleK;
    m.prevWhiteCastleQ = board.whiteCanCastleQ;
    m.prevBlackCastleK = board.blackCanCastleK;
    m.prevBlackCastleQ = board.blackCanCastleQ;
    m.prevEpRank = board.epRank;
    m.prevEpFile = board.epFile;
    
    return m;
}

std::vector<Move> MoveGenerator::generateLegalMoves(Board& board, PieceColor color) {
    std::vector<Move> pseudoLegalMoves = generatePseudoLegalMoves(board, color, false);
    std::vector<Move> legalMoves;

    for (const Move& move : pseudoLegalMoves) {
        board.makeMove(move);
        board.sideToMove = color; 
        if (!isInCheck(board, color)) {
            legalMoves.push_back(move);
        }
        board.sideToMove = (color == PieceColor::White) ? PieceColor::Black : PieceColor::White;
        board.undoMove(move);
    }
    return legalMoves;
}

std::vector<Move> MoveGenerator::generatePseudoLegalMoves(const Board& board, PieceColor color, bool attacksOnly) {
    std::vector<Move> moves;

    auto isValidSquare = [&](int r, int c) {
        return r >= 0 && r < 8 && c >= 0 && c < 8;
    };

    auto canMoveTo = [&](int r, int c) {
        if (!isValidSquare(r, c)) return false;
        return board.getPiece(r, c).isEmpty() || board.getPiece(r, c).color != color;
    };

    for (int rank = 0; rank < 8; ++rank) {
        for (int file = 0; file < 8; ++file) {
            const Piece& p = board.getPiece(rank, file);
            if (p.isEmpty() || p.color != color) continue;

            if (p.type == PieceType::Knight) {
                int dr[] = {-2, -2, -1, -1, 1, 1, 2, 2};
                int dc[] = {-1, 1, -2, 2, -2, 2, -1, 1};
                for (int i = 0; i < 8; ++i) {
                    if (canMoveTo(rank + dr[i], file + dc[i])) {
                        moves.push_back(createMove(board, rank, file, rank + dr[i], file + dc[i], board.getPiece(rank + dr[i], file + dc[i])));
                    }
                }
            } else if (p.type == PieceType::King) {
                int dr[] = {-1, -1, -1, 0, 0, 1, 1, 1};
                int dc[] = {-1, 0, 1, -1, 1, -1, 0, 1};
                for (int i = 0; i < 8; ++i) {
                    if (canMoveTo(rank + dr[i], file + dc[i])) {
                        moves.push_back(createMove(board, rank, file, rank + dr[i], file + dc[i], board.getPiece(rank + dr[i], file + dc[i])));
                    }
                }
                
                // Castling
                if (!attacksOnly) {
                    if (color == PieceColor::White) {
                        if (board.whiteCanCastleK && board.getPiece(7, 5).isEmpty() && board.getPiece(7, 6).isEmpty()) {
                            if (!isSquareAttacked(board, 7, 4, PieceColor::Black) && !isSquareAttacked(board, 7, 5, PieceColor::Black)) {
                                moves.push_back(createMove(board, 7, 4, 7, 6, {PieceType::None, PieceColor::White}, true));
                            }
                        }
                        if (board.whiteCanCastleQ && board.getPiece(7, 3).isEmpty() && board.getPiece(7, 2).isEmpty() && board.getPiece(7, 1).isEmpty()) {
                            if (!isSquareAttacked(board, 7, 4, PieceColor::Black) && !isSquareAttacked(board, 7, 3, PieceColor::Black)) {
                                moves.push_back(createMove(board, 7, 4, 7, 2, {PieceType::None, PieceColor::White}, true));
                            }
                        }
                    } else {
                        if (board.blackCanCastleK && board.getPiece(0, 5).isEmpty() && board.getPiece(0, 6).isEmpty()) {
                            if (!isSquareAttacked(board, 0, 4, PieceColor::White) && !isSquareAttacked(board, 0, 5, PieceColor::White)) {
                                moves.push_back(createMove(board, 0, 4, 0, 6, {PieceType::None, PieceColor::White}, true));
                            }
                        }
                        if (board.blackCanCastleQ && board.getPiece(0, 3).isEmpty() && board.getPiece(0, 2).isEmpty() && board.getPiece(0, 1).isEmpty()) {
                            if (!isSquareAttacked(board, 0, 4, PieceColor::White) && !isSquareAttacked(board, 0, 3, PieceColor::White)) {
                                moves.push_back(createMove(board, 0, 4, 0, 2, {PieceType::None, PieceColor::White}, true));
                            }
                        }
                    }
                }
                
            } else if (p.type == PieceType::Pawn) {
                int dir = (color == PieceColor::White) ? -1 : 1;
                int startRank = (color == PieceColor::White) ? 6 : 1;
                int promoRank = (color == PieceColor::White) ? 0 : 7;
                
                // Forward 1
                if (!attacksOnly && isValidSquare(rank + dir, file) && board.getPiece(rank + dir, file).isEmpty()) {
                    if (rank + dir == promoRank) {
                        moves.push_back(createMove(board, rank, file, rank + dir, file, {PieceType::None, PieceColor::White}, false, false, true, PieceType::Queen));
                        moves.push_back(createMove(board, rank, file, rank + dir, file, {PieceType::None, PieceColor::White}, false, false, true, PieceType::Rook));
                        moves.push_back(createMove(board, rank, file, rank + dir, file, {PieceType::None, PieceColor::White}, false, false, true, PieceType::Bishop));
                        moves.push_back(createMove(board, rank, file, rank + dir, file, {PieceType::None, PieceColor::White}, false, false, true, PieceType::Knight));
                    } else {
                        moves.push_back(createMove(board, rank, file, rank + dir, file, {PieceType::None, PieceColor::White}));
                        // Forward 2
                        if (rank == startRank && board.getPiece(rank + 2 * dir, file).isEmpty()) {
                            moves.push_back(createMove(board, rank, file, rank + 2 * dir, file, {PieceType::None, PieceColor::White}));
                        }
                    }
                }
                
                // Captures
                for (int df : {-1, 1}) {
                    if (isValidSquare(rank + dir, file + df)) {
                        if (attacksOnly) {
                             moves.push_back(createMove(board, rank, file, rank + dir, file + df, board.getPiece(rank + dir, file + df)));
                             continue;
                        }
                        
                        if (!board.getPiece(rank + dir, file + df).isEmpty() && board.getPiece(rank + dir, file + df).color != color) {
                            if (rank + dir == promoRank) {
                                moves.push_back(createMove(board, rank, file, rank + dir, file + df, board.getPiece(rank + dir, file + df), false, false, true, PieceType::Queen));
                                moves.push_back(createMove(board, rank, file, rank + dir, file + df, board.getPiece(rank + dir, file + df), false, false, true, PieceType::Rook));
                                moves.push_back(createMove(board, rank, file, rank + dir, file + df, board.getPiece(rank + dir, file + df), false, false, true, PieceType::Bishop));
                                moves.push_back(createMove(board, rank, file, rank + dir, file + df, board.getPiece(rank + dir, file + df), false, false, true, PieceType::Knight));
                            } else {
                                moves.push_back(createMove(board, rank, file, rank + dir, file + df, board.getPiece(rank + dir, file + df)));
                            }
                        } else if (rank + dir == board.epRank && file + df == board.epFile) {
                            Piece capturedPawn = {PieceType::Pawn, (color == PieceColor::White) ? PieceColor::Black : PieceColor::White};
                            moves.push_back(createMove(board, rank, file, rank + dir, file + df, capturedPawn, false, true));
                        }
                    }
                }
            } else if (p.type == PieceType::Rook || p.type == PieceType::Bishop || p.type == PieceType::Queen) {
                if (p.type == PieceType::Rook || p.type == PieceType::Queen) {
                    addSlidingMoves(board, rank, file, 1, 0, color, moves);
                    addSlidingMoves(board, rank, file, -1, 0, color, moves);
                    addSlidingMoves(board, rank, file, 0, 1, color, moves);
                    addSlidingMoves(board, rank, file, 0, -1, color, moves);
                }
                if (p.type == PieceType::Bishop || p.type == PieceType::Queen) {
                    addSlidingMoves(board, rank, file, 1, 1, color, moves);
                    addSlidingMoves(board, rank, file, -1, -1, color, moves);
                    addSlidingMoves(board, rank, file, 1, -1, color, moves);
                    addSlidingMoves(board, rank, file, -1, 1, color, moves);
                }
            }
        }
    }
    return moves;
}

void MoveGenerator::addSlidingMoves(const Board& board, int rank, int file, int dRank, int dFile, PieceColor color, std::vector<Move>& moves) {
    int r = rank + dRank;
    int c = file + dFile;
    while (r >= 0 && r < 8 && c >= 0 && c < 8) {
        if (board.getPiece(r, c).isEmpty()) {
            moves.push_back(createMove(board, rank, file, r, c, {PieceType::None, PieceColor::White}));
        } else {
            if (board.getPiece(r, c).color != color) {
                moves.push_back(createMove(board, rank, file, r, c, board.getPiece(r, c)));
            }
            break;
        }
        r += dRank;
        c += dFile;
    }
}

bool MoveGenerator::isSquareAttacked(const Board& board, int rank, int file, PieceColor attackerColor) {
    // Check Pawns
    int expectedPawnRank = (attackerColor == PieceColor::White) ? rank + 1 : rank - 1;
    if (expectedPawnRank >= 0 && expectedPawnRank < 8) {
        for (int df : {-1, 1}) {
            int c = file + df;
            if (c >= 0 && c < 8) {
                const Piece& p = board.getPiece(expectedPawnRank, c);
                if (p.color == attackerColor && p.type == PieceType::Pawn) return true;
            }
        }
    }

    // Check Knights
    int nr[] = {-2, -2, -1, -1, 1, 1, 2, 2};
    int nc[] = {-1, 1, -2, 2, -2, 2, -1, 1};
    for (int i = 0; i < 8; ++i) {
        int r = rank + nr[i];
        int c = file + nc[i];
        if (r >= 0 && r < 8 && c >= 0 && c < 8) {
            const Piece& p = board.getPiece(r, c);
            if (p.color == attackerColor && p.type == PieceType::Knight) return true;
        }
    }

    // Check King
    int kr[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int kc[] = {-1, 0, 1, -1, 1, -1, 0, 1};
    for (int i = 0; i < 8; ++i) {
        int r = rank + kr[i];
        int c = file + kc[i];
        if (r >= 0 && r < 8 && c >= 0 && c < 8) {
            const Piece& p = board.getPiece(r, c);
            if (p.color == attackerColor && p.type == PieceType::King) return true;
        }
    }

    // Check straight lines (Rook, Queen)
    int sr[] = {-1, 1, 0, 0};
    int sc[] = {0, 0, -1, 1};
    for (int i = 0; i < 4; ++i) {
        int r = rank + sr[i];
        int c = file + sc[i];
        while (r >= 0 && r < 8 && c >= 0 && c < 8) {
            const Piece& p = board.getPiece(r, c);
            if (!p.isEmpty()) {
                if (p.color == attackerColor && (p.type == PieceType::Rook || p.type == PieceType::Queen)) return true;
                break;
            }
            r += sr[i];
            c += sc[i];
        }
    }

    // Check diagonals (Bishop, Queen)
    int dr[] = {-1, -1, 1, 1};
    int dc[] = {-1, 1, -1, 1};
    for (int i = 0; i < 4; ++i) {
        int r = rank + dr[i];
        int c = file + dc[i];
        while (r >= 0 && r < 8 && c >= 0 && c < 8) {
            const Piece& p = board.getPiece(r, c);
            if (!p.isEmpty()) {
                if (p.color == attackerColor && (p.type == PieceType::Bishop || p.type == PieceType::Queen)) return true;
                break;
            }
            r += dr[i];
            c += dc[i];
        }
    }

    return false;
}

bool MoveGenerator::isInCheck(const Board& board, PieceColor color) {
    int kingRank = -1;
    int kingFile = -1;
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            const Piece& p = board.getPiece(r, c);
            if (p.type == PieceType::King && p.color == color) {
                kingRank = r;
                kingFile = c;
                break;
            }
        }
    }
    if (kingRank == -1) return false;
    PieceColor attackerColor = (color == PieceColor::White) ? PieceColor::Black : PieceColor::White;
    return isSquareAttacked(board, kingRank, kingFile, attackerColor);
}
