#pragma once
#include <vector>
#include <random>
#include <map>
#include <iostream>
#include "Structures.h"  // Make sure this includes relevant structures and rules.
#include <algorithm>
#include <bitset>

class Tile {
public:
    int x, y;
    bool collapsed = false;
    std::map<int, Rule>* entropyList = nullptr;
    std::vector<int> entropy;
    std::vector<int> entropyBackup;
    Tile* up = nullptr;
    Tile* right = nullptr;
    Tile* down = nullptr;
    Tile* left = nullptr;

    Tile() = default;
    Tile(int _x, int _y, const std::vector<int>& keys, int* _seed)
        : x(_x), y(_y), entropy(keys), entropyBackup(keys), seed(_seed) {}

    ~Tile() = default;

    int* seed;

    void SetNeighbours(Tile* _up, Tile* _right, Tile* _down, Tile* _left);
    void CollapseTile();
    void Propagate();
    void Reset();


private:

};