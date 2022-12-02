/******************************************************************************

    (C) Nick Marley, 2001 -

    This software is distributed under the GNU Lesser General Public Licence.
    Please read and understand the comments at the top of vf_iface.h before use!

FILE
    $Workfile: vf_reader.c $
    $Revision: 1.18 $
    $Author: tilda $
         
ORIGINAL AUTHOR
    Nick Marley.

DESCRIPTION
    Code for reading vformat files from disk.  Delegates the real responsibility
    to vf_parser for actually converting chunks of text into the memory data
    structure.

REFERENCES
    (none)    

MODIFICATION HISTORY
 *  $Log: vf_reader.c,v $
 *  Revision 1.18  2002/11/15 09:20:59  tilda
 *  IID638823 - Don't include unistd.h unless required.
 *
 *  Revision 1.17  2002/11/15 09:15:00  tilda
 *  IID638823 - Various portability issues.
 *
 *  Revision 1.16  2002/11/03 18:43:16  tilda
 *  IID619851 - Update and check headers and function prototypes.
 *
 *  Revision 1.15  2002/10/26 16:09:23  tilda
 *  IID629125 - Ensure string functions used are portable.
 *
 *  Revision 1.14  2002/10/08 21:11:36  tilda
 *  Remove common.h.
 *
 *  Revision 1.13  2001/12/13 06:45:35  tilda
 *  IID488021 - Various bugs with quoted printable format.
 *
 *  Revision 1.12  2001/11/05 21:07:19  tilda
 *  Various changes for initial version of vfedit.
 *
 *  Revision 1.11  2001/10/24 18:36:06  tilda
 *  BASE64 bugfixes.  Split reader/writer code. Start create/modify work.
 *
 *  Revision 1.10  2001/10/24 05:32:19  tilda
 *  BASE64 bugfixes - first part
 *
 *  Revision 1.9  2001/10/16 05:50:53  tilda
 *  Debug support for lists of vobjects from single file (ie. a phonebook).
 *
 *  Revision 1.8  2001/10/14 19:53:36  tilda
 *  Group handling.  NO group searching functions.
 *
 *  Revision 1.7  2001/10/13 16:22:08  tilda
 *  Introduce VBINDATA_T and VOBJDATA_T to tidy up internals.
 *
 *  Revision 1.6  2001/10/13 14:58:56  tilda
 *  Tidy up version headers, add vf_strings.h where needed.
 *
 *  Revision 1.5  2001/10/13 14:49:30  tilda
 *  Add string array code to unify handling of names / values.
 *  
 *  Revision 1.4  2001/10/12 16:20:02  tilda
 *  Correctly parse compound quoted printable properties.
 *  
 *  Revision 1.3  2001/10/10 20:53:55  tilda
 *  Various minor tidy ups.
 *  
 *  Revision 1.2  2001/10/09 22:01:59  tilda
 *  Remove older version control comments.
 * 
 *******************************************************************************/

#ifndef NORCSID
static const char vf_reader_c_vss_id[] = "$Header: /cvsroot/vformat/src/vformat/src/vf_reader.c,v 1.18 2002/11/15 09:20:59 tilda Exp $";
#endif

/*=============================================================================*
 ANSI C & System-wide Header Files
 *=============================================================================*/

#include <common/types.h>

#include <stdio.h>
#include <sys/stat.h>

#if defined(HAS_UNISTD_H)
#include <unistd.h>
#endif

#if defined(WIN) || defined(WIN32)
#include <io.h>
#endif


/*============================================================================*
 Interface Header Files
 *============================================================================*/

#include <vformat/vf_iface.h>

/*============================================================================*
 Local Header File
 *============================================================================*/

#include "vf_config.h"
#include "vf_internals.h"
#include "vf_malloc.h"

/*============================================================================*
 Public Data
 *============================================================================*/
/* None */

/*============================================================================*
 Private Defines
 *============================================================================*/

#define PARSEBUFSIZE    (1024)

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
/* None */

/*============================================================================*
 Public Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 * NAME
 *      vf_read_file()
 * 
 * DESCRIPTION
 *      Reads indicated VOBJECT_T file.
 *
 * RETURNS
 *      TRUE <=> read OK, FALSE else.
 *----------------------------------------------------------------------------*/

bool_t vf_read_file(
    VF_OBJECT_T **pp_object,
    const char *p_name
    )
{
    bool_t ret = FALSE;

    if (pp_object)
    {
        FILE *fp;

        fp = fopen(p_name, "rb");

        if (fp)
        {
            char buffer[PARSEBUFSIZE];
            int charsread;
            VF_PARSER_T *p_parser;

            if (vf_parse_init(&p_parser, pp_object))
            {
                do
                {
                    charsread = read(fileno(fp), buffer, sizeof(buffer));

                    if (0 < charsread)
                    {
                        ret = vf_parse_text(p_parser, buffer, charsread);
                    }
                }
                while (ret && (0 < charsread))
                    ;

                if (!vf_parse_end(p_parser))
                {
                    ret = FALSE;
                }
            }

            if (0 == fclose(fp))
            {
                /* OK */
            }
            else
            {
                ret = FALSE;
            }
        }
    }

    return ret;
}




/*---------------------------------------------------------------------------*
 * NAME
 *      vf_set_property_from_file()
 * 
 * DESCRIPTION
 *      Loads the indicated file into memory and sets the indicated property.
 *
 * RETURNS
 *      (none)
 *---------------------------------------------------------------------------*/

bool_t vf_set_property_from_file(
    VF_PROP_T *p_prop,          /* The property */
    vf_encoding_t encoding,     /* Encoding to use */    
    const char *p_filename      /* Source filename */
    )
{
    bool_t ret = FALSE;
    struct stat buf;

    if (0 == stat(p_filename, &buf))
    {
        uint8_t *p_data = vf_malloc(buf.st_size);

        if (p_data)
        {
            FILE *fp = fopen(p_filename, "rb");

            if (fp)
            {
                if ((int)buf.st_size == read(fileno(fp), p_data, buf.st_size))
                {
                    ret = TRUE;
                }

                if (0 == fclose(fp))
                {
                    /* */
                }
                else
                {
                    ret = FALSE;
                }

                if (ret)
                {
                    ret &= vf_set_prop_value(p_prop, p_data, buf.st_size, encoding, TRUE);
                }
            }

            vf_free(p_data);
        }
    }

    return ret;
}


/*============================================================================*
 Private Function Implementations
 *============================================================================*/
/* None */

/*============================================================================*
 End Of File
 *============================================================================*/
