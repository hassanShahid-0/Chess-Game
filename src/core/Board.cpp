#include "Board.h"
#include <iostream>
#include <iomanip>
using namespace std;

Board::Board() {
    clearBoard();
}

void Board::initializeBoard() {
    clearBoard();

    for(int col = 0; col < 8; ++col) {
        board[1][col] = Piece(PieceType::PAWN, Color::WHITE);
        board[6][col] = Piece(PieceType::PAWN, Color::BLACK);
    }

    board[0][0] = Piece(ROOK, WHITE);
    board[0][1] = Piece(KNIGHT, WHITE);
    board[0][2] = Piece(BISHOP, WHITE);
    board[0][3] = Piece(QUEEN, WHITE);
    board[0][4] = Piece(KING, WHITE);
    board[0][5] = Piece(BISHOP, WHITE);
    board[0][6] = Piece(KNIGHT, WHITE);
    board[0][7] = Piece(ROOK, WHITE);

    board[7][0] = Piece(ROOK, BLACK);
    board[7][1] = Piece(KNIGHT, BLACK);
    board[7][2] = Piece(BISHOP, BLACK);
    board[7][3] = Piece(QUEEN, BLACK);
    board[7][4] = Piece(KING, BLACK);
    board[7][5] = Piece(BISHOP, BLACK);
    board[7][6] = Piece(KNIGHT, BLACK);
    board[7][7] = Piece(ROOK, BLACK);
}

void Board::clearBoard() {
    for(int row = 0; row < 8; ++row) {
        for(int col = 0; col < 8; ++col) {
            board[row][col] = Piece();
        }
    }
    whiteCaptured.clear();
    blackCaptured.clear();
}

Piece Board::getPiece(Position pos) const {
    if(!isPositionValid(pos)) {
        return Piece();
    }
    return board[pos.row][pos.col];
}
Piece Board::getPiece(int row, int col) const {
    if(!isPositionValid(row, col)) {
        return Piece();
    }
    return board[row][col];
}

void Board::setPiece(Position pos, Piece piece) {
    if(isPositionValid(pos)) {
        board[pos.row][pos.col] = piece;
    }
}
void Board::setPiece(int row, int col, Piece piece) {
    if(isPositionValid(row, col)) {
        board[row][col] = piece;
    }
}

bool Board::isPositionValid(Position pos) const {
    return pos.isValid();
}
bool Board::isPositionValid(int row, int col) const {
    return row >= 0 && row < 8 && col >= 0 && col < 8;
}
bool Board::isSquareEmpty(Position pos) const {
    if(!isPositionValid(pos)) {
        return false;
    }
    return board[pos.row][pos.col].isEmpty();
}
bool Board::isSquareOccupiedByColor(Position pos, Color color) const {
    if(!isPositionValid(pos)) {
        return false;
    }
    Piece piece = board[pos.row][pos.col];
    return !piece.isEmpty() && piece.getColor() == color;
}

bool Board::movePiece(Position from, Position to) {
    if(!isPositionValid(from) || !isPositionValid(to)) {
        return false;
    }
    if(isSquareEmpty(from)) {
        return false;
    }
    Piece piece = getPiece(from);

    Piece capturedPiece = getPiece(to);
    if(!capturedPiece.isEmpty()) {
        addCapturedPiece(capturedPiece);
    }

    setPiece(to, piece);
    setPiece(from, Piece());

    board[to.row][to.col].setHasMoved(true);
    return true;
}

bool Board::executeMove(const Move& move) {
    if(!move.isValid()) {
        return false;
    }

    Position from = move.getFrom();
    Position to = move.getTo();

    if(move.isCastlingMove()) {
        movePiece(from, to);

        if(to.col> from.col) {
            Position rookFrom(from.row, 7);
            Position rookTo(from.row, 5);
            movePiece(rookFrom, rookTo);
        } else {
            Position rookFrom(from.row, 0);
            Position rookTo(from.row, 3);
            movePiece(rookFrom, rookTo);
        }
        return true;
    }

    if(move.isEnPassantMove()) {
        movePiece(from, to);

        Position capturedPawnPos(from.row, to.col);
        Piece capturedPawn = getPiece(capturedPawnPos);
        addCapturedPiece(capturedPawn);
        setPiece(capturedPawnPos, Piece());
        return true;
    }

    if(move.isPromotionMove()) {
        Piece piece = getPiece(from);

        Piece capturedPiece = getPiece(to);
        if(!capturedPiece.isEmpty()) {
            addCapturedPiece(capturedPiece);
        }

        Piece promotedPiece(move.getPromotionPiece(), piece.getColor());
        promotedPiece.setHasMoved(true);

        setPiece(to, promotedPiece);
        setPiece(from, Piece());
        return true;
    }
    return movePiece(from, to);
}

void Board::addCapturedPiece(Piece piece) {
    if(piece.isEmpty()) {
        return;
    }
    if(piece.getColor() == WHITE) {
        whiteCaptured.push_back(piece);
    } 
    else if(piece.getColor() == BLACK) {
        blackCaptured.push_back(piece);
    }
}

Position Board::findKing(Color color) const {
    for(int row = 0; row < 8; row++) {
        for(int col = 0; col < 8; col++) {
            Piece piece = board[row][col];
            if(piece.getType() == KING && piece.getColor() == color) {
                return Position(row, col);
            }
        }
    }
    return Position(-1, -1);
}

vector<Position> Board::findAllPieces(Color color) const {
    vector<Position> pieces;
    for(int row = 0; row < 8; row++) {
        for(int col = 0; col < 8; col++) {
            Piece piece = board[row][col];
            if(!piece.isEmpty() && piece.getColor() == color) {
                pieces.push_back(Position(row, col));
            }
        }
    }
    return pieces;
}

vector<Position> Board::findPieceTypes(PieceType type, Color color) const {
    vector<Position> pieces;
    for(int row = 0; row < 8; row++) {
        for(int col = 0; col < 8; col++) {
            Piece piece = board[row][col];
            if(piece.getType() == type && piece.getColor() == color) {
                pieces.push_back(Position(row, col));
            }
        }
    }
    return pieces;
}

vector<Piece> Board::getWhiyteCaptured() const {
    return whiteCaptured;
}
vector<Piece> Board::getBlackCaptured() const {
    return blackCaptured;
}

void Board::printBoard() const {
    cout<<"\n";
    for(int row = 7; row >= 0; row--) {
        cout<<row + 1<<" ";
        for(int col = 0; col < 8; col++) {
            Piece piece = board[row][col];
            cout<<piece.getSymbol()<<" ";
        }
        cout<<"\n";
    }
    cout<<"  a b c d e f g h\n\n";
}

void Board::printBoardWithCoordinates() const {
    cout<<"\n  +---+---+---+---+---+---+---+---+\n";
    for(int row = 7; row >= 0; row--) {
        cout<<row + 1<<" |";
        for(int col = 0; col < 8; col++) {
            cout<<" " << board[row][col].getSymbol() << " |";
        }
        cout<< " "<< row + 1 << "\n";
        cout<<"  +---+---+---+---+---+---+---+---+\n";
    }
    cout<<"    a   b   c   d   e   f   g   h  \n\n";
}

void Board::printCapturedPieces() const {
    cout<<"White Captured: ";
    for(const Piece& piece : whiteCaptured) {
        cout<<piece.getSymbol()<<" ";
    }
    cout<<"\n";
    cout<<"Black Captured: ";
    for(const Piece& piece : blackCaptured) {
        cout<<piece.getSymbol()<<" ";
    }
    cout<<"\n\n";
}

Board Board::copy() const {
    Board newBoard;
    for(int row = 0; row < 8; row++) {
        for(int col = 0; col < 8; col++) {
            newBoard.board[row][col] = board[row][col];
        }
    }

    newBoard.whiteCaptured = whiteCaptured;
    newBoard.blackCaptured = blackCaptured;
    return newBoard;
}

int Board::countPieces(Color color) const {
    int count = 0;
    for(int row = 0; row < 8; row++) {
        for(int col = 0; col < 8; col++) {
            Piece piece = board[row][col];
            if(!board[row][col].isEmpty() && board[row][col].getColor() == color) {
                count++;
            }
        }
    }
    return count;
}

int Board::countPieceType(PieceType type, Color color) const {
    int count = 0;
    for(int row = 0; row < 8; row++) {
        for(int col = 0; col < 8; col++) {
            Piece piece = board[row][col];
            if(piece.getType() == type && piece.getColor() == color) {
                count++;
            }
        }
    }
    return count;
}

string Board::toString() const {
    string result = "";
    for(int row = 7; row >= 0; row--) {
        for(int col = 0; col < 8; col++) {
            result += board[row][col].getSymbol();
        }
        if(row > 0) {
            result += "\n";
        }
    }
    return result;
}