#ifndef LIBMIDIFILEX_MIDICVT_GLOBALS_H
#define LIBMIDIFILEX_MIDICVT_GLOBALS_H

/*
 * midicvt - A text-MIDI translater
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
 * \file          midicvt_globals.h
 *
 *    This module provides global variables and accessor functions for the
 *    midicvt portion of libmidifilex.
 *
 * \library       midicvt application portion of libmidifilex
 * \author        Chris Ahlstrom and others; see documentation
 * \date          2014-04-09
 * \updates       2016-05-19
 * \version       $Revision$
 * \license       GNU GPL
 */

#include <setjmp.h>                    /* jmp_buf                             */
#include <stdio.h>                     /* FILE *                              */
#include <midicvt_macros.h>            /* cbool_t and other stuff             */

/**
 *    Provides a return value for readmt() that indicates that EOF was
 *    encountered.  This is a more searchable macro for EOF.
 */

#define READMT_EOF                   EOF

/**
 *    Provides a return value for readmt() that indicates that "MTrk" was
 *    not matched, but we're simply ignoring such results.
 */

#define READMT_IGNORE_NON_MTRK      (-9)

/**
 *    Global variables!  We have managed to hide some of the other ones
 *    within the midicvt_globals.c module, but not all of them yet.
 */

extern FILE * g_io_file;
extern FILE * g_redirect_file;         /* for redirecting stdout              */
extern jmp_buf g_status_erjump;
extern int g_status_tracks_to_do;
extern int g_status_err_cont;
extern int g_status_format;
extern int g_status_no_of_tracks;
extern unsigned char * g_status_buffer;
extern int g_status_buflen;
extern int g_status_bufsiz;
extern int g_status_track_number;
extern int g_status_measure;
extern int g_status_M0;
extern int g_status_beat;
extern int g_status_clicks;
extern long g_status_T0;
extern char * g_option_Onmsg;
extern char * g_option_Offmsg;
extern char * g_option_PoPrmsg;
extern char * g_option_Parmsg;
extern char * g_option_Pbmsg;
extern char * g_option_PrChmsg;
extern char * g_option_ChPrmsg;

/*
 * Additions for the --human option
 */

extern char * g_human_Parmsg;

/*
 * Use externs from the flex-generated file.
 */

extern long yyval;

/*
 * Global functions
 */

EXTERN_C_DEC

extern void midicvt_set_defaults (void);

extern void midicvt_set_option_fold (int f);
extern int midicvt_option_fold (void);

extern void midicvt_set_option_mfile (cbool_t f);  /* new 2015-08-14 */
extern cbool_t midicvt_option_mfile (void);

extern void midicvt_set_option_strict (cbool_t f); /* new 2015-08-18 */
extern cbool_t midicvt_option_strict (void);

extern void midicvt_set_option_ignore (cbool_t f); /* new 2015-08-19 */
extern cbool_t midicvt_option_ignore (void);

extern void midicvt_set_option_verbose (cbool_t f);
extern cbool_t midicvt_option_verbose (void);

extern void midicvt_set_option_verbose_notes (cbool_t f);
extern cbool_t midicvt_option_verbose_notes (void);

extern void midicvt_set_option_absolute_times (cbool_t f);
extern cbool_t midicvt_option_absolute_times (void);

extern void midicvt_set_option_debug (cbool_t f);
extern cbool_t midicvt_option_debug (void);

extern void midicvt_set_option_compile (cbool_t f);
extern cbool_t midicvt_option_compile (void);

extern void midicvt_set_option_m2m (cbool_t f);
extern cbool_t midicvt_option_m2m (void);

EXTERN_C_END

#endif         /* LIBMIDIFILEX_MIDICVT_GLOBALS_H */

/*
 * midicvt_globals.c
 *
 * vim: sw=3 ts=3 wm=8 et ft=c
 */
