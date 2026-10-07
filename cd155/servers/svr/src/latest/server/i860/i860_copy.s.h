/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: i860_copy.s.h,v $
 * Revision 1.3  1994/11/18  20:31:48  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1994/04/05  16:08:58  cfj
 * Merge revision 1.1.2.1 from R1_2 into the main stem.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.1.2.1  1994/04/05  16:06:31  cfj
 * Use f16 to prime the pipeline so not to trash f2 & f3.
 *
 *  Reviewer:andyp,jlitvin
 *  Risk:M
 *  Benefit or PTS #:8822
 *  Testing:pccm2,VSX EAT
 *  Module(s):server/i860/i860_copy.s.h
 *
 * Revision 1.1  1993/09/21  19:58:17  cfj
 * Picked up dnoveck@osf.org i860 bcopy changed from ad1.0.5.
 *
 * Revision 2.3  93/06/16  14:04:08  dnoveck
 * 	Fix dollar-EndLog-dollar which ad-ci treated as part of comment.
 * 
 * Revision 2.2  93/06/16  13:51:57  dnoveck
 * 	Initial version.
 * 
 * $EndLog$
 * 
 */

#include <sys/errno.h>

//  i860_copy.s.h -- Provide bcopy-type subroutine
//
//  This header file will generate a bcopy-type subroutine.
//  The goal is to only have one source for such subroutines
//  in the system rather than the following four:
//
//       Server bcopy
//       Server user_bcopy
//       Emulator bcopy
//       Emulator user_bcopy
//
//  Generation of the approriate subroutine is governed by 
//  the previous definition of the following symbols.
//
//     _BCOPY_ -- Generates a bcopy subroutine and a memcopy
//                entry point.
//     _USER_BCOPY_ -- Generates a user_bcopy subroutine with
//                     a user_bcopy2 entry point.
//
//  The options _BCOPY_ and _USER_BCOPY_ are incompatible.
//
//
//  In the _BCOPY_ case, the following options control code
//  generation.
//
//     _BCOPY_DEBUG_ -- If defined, tests for negative length 
//                      and wraparound are assembled.
//     _BCOPY_ERROR1_ -- Name of routine to call if we have
//                       negative length or wraparound.
//     _BCOPY_ERROR2_ -- If defined, name of second routine to
//                       call in the event of negative length
//                       or wraparound.
//
//  Register usage is as follows:
//
//     r16 -- Input pointer (requires a small displacement
//            when doing the pipelined copy)
//     r17 -- Output pointer (requires a small displacement
//            when doing the pipelined copy)
//     r18 -- Byte count remaining.
//     r19 -- Pointer to output length with value to be returned
//            on success on low-order bit (for user_bcopy[2])
//     r20 -- Loop control information (for user_bcopy2)
//     r20 -- Value to return to caller (if doing memcopy)
//     r21 -- Additional count register 
//     r22 -- Additional count register
//     r23 -- Pointer to area copied with an unwound loop (for 
//            user_bcopy2)
//     r24 -- Initial value of destination (for user_bcopy2)
//     r25 -- Holds current word or byte
//     r26-r29 -- Additional work registers for multiword copies
//     r30 -- Holds minus-one for loop control
//     r31 -- Miscellaneous temporary
//     f16-f31 -- Holds current sixty-four bytes to move in
//                quad copy loop
//
//  The algorithm for the basic copy (common to bcopy, user_bcopy
//  and user_bcopy2) is as follows:
//
//      If source and destination are quad-aligned,
//          If at least sixty-four bytes to move,
//              If running on an I860XP,
//                  Copy sixty-four bytes per pass using pfld.q
//                  and fst.q to/from sixteen fp registers.
//              Else,
//                  Copy sixty-four bytes per pass using pfld.d
//                  and fst.q to/from sixteen fp registers.
//          CopyQuadRemnants() as defined below.
//      Else if source and destination are word-aligned,
//          CopyWordAligned() as defined below.
//      Else if alignments within word do not match,
//          If length is less than thirty-two bytes,
//              If length is zero,
//                  Just return.
//              Else,
//                  Do byte-byte-copy for upto thirty-one bytes.
//          Else,
//              If destination not word alignede
//                  Copy byte-by-byte for upto three bytes until
//                  destination is word-aligned.
//              Copy sixteen bytes at a time using four integer
//              registers doing shrd's to fix alignment.
//              If anything is left to do,
//                  Do byte-by-byte copy for upto fifteen bytes.
//      Else if we have to move at least eight bytes,
//          If arguments are not word aligned,
//              Do byte-by-byte copy for upto three bytes until
//              arguments are word-aligned.
//          CopyWordAligned() as defined below.
//
//  Where CopyWordAligned() is defined as:
//
//      If we have at least four words to move,
//          If destination is not quad-word-aligned,
//              Copy word-by-word for upto three words until
//              destination is quad-word-aligned.
//          If at least sixty-four bytes left to move,
//              If source is quad-word-aligned and we are running on 
//              an I860XP,
//                  Copy sixty-four bytes per pass using pfld.q
//                  and fst.q to/from sixteen fp registers.
//              Else if source is double-aligned,
//                  Copy sixty-four bytes per pass using pfld.d
//                  and fst.q to/from sixteen fp registers.
//              Else,
//                  Copy sixty-four bytes per pass using pfld.l
//                  and fst.q to/from sixteen fp registers.
//          CopyQuadRemnants() as defined below.
//      Else if we have something to copy,
//          If there are full words to move,
//              Do word-by-word copy for up to three words
//              until no full words remain to be copied.
//          If anything left to copy,
//              Do byte-by-byte copy for upto three bytes.
//      Else,
//          Just return.
//
//  And where CopyQuadRemants() is defined as:
//
//      If we have something left to copy,
//          If there are at least sixteen bytes to move,
//              Copy sixteen bytes per pass using pfld.l and fst.q
//              to/from four fp registers.
//          If there are full words to move,
//              Do word-by-word copy for up to three words
//              until no full words remain to be copied.
//          If anything left to copy,
//              Do byte-by-byte copy for upto three bytes.

#ifdef _BCOPY_
#define RET_OK      bri r1;  mov  r20,r16
#define SET_PIPE    nop
#define RESET_PIPE  
#define SHRLEN     shra

#ifdef _BCOPY_DEBUG_
.bcopy_neg:
	orh	h%.negmsg,r0,r16    // First half of parm
        br      .bcopy_err          // Go join the other cases
	 or	l%.negmsg,r16,r16   // Set the rest of parameter

.bcopy_wrap:
	orh	h%.wrapmsg,r0,r16   // First half of parm
        br      .bcopy_err          // Go join the other cases
	 or	l%.wrapmsg,r16,r16  // Set the rest of parameter

.bcopy_err:
        addu    -16,sp,sp           // Get some stack space
        st.l    r1,0(sp)            // Save return address
        st.l    r16,4(sp)           // Save parameter
        call    _BCOPY_ERROR1_      // Call error routine
         nop                        // Empty delay slot
#ifdef _BCOPY_ERROR2_
        call    _BCOPY_ERROR2_      // Call error routine
         ld.l   4(sp),r16           // Reload parameter
#endif /* _BCOPY_ERROR2_ */
        ld.l    0(sp),r1            // Restore return address
        addu    16,sp,sp            // Reset stack
        RET_OK
        .data
.negmsg:  .string "bcopy negative length"
.wrapmsg: .string "bcopy wrap address"
        .align 4
        .text
#endif /* _BCOPY_DEBUG_ */

//	char * memcopy   	    Returns original dest pointer
//          (char * dst,            Destination pointer
//           char * src,            Source pointer
//           int count)             Count of characters to move

_memcopy::
	mov	r16,r20		    // Move original r16 for return
	mov	r17,r16		    // Move source to its std. home
	mov     r20,r17		    // Move dest to its std. home


//	void bcopy		    Returns updated src pointer
//          (char * src,            Source pointer
//           char * dst,            Destination pointer
//           int count)             Count of characters to move
//
// Calling bcopy with a negative count or with addresses that would 
// wrap around is an error.  These conditions are checked for if
// _BCOPY_DEBUG_ is defined.  If it is not defined, negative counts
// will result in some small amount of data (1-143) bytes being 
// copied and addresses will wrap from 0xffffffff to zero.

_bcopy::
#ifdef _BCOPY_DEBUG_
        andh    0x8000,r18,r0       // Check for negative length
        bnc     .bcopy_neg          // Branch if so
        addu    r16,r18,r31         // Compute end of source
        bte     r0,r31,.chk2        // If source, goes to end, OK
        bc      .bcopy_wrap         // If wrap, report problem
.chk2:  addu    r17,r18,r30         // Compute end of dest
        bte     r0,r30,.copy        // If source, goes to end, OK
        bc      .bcopy_wrap         // If wrap, report problem
#endif /* _BCOPY_DEBUG_ */

#endif /* _BCOPY_ */

#ifdef _USER_BCOPY_
#define RET_OK     bri r1;  and  1,r19,r16
#define SET_PIPE   or 0x1000,r20,r20
#define RESET_PIPE andnot 0x1000,r20,r20
#define SHRLEN     shr



//	_uacc_start marks the starting address of functions that can 
//      fault on access to user addresses.  Any exception between there
//      and and _uacc_end will result in a branch to the common error
//      error recovery point, _uacc_err.  The includer must define
//      _uacc_start before this point and before any of his own routines
//      that need this treatment.  The following options are available
//      to such routines:
//      
//          If r19 is zero, r16 will be returned as zero on fault.
//
//          If r19 is non-zero and r20 is zero, r19 will be treated
//          as the address of a word to receive the length processed,
//          computed as r17 minus r24.  In this case, r16 will, on
//          return contain the error code left in r16 by the fault
//          code.
//
//          If both r19 and r20 are non-zero, special handling 
//          designed for user-bcopy2 will be used to compute the 
//          length processed with other handling as in the previous
//          case.  Other special handling could be invoked with new
//          code to deal with special indication in r20, as long 
//          as these are disjoint from those currently implemented.
//          See user_bcopy2 for details on these.

//	int user_bcopy		    Returns success indication
//              (char * src,	    Source pointer
//               char * dst,	    Destination pointer
//		 unsigned count)    Count of characters to move
//
//  Move routine which returns indication of success or failure
//  as a result of a data fault or address wraparound.  
	    
_user_bcopy::
        addu    r16,r18,r31         // Compute end of source
        bte     r0,r31,.uch2        // Branch if to end
        bc     .ubc_wrap            // Branch if overflow
.uch2:  addu    r17,r18,r30         // Compute end of dest
        bte     r0,r30,.uchok      // Branch if to end
        bnc.t   .copy               // Branch if no overflow
	 mov	1,r19		    // Indicate no count pointer and
                                    // return one on success
	br	.ubc_wrap	    // Otherwise, go handle fault
         nop                        // A dead clock.  Horrors!

.uchok:
        br     .copy                // Go do the copy
	 mov	1,r19		    // Indicate no count pointer and
                                    // return one on success

//	void user_bcopy2	    Returns error code or zero
//               (char * src,	    Source pointer
//                char * dst,	    Destination pointer
//		  unsigned * count) Count of characters to move
//                                  which is updated to reflect the
//                                  amount actually moved
//
//  Move routine which indicates the amount actually moved before
//  hitting a fault or address wraparound.
//
//  It is assumed in the implementation below that accurate counts
//  are required in "normal" cases, i.e. when the fault occurs on the
//  first byte of the transfer or on a page boundary.  Faults which
//  occur in the middle of a page due to the unmapping of the target
//  region by an asynchronous thread may give result that are off
//  by no more than the amount moved by a single loop iteration, i.e.
//  sixty-four bytes.
//
//  Handling of faults is controlled by fault control info in r20.
//  If r20 is zero, simple fault handling is used.  It is assumed
//  that the loop fetches one data item updates the source, stores
//  a data item and then updates the destination.  Each fetch is 
//  assumed to be aligned mod its length and all fetches use a
//  displacement of zero.  With a such a loop, the amount copied
//  can be determined by simply subtracting the base destination
//  from r17.
//
//  For more complicated loops, a non-zero value in r20 helps the
//  fault routine to properly deal with the fault.  The four valid
//  fields in r20, starting from the low-order end, are:
//
//     0-3 --  log2 of amount moved by loop
//     4-7 --  log2 of destination alignment for loop
//     8-11 -- log2 of destination displacement used in loop (zero 
//             if displacement is zero)
//     12 --   non-zero if pipelined loop and not draining
//
//  The more complicated loops must obey certain rules:
//
//     No fetch or store may be done after updating the destination
//     pointer to final value for the current loop iteration.
//
//     If a loop iteration fetches values for the next iteration.  It
//     must have set the pipelining bit set in r20.
//
//     The source starting point for the loop must be saved in r23
//     and the destination starting point must be determinable from
//     r17 and destination alignment in r20.

_user_bcopy2::
	mov	r18,r19		   // Save pointer to count
	ld.l	0(r19),r18	   // Load the count
	mov	r17,r24		   // Save the base output pointer
	mov	0,r20		   // Indicate simple loops
        addu    r16,r18,r31        // Compute end of source
        bte     r0,r31,.u2ch2      // Branch if to end
        bc      .ubc2_wrap         // Branch if overflow
.u2ch2: addu    r17,r18,r30        // Compute end of dest
        bte     r0,r30,.copy       // If to end, branch
        bc      .ubc2_wrap         // Branch if overflow

#endif /* _USER_BCOPY_ */	    



#if defined(_BCOPY_) || defined(_USER_BCOPY_)

.copy:
	adds    -1,r0,r30           // Put -1 in r30 for loops
//
//      Quick checks for highly-aligned cases.
//
        or      r16,r17,r31         // Or together source and dest
        and     15,r31,r0           // Both quad-aligned?
        bc.t    .qboth              // Branch if so
  	 SHRLEN	6,r18,r21           // Shift to loop count
        and     3,r31,r0            // Both word-aligned?
        bc.t    .wboth              // Branch if so
  	 SHRLEN	2,r18,r21           // Shift to word count
//
//      Check for alignments within word which do not match.
//	
	xor	r16,r17,r31         // Xor for alignment check
	and	3,r31,r0            // Same alignment?
	bnc	.misalign 	    // If not, go do special code
//                                           
//      Source and destination are not word-aligned but the alignments
//      within word match.
//
	andnot  7,r18,r0	    // See if length is at least 8
	bc	.bshort		    // Branch if not
	and	3,r16,r22	    // Displacement within word
	addu	-4,r18,r18	    // Subtract a word from length
	addu	r22,r18,r18	    // Add in bytes not moved
//                                           
//      Non-word alignment.  Move bytes until aligned. 
//
.alignwL1:
	ld.b	0(r16),r25	    // Load the next byte
        addu	1,r16,r16	    // Bump the input pointer
	st.b	r25,0(r17)	    // Store the current byte
	and	3,r16,r0	    // Are we aligned?
	bnc.t	.alignwL1	    // If not, keep going
	 addu	1,r17,r17	    // Bump the output pointer
//
//      We have done alignment.  Set up for full words
//
	addu	1,r17,r17	    // Bump the output pointer
	SHRLEN	2,r18,r21           // Shift to word count
// 
//      Move full words
//
.wboth:
	andnot	3,r21,r0	    // Do we have at least four words?
	bc	.worb		    // Branch if not
	and	15,r17,r0	    // Destination quad-aligned?
	bc	.dquad		    // Branch if it is
//                                           
//      Non-quad alignment.  Move words until aligned. 
//
.alignqL1:
	ld.l	0(r16),r25	    // Load the next word
        addu	4,r16,r16	    // Bump the input pointer
	st.l	r25,0(r17)	    // Store the current word
	addu	4,r17,r17	    // Bump the output pointer
	and	15,r17,r0	    // Are we aligned?
	bnc.t	.alignqL1	    // If not, keep going
	 addu   -4,r18,r18	    // Adjust the length
	addu   -4,r18,r18	    // Adjust the length
//  
//      Copy sixty-four bytes to quad-aligned destination.
// 
.dquad:
	SHRLEN	6,r18,r21	    // Convert the count to loops
        bte     r0,r21,.dqremonly   // If no loops, do remnants with
                                    // quad-aligned destination
#ifdef _USER_BCOPY_
	mov	0x0446,r20	    // Move in loop control info
        mov     r16,r23             // Save starting point
#endif /* _USER_BCOPY_ */
	and	4,r16,r0	    // Is source double-word aligned?
	bnc	.quadsw		    // If not, go handle
        ld.c    epsr,r31            // Fetch the epsr
        or      r31,r16,r31         // Or epsr and source pointer
        and     9,r31,r0            // Test alignment and proc type
        bc      .quadsq             // Branch if on quad boundary and
                                    // not XR


//   
//      Loop Initialization for pipelined copy with double-aligned 
//      source and quad-aligned destination.
//
.quadsd: 
        pfld.d  0(r16),f16	    // Prime the load pipe
	addu   -16,r17,r17	    // Adjust pointer for displacement
	pfld.d  8(r16)++,f16        // Prime the load pipe
	pfld.d  8(r16)++,f16        // Prime the load pipe
	adds	-2,r21,r21	    // Decrement for first and last
                                    // blocks
	bc	.quadsdL2	    // If only one block, branch
	bla	r30,r21,.quadsdL1   // Prime the bla logic
         SET_PIPE                   // Set pipe mode if user bcopy
// 
//      Pipelined copy loop for double-aligned source and 
//      quad-aligned destination.
//
.quadsdL1:
	pfld.d	8(r16)++,f16	    // Load two words'
	pfld.d	8(r16)++,f18	    // Load next two words
	pfld.d	8(r16)++,f20	    // Load next two words
	pfld.d	8(r16)++,f22	    // Load next two words
	pfld.d	8(r16)++,f24	    // Load next two words
	pfld.d	8(r16)++,f26	    // Load next two words
	pfld.d	8(r16)++,f28	    // Load next two words
	pfld.d	8(r16)++,f30	    // Load last two words
	fst.q	f16,16(r17)++	    // Store four words
	fst.q	f20,16(r17)++	    // Store another four words
	fst.q	f24,16(r17)++	    // Store another four words
	bla	r30,r21,.quadsdL1   // Loop until last block
	 fst.q	f28,16(r17)++	    // Store last four in delay slot
// 
//      Special code to deal with the last block 
//
.quadsdL2:
        RESET_PIPE                  // Reset pipe control
	pfld.d	8(r16)++,f16	    // Load two words
	pfld.d	8(r16)++,f18	    // Load next two words
	pfld.d	8(r16)++,f20	    // Load next two words
	pfld.d	8(r16)++,f22	    // Load next two words
	pfld.d	8(r16)++,f24	    // Load next two words
        pfld.d  0(r16),f26	    // Empty the pipeline 
        pfld.d  0(r16),f28	    // Empty the pipeline 
        pfld.d  0(r16),f30	    // Empty the pipeline 
	fst.q	f16,16(r17)++	    // Store four words
	fst.q	f20,16(r17)++	    // Store another four words
	fst.q	f24,16(r17)++	    // Store another four words
	fst.q	f28,16(r17)++	    // Store another four words
//
// 	Done with quads.  See what else
//
	and	63,r18,r18	    // Check the remnants
	bnc	.dqremP8	    // Branch if something to do
        RET_OK                      // Return otherwise
//   
//      Loop Initialization for word-aligned source and quad-aligned
//      destination with pipelining.
//
.quadsw:
        pfld.l  0(r16),f16	    // Prime the load pipe
	addu   -16,r17,r17	    // Adjust pointer for displacement
	pfld.l  4(r16)++,f16        // Prime the load pipe
	pfld.l  4(r16)++,f16        // Prime the load pipe
	adds	-2,r21,r21	    // Decrement for first and last
                                    // blocks
	bc	.quadswL2	    // If only one block, branch
	bla	r30,r21,.quadswL1   // Prime the bla logic
         SET_PIPE                   // Set pipe mode if user bcopy
// 
// 
//      Pipelined copy loop for word-aligned source and quad-aligned
//      destination.
//
.quadswL1:
	pfld.l	4(r16)++,f16	    // Load a word
	pfld.l	4(r16)++,f17	    // Load a word
	pfld.l	4(r16)++,f18	    // Load a word
	pfld.l	4(r16)++,f19	    // Load a word
	pfld.l	4(r16)++,f20	    // Load a word
	pfld.l	4(r16)++,f21	    // Load a word
	pfld.l	4(r16)++,f22	    // Load a word
	pfld.l	4(r16)++,f23	    // Load a word
	pfld.l	4(r16)++,f24	    // Load a word
	pfld.l	4(r16)++,f25	    // Load a word
	pfld.l	4(r16)++,f26	    // Load a word
	pfld.l	4(r16)++,f27	    // Load a word
	pfld.l	4(r16)++,f28	    // Load a word
	pfld.l	4(r16)++,f29	    // Load a word
	pfld.l	4(r16)++,f30	    // Load a word
	pfld.l	4(r16)++,f31	    // Load a word
	fst.q	f16,16(r17)++	    // Store four words
	fst.q	f20,16(r17)++	    // Store another four words
	fst.q	f24,16(r17)++	    // Store another four words
	bla	r30,r21,.quadswL1    // Loop until last block
	 fst.q	f28,16(r17)++	    // Store last four in delay slot
// 
//      Special code to deal with the last block 
//
.quadswL2:
	pfld.l	4(r16)++,f16	    // Load a word
	pfld.l	4(r16)++,f17	    // Load a word
	pfld.l	4(r16)++,f18	    // Load a word
	pfld.l	4(r16)++,f19	    // Load a word
	pfld.l	4(r16)++,f20	    // Load a word
	pfld.l	4(r16)++,f21	    // Load a word
	pfld.l	4(r16)++,f22	    // Load a word
	pfld.l	4(r16)++,f23	    // Load a word
	pfld.l	4(r16)++,f24	    // Load a word
	pfld.l	4(r16)++,f25	    // Load a word
	pfld.l	4(r16)++,f26	    // Load a word
	pfld.l	4(r16)++,f27	    // Load a word
	pfld.l	4(r16)++,f28	    // Load a word
        pfld.l  0(r16),f29	    // Empty the pipeline 
        pfld.l  0(r16),f30	    // Empty the pipeline 
        pfld.l  0(r16),f31	    // Empty the pipeline 
	fst.q	f16,16(r17)++	    // Store four words
	fst.q	f20,16(r17)++	    // Store another four words
	fst.q	f24,16(r17)++	    // Store another four words
	fst.q	f28,16(r17)++	    // Store another four words
//
// 	Done with quads.  See what else
//
        RESET_PIPE                  // Reset pipe control
	and	63,r18,r18	    // Check the remnants
	bnc	.dqremP4	    // Branch if something to do
        RET_OK                      // Return otherwise
	 
//
//      Source and destination are quad-aligned
//
.qboth:
        bte     r0,r21,.dqremonly   // If we don't have one loop,
                                    // go move remnants
#ifdef _USER_BCOPY_
	mov	0x0446,r20	    // Move in loop control info
        mov     r16,r23             // Save starting point
#endif /* _USER_BCOPY_ */
        ld.c    epsr,r31            // Fetch the epsr
        and     2,r31,r0            // Is this an XP?
        bc     .quadsd              // Branch if not so no pfld.q's

//   
//      Loop Initialization for pipelined quad copy.
// 
.quadsq:
        pfld.q 0(r16),f16	    // Prime the load pipe
	addu   -16,r17,r17	    // Adjust pointer for displacement
	pfld.q  16(r16)++,f16       // Prime the load pipe
	pfld.q  16(r16)++,f16       // Prime the load pipe
	adds	-2,r21,r21	    // Decrement for first and last
                                    // blocks
	bc	.quadsqL2	    // If only one block, branch
	bla	r30,r21,.quadsqL1   // Prime the bla logic
         SET_PIPE                   // Set pipe mode if user bcopy
// 
//      Copy loop for quad-aligned data using pipelining.  This
//      loop uses pfld.q's and so will not work on the i860XR.
//
.quadsqL1:
	pfld.q	16(r16)++,f16	    // Load four words
	pfld.q	16(r16)++,f20	    // Load another four words
	pfld.q	16(r16)++,f24	    // Load another four words
	pfld.q	16(r16)++,f28	    // Load another four words
	fst.q	f16,16(r17)++	    // Store four words
	fst.q	f20,16(r17)++	    // Store another four words
	fst.q	f24,16(r17)++	    // Store another four words
	bla	r30,r21,.quadsqL1   // Loop until last block
	 fst.q	f28,16(r17)++	    // Store last four in delay slot
// 
//      Special code to deal with the last block.  Use pfld.d to
//      drain the pipeline to avoid clobbering f2 and f3 when the
//      pipe is being primed by code not suspecting quads are in
//      the pipe.
//
.quadsqL2:
        RESET_PIPE                  // Reset pipe control
	pfld.q	16(r16)++,f16	    // Load four words
        pfld.d  0(r16),f20	    // Empty the pipeline 
        pfld.d  0(r16),f24	    // Empty the pipeline 
        pfld.d  0(r16),f28	    // Empty the pipeline 
	fst.q	f16,16(r17)++	    // Store four words
	fst.q	f20,16(r17)++	    // Store another four words
	fst.q	f24,16(r17)++	    // Store another four words
	fst.q	f28,16(r17)++	    // Store another four words
//
// 	Done with quads.  See what else
//
	and	63,r18,r18	    // Check the remnants
	bnc	.dqremP16	    // Branch if something to do
        RET_OK                      // Return otherwise

//
//      Deal with the only thing being remnants
//
.dqremonly:
        and     48,r18,r31          // See if any quads left
        bc     .worb                // Branch if not
#ifdef _USER_BCOPY_
        mov     r16,r23             // Save starting point
        mov     0x444,r20           // Set new loop control
#endif /* _USER_BCOPY_ */
        br      .finalq             // Go move some quads
         addu   -16,r17,r17         // Adjust destination
//
//	Fix up source pointer before dealing with remnants.
//
.dqremP16:
        br      .dqrem              // Branch to common code
         addu   16,r16,r16          // Advance source by a quad
.dqremP8:
        addu    4,r16,r16           // Advance source by a word
.dqremP4:
        addu    4,r16,r16           // Advance source by a word
//
//	Deal with remnants
//
.dqrem:
        and     48,r18,r31          // See if any quads left
        bc      .wbrem              // Branch if not
#ifdef _USER_BCOPY_
        mov     0x444,r20           // Set new loop control
#endif /* _USER_BCOPY_ */
//
//      Set up move final quads
//
.finalq:
        SHRLEN  4,r18,r21           // See how many quads
	adds	-1,r21,r21	    // Decrement count for bla
	bla	r30,r21,.lquadsL1   // Prime the bla
         nop                        // Nothing to do in delay slot
//
//      Loop to move words to quads.  We only assume word-alignment
//      of the source and do not have separate cases since we are
//      going to take cache misses to access the data in any case
//      and the additional cost of the fld.l approach to loads will
//      then be small.
//
.lquadsL1:
        fld.l   0(r16),f16          // Load first word of group
        fld.l   4(r16),f17          // Load second word of group
        fld.l   8(r16),f18          // Load third word of group
        fld.l   12(r16),f19         // Load the first word
        addu    16,r16,r16          // Advance source pointer
	bla	r30,r21,.lquadsL1   // Loop until last block
         fst.q  f16,16(r17)++       // Store all four words
//
//      Done with all quads.  Do words and bytes
//
.wbrem:
        addu    16,r17,r17          // Unadjust destination
        and     15,r18,r18          // Isolate non-quad length 
#ifdef _USER_BCOPY_
	mov	0,r20		    // Set up for ordinary moves
#endif /* _USER_BCOPY_ */
//
//      Move words and/or bytes
//
.worb:
	and	12,r18,r31	    // See if full words
	bnc.t	.words		    // Branch if there are
	 SHRLEN	2,r31,r21           // After fixing count
//
//      Initialize loop to move final or all bytes.  Decrement  
//      count and prime the bla logic.  
//
.lbytes_or_none:
        bte     r0,r18,.return      // Branch if nothing to move
.lbytes:
	adds	-1,r18,r18	    // Decrement count for bla
	bla	r30,r18,.lbytesL1   // Prime the bla
         nop                        // Nothing to do in delay slot
//
//      Basic byte copy loop.  
//
.lbytesL1:
	ld.b	0(r16),r29	    // Load the next byte
        addu	1,r16,r16	    // Bump the input pointer
	st.b	r29,0(r17)	    // Store the current byte
	bla	r30,r18,.lbytesL1   // Loop until done
	 addu	1,r17,r17	    // Bump the output pointer
//
//      Finish up 
//
.return: 
        RET_OK                      // Return indicating success
//
//	Special code for short transfers
//
.bshort:	
	btne	r18,r0,.lbytes	    // Branch if something to do
        RET_OK                      // Return indicating success if
                                    // nothing to do
//
//      Initialize the move-words loop.  Decrement count and 
//	prime the bla logic.  
//
.words:
	adds	-1,r21,r21	    // Decrement count for bla
	bla	r30,r21,.wordsL1    // Prime the bla
         nop                        // Nothing to do in delay slot
//
//      Basic word copy loop.  
//
.wordsL1:
	ld.l	0(r16),r25	    // Load the next word
        addu	4,r16,r16	    // Bump the input pointer
	st.l	r25,0(r17)	    // Store the current word
	bla	r30,r21,.wordsL1    // Loop until done
	 addu	4,r17,r17	    // Bump the output pointer
//  
//      Copy odd bytes at the end
// 
	and	3,r18,r18	    // Any odd bytes?
	bnc	.lbytes             // Branch if so
        RET_OK                      // Return indicating success
//
//	Deal with operands whose word-alignment is not congruent
//
.misalign:
	andnot	31,r18,r0	    // Do we have at least 32 bytes?
	bc	.lbytes_or_none     // Do simple copy if not
	and	3,r17,r0 	    // Is destination aligned?
	bc	.mawords	    // Branch if so
//                                           
//      Non-word alignment.  Move bytes until aligned. 
//
.mafixL1:
	ld.b	0(r16),r25	    // Load the next byte
        addu	1,r16,r16	    // Bump the input pointer
	st.b	r25,0(r17)	    // Store the current byte
	addu	1,r17,r17	    // Bump the output pointer
	and	3,r17,r0	    // Are we aligned?
	bnc.t	.mafixL1	    // If not, keep going
	 adds	r30,r18,r18	    // Decrement the count
	adds	r30,r18,r18	    // Final decrement of count
//
//	Set up for copy of shifted words
//
.mawords:
#ifdef _USER_BCOPY_
        mov     r16,r23             // Save starting point
#endif /* _USER_BCOPY_ */
        and     3,r16,r22           // Compute alignment
	andnot	3,r16,r16	    // Now align the source
	ld.l	0(r16),r29	    // Load the first word
#ifdef _USER_BCOPY_
	mov	0x0024,r20	    // Set loop control
#endif /* _USER_BCOPY_ */
        addu    4,r16,r16           // Bump to next word
	shl	3,r22,r25	    // Compute bit shift
	SHRLEN	4,r18,r21	    // Compute the loop count
	adds	-1,r21,r21	    // Decrement for last block
	bla	r30,r21,.mawordsL1  // Prime the bla logic
 	 shr	r25,r0,r0	    // Set the SC register
//
//	Do shifted copy sixteen bytes at a time.
//
.mawordsL1:
	ld.l	0(r16),r25	    // Load a word	
	ld.l	4(r16),r26	    // Load a word	
	ld.l	8(r16),r27	    // Load a word	
	ld.l	12(r16),r28	    // Load a word
	addu	16,r16,r16	    // Advance source pointer
	shrd	r25,r29,r31	    // Form first word
	shrd    r26,r25,r25	    // Form second word
	shrd    r27,r26,r26	    // Form third word
	shrd	r28,r27,r27	    // Form fourth word	
	st.l	r31,0(r17)	    // Store a word
	st.l	r25,4(r17)	    // Store a word
	st.l	r26,8(r17)	    // Store a word
	st.l	r27,12(r17)	    // Store a word
	addu	16,r17,r17	    // Advance destination pointer
	bla	r30,r21,.mawordsL1  // Loop until last block
	 mov	r28,r29		    // Save remnants for next time
//
//	See if there is anything left
//
        adds    -4,r16,r16          // Back up one word
	adds	r16,r22,r16	    // Readjust source
	and	15,r18,r18	    // See what is left
        bnc     .lbytes             // Branch if something to do
        RET_OK                      // Return indicating success
	 
#endif /* _BCOPY_ || _USER_BCOPY_ */

#ifdef _USER_BCOPY_

//
//      Deal with address wraparound in user_bcopy.
//
.ubc_wrap:
        bri	r1		    // Return to caller
         mov    0,r16               // Indicate failure

//
//      Deal with address wraparound in user_bcopy2.
//
.ubc2_wrap:
        ld.b    0(r16),r25          // Load the next byte
        addu    1,r16,r16           // Bump input pointer
        st.b    r25,0(r17)          // Store the current byte
        addu    1,r17,r17           // Bump the output pointer
        addu    -1,r18,r18          // Subtract one from length
        bte     r0,r16,.ubc2w_done  // Branch if we have wrapped
        bte     r0,r17,.ubc2w_done  // Branch if we have wrapped        
        btne    r0,r18,.ubc2_wrap   // Loop if more to do
//
//      Finished copy with no fault
//
.ubc2w_done:
	subu	r17,r24,r18	    // Compute amount moved
        mov     EFAULT,r16          // Set code for return 
	bri	r1		    // Return to caller
	 st.l	r18,0(r19)	    // Store actual count

//
//      Do recopy to resolve data fault in middle of unwound loop
//
.recopy:
	adds	-1,r18,r18	    // Decrement count for bla
	bla	r30,r18,.recopyL1   // Prime the bla
         mov     0,r20              // This is a simple loop
//
//      Do the copy until a fault occurs
//
.recopyL1:
	ld.b	0(r16),r29	    // Load the next byte
        addu	1,r16,r16	    // Bump the input pointer
	st.b	r29,0(r17)	    // Store the current byte
	bla	r30,r18,.recopyL1   // Loop until done
	 addu	1,r17,r17	    // Bump the output pointer
//
//      If a fault did not occur, it must have been due to another
//      thread unmapping and then mapping a region.  In this case,
//      the fault could just as well have happened at the end of 
//      the loop and this is what the user_bcopy2 return will 
//      indicate and it is certainly consistent with what was 
//      actually moved.  
//
        br      .esimple            // Report fault at end of loop
         mov    r26,r16             // Restore the error code


//
//	Marks the end of code that access user address.  See 
//      _uacc_start above.
//
_uacc_end::

//
//	Error return after exception when accessing user memory.
//
//      At entry:
//           r16 -- Error code
//           r17 -- Current output pointer (may be off due to
//                  adjustment for auto-increment)
//	     r19 -- Pointer to output counter or one.
//           r20 -- Current loop control info
//
_uacc_err::
        andnot  1,r19,r0            // Check for length pointer
        bc      .ret0               // If none, go return 0
	bte	r0,r20,.esimple	    // Branch if nothing fancy
//
//      Adjust the destination pointer
//
        and     0xf00,r20,r31       // Isolate the shift 
        bc      .getlen             // If none, go compute the length
        shr     8,r31,r31           // Shift the shift down
        mov     1,r25               // Start with a one
        shl     r31,r25,r31         // Get the displacement
        addu    r31,r17,r17         // Adjust the destination
//
//      Compute the length to do in the recopy
//
.getlen:
        and     0xf,r20,r31         // Isolate the log2
        mov     1,r18               // Start with one
        shl     r31,r18,r18         // Compute one loop interation
        and     0x1000,r20,r0       // Test for pipelining
        bc      .getptr             // Branch if not
//
//      Special code for pipeline case.  Make sure no quads.
//
        shl     1,r18,r18           // Do two loops worth of data
        orh     ha%zero,r0,r28      // Start to build pointer
        pfld.d  l%zero(r28),f16     // Drain the pipe in case
        pfld.d  l%zero(r28),f16     // Drain the pipe in case
        pfld.d  l%zero(r28),f16     // Drain the pipe in case
//
//      Get the pointer and recopy the loop iteration that failed
//      byte by byte.
//
.getptr:
        and     0xf0,r20,r28        // Isolate the log2
        shr     4,r28,r28           // Shift the log2 down
        mov     1,r29               // Start with one
        shl     r28,r29,r29         // Compute the amount
        adds    -1,r29,r29          // Get a mask
        addu    r29,r24,r25         // Bump original destination 
        andnot  r29,r25,r25         // And for round up to quad
        subu    r17,r25,r27         // See how far we have gotten
        shr     r31,r27,r27         // Shift right for round down
        shl     r31,r27,r27         // Shift left for round down
        mov     r16,r26             // Save the error code
        addu    r23,r27,r16         // Advance source to start of loop
        br      .recopy             // No try the recopy
         addu    r25,r27,r17        // Advance destination to start of
                                    // loop
//
//	Return count for the simple case
//
.esimple:
	subu	r17,r24,r18	    // Compute amount moved
	bri	r1		    // Return to caller
	 st.l	r18,0(r19)	    // Store actual count

.ret0:
	bri	r1		    // Return to caller
	 mov	r0,r16		    // Indicate failure

        .data
zero:   .double 0.0
        .text 

#endif /* _USER_BCOPY_ */	    









