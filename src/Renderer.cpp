#include "Renderer.hpp"
#include <string>

Renderer::Renderer() {
    lightSquare = { 238, 238, 210, 255 }; // Light beige
    darkSquare  = { 118, 150, 86, 255 };  // Chess green
    
    const char* pieceNames[6] = { "King", "Queen", "Bishop", "Knight", "Rook", "Pawn" };
    const char* colors[2] = { "b", "w" };
    
    for (int color = 0; color < 2; ++color) {
        for (int type = 0; type < 6; ++type) {
            std::string path = std::string("assets/ChessAssets/") + colors[color] + "_" + pieceNames[type] + ".png";
            pieceTextures[color][type] = LoadTexture(path.c_str());
        }
    }
}

Renderer::~Renderer() {
    for (int color = 0; color < 2; ++color) {
        for (int type = 0; type < 6; ++type) {
            UnloadTexture(pieceTextures[color][type]);
        }
    }
}

void Renderer::drawBoard(int screenWidth, int screenHeight, const Board& board, int selectedRank, int selectedFile, const std::vector<std::pair<int, int>>& validMoves, bool flipBoard) {
    int squareSize = screenWidth / 8; // Assuming a square window for now

    for (int rank = 0; rank < 8; ++rank) {
        for (int file = 0; file < 8; ++file) {
            int drawRank = flipBoard ? 7 - rank : rank;
            int drawFile = flipBoard ? 7 - file : file;

            bool isLight = (rank + file) % 2 == 0;
            Color color = isLight ? lightSquare : darkSquare;
            
            DrawRectangle(drawFile * squareSize, drawRank * squareSize, squareSize, squareSize, color);
            
            // Highlight selected square
            if (rank == selectedRank && file == selectedFile) {
                DrawRectangle(drawFile * squareSize, drawRank * squareSize, squareSize, squareSize, { 255, 255, 0, 150 });
            }
            
            // Draw piece from board state
            const Piece& piece = board.getPiece(rank, file);
            if (!piece.isEmpty()) {
                drawPiece((int)piece.type, piece.color == PieceColor::White, drawRank, drawFile, squareSize);
            }
            
            // Highlight valid moves
            for (const auto& move : validMoves) {
                if (move.first == rank && move.second == file) {
                    DrawCircle(drawFile * squareSize + squareSize / 2, drawRank * squareSize + squareSize / 2, squareSize / 6.0f, { 0, 0, 0, 100 });
                }
            }
        }
    }
}

void Renderer::drawPiece(int pieceIndex, bool isWhite, int rank, int file, int squareSize) {
    int colorIndex = isWhite ? 1 : 0;
    Texture2D tex = pieceTextures[colorIndex][pieceIndex];
    
    Rectangle sourceRec = { 
        0.0f, 
        0.0f, 
        (float)tex.width, 
        (float)tex.height 
    };
    
    Rectangle destRec = { 
        (float)file * squareSize, 
        (float)rank * squareSize, 
        (float)squareSize, 
        (float)squareSize 
    };
    
    Vector2 origin = { 0.0f, 0.0f };
    
    DrawTexturePro(tex, sourceRec, destRec, origin, 0.0f, WHITE);
}

void Renderer::drawPieceAt(int type, bool isWhite, int x, int y, int size) {
    if (type >= 1 && type <= 6) {
        Texture2D texture = pieceTextures[isWhite ? 1 : 0][type - 1];
        Rectangle source = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
        Rectangle dest = { (float)x, (float)y, (float)size, (float)size };
        Vector2 origin = { 0.0f, 0.0f };
        DrawTexturePro(texture, source, dest, origin, 0.0f, WHITE);
    }
}

void Renderer::drawGameOver(int screenWidth, int screenHeight, int state) {
    DrawRectangle(0, 0, screenWidth, screenHeight, { 0, 0, 0, 150 });
    
    const char* text = "";
    if (state == 1) text = "CHECKMATE! White Wins.";
    else if (state == 2) text = "CHECKMATE! Black Wins.";
    else if (state == 3) text = "STALEMATE! It's a draw.";

    int fontSize = 40;
    int textWidth = MeasureText(text, fontSize);
    DrawText(text, screenWidth / 2 - textWidth / 2, screenHeight / 2 - fontSize / 2, fontSize, WHITE);
}
