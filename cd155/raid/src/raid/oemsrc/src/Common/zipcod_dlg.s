// $Copyright
// Copyright 1992, 1993, 1994, 1995 Intel Corporation
// INTEL CONFIDENTIAL
// The technical data and computer software contained herein are subject
// to the copyright notices; trademarks; and use and disclosure
// restrictions identified in the file located in /etc/copyright on
// this system.
// Copyright$
	.file	"zipcod_dlg.c"
// PGC Rel 2.1a -opt 1 -debug
	.text
	.def size_t;	.scl 13;	.type 14;	.size 4;	.endef
	.def fpos_t;	.scl 13;	.type 4;	.size 4;	.endef
	.def .61fak;	.scl 10;	.type 8;	.size 36;	.endef
	.def _cnt;	.val 0;	.scl 8;	.type 4;	.endef
	.def _ptr;	.val 4; 	.scl 8;	.type 28;	.size 4;	.endef
	.def _base;	.val 8; 	.scl 8;	.type 28;	.size 4;	.endef
	.def _bufsiz;	.val 12;	.scl 8;	.type 4;	.endef
	.def _flag;	.val 16;	.scl 8;	.type 3;	.endef
	.def _file;	.val 18;	.scl 8;	.type 3;	.endef
	.def _unused;	.val 20; 	.scl 8;	.type 52;	.dim 3;	.size 12;	.endef
	.def _bufendp;	.val 32; 	.scl 8;	.type 28;	.size 4;	.endef
	.def .eos;	.val 36;	.scl 102;	.tag .61fak;	.size 36;	.endef
	.def FILE;	.scl 13;	.type 8;	.tag .61fak;	.size 36;	.size 36;	.endef
	.def .94fak;	.scl 10;	.type 8;	.size 16;	.endef
	.def ireg_used;	.val 0;	.scl 8;	.type 14;	.endef
	.def freg_used;	.val 4;	.scl 8;	.type 14;	.endef
	.def reg_base;	.val 8; 	.scl 8;	.type 20;	.size 4;	.endef
	.def mem_ptr;	.val 12; 	.scl 8;	.type 20;	.size 4;	.endef
	.def .eos;	.val 16;	.scl 102;	.tag .94fak;	.size 16;	.endef
	.def va_list;	.scl 13;	.type 8;	.tag .94fak;	.size 16;	.size 16;	.endef
	.def ptrdiff_t;	.scl 13;	.type 4;	.size 4;	.endef
	.def wchar_t;	.scl 13;	.type 13;	.size 2;	.endef
	.def time_t;	.scl 13;	.type 4;	.size 4;	.endef
	.def clock_t;	.scl 13;	.type 4;	.size 4;	.endef
	.def uchar_t;	.scl 13;	.type 12;	.size 1;	.endef
	.def ushort_t;	.scl 13;	.type 13;	.size 2;	.endef
	.def uint_t;	.scl 13;	.type 14;	.size 4;	.endef
	.def ulong_t;	.scl 13;	.type 14;	.size 4;	.endef
	.def .120fak;	.scl 10;	.type 8;	.size 4;	.endef
	.def r;	.val 0; 	.scl 8;	.type 52;	.dim 1;	.size 4;	.endef
	.def .eos;	.val 4;	.scl 102;	.tag .120fak;	.size 4;	.endef
	.def physadr_t;	.scl 13;	.type 24;	.tag .120fak;	.size 4;	.size 4;	.endef
	.def label_t;	.scl 10;	.type 8;	.size 120;	.endef
	.def val;	.val 0; 	.scl 8;	.type 52;	.dim 30;	.size 120;	.endef
	.def .eos;	.val 120;	.scl 102;	.tag label_t;	.size 120;	.endef
	.def label_t;	.scl 13;	.type 8;	.tag label_t;	.size 120;	.size 120;	.endef
	.def level_t;	.scl 13;	.type 4;	.size 4;	.endef
	.def daddr_t;	.scl 13;	.type 4;	.size 4;	.endef
	.def caddr_t;	.scl 13;	.type 18;	.size 4;	.endef
	.def qaddr_t;	.scl 13;	.type 20;	.size 4;	.endef
	.def ino_t;	.scl 13;	.type 14;	.size 4;	.endef
	.def cnt_t;	.scl 13;	.type 3;	.size 2;	.endef
	.def dev_t;	.scl 13;	.type 4;	.size 4;	.endef
	.def chan_t;	.scl 13;	.type 4;	.size 4;	.endef
	.def off_t;	.scl 13;	.type 4;	.size 4;	.endef
	.def paddr_t;	.scl 13;	.type 4;	.size 4;	.endef
	.def nlink_t;	.scl 13;	.type 13;	.size 2;	.endef
	.def key_t;	.scl 13;	.type 4;	.size 4;	.endef
	.def mode_t;	.scl 13;	.type 14;	.size 4;	.endef
	.def uid_t;	.scl 13;	.type 14;	.size 4;	.endef
	.def gid_t;	.scl 13;	.type 14;	.size 4;	.endef
	.def mid_t;	.scl 13;	.type 16;	.size 4;	.endef
	.def pid_t;	.scl 13;	.type 4;	.size 4;	.endef
	.def slab_t;	.scl 13;	.type 50;	.dim 12;	.size 12;	.endef
	.def sigset_t;	.scl 13;	.type 14;	.size 4;	.endef
	.def sig_t;	.scl 13;	.type 144;	.size 4;	.endef
	.def vm_offset_t;	.scl 13;	.type 14;	.size 4;	.endef
	.def vm_size_t;	.scl 13;	.type 14;	.size 4;	.endef
	.def uchar;	.scl 13;	.type 12;	.size 1;	.endef
	.def ushort;	.scl 13;	.type 13;	.size 2;	.endef
	.def uint;	.scl 13;	.type 14;	.size 4;	.endef
	.def ulong;	.scl 13;	.type 14;	.size 4;	.endef
	.def physadr;	.scl 13;	.type 24;	.tag .120fak;	.size 4;	.size 4;	.endef
	.def u_char;	.scl 13;	.type 12;	.size 1;	.endef
	.def u_short;	.scl 13;	.type 13;	.size 2;	.endef
	.def u_int;	.scl 13;	.type 14;	.size 4;	.endef
	.def u_long;	.scl 13;	.type 14;	.size 4;	.endef
	.def _quad;	.scl 10;	.type 8;	.size 8;	.endef
	.def val;	.val 0; 	.scl 8;	.type 52;	.dim 2;	.size 8;	.endef
	.def .eos;	.val 8;	.scl 102;	.tag _quad;	.size 8;	.endef
	.def quad;	.scl 13;	.type 8;	.tag _quad;	.size 8;	.size 8;	.endef
	.def swblk_t;	.scl 13;	.type 4;	.size 4;	.endef
	.def fixpt_t;	.scl 13;	.type 14;	.size 4;	.endef
	.def fd_mask;	.scl 13;	.type 4;	.size 4;	.endef
	.def fd_set;	.scl 10;	.type 8;	.size 32;	.endef
	.def fds_bits;	.val 0; 	.scl 8;	.type 52;	.dim 8;	.size 32;	.endef
	.def .eos;	.val 32;	.scl 102;	.tag fd_set;	.size 32;	.endef
	.def fd_set;	.scl 13;	.type 8;	.tag fd_set;	.size 32;	.size 32;	.endef
	.def coldesc;	.scl 10;	.type 8;	.size 8;	.endef
	.def cd_stroff;	.val 0;	.scl 8;	.type 3;	.endef
	.def cd_repoff;	.val 2;	.scl 8;	.type 3;	.endef
	.def cd_cval;	.val 4;	.scl 8;	.type 3;	.endef
	.def cd_cuniq;	.val 6;	.scl 8;	.type 3;	.endef
	.def .eos;	.val 8;	.scl 102;	.tag coldesc;	.size 8;	.endef
	.def coldesc_t;	.scl 13;	.type 8;	.tag coldesc;	.size 8;	.size 8;	.endef
	.def collation_table;	.scl 10;	.type 8;	.size 44;	.endef
	.def lc_version;	.val 0;	.scl 8;	.type 3;	.endef
	.def lc_length;	.val 2;	.scl 8;	.type 3;	.endef
	.def lc_locale_name;	.val 4; 	.scl 8;	.type 18;	.size 4;	.endef
	.def len_collate;	.val 8;	.scl 8;	.type 4;	.endef
	.def lc_collate;	.val 12; 	.scl 8;	.type 19;	.size 4;	.endef
	.def len_coluniq;	.val 16;	.scl 8;	.type 4;	.endef
	.def lc_coluniq;	.val 20; 	.scl 8;	.type 19;	.size 4;	.endef
	.def len_coldesc;	.val 24;	.scl 8;	.type 4;	.endef
	.def lc_coldesc;	.val 28; 	.scl 8;	.type 24;	.tag coldesc;	.size 8;	.size 4;	.endef
	.def len_strings;	.val 32;	.scl 8;	.type 4;	.endef
	.def lc_strings;	.val 36; 	.scl 8;	.type 29;	.size 4;	.endef
	.def high_cvalue;	.val 40;	.scl 8;	.type 4;	.endef
	.def .eos;	.val 44;	.scl 102;	.tag collation_table;	.size 44;	.endef
	.def col_t;	.scl 13;	.type 8;	.tag collation_table;	.size 44;	.size 44;	.endef
	.def char_classification_table;	.scl 10;	.type 8;	.size 32;	.endef
	.def lc_version;	.val 0;	.scl 8;	.type 3;	.endef
	.def lc_length;	.val 2;	.scl 8;	.type 3;	.endef
	.def lc_code_type;	.val 4;	.scl 8;	.type 3;	.endef
	.def mb_cur_max;	.val 6;	.scl 8;	.type 3;	.endef
	.def mb_cur_min;	.val 8;	.scl 8;	.type 3;	.endef
	.def lc_dsp_width;	.val 10;	.scl 8;	.type 3;	.endef
	.def lc_locale_name;	.val 12; 	.scl 8;	.type 18;	.size 4;	.endef
	.def len_caseconv;	.val 16;	.scl 8;	.type 4;	.endef
	.def lc_caseconv;	.val 20; 	.scl 8;	.type 29;	.size 4;	.endef
	.def len_ctype;	.val 24;	.scl 8;	.type 4;	.endef
	.def lc_ctype;	.val 28; 	.scl 8;	.type 29;	.size 4;	.endef
	.def .eos;	.val 32;	.scl 102;	.tag char_classification_table;	.size 32;	.endef
	.def ctype_t;	.scl 13;	.type 8;	.tag char_classification_table;	.size 32;	.size 32;	.endef
	.def lc_monetary_table;	.scl 10;	.type 8;	.size 44;	.endef
	.def lc_version;	.val 0;	.scl 8;	.type 3;	.endef
	.def lc_length;	.val 2;	.scl 8;	.type 3;	.endef
	.def lc_locale_name;	.val 4; 	.scl 8;	.type 18;	.size 4;	.endef
	.def int_curr_symbol;	.val 8; 	.scl 8;	.type 18;	.size 4;	.endef
	.def currency_symbol;	.val 12; 	.scl 8;	.type 18;	.size 4;	.endef
	.def mon_decimal_point;	.val 16; 	.scl 8;	.type 18;	.size 4;	.endef
	.def mon_thousands_sep;	.val 20; 	.scl 8;	.type 18;	.size 4;	.endef
	.def mon_grouping;	.val 24; 	.scl 8;	.type 18;	.size 4;	.endef
	.def positive_sign;	.val 28; 	.scl 8;	.type 18;	.size 4;	.endef
	.def negative_sign;	.val 32; 	.scl 8;	.type 18;	.size 4;	.endef
	.def int_frac_digits;	.val 36;	.scl 8;	.type 2;	.endef
	.def frac_digits;	.val 37;	.scl 8;	.type 2;	.endef
	.def p_cs_precedes;	.val 38;	.scl 8;	.type 2;	.endef
	.def p_sep_by_space;	.val 39;	.scl 8;	.type 2;	.endef
	.def n_cs_precedes;	.val 40;	.scl 8;	.type 2;	.endef
	.def n_sep_by_space;	.val 41;	.scl 8;	.type 2;	.endef
	.def p_sign_posn;	.val 42;	.scl 8;	.type 2;	.endef
	.def n_sign_posn;	.val 43;	.scl 8;	.type 2;	.endef
	.def .eos;	.val 44;	.scl 102;	.tag lc_monetary_table;	.size 44;	.endef
	.def mon_t;	.scl 13;	.type 8;	.tag lc_monetary_table;	.size 44;	.size 44;	.endef
	.def numeric_table;	.scl 10;	.type 8;	.size 20;	.endef
	.def lc_version;	.val 0;	.scl 8;	.type 3;	.endef
	.def lc_length;	.val 2;	.scl 8;	.type 3;	.endef
	.def lc_locale_name;	.val 4; 	.scl 8;	.type 18;	.size 4;	.endef
	.def decimal_point;	.val 8; 	.scl 8;	.type 18;	.size 4;	.endef
	.def thousands_sep;	.val 12; 	.scl 8;	.type 18;	.size 4;	.endef
	.def grouping;	.val 16; 	.scl 8;	.type 18;	.size 4;	.endef
	.def .eos;	.val 20;	.scl 102;	.tag numeric_table;	.size 20;	.endef
	.def num_t;	.scl 13;	.type 8;	.tag numeric_table;	.size 20;	.size 20;	.endef
	.def lc_messages_table;	.scl 10;	.type 8;	.size 20;	.endef
	.def lc_version;	.val 0;	.scl 8;	.type 3;	.endef
	.def lc_length;	.val 2;	.scl 8;	.type 3;	.endef
	.def lc_locale_name;	.val 4; 	.scl 8;	.type 18;	.size 4;	.endef
	.def messages;	.val 8; 	.scl 8;	.type 18;	.size 4;	.endef
	.def yes_string;	.val 12; 	.scl 8;	.type 18;	.size 4;	.endef
	.def no_string;	.val 16; 	.scl 8;	.type 18;	.size 4;	.endef
	.def .eos;	.val 20;	.scl 102;	.tag lc_messages_table;	.size 20;	.endef
	.def msg_t;	.scl 13;	.type 8;	.tag lc_messages_table;	.size 20;	.size 20;	.endef
	.def lc_time_table;	.scl 10;	.type 8;	.size 60;	.endef
	.def lc_version;	.val 0;	.scl 8;	.type 3;	.endef
	.def lc_length;	.val 2;	.scl 8;	.type 3;	.endef
	.def lc_locale_name;	.val 4; 	.scl 8;	.type 18;	.size 4;	.endef
	.def t_fmt;	.val 8; 	.scl 8;	.type 18;	.size 4;	.endef
	.def d_fmt;	.val 12; 	.scl 8;	.type 18;	.size 4;	.endef
	.def nlldate;	.val 16; 	.scl 8;	.type 18;	.size 4;	.endef
	.def d_t_fmt;	.val 20; 	.scl 8;	.type 18;	.size 4;	.endef
	.def abday;	.val 24; 	.scl 8;	.type 18;	.size 4;	.endef
	.def day;	.val 28; 	.scl 8;	.type 18;	.size 4;	.endef
	.def abmon;	.val 32; 	.scl 8;	.type 18;	.size 4;	.endef
	.def mon;	.val 36; 	.scl 8;	.type 18;	.size 4;	.endef
	.def misc;	.val 40; 	.scl 8;	.type 18;	.size 4;	.endef
	.def tstrs;	.val 44; 	.scl 8;	.type 18;	.size 4;	.endef
	.def tunits;	.val 48; 	.scl 8;	.type 18;	.size 4;	.endef
	.def year;	.val 52; 	.scl 8;	.type 18;	.size 4;	.endef
	.def am_pm;	.val 56; 	.scl 8;	.type 18;	.size 4;	.endef
	.def .eos;	.val 60;	.scl 102;	.tag lc_time_table;	.size 60;	.endef
	.def tim_t;	.scl 13;	.type 8;	.tag lc_time_table;	.size 60;	.size 60;	.endef
	.def wchar_mapping_table;	.scl 10;	.type 8;	.size 8;	.endef
	.def lc_version;	.val 0;	.scl 8;	.type 3;	.endef
	.def lc_length;	.val 2;	.scl 8;	.type 3;	.endef
	.def lc_identifier;	.val 4; 	.scl 8;	.type 18;	.size 4;	.endef
	.def .eos;	.val 8;	.scl 102;	.tag wchar_mapping_table;	.size 8;	.endef
	.def map_t;	.scl 13;	.type 8;	.tag wchar_mapping_table;	.size 8;	.size 8;	.endef
	.def localeinfo_table;	.scl 10;	.type 8;	.size 36;	.endef
	.def lc_mag0;	.val 0;	.scl 8;	.type 2;	.endef
	.def lc_mag1;	.val 1;	.scl 8;	.type 2;	.endef
	.def lc_version;	.val 2;	.scl 8;	.type 3;	.endef
	.def lc_code_type;	.val 4;	.scl 8;	.type 3;	.endef
	.def lc_length;	.val 6;	.scl 8;	.type 3;	.endef
	.def lc_coltbl;	.val 8; 	.scl 8;	.type 24;	.tag collation_table;	.size 44;	.size 4;	.endef
	.def lc_chrtbl;	.val 12; 	.scl 8;	.type 24;	.tag char_classification_table;	.size 32;	.size 4;	.endef
	.def lc_montbl;	.val 16; 	.scl 8;	.type 24;	.tag lc_monetary_table;	.size 44;	.size 4;	.endef
	.def lc_numtbl;	.val 20; 	.scl 8;	.type 24;	.tag numeric_table;	.size 20;	.size 4;	.endef
	.def lc_timtbl;	.val 24; 	.scl 8;	.type 24;	.tag lc_time_table;	.size 60;	.size 4;	.endef
	.def lc_msgtbl;	.val 28; 	.scl 8;	.type 24;	.tag lc_messages_table;	.size 20;	.size 4;	.endef
	.def lc_maptbl;	.val 32; 	.scl 8;	.type 24;	.tag wchar_mapping_table;	.size 8;	.size 4;	.endef
	.def .eos;	.val 36;	.scl 102;	.tag localeinfo_table;	.size 36;	.endef
	.def loc_t;	.scl 13;	.type 8;	.tag localeinfo_table;	.size 36;	.size 36;	.endef
	.def NLchar;	.scl 13;	.type 13;	.size 2;	.endef
	.def NLescdata;	.scl 10;	.type 8;	.size 8;	.endef
	.def key;	.val 0; 	.scl 8;	.type 28;	.size 4;	.endef
	.def value;	.val 4;	.scl 8;	.type 13;	.endef
	.def .eos;	.val 8;	.scl 102;	.tag NLescdata;	.size 8;	.endef
	.def partID;	.scl 10;	.type 8;	.size 4;	.endef
	.def OS_Major;	.val 0;	.scl 18;	.type 14;	.size 8;	.endef
	.def IO_Bus;	.val 8;	.scl 18;	.type 14;	.size 3;	.endef
	.def SCSI_Controller;	.val 11;	.scl 18;	.type 14;	.size 4;	.endef
	.def SCSI_Bus;	.val 15;	.scl 18;	.type 14;	.size 3;	.endef
	.def PUN;	.val 18;	.scl 18;	.type 14;	.size 4;	.endef
	.def LUN;	.val 22;	.scl 18;	.type 14;	.size 3;	.endef
	.def reserved;	.val 25;	.scl 18;	.type 14;	.size 3;	.endef
	.def Partition;	.val 28;	.scl 18;	.type 14;	.size 4;	.endef
	.def .eos;	.val 4;	.scl 102;	.tag partID;	.size 4;	.endef
	.def Partition_Identifier_t;	.scl 13;	.type 8;	.tag partID;	.size 4;	.size 4;	.endef
	.def dpcb;	.scl 10;	.type 8;	.size 256;	.endef
	.def Partition_Start;	.val 0;	.scl 8;	.type 4;	.endef
	.def Partition_Size;	.val 4;	.scl 8;	.type 4;	.endef
	.def Who_Am_I;	.val 8;	.scl 8;	.type 8;	.tag partID;	.size 4;	.endef
	.def Number_Of_Stripes;	.val 12;	.scl 8;	.type 4;	.endef
	.def Stripe_Extent_DPCB;	.val 16;	.scl 8;	.type 4;	.endef
	.def Number_Of_Mirrors;	.val 20;	.scl 8;	.type 4;	.endef
	.def Mirror_Extent_DPCB;	.val 24;	.scl 8;	.type 4;	.endef
	.def Stripe_Size;	.val 28;	.scl 8;	.type 4;	.endef
	.def Stripe;	.val 32; 	.scl 8;	.type 56;	.tag partID;	.size 4;	.dim 32;	.size 128;	.endef
	.def Mirror;	.val 160; 	.scl 8;	.type 56;	.tag partID;	.size 4;	.dim 16;	.size 64;	.endef
	.def filler;	.val 224; 	.scl 8;	.type 52;	.dim 8;	.size 32;	.endef
	.def .eos;	.val 256;	.scl 102;	.tag dpcb;	.size 256;	.endef
	.def Disk_Partition_Control_Block_t;	.scl 13;	.type 8;	.tag dpcb;	.size 256;	.size 256;	.endef
	.def dcb;	.scl 10;	.type 8;	.size 8192;	.endef
	.def Partition_Control_Blocks;	.val 0; 	.scl 8;	.type 56;	.tag dpcb;	.size 256;	.dim 16;	.size 4096;	.endef
	.def Extension_Control_Blocks;	.val 4096; 	.scl 8;	.type 56;	.tag dpcb;	.size 256;	.dim 16;	.size 4096;	.endef
	.def .eos;	.val 8192;	.scl 102;	.tag dcb;	.size 8192;	.endef
	.def Disk_Control_Block_t;	.scl 13;	.type 8;	.tag dcb;	.size 8192;	.size 8192;	.endef
	.def DiskGeometry;	.scl 10;	.type 8;	.size 24;	.endef
	.def Capacity;	.val 0;	.scl 8;	.type 4;	.endef
	.def NumberOfCylinders;	.val 4;	.scl 8;	.type 4;	.endef
	.def NumberOfHeads;	.val 8;	.scl 8;	.type 4;	.endef
	.def SectorsPerTrack;	.val 12;	.scl 8;	.type 4;	.endef
	.def BytesPerSector;	.val 16;	.scl 8;	.type 4;	.endef
	.def SectorsPerCylinder;	.val 20;	.scl 8;	.type 4;	.endef
	.def .eos;	.val 24;	.scl 102;	.tag DiskGeometry;	.size 24;	.endef
	.def DiskGeometry_t;	.scl 13;	.type 8;	.tag DiskGeometry;	.size 24;	.size 24;	.endef
	.def minor_t;	.scl 13;	.type 4;	.size 4;	.endef
	.def SCSI_Command_Block;	.scl 10;	.type 8;	.size 12;	.endef
	.def cmd;	.val 0; 	.scl 8;	.type 60;	.dim 12;	.size 12;	.endef
	.def .eos;	.val 12;	.scl 102;	.tag SCSI_Command_Block;	.size 12;	.endef
	.def SCSI_Command_Block_t;	.scl 13;	.type 8;	.tag SCSI_Command_Block;	.size 12;	.size 12;	.endef
	.def IO_DeviceParameters;	.scl 10;	.type 8;	.size 12;	.endef
	.def dp_IO_BusNumber;	.val 0;	.scl 8;	.type 14;	.endef
	.def dp_BaseAddress;	.val 4;	.scl 8;	.type 4;	.endef
	.def dp_IO_BusSlot;	.val 8;	.scl 8;	.type 12;	.endef
	.def dp_SCSI_BusNumber;	.val 9;	.scl 8;	.type 12;	.endef
	.def dp_PUN;	.val 10;	.scl 8;	.type 12;	.endef
	.def dp_LUN;	.val 11;	.scl 8;	.type 12;	.endef
	.def .eos;	.val 12;	.scl 102;	.tag IO_DeviceParameters;	.size 12;	.endef
	.def SCSI_Inquiry_Data;	.scl 10;	.type 8;	.size 36;	.endef
	.def Periph_Device_Type;	.val 0;	.scl 18;	.type 14;	.size 5;	.endef
	.def Periph_Qualifier;	.val 5;	.scl 18;	.type 14;	.size 3;	.endef
	.def Device_Type_Qualifier;	.val 8;	.scl 18;	.type 14;	.size 7;	.endef
	.def Removable_Media;	.val 15;	.scl 18;	.type 14;	.size 1;	.endef
	.def ANSI_Version;	.val 16;	.scl 18;	.type 14;	.size 3;	.endef
	.def ECMA_Version;	.val 19;	.scl 18;	.type 14;	.size 3;	.endef
	.def ISO_Version;	.val 22;	.scl 18;	.type 14;	.size 2;	.endef
	.def Response_Data_Format;	.val 24;	.scl 18;	.type 14;	.size 4;	.endef
	.def reserved_0;	.val 28;	.scl 18;	.type 14;	.size 3;	.endef
	.def AENC;	.val 31;	.scl 18;	.type 14;	.size 1;	.endef
	.def Additional_Length;	.val 32;	.scl 18;	.type 14;	.size 8;	.endef
	.def reserved_1;	.val 40;	.scl 18;	.type 14;	.size 16;	.endef
	.def SftReset;	.val 56;	.scl 18;	.type 14;	.size 1;	.endef
	.def CmdQue;	.val 57;	.scl 18;	.type 14;	.size 1;	.endef
	.def reserved_2;	.val 58;	.scl 18;	.type 14;	.size 1;	.endef
	.def Linked;	.val 59;	.scl 18;	.type 14;	.size 1;	.endef
	.def Sync;	.val 60;	.scl 18;	.type 14;	.size 1;	.endef
	.def WBus16;	.val 61;	.scl 18;	.type 14;	.size 1;	.endef
	.def WBus32;	.val 62;	.scl 18;	.type 14;	.size 1;	.endef
	.def RelAdr;	.val 63;	.scl 18;	.type 14;	.size 1;	.endef
	.def Vendor_ID;	.val 8; 	.scl 8;	.type 50;	.dim 8;	.size 8;	.endef
	.def Product_ID;	.val 16; 	.scl 8;	.type 50;	.dim 16;	.size 16;	.endef
	.def Revision_Level;	.val 32; 	.scl 8;	.type 50;	.dim 4;	.size 4;	.endef
	.def .eos;	.val 36;	.scl 102;	.tag SCSI_Inquiry_Data;	.size 36;	.endef
	.def SCSI_Inquiry_Data_t;	.scl 13;	.type 8;	.tag SCSI_Inquiry_Data;	.size 36;	.size 36;	.endef
	.def Read_Capacity;	.scl 10;	.type 8;	.size 8;	.endef
	.def blkno;	.val 0;	.scl 8;	.type 4;	.endef
	.def capacity;	.val 4;	.scl 8;	.type 4;	.endef
	.def .eos;	.val 8;	.scl 102;	.tag Read_Capacity;	.size 8;	.endef
	.def Read_Capacity_Data_t;	.scl 13;	.type 8;	.tag Read_Capacity;	.size 8;	.size 8;	.endef
	.def Block_Limits;	.scl 10;	.type 8;	.size 6;	.endef
	.def reserved;	.val 0;	.scl 8;	.type 2;	.endef
	.def Maximum_Block_Len_HI;	.val 1;	.scl 8;	.type 2;	.endef
	.def Maximum_Block_Len_MI;	.val 2;	.scl 8;	.type 2;	.endef
	.def Maximum_Block_Len_LO;	.val 3;	.scl 8;	.type 2;	.endef
	.def Minimum_Block_Length;	.val 4;	.scl 8;	.type 3;	.endef
	.def .eos;	.val 6;	.scl 102;	.tag Block_Limits;	.size 6;	.endef
	.def Read_Block_Limits_Data_t;	.scl 13;	.type 8;	.tag Block_Limits;	.size 6;	.size 6;	.endef
	.def SCSI_Sense_Data;	.scl 10;	.type 8;	.size 252;	.endef
	.def Error_Code;	.val 0;	.scl 18;	.type 12;	.size 4;	.endef
	.def Error_Class;	.val 4;	.scl 18;	.type 12;	.size 3;	.endef
	.def Valid;	.val 7;	.scl 18;	.type 12;	.size 1;	.endef
	.def Segment_Number;	.val 1;	.scl 8;	.type 12;	.endef
	.def Sense_Key;	.val 16;	.scl 18;	.type 12;	.size 4;	.endef
	.def Reserved;	.val 20;	.scl 18;	.type 12;	.size 1;	.endef
	.def Incorrect_Length_Indicator;	.val 21;	.scl 18;	.type 12;	.size 1;	.endef
	.def End_Of_Media;	.val 22;	.scl 18;	.type 12;	.size 1;	.endef
	.def Filemark;	.val 23;	.scl 18;	.type 12;	.size 1;	.endef
	.def Information_MSB;	.val 3;	.scl 8;	.type 12;	.endef
	.def Information_Bytes2;	.val 4;	.scl 8;	.type 12;	.endef
	.def Information_Bytes1;	.val 5;	.scl 8;	.type 12;	.endef
	.def Information_Bytes0;	.val 6;	.scl 8;	.type 12;	.endef
	.def Additional_Length;	.val 7;	.scl 8;	.type 12;	.endef
	.def Command_Specific_Information;	.val 8;	.scl 8;	.type 4;	.endef
	.def Additional_Sense_Code;	.val 12;	.scl 8;	.type 12;	.endef
	.def Additional_Sense_Code_Qualifier;	.val 13;	.scl 8;	.type 12;	.endef
	.def Field_Replaceable_Unit_Code;	.val 14;	.scl 8;	.type 12;	.endef
	.def Illegal_Req_Bit_Pointer;	.val 120;	.scl 18;	.type 12;	.size 3;	.endef
	.def Illegal_Req_Bit_Valid;	.val 123;	.scl 18;	.type 12;	.size 1;	.endef
	.def Illegal_Req_Reserved;	.val 124;	.scl 18;	.type 12;	.size 2;	.endef
	.def Illegal_Req_Cmd_Data;	.val 126;	.scl 18;	.type 12;	.size 1;	.endef
	.def Sense_Key_Specific_Valid;	.val 127;	.scl 18;	.type 12;	.size 1;	.endef
	.def Sense_Key_Specific_Data;	.val 16;	.scl 8;	.type 13;	.endef
	.def Additional_Sense_Data;	.val 18; 	.scl 8;	.type 50;	.dim 232;	.size 232;	.endef
	.def .eos;	.val 252;	.scl 102;	.tag SCSI_Sense_Data;	.size 252;	.endef
	.def SCSI_Sense_Data_t;	.scl 13;	.type 8;	.tag SCSI_Sense_Data;	.size 252;	.size 252;	.endef
	.def WD_Registers;	.scl 10;	.type 8;	.size 16;	.endef
	.def Driver_Flags;	.val 0;	.scl 8;	.type 4;	.endef
	.def Driver_State0;	.val 4;	.scl 8;	.type 4;	.endef
	.def Driver_State1;	.val 8;	.scl 8;	.type 4;	.endef
	.def Driver_State2;	.val 12;	.scl 8;	.type 4;	.endef
	.def .eos;	.val 16;	.scl 102;	.tag WD_Registers;	.size 16;	.endef
	.def WD_Registers_t;	.scl 13;	.type 8;	.tag WD_Registers;	.size 16;	.size 16;	.endef
	.def FPIII_Registers;	.scl 10;	.type 8;	.size 16;	.endef
	.def Driver_Flags;	.val 0;	.scl 8;	.type 4;	.endef
	.def Driver_State0;	.val 4;	.scl 8;	.type 4;	.endef
	.def Driver_State1;	.val 8;	.scl 8;	.type 4;	.endef
	.def Driver_State2;	.val 12;	.scl 8;	.type 4;	.endef
	.def .eos;	.val 16;	.scl 102;	.tag FPIII_Registers;	.size 16;	.endef
	.def FPIII_Registers_t;	.scl 13;	.type 8;	.tag FPIII_Registers;	.size 16;	.size 16;	.endef
	.def Hardware_Specific;	.scl 12;	.type 9;	.size 16;	.endef
	.def WD_Registers;	.val 0;	.scl 8;	.type 8;	.tag WD_Registers;	.size 16;	.endef
	.def FPIII_Registers;	.val 0;	.scl 8;	.type 8;	.tag FPIII_Registers;	.size 16;	.endef
	.def .eos;	.val 16;	.scl 102;	.tag Hardware_Specific;	.size 16;	.endef
	.def SCSI_CCB;	.scl 10;	.type 8;	.size 36;	.endef
	.def SCSI_Command_Block;	.val 0;	.scl 8;	.type 8;	.tag SCSI_Command_Block;	.size 12;	.endef
	.def SCSI_status;	.val 12;	.scl 8;	.type 2;	.endef
	.def SCSI_message;	.val 13;	.scl 8;	.type 2;	.endef
	.def Driver_Error;	.val 16;	.scl 8;	.type 4;	.endef
	.def Hardware_Specific;	.val 20;	.scl 8;	.type 9;	.tag Hardware_Specific;	.size 16;	.endef
	.def .eos;	.val 36;	.scl 102;	.tag SCSI_CCB;	.size 36;	.endef
	.def E_SCSI_Command_Block_t;	.scl 13;	.type 8;	.tag SCSI_CCB;	.size 36;	.size 36;	.endef
	.def ModeParameterHeader6;	.scl 10;	.type 8;	.size 4;	.endef
	.def ModeDataLength;	.val 0;	.scl 8;	.type 12;	.endef
	.def MediumType;	.val 1;	.scl 8;	.type 12;	.endef
	.def DeviceSpecificParameter;	.val 2;	.scl 8;	.type 12;	.endef
	.def BlockDescriptorLength;	.val 3;	.scl 8;	.type 12;	.endef
	.def .eos;	.val 4;	.scl 102;	.tag ModeParameterHeader6;	.size 4;	.endef
	.def ModeParameterHeader6_t;	.scl 13;	.type 8;	.tag ModeParameterHeader6;	.size 4;	.size 4;	.endef
	.def ModeBlockDescriptor;	.scl 10;	.type 8;	.size 8;	.endef
	.def NumberOfBlocks;	.val 0;	.scl 18;	.type 14;	.size 24;	.endef
	.def DensityCode;	.val 24;	.scl 18;	.type 14;	.size 8;	.endef
	.def BlockLength;	.val 32;	.scl 18;	.type 14;	.size 24;	.endef
	.def reserved;	.val 56;	.scl 18;	.type 14;	.size 8;	.endef
	.def .eos;	.val 8;	.scl 102;	.tag ModeBlockDescriptor;	.size 8;	.endef
	.def ModeBlockDescriptor_t;	.scl 13;	.type 8;	.tag ModeBlockDescriptor;	.size 8;	.size 8;	.endef
	.def SCSI_ModeSelectData;	.scl 10;	.type 8;	.size 12;	.endef
	.def Header;	.val 0;	.scl 8;	.type 8;	.tag ModeParameterHeader6;	.size 4;	.endef
	.def BlockDescriptor;	.val 4;	.scl 8;	.type 8;	.tag ModeBlockDescriptor;	.size 8;	.endef
	.def .eos;	.val 12;	.scl 102;	.tag SCSI_ModeSelectData;	.size 12;	.endef
	.def ModeSelectData_t;	.scl 13;	.type 8;	.tag SCSI_ModeSelectData;	.size 12;	.size 12;	.endef
	.def PageHeader;	.scl 10;	.type 8;	.size 2;	.endef
	.def PageCode;	.val 0;	.scl 8;	.type 12;	.endef
	.def PageLength;	.val 1;	.scl 8;	.type 12;	.endef
	.def .eos;	.val 2;	.scl 102;	.tag PageHeader;	.size 2;	.endef
	.def PageHeader_t;	.scl 13;	.type 8;	.tag PageHeader;	.size 2;	.size 2;	.endef
	.def Mode_Select;	.scl 10;	.type 8;	.size 4;	.endef
	.def dummy;	.val 0;	.scl 8;	.type 4;	.endef
	.def .eos;	.val 4;	.scl 102;	.tag Mode_Select;	.size 4;	.endef
	.def Mode_Select_t;	.scl 13;	.type 8;	.tag Mode_Select;	.size 4;	.size 4;	.endef
	.def .533fak;	.scl 10;	.type 8;	.size 36;	.endef
	.def Header;	.val 0;	.scl 11;	.type 8;	.tag ModeParameterHeader6;	.size 4;	.endef
	.def BlockDescriptor;	.val 4;	.scl 11;	.type 8;	.tag ModeBlockDescriptor;	.size 8;	.endef
	.def PageHeader;	.val 12;	.scl 11;	.type 8;	.tag PageHeader;	.size 2;	.endef
	.def TracksPerZone;	.val 14; 	.scl 8;	.type 60;	.dim 2;	.size 2;	.endef
	.def AlternateSectorsPerZone;	.val 16; 	.scl 8;	.type 60;	.dim 2;	.size 2;	.endef
	.def AlternateTracksPerZone;	.val 18; 	.scl 8;	.type 60;	.dim 2;	.size 2;	.endef
	.def AlternateTracksPerLogicalUnit;	.val 20; 	.scl 8;	.type 60;	.dim 2;	.size 2;	.endef
	.def SectorsPerTrack;	.val 22; 	.scl 8;	.type 60;	.dim 2;	.size 2;	.endef
	.def DataBytesPerPhysicalSector;	.val 24; 	.scl 8;	.type 60;	.dim 2;	.size 2;	.endef
	.def Interleave;	.val 26; 	.scl 8;	.type 60;	.dim 2;	.size 2;	.endef
	.def TrackSkewFactor;	.val 28; 	.scl 8;	.type 60;	.dim 2;	.size 2;	.endef
	.def CylinderSkewFactor;	.val 30; 	.scl 8;	.type 60;	.dim 2;	.size 2;	.endef
	.def reserved0;	.val 256;	.scl 18;	.type 12;	.size 4;	.endef
	.def SURF;	.val 260;	.scl 18;	.type 12;	.size 1;	.endef
	.def RMB;	.val 261;	.scl 18;	.type 12;	.size 1;	.endef
	.def HSEC;	.val 262;	.scl 18;	.type 12;	.size 1;	.endef
	.def SSEC;	.val 263;	.scl 18;	.type 12;	.size 1;	.endef
	.def reserved1;	.val 33; 	.scl 8;	.type 60;	.dim 2;	.size 2;	.endef
	.def .eos;	.val 36;	.scl 102;	.tag .533fak;	.size 36;	.endef
	.def FormatDevicePage_t;	.scl 13;	.type 8;	.tag .533fak;	.size 36;	.size 36;	.endef
	.def .547fak;	.scl 10;	.type 8;	.size 36;	.endef
	.def Header;	.val 0;	.scl 11;	.type 8;	.tag ModeParameterHeader6;	.size 4;	.endef
	.def BlockDescriptor;	.val 4;	.scl 11;	.type 8;	.tag ModeBlockDescriptor;	.size 8;	.endef
	.def PageHeader;	.val 12;	.scl 11;	.type 8;	.tag PageHeader;	.size 2;	.endef
	.def NumberOfCylinders;	.val 14; 	.scl 8;	.type 60;	.dim 3;	.size 3;	.endef
	.def NumberOfHeads;	.val 17;	.scl 8;	.type 12;	.endef
	.def WritePrecompCylinder;	.val 18; 	.scl 8;	.type 60;	.dim 3;	.size 3;	.endef
	.def ReducedWriteCurrentCylinder;	.val 21; 	.scl 8;	.type 60;	.dim 3;	.size 3;	.endef
	.def DriveStepRate;	.val 24; 	.scl 8;	.type 60;	.dim 2;	.size 2;	.endef
	.def LandingZoneCylinder;	.val 26; 	.scl 8;	.type 60;	.dim 3;	.size 3;	.endef
	.def Resv_RPL;	.val 29;	.scl 8;	.type 12;	.endef
	.def RotationalOffset;	.val 30;	.scl 8;	.type 12;	.endef
	.def Reserved0;	.val 31;	.scl 8;	.type 12;	.endef
	.def MediumRotationRate;	.val 32; 	.scl 8;	.type 60;	.dim 2;	.size 2;	.endef
	.def Reserved;	.val 34; 	.scl 8;	.type 60;	.dim 2;	.size 2;	.endef
	.def .eos;	.val 36;	.scl 102;	.tag .547fak;	.size 36;	.endef
	.def RigidDiskGeometry_t;	.scl 13;	.type 8;	.tag .547fak;	.size 36;	.size 36;	.endef
	.def .573fak;	.scl 10;	.type 8;	.size 44;	.endef
	.def Header;	.val 0;	.scl 11;	.type 8;	.tag ModeParameterHeader6;	.size 4;	.endef
	.def BlockDescriptor;	.val 4;	.scl 11;	.type 8;	.tag ModeBlockDescriptor;	.size 8;	.endef
	.def PageHeader;	.val 12;	.scl 11;	.type 8;	.tag PageHeader;	.size 2;	.endef
	.def TransferRate;	.val 14;	.scl 8;	.type 13;	.endef
	.def NumberofHeads;	.val 16;	.scl 8;	.type 12;	.endef
	.def SectorsPerTrack;	.val 17;	.scl 8;	.type 12;	.endef
	.def BytesPerSector;	.val 18;	.scl 8;	.type 13;	.endef
	.def NumberOfCylinders;	.val 20;	.scl 8;	.type 13;	.endef
	.def WritePrecompCylinder;	.val 22;	.scl 8;	.type 13;	.endef
	.def ReducedWriteCurrentCylinder;	.val 24;	.scl 8;	.type 13;	.endef
	.def DriveStepRate;	.val 26;	.scl 8;	.type 13;	.endef
	.def DriveStepPulseWidth;	.val 28;	.scl 8;	.type 12;	.endef
	.def HeadSettleDelay;	.val 29; 	.scl 8;	.type 60;	.dim 2;	.size 2;	.endef
	.def MotorOnDelay;	.val 31;	.scl 8;	.type 12;	.endef
	.def MotorOffDelay;	.val 32;	.scl 8;	.type 12;	.endef
	.def TRDY_SSN_MO;	.val 33;	.scl 8;	.type 12;	.endef
	.def StepPulsesPerCylinder;	.val 34;	.scl 8;	.type 12;	.endef
	.def WritePrecompensationValue;	.val 35;	.scl 8;	.type 12;	.endef
	.def HeadLoadDelay;	.val 36;	.scl 8;	.type 12;	.endef
	.def HeadUnloadDelay;	.val 37;	.scl 8;	.type 12;	.endef
	.def Pin34Pin2;	.val 38;	.scl 8;	.type 12;	.endef
	.def Pin4Pin1;	.val 39;	.scl 8;	.type 12;	.endef
	.def MediumRotationRate;	.val 40;	.scl 8;	.type 13;	.endef
	.def Reserved1;	.val 42;	.scl 8;	.type 13;	.endef
	.def .eos;	.val 44;	.scl 102;	.tag .573fak;	.size 44;	.endef
	.def FlexibleDisk_t;	.scl 13;	.type 8;	.tag .573fak;	.size 44;	.size 44;	.endef
	.def buf;	.scl 10;	.type 8;	.size 0;	.endef
	.def .eos;	.val 0;	.scl 102;	.tag buf;	.size 0;	.endef
	.def iostat;	.scl 10;	.type 8;	.size 16;	.endef
	.def io_ops;	.val 0;	.scl 8;	.type 4;	.endef
	.def io_misc;	.val 4;	.scl 8;	.type 4;	.endef
	.def io_qcnt;	.val 8;	.scl 8;	.type 4;	.endef
	.def io_unlog;	.val 12;	.scl 8;	.type 13;	.endef
	.def .eos;	.val 16;	.scl 102;	.tag iostat;	.size 16;	.endef
	.def iotime;	.scl 10;	.type 8;	.size 32;	.endef
	.def ios;	.val 0;	.scl 8;	.type 8;	.tag iostat;	.size 16;	.endef
	.def io_bcnt;	.val 16;	.scl 8;	.type 4;	.endef
	.def io_resp;	.val 20;	.scl 8;	.type 4;	.endef
	.def io_act;	.val 24;	.scl 8;	.type 4;	.endef
	.def io_pad;	.val 28;	.scl 8;	.type 4;	.endef
	.def .eos;	.val 32;	.scl 102;	.tag iotime;	.size 32;	.endef
	.def SCSI_Tallies;	.scl 10;	.type 8;	.size 164;	.endef
	.def DeviceId;	.val 0;	.scl 8;	.type 4;	.endef
	.def TallyData;	.val 4; 	.scl 8;	.type 62;	.dim 39;	.size 156;	.endef
	.def Next_Tally;	.val 160; 	.scl 8;	.type 24;	.tag SCSI_Tallies;	.size 164;	.size 4;	.endef
	.def .eos;	.val 164;	.scl 102;	.tag SCSI_Tallies;	.size 164;	.endef
	.def StatStructure;	.scl 10;	.type 8;	.size 204;	.endef
	.def ForwardPointer;	.val 0; 	.scl 8;	.type 24;	.size 4;	.endef
	.def BackPointer;	.val 4; 	.scl 8;	.type 24;	.size 4;	.endef
	.def SCSI_Stats;	.val 8;	.scl 8;	.type 8;	.tag iotime;	.size 32;	.endef
	.def SCSI_Tallies;	.val 40;	.scl 8;	.type 8;	.tag SCSI_Tallies;	.size 164;	.endef
	.def .eos;	.val 204;	.scl 102;	.tag StatStructure;	.size 204;	.endef
	.def P_OpenInfo;	.scl 10;	.type 8;	.size 8;	.endef
	.def Open_Type;	.val 0;	.scl 8;	.type 4;	.endef
	.def Layered_Open;	.val 4;	.scl 8;	.type 4;	.endef
	.def .eos;	.val 8;	.scl 102;	.tag P_OpenInfo;	.size 8;	.endef
	.def Current_Op;	.scl 10;	.type 8;	.size 12;	.endef
	.def status;	.val 0;	.scl 8;	.type 2;	.endef
	.def head;	.val 1;	.scl 8;	.type 2;	.endef
	.def entry;	.val 4; 	.scl 8;	.type 120;	.dim 2;	.size 8;	.endef
	.def .eos;	.val 12;	.scl 102;	.tag Current_Op;	.size 12;	.endef
	.def WD_Driver;	.scl 10;	.type 8;	.size 16;	.endef
	.def operation_level;	.val 0;	.scl 8;	.type 2;	.endef
	.def current_op;	.val 4;	.scl 11;	.type 8;	.tag Current_Op;	.size 12;	.endef
	.def .eos;	.val 16;	.scl 102;	.tag WD_Driver;	.size 16;	.endef
	.def SIOP_Driver;	.scl 10;	.type 8;	.size 16;	.endef
	.def operation_level;	.val 0;	.scl 8;	.type 2;	.endef
	.def status;	.val 1;	.scl 8;	.type 2;	.endef
	.def current_op;	.val 4; 	.scl 8;	.type 24;	.size 4;	.endef
	.def SCSI_SCRIPT;	.val 8; 	.scl 8;	.type 30;	.size 4;	.endef
	.def SENSE_SCRIPT;	.val 12; 	.scl 8;	.type 30;	.size 4;	.endef
	.def .eos;	.val 16;	.scl 102;	.tag SIOP_Driver;	.size 16;	.endef
	.def HA_Driver;	.scl 10;	.type 8;	.size 4;	.endef
	.def x;	.val 0;	.scl 8;	.type 4;	.endef
	.def .eos;	.val 4;	.scl 102;	.tag HA_Driver;	.size 4;	.endef
	.def FP_Driver;	.scl 10;	.type 8;	.size 4;	.endef
	.def x;	.val 0;	.scl 8;	.type 4;	.endef
	.def .eos;	.val 4;	.scl 102;	.tag FP_Driver;	.size 4;	.endef
	.def SIMPL_Driver;	.scl 10;	.type 8;	.size 28;	.endef
	.def current_op;	.val 0; 	.scl 8;	.type 24;	.size 4;	.endef
	.def untagged_op;	.val 4; 	.scl 8;	.type 24;	.size 4;	.endef
	.def auto_sense_op;	.val 8; 	.scl 8;	.type 24;	.size 4;	.endef
	.def target_info;	.val 12; 	.scl 8;	.type 16;	.size 4;	.endef
	.def busctrl;	.val 16; 	.scl 8;	.type 16;	.size 4;	.endef
	.def msg_bfr;	.val 20; 	.scl 8;	.type 60;	.dim 8;	.size 8;	.endef
	.def .eos;	.val 28;	.scl 102;	.tag SIMPL_Driver;	.size 28;	.endef
	.def Driver_Specific_Info;	.scl 12;	.type 9;	.size 28;	.endef
	.def WD_Driver;	.val 0;	.scl 8;	.type 8;	.tag WD_Driver;	.size 16;	.endef
	.def SIOP_Driver;	.val 0;	.scl 8;	.type 8;	.tag SIOP_Driver;	.size 16;	.endef
	.def HA_Driver;	.val 0;	.scl 8;	.type 8;	.tag HA_Driver;	.size 4;	.endef
	.def FP_Driver;	.val 0;	.scl 8;	.type 8;	.tag FP_Driver;	.size 4;	.endef
	.def SIMPL_Driver;	.val 0;	.scl 8;	.type 8;	.tag SIMPL_Driver;	.size 28;	.endef
	.def .eos;	.val 28;	.scl 102;	.tag Driver_Specific_Info;	.size 28;	.endef
	.def Hard_Disk;	.scl 10;	.type 8;	.size 32;	.endef
	.def Disk_Control_Block_Ptr;	.val 0; 	.scl 8;	.type 24;	.tag dcb;	.size 8192;	.size 4;	.endef
	.def Geometry;	.val 4;	.scl 11;	.type 8;	.tag DiskGeometry;	.size 24;	.endef
	.def current_mode;	.val 28;	.scl 8;	.type 4;	.endef
	.def .eos;	.val 32;	.scl 102;	.tag Hard_Disk;	.size 32;	.endef
	.def Cache_Tape;	.scl 10;	.type 8;	.size 28;	.endef
	.def Cache_Size;	.val 0;	.scl 8;	.type 4;	.endef
	.def Cache_Error;	.val 4;	.scl 8;	.type 4;	.endef
	.def Cache_Start;	.val 8;	.scl 8;	.type 4;	.endef
	.def Cache_End;	.val 12;	.scl 8;	.type 4;	.endef
	.def Cache_Sense;	.val 16; 	.scl 8;	.type 24;	.tag SCSI_Sense_Data;	.size 252;	.size 4;	.endef
	.def Filemark_Found;	.val 160;	.scl 18;	.type 14;	.size 1;	.endef
	.def End_Of_Media;	.val 161;	.scl 18;	.type 14;	.size 1;	.endef
	.def Cache_Write;	.val 162;	.scl 18;	.type 14;	.size 1;	.endef
	.def pad;	.val 163;	.scl 18;	.type 14;	.size 29;	.endef
	.def Cache_Buffer;	.val 24; 	.scl 8;	.type 18;	.size 4;	.endef
	.def .eos;	.val 28;	.scl 102;	.tag Cache_Tape;	.size 28;	.endef
	.def Tape_Specific;	.scl 10;	.type 8;	.size 44;	.endef
	.def Tape_Position;	.val 0;	.scl 8;	.type 4;	.endef
	.def Maximum_Tape_Size;	.val 4;	.scl 8;	.type 4;	.endef
	.def Maximum_Block_Length;	.val 8;	.scl 8;	.type 4;	.endef
	.def Minimum_Block_Length;	.val 12;	.scl 8;	.type 4;	.endef
	.def Cache;	.val 16;	.scl 11;	.type 8;	.tag Cache_Tape;	.size 28;	.endef
	.def .eos;	.val 44;	.scl 102;	.tag Tape_Specific;	.size 44;	.endef
	.def Additional_Device;	.scl 10;	.type 8;	.size 40;	.endef
	.def your_variables;	.val 0; 	.scl 8;	.type 52;	.dim 10;	.size 40;	.endef
	.def .eos;	.val 40;	.scl 102;	.tag Additional_Device;	.size 40;	.endef
	.def Device_Specific_Info;	.scl 12;	.type 9;	.size 44;	.endef
	.def Hard_Disk;	.val 0;	.scl 8;	.type 8;	.tag Hard_Disk;	.size 32;	.endef
	.def Tape;	.val 0;	.scl 8;	.type 8;	.tag Tape_Specific;	.size 44;	.endef
	.def Additional_Device;	.val 0;	.scl 8;	.type 8;	.tag Additional_Device;	.size 40;	.endef
	.def .eos;	.val 44;	.scl 102;	.tag Device_Specific_Info;	.size 44;	.endef
	.def SCSI_Memory;	.scl 10;	.type 8;	.size 8;	.endef
	.def Size;	.val 0;	.scl 8;	.type 4;	.endef
	.def Address;	.val 4; 	.scl 8;	.type 18;	.size 4;	.endef
	.def .eos;	.val 8;	.scl 102;	.tag SCSI_Memory;	.size 8;	.endef
	.def SCSI_Device;	.scl 10;	.type 8;	.size 1096;	.endef
	.def Device_forw;	.val 0; 	.scl 8;	.type 24;	.tag SCSI_Device;	.size 1096;	.size 4;	.endef
	.def Device_back;	.val 4; 	.scl 8;	.type 24;	.tag SCSI_Device;	.size 1096;	.size 4;	.endef
	.def s_forw;	.val 8; 	.scl 8;	.type 24;	.size 4;	.endef
	.def s_back;	.val 12; 	.scl 8;	.type 24;	.size 4;	.endef
	.def sort_forw;	.val 16; 	.scl 8;	.type 24;	.size 4;	.endef
	.def sort_back;	.val 20; 	.scl 8;	.type 24;	.size 4;	.endef
	.def PrivateOp;	.val 24; 	.scl 8;	.type 24;	.size 4;	.endef
	.def QueueSize;	.val 28;	.scl 8;	.type 4;	.endef
	.def dev;	.val 32;	.scl 8;	.type 4;	.endef
	.def Is_Open;	.val 36;	.scl 8;	.type 4;	.endef
	.def EXCL_Open;	.val 40;	.scl 8;	.type 4;	.endef
	.def First_Open;	.val 352;	.scl 18;	.type 14;	.size 1;	.endef
	.def Opening;	.val 353;	.scl 18;	.type 14;	.size 1;	.endef
	.def Forget_On_Close;	.val 354;	.scl 18;	.type 14;	.size 1;	.endef
	.def ReadInfoOnClose;	.val 355;	.scl 18;	.type 14;	.size 1;	.endef
	.def Write_Protected;	.val 356;	.scl 18;	.type 14;	.size 1;	.endef
	.def Busy;	.val 357;	.scl 18;	.type 14;	.size 1;	.endef
	.def Initiator;	.val 358;	.scl 18;	.type 14;	.size 1;	.endef
	.def Filemark_Found;	.val 359;	.scl 18;	.type 14;	.size 1;	.endef
	.def End_Of_Media;	.val 360;	.scl 18;	.type 14;	.size 1;	.endef
	.def No_Data_Found;	.val 361;	.scl 18;	.type 14;	.size 1;	.endef
	.def Rewound;	.val 362;	.scl 18;	.type 14;	.size 1;	.endef
	.def Written;	.val 363;	.scl 18;	.type 14;	.size 1;	.endef
	.def Middle_Read;	.val 364;	.scl 18;	.type 14;	.size 1;	.endef
	.def Fixed;	.val 365;	.scl 18;	.type 14;	.size 1;	.endef
	.def Caching;	.val 366;	.scl 18;	.type 14;	.size 1;	.endef
	.def Byte_Order_Exception;	.val 367;	.scl 18;	.type 14;	.size 1;	.endef
	.def Sort_Direction;	.val 368;	.scl 18;	.type 14;	.size 1;	.endef
	.def Sort_Device;	.val 369;	.scl 18;	.type 14;	.size 1;	.endef
	.def NegotiateSynchronous;	.val 370;	.scl 18;	.type 14;	.size 1;	.endef
	.def NegotiateQueueing;	.val 371;	.scl 18;	.type 14;	.size 1;	.endef
	.def NegotiateWide;	.val 372;	.scl 18;	.type 14;	.size 1;	.endef
	.def ReadyToNegotiate;	.val 373;	.scl 18;	.type 14;	.size 1;	.endef
	.def LowLevelDriverLocked;	.val 374;	.scl 18;	.type 14;	.size 1;	.endef
	.def FirstLUN;	.val 375;	.scl 18;	.type 14;	.size 1;	.endef
	.def pad;	.val 376;	.scl 18;	.type 14;	.size 8;	.endef
	.def SCSI_device;	.val 48;	.scl 8;	.type 8;	.tag partID;	.size 4;	.endef
	.def Bus_forw;	.val 52; 	.scl 8;	.type 24;	.tag SCSI_Device;	.size 1096;	.size 4;	.endef
	.def Bus_back;	.val 56; 	.scl 8;	.type 24;	.tag SCSI_Device;	.size 1096;	.size 4;	.endef
	.def Next_Device;	.val 60; 	.scl 8;	.type 24;	.tag SCSI_Device;	.size 1096;	.size 4;	.endef
	.def Bus_Info;	.val 64; 	.scl 8;	.type 24;	.tag SCSI_Device;	.size 1096;	.size 4;	.endef
	.def block_size;	.val 68;	.scl 8;	.type 4;	.endef
	.def previous_key;	.val 72;	.scl 8;	.type 4;	.endef
	.def turnaround;	.val 76; 	.scl 8;	.type 24;	.size 4;	.endef
	.def driver;	.val 80;	.scl 8;	.type 4;	.endef
	.def Cmd_Page;	.val 84; 	.scl 8;	.type 16;	.size 4;	.endef
	.def Timeout_Table;	.val 88; 	.scl 8;	.type 19;	.size 4;	.endef
	.def sort_funcs;	.val 92; 	.scl 8;	.type 24;	.size 4;	.endef
	.def request_sense_op;	.val 96; 	.scl 8;	.type 24;	.size 4;	.endef
	.def inquiry_data;	.val 100;	.scl 8;	.type 8;	.tag SCSI_Inquiry_Data;	.size 36;	.endef
	.def sense_data;	.val 136;	.scl 8;	.type 8;	.tag SCSI_Sense_Data;	.size 252;	.endef
	.def Statistics;	.val 388;	.scl 8;	.type 8;	.tag StatStructure;	.size 204;	.endef
	.def PartitionOpenInformation;	.val 592; 	.scl 8;	.type 56;	.tag P_OpenInfo;	.size 8;	.dim 32;	.size 256;	.endef
	.def Block_Length;	.val 848;	.scl 8;	.type 4;	.endef
	.def Fragment_Mask;	.val 852;	.scl 8;	.type 4;	.endef
	.def CommandQueue;	.val 856; 	.scl 8;	.type 88;	.size 4;	.endef
	.def NumberOfCommandsQueued;	.val 860;	.scl 8;	.type 4;	.endef
	.def debug_tag1;	.val 864;	.scl 8;	.type 4;	.endef
	.def SCSI_Device_Configuration;	.val 868; 	.scl 8;	.type 24;	.size 4;	.endef
	.def CurrentConfiguration;	.val 872;	.scl 8;	.type 8;	.size 16;	.endef
	.def Driver;	.val 888;	.scl 8;	.type 9;	.tag Driver_Specific_Info;	.size 28;	.endef
	.def debug_tag2;	.val 916;	.scl 8;	.type 4;	.endef
	.def type;	.val 920;	.scl 8;	.type 9;	.tag Device_Specific_Info;	.size 44;	.endef
	.def debug_tag3;	.val 964;	.scl 8;	.type 4;	.endef
	.def SCSI_Memory_Allocated;	.val 968; 	.scl 8;	.type 56;	.tag SCSI_Memory;	.size 8;	.dim 16;	.size 128;	.endef
	.def .eos;	.val 1096;	.scl 102;	.tag SCSI_Device;	.size 1096;	.endef
	.def Sscript;	.scl 10;	.type 8;	.size 16;	.endef
	.def s_bcount;	.val 0;	.scl 8;	.type 4;	.endef
	.def s_addr;	.val 4;	.scl 8;	.type 4;	.endef
	.def JUMP;	.val 8;	.scl 8;	.type 4;	.endef
	.def NEXT;	.val 12;	.scl 8;	.type 4;	.endef
	.def .eos;	.val 16;	.scl 102;	.tag Sscript;	.size 16;	.endef
	.def Simpl;	.scl 10;	.type 8;	.size 12;	.endef
	.def residual;	.val 0;	.scl 8;	.type 4;	.endef
	.def s_addr;	.val 4;	.scl 8;	.type 4;	.endef
	.def last_iov;	.val 8; 	.scl 8;	.type 24;	.size 4;	.endef
	.def .eos;	.val 12;	.scl 102;	.tag Simpl;	.size 12;	.endef
	.def FreeInfo;	.scl 10;	.type 8;	.size 16;	.endef
	.def h_forw;	.val 0; 	.scl 8;	.type 24;	.size 4;	.endef
	.def h_back;	.val 4; 	.scl 8;	.type 24;	.size 4;	.endef
	.def iov_back;	.val 8; 	.scl 8;	.type 24;	.size 4;	.endef
	.def iov_maxcount;	.val 12;	.scl 8;	.type 4;	.endef
	.def .eos;	.val 16;	.scl 102;	.tag FreeInfo;	.size 16;	.endef
	.def VectorUnion;	.scl 12;	.type 9;	.size 16;	.endef
	.def SSCRIPT;	.val 0;	.scl 8;	.type 8;	.tag Sscript;	.size 16;	.endef
	.def SIMPL;	.val 0;	.scl 8;	.type 8;	.tag Simpl;	.size 12;	.endef
	.def FreeInfo;	.val 0;	.scl 8;	.type 8;	.tag FreeInfo;	.size 16;	.endef
	.def .eos;	.val 16;	.scl 102;	.tag VectorUnion;	.size 16;	.endef
	.def SCSI_IO_Vector;	.scl 10;	.type 8;	.size 44;	.endef
	.def s_bcount;	.val 0;	.scl 8;	.type 4;	.endef
	.def s_addr;	.val 4; 	.scl 8;	.type 18;	.size 4;	.endef
	.def s_iov;	.val 8; 	.scl 8;	.type 24;	.tag SCSI_IO_Vector;	.size 44;	.size 4;	.endef
	.def VU;	.val 12;	.scl 8;	.type 9;	.tag VectorUnion;	.size 16;	.endef
	.def Free_Header;	.val 28; 	.scl 8;	.type 24;	.tag SCSI_IO_Vector;	.size 44;	.size 4;	.endef
	.def bp;	.val 32; 	.scl 8;	.type 24;	.tag buf;	.size 0;	.size 4;	.endef
	.def s_total_count;	.val 36;	.scl 8;	.type 4;	.endef
	.def iov_wanted;	.val 40;	.scl 8;	.type 4;	.endef
	.def .eos;	.val 44;	.scl 102;	.tag SCSI_IO_Vector;	.size 44;	.endef
	.def tmp_buffer;	.scl 10;	.type 8;	.size 8;	.endef
	.def s_size;	.val 0;	.scl 8;	.type 4;	.endef
	.def s_addr;	.val 4; 	.scl 8;	.type 16;	.size 4;	.endef
	.def .eos;	.val 8;	.scl 102;	.tag tmp_buffer;	.size 8;	.endef
	.def PhysicalAddresses;	.scl 10;	.type 8;	.size 12;	.endef
	.def SCSI_Message_Bytes;	.val 0;	.scl 8;	.type 4;	.endef
	.def SCSI_Status_Byte;	.val 4;	.scl 8;	.type 4;	.endef
	.def s_scsi_cmd_blk;	.val 8;	.scl 8;	.type 4;	.endef
	.def .eos;	.val 12;	.scl 102;	.tag PhysicalAddresses;	.size 12;	.endef
	.def SIOP_Op;	.scl 10;	.type 8;	.size 80;	.endef
	.def RestartSCRIPT;	.val 0;	.scl 8;	.type 14;	.endef
	.def PMM_Move;	.val 4;	.scl 8;	.type 14;	.endef
	.def PMM_ReturnAddress;	.val 8;	.scl 8;	.type 14;	.endef
	.def PMM_ScriptAddress;	.val 12;	.scl 8;	.type 14;	.endef
	.def PMM_ByteCountLeft;	.val 16;	.scl 8;	.type 14;	.endef
	.def IOV;	.val 20;	.scl 11;	.type 8;	.tag SCSI_IO_Vector;	.size 44;	.endef
	.def SCSI_SCRIPT;	.val 64; 	.scl 8;	.type 30;	.size 4;	.endef
	.def PA;	.val 68;	.scl 11;	.type 8;	.tag PhysicalAddresses;	.size 12;	.endef
	.def .eos;	.val 80;	.scl 102;	.tag SIOP_Op;	.size 80;	.endef
	.def WD_Driver_Op;	.scl 10;	.type 8;	.size 10;	.endef
	.def SCSI_DMA_state;	.val 0; 	.scl 8;	.type 50;	.dim 8;	.size 8;	.endef
	.def WD_Command_Type;	.val 8;	.scl 8;	.type 2;	.endef
	.def SCSI_Phase;	.val 9;	.scl 8;	.type 2;	.endef
	.def .eos;	.val 10;	.scl 102;	.tag WD_Driver_Op;	.size 10;	.endef
	.def HA_Driver_Op;	.scl 10;	.type 8;	.size 4;	.endef
	.def av_forw;	.val 0; 	.scl 8;	.type 24;	.size 4;	.endef
	.def .eos;	.val 4;	.scl 102;	.tag HA_Driver_Op;	.size 4;	.endef
	.def FP_Driver_Op;	.scl 10;	.type 8;	.size 4;	.endef
	.def x;	.val 0;	.scl 8;	.type 4;	.endef
	.def .eos;	.val 4;	.scl 102;	.tag FP_Driver_Op;	.size 4;	.endef
	.def SIMPL_Op;	.scl 10;	.type 8;	.size 16;	.endef
	.def sequence_start;	.val 0;	.scl 8;	.type 12;	.endef
	.def flags;	.val 1;	.scl 8;	.type 12;	.endef
	.def Qtag_nbr;	.val 4;	.scl 8;	.type 4;	.endef
	.def op_with_sense;	.val 8; 	.scl 8;	.type 24;	.size 4;	.endef
	.def wide_residue;	.val 12;	.scl 8;	.type 4;	.endef
	.def .eos;	.val 16;	.scl 102;	.tag SIMPL_Op;	.size 16;	.endef
	.def Driver_Specific_Info_Per_SCSI_Op;	.scl 12;	.type 9;	.size 80;	.endef
	.def SIOP;	.val 0;	.scl 8;	.type 8;	.tag SIOP_Op;	.size 80;	.endef
	.def WD_Driver_Op;	.val 0;	.scl 8;	.type 8;	.tag WD_Driver_Op;	.size 10;	.endef
	.def HA_Driver_Op;	.val 0;	.scl 8;	.type 8;	.tag HA_Driver_Op;	.size 4;	.endef
	.def FP_Driver_Op;	.val 0;	.scl 8;	.type 8;	.tag FP_Driver_Op;	.size 4;	.endef
	.def SIMPL_Op;	.val 0;	.scl 8;	.type 8;	.tag SIMPL_Op;	.size 16;	.endef
	.def .eos;	.val 80;	.scl 102;	.tag Driver_Specific_Info_Per_SCSI_Op;	.size 80;	.endef
	.def Save_SCSI_Pointers;	.scl 10;	.type 8;	.size 16;	.endef
	.def s_iov_ptr;	.val 0; 	.scl 8;	.type 24;	.tag SCSI_IO_Vector;	.size 44;	.size 4;	.endef
	.def s_addr;	.val 4; 	.scl 8;	.type 18;	.size 4;	.endef
	.def s_bcount;	.val 8;	.scl 8;	.type 4;	.endef
	.def s_total_count;	.val 12;	.scl 8;	.type 4;	.endef
	.def .eos;	.val 16;	.scl 102;	.tag Save_SCSI_Pointers;	.size 16;	.endef
	.def SCSI_Op;	.scl 10;	.type 8;	.size 332;	.endef
	.def s_bp;	.val 0; 	.scl 8;	.type 24;	.tag buf;	.size 0;	.size 4;	.endef
	.def s_dev;	.val 4; 	.scl 8;	.type 24;	.tag SCSI_Device;	.size 1096;	.size 4;	.endef
	.def s_forw;	.val 8; 	.scl 8;	.type 24;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def s_back;	.val 12; 	.scl 8;	.type 24;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def sort_forw;	.val 16; 	.scl 8;	.type 24;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def sort_back;	.val 20; 	.scl 8;	.type 24;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def h_forw;	.val 24; 	.scl 8;	.type 24;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def h_back;	.val 28; 	.scl 8;	.type 24;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def S_ACTIVE;	.val 256;	.scl 18;	.type 14;	.size 1;	.endef
	.def S_DONE;	.val 257;	.scl 18;	.type 14;	.size 1;	.endef
	.def S_ERROR;	.val 258;	.scl 18;	.type 14;	.size 1;	.endef
	.def S_ASYNC;	.val 259;	.scl 18;	.type 14;	.size 1;	.endef
	.def S_NO_LOG;	.val 260;	.scl 18;	.type 14;	.size 1;	.endef
	.def S_DELAY;	.val 261;	.scl 18;	.type 14;	.size 1;	.endef
	.def S_0;	.val 262;	.scl 18;	.type 14;	.size 1;	.endef
	.def S_1;	.val 263;	.scl 18;	.type 14;	.size 1;	.endef
	.def S_2;	.val 264;	.scl 18;	.type 14;	.size 1;	.endef
	.def S_3;	.val 265;	.scl 18;	.type 14;	.size 1;	.endef
	.def S_4;	.val 266;	.scl 18;	.type 14;	.size 1;	.endef
	.def S_5;	.val 267;	.scl 18;	.type 14;	.size 1;	.endef
	.def S_6;	.val 268;	.scl 18;	.type 14;	.size 1;	.endef
	.def S_7;	.val 269;	.scl 18;	.type 14;	.size 1;	.endef
	.def S_8;	.val 270;	.scl 18;	.type 14;	.size 1;	.endef
	.def S_9;	.val 271;	.scl 18;	.type 14;	.size 1;	.endef
	.def S_10;	.val 272;	.scl 18;	.type 14;	.size 1;	.endef
	.def S_11;	.val 273;	.scl 18;	.type 14;	.size 1;	.endef
	.def S_12;	.val 274;	.scl 18;	.type 14;	.size 1;	.endef
	.def S_13;	.val 275;	.scl 18;	.type 14;	.size 1;	.endef
	.def S_14;	.val 276;	.scl 18;	.type 14;	.size 1;	.endef
	.def S_15;	.val 277;	.scl 18;	.type 14;	.size 1;	.endef
	.def S_16;	.val 278;	.scl 18;	.type 14;	.size 1;	.endef
	.def S_FREE;	.val 279;	.scl 18;	.type 14;	.size 1;	.endef
	.def Byte_Order_Exception;	.val 280;	.scl 18;	.type 14;	.size 1;	.endef
	.def Concat;	.val 281;	.scl 18;	.type 14;	.size 1;	.endef
	.def Concatable;	.val 282;	.scl 18;	.type 14;	.size 1;	.endef
	.def Static;	.val 283;	.scl 18;	.type 14;	.size 1;	.endef
	.def Save;	.val 284;	.scl 18;	.type 14;	.size 1;	.endef
	.def DIR_NONE;	.val 285;	.scl 18;	.type 14;	.size 1;	.endef
	.def DIR_DEV_TO_MEM;	.val 286;	.scl 18;	.type 14;	.size 1;	.endef
	.def DIR_MEM_TO_DEV;	.val 287;	.scl 18;	.type 14;	.size 1;	.endef
	.def s_scsi_cmd_blk;	.val 36;	.scl 8;	.type 8;	.tag SCSI_Command_Block;	.size 12;	.endef
	.def s_op_state;	.val 48;	.scl 8;	.type 12;	.endef
	.def s_queue_type;	.val 49;	.scl 8;	.type 12;	.endef
	.def s_retrys;	.val 52;	.scl 8;	.type 4;	.endef
	.def s_delay_time;	.val 56;	.scl 8;	.type 4;	.endef
	.def s_maxretrys;	.val 60;	.scl 8;	.type 4;	.endef
	.def s_block_number;	.val 64;	.scl 8;	.type 4;	.endef
	.def s_cylinder;	.val 68;	.scl 8;	.type 4;	.endef
	.def s_timeout;	.val 72;	.scl 8;	.type 4;	.endef
	.def s_timer;	.val 76;	.scl 8;	.type 4;	.endef
	.def s_coi;	.val 80; 	.scl 8;	.type 148;	.size 4;	.endef
	.def s_residual;	.val 84;	.scl 8;	.type 4;	.endef
	.def Free_Header;	.val 88; 	.scl 8;	.type 24;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def concat_forw;	.val 92; 	.scl 8;	.type 24;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def concat_back;	.val 96; 	.scl 8;	.type 24;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def s_iov;	.val 100;	.scl 8;	.type 8;	.tag SCSI_IO_Vector;	.size 44;	.endef
	.def s_iov_ptr;	.val 144; 	.scl 8;	.type 24;	.tag SCSI_IO_Vector;	.size 44;	.size 4;	.endef
	.def s_iov_last;	.val 148; 	.scl 8;	.type 24;	.tag SCSI_IO_Vector;	.size 44;	.size 4;	.endef
	.def s_total_count;	.val 152;	.scl 8;	.type 4;	.endef
	.def s_save_count;	.val 156;	.scl 8;	.type 4;	.endef
	.def s_error_record_ptr;	.val 160;	.scl 8;	.type 8;	.tag tmp_buffer;	.size 8;	.endef
	.def s_error;	.val 168;	.scl 8;	.type 4;	.endef
	.def s_iostart;	.val 172;	.scl 8;	.type 4;	.endef
	.def s_temp;	.val 176; 	.scl 8;	.type 56;	.tag tmp_buffer;	.size 8;	.dim 4;	.size 32;	.endef
	.def Driver_Op;	.val 208;	.scl 8;	.type 9;	.tag Driver_Specific_Info_Per_SCSI_Op;	.size 80;	.endef
	.def Save_SCSI_Pointers;	.val 288;	.scl 8;	.type 8;	.tag Save_SCSI_Pointers;	.size 16;	.endef
	.def SCSI_Status_Byte;	.val 304;	.scl 8;	.type 12;	.endef
	.def SCSI_Message_Size;	.val 308;	.scl 8;	.type 4;	.endef
	.def SCSI_Message_Bytes;	.val 312; 	.scl 8;	.type 60;	.dim 12;	.size 12;	.endef
	.def s_sense;	.val 324; 	.scl 8;	.type 24;	.tag SCSI_Sense_Data;	.size 252;	.size 4;	.endef
	.def s_owner_id;	.val 328;	.scl 8;	.type 4;	.endef
	.def .eos;	.val 332;	.scl 102;	.tag SCSI_Op;	.size 332;	.endef
	.def SCSI_Sort_Table;	.scl 10;	.type 8;	.size 16;	.endef
	.def sort_queue;	.val 0; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def change_sort_key;	.val 4; 	.scl 8;	.type 144;	.size 4;	.endef
	.def finishup_sort_op;	.val 8; 	.scl 8;	.type 144;	.size 4;	.endef
	.def retry_op;	.val 12; 	.scl 8;	.type 148;	.size 4;	.endef
	.def .eos;	.val 16;	.scl 102;	.tag SCSI_Sort_Table;	.size 16;	.endef
	.def SCSI_Device_Configuration;	.scl 10;	.type 8;	.size 16;	.endef
	.def Disconnect;	.val 0;	.scl 8;	.type 12;	.endef
	.def SynchronousOffset;	.val 1;	.scl 8;	.type 12;	.endef
	.def SynchronousPeriod;	.val 2;	.scl 8;	.type 12;	.endef
	.def Wide_Transfer_Size;	.val 3;	.scl 8;	.type 12;	.endef
	.def Target_Support;	.val 4;	.scl 8;	.type 12;	.endef
	.def Queue_Size;	.val 5;	.scl 8;	.type 12;	.endef
	.def Sort_Table;	.val 8; 	.scl 8;	.type 24;	.tag SCSI_Sort_Table;	.size 16;	.size 4;	.endef
	.def Reserved1;	.val 12;	.scl 8;	.type 12;	.endef
	.def Reserved2;	.val 13;	.scl 8;	.type 12;	.endef
	.def .eos;	.val 16;	.scl 102;	.tag SCSI_Device_Configuration;	.size 16;	.endef
	.def SCSI_Device_Configuration_t;	.scl 13;	.type 8;	.tag SCSI_Device_Configuration;	.size 16;	.size 16;	.endef
	.def CDB_struct;	.scl 10;	.type 8;	.size 20;	.endef
	.def Byte;	.val 0; 	.scl 8;	.type 60;	.dim 12;	.size 12;	.endef
	.def Buffer;	.val 12; 	.scl 8;	.type 28;	.size 4;	.endef
	.def BufferLength;	.val 16;	.scl 8;	.type 14;	.endef
	.def .eos;	.val 20;	.scl 102;	.tag CDB_struct;	.size 20;	.endef
	.def CDB_t;	.scl 13;	.type 8;	.tag CDB_struct;	.size 20;	.size 20;	.endef
	.def SCSI_Arguments;	.scl 10;	.type 8;	.size 72;	.endef
	.def SenseData;	.val 0; 	.scl 8;	.type 24;	.tag SCSI_Sense_Data;	.size 252;	.size 4;	.endef
	.def Command_Indicator;	.val 4;	.scl 8;	.type 4;	.endef
	.def ArgumentList;	.val 8; 	.scl 8;	.type 52;	.dim 16;	.size 64;	.endef
	.def .eos;	.val 72;	.scl 102;	.tag SCSI_Arguments;	.size 72;	.endef
	.def SCSI_Arguments_t;	.scl 13;	.type 8;	.tag SCSI_Arguments;	.size 72;	.size 72;	.endef
	.def SCSI_Tallies_t;	.scl 13;	.type 8;	.tag SCSI_Tallies;	.size 164;	.size 164;	.endef
	.def StatStructure_t;	.scl 13;	.type 8;	.tag StatStructure;	.size 204;	.size 204;	.endef
	.def errno_t;	.scl 13;	.type 4;	.size 4;	.endef
	.def DeviceId;	.scl 10;	.type 8;	.size 52;	.endef
	.def Periph_Qualifier;	.val 0;	.scl 18;	.type 14;	.size 3;	.endef
	.def Periph_Device_Type;	.val 3;	.scl 18;	.type 14;	.size 5;	.endef
	.def Removable_Media;	.val 8;	.scl 18;	.type 14;	.size 1;	.endef
	.def Device_Type_Qualifier;	.val 9;	.scl 18;	.type 14;	.size 7;	.endef
	.def ISO_Version;	.val 16;	.scl 18;	.type 14;	.size 2;	.endef
	.def ECMA_Version;	.val 18;	.scl 18;	.type 14;	.size 3;	.endef
	.def ANSI_Version;	.val 21;	.scl 18;	.type 14;	.size 3;	.endef
	.def AENC;	.val 24;	.scl 18;	.type 14;	.size 1;	.endef
	.def reserved_0;	.val 25;	.scl 18;	.type 14;	.size 3;	.endef
	.def Response_Data_Format;	.val 28;	.scl 18;	.type 14;	.size 4;	.endef
	.def Additional_Length;	.val 32;	.scl 18;	.type 14;	.size 8;	.endef
	.def reserved_1;	.val 40;	.scl 18;	.type 14;	.size 16;	.endef
	.def RelAdr;	.val 56;	.scl 18;	.type 14;	.size 1;	.endef
	.def WBus32;	.val 57;	.scl 18;	.type 14;	.size 1;	.endef
	.def WBus16;	.val 58;	.scl 18;	.type 14;	.size 1;	.endef
	.def Sync;	.val 59;	.scl 18;	.type 14;	.size 1;	.endef
	.def Linked;	.val 60;	.scl 18;	.type 14;	.size 1;	.endef
	.def reserved_2;	.val 61;	.scl 18;	.type 14;	.size 1;	.endef
	.def CmdQue;	.val 62;	.scl 18;	.type 14;	.size 1;	.endef
	.def SftReset;	.val 63;	.scl 18;	.type 14;	.size 1;	.endef
	.def Vendor_ID;	.val 8; 	.scl 8;	.type 50;	.dim 8;	.size 8;	.endef
	.def Product_ID;	.val 16; 	.scl 8;	.type 50;	.dim 16;	.size 16;	.endef
	.def Revision_Level;	.val 32; 	.scl 8;	.type 50;	.dim 4;	.size 4;	.endef
	.def KF_PeriphQualifier;	.val 288;	.scl 18;	.type 14;	.size 1;	.endef
	.def KF_DeviceTypeQualifier;	.val 289;	.scl 18;	.type 14;	.size 1;	.endef
	.def KF_ISO_Version;	.val 290;	.scl 18;	.type 14;	.size 1;	.endef
	.def KF_ECMA_Version;	.val 291;	.scl 18;	.type 14;	.size 1;	.endef
	.def KF_ANSI_Version;	.val 292;	.scl 18;	.type 14;	.size 1;	.endef
	.def KF_AENC;	.val 293;	.scl 18;	.type 14;	.size 1;	.endef
	.def KF_ResponseDataFormat;	.val 294;	.scl 18;	.type 14;	.size 1;	.endef
	.def KF_AdditionalLength;	.val 295;	.scl 18;	.type 14;	.size 1;	.endef
	.def KF_RelAdr;	.val 296;	.scl 18;	.type 14;	.size 1;	.endef
	.def KF_WBus32;	.val 297;	.scl 18;	.type 14;	.size 1;	.endef
	.def KF_WBus16;	.val 298;	.scl 18;	.type 14;	.size 1;	.endef
	.def KF_Sync;	.val 299;	.scl 18;	.type 14;	.size 1;	.endef
	.def KF_Linked;	.val 300;	.scl 18;	.type 14;	.size 1;	.endef
	.def KF_Reserved;	.val 301;	.scl 18;	.type 14;	.size 1;	.endef
	.def KF_CmdQue;	.val 302;	.scl 18;	.type 14;	.size 1;	.endef
	.def KF_SftReset;	.val 303;	.scl 18;	.type 14;	.size 1;	.endef
	.def KF_VendorIdentification;	.val 304;	.scl 18;	.type 14;	.size 1;	.endef
	.def KF_ProductIdentification;	.val 305;	.scl 18;	.type 14;	.size 1;	.endef
	.def KF_ProductRevisionLevel;	.val 306;	.scl 18;	.type 14;	.size 1;	.endef
	.def KF_Unused;	.val 307;	.scl 18;	.type 14;	.size 13;	.endef
	.def Cmd_Page;	.val 40; 	.scl 8;	.type 16;	.size 4;	.endef
	.def Timeout_Table;	.val 44; 	.scl 8;	.type 19;	.size 4;	.endef
	.def Configuration_Table;	.val 48; 	.scl 8;	.type 24;	.tag SCSI_Device_Configuration;	.size 16;	.size 4;	.endef
	.def .eos;	.val 52;	.scl 102;	.tag DeviceId;	.size 52;	.endef
	.def DeviceId_t;	.scl 13;	.type 8;	.tag DeviceId;	.size 52;	.size 52;	.endef
	.def All_Types_Cmd_Page;	.scl 10;	.type 8;	.size 52;	.endef
	.def vdev_test_ready;	.val 0; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_request_sense;	.val 4; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_inquiry;	.val 8; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_reserve;	.val 12; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_release;	.val 16; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_mode_sense;	.val 20; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_mode_select;	.val 24; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_send_diagnostic;	.val 28; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_receive_diagnostic;	.val 32; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_prevent_allow;	.val 36; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_read_capacity;	.val 40; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_reassign;	.val 44; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_rezero;	.val 48; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def .eos;	.val 52;	.scl 102;	.tag All_Types_Cmd_Page;	.size 52;	.endef
	.def Disk_Cmd_Page;	.scl 10;	.type 8;	.size 108;	.endef
	.def all;	.val 0;	.scl 8;	.type 8;	.tag All_Types_Cmd_Page;	.size 52;	.endef
	.def vdev_format;	.val 52; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_read;	.val 56; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_read_defect_data;	.val 60; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_start_stop;	.val 64; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_synchronize_cache;	.val 68; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_verify;	.val 72; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_write;	.val 76; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_set_flex;	.val 80; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_user0;	.val 84; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_user1;	.val 88; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_user2;	.val 92; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_user3;	.val 96; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_user4;	.val 100; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_user5;	.val 104; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def .eos;	.val 108;	.scl 102;	.tag Disk_Cmd_Page;	.size 108;	.endef
	.def Tape_Cmd_Page;	.scl 10;	.type 8;	.size 116;	.endef
	.def all;	.val 0;	.scl 8;	.type 8;	.tag All_Types_Cmd_Page;	.size 52;	.endef
	.def vdev_erase;	.val 52; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_load_unload;	.val 56; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_read;	.val 60; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_read_block_limits;	.val 64; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_rewind;	.val 68; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_space;	.val 72; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_verify;	.val 76; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_write;	.val 80; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_write_filemark;	.val 84; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_set_block_length;	.val 88; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_get_density;	.val 92; 	.scl 8;	.type 148;	.size 4;	.endef
	.def vdev_user1;	.val 96; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_user2;	.val 100; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_user3;	.val 104; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_user4;	.val 108; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_user5;	.val 112; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def .eos;	.val 116;	.scl 102;	.tag Tape_Cmd_Page;	.size 116;	.endef
	.def Printer_Cmd_Page;	.scl 10;	.type 8;	.size 100;	.endef
	.def all;	.val 0;	.scl 8;	.type 8;	.tag All_Types_Cmd_Page;	.size 52;	.endef
	.def vdev_format;	.val 52; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_print;	.val 56; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_slew_and_print;	.val 60; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_stop_print;	.val 64; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_synchronize_buffer;	.val 68; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_setup;	.val 72; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_user0;	.val 76; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_user1;	.val 80; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_user2;	.val 84; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_user3;	.val 88; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_user4;	.val 92; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def vdev_user5;	.val 96; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def .eos;	.val 100;	.scl 102;	.tag Printer_Cmd_Page;	.size 100;	.endef
	.def Cache_Tape_t;	.scl 13;	.type 8;	.tag Cache_Tape;	.size 28;	.size 28;	.endef
	.def P_OpenInfo_t;	.scl 13;	.type 8;	.tag P_OpenInfo;	.size 8;	.size 8;	.endef
	.def SCSI_Device_t;	.scl 13;	.type 8;	.tag SCSI_Device;	.size 1096;	.size 1096;	.endef
	.def SCSI_Sort_Table_t;	.scl 13;	.type 8;	.tag SCSI_Sort_Table;	.size 16;	.size 16;	.endef
	.def SCSI_Device_Hash;	.scl 10;	.type 8;	.size 8;	.endef
	.def Device_forw;	.val 0; 	.scl 8;	.type 24;	.tag SCSI_Device;	.size 1096;	.size 4;	.endef
	.def Device_back;	.val 4; 	.scl 8;	.type 24;	.tag SCSI_Device;	.size 1096;	.size 4;	.endef
	.def .eos;	.val 8;	.scl 102;	.tag SCSI_Device_Hash;	.size 8;	.endef
	.def SCSI_Device_Hash_t;	.scl 13;	.type 8;	.tag SCSI_Device_Hash;	.size 8;	.size 8;	.endef
	.def SCSI_IO_Vector_t;	.scl 13;	.type 8;	.tag SCSI_IO_Vector;	.size 44;	.size 44;	.endef
	.def Save_SCSI_Pointers_t;	.scl 13;	.type 8;	.tag Save_SCSI_Pointers;	.size 16;	.size 16;	.endef
	.def OS_Device;	.scl 10;	.type 8;	.size 4;	.endef
	.def Partition;	.val 0;	.scl 18;	.type 14;	.size 4;	.endef
	.def PUN;	.val 4;	.scl 18;	.type 14;	.size 4;	.endef
	.def LUN;	.val 8;	.scl 18;	.type 14;	.size 3;	.endef
	.def SCSI_Bus;	.val 11;	.scl 18;	.type 14;	.size 2;	.endef
	.def SCSI_Controller;	.val 13;	.scl 18;	.type 14;	.size 3;	.endef
	.def IO_Bus;	.val 16;	.scl 18;	.type 14;	.size 2;	.endef
	.def OS_Major;	.val 18;	.scl 18;	.type 14;	.size 14;	.endef
	.def .eos;	.val 4;	.scl 102;	.tag OS_Device;	.size 4;	.endef
	.def OS_Device_t;	.scl 13;	.type 8;	.tag OS_Device;	.size 4;	.size 4;	.endef
	.def temp_buffer_t;	.scl 13;	.type 8;	.tag tmp_buffer;	.size 8;	.size 8;	.endef
	.def SCSI_Op_t;	.scl 13;	.type 8;	.tag SCSI_Op;	.size 332;	.size 332;	.endef
	.def csdevsw;	.scl 10;	.type 8;	.size 44;	.endef
	.def d_open;	.val 0; 	.scl 8;	.type 148;	.size 4;	.endef
	.def d_close;	.val 4; 	.scl 8;	.type 148;	.size 4;	.endef
	.def d_read;	.val 8; 	.scl 8;	.type 148;	.size 4;	.endef
	.def d_write;	.val 12; 	.scl 8;	.type 148;	.size 4;	.endef
	.def d_strategy;	.val 16; 	.scl 8;	.type 408;	.tag SCSI_Op;	.size 332;	.size 4;	.endef
	.def d_ioctl;	.val 20; 	.scl 8;	.type 148;	.size 4;	.endef
	.def d_init;	.val 24; 	.scl 8;	.type 148;	.size 4;	.endef
	.def d_size;	.val 28; 	.scl 8;	.type 148;	.size 4;	.endef
	.def d_pfr;	.val 32; 	.scl 8;	.type 148;	.size 4;	.endef
	.def d_comp;	.val 36; 	.scl 8;	.type 144;	.size 4;	.endef
	.def d_flag;	.val 40;	.scl 8;	.type 4;	.endef
	.def .eos;	.val 44;	.scl 102;	.tag csdevsw;	.size 44;	.endef
	.def LLD_Switch;	.scl 10;	.type 8;	.size 24;	.endef
	.def d_init;	.val 0; 	.scl 8;	.type 148;	.size 4;	.endef
	.def d_open;	.val 4; 	.scl 8;	.type 148;	.size 4;	.endef
	.def d_start;	.val 8; 	.scl 8;	.type 144;	.size 4;	.endef
	.def d_pfr;	.val 12; 	.scl 8;	.type 148;	.size 4;	.endef
	.def d_configure;	.val 16; 	.scl 8;	.type 148;	.size 4;	.endef
	.def d_intr;	.val 20; 	.scl 8;	.type 148;	.size 4;	.endef
	.def .eos;	.val 24;	.scl 102;	.tag LLD_Switch;	.size 24;	.endef
	.def SCSI_BUSInfo;	.scl 10;	.type 8;	.size 24;	.endef
	.def device_info;	.val 0; 	.scl 8;	.type 24;	.tag SCSI_Device;	.size 1096;	.size 4;	.endef
	.def SCSI_DMA_Channel;	.val 4;	.scl 8;	.type 12;	.endef
	.def flags;	.val 8;	.scl 8;	.type 14;	.endef
	.def state;	.val 12;	.scl 8;	.type 12;	.endef
	.def mode;	.val 13;	.scl 8;	.type 12;	.endef
	.def current_pun;	.val 14;	.scl 8;	.type 12;	.endef
	.def current_lun;	.val 15;	.scl 8;	.type 12;	.endef
	.def scsi_id;	.val 16;	.scl 8;	.type 12;	.endef
	.def base_addr_offset;	.val 20;	.scl 8;	.type 14;	.endef
	.def .eos;	.val 24;	.scl 102;	.tag SCSI_BUSInfo;	.size 24;	.endef
	.def SCSI_BUSInfo_t;	.scl 13;	.type 8;	.tag SCSI_BUSInfo;	.size 24;	.size 24;	.endef
	.def ncr53c90;	.scl 10;	.type 8;	.size 2;	.endef
	.def haPOS0;	.val 0;	.scl 8;	.type 12;	.endef
	.def haPOS1;	.val 1;	.scl 8;	.type 12;	.endef
	.def .eos;	.val 2;	.scl 102;	.tag ncr53c90;	.size 2;	.endef
	.def ncr53c700;	.scl 10;	.type 8;	.size 12;	.endef
	.def ClockFrequencyBits;	.val 0;	.scl 8;	.type 4;	.endef
	.def tcp;	.val 4;	.scl 8;	.type 4;	.endef
	.def haPOS0;	.val 8;	.scl 8;	.type 12;	.endef
	.def haPOS1;	.val 9;	.scl 8;	.type 12;	.endef
	.def .eos;	.val 12;	.scl 102;	.tag ncr53c700;	.size 12;	.endef
	.def SIMPL;	.scl 10;	.type 8;	.size 4;	.endef
	.def just_junk_now;	.val 0;	.scl 8;	.type 14;	.endef
	.def .eos;	.val 4;	.scl 102;	.tag SIMPL;	.size 4;	.endef
	.def Controller_Specific_Info;	.scl 12;	.type 9;	.size 12;	.endef
	.def ncr53c90;	.val 0;	.scl 8;	.type 8;	.tag ncr53c90;	.size 2;	.endef
	.def ncr53c700;	.val 0;	.scl 8;	.type 8;	.tag ncr53c700;	.size 12;	.endef
	.def SIMPL;	.val 0;	.scl 8;	.type 8;	.tag SIMPL;	.size 4;	.endef
	.def .eos;	.val 12;	.scl 102;	.tag Controller_Specific_Info;	.size 12;	.endef
	.def SCSI_CtrlInfo;	.scl 10;	.type 8;	.size 52;	.endef
	.def Initialized;	.val 0;	.scl 8;	.type 12;	.endef
	.def driver;	.val 4;	.scl 8;	.type 14;	.endef
	.def flags;	.val 8;	.scl 8;	.type 14;	.endef
	.def BaseAddress;	.val 12;	.scl 8;	.type 4;	.endef
	.def SCSI_Vector;	.val 16;	.scl 8;	.type 14;	.endef
	.def HostID_Low;	.val 20;	.scl 8;	.type 12;	.endef
	.def HostID_High;	.val 21;	.scl 8;	.type 12;	.endef
	.def SlotNumber;	.val 22;	.scl 8;	.type 12;	.endef
	.def NumberOfBusses;	.val 24;	.scl 8;	.type 14;	.endef
	.def SCSI_BUSInfo;	.val 28; 	.scl 8;	.type 120;	.tag SCSI_BUSInfo;	.size 24;	.dim 2;	.size 8;	.endef
	.def Controller;	.val 36;	.scl 8;	.type 9;	.tag Controller_Specific_Info;	.size 12;	.endef
	.def AdditionalInformation;	.val 48; 	.scl 8;	.type 18;	.size 4;	.endef
	.def .eos;	.val 52;	.scl 102;	.tag SCSI_CtrlInfo;	.size 52;	.endef
	.def SCSI_CtrlInfo_t;	.scl 13;	.type 8;	.tag SCSI_CtrlInfo;	.size 52;	.size 52;	.endef
	.def IO_BusInfo;	.scl 10;	.type 8;	.size 44;	.endef
	.def state;	.val 0;	.scl 8;	.type 12;	.endef
	.def bus_type;	.val 1;	.scl 8;	.type 12;	.endef
	.def mode;	.val 2;	.scl 8;	.type 12;	.endef
	.def number_of_slots;	.val 4;	.scl 8;	.type 14;	.endef
	.def SCSI_CtrlInfo;	.val 8; 	.scl 8;	.type 120;	.tag SCSI_CtrlInfo;	.size 52;	.dim 8;	.size 32;	.endef
	.def AdditionalInformation;	.val 40; 	.scl 8;	.type 18;	.size 4;	.endef
	.def .eos;	.val 44;	.scl 102;	.tag IO_BusInfo;	.size 44;	.endef
	.def IO_BusInfo_t;	.scl 13;	.type 8;	.tag IO_BusInfo;	.size 44;	.size 44;	.endef
	.def u_int32;	.scl 13;	.type 14;	.size 4;	.endef
	.def Inquiry_CDB;	.scl 10;	.type 8;	.size 8;	.endef
	.def Op_Code;	.val 0;	.scl 18;	.type 14;	.size 8;	.endef
	.def EVPD;	.val 8;	.scl 18;	.type 14;	.size 1;	.endef
	.def Resv1;	.val 9;	.scl 18;	.type 14;	.size 4;	.endef
	.def LUN;	.val 13;	.scl 18;	.type 14;	.size 3;	.endef
	.def Pg_Code;	.val 16;	.scl 18;	.type 14;	.size 8;	.endef
	.def Reserved;	.val 24;	.scl 18;	.type 14;	.size 8;	.endef
	.def Allocation_Length;	.val 4;	.scl 8;	.type 12;	.endef
	.def Control_byte;	.val 5;	.scl 8;	.type 12;	.endef
	.def .eos;	.val 8;	.scl 102;	.tag Inquiry_CDB;	.size 8;	.endef
	.def Inquiry_CDB_t;	.scl 13;	.type 8;	.tag Inquiry_CDB;	.size 8;	.size 8;	.endef
	.def Read_Write_CDB;	.scl 10;	.type 8;	.size 10;	.endef
	.def Op_Code;	.val 0;	.scl 8;	.type 12;	.endef
	.def RelAdr;	.val 8;	.scl 18;	.type 12;	.size 1;	.endef
	.def Resv1;	.val 9;	.scl 18;	.type 12;	.size 2;	.endef
	.def FUA;	.val 11;	.scl 18;	.type 12;	.size 1;	.endef
	.def DPO;	.val 12;	.scl 18;	.type 12;	.size 1;	.endef
	.def LUN;	.val 13;	.scl 18;	.type 12;	.size 3;	.endef
	.def Logical_Blk_Addr;	.val 2; 	.scl 8;	.type 60;	.dim 4;	.size 4;	.endef
	.def Resv2;	.val 6;	.scl 8;	.type 12;	.endef
	.def Transfer_Length;	.val 7; 	.scl 8;	.type 60;	.dim 2;	.size 2;	.endef
	.def Control_Byte;	.val 9;	.scl 8;	.type 12;	.endef
	.def .eos;	.val 10;	.scl 102;	.tag Read_Write_CDB;	.size 10;	.endef
	.def Read_Write_CDB_t;	.scl 13;	.type 8;	.tag Read_Write_CDB;	.size 10;	.size 10;	.endef
	.def Mode_Sense_CDB;	.scl 10;	.type 8;	.size 10;	.endef
	.def Op_Code;	.val 0;	.scl 18;	.type 12;	.size 8;	.endef
	.def Resv1;	.val 8;	.scl 18;	.type 12;	.size 3;	.endef
	.def DBD;	.val 11;	.scl 18;	.type 12;	.size 1;	.endef
	.def Resv2;	.val 12;	.scl 18;	.type 12;	.size 1;	.endef
	.def LUN;	.val 13;	.scl 18;	.type 12;	.size 3;	.endef
	.def Pg_Code;	.val 16;	.scl 18;	.type 12;	.size 6;	.endef
	.def Pg_Ctrl;	.val 22;	.scl 18;	.type 12;	.size 2;	.endef
	.def Resv3;	.val 3; 	.scl 8;	.type 60;	.dim 4;	.size 4;	.endef
	.def Allocation_Length;	.val 7; 	.scl 8;	.type 60;	.dim 2;	.size 2;	.endef
	.def Control_Byte;	.val 9;	.scl 8;	.type 12;	.endef
	.def .eos;	.val 10;	.scl 102;	.tag Mode_Sense_CDB;	.size 10;	.endef
	.def Mode_Sense_CDB_t;	.scl 13;	.type 8;	.tag Mode_Sense_CDB;	.size 10;	.size 10;	.endef
	.def Mode_Select_CDB;	.scl 10;	.type 8;	.size 10;	.endef
	.def Op_Code;	.val 0;	.scl 18;	.type 12;	.size 8;	.endef
	.def SP;	.val 8;	.scl 18;	.type 12;	.size 1;	.endef
	.def Reserved;	.val 9;	.scl 18;	.type 12;	.size 3;	.endef
	.def PF;	.val 12;	.scl 18;	.type 12;	.size 1;	.endef
	.def LUN;	.val 13;	.scl 18;	.type 12;	.size 3;	.endef
	.def Resv1;	.val 2; 	.scl 8;	.type 60;	.dim 5;	.size 5;	.endef
	.def Param_List_Length;	.val 7; 	.scl 8;	.type 60;	.dim 2;	.size 2;	.endef
	.def Control_Byte;	.val 9;	.scl 8;	.type 12;	.endef
	.def .eos;	.val 10;	.scl 102;	.tag Mode_Select_CDB;	.size 10;	.endef
	.def Mode_Select_CDB_t;	.scl 13;	.type 8;	.tag Mode_Select_CDB;	.size 10;	.size 10;	.endef
	.def .1150fak;	.scl 10;	.type 8;	.size 8;	.endef
	.def Buffer_ID;	.val 0;	.scl 8;	.type 12;	.endef
	.def Buffer_Offset;	.val 1; 	.scl 8;	.type 60;	.dim 3;	.size 3;	.endef
	.def Param_List_Length;	.val 4; 	.scl 8;	.type 60;	.dim 3;	.size 3;	.endef
	.def Control_Byte;	.val 7;	.scl 8;	.type 12;	.endef
	.def .eos;	.val 8;	.scl 102;	.tag .1150fak;	.size 8;	.endef
	.def .1154fak;	.scl 10;	.type 8;	.size 8;	.endef
	.def Ctlr_Addr;	.val 0; 	.scl 8;	.type 60;	.dim 4;	.size 4;	.endef
	.def Transfer_Length;	.val 4; 	.scl 8;	.type 60;	.dim 3;	.size 3;	.endef
	.def Control_Byte;	.val 7;	.scl 8;	.type 12;	.endef
	.def .eos;	.val 8;	.scl 102;	.tag .1154fak;	.size 8;	.endef
	.def .1156fak;	.scl 12;	.type 9;	.size 8;	.endef
	.def Buffer_Info;	.val 0;	.scl 8;	.type 8;	.tag .1150fak;	.size 8;	.endef
	.def Array_Specific;	.val 0;	.scl 8;	.type 8;	.tag .1154fak;	.size 8;	.endef
	.def .eos;	.val 8;	.scl 102;	.tag .1156fak;	.size 8;	.endef
	.def Write_Buffer_CDB;	.scl 10;	.type 8;	.size 10;	.endef
	.def Op_Code;	.val 0;	.scl 8;	.type 12;	.endef
	.def Mode;	.val 8;	.scl 18;	.type 12;	.size 3;	.endef
	.def Reserved;	.val 11;	.scl 18;	.type 12;	.size 2;	.endef
	.def LUN;	.val 13;	.scl 18;	.type 12;	.size 3;	.endef
	.def _8_bytes;	.val 2;	.scl 8;	.type 9;	.tag .1156fak;	.size 8;	.endef
	.def .eos;	.val 10;	.scl 102;	.tag Write_Buffer_CDB;	.size 10;	.endef
	.def Write_Buffer_CDB_t;	.scl 13;	.type 8;	.tag Write_Buffer_CDB;	.size 10;	.size 10;	.endef
	.def Send_Recv_Diagnostic_CDB;	.scl 10;	.type 8;	.size 6;	.endef
	.def Op_Code;	.val 0;	.scl 8;	.type 12;	.endef
	.def UnitOfl;	.val 8;	.scl 18;	.type 12;	.size 1;	.endef
	.def DevOfl;	.val 9;	.scl 18;	.type 12;	.size 1;	.endef
	.def SelfTest;	.val 10;	.scl 18;	.type 12;	.size 1;	.endef
	.def Resv1;	.val 11;	.scl 18;	.type 12;	.size 1;	.endef
	.def Pg_Format;	.val 12;	.scl 18;	.type 12;	.size 1;	.endef
	.def LUN;	.val 13;	.scl 18;	.type 12;	.size 3;	.endef
	.def Reserved;	.val 2;	.scl 8;	.type 12;	.endef
	.def Length;	.val 3; 	.scl 8;	.type 60;	.dim 2;	.size 2;	.endef
	.def Control_Byte;	.val 5;	.scl 8;	.type 12;	.endef
	.def .eos;	.val 6;	.scl 102;	.tag Send_Recv_Diagnostic_CDB;	.size 6;	.endef
	.def Send_Recv_Diagnostic_CDB_t;	.scl 13;	.type 8;	.tag Send_Recv_Diagnostic_CDB;	.size 6;	.size 6;	.endef
	.def Request_Sense_CDB;	.scl 10;	.type 8;	.size 6;	.endef
	.def Op_Code;	.val 0;	.scl 8;	.type 12;	.endef
	.def Reserved;	.val 8;	.scl 18;	.type 12;	.size 3;	.endef
	.def AEN;	.val 11;	.scl 18;	.type 12;	.size 1;	.endef
	.def Reserved2;	.val 12;	.scl 18;	.type 12;	.size 1;	.endef
	.def LUN;	.val 13;	.scl 18;	.type 12;	.size 3;	.endef
	.def Resv1;	.val 2;	.scl 8;	.type 12;	.endef
	.def Resv2;	.val 3;	.scl 8;	.type 12;	.endef
	.def Allocation_Length;	.val 4;	.scl 8;	.type 12;	.endef
	.def Control_Byte;	.val 5;	.scl 8;	.type 12;	.endef
	.def .eos;	.val 6;	.scl 102;	.tag Request_Sense_CDB;	.size 6;	.endef
	.def Request_Sense_CDB_t;	.scl 13;	.type 8;	.tag Request_Sense_CDB;	.size 6;	.size 6;	.endef
	.def Log_Select_CDB;	.scl 10;	.type 8;	.size 10;	.endef
	.def Op_Code;	.val 0;	.scl 8;	.type 12;	.endef
	.def SP;	.val 8;	.scl 18;	.type 12;	.size 1;	.endef
	.def PCR;	.val 9;	.scl 18;	.type 12;	.size 1;	.endef
	.def Resv1;	.val 10;	.scl 18;	.type 12;	.size 3;	.endef
	.def LUN;	.val 13;	.scl 18;	.type 12;	.size 3;	.endef
	.def Resv2;	.val 16;	.scl 18;	.type 12;	.size 6;	.endef
	.def Pg_Ctrl;	.val 22;	.scl 18;	.type 12;	.size 2;	.endef
	.def Resv3;	.val 3; 	.scl 8;	.type 60;	.dim 4;	.size 4;	.endef
	.def Param_List_Length;	.val 7; 	.scl 8;	.type 60;	.dim 2;	.size 2;	.endef
	.def Control_Byte;	.val 9;	.scl 8;	.type 12;	.endef
	.def .eos;	.val 10;	.scl 102;	.tag Log_Select_CDB;	.size 10;	.endef
	.def Log_Select_CDB_t;	.scl 13;	.type 8;	.tag Log_Select_CDB;	.size 10;	.size 10;	.endef
	.def Log_Sense_CDB;	.scl 10;	.type 8;	.size 10;	.endef
	.def Op_Code;	.val 0;	.scl 8;	.type 12;	.endef
	.def SP;	.val 8;	.scl 18;	.type 12;	.size 1;	.endef
	.def PPC;	.val 9;	.scl 18;	.type 12;	.size 1;	.endef
	.def Resv1;	.val 10;	.scl 18;	.type 12;	.size 3;	.endef
	.def LUN;	.val 13;	.scl 18;	.type 12;	.size 3;	.endef
	.def Pg_Code;	.val 16;	.scl 18;	.type 12;	.size 6;	.endef
	.def Pg_Ctrl;	.val 22;	.scl 18;	.type 12;	.size 2;	.endef
	.def Resv2;	.val 3; 	.scl 8;	.type 60;	.dim 2;	.size 2;	.endef
	.def Param_Pointer;	.val 5; 	.scl 8;	.type 60;	.dim 2;	.size 2;	.endef
	.def Allocation_Length;	.val 7; 	.scl 8;	.type 60;	.dim 2;	.size 2;	.endef
	.def Control_Byte;	.val 9;	.scl 8;	.type 12;	.endef
	.def .eos;	.val 10;	.scl 102;	.tag Log_Sense_CDB;	.size 10;	.endef
	.def Log_Sense_CDB_t;	.scl 13;	.type 8;	.tag Log_Sense_CDB;	.size 10;	.size 10;	.endef
	.def Mode10ParameterHeader;	.scl 10;	.type 8;	.size 8;	.endef
	.def ModeDataLength;	.val 0; 	.scl 8;	.type 60;	.dim 2;	.size 2;	.endef
	.def MediumType;	.val 2;	.scl 8;	.type 12;	.endef
	.def DeviceSpecificParameter;	.val 3;	.scl 8;	.type 12;	.endef
	.def Reserved1;	.val 4;	.scl 8;	.type 12;	.endef
	.def Reserved2;	.val 5;	.scl 8;	.type 12;	.endef
	.def BlockDescriptorLength;	.val 6; 	.scl 8;	.type 60;	.dim 2;	.size 2;	.endef
	.def .eos;	.val 8;	.scl 102;	.tag Mode10ParameterHeader;	.size 8;	.endef
	.def Mode10ParameterHeader_t;	.scl 13;	.type 8;	.tag Mode10ParameterHeader;	.size 8;	.size 8;	.endef
	.def Mode10BlockDescriptor;	.scl 10;	.type 8;	.size 8;	.endef
	.def NumberOfBlocks;	.val 0;	.scl 18;	.type 14;	.size 24;	.endef
	.def DensityCode;	.val 24;	.scl 18;	.type 14;	.size 8;	.endef
	.def BlockLength;	.val 32;	.scl 18;	.type 14;	.size 24;	.endef
	.def reserved;	.val 56;	.scl 18;	.type 14;	.size 8;	.endef
	.def .eos;	.val 8;	.scl 102;	.tag Mode10BlockDescriptor;	.size 8;	.endef
	.def Mode10BlockDescriptor_t;	.scl 13;	.type 8;	.tag Mode10BlockDescriptor;	.size 8;	.size 8;	.endef
	.def logical_array_page;	.scl 10;	.type 8;	.size 148;	.endef
	.def Header;	.val 0;	.scl 8;	.type 8;	.tag Mode10ParameterHeader;	.size 8;	.endef
	.def BlockDescriptor;	.val 8;	.scl 8;	.type 8;	.tag Mode10BlockDescriptor;	.size 8;	.endef
	.def Page_Code;	.val 128;	.scl 18;	.type 12;	.size 7;	.endef
	.def Parameters_Savable;	.val 135;	.scl 18;	.type 12;	.size 1;	.endef
	.def Page_Length;	.val 17;	.scl 8;	.type 12;	.endef
	.def Action_Status;	.val 18;	.scl 8;	.type 12;	.endef
	.def RAID_Level;	.val 19;	.scl 8;	.type 12;	.endef
	.def LUN_Type;	.val 20;	.scl 8;	.type 12;	.endef
	.def LUN_Block_Size;	.val 21; 	.scl 8;	.type 60;	.dim 4;	.size 4;	.endef
	.def Drive_Sector_Size;	.val 25; 	.scl 8;	.type 60;	.dim 2;	.size 2;	.endef
	.def LUN_Number_of_Blocks;	.val 27; 	.scl 8;	.type 60;	.dim 4;	.size 4;	.endef
	.def Reserved_1;	.val 31; 	.scl 8;	.type 60;	.dim 4;	.size 4;	.endef
	.def LUN_Segment_Size;	.val 35; 	.scl 8;	.type 60;	.dim 4;	.size 4;	.endef
	.def Segment_Zero_Size;	.val 39;	.scl 8;	.type 12;	.endef
	.def Type_of_Mode_Page_to_Sense;	.val 320;	.scl 18;	.type 12;	.size 2;	.endef
	.def Reserved_6;	.val 322;	.scl 18;	.type 12;	.size 6;	.endef
	.def Reserved_2;	.val 328;	.scl 18;	.type 12;	.size 1;	.endef
	.def Auto_Detect_Replace_Disabled;	.val 329;	.scl 18;	.type 12;	.size 1;	.endef
	.def Degraded_Modes_Writes_Disabled;	.val 330;	.scl 18;	.type 12;	.size 1;	.endef
	.def Array_Assurance_Disabled;	.val 331;	.scl 18;	.type 12;	.size 1;	.endef
	.def AEN_Polling_Enabled;	.val 332;	.scl 18;	.type 12;	.size 1;	.endef
	.def Parity_Verification_Enabled;	.val 333;	.scl 18;	.type 12;	.size 1;	.endef
	.def Wr_Parity_Verification_Enabled;	.val 334;	.scl 18;	.type 12;	.size 1;	.endef
	.def Reserved_3;	.val 335;	.scl 18;	.type 12;	.size 1;	.endef
	.def Block_Reconstructed;	.val 42; 	.scl 8;	.type 60;	.dim 4;	.size 4;	.endef
	.def Reconstruction_Frequency;	.val 46;	.scl 8;	.type 12;	.endef
	.def Reconstruction_Amount;	.val 47; 	.scl 8;	.type 60;	.dim 2;	.size 2;	.endef
	.def Lun_Number;	.val 49;	.scl 8;	.type 12;	.endef
	.def Reserved_5;	.val 50; 	.scl 8;	.type 60;	.dim 2;	.size 2;	.endef
	.def Disk_Bit_Map;	.val 52; 	.scl 8;	.type 60;	.dim 32;	.size 32;	.endef
	.def Configuration_Table;	.val 84; 	.scl 8;	.type 60;	.dim 64;	.size 64;	.endef
	.def .eos;	.val 148;	.scl 102;	.tag logical_array_page;	.size 148;	.endef
	.def logical_array_page_t;	.scl 13;	.type 8;	.tag logical_array_page;	.size 148;	.size 148;	.endef
	.def physical_array_page;	.scl 10;	.type 8;	.size 260;	.endef
	.def Header;	.val 0;	.scl 8;	.type 8;	.tag Mode10ParameterHeader;	.size 8;	.endef
	.def BlockDescriptor;	.val 8;	.scl 8;	.type 8;	.tag Mode10BlockDescriptor;	.size 8;	.endef
	.def Page_Code;	.val 128;	.scl 18;	.type 12;	.size 7;	.endef
	.def Parameters_Savable;	.val 135;	.scl 18;	.type 12;	.size 1;	.endef
	.def Page_Length;	.val 17;	.scl 8;	.type 12;	.endef
	.def Drive_Status;	.val 18; 	.scl 8;	.type 60;	.dim 240;	.size 240;	.endef
	.def .eos;	.val 260;	.scl 102;	.tag physical_array_page;	.size 260;	.endef
	.def physical_array_page_t;	.scl 13;	.type 8;	.tag physical_array_page;	.size 260;	.size 260;	.endef
	.def redundant_controller_page;	.scl 10;	.type 8;	.size 124;	.endef
	.def Header;	.val 0;	.scl 8;	.type 8;	.tag Mode10ParameterHeader;	.size 8;	.endef
	.def BlockDescriptor;	.val 8;	.scl 8;	.type 8;	.tag Mode10BlockDescriptor;	.size 8;	.endef
	.def Page_Code;	.val 128;	.scl 18;	.type 12;	.size 6;	.endef
	.def Reserved_1;	.val 134;	.scl 18;	.type 12;	.size 1;	.endef
	.def Parameters_Savable;	.val 135;	.scl 18;	.type 12;	.size 1;	.endef
	.def Page_Length;	.val 17;	.scl 8;	.type 12;	.endef
	.def Controller_Serial_No;	.val 18; 	.scl 8;	.type 60;	.dim 16;	.size 16;	.endef
	.def Alt_Controller_Serial_No;	.val 34; 	.scl 8;	.type 60;	.dim 16;	.size 16;	.endef
	.def RDAC_Mode;	.val 50; 	.scl 8;	.type 60;	.dim 2;	.size 2;	.endef
	.def Alt_RDAC_Mode;	.val 52; 	.scl 8;	.type 60;	.dim 2;	.size 2;	.endef
	.def Reserved_2;	.val 54;	.scl 8;	.type 12;	.endef
	.def Reserved_3;	.val 55;	.scl 8;	.type 12;	.endef
	.def Controller_LUN_Table;	.val 56; 	.scl 8;	.type 60;	.dim 64;	.size 64;	.endef
	.def Reserved_4;	.val 120;	.scl 8;	.type 12;	.endef
	.def Reserved_5;	.val 121;	.scl 8;	.type 12;	.endef
	.def .eos;	.val 124;	.scl 102;	.tag redundant_controller_page;	.size 124;	.endef
	.def redundant_controller_page_t;	.scl 13;	.type 8;	.tag redundant_controller_page;	.size 124;	.size 124;	.endef
	.def rw_recovery_page;	.scl 10;	.type 8;	.size 28;	.endef
	.def Header;	.val 0;	.scl 8;	.type 8;	.tag Mode10ParameterHeader;	.size 8;	.endef
	.def BlockDescriptor;	.val 8;	.scl 8;	.type 8;	.tag Mode10BlockDescriptor;	.size 8;	.endef
	.def Page_Code;	.val 128;	.scl 18;	.type 12;	.size 7;	.endef
	.def Parameters_Savable;	.val 135;	.scl 18;	.type 12;	.size 1;	.endef
	.def Page_Length;	.val 17;	.scl 8;	.type 12;	.endef
	.def DCR;	.val 144;	.scl 18;	.type 12;	.size 1;	.endef
	.def DTE;	.val 145;	.scl 18;	.type 12;	.size 1;	.endef
	.def PER;	.val 146;	.scl 18;	.type 12;	.size 1;	.endef
	.def EER;	.val 147;	.scl 18;	.type 12;	.size 1;	.endef
	.def RC;	.val 148;	.scl 18;	.type 12;	.size 1;	.endef
	.def TB;	.val 149;	.scl 18;	.type 12;	.size 1;	.endef
	.def ARRE;	.val 150;	.scl 18;	.type 12;	.size 1;	.endef
	.def AWRE;	.val 151;	.scl 18;	.type 12;	.size 1;	.endef
	.def Read_Retry_Count;	.val 19;	.scl 8;	.type 12;	.endef
	.def Correction_Span;	.val 20;	.scl 8;	.type 12;	.endef
	.def Head_Offset_Count;	.val 21;	.scl 8;	.type 12;	.endef
	.def Data_Strobe_Offset_Count;	.val 22;	.scl 8;	.type 12;	.endef
	.def Reserved1;	.val 23;	.scl 8;	.type 12;	.endef
	.def Write_Retry_Count;	.val 24;	.scl 8;	.type 12;	.endef
	.def Reserved2;	.val 25;	.scl 8;	.type 12;	.endef
	.def Recovery_Time_Limit;	.val 26; 	.scl 8;	.type 60;	.dim 2;	.size 2;	.endef
	.def .eos;	.val 28;	.scl 102;	.tag rw_recovery_page;	.size 28;	.endef
	.def rw_recovery_page_t;	.scl 13;	.type 8;	.tag rw_recovery_page;	.size 28;	.size 28;	.endef
	.def date_time_page;	.scl 10;	.type 8;	.size 24;	.endef
	.def Header;	.val 0;	.scl 8;	.type 8;	.tag Mode10ParameterHeader;	.size 8;	.endef
	.def BlockDescriptor;	.val 8;	.scl 8;	.type 8;	.tag Mode10BlockDescriptor;	.size 8;	.endef
	.def Page_Code;	.val 128;	.scl 18;	.type 12;	.size 6;	.endef
	.def Reserved_1;	.val 134;	.scl 18;	.type 12;	.size 1;	.endef
	.def Parameters_Savable;	.val 135;	.scl 18;	.type 12;	.size 1;	.endef
	.def Reserved_2;	.val 17;	.scl 8;	.type 12;	.endef
	.def Reserved_3;	.val 18;	.scl 8;	.type 12;	.endef
	.def Number_of_Seconds;	.val 19; 	.scl 8;	.type 60;	.dim 4;	.size 4;	.endef
	.def .eos;	.val 24;	.scl 102;	.tag date_time_page;	.size 24;	.endef
	.def date_time_page_t;	.scl 13;	.type 8;	.tag date_time_page;	.size 24;	.size 24;	.endef
	.def recovery_actions;	.scl 10;	.type 8;	.size 2;	.endef
	.def Reserved;	.val 0;	.scl 18;	.type 13;	.size 1;	.endef
	.def Information_Logged;	.val 1;	.scl 18;	.type 13;	.size 1;	.endef
	.def Drive_Isolation;	.val 2;	.scl 18;	.type 13;	.size 1;	.endef
	.def Bus_Isolation;	.val 3;	.scl 18;	.type 13;	.size 1;	.endef
	.def SCSI_Chip_Level_Zeros;	.val 4;	.scl 18;	.type 13;	.size 1;	.endef
	.def Drive_Assurance;	.val 5;	.scl 18;	.type 13;	.size 1;	.endef
	.def Array_Assurance;	.val 6;	.scl 18;	.type 13;	.size 1;	.endef
	.def Synchronous_Re_Negotiation;	.val 7;	.scl 18;	.type 13;	.size 1;	.endef
	.def Asynchronous_Transfer;	.val 8;	.scl 18;	.type 13;	.size 1;	.endef
	.def SCSI_Sequences_Overriden;	.val 9;	.scl 18;	.type 13;	.size 1;	.endef
	.def Bus_Or_Device_Reset;	.val 10;	.scl 18;	.type 13;	.size 1;	.endef
	.def Start_Stop_Command;	.val 11;	.scl 18;	.type 13;	.size 1;	.endef
	.def Block_Reassigned;	.val 12;	.scl 18;	.type 13;	.size 1;	.endef
	.def RTZ_Command;	.val 13;	.scl 18;	.type 13;	.size 1;	.endef
	.def Drive_Command_Broken_Down;	.val 14;	.scl 18;	.type 13;	.size 1;	.endef
	.def Parity_Used;	.val 15;	.scl 18;	.type 13;	.size 1;	.endef
	.def .eos;	.val 2;	.scl 102;	.tag recovery_actions;	.size 2;	.endef
	.def recovery_actions_t;	.scl 13;	.type 8;	.tag recovery_actions;	.size 2;	.size 2;	.endef
	.def fru_group_qualifier;	.scl 10;	.type 8;	.size 2;	.endef
	.def SDP_Chip;	.val 0;	.scl 18;	.type 12;	.size 1;	.endef
	.def SCSI_Chip_0;	.val 1;	.scl 18;	.type 12;	.size 1;	.endef
	.def SCSI_Chip_1;	.val 2;	.scl 18;	.type 12;	.size 1;	.endef
	.def SCSI_Chip_2;	.val 3;	.scl 18;	.type 12;	.size 1;	.endef
	.def SCSI_Chip_3;	.val 4;	.scl 18;	.type 12;	.size 1;	.endef
	.def SCSI_Chip_4;	.val 5;	.scl 18;	.type 12;	.size 1;	.endef
	.def SCSI_Chip_5;	.val 6;	.scl 18;	.type 12;	.size 1;	.endef
	.def Reserved_1;	.val 7;	.scl 18;	.type 12;	.size 1;	.endef
	.def Reserved_2;	.val 8;	.scl 18;	.type 12;	.size 4;	.endef
	.def SCSI_Cable;	.val 12;	.scl 18;	.type 12;	.size 1;	.endef
	.def Drive_Power_Supply;	.val 13;	.scl 18;	.type 12;	.size 1;	.endef
	.def Drive_Electronics;	.val 14;	.scl 18;	.type 12;	.size 1;	.endef
	.def Drive_HDA;	.val 15;	.scl 18;	.type 12;	.size 1;	.endef
	.def .eos;	.val 2;	.scl 102;	.tag fru_group_qualifier;	.size 2;	.endef
	.def fru_group_qualifier_t;	.scl 13;	.type 8;	.tag fru_group_qualifier;	.size 2;	.size 2;	.endef
	.def additional_sense;	.scl 10;	.type 8;	.size 142;	.endef
	.def Recovery_Actions;	.val 0; 	.scl 8;	.type 60;	.dim 2;	.size 2;	.endef
	.def Total_Errors;	.val 2;	.scl 8;	.type 12;	.endef
	.def Total_Retry_Count;	.val 3;	.scl 8;	.type 12;	.endef
	.def Error_Stack;	.val 4; 	.scl 8;	.type 60;	.dim 4;	.size 4;	.endef
	.def Additional_FRU_Info;	.val 8; 	.scl 8;	.type 60;	.dim 8;	.size 8;	.endef
	.def Error_Specific_Information;	.val 16; 	.scl 8;	.type 60;	.dim 3;	.size 3;	.endef
	.def Error_Detection_Point;	.val 19; 	.scl 8;	.type 60;	.dim 4;	.size 4;	.endef
	.def Original_CDB;	.val 23; 	.scl 8;	.type 60;	.dim 10;	.size 10;	.endef
	.def Host_ID;	.val 33;	.scl 8;	.type 12;	.endef
	.def Host_Descriptor;	.val 34; 	.scl 8;	.type 60;	.dim 2;	.size 2;	.endef
	.def Serial_Number;	.val 36; 	.scl 8;	.type 60;	.dim 16;	.size 16;	.endef
	.def Revision_Level;	.val 52; 	.scl 8;	.type 60;	.dim 4;	.size 4;	.endef
	.def Data_Transfer_Operation;	.val 56;	.scl 8;	.type 12;	.endef
	.def LUN;	.val 57;	.scl 8;	.type 12;	.endef
	.def LUN_Status;	.val 58;	.scl 8;	.type 12;	.endef
	.def Drive_Identifier;	.val 59;	.scl 8;	.type 12;	.endef
	.def Transfer_Start_Id;	.val 60;	.scl 8;	.type 12;	.endef
	.def Transfer_Start_LBA;	.val 61; 	.scl 8;	.type 60;	.dim 4;	.size 4;	.endef
	.def Drive_Product_ID;	.val 65; 	.scl 8;	.type 60;	.dim 16;	.size 16;	.endef
	.def Power_Up_Status;	.val 81; 	.scl 8;	.type 60;	.dim 2;	.size 2;	.endef
	.def RAID_Level;	.val 83;	.scl 8;	.type 12;	.endef
	.def Drive_Sense_Id;	.val 84; 	.scl 8;	.type 60;	.dim 2;	.size 2;	.endef
	.def Drive_Sense_Data;	.val 86; 	.scl 8;	.type 60;	.dim 32;	.size 32;	.endef
	.def Reserved_Area;	.val 118; 	.scl 8;	.type 60;	.dim 24;	.size 24;	.endef
	.def .eos;	.val 142;	.scl 102;	.tag additional_sense;	.size 142;	.endef
	.def additional_sense_t;	.scl 13;	.type 8;	.tag additional_sense;	.size 142;	.size 142;	.endef
	.def .1378fak;	.scl 10;	.type 8;	.size 8;	.endef
	.def SCSI_Controller;	.val 3;	.scl 18;	.type 14;	.size 4;	.endef
	.def SCSI_Bus;	.val 7;	.scl 18;	.type 14;	.size 3;	.endef
	.def PUN;	.val 10;	.scl 18;	.type 14;	.size 4;	.endef
	.def LUN;	.val 14;	.scl 18;	.type 14;	.size 3;	.endef
	.def DAC_SCSI_Bus;	.val 17;	.scl 18;	.type 14;	.size 4;	.endef
	.def DAC_PUN;	.val 21;	.scl 18;	.type 14;	.size 4;	.endef
	.def DAC_LUN;	.val 25;	.scl 18;	.type 14;	.size 3;	.endef
	.def IO_Bus;	.val 4;	.scl 8;	.type 14;	.endef
	.def .eos;	.val 8;	.scl 102;	.tag .1378fak;	.size 8;	.endef
	.def hw_zipcode_t;	.scl 13;	.type 8;	.tag .1378fak;	.size 8;	.size 8;	.endef
	.def Bytes_In_Long;	.scl 10;	.type 8;	.size 4;	.endef
	.def LSB;	.val 0;	.scl 18;	.type 14;	.size 8;	.endef
	.def MIB_2;	.val 8;	.scl 18;	.type 14;	.size 8;	.endef
	.def MIB_1;	.val 16;	.scl 18;	.type 14;	.size 8;	.endef
	.def MSB;	.val 24;	.scl 18;	.type 14;	.size 8;	.endef
	.def .eos;	.val 4;	.scl 102;	.tag Bytes_In_Long;	.size 4;	.endef
	.def Bytes_In_Long_t;	.scl 13;	.type 8;	.tag Bytes_In_Long;	.size 4;	.size 4;	.endef
	.def Bytes_In_Short;	.scl 10;	.type 8;	.size 2;	.endef
	.def LSB;	.val 0;	.scl 18;	.type 13;	.size 8;	.endef
	.def MSB;	.val 8;	.scl 18;	.type 13;	.size 8;	.endef
	.def .eos;	.val 2;	.scl 102;	.tag Bytes_In_Short;	.size 2;	.endef
	.def Bytes_In_Short_t;	.scl 13;	.type 8;	.tag Bytes_In_Short;	.size 2;	.size 2;	.endef
	.def Bits_In_Short;	.scl 10;	.type 8;	.size 2;	.endef
	.def b8;	.val 0;	.scl 18;	.type 13;	.size 1;	.endef
	.def b9;	.val 1;	.scl 18;	.type 13;	.size 1;	.endef
	.def b10;	.val 2;	.scl 18;	.type 13;	.size 1;	.endef
	.def b11;	.val 3;	.scl 18;	.type 13;	.size 1;	.endef
	.def b12;	.val 4;	.scl 18;	.type 13;	.size 1;	.endef
	.def b13;	.val 5;	.scl 18;	.type 13;	.size 1;	.endef
	.def b14;	.val 6;	.scl 18;	.type 13;	.size 1;	.endef
	.def b15;	.val 7;	.scl 18;	.type 13;	.size 1;	.endef
	.def b0;	.val 8;	.scl 18;	.type 13;	.size 1;	.endef
	.def b1;	.val 9;	.scl 18;	.type 13;	.size 1;	.endef
	.def b2;	.val 10;	.scl 18;	.type 13;	.size 1;	.endef
	.def b3;	.val 11;	.scl 18;	.type 13;	.size 1;	.endef
	.def b4;	.val 12;	.scl 18;	.type 13;	.size 1;	.endef
	.def b5;	.val 13;	.scl 18;	.type 13;	.size 1;	.endef
	.def b6;	.val 14;	.scl 18;	.type 13;	.size 1;	.endef
	.def b7;	.val 15;	.scl 18;	.type 13;	.size 1;	.endef
	.def .eos;	.val 2;	.scl 102;	.tag Bits_In_Short;	.size 2;	.endef
	.def Bits_In_Short_t;	.scl 13;	.type 8;	.tag Bits_In_Short;	.size 2;	.size 2;	.endef
	.def scsi_stat;	.scl 10;	.type 8;	.size 4;	.endef
	.def Errno;	.val 0;	.scl 18;	.type 14;	.size 16;	.endef
	.def Reserved;	.val 16;	.scl 18;	.type 14;	.size 14;	.endef
	.def Sense_Present;	.val 30;	.scl 18;	.type 14;	.size 1;	.endef
	.def Error;	.val 31;	.scl 18;	.type 14;	.size 1;	.endef
	.def .eos;	.val 4;	.scl 102;	.tag scsi_stat;	.size 4;	.endef
	.def scsi_stat_t;	.scl 13;	.type 8;	.tag scsi_stat;	.size 4;	.size 4;	.endef
	.def object;	.scl 10;	.type 8;	.size 72;	.endef
	.def keyval;	.val 0; 	.scl 8;	.type 60;	.dim 32;	.size 32;	.endef
	.def description;	.val 32; 	.scl 8;	.type 18;	.size 4;	.endef
	.def describe_fp;	.val 36; 	.scl 8;	.type 24;	.tag .61fak;	.size 36;	.size 4;	.endef
	.def is_a_nop_obj;	.val 320;	.scl 18;	.type 14;	.size 1;	.endef
	.def arg_needed;	.val 321;	.scl 18;	.type 14;	.size 1;	.endef
	.def destroy;	.val 44; 	.scl 8;	.type 144;	.size 4;	.endef
	.def describe;	.val 48; 	.scl 8;	.type 144;	.size 4;	.endef
	.def redirect;	.val 52; 	.scl 8;	.type 144;	.size 4;	.endef
	.def key;	.val 56; 	.scl 8;	.type 412;	.size 4;	.endef
	.def needs_arg;	.val 60; 	.scl 8;	.type 158;	.size 4;	.endef
	.def is_a_nop;	.val 64; 	.scl 8;	.type 158;	.size 4;	.endef
	.def hw_address;	.val 68; 	.scl 8;	.type 151;	.size 4;	.endef
	.def .eos;	.val 72;	.scl 102;	.tag object;	.size 72;	.endef
	.def OBJECT;	.scl 13;	.type 8;	.tag object;	.size 72;	.size 72;	.endef
	.def dialog;	.scl 10;	.type 8;	.size 240;	.endef
	.def keyval;	.val 0; 	.scl 8;	.type 60;	.dim 32;	.size 32;	.endef
	.def description;	.val 32; 	.scl 8;	.type 18;	.size 4;	.endef
	.def describe_fp;	.val 36; 	.scl 8;	.type 24;	.tag .61fak;	.size 36;	.size 4;	.endef
	.def is_a_nop_obj;	.val 320;	.scl 18;	.type 14;	.size 1;	.endef
	.def arg_needed;	.val 321;	.scl 18;	.type 14;	.size 1;	.endef
	.def destroy;	.val 44; 	.scl 8;	.type 144;	.size 4;	.endef
	.def describe;	.val 48; 	.scl 8;	.type 144;	.size 4;	.endef
	.def redirect;	.val 52; 	.scl 8;	.type 144;	.size 4;	.endef
	.def key;	.val 56; 	.scl 8;	.type 412;	.size 4;	.endef
	.def needs_arg;	.val 60; 	.scl 8;	.type 158;	.size 4;	.endef
	.def is_a_nop;	.val 64; 	.scl 8;	.type 158;	.size 4;	.endef
	.def hw_address;	.val 68; 	.scl 8;	.type 151;	.size 4;	.endef
	.def prompted;	.val 576;	.scl 18;	.type 14;	.size 1;	.endef
	.def default_valid;	.val 577;	.scl 18;	.type 14;	.size 1;	.endef
	.def value;	.val 80;	.scl 8;	.type 7;	.endef
	.def default_value;	.val 88;	.scl 8;	.type 7;	.endef
	.def prompt_str;	.val 96; 	.scl 8;	.type 18;	.size 4;	.endef
	.def reply_str;	.val 100; 	.scl 8;	.type 60;	.dim 128;	.size 128;	.endef
	.def prompt;	.val 228; 	.scl 8;	.type 158;	.size 4;	.endef
	.def edit;	.val 232; 	.scl 8;	.type 158;	.size 4;	.endef
	.def reset;	.val 236; 	.scl 8;	.type 144;	.size 4;	.endef
	.def .eos;	.val 240;	.scl 102;	.tag dialog;	.size 240;	.endef
	.def DIALOG;	.scl 13;	.type 8;	.tag dialog;	.size 240;	.size 240;	.endef
	.def zipcod_dlg;	.scl 10;	.type 8;	.size 248;	.endef
	.def keyval;	.val 0; 	.scl 8;	.type 60;	.dim 32;	.size 32;	.endef
	.def description;	.val 32; 	.scl 8;	.type 18;	.size 4;	.endef
	.def describe_fp;	.val 36; 	.scl 8;	.type 24;	.tag .61fak;	.size 36;	.size 4;	.endef
	.def is_a_nop_obj;	.val 320;	.scl 18;	.type 14;	.size 1;	.endef
	.def arg_needed;	.val 321;	.scl 18;	.type 14;	.size 1;	.endef
	.def destroy;	.val 44; 	.scl 8;	.type 144;	.size 4;	.endef
	.def describe;	.val 48; 	.scl 8;	.type 144;	.size 4;	.endef
	.def redirect;	.val 52; 	.scl 8;	.type 144;	.size 4;	.endef
	.def key;	.val 56; 	.scl 8;	.type 412;	.size 4;	.endef
	.def needs_arg;	.val 60; 	.scl 8;	.type 158;	.size 4;	.endef
	.def is_a_nop;	.val 64; 	.scl 8;	.type 158;	.size 4;	.endef
	.def hw_address;	.val 68; 	.scl 8;	.type 151;	.size 4;	.endef
	.def prompted;	.val 576;	.scl 18;	.type 14;	.size 1;	.endef
	.def default_valid;	.val 577;	.scl 18;	.type 14;	.size 1;	.endef
	.def value;	.val 80;	.scl 8;	.type 7;	.endef
	.def default_value;	.val 88;	.scl 8;	.type 7;	.endef
	.def prompt_str;	.val 96; 	.scl 8;	.type 18;	.size 4;	.endef
	.def reply_str;	.val 100; 	.scl 8;	.type 60;	.dim 128;	.size 128;	.endef
	.def prompt;	.val 228; 	.scl 8;	.type 158;	.size 4;	.endef
	.def edit;	.val 232; 	.scl 8;	.type 158;	.size 4;	.endef
	.def reset;	.val 236; 	.scl 8;	.type 144;	.size 4;	.endef
	.def prompt_mask;	.val 240;	.scl 8;	.type 7;	.endef
	.def .eos;	.val 248;	.scl 102;	.tag zipcod_dlg;	.size 248;	.endef
	.def ZIPCOD_DLG;	.scl 13;	.type 8;	.tag zipcod_dlg;	.size 248;	.size 248;	.endef
	.align	32
_edit:
	.def edit;	.val _edit; 	.scl 3;	.type 46;	.endef
.a1 = 80
.f1 = 336
	addu -(.a1+.f1), sp, sp
	st.l fp,(.f1-16)(sp)
	addu (.f1-16), sp, fp
	st.l r1, 4(fp)
	st.l r4, -304(fp)
	st.l r5, -300(fp)
	st.l r6, -296(fp)
	st.l r7, -292(fp)
                                                             
                                                                                
                                                                                
                                                                                
                                                                                
                                                                                
                                                                                
                                                                                
	
	
	
	
	
	
	st.l r16, 48(fp)
	st.l r17, 52(fp)
	st.l r18, 56(fp)
	mov r16, r4
	mov r18, r5
	.ln	1
	.def	.bf;	.val	.;	.scl	101;	.line	38;	.endef
// lineno: 38
	adds -256, fp, r16
	call _strcpy
	ld.l 52(fp), r17
	adds -264, fp, r7
	fst.d f0, -264(fp)
	adds -256, fp, r16
	ld.b 2(r7), r28
	adds 58, r0, r17
	andnot 0x001e, r28, r29
	or 0x0002, r29, r30
	st.b r30, 2(r7)
	call _strchr
	st.l r0, -268(fp)
	mov r16, r6
	bte 0x0000, r16, .B1502
	st.b r0, 0(r16)
// lineno: 63
	.ln	26
.B1502:	//.B0000
	adds -256, fp, r16
.DB.B15021502:	
	adds -268, fp, r19
	mov r7, r17
	mov r5, r18
	adds 1, r0, r28
	call _edit_DAC_part
	st.l r28, 0(r5)
	bte 0x0000, r6, .B1504
	ld.l r0(r5), r28
	
	bte 0x0000, r28, .B1504
	ld.l -268(fp), r29
	
	bte 0x0000, r29, .B1505
	br .B1507
	st.l r0, 0(r5)
// lineno: 69
	.ln	32
.B1505:	//.B0002
	addu 1, r6, r6
.DB.B15051505:	
	mov r6, r16
	mov r7, r17
	call _edit_drive_part
	mov r5, r18
// lineno: 0
.B1507:	//.B0003
// lineno: 70
	.ln	33
.B1504:	//.B0001
	addu 100, r4, r16
.DB.B15041504:	
	
	call _strlen
	nop
	bte 0x0000, r16, .B1508
	ld.l r0(r5), r28
	
	bte 0x0000, r28, .B1508
	ld.b 72(r4), r29
	adds 40, r0, r17
	or 0x0002, r29, r30
	st.b r30, 72(r4)
	
	fld.d -264(fp), f16
	
	
	fst.d f16, 88(r4)
	
	call _strrchr
	ld.l 96(r4), r16
	st.b r0, 0(r16)
	
	ld.l 96(r4), r16
	orh h%.S01512, r0, r31
	call _strcat
	or l%.S01512, r31, r17
	ld.l 96(r4), r16
	call _strcat
	addu 100, r4, r17
	ld.l 96(r4), r16
	orh h%.S01514, r0, r31
	call _strcat
	or l%.S01514, r31, r17
// lineno: 78
	.ln	41
.B1508:	//.B0004
	fld.d -264(fp), f8
.DB.B15081508:	
	
	
	
	call _mth_i_dfixu
	nop
// lineno: 79
	.ln	42
	
	
	
	
	.def this;	.val 48; 	.scl 9;	.type 16;	.size 4;	.endef
	.def in_str;	.val 52; 	.scl 9;	.type 28;	.size 4;	.endef
	.def str_valid;	.val 56; 	.scl 9;	.type 20;	.size 4;	.endef
	.def str;	.val -256; 	.scl 1;	.type 60;	.dim 256;	.size 256;	.endef
	.def p;	.val -272; 	.scl 1;	.type 28;	.size 4;	.endef
	.def hw_addr;	.val -264;	.scl 1;	.type 7;	.size 8;	.endef
	.def hw;	.val -276; 	.scl 1;	.type 24;	.tag .1378fak;	.size 8;	.size 4;	.endef
	.def lun_specified;	.val -268;	.scl 1;	.type 4;	.size 4;	.endef
	.ln	42
	.def	.ef;	.val	.;	.scl	101;	.line	79;	.endef
	ld.l -304(fp), r4
	ld.l -300(fp), r5
	ld.l -296(fp), r6
	ld.l -292(fp), r7
	adds .a1+16, fp, r31
	ld.l 4(fp), r1
	ld.l 0(fp), fp
	bri r1
	mov r31, sp
	.def edit;	.val .;	.scl -1;	.endef
	.globl	_edit_DAC_part
	.align	32
_edit_DAC_part:
	.def _edit_DAC_part;	.val _edit_DAC_part; 	.scl 2;	.type 36;	.endef
.a2 = 80
.f2 = 80
	addu -(.a2+.f2), sp, sp
	st.l fp,(.f2-16)(sp)
	addu (.f2-16), sp, fp
	st.l r1, 4(fp)
	st.l r4, -48(fp)
	st.l r5, -44(fp)
	st.l r6, -40(fp)
	st.l r7, -36(fp)
                                                                  
                                                                                
                                                                                
                                                                                
                                                                                
                                                                                
                                                                                
                                                                                
	
	
	
	
	
	
	st.l r16, 48(fp)
	st.l r17, 52(fp)
	st.l r18, 56(fp)
	st.l r19, 60(fp)
	mov r18, r6
	mov r17, r7
	.ln	1
	.def	.bf;	.val	.;	.scl	101;	.line	82;	.endef
// lineno: 82
	orh h%.S01527, r0, r31
	call _debug
	or l%.S01527, r31, r16
	adds 109, r0, r28
	st.b r28, -8(fp)
	adds 99, r0, r29
	st.b r29, -7(fp)
	adds 98, r0, r30
	st.b r30, -6(fp)
	adds 112, r0, r16
	st.b r16, -5(fp)
	adds 108, r0, r17
	st.b r17, -4(fp)
	
	ld.l 48(fp), r5
	st.b r0, -3(fp)
// lineno: 103
	.ln	22
.B1538:	//.B0006
	ld.b r0(r5), r28
.DB.B15381538:	
	
	and 0xff, r28, r28
	bte 0x0000, r28, .B1539
	adds -8, fp, r17
	mov r5, r16
	st.b r28, -9(fp)
	call _strcspn
	addu 1, r5, r5
	st.l r16, -16(fp)
	btne 0x0000, r16, .B1542
	ld.b r0(r5), r16
	
	call _isdigit
	and 0xff, r16, r16
	btne 0x0000, r16, .B1545
	adds 1, r0, r16
	adds 48, r0, r17
	call _error
	st.l r0, 0(r6)
	br .B1708
	mov r0, r16
// lineno: 114
	.ln	33
.B1545:	//.B0011
	mov r0, r4
.DB.B15451545:	
// lineno: 115
	.ln	34
.B1552:	//.B0015
	ld.b r0(r5), r16
.DB.B15521552:	
	
	call _isdigit
	and 0xff, r16, r16
	bte 0x0000, r16, .B1553
	adds 10, r0, r28
	ixfr r28, f16
	ixfr r4, f18
	
	ld.b r0(r5), r30
	fmlow.dd f18, f16, f20
	and 0xff, r30, r30
	adds -48, r30, r17
	addu 1, r5, r5
	fxfr f20, r29
	
	br .B1552
	adds r29, r17, r4
// lineno: 116
	.ln	35
.B1553:	//.B0016
	ld.b -9(fp), r28
.DB.B15531553:	
	addu -1, r5, r5
	and 0xff, r28, r28
	
	subs 108, r28, r0
	st.l r28, -20(fp)
	bc .B1713
	xor 0x0062, r28, r0
	bc .B1572
	xor 0x0063, r28, r0
	bc .B1565
	xor 0x006c, r28, r0
	bc .B1585
	br .B1564
	nop
// lineno: 119
	.ln	38
.B1713:	//.B0067
	ld.l -20(fp), r28
.DB.B17131713:	
	
	xor 0x006d, r28, r0
	bc .B1557
	xor 0x0070, r28, r0
	bc .B1579
	br .B1564
	nop
// lineno: 122
	.ln	41
.B1557:	//.B0018
	orh 0x0001, r0, r28
.DB.B15571557:	
	subu r28, r4, r0
	
	bnc .B1559
	br .B1560
	st.l r4, 4(r7)
// lineno: 125
	.ln	44
.B1559:	//.B0019
	adds 2, r0, r16
.DB.B15591559:	
	call _error
	adds 51, r0, r17
	st.l r0, 0(r6)
// lineno: 129
	.ln	48
.B1560:	//.B0020
	br .B1564
	nop
// lineno: 132
	.ln	51
.B1565:	//.B0023
	adds 15, r0, r28
.DB.B15651565:	
	subu r28, r4, r0
	
	bnc .B1567
	ld.b r0(r7), r29
	and 0x000f, r4, r16
	andnot 0x0078, r29, r30
	shl 3, r16, r17
	or r30, r17, r18
	br .B1568
	st.b r18, 0(r7)
// lineno: 135
	.ln	54
.B1567:	//.B0024
	adds 2, r0, r16
.DB.B15671567:	
	call _error
	adds 52, r0, r17
	st.l r0, 0(r6)
// lineno: 138
	.ln	57
.B1568:	//.B0025
	br .B1564
	nop
// lineno: 141
	.ln	60
.B1572:	//.B0027
	adds 7, r0, r28
.DB.B15721572:	
	subu r28, r4, r0
	
	bnc .B1574
	ld.s r0(r7), r29
	and 0x0007, r4, r16
	andnot 0x0380, r29, r30
	shl 7, r16, r17
	or r30, r17, r18
	br .B1575
	st.s r18, 0(r7)
// lineno: 144
	.ln	63
.B1574:	//.B0028
	adds 2, r0, r16
.DB.B15741574:	
	call _error
	adds 53, r0, r17
	st.l r0, 0(r6)
// lineno: 147
	.ln	66
.B1575:	//.B0029
	br .B1564
	nop
// lineno: 150
	.ln	69
.B1579:	//.B0031
	adds 15, r0, r28
.DB.B15791579:	
	subu r28, r4, r0
	
	bnc .B1580
	ld.b 1(r7), r29
	and 0x000f, r4, r16
	andnot 0x003c, r29, r30
	shl 2, r16, r17
	or r30, r17, r18
	br .B1581
	st.b r18, 1(r7)
// lineno: 153
	.ln	72
.B1580:	//.B0032
	adds 2, r0, r16
.DB.B15801580:	
	call _error
	adds 54, r0, r17
	st.l r0, 0(r6)
// lineno: 156
	.ln	75
.B1581:	//.B0033
	br .B1564
	nop
// lineno: 159
	.ln	78
.B1585:	//.B0035
	adds 7, r0, r28
.DB.B15851585:	
	subu r28, r4, r0
	
	bnc .B1586
	ld.l 60(fp), r29
	adds 1, r0, r30
	st.l r30, 0(r29)
	and 0x0007, r4, r19
	ld.l r0(r7), r16
	or (-114689&0xffff), r0, r17
	orh ((-114689>>16)&0xffff), r17, r17
	and r17, r16, r18
	shl 14, r19, r20
	or r18, r20, r21
	br .B1589
	st.l r21, 0(r7)
// lineno: 164
	.ln	83
.B1586:	//.B0036
	adds 2, r0, r16
.DB.B15861586:	
	call _error
	adds 55, r0, r17
	st.l r0, 0(r6)
// lineno: 167
	.ln	86
.B1589:	//.B0038
// lineno: 169
	.ln	88
.B1564:	//.B0022
	br .B1593
	nop
// lineno: 171
	.ln	90
.B1542:	//.B0009
	adds 1, r0, r16
.DB.B15421542:	
	adds 48, r0, r17
	call _error
	st.l r0, 0(r6)
	br .B1708
	mov r0, r16
// lineno: 175
	.ln	94
.B1593:	//.B0040
	br .B1538
	addu 1, r5, r5
// lineno: 176
	.ln	95
.B1539:	//.B0007
// lineno: 0
.B1708:	//.R0001
	
	
	
	
	.def str;	.val 48; 	.scl 9;	.type 28;	.size 4;	.endef
	.def hw;	.val 52; 	.scl 9;	.type 24;	.tag .1378fak;	.size 8;	.size 4;	.endef
	.def str_valid;	.val 56; 	.scl 9;	.type 20;	.size 4;	.endef
	.def lun_specified;	.val 60; 	.scl 9;	.type 20;	.size 4;	.endef
	.def i;	.val -24;	.scl 1;	.type 4;	.size 4;	.endef
	.def value;	.val -28;	.scl 1;	.type 14;	.size 4;	.endef
	.def p;	.val -32; 	.scl 1;	.type 28;	.size 4;	.endef
	.def s;	.val -8; 	.scl 1;	.type 60;	.dim 8;	.size 8;	.endef
	.def hw_designator;	.val -9;	.scl 1;	.type 12;	.size 1;	.endef
	.def cmp;	.val -16;	.scl 1;	.type 14;	.size 4;	.endef
	.ln	95
	.def	.ef;	.val	.;	.scl	101;	.line	176;	.endef
	ld.l -48(fp), r4
	ld.l -44(fp), r5
	ld.l -40(fp), r6
	ld.l -36(fp), r7
	adds .a2+16, fp, r31
	ld.l 4(fp), r1
	ld.l 0(fp), fp
	bri r1
	mov r31, sp
	.def _edit_DAC_part;	.val .;	.scl -1;	.endef
	.globl	_edit_drive_part
	.align	32
_edit_drive_part:
	.def _edit_drive_part;	.val _edit_drive_part; 	.scl 2;	.type 36;	.endef
.a3 = 80
.f3 = 80
	addu -(.a3+.f3), sp, sp
	st.l fp,(.f3-16)(sp)
	addu (.f3-16), sp, fp
	st.l r1, 4(fp)
	st.l r4, -48(fp)
	st.l r5, -44(fp)
	st.l r6, -40(fp)
   
                                                                                
                                                                                
                                                                                
                                                                                
                                                                                
                                                                                
                                                                                
                                                                                
	
	
	
	
	
	
	st.l r16, 48(fp)
	st.l r17, 52(fp)
	st.l r18, 56(fp)
	mov r18, r6
	.ln	1
	.def	.bf;	.val	.;	.scl	101;	.line	179;	.endef
// lineno: 179
	orh h%.S01605, r0, r31
	call _debug
	or l%.S01605, r31, r16
	adds 98, r0, r28
	st.b r28, -8(fp)
	adds 112, r0, r29
	st.b r29, -7(fp)
	
	ld.l 48(fp), r5
	st.b r0, -6(fp)
// lineno: 191
	.ln	13
.B1607:	//.B0042
	ld.b r0(r5), r28
.DB.B16071607:	
	
	and 0xff, r28, r28
	bte 0x0000, r28, .B1608
	adds -8, fp, r17
	mov r5, r16
	st.b r28, -9(fp)
	call _strcspn
	addu 1, r5, r5
	st.l r16, -16(fp)
	btne 0x0000, r16, .B1611
	ld.b r0(r5), r16
	
	call _isdigit
	and 0xff, r16, r16
	btne 0x0000, r16, .B1614
	adds 1, r0, r16
	adds 48, r0, r17
	call _error
	st.l r0, 0(r6)
	br .B1731
	mov r0, r16
// lineno: 202
	.ln	24
.B1614:	//.B0047
	mov r0, r4
.DB.B16141614:	
// lineno: 203
	.ln	25
.B1620:	//.B0051
	ld.b r0(r5), r16
.DB.B16201620:	
	
	call _isdigit
	and 0xff, r16, r16
	bte 0x0000, r16, .B1621
	adds 10, r0, r28
	ixfr r28, f16
	ixfr r4, f18
	
	ld.b r0(r5), r30
	fmlow.dd f18, f16, f20
	and 0xff, r30, r30
	adds -48, r30, r17
	addu 1, r5, r5
	fxfr f20, r29
	
	br .B1620
	adds r29, r17, r4
// lineno: 204
	.ln	26
.B1621:	//.B0052
	ld.b -9(fp), r28
.DB.B16211621:	
	addu -1, r5, r5
	and 0xff, r28, r28
	xor 0x0062, r28, r0
	bc .B1624
	xor 0x0070, r28, r0
	bc .B1631
	br .B1630
	nop
// lineno: 210
	.ln	32
.B1624:	//.B0054
	adds 1, r0, r28
.DB.B16241624:	
	subs r4, r28, r0
	
	bc .B1625
	subs 15, r4, r0
	
	bc .B1625
	ld.l 52(fp), r29
	and 0x000f, r4, r17
	ld.b 2(r29), r30
	shl 1, r17, r18
	andnot 0x001e, r30, r16
	or r16, r18, r19
	br .B1626
	st.b r19, 2(r29)
// lineno: 213
	.ln	35
.B1625:	//.B0055
	adds 2, r0, r16
.DB.B16251625:	
	call _error
	adds 56, r0, r17
	st.l r0, 0(r6)
// lineno: 216
	.ln	38
.B1626:	//.B0056
	br .B1630
	nop
// lineno: 219
	.ln	41
.B1631:	//.B0059
	adds 0, r4, r0
.DB.B16311631:	
	
	bc .B1632
	subs 15, r4, r0
	
	bc .B1632
	ld.l 52(fp), r28
	and 0x000f, r4, r16
	ld.s 2(r28), r29
	shl 5, r16, r17
	andnot 0x01e0, r29, r30
	or r30, r17, r18
	br .B1633
	st.s r18, 2(r28)
// lineno: 222
	.ln	44
.B1632:	//.B0060
	adds 2, r0, r16
.DB.B16321632:	
	call _error
	adds 57, r0, r17
	st.l r0, 0(r6)
// lineno: 225
	.ln	47
.B1633:	//.B0061
// lineno: 227
	.ln	49
.B1630:	//.B0058
	br .B1637
	nop
// lineno: 229
	.ln	51
.B1611:	//.B0045
	adds 1, r0, r16
.DB.B16111611:	
	adds 48, r0, r17
	call _error
	st.l r0, 0(r6)
	br .B1731
	mov r0, r16
// lineno: 233
	.ln	55
.B1637:	//.B0063
	br .B1607
	addu 1, r5, r5
// lineno: 234
	.ln	56
.B1608:	//.B0043
// lineno: 0
.B1731:	//.R0002
	
	
	
	
	.def str;	.val 48; 	.scl 9;	.type 28;	.size 4;	.endef
	.def hw;	.val 52; 	.scl 9;	.type 24;	.tag .1378fak;	.size 8;	.size 4;	.endef
	.def str_valid;	.val 56; 	.scl 9;	.type 20;	.size 4;	.endef
	.def i;	.val -20;	.scl 1;	.type 4;	.size 4;	.endef
	.def value;	.val -24;	.scl 1;	.type 4;	.size 4;	.endef
	.def p;	.val -28; 	.scl 1;	.type 28;	.size 4;	.endef
	.def s;	.val -8; 	.scl 1;	.type 60;	.dim 8;	.size 8;	.endef
	.def hw_designator;	.val -9;	.scl 1;	.type 12;	.size 1;	.endef
	.def cmp;	.val -16;	.scl 1;	.type 14;	.size 4;	.endef
	.ln	56
	.def	.ef;	.val	.;	.scl	101;	.line	234;	.endef
	ld.l -48(fp), r4
	ld.l -44(fp), r5
	ld.l -40(fp), r6
	adds .a3+16, fp, r31
	ld.l 4(fp), r1
	ld.l 0(fp), fp
	bri r1
	mov r31, sp
	.def _edit_drive_part;	.val .;	.scl -1;	.endef
	.globl	_destroy_ZIPCOD_DLG
	.align	32
_destroy_ZIPCOD_DLG:
	.def _destroy_ZIPCOD_DLG;	.val _destroy_ZIPCOD_DLG; 	.scl 2;	.type 32;	.endef
.a4 = 80
.f4 = 32
	addu -(.a4+.f4), sp, sp
	st.l fp,(.f4-16)(sp)
	addu (.f4-16), sp, fp
	st.l r1, 4(fp)
	st.l r4, -8(fp)
                                        
                                                                                
                                                                                
                                                                                
                                                                                
                                                                                
                                                                                
                                                                                
                                                                                
	
	
	
	
	
	
	st.l r16, 48(fp)
	mov r16, r4
	.ln	1
	.def	.bf;	.val	.;	.scl	101;	.line	238;	.endef
// lineno: 238
	orh h%.S01641, r0, r31
	or l%.S01641, r31, r16
	
	call _debug
	mov r4, r17
	call _free
	ld.l 96(r4), r16
	call _destroy_DIALOG
	mov r4, r16
	orh h%.S01643, r0, r31
	call _debug
	or l%.S01643, r31, r16
// lineno: 244
	.ln	7
	
	
	
	
	.def this;	.val 48; 	.scl 9;	.type 16;	.size 4;	.endef
	.ln	7
	.def	.ef;	.val	.;	.scl	101;	.line	244;	.endef
	ld.l -8(fp), r4
	adds .a4+16, fp, r31
	ld.l 4(fp), r1
	ld.l 0(fp), fp
	bri r1
	mov r31, sp
	.def _destroy_ZIPCOD_DLG;	.val .;	.scl -1;	.endef
	.globl	_new_ZIPCOD_DLG
	.align	32
_new_ZIPCOD_DLG:
	.def _new_ZIPCOD_DLG;	.val _new_ZIPCOD_DLG; 	.scl 2;	.type 104;	.tag zipcod_dlg;	.size 248;	.endef
.a5 = 80
.f5 = 96
	addu -(.a5+.f5), sp, sp
	st.l fp,(.f5-16)(sp)
	addu (.f5-16), sp, fp
	st.l r1, 4(fp)
	st.l r4, -64(fp)
	st.l r5, -60(fp)
                     
                                                                                
                                                                                
                                                                                
                                                                                
                                                                                
                                                                                
                                                                                
                                                                                
	
	
	
	
	
	
	fst.d f8, 16(fp)
	.ln	1
	.def	.bf;	.val	.;	.scl	101;	.line	250;	.endef
// lineno: 250
	orh h%.S01651, r0, r31
	call _debug
	or l%.S01651, r31, r16
	call _new_DIALOG
	nop
	mov r16, r28
	adds 248, r0, r16
	call _zalloc
	mov r28, r5
	adds 240, r0, r18
	mov r5, r17
	call _memcpy
	mov r16, r4
	call _free
	mov r5, r16
	orh h%.S01655, r0, r31
	or l%.S01655, r31, r17
	call _strcpy
	mov r4, r16
	orh h%.S01657, r0, r31
	or l%.S01657, r31, r28
	st.l r28, 32(r4)
	call _zalloc
	adds 512, r0, r16
	st.l r16, 96(r4)
	orh ha%_Interactive, r0, r31
	ld.l l%_Interactive(r31), r28
	
	btne 0x0000, r28, .B1660
	ld.b 72(r4), r29
	
	andnot 0x0002, r29, r30
	br .B1661
	st.b r30, 72(r4)
// lineno: 268
	.ln	19
.B1660:	//.B0065
	ld.b 72(r4), r28
.DB.B16601660:	
	
	or 0x0002, r28, r29
	st.b r29, 72(r4)
// lineno: 269
	.ln	20
.B1661:	//.B0066
	orh ha%.C01751, r0, r31
.DB.B16611661:	
	fld.d l%.C01751(r31), f16
	ld.l 68(r4), r28
	fiadd.dd f0, f0, f8
	mov r28, r29
	ixfr r29, f18
	fiadd.ss f17, f0, f19
	adds -48, fp, r16
	fsub.dd f18, f16, f20
	adds 112, r0, r30
	st.l r30, -8(fp)
	call _convert_zip
	fst.d f20, 88(r4)
	mov r16, r28
	ld.l 96(r4), r16
	orh h%.S01662, r0, r31
	or l%.S01662, r31, r17
	adds 109, r0, r18
	adds 99, r0, r19
	adds 98, r0, r20
	adds 112, r0, r21
	adds 108, r0, r22
	st.l r28, -4(fp)
	mov r18, r23
	mov r19, r24
	mov r20, r25
	adds -8, fp, r29
	mov r21, r26
	mov r20, r27
	call _sprintf
	mov r29, r28
	ld.b 40(r4), r28
	fld.d 16(fp), f16
	or 0x0002, r28, r29
	st.b r29, 40(r4)
	fst.d f16, 240(r4)
	orh h%.S01664, r0, r31
	or l%.S01664, r31, r16
	orh h%_edit, r0, r31
	or l%_edit, r31, r30
	st.l r30, 232(r4)
	orh h%_destroy_ZIPCOD_DLG, r0, r31
	or l%_destroy_ZIPCOD_DLG, r31, r17
	call _debug
	st.l r17, 44(r4)
	mov r4, r16
// lineno: 277
	.ln	28
	
	
	
	
	.def prompt_mask;	.val 16;	.scl 9;	.type 7;	.size 8;	.endef
	.def czbuf;	.val -48; 	.scl 1;	.type 50;	.dim 32;	.size 32;	.endef
	.def s;	.val -52; 	.scl 1;	.type 24;	.tag dialog;	.size 240;	.size 4;	.endef
	.def this;	.val -56; 	.scl 1;	.type 24;	.tag zipcod_dlg;	.size 248;	.size 4;	.endef
	.ln	28
	.def	.ef;	.val	.;	.scl	101;	.line	277;	.endef
	ld.l -64(fp), r4
	ld.l -60(fp), r5
	adds .a5+16, fp, r31
	ld.l 4(fp), r1
	ld.l 0(fp), fp
	bri r1
	mov r31, sp
	.def _new_ZIPCOD_DLG;	.val .;	.scl -1;	.endef
	.data
	.align	8
.C01751:	// (0)
	.long	0x0, 0x43300000	// 4.50359962737049600E+15
	.data
	.align	1
.S01512:	//
// "("
	.byte	0x28,0x00
.S01514:	//
// "):"
	.byte	0x29,0x3a,0x00
	.align	16
.S01527:	//
// "editing zipcode\n"
	.byte	0x65,0x64,0x69,0x74,0x69,0x6e,0x67,0x20,0x7a,0x69,0x70
	.byte	0x63,0x6f,0x64,0x65,0x0a,0x00
	.align	16
.S01605:	//
// "editing zipcode\n"
	.byte	0x65,0x64,0x69,0x74,0x69,0x6e,0x67,0x20,0x7a,0x69,0x70
	.byte	0x63,0x6f,0x64,0x65,0x0a,0x00
	.align	16
.S01641:	//
// "entered destroy_ZIPCOD_DLG, this = %x\n"
	.byte	0x65,0x6e,0x74,0x65,0x72,0x65,0x64,0x20,0x64,0x65,0x73
	.byte	0x74,0x72,0x6f,0x79,0x5f,0x5a,0x49,0x50,0x43,0x4f,0x44
	.byte	0x5f,0x44,0x4c,0x47,0x2c,0x20,0x74,0x68,0x69,0x73,0x20
	.byte	0x3d,0x20,0x25,0x78,0x0a,0x00
	.align	16
.S01643:	//
// "leaving destroy_ZIPCOD_DLG\n"
	.byte	0x6c,0x65,0x61,0x76,0x69,0x6e,0x67,0x20,0x64,0x65,0x73
	.byte	0x74,0x72,0x6f,0x79,0x5f,0x5a,0x49,0x50,0x43,0x4f,0x44
	.byte	0x5f,0x44,0x4c,0x47,0x0a,0x00
	.align	16
.S01651:	//
// "entered new_ZIPCOD_DLG\n"
	.byte	0x65,0x6e,0x74,0x65,0x72,0x65,0x64,0x20,0x6e,0x65,0x77
	.byte	0x5f,0x5a,0x49,0x50,0x43,0x4f,0x44,0x5f,0x44,0x4c,0x47
	.byte	0x0a,0x00
.S01655:	//
// "z"
	.byte	0x7a,0x00
	.align	16
.S01657:	//
// "device hardware address"
	.byte	0x64,0x65,0x76,0x69,0x63,0x65,0x20,0x68,0x61,0x72,0x64
	.byte	0x77,0x61,0x72,0x65,0x20,0x61,0x64,0x64,0x72,0x65,0x73
	.byte	0x73,0x00
	.align	16
.S01662:	//
// "\nA value for the device hardware address is required.  It should be\nof the form %cX%cX%cX%"
	.byte	0x0a,0x41,0x20,0x76,0x61,0x6c,0x75,0x65,0x20,0x66,0x6f
	.byte	0x72,0x20,0x74,0x68,0x65,0x20,0x64,0x65,0x76,0x69,0x63
	.byte	0x65,0x20,0x68,0x61,0x72,0x64,0x77,0x61,0x72,0x65,0x20
	.byte	0x61,0x64,0x64,0x72,0x65,0x73,0x73,0x20,0x69,0x73,0x20
	.byte	0x72,0x65,0x71,0x75,0x69,0x72,0x65,0x64,0x2e,0x20,0x20
	.byte	0x49,0x74,0x20,0x73,0x68,0x6f,0x75,0x6c,0x64,0x20,0x62
	.byte	0x65,0x0a,0x6f,0x66,0x20,0x74,0x68,0x65,0x20,0x66,0x6f
	.byte	0x72,0x6d,0x20,0x25,0x63,0x58,0x25,0x63,0x58,0x25,0x63
	.byte	0x58,0x25,0x63,0x58,0x25,0x63,0x58,0x20,0x6f,0x72,0x20
	.byte	0x25,0x63,0x58,0x25,0x63,0x58,0x25,0x63,0x58,0x25,0x63
	.byte	0x58,0x3a,0x25,0x63,0x58,0x25,0x63,0x58,0x2c,0x20,0x77
	.byte	0x68,0x65,0x72,0x65,0x20,0x6d,0x2c,0x20,0x63,0x2c,0x20
	.byte	0x62,0x2c,0x20,0x70,0x2c,0x20,0x61,0x6e,0x64,0x20,0x6c
	.byte	0x0a,0x72,0x65,0x70,0x72,0x65,0x73,0x65,0x6e,0x74,0x20
	.byte	0x64,0x65,0x6c,0x69,0x6d,0x69,0x74,0x65,0x72,0x73,0x20
	.byte	0x61,0x6e,0x64,0x20,0x65,0x61,0x63,0x68,0x20,0x58,0x20
	.byte	0x72,0x65,0x70,0x72,0x65,0x73,0x65,0x6e,0x74,0x73,0x20
	.byte	0x61,0x20,0x64,0x65,0x63,0x69,0x6d,0x61,0x6c,0x20,0x76
	.byte	0x61,0x6c,0x75,0x65,0x2e,0x20,0x20,0x49,0x66,0x20,0x58
	.byte	0x20,0x3d,0x20,0x30,0x2c,0x0a,0x74,0x68,0x65,0x6e,0x20
	.byte	0x62,0x6f,0x74,0x68,0x20,0x74,0x68,0x65,0x20,0x64,0x65
	.byte	0x6c,0x69,0x6d,0x69,0x74,0x65,0x72,0x20,0x61,0x6e,0x64
	.byte	0x20,0x58,0x20,0x76,0x61,0x6c,0x75,0x65,0x20,0x61,0x72
	.byte	0x65,0x20,0x6f,0x6d,0x69,0x74,0x74,0x65,0x64,0x2e,0x0a
	.byte	0x0a,0x50,0x6c,0x65,0x61,0x73,0x65,0x20,0x65,0x6e,0x74
	.byte	0x65,0x72,0x20,0x61,0x20,0x76,0x61,0x6c,0x75,0x65,0x20
	.byte	0x66,0x6f,0x72,0x20,0x74,0x68,0x65,0x20,0x64,0x65,0x76
	.byte	0x69,0x63,0x65,0x20,0x68,0x61,0x72,0x64,0x77,0x61,0x72
	.byte	0x65,0x20,0x61,0x64,0x64,0x72,0x65,0x73,0x73,0x20,0x28
	.byte	0x25,0x73,0x29,0x3a,0x00
	.align	16
.S01664:	//
// "leaving new_ZIPCOD_DLG\n"
	.byte	0x6c,0x65,0x61,0x76,0x69,0x6e,0x67,0x20,0x6e,0x65,0x77
	.byte	0x5f,0x5a,0x49,0x50,0x43,0x4f,0x44,0x5f,0x44,0x4c,0x47
	.byte	0x0a,0x00
	.data
	.comm	_SCSI_Stat_Ptr,4
	.def _SCSI_Stat_Ptr;	.val _SCSI_Stat_Ptr; 	.scl 2;	.type 24;	.tag SCSI_Device;	.size 1096;	.size 4;	.endef
	.comm	_SCSI_Tally_Ptr,4
	.def _SCSI_Tally_Ptr;	.val _SCSI_Tally_Ptr; 	.scl 2;	.type 24;	.tag SCSI_Tallies;	.size 164;	.size 4;	.endef
	.extern	_sprintf
	.extern	_convert_zip
	.extern	_memcpy
	.extern	_zalloc
	.extern	_new_DIALOG
	.extern	_destroy_DIALOG
	.extern	_free
	.extern	_error
	.extern	_isdigit
	.extern	_strcspn
	.extern	_debug
	.extern	_mth_i_dfixu
	.extern	_strcat
	.extern	_strrchr
	.extern	_strlen
	.extern	_strchr
	.extern	_strcpy
