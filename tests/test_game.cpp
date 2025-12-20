#include <iostream>
#include <string>
#include "../src/core/Game.h"
#include "../src/engine/AI.h"

using namespace std;

// ============================================================================
// GLOBAL PLAYER INFO (TEST-ONLY)
// ============================================================================
string whitePlayerName = "White";
string blackPlayerName = "Black";

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void printStartupMenu() {
    cout << "\n==============================\n";
    cout << "        CHESS GAME\n";
    cout << "==============================\n";
    cout << "1. Player vs Player\n";
    cout << "2. Player vs Computer\n";
    cout << "3. Exit\n";
    cout << "Choose option: ";
}

Difficulty chooseDifficulty() {
    int choice;
    cout << "\nChoose Difficulty:\n";
    cout << "1. Easy\n";
    cout << "2. Medium\n";
    cout << "3. Hard\n";
    cout << "Choice: ";
    cin >> choice;
    cin.ignore();

    switch (choice) {
        case 1: return EASY;
        case 2: return MEDIUM;
        case 3: return HARD;
        default:
            cout << "Invalid choice. Defaulting to Medium.\n";
            return MEDIUM;
    }
}

void printInstructions() {
    cout << "\n-------------------------------\n";
    cout << "HOW TO PLAY\n";
    cout << "-------------------------------\n";
    cout << "Enter moves like: e2 e4\n";
    cout << "undo  -> undo last move\n";
    cout << "quit  -> return to menu\n";
    cout << "help  -> show instructions\n";
    cout << "-------------------------------\n\n";
}

Position parsePosition(const string& pos) {
    if (pos.length() != 2) return Position(-1, -1);
    return Position::fromString(pos);
}

// ============================================================================
// GAME MODES
// ============================================================================

void playPvP(Game& game) {
    printInstructions();

    while (!game.isGameOver()) {
        clearScreen();
        game.printBoardWithCoordinates();
        game.printGameStatus();
        game.printCapturedPieces();

        Color current = game.getCurrentPlayer();
        string currentName = (current == WHITE) ? whitePlayerName : blackPlayerName;

        cout << "\nMove " << game.getMoveCount() + 1 << " | ";
        cout << currentName << "'s turn (" << colorToString(current) << ")\n";
        cout << "Enter move: ";

        string from, to;
        cin >> from;

        if (from == "quit") return;

        if (from == "undo") {
            game.undoMove();
            continue;
        }

        if (from == "help") {
            printInstructions();
            cin.get();
            continue;
        }

        cin >> to;

        if (!game.makeMove(parsePosition(from), parsePosition(to))) {
            cout << "Invalid move! Press Enter...";
            cin.ignore();
            cin.get();
        }
    }

    clearScreen();
    game.printBoardWithCoordinates();
    cout << "\nGAME OVER\n";
    cout << game.getGameStatus() << "\n";
}

void playPvAI(Game& game, Difficulty difficulty) {
    AI ai(difficulty);
    printInstructions();

    while (!game.isGameOver()) {
        clearScreen();
        game.printBoardWithCoordinates();
        game.printGameStatus();
        game.printCapturedPieces();

        Color current = game.getCurrentPlayer();

        if (current == WHITE) {
            cout << "\n" << whitePlayerName << "'s turn (White)\n";
            cout << "Enter move: ";

            string from, to;
            cin >> from;

            if (from == "quit") return;

            cin >> to;

            if (!game.makeMove(parsePosition(from), parsePosition(to))) {
                cout << "Invalid move! Press Enter...";
                cin.ignore();
                cin.get();
            }
        } else {
            cout << "\nAI is thinking...\n";
            Move aiMove = ai.getBestMove(game, BLACK);
            game.makeMove(aiMove);
        }
    }

    clearScreen();
    game.printBoardWithCoordinates();
    cout << "\nGAME OVER\n";
    cout << game.getGameStatus() << "\n";
}

// ============================================================================
// MAIN
// ============================================================================

int main() {
    while (true) {
        clearScreen();
        printStartupMenu();

        int choice;
        cin >> choice;
        cin.ignore();

        if (choice == 3) {
            cout << "\nThanks for playing!\n";
            break;
        }

        Game game;

        if (choice == 1) {
            cout << "\nEnter Player 1 name (White): ";
            getline(cin, whitePlayerName);
            cout << "Enter Player 2 name (Black): ";
            getline(cin, blackPlayerName);

            playPvP(game);
        }
        else if (choice == 2) {
            cout << "\nEnter your name: ";
            getline(cin, whitePlayerName);
            blackPlayerName = "Computer";

            Difficulty diff = chooseDifficulty();
            playPvAI(game, diff);
        }
        else {
            cout << "Invalid choice. Press Enter...";
            cin.get();
        }

        cout << "\nPlay again? (y/n): ";
        char again;
        cin >> again;
        cin.ignore();
        if (again != 'y' && again != 'Y')
            break;
    }

    return 0;
}
