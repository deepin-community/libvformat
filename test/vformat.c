/******************************************************************************

    (C) Nick Marley, 2001 -

    This software is distributed under the GNU Lesser General Public Licence.
    Please read and understand the comments at the top of vf_iface.h before use!

FILE
    $Workfile$
    $Revision: 1.18 $
    $Author: tilda $
         
ORIGINAL AUTHOR
    Nick Marley

DESCRIPTION
    Platform independant test harness for the vformat library functions.  This
    code runs various tests on the library, results are written to stdout.

REFERENCES
    (none)    

MODIFICATION HISTORY
 *  $Log: vformat.c,v $
 *  Revision 1.18  2002/11/16 13:33:18  tilda
 *  IID639288 - Implement method for adding subobjects.
 *
 *  Revision 1.17  2002/11/15 09:14:59  tilda
 *  IID638823 - Various portability issues.
 *
 *  Revision 1.16  2002/11/14 20:14:25  tilda
 *  Minot tidy ups to WIN32 versions.
 *
 *  Revision 1.15  2002/11/11 17:23:08  monos
 *  run testing harness on linux systems
 *
 *  Revision 1.14  2002/11/03 18:56:45  tilda
 *  Add test functions for VCAL encoding functions.
 *
 *  Revision 1.13  2002/11/02 18:29:26  tilda
 *  IID485157 - UI does character conversion based on CHARSET property.
 *
 *  Revision 1.12  2002/11/02 09:03:45  tilda
 *  Start of internationalisation changes.
 *
 *  Revision 1.11  2001/11/18 22:05:57  tilda
 *  Correct args for vf_get_property().
 *
 *  Revision 1.10  2001/10/24 20:20:27  tilda
 *  uit printing when we're done.
 *
 *  Revision 1.9  2001/10/24 18:40:40  tilda
 *  BASE64 bugfixes.  Split reader/writer code. Start create/modify work.
 *
 *  Revision 1.8  2001/10/24 05:33:59  tilda
 *  Test harness / file list mods
 *
 *  Revision 1.7  2001/10/16 05:50:53  tilda
 *  Debug support for lists of vobjects from single file (ie. a phonebook).
 *  
 *  Revision 1.6  2001/10/14 20:42:37  tilda
 *  Addition of group searching.
 *  
 *  Revision 1.5  2001/10/14 19:53:36  tilda
 *  Group handling.  NO group searching functions.
 *  
 *  Revision 1.4  2001/10/13 14:58:56  tilda
 *  Tidy up version headers, add vf_strings.h where needed.
 *  
 *  Revision 1.3  2001/10/12 19:02:15  tilda
 *  Moved generic test functions to new file
 * 
 ******************************************************************************/

#ifndef NORCSID
static const char vformat_c_vss_id[] = "$Header: /cvsroot/vformat/build/vformat/vformat.c,v 1.18 2002/11/16 13:33:18 tilda Exp $";
#endif

/*============================================================================*
 ANSI C & System-wide Header Files
 *============================================================================*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

#define LINELENGTH      (76)

#define MAXBASE64ENC    (1000)

/*============================================================================*
 Private Data Types
 *============================================================================*/
/* None */

/*============================================================================*
 Private Function Prototypes
 *============================================================================*/

static void read_cards_from_dir(const char *dirname, const char *p_pattern);
static void test_read_vcard(const char *filename);
static bool_t read_show_status(VF_OBJECT_T **pp_object, const char *p_name);
static bool_t write_show_status(const char *p_name, VF_OBJECT_T *p_object);
static void update_results(bool_t pass, bool_t warning);
static void do_title_line(const char *filename);
static void print_search_results(VF_PROP_T *p_tmp);
static void check_extract_fields(const char *filename, const char *p_array[3]);
static void check_extract_groups(const char *filename, const char *p_group);
static void check_base64_enc(bool_t rdata);
static bool_t encode_decode_check(const char *p_data, uint32_t length);
static void check_vcal_functions(void);
static void get_random_date(VF_ISO8601_PERIOD_T *p_period);
static void get_out_file(char *, const char *, const char *);

/*============================================================================*
 Private Data
 *============================================================================*/

static int tot_tests = 0;
static int tot_errors = 0;
static int tot_warnings = 0;

static const char *pp_tel_fields[3] =
{
    VFP_TELEPHONE,
    VFP_WORK,
    VFP_PREFERRED
};


/*============================================================================*
 Public Function Implementations
 *============================================================================*/


/*----------------------------------------------------------------------------*
 * NAME
 *      main()
 * 
 * DESCRIPTION
 *      Entry point.
 *
 * RETURNS
 *      Hopefully.
 *----------------------------------------------------------------------------*/

int main(int argc, char **argv)
{
    /*
     * simple reading / writing tests.
     */
    read_cards_from_dir("tests//spec", "*.vcf");
    read_cards_from_dir("tests//adhoc", "*.vcf");  
    read_cards_from_dir("tests//utf-8", "*.vcf");

    /*
     * list access functions.
     */
    check_extract_fields("tests//access//access_1.vcf", pp_tel_fields);
    check_extract_groups("tests//adhoc//group_7.vcf", "ALPHA");
  
    /*
     * data format tests.
     */
    check_base64_enc(TRUE);
  
    /*
     * VCAL access functions
     */
    check_vcal_functions(); 

    /* split_phonebook("C:\\Windows\\Desktop\\Whole Phonebook.vcf", "C:\\Temp");  */


    /* for automatic testing */
    if (tot_errors) exit (1);
    exit (0);
}



/*============================================================================*
 Private Function Implementations
 *============================================================================*/


/*----------------------------------------------------------------------------*
 * NAME
 *      main()
 * 
 * DESCRIPTION
 *      Entry point.
 *
 * RETURNS
 *      Hopefully.
 *----------------------------------------------------------------------------*/

void read_cards_from_dir(
    const char *p_dirname,
    const char *p_pattern
    )
{
    file_enum_t f;
    char filename[_MAX_PATH];

    if (find_first_file(filename, &f, p_dirname, p_pattern))
    {
        do
        {
            test_read_vcard(filename);
        }
        while (find_next_file(filename, &f))
            ;

        close_file_find(&f);
    }
}






/*----------------------------------------------------------------------------*
 * NAME
 *      test_read_vcard()
 * 
 * DESCRIPTION
 *      Attempt to read the indicated card.  If the read succeeds, then write
 *      the card out to a new file.  Compare the original file with the new
 *      files - they will be the same if the file was in exactly the format
 *      produced by this library (line lengths & ends may differ of course as
 *      well as choice of characters escaped in quoted printable format).
 *
 *      We then read in "our" version of the test file and write it out. This
 *      time, the output should be the same as the input.  Well - for the
 *      purposes of this test I consider it an error if they differ!
 *
 * RETURNS
 *      (none)
 *----------------------------------------------------------------------------*/

static void test_read_vcard(
    const char *filename
    )
{
    VF_OBJECT_T *p_object = NULL;
    bool_t warning = FALSE, pass = FALSE;

    do_title_line(filename);

    if (read_show_status(&p_object, filename))
    {
        VF_OBJECT_T *p_object_1 = NULL;
        char outfile_1[_MAX_PATH];
        char outfile_2[_MAX_PATH];

        get_out_file(outfile_1, ".out-1", filename);
        get_out_file(outfile_2, ".out-2", filename);

        write_show_status("vobject.vcf", p_object);

        write_show_status(outfile_1, p_object);

        if (0 == filecomp(outfile_1, filename))
        {
            printf("    OK - no differences after 1st read/write\n");
        }
        else
        {
            printf("    Warning - differences after 1st read/write\n");

            warning = TRUE;
        }

        if (read_show_status(&p_object_1, outfile_1))
        {
            write_show_status(outfile_2, p_object_1);
        }

        if (0 == filecomp(outfile_2, outfile_1))
        {
            printf("    OK - no differences after 2nd read/write\n");

            pass = TRUE;
        }
        else
        {
            printf("    Error - differences after 2nd read/write\n");
        }

        vf_delete_object(p_object, TRUE);
        vf_delete_object(p_object_1, TRUE);
    }

    update_results(pass, warning);
}





/*----------------------------------------------------------------------------*
 * NAME
 *      do_title_line()
 * 
 * DESCRIPTION
 *      Print a title line.
 *
 * RETURNS
 *      (none)
 *----------------------------------------------------------------------------*/

static void do_title_line(const char *filename)
{
    char title[LINELENGTH];

    memset(title, '-', sizeof(title));
    title[sizeof(title) - 1] = '\0';
    memcpy(title + 2, filename, strlen(filename));
    printf("\n%s\n", title);
}







/*----------------------------------------------------------------------------*
 * NAME
 *      read_show_status()
 * 
 * DESCRIPTION
 *      Wrapper on vf_read_file() which displays status after the read.
 *
 * RETURNS
 *      Same as wrapped vf_read_file().
 *----------------------------------------------------------------------------*/

static bool_t read_show_status(
    VF_OBJECT_T **pp_object,
    const char *p_name
    )
{
    bool_t ret;

    ret = vf_read_file(pp_object, p_name);

    if (ret)
    {
        printf("Successfully read file '%s'\n", p_name);
    }
    else
    {
        printf("Failed to read file '%s'\n", p_name);
    }

    return ret;
}




/*----------------------------------------------------------------------------*
 * NAME
 *      write_show_status()
 * 
 * DESCRIPTION
 *      Wrapper on vf_write_file() which displays status after the read.
 *
 * RETURNS
 *      Same as wrapped vf_read_file().
 *----------------------------------------------------------------------------*/

static bool_t write_show_status(
    const char *p_name,
    VF_OBJECT_T *p_object
    )
{
    bool_t ret;

    ret = vf_write_file(p_name, p_object, TRUE);

    if (ret)
    {
        printf("Successfully wrote file '%s'\n", p_name);
    }
    else
    {
        printf("Failed to wrote file '%s'\n", p_name);
    }

    return ret;
}



/*----------------------------------------------------------------------------*
 * NAME
 *      write_show_status()
 * 
 * DESCRIPTION
 *      Wrapper on vf_write_file() which displays status after the read.
 *
 * RETURNS
 *      Same as wrapped vf_read_file().
 *----------------------------------------------------------------------------*/

static void update_results(
    bool_t pass,
    bool_t warning
    )
{
    char buffer[80];

    tot_tests += 1;

    if (pass)
    {
        /* OK */
    }
    else
    {
        tot_errors += 1;
    }

    if (warning)
    {
        tot_warnings += 1;
    }

    sprintf(buffer, "[%d tests, %d errors, %d warnings]",
        tot_tests, tot_errors, tot_warnings);

    printf("%*s\n", LINELENGTH-1, buffer);
}






/*----------------------------------------------------------------------------*
 * NAME
 *      check_extract_fields()
 * 
 * DESCRIPTION
 *      Exract fields.
 *
 * RETURNS
 *      (none)
 *----------------------------------------------------------------------------*/

static void check_extract_fields(
    const char *filename,
    const char *p_array[3]
    )
{
    VF_OBJECT_T *p_object = NULL;
    bool_t warning = FALSE, pass = FALSE;

    do_title_line(filename);

    if (read_show_status(&p_object, filename))
    {
        VF_PROP_T *p_tmp;

        printf(" Looking for {%s}...\n", p_array[0]);
        if (vf_get_property(&p_tmp, p_object, VFGP_FIND, NULL, p_array[0], NULL))
        {
            print_search_results(p_tmp);
        }

        printf(" Looking for {%s, %s}...\n", p_array[0], p_array[1]);
        if (vf_get_property(&p_tmp, p_object, VFGP_FIND, NULL, p_array[0], p_array[1], NULL))
        {
            print_search_results(p_tmp);
        }

        printf(" Looking for {%s, %s}...\n", p_array[0], p_array[2]);
        if (vf_get_property(&p_tmp, p_object, VFGP_FIND, NULL, p_array[0], p_array[2], NULL))
        {
            print_search_results(p_tmp);
        }

        printf(" Looking for {%s, %s, %s}...\n", p_array[0], p_array[1], p_array[2]);
        if (vf_get_property(&p_tmp, p_object, VFGP_FIND, NULL, p_array[0], p_array[1], p_array[2], NULL))
        {
            print_search_results(p_tmp);
        }

        vf_delete_object(p_object, TRUE);

        pass = TRUE;
    }
    
    update_results(pass, warning);
}







/*----------------------------------------------------------------------------*
 * NAME
 *      extract_groups()
 * 
 * DESCRIPTION
 *      
 *
 * RETURNS
 *      (none)
 *----------------------------------------------------------------------------*/

static void check_extract_groups(
    const char *filename,
    const char *p_group
    )
{
    VF_OBJECT_T *p_object = NULL;
    bool_t warning = FALSE, pass = FALSE;

    do_title_line(filename);

    if (read_show_status(&p_object, filename))
    {
        VF_PROP_T *p_tmp;

        if (vf_get_property(&p_tmp, p_object, VFGP_FIND, p_group, "LOGO", NULL))
        {
            print_search_results(p_tmp);
        }

        vf_delete_object(p_object, TRUE);

        pass = TRUE;
    }
    
    update_results(pass, warning);
}








/*----------------------------------------------------------------------------*
 * NAME
 *      print_search_results()
 * 
 * DESCRIPTION
 *      List the values in each of the current search results.
 *
 * RETURNS
 *      (none)
 *----------------------------------------------------------------------------*/

static void print_search_results(
    VF_PROP_T *p_tmp        /* Pointer to current property */
    )
{
    do
    {
        void *ptr;
        uint32_t size;
        vf_encoding_t enc;

        if (vf_get_prop_value(p_tmp, &ptr, &size, &enc))
        {
            switch (enc)
            {
            case VF_ENC_BASE64:
                {
                    printf("  prop = \"%s\"\n", vf_get_prop_name_string(p_tmp, 0));
                }
                break;

            case VF_ENC_7BIT:
            case VF_ENC_QUOTEDPRINTABLE:
                {
                    uint16_t i;

                    for (i = 0;;i++)
                    {
                        const char *p_value = vf_get_prop_value_string(p_tmp, i);

                        if (p_value)
                        {
                            printf("  field[%d] = \"%s\"\n", i, p_value);
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                break;
            }
        }
    }
    while (vf_get_next_property(&p_tmp))
        ;
}






/*----------------------------------------------------------------------------*
 * NAME
 *      check_base64_enc()
 * 
 * DESCRIPTION
 *      Check BASE64 encoding.
 *
 * RETURNS
 *      (none)
 *----------------------------------------------------------------------------*/

static void check_base64_enc(bool_t rdata)
{
    char buffer[MAXBASE64ENC+1];
    int i, s;
    bool_t pass;

    do_title_line("BASE64 encoding");

    for (s = 1, pass = TRUE;pass && (s < sizeof(buffer));s++)
    {
        for (i = 0;i < s;i++)
        {
            buffer[i] = (rdata ? rand() : (i + 1)) % 256;
        }

        pass &= encode_decode_check(buffer, s);

        if (pass)
        {
            printf("encode_decode_check() : OK @ %d bytes\r", s);
        }
    }

    printf("\n");

    update_results(pass, 0);
}



/*----------------------------------------------------------------------------*
 * NAME
 *      encode_decode_check()
 * 
 * DESCRIPTION
 *      Encode & decode chunk of data.  Check data consistency.
 *
 * RETURNS
 *      TRUE iff encoded / decoded successfully.
 *----------------------------------------------------------------------------*/

static bool_t encode_decode_check(
    const char *p_data,         /* Pointer to data */
    uint32_t length             /* Length */
    )
{
    VF_OBJECT_T *p_object;
    const char *testfile = "bindata.vcf";
    const char *testprop = "X-BINARY-DATA";

    bool_t pass = FALSE;

    p_object = vf_create_object("TEST", NULL);

    if (p_object)
    {
        VF_PROP_T *p_prop;

        if (vf_get_property(&p_prop, p_object, VFGP_GET, NULL, testprop, NULL))
        {
            vf_set_prop_value_base64(p_prop, p_data, length, TRUE);
        }

        vf_write_file(testfile, p_object, TRUE);

        vf_delete_object(p_object, TRUE);
    }

    if (vf_read_file(&p_object, testfile))
    {
        VF_PROP_T *p_prop;

        if (vf_get_property(&p_prop, p_object, VFGP_FIND, NULL, testprop, NULL))
        {
            const char *p_new_data;
            uint32_t new_length;

            p_new_data = vf_get_prop_value_base64(p_prop, &new_length);

            if (p_new_data)
            {
                if (length == new_length)
                {
                    if (0 == memcmp(p_data, p_new_data, length))
                    {
                        pass = TRUE;
                    }
                    else
                    {
                        printf("encode_decode_check() : data NOT OK\n");
                    }
                }
                else
                {
                    printf("encode_decode_check() : Length error\n");
                }
            }
            else
            {
                printf("encode_decode_check() : No data\n");
            }
        }

        vf_delete_object(p_object, TRUE);
    }

    return pass;
}





/*----------------------------------------------------------------------------*
 * NAME
 *      check_vcal_functions()
 * 
 * DESCRIPTION
 *      Check the date encoding functions.  This is a non systematic random
 *      test ehich generates random values, encodes them, decodes them and
 *      compares the result with the initial value.
 *
 * RETURNS
 *      (none)
 *----------------------------------------------------------------------------*/

static void check_vcal_functions(void)
{
    VF_ISO8601_PERIOD_T test_in, test_out;
    int i;
    bool_t pass;

    do_title_line("VCAL encoding functions");

    for (i = 0, pass = TRUE;pass && (i < 1000);i++)
    {
        char sz[256];

        get_random_date(&test_in);

        vf_period_time_to_string(sz, &test_in);
        vf_period_string_to_time(&test_out, sz);

        if (memcmp(&test_in, &test_out, sizeof(VF_ISO8601_PERIOD_T)))
        {
            pass = FALSE;
        }
        else
        {
            printf("check_vcal_functions() : %s        \r", sz);
        }
    }

    printf("\n");

    update_results(pass, 0);
}




/*----------------------------------------------------------------------------*
 * NAME
 *      get_random_date()
 * 
 * DESCRIPTION
 *      Initialise a random VF_ISO8601_PERIOD_T value.
 *
 * RETURNS
 *      (none)
 *----------------------------------------------------------------------------*/

void get_random_date(VF_ISO8601_PERIOD_T *p_period)
{
    p_period->days = rand() % 100;
    p_period->hours = rand() % 24;
    p_period->minutes = rand() % 60;
    p_period->months = rand() % 12;
    p_period->seconds = rand() % 60;
    p_period->weeks = rand() % 9;
    p_period->years = rand() % 20;
}



/*----------------------------------------------------------------------------*
 * NAME
 *      get_out_file()
 * 
 * DESCRIPTION
 *      Obtain the name of a test output file from a teset input file.  The
 *      outout filename is the input name with the subscript replaced.
 *
 * RETURNS
 *      (none)
 *----------------------------------------------------------------------------*/

void get_out_file(
    char *p_outfile,           /* Buffer to receive filename */
    const char *p_subscript,   /* New subscript */
    const char *p_infile       /* Name of source file we're mimicing */
    )
{
    char *p_found_subscript;

    strcpy(p_outfile, p_infile);

    if (NULL == (p_found_subscript = strstr(p_outfile, ".")))
    {
        printf("get_out_file() : invalid name (%s) in test files\n", p_outfile);
    }
    else
    {
        strcpy(p_found_subscript, p_subscript);
    }
}

