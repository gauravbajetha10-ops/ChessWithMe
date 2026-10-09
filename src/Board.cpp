#include "Board.hpp"

Board::Board() {
    initStandardBoard();
}

void Board::initStandardBoard() {
    // Clear board
    for (int rank = 0; rank < 8; ++rank) {
        for (int file = 0; file < 8; ++file) {
            grid[rank][file] = { PieceType::None, PieceColor::White };
        }
    }

    // Set up Black pieces (rank 0 and 1)
    grid[0][0] = { PieceType::Rook, PieceColor::Black };
    grid[0][1] = { PieceType::Knight, PieceColor::Black };
    grid[0][2] = { PieceType::Bishop, PieceColor::Black };
    grid[0][3] = { PieceType::Queen, PieceColor::Black };
    grid[0][4] = { PieceType::King, PieceColor::Black };
    grid[0][5] = { PieceType::Bishop, PieceColor::Black };
    grid[0][6] = { PieceType::Knight, PieceColor::Black };
    grid[0][7] = { PieceType::Rook, PieceColor::Black };

    for (int file = 0; file < 8; ++file) {
        grid[1][file] = { PieceType::Pawn, PieceColor::Black };
    }

    // Set up White pieces (rank 6 and 7)
    for (int file = 0; file < 8; ++file) {
        grid[6][file] = { PieceType::Pawn, PieceColor::White };
    }

    grid[7][0] = { PieceType::Rook, PieceColor::White };
    grid[7][1] = { PieceType::Knight, PieceColor::White };
    grid[7][2] = { PieceType::Bishop, PieceColor::White };
    grid[7][3] = { PieceType::Queen, PieceColor::White };
    grid[7][4] = { PieceType::King, PieceColor::White };
    grid[7][5] = { PieceType::Bishop, PieceColor::White };
    grid[7][6] = { PieceType::Knight, PieceColor::White };
    grid[7][7] = { PieceType::Rook, PieceColor::White };
}

const Piece& Board::getPiece(int rank, int file) const {
    return grid[rank][file];
}

void Board::setPiece(int rank, int file, Piece p) {
    grid[rank][file] = p;
}

void Board::makeMove(const Move& move) {
    // 1. Move piece
    grid[move.endRank][move.endFile] = grid[move.startRank][move.startFile];
    grid[move.startRank][move.startFile] = { PieceType::None, PieceColor::White };
    
    // 2. Handle Promotion
    if (move.isPromotion) {
        grid[move.endRank][move.endFile].type = move.promotionType;
    }
    
    // 3. Handle En Passant Capture
    if (move.isEnPassant) {
        grid[move.startRank][move.endFile] = { PieceType::None, PieceColor::White };
    }
    
    // 4. Handle Castling
    if (move.isCastling) {
        if (move.endFile == 6) { // Kingside
            grid[move.endRank][5] = grid[move.endRank][7];
            grid[move.endRank][7] = { PieceType::None, PieceColor::White };
        } else if (move.endFile == 2) { // Queenside
            grid[move.endRank][3] = grid[move.endRank][0];
            grid[move.endRank][0] = { PieceType::None, PieceColor::White };
        }
    }
    
    // 5. Update Castling Rights
    if (grid[move.endRank][move.endFile].type == PieceType::King) {
        if (sideToMove == PieceColor::White) { whiteCanCastleK = false; whiteCanCastleQ = false; }
        else { blackCanCastleK = false; blackCanCastleQ = false; }
    } else if (grid[move.endRank][move.endFile].type == PieceType::Rook) {
        if (move.startRank == 7 && move.startFile == 7) whiteCanCastleK = false;
        if (move.startRank == 7 && move.startFile == 0) whiteCanCastleQ = false;
        if (move.startRank == 0 && move.startFile == 7) blackCanCastleK = false;
        if (move.startRank == 0 && move.startFile == 0) blackCanCastleQ = false;
    }
    if (move.endRank == 7 && move.endFile == 7) whiteCanCastleK = false;
    if (move.endRank == 7 && move.endFile == 0) whiteCanCastleQ = false;
    if (move.endRank == 0 && move.endFile == 7) blackCanCastleK = false;
    if (move.endRank == 0 && move.endFile == 0) blackCanCastleQ = false;

    // 6. Update EP square
    epRank = -1;
    epFile = -1;
    if (grid[move.endRank][move.endFile].type == PieceType::Pawn && std::abs(move.endRank - move.startRank) == 2) {
        epRank = (move.startRank + move.endRank) / 2;
        epFile = move.startFile;
    }

    sideToMove = (sideToMove == PieceColor::White) ? PieceColor::Black : PieceColor::White;
}

void Board::undoMove(const Move& move) {
    // 1. Restore piece
    grid[move.startRank][move.startFile] = grid[move.endRank][move.endFile];
    grid[move.endRank][move.endFile] = move.capturedPiece;
    
    // 2. Undo Promotion
    if (move.isPromotion) {
        grid[move.startRank][move.startFile].type = PieceType::Pawn;
    }
    
    // 3. Undo En Passant Capture
    if (move.isEnPassant) {
        grid[move.endRank][move.endFile] = { PieceType::None, PieceColor::White };
        grid[move.startRank][move.endFile] = { PieceType::Pawn, sideToMove };
    }
    
    // 4. Undo Castling
    if (move.isCastling) {
        if (move.endFile == 6) { // Kingside
            grid[move.endRank][7] = grid[move.endRank][5];
            grid[move.endRank][5] = { PieceType::None, PieceColor::White };
        } else if (move.endFile == 2) { // Queenside
            grid[move.endRank][0] = grid[move.endRank][3];
            grid[move.endRank][3] = { PieceType::None, PieceColor::White };
        }
    }
    
    // 5. Restore State
    whiteCanCastleK = move.prevWhiteCastleK;
    whiteCanCastleQ = move.prevWhiteCastleQ;
    blackCanCastleK = move.prevBlackCastleK;
    blackCanCastleQ = move.prevBlackCastleQ;
    epRank = move.prevEpRank;
    epFile = move.prevEpFile;

    sideToMove = (sideToMove == PieceColor::White) ? PieceColor::Black : PieceColor::White;
}

void Board::loadFEN(const std::string& fen) {
    // Clear board
    for (int rank = 0; rank < 8; ++rank) {
        for (int file = 0; file < 8; ++file) {
            grid[rank][file] = { PieceType::None, PieceColor::White };
        }
    }
    
    int rank = 0;
    int file = 0;
    
    for (char c : fen) {
        if (c == ' ') break; // Only parse board placement for now
        
        if (c == '/') {
            rank++;
            file = 0;
        } else if (isdigit(c)) {
            file += (c - '0');
        } else {
            Piece p;
            p.color = isupper(c) ? PieceColor::White : PieceColor::Black;
            char l = tolower(c);
            if (l == 'k') p.type = PieceType::King;
            else if (l == 'q') p.type = PieceType::Queen;
            else if (l == 'b') p.type = PieceType::Bishop;
            else if (l == 'n') p.type = PieceType::Knight;
            else if (l == 'r') p.type = PieceType::Rook;
            else if (l == 'p') p.type = PieceType::Pawn;
            
            grid[rank][file] = p;
            file++;
        }
    }
}


