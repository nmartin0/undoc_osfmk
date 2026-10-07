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
#include <i860paragon/vcf/vcf_system.h>
#include <i860paragon/vcf/vcf_pmachine.h>
#include <i860paragon/mcmsg/mcmsg_mp.h>
#undef mcmsg_validate(x)

#define ON_PAGE(addr, page) \
	(((unsigned long)(addr) & ~(VCF_PAGESIZE)) == (page))

extern post_page_t  *mcmsg_user_post_page[];
extern int  mcmsg_user_post_page_out;


/*  
 *  Returns 1 if the buffer 
 *
 *  >  at virtual address buf_vaddr
 *  >  of length buf_len
 *
 *  intersects the page at physical address page_paddr.  Returns 0 otherwise.
 */

static int
intersects_page(unsigned long  buf_vaddr, unsigned int buf_len,
                unsigned long  page_paddr)  {
	unsigned long  buf_paddr;
	unsigned long  buf_start, buf_end;

	buf_end = buf_vaddr + buf_len;
	/* Round down the vaddr to the start of it's page. */
	buf_start = buf_vaddr & ~(VCF_PAGESIZE - 1);
	/*  Check pages in the buffer.  */
	while (buf_start < buf_end)  {
		buf_paddr = (unsigned long)mcmsg_validate_real(buf_start,
																									 (unsigned long)vcf_dirbase);
		if (buf_paddr == 0)
			return(0);
		buf_paddr &= ~((VCF_PAGESIZE-1) | 0xf0000000);
		if (buf_paddr == page_paddr)
			return(1);
		buf_start += VCF_PAGESIZE;
	}
	return(0);
}


/*  Returns 1 if the VCF request's status buffer is on the physical page or
 *  if the request's message buffer intersects the page.  Returns 0 otherwise.
 */
static int
check_req(vcf_req *req, unsigned long phys_addr)  {
	return(ON_PAGE(req->sbufp, phys_addr) ||
	       intersects_page((unsigned long)req->buf, req->len, phys_addr));
}


/*  The following parts of connection memory are wired down using mcmsg_wire()
 *  in vcf_init():
 *
 *  #  System Request Window
 *  #  User Request Window
 *  #  Channel Table
 *  #  Request Pool
 *  #  ID Map
 *  #  ID Map Elements
 *  
 *  The physical address phys_addr of the page being considered for eviction
 *  must be compared with the following parts of connection memory:
 *
 *  #  Message buffers
 *  #  Status buffers
 */

int
vcf_swap(mcmsg_task_t *mt, unsigned long phys_addr)  {
	int vcf_map_size, index;
	vcf_req *temp;

	if (vcf_dirbase == (int *)0)
		return(0);
	phys_addr &= ~((VCF_PAGESIZE-1) | 0xf0000000);
	/*  Check any buffer being prepared for send/recv.  */
#if 0
	if (vcf_usrcomm->status == VCF_WIN_PREPARING_SEND ||
	    vcf_usrcomm->status == VCF_WIN_PREPARING_RECV ||
	    vcf_usrcomm->status == VCF_WIN_RECV ||
	    vcf_usrcomm->status == VCF_WIN_SEND)
		if (intersects_page((unsigned long)vcf_usrcomm->info.send_req.buf,
		                    vcf_usrcomm->info.send_req.len, phys_addr))  {
			return(1);
		}
#else
	if ((mcmsg_user_post_page[0][mcmsg_user_post_page_out].method ==
			 -POST_VCFSEND) ||
			(mcmsg_user_post_page[0][mcmsg_user_post_page_out].method ==
			 -POST_VCFRECV))  {
		if (intersects_page((unsigned long)
		      mcmsg_user_post_page[0][mcmsg_user_post_page_out].arg1,
		      mcmsg_user_post_page[0][mcmsg_user_post_page_out].arg2, phys_addr) ||
				intersects_page((unsigned long)
		      mcmsg_user_post_page[0][mcmsg_user_post_page_out].arg3,
		      4, phys_addr))
			return(1);
	}
#endif /* 0 */
	/*  Check all connections.  */
	for (index = 0;  index < vcf_num_chans;  index++)  {

		/*  Check all send request buffers.  */
		for (temp = vcf_chan_tbl[index].sendh;  temp;  temp = temp->next)
			if (check_req(temp, phys_addr))  {
				return(1);
			}

		/*  Check all receive request buffers.  */
		for (temp = vcf_chan_tbl[index].recvh;  temp;  temp = temp->next)
			if (check_req(temp, phys_addr))  {
				return(1);
			}

		/*  Check status buffers stored in CTE.  */
		if ((vcf_chan_tbl[index].open_sbufp &&
				 ON_PAGE(vcf_chan_tbl[index].open_sbufp, phys_addr)) ||
				(vcf_chan_tbl[index].close_sbufp &&
				 ON_PAGE(vcf_chan_tbl[index].close_sbufp, phys_addr)))  {
			return(1);
		}
	}

	return(0);
}
