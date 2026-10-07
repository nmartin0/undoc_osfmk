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
*
* ports.h
*
* This file defines function prototypes available in ports.c
*
*****************************************************************************/

void portReply();		/* send a response back to a client */
void portCloseConnection();	/* close a particular socket number */
void portRequest();		/* get a request from a client */
int portOpen();			/* open up the sockets for connections */
void portClose();		/* close up the server accept socket */
