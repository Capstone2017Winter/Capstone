//HTTP camera commands

#ifdef __cplusplus
extern "C" {
#endif

#include "cam_control.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/param.h>
#include <sys/fcntl.h>
#include "sys/alt_alarm.h"
#include "alt_types.h"
#include "http.h"
#include "web_server.h"
#include <io.h>

#include "StereoCamera.h"
#include "system.h"

OS_EVENT *ImageDataSem;
int leftLength;
int rightLength;


void cam_control_task(){
	StereoCamera scam = StereoCamera(UART_0_NAME, UART_1_NAME);
	INT8U err;
	ImageDataSem = OSSemCreate(1);


	while(true){
		OSSemPend(takePicSem, 0, &err);
		if(err == OS_NO_ERR){
			scam.setReadLen(IORD(SWITCH_BASE, 0));
			scam.reset();
			scam.setResLrg();
			scam.takePicture();

			OSSemPend(ImageDataSem, 0, &err);
			if(err ==  OS_NO_ERR){
				rightLength = scam.getFrameLength(0);
				leftLength = scam.getFrameLength(1);
			}
			OSSemPost(ImageDataSem);

			IOWR(LED_PIO_BASE, 0, 0x00);
			OSTimeDlyHMSM(0, 0, 1, 0);
			scam.readPicture(RIGHT_ADDRESS, LEFT_ADDRESS);

			OSSemPost(picReadySem);
		}
	}
}

int getImageLength(int image){
	int length;
	INT8U err;
	OSSemPend(ImageDataSem, 0, &err);
	if(err ==  OS_NO_ERR){
		if(image == 0)
			length = leftLength;
		else
			length = rightLength;
	}
	OSSemPost(ImageDataSem);

	return length;
}

int takePictures(){
	INT8U err = OS_NO_ERR;
	OS_SEM_DATA ready;
	OSSemQuery(picReadySem, &ready);
	if(ready.OSCnt > 0)
		OSSemPend(picReadySem, 0, &err);

	if(err == OS_NO_ERR)
		err = OSSemPost(takePicSem);
	if(err == OS_NO_ERR)
		return 0;
	else
		return -1;
}

int isPictureReady(){
	OS_SEM_DATA ready;
	OSSemQuery(picReadySem, &ready);
	if(ready.OSCnt > 0)
		return 1;
	else
		return 0;
}

#ifdef __cplusplus
}
#endif

