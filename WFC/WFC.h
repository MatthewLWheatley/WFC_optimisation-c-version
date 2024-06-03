#include "Region.h"

class WFC
{
public:
	WFC() {};
	WFC(int _gridHeight, int _gridWidth, int _seed, std::string inputFile = "Preset1.txt");
	WFC(int _gridHeight, int _gridWidth, int _regionHeight, int _regionWidth, int _seed, std::string inputFile = "Preset1.txt");
	WFC(int _ySubSections, int _xSubSections, int _regionHeight, int _regionWidth, int variations);

	~WFC();

	void InitRules(std::string inputFile);
	void InitGrid();
	
	void CollapseTile();
	void Propergate(std::pair<int, int> orgin);

	std::vector<std::pair<int, int>> GetLowestEntropyList();
	std::string ReverseString(const std::string& str);

	std::vector<int> parseIntegerList(const std::string& str);
	std::vector<Rule> readCSV(const std::string& filename);

	void writeToJson(const std::vector<Tile*> tiles, const std::string& filename);
private:
	int* seed = 0;
	std::unordered_map<std::pair<int, int>, Tile*, pair_hash> Grid;
	std::unordered_map<std::pair<int, int>, Tile*, pair_hash> useableGrid;
	std::unordered_map<std::pair<int, int>, Region, pair_hash> regionGrid;
	std::map<int, Rule> entropyList;
	std::vector<int> entropyKeys;

	std::stack<std::pair<int, int>> tileStack;
	int failCount = 0;
	int lastHighest = 0;

	int height,width;
	int regionHeight, regionWidth;

	int WFCTYPE = 0;
};