#include "Region.h"

void Region::AddTile(Tile _tile)
{
    
}

void Region::PrintRegion()
{
    // Print out the created tiles
    for (const auto& tile1 : grid) 
    {
        for (const auto& tile : tile1)
        {
            std::cout << "Tile at (" << tile.x << ", " << tile.y << "), Entropy Keys: ";
            for (int key : tile.entropy) 
            {
                std::cout << key << " ";
            }
            std::cout << std::endl;
        }
    }
}
