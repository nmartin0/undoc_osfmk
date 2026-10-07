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
 * Mach Operating System
 * Copyright (c) 1991 Carnegie Mellon University
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
 * Copyright 1988, 1989, 1990, 1991 by Intel Corporation,
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
 * $Log: db_disasm.c,v $
 * Revision 2.7  1994/11/18  20:38:38  mtm
 * Copyright additions/changes
 *
 * Revision 2.6  1994/07/12  19:18:20  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 2.5.8.1  1994/04/27  17:15:18  andyp
 * Disassemble pfld.q correctly.
 *
 * Revision 2.5  1993/06/30  22:30:27  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 2.4  1993/05/07  21:58:54  andyp
 * koichi@gomez.intel.com provided some fixes for floating-point
 * instruction disassembly.
 *
 * Revision 2.3  1992/09/22  17:13:32  SSD
 * *** empty log message ***
 *
 * Revision 2.5  91/12/10  16:30:29  jsb
 * 	Fixes from Intel
 * 	[91/12/10  15:32:33  jsb]
 * 
 * Revision 2.4  91/06/18  20:50:50  jsb
 * 	New code and copyright from Intel.
 * 	[91/06/18  18:49:32  jsb]
 * 
 * Revision 2.3  91/06/17  15:44:17  jsb
 * 	Fixed call to db_find_sym_and_offset.
 * 	[91/06/17  10:22:41  jsb]
 * 
 * Revision 2.2  90/12/04  14:48:08  jsb
 * 	First checkin.
 * 	[90/12/03  21:26:41  jsb]
 * 
 */
/*
 * Instruction disassembler.
 * Derived from NX, with structure from i386 version.
 */

#include <mach/boolean.h>
#include <machine/db_machdep.h>

#include <ddb/db_access.h>
#include <ddb/db_sym.h>

#define SRC1(ir)	((ir >> 11) & 0x1F)
#define SRC2(ir)	((ir >> 21) & 0x1F)
#define DEST(ir)	((ir >> 16) & 0x1F)

static char *ctrlreg[] = {
	/*  0 */	"fir",
	/*  1 */	"psr",
	/*  2 */	"dirbase",
	/*  3 */	"db",
	/*  4 */	"fsr",
	/*  5 */	"epsr",
	/*  6 */	"bear",
	/*  7 */	"ccr",
	/*  8 */	"p0",
	/*  9 */	"p1",
	/* 10 */	"p2",
	/* 11 */	"p3",
	"?", "?", "?", "?",
	"?", "?", "?", "?", "?", "?", "?", "?",
	"?", "?", "?", "?", "?", "?", "?", "?"
};

static unsigned long	reg_contents[32];
static char		reg_valid[32];
static db_addr_t	ip_sequence;
static int		src1_valid;
static unsigned long	src1_value;
static int		src2_valid;
static unsigned long	src2_value;
static unsigned long	dest_reg;

static format_addr(a)
	unsigned long	a;
{
	db_printsym(a, DB_STGY_ANY);
}


static src1i(ir)
	unsigned long	ir;
{
	src1_valid = 1;
	src1_value = ir & 0xFFFF;
	if (src1_value & 0x8000)
		src1_value |= 0xFFFF0000;
	db_printf("%d", src1_value);
}


static src1ni(ir)
	unsigned long	ir;
{
	int rn = SRC1(ir);

	src1_valid = reg_valid[rn];
	src1_value = reg_contents[rn];
	if (rn == 2) {
		db_printf("sp");
	} else if (rn == 3) {
		db_printf("fp");
	} else {
		db_printf("r%d", rn & 0xFF);
	}
}


static src1(ir)
	unsigned long	ir;
{
	if (ir & 0x04000000) {
		return src1i(ir);
	} else {
		return src1ni(ir);
	}
}


static lsrc1(ir)
	unsigned long	ir;
{
	if (ir & 0x04000000) {
		src1_valid = 1;
		src1_value = ir & 0xFFFF;
		db_printf("%u", ir & 0xFFFF);
	} else {
		return src1ni(ir);
	}
}


static fsrc1(ir)
	unsigned long	ir;
{
	int rn = SRC1(ir);

	src1_valid = 0;
	db_printf("f%d", rn & 0xFF);
}


static src2(ir)
	unsigned long	ir;
{
	int rn = SRC2(ir);

	src2_valid = reg_valid[rn];
	src2_value = reg_contents[rn];
	if (rn == 2) {
		db_printf("sp");
	} else if (rn == 3) {
		db_printf("fp");
	} else {
		db_printf("r%d", rn & 0xFF);
	}
}


static fsrc2(ir)
	unsigned long	ir;
{
	int rn = SRC2(ir);

	src2_valid = 0;
	db_printf("f%d", rn & 0xFF);
}


static dest(ir, rtype)
	unsigned long	ir;
	char	rtype;
{
	int rn = DEST(ir);

	if (rtype == 'r') {
		dest_reg = rn;
		reg_valid[rn] = 0;
		if (rn == 2) {
			db_printf("sp");
			return;
		} else if (rn == 3) {
			db_printf("fp");
			return;
		}
	} else {
		dest_reg = 0;
	}

	db_printf("%c%d", rtype, rn & 0xFF);
}


static address(ir, method)
	unsigned long	ir;
	int	method;
{
	int rn1;
	int rn2 = SRC2(ir);

	src1_valid = 0;
	src2_valid = 0;

	switch (method) {
	case 0:			/* src1 */
		if ((ir & 0x04000000) == 0) {

			rn1 = SRC1(ir);
			src1_valid = reg_valid[rn1];
			src1_value = reg_contents[rn1];
			break;
		}
	case 1:			/* src1i */
		rn1 = -1;
		src1_valid = 1;
		src1_value = ir & 0xFFFF;
		if (src1_value & 0x8000)
			src1_value |= 0xFFFF0000;
		break;

	case 2:			/* store */
		rn1 = -1;
		src1_valid = 1;
		src1_value = (ir & 0x000007FF) | ((ir & 0x001F0000) >> 5);
		if (src1_value & 0x8000)
			src1_value |= 0xFFFF0000;
	}

	if (src1_valid && reg_valid[rn2]) {
		src2_valid = 1;
		src2_value = src1_value + reg_contents[rn2];
	}
	if (src2_valid && (reg_contents[rn2] & 0xFFFF) == 0) {
		db_printf("%s", "la%");
		format_addr(src2_value);
	} else if (rn1 == -1) {
		db_printf("%d", src1_value);
	} else {
		if (rn1 == 2) {
			db_printf("sp");
		} else if (rn1 == 3) {
			db_printf("fp");
		} else {
			db_printf("r%d", rn1 & 0xFF);
		}
	}
	db_printf("(");
	src2(ir);
	db_printf(")");
}


static ld(ir, op, rtype, ai)
	unsigned long	ir;
	char	*op;
	char	rtype;
	int	ai;
{
	db_printf("%s\t", op);
	address(ir, 0);
	if (ai) {
		db_printf("++");
	}
	db_printf(", ");
	dest(ir, rtype);
}


static st(ir, op)
	unsigned long	ir;
	char	*op;
{
	db_printf("%s\t", op);
	src1ni(ir);
	db_printf(", ");
	address(ir, 2);
}


static fst(ir, op, ai)
	unsigned long	ir;
	char	*op;
	int	ai;
{
	db_printf("%s\t", op);
	dest(ir, 'f');
	db_printf(", ");
	address(ir, 0);
	if (ai) {
		db_printf("++");
	}
}


static reg(ir, op)
	unsigned long	ir;
	char	*op;
{
	db_printf("%s\t", op);
	src1(ir);
	db_printf(", ");
	src2(ir);
	db_printf(", ");
	dest(ir, 'r');
}


static lreg(ir, op)
	unsigned long	ir;
	char	*op;
{
	db_printf("%s\t", op);
	lsrc1(ir);
	db_printf(", ");
	src2(ir);
	db_printf(", ");
	dest(ir, 'r');
}


static btxe(ir, op, xip)
	unsigned long	ir;
	char	*op;
{
	unsigned long	off;

	db_printf("%s\t", op);
	if (ir & 0x04000000) {
		db_printf("%d", SRC1(ir) & 0xFF);
	} else {
		src1ni(ir);
	}
	db_printf(", ");
	src2(ir);
	db_printf(", ");
	off = (ir & 0x000007FF) | (ir & 0x001F0000) >> 5;
	if (off & 0x00008000) {
		off |= 0xFFFF0000;
	}
	format_addr(xip + 4 + (off << 2));
}


static br(ir, op, xip)
	unsigned long	ir;
	char	*op;
{
	unsigned long	off;

	off = ir & 0x03FFFFFF;
	if (ir & 0x02000000) {
		off |= 0xFC000000;
	}
	db_printf("%s\t", op);
	format_addr(xip + 4 + (off << 2));
}


static flt(ir, op, ppos)
	unsigned long	ir;
	char	*op;
	int	ppos;
{
  	char op_str[16];
	bcopy(op, op_str, ppos);

	if ((ir & 0x100) == 0) {
		op_str[ppos] = 's';
	} else {
		op_str[ppos] = 'd';
	}
	if ((ir & 0x80) == 0) {
		op_str[ppos + 1] = 's';
	} else {
		op_str[ppos + 1] = 'd';
	}
	op_str[ppos + 2] = '\0';
	db_printf("%s\t", op_str);
	fsrc1(ir);
	db_printf(", ");
	fsrc2(ir);
	db_printf(", ");
	dest(ir, 'f');
}


static fltn2(ir, op, ppos)
	unsigned long	ir;
	char	*op;
	int	ppos;
{
  	char op_str[16];
	bcopy(op, op_str, ppos);

	if ((ir & 0x100) == 0) {
		op_str[ppos] = 's';
	} else {
		op_str[ppos] = 'd';
	}
	if ((ir & 0x80) == 0) {
		op_str[ppos + 1] = 's';
	} else {
		op_str[ppos + 1] = 'd';
	}
	op_str[ppos + 2] = '\0';
	db_printf("%s\t", op_str);
	fsrc1(ir);
	db_printf(", ");
	dest(ir, 'f');
}


static pix(ir, op)
	unsigned long	ir;
	char	*op;
{
	db_printf("%s\t", op);
	fsrc1(ir);
	db_printf(", ");
	fsrc2(ir);
	db_printf(", ");
	dest(ir, 'f');
}


/*
 * Disassemble instruction at 'loc'.  'altfmt' specifies an
 * (optional) alternate format.  Return address of start of
 * next instruction.
 */
db_addr_t db_disasm(loc, altfmt, task)
	db_addr_t	loc;
	boolean_t	altfmt;
	task_t		task;
{
	unsigned long	xip;
	unsigned long	ir;

	if (loc != ip_sequence) {
		bzero(reg_valid, sizeof(reg_valid));
	}

	ir = db_get_task_value(loc, 4, FALSE, task);
	xip = loc;
	loc = loc + 4;
	ip_sequence = loc;

	reg_valid[0] = 1;
	reg_contents[0] = 0;
	src1_valid = 0;
	src2_valid = 0;
	dest_reg = 0;

	switch(ir & 0xFC000000) {
	case 0x00000000:
	case 0x04000000:
	case 0x10000000:
	case 0x14000000:
		if (ir & 0x10000000) {
			if (ir & 0x00000001) {
				ld(ir & 0xFFFFFFFE, "ld.l", 'r', 0);
			} else {
				ld(ir & 0xFFFFFFFE, "ld.s", 'r', 0);
			}
		} else {
			ld(ir, "ld.b", 'r', 0);
		}
		break;

	case 0x0C000000:
	case 0x1C000000:
		if (ir & 0x10000000) {
			if (ir & 0x00000001) {
				st(ir & 0xFFFFFFFE, "st.l");
			} else {
				st(ir & 0xFFFFFFFE, "st.s");
			}
		} else {
			st(ir, "st.b");
		}
		break;

	case 0x08000000:
		db_printf("ixfr\t");
		src1ni(ir);
		db_printf(", ");
		dest(ir, 'f');
		break;

	case 0x20000000:
	case 0x24000000:
		switch(ir & 0x00000006) {
		case 0x00000000:
			ld(ir & 0xFFFFFFF8, "fld.d", 'f', ir & 1);
			break;
		case 0x00000004:
			ld(ir & 0xFFFFFFF8, "fld.q", 'f', ir & 1);
			break;
		case 0x00000002:
		case 0x00000006:
			ld(ir & 0xFFFFFFFC, "fld.l", 'f', ir & 1);
			break;
		}
		break;

	case 0x28000000:
	case 0x2C000000:
		switch(ir & 0x00000006) {
		case 0x00000000:
			fst(ir & 0xFFFFFFF8, "fst.d", ir & 1);
			break;
		case 0x00000004:
			fst(ir & 0xFFFFFFF8, "fst.q", ir & 1);
			break;
		case 0x00000002:
		case 0x00000006:
			fst(ir & 0xFFFFFFFC, "fst.l", ir & 1);
			break;
		}
		break;

	case 0x34000000:
		db_printf("flush\t");
		address(ir, 1);
		break;

	case 0x30000000:
		db_printf("ld.c\t%s, ", ctrlreg[SRC2(ir)]);
		dest(ir, 'r');
		break;

	case 0x38000000:
		db_printf("st.c\t");
		src1ni(ir);
		db_printf(", %s", ctrlreg[SRC2(ir)]);
		break;

	case 0x3C000000:
		db_printf("pst.d\t");
		dest(ir, 'f');
		db_printf(", ");
		address(ir & 0xFFFFFFFE, 1);
		if (ir & 0x00000001) {
			db_printf("++");
		}
		break;

	case 0x40000000:
		db_printf("bri\t");
		src1ni(ir);
		ip_sequence = 0;
		break;

	case 0x44000000:
		reg(ir & 0x03FFFFFF, "trap");
		break;

	case 0x48000000:
		switch(ir & 0x47F) {

		case 0x000:
		case 0x001:
		case 0x002:
		case 0x003:
		case 0x004:
		case 0x005:
		case 0x006:
		case 0x007:
		case 0x008:
		case 0x009:
		case 0x00A:
		case 0x00B:
		case 0x00C:
		case 0x00D:
		case 0x00E:
		case 0x00F:
			flt(ir, "pfam.p", 5);
			break;
		case 0x400:
		case 0x401:
		case 0x402:
		case 0x403:
		case 0x404:
		case 0x405:
		case 0x406:
		case 0x407:
		case 0x408:
		case 0x409:
		case 0x40A:
		case 0x40B:
		case 0x40C:
		case 0x40D:
		case 0x40E:
		case 0x40F:
			flt(ir, "pfmam.p", 6);
			break;
		case 0x010:
		case 0x011:
		case 0x012:
		case 0x013:
		case 0x014:
		case 0x015:
		case 0x016:
		case 0x017:
		case 0x018:
		case 0x019:
		case 0x01A:
		case 0x01B:
		case 0x01C:
		case 0x01D:
		case 0x01E:
		case 0x01F:
			flt(ir, "pfsm.p", 5);
			break;
		case 0x410:
		case 0x411:
		case 0x412:
		case 0x413:
		case 0x414:
		case 0x415:
		case 0x416:
		case 0x417:
		case 0x418:
		case 0x419:
		case 0x41A:
		case 0x41B:
		case 0x41C:
		case 0x41D:
		case 0x41E:
		case 0x41F:
			flt(ir, "pfmsm.p", 6);
			break;
		case 0x020:
			flt(ir, "fmul.p", 5);
			break;
		case 0x420:
			flt(ir, "pfmul.p", 6);
			break;
		case 0x021:
			flt(ir, "fmlow.p", 6);
			break;
		case 0x022:
			flt(ir, "frcp.p", 5);
			break;
		case 0x023:
			flt(ir, "frsqr.p", 6);
			break;
		case 0x424:
			if ((ir & 0x18) == 0x18) {
				flt(ir, "pfmul3.p", 7);
				break;
			}
			goto nogood;
		case 0x030:
			if (SRC2(ir) == 0) {
				fltn2(ir, "fmov.p", 5);
			} else {
				flt(ir, "fadd.p", 5);
			}
			break;
		case 0x430:
			if (SRC2(ir) == 0) {
				fltn2(ir, "pfmov.p", 6);
			} else {
				flt(ir, "pfadd.p", 6);
			}
			break;
		case 0x031:
			flt(ir, "fsub.p", 5);
			break;
		case 0x431:
			flt(ir, "pfsub.p", 6);
			break;
		case 0x032:
			fltn2(ir, "fix.p", 4);
			break;
		case 0x432:
			fltn2(ir, "pfix.p", 5);
			break;
		case 0x434:
			if ((ir & 0x80) == 0) {
				flt(ir, "pfgt.p", 5);
			} else {
				flt(ir, "pfle.p", 5);
			}
			break;
		case 0x435:
			flt(ir, "pfeq.p", 5);
			break;
		case 0x03A:
			flt(ir, "ftrunc.p", 7);
			break;
		case 0x43A:
			flt(ir, "pftrunc.p", 8);
			break;
		case 0x040:
			db_printf("fxfr\t");
			fsrc1(ir);
			db_printf(", ");
			dest(ir, 'r');
			break;
		case 0x049:
			if (SRC2(ir) == 0) {
				fltn2(ir, "fmov.p", 5);
			} else {
				flt(ir, "fiadd.p", 6);
			}
			break;
		case 0x449:
			if (SRC2(ir) == 0) {
				fltn2(ir, "pfmov.p", 6);
			} else {
				flt(ir, "pfiadd.p", 7);
			}
			break;
		case 0x04D:
			flt(ir, "fisub.p", 6);
			break;
		case 0x44D:
			flt(ir, "pfisub.p", 7);
			break;
		case 0x050:
			flt(ir, "faddp.p", 6);
			break;
		case 0x450:
			flt(ir, "pfaddp.p", 7);
			break;
		case 0x051:
			flt(ir, "faddz.p", 6);
			break;
		case 0x451:
			flt(ir, "pfaddz.p", 7);
			break;
		case 0x057:
			pix(ir, "fzchkl");
			break;
		case 0x457:
			pix(ir, "pfzchkl");
			break;
		case 0x05A:
			pix(ir, "form");
			break;
		case 0x45A:
			pix(ir, "pform");
			break;
		case 0x05F:
			pix(ir, "fzchks");
			break;
		case 0x45F:
			pix(ir, "pfzchks");
			break;
		default:
			goto nogood;
		}
		break;

	case 0x4C000000:
		switch(ir & 0x1F) {
		case 0x01:
			db_printf("lock");
			break;

		case 0x02:
			db_printf("calli\t");
			src1ni(ir);
			bzero(&reg_valid[16], 16);
			break;

		case 0x04:
			db_printf("intovr");
			break;

		case 0x07:
			db_printf("unlock");
			break;

		case 0x08:
			db_printf("ldio");
			break;

		case 0x09:
			db_printf("stio");
			break;

		case 0x0a:
			db_printf("ldint");
			break;

		case 0x0b:
			db_printf("scyc");
			break;

		default:
			goto nogood;
		}
		break;

	case 0x50000000:
	case 0x54000000:
		btxe(ir, "btne", xip);
		break;

	case 0x58000000:
	case 0x5C000000:
		btxe(ir, "bte", xip);
		break;

	case 0x60000000:
	case 0x64000000:
		switch(ir & 0x00000006) {
		case 0x00000000:
			ld(ir & 0xFFFFFFF8, "pfld.d", 'f', ir & 1);
			break;
		case 0x00000004:
			ld(ir & 0xFFFFFFF8, "pfld.q", 'f', ir & 1);
			break;
		case 0x00000002:
		case 0x00000006:
			ld(ir & 0xFFFFFFFC, "pfld.l", 'f', ir & 1);
			break;
		}
		break;

	case 0x68000000:
		br(ir, "br", xip);
		break;

	case 0x6C000000:
		br(ir, "call", xip);
		bzero(&reg_valid[16], 16);
		break;

	case 0x70000000:
		br(ir, "bc", xip);
		break;

	case 0x74000000:
		br(ir, "bc.t", xip);
		break;

	case 0x78000000:
		br(ir, "bnc", xip);
		break;

	case 0x7C000000:
		br(ir, "bnc.t", xip);
		break;

	case 0x80000000:
	case 0x84000000:
		reg(ir, "addu");
		if (src1_valid && src2_valid && dest_reg != 0) {
			reg_valid[dest_reg] = 1;
			reg_contents[dest_reg] =
				src1_value + src2_value;
		}
		break;

	case 0x88000000:
	case 0x8C000000:
		reg(ir, "subu");
		if (src1_valid && src2_valid && dest_reg != 0) {
			reg_valid[dest_reg] = 1;
			reg_contents[dest_reg] =
				src1_value - src2_value;
		}
		break;

	case 0x90000000:
	case 0x94000000:
		reg(ir, "adds");
		if (src1_valid && src2_valid && dest_reg != 0) {
			reg_valid[dest_reg] = 1;
			reg_contents[dest_reg] =
				src1_value + src2_value;
		}
		break;

	case 0x98000000:
	case 0x9C000000:
		reg(ir, "subs");
		if (src1_valid && src2_valid && dest_reg != 0) {
			reg_valid[dest_reg] = 1;
			reg_contents[dest_reg] =
				src1_value - src2_value;
		}
		break;

	case 0xA0000000:
		if (ir == 0xA0000000) {
			db_printf("nop");
		} else if (SRC1(ir) == 0) {
			db_printf("mov\t");
			src2(ir);
			db_printf(", ");
			dest(ir, 'r');
		} else {
			reg(ir, "shl");
		}
		if (src1_valid && src2_valid && dest_reg != 0) {
			reg_valid[dest_reg] = 1;
			reg_contents[dest_reg] =
				src2_value << src1_value;
		}
		break;

	case 0xA4000000:
		reg(ir, "shl");
		if (src1_valid && src2_valid && dest_reg != 0) {
			reg_valid[dest_reg] = 1;
			reg_contents[dest_reg] =
				src2_value << src1_value;
		}
		break;

	case 0xA8000000:
	case 0xAC000000:
		reg(ir, "shr");
		if (src1_valid && src2_valid && dest_reg != 0) {
			reg_valid[dest_reg] = 1;
			reg_contents[dest_reg] =
				src2_value >> src1_value;
		}
		break;

	case 0xB0000000:
		reg(ir, "shrd");
		break;

	case 0xB4000000:
		btxe(ir & ~0x04000000, "bla", xip);
		break;

	case 0xB8000000:
	case 0xBC000000:
		reg(ir, "shra");
		if (src1_valid && src2_valid && dest_reg != 0) {
			reg_valid[dest_reg] = 1;
			reg_contents[dest_reg] =
				((long)src2_value) >> src1_value;
		}
		break;

	case 0xC0000000:
	case 0xC4000000:
		lreg(ir, "and");
		if (src1_valid && src2_valid && dest_reg != 0) {
			reg_valid[dest_reg] = 1;
			reg_contents[dest_reg] =
				src1_value & src2_value;
		}
		break;

	case 0xCC000000:
		lreg(ir, "andh");
		if (src1_valid && src2_valid && dest_reg != 0) {
			reg_valid[dest_reg] = 1;
			reg_contents[dest_reg] =
				(src1_value << 16) & src2_value;
		}
		break;

	case 0xD0000000:
	case 0xD4000000:
		lreg(ir, "andnot");
		if (src1_valid && src2_valid && dest_reg != 0) {
			reg_valid[dest_reg] = 1;
			reg_contents[dest_reg] =
				~src1_value & src2_value;
		}
		break;

	case 0xDC000000:
		lreg(ir, "andnoth");
		if (src1_valid && src2_valid && dest_reg != 0) {
			reg_valid[dest_reg] = 1;
			reg_contents[dest_reg] =
				~(src1_value << 16) & src2_value;
		}
		break;

	case 0xE0000000:
	case 0xE4000000:
		lreg(ir, "or");
		if (src1_valid && src2_valid && dest_reg != 0) {
			reg_valid[dest_reg] = 1;
			reg_contents[dest_reg] =
				src1_value | src2_value;
		}
		break;

	case 0xEC000000:
		lreg(ir, "orh");
		if (src1_valid && src2_valid && dest_reg != 0) {
			reg_valid[dest_reg] = 1;
			reg_contents[dest_reg] =
				(src1_value << 16) | src2_value;
		}
		break;

	case 0xF0000000:
	case 0xF4000000:
		lreg(ir, "xor");
		if (src1_valid && src2_valid && dest_reg != 0) {
			reg_valid[dest_reg] = 1;
			reg_contents[dest_reg] =
				src1_value ^ src2_value;
		}
		break;

	case 0xFC000000:
		lreg(ir, "xorh");
		if (src1_valid && src2_valid && dest_reg != 0) {
			reg_valid[dest_reg] = 1;
			reg_contents[dest_reg] =
				(src1_value << 16) ^ src2_value;
		}
		break;

	default:
nogood:
		db_printf("?");
		break;

	}

	if (dest_reg != 0 && reg_valid[dest_reg]) {
		db_printf("\t// r%d = 0x%08x",
			dest_reg, reg_contents[dest_reg]);
	}

	db_printf("\n");
	return (loc);
}
