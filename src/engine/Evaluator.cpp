#include "Evaluator.h"
#include "../core/GameRules.h"
#include <cmath>

using namespace std;

int Evaluator::getPieceValue(PieceType type) {
    switch (type) {
        case PAWN:   return 100;
        case KNIGHT: return 320;
        case BISHOP: return 330;
        case ROOK:   return 500;
        case QUEEN:  return 900;
        case KING:   return 20000;
        default:     return 0;
    }
}


int Evaluator::evaluate(const Board& board, Color perspective) {
    int score = 0;
    
    score += evaluateMaterial(board);
    
    score += evaluatePosition(board) / 2;
    
    int whiteMobility = evaluateMobility(board, WHITE);
    int blackMobility = evaluateMobility(board, BLACK);
    score += (whiteMobility - blackMobility) / 3;
    
    int whiteKingSafety = evaluateKingSafety(board, WHITE);
    int blackKingSafety = evaluateKingSafety(board, BLACK);
    score += (whiteKingSafety - blackKingSafety) / 4;

    score += evaluatePawnStructure(board) / 5;
    
    return (perspective == WHITE) ? score : -score;
}


int Evaluator::evaluateMaterial(const Board& board) {
    int score = 0;
    
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            Piece piece = board.getPiece(row, col);
            if (!piece.isEmpty()) {
                int value = getPieceValue(piece.getType());
                
                if (piece.getColor() == WHITE) {
                    score += value;
                } else {
                    score -= value;
                }
            }
        }
    }
    
    return score;
}

int Evaluator::evaluatePosition(const Board& board) {
    int score = 0;
    
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            Piece piece = board.getPiece(row, col);
            if (piece.isEmpty()) continue;
            
            Position pos(row, col);
            int bonus = getPositionBonus(piece.getType(), pos, piece.getColor());
            
            if (piece.getColor() == WHITE) {
                score += bonus;
            } else {
                score -= bonus;
            }
        }
    }
    
    return score;
}


int Evaluator::evaluateMobility(const Board& board, Color color) {
    
    int mobilityScore = 0;
    
    vector<Position> pieces = board.findAllPieces(color);
    
    for (const Position& pos : pieces) {
        int mobility = calculatePieceMobility(board, pos);
        mobilityScore += mobility;
    }
    
    return mobilityScore * 10;
}

int Evaluator::calculatePieceMobility(const Board& board, Position pos) {
    
    vector<Position> reachable = getReachableSquares(board, pos);
    return reachable.size();
}

vector<Position> Evaluator::getReachableSquares(const Board& board, Position pos) {
 
    vector<Position> reachable;
    Piece piece = board.getPiece(pos);
    
    if (piece.isEmpty()) return reachable;
    
    Move dummyLastMove;
    vector<Move> moves = GameRules::getPossibleMovesForPiece(board, pos, dummyLastMove);
    
    for (const Move& move : moves) {
        reachable.push_back(move.getTo());
    }
    
    return reachable;
}

bool Evaluator::isSquareControlled(const Board& board, Position pos, Color color) {
    return GameRules::isSquareAttacked(board, pos, color);
}

int Evaluator::countControlledSquares(const Board& board, Color color) {
    int controlled = 0;
    
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            Position pos(row, col);
            if (isSquareControlled(board, pos, color)) {
                controlled++;
            }
        }
    }
    
    return controlled;
}

int Evaluator::evaluateKingSafety(const Board& board, Color color) {
    Position kingPos = board.findKing(color);
    
    if (!kingPos.isValid()) {
        return -10000;  
    }
    
    int safety = 0;
    
    if (color == WHITE) {
        if (kingPos.row == 0) {
            safety += 30;  
            
            if (kingPos.col <= 2 || kingPos.col >= 6) {
                safety += 20;
            }
        }
    } else {  // BLACK
        if (kingPos.row == 7) {
            safety += 30;
            if (kingPos.col <= 2 || kingPos.col >= 6) {
                safety += 20;
            }
        }
    }
    
    if (kingPos.col >= 2 && kingPos.col <= 5) {
        if ((color == WHITE && kingPos.row >= 3) || 
            (color == BLACK && kingPos.row <= 4)) {
            safety -= 40;  // King too exposed
        }
    }
    
    return safety;
}

int Evaluator::evaluatePawnStructure(const Board& board) {
    int score = 0;
    
    for (int col = 0; col < 8; col++) {
        int whitePawns = 0;
        int blackPawns = 0;
        
        for (int row = 0; row < 8; row++) {
            Piece piece = board.getPiece(row, col);
            if (piece.getType() == PAWN) {
                if (piece.getColor() == WHITE) whitePawns++;
                else blackPawns++;
            }
        }
        
        if (whitePawns > 1) score -= 50 * (whitePawns - 1);
        if (blackPawns > 1) score += 50 * (blackPawns - 1);
    }
    
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            Piece piece = board.getPiece(row, col);
            
            if (piece.getType() == PAWN) {
                if (piece.getColor() == WHITE) {
                    score += row * 10;
                } else {
                    score -= (7 - row) * 10;
                }
            }
        }
    }
    
    return score;
}

int Evaluator::getPositionBonus(PieceType type, Position pos, Color color) {
    int row = (color == WHITE) ? pos.row : (7 - pos.row);
    int col = pos.col;
    
    switch (type) {
        case PAWN:
            return row * 5;
            
        case KNIGHT:
            if ((row == 3 || row == 4) && (col >= 2 && col <= 5)) {
                return 30;  
            }
            if ((row == 2 || row == 5) && (col >= 2 && col <= 5)) {
                return 15;  
            }
            return 0;
            
        case BISHOP:
            if ((row == 3 || row == 4) && (col >= 2 && col <= 5)) {
                return 20;
            }
            return 0;
            
        case ROOK:
            if (row == 6) {
                return 20;  
            }
            return 0;
            
        case QUEEN:
            if ((row >= 2 && row <= 5) && (col >= 2 && col <= 5)) {
                return 10;
            }
            return 0;
            
        case KING:
            return 0;
            
        default:
            return 0;
    }
}