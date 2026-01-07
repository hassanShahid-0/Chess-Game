#ifndef CHESSAPI_H
#define CHESSAPI_H

#include "Game.h"
#include "../engine/AI.h"
#include <string>
#include <memory>

using namespace std;

class ChessAPI {
    private:
        Game game;
        unique_ptr<AI> ai;
        bool aiEnabled;
        
        // Session file path
        static const string SESSION_FILE;
        
        // Save/Load game state
        void saveState();
        void loadState();

    public:
        ChessAPI();
        void newGame();
        void setAI(const string& difficulty);

        bool makeMove(const string& from, const string& to);
        string getAIMove();
        bool undoMove();

        string getBoardJSON();
        string getGameStatusJSON();
        string getValidMovesJSON(const string& position);

        bool isGameOver() const;
        string getCurrentPlayer() const;
};
#endif // CHESSAPI_H