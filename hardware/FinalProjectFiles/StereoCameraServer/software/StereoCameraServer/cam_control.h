#ifndef __CAM_CONTROL_H__
#define __CAM_CONTROL_H__

#include "includes.h"

//Semaphores for determining when to take pictures and when reading pictures is done
OS_EVENT *takePicSem;
OS_EVENT *picReadySem;

//Addresses of where the camera is to write data to
#define LEFT_ADDRESS	(SRAM_IF_0_BASE)
#define RIGHT_ADDRESS	(SRAM_IF_0_BASE+SRAM_IF_0_SPAN/4)

//camera control functions
int takePictures();
int pictureReady();
int getImageLength(int image);
int isPictureReady();

#endif
