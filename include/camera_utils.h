// Author: Kevin Heleodoro
// Date: March 6, 2024
// Purpose: A collection of utils used for camera calibration

#include <string>

#ifndef CAMERA_UTILS_H
#define CAMERA_UTILS_H

// int calibrateCamera(std::string calibrationDirectory);

double calibrateCamera(cv::Mat &cameraMatrix, cv::Mat &distCoeffs, cv::Size &imageSize,
                    std::vector<std::vector<cv::Vec3f>> point_list, std::vector<std::vector<cv::Point2f>> corner_list);

#endif