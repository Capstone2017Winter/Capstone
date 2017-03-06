#ifndef __WEB_SERVER_H__
#define __WEB_SERVER_H__
/******************************************************************************
* Copyright (c) 2006 Altera Corporation, San Jose, California, USA.           *
* All rights reserved. All use of this software and documentation is          *
* subject to the License Agreement located at the end of this file below.     *
*******************************************************************************
*                                                                             *
* File: user.h                                                                *
*                                                                             *
* Headers for our application. HTTP server-specific headers can be found in   *
* file http.c.                                                                *
*                                                                             *
* Please refer to file ReadMe.txt for notes on this software example.         *
******************************************************************************/

#include "alt_types.h"
#include "includes.h"
/*
 * Prototypes:
 *    die_with_error() - Kills current task and delivers error message to 
 *                       STDERR.
 * 
 * dhcp_timeout_task() - Keeps track of whether an IP address has been 
 *                       aquired from a DHCP server, or times out and resorts
 *                       to a static IP address.
 * 
 *         http_task() - Manages HTTP connections and calls relevant 
 *                       subroutines to service HTTP requests.
 */
void die_with_error(char err_msg[]);

/*
 * Mailbox to control board features 
 * 
 */
extern OS_EVENT *board_control_mbox;

struct http_form_data
{
  alt_u8 LED_ON;
  alt_u8 SSD_ON;
  char LCD_TEXT[20];
  alt_u8 File_Upload[20];
};

extern FILE* lcdDevice;


/* 
 * The IP, gateway, and subnet mask address below are used as a last resort 
 * if no network settings can be found, and DHCP (if enabled) fails. You can
 * edit these as a quick-and-dirty way of changing network settings if desired.
 * 
 * Default IP addresses are set to all zeros so that DHCP server packets will
 * penetrate secure routers. They are NOT intended to be valid static IPs, 
 * these values are only a valid default on networks with DHCP server. 
 * 
 * If DHCP will not be used, select valid static IP addresses here, for example:
 *           IP: 192.168.1.234
 *      Gateway: 192.168.1.1
 *  Subnet Mask: 255.255.255.0
 */
#define IPADDR0   0 
#define IPADDR1   0 
#define IPADDR2   0 
#define IPADDR3   0

#define GWADDR0   0 
#define GWADDR1   0 
#define GWADDR2   0 
#define GWADDR3   0

#define MSKADDR0  255
#define MSKADDR1  255
#define MSKADDR2  255
#define MSKADDR3  0

/*
 * Task priorities
 *
 * MicroC/OS-II only allows one task (thread) per priority number. Our web
 * SERVER task is given a high priority (lower only than timers which must run 
 * when they need to) so that we can focus on pushing data *out* of the system.
 * An ethernet CLIENT application would have lower prioritization than the 
 * stack & ethernet tasks.
 */
#define HTTP_PRIO     4
#define WS_INITIAL_TASK_PRIO 5
#define LED_PRIO      6
#define SSD_PRIO      7
#define BOARD_PRIO    8
/* 
 * Buffer size for a routine to call if things fail
 */
 
/* Definition of Task Stacks for tasks not using networking. */
#define   TASK_STACKSIZE       2048


extern OS_STK    WSInitialTaskStk[TASK_STACKSIZE];
extern OS_STK    BCTaskStk[TASK_STACKSIZE];
extern OS_STK    LEDTaskStk[TASK_STACKSIZE];
extern OS_STK    SSDTaskStk[TASK_STACKSIZE];

#define DIE_WITH_ERROR_BUFFER 256

#endif /* __WEB_SERVER_H__ */

/******************************************************************************
*                                                                             *
* License Agreement                                                           *
*                                                                             *
* Copyright (c) 2009 Altera Corporation, San Jose, California, USA.           *
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
