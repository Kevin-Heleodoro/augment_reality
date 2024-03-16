#include <iostream>
#include <opencv2/aruco.hpp>
#include <opencv2/opencv.hpp>

#include "chessboard_utils.h"

using namespace std;
using namespace cv;

Mat chessFrame, chessFrameCopy;
Size chessboardSize(10 - 1, 7 - 1);
vector<vector<Point2f>> allImagePoints;
vector<vector<Point3f>> allObjectPoints;
vector<Point3f> objectPoints;
vector<Point2f> imagePoints;
vector<Mat> allCalibrationFrames;
int chessBoard[2] = {9, 6};
int squareSize = 25; // in mm
int numImages = 0;

void generateChessBoardImage()
{
}

double calibrateChessBoardCamera()
{
    Matx33f cameraMatrix(Matx33f::eye());
    vector<float> distCoeffs(5, 0);
    vector<Mat> rvecs, tvecs;
    vector<double> intrinsics, extrinsics, reprojectionErrors;
    int calibFlags = CALIB_FIX_ASPECT_RATIO + CALIB_FIX_K3 + CALIB_ZERO_TANGENT_DIST + CALIB_FIX_PRINCIPAL_POINT;
    Size frameSize(chessFrame.size());

    cout << "Values pre-calibration: " << endl;
    cout << "Camera Matrix:\n " << cameraMatrix << endl;
    cout << "Distortion Coefficients: ";
    for (const auto &coeff : distCoeffs)
    {
        cout << coeff << " ";
    }
    cout << endl;
    double rms =
        calibrateCamera(allObjectPoints, allImagePoints, frameSize, cameraMatrix, distCoeffs, rvecs, tvecs, calibFlags);

    cout << "Values post-calibration: " << endl;
    cout << "Reprojection Error: " << rms << endl;
    cout << "Camera Matrix:\n " << cameraMatrix << endl;
    cout << "Distortion Coefficients: ";
    for (const auto &coeff : distCoeffs)
    {
        cout << coeff << " ";
    }
    cout << "Rotation Vectors: " << rvecs.size() << endl;
    cout << "Translation Vectors: " << tvecs.size() << endl;
    cout << endl;

    FileStorage fs("chessboard_calibration_results.xml", FileStorage::WRITE);

    return rms;
}

void saveChessBoardImageParameters(Mat frame)
{
    std::string filename = "../img/CameraCalibration/" + std::to_string(++numImages) + "_chessboard_image.jpg";
    imwrite(filename, frame);
    std::cout << "Image saved as " << filename << std::endl;

    allObjectPoints.push_back(objectPoints);
    allImagePoints.push_back(imagePoints);
    allCalibrationFrames.push_back(frame);

    cout << "Number of images: " << numImages << endl;
    cout << "Number of object points: " << allObjectPoints.size() << endl;
    cout << "Object points saved: " << objectPoints << endl;
    cout << "Number of image points: " << allImagePoints.size() << endl;
    cout << "Image points saved: " << imagePoints << endl;
    cout << "Number of calibration frames: " << allCalibrationFrames.size() << endl;
}

void detectChessBoard()
{
    Mat gray;
    cvtColor(chessFrame, gray, COLOR_BGR2GRAY);

    bool found = findChessboardCorners(gray, chessboardSize, imagePoints,
                                       CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE + CALIB_CB_FAST_CHECK);

    if (found)
    {
        cornerSubPix(gray, imagePoints, Size(11, 11), Size(-1, -1),
                     TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 30, 0.1));
        drawChessboardCorners(chessFrameCopy, chessboardSize, Mat(imagePoints), found);
    }
}

int chessboardDetectionAndCalibration()
{
    VideoCapture cap(0);
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

    namedWindow("Chessboard Detection", WINDOW_AUTOSIZE);

    for (int i = 0; i < chessBoard[1]; i++)
    {
        for (int j = 0; j < chessBoard[0]; j++)
        {
            objectPoints.push_back(Point3f(j * squareSize, i * squareSize, 0));
        }
    }

    while (true)
    {

        cap >> chessFrame;
        chessFrame.copyTo(chessFrameCopy);

        detectChessBoard();

        imshow("Chessboard Detection", chessFrameCopy);

        char key = (char)waitKey(10);
        if (key == 'q' || key == 'Q' || key == 27)
        {
            std::cout << "User terminated program" << std::endl;
            break;
        }
        if (key == 's' || key == 'S')
        {
            std::cout << "Saving frame" << std::endl;
            saveChessBoardImageParameters(chessFrameCopy);
            cv::waitKey(500);
        }
        if (key == 'c' || key == 'C')
        {
            std::cout << "Calibrating camera" << std::endl;
            double error = calibrateChessBoardCamera();
        }
    }

    return 0;
}