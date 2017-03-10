/******************************************************************************
* Copyright (c) 2006 Altera Corporation, San Jose, California, USA.           *
* All rights reserved. All use of this software and documentation is          *
* subject to the License Agreement located at the end of this file below.     *
*******************************************************************************
*                                                                             *
* File: srec_flash.c                                                          *
*   This file contains srec parsing and flash programming utilities.          *
*                                                                             *
*******************************************************************************/

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

#include "sys/alt_flash.h"
#include "sys/alt_flash_dev.h"

int current_flash_block;

struct flash_inf_struct
{
  char* start;
  int size;
  char device[20];
};

extern struct flash_inf_struct flash_info;

/* Data structure holding the line fragment (leftover) info. */

struct {
  int len;
  char data[MAXLINE];
} frag;

/* Global line count variable. */

int line_count;

/* Flash Functions */

/******************************************************************
*  Function: ProgFlash
*
*  Purpose: This function asks for the name of a flash device,
*  a pointer to a data buffer, and the size of the data buffer.
*
******************************************************************/
int ProgFlash(struct flash_inf_struct *flash_info, int target_addr,
              char* data, int data_len)
{

  // Flash device variables
  alt_flash_fd* fd;
  int number_of_regions;
  flash_region* regions;
  char flashname[40]; /* Be conservative on the size of the string. */
  char* flashname_ptr = flashname;
  int new_flash_block = -1;
  
  // General purpose variables
  unsigned int sw_offset = 0;
  int ret_code = 0x0;
  
  /* 
   * flash_info->device contains the flash device name
   * from the multipart form.
   * - If you want your flash name to be an option, you must change the 
   * upload_image form in index.html to include your flash device's name 
   * in the pick list.  
   */
  
  flashname_ptr += sprintf( flashname_ptr, "/dev/%s", flash_info->device );  
	*(flashname_ptr+1) = '\0';
  
  fd = alt_flash_open_dev(flashname);
  if (fd)
  { 
    /* Get some useful info about the flash */
    ret_code = alt_get_flash_info(fd, &regions, &number_of_regions);
    /* To which flash block is the SREC data destined? */
    /* new_flash_block = target_addr / regions->block_size; */
    /* Ahhh, but what happens if a line spans the end of one block and the 
     * beginning of another? 
     *  - Better to handle this case well...as well!
     */
    new_flash_block = (target_addr + data_len) / regions->block_size;
    if( current_flash_block == -1 )
    {
      /* Output various flash information when programming the first line. */
      printf( "\nFlash Name is %s.\nBlock size is %d bytes.\n\nProgramming Flash...\n", flashname, regions->block_size );
    }
    /* if it's a new block, we need to erase it first. */
    if(new_flash_block != current_flash_block)
    {
      printf("\nFlash Block %d", new_flash_block);
      /* Blindly erase the new flash block */
      alt_erase_flash_block(fd, (new_flash_block * regions->block_size), regions->block_size);
      current_flash_block = new_flash_block;
    }
    alt_write_flash_block(fd, (current_flash_block * regions->block_size), target_addr, data, data_len);
    /* This just gives us some zippy dots so we know hard work is being done */
    if ((target_addr - sw_offset) % (regions->block_size / 8) < data_len)
    {
      printf("\n 0x%8.8X: ", (target_addr & 0xFFFFFF00));
    }
    /*if ((target_addr - sw_offset) % (regions->block_size / 256) < data_len)
    {
      printf(".");
    }*/
  }
  else
  {
    printf("Error Opening flash device. Exiting.");
  }
  alt_flash_close_dev(fd);

  return (ret_code);  
}

/* SREC Functions. */


/******************************************************************
 * Function:  ParseAndProgramLine
 * 
 * Purpose:  Parse line of SREC data and program to flash.
 * 
 * NOTE:  Only S1-S3 record types are supported!
 * S0 lines are ignored, as they contain comment/file header info.
 * The S1-S3 S-Record data lines are parsed and programmed to flash.
 * 
 * The reason this subset is supported is twofold:
 * 1.  It's what the Nios II IDE generates for flash programming.
 * 2.  The other S-Record types (S7,S8,S9) are simpler to parse
 *     and contain less useful information.
 *    - These are left as an exercise up to the user.
 * 
 ******************************************************************/
 
 int ParseAndProgramLine( char* lineBuf, struct flash_inf_struct *flash_info )
 {
  int target_addr;    /* The address to be programmed...parsed from the SREC. */
  char cksm;          /* The SREC line's checksum. */
  int i, byte_count;  /* Number of bytes in record. */
  int overhead;       /* Number of overhead (address + checksum) bytes. */
  int temp;       /* Temporary variable for checksum calculation. */
  char data[80];
  char* data_ptr;
  int data_len;

  if( *lineBuf != 'S' )
  {
    printf( "\nSREC ERROR: line starts with %c.\n", *lineBuf );
    return( 10 );
  }
   
  switch( *(lineBuf + 1) )
  {
    case '0': /* Ignore S0 (header/filename) lines. */
      return( 0 );
    case '1':
      if( sscanf( lineBuf, "S1%2x%4x", &byte_count, &target_addr) != 2 )
      {
        printf("\nSREC Error:  Error scanning S1 type.\n");
        return( 11 );
      }
      overhead = 3;
      break;
    case '2':
      if( sscanf( lineBuf, "S2%2x%6x", &byte_count, &target_addr) != 2 )
      {
        printf("\nSREC Error:  Error scanning S2 type.\n");
        return( 12 );
      }
      overhead = 4;
      break;
    case '3':
      if( sscanf( lineBuf, "S3%2x%8x", &byte_count, &target_addr) != 2 )
      {
        printf("\nSREC Error:  Error scanning S3 type.\n");
        return( 13 );
      }
      overhead = 5;
      break;
      
    default:
      return( 0 );
  }
  
  data_len = byte_count - overhead; /* The amount of data in this SREC line. */
  data_ptr = data; /* Initialize the data pointer */
  
  cksm = 0;
  byte_count++; /* Add one to counter to read the final checksum, as well. */
  
  for( i = 1; i <= byte_count; i++ )
  {
    sscanf( lineBuf + i*2, "%2x", &temp ); /* Do the hex->binary conversion.*/
    cksm += (unsigned char) temp; /* Rolling checksum addition. */
    if( ( i > overhead ) && ( i < byte_count )) /* Copy data into data buffer. */
    {
      *data_ptr++ = (unsigned char) temp; /* Copy value into the data buffer. */
    }
  }
  if( cksm += 1 ) /* Catch checksum error. */
  {
    return( 14 );
  }
  
  if (ProgFlash( flash_info, target_addr, data, data_len) != 0)
  {
    printf("\nFLASH:  Error in flash programming.\n");
  }
  
  return( 0 );
 }
  

/******************************************************************
 * Function:  ParseSRECBuf
 * 
 * Purpose:  
 *  1.  Parses SREC buffer into lines.
 *    - Supports '\n' (UNIX) or '\r\n' (DOS) line endings.
 *  2.  Programs each of these lines into the flash.
 *  3.  Stores unterminated line data for the next pass.
 * 
 * 
 ******************************************************************/
 
int ParseSRECBuf(struct flash_inf_struct *flash_info)
{
  char* cur_ptr;
  char *lineBuf_ptr, *frag_end;
  int lineBuf_len = 0;
  char lineBuf[MAXLINE];
  char tmpStrBuf[MAXLINE];
  int ret_code;
  alt_u8 eol_flag = 0;
  
  lineBuf_ptr = lineBuf; /* Point to beginning of line buffer. */
  cur_ptr = flash_info->start; /* Point to the beginning of the SREC buffer. */

  //printf( "cur_ptr = 0x%x\n", (int) cur_ptr );
  
  /* Iterate over entire buffer. */
  while( cur_ptr <= ( flash_info->start + flash_info->size ))
  { 
    /* End of line processing follows. */
    if ( (*cur_ptr == '\r') || (*cur_ptr == '\n') )
    {
      /* Exception for when \r\n span buffers. */
      if ( (cur_ptr == flash_info->start) && (*cur_ptr == '\n') && (frag.len == 0))
      {
        /* Dont' do anything when '\n' is the first character received and
         * there's no data in the frag (line fragment) structure.
         */
        continue;
      }
      eol_flag = 1; /* Set the End of Line flag to 1. */
      *lineBuf_ptr = 0; /* Terminate the Line String...do not include CR/LFs. */
      lineBuf_len = lineBuf_ptr - lineBuf;
      if ( frag.len > 0 ) /* We have leftovers from the last buffer parsed. */
      {
        if ( lineBuf_len != 0 ) /* If the line buffer has a partial line. */
        {
          /* A little bit of string shuffling to get things in the right order. */
          strcpy( tmpStrBuf, frag.data );
          strcat( tmpStrBuf, lineBuf ); /* Concatenate the lineBuf to the leftovers. */
          strcpy( lineBuf, tmpStrBuf ); /* Should have a line's worth of data, now. */
          lineBuf_len = lineBuf_len + frag.len; /* Update the actual line buffer length. */
        } 
        else /* If the entire line is in frag.data. */
        {
          /* This case is possible when the buffer is '\n' terminated and
           * '\n' is the first character received in a new buffer.
           */
          strcpy( lineBuf, frag.data );
          lineBuf_len = frag.len;
        }
        frag.len = 0; /* The line fragment has been copied into lineBuf. */
      }
      ++line_count;
      /* Since a full (hopefully valid) line has been received, parse it and
       * program it to flash.
       */
      if( (ret_code = ParseAndProgramLine( lineBuf, flash_info)) )
      {
        printf("SREC ERR:  SREC Line Parser returned Error %d.\n", ret_code);
        return( -1 );
      }
      lineBuf_ptr = lineBuf; /* Reset the line buffer pointer. */
      eol_flag = 0; /* Reset the end of line flag. */
      /* Need to skip over the end of line markers.
       *    - Both '\n' (UNIX) and '\r\n' (DOS) are supported. */
      if ( (*cur_ptr == '\r') )
      {
        cur_ptr = cur_ptr + 2;
      }
      else if ( (*cur_ptr == '\n' ))
      {
        ++cur_ptr;
      }
    }
    else
    {
      /* Filling the line buffer. */
      *lineBuf_ptr = *cur_ptr; /* Copy byte from current buffer to line buffer. */
      ++cur_ptr; /* Increment current buffer pointer. */
      ++lineBuf_ptr; /* Increment the line buffer pointer. */
    }
  } /* End of while( cur_ptr <= ( buf + buf_size )) */
  if ( !eol_flag ) /* End of line wasn't reached, but end of buffer was. */
  {
    /* The following fills the frag (line fragment) data structure
     * which is prepended to the first line received in the next buffer.
     */
    frag.len = lineBuf_ptr - lineBuf; /* Length of line "fragment". */
    strcpy( frag.data, lineBuf ); /* Store line fragment. */
    frag_end = (char*) (frag.data + frag.len); /* End of the line fragment. */
    *frag_end = 0; /* Be sure fragment is a NULL terminated string. */
  }
  return( 0 );
}
