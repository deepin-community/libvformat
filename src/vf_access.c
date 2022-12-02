/****************************************************************************

    (C) Nick Marley, 2001 -

    This software is distributed under the GNU Lesser General Public Licence.
    Please read and understand the comments at the top of vf_iface.h before use!

FILE
    $Revision: 1.23 $
    $Author: tilda $
         
ORIGINAL AUTHOR
    Nick Marley

DESCRIPTION
    Core access functions for VF_OBJECT_Ts.  The library includes other access
    functions (see vf_access_wrappers.c) which are often just alternative slightly
    higher level versions of what's found below.
    
    The intention is to provide a basic interface which covers all the required
    functionality and also some quick'n'easy functions which can be used but don't
    bloat the code (in a static library build anyway).

REFERENCES
    (none)    

MODIFICATION HISTORY
 *  $Log: vf_access.c,v $
 *  Revision 1.23  2002/11/03 18:43:16  tilda
 *  IID619851 - Update and check headers and function prototypes.
 *
 *  Revision 1.22  2002/11/02 18:29:26  tilda
 *  IID485157 - UI does character conversion based on CHARSET property.
 *
 *  Revision 1.21  2002/11/02 08:56:17  tilda
 *  Start of internationalisation changes.
 *
 *  Revision 1.20  2002/10/26 16:09:24  tilda
 *  IID629125 - Ensure string functions used are portable.
 *
 *  Revision 1.19  2002/10/08 21:45:06  tilda
 *  IID620473 - reduce c-runtime dependencies.
 *
 *  Revision 1.18  2002/10/08 21:11:36  tilda
 *  Remove common.h.
 *
 *  Revision 1.17  2002/02/24 17:10:34  tilda
 *  Add API for "is modified" functionality.
 *
 *  Revision 1.16  2002/01/06 16:18:48  tilda
 *  Add dialog box for events / todos.
 *
 *  Revision 1.15  2001/11/18 21:45:58  tilda
 *  Ensure parameters changed to QP are updated correctly.
 *
 *  Revision 1.14  2001/11/17 17:40:29  tilda
 *  Image / sound dialog box now works.
 *
 *  Revision 1.13  2001/11/05 21:07:20  tilda
 *  Various changes for initial version of vfedit.
 *
 *  Revision 1.12  2001/10/24 18:36:06  tilda
 *  BASE64 bugfixes.  Split reader/writer code. Start create/modify work.
 *
 *  Revision 1.11  2001/10/24 05:30:39  tilda
 *  Start work on object create/modify API.
 *
 *  Revision 1.10  2001/10/16 05:50:53  tilda
 *  Debug support for lists of vobjects from single file (ie. a phonebook).
 *
 *  Revision 1.9  2001/10/14 20:42:37  tilda
 *  Addition of group searching.
 *
 *  Revision 1.8  2001/10/14 16:40:05  tilda
 *  Initial testing of access functions.
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
 *  Revision 1.4  2001/10/12 16:20:03  tilda
 *  Correctly parse compound quoted printable properties.
 *  
 *  Revision 1.3  2001/10/10 20:53:56  tilda
 *  Various minor tidy ups.
 *  
 *  Revision 1.2  2001/10/09 22:01:59  tilda
 *  Remove older version control comments.
 * 
 *****************************************************************************/

#ifndef NORCSID
static const char vf_access_c_vss_id[] = "$Header: /cvsroot/vformat/src/vformat/src/vf_access.c,v 1.23 2002/11/03 18:43:16 tilda Exp $";
#endif

/*============================================================================*
 ANSI C & System-wide Header Files
 *============================================================================*/

#include <common/types.h>

/*===========================================================================*
 Interface Header Files
 *===========================================================================*/

#include "vformat/vf_iface.h"

/*===========================================================================*
 Local Header File
 *===========================================================================*/

#include "vf_config.h"
#include "vf_malloc.h"
#include "vf_internals.h"
#include "vf_strings.h"
#include "vf_string_arrays.h"
#include "vf_modified.h"

/*===========================================================================*
 Public Data
 *===========================================================================*/
/* None */

/*===========================================================================*
 Private Defines
 *===========================================================================*/

#define MAXINCREMENT        (5)
#define MAXNUMTAGS          (10)

/*===========================================================================*
 Private Data Types
 *===========================================================================*/
/* None */

/*===========================================================================*
 Private Function Prototypes
 *===========================================================================*/

static bool_t strings_valid_index(
    VPROP_T *p_vprop,           /* The property */
    uint32_t n_string           /* Index required */
    );

static bool_t ensure_value_encoding_tag(
    VPROP_T *p_vprop,           /* The property */
    vf_encoding_t encoding      /* Encoding required */
    );

static bool_t set_prop_value_string(
    VPROP_T *p_vprop,           /* The property */
    uint32_t n_string,          /* Index required */
    const char *p_string        /* String required */
    );

static bool_t set_prop_value_base64(
    VPROP_T *p_vprop,           /* The property */
    const uint8_t *p_data,      /* Pointer to the binary data */
    uint32_t length,            /* Length of the binary data */
    bool_t copy                 /* Copy or keep pointer */
    );




/*===========================================================================*
 Private Data
 *===========================================================================*/
/* None */

/*===========================================================================*
 Public Function Implementations
 *===========================================================================*/


/*---------------------------------------------------------------------------*
 * NAME
 *      vf_get_prop_name()
 * 
 * DESCRIPTION
 *      Build the property name string in the indicated buffer.
 *
 * RETURNS
 *      (none)
 *---------------------------------------------------------------------------*/

void vf_get_prop_name(
    VF_PROP_T *p_prop,          /* The property */
    char *p_buffer,             /* The buffer */
    uint32_t bufsize            /* Size of the buffer */
    )
{
    VPROP_T *p_vprop = (VPROP_T *)p_prop;
    uint32_t i;

    if (p_vprop->p_group)
    {
        p_strcpy(p_buffer, p_vprop->p_group);
        p_strcat(p_buffer, ".");
    }
    else
    {
        p_buffer[0] = '\0';
    }

    for (i = 0;i < p_vprop->name.n_strings;i++)
    {
        if (0 < i)
            p_strcat(p_buffer, ";");

        if (p_vprop->name.pp_strings[i])
        {
            p_strcat(p_buffer, p_vprop->name.pp_strings[i]);
        }
    }
}



/*---------------------------------------------------------------------------*
 * NAME
 *      vf_get_object_type()
 * 
 * DESCRIPTION
 *      Return the type string identifying the indicated vformat object.
 *
 * RETURNS
 *      Ptr to string.
 *---------------------------------------------------------------------------*/

const char *vf_get_object_type(
    VF_OBJECT_T *p_object
    )
{
    const char *p_ret = NULL;

    if (p_object)
    {
        p_ret = ((VOBJECT_T *)p_object)->p_type;
    }

    return p_ret;
}






/*---------------------------------------------------------------------------*
 * NAME
 *      vf_get_prop_value()
 * 
 * DESCRIPTION
 *      Get hold of raw fields associated with the property.  These are of
 *      various types:
 *
 *        VF_ENC_VOBJECT
 *          - *pp_value = pointer to contained VF_OBJECT_T which can be
 *          passed back to any of the object manipulation functions.
 *
 *        VF_ENC_7BIT, VF_ENC_QUOTEDPRINTABLE
 *          - *pp_value = ptr to array of char*, *p_size = number of elts.
 *
 *        VF_ENC_8BIT, VF_BASE64
 *          - *pp_value = ptr to bytes, *p_size = number of bytes
 *
 * RETURNS
 *      TRUE <=> encoding is valid, FALSE else.
 *---------------------------------------------------------------------------*/

bool_t vf_get_prop_value(
    VF_PROP_T *p_prop,          /* The property */
    void **pp_value,            /* Pointer value */
    uint32_t *p_size,           /* Integer value */
    vf_encoding_t *p_encoding   /* Type of return values */
    )
{
    VPROP_T *p_vprop = (VPROP_T *)p_prop;
    bool_t ret = TRUE;

    switch (p_vprop->value.encoding)
    {
    case VF_ENC_VOBJECT:
        if (pp_value)
        {
            *pp_value = p_vprop->value.v.o.p_object;
        }
        break;

    case VF_ENC_7BIT:
    case VF_ENC_QUOTEDPRINTABLE:
        if (pp_value)
        {
            *pp_value = p_vprop->value.v.s.pp_strings;
        }
        if (p_size)
        {
            *p_size = p_vprop->value.v.s.n_strings;
        }
        break;

    case VF_ENC_8BIT:
    case VF_ENC_BASE64:
        if (pp_value)
        {
            *pp_value = p_vprop->value.v.b.p_buffer;
        }
        if (p_size)
        {
            *p_size = p_vprop->value.v.b.n_bufsize;
        }
        break;

    default:
        ret = FALSE;
        break;
    }

    if (p_encoding)
    {
        *p_encoding = p_vprop->value.encoding;
    }

    return ret;
}







/*---------------------------------------------------------------------------*
 * NAME
 *      vf_set_prop_value()
 * 
 * DESCRIPTION
 *      Set values associated with a property.
 *
 *      Passing a value of encoding not the same as the current property
 *      encoding will cause the property contents to be freed prior to
 *      setting the indicated value.
 *
 * RETURNS
 *      TRUE <=> re-allocation success & encoding correct, FALSE else.
 *---------------------------------------------------------------------------*/

bool_t vf_set_prop_value(
    VF_PROP_T *p_prop,          /* The property */
    void *p_value,              /* Pointer to the data */
    uint32_t n_param,           /* Data size or index */
    vf_encoding_t encoding,     /* Encoding in use */
    bool_t copy                 /* Copy the data? */
    )
{
    VPROP_T *p_vprop = (VPROP_T *)p_prop;
    bool_t ret = TRUE;

    if (!copy)
        return FALSE;

    if (encoding == p_vprop->value.encoding)
    {
        /* Leave it as is */
    }
    else
    {
        delete_prop_contents(p_prop, FALSE);

        ensure_value_encoding_tag(p_vprop, encoding);
    }

    switch (p_vprop->value.encoding)
    {
    case VF_ENC_VOBJECT:
        {
            if (p_vprop->value.v.o.p_object != p_value)
            {
                mark_property_modified(p_vprop, TRUE);
            }

            p_vprop->value.v.o.p_object = p_value;
        }
        break;

    case VF_ENC_7BIT:
    case VF_ENC_QUOTEDPRINTABLE:
        {
            ret = set_prop_value_string(p_vprop, n_param, p_value);
        }
        break;

    case VF_ENC_8BIT:
    case VF_ENC_BASE64:
        {
            ret = set_prop_value_base64(p_vprop, p_value, n_param, copy);
        }
        break;

    default:
        ret = FALSE;
        break;
    }

    return ret;
}







/*===========================================================================*
 Private Function Implementations
 *===========================================================================*/




/*---------------------------------------------------------------------------*
 * NAME
 *      set_prop_value_string()
 * 
 * DESCRIPTION
 *      Set the value of a property.
 *
 * RETURNS
 *      TRUE <=> set successfully.
 *---------------------------------------------------------------------------*/

bool_t set_prop_value_string(
    VPROP_T *p_vprop,
    uint32_t n_string,
    const char *p_string
    )
{
    bool_t ret = FALSE;

    if (strings_valid_index(p_vprop, n_string))
    {
        if ((p_vprop->value.v.s.pp_strings[n_string] && !p_string) ||
            (!p_vprop->value.v.s.pp_strings[n_string] && p_string) ||
            (p_vprop->value.v.s.pp_strings[n_string] && p_string && 
                p_stricmp(p_vprop->value.v.s.pp_strings[n_string], p_string)))
        {
            mark_property_modified(p_vprop, TRUE);
        }

        ret = set_string_array_entry(&(p_vprop->value.v.s), p_string, n_string);
    }

    return ret;
}




/*---------------------------------------------------------------------------*
 * NAME
 *      set_prop_value_base64()
 * 
 * DESCRIPTION
 *      Set the value of a property.
 *
 * RETURNS
 *      TRUE <=> set successfully.
 *---------------------------------------------------------------------------*/

bool_t set_prop_value_base64(
    VPROP_T *p_vprop,
    const uint8_t *p_data,      /* Pointer to the binary data */
    uint32_t length,            /* Length of the binary data */
    bool_t copy                 /* Copy or keep pointer */
    )
{
    bool_t ret = FALSE;

    if (copy)
    {
        p_vprop->value.v.b.p_buffer = vf_malloc(length);

        if (p_vprop->value.v.b.p_buffer)
        {
            p_memcpy(p_vprop->value.v.b.p_buffer, p_data, length);
            p_vprop->value.v.b.n_bufsize = length;

            ret = TRUE;
        }
    }
    else
    {
        /* TBD */
    }

    return ret;
}







/*---------------------------------------------------------------------------*
 * NAME
 *      strings_valid_index()
 * 
 * DESCRIPTION
 *      Check request for indicated string.  If it's either in the current
 *      object or represents a reasonable exapansion the request is allowed.
 *
 * RETURNS
 *      TRUE <=> object can include indicated string.
 *---------------------------------------------------------------------------*/

bool_t strings_valid_index(
    VPROP_T *p_vprop,
    uint32_t n_string
    )
{
    bool_t ret = FALSE;

    if (n_string < p_vprop->value.v.s.n_strings)
    {
        /* String in current extent of object */

        ret = TRUE;
    }
    else
    if ((p_vprop->value.v.s.n_strings <= n_string) && (n_string < p_vprop->value.v.s.n_strings + MAXINCREMENT))
    {
        /* Set string within reasonable expansion of object */

        void *p_tmp = vf_realloc(p_vprop->value.v.s.pp_strings, (1 + n_string) * sizeof(char *));

        if (p_tmp)
        {
            uint32_t i;

            p_vprop->value.v.s.pp_strings = p_tmp;

            for (i = p_vprop->value.v.s.n_strings;i < (uint32_t)(1 + n_string);i++)
            {
                p_vprop->value.v.s.pp_strings[i] = NULL;
            }

            p_vprop->value.v.s.n_strings = (1 + n_string);

            ret = TRUE;
        }
    }
    else
    {
        /* Invalid request */
    }

    return ret;
}





/*---------------------------------------------------------------------------*
 * NAME
 *      ensure_value_encoding_tag()
 * 
 * DESCRIPTION
 *      Check/set encoding.
 *
 * RETURNS
 *      TRUE <=> encoding was set successfully.
 *---------------------------------------------------------------------------*/

bool_t ensure_value_encoding_tag(
    VPROP_T *p_vprop,
    vf_encoding_t encoding
    )
{
    int  n;
    uint32_t i;
    bool_t ret = TRUE;

    /*
     * Locate the encoding value.
     */
    for (i = 0, n = (-1);n == (-1) && (i < p_vprop->name.n_strings);i++)
    {
        const char *p_string = p_vprop->name.pp_strings[i];

        if (p_string)
        {
            if (p_stristr(p_string, VFP_ENCODING)
                || p_stristr(p_string, VFP_QUOTEDPRINTABLE)
                || p_stristr(p_string, VFP_BASE64)
                || p_stristr(p_string, VFP_8BIT)
                || p_stristr(p_string, VFP_7BIT))
            {
                n = i;
            }
        }
    }

    /*
     * Remove previous encoding
     */
    if (0 <= n)
    {
        ret = set_string_array_entry(&(p_vprop->name), NULL, n);
    }

    if (ret)
    {
        char *p_enc_string;

        switch (encoding)
        {
        case VF_ENC_QUOTEDPRINTABLE:
            p_enc_string = VFP_QUOTEDPRINTABLE;
            break;

        case VF_ENC_BASE64:
            p_enc_string = VFP_BASE64;
            break;

        default:
            p_enc_string = NULL;
        }

        if (p_enc_string)
        {
            if ((-1) == n)
            {
                ret = add_string_to_array(&(p_vprop->name), p_enc_string);
            }
            else
            {
                ret = set_string_array_entry(&(p_vprop->name), p_enc_string, n);
            }
        }
    }

    if (ret)
    {
        p_vprop->value.encoding = encoding;
    }

    return ret;
}


/*===========================================================================*
 End Of File
 *===========================================================================*/
