/******************************************************************************
* Copyright (c) 2006 Altera Corporation, San Jose, California, USA.           *
* All rights reserved. All use of this software and documentation is          *
* subject to the License Agreement located at the end of this file below.     *
*******************************************************************************
*                                                                             *
* Modified to work with Interniche (week of 9/22/06) - BjR                    *
*                                                                             *
* This is an example webserver using NicheStack on the MicroC/OS-II RTOS.     *
* It is in no way a complete implementation of a webserver, it is enough to   *
* serve up our demo pages and control a few board elements and that's it.     *
*                                                                             *
* This example uses the sockets interface. A good introduction to sockets     *
* programming is the book Unix Network Programming by Richard Stevens.        *
*                                                                             *
* Please refer to file ReadMe.txt for notes on this software example.         *
******************************************************************************
* NicheStack TCP/IP stack initialization and Operating System Start in main()
* for this example. 
* 
* This example demonstrates the use of MicroC/OS-II running on NIOS II.       
* In addition it is to serve as a good starting point for designs using       
* MicroC/OS-II and Altera NicheStack TCP/IP Stack - NIOS II Edition.                                                                                           
*      
* Please refer to the Altera NicheStack Tutorial documentation for details on 
* this software example, as well as details on how to configure the NicheStack
* TCP/IP networking stack and MicroC/OS-II Real-Time Operating System.  
*/

/* MicroC/OS-II definitions */
#include "includes.h"

#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include "system.h"
#include "altera_avalon_pio_regs.h"
#include <unistd.h>

/* Web Server definitions */
#include "alt_error_handler.h"
#include "web_server.h"

/* Nichestack definitions */
#include "ipport.h"
#include "libport.h"
#include "osport.h"
#include "tcpport.h"
#include "net.h"

#ifndef ALT_INICHE
  #error This Web Server requires the Interniche IP Stack Software Component.
#endif

#ifndef __ucosii__
  #error This Web Server requires the UCOS II IP Software Component.
#endif

#ifndef  RO_ZIPFS
  #error This Web Server requires the Altera Read only Zip file system.
#endif

#ifdef LCD_DISPLAY_NAME  
FILE* lcdDevice;
#endif /* LCD_DISPLAY_NAME */

extern int current_flash_block;

extern void WSTask();
static void WSCreateTasks();

/* NicheStack network structure. */
extern struct net netstatic[STATIC_NETS];


/* Declarations for creating a task with TK_NEWTASK.  
 * All tasks which use NicheStack (those that use sockets) must be created this way.
 * TK_OBJECT macro creates the static task object used by NicheStack during operation.
 * TK_ENTRY macro corresponds to the entry point, or defined function name, of the task.
 * inet_taskinfo is the structure used by TK_NEWTASK to create the task.
 */

TK_OBJECT(to_wstask);
TK_ENTRY(WSTask);

struct inet_taskinfo wstask = {
      &to_wstask,
      "web server",
      WSTask,
      HTTP_PRIO,
      APP_STACK_SIZE,
};

/* Function which displays the obtained (or assigned) IP
 * Address on the LCD Display.
 */
#ifdef LCD_DISPLAY_NAME
void lcd_ip_addr()
{
  /* Declare local ipaddr variable. */
  ip_addr* ipaddr;
  
  /* Assign ipaddr to the network interface's IP Address. 
   * NOTE:  This code assumes that only a single network
   * interface exists
   */
  ipaddr = &nets[0]->n_ipaddr;
  
  /* Display the IP Address (initially) on the LCD Display. */
  lcdDevice = fopen( "/dev/lcd_display", "w" ); 
  fprintf(lcdDevice, "\nIP Address\n%d.%d.%d.%d\n",
        ip4_addr1(*ipaddr),
        ip4_addr2(*ipaddr),
        ip4_addr3(*ipaddr),
        ip4_addr4(*ipaddr));
  fclose( lcdDevice );
}
#endif

/* Function which resets the system.  Initiated by sending a string to the "LCD"
 * or through the Reset web form.
 */
#ifdef RECONFIG_REQUEST_PIO_NAME 
void trigger_reset()
{
  /* Drive a 0 out to the configuration PLD reconfig_request pin. */
  IOWR_ALTERA_AVALON_PIO_DATA( RECONFIG_REQUEST_PIO_BASE, 0x0 );
  /* Set BIDIR PIO to drive out. */
  IOWR_ALTERA_AVALON_PIO_DIRECTION( RECONFIG_REQUEST_PIO_BASE, 1 );
  usleep ( 1000000 );
  /* Drive out a 1....probably won't reach this point!!! */
  IOWR_ALTERA_AVALON_PIO_DATA( RECONFIG_REQUEST_PIO_BASE, 0x1 );
}
#endif

/* WSInitialTask will initialize the NichStack TCP/IP stack and then initialize
 * the rest of the web server example tasks.
 */

void WSInitialTask(void* pdata)
{
  INT8U error_code = OS_NO_ERR;

  /*
  * Initialize Altera NicheStack TCP/IP Stack - Nios II Edition specific code.
  * NicheStack is initialized from a task, so that RTOS will have started, and 
  * I/O drivers are available.  Two tasks are created:
  *    "Inet main"  task with priority 2
  *    "clock tick" task with priority 3
  */   
  alt_iniche_init();
  /* Start the Iniche-specific network tasks and initialize the network
   * devices.
   */
  netmain(); 
  /* Wait for the network stack to be ready before proceeding. */
  while (!iniche_net_ready)
    TK_SLEEP(1);

  /* Create the main network task.  In this case, a web server. */
  TK_NEWTASK(&wstask);

  /* Application specific code starts here... */
  
  /*Create Tasks*/
  WSCreateTasks();
  printf("\nWeb Server starting up\n");
  /* Application specific code ends here. */

  /* Display the IP Address on the LCD Display. */
  #ifdef LCD_DISPLAY_NAME
  lcd_ip_addr();
  #endif
  /*This task deletes itself, since there's no reason to keep it around, once
   *it's complete.
   */
  error_code = OSTaskDel(OS_PRIO_SELF);
  alt_uCOSIIErrorHandler(error_code, 0);

  while(1); /*Correct Program Flow should not reach here.*/
}
/*
 * A MicroC/OS-II message box will be used to communicate between telnet
 * and board LED control tasks.
 */

//static void board_control_init();
void LED_task(void* pdata);
void SSD_task(void* pdata);
#ifdef SEVEN_SEG_PIO_NAME
static void sevenseg_set_hex(alt_u8 hex);
#endif
void board_control_task(void *pdata);

/* Definition of Task Stacks for tasks not using networking.*/


OS_STK    WSInitialTaskStk[TASK_STACKSIZE];
OS_STK    LEDTaskStk[TASK_STACKSIZE];
OS_STK    SSDTaskStk[TASK_STACKSIZE];
OS_STK    BCTaskStk[TASK_STACKSIZE];


/*
 * Mailbox to control board features 
 * 
 */
OS_EVENT *board_control_mbox;

int main (int argc, char* argv[], char* envp[])
{
  /* Initialize the current flash block, for flash programming. */
  
  current_flash_block = -1;
  
  INT8U error_code;

  /* Clear the RTOS timer */
  OSTimeSet(0);
  
  /* WSInitialTask will initialize the NicheStack TCP/IP Stack and then 
   * initialize the rest of the web server's tasks.
   */ 

  error_code = OSTaskCreateExt(WSInitialTask,
                             NULL,
                             (void *)&WSInitialTaskStk[TASK_STACKSIZE-1],
                             WS_INITIAL_TASK_PRIO,
                             WS_INITIAL_TASK_PRIO,
                             WSInitialTaskStk,
                             TASK_STACKSIZE,
                             NULL,
                             0);
  alt_uCOSIIErrorHandler(error_code, 0);


  /*
   * As with all MicroC/OS-II designs, once the initial thread(s) and 
   * associated RTOS resources are declared, we start the RTOS. That's it!
   */
  OSStart();
  
  while(1); /* Correct Program Flow never gets here. */

  return -1;
}

static void WSCreateTasks()
{
  INT8U error_code = OS_NO_ERR;
  
  /* Start LED Task. */
  
  error_code = OSTaskCreateExt(LED_task,
                             NULL,
                             (void *)&LEDTaskStk[TASK_STACKSIZE-1],
                             LED_PRIO,
                             LED_PRIO,
                             LEDTaskStk,
                             TASK_STACKSIZE,
                             NULL,
                             0);
  alt_uCOSIIErrorHandler(error_code, 0);
  
  /* Start SSD Task. */
  #ifdef SEVEN_SEG_PIO_NAME
  error_code = OSTaskCreateExt(SSD_task,
                             NULL,
                             (void *)&SSDTaskStk[TASK_STACKSIZE-1],
                             SSD_PRIO,
                             SSD_PRIO,
                             SSDTaskStk,
                             TASK_STACKSIZE,
                             NULL,
                             0);
  alt_uCOSIIErrorHandler(error_code, 0);
  #endif
  
  /* Start Board Control Task. */
  
  error_code = OSTaskCreateExt(board_control_task,
                             NULL,
                             (void *)&BCTaskStk[TASK_STACKSIZE-1],
                             BOARD_PRIO,
                             BOARD_PRIO,
                             BCTaskStk,
                             TASK_STACKSIZE,
                             NULL,
                             0);
  alt_uCOSIIErrorHandler(error_code, 0);
  
  /* Suspend both the LED and SSD tasks on start. */
  
  OSTaskSuspend(LED_PRIO);
  OSTaskSuspend(SSD_PRIO);

  /* The web server task is started by the Interniche stack, as the "main" network servicing task. */
  
}

#ifdef LCD_DISPLAY_NAME
void lcd_output_text( char text[20] )
{
  /* If the incoming string is "ip_address" (case insensitive)
   * output the IP Address.  Otherwise, dump whatever's passed
   * into the text[] array.
   */
  if ( stricmp( text, "ip_address" ) == 0 )
  {
    lcd_ip_addr();
  }
#ifdef RECONFIG_REQUEST_PIO_NAME
  else if ( stricmp( text, "reset" ) == 0 )
  {
    trigger_reset();
  }
#endif
  else
  {
    lcdDevice = fopen( "/dev/lcd_display", "w" ); 
    fprintf(lcdDevice, "\n\n%s", text);
    fclose( lcdDevice );
  }
}
#endif

void board_control_task(void *pdata)
{
  INT8U error_code = OS_NO_ERR;
  board_control_mbox = OSMboxCreate((void *)NULL);

  struct http_form_data* board_control_mbox_contents;
  
  while(1)
  {
      board_control_mbox_contents = (void*)OSMboxPend(board_control_mbox, 0, &error_code);
      
      if (board_control_mbox_contents->LED_ON)
      {
        OSTaskResume(LED_PRIO);
      }
      else
      {
        /* Suspend the task and clear the LED. */
        OSTaskSuspend(LED_PRIO);
        IOWR_ALTERA_AVALON_PIO_DATA( LED_PIO_BASE, 0 );
      }
      
      if (board_control_mbox_contents->SSD_ON)
      {
        OSTaskResume(SSD_PRIO);
      }
      else
      {
        /* Suspend the task and set SSD to all zeros. */
        OSTaskSuspend(SSD_PRIO);
		#ifdef SEVEN_SEG_PIO_NAME
        sevenseg_set_hex(0); 
		#endif
      }  

      /* Always dump text to the LCD... */
	  #ifdef LCD_DISPLAY_NAME
      lcd_output_text( board_control_mbox_contents->LCD_TEXT );
	  #endif
  }
}

void LED_task(void* pdata)
{
  
  alt_u8 led = 0x2;
  alt_u8 dir = 0;
     
  /* 
   * Infinitely shift a variable with one bit set back and forth, and write
   * it to the LED PIO.  Software loop provides delay element.
   */
  while (1) 
  {
    if (led & 0x81) 
    {
      dir = (dir ^ 0x1);
    }

    if (dir) 
    {
      led = led >> 1;
    } 
    else 
    {
      led = led << 1;
    }
    IOWR_ALTERA_AVALON_PIO_DATA(LED_PIO_BASE, led);
    OSTimeDlyHMSM(0,0,0,50);
  }
}

#ifdef SEVEN_SEG_PIO_NAME
static void sevenseg_set_hex(alt_u8 hex)
{
  static alt_u8 segments[16] = {
    0x81, 0xCF, 0x92, 0x86, 0xCC, 0xA4, 0xA0, 0x8F, 0x80, 0x84, /* 0-9 */
    0x88, 0xE0, 0xF2, 0xC2, 0xB0, 0xB8 };                       /* a-f */

  alt_u32 data = segments[hex & 15] | (segments[(hex >> 4) & 15] << 8);

  IOWR_ALTERA_AVALON_PIO_DATA(SEVEN_SEG_PIO_BASE, data);
}

void SSD_task(void* pdata)
{
  alt_u32 count;
  
  while (1)
  {
    for (count = 0; count <= 0xff; count++)
    {
      sevenseg_set_hex(count);
      OSTimeDlyHMSM(0,0,0,50);
    }
  }
}
#endif
/******************************************************************************
*                                                                             *
* License Agreement                                                           *
*                                                                             *
* Copyright (c) 2006 Altera Corporation, San Jose, California, USA.           *
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
