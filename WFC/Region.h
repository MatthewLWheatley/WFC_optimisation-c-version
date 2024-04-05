#pragma once
#include "vector"
#include "stack"
#include "Tile.h"
#include <iostream>

class Region
{
public:
	void GetTile();
	void GetTotalEntropy();
	void SetGrid();
	void AddTile(Tile _tile);
	void PrintRegion();
private:
	int x, y;
	int width, height;
	std::vector<std::vector<Tile>> grid;
	std::stack<std::pair<int, int>> backstack;

	std::vector<std::vector<Tile>> connecting;
};

