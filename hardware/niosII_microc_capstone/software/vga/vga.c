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

#define SCREEN_WIDTH        640       /* width in pixels of mode 0x13 */
#define SCREEN_HEIGHT       480       /* height in pixels of mode 0x13 */
#define NUM_COLORS          256       /* number of colors in mode 0x13 */
#define BMP_OFFSET          54


/**************************************************************************
 *  load_bmp                                                              *
 *    Loads a bitmap file into memory.                                    *
 **************************************************************************/

void load_bmp(int file_handle, unsigned char **data_array)
{

  for(times_read = 0; times_read < BMP_OFFSET; times_read++){
    data = alt_up_sd_card_read(file_handle);
  }

  int x, y;
  unsigned char byte_b, byte_g, byte_r;
  /* read the bitmap. Bytes are red in BRG format and then translated into RGB */
  for(y=(SCREEN_HEIGHT-1);y>+ 0;y--){
    for(x=0;x<SCREEN_WIDTH;x++)  {
        byte_b=alt_up_sd_card_read(file_handle);
        byte_g=alt_up_sd_card_read(file_handle);
        byte_r=alt_up_sd_card_read(file_handle);
        
        if((byte_b < 0) || (byte_g < 0) || (byte_r < 0)){
            break;
        }

        byte_b = (unsigned char) byte_b;
        byte_g = (unsigned char) byte_g;
        byte_r = (unsigned char) byte_r;

        data_array[x][y]=((byte_r>>3)<<11)|((byte_g>>2)<<5)|((byte_b>>3)<<0);

    }
  }
}

/**************************************************************************
 *  draw_bitmap                                                           *
 *    Draws a bitmap.                                                     *
 **************************************************************************/

void draw_bitmap(unsigned char **data_array)
{
  for(y = 0; y < SCREEN_HEIGHT; y++){
    for(x = 0; x < SCREEN_WIDTH; x++){
      alt_up_pixel_buffer_dma_draw(vga_buffer, data_array[x + SCREEN_HEIGHT*y],x,y);

    }
  }
}


/* The main function instantiates the pixel buffer and sd card and loads a bitmap from sd card into the buffer */
int main(void)
{
  alt_up_sd_card_dev *sd_card = alt_up_sd_card_open_dev(ALTERA_UP_SD_CARD_AVALON_INTERFACE_0_NAME);

  alt_up_pixel_buffer_dma_dev *vga_buffer = alt_up_pixel_buffer_dma_open_dev(VIDEO_PIXEL_BUFFER_DMA_0_NAME);

  if(vga_buffer == NULL){
	  printf("Could not instantiate VGA buffer");
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
  printf("Reading Data \n");

  unsigned char **data_array = malloc(SCREEN_WIDTH * sizeof(unsigned char*));
  int i;
  for(i = 0; i < SCREEN_WIDTH; i++){
    data_array[i] = malloc(SCREEN_HEIGHT * sizeof(unsigned char));
  }
  
  load_bmp(file_handle, data_array);

  printf("Done Reading File \n");
  //OSTimeDly(1);
  printf("Number of reads: %d\n", times_read);

  alt_up_pixel_buffer_dma_clear_screen(vga_buffer, 0);

  while(1){
    draw_bitmap(data_array);

  }

  free(data_array);
  return 0;
}

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
