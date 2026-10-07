/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
#if POST_SECTION

#ifndef MCMSG_CONFIG_H
#define POST_VCFINIT (VCF_POST_BASE+0)
#endif MCMSG_CONFIG_H

POST_DEFINE(POST_VCFINIT, "VCFINIT", vcf_init, 5, MCMSG_MODULE_VCF, SYS)

#ifndef MCMSG_CONFIG_H
#define POST_VCFOPEN (VCF_POST_BASE+1)
#endif MCMSG_CONFIG_H

POST_DEFINE(POST_VCFOPEN, "VCFOPEN", vcf_open, 7, MCMSG_MODULE_VCF, USER)

#ifndef MCMSG_CONFIG_H
#define POST_VCFCLOSE (VCF_POST_BASE+2)
#endif MCMSG_CONFIG_H

POST_DEFINE(POST_VCFCLOSE, "VCFCLOSE", vcf_close, 3, MCMSG_MODULE_VCF, USER)

#ifndef MCMSG_CONFIG_H
#define POST_VCFSEND (VCF_POST_BASE+3)
#endif MCMSG_CONFIG_H

POST_DEFINE(POST_VCFSEND, "VCFSEND", vcf_send, 4, MCMSG_MODULE_VCF, USER)

#ifndef MCMSG_CONFIG_H
#define POST_VCFRECV (VCF_POST_BASE+4)
#endif MCMSG_CONFIG_H

POST_DEFINE(POST_VCFRECV, "VCFRECV", vcf_recv, 4, MCMSG_MODULE_VCF, USER)

#ifndef MCMSG_CONFIG_H
#define POST_VCFSWAP (VCF_POST_BASE+5)
#endif MCMSG_CONFIG_H

POST_DEFINE(POST_VCFSWAP, "VCFSWAP", vcf_swap, 1, MCMSG_MODULE_VCF, SYS)

#endif POST_SECTION
#if PACKET_SECTION

#ifndef MCMSG_CONFIG_H
#define VCF_HWORD_RD (VCF_PACKET_BASE+0)
#endif MCMSG_CONFIG_H

PACKET_DEFINE(VCF_HWORD_RD, "RD", "VCF_HWORD_RD", vcf_handle_rd, vcf_handle_rd)

#ifndef MCMSG_CONFIG_H
#define VCF_HWORD_RDA (VCF_PACKET_BASE+1)
#endif MCMSG_CONFIG_H

PACKET_DEFINE(VCF_HWORD_RDA, "RDA", "VCF_HWORD_RDA", vcf_handle_rda, vcf_handle_rda)

#ifndef MCMSG_CONFIG_H
#define VCF_HWORD_RDAC (VCF_PACKET_BASE+2)
#endif MCMSG_CONFIG_H

PACKET_DEFINE(VCF_HWORD_RDAC, "RDAC", "VCF_HWORD_RDAC", vcf_handle_rdac, vcf_handle_rdac)

#ifndef MCMSG_CONFIG_H
#define VCF_HWORD_ROC (VCF_PACKET_BASE+3)
#endif MCMSG_CONFIG_H

PACKET_DEFINE(VCF_HWORD_ROC, "ROC", "VCF_HWORD_ROC", vcf_handle_roc, vcf_handle_roc)

#ifndef MCMSG_CONFIG_H
#define VCF_HWORD_RCC (VCF_PACKET_BASE+4)
#endif MCMSG_CONFIG_H

PACKET_DEFINE(VCF_HWORD_RCC, "RCC", "VCF_HWORD_RCC", vcf_handle_rcc, vcf_handle_rcc)

#ifndef MCMSG_CONFIG_H
#define VCF_HWORD_RJG (VCF_PACKET_BASE+5)
#endif MCMSG_CONFIG_H

PACKET_DEFINE(VCF_HWORD_RJG, "RJG", "VCF_HWORD_RJG", vcf_handle_rjg, vcf_handle_rjg)

#ifndef MCMSG_CONFIG_H
#define VCF_HWORD_RJOC (VCF_PACKET_BASE+6)
#endif MCMSG_CONFIG_H

PACKET_DEFINE(VCF_HWORD_RJOC, "RJOC", "VCF_HWORD_RJOC", vcf_handle_rjoc, vcf_handle_rjoc)

#endif POST_SECTION
