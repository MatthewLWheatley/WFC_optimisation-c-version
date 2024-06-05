#include "Region.h"

class WFC
{
public:
	WFC() {};
	WFC(int _gridHeight, int _gridWidth, int _seed, std::string inputFile = "Preset1.txt");
	WFC(int _gridHeight, int _gridWidth, int _regionHeight, int _regionWidth, int _seed, std::string inputFile = "Preset1.txt", bool nested = false);
	WFC(int _ySubSections, int _xSubSections, int _regionHeight, int _regionWidth, int variations);

	~WFC();

	void printNeighbours(const std::map<int, Neighbours>& neighbourRules);

	void InitRules(std::string inputFile);
	std::map<int, Neighbours> findNeighbours(const std::map<int, Rule>& entropyList);
	void InitGrid();
	
	void CollapseTile();
	void Propergate(std::pair<int, int> orgin);

	std::vector<std::pair<int, int>> GetLowestEntropyList();
	std::vector<std::pair<int, int>> GetLowestRegionEntropyList();
	std::string ReverseString(const std::string& str);

	std::vector<int> parseIntegerList(const std::string& str);
	std::vector<Rule> readCSV(const std::string& filename);

	void writeToJson(const std::vector<Tile*> tiles, const std::string& filename);
private:
	int *seed = 0;
	std::unordered_map<std::pair<int, int>, Tile*,pair_hash> Grid;
	std::unordered_map<std::pair<int, int>, Tile*, pair_hash> useableGrid;
	std::unordered_map<std::pair<int, int>, Region*, pair_hash> regionGrid;
	std::map<int, Rule> entropyList;
	std::map<int, Neighbours> neighbourRules;
	std::vector<int> entropyKeys;

	std::stack<std::pair<int, int>> tileStack;
	int failCount = 0;
	int lastHighest = 0;

	int height,width;
	int regionHeight, regionWidth;

	std::pair<int, int> currentRegion = {INT_MAX,INT_MAX };

	int WFCTYPE = 0;
};