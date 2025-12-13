#include "GameRules.h"
#include <cmath>

using namespace std;

// ============================================================================
// MOVE VALIDATION FOR EACH PIECE TYPE
// ============================================================================

bool GameRules::isValidPawnMove(const Board &board, Position from, Position to, Color color, const Move &lastMove)
{
    int direction = (color == WHITE) ? 1 : -1;
    int startRow = (color == WHITE) ? 1 : 6;

    int rowDiff = to.row - from.row;
    int colDiff = abs(to.col - from.col);

    if (colDiff == 0 && rowDiff == direction)
    {
        return board.isSquareEmpty(to);
    }

    if (colDiff == 0 && rowDiff == 2 * direction && from.row == startRow)
    {
        Position middle(from.row + direction, from.col);
        return board.isSquareEmpty(middle) && board.isSquareEmpty(to);
    }

    if (colDiff == 1 && rowDiff == direction)
    {
        if (!board.isSquareEmpty(to) &&
            board.getPiece(to).getColor() != color)
        {
            return true;
        }

        return canEnPassant(board, from, to, lastMove);
    }

    return false;
}

bool GameRules::isValidKnightMove(const Board &board, Position from, Position to)
{
    int rowDiff = abs(to.row - from.row);
    int colDiff = abs(to.col - from.col);

    bool isLShape = (rowDiff == 2 && colDiff == 1) || (rowDiff == 1 && colDiff == 2);

    if (!isLShape)
        return false;

    Piece destPiece = board.getPiece(to);
    if (destPiece.isEmpty())
        return true;

    Piece fromPiece = board.getPiece(from);
    return destPiece.getColor() != fromPiece.getColor();
}

bool GameRules::isValidBishopMove(const Board &board, Position from, Position to)
{
    int rowDiff = abs(to.row - from.row);
    int colDiff = abs(to.col - from.col);

    if (rowDiff != colDiff)
        return false;

    if (!isPathClear(board, from, to))
        return false;

    Piece destPiece = board.getPiece(to);
    if (destPiece.isEmpty())
        return true;

    Piece fromPiece = board.getPiece(from);
    return destPiece.getColor() != fromPiece.getColor();
}

bool GameRules::isValidRookMove(const Board &board, Position from, Position to)
{
    if (from.row != to.row && from.col != to.col)
        return false;

    if (!isPathClear(board, from, to))
        return false;

    Piece destPiece = board.getPiece(to);
    if (destPiece.isEmpty())
        return true;

    Piece fromPiece = board.getPiece(from);
    return destPiece.getColor() != fromPiece.getColor();
}

bool GameRules::isValidQueenMove(const Board &board, Position from, Position to)
{
    return isValidRookMove(board, from, to) || isValidBishopMove(board, from, to);
}

bool GameRules::isValidKingMove(const Board &board, Position from, Position to)
{
    int rowDiff = abs(to.row - from.row);
    int colDiff = abs(to.col - from.col);

    if (rowDiff <= 1 && colDiff <= 1 && (rowDiff != 0 || colDiff != 0))
    {
        Piece destPiece = board.getPiece(to);
        if (destPiece.isEmpty())
            return true;

        Piece fromPiece = board.getPiece(from);
        return destPiece.getColor() != fromPiece.getColor();
    }

    if (rowDiff == 0 && colDiff == 2)
    {
        Piece king = board.getPiece(from);
        if (king.getHasMoved())
            return false;

        if (to.col > from.col)
        {
            Position rookPos(from.row, 7);
            return canCastle(board, from, rookPos);
        }
        else
        {
            Position rookPos(from.row, 0);
            return canCastle(board, from, rookPos);
        }
    }

    return false;
}

// ============================================================================
// PATH CHECKING
// ============================================================================

bool GameRules::isPathClear(const Board &board, Position from, Position to)
{
    int rowDiff = to.row - from.row;
    int colDiff = to.col - from.col;

    int rowStep = (rowDiff == 0) ? 0 : (rowDiff > 0 ? 1 : -1);
    int colStep = (colDiff == 0) ? 0 : (colDiff > 0 ? 1 : -1);

    Position current(from.row + rowStep, from.col + colStep);

    while (current != to)
    {
        if (!board.isSquareEmpty(current))
        {
            return false;
        }
        current.row += rowStep;
        current.col += colStep;
    }

    return true;
}

// ============================================================================
// SPECIAL MOVES
// ============================================================================

bool GameRules::canCastle(const Board &board, Position kingPos, Position rookPos)
{
    Piece king = board.getPiece(kingPos);
    Piece rook = board.getPiece(rookPos);

    if (king.getHasMoved() || rook.getHasMoved())
        return false;

    if (!isPathClear(board, kingPos, rookPos))
        return false;

    if (isKingInCheck(board, king.getColor()))
        return false;

    int direction = (rookPos.col > kingPos.col) ? 1 : -1;
    Position square1(kingPos.row, kingPos.col + direction);
    Position square2(kingPos.row, kingPos.col + 2 * direction);

    Color opponentColor = oppositeColor(king.getColor());
    if (isSquareAttacked(board, square1, opponentColor))
        return false;
    if (isSquareAttacked(board, square2, opponentColor))
        return false;

    return true;
}

bool GameRules::canEnPassant(const Board &board, Position from, Position to, const Move &lastMove)
{
    if (!lastMove.isValid())
        return false;

    Piece movingPawn = board.getPiece(from);
    if (movingPawn.getType() != PAWN)
        return false;

    Piece lastMovedPiece = board.getPiece(lastMove.getTo());
    if (lastMovedPiece.getType() != PAWN)
        return false;

    int lastMoveDistance = abs(lastMove.getTo().row - lastMove.getFrom().row);
    if (lastMoveDistance != 2)
        return false;

    if (lastMove.getTo().row != from.row)
        return false;
    if (abs(lastMove.getTo().col - from.col) != 1)
        return false;

    int direction = (movingPawn.getColor() == WHITE) ? 1 : -1;
    Position expectedTo(from.row + direction, lastMove.getTo().col);

    return to == expectedTo;
}

bool GameRules::canPromote(const Board &board, Position pos)
{
    Piece piece = board.getPiece(pos);
    if (piece.getType() != PAWN)
        return false;

    if (piece.getColor() == WHITE && pos.row == 7)
        return true;
    if (piece.getColor() == BLACK && pos.row == 0)
        return true;

    return false;
}

// ============================================================================
// CHECK DETECTION (USES RECURSION)
// ============================================================================

bool GameRules::isKingInCheck(const Board &board, Color kingColor)
{
    Position kingPos = board.findKing(kingColor);
    if (!kingPos.isValid())
        return false;

    Color opponentColor = oppositeColor(kingColor);
    return isSquareAttacked(board, kingPos, opponentColor);
}

// BACKTRACKING: Try move, check if safe, undo move
bool GameRules::wouldBeInCheck(Board board, Move move, Color kingColor)
{
    board.executeMove(move);

    bool inCheck = isKingInCheck(board, kingColor);

    return inCheck;
}

bool GameRules::isSquareAttacked(const Board &board, Position pos, Color attackerColor)
{
    vector<Position> attackerPieces = board.findAllPieces(attackerColor);

    for (const Position &attackerPos : attackerPieces)
    {
        Piece attacker = board.getPiece(attackerPos);

        bool canAttack = false;
        Move dummyMove;

        switch (attacker.getType())
        {
        case PAWN:
        {
            int direction = (attackerColor == WHITE) ? 1 : -1;
            int rowDiff = pos.row - attackerPos.row;
            int colDiff = abs(pos.col - attackerPos.col);
            canAttack = (rowDiff == direction && colDiff == 1);
        }
        break;
        case KNIGHT:
            canAttack = isValidKnightMove(board, attackerPos, pos);
            break;
        case BISHOP:
            canAttack = isValidBishopMove(board, attackerPos, pos);
            break;
        case ROOK:
            canAttack = isValidRookMove(board, attackerPos, pos);
            break;
        case QUEEN:
            canAttack = isValidQueenMove(board, attackerPos, pos);
            break;
        case KING:
        {
            int rowDiff = abs(pos.row - attackerPos.row);
            int colDiff = abs(pos.col - attackerPos.col);
            canAttack = (rowDiff <= 1 && colDiff <= 1);
        }
        break;
        default:
            break;
        }

        if (canAttack)
            return true;
    }

    return false;
}

// ============================================================================
// GAME END CONDITIONS
// ============================================================================

bool GameRules::isCheckmate(Board &board, Color color)
{
    if (!isKingInCheck(board, color))
        return false;

    Move emptyMove;
    vector<Move> validMoves = getAllValidMoves(board, color, emptyMove);

    return validMoves.empty();
}

bool GameRules::isStalemate(Board &board, Color color)
{
    if (isKingInCheck(board, color))
        return false;

    Move emptyMove;
    vector<Move> validMoves = getAllValidMoves(board, color, emptyMove);

    return validMoves.empty();
}

bool GameRules::isDrawByInsufficientMaterial(const Board &board)
{
    int whiteCount = board.countPieces(WHITE);
    int blackCount = board.countPieces(BLACK);

    if (whiteCount == 1 && blackCount == 1)
        return true;

    if (whiteCount == 2 && blackCount == 1)
    {
        if (board.countPieceType(BISHOP, WHITE) == 1 ||
            board.countPieceType(KNIGHT, WHITE) == 1)
            return true;
    }
    if (blackCount == 2 && whiteCount == 1)
    {
        if (board.countPieceType(BISHOP, BLACK) == 1 ||
            board.countPieceType(KNIGHT, BLACK) == 1)
            return true;
    }

    return false;
}

// ============================================================================
// MOVE GENERATION
// ============================================================================

vector<Move> GameRules::getValidMovesForPiece(Board &board, Position pos, const Move &lastMove)
{
    vector<Move> validMoves;

    vector<Move> possibleMoves = getPossibleMovesForPiece(board, pos, lastMove);

    Piece piece = board.getPiece(pos);

    for (const Move &move : possibleMoves)
    {
        if (!wouldBeInCheck(board, move, piece.getColor()))
        {
            validMoves.push_back(move);
        }
    }

    return validMoves;
}

vector<Move> GameRules::getAllValidMoves(Board &board, Color color, const Move &lastMove)
{
    vector<Move> allMoves;

    vector<Position> pieces = board.findAllPieces(color);

    for (const Position &pos : pieces)
    {
        vector<Move> pieceMoves = getValidMovesForPiece(board, pos, lastMove);
        allMoves.insert(allMoves.end(), pieceMoves.begin(), pieceMoves.end());
    }

    return allMoves;
}

// ============================================================================
// UTILITY
// ============================================================================

bool GameRules::isBasicMoveLegal(const Board &board, Position from, Position to, const Move &lastMove)
{
    if (!board.isPositionValid(from) || !board.isPositionValid(to))
        return false;
    if (from == to)
        return false;

    Piece piece = board.getPiece(from);
    if (piece.isEmpty())
        return false;

    switch (piece.getType())
    {
    case PAWN:
        return isValidPawnMove(board, from, to, piece.getColor(), lastMove);
    case KNIGHT:
        return isValidKnightMove(board, from, to);
    case BISHOP:
        return isValidBishopMove(board, from, to);
    case ROOK:
        return isValidRookMove(board, from, to);
    case QUEEN:
        return isValidQueenMove(board, from, to);
    case KING:
        return isValidKingMove(board, from, to);
    default:
        return false;
    }
}

vector<Move> GameRules::getPossibleMovesForPiece(const Board &board, Position pos, const Move &lastMove)
{
    Piece piece = board.getPiece(pos);

    switch (piece.getType())
    {
    case PAWN:
        return generatePawnMoves(board, pos, lastMove);
    case KNIGHT:
        return generateKnightMoves(board, pos);
    case BISHOP:
        return generateBishopMoves(board, pos);
    case ROOK:
        return generateRookMoves(board, pos);
    case QUEEN:
        return generateQueenMoves(board, pos);
    case KING:
        return generateKingMoves(board, pos);
    default:
        return vector<Move>();
    }
}

// ============================================================================
// HELPER FUNCTIONS - MOVE GENERATION
// ============================================================================

vector<Move> GameRules::generatePawnMoves(const Board &board, Position pos, const Move &lastMove)
{
    vector<Move> moves;
    Piece pawn = board.getPiece(pos);
    Color color = pawn.getColor();
    int direction = (color == WHITE) ? 1 : -1;

    Position forward(pos.row + direction, pos.col);
    if (board.isPositionValid(forward) && board.isSquareEmpty(forward))
    {
        Move move(pos, forward, PAWN, EMPTY);
        if (canPromote(board, forward))
        {
            move.setPromotion(QUEEN);
        }
        moves.push_back(move);

        int startRow = (color == WHITE) ? 1 : 6;
        if (pos.row == startRow)
        {
            Position forward2(pos.row + 2 * direction, pos.col);
            if (board.isSquareEmpty(forward2))
            {
                moves.push_back(Move(pos, forward2, PAWN, EMPTY));
            }
        }
    }

    for (int colOffset = -1; colOffset <= 1; colOffset += 2)
    {
        Position diagonal(pos.row + direction, pos.col + colOffset);
        if (board.isPositionValid(diagonal))
        {
            Piece target = board.getPiece(diagonal);

            if (!target.isEmpty() && target.getColor() != color)
            {
                Move move(pos, diagonal, PAWN, target.getType());
                if (canPromote(board, diagonal))
                {
                    move.setPromotion(QUEEN);
                }
                moves.push_back(move);
            }

            if (canEnPassant(board, pos, diagonal, lastMove))
            {
                Move move(pos, diagonal, PAWN, PAWN);
                move.setEnPassant(true);
                moves.push_back(move);
            }
        }
    }

    return moves;
}

vector<Move> GameRules::generateKnightMoves(const Board &board, Position pos)
{
    vector<Move> moves;
    Piece knight = board.getPiece(pos);

    int rowOffsets[] = {-2, -2, -1, -1, 1, 1, 2, 2};
    int colOffsets[] = {-1, 1, -2, 2, -2, 2, -1, 1};

    for (int i = 0; i < 8; i++)
    {
        Position to(pos.row + rowOffsets[i], pos.col + colOffsets[i]);
        if (board.isPositionValid(to))
        {
            Piece target = board.getPiece(to);
            if (target.isEmpty() || target.getColor() != knight.getColor())
            {
                moves.push_back(Move(pos, to, KNIGHT, target.getType()));
            }
        }
    }

    return moves;
}

vector<Move> GameRules::generateBishopMoves(const Board &board, Position pos)
{
    vector<Move> moves;
    Piece bishop = board.getPiece(pos);

    int rowDirs[] = {-1, -1, 1, 1};
    int colDirs[] = {-1, 1, -1, 1};

    for (int dir = 0; dir < 4; dir++)
    {
        Position current = pos;
        while (true)
        {
            current.row += rowDirs[dir];
            current.col += colDirs[dir];

            if (!board.isPositionValid(current))
                break;

            Piece target = board.getPiece(current);
            if (target.isEmpty())
            {
                moves.push_back(Move(pos, current, BISHOP, EMPTY));
            }
            else
            {
                if (target.getColor() != bishop.getColor())
                {
                    moves.push_back(Move(pos, current, BISHOP, target.getType()));
                }
                break;
            }
        }
    }

    return moves;
}

vector<Move> GameRules::generateRookMoves(const Board &board, Position pos)
{
    vector<Move> moves;
    Piece rook = board.getPiece(pos);

    int rowDirs[] = {-1, 1, 0, 0};
    int colDirs[] = {0, 0, -1, 1};

    for (int dir = 0; dir < 4; dir++)
    {
        Position current = pos;
        while (true)
        {
            current.row += rowDirs[dir];
            current.col += colDirs[dir];

            if (!board.isPositionValid(current))
                break;

            Piece target = board.getPiece(current);
            if (target.isEmpty())
            {
                moves.push_back(Move(pos, current, ROOK, EMPTY));
            }
            else
            {
                if (target.getColor() != rook.getColor())
                {
                    moves.push_back(Move(pos, current, ROOK, target.getType()));
                }
                break;
            }
        }
    }

    return moves;
}

vector<Move> GameRules::generateQueenMoves(const Board &board, Position pos)
{
    vector<Move> moves;

    vector<Move> rookMoves = generateRookMoves(board, pos);
    vector<Move> bishopMoves = generateBishopMoves(board, pos);

    moves.insert(moves.end(), rookMoves.begin(), rookMoves.end());
    moves.insert(moves.end(), bishopMoves.begin(), bishopMoves.end());

    for (Move &move : moves)
    {
        move.setMovedPiece(QUEEN);
    }

    return moves;
}

vector<Move> GameRules::generateKingMoves(const Board &board, Position pos)
{
    vector<Move> moves;
    Piece king = board.getPiece(pos);

    int rowOffsets[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int colOffsets[] = {-1, 0, 1, -1, 1, -1, 0, 1};

    for (int i = 0; i < 8; i++)
    {
        Position to(pos.row + rowOffsets[i], pos.col + colOffsets[i]);
        if (board.isPositionValid(to))
        {
            Piece target = board.getPiece(to);
            if (target.isEmpty() || target.getColor() != king.getColor())
            {
                moves.push_back(Move(pos, to, KING, target.getType()));
            }
        }
    }

    if (!king.getHasMoved())
    {
        Position kingsideRook(pos.row, 7);
        if (canCastle(board, pos, kingsideRook))
        {
            Move castle(pos, Position(pos.row, 6), KING, EMPTY);
            castle.setCastling(true);
            moves.push_back(castle);
        }

        Position queensideRook(pos.row, 0);
        if (canCastle(board, pos, queensideRook))
        {
            Move castle(pos, Position(pos.row, 2), KING, EMPTY);
            castle.setCastling(true);
            moves.push_back(castle);
        }
    }

    return moves;
}