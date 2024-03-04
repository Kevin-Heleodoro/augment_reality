// Author: Kevin Heleodoro
// Date: March 1, 2024
// Purpose: The main entrypoint for the Augmented Reality application

#include <iostream>
#include <opencv2/aruco.hpp>
#include <opencv2/opencv.hpp>

#include "../include/aruco_utils.h"

using namespace std;

/**
 * @brief Prints the usage message
 */
void printUsage()
{
    cout << "Usage: ./augment_reality.exe [options]\n"
         << "Options:\n"
         << "  -c --calibration\tRun the calibration process\n"
         << "  -a --aruco\t\tCreate new Aruco marker. (optional: pass in marker id. default is 23) \n"
         << "  -v --video\t\tInitiate video stream  \n"
         << "  -h or --help\t\tShow this help message\n"
         << endl;
}

int main(int argc, char *argv[])
{
    cout << "Hello, Augmented Reality!\n" << endl;
    // Print usage message if no arguments are passed
    if (argc <= 1)
    {
        printUsage();
        return -1;
    }

    // Check for command line arguments
    if (argc >= 2)
    {
        // Calibration command is passed
        if (strcmp(argv[1], "-c") == 0 || strcmp(argv[1], "--calibration") == 0)
        {
            cout << "Running calibration process..." << endl;
        }

        // Aruco command is passed
        else if (strcmp(argv[1], "-a") == 0 || strcmp(argv[1], "--aruco") == 0)
        {
            // Create new ArUco marker
            if (argc == 3)
            {
                createArucoMarker(atoi(argv[2]));
            }
            else
            {
                createArucoMarker();
            }
        }

        // Video command is passed
        else if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--video") == 0)
        {
            return videoStreaming();
        }

        // Help command is passed
        else if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
        {
            printUsage();
        }

        // Invalid option is passed
        else
        {
            cout << "Invalid option: " << argv[1] << endl;
            printUsage();
            return -1;
        }
    }
    // Invalid number of arguments
    else
    {
        cout << "Invalid number of arguments" << endl;
        printUsage();
        return -1;
    }

    return 0;
}