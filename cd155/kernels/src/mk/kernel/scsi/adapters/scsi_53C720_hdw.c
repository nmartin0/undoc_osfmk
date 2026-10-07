/*
 *
 * $Copyright
 * Copyright 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright 1994 by Intel Corporation,
 * Santa Clara, California.
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and that
 * both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Intel not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * 
 * INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */
/*
 * HISTORY
 * $Log: scsi_53C720_hdw.c,v $
 * Revision 1.6.8.1  1995/06/11  18:34:16  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.6  1995/04/07  21:23:31  jerrie
 * Added missing splx() function calls.
 *
 *  Reviewer:	   Vineet Kumar
 *  Risk:		   Low
 *  Benefit or PTS #: Prevents the possibility of a hang
 *  Testing:	   Ran four kernels: nica/nicb, +assert/-assert
 *  Module(s):	   scsi_53C720_hdw.c
 *
 * Revision 1.5  1995/03/28  23:19:57  jerrie
 * Increment chain_depth statistic whenever an ior is added to the front of
 * the tgt->ior chain during error handling.  When the ior is pulled from the
 * chain and re-issued by the scmumble_start() routine, the chain_depth is
 * decremented.  This keeps chain_depth from becoming a negative value during
 * error handling.
 *
 *  Reviewer:	   Self
 *  Risk:		   Low
 *  Benefit or PTS #: Keeps statistics variables accurate during error handling
 *  Testing:	   Developer
 *  Module(s):	   scsi_53C720_hdw.c
 *
 * Revision 1.4  1995/03/28  17:29:19  jerrie
 * Added assert to siop_scheduler to insure that the target queue full flag
 * is clear.  This makes sure requests are not scheduled for a full device.
 *
 *  Reviewer:	   Vineet Kumar
 *  Risk:		   Low
 *  Benefit or PTS #: Improved debugging
 *  Testing:	   Developer
 *  Module(s):	   scsi_53C720_hdw.c
 *
 * Revision 1.3  1995/03/28  01:42:19  jerrie
 * Changed cache flush to check of sdb_configuration.
 *
 *  Reviewer:	   Vineet Kumar
 *  Risk:		   Low.
 *  Benefit or PTS #: 12827
 *  Testing:	   Developer
 *  Module(s):	   See PTS report.
 *
 * Revision 1.2  1995/03/14  23:51:06  jerrie
 *  Reviewer:         Jerrie Coffman, Vineet Kumar, Richard Griffiths
 *  Risk:             High
 *  Benefit or PTS #: Add SCSI-16 daughter board support.
 *  Testing:          Ran PFS, RAID, fileio, and tape EATs.
 *  Module(s):        Too numerous to mention.  See Jerrie for details.
 *
 */
/*
 *	File:	scsi_53C720_hdw.c
 *	Author:	Vineet Kumar & Jerrie Coffman
 *		Intel Corporation Supercomputer Systems Division
 *	Date:	4/94
 *
 * 	Created, from the NCR specs:
 * 	    "NCR 53C720 SCSI I/O Processor Data Manual, Rev 2.1"
 * 	    NCR Microelectronics Division, Colorado Spring, 7/93
 *
 *	Bottom layer of the SCSI driver: chip-dependent functions
 *
 *	This file contains the code that is specific to the NCR 53C720
 *	SCSI chip (Host Bus Adapter in SCSI parlance): probing, start
 *	operation, and interrupt routines.
 */


/* NOTES:
 * 
 */

#include <siop.h>
#if	NSIOP > 0

#include <vm/vm_kern.h>
#include <kern/assert.h>
#include <sys/types.h>
#include <chips/busses.h>

#include <scsi/compat_30.h>
#include <scsi/scsi_defs.h>
#include <scsi/scsi_queue.h>
#include <scsi/scsi.h>
#include <scsi/scsi2.h>

#include <i860paragon/sdb/sdb.h>
#include <i860paragon/sdb/siop_script.h>
#include <scsi/adapters/scsi_53C720.h>
#include <i860paragon/sdb/siop.h>
#include <i860paragon/sdb/if_siop.h>
#include <i860paragon/sdb/script/siop_ent.h>
#include <device/io_req.h>

extern void		outw();
extern void		outb();
extern unsigned short	inw();
extern unsigned char	inb();

/*
 * Definition of the driver for the auto-configuration program
 */
int	siop_probe(), scsi_slave(), scsi_attach(), siop_go(), siop_intr();

static caddr_t			siop_std[NSIOP] = { 0, 0 };
static struct bus_device	*siop_dinfo[NSIOP * MAX_SCSI_TARGETS];
static struct bus_ctlr		*siop_minfo[NSIOP];
struct bus_driver		siop_driver = {
/*
 *	  structure		structure     field
 *	initialization	 	  field	    description
 */
	siop_probe,		/* probe  - is the 53C720 installed ?	 */
	scsi_slave,		/* slave  - really a probe for slaves... */
	scsi_attach,		/* attach - setup driver after the probe */
	siop_go,		/* go	  - start transfer		 */
	siop_std,		/* addr	  - device csr addresses	 */
	"rz",			/* dname  - device driver name		 */
	siop_dinfo,		/* dinfo  - bus_device backpointers	 */
	"siop",			/* mname  - name of controller		 */
	siop_minfo,		/* minfo  - bus_ctlr backpointers	 */
	BUS_INTR_B4_PROBE	/* flags  - flags			 */
};

/*
 * State descriptor for this layer
 * There is one such structure per (enabled) 53C720 interface
 */
static struct siop_softc {
	watchdog_t	wd;		/* watchdog information		     */
	siop_regmap_t	*regs;		/* 53C720 registers		     */
	volatile char	state;		/* flags			     */
	scsi_softc_t	*sc;		/* HBA-independent information	     */
	int		unit;		/* which SIOP			     */
	char		chip_rev; 	/* siop chip revision		     */
	int		clock_mhz;	/* chip clock frequency		     */
	unsigned char	siop_scntl3;	/* async values for SCNTL3 reg 	     */
	unsigned char	siop_sxfer;	/* async values for SXFER reg        */
	unsigned long 	script;		/* SCRIPT load address; virt pointer */
	unsigned long	script_phys;	/* SCRIPT load address; phys pointer */
	queue_head_t	scheduler_q;	/* other commands competing for bus  */
	struct cntrlr_shrd_data	*shrd_locs; /* shared locations in DPRAM 
					       for host and an SIOP	     */
	struct request_table	**rslct_tbl_ptr_clram; /* ptr to reselect table 
						      in CLRAM; one per SIOP */
	struct request_table	*req_tbl_ptr_clram; /* ptr to request table in 
						    clram; one for all SIOPs;
						    virt pointer */
	struct request_table	*req_tbl_ptr_clram_phys; /* ptr to request table
							 in clram; one for all
							 SIOPs; phys pointer */
	unsigned char	cntrlr_q_index; /* index into the cntrlr_q array in
					   cntrlr_shrd_data structure */
	unsigned char	req_done_q_index; /* index into the req_done_q array in
                                             cntrlr_shrd_data structure */
	unsigned char   hw_sem_count;	/* keeps track of whether the host 
					   has the hardware semaphore */
} siop_softc_data[NSIOP];

typedef struct siop_softc *siop_softc_t;
siop_softc_t    siop_softc[NSIOP];

#define SG_ENTRIES_IN_FIRST_PAGE	\
	((PAGE_SIZE-sizeof(struct io_sglist_hdr)) / sizeof(struct io_sg_entry))
#define SG_ENTRIES_PER_PAGE		\
	(PAGE_SIZE / sizeof(struct io_sg_entry))
#define SIOP_SG_ENTRIES_MAX		\
	(SG_ENTRIES_IN_FIRST_PAGE + (SG_ENTRIES_PER_PAGE * (SG_LISTS - 1)))
#define SIOP_TC_MAX			\
	(SIOP_SG_ENTRIES_MAX * PAGE_SIZE)

#define MAX_REQ_TBLS_CLRAM ((SIZEOF_REQ_TBL_AREA)/sizeof(struct request_table))
static struct cntrlr_intf_tag *req_tbl_clram_image[MAX_REQ_TBLS_CLRAM];

struct cntrlr_intf_tag {
	tag_info_t	tag;
	queue_chain_t	sch_links;
};

#define siop_to_scsi_period(a)	((a) * 250 / siop->clock_mhz)
#define scsi_period_to_siop(p)	((p) * siop->clock_mhz / 250)

#define CMD_BUF_SIZE       256

/*
 * if bit x of SLOG_level = 1 trace this message level
 */
static long	SLOG_level = 0			/* for non-debug set to 0 */
		/*	| SLOG_ENTER	/* upon function entry and exit	  */
		/*	| SLOG_LOW	/* branches within a function	  */
		/*	| SLOG_MEDIUM	/* something between low and high */
		/*	| SLOG_HIGH	/* serious condition and problems */
			;

/*
 * if bit x of SLOG_id = 1 trace this routine
 */
static long	SLOG_id = 0			/* for non-debug set to 0 */
		/*	| BIT(0)	/* siop_probe()			  */
		/*	| BIT(1)	/* siop_wait()			  */
		/*	| BIT(2)	/* acquire_hdw_sem()		  */
		/*	| BIT(3)	/* siop_reset()			  */
		/*	| BIT(4)	/* release_hdw_sem()		  */
		/*	| BIT(5)	/* siop_debug_msg()		  */
		/*	| BIT(6)	/* siop_tag_q_init()		  */
		/*	| BIT(7)	/* siop_probe_target()		  */
		/*	| BIT(8)	/* siop_go()			  */
		/*	| BIT(9)	/* siop_intr()			  */
		/*	| BIT(10)	/* siop_scheduler()		  */
		/*	| BIT(11)	/* alloc_req_tbl_clram()	  */
		/*	| BIT(12)	/* dealloc_req_tbl_clram()	  */
		/*	| BIT(13)	/* cntrlr_q_not_empty()		  */
		/*	| BIT(14)	/* siop_req_done()		  */
			| BIT(15)	/* siop_tgt_q_full()		  */
			| BIT(16)	/* siop_tgt_busy()		  */
		/*	| BIT(17)	/* siop_chk_cond()		  */
		/*	| BIT(18)	/* siop_trgt_busy()		  */
		/*	| BIT(19)	/* siop_cleanup_all_tgt_queues()  */
		/*	| BIT(20)	/* siop_one_req_done()		  */
		/*	| BIT(21)	/* siop_all_req_done()		  */
		/*	| BIT(22)	/* siop_other_msg()		  */
		/*	| BIT(23)	/* siop_sel_timeout()		  */
		/*	| BIT(24)	/* siop_phase_mismatch()	  */
		/*	| BIT(25)	/* siop_debugger()		  */
		/*	| BIT(26)	/* siop_was_reset()		  */
		/*	| BIT(27)	/* siop_status()		  */
			| BIT(28)	/* siop_error()			  */
		/*	| BIT(29)	/*				  */
		/*	| BIT(30)	/*				  */
		/*	| BIT(31)	/* generic functions		  */
			;

#define         SLOG_GENERIC_ID      31

#define SCRIPT_ASSERT(dsps)       (((dsps) & 0xffff0000) == 0xc0de0000)
#define SCRIPT_ASSERT_VALUE(dsps)  ((dsps) & 0x0000ffff)
#define SCRIPT_DEAD(dsps)       ((dsps) == 0xdead0000)

				 
/* messages that SIOP will request host to print */
static char *debug_msg_low_tbl[DEBUG_MSG_LOW_LAST - DEBUG_MSG0_LOW + 1] = {
    "DEBUG_MSG0_LOW"                      /*  1 */
};
static char *debug_msg_med_tbl[DEBUG_MSG_MED_LAST - DEBUG_MSG0_MED + 1] = {
    "Init: Entry",			/*  101 */
    "Init: Exit",			/*  102 */
    "Schedular: Entry",			/*  103 */
    "Schedular: Ready to select target", /*  104 */
    "Select With ATN",			/*  105 */
    "Select Without ATN",		/*  106 */
    "Request table copied to local mem", /*  107 */
    "CNTRLR_Q Increment buf Done",		/*  108 */
    "Schedular: Exit",			/*  109 */
    "Message Out: Entry",		/* 110 */
    "Message Out: Exit",		/* 111 */
    "Reselect: Entry",			/* 112 */
    "not used, use it for future debug messages",	/* 113 */
    "Reselect: Exit",			/* 114 */
    "Main: Entry",			/* 115 */
    "CMD Phase Entry",			/* 116 */
    "CMD Phase Exit",			/* 117 */
    "DATA_IN Phase Entry",		/* 118 */
    "DATA_IN Phase Exit",		/* 119 */
    "DATA_OUT Phase Entry",		/* 120 */
    "DATA_OUT Phase Exit",		/* 121 */
    "STATUS Phase Entry",		/* 122 */
    "STATUS Phase Exit",		/* 123 */
    "MSG_IN Phase Entry",		/* 124 */
    "Disconnect Message",		/* 125 */
    "Command Complete Message",		/* 126 */
    "Ignore Wide Residue Message",	/* 127 */
    "Extended Message",			/* 128 */
    "Extended Message Bit Bucket",	/* 129 */
    "Save Data Pointers Message",	/* 130 */
    "Save Data Pointers Exit",		/* 131 */
    "Command Complete Message",		/* 132 */
    "Command Complete Exit",		/* 133 */
    "Copy Entry: Entry",		/* 134 */
    "Copy Entry: Exit, empty",		/* 135 */
    "Copy Entry: Exit, not empty",	/* 136 */
    "Iszero32: Entry",			/* 137 */
    "Iszero32: Exit False",		/* 138 */
    "Iszero32: Exit True",		/* 139 */
    "Sub32: Entry",			/* 140 */
    "Sub32 2's Complement",		/* 141 */
    "Add32: Entry",			/* 142 */
    "Add32: Exit",			/* 143 */
    "Cmp32: Entry",			/* 144 */
    "Cmp32: Exit True",			/* 145 */
    "Cmp32: Exit False",		/* 146 */
    "ID_To_Addr: Entry",		/* 147 */
    "ID_To_Addr: Exit",			/* 148 */
    "Main: Disconnect Exit",		/* 149 */
    "Main: Status not good Exit",	/* 150 */
    "Main: Ignore Wide Exit", 		/* 151 */
    "Other Message: Exit ",		/* 152 */
    "Selection: Entry",			/* 153 */
    "General Purpose Interrupt 1",	/* 154 */
    "General Purpose Interrupt 2",	/* 155 */
    "General Purpose Interrupt 3",      /* 156 */
    "Message Reject"                    /* 157 */
};
static char *debug_msg_high_tbl[DEBUG_MSG_HIGH_LAST - DEBUG_MSG0_HIGH + 1] = {
    "DEBUG_MSG0_HIGH"
};

/* Forward declarations */
#if	MACH_ASSERT
static void     siop_debug_msg(siop_softc_t);
#endif	MACH_ASSERT

static void		siop_phase_mismatch(siop_softc_t, 
				struct cntrlr_shrd_data *);
static boolean_t	siop_debugger(siop_softc_t, int, int, int, int);
static void		siop_was_reset(siop_softc_t);
static void		siop_status(siop_softc_t , unsigned char *);
static void		siop_error(siop_softc_t);
static int		siop_wait(siop_softc_t);
static void		acquire_hdw_sem(siop_softc_t);
static void		release_hdw_sem(siop_softc_t);
boolean_t		siop_probe_target();
int			siop_tag_q_init(target_info_t *, int);
int			siop_reset(siop_softc_t);
static void		siop_scheduler(siop_softc_t);
siop_regmap_t		*siop_reg_ptr(struct bus_ctlr *);
void			scsi_queue_save(target_info_t *, struct tag_info *);
void			scsi_queue_restore(target_info_t *, struct tag_info *);
void			req_q_enter_first(tag_info_t *, target_info_t *);
static struct request_table *alloc_req_tbl_clram(siop_softc_t, int, 
				struct cntrlr_intf_tag *);
static struct cntrlr_intf_tag *dealloc_req_tbl_clram(siop_softc_t,
				volatile struct request_table *);
static struct request_table *cntrlr_q_not_empty(siop_softc_t);
static void		siop_req_done(struct cntrlr_intf_tag *, char);
static void		siop_tgt_q_full(siop_softc_t);
static void		siop_tgt_busy(siop_softc_t);
static void		siop_chk_cond(siop_softc_t);
static void		siop_trgt_busy(siop_softc_t siop,
				struct cntrlr_intf_tag *);
static void		siop_cleanup_all_tgt_queues(struct cntrlr_intf_tag *,
				char);
static void		siop_one_req_done(siop_softc_t);
static void		siop_all_req_done(siop_softc_t);
static void		siop_other_msg(siop_softc_t);
static void		siop_sel_timeout(siop_softc_t);
static void		display_cmd(unsigned char *);
static void             assert_select_tbl_ptr(siop_softc_t);
static void		assert_reselect_tbl_ptr(siop_softc_t);
static int	        print_q_tag_id(siop_softc_t);
static void             assert_data_ptr_saved(siop_softc_t);
static void             assert_gen_purp_1(siop_softc_t);
static void             assert_gen_purp_2(siop_softc_t);
static void             assert_gen_purp_3(siop_softc_t);

void			active_q(int, int);
void			scheduler_q(int);
void                    request_q(int, int);
void			shrd_locs(int);


void (*assert_func[])(siop_softc_t) = {
	/* siop assert functions here */
	assert_reselect_tbl_ptr,
	assert_data_ptr_saved,
	assert_select_tbl_ptr,
	assert_gen_purp_1,
        assert_gen_purp_2,
        assert_gen_purp_3
};


/*
 * Probe routine:
 *	Should find out
 *	    (a) if the controller is present and
 *	    (b) which/where slaves are present.
 *          Return NULL if controller is not present.
 * Implementation:
 *	Send a test-unit-ready to each possible target
 *	on the bus except of course ourselves.
 */
siop_probe(vaddr, ctlr)
	caddr_t		vaddr;
	struct bus_ctlr	*ctlr;
{
    extern target_info_t	scsi_target_data[];
    int             		unit = ctlr->unit;
    siop_softc_t		siop = &siop_softc_data[unit];
    unsigned char		target_id;
    scsi_softc_t		*sc;
    boolean_t       		did_banner = FALSE;
    int 			s;

    SLOG_ENTRY(0, ("siop_probe: enter; ctlr=0x%x, unit=%d\n", ctlr, unit));

    /* Initialize hw descriptor */
    siop_softc[unit] = siop;

    /* initialize siop_softc_t */
    siop->unit = unit;

    if (!(siop->regs = (siop_regmap_t*)siop_reg_ptr(ctlr))) {
	/*
        db_printf(
	"siop_probe: NCR 53C720 SCSI Controller registers unit %d not mapped\n",
	       unit);
	*/
	return 0; /* no expansion board */
    }

    (void)siop_map(ctlr, &siop->shrd_locs, &siop->rslct_tbl_ptr_clram,
		    &siop->req_tbl_ptr_clram);

    siop->req_tbl_ptr_clram_phys =
	(struct request_table *)kvtophys((vm_offset_t)siop->req_tbl_ptr_clram);

    if (!siop->shrd_locs	   ||
	!siop->rslct_tbl_ptr_clram ||
	!siop->req_tbl_ptr_clram   ||
	!siop->req_tbl_ptr_clram_phys) {

	printf(
"siop_probe: NCR 53C720 SCSI Controller data structures at unit %d not mapped\n", unit);
	return 0; /* zero => board is not present */
    }

    siop->wd.reset = siop_reset;

    if (!(sc = scsi_master_alloc(unit, siop))) {
        printf(
"siop_probe: NCR 53C720 SCSI Controller scsi_master_alloc unit %d failed\n",
		unit);
	return 0;
    }

    siop->sc = sc;
    sc->initiator_id  = (scsi_initiator_id[unit] & 0xf);
    sc->supports_sgio = TRUE;
    sc->tag_q_init    = siop_tag_q_init;
    sc->max_dma_data  = SIOP_TC_MAX;
    sc->go	      = siop_go;
    sc->watchdog      = scsi_watchdog;
    sc->probe	      = siop_probe_target;

    /* start the SIOP and wait for response */
    s = splbio(); /* mask interrupts from expansion board, and return previous
		     mask */

    (void)siop_reset(siop); /* SIOP is also started by this routine */

    /* SIOP is started by siop_reset() */
    if (!siop_wait(siop)) {
	printf("siop_probe: NCR 53C720 SCSI Controller unit %d is not alive\n",
		unit);
        splx(s);
	return 0;
    }
    /* NOTE that siop_wait() removes sources of int from SIOP */

    if (siop->shrd_locs->stat.byte.halt_reason != SIOP_RESET) {
        printf("siop_probe: NCR 53C720 SCSI Controller unit %d did not reset\n",
		unit);
        splx(s);
	return 0;
    } 
    siop->shrd_locs->stat.byte.halt_reason = 0;
    siop->regs->siop_dsp = siop->regs->siop_dsp; /* Restart the siop SCRIPT */

    if (!siop_wait(siop)) {
        printf("siop_probe: NCR 53C720 SCSI Controller unit %d is dead\n",
		unit);
        splx(s);
	return 0;
    }

#if	MACH_ASSERT
    while (siop->shrd_locs->stat.byte.halt_reason == DEBUG_MSG) {
	siop->shrd_locs->stat.byte.halt_reason = 0;
	siop_debug_msg(siop); /* print debug msg and start SIOP */
	if (!siop_wait(siop)) {
	    printf(
"siop_probe: NCR 53C720 SCSI Controller unit %d works improperly after debug msg\n", unit);
            splx(s);
	    return 0;
	}
    }
#endif	MACH_ASSERT

    if (!(siop->shrd_locs->stat.byte.no_work_to_do)) {
        printf(
"siop_probe: NCR 53C720 SCSI Controller at unit %d does not work properly",
		unit);
        splx(s);
	return 0;
    }

    /* check integrity of SIOP regs and shared locs, return if unexpected
       values found; these are not debug messages */

    if(siop->shrd_locs->stat.byte.halt_reason ||
       siop->shrd_locs->stat.byte.status_buf ||
       siop->shrd_locs->stat.byte.msg_in_count ||
       (*(struct cntrlr_buf**)SCRIPT_CNTRLR_Q_P(siop->script) != 
	(struct cntrlr_buf*)kvtophys((vm_offset_t)siop->shrd_locs->cntrlr_q))
       || (*(struct request_table***)SCRIPT_REQ_DONE_PTR(siop->script) !=
        (struct request_table**)kvtophys((vm_offset_t)siop->shrd_locs->
					req_done_q))) {
	printf(
"siop_probe: NCR 53C720 SCSI Controller at unit %d data check failed\n", unit);
        splx(s);
        return 0;
    }

    /* end of integrity of SIOP regs and shared locs */

    /* Print the driver signon message */
    printf("%s%d: %sNCR 53C720 SCSI Controller at id %d", ctlr->name, unit,
	(siop_diff(siop->unit)) ? "Differential " : "", sc->initiator_id);

    /*
     * For all possible targets, see if one is there and
     * allocate a descriptor for it if it is found.
     */
    for (target_id = 0; target_id < MAX_SCSI_TARGETS; target_id++) {

	/* except of course ourselves */
	if (target_id == sc->initiator_id)
	    continue;

	/* check error conditions */
	assert(!(siop->shrd_locs->cntrlr_q[siop->cntrlr_q_index].req_tbl.
		 src_ptr));
	assert (!(siop->shrd_locs->req_done_q[siop->req_done_q_index]));

	/* fill the request table */
        siop->shrd_locs->req_tbl[siop->cntrlr_q_index].bus_conf.sxfer =
            siop->siop_sxfer;
        siop->shrd_locs->req_tbl[siop->cntrlr_q_index].bus_conf.dest_id = 
	    target_id;
	siop->shrd_locs->req_tbl[siop->cntrlr_q_index].bus_conf.scntl3 = 
	    siop->siop_scntl3;
	siop->shrd_locs->req_tbl[siop->cntrlr_q_index].msg_out.len = 0;
	siop->shrd_locs->req_tbl[siop->cntrlr_q_index].msg_out.ptr = 
	    (unsigned char *)(siop->req_tbl_ptr_clram_phys) + 
		OFST_MSG_OUT_IN_REQ_TBL; /* needed by the script test module 
					    to check data integrity */
	{
	    scsi_cmd_test_unit_ready_t      *cmd;
	    siop->shrd_locs->req_tbl[siop->cntrlr_q_index].cmd.len = 
		sizeof(*cmd);
            siop->shrd_locs->req_tbl[siop->cntrlr_q_index].cmd.ptr =
        	(unsigned char *)siop->req_tbl_ptr_clram_phys +
		OFST_CMD_IN_REQ_TBL;
	    cmd = (scsi_cmd_test_unit_ready_t *)(siop->shrd_locs->
				  req_tbl[siop->cntrlr_q_index].cmd.bytes);
	    cmd->scsi_cmd_code = SCSI_CMD_TEST_UNIT_READY;
	    cmd->scsi_cmd_lun_and_lba1 = 0;
	    cmd->scsi_cmd_lba2 = 0;
	    cmd->scsi_cmd_lba3 = 0;
	    cmd->scsi_cmd_ss_flags = 0;
	    cmd->scsi_cmd_ctrl_byte = 0;    /* not linked */
	}

	/* fill the cntrlr_buf element of cntrlr_q */
	siop->shrd_locs->cntrlr_q[siop->cntrlr_q_index].info.q_tag = 0;
        siop->shrd_locs->cntrlr_q[siop->cntrlr_q_index].info.target_id = 
	    target_id;

	siop->shrd_locs->cntrlr_q[siop->cntrlr_q_index].req_tbl.src_ptr = 
	    (struct request_table *)kvtophys((vm_offset_t)(&(siop->shrd_locs->
				     req_tbl[siop->cntrlr_q_index])));
        assert(!((unsigned long)(&(siop->shrd_locs->req_tbl
                           [siop->cntrlr_q_index])) & NOT_ALIGNED_16_BYTE));
	siop->shrd_locs->cntrlr_q[siop->cntrlr_q_index].req_tbl.dest_ptr =
					     siop->req_tbl_ptr_clram_phys; 
	assert(!((unsigned long)siop->req_tbl_ptr_clram_phys & 
		  NOT_ALIGNED_16_BYTE));

	siop->shrd_locs->cntrlr_q[siop->cntrlr_q_index].req_tbl.cnt = 
	    OFST_SG_ENTS_PTR_IN_REQ_TBL;
	siop->shrd_locs->cntrlr_q[siop->cntrlr_q_index].sg_list[0].src_ptr = 0;

	/* start the SIOP */
        if (siop->shrd_locs->stat.byte.no_work_to_do) {
            assert(!siop->shrd_locs->stat.byte.halt_reason &&
                   !siop->shrd_locs->stat.byte.msg_in_count);
	    siop->shrd_locs->stat.word = (unsigned long)0;
	    /* no reason to acquire hdw sem because SIOP cannot get 
	       reselected; no need to mask other bits in ISTAT reg */
	    siop->regs->siop_istat = SIOP_ISTAT_SIGP; /* signal SIOP */
	}
	else if (siop->shrd_locs->stat.byte.halt_reason) {
	    /* halt_reason can happen only if status is not-good */
            assert(!siop->shrd_locs->stat.byte.no_work_to_do &&
                   !siop->shrd_locs->stat.byte.msg_in_count);
	    siop->shrd_locs->stat.word = (unsigned long)0;
	    /* start SIOP */
	    siop->regs->siop_dsp = SCRIPT_SCHEDULER(siop->script_phys);
	}

	if (!siop_wait(siop)) {
	    printf(
"siop_probe: NCR 53C720 SCSI Controller unit %d failed when probing target %d\n", unit, target_id);
            splx(s);
            return 0;
        }

#if	MACH_ASSERT
	while (siop->shrd_locs->stat.byte.halt_reason == DEBUG_MSG) {
            siop->shrd_locs->stat.byte.halt_reason = 0;
	    siop_debug_msg(siop); /* print debug msg and start SIOP */
	    if (!siop_wait(siop)) {
		printf("siop_probe: NCR 53C720 SCSI Controller unit %d failed when probing target %d and debug msg is received\n", unit, target_id);
                splx(s);
		return 0;
	    }
	}
#endif	MACH_ASSERT

	if (siop->shrd_locs->stat.byte.no_work_to_do) {
            /* 2 possibilities: (1) status_buf = 0 => target responded and
               command is complete, (2) status_buf != 0 => target responded
               with status and then disconnected, target did not reselect later
	       */
	    assert(!(siop->shrd_locs->
		     cntrlr_q[siop->cntrlr_q_index].req_tbl.src_ptr));
	    assert(siop->shrd_locs->cntrlr_q[siop->cntrlr_q_index].
                       req_tbl.dest_ptr == siop->shrd_locs->req_tbl_clram_ptr);

	    if (siop->shrd_locs->stat.byte.status_buf) {
                if (++siop->cntrlr_q_index == CNTRLR_Q_SIZE)
                    siop->cntrlr_q_index = 0;
                continue; /* no target exists at the given target ID */
	    } else {
		assert(siop->shrd_locs->req_done_q[siop->req_done_q_index] ==
		       siop->shrd_locs->cntrlr_q[siop->cntrlr_q_index].
		       req_tbl.dest_ptr);

		siop->shrd_locs->req_done_q[siop->req_done_q_index] = 0;
		if (++siop->cntrlr_q_index == CNTRLR_Q_SIZE)
		    siop->cntrlr_q_index = 0;
		if (++siop->req_done_q_index == REQ_DONE_Q_SIZE)
		    siop->req_done_q_index = 0;
	    }
        } 
	else if (siop->shrd_locs->stat.byte.halt_reason) {
	    if (siop->shrd_locs->stat.byte.halt_reason == SELECT_TIMEOUT) {
		assert(siop->shrd_locs->
                     cntrlr_q[siop->cntrlr_q_index].req_tbl.src_ptr);
		/* zero-out src_ptr so it doesn't assert when checking */
		siop->shrd_locs->cntrlr_q[siop->cntrlr_q_index].req_tbl.
		    src_ptr = 0;
		/* do NOT inc cntrlr_q_index and req_done_q_index */
		continue; /* no target exists at the given target ID */
	    }
	    else if (siop->shrd_locs->stat.byte.halt_reason == STAT_NOT_GOOD) {
		/*
		 * We expect not good status following a SCSI bus reset
		 *
                 * db_printf("siop_probe: WARNING: SIOP%d, ", siop->unit);
		 * db_printf("target id = %d sent a not good ", target_id);
                 * db_printf("status = %x\n",
		 *	siop->shrd_locs->stat.byte.status_buf);
		 */
		assert(!(siop->shrd_locs->cntrlr_q[siop->cntrlr_q_index].
			 req_tbl.src_ptr));
		scsi_error(0, SCSI_ERR_STATUS,
			siop->shrd_locs->stat.byte.status_buf, 0);
		if (++siop->cntrlr_q_index == CNTRLR_Q_SIZE)
		    siop->cntrlr_q_index = 0;
	    }
	    else { /* bad halt_reason */
		panic("siop_probe: bad halt_reason from SIOP%d\n", unit);
	    }
	}
	else { /* should'nt happen */
	    panic("siop_probe: unexpected SIOP%d value\n", unit);
	}

	/* found a target */
	{
	    target_info_t  *tgt;
            int            lun;

	    printf("%s%d", did_banner++ ? ", " : " target(s) at ", target_id);
	    (void)scsi_slave_alloc(sc->masterno, target_id, siop);

            for (lun = 0; lun < MAX_LUNS; lun++) {
                tgt = sc->target[target_id][lun];

		tgt->max_dma_data = sc->max_dma_data;
	        tgt->sync_period = siop->siop_scntl3;
	        tgt->sync_offset = siop->siop_sxfer;
	        if (!siop_tag_q_init(tgt, 1)) {
		    /* cannot alloc space for cntrlr_intf_tag */
                    printf("siop_probe: Memory allocation for siop%d failed\n",
                        siop->unit);
                    splx(s);
                    return 0;
	        }
            }
	}
    } /* for loop */

    printf("\n");

    if (!siop->shrd_locs->stat.byte.no_work_to_do) {
        siop->shrd_locs->stat.word = (unsigned long)0;
	siop->regs->siop_dsp = SCRIPT_SCHEDULER(siop->script_phys);
	if (!siop_wait(siop)) {
	    printf("siop_probe: NCR 53C720 SCSI Controller unit %d is dead after all targets probed\n", unit);
            splx(s);
	    return 0;
	}
#if	MACH_ASSERT
	while (siop->shrd_locs->stat.byte.halt_reason == DEBUG_MSG) {
	    siop->shrd_locs->stat.byte.halt_reason = 0;
	    siop_debug_msg(siop); /* print debug msg and start SIOP */
	    if (!siop_wait(siop)) {
		printf("siop_probe: NCR 53C720 SCSI Controller unit %d works improperly after debug msg and targets probed\n", unit);
                splx(s);
		return 0;
	    }
	}
#endif	MACH_ASSERT

	if (!(siop->shrd_locs->stat.byte.no_work_to_do)) {
	    printf("siop_probe: NCR 53C720 SCSI Controller at unit %d does not work properly after all targets probed\n", unit);
	    assert(0);
            splx(s);
	    return 0;
	}
    }

    /* NOTE: do not clear no_work_to_do so that other functions know that 
       the SIOP is in wait_reselect mode */

    /* there should be no intr from SIOP, all bits of ISTAT should be clear */

    /* check integrity of SIOP regs and shared locs */

    /* as a safety precaution, clear DMA interrupt bit */
    siop_clear_dma_int(siop->unit);	/* clear sticky bit */

    assert(!siop->regs->siop_istat); /* ISTAT reg should have zero */
    /* integrity of SIOP regs and shared locs checked */
    assert(!(siop->shrd_locs->stat.byte.halt_reason ||
             siop->shrd_locs->stat.byte.status_buf  ||
             siop->shrd_locs->stat.byte.msg_in_count));

    splx(s); /* restore previous mask before splbio() sets new mask */
    SLOG_EXIT(("siop_probe: exit, return=1\n"));

    return 1;
}


boolean_t
siop_probe_target(tgt, ior)
	target_info_t	*tgt;
	io_req_t	ior;
{
    SLOG_ENTRY(7, ("siop_probe_target: enter, masterno=%d\n", tgt->masterno));

    /* after plugging and un-plugging the target without resetting the scsi
       bus, the target could have synchronous transfer rate different from the
       initiator or the new hot plugged-in target could do wide but the 
       previous plugged-out target could not; therefore sync and wide 
       negotiations must be done to synchronize these values */

    if (!siop_tag_q_init(tgt, 1)) {
	/* cannot alloc space for cntrlr_intf_tag */
	printf("siop_probe_target:Memory allocation for siop%d failed\n", tgt->masterno);
	return FALSE;
    }

    /* default values for sync and wide transfers */
    tgt->max_dma_data = scsi_softc[tgt->masterno]->max_dma_data;
    tgt->sync_period = siop_softc[tgt->masterno]->siop_scntl3;
    tgt->sync_offset = siop_softc[tgt->masterno]->siop_sxfer;
    /* force sync and wide transfers to happen */
    BCLR(scsi_no_wide_xfer,tgt->masterno,tgt->target_id);
    BCLR(scsi_no_synchronous_xfer,tgt->masterno,tgt->target_id);

    if (scsi_test_unit_ready(tgt, 0) == SCSI_RET_DEVICE_DOWN) { /* wide nego */
	siop_tag_q_init(tgt, 0);
	SLOG_EXIT(("siop_probe_target: device down, exit, return=FALSE\n"));
	return FALSE;
    } 
    if (scsi_test_unit_ready(tgt, 0) == SCSI_RET_DEVICE_DOWN) { /* sync nego */
        siop_tag_q_init(tgt, 0);
        SLOG_EXIT(("siop_probe_target: device down, exit, return=FALSE\n"));
        return FALSE;
    }

    tgt->flags = TGT_ALIVE;

    {
        siop_softc_t	siop = (siop_softc_t)tgt->hw_state;
        scsi_softc_t	*sc = siop->sc;
        unsigned char	target_id = tgt->target_id;
        int		probed_lun = tgt->lun;
        int		lun;

	/*
         * Initialize all other LUN structures for this target
         */
        for (lun = 0; lun < MAX_LUNS; lun++) {
            tgt = sc->target[target_id][lun];

	    /* except of course the probed lun */
	    if (lun == probed_lun)
	        continue;

            tgt->max_dma_data = sc->max_dma_data;
            tgt->sync_period = siop->siop_scntl3;
	    tgt->sync_offset = siop->siop_sxfer;
            /* force sync and wide transfers to happen */
            BCLR(scsi_no_wide_xfer,tgt->masterno,target_id);
            BCLR(scsi_no_synchronous_xfer,tgt->masterno,target_id);

	    if (!siop_tag_q_init(tgt, 1)) {
	       /* cannot alloc space for cntrlr_intf_tag */
	       printf("siop_probe_target:Memory allocation for siop%d failed\n",
                    tgt->masterno);
		/*
		 * Clean up what was allocated
		 */
                for (lun = 0; lun < MAX_LUNS; lun++) {
                    tgt = sc->target[target_id][lun];

                    siop_tag_q_init(tgt, 0);
                    tgt->flags = 0;
                }
    		SLOG_EXIT(("siop_probe_target: exit, return=FALSE\n"));
                return FALSE;
	    }
            tgt->flags = TGT_ALIVE;
        }
    }

    SLOG_EXIT(("siop_probe_target: device up, exit, return=TRUE\n"));
    return TRUE;
}

/*
 * siop_go is reentrant 
 */
siop_go(tgt, cmd_count, in_count, cmd_only)
	target_info_t	*tgt;
	int		cmd_count;
	int		in_count;
	boolean_t	cmd_only;
{
    struct cntrlr_intf_tag 	*cntrlr_tag_p;
    siop_softc_t        	siop = siop_softc[tgt->masterno];

    /* interrupt routines manipulate the same queues that are manipulated
       here; therefore, mask interrupts from expansion board */
    int                         exp_intr = splbio();

    /*
     * This SLOG_ENTRY should occur after the splbio() call above.
     * Otherwise nested debug printf's may occur from routines
     * called by the interrupt handler resulting in an slock
     * expiration on the printf_lock.
     */
    SLOG_ENTRY(8, ("siop_go: enter, tgt=0x%x, cmd_count=%d, in_count=%d, cmd_only=%d, masterno=%d, cur_cmd=0x%x\n", tgt, cmd_count, in_count, cmd_only, tgt->masterno, tgt->cur_cmd));

    tgt->done = SCSI_RET_IN_PROGRESS;

    /* if queueing is disabled then this routine should be called when
       the current cmd is done which implies that free_tag_q is not empty */
    assert (!(!is_scsi_queue_enabled(tgt) && queue_empty(&tgt->free_tag_q)));
    /* if queueing is disabled then the target's queue should never be full */
    assert (!(!is_scsi_queue_enabled(tgt) && is_tgt_q_full(tgt)));

    if (queue_empty(&tgt->free_tag_q) || is_tgt_q_full(tgt)) {
	SLOG(SLOG_MEDIUM, ("    siop_go: free_tag_q is empty\n"));
#ifdef	SCSI_STATISTICS
	tgt->statistics.driver_full++;
#endif	SCSI_STATISTICS
	tgt->done = SCSI_RET_RETRY;
	SLOG_EXIT(("siop_go: exit, resources not available\n"));
	splx(exp_intr);
	return 1; /* return value is not important */
	/* the upper layer will not call siop_go() until an active request
	   for this target completes */
    }

    queue_remove_first(&tgt->free_tag_q, cntrlr_tag_p, 
		      struct cntrlr_intf_tag *, tag.links);

    /* if scsi_queue_disabled then this cntrlr_tag will be the only one
       scheduled even though it is at the tail of the active_tag_q; other
       cntrlr_tags on active_tag_q are not scheduled */
    queue_enter(&tgt->active_tag_q, cntrlr_tag_p, struct cntrlr_intf_tag *,
                      tag.links);

    /* if scsi_queue_disabled then scsi_queue_save() does not dequeue this
       request from the request_q and siop_go() is not called again until
       this request completes */
    scsi_queue_save(tgt, &cntrlr_tag_p->tag);
    cntrlr_tag_p->tag.cmd_count = cmd_count;
    cntrlr_tag_p->tag.cmd_only = cmd_only;
    queue_enter(&siop->scheduler_q, cntrlr_tag_p, struct cntrlr_intf_tag *,
                      sch_links);
    if (siop->wd.nactive++== 0)
	siop->wd.watchdog_state = SCSI_WD_ACTIVE;

    siop_scheduler(siop);

    /*
     * This SLOG_EXIT should occur before the splx() call below.
     * Otherwise nested debug printf's may occur from routines
     * called by the interrupt handler resulting in an slock
     * expiration on the printf_lock.
     */
    SLOG_EXIT(("siop_go: exit\n"));

    splx(exp_intr);
    return 1; /* return value is not important */
}

static void 
siop_scheduler(siop_softc_t siop)
{
    int num_req_tbls_clram;
    struct request_table *dest_req_tbl_p; /* phys ptr to req tbl in clram */
    struct request_table *src_req_tbl_p;
    int entries;
    struct cntrlr_intf_tag *cntrlr_tag_p;
    target_info_t *tgt;
    boolean_t disconn;
    struct io_req *ior_ptr;
    extern unsigned short scsi_might_disconnect[];
    extern unsigned short scsi_should_disconnect[];

    SLOG_ENTRY(10, ("siop_scheduler: enter, unit=%x\n", siop->unit));

    while ((!queue_empty(&siop->scheduler_q)) && (!cntrlr_q_full(siop))) {

	/* find num of req tbls needed in clram */
	ior_ptr = ((struct cntrlr_intf_tag*)
	    queue_first(&siop->scheduler_q))->tag.ior;
	if (ior_ptr && ior_ptr->io_sgp) {
	    assert(!((unsigned long)ior_ptr->io_sgp & (PAGE_SIZE - 1)));
            entries = ior_ptr->io_sgp->iosg_hdr.nentries;
	    if (entries > SIOP_SG_ENTRIES_MAX) {
		entries = SIOP_SG_ENTRIES_MAX;	/* can only do this much */
	    }
	}
	else {
	    entries = 0;
	}
	num_req_tbls_clram = 1; /* atleast one req_tbl in clram for each req */
	if (entries > SG_ENTS) {
	    entries = (entries - SG_ENTS) * sizeof(struct io_sg_entry);
	    num_req_tbls_clram += (entries / sizeof(struct request_table));
	    if (entries % sizeof(struct request_table)) {
		num_req_tbls_clram++;
	    }
	}
        SLOG(SLOG_MEDIUM, (
		   "    siop_scheduler: num of req tbls in clram = %d\n",
                            num_req_tbls_clram));

	/* allocate space in req_tbl_ptr_clram_phys[] */
	if (!(dest_req_tbl_p = (struct request_table *)
	      (alloc_req_tbl_clram(siop, num_req_tbls_clram,
	   (struct cntrlr_intf_tag *)queue_first(&siop->scheduler_q))))) {
	    SLOG_EXIT(("siop_scheduler: req tbl clram can't alloc; exit\n"));
	    return;
	}
	
	/* all conditions have been met; now we can schedule a request */
	queue_remove_first(&siop->scheduler_q, cntrlr_tag_p, 
			   struct cntrlr_intf_tag *, sch_links);
	cntrlr_tag_p->sch_links.next = NULL;
	tgt = cntrlr_tag_p->tag.tgt;
        assert(!is_tgt_q_full(tgt));

        /* fill the request table */
	src_req_tbl_p = (struct request_table *)
	    &siop->shrd_locs->req_tbl[siop->cntrlr_q_index];
        src_req_tbl_p->bus_conf.sxfer = tgt->sync_offset;
        src_req_tbl_p->bus_conf.dest_id = tgt->target_id;
        src_req_tbl_p->bus_conf.scntl3 = tgt->sync_period;
        src_req_tbl_p->msg_out.len = 0;
	src_req_tbl_p->msg_out.ptr = (unsigned char*)dest_req_tbl_p
	    + OFST_MSG_OUT_IN_REQ_TBL;
	if (!cntrlr_tag_p->tag.cmd_only) {
	    disconn  = BGET(scsi_might_disconnect,tgt->masterno,
			    tgt->target_id);
	    disconn |= BGET(scsi_should_disconnect,tgt->masterno,
			    tgt->target_id);
	    src_req_tbl_p->msg_out.bytes[src_req_tbl_p->msg_out.len++] =
                (disconn ? SCSI_IDENTIFY | SCSI_IFY_ENABLE_DISCONNECT 
		 | (cntrlr_tag_p->tag.lun & SCSI_IFY_LUN_MASK) :
                SCSI_IDENTIFY) | (cntrlr_tag_p->tag.lun & SCSI_IFY_LUN_MASK);
            if (is_tag_queuing_enabled(tgt) && is_scsi_queue_enabled(tgt)) {
		/* scsi spec requires the disconnect privilege bit in Identify
		   to be enabled if queue tag msg is also sent */
		src_req_tbl_p->msg_out.bytes[src_req_tbl_p->msg_out.len - 1] |=
		    SCSI_IFY_ENABLE_DISCONNECT;
                src_req_tbl_p->msg_out.bytes[src_req_tbl_p->msg_out.len++] = 
		    SCSI_SIMPLE_QUEUE_TAG;
                src_req_tbl_p->msg_out.bytes[src_req_tbl_p->msg_out.len++] = 
		    cntrlr_tag_p->tag.tag;
            }
	}

        {
	    /* copy cmd from cmd_ptr to the command field in request_table */
            int i;
            SLOG(SLOG_MEDIUM, ("    siop_scheduler: cur_cmd = %x\n",
                            cntrlr_tag_p->tag.cur_cmd));
            src_req_tbl_p->cmd.len = cntrlr_tag_p->tag.cmd_count;
            src_req_tbl_p->cmd.ptr = (unsigned char *)dest_req_tbl_p
                + OFST_CMD_IN_REQ_TBL;
	    if (cntrlr_tag_p->tag.cmd_count <= MAX_CMD_LGT) {
		for (i = 0; i < cntrlr_tag_p->tag.cmd_count; i++) {
		    src_req_tbl_p->cmd.bytes[i] =
			(unsigned char)cntrlr_tag_p->tag.cmd_ptr[i];
		}
	    } else {
		assert((cntrlr_tag_p->tag.cur_cmd == SCSI_CMD_MODE_SELECT) ||
		       (cntrlr_tag_p->tag.cur_cmd == SCSI_CMD_REASSIGN_BLOCKS)
		       || (cntrlr_tag_p->tag.cur_cmd == SCSI_CMD_FORMAT_UNIT)
		       || (cntrlr_tag_p->tag.cur_cmd == 
			   SCSI_CMD_LOAD_DISPLAY));
                for (i = 0; i < sizeof(scsi_command_group_0); i++) {
                    src_req_tbl_p->cmd.bytes[i] =
                        (unsigned char)cntrlr_tag_p->tag.cmd_ptr[i];
                }
	    }
        }

        switch (cntrlr_tag_p->tag.cur_cmd) {
	  case SCSI_CMD_READ:
	  case SCSI_CMD_LONG_READ:
	  case SCSI_CMD_WRITE:
	  case SCSI_CMD_LONG_WRITE:
	    break;
          case SCSI_CMD_INQUIRY:
	    if (cntrlr_tag_p->tag.cmd_only)
		break;
	    /* else fall through */
          case SCSI_CMD_TEST_UNIT_READY:
	    /* Do synch and wide negotiation here, unless prohibited
	       or done already */
	    /* don't care about the value of cntrlr_tag_p->tag.cmd_only */
	    /* NOTE: SCSI spec requires wide nogotiation before synch */
            if (cntrlr_tag_p->tag.cmd_only && (
  	       !(cntrlr_tag_p->tag.flags & TGT_DID_WIDE) || 
		!(cntrlr_tag_p->tag.flags & TGT_DID_SYNCH))) {
                disconn  = BGET(scsi_might_disconnect,tgt->masterno,
                                tgt->target_id);
                disconn |= BGET(scsi_should_disconnect,tgt->masterno,
                                         tgt->target_id);
                src_req_tbl_p->msg_out.bytes[src_req_tbl_p->msg_out.len++] =
                    (disconn ? SCSI_IDENTIFY | SCSI_IFY_ENABLE_DISCONNECT
                     | (cntrlr_tag_p->tag.lun & SCSI_IFY_LUN_MASK) :
                     SCSI_IDENTIFY) | (cntrlr_tag_p->tag.lun &
                                       SCSI_IFY_LUN_MASK);
	    }
	    if (!(cntrlr_tag_p->tag.flags & TGT_DID_WIDE)) {
                SLOG(SLOG_MEDIUM, (
			   "    siop_scheduler: do wide negotiation\n"));
                {
                    scsi_wide_xfer_t *wide_msg;
                    wide_msg = (scsi_wide_xfer_t *)
                        (&src_req_tbl_p->msg_out.bytes[src_req_tbl_p->
                                                       msg_out.len]);
                    wide_msg->xtn_msg_tag = SCSI_EXTENDED_MESSAGE;
                    wide_msg->xtn_msg_len = 0x02;
                    wide_msg->xtn_msg_code = SCSI_WIDE_XFER_REQUEST;
                    wide_msg->xtn_msg_xfer_width = 0x01;

		    src_req_tbl_p->msg_out.len += sizeof(*wide_msg);
                }
                tgt->flags |= TGT_TRY_WIDE;
	    }
            else if (!(cntrlr_tag_p->tag.flags & TGT_DID_SYNCH)) {
                SLOG(SLOG_MEDIUM, (
			   "    siop_scheduler: do synch negotiation\n"));
                {
		    unsigned char	xfer_period;
                    scsi_synch_xfer_req_t *synch_msg;

		    xfer_period = ((siop->siop_sxfer & SIOP_SXFER_TP_MASK) >>
				   SIOP_TP_SHIFT) + SIOP_MIN_XFERP;

                    synch_msg = (scsi_synch_xfer_req_t *)
                (&src_req_tbl_p->msg_out.bytes[src_req_tbl_p->msg_out.len]);

                    synch_msg->xtn_msg_tag = SCSI_EXTENDED_MESSAGE;
                    synch_msg->xtn_msg_len = 0x03;
                    synch_msg->xtn_msg_code = SCSI_SYNC_XFER_REQUEST;
                    synch_msg->xtn_msg_xfer_period =
			siop_to_scsi_period(xfer_period);
                    synch_msg->xtn_msg_xfer_offset = SIOP_SYNCH_OFFSET;

                    src_req_tbl_p->msg_out.len += sizeof(*synch_msg);
                }
		tgt->flags |= TGT_TRY_SYNCH;
            }
            break;
	  case SCSI_CMD_MODE_SELECT:
	  case SCSI_CMD_REASSIGN_BLOCKS:
	  case SCSI_CMD_FORMAT_UNIT:
	  case SCSI_CMD_LOAD_DISPLAY:
	    /* adjust len so that only the command part of *cmd_ptr is in
	       the cmd field; later in code the data part of *cmd_ptr will
	       be made part of sg_list */
	    src_req_tbl_p->cmd.len = sizeof(scsi_command_group_0);
            break;
	  default:
	    break;
	}
	assert(src_req_tbl_p->msg_out.len <= MAX_MSG_OUT_LGT);

	src_req_tbl_p->sg_ent_ptr = (unsigned char *)dest_req_tbl_p
                + OFST_SG_ENTS_IN_REQ_TBL;

        /* complete filling the sg_ent[] field in request_table and also
	   start filling the cntrlr_buf field of cntrlr_q */
	{
            unsigned char *sg_p;
            int i;
	    struct cntrlr_buf *cntrlr_buf_p;

	    cntrlr_buf_p = (struct cntrlr_buf *)
		&siop->shrd_locs->cntrlr_q[siop->cntrlr_q_index];

            if (!cntrlr_tag_p->tag.cmd_only && is_tag_queuing_enabled(tgt) &&
		is_scsi_queue_enabled(tgt)) 
		cntrlr_buf_p->info.q_tag = cntrlr_tag_p->tag.tag;
	    else
		cntrlr_buf_p->info.q_tag = 0;
	    /* assume high nibble of tgt->target_id is zero */
	    cntrlr_buf_p->info.target_id = (unsigned char)tgt->target_id;
	    /* NOTE THAT cntrlr_buf_p->req_tbl.src_ptr should be written last
	       because SIOP uses it to decide whether to move the request
	       table from DPRAM to CLRAM */
	    cntrlr_buf_p->req_tbl.dest_ptr = dest_req_tbl_p;
	    cntrlr_buf_p->req_tbl.cnt = sizeof(struct request_table);
	    switch (tgt->cur_cmd) {
	      case SCSI_CMD_READ:
	      case SCSI_CMD_LONG_READ:
	      case SCSI_CMD_WRITE:
	      case SCSI_CMD_LONG_WRITE:
		assert(ior_ptr && ior_ptr->io_sgp);
		/* check if sg ptr is aligned on a page boundary */
		assert(!((unsigned long)ior_ptr->io_sgp & (PAGE_SIZE - 1)));
		sg_p = (unsigned char *)ior_ptr->io_sgp +
		    sizeof(struct io_sglist_hdr);
		for (i = 0; i < SG_ENTS; i++) {
		    src_req_tbl_p->sg_ent[i].len = (unsigned long)
			((struct io_sg_entry *)sg_p)->iosge_length;
		    src_req_tbl_p->sg_ent[i].ptr = (unsigned char *)
			((struct io_sg_entry *)sg_p)->iosge_phys;
		    sg_p += sizeof(struct io_sg_entry);
		}
		entries = ior_ptr->io_sgp->iosg_hdr.nentries;
		if (entries > SIOP_SG_ENTRIES_MAX) {
		    entries = SIOP_SG_ENTRIES_MAX; /* can only do this much */
		}
		break;
                /* during initialization ior is not allocated, therefore the
                   data coming/going in/out from/to the target is written
                   in the command buffer */
	      case SCSI_CMD_INQUIRY:
	      case SCSI_CMD_REQUEST_SENSE:
	      case SCSI_CMD_MODE_SENSE:
	      case SCSI_CMD_LOG_SENSE:
	      case SCSI_CMD_RECEIVE_DIAG_RESULTS:
	      case SCSI_CMD_READ_CAPACITY:
	      case SCSI_CMD_READ_BLOCK_LIMITS:
	      case SCSI_CMD_READ_POSITION:
	      case SCSI_CMD_READ_ELEMENT_STATUS:
		/* note that SIOP does not have alignment restrictions
		   in its "move from ..." instruction */
		src_req_tbl_p->sg_ent[0].len = CMD_BUF_SIZE;
		src_req_tbl_p->sg_ent[0].ptr = (unsigned char * )
		    kvtophys((vm_offset_t)cntrlr_tag_p->tag.cmd_ptr);
		entries = 0;
		break;
	      case SCSI_CMD_MODE_SELECT:
	      case SCSI_CMD_REASSIGN_BLOCKS:
	      case SCSI_CMD_FORMAT_UNIT:
	      case SCSI_CMD_LOAD_DISPLAY:
		/* note that SIOP does not have alignment restrictions
		   in its "move from ..." instruction */
		src_req_tbl_p->sg_ent[0].len = cntrlr_tag_p->tag.cmd_count
		    - sizeof(scsi_command_group_0);
		src_req_tbl_p->sg_ent[0].ptr = (unsigned char * )
		    kvtophys((vm_offset_t)(cntrlr_tag_p->tag.cmd_ptr +
					   sizeof(scsi_command_group_0)));
		entries = 0;	
		break;
	      case SCSI_CMD_TEST_UNIT_READY:
		break;
	      default:
		/* it is a simple command that does'nt do data in/out */
		SLOG(SLOG_HIGH, ("    siop_scheduler: ior or sg_list ptr is zero and cur_cmd=0x%x has no data in/out\n", cntrlr_tag_p->tag.cur_cmd));
		break;
	    } /* switch (tgt->cur_cmd) */
	    /* check if sg-list fits in the first request_table */
	    if (entries > SG_ENTS) {
		entries = (entries - SG_ENTS) * sizeof(struct io_sg_entry);
		cntrlr_buf_p->sg_list[0].src_ptr = (unsigned char *)
		    kvtophys((vm_offset_t)sg_p);
                assert(!((unsigned long)cntrlr_buf_p->sg_list[0].src_ptr & 
			 NOT_ALIGNED_16_BYTE));
		cntrlr_buf_p->sg_list[0].dest_ptr = (unsigned char *)
		    dest_req_tbl_p + sizeof(struct request_table);
                assert(!((unsigned long)cntrlr_buf_p->sg_list[0].dest_ptr &
                         NOT_ALIGNED_16_BYTE));
		cntrlr_buf_p->sg_list[0].cnt = entries;

		/* check if sg-list is greater than a page */
		if (entries > (PAGE_SIZE - (SG_ENTS * sizeof
		    (struct io_sg_entry) + sizeof(struct io_sglist_hdr)))) {
                    cntrlr_buf_p->sg_list[0].cnt = PAGE_SIZE -
                        (SG_ENTS * sizeof(struct io_sg_entry) +
                        sizeof(struct io_sglist_hdr));
		    entries -= cntrlr_buf_p->sg_list[0].cnt;
		    sg_p += cntrlr_buf_p->sg_list[0].cnt;
		    dest_req_tbl_p = (struct request_table *)((unsigned char *)
			      dest_req_tbl_p + sizeof(struct request_table) +
			      cntrlr_buf_p->sg_list[0].cnt);
		    for (i = 1; entries; i++) {
			assert(i < SG_LISTS);
			cntrlr_buf_p->sg_list[i].src_ptr = (unsigned char *)
			    kvtophys((vm_offset_t)sg_p);
			assert(!((unsigned long)cntrlr_buf_p->sg_list[i].
				 src_ptr & NOT_ALIGNED_16_BYTE));
			cntrlr_buf_p->sg_list[i].dest_ptr = (unsigned char *)
			    dest_req_tbl_p;
			assert(!((unsigned long)cntrlr_buf_p->sg_list[i].
				 dest_ptr & NOT_ALIGNED_16_BYTE));
			if (entries > PAGE_SIZE) {
			    cntrlr_buf_p->sg_list[i].cnt = PAGE_SIZE;
			    entries -= PAGE_SIZE;
			    sg_p += PAGE_SIZE;
			    dest_req_tbl_p = (struct request_table *)
				((unsigned char *)dest_req_tbl_p + PAGE_SIZE);
			}
			else {  /* reached last page of sg-list */
			    cntrlr_buf_p->sg_list[i].cnt = entries;
			    entries = 0;
			    /* mark end of sg-list */
			    if ((i+1) < SG_LISTS)
				cntrlr_buf_p->sg_list[i+1].src_ptr = 0;
			    else
				cntrlr_buf_p->end_marker = 0;
			}
		    }	/* for (i = 1; entries; i++) */
		}  /* if (entries > (PAGE_SIZE - (SG_ENTS * sizeof ...... */
                else {  /* sg-list is less than equal to a page */
                    /* mark the end of sg-list */
                    cntrlr_buf_p->sg_list[1].src_ptr = 0;
                }
	    }  /* if (entries > SG_ENTS) */
            else {  /* sg list fits in the first request_table */
		    /* mark the end of sg-list */
		    cntrlr_buf_p->sg_list[0].src_ptr = 0;
            }

            /*
             * Flush the i860 caches if desired
             */
            if (sdb_configuration & SDB_CONFIGURATION_ENABLE_FLUSH) {
                    flush();
                    call_mcmsg_dflush_interrupt();
            }

	    /* NOTE THAT cntrlr_buf_p->req_tbl.src_ptr should be the last
	       to be written in cntrlr_buf field of cntrlr_q because it
	       can trigger the SIOP to transfer the request table from DPRAM
	       to CLRAM */
            cntrlr_buf_p->req_tbl.src_ptr = (struct request_table *)
                kvtophys((vm_offset_t)src_req_tbl_p);
	} /* done filling the cntrlr_buf field of cntrlr_q */
	if (++siop->cntrlr_q_index == CNTRLR_Q_SIZE)
	    siop->cntrlr_q_index = 0;

#ifdef	SCSI_STATISTICS
	if (++tgt->statistics.queue_depth > tgt->statistics.queue_max)
		tgt->statistics.queue_max = tgt->statistics.queue_depth;
	SCSI_DEVICE_CHECKIN(tgt, ior_ptr);
#endif	SCSI_STATISTICS

    } /* while ((!queue_empty(&siop->scheduler_q)) && ..... */
    
    /* signal the SIOP to start executing if the cntrlr_q is not empty AND
       the SIOP has no_work_to_do */
    if (cntrlr_q_not_empty(siop) && siop->shrd_locs->stat.byte.no_work_to_do) {
        SLOG(SLOG_MEDIUM, (
		   "    siop_scheduler: no_work_to_do is set by SIOP\n"));
	siop->shrd_locs->stat.byte.no_work_to_do = 0;
	acquire_hdw_sem(siop);
	siop->regs->siop_istat = SIOP_ISTAT_SIGP;
	release_hdw_sem(siop);
    }

#if	MACH_ASSERT
    if (queue_empty(&siop->scheduler_q)) {
        SLOG_EXIT(("siop_scheduler: scheduler_q empty; exit\n"));
    }
    if (cntrlr_q_full(siop)) {
        SLOG_EXIT(("siop_scheduler: cntrlr_q full; exit\n"));
    }
#endif	MACH_ASSERT

    SLOG_EXIT(("siop_scheduler: exit\n"));
}


static void
siop_req_done(struct cntrlr_intf_tag *cntrlr_tag_p, char req_stat)
{
    target_info_t *tgt = cntrlr_tag_p->tag.tgt;
    siop_softc_t  siop = siop_softc[tgt->masterno];
   
    SLOG_ENTRY(14, ("siop_req_done: enter, cntrlr_intf_tag=%x, tgt->done=%x\n",
		    cntrlr_tag_p, req_stat));

#ifdef	SCSI_STATISTICS
	SCSI_DEVICE_CHECKOUT(tgt, cntrlr_tag_p->tag.ior);
	tgt->statistics.queue_depth--;
#endif	SCSI_STATISTICS

    queue_remove(&tgt->active_tag_q, cntrlr_tag_p,
                     struct cntrlr_intf_tag *, tag.links);
    queue_enter(&tgt->free_tag_q, cntrlr_tag_p,
                    struct cntrlr_intf_tag *, tag.links);
    tgt->done = req_stat;
    scsi_queue_restore(tgt, &cntrlr_tag_p->tag);
    assert(siop->wd.nactive);
    if (siop->wd.nactive-- == 1)
	siop->wd.watchdog_state = SCSI_WD_INACTIVE;

    if (is_tgt_q_full(tgt)) {
	tgt_q_not_full(tgt);
    }

    /* when check conditions status is returned by the target, the next
       cmd sent out to the target must be untagged: since cmd_only is true,
       the  siop_scheduler() does not add q_tag in msg_out */
    /* the ONLY time scsi_queue_disable is done is when check condition status
       is returned by the target; any other status returned by the target
       should scsi_queue_enable */
    if (req_stat == SCSI_RET_NEED_SENSE) {
	scsi_queue_disable(cntrlr_tag_p->tag.tgt);
    } else if (!is_scsi_queue_enabled(tgt)) {
	scsi_queue_enable(tgt);
    }

    siop_scheduler(siop);

    /* restart() should be the last one called because the upper layer could
       also call siop_go() */
    if (tgt->ior) {
	(*tgt->dev_ops->restart)(tgt, TRUE);
    }

    SLOG_EXIT(("siop_req_done: exit\n"));
}


static void 
siop_tgt_q_full(siop_softc_t siop)
{
    struct cntrlr_intf_tag *cntrlr_tag_p;
    struct cntrlr_intf_tag *ptr;

    SLOG_ENTRY(15, ("siop_tgt_q_full: enter, unit=%x\n", siop->unit));

    cntrlr_tag_p =
	dealloc_req_tbl_clram(siop, siop->shrd_locs->req_tbl_clram_ptr);

    /* if target's queue is already full then why did we get here */
    assert(!is_tgt_q_full(cntrlr_tag_p->tag.tgt));

    /* check if target's queue is empty; if empty => target's queue is not
       full but target is busy */
    /* find first cntrlr_intf_tag that has been sent to the target and it is
       not the cntrlr_intf_tag that caused the target's queue to fill up */
    queue_iterate(&cntrlr_tag_p->tag.tgt->active_tag_q, ptr, 
		  struct cntrlr_intf_tag *, tag.links) {
	/* sch_links.next == NULL => cntrlr_intf_tag has been scheduled */
	if ((ptr->sch_links.next == NULL) && (cntrlr_tag_p != ptr))
	    break;
    }
    if ((ptr->sch_links.next == NULL) && (cntrlr_tag_p != ptr)) {
	/* target's queue is full */
#ifdef	SCSI_STATISTICS
	SCSI_DEVICE_CHECKOUT(cntrlr_tag_p->tag.tgt, cntrlr_tag_p->tag.ior);
	cntrlr_tag_p->tag.tgt->statistics.queue_depth--;
	cntrlr_tag_p->tag.tgt->statistics.device_full++;
#endif	SCSI_STATISTICS
	siop_cleanup_all_tgt_queues(cntrlr_tag_p, TRUE);
	tgt_q_full(cntrlr_tag_p->tag.tgt);
	/* schedule more requests since cntrlr_q was emptied out */
	siop_scheduler(siop);
        siop->shrd_locs->stat.byte.status_buf = 0;
        siop->shrd_locs->stat.byte.halt_reason = 0;
	siop->regs->siop_dsp = SCRIPT_SCHEDULER(siop->script_phys);
    } else {
	/* target's queue is empty and target is busy */
	siop_trgt_busy(siop, cntrlr_tag_p);
    }
	
    SLOG_EXIT(("siop_tgt_q_full: exit\n"));
}


static void 
siop_tgt_busy(siop_softc_t siop)
{
    SLOG_ENTRY(16, ("siop_tgt_busy: enter, unit=%x\n", siop->unit));

/* good place to check SIOP's ptr to cntrlr_q; from siop->shrd_locs->req_tbl_clram_ptr compare with each dest_ptr of cntrlr_buf until match is found, this 
gives index into cntrlr_q and SIOP should point to next entry in cntrlr_q */
    siop_trgt_busy(siop, dealloc_req_tbl_clram
		   (siop, siop->shrd_locs->req_tbl_clram_ptr));

    SLOG_EXIT(("siop_tgt_busy: exit\n"));
}


static void
siop_trgt_busy(siop_softc_t siop, struct cntrlr_intf_tag *cntrlr_tag_p)
{
    SLOG_ENTRY(18, ("siop_trgt_busy: enter, unit=%d, cntrlr_intf_tag=%x\n",
		    siop->unit, cntrlr_tag_p));

    siop_req_done(cntrlr_tag_p, SCSI_RET_RETRY);
    siop->shrd_locs->stat.byte.status_buf = 0;
    siop->shrd_locs->stat.byte.halt_reason = 0;
    siop->regs->siop_dsp = SCRIPT_SCHEDULER(siop->script_phys);

    SLOG_EXIT(("siop_tgt_busy: exit\n"));
}


static void
siop_chk_cond(siop_softc_t siop)
{
    struct cntrlr_intf_tag *cntrlr_tag_p;
    target_info_t          *tgt;

    SLOG_ENTRY(17, ("siop_chk_cond: enter, unit=%x\n", siop->unit));

    cntrlr_tag_p =
	dealloc_req_tbl_clram(siop, siop->shrd_locs->req_tbl_clram_ptr);
    siop_cleanup_all_tgt_queues(cntrlr_tag_p, FALSE);

    /*
     * The HP DAT drive handles a wide bus negotiation differently.
     * Instead of returning the wide bus message with the bus width or
     * sending a message reject, it changes from message out phase to
     * status phase and returns a check condition status, then sends a
     * command complete message.  The following code checks for this and
     * sets the TGT_DID_WIDE flag so we don't try it again.
     */
    tgt = cntrlr_tag_p->tag.tgt;
    if (tgt->flags & TGT_TRY_WIDE) {
	    tgt->sync_period &= ~SIOP_SCNTL3_EWS;
            tgt->flags &= (~TGT_TRY_WIDE);
            tgt->flags |= TGT_DID_WIDE;
    }

    /* scsi_queue_disable() is done by siop_req_done(....) */
    siop_req_done(cntrlr_tag_p, SCSI_RET_NEED_SENSE);
    siop->shrd_locs->stat.byte.status_buf = 0;
    siop->shrd_locs->stat.byte.halt_reason = 0;
    siop->regs->siop_dsp = SCRIPT_SCHEDULER(siop->script_phys);

    SLOG_EXIT(("siop_chk_cond: exit\n"));
}


static struct request_table *
cntrlr_q_not_empty(siop_softc_t siop)
{
    SLOG_ENTRY(13, ("cntrlr_q_not_empty: enter, unit=%x\n", siop->unit));

    if (siop->cntrlr_q_index) {
	SLOG_EXIT(("cntrlr_q_not_empty: no wrap around, exit, request_table ptr=%x\n", siop->shrd_locs->cntrlr_q[siop->cntrlr_q_index-1].req_tbl.src_ptr));
	return (siop->shrd_locs->cntrlr_q[siop->cntrlr_q_index-1].req_tbl.
		src_ptr);
    } 
    else {
        SLOG_EXIT(("cntrlr_q_not_empty: wrap around, exit, request_table ptr=0x%x\n", siop->shrd_locs->cntrlr_q[CNTRLR_Q_SIZE-1].req_tbl.src_ptr));
        return (siop->shrd_locs->cntrlr_q[CNTRLR_Q_SIZE-1].req_tbl.src_ptr);
    }
}


static void
siop_one_req_done(siop_softc_t siop)
{
    volatile struct request_table *clram_req_tbl_ptr;

    SLOG_ENTRY(20, ("siop_one_req_done: enter, unit=%x\n", siop->unit));

    while (clram_req_tbl_ptr = 
	   siop->shrd_locs->req_done_q[siop->req_done_q_index]) {
	/* zero-out req_done_q[siop->req_done_q_index] before calling
	   siop_req_done() so that an empty slot is quickly available to 
	   SIOP */
	siop->shrd_locs->req_done_q[siop->req_done_q_index] = 0;
	if (++siop->req_done_q_index == REQ_DONE_Q_SIZE)
	    siop->req_done_q_index = 0;
        siop_req_done(dealloc_req_tbl_clram(siop, clram_req_tbl_ptr), 
		      SCSI_RET_SUCCESS);
    }
    SLOG_EXIT(("siop_one_req_done: exit\n"));
}


static void
siop_all_req_done(siop_softc_t siop)
{
    SLOG_ENTRY(21, ("siop_all_req_done: enter, unit=%x\n", siop->unit));

    siop_one_req_done(siop);
    assert(siop->shrd_locs->req_tbl_clram_ptr);
    siop_req_done(dealloc_req_tbl_clram(siop,
        siop->shrd_locs->req_tbl_clram_ptr), SCSI_RET_SUCCESS);
    siop->shrd_locs->stat.byte.halt_reason = 0;
    siop->regs->siop_dsp = SCRIPT_SCHEDULER(siop->script_phys);

    SLOG_EXIT(("siop_all_req_done: exit\n"));
}


static void
siop_other_msg(siop_softc_t siop)
{
    target_info_t *tgt;

    SLOG_ENTRY(22, ("siop_other_msg: enter, unit=%x\n", siop->unit));

    /* read target_id from the request table in clram and use it to find tgt;
       NOTE that dealloc_req_tbl_clram(..) cannot be used because the command
       has not ended yet */

    tgt = (get_cntrlr_intf_tag(siop,
		siop->shrd_locs->req_tbl_clram_ptr))->tag.tgt;

    if ((siop->shrd_locs->stat.byte.msg_in_count == 5) && 
	(siop->shrd_locs->msg_in.byte[2] == SCSI_SYNC_XFER_REQUEST)) {

	/* can't have an offset greater than 8 */
	assert(siop->shrd_locs->msg_in.byte[4] <= SIOP_SYNCH_OFFSET);

	/* sync offset from target = zero => asynchronous transfers */
	if ((siop->shrd_locs->msg_in.byte[4] > SIOP_ASYNC_OFFSET) && 
	    (siop->shrd_locs->msg_in.byte[4] <= SIOP_SYNCH_OFFSET)) {

	    /* SXFER(TP2-0) = msg_in.byte[3], SXFER(MO3-MO0) = msg_in.byte[4],
	       SXFER(RES) = 0 */
	    if (!(scsi_period_to_siop(siop->shrd_locs->msg_in.byte[3]) >= 
		 SIOP_MIN_XFERP && scsi_period_to_siop
		 (siop->shrd_locs->msg_in.byte[3]) <= SIOP_MAX_XFERP)) {
		panic("illegal Transfer Period Factor received from target %d during synchronous negotiation\n", ((struct request_table *)
            (siop->req_tbl_ptr_clram + (siop->shrd_locs->req_tbl_clram_ptr -
             siop->req_tbl_ptr_clram_phys)))->bus_conf.dest_id);
	    }
	    tgt->sync_offset = (scsi_period_to_siop
				(siop->shrd_locs->msg_in.byte[3])
				- SIOP_MIN_XFERP) << SIOP_TP_SHIFT;
	    tgt->sync_offset |= (siop->shrd_locs->msg_in.byte[4] & 
		SIOP_SXFER_MO_MASK);
	
	    /* SCNTL3(SCF2-0) = msg_in.byte[3], SCNTL3(RES) = 0 */
	    tgt->sync_period &= ~SIOP_SCNTL3_SCF_MASK;
	    tgt->sync_period |= (scsi_period_to_siop
                                (siop->shrd_locs->msg_in.byte[3])
                                - SIOP_MIN_XFERP) << SIOP_SCF_SHIFT;
	} else {
	    panic("siop%d tgt%d: Invalid synchrounous offset %d\n",
		siop->unit, tgt->target_id, siop->shrd_locs->msg_in.byte[4]);
	    tgt->sync_offset = siop->siop_sxfer;
            tgt->sync_period = siop->siop_scntl3;
	}
	tgt->flags &= (~TGT_TRY_SYNCH);
        tgt->flags |= TGT_DID_SYNCH;
    }
    else if ((siop->shrd_locs->stat.byte.msg_in_count == 4) &&
        (siop->shrd_locs->msg_in.byte[2] == SCSI_WIDE_XFER_REQUEST)) {
	assert(siop->shrd_locs->msg_in.byte[3] <= 1);
	if (siop->shrd_locs->msg_in.byte[3]) {
            tgt->sync_period |= SIOP_SCNTL3_EWS;
	}
	else {
	    tgt->sync_period &= ~SIOP_SCNTL3_EWS;
	}
        tgt->flags &= (~TGT_TRY_WIDE);
        tgt->flags |= TGT_DID_WIDE;
    }
    else if ((siop->shrd_locs->stat.byte.msg_in_count == 1) &&
        (siop->shrd_locs->msg_in.byte[0] == SCSI_MESSAGE_REJECT)) {
	assert(!(tgt->flags & TGT_TRY_WIDE && tgt->flags & TGT_TRY_SYNCH));
	if (tgt->flags & TGT_TRY_WIDE) {
            tgt->sync_period &= ~SIOP_SCNTL3_EWS;
	    tgt->flags &= (~TGT_TRY_WIDE);
	    tgt->flags |= TGT_DID_WIDE;
	}
	else if (tgt->flags & TGT_TRY_SYNCH) {
            tgt->sync_offset = siop->siop_sxfer;
            tgt->sync_period = siop->siop_scntl3;
	    tgt->flags &= (~TGT_TRY_SYNCH);
	    tgt->flags |= TGT_DID_SYNCH;
	}
	else {
	    /* msg-out is neither wide or sync negotiation; it is most probably
	       a bad msg-out */
	    panic("siop_other_msg: message reject received due to possibly a bad msg-out\n");
	}
    }
    else { /* target sent an unknown message that is not implemented */
	panic("siop_other_msg: unknown message from target, SIOP%d\n",
	      siop->unit);
	(void)siop_reset(siop);
	SLOG_EXIT(("siop_other_msg: unknown message, exit\n"));
	return;
    }
    siop->shrd_locs->stat.byte.msg_in_count = 0;
    siop->shrd_locs->stat.byte.halt_reason = 0;
    /* after the target is in msg_in phase, it will go to cmd phase and
       the initiator will send the cmd */
    siop->regs->siop_dsp = SCRIPT_MAIN(siop->script_phys);
    
    SLOG_EXIT(("siop_other_msg: exit\n"));
}


static void
siop_sel_timeout(siop_softc_t siop)
{
    struct cntrlr_intf_tag *cntrlr_tag_p;
    unsigned char script_cntrlr_q_index;
    struct target_info *tgt;

    SLOG_ENTRY(23, ("siop_sel_timeout: enter, unit=%x\n", siop->unit));

    cntrlr_tag_p =
	dealloc_req_tbl_clram(siop, siop->shrd_locs->req_tbl_clram_ptr);

    /* when select timeout occurs, the SIOP is pointing at the cntrlr_buf in
       the cntrlr_q that caused this problem; need to move this SIOP pointer 
       to the next cntrlr_buf before calling siop_cleanup_all_tgt_queues() */

    script_cntrlr_q_index =
      (*((struct cntrlr_buf **)SCRIPT_CNTRLR_Q_P(siop->script))) -
      ((struct cntrlr_buf *)kvtophys((vm_offset_t)(siop->shrd_locs->cntrlr_q)));

    /* this request that caused the select timeout is done by the SIOP */
    assert(siop->shrd_locs->cntrlr_q[script_cntrlr_q_index].req_tbl.src_ptr);
    siop->shrd_locs->cntrlr_q[script_cntrlr_q_index].req_tbl.src_ptr = NULL;

    if (++script_cntrlr_q_index == CNTRLR_Q_SIZE)
	script_cntrlr_q_index = 0;

    *((struct cntrlr_buf **)SCRIPT_CNTRLR_Q_P(siop->script)) = 
	((struct cntrlr_buf *)kvtophys((vm_offset_t)
            (siop->shrd_locs->cntrlr_q))) + script_cntrlr_q_index;

    siop_cleanup_all_tgt_queues(cntrlr_tag_p, FALSE);

    siop_req_done(cntrlr_tag_p, SCSI_RET_DEVICE_DOWN);
    siop->regs->siop_dsp = SCRIPT_SCHEDULER(siop->script_phys);

    /* if the target was there during probe but it got disconnected from the 
       scsi bus then we should reclaim the resources (cntrlr_intf_tags)
       associated with this target when the active_tag_q and the request_q are
       empty; unfortunately the upper layer will keep on sending new
       user requests to us even when it knows that this target is down,
       therefore we should keep only one cntrlr_intf_tag */
    {
        scsi_softc_t	*sc = siop->sc;
        unsigned char	target_id;
        int		lun;

        target_id = cntrlr_tag_p->tag.tgt->target_id;

	/*
         * reclaim resources for all LUNs on this target
         */
        for (lun = 0; lun < MAX_LUNS; lun++) {

	    /* except of course ourselves */
	    if (target_id == sc->initiator_id)
	        continue;

            tgt = sc->target[target_id][lun];

            if (tgt->flags & TGT_FULLY_PROBED) {
	        tgt->flags = 0; /* this deasserts TGT_ALIVE too */
            }
    
            if (queue_empty(&tgt->active_tag_q) && !tgt->ior) {
	        SLOG(SLOG_LOW,
	         ("  siop_sel_timeout: tear down resources for dead target\n"));
	        if (!siop_tag_q_init(tgt, 1)) {
	            /* cannot alloc space for cntrlr_intf_tag */
	            panic("siop_sel_timeout:Memory allocation for siop%d failed\n",
                        siop->unit);
	        }
	    }
        }
    }

    SLOG_EXIT(("siop_sel_timeout: exit\n"));
}


static struct request_table *
alloc_req_tbl_clram(siop_softc_t siop, int num_req_tbls_clram,
		    struct cntrlr_intf_tag *cntrlr_tag_p)
{
    int index, next_index;

    SLOG_ENTRY(11, ("alloc_req_tbl_clram: enter, siop=0x%x, num_req_tbls_clram=%d, cntrlr_intf_tag=0x%x\n", siop, num_req_tbls_clram, cntrlr_tag_p));

    /* find "num_req_tbls_clram" contiguous entries of NULL in 
       req_tbl_clram_image[] */
    for (index = 0; (index + num_req_tbls_clram) <= MAX_REQ_TBLS_CLRAM;
	 index++) {
	for (next_index = index; req_tbl_clram_image[next_index] == NULL;
	     next_index++) {
	    if ((next_index - index + 1) == num_req_tbls_clram) {
		/* found contiguous entries in "req_tbl_clram_image" to 
		   allocate from */
		for (next_index = index; 
		     (next_index - index) < num_req_tbls_clram; next_index++) {
		    req_tbl_clram_image[next_index] = cntrlr_tag_p;
		}
		SLOG_EXIT(("alloc_req_tbl_clram: allocated, exit, req_tbl_ptr_clram_phys=0x%x\n", siop->req_tbl_ptr_clram_phys + index));
		return(siop->req_tbl_ptr_clram_phys + index);
	    }
	}
    }

    SLOG_EXIT(("alloc_req_tbl_clram: cannot allocate, exit, req_tbl_ptr_clram_phys=NULL\n"));
    return NULL;
}


static struct cntrlr_intf_tag *
dealloc_req_tbl_clram(siop_softc_t siop, volatile struct request_table *req_tbl_clram_ptr)
{
    struct cntrlr_intf_tag      *cntrlr_tag_p, **ptr;
    int	                        index;

    SLOG_ENTRY(12,
	("dealloc_req_tbl_clram: enter, siop=0x%x, req_tbl_clram_ptr=0x%x\n",
	siop, req_tbl_clram_ptr));

    index = req_tbl_clram_ptr - siop->req_tbl_ptr_clram_phys;
    ptr = &req_tbl_clram_image[index];

    /* remove all request tables allocated */
    for (cntrlr_tag_p = *ptr;
         *ptr == cntrlr_tag_p && ptr < &req_tbl_clram_image[MAX_REQ_TBLS_CLRAM];
         ptr++) {
	*ptr = NULL;
    }

#if	MACH_ASSERT
    /* check if cntrlr_tag_p is in the active_tag_q */
    {
	struct cntrlr_intf_tag *ptr_cntrlr_intf_tag;

	/* check if cntrlr_tag_p is valid */
	assert (cntrlr_tag_p);
	assert (cntrlr_tag_p->tag.tgt->masterno >= 0 && 
		cntrlr_tag_p->tag.tgt->masterno < NSIOP);
        assert (cntrlr_tag_p->tag.tgt->target_id >= 0 &&
                cntrlr_tag_p->tag.tgt->target_id < MAX_SCSI_TARGETS);
	/* check if cntrlr_tag_p is in the active_tag_q */
	queue_iterate(&cntrlr_tag_p->tag.tgt->active_tag_q,
	      ptr_cntrlr_intf_tag, struct cntrlr_intf_tag *, tag.links) {
	    if (ptr_cntrlr_intf_tag == cntrlr_tag_p)
		break;
	}
	assert (ptr_cntrlr_intf_tag == cntrlr_tag_p);
    }
#endif	MACH_ASSERT
    SLOG_EXIT(("dealloc_req_tbl_clram: exit, cntrlr_intf_tag ptr=%x\n",
	       cntrlr_tag_p));

    return cntrlr_tag_p;
}


static void
siop_cleanup_all_tgt_queues(struct cntrlr_intf_tag *cntrlr_tag_p, 
			    char rm_last_tag)
{
    struct cntrlr_intf_tag *ptr, *ptr0;
    siop_softc_t siop;
    target_info_t   *tgt;
    unsigned char script_cntrlr_q_index;

    SLOG_ENTRY(19, ("siop_cleanup_all_tgt_queues: enter, cntrlr_intf_tag=%x, rm_last_tag=%d\n", cntrlr_tag_p, rm_last_tag));

    tgt = cntrlr_tag_p->tag.tgt;
    siop = siop_softc[tgt->masterno];

    for (ptr0 = (struct cntrlr_intf_tag *)queue_last(&tgt->active_tag_q);
	  !queue_end((&tgt->active_tag_q), (queue_entry_t)(ptr0));) {
	ptr = ptr0; /* queue_enter(...) changes the tag.links */
	ptr0 = (struct cntrlr_intf_tag *)queue_prev(&(ptr0)->tag.links);
        if (ptr->sch_links.next != NULL) { /* check if in scheduler_q */
	    assert(ptr != cntrlr_tag_p);
	    /* remove from scheduler_q */
            queue_remove(&siop->scheduler_q, 
			 ptr, struct cntrlr_intf_tag *, sch_links);
            ptr->sch_links.next = NULL; /* remove from scheduler_q */
	    /* move from active_q to tail of free_q */
	    queue_remove(&tgt->active_tag_q, ptr,
                     struct cntrlr_intf_tag *, tag.links);
	    queue_enter(&tgt->free_tag_q, ptr,
                    struct cntrlr_intf_tag *, tag.links);
	    /* move to head of request_q */
	    req_q_enter_first((tag_info_t *)&ptr->tag, tgt);
#ifdef	SCSI_STATISTICS
            tgt->statistics.chain_depth++;
#endif	SCSI_STATISTICS
	    assert(siop->wd.nactive);
            if (siop->wd.nactive-- == 1)
                siop->wd.watchdog_state = SCSI_WD_INACTIVE;
	}
    }

    /* empty out the cntrlr_q */

    /* both the host and the SIOP point to the next
       cntrlr_buf in cntrlr_q that has not yet been operated on:
       (1) when both SIOP and host point to the same cntrlr_buf and 
           (i) cntrlr_buf.req_tbl.src_ptr == NULL => cntrlr_q is empty, 
           (ii) cntrlr_buf.req_tbl.src_ptr == not NULL => cntrlr_q is full;
       (2) when host does not point at the same cntrlr_buf as the SIOP then
           cntrlr_buf.req_tbl.src_ptr == NULL => cntrlr_q is not full =>
	   cntrlr_q is not empty */

    script_cntrlr_q_index =
      (*((struct cntrlr_buf **)SCRIPT_CNTRLR_Q_P(siop->script))) -
      ((struct cntrlr_buf *)kvtophys((vm_offset_t)(siop->shrd_locs->cntrlr_q)));

    if ((siop->cntrlr_q_index == script_cntrlr_q_index) && (siop->shrd_locs->
	    cntrlr_q[script_cntrlr_q_index].req_tbl.src_ptr == NULL)) {
	/* cntrlr_q is empty */
	SLOG(SLOG_MEDIUM, (
		   "    siop_cleanup_all_tgt_queues:cntrlr_q is empty\n"));
	if (rm_last_tag)
	    goto remove_last_tag;
        SLOG_EXIT(("siop_cleanup_all_tgt_queues: cntrlr_q empty, exit\n"));
	return;
    } else {
	if (script_cntrlr_q_index) {
	    script_cntrlr_q_index--;
	} else {
	    script_cntrlr_q_index = CNTRLR_Q_SIZE - 1;
	}
	if (siop->cntrlr_q_index) {
	    siop->cntrlr_q_index--;
	} else {
	    siop->cntrlr_q_index = CNTRLR_Q_SIZE - 1;
	}
    }
    /* iterate from the tail to the head of cntrlr_q for cntrlr_buf that have
       not been removed by SIOP */
    do {
	ptr = dealloc_req_tbl_clram(siop, siop->shrd_locs->cntrlr_q[siop->
				      cntrlr_q_index].req_tbl.dest_ptr);
	assert(ptr->sch_links.next == NULL); /* should not be on scheduler_q */
	assert(ptr != cntrlr_tag_p);

	siop->shrd_locs->cntrlr_q[siop->cntrlr_q_index].req_tbl.src_ptr = NULL;

	if (siop->shrd_locs->cntrlr_q[siop->cntrlr_q_index].info.target_id == 
	    tgt->target_id) { /* request belonging to the same target */
            /* move from active_q to tail of free_q */
            queue_remove(&tgt->active_tag_q, ptr,
                     struct cntrlr_intf_tag *, tag.links);
            queue_enter(&tgt->free_tag_q, ptr,
                    struct cntrlr_intf_tag *, tag.links);
            /* move to head of request_q */
            req_q_enter_first((tag_info_t *)&ptr->tag, tgt);
#ifdef	SCSI_STATISTICS
            tgt->statistics.chain_depth++;
#endif	SCSI_STATISTICS
	    assert(siop->wd.nactive);
            if (siop->wd.nactive-- == 1)
                siop->wd.watchdog_state = SCSI_WD_INACTIVE;
	} else { /* request belonging to the different target */
            queue_enter(&siop->scheduler_q,
                         ptr, struct cntrlr_intf_tag *, sch_links);
	}

	if (siop->cntrlr_q_index) {
	    siop->cntrlr_q_index--;
	} else {
	    siop->cntrlr_q_index = CNTRLR_Q_SIZE - 1;
	}
    } while (siop->cntrlr_q_index != script_cntrlr_q_index);

    /* siop->cntrlr_q_index and script_cntrlr_q_index must point to the same
       cntrlr_buf in cntrlr_q */
    siop->cntrlr_q_index = 0;
    *((struct cntrlr_buf **)SCRIPT_CNTRLR_Q_P(siop->script)) = 
	(struct cntrlr_buf *)kvtophys((vm_offset_t)(siop->shrd_locs->cntrlr_q));

remove_last_tag:
    if (rm_last_tag) {
	SLOG(SLOG_MEDIUM, ("    siop_cleanup_all_tgt_queues: removing cntrlr_intf_tag that caused intr form SIOP\n"));
	/* move from active_q to tail of free_q */
	queue_remove(&tgt->active_tag_q, cntrlr_tag_p,
		     struct cntrlr_intf_tag *, tag.links);
	queue_enter(&tgt->free_tag_q, cntrlr_tag_p,
		    struct cntrlr_intf_tag *, tag.links);
	/* move to head of request_q */
	req_q_enter_first((tag_info_t *)&cntrlr_tag_p->tag, tgt);
#ifdef	SCSI_STATISTICS
        tgt->statistics.chain_depth++;
#endif	SCSI_STATISTICS
	assert(siop->wd.nactive);
        if (siop->wd.nactive-- == 1)
	    siop->wd.watchdog_state = SCSI_WD_INACTIVE;
    }
	
    SLOG_EXIT(("siop_cleanup_all_tgt_queues: exit\n"));
}


/*
 * Interrupt routine
 *	Take interrupts from the SCSI chip
 *
 * Implementation:
 *	Read chip status registers and halt_reason to
 *	determine what to do.  Invoke an error handler
 *	if something has gone awry.
 */
int
siop_intr(unit)
int unit;
{
        siop_softc_t            siop;
        siop_regmap_t           *regs;
	unsigned short		sist;
        unsigned char	       istat, dstat;
        unsigned char	       sist0, sist1;
        struct cntrlr_shrd_data *shdata;

        SLOG_ENTRY(9, ("siop_intr: enter, unit=%d\n", unit));

        assert(unit < NSIOP);

        /*
         * Interrupts are generated by either the siop SCRIPT or
         * the chip itself.
         *
         * SCRIPT interrupts are as follows:
         *
         *      Instruction     Shared Data
         *      -----------     ---------------------------------------------
         *      INTFLY          no_work_to_do == TRUE
         *      INTFLY          req_done_q completed request with good status
         *      INT             halt_reason == STAT_NOT_GOOD
         *      INT             halt_reason == REQ_DONE_Q_FULL
         *      INT             halt_reason == ERROR_UNKNOWN_PHASE
         *      INT             halt_reason == ERROR_RESELECT_ID
         *      INT             halt_reason == OTHER_MSG
         *      INT             halt_reason == ERROR_WRONG_PHASE
         *
         * During debugging, there may also be a DEBUG_MSG interrupt
         * from the SCRIPT.
         *
         * SIOP chip interrupts are as follows:
         *
         *      Interrupt                 Action
         *      ---------------------     -------------------------------------
         *      Selection Timeout . . . . call siop_sel_timeout(), tgt is down
         *      Phase Mismatch. . . . . . call siop_phase_mismatch() to recover
         *      Unexpected Disconnect . . print message, reset SCSI bus
         *      SCSI Bus Reset. . . . . . call siop_was_reset() to clean up
         *      Bus Fault . . . . . . . . print message, reset SCSI bus
         *      Abort . . . . . . . . . . print message, reset SCSI bus
         *      Parity Errors . . . . . . print message, reset SCSI bus
         *      Illegal Instruction . . . print message, reset SCSI bus
         *      Gross Errors. . . . . . . print message, reset SCSI bus
         *      Single-Step Interrupt . . call siop_debugger()
         */

        /*
         * Initialize some pointers
         */
        siop = siop_softc[unit];
        assert(siop);
        regs = siop->regs;
        assert(regs);
        shdata = siop->shrd_locs;
        assert(shdata);

        /*
         * The istat register must be read first
         */
        acquire_hdw_sem(siop);
        istat = regs->siop_istat;
	release_hdw_sem(siop);

        /*
         * Drop spurious interrupts
         */
        if (!(istat & (SIOP_ISTAT_ABRT |        /* Abort Operation */
                       SIOP_ISTAT_INTF |        /* INTFLY          */
                       SIOP_ISTAT_SIP  |        /* SCSI Interrupt  */
                       SIOP_ISTAT_DIP))) {      /* DMA Interrupt   */

                SLOG(SLOG_MEDIUM, ("    siop_intr: spurious interrupt!\n"));

                assert(0);                      /* Unexpected condition */

                goto siop_intr_done;
        }

	siop->wd.watchdog_state = SCSI_WD_ACTIVE;

        /*
         * Check and handle INTFLY's
         *
         * INTFLY's must be cleared before servicing any
         * other interrupts indicated by SIP or DIP
         */
        if (istat & SIOP_ISTAT_INTF) {

                SLOG(SLOG_LOW, ("  siop_intr: INTFLY\n"));

		/* there is a possibility that INTFLY can happen but 
		   no_work_to_do or req_done_q are not set; Ex. SIOP
		   could assert INTFLY twice as a result of two requests
		   getting done but the host could service both the requests
		   after the first INTFLY */

                /*
                 * Write a one to the INTF bit to clear; NOTE: Clear INTF bit
		 * before checking no_work_to_do or req_done_q so that race
		 * conditions between host and SIOP do not happen
                 */
		acquire_hdw_sem(siop);
                regs->siop_istat |= SIOP_ISTAT_INTF;
		release_hdw_sem(siop);

                /*
                 * Check for no work to do
                 */
                if (shdata->stat.byte.no_work_to_do) {
                        SLOG(SLOG_MEDIUM, ("    siop_intr: no_work_to_do\n"));

                        /*
                         * Feed the hungry siop more requests
                         */
                        siop_scheduler(siop);
		}

                /*
                 * Check for a completed request
                 */
                if (shdata->req_done_q[siop->req_done_q_index]) {
                    SLOG(SLOG_MEDIUM, ("    siop_intr: request done[%d]=0x%x\n",
			siop->req_done_q_index,
			shdata->req_done_q[siop->req_done_q_index]));

                        /*
                         * A request has completed
                         */
                        siop_one_req_done(siop);
		}

		/*
		 * When INTFLY is cleared the 53C720 interrupt line deasserts 
		 * even if the SIP or DIP bits in istat are set, then asserts
		 * again if necessary.  To avoid spurious interrupts, we need
		 * to return here before handling the SIP and DIP interrupts.
		 */
		return 0;
	}

        /*
         * Check for an Abort Operation interrupt
         */
        if (istat & SIOP_ISTAT_ABRT) {
                panic("\nsiop%d: Abort Operation Interrupt, istat=0x%02x\n",
                        unit, istat);

                assert(0);                      /* Unexpected condition */

                /*
                 * As stated in the NCR 53C720 SCSI I/O Processor Data Manual:
                 *
                 *      If this bit is set and an interrupt is received,
                 *      reset this bit before reading the DSTAT register
                 *      to prevent further aborted interrupts from being
                 *      generated.
                 */
                regs->siop_istat &= ~SIOP_ISTAT_ABRT;
	    }

        /*
         * As stated in the NCR 53C720 SCSI I/O Processor Data Manual:
         *
         *      When performing consecutive 8-bit reads of the DSTAT,
         *      SIST0, and SIST1 registers (in any order), insert a
         *      delay equivalent to 12 BCLK periods between the reads
         *      to ensure the interrupts clear properly.  Also, if
         *      reading the registers when both the ISTAT SIP (bit 1)
         *      and DIP (bit 0) bits may not be set, the SIST0 and
         *      SIST1 registers should be read before the DSTAT register
         *      to avoid missing a SCSI interrupt.
	 *
	 * We read the sist0 and sist1 registers together as a short, but
	 * still place the BCLK delay before reading dstat.
         */
	/* if SIP or DIP is set then the SIOP has halted and we don't need
	   to acquire and release the hardware semaphore */
        if (istat & SIOP_ISTAT_SIP) {
	    sist = regs->siop_sist;	    /* read sist0 and sist1 */
	    sist0 = (sist >> 0) & 0xff;
	    sist1 = (sist >> 8) & 0xff;
	    delay(1);                       /* BCLK delay */
	}
        if (istat & SIOP_ISTAT_DIP) {
	    dstat = regs->siop_dstat;
	    delay(1);                       /* BCLK delay */
	}

        /*
         * Check and handle DMA interrupts
         *
         * DMA interrupts should be serviced before SCSI interrupts
         * because a serious DMA interrupt condition could influence
         * how the SCSI interrupt is acted upon.
         */
        if (istat & SIOP_ISTAT_DIP) {
                SLOG(SLOG_LOW, ("    siop_intr: DMA interrupt\n"));

                assert(dstat & (SIOP_DSTAT_HPE  | /* Host Parity Error     */
                                SIOP_DSTAT_BF   | /* Bus Fault             */
                                SIOP_DSTAT_ABRT | /* Aborted               */
                                SIOP_DSTAT_SSI  | /* Single-Step Interrupt */
                                SIOP_DSTAT_SIR  | /* INT Instruction       */
                                SIOP_DSTAT_WTD  | /* Watchdog Timeout      */
                                SIOP_DSTAT_IID)); /* Illegal Instruction   */

                /*
                 * Mask off the status bits to check for error conditions
                 * These errors result in a SCSI bus reset to restart any
                 * operations that may have been active.
                 */
                if (dstat & (SIOP_DSTAT_HPE  |  /* Host Parity Error   */
                             SIOP_DSTAT_BF   |  /* Bus Fault           */
                             SIOP_DSTAT_ABRT |  /* Aborted             */
                             SIOP_DSTAT_WTD  |  /* Watchdog Timeout    */
                             SIOP_DSTAT_IID)) { /* Illegal Instruction */

                        if (dstat & SIOP_DSTAT_HPE)
                                panic("siop%d: Host Parity Error\n", unit);
                        if (dstat & SIOP_DSTAT_BF)
                                panic("siop%d: Bus Fault\n", unit);
                        if (dstat & SIOP_DSTAT_ABRT)
                                panic("siop%d: Abort Interrupt\n", unit);
                        if (dstat & SIOP_DSTAT_WTD)
                                printf("siop%d: Watchdog Timeout\n", unit);
                        if (dstat & SIOP_DSTAT_IID)
                                panic("siop%d: Illegal Instruction\n", unit);

                        /*
                         * Reset to start all over
                         */
                        printf("siop%d: Attempting recovery...\n", unit);
                        (void)siop_reset(siop);

                        goto siop_intr_done;
		    }

                /*
                 * Check for a Single-Step Interrupt
                 */
                if (dstat & (SIOP_DSTAT_SSI)) { /* Single-Step         */
                        SLOG(SLOG_MEDIUM, (
				   "    siop_intr: Single-Step Interrupt\n"));

                        /*
                         * Invoke the siop_debugger() and return or
                         * continue based on the function outcome
                         */
                        if (siop_debugger(siop, istat, sist0, sist1, dstat)) {
                                goto siop_intr_done;
			    }
		    }

                /*
                 * Check for a SCRIPT INT instruction
                 */
                if (dstat & (SIOP_DSTAT_SIR)) { /* SCRIPT INT          */
                        unsigned char reason = shdata->stat.byte.halt_reason;

                        SLOG(SLOG_LOW, (
                "    siop_intr: SCRIPT INT Instruction, halt_reason=0x%x\n",
                                reason));

                        switch (reason) {

				      case STAT_NOT_GOOD:
                                        siop_status(siop,
                                (unsigned char *)&shdata->stat.byte.status_buf);
                                        break;

				      case REQ_DONE_Q_FULL:
                                        siop_all_req_done(siop);
                                        break;

				      case ERROR_UNKNOWN_PHASE:
                                        printf(
                            	"siop%d: unknown phase, sbcl=0x%02x\n",
                                                unit, regs->siop_sbcl);
                                        assert(0);  /* Unexpected condition */
                                        siop_error(siop);
                                        goto siop_intr_done;
                                        break;

				      case ERROR_WRONG_PHASE:
                                        printf(
                            	"siop%d: wrong phase, sbcl=0x%02x\n",
                                                unit, regs->siop_sbcl);
                                        assert(0);  /* Unexpected condition */
                                        siop_error(siop);
                                        goto siop_intr_done;
                                        break;

				      case ERROR_RESELECT_ID:
                                        printf(
                      		"siop%d: invalid reselect id, ssid=0x%02x\n",
                                                unit, regs->siop_ssid);
                                        assert(0);  /* Unexpected condition */
                                        siop_error(siop);
                                        goto siop_intr_done;
                                        break;

				      case OTHER_MSG:
                                        siop_other_msg(siop);
                                        break;

				      case 0:
					/* check if SIOP detected corrupted
					   data */
					if (SCRIPT_DEAD(regs->siop_dsps)) {
/* ***********************stop the other SIOP************* */
					    outl(SDB_TRIGGER,(long)0xdeadbeef);
					    panic("siop_intr: Data corruption detected by SIOP%d\n", siop->unit);
					}
                                        /*
                                         * Check for an siop debug message
                                         */
                                        if (SCRIPT_ASSERT(regs->siop_dsps)) {
#if	MACH_ASSERT
                                                siop_debug_msg(siop);
#else	MACH_ASSERT
                                                panic("Unexpected SIOP debug me\
ssage");
                                                /*
                                                 * Restart the siop SCRIPT
                                                 */
                                                regs->siop_dsp = regs->siop_dsp\
;
#endif	MACH_ASSERT
                                            }
                                        break;

				      default:
					printf(
                          	"siop%d: unknown halt_reason 0x%02x\n",
                                                        unit, reason);
                                                /* Unexpected condition */
                                                assert(0);
                                                siop_error(siop);
                                        goto siop_intr_done;
                                        break;
				    } /* switch (reason) */
		    } /* if (dstat & (SIOP_DSTAT_SIR)) */
	    }  /* if (istat & SIOP_ISTAT_DIP) */

        /*
         * Check and handle SCSI interrupts
         */
        if (istat & SIOP_ISTAT_SIP) {

                SLOG(SLOG_LOW,
	    ("    siop_intr: SCSI interrupt, istat=%x,sist0=%x, sist1=%x \n",
			istat, sist0, sist1));

                assert(sist0 | sist1);

                /*
                 * Mask off the status bits to check for error conditions
                 * These errors result in a SCSI bus reset to restart any
                 * operations that may have been active.
                 */
                if (sist0 & (SIOP_SIST0_SGE |   /* SCSI Gross Error      */
                             SIOP_SIST0_UDC |   /* Unexpected Disconnect */
                             SIOP_SIST0_PAR)) { /* SCSI Bus Parity Error */

                        if (sist0 & SIOP_SIST0_SGE)
                                printf("siop%d: SCSI Gross Error\n", unit);
                        if (sist0 & SIOP_SIST0_UDC)
                                printf("siop%d: Unexpected Disconnect\n", unit);
                        if (sist0 & SIOP_SIST0_PAR)
                                printf("siop%d: SCSI Bus Parity Error\n", unit);

                        /*
                         * Reset to start all over
                         */
                        printf("siop%d: Attempting recovery...\n", unit);
                        (void)siop_reset(siop);

                        goto siop_intr_done;
		    }

                /*
                 * Determine if the SCSI bus was reset (we initiated it)
                 */
                if (sist0 & SIOP_SIST0_RST) {   /* SCSI Bus RST Received */
                        SLOG(SLOG_MEDIUM, (
				   "    siop_intr: SCSI Bus RST Received\n"));

                        /*
                         * Call the siop_was_reset function to clean
                         * up the controller interface and restart any
                         * active operations
                         */
                        siop_was_reset(siop);

                        goto siop_intr_done;
		    }

                /*
                 * Check for a phase mismatch interrupt
                 */
                if (sist0 & SIOP_SIST0_M_A) {   /* Phase Mismatch        */
			SLOG(SLOG_MEDIUM, ("    siop_intr: Phase Mismatch\n"));

			/*
			 * Call the phase mismatch handler routine
			 * to clean up the siop chip data paths
			 */	
			siop_phase_mismatch(siop, shdata);

			goto siop_intr_done;
		}

                /*
                 * Check for a selection timeout
                 */
                if (sist1 & SIOP_SIST1_STO) {   /* Selection Timeout     */
                        SLOG(SLOG_MEDIUM, (
				   "    siop_intr: Selection Timeout\n"));

                        /*
                         * Call the selection timeout routine
                         * to clean up this request and others
                         * queued up for this target
                         */
                        siop_sel_timeout(siop);

                        goto siop_intr_done;
		    }

                /*
                 * Check for General Purpose Timer Expiry
                 */
                if (sist1 & SIOP_SIST1_GEN) {   /* General Purpose Timer */
                        SLOG(SLOG_MEDIUM, (
				   "    siop_intr: Selection Timeout\n"));

                        /*
                         * We don't use this timer
                         */
                        assert(0);  /* Unexpected condition */
		    }

                /*
                 * Check for Handshake-Handshake Timer Expiry
                 */
                if (sist1 & SIOP_SIST1_HTH) {   /* Handshake Timer       */
                        SLOG(SLOG_MEDIUM, (
				   "    siop_intr: Selection Timeout\n"));

                        /*
                         * We don't use this timer
                         */
                        assert(0);  /* Unexpected condition */
		    }
	    }

siop_intr_done:

        SLOG_EXIT(("siop_intr: exit\n"));

        return 0;
}

/*
 * Phase Mismatch Handler
 *
 * Perform byte recovery on the siop data paths
 * Provide SCRIPT with an updated next_addr value
 */
static void
siop_phase_mismatch(siop_softc_t siop, struct cntrlr_shrd_data *shdata)
{
	siop_regmap_t	*regs;

        SLOG_ENTRY(24,
		("siop_phase_mismatch: enter, siop=0x%x, shdata=0x%x\n",
			siop, shdata));

	/*
	 * Initialize register pointer
	 */
	regs = siop->regs;
	assert(regs);

	/*
	 * If a phase mismatch occured during command phase the target
	 * took all the command bytes it wanted, so just continue.
	 * Otherwise, the target may have switched to data in phase,
	 * and clearing the chip data paths will destroy any data
	 * already sent by the target.
	 */
	if (regs->siop_dsp == SCRIPT_CMD_SENT(siop->script_phys)) {

		SLOG(SLOG_MEDIUM,
	    ("    siop_phase_mismatch: Phase Mismatch during CMD phase\n"));

		/*
		 * Restart the siop from the current address
		 */
		regs->siop_dsp = regs->siop_dsp;

        	SLOG_EXIT(("siop_phase_mismatch: exit\n"));
		return;
	}

        /*
         * Make sure cache-line bursting is disabled
         * The SCRIPT code enables it at the appropriate time
         */
        regs->siop_ctest0 |= SIOP_CTEST0_CDIS;

        /*
         * When a phase mismatch interrupt occurs, the 720 interrupts the
         * host and halts.  If this was a data transfer, the 720 may be
         * holding on to some of the data bytes.  For writes to the target
         * (DATA_OUT), just count the number of bytes seen by the target;
         * for reads (DATA_IN), the bytes need to be written to memory.
         *
         * In either case, the chip needs to be cleaned afterwards so the
         * lingering bytes do not interfere with subsequent I/O requests.
         * Adjust the data buffer address (next_addr) for the 720 as required.
         */

        /*
         * Check DDIR (data transfer direction)
         */
	if (regs->siop_ctest2 & SIOP_CTEST2_DDIR) {
                unsigned char   *cp;
                boolean_t       wide;
                int             i;

                /* DATA_IN */
                SLOG(SLOG_MEDIUM, ("    siop_phase_mismatch: DATA_IN\n"));

                /*
                 * The receive data path is:
                 *
                 * SCSI Device -> SCSI FIFO -> SWIDE -> DMA FIFO -> Host Memory
                 *                    or
                 *                   SIDL
                 *
                 * The SCSI FIFO is used for synchronous transfers; SIDL is
                 * for asynchronous transfers.  SWIDE is one byte, SIDL is
                 * up to two bytes.
                 *
                 * The target will not resend the data frozen in these
                 * registers, at least we cannot count on it too.  The
                 * objective is moving all the data to host memory.
                 *
                 * First thing is to make sure the DMA FIFO is flushed.
                 * Thomas Colino, on 27 may 94 asserted that a SCSI error
                 * (e.g., a phase mismatch) causes the DMA portion of the
                 * chip to be flushed out before halting.  In spite of this,
                 * the code to ensure it is simple.
                 *
                 * This is done by setting the FLF bit (CTEST3 bit 3) for
                 * awhile.  The book says to clear the bit after the "720 has
                 * successfully cleared the appropriate FIFO pointers."  No
                 * hints on when that has happened though.  The answer is to
                 * poll the DMA FIFO empty bit (DFE bit 7 in DSTAT).
                 */

                regs->siop_ctest3 |=  SIOP_CTEST3_FLF;  /* turn on DMA flush */

		/*
		 * According to Wayne Moore we do not need
		 * to aquire the hardware semaphore here
		 */
                for (i = 0; !(regs->siop_dstat & SIOP_DSTAT_DFE) && 
		     (i < 100000); i++) {
		    delay(1);		/* so host doesn't hog controller bus */
		}
		if (i >= 100000) {
		    panic("SIOP DMA FIFO flush timeout");
		}

                regs->siop_ctest3 &= ~SIOP_CTEST3_FLF;  /* turn off DMA flush */

                /*
                 * Use "DMA Next Data Address" from the chip
                 * for the destination of the recovered bytes.
                 */
                cp = (unsigned char *)regs->siop_dnad;
                assert(cp);

                /*
                 * Convert from physical to virtual address
                 */
                cp = (unsigned char *)phystokv(cp);

                /*
                 * Recover the bytes in the order of the data path.  First is
                 * SWIDE.  SWIDE is full if WSR is set (bit 0 in SCNTL2).
                 */
                if (regs->siop_scntl2 & SIOP_SCNTL2_WSR)
                        *cp++ = regs->siop_swide;

                /*
                 * Check SIDL.  The corresponding bytes of SIDL are full if
                 * bit 7 (ILF or ILF1) are full in SSTAT0 and SSTAT2.  Do the
                 * least signicant byte first.
                 */
                if (regs->siop_sstat0 & SIOP_SSTAT0_ILF)
                        *cp++ = regs->siop_sidl0;
                if (regs->siop_sstat2 & SIOP_SSTAT2_ILF1)
                        *cp++ = regs->siop_sidl1;

                /*
                 * Drain the SCSI FIFO.  Put the FIFO into test mode and
                 * drain the data.  Test mode is entered by asserting STR
                 * (SCSI Test Read:  bit 6 of STEST3).  The FIFO bytes are
                 * read through SODL until there are no more to be read,
                 * which is determined from bits 7-4 of SSTAT1.  SODL is
                 * read 16 bits at a time if the transfer was wide,
                 * determined by EWS (Enable Wide SCSI:  bit 3 of SCNTL3).
                 */

                /* determine width of transfer */
                wide = (regs->siop_scntl3 & SIOP_SCNTL3_EWS) ? TRUE : FALSE;

                /* turn on SCSI Test Read */
                regs->siop_stest3 |=  SIOP_STEST3_STR;

                for (i = 0; regs->siop_sstat1 & SIOP_SSTAT1_FF_MASK; i++) {

                        assert(i < SIOP_FF_MAX);

                        if (wide) {
                            unsigned short word;

                            word =
				*(volatile unsigned short *)&regs->siop_sodl0;

                            *cp++ = (word >> 0) & 0xff;
                            *cp++ = (word >> 8) & 0xff;
                        } else {
                            *cp++ = regs->siop_sodl0;
			}
                }

                /* turn off SCSI Test Read */
                regs->siop_stest3 &= ~SIOP_STEST3_STR;

                /*
                 * Clear SCSI FIFO, swide, dfifo, etc. so they don't
                 * interfere with a subsequent CHMOV instruction.
                 */

                /*
                 * Assert CSF (clear SCSI FIFO); this bit also clears
                 * sidl, sodl, and sodr.  See page 4-48 of the NCR 53C720
                 * SCSI I/O Processor Data Manual.  It also clears
                 * swide per page 8-18 of the NCR 53C8XX Family
                 * PCI-SCSI I/O Processors Programming Guide.
                 */
                /* clear intermediate registers */
                regs->siop_stest3 |= SIOP_STEST3_CSF;
                regs->siop_dfifo   = 0;                 /* clear dfifo */
                regs->siop_ctest3 |= SIOP_CTEST3_CLF;   /* clear DMA FIFO */
                regs->siop_stest2 |= SIOP_STEST2_ROF;   /* clear sync offset */

                /* give "next_addr" to the 720 */
                shdata->next_addr = (unsigned long)kvtophys((vm_offset_t)cp);

                SLOG(SLOG_HIGH,
		  ("      siop_phase_mismatch: DATA_IN, next_addr=0x%x\n",
			shdata->next_addr));
	} else {

		/* DATA_OUT */
                SLOG(SLOG_MEDIUM, ("    siop_phase_mismatch: DATA_OUT\n"));

                /*
                 * For a transfer to the target, determine how many bytes
                 * were sent and compute the next_addr value accordingly.
                 *
                 * This value is:
                 *
                 *      next_addr =  DNAD - (DFIFO - DBC) - SODL - SODR
                 *
                 * where:
                 *      DNAD    The 720 DMA Next Data Address register;
                 *              contains the next address to read had the
                 *              transfer not been interrupted.
                 *
                 *      DFIFO   [0..63] 720 DMA FIFO bytes transferred from
		 *		the chip DMA core to the SCSI core.  DFIFO
		 *		is an 8-bit register; the lower 7 bits
                 *              indicate the DMA FIFO byte count.
                 *
                 *      DBC     The 720 dbc register contains the number of
                 *              bytes left to transfer from host memory.
		 *		This counter is decremented as data is moved
		 *		from host memory into the DMA FIFO.  DBC is
		 *		a 24-bit 720 register directly useful.  No
		 *		off-by-one adjustments are needed.
                 *
                 *      SODL    [0..2 bytes] 720 uses to write to a 16-bit
                 *              SCSI device.  SODL contents are indicated by
                 *              SSTAT0 bit 5 (OLF) and SSTAT2 bit 5 (OLF1).
                 *
                 *      SODR    [0..2 bytes] another 720 register for
                 *              synchronous data transfers.  SODR contents
                 *              are indicated by SSTAT0 bit 6 (ORF) and SSTAT2
                 *              bit 6 (ORF1).
                 */

		/*
		 * Give next_addr to 720.  Start with the dnad register.
		 */
		shdata->next_addr  = regs->siop_dnad;

		/*
		 * Adjust next_addr by the number of bytes in the DMA FIFO
		 */
		{
		    int	dma_fifo_count;

		    /*
		     * Determine the number of bytes in the DMA FIFO.
		     *
		     * The DMA FIFO register is poorly documented in the
		     * NCR data manuals.  According to Linda Miquelon,
		     * Applications Engineer at NCR's Microelectronics
		     * Division <LMIQUELO@cosmpdaero.ftcollinsco.NCR.COM>,
		     * when an instruction is loaded into the chip, the DMA
		     * FIFO register is loaded with the low order bits from
		     * the DBC register.  This counter will count down when
		     * a byte crosses between the DMA and SCSI portions of
		     * the chip, including when bytes move into the SODL
		     * and SODR registers.  Thanks, Linda!
		     */
		    dma_fifo_count = (regs->siop_dfifo & SIOP_DFIFO_MASK) -
				     (regs->siop_dbc0  & SIOP_DFIFO_MASK);

		    /*
		     * Because the two registers decrement independently
		     * it is possible to get a negative value.  In that
		     * case just add (instead of subtracting the two's
		     * complement).
		     */
		    if (dma_fifo_count < 0)
			    shdata->next_addr += dma_fifo_count;
		    else
		            shdata->next_addr -= dma_fifo_count;
		}

		/*
		 * Adjust next_addr by the number of bytes in SODL
		 */
		shdata->next_addr -=
			  ((regs->siop_sstat0 & SIOP_SSTAT0_OLF)  != 0) ? 1 : 0
			+ ((regs->siop_sstat2 & SIOP_SSTAT2_OLF1) != 0) ? 1 : 0;

		/*
		 * Adjust next_addr by the number of bytes in SODR
		 */
		shdata->next_addr -=
			  ((regs->siop_sstat0 & SIOP_SSTAT0_ORF)  != 0) ? 1 : 0
			+ ((regs->siop_sstat2 & SIOP_SSTAT2_ORF1) != 0) ? 1 : 0;

                /*
                 * Clear dfifo, sodl, sodr, synchronous offset so they
                 * don't interfere with a subsequent CHMOV instruction.
                 */

                /*
                 * Assert CSF (clear SCSI FIFO); this bit also clears
                 * sidl, sodl, and sodr.  See page 4-48 of the NCR 53C720
                 * SCSI I/O Processor Data Manual.  It also clears
                 * swide per page 8-18 of the NCR 53C8XX Family
                 * PCI-SCSI I/O Processors Programming Guide.
                 */
                /* clear intermediate registers */
                regs->siop_stest3 |= SIOP_STEST3_CSF;
                /* clear synch offset ("ack behind") */
                regs->siop_stest2 |= SIOP_STEST2_ROF;   /* clear sync offset */
                regs->siop_ctest3 |= SIOP_CTEST3_CLF;   /* clear DMA FIFO */

		/*
		 * Flush the SCSI daughter board DMA read FIFO
		 */
		outw(SDB_DMA_FLUSH_RD, 0);

                SLOG(SLOG_HIGH,
		  ("      siop_phase_mismatch: DATA_OUT, next_addr=0x%x\n",
			shdata->next_addr));
	}

	/*
	 * Phase mismatch can occur in msg_out during synch and wide
	 * negotiation when the target forces a phase change to msg_in
	 * to reject the message.
	 */
	if (regs->siop_dsp == SCRIPT_MSG_SENT(siop->script_phys)) {

		SLOG(SLOG_MEDIUM,
	    ("    siop_phase_mismatch: Phase Mismatch during MSG_OUT phase\n"));

		shdata->next_addr = 0;

		/*
		 * Restart the siop from the current address
		 */
		regs->siop_dsp = regs->siop_dsp;
	} else {
		/*
		 * Restart the siop
		 */
		regs->siop_dsp = SCRIPT_MAIN(siop->script_phys);
	}

        SLOG_EXIT(("siop_phase_mismatch: exit\n"));
}


/*
 * Debugger for Single-Step interrupts
 */
boolean_t
siop_debugger(siop_softc_t siop, int istat, int sist0, int sist1, int dstat)
{
        SLOG_ENTRY(25, ("siop_debugger: enter, siop=0x%x, istat=0x%x, sist0=0x%x, sist1=0x%x, dstat=0x%x\n",
                siop, istat, sist0, sist1, dstat));

        /*
         * We currently do not support single-step debugging, but
         * this routine is provided as a hook to add the support.
         * Instead, we just reset when we see this interrupt.
         */

        assert(0);                      /* Unexpected Condition */

        /*
         * Reset the siop to start all over
         */
        printf("siop_debugger: Attempting recovery...\n");
        (void)siop_reset(siop);

        SLOG_EXIT(("siop_debugger: exit\n"));

        return TRUE;
}


/*
 * The SCSI bus was reset
 */
static void
siop_was_reset(siop_softc_t siop)
{
    target_info_t *tgt;
    int i, lun;
    struct cntrlr_intf_tag * cntrlr_tag_p, *cntrlr_tag_p0;

    SLOG_ENTRY(26, ("siop_was_reset: enter, siop=0x%x\n", siop));

    /* siop_reset() clears the cntrlr_q */
    /* remove all request tables allocated in CLRAM */
    for (i = 0; i < MAX_REQ_TBLS_CLRAM; i++)
        req_tbl_clram_image[i] = NULL;

    /* remove all cntrlr_intf_tag from active_tag_q to free_tag_q,
       place cntrlr_intf_tag in request_q, and empty scheduler_q */
    for (i = 0; i < MAX_SCSI_TARGETS; i++) {
        for (lun = 0; lun < MAX_LUNS; lun++) {
	    if (!(tgt = siop->sc->target[i][lun])) /* check for valid target */
	        continue;
	    /* put all request of this target in the request_q */
	    for (cntrlr_tag_p =
		 (struct cntrlr_intf_tag *)queue_last(&tgt->active_tag_q); 
	         !queue_end((&tgt->active_tag_q),
		 (queue_entry_t)(cntrlr_tag_p));) {
	        cntrlr_tag_p0 = cntrlr_tag_p; /* queue_enter(...) changes the
					         tag.links */
	        cntrlr_tag_p = (struct cntrlr_intf_tag *)
		    queue_prev(&(cntrlr_tag_p)->tag.links);
                cntrlr_tag_p0->sch_links.next = NULL; /* remove from
							 scheduler_q */
                /* move from active_q to tail of free_q */
                queue_remove(&tgt->active_tag_q, cntrlr_tag_p0,
                         struct cntrlr_intf_tag *, tag.links);
                queue_enter(&tgt->free_tag_q, cntrlr_tag_p0,
                        struct cntrlr_intf_tag *, tag.links);
                /* move to head of request_q */
                req_q_enter_first((tag_info_t *)&cntrlr_tag_p0->tag, tgt);
#ifdef	SCSI_STATISTICS
                tgt->statistics.chain_depth++;
#endif	SCSI_STATISTICS
            } /* for (cntrlr_tag_p = (struct cntrlr_intf_tag.... */

#if	MACH_ASSERT
            queue_iterate(&tgt->free_tag_q, cntrlr_tag_p,
                      struct cntrlr_intf_tag *, tag.links) {
	        assert(cntrlr_tag_p->sch_links.next == NULL);
            }
#endif	MACH_ASSERT

	    queue_init(&tgt->active_tag_q);
	}
    }
    queue_init(&siop->scheduler_q);
    siop->wd.nactive = 0;

    siop->shrd_locs->stat.byte.halt_reason = 0;
    siop->regs->siop_dsp = siop->regs->siop_dsp; /* Restart the siop SCRIPT */

    printf("siop%d: (%d) bus reset", siop->unit, ++siop->wd.reset_count);
    delay(scsi_delay_after_reset); /* some targets take long to reset */

    assert(siop->sc);
    scsi_bus_was_reset(siop->sc);
    SLOG_EXIT(("siop_was_reset: exit\n"));
}


/*
 * Examine target status code and dispatch to the status handler
 */
static void
siop_status(siop_softc_t siop, unsigned char *status_p)
{
        scsi2_status_byte_t     *status = (scsi2_status_byte_t *)status_p;
	target_info_t		*tgt;

        SLOG_ENTRY(27,
                ("siop_status: enter, siop=0x%x, *status=0x%x\n",
                        siop, status));

	/* Read target_id from the request table in clram and use it to tgt */
/*
	tgt = siop->sc->target[((struct request_table *)
		(siop->req_tbl_ptr_clram +
		 (siop->shrd_locs->req_tbl_clram_ptr -
		  siop->req_tbl_ptr_clram_phys)))->bus_conf.dest_id];
*/
	tgt = (get_cntrlr_intf_tag(siop,
		siop->shrd_locs->req_tbl_clram_ptr))->tag.tgt;

	scsi_error(tgt, SCSI_ERR_STATUS, status->bits, 0);

        switch (status->st.scsi_status_code) {

	  case SCSI_ST_CHECK_CONDITION:
                        siop_chk_cond(siop);
                        break;

		      case SCSI_ST_BUSY:
                        siop_tgt_busy(siop);
                        break;

		      case SCSI_ST2_QUEUE_FULL:
                        siop_tgt_q_full(siop);
                        break;

		      default:
                        printf("siop_status: Unexpected SCSI Status: 0x%02x\n",
                                status->bits);
                        assert(0);      /* Unexpected Condition */
                        /*
                         * Reset to start all over
                         */
                        printf("siop_status: Attempting recovery...\n");
                        (void)siop_reset(siop);
                        break;
		    }
        SLOG_EXIT(("siop_status: exit\n"));
}

/*
 * Error handler for siop
 */
static void
siop_error(siop_softc_t siop)
{
    SLOG_ENTRY(28, ("siop_error: enter, siop=0x%x\n", siop));

    printf("siop_error: Attempting recovery...\n");

    (void)siop_reset(siop);
    /* upon unmasking of interrupt from controller board when we exit the
       interrupt handler, siop_was_reset() is called */

    SLOG_EXIT(("siop_error: exit\n"));
}


static int
siop_wait(siop_softc_t siop)
{
    int			timeo;
    unsigned char	temp; /* holds values read from SIOP regs */

    SLOG_ENTRY(1, ("siop_wait: enter, unit=%x\n", siop->unit));

    for (timeo = 100000; timeo; timeo--) {
	/* when SIP or DIP bit in ISTAT of SIOP is set then
	   SIOP has halted. CMP, SEL, and RSL bits in SISTO are
	   masked, GEN and HTH bits in SIST1 should never be
	   set because these timers are not used; 
           SIP should be set only due to a select timeout or a
	   SCSI bus reset, otherwise a fatal error has occured;
	   DIP should be set only due to single step or
	   INT instruction, otherwise a fatal error has occured */

	/* 4 possibiities: (1) SIOP goes in wait_reselect state because it
	   has no work to do, (2) SIOP halts with a halt_reason, (3) SIOP's
	   select timer expires, or SCSI bus was reset, (4) SIOP hits a debug
	   msg */

	if (siop->shrd_locs->stat.byte.no_work_to_do) {
	    SLOG(SLOG_MEDIUM, (
		       "    siop_wait: no_work_to_do is set by SIOP\n"));
	    delay(10); /* wait for SIOP to set INTFLY bit in ISTAT of 53C720 */

	    /* 3 possibilities: (1) Host signalled, thru SIGP, SIOP to start 
	       processing; SIOP found that there was no work to do,
	       (2) status_buf = 0 => target responded and 
	       command is complete, (3) status_buf != 0 => target responded
	       with status and then disconnect, target will reselect later;
	       SIOP goes to schedular module and then finds no work to do
	       and ends up in wait_reselect;
	       Possibilities (1) and (2) are the same for all purposes here */
	    if (siop->shrd_locs->stat.byte.status_buf) {
		/* target device will reselect later; when target completes
		   command, either status_buf should  be zero (command complete
		   with good status) OR halt_reason should be non-zero 
		   (command complete with not good status); NOTE that host
		   should not change status_buf or halt_reason because SIOP
		   may also change them, thus resulting in a race condition */
		SLOG(SLOG_MEDIUM, ("    siop_wait: target will reselect later\n"));
		{
		    /* wait till target reselects and completes cmd */
		    int time_res;
		    for (time_res = 100000; time_res && 
			 siop->shrd_locs->stat.byte.status_buf && 
			 !siop->shrd_locs->stat.byte.halt_reason; time_res--) {
			/* check for a debug message */
#if	MACH_ASSERT
			acquire_hdw_sem(siop);
			if (siop->regs->siop_istat & SIOP_ISTAT_DIP) {
			    /* SIOP does not write into halt_reason before INT
			       for debug msg but first writes into halt_reason
			       followed by INT for normal INT; NOTE
			       that check SIOP_ISTAT_DIP followed by 
			       halt_reason and not the other way around */
			    if (!siop->shrd_locs->stat.byte.halt_reason) {
				siop_clear_dma_int(siop->unit);
				siop->regs->siop_istat |= SIOP_ISTAT_INTF;
				temp = siop->regs->siop_dstat;  /* clear int */
				siop_debug_msg(siop); /* print debug message */
			    }
			}
			release_hdw_sem(siop);
#endif	MACH_ASSERT
			delay(10); /* wait for SIOP to execute a little bit */
		    }
                    acquire_hdw_sem(siop);
		    siop_clear_dma_int(siop->unit);
                    siop->regs->siop_istat |= SIOP_ISTAT_INTF;
                    release_hdw_sem(siop);
		    /* 3 possibilites: (1) reselection did not happen =>
		       no_work_to_do = 1 and status_buf != 0, (2) 
		       reselection happened, target responded with good status
		       and command completed => no_work_to_do = 1 and 
		       status_buf = 0, (3) reselection happened, target
		       responded with bad status, command completed, and SIOP
		       halted => no_work_to_do = 0 and halt_reason != 0 and 
		       status_buf != 0 */
		    if (!time_res) {
		        SLOG(SLOG_HIGH, ("      siop_wait: BUG, target on SIOP%d bus did not reselect\n", siop->unit));
		    }
		    else if (!siop->shrd_locs->stat.byte.status_buf) {
                        SLOG(SLOG_MEDIUM, ("    siop_wait: target for SIOP%d successfully reselected\n", siop->unit));
		    }
		    else if (siop->shrd_locs->stat.byte.halt_reason) {
                        SLOG(SLOG_MEDIUM, ("    siop_wait: target reselected and SIOP%d halted\n", siop->unit));
                        siop->shrd_locs->stat.byte.no_work_to_do = 0;
			continue; /* clear halt_reason INT */
		    }
		    else {
			assert(0); /* should not happen */
		    }
		    SLOG_EXIT(("siop_wait: exit, no_work_to_do with reselection, return=1\n"));
		    return 1;
		}
	    }	/* if (siop->shrd_locs->stat.byte.status_buf) */
	    /* else !siop->shrd_locs->stat.byte.status_buf */
	    acquire_hdw_sem(siop);
	    temp = siop->regs->siop_istat;
	    assert(temp & SIOP_ISTAT_INTF); /* no_work_to_do==1 => INTF==1 */
	    /* no_work_to_do == 1 => SIP | DIP == 0 */
            assert(!(temp & (SIOP_ISTAT_SIP | SIOP_ISTAT_DIP)));
            assert(!(siop->shrd_locs->stat.byte.halt_reason || 
		     siop->shrd_locs->stat.byte.status_buf ||
		     siop->shrd_locs->stat.byte.msg_in_count));
	    siop_clear_dma_int(siop->unit);
            siop->regs->siop_istat |= SIOP_ISTAT_INTF; /* clear INTF bit */
            release_hdw_sem(siop);
	    SLOG_EXIT(("siop_wait: exit, no_work_to_do, return=1\n"));
            return 1;
	} /* if (siop->shrd_locs->stat.byte.no_work_to_do) */

	if (siop->shrd_locs->stat.byte.halt_reason) {
            delay(10); /* wait for SIOP to write to hdw after setting
                          shared data */
	    acquire_hdw_sem(siop);
	    temp = siop->regs->siop_istat;
            assert(!(temp & SIOP_ISTAT_INTF)); /* halt_reason==1 => INTF==0 */
	    assert(!(temp & SIOP_ISTAT_SIP)); /* halt_reason==1 => SIP==0 */
	    assert(temp & SIOP_ISTAT_DIP); /* halt_reason==1 => DIP==1 */

	    siop_clear_dma_int(siop->unit);
	    temp = siop->regs->siop_dstat;  /* clear int in DIP */

            /* all bits in DSTAT should be clear except SIR and maybe DFE */
            assert(!(temp & ~(SIOP_DSTAT_SIR | SIOP_DSTAT_DFE)));
	    assert(!siop->shrd_locs->stat.byte.no_work_to_do &&
		   (((siop->shrd_locs->stat.byte.halt_reason & STAT_NOT_GOOD)
		     && siop->shrd_locs->stat.byte.status_buf) ||
		    (!(siop->shrd_locs->stat.byte.halt_reason & STAT_NOT_GOOD)
		     && !siop->shrd_locs->stat.byte.status_buf)) &&
		   !siop->shrd_locs->stat.byte.msg_in_count);
            release_hdw_sem(siop);
            SLOG_EXIT(("siop_wait: exit, halt_reason, return=1\n"));
	    return 1;
	}	/* if (siop->shrd_locs->stat.byte.halt_reason) */

	acquire_hdw_sem(siop);
        /* check for select timeout or reset */
	if ((temp = siop->regs->siop_istat) & SIOP_ISTAT_SIP) {
	    unsigned short sist;

	    assert(!(temp & SIOP_ISTAT_INTF)); /* SIP==1 => INTF==0 */
            assert(!(temp & SIOP_ISTAT_DIP)); /* SIP==1 => DIP==0 */
	    siop_clear_dma_int(siop->unit);
	    sist = siop->regs->siop_sist;	/* clear int */
	    delay(1);				/* BCLK delay */
	    temp = sist & 0xff;
	    if (temp & SIOP_SIST0_RST) {
		/* SCSI bus was reset */
                siop->shrd_locs->stat.byte.halt_reason = SIOP_RESET;
	        release_hdw_sem(siop);
	        SLOG_EXIT(("siop_wait: exit, SCSI bus reset occured, return=1\n"));
	        return 1;
	    }
	    /* all bits except CMP, SEL, or RSL in SISTO should be clear */
	    assert(!(temp & ~(SIOP_SIST0_CMP|SIOP_SIST0_SEL|SIOP_SIST0_RSL)));
            temp = (sist >> 8) & 0xff;
	    /*  STO should be set, GEN and HTH should be clear, other bits in
	        SIST1 are reserved and therefore don't care */
	    assert((!(temp & (SIOP_SIST1_GEN | SIOP_SIST1_HTH))) && 
		   (temp & SIOP_SIST1_STO));
            siop->shrd_locs->stat.byte.halt_reason = SELECT_TIMEOUT;
	    release_hdw_sem(siop);
	    SLOG_EXIT(("siop_wait: exit, select timeout occured, return=1\n"));
	    return 1;
	}   /* if ((temp = siop->regs->siop_istat) & SIOP_ISTAT_SIP) */

#if	MACH_ASSERT
        /* check for debug msg */
        if ((temp = siop->regs->siop_istat) & SIOP_ISTAT_DIP) {
	    /* potential problem here; halt_reason could get set since last
	       checked; therefore if non-zero then it is not a debug msg */
	    if (siop->shrd_locs->stat.byte.halt_reason) {
		release_hdw_sem(siop);
		continue; /* go to beginning of for loop */
	    }

	    /* there is a possibility that the SIOP enqueues in the
	       req_done_q, does intfly, and then int DEBUG_IO_COMPLETE_EXIT;
	       here we don't care about the intfly */

            assert(!(temp & SIOP_ISTAT_SIP)); /* DIP==1 => SIP==0 */
	    siop_clear_dma_int(siop->unit);
            siop->regs->siop_istat |= SIOP_ISTAT_INTF; /* clear INTF bit */
            temp = siop->regs->siop_dstat;  /* clear int in DIP */

            /* all bits in DSTAT should be clear except SIR and maybe DFE */
            assert(!(temp & ~(SIOP_DSTAT_SIR | SIOP_DSTAT_DFE)));
            assert(!siop->shrd_locs->stat.byte.no_work_to_do &&
                   !siop->shrd_locs->stat.byte.halt_reason &&
                   !siop->shrd_locs->stat.byte.msg_in_count);

	    siop->shrd_locs->stat.byte.halt_reason = DEBUG_MSG;
            release_hdw_sem(siop);
            SLOG_EXIT(("siop_wait: exit, debug msg occured, return=1\n"));
            return 1;
        }   /* if ((temp = siop->regs->siop_istat) & SIOP_ISTAT_DIP) */
#endif	MACH_ASSERT

	release_hdw_sem(siop);
	delay (10); /* wait for SIOP to execute more before checking again */
    } 	/* for (timeo = 100000; timeo; timeo--) */

    SLOG_EXIT(("siop_wait: exit, timeout occured, return=0\n"));
    return 0; /* timed out waiting for SIOP */
}


/*
 * Acquire hardware semaphore to access SIOP regs
 */
static void
acquire_hdw_sem(siop_softc_t siop)
{
    int			timeSem;

    SLOG_ENTRY(2, ("acquire_hdw_sem: enter, unit=%x\n", siop->unit));

    assert(siop->unit < NSIOP);

    if (siop->hw_sem_count++) {
	SLOG_EXIT(("acquire_hdw_sem: already acquired count=%d, SIOP%d exit\n",
		   siop->hw_sem_count, siop->unit));
	return;
    }

    for (timeSem = 1000000; timeSem; timeSem--) {
	switch (siop->unit) {
	  case 0:
	    if (inw(SDB_SCSI0_SEMAPHORE) & SDB_SCSI0_ACCESS) {

                SLOG_EXIT(("acquire_hdw_sem: just acquired count=%d, SIOP%d exit\n", siop->hw_sem_count, siop->unit));
		return;
	    }
	    break;
	  case 1:
            if (inw(SDB_SCSI1_SEMAPHORE) & SDB_SCSI1_ACCESS) {

                SLOG_EXIT(("acquire_hdw_sem: just acquired count=%d, SIOP%d exit\n", siop->hw_sem_count, siop->unit));
		return;
            }
	    break;
	  default:
	    panic("acquire_hdw_sem: Invalid unit number=%d\n", siop->unit);
	    break;
	}
	delay(1); /* delay in microseconds; NOTE: this delay is a spin lock */
    }
    panic("acquire_hdw_sem: HARDWARE BUG, cannot acquire sem\n");
}


/*
 * Release hardware semaphore
 */
static void 
release_hdw_sem(siop_softc_t siop)
{
    unsigned short	hw_sem;

    SLOG_ENTRY(4, ("release_hdw_sem: enter, unit=%x\n", siop->unit));

    assert(siop->unit < NSIOP);

    assert(siop->hw_sem_count);
    if (--siop->hw_sem_count) {
        SLOG_EXIT(("release_hdw_sem: not released count=%d, SIOP%d exit\n",
                   siop->hw_sem_count, siop->unit));
        return;
    }

    switch (siop->unit) {
	case 0:
		outw(SDB_SCSI0_SEMAPHORE, SDB_SCSI0_ACCESS);
		break;
	case 1:
		outw(SDB_SCSI1_SEMAPHORE, SDB_SCSI1_ACCESS);
		break;
	default:
		panic("release_hdw_sem: Invalid unit number\n");
		break;
    }
    SLOG_EXIT(("release_hdw_sem: released count=%d, SIOP%d exit\n",
               siop->hw_sem_count, siop->unit));
}


int
siop_tag_q_init(target_info_t *tgt, int depth)
{
    int bytes;
    int i;

    SLOG_ENTRY(6, ("siop_tag_q_init: enter, masterno=%d, tgt=%x, depth=%d\n", 
		   tgt->masterno, tgt, depth));

    /* free q_tags and command buffers associated with this target */
    if (tgt->tag_q) {
        assert(tgt->tag_q_depth);
	bytes = (sizeof(struct cntrlr_intf_tag) * tgt->tag_q_depth) +
	    (CMD_BUF_SIZE * tgt->tag_q_depth) + CMD_BUF_SIZE;
        kmem_free(kernel_map, tgt->tag_q, bytes);
    }

    queue_init(&tgt->free_tag_q);
    queue_init(&tgt->active_tag_q);
    tgt->tag_q = 0;
    tgt->tag_q_depth = depth;
    tgt->cmd_ptr = 0;
    
    /* allocate q_tags and command buffers associated with this target */
    if (tgt->tag_q_depth) {
	struct cntrlr_intf_tag *ptr;

        bytes = (sizeof(struct cntrlr_intf_tag) * tgt->tag_q_depth) +
            (CMD_BUF_SIZE * tgt->tag_q_depth) + CMD_BUF_SIZE;

        if (kmem_alloc_wired(kernel_map, &tgt->tag_q, bytes) != KERN_SUCCESS) {
	    tgt->tag_q_depth = 0;
	    tgt->tag_q       = NULL;
	    return 0;
	}

        ptr = (struct cntrlr_intf_tag *)(tgt->tag_q);
        bzero((unsigned char *)ptr, bytes);
        tgt->cmd_ptr = (unsigned char *)ptr;
        ptr = (struct cntrlr_intf_tag *)((unsigned char *)ptr + CMD_BUF_SIZE);
        for (i = 0; i < tgt->tag_q_depth; i++) {
	    assert(ptr < (struct cntrlr_intf_tag *)
		   ((unsigned char *)tgt->tag_q + bytes));
	    queue_enter(&tgt->free_tag_q, ptr,
			struct cntrlr_intf_tag *, tag.links);
	    ptr->tag.tag = i;
	    ptr->tag.tgt = tgt;
	    ptr->tag.cmd_ptr = (unsigned char *)(ptr) + 
		sizeof(struct cntrlr_intf_tag);
	    ptr = (struct cntrlr_intf_tag *) ((unsigned char *)ptr + 
			      sizeof(struct cntrlr_intf_tag) + CMD_BUF_SIZE);
	}
    }

    SLOG_EXIT(("siop_tag_q_init: exit, return 1\n"));
    return 1;
}

#if	MACH_ASSERT
static void
siop_debug_msg(siop_softc_t siop)
{
    /* routine assumes that the interrupt service routine will clear the 
       DIP bit in ISTAT before calling this routine because this routine
       starts the SIOP */

    unsigned long value;

    /* don't want to print SLOG_ENTRY message, declare _this_id explicitly
       SLOG_ENTRY(5, ("siop_debug_msg: enter, unit=%x\n", siop->unit)); */
    long  _this_id = S_BIT(5);

    /* assume SIOP has halted, therefore any register can be read without
       acquiring semaphore and reading the ISTAT reg */

    value = SCRIPT_ASSERT_VALUE(siop->regs->siop_dsps);
    assert(value); /* debug msg will never have a zero */

    if ((value >= DEBUG_MSG0_LOW) && (value <= DEBUG_MSG_LOW_LAST)) {
	value -= DEBUG_MSG0_LOW;
        SLOG(SLOG_LOW, ("  SIOP%d: %s\n", siop->unit, debug_msg_low_tbl[value]));
    }
    else if ((value >= DEBUG_MSG0_MED) && (value <= DEBUG_MSG_MED_LAST)) {
	value -= DEBUG_MSG0_MED;
	SLOG(SLOG_MEDIUM, (
		   "    SIOP%d: %s\n", siop->unit, debug_msg_med_tbl[value]));
    }
    else if ((value >= DEBUG_MSG0_HIGH) && (value <= DEBUG_MSG_HIGH_LAST)) {
	value -= DEBUG_MSG0_HIGH;
        SLOG(SLOG_HIGH, ("      SIOP%d: %s\n", siop->unit, debug_msg_med_tbl[value]));
    }
    else if ((value >= DEBUG_ASSERT0) && (value <= DEBUG_ASSERT_LAST)) {
	value -= DEBUG_ASSERT0;
	(*assert_func[value])(siop);
    }
    else {
	assert(0);
    }
    /* get SIOP started to execute the next instruction */
    siop->regs->siop_dsp = siop->regs->siop_dsp;

    /* don't want to print SLOG_EXIT
       SLOG_EXIT(("siop_debug_msg: exit\n")); */
}
#endif	MACH_ASSERT

int
siop_reset(siop)
	siop_softc_t	siop;
{
        extern unsigned long    siop_script_load();
	extern void             siop_script_patch();
	siop_regmap_t		*regs = siop->regs;
	unsigned char		siop_ccf, diff;
	int			i, xfer_period;
	char			my_id;

	SLOG_ENTRY(3, ("siop_reset: enter; unit=%x\n", siop->unit));

	/*
	 * Determine the SCSI bus ID
	 */
	my_id = scsi_initiator_id[siop->unit] & 0xf;
	siop->sc->initiator_id = my_id;

	/*
	 * Reset the chip
	 */
#ifdef	PARAGON860

	/* Hard reset 53C720 */
	siop_reset_chip(siop->unit);

	/*
	 * Setup the DMA Control register
	 *
	 * This must be the first register initialized.
	 *
	 * We want the Enable ACK (EA) bit set.  The spec says that
	 * setting this bit must be the first I/O performed to the
	 * NCR 53C720.  We also want the 53C720 to act as a 53C720.
	 */
	regs->siop_dcntl = SIOP_DCNTL_EA | SIOP_DCNTL_COM;

	/*
	 * Setup the Host Bus parity generation
	 *
	 * Note that this should be done before any
	 * of the 53C720 chip registers are read
	 */

        /*
         * Generate even host parity, disable cache-line bursts since SCRIPT
         * code will enable when request table is xferred from DPRAM to SRAM
         * and when it is in data_in or data_out phase
         */
        regs->siop_ctest0 = SIOP_CTEST0_GRP   |
                            SIOP_CTEST0_EHP   |
                            SIOP_CTEST0_C386E |
                            SIOP_CTEST0_CDIS;

	/*
	 * Enable host bus parity checking
	 */
	regs->siop_ctest4 = SIOP_CTEST4_EHPC;

	/*
	 * Host Bus parity generation is now setup.
	 *
	 * Use asserts to check the registers initialized so far.
	 */
	assert(regs->siop_dcntl == (SIOP_DCNTL_EA | SIOP_DCNTL_COM));
	assert(regs->siop_ctest0 == (SIOP_CTEST0_GRP   |
				     SIOP_CTEST0_EHP   |
				     SIOP_CTEST0_C386E |
				     SIOP_CTEST0_CDIS));
	assert(regs->siop_ctest4 == SIOP_CTEST4_EHPC);

	regs->siop_istat = 0;

#else	PARAGON860

	/* Soft reset */
	regs->siop_istat = SIOP_ISTAT_RST;
	delay(25);
	regs->siop_istat = 0;

	regs->siop_dcntl  = ?;
	regs->siop_ctest0 = ?;
	regs->siop_ctest4 = ?;

#endif	PARAGON860

	/*
	 * Save chip revision, it may be useful
	 */
	siop->chip_rev = (char)(SIOP_REV(regs->siop_ctest3));

	/*
	 * Setup various chip parameters
	 */

	/* Full arbitration, check SCSI parity */
	regs->siop_scntl0 = SIOP_ARB_FULL | SIOP_SCNTL0_EPC;
	assert(regs->siop_scntl0 == (SIOP_ARB_FULL | SIOP_SCNTL0_EPC));
	regs->siop_scntl1 = 0;
	assert(regs->siop_scntl1 == 0);
	regs->siop_scntl2 = 0;
	assert((regs->siop_scntl2 & SIOP_SCNTL2_MASK) == 0);

	/*
	 * During synchronous data transfers, we want to run the SCSI bus
	 * as fast as possible.  We will set up the chip for the best rate,
	 * but the synchronous negotiation must still be done with the target.
	 * Setting the synchronous offset to zero disables synchrounous
	 * transfers until the negotiation can be done.
	 *
	 * The synchronous transfer period (number of clocks per data byte)
	 * is the chip clock frequency divided by the SCSI bus REQ/ACK
	 * frequency.  We start with a REQ/ACK of 10 MB/sec and lower it
	 * until we find a transfer period that will match the chip clock
	 * frequency.  The minimum synchronous transfer period for the
	 * NCR 53C720 (SIOP_MIN_XFERP) is four.
	 */
	siop->clock_mhz = siop_clock(siop->unit); /* chip clock freq in MHz */

	/* Start with 10 MB/sec and decrease until a working value is found */
	for (i = 10; i >= SIOP_MIN_XFERP; i--) {
		xfer_period = siop->clock_mhz / i;/* period in CLKS/BYTE    */

		if ((xfer_period >= SIOP_MIN_XFERP) &&
		    (xfer_period <= SIOP_MAX_XFERP))
			break;
	}

	/* Convert the calculated period into a meaningful chip value */
	xfer_period -= SIOP_MIN_XFERP;

	/* Select the conversion factor based on the chip clock frequency */
	if (siop->clock_mhz <= 25)
		siop_ccf = SIOP_CCF_25MHz;
	else if (siop->clock_mhz <= 37)
		siop_ccf = SIOP_CCF_37MHz;
	else if (siop->clock_mhz <= 50)
		siop_ccf = SIOP_CCF_50MHz;
	else if (siop->clock_mhz <= 66)
		siop_ccf = SIOP_CCF_66MHz;
	else
		panic("Unknown SCSI chip clock frequency");

	siop->siop_scntl3 = (siop_ccf << SIOP_SCF_SHIFT) | siop_ccf;
	regs->siop_scntl3 = siop->siop_scntl3;
	assert((regs->siop_scntl3 & SIOP_SCNTL3_MASK) == siop->siop_scntl3);
	siop->siop_sxfer  = (xfer_period << SIOP_TP_SHIFT)  |
		SIOP_ASYNC_OFFSET;	/* asynch for now */
	regs->siop_sxfer  = siop->siop_sxfer;
	assert((regs->siop_sxfer & SIOP_SXFER_MASK) == siop->siop_sxfer);

	/* Restore our ID */
	regs->siop_respid0 = ((1 << my_id) >> 0) & 0xff;
	assert(regs->siop_respid0 == (((1 << my_id) >> 0) & 0xff));
	regs->siop_respid1 = ((1 << my_id) >> 8) & 0xff;
	assert(regs->siop_respid1 == (((1 << my_id) >> 8) & 0xff));
	regs->siop_scid	   = (my_id & SIOP_SCID_ID_MASK) | SIOP_SCID_RRE;
	assert((regs->siop_scid & SIOP_SCID_MASK) ==
		((my_id & SIOP_SCID_ID_MASK) | SIOP_SCID_RRE));

#ifdef	PARAGON860

	/* Setup the General Purpose Register bits */
	/*
	 * The bits are used as follows:
	 *
	 *	0-2	inputs, not connected
	 *	3	output, Green LED, active low
	 *	4	output, Red LED, active low
	 */
	regs->siop_gpcntl = SIOP_GPCNTL_IO0|SIOP_GPCNTL_IO1|SIOP_GPCNTL_IO2;
	assert((regs->siop_gpcntl & SIOP_GPCNTL_MASK) ==
		(SIOP_GPCNTL_IO0 | SIOP_GPCNTL_IO1 | SIOP_GPCNTL_IO2));
	regs->siop_gpreg  = SIOP_GPREG_IO3;	/* Red on, Green off */
	assert((regs->siop_gpreg & (SIOP_GPREG_IO3 | SIOP_GPREG_IO4)) ==
		SIOP_GPREG_IO3);

	/* Assert FETCH/ pin for op-codes only */
	regs->siop_ctest3 = SIOP_CTEST3_FM;
	assert((regs->siop_ctest3 & (SIOP_CTEST3_FLF |
				     SIOP_CTEST3_CLF |
				     SIOP_CTEST3_FM  |
				     SIOP_CTEST3_SM)) == SIOP_CTEST3_FM);

	/* Perform 8 4-byte transfers per burst (32 bytes) */
	regs->siop_dmode = (SIOP_8_XFER_BURST << SIOP_BURST_SHIFT) |
			   SIOP_DMODE_PD; /* Drive FC0 low on op-code fetch */
	assert(regs->siop_dmode == ((SIOP_8_XFER_BURST << SIOP_BURST_SHIFT) |
			   SIOP_DMODE_PD));

	/* All SCRIPT memory accesses are far (for hardware semaphore) */
	regs->siop_macntl = SIOP_MACNTL_SCRIPT;
	assert((regs->siop_macntl & SIOP_MACNTL_MASK)==SIOP_MACNTL_SCRIPT);

#else	PARAGON860

	regs->siop_gpcntl = ?;
	regs->siop_gpreg  = ?;
	regs->siop_ctest3 = ?;
	regs->siop_dmode  = ?;
	regs->siop_macntl = ?;

#endif	PARAGON860

	regs->siop_ctest5 = 0;
	assert((regs->siop_ctest5 & SIOP_CTEST5_MASK) == 0);

	/* Zero DMA byte counter */
	regs->siop_dbc0 = 0;
	assert(regs->siop_dbc0 == 0);
	regs->siop_dbc1 = 0;
	assert(regs->siop_dbc1 == 0);
	regs->siop_dbc2 = 0;
	assert(regs->siop_dbc2 == 0);

	/* Clear Scratch Pad registers */
	regs->siop_scratcha = 0;
	assert(regs->siop_scratcha == 0);
	regs->siop_scratchb = 0;
	assert(regs->siop_scratchb == 0);

	/*
	 * Setup chip DMA interrupt masks
	 *
	 * All DMA interrupts are fatal.  Therefore, we
	 * want to enable all DMA interrupts to insure
	 * that the interrupt handler is invoked.
	 *
	 * Enable Host Bus Parity Error, Bus Fault, Abort,
	 * SCRIPT Single-Step Interrupt, SCRIPT Interrupt
	 * Instruction, Watchdog Timeout, and Illegal
	 * Instruction interrupts
	 */
	regs->siop_dien = SIOP_DIEN_HPED |	/* Fatal */
			  SIOP_DIEN_BF	 |	/* Fatal */
			  SIOP_DIEN_ABRT |	/* Fatal */
			  SIOP_DIEN_SSI  |	/* Fatal */
			  SIOP_DIEN_SIR	 |	/* Fatal */
			  SIOP_DIEN_WTD  |	/* Fatal */
			  SIOP_DIEN_IID;	/* Fatal */
	assert((regs->siop_dien & SIOP_DIEN_MASK) == (SIOP_DIEN_HPED |
						      SIOP_DIEN_BF   |
						      SIOP_DIEN_ABRT |
						      SIOP_DIEN_SSI  |
						      SIOP_DIEN_SIR  |
						      SIOP_DIEN_WTD  |
						      SIOP_DIEN_IID));

	/* Disable 53C720 chip watchdog timer */
	regs->siop_dwt = 0;
	assert(regs->siop_dwt == 0);

	/*
	 * Setup chip SCSI interrupt masks
	 *
	 * In initiator mode, only the M_A, SGE, UDC, RST, PAR, and
	 * STO interrupts are considered fatal.  Therefore, we want
	 * to enable these interrupts to insure that the interrupt
	 * handler is invoked.  The SCRIPTs will take care of the
	 * normal target behavior.  We enable only the interrupts
	 * that this layer must handle, namely:
	 *
	 *	Phase Mismatch, Gross Errors, Unexpected Disconnects,
	 *	SCSI Bus Resets, Parity Errors, Selection Timeouts,
	 *	General Purpose Timer, and Handshake-to-Handshake Timer
	 */
	regs->siop_sien0 = SIOP_SIEN0_M_A |	/* Fatal */
			   SIOP_SIEN0_SGE |	/* Fatal */
			   SIOP_SIEN0_UDC |	/* Fatal */
			   SIOP_SIEN0_RST |	/* Fatal */
			   SIOP_SIEN0_PAR;	/* Fatal */
	assert(regs->siop_sien0 == (SIOP_SIEN0_M_A |
				    SIOP_SIEN0_SGE |
				    SIOP_SIEN0_UDC |
				    SIOP_SIEN0_RST |
				    SIOP_SIEN0_PAR));
	regs->siop_sien1 = SIOP_SIEN1_STO |	/* Fatal */
			   SIOP_SIEN1_GEN |
			   SIOP_SIEN1_HTH;
	assert((regs->siop_sien1 & SIOP_SIEN1_MASK) == (SIOP_SIEN1_STO |
							SIOP_SIEN1_GEN |
							SIOP_SIEN1_HTH));

	/* Zero the Longitudinal Parity register */
	regs->siop_slpar = 0;
	assert(regs->siop_slpar == 0);

	/*
	 * Setup the Handshake to Handshake and Selection Timeout Periods.
	 * For some reason, the chip doesn't have a vaule for the 250 msec
	 * SCSI bus timeout value.  The next higher value is 409.6 msec.
	 * The Handshake timeout is disabled so the software watchdog timer
	 * can work.  The General Purpose timer is disabled.
	 */
	regs->siop_stime0 = (SIOP_TIMEOUT_DISABLE << SIOP_HTH_SHIFT) |
			    SIOP_TIMEOUT_409_6_ms;	/* selection timeout */
	assert(regs->siop_stime0 == ((SIOP_TIMEOUT_DISABLE << SIOP_HTH_SHIFT) |
				      SIOP_TIMEOUT_409_6_ms));
	regs->siop_stime1 = SIOP_TIMEOUT_DISABLE;
	assert((regs->siop_stime1 & SIOP_STIME1_MASK) == SIOP_TIMEOUT_DISABLE);

	/* Setup differential or single-ended mode of operation */
	diff = (siop_diff(siop->unit)) ? SIOP_STEST2_DIF : 0;
	regs->siop_stest2 = diff;
	assert(regs->siop_stest2 == diff);

	/*
	 * Set NCR TolerANT technology bit for higher bus noise margin,
	 * disable single-initiator response, and flush the chip FIFO.
	 */
	regs->siop_stest3 = SIOP_STEST3_TE | SIOP_STEST3_DSI | SIOP_STEST3_CSF;
	assert(regs->siop_stest3 == (SIOP_STEST3_TE | SIOP_STEST3_DSI));

	/*
	 * Reset the SCSI bus, the interrupt routine does the rest
	 */
	regs->siop_scntl1 = SIOP_SCNTL1_RST;
	assert(regs->siop_scntl1 == SIOP_SCNTL1_RST);
	delay(35);		/* 25 usec minimum hold time */
	regs->siop_scntl1 = 0;
	assert(regs->siop_scntl1 == 0);

	/*
	 * Load NCR 53C720 SCRIPTs
	 */
	siop->script = siop_script_load(siop->unit);

        /*
         * The siop_script_load() function returns a virtual address
         * but we don't need it.  Just convert it to physical to use
         * when loading the siop_dsp register.
         */
        siop->script_phys = (unsigned long)kvtophys((vm_offset_t)siop->script);

        /*
         * Patch NCR 53C720 SCRIPT
         */
        siop_script_patch(siop->unit);

	/*
	 * Initialize controller queues
	 */
	queue_init(&siop->scheduler_q);
	siop->cntrlr_q_index   = (unsigned char)0;
	siop->req_done_q_index = (unsigned char)0;

	/*
	 * Initialize shared locations
	 */
	sdbzero(siop->shrd_locs, sizeof(struct cntrlr_shrd_data));

	/*
	 * Start NCR 53C720 SCRIPTs
	 * Set regs->siop_dsp to SCRIPTs start address
	 */
        regs->siop_dsp = SCRIPT_INIT(siop->script_phys);
	delay(scsi_delay_after_reset); /* some targets take long to reset */

	SLOG_EXIT(("siop_reset: exit\n"));
	return 1; /* return value is not important */
}




/********************** functions used by debugger only *****************/
#if	MACH_ASSERT

void
addr(int unit)
{
    /* print addresses of all data structures used by the given channel */

    siop_softc_t	siop;
    int			i;

    if (unit >= NSIOP) {
        db_printf("Bad SIOP unit = %d\n", unit);
        return;
    }
    if (!(siop = siop_softc[unit])) {
        db_printf("Bad ptr to siop structure\n");
        return;
    }

    db_printf("\nvirtual, physical addresses of data structures used by unit %d\n", siop->unit);

    db_printf("\nvirtual, physical addresses of data structures in node memory");
    db_printf("\n   ");
    db_printf("softc	0x%x, 0x%x", 
	      siop, kvtophys((vm_offset_t)siop));

    db_printf("\nvirtual, physical addresses of data structures in dual-ported RAM");
    db_printf("\n   ");
    db_printf("shrd_locs	0x%x, 0x%x", siop->shrd_locs,
	      kvtophys((vm_offset_t)siop->shrd_locs));
    db_printf("\n   ");
    db_printf("shrd_locs->stat	0x%x, 0x%x",&siop->shrd_locs->stat,
              kvtophys((vm_offset_t)&siop->shrd_locs->stat));
    db_printf("\n   ");
    db_printf("shrd_locs->msg_in 0x%x, 0x%x", &siop->shrd_locs->msg_in,
              kvtophys((vm_offset_t)&siop->shrd_locs->msg_in));
    db_printf("\n   ");
    db_printf("shrd_locs->next_addr 0x%x, 0x%x", &siop->shrd_locs->next_addr,
              kvtophys((vm_offset_t)&siop->shrd_locs->next_addr));
    db_printf("\n   ");
    db_printf("shrd_locs->req_tbl_clram_ptr 0x%x, 0x%x", 
	      &siop->shrd_locs->req_tbl_clram_ptr,
              kvtophys((vm_offset_t)&siop->shrd_locs->req_tbl_clram_ptr));
    db_printf("\n   ");
    db_printf("shrd_locs->req_done_q 0x%x, 0x%x", siop->shrd_locs->req_done_q,
              kvtophys((vm_offset_t)siop->shrd_locs->req_done_q));
    for (i = 0; i < CNTRLR_Q_SIZE; i++) {
	db_printf("\n   ");
	db_printf("shrd_locs->req_tbl[%d] 0x%x, 0x%x", i, 
		  &siop->shrd_locs->req_tbl[i],
              kvtophys((vm_offset_t)&siop->shrd_locs->req_tbl[i]));
    }
    for (i = 0; i < CNTRLR_Q_SIZE; i++) {
        db_printf("\n   ");
        db_printf("shrd_locs->cntrlr_q[%d] 0x%x, 0x%x", i,
                  &siop->shrd_locs->cntrlr_q[i],
              kvtophys((vm_offset_t)&siop->shrd_locs->cntrlr_q[i]));
    }

    db_printf("\nvirtual, physical addresses of data structures in local RAM");
    db_printf("\n   ");
    db_printf("script = 0x%x, 0x%x", siop->script, siop->script_phys);
    db_printf("\n   virtual, physical addresses of variables used by script code");
    db_printf("\n      ");
    db_printf("SCRIPT_CNTRLR_Q_P 0x%x, 0x%x", 
	      (unsigned char *)phystokv(SCRIPT_CNTRLR_Q_P(siop->script)), 
	      (unsigned char *)(SCRIPT_CNTRLR_Q_P(siop->script)));
    db_printf("\n      ");
    db_printf("SCRIPT_REQ_DONE_PTR 0x%x, 0x%x",
              (unsigned char *)phystokv(SCRIPT_REQ_DONE_PTR(siop->script)),
              (unsigned char *)(SCRIPT_REQ_DONE_PTR(siop->script)));
    db_printf("\n      ");
    db_printf("SCRIPT_IGNORE_WIDE_FLAG 0x%x, 0x%x",
              (unsigned char *)phystokv(SCRIPT_IGNORE_WIDE_FLAG(siop->script)),
              (unsigned char *)(SCRIPT_IGNORE_WIDE_FLAG(siop->script)));
    db_printf("\n      ");
    db_printf("SCRIPT_COPY_BUF 0x%x, 0x%x",
              (unsigned char *)phystokv(SCRIPT_COPY_BUF(siop->script)),
              (unsigned char *)(SCRIPT_COPY_BUF(siop->script)));
    db_printf("\n   ");
    db_printf("rslct_tbl_ptr_clram = 0x%x, 0x%x", siop->rslct_tbl_ptr_clram,
	      kvtophys((vm_offset_t)(siop->rslct_tbl_ptr_clram)));
    db_printf("\n   ");
    db_printf("req_tbl_ptr_clram = 0x%x, 0x%x", siop->req_tbl_ptr_clram, 
	      siop->req_tbl_ptr_clram_phys);

    db_printf("\nvirtual, physical addresses of registers in 53C720");
    db_printf("\n   ");
    db_printf("regs = 0x%x, 0x%x", siop->regs,
              kvtophys((vm_offset_t)siop->regs));

    db_printf("\n");
}

void
shrd_locs(int unit)
{
    /* should be called from debugger after siop_probe() has initialized
       shrd_locs and unit elements in siop_softc_t structure */

    siop_softc_t        siop;

    if (unit >= NSIOP) {
        db_printf("Bad SIOP unit = %d\n", unit);
        return;
    }
    if (!(siop = siop_softc[unit])) {
        db_printf("Bad ptr to siop structure\n");
        return;
    }

    db_printf("\nShared locations for SIOP%d\n", siop->unit);
    
    db_printf("\n   ");
    db_printf("msg_in_count = %d ",
                  siop->shrd_locs->stat.byte.msg_in_count);
    db_printf("status_buf = 0x%x ",
                  siop->shrd_locs->stat.byte.status_buf);
    db_printf("halt_reason = %d ",
                  siop->shrd_locs->stat.byte.halt_reason);
    db_printf("no_work_to_do = 0x%x", 
		  siop->shrd_locs->stat.byte.no_work_to_do);
    db_printf("\n   ");
    if (siop->shrd_locs->stat.byte.msg_in_count) {
	int i;
	db_printf("msg_in bytes are:");
	for (i = 0; i < siop->shrd_locs->stat.byte.msg_in_count; i++)
	    db_printf(" 0x%x", siop->shrd_locs->msg_in.byte[i]);
	db_printf("\n   ");
    } 
    db_printf("next_addr = 0x%x",
	      siop->shrd_locs->next_addr);
    db_printf("\n   ");
    db_printf("virt, phys req_tbl_clram_ptr = 0x%x, 0x%x",
	      siop->req_tbl_ptr_clram + (siop->shrd_locs->req_tbl_clram_ptr - 
	 siop->req_tbl_ptr_clram_phys), siop->shrd_locs->req_tbl_clram_ptr);
    db_printf("\n   ");
    db_printf("addr of req_done_q=0x%x, req_tbl=0x%x, cntrlr_q=0x%x",
	      siop->shrd_locs->req_done_q, siop->shrd_locs->req_tbl,
	      siop->shrd_locs->cntrlr_q);
    db_printf("\n   ");
}

void
softc(int unit)
{
    siop_softc_t        siop;

    if (unit >= NSIOP) {
        db_printf("Bad SIOP unit = %d\n", unit);
        return;
    }
    if (!(siop = siop_softc[unit])) {
        db_printf("Bad ptr to siop structure\n");
        return;
    }

        db_printf("\nsiop_softc structure for SIOP%d @ 0x%x\n", siop->unit,siop);
        db_printf("\n   ");
        db_printf("virt, phys ptr to regs = 0x%x, 0x%x",
             siop->regs, kvtophys((vm_offset_t)
					  (siop->regs)));
        db_printf("\n   ");
        db_printf("state = 0x%x", siop->state);
        db_printf("\n   ");
        db_printf("chip_rev = %d", siop->chip_rev);
        db_printf("\n   ");
        db_printf("clock_mhz = %d", siop->clock_mhz);
        db_printf("\n   ");
        db_printf("siop_scntl3 = 0x%x", siop->siop_scntl3);
        db_printf("\n   ");
        db_printf("siop_sxfer = 0x%x", siop->siop_sxfer);
        db_printf("\n   ");
        db_printf("virt, phys ptr to script = 0x%x, 0x%x",
                  siop->script, siop->script_phys);
        db_printf("\n   ");
        db_printf("virt, phys ptr to shrd_locs = 0x%x, 0x%x",
		  siop->shrd_locs, 
		  kvtophys((vm_offset_t)(siop->shrd_locs)));
        db_printf("\n   ");
        db_printf("virt, phys ptr to rslct_tbl_ptr_clram = 0x%x, 0x%x",
                  siop->rslct_tbl_ptr_clram,
                  kvtophys((vm_offset_t)(siop->rslct_tbl_ptr_clram)));
        db_printf("\n   ");
        db_printf("virt, phys ptr to req_tbl_ptr_clram = 0x%x, 0x%x",
                  siop->req_tbl_ptr_clram, siop->req_tbl_ptr_clram_phys);
        db_printf("\n   ");
        db_printf("cntrlr_q_index = %d",
                  siop->cntrlr_q_index);
        db_printf("\n   ");
        db_printf("req_done_q_index = %d",
                  siop->req_done_q_index);
        db_printf("\n   ");
        db_printf("hw_sem_count = %d",
                  siop->hw_sem_count);
        db_printf("\n   ");
}

void
regs(int unit, int all_regs, int force)
{
	siop_softc_t	siop;
	siop_regmap_t	*regs;

	if (unit >= NSIOP) {
		db_printf("Bad SIOP unit = %d\n", unit);
		return;
	}
	if (!(siop = siop_softc[unit])) {
		db_printf("Bad ptr to siop structure\n");
		return;
	}

	regs = siop->regs;
	acquire_hdw_sem(siop);

        /* when SIP or DIP bit in ISTAT of SIOP is set then SIOP has halted */
	if (!force && !(regs->siop_istat & (SIOP_ISTAT_SIP | SIOP_ISTAT_DIP))) {

            db_printf("\nSIOP%d has not halted, can't read regs; istat=0x%x\n", 
		      siop->unit, regs->siop_istat);
	    release_hdw_sem(siop);
	    return;
	}

	if (all_regs) {
	    db_printf("NOTE: reading ALL regs may deassert SIOP interrupt to host\n");
	}
        db_printf("\nRegisters of SIOP%d\n", siop->unit);

        db_printf("\n   ");
        db_printf("scntl0=0x%0x         ", regs->siop_scntl0);
        db_printf("scntl1=0x%0x         ", regs->siop_scntl1);
        db_printf("scntl2=0x%0x         ", regs->siop_scntl2);
        db_printf("scntl3=0x%0x         ", regs->siop_scntl3);
        db_printf("\n   ");
        db_printf("scid=0x%0x           ", regs->siop_scid);
        db_printf("sxfer=0x%0x          ", regs->siop_sxfer);
        db_printf("sdid=0x%0x           ", regs->siop_sdid);
        db_printf("gpreg=0x%0x          ", regs->siop_gpreg);
        db_printf("\n   ");
        db_printf("sfbr=0x%0x           ", regs->siop_sfbr);
        db_printf("socl=0x%0x           ", regs->siop_socl);
        db_printf("ssid=0x%0x           ", regs->siop_ssid);
        db_printf("sbcl=0x%0x           ", regs->siop_sbcl);
        db_printf("\n   ");
        if (all_regs) {
            db_printf("NOTE: reading dstat will deassert INT to host");
	    db_printf("\n   ");
	    db_printf("dstat=0x%0x          ", regs->siop_dstat);
        }
	else {
	    db_printf("dstat=not_read      ");
	}
        db_printf("sstat0=0x%0x         ", regs->siop_sstat0);
        db_printf("sstat1=0x%0x         ", regs->siop_sstat1);
        db_printf("sstat2=0x%0x         ", regs->siop_sstat2);
        db_printf("\n   ");
        db_printf("dsa=0x%0x            ", regs->siop_dsa);
        db_printf("\n   ");
        db_printf("istat=0x%0x          ", regs->siop_istat);
        db_printf("rsrvd1=not_read     ");
        db_printf("rsrvd2=not_read     ");
        db_printf("rsrvd3=not_read");
        db_printf("\n   ");
        db_printf("ctest0=0x%0x         ", regs->siop_ctest0);
        db_printf("ctest1=0x%0x         ", regs->siop_ctest1);
        db_printf("ctest2=0x%0x         ", regs->siop_ctest2);
        db_printf("ctest3=0x%0x         ", regs->siop_ctest3);
        db_printf("\n   ");
        db_printf("temp=0x%0x           ", regs->siop_temp);
        db_printf("\n   ");
        db_printf("dfifo=0x%0x          ", regs->siop_dfifo);
        db_printf("ctest4=0x%0x         ", regs->siop_ctest4);
        db_printf("ctest5=0x%0x         ", regs->siop_ctest5);
        db_printf("ctest6=0x%0x         ", regs->siop_ctest6);
        db_printf("\n   ");
        db_printf("dbc0=0x%0x           ", regs->siop_dbc0);
        db_printf("dbc1=0x%0x           ", regs->siop_dbc1);
        db_printf("dbc2=0x%0x           ", regs->siop_dbc2);
        db_printf("dcmd=0x%0x           ", regs->siop_dcmd);
        db_printf("\n   ");
        db_printf("dnad=0x%0x           ", regs->siop_dnad);
        db_printf("\n   ");
        db_printf("dsp=0x%0x            ", regs->siop_dsp);
        db_printf("\n   ");
        db_printf("dsps=0x%0x           ", regs->siop_dsps);
        db_printf("\n   ");
        db_printf("scratcha=0x%0x       ", regs->siop_scratcha);
        db_printf("\n   ");
        db_printf("dmode=0x%0x          ", regs->siop_dmode);
        db_printf("dien=0x%0x           ", regs->siop_dien);
        db_printf("dwt=0x%0x            ", regs->siop_dwt);
        db_printf("dcntl=0x%0x          ", regs->siop_dcntl);
        db_printf("\n   ");
        db_printf("adder=0x%0x          ", regs->siop_adder);
        db_printf("\n   ");
        if (all_regs) {
            db_printf("NOTE: reading sist will deassert INT to host");
	    db_printf("\n   ");
	}
        db_printf("sien0=0x%0x          ", regs->siop_sien0);
        db_printf("sien1=0x%0x          ", regs->siop_sien1);
        if (all_regs) {
	    db_printf("sist=0x%0x          ", regs->siop_sist);
        }
	else {
	    db_printf("sist=not_read      ");
	}
        db_printf("\n   ");
        db_printf("slpar=0x%0x          ", regs->siop_slpar);
        db_printf("swide=0x%0x          ", regs->siop_swide);
        db_printf("macntl=0x%0x         ", regs->siop_macntl);
        db_printf("gpcntl=0x%0x         ", regs->siop_gpcntl);
        db_printf("\n   ");
        db_printf("stime0=0x%0x         ", regs->siop_stime0);
        db_printf("stime1=0x%0x         ", regs->siop_stime1);
        db_printf("respid0=0x%0x        ", regs->siop_respid0);
        db_printf("respid1=0x%0x        ", regs->siop_respid1);
        db_printf("\n   ");
        db_printf("stest0=0x%0x         ", regs->siop_stest0);
        db_printf("stest1=0x%0x         ", regs->siop_stest1);
        db_printf("stest2=0x%0x         ", regs->siop_stest2);
        db_printf("stest3=0x%0x         ", regs->siop_stest3);
        db_printf("\n   ");
        db_printf("sidl0=0x%0x           ", regs->siop_sidl0);
        db_printf("\n   ");
        db_printf("sidl1=0x%0x           ", regs->siop_sidl1);
        db_printf("                    ");
        db_printf("rsrvd=not_read  ");
        db_printf("\n   ");
        db_printf("sodl0=0x%0x           ", regs->siop_sodl0);
        db_printf("\n   ");
        db_printf("sodl1=0x%0x           ", regs->siop_sodl1);
        db_printf("                    ");
        db_printf("rsrvd=not_read");
        db_printf("\n   ");
        db_printf("sbdl0=0x%0x           ", regs->siop_sbdl0);
        db_printf("\n   ");
        db_printf("sbdl1=0x%0x           ", regs->siop_sbdl1);
        db_printf("                    ");
        db_printf("rsrvd=not_read  ");
        db_printf("\n   ");
        db_printf("scratchb=0x%0x       ", regs->siop_scratchb);
        db_printf("\n   ");

	release_hdw_sem(siop);
}


void
active_q(int unit, int target)
{
    siop_softc_t		siop;
    target_info_t		*tgt;
    struct cntrlr_intf_tag	*cntrlr_tag_p;
    int				i, lun;

    if (unit >= NSIOP) {
        db_printf("Bad SIOP unit = %d\n", unit);
        return;
    }
    if (!(siop = siop_softc[unit])) {
        db_printf("Bad ptr to siop structure\n");
        return;
    }
    if (target >= MAX_SCSI_TARGETS) {
        db_printf("Bad target = %d\n", target);
        return;
    }

    for (lun = 0; lun < MAX_LUNS; lun++) {

	if (!(tgt = siop->sc->target[target][lun]))
	    continue;

	/*
	 * Weird stuff for disk.  We're not supporting LUNs
	 * on disks yet, so each target/lun structure points
	 * to target/lun 0.
	 */
	if (lun != 0 && tgt == siop->sc->target[target][0])
	    continue;

        queue_iterate(&tgt->active_tag_q, cntrlr_tag_p,
		      struct cntrlr_intf_tag *, tag.links) {
            db_printf("\n\nu=%d,t=%d,l=%d,q=%d:\n", unit,target,lun,
		      cntrlr_tag_p->tag.tag);
            db_printf("   ");
            db_printf("&cntrlr_intf_tag=0x%x ", cntrlr_tag_p);
            db_printf("lun=0x%x ", cntrlr_tag_p->tag.lun);
	    db_printf("tag id=0x%x ", cntrlr_tag_p->tag.tag);
            db_printf("cmd ptr=0x%x ", cntrlr_tag_p->tag.cmd_ptr);
            db_printf("cmd=");
	    display_cmd(cntrlr_tag_p->tag.cmd_ptr);
            db_printf(": ");
	    for (i = 0; i < cntrlr_tag_p->tag.cmd_count; i++) {
	        db_printf(" 0x%x", cntrlr_tag_p->tag.cmd_ptr[i]);
	    }
	    if (cntrlr_tag_p->sch_links.next)
	        db_printf(" On scheduler queue");
            else
	        db_printf(" Scheduled: out of the scheduler queue");
        }
    }
}


void
free_q(int unit, int target)
{
    siop_softc_t                siop;
    target_info_t               *tgt;
    struct cntrlr_intf_tag      *cntrlr_tag_p;
    int                         i, lun;

    if (unit >= NSIOP) {
        db_printf("Bad SIOP unit = %d\n", unit);
        return;
    }
    if (!(siop = siop_softc[unit])) {
        db_printf("Bad ptr to siop structure\n");
        return;
    }
    if (target >= MAX_SCSI_TARGETS) {
        db_printf("Bad target = %d\n", target);
        return;
    }

    for (lun = 0; lun < MAX_LUNS; lun++) {

	if (!(tgt = siop->sc->target[target][lun]))
	    continue;

	/*
	 * Weird stuff for disk.  We're not supporting LUNs
	 * on disks yet, so each target/lun structure points
	 * to target/lun 0.
	 */
	if (lun != 0 && tgt == siop->sc->target[target][0])
	    continue;

        queue_iterate(&tgt->free_tag_q, cntrlr_tag_p, struct cntrlr_intf_tag *,
                      tag.links) {
            db_printf("   ");
            db_printf("&cntrlr_intf_tag=0x%x ", cntrlr_tag_p);
            db_printf("lun=0x%x ", cntrlr_tag_p->tag.lun);
            db_printf("tag id=0x%x ", cntrlr_tag_p->tag.tag);
            db_printf("cmd ptr=0x%x\n", cntrlr_tag_p->tag.cmd_ptr);
        }
    }
}

  
void
scheduler_q(int unit)
{
    siop_softc_t		siop;
    struct cntrlr_intf_tag	*cntrlr_tag_p;
    int				i;

    if (unit >= NSIOP) {
        db_printf("Bad SIOP unit = %d\n", unit);
        return;
    }
    if (!(siop = siop_softc[unit])) {
        db_printf("Bad ptr to siop structure\n");
        return;
    }

    queue_iterate(&siop->scheduler_q, cntrlr_tag_p, struct cntrlr_intf_tag *,
                      sch_links) {
        db_printf("\n   ");
        db_printf("&cntrlr_intf_tag=0x%x ", cntrlr_tag_p);
        db_printf("target id=0x%x ", cntrlr_tag_p->tag.tgt->target_id);
        db_printf("lun=0x%x ", cntrlr_tag_p->tag.lun);
        db_printf("tag id=0x%x ", cntrlr_tag_p->tag.tag);
        db_printf("cmd=");
	display_cmd(cntrlr_tag_p->tag.cmd_ptr);
        db_printf(": ");
	for (i = 0; i < cntrlr_tag_p->tag.cmd_count; i++) {
	    db_printf(" 0x%x", cntrlr_tag_p->tag.cmd_ptr[i]);
	}
    }
}

void
request_q(int unit, int target)
{
    siop_softc_t		siop;
    target_info_t		*tgt;
    io_req_t			ptr;
    int				lun;

    db_printf("\n");

    if (unit >= NSIOP) {
        db_printf("Bad SIOP unit = %d\n", unit);
        return;
    }
    if (!(siop = siop_softc[unit])) {
        db_printf("Bad ptr to siop structure\n");
        return;
    }
    if (target >= MAX_SCSI_TARGETS) {
        db_printf("Bad target = %d\n", target);
        return;
    }

    for (lun = 0; lun < MAX_LUNS; lun++) {

	if (!(tgt = siop->sc->target[target][lun]))
	    continue;

	/*
	 * Weird stuff for disk.  We're not supporting LUNs
	 * on disks yet, so each target/lun structure points
	 * to target/lun 0.
	 */
	if (lun != 0 && tgt == siop->sc->target[target][0])
	    continue;

        for (ptr = tgt->ior; ptr; ptr = tgt->ior->io_next) {
            db_printf("\n\nu=%d,t=%d,l=%d:\n", unit,target,lun);
	    db_printf("ior @ address 0x%x", ptr);
            db_printf("\n   ");
            db_printf("io_unit = %d", ptr->io_unit);
            db_printf("\n   ");
            db_printf("io_op = %d", ptr->io_op);
	    db_printf("\n   ");
            db_printf("io_mode = %d", ptr->io_mode);
            db_printf("\n   ");
            db_printf("io_recnum = %d", ptr->io_recnum);
            db_printf("\n   ");
            db_printf("data ptr = %d", ptr->io_un.data);
            db_printf("\n   ");
            db_printf("io_sgp = 0x%x", ptr->io_sgp);
            db_printf("\n   ");
	    if (ptr->io_sgp) {
	        db_printf("   #entries = %d", ptr->io_sgp->iosg_hdr.nentries);
	        db_printf("\n   ");
	    }
            db_printf("io_count = %d", ptr->io_count);
            db_printf("\n   ");
            db_printf("io_alloc_size = %d", ptr->io_alloc_size);
            db_printf("\n   ");
            db_printf("io_total = %d", ptr->io_total);
        }
    }
}

void
cntrlr_q(int unit)
{
    siop_softc_t	siop;
    struct cntrlr_buf	*cntrlr_buf_p;
    int			i, j;

    if (unit >= NSIOP) {
        db_printf("Bad SIOP unit = %d\n", unit);
        return;
    }
    if (!(siop = siop_softc[unit])) {
        db_printf("Bad ptr to siop structure\n");
        return;
    }

    db_printf("siop->cntrlr_q_index  = %d\n", siop->cntrlr_q_index);
    db_printf("script cntrlr_q index = %d\n",
        ((struct cntrlr_buf *)
         (*((struct cntrlr_buf **)SCRIPT_CNTRLR_Q_P(siop->script)))) -
        ((struct cntrlr_buf *)
         kvtophys((vm_offset_t)(siop->shrd_locs->cntrlr_q))));

    for (i = 0; i < CNTRLR_Q_SIZE; i++) {
	cntrlr_buf_p = (struct cntrlr_buf *)&siop->shrd_locs->cntrlr_q[i];

	db_printf("cntrlr_q[%d]\n", i);
	db_printf("    q_tag = 0x%x\n", cntrlr_buf_p->info.q_tag);
	db_printf("    target_id = %d\n", cntrlr_buf_p->info.target_id);
	db_printf("    req_tbl.src_ptr  = 0x%x\n",
		cntrlr_buf_p->req_tbl.src_ptr);
	db_printf("    req_tbl.dest_ptr = 0x%x\n",
		cntrlr_buf_p->req_tbl.dest_ptr);
	db_printf("    req_tbl.cnt      = %d\n",
		cntrlr_buf_p->req_tbl.cnt);
	for (j = 0; j < SG_LISTS; j++) {
		db_printf("    sg_list[%d].src_ptr = %d\n", j,
			cntrlr_buf_p->sg_list[j].src_ptr);
		db_printf("    sg_list[%d].dest_ptr= %d\n", j,
			cntrlr_buf_p->sg_list[j].dest_ptr);
		db_printf("    sg_list[%d].cnt     = %d\n", j,
			cntrlr_buf_p->sg_list[j].cnt);
	}
    }
}

void
req_done_q(int unit)
{
    siop_softc_t			siop;
    int					i;

    if (unit >= NSIOP) {
        db_printf("Bad SIOP unit = %d\n", unit);
        return;
    }
    if (!(siop = siop_softc[unit])) {
        db_printf("Bad ptr to siop structure\n");
        return;
    }

    db_printf("siop->req_done_q_index  = %d\n", siop->req_done_q_index);
    db_printf("script req_done_q index = %d\n",
	((struct request_table **)
	 (*(struct request_table ***)SCRIPT_REQ_DONE_PTR(siop->script))) -
        ((struct request_table **)
	 kvtophys((vm_offset_t)siop->shrd_locs->req_done_q)));

    for (i = 0; i < REQ_DONE_Q_SIZE; i++) {
	if (siop->shrd_locs->req_done_q[i]) {
	    db_printf("virt, phys req_done_q[%d] = 0x%x, 0x%x\n",
		      i, siop->shrd_locs->req_done_q[i], 
		      siop->req_tbl_ptr_clram + 
	      (siop->shrd_locs->req_done_q[i] - siop->req_tbl_ptr_clram_phys));
	}
    }
}

void
req_tbl(volatile struct request_table *req_tbl_p, int unit, 
	                                          int index_shrd_req_tbl)
{
    /* usage from debugger:
       req_tbl(req_tbl_p) - prints the entries of req tbl at given address
       req_tbl(0, unit, index_shrd_req_tbl) - prints the entries of req tbl
                             at &siop->shrd_locs->req_tbl[index_shrd_req_tbl]
    */			     

    int	j;

    if(req_tbl_p) {
	if ((unsigned int)req_tbl_p < 0xf0000000) {
	    db_printf("Bad request table ptr\n");
	    return;
	}
    } else {
	if (unit >= NSIOP) {
	    db_printf("Bad SIOP unit = %d\n", unit);
	    return;
	}
	if (index_shrd_req_tbl >= CNTRLR_Q_SIZE) {
            db_printf("Bad index\n");
            return;
        }
	req_tbl_p = &siop_softc[unit]->shrd_locs->req_tbl[index_shrd_req_tbl];
        if ((unsigned int)req_tbl_p < 0xf0000000) {
            db_printf("request table ptr calculated from unit and index is bad\n");
            return;
        }
    }

    db_printf("    bus_conf.sxfer=0x%x\n",
	      req_tbl_p->bus_conf.sxfer);
    db_printf("    bus_conf.dest_id=0x%x\n",
	      req_tbl_p->bus_conf.dest_id);
    db_printf("    bus_conf.scntl3=%x\n",
	      req_tbl_p->bus_conf.scntl3);
    db_printf("    msg_out.len=%d\n", req_tbl_p->msg_out.len);
    db_printf("    msg_out.ptr=0x%x\n", req_tbl_p->msg_out.ptr);
    db_printf("    &msg_out.bytes=0x%x\n",
	      req_tbl_p->msg_out.bytes);
    if (req_tbl_p->msg_out.len > MAX_MSG_OUT_LGT) {
        db_printf("Bad msg_out.len\n");
    }
    db_printf("    msg_out.bytes=");
    for (j = 0; j < req_tbl_p->msg_out.len; j++) {
	db_printf(" 0x%x", req_tbl_p->msg_out.bytes[j]);
    }
    db_printf("\n");

    db_printf("    cmd.len=%d\n", req_tbl_p->cmd.len);
    db_printf("    cmd.ptr=0x%x\n", req_tbl_p->cmd.ptr);
    db_printf("    &cmd.bytes=0x%x\n", req_tbl_p->cmd.bytes);
    if (req_tbl_p->cmd.len > MAX_CMD_LGT) {
        db_printf("Bad cmd.len\n");
    }
    db_printf("    cmd.bytes=");
    display_cmd(req_tbl_p->cmd.bytes);
    db_printf(": ");
    for (j = 0; j < req_tbl_p->cmd.len; j++) {
	db_printf(" 0x%x", req_tbl_p->cmd.bytes[j]);
    }
    db_printf("\n");

    db_printf("    sg_ent_ptr=0x%x\n", req_tbl_p->sg_ent_ptr);

    db_printf("\n");
    db_printf("    sg entries=");
    for (j = 0; j < SG_ENTS; j++) {
        db_printf("0x%x 0x%x ",
                req_tbl_p->sg_ent[j].len, req_tbl_p->sg_ent[j].ptr);
    }
}

void 
ior(struct io_req * ptr)
{
    db_printf("\n");
    if (!ptr) {
	db_printf("Bad struct io_req ptr\n");
        return;
    }
    db_printf("ior @ address 0x%x", ptr);
    db_printf("\n   ");
    db_printf("io_unit = %d", ptr->io_unit);
    db_printf("\n   ");
    db_printf("io_op = 0x%x", ptr->io_op);
    db_printf("\n   ");
    db_printf("io_error = 0x%x", ptr->io_error);
    db_printf("\n   ");
    db_printf("io_mode = %d", ptr->io_mode);
    db_printf("\n   ");
    db_printf("io_recnum = %d", ptr->io_recnum);
    db_printf("\n   ");
    db_printf("data ptr = 0x%x", ptr->io_un.data);
    db_printf("\n   ");
    db_printf("io_sgp = 0x%x", ptr->io_sgp);
    db_printf("\n   ");
    if (ptr->io_sgp) {
	db_printf("   #entries = %d", ptr->io_sgp->iosg_hdr.nentries);
	db_printf("\n   ");
    }
    db_printf("io_count = %d", ptr->io_count);
    db_printf("\n   ");
    db_printf("io_alloc_size = %d", ptr->io_alloc_size);
    db_printf("\n   ");
    db_printf("io_total = %d", ptr->io_total);
}

void
tag(struct cntrlr_intf_tag *tag_p)
{
    if (!tag_p || ((unsigned int)tag_p < 0xf0000000)) {
	db_printf("Bad cntrlr_intf_tag ptr\n");
	return;
    }
    db_printf("\n   ");
    db_printf("links.next=0x%x ", tag_p->tag.links.next);
    db_printf("\n   ");
    db_printf("links.prev=0x%x ", tag_p->tag.links.prev);
    db_printf("\n   ");
    db_printf("tag id=0x%x ", tag_p->tag.tag);
    db_printf("\n   ");
    db_printf("ior=0x%x ", tag_p->tag.ior);
    db_printf("\n   ");
    db_printf("flags=0x%x ", tag_p->tag.flags);
    db_printf("\n   ");
    db_printf("cmd_ptr=0x%x ", tag_p->tag.cmd_ptr);
    db_printf("\n   ");
    db_printf("cmd_count=%d ", tag_p->tag.cmd_count);
    db_printf("\n   ");
    db_printf("cmd_only=%d ", tag_p->tag.cmd_only);
    db_printf("\n   ");
    db_printf("cur_cmd=0x%x ", tag_p->tag.cur_cmd);
    db_printf("\n   ");
    db_printf("lun=0x%x ", tag_p->tag.lun);
    db_printf("\n   ");
    db_printf("tgt ptr=0x%x ", tag_p->tag.tgt);
    db_printf("\n   ");
    db_printf("sch_links.next=0x%x ", tag_p->sch_links.next);
    db_printf("\n   ");
    db_printf("sch_links.prev=0x%x ", tag_p->sch_links.prev);
    db_printf("\n   ");
}

void
tgt(struct target_info *tgt)
{
    if (!tgt || ((unsigned int)tgt < 0xf0000000)) {
        db_printf("Bad tgt ptr\n");
        return;
    }
    db_printf("\n   ");
    db_printf("links not used");
    db_printf("\n   ");
    db_printf("ior ptr=0x%x ", tgt->ior);
    db_printf("\n   ");
    db_printf("flags=0x%x ", tgt->flags);
    db_printf("\n   ");
    db_printf("hw_state=0x%x ", tgt->hw_state);
    db_printf("\n   ");
    db_printf("cmd_ptr=0x%x ", tgt->cmd_ptr);
    db_printf("\n   ");
    db_printf("dev_ops=0x%x ", tgt->dev_ops);
    db_printf("\n   ");
    db_printf("target_id=%d ", tgt->target_id);
    db_printf("\n   ");
    db_printf("unit_no=%d ", tgt->unit_no);
    db_printf("\n   ");
    db_printf("scntl3=0x%x ", tgt->sync_period);
    db_printf("\n   ");
    db_printf("sxfer=0x%x ", tgt->sync_offset);
    db_printf("\n   ");
    db_printf("block_size=%d ", tgt->block_size);
    db_printf("\n   ");
    db_printf("done=%d ", tgt->done);
    db_printf("\n   ");
    db_printf("cur_cmd=0x%x ", tgt->cur_cmd);
    db_printf("\n   ");
    db_printf("lun=%d ", tgt->lun);
    db_printf("\n   ");
    db_printf("true_lun=%d ", tgt->true_lun);
    db_printf("\n   ");
    db_printf("masterno=%d ", tgt->masterno);
    db_printf("\n   ");
    db_printf("tgt_name=%s ", tgt->tgt_name);
    db_printf("\n   ");
    db_printf("tag_q ptr=0x%x ", tgt->tag_q);
    db_printf("\n   ");
    db_printf("tag_q_size=%d ", tgt->tag_q_size);
    db_printf("\n   ");
    db_printf("tag_q_depth=%d ", tgt->tag_q_depth);
    db_printf("\n   ");
}

void
stat(int unit, int target)
{
    siop_softc_t		siop;
    target_info_t		*tgt;
    struct cntrlr_intf_tag	*cntrlr_tag_p;
    io_req_t			req_ptr;
    int				i, lun;

    if (unit >= NSIOP) {
        db_printf("Bad SIOP unit = %d\n", unit);
        return;
    }
    if (!(siop = siop_softc[unit])) {
        db_printf("Bad ptr to siop structure\n");
        return;
    }
    if (target >= MAX_SCSI_TARGETS) {
        db_printf("Bad target = %d\n", target);
        return;
    }

    for (lun = 0; lun < MAX_LUNS; lun++) {

	if (!(tgt = siop->sc->target[target][lun]))
	    continue;

	/*
	 * Weird stuff for disk.  We're not supporting LUNs
	 * on disks yet, so each target/lun structure points
	 * to target/lun 0.
	 */
	if (lun != 0 && tgt == siop->sc->target[target][0])
	    continue;
	db_printf("\n\nu=%d,t=%d,l=%d:\n", unit,target,lun);
        db_printf("\n   ");
        db_printf("target alive=%d\n", 1 && (tgt->flags & TGT_ALIVE));
        db_printf("\n   ");
        db_printf("tag_queuing_enable=0x%x ", 
	          is_tag_queuing_enabled(tgt) == TGT_TAGGED_QUEUING);
        db_printf("tgt_q_full=0x%x ", is_tgt_q_full(tgt) == TGT_QUEUE_FULL);
        db_printf("scsi_queue_enable=0x%x ", is_scsi_queue_enabled(tgt));
        db_printf("\n   ");

        db_printf("size of: ");
        i = 0;
        queue_iterate(&tgt->active_tag_q, cntrlr_tag_p,
		      struct cntrlr_intf_tag *, tag.links) {
	    i++;
        }
        db_printf("active_tag_q=%d ", i);

        i = 0;
        queue_iterate(&tgt->free_tag_q, cntrlr_tag_p,
		      struct cntrlr_intf_tag *, tag.links) {
            i++;
        }
        db_printf("free_tag_q=%d ", i);

        i = 0;
        queue_iterate(&siop->scheduler_q, cntrlr_tag_p,
		      struct cntrlr_intf_tag *, sch_links) {
            i++;
        }
        db_printf("scheduler_q=%d ", i);

        i = 0;
        for (req_ptr = tgt->ior; req_ptr; req_ptr = tgt->ior->io_next) {
            i++;
        }
        db_printf("request_q=%d ", i);
        db_printf("\n   ");
        db_printf("target_info_t ptr=0x%x ", tgt);
        db_printf("\n   ");
    }
}

static void
display_cmd(cmd)
        unsigned char	*cmd;
{
        char    *msg;

        switch (*cmd) {
	  case SCSI_CMD_READ:
                        msg = "Read";
                        break;
		      case SCSI_CMD_LONG_READ:
                        msg = "Long Read";
                        break;
		      case SCSI_CMD_WRITE:
                        msg = "Write";
                        break;
		      case SCSI_CMD_LONG_WRITE:
                        msg = "Long Write";
                        break;
		      case SCSI_CMD_INQUIRY:
                        msg = "Inquiry";
                        break;
		      case SCSI_CMD_REQUEST_SENSE:
                        msg = "Request Sense";
                        break;
		      case SCSI_CMD_MODE_SENSE:
                        msg = "Mode Sense";
                        break;
		      case SCSI_CMD_MODE_SELECT:
                        msg = "Mode Select";
                        break;
		      case SCSI_CMD_LOG_SENSE:
                        msg = "Log Select";
                        break;
		      case SCSI_CMD_FORMAT_UNIT:
                        msg = "Format";
                        break;
		      case SCSI_CMD_RECEIVE_DIAG_RESULTS:
                        msg = "Receive Diagnostic Results";
                        break;
		      case SCSI_CMD_READ_CAPACITY:
                        msg = "Read Capacity";
                        break;
		      case SCSI_CMD_REASSIGN_BLOCKS:
                        msg = "Reassign Block";
                        break;
		      case SCSI_CMD_PREVENT_ALLOW_REMOVAL:
                        msg = "Prevent/Allow Media Removal";
                        break;
		      case SCSI_CMD_REWIND:
                        msg = "Rewind";
                        break;
		      case SCSI_CMD_WRITE_FILEMARKS:
                        msg = "Write Filemark";
                        break;
		      case SCSI_CMD_SPACE:
                        msg = "Space Filemark";
                        break;
		      case SCSI_CMD_READ_BLOCK_LIMITS:
                        msg = "Read Block Limits";
                        break;
		      case SCSI_CMD_READ_POSITION:
                        msg = "Read Position";
                        break;
		      case SCSI_CMD_LOCATE:
                        msg = "Locate";
                        break;
		      case SCSI_CMD_TEST_UNIT_READY:
                        msg = "Test Unit Ready";
                        break;
		      default:
                        msg = "Unknown Command";
                        break;
		    }

        db_printf("%s", msg);
}

void
script_vars(int unit)
{
    siop_softc_t        siop;

    if (unit >= NSIOP) {
        db_printf("Bad SIOP unit = %d\n", unit);
        return;
    }
    if (!(siop = siop_softc[unit])) {
        db_printf("Bad ptr to siop structure\n");
        return;
    }

    db_printf("\nLocal script variables for SIOP%d\n", siop->unit);

    db_printf("\n   ");
    db_printf("index in cntrlr_q = %d ",
	      ((struct cntrlr_buf *)
               (*(struct cntrlr_buf **)SCRIPT_CNTRLR_Q_P(siop->script))) -
              ((struct cntrlr_buf *)
               kvtophys((vm_offset_t)siop->shrd_locs->cntrlr_q)));
    db_printf("\n   ");
    db_printf("index in req_done_q = %d ",
              ((struct request_table **)
               (*(struct request_table ***)SCRIPT_REQ_DONE_PTR(siop->script))) -
              ((struct request_table **)
               kvtophys((vm_offset_t)siop->shrd_locs->req_done_q)));
    db_printf("\n   ");
    db_printf("ignore wide residue = 0x%x ", 
		  *(unsigned long *)SCRIPT_IGNORE_WIDE_FLAG(siop->script));
    db_printf("\n   ");
    db_printf("copy buffer: src=0x%x, dst=0x%x, cnt=0x%x\n",
	*((unsigned long *)SCRIPT_COPY_BUF(siop->script) + 0),
	*((unsigned long *)SCRIPT_COPY_BUF(siop->script) + 1),
	*((unsigned long *)SCRIPT_COPY_BUF(siop->script) + 2));
    db_printf("\n   ");
    db_printf("logic analyzer value = 0x%x ",
                  *(unsigned long *)SCRIPT_VALUE_LOGIC_ANAL(siop->script));
    db_printf("\n   ");
}

struct cntrlr_intf_tag *
req_tbl_clram_image_not_empty()
{
    int i;

    db_printf("\nreq_tbl_clram_image=0x%x",req_tbl_clram_image);
    db_printf("\n   ");
    for (i = 0; i < MAX_REQ_TBLS_CLRAM; i++)
        if (req_tbl_clram_image[i]) {
          db_printf("index=0x%x, &cntrlr_intf_tag=0x%x",
                    i, req_tbl_clram_image[i]);
          db_printf("\n   ");
      }
    return 0;
}

#endif	MACH_ASSERT

/******************** end of functions used by debugger only *****************/

/******************** asserts from SIOP *************************************/
static void
assert_select_tbl_ptr(siop_softc_t siop)
{
    /* don't want to print SLOG_ENTRY message, declare _this_id explicitly */
    long  _this_id = S_BIT(SLOG_GENERIC_ID);

    if(print_q_tag_id(siop))
	SLOG(SLOG_HIGH, ("assert_select_tbl_ptr: exit\n"));
}

static void
assert_reselect_tbl_ptr(siop_softc_t siop)
{
    /* don't want to print SLOG_ENTRY message, declare _this_id explicitly */
    long  _this_id = S_BIT(SLOG_GENERIC_ID);

    if(print_q_tag_id(siop))
        SLOG(SLOG_HIGH, ("assert_reselect_tbl_ptr: exit\n"));
}

static int
print_q_tag_id(siop_softc_t siop)
{
    struct request_table *req_tbl_clram_ptr;
    struct cntrlr_intf_tag *cntrlr_tag_p, *ptr_cntrlr_intf_tag;
    struct target_info *tgt;
    /* don't want to print SLOG_ENTRY message, declare _this_id explicitly */
    long  _this_id = S_BIT(SLOG_GENERIC_ID);

    /* we need to check whether the request table ptr in CLRAM is valid */
    assert(siop->shrd_locs->req_tbl_clram_ptr >= siop->req_tbl_ptr_clram_phys);

    req_tbl_clram_ptr = siop->req_tbl_ptr_clram + 
	(siop->shrd_locs->req_tbl_clram_ptr - siop->req_tbl_ptr_clram_phys);
    assert(req_tbl_clram_ptr->bus_conf.dest_id >= 0 &&
           req_tbl_clram_ptr->bus_conf.dest_id < MAX_SCSI_TARGETS);
    
    /* if we are probing the targets, lots of data structures have not been
       set up, therefore return */
    if (!siop->sc->target[req_tbl_clram_ptr->bus_conf.dest_id][0]) {
	return 0;
    }

    cntrlr_tag_p =
	get_cntrlr_intf_tag(siop, siop->shrd_locs->req_tbl_clram_ptr);
    assert(cntrlr_tag_p);
    tgt = cntrlr_tag_p->tag.tgt;

    assert(req_tbl_clram_ptr->bus_conf.sxfer == tgt->sync_offset ||
	   req_tbl_clram_ptr->bus_conf.sxfer == siop->siop_sxfer);
    assert(req_tbl_clram_ptr->bus_conf.scntl3 == tgt->sync_period ||
	   req_tbl_clram_ptr->bus_conf.scntl3 == siop->siop_scntl3);
    assert(req_tbl_clram_ptr->bus_conf.dest_id == tgt->target_id);

    queue_iterate(&cntrlr_tag_p->tag.tgt->active_tag_q,
                  ptr_cntrlr_intf_tag, struct cntrlr_intf_tag *, tag.links) {
        if (ptr_cntrlr_intf_tag == cntrlr_tag_p) {
            if (!cntrlr_tag_p->tag.cmd_only && 
		is_tag_queuing_enabled(cntrlr_tag_p->tag.tgt) && 
		is_scsi_queue_enabled(cntrlr_tag_p->tag.tgt)) {
                assert(cntrlr_tag_p->tag.tag == 
		       req_tbl_clram_ptr->msg_out.bytes[2]);
	    }
	    SLOG(SLOG_HIGH, ("      u=%d,t=%d,l=%d,", tgt->masterno, 
			     tgt->target_id, cntrlr_tag_p->tag.lun));
            SLOG(SLOG_HIGH, ("q=%d; ",
                             (!cntrlr_tag_p->tag.cmd_only &&
                              is_tag_queuing_enabled(cntrlr_tag_p->tag.tgt) &&
			      is_scsi_queue_enabled(cntrlr_tag_p->tag.tgt)) ?
                              cntrlr_tag_p->tag.tag: 99999));
            return 1;
	}
    }	/* queue_iterate(&cntrlr_tag_p..... */
    assert(0);
}

static void
assert_data_ptr_saved(siop_softc_t siop)
{
    struct request_table *req_tbl_clram_ptr;
    unsigned long *sg_ent_p;
    /* don't want to print SLOG_ENTRY message, declare _this_id explicitly */
    long  _this_id = S_BIT(SLOG_GENERIC_ID);

    SLOG(SLOG_HIGH, ("      assert_data_ptr_saved: enter\n"));

    if(print_q_tag_id(siop)) {
	req_tbl_clram_ptr = siop->req_tbl_ptr_clram +
        (siop->shrd_locs->req_tbl_clram_ptr - siop->req_tbl_ptr_clram_phys);

	sg_ent_p = (unsigned long *)((unsigned char *)siop->req_tbl_ptr_clram +
		     (req_tbl_clram_ptr->sg_ent_ptr -
		      (unsigned char *)siop->shrd_locs->req_tbl_clram_ptr));
	SLOG(SLOG_HIGH, ("assert_data_ptr_saved: sg_entry, len=%d, ptr=%x\n",
			 *sg_ent_p, *(++sg_ent_p)));
    }
}

static void
assert_gen_purp_1(siop_softc_t siop)
{
    /* don't want to print SLOG_ENTRY message, declare _this_id explicitly */
    long  _this_id = S_BIT(SLOG_GENERIC_ID);
}

static void
assert_gen_purp_2(siop_softc_t siop)
{
    /* don't want to print SLOG_ENTRY message, declare _this_id explicitly */
    long  _this_id = S_BIT(SLOG_GENERIC_ID);
}

static void
assert_gen_purp_3(siop_softc_t siop)
{
    /* don't want to print SLOG_ENTRY message, declare _this_id explicitly */
    long  _this_id = S_BIT(SLOG_GENERIC_ID);
}

/******************** end of asserts from SIOP ******************************/

#endif	NSIOP > 0
