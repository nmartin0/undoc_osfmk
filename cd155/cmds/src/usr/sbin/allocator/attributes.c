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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/allocator/attributes.c,v 1.6 1994/11/19 03:04:09 mtm Exp $
 *
 */

/* History:
 *	$Log: attributes.c,v $
 * Revision 1.6  1994/11/19  03:04:09  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/09/16  16:29:55  sdh
 *  Changed to error msg printed if the open of the nodeinfo file
 *  fails.
 *
 *  Reviewer: davidl
 *  Risk: low
 *  Benefit or PTS #: 10447
 *  Testing: manual
 *  Module(s):
 * 	attributes.c
 *
 * Revision 1.4  1994/07/13  20:27:56  mag
 * Correct handling of empty attributes
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #:
 *  Testing: developer
 *  Module(s): attributes.c
 *
 * Revision 1.3  1994/06/15  00:47:19  mag
 * Better handling of syntax errors in node attribute strings
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: 9845
 *  Testing: developer
 *  Module(s): attributes.c
 *
 * Revision 1.2  1994/06/08  17:16:39  mag
 * Correct core dumps when user passes bad args to showpart
 *  Reviewer: gregt
 *  Risk: Low
 *  Benefit or PTS #: 9720
 *  Testing: EATS: rmcmd
 *  Module(s): allocator/{attribute.c, attribute.h, init_appl.c, mkpart_rpc.c}
 * 	    showpart/showpart.c
 *  Related: None
 *
 * Revision 1.1  1994/06/01  20:46:21  mag
 * Initial revision
 *
*/


#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <mach/mach_types.h>
#include <mcmsg/mcmsg_appl.h>
#include <nx/defines.h>
#include <nx/schedule.h>
#include <nx/bitmap.h>

#include "attributes.h"

ATTRIBUTE_TABLE_T*	pAttributeTable;

extern int		attrib_debug;

PART_T			*root;

/******************************************************************************
 *
 * ParseSelector
 *
 *	Parse the selector string, returning the Relop (=, >, etc),
 *	the value and the attribute.
 *
 *	pszSelector	The selector string
 *	pszRelop	Buffer to receive Relop, which may be 1 or 2 chars
 *	nValue		Value is stored through this pointer
 *
 *	Returns:
 *
 *		Attribute string or NULL if there is a syntax error
 *
 ******************************************************************************/

char*
ParseSelector(char* pszSelector, char* pszRelop, int* nValue)
{
/*
 * If he didn't pass us a place for the Relop, just chuck it here
 */
    char szRelop[3];

    if (!pszRelop)
	pszRelop = szRelop;

/*
* Default Relop is =
*/

    pszRelop[0] = '=';
    pszRelop[1] = '\0';

    switch (*pszSelector) {
    case '=':
    case '!':
	*pszRelop++ = *pszSelector;
/*
 * Allow 2nd char to be '=', as in '==' or '!='.  Only store 1st.
 */
	if (*++pszSelector == '=')
	    pszSelector++;
	break;

    case '>':
    case '<':
/*
 * 2nd char may be '=', as in '>=' or '<='.  Store both chars
 */
	*pszRelop++ = *pszSelector;
	if (*++pszSelector == '=')
	    *pszRelop++ = *pszSelector++;
	break;

    }

/*
 * No more =, !, > or < allowed
 */

    switch (*pszSelector) {
    case '=':
    case '!':
    case '>':
    case '<':
	return NULL;
    }

/*
 * Default value is 1
 */

    *nValue = 1;

    if (isdigit(*pszSelector)) {
	*nValue = 0;
	while (isdigit(*pszSelector))
	    *nValue = *nValue * 10 + *pszSelector++ - '0';
    }

    if (*pszSelector == '\0')
	return NULL;

    return pszSelector;
}

/******************************************************************************
 *
 * ReadAttributes(pszAttributeFile)
 *
 *	Read the specified attribute file and build the Attribute Table
 *
 *	The file contains one line per physical node (slot).  Each line
 *	contains a list of comma-separated attribute strings
 ******************************************************************************/

ReadAttributes(char* pszAttributeFile)
{
    int i, j;
    FILE* fd;
    char errstr[80];
/*
 * Allocate the attribute table
 */

    pAttributeTable =
      (ATTRIBUTE_TABLE_T*) MALLOC(root->slots * sizeof(ATTRIBUTE_TABLE_T));

    if (!pAttributeTable) {
	errno = ENOMEM;
	perror("allocator(attribute table)");
	exit(1);
    }

/*
 * Read the attribute file
 */

    if (!(fd = fopen(pszAttributeFile, "r"))) {
	sprintf(errstr, "Allocator: Failed to open attribute file %s", pszAttributeFile);
	perror(errstr);
	exit(1);
    }

    for (i=0; i<root->slots; i++) {
	char  szBuff[1024];
	char* pszAttribute;
	char* pszTemp;
	char* pszComma;

	if (!fgets(szBuff, 1023, fd))
	    break;

	pszTemp = strchr(szBuff, '\n');
	if (pszTemp)
	    *pszTemp = '\0';

/*
 * Break the comma-separated string into individual elements
 * First, count 'em
 */

	pAttributeTable[i].m_nAttributes = 
	  NX_analyse_selector_string(szBuff, &j);

/*
 * Allocate the array of string pointers and values
 */
	if (pAttributeTable[i].m_nAttributes) {
	    pAttributeTable[i].m_ppszAttributes =
	      (char**) MALLOC(pAttributeTable[i].m_nAttributes * sizeof(char*));

	    pAttributeTable[i].m_pnValues =
	      (int*) MALLOC(pAttributeTable[i].m_nAttributes * sizeof(int));
	} else {
	    pAttributeTable[i].m_ppszAttributes = (char**)0;
	    pAttributeTable[i].m_pnValues = (int*)0;
	    continue;
	}

/*
 * Copy the strings to the new array
 */

	pszAttribute = szBuff;
	j = 0;

	while (*pszAttribute) {
	    pszComma = strchr(pszAttribute, ',');
	    if (pszComma)
		*pszComma = '\0';
	    pszTemp = ParseSelector(pszAttribute,
	      NULL, pAttributeTable[i].m_pnValues+j);
	    if (!pszTemp) {
		errno = EINVAL;
		perror("allocator(parsing attributes)");
		exit(1);
	    }
	    pAttributeTable[i].m_ppszAttributes[j++] = strdup(pszTemp);
	    if (pszComma)
		pszAttribute = pszComma+1;
	    else
		break;
	}
    }

    fclose(fd);
}


/******************************************************************************
 *
 * BuildMatchingBitmap
 *
 *	Construct a bitmap have a bit set for each node that matches
 *	the specified selectors.
 *
 *	Return 0 on success, -1 on syntax error
 *
 ******************************************************************************/

typedef struct selector {
    int		m_nValue;
    char	m_sRelop[2];
    char*	m_pszAttribute;
} SELECTOR_T;

int
BuildMatchingBitmap(int selector_cnt, char* selector_buf,
  int exact, BITMAP_T** res)
{
    int i, j, k;

    BITMAP_T*	pBitMap;
    SELECTOR_T*	pSelectors;

/*
 * Special case of no selectors
 */

    if (selector_cnt == 0) {
/*
 * If not exact, return nothing.
 */
	if (!exact) {
	    *res = (BITMAP_T*)0;
	    return 0;
	}
/*
 * If exact, return nodes with no attributes
 */
	pBitMap = bitmap_clone(root->bitmap);
	init_bitmap(0, pBitMap);

	for (j=0; j<root->slots; j++)
	    if (pAttributeTable[j].m_nAttributes > 0)
		setbit_bitmap(pBitMap, 0, j);
	    else
		setbit_bitmap(pBitMap, 1, j);

	*res = pBitMap;
	return 0;
    }

    pSelectors = (SELECTOR_T*) MALLOC(selector_cnt * sizeof(SELECTOR_T));

/*
 * Convert to list of strings into an array of selectors
 */

    for (i=0; i<selector_cnt; i++) {
        pSelectors[i].m_pszAttribute =
	  ParseSelector(selector_buf,
	  pSelectors[i].m_sRelop, &(pSelectors[i].m_nValue));
	if (!pSelectors[i].m_pszAttribute) {
	    free(pSelectors);
	    return -1;
	}
	selector_buf += strlen(selector_buf)+1;
    }

    if (attrib_debug) {
	printf("BuildMatchingBitmap(");
	for (i=0; i<selector_cnt; i++)
	    printf("%s%d%s, ", pSelectors[i].m_sRelop,
	    pSelectors[i].m_nValue,
	    pSelectors[i].m_pszAttribute);
	printf(")\n");
    }

    pBitMap = bitmap_clone(root->bitmap);
    init_bitmap(0, pBitMap);

/*
 * Loop over all slots
 */

    for (j=0; j<root->slots; j++) {
	int	bMatch = 1;
	int	nValue;

/*
 * If exact match is requested, the selector_cnt must be
 * the same is the # of attributes on the node.  If not,
 * give up before we start.
 */

	if (exact && selector_cnt != pAttributeTable[j].m_nAttributes) {
	    setbit_bitmap(pBitMap, 0, j);
	    continue;
	}

/*
 * Loop over all selectors.  Note that we bail out early if we ever
 * get a mismatch.  That's where the 'AND' comes in.
 */

	for (i=0; i<selector_cnt && bMatch; i++) {

/*
 * Find the value for the candidate attribute on node j
 * If it is not present, we assume 0.
 */

	    nValue = 0;

	    for (k=0; k<pAttributeTable[j].m_nAttributes; k++) {
		if (!strcasecmp(pSelectors[i].m_pszAttribute,
		  pAttributeTable[j].m_ppszAttributes[k])) {
		    nValue = pAttributeTable[j].m_pnValues[k];
		    break;
		}
	    }

/*
 * This macro constructs a 16-bit constant from a 1 or 2 character array
 */

#define INT_OF(x, y)	(((int)(x)) << 8 | ((int)(y)))

/*
 * Perform the relational operation
 */

	    switch (INT_OF(pSelectors[i].m_sRelop[0],
	      pSelectors[i].m_sRelop[1])) {

	    case INT_OF('=', '\0'):
		bMatch = nValue == pSelectors[i].m_nValue;
		break;

	    case INT_OF('!', '\0'):
		bMatch = nValue != pSelectors[i].m_nValue;
		break;

	    case INT_OF('>', '\0'):
		bMatch = nValue > pSelectors[i].m_nValue;
		break;

	    case INT_OF('<', '\0'):
		bMatch = nValue < pSelectors[i].m_nValue;
		break;

	    case INT_OF('>', '='):
		bMatch = nValue >= pSelectors[i].m_nValue;
		break;

	    case INT_OF('<', '='):
		bMatch = nValue <= pSelectors[i].m_nValue;
		break;
	    }
	}
/*
 * Set the bit in the bitmap for slot j
 */

	setbit_bitmap(pBitMap, bMatch, j);
    }

    if (attrib_debug) {
	printf("Matching bitmap is\n");
	print_bitmap(pBitMap);
	printf("\n");
    }

    free(pSelectors);

    *res = pBitMap;
    return 0;
}




get_matching_bitmap(serv_port,uid,gid,bitmap,bitmapCnt,
  selector_buf,selector_buflen,exact,retcode)
mach_port_t	serv_port;
uid_t		uid;
gid_t		gid;
BITMAP_T	**bitmap;
int		*bitmapCnt;
char		*selector_buf;
int		selector_buflen;
int		exact;
int		*retcode;
{
    BITMAP_T* selector_bitmap;
    int selector_cnt;
    char *buf, *s;
    int buflen;
    int i;

    *retcode = (int) vm_allocate(mach_task_self(),(vm_address_t ) bitmap,
		    bitmap_size(root->bitmap),1);

    if ( *retcode != KERN_SUCCESS){
	    /* We don't have any memory so quit */
	    *retcode = ENOMEM;
	    return KERN_SUCCESS;
    }

    *bitmapCnt = bitmap_size(root->bitmap);
    (*bitmap)->rows = root->bitmap->rows;
    (*bitmap)->cols = root->bitmap->cols;
    (*bitmap)->row_offset = root->bitmap->row_offset;

    init_bitmap(0, *bitmap);

    selector_cnt = NX_analyse_selector_string(selector_buf, &buflen);

    if (selector_cnt <= 0) {
	*retcode = EINVAL;
	return KERN_SUCCESS;
    }

    buf = (char*) malloc(buflen);
    s = buf;
    NX_parse_selector_string(selector_buf, &s);

    if (BuildMatchingBitmap(selector_cnt, buf, exact, &selector_bitmap) == -1) {
	*retcode = EINVAL;
	free((void*)buf);
	return KERN_SUCCESS;
    }

    free((void*)buf);

    *retcode = 0;

    if (selector_bitmap == (BITMAP_T*) 0)
	return KERN_SUCCESS;

    for (i = 0; i < selector_bitmap->cols; i++)
	(*bitmap)->colmap[i] = selector_bitmap->colmap[i];

    free((void*) selector_bitmap);

    return KERN_SUCCESS;
}
