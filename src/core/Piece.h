#ifndef PIECE_H
#define PIECE_H

#include "../utils/Types.h"
using namespace std;

class Piece
{
private:
    PieceType type;
    Color color;
    bool hasMoved;

public:
    Piece();
    Piece(PieceType t, Color c);

    PieceType getType() const;
    Color getColor() const;
    bool getHasMoved() const;
    bool isEmpty() const;

    void setType(PieceType t);
    void setColor(Color c);
    void setHasMoved(bool moved);

    char getSymbol() const;
    string getUnicodeSymbol() const;
    int getValue() const;
};

#endif