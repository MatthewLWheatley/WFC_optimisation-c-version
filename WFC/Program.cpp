#include "WFC.h"
#include <chrono>
#include <windows.h>
#include <cstdlib>
#include <shlobj.h>
#include <iostream>
#include <limits>
#include <string>
#include <conio.h> // for _getch() on Windows
#include <omp.h>

std::string getAppDataPath() {
    char path[MAX_PATH];
    if (SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, path) != S_OK) {
        std::cerr << "Failed to get AppData path." << std::endl;
        return "";
    }
    return std::string(path);
}

void runPython(int seed,std::string filename)
{
    std::string appDataPath = getAppDataPath();
    if (appDataPath.empty()) {
        return;
    }

    // Construct the full path to the Python interpreter
    std::string pythonPath = appDataPath + "\\Programs\\Python\\Python312\\python.exe";

    // Construct the command to run the Python script
    std::string command = pythonPath + " image.py " + std::to_string(seed) + " input/"+filename+".png";

    int result = system(command.c_str());  // Use "python" or "python3" depending on your Python installation

    if (result == 0) {
        //std::cout << "Python script executed successfully." << std::endl;
    }
    else {
        std::cerr << "Error executing Python script. Return code: " << result << std::endl;
    }

    return;
}

int main() {

    std::string input;
    int offset = 0;
    while (true) {
        float time = 0;
        int countMax = 1 + offset;
        int x = 100; int y = 100;
        int w = 15; int h = 15;
        int stitchedSize = 4;
        int WFCTYPE = 2;
        bool nested = false;
        std::string filename = "Preset1";

        for (int count = 1 + offset; count < countMax + 1; count++) 
        {
            // Start stopwatch
            WFC* wfc;
            auto start = std::chrono::high_resolution_clock::now();

            if (WFCTYPE == 2) 
            {
                wfc = new WFC(x, y, w, h, stitchedSize,count, filename + ".txt");
            }
            else if (WFCTYPE == 1) {
                if (((x % w <= 5 && x % w != 0) || (y % h < 5 && y % h != 0))) std::cout << "this might take a while" << std::endl;
                wfc = new WFC(x, y, w, h, count, filename + ".txt",nested);
            }
            else {
                wfc = new WFC(x, y, count, filename + ".txt");
            }

            // Stop stopwatch
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
            time += duration.count();
            std::cout << time << std::endl;
            delete(wfc);
        }

        std::cout << time << std::endl;
        std::cout << time / countMax << std::endl;

        #pragma omp parallel for
        for (int count = 1 + offset; count < countMax+1; count++) {
            runPython(count, filename);
        }

        std::cout << "Press Enter to Continue or any other key to exit" << std::endl;

        // Wait for a single key press
        char c = _getch(); // _getch() is used instead of std::cin.get() for non-blocking single character input
        if (c != '\r' && c != '\n') {
            break;
        }
        std::cout << "Starting again.. " << std::endl;
        offset++;
    }

    return 0;
}