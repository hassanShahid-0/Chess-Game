@echo off
echo Compiling Chess Game...

g++ -std=c++11 ^
    tests/test_game.cpp ^
    src/core/Piece.cpp ^
    src/core/Move.cpp ^
    src/core/Board.cpp ^
    src/core/MoveHistory.cpp ^
    src/core/GameRules.cpp ^
    src/core/Game.cpp ^
    src/engine/TranspositionTable.cpp ^
    src/engine/Evaluator.cpp ^
    src/engine/AI.cpp ^
    -o chess_game.exe

if %errorlevel% == 0 (
    echo.
    echo ✓ Compilation successful!
    echo.
    echo Running game...
    chess_game.exe
) else (
    echo.
    echo ✗ Compilation failed!
    pause
)