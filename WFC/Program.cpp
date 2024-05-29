#include "WFC.h"
#include <iostream>
#include <chrono>
#include <Python.h>
#include <windows.h>
#include <cstdlib>
#include <shlobj.h>

std::string getAppDataPath() {
    char path[MAX_PATH];
    if (SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, path) != S_OK) {
        std::cerr << "Failed to get AppData path." << std::endl;
        return "";
    }
    return std::string(path);
}

void runPython(int seed)
{
    std::string appDataPath = getAppDataPath();
    if (appDataPath.empty()) {
        return;
    }

    // Construct the full path to the Python interpreter
    std::string pythonPath = appDataPath + "\\Programs\\Python\\Python312\\python.exe";

    // Construct the command to run the Python script
    std::string command = pythonPath + " image.py " + std::to_string(seed);

    int result = system(command.c_str());  // Use "python" or "python3" depending on your Python installation

    if (result == 0) {
        //std::cout << "Python script executed successfully." << std::endl;
    }
    else {
        std::cerr << "Error executing Python script. Return code: " << result << std::endl;
    }

    return;
}

int main()
{
    float time = 0;
    int countMax = 1;

    for (int count = 1; count < countMax+1; count++) {
        // Start stopwatch
        auto start = std::chrono::high_resolution_clock::now();

        WFC* wfc = new WFC(100,100,count);

        // Stop stopwatch
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        time += duration.count();
    }


    std::cout << time << std::endl;
    for (int count = 1; count < countMax+1; count++) {
        runPython(count);
    }

    std::string temp = "";
    std::cin >> temp;

    return 0;
}
