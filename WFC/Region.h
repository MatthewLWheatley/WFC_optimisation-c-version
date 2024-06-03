#pragma once
#include "Tile.h"


class Region
{
public:
    Region() : width(0), height(0) {}

    // Constructor with parameters
    Region(std::unordered_map<std::pair<int,int>,Tile*,pair_hash> _grid, int _width, int _height, int _startX, int _startY)
        : Grid(std::move(_grid)), width(_width), height(_height), startX(_startX), startY(_startY) {}

    std::unordered_map<std::pair<int, int>, Tile*, pair_hash> Grid;

    int width;
    int height;
    int startX;
    int startY;
};