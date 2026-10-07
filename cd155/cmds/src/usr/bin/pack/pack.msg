$ 
$ $Copyright
$ Copyright 1993, 1994, 1995  Intel Corporation
$ INTEL CONFIDENTIAL
$ The technical data and computer software contained herein are subject
$ to the copyright notices; trademarks; and use and disclosure
$ restrictions identified in the file located in /etc/copyright on
$ this system.
$ Copyright$
$ 
 
$
$ (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
$ ALL RIGHTS RESERVED
$
$
$ OSF/1 Release 1.0

$
$ @(#) $RCSfile: pack.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:32:49 $
$
$ COMPONENT_NAME: (CMDFILES) commands that manipulate files
$
$ FUNCTIONS:
$
$ ORIGINS: 27
$
$ IBM CONFIDENTIAL -- (IBM Confidential Restricted when
$ combined with the aggregated modules for this product)
$ OBJECT CODE ONLY SOURCE MATERIALS
$ (C) COPYRIGHT International Business Machines Corp. 1985, 1989
$ All Rights Reserved
$
$ US Government Users Restricted Rights - Use, duplication or
$ disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
$
$ "pack.msg	1.7  com/cmd/files,3.1,9013 12/1/89 15:10:38"
$
$
$quote "
$set MS_PACK
$		GLOBAL COMMENT FOR TRANSLATORS
$ Do not translate pack: -- this is the command name.
$ Do not translate %o %s %1$s %2$s etc.
$ These are output formats for file names, directory
$ names, permission codes, command names, and the like,
$ depending on their context within each message. 
$ The program replaces %o,%1$o,etc with an octal number.
$ The program replaces %d,%1$d,etc with a decimal number.
$ the program replaces %.2f with a fixed-point number.
$ The program replaces %.1f%% with a percentage.
$ The program replaces %s,%1$s,etc with a string of 
$ characters.   
$ 
READERR ": There is a read error.\n"
$ This message is followed by the message FILUNCHN. 
$ 
READERR1 ": There is a read error.\n"
$ This message is followed by the message FILUNCHN. 
$ 
WRITERR ".z: There is a write error while creating the .z file.\n"
$ This message is followed by the message FILUNCHN. 
$ 
TRIVIAL ": There is no saving in file space.\n"
$ This message is followed by the message FILUNCHN. 
$ 
LVLCNT ": The file is too large or too complex to pack.\n"
$ This message is followed by the message FILUNCHN. 
$ 
NOSAVE ": Packing this file saves no storage space.\n"
$ This message is followed by the message FILUNCHN. 
$ 
PCKDMSG ": The specified file is already packed.\n"
$ 
FILNAMLEN ": The file name is too long.\n"
$ 
CANTOPEN ": Cannot open the specified file.\n"
$ 
PACKDIR ": Cannot pack a directory.\n"
$ 
LINKCNT ": Cannot pack the specified file.\n"
$ 
ZEXISTS ".z: A file by this name already exists.\n"
$ 
CANTCREAT ".z: Cannot create the specified file.\n"
$ 
CHMODMSG "Cannot change the mode to %o.\n"
$ 
CANTUNLNK "%1$s: Cannot unlink %2$s.\n"
$ 
COMPRESS ": The file is reduced in size %.1f%%.\n"
$ The program replaces %.1f%% with a percentage.
$ 
SIZEMSG "\tThe file changed from %1$ld to %2$ld bytes.\n"
$ 
HUFFLVLS "\tThe Huffman tree has %d levels below the root.\n"
$ Do not translate "Huffman".  It is the name of a file building technigue.
$ 
DISTINCT "\tThere are %d distinct bytes in input.\n"
$ %d is replaced by the number of bytes in the unpacked file. 
DICTOVRHD "\tThe dictionary overhead is %ld bytes.\n"
$ %d is replaced by the number of storage medium bytes occupied by 
$ the current system dictionary. 
EFFENTROPY "\tThe effective entropy is %.2f bits per byte.\n"
$ effective entropy means the average number of bits per byte for this
$ current packed message. 
ASYMENTROPY "\tThe asymptotic entropy is %.2f bits per byte.\n"
$ asymptotic entropy means the average number of bits per byte for this
$ current group of packed messages.
FILUNCHG "\tThe file is unchanged.\n"
