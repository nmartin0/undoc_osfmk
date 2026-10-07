/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 *	INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license 
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance with
 *	the terms of that agreement.
 *	Copyright 1991 Intel Corporation.
 *
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/msgp/msgp_atdt.c,v 1.19 1995/02/07 21:27:38 joel Exp $
 * 
 * $Log: msgp_atdt.c,v $
 * Revision 1.19  1995/02/07  21:27:38  joel
 *  Reviewer: 	sean
 *  Risk: 		low
 *  Benefit or PTS #: 10850
 *  Testing: 	test case, messge EATs, parallel SATs
 *  Module(s):	msgp_atdt.c version 1.19 msgp_provide version 1.48
 *
 * Revision 1.18  1995/01/20  18:24:11  joel
 *  Reviewer:      sean
 *  Risk:          low
 *  Benefit or PTS #: 11968 sender exits before recv_continue (comment only here)
 *  Testing:       testcase, message EATs
 *  Module(s): msgp_inq.c msgp_nxdat.c msgp_atdt.c mcmsg_nx.h nxlib.c
 *             libc/errlst.c  server/sys/errno.h
 *
 * Revision 1.17  1994/11/18  20:46:35  mtm
 * Copyright additions/changes
 *
 * Revision 1.16  1994/09/15  15:57:26  joel
 *  Reviewer: tnt
 *  Risk: low
 *  Benefit or PTS #:10506
 *  Testing: Message EATs
 *  Module(s): msgp_atdt.c
 *
 * Revision 1.15  1994/06/06  17:11:31  joel
 *  Reviewer: Regnier
 *  Risk: low
 *  Benefit or PTS #: forgot #if BIGPKTS around my last change
 *  Testing: message eats and developer tests
 *  Module(s): msgp_atdt.c
 *
 * Revision 1.14  1994/06/03  22:11:10  joel
 *  Reviewer: Greg Regnier
 *  Risk:low
 *  Benefit or PTS #: 9670
 *  Testing: noc developer tests and message EATS
 *  Module(s):msgp_nxdat.c msgp_atdt.c
 *
 * Revision 1.13  1994/05/04  18:46:12  regnier
 *  Merged bug fix for PTS #9181 from R1.2 into mainline.
 *
 *    Reviewer(s): tnt, terry
 *    Risk:        Low
 *    Benefit:     PTS #9181
 *    Testing:     Message Passing and MISC Eats + developer tests
 *    Module(s):   i860paragon/msgp/msgp_atdt.c
 *
 * Revision 1.12  1994/04/06  16:47:38  regnier
 * Fix race condition where termination messages are queued and sent
 * after the process has exited. Problem seen on System BALANCE SAT on
 * an MP3 system. Not seen in R1.2 but potential for problem is large.
 *
 *   Reviewer:    tnt
 *   Risk:        Low
 *   Benefit:     PTS #8848
 *   Testing:     Message Passing and MISC Eats, Parallel SATS
 *   Module(s):   i860paragon/msgp/msgp_task.c, msgp_inq.c, msgp_atdt.c
 *
 * Revision 1.11  1994/03/23  23:10:41  regnier
 *  Reviewer: None.
 *  Risk: Low
 *  Benefit: Integrate fix for #8134 from R1.2 branch (uninit'ed vars)
 *  Testing: lat, irxnode
 *  Module(s): msgp_atdt.c
 *
 * Revision 1.10  1994/02/28  18:51:02  regnier
 *   Reviewer(s): prp, terry, tnt, joel
 *   Risk:        Medium
 *   Benefit:     Better communication performance
 *   Testing:     EATs, SATs, irxnode and latency
 *   Module(s):   mk/kernel/i860paragon/model_dep.c
 *                mk/kernel/i860paragon/msgp/msgp_atdt.c
 *                mk/kernel/i860paragon/msgp/msgp_boot.c
 *                mk/kernel/i860paragon/msgp/msgp_dispatch.s
 *                mk/kernel/i860paragon/msgp/msgp_hw.h
 *                mk/kernel/i860paragon/msgp/msgp_hw.s
 *                mk/kernel/i860paragon/msgp/msgp_hwi.c
 *                mk/kernel/i860paragon/msgp/msgp_hwr1.c
 *                mk/kernel/i860paragon/msgp/msgp_hwr2.c
 *                mk/kernel/i860paragon/msgp/msgp_hws1.c
 *                mk/kernel/i860paragon/msgp/msgp_hws2.c
 *                mk/kernel/i860paragon/msgp/msgp_inq.c
 *                mk/kernel/i860paragon/msgp/msgp_ipc.c
 *                mk/kernel/i860paragon/msgp/msgp_mp.c
 *                mk/kernel/i860paragon/msgp/msgp_nxctl.c
 *                mk/kernel/i860paragon/msgp/msgp_nxdat.c
 *                mk/kernel/i860paragon/mcmsg/mcmsg_hw.h
 *                mk/kernel/i860paragon/mcmsg/mcmsg_mp.c
 *                mk/kernel/i860paragon/mcmsg/mcmsg_select.h
 *                mk/kernel/i860paragon/mcmsg/mcmsg_trace.c
 *
 * Revision 1.9  1993/10/27  20:54:55  joel
 * Reviewer: joel
 * Risk:	  low
 * Benefit:  fix race condition.
 * Testing:  adaptive buffer protocol stress testing.
 * Module:	  msgp_atdt.c
 *
 * Moved putting pid on avail_need list from recv_nda to send_naa so provide
 * won't hang walking the list and finding a process not yet ready for buffers.
 *
 * Revision 1.8  1993/10/27  18:52:55  joel
 * Reviewers  Mike Andrews "Looks OK, but need 2-3 hours to be sure"
 *            Terry Prickett "About 1/2 done reviewing"
 *         This week check in deadline 10 minutes.
 * Risk    Medium
 * PTS     None Developer testing.
 * Benefit stress testing adaptive buffer protocol with small -noc value showed
 *         some race conditions.
 * Testing Stress testing.
 *
 * Modules: msgp_provide.c msgp_nxdat.c msgp_atdt.c
 *
 * Revision 1.7  1993/10/22  01:54:11  joel
 * Finish developer stress testing. 65 nodes 1 noc 1200 loops all to 1, noc to 1.
 *
 * Revision 1.6  1993/10/21  00:56:17  joel
 * Some recv_give fields needed to be zero'd.  Also moved the code the takes a
 * detached pid off the avail_need list from recv_nda to find_detachee so
 * mcmsg_provide can't get stuck on it.
 *
 * Revision 1.5  1993/10/20  20:58:35  joel
 * Add code to remove pid_si from avail_need in recv_nda().
 *
 * Revision 1.4  1993/10/20  18:40:59  joel
 * Fix uninitialized variable.
 *
 * Revision 1.3  1993/10/19  21:38:19  joel
 * Lots of fixes to Adaptive message buffer protocol based on developer testing.
 *
 * Revision 1.2  1993/10/11  23:36:30  joel
 * Add a few blank lines for readability.
 *
 * Revision 1.1  1993/10/04  22:44:41  joel
 * Initial revision
 *
 *
 */

/*
 * mcmsg_atdt.c
 *
 * Attach/Detach remote processes (assign message buffers to them)
 */


#define	MCMSG_MODULE	MCMSG_MODULE_NX
#define XPKTMIN (mt->applinfo.pkt_size + sizeof(xmsg_t))

#include <i860paragon/mcmsg/mcmsg_ext.h>
#include <i860paragon/msgp/msgp.h>
#include <i860paragon/mcmsg/mcmsg_hw.h>
#include <i860paragon/mcmsg/mcmsg_nx.h>


mcmsg_attach(mt, si)
        mcmsg_task_t *mt;
        select_item_t   *si;
{
        select_item_t   *sh;
        select_item_t   *st;

        si->method = MCTRL_NAT;

        /* store attach request */
        st = mt->attach_wait;
        mt->attach_wait = si;

        /* if queue was empty, send request now */
        if (st == 0) {
                si->link = si;
#if   BIGPKTS
				/*
				 * Handle overlap.
				 */
				if (si->nxrq.pid_si->ppid.sent_nat == 0) {
					mcmsg_trace_debug("attach send NAT",
							2, si, si->nxrq.pid_si->ppid.node, 0, 0);
					si->nxrq.pid_si->ppid.sent_nat = 1;
             		mcmsg_send(mt, MCTRL_NAT, si, 0x1111);
				}
#else BIGPKTS
                mcmsg_send(mt, MCTRL_NAT, si, 0);
#endif BIGPKTS
        } else {
                sh = st->link;
                assert(sh != 0 && sh->method != 0xdead);
                si->link = sh;
                st->link = si;
        }
}


mcmsg_send_nat(mt, ctl, si)
        mcmsg_task_t    *mt;
        select_item_t   *si;
{
        register unsigned long  hdr1;
        register unsigned long  hdr2;
        register unsigned long  hdr3;

        hdr1 = MCTRL_NAT;
        hdr2 = 8;
        hdr3 = mt->pid;
        mcmsg_trace_debug("send_nat si pid_si", 2, si, si->nxrq.pid_si, 0, 0);
        mcmsg_trace_send(hdr1,hdr2,hdr3, 2, si, si->nxrq.pid_si->ppid.node);
        assert(si != 0);
        assert(si->nxrq.pid_si != 0);
		assert(si->nxrq.pid_si->ppid.send_ready == 0);
        assert(si->mcmsg_task == mt);
        send2(si->nxrq.pid_si->ppid.route, 0);
        send2(hdr1, hdr2);
        send2eod(hdr3, si->nxrq.pid_si->value);
}

unsigned long
mcmsg_attach_buffer(mt,dpid_si)
        mcmsg_task_t    *mt;
        select_item_t   *dpid_si;
{
        register unsigned long  avail;

        /* put process on attached list */
        assert(dpid_si->ppid.attach_link == 0);
        if (mt->attached != 0) {
            dpid_si->ppid.attach_link = mt->attached;
        }
        mt->attached = dpid_si;

	/* mt->reserved == (noc * memory_each) - (the amount of memory 
	   allocated to the memory_each buffers of the attached correspondents)
	   mt->assigned == (noc * memory_each) - (the amount of memory actually
	   used to buffer messages in xmsgs). Thus at task_init time they 
	   are equal, but when messages are buffered assigned is 
	   usually > reserved.
	 */
        /* calculate buffer information */
        if (mt->reserved  > mt->applinfo.memory_each) {
            mt->reserved -= mt->applinfo.memory_each;
            avail = mt->applinfo.memory_each;
        } else {
	    if ((avail = mt->provided - mt->assigned) > 0) {
		if ((avail + mt->reserved) > mt->applinfo.memory_each) {
			avail = mt->applinfo.memory_each - mt->reserved;
		}
		mt->assigned += avail;
		avail += mt->reserved;
	    } else {
            	avail = mt->reserved;
	    }
            mt->reserved = 0;

	    /* if avail < XPKTMIN then mark pid_si as not detachable until 
	     * mcmsg_provide gives it enough to send at least one msg
	     */
	    if (avail < XPKTMIN) {
		dpid_si->ppid.sub_pid_si = (select_item_t *) 1;
	    }
            /* put process on avail_need list in send_naa() */
        }
	return(avail);
}

select_item_t *
mcmsg_find_detachee(mt, pid_si)
register mcmsg_task_t   *mt;
register select_item_t  *pid_si;
{
        register select_item_t  *st;
        register select_item_t  *lru;
        register unsigned long  t;
	
        lru = mt->attached;
        assert(lru != 0);

	/* 
         * sub_pid_si is overloaded in mcmsg_send_naa(). if it == 1 then 
	 * the pid has never gotten enough buffer to send 1 packet and 
	 * should not be detached until it does.  If it is 0 it can be
	 * detached.  Otherwise it is already being detached and sub_pid_si
	 * is the replacement pid
	 */
        assert((t = MAXLOOP) != 0);
        while((lru) && (lru->ppid.sub_pid_si)) {
	    lru = lru->ppid.attach_link;
	    assert(t-- != 0);
	}
	if ((lru) && ((st = lru->ppid.attach_link) != 0)) {
            assert((t = MAXLOOP) != 0);
            while(st->ppid.attach_link != 0) {
                if ((st->ppid.sub_pid_si == 0) &&
                    (st->ppid.when_recvd <lru->ppid.when_recvd) &&
                    ((st->ppid.recv_total >= XPKTMIN) ||
                     (lru->ppid.recv_total < XPKTMIN))) {
                        lru = st;
                } 
	        st = st->ppid.attach_link;
                assert(t-- != 0);
	    }
        } 
        if ((lru) && (lru->ppid.sub_pid_si == 0)) {
            /* store replacement pid_si in detachee for NDA */
	    lru->ppid.sub_pid_si = pid_si;

            /* transfer detachee's give to replacement pid 
             * would like to put in recv_give, but then if this node sends
             * a message to pid_si before sending a NAA the give will go with it
             */
            pid_si->ppid.recv_total = lru->ppid.recv_give;

            lru->ppid.recv_give = lru->ppid.recv_target = 0;

	    /*
	     * Remove from avail_need list if present
	     */

	    if ((mt->avail_need) && 
	            (lru->ppid.avail_link) &&
	            (lru->ppid.rk_recv_pid == 0)) { /* skip NXS pids */
	        st = mt->avail_need;
	        for(;;) {
	    	    assert(st->method == SELMETH_PID);
		    if (lru == st->ppid.avail_link) {
			assert(lru->ppid.avail_link != 0);
			if (lru == st) {
	       			assert(mt->avail_need == lru);
				mt->avail_need = 0;
			} else {
				st->ppid.avail_link =
				 lru->ppid.avail_link;
				if (mt->avail_need == lru) {
				    mt->avail_need = st;
				}
			}
			lru->ppid.avail_link = 0;
			break;   
		    }
		    st = st->ppid.avail_link;
		    if (st == mt->avail_need) 
			break;
	        }
	    }
	} else {
	    /* 
	     * all attached pids already being replaced,
             * leave NAT pid_si on attach_pending list for later.
	     */
	    if (mt->attach_pend == 0) {
		mt->attach_pend = pid_si;
                mcmsg_trace_debug("new attach_pend new pid", 
			2, pid_si,pid_si->value,0,0);
	        assert(pid_si->ppid.att_pend_link == 0);
	    } else if (mt->attach_pend != pid_si) { /* ! called from recv_nda */
		st = mt->attach_pend;
           	assert((t = MAXLOOP) != 0);
            	while(st->ppid.att_pend_link != 0) {
		    st = st->ppid.att_pend_link;
               	    assert(t-- != 0);
            	} 
		st->ppid.att_pend_link = pid_si;
                mcmsg_trace_debug("another attach_pend prev new pid", 
			3, st,pid_si,pid_si->value,0);
	        assert(pid_si->ppid.att_pend_link == 0);
	    }
	    lru = 0;
	}
	return(lru);
}
	 

mcmsg_recv_nat(hdr1, hdr2)
        register unsigned long  hdr1;
        register unsigned long  hdr2;

{
        register unsigned long source_pid;
        register unsigned long dest_pid;
        register mcmsg_task_t   *mt;
        register select_item_t  *pid_si;
        register select_item_t  *st;
        register select_item_t  *lru;
        register select_item_t  *last_si;
        register unsigned long  t;

        recv2(source_pid, dest_pid);
	mcmsg_trace_recv(hdr1, hdr2, source_pid, 1, dest_pid, 0);

        /* lookup destination process */
        if ((st = mcmsg_selector_lookup_si(&mcmsg_local_sel,  
                        dest_pid)) == 0) {
                mcmsg_trace_recv(hdr1, hdr2, source_pid, 0, 0, 0);
                mcmsg_trace_drop("dest pid not found", dest_pid);
                mcmsg_msg_drop++;
                return;
        }
        mt = st->mcmsg_task;

        /* look up source process */
        pid_si = mcmsg_lookup_remote(mt, source_pid);
        assert(pid_si != 0);
        assert(mt->assigned > 0);
        
        /* if a buffer is available mcmsg_send_naa() will send it */
        if (mt->reserved > 0) {
                mcmsg_send(mt,MCTRL_NAA,pid_si,mcmsg_attach_buffer(mt,pid_si));
        } else {
            /* detach LRU process */
            lru = mcmsg_find_detachee(mt, pid_si);
            if (lru) {
                mcmsg_trace_debug("recv_nat send NDT old new pid", 
			3, lru,pid_si,pid_si->value,0);
		mcmsg_send(mt, MCTRL_NDT, lru);
	    }
        }
}


mcmsg_send_ndt(mt, ctl, dpid_si)
        mcmsg_task_t    *mt;
        register select_item_t   *dpid_si;
{
        register unsigned long  hdr1;
        register unsigned long  hdr2;
        register unsigned long  hdr3;

        /* send detach request */
        hdr1 = MCTRL_NDT;
        hdr2 = 16;
        hdr3 = mt->pid;
        assert(dpid_si != 0);
        mcmsg_trace_send(hdr1, hdr2, hdr3, 2, dpid_si,0);
        send2(dpid_si->ppid.route, 0);
        send2(hdr1, hdr2);
        send2eod(hdr3, dpid_si->value);
        mcmsg_trace_debug("send_ndt pid_si pid", 2, dpid_si,dpid_si->value,0,0);
}



mcmsg_recv_ndt(hdr1, hdr2)
        register unsigned long  hdr1;
        register unsigned long  hdr2;
{
        register unsigned long source_pid,dest_pid;
        register mcmsg_task_t   *mt;
        register select_item_t  *spid_si, *dpid_si;

        recv2(source_pid, dest_pid);
	mcmsg_trace_recv(hdr1, hdr2, source_pid, 1, dest_pid, 0);

        /* look up destination process */
        if ((dpid_si = mcmsg_selector_lookup_si(&mcmsg_local_sel,
                        dest_pid)) == 0) {
                mcmsg_trace_recv(hdr1, hdr2, source_pid, 0, 0, 0);
                mcmsg_trace_drop("dest pid not found", dest_pid);
                mcmsg_msg_drop++;
                return;
        }
        mt = dpid_si->mcmsg_task;

        /* look up source process */
        spid_si = mcmsg_lookup_remote(mt, source_pid);
        assert(spid_si != 0);

        /* mark detached and send ACK
		 *
		 * Since NDA is used for exiting and the related
		 * data structures (mt, pid_si) may get deallocated,
		 * don't rely on them being there if the send is queued. gjr
		 */
        spid_si->ppid.send_ready = 0;
#if BIGPKTS 
        spid_si->ppid.sent_nat = 0;
#endif /* BIGPKTS */
        mcmsg_send(mt, MCTRL_NDA,
		           spid_si->ppid.route,
		           spid_si->ppid.send_avail,
		           dpid_si->value,
		           spid_si->value);

		spid_si->ppid.send_avail = 0;
	
	/* send re-attach request if unsent send waiting for buffer */
	if (spid_si->ppid.send_wait && 
	    (spid_si->ppid.send_wait->nxrq.offset == 0)) {
        	mcmsg_trace_debug("recv_ndt send_wait ",1,spid_si,0,0,0);
		dpid_si = spid_si->ppid.send_wait;
		if(dpid_si == dpid_si->link) {
		    spid_si->ppid.send_wait = 0;
		} else {
		    spid_si->ppid.send_wait = spid_si->ppid.send_wait->link;
		}
		mcmsg_attach(mt,dpid_si);
	}
}



mcmsg_send_nda(mt, ctl, route, send_avail, src_pid, dst_pid)
        mcmsg_task_t    *mt;
        register unsigned long  route;
        register unsigned long  send_avail;
        register unsigned long  src_pid;
        register unsigned long  dst_pid;
{
        register unsigned long  hdr1;
        register unsigned long  hdr2;
        register unsigned long  hdr3;

        /* return available memory */
        hdr1 = MCTRL_NDA | (send_avail << 11);
        hdr2 = 16;
        hdr3 = src_pid;
        mcmsg_trace_send(hdr1, hdr2, dst_pid, 2, src_pid, 0);
        send2(route, 0);
        send2(hdr1, hdr2);
        send2eod(hdr3, dst_pid);
        mcmsg_trace_debug("send_nda src dst avl", 3,
		                  src_pid, dst_pid, send_avail, 0);
}



mcmsg_recv_nda(hdr1, hdr2)
        register unsigned long  hdr1;
        register unsigned long  hdr2;

{
        register unsigned long source_pid;
        register unsigned long dest_pid;
        register mcmsg_task_t   *mt;
        register select_item_t  *spid_si;
        register select_item_t  *dpid_si;
        register select_item_t  *new_si;
        register select_item_t  *pid_si;
        register unsigned long  t;

        recv2(source_pid, dest_pid);
	mcmsg_trace_recv(hdr1, hdr2, source_pid, 1, dest_pid, 0);

        /* look up destination process */
        if ((dpid_si = mcmsg_selector_lookup_si(&mcmsg_local_sel,
                        dest_pid)) == 0) {
                mcmsg_trace_recv(hdr1, hdr2, source_pid, 0, 0, 0);
                mcmsg_trace_drop("dest pid not found", dest_pid);
                mcmsg_msg_drop++;
                return;
        }
        mt = dpid_si->mcmsg_task;

        /* look up source process */
        spid_si = mcmsg_lookup_remote(mt, source_pid);
        assert(spid_si != 0);

        /* remove from attached list */
        if (mt->attached == spid_si) {
            mt->attached = spid_si->ppid.attach_link;
        } else {
            pid_si = mt->attached;
            assert((t = MAXLOOP) != 0);
            while (pid_si->ppid.attach_link != spid_si) {
                assert(t-- != 0);
                pid_si = pid_si->ppid.attach_link;
		if (pid_si == 0) break;
            }
            if (pid_si) {
		 pid_si->ppid.attach_link = spid_si->ppid.attach_link;
	    }
        }
        spid_si->ppid.attach_link = 0;

        /* adjust buffer info */
        mt->reserved += ((hdr1 >> 11) & ~(0x1f)) + spid_si->ppid.recv_give;
        spid_si->ppid.recv_give = spid_si->ppid.recv_total = 0;

        /* grab the replacement pid_si */
        new_si = spid_si->ppid.sub_pid_si;
        spid_si->ppid.sub_pid_si = 0;

        mcmsg_trace_debug("recv_nda old new",2,spid_si,new_si,0,0);

	if ((new_si == 0) && (mt->attach_pend)) {
	    new_si = mt->attach_pend;
	    mt->attach_pend = mt->attach_pend->ppid.att_pend_link;
            new_si->ppid.att_pend_link = 0;
	}
		/* new_si == 1 means the spid_si should not be selected by
		 * mcmsg_find_detachee() for detachment.  If we`ve 
		 * recv'd a NDA from a spid whose sub_pid_si was 1 it must
		 * be because the spid is exiting (probably because of ^C)
		 */
        if ((new_si != 0) && (new_si != (select_item_t *) 1 )) {
	       /* used recv_total here instead of recv_give so outbound msg
       		* wouldn't give buffer before we send NAA
		* see mcmsg_relinquish() in msgp_nxdat.c
         	*/
            mt->reserved += new_si->ppid.recv_total;

            new_si->ppid.recv_give = new_si->ppid.recv_total = 0;
            mcmsg_trace_debug("recv_nda send NAA to ",1,new_si,0,0,0);
            mcmsg_send(mt, MCTRL_NAA, new_si,mcmsg_attach_buffer(mt, new_si));

        }
	/* look for other pids waiting to be attached */
	while((mt->attach_pend) &&
	      (mt->attached) &&
              ((pid_si = mcmsg_find_detachee(mt, mt->attach_pend)) != 0)) {
            mcmsg_trace_debug("recv_nda NDT to",2,pid_si,mt->attach_pend,0,0);
            mcmsg_send(mt, MCTRL_NDT, pid_si);
            new_si = mt->attach_pend;
	    mt->attach_pend = mt->attach_pend->ppid.att_pend_link;
	    new_si->ppid.att_pend_link = 0;
        }

}




mcmsg_send_naa(mt, ctl, dpid_si, avail)
        mcmsg_task_t    *mt;
        register select_item_t   *dpid_si;
        register unsigned long avail;

{
        register unsigned long  hdr1;
        register unsigned long  hdr2;
        register unsigned long  hdr3;
        register select_item_t  *st;
        register select_item_t  *sh;

	if (dpid_si->ppid.sub_pid_si == 0 ||
	    dpid_si->ppid.sub_pid_si == (select_item_t *) 1) {
        	dpid_si->ppid.recv_target = mt->applinfo.memory_each;
	}
        if (avail < mt->applinfo.memory_each) {
	    if (dpid_si->ppid.avail_link == 0) {
                if (mt->avail_need == 0) {
                    dpid_si->ppid.avail_link = dpid_si;
                } else {
                    st = mt->avail_need;
                    sh = st->ppid.avail_link;
                    st->ppid.avail_link = dpid_si;
                    dpid_si->ppid.avail_link = sh;
		}
            	/* move to tail of list */
            	mt->avail_need = dpid_si;
            }
	}
        dpid_si->ppid.recv_total = avail;
        hdr1 = (MCTRL_NAA | (avail << 11));
	dpid_si->ppid.recv_give = 0;
        hdr2 = 8 ;
        hdr3 = mt->pid;
        mcmsg_trace_send(hdr1,hdr2,hdr3,2,dpid_si,dpid_si->value);
        send2(dpid_si->ppid.route, 0);
        send2(hdr1, hdr2);
        send2eod(hdr3, dpid_si->value);
        mcmsg_trace_debug("NAA",2,dpid_si,dpid_si->value,0,0);
        return;
}



mcmsg_recv_naa(hdr1, hdr2)
        register unsigned long  hdr1;
        register unsigned long  hdr2;

{
        register unsigned long source_pid;
        register unsigned long dest_pid;
        register mcmsg_task_t   *mt;
        register select_item_t  *spid_si;
        register select_item_t  *dpid_si;
        register select_item_t  *st;
        register select_item_t  *sn;
        register select_item_t  *si;
        register unsigned long  t;

        recv2(source_pid, dest_pid);
	mcmsg_trace_recv(hdr1, hdr2, source_pid, 1, dest_pid, 0);

        /* look up destination process */
        if ((dpid_si = mcmsg_selector_lookup_si(&mcmsg_local_sel,
                        dest_pid)) == 0) {
                mcmsg_trace_recv(hdr1, hdr2, source_pid, 0, 0, 0);
                mcmsg_trace_drop("dest pid not found", dest_pid);
                mcmsg_msg_drop++;
                return;
        }
        mt = dpid_si->mcmsg_task;

        /* look up source process */
        spid_si = mcmsg_lookup_remote(mt, source_pid);
        assert(spid_si != 0);

        /* adjust buffer info */
        assert(spid_si->ppid.send_avail == 0);
        spid_si->ppid.send_ready = 1;
        spid_si->ppid.send_avail = ((hdr1 >> 11) & ~(0x1f));
	assert(spid_si->ppid.send_avail <= mt->applinfo.memory_each);

        /* remove from attach_wait list */
        si = st = mt->attach_wait;
        assert (st != 0);
        assert((t = MAXLOOP) != 0);
        do {
            sn = si->link;
            assert(t-- != 0);
            if (sn->nxrq.pid_si != spid_si) {
                si = si->link;
                continue;
            }
            if (sn == sn->link) {
                mt->attach_wait = 0;
            } else {
                if (sn == st) {
                    mt->attach_wait = si;
                }
                si->link = sn->link;
            }
            sn->link = 0;
            mcmsg_trace_debug("naa scan", 4, si, sn, st, mt->attach_wait);

            /* send pending message */
            assert(sn->method == MCTRL_NAT);
            mcmsg_schedule_send(mt, sn);
        } while (sn != st);

        /* send attach request for any process still waiting */
        if ((st = mt->attach_wait) != 0) {
	     assert(st->link != 0);
#if   BIGPKTS
			/*
			 * Handle overlap.
			 */
			si = st->link;
			if (si->nxrq.pid_si->ppid.sent_nat == 0) {
				mcmsg_trace_debug("recv_naa send NAT",
	                        2, si, si->nxrq.pid_si->ppid.node, 0, 0);
				si->nxrq.pid_si->ppid.sent_nat = 1;
				mcmsg_send(mt, MCTRL_NAT, st->link, 0x2222);
			}
#else BIGPKTS
            mcmsg_send(mt, MCTRL_NAT, st->link, 0);
#endif BIGPKTS
        }
}

