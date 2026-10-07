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
 
#define LTU 1

#include <i860paragon/vcf/vcf.h>
#include <i860paragon/vcf/vcf_cnic.h>
#include <i860paragon/vcf/vcf_msgout.h>
#include <i860paragon/dp.h>
#include <i860paragon/ltu.h>

static int  *ltu_in_addr, *ltu_out_addr, ltu_in_count=0, ltu_out_count=0;
static int  ltu_in_bypass = 0, ltu_out_bypass = 0;

#define  FALSE  0
#define  TRUE   1

#define  NIC_STAT_RX_FIFO_NOT_EMPTY NIC_STAT_RX_FIFO_NOT_EMPTY
#define  SAFE_TO_READ(x)  ((x)->halfs.lo & (NIC_STAT_RX_FIFO_NOT_EMPTY))




#ifndef VCF_INLINE

void
NIC_RECV_END_MSG(void)  {
	int x, y;

	recv2(x, y);

#if (DEBUG > 1)
	printf("Received end of message.  x == 0x%x, y == 0x%x\n", x, y);
#endif
}




void
NIC_RECV_WORDS(int *val1, int *val2)  {
	int  x, y;

#if (DEBUG > 1)
	printf("Entering NIC_RECV_WORDS().\n");
#endif

	recv2(x, y);
	if (val1 != NULL)
		*val1 = x;
	if (val2 != NULL)
		*val2 = y;
#if (DEBUG > 1)
	printf("Recv: 0x%x 0x%x\n", x, y);
#endif
}




void
NIC_SEND_WORDS(int val1, int val2)  {
#if (DEBUG > 1)
	printf("Send: 0x%x 0x%x\n", val1, val2);
#endif
	send2(val1, val2);
}




void
NIC_SEND_START_MSG(int dest)  {

#ifdef DEBUG
    printf("SEND START 0x%08x\n", dest);
#endif

  send2(dest, 0);
}




void
NIC_SEND_END_MSG()  {
  nic_reg  burst;

  send2eod(0xdeadbeef, 0xbeefdead);

  /* Stuff for the burst workaround. */
  burst.halfs.hi = 0;
  burst.halfs.lo = NIC_TEST_RCV_AFULL_SEL;
  NIC.reset_test.full = burst.full;

#ifdef DEBUG
    printf("SENT END\n");
#endif
}

#endif VCF_INLINE


#ifdef  NO_ASSEMBLY

void
LTU_RECV_BLOCK(int *buf, int packetlen)  {
#ifdef DEBUG
	printf("LTU_RECV_BLOCK(0x%x, %d)\n", buf, packetlen);
#endif

#if LTU
	/*  Since the LTU can not send a single cache line, send it by hand.
	 */
	if (packetlen == (LTU_MASK + 1))  {
		ltu_in_bypass = 1;
		while (packetlen)  {
			NIC_RECV_WORDS(&buf[0], &buf[1]);
			buf += 2;
			packetlen -= 8;
		}
	}  else
		ltu_start(((unsigned long) buf) & 0x3fffffff,
			  BYTES_TO_LTUS(packetlen) | 0x40000000);
#ifdef DEBUG
	printf("Count = 0x%x.\n", inl(DP_LTU0_COUNT));
#endif
#else
	while (packetlen)  {
		while (NIC_INBUF_EMPTY());
		NIC_RECV_WORDS(&buf[0], &buf[1]);
		buf += 2;
		packetlen -= 8;
	}
#endif

#ifdef DEBUG
	printf("IN LTU DONE\n");
#endif
}




void
LTU_SEND_BLOCK(int *buf, int packetlen)  {
#ifdef DEBUG
	printf("LTU_SEND_BLOCK(0x%x, %d)\n", buf, packetlen);
#endif

#if LTU
	/*  Since the LTU can not send a single cache line, send it by hand.
	 */
	if (packetlen == (LTU_MASK + 1))  {
		ltu_out_bypass = 1;
		while (packetlen)  {
			NIC_SEND_WORDS(buf[0], buf[1]);
			buf += 2;
			packetlen -= 8;
		}
	}  else
		ltu_start((((unsigned long) buf) + 4) & 0x3fffffff,
			  BYTES_TO_LTUS(packetlen) | 0xc0000000);
#if DEBUG
	printf("Count = 0x%x.\n", inl(DP_LTU1_COUNT));
#endif
#else
	while (packetlen)  {
		while (NIC_OUTBUF_FULL());
		NIC_SEND_WORDS(buf[0], buf[1]);
		buf += 2;
		packetlen -= 8;
	}
#endif

#ifdef DEBUG
	printf("OUT LTU DONE\n");
#endif
}

#endif  /* NO_ASSEMBLY */


static int
NIC_INBUF_READY(void)  {
  nic_reg  status;

  status.full = NIC.status.full;
  return(SAFE_TO_READ(&status));
}


static int
NIC_INBUF_EMPTY(void)  {
  return (!NIC_INBUF_READY());
}




int
NIC_OUTBUF_FULL(void)  {
	nic_reg  status;
	static int count = 0;

	status.full = NIC.status.full;
#if (DEBUG > 2)
	if (count++ == 100000)  {
		if (out_ltu_active)
			printf("*");
		else
			printf("#");
		count = 0;
	}
#endif
	return(!NIC_TX_READY(&status));
}


#ifdef  NO_ASSEMBLY

int
IN_LTU_DONE(void)  {
	static int count = 0;

#ifdef DEBUG
	if (count++ == 100000)  {
		printf("(");
		count = 0;
	}
#endif

#if LTU
	if (ltu_in_bypass)  {
		ltu_in_bypass = 0;
		return(1);
	}  else if (inl(DP_STATUS_HI) & DP_ISTAT_LTU0_CNT)  {
		inl(DP_LTU0_CLEAR_CNT);
		return(1);
	} else
		return(0);
#else
	return(1);
#endif
}




int
OUT_LTU_DONE(void)  {
	static int count = 0;

#if (DEBUG > 2)
	if (count++ == 100000)  {
		printf("count = 0x%x, status_hi = 0x%x", inl(DP_LTU1_COUNT),
		       inl(DP_STATUS_HI));
		count = 0;
	}
#endif

#if LTU
	if (ltu_out_bypass)  {
		ltu_out_bypass = 0;
		return(1);
	} else if (inl(DP_STATUS_HI) & DP_ISTAT_LTU1_CNT)  {
		inl(DP_LTU1_CLEAR_CNT);
		return(1);
	} else
		return(0);
#else
	return(1);
#endif
}

#endif  /* NO_ASSEMBLY */
