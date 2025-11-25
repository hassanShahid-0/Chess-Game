#include "Piece.h"
using namespace std;

Piece::Piece()
{
    type = EMPTY;
    color = NONE;
    hasMoved = false;
}

Piece::Piece(PieceType t, Color c)
{
    type = t;
    color = c;
    hasMoved = false;
}

PieceType Piece::getType() const
{
    return type;
}
Color Piece::getColor() const
{
    return color;
}
bool Piece::getHasMoved() const
{
    return hasMoved;
}
bool Piece::isEmpty() const
{
    return type == EMPTY;
}

void Piece::setType(PieceType t)
{
    type = t;
}
void Piece::setColor(Color c)
{
    color = c;
}
void Piece::setHasMoved(bool moved)
{
    hasMoved = moved;
}

char Piece::getSymbol() const
{
    if (isEmpty())
    {
        return ' ';
    }

    char symbol;
    switch (type)
    {
    case PAWN:
        symbol = 'P';
        break;
    case KNIGHT:
        symbol = 'N';
        break;
    case BISHOP:
        symbol = 'B';
        break;
    case ROOK:
        symbol = 'R';
        break;
    case QUEEN:
        symbol = 'Q';
        break;
    case KING:
        symbol = 'K';
        break;
    default:
        symbol = '?';
    }

    if (color == BLACK)
    {
        symbol = tolower(symbol);
    }
    return symbol;
}

string Piece::getUnicodeSymbol() const
{
    if (isEmpty())
    {
        return " ";
    }

    if (color == WHITE)
    {
        switch (type)
        {
        case PAWN:
            return "♙";
        case KNIGHT:
            return "♘";
        case BISHOP:
            return "♗";
        case ROOK:
            return "♖";
        case QUEEN:
            return "♕";
        case KING:
            return "♔";
        default:
            return "?";
        }
    }
    else
    {
        switch (type)
        {
        case PAWN:
            return "♟";
        case KNIGHT:
            return "♞";
        case BISHOP:
            return "♝";
        case ROOK:
            return "♜";
        case QUEEN:
            return "♛";
        case KING:
            return "♚";
        default:
            return "?";
        }
    }
}

int Piece::getValue() const
{
    switch (type)
    {
    case PAWN:
        return 1;
    case KNIGHT:
        return 3;
    case BISHOP:
        return 3;
    case ROOK:
        return 5;
    case QUEEN:
        return 9;
    case KING:
        return 0; // King's value is often considered infinite, but for practical purposes we return 0 here.
    default:
        return 0;
    }
}