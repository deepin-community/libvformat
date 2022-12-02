/******************************************************************************

    (C) Nick Marley, 2001 -

    This software is distributed under the GNU Lesser General Public Licence.
    Please read and understand the comments at the top of vf_iface.h before use!

FILE
    $Workfile: vf_access.c $
    $Revision: 1.7 $
    $Author: tilda $
         
ORIGINAL AUTHOR
    Nick Marley

DESCRIPTION
    See accompanying header file.

REFERENCES
    (none)    

MODIFICATION HISTORY
    $Log: testsuppt.c,v $
    Revision 1.7  2002/11/15 09:20:58  tilda
    IID638823 - Don't include unistd.h unless required.

    Revision 1.6  2002/11/15 09:14:59  tilda
    IID638823 - Various portability issues.

    Revision 1.5  2002/11/14 20:14:24  tilda
    Minot tidy ups to WIN32 versions.

    Revision 1.4  2002/11/11 17:23:07  monos
    run testing harness on linux systems

    Revision 1.3  2001/10/24 18:40:25  tilda
    FindClose() not CloseHandle().  Nit.

    Revision 1.2  2001/10/14 19:53:36  tilda
    Group handling.  NO group searching functions.

    Revision 1.1  2001/10/12 19:02:15  tilda
    Moved generic test functions to new file
 * 
 *******************************************************************************/

#ifndef NORCSID
static const char vf_access_c_vss_id[] = "$Header: /cvsroot/vformat/build/vformat/testsuppt.c,v 1.7 2002/11/15 09:20:58 tilda Exp $";
#endif

/*============================================================================*
 ANSI C & System-wide Header Files
 *============================================================================*/

#include <stdlib.h>
#if defined(HAS_UNISTD_H)
#include <unistd.h>
#endif
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#if defined(WIN) || defined(WIN32)
#include <io.h>
#endif

/*============================================================================*
 Interface Header Files
 *============================================================================*/

#include "vformat/vf_iface.h"

/*============================================================================*
 Local Header File
 *============================================================================*/

#include "testsuppt.h"

/*============================================================================*
 Private Defines
 *============================================================================*/
/* None */

/*============================================================================*
 Private Data Types
 *============================================================================*/
/* None */

/*============================================================================*
 Private Function Prototypes
 *============================================================================*/
/* None */

/*============================================================================*
 Private Data
 *============================================================================*/

static int ts_nextfile;

/*============================================================================*
 Public Function Implementations
 *============================================================================*/



/*----------------------------------------------------------------------------*
 * NAME
 *      find_first_file()
 * 
 * DESCRIPTION
 *      Locate (initialise search for) first file of indicated pattern in
 *      directory.  See also find_next_file().
 *
 * RETURNS
 *      TRUE <=> first file found, FALSE else.
 *----------------------------------------------------------------------------*/

bool_t find_first_file(
    char *p_name,                   /* Name of file found (output) */
    file_enum_t *p_enum,            /* Pointer to enumerator */
    const char *p_dirname,          /* Directory we're searching in */
    const char *p_pattern           /* Pattern we're searching for */
    )
{
    bool_t ret = FALSE;
    char pattern[_MAX_PATH];

    sprintf(pattern, "%s//%s", p_dirname, p_pattern);

    if (p_enum)
    {
        memset(p_enum, '\0', sizeof(*p_enum));

#if defined(WIN) || defined(WIN32)
        
        p_enum->h = INVALID_HANDLE_VALUE;

        if (INVALID_HANDLE_VALUE == (p_enum->h = FindFirstFile(pattern, &p_enum->fd)))
        {
            /* No files */
        }
        else
        {
            p_enum->p_dirname = strdup(p_dirname);

            if (p_enum->p_dirname)
            {
                sprintf(p_name, "%s//%s", p_enum->p_dirname, p_enum->fd.cFileName);

                ret = TRUE;
            }
            else
            {
                close_file_find(p_enum);
            }
        }
#else
        if (0 == glob(pattern, GLOB_ERR, NULL, &p_enum->h))
        {
            /* Return first found pathname and increment counter */
            sprintf(p_name, "%s", p_enum->h.gl_pathv[0]);
            ts_nextfile = 1;

            ret = TRUE;
        }
        else
        {
            /* Failed to open directory or to find files */
        }      
#endif
    }

    return ret;
}




/*----------------------------------------------------------------------------*
 * NAME
 *      find_next_file()
 * 
 * DESCRIPTION
 *      Locate (initialise search for) first file of indicated pattern in
 *      directory.  See also find_first_file().
 *
 * RETURNS
 *      TRUE <=> next file found, FALSE else.
 *----------------------------------------------------------------------------*/

bool_t find_next_file(
    char *p_name,                   /* Name of file found (output) */
    file_enum_t *p_enum             /* Pointer to enumerator */
    )
{
    bool_t ret = FALSE;

    if (p_enum)
    {
#if defined(WIN) || defined(WIN32)
        if (INVALID_HANDLE_VALUE == p_enum->h)
        {
            /* No files */
        }
        else
        {
            if (FindNextFile(p_enum->h, &p_enum->fd))
            {
                sprintf(p_name, "%s//%s", p_enum->p_dirname, p_enum->fd.cFileName);

                ret = TRUE;
            }
        }
#else
        if (ts_nextfile < p_enum->h.gl_pathc)
        {
            sprintf(p_name, "%s", p_enum->h.gl_pathv[ts_nextfile++]);
            ret = TRUE;
        }
#endif
    }

    return ret;
}





/*----------------------------------------------------------------------------*
 * NAME
 *      close_file_find()
 * 
 * DESCRIPTION
 *      Close the file search.  Free memory, close handles etc.  The enumerator
 *      is ready for another call to find_first_file().
 *
 * RETURNS
 *      (none)
 *----------------------------------------------------------------------------*/

void close_file_find(
    file_enum_t *p_enum             /* Pointer to enumerator */
    )
{
    if (p_enum)
    {
#if defined(WIN) || defined(WIN32)
        if (INVALID_HANDLE_VALUE == p_enum->h)
        {
            /* Not an open handle */
        }
        else
        {
            FindClose(p_enum->h);
            p_enum->h = INVALID_HANDLE_VALUE;
        }

        if (p_enum->p_dirname)
        {
            free(p_enum->p_dirname);
            p_enum->p_dirname = NULL;
        }
#else
        globfree(&p_enum->h);
#endif
    }
}



/*----------------------------------------------------------------------------*
 * NAME
 *      filecomp()
 * 
 * DESCRIPTION
 *      Compare files.
 *
 * RETURNS
 *      0 <=> fies are th same, != 0 else.
 *----------------------------------------------------------------------------*/

int filecomp(
    const char *p_file1,            /* Name of first file */
    const char *p_file2             /* Name of second file */
    )
{
    int ret = (-1);
    file_image_t f1, f2;

    if (get_file_image(&f1, p_file1))
    {
        if (get_file_image(&f2, p_file2))
        {
            if (f1.size == f2.size)
            {
                ret = memcmp(f1.p_data, f2.p_data, f1.size);
            }
            else
            {
                printf(" Sizes difference :\n  %s (%lu bytes) Vs.\n  %s (%lu bytes)\n",
                    p_file1, (unsigned long)f1.size, p_file2, (unsigned long)f2.size);
            }

            free_file_image(&f2);
        }

        free_file_image(&f1);
    }

    return ret;
}






/*----------------------------------------------------------------------------*
 * NAME
 *      get_file_image()
 * 
 * DESCRIPTION
 *      Load file into memory.
 *
 * RETURNS
 *      TRUE <=> file i/o successful, FALSE else.
 *----------------------------------------------------------------------------*/

bool_t get_file_image(
    file_image_t *p,                /* Info on file (output) */
    const char *p_name              /* Name of file */
    )
{
    bool_t ret = FALSE;

    if (p)
    {
        struct stat buf;

        memset(p, '\0', sizeof(*p));

        if (0 == stat(p_name, &buf))
        {
            p->size = buf.st_size;
            p->p_data = malloc(p->size);

            if (p->p_data)
            {
                FILE *fp;

                fp = fopen(p_name, "rb");

                if (fp)
                {
                    if (p->size == (size_t)read(fileno(fp), p->p_data, p->size))
                    {
                        ret = TRUE;
                    }

                    fclose(fp);
                }
            }
        }
    }

    return ret;
}




/*----------------------------------------------------------------------------*
 * NAME
 *      free_file_image()
 * 
 * DESCRIPTION
 *      Free up resources allocated to a file image.
 *
 * RETURNS
 *      (none)
 *----------------------------------------------------------------------------*/

void free_file_image(
    file_image_t *p                 /* Info on file (output) */
    )
{
    if (p)
    {
        if (p->p_data)
        {
            free(p->p_data);
            p->p_data = NULL;
        }

        p->size = 0l;
    }
}


/*============================================================================*
 Private Function Implementations
 *============================================================================*/
/* None */

/*============================================================================*
 FIN
 *============================================================================*/
