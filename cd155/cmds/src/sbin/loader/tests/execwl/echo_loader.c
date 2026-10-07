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
 */

#define	DEFAULT_LOADER	"/sbin/loader"

extern char **environ;

extern int _ldr_crt0_request;
static int *_lcrp = &_ldr_crt0_request;
int _ldr_present = 1;

void print_header(), print_argv(), print_env(), print_auxv();
void auxv_init(), auxv_print();

int
main(argc, argv, envp, auxv)
	char *argv[];
	void *envp, *auxv;
{
	auxv_init();

	print_header();

	(void)printf("\n");
	(void)printf("  argc = %d\n", argc);
	(void)printf("  argv = 0x%x\n", argv);
	(void)printf("  envp = 0x%x\n", envp);
	(void)printf("  auxv = 0x%x\n", auxv);

	print_argv(argv);
	print_env();
	print_auxv();

	exit(0);
	return(0);
}

void
print_header()
{
	char *exec_loader_filename, *auxv_get_exec_loader_filename();
	char *exec_filename, *auxv_get_exec_filename();
	int exec_loader_flags;

	(void)printf("\n\nEcho Loader");
	exec_filename = auxv_get_exec_filename();
	if (auxv_get_exec_loader_flags(&exec_loader_flags)) {
		(void)printf(": called via execve()\n");
		(void)printf("  file = \"%s\"\n", exec_filename);
	} else {
		(void)printf(": called via exec_with_loader()\n");
		exec_loader_filename = auxv_get_exec_loader_filename();
		if (strcmp(exec_loader_filename, DEFAULT_LOADER))
			(void)printf("  loader = \"%s\"\n",
				exec_loader_filename);
		else
			(void)printf("  loader = \"%s\" (DEFAULT_LOADER)\n",
				exec_loader_filename);
		(void)printf("  file = \"%s\"\n", exec_filename);
	}
}

void
print_argv(argv)
	char **argv;
{
	int i;

	if (!(*argv))
		return;
	(void)printf("\n");
	for (i = 0; *argv; i++) {
		(void)printf("  argv[%d] = 0x%x \"%s\"\n", i, argv, *argv);
		argv++;
	}
}

void
print_env()
{
	char **envp;
	int i;

	if (!(envp = environ))
		return;
	(void)printf("\n");
	for (i = 0; *envp; i++) {
		(void)printf("  envp[%d] = 0x%x \"%s\"\n", i, envp, *envp);
		envp++;
	}
}

void
print_auxv()
{
	(void)printf("\n");
	auxv_print("  ");
}
