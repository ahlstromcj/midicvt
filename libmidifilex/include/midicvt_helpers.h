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
 * \file          midicvt_helpers.h
 *
 *    This module declares the help and version functions for midicvt.
 *
 * \library       midicvt application
 * \author        Chris Ahlstrom
 * \date          2014-04-19
 * \updates       2014-04-23
 * \version       $Revision$
 * \license       GNU GPL
 *
 *    We've offloaded the help text of midicvt to this module so that it
 *    can be re-used in the C++ version of midicvt.
 */

#include <midicvt_macros.h>            /* cbool_t, nullptr, true, false, etc. */

EXTERN_C_DEC

extern cbool_t check_option
(
   const char * source,
   const char * shortopt,
   const char * longopt
);
extern void midicvt_version (const char * version);
extern void midicvt_help (const char * version);
extern cbool_t midicvt_set_input_file (const char * inputfile);
extern cbool_t midicvt_have_input_file ();
extern const char * midicvt_input_file ();
extern cbool_t midicvt_set_output_file (const char * outputfile);
extern cbool_t midicvt_have_output_file ();
extern const char * midicvt_output_file ();
extern cbool_t midicvt_parse (int argc, char * argv [], const char * version);
extern long midi_file_offset (void);
extern void midi_file_offset_clear (void);
extern void midi_file_offset_increment (void);
extern cbool_t midi_version_option (void);

EXTERN_C_END

/*
 * midicvt_helpers.h
 *
 * vim: sw=3 ts=3 wm=8 et ft=c
 */

