#include "TranspositionTable.h"
#include "../core/Board.h"

using namespace std;

TranspositionTable::TranspositionTable(int maxSize) {
    this->maxSize = maxSize;
    hits = 0;
    misses = 0;
}

void TranspositionTable::store(const string &boardKey, int evaluation) {
    if (table.size() >= static_cast<size_t>(maxSize)) {
        clear();
    }

    table[boardKey] = evaluation;
}

bool TranspositionTable::lookup(const string &boardKey, int &evaluation) {
    auto it = table.find(boardKey);
    if (it != table.end()) {
        evaluation = it->second;
        hits++;
        return true;
    }

    misses++;
    return false;
}

bool TranspositionTable::contains(const string &boardKey) const {
    return table.find(boardKey) != table.end();
}

void TranspositionTable::clear() {
    table.clear();
    hits = 0;
    misses = 0;
}

int TranspositionTable::getSize() const {
    return table.size();
}

int TranspositionTable:: getHits() const {
    return hits;
}

int TranspositionTable::getMisses() const {
    return misses;
}

float TranspositionTable::getHitRate() const {
    int total = hits + misses;
    if (total == 0) return 0.0f; 

    return (float)hits / total * 100.0f; // "f" to indicate float literal
}

string TranspositionTable::generateKey(const Board& board) {
    return board.toString();
}
