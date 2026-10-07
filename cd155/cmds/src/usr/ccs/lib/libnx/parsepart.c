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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/parsepart.c,v 1.8 1994/11/19 02:32:10 mtm Exp $
 *
 */

#include <sys/types.h>
#include <stdio.h>
#include <string.h>

#include <mcmsg/mcmsg_appl.h>
#include <nx/allocator.h>
#include <nx/bitmap.h>
#include <errno.h>
#include <nx/defines.h>

int lbase = 0;            /* Indicates if the base node is starting_node of
                              a rectangle or otherwise */
short partsize = 0;       /* Size of the partition */
int ndlist_cntr = 0;      /* Index to the logical node list */

#define	NODE_QUANTUM	64

/*	Add a node to the node list 
 *
 *	Parameters:
 *		node		integer node value to add to list
 *		list_index	pointer to integer representing the
 *				number of nodes currently in the list
 *		node_list	pointer to buffer containing the node
 *				list
 *		node_list_size	pointer to the integer containing the
 *				current size of the node list
 *		
 * 	The node list is allocated in chunks of NODE_QUANTUM number of
 *	nodes at a time.
 *
 *	Returns:
 *		0	Node was properly added to the node list
 *			The value of list_index will be incremented.
 *			A new buffer space may be allocated with the
 *			current contents of the old node list being
 *			being copied over if list_index is greater than
 *			or equal to node_list_size
 *		-1	otherwise and errno will be set appropriately
 *
 *
*/
int
add_node_to_list(int node,int *list_index,
			LP_MAP_T *node_list,int *node_list_size)
{
	int		num_in_list;
	LP_MAP_T	tmp;
	int		error;

	error = 0;
	if ( (*list_index >= *node_list_size) || (*node_list_size == 0)) {
		tmp = *node_list;
		*node_list = (LP_MAP_T)malloc((*node_list_size + NODE_QUANTUM) * sizeof(LP_MAP_ENTRY_T));
		if ( *node_list == (LP_MAP_T)0){
			error = -1;
			errno = ENOMEM;
			goto done;
		}
		if (*node_list_size > 0)
			bcopy(tmp,*node_list,*node_list_size * sizeof(LP_MAP_ENTRY_T));
		free(tmp);
		*node_list_size += NODE_QUANTUM;
	}
	(*node_list)[*list_index] = (LP_MAP_ENTRY_T ) node;
	(*list_index)++;
done:
	return(error);
}

/************************** get_rect  ****************************************
 *
 *      Calling Sequence:
 *             get_rect(adescr);
 *
 *      Description:
 *              This routine takes in a description of the form aXb:c,d,fg.....
 *              and returns aXb:c.
 *
 *      Parameters:
 *              descr: Char pointer to the description
 *
 *      Returns:
 *               Char pointer
 *
 *
*/

static char *
get_rect(adescr)
char *adescr;
{
    char *descr;
    char *tmpptr,*preptr;         
  
#ifdef DEBUG
printf("get_rect : Entered. Argument passed %s\n",adescr);
#endif

    /*
     * Make a local copy of the description
     */

	descr = (char *)malloc(strlen(adescr) + 1);
	if( descr == (char *) 0) {
		errno = ENOMEM;
		goto error;
	}

	strncpy(descr,adescr,strlen(adescr));
    
	preptr = descr;
	while((tmpptr = strpbrk(preptr,",Xx")) != NULL ){
		if ( *tmpptr == 'X'  || *tmpptr == 'x' ){
			descr = preptr;
			while( *tmpptr != ',' && *tmpptr != '\0' && *tmpptr != ' ')
				tmpptr++;
				*tmpptr = '\0';
				return descr;
		}
		else{
			preptr = tmpptr;
			preptr++;
		}
	}

	errno = EPBADNODE;
error:
	return(NULL);
}

/************************** parse_grid ********************************
 *
 *      Description:
 *            parse_grid() takes in a description of the form aXb and sets
 *            the rows and the columns field in structure rect. 
 *
 *      Parameters:
 *              descr: Char pointer to the description of the form aXb
 *              node_list: pointer to node list
 *		ndlist_index : pointer to an integer indicated the next 
 *				position to fill in the node list
 *
 *      Returns:
 *		0 == all is ok
 *		-1 == error occured and errno will be set
 *
 *
*/

static int
parse_grid(descr,node_list,node_list_size,ndlist_index)
char		*descr;
LP_MAP_T	*node_list;
int		*node_list_size;
int		*ndlist_index;
{
	char *rdesc;      /* Local copy of the description */
	char *buf;  /* Temporary buffers */
	int	error;
	int	rows,cols;

	error = 0;
#ifdef DEBUG
printf("parse_grid : Entered. Argument passed %s\n",descr);
#endif
        
	/*
	* Make a copy of the description
	*/
       
	rdesc = (char *)malloc(strlen(descr) + 1 );
	if( rdesc == (char *) 0) {
		errno = ENOMEM;
		goto bad_error;
	}
	strncpy(rdesc,descr,strlen(descr));

	buf = strtok(rdesc,"Xx");
	rows = atoi(buf);

	buf = strtok(NULL,",:");
	cols = atoi(buf);

	if(( rows == 0 ) || ( cols == 0)) {
		errno = EPBADNODE;
		goto bad_error;
	}


	if (add_node_to_list(-rows,ndlist_index,node_list,node_list_size) == -1)
		goto bad_error;
	if (add_node_to_list(-cols,ndlist_index,node_list,node_list_size) == -1)
		goto bad_error;

	return(0);
bad_error:
	return(-1);
} 

/************************** parse_rect ********************************
 *
 *      Calling Sequence:
 *             parse_rect(descr,rect);
 *
 *      Description:
 *             This function is called if the argument to -nd has description 
 *             of a rectangle in aXb:c form. The description is parsed to set 
 *             the appropriate nodes in the logical node list. Since we cannot
 *             find logical number of the nodes in the row a+1, we just set it
 *             to an invalid value (RECTSET). While creating the bitmap,
 *             this value will be decoded and will be replaced by correct 
 *             logical node number.
 *
 *      Parameters:
 *              sdesc: Char pointer to the argument to -nd
 *              rect:  pointer to type RECT_T.
 *
 *      Returns:
 *               status
 *
 *
 */

static int
parse_rect(descr,l_ndlist,node_list_size,ndlist_index)
char		*descr;
LP_MAP_T	*l_ndlist;
int		*node_list_size;
int		*ndlist_index;
{

	register i,j;

	long status;
	char *rdesc;          /* Description of the rectangle */
	char *buf,*buf2;      /* Temorary Buffer */
	short snode;            /* Starting_node in aXb:starting_node */

#ifdef DEBUG
printf("parse_rect : Entered : Argument passed %s\n",descr);
#endif

	/*
	* Parse the description to get the rectangle
	*/

	rdesc = get_rect(descr);
	if(rdesc == NULL)
		goto error;

	/*
	* Get the starting_node from the description
	*/
  
	if(( buf = strpbrk(rdesc,":")) == NULL ) {
		errno = EPBADNODE;
		goto error;
	}
	else{
		buf++;
		snode = atoi(buf);
	}

	/*
	* Get the rows and the columns from the description
	*/

	if( parse_grid(rdesc,l_ndlist,node_list_size,ndlist_index ) < 0 ) {
		errno = EPBADNODE;
		goto error;
	}

	/*
	* Add the start node to the logical list.
	*/

	if (add_node_to_list(-snode,ndlist_index,l_ndlist,node_list_size) == -1)
		goto error;

	return(0); 

error:
	return (-1);

}

/************************** parse_sdesc ********************************
 *
 *      Calling Sequence:
 *             parse_sdesc(sdesc); 
 *
 *      Description:
 *             parse_sdesc takes in the argument to the flag -sz of
 *             the command mkpart and breaks it to calculate the size 
 *             of the partition and also set the rows and the columns 
 *             of the rectangular partition in case argument was in the
 *             form axb.
 *
 *      Parameters:
 *              sdesc: Char pointer to the argument to flag -sz
 *
 *      Returns:
 *               status
 *
 *
 */

int
parse_sdesc(char *sdesc,LP_MAP_T *node_list)
{
	register i;

	char	*buf;	/* Temorary buffer */
	long	status;	/* Status */
	RECT_T	rect;	/* structure for rectangular specification */
	int	spec;	/* part of the spec */
	int	node_list_size;
	

	node_list_size = 0;
	/* See if there is any : in the description. A colon is only allowed
	 * for the -nd option
	*/
	if ( (buf = strpbrk(sdesc,":")) != NULL){
		errno = USAGE_ERROR;
		goto error;
	}
#ifdef DEBUG
printf("parse_sdesc entered. Argument passed %s\n",sdesc);
#endif
  
	/*
	* The description may or may not have a rectangular specification
	* In either case we need to set the parameters to default value
	*/
  
	buf = sdesc;
        
	spec = atoi(sdesc);

	/*
	* The description must start with numerals
	*/

#ifdef DEBUG
printf("part size %d sdesc %s \n",partsize,sdesc);
#endif
	if( spec <= 0 ) { 
		errno = EPBADNODE;
		goto error;
	}
           
	while ( *sdesc >= '0' &&  *sdesc <= '9' ) 
		sdesc++;
	/*
	* If the description has an 'X' or an 'x', the request is
	* for a rectangular partition.  
	*/

	if( *sdesc == 'X' || *sdesc == 'x' ) { 
		sdesc = buf; 
		if( parse_grid(sdesc,node_list,&node_list_size,&ndlist_cntr) < 0 ) {
			errno = EPBADNODE;
			goto error;
		}
	}
	else
	if( *sdesc != '\0' ) {
		errno = USAGE_ERROR;
		goto error;
	}
	else
		ndlist_cntr = spec;
   
	return(ndlist_cntr);

error:
	return(0);

}
   

static void
error_invtype()
{
 fprintf(stderr, "invalid size \n");
}

/*************************** set_lbase ****************************/
static void
set_lbase(andesc)
char *andesc;
{

	char *tmpptr;
	tmpptr = strpbrk(andesc,".,Xx");
	if ( tmpptr == NULL ) {
		lbase = -1;
		return;
	}
	if ( *tmpptr == 'X' || *tmpptr == 'x' )      
		lbase = 1;
	else 
		lbase = 0; 
	return;
}

/************************** parse_ndesc ********************************
 *
 *      Description:
 *            parse_ndesc is called when the user uses -nd option flag
 *            as an argument to mkpart. The argument is parsed to set
 *            the size and sets the dimensions of the rectangle in case
 *            the user asked for nodes in aXb:c format.
 *
 *      Parameters:
 *              sdesc: Char pointer to the argument to flag -nd
 *              l_ndlist: pointer to node list to be allocated and filled in
 *      Returns:
 *               Number of nodes added to the node list, a zero value
 *		 indicates that there was an error and errno will be set
 *
*/

int
parse_ndesc(char *andesc,LP_MAP_T *l_ndlist)
{
	int 	tmpi,i,j;
	char 	*ndesc;            /* Local copy of the argument to -nd */
	char 	nodenum[4];        /* An array of digits that belong to an integer */
	int 	index;             /* Index to nodenum */
	char 	*preptr;           /* Temporary pointer */
	short 	bgrange;           /* Element a in a..b */
	short 	endrange;          /* Element b in a..b */
	int 	setrange;          /* Indicates that the previous entry in the 
			              description was a range */
	int 	setrect = 0;       /* Indicates that the previous entry in the 
                               	      description was a rectangle */
	int 	inc;               /* Gets set to 1 when the end of the description 
                               is reached */
	long 	status;
 	int	node_list_size; 


	node_list_size = 0;
#ifdef DEBUG
printf("parse_ndesc: Entered with desc %s nodes = %d\n",andesc,part.nodes);
#endif

	/*
	* Make a null terminated copy of the description for local use
	*/
   
	ndesc = (char *)malloc(strlen(andesc) + 1);
	if( ndesc == (char *) 0 ) {
		goto error;
	}
	strncpy(ndesc,andesc,strlen(andesc) + 1);
        
	/*
	* Find if the base node is a start point of a rectangle or the first
	* node in nodelist 
	*/
   
	set_lbase(ndesc);

	index = 0; i = 0; bgrange = 0;endrange = 0; setrange = 0;
	inc = 0;
	preptr = ndesc;
	while (*ndesc) {
		switch(*ndesc) {
		case ' ':
			ndesc++;
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			/*
			* Create an integer where index will show the position of 
			* the digit in the integer
			*/
			if( index > 3) {
				errno = EPBADNODE;
				goto error;
			}
			nodenum[index] = *ndesc;
			ndesc++;
			index++;
			break;
		case ',' :
			/*
			* If , the following value could be another element in the
			* nodelist or specification for a rectangle
			*/
			if (setrect == 1) 
				setrect = 0;
			else {
				nodenum[index] = '\0';
                           	 
				/* 
				* If the previous characters were .., the request
				* is for a range of the form a..b. Set the
				* logical nodes accordingly
				*/
		
				if (setrange == 1){
					endrange = atoi(nodenum);
					if  ( endrange < bgrange )
						for (j = bgrange - 1 ; j >= endrange ; j-- ){ 
							if (add_node_to_list(j,&ndlist_cntr,
								l_ndlist,&node_list_size) == -1)
								goto error;
						}
					else 
						for (j = bgrange + 1 ; j <= endrange ; j++ ) 
							if (add_node_to_list(j,&ndlist_cntr,
								l_ndlist,&node_list_size) == -1)
								goto error;
					setrange = 0;
				}
				else 
					/* Just a node number */
					if (add_node_to_list(atoi(nodenum),&ndlist_cntr,
							l_ndlist,&node_list_size) == -1)
						goto error;
			}
			ndesc++;
			index = 0; 
			preptr = ndesc;
                     
			/*
			* Character after the , must be a number
			*/
			if (  ndesc == NULL || *ndesc < '0' || *ndesc > '9' ) {
				errno = EPBADNODE;
				goto error;
			}
			break;

		case 'X' :
		case 'x' :
		/*
		* The user wants a rectangular partition.
		*/ 

			status = parse_rect(preptr,l_ndlist,&node_list_size,
							&ndlist_cntr);
			if( status < 0)
				goto error;

			/*
			* Jump to the , after the description of the rectangle 
			*/
			if((andesc = strpbrk(ndesc,",")) == NULL) 
				inc++;
			else {
				ndesc = andesc;
			}
			setrect = 1; 
			break;
		case '.' :
			/*
			* Argument has description of a range. 
			*/
			nodenum[index] = '\0';
			if (add_node_to_list(atoi(nodenum),&ndlist_cntr,
					l_ndlist,&node_list_size) == -1)
				goto error;
			bgrange = (*l_ndlist)[ndlist_cntr-1];
			ndesc++;
			if (*ndesc == '.') {
				setrange = 1;
				ndesc++; 
				index = 0;
			} else {
				/*
				* One . must be followed by another .
				*/
				errno = USAGE_ERROR;
				goto error;
			}
			break;
		default:
			errno = USAGE_ERROR;
			goto error;
		} /* switch */

		if (inc)
			break;
	} /* while */

	/*
	* Get the last number in the logical list. Do not add to the logical 
	* list if the previous entry 
	* was a part of the description for a rectangle or a range
	*/

	if(setrect != 1 &&  setrange != 1){
		nodenum[index] = '\0';
		if (add_node_to_list(atoi(nodenum),&ndlist_cntr,
					l_ndlist,&node_list_size) == -1)
			goto error;
	}

	/*
	* If the last entry was a part of description of a range, 
	* set the logical list accordingly
	*/
	if (setrange == 1){
		nodenum[index] = '\0';
		endrange = (short)atoi(nodenum);

		if  ( endrange < bgrange ){
			for (j = bgrange - 1 ; j >= endrange ; j-- ){
				if (add_node_to_list(j,&ndlist_cntr,l_ndlist,
							&node_list_size) == -1)
					goto error;
			}
		} else {
			for (j = bgrange + 1 ; j <= endrange ; j++ ){
				if (add_node_to_list(j,&ndlist_cntr,l_ndlist,
							&node_list_size) == -1)
					goto error;
			}
		}
	}

	return(ndlist_cntr);

error:
	return(0);

}
