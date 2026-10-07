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
 
/*
 *		INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *	feov_.c 11.1 94/03/22 16:50:07
 */
static char	feov__ver[] = "@(#) sourcefile feov_.c 11.1 94/03/22 16:50:07";

/*
 *
 *  Force End of Volume processing
 *
 */
feov_(int *handle)
{
  return(feov(*handle));
}
