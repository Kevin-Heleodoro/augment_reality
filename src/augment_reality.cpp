// Author: Kevin Heleodoro
// Date: March 1, 2024
// Purpose: The main entrypoint for the Augmented Reality application

#include <iostream>
#include <opencv2/aruco.hpp>
#include <opencv2/opencv.hpp>

#include "../include/aruco_utils.h"
#include "../include/camera_utils.h"
#include "../include/chessboard_utils.h"

using namespace std;

/**
 * @brief Prints the usage message
 */
void printUsage()
{
    cout << "Usage: ./augment_reality.exe [options] (arguments)\n"
         << "Options:\n"
         << "  -a --aruco\t\tCreate new Aruco board \n"
         << "  -v --video\t\tInitiate video stream  \n"
         << "  -c --chessboard\tDetect and calibrate using chessboard\n"
         << "  -hc --harriscorner\tDetect Harris Corners\n"
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
        // Aruco command is passed
        if (strcmp(argv[1], "-a") == 0 || strcmp(argv[1], "--aruco") == 0)
        {
            createArucoBoard();
        }

        else if (strcmp(argv[1], "-c") == 0 || strcmp(argv[1], "--chessboard") == 0)
        {
            if (argv[2] != NULL)
            {
                std::string calibrationFileName = argv[2];
                return chessboardDetectionAndCalibration(calibrationFileName);
            }
            else
            {
                return chessboardDetectionAndCalibration("");
            }
        }

        // Video command is passed
        else if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--video") == 0)
        {
            if (argc == 3)
            {
                std::string calibrationFileName = argv[2];
                return videoStreaming(calibrationFileName);
            }
            else
            {
                return videoStreaming("");
            }
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