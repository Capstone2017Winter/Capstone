/******************************************************************************
 * Copyright (c) 2006 Altera Corporation, San Jose, California, USA.           *
 * All rights reserved. All use of this software and documentation is          *
 * subject to the License Agreement located at the end of this file below.     *
 *******************************************************************************
 *                                                                             *
 * File: http.c                                                                *
 *                                                                             *
 * A rough imlementation of HTTP. This is not intended to be a complete        *
 * implementation, just enough for a demo simple web server. This example      *
 * application is more complex than the telnet serer example in that it uses   *
 * non-blocking IO & multiplexing to allow for multiple simultaneous HTTP      *
 * sessions.                                                                   *
 *                                                                             *
 * This example uses the sockets interface. A good introduction to sockets     *
 * programming is the book Unix Network Programming by Richard Stevens         *
 *                                                                             *
 * Please refer to file ReadMe.txt for notes on this software example.         *
 *******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

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

#include "ipport.h"
#include "libport.h"
#include "osport.h"
#include "tcpport.h"

#include "cam_control.h"

//#define	SEND_FILE_DEBUG

#ifdef DEBUG
#include alt_debug.h
#else
#define ALT_DEBUG_ASSERT(a)
#endif /* DEBUG */

//function for sending photos
//int send_captured_pictures(http_conn* conn);

/* 
 * TX & RX buffers. 
 * 
 * These are declared globally to prevent the MicroC/OS-II thread from
 * consuming too much OS-stack space
 */
alt_u8 http_rx_buffer[HTTP_NUM_CONNECTIONS][HTTP_RX_BUF_SIZE];
alt_u8 http_tx_buffer[HTTP_NUM_CONNECTIONS][HTTP_TX_BUF_SIZE];

/* Declare upload buffer structure globally. */
struct upload_buf_struct {
	alt_u8* wr_pos;
	alt_u8* rd_pos;
	alt_u8 buffer[UPLOAD_BUF_SIZE];
} upload_buf;

/* Declare a structure to hold flash programming information. */
struct flash_inf_struct {
	alt_u8* start;
	int size;
	alt_u8 device[40];
} flash_inf;

/*Function prototypes and external functions. */

#ifdef RECONFIG_REQUEST_PIO_NAME
extern void trigger_reset();
#endif
extern int ParseSRECBuf(struct flash_inf_struct *flash_info);
int http_find_file(http_conn* conn);

/* 
 * This canned HTTP reply will serve as a "404 - Not Found" Web page. HTTP
 * headers and HTML embedded into the single string.
 */
static const alt_8
		canned_http_response[] = {
				"\
HTTP/1.0 404 Not Found\r\n\
Content-Type: text/html\r\n\
Content-Length: 272\r\n\r\n\
<HTML><HEAD><TITLE>Nios II Web Server Demonstration</TITLE></HEAD>\
<title>NicheStack on Nios II</title><BODY><h1>HTTP Error 404</h1>\
<center><h2>Nios II Web Server Demonstration</h2>\
Can't find the requested file file. \
Have you programmed the flash filing system into flash?</html>\
" };

static const alt_8
		canned_response2[] = {
				"\
HTTP/1.0 200 OK\r\n\
Content-Type: text/html\r\n\
Content-Length: 2000\r\n\r\n\
<HTML><HEAD><TITLE>Nios II Web Server Demonstration</TITLE></HEAD>\
<title>NicheStack on Nios II</title><BODY>\
<center><h2>Nios II Web Server Hardware Report</h2>\
</center>\
" };

static const alt_8
		status_ready[] = {
				"\
HTTP/1.1 200 OK\r\n\
Content-Type: text/html\r\n\
Content-Length: 2000\r\n\r\n\
READY\
" };

static const alt_8
		status_not_ready[] = {
				"\
HTTP/1.1 200 OK\r\n\
Content-Type: text/html\r\n\
Content-Length: 2000\r\n\r\n\
NOT_READY\
" };
/* 
 * Mapping between pages to post to and functions to call: This allows us to
 * have the HTTP server respond to a POST requset saying "print" by calling
 * a "print" routine (below).
 */
typedef struct funcs {
	alt_u8* name;
	void (*func)();
} post_funcs;

typedef struct conn_funcs {
	alt_u8* name;
	void (*func)(http_conn*);
} post_conn_funcs;

struct http_form_data board_funcs;

/* 
 * print()
 * 
 * This routine is called to demonstrate doing something server-side when an
 * HTTP "POST" command is received.
 */
void print() {
	printf("HTTP POST received.\n");
}

/* To sweep or not to sweep?  The sweep form will tell this function what to do.
 */

void sweep(http_conn* conn) {
	alt_u8* delimiter_token;

	/* Set board_funcs to be off, by default. */

	board_funcs.LED_ON = 0;
	board_funcs.SSD_ON = 0;

	delimiter_token = (alt_u8*) strtok((char*) conn->rx_rd_pos, "&");

	while (delimiter_token != NULL) {
		if (strstr((char*) delimiter_token, "LED")) {
			board_funcs.LED_ON = 1;
		} else if (strstr((char*) delimiter_token, "seven")) {
			board_funcs.SSD_ON = 1;
		}

		delimiter_token = (alt_u8*) strtok(NULL, "&");
	}

	OSMboxPost(board_control_mbox, (void*) &board_funcs);

}

void http_cleanup_lcd_text() {
	int index;

	/* Step through the board_funcs.LCD_TEXT array looking for chars to replace.
	 * For now, just replace '+'s with space characters.
	 */

	for (index = 0; index < 20; ++index) {
		if (board_funcs.LCD_TEXT[index] == '+') {
			board_funcs.LCD_TEXT[index] = ' ';
		}
	}
}

void lcd_output(http_conn* conn) {
	alt_u8* delimiter_token;
	alt_u8* temp_pos;

	delimiter_token = (alt_u8*) strtok((char*) conn->rx_rd_pos, "&");

	while (delimiter_token != NULL) {
		if (strstr((char*) delimiter_token, "lcd_text")) {
			temp_pos = (alt_u8*) strstr((char*) delimiter_token, "=");
			++temp_pos;
			/* LCD_TEXT is limited to 20 characters...limiting string to that size. */
			strncpy((char*) board_funcs.LCD_TEXT, (char*) temp_pos, 20);
		}
		delimiter_token = (alt_u8*) strtok(NULL, "&");
	}

	/* Clean up the HTTP-formatted text string.  For now this just replaces '+'s with spaces. */

	http_cleanup_lcd_text();

	/* Post the updated LCD_TEXT to the board_control_mbox. */
	OSMboxPost(board_control_mbox, (void*) &board_funcs);

}

int http_parse_multipart_header(http_conn* conn) {
	/* Most of the information is on the first line following the boundary. */
	alt_u8* cr_pos;
	alt_u8* temp_pos;

	/*
	 * For now, make the assumption that no multipart headers are split
	 * across packets.  This is a reasonable assumption, but not a surety.
	 *
	 */
	while ((temp_pos = (alt_u8*) strstr((char*) conn->rx_rd_pos,
			(char*) conn->boundary))) {
		if (strstr((char*) conn->rx_rd_pos, "upload_image")) {
			/* Terminate the received data by going back 5
			 * from temp_pos and setting it to NULL.*/
			*(temp_pos - 5) = '\0';
			conn->file_upload = 0;
			break;
		}
		/* Find the end of the content disposition line. */
		conn->rx_rd_pos = (alt_u8*) strstr((char*) conn->rx_rd_pos,
				"Content-Disposition");
		if (conn->rx_rd_pos == 0)
			return (-1);
		cr_pos = (alt_u8*) strchr((char*) conn->rx_rd_pos, '\r');
		if (cr_pos == 0)
			return (-1);
		/* Insert a NULL byte over the second quotation mark. */
		*(cr_pos - 1) = '\0';
		/* Move rx_rd_pos to end of the line, just beyond the newly
		 * inserted NULL.
		 */
		/* Look for "=" delimiter. */
		temp_pos = (alt_u8*) strstr((char*) conn->rx_rd_pos, "=");
		if (temp_pos == 0)
			return (-1);
		/* If second "=" delimiter exists, then parse for conn->filename. */
		if ((temp_pos = (alt_u8*) strstr((char*) (temp_pos + 1), "="))) {
			if (strlen((char*) temp_pos + 2) > 256) {
				return (-1);
			}
			strcpy((char*) conn->filename, ((char*) temp_pos + 2));
			/*
			 * Place rx_rd_pos at the start of the next pertinent line.
			 * In this case, skip two lines ahead.
			 */
			cr_pos = (alt_u8*) strstr(((char*) cr_pos + 1), "\r");
			if (cr_pos == 0)
				return (-1);
			cr_pos = (alt_u8*) strstr(((char*) cr_pos + 1), "\r");
			if (cr_pos == 0)
				return (-1);
			conn->rx_rd_pos = cr_pos + 2;
		} else {
			/*
			 * If no second delimiter, then skip ahead to start of 2nd. line.
			 * That will be the start of the flash device name.
			 *
			 */
			temp_pos = (alt_u8*) strstr(((char*) cr_pos + 1), "\r");
			conn->rx_rd_pos = temp_pos + 2;
			cr_pos = (alt_u8*) strstr((char*) conn->rx_rd_pos, "\r");
			*cr_pos = '\0';
			/* Ok, now copy the flash_device string. */
			strcpy((char*) conn->flash_device, (char*) conn->rx_rd_pos);
			/* Place rx_rd_pos at the start of the next line. */
			conn->rx_rd_pos = cr_pos + 2;
		}
	}
	return (0);
}

void file_upload(http_conn* conn) {
	int buf_len;
	int data_used;
	struct upload_buf_struct *upload_buffer = &upload_buf;
	struct flash_inf_struct *flash_info = &flash_inf;
	/* Look for boundary, parse multipart form "mini" header information if found. */
	if (strstr((char*) conn->rx_rd_pos, (char*) conn->boundary)) {
		if (http_parse_multipart_header(conn)) {
			printf(
					"multipart-form:  header parse failure...resetting connection!");
			conn->state = RESET;
		}
	}
	/* Exception for IE.  It sometimes sends _really_ small initial packets! */
	if ((alt_u8*) strstr((char*) conn->rx_rd_pos, ":")) {
		conn->state = READY;
		return;
	}
	/* Calculate the string size... */
	buf_len = strlen((char*) conn->rx_rd_pos);
	conn->content_received = conn->content_received + buf_len;
	/* Copy all the received data into the upload buffer. */
	if (memcpy((void*) upload_buffer->wr_pos, (void*) conn->rx_rd_pos, buf_len)
			== NULL) {
		printf("ERROR:  memcpy to file upload buffer failed!");
	}
	/* Increment the wr_pos pointer to just after the received data. */
	upload_buffer->wr_pos = upload_buffer->wr_pos + buf_len;
	conn->rx_rd_pos = conn->rx_rd_pos + buf_len;
	/* Reset the buffers after copying the data into the big intermediate
	 * buffer.*/
	data_used = conn->rx_rd_pos - conn->rx_buffer;
	memmove(conn->rx_buffer, conn->rx_rd_pos, conn->rx_wr_pos - conn->rx_rd_pos);
	conn->rx_rd_pos = conn->rx_buffer;
	conn->rx_wr_pos -= data_used;
	memset(conn->rx_wr_pos, 0, data_used);
	if (conn->file_upload == 0) {
		printf("Received a total of %d bytes.\n", conn->content_received);
		/* Insert a NULL character (temporarily). */
		*upload_buffer->wr_pos = '\0';
		/* Populate flash_info struct... print the buffer size. */
		flash_info->size = (int) strlen((char*) upload_buffer->buffer);
		printf("Upload Buffer size = %d.\n", flash_info->size);
		strcpy((char*) flash_info->device, (char*) conn->flash_device);
		flash_info->start = upload_buffer->rd_pos;
		/* Populate the flash_inf struct. */
		//printf( "Here's the Buffer:\n\n%s", upload_buffer->buffer);
		http_find_file(conn);
		conn->close = 1;
	} else {
		conn->state = READY;
	}
}

/* ProgFlashStub()
 *
 * A thin wrapper around the ProgSRECBuf() function in srec_flash.c.
 *
 */
void ProgFlashStub(http_conn* conn) {
	struct flash_inf_struct *flash_info = &flash_inf;
	/* Call ParseSRECBuf, with the flash_info argument. */
	ParseSRECBuf(flash_info);
	/* Go find and send the reset_system.html file. */
	http_find_file(conn);
	/* Close the connection. */
	conn->close = 1;
	return;
}

/*
 * The mapping (using our struct defined above) between HTTP POST commands
 * that we will service and the subroutine called to service that POST
 * command.
 */
post_funcs mapping = { (alt_u8*) "/PRINT", print };

post_conn_funcs sweep_field = { (alt_u8*) "/SWEEP", sweep };

post_conn_funcs lcd_field = { (alt_u8*) "/LCD", lcd_output };

post_conn_funcs upload_field = { (alt_u8*) "/program_flash.html", file_upload };

post_conn_funcs flash_field = { (alt_u8*) "/reset_system.html", ProgFlashStub };

#ifdef RECONFIG_REQUEST_PIO_NAME
post_funcs reset_field =
{
	(alt_u8*)"/RESET_SYSTEM",
	trigger_reset
};
#endif

/*
 * http_reset_connection()
 * 
 * This routine will clear our HTTP connection structure & prepare it to handle
 * a new HTTP connection.
 */
void http_reset_connection(http_conn* conn, int http_instance) {
	memset(conn, 0, sizeof(http_conn));

	conn->fd = -1;
	conn->state = READY;
	conn->keep_alive_count = HTTP_KEEP_ALIVE_COUNT;

	conn->rx_buffer = (alt_u8 *) &http_rx_buffer[http_instance][0];
	conn->tx_buffer = (alt_u8 *) &http_tx_buffer[http_instance][0];
	conn->rx_wr_pos = (alt_u8 *) &http_rx_buffer[http_instance][0];
	conn->rx_rd_pos = (alt_u8 *) &http_rx_buffer[http_instance][0];
}

/*
 * http_manage_connection()
 * 
 * This routine performs house-keeping duties for a specific HTTP connection
 * structure. It is called from various points in the HTTP server code to
 * ensure that connections are reset properly on error, completion, and
 * to ensure that "zombie" connections are dealt with.
 */
void http_manage_connection(http_conn* conn, int http_instance) {
	alt_u32 current_time = 0;

	/*
	 * Keep track of whether an open connection has timed out. This will be
	 * determined by comparing the current time with that of the most recent
	 * activity.
	 */
	if (conn->state == READY || conn->state == PROCESS || conn->state == DATA) {
		current_time = alt_nticks();

		if (((current_time - conn->activity_time) >= HTTP_KEEP_ALIVE_TIME)
				&& conn->file_upload != 1) {
			conn->state = RESET;
		}
	}

	/*
	 * The reply has been sent. Is is time to drop this connection, or
	 * should we persist? We'll keep track of these here and mark our
	 * state machine as ready for additional connections... or not.
	 *  - Only send so many files per connection.
	 *  - Stop when we reach a timeout.
	 *  - If someone (like the client) asked to close the connection, do so.
	 */
	if (conn->state == COMPLETE) {
		if (conn->file_handle != NULL) {
			fclose(conn->file_handle);
		}

		conn->keep_alive_count--;
		conn->data_sent = 0;
		memset(conn->tx_buffer, 0, HTTP_TX_BUF_SIZE);

		if (conn->keep_alive_count == 0) {
			conn->close = 1;
		}

		conn->state = conn->close ? CLOSE : READY;
	}

	/*
	 * Some error occured. http_reset_connection() will take care of most
	 * things, but the RX buffer still needs to be cleared, and any open
	 * files need to be closed. We do this in a separate state to maintain
	 * efficiency between successive (error-free) connections.
	 */
	if (conn->state == RESET) {
		if (conn->file_handle != NULL) {
			fclose(conn->file_handle);
		}

		memset(conn->rx_buffer, 0, HTTP_RX_BUF_SIZE);
		conn->state = CLOSE;
	}

	/* Close the TCP connection */
	if (conn->state == CLOSE) {
		close(conn->fd);
		http_reset_connection(conn, http_instance);
	}
}

/*
 * http_handle_accept()
 * 
 * A listening socket has detected someone trying to connect to us. If we have 
 * any open connection slots we will accept the connection (this creates a 
 * new socket for the data transfer), but if all available connections are in 
 * use we'll ignore the client's incoming connection request.
 */
int http_handle_accept(int listen_socket, http_conn* conn) {
	int ret_code = 0, i, socket, len;
	struct sockaddr_in rem;

	len = sizeof(rem);

	/*
	 * Loop through available connection slots to determine the first available
	 * connection.
	 */
	for (i = 0; i < HTTP_NUM_CONNECTIONS; i++) {
		if ((conn + i)->fd == -1) {
			break;
		}
	}

	/*
	 * There are no more connection slots available. Ignore the connection
	 * request for now.
	 */
	if (i == HTTP_NUM_CONNECTIONS)
		return -1;

	if ((socket = accept(listen_socket,(struct sockaddr*)&rem,&len)) < 0) {
		fprintf(stderr, "[http_handle_accept] accept failed (%d)\n", socket);
		return socket;
	}

	(conn + i)->fd = socket;
	(conn + i)->activity_time = alt_nticks();

	return ret_code;
}

/*
 * http_read_line()
 * 
 * This routine will scan the RX data buffer for a newline, allowing us to
 * parse an in-coming HTTP request line-by-line.
 */
int http_read_line(http_conn* conn) {
	alt_u8* lf_addr;
	int ret_code = 0;

	/* Find the Carriage return which marks the end of the header */
	lf_addr = (alt_u8*) strstr((char*) conn->rx_rd_pos, "\n");

	if (lf_addr == NULL) {
		ret_code = -1;
	} else {
		/*
		 * Check that the line feed has a matching CR, if so zero that
		 * else zero the LF so we can use the string searching functions.
		 */
		if ((lf_addr > conn->rx_buffer) && (*(lf_addr - 1) == '\r')) {
			*(lf_addr - 1) = 0;
		}

		*lf_addr = 0;
		conn->rx_rd_pos = lf_addr + 1;
	}

	return ret_code;
}

/* http_process_multipart()
 * 
 * This function parses and parses relevant "header-like" information
 * from HTTP multipart forms.
 *   - Content-Type, Content-Disposition, boundary, etc.
 */
int http_parse_type_boundary(http_conn* conn, char* start, int len) {
	char* delimiter;
	char* boundary_start;
	char line[HTTP_MAX_LINE_SIZE];

	/* Copy the Content-Type/Boundary line. */
	if (len > HTTP_MAX_LINE_SIZE) {
		printf("process headers:  overflow content-type/boundary parsing.\n");
		return (-1);
	}
	strncpy(line, start, len);
	/* Add a null byte to the end of it. */
	*(line + len) = '\0';
	/* Get the Content-Type value. */
	if ((delimiter = strstr((char*) line, ";"))) {
		/* Need to parse both a boundary and Content-Type. */
		boundary_start = strstr((char*) line, "=") + 2;
		strcpy((char*) conn->boundary, boundary_start);
		/* Insert a null space in place of the delimiter. */
		*delimiter = '\0';
		/* First part of the line is the Content-Type. */
		strcpy((char*) conn->content_type, line);
	} else {
		strcpy((char*) conn->content_type, line);
	}
	return 0;
}

/*
 * http_process_headers()
 * 
 * This routine looks for HTTP header commands, specified by a ":" character.
 * We will look for "Connection: Close" and "Content-length: <len>" strings. 
 * A more advanced server would parse far more header information.
 * 
 * This routine should be modified in the future not to use strtok() as its
 * a bit invasive and is not thread-safe!
 * 
 */
int http_process_headers(http_conn* conn) {
	alt_u8* option;
	alt_u8* cr_pos;
	alt_u8* ct_start;
	alt_u8* orig_read_pos = conn->rx_rd_pos;
	alt_u8* delimiter_token;
	alt_u8 temp_null;
	alt_u8* boundary_start;
	int ct_len;
	int opt_len;

	/*
	 * A boundary was found.  This is a multi-part form
	 * and header processing stops here!
	 *
	 */
	if ((conn->boundary[0] == '-') && (conn->content_length > 0))

	{
		boundary_start = (alt_u8*) strstr((char*) conn->rx_rd_pos,
				(char*) conn->boundary);
		//conn->rx_rd_pos = boundary_start + strlen(conn->boundary);
		return -1;
	}
	/* Skip the next section we'll chop with strtok(). Perl for Nios, anyone? */
	else if ((delimiter_token = (alt_u8*) strstr((char*) conn->rx_rd_pos, ":"))) {
		conn->rx_rd_pos = delimiter_token + 1;
		conn->content_received = conn->rx_rd_pos - conn->rx_buffer;
	} else {
		return -1;
	}

	option = (alt_u8*) strtok((char*) orig_read_pos, ":");

	if (stricmp((char*) option, "Connection") == 0) {
		temp_null = *(option + 17);
		*(option + 17) = 0;

		if (stricmp(((char*) option + 12), "close") == 0) {
			conn->close = 1;
		}
		*(option + 17) = temp_null;
	} else if ((char*) stricmp((char*) option, "Content-Length") == 0) {
		conn->content_length = atoi((char*) option + 16);
		//printf( "Content Length = %d.\n", conn->content_length );
	}
	/* When getting the Content-Type, get the whole line and throw it
	 * to another function.  This will be done several times.
	 */
	else if (stricmp((char*) option, "Content-Type") == 0) {
		/* Determine the end of line for "Content-Type" line. */
		cr_pos = (alt_u8*) strstr((char*) conn->rx_rd_pos, "\r");
		/* Find the length of the string. */
		opt_len = strlen((char*) option);
		ct_len = cr_pos - (option + opt_len + 2);
		/* Calculate the start of the string. */
		ct_start = cr_pos - ct_len;
		/* Pass the start of the string and the size of the string to
		 * a function.
		 */
		if ((http_parse_type_boundary(conn, (char*) ct_start, ct_len) < 0)) {
			/* Something failed...return a negative value. */
			return -1;
		}
	}
	return 0;
}

/*
 * http_process_request()
 * 
 * This routine parses the beginnings of an HTTP request to extract the
 * command, version, and URI. Unsupported commands/versions/etc. will cause
 * us to error out drop the connection.
 */
int http_process_request(http_conn* conn) {
	alt_u8* uri = 0;
	alt_u8* version = 0;
	alt_u8* temp = 0;
	if ((temp = (alt_u8*) strstr((char*) conn->rx_rd_pos, "GET"))) {
		conn->action = GET;
		conn->rx_rd_pos = temp;
	} else if ((temp = (alt_u8*) strstr((char*) conn->rx_rd_pos, "POST"))) {
		conn->action = POST;
		conn->rx_rd_pos = temp;
	} else {
		fprintf(stderr, "Unsupported (for now) request\n");
		conn->action = UNKNOWN;
		return -1;
	}

	/* First space char separates action from URI */
	if ((conn->rx_rd_pos = (alt_u8*) strstr((char*) conn->rx_rd_pos, " "))) {
		conn->rx_rd_pos++;
		uri = conn->rx_rd_pos;
	} else {
		return -1;
	}

	/* Second space char separates URI from HTTP version. */
	if ((conn->rx_rd_pos = (alt_u8*) strstr((char*) conn->rx_rd_pos, " "))) {
		*conn->rx_rd_pos = 0;
		conn->rx_rd_pos++;
		version = conn->rx_rd_pos;
	} else {
		return -1;
	}

	/* Is this an HTTP version we support? */
	if ((version == NULL) || (strncmp((char*) version, "HTTP/", 5) != 0)) {
		return -1;
	}

	if (!isdigit(version[5]) || version[6] != '.' || !isdigit(version[7])) {
		return -1;
	}

	/* Before v1.1 we close the connection after responding to the request */
	if ((((version[5] - '0') * 10) + version[7] - '0') < 11) {
		conn->close = 1;
	}

	strcpy((char*) conn->uri, (char*) uri);
	return 0;
}

//TODO: hello
/*
 * http_send_file_chunk()
 * 
 * This routine will send the next chunk of a file during an open HTTP session
 * where a file is being sent back to the client. This routine is called 
 * repeatedly until the file is completely sent, at which time the connection
 * state will go to "COMPLETE". Doing this rather than sending the entire
 * file allows us (in part) to multiplex between connections "simultaneously".
 */
int http_send_file_chunk(http_conn* conn) {
	int chunk_sent = 0, ret_code = 0, file_chunk_size = 0, result = 0;
	alt_u8* tx_ptr;
#ifdef SEND_FILE_DEBUG
	printf("Entering send file chunk\n");
#endif

	if (conn->data_sent < conn->file_length) {
		file_chunk_size
				= fread(
						conn->tx_buffer,
						1,
						MIN(HTTP_TX_BUF_SIZE, (conn->file_length - conn->data_sent)),
						conn->file_handle);

#ifdef SEND_FILE_DEBUG
		printf("Sending a total of %i bytes\n", file_chunk_size);
#endif

		tx_ptr = conn->tx_buffer;

		while (chunk_sent < file_chunk_size) {
			result = send(conn->fd, (char*)tx_ptr, file_chunk_size, 0);

#ifdef SEND_FILE_DEBUG
			printf("Just sent %i bytes\n", result);
#endif

			/* Error - get out of here! */
			if (result < 0) {
				fprintf(stderr, "[http_send_file] file send returned %d\n",
						result);
				ALT_DEBUG_ASSERT(1);
				conn->state = RESET;
				return result;
			}

			/*
			 * No errors, but the number of bytes sent might be less than we wanted.
			 */
			else {
				conn->activity_time = alt_nticks();
				chunk_sent += result;
				conn->data_sent += result;
				tx_ptr += result;
				file_chunk_size -= result;
			}
		} /* while(chunk_sent < file_chunk_size) */
	} /* if(conn->data_sent < conn->file_length) */

	/*
	 * We managed to send all of the file contents to the IP stack successfully.
	 * At this point we can mark our connection info as complete.
	 */
	if (conn->data_sent >= conn->file_length) {
		conn->state = COMPLETE;
	}

#ifdef SEND_FILE_DEBUG
	printf("Done file chunk\n");
#endif

	return ret_code;
}

//TODO
/*
 * http_send_file_header()
 *
 * Construct and send an HTTP header describing the now-opened file that is
 * about to be sent to the client.
 */
int http_send_file_header(http_conn* conn, const alt_u8* name, int code) {
	int result = 0, ret_code = 0;
	alt_u8* tx_wr_pos = conn->tx_buffer;
	fpos_t end, start;
	const alt_u8* ext = (alt_u8*) strstr((char*) name, ".");

	tx_wr_pos += sprintf((char*) tx_wr_pos, HTTP_VERSION_STRING);

	switch (code) {
	/* HTTP Code: "200 OK\r\n" (we have opened the file successfully) */
	case HTTP_OK: {
		tx_wr_pos += sprintf((char*) tx_wr_pos, HTTP_OK_STRING);
		break;
	}
		/* HTTP Code: "404 Not Found\r\n" (couldn't find requested file) */
	case HTTP_NOT_FOUND: {
		tx_wr_pos += sprintf((char*) tx_wr_pos, HTTP_NOT_FOUND_STRING);
		break;
	}
	default: {
		fprintf(stderr, "[http_send_file_header] Invalid HTTP code: %d\n", code);
		conn->state = RESET;
		return -1;
		break;
	}
	}

	/* Handle the various content types */
	tx_wr_pos += sprintf((char*) tx_wr_pos, HTTP_CONTENT_TYPE);

	if (!strcasecmp((char*) ext, ".html")) {
		tx_wr_pos += sprintf((char*) tx_wr_pos, HTTP_CONTENT_TYPE_HTML);
	} else if (!strcasecmp((char*) ext, ".jpg")) {
		tx_wr_pos += sprintf((char*) tx_wr_pos, HTTP_CONTENT_TYPE_JPG);
	} else if (!strcasecmp((char*) ext, ".gif")) {
		tx_wr_pos += sprintf((char*) tx_wr_pos, HTTP_CONTENT_TYPE_GIF);
	} else if (!strcasecmp((char*) ext, ".png")) {
		tx_wr_pos += sprintf((char*) tx_wr_pos, HTTP_CONTENT_TYPE_PNG);
	} else if (!strcasecmp((char*) ext, ".js")) {
		tx_wr_pos += sprintf((char*) tx_wr_pos, HTTP_CONTENT_TYPE_JS);
	} else if (!strcasecmp((char*) ext, ".css")) {
		tx_wr_pos += sprintf((char*) tx_wr_pos, HTTP_CONTENT_TYPE_CSS);
	} else if (!strcasecmp((char*) ext, ".swf")) {
		tx_wr_pos += sprintf((char*) tx_wr_pos, HTTP_CONTENT_TYPE_SWF);
	} else if (!strcasecmp((char*) ext, ".ico")) {
		tx_wr_pos += sprintf((char*) tx_wr_pos, HTTP_CONTENT_TYPE_ICO);
	} else {
		fprintf(stderr, "[http_send_file] Unknown content type: \"%s\"\n", ext);
		conn->state = RESET;
		ALT_DEBUG_ASSERT(1);
		return -1;
	}

	/* Get the file length and stash it into our connection info */
	fseek(conn->file_handle, 0, SEEK_END);
	fgetpos(conn->file_handle, &end);
	fseek(conn->file_handle, 0, SEEK_SET);
	fgetpos(conn->file_handle, &start);
	conn->file_length = end - start;

	/* "Content-Length: <length bytes>\r\n" */
	tx_wr_pos += sprintf((char*) tx_wr_pos, HTTP_CONTENT_LENGTH);
	tx_wr_pos += sprintf((char*) tx_wr_pos, "%d\r\n", conn->file_length);

	/*
	 * 'close' will be set during header parsing if the client either specified
	 * that they wanted the connection closed ("Connection: Close"), or if they
	 * are using an HTTP version prior to 1.1. Otherwise, we will keep the
	 * connection alive.
	 *
	 * We send a specified number of files in a single keep-alive connection,
	 * we'll also close the connection. It's best to be polite and tell the client,
	 * though.
	 */
	if (!conn->keep_alive_count) {
		conn->close = 1;
	}

	if (conn->close) {
		tx_wr_pos += sprintf((char*) tx_wr_pos, HTTP_CLOSE);
	} else {
		tx_wr_pos += sprintf((char*) tx_wr_pos, HTTP_KEEP_ALIVE);
	}

	/* "\r\n" (two \r\n's in a row means end of headers */
	tx_wr_pos += sprintf((char*) tx_wr_pos, HTTP_CR_LF);

	/* Send the reply header */
	result
			= send(conn->fd, (char*)conn->tx_buffer, (tx_wr_pos - conn->tx_buffer),
					0);

	if (result < 0) {
		fprintf(stderr, "[http_send_file] header send returned %d\n", result);
		conn->state = RESET;
		return result;
	} else {
		conn->activity_time = alt_nticks();
	}

	return ret_code;
}

/*
 * http_find_file()
 *
 * Try to find the file requested. If nothing is requested you get /index.html
 * If we can't find it, send a "404 - Not found" message.
 */
int http_find_file(http_conn* conn) {
	alt_u8 filename[256];
	int ret_code = 0;

	strncpy((char*) filename, ALTERA_RO_ZIPFS_NAME,
			strlen(ALTERA_RO_ZIPFS_NAME));

	/* URI of "/" means get the default, usually index.html */
	if ((conn->uri[0] == '/') && (conn->uri[1] == '\0')) {
		strcpy((char*) filename + strlen(ALTERA_RO_ZIPFS_NAME),
				HTTP_DEFAULT_FILE);
	} else {
		strcpy((char*) filename + strlen(ALTERA_RO_ZIPFS_NAME),
				(char*) conn->uri);
	}

	/* Try to open the file */
	printf("\nFetching file:  %s.\n", filename);
	conn->file_handle = fopen((char*) filename, "r");

	/* Can't find the requested file? Try for a 404-page. */
	if (conn->file_handle == NULL) {
		strcpy((char*) filename, ALTERA_RO_ZIPFS_NAME);
		strcpy((char*) filename + strlen(ALTERA_RO_ZIPFS_NAME),
				HTTP_NOT_FOUND_FILE);
		conn->file_handle = fopen((char*) filename, "r");

		/* We located the specified "404: Not-Found" page */
		if (conn->file_handle != NULL) {
			ALT_DEBUG_ASSERT(fd != NULL);
			ret_code = http_send_file_header(conn, filename, HTTP_NOT_FOUND);
		}
		/* Can't find the 404 page: This likely means there is no file system */
		else {
			fprintf(stderr, "Can't open the 404 File Not Found error page.\n");
			fprintf(stderr,
					"Have you programmed the filing system into flash?\n");
			send(conn->fd,(char*)canned_http_response,strlen((char*)canned_http_response),0);

			fclose(conn->file_handle);
			conn->state = RESET;
			return -1;
		}
	}
	/* We've found the requested file; send its header and move on. */
	else {
		ret_code = http_send_file_header(conn, filename, HTTP_OK);
	}

	return ret_code;
}

/*
 * http_send_file()
 *
 * This function sends re-directs to either program_flash.html or
 * reset_sytem.html.
 */

void http_send_redirect(alt_u8 redirect[256]) {
	printf("Don't do anything....for now.\n");
}

/*
 * http_handle_post()
 *
 * Process the post request and take the appropriate action.
 */
int http_handle_post(http_conn* conn) {
	alt_u8* tx_wr_pos = conn->tx_buffer;
	int ret_code = 0;
	struct upload_buf_struct *upload_buffer = &upload_buf;

	tx_wr_pos += sprintf((char*) tx_wr_pos, HTTP_VERSION_STRING);
	tx_wr_pos += sprintf((char*) tx_wr_pos, HTTP_NO_CONTENT_STRING);
	tx_wr_pos += sprintf((char*) tx_wr_pos, HTTP_CLOSE);
	tx_wr_pos += sprintf((char*) tx_wr_pos, HTTP_END_OF_HEADERS);

	if (!strcmp((char*) conn->uri, (char*) mapping.name)) {
		send(conn->fd, (char*)conn->tx_buffer, (tx_wr_pos - conn->tx_buffer), 0);
		conn->state = CLOSE;
		mapping.func();
	}

	else if (!strcmp((char*) conn->uri, (char*) sweep_field.name)) {
		send(conn->fd, (char*)conn->tx_buffer, (tx_wr_pos - conn->tx_buffer), 0);
		conn->state = CLOSE;
		sweep_field.func(conn);
	}

	else if (!strcmp((char*) conn->uri, (char*) lcd_field.name)) {
		send(conn->fd, (char*)conn->tx_buffer, (tx_wr_pos - conn->tx_buffer), 0);
		conn->state = CLOSE;
		lcd_field.func(conn);
	}

	else if (!strcmp((char*) conn->uri, (char*) upload_field.name)) {
		conn->file_upload = 1;
		upload_buffer->rd_pos = upload_buffer->wr_pos
				= (alt_u8*) upload_buffer->buffer;
		memset(upload_buffer->rd_pos, '\0', conn->content_length);
		upload_field.func(conn);
	} else if (!strcmp((char*) conn->uri, (char*) flash_field.name)) {
		/* Kick off the flash programming. */
		flash_field.func(conn);
	}
#ifdef RECONFIG_REQUEST_PIO_NAME
	else if (!strcmp(conn->uri, reset_field.name))
	{
		/* Close the socket. */
		send(conn->fd, conn->tx_buffer, (tx_wr_pos - conn->tx_buffer), 0);
		reset_field.func();
	}
#endif  
	return ret_code;
}
/*
 * http_prepare_response()
 * 
 * Service the various HTTP commands, calling the relevant subroutine. 
 * We only handle GET and POST.
 */
int http_prepare_response(http_conn* conn) {
	int ret_code = 0;

	//printf("Preparing Response\n");
	switch (conn->action) {
	case GET:
		if (strcasecmp((char*) conn->uri, CAPTURE) == 0) {
			ret_code = takePictures();
			send(conn->fd,(char*)status_not_ready,strlen((char*)status_not_ready),0);
			conn->state = CLOSE;
		} else if (strcasecmp((char*) conn->uri, IMAGE_LEFT) == 0) {
			conn->dynamic_image = 1;
			conn->left_length = getImageLength(0);
			ret_code = http_send_capture_left_header(conn);
		} else if (strcasecmp((char*) conn->uri, IMAGE_RIGHT) == 0) {
			conn->dynamic_image = 2;
			conn->right_length = getImageLength(1);
			ret_code = http_send_capture_right_header(conn);
		} else if (strcasecmp((char*) conn->uri, IMAGES_READY) == 0) {
			conn->jpgs_written = isPictureReady();
			if (conn->jpgs_written == 1) {
				send(conn->fd,(char*)status_ready,strlen((char*)status_ready),0);
				conn->state = CLOSE;
			} else {
				send(conn->fd, (char*)status_not_ready, strlen((char*)status_not_ready), 0);
				conn->state = CLOSE;
			}
		} else {
			/* Find file from uri */
			ret_code = http_find_file(conn);
		}
		break;

	case POST:
		/* Handle POSTs. */
		ret_code = http_handle_post(conn);
		break;

	default:
		break;
	}/* switch (conn->action) */

	return ret_code;
}

/*
 * http_send_dynamic_image_chunk()
 *
 * This routine will send the next chunk of a dynamic image stored in memory during an open HTTP session
 * where a dynamic image is being sent back to the client. This routine is called
 * repeatedly until the dynamic image is completely sent, at which time the connection
 * state will go to "COMPLETE". Doing this rather than sending the entire
 * file allows us (in part) to multiplex between connections "simultaneously"
 * If the image format is bitmap, the first time this routine is run it will
 * generate and send the bitmap file headers.
 */
//int http_send_dynamic_image_chunk(http_conn* conn, int left_right) {
//
//#ifdef SEND_FILE_DEBUG
//	printf("Entered send image chunk\n");
//#endif
//
//	int chunk_sent = 0, ret_code = 0, file_chunk_size = 0, result = 0;
//	alt_u8* tx_ptr;
//	alt_u32 data_address;
//
//	data_address = (left_right) ? RIGHT_ADDRESS : LEFT_ADDRESS;
//
//#ifdef SEND_FILE_DEBUG
//	printf("Data Address used is: %X, File Length is: %d\n", (unsigned int)data_address, conn->image_length);
//#endif
//
//	int min = MIN(HTTP_TX_BUF_SIZE, (conn->image_length - conn->data_sent));
//
//	if (conn->data_sent < conn->image_length) {
//		int i;
//		alt_u32 bytes_read;
//		for (i = 0; i < min; i++) {
//				conn->tx_buffer[i] = IORD_8DIRECT(data_address, conn->data_sent+i);
//
//				/* read in 32 bytes at a time */
////				bytes_read = IORD_32DIRECT(data_address, conn->data_sent+i);
////				conn->tx_buffer[i + 0] = (alt_u8)((bytes_read & 0x000000FF)>>0);
////				conn->tx_buffer[i + 1] = (alt_u8)((bytes_read & 0x0000FF00)>>8);
////				conn->tx_buffer[i + 2] = (alt_u8)((bytes_read & 0x00FF0000)>>16);
////				conn->tx_buffer[i + 3] = (alt_u8)((bytes_read & 0xFF000000)>>24);
//				file_chunk_size++;
//		}
//
//#ifdef SEND_FILE_DEBUG
////		printf("image data read is:\n");
////		int k;
////		for(k=0;k<file_chunk_size;k++){
////			printf("0x%X\n", conn->tx_buffer[k]);
////		}
//		printf("Sending image data from address %X to %X == %i bytes\n",
//				(unsigned int)(data_address+conn->data_sent), (unsigned int)(data_address+conn->data_sent+file_chunk_size),
//				file_chunk_size);
//#endif
//
//		tx_ptr = conn->tx_buffer;
//
//		while (chunk_sent < file_chunk_size) {
//			result = send(conn->fd, (char*)tx_ptr, file_chunk_size, 0);
//
//#ifdef SEND_FILE_DEBUG
//			printf("Just sent %i bytes\n", result);
//#endif
//
//			/* Error - get out of here! */
//			if (result < 0) {
//				fprintf(stderr, "[http_send_file] file send returned %d\n",	result);
//				ALT_DEBUG_ASSERT(1);
//				conn->state = RESET;
//				return result;
//			} else {
//				conn->activity_time = alt_nticks();
//				chunk_sent += result;
//				conn->data_sent += result;
//				tx_ptr += result;
//				file_chunk_size -= result;
//			}
//		}/* while(chunk_sent < file_chunk_size) */
//	} /* if(conn->data_sent < conn->file_length) */
//
//	/*
//	 * We managed to send all of the file contents to the IP stack successfully.
//	 * At this point we can mark our connection info as complete.
//	 */
//	if (conn->data_sent >= conn->image_length) {
//		conn->state = COMPLETE;
//	}
//
//#ifdef SEND_FILE_DEBUG
//	printf("Dynamic image send complete\n");
//	fflush(stdin);
//#endif
//
//	return ret_code;
//}
//TODO:
/*
 *
 */
int http_send_left_image_chunk(http_conn* conn) {
	INT8U err;
#ifdef SEND_FILE_DEBUG
	printf("Entered send image chunk\n");
#endif
	int chunk_sent = 0, ret_code = 0, file_chunk_size = 0, result = 0;
	alt_u8* tx_ptr;
	alt_u32 data_address;

	data_address = LEFT_ADDRESS;
	OSSemPend(leftImageSem, 0, &err);
	if (err == OS_NO_ERR) {
#ifdef SEND_FILE_DEBUG
		printf("Data Address used is: %X, File Length is: %d\n",
				(unsigned int) data_address, conn->left_length);
#endif

		if (conn->data_sent < conn->left_length) {
			int i;
			//			alt_u32 bytes_read;
			int
					min =
							MIN(HTTP_TX_BUF_SIZE, (conn->left_length - conn->data_sent));
			for (i = 0; i < min; i++) {
				conn->tx_buffer[i]
						= IORD_8DIRECT(data_address, conn->data_sent+i);

				/* read in 32 bytes at a time */
				//				bytes_read = IORD_32DIRECT(data_address, conn->data_sent+i);
				//				conn->tx_buffer[i + 0] = (alt_u8) ((bytes_read & 0x000000FF)
				//						>> 0);
				//				conn->tx_buffer[i + 1] = (alt_u8) ((bytes_read & 0x0000FF00)
				//						>> 8);
				//				conn->tx_buffer[i + 2] = (alt_u8) ((bytes_read & 0x00FF0000)
				//						>> 16);
				//				conn->tx_buffer[i + 3] = (alt_u8) ((bytes_read & 0xFF000000)
				//						>> 24);
				file_chunk_size++;
			}

#ifdef SEND_FILE_DEBUG
//					printf("image data read is:\n");
//					int k;
//					for(k=0;k<file_chunk_size;k++){
//						printf("0x%X\n", conn->tx_buffer[k]);
//					}
			printf("Sending image data from address %X to %X == %i bytes\n",
					(unsigned int) (data_address + conn->data_sent),
					(unsigned int) (data_address + conn->data_sent
							+ file_chunk_size), file_chunk_size);
#endif

			tx_ptr = conn->tx_buffer;

			if (chunk_sent < file_chunk_size) {
				result = send(conn->fd, (char*)tx_ptr, file_chunk_size, 0);

#ifdef SEND_FILE_DEBUG
				printf("Just sent %i bytes\n", result);
#endif

				/* Error - get out of here! */
				if (result < 0) {
					fprintf(stderr, "[http_send_file] file send returned %d\n",
							result);
					ALT_DEBUG_ASSERT(1);
					conn->state = RESET;
					return result;
				} else {
					conn->activity_time = alt_nticks();
					chunk_sent += result;
					conn->data_sent += result;
					tx_ptr += result;
					file_chunk_size -= result;
				}
			}/* while(chunk_sent < file_chunk_size) */
		} /* if(conn->data_sent < conn->file_length) */

		/*
		 * We managed to send all of the file contents to the IP stack successfully.
		 * At this point we can mark our connection info as complete.
		 */
		if (conn->data_sent >= conn->left_length) {
			memset(conn->tx_buffer, 0, sizeof(conn->tx_buffer));
			conn->state = COMPLETE;
		}

#ifdef SEND_FILE_DEBUG
		printf("Dynamic image send complete\n");
		fflush(stdin);
#endif
	}
	OSSemPost(leftImageSem);
	return ret_code;
}

int http_send_right_image_chunk(http_conn* conn) {
	INT8U err;
#ifdef SEND_FILE_DEBUG
	printf("Entered send image chunk\n");
#endif

	int chunk_sent = 0, ret_code = 0, file_chunk_size = 0, result = 0;
	alt_u8* tx_ptr;
	alt_u32 data_address;

	data_address = RIGHT_ADDRESS;
	OSSemPend(rightImageSem, 0, &err);
	if (err == OS_NO_ERR) {
#ifdef SEND_FILE_DEBUG
		printf("Data Address used is: %X, File Length is: %d\n",
				(unsigned int) data_address, conn->right_length);
#endif

		if (conn->data_sent < conn->right_length) {
			int i;
			//			alt_u32 bytes_read;
			int
					min =
							MIN(HTTP_TX_BUF_SIZE, (conn->right_length - conn->data_sent));
			for (i = 0; i < min; i++) {
				conn->tx_buffer[i]
						= IORD_8DIRECT(data_address, conn->data_sent+i);

				/* read in 32 bytes at a time */
				//				bytes_read = IORD_32DIRECT(data_address, conn->data_sent+i);
				//				conn->tx_buffer[i + 0] = (alt_u8) ((bytes_read & 0x000000FF)
				//						>> 0);
				//				conn->tx_buffer[i + 1] = (alt_u8) ((bytes_read & 0x0000FF00)
				//						>> 8);
				//				conn->tx_buffer[i + 2] = (alt_u8) ((bytes_read & 0x00FF0000)
				//						>> 16);
				//				conn->tx_buffer[i + 3] = (alt_u8) ((bytes_read & 0xFF000000)
				//						>> 24);
				file_chunk_size++;
			}

#ifdef SEND_FILE_DEBUG
//					printf("image data read is:\n");
//					int k;
//					for(k=0;k<file_chunk_size;k++){
//						printf("0x%X\n", conn->tx_buffer[k]);
//					}
			printf("Sending image data from address %X to %X == %i bytes\n",
					(unsigned int) (data_address + conn->data_sent),
					(unsigned int) (data_address + conn->data_sent
							+ file_chunk_size), file_chunk_size);
#endif

			tx_ptr = conn->tx_buffer;

			while (chunk_sent < file_chunk_size) {
				result = send(conn->fd, (char*)tx_ptr, file_chunk_size, 0);

#ifdef SEND_FILE_DEBUG
				printf("Just sent %i bytes\n", result);
#endif

				/* Error - get out of here! */
				if (result < 0) {
					fprintf(stderr, "[http_send_file] file send returned %d\n",
							result);
					ALT_DEBUG_ASSERT(1);
					conn->state = RESET;
					return result;
				} else {
					conn->activity_time = alt_nticks();
					chunk_sent += result;
					conn->data_sent += result;
					tx_ptr += result;
					file_chunk_size -= result;
				}
			}/* while(chunk_sent < file_chunk_size) */
		} /* if(conn->data_sent < conn->file_length) */

		/*
		 * We managed to send all of the file contents to the IP stack successfully.
		 * At this point we can mark our connection info as complete.
		 */
		if (conn->data_sent >= conn->right_length) {
			memset(conn->tx_buffer, 0, sizeof(conn->tx_buffer));
			conn->state = COMPLETE;
		}

#ifdef SEND_FILE_DEBUG
		printf("Dynamic image send complete\n");
		fflush(stdin);
#endif
	}
	OSSemPost(rightImageSem);

	return ret_code;
}

//TODO
int http_send_capture_left_header(http_conn* conn) {
	int result = 0, ret_code = 0;
	alt_u8* tx_wr_pos = conn->tx_buffer;

	// Version, HTTP status, and content type
	tx_wr_pos += sprintf((char*) tx_wr_pos, HTTP_VERSION_STRING);

	tx_wr_pos += sprintf((char*) tx_wr_pos, HTTP_OK_STRING);

	tx_wr_pos += sprintf((char*) tx_wr_pos, HTTP_CONTENT_TYPE);

	tx_wr_pos += sprintf((char*) tx_wr_pos, HTTP_CONTENT_TYPE_JPG);

	// Content-Length
	tx_wr_pos += sprintf((char*) tx_wr_pos, HTTP_CONTENT_LENGTH);
	tx_wr_pos += sprintf((char*) tx_wr_pos, "%d\r\n", conn->left_length);

	//Keep connection alive or close it
	if (!conn->keep_alive_count)
		conn->close = 1;

	if (conn->close)
		tx_wr_pos += sprintf((char*) tx_wr_pos, HTTP_CLOSE);
	else
		tx_wr_pos += sprintf((char*) tx_wr_pos, HTTP_KEEP_ALIVE);

	// "\r\n" (two \r\n's in a row means end of headers
	tx_wr_pos += sprintf((char*) tx_wr_pos, HTTP_CR_LF);

	// Send the reply header
	result
			= send(conn->fd, (char*)conn->tx_buffer, (tx_wr_pos-conn->tx_buffer), 0);

	if (result < 0) {
		fprintf(stderr, "[http_send_file] header send returned %d\n", result);
		conn->state = RESET;
		return result;
	} else
		conn->activity_time = alt_nticks();

	return ret_code;
}

int http_send_capture_right_header(http_conn* conn) {
	int result = 0, ret_code = 0;
	alt_u8* tx_wr_pos = conn->tx_buffer;

	// Version, HTTP status, and content type
	tx_wr_pos += sprintf((char*) tx_wr_pos, HTTP_VERSION_STRING);

	tx_wr_pos += sprintf((char*) tx_wr_pos, HTTP_OK_STRING);

	tx_wr_pos += sprintf((char*) tx_wr_pos, HTTP_CONTENT_TYPE);

	tx_wr_pos += sprintf((char*) tx_wr_pos, HTTP_CONTENT_TYPE_JPG);

	// Content-Length
	tx_wr_pos += sprintf((char*) tx_wr_pos, HTTP_CONTENT_LENGTH);

	tx_wr_pos += sprintf((char*) tx_wr_pos, "%d\r\n", conn->right_length);

	//Keep connection alive or close it
	if (!conn->keep_alive_count)
		conn->close = 1;

	if (conn->close)
		tx_wr_pos += sprintf((char*) tx_wr_pos, HTTP_CLOSE);
	else
		tx_wr_pos += sprintf((char*) tx_wr_pos, HTTP_KEEP_ALIVE);

	// "\r\n" (two \r\n's in a row means end of headers
	tx_wr_pos += sprintf((char*) tx_wr_pos, HTTP_CR_LF);

	// Send the reply header
	result
			= send(conn->fd, (char*)conn->tx_buffer, (tx_wr_pos-conn->tx_buffer), 0);

	if (result < 0) {
		fprintf(stderr, "[http_send_file] header send returned %d\n", result);
		conn->state = RESET;
		return result;
	} else
		conn->activity_time = alt_nticks();

	return ret_code;
}

/*
 * http_handle_receive()
 *
 * Work out what the request we received was, and handle it.
 */
void http_handle_receive(http_conn* conn, int http_instance) {
	int data_used, rx_code;

	if (conn->state == READY) {
		rx_code
				= recv(conn->fd, (char*)conn->rx_wr_pos,
						(HTTP_RX_BUF_SIZE - (conn->rx_wr_pos - conn->rx_buffer) -1),
						0);

		/*
		 * If a valid data received, take care of buffer pointer & string
		 * termination and move on. Otherwise, we need to return and wait for more
		 * data to arrive (until we time out).
		 */
		if (rx_code > 0) {
			/* Increment rx_wr_pos by the amount of data received. */
			conn->rx_wr_pos += rx_code;
			/* Place a zero just after the data received to serve as a terminator. */
			*(conn->rx_wr_pos + 1) = 0;

			if (strstr((char*) conn->rx_buffer, HTTP_END_OF_HEADERS)) {
				conn->state = PROCESS;
			}
			/* If the connection is a file upload, skip right to DATA.*/
			if (conn->file_upload == 1) {
				conn->state = DATA;
			}
		}
	}

	if (conn->state == PROCESS) {
		/*
		 * If we (think) we have valid headers, keep the connection alive a bit
		 * longer.
		 */
		conn->activity_time = alt_nticks();

		/*
		 * Attempt to process the fundamentals of the HTTP request. We may
		 * error out and reset if the request wasn't complete, or something
		 * was asked from us that we can't handle.
		 */
		if (http_process_request(conn)) {
			fprintf(stderr,
					"[http_handle_receive] http_process_request failed\n");
			conn->state = RESET;
			http_manage_connection(conn, http_instance);
		}

		/*
		 * Step through the headers to see if there is any other useful
		 * information about our pending transaction to extract. After that's
		 * done, send some headers of our own back to let the client know
		 * what's happening. Also, once all in-coming headers have been parsed
		 * we can manage our RX buffer to prepare for the next in-coming
		 * connection.
		 */
		while (conn->state == PROCESS) {
			if (http_read_line(conn)) {
				fprintf(stderr, "[http_handle_receive] error reading headers\n");
				conn->state = RESET;
				http_manage_connection(conn, http_instance);
				break;
			}
			if (http_process_headers(conn)) {
				if ((conn->rx_rd_pos = (alt_u8*) strstr(
						(char*) conn->rx_rd_pos, HTTP_CR_LF))) {
					conn->rx_rd_pos += 2;
					conn->state = DATA;
					conn->activity_time = alt_nticks();
				} else {
					fprintf(stderr,
							"[http_handle_receive] Can't find end of headers!\n");
					conn->state = RESET;
					http_manage_connection(conn, http_instance);
					break;
				}
			}
		} /* while(conn->state == PROCESS) */

		if (http_prepare_response(conn)) {
			conn->state = RESET;
			fprintf(stderr, "[http_handle_receive] Error preparing response\n");
			http_manage_connection(conn, http_instance);
		}

		/*
		 * Manage RX Buffer: Slide any un-read data in our input buffer
		 * down over previously-read data that can now be overwritten, and
		 * zero-out any bytes in question at the top of our new un-read space.
		 */
		if (conn->rx_rd_pos > (conn->rx_buffer + HTTP_RX_BUF_SIZE)) {
			conn->rx_rd_pos = conn->rx_buffer + HTTP_RX_BUF_SIZE;
		}

		data_used = conn->rx_rd_pos - conn->rx_buffer;
		memmove(conn->rx_buffer, conn->rx_rd_pos, conn->rx_wr_pos
				- conn->rx_rd_pos);
		conn->rx_rd_pos = conn->rx_buffer;
		conn->rx_wr_pos -= data_used;
		memset(conn->rx_wr_pos, 0, data_used);
	}

	if (conn->state == DATA && conn->file_upload == 1) {
		/* Jump to the file_upload() function....process more received data. */
		upload_field.func(conn);
	}
}

/* 
 * http_handle_transmit()
 * 
 * Transmit a chunk of a file in an active HTTP connection. This routine
 * will be called from the thread's main loop when ever the socket is in 
 * the 'DATA' state and the socket is marked as available for writing (free
 * buffer space). 
 */
void http_handle_transmit(http_conn* conn, int http_instance) {

	if (conn->dynamic_image == 1) {
		if (http_send_left_image_chunk(conn)) {
			fprintf(stderr,
					"[http_handle_transmit]: Send dynamic image chunk failed\n");
		}
	} else if (conn->dynamic_image == 2) {
		if (http_send_right_image_chunk(conn)) {
			fprintf(stderr,
					"[http_handle_transmit]: Send dynamic image chunk failed\n");
		}
	} else {
		if (http_send_file_chunk(conn)) {
			fprintf(stderr, "[http_handle_transmit]: Send file chunk failed\n");
		}
	}
}

/*
 * WStask()
 * 
 * This MicroC/OS-II thread spins forever after first establishing a listening
 * socket for HTTP connections, binding them, and listening. Once setup,
 * it perpetually waits for incoming data to either a listening socket, or
 * (if there is an active connection), an HTTP data socket. When data arrives, 
 * the approrpriate routine is called to either accept/reject a connection 
 * request, or process incoming data.
 * 
 * This routine calls "select()" to determine which sockets are ready for
 * reading or writing. This, in conjunction with the use of non-blocking 
 * send() and recv() calls and sending responses broken up into chunks lets
 * us handle multiple active HTTP requests.
 */
void WSTask() {
	int i, fd_listen, max_socket;
	struct sockaddr_in addr;
	struct timeval select_timeout;
	fd_set readfds, writefds;
	static http_conn conn[HTTP_NUM_CONNECTIONS];

	/*
	 * Sockets primer...
	 * The socket() call creates an endpoint for TCP of UDP communication. It
	 * returns a descriptor (similar to a file descriptor) that we call fd_listen,
	 * or, "the socket we're listening on for connection requests" in our web
	 * server example.
	 */
	if ((fd_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		die_with_error("[WSTask] Listening socket creation failed");
	}

	/*
	 * Sockets primer, continued...
	 * Calling bind() associates a socket created with socket() to a particular IP
	 * port and incoming address. In this case we're binding to HTTP_PORT and to
	 * INADDR_ANY address (allowing anyone to connect to us. Bind may fail for
	 * various reasons, but the most common is that some other socket is bound to
	 * the port we're requesting.
	 */
	addr.sin_family = AF_INET;
	addr.sin_port = htons(HTTP_PORT);
	addr.sin_addr.s_addr = INADDR_ANY;

	if ((bind(fd_listen,(struct sockaddr *)&addr,sizeof(addr))) < 0) {
		die_with_error("[WSTask] Bind failed");
	}

	/*
	 * Sockets primer, continued...
	 * The listen socket is a socket which is waiting for incoming connections.
	 * This call to listen will block (i.e. not return) until someone tries to
	 * connect to this port.
	 */
	if ((listen(fd_listen,1)) < 0) {
		die_with_error("[WSTask] Listen failed");
	}

	/*
	 * At this point we have successfully created a socket which is listening
	 * on HTTP_PORT for connection requests from any remote address.
	 */
	for (i = 0; i < HTTP_NUM_CONNECTIONS; i++) {
		http_reset_connection(&conn[i], i);
	}

	leftImageSem = OSSemCreate(1);
	rightImageSem = OSSemCreate(1);

	while (1) {
		/*
		 * The select() call below tells the stack to return  from this call
		 * when any of the events we have expressed an interest in happen (it
		 * blocks until our call to select() is satisfied).
		 *
		 * In the call below we're only interested in either someone trying to
		 * connect to us, or when an existing (active) connection has new receive
		 * data, or when an existing connection is in the "DATA" state meaning that
		 * we're in the middle of processing an HTTP request. If none of these
		 * conditions are satisfied, select() blocks until a timeout specified
		 * in the select_timeout struct.
		 *
		 * The sockets we're interested in (for RX) are passed in inside the
		 * readfds parameter, while those we're interested in for TX as passed in
		 * inside the writefds parameter. The format of readfds and writefds is
		 * implementation dependant, hence there are standard macros for
		 * setting/reading the values:
		 *
		 *   FD_ZERO  - Zero's out the sockets we're interested in
		 *   FD_SET   - Adds a socket to those we're interested in
		 *   FD_ISSET - Tests whether the chosen socket is set
		 */
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		FD_SET(fd_listen, &readfds);

		max_socket = fd_listen + 1;

		for (i = 0; i < HTTP_NUM_CONNECTIONS; i++) {
			if (conn[i].fd != -1) {
				/* We're interested in reading any of our active sockets */
				FD_SET(conn[i].fd, &readfds);

				/*
				 * We're interested in writing to any of our active sockets in the DATA
				 * state
				 */
				if (conn[i].state == DATA) {
					FD_SET(conn[i].fd, &writefds);
				}

				/*
				 * select() must be called with the maximum number of sockets to look
				 * through. This will be the largest socket number + 1 (since we start
				 * at zero).
				 */
				if (max_socket <= conn[i].fd) {
					max_socket = conn[i].fd + 1;
				}
			}
		}

		/*
		 * Set timeout value for select. This must be reset for each select()
		 * call.
		 */
		select_timeout.tv_sec = 0;
		select_timeout.tv_usec = 500000;

		select(max_socket, &readfds, &writefds, NULL, &select_timeout);

		/*
		 * If fd_listen (the listening socket we originally created in this thread
		 * is "set" in readfds, then we have an incoming connection request.
		 * We'll call a routine to explicitly accept or deny the incoming connection
		 * request.
		 */
		if (FD_ISSET(fd_listen, &readfds)) {
			http_handle_accept(fd_listen, conn);
		}

		/*
		 * If http_handle_accept() accepts the connection, it creates *another*
		 * socket for sending/receiving data. This socket is independant of the
		 * listening socket we created above. This socket's descriptor is stored
		 * in conn[i].fd. Therefore if conn[i].fd is set in readfs, we have
		 * incoming data for our HTTP server, and we call our receive routine
		 * to process it. Likewise, if conn[i].fd is set in writefds, we have
		 * an open connection that is *capable* of being written to.
		 */
		for (i = 0; i < HTTP_NUM_CONNECTIONS; i++) {
			if (conn[i].fd != -1) {
				if (FD_ISSET(conn[i].fd,&readfds)) {
					http_handle_receive(&conn[i], i);
				}

				if (FD_ISSET(conn[i].fd,&writefds)) {
					http_handle_transmit(&conn[i], i);
				}

				http_manage_connection(&conn[i], i);
			}
		}
	} /* while(1) */
}

#ifdef __cplusplus
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
