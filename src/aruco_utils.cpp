// Author: Kevin Heleodoro
// Date: March 1, 2024
// Purpose: A collection of utils used for Aruco marker recognition and calibration

#include <opencv2/aruco.hpp>
#include <opencv2/opencv.hpp>

#include "aruco_utils.h"
#include "camera_utils.h"

using namespace std;
using namespace cv;

//--------------------- Global Variables ---------------------//
string defaultCalibrationDirectory = "../img/CameraCalibration/";
aruco::DetectorParameters detectorParams;
aruco::Dictionary dict;
Mat cameraMatrix, distCoeffs, frame, frameCopy;
Size imageSize;
vector<int> markerIds, markerIdsCopy, markerCounterPerFrame;
// vector<int> markerCounterPerFrame;
vector<vector<Point2f>> markerCorners, rejectedCandidates;
Ptr<aruco::Board> arucoBoard;

// Future Goal: Create a class or struct to store the following variables
vector<Vec3f> point_set;             // should equal markerCorners // object points
vector<vector<Vec3f>> point_list;    // should be the same size as corner_list
vector<vector<Point2f>> corner_list; // image points
vector<Mat> calibrationImages;
vector<Mat> tvecs, rvecs;
int numOfCalibrationImages;
float markerSize, aspectRatio;
// int markersX = 7;
int markersX = 5;
int markersY = 7;
// int markersY = 5;
int margins = 10;
int borderBits = 1;
int markerLength = 10;
// int markerLength = 100;
int markerSeparation = 10;
// int markerSeparation = 10;
// Size boardSize = Size(780, 560);
Size boardSize = Size(560, 780);
bool isCalibrated = false;
bool areVariablesInitialized = false;

//------------------------------------------------------------//

/**
 * @brief Get the current date and time as a formatted string.
 *
 * This function retrieves the current date and time from the system clock and formats it as a string
 * in the format "YYYY-MM-DD_HH-MM-SS". The formatted string is returned.
 *
 * @return string The current date and time as a formatted string.
 */
string getCurrentDateTimeStamp()
{
    auto now = chrono::system_clock::now();
    time_t currentTime = chrono::system_clock::to_time_t(now);

    stringstream ss;
    ss << put_time(localtime(&currentTime), "%Y-%m-%d_%H-%M-%S");

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
    cout << "Creating new Aruco marker..." << endl;
    Mat markerImage;
    aruco::Dictionary arucoDict = aruco::getPredefinedDictionary(aruco::DICT_6X6_250);
    int markerSize = 200;
    int borderBits = 1;
    aruco::generateImageMarker(arucoDict, markerId, markerSize, markerImage, borderBits);
    string filename = "aruco_marker_" + to_string(markerId) + ".png";
    imwrite(filename, markerImage);
}

/**
 * @brief Creates a new Aruco board and saves it to a file
 */
void createArucoBoard()
{
    cout << "Creating new Aruco board..." << endl;
    dict = aruco::getPredefinedDictionary(aruco::DICT_6X6_250);
    Mat boardImage;
    aruco::GridBoard board = aruco::GridBoard(Size(markersX, markersY), markerLength, markerSeparation, dict);
    string filename = "aruco_board_" + getCurrentDateTimeStamp() + ".png";
    board.generateImage(boardSize, boardImage, margins, borderBits);
    imwrite(filename, boardImage);
    cout << "Aruco board created and saved to " << filename << endl;
}

/**
 * @brief Detects and draws Aruco markers in the video stream
 *
 * @param src The source image
 * @param estimatePose Flag to estimate the pose of the markers
 * @return void
 */
void detectAndDrawMarkers(Mat &src, bool estimatePose = false, bool showRejected = false)
{
    aruco::GridBoard board = aruco::GridBoard(Size(markersX, markersY), markerLength, markerSeparation, dict);

    arucoBoard = makePtr<aruco::Board>(board);
    // cout << "Aruco Board: " << board << endl;
    // cout << "Marker Size: " << board.getMarkerLength() << endl;
    // cout << "Marker Separation: " << board.getMarkerSeparation() << endl;
    // cout << "Number of markers: " << board.getGridSize().area() << endl;
    // cout << "Board Size: " << board.getObjPoints() << endl;

    aruco::ArucoDetector detector(dict, detectorParams);
    detector.detectMarkers(src, markerCorners, markerIds, rejectedCandidates);

    int markersDetected = 0;
    // if (!markerIds.empty())
    // {
    //     Mat objPoints, imgPoints;
    //     board.matchImagePoints(markerCorners, markerIds, objPoints, imgPoints);
    //     solvePnP(objPoints, imgPoints, cameraMatrix, distCoeffs, rvecs, tvecs);
    //     markersDetected = (int)objPoints.total() / 4;
    //     aruco::estimatePoseSingleMarkers(markerCorners, markerLength, cameraMatrix, distCoeffs, rvecs, tvecs);
    //     // for (int i = 0; i < markerIds.size(); i++)
    //     // {
    //     //     aruco::drawAxis(src, cameraMatrix, distCoeffs, rvecs[i], tvecs[i], markerLength * 0.5f);
    //     // }
    // }

    if (markerIds.size() > 0)
    {
        aruco::drawDetectedMarkers(src, markerCorners, markerIds);
    }

    if (showRejected && !rejectedCandidates.empty())
    {
        aruco::drawDetectedMarkers(src, rejectedCandidates, noArray(), Scalar(100, 0, 255));
    }

    // if (estimatePose)
    // {
    //     drawFrameAxes(src, cameraMatrix, distCoeffs, rvecs, tvecs, markerLength * 0.5f);
    // }
}

/**
 * @brief Prints the calibration variables to the console
 */
void printCalibrationVariables()
{
    // Iterating over corner_list and comparing each corner to the point_set
    for (size_t i = 0; i < corner_list.size(); i++)
    {
        for (size_t j = 0; j < corner_list[i].size(); j++)
        {
            cout << "Corner " << i + 1 << ": " << corner_list[i][j] << endl;
            cout << "Point " << i + 1 << ": " << point_set[j] << endl;
        }
    }
}

/**
 * @brief Saves the calibration variables to a file
 *
 * @param reprojectionError The reprojection error
 */
void saveCalibrationVariables(double reprojectionError = 0.0)
{
    string filename = "calibration_variables_" + getCurrentDateTimeStamp() + ".xml";
    FileStorage fs(filename, FileStorage::WRITE);
    fs << "frame_width" << frame.cols;
    fs << "frame_height" << frame.rows;
    fs << "aspect_ratio" << aspectRatio;
    fs << "camera_matrix" << cameraMatrix;
    fs << "dist_coeffs" << distCoeffs;
    fs << "reprojection_error" << reprojectionError;
    fs << "point_set" << point_set;
    fs << "corner_list" << corner_list;
    fs << "marker_corners" << markerCorners;
    fs.release();
}

/**
 * @brief Saves the calibration image to a file
 *
 * @param src The source image
 * @param calibrationDirectory The directory to save the calibration images
 * @return void
 */
void saveCalibrationImage(Mat &src, string calibrationDirectory = defaultCalibrationDirectory)
{
    // TODO: Add some error handling for the directory and validation for the image

    cout << "Marker Corners vs point_set: " << endl;
    cout << "Marker Corners: " << markerCorners.size() << endl;
    cout << "Point Set: " << point_set.size() << endl;

    vector<Point2f> corners;

    for (size_t i = 0; i < markerCorners.size(); i++)
    {
        for (size_t j = 0; j < markerCorners[i].size(); j++)
        {
            Point2f point(markerCorners[i][j].x, markerCorners[i][j].y);
            corners.push_back(point);
            // corners.push_back(markerCorners[i][j]);
            // cout << "Marker ID: " << markerIds[i] << endl;
            // cout << "Marker Corner: " << markerCorners[i][j] << endl;
            // cout << "Corner: " << point << endl;
        }
    }

    if (corners.size() != point_set.size())
    {
        cerr << "\n===========\nError: The number of corners and points do not match\n===========\n" << endl;
        return;
    }

    corner_list.push_back(corners);
    // aruco::GridBoard board =
    //     aruco::GridBoard(Size(markersX, markersY), markerLength, markerSeparation, dict);
    // cout << "Object Points: " << board.getObjPoints()[0] << endl;
    point_list.push_back(point_set);
    markerCounterPerFrame.push_back((int)markerIds.size());
    markerIdsCopy = markerIds;

    string filename = calibrationDirectory + to_string(numOfCalibrationImages) + "_calibration_image.png";
    imwrite(filename, src);
    numOfCalibrationImages++;
    cout << "Calibration image saved" << endl;
    calibrationImages.push_back(src);

    cout << "Number of calibration images: " << numOfCalibrationImages << endl;
    cout << "Number of corner sets: " << corner_list.size() << endl;
    cout << "Number of corners in last set: " << corner_list[corner_list.size() - 1].size() << endl;
    cout << "Number of points: " << point_list.size() << endl;
    cout << "Number of points in last set: " << point_list[point_list.size() - 1].size() << endl;
    cout << "Number of marker ids: " << markerIds.size() << endl;
    cout << "Number of marker ids in last set: " << markerCounterPerFrame[markerCounterPerFrame.size() - 1] << endl;
    cout << "Number of marker corners: " << markerCorners.size() << endl;
    cout << "Number of marker corners in last set: " << markerCorners[markerCorners.size() - 1].size() << endl;
    cout << "Number of rejected candidates: " << rejectedCandidates.size() << endl;

    cout << "\n" << endl;

    // printCalibrationVariables();
}

/**
 * @brief Initializes the variables used for camera calibration
 *
 * @return void
 */
void initializeVariables()
{
    cout << "Initializing variables..." << endl;
    numOfCalibrationImages = 0;
    dict = aruco::getPredefinedDictionary(aruco::DICT_6X6_250);
    detectorParams = aruco::DetectorParameters();
    aspectRatio = 1;
    double focalLength = frame.cols;
    cout << "Focal Length: " << focalLength << endl;
    cout << "Frame width: " << frame.cols << endl;
    cout << "Frame height: " << frame.rows << endl;
    cout << "Aspect Ratio: " << aspectRatio << endl;

    distCoeffs = Mat::zeros(5, 1, CV_64F);
    // distCoeffs = Mat::zeros(8, 1, CV_64F);

    /**
     * @brief The camera matrix is a 3x3 matrix that represents the intrinsic parameters of the camera. Initialized to
     * [1, 0, frame.cols/2]
     * [0, 1, frame.rows/2]
     * [0, 0, 1           ]
     */
    cameraMatrix = Mat::eye(3, 3, CV_64FC1);
    // cameraMatrix.at<double>(0, 0) = focalLength * aspectRatio;
    cameraMatrix.at<double>(0, 0) = 1;
    cameraMatrix.at<double>(0, 1) = 0;
    cameraMatrix.at<double>(0, 2) = frame.cols / 2.0;
    cameraMatrix.at<double>(1, 0) = 0;
    // cameraMatrix.at<double>(1, 1) = focalLength;
    cameraMatrix.at<double>(1, 1) = 1;
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
            point_set.push_back(Vec3f(y, x, 0));
            point_set.push_back(Vec3f(y + markerLength, x, 0));
            point_set.push_back(Vec3f(y + markerLength, x + markerLength, 0));
            point_set.push_back(Vec3f(y, x + markerLength, 0));
            // point_set.push_back(Vec3f(x, y, 0));
            // point_set.push_back(Vec3f(x + markerLength, y, 0));
            // point_set.push_back(Vec3f(x + markerLength, y + markerLength, 0));
            // point_set.push_back(Vec3f(x, y + markerLength, 0));
        }
    }

    cout << "Point Set Size: " << point_set.size() << endl;
    cout << "Camera Matrix" << cameraMatrix << endl;

    areVariablesInitialized = true;
    cout << "Variables initialized! \n" << endl;
}

/**
 * @brief Starts the video stream and applies the Aruco marker detection algorithm
 *
 * @param cameraCalibrationFile The file containing the camera calibration parameters
 */
int videoStreaming(string cameraCalibrationFile)
{
    VideoCapture cap(0);
    if (!cap.isOpened())
    {
        cerr << "Error opening video stream" << endl;
        return -1;
    }

    if (cameraCalibrationFile != "")
    {
        readCameraParameters(cameraMatrix, distCoeffs, cameraCalibrationFile);
        isCalibrated = true;
    }

    cout << "\nWelcome to the Augmented Reality Application\n" << endl;
    cout << "Press 'q' to quit the program" << endl;
    cout << "Press 's' to save a calibration image" << endl;
    cout << "Press 'c' to calibrate the camera" << endl;
    cout << "\n" << endl;

    // calibrationDirectory = calibrationDirectory == "" ? defaultCalibrationDirectory : calibrationDirectory;
    namedWindow("Video Stream", WINDOW_AUTOSIZE);

    cout << "Initial Camera Matrix: " << cameraMatrix << endl;

    while (true)
    {
        // Mat frame, frameCopy;
        cap >> frame;
        if (frame.empty())
        {
            cerr << "Error: Could not capture frame" << endl;
            break;
        }
        if (!areVariablesInitialized)
        {
            initializeVariables();
        }

        // flip image vertically
        // flip(frame, frame, 1);

        frame.copyTo(frameCopy);
        imageSize = frame.size();
        detectAndDrawMarkers(frameCopy);

        // display number of markers detected in window
        putText(frameCopy, "Number of markers detected: " + to_string(markerIds.size()), Point(10, 30),
                FONT_HERSHEY_SIMPLEX, .75, Scalar(0, 0, 255), 2);

        imshow("Video Stream", frameCopy);

        char key = waitKey(10);
        if (key == 'q' || key == 'Q')
        {
            cout << "User terminated program" << endl;
            break;
        }
        if (key == 's' || key == 'S')
        {
            cout << "Saving frame" << endl;
            saveCalibrationImage(frameCopy, defaultCalibrationDirectory);
            waitKey(500);
        }
        if (key == 'c' || key == 'C')
        {
            cout << "Calibrating camera" << endl;
            if (numOfCalibrationImages >= 5)
            {
                // TODO: Make sure that this calibration call allows the video loop to continue
                cout << "User began calibration" << endl;
                arucoBoard->matchImagePoints(markerCorners, markerIds, point_set, corner_list);
                // double result = calibrateCamera(cameraMatrix, distCoeffs, boardSize, point_list, corner_list);
                double result = calibrateCamera(cameraMatrix, distCoeffs, imageSize, point_list, corner_list, rvecs,
                                                tvecs, markerIdsCopy, markerCounterPerFrame, arucoBoard);
                // double result = calibrateCamera(cameraMatrix, distCoeffs, imageSize, point_list, corner_list, rvecs,
                //                                 tvecs);
                saveCalibrationVariables(result);

                if (result > 0)
                {
                    cout << "Camera calibration successful" << endl;
                    isCalibrated = true;
                }
                else
                {
                    cout << "Camera calibration failed" << endl;
                }
            }
            else
            {
                cout << "Need at least 5 calibration images" << endl;
            }
            continue;
        }
    }

    return 0;
}