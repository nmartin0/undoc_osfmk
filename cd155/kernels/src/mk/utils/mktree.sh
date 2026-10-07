#!/bin/sh
# 
# $Copyright
# Copyright 1991 , 1994, 1995 Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
# 
 
# 
# Mach Operating System
# Copyright (c) 1991 Carnegie Mellon University
# All Rights Reserved.
# 
# Permission to use, copy, modify and distribute this software and its
# documentation is hereby granted, provided that both the copyright
# notice and this permission notice appear in all copies of the
# software, derivative works or modified versions, and any portions
# thereof, and that both notices appear in supporting documentation.
# 
# CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
# CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
# ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
# 
# Carnegie Mellon requests users of this software to return to
# 
#  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
#  School of Computer Science
#  Carnegie Mellon University
#  Pittsburgh PA 15213-3890
# 
# any improvements or extensions that they make and grant Carnegie Mellon 
# the rights to redistribute these changes.
#
#
#
#
# HISTORY
# $Log: mktree.sh,v $
# Revision 1.5  1994/11/18  21:07:37  mtm
# Copyright additions/changes
#
# Revision 1.4  1993/06/30  23:06:55  dleslie
# Adding copyright notices required by legal folks
#
# Revision 1.3  1993/04/27  21:01:14  dleslie
# Copy of R1.0 sources onto main trunk
#
# Revision 1.1.10.2  1993/04/22  19:08:55  dleslie
# First R1_0 release
#
# Revision 2.5  91/08/24  12:36:24  af
# 	Fixed a comment.
# 	[91/07/21            danner]
# 	Missed one pmax/mips thingie.
# 	[91/07/13            danner]
# 
# 	Fixed mistake where pmax_mach was a link to pmax instead of mips.
# 	 This is the only machine were @sys_mach should not be a link to
# 	 @sys. Sigh.
# 	[91/07/11            danner]
# 
# 	Added i860 to machines. Changed directory warning to warn if the
# 	 current directory is not 'latest' instead of 'kernel'.
# 	[91/07/11            danner]
# 
# Revision 2.4.3.1  91/08/19  13:49:46  danner
# 	Fixed a comment.
# 	[91/07/21            danner]
# 	Missed one pmax/mips thingie.
# 	[91/07/13            danner]
# 
# 	Fixed mistake where pmax_mach was a link to pmax instead of mips.
# 	 This is the only machine were @sys_mach should not be a link to
# 	 @sys. Sigh.
# 	[91/07/11            danner]
# 
# 	Added i860 to machines. Changed directory warning to warn if the
# 	 current directory is not 'latest' instead of 'kernel'.
# 	[91/07/11            danner]
# 
# Revision 2.4  91/07/11  11:01:37  danner
# 	Copyright Fixes
# 
# Revision 2.3  91/07/08  16:59:51  danner
# 	Added luna88k support.
# 	Reorganized code to use loops instead of endlessly repeated code.
# 	[91/06/26            danner]
# 
# Revision 2.2  90/06/02  15:14:35  rpd
# 	Added i386 cases.
# 	[90/05/03            dbg]
# 
# 	Updated for new IPC.
# 
# 	Added pmax cases.
# 	[89/11/29            af]
# 
# 	Created from rpd's machtree
# 	[89/10/10            rwd]
# 
# Revision 2.12  89/04/08  23:33:20  rpd
# 	Put back code to link sys to h, for pre-X77 kernels.
# 	Removed a bunch of progress messages from MACHINE creation;
# 	made it create MACHINE/mach and MACHINE/mach_debug.
# 	Create mach/machine and mach_debug/machine links.
# 	[89/03/14  15:00:25  rpd]
# 

#
# A script to generate a complete set of kernel sources for any RCS
# configuration.  The result is a ready-to-compile directory, populated
# with RCS links, sources, and symbolic links for "machine" and "sys" (or "h").
#
#
# Architectures we generate for 
#
MACHINES="vax sun3 pmax i386 luna88k i860"


# Our name, for warnings.
ProgName="`expr \"$0\" : '.*/\(.*\)' '|' \"$0\"`"

# Location of kernel RCS tree
KernelRCS=/afs/cs.cmu.edu/project/mach-7/mk/rcs

# Location of kernel latest tree
KernelLatest=/afs/cs.cmu.edu/project/mach-7/mk/src/latest

# Location of kernel merge tree
KernelMerge=/afs/cs.cmu.edu/project/mach-7/mk/src/merge

# Location of VERSIONS file
Versions="$KernelMerge/VERSIONS"

# Directories that we will process.
# -dir adds to this list.
# Normals args are sets of dirs from Directories/
# Defaults to "."
Dirs=

# -co sets CheckOut on; -noco sets CheckOut off
# -co is the default
CheckOut=yes

# -q sets Quiet on; -v sets Quiet off
# -v is the default
Quiet=

# -i specifies interactive prompting; -f forces the default
# the default is -i
Force=

# -b <version> specifies a base version
# the default is to prompt the user (unless Force)
BaseVersion=

# -n <name> specifies a branch name
# the default is to prompt the user (unless Force)
BranchName=

while [ "$#" -gt 0 ]; do
	case "$1" in
	  -help) echo "$ProgName: Summary of switches:";
		 echo "  -dir <dir>: specify a dir to process";
		 echo "  -co: check out subdirectories (default)";
		 echo "  -noco: don't check out subdirectories";
		 echo "  -q: be quiet";
		 echo "  -v: be verbose (default)";
		 echo "  -i: ask questions (default)";
		 echo "  -f: don't ask questions; use default answer";
		 echo "  -b <version>: specify a base version";
		 echo "  -n <name>: specify a branch name";
		 echo "  -help: this summary";
		 exit 0;;
	  -dir) shift;
		if [ "$1" = "." ]; then
			Dirs="$Dirs $1";
		elif expr "$1" : '\./' > /dev/null; then
			Dirs="$Dirs $1";
		else
			Dirs="$Dirs ./$1";
		fi
		shift;;
	  -co) CheckOut=yes; shift;;
	  -noco) CheckOut=; shift;;
	  -q) Quiet=yes; shift;;
	  -v) Quiet=; shift;;
	  -i) Force=; shift;;
	  -f) Force=yes; shift;;
	  -b) shift; BaseVersion="$1"; shift;;
	  -n) shift; BranchName="$1"; shift;;
	  -*) echo "$ProgName: unrecognized switch $1 (try -help)" 1>&2; exit 1;;
	  *) break;;
	esac
done

#
# The b-style rcs cover program doesn't work properly
# unless the last component of the directory name is "latest".
# (We really should check for mk/src/latest)
#

if [ "`expr \"\`pwd\`\" : '.*/\(.*\)'`" != latest ]; then
	echo "$ProgName: WARNING: directory name not 'latest'" 1>&2
fi

if [ -d RCS ]; then
	if [ ! "$Quiet" ]; then echo "[ RCS already exists ]"; fi
else
	default=yes;
	if [ "$Force" ]; then answer="$default"; else
		echo -n "Link RCS to $KernelRCS? [$default] "
		read answer
		if [ -z "$answer" ]; then answer="$default"; fi
	fi

	if [ "$answer" = "yes" ] || [ "$answer" = "y" ]; then
		if [ ! "$Quiet" ]; then echo "[ linking RCS to $KernelRCS ]"; fi
		ln -s "$KernelRCS" RCS
	fi

	if [ ! -d RCS ]; then
		echo "$ProgName: You must have an RCS directory" 1>&2
		exit 1
	fi
fi

if [ ! -d RCS/Directories ]; then
	echo "$ProgName: You must have an RCS/Directories directory" 1>&2
	exit 1
fi

if [ -r RCSconfig ]; then
	if [ ! "$Quiet" ]; then echo "[ RCSconfig already exists ]"; fi
else
	if [ ! "$BaseVersion" ]; then
		default="`awk '{ v = $1; } END { print v; }' \"$Versions\"`"
		if [ "$Force" ]; then answer="$default"; else
			echo -n "Base version? [$default] "
			read answer
			if [ -z "$answer" ]; then answer="$default"; fi
		fi
		BaseVersion="$answer"
	fi

	# for some reason, can't put "" around `awk ...`

	date=`awk 'BEGIN { mon["Jan"] = "01"; mon["Feb"] = "02"; mon["Mar"] = "03";
                           mon["Apr"] = "04"; mon["May"] = "05"; mon["Jun"] = "06";
                           mon["Jul"] = "07"; mon["Aug"] = "08"; mon["Sep"] = "09";
                           mon["Oct"] = "10"; mon["Nov"] = "11"; mon["Dec"] = "12"; }
                    $1 == "'"$BaseVersion"'" {
printf("%s/%s/%02d,%s\n", substr($8, 3, 2), mon[$4], $5, $6); }' "$Versions"`

	if [ -z "$date" ]; then
		echo "$ProgName: Unknown version $answer specified" 1>&2
		exit 1
	fi

	if [ ! "$BranchName" ]; then
		default="${USER}_${BaseVersion}"
		if [ "$Force" ]; then answer="$default"; else
			echo -n "Branch name? [$default] "
			read answer
			if [ -z "$answer" ]; then answer="$default"; fi
		fi
		BranchName="$answer"
	fi

	if [ ! "$Quiet" ]; then
		echo "[ creating RCSconfig ]"
		echo "[   branch: $BranchName ]"
		echo "[   date:   $date ($BaseVersion) ]"
		echo "[   vbase:  $KernelRCS ]"
	fi
	rm -f RCSconfig
	echo "$BranchName" >> RCSconfig
	echo "<$date" >> RCSconfig
	echo "-relative_to $KernelRCS" >> RCSconfig

	if [ ! "$Quiet" ]; then echo "[ linking RCSCONFIG to RCSconfig ]"; fi
	rm -f RCSCONFIG
	ln -s RCSconfig RCSCONFIG

	if [ ! "$Quiet" ]; then echo "[ creating .BCSVBASE ]"; fi
	rm -f .BCSVBASE
	echo "$KernelRCS" > .BCSVBASE

	if [ ! "$Quiet" ]; then echo "[ creating .BCSBRANCH ]"; fi
	rm -f .BCSBRANCH
	echo "$BranchName" > .BCSBRANCH

	if [ ! "$Quiet" ]; then echo "[ linking .BCSconfig-$BranchName to RCSconfig ]"; fi
	rm -f ".BCSconfig-$BranchName"
	ln -s RCSconfig ".BCSconfig-$BranchName"
fi

if [ -d Directories ]; then
	if [ ! "$Quiet" ]; then echo "[ Directories already exists ]"; fi
else
	if [ ! "$Quiet" ]; then echo "[ creating Directories ]"; fi
	rm -rf Directories
	mkdir Directories

	if [ ! "$Quiet" ]; then echo "[ linking Directories/RCSconfig to ../RCSCONFIG ]"; fi
	ln -s ../RCSCONFIG Directories/RCSconfig

	if [ ! "$Quiet" ]; then echo "[ linking Directories/RCS to ../RCS/Directories ]"; fi
	ln -s ../RCS/Directories Directories/RCS

	if [ ! "$Quiet" ]; then echo "[ checking out Directories ]"; fi
	(cd Directories; rcsco -q RCS/*,v)
fi

if [ "$*" ]; then
	Dirs="$Dirs `(cd Directories; cat $@) | egrep -v '^#'`"
fi

if [ ! "$Dirs" ]; then
	Dirs="."
fi

echo "$Dirs" | tr -s ' ' '\12' | sort -u | awk '
{
  if (NF == 0) next;

  dots=""; dir = $1;
  for (i=0; i < length(dir); i++)
    if (substr(dir,i,1) == "/")
      dots = "../" dots;
  if (dots == "")
    dots = "/";
  if (substr(dir,1,2) == "./")
    dir = substr(dir,3,length(dir)-2);

  printf("%s %s\n", dots, dir);
}' | while read dots dir; do
	if [ -d "$dir" ]; then
		if [ "$dir" != "." ]; then
			if [ ! "$Quiet" ]; then echo "[ $dir already exists ]"; fi
		fi
	else
		if [ ! "$Quiet" ]; then echo "[ creating $dir ]"; fi
		rm -rf "$dir"
		mkdir "$dir"

		if [ ! "$Quiet" ]; then echo "[ linking $dir/RCSconfig to ${dots}RCSCONFIG ]"; fi
		ln -s "${dots}RCSCONFIG" "$dir/RCSconfig"

		if [ ! "$Quiet" ]; then echo "[ linking $dir/RCS to ${dots}RCS/$dir ]"; fi
		ln -s "${dots}RCS/$dir" "$dir/RCS"
	fi

	if [ "$CheckOut" ]; then
		(cd "$dir" &&
		 if [ ! -d RCS ]; then
			echo "$ProgName: WARNING: no access to $dir" 1>&2
			if [ ! "$Quiet" ]; then echo "[ skipping check out of $dir ]"; fi
		 elif [ "`echo RCS/*,v`" = "RCS/*,v" ]; then
			if [ ! "$Quiet" ]; then echo "[ no *,v files in $dir ]"; fi
		 else
			if [ ! "$Quiet" ]; then echo "[ checking out $dir ]"; fi
			rcsco -q RCS/*,v
		 fi)
	else
		if [ ! "$Quiet" ]; then echo "[ skipping check out of $dir ]"; fi
	fi
done

if [ -f RCSlog ]; then
	if [ ! "$Quiet" ]; then echo "[ removing RCSlog ]"; fi
	rm -f RCSlog
fi
if [ -f RCSset ]; then
	if [ ! "$Quiet" ]; then echo "[ removing RCSset ]"; fi
	rm -f RCSset
fi

OBJS=../../obj
if [ ! -d "$OBJS" ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$OBJS" ]"; fi
	rm -rf "$OBJS"
	mkdir "$OBJS"
fi

for F in "" /latest /latest/kernel /latest/user /latest/user/libmach /latest/user/threads
do
for M in $MACHINES 
do
if [ ! -d "$OBJS"/"$M"_mach"$F" ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$OBJS"/"$M"_mach"$F" ]"; fi
	rm -rf "$OBJS"/"$M"_mach"$F"
	mkdir "$OBJS"/"$M"_mach"$F"
fi
done
done


RELS=../../release
if [ ! -d "$RELS" ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$RELS" ]"; fi
	rm -rf "$RELS"
	mkdir "$RELS"
fi


for F in "" /latest /latest/lib /latest/bin /latest/include 
do
for M in $MACHINES 
do
if [ ! -d "$RELS"/"$M"_mach"$F" ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$RELS"/"$M"_mach"$F" ]"; fi
	rm -rf "$RELS"/"$M"_mach"$F"
	mkdir "$RELS"/"$M"_mach"$F"
fi
done
done


if [ ! -d RCSTMP ]; then
	if [ ! "$Quiet" ]; then echo "[ creating RCSTMP ]"; fi
	rm -rf RCSTMP
	mkdir RCSTMP
	chmod a+w RCSTMP
fi

cd kernel
if [ ! "$Quiet" ]; then echo "[ creating kernel/machine ]"; fi
rm -rf machine
ln -s @sys machine
for M in $MACHINES 
do
#
# pmax kludge 
#
if [ "$M" = "pmax" ]; then ln -s mips pmax_mach; 
else ln -s "$M" "$M"_mach;  fi 
done

cd mach
if [ ! "$Quiet" ]; then echo "[ creating kernel/mach/machine ]"; fi
rm -rf machine
ln -s @sys machine
for M in $MACHINES 
do
#
# pmax kludge 
#
if [ "$M" = "pmax" ]; then ln -s mips pmax_mach; 
else ln -s "$M" "$M"_mach;  fi 
done

cd ../../user/libmach
if [ ! "$Quiet" ]; then echo "[ creating user/libmach/machine ]"; fi
rm -rf machine
ln -s @sys machine
for M in $MACHINES 
do
#
# pmax kludge 
#
if [ "$M" = "pmax" ]; then ln -s mips pmax_mach; 
else ln -s "$M" "$M"_mach;  fi 
done

cd ../threads
if [ ! "$Quiet" ]; then echo "[ creating user/threads/machine ]"; fi
rm -rf machine
ln -s @sys machine
for M in $MACHINES 
do
#
# pmax kludge 
#
if [ "$M" = "pmax" ]; then ln -s mips pmax_mach; 
else ln -s "$M" "$M"_mach;  fi 
done

cd ../..

exit 0
