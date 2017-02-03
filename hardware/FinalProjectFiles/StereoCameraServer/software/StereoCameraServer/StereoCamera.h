/******************************************************************************
 * File: StereoCamera.h
 * Author: Leo Nickerson, Timo Hohn
 * Client: ECE 492 project
 * Version: 0.50
 * Last Modified: 09/04/2013
 *
 * Library for using two instances of the Camera module declared in Camera.h
 *
 * This library was written for interfacing the cameras with Altera DE2 board
 * running a NIOS II processor
 *
 * This library was written with the supplied Adafruit code as reference.  This
 * library contains basic functionality for using the camera module as a camera
 * it does not support all commands available in the camera module and is meant
 * for demonstration purposes
 *
 * This module was designed to be used with the Altera DE2 board using the
 * Altera Avalon drivers as well as the uC OS II. The code was developed in
 * NIOS II Eclipse IDE v10.1 and cannot be guaranteed to work outside of this
 * environment
 *
 * Note: The StereoCamera class can be used as a wrapper for only one camera by
 * initializing the StereoCamera class with one path as null however the camera
 * being used must be specified in all functions
 *****************************************************************************/

#ifndef STEREOCAMERA_H
#define SERTEOCAMERA_H

#include "Camera.h"

//If SCAM_DEBUG is defined this module will print debug statement when taking
//pictures
#define SCAM_DEBUG
//SCAM_LED_DEBUG can be used to display display debug info when stdout can't
//be used
#define SCAM_LED_DEBUG

class StereoCamera {
public:
	// Constructor
	StereoCamera(const char * path0, const char * path1);
	// resets the cameras
	bool reset(int cameraNum = 2);
	// sets the camera image size to large
	void setResLrg(int cameraNum = 2);
	// takes a picture
	bool takePicture(int cameraNum = 2);
	// resumes video
	bool resumeVideo(int cameraNum = 2);
	// reads picture to specified address
	void readPicture(alt_64 addr0, alt_64 addr1);
	// returns the frame length of the specified camera
	int getFrameLength(int cameraNum);
	// sets the number of bytes read from the camera at a time
	alt_u8 setReadLen(alt_u8 len);

private:
	// The cameras being controlled
	Camera camera0, camera1;
	// The number of bytes being read from the camera at a time
	// The larger the number the less time to complete the read but with a
	// larger chance for errors to occur, tested at rate of 16, 32, and 64
	alt_u8 READ_LEN;
};

#endif
