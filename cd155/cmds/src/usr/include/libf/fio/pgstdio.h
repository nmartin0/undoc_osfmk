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
 *	Copyright 1991, The Portland Group, Incorporated.
 *	All rights reserved.
 *
 *	THE PORTLAND GROUP, INCORPORATED PROPRIETARY INFORMATION
 * This software is supplied under the terms of a license agreement
 * or nondisclosure agreement with The Portland Group and may not be
 * copied or disclosed except in accordance with the terms of that
 * agreement.
 */

/* pgstdio.h - this stubbed version of stdio.h is used instead of the
	correct host version so that the fio lib can be compiled, and then
	linked with versions of libc other than PGI's. */

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned size_t;
#endif

#ifndef NULL
#define NULL 0
#endif

#define EOF	-1

#define SEEK_SET	0
#define SEEK_CUR	1
#define SEEK_END	2

typedef void	*FILE;

#ifdef __STDC__
void	clearerr(FILE *);
int	fclose(FILE *);
int	fflush(FILE *);
int	fgetc(FILE *);
char	*fgets(char *, int, FILE *);
FILE	*fopen(const char *, const char *);
int	fprintf(FILE *, const char *,...);
int	fputc(int, FILE *);
int	fputs(const char *, FILE *);
size_t	fread(void *, size_t, size_t, FILE *);
FILE	*freopen(const char *, const char *, FILE *);
int	fscanf(FILE *, const char *,...);
int	fseek(FILE *, long, int);
long	ftell(FILE *);
size_t	fwrite(const void *, size_t, size_t, FILE *);
char	*gets(char *);
void	perror(const char *);
int	printf(const char *,...);
int	puts(const char *);
int	remove(const char *);
int	rename(const char *, const char *);
void	rewind(FILE *);
int	scanf(const char *,...);
void	setbuf(FILE *, char *);
int	setvbuf(FILE *, char *, int, size_t);
int	sprintf(char *, const char *,...);
int	sscanf(char *, const char *,...);
FILE	*tmpfile(void);
char	*tmpnam(char *);
int	ungetc(int, FILE *);
int	feof(FILE *);
int	ferror(FILE *);

#else

void	clearerr();
int	fclose();
int	fflush();
int	fgetpos();
char	*fgets();
FILE	*fopen();
int	fprintf();
int	fputs();
size_t	fread();
FILE	*freopen();
int	fscanf();
int	fseek();
int	fsetpos();
long	int ftell();
size_t	fwrite();
char	*gets();
void	perror();
int	printf();
int	puts();
int	remove();
int	rename();
void	rewind();
int	scanf();
void	setbuf();
int	setvbuf();
int	sprintf();
int	sscanf();
FILE	*tempfile();
char	*tmpnam();
int	ungetc();
#endif
