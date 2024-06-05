#pragma once
#include "Tile.h"


class Region
{
public:
    Region() : width(0), height(0) {}

    // Constructor with parameters
    Region(std::map<std::pair<int,int>,Tile*> _grid, int _width, int _height, int _startX, int _startY)
        : Grid(std::move(_grid)), width(_width), height(_height), startX(_startX), startY(_startY) {}

    std::map<std::pair<int, int>, Tile*> Grid;

    int width;
    int height;
    int startX;
    int startY;

    bool completed = false;

    int entropy = 0;

    int GetEntropy() 
    {
        int count = 0;

        for (auto tile : Grid) 
        {
            count += tile.second->entropy.size();
        }
        entropy = count;
        return count;
    }
};