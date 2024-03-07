// Author: Kevin Heleodoro
// Date: March 1, 2024
// Purpose: A collection of utils used for Aruco marker recognition and calibration

#include <opencv2/aruco.hpp>
#include <opencv2/opencv.hpp>

#include "aruco_utils.h"
#include "camera_utils.h"

//--------------------- Global Variables ---------------------//
std::string defaultCalibrationDirectory = "../img/CameraCalibration";
cv::aruco::DetectorParameters detectorParams;
cv::Mat cameraMatrix, distCoeffs;
cv::Size imageSize;
std::vector<int> markerIds;
std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;

std::vector<cv::Vec3f> point_set;
std::vector<std::vector<cv::Vec3f>> point_list;
std::vector<std::vector<cv::Point2f>> corner_list;
std::vector<cv::Mat> calibrationImages;
int numOfCalibrationImages = 1;

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

/**
 * @brief Detects and draws Aruco markers in the video stream
 *
 * @param src The source image
 * @param estimatePose Flag to estimate the pose of the markers
 * @return void
 */
void detectAndDrawMarkers(cv::Mat &src, bool estimatePose = false)
{
    // std::vector<int> markerIds;
    // std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;
    // cv::aruco::DetectorParameters params = cv::aruco::DetectorParameters();
    cv::aruco::Dictionary dict = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    cv::aruco::ArucoDetector detector(dict, detectorParams);
    // cv::aruco::ArucoDetector detector(dict, params);
    detector.detectMarkers(src, markerCorners, markerIds, rejectedCandidates);

    if (markerIds.size() > 0)
    {
        // if (estimatePose)
        // {
        //     size_t numMarkers = markerCorners.size();
        //     std::vector<cv::Vec3d> rvecs(numMarkers), tvecs(numMarkers);
        //     for (size_t i = 0; i < numMarkers; i++) {
        //         cv::aruco::estimatePoseSingleMarkers(markerCorners[i], 0.05, cameraMatrix, distCoeffs, rvecs[i],
        //         tvecs[i]);
        //         // cv::aruco::drawAxis(src, cameraMatrix, distCoeffs, rvecs[i], tvecs[i], 0.1);
        //     }
        // }
        // std::cout << "Detected " << markerIds.size() << " markers" << std::endl;
        cv::aruco::drawDetectedMarkers(src, markerCorners, markerIds);
    }
}

/**
 * @brief Saves the calibration image to a file
 *
 * @param src The source image
 * @return void
 */
void saveCalibrationImage(cv::Mat &src)
{
    std::string filename =
        "../img/CameraCalibration/" + std::to_string(numOfCalibrationImages) + "_calibration_image.png";
    cv::imwrite(filename, src);
    numOfCalibrationImages++;
    std::cout << "Calibration image saved" << std::endl;
    calibrationImages.push_back(src);

    corner_list.push_back(markerCorners[0]);
    point_set.push_back(cv::Vec3f(0, 0, 0));
    point_set.push_back(cv::Vec3f(0, 1, 0));
    point_set.push_back(cv::Vec3f(1, 1, 0));
    point_set.push_back(cv::Vec3f(1, 0, 0));
    point_list.push_back(point_set);

    std::cout << "Number of calibration images: " << numOfCalibrationImages << std::endl;
    std::cout << "Number of corners: " << corner_list.size() << std::endl;
    std::cout << "Number of points: " << point_list.size() << std::endl;
    std::cout << "Number of point sets: " << point_set.size() << std::endl;
    std::cout << "\n" << std::endl;
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

    detectorParams = cv::aruco::DetectorParameters();

    while (true)
    {
        cv::Mat frame, frameCopy;
        cap >> frame;
        if (frame.empty())
        {
            std::cerr << "Error: Could not capture frame" << std::endl;
            break;
        }

        frame.copyTo(frameCopy);
        // detectAndDrawMarkers(frameCopy, estimateMarkerPose);
        detectAndDrawMarkers(frameCopy);

        // display number of markers detected in window
        cv::putText(frameCopy, "Number of markers detected: " + std::to_string(markerIds.size()), cv::Point(10, 30),
                    cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 2);

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
            saveCalibrationImage(frame);
            cv::waitKey(500);
            // break;
        }
        if (key == 'c' || key == 'C')
        {
            std::cout << "Calibrating camera" << std::endl;
            if (numOfCalibrationImages > 5)
            {
                calibrateCamera(calibrationDirectory);
                // break;
            }
            else
            {
                std::cout << "Need at least 5 calibration images" << std::endl;
                // break;
            }
        }
    }

    return 0;
}