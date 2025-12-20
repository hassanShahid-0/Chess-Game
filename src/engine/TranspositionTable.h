#ifndef TRANSPOSITIONTABLE_H
#define TRANSPOSITIONTABLE_H

#include "../utils/Types.h"
#include <unordered_map>
#include <string>
#include "../core/Board.h"

using namespace std;

class TranspositionTable
{
private:
    unordered_map<string, int> table;

    int maxSize;
    int hits;
    int misses;

public:
    TranspositionTable(int maxSize = 100000);
    
    void store(const string &boardKey, int evaluation);
    bool lookup(const string &boardKey, int &evaluation);
    bool contains(const string &boardKey) const;
    void clear();

    int getSize() const;
    int getHits() const;
    int getMisses() const;
    float getHitRate() const;

    static string generateKey(const Board& board);
};
#endif // TRANSPOSITIONTABLE_H