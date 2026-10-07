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
 
/*
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/include/nx/list.h,v 1.5 1994/11/19 02:44:45 mtm Exp $
 *
 */


/*
 * list.h	- Don Cameron	3/92
 *
 * This file contains generic linked list macros for inserting and
 * deleting items from a doubly linked list.
 */
#ifndef	_list_h
#define	_list_h

/*
 * Generic description of structure for use by generic linked list macros.
 */
typedef struct list { 
        struct list	*prev; 
        struct list	*next; 
} LIST_T;

/*
 * Insert element "a" after element "b"
 */
#define	INSERT_AFTER(a, b)	{if ((b)->next)				\
					(b)->next->prev = (void *) (a);	\
				(a)->next = (void *) (b)->next;		\
				(a)->prev = (void *) (b);		\
				(b)->next = (void *) (a);}   

/*
 * Remove element "a" from list pointed to by hd.
*/
#define	REMOVE(hd, a)	{if ((a)->next)					\
				(a)->next->prev = (void *) (a)->prev;	\
			if ((LIST_T *) (hd) == (LIST_T *) (a))		\
				(hd) = (void *) (a)->next;		\
			if ((a)->prev)					\
		  		(a)->prev->next = (void *) (a)->next;	\
			(a)->next = (void *)0;					\
			(a)->prev = (void *)0;}

/*
 * Insert element "a" at beginning of list pointed to by "hd".
 */
#define	INSERT_FIRST(hd, a)	{(a)->prev = (void *) 0;			\
				(a)->next = (void *) (hd);		\
				if (hd)					\
					(hd)->prev = (void *) (a);	\
				(hd) = (void *) (a);}

#endif	/* !_list_h */
