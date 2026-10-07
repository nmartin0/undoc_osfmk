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
 * rose_names.h
 * Contains name strings for the types and flags in the OSF/ROSE object 
 * format, along with the corresponding values of the types and flags.
 */
#include "rose_namesmd.h"

flags_desc_t moh_flags_array[] = {
	{"MOH_RELOCATABLE_F", MOH_RELOCATABLE_F},
	{"MOH_LINKABLE_F", MOH_LINKABLE_F},
	{"MOH_EXECABLE_F", MOH_EXECABLE_F},
	{"MOH_EXECUTABLE_F", MOH_EXECUTABLE_F},
	{"MOH_UNRESOLVED_F", MOH_UNRESOLVED_F},
};
flags_desc_t moh_flags_stripped_array[] = {
	{"RELOCATABLE", MOH_RELOCATABLE_F},
	{"LINKABLE", MOH_LINKABLE_F},
	{"EXECABLE", MOH_EXECABLE_F},
	{"EXECUTABLE",  MOH_EXECUTABLE_F},
	{"UNRESOLVED", MOH_UNRESOLVED_F},
};
int moh_flags_count = array_count(moh_flags_array);
long moh_known_flags_mask = ~(MOH_RELOCATABLE_F | MOH_LINKABLE_F 
			     | MOH_EXECABLE_F | MOH_EXECUTABLE_F
			     | MOH_UNRESOLVED_F);


flags_desc_t entc_flags_array[] = {
	{"ENT_VALID_ABSADDR_F", ENT_VALID_ABSADDR_F},
};
flags_desc_t entc_flags_stripped_array[] = {
	{"VALID_ABSADDR", ENT_VALID_ABSADDR_F},
};
int entc_flags_count = array_count(entc_flags_array);
long entc_known_flags_mask = ~(ENT_VALID_ABSADDR_F);

flags_desc_t regc_flags_array[] = {
	{"REG_ABS_ADDR_F", REG_ABS_ADDR_F},
	{"REG_REL_ADDR_F", REG_REL_ADDR_F},
};
flags_desc_t regc_flags_stripped_array[] = {
	{"ABS_ADDR", REG_ABS_ADDR_F},
	{"REL_ADDR", REG_REL_ADDR_F},
};
int regc_flags_count = array_count(regc_flags_array);
long regc_known_flags_mask = ~(REG_ABS_ADDR_F | REG_REL_ADDR_F);

flags_desc_t regc_prot_array[] = {
	{"MO_PROT_NONE", MO_PROT_NONE},
	{"MO_PROT_READ", MO_PROT_READ},
	{"MO_PROT_WRITE", MO_PROT_WRITE},
	{"MO_PROT_EXECUTE", MO_PROT_EXECUTE},
};
flags_desc_t regc_prot_short_array[] = {
	{"NONE", MO_PROT_NONE},
	{"R", MO_PROT_READ},
	{"W", MO_PROT_WRITE},
	{"E", MO_PROT_EXECUTE},
};
int regc_prot_count = array_count(regc_prot_array);
long regc_known_prot_mask = ~(MO_PROT_READ | MO_PROT_WRITE | MO_PROT_EXECUTE);

flags_desc_t ri_flags_array[] = {
	{"RI_SYMBOL_F", RI_SYMBOL_F},
	{"RI_LOC_F", RI_LOC_F},
	{"RI_PC_REL_F", RI_PC_REL_F},
	{"RI_INDIRECT_F", RI_INDIRECT_F},
	{"RI_RELOC_VADDR_F", RI_RELOC_VADDR_F},
	{"RI_RELOC_OFFSET_F", RI_RELOC_OFFSET_F},
};
flags_desc_t ri_flags_short_array[] = {
	{"SYM", RI_SYMBOL_F},
	{"LOC", RI_LOC_F},
	{"PCR", RI_PC_REL_F},
	{"IND", RI_INDIRECT_F},
	{"VAD", RI_RELOC_VADDR_F},
	{"OFF", RI_RELOC_OFFSET_F},
};
int ri_flags_count = array_count(ri_flags_array);
long ri_known_flags_mask = ~(RI_SYMBOL_F | RI_LOC_F | RI_PC_REL_F
			     | RI_INDIRECT_F | RI_RELOC_VADDR_F 
			     | RI_RELOC_OFFSET_F);

flags_desc_t si_flags_array[] = {
	{"SI_EXPORT_F", SI_EXPORT_F},
	{"SI_IMPORT_F", SI_IMPORT_F},
	{"SI_LOCAL_F", SI_LOCAL_F},
	{"SI_CODE_F", SI_CODE_F},
	{"SI_DATA_F", SI_DATA_F},
	{"SI_LITERAL_F", SI_LITERAL_F},
	{"SI_FORWARD_F", SI_FORWARD_F},
	{"SI_COMMON_F", SI_COMMON_F},
	{"SI_LOCAL_LABEL_F", SI_LOCAL_LABEL_F},
	{"SI_ABSOLUTE_VALUE_F", SI_ABSOLUTE_VALUE_F},
};
flags_desc_t si_flags_short_array[] = {
	{"EXP", SI_EXPORT_F},
	{"IMP", SI_IMPORT_F},
	{"LCL", SI_LOCAL_F},
	{"COD", SI_CODE_F},
	{"DAT", SI_DATA_F},
	{"LIT", SI_LITERAL_F},
	{"FWD", SI_FORWARD_F},
	{"COM", SI_COMMON_F},
	{"LAB", SI_LOCAL_LABEL_F},
	{"ABS", SI_ABSOLUTE_VALUE_F},
};
int si_flags_count = array_count(si_flags_array);
long si_known_flags_mask = ~(SI_EXPORT_F | SI_IMPORT_F | SI_LOCAL_F | SI_CODE_F
			     | SI_DATA_F | SI_LITERAL_F | SI_FORWARD_F
			     | SI_COMMON_F | SI_LOCAL_LABEL_F
			     | SI_ABSOLUTE_VALUE_F);

flags_desc_t pkgc_flags_array[] = {
	{"PKG_EXPORT_F", PKG_EXPORT_F},
	{"PKG_IMPORT_F", PKG_IMPORT_F},
};
flags_desc_t pkgc_flags_stripped_array[] = {
	{"EXPORT", PKG_EXPORT_F},
	{"IMPORT", PKG_IMPORT_F},
};
int pkgc_flags_count = array_count(pkgc_flags_array);
long pkgc_known_flags_mask = ~(PKG_EXPORT_F | PKG_IMPORT_F);

type_desc_t moh_byte_order_array[] = {
	{"BO_LSB", BO_LSB},
	{"BO_MSB", BO_MSB},
};
type_desc_t moh_byte_order_stripped_array[] = {
	{"LSB", BO_LSB},
	{"MSB", BO_MSB},
};
int moh_byte_order_count = array_count(moh_byte_order_array);

type_desc_t moh_data_rep_id_array[] = {
	{"DREP_GCC_LSB", DREP_GCC_LSB},
};
int moh_data_rep_id_count = array_count(moh_data_rep_id_array);

type_desc_t moh_cpu_type_array[] = {
	{"MO_CPU_TYPE_MIPS", MO_CPU_TYPE_MIPS},
	{"MO_CPU_TYPE_NS32000", MO_CPU_TYPE_NS32000},
	{"MO_CPU_TYPE_I386", MO_CPU_TYPE_I386},
	{"MO_CPU_TYPE_M68000", MO_CPU_TYPE_M68000},
};
type_desc_t moh_cpu_type_stripped_array[] = {
	{"MIPS", MO_CPU_TYPE_MIPS},
	{"NS32000", MO_CPU_TYPE_NS32000},
	{"I386", MO_CPU_TYPE_I386},
	{"M68000", MO_CPU_TYPE_M68000},
};
int moh_cpu_type_count = array_count(moh_cpu_type_array);

type_desc_t moh_cpu_subtype_array[] = {
	{"MO_CPU_SUBTYPE_PMAX", MO_CPU_SUBTYPE_PMAX},
	{"MO_CPU_SUBTYPE_MMAX", MO_CPU_SUBTYPE_MMAX},
};
type_desc_t moh_cpu_subtype_stripped_array[] = {
	{"PMAX", MO_CPU_SUBTYPE_PMAX},
	{"MMAX", MO_CPU_SUBTYPE_MMAX},
};
int moh_cpu_subtype_count = array_count(moh_cpu_subtype_array);

type_desc_t moh_vendor_type_array[] = {
	{"MO_VENDOR_TYPE_OSF", MO_VENDOR_TYPE_OSF},
};
type_desc_t moh_vendor_type_stripped_array[] = {
	{"OSF", MO_VENDOR_TYPE_OSF},
};
int moh_vendor_type_count = array_count(moh_vendor_type_array);

type_desc_t ldc_cmd_type_array[] = {
	{"LDC_UNDEFINED", LDC_UNDEFINED},
	{"LDC_CMD_MAP", LDC_CMD_MAP},
	{"LDC_INTERPRETER", LDC_INTERPRETER},
	{"LDC_STRINGS", LDC_STRINGS},
	{"LDC_REGION", LDC_REGION},
	{"LDC_RELOC", LDC_RELOC},
	{"LDC_PACKAGE", LDC_PACKAGE},
	{"LDC_SYMBOLS", LDC_SYMBOLS},
	{"LDC_ENTRY", LDC_ENTRY},
	{"LDC_FUNC_TABLE", LDC_FUNC_TABLE},
	{"LDC_GEN_INFO", LDC_GEN_INFO},
};
type_desc_t ldc_cmd_type_stripped_array[] = {
	{"UNDEFINED", LDC_UNDEFINED},
	{"CMD_MAP", LDC_CMD_MAP},
	{"INTERPRETER", LDC_INTERPRETER},
	{"STRINGS", LDC_STRINGS},
	{"REGION", LDC_REGION},
	{"RELOC", LDC_RELOC},
	{"PACKAGE", LDC_PACKAGE},
	{"SYMBOLS", LDC_SYMBOLS},
	{"ENTRY", LDC_ENTRY},
	{"FUNC_TABLE", LDC_FUNC_TABLE},
	{"GEN_INFO", LDC_GEN_INFO},
};
int ldc_cmd_type_count = array_count(ldc_cmd_type_array);

type_desc_t symc_kind_array[] = {
	{"SYMC_IMPORTS", SYMC_IMPORTS},
	{"SYMC_DEFINED_SYMBOLS", SYMC_DEFINED_SYMBOLS},
	{"SYMC_STABS", SYMC_STABS},
};
type_desc_t symc_kind_stripped_array[] = {
	{"IMPORTS", SYMC_IMPORTS},
	{"DEFINED", SYMC_DEFINED_SYMBOLS},
	{"STABS", SYMC_STABS},
};
int symc_kind_count = array_count(symc_kind_array);

type_desc_t fntc_type_array[] = {
	{"FNTC_INITIALIZATION", FNTC_INITIALIZATION},
	{"FNTC_TERMINATION", FNTC_TERMINATION},
};
type_desc_t fntc_type_stripped_array[] = {
	{"INITIALIZATION", FNTC_INITIALIZATION},
	{"TERMINATION", FNTC_TERMINATION},
};
int fntc_type_count = array_count(fntc_type_array);

type_desc_t mo_error_messages_array[] = {
	{"\nBad magic number for OSF/ROSE.\n", MO_ERROR_BAD_MAGIC},
	{"\nBad OSF/ROSE header version.\n", MO_ERROR_BAD_HDR_VERS},
	{"\nBad OSF/ROSE raw header version.\n", MO_ERROR_BAD_RAW_HDR_VERS},
	{"\nHeader buffer too small.\n", MO_ERROR_BUF2SML},
	{"\nHeader file for raw header out of date.\n", MO_ERROR_OLD_RAW_HDR_FILE},
	{"\nUnsupported OSF/ROSE header version.\n", MO_ERROR_UNSUPPORTED_VERS},
};
int mo_error_messages_count = array_count(mo_error_messages_array);
