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
 * \file          midicvt_m2m.c
 *
 *    This module provides functions for basic MIDI-to-MIDI conversions.
 *
 * \library       midicvt application
 * \author        Chris Ahlstrom and many other authors
 * \date          2014-04-27
 * \updates       2016-04-17
 * \version       $Revision$
 * \license       GNU GPL
 *
 *    Why do we want to convert MIDI-to-MIDI?  Well, for one thing, as we
 *    have seen with some of the test files, that conversion can fix MIDI
 *    files.
 *
 *    For another thing, we can insert transformations directly into the
 *    midicvt program, and call the functions here to write out the
 *    transformed MIDI.  This work will give users some stock
 *    functionality without having to dealing with pipes and scripts.
 *
 *    A program that transforms MIDI data will define a callback function
 *    that first transforms the MIDI data, and then calls one of the
 *    functions here to write the MIDI.
 */

#include <ctype.h>                     /* isprint(), islower()                */
#include <malloc.h>                    /* malloc() and realloc()              */
#include <errno.h>                     /* strerror() and errno                */
#include <unistd.h>                    /* UNIX/Linux only                     */
#include <stdio.h>                     /* FILE *, etc.                        */
#include <stdlib.h>                    /* exit()                              */
#include <string.h>                    /* strerror()                          */

#include <midicvt_macros.h>            /* nullptr, true, false, etc.          */
#include <midicvt_m2m.h>               /* this module's functions and stuff   */
#include <midicvt_globals.h>           /* midicvt_setup_compile()             */
#include <midicvt_helpers.h>           /* midicvt_input_file(), output_file() */
#include <midifilex.h>                 /* routines that read/write MIDI data  */

/**
 *    Callback function implementing Mf_putc() for MIDI-to-MIDI
 *    conversions.
 *
 *    This function writes to the g_redirect_file FILE pointer.
 *
 * \param c
 *    Provides the character to be written.
 *
 * \return
 *    Returns the value returned by putc(), or a -1 upon error.
 */

static int
fileputc (unsigned char c)
{
   cbool_t ok = not_nullptr(g_redirect_file);
   if (ok)
      return putc((int) c, g_redirect_file);
   else
   {
      errprint("null redirect pointer in m2m's fileputc()");
      return (-1);
   }
}

/**
 *    Callback function implementing Mf_getc().
 *
 *    This function reads from the g_io_file FILE pointer.
 *
 * \return
 *    Returns the value returned by getc(), or a -1 upon error.
 */

static int
filegetc (void)
{
   cbool_t ok = not_nullptr(g_redirect_file);
   if (ok)
   {
      midi_file_offset_increment();
      return getc(g_io_file);
   }
   else
   {
      errprint("null redirect pointer in m2m's filegetc()");
      return (-1);
   }
}

/**
 *    Callback function implementing Mf_error().
 *
 *    This fuction checks for garbage at the end of the MIDI file.  If it
 *    does not find such garbage, then the error is simply printed.
 *
 * \param s
 *    Provides the error string to report.
 *
 * \return
 *    Returns true, always.
 */

static int
m2m_error (const char * s)
{
   if (g_status_tracks_to_do <= 0)
      fprintf(stderr, "Error: Garbage at end '%s'\n", s);
   else
      fprintf(stderr, "Error: %s\n", s);

   return true;
}

/**
 *    Callback function implementing Mf_header().
 *
 *    Provides a libmidifilex callback function to write the MIDI header
 *    data to standard output in MIDI format.
 *
 *    This function is called by readheader() in the midifilex.c module.
 *    It merely needs to pass the parameters to the mf_w_header_chunk()
 *    function defined in that module.
 *
 * \param format
 *    Provides the byte describing the format (0, 1, or 2) of the MIDI file.
 *
 * \param ntrks
 *    Provides the byte describing the number of tracks in the MIDI file.
 *    This value ranges from 1 to 65536.  SMF 0 files have 1 track, SMF 1
 *    has multiple tracks, with the first track containing song
 *    information.
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
m2m_header (int format, int ntrks, int division)
{
   /*
    * g_option_absolute_times = false; now we cannot do beats
    */

   if (format < 0 || format > 2)
   {
      fprintf(stderr, "Can't deal with format %d or missing files\n", format);
      exit(1);
   }
   else
      mf_w_header_chunk(format, ntrks, division);

   /*
    * g_status_beat = g_status_clicks = division;
    * g_status_tracks_to_do = ntrks;
    */

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
m2m_trstart (void)
{
   if (not_nullptr(g_redirect_file))
   {
      /*
       * mf_w_track_chunk(g_status_track_number, g_redirect_file, Mf_wtrack);
       */

      mf_w_track_start
      (
         g_status_track_number, g_redirect_file
      );
      g_status_track_number++;
   }
   else
      errprint("null redirect-file pointer in m2m_trstart()");

   return true;
}

/**
 *    Callback function implementing Mf_endtrack().
 *
 *    This function write "TrkEnd" and a newline to standard output, and
 *    decrements the global "tracks to do" counter.  Note that there is
 *    also a m2m_meot() function that we do not use.  That function outputs
 *    the end-of-track found in the input MIDI file, but we need to
 *    manufacturer our own end-of-track..
 *
 *    In MIDI, the end-of-track marker is three bytes, ff 2f 00.
 *
 * Oddity:
 *
 *    Some MIDI files end with a 0a byte as well.
 *
 * \param header_offset
 *    Provides the offset in the output file where the "MTrk" and track
 *    length were tentatively written.
 *
 * \param track_size
 *    Provides the track-size that libmidifile actually generated.
 *    However, note that this size should also include the end-of-track
 *    meta-event written in this function, and doesn't, so we have to add
 *    the length of that event.  We don't really need this parameter,
 *    since we have access (currently) to the global counter
 *    s_Mf_numbyteswritten.
 *
 * \return
 *    Returns true, unless the file-seeks into the stdout-redirect file
 *    fail.
 */

static int
m2m_trend (long header_offset, unsigned long track_size)
{
   int newbytes = mf_w_meta_event(Mf_currtime, end_of_track, nullptr, 0);
   long current_offset = ftell(g_redirect_file);
   --g_status_tracks_to_do;

   /*
    * It's impossible to know how long the track will be beforehand, so
    * the position of the track length data is given, to be written after
    * the chunk has been generated.
    */

   if (fseek(g_redirect_file, header_offset, 0) < 0)
   {
      m2m_error("error seeking during track header rewrite");
      return false;
   }
   else
   {
      /*
       * Adjust the track size as per the number of bytes written for the
       * end-of-track meta-event.
       */

      unsigned long track_header = MTrk;
      unsigned long full_track_size = track_size + newbytes;
      write32bit(track_header);
      write32bit(full_track_size);
      if (current_offset > 0)
         fseek(g_redirect_file, current_offset, 0);
      else
      {
         m2m_error("error seeking to end of MIDI file");
         return false;
      }
   }
   return true;
}

/**
 *    Callback function implementing Mf_on().
 *
 *    No longer static, so that it can be using in the C++ program
 *    midicvtpp.
 *
 *    Command: <code> 0x9n </code>.
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

int
m2m_non (int chan, int pitch, int vol)
{
   unsigned char s_data[2];            /* no need for static gs_data[5] */
   s_data[0] = pitch;
   s_data[1] = vol;
   (void) mf_w_midi_event(Mf_currtime, note_on, chan, s_data, 2UL);
   return true;
}

/**
 *    Callback function implementing Mf_off().
 *
 *    No longer static, so that it can be using in the C++ program
 *    midicvtpp.
 *
 *    Command: <code> 0x8n </code>.
 *
 *    Very similar to Note-On messages [see m2m_non()].
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

int
m2m_noff (int chan, int pitch, int vol)
{
   unsigned char s_data[2];            /* no need for static gs_data[5] */
   s_data[0] = pitch;
   s_data[1] = vol;
   (void) mf_w_midi_event(Mf_currtime, note_off, chan, s_data, 2UL);
   return true;
}

/**
 *    Callback function implementing Mf_pressure().
 *
 *    No longer static, so that it can be using in the C++ program
 *    midicvtpp.
 *
 *    Command: <code> 0xAn </code>.
 *
 *    Very similar to Note-On messages [see m2m_non()].
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

int
m2m_pressure (int chan, int pitch, int pressure)
{
   unsigned char s_data[2];            /* no need for static gs_data[5] */
   s_data[0] = pitch;
   s_data[1] = pressure;
   (void) mf_w_midi_event(Mf_currtime, poly_aftertouch, chan, s_data, 2UL);
   return true;
}

/**
 *    Callback function implementing Mf_parameter().
 *
 *    No longer static, so that it can be using in the C++ program
 *    midicvtpp.
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

int
m2m_parameter (int chan, int control, int value)
{
   unsigned char s_data[2];            /* no need for static gs_data[5] */
   s_data[0] = control;
   s_data[1] = value;
   (void) mf_w_midi_event(Mf_currtime, control_change, chan, s_data, 2UL);
   return true;
}

/**
 *    Callback function implementing Mf_pitchbend().
 *
 *    No longer static, so that it can be using in the C++ program
 *    midicvtpp.
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

int
m2m_pitchbend (int chan, int lsb, int msb)
{
   unsigned char s_data[2];            /* no need for static gs_data[5] */
   s_data[0] = lsb;                    /* make sure order is correct!!! */
   s_data[1] = msb;
   (void) mf_w_midi_event(Mf_currtime, pitch_wheel, chan, s_data, 2UL);
   return true;
}

/**
 *    Callback function implementing Mf_program().
 *
 *    No longer static, so that it can be using in the C++ program
 *    midicvtpp.
 *
 *    Command: <code> 0xCn pp</code>.
 *
 *    A program-change (patch change) command has the following 3 bytes:
 *
 *       -# Delta-time byte.  A typical value is 0x60 (96).
 *       -# Program-change byte.  <code> 0xCn </code>.
 *       -# Program/patch number byte <code>nn</code>.
 *          This value ranges from 0 to 127.
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

int
m2m_program (int chan, int program)
{
   unsigned char s_data[2];            /* no need for static gs_data[5] */
   s_data[0] = program;
   (void) mf_w_midi_event(Mf_currtime, program_chng, chan, s_data, 1UL);
   return true;
}

/**
 *    Callback function implementing Mf_chanpressure().
 *
 *    No longer static, so that it can be using in the C++ program
 *    midicvtpp.
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

int
m2m_chanpressure (int chan, int pressure)
{
   unsigned char s_data[5];            /* no need for static gs_data[5] */
   s_data[0] = pressure;
   (void) mf_w_midi_event(Mf_currtime, channel_aftertouch, chan, s_data, 1UL);
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
 *    The format of this event is roughly as follows:
 *
 *       -# Delta-time byte.  A typical value is 0x60 (96).
 *       -# System-exclusive byte.  <code> 0xF0 </code>.
 *       -# Manufacturer's code.  A 7-bit value, highest bit is 0.
 *       -# Data bytes. A series of 7-bit values.
 *       -# EOS (End-of-System-exclusive) byte.  <code> 0xF7 </code>.
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
m2m_sysex (int leng, char * mess)
{
   (void) mf_w_sysex_event(Mf_currtime, (unsigned char *) mess, (long) leng);
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
m2m_mmisc (int typecode, int leng, char * mess)
{
   (void) mf_w_meta_event
   (
      Mf_currtime, typecode /* ??? */, (unsigned char *) mess, (long) leng
   );
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
m2m_mspecial (int leng, char * mess)
{
   (void) mf_w_meta_event
   (
      Mf_currtime, sequencer_specific, (unsigned char *) mess, (long) leng
   );
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
 *    (void) mf_w_meta_event(Mf_currtime, text_event, mess, (long) leng);
 *
 * \param type
 *    Here, nn ranges from 01 to 0F.  01 to 08 are the known types of text
 *    event, while 09 to 0F are unrecognized text events.
 *    Other values are treat as in m2m_mmisc().
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
m2m_mtext (int type, int leng, char * mess)
{
   (void) mf_w_meta_event
   (
      Mf_currtime, type, (unsigned char *) mess, (long) leng
   );
   return true;
}

/**
 *    Callback function implementing Mf_seqnum().
 *
 *    This function writes "SeqNr" and the sequence number to standard
 *    output.
 *
 *    Command: <code> FF 00 02 ss ss</code> or <code> FF 00 00</code>.
 *
 *    The format of this event is roughly as follows:
 *
 *       -# Delta-time byte.  A typical value is 0x60 (96).
 *       -# Meta-event byte.  <code> 0xFF </code>.
 *       -# Sequence-number marker.  <code> 0x00 </code>.
 *       -# Length byte.  <code> 0x02 </code>.
 *       -# Data bytes.  Two byte of data, I believe, at this time.
 *
 * \todo
 *    Need to figure out the portable format of the sequence number bytes.
 *
 * \return
 *    Returns true, always.
 */

static int
m2m_mseq (short int num)
{
   unsigned char s_data[2];            /* no need for static gs_data[5] */
   unsigned long leng;
   if (num > 0)
   {
      s_data[0] = (num & 0xff00) >> 8; /* most significant portion      */
      s_data[1] = (num & 0x00ff);
      leng = 2UL;
   }
   else
   {
      s_data[0] = 0;
      leng = 0;
   }
   mf_w_meta_event(Mf_currtime, sequence_number, s_data, leng);
   return true;
}

/**
 *    Callback function implementing Mf_eot().
 *
 *    This function writes "Mata TrkEnd" to standard output.  However the
 *    m2m_trend() function takes care of this for the MIDI-to-MIDI
 *    conversion.
 *
 *    So this function is unused; we make it non-static (but don't expose
 *    it in the header file) just to avoid a compiler warning.
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

int
m2m_meot (void)
{
   unsigned char s_data[2];            /* no need for static gs_data[5] */
   s_data[0] = 0;
   mf_w_meta_event(Mf_currtime, end_of_track, s_data, 1UL);
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
m2m_keysig (int sf, int mi)
{
   unsigned char s_data[2];            /* no need for static gs_data[5] */
   s_data[0] = sf;
   s_data[1] = mi;
   mf_w_meta_event(Mf_currtime, key_signature, s_data, 2UL);
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
 *    Note that we don't need to use mf_w_meta_event() here, as a more
 *    specific function, mf_w_tempo() can be used.
 *
 * \param tempo
 *    Provides the tempo value as a single integer.
 *
 * \return
 *    Returns true, always.
 */

static int
m2m_tempo (long tempo)
{
   mf_w_tempo(Mf_currtime, (unsigned long) tempo);
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
 *       FF 58 04 06 03 24 08
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
m2m_timesig (int nn, int dd, int cc, int bb)
{
   unsigned char s_data[4];            /* no need for static gs_data[5] */
   s_data[0] = nn;
   s_data[1] = dd;
   s_data[2] = cc;
   s_data[3] = bb;
   mf_w_meta_event(Mf_currtime, time_signature, s_data, 4UL);
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
m2m_smpte (int hr, int mn, int se, int fr, int ff)
{
   unsigned char s_data[6];            /* no need for static gs_data[5] */
   s_data[0] = hr;
   s_data[1] = mn;
   s_data[2] = se;
   s_data[3] = fr;
   s_data[4] = ff;
   mf_w_meta_event(Mf_currtime, smpte_offset, s_data, 5UL);
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
m2m_arbitrary (int leng, char * mess)
{
   (void) mf_w_meta_event
   (
      Mf_currtime, 0x60 /* ??? */, (unsigned char *) mess, (long) leng
   );
   return true;
}

/**
 *    Makes the function assignments needed by the midifile library when
 *    converting a MIDI file to text.
 *
 *    We already handle the end of track.  See m2m_trend for an
 *    explanation.
 *
 *       Mf_eot            = m2m_meot;
 */

void
midicvt_initfuncs_m2m (void)
{
   Mf_error          = m2m_error;
   Mf_header         = m2m_header;
   Mf_starttrack     = m2m_trstart;
   Mf_endtrack       = m2m_trend;
   Mf_on             = m2m_non;
   Mf_off            = m2m_noff;
   Mf_pressure       = m2m_pressure;
   Mf_parameter      = m2m_parameter;
   Mf_pitchbend      = m2m_pitchbend;
   Mf_program        = m2m_program;
   Mf_chanpressure   = m2m_chanpressure;
   Mf_sysex          = m2m_sysex;
   Mf_metamisc       = m2m_mmisc;
   Mf_seqnum         = m2m_mseq;
   Mf_timesig        = m2m_timesig;
   Mf_smpte          = m2m_smpte;
   Mf_tempo          = m2m_tempo;
   Mf_keysig         = m2m_keysig;
   Mf_sqspecific     = m2m_mspecial;
   Mf_text           = m2m_mtext;
   Mf_arbitrary      = m2m_arbitrary;
   Mf_getc           = filegetc;
   Mf_putc           = fileputc;
}

/*
 * midicvt_m2m.c
 *
 * vim: sw=3 ts=3 wm=8 et ft=c
 */
