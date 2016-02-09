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
 * \file          midicvt_main.c
 *
 *    This module creates the midicvt program for translating between MIDI
 *    files and text files.
 *
 * \library       midicvt application
 * \author        Major modifications by Chris Ahlstrom
 * \date          2014-04-09
 * \updates       2016-02-08
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

#include <midicvt_base.h>              /* provides default midifile callbacks */
#include <midicvt_helpers.h>           /* provides flex and help-text support */
#include <midicvt_globals.h>           /* encapsulates global variables       */
#include <midicvt_m2m.h>               /* provides alternate callbacks        */

/**
 *    Version string.
 */

static const char * const s_help_version = "midicvt v 0.3.3.5 2016-02-09";

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
   if (! midicvt_parse(argc, argv, s_help_version))
      return 1;                        /* --help, --version or bad command    */

   if (midicvt_option_compile())       /* convert "x.txt" to "y.mid"          */
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
   else if (midicvt_option_m2m())
   {
      if (midicvt_setup_mfread())
      {
         midicvt_initfuncs_m2m();
         mftransform();                /* a new version of mfread()           */
         midicvt_close_mfread();
      }
      else
         return 1;
   }
   else
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
   return 0;
}

/*
 * midicvt_main.c
 *
 * vim: sw=3 ts=3 wm=8 et ft=c
 */

