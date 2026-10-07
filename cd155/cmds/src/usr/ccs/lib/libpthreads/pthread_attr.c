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
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */

#ifndef	lint
static char rcs_id[] = "$Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libpthreads/pthread_attr.c,v 1.3 1994/11/19 02:33:52 mtm Exp $";
#endif	not lint

/*
 * File: pthread_attr.c
 *
 * Support for thread attribute objects. Currently only the stack size
 * attribute is supported. The remaining attibutes are all scheduling.
 */

#include <pthread.h>
#include <errno.h>
#include "internal.h"

/*
 * Imported Data
 */
extern int	pthread_default_stack_size;

/*
 * Global Data
 */
pthread_attr_t		pthread_attr_default;

/*
 * Function:
 *	pthread_attr_init
 *
 * Description:
 *      This function is called from pthread_init() but after the
 *	call to stack_init() and is used to initialize the default
 *	attribute structure.
 *	
 */
void
pthread_attr_startup()
{
	pthread_attr_default = (pthread_attr_t)pmalloc(sizeof(struct pthread_attr));
	pthread_attr_default->stacksize = pthread_default_stack_size;
	pthread_attr_default->flags = ATTRIBUTE_VALID;
}

/*
 * Function:
 *	pthread_attr_create
 *
 * Parameters:
 *	attr - a pointer to the attribute structure to be created
 *
 * Return value:
 *	0	Success
 *	-1	the pointer to the attribute was invalid (EINVAL)
 *
 * Description:
 *	The structure is created by copying the default attribute structure
 *	into the new attribute.
 *
 */
int
pthread_attr_create(pthread_attr_t *attr)
{
	if (attr == NULL) {
		set_errno(EINVAL);
		return(-1);
	}
	*attr = (pthread_attr_t)pmalloc(sizeof(struct pthread_attr));
	**attr = *pthread_attr_default;
	return(0);
}

/*
 * Function:
 *	pthread_attr_delete
 *
 * Parameters:
 *	attr - a pointer to the attribute structure to be deleted
 *
 * Return value:
 *	0	Success
 *	-1	the pointer to the attribute was invalid (EINVAL)
 *		the attribute was invalid (EINVAL)
 *		the attribute was the default attribute (EINVAL)
 *
 * Description:
 *	attributes are deleted by marking them as invalid. No storage
 *	needs to be reclaimed.
 */
int
pthread_attr_delete(pthread_attr_t *attr)
{
	if ((attr == NULL) || (*attr == NO_ATTRIBUTE) ||
	    (*attr == pthread_attr_default) ||
	    !((*attr)->flags&ATTRIBUTE_VALID)) {
		set_errno(EINVAL);
		return(-1);
	}
	(*attr)->flags &= ~ATTRIBUTE_VALID;;
	pfree(*attr);
	*attr = NO_ATTRIBUTE;
	return(0);
}

/*
 * Function:
 *	pthread_attr_setstacksize
 *
 * Parameters:
 *	attr - a pointer to the attribute to be altered
 *	newsize - the size of the stack to be created when using this attribute
 *
 * Return value:
 *	0	Success
 *	-1	the pointer to the attribute was invalid (EINVAL)
 *		the attribute was invalid (EINVAL)
 *		the attribute was the default attribute (EINVAL)
 *
 * Description:
 *	The stacksize is rounded up to the nearest page so we don't
 *	get silly numbers. The value is a minimum value so when the
 *	stack is allocated the thread may get more than this.
 */
int
pthread_attr_setstacksize(pthread_attr_t *attr, long newsize)
{
	if ((attr == NULL) || (*attr == NO_ATTRIBUTE) ||
	    (*attr == pthread_attr_default) ||
	    !((*attr)->flags&ATTRIBUTE_VALID)) {
		set_errno(EINVAL);
		return(-1);
	}
	/*
	 * round up to the nearest page
	 */
	newsize = (newsize + vm_page_size - 1) & ~(vm_page_size - 1);
	/*
	 * if we have gone negative then the requested size was too big
	 * and so we return an error. Also check that we have enough
	 * to add the size of the red zone without overflowing.
	 */
	if ((newsize + RED_ZONE_SIZE) < 0) {
		set_errno(EINVAL);
		return(-1);
	}
	(*attr)->stacksize = newsize;
	return(0);
}

/*
 * Function:
 *	pthread_attr_getstacksize
 *
 * Parameters:
 *	attr - a pointer to the attribute
 *
 * Return value:
 *	-1	the pointer to the attribute was invalid (EINVAL)
 *		the attribute was invalid (EINVAL)
 *	otherwise the size of stack to be created with this attribute
 *
 * Description:
 *	After checking that we have a real attribute we return whatever
 *	is in the stacksize element.
 */
long
pthread_attr_getstacksize(pthread_attr_t attr)
{
	if ((attr == NO_ATTRIBUTE) || !(attr->flags&ATTRIBUTE_VALID)) {
		set_errno(EINVAL);
		return(-1);
	}
	return(attr->stacksize);
}

/*
 * Function:
 *	pthread_attr_setprio
 *
 * Parameters:
 *	attr - a pointer to the attribute to be altered
 *	priority - the priority a thread is created with using this attribute
 *
 * Return value:
 *	-1	This function is not supported (ENOSYS)
 *
 * Description:
 *	This function is not supported.
 */
int
pthread_attr_setprio(pthread_attr_t *attr, int priority)
{
	set_errno(ENOSYS);
	return(-1);
}

/*
 * Function:
 *	pthread_attr_getprio
 *
 * Parameters:
 *	attr - a pointer to the attribute
 *
 * Return value:
 *	-1	This function is not supported (ENOSYS)
 *
 * Description:
 *	This function is not supported.
 */
int
pthread_attr_getprio(pthread_attr_t attr)
{
	set_errno(ENOSYS);
	return(-1);
}

/*
 * Function:
 *	pthread_attr_setsched
 *
 * Parameters:
 *	attr - a pointer to the attribute to be altered
 *	scheduler - the scheduler policy the thread is created with
 *
 * Return value:
 *	-1	This function is not supported (ENOSYS)
 *
 * Description:
 *	This function is not supported.
 */
int
pthread_attr_setsched(pthread_attr_t *attr, int scheduler)
{
	set_errno(ENOSYS);
	return(-1);
}

/*
 * Function:
 *	pthread_attr_getsched
 *
 * Parameters:
 *	attr - a pointer to the attribute
 *
 * Return value:
 *	-1	This function is not supported (ENOSYS)
 *
 * Description:
 *	This function is not supported.
 */
int
pthread_attr_getsched(pthread_attr_t attr)
{
	set_errno(ENOSYS);
	return(-1);
}

/*
 * Function:
 *	pthread_attr_setinheritsched
 *
 * Parameters:
 *	attr - a pointer to the attribute to be altered
 *	inherit - flag indicating inheritence of scheduling policy
 *
 * Return value:
 *	-1	This function is not supported (ENOSYS)
 *
 * Description:
 *	This function is not supported.
 */
int
pthread_attr_setinheritsched(pthread_attr_t *attr, int inherit)
{
	set_errno(ENOSYS);
	return(-1);
}

/*
 * Function:
 *	pthread_attr_getinheritsched
 *
 * Parameters:
 *	attr - a pointer to the attribute
 *
 * Return value:
 *	-1	This function is not supported (ENOSYS)
 *
 * Description:
 *	This function is not supported.
 */
int
pthread_attr_getinheritsched(pthread_attr_t attr)
{
	set_errno(ENOSYS);
	return(-1);
}
