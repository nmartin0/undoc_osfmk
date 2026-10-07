; $Copyright
; Copyright 1992, 1993, 1994, 1995 Intel Corporation
; INTEL CONFIDENTIAL
; The technical data and computer software contained herein are subject
; to the copyright notices; trademarks; and use and disclosure
; restrictions identified in the file located in /etc/copyright on
; this system.
; Copyright$
; scsi16.ss:  NCR 53c720 scripts source for the Intel SSD Scsi-16 board
; consists of:	(1) test modules to check that the request table in CLRAM
;                   is not corrupted
;
; Written Apr-May 1994 Wally Kramer, Step Technology, Portland, Oregon
;
; Purpose:
;	This code performs the specific functions of a "scsi initiator" on
;	behalf of a host processor.  Host I/O requests are translated into
;	command, message_out, data_in or data_out, message_in, status
;	sequences to accomplish the requested I/O.  When I/O is complete,
;	the host is signaled so it can perform the complete_io portion of
;	the driver.
;
; Conventions:
;	Generally, instructions changed by runtime modification "patching"
;	(not to be confused with what NCR calls "patching" which is really
;	load time relocation) have the PATCH symbol used in the instruction
;	as a place holder.  Generally such an instruction has a label of
;	the form xxx_n where xxx is describes the functional procedure as a
;	unique but suggestive name and n is a digit, always increasing down
;	the file.  This saves creative effort and helps identify where
;	patches are done.
;
;	??? indicates things which may need further research or testing
;	to determine validity or appropriateness.  So far, these have all
;	been NCR questions.

;	!! indicates code which was changed to allow debugging.
;
; Modifications:
;	27 June 1994, Wally Kramer
; Since 21 June, added sigp bit clear instruction in schedular; added lots of
; assert statements (presently commented), added LED control; moved assign of
; req_tbl_ptr to before selection for selection timeout host error processing.
;
;	29 June 1994, Wally Kramer
; Added write of req_tbl_ptr into req_done_q in io_complete.
;
;	30 June 1994, Wally Kramer
; Removed debugger stuff
;
;	26 July 1994, Jerrie Coffman and Vineet Kumar
; Clean up more debugger stuff that Wally left in by mistake.
; Added more asserts.
;
;	11 August 1994, Jerrie Coffman
; Using absolutes for bss label accesses does not produce releative
; addressing in the output code.  Replaced absolute names with bss?+n
; lables to allow code to be relocatable.  Updated and cleaned up
; some comments.
;
;	23 August 1994, Jerrie Coffman and Vineet Kumar
; A selection does not halt the script processor.  The script processor
; will continue running until another instruction is executed which
; requires the SCSI bus or a select timeout occurs.  Added a "jump when"
; instruction following the selection to force a pause until the selection
; phase is complete.
;
;	18 November 1994, Jerrie Coffman
; Corrected a coding error in save data pointers following a phase mismatch.
; The calulation for the new scatter/gather entry length and address fields
; was incorrect.  They were being adjusted by the amount remaining to be
; transferred instead of the amount that was just transferred. 
;
;	7 December 1994, Jerrie Coffman
; Added a global labels "msg_sent" and "cmd_sent" placed immediately after
; the SCSI message out and command transfer instructions.  These can be used
; by the interrupt handler to detect a phase mismatch during message out or
; command phase and recover by simply restarting the script from the current
; location.
;
;	11 March 1995, Jerrie Coffman
; Added jump to bad_error if actual phase does not match expected phase
; following a selection.

	arch	720		; 53c720

;absolute dsa      = 0x10 + REG_BASE	; DSA (data structure address) register
;absolute scratcha = 0x34 + REG_BASE	; SCRATCH A register
;absolute scratchb = 0x5c + REG_BASE	; SCRATCH B register
absolute BYTE_LANE_1	= 1
absolute BYTE_LANE_2	= 2
absolute BYTE_LANE_3	= 3

absolute PATCH = 0		; visual place holder for patched locations

; These values from ...Kernel/src/mk/kernel/scsi/adapters/scsi_53C720.h

absolute ERROR_STAT_NOT_GOOD	= 1
absolute ERROR_REQ_DONE_Q_FULL	= 2
absolute ERROR_UNKNOWN_PHASE	= 3
absolute ERROR_RESELECT_ID	= 4
absolute ERROR_OTHER_MSG	= 5	; not an error, but handled like one
absolute ERROR_WRONG_PHASE	= 6


; Constants for gpreg to make blinkenlightsgo.

absolute LED_OFF	= 0x18
absolute LED_GREEN	= 0x10
absolute LED_RED	= 8
absolute LED_RED_GREEN	= 0	; both on


; For host-based debugging support:  a collection of identifiers present
; during an interrupt for the host to verify 720 conditions at strategic
; points.

absolute DEBUG_INIT_ENTRY		= 0xc0de0065
absolute DEBUG_INIT_EXIT		= 0xc0de0066	; initialization done
absolute DEBUG_SCHEDULAR_ENTRY		= 0xc0de0067
absolute DEBUG_SCHEDULE_READY		= 0xc0de0068	; schedular has entry
absolute DEBUG_SELECT_ATN		= 0xc0de0069
absolute DEBUG_SELECT_NO_ATN		= 0xc0de006a
absolute DEBUG_CNTRLR_Q_ZAP		= 0xc0de006b	; sched to clear entry
absolute DEBUG_CNTRLR_Q_INC_DONE	= 0xc0de006c	; ++cntrlr_q_p performed
absolute DEBUG_SCHEDULAR_EXIT		= 0xc0de006d
absolute DEBUG_MSG_OUT_ENTRY		= 0xc0de006e
absolute DEBUG_MSG_OUT_EXIT		= 0xc0de006f
absolute DEBUG_RESELECTED_ENTRY		= 0xc0de0070
absolute DEBUG_NOT_USED			= 0xc0de0071	; targ resel but no ent
absolute DEBUG_RESELECTED_EXIT		= 0xc0de0072
absolute DEBUG_MAIN_ENTRY		= 0xc0de0073
absolute DEBUG_CMD_ENTRY		= 0xc0de0074
absolute DEBUG_CMD_EXIT			= 0xc0de0075
absolute DEBUG_DATA_IN_ENTRY		= 0xc0de0076
absolute DEBUG_DATA_IN_EXIT		= 0xc0de0077
absolute DEBUG_DATA_OUT_ENTRY		= 0xc0de0078
absolute DEBUG_DATA_OUT_EXIT		= 0xc0de0079
absolute DEBUG_STATUS_ENTRY		= 0xc0de007a
absolute DEBUG_STATUS_EXIT		= 0xc0de007b
absolute DEBUG_MSG_IN_ENTRY		= 0xc0de007c
absolute DEBUG_MSG_DISCONNECT		= 0xc0de007d
absolute DEBUG_MSG_CMD_COMPLETE		= 0xc0de007e
absolute DEBUG_MSG_IGNORE_WIDE		= 0xc0de007f
absolute DEBUG_MSG_EXTENDED		= 0xc0de0080
absolute DEBUG_MSG_EXTENDED_BIT_BUCKET	= 0xc0de0081
absolute DEBUG_MSG_SAVE_DATA_PTRS	= 0xc0de0082
absolute DEBUG_MSG_SAVE_DATA_PTRS_EXIT	= 0xc0de0083
absolute DEBUG_IO_COMPLETE_ENTRY	= 0xc0de0084
absolute DEBUG_IO_COMPLETE_EXIT		= 0xc0de0085
absolute DEBUG_COPY_ENTRY_ENTRY		= 0xc0de0086
absolute DEBUG_COPY_ENTRY_EXIT_TRUE	= 0xc0de0087
absolute DEBUG_COPY_ENTRY_EXIT_FALSE	= 0xc0de0088
absolute DEBUG_ISZERO32_ENTRY		= 0xc0de0089
absolute DEBUG_ISZERO32_EXIT_FALSE	= 0xc0de008a
absolute DEBUG_ISZERO32_EXIT_TRUE	= 0xc0de008b
absolute DEBUG_SUB32_ENTRY		= 0xc0de008c
absolute DEBUG_SUB32_2s_COMP		= 0xc0de008d
absolute DEBUG_ADD32_ENTRY		= 0xc0de008e
absolute DEBUG_ADD32_EXIT		= 0xc0de008f
absolute DEBUG_CMP32_ENTRY		= 0xc0de0090
absolute DEBUG_CMP32_EXIT_TRUE		= 0xc0de0091
absolute DEBUG_CMP32_EXIT_FALSE		= 0xc0de0092
absolute DEBUG_ID_TO_ADDR_ENTRY		= 0xc0de0093
absolute DEBUG_ID_TO_ADDR_EXIT		= 0xc0de0094
absolute DEBUG_DISCONNECT_EXIT		= 0xc0de0095
absolute DEBUG_STAT_NOT_GOOD_EXIT	= 0xc0de0096
absolute DEBUG_IGNORE_WIDE_EXIT		= 0xc0de0097
absolute DEBUG_OTHER_MSG_EXIT		= 0xc0de0098
absolute DEBUG_SUCCESSFUL_SELECTION	= 0xc0de0099
absolute DEBUG_GENERAL_PURPOSE_1	= 0xc0de009a		; spare
absolute DEBUG_GENERAL_PURPOSE_2        = 0xc0de009b            ; spare
absolute DEBUG_GENERAL_PURPOSE_3        = 0xc0de009c            ; spare
absolute DEBUG_MSG_REJECT		= 0xc0de009d

absolute ASSERT_RESELECT_TBL_PTR	= 0xc0de012d
absolute ASSERT_DATA_PTR_SAVED	        = 0xc0de012e     
absolute ASSERT_SELECT_TBL_PTR          = 0xc0de012f   
absolute ASSERT_GENERAL_PURPOSE_1       = 0xc0de0130
absolute ASSERT_GENERAL_PURPOSE_2       = 0xc0de0131
absolute ASSERT_GENERAL_PURPOSE_3       = 0xc0de0132

; following values are written in the DSP register through "INT value" when
; SIOP detects corrupted data. Values are:
absolute DATA_CORRUPTED			= 0xdead0000


; These values from scsi-2 spec, table 5-2

absolute MSG_COMMAND_COMPLETE	= 0
absolute MSG_DISCONNECT		= 4
absolute MSG_EXTENDED		= 1
absolute MSG_REJECT	        = 7
absolute MSG_IGNORE_WIDE_RESIDUE = 0x23
absolute MSG_SAVE_DATA_PTRS	= 2

; This offset matches the layout of a scatter gather entry.
; Offset zero is assumed to contain the length of the entry.

absolute OFFSETOF_SG_ENT_PTR	= 4

absolute SIZEOF_REQ_DONE_ENT	= 4	; sizeof (req_done_q [0])
absolute SIZEOF_SG_ENTRY	= 8	; size of scatter gather entry (cnt,ptr)
absolute SIZEOF_STRUCT_SDC	= 12	; sizeof (cntrlr_buf[0].scat_gath0)
absolute SIZEOF_CNTRLR_BUF	= 44	; sizeof (CNTRLR_Q[0]))
absolute OFFSETOF_REQ_TBL	= 4
absolute OFFSETOF_REQ_DEST	= 8
absolute OFFSETOF_MSG_OUT_LEN	= 8
absolute OFFSETOF_DSA_SG_PTR	= 44

; Following are offsets used by modules that test the integrity of data in 
; CLRAM, DPRAM, or the node memory. These modules can be used even when 
; debugging is turned off in the host. They check the integrity of data and
; INT the host if data are inconsistent. The host then stops the other SIOP 
; which did not INT and then hits a panic or an assert itself
; This way all three uPs (two 53C720s and one 80860) stop and the logic
; analyzer does not catch any unnecessary data on the local bus

absolute OFFSETOF_PTR_MSG_OUT	= 12	; relative to start of request table
absolute OFFSETOF_BYTES_MSG_OUT	= 16    ; relative to start of request table


; This TABLE directive must match the host "request_table" structure in
; ...Kernel/src/mk/kernel/scsi/adapters/scsi_53C720.h

;  dsa_dummy = 1{??},
;  dsa_sxfer_value = 1{??},
;  dsa_dest_id_val = 1{??},
;  dsa_scntrl3_val = 1{??},

TABLE dsa_relative_table	\
  dsa_dummy = {0, 0, 0, 0},	\
  dsa_msg_out_len = {0, 0, 0, 0},	\ ;dsa_msg_out_ptr = {0, 0, 0, 0},
  dsa_msg_out_buf = {0, 0, 0, 0,  0, 0, 0, 0},	\
  dsa_cmd_len = {0, 0, 0, 0},		\ ;dsa_cmd_ptr = {0, 0, 0, 0},
  dsa_cmd_buf = 12{0},		\
  dsa_sg_ptr = 4{0}			; offset not used



; shmem_bytes
;absolute	no_work_to_do	= SHMEM_BASE+0	; one byte
;absolute	halt_reason	= SHMEM_BASE+1	; one byte
;absolute	status_buf	= SHMEM_BASE+2	; one byte
;absolute	msg_in_cnt	= SHMEM_BASE+3	; one byte
;absolute	msg_in_buf	= SHMEM_BASE+4	; 8 byte length
;absolute	next_addr	= SHMEM_BASE+12 ; (4 bytes) from host
;absolute	req_tbl_ptr	= SHMEM_BASE+16	; (4 bytes) ptr to cur i/o req



	entry	init, main, schedular	; globals for host to use

	; debug access only
	entry	cntrlr_q_p, sel_no_atn, sched_empty, got_reselected
	entry	msg_sent, cmd_phase, cmd_sent, data_in_phase, data_out_phase
	entry	status_phase, msg_in_phase, err_unknown_phase, copy_buf
	entry	ignore_wide_flag, copy_entry, iszero32, add32, cmp32
	entry	req_done_ptr, targ_id_q_tag_to_addr
	entry	data_analyzer_trig

; * * * * * *    Variables    * * * * * *

	; CNTRLR_Q_P inited to &cntrlr_q [0]

cntrlr_q_p:
	nop	; word to hold address into cntrlr_q
		; of next element to wait for an item to get.
		; Note:  cntrlr_q_p is not advanced until the target
		; receives the request.

req_done_ptr:
	nop		; pointer into circular queue of request done I/Os

ignore_wide_flag:
	nop

copy_buf:
	nop		; 12-byte area to deal with src,dst,cnt descript
	nop
	nop

data_analyzer_trig:
        nop

garbage:
	nop;		; pointer to garbage data

; This is NOT code!  It is data.

bss0:	move memory 0, 0, 1		; provide zero and true

bss1:	move memory 0, pass(CNTRLR_Q), pass(&CNTRLR_Q[CNTRLR_Q_SIZE])
bss2:	move memory 0, pass(REQ_DONE_Q), pass(&REQ_DONE_Q[REQ_DONE_Q_SIZE])
bss3:	move memory 0, pass(RESELECT_TBL), 0

; Would like to use these absolute definitions for bss access but
; they don't produce relative addressing in the code, just a hard
; coded offset from zero.
;absolute	zero = bss0+4
;absolute	true = bss0+8
;absolute	startof_cntrlr_q = bss1+4	
;absolute	limitof_cntrlr_q = bss1+8
;absolute	startof_req_done_q = bss2+4
;absolute	limitof_req_done_q = bss2+8
;absolute	baseof_reselect_tbl = bss3+4



init:

; init .word initialized pointers
	move	memory 4, bss1+4, cntrlr_q_p
	move	memory 4, bss2+4, req_done_ptr

	; write 0xdeadbeef to data_analyzer_trig location
	move	0xde to scratcha3
        move    0xad to scratcha2
        move    0xbe to scratcha1
        move    0xef to scratcha0
        move    memory 4, pass(REG_BASE+0x34), data_analyzer_trig

	jump	rel (schedular)		; starting point is with nothing to do


; schedular:  checks request queue for work to do
; Usage:
;	jmp schedular
;
; Input:
;	None, except globals in cntrlr_q
;
; Output:
;	None, except side effects
;
; Uses:
;	everything
;
; Desc:
;	Checks current element of cntrlr_q which is pointed to by CNTRLR_Q_P.
;	If entry is empty, the queue must be empty.  The host is notified
;	and we go to sleep until something happens.
;
;	If the next cntrlr_q entry is valid, schedular copies request
;	and scatter-gather pointers from host to SIOP local ram.  CNTRLR_Q_P is
;	incremented regarding cntrlr_q as an N-entry circular buffer.
;	Doing the increment only after successfully selecting a target
;	allows reselections to preempt without forgetting to try to start
;	the i/o later.
;
; Data:
;	cntrlr_q:
;		uchar	queue_tag;	// target_id & queue_tag are arranged
;		uchar	target_id;	// for 720 easy access
;		ushort	filler;
;		struct {
;			void *source;
;			void *dest;
;			long count;
;		} req_tbl, scat_gath [N];



schedular:
	move	ctest2 to sfbr			; clear sigp

	move	memory 4, cntrlr_q_p, pass(REG_BASE+0x34)	; scratchA = &next entry
	move	scratcha0 + OFFSETOF_REQ_TBL to scratcha0
	move	scratcha1 + 0 to scratcha1 with carry
	move	scratcha2 + 0 to scratcha2 with carry
	move	scratcha3 + 0 to scratcha3 with carry ; A = &cntrl_q [X].req_tbl

; If source address is nonzero, there is work to do.  Try to copy entry
; from host memory to local memory.  If it is empty (zero source),
; copy_entry returns with carry set.

	call	rel (copy_entry)
	jump	rel (sched_empty), if carry	; if zero, nothing to do

; Here if a valid entry; the sched_ready code expects the DSA to point to
; the first destination address.  This (non-modular) fetch from COPY_BUF
; (set as a side effect of copy_entry) is quite expedient.  :-)

	move	memory 4, copy_buf+4, pass(REG_BASE+0x10)	; set DSA to request_tbl

; Copy the subsequent entries (scatter/gather tables) until a zero entry.
; The pseudo code is approximately:
;	do {
;		scratchA += 12;
;		copy (*(scratchA+0) to *(scratchA+4), length *(scratchA+8));
;	} while (*(scratchA+0) != 0)

sched_loop:
	move	scratcha0 + SIZEOF_STRUCT_SDC to scratcha0
	move	scratcha1 + 0 to scratcha1 with carry
	move	scratcha2 + 0 to scratcha2 with carry
	move	scratcha3 + 0 to scratcha3 with carry
	call	rel (copy_entry)		; copy scat_gath entry
	jump	rel (sched_loop), if not carry

; start of test module -------------------------------------------------------
; test module checks that the request table in CLRAM is not corrupted
        move    memory 4, pass(REG_BASE+0x10), pass(REG_BASE+0x34)
        move    scratcha0 + OFFSETOF_PTR_MSG_OUT to scratcha0
        move    scratcha1 + 0 to scratcha1 with carry
        move    scratcha2 + 0 to scratcha2 with carry
        move    scratcha3 + 0 to scratcha3 with carry ; scratchA=&msg_out_ptr
        move    memory 4, pass(REG_BASE+0x34), test_2+4
test_2: move    memory 4, PATCH, pass(REG_BASE+0x5c)    ; scratchB =
                                                        ; *(msg_out_ptr) =
                                                        ;       &msg_out_bytes
        move    scratcha0 + 0x04 to scratcha0
        move    scratcha1 + 0 to scratcha1 with carry
        move    scratcha2 + 0 to scratcha2 with carry
        move    scratcha3 + 0 to scratcha3 with carry ; scratchA=&msg_out_bytes
        call    rel (cmp32)             ; *(msg_out_ptr) == &msg_out_bytes
        jump    rel (test_2_pass), if carry  ; branch if equal => test passes
        move    memory 4, data_analyzer_trig, 0x80000038 ; trig logic analyzer
        move    LED_RED to gpreg
        int     DATA_CORRUPTED
itself_2: jump  rel (itself_2)  ;preventive step, should'nt get here
test_2_pass:
; end of test module -------------------------------------------------------

; sched_ready:  Determine whether to select with ATN or not.  Decided by
; whether or not msg_out is present.  This is probably directly related to
; the target being SCSI-1 or SCSI-2, but maybe not.  msg_out phase is
; apparently entered if the target receives the ATN signal.
;
; Though DSA points to the request, the 720 doesn't have a memory to memory
; move which makes use of DSA.  So indirection is done via runtime patching.
;
; Pseudo code for the next 5 instructions:
;	scratcha = *(dsa+4)
; where "4" is "offsetof_msg_out_len"

	move	memory 4, pass(REG_BASE+0x10), pass(REG_BASE+0x34)
	move	scratcha0 + OFFSETOF_MSG_OUT_LEN to scratcha0
	move	scratcha1 + 0 to scratcha1 with carry
	move	scratcha2 + 0 to scratcha2 with carry
	move	scratcha3 + 0 to scratcha3 with carry

	move	memory 4, pass(REG_BASE+0x34), sch_0+4	; patch source addr
sch_0:	move	memory 4, PATCH, pass(REG_BASE+0x34)	; move *(dsa+4) to scratcha

	move	memory 4, pass(REG_BASE+0x10), pass(REQ_TBL_PTR)	; save i/o request pointer

; pseudo code for the next 5 instructions:
;	if (scratcha0)
;		select target with    atn
;	else	select target without atn

	move	scratcha0 to sfbr
	jump	rel (sel_no_atn), if 0		; jump if no msg_out

; Both these "select" instructions implicitly set the SXFER and SCNTL3
; from DSA+0.

	select	ATN from 0, rel (got_reselected)
	jump	rel (selected) when msg_out
	move	ERROR_WRONG_PHASE to scratcha1	; jump if not expected phase
	jump	rel (bad_error)

sel_no_atn:
	select	from 0, rel (got_reselected)
	jump	rel (selected) when cmd
	move	ERROR_WRONG_PHASE to scratcha1	; jump if not expected phase
	jump	rel (bad_error)

; Successfully selected scsi target.  (If we didn't, we were reselected by
; some target and jumped to "got_reselected".)

selected:
	move	LED_GREEN to gpreg


; ---- Begin section that could be moved to disconnect handler ----
; Save request pointer for possible later reselection.
; pseudo code:
;
;	reselect_tbl [target_id << 8 + queue_tag] = cntrlr_q [X].req_tbl.dest
;
; written directly for the 720 environment:
;	*(reselect_tbl + ((ushort) *cntrlr_q_p << 2)) = *(cntrlr_q_p+8)

; evaluate right address
	move	memory 4, cntrlr_q_p, pass(REG_BASE+0x34)
	move	scratcha0 + OFFSETOF_REQ_DEST to scratcha0
	move	scratcha1 + 0 to scratcha1 with carry
	move	scratcha2 + 0 to scratcha2 with carry
	move	scratcha3 + 0 to scratcha3 with carry

	move	memory 4, pass(REG_BASE+0x34), sch_3+4	; patch source address

; evaluate left address
	move	memory 4, cntrlr_q_p, sch_2+4
sch_2:	move	memory 2, PATCH, pass(REG_BASE+0x34)	; scratcha = targ_id<<8 + q_tag
	call	rel (targ_id_q_tag_to_addr)	; scratcha = &resel_tbl[x]
	move	memory 4, pass(REG_BASE+0x34), sch_3+8	; fill in destination address

sch_3:	move	memory 4, PATCH, PATCH		; save request pointer
; ---- End section that could be moved to disconnect handler ----


; Mark the cntrlr_q entry available by setting req_tbl.src to zero.
;
;	*(cntrlr_q_p+4) = 0;

	move	memory 4, cntrlr_q_p, pass(REG_BASE+0x34)
	move	scratcha0 + OFFSETOF_REQ_TBL to scratcha0
	move	scratcha1 + 0 to scratcha1 with carry
	move	scratcha2 + 0 to scratcha2 with carry
	move	scratcha3 + 0 to scratcha3 with carry ; A = &cntrl_q [X].req_tbl
	move	memory 4, pass(REG_BASE+0x34), sch_4+8	; fix next "move"
sch_4:	move	memory 4, bss0+4, PATCH		; set first element zero

; Finish the pointer increment with modulus:
;	struct cntrlr_q	*cntrlr_q_p;
;
;	if (++cntrlr_q_p == &cntrlr_q [N-1])
;		cntrlr_q_p = &cntrlr_q [0];

	move	memory 4, cntrlr_q_p, pass(REG_BASE+0x34)
	move	scratcha0 + SIZEOF_CNTRLR_BUF to scratcha0
	move	scratcha1 + 0 to scratcha1 with carry
	move	scratcha2 + 0 to scratcha2 with carry
	move	scratcha3 + 0 to scratcha3 with carry
						; scratcha += sizeof (cntrlr_q)
	move	memory 4, pass(REG_BASE+0x34), cntrlr_q_p ; update cntrlr_q_p

	move	memory 4, bss1+8, pass(REG_BASE+0x5c)
	call	rel (cmp32)			; scratchb == end of cntrlr_q?
		; would like that to be "a<=b" to catch incrementing past end

	jump	rel (sch_no_wrap), if not carry	; branch if not equal

	move	memory 4, bss1+4, cntrlr_q_p ; wrap to beginning
sch_no_wrap:

; start of test module -------------------------------------------------------
; test module checks that the request table in CLRAM is not corrupted
        move    memory 4, pass(REQ_TBL_PTR), pass(REG_BASE+0x34)
        move    scratcha0 + OFFSETOF_PTR_MSG_OUT to scratcha0
        move    scratcha1 + 0 to scratcha1 with carry
        move    scratcha2 + 0 to scratcha2 with carry
        move    scratcha3 + 0 to scratcha3 with carry ; scratchA=&msg_out_ptr
        move    memory 4, pass(REG_BASE+0x34), test_0+4
test_0: move    memory 4, PATCH, pass(REG_BASE+0x5c)    ; scratchB = 
							; *(msg_out_ptr) = 
							; 	&msg_out_bytes
        move    scratcha0 + 0x04 to scratcha0
        move    scratcha1 + 0 to scratcha1 with carry
        move    scratcha2 + 0 to scratcha2 with carry
        move    scratcha3 + 0 to scratcha3 with carry ; scratchA=&msg_out_bytes
        call    rel (cmp32)             ; *(msg_out_ptr) == &msg_out_bytes
        jump    rel (test_0_pass), if carry  ; branch if equal => test passes
        move    memory 4, data_analyzer_trig, 0x80000038 ; trig logic analyzer
	move 	LED_RED to gpreg
	int	DATA_CORRUPTED
itself_0: jump	rel (itself_0)	;preventive step, should'nt get here	
test_0_pass:
; end of test module -------------------------------------------------------


; Wait for message out phase.  If valid phase is not message out, then it's
; almost certainly not a scsi-2 target, so just skip onto whatever phase it
; wants.  Those are all handled beginning at "main".

	jump	rel (main), when not msg_out


; Here if msg_out
msg_out_phase:
	move	memory 4, pass(REQ_TBL_PTR), pass(REG_BASE+0x10)
	move	from dsa_msg_out_len, when msg_out
msg_sent:	; this label is used to detect phase mismatch during msg out
	clear	atn	; ??? ncr:  why don't their examples show this?
	jump	rel (main)



; If our controller queue is empty, notify the host.  Desirable for the
; (rather common) case where:
; - the host filled up the controller queue
; - the 720 starts servicing the requests
; - host is busy with other things and doesn't notice new slots available
; - 720 depletes the queue without attracting notice from host (e.g., io_done)

sched_empty:
	move	memory 1, bss0+8, pass(NO_WORK_TO_DO) ; indicate our hunger
						      ; to the host

	intfly	0				; rattle the host & don't halt;
						; value required, but useless
	move	LED_OFF to gpreg
	wait	reselect schedular		; wait for target reselect
						; if sigp, goto schedular

; Here if reselected by target.
;
; Reestablish request context by getting the queue tag from the target.
; Use the target id bit-concatenated with the queue tag i.d. to form a 12-bit
; index into a table of pointers.  Each pointer is the address of the
; beginning of the corresponding request table for this target and queue tag.
;
; If a target doesn't support queue tags, the host will assign all i/o to
; queue tag zero so it's important to clear the memory receiving the queue
; tag so that when the target doesn't provide a queue tag, we still use
; zero.

got_reselected:
	move	LED_GREEN to gpreg
	move	memory 4, bss0+4, pass(REG_BASE+0x34) ; prepare to hold id:t_l_q
	move	ssid to sfbr
	move	sfbr & 15 to scratcha1		; target number in bits 8-12

; Get 2 messages from the target:  The first is 1 byte and the second 2 bytes.
; The first message is IDENTIFY.  It contains the message code (80) plus the
; lun in bits 0-2.  We don't care about luns so we just toss the byte.
;
; The second message is the SIMPLE QUEUE TAG message:  the first byte is the
; message code (22) and the second byte is the queue_tag.

; This sequence guards against phase mismatch.

	move	1, pass(REG_BASE+0x5c),  when msg_in	; don't care about IDENTIFY
						; which contains LUN
	clear	ack
	jump	rel (resel_x), when not msg_in
	move	1, pass(REG_BASE+0x5c+0), when msg_in	; discard 0x22
	clear	ack
	jump	rel (resel_x), when not msg_in
	move	1, pass(REG_BASE+0x34+0), when msg_in	; scratchA0 = q_tag
resel_x:
	clear	ack
	call	rel (targ_id_q_tag_to_addr)
	move	memory 4, pass(REG_BASE+0x34), res_0+4	; A = &resel_tbl[targ<<8|q_tag]
res_0:	move	memory 4, PATCH, pass(REG_BASE+0x34)	; A = resel_tbl[targ<<8|q_tag]

	move	memory 4, pass(REG_BASE+0x34), pass(REG_BASE+0x10)		; dsa = resel_tbl[targ<<8|q_tag]

	move	memory 4, pass(REG_BASE+0x34), pass(REQ_TBL_PTR)

resel_1:

; Reload scsi control registers for this target.
;
; sxfer and scntl3 are normally automagically loaded for "select" but not
; for "reselect"--go figure.  Runtime patching is used because there's no
; "move memory from" variation to use indirection through the DSA reg.

	move	memory 4, pass(REG_BASE+0x34), res_2+4
res_2:	move	memory 4, PATCH, pass(REG_BASE+0x5c)

	move	scratchb1 to sfbr
	move	sfbr to sxfer

	move	scratchb3 to sfbr
	move	sfbr to scntl3


; fall into main


; start of test module -------------------------------------------------------
; test module checks that the request table in CLRAM is not corrupted
        move    memory 4, pass(REQ_TBL_PTR), pass(REG_BASE+0x34)
        move    scratcha0 + OFFSETOF_PTR_MSG_OUT to scratcha0
        move    scratcha1 + 0 to scratcha1 with carry
        move    scratcha2 + 0 to scratcha2 with carry
        move    scratcha3 + 0 to scratcha3 with carry ; scratchA=&msg_out_ptr
        move    memory 4, pass(REG_BASE+0x34), test_1+4
test_1: move    memory 4, PATCH, pass(REG_BASE+0x5c)  ; scratchB =
                                                      ; *(msg_out_ptr) =
                                                      ;       &msg_out_bytes
        move    scratcha0 + 0x04 to scratcha0        
        move    scratcha1 + 0 to scratcha1 with carry
        move    scratcha2 + 0 to scratcha2 with carry
        move    scratcha3 + 0 to scratcha3 with carry ; scratchA=&msg_out_bytes
        call    rel (cmp32)             ; *(msg_out_ptr) == &msg_out_bytes
        jump    rel (test_1_pass), if carry  ; branch if equal => test passes
        move    memory 4, data_analyzer_trig, 0x80000038 ; trig logic analyzer
        move    LED_RED to gpreg
        int     DATA_CORRUPTED
itself_1: jump  rel (itself_1)  ;preventive step, should'nt get here
test_1_pass:
; end of test module -------------------------------------------------------



main:
	jump	rel (cmd_phase),	when cmd
	jump	rel (data_in_phase),	if data_in
	jump	rel (data_out_phase),	if data_out
	jump	rel (status_phase),	if status
	jump	rel (msg_in_phase),	if msg_in
	jump	rel (msg_out_phase),	if msg_out

; Illegal phase.

err_unknown_phase:
	move	ERROR_UNKNOWN_PHASE to scratcha1
bad_error:
	move	memory 1, pass(REG_BASE+0x34+1), pass(HALT_REASON)

; This INT ceases script execution; something requires host attention.
; Let the host recognize it and reset us.

	move	LED_RED to gpreg
	int	0
	jump	rel (bad_error)		; preventative in case DSP rewritten
					; We shouldn't ever need this.


cmd_phase:
	move	memory 4, pass(REQ_TBL_PTR), pass(REG_BASE+0x10)
	move	from dsa_cmd_len,	when cmd
cmd_sent:	; this label is used to detect phase mismatch during cmd phase

; Some devices (e.g., Maxtor-1240S) issues a save data pointer message right
; after a queue tag command.
; To operate correctly, set DSA to the current scatter-gather entry before
; handling either msg_in or data_X phase.  Also, zero NEXT_ADDR so
; save_data_ptr doesn't get confused whether a phase mismatch occurred.

	move	memory 4, pass(REG_BASE+0x10), pass(REG_BASE+0x34)
	move	scratcha0 + OFFSETOF_DSA_SG_PTR to scratcha0
	move	scratcha1 + 0 to scratcha1 with carry
	move	scratcha2 + 0 to scratcha2 with carry
	move	scratcha3 + 0 to scratcha3 with carry	; scratchA = &sg_ptr

	move	memory 4, pass(REG_BASE+0x34), cmd_1+4
cmd_1:	move	memory 4, PATCH, pass(REG_BASE+0x10)		; dsa = sg_ptr

	move	memory 4, bss0+4, pass(NEXT_ADDR)	; show no phase mismatch
	jump	rel (main)




data_in_phase:

; Compute address of first scatter-gather entry.  This is pointed to by the
; dsa_sg_ptr entry in the i/o request.  It might have been updated
; by a save data pointers message, if previously processed.

	move	memory 4, pass(REQ_TBL_PTR), pass(REG_BASE+0x10)
	move	memory 4, pass(REG_BASE+0x10), pass(REG_BASE+0x34)
	move	scratcha0 + OFFSETOF_DSA_SG_PTR to scratcha0
	move	scratcha1 + 0 to scratcha1 with carry
	move	scratcha2 + 0 to scratcha2 with carry
	move	scratcha3 + 0 to scratcha3 with carry	; scratchA = &sg_ptr

	move	memory 4, pass(REG_BASE+0x34), din_0+4
din_0:	move	memory 4, PATCH, pass(REG_BASE+0x10)		; dsa = sg_ptr

	move	memory 4, bss0+4, ignore_wide_flag ; init flag
	move	memory 4, bss0+4, pass(NEXT_ADDR)  ; indicate no phase mismatch

	move	ctest0 & 127 to ctest0		; disable CDIS cache burst dsbl;
						; i.e. enable cache bursting
data_in_loop:
; Advance current scat-gath ptr to next entry.  This scheme keeps DSA
; always pointing to the current entry if a phase mismatch occurs.
;
; Also a copy of the current descriptor is used in case a phase mismatch
; causes us to update the entry to continue from the right place, but not
; update the original scat-gath entry unless a save data pointers message
; arrives.

; If a phase mismatch occurs, the 720 is halted and interrupts the host--all
; beyond our control (and against our desires).  This happens when a target
; disconnects mid transfer (changes to msg_in phase to send a disconnnect
; message, maybe because it wants to seek a different track, or, for a raid
; controller, talk to a different drive).
;
; To gracefully recover, the host checks for a phase mismatch interrupt,
; executes the phase_mismatch C code (below) and sets us (the 720) running.
; If we had our druthers, the "chmov" instruction would accept an alternate
; label to jump to for a phase change, analogous to a "select" or "reselect"
; instruction.

	chmov	from dsa_dummy, when data_in	; "dsa_dummy" == 0
; i.e.	chmov	from 0,         when data_in


	move	dsa0+SIZEOF_SG_ENTRY to dsa0	; dsa += 8;
	move	dsa1+0 to dsa1 with carry
	move	dsa2+0 to dsa2 with carry
	move	dsa3+0 to dsa3 with carry

; If an i/o request specifies an odd number of bytes and it is a 16-bit scsi
; device, getting the last byte to the target is a problem.  The 720 will not
; send the last byte unless it is paired up with another to fill the 16-bit
; pathway.  If a solution is implemented, it will be done by the host when it
; creates the last scatter gather entry in the list.  The command should
; contain the actual byte count (an odd value, otherwise it's no problem) but
; the last scatter gather entry should have one added to it so an even number
; of bytes are "sent" to the target.  The target will dutifully ignore the
; extra eight bits since it knows they aren't part of the command.  Failure
; to pad with an extra byte would make the 720 hang waiting for the target to
; phase change while the target is waiting for the last byte.
;
; The bottom line is the 720 code does nothing special for this case.
; There's nothing for the 720, except this long comment.

	jump	rel (data_in_loop),	when data_in

	move	ctest0 | 128 to ctest0		; disable cache bursting
	jump	rel (status_phase),	if status
	jump	rel (main)





data_out_phase:

; This code is identical to that of data_in_phase, except, of course, it
; is for data_out phase.

	move	memory 4, pass(REQ_TBL_PTR), pass(REG_BASE+0x10)
	move	memory 4, pass(REG_BASE+0x10), pass(REG_BASE+0x34)
	move	scratcha0 + OFFSETOF_DSA_SG_PTR to scratcha0
	move	scratcha1 + 0 to scratcha1 with carry
	move	scratcha2 + 0 to scratcha2 with carry
	move	scratcha3 + 0 to scratcha3 with carry	; scratchA = &sg_ptr

	move	memory 4, pass(REG_BASE+0x34), dout_0+4
dout_0:	move	memory 4, PATCH, pass(REG_BASE+0x10)

	move	memory 4, bss0+4, ignore_wide_flag
	move	memory 4, bss0+4, pass(NEXT_ADDR)
	move	ctest0 & 127 to ctest0		; enable cache bursting

data_out_loop:
	chmov	from dsa_dummy, when data_out

	move	dsa0+SIZEOF_SG_ENTRY to dsa0
	move	dsa1+0 to dsa1 with carry
	move	dsa2+0 to dsa2 with carry
	move	dsa3+0 to dsa3 with carry

	jump	rel (data_out_loop),	when data_out

; XXX The DMA read FIFO flush can be removed when handled by the DMA hardware.
	move	memory 4, pass(SDB_DMA_FLUSH_RD), garbage ; flush DMA FIFO

	move	ctest0 | 128 to ctest0		; disable cache bursting
	jump	rel (main),		if not status

; Fall through to status phase...



status_phase:
	move	1, pass(STATUS_BUF),	when status
	jump	rel (main),		when not msg_in



msg_in_phase:

; Read first byte of message in to determine what type of message it is.

	move	1, pass(MSG_IN_BUF),	when msg_in
	jump	rel (mi_1),	if not MSG_DISCONNECT

; DISCONNECT message from target

	move	scntl2 & 127 to scntl2		; allow disconnect without intr
	clear	ack
	wait	disconnect

; Scntl2 bit 7 is reasserted with every select/reselect operation.  So no
; need to turn it back on.

; Place to add code to mark expectation of reconnection (probably not needed)

	jump	rel (schedular)



; Save data pointers is complicated, so it is elsewhere.

mi_1:	jump	rel (do_save_data_ptrs), if MSG_SAVE_DATA_PTRS

	jump	rel (mi_2), if not MSG_COMMAND_COMPLETE

; COMMAND COMPLETE  message from target

	move	scntl2 & 127 to scntl2	; prevent 720 disconnect interrupt
	clear	ack
	wait	disconnect

; Check status byte.  Refer to Table 6-6 in scsi-2 spec.

	move	memory 1, pass(STATUS_BUF), pass(REG_BASE+0x34+2)
	move	scratcha2 & 0x3e to sfbr	; bits 1-5 only matter
	jump	rel (io_complete), if 0

; Error status is not "good".  (Queue_full, busy or check_condition are the
; ones the host will react to; others are handled more drastically.)

	move	ERROR_STAT_NOT_GOOD to scratcha1
	jump	rel (bad_error)


mi_2:	jump	rel (mi_3), if not MSG_IGNORE_WIDE_RESIDUE


; IGNORE WIDE RESIDUE message from target
; Read the second byte, the second byte will "always" have 1 in it
; meaning "ignore bits 8-15".

; Have to save the presence of this message.  Signal save data ptrs for
; receive handler to discard the last byte.  (I.e. 1000 bytes arrived,
; but byte #1000 is not part of the data.)

	clear	ack		; got "ignore wide" byte

	move	1, pass(MSG_IN_BUF+1), when msg_in
	clear	ack

; Flag the save data pointers routine to not count the last byte received.
; This is done by making ignore_wide_flag nonzero.

	move	memory 1, bss0+8, ignore_wide_flag
	jump	rel (msg_in_phase), when msg_in
	jump	rel (main)

mi_3:	move	1 to scratchb3			; one byte of "other" or
						; extended msg_in
        jump    rel (mi_4), if not MSG_REJECT
        clear   ack                             ; got "msg_reject" byte
        jump    rel (mi_5)


mi_4:	jump	rel (mi_5), if not MSG_EXTENDED

	clear	ack				; got "msg_extended" byte

; EXTENDED message from target


; As simple as this should be, it's not simple.  The 720 does *not* provide
; a way of "move as many bytes as there are from the target".  So this is a
; bit more circuitous.  It also provides scratchb3 with the message(s) length.
	move	2 to scratchb3
	move	1, pass(MSG_IN_BUF+1), when msg_in
	clear	ack
	jump	rel (msg_in_done), when not msg_in

	move	3 to scratchb3
	move	1, pass(MSG_IN_BUF+2), when msg_in
	clear	ack
	jump	rel (msg_in_done), when not msg_in

	move	4 to scratchb3
	move	1, pass(MSG_IN_BUF+3), when msg_in
	clear	ack
	jump	rel (msg_in_done), when not msg_in

	move	5 to scratchb3
	move	1, pass(MSG_IN_BUF+4), when msg_in
	clear	ack
	jump	rel (msg_in_done), when not msg_in

	move	6 to scratchb3
	move	1, pass(MSG_IN_BUF+5), when msg_in
	clear	ack
	jump	rel (msg_in_done), when not msg_in

	move	7 to scratchb3
	move	1, pass(MSG_IN_BUF+6), when msg_in
	clear	ack
	jump	rel (msg_in_done), when not msg_in

	move	8 to scratchb3
	move	1, pass(MSG_IN_BUF+7), when msg_in
	clear	ack
	jump	rel (msg_in_done), when not msg_in

mi_loop:
	move	scratchb3 + 1 to scratchb3
	move	1, pass(REG_BASE+0x5c),	when msg_in
	clear	ack
	jump	rel (mi_loop),	when msg_in

msg_in_done:

; Here for an unexpected or extended message from target.
; Give it to the host.

mi_5:
	move	memory 1, pass(REG_BASE+0x5c+3), pass(MSG_IN_CNT) ; save count for host

; Give the extended message or other messages to the host.

	move	ERROR_OTHER_MSG to scratcha1
	jump	rel (bad_error)



do_save_data_ptrs:
	clear	ack

; Steps to perform:
; (1) If no phase mismatch occurred, save DSA in the request desc_ptr.
; (2) If a phase mismatch occurred, save DSA in the request desc_ptr
;     and update the current entry to reflect the i/o performed so far.
; In either case, DSA points to the next scat-gath entry to do i/o.
;
; The presense of a phase mismatch is determined by whether "next_addr"
; is nonzero.

	move	memory 4, pass(REQ_TBL_PTR), pass(REG_BASE+0x34)	; scratchA = cur_i_o_request
	move	scratcha0 + OFFSETOF_DSA_SG_PTR to scratcha0
	move	scratcha1 + 0 to scratcha1 with carry
	move	scratcha2 + 0 to scratcha2 with carry
	move	scratcha3 + 0 to scratcha3 with carry	; scratchA = &sg_ptr

	move	memory 4, pass(REG_BASE+0x34), dsp_1+8	; patch dest addr of next instr
dsp_1:	move	memory 4, pass(REG_BASE+0x10), PATCH		; update sg_ptr

	move	memory 4, pass(NEXT_ADDR), pass(REG_BASE+0x34)	; check for phase mismatch
	call	rel (iszero32)
	jump	rel (main), if carry		; jump if no phase mismatch
						; save_data_ptr message complete


; Here for phase mismatch leading to reception of save_data_ptrs message.
; Update current scat-gath entry.  The entry is updated with
;	xferred = (next_addr - orig_addr) - ignore_wide_flag;
;	sg.len -= xferred;
;	sg.ptr += xferred;


; Get current ptr (at offset 4 from DSA)
	move	memory 4, pass(REG_BASE+0x10), pass(REG_BASE+0x34)
	move	scratcha0 + OFFSETOF_SG_ENT_PTR to scratcha0
	move	scratcha1 + 0 to scratcha1 with carry
	move	scratcha2 + 0 to scratcha2 with carry
	move	scratcha3 + 0 to scratcha3 with carry	; scratchA = 4 + dsa;

	move	memory 4, pass(REG_BASE+0x34), dsp_6+8	; fix cur_desc.ptr ref
	move	memory 4, pass(REG_BASE+0x34), dsp_5+4
	move	memory 4, pass(REG_BASE+0x34), dsp_2+4
dsp_2:	move	memory 4, PATCH, pass(REG_BASE+0x5c)	; scratchB = *(dsa+4)
						;	   = orig_ptr

	move	memory 4, pass(NEXT_ADDR), pass(REG_BASE+0x34)	; scratchA = next_addr
	call	rel (sub32)			; scratchA = next_addr - orig_p
						;          = "n"

	move	memory 4, ignore_wide_flag, pass(REG_BASE+0x5c)
	call	rel (sub32)			; scratchA = n - ignore_wide
						;	   = xferred
	move	memory 4, pass(REG_BASE+0x34), copy_buf	; save copy of xferred

	move	memory 4, pass(REG_BASE+0x34), pass(REG_BASE+0x5c)	; scratchB = xferred

; Have xferred; update scatter-gather length

	move	memory 4, pass(REG_BASE+0x10), dsp_3+4	; fix count source address
	move	memory 4, pass(REG_BASE+0x10), dsp_4+8	; and count rewrite address
dsp_3:	move	memory 4, PATCH, pass(REG_BASE+0x34)
	call	rel (sub32)			; scratchA = sg.len - xfer_len

dsp_4:	move	memory 4, pass(REG_BASE+0x34), PATCH	; update sg entry count


; update scatter-gather pointer

dsp_5:	move	memory 4, PATCH, pass(REG_BASE+0x34)	; scratchA = sg.ptr
	move	memory 4, copy_buf, pass(REG_BASE+0x5c)
	call	rel (add32)			; scratchA = sg.ptr + xferred

dsp_6:	move	memory 4, pass(REG_BASE+0x34), PATCH	; update sg entry pointer

; save_data_ptr message complete
	jump	rel (main)




; I/O Complete:  put pointer of completed i/o req_tbl_ptr in next place in
;		req_done_q.  If the queue is full, give error and halt.
;
; The queue is full if the entry which req_done_ptr points to is nonzero.

io_complete:
	move	memory 4, req_done_ptr, iod_1+4

iod_1:	move	memory 4, PATCH, pass(REG_BASE+0x34)	; scratchA = *req_done_p
	call	rel (iszero32)
	jump	rel (iod_full), if not carry	; jump if full

	move	memory 4, req_done_ptr, iod_2+8
iod_2:	move	memory 4, pass(REQ_TBL_PTR), PATCH	; *req_done_p = req_tbl_ptr

	move	memory 4, req_done_ptr, pass(REG_BASE+0x34); scratchA = req_done_p

; increment req_done_ptr
	move	scratcha0 + SIZEOF_REQ_DONE_ENT to scratcha0
	move	scratcha1 + 0 to scratcha1 with carry
	move	scratcha2 + 0 to scratcha2 with carry
	move	scratcha3 + 0 to scratcha3 with carry ; scratchA = ++req_done_p
	move	memory 4, pass(REG_BASE+0x34), req_done_ptr

	move	memory 4, bss2+8, pass(REG_BASE+0x5c)

	call	rel (cmp32)		; scratchb == end of req_done_q?
		; would like that to be "a<=b" to catch incrementing past end

	jump	rel (iod_nowrap), if not carry  ; branch if not equal

; pointer is past end of queue, wrap back to beginning
	move	memory 4, bss2+4, req_done_ptr

iod_nowrap:
	intfly	0
	jump	rel (schedular)


; No room in req_done_q
iod_full:
	move	ERROR_REQ_DONE_Q_FULL to scratcha1
	jump	rel (bad_error)




; copy_entry:  memory to memory move with operands in descriptor
; Usage:
;	(set scratchA to address of src,dst,cnt)
;	call copy_entry
;
; Input:
;	scratcha:  address of copy descriptor
;
; Output:
;	none, except side effects
;
; Returns:
;	carry bit is set if the source address is zero (no copy performed)
;	carry bit clear means the copy completed.
;
; Uses:
;	sfbr, scratchb
;
; Bugs:
;	Only the 24 least significant bits of count are used.
;	??? Not sure what a zero count does.
;	The 2 least significant bits of source and destination must
;	be the same, unless source is zero.
;
; Desc:
;	ScratchA points to a table of 3 32-bit words:
;		.long	source_address
;		.long	dest_address
;		.long	byte_count

copy_entry:
	move	memory 4, pass(REG_BASE+0x34), copy0+4	; patch inst. to get source addr
copy0:	move	memory 12, PATCH, copy_buf	; put descr in buffer so not so
						; much more patching
	move	memory 4, copy_buf, pass(REG_BASE+0x5c)	; scratchB = src_addr

; Would like to call iszero, but nested procs aren't supported
; If nonzero, jump to copy1

	move	scratchb0 to sfbr
	jump	rel (copy1), if not 0
	move	scratchb1 to sfbr
	jump	rel (copy1), if not 0
	move	scratchb2 to sfbr
	jump	rel (copy1), if not 0
	move	scratchb3 to sfbr
	jump	rel (copy1), if not 0

; return with carry set to indicate empty entry
	set	carry
	return

copy1:
; Set up to perform copy.
; The objective is to do the following (non-existent) operation:
;	move	memory *(scratcha+8), *(scratcha+0), *(scratcha+4)
;
; It's easy to do the equivalent of:
;	move	memory *cnt, *src, *dst
; This uses four memory moves (3 to modify a move instruction and one to do
; it).  But including the offsets requires a bit more.  To simplify the
; whole thing, the 3 values were moved to "copy_buf" to reduce runtime
; patching.  This reduces, somewhat counterintuitively, to 3 instructions.

	move	memory 8, copy_buf, copy2+4		; patch source/dest addr
	move	memory 3, copy_buf+8, copy2		; patch 24-bit count

	move	ctest0 & 127 to ctest0			; enable cache bursting
copy2:	move	memory PATCH, PATCH, PATCH		; do the copy
	move	ctest0 | 128 to ctest0			; disable cache bursts

	clear	carry
	return



; iszero32:  if 32-bit value of scratcha is zero, return with carry set.
; Usage:
;	call iszero32
;
; Input:
;	32-bits of scratcha
;
; Output:
;	none (scratcha is not changed)
;
; Uses:
;	sfbr is destroyed.

iszero32:
	move	scratcha0 to sfbr
	jump	rel (isnotzero), if not 0

	move	scratcha1 to sfbr
	jump	rel (isnotzero), if not 0

	move	scratcha2 to sfbr
	jump	rel (isnotzero), if not 0

	move	scratcha3 to sfbr
	jump	rel (isnotzero), if not 0

	set	carry			; Value is zero
	return

isnotzero:
	clear	carry
	return



; sub32:  32-bit subtraction.  scratcha -= scratchb
; Usage:
;	call sub32
;
; Input:
;	32-bits of scratchb
;	32-bits of scratcha
;
; Output:
;	32-bits of scratcha, the carry bit contains any borrow
;
; Uses:
;	scratchb is destroyed.
;
; Desc:
;	calculates scratcha = scratcha - scratchb
;
; Algorithm:
;	Calculates the 2s complement of scratchb and adds it to scratcha

sub32:

; Ones complement of scratchb:
	move	scratchb0 xor 255 to scratchb0
	move	scratchb1 xor 255 to scratchb1
	move	scratchb2 xor 255 to scratchb2
	move	scratchb3 xor 255 to scratchb3

; Add one
	move	scratchb0 + 1 to scratchb0
	move	scratchb1 + 0 to scratchb1 with carry
	move	scratchb2 + 0 to scratchb2 with carry
	move	scratchb3 + 0 to scratchb3 with carry
	;jump	rel (add32)		; "correct" coding would be
					;	call	rel (add32)
					;	return
					; but nested "call"s are messy

	; fall through to add32



; add32:  32-bit addition.  scratcha += scratchb
; Usage:
;	call add32
;
; Input:
;	32-bits of scratchb
;	32-bits of scratcha
;
; Output:
;	32-bits of scratcha, the carry bit contains any overflow
;
; Returns:
;	The carry bit has any overflow generated from the addition
;
; Uses:
;	scratchb is destroyed.
;
; Desc:
;	This operation is surprisingly difficult with the NCR instruction
;	set and probably represents a major departure between NCR's intended
;	use of the chip and Intel's requirements.
;
; Algorithm:
;	Uses the "register to register move" instruction (an 8-bit move)
;	which includes an obscure ability to add an 8-bit value.  The 8-bit
;	value must be a constant coded into the "move" instruction.  32-bit
;	addition requires 4 additions plus dealing with byte to byte carrys.
;
;	Move the 4 bytes of scratchb into the 4 instructions where +XXX
;	appears.  +XXX is a place holder to (a) persuade the assembler to
;	generate the right opcode and (b) provide a visual pattern in a hex
;	dump to be sure the right place is modified.
;
;	Complication!  The move memory instruction is the only mechanism the
;	NCR chip provides usable for modifying instructions.  Unfortunately,
;	it demands the source and destination be the same number of bytes
;	past the last 4-byte boundary.  That is, the source and destination
;	address must have the same least significant 2 bits.  (If they are
;	not, an "illegal instruction exception occurs.)  As the destination
;	is always byte 1 of an instruction, scratchb1 is the only register
;	aligned correctly.

add32:
	move	memory 1, pass(REG_BASE+0x5c+1), add1+1		; mod code for bits 8-15
							; and make B1 available
	move	scratchb0 to sfbr			; Move B0 to B1
	move	sfbr	  to scratchb1
	move	memory 1, pass(REG_BASE+0x5c+1), add0+1		; bits 0-7 patch

	move	scratchb2 to sfbr			; move B2 to B1
	move	sfbr	  to scratchb1
	move	memory 1, pass(REG_BASE+0x5c+1), add2+1		; 16-23 patch

	move	scratchb3 to sfbr			; move B3 to B1
	move	sfbr	to scratchb1
	move	memory 1, pass(REG_BASE+0x5c+1), add3+1		; 24-31 patch

absolute	XXX=127 ; place holder only--actual value is patched (above)

add0:	move	scratcha0+XXX to scratcha0	; add bits 7-0
add1:	move	scratcha1+XXX to scratcha1 with carry	; add bits 15-8
add2:	move	scratcha2+XXX to scratcha2 with carry	; add bits 23-16
add3:	move	scratcha3+XXX to scratcha3 with carry	; add bits 31-24
	return




; cmp32:  Compare scratcha to scratchb; if equal, return with carry set.
; Usage:
;	call cmp32
;
; Input:
;	32-bits of scratcha
;	32-bits of scratchb
;
; Output:
;	32-bits of scratcha, the carry bit contains any overflow
;
; Returns:
;	The carry bit has any overflow generated from the addition
;
; Uses:
;	scratchb1 is destroyed.

cmp32:

	move	memory 1, pass(REG_BASE+0x5c+1), cmp1+1
cmp1:	move	scratcha1 xor XXX to sfbr	; cmp bits 15-8
	jump	rel (cmp9), if not 0

	move	scratchb0 to sfbr
	move	sfbr	  to scratchb1
	move	memory 1, pass(REG_BASE+0x5c+1), cmp0+1

cmp0:	move	scratcha0 xor XXX to sfbr	; cmp bits 7-0
	jump	rel (cmp9), if not 0

	move	scratchb2 to sfbr
	move	sfbr	  to scratchb1
	move	memory 1, pass(REG_BASE+0x5c+1), cmp2+1

cmp2:	move	scratcha2 xor XXX to sfbr	; cmp bits 23-16
	jump	rel (cmp9), if not 0

	move	scratchb3 to sfbr
	move	sfbr	  to scratchb1
	move	memory 1, pass(REG_BASE+0x5c+1), cmp3+1

cmp3:	move	scratcha3 xor XXX to sfbr	; cmp bits 31-24
	jump	rel (cmp9), if not 0

; Values must be equal
	set	carry
	return

cmp9:	clear	carry
	return


; targ_id_q_tag_to_addr:  given target id and q_tag, calc reselect_tbl address
;		scratcha0 with the left one position
; Usage:
;	<put targ_id in scratch1>
;	<put q_tag in scratcha0>
;	call targ_id_q_tag_to_addr
;
; Input:
;	16-bits of scratcha (scratcha0, scratcha1)
;
; Output:
;	32-bits of scratcha;
;	Contains the address in the reselect table of the corresponding
;	entry
;
; Uses:
;	none
;
; Desc:
;	The operation corresponds to
;	scratcha = &resel_tbl [targ<<8 | q_tag]

targ_id_q_tag_to_addr:
	clear	carry
	move	scratcha0 shl 0 to scratcha0
	move	scratcha1 shl 0 to scratcha1

	clear	carry
	move	scratcha0 shl 0 to scratcha0
	move	scratcha1 shl 0 to scratcha1

	move	0 to scratcha2
	move	0 to scratcha3

	move	memory 4, bss3+4, pass(REG_BASE+0x5c)
	jump	rel (add32)		; scratcha = &resel_tbl[targ<<8 | q_tag]
	; return performed from add32
