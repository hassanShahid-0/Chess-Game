#include "MoveHistory.h"
#include <iostream>
using namespace std;

MoveHistory::MoveHistory() {
    head = nullptr;
    tail = nullptr;
    current = nullptr;
    size = 0;
}

MoveHistory::~MoveHistory() {
    clear();
}

void MoveHistory::addMove(Move move) {
    Node* newNode = new Node(move);
    if (head == nullptr) {
        head = newNode;
        tail = newNode;
        current = newNode;
    } else {
        tail->next = newNode;
        newNode->prev = tail;
        tail = newNode;
        current = newNode;
    }
    size++;
}

Move MoveHistory::getCurrentMove() const {
    if (current == nullptr) {
        return Move();
    }
    return current->data;
}

Move MoveHistory::getPreviousMove() {
    if (!hasPrevious()) {
        return Move();
    }
    current = current->prev;
    return current->data;
}

Move MoveHistory::getNextMove() {
    if (!hasNext()) {
        return Move();
    }
    current = current->next;
    return current->data;
}

bool MoveHistory::hasPrevious() const {
    return current != nullptr && current->prev != nullptr;
}

bool MoveHistory::hasNext() const {
    return current != nullptr && current->next != nullptr;
}

void MoveHistory::goToStart() {
    current = head;
}

void MoveHistory::goToEnd() {
    current = tail;
}

bool MoveHistory::goToMove(int moveNumber) {
    if (moveNumber < 1 || moveNumber > size) {
        return false;
    }
    current = head;
    for (int i = 1; i < moveNumber; i++) {
        current = current->next;
    }
    return true;
}

int MoveHistory::getSize() const {
    return size;
}

bool MoveHistory::isEmpty() const {
    return size == 0;
}

int MoveHistory::getCurrentPosition() const {
    if (current == nullptr) return 0;
    int position = 1;
    Node* temp = head;
    while (temp != nullptr && temp != current) {
        temp = temp->next;
        position++;
    }
    return position;
}

vector<Move> MoveHistory::getAllMoves() const {
    vector<Move> moves;
    Node* temp = head;
    while (temp != nullptr) {
        moves.push_back(temp->data);
        temp = temp->next;
    }
    return moves;
}

Move MoveHistory::getMoveAt(int index) const {
    if (index < 0 || index >= size) {
        return Move();
    }
    Node* temp = head;
    for (int i = 0; i < index; i++) {
        temp = temp->next;
    }
    return temp->data;
}

Move MoveHistory::getLastMove() const {
    if (tail == nullptr) {
        return Move();
    }
    return tail->data;
}

Move MoveHistory::removeLastMove() {
    if (tail == nullptr) {
        return Move();
    }
    Move lastMove = tail->data;
    if (head == tail) {
        delete tail;
        head = nullptr;
        tail = nullptr;
        current = nullptr;
    } else {
        Node* temp = tail;
        tail = tail->prev;
        tail->next = nullptr;
        if (current == temp) {
            current = tail;
        }
        delete temp;
    }
    size--;
    return lastMove;
}

void MoveHistory::removeMovesAfterCurrent() {
    if (current == nullptr || current == tail) {
        return;
    }
    Node* temp = current->next;
    while (temp != nullptr) {
        Node* toDelete = temp;
        temp = temp->next;
        delete toDelete;
        size--;
    }
    tail = current;
    current->next = nullptr;
}

void MoveHistory::clear() {
    Node* temp = head;
    while (temp != nullptr) {
        Node* toDelete = temp;
        temp = temp->next;
        delete toDelete;
    }
    head = nullptr;
    tail = nullptr;
    current = nullptr;
    size = 0;
}

void MoveHistory::printHistory() const {
    if (isEmpty()) {
        cout << "No moves in history.\n";
        return;
    }
    Node* temp = head;
    int moveNum = 1;
    while (temp != nullptr) {
        cout << moveNum << ". " << temp->data.toString() << "\n";
        temp = temp->next;
        moveNum++;
    }
    cout << "\n";
}

void MoveHistory::printHistoryWithPosition() const {
    if (isEmpty()) {
        cout << "No moves in history.\n";
        return;
    }
    cout << "Current position: " << getCurrentPosition() << "\n";
    Node* temp = head;
    int moveNum = 1;
    while (temp != nullptr) {
        if (temp == current) cout << ">>> ";
        else cout << "    ";
        cout << moveNum << ". " << temp->data.toString() << "\n";
        temp = temp->next;
        moveNum++;
    }
    cout << "\n";
}
