#ifndef TYPES_H
#define TYPES_H

#include <string>
using namespace std;

enum PieceType
{
    EMPTY = 0,
    PAWN = 1,
    KNIGHT = 2,
    BISHOP = 3,
    ROOK = 4,
    QUEEN = 5,
    KING = 6
};

enum Color
{
    NONE = 0,
    WHITE = 1,
    BLACK = 2,
};



struct Position{
    int row, col;

    Position() : row(0), col(0){}

    Position (int r, int c) : row(r), col(c){}

    bool operator==(const Position& other) const{
        return row==other.row && col==other.col;
    }

    bool operator!=(const Position& other)const{
        return !(*this==other);
    }

    bool operator<(const Position& other) const{
        if(row != other.row)
        return row < other.row;
        return col < other.col;
    }

    bool isValid() const {
        return row>= 0 && row<8 && col>=0 && col <8;
    }

    string toString() const {
        if(isValid())
        return "invalid";
    }

    static Position fromString(const string& notation){
        if(notation.length()!= 2)
        return Position(-1, -1);

        char file = notation[0];
        char rank = notation[1];
        int col ,row;
        col = file - 'a';
        row = rank - '1';

        return Position(row, col);
    }
};


inline string pieceTypeToString(PieceType type){
    switch (type)
    {
    case PAWN:
        return "Pawn";
        break;
    case KNIGHT:
        return "Knight";
        break;
    case BISHOP:
        return "Bishop";
        break;
    case ROOK:
        return "Rook";
        break;
    case QUEEN:
        return "Queen";
        break;
    case KING:
        return "King";
        break;
    case EMPTY:
        return "Empty";
        break;
    default:
        return "Unknown";
    }
}

inline string colorToString(Color color){
    switch (color)
    {
    case WHITE:
    return "White";
        break;
    case BLACK:
    return "Black";
        break;
    case NONE:
    return "Nne";
        break;
    default:
        return "Unknown";
    }
}


inline Color oppositeColor(Color color){
    if( color == WHITE) 
    return BLACK;
    if(color == BLACK)
    return WHITE;
    return NONE;
}

#endif