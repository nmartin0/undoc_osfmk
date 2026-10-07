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
 *      Copyright 1994  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/bin/coreinfo/coreinfo.c,v 1.4 1994/11/19 01:21:04 mtm Exp $
 * History:
 * $Log: coreinfo.c,v $
 * Revision 1.4  1994/11/19  01:21:04  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/09/20  14:42:39  johannes
 * changed column header to Date/Time of column Time/Date
 * changed date output format to have a leading zero for day
 *
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: 11002
 *  Testing: test with modified actual date
 *  Module(s): cmds_libs/src/usr/bin/coreinfo.c
 *
 * Revision 1.2  1994/09/13  15:21:29  johannes
 * implementation of printing the fault address
 *
 *  Reviewer: Stefan Tritscher
 *  Risk: Low
 *  Benefit or PTS #: 10707
 *  Testing: developer tests [corefile EAT have to be adapted first]
 *  Module(s): cmds_libs/src/usr/bin/coreinfo.c
 *
 * Revision 1.1  1994/08/02  16:33:55  johannes
 * integrated new coreinfo command
 *
 *  Reviewer: Stefan Tritscher
 *  Risk: L
 *  Benefit or PTS #: new command /usr/bin/coreinfo
 *  Testing: corefile EAT (no reference case, but should be ok), developer
 *  Modified files: cmds_libs/src/usr/bin/Makefile
 *  New directory:  cmds_libs/src/usr/bin/coreinfo
 *  New files:      cmds_libs/src/usr/bin/coreinfo/Makefile
 *                  cmds_libs/src/usr/bin/coreinfo/coreinfo.c
 *                  cmds_libs/src/usr/bin/coreinfo/coreinfo.msg
 *
 *
 */

#define TNC

#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/core.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <dirent.h>

void Usage();
int do_corefile();
int do_coredir();
void print_title();
void print_line();
int read_line();
int read_struct();
char *convert_signal();
char *convert_type();
void set_exec_path();

struct summary_line {
    struct timeval time;
    pid_t pid;
    long node;
    long ptype;
    int signal;
    unsigned int location;
    boolean_t main_thread;
    int type;
    char exec_path[MAXPATHLEN];
};

int compare_time(line1, line2)
    struct summary_line **line1;
    struct summary_line **line2;
{
    long sec_diff = (*line1)->time.tv_sec - (*line2)->time.tv_sec;
    
    if (sec_diff != 0)
        return sec_diff;
    else
        return (*line1)->time.tv_usec - (*line2)->time.tv_usec;
}

boolean_t other_thread = FALSE;		/* there is a faulting thread, */
					/* which is not a main user thread */

/******************************** coreinfo ********************************
 *        
 *      Calling Sequence:
 *             NONE
 *
 *      Description:
 *             Display summary information of a core file or core directory.
 *  
 *      Parameters:
 *             NONE
 *
 *      Returns:
 *             status
 *
 *
 */

int main(argc, argv)
    int argc;
    char *argv[];
{
    char file[MAXPATHLEN];
    char cwd[MAXPATHLEN];
    struct stat	file_stat;
    int ret;

    /*
     * Check arguments.
     */
    if (argc > 2 ||
	argc == 2 && argv[1][0] == '-') {
	Usage();
	return 1;
    }

    /*
     * Get current working directory.
     */
    if (getwd(cwd) == NULL)
	strcpy(cwd, ".");

    /*
     * Construct absolute file name.
     */
    file[0] = '\0';
    
    if (argc == 2) {
	if (argv[1][0] != '/') {
	    strcat(file, cwd);
            if (file[strlen(file) - 1] != '/')
	        strcat(file, "/");
	}
        strcat(file, argv[1]);
    }
    else { /* no argument */
	char *core_path;

	if ((core_path = getenv("CORE_PATH")) == NULL) {
	    strcat(file, cwd);
	}
	else {
	    if (core_path[0] != '/') {
		strcat(file, cwd);
                if (file[strlen(file) - 1] != '/')
	            strcat(file, "/");
	    }
	    strcat(file, core_path);
	}
        if (file[strlen(file) - 1] != '/')
	    strcat(file, "/");
        strcat(file, "core");
    }

    /*
     * Determine type of file.
     */
    if (stat(file, &file_stat) != 0) {
	fprintf(stderr, "coreinfo: stat %s ", file);
	fflush(stderr);
	perror("failed");
	return 1;
    }

    /*
     * Display summary information.
     */
    if (S_ISREG(file_stat.st_mode))
	ret = do_corefile(file);
    else if (S_ISDIR(file_stat.st_mode))
	ret = do_coredir(file);
    else {
	fprintf(stderr, "coreinfo: unknown type of file %s\n", file);
	return 1;
    }
    
    if (other_thread)
        printf("\n* Faulting thread was not the main user thread\n");

    return ret;
}


/********************************  Usage  ********************************
 *
 *      Calling Sequence:
 *             Usage();
 *
 *      Description:
 *             Prints the Usage message
 *
 *      Parameters:
 *             NONE
 *
 *      Returns:
 *             NONE
 *
 *      Called by:
 *             coreinfo
 *
 */

void Usage()
{
    fprintf(stderr,"coreinfo: syntax error\n");
    fprintf(stderr,"Usage: coreinfo [{corefile | coredir}]\n");
}


/********************************  do_corefile  *****************************
 *
 *      Calling Sequence:
 *             do_corefile(file);
 *
 *      Description:
 *             Display summary information of a core file
 *
 *      Parameters:
 *             file name
 *
 *      Returns:
 *             status
 *
 *      Called by:
 *             coreinfo
 *
 */

int do_corefile(file)
    char *file;
{
    int fd, is_nx;
    struct core_hdr header;
    struct summary_line line;

    /*
     * Open core file.
     */
    if ((fd = open(file, O_RDONLY)) <= 0) {
	fprintf(stderr, "coreinfo: open %s ", file);
	fflush(stderr);
	perror("failed");
	return 1;
    }

    /*
     * Read header of core file.
     */
    if (read_struct(fd, &header, sizeof header, 0, file))
        return 1;

    /*
     * Check if it's a core file.
     */
    if (header.c_magic != CORE_MAGIC) {
	fprintf(stderr, "coreinfo: %s not a core file\n", file);
	return 1;
    }

    /*
     * Check if it's a NX application.
     */
    is_nx = (header.c_numnodes != -1);
    
    /*
     * Read a summary line.
     */
    if (read_line(fd, &line, &header, is_nx, file))
        return 1;
    
    /*
     * Print the summary.
     */
    printf("Summary information for file: %s\n", file);
    if (is_nx)
	printf("Number of nodes: %d\n", header.c_numnodes);
    print_title(is_nx);
    print_line(is_nx, &line);
    
    return 0;
}


/********************************  do_coredir  *****************************
 *
 *      Calling Sequence:
 *             do_coredir(file);
 *
 *      Description:
 *             Display summary information of a core directory
 *
 *      Parameters:
 *             file name
 *
 *      Returns:
 *             status
 *
 *      Called by:
 *             coreinfo
 *
 */

int do_coredir(dir)
    char *dir;
{
    DIR *dirp;
    struct dirent *entry;
    int fd, is_nx;
    char file[MAXPATHLEN];
    struct core_hdr header;
    int current, num_lines;
    struct summary_line **summary;

    /*
     * Open core directory.
     */
    if ((dirp = opendir(dir)) == NULL) {
	fprintf(stderr, "coreinfo: opendir %s ", dir);
	fflush(stderr);
	perror("failed");
	return 1;
    }

    /*
     * Allocate memory for first summary line.
     */
    summary = (struct summary_line **) 
              malloc(sizeof(struct summary_line *));
    if (summary == NULL) {
        fprintf(stderr, "coreinfo: malloc failed\n");
        return 1;
    }
    
    summary[0] = (struct summary_line *)
                 malloc(sizeof(struct summary_line));
    if (summary[0] == NULL) {
        fprintf(stderr, "coreinfo: malloc failed\n");
        return 1;
    }
    
    current = 0;

    for (;;) {
        int i, is_core;

	/*
	 * Read next core directory entry.
	 */
	if ((entry = readdir(dirp)) == NULL)
	    if (current > 0)
		break;
	    else {
		fprintf(stderr, "coreinfo: no core file found in %s\n", dir);
		return 1;
	    }

	/*
	 * Check name of file found in core directory.
	 */
	if (strlen(entry->d_name) < 6)
	    continue;
	if (strncmp(entry->d_name, "core.", 5) != 0)
	    continue;
	is_core = 1;
	for (i = 5; i < strlen(entry->d_name); i++)
	    if (entry->d_name[i] < '0' || entry->d_name[i] > '9')
		is_core = 0;
	if (! is_core)
	    continue;

	/*
	 * Construct absolute name of core file.
	 */
	strcpy(file, dir);
	strcat(file, "/");
	strcat(file, entry->d_name);

	/*
	 * Open core file.
	 */
	if ((fd = open(file, O_RDONLY)) <= 0) {
	    fprintf(stderr, "coreinfo: open %s ", file);
	    fflush(stderr);
	    perror("failed");
	    continue;
	}

	/*
	 * Read header of core file.
	 */
	if (read_struct(fd, &header, sizeof header, 0, file)) {
   	    close(fd);
	    continue;
	}

	/*
	 * Check if it's a core file.
	 */
	if (header.c_magic != CORE_MAGIC) {
	    fprintf(stderr, "coreinfo: %s not a core file\n", file);
   	    close(fd);
	    continue;
	}

	/*
	 * Check if it's a NX application, if first core file.
	 */
	if (current == 0)
	    is_nx = (header.c_numnodes != -1);
    
	/*
	 * Read a summary line.
	 */
	if (read_line(fd, summary[current], &header, is_nx, file)) {
   	    close(fd);
	    continue;
	}
	
	current++;
	
	/*
	 * Allocate memory for next summary line.
	 */
	summary = (struct summary_line **) 
		  realloc(summary,
		          sizeof(struct summary_line *) * (current + 1));
	if (summary == NULL) {
	    fprintf(stderr, "coreinfo: realloc failed\n");
	    close(fd);
	    break;
	}
	
	summary[current] = (struct summary_line *)
		     	   malloc(sizeof(struct summary_line));
	if (summary[current] == NULL) {
	    fprintf(stderr, "coreinfo: malloc failed\n");
	    close(fd);
	    break;
	}
	
	close(fd);
    }
    
    num_lines = current;
    
    closedir(dirp);

    /*
     * Sort lines in the summary.
     */
    qsort((void *)summary, 
          (size_t)num_lines, 
          (size_t)sizeof(struct summary_lines*), 
          (int(*)(void *, void *))compare_time);
    
    /*
     * Print the summary.
     */
    printf("Summary information for directory: %s\n", dir);
    if (is_nx)
	printf("Number of nodes: %d\n", header.c_numnodes);
    print_title(is_nx);
    for (current = 0; current < num_lines; current++)
        print_line(is_nx, summary[current]);
    
    return 0;
}

/********************************  print_title  *****************************
 *
 *      Calling Sequence:
 *             print_title(is_nx);
 *
 *      Description:
 *             Print title of summary table
 *
 *      Parameters:
 *             flag if NX application
 *
 *      Returns:
 *             NONE
 *
 *      Called by:
 *             do_corefile, do_coredir
 *
 */

void print_title(nx_flag)
    int nx_flag;
{
    printf("Date/Time");
    printf("\tPid");
    if (nx_flag) {
	printf("\tNode");
	printf("\tPtype");
    }
    printf("\tSignal");
    printf("\tLocation    ");
    printf("Type  ");
    printf("Executable\n");

    printf("---------");
    printf("\t---");
    if (nx_flag) {
	printf("\t----");
	printf("\t-----");
    }
    printf("\t------");
    printf("\t--------    ");
    printf("----  ");
    printf("----------\n");
}


/********************************  print_line  *****************************
 *
 *      Calling Sequence:
 *             print_line(is_nx, &line);
 *
 *      Description:
 *             Print a line of summary table
 *
 *      Parameters:
 *             flag if NX application
 *             summary line to print
 *
 *      Returns:
 *             NONE
 *
 *      Called by:
 *             do_corefile, do_coredir
 *
 */

#define TIME_BUF 13

void print_line(nx_flag, line)
    int nx_flag;
    struct summary_line *line;
{
    struct tm *time_str;
    char time_buf[TIME_BUF];
    
    /*
     * Prepare time information.
     */
    time_str = localtime(&line->time.tv_sec);
    strftime(time_buf, (size_t)TIME_BUF, "%sD %sT", time_str);
    
    /*
     * Print the summary line.
     */
    printf("%s", time_buf);
    printf("\t%d", line->pid);
    if (nx_flag) {
        printf("\t%d", line->node);
        printf("\t%d", line->ptype);
    }
    printf("\t%s", convert_signal(line->signal));
    printf("\t0x%08x", line->location);
    if (line->main_thread)
        printf("  ");
    else
        printf("* ");
    printf("%s ", convert_type(line->type));
    printf("%s\n", line->exec_path);
}


/********************************  read_line  *****************************
 *
 *      Calling Sequence:
 *             read_line(fd, &line, &header, nx_flag, file);
 *
 *      Description:
 *             Read a summary line
 *
 *      Parameters:
 *             file descriptor
 *             summary line
 *             header of core file
 *             flag if NX application
 *             file name for error message
 *
 *      Returns:
 *             status
 *
 *      Called by:
 *             do_corefile, do_coredir
 *
 */

int read_line(fd, line, header, nx_flag, file)
    int	fd;
    struct summary_line *line;
    struct core_hdr *header;
    int nx_flag;
    char *file;
{
    struct core_proc_info proc_info;
    struct i860_thread_state thread_state;
    char prg_name[MAXPATHLEN];
    char root_path[MAXPATHLEN];
    char cwd_path[MAXPATHLEN];
    int ret;
    
    /*
     * Read needed information from core file.
     */
    if (ret = read_struct(fd, &proc_info, sizeof proc_info, 
			  header->c_procinfo, file))
	return ret;
    if (ret = read_struct(fd, &thread_state, sizeof thread_state, 
			  header->c_firstthread + 
			  header->c_activethread * sizeof thread_state, 
			  file))
	return ret;
    if (ret = read_struct(fd, prg_name, proc_info.c_prglen, 
    			  proc_info.c_prgname, file))
	return ret;
    if (ret = read_struct(fd, root_path, proc_info.c_rootlen, 
    			  proc_info.c_rootname, file))
	return ret;
    if (ret = read_struct(fd, cwd_path, proc_info.c_cwdlen, 
    			  proc_info.c_cwdname, file))
	return ret;

    /*
     * Fill a summary line.
     */
    line->time = header->c_timdat;
    line->pid = proc_info.c_pid;
    if (nx_flag) {
	line->node = header->c_node;
	line->ptype = header->c_ptype;
    }
    line->signal = header->c_signo;
    line->location = thread_state.pc;
    line->main_thread = (header->c_activethread == 0);
    if (! line->main_thread)
        other_thread = TRUE;
    line->type = header->c_type;
    set_exec_path(line->exec_path, prg_name, root_path, cwd_path);

    return 0;
}


/********************************  read_struct  *****************************
 *
 *      Calling Sequence:
 *             read_struct(fd, &structure, sizeof structure, offset, file);
 *
 *      Description:
 *             Read in a structure
 *
 *      Parameters:
 *             file descriptor
 *             pointer to & length of structure
 *             offset in file
 *             file name for error message
 *
 *      Returns:
 *             status
 *
 *      Called by:
 *             do_corefile, do_coredir, read_line
 *
 */

int read_struct(fd, ptr, len, offset, file)
    int	fd;
    char *ptr;
    int	len;
    off_t offset;
    char *file;
{
    int ret;
    
    if ((ret = lseek(fd, offset, SEEK_SET)) !=  offset) {
	if (ret < 0) {
	    fprintf(stderr, "coreinfo: lseek %s ", file);
	    fflush(stderr);
	    perror("failed");
	}
	else 
	    fprintf(stderr, "coreinfo: %s not a core file\n", file);
	return -1;
    }

    if ((ret = read(fd, ptr, len)) != len) {
	if (ret < 0) {
	    fprintf(stderr, "coreinfo: read %s ", file);
	    fflush(stderr);
	    perror("failed");
	}
	else 
	    fprintf(stderr, "coreinfo: %s not a core file\n", file);
	return -1;
    }
    
    return 0;
}


/********************************  convert_signal  *****************************
 *
 *      Calling Sequence:
 *             convert_signal(signo);
 *
 *      Description:
 *             Convert signal number into signal text
 *
 *      Parameters:
 *             signal number
 *
 *      Returns:
 *             signal text
 *
 *      Called by:
 *             read_line
 *
 */

char *convert_signal(signo)
    int signo;
{
    switch (signo) {
    case SIGHUP:	return "SIGHUP";
    case SIGINT:	return "SIGINT";
    case SIGQUIT:	return "SIGQUIT";
    case SIGILL:	return "SIGILL";
    case SIGTRAP:	return "SIGTRAP";
    case SIGABRT:	return "SIGABRT";
    case SIGEMT:	return "SIGEMT";
    case SIGFPE:	return "SIGFPE";
    case SIGKILL:	return "SIGKILL";
    case SIGBUS:	return "SIGBUS";
    case SIGSEGV:	return "SIGSEGV";
    case SIGSYS:	return "SIGSYS";
    case SIGPIPE:	return "SIGPIPE";
    case SIGALRM:	return "SIGALRM";
    case SIGTERM:	return "SIGTERM";
    case SIGURG:	return "SIGURG";
    case SIGSTOP:	return "SIGSTOP";
    case SIGTSTP:	return "SIGTSTP";
    case SIGCONT:	return "SIGCONT";
    case SIGCHLD:	return "SIGCHLD";
    case SIGTTIN:	return "SIGTTIN";
    case SIGTTOU:	return "SIGTTOU";
    case SIGIO:		return "SIGIO";
    case SIGXCPU:	return "SIGXCPU";
    case SIGXFSZ:	return "SIGXFSZ";
    case SIGVTALRM:	return "SIGVTALRM";
    case SIGPROF:	return "SIGPROF";
    case SIGWINCH:	return "SIGWINCH";
    case SIGINFO:	return "SIGINFO";
    case SIGUSR1:	return "SIGUSR1";
    case SIGUSR2:	return "SIGUSR2";
#ifdef TNC
    case SIGMIGRATE:	return "SIGMIGRATE";
#endif /* TNC */
    default:		return "???";
    }
}


/********************************  convert_type  *****************************
 *
 *      Calling Sequence:
 *             convert_type(type);
 *
 *      Description:
 *             Convert core type into type text
 *
 *      Parameters:
 *             core type
 *
 *      Returns:
 *             type text
 *
 *      Called by:
 *             read_line
 *
 */

char *convert_type(type)
    int type;
{
    switch (type) {
    case CORE_TYPE_FULL:	return "FULL ";
    case CORE_TYPE_TRACE:	return "TRACE";
    default:			return "???  ";
    }
}


/********************************  set_exec_path  *****************************
 *
 *      Calling Sequence:
 *             set_exec_path(exec_path, prg_name, root_path, cwd_path);
 *
 *      Description:
 *             Construct absolute path of program dumped core
 *
 *      Parameters:
 *             constructed path
 *             program name as given on exec call
 *             root path when program been execed
 *             cwd path when program been execed
 *
 *      Returns:
 *             NONE
 *
 *      Called by:
 *             read_line
 *
 */

void set_exec_path(exec_path, prg_name, root_path, cwd_path)
    char *exec_path;
    char *prg_name;
    char *root_path;
    char *cwd_path;
{
    exec_path[0] = '\0';
    
    /*
     * No program name, nothing to do.
     */
    if (prg_name[0] == '\0')
        return;

    /*
     * Append root path.
     */
    strcat(exec_path, root_path);    
    if (exec_path[strlen(exec_path) - 1] != '/')
	strcat(exec_path, "/");
    
    /*
     * Append cwd path, if necessary.
     */
    if (prg_name[0] != '/') {
	strcat(exec_path, &cwd_path[1]);
        if (exec_path[strlen(exec_path) - 1] != '/')
	    strcat(exec_path, "/");
    }
    
    /*
     * Append program name.
     */
    if (prg_name[0] == '/')
        strcat(exec_path, &prg_name[1]);
    else
        strcat(exec_path, prg_name);
}
