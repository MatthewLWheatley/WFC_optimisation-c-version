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
    std::shared_ptr<Tile> up = nullptr;
    std::shared_ptr<Tile> right = nullptr;
    std::shared_ptr<Tile> down = nullptr;
    std::shared_ptr<Tile> left = nullptr;
    
    Tile() = default;
    Tile(int _x, int _y, const std::vector<int>& keys, int* _seed) : x(_x), y(_y), entropy(keys),entropyBackup(keys), seed(_seed) {}
    ~Tile();  

    int* seed;

    void SetNeighbours(std::shared_ptr<Tile> _up, std::shared_ptr<Tile> _right, std::shared_ptr<Tile> _down, std::shared_ptr<Tile> _left);
    void CollapseTile();
    void Propagate();
    void Reset();


private:

};