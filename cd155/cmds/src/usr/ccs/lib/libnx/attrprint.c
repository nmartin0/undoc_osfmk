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
 *      $Id: attrprint.c,v 1.3 1994/11/19 02:29:05 mtm Exp $
 *
 */

/*
 * attrprint.c
 *
 * HISTORY
 * $Log: attrprint.c,v $
 * Revision 1.3  1994/11/19  02:29:05  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1994/09/16  16:54:22  sdh
 *  Check error return from nx_get_exact_matching_bitmap().
 *
 *  Reviewer: davidl
 *  Risk: low
 *  Benefit or PTS #: 10540
 *  Testing:
 * 	manual
 * 	EATS: rmcall, rmcmd, controlc
 *  Module(s):
 * 	attrprint.c
 *
 * Revision 1.1  1994/06/01  20:21:56  mag
 * Initial revision
 *
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <mach/mach_types.h>
#include <mcmsg/mcmsg_appl.h>
#include <malloc.h>
#include <nx.h>
#include <nx/bitmap.h>
#include <nx/defines.h>

static FreeAttributeTable(int nNodes);

extern BITMAP_T* root_bitmap;

typedef struct {
	int	m_nAttributes;		/* # of attributes for this node */
	int*	m_pnValues;		/* array of attribute values */
	char**	m_ppszAttributes;	/* array of attribute strings */
} ATTRIBUTE_TABLE_T;	


static ATTRIBUTE_TABLE_T*	pAttributeTable;


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

static char*
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

    return pszSelector;
}


/******************************************************************************
 *
 * BuildAttributeTable
 *
 *	Build an Attribute Table from a list of attributes returned
 *	by nx_node_attr
 *
 *	Returns:
 *		0	Success
 *		-1	Failure
 *
 ******************************************************************************/

static int
BuildAttributeTable(char** ppszAttrs, int nNodes)
{
    int i, j;

/*
 * Allocate the attribute table
 */

    pAttributeTable =
      (ATTRIBUTE_TABLE_T*) MALLOC(nNodes * sizeof(ATTRIBUTE_TABLE_T));

    if (!pAttributeTable) {
	errno = ENOMEM;
	goto FailToBuild;
    }

    bzero(pAttributeTable, nNodes * sizeof(ATTRIBUTE_TABLE_T));

/*
 * Parse the attributes
 */

    for (i=0; i<nNodes; i++) {
	char* pszAttr;
	char* pszAttribute;
	char* pszTemp;
	char* pszComma;

	pszAttr = ppszAttrs[i];

	pszTemp = strchr(pszAttr, '\n');
	if (pszTemp)
	    *pszTemp = '\0';

/*
 * Break the comma-separated string into individual elements
 * First, count 'em
 */

	pAttributeTable[i].m_nAttributes = 
	  NX_analyse_selector_string(pszAttr, &j);

/*
 * Allocate the array of string pointers and values
 */
	if (pAttributeTable[i].m_nAttributes) {
	    pAttributeTable[i].m_ppszAttributes =
	      (char**) MALLOC(pAttributeTable[i].m_nAttributes * sizeof(char*));
	    if (!pAttributeTable[i].m_ppszAttributes) {
		errno = ENOMEM;
		goto FailToBuild;
	    }

	    pAttributeTable[i].m_pnValues =
	      (int*) MALLOC(pAttributeTable[i].m_nAttributes * sizeof(int));
	    if (!pAttributeTable[i].m_pnValues) {
		errno = ENOMEM;
		goto FailToBuild;
	    }
	} else {
	    pAttributeTable[i].m_ppszAttributes = (char**)0;
	    pAttributeTable[i].m_pnValues = (int*)0;
	    continue;
	}

/*
 * Copy the strings to the new array
 */

	pszAttribute = pszAttr;
	j = 0;

	while (*pszAttribute) {
	    pszComma = strchr(pszAttribute, ',');
	    if (pszComma)
		*pszComma = '\0';
	    pszTemp = ParseSelector(pszAttribute,
	      NULL, pAttributeTable[i].m_pnValues+j);
	    if (!pszTemp) {
		errno = EINVAL;
		goto FailToBuild;
	    }
	    pAttributeTable[i].m_ppszAttributes[j++] = strdup(pszTemp);
	    if (pszComma)
		pszAttribute = pszComma+1;
	    else
		break;
	}
    }

    return 0;

FailToBuild:

    FreeAttributeTable(nNodes);
    return -1;
}

static
FreeAttributeTable(int nNodes)
{
    int i, j;

    for (i=0; i<nNodes; i++) {
	for (j=0; j<pAttributeTable[i].m_nAttributes; j++)
	    FREE((void*)pAttributeTable[i].m_ppszAttributes[j]);
	FREE((void*) pAttributeTable[i].m_pnValues);
	FREE((void*) pAttributeTable[i].m_ppszAttributes);
    }

    FREE((void*) pAttributeTable);
}


/*****************************************************************************
 *	Get the attributes common to all nodes in a partition
 *
 * Arguments:
 *
 *	pszPartName		Name of partition
 *	pszBuff			Place to put attributes
 *	nSize			Size of supplied buffer
 *
 * Return:
 *	-1			error (buffer overflow) or bad partition
 *
 *	0			success
 *****************************************************************************/

int
nx_get_common_attributes(char* pszPartName, char* pszBuff, int nSize)
{
    char*	pszOut = pszBuff;
    int		nOutCnt = 0;
    char**	ppszAttrs;
    int		nNodes;
    typedef struct attr_list {
	char*			m_pszAttr;
	struct attr_list*	m_pAttrListNext;
    } AttrList;
    AttrList*	pAttrListHead = (AttrList*)0;
    AttrList*	pAttrListTail = (AttrList*)0;
    AttrList*	pAttrListTemp;
    int		i, j, nRval;
    int		nValue, nTest;
    int		bMatch;
    int		bFirst;
    nx_node_list_t	nodesUnavail;
    unsigned long	nUnavailSize;
    BITMAP_T*		bitmapAvail;
    nx_nodes_t		nodesL2P;
    unsigned long	nTemp;

/*
 * Get the failed nodes
 */

    if (nx_failed_nodes(&nodesUnavail, &nUnavailSize) == -1)
	return -1;

    bitmapAvail = bitmap_clone(root_bitmap);

    for (i=0; i<nUnavailSize; i++)
	setbit_bitmap(bitmapAvail, 0, nodesUnavail[i]);

    free((void*) nodesUnavail);

    if (nx_get_node_list(pszPartName, &nodesL2P, &nTemp) == -1)
	return -1;

/*
 * Get the node attributes for the partition
 */

    nNodes = nx_node_attr(pszPartName, &ppszAttrs);

    if (nNodes < 0)
	return -1;
	
    *pszBuff = 0;

/*
 * Build an Attribute Table
 */

    BuildAttributeTable(ppszAttrs, nNodes);
    free((void*)ppszAttrs);

/*
 * Enumerate all possible attributes
 *
 * Loop over all nodes
 */
    for (i=0; i<nNodes; i++) {
/*
 * Loop over all attributes for each node
 */
	for (j=0; j<pAttributeTable[i].m_nAttributes; j++) {
/*
 * Have we seen this attribute?
 */
	    for (pAttrListTemp=pAttrListHead;
	      pAttrListTemp; pAttrListTemp = pAttrListTemp->m_pAttrListNext)
		if (!strcasecmp(pAttrListTemp->m_pszAttr,
		  pAttributeTable[i].m_ppszAttributes[j]))
		    break;
/*
 * Add it to the list, if needed.  Add it at the end
 */
	    if (!pAttrListTemp) {
		pAttrListTemp = (AttrList*) MALLOC(sizeof(AttrList));
		if (!pAttrListTemp) {
		    errno = ENOMEM;
		    nRval = -1;
		    goto Fail;
		}
		pAttrListTemp->m_pszAttr =
		  pAttributeTable[i].m_ppszAttributes[j];
		pAttrListTemp->m_pAttrListNext = (AttrList*)0;
		if (pAttrListTail) {
		    pAttrListTemp->m_pAttrListNext = (AttrList*)0;
		    pAttrListTail->m_pAttrListNext = pAttrListTemp;
		} else
		    pAttrListHead = pAttrListTemp;
		pAttrListTail = pAttrListTemp;
	    }
	}
    }
/*
 * Loop over all discovered attributes
 */

    for (pAttrListTemp=pAttrListHead;
      pAttrListTemp; pAttrListTemp = pAttrListTemp->m_pAttrListNext) {
/*
 * Check if the attribute appears on all nodes (with the same value
 */
	bMatch = 1;
	bFirst = 1;

	for (i=0; i<nNodes; i++) {
/*
 * Skip nodes not in available bitmap
 */
	    if (!getbit_bitmap(bitmapAvail, nodesL2P[i]))
		continue;
/*
 * Look at all attributes for node i
 */
	    nTest = 0;
	    for (j=0; j<pAttributeTable[i].m_nAttributes; j++) {
/*
 * If attribute exists on node i, get its value.
 */
		if (!strcasecmp(pAttrListTemp->m_pszAttr,
		  pAttributeTable[i].m_ppszAttributes[j])) {
		    nTest = pAttributeTable[i].m_pnValues[j];
		    break;
		}
	    }

/*
 * If on 1st node in list, save value.
 */
	    if (bFirst) {
		if (j >= pAttributeTable[i].m_nAttributes) {
		    bMatch = 0;
		    break;
		}
		nValue =  nTest;
		bFirst = 0;
	    }
/*
 * All others must have the same value
 */
	    else if (nValue != nTest) {
		bMatch = 0;
		break;
	    }
	}
/*
 * Was the attribute on all nodes with the same value?
 */
	if (bMatch) {
/*
 * Yes! Add to output buffer
 */
	    char szTemp[100];
	    int k;
	    if (nValue != 1 || !strcasecmp(pAttrListTemp->m_pszAttr, "proc"))
		sprintf(szTemp, nOutCnt?",%d%s":"%d%s",
		  nValue, pAttrListTemp->m_pszAttr);
	    else
		sprintf(szTemp, nOutCnt?",%s":"%s",
		  pAttrListTemp->m_pszAttr);
	    k = strlen(szTemp);
	    if ((k+nOutCnt) > nSize) {
		errno = ENOMEM;
		nRval = -1;
		goto Fail;
	    }
	    strcpy(pszOut+nOutCnt, szTemp);
	    nOutCnt += k;
	}
    }

    nRval = 0;

Fail:
    while (pAttrListHead) {
	pAttrListTemp = pAttrListHead->m_pAttrListNext;
	FREE((void*)pAttrListHead);
	pAttrListHead = pAttrListTemp;
    }

    FreeAttributeTable(nNodes);

    free((void*)nodesL2P);

    free((void*)bitmapAvail);

    return nRval;
}


/*****************************************************************************
 *	Routines to get details about node attributes
 *
 *	int nx_begin_detail_list(char* pszPartName)
 *
 *	BITMAP_T* nx_get_next_detail(char* pszBuff, int nSize)
 *
 *****************************************************************************/


/*****************************************************************************
 *
 *	int nx_begin_detail_list(char* pszPartName)
 *
 *	Initialize for listing node attribute details for a partition
 *
 * Arguments:
 *
 *	pszPartName		Name of partition
 *
 * Return:
 *	-1			error 
 *
 *	0			success
 *****************************************************************************/

static char*		pbDisplayed;
static int		nCurrentPosition;
static int		nNodeCnt;
static nx_nodes_t	nodesLog2Phys;
static BITMAP_T*	bitmapPart;

int
nx_begin_detail_list(char* pszPartName)
{
    char**		ppszAttrs;
    unsigned long	nSize;

/*
 * Get node attributes for the partition
 */

    nNodeCnt = nx_node_attr(pszPartName, &ppszAttrs);

    if (nNodeCnt < 0)
	return -1;

/*
 * Build an Attribute Table
 */

    BuildAttributeTable(ppszAttrs, nNodeCnt);
    free((void*)ppszAttrs);

/*
 * Flags for nodes in partition already seen
 */

    pbDisplayed = (char*) MALLOC(nNodeCnt);

    if (pbDisplayed == (char*)0) {
	FreeAttributeTable(nNodeCnt);
	errno = ENOMEM;
	return -1;
    }
    bzero(pbDisplayed, nNodeCnt);

    if (nx_get_node_list(pszPartName, &nodesLog2Phys, &nSize) == -1) {
	free((void*)pbDisplayed);
	FreeAttributeTable(nNodeCnt);
	return -1;
    }

    if (nx_get_part_bitmap(pszPartName, &bitmapPart, &nSize) == -1) {
	free((void*)pbDisplayed);
	free((void*)nodesLog2Phys);
	FreeAttributeTable(nNodeCnt);
	return -1;
    }

/*
 * Mark nodes not in the partition as "Displayed".
 * This is expressly to handle the root partition.
 */

    for (nCurrentPosition = 0; nCurrentPosition < nNodeCnt;
      nCurrentPosition++)
	if (!getbit_bitmap(bitmapPart, nodesLog2Phys[nCurrentPosition]))
	    pbDisplayed[nCurrentPosition] = 1;

    nCurrentPosition = 0;

    return 0;
}

/*****************************************************************************
 *
 *	BITMAP_T* nx_get_next_detail(char* pszBuff, int nSize)
 *
 *	Get the next group of nodes and their attributes
 *
 * Arguments:
 *
 *	pszPartName		Buffer into which attributes are placed
 *	nSize			Size of the supplied buffer
 *				Retured string is silently truncated to size
 *				of buffer
 *
 * Return:
 *	NULL			Done
 *
 *	BITMAP_T*		Pointer to bitmap of nodes to which these
 *				attributes apply.  Caller must free this
 *				when done with it.
 *****************************************************************************/

BITMAP_T*
nx_get_next_detail(char* pszBuff, int nSize)
{
    int i;
    char szLocalBuff[1024];
    BITMAP_T*		bitmapRVal;
    unsigned long	nBitmapSize;

    while (nCurrentPosition < nNodeCnt && pbDisplayed[nCurrentPosition])
	nCurrentPosition++;

    if (nCurrentPosition >= nNodeCnt) {
/*
 * All done!
 */

	free((void*)pbDisplayed);
	free((void*)nodesLog2Phys);
	FreeAttributeTable(nNodeCnt);
	free((void*)bitmapPart);
	return (BITMAP_T*)0;
    }

/*
 * Build description of attributes into local buffer
 */

    szLocalBuff[0] = '\0';

    for (i=0; i<pAttributeTable[nCurrentPosition].m_nAttributes; i++) {
	char szTemp[100];
	if (pAttributeTable[nCurrentPosition].m_pnValues[i] != 1 ||
	   !strcasecmp(pAttributeTable[nCurrentPosition].m_ppszAttributes[i],
	   "proc"))
	    sprintf(szTemp, i?",%d%s":"%d%s",
	      pAttributeTable[nCurrentPosition].m_pnValues[i],
	      pAttributeTable[nCurrentPosition].m_ppszAttributes[i]);
	else
	    sprintf(szTemp, i?",%s":"%s",
	      pAttributeTable[nCurrentPosition].m_ppszAttributes[i]);
	strcat(szLocalBuff, szTemp);
    }

/*
 * Get bitmap of matching nodes (in all of root partition)
 */

    if (nx_get_exact_matching_bitmap(&bitmapRVal, &nBitmapSize, szLocalBuff, 1) < 0)
	return (BITMAP_T*)0;

/*
 * Mask to interesting nodes in current partition
 */
    bitmap_mask(bitmapPart, bitmapRVal);

    if (num_nodes_in_map(bitmapRVal) == 0) {
	free((void*)pbDisplayed);
	free((void*)nodesLog2Phys);
	FreeAttributeTable(nNodeCnt);
	free((void*)bitmapPart);
	return (BITMAP_T*)0;
    }

/*
 * Give attributes back to user
 */

    strncpy(pszBuff, szLocalBuff, nSize);

/*
 * Mark out all nodes returned to we don't display them again
 */

    for (i=nCurrentPosition; i<nNodeCnt; i++)
	if (getbit_bitmap(bitmapRVal, nodesLog2Phys[i]))
	    pbDisplayed[i] = 1;

    return bitmapRVal;
}
