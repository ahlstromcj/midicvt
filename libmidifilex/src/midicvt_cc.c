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
 * \file          midicvt_cc.c
 *
 *    This module provides a place to collect the names of MIDI Control
 *    Change (Continuous Controller, CC) for the upcoming --human option.
 *
 * \library       midicvt application
 * \author        Chris Ahlstrom
 * \date          2016-05-18
 * \updates       2016-05-19
 * \version       $Revision$
 * \license       GNU GPL
 *
 *    This module provides a static/private list of MIDI CC names, and a
 *    function to safely access them.
 */

#include <midicvt_macros.h>            /* nullptr and other stock stuff       */
#include <midicvt_cc.h>                /* declares these global variables     */

/**
 *  Provides the default names of MIDI controllers.
 */

static const char * const s_controller_names[MIDI_VALUE_COUNT] =
{
    /*   0 */ "Bank Select",
    /*   1 */ "Modulation Wheel",
    /*   2 */ "Breath Controller",
    /*   3 */ "Undefined",
    /*   4 */ "Foot Pedal",
    /*   5 */ "Portamento Time",
    /*   6 */ "Data Entry",
    /*   7 */ "Volume",
    /*   8 */ "Balance",
    /*   9 */ "Undefined",
    /*  10 */ "Pan Position",
    /*  11 */ "Expression",
    /*  12 */ "Effect Controller 1",
    /*  13 */ "Effect Controller 2",
    /*  14 */ "Undefined",
    /*  15 */ "Undefined",
    /*  16 */ "General Purpose Slider 1",
    /*  17 */ "General Purpose Slider 2",
    /*  18 */ "General Purpose Slider 3",
    /*  19 */ "General Purpose Slider 4",
    /*  20 */ "Undefined",
    /*  21 */ "Undefined",
    /*  22 */ "Undefined",
    /*  23 */ "Undefined",
    /*  24 */ "Undefined",
    /*  25 */ "Undefined",
    /*  26 */ "Undefined",
    /*  27 */ "Undefined",
    /*  28 */ "Undefined",
    /*  29 */ "Undefined",
    /*  30 */ "Undefined",
    /*  31 */ "Undefined",
    /*  32 */ "Bank Select (fine)",
    /*  33 */ "Modulation Wheel (fine)",
    /*  34 */ "Breath Controller (fine)",
    /*  35 */ "Undefined Controller (fine)",
    /*  36 */ "Foot Pedal (fine)",
    /*  37 */ "Portamento Time (fine)",
    /*  38 */ "Data Entry (fine)",
    /*  39 */ "Volume (fine)",
    /*  40 */ "Balance (fine)",
    /*  41 */ "Undefined Controller (fine)",
    /*  42 */ "Pan position (fine)",
    /*  43 */ "Expression (fine)",
    /*  44 */ "Effect Control 1 (fine)",
    /*  45 */ "Effect Control 2 (fine)",
    /*  46 */ "Undefined Controller (fine)",
    /*  47 */ "Undefined Controller (fine)",
    /*  48 */ "General Purpose Slider 1 (fine)",
    /*  49 */ "General Purpose Slider 2 (fine)",
    /*  50 */ "General Purpose Slider 3 (fine)",
    /*  51 */ "General Purpose Slider 4 (fine)",
    /*  52 */ "Undefined Controller (fine)",
    /*  53 */ "Undefined Controller (fine)",
    /*  54 */ "Undefined Controller (fine)",
    /*  55 */ "Undefined Controller (fine)",
    /*  56 */ "Undefined Controller (fine)",
    /*  57 */ "Undefined Controller (fine)",
    /*  58 */ "Undefined Controller (fine)",
    /*  59 */ "Undefined Controller (fine)",
    /*  60 */ "Undefined Controller (fine)",
    /*  61 */ "Undefined Controller (fine)",
    /*  62 */ "Undefined Controller (fine)",
    /*  63 */ "Undefined Controller (fine)",
    /*  64 */ "Hold Pedal (on/off)",   /* Damper Pedal 0-63=Off, 64-127=on */
    /*  65 */ "Portamento (on/off)",
    /*  66 */ "Sustenuto Pedal (on/off)",
    /*  67 */ "Soft Pedal (on/off)",
    /*  68 */ "Legato Pedal (on/off)",
    /*  69 */ "Hold 2 Pedal (on/off)",
    /*  70 */ "Sound Variation",
    /*  71 */ "Sound Timbre",
    /*  72 */ "Sound Release Time",
    /*  73 */ "Sound Attack Time",
    /*  74 */ "Sound Brightness",
    /*  75 */ "Sound Control 6",
    /*  76 */ "Sound Control 7",
    /*  77 */ "Sound Control 8",
    /*  78 */ "Sound Control 9",
    /*  79 */ "Sound Control 10",
    /*  80 */ "General Purpose Button 1 (on/off)",
    /*  81 */ "General Purpose Button 2 (on/off)",
    /*  82 */ "General Purpose Button 3 (on/off)",
    /*  83 */ "General Purpose Button 4 (on/off)",
    /*  84 */ "Portamento CC",
    /*  85 */ "Undefined",
    /*  86 */ "Undefined",
    /*  87 */ "Undefined",
    /*  88 */ "Undefined",
    /*  89 */ "Undefined",
    /*  90 */ "Undefined",
    /*  91 */ "Reverb Level",
    /*  92 */ "Tremulo Level",
    /*  93 */ "Chorus Level",
    /*  94 */ "Detune Level",
    /*  95 */ "Phaser Level",
    /*  96 */ "Data Button Increment",
    /*  97 */ "Data Button Decrement",
    /*  98 */ "Non-registered Parameter (fine)",   /* for 6, 38, 96, 97 */
    /*  99 */ "Non-registered Parameter (coarse)", /* for 6, 38, 96, 97 */
    /* 100 */ "Registered Parameter (fine)",       /* for 6, 38, 96, 97 */
    /* 101 */ "Registered Parameter (coarse)",     /* for 6, 38, 96, 97 */
    /* 102 */ "Undefined",
    /* 103 */ "Undefined",
    /* 104 */ "Undefined",
    /* 105 */ "Undefined",
    /* 106 */ "Undefined",
    /* 107 */ "Undefined",
    /* 108 */ "Undefined",
    /* 109 */ "Undefined",
    /* 110 */ "Undefined",
    /* 111 */ "Undefined",
    /* 112 */ "Undefined",
    /* 113 */ "Undefined",
    /* 114 */ "Undefined",
    /* 115 */ "Undefined",
    /* 116 */ "Undefined",
    /* 117 */ "Undefined",
    /* 118 */ "Undefined",
    /* 119 */ "Undefined",
    /* 120 */ "All Sound Off",
    /* 121 */ "All Controllers Off",
    /* 122 */ "Local Keyboard (on/off)",
    /* 123 */ "All Notes Off",
    /* 124 */ "Omni Mode Off",
    /* 125 */ "Omni Mode On",
    /* 126 */ "Mono Operation",
    /* 127 */ "Poly Operation"
};

/**
 *    Provides safe access to the s_controller_names strings.
 *
 * \param cc
 *    The putative MIDI CC value, which is validated before usage.
 *
 * \return
 *    Returns a pointer to the desired MIDI CC name if the \a cc parameter
 *    is valid.  Otherwise, a null pointer is returned.  The caller really
 *    ought to check the pointer before using it.
 */

const char *
midi_controller_name (int cc)
{
   const char * result = nullptr;
   if (cc >= 0 && cc < MIDI_VALUE_COUNT)
      result = s_controller_names[cc];

   return result;
}

/*
 * midicvt_cc.c
 *
 * vim: sw=3 ts=3 wm=8 et ft=c
 */
