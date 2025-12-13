#ifndef GAMERULES_H
#define GAMERULES_H

#include "Board.h"
#include "Move.h"
#include "../utils/Types.h"
#include <vector>

class GameRules {
public:
    static bool isValidPawnMove(const Board& board, Position from, Position to, Color color, const Move& lastMove);
    static bool isValidKnightMove(const Board& board, Position from, Position to);
    static bool isValidBishopMove(const Board& board, Position from, Position to);
    static bool isValidRookMove(const Board& board, Position from, Position to);
    static bool isValidQueenMove(const Board& board, Position from, Position to);
    static bool isValidKingMove(const Board& board, Position from, Position to);
    static bool isPathClear(const Board& board, Position from, Position to);
    static bool canCastle(const Board& board, Position kingPos, Position rookPos);
    static bool canEnPassant(const Board& board, Position from, Position to, const Move& lastMove);
    static bool canPromote(const Board& board, Position pos);
    static bool isKingInCheck(const Board& board, Color kingColor);
    static bool wouldBeInCheck(Board board, Move move, Color kingColor);
    static bool isSquareAttacked(const Board& board, Position pos, Color attackerColor);
    static bool isCheckmate(Board& board, Color color);
    static bool isStalemate(Board& board, Color color);
    static bool isDrawByInsufficientMaterial(const Board& board);
    static std::vector<Move> getValidMovesForPiece(Board& board, Position pos, const Move& lastMove);
    static std::vector<Move> getAllValidMoves(Board& board, Color color, const Move& lastMove);
    static bool isBasicMoveLegal(const Board& board, Position from, Position to, const Move& lastMove);
    static std::vector<Move> getPossibleMovesForPiece(const Board& board, Position pos, const Move& lastMove);

private:
    static std::vector<Move> generatePawnMoves(const Board& board, Position pos, const Move& lastMove);
    static std::vector<Move> generateKnightMoves(const Board& board, Position pos);
    static std::vector<Move> generateBishopMoves(const Board& board, Position pos);
    static std::vector<Move> generateRookMoves(const Board& board, Position pos);
    static std::vector<Move> generateQueenMoves(const Board& board, Position pos);
    static std::vector<Move> generateKingMoves(const Board& board, Position pos);
};

#endif // GAMERULES_H
