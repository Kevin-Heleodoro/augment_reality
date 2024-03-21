// Author: Kevin Heleodoro
// Date: March 18, 2024
// Purpose: A collection of utils used for Harris Corner Detection

#include <opencv2/aruco.hpp>
#include <opencv2/opencv.hpp>

#include "harris_detection.h"

using namespace std;
using namespace cv;

// ----------------- Global Variables ----------------- //
Mat src, grayImage;
const char *source_window = "Original image";
const char *corners_window = "Harris Corner Detection";
// ---------------------------------------------------- //

/**
 * @brief This function is used to detect corners in an image using the Harris Corner Detection algorithm
 *
 * @param inputImage
 * @param blockSize
 * @param apertureSize
 * @param k
 *
 */
Mat harrisCornerDetection(Mat &inputImage, int blockSize, int apertureSize, double k)
{
    Mat dst, dst_norm, dst_norm_scaled, outputImage;
    // int threshold = 200;
    int maxThreshold = 255;
    cvtColor(inputImage, grayImage, COLOR_BGR2GRAY);
    dst = Mat::zeros(grayImage.size(), CV_32FC1);
    cornerHarris(grayImage, dst, blockSize, apertureSize, k);
    cornerSubPix(grayImage, dst, Size(5, 5), Size(-1, -1),
                 TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 30, 0.01));
    normalize(dst, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
    convertScaleAbs(dst_norm, dst_norm_scaled);
    outputImage = inputImage.clone();
    int threshold = 225;

    for (int i = 0; i < dst_norm.rows; i++)
    {
        for (int j = 0; j < dst.cols; j++)
        {
            if ((int)dst_norm.at<float>(i, j) > threshold)
            {
                circle(outputImage, Point(j, i), 5, Scalar(0, 0, 255), 2);
            }
        }
    }

    return outputImage;
}

/**
 * @brief This function is used to start the video stream and apply the Harris Corner Detection algorithm
 *
 * @param calibrationFileName
 */
int startVideoStream(string calibrationFileName)
{
    VideoCapture cap(0);
    if (!cap.isOpened())
    {
        cerr << "Error opening video stream or file" << endl;
        return -1;
    }
    int blockSize = 2;
    int apertureSize = 3;
    double k = 0.04;

    namedWindow(source_window, WINDOW_AUTOSIZE);
    namedWindow(corners_window, WINDOW_AUTOSIZE);

    Mat frame, harrisFrame;
    while (true)
    {
        cap >> frame;
        if (frame.empty())
        {
            cerr << "Error: frame is empty" << endl;
            break;
        }

        harrisFrame = harrisCornerDetection(frame, blockSize, apertureSize, k);

        cvtColor(frame, grayImage, COLOR_BGR2GRAY);

        imshow(source_window, frame);
        imshow(corners_window, harrisFrame);

        char key = (char)waitKey(10);
        if (key == 'q' || key == 'Q')
        {
            cout << "User terminated program" << endl;
            break;
        }
        if (key == 's' || key == 'S')
        {
            imwrite("harris_corner_detection.jpg", harrisFrame);
            cout << "Image saved as 'harris_corner_detection.jpg'" << endl;
        }
    }
    cap.release();
    destroyAllWindows();
    return 0;
}