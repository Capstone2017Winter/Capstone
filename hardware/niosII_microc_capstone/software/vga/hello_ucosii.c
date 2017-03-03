/*************************************************************************
* Copyright (c) 2004 Altera Corporation, San Jose, California, USA.      *
* All rights reserved. All use of this software and documentation is     *
* subject to the License Agreement located at the end of this file below.*
**************************************************************************
* Description:                                                           *
* The following is a simple hello world program running MicroC/OS-II.The * 
* purpose of the design is to be a very simple application that just     *
* demonstrates MicroC/OS-II running on NIOS II.The design doesn't account*
* for issues such as checking system call return codes. etc.             *
*                                                                        *
* Requirements:                                                          *
*   -Supported Example Hardware Platforms                                *
*     Standard                                                           *
*     Full Featured                                                      *
*     Low Cost                                                           *
*   -Supported Development Boards                                        *
*     Nios II Development Board, Stratix II Edition                      *
*     Nios Development Board, Stratix Professional Edition               *
*     Nios Development Board, Stratix Edition                            *
*     Nios Development Board, Cyclone Edition                            *
*   -System Library Settings                                             *
*     RTOS Type - MicroC/OS-II                                           *
*     Periodic System Timer                                              *
*   -Know Issues                                                         *
*     If this design is run on the ISS, terminal output will take several*
*     minutes per iteration.                                             *
**************************************************************************/



  /**************************************************************************
   * bitmap.c                                                               *
   * written by David Brackeen                                              *
   * http://www.brackeen.com/home/vga/                                      *
   *                                                                        *
   * Tab stops are set to 2.                                                *
   * This program compiles with DJGPP! (www.delorie.com)                    *
   * To compile in DJGPP: gcc bitmap.c -o bitmap.exe                        *
   *                                                                        *
   * This program will only work on DOS- or Windows-based systems with a    *
   * VGA, SuperVGA, or compatible video adapter.                            *
   *                                                                        *
   * Please feel free to copy this source code.                             *
   *                                                                        *
   * DESCRIPTION: This program demostrates drawing bitmaps, including       *
   * transparent bitmaps.                                                   *
   **************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "includes.h"
#include "altera_up_sd_card_avalon_interface.h"
#include "altera_up_avalon_video_pixel_buffer_dma.h"

/* Definition of Task Stacks */
#define   TASK_STACKSIZE       2048
OS_STK    task1_stk[TASK_STACKSIZE];
OS_STK    task2_stk[TASK_STACKSIZE];

/* Definition of Task Priorities */

#define TASK1_PRIORITY      1
#define TASK2_PRIORITY      2

#define VIDEO_INT           0x10      /* the BIOS video interrupt. */
#define SET_MODE            0x00      /* BIOS func to set the video mode. */
#define VGA_256_COLOR_MODE  0x13      /* use to set 256-color mode. */
#define TEXT_MODE           0x03      /* use to set 80x25 text mode. */

#define SCREEN_WIDTH        800       /* width in pixels of mode 0x13 */
#define SCREEN_HEIGHT       480       /* height in pixels of mode 0x13 */
#define NUM_COLORS          256       /* number of colors in mode 0x13 */

typedef unsigned char  byte;
typedef unsigned short word;
typedef unsigned long  dword;

byte *VGA = (byte *)0xA0000;          /* this points to video memory. */
word *my_clock = (word *)0x046C;      /* this points to the 18.2hz system
                                           clock. */

typedef struct tagBITMAP              /* the structure for a bitmap. */
{
  word width;
  word height;
  byte *data;
} BITMAP;

/**************************************************************************
 *  fskip                                                                 *
 *     Skips bytes in a file.                                             *
 **************************************************************************/

void fskip(FILE *fp, int num_bytes)
{
   int i;
   for (i=0; i<num_bytes; i++)
      fgetc(fp);
}

/**************************************************************************
 *  load_bmp                                                              *
 *    Loads a bitmap file into memory.                                    *
 **************************************************************************/

void load_bmp(char *file,BITMAP *b)
{
  FILE *fp;
  long index;
  word num_colors;
  int x;



  /* read in the width and height of the image, and the
     number of colors used; ignore the rest */
  fskip(fp,16);
  fread(&b->width, sizeof(word), 1, fp);
  fskip(fp,2);
  fread(&b->height,sizeof(word), 1, fp);
  fskip(fp,22);
  fread(&num_colors,sizeof(word), 1, fp);
  fskip(fp,6);

  /* assume we are working with an 8-bit file */
  if (num_colors==0) num_colors=256;


  /* try to allocate memory */
  if ((b->data = (byte *) malloc((word)(b->width*b->height))) == NULL)
  {
    fclose(fp);
    printf("Error allocating memory for file %s.\n",file);
    exit(1);
  }

  /* Ignore the palette information for now.
     See palette.c for code to read the palette info. */
  fskip(fp,num_colors*4);



  fclose(fp);
}

/**************************************************************************
 *  draw_bitmap                                                           *
 *    Draws a bitmap.                                                     *
 **************************************************************************/

void draw_bitmap(BITMAP *bmp,int x,int y)
{
  int j;
  word screen_offset = (y<<8)+(y<<6)+x;
  word bitmap_offset = 0;

  for(j=0;j<bmp->height;j++)
  {
    memcpy(&VGA[screen_offset],&bmp->data[bitmap_offset],bmp->width);

    bitmap_offset+=bmp->width;
    screen_offset+=SCREEN_WIDTH;
  }
}


/* The main function creates two task and starts multi-tasking */
int main(void)
{
  alt_up_sd_card_dev *sd_card = alt_up_sd_card_open_dev(ALTERA_UP_SD_CARD_AVALON_INTERFACE_0_NAME);

  alt_up_pixel_buffer_dma_dev *vga_buffer = alt_up_pixel_buffer_dma_open_dev(VIDEO_PIXEL_BUFFER_DMA_0_NAME);

  if(vga_buffer == NULL){
	  printf("WE all DOOMED");
  }

  if(sd_card == NULL){
	  printf("Error instantiating sd card core \n");
  }

  if(alt_up_sd_card_is_FAT16()){
	  printf("Card connected and formatted properly \n");
  }

  short int file_handle = alt_up_sd_card_fopen("test.bmp", false);
  printf("File Handle: %d\n",file_handle );

  short int file_attributes = alt_up_sd_card_get_attributes(file_handle);
  printf("File Attributes: %d\n",file_attributes );
  byte data;
  printf("Reading Data \n");

  int times_read = 0;
  unsigned char data_array[640*480*4];
  //BITMAP bmp;
  for(times_read = 0; times_read < 54; times_read++){
	  data = alt_up_sd_card_read(file_handle);
  }
/*
  word width_buf[sizeof(word)/8];

  for(times_read = 0; times_read < sizeof(word); times_read++){
	  width_buf[times_read%8] = (unsigned char)alt_up_sd_card_read(file_handle);
	  times_read +=8;
  }
  printf(width_buf);
  printf("\n");
  */
  int x, y;
  unsigned char byte_b, byte_g, byte_r;
  /* read the bitmap */
  for(y=0;y<480;y++){
	  for(x=0;x<640;x++)  {
  byte_b=(unsigned char)(alt_up_sd_card_read(file_handle));
  byte_g=(unsigned char)(alt_up_sd_card_read(file_handle));
  byte_r=(unsigned char)(alt_up_sd_card_read(file_handle));
  data_array[x+480*y]=((byte_r>>3)<<11)|((byte_g>>2)<<5)|((byte_b>>3)<<0);

  }
  }
/*
  while(data >= 0){
	  //printf("Reading file data: %d \n", data);
	  data = (byte)alt_up_sd_card_read(file_handle);
	  if(data >= 0){
		  data_array[times_read] = data;
	  }
	  times_read++;
  }
  */
  printf("Done Reading File \n");
  //OSTimeDly(1);
  printf("Number of reads: %d\n", times_read);


	  //printf("Reading file data: %d \n", data);
	 // data = ;


  alt_up_pixel_buffer_dma_clear_screen(vga_buffer, 0);

  while(1){
  for(y = 0; y < 480; y++){
	  for(x = 0; x < 640; x++){
		  alt_up_pixel_buffer_dma_draw(vga_buffer, data_array[x + 480*y],x,y);


	  }
  }
  //printf("Finished displaying data\n");
  }
}





  /**************************************************************************
   *  draw_transparent_bitmap                                               *
   *    Draws a transparent bitmap.                                         *
   **************************************************************************/
/*
  void draw_transparent_bitmap(BITMAP *bmp,int x,int y)
  {
    int i,j;
    word screen_offset = (y<<8)+(y<<6);
    word bitmap_offset = 0;
    byte data;

    for(j=0;j<bmp->height;j++)
    {
      for(i=0;i<bmp->width;i++,bitmap_offset++)
      {
        data = bmp->data[bitmap_offset];
        if (data) VGA[screen_offset+x+i] = data;
      }
      screen_offset+=SCREEN_WIDTH;
    }
  }
*/
  /**************************************************************************
   *  wait                                                                  *
   *    Wait for a specified number of clock ticks (18hz).                  *
   **************************************************************************/



  /**************************************************************************
   *  Main                                                                  *
   *    Draws opaque and transparent bitmaps                                *
   **************************************************************************/
/*
  void main()
  {
    int i,x,y;
    BITMAP bmp;

    if (__djgpp_nearptr_enable() == 0)
    {
      printf("Could get access to first 640K of memory.\n");
      exit(-1);
    }

    VGA+=__djgpp_conventional_base;
    my_clock = (void *)my_clock + __djgpp_conventional_base;

    load_bmp("rocket.bmp",&bmp);        /* open the file */
/*
    set_mode(VGA_256_COLOR_MODE);       /* set the video mode. */

    /* draw the background */
    /*
    for(i=0;i<200;i++)
      memset(&VGA[320*i],i,SCREEN_WIDTH);

    wait(25);

    /* draw a tiled bitmap pattern on the left */
    /*
  	for(y=0;y<=SCREEN_HEIGHT-bmp.height;y+=bmp.height)
      for(x=0;x<=(SCREEN_WIDTH)/2-bmp.width;x+=bmp.width)
        draw_bitmap(&bmp,x,y);

    wait(25);

    /* draw a tiled transparent bitmap pattern on the right */
    /*
  	for(y=0;y<=SCREEN_HEIGHT-bmp.height;y+=bmp.height)
      for(x=SCREEN_WIDTH-bmp.width;x>=SCREEN_WIDTH/2;x-=bmp.width)
        draw_transparent_bitmap(&bmp,x,y);

    wait(100);

    free(bmp.data);                     /* free up memory used */
  	/*
    set_mode(TEXT_MODE);                /* set the video mode back to
                                           text mode. */
  /*
  __djgpp_nearptr_disable();

    return;

  //OSStart();
  return 0;
}
*/
/******************************************************************************
*                                                                             *
* License Agreement                                                           *
*                                                                             *
* Copyright (c) 2004 Altera Corporation, San Jose, California, USA.           *
* All rights reserved.                                                        *
*                                                                             *
* Permission is hereby granted, free of charge, to any person obtaining a     *
* copy of this software and associated documentation files (the "Software"),  *
* to deal in the Software without restriction, including without limitation   *
* the rights to use, copy, modify, merge, publish, distribute, sublicense,    *
* and/or sell copies of the Software, and to permit persons to whom the       *
* Software is furnished to do so, subject to the following conditions:        *
*                                                                             *
* The above copyright notice and this permission notice shall be included in  *
* all copies or substantial portions of the Software.                         *
*                                                                             *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE *
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     *
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER         *
* DEALINGS IN THE SOFTWARE.                                                   *
*                                                                             *
* This agreement shall be governed in all respects by the laws of the State   *
* of California and by the laws of the United States of America.              *
* Altera does not recommend, suggest or require that this reference design    *
* file be used in conjunction or combination with any other product.          *
******************************************************************************/
