// Author: Kevin Heleodoro
// Date: March 1, 2024
// Purpose: A collection of utils used for Aruco marker recognition and calibration

#include <opencv2/aruco.hpp>
#include <opencv2/opencv.hpp>

#include "aruco_utils.h"
#include "camera_utils.h"

//--------------------- Global Variables ---------------------//
std::string defaultCalibrationDirectory = "../img/CameraCalibration/";
cv::aruco::DetectorParameters detectorParams;
cv::aruco::Dictionary dict;
cv::Mat cameraMatrix, distCoeffs, frame, frameCopy;
cv::Size imageSize;
std::vector<int> markerIds;
std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;

// Future Goal: Create a class or struct to store the following variables
std::vector<cv::Vec3f> point_set;                  // should equal markerCorners // object points
std::vector<std::vector<cv::Vec3f>> point_list;    // should be the same size as corner_list
std::vector<std::vector<cv::Point2f>> corner_list; // image points
std::vector<cv::Mat> calibrationImages;
int numOfCalibrationImages;
float squareSize, markerSize, aspectRatio;
int markersX = 5;
int markersY = 7;
int margins = 10;
int borderBits = 1;
int markerLength = 100;
int markerSeparation = 10;
cv::Size boardSize = cv::Size(560, 780);
bool isCalibrated = false;
bool areVariablesInitialized = false;

//------------------------------------------------------------//

/**
 * @brief Get the current date and time as a formatted string.
 *
 * This function retrieves the current date and time from the system clock and formats it as a string
 * in the format "YYYY-MM-DD_HH-MM-SS". The formatted string is returned.
 *
 * @return std::string The current date and time as a formatted string.
 */
std::string getCurrentDateTimeStamp()
{
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&currentTime), "%Y-%m-%d_%H-%M-%S");

    return ss.str();
}

/**
 * @brief Creates a new Aruco marker and saves it to a file
 *
 * @param markerId The id of the marker to be created
 * @return void
 */
void createArucoMarker(int markerId)
{
    std::cout << "Creating new Aruco marker..." << std::endl;
    cv::Mat markerImage;
    cv::aruco::Dictionary arucoDict = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    int markerSize = 200;
    int borderBits = 1;
    cv::aruco::generateImageMarker(arucoDict, markerId, markerSize, markerImage, borderBits);
    std::string filename = "aruco_marker_" + std::to_string(markerId) + ".png";
    cv::imwrite(filename, markerImage);
}

void createArucoBoard()
{
    std::cout << "Creating new Aruco board..." << std::endl;
    dict = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    cv::Mat boardImage;
    cv::aruco::GridBoard board =
        cv::aruco::GridBoard(cv::Size(markersX, markersY), markerLength, markerSeparation, dict);
    std::string filename = "aruco_board_" + getCurrentDateTimeStamp() + ".png";
    board.generateImage(boardSize, boardImage, margins, borderBits);
    cv::imwrite(filename, boardImage);
    std::cout << "Aruco board created and saved to " << filename << std::endl;
}

/**
 * @brief Detects and draws Aruco markers in the video stream
 *
 * @param src The source image
 * @param estimatePose Flag to estimate the pose of the markers
 * @return void
 */
void detectAndDrawMarkers(cv::Mat &src, bool estimatePose = false)
{
    cv::aruco::ArucoDetector detector(dict, detectorParams);
    detector.detectMarkers(src, markerCorners, markerIds, rejectedCandidates);

    if (markerIds.size() > 0)
    {
        cv::aruco::drawDetectedMarkers(src, markerCorners, markerIds);
    }
}

void printCalibrationVariables()
{
    std::cout << "Printing point set: " << std::endl;
    for (size_t i = 0; i < point_set.size(); i++)
    {
        std::cout << "Point " << i << ": " << std::endl;
        std::cout << point_set[i] << std::endl;
    }

    std::cout << "Printing corner list: " << std::endl;
    for (size_t i = 0; i < corner_list.size(); i++)
    {
        std::cout << "Corner " << i << ": " << std::endl;
        std::cout << corner_list[i] << std::endl;
    }
}

void saveCalibrationVariables(double reprojectionError = 0.0)
{
    std::string filename = "calibration_variables_" + getCurrentDateTimeStamp() + ".xml";
    cv::FileStorage fs(filename, cv::FileStorage::WRITE);
    fs << "frame_width" << frame.cols;
    fs << "frame_height" << frame.rows;
    fs << "aspect_ratio" << aspectRatio;
    fs << "camera_matrix" << cameraMatrix;
    fs << "dist_coeffs" << distCoeffs;
    fs << "reprojection_error" << reprojectionError;
    fs << "point_set" << point_set;
    fs << "corner_list" << corner_list;
    fs.release();
}

/**
 * @brief Saves the calibration image to a file
 *
 * @param src The source image
 * @param calibrationDirectory The directory to save the calibration images
 * @return void
 */
void saveCalibrationImage(cv::Mat &src, std::string calibrationDirectory = defaultCalibrationDirectory)
{
    // TODO: Add some error handling for the directory and validation for the image

    std::cout << "Marker Corners vs point_set: " << std::endl;
    std::cout << "Marker Corners: " << markerCorners.size() << std::endl;
    std::cout << "Point Set: " << point_set.size() << std::endl;

    std::vector<cv::Point2f> corners;

    for (size_t i = 0; i < markerCorners.size(); i++)
    {
        for (size_t j = 0; j < markerCorners[i].size(); j++)
        {
            corners.push_back(markerCorners[i][j]);
            std::cout << "i: " << i << "j: " << j << std::endl;
            std::cout << "Marker Corner: " << markerCorners[i][j] << std::endl;
        }
    }

    // std::cout << "First corner: " << corners[0] << std::endl;
    // std::cout << "First point set: " << point_set[0] << std::endl;
    // printCalibrationVariables();

    if (corners.size() != point_set.size())
    {
        std::cerr << "\n===========\nError: The number of corners and points do not match\n===========\n" << std::endl;
        return;
    }

    corner_list.push_back(corners);
    point_list.push_back(point_set);

    std::string filename = calibrationDirectory + std::to_string(numOfCalibrationImages) + "_calibration_image.png";
    cv::imwrite(filename, src);
    numOfCalibrationImages++;
    std::cout << "Calibration image saved" << std::endl;
    calibrationImages.push_back(src);

    std::cout << "Number of calibration images: " << numOfCalibrationImages << std::endl;
    std::cout << "Number of corners: " << corner_list.size() << std::endl;
    std::cout << "Number of points: " << point_list.size() << std::endl;
    std::cout << "\n" << std::endl;

    printCalibrationVariables();
}

/**
 * @brief Initializes the variables used for camera calibration
 *
 * @return void
 */
void initializeVariables()
{
    std::cout << "Initializing variables..." << std::endl;
    numOfCalibrationImages = 0;
    dict = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    detectorParams = cv::aruco::DetectorParameters();
    aspectRatio = 1;
    double focalLength = frame.cols;
    std::cout << "Focal Length: " << focalLength << std::endl;
    std::cout << "Frame width: " << frame.cols << std::endl;
    std::cout << "Frame height: " << frame.rows << std::endl;
    std::cout << "Aspect Ratio: " << aspectRatio << std::endl;

    distCoeffs = cv::Mat::zeros(5, 1, CV_64F);
    // distCoeffs = cv::Mat::zeros(8, 1, CV_64F);

    /**
     * @brief The camera matrix is a 3x3 matrix that represents the intrinsic parameters of the camera. Initialized to
     * [1, 0, frame.cols/2]
     * [0, 1, frame.rows/2]
     * [0, 0, 1           ]
     */
    cameraMatrix = cv::Mat::eye(3, 3, CV_64FC1);
    cameraMatrix.at<double>(0, 0) = focalLength * aspectRatio;
    // cameraMatrix.at<double>(0, 0) = 1;
    cameraMatrix.at<double>(0, 1) = 0;
    cameraMatrix.at<double>(0, 2) = frame.cols / 2.0;
    cameraMatrix.at<double>(1, 0) = 0;
    cameraMatrix.at<double>(1, 1) = focalLength;
    // cameraMatrix.at<double>(1, 1) = 1;
    cameraMatrix.at<double>(1, 2) = frame.rows / 2.0;
    cameraMatrix.at<double>(2, 0) = 0;
    cameraMatrix.at<double>(2, 1) = 0;
    cameraMatrix.at<double>(2, 2) = 1;

    /**
     * @brief Initializing the point set with the top left corner of the markers
     */
    float x, y;
    for (int i = 0; i < markersY; ++i)
    {
        for (int j = 0; j < markersX; ++j)
        {
            x = j * (markerLength + markerSeparation);
            y = i * (markerLength + markerSeparation);
            point_set.push_back(cv::Vec3f(x, y, 0));
            point_set.push_back(cv::Vec3f(x + markerLength, y, 0));
            point_set.push_back(cv::Vec3f(x + markerLength, y + markerLength, 0));
            point_set.push_back(cv::Vec3f(x, y + markerLength, 0));
        }
    }

    // float xStart, yStart = 0.0f;
    // for (int y = 0; y < markersY; y++)
    // {
    //     for (int x = 0; x < markersX; x++)
    //     {
    //         float xPos = xStart + x * (markerLength + markerSeparation);
    //         float yPos = yStart + y * (markerLength + markerSeparation);

    //         point_set.push_back(cv::Vec3f(xPos, yPos, 0));
    //         point_set.push_back(cv::Vec3f(xPos + markerLength, yPos, 0));
    //         point_set.push_back(cv::Vec3f(xPos + markerLength, yPos + markerLength, 0));
    //         point_set.push_back(cv::Vec3f(xPos, yPos + markerLength, 0));
    //     }
    // }

    std::cout << "Point Set Size: " << point_set.size() << std::endl;

    areVariablesInitialized = true;
    std::cout << "Variables initialized! \n" << std::endl;
}

int videoStreaming(std::string calibrationDirectory)
{
    cv::VideoCapture cap(0);
    if (!cap.isOpened())
    {
        std::cerr << "Error opening video stream" << std::endl;
        return -1;
    }

    std::cout << "\nWelcome to the Augmented Reality Application\n" << std::endl;
    std::cout << "Press 'q' to quit the program" << std::endl;
    std::cout << "Press 's' to save a calibration image" << std::endl;
    std::cout << "Press 'c' to calibrate the camera" << std::endl;
    std::cout << "\n" << std::endl;

    calibrationDirectory = calibrationDirectory == "" ? defaultCalibrationDirectory : calibrationDirectory;
    cv::namedWindow("Video Stream", cv::WINDOW_AUTOSIZE);
    // initializeVariables();

    std::cout << "Initial Camera Matrix: " << cameraMatrix << std::endl;

    while (true)
    {
        // cv::Mat frame, frameCopy;
        cap >> frame;
        if (frame.empty())
        {
            std::cerr << "Error: Could not capture frame" << std::endl;
            break;
        }
        if (!areVariablesInitialized)
        {
            initializeVariables();
        }

        frame.copyTo(frameCopy);
        imageSize = frame.size();
        detectAndDrawMarkers(frameCopy);

        // display number of markers detected in window
        cv::putText(frameCopy, "Number of markers detected: " + std::to_string(markerIds.size()), cv::Point(10, 30),
                    cv::FONT_HERSHEY_SIMPLEX, .75, cv::Scalar(0, 0, 255), 2);

        cv::imshow("Video Stream", frameCopy);

        char key = cv::waitKey(10);
        if (key == 'q' || key == 'Q')
        {
            std::cout << "User terminated program" << std::endl;
            break;
        }
        if (key == 's' || key == 'S')
        {
            std::cout << "Saving frame" << std::endl;
            saveCalibrationImage(frameCopy, calibrationDirectory);
            // saveCalibrationImage(frame, calibrationDirectory);
            cv::waitKey(500);
        }
        if (key == 'c' || key == 'C')
        {
            std::cout << "Calibrating camera" << std::endl;
            if (numOfCalibrationImages >= 5)
            {
                // TODO: Make sure that this calibration call allows the video loop to continue
                double result = calibrateCamera(cameraMatrix, distCoeffs, boardSize, point_list, corner_list);
                // double result = calibrateCamera(cameraMatrix, distCoeffs, imageSize, point_list, corner_list);
                saveCalibrationVariables(result);

                if (result > 0)
                {
                    std::cout << "Camera calibration successful" << std::endl;
                    isCalibrated = true;
                }
                else
                {
                    std::cout << "Camera calibration failed" << std::endl;
                }
                // continue;
            }
            else
            {
                std::cout << "Need at least 5 calibration images" << std::endl;
            }
            continue;
        }
    }

    return 0;
}