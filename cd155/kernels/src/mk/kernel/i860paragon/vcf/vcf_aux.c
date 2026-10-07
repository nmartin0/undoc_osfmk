/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
#include <i860paragon/vcf/vcf.h>
#include <i860paragon/vcf/vcf_aux.h>
#include <i860paragon/vcf/vcf_pmachine.h>


static void  vcf_init_rjlist(int num_nodes);


int vcf_errno = -1000;
unsigned int vcf_map_size;
vcf_req *vcf_free_reqs;
int vcf_val_status;
vcf_reject  vcf_rjlist[NUM_NODES];


static vcf_req *
vcf_pop_free_req(void)  {
	vcf_req *result = vcf_free_reqs;

	if (vcf_free_reqs)
		vcf_free_reqs = vcf_free_reqs->next;
	return(result);
}


static void
vcf_push_free_req(vcf_req *req)  {
	req->next = vcf_free_reqs;
	vcf_free_reqs = req;
}


static vcf_chand
vcf_pop_free_chan(void)  {
	vcf_chan  *result = vcf_free_chan_stack;
	if (vcf_free_chan_stack != NULL)  {
		vcf_free_chan_stack = vcf_free_chan_stack->hash_next;
		result->sendh = result->recvh = NULL;
	}
	return(result);
}


static void
vcf_push_free_chan(vcf_chand chan)  {
	chan->hash_next = vcf_free_chan_stack;
	vcf_free_chan_stack = chan;
}


static void
vcf_pushreqs(vcf_req *reqs)  {
  vcf_req  *treq;

  while (reqs != NULL)  {
    treq = reqs->next;
    vcf_push_free_req(reqs);
    reqs = treq;
  }
}


/*  Add a channel to the hash table which maps IDs to channels.
 *  Return 1 if it is already there.
 */
static void
vcf_add_chan_to_hash(vcf_chand chan)  {
	unsigned int id = chan->id;
	unsigned int peer = chan->peer;
	unsigned int hash_index = HASH_INDEX(id, peer);
	vcf_chan  *bucket = vcf_chan_tbl[hash_index].hash_bucket;

	if (!bucket)  {
		vcf_chan_tbl[hash_index].hash_bucket = chan;
		chan->hash_next = chan;
		chan->hash_prev = chan;
	} else {
		chan->hash_next = bucket;
		chan->hash_prev = bucket->hash_prev;
		bucket->hash_prev = chan;
		chan->hash_prev->hash_next = chan;
	}
}


/*  Move send and receive request buffer pools from free list to channel.
 */
int
vcf_give_pools(vcf_chan *chan, unsigned send_pool_size,
               unsigned recv_pool_size)  {
	int index;
	vcf_req *temp;

	/*  Pick the appropriate number of request buffers off the free list
	 *  for the send and recv pools.  If there are going to be a large
	 *  number of request buffers for the pools, a more complex memory
	 *  management system might be faster.
	 */
	for (index = 0;  index < send_pool_size;  index++)  {
		if (!(temp = vcf_pop_free_req()))  {
			return(1);
		}
		temp->next = chan->send_pool;
		chan->send_pool = temp;
	}

	for (index = 0;  index < recv_pool_size;  index++)  {
		if (!(temp = vcf_pop_free_req()))  {
			return(1);
		}
		temp->next = chan->recv_pool;
		chan->recv_pool = temp;
	}
	return(0);
}


/*  Find a hash table element based on channel ID, channel peer address.
 */
vcf_chan *
vcf_find_chan_in_hash(unsigned id, unsigned peer) {
	vcf_chan *candidate;
	unsigned int hash_index = HASH_INDEX(id, peer);

	if (candidate = vcf_chan_tbl[hash_index].hash_bucket)  {
		do  {
			if ((candidate->id == id) &&
			    (candidate->peer == peer))  {
				return(candidate);
			}  else
				candidate = candidate->hash_next;
		} while (candidate != vcf_chan_tbl[hash_index].hash_bucket);
	}
	return(NULL);
}


/*  Get a channel from the free list, 
 *  and give it pools.  Put it at the end of the used queue.
 */
vcf_chand
vcf_alloc_chan(unsigned int id, unsigned int send_pool_size,
               unsigned int recv_pool_size, unsigned int node)  {
	vcf_chand chan;

	if (chan = vcf_pop_free_chan())  {
		chan->id = id;
		chan->peer = node;
		if (vcf_give_pools(chan, send_pool_size, recv_pool_size))  {
			vcf_errno = VCF_ENOBUF;
			return(NULL);
		}
		vcf_add_chan_to_hash(chan);
	} else
		vcf_errno = VCF_ENOCTE;
	return(chan);
}


/*  Remove a channel from the hash table.  */
void
vcf_remove_chan_from_hash(vcf_chan *ch) {
	unsigned int id = ch->id;
	unsigned int peer = ch->peer;
	unsigned int hash_index = HASH_INDEX(id, peer);
	boolean root = FALSE;

	if (vcf_chan_tbl[hash_index].hash_bucket == ch)
		root = TRUE;

	/*  If the element's previous is itself, we must NULLify the hash
	 *  table entry pointing to it.
	 */

	if (ch->hash_prev == ch)  {
		if (root)  {
			vcf_chan_tbl[hash_index].hash_bucket = NULL;
		} else
			panic("First element of bucket not in hash table.\n");
	} else {
		/*  The element has a different previous, so we can just remove it.  */
		ch->hash_prev->hash_next = ch->hash_next;
		ch->hash_next->hash_prev = ch->hash_prev;
		if (root)
			vcf_chan_tbl[hash_index].hash_bucket = ch->hash_next;
	}
}


/*  Move chan from used to free list.  Clear id_map entry, if any.  
 *  Move send and recv pools to request buffer free list.
 */
void
vcf_give_chan(vcf_chand chan)  {
	int index;
	boolean found = FALSE;
	vcf_req *tail;
	vcf_chand prev_used;

	chan->close_sbufp = NULL;
	chan->rd_len = 1;
	chan->status = VCF_CS_UNUSED;
	vcf_push_free_chan(chan);

	/*  Again, sacrificing speed of channel close operation for
	 *  memory.
	 */
	vcf_pushreqs(chan->send_pool);
	vcf_pushreqs(chan->recv_pool);

	chan->send_pool = chan->recv_pool = NULL;
}


#ifndef VCF_INLINE
pt_entry_t 
my_validate(unsigned long a, unsigned long dirbase)  {
	pt_entry_t	pt;

	/* Get Page Directory Entry */
	pt = ((pt_entry_t *) dirbase)[pdenum(a)];
	if ((pt & INTEL_PTE_VALID) == 0) {
		val_status = 1;
		return 0;
	}

	/* Get Page Table Entry */
	pt = ((pt_entry_t *)(pt & INTEL_PTE_PFN))[ptenum(a)]; 
	if ((pt & INTEL_PTE_VALID) == 0) {
		val_status = 2;
		return 0;
	}
	if ((pt & INTEL_PTE_WRITE) == 0)  {
		val_status = 3;
		return 0;
	}

	pt = (pt & INTEL_PTE_PFN) | (a & ~(INTEL_PTE_PFN));
	return pt;
}
#endif  /* VCF_INLINE */



/* Tiny little rejection info searching utility.  Do a binary search for
 *   the rejection info.
 */
vcf_reject *
vcf_get_reject_info(unsigned addr)  {
	vcf_reject  *rj = vcf_rjlist;
	unsigned  cnt = vcf_rjlist_len, hc;

	while (hc = cnt/2)  {
		if (rj[hc].peer <= addr)  {
			rj += hc;
			cnt -= hc;
		} else
			cnt = hc;
	}
#if 0
	if (rj->peer != addr)  {
		printf("Fatal error: Node address 0x%8x not found.\n", addr);
		for (hc = 0;  hc < vcf_rjlist_len;  hc++)  {
			printf("rjlist[%d].peer = 0x%8x\n", hc,
			       vcf_rjlist[hc].peer);
		}
	}
#endif
	return(rj);
}


static void
vcf_swap(vcf_reject *left, vcf_reject *right)  {
	vcf_reject middle;

	bcopy(left, &middle, sizeof(vcf_reject));
	bcopy(right, left, sizeof(vcf_reject));
	bcopy(&middle, right, sizeof(vcf_reject));
}


static void
vcf_bsort(vcf_reject *rjlist, unsigned int size)  {
	int index1, index2;

	for (index1 = size - 1;  index1 > 0;  index1--)
		for (index2 = 0;  index2 < index1;  index2++)
			if (rjlist[index2].peer > rjlist[index2 + 1].peer)
				vcf_swap(&rjlist[index2], &rjlist[index2 + 1]);
}


static void
vcf_init_rjlist(int num_nodes)  {
#ifdef DEBUG
	printf("%s: line %d: Entering init_rjlist().\n", __FILE__, __LINE__);
#endif
	if (num_nodes > NUM_NODES)  {
		printf("*ERROR* VCF cannot set up rjlist for >%d nodes.\n", NUM_NODES);
		num_nodes = NUM_NODES;
	}

	vcf_rjlist_len = num_nodes;
	while (num_nodes--)  {
		vcf_rjlist[num_nodes].peer = calculate_route(num_nodes);
		vcf_rjlist[num_nodes].gang_needed = 0;
		vcf_rjlist[num_nodes].ct_full = 0;
		vcf_rjlist[num_nodes].roc_unknown = 0;
		vcf_rjlist[num_nodes].next = NULL;
	}
	vcf_bsort(vcf_rjlist, vcf_rjlist_len);
#ifdef DEBUG
	printf("%s: line %d: Exiting init_rjlist().\n", __FILE__, __LINE__);
#endif
}
