#ifndef MOVE_H
#define MOVE_H

#include "../utils/Types.h"
#include<string>

using namespace std;

class Move
{
    private:
        Position from;
        Position to;
        PieceType movedPiece;
        PieceType capturedPiece;

        bool castling;
        bool enPassant;
        bool promotion;
        PieceType promotionPiece;

        public:
        Move();
        Move(Position f, Position to);
        Move(Position f, Position to, PieceType moved, PieceType captured);

        Position getFrom() const;
        Position getTo() const;
        PieceType getMovedPiece() const;
        PieceType getCapturedPiece() const;
        PieceType getPromotionPiece() const;

        void setFrom( Position pos);
        void setTo( Position pos);
        void setMovedPiece( PieceType piece);
        void setCapturedPiece( PieceType piece);

        void setCastling(bool val);
        void setEnPassant(bool val);
        void setPromotion(PieceType piece);

        bool isCastlingMove() const;
        bool isEnPassantMove() const;
        bool isPromotionMove() const;
        bool isCapture() const;

        string toString() const;
        string toAlgebraicNotation() const;
        bool isValid() const;

};
#endif