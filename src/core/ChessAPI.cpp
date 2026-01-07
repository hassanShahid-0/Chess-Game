#include "ChessAPI.h"
#include "GameRules.h"
#include "../utils/Types.h"
#include <sstream>
#include <fstream>
#include <vector>

using namespace std;

// Session file to persist game state
const string ChessAPI::SESSION_FILE = "chess_session.bin";

// Constructor - Load existing game or start new
ChessAPI::ChessAPI() {
    aiEnabled = false;
    
    // Try to load existing session
    ifstream file(SESSION_FILE, ios::binary);
    if (file.good()) {
        file.close();
        loadState();
    } else {
        game.startNewGame();
        saveState();
    }
}

// Save game state to file
void ChessAPI::saveState() {
    ofstream file(SESSION_FILE, ios::binary);
    if (!file.is_open()) return;
    
    const Board& board = game.getBoard();
    
    // Save each square's piece
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            Position pos(row, col);
            Piece piece = game.getPieceAt(pos);
            
            int pieceType = static_cast<int>(piece.getType());
            int pieceColor = static_cast<int>(piece.getColor());
            
            file.write(reinterpret_cast<const char*>(&pieceType), sizeof(int));
            file.write(reinterpret_cast<const char*>(&pieceColor), sizeof(int));
        }
    }
    
    // Save current player
    int currentPlayer = static_cast<int>(game.getCurrentPlayer());
    file.write(reinterpret_cast<const char*>(&currentPlayer), sizeof(int));
    
    // Save AI enabled
    file.write(reinterpret_cast<const char*>(&aiEnabled), sizeof(bool));
    
    // NEW: Save AI difficulty level
    int aiDifficulty = 1; // 0=none, 1=easy, 2=medium, 3=hard
    if (!aiEnabled) {
        aiDifficulty = 0;
    } else if (ai) {
        // Try to determine difficulty (we'll use medium as default)
        aiDifficulty = 2; // Assume medium
    }
    file.write(reinterpret_cast<const char*>(&aiDifficulty), sizeof(int));
    
    file.close();
}

// Load game state from file
void ChessAPI::loadState() {
    ifstream file(SESSION_FILE, ios::binary);
    if (!file.is_open()) {
        game.startNewGame();
        return;
    }
    
    // Temporary storage for pieces
    vector<pair<Position, Piece>> pieces;
    
    // Load each square's piece
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            int pieceType, pieceColor;
            
            file.read(reinterpret_cast<char*>(&pieceType), sizeof(int));
            file.read(reinterpret_cast<char*>(&pieceColor), sizeof(int));
            
            Position pos(row, col);
            PieceType type = static_cast<PieceType>(pieceType);
            Color color = static_cast<Color>(pieceColor);
            
            if (type != EMPTY) {
                pieces.push_back({pos, Piece(type, color)});
            }
        }
    }
    
    // Load current player
    int currentPlayer;
    file.read(reinterpret_cast<char*>(&currentPlayer), sizeof(int));
    
    // Load AI enabled
    bool savedAiEnabled;
    file.read(reinterpret_cast<char*>(&savedAiEnabled), sizeof(bool));
    
    // Load AI difficulty (if available)
    int aiDifficulty = 2; // default to medium
    file.read(reinterpret_cast<char*>(&aiDifficulty), sizeof(int));
    
    file.close();
    
    // CRITICAL: DON'T restore AI if it was just set by setAI()
    // Check if AI object already exists
    bool aiJustSet = (ai != nullptr);
    
    if (!aiJustSet) {
        // Restore AI from saved state
        aiEnabled = savedAiEnabled;
        if (aiEnabled) {
            switch(aiDifficulty) {
                case 1: ai = make_unique<AI>(EASY); break;
                case 2: ai = make_unique<AI>(MEDIUM); break;
                case 3: ai = make_unique<AI>(HARD); break;
                default: ai = make_unique<AI>(MEDIUM);
            }
        }
    }
    // If AI was just set, keep the current AI object
    
    // Apply to game
    game.reset();
    game.startNewGame();
    
    // Clear board
    Board& board = game.getBoard();
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            board.setPiece(Position(r, c), Piece(EMPTY, WHITE));
        }
    }
    
    // Set loaded pieces
    for (const auto& p : pieces) {
        board.setPiece(p.first, p.second);
    }
    
    // Restore current player
    Color savedPlayer = static_cast<Color>(currentPlayer);
    game.setCurrentPlayer(savedPlayer);
}

// Start new game
void ChessAPI::newGame() {
    game.reset();
    game.startNewGame();
    saveState();
}

// Set AI difficulty
void ChessAPI::setAI(const string& difficulty) {
    if (difficulty == "easy") {
        aiEnabled = true;
        ai = make_unique<AI>(EASY);
    } else if (difficulty == "medium") {
        aiEnabled = true;
        ai = make_unique<AI>(MEDIUM);
    } else if (difficulty == "hard") {
        aiEnabled = true;
        ai = make_unique<AI>(HARD);
    } else {
        aiEnabled = false;
        ai.reset();
    }
    saveState();
}

// Make a move
bool ChessAPI::makeMove(const string& from, const string& to) {
    Position fromPos = Position::fromString(from);
    Position toPos = Position::fromString(to);
    
    if (!fromPos.isValid() || !toPos.isValid()) {
        return false;
    }
    
    bool success = game.makeMove(fromPos, toPos);
    if (success) {
        saveState();
    }
    return success;
}

// Get AI move
string ChessAPI::getAIMove() {
    if (!aiEnabled || !ai) {
        return "{\"error\":\"AI not enabled\"}";
    }
    
    Move aiMove = ai->getBestMove(game, game.getCurrentPlayer(), true);
    
    if (!aiMove.isValid()) {
        return "{\"error\":\"No valid move\"}";
    }
    
    game.makeMove(aiMove);
    saveState();
    
    stringstream ss;
    ss << "{";
    ss << "\"from\":\"" << aiMove.getFrom().toString() << "\",";
    ss << "\"to\":\"" << aiMove.getTo().toString() << "\"";
    ss << "}";
    
    return ss.str();
}

// Undo move
bool ChessAPI::undoMove() {
    if (!game.canUndo()) {
        return false;
    }
    
    bool success = game.undoMove();
    
    // In AI mode, undo both moves (player + AI)
    if (success && aiEnabled && game.canUndo()) {
        game.undoMove();
    }
    
    if (success) {
        saveState();
    }
    
    return success;
}

// Get board as JSON
string ChessAPI::getBoardJSON() {
    stringstream ss;
    ss << "{\"squares\":[";
    
    const Board& board = game.getBoard();
    
    bool first = true;
    for (int row = 7; row >= 0; row--) {
        for (int col = 0; col < 8; col++) {
            if (!first) ss << ",";
            first = false;
            
            Position pos(row, col);
            Piece piece = game.getPieceAt(pos);
            
            ss << "{";
            ss << "\"row\":" << row << ",";
            ss << "\"col\":" << col << ",";
            
            if (piece.isEmpty()) {
                ss << "\"piece\":null";
            } else {
                ss << "\"piece\":{";
                ss << "\"type\":\"" << pieceTypeToString(piece.getType()) << "\",";
                ss << "\"color\":\"" << colorToString(piece.getColor()) << "\",";
                ss << "\"symbol\":\"" << piece.getSymbol() << "\"";
                ss << "}";
            }
            
            ss << "}";
        }
    }
    
    ss << "]}";
    return ss.str();
}

// Get game status as JSON
string ChessAPI::getGameStatusJSON() {
    stringstream ss;
    ss << "{";
    ss << "\"currentPlayer\":\"" << colorToString(game.getCurrentPlayer()) << "\",";
    ss << "\"isGameOver\":" << (game.isGameOver() ? "true" : "false") << ",";
    ss << "\"isCheck\":" << (game.isInCheck() ? "true" : "false") << ",";
    ss << "\"isCheckmate\":" << (game.isCheckmate() ? "true" : "false") << ",";
    ss << "\"moveCount\":" << game.getMoveCount();
    ss << "}";
    return ss.str();
}

// Get valid moves as JSON
string ChessAPI::getValidMovesJSON(const string& position) {
    Position pos = Position::fromString(position);
    
    if (!pos.isValid()) {
        return "{\"moves\":[]}";
    }
    
    vector<Move> moves = game.getValidMoves(pos);
    
    stringstream ss;
    ss << "{\"moves\":[";
    
    for (size_t i = 0; i < moves.size(); i++) {
        if (i > 0) ss << ",";
        ss << "{";
        ss << "\"to\":\"" << moves[i].getTo().toString() << "\"";  // ONLY "to", NO "from"
        ss << "}";
    }
    
    ss << "]}";
    return ss.str();
}

// Check if game is over
bool ChessAPI::isGameOver() const {
    return game.isGameOver();
}

// Get current player
string ChessAPI::getCurrentPlayer() const {
    return colorToString(game.getCurrentPlayer());
}