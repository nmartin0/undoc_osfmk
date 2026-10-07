#!/bin/csh -f
# 
# $Copyright
# Copyright 1993, 1994 , 1995 Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
# 
 
#
# Mach Operating System
# Copyright (c) 1989 Carnegie-Mellon University
# All rights reserved.  The CMU software License Agreement specifies
# the terms and conditions for use and redistribution.
#
#
# HISTORY
# $Log: mkconfig.csh,v $
# Revision 1.2  1994/11/18  20:25:09  mtm
# Copyright additions/changes
#
# Revision 1.1  1994/04/11  18:41:43  dleslie
#  Reviewer: John Litvin
#  Risk: low
#  Benefit or PTS #: support for configurable emulator builds
#  Testing: builds STD+WS and RAMDISK+WS emulators of same size as old way
# 	of building
#  Module(s):    MASTER MASTER.i386 MASTER.i386.local MASTER.i860
# 	MASTER.i860.local MASTER.local Makefile.i386 Makefile.i860
# 	Makefile.template files files.i386 files.i860 make.template
# 	mkconfig.csh
#
# Revision 1.4  1993/07/14  17:54:29  cfj
# OSF/1 AD 1.0.4 code drop from Locus.
#
# Revision 1.1.1.3  1993/07/01  19:12:55  cfj
# Adding new code from vendor
#
# Revision 1.3  1993/05/06  19:14:58  cfj
# ad103+tnc merged with Intel code.
#
# Revision 1.1.1.1  1993/05/03  17:28:05  cfj
# Initial 1.0.3 code drop
#
# Revision 1.2  1992/11/30  22:19:30  dleslie
# Copy of NX branch back into main trunk
#
# Revision 1.1.2.1  1992/11/05  23:18:40  dleslie
# Local changes for NX through noon, November 5, 1992.
#
# Revision 2.2  1991/08/31  13:31:02  rabii
# 	Initial V2.0 Checkin
#
# Revision 3.0  91/01/17  12:07:00  condict
# Unchanged copy from Mach 3.0 BSD UNIX server
# 
# Revision 2.1  89/08/04  15:29:06  rwd
# Created.
# 
# Revision 2.2  89/04/22  15:24:08  gm0w
# 	Created from root Makefile.
# 	[89/04/14            gm0w]
# 

set CONFIG="${argv[1]}"

shift argv

echo +${CONFIG} \
| \
cat ${argv} - ${argv} \
| \
sed -n \
    -e "/^+/{" \
      -e "s;[-+];#&;gp" \
        -e 't loop' \
      -e ': loop' \
	-e 'n' \
        -e '/^#/b loop' \
        -e '/^$/b loop' \
        -e 's;^\([^#]*\).*#[ 	]*<\(.*\)>[ 	]*$;\2#\1;' \
          -e 't not' \
        -e 's;\([^#]*\).*;#\1;' \
          -e 't not' \
      -e ': not' \
        -e 's;[ 	]*$;;' \
        -e 's;^\!\(.*\);\1#\!;' \
        -e 'p' \
        -e 't loop' \
	-e 'b loop' \
    -e '}' \
    -e "/^[^#]/d" \
    -e 's;	; ;g' \
    -e "s;^# *\([^ ]*\)[ ]*=[ ]*\[\(.*\)\].*;\1#\2;p" \
| \
awk '-F#' '\
part == 0 && $1 != "" {\
    m[$1]=m[$1] " " $2;\
    next;\
}\
part == 0 && $1 == "" {\
    for (i=NF;i>1;i--){\
	s=substr($i,2);\
	c[++na]=substr($i,1,1);\
	a[na]=s;\
    }\
    while (na > 0){\
	s=a[na];\
	d=c[na--];\
	if (m[s] == "") {\
	    f[s]=d;\
	} else {\
	    nx=split(m[s],x," ");\
	    for (j=nx;j>0;j--) {\
		z=x[j];\
		a[++na]=z;\
		c[na]=d;\
	    }\
	}\
    }\
    part=1;\
    next;\
}\
part != 0 {\
    if ($1 != "") {\
	n=split($1,x,",");\
	ok=0;\
	for (i=1;i<=n;i++) {\
	    if (f[x[i]] == "+") {\
		ok=1;\
	    }\
	}\
	if (NF > 2 && ok == 0 || NF <= 2 && ok != 0) {\
	    print $2; \
	}\
    } else { \
	print $2; \
    }\
}'
