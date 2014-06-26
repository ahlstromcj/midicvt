#ifndef LIBMIDIFILEX_MIDICVT_T2MF_H
#define LIBMIDIFILEX_MIDICVT_T2MF_H

/* $Id: t2mf.h,v 1.2 1991/11/03 21:50:50 piet Rel $ */

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
 * \file          t2mf.h
 *
 *    This module provides global variables for the text-to-MIDI
 *    conversion portion of libmidifilex.
 *
 * \library       libmidifilex
 * \author        Chris Ahlstrom and many other authors
 * \date          2014-04-09
 * \updates       2014-04-20
 * \version       $Revision$
 * \license       GNU GPL
 */

#include <midifilex.h>

/**
 * Created and declared by flex.  We added these declarations here to avoid
 * warnings and errors.  Find these variables in the flex-generated file
 * t2mflex.c.
 */

extern int do_hex;
extern int eol_seen;
extern int lineno;
extern long yyval;
extern char * yytext;
extern FILE * yyin;
extern FILE * yyout;

#ifdef __cplusplus
extern "C"
{
#endif

extern int yylex (void);

#ifdef __cplusplus
}                    /* extern "C" */
#endif

/**
 * Good old CentOS's version of flex declared yyleng as an integer.
 * Debian unstable's flex declares it as yy_size_t.  Unfortunately,
 * there's no header-file that defines it!  We found one typedef via a web
 * search, but the actual type is a generated typedef in the generated C
 * file, so we have to match that, and hold off the compiler from
 * complaining about a duplicate typedef.
 */

#ifdef OLDER_FLEX                      /* exact macro to be determined        */

extern int yyleng;

#else

#ifndef YY_TYPEDEF_YY_SIZE_T           /* same code as in generated C file    */
#define YY_TYPEDEF_YY_SIZE_T
typedef size_t yy_size_t;
#endif

extern yy_size_t yyleng;

#endif

/**
 * Macros galore.
 */

#define MTHD                  256
#define MTRK                  257
#define TRKEND                258

#define ON                    note_on
#define OFF                   note_off
#define POPR                  poly_aftertouch
#define PAR                   control_change
#define PB                    pitch_wheel
#define PRCH                  program_chng
#define CHPR                  channel_aftertouch
#define SYSEX                 system_exclusive

#define ARB                   259
#define MINOR                 260
#define MAJOR                 261

#define CH                    262
#define NOTE                  263
#define VAL                   264
#define CON                   265
#define PROG                  266

#define INT                   267
#define STRING                268
#define STRESC                269
#define ERR                   270
#define NOTEVAL               271
#define EOL                   272

#define META                  273
#define SEQSPEC               (META+1+sequencer_specific)
#define TEXT                  (META+1+text_event)
#define COPYRIGHT             (META+1+copyright_notice)
#define SEQNAME               (META+1+sequence_name)
#define INSTRNAME             (META+1+instrument_name)
#define LYRIC                 (META+1+lyric)
#define MARKER                (META+1+marker)
#define CUE                   (META+1+cue_point)
#define SEQNR                 (META+1+sequence_number)
#define KEYSIG                (META+1+key_signature)
#define TEMPO                 (META+1+set_tempo)
#define TIMESIG               (META+1+time_signature)
#define SMPTE                 (META+1+smpte_offset)

#endif         /* LIBMIDIFILEX_MIDICVT_T2MF_H */

/*
 * t2mf.h
 *
 * vim: sw=3 ts=3 wm=8 et ft=c
 */
