/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
C*----------------------------------------------------------------------
C*              INTEL CORPORATION PROPRIETARY INFORMATION
C*
C*  This software is supplied under the terms of a license
C*  agreement or nondisclosure agreement with Intel Corporation
C*  and may not be copied or disclosed except in accordance
C*  with the terms of that agreement.
C*
C*	ft3480.h 11.1 94/03/22 16:49:07
C*
C*----------------------------------------------------------------------*/ 

C     Data set control Block.  Used in calls to tdsopen.
      STRUCTURE /dcb/
C        REC1 fields
         CHARACTER*17   dsname    ! Data Set Name 
         INTEGER*4 dsnlen         ! Data Set Name Length
         INTEGER*4 volseqno       ! Volume Sequence Number      
         INTEGER*4 dsseqno        ! Data Set Sequence Number    
         CHARACTER expirec        ! Expiration Date - century (blank=19, 0=20, etc
         INTEGER*4  expireyy      ! Expiration Date - year (91, 92, etc) 
         INTEGER*4 expiredd       ! Expiration Date - day of year 
         CHARACTER*13 system_code ! System Code
         INTEGER*4 syscdlen       ! System Code Length
         
C        REC2 Fields
         CHARACTER recfm          ! Record Format
         INTEGER*4 blocklen       ! Block Length
         INTEGER*4  reclen        ! Record Length 
         CHARACTER blkattr        ! Block Attribute
         
C        options 
         INTEGER*4 blocknum       ! Block ID Number  
         INTEGER*4 read           ! Opened for read (write if zero) 
         INTEGER*4  buffered_mode ! Use buffering on I/O node
         INTEGER*4 block_mode     ! Read/Write Blocks at a time
         INTEGER*4 num_tapeblk_bufs
      END STRUCTURE


      INTEGER*4 INPUT_HEADER_LABEL  
      INTEGER*4 INPUT_TRAILER_LABEL
      INTEGER*4 OUTPUT_HEADER_LABEL   
      INTEGER*4 OUTPUT_TRAILER_LABEL
      INTEGER*4 EOV_USEREXIT       
      INTEGER*4 WAITING_FOR_OPEN 
      INTEGER*4 NEXT_VOLSER         
      INTEGER*4 OVERWRITE_EXPIRE      
      
      PARAMETER (INPUT_HEADER_LABEL=2)
      PARAMETER (INPUT_TRAILER_LABEL=3)
      PARAMETER (OUTPUT_HEADER_LABEL=4)
      PARAMETER (OUTPUT_TRAILER_LABEL=5)
      PARAMETER (EOV_USEREXIT=6)
      PARAMETER (WAITING_FOR_OPEN=7)
      PARAMETER (NEXT_VOLSER=8)
      PARAMETER (OVERWRITE_EXPIRE=9)
      
      
C*----------------------------------------------------------------------
C* Errors
C*----------------------------------------------------------------------*/ 
      INTEGER*4 TERR_HDRLEN_NOT80
      INTEGER*4 TERR_VOL1_NOT_FOUND
      INTEGER*4 TERR_REC1_NOT_FOUND
      INTEGER*4 TERR_INVALID_INPUT_TAPE_SCRATCH
      INTEGER*4 TERR_ALREADY_TDSOPEND
      INTEGER*4 TERR_INVALID_DSSEQNO
      INTEGER*4 TERR_BLOCKNUM_NOT_SUPPORTED
      INTEGER*4 TERR_NO_VOLSER
      INTEGER*4 TERR_COULD_NOT_RESERVE_TAPE
      INTEGER*4 TERR_INVALID_VOLUME
      INTEGER*4 TERR_EXPIRATION_DATE
      INTEGER*4 TERR_FSF_FAILED
      INTEGER*4 TERR_DSSEQNO_NOT_FOUND
      INTEGER*4 TERR_INVALID_VOLSEQNO
      INTEGER*4 TERR_MEM_ALLOC
      INTEGER*4 TERR_INVALID_DSNAME_SPECIFIED 
      INTEGER*4 TERR_INVALID_DSNAME_LEN
      INTEGER*4 TERR_INVALID_DSNAME_QUALLEN 
      INTEGER*4 TERR_INVALID_DSNAME_FRSTQUAL
      INTEGER*4 TERR_INVALID_DSNAME_CHAR 
      INTEGER*4 TERR_CANNOT_APPEND
      INTEGER*4 TERR_CWRITE_FAILED
      INTEGER*4 TERR_CLOSE_FAILED
      INTEGER*4 TERR_RELEASE_TAPE_FAILED
      INTEGER*4 TERR_BLKCNTS_DONT_MATCH 
      INTEGER*4 TERR_NO_DATASET_OPENED
      INTEGER*4 TERR_INV_HANDLE
      INTEGER*4 TERR_NOTM_USRLABELS 
      INTEGER*4 TERR_NOT_START_OF_DATASET
      INTEGER*4 TERR_MAX_VOLS_EXCEEDED
      INTEGER*4 TERR_NO_NEXT_VOLSER
      INTEGER*4 TERR_VOLSEQNO_ORDER
      INTEGER*4 TERR_INVALID_BLOCKNUM
      INTEGER*4 TERR_BLOCKNUM_ON_WRITE
      INTEGER*4 TINFO_EOF_ENCOUNTERED
      
      PARAMETER(TERR_HDRLEN_NOT80=-3)
      PARAMETER(TERR_VOL1_NOT_FOUND=-4)
      PARAMETER(TERR_REC1_NOT_FOUND=-5)
      PARAMETER(TERR_INVALID_INPUT_TAPE_SCRATCH=-6)
      PARAMETER(TERR_ALREADY_TDSOPEND=-7)
      PARAMETER(TERR_INVALID_DSSEQNO=-8)
      PARAMETER(TERR_BLOCKNUM_NOT_SUPPORTED=-9)
      PARAMETER(TERR_NO_VOLSER=-10)
      PARAMETER(TERR_COULD_NOT_RESERVE_TAPE=-11)
      PARAMETER(TERR_INVALID_VOLUME=-12)
      PARAMETER(TERR_EXPIRATION_DATE=-13)
      PARAMETER(TERR_FSF_FAILED=-14)
      PARAMETER(TERR_DSSEQNO_NOT_FOUND=-15)
      PARAMETER(TERR_INVALID_VOLSEQNO=-16)
      PARAMETER(TERR_MEM_ALLOC=-17)
      PARAMETER(TERR_INVALID_DSNAME_SPECIFIED=-18)
      PARAMETER(TERR_INVALID_DSNAME_LEN=-19)
      PARAMETER(TERR_INVALID_DSNAME_QUALLEN=-20)
      PARAMETER(TERR_INVALID_DSNAME_FRSTQUAL=-21)
      PARAMETER(TERR_INVALID_DSNAME_CHAR=-22)
      PARAMETER(TERR_CANNOT_APPEND=-23)
      PARAMETER(TERR_CWRITE_FAILED=-24)
      PARAMETER(TERR_CLOSE_FAILED=-25)
      PARAMETER(TERR_RELEASE_TAPE_FAILED=-26)
      PARAMETER(TERR_BLKCNTS_DONT_MATCH=-27)
      PARAMETER(TERR_NO_DATASET_OPENED=-28)
      PARAMETER(TERR_INV_HANDLE=-29)
      PARAMETER(TERR_NOTM_USRLABELS=-30)
      PARAMETER(TERR_NOT_START_OF_DATASET=-31)
      PARAMETER(TERR_MAX_VOLS_EXCEEDED=-32)
      PARAMETER(TERR_NO_NEXT_VOLSER=-33)
      PARAMETER(TERR_VOLSEQNO_ORDER=-34)
      PARAMETER(TERR_INVALID_BLOCKNUM=-35)
      PARAMETER(TERR_BLOCKNUM_ON_WRITE=-36)
      PARAMETER(TINFO_EOF_ENCOUNTERED=-37)

