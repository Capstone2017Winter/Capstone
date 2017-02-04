/******************************************************************************
* File: Camera.h
* Author: Leo Nickerson, Timo Hohn
* Client: ECE 492 project
* Version: 1.00
* Last Modified: 09/04/2013
*
* Library for interfacing a TTL JPEG Camera (VC0706 chipset)
*
* Cameras use serial interface to communicate, 2 pins are required: a TX and RX
*
* This library was written for interfacing the cameras with Altera DE2 board
* running a NIOS II processor
*
* This library was written with the supplied Adafruit code and PTC08 serial
* camera module as reference.  It is important to not that this module does
* not support all of the commands available to the camera, these unsupported
* commands are described in the camera manual and in the Adafruit code.
*
* This module was designed to be used with the Altera DE2 board using a
* modified version of the Altera Avalon UART driver as well as using uC OS II.
* The code was developed in NIOS II Eclipse IDE v10.1.  In order for the module
* to perform as expected the uC OS II must be properly implemented so that the
* function OSTimeDlyHMSM can perform properly.  It is of note that while it is
* possible to have two cameras connected to the board at one time they can
* interfere with each others communication if each instance of the camera is
* used in separate tasks.  As such all occurrences of the Camera class should
* be accessed in/through a single task.  The modifications to the the UART
* driver are described in the read me associated with this module.
*
* Note: Not all functions present have been fully tested and verified for
* functionality
******************************************************************************/

#ifndef CAMERA_H
#define CAMERA_H

#include "alt_types.h"
#include <stdio.h>

//If CAMERA_DEBUG is defined debug statements will be printed to stdout
#define CAMERA_DEBUGN

//Camera commands
#define RESET  0x26
#define GEN_VERSION 0x11
#define READ_DATA 0x30
#define WRITE_DATA 0x31
#define READ_FBUF 0x32
#define GET_FBUF_LEN 0x34
#define FBUF_CTRL 0x36
#define DOWNSIZE_CTRL 0x54
#define DOWNSIZE_STATUS 0x55
#define READ_DATA 0x30
#define WRITE_DATA 0x31
#define COMM_MOTION_CTRL 0x37
#define COMM_MOTION_STATUS 0x38
#define COMM_MOTION_DETECTED 0x39
#define MOTION_CTRL 0x42
#define MOTION_STATUS 0x43
#define TVOUT_CTRL 0x44

//Image size options
#define RES_640x480 0x00
#define RES_320x240 0x11
#define RES_160x120 0x22

//FBUF_CTRL commands
#define STOPCURRENTFRAME 0x0
#define STOPNEXTFRAME 0x1
#define RESUMEFRAME 0x2
#define STEPFRAME 0x3

//MOTION_CTRL motion attributes
#define MOTIONCONTROL 0x00
#define ALARM_ATTRIBUTE 0x01
#define ALARM_ENABLE_CTRL 0x02
#define ALARM_CTRL 0x03

//MOTION_CTRL control items for the MOTIONCONTROL motion attribute
#define GPIOMOTION 0x00
#define UARTMOTION 0x01
#define STOPMONITORING 0x00
#define STARTMONITORING 0x01

//MOTION_CTRL control items for the ALARM_ATTRIBUTE motion attribute
//STOP_ALARM/ALARM_ALWAYS can be or'd together with LOW_LEVEL/HIGH_LEVEL
#define STOP_ALARM 0x00
#define ALARM_ALWAYS 0x01
#define LOW_LEVEL 0x00
#define HIGH_LEVEL 0x02

//MOTION_CTRL control items for the ALARM_ENABLE_CTRL motion attribute
#define FORBID_ALARM 0x00
#define ENABLE_ALARM 0x01

//MOTION_CTRL control items for the ALARM_CTRL motion attribute
#define STOP_ALARM 0x00
#define START_ALARM 0x01

//Parameters for the camera control size of the camera buffer and 
#define CAMERABUFFSIZE 100
#define CAMERADELAY 10


/******************************************************************************
* Camera Class:
*   Used as a wrapper class for interacting with ardiono VC0706 camera
*   the are described below:
*
* Camera() - Constructor, sets the values for bufferLen, serialNum, & frameptr
*	
* begin(const char * path)
* Camera Command: N/A
* Description: Connects to the camera through the	specified uart file path and
*   uart address base then calls reset
* Parameters: path - path to the uart for writing to/reading
*   from the camera
* Return: returns true if the camera has been properly started up
*
* getVersion()
* Camera Command: GEN_VERSION
* Description: Sends the request to the camera to get it's firmware version,
*   the camera returns the version of the camera as a C string
* Paremeters: N/A
* Return: returns the camerabuffer pointer which contains the C string with
*   the version of the camera.  On failure returns null.
*
* reset()
* Camera Command: SYSTEM_RESET
* Description: Sends the reset command to the camera causing it to return to
*   default settings, 10 ms after receiving the confirmation from the camera
*   the camera sends back additional information about the camera
* Parameters: N/A
* Return: returns true if the camera has been reset properly
*
* getImageSize()
* Camera Command: READ_DATA
* Description: Reads from the I2C EEPROM register to determine the resolution
*   of the images being taken
* Parameters: N/A
* Return: returns the value in the register or -1 on error, valid values are:
* 	0x00 - for 640x480 resolution
*   0x11 - for 320x240 resolution
*   0x22 - for 160x140 resolution
*
* setImageSize(alt_u8 res)
* Camera Command: WRITE_DATA
* Description: Writes the resolution value into the I2C EEPROM register to set
*   the resolution of the images being taken
* Parameters: res
* Return: returns true if the resolution has been set properly
*
* readPicture(alt_u8 n)
* Camera Command: READ_FBUF
* Description: Reads the specified number of bytes from the frame buffer of the
*   camera starting from the last byte read from the camera.  The bytes	are
*   stored in the buffer of the camera module.
* Parameters: n - the number of bytes to be read from the camera, maximum of 32
* Return: The address the camera buffer, only the number of bytes specified to
*   read from the camera are valid.  On failure returns null.
*
* frameLength()
* Camera Command: GET_FBUF_LEN
* Description: Gets the length of the image in the frame buffer of the camera
* Parameters: N/A
* Return: Returns the length of the image in the cameras frame buffer in bytes
*   on success.  This function returns 0 on failure.
*
* cameraFrameBuffCtrl(alt_u8 command)
* Camera Command: FBUF_CTRL
* Description: Sends frame buffer control command to the camera causing the
*   camera to take the specified action
* Parameters: command - the action to be taken by the camera there are four
*   valid commands available:
*     0 - STOPCURRENTFRAME: stops the video at the current frame
*     1 - STOPNEXTFRAME: stops the video at the next frame
*     2 - RESUMEFRAME: resumes the video
*     3 - STEPFRAME: advances frame to the current frame
* Return: Returns true if the command was executed properly
*
* takePicture()
* Camera Command: N/A
* Description: Calls the cameraFrameBuffCtrl command to stop the camera at the
*   current frame
* Parameters: N/A
* Return: Returns the result of the cameraFrameBuffCtrl function call
*
* resumeVideo()
* Camera Command: N/A
* Description: Calls the cameraFrameBuffCtrl command to resume the video
* Parameters: N/A
* Return: Returns the result of the cameraFrameBuffCtrl function call
*
* setMotionDetect(bool flag)
* Camera Command: COMM_MOTION_CTRL
* Description: Sets the motion detect on or off in the communication interface
*   If the motion control is turned on then the camera will send the
*   COMM_MOTION_DETECTED command when motion is detected
*   If the motion control is turned off then no command will be sent
* Parameters:	flag - weather or not motion detect is turned on true = on
*   false = off
* Return: Returns true if the motion detect has been turned on
*
* getMotionDetect()
* Camera Command: COMM_MOTION_STATUS
* Description: Determines the motion monitering status of the camera
* Parameters: N/A
* Return: Returns the motion monitering status of the camera true = on
*   false = off
*
* motionStatus()
* Camera Command: N/A
* Description: Checks to see if the COMM_MOTION_DETECTED command has been
*   received from the camera to determine weather or not motion has been
*   detected
* Parameters: N/A
* Return: Returns true if motion has been detected by the camera
*
* setMotionStatus(alt_u8 attribute, alt_u8 * control, size_t ctrl_len)
* Camera Command: MOTION_CTRL
* Description: Sets up the motion detect control for the camera
*   Allowing for multiple attributes in regards to the motion control and
*   alarm-output
*   Note: this function has not been fully tested consult the camera manual for
*   further clarification on the functionality of this command
* Parameters: attribute - the motion attribute being adjusted valid motion
*   attributes available to be adjusted:
*     0 - MOTIONCONTROL: enable/disable motion monitoring
*     1 - ALARM_ATTRIBUTE: when alarm goes off and weather high or low level
*     2 - ALARM_ENABLE_CTRL: weather or not alarm output is aloud
*     3 - ALARM_CTRL: starting/stopping the alarm output
*   control - the control item, it has variable byte length, the contents of
*     the parameter depends on the attribute parameter, valid control items
*     for each of the motion attributes is as follows:
*     MOTIONCONTROL Control Items:
*       1st Byte: motion monitoring either via GPIO or UART
*       2nd Byte: stop or start motion monitoring
*     ALARM_ATTRIBUTE Control Items:
*       1st Byte: bit0: alarm type, bit1: electral level
*       2nd & 3rd Byte: alarm time, lower byte follows higher byte, unit 10ms
*     ALARM_ENABLE_CTRL Control Items:
*       1st Byte: forbid or enable alarm output
*     ALARM_CTRL Control Items:
*       1st Byte: stop or start alarm output
*   ctrl_len - the number of bytes in the control item parameter
* Return: Returns weather or not the command was executed properly
*
* getMotionStatus(alt_u8 attribute)
* Camera Command: MOTION_STATUS
* Description: Gets the status of the control items for the given motion
*   attribute
* Parameters: attribute - the motion attribute whose status is being checked
* Return: Returns the camerabuff pointer which contains the control items of
*   the specified motion attribute
*   The control items length is the same as their length for setting the
*   motion attributes
*
* TVon()
* Camera Command: TV_OUT_CTRL
* Description: Turns the output of the NTSC video on
* Parameters: N/A
* Return: Returns true if the command was executed successfully
*
* TVoff()
* Camera Command: TV_OUT_CTRL
* Description: Turns the output of the NTSC video off
* Parameters: N/A
* Return: Returns true if the command was executed successfully
*
* camerabuff - buffer for holding the response read from the camera
* bufferLen - the current length of the data held in the camera buffer
* serialNum - the serial number assigned to the camera
* frameptr - indicates the byte number of the next byte to read from the camera
*   frame buffer
* camera_fp - a FILE pointer for writing to/reading from the uart connected to
*   the camera
*
* runCommand(alt_u8 cmd, alt_u8 args[], alt_u8 argn,
*   alt_u8 resplen, bool flush)
* Camera Command: N/A
* Description: Sends the specified command to the camera with the specified
*   arguments, reads the response from the camera, and checks to ensure that
*   the response received is valid.  This function also flushes the received
*   data from the uarts buffer unless specified otherwise.
* Parameters: cmd - the command to be sent
*   args - the arguments for the command
*   argn - the number of arguments ie. the length of args
*   resplen - the expected length of the response from the camera
*   flush - weather or not to flush the received data from the uart
*     - defaults to true
* Return: Returns true if the response read was as expected false otherwise
*
* sendCommand(alt_u8, int timeout)
* Camera Command: N/A
* Description: Tries to send the specified command to the camera
*   This function does not check to unsure that the command was properly sent
* Parameters: cmd - the command to be sent
*   args - the arguments for the command being sent
*   argn - the number of arguments being sent
*   Return: N/A
*
* readResponse(alt_u8 numbytes, int timeout)
* Camera Command: N/A
* Description: Tries to read the specified number of bytes from the uart
*   If there are not bytes available to be read the function times out
*   This function resets the bufferLen everytime overwriting any unread data
* Parameters: numbyts - the number of bytes to read
*   timeout - the number of times to try and read a byte before timing out
* Return: Returns the number of bytes successfully read
*
* verifyResponse(alt_u8 command)
* Camera Command: N/A
* Description: Checks the last set of data received from the camera for erros
*   Does this by checking that the response receive is in the proper format
*   For more information on expected response from the camera consult the
*   camera manual
* Parameters: command - the command whose response is being verified
* Return: returns true if the response is free of errors false otherwise
*
******************************************************************************/
class Camera {
 public:
  Camera();
  bool begin(const char * path);

  alt_u8 *getVersion();
  bool reset();

  alt_u8 getImageSize();
  bool setImageSize(alt_u8 res);

  alt_u8 *readPicture(alt_u8 n);
  int frameLength();
  bool cameraFrameBuffCtrl(alt_u8 command);
  bool takePicture();
  bool resumeVideo();

  bool setMotionDetect(bool flag);
  bool getMotionDetect();
  bool motionDetected();
  bool setMotionStatus(alt_u8 attribute, alt_u8 * control, size_t ctrl_len);
  alt_u8 * getMotionStatus(alt_u8 attribute);

  bool TVon();
  bool TVoff();

 private:
  alt_u8 camerabuff[CAMERABUFFSIZE+1];
  int bufferLen;
  alt_u8 serialNum;
  alt_u32 frameptr;
  FILE* camera_fp;

  bool runCommand(alt_u8 cmd, alt_u8 args[], alt_u8 argn, alt_u8 resplen,
                  bool flush=true);
  void sendCommand(alt_u8 cmd, alt_u8 args[], alt_u8 argn);
  alt_u8 readResponse(alt_u8 numbytes, int timeout);
  bool verifyResponse(alt_u8 command);
};

#endif
