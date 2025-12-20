#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "../core/Board.h"
#include "../utils/Types.h"
#include <vector>

class Evaluator {
public:
    static int evaluate(const Board& board, Color perspective);
    static int evaluateMaterial(const Board& board);
    static int evaluatePosition(const Board& board);
    static int evaluateMobility(const Board& board, Color color);
    static int evaluateKingSafety(const Board& board, Color color);
    static int evaluatePawnStructure(const Board& board);

    static int getPieceValue(PieceType type);

    static int calculatePieceMobility(const Board& board, Position pos);
    static vector<Position> getReachableSquares(const Board& board, Position pos);
    static bool isSquareControlled(const Board& board, Position pos, Color color);
    static int countControlledSquares(const Board& board, Color color);

    private:
    static int getPositionBonus(PieceType type, Position pos, Color color);
};
#endif // EVALUATOR_H