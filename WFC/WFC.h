#include <vector>
#include <stack>
#include <utility>
#include <tuple>
#include <map>
#include <iostream>
#include "Region.h"
#include <random>
#include <algorithm>

class WFC
{
public:
	WFC(int _gridHeight, int _gridWidth);
	WFC(int _gridHeight, int _gridWidth, int _regionHeight, int _regionWidth);
	WFC(int _ySubSections, int _xSubSections, int _regionHeight, int _regionWidth, int variations);

	~WFC();

	void InitRules();
	void InitGrid();
	
	void CollapseTile();
	void Propergate();

	std::vector<std::pair<int, int>> GetLowestEntropyList();
	std::vector<std::pair<int, int>> GetLowEntropyList();
private:
	Region* currentRegion;
	std::vector<std::vector<Region>> fullGrid;
	std::map<std::pair<int, int>, Tile> fullGridTile;
	std::map<int, Rule> entropyList;
	std::vector<int> entropyKeys;

	int height,width;
	int regionHeight, regionWidth;

	int WFCTYPE = 0;

	Tile surroundingTile;

};
