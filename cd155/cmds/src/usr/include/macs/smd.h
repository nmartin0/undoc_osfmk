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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *
 *      Copyright 1992  Intel Corporation.
 *
 *
 * HISTORY
 *
 * Revision 1.4  1993/07/20  01:40:07  carbajal
 * global_time stuff
 *
 * Revision 1.3  1993/07/18  19:45:45  carbajal
 * changed global time to a float
 *
 * Revision 1.2  1992/10/13  23:56:56  rkl
 * Changed all uid fields from int to uid_t.
 *
 * Revision 1.1  1992/09/24  16:34:10  rkl
 * Initial revision
 *
 *
 */

#ifndef _SMD_H
#define _SMD_H

#include <sys/types.h>

/*
 *  These need to go somewhere different.
 */
typedef long    nodetype_t;

#define get_global_time()       dclock()

/*
 *  Socket interface name for SMD
 */
#define SMD_IFACE_NAME "/etc/nx/SMD"
#define SMD_IFACE_PORT 3050

/*
 *  SMD request/response header
 */
struct smd_hdr {
        int     type;
                        /*
                         *  SMD service request types
                         */
#define                 SMD_EVENT_REQ                   1
#define                 SMD_SET_APP_ALARM_REQ           2
#define                 SMD_GET_STATUS_REQ              3
#define                 SMD_CANCEL_REQ                  4
                        /*
                         *  SMD service indications and responses
                         */
#define                 SMD_APP_START_IND               20
#define                 SMD_APP_ROLLIN_IND              21
#define                 SMD_APP_ROLLOUT_IND             22
#define                 SMD_APP_END_IND                 23
#define                 SMD_APP_ALARM_IND               24
#define                 SMD_STATUS_RESP                 25
                        /*
                         *  Allocator service indications
                         */
#define                 ALLOCATOR_START_IND             30
#define                 ALLOCATOR_ROLLIN_IND            31
#define                 ALLOCATOR_ROLLOUT_IND           32
#define                 ALLOCATOR_END_IND               33
        int     len;
};

typedef struct smd_hdr smd_hdr_t;

/*
 *  SMD request/response qualifer
 */
struct smd_qualifier {
        int             pgid;           /* process group ID to monitor */
        int             part_id;        /* partition ID to monitor */
        int             acct_id;        /* application account ID */
        uid_t           uid;            /* Unix user ID */
#define         SMD_REQ_ID_LEN  16
        char            req_id[ SMD_REQ_ID_LEN ];/* user defined request ID */
};
#define QUALIFY_ANY     (-1)            /* Any value will qualify */

typedef struct smd_qualifier smd_qualifier_t;

/*
 *  SMD request parameters
 */
struct params {
        int     flags;          /* request flags */
#define                 PERSISTENT              0x0001
#define                 KEEP_AFTER_CLOSE        0x0002
#define                 SEND_SIGNAL             0x0004
        int     sig_num;        /* signal value to use for completed requests */
        pid_t   sig_pid;        /* process ID to signal */
        int     events;         /* bit field of events */
#define                 APP_START       0x0001
#define                 APP_ROLLIN      0x0002
#define                 APP_ROLLOUT     0x0004
#define                 APP_END         0x0008
        long    alarm_val;      /* node-seconds of accumulated time for alarm */
        int     service;        /* SMD service to cancel */
};

typedef struct params params_t;
/*
 *  SMD request
 */
struct smd_req {
        struct smd_hdr          hdr;            /* common header */
        struct smd_qualifier    q_id;           /* monitor qualifiers */ 
        struct params           param;          /* request parameters */
};

typedef struct smd_req smd_req_t;

/*
 *  SMD indication/response return values.
 */
struct smd_values {
        int             status;         /* response/indication status */
#define                         SMD_OK                  0
#define                         APP_TERMINATED          1
#define                         APP_NOT_FOUND           (-1)
#define                         NO_RESOURCES            (-2)
        double          global_time;    /* Paragon global clock time of event */
        time_t          event_time;     /* Unix time of event or response */
        time_t          cpu_time;       /* total application CPU seconds */
        int             app_size;       /* nodes used by application */
        int             app_priority;   /* priority of the application */
        int             part_size;      /* size of partition */
};
#define FIELD_NOT_VALID         (-1)

typedef struct smd_values smd_values_t;

/*
 *  SMD indication/response
 */
struct smd_resp {
        struct  smd_qualifier   q_id;   /* qualifying IDs associated with resp*/
        struct  smd_values      value;  /* return values associated with resp */
};

typedef struct smd_resp smd_resp_t;

/*
 *  Allocator indication data.
 */
struct app_data {
        int             pgid;           /* process group ID */
        int             acct_id;        /* application account ID */
        uid_t           uid;            /* Unix user ID */

        int             app_size;       /* size of application */
        u_long          app_rollin;     /* milliseconds this appl rolled in */
        int             app_priority;   /* priority of application */

        int             part_id;        /* partition ID */
        int             part_size;      /* partition size */
        u_long          rollin_quantum; /* partition rollin quantum */
        u_long          part_rollin;    /* milliseconds this part rolled in */

        double          global_time;    /* Paragon global clock time of event */
        time_t          event_time;     /* Unix time of event or response */
};

typedef struct app_data app_data_t;

/*
 *  Allocator indication
 */
struct  allocator_ind {
        struct smd_hdr  hdr;
        struct app_data data;
};

typedef struct allocator_ind allocator_ind_t;

#endif
