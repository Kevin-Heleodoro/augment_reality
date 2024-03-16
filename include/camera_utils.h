// Author: Kevin Heleodoro
// Date: March 6, 2024
// Purpose: A collection of utils used for camera calibration

#ifndef CAMERA_UTILS_H
#define CAMERA_UTILS_H

// int calibrateCamera(std::string calibrationDirectory);

double calibrateCamera(cv::Mat &cameraMatrix, cv::Mat &distCoeffs, cv::Size &imageSize,
                       std::vector<std::vector<cv::Vec3f>> point_list,
                       std::vector<std::vector<cv::Point2f>> corner_list, std::vector<cv::Mat> &rvecs,
                       std::vector<cv::Mat> &tvecs);

double calibrateCamera(cv::Mat &cameraMatrix, cv::Mat &distCoeffs, cv::Size &imageSize,
                       std::vector<std::vector<cv::Vec3f>> point_list,
                       std::vector<std::vector<cv::Point2f>> corner_list, std::vector<cv::Mat> &rvecs,
                       std::vector<cv::Mat> &tvecs, std::vector<int> &markerIds,
                       std::vector<int> &markerCounterPerFrame, cv::Ptr<cv::aruco::Board> &board);

void readCameraParameters(cv::Mat &cameraMatrix, cv::Mat &distCoeffs, std::string filename);

#endif