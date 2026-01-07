#include "../core/ChessAPI.h"
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "{\"error\":\"Usage: chess_cli <command> [args...]\"}" << endl;
        return 1;
    }
    
    // CRITICAL: Static to maintain state between CLI calls
    static ChessAPI api;
    
    string command = argv[1];
    
    try {
        if (command == "newgame") {
            api.newGame();
            cout << "{\"success\":true}" << endl;
        }
        else if (command == "setai") {
            if (argc < 3) {
                cerr << "{\"error\":\"Missing difficulty\"}" << endl;
                return 1;
            }
            api.setAI(argv[2]);
            cout << "{\"success\":true}" << endl;
        }
        else if (command == "move") {
            if (argc < 4) {
                cerr << "{\"error\":\"Missing from/to positions\"}" << endl;
                return 1;
            }
            bool success = api.makeMove(argv[2], argv[3]);
            cout << "{\"success\":" << (success ? "true" : "false") << "}" << endl;
        }
        else if (command == "aimove") {
            cout << api.getAIMove() << endl;
        }
        else if (command == "undo") {
            bool success = api.undoMove();
            cout << "{\"success\":" << (success ? "true" : "false") << "}" << endl;
        }
        else if (command == "board") {
            cout << api.getBoardJSON() << endl;
        }
        else if (command == "status") {
            cout << api.getGameStatusJSON() << endl;
        }
        else if (command == "validmoves") {
            if (argc < 3) {
                cerr << "{\"error\":\"Missing position\"}" << endl;
                return 1;
            }
            cout << api.getValidMovesJSON(argv[2]) << endl;
        }
        else {
            cerr << "{\"error\":\"Unknown command: " << command << "\"}" << endl;
            return 1;
        }
    }
    catch (const exception& e) {
        cerr << "{\"error\":\"" << e.what() << "\"}" << endl;
        return 1;
    }
    
    return 0;
}