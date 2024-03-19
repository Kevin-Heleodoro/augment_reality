#include <iostream>
#include <opencv2/aruco.hpp>
#include <opencv2/opencv.hpp>

#include "chessboard_utils.h"

using namespace std;
using namespace cv;

Mat chessFrame, chessFrameCopy, camMatrix, dCoeffs;
Size chessboardSize(10 - 1, 7 - 1);
vector<vector<Point2f>> allImagePoints;
vector<vector<Point3f>> allObjectPoints;
vector<Point3f> objectPoints;
vector<Point2f> imagePoints;
vector<Mat> allCalibrationFrames;
vector<Mat> rotationsVectors, translationsVectors;
int chessBoard[2] = {9, 6};
int squareSize = 25; // in mm
int numImages = 0;
bool cameraIsCalibrated = false;

void generateChessBoardImage()
{
}

void loadCalibrationFile(string filename)
{
    FileStorage fs(filename, FileStorage::READ);
    fs["camera_matrix"] >> camMatrix;
    fs["dist_coeffs"] >> dCoeffs;
    // fs["rotation_vectors"] >> rotationsVectors;
    // fs["translation_vectors"] >> translationsVectors;

    rotationsVectors.clear();
    FileNode rvecNode = fs["rotation_vectors"];
    for (FileNodeIterator n = rvecNode.begin(); n != rvecNode.end(); ++n)
    {
        cv::Mat tmp;
        *n >> tmp;
        rotationsVectors.push_back(tmp);
    }

    translationsVectors.clear();
    FileNode tvecNode = fs["translation_vectors"];
    for (FileNodeIterator n = tvecNode.begin(); n != tvecNode.end(); ++n)
    {
        cv::Mat tmp;
        *n >> tmp;
        translationsVectors.push_back(tmp);
    }

    fs.release();
    cameraIsCalibrated = true;

    cout << "Loading Parameters" << endl;
    cout << "Camera Matrix: " << camMatrix << endl;
    cout << "Distortion Coefficients: " << dCoeffs << endl;
    cout << "Rotation Vectors: " << rotationsVectors.size() << endl;
    cout << "Translation Vectors: " << translationsVectors.size() << endl;
    cout << "Finished loading ...\n" << endl;
}

void saveCalibrationFile(const cv::Mat &cameraMatrix, const cv::Mat &distCoeffs, double reprojectionError,
                         const std::vector<cv::Mat> &rvecs, const std::vector<cv::Mat> &tvecs, int frameWidth,
                         int frameHeight)
{
    FileStorage fs("chessboard_calibration_results.xml", FileStorage::WRITE);
    fs << "frame_width" << frameWidth;
    fs << "frame_height" << frameHeight;
    fs << "camera_matrix" << cameraMatrix;
    fs << "dist_coeffs" << distCoeffs;
    fs << "reprojection_error" << reprojectionError;
    fs << "rotation_vectors" << rvecs;
    fs << "translation_vectors" << tvecs;
    fs.release();
}

void saveChessBoardImageParameters(Mat frame)
{
    string filename = "../img/CameraCalibration/" + to_string(++numImages) + "_chessboard_image.jpg";
    imwrite(filename, frame);
    cout << "Image saved as " << filename << endl;

    allObjectPoints.push_back(objectPoints);
    allImagePoints.push_back(imagePoints);
    allCalibrationFrames.push_back(frame);

    cout << "Number of images: " << numImages << endl;
    cout << "Number of object points: " << allObjectPoints.size() << endl;
    cout << "Number of image points: " << allImagePoints.size() << endl;
    cout << "Number of calibration frames: " << allCalibrationFrames.size() << endl;
}

double calibrateChessBoardCamera()
{
    Matx33f cameraMatrix(Matx33f::eye());
    vector<float> distCoeffs(5, 0);
    vector<Mat> rvecs, tvecs;
    vector<double> intrinsics, extrinsics, reprojectionErrors;
    int calibFlags = CALIB_FIX_ASPECT_RATIO + CALIB_FIX_K3 + CALIB_ZERO_TANGENT_DIST + CALIB_FIX_PRINCIPAL_POINT;
    Size frameSize(chessFrame.size());

    cout << "\nValues pre-calibration: " << endl;
    cout << "Camera Matrix:\n " << cameraMatrix << endl;
    cout << "Distortion Coefficients: ";
    for (const auto &coeff : distCoeffs)
    {
        cout << coeff << " ";
    }
    cout << endl;
    double rms =
        calibrateCamera(allObjectPoints, allImagePoints, frameSize, cameraMatrix, distCoeffs, rvecs, tvecs, calibFlags);

    cout << "\nValues post-calibration: " << endl;
    cout << "Reprojection Error: " << rms << endl;
    cout << "Camera Matrix:\n " << cameraMatrix << endl;
    cout << "Distortion Coefficients: ";
    for (const auto &coeff : distCoeffs)
    {
        cout << coeff << " ";
    }
    cout << endl;
    cout << "Rotation Vectors: " << rvecs.size() << endl;
    cout << "Translation Vectors: " << tvecs.size() << endl;
    cameraIsCalibrated = true;

    Mat matrix = Mat(cameraMatrix);
    Mat distortion = Mat(distCoeffs);
    saveCalibrationFile(matrix, distortion, rms, rvecs, tvecs, frameSize.width, frameSize.height);

    return rms;
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

        if (cameraIsCalibrated)
        {
            Mat rvec, tvec;
            bool matchingPoints = objectPoints.size() == imagePoints.size();
            // cout << "Matching Points: " << matchingPoints << endl;
            if (matchingPoints)
            {
                // cout << "Using solvePnP" << endl;
                // cout << "cameraMatrix: " << camMatrix << endl;
                // cout << "dCoeffs: " << dCoeffs << endl;
                // cout << "objectPoints: " << objectPoints << endl;
                // cout << "imagePoints: " << imagePoints << endl;
                solvePnP(objectPoints, imagePoints, camMatrix, dCoeffs, rvec, tvec);
                // cout << "Rvec: " << rvec << endl;
                // cout << "Tvec: " << tvec << endl;
                rotationsVectors.push_back(rvec);
                translationsVectors.push_back(tvec);
                drawFrameAxes(chessFrameCopy, camMatrix, dCoeffs, rvec, tvec, 30, 10);
                projectPoints(objectPoints, rvec, tvec, camMatrix, dCoeffs, imagePoints);
                int topLeftCorner = 0;
                int topRightCorner = chessBoard[0] - 1;
                int bottomLeftCorner = chessBoard[0] * (chessBoard[1] - 1);
                int bottomRightCorner = chessBoard[0] * chessBoard[1] - 1;
                vector<int> boardCorners = {topLeftCorner, topRightCorner, bottomLeftCorner, bottomRightCorner};

                // for (int i = 0; i < boardCorners.size(); i++)
                // {
                //     int corner = boardCorners[i];
                //     circle(chessFrameCopy, imagePoints[corner], 5, Scalar(0, 0, 255), 2);
                //     line(chessFrameCopy, imagePoints[corner], imagePoints[corner] + Point2f(0, -30), Scalar(0, 0,
                //     255),
                //          2);
                //     line(chessFrameCopy, imagePoints[corner], imagePoints[corner] + Point2f(30, 0), Scalar(0, 0,
                //     255),
                //          2);
                // }

                // construct pyramid in 3d world space and project it onto the image
                vector<Point3f> pyramidPoints;
                pyramidPoints.push_back(Point3f(0, 0, 0));     // 0
                pyramidPoints.push_back(Point3f(0, 100, 0));   // 1
                pyramidPoints.push_back(Point3f(100, 100, 0)); // 2
                pyramidPoints.push_back(Point3f(100, 0, 0));   // 3

                pyramidPoints.push_back(Point3f(50, 50, -100)); // 4 peak

                pyramidPoints.push_back(Point3f(0, 0, -200));     // 5
                pyramidPoints.push_back(Point3f(0, 100, -200));   // 6
                pyramidPoints.push_back(Point3f(100, 100, -200)); // 7
                pyramidPoints.push_back(Point3f(100, 0, -200));   // 8

                vector<Point2f> projectedPoints;
                projectPoints(pyramidPoints, rvec, tvec, camMatrix, dCoeffs, projectedPoints);
                line(chessFrameCopy, projectedPoints[0], projectedPoints[1], Scalar(0, 0, 255), 2);
                line(chessFrameCopy, projectedPoints[1], projectedPoints[2], Scalar(0, 0, 255), 2);
                line(chessFrameCopy, projectedPoints[2], projectedPoints[3], Scalar(0, 0, 255), 2);
                line(chessFrameCopy, projectedPoints[3], projectedPoints[0], Scalar(0, 0, 255), 2);
                line(chessFrameCopy, projectedPoints[0], projectedPoints[4], Scalar(0, 0, 255), 2);
                line(chessFrameCopy, projectedPoints[1], projectedPoints[4], Scalar(0, 0, 255), 2);
                line(chessFrameCopy, projectedPoints[2], projectedPoints[4], Scalar(0, 0, 255), 2);
                line(chessFrameCopy, projectedPoints[3], projectedPoints[4], Scalar(0, 0, 255), 2);

                line(chessFrameCopy, projectedPoints[5], projectedPoints[6], Scalar(0, 0, 255), 2);
                line(chessFrameCopy, projectedPoints[6], projectedPoints[7], Scalar(0, 0, 255), 2);
                line(chessFrameCopy, projectedPoints[7], projectedPoints[8], Scalar(0, 0, 255), 2);
                line(chessFrameCopy, projectedPoints[8], projectedPoints[5], Scalar(0, 0, 255), 2);

                line(chessFrameCopy, projectedPoints[5], projectedPoints[4], Scalar(0, 0, 255), 2);
                line(chessFrameCopy, projectedPoints[6], projectedPoints[4], Scalar(0, 0, 255), 2);
                line(chessFrameCopy, projectedPoints[7], projectedPoints[4], Scalar(0, 0, 255), 2);
                line(chessFrameCopy, projectedPoints[8], projectedPoints[4], Scalar(0, 0, 255), 2);
            }
        }

        // drawChessboardCorners(chessFrameCopy, chessboardSize, Mat(imagePoints), found);
    }
}

int chessboardDetectionAndCalibration(string calibrationFile)
{
    VideoCapture cap(0);
    if (!cap.isOpened())
    {
        cerr << "Error opening video stream" << endl;
        return -1;
    }

    cout << "\nWelcome to the Augmented Reality Application\n" << endl;
    cout << "Press 'q' to quit the program" << endl;
    cout << "Press 's' to save a calibration image" << endl;
    cout << "Press 'c' to calibrate the camera" << endl;
    cout << "\n" << endl;

    if (calibrationFile != "")
    {
        cout << "Utilizing calibration file: " << calibrationFile << endl;
        try
        {
            loadCalibrationFile(calibrationFile);
        }
        catch (const cv::Exception &e)
        {
            cerr << "Error loading calibration file: " << e.what() << endl;
            return -1;
        }
    }

    cv::namedWindow("Chessboard Detection", WINDOW_AUTOSIZE);

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
            cout << "User terminated program" << endl;
            break;
        }
        if (key == 's' || key == 'S')
        {
            cout << "Saving frame..." << endl;
            saveChessBoardImageParameters(chessFrameCopy);
            cv::waitKey(500);
        }
        if (key == 'c' || key == 'C')
        {
            if (allImagePoints.size() < 1)
            {
                cout << "No images saved for calibration" << endl;
                continue;
            }
            else if (allImagePoints.size() > 5)
            {
                cout << "Calibrating camera..." << endl;
                double error = calibrateChessBoardCamera();
            }
        }
    }

    return 0;
}