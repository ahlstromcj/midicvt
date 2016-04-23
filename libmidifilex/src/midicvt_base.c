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
 * \file          midicvt_base.c
 *
 *    This module provides functions for basic MIDI/text conversions.
 *
 * \library       midicvt application
 * \author        Chris Ahlstrom and many other authors
 * \date          2014-04-09
 * \updates       2016-04-17
 * \version       $Revision$
 * \license       GNU GPL
 *
 * Summary of MIDI messages:
 *
 *    We want to provide a very brief reference to the MIDI messages
 *    handled by the library and application in this project.
 *
 * Channel Voice Messages.
 *
 *    The first nybble is the message (e.g. note on), and the second
 *    nybble (denoted by "c") is the channel number.
 *
 *    One or more parameter bytes follow, and the first bit is 0, so that
 *    the values range from 0 to 127.
 *
 *    "kk" means "key" or note number, "vv" means "velocity" or some other
 *    "value", "pp" means "program" or "patch".  Other variations are
 *    noted.  All didits are hexadecimal.
 *
 *       -  <code>8c kk vv</code>.  Note Off.
 *       -  <code>9c kk vv</code>.  Note On.
 *       -  <code>Ac kk vv</code>.  Aftertouch (Polyphonic Key Pressure).
 *       -  <code>Bc cc vv</code>.  Control (cc) Change.
 *       -  <code>Cc pp </code>.    Program/patch (cc) Change.
 *       -  <code>Dc pp </code>.    Aftertouch (Channel Pressure).
 *       -  <code>Ec ll mm</code>.  Pitch Wheel Change.  Provides the
 *          least significant and most significant 14 bits.
 *       -  <code>Fc mm </code>.    System Common Messages.  See below.
 *
 * System Common Messages:
 *
 *       -  <code> F0 </code>.      System Exclusive.
 *       -  <code> F1 </code>.      Undefined.
 *       -  <code> F2 </code>.      Song Position Pointer.
 *       -  <code> F3 </code>.      Song Select.
 *       -  <code> F4 </code>.      Undefined.
 *       -  <code> F5 </code>.      Undefined.
 *       -  <code> F6 </code>.      Tune Request.
 *       -  <code> F7 </code>.      End-Of-Exclusive marker.
 *
 * System Real-Time Messages:
 *
 *       -  <code> F8 </code>.      Timing Clock.
 *       -  <code> F9 </code>.      Undefined.
 *       -  <code> FA </code>.      Start.
 *       -  <code> FB </code>.      Continue.
 *       -  <code> FC </code>.      Stop.
 *       -  <code> FD </code>.      Undefined.
 *       -  <code> FE </code>.      Active Sensing.
 *       -  <code> FF </code>.      Reset.  In a MIDI file, this code is
 *                                  used as an escape character to
 *                                  introduce "meta events".
 *
 * Meta Events:
 *
 *    "ln" indicates the length byte.  "tx" indicates all of the bytes
 *    that are part of the text of the meta-event.  This text is ASCII in
 *    some cases, binary in the case of sequencer-specific data.
 *
 *       -  <code> FF 00 02 ss ss</code> or <code> FF 00 00</code>.
 *          Sequence Number.  Optional, but must occur at the beginning of
 *          an MTrk.
 *       -  <code> FF 01 ln tx </code>.   Text Event.
 *       -  <code> FF 02 ln tx </code>.   Copyright Notice.
 *       -  <code> FF 03 ln tx </code>.   Sequence/Track Name.
 *       -  <code> FF 04 ln tx </code>.   Instrument Name.
 *       -  <code> FF 05 ln tx </code>.   Lyric.
 *       -  <code> FF 06 ln tx </code>.   Marker.
 *       -  <code> FF 07 ln tx </code>.   Cue Point.
 *       -  <code> FF 08 ln tx </code> to <code> FF 0F ln tx </code>.
 *                                        Indeterminate/unrecognized text event.
 *       -  <code> FF 20 01 cc </code>.   MIDI Channel Prefix.
 *       -  <code> FF 2F 00 </code>.      End of Track.
 *       -  <code> FF 51 03 tt tt tt</code>. Set tempo.
 *       -  <code> FF 54 05 hr mn se fr ff</code>. SMPTE Offset.
 *       -  <code> FF 58 04 nn dd cc bb</code>. Time Signature.
 *       -  <code> FF 59 02 sf mi </code>.   Key Signature.
 *       -  <code> FF 7F ln tx </code>.   Sequencer-specific binary event.
 *
 * System Exclusive Messages:
 *
 *    The raw format of a SysEx event to be sent to a MIDI device is
 *    roughly as follows:
 *
 *       -# System-exclusive start byte.  <code> 0xF0 </code>.
 *       -# Manufacturer's code.  A 7-bit value, highest bit is 0.
 *       -# Data bytes. A series of 7-bit values.
 *       -# EOS (End-of-System-exclusive) byte.  <code> 0xF7 </code>.
 *
 *    As encoded in a MIDI file, the SysEx message is in the following
 *    format:
 *
 *       -# Delta-time byte.  A typical value is 0x00.
 *       -# System-exclusive start byte.
 *       -# A varinum length value that covers all of the following bytes,
 *          including the SysEx termination marker.
 *       -# Manufacturer's code.
 *       -# Data bytes.
 *       -# EOSysEx byte.
 *
 *    Some MIDI devices send a SysEx message as a series of small packets
 *    with a time-delay between each packet:
 *
 *       -# F0 followed by a number of data bytes, but no F0
 *       -# One or more packets of data bytes that have no F0 or F7.
 *       -# One last packet that ends with an F7.
 *
 *    The MIDI file must encode this single, multi-packet SysEx message as
 *    a series of smaller SysE essages, with F7 serving as a "SysEx
 *    Continuation" marker.  So the above multi-packet message becomes:
 *
 *       -# Initial packet:
 *          -# Delta-time byte.
 *          -# F0 SysEx start byte.
 *          -# A varinum length value.
 *          -# Manufacturer's code.
 *          -# Data bytes.
 *       -# Continuation packet, one or more:
 *          -# Delta-time byte.
 *          -# F0 SysEx start byte.
 *          -# F7 SysEx continuation byte.
 *          -# A varinum length value.
 *          -# Data bytes.
 *       -# Final packet.
 *          -# Delta-time byte.
 *          -# F0 SysEx start byte.
 *          -# A varinum length value.
 *          -# Data bytes.
 *          -# F7 SysEx stop byte.
 *
 *    F7 can also serve as an "Escaped" event.  More on that later.
 *
 *    Devices will vary somewhat on the format of the information encoded
 *    in the SysEx message.  Here's one device and its SysEx description:
 *
 *       http://www.midi-hardware.com/instrukcje/mpot32sysex20.pdf
 *
 *          -# Sys-Ex header: F0
 *          -# Manufacturer ID for MIDI-hardware.com: 00 20 7A
 *          -# Product ID (device ID) for MPOT32: 03
 *          -# Input ID: one byte in range 00..63
 *          -# The command: one byte in range 01..11
 *          -# Command's parameters, dependent on what command was used.
 *          -# Sys-Ex footer: F7
 *
 *          An example Sys-Ex string might look like this:
 *
 *          F0 00 20 7A 03 02 02 01 01 03 F7
 *
 *    Here's another device:
 *
 *          -# Sys-Ex header: F0
 *          -# Manufacturer ID for Roland: 41
 *          -# Device ID: 10 by default, could be other values to support
 *             multiple devices in the same MIDI daisy-chain.
 *          -# Model ID: 42 for a Roland GS synth.
 *          -# Mode: 12 for sending, 11 for a request for information
 *          -# Start address for message: 40007F
 *          -# Data size: Amount of data to send or receive.
 *          -# Infamous Roland checksum.
 *          -# Sys-Ex footer: F7
 *
 *          F0 41 10 42 12 40007F 00 41 F7
 *
 *    That's all for now.
 */

#include <ctype.h>                     /* isprint(), islower()                */
#include <malloc.h>                    /* malloc() and realloc()              */
#include <errno.h>                     /* strerror() and errno                */
#include <unistd.h>                    /* UNIX/Linux only                     */
#include <stdio.h>                     /* FILE *, etc.                        */
#include <stdlib.h>                    /* exit()                              */
#include <string.h>                    /* strerror()                          */

#include <midicvt_base.h>              /* this module's functions and stuff   */
#include <midicvt_globals.h>           /* midicvt_setup_compile()             */
#include <midicvt_helpers.h>           /* midicvt_input_file(), output_file() */
#include <t2mf.h>                      /* yylex() and yyval global            */


/**
 *    We now write out "MThd" instead of "MFile" for our generated results.
 *    The reason?  We were puzzled why we didn't see the header, after not
 *    having run this program for a long time.
 *
 *    Now, files that were written by the old (0.2) version of midicvt
 *    can still be read by the new version.  One can write out the old tag
 *    using the --mfile option.
 */

#define MFILE_FORMAT_4                 "MFile %d %d %d %d\n"
#define MFILE_FORMAT_3                 "MFile %d %d %d\n"
#define MTHD_FORMAT_4                  "MThd %d %d %d %d\n"
#define MTHD_FORMAT_3                  "MThd %d %d %d\n"

/*
 * Internal functions.
 */

static void checkchan (void);
static void checkeol (void);
static void checknote (void);
static void checkval (void);
static void checkcon (void);
static void checkprog (void);
static void checkeol (void);
static int fileputc (unsigned char c);
static int filegetc (void);
static int getbyte (char * mess);
static int getint (char * mess);
static void gethex (void);
static void get16val (void);
static void splitval (void);
static int prs_error (char * s);
static void syntax (void);

/**
 *    Writes an obvious error string to standard error.
 *
 * \public
 *    Note that this is an public function used in other modules, and
 *    declared in the midicvt_base.h file.
 *
 * \param s
 *    Provides the null-terminated error message to be written.
 */

void
error (const char * s)
{
   fprintf(stderr, "Error: %s\n", s);
}

/**
 *    Holds up to four bytes of data that comprise the data values for a
 *    MIDI operation.
 */

static unsigned char gs_data [5];

/**
 *    Holds the channel number of the MIDI channel currently being
 *    processed.
 */

static int gs_chan = 0;

/**
 *    Holds the file descriptor for stdout for later restoration.
 */

static int gs_saved_stdout = -1;

/**
 *    Holds the file position for stdout for later restoration.
 */

static fpos_t gs_saved_stdout_pos;

/**
 *    Writes the time of the current MIDI event to standard output in text
 *    format.  This function handles the "absolute times" (--time) and
 *    "verbose notes" (--verbose) options as well.
 */

static void
prtime (void)
{
   if (midicvt_option_absolute_times())
   {
      long m = (Mf_currtime - g_status_T0) / g_status_beat;

      /*
       * The format with 0 filling was used in midicomp, but since the
       * midi2text project uses the shorter format, and provides more
       * example files for testing, we'll use that project's format.
       *
       * const char * mformat = midicvt_option_verbose_notes() ?
       *    "%03ld:%02ld:%03ld " : "%ld:%ld:%ld " ;
       */

      const char * mformat = "%ld:%ld:%ld ";
      printf
      (
         mformat,
         m / g_status_measure + g_status_M0,
         m % g_status_measure,
         (Mf_currtime-g_status_T0) % g_status_beat
      );
   }
   else
   {
      if (midicvt_option_verbose_notes())
         printf("%-10ld ", Mf_currtime);
      else
         printf("%ld ", Mf_currtime);
   }
}

/**
 *    Writes the provided text to standard output.  The text is enclosed
 *    in quotes, and the backslash escape character is emitted where
 *    needed.
 *
 *    This function will also employ the --fold option.
 *
 * \param p
 *    Provides the characters to be written.  It is unsigned, but the
 *    characters should all, I think, be straight 7-bit ASCII printing
 *    characters.
 *
 * \param leng
 *    Provides the number of characters to be written.
 */

static void
prtext (unsigned char * p, int leng)
{
   int n, c;
   int pos = 25;
   printf("\"");
   for (n = 0; n < leng; n++)
   {
      c = *p++;
      if (midicvt_option_fold() > 0 && pos >= midicvt_option_fold())
      {
         printf("\\\n\t");
         pos = 13;                     /* tab + \xab + \ */
         if (c == ' ' || c == '\t')
         {
            putchar('\\');
            ++pos;
         }
      }
      switch (c)
      {
      case '\\':
      case '"':

         printf("\\%c", c);
         pos += 2;
         break;

      case '\r':

         printf("\\r");
         pos += 2;
         break;

      case '\n':

         printf("\\n");
         pos += 2;
         break;

      case '\0':

         printf("\\0");
         pos += 2;
         break;

      default:

         if (isprint(c))
         {
            putchar(c);
            ++pos;
         }
         else
         {
            printf("\\x%02x" , c);
            pos += 4;
         }
      }
   }
   printf("\"\n");
}

/**
 *    Writes the provided text to standard output in hexadecimal format.
 *    It also handles the --fold option.
 *
 * \param p
 *    Provides the characters to be written.
 *
 * \param leng
 *    Provides the number of characters to be written.
 */

static void
prhex (unsigned char * p, int leng)
{
   int n;
   int pos = 25;
   for (n = 0; n < leng; n++, p++)
   {
      if (midicvt_option_fold() && pos >= midicvt_option_fold())
      {
         printf ("\\\n\t%02x", (unsigned char) (*p));
         pos = 14;                     /* tab + ab + " ab" + \ */
      }
      else
      {
         printf(" %02x", (unsigned char) (*p));

         /*
          * This addition is not present in mf2t.c in the midi2text
          * project.
          */

         pos += 3;
      }
   }
   printf("\n");
}

/**
 *    Writes a note value to standard output.
 *
 *    If the --verbose option was given, then the note is written as a
 *    letter value with the octave number following it.  Otherwise, the
 *    note is written as a integer MIDI note value.
 *
 * \note
 *    Not sure why this function isn't called "prnote()", so I renamed it
 *    from "mknote()".
 *
 * \threadunsafe
 *    Due to the static buffer buf[8] used inside this function.
 *
 * \param pitch
 *    Provides the MIDI note value to be written.
 *
 * \return
 *    Returns a pointer to the beginning of the static buffer buf[].
 */

static char *
prnote (int pitch)
{
   static char * s_notes [] =
   {
        "c", "c#", "d", "d#", "e", "f", "f#", "g", "g#", "a", "a#", "b"
   };
   static char buf[8];
   if (midicvt_option_verbose_notes())
      (void) snprintf(buf, sizeof(buf), "%s%d", s_notes[pitch % 12], pitch / 12);
   else
      (void) snprintf(buf, sizeof(buf), "%d", pitch);

   return buf;
}

/**
 *    Callback function implementing Mf_header().
 *
 *    Provides a libmidifilex callback function to write the MIDI header
 *    data to standard output in text format.
 *
 *    This function is called by readheader() in midifilex.c, which reads:
 *
 *       -# "MThd" marker, 4 bytes, unterminated
 *       -# Size (32-bits)
 *       -# MIDI format (16-bits)
 *       -# Number of tracks (16-bits)
 *       -# Time division (16-bits)
 *
 *    and then calls this callback function, after which it flushes
 *    and additional bytes specified by the size parameter.
 *
 * \param format
 *    Provides the byte describing the format of the MIDI file.  This
 *    value will be 0, 1, or 2.
 *
 * \param ntrks
 *    Provides the byte describing the number of tracks in the MIDI file.
 *    This value ranges from 1 to 65536.  SMF 0 files will have only 1
 *    track.  SMF 1 has multiple tracks, with the first track containing
 *    song information.
 *
 * \param division
 *    Provides the time-division value.  The first byte is either 0
 *    (indicates the time format is ticks per quarter-note) or 1 (the time
 *    format is negative SMPTE).  The second byte is the number of "ticks"
 *    per frame.  For example, 0x80 indicates 128 ticks per frame.
 *
 * \return
 *    Returns true, always.
 */

static int
my_header (int format, int ntrks, int division)
{
   cbool_t usemfile = midicvt_option_mfile();
   if (division & 0x8000)                          /* SMPTE                   */
   {
      midicvt_set_option_absolute_times(false);    /* now we cannot do beats  */
      printf
      (
         usemfile ? MFILE_FORMAT_4 : MTHD_FORMAT_4,
         format, ntrks, -((-(division >> 8)) & 0xff), division & 0xff
      );
   }
   else
   {
      printf(usemfile ? MFILE_FORMAT_3 : MTHD_FORMAT_3, format, ntrks, division);
   }
   if (format < 0 || format > 2)
   {
      fprintf(stderr, "Can't deal with format %d or missing files\n", format);
      exit(1);
   }
   g_status_beat = g_status_clicks = division;
   g_status_tracks_to_do = ntrks;
   return true;
}

/**
 *    Callback function implementing Mf_starttrack().
 *
 *    This function writes "MTrk" and a newline to standard output and
 *    increments the global track number counter.
 *
 *    This marker is a 4-byte unterminated ASCII marker.
 *
 * \return
 *    Returns true, always.
 */

static int
my_trstart (void)
{
   printf("MTrk\n");
   g_status_track_number++;
   return true;
}

/**
 *    Callback function implementing Mf_endtrack().
 *
 *    This function write "TrkEnd" and a newline to standard output, and
 *    decrements the global "tracks to do" counter.
 *
 *    In MIDI, the end-of-track marker is three bytes, ff 2f 00.
 *    One file has a 0a as well.
 *
 * \param header_offset
 *    Offset of the header of the track that is now ending.  This
 *
 * \param track_size
 *    Provides the actual size of the track.  This parameter is not used
 *    in this callback.
 *
 * \return
 *    Returns true, always.
 */

static int
my_trend
(
   long header_offset,
   unsigned long track_size
)
{
   printf("TrkEnd\n");
   --g_status_tracks_to_do;
   if (midicvt_option_debug())
   {
      char mesg[128];
      (void) snprintf
      (
         mesg, sizeof(mesg),
         "Tracks left %d:  track size = %lu; header offset = %ld",
         g_status_tracks_to_do, track_size, header_offset
      );
      infoprint(mesg);
   }
   return true;
}

/**
 *    Callback function implementing Mf_on().
 *
 *    Command: <code> 0x9n </code>.
 *
 *    This function outputs the time using prtime(), then writes the
 *    channel, note, and velocity using the g_option_Onmsg format
 *    specifier.
 *
 *    The MIDI bytes for a note on message are 4 in number:
 *
 *       -# Delta-time byte.  A typical value is 0x60 (96).
 *       -# Note on byte.
 *          <code> 0x9n </code>.
 *          This byte is a status byte having two parts:
 *          -  The first (most significant) nybble of this byte is 0x9.
 *          -  The least significant nybble holds the channel number,
 *             ranging from 0x0 to 0xF.  However, in our sample file,
 *             ex1.mid, there see to be notes in which the note-on byte is
 *             <i>missing</i>!!!  This may be correlated to having a note
 *             velocity of 0, but not sure about that.  There are no
 *             Note-Off messages in that file, by the way.
 *       -# Note value byte.  This value ranges from 0 to 127.
 *       -# Note velocity byte.  This value ranges from 0 to 127.
 *          It seems that a note off can be made by setting this value to
 *          zero.  Again, not sure about that.
 *
 * \param chan
 *    Provides the channel number to write.
 *
 * \param pitch
 *    Provides the MIDI note number to write.
 *
 * \param vol
 *    Provides the MIDI note velocity to write.
 *
 * \return
 *    Returns true, always.
 */

static int
my_non (int chan, int pitch, int vol)
{
   prtime();
   printf(g_option_Onmsg, chan+1, prnote(pitch), vol);
   return true;
}

/**
 *    Callback function implementing Mf_off().
 *
 *    Command: <code> 0x8n </code>.
 *
 *    Very similar to Note-On messages [see my_non()].
 *
 *    In our sample file, ex1.mid, there are no Note-Off messages, only
 *    Note-On messages with a velocity of 0.
 *
 * \param chan
 *    Provides the channel number to write.
 *
 * \param pitch
 *    Provides the MIDI note number to write.
 *
 * \param vol
 *    Provides the MIDI note velocity to write.
 *
 * \return
 *    Returns true, always.
 */

static int
my_noff (int chan, int pitch, int vol)
{
   prtime();
   printf(g_option_Offmsg, chan+1, prnote(pitch), vol);
   return true;
}

/**
 *    Callback function implementing Mf_pressure().
 *
 *    Command: <code> 0xAn </code>.
 *
 *    Very similar to Note-On messages [see my_non()].
 *
 * \param chan
 *    Provides the channel number to write.
 *
 * \param pitch
 *    Provides the MIDI note number to write.
 *
 * \param pressure
 *    Provides the MIDI polyphonic key pressure value to write.
 *
 * \return
 *    Returns true, always.
 */

static int
my_pressure (int chan, int pitch, int pressure)
{
   prtime();
   printf(g_option_PoPrmsg, chan+1, prnote(pitch), pressure);
   return true;
}

/**
 *    Callback function implementing Mf_parameter().
 *
 *    Command: <code> 0xBn </code>.
 *
 *    This function prints the Control Change message.  This message has
 *    different parameters than the Note On/Off messages.
 *
 *    The MIDI bytes for a control-change message are 4 in number:
 *
 *       -# Delta-time byte.  A typical value is 0x60 (96).
 *       -# Control-change byte.
 *          <code> 0xBn </code>.
 *          This byte is a status byte having two parts, the command
 *          nybble and the channel nybble.
 *       -# Control-change value byte.  This value ranges from 0 to 127.
 *          It determines which controller (e.g. pitch or sustain) is
 *          changed.
 *       -# Controller value byte.  This value ranges from 0 to 127.
 *
 * \param chan
 *    Provides the channel number to write.
 *
 * \param control
 *    Provides the MIDI controller number to write.
 *
 * \param value
 *    Provides the MIDI controller parameter value to write.
 *
 * \return
 *    Returns true, always.
 */

static int
my_parameter (int chan, int control, int value)
{
   prtime();
   printf(g_option_Parmsg, chan+1, control, value);
   return true;
}

/**
 *    Callback function implementing Mf_pitchbend().
 *
 *    Command: <code> 0xEn </code>.
 *
 *       -  <code>Ec ll mm</code>.  Pitch Wheel Change.  Provides the
 *          least significant and most significant 14 bits.
 *
 *       -# Delta-time byte.  A typical value is 0x60 (96).
 *       -# Pitch-bend byte.  <code> 0xEn </code>.
 *       -# Pitch, least-significat value byte.  This value ranges from 0 to 127.
 *          It provides the lowest 7 bits of the pitch-bend parameter.
 *       -# Pitch, most-significat value byte.
 *          It provides the highest 7 bits of the pitch-bend parameter.
 *
 * \param chan
 *    Provides the channel number to write.
 *
 * \param lsb
 *    Provides the least-significant bits of the MIDI pitch-wheel value to
 *    write.
 *
 * \param msb
 *    Provides the most-significant bits of the MIDI pitch-wheel value to
 *    write.
 *
 * \return
 *    Returns true, always.
 */

static int
my_pitchbend (int chan, int lsb, int msb)
{
   prtime();
   printf(g_option_Pbmsg, chan+1, 128*msb + lsb);
   return true;
}

/**
 *    Callback function implementing Mf_program().
 *
 *    Command: <code> 0xCn </code>.
 *
 *    A program-change (patch change) command has the following 3 bytes:
 *
 *       -# Delta-time byte.  A typical value is 0x60 (96).
 *       -# Program-change byte.  <code> 0xCn </code>.
 *       -# Program/patch number byte.  This value ranges from 0 to 127.
 *
 * \param chan
 *    Provides the channel number to write.
 *
 * \param program
 *    Provides the patch/program number to write.
 *
 * \return
 *    Returns true, always.
 */

static int
my_program (int chan, int program)
{
   prtime();
   printf(g_option_PrChmsg, chan+1, program);
   return true;
}

/**
 *    Callback function implementing Mf_chanpressure().
 *
 *    Command: <code> 0xDn </code>.
 *
 *    A channel-pressure command has the following 3 bytes:
 *
 *       -# Delta-time byte.  A typical value is 0x60 (96).
 *       -# Channel-pressure byte.  <code> 0xDn </code>.
 *       -# Channel-pressure value byte.  This value ranges from 0 to 127.
 *
 * \param chan
 *    Provides the channel number to write.
 *
 * \param pressure
 *    Provides the patch/program number to write.
 *
 * \return
 *    Returns true, always.
 */

static int
my_chanpressure (int chan, int pressure)
{
   prtime();
   printf(g_option_ChPrmsg, chan+1, pressure);
   return true;
}

/**
 *    Callback function implementing Mf_sysex().
 *
 *    This function prints "SysEx" and then a stream of bytes, in hex
 *    format, to standard output.
 *
 *    Command: <code> 0xF0 </code>.
 *
 * \param leng
 *    Provides the number of bytes in the message.
 *
 * \param mess
 *    Provides a pointer to the message bytes.
 *
 * \return
 *    Returns true, always.
 */

static int
my_sysex (int leng, char * mess)
{
   prtime();
   printf("SysEx");
   prhex((unsigned char *) mess, leng);
   return true;
}

/**
 *    Callback function implementing Mf_metamisc().
 *
 *    This function prints "Meta", "0xnn" (the type of event), the length
 *    of the event as a byte, and then a stream of bytes, in hex format,
 *    to standard output.
 *
 *    Command: <code> 0xFF </code>.
 *
 *    The format of this event is roughly as follows:
 *
 *       -# Delta-time byte.  A typical value is 0x60 (96).
 *       -# Meta-event byte.  <code> 0xFF </code>.
 *       -# Event-type byte.  A 7-bit value, highest bit is 0.
 *          These range from 01 to 08 for the known text events.
 *          See the list of other values else where in this document.
 *       -# Length byte.
 *       -# Data bytes.  Delimited by the length byte.
 *
 * \param typecode
 *    Provides the type of meta event.
 *
 * \param leng
 *    Provides the number of bytes in the event.
 *
 * \param mess
 *    Provides a pointer to the event bytes.
 *
 * \return
 *    Returns true, always.
 */

static int
my_mmisc (int typecode, int leng, char * mess)
{
   prtime();
   printf("Meta 0x%02x", typecode);
   prhex((unsigned char *) mess, leng);
   return true;
}

/**
 *    Callback function implementing Mf_sqspecific().
 *
 *    This function prints "SeqSpec", the size of the event as a byte, and
 *    then a stream of bytes, in hex format, to standard output.
 *
 *    Command: <code> 0xFF 0x7F ... </code>.
 *
 *    The format of this event is roughly as follows:
 *
 *       -# Delta-time byte.  A typical value is 0x60 (96).
 *       -# Meta-event byte.  <code> 0xFF </code>.
 *       -# Event-type byte.  <code> 0x7F </code>.
 *       -# Length byte.
 *       -# Data bytes.  Delimited by the length byte.
 *
 * \param leng
 *    Provides the number of bytes in the event.
 *
 * \param mess
 *    Provides a pointer to the event bytes.
 *
 * \return
 *    Returns true, always.
 */

static int
my_mspecial (int leng, char * mess)
{
   prtime();
   printf("SeqSpec");
   prhex((unsigned char *) mess, leng);
   return true;
}

/**
 *    Callback function implementing Mf_text().
 *
 *    This function prints "SeqSpec", the size of the event as a byte, and
 *    then a stream of bytes, in hex format, to standard output.
 *
 *    Command: <code> 0xFF </code>.
 *
 *    The format of this event is roughly as follows:
 *
 *       -# Delta-time byte.  A typical value is 0x60 (96).
 *       -# Meta-event byte.  <code> 0xFF </code>.
 *       -# Event-type byte.  <code> 0xnn </code>.
 *       -# Length byte.
 *       -# Data bytes.  Delimited by the length byte.
 *
 * \param type
 *    Here, nn ranges from 01 to 0F.  01 to 08 are the known types of text
 *    event, while 09 to 0F are unrecognized text events.
 *    Other values are treat as in my_mmisc().
 *
 * \param leng
 *    Provides the number of bytes in the event.
 *
 * \param mess
 *    Provides a pointer to the event bytes.
 *
 * \return
 *    Returns true, always.
 */

static int
my_mtext (int type, int leng, char * mess)
{
   static char * ttype [] =
   {
        nullptr,
        "Text",                        /* type == 0x01         */
        "Copyright",                   /* type == 0x02         */
        "TrkName",                     /* type == 0x03         */
        "InstrName",                   /* type == 0x04         */
        "Lyric",                       /* type == 0x05         */
        "Marker",                      /* type == 0x06         */
        "Cue",                         /* type == 0x07         */
        "Unrec"                        /* type unrecognizedi   */
   };
   int unrecognized = sizeof(ttype) / sizeof(char *) - 1;
   prtime();
   if (type < 1 || type > unrecognized)
      printf("Meta 0x%02x ", type);
   else if (type == 3 && g_status_track_number == 1)
      printf("Meta SeqName ");
   else
      printf("Meta %s ", ttype[type]);

   prtext((unsigned char *) mess, leng);
   return true;
}

/**
 *    Callback function implementing Mf_seqnum().
 *
 *    This function writes "SeqNr" and the sequence number to standard
 *    output.
 *
 *    Command: <code> FF 00 02 </code>.
 *
 *    The format of this event is roughly as follows:
 *
 *       -# Delta-time byte.  A typical value is 0x60 (96).
 *       -# Meta-event byte.  <code> 0xFF </code>.
 *       -# Sequence-number marker.  <code> 0x00 </code>.
 *       -# Length byte.  <code> 0x02 </code>.
 *       -# Data bytes.  Two bytes of data, I believe, at this time.  As
 *          for ordering
 *
 * \param num
 *    Provides the number to be converted to a sequence-number
 *    specification.
 *
 * \return
 *    Returns true, always.
 */

static int
my_mseq (short int num)
{
   prtime();
   printf("SeqNr %d\n", num);
   return true;
}

/**
 *    Callback function implementing Mf_eot().
 *
 *    This function writes "Mata TrkEnd" to standard output.
 *
 *    Command: <code> FF @F 00 </code>.
 *
 *    The format of this event is:
 *
 *       -# Delta-time byte.  A typical value is 0x60 (96).
 *       -# Meta-event byte.  <code> 0xFF </code>.
 *       -# End-of-track marker.  <code> 0x0F </code>.
 *       -# Length byte.  <code> 0x00 </code>.
 *
 *    There is no data for this command, so the length byte is 0.
 *
 * \return
 *    Returns true, always.
 */

static int
my_meot (void)
{
   prtime();
   printf("Meta TrkEnd\n");
   return true;
}

/**
 *    Callback function implementing Mf_keysig().
 *
 *    This function prints "KeySig", followed by the sf parameter,
 *    followed by "major" or "minor", to standard output.
 *
 *    Command: <code> FF 59 02 sf mi </code>.
 *
 *    The format of this event is:
 *
 *       -# Delta-time byte.  A typical value is 0x60 (96).
 *       -# Meta-event byte.  <code> 0xFF </code>.
 *       -# End-of-track marker.  <code> 0x59 </code>.
 *       -# Length byte.  <code> 0x02 </code>.
 *       -# Data bytes:
 *          -# sf = signature flats
 *             -  -7: 7 flats
 *             -  -1: 7 flat
 *             -   0: Key of C
 *             -   1: 1 sharp
 *             -   7: 7 sharps
 *          -# mi = minor flag (0 = major; 1 = minor)
 *
 * \param sf
 *    Provides the code for the keys above and below C. (I don't really
 *    understand what that mean, right now.)
 *
 * \param mi
 *    True (1) indicates the key is minor; otherwise it is major.
 *
 * \return
 *    Returns true, always.
 */

static int
my_keysig (int sf, int mi)
{
   prtime();
   printf
   (
      "KeySig %d %s\n",
      sf > 127 ? (sf-256) : sf,
      mi ? "minor" : "major"
   );
   return true;
}

/**
 *    Callback function implementing Mf_tempo().
 *
 *    Prints the string "Tempo" followed by the tempo value, to standard
 *    output.
 *
 *    Command: <code> FF 51 03 tt tt tt</code>.
 *
 *       -# Delta-time byte.  A typical value is 0x60 (96).
 *       -# Meta-event byte.  <code> 0xFF </code>.
 *       -# End-of-track marker.  <code> 0x51 </code>.
 *       -# Length byte.  <code> 0x03 </code>.
 *       -# Data bytes.  There are three data bytes that specify the tempo
 *          in "microseconds per MIDI quarter-note" or "24ths of a
 *          microsecond per MIDI clock".  Chew on this conversion:
 *          07 a1 20 == 500000 usec/quarter note == 1/2 second per quarter
 *          note == 120 bpm (beats per minute), where each quarter note is
 *          a beat.
 *
 * \param tempo
 *    Provides the tempo value as a single integer.
 *
 * \return
 *    Returns true, always.
 */

static int
my_tempo (long tempo)
{
   prtime();
   printf("Tempo %ld\n", tempo);
   return true;
}

/**
 *    Callback function implementing Mf_timesig().
 *
 *    Prints the string "TimeSig" followed by the four value needed for a
 *    key signature, to standard output.
 *
 *    Command: <code> FF 58 04 nn dd cc bb</code>.
 *
 *    As an example, this sequence of bytes:
 *
 *    FF 58 04 06 03 24 08
 *
 *    This is the complete event for 6/8 time, where the metronome clicks
 *    every three eighth-notes, but there are 24 clocks per quarter-note,
 *    72 to the bar.  That is, 6/8 time (8 is 2 to the 3rd power, so this
 *    is 06 03), 36 MIDI clocks per dotted-quarter (24 hex!), and eight
 *    notated 32nd-notes per quarter-note.
 *
 * \param nn
 *    Provides the numerator of the time signature as it would be notated.
 *
 * \param dd
 *    Provides the denominator of the time signature as it would be
 *    notated.  The denominator is a negative power of two, where 2
 *    represents a quarter note, 3 represents an eighth note....
 *
 * \param cc
 *    Provides the number of MIDI clock in a metronome click.
 *
 * \param bb
 *    The number of notated 32nd notes in a MIDI quarter-note (a MIDI
 *    quarter note is 24 MIDI clocks).
 *
 * \return
 *    Returns true, always.
 */

static int
my_timesig (int nn, int dd, int cc, int bb)
{
   int denom = 1;
   while (dd-- > 0)
      denom *= 2;

   prtime();
   printf("TimeSig %d/%d %d %d\n", nn, denom, cc, bb);
   g_status_M0 += (Mf_currtime-g_status_T0) / (g_status_beat*g_status_measure);
   g_status_T0 = Mf_currtime;
   g_status_measure = nn;
   g_status_beat = 4 * g_status_clicks / denom;
   return true;
}

/**
 *    Callback function implementing Mf_smpte().
 *
 *    Prints "SMPTE" and each of the parameters shown below, to standard
 *    output.
 *
 * \param hr
 *    Provides the hour of the SMPTE time at which the track chunk is
 *    supposed to start.  It should be present at the beginning of the
 *    track, before any non-zero delta times, and before any transmitable
 *    MIDI events.
 *
 * \param mn
 *    Provides the minutes of the SMPTE time.
 *
 * \param se
 *    Provides the seconds of the SMPTE time.
 *
 * \param fr
 *    Provides the frames of the SMPTE time.
 *
 * \param ff
 *    Provides the fractional frames of the SMPTE time, in 100ths of a
 *    frame, even in SMPTE-based tracks which specify as different frame
 *    subdivision for delta times..
 *
 * \return
 *    Returns true, always.
 */

static int
my_smpte (int hr, int mn, int se, int fr, int ff)
{
   prtime();
   printf("SMPTE %d %d %d %d %d\n", hr, mn, se, fr, ff);
   return true;
}

/**
 *    Callback function implementing Mf_arbitrary().
 *
 *    This function is called in readtrack() if there are additional bytes
 *    that are not part of a system exlusive continuation.
 *
 *    I have to confess that, right now, I don't know what that means.
 *
 *    This function writes the time, "Arb", followed by the message bytes,
 *    followed by the length.  Why doesn't the length come first?
 *
 * \param leng
 *    Provides the number of bytes in the message.
 *
 * \param mess
 *    Points to the bytes in the message.
 *
 * \return
 *    Returns true, always.
 */

static int
my_arbitrary (int leng, char * mess)
{
   prtime();
   printf("Arb");                         /* printf("Arb", leng);    */
   prhex((unsigned char *) mess, leng);   /* THIS SEEMS LIKE A BUG   */
   return true;
}

/**
 *    Callback function implementing Mf_error().
 *
 *    This function checks for garbage at the end of the MIDI file.  If it
 *    does not find such garbage, then the error is simply printed.
 *
 * \param s
 *    Provides the error string to report.
 *
 * \return
 *    Returns true, always.
 */

static int
my_error (const char * s)
{
   if (g_status_tracks_to_do <= 0)
      fprintf(stderr, "Error: Garbage at end '%s'\n", s);
   else
      fprintf(stderr, "Error: %s\n", s);

   return true;
}

/**
 *    Callback function implementing Mf_wtrack() and Mf_wtempotrack().
 *
 *    This function is called for writing MIDI to text and for text to
 *    MIDI:
 *
\verbatim
   Mf_wtrack         = my_writetrack;     // text-to-MIDI and vice versa
   Mf_wtempotrack    = my_writetrack;     // text-to-MIDI only
\endverbatim
 *
 *    However, this function also <i> reads </i> data!!!
 *    This function scans the return value of the flex function yylex()
 *    until it finds it is equal to MTRK (which happens if the string
 *    "MTrk" is found).  A bunch of other markers ("TrkEnd", "SysEx",
 *    "Meta", "Arb", and more (see t2mf.fl), are found and processed.
 *
 *    For writing MIDI, the following functions from the midifilex.c
 *    module are called:
 *
 *       -  mf_w_midi_event().  Writes the "delta time", the MIDI event
 *          type plus channel byte, plus the event data.,
 *       -  mf_w_sysex_event().  Writes the delta time and the SYSEX or
 *          "arbitrary" data for the event.
 *       -  mf_w_meta_event().  Writes the delta time and the meta-event.
 *          There are a number of meta-events that are written.
 *
 * \return
 *    Returns -1 upon an error, otherwise returns true (1).
 */

static int
my_writetrack (void)
{
   int opcode, c;
   long currtime = 0;
   long newtime, delta;
   int i, k;
   while ((opcode = yylex()) == EOL)
      ;

   if (opcode != MTRK)
      prs_error("Missing MTrk");

   checkeol();
   for (;;)
   {
      g_status_err_cont = 1;
      setjmp(g_status_erjump);
      switch (yylex())
      {
      case MTRK:

         prs_error("Unexpected MTrk");
         return -1;                    /* new ca 2014-04-11 */

       case EOF:

         g_status_err_cont = 0;
         error("Unexpected EOF");
         return -1;

       case TRKEND:

         g_status_err_cont = 0;
         checkeol();
         return true;

       case INT:

         newtime = yyval;
         if ((opcode = yylex()) == '/')
         {
            if (yylex() != INT)
               prs_error("Illegal time value");

            newtime = (newtime-g_status_M0) * g_status_measure + yyval;
            if (yylex() != '/' || yylex() != INT)
               prs_error("Illegal time value");

            newtime = g_status_T0 + newtime * g_status_beat + yyval;
            opcode = yylex();
         }
         delta = newtime - currtime;
         switch (opcode)
         {
         case ON:
         case OFF:
         case POPR:

            checkchan();
            checknote();
            checkval();
            mf_w_midi_event(delta, opcode, gs_chan, gs_data, 2UL);
            break;

         case PAR:

            checkchan();
            checkcon();
            checkval();
            mf_w_midi_event(delta, opcode, gs_chan, gs_data, 2UL);
            break;

         case PB:

            checkchan();
            splitval();
            mf_w_midi_event(delta, opcode, gs_chan, gs_data, 2UL);
            break;

         case PRCH:

            checkchan();
            checkprog();
            mf_w_midi_event(delta, opcode, gs_chan, gs_data, 1UL);
            break;

         case CHPR:

            checkchan();
            checkval();
            gs_data[0] = gs_data[1];
            mf_w_midi_event(delta, opcode, gs_chan, gs_data, 1UL);
            break;

         case SYSEX:
         case ARB:

            gethex();
            mf_w_sysex_event(delta, g_status_buffer, (long) g_status_buflen);
            break;

         case TEMPO:

            if (yylex() != INT)
               syntax();

            mf_w_tempo(delta, yyval);
            break;

         case TIMESIG:
         {
            int nn, denom, cc, bb;
            if (yylex() != INT || yylex() != '/')
               syntax();

            nn = yyval;
            denom = getbyte("Denom");
            cc = getbyte("clocks per click");
            bb = getbyte("32nd notes per 24 clocks");
            for (i = 0,  k = 1; k < denom; i++, k <<= 1)
               ;

            if (k != denom)
               error("Illegal TimeSig");

            gs_data[0] = nn;
            gs_data[1] = i;
            gs_data[2] = cc;
            gs_data[3] = bb;
            g_status_M0 += (newtime - g_status_T0) /
               (g_status_beat * g_status_measure);

            g_status_T0 = newtime;
            g_status_measure = nn;
            g_status_beat = 4 * g_status_clicks / denom;
            mf_w_meta_event(delta, time_signature, gs_data, 4UL);
         }
            break;

         case SMPTE:

            for (i = 0; i < 5; i++)
               gs_data[i] = getbyte("SMPTE");

            mf_w_meta_event(delta, smpte_offset, gs_data, 5UL);
            break;

         case KEYSIG:

            gs_data[0] = i = getint("Keysig");
            if (i < -7 || i > 7)
                error("Key Sig must be between -7 and 7");

            if ((c = yylex()) != MINOR && c != MAJOR)
                syntax();

            gs_data[1] = (c == MINOR);
            mf_w_meta_event(delta, key_signature, gs_data, 2UL);
            break;

         case SEQNR:

            /*
             * I think there is a bug here.  It would seem logical that
             * "SeqNr" would have to be used to get the value, but neither
             * this version nor midi2text's uses it!
             */

            get16val();                /* get16val("SeqNr"); */
            mf_w_meta_event(delta, sequence_number, gs_data, 2UL);
            break;

         case META:
         {
            int type = yylex();
            switch (type)
            {
            case TRKEND:

               type = end_of_track;
               break;

            case TEXT:
            case COPYRIGHT:
            case SEQNAME:
            case INSTRNAME:
            case LYRIC:
            case MARKER:
            case CUE:

               type -= META + 1;
               break;

            case INT:

               type = yyval;
               break;

            default:

               prs_error("Illegal Meta type");
               break;
            }
            if (type == end_of_track)
               g_status_buflen = 0;
            else
               gethex();

            mf_w_meta_event(delta, type, g_status_buffer, (long) g_status_buflen);
            break;
         }
         case SEQSPEC:

            /*
             * \note
             *    sequencer_specific is a macro from libmidifilex.
             */

            gethex();
            mf_w_meta_event
            (
               delta, sequencer_specific,  g_status_buffer,
               (long) g_status_buflen
            );
            break;

         default:

            prs_error("Unknown input");
             break;
          }
          currtime = newtime;

      case EOL:

          break;

       default:

          prs_error("Unknown input");
          break;
      }
      checkeol();
   }
   return true;
}

/**
 *    Makes the function assignments needed by the midifile library when
 *    converting a text file to MIDI.
 */

void
midicvt_initfuncs_t2mf (void)
{
   Mf_error          = my_error;
   Mf_putc           = fileputc;
   Mf_wtrack         = my_writetrack;
   Mf_wtempotrack    = my_writetrack;
}

/**
 *    Makes the function assignments needed by the midifile library when
 *    converting a MIDI file to text.
 */

void
midicvt_initfuncs_mf2t (void)
{
   Mf_error          = my_error;
   Mf_header         = my_header;
   Mf_starttrack     = my_trstart;
   Mf_endtrack       = my_trend;
   Mf_on             = my_non;
   Mf_off            = my_noff;
   Mf_pressure       = my_pressure;
   Mf_parameter      = my_parameter;
   Mf_pitchbend      = my_pitchbend;
   Mf_program        = my_program;
   Mf_chanpressure   = my_chanpressure;
   Mf_sysex          = my_sysex;
   Mf_metamisc       = my_mmisc;
   Mf_seqnum         = my_mseq;
   Mf_eot            = my_meot;
   Mf_timesig        = my_timesig;
   Mf_smpte          = my_smpte;
   Mf_tempo          = my_tempo;
   Mf_keysig         = my_keysig;
   Mf_sqspecific     = my_mspecial;
   Mf_text           = my_mtext;
   Mf_arbitrary      = my_arbitrary;

   /*
    * Moved from main() to here.
    */

   Mf_getc           = filegetc;
   Mf_putc           = fileputc;
   Mf_wtrack         = my_writetrack;

   /*
    * Unassigned functions:
    *
    *    Mf_wtempotrack
    */
}

/**
 *    Outputs error messages to standard error.
 *
 *    This function also skips thto the end of the line of the input.  If
 *    the end-of-file is encountered, then exit(1) is called.  If there is
 *    an error status, then a longjmp() to g_status_erjump is called.
 *
 * \param s
 *    Provides the error string.  Note that yyleng and yytext are global
 *    inputs to this function, as well.
 *
 * \return
 *    Returns true, always.
 */

static int
prs_error (char * s)
{
   int c;
   int count;
   int ln = eol_seen ? lineno-1 : lineno;
   fprintf(stderr, "%d: %s\n", ln, s);
   if (yyleng > 0 && *yytext != '\n')
      fprintf(stderr, "*** %*s ***\n", (int) yyleng, yytext);

   count = 0;
   while (count < 100 && (c = yylex()) != EOL && c != EOF)
   {
      count++;                         /* skip rest of line */
   }
   if (c == EOF)
      exit(1);

   if (g_status_err_cont)
      longjmp(g_status_erjump, 1);

   return true;
}

/**
 *    Calls psr_error() with a message of "Syntax error".
 */

static void
syntax (void)
{
   prs_error("Syntax error");
}

/**
 *    This function makes sure the "MFile" or (new) "MThd" token is found.
 *    It then gathers up some status information and passes it to
 *    mfwrite().
 *
 * \note
 *    This function used to be called translate(), which was a bit
 *    ambiguous.
 */

void
midicvt_compile (void)
{
   if (yylex() == MTHD)    /* true if "MFile" or (new) "MThd" is found */
   {
      /*
       * Do not change "MFile" to "MThd" here unless you're willing to
       * risk an extended debug session.
       */

      g_status_format = getint("MFile format");
      g_status_no_of_tracks = getint("MFile #tracks");
      g_status_clicks = getint("MFile Clicks");
      if (g_status_clicks < 0)
         g_status_clicks = (g_status_clicks & 0xff) << 8 |
            getint("MFile SMPTE division");

      checkeol();
      mfwrite(g_status_format, g_status_no_of_tracks, g_status_clicks, g_io_file);
   }
   else
   {
      fprintf(stderr, "Missing MFile/MTrk token in ASCII file, can't continue\n");
      exit(1);
   }
}

/**
 *    Gets the current byte value as contained in yyval.
 *
 * \param mess
 *    The message to print if yyval is greater than 127.
 *
 * \return
 *    The value of yyval is returned.  It is set to 0 if an error
 *    occurred.
 */

static int
getbyte (char * mess)
{
   (void) getint(mess);

   /*
    * Now unsigned: if (yyval < 0 || yyval > 127)
    */

   if (yyval > 127)
   {
      char ermesg[128];
      (void) snprintf
      (
         ermesg, sizeof(ermesg), "Wrong value (%ld) for %s", yyval, mess
      );
      error(ermesg);
      yyval = 0;
   }
   return yyval;
}

/**
 *    Returns  the current byte value as contained in yyval.
 *
 * \param mess
 *    The message to print if yylex() is not equal to INT.
 *
 * \return
 *    The value of yyval is returned.  It is set to 0 if an error
 *    occurred.
 */

static int
getint (char * mess)
{
   if (yylex() != INT)
   {
      char ermesg[128];
      (void) snprintf(ermesg, sizeof(ermesg), "Integer expected for %s", mess);
      error(ermesg);
      yyval = 0;
   }
   return yyval;
}

/**
 *    Checks to make sure that yylex() == CH or yylex() == INT, and
 *    yyval >= 1 or yyval  16.
 *
 * \sideeffect
 *    The global variable gs_chan is set to yyval - 1.
 */

static void
checkchan (void)
{
   if (yylex() != CH || yylex() != INT)
      syntax();                        /* calls prs_error("Syntax error")  */

   if (yyval < 1 || yyval > 16)
      error("Chan must be between 1 and 16");

   gs_chan = yyval - 1;
}

/**
 *    Checks the incoming value from flex.
 *
 */

static void
checknote (void)
{
   int c;
   if (yylex() != NOTE || ((c = yylex()) != INT && c != NOTEVAL))
      syntax();

   if (c == NOTEVAL)
   {
      static int notes [] =
      {
         9,    /* a */
         11,   /* b */
         0,    /* c */
         2,    /* d */
         4,    /* e */
         5,    /* f */
         7     /* g */
      };
      char * p = yytext;               /* g_t2mf_yytext; */
      c = *p++;
      if (isupper(c))
         c = tolower(c);

      yyval = notes[c-'a'];
      switch(*p)
      {
      case '#':
      case '+':

         yyval++;
         p++;
         break;

      case 'b':
      case 'B':
      case '-':

         yyval--;
         p++;
         break;
      }
      yyval += 12 * atoi(p);
   }
   if (yyval > 127)                    /* yyval is now unsigned */
      error("Note must be between 0 and 127");

   gs_data[0] = yyval;
}

/**
 *
 */

static void
checkval (void)
{
  if (yylex() != VAL || yylex() != INT)
     syntax();

  if (yyval > 127)                     /* yyval is now unsigned */
     error("Value must be between 0 and 127");

  gs_data[1] = yyval;
}

/**
 *
 */

static void
splitval (void)
{
   if (yylex() != VAL || yylex() != INT)
      syntax();

   if (yyval > 16383)                  /* yyval is now unsigned */
      error("Value must be between 0 and 16383");

   gs_data[0] = yyval % 128;
   gs_data[1] = yyval / 128;
}

/**
 *
 * \question
 *    Does this function need to have and use a char pointer???
 */

static void
get16val (void)
{
   if (yylex() != VAL || yylex() != INT)
      syntax();

   if (yyval > 65535)                  /* yyval is now unsigned */
      error("Value must be between 0 and 65535");

   gs_data[0] = (yyval >> 8) & 0xff;
   gs_data[1] = yyval & 0xff;
}

/**
 *
 */

static void
checkcon (void)
{
   if (yylex() != CON || yylex() != INT)
      syntax();

   if (yyval > 127)                    /* yyval is now unsigned */
      error("Controller must be between 0 and 127");

   gs_data[0] = yyval;
}

/**
 *
 */

static void
checkprog (void)
{
   if (yylex() != PROG || yylex() != INT)
      syntax();

   /*
    * Now unsigned: if (yyval < 0 || yyval > 127)
    */

   if (yyval > 127)
      error("Program number must be between 0 and 127");

   gs_data[0] = yyval;
}

/**
 *    Check for the end-of-line.
 * \internal
 */

static void
checkeol (void)
{
   if (eol_seen)
      return;

   if (yylex() != EOL)
   {
      prs_error("Garbage deleted");
      while (! eol_seen)
         yylex();
   }
}

/**
 *
 */

static void
gethex (void)
{
   int c;
   unsigned int * cptr = (unsigned int *) &c;      /* tricky code */
   g_status_buflen = 0;
   do_hex = 1;
   c = yylex();
   if (c == STRING)
   {
      int i = 0;
      if ((int) yyleng - 1 > g_status_bufsiz)
      {
         g_status_bufsiz = (int) (yyleng - 1);
         if (g_status_buffer)
            g_status_buffer = realloc(g_status_buffer, g_status_bufsiz);
         else
            g_status_buffer = malloc(g_status_bufsiz);

         if (! g_status_buffer)
            error("Out of memory");
      }
      while (i < (int) yyleng - 1)
      {
         c = yytext[i++];

rescan:
         if (c == '\\')
         {
            switch (c = yytext[i++])
            {
            case '0':

               c = '\0';
               break;

            case 'n':

               c = '\n';
               break;

            case 'r':

               c = '\r';
               break;

            case 't':

               c = '\t';
               break;

            case 'x':

               /*
                * Note use of cptr for instead of "&c".
                */

               if (sscanf(yytext+i, "%2x", cptr) != 1)
                  prs_error("Illegal \\x in string");

               i += 2;
               break;

            case '\r':
            case '\n':

               while
               (
                  (c = yytext[i++]) == ' ' ||
                  c == '\t' || c == '\r' || c == '\n'
               )
               {
                  /* skip whitespace */
               }
               goto rescan;            /* apologies to EWD? */
            }
         }
         g_status_buffer[g_status_buflen++] = c;
      }
   }
   else if (c == INT)
   {
      do
      {
         if (g_status_buflen >= g_status_bufsiz)
         {
            g_status_bufsiz += 128;
            if (g_status_buffer)
                g_status_buffer = realloc(g_status_buffer, g_status_bufsiz);
            else
               g_status_buffer = malloc(g_status_bufsiz);

            if (! g_status_buffer)
               error("Out of memory");
         }
         g_status_buffer[g_status_buflen++] = yyval;
         c = yylex();

      } while (c == INT);

      if (c != EOL)
         prs_error("Unknown hex input");
   }
   else
      prs_error("String or hex input expected");
}

/**
 *
 */

long
bankno (char * s, int n)
{
   long res = 0;
   int c;
   while (n-- > 0)
   {
      c = (*s++);
      if (islower(c))
         c -= 'a';
      else if (isupper(c))
         c -= 'A';
      else
         c -= '1';

      res = res * 8 + c;
   }
   return res;
}

/**
 *
 */

FILE *
efopen (const char * name, const char * mode)
{
   FILE * f = fopen(name, mode);
   if (midicvt_option_debug())
      fprintf(stderr, "efopen(%s, %s)\n", name, mode);

   if (is_nullptr(f))
      (void) fprintf(stderr, "Cannot open '%s',  %s!\n", name, strerror(errno));

   return f;
}

/**
 *    Callback function implementing Mf_putc().
 *
 */

static int
fileputc (unsigned char c)
{
   return putc((int) c, g_io_file);    /* FILE * F from the t2mf.c module     */
}

/**
 *    Callback function implementing Mf_getc().  This function increments
 *    the offset into the MIDI file, and then calls getc(g_io_file).
 *
 * \return
 *    Returns the value returned by getc().
 */

static int
filegetc (void)
{
   midi_file_offset_increment();
   return getc(g_io_file);
}

/**
 *    Redirects stdout to a file.
 *
 * \param filename
 *    Provides the filename to be use for the out that would have gone to
 *    stdout.
 *
 * \param mode
 *    Provides the mode to use on the file.  Probably makes sense only in
 *    Windows, which this application doesn't support yet.  Should be
 *    either "w" or "wb", but this is not verified.  You're a big boy.
 *
 * \return
 *    Returns true (1) if the setup succeeded.  Do not rely on using stdout
 *    in the rest of the program if this function returns false (0).
 */

static cbool_t
redirect_stdout (const char * filename, const char * mode)
{
   int rc = fflush(stdout);
   cbool_t result = rc == 0;
   if (result)
   {
      rc = fgetpos(stdout, &gs_saved_stdout_pos);  /* fails, 'Illegal seek' */
      result = true;
      if (result)
      {
         gs_saved_stdout = dup(fileno(stdout));
         result = gs_saved_stdout != (-1);
         if (result)
         {
            g_redirect_file = freopen(filename, mode, stdout);
            result = not_nullptr(g_redirect_file);
            if (! result)
               error("redirect_stdout(): failed to freopen() stdout");
         }
         else
            error("redirect_stdout(): failed to dup() stdout");
      }
      else
      {
         error("redirect_stdout(): failed to get the file-position of stdout:");
         fprintf(stderr, "error is '%s'\n", strerror(errno));
      }
   }
   else
      error("redirect_stdout(): failed to fflush() stdout for redirection");

   return result;
}

/**
 *    Handles the file descriptor for stdout that was saved for later
 *    restoration.
 *
 *    To avoid a memory leak at exit(), this function closes
 *    g_redirect_file if it was assigned a value [i.e. fileno(stdio)].
 *
 * \warning
 *    Currently, the dup2() call is returning error 9, EBADF, a bad file
 *    descriptor.  We're guessing it is not needed anyway, and commenting
 *    it out.
 */

static cbool_t
revert_stdout ()
{
   cbool_t result = fflush(stdout) == 0;
   if (not_nullptr(g_redirect_file))
   {
      (void) fclose(g_redirect_file);     /* \change ca 2015-08-21 */
      g_redirect_file = nullptr;
   }
   if (gs_saved_stdout != (-1))
   {
      /*
       * See the warning in the function banner.
       *
       * int rcode = dup2(gs_saved_stdout, fileno(stdout));
       * if (rcode == (-1))
       * {
       *    errprintf("revert_stdout(): dup2() failed, errno = %d\n", errno);
       * }
       */

      int rcode = close(gs_saved_stdout);
      if (rcode == (-1))
      {
         errprintf("revert_stdout(): close() failed, errno = %d\n", errno);
      }
      clearerr(stdout);
      if (fsetpos(stdout, &gs_saved_stdout_pos) != 0)
      {
         /*
          * Right now, things seem to work without the saving and
          * restoring of the position of stdout.  We'll let it pass for
          * now.
          *
          * error("Failed to restore the file-position of stdout");
          */

         result = false;
      }
   }
   else
      result = false;

   return result;
}

/**
 *    Sets up the incoming text file for compiling into MIDI.
 *
 * \return
 *    Returns true if all is well.
 */

cbool_t
midicvt_setup_compile (void)
{
   cbool_t result;
   if (midicvt_have_input_file())
      yyin = efopen(midicvt_input_file(), "r");
   else
   {
      yyin = fdopen(fileno(stdin), "r");
      (void) midicvt_set_input_file("stdin");
   }
   result = not_nullptr(yyin);
   if (result)
   {
      if (midicvt_have_output_file())
         g_io_file = efopen(midicvt_output_file(), "wb");
      else
      {
         g_io_file = fdopen(fileno(stdout), "wb");
         (void) midicvt_set_output_file("stdout");
      }
      result = not_nullptr(g_io_file);
      if (result)
      {
         if (midicvt_option_debug())
         {
            fprintf
            (
               stderr, "Compiling %s to %s...\n",
               midicvt_input_file(), midicvt_output_file()
            );
         }
      }
      else
      {
         errprint("midicvt_setup_compile(): could not set up output MIDI file");
      }
   }
   else
   {
      errprint("midicvt_setup_compile(): could not set up the input ASCII file");
   }
   return result;
}

/**
 *    Closes the input file (yyin) and the output file (g_io_file) for the
 *    --compile option.
 */

void
midicvt_close_compile (void)
{
   if (midicvt_have_input_file())
      fclose(yyin);
   else if (not_nullptr(yyin))
      fclose(yyin);

   yyin = nullptr;
   if (midicvt_have_output_file())
      fclose(g_io_file);
   else if (not_nullptr(g_io_file))
      fclose(g_io_file);

   g_io_file = nullptr;
   if (not_nullptr(g_status_buffer))
   {
      free(g_status_buffer);
      g_status_buffer = nullptr;
   }
   (void) yylex_destroy();             /* new ca 2015-08-22 */
}

/**
 *    Sets up the file handles for both reading and writing.
 *
 * \return
 *    Returns true if all steps succeeded.
 */

cbool_t
midicvt_setup_mfread (void)
{
   cbool_t result;
   if (midicvt_have_input_file())
      g_io_file = efopen(midicvt_input_file(), "rb");
   else
   {
      g_io_file = fdopen(fileno(stdin), "rb");
      (void) midicvt_set_input_file("stdin");
   }
   result = not_nullptr(g_io_file);
   if (result)
   {
      if (midicvt_have_output_file())
      {
         cbool_t ok = redirect_stdout(midicvt_output_file(), "w");
         if (! ok)
         {
            errprint
            (
               "midicvt_setup_mfread(): could not redirect stdout to output file"
            );
            result = false;
         }
      }
      else
      {
         /*
          * Hmmm, this results in a leak at exit time, in fdopen().
          */

         g_redirect_file = fdopen(fileno(stdout), "wb");
         if (not_nullptr(g_redirect_file))
            (void) midicvt_set_output_file("stdout");
         else
         {
            /*
             * If fdopen() returns null, one is supposed to call close()
             * with the first parameter.  But do we want to close
             * fileno(stdout)?  Well, we tried, but still get a leak at
             * exit:  close(fileno(stdout));
             */

            errprint("midicvt_setup_mfread(): could not fdopen(stdout)");
            result = false;
         }
      }
      if (result)
      {
         if (midicvt_option_verbose())
         {
#ifdef USE_MIDICOMP_VERBOSE_FORMAT
             g_option_Onmsg   = "On      ch=%-2d  note=%-3s  vol=%-3d\n";
             g_option_Offmsg  = "Off     ch=%-2d  note=%-3s  vol=%-3d\n";
             g_option_PoPrmsg = "PolyPr  ch=%-2d  note=%-3s  val=%-3d\n";
             g_option_Parmsg  = "Param   ch=%-2d  con=%-3d   val=%-3d\n";
             g_option_Pbmsg   = "Pb      ch=%-2d  val=%-3d\n";
             g_option_PrChmsg = "ProgCh  ch=%-2d  prog=%-3d\n";
             g_option_ChPrmsg = "ChanPr  ch=%-2d  val=%-3d\n";
#elif USE_MIDI2TEXT_DEFAULT_FORMAT

             /*
              * Since the midi2text project provided many more examples
              * for testing, we will use its uglier format.
              */

             g_option_Onmsg   = "On ch=%d n=%s v=%d\n";
             g_option_Offmsg  = "Off ch=%d n=%s v=%d\n";
             g_option_PoPrmsg = "PoPr ch=%d n=%s v=%d\n";
             g_option_Parmsg  = "Par ch=%d c=%d v=%d\n";
             g_option_Pbmsg   = "Pb ch=%d v=%d\n";
             g_option_PrChmsg = "PrCh ch=%d p=%d\n";
             g_option_ChPrmsg = "ChPr ch=%d v=%d\n";
#else
             g_option_Onmsg   = "On ch=%d note=%s vol=%d\n";
             g_option_Offmsg  = "Off ch=%d note=%s vol=%d\n";
             g_option_PoPrmsg = "PolyPr ch=%d note=%s val=%d\n";
             g_option_Parmsg  = "Param ch=%d con=%d val=%d\n";
             g_option_Pbmsg   = "Pb ch=%d val=%d\n";
             g_option_PrChmsg = "ProgCh ch=%d prog=%d\n";
             g_option_ChPrmsg = "ChanPr ch=%d val=%d\n";
#endif
         }
         if (midicvt_option_debug())
         {
            fprintf
            (
               stderr, "Converting %s to %s...\n",
               midicvt_input_file(), midicvt_output_file()
            );
         }
      }
   }
   else
   {
      errprint
      (
         "midicvt_setup_mfread(): could not set up input MIDI file for reading"
      );
   }
   return result;
}

/**
 *    Checks g_io_file for an error status, closes this file handle if
 *    necessary, and restores stdout to normal if necessary.
 */

void
midicvt_close_mfread (void)
{
   if (ferror(g_io_file))
      error("Input file error");

   if (midicvt_have_input_file())
      fclose(g_io_file);

   if (midicvt_have_output_file())
      (void) revert_stdout();
}

/*
 * midicvt_base.c
 *
 * vim: sw=3 ts=3 wm=8 et ft=c
 */
