#include "WFC.h"
#include <iostream>
#include <chrono>

int main()
{
    float time = 0;
    int countMax = 1;

    for (int count = 0; count < countMax; count++) {
        // Start stopwatch
        auto start = std::chrono::high_resolution_clock::now();

        WFC* wfc = new WFC(100,100,count);

        // Stop stopwatch
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        time += duration.count();
    }

    std::cout << time << std::endl;

    std::cin;
    return 0;
}