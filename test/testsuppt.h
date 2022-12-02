/******************************************************************************

    (C) Nick Marley, 2001 -

    This software is distributed under the GNU Lesser General Public Licence.
    Please read and understand the comments at the top of vf_iface.h before use!

FILE
    $Workfile$
    $Revision: 1.3 $
    $Author: tilda $
         
ORIGINAL AUTHOR
    Nick Marley

DESCRIPTION
    Platform dependant functions supporting vformat library test harness.

    A typical usage for the file find functions is as follows:

            if (find_first_file(...))
            {
                do
                {
                    something_worthwhile();
                }
                while (find_next_file(...))
                    ;

                close_file_find(...);
            }

    The only reason this exists is 'cos I like to keep the core test harness
    code platform independant.

REFERENCES
    (none)    

MODIFICATION HISTORY
    $Log: testsuppt.h,v $
    Revision 1.3  2002/11/14 20:14:25  tilda
    Minot tidy ups to WIN32 versions.

    Revision 1.2  2002/11/11 17:23:08  monos
    run testing harness on linux systems

    Revision 1.1  2001/10/12 19:02:15  tilda
    Moved generic test functions to new file
 * 
 *******************************************************************************/

#ifndef NORCSID
static const char testsuppt_h_vss_id[] = "$Header: /cvsroot/vformat/build/vformat/testsuppt.h,v 1.3 2002/11/14 20:14:25 tilda Exp $";
#endif

/*=============================================================================*
 Public Includes
 *=============================================================================*/

#if defined(WIN) || defined(WIN32)
#include <Windows.h>
#else
#include <glob.h>
#endif

/*============================================================================*
 Public Defines
 *============================================================================*/

#ifndef _MAX_PATH
#define _MAX_PATH (256)
#endif

/*============================================================================*
 Private Data Types
 *============================================================================*/

typedef struct
{
#if defined(WIN) || defined(WIN32)
    WIN32_FIND_DATA     fd;
    HANDLE              h;
    char                *p_dirname;
#else
    glob_t              h;
#endif
}
file_enum_t;

typedef struct
{
    size_t              size;
    char                *p_data;
}
file_image_t;

/*============================================================================*
 Public Functions
 *============================================================================*/


/*----------------------------------------------------------------------------*
 * NAME
 *      find_first_file()
 * 
 * DESCRIPTION
 *      Locate (initialise search for) first file of indicated pattern in
 *      directory.  If find_first_file() returns TRUE, users should call
 *      do whatever they wer going to do with the file that's located and
 *      then continue their search using find_next_file().
 *
 * RETURNS
 *      TRUE <=> first file found, FALSE else.
 *----------------------------------------------------------------------------*/

extern bool_t find_first_file(
    char *p_name,                   /* Name of file found (output) */
    file_enum_t *p_enum,            /* Pointer to enumerator */
    const char *p_dirname,          /* Directory we're searching in */
    const char *p_pattern           /* Pattern we're searching for */
    );



/*----------------------------------------------------------------------------*
 * NAME
 *      find_next_file()
 * 
 * DESCRIPTION
 *      Continues a search initialised by find_first_file().  Users should
 *      call find_next_file() until it returns FALSE, then close_file_find().
 *
 * RETURNS
 *      TRUE <=> next file found, FALSE else.
 *----------------------------------------------------------------------------*/

extern bool_t find_next_file(
    char *p_name,                   /* Name of file found (output) */
    file_enum_t *p_enum             /* Pointer to enumerator */
    );



/*----------------------------------------------------------------------------*
 * NAME
 *      close_file_find()
 * 
 * DESCRIPTION
 *      Close the file search.  Free memory, close handles etc.  The enumerator
 *      is ready for another call to find_first_file().  User should always
 *      call close_file_find() if find_first_file() returns TRUE or resources
 *      may be leaked.
 *
 * RETURNS
 *      (none)
 *----------------------------------------------------------------------------*/

extern void close_file_find(
    file_enum_t *p_enum             /* Pointer to enumerator */
    );



/*----------------------------------------------------------------------------*
 * NAME
 *      filecomp()
 * 
 * DESCRIPTION
 *      Compare files.
 *
 * RETURNS
 *      0 <=> contents of files is the same, != 0 else.
 *----------------------------------------------------------------------------*/

extern int filecomp(
    const char *p_file1,            /* Name of first file */
    const char *p_file2             /* Name of second file */
    );


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

extern bool_t get_file_image(
    file_image_t *p,                /* Info on file (output) */
    const char *p_name              /* Name of file */
    );


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

extern void free_file_image(
    file_image_t *p                 /* Info on file (output) */
    );
