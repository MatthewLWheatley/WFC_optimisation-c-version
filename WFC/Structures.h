#pragma once
#include "string"
#include "list"
#include <fstream>
#include <chrono>
#include <unordered_set>
#include <set>
#include <queue>
#include <vector>
#include <stack>
#include <utility>
#include <tuple>
#include <map>
#include <iostream>
#include <random>
#include <algorithm>
#include <unordered_map>
#include "nlohman/json.hpp"

struct Rule
{
    std::string up;
    std::string right;
    std::string down;
    std::string left;
    std::vector<int> spritePosition = { 1 };
    int weight = 1;
};

struct Neighbours
{
    std::vector<int> up;
    std::vector<int> right;
    std::vector<int> down;
    std::vector<int> left;
};

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2>& pair) const {
        auto hash1 = std::hash<T1>{}(pair.first);
        auto hash2 = std::hash<T2>{}(pair.second);
        return hash1 ^ hash2;
    }
};

