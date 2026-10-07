#if 0
This header comment is being turned into a preprocessor comment (#if 0)
so the CVS comments won't break the C comment.  CVS Rev 1.11 comment 
broke the C comment.

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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/bin/sat/sat.c,v 1.12 1994/12/23 00:07:48 scotth Exp $
 *
 *      $Log: sat.c,v $
 * Revision 1.12  1994/12/23  00:07:48  scotth
 *  Reviewer: Ellen D.
 *  Risk: low
 *  Problem:  When the CVS comment from revision 1.11 was included in the
 *    C comment by the $Log$ keyword, the comments interacted and the
 *    compile broke.
 *  Fix:  Use ccp to shield the comment (#if 0 ... #endif)
 *  Benefit or PTS #: sat.c will compile
 *  Testing: tried it.  It compiled.
 *  Module(s): sat.c
 *
 * Revision 1.11  1994/12/15  19:32:33  scotth
 *  Reviewer: Adam Munhall, Scott Huddleston, Jeff Earickson
 *  Risk: Medium a priori, Low after testing
 *  Benefit or PTS #: 11219
 *  Testing: Oak Ridge MCAT, several weeks SAT testing, SAT EAT
 *  Module(s):
 *    cmds_libs/src/usr/bin/sat/sat.c
 *    cmds_libs/src/usr/lib/sat/*/*/run (15 instances)
 *    cmds_libs/src/usr/lib/sat/*/*/*/run (4 instances)
 *
 * Revision 1.9  1994/07/07  16:26:32  adam
 * Transcript banner and final report banner now show true size of partition when
 * -p used instead of size of .compute.
 *
 *  Reviewer: killops
 *  Risk: low
 *  Benefit or PTS #: 4348
 *  Testing: sat -o with NX_DFLT_PART, -p, and default
 *  Module(s):
 *  usr/bin/sat.c
 *
 * Revision 1.8  1993/11/24  20:21:25  merrow
 * Don't catch signals if they are set to SIG_IGN by the calling program.
 * Makes it possible to log out and leave sat running.
 *
 *  Reviewer: Joel
 *  Risk: Low (three lines)
 *  Benefit or PTS #: 6222
 *  Testing: Nandini checked it with nohup and logging out
 *  Module(s): sat.c
 *
 * Revision 1.7  1993/06/24  23:22:28  merrow
 * Write output file after each pass so available if sat doesn't finish.
 *
 * Revision 1.6  1993/02/11  19:24:56  merrow
 * Fixed PTS 4024, updated SAT release to 1.0.
 * Fixed PTS 4025, include -V in usage message.
 * Fixed PTS 4110, use uts.version instead of uts.release.
 *
 * Revision 1.5  1992/12/21  23:57:17  merrow
 * Changed to catch and use SIGHUP to terminate tests. This is needed to
 * support tests cleaning up their own temporary files.
 *
 * Revision 1.4  1992/12/21  22:09:55  merrow
 * Check for existence and format of compute and service partinfo files (#3838).
 * Check that .compute and .service partitions are not size 0 (#3837).
 *
 * Revision 1.3  1992/12/14  23:51:50  merrow
 * Fix #3834, correct error message with -d argument not a directory.
 *
 * Revision 1.2  1992/12/11  19:04:28  merrow
 * For #3671, report error if no write permission in build directory.
 *
 * Revision 1.1  1992/12/11  05:43:37  merrow
 * Correct bug #3674, use of -m flag.
 * Correct bug #3663, error checking for bogus run and makefile.
 *
 * Log file entries lost during AFS disaster:
 * Revision 1.2  1992/12/01  01:20:26  merrow
 * Detect errors in parameters for the -r and -m options.
 *
 * Revision 1.1  1992/10/12  22:08:41  merrow
 * Initial version of System Acceptance Test command and Makefile.
 *
 */
#endif

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <nx.h>

#define VERSION "Paragon OSF/1 System Acceptance Test, Release 1.0"

#define TRUE 1
#define FALSE 0

#define TEST FALSE          /* use test directory hierarchy */
#define DEBUG FALSE         /* enable debug printing */

#if TEST
#define DEFAULT_DIR "../libsat"
#define DEFAULT_HELP "../libsat/help"
#else
#define DEFAULT_DIR "/usr/lib/sat"
#define DEFAULT_HELP "/usr/lib/sat/help"
#endif

#define COMPUTE_PART ".compute"
#define SERVICE_PART ".service"
#define DEFAULT_PART COMPUTE_PART
#define README "README"
#define README_SIZE 2048
#define NO_PID 0
#define RUN "run"
#define SHELL "/bin/sh"
#define MAKE "/usr/bin/make"
#define MAKEARGS "clean" , "all"
#define DEVNULL "/dev/null"
#define TMPPATH "/usr/tmp"
#define DAYSECONDS (3600 * 24)
#define DATEFMT "%b %d, %Y"
#define TIMEFMT "%d:%02d:%02d"
#define STARTFMT "%b %d, %Y %T"
#define ENDFMT ((end_time - start_time) >= DAYSECONDS ? "%b %d %T" : "%T")
#define ERR (errno > sys_nerr ? "unknown error" : sys_errlist[errno])
#define STR_S(c) (c == 1 ? "" : "s")
#define STR_ES(c) (c == 1 ? "" : "es")
#define CONFIG_MSG "Configuration: %d node compute partition, %d node service partition"
#define SEPARATOR "\n---------------------------------------------\n"
#define LIST_MSG "\nThe following is a summary of available tests. You may get more detail\nby running this command with the -h flag and a list of tests.\n"
#define ANYOPT (bflag || cflag || xflag || mflag || rflag || log || output || pflag || dflag || Dflag)
#define DBG_ENV_STR "SAT_DEBUG"
#define ABORT_SIG   SIGUSR1

/* usage information */
#define B_MSG "\t-b\tBuild tests from sources before running them\n"
#define C_MSG "\t-c\tRun tests concurrently\n"
#define D_MSG "\t-d\tRun tests in directory hierarchy rooted at %s\n"
#define H_MSG "\t-h\tDisplay help message\n"
#define L_MSG "\t-l\tWrite transcript to log file %s\n"
#define M_MSG "\t-m\tRepeat tests for %s minutes\n"
#define O_MSG "\t-o\tWrite final report to output file %s\n"
#define P_MSG "\t-p\tRun tests in partition %s instead of compute partition\n"
#define R_MSG "\t-r\tRepeat tests %s times\n"
#define X_MSG "\t-x\tExit immediately if a test fails\n"
#define V_MSG "\t-V\tDisplay version information\n"
#define d_MSG "\t-D\tDebug mode\n"

/* functions */
void    add_tests();
void    build_tests();
char    *catdirfile();
int checkdir();
int checkscript();
int checkbuild();
void    copy_build();
void    copy_file();
void    copy_result();
void    die();
char    *elapsed();
void    error_exit();
void    final_logging();
void    final_report();
void    find_tests();
void    fmessage();
int getfilesize();
void    getinfo();
int get_part_size();
void    help();
void    help_tests();
void    on_alarm();
void    on_intr();
void    run_tests();
int strcount();
int strpath();
char    *tmpout();
char    *tmperr();
void    transcript();
void    transcript_copy();
void    usage();
void    wait_tests();
char    *xmalloc();

/* externals */
extern char *optarg;
extern int optind, errno, sys_nerr;
extern char *sys_errlist[];
extern char *malloc();
extern char *mktemp();
extern char *strrchr();
extern char *getenv();

/* options */
int bflag = FALSE;          /* build from sources */
int cflag = FALSE;          /* run concurrently */
int dflag = FALSE;          /* set directory hierarchy */
int hflag = FALSE;          /* display help message */
int mflag = FALSE;          /* set number of minutes */
int pflag = FALSE;          /* set partition to run in */
int rflag = FALSE;          /* set number of repetitions */
int xflag = FALSE;          /* exit on error */
int Dflag = FALSE;          /* debug mode */
char *dir = DEFAULT_DIR;        /* directory hierarchy */
char *log = NULL;           /* log file */
char *output = NULL;            /* output file */
char *part = DEFAULT_PART;      /* partition for parallel tests */
int mins = 0;               /* minutes to execute */
int reps = 1;               /* passes to execute */
int compute_size = 0;           /* size of compute partition */
int service_size = 0;           /* size of service partition */
struct utsname uts;         /* operating system info */

/* tests */
int ntests = 0;             /* number of tests */
int test_errors = 0;            /* number of failed tests */
int test_reps = 0;          /* actual number of repetitions */
int test_term = FALSE;          /* indicates tests terminated at end */
int test_built = 0;         /* number of tests built */

struct test {
    char *t_name;           /* test directory name */
    char *t_path;           /* path to directory containing test */
    char *t_build;          /* path to build makefile */
    pid_t t_pid;            /* pid for running test */
    char *t_header;         /* first line of README file */
    char *t_summary;        /* help summary from README file */
    time_t t_start_time;        /* start time for test */
    time_t t_end_time;      /* end time for test */
    struct test *t_next;        /* link for list */
} *tests = NULL;            /* list of tests */

/* timing */
time_t start_time;          /* start time for program */
time_t end_time;            /* end time for program */
time_t build_time;          /* time after building tests */
int timeout = FALSE;            /* indicates test time expired */

/* files */
FILE *log_file = NULL;          /* transcript log file */
FILE *output_file = NULL;       /* output file for final report */
FILE *result_file = NULL;       /* temp file for results */
FILE *error_file = NULL;        /* temp file for error messages */

/* system acceptance test command */
main(argc, argv)
int argc;
char    **argv;
{
    int             i, c, status, opterr = FALSE;
    char            *p, buf1[32];
    struct test         *tp;
    time_t          t1, t2;
    struct sigaction    new_action;
    struct sigaction    last_action;

    /* default partition can be specified by environment variable */
    if ((p = getenv("NX_DFLT_PART")) != NULL) part = p;

    /* 
     * debug mode can be specified by environment variable 
     *  <DBG_ENV_STR> | <DBG_ENV_STR>=<non-zero>    [on] 
     *  <DBG_ENV_STR>=0                 [off]
     */
    if ((p = getenv(DBG_ENV_STR)) != NULL) {
        if (*p == '\0') {
            Dflag = TRUE;
        } else {
            Dflag = atoi(p);
        }
    }

    /* parse the arguments on the command line */
    /* command line args typically overrides environment */
    while ((c = getopt(argc, argv, "bchxVd:l:m:o:p:r:D")) != -1)
    switch (c) {
        case 'b': /* do builds before running tests */
            bflag = TRUE;
            break;
        case 'c': /* run tests concurrently */
            cflag = TRUE;
            break;
        case 'h': /* display help information */
            hflag = TRUE;
            break;
        case 'x': /* exit on error */
            xflag = TRUE;
            break;
        case 'V': /* display version information */
            printf("%s\n", VERSION);
            exit(0);
        case 'd': /* set directory hierarchy */
            dflag = TRUE;
            dir = optarg;
            break;
        case 'l': /* specify log file */
            log = optarg;
            break;
        case 'm': /* set time to run in minutes */
            mflag = TRUE;
            mins = atoi(optarg);
            if (!isdigit(optarg[0]) || mins == 0) {
                opterr = TRUE;
                fprintf(stderr, "sat: invalid -m option\n");
            }
            break;
        case 'o': /* specify output file */
            output = optarg;
            break;
        case 'p': /* specify partition to run tests in */
            pflag = TRUE;
            part = optarg;
            break;
        case 'r': /* set number of repetitions to run */
            rflag = TRUE;
            reps = atoi(optarg);
            if (!isdigit(optarg[0])) {
                opterr = TRUE;
                fprintf(stderr, "sat: invalid -r option\n");
            }
            break;
        case 'D': /* Debug mode */
              /* currently this means save logs if error or intr */
            Dflag = TRUE;
            break;
        case '?': /* unknown argument */
            opterr = TRUE;
            break;
    }

#if DEBUG
    printf("**** build %d concurrent %d help %d debug %d error exit %d\n",
        bflag, cflag, hflag, Dflag, xflag);
    printf("**** directory %s log %s output %s part %s\n",
        dir, log, output, part);
    printf("**** minutes %d passes %d\n", mins, reps);
    if (optind < argc) {
        printf("**** ");
        for (c = optind; c < argc; c++) printf("%s ", argv[c]);
        printf("\n");
    }
#endif

    /* display usage message if error in command line arguments */
    if (opterr) {
        usage(stderr);
        die(2);
    }

    /* display help message if -h and no tests specified */
    if (hflag && optind >= argc) help(stdout);

    /* verify base of directory hierarchy */
    if (!checkdir(dir)) {
        fprintf(stderr, "sat: can't open directory %s (%s)\n", dir, ERR);
        die(2);
    }

    /* build list of tests */
    if (optind >= argc) add_tests(dir);
    else find_tests(dir, argc - optind, &argv[optind]);

#if DEBUG
{
    printf("**** ntests %d\n", ntests);
    for (tp = tests; tp; tp = tp->t_next) {
        printf("**** test %s path %s build %s\n",
            tp->t_name, tp->t_path, tp->t_build);
        /* printf("%s\n%s\n", tp->t_header, tp->t_summary); */
    }
}
#endif

    /* error if any tests in list not found */
    i = strlen(dir);
    for (c = optind; c < argc; c++) {
        for (tp = tests; tp; tp = tp->t_next) 
            if (!strpath(argv[c], &tp->t_path[i])) break;
        if (!tp) {
            fprintf(stderr, "sat: %s not found\n", argv[c]);
            test_errors++;
        }
    }
    /* if no other errors then check if any test directories were found */
    if (test_errors == 0  && ntests == 0) {
        fprintf(stderr, "sat: no tests found\n");
        test_errors++;
    }
    if (test_errors) die(2);

    /* display help for tests in the list */
    if (hflag) {
        help_tests(stdout, optind < argc);
        exit(0);
    }

    /* verify log and output files are different */
    if (log && output && strcmp(log, output) == 0) {
        fprintf(stderr, "sat: log and output are same file %s\n", log);
        die(2);
    }

    /* create log, output and temp files */
    if (log && (log_file = fopen(log, "w")) == NULL) {
        fprintf(stderr, "sat: can't open log file %s (%s)\n", log, ERR);
        die(2);
    }

    if (output && (output_file = fopen(output, "w")) == NULL) {
        fprintf(stderr, "sat: can't open output file %s (%s)\n", output, ERR);
        die(2);
    }

    if ((result_file = tmpfile()) == NULL) {
        fprintf(stderr, "sat: can't open tmp file for results (%s)\n", ERR);
        die(2);
    }

    if ((error_file = tmpfile()) == NULL) {
        fprintf(stderr, "sat: can't open tmp file for errors (%s)\n", ERR);
        die(2);
    }

    /* catch signals so we can clean up */
    /* 
     * Need to continue ignoring any ignored INT, QUIT, HUP to handle 
     * signals properly when backgrounded or nohuped. 
     *
     * Once a signal is recieved block all others. What is done in the
     * handler's is not reentrant! 
     */
    new_action.sa_handler = on_intr;
    new_action.sa_mask =    sigmask(SIGINT) | \
                sigmask(SIGQUIT) | \
                sigmask(SIGHUP) | \
                sigmask(SIGTERM) | \
                sigmask(SIGALRM);
    new_action.sa_flags = 0; /* no restart needed, we exit on 1st signal */

    if (sigaction(SIGINT, &new_action, &last_action) != 0) {
        fprintf(stderr, "sat: setup of SIGINT handler failed (%s)\n", ERR); 
        die(2);
    }
    if (last_action.sa_handler == SIG_IGN) {
        if (sigaction(SIGINT, &last_action, 0) != 0) {
            fprintf(stderr, "sat: setup of SIGINT SIG_IGN failed (%s)\n", ERR);
            die(2);
        }
    }

    if (sigaction(SIGQUIT, &new_action, &last_action) != 0) {
        fprintf(stderr, "sat: setup of SIGQUIT handler failed (%s)\n", ERR);
        die(2);
    }
    if (last_action.sa_handler == SIG_IGN) {
        if (sigaction(SIGQUIT, &last_action, 0) != 0) {
            fprintf(stderr, "sat: setup of SIGQUIT SIG_IGN failed (%s)\n", ERR);
            die(2);
        }
    }

    if (sigaction(SIGHUP, &new_action, &last_action) != 0) {
        fprintf(stderr, "sat: setup of SIGHUP handler failed (%s)\n", ERR);
        die(2);
    }
    if (last_action.sa_handler == SIG_IGN) {
        if (sigaction(SIGHUP, &last_action, 0) != 0) {
            fprintf(stderr, "sat: setup of SIGHUP SIG_IGN failed (%s)\n", ERR);
            die(2);
        }
    }

    if (sigaction(SIGTERM, &new_action, &last_action) != 0) {
        fprintf(stderr, "sat: setup of SIGTERM handler failed (%s)\n", ERR);
        die(2);
    }

    /* save the starting time */
    time(&start_time);
    strftime(buf1, sizeof(buf1), DATEFMT, localtime(&start_time));
    transcript("Paragon SAT started %s", buf1);

    /* get operating system information */
    if (uname(&uts) != 0) {
        fprintf(stderr, "sat: uname error (%s)\n", ERR);
        die(2);
    }

    transcript("Host name: %s", uts.nodename);
    transcript("Operating system release: %s %s", uts.sysname, uts.version);

    /* get partition size information */
    compute_size = get_part_size(part);
    service_size = get_part_size(SERVICE_PART);
    transcript(CONFIG_MSG, compute_size, service_size);

    /* build tests if necessary and exit if errors */
    if (bflag) {
        build_tests();
        if (test_errors) {

            time(&build_time);
            transcript("Run failed, %d build error%s", 
                    test_errors, STR_S(test_errors));
            /* write the final report and close open files */
            if (output_file != NULL) {
                final_report();
                fclose(output_file);
            }
            if (log_file != NULL) fclose(log_file);

            die(1);
        }
    }
    time(&build_time);

    /* set alarm if max time specified */
    if (mins > 0) {
        /* using same mask and flags as above */
        new_action.sa_handler = on_alarm;
        if (sigaction(SIGALRM, &new_action, 0) != 0) {
            fprintf(stderr, "sat: setup of SIGALRM handler failed (%s)\n", ERR); 
            die(2);
        }
        alarm(mins * 60);
    }

    /* run each test repetition */
    for (c = 0; !timeout && ((!rflag && mflag) || (c < reps)); c++) {
        time(&t1);
        run_tests();
        test_reps++;
        if (reps > 1 || mflag) {
            time(&t2);
            transcript("Pass %d %s, elapsed time %s", test_reps,
                test_term ? "terminated" : "finished", elapsed(t1, t2));
        }

        /* update the report in case of system crash */
        if (output_file != NULL) {
            final_report();
            fflush(output_file);
        }
    }
        
    /* finalize all output */
    final_logging();

    /* 
     * exit with status 0 if no failures, 1 otherwise 
     */
    status = 1;
    if (test_errors == 0) {
        status = 0;
    }

    die(status);
}

/* 
 * finalize all output. To be used just before exiting.
 */
void
final_logging() {
char    *p;

    /* get the ending time and calculate elapsed time */
    time(&end_time);
    p = elapsed(start_time, end_time);

    /* display the last transcript message */
    if (test_errors == 0) {
        transcript("Run %s, no errors, total elapsed time %s",
            timeout ? "terminated" : "finished", p);
    } else {
        transcript("Run %s, %d error%s, total elapsed time %s",
            timeout ? "terminated" : "finished",
            test_errors, STR_S(test_errors), p);
    }

    /* write the final report and close open files */
    if (output_file != NULL) {
        final_report();
        fclose(output_file);
    }
    if (log_file != NULL) fclose(log_file);
}

/* check if a directory is accessible */
int 
checkdir(path)
char    *path;
{
    struct stat sbuf;

    if (stat(path, &sbuf) != 0) return FALSE;
    if (!S_ISDIR(sbuf.st_mode)) {
        errno = ENOTDIR;
        return FALSE;
    }
    if (access(path, R_OK | X_OK) != 0) return FALSE;
    return TRUE;
}

/* check if a file is a script (executable not required) */
int 
checkscript(path)
char    *path;
{
    struct stat sbuf;

    if (access(path, R_OK) != 0) return FALSE;
    if (stat(path, &sbuf) != 0) return FALSE;
    if (!S_ISDIR(sbuf.st_mode)) return TRUE;
    errno = EISDIR;
    return FALSE;
}

/* check if a build file is accessible and not empty */
int 
checkbuild(tp, make)
struct test     *tp;
char        *make;
{
    struct stat sbuf;
    char *p = catdirfile(tp->t_path, make);

    if (access(p, R_OK) != 0) return FALSE;
    if (stat(p, &sbuf) != 0) return FALSE;
    if (sbuf.st_size == 0) {
        fprintf(stderr, "sat: %s is empty\n", p);
        test_errors++;
        return FALSE;
    }
    if (S_ISDIR(sbuf.st_mode)) {
        fprintf(stderr, "sat: %s is a directory\n", p);
        test_errors++;
        return FALSE;
    }
    if (access(tp->t_path, W_OK) != 0) {
        fprintf(stderr, "sat: can't build without write permission in %s\n", tp->t_path);
        test_errors++;
        return FALSE;
    }
    tp->t_build = make;
    return TRUE;
}

/* find all tests in the directory hierarchy */
void 
find_tests(path, n, testp)
char    *path;
int n;
char    **testp;
{
    int i;
    char *p;
    DIR *dirp;
    struct dirent *dp;

    p = catdirfile(path, RUN);
    if (!checkscript(p)) {
        dirp = opendir(path);
        while (dp = readdir(dirp)) {
            if (dp->d_name[0] != '.') {
                p = catdirfile(path, dp->d_name);
                if (checkdir(p)) {
                    for (i = 0; i < n; i++) {
                        if (strcmp(testp[i], dp->d_name) == 0) {
                            add_tests(p);
                            break;
                        }
                    }
                    if (i == n) find_tests(p, n, testp);
                }
            }
        }
        closedir(dirp);
    }
}

/* add all tests in the directory hierarchy to the list */
void 
add_tests(path)
char    *path;
{
    char *p;
    struct test *tp;
    DIR *dirp;
    struct dirent *dp;
    struct stat sbuf;

    p = catdirfile(path, RUN);
    if (checkscript(p)) {
        ntests++;
        tp = (struct test *)xmalloc(sizeof(struct test));
        tp->t_path = path;
        tp->t_pid = NO_PID;
        if (stat(p, &sbuf) != 0 || sbuf.st_size == 0) {
            fprintf(stderr, "sat: %s is empty\n", p);
            test_errors++;
        }
        getinfo(tp);
        if (bflag) { /* if building looking for makefiles */
            if (!checkbuild(tp, "makefile"))
                checkbuild(tp, "Makefile");
        }
        tp->t_next = tests;
        tests = tp;
    } else {
        dirp = opendir(path);
        while (dp = readdir(dirp)) {
            if (dp->d_name[0] != '.') {
                p = catdirfile(path, dp->d_name);
                if (checkdir(p)) add_tests(p);
            }
        }
        closedir(dirp);
    }
}

/* build tests from sources */
void 
build_tests()
{
    pid_t       pid;
    int         i, c, sloc;
    struct test     *tp;
    char        *p;
    char        s[32];
    sigset_t    new_mask;
    sigset_t    old_mask;

    /* setup signal mask */
    sigemptyset(&new_mask);
    sigaddset(&new_mask,SIGALRM);
    sigaddset(&new_mask,SIGTERM);
    sigaddset(&new_mask,SIGINT);
    sigaddset(&new_mask,SIGQUIT);
    sigaddset(&new_mask,SIGHUP);

    for (tp = tests; tp && !timeout ; tp = tp->t_next) {
        /* 
         * quit if were interrupted 
         */
        if (timeout) {
            test_term = TRUE; 
            break;
        }

        /* no makefile--skip */
        if (tp->t_build == NULL) continue;

        /* 
         * block interrupts during critical section 
         */ 
        sigprocmask(SIG_BLOCK,&new_mask,&old_mask);

        time(&tp->t_start_time);
        p = tp->t_header;
        pid = fork();
        if (pid < 0) {
            fprintf(stderr, "sat: fork error (%s)\n", ERR);
            die(2);
        } else if (pid == 0) {
            /*
             * child does compile
             */
            pid = getpid();
            if (chdir(tp->t_path) != 0) {
                fprintf(stderr, "sat child: can't build %s (%s)\n", tp->t_path, ERR);
                die(2);
            }
            sprintf(s, "%s=%d", DBG_ENV_STR, Dflag);
            if (putenv(s) != 0) {
                fprintf(stderr, "sat child: putenv failed \n");
                die(2);
            }
            /* pre-open files in case of error */
            if (open(DEVNULL, O_RDWR) < 0) {
                fprintf(stderr, "sat child: couldn't open %s (%s)\n", DEVNULL, ERR);
                die(2);
            }
            if (creat(tmpout(pid), 0644) < 0) {
                fprintf(stderr, "sat child: couldn't create %s (%s)\n", tmpout(pid), ERR);
                die(2);
            }

            /* close parent's duped file descriptors */
            /* and open new stdin, stdout, stderr for child */
            for (i = 0; close(i) == 0; i++); 
            /* if these guys fail, the best we can do is return a weird exitcode */
            /* because all file descriptors are already closed... */
            if (open(DEVNULL, O_RDWR) != 0) die(94);
            if (creat(tmpout(pid), 0644) != 1) die(95);
            if (dup(1) != 2) die(96);

            sigsetmask(0);
            execl(MAKE, MAKE, "-f", tp->t_build, MAKEARGS, NULL);
            fprintf(stderr,"sat: exec(make) failed (%s)\n", ERR);
            die(2);
        }
        tp->t_pid = pid;
        transcript("%s build started", p);

        /* restore original signal mask */
        sigprocmask(SIG_SETMASK,&old_mask,0);

        /* wait for build to finish or terminate */
        while (waitpid(pid, &sloc, 0) != pid);

        /* 
         * block interrupts during critical section 
         */ 
        sigprocmask(SIG_BLOCK,&new_mask,&old_mask);

        time(&tp->t_end_time);
        if (WIFSIGNALED(sloc)) {
            c = WTERMSIG(sloc);
            transcript("%s build terminated (signal %d)", p, c);
            copy_build(tp, FALSE, "signal", c);
            test_errors++;
            /* don't remove log */
        } else {
            c = WEXITSTATUS(sloc);
            if (c == 0) {
                transcript("%s build done", p);
                copy_build(tp, TRUE, "", 0);
                unlink(tmpout(pid));
                test_built++;
            } else {
                transcript("%s build failed (status %d)", p, c);
                copy_build(tp, FALSE, "status", c);
                test_errors++;
                if (!Dflag) unlink(tmpout(pid));
            }
        }
        tp->t_pid = NO_PID;

        if (xflag && test_errors) error_exit();

        /* restore original signal mask */
        sigprocmask(SIG_SETMASK,&old_mask,0);
    }
}

/* run one repetition of tests */
void 
run_tests()
{
    pid_t       pid;
    int         i;
    struct test     *tp;
    char        s[32];
    sigset_t    new_mask;
    sigset_t    old_mask;

    /* preset signal mask */
    sigemptyset(&new_mask);
    sigaddset(&new_mask,SIGALRM);
    sigaddset(&new_mask,SIGTERM);
    sigaddset(&new_mask,SIGINT);
    sigaddset(&new_mask,SIGQUIT);
    sigaddset(&new_mask,SIGHUP);

    for (tp = tests; tp; tp = tp->t_next) {
        /* 
         * block interrupts during critical section 
         */ 
        sigprocmask(SIG_BLOCK,&new_mask,&old_mask);

        /* 
         * quit if we've run out of time or were otherwise interrupted 
         */
        if (timeout) {
            test_term = TRUE; 
            break;
        }

        /* 
         * start a test
         */
        time(&tp->t_start_time);
        pid = fork();
        if (pid < 0) {
            fprintf(stderr, "sat: fork error (%s)\n", ERR);
            die(2);
        } else if (pid == 0) {

            /*
             * child
             */

            pid = getpid();
            if (chdir(tp->t_path) != 0) {
                fprintf(stderr, "sat child: can't run %s (%s)\n", tp->t_path, ERR);
                die(2);
            }
            sprintf(s, "%s=%d", DBG_ENV_STR, Dflag);
            if (putenv(s) != 0) {
                fprintf(stderr, "sat child: putenv failed\n");
                die(2);
            }
            /* pre-open files in case of error */
            if (open(DEVNULL, O_RDWR) < 0) {
                fprintf(stderr, "sat child: couldn't open %s (%s)\n", DEVNULL, ERR);
                die(2);
            }
            if (creat(tmpout(pid), 0644) < 0) {
                fprintf(stderr, "sat child: couldn't creat %s (%s)\n", tmpout(pid), ERR);
                die(2);
            }
            if (creat(tmperr(pid), 0644) < 0) {
                fprintf(stderr, "sat child: couldn't creat %s (%s)\n", tmperr(pid), ERR);
                die(2);
            }

            /* close parent's duped file descriptors */
            /* and open new stdin, stdout, stderr for child */
            for (i = 0; close(i) == 0; i++); 
            /* if these guys fail, the best we can do is return a weird exitcode */
            /* because all file descriptors are already closed... */
            if (open(DEVNULL, O_RDWR) != 0) die(97);
            if (creat(tmpout(pid), 0644) != 1) die(98);
            if (creat(tmperr(pid), 0644) != 2) die(99);

            sigsetmask(0);
            execl(RUN, RUN, part, NULL);
            fprintf(stderr,"sat: exec(run) failed (%s)\n", ERR);
            die(2);
        }

        /*
         * parent
         */

        tp->t_pid = pid;

        /* restore original signal mask */
        sigprocmask(SIG_SETMASK,&old_mask,0);

        transcript("%s started", tp->t_header);
        if (!cflag) wait_tests();
    }
    if (cflag) wait_tests();
}

/* wait for all running tests to complete */
void 
wait_tests()
{
    pid_t       pid;
    int         sloc, c;
    char        *p;
    struct test     *tp;
    sigset_t    new_mask;
    sigset_t    old_mask;

    /* setup sigmask */
    sigemptyset(&new_mask);
    sigaddset(&new_mask,SIGALRM);
    sigaddset(&new_mask,SIGTERM);
    sigaddset(&new_mask,SIGINT);
    sigaddset(&new_mask,SIGQUIT);
    sigaddset(&new_mask,SIGHUP);

    while (TRUE) {
        pid = wait(&sloc);
        if (pid == -1) {
            if (errno == ECHILD) return;
            if (errno != EINTR) {
                fprintf(stderr, "sat: error waiting for tests (%s)\n", ERR);
                die(2);
            }
        } else {
            /*
             * a test has died. 
             * determine and report cause of death.
             * cleanup logs.
             */

            /* block interrupts during critical section */ 
            sigprocmask(SIG_BLOCK,&new_mask,&old_mask);

            for (tp = tests; tp; tp = tp->t_next) {
                if (tp->t_pid == pid) {
                    time(&tp->t_end_time);
                    p = tp->t_header;

                    /* tell us about stray signals */
                    if (WIFSIGNALED(sloc)) {
                        c = WTERMSIG(sloc);
#if DEBUG
    fprintf(stderr,"**** pid %d died of signal %d\n", pid, c);
#endif
                        /* 
                         * test terminated because 
                         * of signal. don't copy 
                         * incomplete results  but
                         * leave logs for debug.
                         */
                        transcript("%s terminated (signal %d)", p, c);
                        test_term = TRUE; 
                        test_errors++;
                    /* test exited normally or with known signal */
                    /* or a signal it knows how trap/deal with */
                    } else {
                        c = WEXITSTATUS(sloc);
#if DEBUG
    fprintf(stderr, "**** pid(%d) exited(%d): err log(%s) size %d\n", 
        pid, c, tmperr(pid), getfilesize(tmperr(pid)));
#endif
                        /* the getfilesize is unfortunate because doing */
                        /* a "set -x" in run script causes failure */
                        if (c == 0 && getfilesize(tmperr(pid)) == 0) {
                            /* success */
                            transcript("%s passed", p);
                            copy_result(tp, TRUE, "", 0);
                            unlink(tmpout(pid));
                            unlink(tmperr(pid));
                        } else if (c == 3) {
                            /* 
                             * test caught 
                             * signal and exited.
                             * don't copy 
                             * incomplete results
                             * but leave logs.
                             */
                            transcript("%s terminated by unknown signal", p);
                            test_term = TRUE; 
        /* ?? test_errors++ ?? 
        we may have been the one that sent the
        signal in order to abort the test. however, if we sent
        it because someone interrupted us from the terminal then 
        the test may have seen the terminal signal first. It would
        look like an error but was really us. (we can't reliably
        block terminal-sourced signals from all our decendents else 
        we could workaround this.)
        */
                        } else {
                            /* not successful */
                            transcript("%s failed (status %d)", p, c);
                            copy_result(tp, FALSE, "status", c);
                            test_errors++;
                            if (!Dflag) {
#if DEBUG
    fprintf(stderr,"**** unlinking tmpout and tmperr\n");
#endif
                                unlink(tmpout(pid));
                                unlink(tmperr(pid));
                            }
                        }
                    }
                    tp->t_pid = NO_PID;
                }
            }
            if (xflag && test_errors) error_exit();
            /* restore original signal mask */
            sigprocmask(SIG_SETMASK,&old_mask,0);
        }
    }
}

/* copy results for a build */
/* if error also written to transcript */
void 
copy_build(tp, success, s, c)
struct test *tp;
int         success, c;
char        *s;
{
    FILE *fp;
    char *p = tp->t_header;
    time_t ts = tp->t_start_time, te = tp->t_end_time;
    char *q = elapsed(ts, te);

    fputc('\n', result_file);
    fmessage(result_file, ts, "%s build started", p);
    if ((fp = fopen(tmpout(tp->t_pid), "r")) != NULL) {
        copy_file(fp, result_file);
        if (!success) transcript_copy(fp);
        fclose(fp);
    }
    if (success)
        fmessage(result_file, te, "%s build done, elapsed time %s", p, q);
    else
        fmessage(result_file, te, "%s build failed (%s %d), elapsed time %s",
            p, s, c, q);
}

/* copy output and error messages for a test */
/* error messages also written to transcript */
void 
copy_result(tp, success, s, c)
struct test *tp;
int         success, c;
char        *s;
{
    FILE *fp;
    char *p = tp->t_header;
    time_t ts = tp->t_start_time, te = tp->t_end_time;
    char *q = elapsed(ts, te);

    fputc('\n', result_file);
    fmessage(result_file, ts, "%s started", p);
    if ((fp = fopen(tmpout(tp->t_pid), "r")) != NULL) {
        copy_file(fp, result_file);
        fclose(fp);
    }

    if (!success) {
        fputc('\n', error_file);
        fmessage(error_file, ts, "%s started", p);
        if ((fp = fopen(tmperr(tp->t_pid), "r")) != NULL) {
            copy_file(fp, error_file);
            rewind(fp);
            copy_file(fp, result_file);
            transcript_copy(fp);
            fclose(fp);
        }
        fmessage(result_file, te, "%s failed (%s %d), elapsed time %s",
            p, s, c, q);
        fmessage(error_file, te, "%s failed (%s %d), elapsed time %s",
            p, s, c, q);
    } else fmessage(result_file, te, "%s passed, elapsed time %s", p, q);
}

/* copy all of source file to destination file */
void 
copy_file(source, dest)
FILE    *source, *dest;
{
    int c;
    while ((c = fgetc(source)) != EOF) fputc(c, dest);
}

/* write the final report */
void 
final_report()
{
    int compute, service;
    FILE *fp = output_file;
    char *p, buf1[32], buf2[32];
    struct test *tp;

    rewind(fp); /* rewind it and rewrite */
    ftruncate(fileno(fp), 0);

    rewind(result_file);
    rewind(error_file);

    strftime(buf1, sizeof(buf1), STARTFMT, localtime(&start_time));
    strftime(buf2, sizeof(buf2), ENDFMT, localtime(&end_time));
    p = test_term ? "terminated" : "finished";
    fprintf(fp, "Paragon SAT started %s, %s %s\n", buf1, p, buf2);

    fprintf(fp, "Host name: %s\n", uts.nodename);
    fprintf(fp, "Operating system release: %s %s\n",
        uts.sysname, uts.version);
    fprintf(fp, CONFIG_MSG, compute_size, service_size);

    if (ANYOPT) {
        sprintf(buf1, "%d", mins);
        sprintf(buf2, "%d", reps);
        fprintf(fp, "\nOptions selected:\n");
        if (bflag) fprintf(fp, B_MSG);
        if (cflag) fprintf(fp, C_MSG);
        if (dflag) fprintf(fp, D_MSG, dir);
        if (log) fprintf(fp, L_MSG, log);
        if (mflag) fprintf(fp, M_MSG, buf1);
        if (output) fprintf(fp, O_MSG, output);
        if (pflag) fprintf(fp, P_MSG, part);
        if (rflag) fprintf(fp, R_MSG, buf2);
        if (xflag) fprintf(fp, X_MSG);
        if (Dflag) fprintf(fp, d_MSG);
    }

    fprintf(fp, "\nEach pass includes %d test%s:\n", ntests, STR_S(ntests));
    for (tp = tests; tp; tp = tp->t_next) fprintf(fp, "\t%s\n", tp->t_header);

    if (bflag) fprintf(fp, "\nBuilt %d test%s, elapsed time %s",
                test_built, STR_S(test_built), elapsed(start_time, build_time));

    fprintf(fp, "\nRan %d pass%s with %d error%s detected, elapsed time %s\n",
        test_reps, STR_ES(test_reps), test_errors, STR_S(test_errors),
        elapsed(build_time, end_time));
    if (test_term) fprintf(fp, "Last pass was terminated\n");
    fprintf(fp, "Total elapsed time %s\n", elapsed(start_time, end_time));

    if (test_errors && !feof(error_file)) {
        fprintf(fp, SEPARATOR);
        fprintf(fp, "Error messages written by failed tests...\n");
        copy_file(error_file, fp);
        fputc('\n', fp);
    }

    if (!feof(result_file)) {
        fprintf(fp, SEPARATOR);
        fprintf(fp, "Output written by all %stests...\n",
            bflag ? "builds and " : "");
        copy_file(result_file, fp);
        fputc('\n', fp);
    }
}

/* return the size of a partition from line 4 in .partinfo file */
int 
get_part_size(partname)
char    *partname;
{
    nx_part_info_t partattr;

    if (nx_part_attr(partname, &partattr) != 0) {
        partattr.nodes = 0;
    } 
    if (partattr.nodes <= 0) {
        fprintf(stderr, "sat: %s partition size 0\n", partname);
    }

    return(partattr.nodes);
}

/* alarm indicates end of time for tests */
void 
on_alarm()
{
    struct test *tp;

    fprintf(stderr, "sat: time's up: aborting test(s)...\n");

    timeout = TRUE;
    for (tp = tests; tp; tp = tp->t_next)
        if (tp->t_pid != NO_PID) {
            kill(tp->t_pid, ABORT_SIG);
        }

    /* drop back into run_tests loop with timeout flag set */
}

/* interrupt so kill tests, clean up and exit */
void 
on_intr(sig)
int sig;
{
    struct test *tp;

    fprintf(stderr, "sat: caught signal %d: aborting test(s)...\n", sig);

    timeout = TRUE;
    for (tp = tests; tp; tp = tp->t_next) {
        if (tp->t_pid != NO_PID) {
#if DEBUG
    fprintf(stderr, "**** sat: sending signal %d to pid %d\n",
        ABORT_SIG,tp->t_pid);
#endif
            kill(tp->t_pid, ABORT_SIG);
        }
    }

    /* drop back into run_tests or build_tests loop with timeout flag set */
}

/* exit on first error */
void 
error_exit()
{
    transcript("Exiting on first error");

    final_logging();

    die(1);
}

/* generic routine to flush buffers and die abnormally */
void 
die(exit_code)
int exit_code;
{
    fflush(stdout);
    fflush(stderr);
    exit(exit_code);
}

/* allocate zeroed memory and verify result */
char *
xmalloc(len)
int len;
{
    char *p = malloc(len);
    if (!p) {
        fprintf(stderr, "sat: error allocating memory (length %d) (%s)\n", len, ERR);
        die(2);
    }
    bzero(p, len);
    return p;
}

/* concatenate directory and file names */
char *
catdirfile(dpath, fpath)
char    *dpath, *fpath;
{
    char *p = xmalloc(strlen(dpath) + strlen(fpath) + 2);
    sprintf(p, "%s/%s", dpath, fpath);
    return p;
}

/* get info about a test from its README file */
void 
getinfo(tp)
struct test *tp;
{
    int len, fd;
    char *p = catdirfile(tp->t_path, README);

    tp->t_name = strrchr(tp->t_path, '/') + 1;
    tp->t_header = tp->t_path;
    tp->t_summary = "";
    fd = open(p, O_RDONLY, 0);
    free(p);
    if (fd >= 0) {
        p = xmalloc(README_SIZE);
        len = read(fd, p, README_SIZE);
        if (len < 2 || strcount(p, len, '\n') < 3) {
            fprintf(stderr, "sat: %s README file format error\n", tp->t_path);
            test_errors++;
        } else {
            if (len == README_SIZE) {
                p[len-2] = '\n';
                p[len-1] = 0;
            } else p[len] = 0;
            tp->t_header = p;
            while (*p != '\n') p++;
            *p++ = 0;
            while (*p == '\n') p++;
            tp->t_summary = p;
            while (*p && !(*p == '\n' && *(p+1) == '\n')) p++;
            if (*p++) *p = 0;
        }
        close(fd);
    } else {
        fprintf(stderr, "sat: %s does not include README file\n", tp->t_path);
        test_errors++;
    }
}

/* return the number of times a char is in a string */
int 
strcount(s, n, c)
char    *s;
int n, c;
{
    int i = 0;
    while (n--) if (*s++ == c) i++;
    return i;
}

/* check if string is part of a path */
int 
strpath(s, p)
char    *s, *p;
{
    int i = 0;
    int ls = strlen(s);
    int lps = strlen(p) - ls;

    while (i <= lps) {
        if ((strncmp(s, p, ls) == 0) && (p[i + ls] == '/' || p[i + ls] == 0))
            return TRUE;
        while (p[i] != 0 && p[i++] != '/');
    }
    return FALSE;
}

/* transcript message for stdout and the log file */
void 
transcript(s, a, b, c, d, e, f, g, h)
char    *s;
int a, b, c, d, e, f, g, h;
{
    time_t now;
    time(&now);
    fmessage(stdout, now, s, a, b, c, d, e, f, g, h);
    if (log_file) fmessage(log_file, now, s, a, b, c, d, e, f, g, h);
}

/* transcript copy from a file */
void 
transcript_copy(fp)
FILE    *fp;
{
    rewind(fp);
    copy_file(fp, stdout);
    rewind(fp);
    if (log_file) copy_file(fp, log_file);
}

/* write timestamped message to file */
void 
fmessage(fp, t, s, a, b, c, d, e, f, g, h)
FILE    *fp;
time_t  t;
char    *s;
int a, b, c, d, e, f, g, h;
{
    char *p;
    p = ctime(&t);
    p[19] = 0;

    fprintf(fp, "[%s] ", &p[11]);
    fprintf(fp, s, a, b, c, d, e, f, g, h);
    fprintf(fp, "\n");
    fflush(fp);
}

/* write usage message to file stream */
void 
usage(fp)
FILE    *fp;
{
    fprintf(fp, "usage: sat [-bchxVD] [-d dir] [-l log] [-m mins] [-o output]\n");
    fprintf(fp, "           [-p part] [-r reps] [tests...]\n\n");
    fprintf(fp, B_MSG);
    fprintf(fp, C_MSG);
    fprintf(fp, D_MSG, "dir");
    fprintf(fp, H_MSG);
    fprintf(fp, L_MSG, "");
    fprintf(fp, M_MSG, "mins");
    fprintf(fp, O_MSG, "");
    fprintf(fp, P_MSG, "part");
    fprintf(fp, R_MSG, "reps");
    fprintf(fp, X_MSG);
    fprintf(fp, V_MSG);
    fprintf(fp, d_MSG);
}

/* display help message */
void 
help(fp)
FILE    *fp;
{
    FILE *hp;

    fprintf(fp, "%s\n\n", VERSION);
    hp = fopen(DEFAULT_HELP, "r");
    if (!hp) {
        fprintf(stderr, "sat: can't open help file %s (%s)\n",
            DEFAULT_HELP, ERR);
        die(2);
    }
    copy_file(hp, fp);
    fclose(hp);
    fprintf(fp, "\n");
    usage(fp);
}

/* display help for each test in list */
void 
help_tests(fp, details)
FILE    *fp;
int details;
{
    int c = 0;
    struct test *tp = tests;

    if (ntests) {
        if (!details) {
            fprintf(fp, LIST_MSG);
            fprintf(fp, "\nDirectory: %s\n", dir);
            fprintf(fp, "Number of tests: %d\n", ntests);
            fprintf(fp, "\nTest             Description\n");
            fprintf(fp, "----             -----------\n");
        }
        while (tp) {
            if (!details) {
                fprintf(fp, "%s", tp->t_name);
                for (c = 16 - strlen(tp->t_name); c > 0; c--) fputc(' ', fp);
                fprintf(fp, " %s\n", tp->t_header);
            } else {
                if (c++) fprintf(fp, SEPARATOR);
                fprintf(fp, "Test: %s\n", tp->t_name);
                fprintf(fp, "%s\n\n", tp->t_header);
                fprintf(fp, "%s\n", tp->t_summary);
            }
            tp = tp->t_next;
        }
    }
}

/* return a pointer to elapsed time string */
char *
elapsed(start, stop)
time_t  start, stop;
{
    static char estring[32];
    int diff = stop - start;
    int hours = diff / 3600;
    int minutes = (diff - (hours * 3600)) / 60;
    int seconds = diff - (hours * 3600) - (minutes * 60);
    char *p = estring;
    sprintf(estring, TIMEFMT, hours, minutes, seconds);
    if (*p == '0') {
        p += 2; /* no hours */
        if (*p == '0') p++; /* and no tens of minutes */
    }
    return p;
}

/* return the temporary stdout file name for a given pid */
char *
tmpout(pid)
pid_t   pid;
{
    static char path[64];
    sprintf(path, "%s/sat.out.%d", TMPPATH, pid);
    return path;
}

/* return the temporary stderr file name for a given pid */
char *
tmperr(pid)
pid_t   pid;
{
    static char path[64];
    sprintf(path, "%s/sat.err.%d", TMPPATH, pid);
    return path;
}

/* return size of given file path */
int
getfilesize(path)
char    *path;
{
    struct stat info;

    if (stat(path, &info) != 0) {
        perror("stat");
        fprintf(stderr, "sat: internal error: can't stat %s\n", path);
        return(0);
    }
    return(info.st_size);
}
