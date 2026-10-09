# ChessWithMe

A beautifully designed, fully functional Chess application built from scratch in C++ using Raylib. Featuring a custom-built Minimax AI engine, sleek user interface, and smooth gameplay mechanics.

## Features
- **Player vs Player (PvP):** Play against a friend locally. Choose which color is placed at the bottom of the board!
- **Player vs CPU (PvC):** Play against the built-in AI.
- **Adjustable Difficulty:** 
  - **Beginner:** Pawn-level AI (Fast)
  - **Intermediate:** Knight-level AI
  - **Advanced:** Bishop-level AI
  - **Pro:** Queen-level AI (Uses Alpha-Beta pruning & Move Ordering to calculate 5 moves deep)
- **Time Limits:** Play with unlimited time, or set 5, 10, or 15-minute timers.
- **Elegant UI:** A sleek dark-mode menu with embedded piece graphics and dynamic board orientation.

## How to Play (Windows)

**The easiest way to play is to download the pre-compiled game!**
1. Go to the **[Releases](../../releases)** page on this GitHub repository.
2. Download the `ChessWithMe_Windows.zip` file.
3. Extract the folder to your computer.
4. Double-click `ChessWithMe.exe` to launch the game!

## Building from Source

This project uses CMake and requires C++17.

### Prerequisites
- CMake (3.14 or higher)
- A C++17 compatible compiler (MSVC, GCC, or Clang)
- [Raylib](https://www.raylib.com/) (Automatically fetched by CMake)

### Build Instructions
1. Clone the repository.
2. Open a terminal in the project directory.
3. Run CMake to configure the project:
   ```bash
   cmake -B build
   ```
4. Build the executable (Release mode is recommended for faster AI):
   ```bash
   cmake --build build --config Release
   ```
5. Navigate to the `build/Release` folder (or just `build` depending on your OS/generator) and run `ChessWithMe.exe`. 
   > **Note:** The `assets/` folder must be copied to the same directory as the executable to load the piece graphics correctly!

## Architecture
- `main.cpp` - Game loop and sleek UI rendering.
- `Board` - Core piece representation, FEN parsing, and move logic.
- `MoveGenerator` - Calculates legal moves, sliding attacks, and checkmate detection.
- `Engine` - The AI opponent utilizing the Minimax algorithm with Alpha-Beta pruning.
- `Renderer` - Raylib integration for drawing the board, textures, and UI overlays.
