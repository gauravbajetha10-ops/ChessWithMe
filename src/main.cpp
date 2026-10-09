#include <raylib.h>

#if defined(_WIN32)
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif
#include <vector>
#include <algorithm>
#include <string>
#include <cstdio>
#include "Renderer.hpp"
#include "Board.hpp"
#include "MoveGenerator.hpp"
#include "Engine.hpp"

enum class GameMode { PvP, PvC };
enum class Difficulty { Beginner = 1, Intermediate = 3, Advance = 4, Pro = 5 };
enum class PlayerColor { White, Black };
enum class TimeLimit { Unlimited = 0, Min5 = 300, Min10 = 600, Min15 = 900 };

struct GameConfig {
    GameMode mode = GameMode::PvC;
    Difficulty difficulty = Difficulty::Intermediate;
    PlayerColor playerColor = PlayerColor::White;
    TimeLimit timeLimit = TimeLimit::Unlimited;
};

bool GuiButtonAdv(Rectangle bounds, const char* text, bool isSelected) {
    Vector2 mousePoint = GetMousePosition();
    bool isHovered = CheckCollisionPointRec(mousePoint, bounds);
    bool clicked = (isHovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT));
    
    Color bgColor = isSelected ? Color{70, 70, 80, 255} : (isHovered ? Color{50, 50, 60, 255} : Color{35, 35, 45, 255});
    Color borderColor = isSelected ? RAYWHITE : (isHovered ? LIGHTGRAY : BLANK);
    
    DrawRectangleRec(bounds, bgColor);
    if (isSelected || isHovered) {
        DrawRectangleLinesEx(bounds, 2, borderColor);
    }
    
    if (text) {
        int fontSize = 20;
        int textWidth = MeasureText(text, fontSize);
        DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - fontSize/2, fontSize, WHITE);
    }
    
    return clicked;
}

bool GuiBoxButton(Rectangle bounds, const char* text, bool isSelected, Renderer& renderer, int pieceType) {
    Vector2 mousePoint = GetMousePosition();
    bool isHovered = CheckCollisionPointRec(mousePoint, bounds);
    bool clicked = (isHovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT));
    
    Color bgColor = isSelected ? Color{70, 70, 80, 255} : (isHovered ? Color{50, 50, 60, 255} : Color{35, 35, 45, 255});
    Color borderColor = isSelected ? RAYWHITE : (isHovered ? LIGHTGRAY : BLANK);
    
    DrawRectangleRec(bounds, bgColor);
    if (isSelected || isHovered) {
        DrawRectangleLinesEx(bounds, 2, borderColor);
    }
    
    if (pieceType != -1) {
        int size = bounds.width - 20;
        renderer.drawPieceAt(pieceType, true, bounds.x + 10, bounds.y + 10, size);
    }
    
    if (text) {
        int fontSize = 18;
        int textWidth = MeasureText(text, fontSize);
        DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height - 25, fontSize, WHITE);
    }
    
    return clicked;
}

bool GuiButtonStart(Rectangle bounds, const char* text) {
    Vector2 mousePoint = GetMousePosition();
    bool isHovered = CheckCollisionPointRec(mousePoint, bounds);
    bool clicked = (isHovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT));
    
    Color bgColor = isHovered ? Color{220, 50, 50, 255} : Color{180, 40, 40, 255};
    DrawRectangleRec(bounds, bgColor);
    
    int fontSize = 24;
    int textWidth = MeasureText(text, fontSize);
    DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - fontSize/2, fontSize, WHITE);
    
    return clicked;
}

int main() {
    const int screenWidth = 800;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "Chess With Me - AI Chess");
    SetTargetFPS(60);

    Renderer renderer;
    Board board;
    GameConfig config;

    bool inMenu = true;
    int gameState = 0; // 0 = Playing, 1 = White Won, 2 = Black Won, 3 = Stalemate
    int selectedRank = -1;
    int selectedFile = -1;
    
    float whiteTimeRemaining = 0;
    float blackTimeRemaining = 0;
    float aiMoveTimer = 0.0f;
    float aiTargetDelay = 1.0f;

    while (!WindowShouldClose()) {
        if (inMenu) {
            BeginDrawing();
            ClearBackground(Color{20, 20, 25, 255}); // Dark elegant background
            
            DrawText("CHESS WITH ME", screenWidth/2 - MeasureText("CHESS WITH ME", 40)/2, 50, 40, WHITE);
            
            // Mode Section
            DrawText("GAME MODE", 170, 150, 20, LIGHTGRAY);
            if (GuiButtonAdv({170, 180, 220, 40}, "Player vs Player", config.mode == GameMode::PvP)) config.mode = GameMode::PvP;
            if (GuiButtonAdv({410, 180, 220, 40}, "Player vs CPU", config.mode == GameMode::PvC)) config.mode = GameMode::PvC;
            
            // Color Section
            int currentY = 250;
            DrawText(config.mode == GameMode::PvC ? "YOUR COLOR" : "BOTTOM COLOR", 170, currentY, 20, LIGHTGRAY);
            if (GuiButtonAdv({170, (float)currentY + 30, 220, 40}, "White", config.playerColor == PlayerColor::White)) config.playerColor = PlayerColor::White;
            if (GuiButtonAdv({410, (float)currentY + 30, 220, 40}, "Black", config.playerColor == PlayerColor::Black)) config.playerColor = PlayerColor::Black;
            currentY += 100;
            
            // AI IQ Section
            if (config.mode == GameMode::PvC) {
                DrawText("AI IQ", 170, currentY, 20, LIGHTGRAY);
                if (GuiBoxButton({170, (float)currentY + 30, 100, 130}, "Beginner", config.difficulty == Difficulty::Beginner, renderer, (int)PieceType::Pawn)) config.difficulty = Difficulty::Beginner;
                if (GuiBoxButton({280, (float)currentY + 30, 100, 130}, "Intermediate", config.difficulty == Difficulty::Intermediate, renderer, (int)PieceType::Knight)) config.difficulty = Difficulty::Intermediate;
                if (GuiBoxButton({390, (float)currentY + 30, 100, 130}, "Advanced", config.difficulty == Difficulty::Advance, renderer, (int)PieceType::Bishop)) config.difficulty = Difficulty::Advance;
                if (GuiBoxButton({500, (float)currentY + 30, 100, 130}, "Pro", config.difficulty == Difficulty::Pro, renderer, (int)PieceType::Queen)) config.difficulty = Difficulty::Pro;
                currentY += 190;
            }
            
            // Time Limit
            DrawText("TIME LIMIT", 170, currentY, 20, LIGHTGRAY);
            if (GuiButtonAdv({170, (float)currentY + 30, 100, 40}, "Unlimited", config.timeLimit == TimeLimit::Unlimited)) config.timeLimit = TimeLimit::Unlimited;
            if (GuiButtonAdv({280, (float)currentY + 30, 100, 40}, "5 Mins", config.timeLimit == TimeLimit::Min5)) config.timeLimit = TimeLimit::Min5;
            if (GuiButtonAdv({390, (float)currentY + 30, 100, 40}, "10 Mins", config.timeLimit == TimeLimit::Min10)) config.timeLimit = TimeLimit::Min10;
            if (GuiButtonAdv({500, (float)currentY + 30, 100, 40}, "15 Mins", config.timeLimit == TimeLimit::Min15)) config.timeLimit = TimeLimit::Min15;
            
            currentY += 100;
            
            // Start Game
            if (GuiButtonStart({(float)screenWidth/2 - 125, (float)currentY, 250, 60}, "START GAME")) {
                inMenu = false;
                board = Board();
                gameState = 0;
                selectedRank = -1;
                selectedFile = -1;
                whiteTimeRemaining = (float)config.timeLimit;
                blackTimeRemaining = (float)config.timeLimit;
                aiMoveTimer = 0.0f;
                aiTargetDelay = 0.5f + (GetRandomValue(0, 100) / 100.0f);
            }
            
            EndDrawing();
        } else {
            float dt = GetFrameTime();
            float cappedDt = (dt > 0.1f) ? 0.1f : dt;

            // Gameplay Timers
            if (gameState == 0 && config.timeLimit != TimeLimit::Unlimited) {
                if (board.sideToMove == PieceColor::White) {
                    whiteTimeRemaining -= cappedDt;
                    if (whiteTimeRemaining <= 0) {
                        whiteTimeRemaining = 0;
                        gameState = 2; // Black Won
                    }
                } else {
                    blackTimeRemaining -= cappedDt;
                    if (blackTimeRemaining <= 0) {
                        blackTimeRemaining = 0;
                        gameState = 1; // White Won
                    }
                }
            }

            // State Evaluation
            if (gameState == 0) {
                auto legalMoves = MoveGenerator::generateLegalMoves(board, board.sideToMove);
                if (legalMoves.empty()) {
                    if (MoveGenerator::isInCheck(board, board.sideToMove)) {
                        gameState = (board.sideToMove == PieceColor::White) ? 2 : 1;
                    } else {
                        gameState = 3;
                    }
                }
            }

            // AI / Input Processing
            std::vector<std::pair<int, int>> validDestinations;
            if (gameState == 0) {
                bool isAiTurn = (config.mode == GameMode::PvC) && 
                                ((board.sideToMove == PieceColor::White && config.playerColor == PlayerColor::Black) || 
                                 (board.sideToMove == PieceColor::Black && config.playerColor == PlayerColor::White));
                
                if (isAiTurn) {
                    aiMoveTimer += cappedDt;
                    if (aiMoveTimer >= aiTargetDelay) {
                        double startTime = GetTime();
                        Move aiMove = Engine::getBestMove(board, (int)config.difficulty, board.sideToMove);
                        double thinkTime = GetTime() - startTime;
                        
                        if (config.timeLimit != TimeLimit::Unlimited) {
                            if (board.sideToMove == PieceColor::White) {
                                whiteTimeRemaining -= (float)thinkTime;
                            } else {
                                blackTimeRemaining -= (float)thinkTime;
                            }
                        }
                        
                        if (aiMove.startRank != -1) {
                            board.makeMove(aiMove);
                        }
                        aiMoveTimer = 0.0f;
                        aiTargetDelay = 0.5f + (GetRandomValue(0, 100) / 100.0f);
                    }
                } else {
                    aiMoveTimer = 0.0f;
                    // Human Turn
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        Vector2 mousePos = GetMousePosition();
                        int squareSize = screenWidth / 8;
                        int file = (int)mousePos.x / squareSize;
                        int rank = (int)mousePos.y / squareSize;
                        
                        bool flipBoard = (config.playerColor == PlayerColor::Black);
                        if (flipBoard) {
                            file = 7 - file;
                            rank = 7 - rank;
                        }
                        
                        if (file >= 0 && file < 8 && rank >= 0 && rank < 8) {
                            if (selectedRank == -1) {
                                if (!board.getPiece(rank, file).isEmpty() && board.getPiece(rank, file).color == board.sideToMove) {
                                    selectedRank = rank;
                                    selectedFile = file;
                                }
                            } else {
                                if (selectedRank == rank && selectedFile == file) {
                                    selectedRank = -1;
                                    selectedFile = -1;
                                } else {
                                    auto legalMoves = MoveGenerator::generateLegalMoves(board, board.sideToMove);
                                    bool isValid = false;
                                    Move validMove;
                                    for (const auto& move : legalMoves) {
                                        if (move.startRank == selectedRank && move.startFile == selectedFile &&
                                            move.endRank == rank && move.endFile == file) {
                                            isValid = true;
                                            validMove = move;
                                            break;
                                        }
                                    }
                                    
                                    if (isValid) {
                                        board.makeMove(validMove);
                                        selectedRank = -1;
                                        selectedFile = -1;
                                    } else {
                                        const Piece& clickedPiece = board.getPiece(rank, file);
                                        if (!clickedPiece.isEmpty() && clickedPiece.color == board.sideToMove) {
                                            selectedRank = rank;
                                            selectedFile = file;
                                        } else {
                                            selectedRank = -1;
                                            selectedFile = -1;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    
                    if (selectedRank != -1 && selectedFile != -1) {
                        auto legalMoves = MoveGenerator::generateLegalMoves(board, board.sideToMove);
                        for (const auto& move : legalMoves) {
                            if (move.startRank == selectedRank && move.startFile == selectedFile) {
                                validDestinations.push_back({move.endRank, move.endFile});
                            }
                        }
                    }
                }
            }

            BeginDrawing();
            ClearBackground(RAYWHITE);
            
            bool flipBoard = (config.playerColor == PlayerColor::Black);
            renderer.drawBoard(screenWidth, screenHeight, board, selectedRank, selectedFile, validDestinations, flipBoard);
            
            // Draw Timers
            if (config.timeLimit != TimeLimit::Unlimited) {
                auto formatTime = [](float t) {
                    int m = (int)t / 60;
                    int s = (int)t % 60;
                    char buf[10];
                    snprintf(buf, sizeof(buf), "%02d:%02d", m, s);
                    return std::string(buf);
                };
                
                // Black Timer (Top Right)
                DrawRectangle(screenWidth - 110, 10, 100, 40, {0, 0, 0, 150});
                DrawText(formatTime(blackTimeRemaining).c_str(), screenWidth - 100, 20, 20, WHITE);
                
                // White Timer (Bottom Right)
                DrawRectangle(screenWidth - 110, screenHeight - 50, 100, 40, {0, 0, 0, 150});
                DrawText(formatTime(whiteTimeRemaining).c_str(), screenWidth - 100, screenHeight - 40, 20, WHITE);
            }

            if (gameState != 0) {
                renderer.drawGameOver(screenWidth, screenHeight, gameState);
                // Draw a menu button
                if (GuiButtonAdv({(float)screenWidth / 2 - 100, (float)screenHeight / 2 + 50, 200, 50}, "Main Menu", false)) {
                    inMenu = true;
                }
            }
            
            EndDrawing();
        }
    }

    CloseWindow();
    return 0;
}
