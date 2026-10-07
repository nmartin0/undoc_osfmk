#!/bin/csh -f
# 
# $Copyright
# Copyright 1993, 1994, 1995  Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
# 
 
# (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
# ALL RIGHTS RESERVED
#
#
# OSF/1 Release 1.0.2

#
# @(#)$RCSfile: which.csh,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 01:47:26 $
#
# COMPONENT_NAME: (CMDSCAN) commands that scan files
#
# FUNCTIONS:
#
# ORIGINS: 26, 27
#
# (C) COPYRIGHT International Business Machines Corp. 1989
# All Rights Reserved
# Licensed Materials - Property of IBM
#
# US Government Users Restricted Rights - Use, duplication or
# disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
#
# (Copyright statements and/or associated legends of other
# companies whose code appears in any part of this module must
# be copied here.)
#
#                                                                    
# Copyright (c) 1980 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	which.csh	5.2 (Berkeley) 7/29/85
#
#	which : tells you which program you get
#
set _which_saved_path_ = ( $path )
set prompt = "% "
if ( -f ~/.cshrc) then
	source ~/.cshrc
endif
set path = ( $_which_saved_path_ )
unset prompt _which_saved_path_
set noglob
foreach arg ( $argv )
    set alius = `alias $arg`
    switch ( $#alius )
	case 0 :
	    breaksw
	case 1 :
	    set arg = $alius[1]
	    breaksw
        default :
	    echo ${arg}: "	" aliased to $alius
	    continue
    endsw
    unset found
    if ( $arg:h != $arg:t ) then
	if ( -e $arg ) then
	    echo $arg
	else
	    dspmsg which.cat 1 '%1$s not found\n' "$arg"
	endif
	continue
    else
	foreach i ( $path )
	    if ( -x $i/$arg && ! -d $i/$arg ) then
		echo $i/$arg
		set found
		break
	    endif
	end
    endif
    if ( ! $?found ) then
	dspmsg which.cat 2 'no %1$s in %2$s\n' "$arg" "$path" 
    endif
end




