#ifndef MOVEHISTORY_H
#define MOVEHISTORY_H

#include "Move.h"
#include <vector>

class MoveHistory {
private:
    struct Node {
        Move data;
        Node* next;
        Node* prev;
        Node(Move m) : data(m), next(nullptr), prev(nullptr) {}
    };

    Node* head;
    Node* tail;
    Node* current;
    int size;

public:
    MoveHistory();
    ~MoveHistory();

    MoveHistory(const MoveHistory&) = delete;
    MoveHistory& operator=(const MoveHistory&) = delete;

    void addMove(Move move);

    Move getCurrentMove() const;
    Move getPreviousMove();
    Move getNextMove();
    bool hasPrevious() const;
    bool hasNext() const;
    void goToStart();
    void goToEnd();
    bool goToMove(int moveNumber);

    int getSize() const;
    bool isEmpty() const;
    int getCurrentPosition() const;

    std::vector<Move> getAllMoves() const;
    Move getMoveAt(int index) const;
    Move getLastMove() const;

    Move removeLastMove();
    void removeMovesAfterCurrent();

    void clear();

    void printHistory() const;
    void printHistoryWithPosition() const;
};

#endif
