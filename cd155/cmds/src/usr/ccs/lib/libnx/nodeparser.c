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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/nodeparser.c,v 1.5 1994/11/19 02:31:36 mtm Exp $
 *
 * History:$
 */

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <mcmsg/mcmsg_appl.h>
#include <stdio.h>
#include <sys/types.h>
#include <strings.h>
#include <errno.h>
#include <nx/h.h>
#include <nx/defines.h>


int
isdigit(c)
char c;
{
	if ( (c >= '0') && (c <= '9') )
		return TRUE;
	else
		return FALSE;
}

/* State of the machine */
#define STATE_START     0
#define STATE_NUM1      (STATE_START+1)
#define STATE_NUM2      (STATE_NUM1+1)
#define STATE_SYM       (STATE_NUM2+1)
#define STATE_RANGE     (STATE_SYM+1)
#define	STATE_DOT1	(STATE_RANGE+1)
#define	STATE_DOT2	(STATE_DOT1+1)
#define STATE_COMMA	(STATE_DOT2+1)
#define STATE_ANY	(STATE_COMMA+1)
#define STATE_COMMADOT	(STATE_ANY+1)

/*	nodeparser
 *
 *	Parameters:
 *		descr	character pointer to string to be parsed
 *		node_array 	pointer to node array
 *	Description:
 *		This parser accepts node lists of the following form:
 *
 *	Returns:
 *		nodecnt
 *		If nodecnt = 0 then errno is set to EPBADNODE
 *
*/
int
nodeparser(descr,node_array)
char *descr;
node_t *node_array;
{
	char    *cptr,*consumed,*cp;
	int     nodecnt,have_range,end,begin,state,j,i,len;
	node_t	*np;

	nodecnt = 0;
	np = node_array;

	have_range = FALSE;
	len  = strlen(descr);
	begin = -1;
	end   = -1;

	/* Grammar for a valid node list:
	* nodeexp = symbol "n" <or> 
	*      ( node <&> ([symbol ".." <&> node <&> [nodeexp]] |
	*                      [symbol "," <&> nodeexp]))
	*/

	if ( (consumed = (char *)malloc(len)) == NULL)
		return 0;

	cptr = consumed;
	state = STATE_NUM1;
	cp = descr;
	len = strlen(descr);
	errno = 0;

	for(i = 0; (errno == 0) && (i < len);i++){
		if (state == STATE_ANY){
			if (isdigit(*cp))
				state = STATE_NUM2;
			else
			if ( *cp == ',')
				state = STATE_COMMA;
			else
			if ( *cp == '.')
				state = STATE_DOT1;
			else
			if ( *cp == 'n')
				state = STATE_NUM1;
		}
		else
		if (state == STATE_COMMADOT){
			if ( *cp == ',')
				state = STATE_COMMA;
			else
			if ( *cp == '.')
				state = STATE_DOT1;
		}

		switch (state){
		case STATE_NUM1:{
		/* take a number or an 'n' */
			if ( (!isdigit(*cp)) && (*cp != 'n') ){
				errno = EPBADNODE;
				break;
			}
			else
			if (isdigit(*cp)){
				*cptr++ = *cp++;	
				state = STATE_ANY;
			}
			else{
				if (have_range){
					*np++ = -200;
					have_range = FALSE;
					cptr = consumed;
					state = STATE_COMMA;
				}
				else{
					*np++ = -100;
					cptr = consumed;
					state = STATE_COMMADOT;
				}
				nodecnt++;
				*cp++;
			}	
			break;
		}
		case STATE_NUM2:{
		/* only accept numbers here */
			if (!isdigit(*cp)){
				errno = EPBADNODE;
				break;
			}
			else{
				*cptr++ = *cp++;	
			}
			state = STATE_ANY;
			break;
		}
		case STATE_DOT1:{
			if (*cp != '.'){
				errno = EPBADNODE;
				break;
			}
			*cptr = '\0';
			/* see if there is any input to consume */
			if (cptr > consumed){
				/* no, go ahead and consume the input */
				*np++ = atoi(consumed);
				nodecnt++;
			}
#ifdef JMC
			/* See if the last item consumed was an 'n' */
			if ( *(np-1) != -100){
				/* no, go ahead and consume the input */
				*np++ = atoi(consumed);
				nodecnt++;
			}
#endif JMC
			consumed[0] = '\0';
			cptr = consumed;
			cp++;
			state = STATE_DOT2;
			break;
		}
		case STATE_DOT2:{
			if (*cp != '.'){
				errno = EPBADNODE;
				break;
			}
			cp++;
			state = STATE_NUM1;
			have_range = TRUE;
			break;
		}
		case STATE_COMMA:{
			if (*cp != ','){
				errno = EPBADNODE;
				break;
			}
			*cptr = '\0';
			if (cptr > consumed){
				if (have_range){
					end = atoi(consumed);
					/* check to see if the spec is of the form
					 * n..0, ie a descending range starting from n
					*/
					if (*(np-1) == -100){
						/* yes it is! */
						*(np-1) = -300;
						*np++ = end;
						nodecnt++;
					}
					else{
						if ( *(np-1) <= end)
							/* count from *(np-1)+1 upto end */
							for(j = *(np-1)+1; j <= end; j++){
								*np++ = j;
								nodecnt++;
							}
						else
							/* count *(np-1)-1 downto end*/
							for(j = *(np-1)-1; j >= end; j--){
								*np++ = j;
								nodecnt++;
							}
					}
					have_range = FALSE;
				}
				else{
					*np++ = atoi(consumed);
					nodecnt++;
				}
				cptr = consumed;
				consumed[0] = '\0';
			}
			cp++;
			state = STATE_NUM1;
			break;
		}
		default:
			errno = EPBADNODE;
			break;
		}
	}

	if ( (errno == 0) &&
		/* make sure we are in a final state */
		( (state == STATE_ANY) || (state == STATE_COMMA) ||
		  (state == STATE_COMMADOT) || (state == STATE_NUM2) ) ){
		/* check for unconsumed input */
		if ( cptr > consumed){
			*cptr = '\0';
			if (have_range){
				end = atoi(consumed);
				/* Do we have a downward range of the form n..0 */
				if ( *(np-1) == -100){
					*(np-1) = -300;
					*np++ = end;
					nodecnt++;
				}
				else{
					/* see if we need to count up or down */
					if (end >= *(np-1))
						for(j = *(np-1)+1; j <= end; j++){
							*np++ = j;
							nodecnt++;
						}
					else
						for(j = *(np-1)-1; j >= end; j--){
							*np++ = j;
							nodecnt++;
						}
				}
				have_range = FALSE;
			}
			else{
				*np++ = atoi(consumed);
				nodecnt++;
			}
		}
	}
	else{
		nodecnt = 0;
		errno = EPBADNODE;
	}

	return(nodecnt);
		
}

