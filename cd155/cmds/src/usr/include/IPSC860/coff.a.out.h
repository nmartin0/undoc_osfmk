/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.1
 */
/* @(#)$RCSfile: coff.a.out.h,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:41:24 $ */
/*	a.out.h	2.4	*/

#ifndef _COFF_A_OUT_H_
#define _COFF_A_OUT_H_
#include <nlist.h>	/* included for all machines */

 /*		COMMON OBJECT FILE FORMAT



 	File Organization:

 	_______________________________________________    INCLUDE FILE
 	|_______________HEADER_DATA___________________|
 	|					      |
 	|	File Header			      |    "filehdr.h"
 	|.............................................|
 	|					      |
 	|	Auxilliary Header Information	      |	   "aouthdr.h"
 	|					      |
 	|_____________________________________________|
 	|					      |
 	|	".text" section header		      |	   "scnhdr.h"
 	|					      |
 	|.............................................|
 	|					      |
 	|	".data" section header		      |	      ''
 	|					      |
 	|.............................................|
 	|					      |
 	|	".bss" section header		      |	      ''
 	|					      |
 	|_____________________________________________|
 	|______________RAW_DATA_______________________|
 	|					      |
 	|	".text" section data (rounded to 4    |
 	|				bytes)	      |
 	|.............................................|
 	|					      |
 	|	".data" section data (rounded to 4    |
 	|				bytes)	      |
 	|_____________________________________________|
 	|____________RELOCATION_DATA__________________|
 	|					      |
 	|	".text" section relocation data	      |    "reloc.h"
 	|					      |
 	|.............................................|
 	|					      |
 	|	".data" section relocation data	      |       ''
 	|					      |
 	|_____________________________________________|
 	|__________LINE_NUMBER_DATA_(SDB)_____________|
 	|					      |
 	|	".text" section line numbers	      |    "linenum.h"
 	|					      |
 	|.............................................|
 	|					      |
 	|	".data" section line numbers	      |	      ''
 	|					      |
 	|_____________________________________________|
 	|________________SYMBOL_TABLE_________________|
 	|					      |
 	|	".text", ".data" and ".bss" section   |    "syms.h"
 	|	symbols				      |	   "storclass.h"
 	|					      |
 	|_____________________________________________|
	|________________STRING_TABLE_________________|
	|					      |
	|	    long symbol names		      |
	|_____________________________________________|



 		OBJECT FILE COMPONENTS

 	HEADER FILES:
 			/usr/include/filehdr.h
			/usr/include/aouthdr.h
			/usr/include/scnhdr.h
			/usr/include/reloc.h
			/usr/include/linenum.h
			/usr/include/syms.h
			/usr/include/storclass.h

	STANDARD FILE:
			/usr/include/a.out.h    "object file" 
   */

#include "filehdr.h"
#include "aouthdr.h"
#include "scnhdr.h"
#include "reloc.h"
#include "linenum.h"
#include "syms.h"

/*
 * Start of the text section must be on an 8 byte boundary.
 */
#define N_TXTOFF(f, a) \
	((FILHSZ + AOUTHSZ + (f).f_nscns * SCNHSZ) & 0xfffffff8)

#endif /* _COFF_A_OUT_H_ */
