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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/_gcolx.c,v 1.
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
static long *offsets = NULL;

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

#undef MAX
#define MAX(a,b)        (((a)>(b))?(a):(b))

/*
 *  Calling Sequence:
 *      node_count = tree_count_list(from_node, this_node, node_list);
 *
 *  Description:
 *      Calculate node list below this_node
 *      using spanning tree algorithm.
 *
 *  Parameters:
 *      int from_node   the node above this_node in spanning tree.
 *	int this_node   node from which to calculate tree.
 *      int node_list[] list of logical nodes below this_node in tree.
 *
 *  Returns:
 *      int node_count  number of nodes in node_list
 */

int 
tree_count_list(from_node, this_node, nlist)
	int from_node, this_node;
	int nlist[NLIST_SIZE]; 

{

        int node_count;
        int i, pow, rem;
        unsigned int gval, gsval, tmask, bitset, gdestproc, destproc;
        
        long pt;

        node_count = 0;
        pt = power_of_two(&pow,&rem);
        if (this_node < (1<<pow)) {
            gval = bintogray(this_node);       
            gsval = bintogray(from_node);
            if ((tmask = gval ^ gsval) != 0) {
                bitset = 0;
                while (tmask != 1) {
                    tmask = tmask >> 1;
                    bitset++;
                }
                for (i = bitset -1; i >= 0; i--) {
                    toggle(gval, i, &gdestproc);
                    nlist[node_count++] = graytobin(gdestproc);
                }
            }
            if (pt != 1) {
                toggle(gval, pow, &gdestproc);
                destproc = graytobin(gdestproc);
                if ((destproc < mcmsg_numnodes) &&
                    (destproc != from_node)) {
                    nlist[node_count++] = destproc;
                }
            }   
        }
        return node_count;
}
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

static 
int
sum_tree_recv(from_node,this_node,inlen)
        int from_node,this_node,inlen[];

{
    int nlist[NLIST_SIZE];
    int count,ncnt,i;

    if ((count = tree_count_list(from_node,this_node,nlist)) == 0) {
        return(count);
    }
    ncnt = count;
    for (i = 0; i < ncnt; i++) {
        count += sum_tree_recv(this_node, nlist[i],inlen);
        if (((tmpbuf[msgindx] = malloc(MAX(inlen[nlist[i]],1))) != NULL) &&
            ((infobuf[msgindx] = (info *)malloc(sizeof(info))) != NULL) &&
            ((msgids[msgindx] = _irecvx(GATHER_1 + nlist[i], tmpbuf[msgindx],
			inlen[nlist[i]],-1,-1,infobuf[msgindx])) >= 0)){
                    msgindx++;
        } else {
            nomsgids[nomsgindx++] = nlist[i];
        }
    }
    return(count);
}

/*
 *  Calling Sequence:
 *      error = nx_gatherds(inbuf, outbuf, inlen, root)
 *
 *  Description:
 *      Gather fixed size data from all nodes in a contiguous output buffer
 *
 *  Parameters:
 *      int inbuf       input data
 *      int outbuf      output  buffer valid only on root node.
 *      int inlen[]     array of lengths of input datas
 *      int root        root of gather tree
 *
 *  Returns:
 *      int error       -1 on error, otherwise 0
 */

int
nx_gatherds(inbuf, outbuf, inlen, root)
char *inbuf, *outbuf;
long *inlen;
long root;

{
    long go, pow, rem, ncnt, ncount, nlist[NLIST_SIZE],tmpmid[NLIST_SIZE];
    long retnode, gmid, from_node;
    int i,j, msgrecvd;


    if (mcmsg_numnodes == 1) {
        bcopy(inbuf,outbuf,inlen[0]);
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
	for(i=0;i<NLIST_SIZE;i++) {   /* may need these below so save them*/
		tmpmid[i] = _alloc_mid();
		if(tmpmid[i] == -1) {
			return(-1);
		}
	}
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
            if (((tmpbuf[msgindx] = malloc(MAX(inlen[nlist[j]],1))) != NULL) &&
                ((infobuf[msgindx] = (info *)malloc(sizeof(info))) != NULL)) {
                if((msgids[msgindx] = _irecvx(GATHER_1+nlist[j],tmpbuf[msgindx],
					inlen[nlist[j]],-1,-1,infobuf[msgindx])) >= 0){
                    msgindx++;
                } else {
                    nomsgids[nomsgindx++] = nlist[j];
                }
            }
        }
    }
	for(i=0;i<NLIST_SIZE;i++) {   /* incase we need these */
		_free_mid(tmpmid[i]);
	}
    if (mcmsg_mynode != root) {
        if (_csend(GATHER_1 + mcmsg_mynode, 
			inbuf, inlen[mcmsg_mynode], 
			retnode, mcmsg_ptype) < 0) {
	    return(-1);
	}
    } else {
        if (_csend(GATHER_2,(char*) &go,sizeof(long),-1,mcmsg_ptype) < 0) {
	    return(-1);
	}
        bcopy(inbuf,outbuf,inlen[root]);
    }
    i = 0;
    for (msgrecvd=0; msgrecvd < ncount; msgrecvd++ ) {
        while (!msgdone(msgids[i])) {
			if(msgindx != 0) {
				i = ++i % msgindx;
			} else {
				i = 0;
			}
        }
        if (mcmsg_mynode != root) {
            if (_csend(infobuf[i]->type, tmpbuf[i], infobuf[i]->len, 
			retnode, mcmsg_ptype) < 0){
		return(-1);
	    }
        } else {
            bcopy(tmpbuf[i],&outbuf[offsets[(infobuf[i]->type - GATHER_1)]],infobuf[i]->len);
        }
        if ((msgindx + msgrecvd) < ncount ) {
/*
			printf("node: %x, rec_node = %x, len = %x\n",
				mynode(),nomsgids[nomsgindx],inlen[nomsgids[nomsgindx]]);
*/
			free(tmpbuf[i]);
            tmpbuf[i] = malloc(MAX(inlen[nomsgids[nomsgindx-1]],1));
            if((msgids[i] = irecvx(GATHER_1 + nomsgids[nomsgindx--], tmpbuf[i],
				inlen[nomsgids[nomsgindx]],-1,-1,infobuf[i])) < 0) {
                return(-1);
            }
        } else {
			free(tmpbuf[i]);
			free(infobuf[i]);
			tmpbuf[i] = tmpbuf[--msgindx];
            infobuf[i] = infobuf[msgindx];
            msgids[i] = msgids[msgindx];
        }

		/*
		 * if the msgindx == 0 below then we will be done and it
		 * does not matter what mod ( % ) returns.
		 */
		if(msgindx != 0) {
			i = ++i % msgindx;
		} else {
			i = 0;
		}
    }
    return(0);
}

/*
 *      _gcolx.c
 *
 * Globally collects(concatenates) a contribution of specified length from
 * each node in node number order.  By providing the expected length of each
 * contribution, the speed of this operation is greatly improved due to the
 * reduced overhead of calculating where each contribution belongs in the
 * output buffer.
 *
 * input..
 *
 *   x         pointer to the input buffer to be used in the operation.
 *   xlens     array of the lengths of each contribution in bytes.
 *
 * output..
 *
 *   y         pointer to the output buffer to be used in the operation.
 *             y contains the desired result.
 *
 */

long
_gcolx(x, xlens, y)
	char x[];
	long xlens[];
	char y[];
{
	long go, pow, rem, ncnt, ncount, nlist[NLIST_SIZE]; 
	long retnode, gmid, from_node;
	int i,j, msgrecvd, xmsgrecvd;


	if (mcmsg_numnodes == 1) {
		bcopy(x,y,xlens[0]);
		return(0);
	}
	if (offsets == NULL) {
		if((offsets = (long *)malloc((mcmsg_numnodes+1)*sizeof(long))) == NULL){
			return(-1);
		}
	}
	offsets[0] = 0;
	for (i = 1; i <= mcmsg_numnodes; i++) {
		offsets[i] = offsets[i - 1] + xlens[i - 1];	
	}

	nx_gatherds(x, y, xlens, 0);

	if (mcmsg_mynode == 0) {
		if (_csend(COMM_TYPE_2, y,offsets[mcmsg_numnodes],-1,mcmsg_ptype) < 0) {
			return(-1);
		}
	} else {
		if (_crecv (COMM_TYPE_2, y, offsets[mcmsg_numnodes]) < 0) {
			return(-1);
		}
	}	
	return(0);
}
