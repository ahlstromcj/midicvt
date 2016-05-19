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
 * \file          midicvt_globals.c
 *
 *    This module provides a place to collect all the global variables
 *    used, and functions to set them.
 *
 * \library       midicvt application
 * \author        Chris Ahlstrom
 * \date          2014-04-09
 * \updates       2015-08-19
 * \version       $Revision$
 * \license       GNU GPL
 */

#include <midicvt_macros.h>            /* nullptr and other stock stuff       */
#include <midicvt_globals.h>           /* declares these global variables     */
#include <midicvt_helpers.h>           /* help and file-management functions  */
#include <t2mf.h>                      /* yyin flex variable                  */

FILE * g_io_file                 = nullptr;
FILE * g_redirect_file           = nullptr;
jmp_buf g_status_erjump;
int g_status_tracks_to_do        = 1;
int g_status_err_cont            = 0;
int g_status_format;
int g_status_no_of_tracks;
unsigned char * g_status_buffer  = nullptr;
int g_status_buflen              = 0;
int g_status_bufsiz              = 0;
int g_status_track_number;
int g_status_measure;
int g_status_M0;
int g_status_beat;
int g_status_clicks;
long g_status_T0;

char * g_option_Onmsg            = "On ch=%d n=%s v=%d\n";
char * g_option_Offmsg           = "Off ch=%d n=%s v=%d\n";
char * g_option_PoPrmsg          = "PoPr ch=%d n=%s v=%d\n";
char * g_option_Parmsg           = "Par ch=%d c=%d v=%d\n";
char * g_option_Pbmsg            = "Pb ch=%d v=%d\n";
char * g_option_PrChmsg          = "PrCh ch=%d p=%d\n";
char * g_option_ChPrmsg          = "ChPr ch=%d v=%d\n";

/*
 * For the --human option.
 */

char * g_human_Parmsg            = "CC channel %d %s value %d\n";

/*
 * These variables have accessor functions in this module, but are
 * otherwise private.
 */

static int g_option_fold                = 0;
static cbool_t g_option_mfile_tag       = false;   /* new 2015-08-14 */
static cbool_t g_option_strict_track    = false;   /* new 2015-08-18 */
static cbool_t g_option_ignore_track    = false;   /* new 2015-08-19 */
static cbool_t g_option_verbose         = false;
static cbool_t g_option_verbose_notes   = false;
static cbool_t g_option_absolute_times  = false;
static cbool_t g_option_debug           = false;
static cbool_t g_option_docompile       = false;
static cbool_t g_option_midi2midi       = false;
static cbool_t g_option_human           = false;   /* new 2015-05-19 */

/**
 * Use externs from an include file!  The flex-generated code is a bit
 * messy.  The generated t2mflex.c file defines these variables, we
 * declare them in t2fm.h, and have to define yyval here.  Bleh.
 */

long yyval = 0UL;

/**
 *    Sets some defaults for status variables.
 */

void
midicvt_set_defaults (void)
{
   /*
    * Statuses
    */

   g_status_track_number   = 0;
   g_status_measure        = 4;
   g_status_beat           = 96;
   g_status_clicks         = 96;
   g_status_M0             = 0;
   g_status_T0             = 0;

   /*
    * Options
    */

   g_option_fold           = 0;
   g_option_mfile_tag      = false;       /* new 2015-08-14 */
   g_option_strict_track   = false;       /* new 2015-08-18 */
   g_option_ignore_track   = false;       /* new 2015-08-19 */
   g_option_verbose        = false;
   g_option_verbose_notes  = false;
   g_option_absolute_times = false;
   g_option_debug          = false;
   g_option_docompile      = false;
   g_option_human          = false;       /* new 2015-05-19 */
   g_option_Onmsg          = "On ch=%d n=%s v=%d\n";
   g_option_Offmsg         = "Off ch=%d n=%s v=%d\n";
   g_option_PoPrmsg        = "PoPr ch=%d n=%s v=%d\n";
   g_option_Parmsg         = "Par ch=%d c=%d v=%d\n";
   g_option_Pbmsg          = "Pb ch=%d v=%d\n";
   g_option_PrChmsg        = "PrCh ch=%d p=%d\n";
   g_option_ChPrmsg        = "ChPr ch=%d v=%d\n";

   /*
    * Upcoming --human options
    */

   g_human_Parmsg          = "CC channel %d %s value %d\n";
}

/**
 * \setter g_option_fold
 */

void
midicvt_set_option_fold (int f)
{
   g_option_fold = f;
}

/**
 * \getter g_option_fold
 */

int
midicvt_option_fold (void)
{
   return g_option_fold;
}

/**
 * \setter g_option_mfile_tag
 */

void
midicvt_set_option_mfile (cbool_t f)
{
   g_option_mfile_tag = f;
}

/**
 * \getter g_option_mfile_tag
 */

cbool_t
midicvt_option_mfile (void)
{
   return g_option_mfile_tag;
}

/**
 * \setter g_option_strict_track
 */

void
midicvt_set_option_strict (cbool_t f)
{
   g_option_strict_track = f;
}

/**
 * \getter g_option_strict_track
 */

cbool_t
midicvt_option_strict (void)
{
   return g_option_strict_track;
}

/**
 * \setter g_option_ignore_track
 */

void
midicvt_set_option_ignore (cbool_t f)
{
   g_option_ignore_track = f;
}

/**
 * \getter g_option_ignore_track
 */

cbool_t
midicvt_option_ignore (void)
{
   return g_option_ignore_track;
}

/**
 * \setter g_option_verbose
 */

void
midicvt_set_option_verbose (cbool_t f)
{
   g_option_verbose = f;
}

/**
 * \getter g_option_verbose
 */

cbool_t
midicvt_option_verbose (void)
{
   return g_option_verbose;
}

/**
 * \setter g_option_verbose_notes
 */

void
midicvt_set_option_verbose_notes (cbool_t f)
{
   g_option_verbose_notes = f;
}

/**
 * \getter g_option_verbose_notes
 */

cbool_t
midicvt_option_verbose_notes (void)
{
   return g_option_verbose_notes;
}

/**
 * \setter g_option_absolute_times
 */

void
midicvt_set_option_absolute_times (cbool_t f)
{
   g_option_absolute_times = f;
}

/**
 * \getter g_option_absolute_times
 */

cbool_t
midicvt_option_absolute_times (void)
{
   return g_option_absolute_times;
}

/**
 * \setter g_option_debug
 */

void
midicvt_set_option_debug (cbool_t f)
{
   g_option_debug = f;
}

/**
 * \getter g_option_debug
 */

cbool_t
midicvt_option_debug (void)
{
   return g_option_debug;
}

/**
 * \setter g_option_docompile
 */

void
midicvt_set_option_compile (cbool_t f)
{
   g_option_docompile = f;
}

/**
 * \getter g_option_docompile
 *
 *    We need to expose this value for main() to use.
 */

cbool_t
midicvt_option_compile (void)
{
   return g_option_docompile;
}

/**
 * \setter g_option_midi2midi
 */

void
midicvt_set_option_m2m (cbool_t f)
{
   g_option_midi2midi = f;
}

/**
 * \getter g_option_midi2midi
 *
 *    We need to expose this value for main() to use.
 */

cbool_t
midicvt_option_m2m (void)
{
   return g_option_midi2midi;
}

/*
 * midicvt_globals.c
 *
 * vim: sw=3 ts=3 wm=8 et ft=c
 */
