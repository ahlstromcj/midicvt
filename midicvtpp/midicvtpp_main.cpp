/*
 * midicvtpp - A text-MIDI translator and remapper based on midicomp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/**
 * \file          midicvtpp_main.cpp
 *
 *    This module creates the midicvtpp program for translating between MIDI
 *    files and text files, and doing some basic remapping operations.
 *
 * \library       midicvtpp application
 * \author        Chris Ahlstrom
 * \date          2014-04-19
 * \updates       2016-04-23
 * \version       $Revision$
 * \license       GNU GPL
 *
 *    This C++ program extends the C program midicvt by providing some
 *    remapping functionality.
 *
 *    This can save the user from having to create an Awk or Perl script
 *    to do some common remapping operations.
 */

#include <csvarray.hpp>                /* class midipp::csvarray              */
#include <initree.hpp>                 /* class midipp::initree               */
#include <iniwriting.hpp>              /* free functions to write INI's       */
#include <midicvt_base.h>              /* provides default midifile callbacks */
#include <midicvt_helpers.h>           /* provides flex and help-text support */
#include <midicvt_globals.h>           /* encapsulates global variables       */
#include <midicvt_m2m.h>               /* provides alternate callbacks        */
#include <midimapper.hpp>              /* class midipp::midimapper            */
#include "midicvt-config.h"            /* VERSION information                 */

/**
 *    Provides the version string for this program.
 *
 *    Please leave "midicvtpp" as the first characters of this version
 *    string.  The midicvt command-line parser checks this value if
 *    C++-only options are provided.
 */

static const char * const s_help_version =
   "midicvtpp v " MIDICVT_VERSION " " MIDICVT_VERSION_DATE_SHORT
   ;

/**
 *    Addition help string, which supplements the help for the C program
 *    midicvt, contained in libmidifilex/src/midicvt_helpers.h.
 */

static const char * const gs_help_usage_1 =
   "midicvtpp adds functionality to midicvt.\n"
   "\n"
   " --csv-drums f   Convert a CSV (comma-separated values) file to a sectioned\n"
   "                 INI drum file.  Option -o/--output specifies the full name\n"
   "                 of the output file.  The default is 'out.ini', not stdout.\n"
   " --csv-patches f Convert a CSV file to a sectioned INI patch/program file.\n"
   "                 Option -o/--output specifies the output name.  Default is\n"
   "                 'out.ini', not stdout.\n"
   " --m2m f         Employ the given INI mapping file to convert MIDI to MIDI.\n"
   "\n"
   "The following options require the --m2m option:\n"
   "\n"
   " --reverse       Reverse the mapping specified by --m2m. Not all mappings\n"
   "                 can be fully reversed; unique key values are required in\n"
   "                 both directions.\n"
   " --extract n     Write only channel events from channel n, n = 1 to 16.\n"
   " --reject n      Write only channel events not from channel n.\n"
   " --summarize     Show a summary count of the conversions that occurred.\n"
   " --testing       Only the programmer knows what this one does. :-D\n"
   ;

/**
 *    Provides the help text for the midicvt program.
 *
 * \param version
 *    Provides the optional version information.
 */

static void
midicvtpp_help ()
{
   fprintf(stderr, "%s\n", gs_help_usage_1);
}

/**
 *    Holds the flag needed for the --csv-drum options.  See the help text
 *    for this option.
 */

static bool s_write_csv_drum = false;

/**
 *    Holds the flag needed for the --csv-patch options.  See the help text
 *    for this option.
 */

static bool s_write_csv_patch = false;

/**
 *    For the --csv-xxxx options, holds the name of the input CSV
 *    (comma-separated value) file.
 */

static std::string s_csv_in_filename;

/**
 *    For the --csv-xxxx options, holds the name of the output INI file.
 */

static std::string s_ini_out_filename;

/**
 *    For the --m2m option, holds the name of the input INI file that
 *    contains the MIDI mapping information for the MIDI-to-MIDI
 *    conversion.
 *
 *    The default name used to be "../doc/GM_PSS-790_Drums.ini",
 *    useful mainly for testing.  But we want to be able to do --m2m
 *    without requiring an INI file, in the same way the C code does it.
 */

static std::string s_ini_in_filename = "";

/**
 *    Indicates that the --m2m mapping will be treated as a reverse
 *    mapping, and is specified by the --reverse option.
 */

static bool s_m2m_reversal = false;

/**
 *    Provides the name of the MIDI remapping process.  This name is just
 *    a tag name for output, and defaults to "midicvtpp". However, the
 *    --testing option changes it to "testing", which is an internal
 *    signal for unknown purposes.
 */

static std::string s_mapping_name = "midicvtpp";

/**
 *    Provides the channel number to be affected by channel extraction
 *    (--extract) or rejection (--reject).
 */

static int s_filter_channel = -1;

/**
 *    Indicates if the filtered channel is to be rejected (as opposed to
 *    extracted.
 */

static bool s_rejection_on = false;

/**
 *    Indicates that we want summary output, which is set using the
 *    --summarize option.  This output is similar to
 *    that produced by the --debug option, but only shows values that were
 *    actually converted during the MIDI mapping.
 */

static bool s_summarize_conversion = false;

/**
 *    Parses midicvtpp the command-line for options.
 *
 *    First, calls midicvt_parse() to get the midicvt options, and then
 *    parses for the the midicvtpp-specific options.
 *
 * \param argc
 *    The standard argument count for the command-line.
 *
 * \param argv
 *    The standard argument list for the command-line.
 *
 * \return
 *    Returns 0 if the program succeeded, and a non-zero number if the
 *    program fails or help was obtained.
 */

static bool
midicvtpp_parse (int argc, char * argv [])
{
   bool result = midicvt_parse(argc, argv, s_help_version);
   if (! result)
   {
      if (! midi_version_option())
         midicvtpp_help();
   }
   else
   {
      int option_index = 1;
      Mf_nomerge = true;               /* hmmm, the odd man out here          */

      /*
       * Guarantee default option values for C++ options.
       */

      s_write_csv_drum = false;
      s_write_csv_patch = false;
      s_csv_in_filename.clear();
      s_ini_out_filename.clear();
      while (option_index < argc)      /* check arguments 1 through argc-1    */
      {
         if (check_option(argv[option_index], "-d", "--debug"))
         {
            // no additional C++ settings yet
         }
         else if (check_option(argv[option_index], "-v", "--verbose"))
         {
            // no additional C++ settings yet
         }
         else if (check_option(argv[option_index], "", "--testing"))
         {
            s_mapping_name = "testing";
         }
         else if (check_option(argv[option_index], "", "--reverse"))
         {
            s_m2m_reversal = true;
         }
         else if (check_option(argv[option_index], "", "--summarize"))
         {
            s_summarize_conversion = true;
         }
         else if (check_option(argv[option_index], "", "--extract"))
         {
            s_rejection_on = false;
            if ((option_index + 1) < argc)
            {
               option_index++;
               s_filter_channel = atoi(argv[option_index]);
            }
            else
            {
               errprint("Filter channel number required for --extract");
               s_filter_channel = -1;
            }
         }
         else if (check_option(argv[option_index], "", "--reject"))
         {
            if ((option_index + 1) < argc)
            {
               option_index++;
               s_filter_channel = atoi(argv[option_index]);
               s_rejection_on = true;
            }
            else
            {
               errprint("Filter channel number required for --reject");
               s_filter_channel = -1;
               s_rejection_on = false;
            }
         }
         else if (check_option(argv[option_index], "--csv-drum", "--csv-drums"))
         {
            s_write_csv_drum = true;
            if ((option_index + 1) < argc)
            {
               option_index++;
               s_csv_in_filename = argv[option_index];
               s_ini_out_filename = "out.ini";
            }
            else
               errprint("Input CSV filename required");
         }
         else if
         (
            check_option(argv[option_index], "--csv-patch", "--csv-patches")
         )
         {
            s_write_csv_patch = true;
            if ((option_index + 1) < argc)
            {
               option_index++;
               s_csv_in_filename = argv[option_index];
               s_ini_out_filename = "out.ini";
            }
            else
               errprint("Input CSV filename required");
         }
         else if (check_option(argv[option_index], "-o", "--output"))
         {
            /*
             * \change ca 2016-04-23
             *    This option should not be used to set the INI file-name
             *    unless we are setting up only to write that, using one
             *    of the "csv" options.  Otherwise, this option is already
             *    parsed in the C code, in module midicvt_helpers.c, in
             *    the midicvt_parse() function.
             */

            if (s_write_csv_drum || s_write_csv_patch)
            {
               if ((option_index + 1) < argc)
               {
                  option_index++;
                  s_ini_out_filename = argv[option_index];
               }
               else
                  s_ini_out_filename = "out.ini";
            }
         }
         else if (check_option(argv[option_index], "-2", "--m2m"))
         {
            if ((option_index + 1) < argc)
            {
               if (argv[option_index+1][0] != '-')
               {
                  option_index++;
                  s_ini_in_filename = argv[option_index];
               }
            }
            else
            {
               fprintf
               (
                  stderr, "Using '%s' as INI mapping file\n",
                  s_ini_in_filename.c_str()
               );
            }
         }
         option_index++;
      }
   }
   return result;
}

/**
 *    Provides the entry-point for the midicvtpp program.
 *
 * \param argc
 *    Provides the standard count of the number of command-line arguments,
 *    including the name of the program.
 *
 * \param argv
 *    Provides the command-line arguments as an array of pointers.
 *
 * \return
 *    Returns a 0 value if the application succeeds, and a non-zero value
 *    otherwise.
 */

int
main (int argc, char * argv [])
{
   if (! midicvtpp_parse(argc, argv))
      return 1;                        /* --help or --version was given       */

   if (s_write_csv_drum)
   {
      std::string name("midicvtpp");
      midipp::csvarray csv(name, s_csv_in_filename);
      if (csv.is_valid())
      {
         bool ok = midipp::write_sectioned_drum_file(s_ini_out_filename, csv);
         if (! ok)
            return 1;
      }
      else
         return 1;
   }
   else if (s_write_csv_patch)
   {
      std::string name("midicvtpp");
      midipp::csvarray csv(name, s_csv_in_filename);
      if (csv.is_valid())
      {
         bool ok = midipp::write_sectioned_patch_file(s_ini_out_filename, csv);
         if (! ok)
            return 1;
      }
      else
         return 1;
   }
   else
   {
      /*
       * Same processing as the C program 'midicvt'.
       */

      if (midicvt_option_compile())    /* convert "x.txt" to "y.mid"          */
      {
         if (s_mapping_name != "testing")
         {
            if (midicvt_setup_compile())
            {
               midicvt_initfuncs_t2mf();
               midicvt_compile();
               midicvt_close_compile();
            }
            else
               return 1;
         }
         else
         {
            fprintf(stderr, "--testing not supported for this operation\n");
            return 1;
         }
      }
      else if (midicvt_option_m2m())
      {
         if (midicvt_setup_mfread())
         {
            midipp::midimapper m
            (
               s_mapping_name, s_ini_in_filename, s_m2m_reversal,
               s_filter_channel, s_rejection_on,
               std::string(midicvt_input_file()),
               std::string(midicvt_output_file())
            );
            if (m.valid())
            {
               midimap_init(m);        /* hook it in and set it all up        */
               mftransform();          /* a new version of mfread()           */
               if (s_summarize_conversion)
                  show_maps("Conversions", m, false);
            }
            midicvt_close_mfread();
         }
         else
            return 1;
      }
      else
      {
         if (s_mapping_name != "testing")
         {
            if (midicvt_setup_mfread())
            {
               midicvt_initfuncs_mf2t();
               mfread();
               midicvt_close_mfread();
            }
            else
               return 1;
         }
         else
         {
            fprintf(stderr, "--testing not supported for this operation\n");
            return 1;
         }
      }
   }
   return 0;
}

/*
 * midicvtpp_main.cpp
 *
 * vim: sw=3 ts=3 wm=8 et ft=cpp
 */

