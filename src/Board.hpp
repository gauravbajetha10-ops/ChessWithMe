#pragma once
#include <vector>
#include <utility>
#include <string>

enum class PieceType { None = -1, King = 0, Queen = 1, Bishop = 2, Knight = 3, Rook = 4, Pawn = 5 };
enum class PieceColor { Black = 0, White = 1 };

struct Piece {
    PieceType type = PieceType::None;
    PieceColor color = PieceColor::White;
    bool isEmpty() const { return type == PieceType::None; }
};

struct Move {
    int startRank = -1, startFile = -1;
    int endRank = -1, endFile = -1;
    Piece capturedPiece = {PieceType::None, PieceColor::White};
    
    bool isCastling = false;
    bool isEnPassant = false;
    bool isPromotion = false;
    PieceType promotionType = PieceType::Queen;

    bool prevWhiteCastleK = false, prevWhiteCastleQ = false;
    bool prevBlackCastleK = false, prevBlackCastleQ = false;
    int prevEpRank = -1, prevEpFile = -1;
    
    bool operator==(const Move& other) const {
        return startRank == other.startRank && startFile == other.startFile && 
               endRank == other.endRank && endFile == other.endFile &&
               promotionType == other.promotionType;
    }
};

class Board {
public:
    Board();
    void initStandardBoard();
    void loadFEN(const std::string& fen);
    
    // Returns the piece at a given rank (0-7) and file (0-7)
    const Piece& getPiece(int rank, int file) const;
    void setPiece(int rank, int file, Piece p);
    
    // Core move functions for tree traversal and game state
    void makeMove(const Move& move);
    void undoMove(const Move& move);

    PieceColor sideToMove = PieceColor::White;
    
    bool whiteCanCastleK = true, whiteCanCastleQ = true;
    bool blackCanCastleK = true, blackCanCastleQ = true;
    int epRank = -1, epFile = -1;

private:
    Piece grid[8][8];
};
