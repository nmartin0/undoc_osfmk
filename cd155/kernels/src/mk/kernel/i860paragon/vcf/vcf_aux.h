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
 
#ifndef _AUX_H_
#define _AUX_H_

#define HASH_INDEX(x, y) ((unsigned)((x << 16) | y) % (unsigned)vcf_map_size)

extern int  vcf_give_pools(vcf_chan *chan, unsigned send_pool_size,
														unsigned recv_pool_size);
extern vcf_chan  *vcf_find_chan_in_hash(unsigned id, unsigned peer);
extern vcf_chan  *vcf_alloc_chan(unsigned int id, unsigned int send_pool_size,
																 unsigned int recv_pool_size,
																 unsigned int node);
extern void  vcf_remove_chan_from_hash(vcf_chan *ch);
extern void  vcf_give_chan(vcf_chand chan);
/*
extern void  vcf_remove_elt(vcf_map_elt *elt);
extern void  vcf_remove_elt_chan(vcf_chand chan);
extern void  vcf_give_chan(vcf_chand chan);
extern int   vcf_give_pools(vcf_chan *chan, unsigned send_pool_size,
                            unsigned recv_pool_size);
extern vcf_map_elt  *vcf_find_chan_elt(unsigned id, unsigned peer);
extern vcf_chand    vcf_alloc_chan(unsigned id, unsigned send_pool_size,
                                   unsigned recv_pool_size, unsigned node);
*/
#define VCF_INLINE
#ifdef VCF_INLINE

#define my_pde(addr, dirbase) (((long *) (dirbase))[pdenum(addr)])
#define my_pte(addr, dirbase) \
	(((long *) ((unsigned int) my_pde(addr, dirbase) & \
		    INTEL_PTE_PFN))[ptenum(addr)])
#define my_validate(addr, dirbase) (((unsigned int) my_pte(addr, dirbase) & INTEL_PTE_PFN) | (addr & ~(INTEL_PTE_PFN)))

#else

extern unsigned long my_validate(unsigned long addr, unsigned long dirbase);

#endif

extern int  vcf_errno, vcf_val_status;
extern unsigned int  vcf_map_size;
extern vcf_req  *vcf_free_reqs;

#endif
