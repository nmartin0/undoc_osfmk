// $Copyright
// Copyright 1989-1995 Intel Corporation
// INTEL CONFIDENTIAL
// The technical data and computer software contained herein are subject
// to the copyright notices; trademarks; and use and disclosure
// restrictions identified in the file located in /etc/copyright on
// this system.
// Copyright$
//


	.file	"sigsetjmp.c"

.extern _setjmp, __setjmp, _longjmp, __longjmp
	.text
	.globl	_sigsetjmp
	.globl	_siglongjmp
	.align	8
_sigsetjmp:
	
	st.l r17,192(r16)   // store savemask in 48*4 off of env (r16) 

        btne r0,r17,savesignal
	    
           br __setjmp
	      nop
savesignal:

           br _setjmp
              nop



_siglongjmp:

	ld.l 192(r16),r18   // check savemask in 48*4 off of env (r17) 

        btne r0,r18,savesignal1

           br __longjmp
	       nop
savesignal1:

           br _longjmp
	       nop

