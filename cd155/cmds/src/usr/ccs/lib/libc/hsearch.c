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
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: hsearch.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:06:06 $";
#endif
/*
 * COMPONENT_NAME: (LIBCGEN) Standard C Library General Functions
 *
 * FUNCTIONS: hsearch, hcreate, hdestroy
 *
 * ORIGINS: 3 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989 
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1984 AT&T	
 * All Rights Reserved  
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	
 * The copyright notice above does not evidence any   
 * actual or intended publication of such source code.
 *
 * hsearch.c	1.11  com/lib/c/gen,3.1,8943 10/26/89 14:52:17
 */

#include <stdio.h>				/* for NULL	*/
#include <stdlib.h>
#include <string.h>
#include <search.h>				/* for ACTION	*/

/* Compile time switches:

   MULT - use a multiplicative hashing function.
   DIV - use the remainder mod table size as a hashing function.
   CHAINED - use a linked list to resolve collisions.
   OPEN - use open addressing to resolve collisions.
   BRENT - use Brent's modification to improve the OPEN algorithm.
   SORTUP - CHAINED list is sorted in increasing order.
   SORTDOWN - CHAINED list is sorted in decreasing order.
   START - CHAINED list with entries appended at front.
   DRIVER - compile in a main program to drive the tests.
   DEBUG - compile some debugging printout statements.
   USCR - user supplied comparison routine.
*/


#define SUCCEED		0
#define FAIL		1
#define TRUE		1
#define FALSE		0
#define repeat		for(;;)
#define until(A)	if(A) break;

#ifdef OPEN
#    undef CHAINED
#else
#ifndef CHAINED
#    define OPEN
#endif
#endif

#ifdef MULT
#    undef DIV
#else
#ifndef DIV
#    define MULT
#endif
#endif

#ifdef START
#    undef SORTUP
#    undef SORTDOWN
#else
#ifdef SORTUP
#    undef SORTDOWN
#endif
#endif

#ifdef USCR
#    define COMPARE(A, B) (* hcompar)((A), (B))
     extern int (* hcompar)();
#else
#    define COMPARE(A, B) strcmp((A), (B))
#endif

#ifdef MULT
#    define SHIFT ((bitsper * sizeof(int)) - m) /* Shift factor */
#    define FACTOR 035761254233	/* Magic multiplication factor */
#    define HASH hashm		/* Multiplicative hash function */
#    define HASH2 hash2m	/* Secondary hash function */
static unsigned int bitsper;	/* Bits per byte */
static unsigned int hashm();
static unsigned int hash2m();
#else
#ifdef DIV
#    define HASH hashd		/* Division hashing routine */
#    define HASH2(A) 1		/* Secondary hash function */
static unsigned int hashd();
#endif
#endif

/*
 *	POINTER should match the type of both `key' and `data' within
 *	an ENTRY structure...
 */
typedef char *POINTER;

#ifdef CHAINED
typedef struct node {	/* Part of the linked list of entries */
	ENTRY item;
	struct node *next;
} NODE;
typedef NODE *TABELEM;
static NODE **table;	/* The address of the hash table */
static ENTRY *build();
#else
#ifdef OPEN
typedef ENTRY TABELEM;	/* What the table contains (TABle ELEMents) */
static TABELEM *table;	/* The address of the hash table */
static unsigned int count = 0;	/* Number of entries in hash table */
#endif
#endif

static unsigned int length;	/* Size of the hash table */
static unsigned int m;		/* Log base 2 of length */
static unsigned int prcnt;	/* Number of probes this item */
static unsigned int crunch();

#ifdef DRIVER
static void hdump();

main()
{
    char line[80];	/* Room for the input line */
    int i = 0;		/* Data generator */
    ENTRY *res;		/* Result of hsearch */
    ENTRY *new;		/* Test entry */

    if(hcreate(5))
	printf("Length = %u, m = %u\n", length, m);
    else {
	fprintf(stderr, "Out of core\n");
	exit(FAIL);
    }

    repeat {
	hdump();
	printf("Enter a probe: ");
	until (EOF == scanf("%s", line));
	new = (ENTRY *) malloc(sizeof(ENTRY));
	if(new == NULL) {
	    fprintf(stderr, "Out of core \n");
	    exit(FAIL);
	}
	else {
	    new->key = malloc((unsigned) strlen(line) + 1);
	    if(new->key == NULL) {
		fprintf(stderr, "Out of core \n");
		exit(FAIL);
	    }
	    strcpy(new->key, line);
	    new->data = malloc(sizeof(int));
	    if(new->data == NULL) {
		fprintf(stderr, "Out of core \n");
		exit(FAIL);
	    }
	    *new->data = i++;
	}
	res = hsearch(*new, ENTER);
	printf("The number of probes required was %d\n", prcnt);
	if(res == (ENTRY *) 0)
	    printf("Table is full\n");
	else {
	    printf("Success: ");
	    printf("Key = %s, Value = %d\n", res->key, *res->data);
	}
    }
    exit(SUCCEED);
}
#endif

/*
 * NAME:	hcreate
 *                                                                    
 * FUNCTION:	create the hash table
 *                                                                    
 * NOTES:	Hcreate creates the hash table.  The parameter indicates
 *		the largest number of entries this hash table will contain.
 *
 * DATA STRUCTURES:	'table' is allocated
 *
 * RETURN VALUE DESCRIPTION:	1 is returned if the allocation succeeds,
 *		else 0 is returned
 */  

int
hcreate(   		/* Create a hash table no smaller than size */
unsigned int size)	/* Minimum size for hash table */
{
    unsigned int unsize;	/* Holds the shifted size */

    if(size == 0)
	return(FALSE);

    unsize = size;	/* +1 for empty table slot; -1 for ceiling */
    length = 1;		/* Maximum entries in tabbe */
    m = 0;		/* Log2 length */
    while(unsize) {
	unsize >>= 1;
	length <<= 1;
	m++;
    }

    table = (TABELEM *) calloc((size_t)length, (size_t)sizeof(TABELEM));
    return(table != NULL);
}

/*
 * NAME:	hdestroy
 *                                                                    
 * FUNCTION:	destroy the hash table
 *                                                                    
 * NOTES:	hdestroy frees up the hash table.  This allows the
 *		program to then hcreate another, as only one hash
 *		table can exist at a time.
 *
 * DATA STRUCTURES:	'table' is freed
 *
 * RETURN VALUE DESCRIPTION:	none
 */  

void
hdestroy(void)	/* Reset the module to its initial state */
{
    free((void *) table);
#ifdef OPEN
    count = 0;
#endif
}

#ifdef OPEN
/* Hash search of a fixed-capacity table.  Open addressing used to
   resolve collisions.  Algorithm modified from Knuth, Volume 3,
   section 6.4, algorithm D.  Labels flag corresponding actions.
*/

/*
 * NAME:	hsearch
 *                                                                    
 * FUNCTION:	search and/or add to a hash table
 *                                                                    
 * NOTES:	hsearch searches a hash table for a matching entry.  If
 *		no matching entry is found and 'action' is ENTER, then
 *		the entry is added, if possible.  If no matching entry
 *		is found, and 'action' is FIND, then the entry is not
 *		added.
 *
 * DATA STRUCTURES:	'table' may be added to
 *
 * RETURN VALUE DESCRIPTION:	NULL if 'action' is FIND and the entry
 *		was not found, NULL if 'action' is ENTER, the entry was
 *		not found, and the table was full, else a pointer to
 *		the matching entry.
 */  

ENTRY *
hsearch(             	/* Find or insert the item into the table */
ENTRY item,		/* Item to be inserted or found */
ACTION action)		/* FIND or ENTER */
{
    unsigned int i;	/* Insertion index */
    unsigned int c;	/* Secondary probe displacement */

    prcnt = 1;

/* D1: */ 
    i = HASH(item.key);	/* Primary hash on key */

/* D2: */
    if(table[i].key == NULL)	/* Empty slot? */
	goto D6;
    else if(COMPARE(table[i].key, item.key) == 0)	/* Match? */
	return(&table[i]);

/* D3: */
    c = HASH2(item.key);	/* No match => compute secondary hash */

D4: 
    i = (i + c) % length;	/* Advance to next slot */
    prcnt++;

/* D5: */
    if(table[i].key == NULL)	/* Empty slot? */
	goto D6;
    else if(COMPARE(table[i].key, item.key) == 0)	/* Match? */
	return(&table[i]);
    else
	goto D4;

D6: if(action == FIND)		/* Insert if requested */
	return((ENTRY *) NULL);
    if(count == (length - 1))	/* Table full? */
	return((ENTRY *) 0);

#ifdef BRENT
/* Brent's variation of the open addressing algorithm.  Do extra
   work during insertion to speed retrieval.  May require switching
   of previously placed items.  Adapted from Knuth, Volume 3, section
   4.6 and Brent's article in CACM, volume 10, #2, February 1973.
*/

    {   unsigned int p0 = HASH(item.key);   /* First probe index */
	unsigned int c0 = HASH2(item.key);  /* Main branch increment */
	unsigned int r = prcnt - 1; /* Current minimum distance */
	unsigned int j;         /* Counts along main branch */
	unsigned int k;         /* Counts along secondary branch */
	unsigned int curj;      /* Current best main branch site */
	unsigned int curpos;    /* Current best table index */
	unsigned int pj;        /* Main branch indices */
	unsigned int cj;        /* Secondary branch increment distance*/
	unsigned int pjk;       /* Secondary branch probe indices */

	if(prcnt >= 3) {
	    for(j = 0; j < prcnt; j++) {   /* Count along main branch */
		pj = (p0 + j * c0) % length; /* New main branch index */
		cj = HASH2(table[pj].key); /* Secondary branch incr. */
		for(k=1; j+k <= r; k++) { /* Count on secondary branch*/
		    pjk = (pj + k * cj) % length; /* Secondary probe */
		    if(table[pjk].key == NULL) { /* Improvement found */
		        r = j + k;	/* Decrement upper bound */
		        curj = pj;	/* Save main probe index */
		        curpos = pjk;	/* Save secondeary index */
		    }
		}
	    }
	    if(r != prcnt - 1) {       /* If an improvement occurred */
		table[curpos] = table[curj]; /* Old key to new site */
		i = curj;
	    }
	}
    }
#endif
    count++;			/* Increment table occupancy count */
    table[i] = item;		/* Save item */
    return(&table[i]);		/* Address of item is returned */
}
#endif

#ifdef USCR
#    ifdef DRIVER
static int
compare(a, b)
POINTER a;
POINTER b;
{
    return(strcmp(a, b));
}

int (* hcompar)() = compare;
#    endif
#endif

#ifdef CHAINED
#    ifdef SORTUP
#        define STRCMP(A, B) (COMPARE((A), (B)) > 0)
#    else
#    ifdef SORTDOWN
#        define STRCMP(A, B) (COMPARE((A), (B)) < 0)
#    else
#        define STRCMP(A, B) (COMPARE((A), (B)) != 0)
#    endif
#    endif

ENTRY *
hsearch(item, action)	/* Chained search with sorted lists */
ENTRY item;		/* Item to be inserted or found */
ACTION action;		/* FIND or ENTER */
{
    NODE *p;		/* Searches through the linked list */
    NODE **q;		/* Where to store the pointer to a new NODE */
    unsigned int i;	/* Result of hash */
    int res;		/* Result of string comparison */

    prcnt = 1;

    i = HASH(item.key);	/* Table[i] contains list head */

    if(table[i] == (NODE*)NULL) { /* List has not yet been begun */
	if(action == FIND)
	    return((ENTRY *) NULL);
	else
	    return(build(&table[i], (NODE *) NULL, item));
    }
    else {			/* List is not empty */
	q = &table[i];
	p = table[i];
	while(p != NULL && (res = STRCMP(item.key, p->item.key))) {
	    prcnt++;
	    q = &(p->next);
	    p = p->next;
	}

	if(p != NULL && res == 0)	/* Item has been found */
	    return(&(p->item));
	else {			/* Item is not yet on list */
	    if(action == FIND)
		return((ENTRY *) NULL);
	    else
#ifdef START
		return(build(&table[i], table[i], item));
#else
		return(build(q, p, item));
#endif
	}
    }
}

static ENTRY *
build(last, next, item)
NODE **last;		/* Where to store in last list item */
NODE *next;		/* Link to next list item */
ENTRY item;		/* Item to be kept in node */
{
    NODE *p = (NODE *) malloc(sizeof(NODE));

    if(p != NULL) {
	p->item = item;
	*last = p;
	p->next = next;
	return(&(p->item));
    }
    else
	return(NULL);
}
#endif

#ifdef DIV
static unsigned int
hashd(key)		/* Division hashing scheme */
POINTER key;		/* Key to be hashed */
{
    return(crunch(key) % length);
}
#else
#ifdef MULT
/*
    NOTE: The following algorithm only works on machines where
    the results of multiplying two integers is the least
    significant part of the double word integer required to hold
    the result.  It is adapted from Knuth, Volume 3, section 6.4.
*/

static unsigned int
hashm(key)		/* Multiplication hashing scheme */
POINTER key;		/* Key to be hashed */
{
    static int first = TRUE;	/* TRUE on the first call only */

    if(first) {		/* Compute the number of bits in a byte */
	unsigned char c = ~0;	/* A byte full of 1's */
	bitsper = 0;
	while(c) {		/* Shift until no more 1's */
	    c >>= 1;
	    bitsper++;		/* Count number of shifts */
	}
	first = FALSE;
    }
    return((int) (((unsigned) (crunch(key) * FACTOR)) >> SHIFT));
}

/*
 * Secondary hashing, for use with multiplicitive hashing scheme.
 * Adapted from Knuth, Volume 3, section 6.4.
 */

static unsigned int
hash2m(key)		/* Secondary hashing routine */
POINTER key;		/* String to be hashed */
{
    return((int) (((unsigned) ((crunch(key) * FACTOR) << m) >> SHIFT) | 1));
}
#endif
#endif

static unsigned int
crunch(key)		/* Convert multicharacter key to unsigned int */
POINTER key;
{
    unsigned int sum = 0;	/* Results */
    int s;			/* Length of the key */

    for(s = 0; *key; s++)	/* Simply add up the bytes */
	sum += *key++;

    return(sum + s);
}

#ifdef DRIVER
static void
hdump()			/* Dumps loc, data, probe count, key */
{
    unsigned int i;	/* Counts table slots */
#ifdef OPEN
    unsigned int sum = 0;	/* Counts probes */
#else
#ifdef CHAINED
    NODE *a;		/* Current Node on list */
#endif
#endif

    for(i = 0; i < length; i++)
#ifdef OPEN
	if(table[i].key == NULL)
	    printf("%o.\t-,\t-,\t(NULL)\n", i);
	else {
	    unsigned int oldpr = prcnt; /* Save current probe count */
	    hsearch(table[i], FIND);
	    sum += prcnt;
	    printf("%o.\t%d,\t%d,\t%s\n", i,
		*table[i].data, prcnt, table[i].key);
	    prcnt = oldpr;
	}
    printf("Total probes = %d\n", sum);
#else
#ifdef CHAINED
	if(table[i] == NULL)
	    printf("%o.\t-,\t-,\t(NULL)\n", i);
	else {
	    printf("%o.", i);
	    for(a = table[i]; a != NULL; a = a->next)
		printf("\t%d,\t%#0.4x,\t%s\n",
		    *a->item.data, a, a->item.key);
	}
#endif
#endif
}
#endif
