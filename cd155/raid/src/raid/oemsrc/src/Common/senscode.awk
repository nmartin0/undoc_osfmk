# $Copyright
# Copyright 1992, 1993, 1994, 1995 Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
BEGIN {
	TRUE = 1
	FALSE = 0
	#  make sure one of the following lines is commented out
	#debug = TRUE
	debug = FALSE

	#  set up a table of sense key names found in
	#  /usr/include/sys/scsi/SCSI.h
	sense_name["0x00"] = "Key_No_Sense"
	sense_name["0x01"] = "Key_Recovered_Error"
	sense_name["0x02"] = "Key_Not_Ready"
	sense_name["0x03"] = "Key_Medium_Error"
	sense_name["0x04"] = "Key_Hardware_Error"
	sense_name["0x05"] = "Key_Illegal_Request"
	sense_name["0x06"] = "Key_Unit_Attention"
	sense_name["0x07"] = "Key_Data_Protect"
	sense_name["0x08"] = "Key_Blank_Check"
	sense_name["0x09"] = "Key_Vendor_Specific"
	sense_name["0x0a"] = "Key_Copy_Aborted"
	sense_name["0x0b"] = "Key_Aborted_Command"
	sense_name["0x0c"] = "Key_Equal"
	sense_name["0x0d"] = "Key_Volume_Overflow"
	sense_name["0x0e"] = "Key_Miscompare"
	sense_name["0x0f"] = "Key_Reserved"

	#  read and discard all lines up to and including the one that begins
	#  with our "magic token" which is [arbitrarily] "#--"
	headers_gone = FALSE
	while ( ! headers_gone ) {
	  getline
	  if ( debug )
	    printf( "header_line[$1] = :%s:\n", $1 )
	  if ( $1 == "#--" )
	    headers_gone = TRUE
	}
	output_count = 0
	input_count = 0
	printf( "\nstat_tbl_entry_t stat_tbl[] = {\n" )
}

{
	if ( debug )
	  printf( "DBG: %s\n", $0 )
	ASC = $1
	ASCQ = $2
	MPSK = $3 ""			# coerce MPSK to ascii
	#  ASC, ASCQ, & MPSK are saved
	#  Set DESCRIP to entire line
	DESCRIP = $0
	#  Now remove ASC, ASCQ, & MPSK from DESCRIP, and substitute a
	#  C language "escaped tab" char for the literal tab char
	#  tab char
	gsub( /^.*\t/, "\\t", DESCRIP )

#	DESCRIP = "\\t"
#	for ( i = 4; i <= NF; i++)
#	  if ( i != NF )
#	    DESCRIP = DESCRIP $i " "
#	  else
#	    DESCRIP = DESCRIP $i

	#  Get next input line
	getline
	#  Check for continuation of the DESCRIP text; keep concat'ing to
	#  the DESCRIP text as long as line begins with 2 tab chars
	while ( $0 ~ /^\t\t/ ) {
	  if ( debug )
	    printf( "DBG(while_nl1): %s\n", $0 )
	  gsub( /\t/, "" )
	  if ( debug )
	    printf( "DBG(while_nl2): %s\n", $0 )
	  DESCRIP = DESCRIP "\\n\\t" $0
	  getline
	}
	FUNCNAME = $1

	#  OK, input has been gathered, construct output
	if ( (num_mpskeys = split( MPSK, mpsk_arr, ",")) == 0 ) {
	  mpsk_arr[1] = MPSK
	  num_mpskeys = 1
	}
	#  grrr...  What's the best way to convert [ascii] HEX input to hex?
	for ( idx in mpsk_arr ) {
	  if ( mpsk_arr[idx] ~ /[abcdefABCDEF]/ ) {
	    if ( debug )
	      printf( "DBG: (got_alpha_in_mpsk_arr) mpsk_arr[%d]=%s\n", idx, mpsk_arr[idx] )
	    mpsk_arr[idx] = 10 + 0		# set to 10 by default
	    if ( mpsk_arr[idx] ~ /[bB]/ )
	      mpsk_arr[idx] = 11 + 0
	    if ( mpsk_arr[idx] ~ /[cC]/ )
	      mpsk_arr[idx] = 12 + 0
	    if ( mpsk_arr[idx] ~ /[dD]/ )
	      mpsk_arr[idx] = 13 + 0
	    if ( mpsk_arr[idx] ~ /[eE]/ )
	      mpsk_arr[idx] = 14 + 0
	    if ( mpsk_arr[idx] ~ /[fF]/ )
	      mpsk_arr[idx] = 15 + 0
	  }
	  if ( debug )
	    printf( "DBG: mpsk_arr[%d] = %s\n", idx, mpsk_arr[idx] )
	}
	for ( i = 1; i <= num_mpskeys; i++ ) {
	  printf( "  {\n" )
	  mpsk_hex = sprintf( "0x%02x", mpsk_arr[i])
	  if ( debug )
	    printf( "DBG: mpsk_hex = %s\n", mpsk_hex )
	  printf( "    %s, 0x%s, 0x%s,\n", sense_name[mpsk_hex], ASC, ASCQ )
	  printf( "    \"%s\",\n", DESCRIP )
	  printf( "    %s\n", FUNCNAME )
	  printf( "  },\n" )
	  output_count++
	}
	#  Now pull in the empty line between records
	getline
	if ( debug )
	  printf( "DBG(this_better_be_empty): '%s'\n", $0 )
	input_count++
}

END {
	printf( "};\n\n" )
	errout = sprintf( "echo 'Total of %d SK/ASC/ASCQ triplets generated from %d input records' 1>&2",
	    output_count, input_count )
	system( errout )
}
