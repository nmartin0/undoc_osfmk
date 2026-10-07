/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*****************************************************************************
 *              Copyright (c) 1990 San Diego Supercomputer Center.
 *              All rights reserved.  The SDSC software License Agreement
 *              specifies the terms and conditions for redistribution.
 *
 * File:        ports.c
 *
 * Abstract:	This file contains functions which take care of socket 
 *		communications for MACD
 *
 *****************************************************************************/

#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/syslimits.h>
#include <stdio.h>
#include <errno.h>
#include <netdb.h>
#include <fcntl.h>
#include <time.h>
#include <sys/mode.h>
#include <syslog.h>
#include "conf.h"
#include "macd.h"
#include "smd.h"
#include "filename.h"

#define ERROR (-1)
#define OK (0)
#define HOSTNAMESIZE (64)
#define GETNOFILES (4)
#define FDSETNULL ((fd_set *) NULL)
#define TIMENULL ((struct timeval *) NULL)
#define CHECK(X) if ((X) < 0) return(ERROR)

/*
  Global variables used in this file - socket ports and set flags
*/

static int acceptsock;
static fd_set readsocks;
static unsigned long local_address;
static unsigned long our_address;

/***********************************************************************
*
* smOpen()
*
* Abstract:	This function open a connection to scheduler monitor daemon
*		The socket number of this connection is stored in a global
*		variable sm_sock.  It will be -1 when the connection is
*		down.  When the connection is successfully established,
*		it will send an event-request message to SMD, indicates
*		MACD's interest in all application start and end events
*		over its entire run.
*
* Arguments:	None
*
* Return value: 0 -	successful
*		-1 -	error
*
************************************************************************/

int smOpen()
{
	struct smd_req reqst;
	int rval, msglen;
#ifdef SMD_IP
        struct sockaddr_in addr;
        struct hostent *hp;
#else
	struct sockaddr_un addr;
#endif
	char *ptr, lhost[32];
	extern int sm_sock;
	extern int sm_conn;
	extern int _debug_;
	extern char *timstr();
	extern char *strcpy();
	extern void bcopy();
	extern void bzero();
	extern u_short htons();

    if (_debug_) {
	(void) fprintf (stderr, "Enter smOpen()\n");
	(void) fflush (stderr);
    }

/*
 * No more than one connection per client
 */
	if (sm_sock > 0) {
		errno = EISCONN;
		return(-1);
	}

/*
 * Open a socket, bind it with a previledged port then connect to
 * scheduler monitor
 */
#ifdef SMD_IP
        sm_sock = socket(AF_INET, SOCK_STREAM, 0);
#else
	sm_sock = socket(AF_UNIX, SOCK_STREAM, 0);
#endif
        if (sm_sock < 0) {
                if (!_debug_) return (-1);
                if (errno == EAGAIN)
                        (void) fprintf(stderr, "Socket: All ports in use\n");
                else
                        perror("c_open: socket");
		(void) fflush (stderr);
        }

#ifdef SMD_IP
	bzero ((char *)&addr, sizeof (struct sockaddr_in));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(SMD_IFACE_PORT);
	(void) strcpy (lhost, "localhost");
	CHECK (gethostname (lhost, 32));
        hp = gethostbyname(lhost);
        (void) bcopy(hp->h_addr, &(addr.sin_addr.s_addr), hp->h_length);
/*
   	CHECK(bind(sm_sock, &addr, sizeof(addr)));
*/
#else
	bzero ((char *)&addr, sizeof (struct sockaddr_un));
	addr.sun_family = AF_UNIX;
        (void) strcpy (addr.sun_path, SMD_IFACE_NAME);
#endif
        if (connect(sm_sock, (struct sockaddr *)&addr, sizeof (addr)) < 0) {
        	(void) close(sm_sock);
        	sm_sock = -1;
	        if (_debug_) perror(hp->h_name);
	        return (-1);
	}


/*
 * Pack a event request messge and send to scheduler monitor
 */
	msglen = sizeof (struct smd_req);
	(void) bzero ((char *)&reqst, msglen);
	reqst.hdr.type = SMD_EVENT_REQ;
	reqst.hdr.len = msglen - sizeof(int)*2;
	reqst.q_id.pgid = QUALIFY_ANY;
	reqst.q_id.part_id = QUALIFY_ANY;
	reqst.q_id.acct_id = QUALIFY_ANY;
	reqst.q_id.uid = QUALIFY_ANY;
	bzero (reqst.q_id.req_id, SMD_REQ_ID_LEN);
	(void) strcpy (reqst.q_id.req_id, "macd");
	reqst.param.flags = PERSISTENT;
	reqst.param.events = APP_START | APP_END;
	for (ptr=(char *)&reqst; msglen>0; msglen-=rval, ptr+=rval) {
                rval = write(sm_sock, ptr, msglen);
                if (rval <= 0) {
			errno = EPIPE;
			return (-1);
		}
        }

	sm_conn = UP;
        (void) printf("GOTSMD   : %s - SMD comes back up\n", timstr(0));
	FD_SET(sm_sock, &readsocks);
	return(0);
}


/*****************************************************************************
*
* portOpen()
*
* Abstract:	This function opens up a port that clients can connect to.  
*		The port is a privileged port.  This function should be 
*		called before any other functions in this file.
*
* Arguments:	None
*
* Return value:	0 -	successful
*		-1 -	error
*
*****************************************************************************/

int portOpen()

{
   struct sockaddr_in macd_port;
   struct hostent *he;
   int on = TRUE;
   char host[HOSTNAMESIZE];
   extern int sm_sock;
   extern u_short htons();

   /*
     Set up the macd_port address and open up the socket and bind address
   */

   bzero ((char *)&macd_port, sizeof (struct sockaddr_in));
   macd_port.sin_family = AF_INET;
   macd_port.sin_addr.s_addr = INADDR_ANY;
   macd_port.sin_port = htons(MACD_PORT);
   CHECK(acceptsock = socket(AF_INET, SOCK_STREAM, 0));
   CHECK(setsockopt(acceptsock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)));
   CHECK(bind(acceptsock, (struct sockaddr *)&macd_port, sizeof(macd_port)));
   CHECK(listen(acceptsock, SOMAXCONN));
   CHECK(fcntl(acceptsock, F_SETFL, O_NDELAY));

   /*
     Set up the set of readable socket names in fd_set readsocks
   */

   FD_ZERO(&readsocks);
   if (sm_sock != -1) FD_SET(sm_sock, &readsocks);
   FD_SET(acceptsock, &readsocks);

   /*
     Get the address of the local host - allow only local connections
   */

   if ((he = gethostbyname("localhost")) == (struct hostent *) NULL) 
	return(ERROR);
   local_address = ntohl(*((unsigned long *) he->h_addr_list[0]));
   CHECK(gethostname(host, HOSTNAMESIZE));
   if ((he = gethostbyname(host)) == (struct hostent *) NULL) return(ERROR);
   our_address = ntohl(*((unsigned long *) he->h_addr_list[0]));
   return(OK);
}

/*****************************************************************************
*
* _portWrite()
*
* Abstract:	This routine takes care of writing all of the data to 
*		the socket.
*
* Arguments:	socket -	socket number to write to
*		data -	data to write
*		size -	size of data to write
*
* Return value:	>=0 -	number of bytes written
*		-1 -	error
*
*****************************************************************************/

static int _portWrite(socket, data, size)
int socket;
char *data;
int size;

{
   int written, chunk;
   extern int _debug_;

   /*
     Loop until all of the data has been written to the socket
   */

   for (written = 0; size != 0; written += chunk, size -= chunk)
      if ((chunk = write(socket, &data[written], size)) < 0)
         return(ERROR);
   return(written);
}

/*****************************************************************************
*
* portReply()
*
* Abstract:	This routine sends a reply message to the specified 
*		socket number.   All errors are ignored, since the 
*		client may have already disappeared.
*
* Arguments:	socket -	socket number to send the message
*		returnvalue -	return value from request handler to client
*		error -	errno if the return value is -1
*		size - data size
*		data - reply data to client
*
* Return value:	None
*
*****************************************************************************/

void portReply(socket, returnvalue, error, size, data)
int socket;
int returnvalue;
int error;
int size;
char *data;

{
   struct macd_reply reply;
   extern int _debug_;

    if (_debug_) {
	(void) fprintf (stderr, 
	    "Enter portReply(socket=%d, returnvalue=%d, error=%d, size=%d, data=%d)\n",
	    socket, returnvalue, error, size, data);
	(void) fflush (stderr);
    }

   /*
     Fill in the reply structure and send the data back
     If there are any errors, then the client may have disappeared
   */

   reply.returnvalue = returnvalue;
   reply.errno = error;
   reply.size = size;
   if ((_portWrite(socket, (char *) &reply, sizeof(reply)) < 0) ) {
      FD_CLR(socket, &readsocks);
      close(socket);
   }
   else if (data == NULL && size > 0) {
      (void) printf("WARNING  : portReply, data=NULL and size=%d\n", size);
      FD_CLR(socket, &readsocks);
      close(socket);
   }
   else if (data!=NULL && _portWrite(socket, data, size) < 0) {
      FD_CLR(socket, &readsocks);
      close(socket);
   }
}

/*****************************************************************************
*
* _portRead()
*
* Abstract:	This routine reads data into the data array from 
* 		the specified socket.
*
* Arguments:	socket -	socket number from which to read
*		data -	memory address where to store data read in
*		size -	size of data to read
*
* Return value: >=0	the number of bytes read
*		-1	error
*
*****************************************************************************/

static int _portRead(socket, data, size)
int socket;
char *data;
int size;

{
   int readsofar, chunk;
   extern int _debug_;

    if (_debug_) {
	(void) fprintf (stderr, 
	    "Enter _portRead(socket=%d, data=%d, size=%d)\n",
	    socket, data, size);
	(void) fflush (stderr);
    }

   /*
     Loop until all of the data has been read from the socket
   */

   for (readsofar = 0; size != 0; readsofar += chunk, size -= chunk) {
      if ((chunk = read(socket, &data[readsofar], size)) <= 0) {
         if (errno == EWOULDBLOCK) {
            (void) sleep(1);
            chunk = 0;
         }
         else return(ERROR);
      }
   }
   return(readsofar);
}

/*****************************************************************************
*
* _portAccept()	
*
* Abstract:	This routine accepts a connection on port acceptsock.  If the 
* 		connection is OK, then we add it to the readsocks fd_set.
*
* Arguments:	None
*
* Return value: None
*
*****************************************************************************/

void _portAccept()

{
   int connect, length;
   struct sockaddr_in who;
   extern u_short ntohs();

   /*
     Get ready to accept the connection from acceptsock
   */

   length = sizeof(who);
   if ((connect = accept(acceptsock, (struct sockaddr *)&who, &length)) < 0)
      (void) printf("WARNING  : Unable to accept() connection - %d\n", errno);
   else if (ntohs(who.sin_port) >= IPPORT_RESERVED) {
      (void) printf("WARNING  : Rejected non-privileged port %d\n", 
		ntohs(who.sin_port));
      (void) portReply(connect, ERROR, EACCES, 0, NULL);
      (void) close(connect);
   }
   else if ((ntohl(who.sin_addr.s_addr) != local_address) &&
   	(ntohl(who.sin_addr.s_addr) != our_address)) {
      (void) printf("WARNING  : Rejected remote host %08x\n", 
		ntohl(who.sin_addr.s_addr));
      (void) portReply(connect, ERROR, EACCES, 0, NULL);
      (void) close(connect);
   }
   else {
      FD_SET(connect, &readsocks);
/*
      (void) printf("MACDINFO : IP address validated %08x\n",
		ntohl(who.sin_addr.s_addr));
*/
   }
}

/*****************************************************************************
*
* portRequest()
*
* Abstract:	This routine detects events on open sockets and process
*		then one-by-one. It also sets timer for and take care of
*		following periodic operations:
*		1.  switch log file if the time comes
*		2.  issue a status request to SMD every SYNC_INTERVAL
*		3.  write entire macd databaset to disk
*		When an event is detected on the socket for accepting
*		a client connection, this routine will call portAccept()
*		to handle it; if an event is detected on other socket
*		this routine will call portRead() to read in-coming data.
*		Only when the read is successful, the routine will break
*		out its event-detection loop and return to its caller --
*		the MACD main routine.
*
* Arguments:	request -	data structure to store in-coming message
*
* Return value: None
*
*****************************************************************************/

void portRequest(request)
struct msg_in *request;

{
   fd_set readysocks;
   int maxsocks, s, n, i;
   struct timeval sync_time;
   int pret;
   time_t now;
   struct tm nowtm;
   static int reported=0;
   extern int sm_sock, tlastsync, _debug_;
   extern struct macsconf *conf;
   extern struct tm tlastlog;
   extern char *timstr();
   extern char *malloc();
   extern void newlog();

   int lcm = conf->switchlog;
   struct tm *llc = &tlastlog;

    if (_debug_) { 
	(void) fprintf (stderr, "Enter portRequest(request=%d)\n", request);
	(void) fflush (stderr);
    }

   /*
     Wait until somebody wakes us up with a connection or some data
     or time-out to issue status request to SMD
   */

   sync_time.tv_usec = 0;
   if ((maxsocks = ulimit(GETNOFILES, 0)) < 0) maxsocks = OPEN_MAX;
   while (TRUE) {
      now = time(0);
      nowtm = *(localtime(&now));

      /*
        touch the heart-beat file
      */
      if (utime (HEART_BEAT_FNAME, NULL) != 0) {
          if (errno == ENOENT && creat (HEART_BEAT_FNAME, S_IRWXU) >= 0) {
               reported = 0;
          }
          else if (!reported) {
               char log_msg[128];
               (void) sprintf(log_msg, "Cannot touch %s, errno=%d\n",
                       HEART_BEAT_FNAME, errno);
               (void) syslog (LOG_ERR, log_msg);
               (void) _sendmail (ERR_MACHEART, NULL);
	       reported = 1;
          }
      }
      else if (reported) reported = 0;

      if (((lcm == LOGDAILY  ) && (nowtm.tm_yday != llc->tm_yday)) ||
          ((lcm == LOGWEEKLY ) && (nowtm.tm_wday == 0) && 
	  (nowtm.tm_yday != llc->tm_yday)) ||
          ((lcm == LOGMONTHLY) && (nowtm.tm_mon != llc->tm_mon))) 
         (void) newlog(now, &nowtm);

      if (now >= (tlastsync + conf->sync_interval*60)) {
         (void) smStatuReq();
	 if (conf->macdmode != ACCTONLY) {
	    if (db_writeall() < 0)
	        pret = printf("MACDSYNC : %s - Error in flushing database errno %d\n",
                timstr(0), errno);
             else pret = printf("MACDSYNC : %s - Flushed database\n", timstr(0));
	 }
         else pret = printf("MACDSTAT : %s - Status request to SMD\n", timstr(0));

	 /* Fix for PTS 8753: "MACS doesn't notice when it can no longer 
	    write logfiles".  A full solution would involve checking the
	    return value of every printf() in MACS.  This is a compromise:
	    check the return value of the printf() that occurs each
	    sync_interval.

	    Note: The value of errno is not included in the error
	    message because it appears to be random.  Not sure why. */

	 if(pret < 0) {
		char log_msg[128]; 
		(void) sprintf(log_msg,
		    "Write to MACS logfile in directory %s failed! (File system full?)\n",
		    MACD_LOG_PATH); (void)
		ttywrite(conf->operator, log_msg); 
		(void) syslog (LOG_ERR, log_msg); 
		(void) _sendmail (ERR_MACLOG, NULL); 
	 }

         (void) fflush(stdout);
	 tlastsync = now;
      }
      sync_time.tv_sec = 60;

      readysocks = readsocks;
    if (_debug_) {
	for (i=0; i<maxsocks; i++) if (FD_ISSET(i, &readysocks))
	    (void) fprintf (stderr, "Before readysocks bit %d set\n", i);
	(void) fflush (stderr);
    }
      n = select(maxsocks, &readysocks, FDSETNULL, FDSETNULL, &sync_time);
    if (_debug_) { 
	(void) fprintf (stderr, "Select return %d, sm_sock=%d\n", n, sm_sock);
	(void) fflush (stderr);
	for (i=0; i<maxsocks; i++) if (FD_ISSET(i, &readysocks))
		(void) fprintf (stderr, "After readysocks bit %d set\n", i);
	(void) fflush (stderr);
    }
      /* time-out, issue status request to SMD */
      if (n == 0) continue;

      if (n < 0) {
         (void) printf("WARNING : Error in select()ing - %d\n", errno);
         if (errno == EBADF) {
            for (s = 0; (s < maxsocks) && (!FD_ISSET(s, &readysocks)) && (s != acceptsock); s++);
            if (s < maxsocks) {
               FD_CLR(s, &readsocks);
               (void) close(s);
	       if (s == sm_sock) sm_sock = -1;
            }
            else {
               FD_CLR(acceptsock, &readsocks);
               (void) close(acceptsock);
               while (portOpen() < 0) {
                  (void) printf("WARNING  : Cannot open up server socket - %d\n", errno);
                  (void) sleep(1);
               }
            }
         }
      }
      else if (FD_ISSET(acceptsock, &readysocks))
         (void) _portAccept();
      else {
         for (s = 0; (s < maxsocks) && (!FD_ISSET(s, &readysocks)); s++);
         request->socket = s;
         if (_portRead(s, header(request), C_REQUEST_HDRLEN) == ERROR) {
            FD_CLR(s, &readsocks);
            (void) close(s);
	    if (s == sm_sock) sm_sock = -1;
         }
	 else if (request->size <= 0) return;
	 else if ((request->data = (char *)malloc(request->size)) == NULL) {
	    (void) printf("WARNING  : %s - Cannot allocate memory size=%d\n", 
		timstr(0), request->size);
	    (void) fflush (stdout);
            (void) sleep(1);
            FD_CLR(s, &readsocks);
            (void) close(s);
	    if (s == sm_sock) sm_sock = -1;
         }
	 else if (_portRead(s, request->data, request->size) == ERROR) {
	    (void) free (request->data);
            FD_CLR(s, &readsocks);
            (void) close(s);
	    if (s == sm_sock) sm_sock = -1;
         }
         else return;
      }
   }
}

/*****************************************************************************
*
* portCloseConnection()
*
* Abstract:	This routine closes the specified socket and clears its
* 		bit in the readsocks fd_set.
*
* Arguments:	socket -	socket number to close
*
* Return value:	None
*
*****************************************************************************/

void portCloseConnection(socket)
int socket;

{
   (void) close(socket);
   FD_CLR(socket, &readsocks);
}

/*****************************************************************************
*
* portClose()
*	
* Abstract:	This routine closes all open sockets.	
*
* Arguments:	None
*
* Return value: None
*
*****************************************************************************/

void portClose()

{
   int maxsocks, s;

   if ((maxsocks = ulimit(GETNOFILES, 0)) < 0) maxsocks = OPEN_MAX;
   for (s = 0; s < maxsocks; s++)
      if (FD_ISSET(s, &readsocks)) {
         (void) close(s);
         FD_CLR(s, &readsocks);
      }
}
