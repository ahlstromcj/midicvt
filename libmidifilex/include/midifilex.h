#ifndef LIBMIDIFILEX_MIDIFILE_H
#define LIBMIDIFILEX_MIDIFILE_H

/* $Id: midifile.h,v 1.3 1991/11/03 21:50:50 piet Rel $ */

/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version. This program is distributed in the hope that
 * it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 * the GNU General Public License for more details. You should have
 * received a copy of the GNU General Public License along with this
 * program; if not, write to the...
 *
 * Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/**
 * \file          midifilex.h
 *
 *    This module provides functions for libmidifilex.
 *
 * \library       libmidifilex
 * \author        Chris Ahlstrom and many other authors; see documentation
 * \date          2014-04-08
 * \updates       2014-05-20
 * \version       $Revision$
 * \license       GNU GPL
 */

#include <stdio.h>                     /* FILE *                              */
#include <midicvt_macros.h>            /* EXTERN_C_DEC, true, false, etc.     */

extern int Mf_nomerge;
extern long Mf_currtime;

/* definitions for MIDI file parsing code */

EXTERN_C_DEC

extern int (* Mf_getc) (void);
extern int (* Mf_error) (const char *);
extern int (* Mf_report) (const char *);
extern int (* Mf_header) (int, int, int);
extern int (* Mf_starttrack) (void);
extern int (* Mf_endtrack) (long, unsigned long);
extern int (* Mf_on) (int, int, int);
extern int (* Mf_off) (int, int, int);
extern int (* Mf_pressure) (int, int, int);
extern int (* Mf_parameter) (int, int, int);
extern int (* Mf_pitchbend) (int, int, int);
extern int (* Mf_program) (int, int);
extern int (* Mf_chanpressure) (int, int);
extern int (* Mf_sysex) (int, char *);
extern int (* Mf_metamisc) (int, int, char *);
extern int (* Mf_sqspecific) (int, char *);
extern int (* Mf_seqnum) (short int);
extern int (* Mf_text) (int, int, char *);
extern int (* Mf_eot) (void);
extern int (* Mf_timesig) (int, int, int, int);
extern int (* Mf_smpte) (int, int, int, int, int);
extern int (* Mf_tempo) (long);
extern int (* Mf_keysig) (int, int);
extern int (* Mf_arbitrary) (int, char *);
extern int (* Mf_putc) (unsigned char);
extern int (* Mf_wtrack) (void);
extern int (* Mf_wtempotrack) (void);

extern float mf_ticks2sec (unsigned long, int, unsigned int);
extern unsigned long mf_sec2ticks (float, int, unsigned int);
extern void write32bit (unsigned long data);

extern void mfread (void);
extern void mftransform (void);
extern void mfwrite (int, int, int, FILE *);
extern void midifile (void);

extern void mf_w_header_chunk (int format, int ntracks, int division);
extern void mf_w_track_chunk
(
   int which_track,
   FILE * fp,
   int (* wtrack)(void)
);
extern void mf_w_track_start
(
   int which_track,
   FILE * fp
);
extern int mf_w_midi_event
(
   unsigned long, unsigned int, unsigned int, unsigned char *, unsigned long
);
extern int mf_w_sysex_event
(
   unsigned long, unsigned char *, unsigned long
);
extern void mf_w_tempo
(
   unsigned long, unsigned long
);
extern int mf_w_meta_event
(
   unsigned long, unsigned char, unsigned char *, unsigned long
);

EXTERN_C_END

/**
 *    Provides lower-case (!) macros for MIDI status commands.  The most
 *    significant bit is 1.
 */

#define note_off              0x80
#define note_on               0x90
#define poly_aftertouch       0xa0
#define control_change        0xb0
#define program_chng          0xc0
#define channel_aftertouch    0xd0
#define pitch_wheel           0xe0
#define system_exclusive      0xf0
#define delay_packet         (1111)

/**
 *    Provides lower-case macros for the 7 bit controllers.
 */

#define damper_pedal          0x40
#define portamento            0x41
#define sostenuto             0x42
#define soft_pedal            0x43
#define general_4             0x44
#define hold_2                0x45
#define general_5             0x50
#define general_6             0x51
#define general_7             0x52
#define general_8             0x53
#define tremolo_depth         0x5c
#define chorus_depth          0x5d
#define detune                0x5e
#define phaser_depth          0x5f

/**
 *    Provides parameter values.
 */

#define data_inc              0x60
#define data_dec              0x61

/**
 *    Provides parameter selection.
 */

#define non_reg_lsb           0x62
#define non_reg_msb           0x63
#define reg_lsb               0x64
#define reg_msb               0x65

/**
 *    Provides standard MIDI Files meta event definitions.
 */

#define meta_event            0xff
#define sequence_number       0x00
#define text_event            0x01
#define copyright_notice      0x02
#define sequence_name         0x03
#define instrument_name       0x04
#define lyric                 0x05
#define marker                0x06
#define cue_point             0x07
#define channel_prefix        0x20
#define end_of_track          0x2f
#define set_tempo             0x51
#define smpte_offset          0x54
#define time_signature        0x58
#define key_signature         0x59
#define sequencer_specific    0x7f

/**
 *    Provides the manufacturer's ID number.
 */

#define Seq_Circuits          (0x01)   /* Sequential Circuits Inc.   */
#define Big_Briar             (0x02)   /* Big Briar Inc.             */
#define Octave                (0x03)   /* Octave/Plateau             */
#define Moog                  (0x04)   /* Moog Music                 */
#define Passport              (0x05)   /* Passport Designs           */
#define Lexicon               (0x06)   /* Lexicon                    */
#define Tempi                 (0x20)   /* Bon Tempi                  */
#define Siel                  (0x21)   /* S.I.E.L.                   */
#define Kawai                 (0x41)
#define Roland                (0x42)
#define Korg                  (0x42)
#define Yamaha                (0x43)

/**
 *    Provides miscellaneous definitions and macros.
 */

#define MThd 0x4d546864L
#define MTrk 0x4d54726bL

#define lowerbyte(x) ((unsigned char)(x & 0xff))
#define upperbyte(x) ((unsigned char)((x & 0xff00)>>8))

#endif         /* LIBMIDIFILEX_MIDIFILE_H */

/*
 * midifilex.h
 *
 * vim: sw=3 ts=3 wm=8 et ft=c
 */
