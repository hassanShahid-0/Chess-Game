#ifndef BOARD_H
#define BOARD_H

#include "Piece.h"
#include "Move.h"
#include "../utils/Types.h"
#include <vector>
#include <string>

class Board {
    private:
        Piece board[8][8];
        vector<Piece> whiteCaptured;
        vector<Piece> blackCaptured;
    public:
        Board();

        void initializeBoard();
        void clearBoard();

        Piece getPiece(Position pos) const;
        Piece getPiece(int row, int ccol) const;

        void setPiece(Position pos, Piece piece);
        void setPiece(int row, int col, Piece piece);

        bool isPositionValid(Position pos) const;
        bool isPositionValid(int row, int col) const;

        bool isSquareEmpty(Position pos) const;
        bool isSquareOccupiedByColor(Position pos, Color color) const;

        bool movePiece(Position from, Position to);
        bool executeMove(const Move& move);
        void addCapturedPiece(Piece piece);

        Position findKing(Color color) const;
        vector<Position> findAllPieces(Color color) const;
        vector<Position> findPieceTypes(PieceType type, Color color) const;

        vector<Piece> getWhiyteCaptured() const;
        vector<Piece> getBlackCaptured() const;

        void printBoard() const;
        void printBoardWithCoordinates() const;
        void printCapturedPieces() const;

        Board copy() const;

        int countPieces(Color color) const;
        int countPieceType(PieceType type, Color color) const;

        string toString() const;
};
#endif 