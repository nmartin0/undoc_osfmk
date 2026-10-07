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
 *      This software is supplied under the terms of a license
 *      agreement or nondisclosure agreement with Intel Corporation
 *      and may not be copied or disclosed except in accordance
 *      with the terms of that agreement.
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/_gcol.c,v 1.
1 1992/04/07 10:25:44 regnier Exp $
 *
 */

#include <nx/gops.h>
#include <mcmsg/mcmsg_xmsg.h>
#include <mcmsg/mcmsg_nx.h>
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
extern char * malloc();
extern int tree_recv();
extern int power_of_two();
extern int bintogray();
extern int mcmsg_mynode;
extern int mcmsg_numnodes;
extern int mcmsg_ptype;
extern int msginfo[];
extern int tree_count_recv();
extern int _gcolx();

static int *msgids = NULL;
static int msgindx;
static int *nomsgids = NULL;
static int nomsgindx;
static char *tmpbuf[NUM_NXREQ];
typedef struct {
	long type;
 	long len;
	long snode;
	long sptype;
	long sys[4];
} info; 
static info *infobuf[NUM_NXREQ];

static long *xlens = NULL;

/*
 *  Calling Sequence:
 *      node_count = sum_tree_recv(from_node, this_node,inlen);
 *
 *  Description:
 *      Calculate total number of nodes below this_node in spanning tree.
 *      and post a irecv() (if possible) for a GATHER message from it...
 *      use a msgtype == GATHER_1 + node_number of node sending msg.
 *
 *  Parameters:
 *      int from_node   the node above this_node in spanning tree.
 *      int this_node   node from which to calculate tree.
 *      int inlen       length of buffer to allocate for irecv
 *
 *  Returns:
 *      int node_count  total number of nodes below this_node in tree.
 */


int
sum_tree_recv(from_node,this_node,inlen)
        int from_node,this_node,inlen;

{
    int nlist[NLIST_SIZE];
    int count,ncnt,i;

    if ((count = tree_count_list(from_node,this_node,nlist)) == 0) {
        return(count);
    }
    ncnt = count;
    for (i = 0; i < ncnt; i++) {
        count += sum_tree_recv(this_node, nlist[i],inlen);
        if (((tmpbuf[msgindx] = malloc(inlen)) != NULL) &&
            ((infobuf[msgindx] = (info *)malloc(sizeof(info))) != NULL) &&
            ((msgids[msgindx] = irecvx(GATHER_1 + nlist[i], tmpbuf[msgindx],
			inlen,-1,-1,infobuf[msgindx])) >= 0)){
                    msgindx++;
        } else {
            nomsgids[nomsgindx++] = nlist[i];
        }
    }
    return(count);
}

/*
 *  Calling Sequence:
 *      error = nx_gathercs(inbuf, outbuf, inlen, root)
 *
 *  Description:
 *      Gather fixed size data from all nodes in a contiguous output buffer
 *
 *  Parameters:
 *      int inbuf       input data
 *      int outbuf      output  buffer valid only on root node.
 *      int inlen       length of input data
 *      int root        root of gather tree
 *
 *  Returns:
 *      int error       -1 on error, otherwise 0
 */

int
nx_gathercs(inbuf, outbuf, inlen, root)
char *inbuf, *outbuf;
long inlen, root;

{
    long go, pow, rem, ncnt, ncount, nlist[NLIST_SIZE];
    long retnode, gmid, from_node;
    int i,j, msgrecvd;


    if (mcmsg_numnodes == 1) {
        bcopy(inbuf,outbuf,inlen);
        return(0);
    }
    nomsgindx = msgindx = 0;
    if (msgids == NULL)
        if ((msgids = (int *) malloc(NUM_NXREQ * sizeof(int))) == NULL ) {
            return(-1);
    }
    if (nomsgids == NULL)
        if((nomsgids = (int *)malloc(MAXNODES * sizeof(int))) == NULL ) {
            return(-1);
    }
    if (mcmsg_mynode != root) {
        if (_crecv(GATHER_2, (char*) &go, sizeof(long)) < 0) {
	    return(-1);
	}
        retnode = msginfo[4];
    }
    power_of_two(&pow,&rem);
    if( mcmsg_mynode >= (1<<pow)) {
        ncnt = ncount = 0;
    } else {
        if (mcmsg_mynode != root) {
            ncnt = ncount = tree_recv(retnode,nlist);
        } else {
            ncnt = ncount = tree_send(nlist);
        }
        for(j=0; j < ncnt; j++ ) {
            ncount += sum_tree_recv(mcmsg_mynode,nlist[j],inlen);
            if (((tmpbuf[msgindx] = malloc(inlen)) != NULL) &&
                ((infobuf[msgindx] = (info *)malloc(sizeof(info))) != NULL)) {
                if((msgids[msgindx] = irecvx(GATHER_1+nlist[j],tmpbuf[msgindx],
					inlen,-1,-1,infobuf[msgindx])) >= 0){
                    msgindx++;
                } else {
                    nomsgids[nomsgindx++] = nlist[j];
                }
            }
        }
    }
    if (mcmsg_mynode != root) {
        if (_csend(GATHER_1 + mcmsg_mynode, 
			inbuf, inlen, 
			retnode, mcmsg_ptype) < 0) {
	    return(-1);
	}
    } else {
        if (_csend(GATHER_2,(char*) &go,sizeof(long),-1,mcmsg_ptype) < 0) {
	    return(-1);
	}
        bcopy(inbuf,outbuf,inlen);
    }
    i = 0;
    for (msgrecvd=0; msgrecvd < ncount; msgrecvd++ ) {
        while (!msgdone(msgids[i])) {
            i = ++i % msgindx;
        }
        if (mcmsg_mynode != root) {
            if (_csend(infobuf[i]->type, tmpbuf[i], inlen, 
			retnode, mcmsg_ptype) < 0){
		return(-1);
	    }
        } else {
            bcopy(tmpbuf[i],&outbuf[((infobuf[i]->type - GATHER_1) * inlen)],inlen);
        }
        if ((msgindx + msgrecvd) < ncount ) {
            if((msgids[i] = irecvx(GATHER_1 + nomsgids[nomsgindx--], tmpbuf[i],
				inlen,-1,-1,infobuf[i])) < 0) {
                return(-1);
            }
        } else {
            free(tmpbuf[i]);
	    free(infobuf[i]);
            tmpbuf[i] = tmpbuf[--msgindx];
            infobuf[i] = infobuf[msgindx];
            msgids[i] = msgids[msgindx];
        }
        i = ++i % msgindx;
    }
    return(0);
}




/*
 *      _gcol.c
 *
 * Globally collects(concatenates) a contribution from each node 
 * in node number order.
 *
 * input..
 *
 *   x         pointer to the input buffer to be used in the operation.
 *   xlen      the length of input buffer x (in bytes).
 *   ylen      the length of output buffer y (in bytes).
 *
 * output..
 *
 *   y         pointer to the output buffer to be used in the operation.
 *             y contains the desired result.
 *   resultlen pointer to the number of bytes returned in y(truncated...)
 *
 * errors conditions
 *
 *       If a result longer than ylen bytes is attempted, an error
 *       has occurred, in this case, an error message is sent to stderr
 *       and the function returns -1.
 *
 */

long
_gcol(x, xlen, y, ylen, resultlen)
        char x[];
        long xlen;
        char y[];
        long ylen;
        long *resultlen;
{
    long go, pow, rem, ncnt, ncount, nlist[NLIST_SIZE]; 
    long retnode, gmid, from_node;
    int i,j, msgrecvd;


    if (mcmsg_numnodes == 1) {
	if (ylen < xlen) {
	    errno = EQMSGLONG;
	    return(-1);
	}
        bcopy(x,y,xlen);
	*resultlen = xlen;
        return(0);
    }
    if (xlens == NULL) 
	if((xlens = (long *)malloc((mcmsg_numnodes+1) * sizeof(long))) == NULL){
	    return(-1);
	}
    if (nx_gathercs(&xlen,xlens,sizeof(long),0) < 0) {
	return(-1);
    }
    if (mcmsg_mynode != 0) {
        if (_crecv(GATHER_2,(char*)xlens,
			((mcmsg_numnodes+1) * sizeof(long))) < 0) {
            return(-1);
        }
    } else {
	xlens[mcmsg_numnodes] = 0;
	for (i = 0; i < mcmsg_numnodes; i++) {
	    xlens[mcmsg_numnodes] += xlens[i];
	}
	if (_csend(GATHER_2,(char*) 
			xlens,
			((mcmsg_numnodes+1) * sizeof(long)),
			-1,mcmsg_ptype) < 0) {
	    return(-1);
        }
    }
    if ((*resultlen = xlens[mcmsg_numnodes]) > ylen) {
	errno = EQMSGLONG;
	return(-1);
    }
    if (_gcolx(x,xlens,y) < 0) {
	return(-1);
    }
    return(0);
}
