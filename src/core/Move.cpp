#include "Move.h"

Move :: Move(){
    from = Position(-1, -1);
    to = Position(-1, -1);
    movedPiece = EMPTY;
    capturedPiece = EMPTY;
    castling = false;
    enPassant = false;
    promotion = false;
    promotionPiece = EMPTY;
}

Move::Move( Position f, Position t){
    from = f;
    to = t;
    movedPiece = EMPTY;
    capturedPiece = EMPTY;
    castling = false;
    enPassant = false;
    promotion = false;
    promotionPiece = EMPTY;
}

Move::Move( Position f, Position t, PieceType moved, PieceType captured){
    from = f;
    to = t;
    movedPiece = moved;
    capturedPiece = captured;
    castling = false;
    enPassant = false;
    promotion = false;
    promotionPiece = EMPTY;
}

Position Move::getFrom() const {
    return from;
}
Position Move::getTo() const {
    return to;
}
PieceType Move::getMovedPiece() const {
    return movedPiece;
}
PieceType Move::getCapturedPiece() const {
    return capturedPiece;
}
PieceType Move::getPromotionPiece() const {
    return promotionPiece;
}

void Move::setFrom( Position pos){
    from = pos;
}
void Move::setTo( Position pos){
    to = pos;
}
void Move::setMovedPiece( PieceType piece){
    movedPiece = piece;
}
void Move::setCapturedPiece( PieceType piece){
    capturedPiece = piece;
}
void Move::setCastling(bool val){
    castling = val;
}
void Move::setEnPassant(bool val){
    enPassant = val;
}
void Move::setPromotion(PieceType piece){
    promotion = true;
    promotionPiece = piece;
}

bool Move::isCastlingMove() const {
    return castling;
}
bool Move::isEnPassantMove() const {
    return enPassant;
}
bool Move::isPromotionMove() const {
    return promotion;
}
bool Move::isCapture() const {
    return capturedPiece != EMPTY || enPassant;
}

string Move::toString() const {
    string result = from.toString() + "-" + to.toString();
    
    // Add special move indicators
    if (isCastlingMove()) {
        result += " (Castle)";
    }
    if (isEnPassantMove()) {
        result += " (En Passant)";
    }
    if (isPromotionMove()) {
        result += "=" + pieceTypeToString(promotionPiece);
    }
    if (isCapture() && !isEnPassantMove()) {
        result += " (Capture)";
    }
    
    return result;
}

    string Move::toAlgebraicNotation() const {
    string result = "";
    
    // Special case: Castling
    if (isCastlingMove()) {
        // Kingside castle (king moves 2 squares right)
        if (to.col - from.col == 2) {
            return "O-O";
        }
        // Queenside castle (king moves 2 squares left)
        else if (from.col - to.col == 2) {
            return "O-O-O";
        }
    }
    
    // Add piece symbol (except for pawns)
    if (movedPiece != PAWN && movedPiece != EMPTY) {
        switch (movedPiece) {
            case KNIGHT: result += "N"; break;
            case BISHOP: result += "B"; break;
            case ROOK:   result += "R"; break;
            case QUEEN:  result += "Q"; break;
            case KING:   result += "K"; break;
            default: break;
        }
    }
    
    // Add capture indicator
    if (isCapture()) {
        // For pawn captures, include the starting file
        if (movedPiece == PAWN) {
            result += from.toString()[0];  // Starting file (a-h)
        }
        result += "x";
    }
    
    // Add destination square
    result += to.toString();
    
    // Add promotion
    if (isPromotionMove()) {
        result += "=";
        switch (promotionPiece) {
            case KNIGHT: result += "N"; break;
            case BISHOP: result += "B"; break;
            case ROOK:   result += "R"; break;
            case QUEEN:  result += "Q"; break;
            default: break;
        }
    }
    
    // Add en passant indicator
    if (isEnPassantMove()) {
        result += " e.p.";
    }
    
    return result;
}
bool Move::isValid() const {
    return from.isValid() && to.isValid() && movedPiece != EMPTY;
}
