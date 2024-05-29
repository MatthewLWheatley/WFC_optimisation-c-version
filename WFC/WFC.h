#include <vector>
#include <stack>
#include <utility>
#include <tuple>
#include <map>
#include <iostream>
#include <random>
#include <algorithm>
#include "nlohman/json.hpp"
#include <fstream>
#include <chrono>
#include "Tile.h"
#include <unordered_set>
#include <set>
#include <queue>
#include <unordered_map>

class WFC
{
public:
	WFC(int _gridHeight, int _gridWidth, int _seed);
	WFC(int _gridHeight, int _gridWidth, int _regionHeight, int _regionWidth);
	WFC(int _ySubSections, int _xSubSections, int _regionHeight, int _regionWidth, int variations);

	~WFC();

	void InitRules();
	void InitGrid();
	
	void CollapseTile();
	void Propergate(std::pair<int, int> orgin);

	std::vector<std::pair<int, int>> GetLowestEntropyList();
	std::vector<std::pair<int, int>> GetLowEntropyList();
	std::string ReverseString(const std::string& str);

	void writeToJson(const std::vector<Tile>& tiles, const std::string& filename);
private:
	int* seed = 0;
	std::unordered_map<std::pair<int, int>, Tile, pair_hash> fullGridTile;
	std::map<int, Rule> entropyList;
	std::vector<int> entropyKeys;

	std::stack<std::pair<int, int>> tileStack;
	int failCount = 0;
	int lastHighest = 0;

	int height,width;
	int regionHeight, regionWidth;

	int WFCTYPE = 0;

	Tile surroundingTile;

};
