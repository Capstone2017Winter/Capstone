/******************************************************************************
* File: StereoCamera.cpp
* Author: Leo Nickerson, Timo Hohn
* Client: ECE 492 project
* Version: 0.50
* Last Modified: 09/04/2013
*
* Library for the TTL JPEG Camera (VC0706 chipset)
*
* Cameras use serial interface to communicate, 2 pins are required
*
* This library was written for interfacing the cameras with Altera DE2 board
* running a NIOS II processor
*
* Consult the header file for a basic description of the functions
*
******************************************************************************/

#include "StereoCamera.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "io.h"
#ifdef __cplusplus
}
#endif
#include <stdio.h>
#include <system.h>

StereoCamera::StereoCamera(const char * path0, const char * path1){
#ifdef SCAM_LED_DEBUG
	IOWR(LED_PIO_BASE, 0, 0x00);
#endif

	if(path0 != NULL){
		camera0 = Camera();
		camera0.begin(path0);
		while(!camera0.setImageSize(RES_640x480));
	}

#ifdef SCAM_LED_DEBUG
	IOWR(LED_PIO_BASE, 0, 0x0F);
#endif

	if(path1 != NULL){
		camera1 = Camera();
		camera1.begin(path1);
		while(!camera1.setImageSize(RES_640x480));
	}

#ifdef SCAM_LED_DEBUG
	IOWR(LED_PIO_BASE, 0, 0xFF);
#endif

	READ_LEN = 32;
}

bool StereoCamera::reset(int cameraNum){
	switch(cameraNum){
	case 0:
		return camera0.reset();
		break;
	case 1:
		return camera1.reset();
		break;
	default:
		return camera0.reset() && camera1.reset();
	}
}

void StereoCamera::setResLrg(int cameraNum){
	switch(cameraNum){
	case 0:
		while(!camera0.setImageSize(RES_640x480));
		break;
	case 1:
		while(!camera1.setImageSize(RES_640x480));
		break;
	default:
		while(!camera0.setImageSize(RES_640x480));
		while(!camera1.setImageSize(RES_640x480));
	}
}


bool StereoCamera::takePicture(int cameraNum){
	switch(cameraNum){
	case 0:
		return camera0.takePicture();
		break;
	case 1:
		return camera1.takePicture();
		break;
	default:
		return camera0.takePicture() && camera1.takePicture();
	}
}

bool StereoCamera::resumeVideo(int cameraNum){
	switch(cameraNum){
	case 0:
		return camera0.resumeVideo();
		break;
	case 1:
		return camera1.resumeVideo();
		break;
	default:
		return camera0.resumeVideo() && camera1.resumeVideo();
	}
}

void StereoCamera::readPicture(alt_64 addr0, alt_64 addr1){
	if(addr0 != -1){
		alt_u32 jpeglen = 0;
		alt_u32 addr_offset = 0;

		while(jpeglen == 0)
			jpeglen = camera0.frameLength();

#ifdef SCAM_DEBUG
		int counter=0;
		printf("taking picture 0 ");
#endif

		while(jpeglen > 0){
			unsigned char bytestoread = (jpeglen < READ_LEN) ? jpeglen : READ_LEN;

			alt_u8 * picture = NULL;
			while(picture == NULL)
				picture = camera0.readPicture(bytestoread);

#ifdef SCAM_DEBUG
			if(counter++%32==0)
				printf(".");
#endif

			for(int i = 0; i<bytestoread; i++){
				IOWR_8DIRECT((addr0+addr_offset), i, picture[i]);
			}
			jpeglen -= bytestoread;
			addr_offset += bytestoread;
		}

#ifdef SCAM_DEBUG
		printf("\tDone!\n");
#endif
#ifdef SCAM_LED_DEBUG
		IOWR(LED_PIO_BASE, 0, 0x0F);
#endif
	}

	if(addr1 != -1){
		alt_u32 jpeglen = 0;
		alt_u32 addr_offset = 0;
		while(jpeglen == 0)
			jpeglen = camera1.frameLength();

#ifdef SCAM_DEBUG
		int counter=0;
		printf("taking picture 1 ");
#endif

		while(jpeglen > 0){
			unsigned char bytestoread = (jpeglen < READ_LEN) ? jpeglen : READ_LEN;

			alt_u8 * picture = NULL;
			while(picture == NULL)
				picture = camera1.readPicture(bytestoread);

#ifdef SCAM_DEBUG
			if(counter++%32==0)
				printf(".");
#endif
			for(int i = 0; i<bytestoread; i++){
				IOWR_8DIRECT((addr1+addr_offset), i, picture[i]);
			}
			jpeglen -= bytestoread;
			addr_offset += bytestoread;
		}

#ifdef SCAM_DEBUG
		printf("\tDone!\n");
#endif
#ifdef SCAM_LED_DEBUG
		IOWR(LED_PIO_BASE, 0, 0xFF);
#endif
	}
}


int StereoCamera::getFrameLength(int cameraNum) {
	if (cameraNum==0) {
		return camera0.frameLength();
	} else if (cameraNum==1) {
		return camera1.frameLength();
	} else {
		return 0;
	}
}

alt_u8 StereoCamera::setReadLen(alt_u8 len){
	if(len>64)
		len = 64;
	else if(len<16)
		len = 16;

	return (READ_LEN = len);
}
