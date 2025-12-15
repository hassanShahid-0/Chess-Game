#include "Game.h"
#include <iostream>
#include <cmath>
#include <cstdlib>

using namespace std;

// ============================================================================
// CONSTRUCTORS
// ============================================================================

Game::Game() {
    startNewGame();
}

// ============================================================================
// GAME CONTROL
// ============================================================================

void Game::startNewGame() {
    board.initializeBoard();
    moveHistory.clear();
    
    // Clear the stack
    while (!boardHistory.empty()) {
        boardHistory.pop();
    }
    
    currentPlayer = WHITE;
    gameOver = false;
    winner = NONE;
    lastMove = Move();
    moveCount = 0;
}

bool Game::makeMove(Position from, Position to) {
    // Create move object
    Piece piece = board.getPiece(from);
    Piece captured = board.getPiece(to);
    
    Move move(from, to, piece.getType(), captured.getType());
    
    return makeMove(move);
}

bool Game::makeMove(Move move) {
    if (gameOver) {
        cout << "Game is over! Start a new game.\n";
        return false;
    }
    
    Position from = move.getFrom();
    Position to = move.getTo();
    
    // Validate move
    if (!isValidMove(move)) {
        return false;
    }
    
    // Get piece information
    Piece piece = board.getPiece(from);
    move.setMovedPiece(piece.getType());
    
    Piece captured = board.getPiece(to);
    if (!captured.isEmpty()) {
        move.setCapturedPiece(captured.getType());
    }
    
    // Handle special moves
    handleSpecialMoves(move);
    
    // Save current board state to stack (STACK OPERATION!)
    saveCurrentBoardState();
    
    // Execute the move
    board.executeMove(move);
    
    // Add to move history (LINKED LIST OPERATION!)
    moveHistory.addMove(move);
    lastMove = move;
    moveCount++;
    
    // Switch player
    switchPlayer();
    
    // Check for game end
    checkGameEnd();
    
    return true;
}

bool Game::undoMove() {
    // STACK OPERATION: Pop previous board state
    if (boardHistory.empty()) {
        cout << "Cannot undo - no moves made yet.\n";
        return false;
    }
    
    // Restore previous board state
    board = boardHistory.top();
    boardHistory.pop();
    
    // Remove last move from history (LINKED LIST OPERATION!)
    moveHistory.removeLastMove();
    
    // Switch player back
    switchPlayer();
    
    // Update last move
    if (!moveHistory.isEmpty()) {
        lastMove = moveHistory.getLastMove();
    } else {
        lastMove = Move();
    }
    
    moveCount--;
    
    // Game is no longer over after undo
    gameOver = false;
    winner = NONE;
    
    return true;
}

bool Game::canUndo() const {
    return !boardHistory.empty();
}

// ============================================================================
// GAME STATE
// ============================================================================

bool Game::isGameOver() const {
    return gameOver;
}

Color Game::getCurrentPlayer() const {
    return currentPlayer;
}

Color Game::getWinner() const {
    return winner;
}

Move Game::getLastMove() const {
    return lastMove;
}

int Game::getMoveCount() const {
    return moveCount;
}

string Game::getGameStatus() const {
    if (!gameOver) {
        string player = (currentPlayer == WHITE) ? "White" : "Black";
        
        if (isInCheck()) {
            return player + " is in check!";
        }
        return player + "'s turn";
    }
    
    if (winner != NONE) {
        string winnerName = (winner == WHITE) ? "White" : "Black";
        return "Checkmate! " + winnerName + " wins!";
    }
    
    return "Stalemate! Game is a draw.";
}

// ============================================================================
// MOVE VALIDATION
// ============================================================================

bool Game::isValidMove(Position from, Position to) const {
    // Check basic validity
    if (!board.isPositionValid(from) || !board.isPositionValid(to)) {
        return false;
    }
    
    // Check if there's a piece to move
    Piece piece = board.getPiece(from);
    if (piece.isEmpty()) {
        return false;
    }
    
    // Check if it's the correct player's piece
    if (piece.getColor() != currentPlayer) {
        return false;
    }
    
    // Check if move follows piece rules
    if (!GameRules::isBasicMoveLegal(board, from, to, lastMove)) {
        return false;
    }
    
    // Check if move would leave king in check (BACKTRACKING)
    Move testMove(from, to, piece.getType(), board.getPiece(to).getType());
    if (GameRules::wouldBeInCheck(board, testMove, currentPlayer)) {
        return false;
    }
    
    return true;
}

bool Game::isValidMove(Move move) const {
    return isValidMove(move.getFrom(), move.getTo());
}

vector<Move> Game::getValidMoves(Position pos) const {
    Piece piece = board.getPiece(pos);
    
    // Must be current player's piece
    if (piece.isEmpty() || piece.getColor() != currentPlayer) {
        return vector<Move>();
    }
    
    // Get valid moves from GameRules
    Board tempBoard = board.copy();
    return GameRules::getValidMovesForPiece(tempBoard, pos, lastMove);
}

vector<Move> Game::getAllValidMovesForCurrentPlayer() const {
    Board tempBoard = board.copy();
    return GameRules::getAllValidMoves(tempBoard, currentPlayer, lastMove);
}

// ============================================================================
// CHECK CONDITIONS
// ============================================================================

bool Game::isInCheck() const {
    return GameRules::isKingInCheck(board, currentPlayer);
}

bool Game::isInCheck(Color color) const {
    return GameRules::isKingInCheck(board, color);
}

bool Game::isCheckmate() const {
    Board tempBoard = board.copy();
    return GameRules::isCheckmate(tempBoard, currentPlayer);
}

bool Game::isCheckmate(Color color) const {
    Board tempBoard = board.copy();
    return GameRules::isCheckmate(tempBoard, color);
}

bool Game::isStalemate() const {
    Board tempBoard = board.copy();
    return GameRules::isStalemate(tempBoard, currentPlayer);
}

bool Game::isStalemate(Color color) const {
    Board tempBoard = board.copy();
    return GameRules::isStalemate(tempBoard, color);
}

// ============================================================================
// BOARD ACCESS
// ============================================================================

const Board& Game::getBoard() const {
    return board;
}

Board& Game::getBoard() {
    return board;
}

Piece Game::getPieceAt(Position pos) const {
    return board.getPiece(pos);
}

// ============================================================================
// MOVE HISTORY ACCESS
// ============================================================================

const MoveHistory& Game::getMoveHistory() const {
    return moveHistory;
}

vector<Move> Game::getAllMoves() const {
    return moveHistory.getAllMoves();
}

// ============================================================================
// DISPLAY
// ============================================================================

void Game::printBoard() const {
    board.printBoard();
}

void Game::printBoardWithCoordinates() const {
    board.printBoardWithCoordinates();
}

void Game::printGameStatus() const {
    cout << "\n" << string(50, '=') << "\n";
    cout << "  GAME STATUS\n";
    cout << string(50, '=') << "\n";
    cout << "Move: " << moveCount << "\n";
    cout << "Current Player: " << colorToString(currentPlayer) << "\n";
    cout << "Status: " << getGameStatus() << "\n";
    cout << string(50, '=') << "\n\n";
}

void Game::printMoveHistory() const {
    moveHistory.printHistory();
}

void Game::printCapturedPieces() const {
    board.printCapturedPieces();
}

// ============================================================================
// UTILITY
// ============================================================================

void Game::reset() {
    startNewGame();
}

string Game::toFEN() const {
    // Simplified FEN notation (optional advanced feature)
    // You can implement full FEN later if needed
    return "FEN notation not implemented yet";
}

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

void Game::switchPlayer() {
    currentPlayer = oppositeColor(currentPlayer);
}

void Game::checkGameEnd() {
    // Check for checkmate
    if (isCheckmate()) {
        gameOver = true;
        winner = oppositeColor(currentPlayer);  // Previous player wins
        return;
    }
    
    // Check for stalemate
    if (isStalemate()) {
        gameOver = true;
        winner = NONE;  // Draw
        return;
    }
    
    // Check for insufficient material
    if (GameRules::isDrawByInsufficientMaterial(board)) {
        gameOver = true;
        winner = NONE;  // Draw
        return;
    }
}

bool Game::handleSpecialMoves(Move& move) {
    Position from = move.getFrom();
    Position to = move.getTo();
    Piece piece = board.getPiece(from);
    
    // Check for castling
    if (piece.getType() == KING && abs(to.col - from.col) == 2) {
        move.setCastling(true);
        return true;
    }
    
    // Check for en passant
    if (piece.getType() == PAWN) {
        int direction = (piece.getColor() == WHITE) ? 1 : -1;
        if (to.col != from.col && board.isSquareEmpty(to)) {
            // Diagonal move to empty square = en passant
            move.setEnPassant(true);
            return true;
        }
        
        // Check for promotion
        int promotionRow = (piece.getColor() == WHITE) ? 7 : 0;
        if (to.row == promotionRow) {
            move.setPromotion(QUEEN);  // Default to queen promotion
            return true;
        }
    }
    
    return false;
}

void Game::saveCurrentBoardState() {
    // STACK OPERATION: Push current board state
    boardHistory.push(board.copy());
}