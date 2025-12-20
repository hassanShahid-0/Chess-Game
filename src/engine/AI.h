#ifndef AI_H
#define AI_H

#include "../core/Game.h"
#include "../core/Move.h"
#include "Evaluator.h"
#include "TranspositionTable.h"
#include <vector>

enum Difficulty {
    EASY = 0,
    MEDIUM = 1,
    HARD = 2
};

struct DifficultyConfig {
    int depth;
    bool useTransTable;
    bool useAdvancedEval;
    int randomness;
};

class AI {
private:
    Difficulty difficulty;
    int maxDepth;
    TranspositionTable transTable;
    
    int nodesEvaluated;
    int cacheHits;
    int prunedBranches;

public:
    AI(Difficulty diff = MEDIUM);
    
    Move getBestMove(Game& game, Color color);
    
    int minimax(Board& board, int depth, int alpha, int beta, bool isMaximizing, Color color, Move lastMove, const DifficultyConfig& config);
    
    void setDifficulty(Difficulty diff);
    Difficulty getDifficulty() const;
    int getMaxDepth() const;
    
    int getNodesEvaluated() const;
    int getCacheHits() const;
    int getPrunedBranches() const;
    void resetStatistics();
    
    void printThinkingInfo() const;

private:
    DifficultyConfig getDifficultyConfig() const;
    
    std::vector<Move> orderMoves(std::vector<Move>& moves, const Board& board);
};

#endif // AI_H