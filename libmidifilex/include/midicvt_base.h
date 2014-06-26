#ifndef LIBMIDIFILEX_MIDICVT_BASE_H
#define LIBMIDIFILEX_MIDICVT_BASE_H

/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version. This program is distributed in the hope that
 * it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details. You should have received a
 * copy of the GNU General Public License along with this program; if not,
 * write to the...
 *
 * Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/**
 * \file          midicvt_base.h
 *
 *    This module provides functions for a basic MIDI/text conversion
 *    application.
 *
 * \library       midicvt application portion of libmidifilex
 * \author        Chris Ahlstrom and many others; see documentation
 * \date          2014-04-09
 * \updates       2014-05-20
 * \version       $Revision$
 * \license       GNU GPL
 */

#include <stdio.h>                     /* FILE *                              */
#include <midicvt_macros.h>            /* cbool_t and more                    */
#include <midifilex.h>                 /* Mf_nomerge and much more            */

/*
 * Already defined in midifilex.h.
 *
 * #define MThd            0x4d546864L
 * #define MTrk            0x4d54726bL
 */

#define MTHD            256
#define MTRK            257
#define TRKEND          258

#define ON              note_on
#define OFF             note_off
#define POPR            poly_aftertouch
#define PAR             control_change
#define PB              pitch_wheel
#define PRCH            program_chng
#define CHPR            channel_aftertouch
#define SYSEX           system_exclusive

#define ARB             259
#define MINOR           260
#define MAJOR           261

#define CH              262
#define NOTE            263
#define VAL             264
#define CON             265
#define PROG            266

#define INT             267
#define STRING          268
#define STRESC          269
#define ERR             270
#define NOTEVAL         271
#define EOL             272

#define META            273
#define SEQSPEC         (META+1+sequencer_specific)
#define TEXT            (META+1+text_event)
#define COPYRIGHT       (META+1+copyright_notice)
#define SEQNAME         (META+1+sequence_name)
#define INSTRNAME       (META+1+instrument_name)
#define LYRIC           (META+1+lyric)
#define MARKER          (META+1+marker)
#define CUE             (META+1+cue_point)
#define SEQNR           (META+1+sequence_number)
#define KEYSIG          (META+1+key_signature)
#define TEMPO           (META+1+set_tempo)
#define TIMESIG         (META+1+time_signature)
#define SMPTE           (META+1+smpte_offset)

#ifdef NO_YYLENG_VAR
#define yyleng          yylength
#endif

EXTERN_C_DEC

extern void error (const char * s);
extern void midicvt_initfuncs_t2mf (void);
extern void midicvt_initfuncs_mf2t (void);
extern FILE * efopen (const char * name, const char * mode);
extern cbool_t midicvt_setup_compile (void);
extern void midicvt_close_compile (void);
extern void midicvt_compile (void);
extern cbool_t midicvt_setup_mfread (void);
extern void midicvt_close_mfread (void);

/*
 * Leave static (hidden) for now:
 *
 * extern cbool_t redirect_stdout (const char * filename, const char * mode);
 * extern cbool_t revert_stdout ();
 */

EXTERN_C_END

#endif         /*  LIBMIDIFILEX_MIDICVT_BASE_H */

/*
 * midicvt.h
 *
 * vim: sw=3 ts=3 wm=8 et ft=c
 */

