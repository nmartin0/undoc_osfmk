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
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 *
 */

/*
 * rose_names.c
 * Calls generic flag/type-name routines with OSF/ROSE-specific name arrays.
 */

#include <mach_o_header.h>
#include <mach_o_format.h>
/* these in turn include ...types and ...vals */
#include <standards.h>
#include "tpfl_desc.h"
#include "rose_names.h"
#include "rose_namegrp.h"


void
rose_flags_print(long flags, int namegrp)
{

  switch (namegrp) {
case REGC_INITPROT_SHORT_NAMES:
	  flag_names_print (flags, regc_known_prot_mask, 
			    regc_prot_short_array, regc_prot_count);
	  break;

case REGC_FLAGS_STRIPPED_NAMES:
	  flag_names_print (flags, regc_known_flags_mask, 
			    regc_flags_stripped_array, regc_flags_count);
	  break;

case PKGC_FLAGS_STRIPPED_NAMES:
	  flag_names_print (flags, pkgc_known_flags_mask, 
			    pkgc_flags_stripped_array, pkgc_flags_count);
	  break;

case SI_FLAGS_SHORT_NAMES:
	  flag_names_print (flags, si_known_flags_mask, 
			    si_flags_short_array, si_flags_count);
	  break;

case ENTC_FLAGS_STRIPPED_NAMES:
	  flag_names_print (flags, entc_known_flags_mask, 
			    entc_flags_stripped_array, entc_flags_count);
	  break;

case MOH_FLAGS_STRIPPED_NAMES:
	  flag_names_print (flags, moh_known_flags_mask, 
			    moh_flags_stripped_array, moh_flags_count);
	  break;

case RI_FLAGS_SHORT_NAMES:
	  flag_names_print (flags, ri_known_flags_mask, 
			    ri_flags_short_array, ri_flags_count);
	  break;

default:
	  error("Unknown flag name group %d.\n", namegrp);
  }
}		/* end of rose_flags_print */


void
rose_flags_get (long flags, int namegrp, char * name_string)
{

  switch (namegrp) {
case REGC_INITPROT_SHORT_NAMES:
	  flag_names_get (flags, regc_known_prot_mask, 
			  regc_prot_short_array, regc_prot_count,
			  name_string);
	  break;

case REGC_FLAGS_STRIPPED_NAMES:
	  flag_names_get (flags, regc_known_flags_mask, 
			  regc_flags_stripped_array,
			  regc_flags_count, name_string);
	  break;

case PKGC_FLAGS_STRIPPED_NAMES:
	  flag_names_get (flags, pkgc_known_flags_mask, 
			    pkgc_flags_stripped_array,
			    pkgc_flags_count, name_string);
	  break;

case SI_FLAGS_SHORT_NAMES:
	  flag_names_get (flags, si_known_flags_mask, 
			  si_flags_short_array, si_flags_count,
			  name_string);
	  break;

case ENTC_FLAGS_STRIPPED_NAMES:
	  flag_names_get (flags, entc_known_flags_mask, 
			  entc_flags_stripped_array,
			  entc_flags_count, name_string);
	  break;

case MOH_FLAGS_STRIPPED_NAMES:
	  flag_names_get (flags, moh_known_flags_mask, 
			  moh_flags_stripped_array, moh_flags_count,
			  name_string);
	  break;

case RI_FLAGS_SHORT_NAMES:
	  flag_names_get (flags, ri_known_flags_mask, 
			  ri_flags_short_array, ri_flags_count,
			  name_string);
	  break;

default:
	  error("Unknown flag name group %d.\n", namegrp);
  }
}		/* end of rose_flags_get */


void
rose_type_print (int type, int namegrp)
{

  switch (namegrp) {

case LDC_CMD_TYPE_STRIPPED_NAMES:
	  type_name_print (type, ldc_cmd_type_stripped_array,
			   ldc_cmd_type_count);
	  break;

case REGC_USAGE_TYPE_STRIPPED_NAMES:
	  type_name_print (type, regc_usage_type_stripped_array,
			   regc_usage_type_count);
	  break;

case RI_SIZE_TYPE_STRIPPED_NAMES:
	  type_name_print (type, ri_size_type_stripped_array,
			   ri_size_type_count);
	  break;

case SYMC_KIND_STRIPPED_NAMES:
	  type_name_print (type, symc_kind_stripped_array,
			   symc_kind_count);
	  break;

case MOH_BYTE_ORDER_STRIPPED_NAMES:
	  type_name_print (type, moh_byte_order_stripped_array,
			   moh_byte_order_count);
	  break;

case MOH_DATA_REP_ID_NAMES:
	  type_name_print (type, moh_data_rep_id_array,
			   moh_data_rep_id_count);
	  break;

case MOH_CPU_TYPE_STRIPPED_NAMES:
	  type_name_print (type, moh_cpu_type_stripped_array,
			   moh_cpu_type_count);
	  break;

case MOH_CPU_SUBTYPE_STRIPPED_NAMES:
	  type_name_print (type, moh_cpu_subtype_stripped_array,
			   moh_cpu_subtype_count);
	  break;

case MOH_VENDOR_TYPE_STRIPPED_NAMES:
	  type_name_print (type, moh_vendor_type_stripped_array,
			   moh_vendor_type_count);
	  break;

case FNTC_TYPE_STRIPPED_NAMES:
	  type_name_print (type, fntc_type_stripped_array,
			   fntc_type_count);
	  break;

case MO_ERROR_MESSAGES:
	  type_name_print (type, mo_error_messages_array,
			   mo_error_messages_count);
	  break;

default:
	  error("Unknown type name group %d.\n", namegrp);
  }
}		/* end of rose_type_print */


void
rose_type_get (int type, int namegrp, char * name_string)
{

  switch (namegrp) {

case LDC_CMD_TYPE_STRIPPED_NAMES:
	  type_name_get (type, ldc_cmd_type_stripped_array,
			     ldc_cmd_type_count, name_string);
	  break;

case REGC_USAGE_TYPE_STRIPPED_NAMES:
	  type_name_get (type, regc_usage_type_stripped_array,
			 regc_usage_type_count, name_string);
	  break;

case RI_SIZE_TYPE_STRIPPED_NAMES:
	  type_name_get (type, ri_size_type_stripped_array,
			 ri_size_type_count, name_string);
	  break;

case SYMC_KIND_STRIPPED_NAMES:
	  type_name_get (type, symc_kind_stripped_array,
			 symc_kind_count, name_string);
	  break;

case MOH_BYTE_ORDER_STRIPPED_NAMES:
	  type_name_get (type, moh_byte_order_stripped_array,
			 moh_byte_order_count, name_string);
	  break;

case MOH_DATA_REP_ID_NAMES:
	  type_name_get (type, moh_data_rep_id_array,
			 moh_data_rep_id_count, name_string);
	  break;

case MOH_CPU_TYPE_STRIPPED_NAMES:
	  type_name_get (type, moh_cpu_type_stripped_array,
			 moh_cpu_type_count, name_string);
	  break;

case MOH_CPU_SUBTYPE_STRIPPED_NAMES:
	  type_name_get (type, moh_cpu_subtype_stripped_array,
			 moh_cpu_subtype_count, name_string);
	  break;

case MOH_VENDOR_TYPE_STRIPPED_NAMES:
	  type_name_get (type, moh_vendor_type_stripped_array,
			 moh_vendor_type_count, name_string);
	  break;

case  FNTC_TYPE_STRIPPED_NAMES:
	  type_name_get (type, fntc_type_stripped_array,
			 fntc_type_count, name_string);
	  break;

case MO_ERROR_MESSAGES:
	  type_name_get (type, mo_error_messages_array,
			 mo_error_messages_count, name_string);
	  break;

default:
	  error("Unknown type name group %d.\n", namegrp);
  }
}		/* end of rose_type_get */
