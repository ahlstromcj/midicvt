/*
 * midicvt - A text-MIDI translater based on midicomp.
 *
 * Copyright (C) 1998-2008,2011 Kengo Ichiki <kichiki@users.sourceforge.net>
 *
 * $Id: main.c,v 1.12 2011/12/27 13:11:00 kichiki Exp $
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
 * \file          main.c
 *
 *    This module creates the midicvt program for translating between MIDI
 *    files and text files.
 *
 * \library       midicvt application
 * \author        ??? with modifications by Chris Ahlstrom
 * \date          2014-04-09
 * \updates       2014-04-12
 * \version       $Revision$
 * \license       GNU GPL
 *
 *    By translating a MIDI file to a text file, midicvt allows one to use
 *    standard UNIX/Linux/OSX/Windows tools to modify a MIDI file.  For
 *    example, one can write a script to re-map the track that contains a
 *    non-standard MIDI drum kit into a General MIDI drumkit.  This can
 *    free the user from having to buy a proprietary application that
 *    includes that ability as one of its "features".
 *
 *    This program was derived from the midicomp project, which itself has
 *    a heritage of legacy code, some dating back to the Atari ST!
 *    Another project derived from midicomp is the midi2text project.
 *    That project takes some iffy shortcuts, so the only features taken
 *    from that project are bug-fixes, and [SOON] the ability to call the
 *    program using different app-names.
 *
 *    This program also takes advantage of GNU autotools, and refactors
 *    the global variables into their own module, and the midifile project
 *    into its own library.
 *
 *    Finally, Doxygen markup is being added to make it a bit easier to
 *    grok this project.  It's all just cleanup, but I had fun doing it.
 */

#include <getopt.h>                    /* getopt_long()                       */
#include <stdlib.h>                    /* atoi()                              */
#include <string.h>                    /* strncpy()                           */

#include "macros.h"                    /* nullptr, true, false, etc.          */
#include "midicvt.h"
#include "midicvt_globals.h"
#include "midifile.h"                  /* Mf_nomerge and much more            */
#include "t2mf.h"                      /* declare flex-generated variables    */

/**
 *    Help string.
 */

static const char * const gs_help_usage_1 =
   "midicvt v0.1 2014-04-12 ahlstromcj@gmail.com\n"
   "\n"
   "midicvt is a refactoring of the midicomp program for translating between\n"
   "MIDI files and text files.  Also compare it to the midi2text project at\n"
   "code.google.com.\n"
   "\n"
   "Command line argument usage:\n"
   ;

static const char * const gs_help_usage_2 =
   "    -d  --debug     Send any debug output to stderr.\n"
   "    -v  --verbose   Output in columns with --notes on.\n"
   "    -c  --compile   Compile ascii input into SMF.\n"
   "    -i  --input     Specify the input file, instead of stdin.\n"
   "    -o  --output    Specify the output file, instead of stdout.\n"
   "    -n  --note      Show note on/off value using note|octave.\n"
   "    -t  --time      Use absolute time instead of ticks.\n"
   "    -fN --fold=N    Fold SYSEX data at N columns.\n"
   ;

static const char * const gs_help_usage_3 =
   "To translate a SMF file to plain ascii format:\n"
   "\n"
   "    midicvt some.mid                   # to view as plain text\n"
   "    midicvt some.mid > some.asc        # to create a text version\n"
   "\n"
   "To translate a plain ascii formatted file to SMF:\n"
   "\n"
   "    midicvt -c some.asc some.mid       # input and output filenames\n"
   "    midicvt -c some.asc -o some.mid    # input and output filenames\n"
   "    midicvt -c some.mid < some.asc     # input from stdin with one arg\n"
   "\n"
   "    midicvt some.mid | somefilter | midicvt -c some2.mid\n"
    ;

/**
 *    getopt() short options list.  Note the colons, which are necessary
 *    for options that require arguments.
 */

static const char * const gs_short_options = "dvc:i:o:ntf:";

/**
 *    getopt() long/short options structure.
 */

static struct option gs_long_options [] =
{
   { "debug",   no_argument,       0, 'd' },
   { "verbose", no_argument,       0, 'v' },
   { "compile", required_argument, 0, 'c' },    /* short requires colon */
   { "input",   required_argument, 0, 'i' },    /* short requires colon */
   { "output",  required_argument, 0, 'o' },    /* short requires colon */
   { "note",    no_argument,       0, 'n' },
   { "time",    no_argument,       0, 't' },
   { "fold",    required_argument, 0, 'f' },    /* short requires colon */
   { "help",    no_argument,       0, 'h' },
   { 0, 0, 0, 0 }
};

/**
 *    Checks the option argument.
 */

cbool_t
check_optarg (const char * oarg)
{
   cbool_t result = not_nullptr(oarg);
   if (! result)
      errprint("The option-argument is null");

   return result;
}

/**
 *    Provides the entry-point for the midicvt program.
 *
 * @param argc
 *    Provides the standard count of the number of command-line arguments,
 *    including the name of the program.
 *
 * @param argv
 *    Provides the command-line arguments as an array of pointers.
 *
 * @return
 *    Returns a 0 value if the application succeeds, and a non-zero value
 *    otherwise.
 */

int
main (int argc, char * argv [])
{
   int docompile = 0;
   int c;
   int option_index = 0;
   char inputfile[1024];               /* holds the value for -c & -i options */
   char outputfile[1024];              /* holds the value for the -o option   */
   Mf_nomerge = 1;                     /* from midifile module                */
   opterr = 0;                         /* from the getopt module              */
   set_midicvt_defaults();
   while
   (
      (
         c = getopt_long
         (
            argc, argv, gs_short_options, gs_long_options, &option_index
         )
      )
      != -1
   )
   {
      switch (c)
      {
      case 0:

          if (gs_long_options[option_index].flag != 0)
              break;

      case 'd':

          g_option_debug = true;
          break;

      case 'f':

          if (check_optarg(optarg))
          {
             g_option_fold = atoi(optarg);
             fprintf(stderr, "fold=%d\n", g_option_fold);
          }
          break;

      case 'm':

          Mf_nomerge = 0;
          break;

      case 'n':

          g_option_verbose_notes = true;
          break;

      case 't':

          g_option_absolute_times = true;
          break;

      case 'c':

          docompile = true;
          if (check_optarg(optarg))
          {
             (void) strncpy(inputfile, optarg, sizeof(inputfile));
             inputfile[1023] = 0;
          }
          break;

      case 'i':

          if (check_optarg(optarg))
          {
             (void) strncpy(inputfile, optarg, sizeof(inputfile));
             inputfile[1023] = 0;
          }
          break;

      case 'o':

          if (check_optarg(optarg))
          {
             (void) strncpy(outputfile, optarg, sizeof(outputfile));
             outputfile[1023] = 0;
          }
          break;

      case 'v':

          g_option_verbose = true;
          g_option_verbose_notes = true;
          break;

      case 'h':
      default:

          fprintf(stderr, "%s\n", gs_help_usage_1);
          fprintf(stderr, "%s\n", gs_help_usage_2);
          fprintf(stderr, "%s\n", gs_help_usage_3);
          return 1;
      }
   }
   if (g_option_debug)
      fprintf(stderr, "main()\n");

   if (docompile)
   {
      char * infile;
      char * outfile;
      if (optind < argc)
      {
         infile = inputfile;
         yyin = efopen(infile, "r");
      }
      else
      {
         yyin = stdin;
         infile = "stdin";
      }
      if (optind+1 < argc)
      {
         g_file = efopen(argv[optind+1], "wb");
         outfile = argv[optind+1];
      }
      else
      {
#ifdef SETMODE
         setmode(fileno(stdout), O_BINARY);
         g_file = stdout;
#else
         g_file = fdopen(fileno(stdout), "wb");
#endif
         outfile = "stdout";
      }
      if (g_option_debug)
         fprintf(stderr, "Compiling %s to %s\n", infile, outfile);

      /*
       * Moved to initfuncs() in the midicvt.c module.
       *
       * Mf_putc = fileputc;
       * Mf_wtrack = mywritetrack;
       */

      translate();
      fclose(g_file);
      fclose(yyin);
   }
   else
   {
      if (g_option_verbose)
      {
          g_option_Onmsg   = "On      ch=%-2d  note=%-3s  vol=%-3d\n";
          g_option_Offmsg  = "Off     ch=%-2d  note=%-3s  vol=%-3d\n";
          g_option_PoPrmsg = "PolyPr  ch=%-2d  note=%-3s  val=%-3d\n";
          g_option_Parmsg  = "Param   ch=%-2d  con=%-3d   val=%-3d\n";
          g_option_Pbmsg   = "Pb      ch=%-2d  val=%-3d\n";
          g_option_PrChmsg = "ProgCh  ch=%-2d  prog=%-3d\n";
          g_option_ChPrmsg = "ChanPr  ch=%-2d  val=%-3d\n";
      }
      if (optind < argc)
          g_file = efopen(argv[optind],"rb");
      else
          g_file = fdopen(fileno(stdin),"rb");

      initfuncs();

      /*
       * Moved to initfuncs() in midicvt.c module.
       *
       * Mf_getc = filegetc;
       */

      mfread();
      if (ferror(g_file))
         error ("Output file error");

      fclose(g_file);
   }
   return 0;
}

/*
 * midicvt-main.c
 *
 * vim: sw=3 ts=3 wm=8 et ft=c
 */
