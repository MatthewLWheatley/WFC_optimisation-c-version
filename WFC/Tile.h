#pragma once
#include <vector>
#include <random>
#include <map>
#include <iostream>
#include "Structures.h"  // Make sure this includes relevant structures and rules.
#include <algorithm>
#include <bitset>

class Region;

class Tile {
public:
    int x, y;
    bool collapsed = false;
    std::map<int, Rule>* entropyList = nullptr;
    std::map<int, Neighbours>* neighbourRules = nullptr;
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
    std::vector<int> filterRules(Tile* other, const std::vector<int>& thisEntropy, const std::function<const std::vector<int>& (const Neighbours&)>& getAllowedTiles);
    void Propagate();
    void Reset();

    Region *region;
private:

};

//
//class TileGrid
//{
//public:
//    std::unordered_map<int, std::unordered_map<int, Tile*>> Grid;
//
//    // Get tile at specific coordinates
//    Tile* get(int x, int y)
//    {
//        if (Grid.find(x) != Grid.end() && Grid[x].find(y) != Grid[x].end()) {
//            return Grid[x][y];
//        }
//        return nullptr; // or throw an exception
//    }
//
//    // Get tile using a pair of coordinates
//    Tile* get(std::pair<int, int> a)
//    {
//        return get(a.first, a.second);
//    }
//
//    // Insert a tile at specific coordinates
//    void insert(int x, int y, Tile* tile)
//    {
//        Grid[x][y] = tile;
//    }
//
//    // Insert a tile using a pair of coordinates
//    void insert(std::pair<int, int> a, Tile* tile)
//    {
//        insert(a.first, a.second, tile);
//    }
//
//    // Remove a tile at specific coordinates
//    void remove(int x, int y)
//    {
//        if (Grid.find(x) != Grid.end()) {
//            Grid[x].erase(y);
//            if (Grid[x].empty()) {
//                Grid.erase(x);
//            }
//        }
//    }
//
//    // Remove a tile using a pair of coordinates
//    void remove(std::pair<int, int> a)
//    {
//        remove(a.first, a.second);
//    }
//
//    // Check if a tile exists at specific coordinates
//    bool exists(int x, int y)
//    {
//        return Grid.find(x) != Grid.end() && Grid[x].find(y) != Grid[x].end();
//    }
//
//    // Check if a tile exists using a pair of coordinates
//    bool exists(std::pair<int, int> a)
//    {
//        return exists(a.first, a.second);
//    }
//
//    // Get the number of tiles in the grid
//    size_t size() const
//    {
//        size_t count = 0;
//        for (const auto& row : Grid) {
//            count += row.second.size();
//        }
//        return count;
//    }
//
//    // Check if the grid is empty
//    bool empty() const
//    {
//        return Grid.empty();
//    }
//
//    // Clear the entire grid
//    void clear()
//    {
//        Grid.clear();
//    }
//
//    // Operator overloading for ease of access
//    Tile* operator()(int x, int y)
//    {
//        return get(x, y);
//    }
//
//    Tile* operator()(std::pair<int, int> a)
//    {
//        return get(a);
//    }
//
//    // Iterator implementation
//    class Iterator {
//    public:
//        using InnerMapIterator = std::unordered_map<int, Tile*>::iterator;
//        using OuterMapIterator = std::unordered_map<int, std::unordered_map<int, Tile*>>::iterator;
//
//        Iterator(OuterMapIterator outer, OuterMapIterator outerEnd)
//            : outerIt(outer), outerEnd(outerEnd)
//        {
//            if (outerIt != outerEnd) {
//                innerIt = outerIt->second.begin();
//                advanceToValid();
//            }
//        }
//
//        bool operator!=(const Iterator& other) const
//        {
//            return outerIt != other.outerIt || (outerIt != outerEnd && innerIt != other.innerIt);
//        }
//
//        Tile* operator*() const
//        {
//            return innerIt->second;
//        }
//
//        const Iterator& operator++()
//        {
//            ++innerIt;
//            advanceToValid();
//            return *this;
//        }
//
//    private:
//        OuterMapIterator outerIt;
//        OuterMapIterator outerEnd;
//        InnerMapIterator innerIt;
//
//        void advanceToValid()
//        {
//            while (outerIt != outerEnd && innerIt == outerIt->second.end()) {
//                ++outerIt;
//                if (outerIt != outerEnd) {
//                    innerIt = outerIt->second.begin();
//                }
//            }
//        }
//    };
//
//    Iterator begin()
//    {
//        return Iterator(Grid.begin(), Grid.end());
//    }
//
//    Iterator end()
//    {
//        return Iterator(Grid.end(), Grid.end());
//    }
//};