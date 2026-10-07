.\" 
.\" $Copyright
.\" Copyright 1993, 1994, 1995  Intel Corporation
.\" INTEL CONFIDENTIAL
.\" The technical data and computer software contained herein are subject
.\" to the copyright notices; trademarks; and use and disclosure
.\" restrictions identified in the file located in /etc/copyright on
.\" this system.
.\" Copyright$
.\" 
 
.\"
.\" (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
.\" ALL RIGHTS RESERVED
.\"
.\"
.\" OSF/1 Release 1.0
.	\" @(#)ths.ms	1.2 4/22/88
.	\" TM - U.C. Berkeley thesis mode
.ls 2
.if v .nr PO 1.5i
.rm CH
.ds RH \\n(PN
.rm CF
.P1
.nr TM 1
.	\" CT - chapter title 
.de CT
.rn RH R@
.if \\n(1T .bp
.RT
.if \\n(1T .sp
.if !\\n(1T .BG
.RT
.ft 3
.if n .ul 100
.ps \\n(PS+2
.ce 100
.nr CT 1
.rn R@ RH
..
