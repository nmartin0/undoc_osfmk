/*
 *
 *$Copyright
 *Copyright 1993, 1994 , 1995 Intel Corporation
 *INTEL CONFIDENTIAL
 *The technical data and computer software contained herein are subject
 *to the copyright notices; trademarks; and use and disclosure
 *restrictions identified in the file located in /etc/copyright on
 *this system.
 *Copyright$
 *
 */
 
/*
 * @OSF_COPYRIGHT@
 */
/*
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log: user_copy.s,v $
 *Revision 1.9  1995/03/09  01:24:00  stans
 * user_strlen(); changed the comment for detection of <null> string pointer
 * to reflect the error returned.
 *
 * Reviewer:yazz,jlitvin
 * Risk:low
 * Benefit or PTS #:12606
 *
 *Revision 1.8  1995/03/08  21:44:48  stans
 * Test for <null> string pointer arg passed into user_strlen(). Prevents
 * syscalls such as unlink(0) causing an emulator exception.
 *
 * Reviewer:terry,jlitvin
 * Risk:medium
 * Benefit or PTS #:12606
 * Testing:WW09 sats and developer tests.
 *
 *Revision 1.7  1994/11/18  20:25:49  mtm
 *Copyright additions/changes
 *
 *Revision 1.6  1994/03/31  23:03:18  jlitvin
 *Have the emulator check system call pointers vs. page 0 addresses.
 *Valid pointers will actually be much larger, but this will catch most
 *user mistakes of passing a pointer that contains too many leading zeroes.
 *
 * Reviewer: cfj
 * Risk: low
 * Benefit or PTS #: 8723
 * Testing: pts & boundary conditions
 * Module(s): emulator/i860/user_copy.s
 *
 *Revision 1.5  1993/09/21  20:06:05  cfj
 *Pick up dnoveck@osf.org i860 bcopy enhancements from ad1.0.5
 *
 * Revision 2.9  93/06/16  13:55:02  dnoveck
 * 	Converted to use i860_copy.s.h for high-speed copies.
 * 	Convert user_bcopy(2), user_strlen, user_r(w)check(2) to use a 
 * 	linear addressing model treating wraparound as a fault.
 * 	Fix numerous bugs in user_r(w)check(2) routines:
 * 	     user_r(w)check2 returns length one too short on fetch fault.
 * 	     user_r(w)check did not check the second page when the area
 * 	     was less than a page and crossed a page boundary.
 * 	     user_r(w)check2 routines were inconsistent about the return
 * 	     value.
 * 	     user_rwcheck2 did not try to store in the first page.
 * 	Cleanup and fix comments.
 * 
 * Revision 2.8  92/09/11  09:26:04  rabii
 * 	Fixed user_bcopy2 and uacc_err routines. Added user_rwcheck2 and
 * 	user_rcheck2 to provides counts of valid data areas.
 * 	[92/09/04            sjs]
 * 
 * Revision 2.7  92/07/29  08:52:03  rabii
 * 	Added user_bcopy2 (see comments below).
 * 
 * Revision 2.6  92/07/15  11:05:51  rabii
 * 	Fix from Locus to handle addr,len wrapping around 0 in
 * 	user_rcheck and user_rwcheck; also, user_rwcheck contained
 * 	erroneous branches to user_rcheck!
 * 
 * Revision 2.5  92/06/09  16:39:06  pjg
 * 	Fix typos (sjs).
 * 
 * Revision 2.4  92/06/08  18:17:36  pjg
 * 	Faster versions (sjs)
 * 
 * Revision 2.3  92/06/01  15:03:56  loverso
 * 	Fixed user_strlen; test comparision was wrong!
 * 
 * Revision 2.2  92/05/31  18:57:57  loverso
 * 	Created i860 version by crafting appropriate C code and
 * 	saving assembler output, following by hand fitting as appropriate.
 * 	These are probably anything but optimal.
 * 	$EndLog$
 * 
 */

#include <i860/asm.h>
#include <sys/errno.h>

/*
 * Functions for copying data between the emulator and the user, and for
 * checking access to user data, with fault recovery.  This is the analog
 * of copyin/copyout in an integrated kernel.
 *
 * These functions fall into three classes:
 *
 *      Returns one on success and zero on failure, no length returned:
 *       
 *          user_bcopy -- Copy with checks for faults
 *          user_rcheck -- Checks for read access to specified area.
 *          user_rwcheck -- Checks for read-write access to specified
 *                          area.
 *
 *      Returns one on success and zero on failure, with a length 
 *      returned in user parameter on success:
 *
 *          user_strlen -- Check for length of string upto zero-byte.
 *
 *      Returns zero on success and error number on failure with
 *      the length gotten from/returned-to user parameter on both
 *      success or failure.
 *
 *          user_bcopy2 -- Copy with checks for faults
 *          user_rcheck2 -- Check for read access to specified area.
 *          user_rwcheck2 -- Check for read-write access to specified
 *                           area.
 *
 * In all cases, lengths are treated as unsigned.  Addresses do not
 * wrap around to zero.  Going past 0xffffffff results in a fault
 * with error code EFAULT.
 */



/*
 * Marks the starting address of functions that can fault on access to user
 * addresses.  Any exception between here and uacc_end will result in the
 * server branching us to the common error recovery point, uacc_err.
 *
 * THUS, it is extremely important that all routines follow the conventions
 * defined in i860_copy.s.h.
 */
ENTRY(uacc_start)


//	int user_strlen		    Returns one if OK, zero if fault
//          (char * str             Start of string to get length of
//           unsigned * len_p)      Gets resulting length if no fault
//
// This version of strlen must be used whenever accessing user addresses.
// On a fault, uacc_err will return zero on our behalf.

ENTRY(user_strlen)
	mov	r0,r19                  // we want zero return on fault
	or 	r16,r0,r18
	bc	strlenwrap		// <null> pointer? return an error.
	bte	r17,r0,strlenwrap	// <null> len pointer? error out.
//
//      Scan for a zero-byte.  Note that we must find one if we don't
//      fault since the ld.b at the head of the loop has a zero-byte
//      in its representation.
//
strlenloop:
 	ld.b	r0(r18),r28
 	addu	1,r18,r18
        bc      strlenwrap              // branch if address wrapped
 	btne	r0,r28,strlenloop 
//
//      Figure out how far we had to go and return success indication.
//      Note that in the wraparound case, the length can be negative.
//
	subu	r18,r16,r18		// get the delta
	adds	-1,r18,r18		// subtract out the NULL
	st.l	r18,0(r17)		// store the length
 	bri	r1			// return
         mov    1,r16                   // success

//
//      Deal with address swaparound
//
strlenwrap:
 	bri	r1			// return
         mov    0,r16                   // failure
 

//	int user_rcheck		    Returns one if OK, zero if fault
//          (char * cp              Start of area to check
//           unsigned len)          Length of area to check
//
// 
// Checks for read access to a specified area of memory.  On a fault, 
// uacc_err will return zero on our behalf.

ENTRY(user_rcheck)
	mov	r0,r19                  // indicate return zero on fault
	mov	0x01fff,r20		// page 0 limit
	subu	r20,r16,r0		// branch if we are too small
	bc	rcheckwrap		// is this address in page 0?
	bte	r17,r0,rcheckdone	// check for 0 length
 	orh	ha%_vm_page_size,r0,r31 // get value into r31 for address
 	ld.l	l%_vm_page_size(r31),r28 // r28 has vm_page_size
//
//      Make sure there is no wraparound and that we can access last 
//      byte in area.
//
        addu    -1,r17,r17              // get length minus one
 	addu	r17,r16,r20		// r20 now has end
        bc      rcheckwrap              // branch if wraparound
        ld.b    0(r20),r30              // check for fault
//
//      Check for access to each page.
//
rcheckloop:
	subu	r20,r16,r0              // check address against end
	bnc	rcheckdone              // branch if we have hit end
	ld.b	r0(r16),r18		// load the test-fault byte
	br	rcheckloop              // go do next page
 	 addu	r28,r16,r16		// inc to next page
//
//      We survived.  Tell the caller.
//
rcheckdone:
 	bri	r1                      // return
         mov    1,r16                   // success
//
//      The address wrapped.  Tell the caller.
//
rcheckwrap:
 	bri	r1                      // return
         mov    0,r16                   // failure

//	int user_rwcheck	    Returns one if OK, zero if fault
//          (char * cp              Start of area to check
//           unsigned len)          Length of area to check
//
// 
// Checks for read and write access to a specified area of memory.  On a 
// fault, uacc_err will return zero on our behalf.

ENTRY(user_rwcheck)
	mov	r0,r19                  // indicate return zero on fault
	mov	0x01fff,r20		// page 0 limit
	subu	r20,r16,r0		// branch if we are too small
	bc	rwcheckwrap		// is this address in page 0?
	bte	r17,r0,rwcheckdone	// check for 0 length
 	orh	ha%_vm_page_size,r0,r31 // get value into r31 for address
 	ld.l	l%_vm_page_size(r31),r28 // r28 has vm_page_size
//
//      Make sure there is no fault accessing last byte in area.
//
        addu    -1,r17,r17              // get length minus one
 	addu	r17,r16,r20		// r20 now has end
        bc      rwcheckwrap             // branch if wraparound
        ld.b    0(r20),r31              // check for read fault
        st.b    r31,0(r20)              // check for store fault
//
//      Check for access to each page.
//
rwcheckloop:
	subu	r20,r16,r0              // check address against end
	bnc	rwcheckdone             // branch if we have hit end
	ld.b	r0(r16),r18		// load the test-fault byte
	st.b	r18,0(r16)		// store test byte
	br	rwcheckloop             // go do next page
 	 addu	r28,r16,r16		// inc to next page
//
//      We survived.  Tell the caller.
//
rwcheckdone:
 	bri	r1                      // return
         mov    1,r16                   // success
//
//      The address wrapped.  Tell the caller.
//
rwcheckwrap:
 	bri	r1                      // return
         mov    0,r16                   // failure

//	int user_rcheck2	    Returns zero if OK, non-zero fault
//                                  code otherwise
//          (char * cp              Start of area to check
//           unsigned * len_p)      Length of area to check which is
//                                  updated to length accessed success-
//                                  fully in the fault case
//
// 
// Checks for read access to a specified area of memory.  On a fault, 
// uacc_err will return the fault code and set the length on our behalf.

ENTRY(user_rcheck2)
	mov	r17,r19			// save count pointer
        mov     0,r20                   // indicate no special loop control
	ld.l	0(r17),r18		// get the count
	mov	r16,r24			// save out the base pointer
	bte	r18,r0,rcheck2done	// check for 0 length
 	addu	r18,r16,r29		// r29 now has end
//
//      Try to access the first byte.
//
	mov	r16,r17			// use r17 (for error processing)
	ld.b	r0(r17),r30		// load the test-fault byte
//
//      If the first byte is valid, we want to set the pointer to
//      the start of the next page.  If this is beyond the length to
//      check we are done.
//
 	orh	ha%_vm_page_size,r0,r31 // value into r31 for addressing
 	ld.l	l%_vm_page_size(r31),r28 // r28 has vm_page_size
	addu	r17,r28,r17		// advance to next page
	adds	-1,r28,r30              // get a page mask
	andnot	r30,r17,r17             // round down to start of page
	subu	r17,r24,r30		// check count in same page
	subu	r30,r18,r0              // have gone further than count
	bc	rcheck2done		// branch if in same page
//
//      Check each page for valid access.
//
rcheck2loop:
        bte     r0,r17,rcheck2wrap      // branch if we wrapped to zero
	subu	r17,r29,r0              // have we hit then end?
	bc	rcheck2done             // branch if so
	ld.b	0(r17),r18		// load the test-fault byte
	br	rcheck2loop             // go do next page
 	 addu	r28,r17,r17		// inc to next page
//
//      We survived.  Tell the caller.
//
rcheck2done:
 	bri	r1                      // return
         mov    0,r16                   // no error code
//
//      The address wrapped.  Tell the caller.
//
rcheck2wrap:
 	bri	r1                      // return
         mov    EFAULT,r16              // error code


//	int user_rwcheck2	    Returns zero if OK, non-zero fault
//                                  code otherwise
//          (char * cp              Start of area to check
//           unsigned * len_p)      Length of area to check which is
//                                  updated to length accessed success-
//                                  fully in the fault case
//
// 
// Checks for read and write access to a specified area of memory.  On  
// a fault, uacc_err will return the fault code and set the length on our 
// behalf.

ENTRY(user_rwcheck2)
	mov	r17,r19			// save count pointer
        mov     0,r20                   // indicate no special loop ctl
	ld.l	0(r17),r18		// get the count
	mov	r16,r24			// save out the base pointer
	bte	r18,r0,rwcheck2done	// check for 0 length
 	addu	r18,r16,r29		// r29 now has end
//
//      Try to access the first byte.
//
	mov	r16,r17			// use r17 (for error processing)
	ld.b	r0(r17),r31		// load the test-fault byte
        st.b    r31,0(r17)              // store test byte
//
//      If the first byte is valid, we want to set the pointer to
//      the start of the next page.  If this is beyond the length to
//      check we are done.
//
 	orh	ha%_vm_page_size,r0,r31 // value into r31 for addressing
 	ld.l	l%_vm_page_size(r31),r28 // r28 has vm_page_size
	addu	r17,r28,r17		// advance to next page
	adds	-1,r28,r30              // get a page mask
	andnot	r30,r17,r17             // round down to start of page
	subu	r17,r24,r30		// check count in same page
	subu	r30,r18,r0              // have gone further than count
	bc	rwcheck2done		// branch if in same page
//
//      Check each page for valid access.
//
rwcheck2loop:
        bte     r0,r17,rwcheck2wrap     // branch if we wrapped to zero
	subu	r17,r29,r0              // have we hit then end?
	bc	rwcheck2done            // branch if so
	ld.b	0(r17),r18		// load the test-fault byte
	st.b	r18,0(r17)		// store test byte
	br	rwcheck2loop            // go do next page
 	 addu	r28,r17,r17		// inc to next page
//
//      We survived.  Tell the caller.
//
rwcheck2done:
 	bri	r1
         mov    0, r16                  // no error code
//
//      The address wrapped.  Tell the caller.
//
rwcheck2wrap:
 	bri	r1                      // return
         mov    EFAULT,r16              // error code

 
#define _USER_BCOPY_
#include <i860/i860_copy.s.h>

