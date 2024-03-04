// Author: Kevin Heleodoro
// Date: March 1, 2024
// Purpose: A collection of utils used for Aruco marker recognition and calibration

#ifndef ARUCO_UTILS_H
#define ARUCO_UTILS_H

/**
 * @brief Creates a new Aruco marker and saves it to a file
 *
 * @param markerId The id of the marker to be created. Default is 23
 * @return void
 */
void createArucoMarker(int markerId = 23);

int videoStreaming();

#endif