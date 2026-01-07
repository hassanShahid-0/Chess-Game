#ifndef GAME_H
#define GAME_H

#include "Board.h"
#include "Move.h"
#include "MoveHistory.h"
#include "GameRules.h"
#include "../utils/Types.h"
#include <stack>
#include <string>

using namespace std;

class Game {
    private:
    Board board;
    MoveHistory moveHistory;
    stack<Board> boardHistory;
    Color currentPlayer;
    bool gameOver;
    Color winner;
    Move lastMove;
    int moveCount;

    public:
    Game();
    void startNewGame();
    bool makeMove(Position from, Position to);
    bool makeMove(Move move);
    bool undoMove();
    bool canUndo() const;


    bool isGameOver() const;
    Color getCurrentPlayer() const;
    Color getWinner() const;
    Move getLastMove() const;
    int getMoveCount() const;

    string getGameStatus() const;
    void setCurrentPlayer(Color player);

    bool isValidMove (Position from, Position to) const;
    bool isValidMove (Move move) const;

    vector<Move> getAllValidMovesForCurrentPlayer() const;
    vector<Move> getValidMoves(Position pos) const;

    bool isInCheck() const;
    bool isInCheck(Color color) const;
    bool isCheckmate() const;
    bool isCheckmate(Color color) const;
    bool isStalemate() const;
    bool isStalemate(Color color) const;

    const Board& getBoard() const;
    Board& getBoard();

    Piece getPieceAt(Position pos) const;

    //Move History Access
    const MoveHistory& getMoveHistory() const;

    vector<Move> getAllMoves() const;

    //Display
    void printBoard() const;
    void printBoardWithCoordinates() const;
    void printGameStatus() const;
    void printMoveHistory() const;
    void printCapturedPieces() const;

    //Utility
    void reset();

    string toFEN() const;

    private:
    void switchPlayer();
    void checkGameEnd();
    bool handleSpecialMoves(Move& move);
    void saveCurrentBoardState();


};

#endif