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
$ @(#)nwords.msg	1.3  com/bsd.d/style.d,3.1,9021 3/13/90 16:42:08
$
$ COMPONENT_NAME: (CMDTEXT) Text Formatting
$
$ FUNCTIONS: nwords.msg
$
$ ORIGINS: 3,27,28
$
$ This module contains IBM CONFIDENTIAL code. -- (IBM
$ Confidential Restricted when combined with the aggregated
$ modules for this product)
$ OBJECT CODE ONLY SOURCE MATERIALS
$ (C) COPYRIGHT International Business Machines Corp. 1989
$ All Rights Reserved
$
$ US Government Users Restricted Rights - Use, duplication or
$ disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
$
$quote "         define quote character.

$len 
$set MS_nwords        Define initial set#
$ Do not translate output formats used with C programs such as:
$ 	%d %i %x %X %u %c %s %f %e %E %g %G %p %n %ld
$ Do not translate shell variables such as:
$ 	$# $? $$ $! $- {$0 $1 $2 .... $some number}

M_MSG_1  "style: Sentence %d has too many words.\n"
M_MSG_2  "style: Sentence %d has too many characters.\n"
M_MSG_8  "style: Cannot find or open %s\n"
M_MSG_11  "Readability grades:\n\
	(Kincaid) %4.1f  (auto) %4.1f  (Coleman-Liau) %4.1f  (Flesch) %4.1f (%4.1f)\n"
M_MSG_12  "sentence info:\n"
M_MSG_13  "	number of sentence %d number of words %ld\n"
M_MSG_14  "	average sentence length %4.1f average word length %4.2f\n"
M_MSG_15  "	number of questions %$1d number of imperatives %$2d\n"
M_MSG_16  "	number of nonfunction words %ld  %4.1f%%   average length %4.2f\n"
M_MSG_17  "	short sentence (<%d)%3.0f%% (%d) long sentence (>%d) %3.0f%% (%d)\n"
M_MSG_18  "	longest sentence %1$d words at sentence %2$d; shortest sentence %3$d words at sentence %4$d\n"
M_MSG_19  "sentence types:\n"
M_MSG_20  "	simple %3.0f%% (%d) complex %3.0f%% (%d)\n"
M_MSG_21  "	compound %3.0f%% (%d) compound-complex %3.0f%% (%d)\n"
M_MSG_22  "word usage:\n"
M_MSG_23  "	verb types as %% of total verbs\n"
M_MSG_24  "	tobe %3.0f%% (%d) auxiliary %3.0f%% (%d) infinitive %3.0f%% (%d)\n"
M_MSG_25  "	passives as %% of non-infinitive verbs %3.0f%% (%d)\n"
M_MSG_26  "	types as %% of total\n"
M_MSG_27  "	prepositions %3.1f%% (%d) conjunctions %3.1f%% (%d) adverbs %3.1f%% (%d)\n"
M_MSG_28  "	noun %3.1f%% (%d) adjectives %3.1f%% (%d) pronouns %3.1f%% (%d)\n"
M_MSG_29  "	nominalizations %3.0f %% (%d)\n"
M_MSG_30  "sentence beginnings:\n"
M_MSG_31  "	subject opener: noun (%d) pronouns (%d) possessive (%d) adjectives (%d) article (%d) tot %3.0f%%\n"
M_MSG_32  "	prepositions %3.0f%% (%d) adverbs %3.0f%% (%d) \n"
M_MSG_33  "	verb %3.0f%% (%d) "
M_MSG_34  "sub_conjunctions %3.0f%% (%d) conjunctions %3.0f%% (%d)\n"
M_MSG_35  "	expletives %3.0f%% (%d)\n"
M_MSG_44  "style: The %s is not valid for part.l.\n"

