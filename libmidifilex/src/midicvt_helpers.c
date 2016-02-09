/*
 * midicvt - A text-MIDI translater based on midicomp.
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
 * \file          midicvt_helpers.c
 *
 *    This module provides the help text for midicvt.
 *
 * \library       midicvt application
 * \author        Chris Ahlstrom
 * \date          2014-04-19
 * \updates       2016-02-09
 * \version       $Revision$
 * \license       GNU GPL
 *
 *    We've offloaded the help text of midicvt to this module so that it
 *    can be re-used in an upcoming C++ version of midicvt.
 *
 *    Also for code re-use, the flex and file-access code from main() have
 *    been moved to this module.
 */

#include <stdio.h>                     /* fprintf()                           */
#include <stdlib.h>                    /* atoi()                              */
#include <string.h>                    /* strlen(), strncpy()                 */
#include <midifilex.h>                 /* global variables from midifile lib  */
#include <midicvt_globals.h>           /* global option variables             */
#include <midicvt_helpers.h>           /* provides flex and help-text support */

/**
 *    Default version string.  Normally, the caller of midicvt_version()
 *    will provide a string, though.
 */

static const char * const gs_help_version = "midicvt v 0.3.3.5";

/**
 *    Help string.  Because of legacy C rules, we have to define 5
 *    different help strings to avail ourselves of enough characters.
 */

static const char * const gs_help_usage_1 =
   "\n"
   "midicvt refactors the midicomp program for translating between MIDI/SMF and\n"
   "text files.  Compare it to the midi2text project at code.google.com.\n"
   "\n"
   "Command line argument usage:\n"
   ;

static const char * const gs_help_usage_2_1 =
   " -2  --m2m       Convert MIDI to MIDI (testing only in midicvt).\n"
   " -c  --compile   Flag to compile ASCII input into MIDI/SMF.\n"
   " -d  --debug     Send any debug output to stderr.\n"
   " -f  --fold [N]  Fold SysEx and SeqSpec data at N (default 80) columns.\n"
   " -i  --input [F] Specify input file (replaces stdin).  Default file-name is\n"
   "                 'out.mid' or 'out.asc', depending on --compile option.\n"
   " -m  --merge     Collapse continued system-exclusives."
   ;

static const char * const gs_help_usage_2_2 =
   " -n  --note      Show note on/off value using note+octave.\n"
   " -o --output [F] Specify output file (replaces stdout). Default file-name\n"
   "                 is 'out.asc' or 'out.mid', depending on --compile option.\n"
   " -t  --time      Use absolute time instead of ticks.\n"
   " -v  --verbose   Output in columns with --notes on.\n"
   " -r  --report    Write detailed information to stderr (debugging).\n"
   " --version       Show the version information for this program."
   ;

/*
 * This string is near the GNU 509-character character-constant limit.
 */

static const char * const gs_help_usage_2_3 =
   " --mfile         Write ASCII using 'MFile' instead of 'MThd' tag.\n"
   " --mthd          Write ASCII using the 'MThd' tag (default).  The program\n"
   "                 can read either tag.\n"
   " --strict        Require that 'MTrk' is the tag for tracks.  By default,\n"
   "                 tracks with other name-tags can be processed.\n"
   " --ignore        Allow non-MTrk chunks, but don't process them. MIDI\n"
   "                 specifies that they should be ignored; midicvt otherwise\n"
   "                 treats them like tracks."
   "\n"
   ;

static const char * const gs_help_usage_3 =
   "To translate a MIDI/SMF file to plain ASCII format:\n"
   "\n"
   "    midicvt midi.mid                     View as plain text.\n"
   "    midicvt -i midi.mid [ -o ] midi.asc  Create a text version.\n"
   "    midicvt midi.mid > midi.asc          Create a text version.\n"
   "\n"
   "To translate a plain ASCII formatted file to MIDI/SMF:\n"
   ;

static const char * const gs_help_usage_4 =
   "    midicvt -c midi.asc midi.mid         Create a MIDI version.\n"
   "    midicvt -c midi.asc -o midi.mid      Create a MIDI version.\n"
   "\n"
   "    midicvt midi.mid | somefilter | midicvt -c -o midi2.mid\n"
   "\n"
   " It is recommended to always use -i/--input and -o/--output to specify\n"
   " the input and output file-names.\n"
   ;

/**
 *    Maximum path-name length.  (We don't really support Windows yet.)
 */

#ifdef _MSC_VER
#define MIDICVT_PATH_MAX         260
#else
#define MIDICVT_PATH_MAX         1024
#endif

/**
 *    Indicates if an input file has yet been specified on the command
 *    line.  This value is usually the first non-option argument
 *    specified, or the parameter for the -i (--input) or -c (--compile)
 *    options.
 */

static cbool_t gs_have_input_file = false;

/**
 *    Holds the value of the input file name.  Restricted to 1024 bytes
 *    for now, but we should make it a heap variable soon.
 */

static char gs_input_file[MIDICVT_PATH_MAX];

/**
 *    Indicates if an output file has yet been specified on the command
 *    line.  This value is usually the second non-option argument
 *    specified, or the parameter for the -o (--output) option.
 */

static cbool_t gs_have_output_file = false;

/**
 *    Holds the value of the output file name.  Restricted to 1024 bytes
 *    for now, but we should make it a heap variable soon.
 */

static char gs_output_file[MIDICVT_PATH_MAX];

/**
 *    Provides the current offset into the global input file.
 */

static long gs_file_offset = 0;

/**
 *    Provides the version text for the midicvt-related programs.
 *
 * \param version
 *    Provides the optional version information.
 */

void
midicvt_version (const char * version)
{
   if (not_nullptr(version))
      fprintf(stderr, "%s\n", version);
   else
      fprintf(stderr, "%s\n", gs_help_version);
}

/**
 *    Provides the help text for the midicvt program.
 *
 * \param version
 *    Provides the optional version information.
 */

void
midicvt_help (const char * version)
{
   midicvt_version(version);
   fprintf(stderr, "%s\n", gs_help_usage_1);
   fprintf(stderr, "%s\n", gs_help_usage_2_1);
   fprintf(stderr, "%s\n", gs_help_usage_2_2);
   fprintf(stderr, "%s\n", gs_help_usage_2_3);
   fprintf(stderr, "%s\n", gs_help_usage_3);
   fprintf(stderr, "%s\n", gs_help_usage_4);
}

/**
 * \getter gs_have_input_file
 *    Provides a safe accessor for this global variable.
 */

cbool_t
midicvt_have_input_file ()
{
   return gs_have_input_file;
}

/**
 * \getter gs_input_file
 *    Provides a safe accessor for this global variable.
 */

const char *
midicvt_input_file ()
{
   return gs_input_file;
}

/**
 *    Sets the input file-name.
 *
 * \param inputfile
 *    Provides the full path specification for the input file.
 *
 * \return
 *    Returns true if the file-name was valid and able to fit in the
 *    file-name buffer.
 */

cbool_t
midicvt_set_input_file (const char * inputfile)
{
   cbool_t result = not_nullptr(inputfile);
   if (result)
      result = strlen(inputfile) < sizeof(gs_input_file);

   if (result)
   {
      (void) strncpy(gs_input_file, inputfile, sizeof(gs_input_file));
      gs_input_file[sizeof(gs_input_file) - 1] = 0;
      gs_have_input_file = true;
   }
   else
      errprint("input file-name is null or too long");

   return result;
}

/**
 * \getter gs_have_output_file
 *    Safe accessor.
 */

cbool_t
midicvt_have_output_file ()
{
   return gs_have_output_file;
}

/**
 * \getter gs_output_file
 *    Safe accessor.
 */

const char *
midicvt_output_file ()
{
   return gs_output_file;
}

/**
 *    Sets the output file-name.
 *
 * \param outputfile
 *    Provides the full path specification for the output file.
 *
 * \return
 *    Returns true if the file-name was valid and able to fit in the
 *    file-name buffer.
 */

cbool_t
midicvt_set_output_file (const char * outputfile)
{
   cbool_t result = not_nullptr(outputfile);
   if (result)
      result = strlen(outputfile) < sizeof(gs_output_file);

   if (result)
   {
      (void) strncpy(gs_output_file, outputfile, sizeof(gs_output_file));
      gs_output_file[sizeof(gs_output_file) - 1] = 0;
      gs_have_output_file = true;
   }
   else
      errprint("output file-name is null or too long");

   return result;
}

/**
 *    Exposes the file-offset counter, useful in debugging.  Since this
 *    value is used in reporting on the character just gotten, and the
 *    offset is already incremented at that point, we deduct one from the
 *    offset.
 */

long
midi_file_offset (void)
{
   return gs_file_offset - 1;
}

/**
 *    Exposes the file-offset counter, useful in debugging.
 */

void
midi_file_offset_clear (void)
{
   gs_file_offset = 0;
}

/**
 *    Increments the file-offset counter.
 */

void
midi_file_offset_increment (void)
{
   ++gs_file_offset;
}

/**
 *    Provides an optional and simple reporting function to be used by
 *    libmidifileex.
 *
 * \param msg
 *    Provides the basic message to be written to standard error.
 *
 * \return
 *    Returns 1 if the result of midi_file_offset() is
 *    greater-than-or-equal to 0, and it returns 0 otherwise.
 */

static int
report (const char * msg)
{
   if (midi_file_offset() >= 0)
   {
      fprintf(stderr, "%04lx: %s\n", midi_file_offset(), msg);
      return 1;
   }
   else
   {
      fprintf(stderr, "? %s\n", msg);
      return 0;
   }
}

/**
 *    Checks the option argument to see if it matches a short or a long
 *    option.
 *
 *    None of these values are checked for being null, since they are
 *    hardwired in the code, and the programmer will fix them to make the
 *    program work.  :-D
 *
 * \param source
 *    Provides the source argument, taken from the command line.  If
 *    empty, the check will fail.
 *
 * \param shortopt
 *    Provides the short version of the option, such as "-c".  If empty,
 *    this value is ignored.
 *
 * \param longopt
 *    Provides the long version of the option, such as "--compile".  If
 *    empty, this value is ignored.
 */

cbool_t
check_option (const char * source, const char * shortopt, const char * longopt)
{
   cbool_t result = false;
   if (strlen(source) > 0)
   {
      if (strlen(shortopt) > 0)
         result = strcmp(source, shortopt) == 0;      /* a match, we're done  */

      if (! result)
      {
         if (strlen(longopt) > 0)
            result = strcmp(source, longopt) == 0;    /* a match, we're done  */
      }
   }
   return result;
}

/**
 *    Parses the midicvt (C) command line.
 *
 * \param argc
 *    Provides the command-line argument count, including the name of the
 *    program.
 *
 * \param argv
 *    Provides the command-line argument list, including the name of the
 *    program.
 *
 * \param version
 *    Provides the actual version string for the --version and --help
 *    options to display.
 *
 * \return
 *    Returns true if there was no "--help" or "--version" option, and the
 *    other options were legal options for the midicvt program.
 */

cbool_t
midicvt_parse (int argc, char * argv [], const char * version)
{
   cbool_t result = true;
   int option_index = 1;
   midicvt_set_defaults();
   Mf_nomerge = true;                  /* hmmm, the odd man out here          */
   while (option_index < argc)         /* check arguments 1 through argc-1    */
   {
      if (check_option(argv[option_index], "-d", "--debug"))
      {
         midicvt_set_option_debug(true);
      }
      else if (check_option(argv[option_index], "-f", "--fold"))
      {
         int fold = 80;                /* provides the default option value   */
         if ((option_index + 1) < argc)
         {
            option_index++;
            fold = atoi(argv[option_index]);
            if (fold <= 0)             /* found a "-d" or nonconvertible no.  */
            {
               option_index--;
               fold = 80;
            }
         }
         midicvt_set_option_fold(fold);
         fprintf(stderr, "fold=%d\n", midicvt_option_fold());
      }
      else if (check_option(argv[option_index], "-m", "--merge"))
      {
         Mf_nomerge = false;
      }
      else if (check_option(argv[option_index], "-n", "--note"))
      {
         midicvt_set_option_verbose_notes(true);
      }
      else if (check_option(argv[option_index], "-t", "--time"))
      {
         midicvt_set_option_absolute_times(true);
      }
      else if (check_option(argv[option_index], "-c", "--compile"))
      {
         midicvt_set_option_compile(true);

         /*
          * Oops.  In order to act properly in a pipe, this option should
          * <i> not </i> set the input file.
          *
          * if ((option_index + 1) < argc)
          * {
          *    option_index++;
          *    if (! midicvt_set_input_file(argv[option_index]))
          *       return false;
          * }
          * else
          *    (void) midicvt_set_input_file("out.asc");
          */
      }
      else if (check_option(argv[option_index], "-2", "--m2m"))
      {
         midicvt_set_option_m2m(true);

         /*
          * \change ca 2014-05-13
          *    We shouldn't bypass any argument in the C processing of
          *    --m2m.
          *
          * if ((option_index + 1) < argc)
          * {
          *    if (argv[option_index+1][0] != '-')
          *       option_index++;         // skip m2m filename here
          * }
          */
      }
      else if (check_option(argv[option_index], "-i", "--input"))
      {
         if ((option_index + 1) < argc)
         {
            option_index++;
            if (! midicvt_set_input_file(argv[option_index]))
               return false;
         }
         else
         {
            if (midicvt_option_compile())
               (void) midicvt_set_input_file("out.asc");
            else
               (void) midicvt_set_input_file("out.mid");
         }
      }
      else if (check_option(argv[option_index], "-o", "--output"))
      {
         if ((option_index + 1) < argc)
         {
            option_index++;
            if (! midicvt_set_output_file(argv[option_index]))
               return false;
         }
         else
         {
            if (midicvt_option_compile())
               (void) midicvt_set_output_file("out.mid");
            else
               (void) midicvt_set_output_file("out.asc");
         }
      }
      else if (check_option(argv[option_index], "", "--mfile"))
      {
         midicvt_set_option_mfile(true);
      }
      else if (check_option(argv[option_index], "", "--strict"))
      {
         midicvt_set_option_strict(true);
      }
      else if (check_option(argv[option_index], "", "--ignore"))
      {
         midicvt_set_option_ignore(true);
      }
      else if (check_option(argv[option_index], "", "--mthd"))
      {
         midicvt_set_option_mfile(false);
      }
      else if (check_option(argv[option_index], "-v", "--verbose"))
      {
         midicvt_set_option_verbose(true);

         /*
          * This is not done in the midi2text project, so we won't do it
          * here.  Use -n separately.
          *
          * midicvt_set_option_verbose_notes(true);
          */
      }
      else if (check_option(argv[option_index], "-r", "--report"))
      {
         midi_file_offset_clear();
         Mf_report = report;
      }
      else if (check_option(argv[option_index], "--", "--version"))
      {
         midicvt_version(version);
         return false;
      }
      else if (check_option(argv[option_index], "-h", "--help"))
      {
         midicvt_help(version);
         return false;
      }
      else if
      (
         check_option(argv[option_index], "", "--csv-drum") ||
         check_option(argv[option_index], "", "--csv-drums") ||
         check_option(argv[option_index], "", "--csv-patch") ||
         check_option(argv[option_index], "", "--csv-patches") ||
         check_option(argv[option_index], "", "--extract") ||
         check_option(argv[option_index], "", "--reject") ||
         check_option(argv[option_index], "", "--reverse") ||
         check_option(argv[option_index], "", "--testing")
      )
      {
         /*
          * Options for the C++ file.  Ignored if the midicvtpp program
          * called this function.  Otherwise, causes an error.
          */

         const char * cppver = "midicvtpp";
         if (strncmp(cppver, version, strlen(cppver)) != 0)
         {
            midicvt_help(version);        /* bad option, give help */
            fprintf
            (
               stderr,
               "C++-only option '%s' provided, aborting.  See help above.\n",
               argv[option_index]
            );
            return false;
         }
         else
         {
            /*
             * We have to skip over the value for options that provide
             * values.  Otherwise, they might be treated as an input or
             * output file instead of an (unused in C code) value.
             */

            if
            (
               check_option(argv[option_index], "", "--csv-drum") ||
               check_option(argv[option_index], "", "--csv-patch") ||
               check_option(argv[option_index], "", "--extract") ||
               check_option(argv[option_index], "", "--reject")
            )
            {
               if ((option_index + 1) < argc)
               {
                  if (argv[option_index+1][0] != '-')
                     option_index++;         /* skip option value here        */
               }
            }
         }
      }
      else if (argv[option_index][0] != '-')
      {
         if (! midicvt_have_input_file())
         {
            if (! midicvt_set_input_file(argv[option_index]))
               return false;
         }
         else if (! midicvt_have_output_file())
         {
            if (! midicvt_set_output_file(argv[option_index]))
               return false;
         }
      }
      else
      {
         midicvt_help(version);        /* bad option, give help */
         errprintf
         (
            "? Bad option '%s' given, see the help above\n",
            argv[option_index]
         );
         return false;
      }
      option_index++;
   }
   return result;
}

/*
 * midicvt_helpers.c
 *
 * vim: sw=3 ts=3 wm=8 et ft=c
 */

