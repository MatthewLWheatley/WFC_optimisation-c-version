#pragma once
#include <vector>
#include <random>
#include <map>
#include <iostream>
#include "Structures.h"

class Tile
{
public:
    int x, y;
    bool collapsed;
    std::map<int, Rule>* entropyList;
    std::vector<int> entropy;
    int variation;
    Tile();
    Tile(int _x, int _y, const std::vector<int>& keys) : x(_x), y(_y), entropy(keys), up(nullptr), right(nullptr), down(nullptr), left(nullptr) {}
    ~Tile();
    void SetNeighbours(Tile* _up, Tile* _right, Tile* _down, Tile* _left); // Added return type void
    Tile* up;
    Tile* right;
    Tile* down;
    Tile* left;

    void CollapseTile();
    void Propogate();
private:
};