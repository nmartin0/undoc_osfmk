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
 
/* Makeconf support for GNU Make. 
Copyright (C) 1988, 1989, 1991 Free Software Foundation, Inc.
This file is part of GNU Make.

GNU Make is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Make is distributed "AS IS" in the hope that it will be useful,
but WITHOUT ANY WARRANTY of any kind; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Make; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/***************************************************************************
 *
 *    Copyright (c) 1988-1992 Intel Corporation
 *
 *    This file is part of pmake, an extension of GNU Make.
 *    Pmake is distributed by the Intel Corporation under the
 *    terms of the GNU General Public License.
 *
 *    Title:
 *        $Id: makeconf.c,v 1.3 1994/11/19 01:33:55 mtm Exp $
 *
 *    Description:
 *        Routines to support Makeconf file processing and separate
 *	  source and object directories.
 *
 **************************************************************************/

#include "make.h"
#include "variable.h"
#include "file.h"
#include "dep.h"
#include "commands.h"

#if defined (USGr3) && !defined (DIRENT)
#define DIRENT
#endif /* USGr3 */
#if defined (Xenix) && !defined (SYSNDIR)
#define SYSNDIR
#endif /* Xenix */

#if defined (POSIX) || defined (DIRENT) || defined (__GNU_LIBRARY__)
#include <dirent.h>
#define direct dirent
#define D_NAMLEN(d) strlen((d)->d_name)
#else /* not POSIX or DIRENT */
#define D_NAMLEN(d) ((d)->d_namlen)
#if defined (USG) && !defined (sgi)
#if defined (SYSNDIR)
#include <sys/ndir.h>
#else /* SYSNDIR */
#include "ndir.h"
#endif /* not SYSNDIR */
#else /* not USG */
#include <sys/dir.h>
#endif /* USG */
#endif /* POSIX or DIRENT or __GNU_LIBRARY__ */

#ifdef PMAKE


/***************************************************************************
 *
 *    Calling Sequence:
 *        set_makeconf_environment ()
 *
 *    Description:
 *        Set up makeconf environment prior to reading makefiles.
 *
 *    Parameters:
 *        None
 *
 *    Returns:
 *        Void.
 *
 **************************************************************************/

void 
set_makeconf_environment()
{
  char *temp;   
  PATH_VAR(cwd);
  PATH_VAR(makedir);
  PATH_VAR(maketop);
  struct variable *vpath;
  struct variable *makepsd;
  struct variable *objecttop;

  /* Set MAKECWD */
  getwd(cwd);
  define_variable("MAKECWD", 7, cwd, o_env, 0, 1);
  
  makepsd = lookup_variable("MAKEPSD", 7);
  if (!makepsd) {
    /* 
     *  No path to source directory, so we have not found a Makeconf 
     *  file yet; we must search for one. 
     */
    define_variable("OBJECTDIR", 9, "", o_env, 0, 1);
    define_variable("SOURCEDIR", 9, "", o_env, 0, 1);
    define_variable("OBJECTTOP", 9, "", o_env, 0, 1);
    define_variable("SOURCETOP", 9, "", o_env, 0, 1);
    define_variable("MAKEDIR", 7, "", o_env, 0, 1);
    define_variable("MAKETOP", 7, "", o_env, 0, 1);
    makeconf = find_makeconf();
    if (!makeconf) 
      /* 
       *  Did not find a Makeconf file, so the current directory will
       *  be our object directory.
       */
      in_objectdir = 1;

  } else {
    /*
     *  We found a path to source directory, so we must reset it
     *  in case we have changed directories since pmake was last
     *  invoked.  First reset the MAKEDIR and MAKETOP variables.
     */
    objecttop = lookup_variable("OBJECTTOP", 9);
    if (!objecttop)
      fatal("Internal error -- OBJECTTOP not correct\n");
    temp = cwd + strlen(objecttop->value);
    strcpy(makedir, temp);
    if (*makedir == '\0')
      strcat(makedir, "/");
    define_variable("MAKEDIR", 7, makedir, o_env, 0, 1);

    *maketop = '\0';
    temp = makedir + strlen(makedir) - 1;
    if (*temp == '/') 
      *temp = '\0';
    temp = makedir;
    while (temp = index(temp, '/')) {
      strcat(maketop, "../");
      temp++;
    }
    define_variable("MAKETOP", 7, maketop, o_env, 0, 1);
 
    objectdir = savestring(cwd, strlen(cwd));
    in_objectdir = 1;

    /* 
     *  Now reset MAKEPSD.
     */
    set_psd();

  }
}



/***************************************************************************
 *
 *    Calling Sequence:
 *        find_makeconf ()
 *
 *    Description:
 *        Search for a Makeconf file on the path from the cwd to
 *	  the root directory.
 *
 *    Parameters:
 *        None.
 *
 *    Returns:
 *        Pointer to path name if found; NULL otherwise.
 *
 **************************************************************************/

char *
find_makeconf ()
{
  PATH_VAR(cwd);
  PATH_VAR(path);
  char *slash;

  /* 
   *  Get the current directory.
   */
  getwd(cwd); 
  strcpy(path, cwd);
  slash = &path[strlen(path)];
  *slash = '/';
  
  do {
    /* 
     *  Loop through each of the directories between the pwd and 
     *  the root directory.
     */
    strcpy(slash+1, "Makeconf");
    if ( file_exists_p(path) ) {
      (void) define_variable("MAKECONF", 8, path, o_env, 0, 1);
      return savestring(path, strlen(path)); 
    }

    *slash = '\0';
    slash = rindex(path, '/');
  } while (slash);

  return (char *)NULL;
}



/***************************************************************************
 *
 *    Calling Sequence:
 *        set_psd ()
 *
 *    Description:
 *        Set the path to source directory based on the pwd and the 
 *	  top of the object and source trees.
 *
 *    Parameters:
 *        None.
 *
 *    Returns:
 *        Void.
 *
 **************************************************************************/

void
set_psd ()
{
  int  i, j;
  int  len;
  char *p, *q;
  char *psd;
  char *temp;
  char **dirs;
  char *maketop;
  char *sourcetop;
  struct variable *v;
  struct variable *makedir;
 
  /*
   *  First get the path for the top of the make tree (i.e. where
   *  the Makeconf file is located).
   */ 
  if (!(v = lookup_variable("MAKECONF", 8)))
    fatal("Internal error -- MAKECONF incorrect.\n");
  maketop = savestring(v->value, strlen(v->value));
  i = strlen(maketop);
  maketop[i-9] = '\0';

  /*
   *  Now get the relative path from the top of the make directory
   *  to the current make directory.  The current make directory 
   *  is one possible location for sources.
   */
  if (!(makedir = lookup_variable("MAKEDIR", 7)))
    fatal("Internal error -- MAKEDIR incorrect.\n");

  /*
   *  If SOURCEDIR is defined, it specifes the top of additional 
   *  source directory trees.
   */
  v = lookup_variable("SOURCEDIR", 9);
  if (!v || (*v->value == '\0')) {
    sourcetop = "";
  } else {
    if (!(v = lookup_variable("SOURCETOP", 9)))
      fatal("Internal error -- SOURCETOP incorrect.\n");
    sourcetop = savestring(v->value, strlen(v->value));
  }

  /*
   *  We are going to parse SOURCEDIR to get the individual 
   *  directories, so allocate a table to hold the directory
   *  names.
   */
  len = strlen(sourcetop) * strlen(makedir->value) + 1;
  dirs = (char **) gmalloc (len * sizeof(char *));

  /*
   *  The first entry in the table is maketop/makedir.
   */
  p = concat(maketop, makedir->value, "");
  q = p + strlen(p) - 1;
  if (*q == '/')
    *q = '\0';
  /*
   *  Call path_to() to get a relative path from the object 
   *  directory to the maketop/makedir directory.
   */
#ifdef TEST
  dirs[0] = p;
#else
  dirs[0] = path_to (p, objectdir);
#endif
  free (p);
  free (maketop);
  i = 1;

  /*
   *  Now parse SOURCEDIR and fill in the rest of the table in
   *  the same manner.
   */
  if (*sourcetop != '\0') {
    p = q = sourcetop + 1;
    while (1) {
      if (*p == ':') {
        *p = '\0';
        dirs[i++] = combine_paths(q, makedir->value);
        *p = ':';
        q = p + 1;
      } else if (*p == '\0') {
        dirs[i++] = combine_paths(q, makedir->value);
        break;
      }
      p ++;
    }
    free (sourcetop);
  }

  /*
   *  Allocate space for the psd string.
   */
  len = 0;
  for (j = 0; j < i; j++)
    len += strlen(dirs[j]) + 1;
  psd = (char *) gmalloc (len);

  /*
   *  Now build the psd string as a colon-separated list.
   */
  strcpy(psd, dirs[0]);
  for (j = 1; j < i; j++) {
    if (*dirs[j-1] != '\0')
      strcat(psd, ":");
    free (dirs[j-1]);
    strcat(psd, dirs[j]);
  }
  free (dirs[j-1]);

  free (dirs);

  /*
   *  Finally, define the MAKEPSD variable.
   */
  define_variable("MAKEPSD", 7, psd, o_env, 0, 1);
  free (psd); 
}



/***************************************************************************
 *
 *    Calling Sequence:
 *        path_to (path, base)
 *
 *    Description:
 *        Compute the relative path from the base directory to the 
 *	  path argument.
 *
 *    Parameters:
 *        path		Absolute path.
 *	  base   	Absolute base directory.
 *
 *    Returns:
 *        String representing relative path from BASE to PATH.
 *
 **************************************************************************/

char *
path_to (path, base)
  char *path;
  char *base;
{
  PATH_VAR(temp);
  char *p, *b; 

  /*
   *  Find the matching part of the path and base directory.
   */
  for (p=path, b=base; *p == *b; p++, b++) 
    if (*p == '\0') 
      return savestring("", 0);

  for ( ; *p != '/'; p--, b--) 
    ;

  /*
   *  Put in enough "../" components to get to the common root.
   */
  *temp = '\0';
  for ( ; *b != '\0'; b++) 
    if (*b == '/') strcat(temp, "../");

  /*
   *  Now append the path from the common root to the "path" 
   *  directory.
   */
  strcat(temp, p+1);

  return savestring(temp, strlen(temp));
}



/***************************************************************************
 *
 *    Calling Sequence:
 *        reset_makeconf_environment ()
 *
 *    Description:
 *        Resets the makeconf environment after reading the Makeconf
 *	  file.
 *
 *    Parameters:
 *        None.
 *
 *    Returns:
 *        Void.
 *
 **************************************************************************/

void
reset_makeconf_environment()
{
  int len;
  int i, j;
  char *p, *q;
  char **dirs;
  char *temp;
  char *makecnf;
  char *objecttop;
  char *sourcedir;
  char *sourcetop;
  int  resetpsd = 0;
  PATH_VAR(cwd);
  PATH_VAR(temppath);
  PATH_VAR(makedir);
  PATH_VAR(maketop);
  struct variable *v;

  /* 
   *  Get the path to the top of the make tree (i.e., the location
   *  of the Makeconf file).
   */
  makecnf = savestring(makeconf, strlen(makeconf));
  i = strlen(makecnf);
  makecnf[i-9] = '\0';
 
  /*  We are somewhere below the Makeconf file in the make tree;
   *  find the relative path from the top of the make tree to 
   *  the pwd and vice versa.
   */ 
  getwd(cwd);
  strcpy(makedir, cwd + strlen(makecnf));
  if (*makedir == '\0')
    strcat (makedir, "/");
  define_variable("MAKEDIR", 7, makedir, o_env, 0, 1);

  strcpy(temppath, makedir);
  *maketop = '\0';
  temp = temppath + strlen(temppath) - 1;
  if (*temp == '/') 
    *temp = '\0';
  temp = temppath;
  while (temp = index(temp, '/')) {
    strcat(maketop, "../");
    temp++;
  }
  define_variable("MAKETOP", 7, maketop, o_env, 0, 1);
 

  /* 
   *  If OBJECTDIR was defined in the Makeconf file, then 
   *  set objecttop to be the absolute path to the top of
   *  the object tree.  If OBJECTDIR was defined relative to 
   *  the location of the Makeconf file, use combine_paths()
   *  to determine the absolute path.  Also, set objectdir
   *  to be objecttop/makedir (the absolute path to the
   *  current object directory).
   */
  v = lookup_variable("OBJECTDIR", 9);
  if (v && (*v->value != '\0') && (v->origin == o_makeconf)) {
    if (*v->value != '/') 
      objecttop = combine_paths(makecnf, v->value);
    else
      objecttop = savestring(v->value, strlen(v->value));
    /*
     *  Since the top of the object tree may be a symbolic 
     *  link, cd to the directory to get the real path.
     */
    if (chdir(objecttop) == 0) {
      getwd(temppath);
      free (objecttop);
      objecttop = savestring(temppath, strlen(temppath));
      chdir(cwd);
    } 
    objectdir = combine_paths(objecttop, makedir);
    resetpsd = 1;
  } else {
    objecttop = savestring(makecnf, strlen(makecnf));
    objectdir = savestring(cwd, strlen(cwd));
    in_objectdir = 1;
  }

  define_variable("OBJECTTOP", 9, objecttop, o_env, 0, 1);
  free (objecttop);

  /*
   *  If SOURCEDIR is defined, parse it into separate directories
   *  and set SOURCETOP to be a colon-separated list of absolute
   *  paths to the top of the source directory trees.
   */
  v = lookup_variable("SOURCEDIR", 9);
  if (v && (*v->value != '\0') && (v->origin == o_makeconf)) {
    /* 
     *  Since we need to parse SOURCEDIR into separate directories,
     *  allocate space for a table of directory names.
     */
    dirs = (char **) gmalloc (strlen(v->value) * sizeof(char *));
    /*
     *  Add a leading colon if there isn't one.
     */
    if (*v->value != ':') 
      sourcedir = concat(":", v->value, "");
    else 
      sourcedir = savestring(v->value, strlen(v->value)); 
   
    /*
     *  Now parse SOURCEDIR.  Use combine_paths() to compute an
     *  absolute path if a SOURCEDIR path is relative to the
     *  location of the Makeconf file.
     */ 
    i = 0;
    p = q = sourcedir + 1;
    while (1) {
      if (*p == ':') {
        *p = '\0';
        if (*q != '/') 
          dirs[i++] = combine_paths(makecnf, q);
        else
          dirs[i++] = savestring(q, strlen(q));
        *p = ':';
        q = p + 1;
      } else if (*p == '\0') {
        if (*q != '/') 
          dirs[i++] = combine_paths(makecnf, q);
        else
          dirs[i++] = savestring(q, strlen(q));
        break;
      }
      p ++;
    }

    /*
     *  We've parsed SOURCEDIR into separate directories; now allocate
     *  the space to put the directories back together in a colon-separated
     *  list.
     */
    len = 0;
    for (j = 0; j < i; j++) 
      len += strlen(dirs[j]) + 1;
    sourcetop = (char *) gmalloc (len);
    *sourcetop = '\0';

    /*
     *  Build the sourcetop list.  
     */
    for (j = 0; j < i; j++) {
      if (*dirs[j] != '\0') {
        strcat(sourcetop, ":");
        strcat(sourcetop, dirs[j]);
      }
      free (dirs[j]);
    }

    free (sourcedir);
    free (dirs);
    resetpsd = 1;     

  } else {
    sourcetop = concat(":", makecnf, "");
  }

  define_variable("SOURCETOP", 9, sourcetop, o_env, 0, 1);
  free (sourcetop);
  free (makecnf);

  /* 
   *  If either the objecttop or sourcetop was changed, we must
   *  recompute the path to source directories.
   */
  if (resetpsd) 
    set_psd();
  
}



/***************************************************************************
 *
 *    Calling Sequence:
 *        fix_vpath ()
 *
 *    Description:
 *        Adds MAKEPSD to vpath.
 *
 *    Parameters:
 *	  None.
 *
 *    Returns:
 *        Void.
 *
 **************************************************************************/

void
fix_vpath ()
{
  int k;
  int len;
  int i, j;
  int n, m;
  char *vp;
  char *temp;
  char *p, *q;
  char **vdirs;
  char **pdirs;
  struct variable *psd;
  struct variable *vpath;
  struct variable *makecpp;
 
  psd = lookup_variable("MAKEPSD", 7);
  vpath = lookup_variable("VPATH", 5);
 
  /*
   *  We want to define VPATH to be the MAKEPSD list, plus
   *  the existing VPATH list, plus all possible combinations
   *  of makepsd/vpath where makepsd is from the MAKEPSD list
   *  and vpath is from the existing VPATH list.
   *
   *  Handle the easy cases first.
   */ 
  if (!psd || *psd->value == '\0') {
    return;
  } else if (!vpath || (*vpath->value == '\0')) {
    define_variable("VPATH", 5, psd->value, o_env, 0, 1);
    temp = concat(":", psd->value, "");
    define_variable("MAKECPP", 7, temp, o_env, 0, 1);
    free (temp);
    return;
  } else if (!makeconf && (vpath->origin == o_env)) {
    define_variable("VPATH", 5, psd->value, o_env, 0, 1);
    temp = concat(":", psd->value, "");
    define_variable("MAKECPP", 7, temp, o_env, 0, 1);
    free (temp);
    return;
  } 

  /*
   *  We have both MAKEPSD and VPATH lists; we must parse each
   *  of them in order to combine them properly.  Allocate space
   *  for tables to hold the individual directory names from the
   *  two lists.
   */
  vdirs = (char **) gmalloc (sizeof(char *) * strlen(vpath->value));
  pdirs = (char **) gmalloc (sizeof(char *) * strlen(psd->value));

  len = 0;
  p = q = vpath->value;
  if (*p == ':') {
    p++;
    q++;
  }

  /*
   *  Parse VPATH into individual directories pointed to by the 
   *  vdirs table.
   */
  i = 0;
  while (1) {
    if (*p == ':') {
      *p = '\0';
      k = strlen(q);
      vdirs[i++] = savestring(q, k);
      len += k + 1;
      *p = ':';
      q = p + 1;
    } else if (*p == '\0') {
      k = strlen(q);
      vdirs[i++] = savestring(q, k);
      len += k + 1;
      break;
    }
    p ++;
  }

  /*
   *  Parse MAKEPSD into individual directories pointed to by the 
   *  pdirs table.
   */
  p = q = psd->value;
  j = 0;
  while (1) {
    if (*p == ':') {
      *p = '\0';
      k = strlen(q);
      pdirs[j++] = savestring(q, k);
      len += k + 1;
      *p = ':';
      q = p + 1;
    } else if (*p == '\0') {
      k = strlen(q);
      pdirs[j++] = savestring(q, k);
      len += k + 1;
      break;
    }
    p ++;
  }

  /*
   *  Figure out how much space we need for the new VPATH, and allocate
   *  it.
   */
  for (n = 0; n < j; n++) {
    k = strlen(pdirs[n]);
    for (m = 0; m < i; m++) {
      if (*vdirs[m] != '/') 
        len += k + strlen(vdirs[m]) + 2;
    }
  }

  vp = (char *) gmalloc (len+1);
  *vp = '\0';

  /*
   *  The MAKEPSD list should be at the start of the new VPATH 
   *  followed by the old VPATH list.
   */
  strcat(vp, psd->value);
  if (*vpath->value != ':') 
    strcat(vp, ":");
  strcat(vp, vpath->value);
 
  /* 
   *  Next add the makepsd/vpath combinations.
   */ 
  for (n = 0; n < j; n++) {
    for (m = 0; m < i; m++) {
      if (*vdirs[m] != '/') {
        strcat(vp, ":");
        temp = combine_paths(pdirs[n], vdirs[m]);
        strcat(vp, temp);
        free (temp);
      }
    }
  }
    
  /* 
   *  Define VPATH.  Also define MAKECPP for export to the compilers.
   */  
  vpath = define_variable("VPATH", 5, vp, vpath->origin, 1, 1);
  vpath->expand_export = 1;

  temp = concat(":", vp, "");
  makecpp = define_variable("MAKECPP", 7, temp, o_env, 1, 1);
  makecpp->expand_export = 1;
  free (temp);
  free (vp);

}


#define DOT_SLASH(x)    ((*(x)=='.') && (*((x)+1)=='/'))
#define DOTDOT_SLASH(x) ((*(x)=='.') && (*((x)+1)=='.') && (*((x)+2)=='/'))


/***************************************************************************
 *
 *    Calling Sequence:
 *        combine_paths (p1, p2)
 *
 *    Description:
 *        Combine a relative with an absolute path, or two
 *	  relative paths.
 *
 *    Parameters:
 *        p1  		First path.  May be relative or absolute.
 *	  p2  		Second path.  Must be relative.  
 *
 *    Returns:
 *        String representing combined path.
 *
 **************************************************************************/

char *
combine_paths (p1, p2)
  char *p1;
  char *p2;
{
  int i;
  char *p, *q;
  PATH_VAR(path);

  /*
   *  Set path to be p1/p2.
   */
  if (!p1) {
    if (!p2) 
      return (char *)0;
    else 
      strcpy (path, p2);
  } else if (!p2) {
    strcpy(path, p1);
  } else {
    strcpy (path, p1);
    strcat (path, "/");
    strcat (path, p2);
  }

  /*
   *  Use p to point to the slashes within path.  Later, q will
   *  be used to point to the last absolute pathname component
   *  encountered within path.  Also, i will count the number of
   *  absolute pathname components encountered within path.
   */
  p = index (path, '/');
  if ((*path != '/') && (*path != '.')) 
    i = 1;
  else
    i = 0;

  while (p) {

    if (DOT_SLASH(p+1)) {
      /* 
       *  Found a "./"; compress the path.
       */
      bcopy (p+2, p, strlen(p+2)+1);
    } else if (DOTDOT_SLASH(p+1)) {
      /* 
       *  Found a "../"; if q is pointing at an absolute pathname
       *  component, compress the path; else, just bump p along.
       */
      if (i) {
        *p = '\0';
        q = rindex (path, '/');
        if (!q) 
          q = path;
        bcopy (p+3, q, strlen(p+3)+1);
        p = q;
        i--;
      } else {
        p += 3;
      }
    } else if (*(p+1) == '/') {
      /*
       *  Compress out extraneous slashes.
       */
      bcopy (p+1, p, strlen(p+1)+1);
    } else if (*(p+1) != '\0') {
      p++;
      p = index (p, '/');
      i++;
    } else {
      *p = '\0';
      p = 0;
    }
  }
  
  return savestring (path, strlen(path));
}
 
#ifdef TEST

char *
combine_paths (p1, p2)
    char *p1;
    char *p2;
{
    int n;
    char  *path;
    char **dirs;
    register char *p, *q;
    register int   i = 1, j = 0;

    path = concat (p1, "/", p2); 
    dirs = (char **) malloc (PATH_MAX * sizeof(char *));
    dirs[0] = path;

    for (p = path; *p; p++) {
        if (*p == '/') {
            dirs[i++] = p + 1;
            *p = '\0';
        }
    }
    n = i;

    for (j = -1, i = 0; i < n; i++) {
        p = dirs[i];
        if (*p++ == '.') {
            if (*p == '\0') {
                *dirs[i] = '\0';
            } else if ((*p++ == '.') && (*p == '\0') && (j >= 0)) {
                *dirs[i] = *dirs[j] = '\0';
                while (j--) {
                    if (*dirs[j]) break;
                }
            }
        } else if (*dirs[i]) {
            j = i;
        }
    }

    p = path = (char *) malloc (PATH_MAX);
    if (*p1 == '/')
        *p++ = '/';

    for (i = 0; i < n; i++) {
        q = dirs[i];
        if (*q == '\0')
            continue;
        while (*q)
            *p++ = *q++;
        *p++ = '/';
    }
    *p-- = '\0';
    if (p > path)
        *p = '\0';

    free (dirs[0]);
    free (dirs);

    return path;
}
#endif

 

/***************************************************************************
 *
 *    Calling Sequence:
 *        fixup_cmdline (line, file)
 *
 *    Description:
 *        Fix command lines by substituting the new names for files
 *	  found on the VPATH for the names in the command.
 *
 *    Parameters:
 *        line		Command line. 
 *	  file		Target file to which command line belongs.
 *
 *    Returns:
 *        String representing fixed command line.
 *
 **************************************************************************/

#define OTHER_TYPE 0
#define CMD_TYPE   1
#define ARG_TYPE   2

char *
fixup_cmdline (line, file)
  char *line;
  struct file *file;
{
  static char special[] = ";\\\n \t\'\"{}<>[]()&$`=|?+#";
  struct dep *d;
  int  i, j, n;
  char **tok;
  int  *type;
  char *p;
  int  intoken;
  int  instring;
  int  nexttype;
  char *buf;
  int  blen;
  int  len;

  /*
   *  If this file has no dependencies, just return.
   */
  i = strlen(line);
  if (!file || !file->deps)
    return savestring(line, i);

  /*
   *  File has dependencies, so we must search the command line for them.
   *  Allocate token and type tables to use in parsing the command line.
   */
  type = (int *) gmalloc ((i+1) * sizeof(int));
  tok = (char **) gmalloc ((i+1) * sizeof (char *));

  tok[0] = line;
  type[0] = OTHER_TYPE;

  /*
   *  Skip the nonimportant parts of the command line.
   */
  for (p = line; *p != '\0'; ++p) {
    if ((*p != '@') && (*p != '+') && (*p != '-') && (!isspace(*p)))
      break;
  }

  /*
   *  Parse the command line and fill in the token and type tables.
   */
  i = 1;
  instring = 0;
  intoken = 0;
  nexttype = CMD_TYPE;

  for ( ; *p != '\0'; ++p) {
    if (instring) {
      if (*p == '\'')
        instring = 0;
    } else if (index(special, *p) != 0) {
      if (intoken) {
        type[i] = OTHER_TYPE;
        tok[i++] = p;
        intoken = 0;
      }
      if (*p == '\'')
        instring = 1;
      else if (*p == ';')
        nexttype = CMD_TYPE;
      else if ((*p == '\n') && (*(p-1) != '\\'))
        nexttype = CMD_TYPE;
    } else if (!intoken) {
        type[i] = nexttype;
        tok[i++] = p;
        intoken = 1;
        nexttype = ARG_TYPE;
    }
  }
  tok[i] = p;

  /*
   *  Allocate space for the new command line.
   */
  len = 0;
  blen = 200;
  buf = (char *) gmalloc (blen);
  *buf = '\0';

  /*
   *  Build the new command line.
   */
  for (j = 0; j < i; j++) {
    n = tok[j+1] - tok[j];
    switch (type[j]) {
      case OTHER_TYPE:
      case CMD_TYPE:
        /*  
         *  We don't need to examine anything but arguments,
         *  so skip commands and other stuff.
         */
        len += n;
        if (len >= blen)
          buf = (char *) xrealloc (buf, blen = len * 2);
        strncat (buf, tok[j], n);
        break;
      case ARG_TYPE:
        /*
         *  We found an argument, loop though the file's dependencies
         *  looking for a matching name.
         */
        for (d = file->deps; d != 0; d = d->next) {
          p = old_dep_name(d);
          if ((strncmp(tok[j], p, n) == 0) && (*(p+n) == '\0')) {
            /*
             *  Found a match; substitute the new dependency name
             *  for the argument on the command line.
             */
            p = dep_name(d);
            len += strlen(p);
            if (len >= blen)
              buf = (char *) xrealloc (buf, blen = len * 2);
            strcat (buf, p);
            break;
          }
        }

        if (d == 0) {
          /*
           *  No match; just copy the argument to the new command
           *  line.
           */
          len += n;
          if (len >= blen)
            buf = (char *) xrealloc (buf, blen = len * 2);
          strncat (buf, tok[j], n);
        }
        break;
    }
  }
  
  free (tok);
  free (type);
  return buf;

}


/***************************************************************************
 *
 *    Calling Sequence:
 *        old_dep_name (dep)
 *
 *    Description:
 *        Resolves the old name of a dependency, prior to vpath search.  
 *
 *    Parameters:
 *        dep 		Pointer to dependency structure.
 *
 *    Returns:
 *        Old dependency name.  
 *
 **************************************************************************/

char *
old_dep_name (dep)
     struct dep *dep;
{
  if (dep->origname != 0)
    return dep->origname;
  else 
    return "";
}



/***************************************************************************
 *
 *    Calling Sequence:
 *        change_dir (dir)
 *
 *    Description:
 *        Change to directory dir, creating a path if one does not exist.
 *
 *    Parameters:
 *        dir 		Directory to change to.
 *
 *    Returns:
 *        Void.
 *
 **************************************************************************/

void
change_dir (dir)
  char   *dir;
{
  struct stat s;
  PATH_VAR(temp);
  int  mode = umask(022);
  char *path, *p;
  char *t;

  /*
   *  Add a slash to the end of dir.
   */
  p = dir + strlen(dir) - 1;
  if (*p  == '/')
    path = savestring(dir, strlen(dir));
  else
    path = concat(dir, "/", "");
 
  /*
   *  For each directory component in the dir path, see if it 
   *  exists and make it if it does not.
   */ 
  for (t=temp, p=path; *p != 0; ) {
    *t++ = *p++;
    if (*p == '/') {
      *t = 0;
      if (stat(temp, &s) == -1) {
        if (mkdir(temp, mode^0777) == -1) { 
          pfatal_with_name(temp);
        }
      }
    } 
  } 

  /*
   *  Change to the new directory.
   */
  chdir(dir);
  if (!silent_flag && !question_flag)
    printf("cd %s\n", dir);

  strcpy(currentdir, dir);

  umask(mode);
  free (path);

}



/***************************************************************************
 *
 *    Calling Sequence:
 *        debug_makeconf()
 *
 *    Description:
 *        Print some variables to help in debugging the "Makeconf
 *	  functionality".
 *
 *    Parameters:
 *        none
 *
 *    Returns:
 *        void
 *
 **************************************************************************/

void
debug_makeconf()
{
  struct variable *v;
  
  printf("============================================================\n");

  if (makeconf)
    printf("makeconf = %s\n", makeconf);
  else
    printf("makeconf not defined\n");

  v = lookup_variable("MAKECWD", 7);
  if (v)
    printf("MAKECWD = %s\n", v->value);
  else
    printf("MAKECWD not defined\n");

  v = lookup_variable("MAKECONF", 8);
  if (v)
    printf("MAKECONF = %s\n", v->value);
  else
    printf("MAKECONF not defined\n");

  v = lookup_variable("MAKEPSD", 7);
  if (v)
    printf("MAKEPSD = %s\n", v->value);
  else
    printf("MAKEPSD not defined\n");

  v = lookup_variable("MAKEDIR", 7);
  if (v)
    printf("MAKEDIR = %s\n", v->value);
  else
    printf("MAKEDIR not defined\n");

  v = lookup_variable("MAKETOP", 7);
  if (v)
    printf("MAKETOP = %s\n", v->value);
  else
    printf("MAKETOP not defined\n");

  v = lookup_variable("OBJECTDIR", 9);
  if (v)
    printf("OBJECTDIR = %s\n", v->value);
  else
    printf("OBJECTDIR not defined\n");

  v = lookup_variable("OBJECTTOP", 9);
  if (v)
    printf("OBJECTTOP = %s\n", v->value);
  else
    printf("OBJECTTOP not defined\n");

  v = lookup_variable("SOURCEDIR", 9);
  if (v)
    printf("SOURCEDIR = %s\n", v->value);
  else
    printf("SOURCEDIR not defined\n");

  v = lookup_variable("SOURCETOP", 9);
  if (v)
    printf("SOURCETOP = %s\n", v->value);
  else
    printf("SOURCETOP not defined\n");

  v = lookup_variable("VPATH", 5);
  if (v)
    printf("VPATH = %s\n", v->value);
  else
    printf("VPATH not defined\n");

  v = lookup_variable("MAKECPP", 7);
  if (v)
    printf("MAKECPP = %s\n", v->value);
  else
    printf("MAKECPP not defined\n");

  v = lookup_variable("CPATH", 5);
  if (v)
    printf("CPATH = %s\n", v->value);
  else
    printf("CPATH not defined\n");

  v = lookup_variable("LPATH", 5);
  if (v)
    printf("LPATH = %s\n", v->value);
  else
    printf("LPATH not defined\n");

  printf("============================================================\n");

}


#endif


