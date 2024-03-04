// Author: Kevin Heleodoro
// Date: March 1, 2024
// Purpose: A collection of utils used for Aruco marker recognition and calibration

#include <opencv2/aruco.hpp>
#include <opencv2/opencv.hpp>

#include "aruco_utils.h"

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

void detectAndDrawMarkers(cv::Mat &src)
{
    std::vector<int> markerIds;
    std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;
    cv::aruco::DetectorParameters params = cv::aruco::DetectorParameters();
    cv::aruco::Dictionary dict = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    cv::aruco::ArucoDetector detector(dict, params);
    detector.detectMarkers(src, markerCorners, markerIds, rejectedCandidates);

    if (markerIds.size() > 0)
    {
        std::cout << "Detected " << markerIds.size() << " markers" << std::endl;
        cv::aruco::drawDetectedMarkers(src, markerCorners, markerIds);
    }
}

int videoStreaming()
{
    cv::VideoCapture cap(0);
    if (!cap.isOpened())
    {
        std::cerr << "Error opening video stream" << std::endl;
        return -1;
    }

    cv::namedWindow("Video Stream", cv::WINDOW_AUTOSIZE);

    while (true)
    {
        cv::Mat frame;
        cap >> frame;
        if (frame.empty())
        {
            std::cerr << "Error: Could not capture frame" << std::endl;
            break;
        }

        detectAndDrawMarkers(frame);

        cv::imshow("Video Stream", frame);

        char key = cv::waitKey(30);
        if (key == 'q' || key == 'Q')
        {
            std::cout << "User terminated program" << std::endl;
            break;
        }
        if (key == 's' || key == 'S')
        {
            std::cout << "Saving frame" << std::endl;
            imwrite("frame.png", frame);
        }
    }

    return 0;
}