// Author: Kevin Heleodoro
// Date: March 6, 2024
// Purpose: A collection of utils used for camera calibration

#include <opencv2/aruco.hpp>
#include <opencv2/opencv.hpp>

#include "camera_utils.h"

// int calibrateCamera(std::string calibrationDirectory)
// {
//     return 0;
// }

double calibrateCamera(cv::Mat &cameraMatrix, cv::Mat &distCoeffs, cv::Size &imageSize,
                       std::vector<std::vector<cv::Vec3f>> point_list,
                       std::vector<std::vector<cv::Point2f>> corner_list, std::vector<cv::Mat> &rvecs,
                       std::vector<cv::Mat> &tvecs, std::vector<int> &markerIds,
                       std::vector<int> &markerCounterPerFrame, cv::Ptr<cv::aruco::Board> &board)
{
    std::cout << "Parameters passed to camera calibration function: " << std::endl;
    std::cout << "Camera Matrix:\n " << cameraMatrix << std::endl;
    std::cout << "Distortion Coefficients: " << distCoeffs << std::endl;
    std::cout << "Image Size: " << imageSize << std::endl;
    std::cout << "Point List Size: " << point_list.size() << std::endl;
    std::cout << "Corner List Size: " << corner_list.size() << std::endl;

    // std::vector<cv::Mat> rvecs, tvecs;
    std::vector<float> reprojectionErrors;

    double rms = cv::aruco::calibrateCameraAruco(corner_list, markerIds, markerCounterPerFrame, board, imageSize,
                                                 cameraMatrix, distCoeffs, rvecs, tvecs);

    // Results from the calibration
    std::cout << "\nResults from the calibration: \n";
    std::cout << "Reprojection Error: " << rms << std::endl;
    std::cout << "Camera Matrix:\n " << cameraMatrix << std::endl;
    std::cout << "Distortion Coefficients: " << distCoeffs << std::endl;
    std::cout << "Focal Length 'fx': " << cameraMatrix.at<double>(0, 0) << std::endl;
    std::cout << "Focal Length 'fy': " << cameraMatrix.at<double>(1, 1) << std::endl;
    std::cout << "Principal Point 'u0': " << cameraMatrix.at<double>(0, 2) << std::endl;
    std::cout << "Principal Point 'v0': " << cameraMatrix.at<double>(1, 2) << std::endl;
    std::cout << "Rvecs: " << rvecs.size() << std::endl;
    std::cout << "Tvecs: " << tvecs.size() << std::endl;

    cv::FileStorage fs("calibration_results.xml", cv::FileStorage::WRITE);
    fs << "cameraMatrix" << cameraMatrix << "distCoeffs" << distCoeffs;
    fs.release();

    return rms;
}

double calibrateCamera(cv::Mat &cameraMatrix, cv::Mat &distCoeffs, cv::Size &imageSize,
                       std::vector<std::vector<cv::Vec3f>> point_list,
                       std::vector<std::vector<cv::Point2f>> corner_list, std::vector<cv::Mat> &rvecs,
                       std::vector<cv::Mat> &tvecs)
{
    std::cout << "Parameters passed to camera calibration function: " << std::endl;
    std::cout << "Camera Matrix:\n " << cameraMatrix << std::endl;
    std::cout << "Distortion Coefficients: " << distCoeffs << std::endl;
    std::cout << "Image Size: " << imageSize << std::endl;
    std::cout << "Point List Size: " << point_list.size() << std::endl;
    std::cout << "Corner List Size: " << corner_list.size() << std::endl;

    // std::vector<cv::Mat> rvecs, tvecs;
    std::vector<float> reprojectionErrors;

    double rms = cv::calibrateCamera(point_list, corner_list, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs,
                                     cv::CALIB_FIX_ASPECT_RATIO); // re-project error

    // Results from the calibration
    std::cout << "\nResults from the calibration: \n";
    std::cout << "Reprojection Error: " << rms << std::endl;
    std::cout << "Camera Matrix:\n " << cameraMatrix << std::endl;
    std::cout << "Distortion Coefficients: " << distCoeffs << std::endl;
    std::cout << "Focal Length 'fx': " << cameraMatrix.at<double>(0, 0) << std::endl;
    std::cout << "Focal Length 'fy': " << cameraMatrix.at<double>(1, 1) << std::endl;
    std::cout << "Principal Point 'u0': " << cameraMatrix.at<double>(0, 2) << std::endl;
    std::cout << "Principal Point 'v0': " << cameraMatrix.at<double>(1, 2) << std::endl;
    std::cout << "Rvecs: " << rvecs.size() << std::endl;
    std::cout << "Tvecs: " << tvecs.size() << std::endl;

    cv::FileStorage fs("calibration_results.xml", cv::FileStorage::WRITE);
    fs << "cameraMatrix" << cameraMatrix << "distCoeffs" << distCoeffs;
    fs.release();

    return rms;
}

void readCameraParameters(cv::Mat &cameraMatrix, cv::Mat &distCoeffs, std::string filename = "calibration_results.xml")
{
    std::cout << "Reading camera parameters from file: " << filename << std::endl;
    cv::FileStorage fs(filename, cv::FileStorage::READ);
    fs["cameraMatrix"] >> cameraMatrix;
    fs["distCoeffs"] >> distCoeffs;
    fs.release();
    std::cout << "Camera Matrix:\n " << cameraMatrix << std::endl;
    std::cout << "Distortion Coefficients: " << distCoeffs << std::endl;
    std::cout << "Reading complete!\n" << std::endl;
}