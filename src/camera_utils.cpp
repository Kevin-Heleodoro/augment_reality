// Author: Kevin Heleodoro
// Date: March 6, 2024
// Purpose: A collection of utils used for camera calibration

#include <opencv2/aruco.hpp>
#include <opencv2/opencv.hpp>

#include "camera_utils.h"

using namespace std;
using namespace cv;

/**
 * @brief Function to calibrate the camera using an aruco board
 *
 * @param cameraMatrix
 * @param distCoeffs
 * @param imageSize
 * @param point_list
 * @param corner_list
 * @param rvecs
 * @param tvecs
 * @param markerIds
 * @param markerCounterPerFrame
 * @param board
 */
double calibrateCamera(Mat &cameraMatrix, Mat &distCoeffs, Size &imageSize, vector<vector<Vec3f>> point_list,
                       vector<vector<Point2f>> corner_list, vector<Mat> &rvecs, vector<Mat> &tvecs,
                       vector<int> &markerIds, vector<int> &markerCounterPerFrame, Ptr<aruco::Board> &board)
{
    cout << "Parameters passed to camera calibration function: " << endl;
    cout << "Camera Matrix:\n " << cameraMatrix << endl;
    cout << "Distortion Coefficients: " << distCoeffs << endl;
    cout << "Image Size: " << imageSize << endl;
    cout << "Point List Size: " << point_list.size() << endl;
    cout << "Corner List Size: " << corner_list.size() << endl;
    cout << "Marker Ids Size: " << markerIds.size() << endl;
    cout << "Marker Counter Per Frame Size: " << markerCounterPerFrame.size() << endl;
    cout << "Board: " << board << endl;

    // vector<Mat> rvecs, tvecs;
    vector<float> reprojectionErrors;

    double rms = aruco::calibrateCameraAruco(corner_list, markerIds, markerCounterPerFrame, board, imageSize,
                                             cameraMatrix, distCoeffs, rvecs, tvecs);

    // Results from the calibration
    cout << "\nResults from the calibration: \n";
    cout << "Reprojection Error: " << rms << endl;
    cout << "Camera Matrix:\n " << cameraMatrix << endl;
    cout << "Distortion Coefficients: " << distCoeffs << endl;
    cout << "Focal Length 'fx': " << cameraMatrix.at<double>(0, 0) << endl;
    cout << "Focal Length 'fy': " << cameraMatrix.at<double>(1, 1) << endl;
    cout << "Principal Point 'u0': " << cameraMatrix.at<double>(0, 2) << endl;
    cout << "Principal Point 'v0': " << cameraMatrix.at<double>(1, 2) << endl;
    cout << "Rvecs: " << rvecs.size() << endl;
    cout << "Tvecs: " << tvecs.size() << endl;

    FileStorage fs("calibration_results.xml", FileStorage::WRITE);
    fs << "cameraMatrix" << cameraMatrix << "distCoeffs" << distCoeffs;
    fs.release();

    return rms;
}

/**
 * @brief Function to calibrate the camera using a chessboard
 *
 * @param cameraMatrix
 * @param distCoeffs
 * @param imageSize
 * @param point_list
 * @param corner_list
 * @param rvecs
 * @param tvecs

*/
double calibrateCamera(Mat &cameraMatrix, Mat &distCoeffs, Size &imageSize, vector<vector<Vec3f>> point_list,
                       vector<vector<Point2f>> corner_list, vector<Mat> &rvecs, vector<Mat> &tvecs)
{
    cout << "Parameters passed to camera calibration function: " << endl;
    cout << "Camera Matrix:\n " << cameraMatrix << endl;
    cout << "Distortion Coefficients: " << distCoeffs << endl;
    cout << "Image Size: " << imageSize << endl;
    cout << "Point List Size: " << point_list.size() << endl;
    cout << "Corner List Size: " << corner_list.size() << endl;

    // vector<Mat> rvecs, tvecs;
    vector<float> reprojectionErrors;

    double rms = calibrateCamera(point_list, corner_list, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs,
                                 CALIB_FIX_ASPECT_RATIO); // re-project error

    // Results from the calibration
    cout << "\nResults from the calibration: \n";
    cout << "Reprojection Error: " << rms << endl;
    cout << "Camera Matrix:\n " << cameraMatrix << endl;
    cout << "Distortion Coefficients: " << distCoeffs << endl;
    cout << "Focal Length 'fx': " << cameraMatrix.at<double>(0, 0) << endl;
    cout << "Focal Length 'fy': " << cameraMatrix.at<double>(1, 1) << endl;
    cout << "Principal Point 'u0': " << cameraMatrix.at<double>(0, 2) << endl;
    cout << "Principal Point 'v0': " << cameraMatrix.at<double>(1, 2) << endl;
    cout << "Rvecs: " << rvecs.size() << endl;
    cout << "Tvecs: " << tvecs.size() << endl;

    FileStorage fs("calibration_results.xml", FileStorage::WRITE);
    fs << "cameraMatrix" << cameraMatrix << "distCoeffs" << distCoeffs;
    fs.release();

    return rms;
}

/**
 * @brief Function to read the camera parameters from a file
 *
 * @param cameraMatrix
 * @param distCoeffs
 * @param filename
 */
void readCameraParameters(Mat &cameraMatrix, Mat &distCoeffs, string filename = "calibration_results.xml")
{
    cout << "Reading camera parameters from file: " << filename << endl;
    FileStorage fs(filename, FileStorage::READ);
    fs["cameraMatrix"] >> cameraMatrix;
    fs["distCoeffs"] >> distCoeffs;
    fs.release();
    cout << "Camera Matrix:\n " << cameraMatrix << endl;
    cout << "Distortion Coefficients: " << distCoeffs << endl;
    cout << "Reading complete!\n" << endl;
}
