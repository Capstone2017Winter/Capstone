/******************************************************************************
* File: Camera.cpp
* Author: Leo Nickerson, Timo Hohn
* Client: ECE 492 project
* Version: 1.00
* Last Modified: 09/04/2013
*
* Library for the TTL JPEG Camera (VC0706 chipset)
*
* Cameras use serial interface to communicate, 2 pins are required
*
* This library was written for interfacing the cameras with Altera DE2 board
* running a NIOS II processor
*
* Consult the header file for a full description of the functions
******************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "includes.h"
#include "Camera.h"
#include "sys/alt_errno.h"
#include "altera_avalon_uart_regs.h"

//Constructor
Camera::Camera() {
	frameptr = 0;
	bufferLen = 0;
	serialNum = 0;
}

//Sets up a file poitner for writing to/reading from the uart
bool Camera::begin(const char * path) {
	camera_fp = fopen(path, "r+");

	if (fcntl(fileno(camera_fp), F_SETFL, O_NONBLOCK) == -1)
		printf("Could not set to nonblocking input");

	return reset();
}

//Gets the cameras firmware version
alt_u8 *Camera::getVersion() {
	alt_u8 args[] = { 0x0 };

	sendCommand(GEN_VERSION, args, sizeof(args));

	readResponse(5, 200);
	if (!verifyResponse(GEN_VERSION))
		return 0;

	readResponse(11, 200);
	camerabuff[bufferLen++] = 0;

	return camerabuff;
}

//Resets the camera to default settings
bool Camera::reset() {
	alt_u8 args[] = { 0x0 };
	bool ret;

	sendCommand(RESET, args, sizeof(args));

	readResponse(5, 10);
	ret = verifyResponse(RESET);

	for (short i; i != -1; i++)
		;
	OSTimeDlyHMSM(0, 0, 0, 10);
	readResponse(CAMERABUFFSIZE, 10);
	camerabuff[bufferLen++] = 0;

	return ret;
}

// Gets the image size (returns a image size macro)
alt_u8 Camera::getImageSize() {
	alt_u8 args[] = { 0x4, 0x4, 0x1, 0x00, 0x19 };
	if (!runCommand(READ_DATA, args, sizeof(args), 6))
		return -1;

	return camerabuff[5];
}

// Sets the image size using the image size macros
bool Camera::setImageSize(alt_u8 res) {
	alt_u8 args[] = { 0x05, 0x04, 0x01, 0x00, 0x19, res };

	return runCommand(WRITE_DATA, args, sizeof(args), 5);
}

//Gets n bytes of data from the camera's frame buffer
alt_u8 *Camera::readPicture(alt_u8 n) {
	alt_u8 args[] = { 0x0C, 0x0, 0x0A, 0, 0, frameptr >> 8, frameptr & 0xFF, 0,
			0, 0, n, CAMERADELAY >> 8, CAMERADELAY & 0xFF };

	if (!runCommand(READ_FBUF, args, sizeof(args), 5, false))
		return NULL;

	if (readResponse(n + 5, CAMERADELAY) == 0)
		return NULL;

	frameptr += n;

	return camerabuff;
}

//Determines the length of the frame buffer (image size)
int Camera::frameLength() {
	alt_u8 args[] = { 0x01, 0x00 };
	int len = 0;
	if (!runCommand(GET_FBUF_LEN, args, sizeof(args), 9))
		return len;

	len = camerabuff[5];
	len <<= 8;
	len |= camerabuff[6];
	len <<= 8;
	len |= camerabuff[7];
	len <<= 8;
	len |= camerabuff[8];

	return len;
}

//Sends a frame buffer command to the camera
bool Camera::cameraFrameBuffCtrl(alt_u8 command) {
	alt_u8 args[] = { 0x1, command };
	return runCommand(FBUF_CTRL, args, sizeof(args), 5);
}

//Stops the current frame
bool Camera::takePicture() {
	frameptr = 0;
	return cameraFrameBuffCtrl(STOPCURRENTFRAME);
}

//Resumes the video
bool Camera::resumeVideo() {
	return cameraFrameBuffCtrl(RESUMEFRAME);
}

//Turns the motion detect on or off
bool Camera::setMotionDetect(bool flag) {
	alt_u8 control[] = { UARTMOTION, STARTMONITORING };

	if (!setMotionStatus(MOTIONCONTROL, control, 2))
		return false;

	alt_u8 args[] = { 0x01, flag };

	return runCommand(COMM_MOTION_CTRL, args, sizeof(args), 5);
}

//Determines the current state of the motion detect
bool Camera::getMotionDetect() {
	alt_u8 args[] = { 0x0 };

	if (!runCommand(COMM_MOTION_STATUS, args, 1, 6))
		return false;

	return camerabuff[5];
}

//Determines weather or not motion was detected
bool Camera::motionDetected() {
	if (readResponse(4, 200) != 4)
		return false;

	if (!verifyResponse(COMM_MOTION_DETECTED))
		return false;

	return true;
}

//Sets the motion detect status
bool Camera::setMotionStatus(alt_u8 attribute, alt_u8 * control,
		size_t ctrl_len) {
	alt_u8 args[4];

	args[0] = ctrl_len + 1;
	args[1] = attribute;
	for (size_t i = 0; i < ctrl_len; i++) {
		args[i + 2] = control[i];
	}

	return runCommand(MOTION_CTRL, args, ctrl_len + 2, 5);
}

//Determines the motion detect status
alt_u8 * Camera::getMotionStatus(alt_u8 attribute) {
	alt_u8 args[] = { 0x01, attribute };

	sendCommand(MOTION_STATUS, args, sizeof(args));

	if (readResponse(5, 200) != 5)
		return false;

	if (readResponse(camerabuff[4], 200) != camerabuff[4])
		return false;

	return camerabuff;
}

//Turns the video output on
bool Camera::TVon() {
	alt_u8 args[] = { 0x1, 0x1 };
	return runCommand(TVOUT_CTRL, args, sizeof(args), 5);
}

//Turns the video output off
bool Camera::TVoff() {
	alt_u8 args[] = { 0x1, 0x0 };
	return runCommand(TVOUT_CTRL, args, sizeof(args), 5);
}

//Runs the specified command
bool Camera::runCommand(alt_u8 cmd, alt_u8 *args, alt_u8 argn, alt_u8 resplen,
		bool flush) {
	if (flush)
		readResponse(100, 10);

	sendCommand(cmd, args, argn);
	if (readResponse(resplen, 200) != resplen)
		return false;
	if (!verifyResponse(cmd))
		return false;

	return true;
}

//Sends the specified command
void Camera::sendCommand(alt_u8 cmd, alt_u8 args[], alt_u8 argn) {
#ifdef CAMERA_DEBUG
	printf("\tCommand: 0x%X 0x%X 0x%X ", 0x56, 0, cmd);
#endif

	fprintf(camera_fp, "%c", 0x56);
	fprintf(camera_fp, "%c", 0);
	fprintf(camera_fp, "%c", cmd);

	for (int i = 0; i < argn; i++) {
		fprintf(camera_fp, "%c", args[i]);
#ifdef CAMERA_DEBUG
		printf("0x%X ", args[i]);
#endif
	}

#ifdef CAMERA_DEBUG
	printf("\n");
#endif

}

//Reads the specified number of bytes from the uart unless a timeout occurs
alt_u8 Camera::readResponse(alt_u8 numbytes, int timeout) {
	int counter = 0;
	bufferLen = 0;

#ifdef CAMERA_DEBUG
	printf("\tResponse: ");
#endif

	while ((timeout != counter) && (bufferLen != numbytes)) {
		static alt_u8 c;
		c = fgetc(camera_fp);
		if (ALT_ERRNO == ALTERA_AVALON_UART_STATUS_ROE_MSK) {
			return 0;
		} else if (ALT_ERRNO == EWOULDBLOCK) {
			OSTimeDlyHMSM(0, 0, 0, 10);
			counter++;
			continue;
		}
		counter = 0;
		camerabuff[bufferLen++] = c;

#ifdef CAMERA_DEBUG
		printf("0x%X ", camerabuff[bufferLen-1]);
#endif

	}

#ifdef CAMERA_DEBUG
	printf("\n");
#endif

	return bufferLen;
}

//Verifies the response from the camera
bool Camera::verifyResponse(alt_u8 command) {
	if ((camerabuff[0] != 0x76) || (camerabuff[1] != serialNum)
			|| (camerabuff[2] != command) || (camerabuff[3] != 0x0))
		return false;
	return true;
}

