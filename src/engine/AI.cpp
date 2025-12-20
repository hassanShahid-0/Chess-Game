#include "AI.h"
#include "../core/GameRules.h"
#include <algorithm>
#include <limits>
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;


DifficultyConfig getConfig(Difficulty diff) {
    DifficultyConfig config;
    
    switch (diff) {
        case EASY:
            config.depth = 2;
            config.useTransTable = false;
            config.useAdvancedEval = false;
            config.randomness = 30;
            break;
            
        case MEDIUM:
            config.depth = 4;
            config.useTransTable = true;
            config.useAdvancedEval = true;
            config.randomness = 10;
            break;
            
        case HARD:
            config.depth = 6;
            config.useTransTable = true;
            config.useAdvancedEval = true;
            config.randomness = 0;
            break;
    }
    
    return config;
}

AI::AI(Difficulty diff) {
    setDifficulty(diff);
    nodesEvaluated = 0;
    cacheHits = 0;
    prunedBranches = 0;
    srand(time(0));
}

Move AI::getBestMove(Game& game, Color color) {
    resetStatistics();
    
    DifficultyConfig config = getDifficultyConfig();
    
    cout << "\n🤖 AI is thinking";
    cout.flush();
    
    Board board = game.getBoard().copy();
    vector<Move> moves = GameRules::getAllValidMoves(board, color, game.getLastMove());
    
    if (moves.empty()) {
        cout << " No valid moves!\n";
        return Move();
    }
    
    if (config.randomness > 0) {
        int random = rand() % 100;
        if (random < config.randomness) {
            int randomIndex = rand() % moves.size();
            cout << " (random move) Done! 🎲\n";
            return moves[randomIndex];
        }
    }
    
    moves = orderMoves(moves, board);
    
    Move bestMove = moves[0];
    int bestScore = numeric_limits<int>::min();
    int alpha = numeric_limits<int>::min();
    int beta = numeric_limits<int>::max();
    
    for (Move& move : moves) {
        Board tempBoard = board.copy();
        tempBoard.executeMove(move);
        int score = minimax(tempBoard, config.depth - 1, alpha, beta, 
                           false, color, move, config);
        
        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
        
        alpha = max(alpha, score);
        
        cout << ".";
        cout.flush();
    }
    
    cout << " Done! ✓\n";
    printThinkingInfo();
    
    return bestMove;
}

int AI::minimax(Board& board, int depth, int alpha, int beta, 
                bool isMaximizing, Color color, Move lastMove,
                const DifficultyConfig& config) {
    
    nodesEvaluated++;
    
    if (depth == 0) {
        if (config.useAdvancedEval) {
            return Evaluator::evaluate(board, color);
        } else {
            return Evaluator::evaluateMaterial(board) * (color == WHITE ? 1 : -1);
        }
    }
    
    if (config.useTransTable) {
        string boardKey = TranspositionTable::generateKey(board);
        int cachedScore;
        if (transTable.lookup(boardKey, cachedScore)) {
            cacheHits++;
            return cachedScore;
        }
    }
    
    Color currentColor = isMaximizing ? color : oppositeColor(color);
    
    if (GameRules::isCheckmate(board, currentColor)) {
        return isMaximizing ? -20000 + depth : 20000 - depth;
    }
    
    if (GameRules::isStalemate(board, currentColor)) {
        return 0;
    }
    
    vector<Move> moves = GameRules::getAllValidMoves(board, currentColor, lastMove);
    
    if (moves.empty()) {
        return 0;
    }
    
    int bestScore;
    
    if (isMaximizing) {
        bestScore = numeric_limits<int>::min();
        for (Move& move : moves) {
            Board tempBoard = board.copy();
            tempBoard.executeMove(move);
            int score = minimax(tempBoard, depth - 1, alpha, beta, 
                               false, color, move, config);
            bestScore = max(bestScore, score);
            alpha = max(alpha, score);
            if (beta <= alpha) {
                prunedBranches++;
                break;
            }
        }
    } else {
        bestScore = numeric_limits<int>::max();
        for (Move& move : moves) {
            Board tempBoard = board.copy();
            tempBoard.executeMove(move);
            int score = minimax(tempBoard, depth - 1, alpha, beta, 
                               true, color, move, config);
            bestScore = min(bestScore, score);
            beta = min(beta, score);
            if (beta <= alpha) {
                prunedBranches++;
                break;
            }
        }
    }
    
    if (config.useTransTable) {
        string boardKey = TranspositionTable::generateKey(board);
        transTable.store(boardKey, bestScore);
    }
    
    return bestScore;
}

void AI::setDifficulty(Difficulty diff) {
    difficulty = diff;
    DifficultyConfig config = getDifficultyConfig();
    maxDepth = config.depth;
}

Difficulty AI::getDifficulty() const {
    return difficulty;
}

int AI::getMaxDepth() const {
    return maxDepth;
}

DifficultyConfig AI::getDifficultyConfig() const {
    return getConfig(difficulty);
}

int AI::getNodesEvaluated() const {
    return nodesEvaluated;
}

int AI::getCacheHits() const {
    return cacheHits;
}

int AI::getPrunedBranches() const {
    return prunedBranches;
}

void AI::resetStatistics() {
    nodesEvaluated = 0;
    cacheHits = 0;
    prunedBranches = 0;
}

void AI::printThinkingInfo() const {
    cout << "\n╔══════════════════════════════════╗\n";
    cout << "║     AI THINKING STATISTICS       ║\n";
    cout << "╠══════════════════════════════════╣\n";
    cout << "║ Difficulty: ";
    
    switch (difficulty) {
        case EASY:   cout << "Easy  "; break;
        case MEDIUM: cout << "Medium"; break;
        case HARD:   cout << "Hard  "; break;
    }
    cout << "                ║\n";
    
    cout << "║ Search Depth: " << maxDepth << "                  ║\n";
    cout << "║ Nodes Evaluated: " << nodesEvaluated;
    
    int padding = 17 - to_string(nodesEvaluated).length();
    for (int i = 0; i < padding; i++) cout << " ";
    cout << "║\n";
    
    cout << "║ Cache Hits: " << cacheHits;
    padding = 22 - to_string(cacheHits).length();
    for (int i = 0; i < padding; i++) cout << " ";
    cout << "║\n";
    
    cout << "║ Branches Pruned: " << prunedBranches;
    padding = 17 - to_string(prunedBranches).length();
    for (int i = 0; i < padding; i++) cout << " ";
    cout << "║\n";
    
    cout << "║ Cache Hit Rate: " << transTable.getHitRate() << "%";
    padding = 18 - to_string((int)transTable.getHitRate()).length();
    for (int i = 0; i < padding; i++) cout << " ";
    cout << "║\n";
    
    cout << "╚══════════════════════════════════╝\n\n";
}

vector<Move> AI::orderMoves(vector<Move>& moves, const Board& board) {
    sort(moves.begin(), moves.end(), [&](const Move& a, const Move& b) {
        int scoreA = 0;
        int scoreB = 0;
        
        if (a.isCapture()) {
            scoreA += 10 + Evaluator::getPieceValue(a.getCapturedPiece()) / 10;
        }
        if (b.isCapture()) {
            scoreB += 10 + Evaluator::getPieceValue(b.getCapturedPiece()) / 10;
        }
        
        if (a.isPromotionMove()) scoreA += 9;
        if (b.isPromotionMove()) scoreB += 9;
        
        if (a.isCastlingMove()) scoreA += 5;
        if (b.isCastlingMove()) scoreB += 5;
        
        Position toA = a.getTo();
        Position toB = b.getTo();
        
        if ((toA.row >= 3 && toA.row <= 4) && (toA.col >= 3 && toA.col <= 4)) {
            scoreA += 1;
        }
        if ((toB.row >= 3 && toB.row <= 4) && (toB.col >= 3 && toB.col <= 4)) {
            scoreB += 1;
        }
        
        return scoreA > scoreB;
    });
    
    return moves;
}
