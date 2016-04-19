#ifndef LIBMIDIFILEX_MIDICVT_MACROS_H
#define LIBMIDIFILEX_MIDICVT_MACROS_H

/**
 * \file          midicvt_macros.h
 *
 *    This module provides macros for generating simple messages, MIDI
 *    parameters, and more.
 *
 * \library       libmidifilex
 * \author        Chris Ahlstrom and other authors; see documentation
 * \date          2013-11-17
 * \updates       2016-04-19
 * \version       $Revision$
 * \license       GNU GPL
 *
 *    The macros in this file cover:
 *
 *       -  Default values of waonc parameters.
 *       -  MIDI manifest constants.
 *       -  Language-support macros.
 *       -  Error and information output macros.
 *
 * Copyright (C) 2013-2016 Chris Ahlstrom <ahlstrom@users.sourceforge.net>
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
 */

/**
 * MIDI Manifest Constants:
 *
 *    The first set of macros provides default values for the
 *    minimum/maximum detection code.  We want to be able to know the
 *    highest and lowest MIDI notes that were generated, as a help to
 *    running the application again.
 */

#define MIDI_NOTE_MIN             0
#define MIDI_NOTE_MAX_INITIAL   (-1)
#define MIDI_NOTE_A4             69
#define MIDI_NOTE_MAX           127
#define MIDI_FREQ_A4            440

/**
 *    Defines the maximum number of unique MIDI notes.
 */

#define MIDI_NOTE_COUNT         (MIDI_NOTE_MAX + 1)

/**
 *    Defines the values of the MIDI events "note off" and "note on".
 */

#define MIDI_EVENT_NOTE_OFF       0
#define MIDI_EVENT_NOTE_ON        1

/**
 *    Defines the number of notes in a MIDI octave.
 */

#define MIDI_NOTE_OCTAVE         12

/**
 *    Converts an integer expression to a MIDI note.
 */

#define MIDI_NOTE(x)            ((char) (x))

/**
 *    Defines some standard velocity values.
 */

#define MIDI_VELOCITY_MIN         0
#define MIDI_VELOCITY_HALF       64
#define MIDI_VELOCITY_MAX       127

/**
 *    Defines a value used to indicate that certain values are still
 *    in their uninitialized state.
 */

#define MIDI_UNINITIALIZED     (-1)

/**
 *    Defines a value used to indicate that a note function returned an
 *    illegal note.
 */

#define MIDI_NOTE_ILLEGAL      (-1)

/**
 * Language macros:
 *
 *    Provides an alternative to NULL.
 */

#ifndef __cplus_plus
#define nullptr                  0
#elif __cplus_plus <= 199711L
#define nullptr                  0
#endif

/**
 *    Provides a way to declare functions as having either a C++ or C
 *    interface.
 */

#ifndef EXTERN_C_DEC

#ifdef __cplusplus
#define EXTERN_C_DEC extern "C" {
#define EXTERN_C_END }
#else
#define EXTERN_C_DEC
#define EXTERN_C_END
#endif

#endif

/**
 *    Test for being a valid pointer.
 */

#define not_nullptr(x)     ((x) != nullptr)

/**
 *    Test for being an invalid pointer.
 */

#define is_nullptr(x)      ((x) == nullptr)

/**
 *    A more obvious boolean type.
 */

#ifndef __cplus_plus
typedef int cbool_t;
#elif __cplusplus <= 199711L
typedef bool cbool_t;
#endif

/**
 *    Provides the "false" value of the wbool_t type definition.
 */

#ifndef __cplus_plus
#define false    0
#endif

/**
 *    Provides the "true" value of the wbool_t type definition.
 */

#ifndef __cplus_plus
#define true     1
#endif

/**
 *    Provides an easy way to convert a boolean to a string.
 */

#define bool_to_cstr(x)       ((x) ? "true" : "false")

/**
 *    Provides an error reporting macro (which happens to match Chris's XPC
 *    error function.
 */

#define errprint(x)           fprintf(stderr, "? %s\n", x)

/**
 *    Provides an error reporting macro that requires a sprintf() format
 *    specifier as well.
 */

#define errprintf(fmt, x)     fprintf(stderr, fmt, x)

/**
 *    Provides a warning reporting macro (which happens to match Chris's
 *    XPC error function.
 */

#define warnprint(x)          fprintf(stderr, "! %s\n", x)

/**
 *    Provides an error reporting macro that requires a sprintf() format
 *    specifier as well.
 */

#define warnprintf(fmt, x)    fprintf(stderr, fmt, x)

/**
 *    Provides an information reporting macro (which happens to match
 *    Chris's XPC information function.
 */

#define infoprint(x)          fprintf(stderr, "* %s\n", x)

/**
 *    Provides an error reporting macro that requires a sprintf() format
 *    specifier as well.
 */

#define infoprintf(fmt, x)    fprintf(stderr, fmt, x)

#endif         /* LIBMIDIFILEX_MIDICVT_MACROS_H_ */

/*
 * midicvt_macros.h
 *
 * vim: sw=3 ts=3 wm=8 et ft=c
 */
