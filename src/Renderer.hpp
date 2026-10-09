#pragma once
#include <raylib.h>
#include <vector>
#include <utility>
#include "Board.hpp"

class Renderer {
public:
    Renderer();
    ~Renderer();

    void drawBoard(int screenWidth, int screenHeight, const Board& board, int selectedRank = -1, int selectedFile = -1, const std::vector<std::pair<int, int>>& validMoves = {}, bool flipBoard = false);
    void drawGameOver(int screenWidth, int screenHeight, int state); // 1: White Won, 2: Black Won, 3: Stalemate
    void drawPieceAt(int type, bool isWhite, int x, int y, int size);

private:
    Color lightSquare;
    Color darkSquare;
    
    // pieceTextures[color][type]
    // color: 0 = Black, 1 = White
    // type: 0=King, 1=Queen, 2=Bishop, 3=Knight, 4=Rook, 5=Pawn
    Texture2D pieceTextures[2][6];
    
    // Helper to draw a specific piece from the individual textures
    void drawPiece(int pieceIndex, bool isWhite, int rank, int file, int squareSize);
};
