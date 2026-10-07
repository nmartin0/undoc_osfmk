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
 * $Log: auxv.h,v $
 * Revision 1.4  1994/11/18  20:40:00  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:23:34  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:32:27  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:41:10  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:39:03  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:37:35  cfj
 * Bump major revision number.
 *
 * Revision 2.4  1991/11/05  12:11:28  sjs
 * Fixed RCS comments
 *
 * Revision 2.3  91/10/14  13:01:18  sjs
 * 	Revision 3.2  91/10/04  16:40:24  sp
 * 	Add types to pass the auxiliary vector in a mach message as MIG doesn't
 * 	cope too well with unions.
 * 
 * Revision 2.2  91/08/31  14:02:17  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/07/31  15:41:30  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.3  90/10/07  14:48:55  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:36:53  gm]
 * 
 * Revision 1.2  90/04/27  19:25:25  devrcs
 * 	Initial Revision.
 * 	[90/04/17  10:01:36  kwallace]
 * 
 * $EndLog$
 */
/*
 *                       The Auxiliary Vector
 *
 * AT_NULL	The auxiliary vector has no fixed length; instead its
 * 		last entry's a_type member has this value.
 * 
 * AT_IGNORE	This type indicates the entry has no meaning.  The
 * 		corresponding value of a_un is undefined.
 * 
 * AT_EXECFD	exec() or exec_with_loader() may pass control to an
 * 		interpreter program.  When this happens, the system
 * 		places either and entry of type AT_EXECFD or one of
 * 		type AT_PHDR in the auxiliary vector.  The entry for
 * 		type AT_EXECFD uses the a_val member to contain a file
 * 		descriptor open to read the application program's
 * 		object file.
 * 
 * AT_PHDR	Under some conditions, the system creates the memory
 * 		image of the application program before passing
 * 		control to the interpreter program.  When this
 * 		happens, the a_ptr member of the AT_PHDR entry tells
 * 		the interpreter where to find the program header table
 * 		in the memory image.  If the AT_PHDR entry is present,
 * 		entries of types AT_PHENT, AT_PHNUM, and AT_ENTRY must
 * 		also be present.  AT_EXEC_LOADER_FILENAME
 * 
 * AT_PHENT	The a_val member of this entry holds the size, in
 * 		bytes, of one entry in the program header table to
 * 		which the entry points.
 * 
 * AT_PHNUM	The a_val member of this entry holds the number of
 * 		entries in the program header table to which the
 * 		AT_PHDR entry points.
 * 
 * AT_PAGESZ	If present, this entry's a_val member gives the system
 * 		page size, in bytes.  The same information also is
 * 		available through sysconf().
 * 
 * AT_BASE	The a_ptr member of this entry holds the base address
 * 		at which the interpreter program was loaded into
 * 		memory.
 * 
 * AT_FLAGS	If present, the a_val member of this entry holds
 * 		one-bit flags.  Bits with undefined semantics are set
 * 		to zero.  Currently, no flag definitions exist for
 * 		this entry.  Nonetheless, bits under the 0xff000000
 * 		mask are reserved for system semantics.
 * 
 * AT_ENTRY	The a_ptr member of this entry holds the entry poiint
 * 		of the application program to which the interpreter
 * 		program should transfer control.
 * 
 * AT_EXEC_FILENAME
 * 		The a_ptr member of this entry holds a pointer to a
 * 		character array that contains the filename as passed
 * 		to exec() or exec_with_loader().
 * 
 * AT_EXEC_LOADER_FILENAME
 * 		If present, the a_ptr member of this entry holds a
 * 		pointer to a character array that contains the
 * 		filename of the loader as passed to
 * 		exec_with_loader(), or the filename of the default
 * 		loader, if NULL was passed to exec_with_loader() or
 * 		exec() was invoked.
 * 
 * AT_EXEC_LOADER_FLAGS
 * 		If present, the a_val member of this entry holds
 * 		one-bit flags intended for use by the loader.  Bits
 * 		with undefined semantics are set to zero.  Currently,
 * 		no flag definitions exist for this entry.
 * 		Nonetheless, bits under the 0xff000000 mask are
 * 		reserved for system semantics.
 */

#ifndef	_SYS_AUXV_H_
#define	_SYS_AUXV_H_

#define	AT_NULL		0
#define	AT_IGNORE	1
#define	AT_EXECFD	2
#define	AT_PHDR		3
#define	AT_PHENT	4
#define	AT_PHNUM	5
#define	AT_PAGESZ	6
#define	AT_BASE		7
#define	AT_FLAGS	8
#define	AT_ENTRY	9

#define	AT_OSF_BASE		1000
#define	AT_EXEC_FILENAME	(AT_OSF_BASE+1)
#define	AT_EXEC_LOADER_FILENAME	(AT_OSF_BASE+2)
#define	AT_EXEC_LOADER_FLAGS	(AT_OSF_BASE+3)
#define	AT_EXEC_FD		(AT_EXECFD)

typedef struct
{
	int	a_type;
	union {
		long	a_val;
		void	*a_ptr;
		void	(*a_fcn)();
	} a_un;
} auxv_t;

#ifdef	OSF1_SERVER
typedef	struct {
	int	a_type;
	union {
		long	a_val;
		long	a_ptr;
	} a_un;
} auxv_type_t;

typedef auxv_type_t auxv_mig_t[10];
typedef char	*auxv_strings_t;
#endif	/* OSF1_SERVER */

#endif	/* _SYS_AUXV_H_ */
