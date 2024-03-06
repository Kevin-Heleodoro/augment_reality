# Augmented Reality

## Task 1

-   [Aruco Marker Detection Tutorial](https://docs.opencv.org/4.x/d5/dae/tutorial_aruco_detection.html)
-   [`detectMarkers()`](https://docs.opencv.org/4.x/d2/d1a/classcv_1_1aruco_1_1ArucoDetector.html#a0c1d14251bf1cbb06277f49cfe1c9b61)
-   [`drawDetectedMarkers()`](https://docs.opencv.org/4.x/de/d67/group__objdetect__aruco.html#ga2ad34b0f277edebb6a132d3069ed2909)

For this task I decided to use the Aruco markers due to them being more stable and offer non-symmetric shapes which allows us to track the top left corner in any orientation.
"Calibrating using ArUco is much more versatile than using traditional chessboard patterns, since it allows occlusions or partial views."

![aruco](img/task_1/detect_markers_1.png)

## Task 2

-   [ArUco Board](https://github.com/opencv/opencv/blob/4.x/doc/tutorials/objdetect/aruco_board_detection/images/board.png)
-   [ChArUco Board](https://github.com/opencv/opencv/blob/4.x/doc/charuco_board_pattern.png)
-   [OpenCV ArUco Board Detection](https://docs.opencv.org/4.x/db/da9/tutorial_aruco_board_detection.html)

<!-- For task 2 OpenCV recommends using a ChArUco board for camera calibration as it is more precise than both the chessboard and ArUco boards. A benefit to using the ChArUco board is that it allows for occlusions and impartial views during camera calibration. --> The ChArUco board requires some refactoring of the code for it to work. Will come back to this later.

Instead of passing in an image, the calibration targets will be captured by the live video stream at different angles. Each time the user hits 's', the image and the corners associated with it will be saved for calibration purposes.

<!-- -w=5 -h=7 -l=100 -s=10
-v=/path_to_opencv/opencv/doc/tutorials/objdetect/aruco_board_detection/gboriginal.jpg
-c=/path_to_opencv/opencv/samples/cpp/tutorial_code/objectDetection/tutorial_camera_params.yml
-cd=/path_to_opencv/opencv/samples/cpp/tutorial_code/objectDetection/tutorial_dict.yml -->

## Resources

-   [Parsing program options](https://medium.com/@mostsignificant/3-ways-to-parse-command-line-arguments-in-c-quick-do-it-yourself-or-comprehensive-36913284460f)
-   [OpenCV GitHub Repo](https://github.com/opencv/opencv/tree/4.x/samples)
